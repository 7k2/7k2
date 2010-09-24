//Filename	 : OUN_ACT.CPP
//Description: Unit class functions

#include <OCONFIG.H>
#include <OUNITRES.H>
#include <OSERES.H>
#include <ORACERES.H>
#include <OFIRMRES.H>
#include <OGODRES.H>
#include <ONATION.H>
#include <OPOWER.H>
#include <OUNIT.H>
#include <ITEMID.H>

//--------- Begin of function Unit::pre_process ---------//
//
void Unit::pre_process()
{
	//--- if the unit's hit point drops to 0, it dies now ---//

	if( hit_points <= 0 )
	{
		set_die();
		return;
	}

	//------- process fog of war ----------//

	visit_area();

	//------ process unit_mode -------//
#ifdef DEBUG
	long startTime;
#endif

	switch( unit_mode )
	{
		case UNIT_MODE_TOWN_DEFENDER:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			process_mode_town_defender();
#ifdef DEBUG
			unit_process_town_defender_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_MODE_CAMP_DEFENDER:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			process_mode_camp_defender();
#ifdef DEBUG
			unit_process_camp_defender_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_MODE_REBEL:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			process_mode_rebel();
#ifdef DEBUG
			unit_process_rebel_profile_time += m.get_time() - startTime;
#endif
			break;
	}

	//--- if the current order has been reset and there is a pushed order, pop the order ---//

	if( cur_order.mode==0 && has_pushed_order() )
   	pop_order();

	//-------- process unit order now -------//

	switch( cur_order.mode )
	{
		case UNIT_MOVE:		// nothing to do with move mode, as UnitB already takes care of it
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_move();
#ifdef DEBUG
			unit_execute_move_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_ATTACK:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_attack();
#ifdef DEBUG
			unit_execute_attack_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_BUILD_FIRM:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_build_firm();
#ifdef DEBUG
			unit_execute_build_firm_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_SETTLE_TOWN:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_settle_town();
#ifdef DEBUG
			unit_execute_settle_town_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_ASSIGN:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_assign();
#ifdef DEBUG
			unit_execute_assign_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_GO_CAST_POWER:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_go_cast_power();
#ifdef DEBUG
			unit_cast_power_profile_time += m.get_time() - startTime;
#endif
			break;

		case UNIT_TRANSFORM_FORTRESS:
#ifdef DEBUG
			startTime = m.get_time();
#endif
			execute_transform_fortress();
#ifdef DEBUG
			unit_transform_fortress_profile_time += m.get_time() - startTime;
#endif
			break;
	}
}
//--------- End of function Unit::pre_process ---------//

//--------- Begin of function Unit::stop_cur_order ---------//
//
// Stop the current order without completion.
//
void Unit::stop_cur_order()
{
	stop_move();

	cur_order.set(UNIT_STOP);
}
//--------- End of function Unit::stop_cur_order ---------//


//--------- Begin of function Unit::stop_order ---------//
//
// Stop the current order without completion.
//
void Unit::stop_order()
{
	stop_move();	// a UnitB function

	//----- if the order is initiated by an AI --//

	if( cur_order.ai_action_id && nation_recno )
		nation_array[nation_recno]->action_failure(cur_order.ai_action_id, sprite_recno);

	cur_order.set(UNIT_STOP);

	clear_pushed_order();			// clear pushed orders if there are any

	set_original_target(-1, -1);	// clear the original target

	force_move_flag = false;
}
//--------- End of function Unit::stop_order ---------//


//--------- Begin of function Unit::stop_move ---------//

void Unit::stop_move()
{
	ai_no_suitable_action = false;

	UnitB::stop_move();
}
//--------- End of function Unit::stop_move ---------//


//--------- Begin of function Unit::completed_order ---------//
//
// The current order is completed.
//
void Unit::completed_order()
{
	stop_move();		// a UnitB function

	//----- if the order is initiated by an AI --//

	if( cur_order.ai_action_id && nation_recno )
		nation_array[nation_recno]->action_finished(cur_order.ai_action_id, sprite_recno);

	cur_order.set(UNIT_STOP);
}
//--------- End of function Unit::completed_order ---------//



//--------- Begin of function Unit::push_order ---------//
//
// Note: the stack only has space for one Order object.
//
void Unit::push_order()
{
	err_when( has_pushed_order() > 0 );		// cannot push when the only one stack space is used

	pushed_order = cur_order;

	// ##### begin Gilbert 31/5 ########//
	// if the unit is stopped, store current location, used when resuming the action
	if( pushed_order.mode == UNIT_STOP )
	{
		pushed_order.set(UNIT_STOP, -2, next_x_loc(), next_y_loc() );
		// para = -2; to identify resume move, see execute_attack
	}
	// ##### end Gilbert 31/5 ########/

	cur_order.set(UNIT_STOP);

	stop_move();
}
//--------- End of function Unit::push_order ---------//


//--------- Begin of function Unit::pop_order ---------//
//
// Note: the stack only has space for one Order object.
//
void Unit::pop_order()
{
	stop_move();	// stop the current action first

	err_when( has_pushed_order() == 0 );		// cannot push when the only one stack space is used

	cur_order = pushed_order;

	pushed_order.set(-1);

	set_original_target(-1, -1);		// clear the original target
}
//--------- End of function Unit::pop_order ---------//


//--------- Begin of function Unit::has_pushed_order ---------//
//
// Return whether there is a pushed order in the stack.
//
int Unit::has_pushed_order()
{
	return pushed_order.mode >= 0;
}
//--------- End of function Unit::has_pushed_order ---------//


//--------- Begin of function Unit::clear_pushed_order ---------//
//
// Clear all pushed order if there is any.
//
void Unit::clear_pushed_order()
{
	pushed_order.set(-1);
}
//--------- End of function Unit::clear_pushed_order ---------//


//--------- Begin of function Unit::set_original_target ---------//
//
// Set original target location for checking when to abandon chasing
// the enemy.
//
void Unit::set_original_target(int xLoc, int yLoc)
{
	original_target_x_loc = xLoc;
	original_target_y_loc = yLoc;
}
//--------- End of function Unit::set_original_target ---------//


//--------- Begin of function UnitOrder::set ---------//
//
void UnitOrder::set(int newMode, int newPara, int newXLoc, int newYLoc)
{
	mode  = newMode;
	para  = newPara;
	loc_x = newXLoc;
	loc_y = newYLoc;

	ai_action_id = 0;
	name_id = 0;

	//--- set name_id to Unit::name_id if the order is attacking a unit. This is for verifying attack target ---//

	if( mode==UNIT_ATTACK )
	{
		BaseObj* targetObj = base_obj_array[para];

		if( targetObj->obj_type == OBJ_UNIT )
			name_id = targetObj->cast_to_Unit()->name_id;
	}
}
//--------- End of function UnitOrder::set ---------//


//--------- Begin of function UnitOrder::place_loc_x2 ---------//
//
// Return the loc_x2 of current destination specified by UnitOrder::loc_x1 && loc_y1
//
int UnitOrder::place_loc_x2()
{
	if( para==0 )
		return loc_x + STD_TOWN_LOC_WIDTH - 1;
	else
		return loc_x + firm_res[para]->loc_width - 1;
}
//--------- End of function UnitOrder::place_loc_x2 ---------//


//--------- Begin of function UnitOrder::place_loc_y2 ---------//
//
// Return the loc_y2 of current destination specified by UnitOrder::loc_x1 && loc_y1
//
int UnitOrder::place_loc_y2()
{
	if( para==0 )
		return loc_y + STD_TOWN_LOC_HEIGHT - 1;
	else
		return loc_y + firm_res[para]->loc_height - 1;
}
//--------- End of function UnitOrder::place_loc_y2 ---------//


//--------- Begin of function Unit::is_next_to_order_place ---------//
//
int Unit::is_next_to_order_place()
{
	return is_in_order_place(1);
}
//--------- End of function Unit::is_next_to_order_place ---------//

//--------- Begin of function Unit::is_in_order_place ---------//
//
// [int] extraLoc - 1 if called by is_next_to_order_place()
//					     in a normal call, this is not passed.
//
int Unit::is_in_order_place(int extraLoc)
{
	if( cur_order.mode == UNIT_BUILD_FIRM ||
		 cur_order.mode == UNIT_SETTLE_TOWN )
	{
		// ###### begin Gilbert 4/11 ######//
//		return next_x_loc() >= cur_order.loc_x-extraLoc &&
//				 next_x_loc() <= cur_order.place_loc_x2()+extraLoc &&		// either inside the building site or just next to it
//				 next_y_loc() >= cur_order.loc_y-extraLoc &&
//				 next_y_loc() <= cur_order.place_loc_y2()+extraLoc;
		return m.area_distance( obj_loc_x1(), obj_loc_y1(), obj_loc_x2(), obj_loc_y2(),
			cur_order.loc_x, cur_order.loc_y, 
			cur_order.place_loc_x2(), cur_order.place_loc_y2() ) <= extraLoc;
		// ###### end Gilbert 4/11 ######//
	}
	else if( cur_order.mode == UNIT_ASSIGN )
	{
		BaseObj* baseObj = base_obj_array[cur_order.para];

		// ###### begin Gilbert 4/11 ######//
//		return next_x_loc() >= baseObj->obj_loc_x1()-extraLoc &&
//				 next_x_loc() <= baseObj->obj_loc_x2()+extraLoc &&
//				 next_y_loc() >= baseObj->obj_loc_y1()-extraLoc &&
//				 next_y_loc() <= baseObj->obj_loc_y2()+extraLoc;
		return area_distance(baseObj) <= extraLoc;
		// ###### end Gilbert 4/11 ######//
	}
	// ####### begin Gilbert 19/2 #######//
	else if( cur_order.mode == UNIT_GO_CAST_POWER )
	{
		return m.area_distance( obj_loc_x1(), obj_loc_y1(), obj_loc_x2(), obj_loc_y2(),
			cur_order.loc_x, cur_order.loc_y, cur_order.loc_x, cur_order.loc_y ) <= extraLoc;
	}
	// ####### end Gilbert 19/2 #######//
	else
	{
		err_here();
		return 0;
	}
}
//--------- End of function Unit::is_in_order_place ---------//

//--------- Begin of function Unit::left_click ---------//
//
// This function is called when the mouse left-click on this unit.
//
// <int> shiftPressed - whether the shift key is pressed.
//
void Unit::left_click(int shiftPressed)
{
	//--------------------------------------//

	if( selected_flag )		// if this unit is currently selected
	{
		if( shiftPressed )	// if the shift key is pressed
			deselect();
	}
	else							// if this unit is currently NOT selected
	{
		select();
	//	selected_flag = 1;
	//	unit_array.selected_count++;

		//--- if there is none selected or this unit has a higher rank than the current selected one ---//

		if( unit_array.selected_recno==0 ||
			 rank_id > unit_array[unit_array.selected_recno]->rank_id )
		{
			unit_array.selected_recno = obj_extern_recno;
		}
	}
	err_when( unit_array.selected_recno>0 && unit_array.selected_count==0 );
}
//--------- End of function Unit::left_click ---------//


//--------- Begin of function Unit::select_team ---------//
//
// Select all members in the team.
//
int Unit::select_team()
{
	if( !in_team() )
		return 0;

	//------- select the team leader ---------//

	int leaderUnitRecno;

	if( leader_unit_recno )
		leaderUnitRecno = leader_unit_recno;
	else
		leaderUnitRecno = sprite_recno;

	if(unit_array[leaderUnitRecno]->is_visible())
		unit_array[leaderUnitRecno]->left_click(0);

	//----- select all members in the team ------//

	TeamInfo* teamInfo = unit_array[leaderUnitRecno]->team_info;

	for( int i=teamInfo->member_count-1 ; i>=0 ; i-- )
	{
		int unitRecno = teamInfo->member_unit_array[i];

		if( unit_array.is_deleted(unitRecno) )
			continue;

		if(unit_array[unitRecno]->is_visible())
			unit_array[unitRecno]->left_click(0);
	}

	return 1;
}
//--------- End of function Unit::select_team ---------//


//--------- Begin of function Unit::gain_experience ---------//

void Unit::gain_experience()
{
	//#### begin Gilbert 22/4 ######//

//	if( !race_id )
//		return; 		// no experience gain if unit is a living being

	UnitInfo *unitInfo = unit_res[unit_id];

	//---- increase the unit's contribution to the nation ----//

	if( !unitInfo->class_info.has_weapon_version		// get_weapon_level() share nation_contribution
		&& unitInfo->class_info.has_loyalty )
	{
		if( nation_contribution < MAX_NATION_CONTRIBUTION )
		{
			nation_contribution++;

			err_when( nation_contribution < 0 );		// overflow
		}
	}

	//------ increase combat skill -------//

	if( unitInfo->class_info.has_combat_level )
	{
		// test raw combat level
		err_when( skill.actual_combat_level()<0 || skill.actual_combat_level(NULL)>1000);

		skill.inc_combat_level(0.10f);

		// ------ effect of god ----------//

		// ##### patch begin Gilbert 16/2 #######//
		//if( race_id == RACE_CELTIC && nation_recno
		//	&& god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
		if( nation_recno && god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
		{
			if( race_id == RACE_CELTIC )
				skill.inc_combat_level(0.10f);		// skill increase greatly in battle
			else
				skill.inc_combat_level(0.04f);		// skill increase greatly in battle
		}
		// ##### patch end Gilbert 16/2 #######//
	}

	//--- if this is a soldier led by a commander, increase the leadership of its commander -----//

	if( !unit_array.is_deleted(leader_unit_recno) )
	{
		Unit* leaderUnit = unit_array[leader_unit_recno];
		int	leaderXLoc= -1, leaderYLoc;

		if( leaderUnit->is_visible() )
		{
			leaderXLoc = cur_x_loc();
			leaderYLoc = cur_y_loc();
		}
		else if( leaderUnit->unit_mode == UNIT_MODE_OVERSEE )
		{
			Firm* firmPtr = firm_array[leaderUnit->unit_mode_para];

			leaderXLoc = firmPtr->center_x;
			leaderYLoc = firmPtr->center_y;
		}
		else
			leaderXLoc = -1;

		if( leaderXLoc >= 0 &&
			 m.points_distance( cur_x_loc(), cur_y_loc(), leaderXLoc, leaderYLoc ) <= EFFECTIVE_LEADING_DISTANCE )
		{
			leaderUnit->skill.inc_skill_level(0.06f);

			//-- give additional increase if the leader has skill potential on leadership --//

			if( leaderUnit->skill.skill_potential > 0 )
			{
				if( m.random(10-leaderUnit->skill.skill_potential/10)==0 )
					leaderUnit->skill.inc_skill_level(0.12f);
			}
		}

		//--- if this soldier has leadership potential and is led by a commander ---//
		//--- he learns leadership by watching how the commander commands the troop --//

		if( unitInfo->class_info.has_skill_level )
		{
			if( skill.skill_potential > 0 )
			{
				if( m.random(10-skill.skill_potential/10)==0 )
					skill.inc_skill_level(0.12f);
			}
		}
	}

	//#### end Gilbert 22/4 ######//
}
//------------ End of function Unit::gain_experience --------------//


//----------- Begin of function Unit::return_camp -----------//
//
// Order this unit to return to the camp. For ordering many
// units to return to a camp, UnitArray::return_camp() should
// be called instead.
//
int Unit::return_camp()
{
	if( !home_camp_firm_recno )
		return 0;

	err_when( firm_array.is_deleted(home_camp_firm_recno) );

	Firm* firmPtr = firm_array[home_camp_firm_recno];

	if( firmPtr->region_id != region_id() )
		return 0;

	err_when( !firmPtr->cast_to_FirmCamp() && !firmPtr->cast_to_FirmMonsterFortress() );
	err_when( firmPtr->nation_recno != nation_recno );

	//--------- assign now ---------//

	// ##### begin Gilbert 2/6 #########//
	assign(firmPtr->loc_x1, firmPtr->loc_y1, -1, true);

//	force_move_flag = 1;
	// ##### end Gilbert 2/6 #########//

	return cur_action != SPRITE_IDLE;
}
//----------- End of function Unit::return_camp -----------//


//------------- Begin of function Unit::set_die --------------//
//
// set parameters for unit die
//
void Unit::set_die()
{
	if( cur_action == SPRITE_DIE )
		return;

	stop_order();

	err_when(hit_points>0);

	cur_action  = SPRITE_DIE;
	cur_frame   = 1;

	//---- if this unit is led by a leader, only mobile units has leader_unit_recno assigned to a leader -----//

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
}
//----------------- End of function Unit::set_die ----------------//


//------------- Begin of function Unit::sound --------------//
//
void Unit::sound(char* soundCode)
{
	if( se_res.mark_command_time() )
	{
		se_res.far_sound( next_x_loc(), next_y_loc(), 1,
			'S', sprite_id, soundCode );
	}
}
//----------------- End of function Unit::sound ----------------//


//--------- Begin of function Unit::is_all_stop --------//

int Unit::is_all_stop()
{
	return cur_action==SPRITE_IDLE && cur_order.mode==UNIT_STOP &&
			 !has_pushed_order();
}
//---------- End of function Unit::is_all_stop --------//

//---------- Begin of function Unit::freeze -----------//
void Unit::freeze(int freezeFrame)
{
//	saved_action = cur_action;
//	saved_order = cur_order;

//	stop_order();
//	cur_action = SPRITE_IDLE;
//	check_freezed = true;
	if(freeze_frame_left < freezeFrame)
		freeze_frame_left = freezeFrame;
}
//---------- End of function Unit::freeze -------------//

//---------- Begin of function Unit::unfreeze ---------//
/*
void Unit::unfreeze()
{
//	cur_action = saved_action;
//	cur_order = saved_order;
//	check_freezed = false;
}
*/
//---------- End of function Unit::unfreeze -----------//


// --------- Begin of function Unit::teleport --------//
// return 1 on success
int Unit::teleport(int targetXLoc, int targetYLoc )
{
	// ###### begin Gilbert 21/3 ########//
	if( !is_visible() )
		return 0;
	// ###### end Gilbert 21/3 ########//

	if( world.check_unit_space( targetXLoc, targetYLoc, 
		targetXLoc+loc_width-1, targetYLoc+loc_height-1, mobile_type) )
	{
		stop_order();

		int dx, dy;
		for( dy = 0; dy < loc_width; ++dy )
			for( dx = 0; dx < loc_height; ++dx )
				world.get_loc(next_x_loc()+dx, next_y_loc()+dy)->remove_unit( mobile_type );
		
		cur_x = go_x = next_x = targetXLoc * LOCATE_WIDTH;
		cur_y = go_y = next_y = targetYLoc * LOCATE_HEIGHT;

		for( dy = 0; dy < loc_width; ++dy )
			for( dx = 0; dx < loc_height; ++dx )
				world.get_loc( next_x_loc()+dx, next_y_loc()+dy)->set_unit( sprite_recno, mobile_type );

		explore_area();

		return 1;
	}

	return 0;
}
// --------- End of function Unit::teleport --------//
