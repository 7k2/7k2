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

//Filename    : OF_ANIMA.CPP
//Description : Firm Animal Training firm.

#include <ONATION.H>
#include <OINFO.H>
#include <OF_ANIM.H>

//--------- Begin of function FirmAnimal::process_ai ---------//

void FirmAnimal::process_ai()
{
	//---- think about which technology to research ----//

	if( info.game_date%7==firm_recno%7 )
		think_hire_new();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmAnimal::process_ai -----------//


//------- Begin of function FirmAnimal::think_del -----------//
//
// Think about deleting this firm.
//
int FirmAnimal::think_del()
{
	return 0;
}
//--------- End of function FirmAnimal::think_del -----------//


//----- Begin of function FirmAnimal::think_hire_new ------//
//
// Think about which weapon to produce.
//
void FirmAnimal::think_hire_new()
{
	//--- if there isn't enough camp space, don't hire new animals ---//

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->total_camp_unit_space(1) < 5 - 4 * nationPtr->pref_military_development / 100 )
		return;

	//------ if we should spend the money ------//

	if( !nationPtr->ai_should_spend( 60 + nationPtr->pref_monster_special/3 ) )
		return;

	//------------------------------------//

	int importanceRating = 40 + nationPtr->pref_monster_special/4;

	for( int i=1 ; i<=animal_unit_count ; i++ )
	{
		if( !can_hire(i) )
			continue;

		if( nationPtr->ai_should_spend( importanceRating, animal_unit_array[i-1].hire_cost,
			 animal_unit_array[i-1].live_point_cost ) )
		{
			hire(i);
			return;
		}
	}
}
//------ End of function FirmAnimal::think_hire_new -------//
