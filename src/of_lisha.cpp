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

//Filename    : OF_LAIRA.CPP
//Description : Firm Monster Lair  - AI functions

#include <otown.h>
#include <ounit.h>
#include <onation.h>
#include <omonsres.h>
#include <of_lair.h>

//--------- Begin of function FirmLair::process_ai ---------//

void FirmLair::process_ai()
{
	if( !nation_recno )
		return;

	//------- assign overseer and soldiers -------//
/*
	if( info.game_date%7==firm_recno%7 || info.game_start_date == info.game_date )		// recruit immediately in the beginning of the game
		think_recruit();

	//--- if the firm is empty and should be closed, sell/destruct it now ---//

	if( should_close_flag )
	{
		if( soldier_count==0 && overseer_recno==0 )
		{
			ai_del_firm();
			return;
		}
	}

	//----- think about assigning a better commander -----//

	if( info.game_date%30==firm_recno%30 )
		think_assign_better_commander();

	//------ think about attacking nearby enemy -----//

	if( info.game_date%30==firm_recno%30 )
		think_attack_nearby_enemy();

	//----- think about attacking linked independent towns -----//

	if( info.game_date%15==firm_recno%15 )
		think_attack_linked_town();

	//----- think about attacking camps/lairs newly built to compete with this lair to control a town -----//

	if( info.game_date%15==firm_recno%15 )
		think_attack_competing_camp();

	//------ think about capturing other independent towns -----//

	if( info.game_date%60==firm_recno%60 || info.game_start_date == info.game_date )		// recruit immediately in the beginning of the game
		think_repress_new_town();*/
}
//----------- End of function FirmLair::process_ai -----------//


//--------- Begin of function FirmLair::ai_should_close ---------//
//
// Whether this AI firm should be closed or not.
//
int FirmLair::ai_should_close()
{
//	return linked_town_count==0;
	return 0;
}
//----------- End of function FirmLair::ai_should_close ----------//


//------ Begin of function FirmLair::think_repress_new_town -------//

void FirmLair::think_repress_new_town()
{
	//------ see if we have enough force to capture town -----//

	Nation* nationPtr = nation_array[nation_recno];

	if( !overseer_recno || 
		 soldier_count < 1 + (should_close_flag ? 0 : nationPtr->pref_force_projection/30) )
	{
		return;
	}

//	if( !nationPtr->ai_should_spend( 35 + nationPtr->pref_collect_tax/3, firm_res[FIRM_LAIR]->setup_cost) )
//		return;

	//------- see if we still have some lairs in the process of subduing existing linked towns ----//

	int outstandingLair = 0;

	for( int i=0 ; i<nationPtr->ai_camp_count ; i++ )
	{
		FirmLair* firmLair = firm_array[nationPtr->ai_camp_array[i]]->cast_to_FirmLair();

		if( firmLair )
		{
			if( firmLair->under_construction || firmLair->has_resisting_linked_town() )
			{
				if( ++outstandingLair == 2 )
					return;
			}
		}
	}

	//---- determine which town to capture ----//

	Town *townPtr, *bestTown=NULL;
	int  curRating, bestRating= -1000;

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno )	// only independent towns
			continue;

		//---- check if we there is already camp/lair next to it ----//

		int skipFlag=0;

		for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* firmPtr = firm_array[townPtr->linked_firm_array[j]];

			if( firmPtr->firm_id == FIRM_LAIR && firmPtr->nation_recno )
			{
				skipFlag = 1;
				break;
			}
		}

		if( skipFlag )
			continue;

		//------------------------------------//

		curRating = world.distance_rating(center_x, center_y, townPtr->center_x, townPtr->center_y);

		curRating -= townPtr->linked_camp_count(false) * 50;		// false-don't just count those with an oversee, count all camps

		if( curRating > bestRating )
		{
			//--- check if a unit is already on its way to build firm for capturing this town ---//

			if( !nationPtr->is_action_exist( 0, 0, townPtr->loc_x1, townPtr->loc_y1,
				 ACTION_AI_BUILD_FIRM, FIRM_LAIR, 0, 2 ) )						// 2-only check the reference location, ignore the action location
			{
				bestRating = curRating;
				bestTown   = townPtr;
			}
		}
	}

	if( !bestTown )
		return;

	//---- promote the best soldier to general and order it to build a lair -----//

	bestRating=0;
	int bestSoldierId=0;

	for( i=0 ; i<soldier_count ; i++ )
	{
		if( soldier_array[i].is_under_training() )
			continue;

		curRating = soldier_array[i].hit_points;

		if( curRating > bestRating )
		{
			bestRating    = curRating;
			bestSoldierId = i+1;
		}
	}

	if( !bestSoldierId )
		return;

	int unitRecno = mobilize_soldier(bestSoldierId, COMMAND_AI);

	if( !unitRecno )
		return;

	unit_array[unitRecno]->set_rank(RANK_GENERAL);

	//------- locate a place to build the camp -------//

	short buildXLoc, buildYLoc;

	if( !nationPtr->find_best_place_loc(FIRM_LAIR, bestTown->loc_x1, bestTown->loc_y1, buildXLoc, buildYLoc) )
	{
		bestTown->no_neighbor_space = 1;
		return;
	}

	//---------- add the action to the queue now ----------//

	nationPtr->add_action( buildXLoc, buildYLoc, bestTown->loc_x1, bestTown->loc_y1,
		ACTION_AI_BUILD_FIRM, FIRM_LAIR, 1, unitRecno );
}
//------- End of function FirmLair::think_repress_new_town ---------//


//--------- Begin of function FirmLair::think_recruit ---------//
//
// Think about recruiting an overseer and soliders to this base.
//
// [int] combatLevelDiff - if this is given, this is the extra
//							      combat level this firm needs.
//									if not given, it is ai_combat_level_needed() - total_combat_level()
//
void FirmLair::think_recruit(int combatLevelDiff)
{
	if( is_soldier_full() )
		return;

	if( !combatLevelDiff )
		combatLevelDiff = ai_combat_level_needed() - total_combat_level();

	//------ get soldiers from other lairs -------//

	int loopCount=0;

	while( combatLevelDiff > 0 )		// not enough combat level
	{
		int objPower = ai_get_soldier_from_other_lair();

		if( objPower )
			combatLevelDiff -= objPower;
		else
			break;

		err_when( ++loopCount > 1000 );
	}

	//--- if we cannot further recruit units because of the lack of money or live points ---//

	if( !can_recruit() )
		return;

	//---- don't recruit further if there are more than one unit under training currently ---//

	int trainingCount=0;

	for( int i=0 ; i<soldier_count ; i++ )
	{
		if( soldier_array[i].is_under_training() )
			trainingCount++;
	}

	if( trainingCount >= 2 )
		return;

	//-------------------------------------//

	Nation* nationPtr = nation_array[nation_recno];

	int importanceRating = 70 * (MAX_SOLDIER-soldier_count) / MAX_SOLDIER
								  + max(combatLevelDiff,0)/5;

	int unitId = monster_res[monster_id()]->unit_id;

	if( !nationPtr->ai_should_spend(importanceRating, unit_res[unitId]->build_cost) )
		return;

	recruit_soldier(COMMAND_AI);
}
//----------- End of function FirmLair::think_recruit ----------//


//----- Begin of function FirmLair::ai_get_soldier_from_other_lair -----//
//
// Get one soldier from another lair.
//
// return: <int> >0 - succeeded. The obj_power() of the added unit.
//						0 - failed.
//
int FirmLair::ai_get_soldier_from_other_lair()
{
	//---- think about getting units from other lairs ----//

	int 		 curRating, bestRating=0;
	Nation*   nationPtr = nation_array[nation_recno];
	FirmLair* bestFirm=NULL;

	for( int i=0 ; i<nationPtr->ai_camp_count ; i++ )
	{
		FirmLair* firmLair = firm_array[ nationPtr->ai_camp_array[i] ]->cast_to_FirmLair();

		if( !firmLair )
			continue;

		if( firmLair->region_id != region_id )
			continue;

		if( firmLair->has_resisting_linked_town() )		// if this firm is dealing with resistanting towns itself, don't bother to get units from it
			continue;

		//--- if this Lair is going to be closed down ---//

		if( firmLair->should_close_flag )
		{
			bestFirm = firmLair;
			break;
		}

		//-----------------------------------------------//

		if( firmLair->soldier_count <= soldier_count + patrol_unit_count )   // only if the firm has more soldier than this
			continue;

		curRating = (firmLair->soldier_count - soldier_count - patrol_unit_count) * 10
						+ world.distance_rating( center_x, center_y,
						firmLair->center_x, firmLair->center_y );

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestFirm   = firmLair;
		}
	}

	if( bestFirm )
	{
		int unitRecno=0;

		//---- if the firm is to be closed and the king is the commander there ---//

		if( bestFirm->should_close_flag && bestFirm->overseer_recno &&
			 m.random(3)==0 )		// and some randomness
		{
			unitRecno = bestFirm->mobilize_overseer();
		}

		//-------- otherwise mobilize a soldier --------//

		if( !unitRecno )
		{
			int readySoldierCount = bestFirm->ready_soldier_count();

			if( readySoldierCount > 0 )
			{
				unitRecno = bestFirm->mobilize_soldier( m.random(readySoldierCount)+1, COMMAND_AI );
			}
		}

		//----------------------------------------------//

		if( unitRecno )
		{
			int objPower = unit_array[unitRecno]->obj_power();

			unit_array[unitRecno]->assign(loc_x1, loc_y1);

			return objPower;
		}
	}

	return 0;
}
//------ End of function FirmLair::ai_get_soldier_from_other_lair -----//


//--------- Begin of function FirmLair::ai_recruit ---------//
//
// <int> recruitCount - the number of soldiers to be recruited.
//
// return: <int> 1-succeeded, 0-failed.
//
int FirmLair::ai_recruit(int recruitCount)
{
	for( int i=0 ; i<recruitCount ; i++ )
		recruit_soldier(COMMAND_AI);

	return 1;
}
//----------- End of function FirmLair::ai_recruit ----------//


//--------- Begin of function FirmLair::think_attack_linked_town ---------//
//
// Think about attacking linked town to force them into submission and
// pay tribute.
//
void FirmLair::think_attack_linked_town()
{
	if( ready_soldier_count()==0 && !overseer_recno )
		return;

	//---------------------------------//

	int hasWar;
	int firmCombatLevel = total_combat_level();
	int scanRange = world.effective_distance(0, FIRM_LAIR);		// 0-town. The effective distance between a town and a Fryhtan Lair
	int recruitCombatLevel=0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno == 0 &&
			 townPtr->resistance(nation_recno) >= MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			int rc = 0;
			int defenseCombatLevel = townPtr->possible_defender_combat_level(nation_recno);

			if( firmCombatLevel > defenseCombatLevel )
			{
				rc = 1;
			}
			else
			{
				//---- if there is a battle going on between the defenders and our units ----//

				if( townPtr->town_defender_count > 0 &&
					 world.net_attacker_power_in_area(townPtr->center_x, townPtr->center_y,
					 nation_recno, 0, 1, hasWar, scanRange) == -1 )		// -1 means a battle is already going on
				{
					rc = 1;
				}
				else
				{
					recruitCombatLevel = defenseCombatLevel-firmCombatLevel;		// need to recruit new soldiers
				}
			}

			//-------- attack now ---------//

			if( rc )
			{
				if( nation_array[nation_recno]->ai_attack_target(townPtr->loc_x1,
					 townPtr->loc_y1, defenseCombatLevel, 0, 0, 0, firm_recno, 0, 1) )		// 1-retain target combat level, don't apply miliary projection preference 
				{
					return;
				}
			}
		}
	}

	//----- if we need to recruit new units -------//

	if( recruitCombatLevel )
	{
		think_recruit(recruitCombatLevel);
	}
}
//----------- End of function FirmLair::think_attack_linked_town ----------//


//--------- Begin of function FirmLair::has_resisting_linked_town ---------//
//
// Return whether this lair has linked towns with resistance > 0.
//
int FirmLair::has_resisting_linked_town()
{
	int firmCombatLevel = total_combat_level();

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno == 0 && townPtr->resistance(nation_recno) >= MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
			return 1;
	}

	return 0;
}
//----------- End of function FirmLair::has_resisting_linked_town ----------//


//--------- Begin of function FirmLair::ai_combat_level_needed ---------//
//
// Think about the needed combat level by this lair.
//
int FirmLair::ai_combat_level_needed()
{
	Town*	  townPtr;
	int  	  combatNeeded=0;
	Nation* nationPtr = nation_array[nation_recno];

	//------- scan for linked towns ---------//

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno )		// only independent towns
			continue;

		//------ if the town has other lairs linked to it, the needed combat level is determined by the number of firms of other kingdoms

		for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* firmPtr = firm_array[ townPtr->linked_firm_array[j] ];

			if( firmPtr->nation_recno )
			{
				int relationStatus = nationPtr->get_relation_status(firmPtr->nation_recno);

				if( relationStatus == RELATION_HOSTILE )
					combatNeeded += townPtr->population * 10;

				else if( relationStatus <= RELATION_NEUTRAL )
					combatNeeded += townPtr->population * 5;
			}
		}
	}

	//--- if the overseer is the king, increase its combat level needed ---//

	if( overseer_recno && unit_array[overseer_recno]->rank_id == RANK_KING )
		combatNeeded = max(400, combatNeeded);

	//---------------------------------------//

	return combatNeeded;
}
//----------- End of function FirmLair::ai_combat_level_needed ----------//


//--------- Begin of function FirmLair::think_attack_competing_camp ---------//
//
// Think about attacking camps/lairs newly built to compete with this lair
// to control a town.
//
int FirmLair::think_attack_competing_camp()
{
	int 		curRating, minRating=0xFFFF;
	Firm* 	bestTargetFirm=NULL;
	Nation* 	nationPtr = nation_array[nation_recno];

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->nation_recno == 0 &&
			 townPtr->resistance(nation_recno) >= MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			//------ scan other competing camps/lairs -------//

			for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
			{
				Firm* firmPtr = firm_array[ townPtr->linked_firm_array[j] ];

				//--- only attack those camps that were set up later than ours ---//

				if( firmPtr->nation_recno != nation_recno &&
					 firmPtr->nation_recno &&
					 firmPtr->setup_date > setup_date )
				{
					int relationStatus = nationPtr->get_relation_status(firmPtr->nation_recno);

					//-------------------------------------//
					//
					// Attack when one of the following is true:
					//
					// - pref_monster_aggressiveness > 50
					// - the nation of the competing firm is hostile with us
					// - the nation of the competing firm is not friendly with us
					//   and it is a human nation.
					//
					//-------------------------------------//

					if( nationPtr->pref_monster_aggressiveness > 50 ||
						 relationStatus == RELATION_HOSTILE ||
						 (relationStatus <= RELATION_NEUTRAL &&
						  nation_array[firmPtr->nation_recno]->is_human()) )
					{
						curRating = nationPtr->get_relation_status(firmPtr->nation_recno) * 150
										+ firmPtr->total_combat_level();

						if( curRating < minRating )
						{
							minRating 		= curRating;
							bestTargetFirm = firmPtr;
						}
					}
				}
			}
		}
	}

	if( bestTargetFirm )
	{
		if( nationPtr->ai_attack_target(bestTargetFirm->loc_x1,
			 bestTargetFirm->loc_y1, bestTargetFirm->total_combat_level(),
			 0, 0, 0, firm_recno, 0, 1) )		// 1-retain target combat level, don't apply miliary projection preference
		{
			return 1;
		}
	}

	return 0;
}
//----------- End of function FirmLair::think_attack_competing_camp ----------//

