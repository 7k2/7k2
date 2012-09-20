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

//Filename    : OU_CARA.CPP
//Description : Unit Caravan

#include <ou_cara.h>
#include <ostr.h>
#include <oinfo.h>
#include <oconfig.h>
#include <ogame.h>
#include <onation.h>
#include <oremote.h>
#include <of_mine.h>
#include <of_fact.h>
#include <ose.h>
#include <ofirmall.h>

//------------- Define static vars ------------//

static char	dummyCaravanEnableFlag[MAX_STOP_FOR_CARAVAN][MAX_GOODS_SELECT_BUTTON];
static char	dummyCaravanGoodsNum[MAX_STOP_FOR_CARAVAN];


//------------- define static function ------------//
static void update_caravan_stop_and_goods_to_dummy(UnitCaravan *caravanPtr)
{
	CaravanStop *stopPtr = caravanPtr->stop_array;
	for(int i=0; i<MAX_STOP_FOR_CARAVAN; i++, stopPtr++)
	{
		if(!stopPtr->firm_recno)
			continue;

		err_when(firm_array.is_deleted(stopPtr->firm_recno));
		dummyCaravanGoodsNum[i] = stopPtr->update_pick_up(dummyCaravanEnableFlag[i]);
	}
}

//--------- Begin of function UnitCaravan::UnitCaravan ---------//
//
UnitCaravan::UnitCaravan()
{
	memset( stop_array, 0, MAX_STOP_FOR_CARAVAN * sizeof(CaravanStop) );

	journey_status		= ON_WAY_TO_FIRM;
	// ##### begin Gilbert 21/12 #####//
	// dest_stop_id		= 1;
	dest_stop_id		= 0;
	// ##### end Gilbert 21/12 #####//
	stop_defined_num	= 0;
	wait_count			= 0;
	stop_x_loc			= 0;
	stop_y_loc			= 0;
	default_market_trade_mode = 1;		// 1 = auto, 0 = manual

	memset(raw_qty_array, 0, sizeof(short)*MAX_RAW);
	memset(product_raw_qty_array, 0, sizeof(short)*MAX_PRODUCT);
}
//---------- End of function UnitCaravan::UnitCaravan ----------//


//--------- Begin of function UnitCaravan::set_stop_pick_up ---------//
//
// Set the pickup type of a specific stop of this caravan.
//
// <int> stopId		  - id. of the stop. (1 - MAX_STOP_FOR_CARAVAN)
// <int> newPickUpType - set the pickup type of the specific stop. (0 - MAX_GOODS_SELECT_BUTTON-1)
// <int> remoteActoin  - remote action type
//
void UnitCaravan::set_stop_pick_up(int stopId, int newPickUpType, int remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <unit recno> <stop id> <new pick_up_type>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_U_CARA_CHANGE_GOODS, 3*sizeof(short));
		*shortPtr = sprite_recno;

		shortPtr[1] = stopId;
		shortPtr[2] = newPickUpType;
		return;
	}
	else
	{
		switch(newPickUpType)
		{
		case AUTO_PICK_UP:
			stop_array[stopId-1].pick_up_set_auto();
			break;

		case NO_PICK_UP:
			stop_array[stopId-1].pick_up_set_none();
			break;

		case MANUAL_PICK_UP:
			stop_array[stopId-1].pick_up_set_manual();
			break;

		default:
			stop_array[stopId-1].pick_up_toggle(newPickUpType);
			break;
		}

		// if not all market is not auto, set to manual

		if( newPickUpType != AUTO_PICK_UP && newPickUpType != MANUAL_PICK_UP && stop_array[stopId-1].firm_id == FIRM_MARKET )
		{
			default_market_trade_mode = 0;	// manual
			for(int i=0; i < MAX_STOP_FOR_CARAVAN; ++i )
			{
				stop_array[stopId-1].pick_up_set_manual();
			}
		}
	}
}
//---------- End of function UnitCaravan::set_stop_pick_up ----------//


//--------- Begin of function UnitCaravan::set_stop ---------//
//
// <int> stopId 				 - the id. of the stop
// <int> stopXLoc, stopYLoc - the location of the stop
//
void UnitCaravan::set_stop(int stopId, int stopXLoc, int stopYLoc, char remoteAction)
{
	//-------------------------------------------------------//
	// check if there is a station in the given location
	//-------------------------------------------------------//

	Location *locPtr = world.get_loc(stopXLoc, stopYLoc);

	if(!locPtr->is_firm())
		return;

	Firm *firmPtr = firm_array[locPtr->firm_recno()];

	if( !can_set_stop(firmPtr->firm_recno) )
		return;

	//-------------------------------------------------------//
	// return if the market stop is in another territory
	//-------------------------------------------------------//

	if(world.get_loc(next_x_loc(), next_y_loc())->region_id!=locPtr->region_id)
		return;

	// ------- cancel if select duplicated market stop -------//

	for( int i = 0; i < MAX_STOP_FOR_CARAVAN; ++i )
	{
		if( stop_array[i].firm_recno == firmPtr->firm_recno )
			return;
	}

	//-------------------------------------------//

	if(!remoteAction && remote.is_enable())
	{
		// packet structure : <unit recno> <stop id> <stop x> <stop y>
		short *shortPtr = (short *) remote.new_send_queue_msg(MSG_U_CARA_SET_STOP, 4*sizeof(short));
		*shortPtr = sprite_recno;
		shortPtr[1] = stopId;
		shortPtr[2] = stopXLoc;
		shortPtr[3] = stopYLoc;
		return;
	}

	if(!stop_array[stopId-1].firm_recno)
	{
		stop_defined_num++;	// no plus one if the recno is defined originally
		// ##### begin Gilbert 21/12 #######//
		if( stop_defined_num == 1 )			// the only stop set
		{
			dest_stop_id = 1;
		}
		// ##### end Gilbert 21/12 #######//

		err_when( stop_defined_num > MAX_STOP_FOR_CARAVAN );
	}

	//-------------------------------------------------------//
	// set the station recno of the stop
	//-------------------------------------------------------//

	CaravanStop *stopPtr = stop_array+stopId-1;

	if(stopPtr->firm_recno == firmPtr->firm_recno)
	{
		err_when(stopPtr->firm_loc_x1!=firmPtr->loc_x1 || stopPtr->firm_loc_y1!=firmPtr->loc_y1 ||
					stopPtr->firm_id!=firmPtr->firm_id);
		return; // same stop as before
	}

	//-------------- reset ignore_power_nation -------------//

	ignore_power_nation = 0;

	short oldStopFirmRecno = dest_stop_id ? stop_array[dest_stop_id-1].firm_recno : 0;
	short newStopFirmRecno;
	memset(stopPtr->pick_up_array, 0, sizeof(char)*MAX_PICK_UP_GOODS);
	stopPtr->firm_recno		= firmPtr->firm_recno;
	stopPtr->firm_id			= firmPtr->firm_id;
	stopPtr->firm_loc_x1		= firmPtr->loc_x1;
	stopPtr->firm_loc_y1		= firmPtr->loc_y1;

	//------------------------------------------------------------------------------------//
	// codes for setting pick_up_type
	//------------------------------------------------------------------------------------//

	int goodsId;

	switch(firmPtr->firm_id)
	{
		case FIRM_MINE:
				// ###### begin Gilbert 3/11 #####//
				goodsId = ((FirmMine*)firmPtr)->raw_id;
				if(goodsId)
					stopPtr->pick_up_toggle(goodsId + PICK_UP_RAW_FIRST - 1); // enable
				// ###### end Gilbert 3/11 #####//
				else
					stopPtr->pick_up_set_none();
				break;

		case FIRM_FACTORY:
				// ###### begin Gilbert 3/11 #####//
				goodsId = ((FirmFactory*)firmPtr)->product_raw_id;
				if(goodsId)
					stopPtr->pick_up_toggle(goodsId + PICK_UP_PRODUCT_FIRST - 1); // enable
				// ###### end Gilbert 3/11 #####//
				else
					stopPtr->pick_up_set_none();
				break;

		case FIRM_MARKET:
				if( default_market_trade_mode )
					stopPtr->pick_up_set_auto();
				else
					stopPtr->pick_up_set_manual();
				break;

		default:
				err_here();
				break;
	}

	last_set_stop_date = info.game_date;

	//-------------------------------------------------------//
	// remove duplicate stop or stop change nation
	//-------------------------------------------------------//

	update_stop_list();

	if(dest_stop_id)
	{
		if((newStopFirmRecno=stop_array[dest_stop_id-1].firm_recno) != oldStopFirmRecno)
		{
			firmPtr = firm_array[newStopFirmRecno];
			err_when(firmPtr->firm_id!=FIRM_MARKET && firmPtr->firm_id!=FIRM_MINE && firmPtr->firm_id!=FIRM_FACTORY);
			move(firmPtr->loc_x1, firmPtr->loc_y1, true);		// true-force move
			journey_status = ON_WAY_TO_FIRM;
		}
	}
	else
		stop_order();
}
//---------- End of function UnitCaravan::set_stop ----------//


//--------- Begin of function UnitCaravan::del_stop ---------//
void UnitCaravan::del_stop(int stopId, char remoteAction)
{
	if(!remoteAction && remote.is_enable())
	{
		// packet structure : <unit recno> <stop id>
		short *shortPtr = (short *) remote.new_send_queue_msg(MSG_U_CARA_DEL_STOP, 2*sizeof(short));
		*shortPtr = sprite_recno;
		shortPtr[1] = stopId;
		return;
	}

	//------ stop is deleted before receiving this message, thus, ignore invalid message -----//

	if(remote.is_enable() && stop_array[stopId-1].firm_recno==0)
		return;

	stop_array[stopId-1].firm_recno = 0;
	stop_defined_num--;
	err_when( stop_defined_num < 0 );

	update_stop_list();
}
//---------- End of function UnitCaravan::del_stop ----------//


//--------- Begin of function UnitCaravan::update_stop_list ---------//
void UnitCaravan::update_stop_list()
{
	err_when(stop_defined_num<0 || stop_defined_num>MAX_STOP_FOR_CARAVAN);

	//------------- used to debug for multiplayer game ------------------//
	#ifdef DEBUG
		misc.random(100);
	#endif

	//-------------------------------------------------------//
	// backup original destination stop firm recno
	//-------------------------------------------------------//
	short nextStopRecno = stop_array[dest_stop_id-1].firm_recno;

	//----------------------------------------------------------------------//
	// check stop existence and the relationship between firm's nation
	//----------------------------------------------------------------------//
	CaravanStop *nodePtr = stop_array;
	Firm			*firmPtr;
	int i;
	for(i=0; i<MAX_STOP_FOR_CARAVAN; i++, nodePtr++)
	{
		if(!nodePtr->firm_recno)
			continue;

		if(firm_array.is_deleted(nodePtr->firm_recno))
		{
			nodePtr->firm_recno = 0;	// clear the recno
			stop_defined_num--;
			err_when( stop_defined_num < 0 );

			continue;
		}

		firmPtr = firm_array[nodePtr->firm_recno];

		if( !can_set_stop(nodePtr->firm_recno) ||
			 firmPtr->loc_x1!=nodePtr->firm_loc_x1 ||
			 firmPtr->loc_y1!=nodePtr->firm_loc_y1 )
		{
			nodePtr->firm_recno = 0;
			stop_defined_num--;
			err_when( stop_defined_num < 0 );

			continue;
		}
	}

	//-------------------------------------------------------//
	// remove duplicate node
	//-------------------------------------------------------//
	CaravanStop *insertNodePtr = stop_array;

	if(stop_defined_num<1)
	{
		memset(stop_array, 0, sizeof(CaravanStop)*MAX_STOP_FOR_CARAVAN);
		dest_stop_id = 0;
		return;	// no stop
	}

	//-------------------------------------------------------//
	// pack the firm_recno to the beginning part of the array
	//-------------------------------------------------------//
	short compareRecno;
	for(i=0, nodePtr=stop_array; i<MAX_STOP_FOR_CARAVAN; i++, nodePtr++)
	{
		if(nodePtr->firm_recno)
		{
			compareRecno = nodePtr->firm_recno;
			break;
		}
	}

	if(i++) // else, the first record is already in the beginning of the array
		memcpy(insertNodePtr, nodePtr, sizeof(CaravanStop));

	if(stop_defined_num==1)
	{
		memset(insertNodePtr+1, 0, sizeof(CaravanStop)*(MAX_STOP_FOR_CARAVAN-1));
		dest_stop_id = 1;
		return;
	}

	short unprocessed = stop_defined_num-1;
	err_when(i==MAX_STOP_FOR_CARAVAN); // error if only one record
	err_when(!unprocessed);
	insertNodePtr++;
	nodePtr++;

	for(; i<MAX_STOP_FOR_CARAVAN && unprocessed; i++, nodePtr++)
	{
		if(!nodePtr->firm_recno)
			continue; // empty

		err_when(!nodePtr->firm_recno);
		if(nodePtr->firm_recno==compareRecno)
		{
			nodePtr->firm_recno = 0;
			stop_defined_num--;
			err_when( stop_defined_num < 0 );
		}
		else
		{
			compareRecno = nodePtr->firm_recno;

			if(insertNodePtr!=nodePtr)
				memcpy(insertNodePtr++, nodePtr, sizeof(CaravanStop));
			else
				insertNodePtr++;
		}
		unprocessed--;
	}

	if(stop_defined_num>2)
	{
		//-------- compare the first and the end record -------//
		nodePtr = stop_array + stop_defined_num - 1; // point to the end
		if(nodePtr->firm_recno == stop_array[0].firm_recno)
		{
			nodePtr->firm_recno = 0;	// remove the end record
			stop_defined_num--;
			err_when( stop_defined_num < 0 );
		}
	}

	if(stop_defined_num<MAX_STOP_FOR_CARAVAN)
		memset(stop_array+stop_defined_num, 0, sizeof(CaravanStop)*(MAX_STOP_FOR_CARAVAN-stop_defined_num));

	#ifdef DEBUG
		int debugCount;
		for(debugCount=0; debugCount<stop_defined_num; debugCount++)
			err_when(!stop_array[debugCount].firm_recno);

		for(; debugCount<MAX_STOP_FOR_CARAVAN; debugCount++)
			err_when(stop_array[debugCount].firm_recno);

		for(debugCount=0; debugCount<stop_defined_num; debugCount++)
			err_when(stop_array[debugCount].firm_recno &&
						stop_array[debugCount].firm_recno==stop_array[(debugCount+1)%MAX_STOP_FOR_CARAVAN].firm_recno);
	#endif

	//-----------------------------------------------------------------------------------------//
	// There should be at least one stop in the list.  Otherwise, clear all the stops
	//-----------------------------------------------------------------------------------------//
	int ourFirmExist = 0;
	for(i=0, nodePtr=stop_array; i<stop_defined_num; i++, nodePtr++)
	{
		err_when(firm_array.is_deleted(nodePtr->firm_recno));
		firmPtr = firm_array[nodePtr->firm_recno];
		if(firmPtr->nation_recno==nation_recno)
		{
			ourFirmExist++;
			break;
		}
	}

	if(!ourFirmExist) // none of the markets belong to our nation
	{
		memset(stop_array, 0, MAX_STOP_FOR_CARAVAN * sizeof(CaravanStop));
		journey_status		= ON_WAY_TO_FIRM;
		dest_stop_id		= 0;
		stop_defined_num	= 0;
		return;
	}

	//-----------------------------------------------------------------------------------------//
	// reset dest_stop_id since the order of the stop may be changed
	//-----------------------------------------------------------------------------------------//

	int xLoc = next_x_loc();
	int yLoc = next_y_loc();
	int dist, minDist=0x7FFF;

	for(i=0, dest_stop_id=0, nodePtr=stop_array; i<stop_defined_num; i++, nodePtr++)
	{
		if(nodePtr->firm_recno==nextStopRecno)
		{
			dest_stop_id = i+1;
			break;
		}
		else
		{
			firmPtr = firm_array[nodePtr->firm_recno];
			dist = misc.points_distance(xLoc, yLoc, firmPtr->center_x, firmPtr->center_y);

			if(dist<minDist)
			{
				dist = minDist;
				dest_stop_id = i+1;
			}
		}
	}

	err_when(dest_stop_id<0 || dest_stop_id>MAX_STOP_FOR_CARAVAN);
}
//----------- End of function UnitCaravan::update_stop_list -----------//


//--------- Begin of function UnitCaravan::update_stop_and_goods_info ---------//
void UnitCaravan::update_stop_and_goods_info()
{
	update_stop_list();
}
//----------- End of function UnitCaravan::update_stop_and_goods_info -----------//


//--------- Begin of function UnitCaravan::can_set_stop ---------//
//
// Whether can set a caravan's stop on the given firm.
//
int UnitCaravan::can_set_stop(int firmRecno)
{
	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->under_construction )
		return 0;

	switch(firmPtr->firm_id)
	{
		case FIRM_MARKET:
			return nation_array[nation_recno]->get_relation(firmPtr->nation_recno)->trade_treaty;

		case FIRM_MINE:
		case FIRM_FACTORY:
			return nation_recno == firmPtr->nation_recno;

		default:
			return 0;
	}
}
//----------- End of function UnitCaravan::can_set_stop -----------//


//--------- Begin of function UnitCaravan::get_next_stop_id ---------//
//
// Get the id. of the next defined stop.
//
// [int] curStopId - the id. of the current stop.
//							if it is MAX_STOP_FOR_CARAVAN, this function will return
//							the id. of the first valid stop.
//
//      					(default: MAX_STOP_FOR_CARAVAN)
// return :	0 ~ MAX_STOP_FOR_CARAVAN, where 0 for no valid stop
//
int UnitCaravan::get_next_stop_id(int curStopId)
{
	int nextStopId = (curStopId>=stop_defined_num) ? 1 : curStopId+1;

	CaravanStop *stopPtr = stop_array+nextStopId-1;

	int needUpdate = 0;

	if(firm_array.is_deleted(stopPtr->firm_recno))
	{
		needUpdate++;
	}
	else
	{
		Firm *firmPtr = firm_array[stopPtr->firm_recno];

		if( !can_set_stop( stopPtr->firm_recno ) ||
			 firmPtr->loc_x1 != stopPtr->firm_loc_x1 ||
			 firmPtr->loc_y1 != stopPtr->firm_loc_y1 )
		{
			needUpdate++;
		}
	}

	if(needUpdate)
	{
		short preStopRecno = stop_array[curStopId-1].firm_recno;

		update_stop_list();

		if(!stop_defined_num)
			return 0;	// no stop is valid

		int i;
		for(i=1, stopPtr=stop_array; i<=stop_defined_num; i++, stopPtr++)
		{
			if(stopPtr->firm_recno==preStopRecno)
				return (i>=stop_defined_num) ? 1 : i+1;
		}

		return 1;
	}
	else
		return nextStopId;
}
//----------- End of function UnitCaravan::get_next_stop_id -----------//


//--------- Begin of function UnitCaravan::pre_process ---------//
//
void UnitCaravan::pre_process()
{
	Unit::pre_process();

	if( cur_action == SPRITE_DIE )		// if the unit is set to die mode in pre_process()
		return;

	if(cur_x == -1) // can't use !is_visible(), keep process if cur_x < -1
		return;

	// #### patch begin Gilbert 3/4 #######//
	// suspected to stop at site, reduce reputation
	if( is_visible() && stop_defined_num == 0	&& nation_recno
		&& world.get_loc(next_x_loc(), next_y_loc())->has_site() )
	{
		nation_array[nation_recno]->change_reputation( -0.02f );
	}
	// #### patch end Gilbert 3/4 #######//

	#define SURROUND_FIRM_WAIT_FACTOR	10

	//-----------------------------------------------------------------------------//
	// stop action if no stop is defined
	//-----------------------------------------------------------------------------//
	if(!stop_defined_num)
	{
		err_when(dest_stop_id!=0);
		if(journey_status!=NO_STOP_DEFINED)
			stop_order();	// stop if no valid stop is defined

		journey_status = NO_STOP_DEFINED;
		return;
	}

	//-----------------------------------------------------------------------------//
	// wait in the surrounding of the stop if stop_defined_num==1 (only one stop)
	//-----------------------------------------------------------------------------//
	if(stop_defined_num==1)
	{
		CaravanStop *stopPtr = &stop_array[0];
		err_when(!stopPtr->firm_recno);

		if(firm_array.is_deleted(stopPtr->firm_recno))
		{
			update_stop_list();
			return;
		}

		Firm *firmPtr = firm_array[stopPtr->firm_recno];
		int firmXLoc1 = firmPtr->loc_x1;
		int firmYLoc1 = firmPtr->loc_y1;
		int firmXLoc2 = firmPtr->loc_x2;
		int firmYLoc2 = firmPtr->loc_y2;
		int firmId = firmPtr->firm_id;
		if(firmXLoc1!=stopPtr->firm_loc_x1 || firmYLoc1!=stopPtr->firm_loc_y1 ||
			(firmId!=FIRM_MINE && firmId!=FIRM_FACTORY && firmId!=FIRM_MARKET))
		{
			update_stop_list();
			return;
		}

		int curXLoc = next_x_loc();
		int curYLoc = next_y_loc();

		if(curXLoc<firmXLoc1-1 || curXLoc>firmXLoc2+1 || curYLoc<firmYLoc1-1 || curYLoc>firmYLoc2+1)
		{
			if(cur_action==SPRITE_IDLE)
				move(firmXLoc1, firmYLoc1, true);
			else
				journey_status = ON_WAY_TO_FIRM;
		}
		else
		{
			journey_status = SURROUND_FIRM;
			//if(firmPtr->nation_recno==nation_recno)
			if(nation_array[nation_recno]->get_relation(firmPtr->nation_recno)->trade_treaty)
			{
				if(wait_count<=0)
				{
					//---------- unloading goods -------------//
					switch(stopPtr->firm_id)
					{
						case FIRM_MINE:
								break; // no goods unload to mine

						case FIRM_FACTORY:
								factory_unload_goods();
								break;

						case FIRM_MARKET:
								market_unload_goods();
								break;

						default: err_here();
									break;
					}

					wait_count = MAX_CARAVAN_WAIT_TERM*SURROUND_FIRM_WAIT_FACTOR;
				}
				else
					wait_count--;
			}
		}
		return;
	}

	//-----------------------------------------------------------------------------//
	// at least 2 stops for the caravan to move between
	//-----------------------------------------------------------------------------//
	err_when(stop_defined_num<=1);

	if(journey_status==INSIDE_FIRM)
		caravan_in_firm();
	else
		caravan_on_way();
}
//----------- End of function UnitCaravan::pre_process -----------//


//--------- Begin of function UnitCaravan::caravan_in_firm ---------//
// journey_status : INSIDE_FIRM -->	ON_WAY_TO_FIRM
//												NO_STOP_DEFINED if no valid stop
//												SURROUND_FIRM if only one stop
//
void UnitCaravan::caravan_in_firm()
{
	//-----------------------------------------------------------------------------//
	// the market is deleted while the caravan is in market
	//-----------------------------------------------------------------------------//
	if(firm_array.is_deleted(entered_firm_recno))
	{
		hit_points = (float) 0;	// caravan also die if the market is deleted
		unit_array.disappear_in_firm(sprite_recno); // caravan also die if the market is deleted
		return;
	}

	//-----------------------------------------------------------------------------//
	// waiting (time to upload/download cargo)
	//-----------------------------------------------------------------------------//
	if(wait_count>0)
	{
		wait_count--;
		return;
	}

	//-----------------------------------------------------------------------------//
	// leave the market and go to another market if possible
	//-----------------------------------------------------------------------------//
	CaravanStop *stopPtr = stop_array + dest_stop_id - 1;
	int xLoc = stop_x_loc;
	int yLoc = stop_y_loc;
	Location *locPtr = world.get_loc(xLoc, yLoc);
	Firm		*firmPtr;

	if(locPtr->can_move(mobile_type))
		init_sprite(xLoc, yLoc); // appear in the location the unit disappeared before
	else
	{
		//---- the entering location is blocked, select another location to leave ----//
		err_when(entered_firm_recno==0);
		firmPtr = firm_array[entered_firm_recno];

		if(appear_in_firm_surround(xLoc, yLoc, firmPtr))
		{
			init_sprite(xLoc, yLoc);
			entered_firm_recno=0;
		}
		else
		{
			wait_count = MAX_CARAVAN_WAIT_TERM*10; //********* BUGHERE, continue to wait or ....
			return;
		}
	}

	//-------------- get next stop id. ----------------//

	int nextStopId = get_next_stop_id(dest_stop_id);
	if(!nextStopId || dest_stop_id==nextStopId)
	{
		dest_stop_id = nextStopId;
		journey_status = (!nextStopId) ? NO_STOP_DEFINED : SURROUND_FIRM;
		return;	// no stop or only one stop is valid
	}

	dest_stop_id = nextStopId;
	firmPtr = firm_array[stop_array[dest_stop_id-1].firm_recno];

	entered_firm_recno = 0; // since entered_firm_recno is used to store the current market recno, reset before searching
	move(firmPtr->loc_x1, firmPtr->loc_y1, true);
	
	journey_status = ON_WAY_TO_FIRM;
}
//----------- End of function UnitCaravan::caravan_in_firm -----------//


//--------- Begin of function UnitCaravan::caravan_on_way ---------//
//
// journey_status : ON_WAY_TO_FIRM --> SURROUND_FIRM
//						  SURROUND_FIRM  --> INSIDE_FIRM
//
void UnitCaravan::caravan_on_way()
{
	CaravanStop *stopPtr = stop_array + dest_stop_id - 1;

	if(cur_action==SPRITE_IDLE && journey_status!=SURROUND_FIRM)
	{
		if(!firm_array.is_deleted(stopPtr->firm_recno))
		{
			Firm *firmPtr = firm_array[stopPtr->firm_recno];
			move(firmPtr->loc_x1, firmPtr->loc_y1, true);
			int nextXLoc = next_x_loc();
			int nextYLoc = next_y_loc();

			if(nextXLoc>=firmPtr->loc_x1-1 && nextXLoc<=firmPtr->loc_x2+1 &&
				nextYLoc>=firmPtr->loc_y1-1 && nextYLoc<=firmPtr->loc_y2+1) // hard code 1 for carvan size 1x1
				journey_status = SURROUND_FIRM;

			if(nextXLoc==cur_order.loc_x && nextYLoc==cur_order.loc_y && !ignore_power_nation)
				ignore_power_nation = 1;

			return;
		}
	}

	short unitRecno = sprite_recno;

	if(unit_array.is_deleted(unitRecno))
		return; 

	if(firm_array.is_deleted(stopPtr->firm_recno))
	{
		update_stop_list();

		if(stop_defined_num) // move to next stop
		{
			Firm *firmPtr = firm_array[stop_array[stop_defined_num-1].firm_recno];
			move(firmPtr->loc_x1, firmPtr->loc_y1, true);
		}
		return;
	}

	//CaravanStop *stopPtr = stop_array + dest_stop_id - 1;
	Firm	*firmPtr = firm_array[stopPtr->firm_recno];

	int nextXLoc = next_x_loc();
	int nextYLoc = next_y_loc();

	if(journey_status==SURROUND_FIRM ||
		( nextXLoc==cur_order.loc_x && nextYLoc==cur_order.loc_y &&
		  cur_x==next_x && cur_y==next_y &&					// move in a tile exactly
		  (nextXLoc>=firmPtr->loc_x1-1 && nextXLoc<=firmPtr->loc_x2+1 &&
			nextYLoc>=firmPtr->loc_y1-1 && nextYLoc<=firmPtr->loc_y2+1) )) // in the surrounding of the firm
	{
		//-------------------- update pick_up_array --------------------//
		stopPtr->update_pick_up();

		//-------------------------------------------------------//
		// load/unload goods
		//-------------------------------------------------------//
		if(nation_array[nation_recno]->get_relation(firmPtr->nation_recno)->trade_treaty)
		{
			// ##### begin Gilbert 15/2 #######//
			// pick goods if any other stop is same nation
			int otherStopHasOwn = 0;
			for( int s = 1; s <= stop_defined_num; ++s )
			{
				if( s == dest_stop_id )
					continue;		// skip that stop

				int stopFirmRecno = stop_array[s-1].firm_recno;
				if( !firm_array.is_deleted(stopFirmRecno) 
					&& firm_array[stopFirmRecno]->nation_recno == nation_recno )
				{
					otherStopHasOwn = 1;
				}
			}

			switch(firmPtr->firm_id)
			{
			case FIRM_MINE:
				if( otherStopHasOwn )
					mine_load_goods(stopPtr->pick_up_type);
				break;

			case FIRM_FACTORY:
				factory_unload_goods();
				if( otherStopHasOwn )
					factory_load_goods(stopPtr->pick_up_type);
				break;

			case FIRM_MARKET:
				market_unload_goods();

				if( otherStopHasOwn )
				{
					if(stopPtr->pick_up_type == AUTO_PICK_UP)
						market_auto_load_goods();
					else if(stopPtr->pick_up_type!=NO_PICK_UP)
						market_load_goods();
				}
				break;

			default:
				err_here();
				break;
			}
			// ##### end Gilbert 15/2 #######//
		}

		//-------------------------------------------------------//
		// entered_firm_recno is used to store the firm_recno of the market
		// where the caravan move in.
		//-------------------------------------------------------//
		entered_firm_recno = stopPtr->firm_recno;

		stop_x_loc = next_x_loc(); // store entering location
		stop_y_loc = next_y_loc();
		wait_count = MAX_CARAVAN_WAIT_TERM;		// set waiting term

		stop_order();
		deinit_sprite(1);	// the caravan enters the market now. 1-keep it selected if it is currently selected

		err_when(cur_x!=-1);
		cur_x--;	// set cur_x to -2, such that invisible but still process pre_process()

		journey_status = INSIDE_FIRM;
	}
	else
	{
		if(cur_action!=SPRITE_MOVE)
		{
			//----------------------------------------------------//
			// blocked by something, go to the destination again
			// note: if return value is 0, cannot reach the firm.		//*********BUGHERE
			//----------------------------------------------------//
			move(firmPtr->loc_x1, firmPtr->loc_y1, true);
			journey_status = ON_WAY_TO_FIRM;
		}
	}
}
//----------- End of function UnitCaravan::caravan_on_way -----------//


//--------- Begin of function UnitCaravan::appear_in_firm_surround ---------//
//
// This function return 1 if a suitable location is found, that means the
// caravan will leave the firm there. Otherwise, return 0.
//
// xLoc, yLoc are reference variables for returning the location found.
//
int UnitCaravan::appear_in_firm_surround(int& xLoc, int& yLoc, Firm* firmPtr)
{
	int upperLeftBoundX	= firmPtr->loc_x1 - 1;	// the surrounding coordinates of the firm
	int upperLeftBoundY	= firmPtr->loc_y1 - 1;
	int lowerRightBoundX = firmPtr->loc_x2 + 1;
	int lowerRightBoundY = firmPtr->loc_y2 + 1;

	int count = 1, inside = 1, found = 0, i;
	int testXLoc = xLoc;
	int testYLoc = yLoc;
	int limit;
	Location *locPtr;

	//---------------------------------------------------------//
	//		9  10  11  12		the location is tested in the order
	//		8   1   2  13		shown, if the location is the surrounding
	//		7   x   3  14		of the firm and non-blocked, break
	//		6   5   4  ...		the test
	//---------------------------------------------------------//

	while(inside)
	{
		inside = 0;
		limit = count<<1;
		err_when(limit!=count*2);

		//------------ upper --------------//
		testXLoc = xLoc - count + 1;
		testYLoc = yLoc - count;
		for(i=0; i<limit; i++)
		{
			if(testXLoc<0 || testYLoc>=MAX_WORLD_X_LOC || testYLoc<0 || testYLoc>=MAX_WORLD_Y_LOC)
				continue;

			if(testXLoc<upperLeftBoundX || testXLoc>lowerRightBoundX || testYLoc<upperLeftBoundY || testYLoc>lowerRightBoundY)
				continue;

			locPtr = world.get_loc(testXLoc, testYLoc);
			if(locPtr->can_move(mobile_type))
			{
				found++;
				break;
			}
			else
				xLoc++;

			inside++;
		}

		if(found)
			break;

		//------------ right --------------//
		testXLoc = xLoc + count;
		testYLoc = yLoc - count + 1;
		for(i=0; i<limit; i++)
		{
			if(testXLoc<0 || testYLoc>=MAX_WORLD_X_LOC || testYLoc<0 || testYLoc>=MAX_WORLD_Y_LOC)
				continue;

			if(testXLoc<upperLeftBoundX || testXLoc>lowerRightBoundX || testYLoc<upperLeftBoundY || testYLoc>lowerRightBoundY)
				continue;

			locPtr = world.get_loc(testXLoc, testYLoc);
			if(locPtr->can_move(mobile_type))
			{
				found++;
				break;
			}
			else
				yLoc++;

			inside++;
		}

		if(found)
			break;

		//------------- down --------------//
		testXLoc = xLoc + count - 1;
		testYLoc = yLoc + count;
		for(i=0; i<limit; i++)
		{
			if(testXLoc<0 || testYLoc>=MAX_WORLD_X_LOC || testYLoc<0 || testYLoc>=MAX_WORLD_Y_LOC)
				continue;

			if(testXLoc<upperLeftBoundX || testXLoc>lowerRightBoundX || testYLoc<upperLeftBoundY || testYLoc>lowerRightBoundY)
				continue;

			locPtr = world.get_loc(testXLoc, testYLoc);
			if(locPtr->can_move(mobile_type))
			{
				found++;
				break;
			}
			else
				xLoc--;

			inside++;
		}

		if(found)
			break;

		//------------- left --------------//
		testXLoc = xLoc - count;
		testYLoc = yLoc + count - 1;
		for(i=0; i<limit; i++)
		{
			if(testXLoc<0 || testYLoc>=MAX_WORLD_X_LOC || testYLoc<0 || testYLoc>=MAX_WORLD_Y_LOC)
				continue;

			if(testXLoc<upperLeftBoundX || testXLoc>lowerRightBoundX || testYLoc<upperLeftBoundY || testYLoc>lowerRightBoundY)
				continue;

			locPtr = world.get_loc(testXLoc, testYLoc);
			if(locPtr->can_move(mobile_type))
			{
				found++;
				break;
			}
			else
				yLoc--;

			inside++;
		}

		if(found)
			break;

		//---------------------------------------------//
		count++;
	}

	if(found)
	{
		xLoc = testXLoc;
		yLoc = testYLoc;
		return 1;
	}

	return 0;
}
//----------- End of function UnitCaravan::appear_in_firm_surround -----------//


//--------- Begin of function UnitCaravan::has_pick_up_type ---------//
//
// Return whether the specific stop has the specific pick up types.
// 
int UnitCaravan::has_pick_up_type(int stopId, int pickUpType)
{
	return stop_array[stopId-1].pick_up_array[pickUpType-1];
}
//---------- End of function UnitCaravan::has_pick_up_type ----------//


//--------- Begin of function UnitCaravan::carrying_qty ---------//
//
// Return whether the qty of the specific product/raw type that
// this caravan is currently carrying.
//
int UnitCaravan::carrying_qty(int pickUpType)
{
	if( pickUpType >= PICK_UP_RAW_FIRST &&
		 pickUpType <= PICK_UP_RAW_LAST )
	{
		return raw_qty_array[pickUpType-PICK_UP_RAW_FIRST];
	}
	else if( pickUpType >= PICK_UP_PRODUCT_FIRST &&
				pickUpType <= PICK_UP_PRODUCT_LAST )
	{
		return product_raw_qty_array[pickUpType-PICK_UP_PRODUCT_FIRST];
	}
	else
	{
		err_here();
		return 0;
	}
}
//---------- End of function UnitCaravan::carrying_qty ----------//

//--------- Begin of function UnitCaravan::can_duplicate_caravan ---------//
//
char UnitCaravan::can_duplicate_caravan()
{
	int i;
	UnitCaravan* caravanPtr = (UnitCaravan*) unit_array[sprite_recno];
	// ####### begin Gilbert 12/4 ########//
	for(i=0; i<MAX_STOP_FOR_CARAVAN; i++)
	{
		CaravanStop *stopPtr = caravanPtr->stop_array + i;

		if(firm_array.is_deleted(stopPtr->firm_recno))
			continue;

		Firm *firmPtr = firm_array[stopPtr->firm_recno];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		Firm *firmPtr2;
		if( firmPtr->cast_to_FirmMarket() )
		{
			FirmMarket *market = firmPtr->cast_to_FirmMarket();
			if(market->can_hire_caravan())
				return 1;
		}
		else
		{
		// else if other kind of firm
			// ###### patch begin Gilbert 7/10 ######//
			for(int x=0; x<firmPtr->linked_firm_count; x++)
			{
				firmPtr2 = firm_array[firmPtr->linked_firm_array[x]];
				if( firmPtr2->nation_recno == nation_recno
					&& firmPtr2->cast_to_FirmMarket() )
				{
					FirmMarket *market = firmPtr2->cast_to_FirmMarket();
					if(market->can_hire_caravan())
						return 1;
				}
			}
			// ###### patch end Gilbert 7/10 ######//
		}
	}
	// ####### end Gilbert 12/4 ########//
	return 0;
}
//--------- End of function UnitCaravan::can_duplicate_caravan ---------//

//--------- Begin of function UnitCaravan::duplicate_caravan ---------//
//
short UnitCaravan::duplicate_caravan(char remoteAction)
{
	int materialInEveryStop;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <sprite recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_U_CARA_DUPLICATE, sizeof(short));
		*shortPtr = sprite_recno;
		return 0;
	}

	UnitCaravan* caravanPtr = (UnitCaravan*) unit_array[sprite_recno];
	CaravanStop *stopPtr = caravanPtr->stop_array;
	for(int i=0; i<MAX_STOP_FOR_CARAVAN; i++, stopPtr++)
	{
		if(firm_array.is_deleted(stopPtr->firm_recno) )
			continue;

		Firm *firmPtr = firm_array[stopPtr->firm_recno];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		FirmMarket *market = firmPtr->cast_to_FirmMarket();
		
		if(!market)
		{
			for(int x=0; x<firmPtr->linked_firm_count; x++)
			{
				Firm *firmPtr2 = firm_array[firmPtr->linked_firm_array[x]];
				if( firmPtr2->nation_recno == nation_recno 
					&& firmPtr2->cast_to_FirmMarket() )
					market = firmPtr2->cast_to_FirmMarket();
			}
		}

		if(market && market->nation_recno == nation_recno)
		{
			short unitRecno = market->hire_caravan(COMMAND_AUTO);
			if(unitRecno)
			{
				UnitCaravan *newCaravanPtr   = (UnitCaravan*) unit_array[unitRecno];
				CaravanStop *newStopPtr = newCaravanPtr->stop_array;
				CaravanStop *oldStopPtr = stopPtr;

				newCaravanPtr->default_market_trade_mode = caravanPtr->default_market_trade_mode;
				newStopPtr->pick_up_type = oldStopPtr->pick_up_type;
				for(materialInEveryStop = 0; materialInEveryStop < MAX_PICK_UP_GOODS; materialInEveryStop++)
					newStopPtr->pick_up_array[materialInEveryStop] = oldStopPtr->pick_up_array[materialInEveryStop];							
				
				newStopPtr++;
				
				// copy stop starting from next stop, loop back to the first

			//	int loopCount = MAX_STOP_FOR_CARAVAN + 1;
			//	int j = i+1;	// source stop id
			//	int k = 1;		// destination stop id
			//	for( ; j != i && j != i+MAX_STOP_FOR_CARAVAN; ++j )
				int loopBreak = 0;
				int j = i;	// source stop id
				int k = 0;		// destination stop id
				// ###### patch begin Gilbert 12/10 ######//
//				for( ; (j != i && j != i+MAX_STOP_FOR_CARAVAN-1) || !loopBreak ; ++j )
//				{
					// skip the market which new caravan comes from
			//		err_when( --loopCount == 0 );
//					if( j >= MAX_STOP_FOR_CARAVAN )
//					{
//						j = 0;
//						loopBreak = 1;
//					}
				// for each stop, but start from stop i and loop back to i-1
				err_when ( j >= MAX_STOP_FOR_CARAVAN );
				int stopCount;
				for( stopCount = MAX_STOP_FOR_CARAVAN; stopCount > 0; --stopCount, ++j )
				{
					if( j >= MAX_STOP_FOR_CARAVAN )
						j -= MAX_STOP_FOR_CARAVAN;
				// ###### patch end Gilbert 12/10 ######//

					oldStopPtr = stop_array + j;
					newStopPtr = newCaravanPtr->stop_array + k;

					if( !firm_array.is_deleted(oldStopPtr->firm_recno) )
					{
						Firm *firmPtr = firm_array[oldStopPtr->firm_recno];
						newCaravanPtr->set_stop( k+1, firmPtr->loc_x1, firmPtr->loc_y1, COMMAND_AUTO);

						newStopPtr->pick_up_type = oldStopPtr->pick_up_type;
						for(materialInEveryStop = 0; materialInEveryStop < MAX_PICK_UP_GOODS; materialInEveryStop++)
							newStopPtr->pick_up_array[materialInEveryStop] = oldStopPtr->pick_up_array[materialInEveryStop];							
						
						//	newCaravanPtr->set_stop_pick_up(j+1, oldStopPtr->pick_up_type, COMMAND_AUTO);

						k++;
					}
				}

			//	newCaravanPtr->stop_defined_num = caravanPtr->stop_defined_num;
			//	newCaravanPtr->dest_stop_id = caravanPtr->dest_stop_id;
				
				return unitRecno;
			}
		}
	}
	return 0;
}
//----------- End of function UnitCaravan::duplicate_caravan -----------//
