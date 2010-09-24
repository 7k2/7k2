//Filename    : OF_WORKA.CPP
//Description : FirmWork - AI functions

#include <OF_WORK.H>
#include <ONATION.H>
#include <OTOWN.H>

//---------- Begin of function FirmWork::process_ai --------//
//
// Think about ways to increase available workforce if the firm
// is short of workers.
//
void FirmWork::process_ai()
{
	if( info.game_date%30 == firm_recno%30 )
	{
		think_adjust_workforce();
		think_inc_worker_supply();
	}
}
//---------- End of function FirmWork::process_ai --------//


//---------- Begin of function FirmWork::think_inc_worker_supply --------//
//
// Think about ways to increase the population of linked town so as
// to increase the supply of workforce if the firm is short of workers.
//
int FirmWork::think_inc_worker_supply()
{
	//--- only do so if the actual number of workers must smaller than the needed number ---//

	if( needed_worker_count - worker_count < 5 )
		return 0;

	//----- if there are linked towns that can supply labor already, don't bother settling a new town ------//

	int  availableWorkforce=0, linkedTownCount=0;
	Town *townPtr;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_enable_array[i] != LINK_EE )
			continue;

		townPtr = town_array[ linked_town_array[i] ];

		linkedTownCount++;
		availableWorkforce += townPtr->jobless_population;
	}

	//---- if there is no linked town, set up one ----//

	if( linkedTownCount==0 )
	{
		ai_build_neighbor_town();
		return 0;
	}

	//----- if there are linked towns that can supply labor already, don't bother settling a new town ------//

	if( availableWorkforce >= needed_worker_count - worker_count )		// if there are enough labor supply
		return 0;

	//----- find the best town to move people to -----//

	Town* bestToTown=NULL;
	int	curRating, bestRating=0;

	for( i=0 ; i<linked_town_count ; i++ )
	{
		if( townPtr->nation_recno != nation_recno )
			continue;

		townPtr = town_array[ linked_town_array[i] ];

		curRating = race_jobless_pop( townPtr->race_id, townPtr->town_recno );

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestToTown   = townPtr;
		}
	}

	if( !bestToTown )
		return 0;

	//------ find the best source town to move people from -----//

	int fromTownRecno = most_jobless_town(bestToTown->race_id, bestToTown->town_recno );

	if( !fromTownRecno )
		return 0;

	Town* fromTown = town_array[fromTownRecno];

	nation_array[nation_recno]->add_action( bestToTown->loc_x1, bestToTown->loc_y1,
		fromTown->loc_x1, fromTown->loc_y1, ACTION_AI_SETTLE_TOWN, 0,
		needed_worker_count - worker_count);

	return 1;
}
//---------- End of function FirmWork::think_inc_worker_supply --------//


//---------- Begin of function FirmWork::most_jobless_race --------//
//
// Return : <int> >0  the race that has most jobless population.
//						==0 if there is no jobless population.
//
int FirmWork::most_jobless_race()
{
	//--- calculate the total jobless population of all races ----//

	Town* 	townPtr;
	Nation* 	nationPtr = nation_array[nation_recno];
	int		joblessPopArray[MAX_RACE];

	memset( joblessPopArray, 0, sizeof(joblessPopArray) );

	for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
	{
		townPtr = town_array[nationPtr->ai_town_array[i]];

		err_when( townPtr->nation_recno != nation_recno );

		err_when( townPtr->race_id < 1 || townPtr->race_id > MAX_RACE );

		joblessPopArray[townPtr->race_id-1] += townPtr->jobless_population;
	}

	//----- return race with the most jobless population ----//

	int maxJoblessPop=0;
	int maxRaceId=0;

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( joblessPopArray[i] > maxJoblessPop )
		{
			maxJoblessPop = joblessPopArray[i];
			maxRaceId 	  = i+1;
		}
	}

	return maxRaceId;
}
//---------- End of function FirmWork::most_jobless_race --------//


//---------- Begin of function FirmWork::race_jobless_pop --------//
//
// Return the total jobless population of the specific race.
//
// <int> raceId            - the specific race
// <int> excludedTownRecno - the town to be excluded in the calculation
//
// Return : <int> the total jobless population of the specific race.
//
int FirmWork::race_jobless_pop(int raceId, int excludedTownRecno)
{
	//--- calculate the total jobless population of all races ----//

	Town* 	townPtr;
	Nation* 	nationPtr = nation_array[nation_recno];
	int		joblessPop=0;

	for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
	{
		townPtr = town_array[nationPtr->ai_town_array[i]];

		err_when( townPtr->nation_recno != nation_recno );

		if( townPtr->race_id != raceId )
			continue;

		if( townPtr->town_recno == excludedTownRecno )
			continue;

		joblessPop += townPtr->jobless_population;
	}

	return joblessPop;
}
//---------- End of function FirmWork::race_jobless_pop --------//


//---------- Begin of function FirmWork::most_jobless_town --------//
//
// Return the town with most jobless population of the specific race.
//
// <int> raceId            - the specific race
// <int> excludedTownRecno - the town to be excluded in the calculation
//
// Return : <int> the total jobless population of the specific race.
//
int FirmWork::most_jobless_town(int raceId, int excludedTownRecno)
{
	//--- calculate the total jobless population of all races ----//

	Town* 	townPtr;
	Nation* 	nationPtr = nation_array[nation_recno];
	int		maxJoblessPop=0;
	int		bestTownRecno=0;

	for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
	{
		townPtr = town_array[nationPtr->ai_town_array[i]];

		err_when( townPtr->nation_recno != nation_recno );

		if( townPtr->race_id != raceId )
			continue;

		if( townPtr->town_recno == excludedTownRecno )
			continue;

		if( townPtr->jobless_population > maxJoblessPop )
		{
			maxJoblessPop = townPtr->jobless_population;
			bestTownRecno = townPtr->town_recno;
		}
	}

	return bestTownRecno;
}
//---------- End of function FirmWork::most_jobless_town --------//


//------- Begin of function FirmWork::ai_build_neighbor_town -------//
//
// Build a town next to this firm.
//
// [int] builderUnitRecno - if given, use this unit to build the town.
//
int FirmWork::ai_build_neighbor_town(int builderUnitRecno)
{
	short buildXLoc, buildYLoc;
	Nation* nationPtr = nation_array[nation_recno];

	if( !nationPtr->find_best_place_loc(0, loc_x1, loc_y1, buildXLoc, buildYLoc) )		// 0-build a town
	{
		no_neighbor_space = 1;
		return 0;
	}

	int townRaceId = most_jobless_race();

	if( !townRaceId )		// there is no jobless population
		return 0;

	//--- check if a unit is already on its way to build firm for capturing this town ---//

	if( nationPtr->is_action_exist( -1, -1, loc_x1, loc_y1,
		 ACTION_AI_BUILD_TOWN, townRaceId ) )		
	{
		return 0;
	}

	//------------ build now -------------//

	nationPtr->add_action(buildXLoc, buildYLoc, loc_x1, loc_y1,
		ACTION_AI_BUILD_TOWN, townRaceId, 1, builderUnitRecno);

	return 1;
}
//-------- End of function FirmWork::ai_build_neighbor_town -------//


//--------- Begin of function FirmWork::is_worker_already_coming --------//
//
// Return if people are already moving from other towns to towns
// linked to this firm.
//
int FirmWork::is_worker_already_coming()
{
	int 			i, j;
	ActionNode* actionNode;
	Nation*		nationPtr = nation_array[nation_recno];

	for( i=nationPtr->action_count() ; i>0 ; i-- )
	{
		if( nationPtr->is_action_deleted(i) )
			continue;

		actionNode = nationPtr->get_action(i);

		if( actionNode->action_mode == ACTION_AI_SETTLE_TOWN )
		{
			//--- see if the destination is any of the linked towns ---//

			for( j=0 ; j<linked_town_count ; j++ )
			{
				Town* townPtr = town_array[ linked_town_array[j] ];

				if( townPtr->loc_x1 == actionNode->action_x_loc &&
					 townPtr->loc_y1 == actionNode->action_y_loc )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}
//---------- End of function FirmWork::is_worker_already_coming --------//

