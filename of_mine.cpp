//Filename    : OF_MINE.CPP
//Description : Firm Mine

#include <OF_MINE.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OSYS.H>
#include <OSTR.H>
#include <OFONT.H>
#include <ONEWS.H>
#include <OUNIT.H>
#include <ORACERES.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <ONATION.H>
#include <OSITE.H>
#include <OF_FACT.H>

//--------- Begin of function FirmMine::FirmMine ---------//
//
FirmMine::FirmMine()
{
	cur_month_production = (float) 0;
	last_month_production = (float) 0;

	next_output_link_id	  = 0;
	next_output_firm_recno = 0;

	ai_should_build_factory_count = 0;
}
//----------- End of function FirmMine::FirmMine -----------//


//--------- Begin of function FirmMine::~FirmMine ---------//
//
FirmMine::~FirmMine()
{
	if( sys.quick_exit_flag() )
		return;

	//------- update the site deposit reserve ------//

	if( site_recno )
	{
		site_array.untapped_raw_count++;

		if( reserve_qty==0 )		// if the reserve has been used up
		{
			site_array.del_site(site_recno);
		}
		else		// restore the site
		{
			Site* sitePtr = site_array[site_recno];

			sitePtr->reserve_qty = (int) reserve_qty;
			sitePtr->has_mine 	= 0;
		}
	}

	//-------- decrease AI raw count --------//

	if( raw_id )
	{
		nation_array[nation_recno]->raw_count_array[raw_id-1]--;

		err_when( nation_array[nation_recno]->raw_count_array[raw_id-1] < 0 );
	}
}
//----------- End of function FirmMine::~FirmMine -----------//


//--------- Begin of function FirmMine::init_derived ---------//
//
void FirmMine::init_derived()
{
	//---- scan for raw site in this firm's building location ----//

	Location* locPtr = scan_raw_site();

	if( locPtr )
	{
		site_recno  = locPtr->site_recno();
		raw_id		= site_array[site_recno]->object_id;
		reserve_qty = (float) site_array[site_recno]->reserve_qty;

		site_array[site_recno]->has_mine = 1;
		site_array.untapped_raw_count--;

		err_when( site_array.untapped_raw_count < 0 );
	}
	else
	{
		site_recno 	= 0;
		raw_id 		= 0;
		reserve_qty = (float) 0;
	}

	stock_qty 	 	= (float) 0;
	max_stock_qty  = (float) DEFAULT_MINE_MAX_STOCK_QTY;

	//-------- increase AI raw count --------//

	if( raw_id )
		nation_array[nation_recno]->raw_count_array[raw_id-1]++;
}
//----------- End of function FirmMine::init_derived -----------//


//------- Begin of function FirmMine::change_nation ---------//
//
void FirmMine::change_nation(int newNationRecno)
{
	if( raw_id )
	{
		nation_array[nation_recno]->raw_count_array[raw_id-1]--;

		err_when( nation_array[nation_recno]->raw_count_array[raw_id-1] < 0 );

		nation_array[newNationRecno]->raw_count_array[raw_id-1]++;
	}

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmMine::change_nation ---------//


//--------- Begin of function FirmMine::scan_raw_site ---------//
//
Location* FirmMine::scan_raw_site()
{
	//---- scan for raw site in this firm's building location ----//

	int xLoc, yLoc;
	Location* locPtr;

	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
		{
			locPtr = world.get_loc(xLoc,yLoc);

			if( locPtr->has_site() &&
				 site_array[locPtr->site_recno()]->site_type == SITE_RAW )
			{
				return locPtr;
			}
		}
	}

	return NULL;
}
//--------- End of function FirmMine::scan_raw_site ---------//


//--------- Begin of function FirmMine::next_day ---------//
//
void FirmMine::next_day()
{
	//----- call next_day() of the base class -----//

	FirmWork::next_day();

	//---------------------------------------//

	if( info.game_date%PROCESS_GOODS_INTERVAL == firm_recno%PROCESS_GOODS_INTERVAL )		// produce raw materials once every 3 days
	{
		produce_raw();
		set_next_output_firm();						// set next output firm
	}
}
//----------- End of function FirmMine::next_day -----------//


//--------- Begin of function FirmMine::next_month ---------//
//
void FirmMine::next_month()
{
	last_month_production = cur_month_production;
	cur_month_production  = (float) 0;
}
//----------- End of function FirmMine::next_month -----------//


//------- Begin of function FirmMine::set_next_output_firm ------//
//
// Set next_output_firm_recno, the recno of the linked firm
// to which this mine is going to output raw materials.
//
void FirmMine::set_next_output_firm()
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

			if( firmId==FIRM_FACTORY || firmId==FIRM_MARKET )
			{
				next_output_firm_recno = firmRecno;
				return;
			}
		}
	}

	next_output_firm_recno = 0;		// this mine has no linked output firms
}
//-------- End of function FirmMine::set_next_output_firm ---------//


//--------- Begin of function FirmMine::produce_raw ---------//
//
// Produce raw materials.
//
void FirmMine::produce_raw()
{
	//----- if stock capacity reached or reserve exhausted -----//

	if( stock_qty == max_stock_qty || reserve_qty==0 )
		return;

	err_when( reserve_qty < 0 );
	err_when( stock_qty > max_stock_qty );

	//-------- mine raw materials -------//

	float rawQty = (float) 100 * productivity / 100;
	float	yieldRate = 0.5f;

	if( nation_recno )
		yieldRate += 0.25f * tech_res[TECH_MINING]->get_nation_tech_level(nation_recno);

	float produceQty = rawQty * yieldRate;

	if( rawQty > reserve_qty )
	{
		produceQty = (rawQty = reserve_qty) * yieldRate;
	}

	if( produceQty > max_stock_qty-stock_qty )
	{
		rawQty = (produceQty = max_stock_qty-stock_qty) / yieldRate;
	}

	reserve_qty -= rawQty;
	stock_qty	+= produceQty;

	cur_month_production += produceQty;

	site_array[site_recno]->reserve_qty = (int) reserve_qty;		// update the reserve_qty in site_array

	err_when( reserve_qty < 0 );
	err_when( stock_qty > max_stock_qty );

	//---- add news if run out of raw deposit ----//

	if( reserve_qty == 0 )
	{
		site_array.untapped_raw_count++;		// have to restore its first as del_site() will decrease uptapped_raw_count

		site_array.del_site(site_recno);
		site_recno = 0;

		if( nation_recno == nation_array.player_recno )
			news_array.raw_exhaust(raw_id, center_x, center_y, firm_build_id);
	}
}
//----------- End of function FirmMine::produce_raw -----------//
