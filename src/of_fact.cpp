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

//Filename    : OF_FACT.CPP
//Description : Firm Factory

#include <oinfo.h>
#include <omodeid.h>
#include <ostr.h>
#include <ounit.h>
#include <onation.h>
#include <orawres.h>
#include <oraceres.h>
#include <otechres.h>
#include <otech.h>
#include <of_mine.h>
#include <of_mark.h>
#include <oremote.h>
#include <ose.h>
#include <of_fact.h>

//-------- define constant ---------//

#define DEFAULT_FACTORY_MAX_STOCK_QTY 		500
#define DEFAULT_FACTORY_MAX_RAW_STOCK_QTY 500

//--------- Begin of function FirmFactory::FirmFactory ---------//
//
FirmFactory::FirmFactory()
{
	product_raw_id = 1;
	
	cur_month_production = (float) 0;
	last_month_production = (float) 0;

	stock_qty	   = (float) 0;
	max_stock_qty  = (float) DEFAULT_FACTORY_MAX_STOCK_QTY;

	raw_stock_qty		= (float) 0;
	max_raw_stock_qty = (float) DEFAULT_FACTORY_MAX_RAW_STOCK_QTY;

	next_output_link_id	  = 0;
	next_output_firm_recno = 0;

	memset( backup_raw_array, 0, sizeof(backup_raw_array) );
	memset( backup_product_array, 0, sizeof(backup_product_array) );

	auto_switch_flag = 0;
}
//----------- End of function FirmFactory::FirmFactory -----------//


//--------- Begin of function FirmFactory::~FirmFactory ---------//
//
FirmFactory::~FirmFactory()
{
}
//----------- End of function FirmFactory::~FirmFactory -----------//


//--------- Begin of function FirmFactory::init_derived ---------//
//
void FirmFactory::init_derived()
{
	auto_set_product();
}
//----------- End of function FirmFactory::init_derived -----------//


//------- Begin of function FirmFactory::auto_set_product --------//
//
void FirmFactory::auto_set_product()
{
	//---- automatically set the factory product type -----//

	int 			i, j, k, rawId, firmDistance;
	Firm* 		firmPtr, *otherFirm;
	FirmMarket* firmMarket;
	int 			minDistance=0x7FFF;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		firmDistance = m.points_distance( firmPtr->center_x, firmPtr->center_y,
													 center_x, center_y );

		//----------- if the firm is a mine ----------//

		if( firmPtr->firm_id == FIRM_MINE )
		{
			rawId = ((FirmMine*)firmPtr)->raw_id;

			if( !rawId )
				continue;

			//--- if this mine hasn't been used by any factories yet, then select it ---//

			for( j=firmPtr->linked_firm_count-1 ; j>=0 ; j-- )
			{
				if( !firmPtr->linked_firm_enable_array[j] )
					continue;

				otherFirm = firm_array[ firmPtr->linked_firm_array[j] ];

				if( otherFirm->firm_id == FIRM_FACTORY &&
					 ((FirmFactory*)otherFirm)->product_raw_id == rawId )
				{
					break;
				}

			}

			if( j<0 )
			{
				product_raw_id = rawId;
				return;
			}

			//--------------------------------//

			if( firmDistance < minDistance )
			{
				product_raw_id = ((FirmMine*)firmPtr)->raw_id;
				minDistance    = firmDistance;
			}
		}

		//----------- if the firm is a market place ----------//

		else if( firmPtr->firm_id == FIRM_MARKET )
		{
			firmMarket = (FirmMarket*) firmPtr;

			for( j=0 ; j<MAX_MARKET_GOODS ; j++ )
			{
				rawId = firmMarket->market_goods_array[j].raw_id;

				if( !rawId )
					continue;

				//--- if this raw material in this market hasn't been used by any factories yet, then select it ---//

				for( k=firmPtr->linked_firm_count-1 ; k>=0 ; k-- )
				{
					if( firmPtr->linked_firm_enable_array[k] != LINK_EE )
						continue;

					otherFirm = firm_array[ firmPtr->linked_firm_array[k] ];

					if( otherFirm->firm_id == FIRM_FACTORY &&
						 ((FirmFactory*)otherFirm)->product_raw_id == rawId )
					{
						break;
					}
				}

				if( k<0 )
				{
					product_raw_id = rawId;
					return;
				}

				//-----------------------------------//

				if( firmDistance < minDistance )
				{
					product_raw_id = rawId;
					minDistance    = firmDistance;
				}
			}
		}
	}
}
//------- End of function FirmFactory::auto_set_product -------//


//--------- Begin of function FirmFactory::next_day ---------//
//
void FirmFactory::next_day()
{
	//----- call next_day() of the base class -----//

	FirmWork::next_day();

	//--------- daily manufacturing activities ---------//

	if( info.game_date%PROCESS_GOODS_INTERVAL == firm_recno%PROCESS_GOODS_INTERVAL )
	{
		input_raw();
		if( raw_stock_qty == 0.0f && auto_switch_flag )
		{
//			auto_set_product();
//			input_raw();

			// keep original product_raw_id
			int oldProductRawId = product_raw_id;
			int loopCount = 0;
			do		// try each other product, until loop back to the original
			{
				change_production(COMMAND_AUTO);
				input_raw();
				err_when( ++loopCount > MAX_PRODUCT );
			} while( raw_stock_qty == 0 && product_raw_id != oldProductRawId );
		}
		production();
		set_next_output_firm();						// set next output firm
	}
}
//----------- End of function FirmFactory::next_day -----------//


//--------- Begin of function FirmFactory::next_month ---------//
//
void FirmFactory::next_month()
{
	last_month_production = cur_month_production;
	cur_month_production  = (float) 0;
}
//----------- End of function FirmFactory::next_month -----------//


//------ Begin of function FirmFactory::change_production -------//
//
void FirmFactory::change_production(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <product id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_FACTORY_CHG_PROD, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = product_raw_id >= MAX_PRODUCT ? 1 : product_raw_id + 1;
		return;
	}

	// update RemoteMsg::factory_change_product

	set_production( product_raw_id==MAX_PRODUCT ? 1 : product_raw_id+1 );
}
//--------- End of function FirmFactory::change_production --------//


//------ Begin of function FirmFactory::set_production -------//
//
void FirmFactory::set_production(int newProductId)
{
	if ( product_raw_id >= 1 && product_raw_id <= MAX_RAW)
	{
		backup_raw_array[ product_raw_id -1 ] = raw_stock_qty;
		backup_product_array[ product_raw_id -1 ] = stock_qty;
	}
	
	product_raw_id = newProductId;

	stock_qty 	   = (float) backup_product_array[ product_raw_id -1 ];
	max_stock_qty  = (float) DEFAULT_FACTORY_MAX_STOCK_QTY;
	raw_stock_qty		= (float) backup_raw_array[ product_raw_id -1 ];
	max_raw_stock_qty = (float) DEFAULT_FACTORY_MAX_RAW_STOCK_QTY;
}
//--------- End of function FirmFactory::set_production --------//


//--------- Begin of function FirmFactory::production ---------//
//
void FirmFactory::production()
{
	//----- if stock capacity reached or reserve exhausted -----//

	if( stock_qty == max_stock_qty )
		return;

	err_when( stock_qty > max_stock_qty );

	//------- generate revenue for the nation --------//

	float produceQty = (float) 20 * productivity / 100;

//	produceQty = MIN( produceQty, max_stock_qty-stock_qty );

	manufacture(produceQty);
}
//----------- End of function FirmFactory::production -----------//


//--------- Begin of function FirmFactory::input_raw ---------//
//
// Input raw materials from mines and market places.
//
void FirmFactory::input_raw()
{
	//------ scan for a firm to input raw materials --------//

	int   	 i, j;
	float		 inputQty;
	Firm* 	 firmPtr;
	FirmMine* firmMine;
	FirmMarket* firmMarket;
	Nation* 	 nationPtr = nation_array[nation_recno];
	MarketGoods* marketGoods;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		firmPtr = firm_array[linked_firm_array[i]];

		//----------- check if the firm is a mine ----------//

		if( firmPtr->firm_id != FIRM_MINE && firmPtr->firm_id != FIRM_MARKET )
			continue;

		//--------- if the firm is a mine ------------//

		if( firmPtr->firm_id == FIRM_MINE )
		{
			firmMine = (FirmMine*) firmPtr;

			if( firmMine->next_output_firm_recno == firm_recno &&
				 firmMine->raw_id==product_raw_id && firmMine->stock_qty > 0 )
			{
				inputQty = MIN( firmMine->stock_qty, max_raw_stock_qty - raw_stock_qty );

				if( firmMine->nation_recno != nation_recno )			// make sure it has the cash to pay for the raw materials
					inputQty = MIN( inputQty, nationPtr->cash/RAW_PRICE );

				if( inputQty > 0 )
				{
					firmMine->stock_qty  -= inputQty;
					raw_stock_qty			+= inputQty;

					err_when( raw_stock_qty > max_raw_stock_qty );

					//---- import from other nation -----//

					if( firmMine->nation_recno != nation_recno )
						nationPtr->import_goods(IMPORT_RAW, firmMine->nation_recno, inputQty*RAW_PRICE );
				}
			}
		}

		//------- if the firm is a market place --------//

		if( firmPtr->firm_id == FIRM_MARKET )
		{
			firmMarket = (FirmMarket*) firmPtr;

			if( firmMarket->next_output_firm_recno == firm_recno )
			{
				marketGoods = firmMarket->market_goods_array;

				for( j=0 ; j<MAX_MARKET_GOODS ; j++, marketGoods++ )
				{
					if( marketGoods->raw_id == product_raw_id &&
						 marketGoods->stock_qty > 0 )
					{
						inputQty = MIN( marketGoods->stock_qty, max_raw_stock_qty - raw_stock_qty );

						if( firmMarket->nation_recno != nation_recno )			// make sure it has the cash to pay for the raw materials
							inputQty = MIN( inputQty, nationPtr->cash/RAW_PRICE );

						if( inputQty > 0 )
						{
							marketGoods->stock_qty -= inputQty;
							raw_stock_qty			  += inputQty;

							err_when( raw_stock_qty > max_raw_stock_qty );

							//---- import from other nation -----//

							if( firmMarket->nation_recno != nation_recno )
								nationPtr->import_goods(IMPORT_RAW, firmMarket->nation_recno, inputQty*RAW_PRICE );
						}
					}
				}
			}
		}
	}
}
//----------- End of function FirmFactory::input_raw -----------//


//--------- Begin of function FirmFactory::manufacture ---------//
//
// Input raw materials into the factory
//
// <float> maxMftQty - maximum qty this firm can manufacture in this call.
//
void FirmFactory::manufacture(float maxMftQty)
{
	if( raw_stock_qty==0 )
		return;

	float	inputQty = maxMftQty;
	float	yieldRate = 1.0f;
	if( nation_recno )
		yieldRate += 0.25f * tech_res[TECH_MANUFACTURING]->get_nation_tech_level(nation_recno);
	float outputQty = inputQty * yieldRate;

	if( inputQty > raw_stock_qty )
		outputQty = (inputQty = raw_stock_qty) * yieldRate;

	if( outputQty > max_stock_qty-stock_qty )
		inputQty = (outputQty = max_stock_qty-stock_qty) / yieldRate;

	if( inputQty <= 0 )
		return;

	raw_stock_qty			-= inputQty;
	stock_qty			   += outputQty;
	cur_month_production += outputQty;
}
//----------- End of function FirmFactory::manufacture -----------//


//------- Begin of function FirmFactory::set_next_output_firm ------//
//
// Set next_output_firm_recno, the recno of the linked firm
// to which this factory is going to output products.
//
void FirmFactory::set_next_output_firm()
{
	int i, firmRecno, firmId;

	for( i=0 ; i<linked_firm_count ; i++ )		// max tries
	{
		if( ++next_output_link_id > linked_firm_count )    // next firm in the link
			next_output_link_id = 1;

		if( linked_firm_enable_array[next_output_link_id-1] == LINK_EE )
		{
			firmRecno = linked_firm_array[next_output_link_id-1];
			firmId 	 = firm_array[firmRecno]->firm_id;

			if( firmId==FIRM_MARKET )
			{
				next_output_firm_recno = firmRecno;
				return;
			}
		}
	}

	next_output_firm_recno = 0;		// this mine has no linked output firms
}
//-------- End of function FirmFactory::set_next_output_firm ---------//


