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

//Filename	 : OPER_INF.CPP
//Description: Unit functions related to basic information

#include <OSYS.H>
#include <OSPY.H>
#include <OTRANSL.H>
#include <OFIRM.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <ONATION.H>
#include <OPOWER.H>
#include <OUNIT.H>
#include <ITEMID.H>
#include <OGAME.H>


//--------- Begin of function Unit::Unit ---------//
//
Unit::Unit()
{
	memset( sizeof(UnitB) + (char *)this, 0, sizeof(Unit) - sizeof(UnitB));

	pushed_order.mode = -1;		// -1 means not used

	set_original_target(-1, -1);

	auto_retreat_hit_point = 0;
}
//--------- End of function Unit::Unit ---------//


//--------- Begin of function Unit::init ---------//
//
// <int> unitId               - the id. of the unit
// <int> nationRecno          - the recno of nation
// [int] rankId               - rank id. of the unit (none for non-human unit)
// [int] unitLoyalty          - loyalty of the unit  (none for non-human unit)
// [int] startXLoc, startYLoc - the starting location of the unit
//                              (if startXLoc < 0, this is a unit for hire, and is not a unit of the game yet. init_sprite() won't be called for this unit)
//                              (default: -1, -1)
//
// Note: sprite_recno must be initialized first before calling Unit::init()
//
void Unit::init(int unitId, int nationRecno, int rankId, int unitLoyalty, int startXLoc, int startYLoc)
{
	//------------ set basic vars -------------//

	nation_recno  = (char) nationRecno;
	rank_id       = rankId;       // rank_id must be initialized before init_unit_id() as init_unit_id() may overwrite it

	if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
	{
		err_when( team_info );
		team_info = (TeamInfo*) mem_add( sizeof(TeamInfo) );
		memset( team_info, 0, sizeof(TeamInfo) );
	}

	init_unit_id(unitId);

	race_id = (char) unit_res[unit_id]->race_id;

	//------- init unit name ---------//

	if( is_human() && unit_id != UNIT_WAGON )
	{
		name_id = race_res[race_id]->get_new_name_id();
	}
	else if( is_monster() )
	{
		name_id = monster_res.get_new_name_id();
	}
	else  //---- init non-human unit series no. ----//
	{
		if( nation_recno )
			name_id = ++nation_array[nation_recno]->last_unit_name_id_array[unit_id-1];
		else
			name_id = 0;
	}

	// ###### begin Gilbert 23/2 ######//
	unique_id = m.rand_long();
	// ###### end Gilbert 23/2 ######//

	//---------- init other vars ----------//

	err_when( unitLoyalty < 0 || unitLoyalty > 100 );

	loyalty = unitLoyalty;

	// ##### begin Gilbert 29/5 ########//
	if( is_die_after_attack() )
		// behavior_mode = UNIT_STAND_GROUND;		// bee die after attack, so don't waste his life
		behavior_mode = UNIT_DEFENSIVE;		// bee die after attack, so don't waste his life
	else
		behavior_mode = UNIT_AGGRESSIVE;		// the default mode is aggressive
	// ##### end Gilbert 29/5 ########//

	//--------- init skill, and skill potential ---------//

	skill.init(unit_id, 100);
	set_combat_level(-1);

	hit_points = (float) max_hit_points();

	//------ initialize the base Sprite class -------//

	if( startXLoc >= 0 )
		init_sprite( startXLoc, startYLoc );
	else
		cur_x = -1;

	//-------------- update loyalty -------------//

	update_loyalty();

	//--------------- init AI info -------------//

	init_ai();

	//----------- init derived class ----------//

	init_derived();

	set_combat_level(-1);
}
//----------- End of function Unit::init -----------//


//--------- Begin of function Unit::init_unit_id ---------//

void Unit::init_unit_id(int unitId)
{
	unit_id = unitId;

	UnitInfo* unitInfo = unit_res[unit_id];

	sprite_id   = unitInfo->sprite_id;
	sprite_info = sprite_res[sprite_id];

	mobile_type = unitInfo->mobile_type;
	// #### begin Gilbert 6/11 ######//
	defense_attribute = unitInfo->defense_attribute;
	// #### end Gilbert 6/11 ######//

	//--- if this unit is a weapon unit with multiple versions ---//

	int techLevel;

	if( nation_recno &&
		// ##### begin Gilbert 24/3 ########//
		//unitInfo->unit_class == UNIT_CLASS_WEAPON )
		unitInfo->class_info.has_weapon_version )
		// ##### end Gilbert 24/3 ########//
	{
		techLevel=unitInfo->nation_tech_level_array[nation_recno-1];
		if (techLevel == 0)
			set_weapon_version(1);
		else
			set_weapon_version(techLevel);
	}

	init_attack_info();

	//-------- set unit count ----------//

	if( nation_recno )
	{
		if( rank_id != RANK_KING )
			unitInfo->inc_nation_unit_count(nation_recno);

		if( rank_id == RANK_GENERAL )
			unitInfo->inc_nation_general_count(nation_recno);
	}

	//--------- increase monster count ----------//

	// ##### begin Gilbert 24/3 #######//
//	if( unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER )
	if( unit_res[unit_id]->class_info.monster_side )
		unit_res.mobile_monster_count++;
	// ##### end Gilbert 24/3 #######//
}
//----------- End of function Unit::init_unit_id -----------//


//--------- Begin of function Unit::init_ai ---------//
//
void Unit::init_ai()
{
	is_ai = nation_recno && nation_array[nation_recno]->nation_type == NATION_AI;

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		if( rank_id==RANK_GENERAL || rank_id==RANK_KING )
			nationPtr->add_general_info(sprite_recno);

		switch( unit_res[unit_id]->unit_class )
		{
			case UNIT_CLASS_CARAVAN:
				nationPtr->add_caravan_info(sprite_recno);
				break;

			case UNIT_CLASS_SHIP:
				nationPtr->add_ship_info(sprite_recno);
				break;
		}
	}
}
//--------- End of function Unit::init_ai ---------//


//----------- Begin of function Unit::init_attack_info -----------//

void Unit::init_attack_info()
{
	int techLevel;
	UnitInfo *unitInfo = unit_res[unit_id];

	attack_count = unitInfo->attack_count;

	if( attack_count > 0 && unitInfo->first_attack > 0)
		attack_info_array = unit_res.attack_info_array+unitInfo->first_attack-1;
	else
		attack_info_array = NULL;

	// ####### begin Gilbert 24/3 ########//
//	if( unitInfo->unit_class == UNIT_CLASS_WEAPON &&
//		(techLevel=get_weapon_version()) > 0 )
	if( unitInfo->class_info.has_weapon_version &&
		(techLevel=get_weapon_version()) > 0 )
	// ####### end Gilbert 24/3 ########//
	{
		switch( unit_id )
		{
		case UNIT_BALLISTA:
//#ifdef AMPLUS
		case UNIT_F_BALLISTA:
//#endif
			attack_count = 2;
			break;
		case UNIT_EXPLOSIVE_CART:
			attack_count = 0;
			break;

		case UNIT_M_CANNON:
			attack_count = 2;
			break;

		default:
			attack_count = 1;
		}

		if( attack_count > 0)
		{
			attack_info_array += (techLevel-1) * attack_count;
		}
		else
		{
			// no attack like explosive cart
			attack_info_array = NULL;
		}
	}
}
//----------- End of function Unit::init_attack_info -----------//


//--------- Begin of function Unit::init_sprite ---------//
//
// <int> startXLoc, startYLoc - the starting location of the unit
//
void Unit::init_sprite(int startXLoc, int startYLoc)
{
	Sprite::init( unit_res[unit_id]->sprite_id, startXLoc, startYLoc );			// BUGHERE use unit_id temporarily

	//-------- set loc_width and loc_height ------//

	loc_width  = (char) sprite_info->loc_width;
	loc_height = (char) sprite_info->loc_height;		// copy the vars for fast access

	// ###### begin Gilbert 30/4 ##########//
	move_to_loc_x = startXLoc; // * LOCATE_WIDTH;
	move_to_loc_y = startYLoc; // * LOCATE_HEIGHT; 
	// ###### end Gilbert 30/4 ##########//

	//-------- set the cargo_recno -------------//

	int w, h, x, y;

	err_if(!sprite_recno)      // sprite_recno must be initialized first before calling UnitB::init()
		err_here();

	for(h=0, y=startYLoc; h<loc_height; h++, y++)
	{
		for(w=0, x=startXLoc; w<loc_width; w++, x++)
		{
			err_if( !world.get_loc(x, y)->can_move(mobile_type) )    // it must be 0 to put the sprite in this location
				err_here();

			world.get_loc(x, y)->set_unit(sprite_recno, mobile_type);
		}
	}

	explore_area();
}
//----------- End of function Unit::init_sprite -----------//


//==============================================================//


//--------- Begin of function Unit::~Unit ---------//
//
Unit::~Unit()
{
	deinit();
}
//--------- End of function Unit::~Unit ---------//


//--------- Begin of function Unit::deinit ---------//
//
void Unit::deinit()
{
	//---------- free up team_info ----------//

	if( team_info )
	{
		mem_del(team_info);
		team_info = NULL;
	}

	//---------------------------------------//

	// ####### begin Gilbert 22/3 ########//
	if( sys.quick_exit_flag() )
	{
		if( game.is_campaign_mode() )
		{
			// if in campaign mode, free name before exit

			if( name_id )
			{
				if( is_human() && unit_id != UNIT_WAGON )
				{
					if( name_id )
						race_res[race_id]->free_name_id( name_id );
				}
				else if( is_monster() )
				{
					if( name_id )
						monster_res.free_name_id( name_id );
				}
				else  //---- init non-human unit series no. ----//
				{
				}

				name_id = 0;
			}
		}
		return;
	}
	// ####### end Gilbert 22/3 ########//

	//---------------------------------------//

	err_when( unit_array.is_truly_deleted(sprite_recno) );

	if( !unit_id )
		return;

	// ######### begin Gilbert 23/2 #######//
	// -------- update win/lose condition or die -------//

	if( hit_points <= 0.0f || cur_action == SPRITE_DIE )
	{
		game.update_condition_on_killed( unique_id );
	}
	// ######### end Gilbert 23/2 #######//

	//--------- drop item -----------//

	if( !sys.signal_exit_flag && item.id )
	{
		drop_item(COMMAND_AUTO);
	}

	//-------- if this is a king --------//

	if( !sys.signal_exit_flag && nation_recno )
	{
		if( rank_id == RANK_KING )    // check nation_recno because monster kings will die also.
		{
			king_die();
			err_when( unit_array.is_truly_deleted(sprite_recno) );
		}
		else if( rank_id == RANK_GENERAL )
		{
			general_die();
			err_when( unit_array.is_truly_deleted(sprite_recno) );
		}
	}

	// ####### begin Gilbert 12/3 #######//

	// -------- free name id after general_die, which reads name_id ---------//

	if( name_id )
	{
		if( is_human() && unit_id != UNIT_WAGON )
		{
			if( name_id )
				race_res[race_id]->free_name_id( name_id );
		}
		else if( is_monster() )
		{
			if( name_id )
				monster_res.free_name_id( name_id );
		}
		else  //---- init non-human unit series no. ----//
		{
		}

		name_id = 0;
	}

	// ####### end Gilbert 12/3 #######//

	//---- if this is a general, deinit its link with its soldiers ----//
   //
   // We do not use team_info because monsters and rebels also use
	// leader_unit_recno and they do not use keep the member info
   // in team_info.
   //
   //-----------------------------------------------------------------//

   if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
	{
		for( int i=unit_array.size() ; i>0 ; i-- )
      {
         if( unit_array.is_deleted(i) )
            continue;

			if( unit_array[i]->leader_unit_recno == sprite_recno )
				unit_array[i]->leader_unit_recno = 0;
      }
   }

	//----- if this is a unit on a ship ------//

	if( unit_mode == UNIT_MODE_ON_SHIP )
	{
		err_here();
		/*
		if( !unit_array.is_deleted(unit_mode_para) )    // the ship may have been destroyed at the same time. Actually when the ship is destroyed, all units onboard are killed and this function is called.
		{
			Unit* unitPtr = unit_array[unit_mode_para];

			err_when( unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_SHIP );

			((UnitMarine*)unitPtr)->del_unit(sprite_recno);
		}
		*/
	}

	//----- if this is a ship in the harbor -----//

	else if( unit_mode == UNIT_MODE_IN_HARBOR )
	{
		err_here();
		/*
		if( !firm_array.is_deleted(unit_mode_para) )    // the ship may have been destroyed at the same time. Actually when the ship is destroyed, all firms onboard are killed and this function is called.
		{
			Firm* firmPtr = firm_array[unit_mode_para];

			err_when( firmPtr->firm_id != FIRM_HARBOR );

			((FirmHarbor*)firmPtr)->del_hosted_ship(sprite_recno);
		}
		*/
	}

	//----- if this unit is a constructor in a firm -------//

	else if( unit_mode == UNIT_MODE_CONSTRUCT_FIRM )
	{
		err_when( firm_array[unit_mode_para]->builder_recno != sprite_recno );

		firm_array[unit_mode_para]->builder_recno = 0;
	}

	//----- if this unit is a constructor in a town -------//

	else if( unit_mode == UNIT_MODE_CONSTRUCT_TOWN )
	{
		err_when( town_array[unit_mode_para]->builder_recno != sprite_recno );

		town_array[unit_mode_para]->builder_recno = 0;
	}

	//-------- if this is a spy ---------//

   if( spy_recno )
   {
      spy_array.del_spy( spy_recno );
		spy_recno = 0;
   }

   //---------- reset command ----------//

   if( power.command_unit_recno == sprite_recno )
      power.reset_command();

   //-----------------------------------//

   deinit_unit_id();

	//----- if cur_x == -1, the unit has not yet been hired -----//

	if( cur_x >= 0 )
		deinit_sprite();
	// ##### patch begin Gilbert 6/10 ######//
	else if( selected_flag )	// caravan may be deinit_sprite(1) and keep selected, now unselect
	{
		int curSelected = unit_array.selected_recno == sprite_recno;		// if it is currently selected unit, reset selected_recno
		deselect();
		if( curSelected && !sys.signal_exit_flag )
			info.disp();
	}
	// ##### patch end Gilbert 6/10 ######//

	//--------------- deinit AI info -------------//

	deinit_ai();

	//-------------- reset unit_id ---------------//

	unit_id = 0;
}
//--------- End of function Unit::deinit ---------//


//--------- Begin of function Unit::deinit_unit_id ---------//

void Unit::deinit_unit_id()
{
	if( sys.signal_exit_flag )
		return;

	//-----------------------------------------//

	UnitInfo *unitInfo = unit_res[unit_id];

	if( nation_recno )
	{
		if( rank_id != RANK_KING )
			unitInfo->dec_nation_unit_count(nation_recno);

      if( rank_id == RANK_GENERAL )
         unitInfo->dec_nation_general_count(nation_recno);
   }

   //--------- if the unit is a spy ----------//
   //
   // A spy has double identity and is counted
   // by both the true controlling nation and
   // the deceiving nation.
   //
   //-----------------------------------------//

   if( spy_recno && true_nation_recno() != nation_recno )
	{
      err_when( !race_id );

      int trueNationRecno = true_nation_recno();

      if( rank_id != RANK_KING )
         unitInfo->dec_nation_unit_count(trueNationRecno);

      if( rank_id == RANK_GENERAL )
         unitInfo->dec_nation_general_count(trueNationRecno);
	}

	//--------- decrease monster count ----------//

	// ####### begin Gilbert 24/3 #########//
//	if( unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER )
	if( unit_res[unit_id]->class_info.monster_side )
	// ####### end Gilbert 24/3 #########//
	{
		unit_res.mobile_monster_count--;
		err_when( unit_res.mobile_monster_count < 0 );
	}
}
//----------- End of function Unit::deinit_unit_id -----------//


//--------- Begin of function Unit::deinit_ai ---------//
//
void Unit::deinit_ai()
{
	if( nation_recno && !nation_array.is_deleted(nation_recno) )
	{
		Nation* nationPtr = nation_array[nation_recno];

		if( rank_id==RANK_GENERAL || rank_id==RANK_KING )
			nationPtr->del_general_info(sprite_recno);

		switch( unit_res[unit_id]->unit_class )
		{
			case UNIT_CLASS_CARAVAN:
				nationPtr->del_caravan_info(sprite_recno);
				break;

			case UNIT_CLASS_SHIP:
				nationPtr->del_ship_info(sprite_recno);
				break;
		}
	}
}
//--------- End of function Unit::deinit_ai ---------//


//--------- Begin of function Unit::deinit_sprite ---------//
//
// [int] keepSelected - keep it selected if it is current selected.
//                      (default: 0)
//
void Unit::deinit_sprite(int keepSelected)
{
	if( cur_x == -1 )
		return;

	//---- if this unit is led by a leader, only mobile units has leader_unit_recno assigned to a leader -----//

	if( leader_unit_recno )
	{
		if( !unit_array.is_deleted(leader_unit_recno) )    // the leader unit may have been killed at the same time
		{
			// ###### patch begin Gilbert 3/9 #######// 
			err_when( !unit_array[leader_unit_recno]->team_info );
			unit_array[leader_unit_recno]->team_info->del_member(sprite_recno);
			// ###### end begin Gilbert 3/9 #######// 
		}

		leader_unit_recno = 0;
	}
	// ###### patch begin Gilbert 3/9 #######//
	if( leader_unit_recno )
	{
		// the leader unit may have been killed at the same time
		if( !unit_array.is_deleted(leader_unit_recno) )
		{
			if( unit_array[leader_unit_recno]->team_info )
			{
				unit_array[leader_unit_recno]->team_info->del_member(sprite_recno);
			}
			else
			{
				err_here();
				int b = 0;	 // set break point here
			}
		}
		leader_unit_recno = 0;
	}
//	if( team_info )
//	{
//		team_info->reset();
//	}
	// ###### patch end Gilbert 3/9 #######//


	//-------- clear the cargo_recno ----------//

	short w, h;
	short x, y;

	for(h=0, y=next_y_loc(); h<sprite_info->loc_height; h++, y++)
	{
		for(w=0, x=next_x_loc(); w<sprite_info->loc_width; w++, x++)
		{
			err_if( world.get_loc(x,y)->unit_recno(mobile_type) != sprite_recno )    // it must be 0 to put the sprite in this location
				err_here();

			world.get_loc(x,y)->remove_unit(mobile_type);
		}
	}

	cur_x = -1;

	//---- reset other parameters related to this unit ----//

	if( !keepSelected && selected_flag )
	{
		int curSelected = unit_array.selected_recno == sprite_recno;		// if it is currently selected unit, reset selected_recno

		deselect();

		if( curSelected )
			info.disp();
	}

	//------- deinit unit mode -------//

	deinit_unit_mode();
}
//----------- End of function Unit::deinit_sprite -----------//


//--------- Begin of function Unit::deinit_unit_mode ---------//
//
void Unit::deinit_unit_mode()
{
	if( sys.signal_exit_flag )
		return;

	//----- this unit was defending the town before it gets killed ----//

	if(unit_mode==UNIT_MODE_TOWN_DEFENDER)
	{
		if(!town_array.is_deleted(unit_mode_para))
		{
			Town *townPtr = town_array[unit_mode_para];

			if(nation_recno == townPtr->nation_recno)
				townPtr->reduce_defender_count();
		}

		set_mode(0);      // reset mode
	}

	// ###### begin Gilbert 30/4 #########//
	else if( unit_mode == UNIT_MODE_CAMP_DEFENDER )
	{
		// no extra action needed other than reset mode
//		if(!firm_array.is_deleted(unit_mode_para))
//		{
//			Firm *firmPtr = firm_array[unit_mode_para];
//			if(nation_recno == firmPtr->nation_recno)
//				firmPtr->reduce_defender_count();
//		}

		set_mode(0);      // reset mode
	}
	// ###### end Gilbert 30/4 #########//

/*
   //----- this is a monster unit defending its town ------//

	else if( unit_mode==UNIT_MODE_MONSTER && unit_mode_para )
	{
		if(((UnitMonster*)this)->monster_action_mode!=MONSTER_ACTION_DEFENSE)
			return;

		FirmMonster* firmMonster = (FirmMonster*) firm_array[unit_mode_para];

		err_when( firmMonster->firm_id != FIRM_MONSTER );

		firmMonster->reduce_defender_count(rank_id);
	}
*/
}
//----------- End of function Unit::deinit_unit_mode -----------//


//--------- Begin of function Unit::select ---------//
//
// Select the current unit.
//
// [bool] mainSelect - set it to the main selected unit.
//							  (default: false)
//
void Unit::select(bool mainSelect)
{
	if( mainSelect )
		unit_array.selected_recno = sprite_recno;

	if( selected_flag )
		return;

	selected_flag = 1;
	unit_array.selected_count++;

	//--------------------------------//

	#ifdef DEBUG

	int selectedCount=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_truly_deleted(i) )
			continue;

		if( unit_array[i]->selected_flag )
			selectedCount++;
	}

	err_when( selectedCount != unit_array.selected_count );

	err_when( unit_array.selected_recno>0 && unit_array.selected_count==0 );

	#endif
}
//----------- End of function Unit::select -----------//


//--------- Begin of function Unit::deselect ---------//
//
// De-select the current unit and set unit_array.selected_recno to
// the highest rank unit.
//
void Unit::deselect()
{
	if( !selected_flag )
		return;

	selected_flag = 0;	// de-select it
	unit_array.selected_count--;

	err_when( unit_array.selected_count < 0 );

	if( power.command_unit_recno == sprite_recno )
		power.command_id = 0;

	if( unit_array.selected_recno == sprite_recno )		// if it is currently selected unit, reset selected_recno
	{
		unit_array.selected_recno = 0;

		//--- attempt to set selected_recno to one that is currently selected ---//

		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( unit_array.is_deleted(i) || i==sprite_recno )
				continue;

			//--- if there is none selected or this unit has a higher rank than the current selected one ---//

			if( unit_array[i]->selected_flag )
			{
				if( unit_array.selected_recno==0 ||
					 unit_array[i]->rank_id > unit_array[unit_array.selected_recno]->rank_id )
				{
					unit_array.selected_recno = i;
				}
			}
		}
	}

	//--------------------------------//

	#ifdef DEBUG

	int selectedCount=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_truly_deleted(i) )
			continue;

		if( unit_array[i]->selected_flag )
			selectedCount++;
	}

	err_when( selectedCount != unit_array.selected_count );

	err_when( unit_array.selected_recno>0 && unit_array.selected_count==0 );

	#endif
}
//----------- End of function Unit::deselect -----------//
