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

//Filename    : OF_MINE2.CPP
//Description : Firm Mine - AI functions

#include <OINFO.H>
#include <OUNIT.H>
#include <ORACERES.H>
#include <OGAME.H>
#include <OWORLD.H>
#include <ONATION.H>
#include <OSITE.H>
#include <OF_MINE.H>
#include <OF_FACT.H>
#include <OF_MARK.H>


//------- Begin of function FirmMine::process_ai -----------//
//
void FirmMine::process_ai()
{
	//---- if the reserve has exhaust ----//

	if( !raw_id || reserve_qty==0 )
	{
		err_when( under_construction );

		ai_del_firm();
		return;
	}

	//---- think about building factory and market place to link to ----//

	if( productivity > 0 )		// only do so if productivity > 0
	{
		int rc = info.game_date%30==firm_recno%30;

		if( nation_array[nation_recno]->ai_factory_count==0 )		// if the nation doesn't have any factories yet, give building factory a higher priority
			rc = info.game_date%5==firm_recno%5;

		if( rc )
		{
			if( !think_build_factory(raw_id) )
				ai_should_build_factory_count = 0;		// reset the counter

			think_build_market();		// don't build it in FirmMine, when it builds a factory the factory will build a mine.
		}
	}

	//---- call parent class process_ai() ------//

	FirmWork::process_ai();
}
//--------- End of function FirmMine::process_ai -----------//


//------- Begin of function FirmMine::think_build_market -----------//
//
int FirmMine::think_build_market()
{
	if( no_neighbor_space )		// if there is no space in the neighbor area for building a new firm.
		return 0;

	Nation* nationPtr = nation_array[nation_recno];

	//-- only build a new market when the mine has a larger supply than the demands from factories and market places

	if( stock_qty < max_stock_qty * 0.2 )
		return 0;

	//--- check whether the AI can build a new firm next this firm ---//

	if( !nationPtr->can_ai_build(FIRM_MARKET) )
		return 0;

	//-- only build one market place next to this mine, check if there is any existing one --//

	FirmMarket* firmPtr;

	for(int i=0; i<linked_firm_count; i++)
	{
		err_when(!linked_firm_array[i] || firm_array.is_deleted(linked_firm_array[i]));

		firmPtr = (FirmMarket*) firm_array[linked_firm_array[i]];

		if(firmPtr->firm_id!=FIRM_MARKET)
			continue;

		//------ if this market is our own one ------//

		if( firmPtr->nation_recno == nation_recno )  // if it already has a raw material market, then no need to build a new one
			return 0;
	}

	//------ queue building a new market -------//

	short buildXLoc, buildYLoc;

	if( !nationPtr->find_best_place_loc(FIRM_MARKET, loc_x1, loc_y1, buildXLoc, buildYLoc) )
	{
		no_neighbor_space = 1;
		return 0;
	}

	nationPtr->add_action(buildXLoc, buildYLoc, loc_x1, loc_y1, ACTION_AI_BUILD_FIRM, FIRM_MARKET);

	return 1;
}
//--------- End of function FirmMine::think_build_market -----------//


//------- Begin of function FirmMine::think_adjust_workforcez -------//
//
// Think about adjusting the workforce by increasing or decreasing
// needed_worker_count.
//
int FirmMine::think_adjust_workforce()
{
	//------ think about decreasing work force -------//

	//-- the more full the stock is, the less worker we needed ----//

	int optimizedWorker = 3 + MAX_WORKER - MAX_WORKER * (int) stock_qty / (int) max_stock_qty;

	if( needed_worker_count > optimizedWorker )
	{
		int decCount = (needed_worker_count-optimizedWorker)/3;

		set_needed_worker_count( needed_worker_count - max(1, decCount), COMMAND_AI );
		return 1;
	}

	//------- think about increasing work force -------//

	if( stock_qty > max_stock_qty * 0.1 && production_30days() > 30 )
		return 0;

	if( reserve_qty < MAX_RAW_RESERVE_QTY/10 )
		return 0;

	if( needed_worker_count < MAX_WORKER )
	{
		if( stock_qty > max_stock_qty * 0.05 )
			set_needed_worker_count( needed_worker_count+1, COMMAND_AI );
		else
			set_needed_worker_count( needed_worker_count+2, COMMAND_AI );

		return 1;
	}

	return 0;
}
//--------- End of function FirmMine::think_adjust_workforce -------//


//------- Begin of function FirmMine::ai_has_excess_worker -------//
//
// Return whether this firm has any excessive workers or not.
//
int FirmMine::ai_has_excess_worker()
{
	//--- if the actual production is lower than the productivity, than the firm must be under-capacity.

	if( worker_count > 4 )		// at least keep 4 workers
	{
		return stock_qty > max_stock_qty * (float) 0.9 &&
				 production_30days() < productivity*25; 		// take 25 days instead of 30 days so there will be small chance of errors.
	}
	else if( worker_count > 1 )
	{
		return reserve_qty < worker_count*200;
	}
	else           	// don't leave if there is only one miner there
	{
		return 0;
	}
}
//--------- End of function FirmMine::ai_has_excess_worker -------//


//---- Begin of function FirmMine::builder_stay_after_construction ----//
//
// This function is called by Firm::process_construction()
// to assign the builder into the firm after the construction
// of the firm is complete.
//
// return: <int> 1 - the builder has been successfully assigned
//							into the firm.
//					  0 - failed.
//
int FirmMine::builder_stay_after_construction()
{
	//---------- only for AI ----------//

	if( !is_ai )
		return 0;

	//--- if there is no linked town, use the builder to build one ---//

	if( linked_town_count==0 )
	{
		int builderRecno = builder_recno;

		set_builder(0);   	// mobilize the builder and reset builder_recno

		ai_build_neighbor_town(builderRecno);		// order the unit to build town

		return 1;
	}

	return 0;
}
//----- End of function FirmMine::builder_stay_after_construction -----//

