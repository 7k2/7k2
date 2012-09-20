/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename	 : OUN_ORD.CPP
//Description: Unit order processing functions

#include <ounitres.h>
#include <onation.h>
#include <oremote.h>
#include <oraceres.h>
#include <omonsres.h>
#include <oun_grp.h>
#include <ofirma.h>
#include <ofirmres.h>
#include <otown.h>
#include <ou_god.h>
#include <of_mfort.h>

#define STD_REACH_DEST_RANGE 8

//--------- Begin of function Unit::pre_move-------//
//
// This is function is used for initiating moving for
// different action orders.
//
// <int>  destXLoc, destYLoc - the destination
//
void Unit::pre_move(int destXLoc, int destYLoc)
{
	stop_order();

	move_to_loc_x = destXLoc;
	move_to_loc_y = destYLoc;
}
//--------- End of function Unit::pre_move---------//


//--------- Begin of function Unit::move ---------//
//
// <int>  destXLoc, destYLoc - the destination
// [bool] forceMoveFlag		  - whether force moving the unit
//										 (default: false)
//
void Unit::move(int destXLoc, int destYLoc, bool forceMoveFlag)
{
	set_move(destXLoc, destYLoc, forceMoveFlag);

	move_to(destXLoc, destYLoc);
}
//----------- End of function Unit::move -----------//


//--------- Begin of function Unit::set_move ---------//
//
// This function is called by Unit::move() and UnitGroup::move()
//	to set move destination.
//
// <int>  destXLoc, destYLoc - the destination
// [bool] forceMoveFlag	- whether force moving the unit
//								  (default: false)
//
void Unit::set_move(int destXLoc, int destYLoc, bool forceMoveFlag)
{
	stop_order();	 		// stop current action

	// ####### begin Gilbert 11/6 ########//
	cur_order.set( UNIT_MOVE, 0, destXLoc, destYLoc );
	// ####### end Gilbert 11/6 ########//

	force_move_flag = forceMoveFlag;
}
//----------- End of function Unit::set_move -----------//


//--------- Begin of function Unit::execute_move ---------//
//
void Unit::execute_move()
{
	if( cur_action != SPRITE_IDLE )
		return;

	//--- if this a commander and its team has formation, form formation ---//

	if( team_info && team_info->formation_id && team_info->member_count>0 )
	{
		//--- check if all members in the team has stooped ---//

		team_info->validate();

		int i;
		for( i=0 ; i<team_info->member_count ; i++ )
		{
			int unitRecno = team_info->member_unit_array[i];		// the leader is a member in team_info->member_unit_array[]

			if( unitRecno != sprite_recno && !unit_array[unitRecno]->is_all_stop() )
				break;
		}
		
		if( i== team_info->member_count && team_info->formation_id == FORMATION_PRE_SQUARE)
		{
			team_info->formation_id = FORMATION_SQUARE;
			// ###### begin Gilbert 26/2 ######//
			unit_group.set( team_info->member_unit_array, team_info->member_count );
			// ###### end Gilbert 26/2 ######//
			unit_group.formation(FORMATION_SQUARE, COMMAND_AUTO);
			stop_order();
			return;
		}
		//--- if all members in the team has stooped, form formation now ---//

		if( i==team_info->member_count )
		{
			unit_group.set( team_info->member_unit_array, team_info->member_count );
			if(team_info->formation_direction)
				unit_group.formation_turn(team_info->formation_direction, COMMAND_AUTO);
			else
				unit_group.formation(team_info->formation_id, COMMAND_AUTO);

			stop_order();		// stop order, if the members are still moving, retain the order and await all of them to stop.
		}
	}
	else
	{
		// ##### begin Gilbert 12/5 #########//
		// avoid move again when too close the the destination
		// because a unit may ask other units to move away
		if( misc.points_distance( next_x_loc(), next_y_loc(), move_to_loc_x, move_to_loc_y ) > STD_REACH_DEST_RANGE )
		{
			// backup retry_state and wait_state
			int backupRetryState = retry_state;
			move_to( move_to_loc_x, move_to_loc_y );
			retry_state = backupRetryState + 1;		// restore and increase
		}
		else
			stop_order();	// set cur_order.mode to UNIT_STOP
		// ##### end Gilbert 12/5 #########//
	}
}
//--------- End of function Unit::execute_move ---------//


//===============================================================//


//--------- Begin of function Unit::build_firm ---------//
//
// Build a firm or a town.
//
// <int> buildXLoc, buildYLoc - the location to build
// <int> firmId               - id. of the firm to build
//										  (0 for building a town)
// [char] remoteAction		   - remote action
//
void Unit::build_firm(int buildXLoc, int buildYLoc, int firmId, char remoteAction)
{
	//NEWCODEHERE

	if( !firm_res[firmId]->can_build(sprite_recno, 1) )		// 1 - check cash
		return;

	//------ if it is a remote action ---------//

	if(!remoteAction && remote.is_enable() )
	{
		// packet structure : <unit recno> <xLoc> <yLoc> <firmId>
		short *shortPtr =(short *)remote.new_send_queue_msg(MSG_UNIT_BUILD_FIRM, 4*sizeof(short) );
		shortPtr[0] = sprite_recno;
		shortPtr[1] = buildXLoc;
		shortPtr[2] = buildYLoc;
		shortPtr[3] = firmId;
		return;
	}

	//---- no action if the unit is dead or dying ------//

	if( is_dead() )
		return;

	//-------- stop current action ----//

	stop_order();

	//----- no action if a firm cannot be built on the location ----//

	if(!world.can_build_firm(buildXLoc, buildYLoc, firmId, sprite_recno))
		return;

	//------ set new UnitOrder -------//

	cur_order.set( UNIT_BUILD_FIRM, firmId, buildXLoc, buildYLoc );

	move_to( buildXLoc, buildYLoc );
}
//--------- End of function Unit::build_firm ---------//


//--------- Begin of function Unit::execute_build_firm ---------//
//
// This function is called by Unit::pre_process().
//
void Unit::execute_build_firm()
{
	//-- if the unit hasn't finished moving yet ----//

	if( cur_action != SPRITE_IDLE )
	{
		if( is_next_to_order_place() )
		{
			stop_move();		// wait for some frames later, stopped then start to build
			if(is_in_formation())
				set_no_longer_in_formation();
		}
		return;
	}

	//---- if the unit cannot build on the specific location or the nation cannot build the firm because of lack of cash or scroll, return now ---//

	if( !world.can_build_firm(cur_order.loc_x, cur_order.loc_y, cur_order.para, sprite_recno) ||
		 !firm_res[cur_order.para]->nation_can_build(nation_recno, race_id) )
	{
		stop_order();			// cancel the action and return
		return;
	}

	//--- only if the unit is already close enough to build a firm ---//

	if( !is_next_to_order_place() )
	{
		stop_order();		// cancel the action, as the unit cannot successfully reach the destination
		return;
	}

	//------- build the place now -------//

	if( build_firm_now() )
		completed_order(); 		// built successfully
	else
		stop_order();				// failed to build it
}
//--------- End of function Unit::execute_build_firm ---------//


//--------- Begin of function Unit::build_firm_now ---------//
//
// This function is called by Unit::build_now() to build
// the specific place.
//
int Unit::build_firm_now()
{
	//--------------------------------------------------------------------//
	// if unit stands on the building site, deinit the unit to free up the
	// space.
	//--------------------------------------------------------------------//

	if( is_in_order_place() )
	{
		deinit_sprite(0); 	// 0-if the unit is currently selected, deactivate it.
	}

	//----- get the build code -----//

	char *buildCode;

	if( race_id )
	{
		if( is_monster() )
			buildCode = monster_res[monster_id()]->firm_build_code;
		else
			buildCode = race_res[race_id]->code;
	}
	else
		err_here();

#ifdef DEBUG
	unsigned long startTime = misc.get_time();
	int check = firm_array.build_firm(cur_order.loc_x, cur_order.loc_y, nation_recno, cur_order.para, buildCode, sprite_recno);
	unit_check_build_firm_profile_time += misc.get_time() - startTime;
	return check;
#else
	return firm_array.build_firm(cur_order.loc_x, cur_order.loc_y, nation_recno,
										  cur_order.para, buildCode, sprite_recno);
#endif
}
//--------- End of function Unit::build_firm_now ---------//


//===============================================================//


//--------- Begin of function Unit::settle_town ---------//
//
// Settle to a town.
//
// If there is no town on the location, build one.
// If there is already one on the location, move into the town.
//
// <int> townXLoc, townYLoc - the location to build
// [char] remoteAction		 - remote action
//
void Unit::settle_town(int townXLoc, int townYLoc, char remoteAction)
{
	err_when( !is_human() );

	if(!remoteAction && remote.is_enable() )
	{
		// packet structure : <xLoc> <yLoc> <1> <unit recno>
		short *shortPtr =(short *)remote.new_send_queue_msg(MSG_UNITS_SETTLE, 4*sizeof(short) );
		shortPtr[0] = townXLoc;
		shortPtr[1] = townYLoc;
		shortPtr[2] = 1;		// single unit settle
		shortPtr[3] = sprite_recno;
		return;
	}

	if( set_settle_town(townXLoc, townYLoc) )
		move_to( townXLoc, townYLoc );
}
//--------- End of function Unit::settle_town ---------//


//--------- Begin of function Unit::set_settle_town ---------//
//
// Settle to a town.
//
// If there is no town on the location, build one.
// If there is already one on the location, move into the town.
//
// <int> townXLoc, townYLoc - the location to build
//
int Unit::set_settle_town(int townXLoc, int townYLoc)
{
	//---- no action if the unit is dead or dying ------//

	if( is_dead() )
		return 0;

	//---- if there is already a town there -----//

	Location* locPtr = world.get_loc(townXLoc, townYLoc);

	if( locPtr->is_town() )
	{
		Town* townPtr = town_array[ locPtr->town_recno() ];

		if( townPtr->nation_recno == nation_recno )		// if the town is our own
			assign(townPtr->loc_x1, townPtr->loc_y1);

		return 1;
	}

	//-------- stop current action ----//

	stop_order();

	//------ set new UnitOrder -------//

	cur_order.set( UNIT_SETTLE_TOWN, 0, townXLoc, townYLoc );

	return 1;
}
//--------- End of function Unit::set_settle_town ---------//


//--------- Begin of function Unit::execute_settle_town ---------//
//
// This function is called by Unit::pre_process().
//
void Unit::execute_settle_town()
{
	//-- if the unit hasn't finished moving yet ----//

	if( cur_action != SPRITE_IDLE )
	{
		if( is_next_to_order_place() )
			stop_move();		// wait for some frames later, stopped then start to build

		return;
	}

	//------ if a town has already be built on the location ----//

	Location* locPtr = world.get_loc(cur_order.loc_x, cur_order.loc_y);

	if( locPtr->is_town() )
	{
		Town* townPtr = town_array[ locPtr->town_recno() ];

		//---- if this is our town, settle into it now ----//

		if( townPtr->nation_recno == nation_recno )
		{
			int aiActionId = cur_order.ai_action_id;	// move the ai_action_id to the new action. Reset it first so that action_failure() won't be called
			cur_order.ai_action_id = 0;

			assign(townPtr->loc_x1, townPtr->loc_y1);

			cur_order.ai_action_id = aiActionId;
			return;
		}
	}

	//---- check if the unit can build on the location now ----//

	if( !world.can_build_town(cur_order.loc_x, cur_order.loc_y, sprite_recno) )
	{
		stop_order();			// cancel the action and return
		return;
	}

	//--- only if the unit is already close enough to build a town ---//

	if( !is_next_to_order_place() )
	{
		stop_order();		// cancel the action, as the unit cannot successfully reach the destination
		return;
	}

	//--------------------------------------------------------------------//
	// if unit stands on the building site, deinit the unit to free up the
	// space.
	//--------------------------------------------------------------------//

	if( is_in_order_place() )
	{
		deinit_sprite(0); 	// 0-if the unit is currently selected, deactivate it.
	}

	//------- build the town now -------//

	if( town_array.build_town(sprite_recno, cur_order.loc_x, cur_order.loc_y) )
		completed_order(); 		// built successfully
	else
		stop_order();				// failed to build it
}
//--------- End of function Unit::execute_settle_town ---------//


//===============================================================//


//--------- Begin of function Unit::assign ---------//
//
// Settle to a town. It has no remoteAction parameter as UnitGroup::assign() already has.
// So that UnitGroup::assign() should be called instead of this for assign action. 
//
// If there is no town on the location, build one.
// If there is already one on the location, move into the town.
//
// <int> destXLoc, destYLoc - the location to build
// [int] actionNationRecno  - the nation that executes the action.
//									   (default: nation_recno of the unit)
//
void Unit::assign(int destXLoc, int destYLoc, int actionNationRecno, bool forceMoveFlag)
{
#ifdef DEBUG
	unsigned long startTime = misc.get_time();
#endif

	// ######## begin Gilbert 15/5 ########//
	if( set_assign(destXLoc, destYLoc, actionNationRecno, forceMoveFlag) )
	{
		// cur_order.loc_x/y is the top left corner of the dest obj
		// but move_to the center of the obj

		if( !base_obj_array.is_deleted(cur_order.para) )
		{
			BaseObj *destObj = base_obj_array[cur_order.para];
			destXLoc = (destObj->obj_loc_x1() + destObj->obj_loc_x2()) / 2;
			destYLoc = (destObj->obj_loc_y1() + destObj->obj_loc_y2()) / 2;
		}
		move_to( destXLoc, destYLoc );
	}
	// ######## end Gilbert 15/5 ########//
#ifdef DEBUG
	unit_assign_profile_time += misc.get_time()-startTime;
#endif
}
//--------- End of function Unit::assign ---------//


//--------- Begin of function Unit::set_assign ---------//
//
// Settle to a town.
//
// If there is no town on the location, build one.
// If there is already one on the location, move into the town.
//
// <int> destXLoc, destYLoc - the location to build
//
int Unit::set_assign(int destXLoc, int destYLoc, int actionNationRecno, bool forceMoveFlag)
{
	//---- no action if the unit is dead or dying ------//

	if( is_dead() )
		return 0;

	//-------- stop current action ----//

	//stop_order();

	//---- if there is a town on the given location -----//

	Location* locPtr = world.get_loc(destXLoc, destYLoc);
	int		 destObjRecno;

	if( locPtr->is_town() )
	{
		Town* townPtr = town_array[ locPtr->town_recno() ];

		destXLoc = townPtr->loc_x1;
		destYLoc = townPtr->loc_y1;
		destObjRecno = townPtr->base_obj_recno;
	}

	//---- if there is a firm on the given location -----//

	else if( locPtr->is_firm() )
	{
		Firm* firmPtr = firm_array[ locPtr->firm_recno() ];

		destXLoc = firmPtr->loc_x1;
		destYLoc = firmPtr->loc_y1;
		destObjRecno = firmPtr->base_obj_recno;
	}
	else //-- if there is nothing assignable on the given location, do nothing --//
	{
		return 0;
	}

	//---- return whether the firm/town accept the assign ----//

	if( !base_obj_array[destObjRecno]->can_accept_assign(sprite_recno) )
		return 0;

	//------ set new UnitOrder -------//

	cur_order.set( UNIT_ASSIGN, destObjRecno, destXLoc, destYLoc );

	cur_order.action_nation_recno = actionNationRecno;

	// ####### begin Gilbert 2/6 #######//
	force_move_flag = forceMoveFlag;
	// ####### end Gilbert 2/6 #######//

	return 1;
}
//--------- End of function Unit::set_assign ---------//


//--------- Begin of function Unit::execute_assign ---------//
//
// This function is called by Unit::pre_process().
//
void Unit::execute_assign()
{
	//-- if the unit hasn't finished moving yet ----//

	if( cur_action != SPRITE_IDLE )
		return;

	//----- check if the assign order is still valid ----//

	if( !validate_assign() )
	{
		stop_order();
		return;
	}

	//--- only if the unit is already close enough to build a town ---//

	if( !is_next_to_order_place())
	{
		//stop_order();		// cancel the action, as the unit cannot successfully reach the destination
		if(seek_path_fail_count < 50*SEEK_PATH_FAIL_INCREMENT)
			assign(cur_order.loc_x, cur_order.loc_y);
		else
			stop_order();
		return;
	}

	//-------- assign now ---------//

	Place* placePtr = base_obj_array[cur_order.para]->cast_to_Place();

	if( !placePtr )
	{
		err_here();
		stop_order();
		return;
	}

	if( !placePtr->under_construction )
	{
		completed_order();		// the unit will be deleted after calling assign_unit(), so complete_order() must be called first
		placePtr->assign_unit(sprite_recno);
	}
}
//--------- End of function Unit::execute_assign ---------//


//--------- Begin of function Unit::validate_assign ---------//
//
int Unit::validate_assign()
{
	if( base_obj_array.is_deleted(cur_order.para) )
	{
		return 0;
	}

	BaseObj* baseObj = base_obj_array[cur_order.para];

	if( baseObj->obj_loc_x1() != cur_order.loc_x ||
		 baseObj->obj_loc_y1() != cur_order.loc_y )
	{
		return 0;
	}

	return baseObj->can_accept_assign(sprite_recno, cur_order.action_nation_recno);
}
//--------- End of function Unit::validate_assign ---------//


//===============================================================//


//--------- Begin of function Unit::go_cast_power ---------//
//
void Unit::go_cast_power(int castXLoc, int castYLoc, char remoteAction, int targetBaseObjRecno )
{
	//NEWCODEHERE

	if(!remoteAction && remote.is_enable() )
	{
		//------------ process multiplayer calling ---------------//
		// packet structure : <unit recno> <xLoc> <yLoc> <power type>

		short *shortPtr =(short *)remote.new_send_queue_msg(MSG_U_GOD_CAST, 5*sizeof(short) );
		shortPtr[0] = sprite_recno;
		shortPtr[1] = castXLoc;
		shortPtr[2] = castYLoc;
		// shortPtr[3] = magicId;
		shortPtr[4] = targetBaseObjRecno;
		return;
	}

	if( set_go_cast_power(castXLoc, castYLoc, targetBaseObjRecno) )
		move_to( castXLoc, castYLoc );
}
//----------- End of function Unit::go_cast_power -----------//


//--------- Begin of function Unit::set_go_cast_power ---------//
//
int Unit::set_go_cast_power(int castXLoc, int castYLoc, int targetBaseObjRecno )
{
	//---- no action if the unit is dead or dying ------//

	if( is_dead() )
		return 0;

	//-------- stop current action ----//

	stop_order();

	cur_order.set( UNIT_GO_CAST_POWER, targetBaseObjRecno, castXLoc, castYLoc );

	return 1;
}
//----------- End of function Unit::set_go_cast_power -----------//


//--------- Begin of function Unit::execute_go_cast_power ---------//
//
void Unit::execute_go_cast_power()
{
	//----------------------------------------------//

	UnitGod *unitGod = cast_to_UnitGod();

	err_when( !unitGod );

	if(cur_action==SPRITE_IDLE)
	{
		//--- if this unit is close enough to the target location ---//

		if( misc.points_distance(next_x_loc(), next_y_loc(),
			 cur_order.loc_x, cur_order.loc_y) <= DO_CAST_POWER_RANGE )
		{
			//--- set the current attack mode to the cast spell attack mode (whose attack range must be 1) ---//

			int i;
			for( i=0 ; i<attack_count ; i++ )
			{
				if( attack_info_array[i].attack_range==1 )
				{
					cur_attack = i;
					break;
				}
			}

			err_when( i==attack_count );

			//------ prepare for attack now -------//

			cur_frame = 1;
			set_cur(next_x, next_y);
			set_attack_dir(next_x_loc(), next_y_loc(), cur_order.loc_x, cur_order.loc_y);

			final_dir = cur_dir;
			turn_delay = 0;
			cur_action = SPRITE_ATTACK;
			// #### begin Gilbert 16/12 ######//
			// set_remain_attack_delay();	// too early to set delay
			// #### end Gilbert 16/12 ######//
		}
		else		// if the unit cannot get close to the destination, stop order now
		{
			stop_order();
		}
	}
	// #### begin Gilbert 19/2 #######//
	else if(cur_action==SPRITE_MOVE)		// check idle first before move
	{
		// see if target exist

		if( cur_order.para )
		{
			if( base_obj_array.is_deleted(cur_order.para)
				|| !base_obj_array[cur_order.para]->is_visible() )
			{
				stop_order();
				return;
			}
			else
			{
				// update target location

				BaseObj *targetObj = base_obj_array[cur_order.para];
				int diff = misc.points_distance( cur_order.loc_x, cur_order.loc_y,
					targetObj->obj_loc_x1(), targetObj->obj_loc_y1() );

				if( diff > 0 )
				{
					if( misc.points_distance( move_to_loc_x, move_to_loc_y ,
						targetObj->obj_loc_x1(), targetObj->obj_loc_y1() ) > DO_CAST_POWER_RANGE
						|| area_distance( targetObj ) < DO_CAST_POWER_RANGE * 2 )
					{
						move_to( targetObj->obj_loc_x1(), targetObj->obj_loc_y1());
					}

					cur_order.loc_x = targetObj->obj_loc_x1();
					cur_order.loc_y = targetObj->obj_loc_y1();
				}
			}
		}

		if( is_next_to_order_place() )
			stop_move();		// wait for some frames later, stopped then start to cast
	}
	// #### end Gilbert 19/2 #######//
	else if(cur_action==SPRITE_ATTACK)
	{
		//------------------------//
		// Do casting power now
		//------------------------//

		if( cur_frame == cur_attack_info()->bullet_out_frame )
		{
			// add effect
			add_attack_effect();

			if( cur_order.para )		// if target is a unit, update cur_order.loc_x/y
			{
				BaseObj *targetObj;
				if( !base_obj_array.is_deleted(cur_order.para)
					&& (targetObj = base_obj_array[cur_order.para])
					&& targetObj->is_visible()
					&& area_distance(targetObj) <= DO_CAST_POWER_RANGE )
				{
					cur_order.loc_x = targetObj->obj_loc_x1();
					cur_order.loc_y = targetObj->obj_loc_y1();
				}
			}

			unitGod->cast_power(cur_order.loc_x, cur_order.loc_y);
			set_remain_attack_delay();
		}

		if( cur_frame==1 && remain_attack_delay==0 )                // last frame of delaying
			completed_order();
	}
}
//--------- End of function Unit::execute_go_cast_power ---------//

//--------- End of function Unit::execute_transform_fortress-----//
void Unit::execute_transform_fortress()
{
	if(cur_action != SPRITE_IDLE)
		return;

	if(	// cur_action == SPRITE_IDLE && 
		(next_x_loc() != cur_order.loc_x  || next_y_loc() != cur_order.loc_y) )
	{
		// if cannot move wait ... and wait forever

		if( can_move2(cur_order.loc_x, cur_order.loc_y)	)
			move_to(cur_order.loc_x, cur_order.loc_y);
		return;
	}

	int fortressXLoc=-1, fortressYLoc;
	// #### begin Gilbert 25/1 ########//
	short builderUnit[MAX_EXTRA_BUILDER];
	if( FirmMonsterFortress::check_transform_from_units(sprite_recno, fortressXLoc, fortressYLoc, builderUnit) )
	{
		int otherBuilderFlag = 1;
		int i;
		for( i = 0; i < MAX_EXTRA_BUILDER; ++i )
		{
			// other checking other than check_transform_from_units

			if( unit_array.is_deleted(builderUnit[i]) )
				break;

			Unit *coBuilder = unit_array[builderUnit[i]];

			if( coBuilder->cur_order.mode != cur_order.mode
				|| coBuilder->cur_action != SPRITE_IDLE )
				break;
		}

		if( i >= MAX_EXTRA_BUILDER )		// all four builder is ok
		{
			FirmMonsterFortress::transform_from_units(sprite_recno, fortressXLoc, fortressYLoc, builderUnit, COMMAND_AUTO);
			for(int i = 0; i<MAX_EXTRA_BUILDER; i++)
				unit_array[builderUnit[i]]->completed_order();
		}
	}
	// #### end Gilbert 25/1 ########//
}
//--------- End of function Unit::execute_transform_fortress-----//
