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

//Filename	 : OUN_ATK2.CPP
//Description: Unit attacking functions

#include <ounitres.h>
#include <oitem.h>
#include <oeffect.h>
#include <oitemres.h>
#include <itemid.h>
#include <ofirm.h>
#include <otech.h>
#include <otechres.h>
#include <oraceres.h> 
#include <ogodres.h>
#include <ounit.h>
#include <ofirm.h>
#include <osys.h>
#include <oweather.h>

//--------- Begin of function Unit::choose_best_attack_mode ---------//
//
// if the unit has more than one attack mode, select the suitable mode
// to attack the target
//
// <int>  attackDistance	- the distance from the target
// [char] targetMobileType	- the target's mobile_type (default: UNIT_LAND)
//
void Unit::choose_best_attack_mode(int attackDistance, char targetMobileType)
{
	//------------- define parameters -----------------------//

	UCHAR attackModeBeingUsed = cur_attack;
	err_when(attackModeBeingUsed<0 || attackModeBeingUsed>MAX_UNIT_ATTACK_TYPE);
	UCHAR maxAttackRangeMode = cur_attack;
	AttackInfo* attackInfoMaxRange = attack_info_array;
	AttackInfo* attackInfoChecking;
	AttackInfo* attackInfoSelected = attack_info_array+cur_attack;

	//--------------------------------------------------------------//
	// If targetMobileType==UNIT_AIR or mobile_type==UNIT_AIR,
	//	force to use range_attack.
	// If there is no range_attack, return 0, i.e. cur_attack=0
	//--------------------------------------------------------------//

	if(attack_count>1)
	{
		int canAttack = 0;
		int checkingDamageWeight, selectedDamageWeight;

		for(UCHAR i=0; i<attack_count; i++)
		{
			if(attackModeBeingUsed==i)
				continue; // it is the mode already used

			attackInfoChecking = attack_info_array+i;
			if(can_attack_with(attackInfoChecking) && attackInfoChecking->attack_range>=attackDistance)
			{
				//-------------------- able to attack ----------------------//
				canAttack = 1;

				if(attackInfoSelected->attack_range<attackDistance)
				{
					attackModeBeingUsed = UCHAR(i);
					attackInfoSelected = attackInfoChecking;
					continue;
				}

				checkingDamageWeight = attackInfoChecking->attack_damage;
				selectedDamageWeight = attackInfoSelected->attack_damage;

				if(attackDistance==1 && (targetMobileType!=UNIT_AIR && mobile_type!=UNIT_AIR))
				{
					//------------ force to use close attack if possible -----------//
					if(attackInfoSelected->attack_range==attackDistance)
					{
						if(attackInfoChecking->attack_range==attackDistance && checkingDamageWeight>selectedDamageWeight)
						{
							attackModeBeingUsed = UCHAR(i); // choose the one with strongest damage
							attackInfoSelected = attackInfoChecking;
						}
						continue;
					}
					else if(attackInfoChecking->attack_range==1)
					{
						attackModeBeingUsed = UCHAR(i);
						attackInfoSelected = attackInfoChecking;
						continue;
					}
				}

				//----------------------------------------------------------------------//
				// further selection
				//----------------------------------------------------------------------//
				if(checkingDamageWeight == selectedDamageWeight)
				{
					if(attackInfoChecking->attack_range<attackInfoSelected->attack_range)
					{
						if(attackInfoChecking->attack_range>1 || (targetMobileType!=UNIT_AIR && mobile_type!=UNIT_AIR))
						{
							//--------------------------------------------------------------------------//
							// select one with shortest attack_range
							//--------------------------------------------------------------------------//
							attackModeBeingUsed = UCHAR(i);
							attackInfoSelected = attackInfoChecking;
						}
					}
				}
				else
				{
					//--------------------------------------------------------------------------//
					// select one that can do the attacking immediately with the strongest damage point
					//--------------------------------------------------------------------------//
					attackModeBeingUsed = UCHAR(i);
					attackInfoSelected = attackInfoChecking;
				}
			}

			if(!canAttack)
			{
				//------------------------------------------------------------------------------//
				// if unable to attack the target, choose the mode with longer attack_range and
				// heavier damage
				//------------------------------------------------------------------------------//
				if(can_attack_with(attackInfoChecking) &&
					(attackInfoChecking->attack_range>attackInfoMaxRange->attack_range ||
					(attackInfoChecking->attack_range==attackInfoMaxRange->attack_range &&
					 attackInfoChecking->attack_damage>attackInfoMaxRange->attack_damage)))
				{
					maxAttackRangeMode = UCHAR(i);
					attackInfoMaxRange = attackInfoChecking;
				}
			}
		}

		if(canAttack)
			cur_attack = attackModeBeingUsed;	// choose the strongest damage mode if able to attack
		else
			cur_attack = maxAttackRangeMode;		//	choose the longest attack range if unable to attack
	}
	else
	{ 
		cur_attack = 0;	// only one mode is supported
	}

	//-----------------------------------------------//

	err_when(final_dir<0 || final_dir>=MAX_SPRITE_DIR_TYPE);
	err_when(cur_attack>=attack_count || cur_attack<0);
}
//---------- End of function Unit::choose_best_attack_mode ----------//


//--------- Begin of function Unit::cal_distance ---------//
//
// calculate the distance from this unit to the target
// (assume the size of this unit is a square)
//
// <int>	targetXLoc		- x location of the target
// <int>	targetYLoc		- y location of the target
// <int> targetWidth		- target width
// <int> targetHeight	- target height
//
int Unit::cal_distance(int targetXLoc, int targetYLoc, int targetWidth, int targetHeight)
{
	int curXLoc = next_x_loc();
	int curYLoc = next_y_loc();
	int dispX=0, dispY=0;

	if(curXLoc<targetXLoc)
		dispX = (targetXLoc - curXLoc - loc_width) + 1;
	else if((dispX=curXLoc-targetXLoc-targetWidth+1)<0)
		dispX  = 0;

	err_when(dispX<0 || dispX>=MAX_WORLD_X_LOC);

	if(curYLoc<targetYLoc)
	{
		dispY = (targetYLoc - curYLoc - loc_height) + 1;
	}
	else if((dispY=curYLoc-targetYLoc-targetHeight+1)<0)
	{
	//	err_when(!dispX); // the target overlaps with the current unit
		return dispX;
	}

	err_when(dispY<0 || dispY>=MAX_WORLD_Y_LOC);

	return max(dispX,dispY);
}
//----------- End of function Unit::cal_distance -----------//


//----------- Begin of function Unit::can_attack_with -------//

int Unit::can_attack_with(int attackId)
{
	err_when( attackId< 0 || attackId >= attack_count);

	AttackInfo *attackInfo = attack_info_array+attackId;

	// ##### begin Gilbert 5/11 ########//
	//
	// special unit berserk attack has to be researched
	// another approach is to add tech_id in attackInfo
	//
	if( attackInfo->consume_power >= 20 && is_human() 
		&& unit_id == race_res[race_id]->special_unit_id )
	{
		err_when( race_id > MAX_RACE );

		if( !nation_recno 
			|| !tech_res[TECH_SPU_BERSERK(race_id)]->get_nation_tech_level(nation_recno) )
		{
			return 0;
		}
	}
	// ##### end Gilbert 5/11 ########//

	// ##### patch begin Gilbert 26/2 ######//
	int skillBonus;
//	if( race_id == RACE_VIKING && nation_recno && attackInfo->consume_power > 20
//		&& god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
	if( nation_recno && attackInfo->consume_power > 20
		&& god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
		skillBonus = 25;		// viking soldier can use power attack earlier
	// ##### patch end Gilbert 26/2 ######//
	// ##### patch begin Gilbert 29/3 ######//
	else if( unit_id == UNIT_ROCK && attackInfo->consume_power <= 0
		&& unit_mode == UNIT_MODE_OVERSEE && !firm_array.is_deleted(unit_mode_para)
		&& firm_array[unit_mode_para]->firm_id == FIRM_FORTRESS )
		skillBonus = 25;		// grokken in monster fortress can fire at most combat level
	// ##### patch end Gilbert 29/3 ######//
	else
		skillBonus = 0;

	return ((combat_level()+skillBonus >= attackInfo->combat_level &&
			 cur_power >= attackInfo->min_power) || (sys.testing_session));
}

int Unit::can_attack_with(AttackInfo *attackInfo)
{
	// ##### begin Gilbert 5/11 ########//
	//
	// special unit berserk attack has to be researched
	// another approach is to add tech_id in attackInfo
	//
	if( attackInfo->consume_power >= 20 && is_human() 
		&& unit_id == race_res[race_id]->special_unit_id )
	{
		err_when( race_id > MAX_RACE );

		if( !nation_recno 
			|| !tech_res[TECH_SPU_BERSERK(race_id)]->get_nation_tech_level(nation_recno) )
		{
			return 0;
		}
	}
	// ##### end Gilbert 5/11 ########//

	// ##### patch begin Gilbert 17/2 ######//
	int skillBonus;
//	if( race_id == RACE_VIKING && nation_recno && attackInfo->consume_power > 20
//		&& god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
	if( nation_recno && attackInfo->consume_power > 20
		&& god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
		skillBonus = 25;		// viking soldier can use power attack earlier
	// ##### patch Gilbert 17/2 ######//
	// ##### patch begin Gilbert 29/3 ######//
	else if( unit_id == UNIT_ROCK && attackInfo->consume_power <= 0
		&& unit_mode == UNIT_MODE_OVERSEE && !firm_array.is_deleted(unit_mode_para)
		&& firm_array[unit_mode_para]->firm_id == FIRM_FORTRESS )
		skillBonus = 25;		// grokken in monster fortress can fire at most combat level
	// ##### patch end Gilbert 29/3 ######//
	else
		skillBonus = 0;

	return ((combat_level()+skillBonus >= attackInfo->combat_level &&
			 cur_power >= attackInfo->min_power) || (sys.testing_session));
}
//----------- End of function Unit::can_attack_with -------//


//----------- Begin of function Unit::cycle_eqv_attack -----------//
void Unit::cycle_eqv_attack()
{
	int trial = MAX_UNIT_ATTACK_TYPE+2;

	if( attack_info_array[cur_attack].eqv_attack_next > 0 )
	{
		do
		{
			cur_attack = attack_info_array[cur_attack].eqv_attack_next-1;

			err_when(cur_attack < 0 || cur_attack >= attack_count);
			err_when(--trial == 0);
		}
		while( !can_attack_with(cur_attack) );
	}
}
//----------- End of function Unit::cycle_eqv_attack -----------//


//--------- Begin of function Unit::actual_damage --------//
//
// Return: return the actual hit damage this unit can do to a target.
//
float Unit::actual_damage()
{
	AttackInfo *attackInfo = cur_attack_info();

	// ###### begin Gilbert 22/10 ######//
	int attackDamage = attackInfo->attack_damage;
	int deciDamage = 0;			// extra damage multiplied by 100

	int combatLevel = combat_level();

	// -------- pierce damage -------- //

	attackDamage += m.random(3);
	if( 100 == MAX_COMBAT_TRAIN )
	{
		deciDamage += attackInfo->pierce_damage * m.random( 1+min(combatLevel, MAX_COMBAT_TRAIN) );
	}
	else
	{
		deciDamage += attackInfo->pierce_damage * m.random( 1+min(combatLevel, MAX_COMBAT_TRAIN) )
			* 100 / MAX_COMBAT_TRAIN;
	}

	// -------- attack damage modified by item ----------//

	int itemDamage = item.ability(	attackInfo->attack_range>1?ITEM_ABILITY_ARROW_DAMAGE:ITEM_ABILITY_MELEE_DAMAGE );
	if( itemDamage )
	{
		attackDamage += itemDamage;
		item.use_now();
	}

	// ---------- increase damage from hero -------------//

	if( combatLevel > MAX_COMBAT_TRAIN )
	{
		deciDamage += combatLevel * (100 / 50);	// 1 more basic damage for every 50 combat skill
	}

	// ---------- increase damage from research ---------//

	if( nation_recno && is_human() )
	{
		if( unit_id == race_res[race_id]->infantry_unit_id 
			|| unit_id == race_res[race_id]->special_unit_id )
		{
			if( attack_range() <= 1 )
				attackDamage += tech_res[TECH_INFANTRY_CLOSE_COMBAT]->get_nation_tech_level(nation_recno) * 2;
			else
				attackDamage += tech_res[TECH_INFANTRY_RANGE_COMBAT]->get_nation_tech_level(nation_recno) * 2;
		}
	}

	if( unit_id == UNIT_PLANT && !weather.cloud() )
		attackDamage += 2;

	//--- if this unit is led by a general, its attacking ability is influenced by the general ---//
	//
	// The unit's attacking ability is increased by a percentage equivalent to
	// the leader unit's leadership.
	//
	//------------------------------------------------------------------------//

	if( leader_unit_recno )
	{
		if( unit_array.is_deleted(leader_unit_recno) )
		{
			leader_unit_recno = 0;
		}
		else
		{
			// ##### begin Gilbert 22/10 ######//
			attackDamage += deciDamage / 100;	// normalize damage
			deciDamage = deciDamage % 100;
			// ##### end Gilbert 22/10 ######//
		
			Unit* leaderUnit = unit_array[leader_unit_recno];
			int	leaderXLoc, leaderYLoc;

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
				 m.points_distance(cur_x_loc(), cur_y_loc(), leaderXLoc, leaderYLoc) <= EFFECTIVE_LEADING_DISTANCE )
			{
				// ##### begin Gilbert 17/6 #####//
				// deciDamage += attackDamage * leaderUnit->skill_level();
				int leaderBonus = attackDamage * leaderUnit->skill_level();
				deciDamage += min( leaderBonus, 1000 );	// 10 damage, avoid cannon or catapult too strong
				// ##### end Gilbert 17/6 #####//
				// ##### begin Gilbert 29/3 ######//
				if( leaderUnit->race_id == RACE_CELTIC && unit_id == UNIT_CELTIC_SPU )
				{
					deciDamage += min( leaderBonus, 1000 );	// 10 damage, avoid cannon or catapult too strong
				}
				// ##### end Gilbert 29/3 ######//
			}
		}
	}

	// return (float) attackDamage / ATTACK_SLOW_DOWN;		// lessen all attacking damages, thus slowing down all battles.
	// ###### begin Gilbert 17/6 #######//
	return (float)deciDamage * 0.01f + (float)attackDamage;		// divide ATTACK_SLOW_DOWN in calculating damage
	// ###### end Gilbert 17/6 #######//
}
//------------ End of function Unit::actual_damage --------------//

//--------- Begin of function Unit::obj_armor --------//
//
float Unit::obj_armor()
{
	int armor = unit_res[unit_id]->armor;

	int deciArmor = 0;
	if( combat_level() > MAX_COMBAT_TRAIN )
		deciArmor += combat_level() * (100 / 50);

	int def = item.ability( ITEM_ABILITY_ARMOR );

	if(def)
	{
		armor += def;
		item.use_now();
	}

	// ------ defense ability modified by nation technology -----//

	// ##### begin Gilbert 22/10 ########//
	if( nation_recno && is_human() )
	// ##### end Gilbert 22/10 ########//
	{
		if( unit_id == race_res[race_id]->infantry_unit_id 
			|| unit_id == race_res[race_id]->special_unit_id )
		{
			armor += tech_res[TECH_INFANTRY_DEFENSE]->get_nation_tech_level(nation_recno) * 2;
		}
	}

	// ###### patch begin Gilbert 13/7 #######//
	if( unit_id == UNIT_TERMITE )
		armor += 4;
	// ###### patch end Gilbert 13/7 #######//

	// ###### begin Gilbert 22/10 #######//
	return (float) deciArmor / 100.0f + (float) armor;
	// ###### end Gilbert 22/10 #######//
}
//---------- End of function Unit::obj_armor ----------//


//----------- Begin of function Unit::add_attack_effect -------//

void Unit::add_attack_effect()
{
	AttackInfo *attackInfo = cur_attack_info();
//	short effectId = (attack_info_array+cur_attack)->effect_id;
	short effectId = attackInfo->effect_id;

	// ###### begin Gilbert 24/3 #######//
	if( attackInfo->effect2_id != 0 )
	{
#if( defined(GERMAN) )
		// german version don't use blood effect
		effectId = attackInfo->effect2_id;
#else
		Unit *targetUnit;

		// has two effect to choose
		if( !base_obj_array.is_deleted(cur_order.para)
			&& (targetUnit = base_obj_array[cur_order.para]->cast_to_Unit())
			&& unit_res[targetUnit->unit_id]->class_info.life > 5 )	 // exclude insect and wagon
		{
			effectId = attackInfo->effect_id;		// blood
		}
		else
		{
			effectId = attackInfo->effect2_id;
		}
#endif
	}
	// ###### end Gilbert 24/3 #######//

	if( effectId )
	{
		short x,y;
		get_hit_x_y(&x, &y, attackInfo->effect_dis);
		Effect::create(effectId, x, y, SPRITE_IDLE, cur_dir, mobile_type == UNIT_AIR ? 8 : 2, 0);
	}
}
//----------- End of function Unit::add_attack_effect -------//

//----------- Begin of function Unit::add_die_effect -------//

void Unit::add_die_effect()
{
	UnitInfo *unitInfo = unit_res[unit_id];
	short effectId = unitInfo->die_effect_id;
	if( effectId )
	{
		Effect::create(effectId, cur_x, cur_y, SPRITE_IDLE, 1, mobile_type == UNIT_AIR ? 8 : 2, 0);
	}
}
//----------- End of function Unit::add_die_effect -------//

//----------- Begin of function Unit::get_hit_x_y -------//

void Unit::get_hit_x_y(short *xPtr, short *yPtr, char distance)
{
	switch(cur_dir)
	{
	
	case 0:	// north
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 - LOCATE_HEIGHT * distance;
		break;
	case 1:	// north east
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 + LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 - LOCATE_HEIGHT * distance;
		break;
	case 2:	// east
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 + LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2;
		break;
	case 3:	// south east
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 + LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 + LOCATE_HEIGHT * distance;
		break;
	case 4:	// south
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 + LOCATE_HEIGHT * distance;
		break;
	case 5:	// south west
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 - LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 + LOCATE_HEIGHT * distance;
		break;
	case 6:	// west
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 - LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2;
		break;
	case 7:	// north west
		// later replace 1 with effect's loc_width
		*xPtr = cur_x + LOCATE_WIDTH * loc_width/2 - LOCATE_WIDTH * 1/2 - LOCATE_WIDTH * distance;
		*yPtr = cur_y + LOCATE_HEIGHT * loc_height/2 - LOCATE_HEIGHT * 1/2 - LOCATE_HEIGHT * distance;
		break;
	default:
		err_here();
		*xPtr = cur_x;
		*yPtr = cur_y;
	}
}
//----------- End of function Unit::get_hit_x_y -------//


// --------- begin of function Unit::is_die_after_attack --------//
//
// if is_die_after_attack() , behavior mode is always UNIT_STAND_GROUND
// Gilbert : change to default DEFENSIVE
//
int Unit::is_die_after_attack()
{
	for( int i = 0; i < attack_count; ++i )
	{
		if( attack_info_array[i].die_after_attack && can_attack_with(i) )
			return 1;
	}
	return 0;
}
// --------- end of function Unit::is_die_after_attack --------//


// ###### patch begin Gilbert 9/11 ########//
// --------- begin of function Unit::attack_consume_power ------//
//
void Unit::attack_consume_power()
{
	AttackInfo *attackInfo = cur_attack_info();

	if (attackInfo->die_after_attack)
		hit_points = 0.0;

	//------- reduce power --------//

	cur_power -= attackInfo->consume_power;

	if(cur_power<0)		// for fixing bug on-fly
		cur_power = 0;
}
// --------- end of function Unit::attack_consume_power ------//
// ###### patch end Gilbert 9/11 ########//
