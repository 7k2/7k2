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

//Filename	 : OPER_AT.CPP
//Description: Unit attacking functions
 
#include <windows.h>
#include <OREBEL.H>
#include <OUNITRES.H>
#include <OBULLET.H>
#include <ONATION.H>
#include <OMONSRES.H>
#include <OU_WAGON.H>
#include <OHERORES.H>
#include <ITEMID.H>
#include <OF_FORT.H>
#include <OU_CART.H>
#include <OW_TRV.H>
#include <OUN_SPAB.H>
#include <OEFFECT.H>


// similar to MAX_UNIT_WAIT_STATE, but for attack, smaller for faster response
// #define MAX_UNIT_WAIT_STATE_ATTACK 2
#define MAX_ATTACK_HELPER 8
#define MAX_HELPER_DISTANCE 8


//--------- Begin of function Unit::execute_attack ---------//
//
// This function is called by Unit::pre_process().
//
void Unit::execute_attack()
{
	//---- stop action if target goes into town/firm or it no longer exists ---//

	if( !validate_attack_target() )
	{
		if( has_pushed_order() )
		{
			//--- if the unit's order is to attack a town, look for defenders to attack first before going back to the town ---//

			if( pushed_order.mode==UNIT_ATTACK )
			{
				if( !base_obj_array.is_deleted(pushed_order.para) )
				{
					BaseObj* baseObj = base_obj_array[pushed_order.para];

					if( baseObj->cast_to_Town() && baseObj->cast_to_Town()->town_defender_count > 0 )
					{
						if( think_aggressive_attack(8, 1) )		// detect full range
							return;
					}
				}
			}

			//------------------------------//

			pop_order();
		}
		else
			stop_order();

		return;
	}

	//----- check if the target has moved. If so, call attack() again ----//

	BaseObj *targetObj = base_obj_array[cur_order.para];

	if( targetObj->obj_loc_x1() != cur_order.loc_x ||
		 targetObj->obj_loc_y1() != cur_order.loc_y )
	{
		// call target_move not too frequent, especially at long distance

		target_move(targetObj);
//		if(cur_order.mode == UNIT_STOP)
			return;
	}

	//---- if the unit is not attacking yet ------//

	if( cur_action != SPRITE_ATTACK )
	{
		if(cur_action == SPRITE_WAIT)
		{
			// ####### begin Gilbert 28/4 #######//
			if( wait_state > (retry_state <= MAX_UNIT_WAIT_STATE ? retry_state+MAX_UNIT_WAIT_STATE_FIRST : MAX_UNIT_WAIT_STATE) )
			{
				wait_state = 0;
				
				int backupRetryState = retry_state;
				attack(cur_order.para, false);
				retry_state = backupRetryState + 1;			// restore and increase
			}
			// ####### end Gilbert 28/4 #######//
		}
		
		// ####### begin Gilbert 3/5 #######//
		// if( abs(cur_x-next_x) <= sprite_info->speed &&
		//	 abs(cur_y-next_y) <= sprite_info->speed )
		if( abs(cur_x-next_x) <= sprite_speed() &&
			abs(cur_y-next_y) <= sprite_speed() &&
			(!sprite_info->need_turning || cur_dir==final_dir) )		// for turning
		// ####### end Gilbert 3/5 #######//
		{
			int attackDistance = area_distance( targetObj );

			//---- if the target is within the attack range -----//

			/*
			if( attackDistance <= attack_range() )
			{
				set_cur(next_x, next_y);
				stop_move();
				if(attack_range() >1 )
					set_attack_dir(next_x_loc(), next_y_loc(), range_attack_x_loc, range_attack_y_loc);
				else
				{
					int closeAttackLocX = targetObj->obj_loc_x1();
					int closeAttackLocY = targetObj->obj_loc_y1();
					close_attack_location(targetObj, closeAttackLocX, closeAttackLocY);
					set_attack_dir(next_x_loc(), next_y_loc(), closeAttackLocX, closeAttackLocY);
				}
				if(is_dir_correct() || (mobile_type == UNIT_AIR && !attackDistance))
					attack_now(targetObj, attackDistance);
				else
					set_turn();
				return;
			}
			*/

			// ######## begin Gilbert 3/5 #########//
//			if(attackDistance <= max_attack_range() )


			AttackInfo *attackInfo = cur_attack_info();
			if( attackDistance <= attackInfo->attack_range )
			{
				// add_bullet_possible checking was made in attack_now
				// but do it now

				if( attackInfo->bullet_sprite_id &&
					!bullet_array.add_bullet_possible( cur_x_loc(), cur_y_loc(), mobile_type, targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
					targetObj->obj_mobile_type(), targetObj->obj_loc_width(), targetObj->obj_loc_height(),
					range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id, this) )
				{
					find_new_position_for_range_attack(targetObj);

					err_when(targetObj->obj_loc_x1()+attack_loc_offset_x < 0);
					err_when(targetObj->obj_loc_x1()+attack_loc_offset_x+obj_loc_width()-1 >= MAX_WORLD_X_LOC);
					err_when(targetObj->obj_loc_y1()+attack_loc_offset_y < 0);
					err_when(targetObj->obj_loc_y1()+attack_loc_offset_y+obj_loc_height()-1 >= MAX_WORLD_Y_LOC);
					err_when((attack_loc_offset_x || attack_loc_offset_y) && (!bullet_array.add_bullet_possible(targetObj->obj_loc_x1()+attack_loc_offset_x, targetObj->obj_loc_y1()+attack_loc_offset_y, mobile_type, targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
					 targetObj->obj_mobile_type(), targetObj->obj_loc_width(), targetObj->obj_loc_height(),
					 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id)));

					if(attack_loc_offset_x ||  attack_loc_offset_y)
						move_to(targetObj->obj_loc_x1()+attack_loc_offset_x, targetObj->obj_loc_y1()+attack_loc_offset_y);
					else
						stop_move();
				}
				else
					attack_now(targetObj, attackDistance);
			}
			// ######## end Gilbert 3/5 #########//

			//--- if the target is out of the attack range ----//

			else if( cur_action == SPRITE_IDLE )	// if the sprite is currently idle, search path and move again
			{
				attack( cur_order.para, false );
			}
		}
	}
}
//--------- End of function Unit::execute_attack ---------//

//--------- Begin of function Unit::attack ---------//
//
// <int>  targetRecno      		- recno of the target (the target should be a BaseObj class)
// <bool> resetOrder             - reset the current and the pushed order if there is any
//											  true if this is a player action or a high level AI action
//											  false if this is a low level AI reaction and aggression action
//
void Unit::attack(short targetRecno, bool resetOrder)
{
	if( !set_attack(targetRecno, resetOrder) )
		return;

	//--------------------------------------//
#ifdef	DEBUG
	unsigned long startTime = m.get_time();
#endif
	BaseObj *targetObj = base_obj_array[targetRecno];

	int	targetXLoc = targetObj->obj_loc_x1();
	int	targetYLoc = targetObj->obj_loc_y1();
	int	targetWidth = targetObj->obj_loc_width();
	int targetHeight = targetObj->obj_loc_height();
	int targetCenterX = (targetXLoc+targetObj->obj_loc_x2())/2;
	int targetCenterY = (targetYLoc+targetObj->obj_loc_y2())/2;
	
	err_when(targetXLoc < 0);
	err_when(targetXLoc+targetWidth-1 >= MAX_WORLD_X_LOC);
	err_when(targetYLoc < 0);
	err_when(targetYLoc+targetHeight-1 >= MAX_WORLD_Y_LOC);

	int attackDistance = area_distance( targetObj );

	//----- if the target is outside the attack range, the unit must move close to the target -----//
/*
	if( attackDistance > attack_range() )
	{
		err_when(attackDistance<=1);

		if(attack_loc_offset_x || attack_loc_offset_y)
		{
			if(!can_move(targetXLoc+attack_loc_offset_x, targetYLoc+attack_loc_offset_y))
				find_new_location_for_attack(targetObj);

			err_when(targetXLoc+attack_loc_offset_x < 0);
			err_when(targetXLoc+attack_loc_offset_x+obj_loc_width()-1 >= MAX_WORLD_X_LOC);
			err_when(targetYLoc+attack_loc_offset_y < 0);
			err_when(targetYLoc+attack_loc_offset_y+obj_loc_height()-1 >= MAX_WORLD_Y_LOC);

			if((cur_action != SPRITE_ATTACK) && (attack_loc_offset_x || attack_loc_offset_y) && ((next_x_loc() != targetXLoc+attack_loc_offset_x) || (next_y_loc() != targetYLoc+attack_loc_offset_y)))
				move_to(targetXLoc+attack_loc_offset_x, targetYLoc+attack_loc_offset_y);
			//	move(targetXLoc, targetYLoc);
			else if(cur_action != SPRITE_ATTACK)
				stop_move();
		}
		else//when there is not a pre-assigned location for attacking the target
		{
			find_new_location_for_attack(targetObj);

			err_when(targetXLoc+attack_loc_offset_x < 0);
			err_when(targetXLoc+attack_loc_offset_x+obj_loc_width()-1 >= MAX_WORLD_X_LOC);
			err_when(targetYLoc+attack_loc_offset_y < 0);
			err_when(targetYLoc+attack_loc_offset_y+obj_loc_height()-1 >= MAX_WORLD_Y_LOC);

			if(attack_loc_offset_x || attack_loc_offset_y)
				move_to(targetXLoc+attack_loc_offset_x, targetYLoc+attack_loc_offset_y);
			//	move(targetXLoc, targetYLoc);
			else
				stop_move();
		}
	}
*/
	if( attackDistance > max_attack_range() )
	{
		//-- don't call move_to() too frequently. If the previous path search fails, wait 10 days before doing a new one.

		if( seek_path_fail_count==0 )
			move_to(targetCenterX, targetCenterY);
		else
			seek_path_fail_count--;
	}

#ifdef DEBUG
	unit_attack_profile_time += m.get_time()-startTime;
#endif
}
//----------- End of function Unit::attack -----------//

//----------- Begin of function Unit::set_attack------//
//
// This function is called by UnitGroup to set unit to
// attack mode without having it start moving.
//
// It is also called by Unit::attack().
//
// <int>  targetRecno      		- recno of the target (the target should be a BaseObj class)
// <bool> resetOrder             - reset the current and the pushed order if there is any
//											  true if this is a player action or a high level AI action
//											  false if this is a low level AI reaction and aggression action
//
int Unit::set_attack(short targetRecno, bool resetOrder)
{
	if( !can_attack() )		// if the unit does not have attacking ability, return now. 
		return 0;

	if( resetOrder )
		stop_order();
	else
		stop_cur_order();

	if( is_dead() )		// if the unit is dead or dying
		return 0;

	//---------------------------------------------//

	if( base_obj_array.is_deleted(targetRecno) )
		return 0;

	BaseObj *targetObj = base_obj_array[targetRecno];

	char	targetMobileType = targetObj->obj_mobile_type();
	int	targetXLoc 		  = targetObj->obj_loc_x1();
	int	targetYLoc 		  = targetObj->obj_loc_y1();

	//------- set action mode -------//

	cur_order.set( UNIT_ATTACK, targetRecno, targetXLoc, targetYLoc );

	// ###### begin Gilbert 16/4 #########//
	// validate_attack_target look at original_target_x_loc 
	set_original_target(targetXLoc, targetYLoc);
	// ###### end Gilbert 16/4 #########//

	//------ validate the attack --------//

	if( !validate_attack_target() )		// this function must be called after setting the order.
	{
		stop_cur_order();
		return 0;
	}

	//------- select the best attack mode --------//

	// #### begin Gilbert 4/11 ######//
	// int attackDistance = cal_distance(targetXLoc, targetYLoc,
	//							targetObj->obj_loc_width(), targetObj->obj_loc_height());
	int attackDistance = area_distance( targetObj );
	// #### end Gilbert 4/11 ######//

	choose_best_attack_mode(attackDistance, targetMobileType);

	//---- set ai_original_target_?_loc if this is an AI unit ----//

	if( is_ai )
	{
		ai_original_target_x_loc = targetXLoc;
		ai_original_target_y_loc = targetYLoc;
	}

	return 1;
}
//--------- End of function Unit::set_attack ------------//


//--------- Begin of function Unit::attack_now ---------//
//
// <BaseObj*> targetObj - the target BaseObj.
// <int> attackDistance - the distance between the target and the current unit.
//
void Unit::attack_now(BaseObj* targetObj, int attackDistance)
{
	// ###### begin Gilbert 16/12 ##########//
//	err_when(abs(cur_x-next_x)>sprite_info->speed || abs(cur_y-next_y)>sprite_info->speed);
	err_when(abs(cur_x-next_x)>sprite_speed() || abs(cur_y-next_y)>sprite_speed() );
	// ###### end Gilbert 16/12 ##########//

	// ####### begin Gilbert 15/3 ########//
	UCHAR oldCurAction = cur_action;
	// ####### end Gilbert 15/3 ########//

	int curXLoc = next_x_loc();
	int curYLoc = next_y_loc();

	//---- if the target is next to this unit and current attack mode is range attack, set the attack mode to close attack ---//
	if( (attackDistance==1 && attack_range()>1) || (attackDistance > 1 && attack_range() == 1)) // often false condition is checked first
		choose_best_attack_mode(attackDistance, targetObj->obj_mobile_type() ); // may change to close attack

	//--------- if the current attack mode is range attack --------//

	if( attack_range()>1 )
	{
		AttackInfo *attackInfo = attack_info_array+cur_attack;

		if( !bullet_array.add_bullet_possible(curXLoc, curYLoc, mobile_type, targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
			 targetObj->obj_mobile_type(), targetObj->obj_loc_width(), targetObj->obj_loc_height(),
			 range_attack_x_loc, range_attack_y_loc, cur_attack_info()->bullet_speed, cur_attack_info()->bullet_sprite_id, this) )
		{
			find_new_position_for_range_attack(targetObj);
			
			err_when(targetObj->obj_loc_x1()+attack_loc_offset_x < 0);
			err_when(targetObj->obj_loc_x1()+attack_loc_offset_x+obj_loc_width()-1 >= MAX_WORLD_X_LOC);
			err_when(targetObj->obj_loc_y1()+attack_loc_offset_y < 0);
			err_when(targetObj->obj_loc_y1()+attack_loc_offset_y+obj_loc_height()-1 >= MAX_WORLD_Y_LOC);
			err_when((attack_loc_offset_x || attack_loc_offset_y) && (!bullet_array.add_bullet_possible(targetObj->obj_loc_x1()+attack_loc_offset_x, targetObj->obj_loc_y1()+attack_loc_offset_y, mobile_type, targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
			 targetObj->obj_mobile_type(), targetObj->obj_loc_width(), targetObj->obj_loc_height(),
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id)));

			if(attack_loc_offset_x ||  attack_loc_offset_y)
				move_to(targetObj->obj_loc_x1()+attack_loc_offset_x, targetObj->obj_loc_y1()+attack_loc_offset_y);
			else
				stop_move();
			return;
		}
		
		set_cur(next_x, next_y);
		stop_move();
		set_attack_dir(curXLoc, curYLoc, range_attack_x_loc, range_attack_y_loc);
	}
	else //----- close attack -------//
	{
		int closeAttackLocX=targetObj->obj_loc_x1();
		int closeAttackLocY=targetObj->obj_loc_y1();
		close_attack_location(targetObj, closeAttackLocX, closeAttackLocY);
		set_cur(next_x, next_y);
		stop_move();
		set_attack_dir(curXLoc, curYLoc, closeAttackLocX, closeAttackLocY);
	}
	
	//--- if the direction is correct, attack now. Otherwise, turn first ---//

	
//	set_cur(next_x, next_y);
//	stop_move();

	if(is_dir_correct() || (mobile_type == UNIT_AIR && !attackDistance))
	{
	//	cycle_eqv_attack();
		final_dir = cur_dir;
		turn_delay = 0;
		//if(cur_action != SPRITE_ATTACK)
		//	cur_frame = 1;
		cur_action = SPRITE_ATTACK;
		// ###### begin Gilbert 15/3 #######//
		if( oldCurAction != SPRITE_ATTACK )
			cur_frame = 1;
		// ###### end Gilbert 15/3 #######//
	}
	else
		set_turn();
}
//--------- End of function Unit::attack_now ---------//

//--------- Begin of function Unit::process_attack --------//
//
// Return: <int> 1 - if the sprite just finished its current attack
//					  0 - other statuses - either waiting for next attack
//					      or is attacking.
//
int Unit::process_attack()
{
	//------ process Sprite level attack animation ----//

	int x = Sprite::process_attack();

	//----- if the sprite has come to the frame of hitting the target of releasing a bullet -----//

	AttackInfo *attackInfo = cur_attack_info();

	//------ add effect -------//

	if(( cur_frame == cur_sprite_attack()->frame_count-1 && attackInfo->effect_out_frame < 1)		// add the effect at the second last frame if no effect_out_frame specified
		|| ( cur_frame == attackInfo->effect_out_frame ))
	{
		add_attack_effect();
	}

	//-------- attack now ---------// 

	BaseObj* targetObj = base_obj_array[cur_order.para];
	// ##### begin Gilbert 15/3 #########//
/*	
	const int panicRange = 3;
	int locX, locY;
	int panicRecno = 0;
	int panicUnitId;
	Unit* unitPtr;

	if((unit_id == UNIT_JAGUAR || unit_id == UNIT_GORILLA) && attackInfo->consume_power)  //frighten people 
	{
		Location* locPtr;
		for(locX = max(0, next_x_loc()-panicRange); locX<=min(MAX_WORLD_X_LOC-1, next_x_loc()+panicRange); locX++)
		{
			for(locY = max(0, next_y_loc()-panicRange); locY<= min(MAX_WORLD_Y_LOC-1, next_y_loc()+panicRange); locY++)
			{
				if(locX == next_x_loc() && locY == next_y_loc() )
					continue;
				locPtr = world.get_loc(locX, locY);
				if(panicRecno = locPtr->unit_recno(UNIT_LAND))
				{
					panicUnitId = (unitPtr=unit_array[panicRecno])->unit_id;
					if(panicUnitId == UNIT_NORMAN_SPU || panicUnitId == UNIT_ROMAN_SPU || panicUnitId == UNIT_GREEK_SPU
						|| (panicUnitId == UNIT_CARTH_SPU && unit_id == UNIT_GORILLA) || panicUnitId == UNIT_MONGOL_SPU || panicUnitId == UNIT_EGYPTIAN_SPU
						|| (panicUnitId == UNIT_INDIAN_SPU && unit_id == UNIT_GORILLA))
					{
						//panic !!
						int panicShiftX = 1+m.random(5);
						int panicShiftY = 1+m.random(5);
						
						if(next_x_loc()-locX > 0) //horror at the right side !
							panicShiftX = -panicShiftX;		//move towards the left
						if(next_y_loc()-locY > 0) //horror at the bottom side !
							panicShiftY = -panicShiftY;		//escape to the top

						if(unitPtr->cur_order.mode == UNIT_ATTACK || unitPtr->cur_action == SPRITE_ATTACK)
							unitPtr->stop_order();   //stop attacking at once and escape
						int escapeLocX = max(0, locX+panicShiftX);
						escapeLocX = min(MAX_WORLD_X_LOC-1, escapeLocX);
						int escapeLocY = max(0, locY+panicShiftY);
						escapeLocY = min(MAX_WORLD_Y_LOC-1, escapeLocY);

						unitPtr->move_to(escapeLocX, escapeLocY);
					}
				}
				else 
					continue;
			}
		}
	}
*/
	// ##### end Gilbert 15/3 #########//

	if( attack_range() > 1 )		// range attack
	{
		//--- release the bullet at the specific frame ---//

		if( cur_frame == attackInfo->bullet_out_frame )
		{
			// ####### begin Gilbert 9/5 #########//
			// ------ process special ability --------//

			// if( is_human() && combat_level() >= MAX_COMBAT_BATTLE )
//				think_use_special_ability();			// call only in next_day
			// ####### end Gilbert 9/5 #########//

			int curXLoc = next_x_loc();
			int curYLoc = next_y_loc();

			if( bullet_array.add_bullet_possible(curXLoc, curYLoc, mobile_type, targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
				 targetObj->obj_mobile_type(), targetObj->obj_loc_width(), targetObj->obj_loc_height(),
				 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id) )
			{
//				set_attack_dir(curXLoc, curYLoc, range_attack_x_loc, range_attack_y_loc);
				bullet_array.add_bullet(this, targetObj);

				// ###### begin Gilbert 10/2 ######//
				gain_experience(); 	// gain experience to increase combat level
				// ###### end Gilbert 10/2 ######//

				//------- reduce power --------//
				cur_power -= attackInfo->consume_power;

				if(cur_power<0)		// for fixing bug on-fly
					cur_power = 0;

				// ------ prcocess special ability ------//

				if( attackInfo->consume_power == 0 && 
					is_special_ability_effective() == SPAB_MIRAGE_ARROW )
				{
					short oldRangeAttackXLoc = range_attack_x_loc;
					short oldRangeAttackYLoc = range_attack_y_loc;

					// BUGHERE : not finish
					// add bullet to every unit in that direction, within a range

					WorldScanTraverser scanner;
					scanner.init( next_x_loc(), next_y_loc(), loc_width, attackInfo->attack_range );

					for( scanner.restart(); !scanner.is_finish(); scanner.next() )
					{
						Unit *scanUnit = unit_array[scanner.get_unit()];
						if( scanUnit->nation_recno == targetObj->nation_recno
							&& scanUnit->is_special_ability_effective() != SPAB_REFLECT_ARROW	// don't shoot at target with reflect on
							&& get_dir(cur_x, cur_y, scanUnit->cur_x, scanUnit->cur_y) == cur_dir
							&& can_attack_target( scanUnit->base_obj_recno )
							&& bullet_array.add_bullet_possible( next_x_loc(), next_y_loc(), mobile_type,
								scanUnit->next_x_loc(), scanUnit->next_y_loc(), scanUnit->mobile_type,
								scanUnit->loc_width, scanUnit->loc_height, range_attack_x_loc, range_attack_y_loc,
								attackInfo->bullet_speed, attackInfo->bullet_sprite_id, this) )
						{
							bullet_array.add_bullet(this, scanUnit);
						}
					}

					range_attack_x_loc = oldRangeAttackXLoc;
					range_attack_y_loc = oldRangeAttackYLoc;
				}

			//	cycle_eqv_attack();
				set_remain_attack_delay();
				
			}
			else
				stop_move();
		}
	}
	else								// close attack
	{
		//----- inflict damage at the last attacking frame ----//

		// ##### begin Gilbert 15/3 #########//
		if( cur_frame == cur_sprite_attack()->frame_count )
		{
			// ####### begin Gilbert 9/5 #########//
			// ------ process special ability --------//

			// if( is_human() && combat_level() >= MAX_COMBAT_BATTLE )
//				think_use_special_ability();			// call only in next_day
			// ####### end Gilbert 9/5 #########//

			const int panicRange = 3;
			int x1 = next_x_loc() - panicRange;
			if( x1 < 0 )
				x1 = 0;
			int x2 = next_x_loc() + loc_width - 1 + panicRange;
			if( x2 > MAX_WORLD_X_LOC )
				x2 = MAX_WORLD_X_LOC-1;
			int y1 = next_y_loc() - panicRange;
			if( y1 < 0 )
				y1 = 0;
			int y2 = next_y_loc() + loc_width - 1 + panicRange;
			if( y2 > MAX_WORLD_Y_LOC )
				y2 = MAX_WORLD_Y_LOC-1;
			int locX, locY;
			int panicRecno = 0;

			if((unit_id == UNIT_JAGUAR || unit_id == UNIT_GORILLA) && attackInfo->consume_power)  //frighten people 
			{
				for( locY = y1; locY <= y2; ++locY )
				{
					Location* locPtr = world.get_loc( x1, locY );
					for( locX = x1; locX <= x2; ++locX, ++locPtr )
					{
						int panicRecno = locPtr->unit_recno(UNIT_LAND);
						if( panicRecno && panicRecno != sprite_recno		// exclude itself
							&& unit_array[panicRecno]->base_obj_recno != cur_order.para )		// also exclude attacking target
						{
							Unit* unitPtr = unit_array[panicRecno];

							int panicFlag = 0;
							switch( unitPtr->unit_id )
							{
							case UNIT_NORMAN_SPU:
							case UNIT_ROMAN_SPU:
							case UNIT_GREEK_SPU:
							case UNIT_MONGOL_SPU:
							case UNIT_EGYPTIAN_SPU:
								panicFlag = 1;
								break;

							case UNIT_CARTH_SPU:
							case UNIT_INDIAN_SPU:
								panicFlag = unit_id == UNIT_GORILLA;
								break;
							}

							if( panicFlag )
							{
								//panic !!
								int panicShiftX = 1+m.random(5);
								int panicShiftY = 1+m.random(5);
								
								if(next_x_loc() > locX ) //horror at the right side !
									panicShiftX = -panicShiftX;		//move towards the left
								if(next_y_loc() > locY) //horror at the bottom side !
									panicShiftY = -panicShiftY;		//escape to the top

								if(unitPtr->cur_order.mode == UNIT_ATTACK || unitPtr->cur_action == SPRITE_ATTACK)
									unitPtr->stop_order();   //stop attacking at once and escape
								int escapeLocX = max(0, locX+panicShiftX);
								escapeLocX = min(MAX_WORLD_X_LOC-1, escapeLocX);
								int escapeLocY = max(0, locY+panicShiftY);
								escapeLocY = min(MAX_WORLD_Y_LOC-1, escapeLocY);

								unitPtr->move_to(escapeLocX, escapeLocY);
							}
						}
					}
				}
			}
			// ##### end Gilbert 15/3 #########//

			float attackDamage = actual_damage()
				* attackInfo->attack_attribute.effectiveness(targetObj->defense_attribute);

			Unit *targetUnit;
			if( (targetUnit = targetObj->cast_to_Unit()) )
			{
				// bonus from attack direction, 50% from back, 25% from side
				int dirDiff = abs( (cur_dir & 7) - (targetUnit->cur_dir & 7) );

				// same dir means attack from behind, dirDiff is 0
				// opposite direction, dirDiff is 4, no bonus

				if( dirDiff < 4 )
					attackDamage += attackDamage * (4-dirDiff) / 8;
				else if( dirDiff > 4 )
					attackDamage += attackDamage * (dirDiff-4) / 8;
			}

			targetObj->being_attack_hit(this, attackDamage );

			gain_experience(); 	// gain experience to increase combat level

			//------- reduce power --------//
			cur_power -= attackInfo->consume_power;

			if(cur_power<0)		// for fixing bug on-fly
				cur_power = 0;

			// ------- process special ability ----------//

			if( is_special_ability_effective() == SPAB_FREEZE )
			{
				// BUGHERE : not finish
				// add bullet to every unit in that direction, within a range

				WorldScanTraverser scanner;
				scanner.init( next_x_loc(), next_y_loc(), loc_width, 8 );	// range is 8
			
				int putEffectFlag = 0;
				for( scanner.restart(); !scanner.is_finish(); scanner.next() )
				{
					Unit *scanUnit = unit_array[scanner.get_unit()];
					if( (!targetUnit || targetUnit != scanUnit )		// not the target attacking
						&& scanUnit->nation_recno == targetObj->nation_recno
						&& m.random(scanUnit->combat_level()) < m.random(combat_level()/2) )	// saving depend on combat level
					{
						// ###### begin Gilbert 10/4 ##########//
						if( scanUnit->freeze_frame_left == 0 )
						{
							putEffectFlag = 1;
							scanUnit->freeze( 40 );		// 4 days
						}
						// ###### end Gilbert 10/4 ##########//
					}
				}

				if( putEffectFlag )
				{
					static int effectId = sprite_res.search_sprite( "ICE" );
					if( effectId )
						Effect::create(effectId, next_x, next_y, SPRITE_DIE, 1, 2, 0); 
				}
			}

			set_remain_attack_delay();	
		}
	}
	if (x && attackInfo->die_after_attack)
		hit_points = 0.0;

	return x;
}
//---------- End of function Unit::process_attack ----------//

//--------- Begin of function Unit::validate_attack_target --------//
//
int Unit::validate_attack_target()
{
	if( base_obj_array.is_deleted(cur_order.para) )
		return 0;

	BaseObj* targetObj = base_obj_array[cur_order.para];

	if( !targetObj->obj_is_visible() )
		return 0;

	if( !can_attack_target(cur_order.para) )
		return 0;

	//---------------------------------------//

	if( targetObj->obj_type==OBJ_FIRM || targetObj->obj_type==OBJ_TOWN )
	{
		if( targetObj->obj_loc_x1() != cur_order.loc_x ||		// a town or a firm cannot move
			 targetObj->obj_loc_y1() != cur_order.loc_y )
		{
			return 0;
		}
	}
	else if( targetObj->obj_type==OBJ_UNIT )
	{
		//-- if Unit::name_id does not match the saved extra_para, that means this is not the original target ---//

		if( targetObj->cast_to_Unit()->name_id != cur_order.name_id )
			return 0;

		//---- if it has a pushed order -----//

		if( has_pushed_order() )
		{
			//-----------------------------------------------------//
			//
			// Stop the chase if the target is being far away from
			// its original location.
			//
			// It is to determine when to stop the aggressive attack.
			// original_target_x_loc is set by think_aggressive_attack()
			//
			//-----------------------------------------------------//

			if( original_target_x_loc >= 0 &&
				 (get_behavior_mode() == UNIT_AGGRESSIVE || unit_mode) )
			{
				//--- only chase in aggressive mode or in special mode like defending and rebel ----//

				enum { AGGRESSIVE_CHASE_RANGE = 5 };

				int curDistance = m.points_distance( targetObj->obj_loc_x1(),
					targetObj->obj_loc_y1(), original_target_x_loc, original_target_y_loc );

				if( curDistance > AGGRESSIVE_CHASE_RANGE )
				{
					return 0;		// stop the current order and resume the pushed order
				}
			}

			//-------------------------------------------//
			//
			// If there is a pushed primary order, it means the
			// current order is an ad hoc order. And as the
			// current mode is ATTACK, it means the unit
			// was defending itself against an attacker.
			//
			// Now the attacker has moved away, the unit will
			// resume its original action.
			//
			//-------------------------------------------//

			else
			{
				// ##### begin Gilbert 15/3 ########//
				// int attackDistance = cal_distance( targetObj->obj_loc_x1(), targetObj->obj_loc_y1(),
				//							targetObj->obj_loc_width(), targetObj->obj_loc_height() );
				int attackDistance = area_distance( targetObj );
				// ##### end Gilbert 15/3 ########//
 
				if( attackDistance > max_attack_range() )
					return 0;
			}
		}
	}
	else
		err_here();

	return 1;
}
//---------- End of function Unit::validate_attack_target ----------//

//###### begin trevor 10/2 ######//

//--------- Begin of function Unit::can_attack --------//
//
int Unit::can_attack()
{
	return attack_count > 0 &&
			 ( !is_civilian() || unit_mode==UNIT_MODE_TOWN_DEFENDER ||
				unit_mode==UNIT_MODE_REBEL );
}
//---------- End of function Unit::can_attack ----------//

//###### end trevor 10/2 ######//


//--------- Begin of function Unit::can_attack_target --------//
//
int Unit::can_attack_target(int targetObjRecno)
{
	if( !can_attack() )		// if the unit does not have attacking ability at all
		return 0;

	BaseObj* targetObj = base_obj_array[targetObjRecno];

	// ####### begin Gilbert 22/12 #########//
/*
	// ------- Bee only attack living being ---------//

	if( unit_id == UNIT_BEE )
	{
		BaseObj *targetBaseObj = base_obj_array[targetObjRecno];

		if( targetBaseObj->cast_to_Unit() )
		{
			Unit *targetUnit = targetBaseObj->cast_to_Unit();

			// attack race_id()
			// do not attack bee, weapon, god

			if( !targetUnit->race_id
				&& (targetUnit->unit_id == UNIT_BEE
				|| unit_res[targetUnit->unit_id]->unit_class == UNIT_CLASS_WEAPON
				|| unit_res[targetUnit->unit_id]->unit_class == UNIT_CLASS_GOD) )
			{
				return 0;
			}
		}
		else
			return 0;
	}
	// ####### end Gilbert 22/12 #########//
*/

	/// ######### begin Gilbert 24/4 #########//
	//----- it's impossible to attack caravans or god-----//

	Unit *targetUnit;
	if( (targetUnit = targetObj->cast_to_Unit()) )
	{
		if( targetUnit->unit_id == UNIT_CARAVAN
			|| targetObj->cast_to_Unit()->cast_to_UnitGod() )
		{
			return 0;
		}
	}
	/// ######### end Gilbert 24/4 #########//

	int targetNationRecno = targetObj->nation_recno;

	//------- if this is a nation unit -----//

	if( nation_recno )
	{
		//--- if it is an explosive cart, allow attacking own units ---//

		// Unit* unitPtr = base_obj_array[targetObjRecno]->cast_to_Unit();

		if( targetUnit && targetUnit->unit_id == UNIT_EXPLOSIVE_CART &&
			 nation_recno == targetNationRecno )
		{
			return 1;
		}

		//--- if the should_attack flag in NationRelation is true, attack now ---//

		// ###### begin Gilbert 29/4 #########//
		// return nation_array[nation_recno]->get_relation_should_attack(targetNationRecno);
		return nation_array.should_attack( nation_recno, targetNationRecno );
		// ###### end Gilbert 29/4 #########//
	}

	//----- if this is an independent unit -----//

	else
	{
		if( unit_mode == UNIT_MODE_TOWN_DEFENDER )
		{
			if( town_array.is_deleted(unit_mode_para) )
				return 0;

			return town_array[unit_mode_para]->last_attack_nation_recno == targetNationRecno;
		}
		else if( unit_mode == UNIT_MODE_CAMP_DEFENDER )
		{
			if( firm_array.is_deleted(unit_mode_para) )
				return 0;

			return firm_array[unit_mode_para]->last_attack_nation_recno == targetNationRecno;
		}
		else if( unit_mode == UNIT_MODE_REBEL )
		{
			if( rebel_array.is_deleted(unit_mode_para) )
				return 0;

			return rebel_array[unit_mode_para]->is_hostile_nation(targetNationRecno);
		}
		else if( hero_id )		// hero can counter-attack when under attack
		{
			return 1;
		}
	}
	
	return 0;
}
//---------- End of function Unit::can_attack_target ----------//


//--------- Begin of function Unit::being_attack_hit --------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
// <float>	  damagePoint - the points of damage made.
//
void Unit::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	// ----- if under magic of invulnerability --------//

	if( invulnerable_day_count )
	{
		damagePoint = 0.0f;
	}

	// ---- invinciblity and energy field ------//

	if( damagePoint > 0.0f &&
		(item.ability(ITEM_ABILITY_ABSORB_HIT) || item.ability(ITEM_ABILITY_INVINCIBLE)) )
	{
		damagePoint = 0.0f;
		item.use_now();
	}

	// ------- anti monster, can penetrate invincibility ------- //

	if( attackerObj && attackerObj->cast_to_Unit() )
	{
		Unit *attackUnit = attackerObj->cast_to_Unit();

		// ------ anti monster ---------//

		if( is_monster() )
		{
			int antiMonsterAbility;
			Item *itemPtr = &attackUnit->item;

			// #### begin Gilbert 26/1 #######//
			if( (antiMonsterAbility = itemPtr->ability(ITEM_ABILITY_ANTI_MONSTER)) 
				&& itemPtr->para == monster_id() )
//			|| !unit_array.is_deleted(attackUnit->leader_unit_recno)			// check leader's item
//				&& (itemPtr = &unit_array[attackUnit->leader_unit_recno]->item)
//				&& (antiMonsterAbility = itemPtr->ability(ITEM_ABILITY_ANTI_MONSTER)) 
//				&& itemPtr->para == monster_id() )
			// #### end Gilbert 26/1 #######//
			{
				damagePoint += (float) antiMonsterAbility;
				itemPtr->use_now();
			}
		}
	}

	// ####### begin Gilbert 15/4 #######//
	// ------- check if this is the first attack of in today ------/
	// must check before BaseObj::being_attack_hit is called

	int lastAttackDateBackup = last_attack_date;
	// ####### end Gilbert 15/4 #######//

	//---- call base class function ------//

	BaseObj::being_attack_hit(attackerObj, damagePoint);

	//--- if this is a monster, increase reputation of the attacking nation ---//

	if( cast_to_UnitMonster() && attackerObj && attackerObj->nation_recno )
	{
		nation_array[attackerObj->nation_recno]->change_reputation(REPUTATION_INCREASE_PER_ATTACK_MONSTER);
	}

	//--- set rebel hostile nation if this unit is a rebel ---//

	if( unit_mode == UNIT_MODE_REBEL &&
		 attackerObj && attackerObj->nation_recno )
	{
		if( !rebel_array.is_deleted(unit_mode_para) )
			rebel_array[unit_mode_para]->set_hostile_nation(attackerObj->nation_recno);
	}

	//------ if this is an AI commander, ask for help -----//

	if( is_ai && rank_id >= RANK_GENERAL )
		ai_leader_being_attacked(attackerObj);

	//------ react by attacking the attacker ----//

	if( can_attack() )
		auto_react_attack(attackerObj);

	//--- if a member in a troop is under attack, ask for other troop members to help ---//

	if( attackerObj &&
		!base_obj_array.is_deleted(attackerObj->base_obj_recno) 	// the object may be dying
		&& !force_move_flag
		// ####### begin Gilbert 15/4 #######//
		// && info.game_date%2 == sprite_recno%2 )	// remarked to maximize the effect
		// && last_attack_date != lastAttackDateBackup )		// once per day
		// && last_attack_date - lastAttackDateBackup >= 2 )	// almost mean only the first attack in a battle
		&& info.game_date - last_ask_attack_date >= 3 )		// no less than 3 days after last ask help attack
		// ####### end Gilbert 15/4 #######//
	{
		// even no people are asked to fight, still mark the last_ask_attack_date to reduce process time

		last_ask_attack_date = info.game_date;

		if( (leader_unit_recno || (team_info && team_info->member_count>1))
			&& ask_team_help_attack(attackerObj) >= MAX_ATTACK_HELPER )
		{
			// empty
		}
		else
		{
			ask_neighbour_help_attack(attackerObj);
		}
		// ##### end Gilbert 27/4 ########//
	}

	// ---- if it is town defender, call town return fire -----//

	if( attackerObj && attackerObj->obj_is_visible() )
	{
		if( unit_mode == UNIT_MODE_TOWN_DEFENDER && !town_array.is_deleted(unit_mode_para)
			&& attackerObj->area_distance(town_array[unit_mode_para]) <= TOWN_TARGET_RANGE )
		{
			town_array[unit_mode_para]->return_fire( attackerObj );
		}

		if( unit_mode == UNIT_MODE_CAMP_DEFENDER && !firm_array.is_deleted(unit_mode_para)
			&& attackerObj->area_distance(firm_array[unit_mode_para]) <= FORT_BULLET_RANGE )
		{
			firm_array[unit_mode_para]->return_fire( attackerObj );
		}
	}

	// ---- side effect from item -------//

	if( attackerObj && attackerObj->cast_to_Unit() )
	{
		Unit *attackUnit = attackerObj->cast_to_Unit();

		// ---- freeze ability -------//

		int freezeProbability;
		if( (freezeProbability = attackUnit->item.ability(ITEM_ABILITY_FREEZE))
			&& m.random(100) < freezeProbability )
		{
			freeze(freezeProbability);

			attackUnit->item.use_now();
		}

		// ---- decrease target loyalty -------//

		int loyaltyDecProbability;
		if( race_id		// target can reduce loyalty
			&&	(loyaltyDecProbability = attackUnit->item.ability(ITEM_ABILITY_DEC_LOYALTY)) 
			&& m.random(100) < loyaltyDecProbability )
		{
			change_loyalty(-1);

			attackUnit->item.use_now();
		}
	}

	// ####### begin Gilbert 9/5 #########//
	// ------ process special ability --------//
	// if( is_human() && combat_level() >= MAX_COMBAT_BATTLE )
//		think_use_special_ability();			// call only in next_day
	// ####### end Gilbert 9/5 #########//
}
//---------- End of function Unit::being_attack_hit ----------//

//--------- Begin of function Unit::auto_react_attack --------//
//
// The unit reacts to the attack by attacking the attacker back.
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void Unit::auto_react_attack(BaseObj* attackerObj)
{
	if( !attackerObj )
		return;

	if( (force_move_flag || behavior_mode==UNIT_STAND_GROUND)
		&& cur_action != SPRITE_IDLE )		// if the unit is stuck and cannot move, then counter-attack 
		return;

//	if( attackerObj->obj_type != OBJ_UNIT )		// only auto attack units, do not attack offensive firms
//		return;
	if( !attackerObj->is_visible())		// only auto attack units, do not attack offensive firms
		return;

	//--- bees do not auto react to attack as they can only attack once ---//

	if( unit_id == UNIT_BEE )
		return;

	//---------------------------------------//
	//
	// If the aggressive_mode is off, then don't
	// fight back when the unit is moving, only
	// fight back when the unit is already fighting
	// or idle.
	//
	//---------------------------------------//

	if( get_behavior_mode() != UNIT_AGGRESSIVE &&
		 cur_action != SPRITE_ATTACK && cur_action != SPRITE_IDLE )
	{
		return;
	}

	//---- if the sprite is already attacking an unit, do nothing ----//

	// ###### begin Gilbert 30/3 #######//
	if( // cur_action == SPRITE_ATTACK &&		// two people attack one target, response to one only
		 cur_order.mode == UNIT_ATTACK &&
		 !base_obj_array.is_deleted(cur_order.para) )
	{
		BaseObj *attackingObj = base_obj_array[cur_order.para];

		if( attackingObj->cast_to_Unit()			// already attack an unit, don't change target
			|| !attackerObj->cast_to_Unit() )	// not attacking an unit, but not attacked by a unit, don't change target as well
			return;
	}
	// ###### end Gilbert 30/3 #######//

	//---- in stand ground mode, only attack unit next to the current unit, don't move to attack if the enemy is using range attack ---//

	if( get_behavior_mode()==UNIT_STAND_GROUND )
	{
		// ######## begin Gilbert 15/3 #########//
//		int attackDistance = cal_distance( attackerObj->obj_loc_x1(), attackerObj->obj_loc_y1(),
//									attackerObj->obj_loc_width(), attackerObj->obj_loc_height() );
		int attackDistance = area_distance( attackerObj );
		// ######## end Gilbert 15/3 #########//

		if( attackDistance > max_attack_range() )
			return;
	}

	//--- if the attacker cannot attack the target, do nothing ---//

	if( !can_attack_target(attackerObj->base_obj_recno) )
		return;

	// ###### begin Gilbert 15/4 #########//
	// ----- stop react if independent town is less than 1 -------//

	if( attackerObj->cast_to_Town() && attackerObj->nation_recno == 0 
		&& nation_recno != 0 && attackerObj->cast_to_Town()->resistance(nation_recno) < 1.0f )
	{
		return;
	}
	// ###### end Gilbert 15/4 #########//

	//------ attack the attacker now -------//

	if( !has_pushed_order() )		// if there is already an order pushed into the stack, that means the unit is already preforming an ad hoc action (likely already defending), then just replace the original ad hoc action with the current one.
		push_order();

	attack(attackerObj->base_obj_recno, false);

	if( cur_order.mode != UNIT_ATTACK )		// if the attack action failed, resume order
		pop_order();
}
//---------- End of function Unit::auto_react_attack ----------//


//------- Begin of function Unit::ask_team_help_attack --------//
//
// It returns whether any of the co-member of this unit in a troop
// is under attack.
//
// <BaseObj*> attackerObj - the pointer of the attacker BaseObj
//
// return number of attack to help
//
int Unit::ask_team_help_attack(BaseObj* attackerObj)
{
	if( !attackerObj )
		return 0;

	//--- if the attacking unit is our unit (this can happen if the unit is porcupine) ---//

	if( attackerObj->nation_recno == nation_recno )
		return 0;

	if( attackerObj->obj_type != OBJ_UNIT )
		return 0;

	if( !attackerObj->is_visible() )
		return 0;

	//-----------------------------------------//

	int leaderUnitRecno=0;
	int helpCount = 0;

	if( leader_unit_recno && !unit_array.is_deleted(leader_unit_recno) )		// if the current unit is a soldier, get its leader's recno
		leaderUnitRecno = leader_unit_recno;

	else if( team_info )			// this unit is the commander
		leaderUnitRecno = sprite_recno;

	if( leaderUnitRecno )
	{
		TeamInfo* teamInfo = unit_array[leaderUnitRecno]->team_info;

		err_when( !teamInfo );

		#define LEADER_IN_DANAGER_HIT_POINT  30

		int leaderInDanger = (leaderUnitRecno == sprite_recno)
			&& 100 * hit_points / max_hit_points() < LEADER_IN_DANAGER_HIT_POINT;

		// ###### begin Gilbert 15/4 #########//
		short suitUnitRecno[MAX_TEAM_MEMBER];
		short suitUnitDistance[MAX_TEAM_MEMBER];
		int	suitCount = 0;

		for( int i=teamInfo->member_count-1 ; i>=0 ; i-- )
		{
			int unitRecno = teamInfo->member_unit_array[i];

			// ####### begin Gilbert 2/3 #######//
			// ----- do not ask himself to attack, it is done in auto-react attack --------//

			if( sprite_recno == unitRecno )
				continue;
			// ####### end Gilbert 2/3 #######//

			if( unit_array.is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = unit_array[ unitRecno ];

			if( !unitPtr->is_visible() )
				continue;

			// if retreating, no assist
			if( force_move_flag )
				continue;

			// if in stand ground mode, attack if within attack range

			int targetDistance = unitPtr->area_distance(attackerObj);
			int maxAttackRange = unitPtr->max_attack_range();

			if( unitPtr->get_behavior_mode() == UNIT_STAND_GROUND 
				&& targetDistance > maxAttackRange )
				continue;

			BaseObj *unitsTarget;
			int unitsTargetDistance;

			if( unitPtr->cur_order.mode == UNIT_ATTACK
				&& !base_obj_array.is_deleted(unitPtr->cur_order.para)
				&& (unitsTarget = base_obj_array[unitPtr->cur_order.para])->is_visible() )
			{
				// already order to attack this target, skip
				if( unitsTarget == attackerObj )
					continue;

				unitsTargetDistance = unitPtr->area_distance(unitsTarget);
			}
			else
			{
				unitsTarget = NULL;
			}
				
			if( leaderInDanger && targetDistance <= maxAttackRange		// behavior is bad unless unitPtr can attack immediately
				||	unitPtr->is_all_stop() 

				// if member is attacking non-unit, preempt
				|| unitsTarget	&& !unitsTarget->cast_to_Unit()

				// if chasing target but new target is in closer, preempt
				|| unitsTarget && unitsTargetDistance-1 > maxAttackRange && targetDistance <= maxAttackRange )
			{
				// -------- add to suit unit recno -------//

				suitUnitRecno[suitCount] = unitRecno;
				suitUnitDistance[suitCount] = targetDistance > maxAttackRange ? targetDistance - maxAttackRange : 0;
				suitCount++;

				if( suitUnitDistance[suitCount] == 0 )		// already find a suitable target
					break;
			}
		}

		// check the behavior of following case : 
		// old target and new target are outside attack range
			// when chasing old target the path may be long not direct that the
			// distance seems long but the actual path may be shorter
			// so never preempt it this case
		// old target inside attack range, new target outside attack range
		// old target outside attack range, new target inside attack range
		// old target and new target are inside

		// -------- find the closet one to help --------//

		for( i = 0; i < suitCount; ++i )
		{
			Unit *unitPtr = unit_array[suitUnitRecno[i]];

			if( !unitPtr->has_pushed_order() )
				unitPtr->push_order();
			unitPtr->attack(attackerObj->base_obj_recno, false);

			// mark it last_ask_attack_date, so it won't trigger other units to attack
			unitPtr->last_ask_attack_date = info.game_date;

			++helpCount;
		}
		// ###### end Gilbert 15/4 #########//
	}

	return helpCount;
}
//-------- End of function Unit::ask_team_help_attack --------//



//------- Begin of function Unit::ask_neighbour_help_attack --------//
//
// It returns whether any of the co-member of this unit in a troop
// is under attack.
//
// <BaseObj*> attackerObj - the pointer of the attacker BaseObj
//
// return number of attack to help
//
int Unit::ask_neighbour_help_attack(BaseObj* attackerObj)
{
	if( !attackerObj )
		return 0;

	//--- if the attacking unit is our unit (this can happen if the unit is porcupine) ---//

	if( attackerObj->nation_recno == nation_recno )
		return 0;

	if( attackerObj->obj_type != OBJ_UNIT )
		return 0;

	if( !attackerObj->is_visible() )
		return 0;

	//-----------------------------------------//

	int helpCount = 0;

	WorldScanTraverser scanner;
	scanner.init( obj_loc_x1(), obj_loc_y1(), loc_width,  MAX_HELPER_DISTANCE );
	
	// quick filter unit selected member

	for( scanner.restart() ; !scanner.is_finish(); scanner.next() )
	{
		int unitRecno = scanner.get_unit();

		// ----- do not ask himself to attack, it is done in auto-react attack --------//
		if( sprite_recno == unitRecno )
		{
			scanner.remove();
			continue;
		}

		Unit* unitPtr = unit_array[ unitRecno ];
		
		if( unitPtr->nation_recno != nation_recno		// same nation
			|| unitPtr->force_move_flag 			// if retreating, no assist
			|| !unitPtr->can_attack() )
		{
			scanner.remove();
			continue;
		}
	}

	scanner.pack_removed();

	// ------- set weight of each unit ---------//

	for( scanner.restart() ; !scanner.is_finish(); scanner.next() )
	{
		int unitRecno = scanner.get_unit();

		Unit* unitPtr = unit_array[ unitRecno ];
		
		int distUnitAttacker = unitPtr->area_distance(attackerObj);
		int maxAttackRange = unitPtr->max_attack_range();

		if( distUnitAttacker <= maxAttackRange )
			scanner.set_weight(0);
		else
			scanner.set_weight( distUnitAttacker - maxAttackRange );
	}

	scanner.sort_by_weight();

	// long filter

	for( scanner.restart(); !scanner.is_finish(); scanner.next() )
	{
		int unitRecno = scanner.get_unit();		// himself is removed in last filter
		if( !unitRecno)			// 0 if removed
			continue;

		Unit* unitPtr = unit_array[ unitRecno ];

		// if in stand ground mode, attack if within attack range

		int targetDistance = unitPtr->area_distance(attackerObj);
		int maxAttackRange = unitPtr->max_attack_range();

		if( unitPtr->get_behavior_mode() == UNIT_STAND_GROUND 
			&& targetDistance > maxAttackRange )
		{
			continue;
		}

		BaseObj *unitsTarget;
		int unitsTargetDistance;

		if( unitPtr->cur_order.mode == UNIT_ATTACK
			&& !base_obj_array.is_deleted(unitPtr->cur_order.para)
			&& (unitsTarget = base_obj_array[unitPtr->cur_order.para])->is_visible() )
		{
			// already order to attack this target, skip
			if( unitsTarget == attackerObj )
			{
				continue;
			}

			unitsTargetDistance = unitPtr->area_distance(unitsTarget);
		}
		else
		{
			unitsTarget = NULL;
		}
				
		if( unitPtr->is_all_stop() 

			// if member is attacking non-unit, preempt
			|| unitsTarget	&& !unitsTarget->cast_to_Unit()

			// if chasing target but new target is in closer, preempt
			|| unitsTarget && unitsTargetDistance-1 > maxAttackRange && targetDistance <= maxAttackRange )
		{
			// ####### begin Gilbert 5/5 ##########//
			if( !unitPtr->has_pushed_order() )
				unitPtr->push_order();
			// ####### end Gilbert 5/5 ##########//
			unitPtr->attack(attackerObj->base_obj_recno, false);

			// mark it last_ask_attack_date, so it won't trigger other units to attack
			unitPtr->last_ask_attack_date = info.game_date;

			++helpCount;

			// 8 units closest to the target and all unit can attack immediately (in the sense of distance)
			if( helpCount >= MAX_ATTACK_HELPER && scanner.get_weight() > 0  )
			{
				break;
			}
		}
	}

	return helpCount;
}
//-------- End of function Unit::ask_neighbour_help_attack --------//


//------- Begin of function Unit::being_killed ------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void Unit::being_killed(BaseObj* attackerObj)
{
	BaseObj::being_killed(attackerObj);

	//---- the attacker rip off money from the dying unit ----//

	being_killed_leave_money(attackerObj);

	//----- update statistic -------//

	update_being_killed_statistic(attackerObj);

	//--- units commanded by this unit may betray when their leader is killed ---//

	being_killed_subordinate_betray(attackerObj);

	//--- if a monster All-high is killed, the action will induce heroes to join the nation that killed the monster ---//

	if( attackerObj && attackerObj->nation_recno &&
		 rank_id == RANK_KING && is_monster() )
	{
		int inducePower = (skill_level()+combat_level()/2) / 2;

		hero_res.induce_hero_join_nation(attackerObj->nation_recno, inducePower);
	}
	
	//-------- increase battling fryhtan score --------//

	if( attackerObj && attackerObj->nation_recno && is_monster() )
	{
		float addScore;

		switch( rank_id )
		{
			case RANK_SOLDIER:
				addScore = 1.0f;

			case RANK_GENERAL:
				addScore = 5.0f;

			case RANK_KING:
				addScore = 20.0f;
		}

		nation_array[attackerObj->nation_recno]->kill_monster_score += addScore;
	}
}
//--------- End of function Unit::being_killed -------//

//------- Begin of function Unit::update_being_killed_statistic ------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void Unit::update_being_killed_statistic(BaseObj* attackerObj)
{
	Nation* curNation=NULL;
	Nation* attackerNation=NULL;

	if( nation_recno )
		curNation = nation_array[nation_recno];

	if( attackerObj )
	{
		int nationRecno = attackerObj->nation_recno;

		if( nationRecno )
			attackerNation = nation_array[nationRecno];
	}

	//---- if the unit killed is a human unit -----//

	// ######## begin Gilbert 24/3 ########//
	// if( race_id )
	if( is_human() )
	// ######## end Gilbert 24/3 ########//
	{
		//---- if the unit killed is a civilian unit -----//

		if( is_civilian() )
		{
			int killedCount = 1;

			if( unit_res[unit_id]->unit_class  == UNIT_CLASS_WAGON )
				killedCount = cast_to_UnitWagon()->population;

			if( curNation )
			{
				curNation->civilian_killed(race_id, 0, killedCount);
				curNation->own_civilian_killed++;
			}

			if( attackerNation )
			{
				attackerNation->civilian_killed(race_id, 1, killedCount);
				attackerNation->enemy_civilian_killed++;
			}
		}
		else	//---- if the unit killed is a soldier -----//
		{
			if( curNation )
				curNation->own_soldier_killed++;

			if( attackerNation )
				attackerNation->enemy_soldier_killed++;
		}
	}
	else
	if( is_monster() && nation_recno > 0)
	{
		if( curNation )
			curNation->own_frythan_killed++;

		if( attackerNation )
			attackerNation->enemy_frythan_killed++;
	}

	// -------- fryhtan unit ---------//

	else if( unit_res[unit_id]->class_info.monster_side )
	{
		if( attackerNation )
		{
			attackerNation->kill_monster_score += (float) 0.1;

			//--- killing Fryhtan kings and generals increase the attacking nation's reputation ---//

			if( attackerNation->nation_recno != nation_recno )
			{
				if( rank_id == RANK_GENERAL )
					attackerNation->change_reputation( (float) max_hit_points() / 600 );

				else if( rank_id == RANK_KING )
					attackerNation->change_reputation( (float) max_hit_points() / 200 );
			}
		}
	}

	//--------- if it's a non-human unit ---------//

	else
	{
		switch( unit_res[unit_id]->unit_class )
		{
		case UNIT_CLASS_WEAPON:
			if( curNation )
				curNation->own_weapon_destroyed++;

			if( attackerNation )
				attackerNation->enemy_weapon_destroyed++;
			break;


		case UNIT_CLASS_SHIP:
			if( curNation )
				curNation->own_ship_destroyed++;

			if( attackerNation )
				attackerNation->enemy_ship_destroyed++;
			break;

		case UNIT_CLASS_CARAVAN:
			if( curNation )
				curNation->civilian_killed(0, 0);		// 0-for all races

			if( attackerNation )
				attackerNation->civilian_killed(0, 1);		// 1-is the nation is the attacking one
			break;
// ####### begin Gilbert 24/3 #######//
//			case UNIT_CLASS_MONSTER:
//			...
// ####### end Gilbert 24/3 #######//
		}
	}
}
//--------- End of function Unit::update_being_killed_statistic -------//

//----------- Begin of function Unit::being_killed_leave_money ---------//
//
// This function is called when the unit gets killed.
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void Unit::being_killed_leave_money(BaseObj* attackerObj)
{
	if( !attackerObj || attackerObj->nation_recno==0 )
		return;

	if (cast_to_UnitExpCart() && cast_to_UnitExpCart()->triggered)
		return;

	int killMoney;

	switch( rank_id )
	{
		case RANK_KING:
			killMoney = 2000 * (skill_level() + combat_level()/2) / 150;
			break;

		case RANK_GENERAL:
			killMoney = 600 * (skill_level() + combat_level()/2) / 150;
			break;

		case RANK_SOLDIER:
			if( is_civilian() )
				killMoney = 20;
			else
				killMoney = 300 * (skill_level() + combat_level()/2) / 150;
			break;
	}

	killMoney = killMoney * (80 + unique_id % 40) / 100;		// -20 to +20 randomness
	
	cur_power = killMoney;
	max_power = (attackerObj->nation_recno == nation_array.player_recno) ? -10 : 0;
	
	nation_array[attackerObj->nation_recno]->add_income( INCOME_TREASURE, (float) killMoney );
	nation_array[attackerObj->nation_recno]->increased_cash = (float) killMoney;
}
//----------- End of function Unit::being_killed_leave_money ---------//

//------ Begin of function Unit::being_killed_subordinate_betray -----//
//
// Units commanded by this unit may betray when their leader is killed.
//
void Unit::being_killed_subordinate_betray(BaseObj* attackerObj)
{
	if( !attackerObj )
		return;

	if( rank_id < RANK_GENERAL )
		return;

	//---- reckon the net power of the enemy in the battling area ----//

	int hasWar;			// ref var for returning the result

	int netAttackerPower = world.net_attacker_power_in_area(next_x_loc(), next_y_loc(),
								  attackerObj->nation_recno, nation_recno, 0, hasWar, EFFECTIVE_LEADING_DISTANCE);

	//--- the nation with most units in the surrounding area ----//

	int mostPopulatedNationRecno = most_populated_nation_nearby(EFFECTIVE_LEADING_DISTANCE);

	//------------------------------------------------------//

	err_when( !team_info );

	int betrayAllowCount = 1 + m.random(3);			// betray only 1 to 3 units at maximum

	for( int i=0 ; i<team_info->member_count ; i++ )
	{
		int memberUnitRecno = team_info->member_unit_array[i];

		if( memberUnitRecno == sprite_recno )		// this is the unit itself
			continue;

		Unit* unitPtr = unit_array[memberUnitRecno];

		if( !unitPtr->is_visible() || !unitPtr->race_id )		// only living beings will betray
			continue;

		//--- only affect units within the commander's leading distance ---//

		if( m.points_distance( unitPtr->next_x_loc(), unitPtr->next_y_loc(),
			 next_x_loc(), next_y_loc() ) > EFFECTIVE_LEADING_DISTANCE )
		{
			continue;
		}

		//----------------------------------------------//
		// Whether a unit will betray is determined by:
		//
		// -the unit's health
		// -the unit's loyalty
		// -the power of the attacker in the surrounding
		//
		//----------------------------------------------//

		int betrayChance = 50 + netAttackerPower/20
							- 50 * (int) unitPtr->hit_points / (int) unitPtr->max_hit_points()
							- (int) unitPtr->loyalty/2;

		if( m.random(100) < betrayChance )
		{
			unitPtr->betray(attackerObj->nation_recno);

			if( --betrayAllowCount == 0 )
				return;
		}

		//----------------------------------------------//
		//
		// If the unit does not defect to the attacking nation,
		// it may defect to a nation with more force in the area.
		//
		//----------------------------------------------//

		else if( mostPopulatedNationRecno != attackerObj->nation_recno )
		{
			betrayChance = 40
								- 50 * (int) unitPtr->hit_points / (int) unitPtr->max_hit_points()
								- (int) unitPtr->loyalty/2;

			if( m.random(100) < betrayChance )
			{
				unitPtr->betray(mostPopulatedNationRecno);
			}
		}
	}
}
//------- End of function Unit::being_killed_subordinate_betray -------//


//------ Begin of function Unit::most_populated_nation_nearby ------//
//
// Return the most populated nation nearby. If there is none, pick one randomly.
//
// <int> the horizontal and vertical scanning range.
//
int Unit::most_populated_nation_nearby(int scanRange)
{
	int		 xOffset, yOffset;
	int		 xLoc, yLoc;
	Location* locPtr;
	int		 curXLoc = next_x_loc(), curYLoc = next_y_loc();
	BYTE	 	 regionId = world.get_region_id(curXLoc, curYLoc);
	short		 nationCountArray[MAX_RACE];

	memset( nationCountArray, 0, sizeof(nationCountArray) );

	for( int i=2 ; i<scanRange*scanRange ; i++ )
	{
		m.cal_move_around_a_point(i, scanRange, scanRange, xOffset, yOffset);

		xLoc = curXLoc + xOffset;
		yLoc = curYLoc + yOffset;

		xLoc = max(0, xLoc);
		xLoc = min(MAX_WORLD_X_LOC-1, xLoc);

		yLoc = max(0, yLoc);
		yLoc = min(MAX_WORLD_Y_LOC-1, yLoc);

		locPtr = world.get_loc(xLoc, yLoc);

		//----- if there is a unit on the location ------//

		if( locPtr->unit_recno(UNIT_LAND) )
		{
			Unit* unitPtr = unit_array[locPtr->unit_recno(UNIT_LAND)];

			if( unitPtr->nation_recno )
				nationCountArray[ unitPtr->nation_recno-1 ]++;
		}
	}

	//------ find out which race is most populated in the area -----//

	int maxNationCount=0, bestNationId=0;

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( nationCountArray[i] > maxNationCount )
		{
			maxNationCount = nationCountArray[i];
			bestNationId   = i+1;
		}
	}

	if( !bestNationId )
		bestNationId = m.random(nation_array.size())+1;		// if there is no human units nearby (perhaps just using weapons)

	return bestNationId;
}
//------- End of function Unit::most_populated_nation_nearby ------//


//--------- Begin of function Unit::set_remain_attack_delay --------//

void Unit::set_remain_attack_delay()
{
	// ####### begin Gilbert 16/12 ######//
	remain_attack_delay = attack_info_array[cur_attack].attack_delay;

	int delayBonus;
	if( (delayBonus = item.ability( ITEM_ABILITY_ATTACK_SPEED)) )
	{
		if( remain_attack_delay > delayBonus )
			remain_attack_delay -= delayBonus;
		else if( remain_attack_delay > 1 )
			remain_attack_delay = 1;
		item.use_now();
	}
	// ####### end Gilbert 16/12 ######//
}
//---------- End of function Unit::set_remain_attack_delay ----------//

//--------- Begin of function Unit::target_move ---------//
// called by process_attack_unit when the target is moved
//
// <Unit*> targetUnit	-	pointer to the target
//
void Unit::target_move(BaseObj* targetObj)
{
	int targetXLoc	= targetObj->obj_loc_x1();
	int targetYLoc = targetObj->obj_loc_y1();

	// #### begin Gilbert 4/11 ######//
	// int attackDistance = cal_distance(targetXLoc, targetYLoc, targetObj->obj_loc_width(), targetObj->obj_loc_height());
	int attackDistance = area_distance( targetObj );
	// #### end Gilbert 4/11 ######//
	
	cur_order.loc_x = targetXLoc;
	cur_order.loc_y = targetYLoc;
	
	//---------------------------------------------------------------------//
	// target is out of attacking range, move closer to it
	//---------------------------------------------------------------------//

	int curXLoc	= next_x_loc();
	int curYLoc	= next_y_loc();

	// ####### begin Gilbert 16/4 ########//
	// ------ prevent attack() or any other function changing original_target_x_loc
	int oriTargetXLoc = original_target_x_loc;
	int oriTargetYLoc = original_target_y_loc;
	// ####### end Gilbert 16/4 ########//

	// ###### begin Gilbert 15/3 ########//
	// if( attackDistance> (max_attack_range()-m.random(3) ))
	if( attackDistance> max_attack_range() )
	// ###### end Gilbert 15/3 ########//
	{
		//---------------------------------------------------------------------//
		// follow the target using the steps_remaining
		//---------------------------------------------------------------------//
		if(!update_attack_path_dist())
		{
			if(cur_action==SPRITE_MOVE || cur_action==SPRITE_WAIT || cur_action==SPRITE_READY_TO_MOVE)
				return;
		}

		attack(cur_order.para, false);

		// ####### begin Gilbert 16/4 ########//
		set_original_target( oriTargetXLoc, oriTargetYLoc );
		// ####### end Gilbert 16/4 ########//

	}
	else // attackDistance <= max_attack_range()
	{
		//-----------------------------------------------------------------------------//
		// although the target has moved, the unit can still attack it. no need to move
		//-----------------------------------------------------------------------------//

		// ####### begin Gilbert 16/12 ########//
//		if(abs(cur_x-next_x)>=sprite_info->speed || abs(cur_y-next_y)>=sprite_info->speed)
//			return;	// return as moving
		if(abs(cur_x-next_x)>=sprite_speed() || abs(cur_y-next_y)>=sprite_speed() )
			return;	// return as moving
		// ####### end Gilbert 16/12 ########//

		// ###### begin Gilbert 15/3 ########//
		// attack_now(targetObj, attackDistance);
		if( cur_action == SPRITE_ATTACK )
		{
			// change direction if needed

			if( attack_dir != get_dir(curXLoc, curYLoc, targetXLoc, targetYLoc) )
			{
				set_attack_dir(curXLoc, curYLoc, targetXLoc, targetYLoc);
				if(!is_dir_correct())
					set_turn();
			}

			// #### begin Gilbert 16/4 ########//
			// change attack if range needed
			if((attackDistance==1 && attack_range() > 1) || (attackDistance > 1 && attack_range() == 1)) // may change attack mode
			{
				int oldCurAttack = cur_attack;
				choose_best_attack_mode(attackDistance, targetObj->obj_mobile_type());
				if( cur_attack != oldCurAttack )
				{
					// no need to call cycle attack as attack has just changed
					cur_frame = 1;
				}
			}
			// #### end Gilbert 16/4 ########//
		}
		else
		{
			attack_now(targetObj, attackDistance);
		}
		// ###### end Gilbert 15/3 ########//

		/*
		if((attackDistance==1 && attack_range() > 1) || (attackDistance > 1 && attack_range() == 1)) // may change attack mode
			choose_best_attack_mode(attackDistance, targetObj->obj_mobile_type());

		if(attack_range() > 1) // range attack
		{
			//------------------ do range attack ----------------------//

			if(bullet_array.add_bullet_possible(curXLoc, curYLoc, mobile_type, targetXLoc, targetYLoc, targetObj->obj_mobile_type(), targetObj->obj_loc_width(),
				targetObj->obj_loc_height(), range_attack_x_loc, range_attack_y_loc, cur_attack_info()->bullet_speed, cur_attack_info()->bullet_sprite_id))	// range attack possible
			{
				set_cur(next_x, next_y);
				err_when(range_attack_x_loc==-1 || range_attack_y_loc==-1);

				set_attack_dir(curXLoc, curYLoc, range_attack_x_loc, range_attack_y_loc);
				stop_move();

//				if(is_dir_correct())
//					bullet_array.add_bullet(this, targetObj);
//				cycle_eqv_attack();
				//cur_frame  = 1;

				if(is_dir_correct())
					attack_now(targetObj, attackDistance);
				else
					set_turn();
			}
		}

		else // close attack
		{
			set_cur(next_x, next_y);
			int closeAttackLocX=targetXLoc;
			int closeAttackLocY=targetYLoc;
			close_attack_location(targetObj, closeAttackLocX, closeAttackLocY);
			set_attack_dir(curXLoc, curYLoc, closeAttackLocX, closeAttackLocY);

			if(is_dir_correct() || (mobile_type == UNIT_AIR && !attackDistance))
				attack_now(targetObj, attackDistance);
			else
				set_turn();
		}
		*/
	}

	// after target move, original_target_x_loc should not be updated,
	// otherwise it keeps chasing
	err_when( original_target_x_loc != oriTargetXLoc || original_target_y_loc != oriTargetYLoc );
}
//---------- End of function Unit::target_move ----------//

//----------------- Begin of function Unit::update_attack_path_dist -----------------//
// return 1 if it is time to update result path
// return 0 otherwise
//
int Unit::update_attack_path_dist()
{
	if(steps_remaining<=6)	//1-6
	{
		return 1;
	}
	else if(steps_remaining<=10) // 8, 10
	{
		return !((steps_remaining-6)%2);
	}
	else if(steps_remaining<=20) // 15, 20
	{
		return !((steps_remaining-10)%5);
	}
	else if(steps_remaining<=60) // 28, 36, 44, 52, 60
	{
		return !((steps_remaining-20)%8);
	}
	else if(steps_remaining<=90) // 75, 90
	{
		return !((steps_remaining-60)%15);
	}
	else // 110, 130, 150, etc
	{
		return !((steps_remaining-90)%20);
	}

	err_here();
	return 0;
}
//---------- End of function Unit::update_attack_path_dist ----------//


//---------- Begin of find_new_location_for_attack --------------------//
//
// If both attack_loc_offset_x and attack_loc_offset_y is 0 after running this function, that means
// no suitable location is found for the unit to attack.
//
void Unit::find_new_location_for_attack(BaseObj* targetObj)
{
	if(attack_range() > 1)
	{
		find_new_position_for_range_attack(targetObj);
		return;
	}

	int targetLocX = targetObj->obj_loc_x1();
	int targetLocY = targetObj->obj_loc_y1();
	int targetWidth = targetObj->obj_loc_width();
	int targetHeight = targetObj->obj_loc_height();

	err_when(targetLocX < 0);
	err_when(targetLocX >= MAX_WORLD_X_LOC);
	err_when(targetLocY < 0);
	err_when(targetLocY >= MAX_WORLD_Y_LOC);

	if(targetLocX == -1 || targetLocY == -1)
	{
		attack_loc_offset_x = 0;
		attack_loc_offset_y = 0;
		return;
	}

	int testLocX, testLocY, index;
	int w ,h;

	if((testLocY = targetLocY-obj_loc_height()) >= 0) //has top edge
	{
		w = obj_loc_width();
		
		while(w>=0)
		{
			if(targetLocX >= w)
			{							//has left top corner
				index = -w;
				break;
			}
			w--;
		}

		for(; index < targetWidth; index++)
		{
			if(can_move(targetLocX+index, testLocY))
			{
				attack_loc_offset_x = index;
				attack_loc_offset_y = -obj_loc_height();
				err_when(targetLocX + attack_loc_offset_x < 0);
				err_when(targetLocX + attack_loc_offset_x >= MAX_WORLD_X_LOC);
				err_when(targetLocY + attack_loc_offset_y < 0);
				err_when(targetLocY + attack_loc_offset_y >= MAX_WORLD_Y_LOC);
				return;
			}
		}
	}
	if((testLocX = targetLocX+targetWidth) < MAX_WORLD_X_LOC-obj_loc_width()) //has right edge
	{
		h = obj_loc_height();
		
		while(h>=0)
		{
			if(targetLocY >= h)
			{
				index = -h;
				break;
			}
			h--;
		}

		for(; index < targetHeight; index++)
		{
			if(can_move(testLocX, targetLocY+index))
			{
				attack_loc_offset_x = targetWidth;
				attack_loc_offset_y = index;
				err_when(targetLocX + attack_loc_offset_x < 0);
				err_when(targetLocX + attack_loc_offset_x >= MAX_WORLD_X_LOC);
				err_when(targetLocY + attack_loc_offset_y < 0);
				err_when(targetLocY + attack_loc_offset_y >= MAX_WORLD_Y_LOC);

				return;
			}
		}
	}
	if((testLocY = targetLocY+targetHeight) < MAX_WORLD_Y_LOC-obj_loc_height()) //has bottom edge
	{
		w = obj_loc_width();
		
		while(w>=0)
		{
			if(targetLocX+targetWidth < MAX_WORLD_X_LOC-w)
			{
				index = targetWidth-(obj_loc_width()-w);
				break;
			}
			w--;
		}

		for(; index >= 0; index--)
		{
			if(can_move(targetLocX+index, testLocY))
			{
				attack_loc_offset_x = index;
				attack_loc_offset_y = targetHeight;
				err_when(targetLocX + attack_loc_offset_x < 0);
				err_when(targetLocX + attack_loc_offset_x >= MAX_WORLD_X_LOC);
				err_when(targetLocY + attack_loc_offset_y < 0);
				err_when(targetLocY + attack_loc_offset_y >= MAX_WORLD_Y_LOC);

				return;
			}
		}
	}
	if((testLocX=targetLocX-obj_loc_width()) >= 0) //has left edge
	{
		h = obj_loc_height();

		while(h>=0)
		{
			if(targetLocY+targetHeight < MAX_WORLD_Y_LOC-h)
			{
				index = targetHeight-(obj_loc_height()-h);
				break;
			}
			h--;
		}

		for(; index >= 0; index--)
		{
			if(can_move(testLocX, targetLocY+index))
			{
				attack_loc_offset_x = -obj_loc_width();
				attack_loc_offset_y = index;
				err_when(targetLocX + attack_loc_offset_x < 0);
				err_when(targetLocX + attack_loc_offset_x >= MAX_WORLD_X_LOC);
				err_when(targetLocY + attack_loc_offset_y < 0);
				err_when(targetLocY + attack_loc_offset_y >= MAX_WORLD_Y_LOC);

				return;
			}
		}
	}

	attack_loc_offset_x = 0;
	attack_loc_offset_y = 0;
}
//-----------------End of function Unit::find_new_position_attack -----------------//

//--------- Begin of function Unit::find_new_position_for_range_attack ---------//
//
void Unit::find_new_position_for_range_attack(BaseObj* targetObj)
{
	int targetXLoc = targetObj->obj_loc_x1();
	int targetYLoc = targetObj->obj_loc_y1();
	int targetWidth = targetObj->obj_loc_width();
	int targetHeight = targetObj->obj_loc_height();
	int curXLoc = next_x_loc();
	int curYLoc = next_y_loc();

	AttackInfo *attackInfo = cur_attack_info();
	int attackRange = attack_range();

	err_when(targetXLoc<0 || targetXLoc>=MAX_WORLD_X_LOC || targetYLoc<0 || targetYLoc>=MAX_WORLD_Y_LOC);

	int xLoc1 = max(targetXLoc-attackRange, 0);
	int yLoc1 = max(targetYLoc-attackRange, 0);
	int xLoc2 = min(targetXLoc+targetWidth-1+attackRange, MAX_WORLD_X_LOC-1);
	int yLoc2 = min(targetYLoc+targetHeight-1+attackRange, MAX_WORLD_Y_LOC-1);
	int checkXLoc, checkYLoc;

	// #### begin Gilbert 4/11 ######//
// 	if((cal_distance(targetXLoc, targetYLoc, targetWidth, targetHeight) <= attackRange)
	if( area_distance(targetObj) <= attackRange
	// #### end Gilbert 4/11 ######//
		&& bullet_array.add_bullet_possible(curXLoc, curYLoc, mobile_type, targetXLoc, targetYLoc,
			 targetObj->obj_mobile_type(), targetWidth, targetHeight,
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id)) // inside the attack range
	{
		attack_loc_offset_x = curXLoc-targetXLoc;
		attack_loc_offset_y = curYLoc-targetYLoc;
		return;
	}

	//-------- checking for surrounding location ----------//
	for(checkXLoc=xLoc1; checkXLoc<=xLoc2; checkXLoc++)
	{
		if(can_move(checkXLoc, yLoc1) &&
			bullet_array.add_bullet_possible(checkXLoc, yLoc1, mobile_type, targetXLoc, targetYLoc,
			 targetObj->obj_mobile_type(), targetWidth, targetHeight,
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id))
		{
			attack_loc_offset_x = checkXLoc-targetXLoc;
			attack_loc_offset_y = yLoc1-targetYLoc;
			return;
		}
		
		if(can_move(checkXLoc, yLoc2) && 
			bullet_array.add_bullet_possible(checkXLoc, yLoc2, mobile_type, targetXLoc, targetYLoc,
			 targetObj->obj_mobile_type(), targetWidth, targetHeight,
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id))
		{
			attack_loc_offset_x = checkXLoc-targetXLoc;
			attack_loc_offset_y = yLoc2-targetYLoc;
			return;
		}
	}

	for(checkYLoc=yLoc1+1; checkYLoc<yLoc2; checkYLoc++)
	{

		if(can_move(xLoc1, checkYLoc) &&
			bullet_array.add_bullet_possible(xLoc1, checkYLoc, mobile_type, targetXLoc, targetYLoc,
			 targetObj->obj_mobile_type(), targetWidth, targetHeight,
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id))
		{
			attack_loc_offset_x = xLoc1-targetXLoc;
			attack_loc_offset_y = checkYLoc-targetYLoc;
			return;
		}


		if(can_move(xLoc2, checkYLoc) &&
			bullet_array.add_bullet_possible(xLoc2, checkYLoc, mobile_type, targetXLoc, targetYLoc,
			 targetObj->obj_mobile_type(), targetWidth, targetHeight,
			 range_attack_x_loc, range_attack_y_loc, attackInfo->bullet_speed, attackInfo->bullet_sprite_id))
		{
			attack_loc_offset_x = xLoc2-targetXLoc;
			attack_loc_offset_y = checkYLoc-targetYLoc;
			return;
		}
	}

	attack_loc_offset_x = 0;
	attack_loc_offset_y = 0;
}	
//---------------End of function Unit::find_new_position_range_attack----------------//


//------ Begin of function Unit::set_attack_dir ---------//
// set direction for attacking
//
//	<short>	curX		-	x location of the unit
// <short>	curY		-	y location of the unit
// <short>	targetX	-	x location of the target
// <short>	targetY	-	y location of the target
//
void Unit::set_attack_dir(short curX, short curY, short targetX, short targetY)
{
	UCHAR targetDir = get_dir(curX, curY, targetX, targetY);
	attack_dir = targetDir;
	set_dir(targetDir);
}
//---------- End of function Unit::set_attack_dir ----------//


//----------- Begin of function Unit::max_attack_range -----------//
//
// return the maximum attack range the unit can make
//
int Unit::max_attack_range()
{
	int maxRange=0;

	AttackInfo *attackInfo = attack_info_array;

	for(int i=0; i<attack_count; i++, attackInfo++)
	{
		if(can_attack_with(attackInfo) &&
			attackInfo->attack_range>maxRange)
			maxRange = attackInfo->attack_range;
	}
	
	return maxRange;
}
//----------- End of function Unit::max_attack_range -----------//

//----------- Begin of function Unit::close_attack_location------//
// find the location of close attack 
void Unit::close_attack_location(BaseObj* targetObj, int& closeAttackLocX, int& closeAttackLocY)
{
	int targetWidth = targetObj->obj_loc_width();
	int targetHeight = targetObj->obj_loc_height();
	int targetLocX = targetObj->obj_loc_x1();
	int targetLocY = targetObj->obj_loc_y1();

	if(targetWidth == 1 && targetHeight == 1)
	{
		closeAttackLocX = targetLocX;
		closeAttackLocY = targetLocY;
		return;
	}

	short	adjWidth	= targetWidth-1;	// adjusted width;
	short	adjHeight = targetHeight-1; // adjusted height
	short xOffset=0, yOffset=0;	// the 1st(default) location for attack

	//----------------------------------------------------------------------//
	// determine initial xOffset
	//----------------------------------------------------------------------//
	if(next_x_loc() <= targetLocX)
		xOffset = 0; // the left hand side of the target
	else if(next_x_loc() >= targetLocX+adjWidth)
		xOffset = adjWidth; // the right hand side of the target
	else
		xOffset = next_x_loc() - targetLocX; // in the middle(vertical) of the target
		
	//----------------------------------------------------------------------//
	// determine initial yOffset
	//----------------------------------------------------------------------//
	if(next_y_loc() <= targetLocY)
		yOffset = 0;	// the upper of the target
	else if(next_y_loc() >= targetLocY+adjHeight)
		yOffset = adjHeight;
	else
		yOffset = next_y_loc() - targetLocY; // in the middle(horizontal) of the target

	closeAttackLocX = targetLocX+xOffset;
	closeAttackLocY = targetLocY+yOffset;
}
