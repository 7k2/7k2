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

//Filename:    OPE_AI2.CPP
//Description: Unit Basic AI function - for both player and AI units.

#include <OSYS.H>
#include <OSPY.H>
#include <OWORLD.H>
#include <ONATION.H>
#include <OFIRM.H>
#include <OREBEL.H>
#include <OTOWN.H>
#include <OUNIT.H>
#include <OPFIND.H>
#include <OW_TRV.H>

//------- Begin of function Unit::think_aggressive_action --------//
//
// This function is called when the unit is in aggressive mode.
//
int Unit::think_aggressive_action()
{
	if( !can_attack() )
		return 0;

	if( has_pushed_order() || force_move_flag )		// can only push once, so if there is already a pushed order, we cannot create a new order to a new target
		return 0;

	//--- Locate for enemies to attack if the unit is idle ---//

	if( is_all_stop() )
	{
		return think_aggressive_attack();
	}

	//-- if this unit is attacking a firm or a town, think about change to attack a unit --//

	if( cur_order.mode==UNIT_ATTACK )	// only in attack mode, as if the unit is still moving the target may be far away from the current position
//	if( cur_action==SPRITE_ATTACK )		// only in attack mode, as if the unit is still moving the target may be far away from the current position
	{
		//--- if the unit is currently attacking a town or a firm ---//

		BaseObj* baseObj;

		if( (is_ai || m.random(2) ) &&			// only AI unit will do so, human units will keep attacking the target no matter what it is.
			 !base_obj_array.is_deleted(cur_order.para) &&
			 !(baseObj = base_obj_array[cur_order.para])->obj_can_attack(0) )      // if the target structure can't attack, then take care of it later, attack the enemy units first
		{
			//--- if the target is town and the town has defenders out, attack it has no effect, so attack the defenders ---//

			if( baseObj->cast_to_Town() && baseObj->cast_to_Town()->town_defender_count > 0 )
			{
				//-- increase the attack scan range so that the chance of hitting defenders is higher --//

				return think_aggressive_attack(8);
			}
			else
			{
				return think_aggressive_attack();
			}
		}
	}

	return 0;
}
//-------- End of function Unit::think_aggressive_action --------//


//------- Begin of function Unit::think_aggressive_attack -------//
//
// When the unit is attacking a firm or town, look out for enemy units
// to attack. Enemy units should be attacked first.
//
// [int] extendedRange - extended attack scan range (default: 0)
// [int] fullDetect    - if this is 1, the unit will scan a full
//								 range to detect enemy as opposite to
//								 detect a bit each time. (default: 0)
//
int Unit::think_aggressive_attack(int extendedRange, int fullDetect)
{
	enum { DETECT_INTERVAL_DIVIDER = 3 };		// it takes 3 days to complete a single detect action

	if( !fullDetect && (info.game_date + sprite_recno) % DETECT_INTERVAL_DIVIDER != 0 )
		return 0;

	int attackRange = max_attack_range();
	// old version attackScanRange is diammeter, but new version is radius
	int attackScanRange = max( attackRange, 8 ) + extendedRange + 4;

	//-- the attack scanning range of town and camp defenders is larger --//

	if( unit_mode==UNIT_MODE_TOWN_DEFENDER ||
		 unit_mode==UNIT_MODE_CAMP_DEFENDER )
	{
		attackScanRange += 4;
	}

	int curXLoc = next_x_loc(), curYLoc = next_y_loc();
	BYTE regionId = world.get_region_id(curXLoc, curYLoc);

	WorldScanTraverser scanner;
	scanner.init( curXLoc, curYLoc, loc_width, attackScanRange );

	if( scanner.scan_count == 0 )
		return 0;

	// ------- find pass, filter target  ---------- //

	BaseObj *chosenTarget = NULL;

	for( scanner.restart(); !scanner.is_finish(); scanner.next() )
	{
		Unit *targetUnit = unit_array[scanner.get_unit()];

		int targetXLoc = targetUnit->next_x_loc();
		int targetYLoc = targetUnit->next_y_loc();

		if( (mobile_type == UNIT_AIR || world.get_loc(targetXLoc, targetYLoc)->region_id == regionId)
			&& targetUnit->nation_recno != nation_recno
			&& should_aggressive_attack(targetUnit->base_obj_recno)
			&& !targetUnit->is_camouflage(nation_recno) )	// don't use is_stealth
		{
			// if find one in attack range, attack immediately

			int areaDistance = area_distance(targetUnit);
			if( areaDistance <= attackRange )
			{
				chosenTarget = targetUnit;
				break;
			}
			else
			{
				// ##### begin Gilbert 31/5 ##########//
				// if unit is moving away, don't chase
				// assume moving away if the target is moving in 
				// 3 directions away of this unit
				if( targetUnit->cur_action == SPRITE_MOVE 
					&& m.abs_direction_diff( targetUnit->cur_dir & 7,
					get_dir(cur_x, cur_y, targetUnit->cur_x, targetUnit->cur_y)) <= 1 )
				{
					scanner.remove();
					continue;		// moving away, never think it
				}
				else
				{
					scanner.set_weight( areaDistance );
				}
				// ##### end Gilbert 31/5 ##########//
			}
		}
		else
		{
			scanner.remove();
			continue;
		}
	}


	// -------- pass two, assign weight (path distance)------------//

	if( !chosenTarget )
	{
		scanner.pack_removed();
		scanner.sort_by_weight();

		// find the minimum path_steps

		int minSteps = MAX_WORLD_X_LOC + MAX_WORLD_Y_LOC;

		for( scanner.restart(); !scanner.is_finish(); scanner.next() )
		{
			if( !scanner.get_unit() )
				continue;

			Unit *targetUnit = unit_array[scanner.get_unit()];

			// weight is the area distance
			// if the distance is already longer than minimum path steps
			// the path steps is also longer than minimun path steps
			if( scanner.get_weight() >= minSteps )
				continue;

			// find the path distance
			path_finder.set_attribute( loc_width, loc_height, mobile_type, 0 );
			path_finder.set_handle_power_nation(0);
			if( path_finder.find_path( next_x_loc(), next_y_loc(), targetUnit->obj_loc_x1(), targetUnit->obj_loc_y1(),
				targetUnit->obj_loc_x1(), targetUnit->obj_loc_y1(), targetUnit->obj_loc_x2(), targetUnit->obj_loc_y2(),
				1, 0, attackScanRange*4, attackRange ) )
			{
				chosenTarget = targetUnit;
				minSteps = path_finder.path_step();
			}
		}
	}

	if( chosenTarget)
	{
		if( !has_pushed_order() )		// if it already has a pushed order, don't push it again 
			push_order();

		set_original_target(chosenTarget->obj_loc_x1(), chosenTarget->obj_loc_y1());			// for unit to determine when to stop, we must set it before calling attack() as attack() will call validate_attack_target() which will check original_target_?_loc

		attack(chosenTarget->base_obj_recno, false);

		if( cur_order.mode != UNIT_ATTACK )		// if the attack action failed, resume order
		{
			pop_order();
			return 0;
		}

		return 1;
	}

	return 0;



	/*
	//----------------------------------------------//

	int attackRange	  = max(attack_range(), 8);
	int attackScanRange = attackRange*2+1;

	attackScanRange += extendedRange;

	//-- the attack scanning range of town and camp defenders is larger --//

	if( unit_mode==UNIT_MODE_TOWN_DEFENDER ||
		 unit_mode==UNIT_MODE_CAMP_DEFENDER )
	{
		attackScanRange += 8;
	}

	//--------------------------------------//

	int		 xOffset, yOffset;
	int		 xLoc, yLoc;
	Location* locPtr;
	int		 curXLoc = next_x_loc(), curYLoc = next_y_loc();
	BYTE	 	 regionId = world.get_region_id(curXLoc, curYLoc);

	//---- set the starting and endding point for the detect action ----//

	enum { DETECT_INTERVAL_DIVIDER = 6 };		// it takes 6 days to complete a single detect action

	int totalPoint = attackScanRange*attackScanRange;
	int startPoint, endPoint;

	if( fullDetect )
	{
		startPoint = 1;
		endPoint   = totalPoint;
	}
	else
	{
		int detectInterval = totalPoint / DETECT_INTERVAL_DIVIDER;

		startPoint = detectInterval * ( (info.game_date-last_attack_date) % DETECT_INTERVAL_DIVIDER);		//	use (info.game_date-last_attack_date) so that the unit always look for nearest enemies first
		endPoint   = startPoint + detectInterval - 1;

		if( sys.frame_count % DETECT_INTERVAL_DIVIDER == DETECT_INTERVAL_DIVIDER-1 )		// if this is last in the series
			endPoint = totalPoint;
   }

	//-------------------------------------------//

	for( int i=startPoint ; i<=endPoint ; i++ )
	{
		m.cal_move_around_a_point(i, attackScanRange, attackScanRange, xOffset, yOffset);

		xLoc = curXLoc + xOffset;
		yLoc = curYLoc + yOffset;

		xLoc = max(0, xLoc);
		xLoc = min(MAX_WORLD_X_LOC-1, xLoc);

		yLoc = max(0, yLoc);
		yLoc = min(MAX_WORLD_Y_LOC-1, yLoc);

		locPtr = world.get_loc(xLoc, yLoc);

		if( locPtr->region_id != regionId )
			continue;

		//----- if there is a unit on the location ------//

		int mobileType;
		int unitRecno = locPtr->get_any_unit(mobileType);

		if( unitRecno )
		{
			if( unit_array.is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = unit_array[unitRecno];

			if( unitPtr->nation_recno != nation_recno &&
				 should_aggressive_attack(unitPtr->base_obj_recno) )
			{
				if( !has_pushed_order() )		// if it already has a pushed order, don't push it again 
					push_order();

				set_original_target(xLoc, yLoc);			// for unit to determine when to stop, we must set it before calling attack() as attack() will call validate_attack_target() which will check original_target_?_loc

				attack(unitPtr->base_obj_recno, false);

				if( cur_order.mode != UNIT_ATTACK )		// if the attack action failed, resume order
				{
					pop_order();
					return 0;
				}

				return 1;
			}
		}
	}
	*/

	return 0;
}
//---------- End of function Unit::think_aggressive_attack ------//


//--------- Begin of function Unit::should_aggressive_attack --------//
//
// Whether this unit should aggressively attack the specified target.
//
int Unit::should_aggressive_attack(int targetObjRecno)
{
	if( base_obj_array.is_deleted(targetObjRecno) )
		return 0;

	if( !can_attack_target(targetObjRecno) )
		return 0;

	// ##### begin Gilbert 13/5 #########//
	// independent hero should not attack aggressive
	if( !nation_recno && hero_id )
	{
		return 0;
	}
	// ##### end Gilbert 13/5 #########//

	//--- if the attacker and the target are both not independent, only attack when the two are hostile ---//

	BaseObj* targetObj = base_obj_array[targetObjRecno];
	int targetNationRecno = targetObj->nation_recno;

	if( nation_recno && targetNationRecno )
	{
		NationRelation* nationRelation = nation_array[nation_recno]->get_relation(targetNationRecno);

		if( nationRelation->status != RELATION_HOSTILE || !nationRelation->should_attack )
			return 0;
	}

	//---- if the unit is in defense mode, do not check the following ----//

	if( unit_mode!=UNIT_MODE_TOWN_DEFENDER && unit_mode!=UNIT_MODE_CAMP_DEFENDER )
	{
		//------ do not attack if the target is a civilian ----//

		Unit* unitPtr = targetObj->cast_to_Unit();

		if( unitPtr &&
			 unitPtr->is_civilian() &&			// don't attack civilian and independent units unless they are
			 unitPtr->unit_mode != UNIT_MODE_TOWN_DEFENDER &&
			 unitPtr->unit_mode != UNIT_MODE_REBEL )
		{
			return 0;
		}

		//------- if the target unit is an independent unit ----//

		if( unitPtr->nation_recno==0 )
		{
			int rc=0;

			if( nation_recno )		// and this unit is a nation unit
			{
				//--- if it is a town defender, only attack it if we are attacking them ---//

				if( unitPtr->unit_mode == UNIT_MODE_TOWN_DEFENDER )
				{
					if( town_array[unitPtr->unit_mode_para]->last_attack_nation_recno == nation_recno )
						rc = 1;
				}

				//--- if this is a rebel, only attack it if we are already hostile with the rebel group ---//

				if( unitPtr->unit_mode == UNIT_MODE_REBEL )
				{
					if( rebel_array[unitPtr->unit_mode_para]->is_hostile_nation(nation_recno) )
						rc = 1;
				}
			}

			if( !rc )
				return 0;
		}

		//----- in Defensive or Stand Ground mode, only attack unit standing next to itself ----//

		if( get_behavior_mode() != UNIT_AGGRESSIVE && !unit_mode )
		{
			// ####### begin Gilbert 31/5 ##########//
//			int attackDistance = cal_distance( targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
//										targetObj->obj_loc_width(), targetObj->obj_loc_height() );
//			if( attackDistance > max_attack_range() )
//				return 0;
			if( !is_visible() || !targetObj->is_visible() )	// avoid invisible unit causes exception in area_distance
				return 0;
			if( is_die_after_attack() || area_distance(targetObj) > max_attack_range() )
				return 0;
			// ####### end Gilbert 31/5 ##########//
		}
	}

	//-------------------------------------------//
	// If the target is a spy of our own and the
	// notification flag is set to 0, then don't
	// attack.
	//-------------------------------------------//

	if( targetObj->obj_type == OBJ_UNIT )
	{
		Unit* targetUnitPtr = targetObj->cast_to_Unit();

		if( targetUnitPtr->spy_recno )		// if the target unit is our spy, don't attack
		{
			Spy* spyPtr = spy_array[targetUnitPtr->spy_recno];

			if( spyPtr->true_nation_recno == nation_recno &&
				 spyPtr->notify_cloaked_nation_flag == 0 )
			{
				return 0;
			}
		}

		if( spy_recno )			// if this unit is our spy, don't attack own units
		{
			Spy* spyPtr = spy_array[spy_recno];

			if( spyPtr->true_nation_recno == targetUnitPtr->nation_recno &&
				 spyPtr->notify_cloaked_nation_flag == 0 )
			{
				return 0;
			}
		}
	}

	//--- test if the unit can move close enough to the target to attack it ---//

	find_new_location_for_attack(targetObj);

	return attack_loc_offset_x != 0 ;		// if attack_loc_offset_x == 0, that means no suitable location is found
}
//----------- End of function Unit::should_aggressive_attack -----------//


