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

//Filename    : OF_ALCHA.CPP
//Description : Firm Monster Alchemy - AI functions

#include <ONATION.H>
#include <OINFO.H>
#include <OTOWN.H>
#include <OUNIT.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OF_ALCH.H>

//--------- Begin of function FirmMonsterAlchemy::process_ai ---------//

void FirmMonsterAlchemy::process_ai()
{
	//------ change conversion -------//

	think_change_conversion();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmMonsterAlchemy::process_ai -----------//


//------- Begin of function FirmMonsterAlchemy::think_del -----------//
//
// Think about deleting this firm.
//
int FirmMonsterAlchemy::think_del()
{
	return 0;
}
//--------- End of function FirmMonsterAlchemy::think_del -----------//


//------- Begin of function FirmMonsterAlchemy::think_change_conversion -----------//
//
void FirmMonsterAlchemy::think_change_conversion()
{
	Nation* nationPtr = nation_array[nation_recno];

	//---- if this building is built on a raw material site ----//

	if( reserve_qty )
	{
		if( tech_res[TECH_RAW_TO_LIVE]->get_nation_tech_level(nation_recno) )
		{
			if( nationPtr->cash > nationPtr->live_points + 500 + nationPtr->pref_cash_reserve*2 )
				change_conversion(TECH_RAW_TO_LIVE, COMMAND_AI);
		}

		if( tech_res[TECH_RAW_TO_GOLD]->get_nation_tech_level(nation_recno) )
		{
			if( nationPtr->live_points > nationPtr->cash + 500 + nationPtr->pref_live_points_reserve*2 )
				change_conversion(TECH_RAW_TO_GOLD, COMMAND_AI);
		}
	}
	else //--- this building is NOT built on a raw material site ---//
	{
		if( nationPtr->live_points < nationPtr->pref_live_points_reserve*5 &&
			 nationPtr->cash > 1500 + nationPtr->pref_cash_reserve*10 )
		{
			if( tech_res[TECH_GOLD_TO_LIVE]->get_nation_tech_level(nation_recno) )
				change_conversion(TECH_GOLD_TO_LIVE, COMMAND_AI);
		}

		else if( nationPtr->cash < nationPtr->pref_cash_reserve*5 &&
			 nationPtr->live_points > 2500 + nationPtr->pref_live_points_reserve*10 )
		{
			if( tech_res[TECH_LIVE_TO_GOLD]->get_nation_tech_level(nation_recno) )
				change_conversion(TECH_GOLD_TO_LIVE, COMMAND_AI);
		}

		else
		{
			change_conversion(0, COMMAND_AI);
		}
	}
}
//--------- End of function FirmMonsterAlchemy::think_change_conversion -----------//

