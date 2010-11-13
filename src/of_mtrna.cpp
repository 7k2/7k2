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

//Filename    : OF_MTRNA.CPP
//Description : Firm special unit training structure - AI functions

#include <ONATION.H>
#include <OINFO.H>
#include <OUN_GRP.H>
#include <OF_CAMP.H>
#include <OF_MTRN.H>
 
//--------- Begin of function FirmMonsterTrain::process_ai ---------//

void FirmMonsterTrain::process_ai()
{
	//---- think about training new units ------//

	if( info.game_date%7==firm_recno%7 )
		think_train_new();

	//----- think about transfering units ------//

	if( info.game_date%15==firm_recno%15 )
		think_transfer_unit();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmMonsterTrain::process_ai -----------//


//------- Begin of function FirmMonsterTrain::think_del -----------//
//
// Think about deleting this firm.
//
int FirmMonsterTrain::think_del()
{
	return 0;
}
//--------- End of function FirmMonsterTrain::think_del -----------//


//----- Begin of function FirmMonsterTrain::think_train_new ------//
//
// Think about which weapon to produce.
//
void FirmMonsterTrain::think_train_new()
{
	// BUGHERE : modified temporary to train only the first unit
	err_when( train_type_count == 0 || !train_unit_id[0] );

	int unitId = train_unit_id[0];

	if( !can_recruit(unitId) )
		return;

	Nation* nationPtr  = nation_array[nation_recno];
	UnitInfo* unitInfo = unit_res[unitId];

	// ####### begin Gilbert 23/2 #########//
	// avoid 0/0 case
	int unitPercent = unitInfo->nation_all_unit_count(nation_recno) == 0 ? 0 :
		100 * unitInfo->nation_all_unit_count(nation_recno) / nationPtr->total_unit_count;
	// ####### end Gilbert 23/2 #########//

	if( !nationPtr->ai_should_spend(80-unitPercent, unitInfo->build_cost) 	// , unitInfo->build_live_points_cost)		// do not pass live point cost as the cost is low enough to be ingored.
		 || nationPtr->cash <= 0 )
	{
		return;
	}

	recruit_trainee(unitId, COMMAND_AI);
}
//------ End of function FirmMonsterTrain::think_train_new -------//


//------- Begin of function FirmMonsterTrain::think_transfer_unit -------//
//
// Assign trained units to camps.
//
int FirmMonsterTrain::think_transfer_unit()
{
	//--- if there is no trained units currently -----//

	if( trainee_count==0 || trainee_array[0].is_under_training() )
		return 0;

	//---- find the closest firm to assign units to ----//

	Nation* nationPtr = nation_array[nation_recno];

	int 		curRating, bestRating=0;
	FirmCamp *firmCamp, *bestCamp=NULL;

	for( int j=0 ; j<nationPtr->ai_camp_count ; j++ )
	{
		firmCamp = firm_array[ nationPtr->ai_camp_array[j] ]->cast_to_FirmCamp();

		if( !firmCamp->is_soldier_full() )
		{
			curRating = world.distance_rating(center_x, center_y,
							firmCamp->center_x, firmCamp->center_y);

			if( curRating > bestRating )
			{
				bestRating = curRating;
				bestCamp   = firmCamp;
			}
		}
	}

	if( !bestCamp )
		return 0;

	//------- assign units to the firm --------//

	int transferCount = MAX_SOLDIER - bestCamp->soldier_count;

	unit_group.zap();

	for( int i=0 ; i<trainee_count && transferCount>0 ; i++ )
	{
		if( trainee_array[i].is_under_training() )
			continue;

		int unitRecno = mobilize_trainee(i+1, COMMAND_AI);

		if( !unitRecno )
			break;

		unit_group.add_unit(unitRecno);
		transferCount--;
	}

	if( unit_group.size() > 0 )
	{
		unit_group.assign(bestCamp->loc_x1, bestCamp->loc_y1, UNIT_LAND, COMMAND_AI);
		return 1;
	}

	return 0;
}
//--------- End of function FirmMonsterTrain::think_transfer_unit -------//

