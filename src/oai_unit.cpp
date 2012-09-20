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

//Filename   : OAI_UNIT.CPP
//Description: AI - unit related functions

#include <all.h>
#include <ounit.h>
#include <of_inn.h>
#include <of_camp.h>
#include <otown.h>
#include <onation.h>


//-------- Begin of function Nation::ai_get_unit -------//
//
// <int>	isCivilian	    -	whether the unit is a civilian unit
// <int>	raceId			 -	the race the selected unit should have
//										(0 for any races)
// <ActionNode*> actionNode - the ActionNode of the action that needs this skilled unit.
//
// return: <Unit*> skilledUnit - pointer to the skilled unit.
//
Unit* Nation::ai_get_unit(int isCivilian, int raceId, ActionNode* actionNode)
{
	//--------- get a skilled unit --------//

	Unit* wantedUnit;

	if(actionNode->unit_recno) // a unit has started training previously
	{
		wantedUnit = unit_array[actionNode->unit_recno];
		err_when( wantedUnit->is_civilian() != isCivilian );
	}
	else
	{
		char resultFlag;
		int  xLoc, yLoc;

		//---- for harbor, we have to get the land region id. instead of the sea region id. ----//
/*
		if( actionNode->action_mode==ACTION_AI_BUILD_FIRM &&
			 actionNode->action_para==FIRM_HARBOR )
		{
			int rc=0;

			for( yLoc=actionNode->action_y_loc ; yLoc<actionNode->action_y_loc+3 ; yLoc++ )
			{
				for( xLoc=actionNode->action_x_loc ; xLoc<actionNode->action_x_loc+3 ; xLoc++ )
				{
					if( region_array[ world.get_region_id(xLoc,yLoc) ]->region_type == REGION_LAND )
					{
						rc=1;
						break;
					}
				}

				if( rc )
					break;
			}
		}
		else
		{
*/			xLoc = actionNode->action_x_loc;
			yLoc = actionNode->action_y_loc;
//		}

		//-----------------------------------------//

		wantedUnit = ai_find_unit(isCivilian, raceId, xLoc, yLoc, resultFlag, actionNode->action_id);

		if( !wantedUnit && !isCivilian && raceId )
		{
			wantedUnit = ai_find_unit_from_camp(raceId, xLoc, yLoc);
		}

		if( !wantedUnit )		// skilled unit not found
			return NULL;
	}

	//------ if the unit is still in training -----//

	if( !wantedUnit->is_visible() )
	{
//		actionNode->next_retry_date = info.game_date + TOTAL_TRAIN_DAYS + 1;
		return NULL;		// continue processing this action after this date, this is used when training a unit for construction
	}

	err_when( !wantedUnit->race_id );
	err_when( wantedUnit->is_civilian() != isCivilian );

	return wantedUnit;
}
//-------- End of function Nation::ai_get_unit -------//


//--------- Begin of function Nation::ai_find_unit --------//
//
// <int>	isCivilian		-	whether the unit is a civilian unit
// <int>		raceId			-	the race the selected unit should have
//										(0 for any races)
// <short>	destX, destY	-	location the unit move to
// <char&>	resultFlag		-	describle how to find the skilled unit
//										0 - for unable to train unit,
//										1 - for existing skilled unit
//										2 - for unit hired from inn
//										3 - for training unit in town (training is required)
//
// [int]	   actionId - the action id. of the action which
//							  the unit should do after it has finished training.
//
// return the unit pointer pointed to the skilled unit
//
Unit* Nation::ai_find_unit(int isCivilian, int raceId, short destX, short destY, char& resultFlag, int actionId)
{
	//----- try to find an existing unit with the required skill -----//

	Unit	*wantedUnit = NULL;
	Unit	*unitPtr;
	Firm	*firmPtr;
	short	curDist, minDist=0x1000;
	int   destRegionId = world.get_region_id(destX, destY);

	for(int i=unit_array.size(); i>0; i--)
	{
		if(unit_array.is_deleted(i))
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno!=nation_recno || !unitPtr->race_id )
			continue;

		if( raceId )
		{
			if( unitPtr->race_id != raceId )
				continue;
		}
		else
		{
			if( !unitPtr->is_human() )
				continue;
		}

		//---- if this unit is on a mission ----//

		if( unitPtr->home_camp_firm_recno )
			continue;

		if( unitPtr->region_id() != destRegionId )
			continue;

		if( unitPtr->is_civilian() != isCivilian )
			continue;

		if( unitPtr->unit_mode )		// it can be a camp defender.
			continue;

		//----- if this is a mobile unit ------//

		if( unitPtr->is_visible() )
		{
			if( !unitPtr->is_all_stop() )
				continue;

			if( unitPtr->cur_action!=SPRITE_ATTACK && !unitPtr->cur_order.ai_action_id )
			{
				curDist = misc.points_distance(unitPtr->next_x_loc(), unitPtr->next_y_loc(), destX, destY);

				if(curDist < minDist)
				{
					wantedUnit = unitPtr;
					minDist = curDist;
				}
			}
		}

		//------- if this is an overseer ------//

		else if( unitPtr->unit_mode==UNIT_MODE_OVERSEE )
		{
			firmPtr = firm_array[unitPtr->unit_mode_para];

			if( firmPtr->region_id != destRegionId )
				continue;

			if( firmPtr->cast_to_FirmCamp() )
			{
				//--- if this military camp is going to be closed, use this overseer ---//

				if( firmPtr->should_close_flag )
				{
					firmPtr->cast_to_FirmCamp()->mobilize_overseer();
					wantedUnit = unitPtr;       	// pick this overseer
					break;
				}
			}
		}
	}

	//---------------------------------------------------//

	if(wantedUnit)
	{
		resultFlag = 1;
		err_when( wantedUnit->is_civilian() != isCivilian );
	}
	else
	{
		//--- if no existing skilled unit found, try to hire one from inns ---//

		int unitRecno=0;

		if( !isCivilian )		// if the wanted unit is a military, we can try to hire it from the inn
		{
			unitRecno = hire_unit(raceId, isCivilian, false, destX, destY); 	// this function will try going with hiring units that are better than training your own ones. false-don't hire a spy, just hire a normal military unit

			if( unitRecno )
			{
				resultFlag = 2;
				err_when( unit_array[unitRecno]->is_civilian() != isCivilian );
			}
		}

		if( !unitRecno && isCivilian )
		{
			int recruitTownRecno;
			unitRecno = recruit_peasant(raceId, destX, destY, recruitTownRecno);

			if( unitRecno )
			{
				err_when( unit_array[unitRecno]->is_civilian() != isCivilian );
				resultFlag = 3;
			}
			else
				resultFlag = 0;
		}

		if( unitRecno )
			wantedUnit = unit_array[unitRecno];
	}

	return wantedUnit;
}
//---------- End of function Nation::ai_find_unit --------//


//--------- Begin of function Nation::ai_should_hire_unit --------//
//
// <int> importantRating - importance of hiring the unit.
//
int Nation::ai_should_hire_unit(int importanceRating)
{
	if( !ai_inn_count )		// don't hire any body in the cash is too low
		return 0;

	return ai_should_spend(importanceRating + pref_hire_unit/5 - 10 );		// -10 to +10 depending on pref_hire_unit
}
//---------- End of function Nation::ai_should_hire_unit --------//


//--------- Begin of function Nation::ai_find_unit_from_camp --------//
//
// <int>		raceId			-	the race the selected unit should have
//										(0 for any races)
// <short>	destX, destY	-	location the unit move to
//
// return the unit pointer pointed to the skilled unit
//
Unit* Nation::ai_find_unit_from_camp(int raceId, short destX, short destY)
{
	int 		 curRating, bestRating=0, bestSoldierId=0;
	FirmCamp* bestFirm=NULL;

	for( int i=0 ; i<ai_camp_count ; i++ )
	{
		FirmCamp* firmCamp = firm_array[ ai_camp_array[i] ]->cast_to_FirmCamp();

		err_when( !firmCamp );

		int j;
		for( j=firmCamp->soldier_count-1 ; j>=0 ; j-- )
		{
			if( !firmCamp->soldier_array[j].is_under_training() &&
				 firmCamp->soldier_array[j].race_id == raceId )
			{
				break;
			}
		}

		if( j>=0 )
		{
			curRating = world.distance_rating(destX, destY, firmCamp->center_x, firmCamp->center_y);

			if( curRating > bestRating )
			{
				bestRating = curRating;
				bestFirm   = firmCamp;
				bestSoldierId = j+1;
			}
		}
	}

	if( bestFirm )
	{
		int unitRecno = bestFirm->mobilize_soldier( bestSoldierId, COMMAND_AI );

		if( unitRecno )
			return unit_array[unitRecno];
	}

	return NULL;
}
//---------- End of function Nation::ai_find_unit_from_camp --------//


//--------- Begin of function Nation::hire_unit --------//
//
// <int>		raceId  - the race the selected unit should have
//							 (0 for any races)
// <int> isCivilian - whether the unit to be hired should be a civilian unit
// <int>   hireSpy  - whether hire a spy or a normal military unit
//	<short>	destX	  - the x location the unit will move to
//	<short>	destY	  - the y location the unit will move to
//
// Note: Units hired in inns are military units only.
// 		There are no civilian units in inns.
//
// return: <int> recno of the unit recruited.
//
int Nation::hire_unit(int raceId, int isCivilian, int hireSpy, short destX, short destY)
{
	if( !ai_should_hire_unit(20) )			// 20 - importance rating
		return 0;

	//-------------------------------------------//

	FirmInn	*firmInnPtr;
	Firm		*firmPtr;
	InnUnit *innUnit;
	Skill		*innUnitSkill;
	int		i, j, innUnitCount, curRating, curFirmDist;
	int		bestRating=0, bestInnRecno=0, bestInnUnitId=0;
	int		destRegionId = world.get_region_id(destX, destY);

	for(i=0; i<ai_inn_count; i++)
	{
		firmPtr = firm_array[ai_inn_array[i]];

		if( firmPtr->region_id != destRegionId )
			continue;

		firmInnPtr = firmPtr->cast_to_FirmInn();

		innUnitCount=firmInnPtr->inn_unit_count;

		if( !innUnitCount )
			continue;

		innUnit = firmInnPtr->inn_unit_array + innUnitCount - 1;

		curFirmDist = misc.points_distance(firmPtr->center_x, firmPtr->center_y, destX, destY);

		//------- check units in the inn ---------//

		for(j=innUnitCount; j>0; j--, innUnit--)
		{
			innUnitSkill = &(innUnit->skill);

			if( raceId && unit_res[innUnit->unit_id]->race_id != raceId )
				continue;

			if( isCivilian != unit_res[innUnit->unit_id]->is_civilian )
				continue;

			if( hireSpy && innUnit->spy_skill==0 )
				continue;

			if( cash < innUnit->hire_cost )
				continue;

			//----------------------------------------------//
			// evalute a unit on:
			// -its race, whether it's the same as the nation's race
			// -the inn's distance from the destination
			// -the skill level of the unit.
			//----------------------------------------------//

			curRating = (int) innUnit->skill_level()
							- (100-100*curFirmDist/MAX_WORLD_X_LOC);

			if( unit_res[innUnit->unit_id]->race_id == race_id )
				curRating += 50;

			if( curRating > bestRating )
			{
				bestRating = curRating;

				bestInnRecno  = firmInnPtr->firm_recno;
				bestInnUnitId = j;
			}
		}
	}

	//----------------------------------------------------//

	if( bestInnUnitId )
	{
		firmPtr = firm_array[bestInnRecno];
		firmInnPtr = firmPtr->cast_to_FirmInn();

		return firmInnPtr->hire(bestInnUnitId);
	}

	return 0;
}
//---------- End of function Nation::hire_unit --------//


//--------- Begin of function Nation::recruit_peasant --------//
//
// <int>		raceId  - the race the selected unit should have
//							 (0 for any races)
//	<short>	destX	  - the x location the unit will move to
//	<short>	destY	  - the y location the unit will move to
//
// <int&>   recruitTownRecno - the recno of the town where this unit is recruited.
//
// return: <int> recno of the unit recruited.
//
int Nation::recruit_peasant(int raceId, short destX, short destY, int& recruitTownRecno)
{
	//----- locate the best town for training the unit -----//

	int 	 i;
	Town	 *townPtr;
	int	 curDist, curRating, bestRating=0;
	int    destRegionId = world.get_loc(destX, destY)->region_id;

	recruitTownRecno = 0;

	for(i=0; i<ai_town_count; i++)
	{
		townPtr = town_array[ai_town_array[i]];

		if( !townPtr->jobless_population ||	// no jobless population or currently a unit is being trained
			 !townPtr->can_recruit_peasant() )
		{
			continue;
		}

		if( townPtr->region_id != destRegionId )
			continue;

		if( raceId && townPtr->race_id != raceId )
			continue;

		//--------------------------------------//

		curDist = misc.points_distance(townPtr->center_x, townPtr->center_y, destX, destY);

		curRating = 100-100*curDist/MAX_WORLD_X_LOC;

		if( curRating > bestRating )
		{
			bestRating 	   = curRating;
			recruitTownRecno = townPtr->town_recno;
		}
	}

	if( !recruitTownRecno )
		return 0;

	//--- if the chosen race is not recruitable, pick any recruitable race ---//

	townPtr = town_array[recruitTownRecno];

	if( !townPtr->can_recruit_peasant() )
	{
		//---- if the loyalty is too low to recruit, grant the town people first ---//

		if( cash > 0 && townPtr->accumulated_reward_penalty < 10 )
			townPtr->reward(COMMAND_AI);

		//---- if the loyalty is still too low, return now ----//

		if( !townPtr->can_recruit_peasant() )
			return 0;
	}

	return townPtr->recruit(0, COMMAND_AI);	// 0-not recruit wagon
}
//---------- End of function Nation::recruit_peasant --------//


//--------- Begin of function Nation::recruit_in_firm_soldier --------//
//
// Get soldiers currently in camps or forts for assigning them to another
// camp or fort.
//
// <Firm*> destFirmPtr  - the firm which the workers are recruited for.
//
// [int] preferedRaceId - the prefered race id.
//								  (if not given, the majority race of the destination
//								   firm will be used.)
//
// return: <int> recno of the unit recruited.
//
int Nation::recruit_in_firm_soldier(Firm* destFirmPtr, int preferedRaceId)
{
	err_when( destFirmPtr->cast_to_FirmCamp() == NULL );	// it must either be a Camp or a Fort 

	if( !preferedRaceId )
	{
		preferedRaceId = destFirmPtr->majority_race();

		if( !preferedRaceId )
			return 0;
	}

	//--- scan existing firms to see if any of them have excess workers ---//

	FirmCamp*	firmCamp, *bestFirmCamp=NULL;
	int			bestRating=0, curRating;
	int			bestFirmSoldierId=0;

	int i;
	for( i=0 ; i<ai_camp_count ; i++ )
	{
		firmCamp = firm_array[ai_camp_array[i]]->cast_to_FirmCamp();

		err_when( !firmCamp );

		if( firmCamp->firm_recno == destFirmPtr->firm_recno )
			continue;

		if( firmCamp->region_id != destFirmPtr->region_id )
			continue;

		int excessCombatLevel = firmCamp->total_combat_level() - firmCamp->ai_combat_level_needed();

		curRating = excessCombatLevel/3 +
						world.distance_rating( firmCamp->center_x, firmCamp->center_y,
						destFirmPtr->center_x, destFirmPtr->center_y );

		for( int j=0 ; j<firmCamp->soldier_count ; j++ )
		{
			if( firmCamp->soldier_array[j].is_under_training() )
				continue;

			if( firmCamp->soldier_array[j].race_id == preferedRaceId )
				curRating += 50;
		}

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestFirmCamp = firmCamp;
		}
	}

	if( !bestFirmCamp )
		return 0;

	//------ mobilize a worker form the selected firm ----//

	int bestSoldierId=0;
		 bestRating = 0;

	Soldier* soldierPtr = bestFirmCamp->soldier_array;

	for( i=0 ; i<bestFirmCamp->soldier_count ; i++, soldierPtr++ )
	{
		if( soldierPtr->is_under_training() )
			continue;

		if( soldierPtr->race_id == preferedRaceId ) 	// same race
			curRating = 50;
		else if( soldierPtr->race_id == 0 )		// weapon
			curRating = 20;
		else             				// different race
			curRating = 10;

		if( curRating > bestRating )
		{
			bestRating 	  = curRating;
			bestSoldierId = i+1;
		}
	}

	if( !bestSoldierId )
		return 0;

	return bestFirmCamp->mobilize_soldier( bestSoldierId, COMMAND_AI );
}
//---------- End of function Nation::recruit_in_firm_soldier --------//

