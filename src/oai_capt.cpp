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

//Filename   : OAI_CAPT.CPP
//Description: AI - capturing independent towns

#include <stdlib.h>
#include <ALL.H>
#include <OREMOTE.H>
#include <OCONFIG.H>
#include <OUNIT.H>
#include <OF_CAMP.H>
#include <OF_INN.H>
#include <ONATION.H>

//------- define struct CaptureTown -------//

struct CaptureTown
{
	short town_recno;
	short min_resistance;
};

//------ Declare static functions --------//

static int sort_capture_town_function( const void *a, const void *b );

//--------- Begin of function Nation::think_capture --------//
//
int Nation::think_capture()
{
	if( ai_camp_count==0 )		// this can happen when a new nation has just emerged
		return 0;

	//--- don't capture if the AI is using growth and capture strategy (as opposite to build mine strategy) ---//

	if( ai_mine_count==0 && total_population < 25 )
		return 0;

	//---- think capture independent towns -------//

	if( think_capture_independent(false) ) 	// false - try to use existing units instead of trying to hire new units
		return 1;

	if( think_capture_independent(true) )		// true - try to hire new units
		return 1;

	return 0;
}
//---------- End of function Nation::think_capture ---------//


//--------- Begin of function Nation::think_capture_independent --------//
//
// Think about capturing independent towns.
//
// <int> hireCapturer  - whether we should hire new units for capturing
//							 independent towns or try to locate existing units.
//
int Nation::think_capture_independent(int hireCapturer)
{
	//------- Capture target choices -------//

	#define MAX_CAPTURE_TOWN	40

	CaptureTown capture_town_array[MAX_CAPTURE_TOWN];
	short 		capture_town_count=0;

	//--- find the town that makes most sense to capture ---//

	int 	townRecno;
	Town* townPtr;

	for(townRecno=town_array.size(); townRecno>0; townRecno--)
	{
		if(town_array.is_deleted(townRecno))
			continue;

		townPtr = town_array[townRecno];

		if( townPtr->nation_recno )		// only capture independent towns
			continue;

		if( townPtr->no_neighbor_space )		// if there is no space in the neighbor area for building a new firm.
			continue;

		if( townPtr->rebel_recno )			// towns controlled by rebels will not drop in resistance even if a command base is present
			continue;

		//------ only if we have a presence/a base town in this region -----//

		if( !has_base_town_in_region(townPtr->region_id) )
			continue;

		//---- check if there are already camps linked to this town ----//

		for( int i=townPtr->linked_firm_count-1 ; i>=0 ; i-- )
		{
			FirmCamp* firmCamp = firm_array[ townPtr->linked_firm_array[i] ]->cast_to_FirmCamp();

			if( !firmCamp )
				continue;

			//------ if we already have a camp linked to this town -----//

			if( firmCamp->nation_recno == nation_recno )
				break;

			//--- if there is an overseer with high leadership and right race in the opponent's camp, do bother to compete with him ---//

			if( firmCamp->overseer_recno && firmCamp->nation_recno )		// ignore independent Fryhtan structures
			{
				Unit* unitPtr = unit_array[firmCamp->overseer_recno];

				if( unitPtr->skill_level() >= 70 &&
					 unitPtr->race_id == townPtr->race_id )
				{
					break;
				}
			}
		}

		if( i>=0 )			// there is already a camp linked to this town and we don't want to get involved with its capturing plan
			continue;

		//--- if a unit is already on its way to build firm for capturing this town ---//

		if( is_action_exist( -1, -1, townPtr->loc_x1, townPtr->loc_y1,
			 ACTION_AI_BUILD_FIRM, FIRM_CAMP ) ||
			 is_action_exist( -1, -1, townPtr->loc_x1, townPtr->loc_y1,
			 ACTION_AI_BUILD_FIRM, FIRM_FORT ) )
		{
			continue;
		}

		//-- if the town has linked military camps of the same nation --//

		int targetResistance = capture_expected_resistance(townRecno, hireCapturer);

		if( targetResistance < 50 - pref_peacefulness/5 )		// 30 to 50 depending on
		{
			capture_town_array[capture_town_count].town_recno 	   = townRecno;
			capture_town_array[capture_town_count].min_resistance = targetResistance;
			if (capture_town_count < MAX_CAPTURE_TOWN)
				capture_town_count++;
		}
	}
	err_when(capture_town_count > MAX_CAPTURE_TOWN);
	//------ sort the capture target choices by min_resistance ----//

	qsort( &capture_town_array, capture_town_count, sizeof(capture_town_array[0]), sort_capture_town_function );

	//------- try to capture the town in their resistance order ----//

	for( int i=0 ; i<capture_town_count ; i++ )
	{
		err_when( town_array.is_deleted(capture_town_array[i].town_recno) );

		//-------------------------------------------//
		// If the map is set to unexplored, wait for a
		// reasonable amount of time before moving out
		// to build the mine.
		//-------------------------------------------//

		if( !config.explore_whole_map )
		{
			Town* targetTown = town_array[ capture_town_array[i].town_recno ];

			for( int i=0 ; i<ai_town_count ; i++ )
			{
				Town* ownTown = town_array[ ai_town_array[i] ];

				int townDistance = m.points_distance(targetTown->center_x, targetTown->center_y,
										 ownTown->center_x, ownTown->center_y);

				if( info.game_date-info.game_start_date >
					 townDistance * (5-config.ai_aggressiveness) / 5 )		// 3 to 5 / 5
				{
					break;
				}
			}

			if( i==ai_town_count )
				continue;
		}

		if( start_capture( capture_town_array[i].town_recno, hireCapturer ) )
			return 1;
	}

	return 0;
}
//---------- End of function Nation::think_capture_independent ---------//


//--------- Begin of function Nation::should_use_cash_to_capture --------//
//
int Nation::should_use_cash_to_capture()
{
	//--- if we have plenty of cash, use cash to decrease the resistance of the villagers ---//

	if( cash >= 20000 * (200+pref_cash_reserve-pref_loyalty_concern) / 200 )
		return 1;

	return military_rank_rating() < 50+pref_peacefulness/5 &&		// 50 to 70
			 ai_should_spend(pref_loyalty_concern/4);
}
//---------- End of function Nation::should_use_cash_to_capture ---------//


//--------- Begin of function Nation::capture_expected_resistance --------//
//
// The lowest resistance can be expected if we are going to capture the
// town.
//
// <int>  townRecno - the recno of the target independent town
// <int> hireCapturer  - whether we should hire new units for capturing
//							 independent towns or try to locate existing units.
//
int Nation::capture_expected_resistance(int townRecno, int hireCapturer)
{
	//--- we have plenty of cash, use cash to decrease the resistance of the villagers ---//

	if( should_use_cash_to_capture() )
		return 0;			// return zero resistance

	//--- get the target resistance of the town if the best available unit assigned there as overseer ---//

	int targetResistance;

	if( hireCapturer )
		targetResistance = hire_best_capturer(townRecno, false);		// false - just checking, don't hire now
	else
		find_best_capturer(townRecno, targetResistance);

	return targetResistance;
}
//---------- End of function Nation::capture_expected_resistance ---------//


//--------- Begin of function Nation::start_capture --------//
//
// <int> townRecno 	 - the target town
//
// <int> hireCapturer - 0 - only hire a new unit as capturer
//								1 - only use an existing unit as capturer
//							   2 - first try using an existing unit,
//								    if failed, then try to hire a new unit.
//
int Nation::start_capture(int townRecno, int hireCapturer)
{
	Town* captureTown = town_array[townRecno];

	//---- find the best available general for the capturing action ---//

	int targetResistance, unitRecno;

	//-------- 0 - only hire a new unit as capturer ------//

	if( hireCapturer==0 )
		unitRecno = hire_best_capturer(townRecno, true);		// true-hire now

	//----- 1 - only use an existing unit as capturer ----//

	else if ( hireCapturer==1 )
		unitRecno = find_best_capturer(townRecno, targetResistance);

	//-- 2 - first try using an existing unit, if failed, then try to hire a new unit. --//

	else if ( hireCapturer==2 )
	{
		unitRecno = find_best_capturer(townRecno, targetResistance);

		if( !unitRecno )
			unitRecno = hire_best_capturer(townRecno, true);		// true-hire now
	}
	else
		err_here();

	//---------------------------------------//

	if( !unitRecno )
	{
		//--- if we have plenty of cash and can use cash to decrease the resistance of the independent villagers ---//

		if( should_use_cash_to_capture() )
		{
			char resultFlag;		// just get any military unit

			Unit* militaryUnit = ai_find_unit(false, captureTown->race_id, captureTown->center_x, captureTown->center_y, resultFlag);	// false-not civilian, what we need is a military unit

			if( militaryUnit )
				unitRecno = militaryUnit->sprite_recno;
		}

		if( !unitRecno )
			return 0;
	}

	//------- locate a place to build the camp -------//

	short buildXLoc, buildYLoc;

	if( !find_best_place_loc(FIRM_CAMP, captureTown->loc_x1, captureTown->loc_y1, buildXLoc, buildYLoc) )
	{
		captureTown->no_neighbor_space = 1;
		return 0;
	}

	//--- if the picked unit is an overseer of an existng camp ---//

	if( !mobilize_capturer(unitRecno) )
		return 0;

	//---------- add the action to the queue now ----------//

	int firmId;		// the AI may build a fort or a camp to capture the town

	if( m.random(4)==0 || ai_should_spend(30+pref_military_development/3) )
		firmId = FIRM_FORT;
	else
		firmId = FIRM_CAMP;

	int actionRecno = add_action( buildXLoc, buildYLoc, captureTown->loc_x1, captureTown->loc_y1,
							ACTION_AI_BUILD_FIRM, firmId, 1, unitRecno );

	if( actionRecno )
		process_action(actionRecno);

	return 1;
}
//---------- End of function Nation::start_capture ---------//


//-------- Begin of function Nation::find_best_capturer ------//
//
// Find an existing unit as the capturer of the town.
//
// <int>  townRecno - recno of the town to capture
// <int&> bestTargetResistance - a reference var for returning the target resistance if the returned unit is assigned as the overseer
//
// return: <int> the recno of the unit found.
//
int Nation::find_best_capturer(int townRecno, int& bestTargetResistance)
{
	#define MIN_CAPTURE_RESISTANCE_DEC 	20		// if we assign a unit as the commander, the minimum expected resistance decrease should be 20, otherwise we don't do it.

	Unit* unitPtr;
	Town* targetTown = town_array[townRecno];
	Firm* firmPtr;
	int   targetResistance;
	int   bestUnitRecno=0;

	if( targetTown->nation_recno==0 )
	{
		bestTargetResistance = (int) targetTown->resistance(nation_recno) - MIN_CAPTURE_RESISTANCE_DEC;
		bestTargetResistance = max(bestTargetResistance, SURRENDER_RESISTANCE);
	}
	else
	{
		bestTargetResistance = (int) targetTown->loyalty - MIN_CAPTURE_RESISTANCE_DEC;
		bestTargetResistance = max(bestTargetResistance, SURRENDER_LOYALTY);
	}

	//-----------------------------------------------------//

	for( int i=ai_general_count-1 ; i>=0 ; i-- )
	{
		unitPtr = unit_array[ ai_general_array[i] ];

		if( unitPtr->race_id != targetTown->race_id )
			continue;

		err_when( unitPtr->nation_recno != nation_recno );
		err_when( unitPtr->rank_id != RANK_KING && unitPtr->rank_id != RANK_GENERAL );

		if( unitPtr->nation_recno != nation_recno )
			continue;

		//---- if this unit is on a mission ----//

		if( unitPtr->home_camp_firm_recno )
			continue;

		//---- don't use the king to build camps next to capture enemy towns, only next to independent towns ----//

		if( unitPtr->rank_id == RANK_KING && targetTown->nation_recno )
			continue;

		//----- if this unit is in a camp -------//

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
		{
			firmPtr = firm_array[unitPtr->unit_mode_para];

			//--- check if the unit currently in a command base trying to take over an independent town ---//

			for( int j=firmPtr->linked_town_count-1 ; j>=0 ; j-- )
			{
				Town* townPtr = town_array[ firmPtr->linked_town_array[j] ];

				//--- if the unit is trying to capture an independent town and he is still influencing the town to decrease resistance ---//

				if( townPtr->nation_recno==0 &&
					 townPtr->target_resistance(nation_recno) <
					 townPtr->resistance(nation_recno) )
				{
					break;	// then don't use this unit
				}
			}

			if( j>=0 )		// if so, don't use this unit
				continue;
		}

		//--- if this unit is idle and the region ids are matched ---//

		if( unitPtr->cur_order.mode != UNIT_STOP ||
			 unitPtr->region_id() != targetTown->region_id )
		{
			continue;
		}

		//------- get the unit's influence index --------//

		err_when( unitPtr->is_civilian() );

		targetResistance = 100-targetTown->camp_influence(unitPtr); 	// influence of this unit if he is assigned as a commander of a military camp, false-get the actual influence or the one with possible fake spy skill influence

		//-- see if this unit's rating is higher than the current best --//

		if( targetResistance < bestTargetResistance )
		{
			bestTargetResistance = targetResistance;
			bestUnitRecno = unitPtr->sprite_recno;
		}
	}

	return bestUnitRecno;
}
//-------- End of function Nation::find_best_capturer -------//


//-------- Begin of function Nation::mobilize_capturer ------//
//
// Mobilize the capturer unit if he isn't mobilized yet.
//
int Nation::mobilize_capturer(int unitRecno)
{
	//--- if the picked unit is an overseer of an existng camp ---//

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
	{
		FirmCamp* firmCamp = firm_array[unitPtr->unit_mode_para]->cast_to_FirmCamp();

		err_when( !firmCamp );

		firmCamp->mobilize_overseer();
	}

	return 1;
}
//-------- End of function Nation::mobilize_capturer -------//


//-------- Begin of function Nation::hire_best_capturer ------//
//
// Hire the best unit you can find in one of the existing inns.
//
// <int>  townRecno	- recno of the town to capture
// <int> hireNow		- whether we should hire the unit now
//
// return: <int> if hireNow is true , the recno of the unit hired.
//					  if hireNow is false, the target resistance of the town
//						  if the hired unit is assigned as the overseer.
//
int Nation::hire_best_capturer(int townRecno, int hireNow)
{
	if( !ai_should_hire_unit(30) )		// 30 - importance rating
		return 0;

	FirmInn	*firmInn;
	Firm		*firmPtr;
	InnUnit *innUnit;
	int		i, j, innUnitCount, curRating;
	int		bestRating=0, bestInnRecno=0, bestInnUnitId=0;
	Town* 	townPtr = town_array[townRecno];
	int		destRegionId = world.get_region_id(townPtr->loc_x1, townPtr->loc_y1);

	for(i=0; i<ai_inn_count; i++)
	{
		firmPtr = (FirmInn*) firm_array[ai_inn_array[i]];

		err_when( firmPtr->firm_id != FIRM_INN );

		if( firmPtr->region_id != destRegionId )
			continue;

		firmInn = firmPtr->cast_to_FirmInn();

		innUnitCount=firmInn->inn_unit_count;

		if( !innUnitCount )
			continue;

		innUnit = firmInn->inn_unit_array + innUnitCount - 1;

		//------- check units in the inn ---------//

		for(j=innUnitCount; j>0; j--, innUnit--)
		{
			if( unit_res[innUnit->unit_id]->race_id == townPtr->race_id &&
				 !unit_res[innUnit->unit_id]->is_civilian &&
				 innUnit->spy_skill==0 && 
				 cash >= innUnit->hire_cost )
			{
				//----------------------------------------------//
				// evalute a unit on:
				// -its race, whether it's the same as the nation's race
				// -the inn's distance from the destination
				// -the skill level of the unit.
				//----------------------------------------------//

				curRating = (int) innUnit->skill_level();

				if( curRating > CITIZEN_SKILL_LEVEL && curRating > bestRating )
				{
					bestRating = curRating;

					bestInnRecno  = firmInn->firm_recno;
					bestInnUnitId = j;
				}
			}
		}
	}

	if( !bestInnUnitId )
		return 0;

	//----------------------------------------------------//

	firmInn = firm_array[bestInnRecno]->cast_to_FirmInn();

	if( hireNow )
	{
		int unitRecno = firmInn->hire(bestInnUnitId);

		if( !unitRecno )
			return 0;

		unit_array[unitRecno]->set_rank(RANK_GENERAL);

		return unitRecno;
	}
	else
	{
		return 100-townPtr->camp_influence( firmInn->inn_unit_array+bestInnUnitId-1, nation_recno );
	}
}
//-------- End of function Nation::hire_best_capturer -------//


//------ Begin of function sort_capture_town_function ------//
//
static int sort_capture_town_function( const void *a, const void *b )
{
	return ((CaptureTown*)a)->min_resistance - ((CaptureTown*)b)->min_resistance;
}
//------- End of function sort_capture_town_function ------//


