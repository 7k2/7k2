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

//Filename    : OSITE.CPP
//Description : Object Site

#include <OSYS.H>
#include <OINFO.H>
#include <OWORLD.H>
#include <OTOWN.H>
#include <OFIRM.H>
#include <OUNIT.H>
#include <ONEWS.H>
#include <OGODRES.H>
#include <ONATION.H>
#include <OSITE.H>
#include <OITEMRES.H>
#include <OTECHRES.H>
#include <OMODEID.H>
#include <OWORLDMT.H>
#include <OIMGRES.H>
#include <OCONFIG.H>
#include <OGAME.H>
#include <math.h>

//-------------- Define constant -----------//

#define EXIST_RAW_RESERVE_QTY 		(MAX_RAW_RESERVE_QTY / 20)   // only sites with reserve qty >= 5% of MAX_RAW_RESERVE_QTY are counted as raw sites

//--------- Begin of function SiteArray::SiteArray -------//

SiteArray::SiteArray() : DynArrayB(sizeof(Site), 50, DEFAULT_REUSE_INTERVAL_DAYS)
{
}
//----------- End of function SiteArray::SiteArray -------//


//--------- Begin of function SiteArray::~SiteArray -------//

SiteArray::~SiteArray()
{
	deinit();
}
//----------- End of function SiteArray::~SiteArray -------//


//--------- Begin of function SiteArray::init -------//

void SiteArray::init()
{
	untapped_raw_count = 0;
	// #### begin Gilbert 3/11 ######//
	scroll_count = gold_coin_count = item_count = 0;
	// #### end Gilbert 3/11 ######//
	// #### begin Gilbert 30/12 ######//
	weapon_blueprint_count = 0;
	// #### begin Gilbert 30/12 ######//
	std_raw_site_count = 0;
}
//----------- End of function SiteArray::init -------//


//--------- Begin of function SiteArray::deinit -------//

void SiteArray::deinit()
{
	if( size()==0 )
		return;

	zap();       // zap the DynArrayB

	untapped_raw_count = 0;
}
//----------- End of function SiteArray::deinit -------//


//--------- Begin of function SiteArray::add_site ---------//
//
// Add a raw item to the site array
//
// If the given location is occupied, it will locate a closest empty
// location to add the item.
//
// <int> xLoc, yLoc - the location of the site
// <int> siteType	  - site type
// <int> objectId   - site parameter
// [int] reserveQty - reserve qty, for raw site only
//
// Return : >0 - the recno of the site.
//          0  - duplicated, not added
//
int SiteArray::add_site(int xLoc, int yLoc, int siteType, int objectId, int reserveQty)
{
	//---- check if the given location is empty ----//

	int foundFlag = 0;

	int siteWidth = 1;
	int siteHeight = 1;
	int gapSpace = 0;
	if( siteType == SITE_RAW )
	{
		siteWidth = raw_res[objectId]->map_loc_width;
		siteHeight = raw_res[objectId]->map_loc_height;
		gapSpace = INTER_PLACE_SPACE;
	}

	if( world.can_build_site(xLoc, yLoc, siteWidth, siteHeight, gapSpace) )
	{
		foundFlag = 1;
	}
	else
	{
		//------ locate for empty space to add the item -------//

		#define ADD_SITE_RANGE	5

		int		 xOffset, yOffset;
		int		 curXLoc = xLoc, curYLoc = yLoc;
		Location* locPtr;
		BYTE	 	 regionId = world.get_region_id(curXLoc, curYLoc);

		for( int i=1 ; i<ADD_SITE_RANGE*ADD_SITE_RANGE ; i++ )
		{
			m.cal_move_around_a_point(i, ADD_SITE_RANGE, ADD_SITE_RANGE, xOffset, yOffset);

			xLoc = curXLoc + xOffset;
			yLoc = curYLoc + yOffset;

			xLoc = max(0, xLoc);
			xLoc = min(MAX_WORLD_X_LOC-siteWidth, xLoc);

			yLoc = max(0, yLoc);
			yLoc = min(MAX_WORLD_Y_LOC-siteHeight, yLoc);

			locPtr = world.get_loc(xLoc, yLoc);

			if( world.can_build_site(xLoc, yLoc, siteWidth, siteHeight, gapSpace) 
				&& locPtr->region_id==regionId )
			{
				foundFlag = 1;
				break;
			}
		}
	}

	if( !foundFlag )
		return 0;

	//----- linkin the raw and update raw attribute ----//

	Site site;

	linkin(&site);

	Site* sitePtr = (Site*) get(recno());

	// #### begin Gilbert 1/2 ######//
	Location *locPtr = NULL;
	if( sizeof(locPtr->extra_para)==sizeof(unsigned char)
		&& recno() > 0xff )
	{
		linkout();
		return 0;
	}
	// #### end Gilbert 1/2 ######//

	sitePtr->init(recno(), siteType, xLoc, yLoc);

	sitePtr->object_id 	= objectId;
	sitePtr->reserve_qty = reserveQty;

	switch( siteType )
	{
		case SITE_RAW:
			untapped_raw_count++;
			break;

		case SITE_SCROLL:
			scroll_count++;
			break;

		case SITE_GOLD_COIN:
		{
			sitePtr->animation_count = 5;
			gold_coin_count++;
			break;
		}
		case SITE_ITEM:
			item_count++;
			break;

		case SITE_WEAPON_BLUEPRINT:
			weapon_blueprint_count++;
			break;
	}

	return sitePtr->site_recno;
}
//----------- End of function SiteArray::add_site ----------//


//--------- Begin of function SiteArray::del_site ----------//
//
// Delete a specified site.
//
// <int> siteRecno = the record no. of the site to be deleted
//
void SiteArray::del_site(int siteRecno)
{
	err_if( siteRecno == 0 )
		err_now( "SiteArray::del_site" );

	Site* sitePtr = site_array[siteRecno];

	switch( sitePtr->site_type )
	{
		case SITE_RAW:
			untapped_raw_count--;
			break;

		case SITE_SCROLL:
			scroll_count--;
			break;

		case SITE_GOLD_COIN:
			gold_coin_count--;
			break;

		case SITE_ITEM:
			item_count--;
			break;

		case SITE_WEAPON_BLUEPRINT:
			weapon_blueprint_count--;
			break;
	}

	//-------------------------------//

	sitePtr->deinit();

	linkout(siteRecno);

	if( siteRecno == site_array.selected_recno )
		site_array.selected_recno = 0;
}
//--------- End of function SiteArray::del_site ----------//


//--------- Begin of function SiteArray::generate_raw_site ----------//
//
// Generate raw sites. This function is both called at the beginning
//	of a game and when existing raw sites are being used up.
//
// [int] rawGenCount - no. of raw sites to be generated.
//							  (if this is not given, it will use the existing std_raw_site_count)
//
void SiteArray::generate_raw_site(int rawGenCount)
{
	if( rawGenCount )
		std_raw_site_count = rawGenCount;		// use this number for determing whether new sites should emerge in the future

	#define MAX_RAW_REGION 			  3		// maximum no. of regions that has raw sites
	#define SMALLEST_RAW_REGION	 50 		// only put raw on the region if its size is larger than this
	#define REGION_SIZE_PER_RAW  	 100

	//----- count the no. of existing raw sites -------//

	Site* sitePtr;
	int   existRawSiteCount=0;

	for( int i=size() ; i>0 ; i-- )
	{
		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( sitePtr->site_type == SITE_RAW &&
			 sitePtr->reserve_qty >= EXIST_RAW_RESERVE_QTY )
		{
			existRawSiteCount++;
		}
	}

	if( existRawSiteCount >= std_raw_site_count )
		return;

	//----- check which regions are valid for raw sites -----//

	int regionCount = min( MAX_RAW_REGION, region_array.region_info_count );
	int validRegionCount, totalValidSize=0;
	RegionInfo* regionInfo;

	for( validRegionCount=0 ; validRegionCount<regionCount ; validRegionCount++ )
	{
		regionInfo = region_array.get_sorted_region(validRegionCount+1);

		if( regionInfo->region_type != REGION_LAND )
			continue;

		if( regionInfo->region_size < SMALLEST_RAW_REGION )
			break;

		totalValidSize += regionInfo->region_size;
	}

	if( validRegionCount==0 )	// valid regions are those that are big enough to put raw sites
		return;

	//----- count the no. of existing raw sites in each ragion ------//

	int   regionId;
	char	regionRawCountArray[MAX_REGION];

	memset( regionRawCountArray, 0, sizeof(regionRawCountArray) );

	for( i=size() ; i>0 ; i-- )
	{
		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( sitePtr->site_type == SITE_RAW &&
			 sitePtr->reserve_qty >= EXIST_RAW_RESERVE_QTY )
		{
			regionId = world.get_region_id(sitePtr->map_x_loc, sitePtr->map_y_loc);

			regionRawCountArray[regionId-1]++;

		}
	}

	//--------- generate raw sites now ----------//

	int avgValidSize = min( 10000, totalValidSize / std_raw_site_count );
	int j, createCount;

	err_when( validRegionCount > region_array.region_info_count || validRegionCount > MAX_RAW_REGION );

	for( int k=0 ; k<10 ; k++ )		//	one loop may not be enough to generate all raw sites, have more loops to make sure all are generated
	{
		for( i=0 ; i<regionCount ; i++ )
		{
			regionInfo = region_array.get_sorted_region(i+1);

			if( regionInfo->region_type != REGION_LAND )
				continue;

			if( regionInfo->region_size < SMALLEST_RAW_REGION )
				break;

			createCount = regionInfo->region_size / avgValidSize;
			createCount = max(1, createCount);

			//--------- create now --------//

			for( j=regionRawCountArray[regionInfo->region_id-1] ; j<createCount ; j++ )	// if currently there are already some, don't create new ones
			{
				if( create_raw_site(regionInfo->region_id) )
				{
					if( ++existRawSiteCount == std_raw_site_count )
						return;
				}
			}
		}
	}
}
//--------- End of function SiteArray::generate_raw_site ----------//


//--------- Begin of function SiteArray::create_raw_site ----------//
//
// <int> regionId  - if this parameter is given, the raw site
//							will be built on this region.
// [int] townRecno - if this parameter is given, the raw site
//							will be built near this town.
//
// return: <int> >0 - the recno of the site
//					  0  - no site is added
//
int SiteArray::create_raw_site(int regionId, int townRecno)
{
	//-------- count the no. of each raw material -------//

	Site* sitePtr;
	short rawCountArray[MAX_RAW];

	memset( rawCountArray, 0, sizeof(rawCountArray) );

	for( int i=size(); i>0 ; i-- )
	{
		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( sitePtr->site_type == SITE_RAW )
		{
			err_when( sitePtr->object_id < 1 || sitePtr->object_id > MAX_RAW );

			rawCountArray[ sitePtr->object_id-1 ]++;
		}
	}

	//---- find the minimum raw count ----//

	int minCount=0xFFFF;

	for( i=0 ; i<MAX_RAW ; i++ )
	{
		if( rawCountArray[i] < minCount )
			minCount = rawCountArray[i];
	}

	//----- pick a raw material type -----//

	int rawId = m.random(MAX_RAW)+1;

	for( i=0 ; i<MAX_RAW ; i++ )
	{
		if( ++rawId > MAX_RAW )
			rawId = 1;

		if( rawCountArray[rawId-1] == minCount )	// don't use this raw type unless it is one of the less available ones.
			break;
	}

	//--------- create the raw site now ------//

	int locX1, locY1, locX2, locY2;
	int maxTries;

	int siteWidth = raw_res[rawId]->map_loc_width;
	int siteHeight = raw_res[rawId]->map_loc_height;

	if( townRecno )
	{
		// ####### begin Gilbert 26/5 ########//
//		#define MAX_TOWN_SITE_DISTANCE   (MAX_LINKED_FIRM_TOWN-STD_TOWN_LOC_WIDTH)
		const maxTownSiteDistance = world.effective_distance(FIRM_MINE, 0);

		Town* townPtr = town_array[townRecno];

		locX1 = townPtr->center_x - maxTownSiteDistance - (siteWidth-1)/2;	// (siteWidth-1)/2 is the distance from mine center_x to mine loc_x1
		locX2 = townPtr->center_x + maxTownSiteDistance + (siteWidth+1)/2;
		locY1 = townPtr->center_y - maxTownSiteDistance - (siteHeight-1)/2;
		locY2 = townPtr->center_y + maxTownSiteDistance + (siteHeight+1)/2;
		// ####### end Gilbert 26/5 ########//

		if(locX1<0)
			locX1 = 0;
		else if(locX2>=MAX_WORLD_X_LOC)
			locX2 = MAX_WORLD_X_LOC-1;

		if(locY1<0)
			locY1 = 0;
		else if(locY2>=MAX_WORLD_Y_LOC)
			locY2 = MAX_WORLD_Y_LOC-1;

		maxTries = (locX2-locX1+1)*(locY2-locY1+1);
		regionId = townPtr->region_id;
	}
	else
	{
		locX1 = 0;
		locY1 = 0;
		locX2 = MAX_WORLD_X_LOC-1;
		locY2 = MAX_WORLD_Y_LOC-1;

		maxTries = 10000;
	}

	//----- randomly locate a space to add the site -----//

	int mineSize = firm_res[FIRM_MINE]->loc_width;
	err_when( firm_res[FIRM_MINE]->loc_height > mineSize );		// assume square size

	if( world.locate_space_random(locX1, locY1, locX2, locY2,
		 mineSize+INTER_PLACE_SPACE*2, mineSize+INTER_PLACE_SPACE*2, maxTries, regionId, 1) )
	{
		if( world.can_build_firm( locX1+INTER_PLACE_SPACE, locY1+INTER_PLACE_SPACE, FIRM_MINE)	 )	
		{
			int reserveQty = MAX_RAW_RESERVE_QTY * (60 + m.random(40)) / 100;
			return add_site( locX1+INTER_PLACE_SPACE, locY1+INTER_PLACE_SPACE, SITE_RAW, rawId, reserveQty );		// xLoc+1 & yLoc+1 as the located size is 3x3, the raw site is at the center of it
		}
	}

	return 0;
}
//--------- End of function SiteArray::create_raw_site ----------//


//--------- Begin of function SiteArray::scan_site ----------//
//
// Scan for the a site that is closest the given location.
//
// <int> xLoc, yLoc - the location given.
// [int] siteType	  - if given, only scan for this site (default: 0)
//
// return: <int> nearestSiteRecno - the recno of the raw materials
//											   that is nearest to the given location.
//
int SiteArray::scan_site(int xLoc, int yLoc, int siteType)
{
	Site* sitePtr;
	int   siteDis, minDis=0x7FFFFFFF, nearestSiteRecno=0;

	for( int i=site_array.size() ; i>0 ; i-- )
	{
		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( siteType==0 || sitePtr->site_type==siteType )
		{
			siteDis = m.points_distance( xLoc, yLoc, sitePtr->map_x_loc, sitePtr->map_y_loc );

			if( siteDis < minDis )
			{
				minDis 		 	  = siteDis;
				nearestSiteRecno = i;
			}
		}
	}

	return nearestSiteRecno;
}
//---------- End of function SiteArray::scan_site -----------//


//--------- Begin of function SiteArray::next_day ----------//
//
void SiteArray::next_day()
{
	if( info.game_date%30 == 0 )
	{
		generate_raw_site();		// check if we need to generate existing raw sites are being used up and if we need to generate new ones
	}

	//-- if there is any scroll or gold coins available, ask AI to get them --//

	// #### begin Gilbert 3/11 ######//
	if(scroll_count || gold_coin_count || item_count || weapon_blueprint_count )
	// #### end Gilbert 3/11 ######//
	{
		int aiGetSiteObject = (info.game_date%5 == 0);

		Site* sitePtr;
		Location *locPtr;

		for(int i=size(); i; i--)
		{
			if(is_deleted(i))
				continue;

			sitePtr = site_array[i];

			switch(sitePtr->site_type)
			{
				case SITE_SCROLL:
				case SITE_GOLD_COIN:
				case SITE_ITEM:
				case SITE_WEAPON_BLUEPRINT:
					locPtr = world.get_loc(sitePtr->map_x_loc, sitePtr->map_y_loc);

					//---- if the unit is standing on a scroll site -----//

					if(locPtr->unit_recno(UNIT_LAND))
					{
						// ####### begin Gilbert 25/5 ##########//
						int unitRecno = locPtr->unit_recno(UNIT_LAND);
						if( !unit_array.is_deleted(unitRecno) )
							sitePtr->get_site_object( unitRecno );
						// ####### end Gilbert 25/5 ##########//
					}
					else if(aiGetSiteObject)
					{
						sitePtr->ai_get_site_object();
               }
					break;
			}
		}
	}

	//-------- debug testing --------//

#ifdef DEBUG

	if( info.game_date%10 == 0 )
	{
		Site* sitePtr;
		Location* locPtr;

		for( int i=1 ; i<=size() ; i++ )
		{
			if( site_array.is_deleted(i) )
				continue;

			sitePtr = site_array[i];

			locPtr = world.get_loc( sitePtr->map_x_loc, sitePtr->map_y_loc );

			err_when( !locPtr->has_site() );
			err_when( locPtr->site_recno() != i );

			if( sitePtr->has_mine )
			{
				err_when( !locPtr->is_firm() );
				err_when( firm_array[locPtr->firm_recno()]->firm_id != FIRM_MINE
					&& firm_array[locPtr->firm_recno()]->firm_id != FIRM_ALCHEMY );
			}
			else
			{
				// disable because a mine can be upon more than one mine
				// err_when( locPtr->is_firm() || locPtr->is_town() );
			}
		}
	}
#endif
}
//--------- End of function SiteArray::next_day ----------//


//--------- Begin of function SiteArray::ai_get_site_object -------//
//
// Notify AI units to acquire scrolls or gold coins available on the
// map.
//
void SiteArray::ai_get_site_object()
{
	Site* sitePtr;

	for( int i=1 ; i<=size() ; i++ )
	{
		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( sitePtr->site_type == SITE_SCROLL ||
			 sitePtr->site_type == SITE_GOLD_COIN ||
			 sitePtr->site_type == SITE_ITEM ||
			 sitePtr->site_type == SITE_WEAPON_BLUEPRINT )
		{
			sitePtr->ai_get_site_object();
		}
	}
}
//----------- End of function SiteArray::ai_get_site_object -------//


//------- Begin of function Site::ai_get_site_object -------//
//
// Ask AI units around to get the object on this site.
//
int Site::ai_get_site_object()
{
	#define MAX_NOTIFY_GET_RANGE 	30		// only notify units within this range
	#define MAX_UNIT_TO_ORDER  	5

	int		 xOffset, yOffset;
	int		 xLoc, yLoc;
	Location* locPtr;
	Unit*	    unitPtr;
	int		 unitOrderedCount=0;
	int 		 siteRaceId = 0;

	int		 notifyGetRange = 10 + (info.game_date-info.game_start_date) / 60;		// it takes over 3 years to reach the maximum range 30

	notifyGetRange = min(notifyGetRange, MAX_NOTIFY_GET_RANGE);

	if( site_type == SITE_SCROLL )
		siteRaceId = god_res[object_id]->race_id;

	for( int i=2 ; i<notifyGetRange*notifyGetRange ; i++ )
	{
		m.cal_move_around_a_point(i, notifyGetRange, notifyGetRange, xOffset, yOffset);

		xLoc = map_x_loc + xOffset;
		yLoc = map_y_loc + yOffset;

		xLoc = max(0, xLoc);
		xLoc = min(MAX_WORLD_X_LOC-1, xLoc);

		yLoc = max(0, yLoc);
		yLoc = min(MAX_WORLD_Y_LOC-1, yLoc);

		locPtr = world.get_loc(xLoc, yLoc);

		if( !locPtr->unit_recno(UNIT_LAND) )
			continue;

		//------------------------------//

		int unitRecno = locPtr->unit_recno(UNIT_LAND);

		if( unit_array.is_deleted(unitRecno) )
			continue;

		unitPtr = unit_array[unitRecno];

		if( !unitPtr->race_id || unitPtr->is_civilian() )		// only military units of humans or monsters
			continue;

		if( unitPtr->item.id )		// the unit alrady carries an item 
			continue;

		if( !unitPtr->is_ai || unitPtr->cur_order.ai_action_id )
			continue;

		if( siteRaceId && siteRaceId != unitPtr->race_id )
			continue;

		unitPtr->move(map_x_loc, map_y_loc);

		//--- if the unit is just standing next to the site ---//

		if( abs(map_x_loc-xLoc)<=1 && abs(map_y_loc-yLoc)<=1 )
		{
			return 1;
		}
		else		// order more than one unit to get the site at the same time
		{
			if( ++unitOrderedCount >= MAX_UNIT_TO_ORDER )
				return 1;
		}
	}

	return 0;
}
//-------- End of function Site::ai_get_site_object -------//


//--------- Begin of function SiteArray::go_to_a_raw_site -------//
//
// Go to an untapped raw site.
//
void SiteArray::go_to_a_raw_site()
{
	//----- try to locate an untapped raw site -----//

	Site* sitePtr;
	int arraySize = size();
	int i = selected_recno ? selected_recno : 0;

	for( int j=1 ; j<=arraySize ; j++ )
	{
		if( ++i > arraySize)
			i = 1;

		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( !sys.debug_session )		// in debug session, browse through all types of sites
		{
			if( sitePtr->site_type != SITE_RAW )
				continue;
		}

		//----------------------------------//

		if( !sitePtr->has_mine )
		{
			if( world.is_explored_quick(sitePtr->map_x_loc, sitePtr->map_y_loc, sitePtr->map_x2, sitePtr->map_y2) )
			{
				world.go_loc( sitePtr->map_x_loc, sitePtr->map_y_loc, GO_LOC_SELECT_SITE );		// 1-select the object on the location
				return;
			}
		}
	}

	//---- if no untapped raw sites left, jump to built mines ----//

	i = 1;

	if(firm_array.selected_recno)
	{
		//------- get the site_recno if a mine is selected ---------//

		Firm *firmPtr = firm_array[firm_array.selected_recno];

		if(firmPtr->firm_id==FIRM_MINE || firmPtr->firm_id==FIRM_ALCHEMY)
		{
			int x1 = firmPtr->loc_x1;
			int y1 = firmPtr->loc_y1;
			int x2 = firmPtr->loc_x2;
			int y2 = firmPtr->loc_y2;

			for(int count=1; count<=arraySize; ++count)
			{
				if(site_array.is_deleted(count))
					continue;

				sitePtr = site_array[count];

				if(sitePtr->map_x_loc>=x1 && sitePtr->map_x_loc<=x2 &&
					sitePtr->map_y_loc>=y1 && sitePtr->map_y_loc<=y2)
				{
					i = count;
					break;
				}
			}
		}
	}

	for( j=1 ; j<=arraySize ; j++ )
	{
		if(++i > arraySize)
			i = 1;

		if( site_array.is_deleted(i) )
			continue;

		sitePtr = site_array[i];

		if( world.get_loc(sitePtr->map_x_loc, sitePtr->map_y_loc)->explored() )
		{
			world.go_loc( sitePtr->map_x_loc, sitePtr->map_y_loc, 1 );		// 1-select the object on the location
			return;
		}
	}
}
//----------- End of function SiteArray::go_to_a_raw_site -------//


//--------- Begin of function Site::init ----------//
//
void Site::init(int siteRecno, int siteType, int xLoc, int yLoc)
{
	site_recno  = siteRecno;
	site_type   = siteType;
	map_x_loc   = xLoc;
	map_y_loc   = yLoc;

	if( site_type == SITE_RAW )
	{
		RawInfo *rawInfo = raw_res[siteType];
		map_x2 = map_x_loc + rawInfo->map_loc_width - 1;
		map_y2 = map_y_loc + rawInfo->map_loc_height - 1;
	}
	else
	{
		map_x2 = map_x_loc;
		map_y2 = map_y_loc;
	}

	err_when( map_x2 >= MAX_WORLD_X_LOC || map_y2 >= MAX_WORLD_Y_LOC );
	altitude = world.get_corner(map_x2+1, map_y2+1)->get_altitude();
	has_mine    = 0;

	//------- set world's location --------//

	Location* locPtr = world.get_loc(xLoc, yLoc);

	region_id = locPtr->region_id;
	for( yLoc = map_y_loc; yLoc <= map_y2; ++yLoc )
	{
		for( xLoc = map_x_loc; xLoc <= map_x2; ++xLoc )
		{
			world.get_loc(xLoc, yLoc)->set_site(siteRecno);
		}
	}

	animation_count = 100;
}
//---------- End of function Site::init -----------//


//--------- Begin of function Site::deinit ----------//
//
void Site::deinit()
{
	//------ reset world's location ---------//
	int xLoc, yLoc;
	for( yLoc = map_y_loc; yLoc <= map_y2; ++yLoc )
	{
		for( xLoc = map_x_loc; xLoc <= map_x2; ++xLoc )
		{
			world.get_loc(xLoc, yLoc)->remove_site();
		}
	}
}
//---------- End of function Site::deinit -----------//


//--------- Begin of function Site::get_site_object ----------//
//
// An unit takes the object from the site
//
// <int> unitRecno - recno of the unit that takes the object on the site.
//
int Site::get_site_object(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];
	int   objectTaken=0;

	if( !unitPtr->nation_recno )
		return 0;

	//----- if this is a scroll site ------//

	if( site_type == SITE_SCROLL )
	{
		if( god_res[object_id]->race_id == unitPtr->race_id )
		{
			god_res[object_id]->enable_know(unitPtr->nation_recno);

			objectTaken = 1;

			news_array.scroll_acquired(unitPtr->nation_recno, god_res[object_id]->race_id );
		}
	}

	//------ if there are gold coins on this site -----//

	if( site_type == SITE_GOLD_COIN )
	{
		nation_array[unitPtr->nation_recno]->add_income(INCOME_TREASURE, object_id);
		// ##### begin Gilbert 15/7 ########//
		if( nation_array[unitPtr->nation_recno]->is_monster() )
			nation_array[unitPtr->nation_recno]->change_live_points( (float) reserve_qty );
		// ##### end Gilbert 15/7 ########//
		objectTaken = 1;

		if( unitPtr->nation_recno == nation_array.player_recno )
			news_array.monster_gold_acquired(object_id);
	}

	//------ if there is item on this site -----//

	if( site_type == SITE_ITEM )
	{
		if( item_res.can_pick(object_id, unitRecno) &&		// can pick even cannot use
			unitPtr->pick_item(object_id, reserve_qty) )
		{
			objectTaken = 1;
		}
	}

	if( site_type == SITE_WEAPON_BLUEPRINT )
	{
		if( unitPtr->nation_recno )
		{
			if( tech_res[object_id]->get_nation_tech_level(unitPtr->nation_recno) == 0 )
			{
				tech_res[object_id]->set_nation_tech_level(unitPtr->nation_recno, 1);
			}

			// ######## begin Gilbert 15/2 #########//
			news_array.blueprint_acquired(unitPtr->nation_recno, object_id );
			// ######## end Gilbert 15/2 #########//

			objectTaken = 1;
		}
	}

	//---- if the object has been taken by the unit ----//

	if( objectTaken )
	{
		// ###### begin Gilbert 23/2 #########//

		// --------- update win/lose condition for scroll taken ------//

		game.update_condition_site_taken( site_recno, unitPtr->nation_recno );
		// ###### end Gilbert 23/2 #########//
		site_array.del_site(site_recno);
		return 1;
	}

	return 0;
}
//---------- End of function Site::get_site_object -----------//


// ------- begin of function Rock::is_stealth -------//
//
int Site::is_stealth()
{
	return config.blacken_map && !world.is_explored_quick( map_x_loc, map_y_loc, map_x2, map_y2 );
}
// ------- end of function Rock::is_stealth -------//


//------- Begin of function SiteArray::is_deleted -----//

int SiteArray::is_deleted(int recNo)
{
	Site* sitePtr = (Site*) get(recNo);

	return !sitePtr || sitePtr->site_type==0;
}
//--------- End of function SiteArray::is_deleted ----//

#ifdef DEBUG

//------- Begin of function SiteArray::operator[] -----//

Site* SiteArray::operator[](int recNo)
{
	Site* sitePtr = (Site*) get(recNo);

	if( !sitePtr || sitePtr->site_type==0 )
		err.run( "SiteArray[] is deleted" );

	return sitePtr;
}
//--------- End of function SiteArray::operator[] ----//


//------- Begin of function SiteArray::operator() -----//

Site* SiteArray::operator()()
{
	Site* sitePtr = (Site*) get();

	if( !sitePtr || sitePtr->site_type==0 )
		err.run( "SiteArray[recno()] is deleted" );

	return sitePtr;
}
//--------- End of function SiteArray::operator() ----//

#endif
