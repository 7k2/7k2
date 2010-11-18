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

//Filename    : OF_RESE2.CPP
//Description : Firm Research Center - AI functions

#include <onation.h>
#include <oinfo.h>
#include <otown.h>
#include <ounit.h>
#include <otech.h>
#include <otechres.h>
#include <of_rese.h>

//--------- Begin of function FirmResearch::process_ai ---------//

void FirmResearch::process_ai()
{
	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmResearch::process_ai -----------//


//------- Begin of function FirmResearch::think_del -----------//
//
// Think about deleting this firm.
//
int FirmResearch::think_del()
{
	//----- if all technologies have been researched -----//

	// ##### begin Gilbert 24/12 ######//
	if( nation_array[nation_recno]->total_tech_level() >= tech_res.total_tech_level(nation_recno) )		// all technology have been researched
	// ##### end Gilbert 24/12 ######//
	{
		ai_del_firm();
		return 1;
	}

   //----------------------------------------------// 

	if( worker_count > 0 )
		return 0;

	//-- check whether the firm is linked to any towns or not --//

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_enable_array[i] == LINK_EE )
			return 0;
	}

	//------------------------------------------------//

	ai_del_firm();

	return 1;
}
//--------- End of function FirmResearch::think_del -----------//


//------ Begin of function FirmResearch::think_adjust_workforce -----//
//
// Think about adjusting the workforce by increasing or decreasing
// needed_worker_count.
//
int FirmResearch::think_adjust_workforce()
{
	Nation* nationPtr = nation_array[nation_recno];

	//----- determine if we should increase or decrease research workers ---//

	int decreaseWorker = 0;

	if( 100 * nationPtr->total_firm_worker(firm_id) / nationPtr->total_population
		 > 10 + nationPtr->pref_research/3 )		// 10% to 43% of the population
	{
		decreaseWorker = 1;
	}

	if( !nationPtr->ai_has_enough_food() )
	{
		decreaseWorker = 1;
	}

	//----------------------------------------//

	if( !decreaseWorker )
	{
		if( needed_worker_count < MAX_WORKER )
		{
			set_needed_worker_count( needed_worker_count+1, COMMAND_AI );
			return 1;
		}
	}

	//---- if we are running out of food -----//

	else
	{
		if( needed_worker_count > MAX_WORKER/2 )
		{
			set_needed_worker_count( needed_worker_count-1, COMMAND_AI );
			return 1;
		}
	}

	return 0;
}
//------- End of function FirmResearch::think_adjust_workforce -----//




