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

//Filename    : OF_MARK.CPP
//Description : Firm Market Place

#include <oinfo.h>
#include <omodeid.h>
#include <ostr.h>
#include <oconfig.h>
#include <orawres.h>
#include <oraceres.h>
#include <ogodres.h>
#include <otown.h>
#include <onation.h>
#include <ou_cara.h>
#include <oworldmt.h>
#include <osys.h>
#include <of_fact.h>
#include <of_mine.h>
#include <of_mark.h>
#include <oremote.h>
#include <ose.h>


//--------- Begin of function FirmMarket::FirmMarket ---------//
//
FirmMarket::FirmMarket()
{
	max_stock_qty = (float) MAX_MARKET_STOCK;

	memset( market_goods_array	 , 0, sizeof(MarketGoods) * MAX_MARKET_GOODS );
//	memset( market_raw_array	 , 0, sizeof(market_raw_array) );
//	memset( market_product_array, 0, sizeof(market_product_array) );

	next_output_link_id	  = 0;
	next_output_firm_recno = 0;

	no_linked_town_since_date  = 0;
	last_import_new_goods_date = 0;
}
//----------- End of function FirmMarket::FirmMarket -----------//


//--------- Begin of function FirmMarket::~FirmMarket ---------//
//
FirmMarket::~FirmMarket()
{
}
//----------- End of function FirmMarket::~FirmMarket -----------//


//--------- Begin of function FirmMarket::init_derived ---------//
//
void FirmMarket::init_derived()
{
	// ----- initialize market_goods_array -------//

	int i, j;

#if( MAX_MARKET_GOODS >= MAX_RAW + MAX_PRODUCT )
	i = 0;
	for( j = 1; j <= MAX_PRODUCT; ++j )
	{
		market_goods_array[i].product_raw_id = j;
		market_goods_array[i].raw_id = 0;
		++i;
	}
	for( j = 1; j <= MAX_RAW; ++j )
	{
		market_goods_array[i].product_raw_id = 0;
		market_goods_array[i].raw_id = j;
		++i;
	}
#endif


	//------ redistribute town demand --------//

	town_array.distribute_demand();
}
//----------- End of function FirmMarket::init_derived -----------//


// ------ begin of function FirmMarket::market_raw_array -----//
MarketGoods* FirmMarket::market_raw_array(int rawId)
{
#if( MAX_MARKET_GOODS >= MAX_RAW + MAX_PRODUCT )
	if( market_goods_array[MAX_PRODUCT+rawId-1].raw_id == rawId )
		return market_goods_array+MAX_PRODUCT+rawId-1;
#endif

	for(int i = 0; i < MAX_MARKET_GOODS; ++i )
	{
		if( market_goods_array[i].raw_id == rawId )
			return market_goods_array+i;
	}

	return NULL;
}
// ------ end of function FirmMarket::market_raw_array -----//


// ------ begin of function FirmMarket::market_raw_array -----//
MarketGoods* FirmMarket::market_product_array(int productId)
{
#if( MAX_MARKET_GOODS >= MAX_RAW + MAX_PRODUCT )
	if( market_goods_array[productId-1].product_raw_id == productId )
		return market_goods_array+productId-1;
#endif

	for(int i = 0; i < MAX_MARKET_GOODS; ++i )
	{
		if( market_goods_array[i].product_raw_id == productId )
			return market_goods_array+i;
	}

	return NULL;
}
// ------ emd of function FirmMarket::market_raw_array -----//


//--------- Begin of function FirmMarket::next_day ---------//
//
void FirmMarket::next_day()
{
	//----- call next_day() of the base class -----//

	Firm::next_day();

	//---- update trade link to harbors to towns -----//

	update_trade_link();

	//-------- input goods ----------//

	if( info.game_date%PROCESS_GOODS_INTERVAL == firm_recno%PROCESS_GOODS_INTERVAL )
	{
		input_goods(50);								// input maximum 50 qty of goods per day
		set_next_output_firm();						// set next output firm
	}

	//-------- sell goods --------//

	sell_goods();

	//------- free up unused slots -------//

	//### begin alex 24/10 ###//
	//if( info.game_date%30 == firm_recno%30 )
	//	free_unused_slot();
	//#### end alex 24/10 ####//
}
//----------- End of function FirmMarket::next_day -----------//


//--------- Begin of function FirmMarket::next_month ---------//
//
void FirmMarket::next_month()
{
	Firm::next_month();

	//------ post goods supply data ------//

	MarketGoods* marketGoods = market_goods_array;

	for( int i=0 ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		marketGoods->last_month_supply = marketGoods->cur_month_supply;
		marketGoods->cur_month_supply  = (float) 0;

		marketGoods->last_month_sale_qty = marketGoods->cur_month_sale_qty;
		marketGoods->cur_month_sale_qty  = (float) 0;
	}
}
//----------- End of function FirmMarket::next_month -----------//


//--------- Begin of function FirmMarket::next_year ---------//
//
void FirmMarket::next_year()
{
	Firm::next_year();

	//------ post goods supply data ------//

	MarketGoods* marketGoods = market_goods_array;

	for( int i=0 ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		marketGoods->last_year_sales = marketGoods->cur_year_sales;
		marketGoods->cur_year_sales  = (float) 0;
	}
}
//----------- End of function FirmMarket::next_year -----------//


//------- Begin of function FirmMarket::can_hire_caravan -------//
//
// return: <int> 0 - if there is no more caravan we can hire
//					 >0 - the number of new caravans we can hire.
//
int FirmMarket::can_hire_caravan()
{
	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->cash < 0 )
		return 0;

	// ###### patch begin Gilbert 16/2 ######//
	int supportedCaravan = nationPtr->total_population / POPULATION_PER_CARAVAN;
	if( supportedCaravan < 1 )
		supportedCaravan = 1;
	// ###### patch end Gilbert 16/2 ######//
	int caravanCount 		= unit_res[UNIT_CARAVAN]->nation_unit_count_array[nation_recno-1];

	if( supportedCaravan > caravanCount )
		return supportedCaravan - caravanCount;
	else
		return 0;
}
//-------- End of function FirmMarket::can_hire_caravan --------//


//--------- Begin of function FirmMarket::hire_caravan ---------//
//
short FirmMarket::hire_caravan(char remoteAction)
{
	if( !can_hire_caravan() )
		return 0;

	//---------------------------------------//

	Nation *nationPtr = nation_array[nation_recno];

	if(!remoteAction && remote.is_enable())
	{
		// packet structure : <firm recno>
		short *shortPtr = (short *) remote.new_send_queue_msg(MSG_F_MARKET_HIRE_CARA, sizeof(short));
		*shortPtr = firm_recno;
		return 0;
	}

	//---------- add the unit now -----------//

	int unitRecno = create_unit( UNIT_CARAVAN );

	UnitCaravan* unitCaravan = (UnitCaravan*)unit_array[unitRecno];

	unitCaravan->loyalty = 100;
	unitCaravan->set_stop( 1, loc_x1, loc_y1, COMMAND_AUTO );

	//---------- deduct cash for the caravan's cost ----------//

	if(unitCaravan)
		return unitCaravan->sprite_recno;
	else
		return 0;
}
//----------- End of function FirmMarket::hire_caravan -----------//


//--------- Begin of function FirmMarket::input_goods ---------//
//
// Input goods from factories and mines.
//
// <int> maxInputQty - maximum goods can be inputed in this call.
//
void FirmMarket::input_goods(int maxInputQty)
{
	//------ scan for a firm to input raw materials --------//

	int   	 		i, t;
	float				inputQty;
	Firm* 	 		firmPtr;
	FirmMine* 		firmMine;
	FirmFactory* 	firmFactory;
	MarketGoods*	marketGoods;
	Nation*		   nationPtr = nation_array[nation_recno];
	char				is_inputing_array[MAX_MARKET_GOODS];
	short				queued_firm_recno=0;

	memset( is_inputing_array, 0, sizeof(is_inputing_array) );

	for( t=0 ; t<linked_firm_count ; t++ )
	{
		if( linked_firm_enable_array[t] != LINK_EE )
			continue;

		firmPtr = firm_array[linked_firm_array[t]];

		//----------- check if the firm is a mine ----------//

		if( firmPtr->firm_id != FIRM_MINE && firmPtr->firm_id != FIRM_FACTORY )
			continue;

		//--------- if it's a mine ------------//

		if( firmPtr->firm_id == FIRM_MINE )
		{
			firmMine = (FirmMine*) firmPtr;

			if( firmMine->raw_id )
			{
				for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
				{
					//--- only assign a slot to the product if it comes from a firm of our own ---//

					if( marketGoods->raw_id == firmMine->raw_id )
					{
						is_inputing_array[i] = 1;

						if( firmMine->next_output_firm_recno == firm_recno &&
							 firmMine->stock_qty > 0 && marketGoods->stock_qty < max_stock_qty )
						{
							inputQty = min( firmMine->stock_qty, maxInputQty );
							inputQty = min( inputQty, max_stock_qty - marketGoods->stock_qty );

							firmMine->stock_qty	  -= inputQty;
							marketGoods->stock_qty += inputQty;
							marketGoods->cur_month_supply += inputQty;

							if( firmPtr->nation_recno != nation_recno )
								nationPtr->import_goods(IMPORT_RAW, firmPtr->nation_recno, inputQty*RAW_PRICE );
						}
						else if( marketGoods->stock_qty == max_stock_qty )
						{
							marketGoods->cur_month_supply++;		// add it so the other functions can know that this market has direct supply links
						}

						break;
					}
				}

				//----- no matched slot for this goods -----//

				if( i==MAX_MARKET_GOODS && firmMine->stock_qty>0 && !queued_firm_recno )
					queued_firm_recno = firmPtr->firm_recno;
			}
		}

		//--------- if it's a factory ------------//

		else if( firmPtr->firm_id == FIRM_FACTORY )
		{
			firmFactory = (FirmFactory*) firmPtr;

			if( firmFactory->product_raw_id )
			{
				for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
				{
					if( marketGoods->product_raw_id == firmFactory->product_raw_id )
					{
						is_inputing_array[i] = 1;

						if( firmFactory->next_output_firm_recno == firm_recno &&
							 firmFactory->stock_qty > 0 && marketGoods->stock_qty < max_stock_qty )
						{
							inputQty = min( firmFactory->stock_qty, maxInputQty );
							inputQty = min( inputQty, max_stock_qty - marketGoods->stock_qty );

							firmFactory->stock_qty -= inputQty;
							marketGoods->stock_qty += inputQty;
							marketGoods->cur_month_supply += inputQty;

							if( firmPtr->nation_recno != nation_recno )
								nationPtr->import_goods(IMPORT_PRODUCT, firmPtr->nation_recno, inputQty*PRODUCT_PRICE );
						}
						else if( marketGoods->stock_qty == max_stock_qty )
						{
							marketGoods->cur_month_supply++;		// add it so the other functions can know that this market has direct supply links
						}

						break;
					}
				}

				//----- no matched slot for this goods -----//

				if( i==MAX_MARKET_GOODS && firmFactory->stock_qty>0 && !queued_firm_recno )
					queued_firm_recno = firmPtr->firm_recno;
			}
		}
	}

	//---- if there are any empty slots for new goods -----//

	if( queued_firm_recno > 0 )
	{
		firmPtr = firm_array[queued_firm_recno];

		for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
		{
			if( !is_inputing_array[i] && marketGoods->stock_qty==0 )
			{
				if( firmPtr->firm_id == FIRM_MINE )
				{
					set_goods(1, ((FirmMine*)firmPtr)->raw_id, i);
					break;
				}
				else if( firmPtr->firm_id == FIRM_FACTORY )
				{
					set_goods(0, ((FirmFactory*)firmPtr)->product_raw_id, i);
					break;
				}
			}
		}
	}
}
//----------- End of function FirmMarket::input_goods -----------//


//------- Begin of function FirmMarket::set_goods -----------//
void FirmMarket::set_goods(int isRaw, int goodsId, int position)
{
	MarketGoods *marketGoods = market_goods_array+position;
	if(isRaw)
	{
#if( MAX_MARKET_GOODS >= MAX_RAW + MAX_PRODUCT )
		err_when( marketGoods->raw_id != goodsId );
#else
		marketGoods->raw_id = goodsId;
		marketGoods->product_raw_id = 0;
#endif
	}
	else
	{
#if( MAX_MARKET_GOODS >= MAX_RAW + MAX_PRODUCT )
		err_when( marketGoods->product_raw_id != goodsId );
#else
		marketGoods->raw_id = 0;
		marketGoods->product_raw_id = goodsId;
#endif
	}

	if( firm_array.selected_recno == firm_recno )
		info.disp();
}
//----------- End of function FirmMarket::set_goods -----------//


//------- Begin of function FirmMarket::sell_goods -----------//
//
// Sell products to consumers. Called by Town::sell_goods()
//
void FirmMarket::sell_goods()
{
	//----------- sell products now ------------//

	int   	 		i;
	float			   saleQty;
	MarketGoods*	marketGoods;
	float				consumerPrice = CONSUMER_PRICE;

	//-------- increased cash for human players -------//

	if( !is_ai )   	// Human players
	{
		switch( config.start_up_cash )
		{
			case OPTION_VERY_HIGH:
				consumerPrice += 2.5;
				break;

			case OPTION_HIGH:
				consumerPrice += 1.5;
				break;
		}
	}
	else					// AI
	{
		switch( config.ai_start_up_cash )
		{
			case OPTION_VERY_HIGH:
				consumerPrice += 2;
				break;

			case OPTION_HIGH:
				consumerPrice += 1;
				break;
		}
	}

	//-----------------------------------------//

	for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		if( marketGoods->product_raw_id && marketGoods->stock_qty > 0 )
		{
			saleQty = min(marketGoods->month_demand/30, marketGoods->stock_qty);

			marketGoods->stock_qty -= saleQty;

			marketGoods->cur_month_sale_qty += saleQty;

			float sales = saleQty * consumerPrice;
			// ##### begin Gilbert 16/2 #######//
			if( nation_recno && god_res[GOD_CHINESE]->nation_prayer_count(nation_recno) > 0 )
				// sales = sales * 1.20f;		// profit incrase by 20%
				sales = sales * 1.30f;		// profit incrase by 30%
			// ##### end Gilbert 16/2 #######//
			marketGoods->cur_year_sales  	  += sales;

			// AI_CHEAT

			if( is_ai && config.ai_aggressiveness > OPTION_MODERATE )
				sales += sales * (config.ai_aggressiveness-OPTION_MODERATE) / 4;		// 25% more cash if AI aggressivness is high, 50% more if aggressiveness is very high

			add_income(INCOME_SELL_GOODS, sales);
		}
	}
}
//--------- End of function FirmMarket::sell_goods -----------//


//------- Begin of function FirmMarket::free_unused_slot -----------//
//
// Free up unused slots (those with sales==0 and stock_qty==0)
//
void FirmMarket::free_unused_slot()
{
#if( MAX_MARKET_GOODS < MAX_RAW + MAX_PRODUCT )
	int   	 		i;
	MarketGoods*	marketGoods;
	for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		if( marketGoods->product_raw_id || marketGoods->raw_id )
		{
			if( marketGoods->sales_365days()==0 &&
				 marketGoods->supply_30days()==0 &&
				 marketGoods->stock_qty==0 )
			{
				clear_market_goods(i+1);
			}
		}
	}
#endif
}
//--------- End of function FirmMarket::free_unused_slot -----------//


//------- Begin of function FirmMarket::clear_market_goods ------//
void FirmMarket::clear_market_goods(int position)
{
	MarketGoods	*marketGoods = market_goods_array + position - 1;

	err_when((marketGoods->raw_id && marketGoods->product_raw_id) ||
				(!marketGoods->raw_id && !marketGoods->product_raw_id));
	marketGoods->stock_qty = (float) 0;

//	if(marketGoods->raw_id)
//	{
//		market_raw_array[marketGoods->raw_id-1] = NULL;
//		marketGoods->raw_id = 0;
//	}
//	else
//	{
//		market_product_array[marketGoods->raw_id-1] = NULL;
//		marketGoods->product_raw_id = 0;
//	}
}
//--------- End of function FirmMarket::clear_market_goods -----------//


//------- Begin of function FirmMarket::set_next_output_firm ------//
//
// Set next_output_firm_recno, the recno of the linked firm
// to which this firm is going to output goods.
//
void FirmMarket::set_next_output_firm()
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

			if( firmId==FIRM_FACTORY )
			{
				next_output_firm_recno = firmRecno;
				return;
			}
		}
	}

	next_output_firm_recno = 0;		// this mine has no linked output firms
}
//-------- End of function FirmMarket::set_next_output_firm ---------//


//------- Begin of function FirmMarket::stock_value_index ------//
//
// For AI, return a 0-100 index number telling the total value
// of the market's stock.
//
int FirmMarket::stock_value_index()
{
	int   	 		i;
	float				totalValue = (float) 0;
	MarketGoods*	marketGoods;

	for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		if( marketGoods->raw_id )
		{
			totalValue += marketGoods->stock_qty * RAW_PRICE;
		}
		else if( marketGoods->product_raw_id )
		{
			totalValue += marketGoods->stock_qty * PRODUCT_PRICE;
		}
	}

	return 100 * (int)totalValue / (MAX_MARKET_GOODS * PRODUCT_PRICE * MAX_MARKET_STOCK);
}
//-------- End of function FirmMarket::stock_value_index ---------//


//--------- Begin of function FirmMarket::free_slot_count ---------//
//
// Count the number of free slots available in the market.
//
int FirmMarket::free_slot_count()
{
	MarketGoods* marketGoods = market_goods_array;
	int			 freeSlotCount = 0;

	for( int i=0 ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
	{
		if( !marketGoods->raw_id && !marketGoods->product_raw_id )
			freeSlotCount++;
	}

	return freeSlotCount;
}
//----------- End of function FirmMarket::free_slot_count -----------//


/*
//--------- Begin of function FirmMarket::read_derived_file ---------//
//
int FirmMarket::read_derived_file(File* filePtr)
{
	if( !Firm::read_derived_file(filePtr) )
		return 0;

	//----- reset market_raw_array[] & market_product_array[] ----//

//	for( int i=0 ; i<MAX_RAW ; i++ )
//	{
//		market_raw_array[i]	   = NULL;
//		market_product_array[i] = NULL;
//	}

	//------- rebuild market_product_array --------//

//	int rawId, productId;
//	for( i=0 ; i<MAX_MARKET_GOODS ; i++ )
//	{
//		rawId 	 = market_goods_array[i].raw_id;
//		productId = market_goods_array[i].product_raw_id;
//		if( rawId )
//			market_raw_array[rawId-1] = market_goods_array + i;
//		if( productId )
//			market_product_array[productId-1] = market_goods_array + i;
//	}

	return 1;
}
//----------- End of function FirmMarket::read_derived_file -----------//
*/


//----- Begin of function FirmMarket::update_trade_link -----//
//
// Update the status of links to harbors and towns based
// on the current trade treaty status. 
//
void FirmMarket::update_trade_link()
{
	Nation* ownNation = nation_array[nation_recno];
	int tradeTreaty;

	//------ update links to towns -----//

	Town* townPtr;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		 townPtr = town_array[linked_town_array[i]];

		 if( !townPtr->nation_recno )
			 continue;

		 if( townPtr->nation_recno )
		 {
			 tradeTreaty = ownNation->get_relation(townPtr->nation_recno)->trade_treaty
								|| townPtr->nation_recno==nation_recno;
		 }
		 else		// if this is an independent town, it buys goods from the market if its resistance towards the nation is < 30
		 {
			 tradeTreaty = townPtr->resistance(nation_recno) <= INDEPENDENT_LINK_RESISTANCE;
		 }

		 if( linked_town_enable_array[i] != (tradeTreaty ? LINK_EE : LINK_DD) )
			 toggle_town_link( i+1, tradeTreaty, COMMAND_AUTO, 1 );					// 1-toggle both side
	}
}
//------ End of function FirmMarket::update_trade_link -----//

