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

// Filename    : OF_ALCH.CPP
// Description : alchemy Tor

#include <osys.h>
#include <of_alch.h>
#include <all.h>
#include <oremote.h>
#include <otechres.h>
#include <otech.h>
#include <onews.h>
#include <osite.h>
#include <onation.h>


// ------ define constant ------//

const float MAX_CONSUME = 60.0f;


// ------ begin of function FirmMonsterAlchemy::FirmMonsterAlchemy -------//
//
FirmMonsterAlchemy::FirmMonsterAlchemy()
{
	memset( (char *)this + sizeof(Firm), 0, sizeof(FirmMonsterAlchemy) - sizeof(Firm) );
	cur_month_production = (float) 0;
	last_month_production = (float) 0;
}
// ------ end of function FirmMonsterAlchemy::FirmMonsterAlchemy -------//


// ------ begin of function FirmMonsterAlchemy::~FirmMonsterAlchemy -------//
//
FirmMonsterAlchemy::~FirmMonsterAlchemy()
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
// ------ end of function FirmMonsterAlchemy::~FirmMonsterAlchemy -------//


// ------ begin of function FirmMonsterAlchemy::init_derived -------//
//
void FirmMonsterAlchemy::init_derived()
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

		//--- set default conversion method ---//

		if( raw_id )
		{
			if( tech_res[TECH_RAW_TO_LIVE]->get_nation_tech_level(nation_recno) )
				conversion_tech_id = TECH_RAW_TO_LIVE;

			else if( tech_res[TECH_RAW_TO_GOLD]->get_nation_tech_level(nation_recno) )
				conversion_tech_id = TECH_RAW_TO_GOLD;
		}

		err_when( site_array.untapped_raw_count < 0 );
	}
	else
	{
		site_recno 	= 0;
		raw_id 		= 0;
		reserve_qty = (float) 0;
	}

	//-------- increase AI raw count --------//

	if( raw_id )
		nation_array[nation_recno]->raw_count_array[raw_id-1]++;
}
// ------ end of function FirmMonsterAlchemy::init_derived -------//


// ------ begin of function FirmMonsterAlchemy::change_nation -------//
//
void FirmMonsterAlchemy::change_nation(int newNationRecno)
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
// ------ end of function FirmMonsterAlchemy::change_nation -------//


// ------ begin of function FirmMonsterAlchemy::scan_raw_site -------//
//
Location* FirmMonsterAlchemy::scan_raw_site()
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
// ------ end of function FirmMonsterAlchemy::scan_raw_site -------//


// ------ begin of function FirmMonsterAlchemy::next_day -------//
//
void FirmMonsterAlchemy::next_day()
{
	//----- call next_day() of the base class -----//

	Firm::next_day();

	//---------------------------------------//
	if( info.game_date%PROCESS_GOODS_INTERVAL == firm_recno%PROCESS_GOODS_INTERVAL )		// produce raw materials once every 3 days
	{
		produce_raw();
	}
}
// ------ end of function FirmMonsterAlchemy::next_day -------//


// ------ begin of function FirmMonsterAlchemy::next_month -------//
//
void FirmMonsterAlchemy::next_month()
{
	last_month_production = cur_month_production;
	cur_month_production  = (float) 0;
}
// ------ end of function FirmMonsterAlchemy::next_month -------//


// ------ begin of function FirmMonsterAlchemy::can_converse -------//
//
int FirmMonsterAlchemy::can_converse(int newTechId )
{
	if( nation_array.is_deleted(nation_recno) )
		return 0;

	if( newTechId && tech_res[newTechId]->get_nation_tech_level(nation_recno) > 0 )
	{
		switch( newTechId )
		{
		case TECH_LIVE_TO_GOLD:
		case TECH_GOLD_TO_LIVE:
			return 1;

		case TECH_RAW_TO_LIVE:
		case TECH_RAW_TO_GOLD:
			return site_recno && reserve_qty > 0.0f;
		}
	}
	return 0;
}
// ------ end of function FirmMonsterAlchemy::can_converse -------//


// ------ begin of function FirmMonsterAlchemy::change_conversion -------//
//
void FirmMonsterAlchemy::change_conversion(int newTechId, char remoteAction )
{
	if( newTechId == conversion_tech_id )
		return;

	if( newTechId && !can_converse(newTechId) )
	{
		return;
	}

	if( !remoteAction && remote.is_enable() )
	{
		// packet struct : <firm recno> <new tech id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_ALCH_CONVERSE, 2*sizeof(short));
		shortPtr[0] = firm_recno;
		shortPtr[1] = newTechId;
		return;
	}

	conversion_tech_id = newTechId;
}
// ------ end of function FirmMonsterAlchemy::change_conversion -------//


// ------ begin of function FirmMonsterAlchemy::produce_raw -------//
//
void FirmMonsterAlchemy::produce_raw()
{
	if( nation_array.is_deleted(nation_recno) )
		return;

	if( conversion_tech_id == 0 )
		return;

	Nation *nationPtr = nation_array[nation_recno];

	float livePoint = 0.0f;
	float gold = 0.0f;
	float rawQty = 0.0f;

	TechInfo* techInfo = tech_res[TECH_ALCHEMY_YIELD];

	float consumeQty = MAX_CONSUME			// 50% to 100% of maximum consume as the technology increases
							 * ( techInfo->common_max_tech_level + techInfo->get_nation_tech_level(nation_recno) )
							 / ( techInfo->common_max_tech_level * 2 );

	switch( conversion_tech_id )
	{
	case TECH_LIVE_TO_GOLD:
		if( nationPtr->live_points >= consumeQty )
		{
			livePoint = -consumeQty;
			gold = -livePoint * 0.5f;
		}
		break;

	case TECH_GOLD_TO_LIVE:
		if( nationPtr->cash >= consumeQty )
		{
			gold = -consumeQty;
			livePoint = -gold * 0.5f;
		}
		break;

	case TECH_RAW_TO_LIVE:
		if( reserve_qty >= consumeQty )
			rawQty = -consumeQty;
		else
			rawQty = -reserve_qty;		// such that it will use up
		livePoint = -rawQty * 0.2f;
		break;

	case TECH_RAW_TO_GOLD:
		if( reserve_qty >= consumeQty )
			rawQty = -consumeQty;
		else
			rawQty = -reserve_qty;		// such that it will use up
		gold = -rawQty * 0.4f;
		break;

	default:
		err_here();
	}

	// ------ reduce raw material ------- //

	err_when( rawQty > 0.0f );
	if( rawQty < 0.0f )
	{
		reserve_qty += rawQty;		// rawQty is negative
		site_array[site_recno]->reserve_qty = (int) reserve_qty;		// update the reserve_qty in site_array
	}
	err_when( reserve_qty < 0 );

	// ----- increase/reduce gold --------//

	if( gold > 0.0f )
	{
		cur_month_production += gold;
		nationPtr->add_income( INCOME_CONVERSION, gold, 1 );
	}
	else if( gold < 0.0f )
	{
		nationPtr->add_expense( EXPENSE_CONVERSION, -gold, 1 );
	}

	// ----- increase/reduce live point --------//

	if( nationPtr->is_monster() )		// only monster nation has live points
	{
		if( livePoint > 0.0f )
		{
			cur_month_production += livePoint;
			nationPtr->change_live_points(livePoint);
		}
		else if( livePoint < 0.0f )
		{
			nationPtr->change_live_points(livePoint);
		}
	}

	//---- add news if run out of raw deposit ----//

	if( site_recno && reserve_qty == 0 )
	{
		site_array.untapped_raw_count++;		// have to restore its first as del_site() will decrease uptapped_raw_count

		site_array.del_site(site_recno);
		site_recno = 0;

		if( nation_recno == nation_array.player_recno )
			news_array.raw_exhaust(raw_id, center_x, center_y, firm_build_id);
	}
}
// ------ end of function FirmMonsterAlchemy::produce_raw -------//
