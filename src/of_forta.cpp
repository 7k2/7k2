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

//Filename    : OF_FORTA.CPP
//Description : Firm Fort - AI functions

#include <onation.h>
#include <oinfo.h>
#include <oconfig.h>
#include <otown.h>
#include <ospy.h>
#include <ounit.h>
#include <of_fort.h>

//--------- Begin of function FirmFort::process_ai ---------//

void FirmFort::process_ai()
{
	think_train_archer();

	//------- process AI at FirmCamp level ------//

	FirmCamp::process_ai();
}
//--------- End of function FirmFort::process_ai ---------//


//--------- Begin of function FirmFort::think_train_archer ---------//

int FirmFort::think_train_archer()
{
	if( target_archer_count > current_archer_count )		// if one is already being trained 
		return 0;

	Nation* nationPtr = nation_array[nation_recno];

	int importanceRating = nationPtr->pref_military_development/3
								  + (ai_combat_level_needed() - total_combat_level()) / 10;

	if( !nationPtr->ai_should_spend( importanceRating, TRAIN_ARCHER_COST ) )
		return 0;
	
	if( target_archer_count == current_archer_count &&
		 target_archer_count < MAX_FORT_ARCHER )
	{
		target_archer_count++;
		return 1;
	}

	return 0;
}
//--------- End of function FirmFort::think_train_archer ---------//


//--------- Begin of function FirmFort::ai_recruit ---------//
//
// <int> recruitCount - the number of soldiers to be recruited.
//
// return: <int> 1-succeeded, 0-failed.
//
int FirmFort::ai_recruit(int recruitCount)
{
	err_when( recruitCount < 1 );

	if( soldier_count + patrol_unit_count >= MAX_SOLDIER || recruitCount==0 )
		return 0;

	//---- don't recruit further if there are more than one unit under training currently ---//

	int trainingCount=0;

	for( int i=0 ; i<soldier_count ; i++ )
	{
		if( soldier_array[i].is_under_training() )
			trainingCount++;
	}

	if( trainingCount >= 2 )
		return 0;

	//----- if the nation does not have enough food, recruit much less than it is needed ----//

	Nation* nationPtr = nation_array[nation_recno];

	if( !nationPtr->ai_has_enough_food() )
	{
		recruitCount = recruitCount * (100-nationPtr->not_enough_food_index()) / 100;

		if( recruitCount < 1 )
			return 0;
	}

	//-- just train 2 units at maximum at a time, to avoid holding up human resources --//

	recruitCount = min( recruitCount, MAX_SOLDIER-soldier_count-patrol_unit_count );
	recruitCount = min( 2, recruitCount );

	//------ first try to train soldiers in the fort -----//

	if( nationPtr->cash > 0 )		// training soldiers costs money
	{
		int	loopCount=0;
		Town* townPtr;

		//-- recruit majority race first, but will also consider other races --//

		for( int tryLoop=1; tryLoop<=2 ; tryLoop++ )
		{
			for( int i=0 ; i<linked_town_count ; i++ )
			{
				townPtr = town_array[ linked_town_array[i] ];

				if( townPtr->nation_recno != nation_recno )
					continue;

				//-- recruit majority race first, but will also consider other races --//

				if( tryLoop == 1 )
				{
					if( townPtr->race_id != race_id )
						continue;
				}

				if( townPtr->jobless_population==0 )
					continue;

				//--- if the loyalty is too low, reward before recruiting ---//

				if( townPtr->jobless_population > 0 &&
					 townPtr->loyalty < 40 )
				{
					if( townPtr->accumulated_reward_penalty > 30 )		// if the reward penalty is too high, do reward
						break;

					townPtr->reward(COMMAND_AI);
				}

				//---- recruit the soldiers we needed ----//

				while( townPtr->can_train() )
				{
					err_when( ++loopCount > 1000 );

					recruit_soldier(townPtr->town_recno, false, COMMAND_AI);			// last 1-force pulling people from the town to the firm

					if( --recruitCount == 0 )
						return 1;
				}
			}
		}
	}

	//------ next, try to recruit from remote villages -----//

	if( recruitCount > 0 )
		nation_array[nation_recno]->add_action(loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_SOLDIER, FIRM_CAMP, recruitCount);

	return 1;
}
//----------- End of function FirmFort::ai_recruit ----------//


