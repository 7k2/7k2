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

//Filename    : OF_TRAIA.CPP
//Description : Firm Training Buildings - AI functions

#include <OF_TRAIN.H>


//--------- Begin of function FirmTrain::total_combat_level ---------//
//
// Total combat level of all soldiers and commander in the base.
// The leadership of the general also applies to the final combat level.
//
// return: <int> the total combat level - it is the sum of hit points
//					  of all the units in the camp.
//
int FirmTrain::total_combat_level()
{
	int 	   totalCombatLevel=0;
	Trainee* traineePtr = trainee_array;

	for( int i=0 ; i<trainee_count ; i++, traineePtr++ )
	{
		totalCombatLevel += traineePtr->max_hit_points();			// use it points instead of combat level because hit_points represent both combat level and hit points left

		err_when( totalCombatLevel < 0 );
	}

	return totalCombatLevel;
}
//----------- End of function FirmTrain::total_combat_level ----------//


// -------- Begin of function FirmTrain::total_military_combat_level ----//
//
// different from total_combat_level, only returns infantriess'/special units'
//
int FirmTrain::total_military_combat_level()
{
	int 	   totalCombatLevel=0;
	Trainee* traineePtr = trainee_array;

	for( int i=0 ; i<trainee_count ; i++, traineePtr++ )
	{
		// count soldier only, ignore civilian spy
		if( !traineePtr->is_under_training 
			&& traineePtr->train_combat_potential > 0 )
		{
			totalCombatLevel += traineePtr->max_hit_points();			// use it points instead of combat level because hit_points represent both combat level and hit points left

			err_when( totalCombatLevel < 0 );
		}
	}

	return totalCombatLevel;
}
// -------- end of function FirmTrain::total_military_combat_level ----//


// -------- Begin of function FirmTrain::total_military_count ----//
//
// different from trainee_count, only returns infantriess'/special units'
//
int FirmTrain::total_military_count()
{
	int 	   soldierCount=0;
	Trainee* traineePtr = trainee_array;

	for( int i=0 ; i<trainee_count ; i++, traineePtr++ )
	{
		// count soldier only, ignore civilian spy
		if( !traineePtr->is_under_training 
			&& traineePtr->train_combat_potential > 0 )
		{
			++soldierCount;
		}
	}

	return soldierCount;
}
// -------- End of function FirmTrain::total_military_count ----//
