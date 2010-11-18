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

//Filename    : OF_SPECA.CPP
//Description : Firm special unit training structure - AI functions

#include <onation.h>
#include <oinfo.h>
#include <otown.h>
#include <oun_grp.h>
#include <of_camp.h>
#include <of_spec.h>

//--------- Begin of function FirmSpecial::process_ai ---------//

void FirmSpecial::process_ai()
{
	//------ think about training new units -------//

	if( info.game_date%15==firm_recno%15 )
		think_train_new();

	//---- think about assigning trained units to forts ----//

	if( info.game_date%15==(firm_recno+5)%15 )
		think_transfer_unit();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmSpecial::process_ai -----------//


//------- Begin of function FirmSpecial::think_del -----------//
//
// Think about deleting this firm.
//
int FirmSpecial::think_del()
{
	if( linked_town_count==0 )
	{
		ai_del_firm();
		return 1;
	}

	return 0;
}
//--------- End of function FirmSpecial::think_del -----------//


//----- Begin of function FirmSpecial::think_train_new ------//
//
// Think about which weapon to produce.
//
void FirmSpecial::think_train_new()
{
	Nation* nationPtr = nation_array[nation_recno];

	if( !nationPtr->ai_should_spend(50) || nationPtr->cash <= 0 )
		return;

	//----- determine which town to train the special unit from ---//

	int maxJoblessPop=0, bestTownRecno=0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_enable_array[i] != LINK_EE )
			continue;

		Town* townPtr = town_array[ linked_town_array[i] ];

		if( townPtr->jobless_population > maxJoblessPop )
		{
			maxJoblessPop = townPtr->jobless_population;
			bestTownRecno = townPtr->town_recno;
		}
	}

	if( !bestTownRecno )
		return;

	//--------- train the special unit now ------------//

	recruit_trainee(bestTownRecno, TRAIN_UNIT_SPECIAL, COMMAND_AI);
}
//------ End of function FirmSpecial::think_train_new -------//


//------- Begin of function FirmSpecial::think_transfer_unit -------//
//
// Assign trained units to camps.
//
int FirmSpecial::think_transfer_unit()
{
	//--- if there is no trained units currently -----//

	if( trainee_count==0 || trainee_array[0].is_under_training )
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
		if( trainee_array[i].is_under_training )
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
//--------- End of function FirmSpecial::think_transfer_unit -------//

