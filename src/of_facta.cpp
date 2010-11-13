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

//Filename    : OF_FACT2.CPP
//Description : Firm Factory - AI functions

#include <OINFO.H>
#include <OUNIT.H>
#include <OGAME.H>
#include <OFONT.H>
#include <ONATION.H>
#include <ORAWRES.H>
#include <ORACERES.H>
#include <OTOWNRES.H>
#include <OWORLD.H>
#include <OF_MINE.H>
#include <OF_MARK.H>
#include <OF_FACT.H>


//------- Begin of function FirmFactory::process_ai -----------//
//
void FirmFactory::process_ai()
{
	if( info.game_date%15==firm_recno%15 )
	{
		if( think_change_production() )
			return;
	}

	//---- think about building market place to link to ----//

	if( info.game_date%30==firm_recno%30 )
		think_build_market();

	//---- call parent class process_ai() ------//

	FirmWork::process_ai();
}
//--------- End of function FirmFactory::process_ai -----------//


//------- Begin of function FirmFactory::think_build_market -----------//
//
int FirmFactory::think_build_market()
{
	if( no_neighbor_space )		// if there is no space in the neighbor area for building a new firm.
		return 0;

	Nation* nationPtr = nation_array[nation_recno];

	//--- check whether the AI can build a new firm next this firm ---//

	if( !nationPtr->can_ai_build(FIRM_MARKET) )
		return 0;

	//----------------------------------------------------//
	// If there is already a firm queued for building with
	// a building location that is within the effective range
	// of the this firm.
	//----------------------------------------------------//

	if( nationPtr->is_build_action_exist(FIRM_MARKET, center_x, center_y) )
		return 0;

	//-- only build one market place next to this factory, check if there is any existing one --//

	FirmMarket* firmPtr;

	for(int i=0; i<linked_firm_count; i++)
	{
		err_when(!linked_firm_array[i] || firm_array.is_deleted(linked_firm_array[i]));

		firmPtr = (FirmMarket*) firm_array[linked_firm_array[i]];

		if( firmPtr->firm_id!=FIRM_MARKET )
			continue;

		//----- if this is a retail market of our own ------//

		if( firmPtr->nation_recno == nation_recno )
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
//--------- End of function FirmFactory::think_build_market -----------//


//------- Begin of function FirmFactory::think_change_production -------//
//
int FirmFactory::think_change_production()
{
	if( cur_month_production + last_month_production > 0 ||
		 raw_stock_qty > 0 )
	{
		return 0;
	}

	if( info.game_date < setup_date + 30 )		// only change production after the factory has been running for at least one month
		return 0;

	//-- only build one market place next to this factory, check if there is any existing one --//

	#define MIN_FACTORY_IMPORT_STOCK_QTY	20

	Firm* firmPtr;
	int	curRating, bestRating=0, bestProductId=0, bestIsOwn=0;

	for(int i=0; i<linked_firm_count; i++)
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->firm_id!=FIRM_MINE && firmPtr->firm_id!=FIRM_MARKET )
			continue;

		//--- if this link to this market is disabled, enable it now ---//

		if( linked_firm_enable_array[i] == LINK_DE )
			toggle_firm_link( i+1, 1,  COMMAND_AI );

		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		curRating=0;

		//-------- if this is a mine ------//

		if( firmPtr->firm_id == FIRM_MINE )
		{
			FirmMine* firmMine = (FirmMine*) firmPtr;

			if( firmMine->stock_qty >= MIN_FACTORY_IMPORT_STOCK_QTY )
			{
				curRating = (int) firmMine->stock_qty;

				if( curRating > bestRating )
				{
					if( firmPtr->nation_recno == nation_recno || !bestIsOwn )	// try to get raw materials from own firms first
					{
						bestRating 	  = curRating;
						bestProductId = firmMine->raw_id;
						bestIsOwn 	  = firmPtr->nation_recno == nation_recno;
					}
				}
			}
		}

		//-------- if this is a market ------//

		else if( firmPtr->firm_id == FIRM_MARKET )
		{
			FirmMarket* firmMarket = (FirmMarket*) firmPtr;
			MarketGoods* marketGoods = firmMarket->market_goods_array;

			for( int j=0 ; j<MAX_MARKET_GOODS ; j++, marketGoods++ )
			{
				if( marketGoods->raw_id &&
					 marketGoods->stock_qty >= MIN_FACTORY_IMPORT_STOCK_QTY )
				{
					curRating = (int) marketGoods->stock_qty;

					if( curRating > bestRating )
					{
						if( firmPtr->nation_recno == nation_recno || !bestIsOwn )	// try to get raw materials from own firms first
						{
							bestRating 	  = curRating;
							bestProductId = marketGoods->raw_id;
							bestIsOwn 	  = firmPtr->nation_recno == nation_recno;
						}
					}
				}
			}
		}
	}

	//------------------------------------//

	if( bestProductId )
	{
		set_production(bestProductId);
		return 1;
	}
	else
	{
		if( info.game_date > setup_date + 60 )
		{
			ai_del_firm();		// delete the firm if there is no raw materials available after it has been built for over 2 months
			return 1;
		}
	}

	return 0;
}
//--------- End of function FirmFactory::think_change_production -------//


//------- Begin of function FirmFactory::think_adjust_workforcez -------//
//
// Think about adjusting the workforce by increasing or decreasing
// needed_worker_count.
//
int FirmFactory::think_adjust_workforce()
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
//--------- End of function FirmFactory::think_adjust_workforce -------//


