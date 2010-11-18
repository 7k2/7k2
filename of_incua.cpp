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

//Filename    : OF_INCUA.CPP
//Description : Firm Incubator

#include <ONATION.H>
#include <OINFO.H>
#include <OF_INCU.H>

//--------- Begin of function FirmIncubator::process_ai ---------//

void FirmIncubator::process_ai()
{
	//---- think about which technology to research ----//

	think_new_production();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmIncubator::process_ai -----------//


//------- Begin of function FirmIncubator::think_del -----------//
//
// Think about deleting this firm.
//
int FirmIncubator::think_del()
{
	return 0;
}
//--------- End of function FirmIncubator::think_del -----------//


//----- Begin of function FirmIncubator::think_new_production ------//
//
// Think about which weapon to produce.
//
void FirmIncubator::think_new_production()
{
	//--- if there isn't enough camp space, don't hire new animals ---//

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->total_camp_unit_space(1) < 5 - 4 * nationPtr->pref_military_development / 100 )
		return;

	//------ if we should spend the money ------//

	if( !nationPtr->ai_should_spend( 65 + nationPtr->pref_use_weapon/4 ) )
		return;

	//---- calculate the average instance count of all available weapons ---//

	int 		 weaponTypeCount=0, totalWeaponCount=0;
	UnitInfo* unitInfo;

	for( int i=0; i<MAX_INCUBATE_TYPE ; i++ )
	{
		unitInfo = unit_res[ incubate_unit_id[i] ];

		if( unitInfo->get_nation_tech_level(nation_recno) == 0 )
			continue;

		weaponTypeCount++;
		totalWeaponCount += unitInfo->nation_unit_count_array[nation_recno-1];
	}

	if( weaponTypeCount==0 )		// none of weapon technologies is available
		return;

	int averageWeaponCount = totalWeaponCount/weaponTypeCount;

	//----- think about which is best to build now ------//

	int curRating, bestRating=0, bestUnitId=0;

	for( i=0; i<MAX_INCUBATE_TYPE ; i++ )
	{
		unitInfo = unit_res[ incubate_unit_id[i] ];

		int techLevel = unitInfo->get_nation_tech_level(nation_recno);

		if( techLevel==0 )
			continue;

		int unitCount = unitInfo->nation_unit_count_array[nation_recno-1];

		curRating = averageWeaponCount-unitCount + techLevel*3;

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestUnitId = unitInfo->unit_id;
		}
	}

	//------------------------------------//

	if( bestUnitId )
		add_queue( bestUnitId );
}
//------ End of function FirmIncubator::think_new_production -------//
