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

//Filename    : OF_CAMPA.CPP
//Description : Firm Military Camp - AI functions

#include <onation.h>
#include <oinfo.h>
#include <obattle.h>
#include <oconfig.h>  
#include <otown.h>
#include <ospy.h>
#include <ounit.h>
#include <oun_grp.h>
#include <of_inn.h>
#include <of_lair.h>
#include <of_camp.h>
#include <ogame.h>

//--------- Begin of function FirmCamp::process_ai ---------//

void FirmCamp::process_ai()
{
	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return;	

	//--- think about building a firm and employ independent citizens to lower their resistance ---//

	if( info.game_date%30==(firm_recno+5)%30 )
		think_employ_to_capture();

	//----- think about using cash to capture -------//

	if( info.game_date%15==(firm_recno+10)%15 )		// do not call too often as the penalty of accumulation is 10 days
		think_use_cash_to_capture();

	//------- assign overseer and soldiers -------//

	if( info.game_date%15==firm_recno%15 )		// do not call too often because when an AI action is queued, it will take a while to carry it out
		think_recruit();

	//---- if this firm is currently trying to capture a town ----//

	if( ai_capture_town_recno )
	{
		should_close_flag = 0;		// disable should_close_flag when trying to capture a firm, should_close_flag is set in process_common_ai()

		if( nation_array[nation_recno]->attack_camp_count==0 &&		// only when attack_camp_count==0, the attack mission is complete
			 patrol_unit_count==0 )
		{
			ai_capture_town_recno = 0;
			defense_flag = 1;    			// turn it on again after the capturing plan is finished
			return;
		}

		return;
	}

	//--- if the firm is empty and should be closed, sell/destruct it now ---//

	if( should_close_flag )
	{
		if( soldier_count==0 )
		{
			ai_del_firm();
			return;
		}
	}

	//----- think about assigning a better commander -----//

	if( info.game_date%30==firm_recno%30 )
		think_assign_better_commander();

	//----- think about changing links to foreign town -----//

	if( info.game_date%30==firm_recno%30 )
		think_change_town_link();

	//------ think about attacking invaders -------//

	int checkInterval = 13 - nation_array[nation_recno]->pref_military_development/10;

	if( info.game_date%checkInterval == firm_recno%checkInterval )
		think_attack_invader();

	//------ think about attacking nearby enemy -----//

	if( info.game_date%30==firm_recno%30 )
		think_attack_nearby_enemy();

	//------ think about capturing independent town -------//

	static short interval_days_array[] = { 250, 60, 30, 20, 10 };

	int intervalDays = interval_days_array[config.ai_aggressiveness];

	if( info.game_date%intervalDays == firm_recno%intervalDays )		// do not call too often because when an AI action is queued, it will take a while to carry it out
		think_capture();

	//------ think attack enslaving fryhtans ------//

	if( info.game_date%intervalDays == (firm_recno+5)%intervalDays )		// do not call too often because when an AI action is queued, it will take a while to carry it out
		think_attack_enslaving_monster();

	//----- think about upgrading to a fort  -----//

	if( info.game_date%30 == (firm_recno+15)%30 )
		think_upgrade_to_fort();
}
//----------- End of function FirmCamp::process_ai -----------//


//--------- Begin of function FirmCamp::process_ai_capturing ---------//
//
// This function is called when the AI is in the process of
// attacking and capturing the independent town this camp is
// linked to.
//
void FirmCamp::process_ai_capturing()
{
	err_when( patrol_unit_count <= 0 );		// this function shouldn't be called at all if patrol_unit_count==0
	err_when( patrol_unit_count>9 );

	if( !ai_capture_town_recno )
		return;

	if( think_capture_return() )	// if the capturing units should now return to their base.
		return;

	//--- there are still town defender out there, order idle units to attack them ---//

	Unit* unitPtr;
	Town* townPtr = town_array[ai_capture_town_recno];

	if( townPtr->town_defender_count > 0 )
	{
		for( int i=patrol_unit_count ; i>0 ; i-- )
		{
			unitPtr = unit_array[ patrol_unit_array[i-1] ];

			if( unitPtr->cur_action == SPRITE_IDLE )
				ai_attack_town_defender(unitPtr);
		}
	}

	//--- if the town is still not captured but all mobile town defenders are destroyed, attack the town again ---//

	if( townPtr->town_defender_count == 0 )
	{
		//----- have one of the units attack the town ----//

		err_when( patrol_unit_count<=0 );

		if( townPtr->nation_recno )
			nation_array[nation_recno]->set_relation_should_attack( townPtr->nation_recno, 1, COMMAND_AI );

		int unitRecno = patrol_unit_array[ m.random(patrol_unit_count) ];

		unit_array[unitRecno]->attack(townPtr->base_obj_recno, true);
	}
}
//----------- End of function FirmCamp::process_ai_capturing -----------//


//------ Begin of function FirmCamp::ai_attack_town_defender ------//
//
void FirmCamp::ai_attack_town_defender(Unit* attackerUnit)
{
	int shouldAttackUnit=0;

	if( attackerUnit->cur_action == SPRITE_IDLE )
		shouldAttackUnit = 1;

	else if( attackerUnit->cur_order.mode == UNIT_ATTACK )
	{
		//--- if this unit is currently attacking the town, ask it to attack a defender unit ---//

		Town* townPtr = town_array[ai_capture_town_recno];

		if( attackerUnit->cur_order.loc_x==townPtr->loc_x1 && 
			 attackerUnit->cur_order.loc_y==townPtr->loc_y1 )
		{
			shouldAttackUnit = 1;
		}
	}

	if( !shouldAttackUnit )
		return;

	//---- if there are still town defenders out there ---//

	int unitCount = unit_array.size();
	int unitRecno = m.random(unitCount)+1;		// scan randomly
	Unit* targetUnit;

	for( int i=0 ; i<unitCount ; i++ )
	{
		if( ++unitRecno > unitCount )
			unitRecno = 1;

		if( unit_array.is_deleted(unitRecno) )
			continue;

		targetUnit = unit_array[unitRecno];

		if( targetUnit->unit_mode == UNIT_MODE_TOWN_DEFENDER &&
			 targetUnit->unit_mode_para == ai_capture_town_recno )
		{
			if( targetUnit->nation_recno )
				nation_array[nation_recno]->set_relation_should_attack( targetUnit->nation_recno, 1, COMMAND_AI );

			attackerUnit->attack( targetUnit->base_obj_recno, true );
			break;
		}
	}
}
//-------- End of function FirmCamp::ai_attack_town_defender ------//


//--------- Begin of function FirmCamp::think_recruit ---------//
//
// Think about recruiting an overseer and soliders to this base.
//
void FirmCamp::think_recruit()
{
	if( patrol_unit_count )		// if there are units of this camp patrolling outside
		return;

	Nation* nationPtr = nation_array[nation_recno];

	ai_recruiting_soldier = 1; 		// the AI is currently trying to recruit soldiers

	//---- if there are currently units coming to this firm ----//

	if( coming_unit_count > 0 )
	{
		Unit* unitPtr;

		for( int i=0 ; i<coming_unit_count ; i++ )
		{
			if( unit_array.is_deleted( coming_unit_array[i] ) )
				continue;

			unitPtr = unit_array[ coming_unit_array[i] ];

			//--- check if any of them are still on their way to this firm ---//

			if( unitPtr->nation_recno == nation_recno &&
				 unitPtr->cur_order.mode == UNIT_ASSIGN )
			{
				//--- if so, do not do anything unit they are all done ---//

				return;
			}
		}

		coming_unit_count = 0;
	}

	//-- if this camp is empty, think about move a whole troop from a useless camp (should_ai_close()==1)

	if( !overseer_recno && soldier_count==0 &&
		 ( nationPtr->firm_should_close_array[FIRM_CAMP-1] > 0 ||
			nationPtr->firm_should_close_array[FIRM_FORT-1] > 0 ) )
	{
		FirmCamp *firmCamp, *bestCampPtr=NULL;
		int		bestRating=0, curRating;

		//--- see if there are any useless camps around and pick the most suitable one ---//

		for( int i=nationPtr->ai_camp_count-1 ; i>=0 ; i-- )
		{
			firmCamp = (FirmCamp*) firm_array[ nationPtr->ai_camp_array[i] ];

			err_when( !firmCamp->cast_to_FirmCamp() );

			if( firmCamp->region_id != region_id )
				continue;

			if( firmCamp->should_close_flag && 
				 (firmCamp->overseer_recno || firmCamp->soldier_count>0) )
			{
				curRating = 100 - m.points_distance( center_x, center_y,
								firmCamp->center_x, firmCamp->center_y );

				if( curRating > bestRating )
				{
					bestRating = curRating;
					bestCampPtr = firmCamp;
				}
			}
		}

		//--------- start the move now -------//

		if( bestCampPtr )
		{
			bestCampPtr->patrol();

			if( bestCampPtr->patrol_unit_count==0 )		// there could be chances that there are no some for mobilizing the units
				return;

			//--- set coming_unit_count for later checking ---//

			err_when( sizeof(coming_unit_array) != sizeof(patrol_unit_array) );

			memcpy( coming_unit_array, bestCampPtr->patrol_unit_array, sizeof(coming_unit_array) );
			coming_unit_count = bestCampPtr->patrol_unit_count;

			//---- order the unit to move to this camp now ---//

			unit_group.set(bestCampPtr->patrol_unit_array, bestCampPtr->patrol_unit_count);
			unit_group.assign(loc_x1, loc_y1, UNIT_LAND, COMMAND_AI);
			
			//------ delete the camp as it no longer has any use ----//
			
			bestCampPtr->ai_del_firm();
			return;
		}
	}

	//------- get an overseer if there isn't any right now -----//

	if( !overseer_recno )
		nationPtr->add_action(loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_OVERSEER, FIRM_FORT);

	//---- calculate the number of soldiers needed to be recruited ----//

	int combatLevelNeeded = ai_combat_level_needed();
	int recruitCount = (combatLevelNeeded - total_combat_level()) / 50;		// assuming each soldier has 50 points of protection

	int extraRecruit = 0;

	if( nationPtr->ai_has_enough_food() )
	{
		extraRecruit += nationPtr->total_jobless_population -							 // 20 to 53
							 (20 + (100-nationPtr->pref_military_development) / 3);

		//-- put more soldiers under training of high leadership commanders --//

		if( overseer_recno )
			extraRecruit += unit_array[overseer_recno]->skill_level() / 10;
	}

	if( overseer_recno == nationPtr->king_unit_recno )		// recruit more soldiers if the commander is the king
	{
		extraRecruit += 10;

		if( extraRecruit < 0 )
			extraRecruit = 0;
	}

	if( extraRecruit < -MAX_SOLDIER )		// set a minimum for extraRecruit, if it is too low, the recruitCount will be affected by it too much. We want to the combat level difference have an affect here
		extraRecruit = -MAX_SOLDIER;

	if( extraRecruit > 0 )
		recruitCount += extraRecruit;
	else
		recruitCount += extraRecruit/2;		// the impact is small if extraDiff is negative

	//-------- recruit now ---------//

	if( recruitCount > 0 )
	{
		ai_recruit(recruitCount);
	}
	else
	{
		if( overseer_recno )
			ai_recruiting_soldier = 0;		// this firm has enough soldiers already
	}
}
//----------- End of function FirmCamp::think_recruit ----------//


//--------- Begin of function FirmCamp::ai_recruit ---------//
//
// <int> recruitCount - the number of soldiers to be recruited.
//
// return: <int> 1-succeeded, 0-failed.
//
int FirmCamp::ai_recruit(int recruitCount)
{
	if( soldier_count + patrol_unit_count >= MAX_SOLDIER || recruitCount==0 )
		return 0;

	recruitCount = min( recruitCount, MAX_SOLDIER-soldier_count-patrol_unit_count );

	err_when( recruitCount < 0 );

	//------ try to recruit from other camps -------//

	if( recruitCount > 0 )
		nation_array[nation_recno]->add_action(loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_SOLDIER, firm_id, recruitCount);

	return 1;
}
//----------- End of function FirmCamp::ai_recruit ----------//


//--------- Begin of function FirmCamp::ai_combat_level_needed ---------//
//
// Think about the no. of soldiers needed by this base.
//
int FirmCamp::ai_combat_level_needed()
{
	Town*	  townPtr;
	int  	  combatNeeded=0;
	Nation* nationPtr = nation_array[nation_recno];

	//------- scan for linked towns ---------//

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[ linked_town_array[i] ];

		//------- this is its own town -------//

		if( townPtr->nation_recno == nation_recno )
		{
			if( townPtr->should_ai_migrate() ) 	// no need for this if this town is going to migrate
				continue;

			combatNeeded += townPtr->population * 10;	// 30 people need 300 combat levels

			if( townPtr->is_base_town )
				combatNeeded += townPtr->population * 10;		// double the combat level need for base town
		}
	}

	//--- if the overseer is the king, increase its combat level needed ---//

	if( overseer_recno && unit_array[overseer_recno]->rank_id == RANK_KING )
		combatNeeded = max(400, combatNeeded);

	//---------------------------------------//

	return combatNeeded;
}
//----------- End of function FirmCamp::ai_combat_level_needed ----------//


//--------- Begin of function FirmCamp::total_combat_level ---------//
//
// Total combat level of all soldiers and commander in the base.
// The leadership of the general also applies to the final combat level.
//
// return: <int> the total combat level - it is the sum of hit points
//					  of all the units in the camp.
//
int FirmCamp::total_combat_level()
{
	int 	  totalCombatLevel=0;
	Soldier* soldierPtr = soldier_array;

	for( int i=0 ; i<soldier_count ; i++, soldierPtr++ )
	{
		if( soldierPtr->is_under_training() )
			continue;

		totalCombatLevel += soldierPtr->hit_points;			// use it points instead of combat level because hit_points represent both combat level and hit points left

		err_when( totalCombatLevel < 0 );

		//---- the combat level of weapons are 100 ------//

		UnitInfo* unitInfo = unit_res[soldierPtr->unit_id];

		// ####### begin Gilbert 24/3 #######//
		if( !unitInfo->class_info.has_combat_level )
		{
			int weaponPower = unitInfo->weapon_power;
			if( unitInfo->class_info.has_weapon_version )
				weaponPower += soldierPtr->extra_para - 1;
			totalCombatLevel += weaponPower * 30;		// extra_para keeps the weapon version
		}
		// ####### end Gilbert 24/3 #######//

		err_when( totalCombatLevel < 0 );
	}

	if( overseer_recno )
	{
		Unit* unitPtr = unit_array[overseer_recno];

		//--- the commander's leadership effects over the soldiers ---//

		totalCombatLevel += totalCombatLevel * (int) unitPtr->skill_level() / 150;		// divided by 150 instead of 100 because while the attacking ability of the unit is affected by the general, the hit points isn't, so we shouldn't do a direct multiplication.

		//------ the leader's own hit points ------//

		totalCombatLevel += (int) unitPtr->hit_points;

		err_when( totalCombatLevel < 0 );
	}

	return totalCombatLevel;
}
//----------- End of function FirmCamp::total_combat_level ----------//


//--------- Begin of function FirmCamp::average_combat_level ---------//
//
int FirmCamp::average_combat_level()
{
	int unitCount = soldier_count + (overseer_recno>0);

	if( unitCount > 0 )
		return total_combat_level() / unitCount;
	else
		return 0;
}
//----------- End of function FirmCamp::average_combat_level ----------//


//--------- Begin of function FirmCamp::ai_should_close ---------//
//
// Whether this AI firm should be closed or not.
//
int FirmCamp::ai_should_close()
{
	Nation* nationPtr = nation_array[nation_recno];

	return linked_town_count==0 && linked_firm_count==0 &&
			 nationPtr->total_camp_unit_space(0) >= nationPtr->total_unit_count + MAX_SOLDIER;
}
//----------- End of function FirmCamp::ai_should_close ----------//


//--------- Begin of function FirmCamp::think_capture ---------//
//
// Think about capturing towns.
//
void FirmCamp::think_capture()
{
	if( is_attack_camp )		// if this camp has been assigned to an attack mission already
		return;

	int targetTownRecno = think_capture_target_town();

	if( !targetTownRecno )
		return;

	//----- if the target town is a nation town -----//

	Town* targetTown = town_array[targetTownRecno];
	Nation* ownNation = nation_array[nation_recno];

	if( targetTown->nation_recno )
	{
		//--- if there are any defenses (camps and mobile units) on the target town, destroy them all first -----//

		if( ownNation->attack_enemy_town_defense(targetTown) != -1 )		// only proceed further when the result is -1, which means no defense on the target town, no attacking is needed.
			return;
	}

	//------ check if the town people will go out to defend -----//

	float resistanceDiff;
	int   defenseCombatLevel=0;

	if( nation_recno )
		resistanceDiff = targetTown->loyalty - MIN_NATION_DEFEND_LOYALTY;
	else
		resistanceDiff = targetTown->resistance(nation_recno) - MIN_INDEPENDENT_DEFEND_LOYALTY;

	if( targetTown->population >= 5 )		// if the pop count is lower than 5, ingore it
	{
		defenseCombatLevel += targetTown->possible_defender_combat_level(nation_recno);
	}

	//--- try using spies if there are defense forces and the nation likes to use spies ---//

	if( defenseCombatLevel > 0 ) 		// && ownNation->pref_spy >= 50 && m.random(3)==0 )		// 1/3 chance of using spies here, otherwise only use spies when we are not strong enough to take over the village by force
	{
		if( targetTown->nation_recno==0 )		// if the camp is trying to capture an independent town, the leadership and race id. of the overseer matters.
		{
			if( think_assign_better_overseer(targetTown) )		// a better general is being assigned to this firm, wait for it
				return;

			if( think_capture_use_spy(targetTown) )
				return;

			if( defenseCombatLevel > 100+ownNation->pref_military_courage &&
				 resistanceDiff > (100-ownNation->pref_peacefulness)/5  )		// depending on the peacefulness, the nation won't attack if resistance > (0-20)
			{
				return;
			}
		}
		else
		{
			//--- don't attack if the target nation's military rating is higher than ours ---//

			if( nation_array[targetTown->nation_recno]->military_rank_rating()
				 > ownNation->military_rank_rating() )
			{
				return;
			}
		}
	}

	//------ send out troops to capture the target town now ----//

	int rc;

	if( targetTown->nation_recno )
		rc = ai_capture_enemy_town(targetTown, defenseCombatLevel);
	else
		rc = ai_capture_independent_town(targetTown, defenseCombatLevel);

	//-- use the same approach to capture both enemy and independent towns --//

	if( rc )
	{
		ai_capture_town_recno = targetTownRecno;
		defense_flag 			 = 0;			// turn off the defense flag during capturing so the general is staying in the base to influence the town

		//--- as the current commander has been out to attack the town by ai_attack_target(), we need to assign him back to the camp for influencing the town and eventually capture it ---//

		if( !overseer_recno && targetTown->nation_recno && patrol_unit_count>0 )
			unit_array[ patrol_unit_array[0] ]->assign(loc_x1, loc_y1);
	}
}
//----------- End of function FirmCamp::think_capture -----------//


//--------- Begin of function FirmCamp::think_capture_target_town ---------//
//
// Think about which town to capture.
//
// Return: <int> recno of the target town.
//
int FirmCamp::think_capture_target_town()
{
	if( !linked_town_count || !overseer_recno )
		return 0;

	//--- if there are any units currently being assigned to this camp ---//

	if( nation_array[nation_recno]->is_action_exist( loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_SOLDIER, firm_id ) )
		return 0;

	//-- decide which town to attack (only when the camp is linked to more than one town ---//

	int	curResistance, curTargetResistance, resistanceDec;
	int 	i, minResistance=100, bestTownRecno=0;
	Town* townPtr;
	int   prefPeacefulness = nation_array[nation_recno]->pref_peacefulness;
	Nation* ownNation = nation_array[nation_recno];
	int   overseerRaceId = unit_array[overseer_recno]->race_id;

	for( i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno == nation_recno )
			continue;

		//------- if it's an independent town -------//

		if( !townPtr->nation_recno )		// only capture independent town
		{
      	//----- if this is a slave town, don't bother -----//

			if( townPtr->is_pay_tribute_to_monster() )
				continue;

			//-------------------------------------------------//

			curResistance = (int) townPtr->resistance(nation_recno);
			curTargetResistance = townPtr->target_resistance(nation_recno);

			resistanceDec = curResistance - curTargetResistance;

			//------- if the resistance is decreasing ---------//

			if( resistanceDec > 0 &&
				 curResistance > 25-25*ownNation->pref_peacefulness/100 &&		// for nation that has a high peacefulness preference they will wait for the loyalty to fall and try not to attack the town unless necessary
				 townPtr->population >= 5 )		// if it's less than 5, don't count it, as that it will be easy to attack
			{
				continue;		// let it decrease until it can no longer decrease
			}
		}
		else	//-------- if it's a nation town ---------//
		{
			NationRelation* nationRelation = ownNation->get_relation(townPtr->nation_recno);

			if( nationRelation->status != RELATION_HOSTILE )
				continue;

			curResistance = (int) townPtr->loyalty;
		}

		//--------------------------------------//

		if( curResistance < minResistance )
		{
			minResistance = curResistance;
			bestTownRecno = townPtr->town_recno;
		}
	}

	return bestTownRecno;
}
//--------- End of function FirmCamp::think_capture_target_town --------//


//--------- Begin of function FirmCamp::think_attack_enslaving_monster ---------//
//
// Think about attacking Fryhtan lairs enslaving independent towns that
// we intend to capture.
//
int FirmCamp::think_attack_enslaving_monster()
{
	if( !linked_town_count || !overseer_recno )
		return 0;

	//--- if there are any units currently being assigned to this camp ---//

	if( nation_array[nation_recno]->is_action_exist( loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_SOLDIER, firm_id ) )
		return 0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno )
			continue;

		if( !townPtr->is_pay_tribute_to_monster() )
			continue;

		//--- if this is an independent town enslaving by Fryhtans ---//

		for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* firmPtr = firm_array[ townPtr->linked_firm_array[j] ];

			// ####### begin Gilbert 3/2 ########//
			if( firmPtr->firm_id == FIRM_LAIR
				&& firmPtr->nation_recno			// avoid townPtr->resistance() error
				&& townPtr->resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
			// ####### end Gilbert 3/2 ########//
			{
				//--- if this is an enslaving town, we should attack it ---//

				if( nation_array[nation_recno]->ai_attack_target(firmPtr->loc_x1, firmPtr->loc_y1,
					 firmPtr->obj_power(), 0, 0, 0, firm_recno, 1) )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}
//--------- End of function FirmCamp::think_attack_enslaving_monster --------//


//--------- Begin of function FirmCamp::ai_capture_independent_town -------//
//
// Try to capture the given independent town.
//
int FirmCamp::ai_capture_independent_town(Town* targetTown, int defenseCombatLevel)
{
	//---- see if the force is strong enough to attack the town ----//

	Nation* nationPtr = nation_array[nation_recno];

	int curCombatLevel = total_combat_level();		// total combat level

	int combatDiff = defenseCombatLevel * (150+nationPtr->pref_force_projection/2) / 100
						  - curCombatLevel;					// try to recruit soldiers based on the force projection perference

	if( combatDiff > 0 )
	{
		if( ai_recruit(combatDiff) )    	// try to recruit new soldiers to increase the combat ability of the troop
			return 0;
	}

	combatDiff = defenseCombatLevel * (200-nationPtr->pref_military_courage/2) / 100
					 - curCombatLevel;

	//---------- attack the target town now ----------//

	if( nation_array[nation_recno]->ai_attack_target(targetTown->loc_x1, targetTown->loc_y1, defenseCombatLevel, 0, 0, 0, firm_recno) )
		return 1;

	return 0;
}
//--------- End of function FirmCamp::ai_capture_independent_town --------//


//--------- Begin of function FirmCamp::think_capture_return ---------//
//
// Think about if the capturing units should now return to their base.
//
int FirmCamp::think_capture_return()
{
	//----- if the target town is destroyed ------//

	int returnCamp=0;

	if( town_array.is_deleted(ai_capture_town_recno) )
	{
		returnCamp = 1;
	}
	else //---- check whether the town has been captured ----//
	{
		Town* townPtr = town_array[ai_capture_town_recno];

		if( townPtr->nation_recno == nation_recno )		// the town has been captured
			returnCamp = 1;
	}

	//-------- if should return to the camp now ---------//

	if( returnCamp )
	{
		Unit* unitPtr;

		for( int i=0; i<patrol_unit_count ; i++ )
		{
			unitPtr = unit_array[ patrol_unit_array[i] ];

			if( unitPtr->is_visible() )
				unitPtr->assign(loc_x1, loc_y1);
		}

		ai_capture_town_recno = 0;		// turn it on again after the capturing plan is finished
		defense_flag = 1;
		return 1;
	}

	return 0;
}
//----------- End of function FirmCamp::think_capture_return -----------//


//--------- Begin of function FirmCamp::think_assign_better_overseer -------//
//
int FirmCamp::think_assign_better_overseer(Town* targetTown)
{
	//----- check if there is already a queued action -----//

	Nation* ownNation = nation_array[nation_recno];

	if( ownNation->is_action_exist( loc_x1, loc_y1, -1, -1,
		 ACTION_AI_ASSIGN_OVERSEER, firm_id ) )
	{
		return 1;		// there is a queued action being processed already
	}

	//-- if the resistance of the majority race has already dropped to its lowest possible --//

	if( targetTown->resistance(nation_recno) <=
		 (float) (targetTown->target_resistance(nation_recno)+1) )
	{
		if( targetTown->resistance(nation_recno) > 30 )
		{
			if( think_assign_better_overseer2(targetTown->town_recno, targetTown->race_id) )
				return 1;
		}
	}

	//-- if the resistance of the 2nd majority race has already dropped to its lowest possible --//

	if( targetTown->resistance(nation_recno) <=
		 (float) (targetTown->target_resistance(nation_recno)+1) )
	{
		if( targetTown->resistance(nation_recno) > 30 )
		{
			if( think_assign_better_overseer2(targetTown->town_recno, targetTown->race_id) )
				return 1;
		}
	}

	return 0;
}
//--------- End of function FirmCamp::think_assign_better_overseer --------//


//--------- Begin of function FirmCamp::think_assign_better_overseer2 -------//
//
int FirmCamp::think_assign_better_overseer2(int targetTownRecno, int raceId)
{
	int reduceResistance;

	Nation* ownNation = nation_array[nation_recno];

	int bestUnitRecno = ownNation->find_best_capturer(targetTownRecno, reduceResistance);

	if( !bestUnitRecno || bestUnitRecno==overseer_recno )		// if we already got the best one here
		return 0;

	err_when( unit_res[ unit_array[bestUnitRecno]->unit_id ]->is_civilian );

	//---- only assign new overseer if the new one's leadership is significantly higher than the current one ----//

	if( overseer_recno &&
		 unit_array[bestUnitRecno]->skill_level() < unit_array[overseer_recno]->skill_level() + 15 )
	{
		return 0;
	}

	//------ check what the best unit is -------//

	if( !ownNation->mobilize_capturer(bestUnitRecno) )
		return 0;

	//---------- add the action to the queue now ----------//

	int actionRecno = ownNation->add_action( loc_x1, loc_y1, -1, -1,
							ACTION_AI_ASSIGN_OVERSEER, firm_id, 1, bestUnitRecno );

	if( actionRecno )
		ownNation->process_action(actionRecno);

	return 1;
}
//--------- End of function FirmCamp::think_assign_better_overseer2 --------//


//--------- Begin of function FirmCamp::ai_capture_enemy_town -------//
//
// When capturing an enemy town, the commander should stay in the
// command base to influence the village and reinforcement should be
// sent instead of using the troop in the base for attacking the enemies.
//
int FirmCamp::ai_capture_enemy_town(Town* targetTown, int defenseCombatLevel)
{
	int useAllCamp;

	Nation* ownNation = nation_array[nation_recno];
	Nation* targetNation = nation_array[targetTown->nation_recno];

	int ourMilitary   = ownNation->military_rank_rating();
	int enemyMilitary = targetNation->military_rank_rating();

	//--- use all camps to attack if we have money and we are stronger than the enemy ---//

	if( ourMilitary - enemyMilitary > 30 && ownNation->ai_should_spend(ownNation->pref_military_courage/2) )
		useAllCamp = 1;

	//---- use all camps to attack the enemy if the enemy is a human player

	else if( config.ai_aggressiveness >= OPTION_MODERATE &&
				!targetNation->is_ai() && ourMilitary > enemyMilitary )
	{
		if( config.ai_aggressiveness >= OPTION_HIGH ||
			 ownNation->pref_peacefulness < 50 )
		{
			useAllCamp = 1;
		}
	}

	return nation_array[nation_recno]->ai_attack_target(targetTown->loc_x1, targetTown->loc_y1,
			 defenseCombatLevel, 0, 0, 0, firm_recno, useAllCamp );
}
//--------- End of function FirmCamp::ai_capture_enemy_town --------//


//--------- Begin of function FirmCamp::think_capture_use_spy ---------//
//
// Think about using spies for capturing the target town.
//
int FirmCamp::think_capture_use_spy(Town* targetTown)
{
	Nation* ownNation = nation_array[nation_recno];

	//-- get the current number of our spies in this town and see if we need more --//

	int spyCount;
	int curSpyLevel = spy_array.total_spy_skill_level( SPY_TOWN, targetTown->town_recno, nation_recno, spyCount );

	return think_capture_use_spy2(targetTown, targetTown->race_id, curSpyLevel );
}
//----------- End of function FirmCamp::think_capture_use_spy -----------//


//--------- Begin of function FirmCamp::think_capture_use_spy2 ---------//
//
// Think about using spies for capturing the target town.
//
int FirmCamp::think_capture_use_spy2(Town* targetTown, int raceId, int curSpyLevel)
{
	Nation* ownNation = nation_array[nation_recno];

	int curResistance, targetResistance;

	if( targetTown->nation_recno )
	{
		curResistance 	  = (int) targetTown->loyalty;
		targetResistance = targetTown->target_loyalty;
	}
	else
	{
		curResistance    = (int) targetTown->resistance(nation_recno);
		targetResistance = targetTown->target_resistance(nation_recno);
	}

	int minResistance = min(curResistance, targetResistance);

	//----- if the resistance is low enough, don't have to use spies -----//

	if( targetTown->nation_recno )
	{
		if( minResistance < MIN_NATION_DEFEND_LOYALTY )
			return 0;
	}
	else
	{
		if( minResistance < MIN_INDEPENDENT_DEFEND_LOYALTY )
			return 0;
	}

	//----- if the needed spy level > current spy level, assign more spies ----//

	int neededSpyLevel = minResistance * (50+ownNation->pref_spy) / 50;

	if( neededSpyLevel > curSpyLevel )
		return ownNation->ai_assign_spy_to_town(targetTown->town_recno, raceId);

	return 0;
}
//----------- End of function FirmCamp::think_capture_use_spy2 -----------//


//--------- Begin of function FirmCamp::ai_update_link_status ---------//
//
// Updating link status of this firm with towns.
//
// It's a overloaded function of Firm::ai_update_link_status().
//
void FirmCamp::ai_update_link_status()
{
	Nation* ownNation = nation_array[nation_recno];

	//------ always enable links to all towns -----//

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[linked_town_array[i]];

		//---- don't try to capture other nation's towns unless the AI is at war or tense with the nation ----//

		if( townPtr->nation_recno &&
			 ownNation->get_relation_status(townPtr->nation_recno) <= RELATION_TENSE )		// hostile or tense 
		{	
			continue;
		}

		toggle_town_link( i+1, 1, COMMAND_AI );

		//------------------------------------------------------------------//
		// Here we only update this camp's link to the town. 
		// The town's link to this firm is updated in Town::update_target_loyalty().
		//------------------------------------------------------------------//
	}
}
//----------- End of function FirmCamp::ai_update_link_status ----------//


//------- Begin of function FirmCamp::ai_has_excess_soldier -------//
//
// Return whether this firm has any excessive soldiers or not.
//
int FirmCamp::ai_has_excess_soldier()
{
	if( linked_town_count==0 )
		return 1;

	if( ai_capture_town_recno )		// no if the camp is trying to capture an independent town
		return 0;

	if( is_attack_camp )		// no if the camp is trying to capture an independent town
		return 0;

	if( should_close_flag )
		return 1;

	return 0;

//	return total_combat_level() > ai_combat_level_needed()+100;
}
//--------- End of function FirmCamp::ai_has_excess_soldier -------//


//------- Begin of function FirmCamp::think_employ_to_capture -------//
//
// Think about building a firm and employ independent citizens to
// lower their resistance.
//
int FirmCamp::think_employ_to_capture()
{
	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->pref_peacefulness > 50 )
	{
		if( nationPtr->cash < 1000 * nationPtr->pref_cash_reserve / 100 )
			return 0;
	}
	else
	{
		//----- see if it's worth spending to build a tower of science ----//

		int importanceRating = 50 + nationPtr->pref_research/2 - nationPtr->ai_research_count*10;

		if( nationPtr->ai_research_count==0 )		// if we haven't had any towers of science yet
			importanceRating += 50;

		if( !nationPtr->ai_should_spend(importanceRating) )
			return 0;
	}

	//-----------------------------------//

	Town* townPtr;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno )		// only independent towns
			continue;

		//---- check if its resistance is low enough ----//
		
		if( townPtr->resistance(nation_recno) > INDEPENDENT_LINK_RESISTANCE )
			continue;

		//----- if we already have towers of science linked to this town ----//

		int j;
		for( j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* firmPtr = firm_array[ townPtr->linked_firm_array[j] ];

			if( firmPtr->firm_id==FIRM_RESEARCH && firmPtr->nation_recno==nation_recno )
				break;
		}	

		if( j<townPtr->linked_firm_count )
			continue;

		//--- check if a unit is already on its way to build firm for capturing this town ---//

		if( nationPtr->is_action_exist( -1, -1, townPtr->loc_x1, townPtr->loc_y1,
			 ACTION_AI_BUILD_FIRM, FIRM_RESEARCH ) )						// 2-only check the reference location, ignore the action location
		{
			continue;
		}

		//----------- build now -----------//

		short buildXLoc, buildYLoc;

		if( nationPtr->find_best_place_loc(FIRM_RESEARCH, townPtr->loc_x1, townPtr->loc_y1, buildXLoc, buildYLoc) )
		{
			nationPtr->add_action(buildXLoc, buildYLoc, townPtr->loc_x1, townPtr->loc_y1,
				ACTION_AI_BUILD_FIRM, FIRM_RESEARCH, 1, 0, nationPtr->race_id);

			return 1;
		}
	}

	return 0;
}
//--------- End of function FirmCamp::think_employ_to_capture -------//


//------- Begin of function FirmCamp::think_use_cash_to_capture -------//
//
// Think about using money to decrease the resistance of the
// independent villagers.
//
int FirmCamp::think_use_cash_to_capture()
{
	if( !nation_array[nation_recno]->should_use_cash_to_capture() )
		return 0;

	//-------------------------------------//

	Town* townPtr;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno == nation_recno )
			continue;

		if( townPtr->accumulated_enemy_grant_penalty > 0 )
			continue;

		//--- if this is a slave town, don't bother ---//

		if( townPtr->is_pay_tribute_to_monster() )
			continue;

		if( townPtr->can_grant_to_non_own_town(nation_recno) )
			townPtr->grant_to_non_own_town(nation_recno, COMMAND_AI);
	}

	return 1;
}
//--------- End of function FirmCamp::think_use_cash_to_capture -------//


//------- Begin of function FirmCamp::think_linked_town_change_nation ------//
//
// This function is called by Town::set_nation() when a town linked
// to this firm has changed nation.
//
// <int> linkedTownRecno - the recno of the town that has changed nation.
// <int> oldNationRecno  - the old nation recno of the town
// <int> newNationRecno  - the new nation recno of the town
//
void FirmCamp::think_linked_town_change_nation(int linkedTownRecno, int oldNationRecno, int newNationRecno)
{
	//-----------------------------------------------//
	//
	// If we are trying to capture an independent town and our
	// enemies have managed to capture it first.
	//
	//-----------------------------------------------//

	Nation* ownNation = nation_array[nation_recno];

	if( oldNationRecno==0 && newNationRecno>0 && newNationRecno != nation_recno  )
	{
		Town* townPtr = town_array[linkedTownRecno];

		//--- if the town does not have any protection, then don't remove this camp ---//

		if( townPtr->protection_available()==0 )
			return;

		should_close_flag = 1;
		ownNation->firm_should_close_array[firm_id-1]++;
	}
}
//-------- End of function FirmCamp::think_linked_town_change_nation ------//


//------- Begin of function FirmCamp::think_attack_invader -------//
//
// Think about attacking invaders to this area.
//
int FirmCamp::think_attack_invader()
{
	//------------------------------------------//

	Nation* 	nationPtr = nation_array[nation_recno];

	int scanRange = 6 + nationPtr->pref_military_courage/20;		// 6 to 11

	int xLoc1 = loc_x1 - scanRange;
	int yLoc1 = loc_y1 - scanRange;
	int xLoc2 = loc_x2 + scanRange;
	int yLoc2 = loc_y2 + scanRange;

	xLoc1 = max( xLoc1, 0 );
	yLoc1 = max( yLoc1, 0 );
	xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
	yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );

	//------------------------------------------//

	int		enemyCombatLevel=0;		// the higher the rating, the easier we can attack the target town.
	int 		xLoc, yLoc;
	int		enemyUnitRecno=0, enemyXLoc= -1, enemyYLoc;
	Unit* 	unitPtr;
	Location* locPtr;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		locPtr = world.get_loc(xLoc1, yLoc);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			//--- if there is a unit here ---//

			if( locPtr->unit_recno(UNIT_LAND) )
			{
				unitPtr = unit_array[ locPtr->unit_recno(UNIT_LAND) ];

				if( !unitPtr->nation_recno )
					continue;

				//--- if the unit is attacking and it is our enemy ----//

				if( unitPtr->cur_action == SPRITE_ATTACK &&
					 nationPtr->get_relation_status(unitPtr->nation_recno) == RELATION_HOSTILE )
				{
					err_when( unitPtr->nation_recno == nation_recno );

					enemyCombatLevel += (int) unitPtr->hit_points;

					if( enemyXLoc == -1 || m.random(5)==0 )
					{
						enemyXLoc = xLoc;
						enemyYLoc = yLoc;
						enemyUnitRecno = unitPtr->sprite_recno;
					}
				}
			}
		}
	}

	if( enemyCombatLevel==0 )
		return 0;

	err_when( enemyXLoc < 0 );

	//--------- attack the target now -----------//

	if( soldier_count > 0 )
	{
		patrol_all_soldier();

		if( patrol_unit_count > 0 )
		{
			unit_group.set(patrol_unit_array, patrol_unit_count);
			unit_group.attack( unit_array[enemyUnitRecno]->base_obj_recno, COMMAND_AI );
			return 1;
		}
	}

	return 0;
}
//-------- End of function FirmCamp::think_attack_invader -------//


//------- Begin of function FirmCamp::think_attack_nearby_enemy -------//
//
// Think about attacking enemy structures/towns near this camp.
//
int FirmCamp::think_attack_nearby_enemy()
{
	//------------------------------------------//

	Nation* 	nationPtr = nation_array[nation_recno];

	int scanRange = 6 + nationPtr->pref_military_courage/20;		// 6 to 11

	int xLoc1 = loc_x1 - scanRange;
	int yLoc1 = loc_y1 - scanRange;
	int xLoc2 = loc_x2 + scanRange;
	int yLoc2 = loc_y2 + scanRange;

	xLoc1 = max( xLoc1, 0 );
	yLoc1 = max( yLoc1, 0 );
	xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
	yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );

	//------------------------------------------//

	int 		 xLoc, yLoc;
	Location* locPtr;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		locPtr = world.get_loc(xLoc1, yLoc);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			//--- if there is a unit here ---//

			int baseObjRecno = locPtr->base_obj_recno(UNIT_LAND);

			if( !baseObjRecno )
				continue;

			BaseObj* baseObj = base_obj_array[baseObjRecno];

			if( !baseObj->cast_to_Place() )
				continue;

			if( baseObj->nation_recno &&
				 nationPtr->get_relation_status(baseObj->nation_recno) == RELATION_HOSTILE )
			{
				if( nationPtr->ai_attack_target(baseObj->obj_loc_x1(),
					 baseObj->obj_loc_y1(), baseObj->obj_power(), 0, 0, 0, firm_recno) )
				{
					return 1;
				}
			}

		}
	}

	return 0;
}
//-------- End of function FirmCamp::think_attack_nearby_enemy -------//


//------- Begin of function FirmCamp::think_change_town_link -------//
//
// Think about changing links to foreign towns.
//
void FirmCamp::think_change_town_link()
{
	Nation* ownNation = nation_array[nation_recno];

	for( int i=linked_town_count-1 ; i>=0 ; i-- )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		//--- only change links to foreign towns, links to own towns are always on ---//

		if( townPtr->nation_recno == nation_recno )
			continue;

		//---- only enable links to non-friendly towns ----//

		int enableFlag = townPtr->nation_recno==0 || 
							  ownNation->get_relation(townPtr->nation_recno)->status == RELATION_HOSTILE;

		toggle_town_link( i+1, enableFlag, COMMAND_AI );
	}
}
//--------- End of function FirmCamp::think_change_town_link -------//


//------- Begin of function FirmCamp::think_assign_better_commander -------//
//
// Assign a better commander to this camp.
//
int FirmCamp::think_assign_better_commander()
{
	//----- if there is already an overseer being assigned to the camp ---//

	Nation* ownNation = nation_array[nation_recno];

	int actionRecno = ownNation->is_action_exist(loc_x1, loc_y1, -1, -1,
							 ACTION_AI_ASSIGN_OVERSEER, firm_id);

	//--- if there is already one existing ---//

	if( actionRecno )
	{
		//--- if the action still is already being processed, don't bother with it ---//

		if( ownNation->get_action(actionRecno)->processing_instance_count > 0 )
			return 0;

		//--- however, if the action hasn't been processed, we still try to use the approach here ---//
	}

	//-------------------------------------------------//

	FirmCamp*   firmCamp;
	Soldier* soldierPtr;
	int     bestRaceId = best_commander_race();
	int	  bestFirmRecno=0, bestSoldierId;
	int	  bestLeadership=0, curLeadership=0;

	if( overseer_recno )
	{
		curLeadership  = cur_commander_leadership(bestRaceId);

		if( is_human() )
			bestLeadership	= curLeadership + 10 + ownNation->pref_loyalty_concern/10;			// nations that have higher loyalty concern will not switch commander too frequently
		else
			bestLeadership	= curLeadership + 5 + ownNation->pref_loyalty_concern/20;			// monsters have less concern on this
	}

	//--- locate for a soldier who has a higher leadership ---//

	int i;
	for( i=ownNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		firmCamp = firm_array[ ownNation->ai_camp_array[i] ]->cast_to_FirmCamp();

		if( !firmCamp || firmCamp->region_id != region_id )
			continue;

		soldierPtr = firmCamp->soldier_array;

		for( int j=1 ; j<=firmCamp->soldier_count ; j++, soldierPtr++ )
		{
			if( !soldierPtr->race_id || soldierPtr->is_under_training() )
				continue;

			int soldierLeadership = soldierPtr->skill_level();

			if( soldierPtr->race_id != bestRaceId )
				soldierLeadership /= 2;

			if( soldierLeadership > bestLeadership )
			{
				bestLeadership = soldierLeadership;
				bestFirmRecno  = firmCamp->firm_recno;
				bestSoldierId   = j;
			}
		}
	}

	int unitRecno=0;

	//---- if we have successfully located a soldier with better leadership ---//

	if( bestFirmRecno )
		unitRecno = firm_array[bestFirmRecno]->cast_to_FirmCamp()->mobilize_soldier(bestSoldierId, COMMAND_AI);

	//---- Otherwise, try to hire units with high leadership from inns ----//

	if( !unitRecno )
	{
		for( i=ownNation->ai_inn_count-1 ; i>=0 && !unitRecno ; i-- )
		{
			FirmInn* firmInn = firm_array[ ownNation->ai_inn_array[i] ]->cast_to_FirmInn();

			for( int j=0 ; j<firmInn->inn_unit_count ; j++ )
			{
				InnUnit* innUnit = firmInn->inn_unit_array+j;

				if( unit_res[ innUnit->unit_id ]->race_id == majority_race() &&
					 innUnit->skill_level() > bestLeadership )
				{
					int importanceRating = 30 + (innUnit->skill_level()-curLeadership) *
												  (150+ownNation->pref_hire_unit) / 100;

					if( ownNation->ai_should_spend( importanceRating, innUnit->hire_cost ) )
					{
						unitRecno = firmInn->hire(j+1);
						break;
					}
				}
			}
		}
	}

	//---- if the above attempts failed, try locating a wandering independent hero ----//

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->hero_id && unitPtr->can_hire_hero(nation_recno) )
		{
			int importanceRating = 30 + (unitPtr->skill_level()-curLeadership) *
										  (150+ownNation->pref_hire_unit) / 100;

			if( ownNation->ai_should_spend( importanceRating, (float) unitPtr->hero_hire_cost() ) )
			{
				unitPtr->hire_hero(nation_recno);
				break;
			}
		}
	}

	//----- if all attempts failed, quit now -----//

	if( !unitRecno )
		return 0;

	//-------- assign the overseer now -------//

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_rank(RANK_GENERAL);

	//---- if there is already an existing but unprocessed one, delete it first ---//

	if( actionRecno )
		ownNation->del_action(actionRecno);

	return ownNation->add_action(loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_OVERSEER, firm_id, 1, unitRecno);
}
//-------- End of function FirmCamp::think_assign_better_commander ---------//


//------- Begin of function FirmCamp::cur_commander_leadership -------//
//
// [int] bestRaceId - if this is given, it will be used, otherwise
//							 it will use best_commander_race().
//
// Return the commander leadership
//
int FirmCamp::cur_commander_leadership(int bestRaceId)
{
	int commanderLeadership=0;

	//--- get the current leadership of the commander ----//

	if( overseer_recno )
	{
		if( !bestRaceId )
			bestRaceId = best_commander_race();

		Unit* unitCommander = unit_array[overseer_recno];

		if( unitCommander->race_id == bestRaceId )
			commanderLeadership = unitCommander->skill_level();
		else
			commanderLeadership = unitCommander->skill_level() / 2;		// divided by 2 if the race doesn't match
	}

	return commanderLeadership;
}
//-------- End of function FirmCamp::cur_commander_leadership ---------//


//------- Begin of function FirmCamp::new_commander_leadership -------//
//
// <int> newRaceId 	  - the race id. of the would-be commander.
// <int> newSkillLevel - the skill level of the would-be commander.
//
// Return the commander leadership if the unit is assigned to this camp.
//
int FirmCamp::new_commander_leadership(int newRaceId, int newSkillLevel)
{
	int commanderLeadership=0;
	int bestRaceId = best_commander_race();

	//--- get the current leadership of the commander ----//

	if( overseer_recno )
	{
		if( newRaceId == bestRaceId )
			commanderLeadership = newSkillLevel;
		else
			commanderLeadership = newSkillLevel / 2;		// divided by 2 if the race doesn't match
	}

	return (int) commanderLeadership;
}
//-------- End of function FirmCamp::new_commander_leadership ---------//


//------- Begin of function FirmCamp::best_commander_race -------//
//
// Return what race the commander should be for this camp.
//
int FirmCamp::best_commander_race()
{
	//---------------------------------------------//
	//
	// If this camp is the commanding camp of a town,
	// then return the majority race of the town.
	//
	// A camp is the commanding camp of a town when
	// it is the closest camp to the town.
	//
	//---------------------------------------------//

	for( int i=linked_town_count-1 ; i>=0 ; i-- )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->closest_own_camp() == firm_recno )
			return townPtr->majority_race();
	}

	//----- check if this camp is trying to capture an independent town ---//

	int targetTownRecno = think_capture_target_town();

	if( targetTownRecno && town_array[targetTownRecno]->nation_recno==0 )
		return town_array[targetTownRecno]->majority_race();

	//----- Otherwise return the majority race of this camp -----//

	return majority_race();
}
//-------- End of function FirmCamp::best_commander_race ---------//


//--------- Begin of function FirmCamp::think_upgrade_to_fort ---------//
//
int FirmCamp::think_upgrade_to_fort()
{
	return 0;
}
//----------- End of function FirmCamp::think_upgrade_to_fort ----------//
