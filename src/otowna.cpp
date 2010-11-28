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

//Filename    : OTOWNA.CPP
//Description : Object Town Array

#include <otown.h>
#include <obox.h>
#include <oinfo.h>
#include <olog.h>
#include <oconfig.h>
#include <osys.h>
#include <of_mark.h>
#include <onation.h>
#include <ogame.h>
#include <ovga.h>
#include <omodeid.h>
#include <oworldmt.h>
#include <stdlib.h>
#include <oworld.h>
#include <of_lair.h>

#ifdef DEBUG
#include <ofont.h>
static unsigned long	last_town_ai_profile_time = 0L;
static unsigned long	town_ai_profile_time = 0L;
static unsigned long	last_town_profile_time = 0L;
static unsigned long	town_profile_time = 0L;
#endif

//---------- define constant ------------//

#define TOWN_BUILDING_COST		200

//--------- Begin of function TownArray::TownArray ----------//

TownArray::TownArray() : DynArrayB(sizeof(Town*), 10, DEFAULT_REUSE_INTERVAL_DAYS)
{
}
//--------- End of function TownArray::TownArary ----------//


//------- Begin of function TownArray::~TownArray ----------//
//
TownArray::~TownArray()
{
	deinit();
}
//--------- End of function TownArray::~TownArray ----------//


//--------- Begin of function TownArray::init ----------//
//
void TownArray::init()
{
	memset( race_wander_pop_array, 0, sizeof(race_wander_pop_array) );
}
//---------- End of function TownArray::init ----------//


//--------- Begin of function TownArray::deinit ----------//
//
void TownArray::deinit()
{
	//----- delete Town objects ------//

	if( size() > 0 )
	{
		for( int i=1 ; i<=size() ; i++ )
		{
			if( !town_array.is_deleted(i) )
				town_array.del_town(i);
		}

		zap();
	}
}
//---------- End of function TownArray::deinit ----------//


//----- Begin of function TownArray::create_town -------//
//
// Create a blank town object and link it into town_array.
//
Town* TownArray::create_town()
{
	Town* townPtr;

	townPtr = new Town;

	linkin(&townPtr);

	return townPtr;
}
//------- End of function TownArray::create_town -------//


//----- Begin of function TownArray::generate_town -------//
//
// Generate a town for new games or when editing scenarios.
//
// <int> nationRecno - the nation recno
// <int> raceId 	   - the race of the majority of the town poulation
// <int> xLoc, yLoc  - location of the town
//
int TownArray::generate_town(int nationRecno, int raceId, int xLoc, int yLoc)
{
	return add_town(nationRecno, raceId, xLoc, yLoc, 0, true);
}
//------- End of function TownArray::generate_town -------//


//----- Begin of function TownArray::add_town -------//
//
// <int> nationRecno - the nation recno
// <int> raceId 	   - the race of the majority of the town poulation
// <int> xLoc, yLoc  - location of the town
// [short] builderRecno = recno of the builder unit
// [bool] noCost		= no cost for building the firm (default: false)
//
int TownArray::add_town(int nationRecno, int raceId, int xLoc, int yLoc, int builderRecno, bool noCost)
{
	if( !world.can_build_town(xLoc, yLoc, builderRecno) )
	{
		err_here();
		return 0;
	}

	//----------------------------------------//

	Town* townPtr;

	townPtr = new Town;

	linkin(&townPtr);

	townPtr->town_recno = recno();

	base_obj_array.add( townPtr );

	townPtr->init(nationRecno, raceId, xLoc, yLoc, builderRecno);

	nation_array.update_statistic();		// update largest_town_recno

	//----------- add building cost ----------//

	if( nationRecno && !noCost )
		nation_array[nationRecno]->add_expense( EXPENSE_TOWN, TOWN_BUILDING_COST );

//	world.plant_limit = world.plant_limit - m.random(10);

	return recno();
}
//------- End of function TownArray::add_town -------//


//----- Begin of function TownArray::del_town -------//
//
void TownArray::del_town(int townRecno)
{
	Town* townPtr = operator[](townRecno);

	//--- del its link from base_obj_array ----//

	base_obj_array.del(townPtr->base_obj_recno);

	//---- delete the object and detach it from town_array ---//

	townPtr->deinit();
	delete townPtr;

	linkout(townRecno);

	if( !sys.quick_exit_flag() )
	{
		nation_array.update_statistic();
	
//		world.plant_limit = world.plant_limit + m.random(5);
	}
}
//------- End of function TownArray::del_town -------//


//--------- Begin of function TownArray::process ---------//
//
// Process all town in town_array for action and movement for next frame
//
void TownArray::process()
{
	//----- call Town::next_day --------//

	int  i;
	Town *townPtr;

	for( i=size() ; i>0 ; i-- )
	{
		townPtr = (Town*) get_ptr(i);

		if( !townPtr )
			continue;

		err_when(town_array.is_deleted(i));

		//------- if the town is under construction ------//

		if( townPtr->under_construction )
		{
			townPtr->process_construction();
			continue;
		}

		// ####### begin Gilbert 28/12 #######//
		// ------- every frame process --------//

		townPtr->process();
		// ####### end Gilbert 28/12 #######//

		//---------------------------------------//

		if( i%FRAMES_PER_DAY == int(sys.frame_count%FRAMES_PER_DAY) )	// only process each firm once per day
		{
			err_when( townPtr->town_recno==0 );

			if( townPtr->nation_recno==0 )
			{
				townPtr->think_independent_town();
			}
			else
			{
				#ifdef DEBUG
				if(!config.disable_ai_flag && townPtr->ai_town)
				#else
				if( townPtr->ai_town )
				#endif
				{
					#ifdef DEBUG
					unsigned long profileStartTime = m.get_time();
					#endif

					townPtr->process_ai();

					#ifdef DEBUG
					town_profile_time += m.get_time() - profileStartTime;
					#endif
				}
			}

			if( town_array.is_deleted(i) )
				continue;

			err_when( townPtr->town_recno==0 );

			#ifdef DEBUG
			unsigned long profileStartTime = m.get_time();
			#endif

			townPtr->next_day();

			#ifdef DEBUG
			town_profile_time += m.get_time() - profileStartTime;
			#endif
		}
	}

	//------ distribute demand -------//

	if( sys.day_frame_count==0 && info.game_date%15==0 )			// distribute demand every 15 days
		distribute_demand();

	//------ create new independent town -----//

	if( info.game_date%30==0 && sys.frame_count%FRAMES_PER_DAY==0 )
		think_new_independent_town();
}
//----------- End of function TownArray::process ---------//


//--------- Begin of function TownArray::next_month ---------//
//
void TownArray::next_month()
{
	int	 i;
	Town*  townPtr;

	LOG_MSG("begin TownArray::next_month");
	LOG_MSG(m.get_random_seed() );
	for(i=1; i <=size() ; i++)
	{
		townPtr = (Town*)get_ptr(i);

		if( townPtr && !townPtr->under_construction )
		{
			LOG_MSG("Town next_month");
			LOG_MSG( i );
			townPtr->next_month();
			LOG_MSG(m.get_random_seed() );
		}
	}
	LOG_MSG("end TownArray::next_month");
	LOG_MSG(m.get_random_seed() );
}
//----------- End of function TownArray::next_month -----------//


//----- Begin of function TownArray::think_new_independent_town -----//
//
// Think about creating new independent towns.
//
void TownArray::think_new_independent_town()
{
	if( !config.new_independent_town_emerge )
		return;

	if( m.random(3) != 0 )		// 1/3 chance
		return;

	//---- count the number of independent towns ----//

	Town* townPtr;
	int   independentTownCount=0, allTotalPop=0;

	int i;
	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		allTotalPop += townPtr->population;

		if( townPtr->nation_recno == 0 )
			independentTownCount++;
	}

	if( independentTownCount >= 10 )		// only when the no. of independent town is less than 10
		return;

	//--- if the total population of all nations combined > 1000, then no new independent town will emerge ---//

	if( allTotalPop > 1000 )
		return;

	//--- add 1 to 2 wanderer per month per race ---//

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		race_wander_pop_array[i] += 2+m.random(5);
	}

	//----- check if there are enough wanderers to set up a new town ---//

	int raceId = m.random(MAX_RACE)+1;

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( ++raceId > MAX_RACE )
			raceId = 1;

		if( race_wander_pop_array[raceId-1] >= 10 )	// one of the race must have at least 10 people
			break;
	}

	if( i==MAX_RACE )
		return;

	//------- locate for a space to build the town ------//

	int xLoc, yLoc;

	if( !think_town_loc(MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC/4, xLoc, yLoc) )
		return;

	//--------------- create town ---------------//

	int townRecno  = town_array.generate_town(0, raceId, xLoc, yLoc);
	int maxTownPop = 20 + m.random(10);
	int addPop, townResistance;
	int loopCount=0;

	townPtr = town_array[townRecno];

	err_when( loopCount++ > 100 );

	addPop = race_wander_pop_array[raceId-1];
	addPop = min(maxTownPop-townPtr->population, addPop);

	townResistance = independent_town_resistance();

	townPtr->init_pop( addPop, townResistance, 0, 1 );		// 0-the add pop do not have jobs, 1-first init
	townPtr->auto_set_layout();

	race_wander_pop_array[raceId-1] -= addPop;

	err_when( race_wander_pop_array[raceId-1] < 0 );
}
//------ End of function TownArray::think_new_independent_town -----//


//----- Begin of function TownArray::independent_town_resistance ------//
//
// Return a random resistance value for new independent town.
//
int TownArray::independent_town_resistance()
{
	switch(config.independent_town_resistance)
	{
		case OPTION_LOW:
			return 50 + m.random(30);
			break;

		case OPTION_MODERATE:
			return 70 + m.random(30);
			break;

		case OPTION_HIGH:
			return 90 + m.random(10);
			break;

		default:
			err_here();
			return 60 + m.random(40);
	}
}
//----- End of function TownArray::independent_town_resistance ------//


//-------- Begin of function TownArray::think_town_loc --------//
//
// Locate for an area of free space
//
// <int>  maxTries       = maximum no. of tries
// <int&> xLoc           = for returning result
// <int&> yLoc           = for returning result
//
// [int]  rangeX1, rangeY1 = the range of the town location.
//			 rangeX2, rangeY2   If not given, it can be on any location.
//
//
// return : <int> 1 - free space found
//                0 - free space found
//
int TownArray::think_town_loc(int maxTries, int& retXLoc, int& retYLoc, int rangeX1, int rangeY1, int rangeX2, int rangeY2)
{
	#define MIN_INTER_TOWN_DISTANCE  16
	#define BUILD_TOWN_LOC_WIDTH     20
	#define BUILD_TOWN_LOC_HEIGHT    16

	int       xLoc, yLoc;
	int       i, canBuildFlag, townRecno;
	Town*     townPtr;

	int		 firstXLoc = -1;
	int		 firstYLoc = -1;

	for( i=0 ; i<maxTries ; i++ )
	{
		if( rangeX1 >= 0 )
		{
			xLoc = rangeX1 + m.random(rangeX2-rangeX1+1-BUILD_TOWN_LOC_WIDTH);
			yLoc = 2 + rangeY1 + m.random(rangeY2-rangeY1+1-BUILD_TOWN_LOC_HEIGHT-2);		// do not build on the upper most location as the flag will go beyond the view area
		}
		else
		{
			xLoc = m.random(MAX_WORLD_X_LOC-BUILD_TOWN_LOC_WIDTH);
			yLoc = 2+m.random(MAX_WORLD_Y_LOC-BUILD_TOWN_LOC_HEIGHT-2);
		}

		canBuildFlag=1;

		//---------- check if the area is all free ----------//

		int x, y;
		Location *locPtr;
		for( y=yLoc ; y<yLoc+BUILD_TOWN_LOC_HEIGHT ; y++ )
		{
			locPtr = world.get_loc(xLoc, y);
			for( x=xLoc ; x<xLoc+BUILD_TOWN_LOC_WIDTH ; x++, locPtr++ )
			{
				if( !locPtr->can_build_town() )
				{
					canBuildFlag=0;
					break;
				}
			}
		}

		if( !canBuildFlag )
			continue;

		if( !world.can_build_town(xLoc+INTER_PLACE_SPACE, yLoc+INTER_PLACE_SPACE) )
			continue;
		
		if( firstXLoc == -1 && firstYLoc == -1 )
		{
			// mark any location has space

			firstXLoc = xLoc;
			firstYLoc = yLoc;
		}

		//-------- check if it's too close to other towns --------//

		for( townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
		{
			if( town_array.is_deleted(townRecno) )
				continue;

			townPtr = town_array[townRecno];

			if( m.points_distance(xLoc+1, yLoc+1, townPtr->center_x,		// xLoc+1 and yLoc+1 to take the center location of the town
				 townPtr->center_y) < MIN_INTER_TOWN_DISTANCE )
			{
				break;
			}
		}

		if( townRecno > 0 )	// if it's too close to other towns
			continue;
/*
		//-------- check if it's too close to monster firms --------//

		for( firmRecno=firm_array.size() ; firmRecno>0 ; firmRecno-- )
		{
			if( firm_array.is_deleted(firmRecno) )
				continue;

			firmPtr = firm_array[firmRecno];

			if( m.points_distance(xLoc+1, yLoc+1, firmPtr->center_x,
				 firmPtr->center_y) < MONSTER_ATTACK_NEIGHBOR_RANGE )
			{
				break;
			}
		}

		if( firmRecno > 0 )     // if it's too close to monster firms
			continue;
*/
		//----------------------------------------//

		
		retXLoc = xLoc + INTER_PLACE_SPACE;
		retYLoc = yLoc + INTER_PLACE_SPACE;
		return 1;
	}

	if( firstXLoc == -1 && firstYLoc == -1 )
		return 0;
	else
	{
		// ##### begin Gilbert 14/10 ######//
		retXLoc = firstXLoc + INTER_PLACE_SPACE;
		retYLoc = firstYLoc + INTER_PLACE_SPACE;
		// ##### end Gilbert 14/10 ######//
		return 1;
	}
}
//--------- End of function TownArray::think_town_loc ---------//


//--------- Begin of function TownArray::draw ---------//
//
// Draw town towns on the zoom map.
//
void TownArray::draw()
{
	int  i;
	Town *townPtr;

	for( i=size() ; i>0 ; i-- )
	{
		townPtr = (Town*) get_ptr(i);

		if( townPtr )
			townPtr->draw();
	}
}
//----------- End of function TownArray::draw ---------//


//--------- Begin of function TownArray::draw_dot ---------//
//
// Draw tiny dots on map window representing the location of the town town.
//
// [int] filterRaceId - if given, only display towns whose race is filterRaceId
//
void TownArray::draw_dot(int filterRaceId)
{
	int	  		i, x, y;
	short			nationColor;
	int			vgaBufPitch = vga_back.buf_pitch();
	int			townNation;

	// ----- prepare color -------//

	short unexploredColor = vga_back.translate_color(UNEXPLORED_COLOR);
	short shadowColor = vga_back.translate_color(VGA_GRAY+1);
	short	nationColorArray[MAX_NATION+1];
	for( int n = 0; n < MAX_NATION+1; ++n )
		nationColorArray[n] = vga_back.translate_color(nation_array.nation_color_array[n]);

	// ##### begin Gilbert 2/11 ######//
	short	excitedColorArray[MAX_NATION+1];  //[excitedColorCount];
	short	slowExcitedColorArray[MAX_NATION+1];  //[excitedColorCount];

	for( i = 0; i <= MAX_NATION; ++i )
	{
		if( i == 0 || !nation_array.is_deleted(i) )
		{
			short *remapTable = game.get_color_remap_table(i, 0);
			excitedColorArray[i] = remapTable[0xe0 + (sys.frame_count % 4)];
			slowExcitedColorArray[i] = remapTable[0xe0 + (sys.frame_count % 8)/2];
		}
		else
		{
			excitedColorArray[i] = 
			slowExcitedColorArray[i] = vga_back.translate_color(V_WHITE);
		}
	}

	// ------ draw dots for each town --------//
	for(i=1; i <=size() ; i++)
	{
		short *rowWritePtr;

		{	// minimize register variable at one time
			register Town *townPtr = (Town*) get_ptr(i);

			if( !townPtr)
				continue;

			townNation = townPtr->nation_recno;

			// modify the nationRecno for slave town
			if (townPtr->is_pay_tribute_to_monster())
			{
				Firm* firmPtr;
				for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
				{
					firmPtr = firm_array[townPtr->linked_firm_array[j]];

					if( firmPtr->firm_id == FIRM_LAIR &&
						 firmPtr->nation_recno &&
						 !firmPtr->under_construction &&
						 townPtr->resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
						
						townNation = firmPtr->nation_recno;
				}
			}

			if( town_array.selected_recno != i )		// if the town is currently selected, ignore all filters
			{
//				if( filterRaceId && townPtr->race_id != filterRaceId )
//					continue;

				// filter selected nation
				if( world.map_matrix->filter_nation_flag && world.map_matrix->filter_nation_recno != townPtr->nation_recno )
					continue;
			}

			//-------------------------------------//

			if( world.map_matrix->map_mode != MAP_MODE_TRADE )
			{
				// if under attack, flash

				if( info.game_date - townPtr->last_attack_date <= 2 )
					nationColor = excitedColorArray[townNation];
				else if( selected_recno == i )
					nationColor = slowExcitedColorArray[townNation];
				else
					nationColor = nationColorArray[townNation];
			}
			else
			{
				// if selected mine, factory or market linked to this town, flash

				Firm *firmPtr;
				if( firm_array.selected_recno
					&& (firmPtr = firm_array[firm_array.selected_recno])
					&& (firmPtr->cast_to_FirmMine() || firmPtr->cast_to_FirmFactory() || firmPtr->cast_to_FirmMarket())
					&& townPtr->get_linked_firm_status(firm_array.selected_recno) == LINK_EE )
				{
					nationColor = slowExcitedColorArray[townNation];
				}
				else
				{
					nationColor = nationColorArray[townNation];
				}
			}

			rowWritePtr = vga_back.buf_ptr( 
				world.map_matrix->calc_loc_pos_x(townPtr->loc_x1, townPtr->loc_y1) + MAP_X1,
				world.map_matrix->calc_loc_pos_y(townPtr->loc_x1, townPtr->loc_y1) + MAP_Y1 );
		}

		{ // minimize register variable at one time
			register short *writePtr;

			for( y = 0; y < STD_TOWN_LOC_HEIGHT; (rowWritePtr+=y&1?vgaBufPitch:-1), ++y )
			{
				writePtr = rowWritePtr;
				for( x = 0; x < STD_TOWN_LOC_WIDTH; (writePtr+=vgaBufPitch+1),x+=2 )
				{
					if( *writePtr != unexploredColor )
						*writePtr = nationColor;
				}
				if( *writePtr != unexploredColor)
					*writePtr = shadowColor;
			}
			writePtr = rowWritePtr;
			for( x = 0; x < STD_TOWN_LOC_WIDTH; (writePtr+=vgaBufPitch+1),x+=2 )
			{
				if( *writePtr != unexploredColor )
					*writePtr = shadowColor;
			}
		}
	} // end for
	// ##### end Gilbert 2/11 ######//
}
//----------- End of function TownArray::draw_dot -----------//

//--------- Begin of function TownArray::draw_profile ---------//
void TownArray::draw_profile()
{
#ifdef DEBUG
	static unsigned long lastDrawTime = m.get_time();

	if(m.get_time() >= lastDrawTime + 1000)
	{
		last_town_ai_profile_time = town_ai_profile_time;
		town_ai_profile_time = 0L;
		last_town_profile_time = town_profile_time;
		town_profile_time = 0L;
		lastDrawTime = m.get_time();
	}

	String str;
	str  = "Town  : ";
	font_news.disp( ZOOM_X1+10, ZOOM_Y1+90, str, MAP_X2);

	str = "";
	str += last_town_ai_profile_time;
	font_news.disp( ZOOM_X1+60, ZOOM_Y1+90, str, MAP_X2);

	str = "";
	str += last_town_profile_time;
	font_news.disp( ZOOM_X1+100, ZOOM_Y1+90, str, MAP_X2);
#endif
}
//----------- End of function TownArray::draw_profile -----------//


//--------- Begin of function TownArray::find_nearest_town --------//
//
// Find the town that is nearest to the firm.
//
// <int>  xLoc, yLoc   = the x and y location of the town
// [int]  nationRecno  = nation recno of the town town
//								 (default: 0, any nation)
//
// return : <int> town town recno of where the firm is nearest to
//
int TownArray::find_nearest_town(int xLoc, int yLoc, int nationRecno)
{
	int   i, curDistance, bestTownRecno=0, minDistance=0x7FFFFFFF;
	Town* townPtr;

	for( i=size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		curDistance = m.points_distance( xLoc, yLoc, townPtr->center_x, townPtr->center_y );

		if( nationRecno && townPtr->nation_recno != nationRecno )
			continue;

		//--- when the firm is outside the town, find the town nearest to the firm ---//

		if( curDistance < minDistance )
		{
			minDistance = curDistance;
			bestTownRecno = i;
		}
	}

	return bestTownRecno;
}
//----------- End of function TownArray::find_nearest_town ---------//


//--------- Begin of function TownArray::build_town --------//
//
// A unit settles on a given location and form a town town.
//
// return: <int> 1 - settled and a town town is formed successfully.
//					  0 - settling failed. too close to a town of another nation.
//
int TownArray::build_town(int unitRecno, int xLoc, int yLoc)
{
	if(!world.can_build_town(xLoc, yLoc, unitRecno))
		return 0;

	//--------- get the nearest town town ------------//

	Unit* unitPtr = unit_array[unitRecno];

	char nationRecno = unitPtr->nation_recno;

	//----- it's far enough to form another town --------//

	town_array.add_town( nationRecno, unitPtr->race_id, xLoc, yLoc, unitRecno );

	return 1;
}
//----------- End of function TownArray::build_town ---------//


//--------- Begin of function TownArray::distribute_demand --------//
//
void TownArray::distribute_demand()
{
	//--- reset market place demand ----//

	int i, j;
	FirmMarket* firmMarket;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i)  )
			continue;

		firmMarket = (FirmMarket*) firm_array[i];

		if( firmMarket->firm_id == FIRM_MARKET )
		{
			for( j=0 ; j<MAX_MARKET_GOODS ; j++ )
			{
				firmMarket->market_goods_array[j].month_demand = (float) 0;
			}
		}
	}

	//-------- distribute demand -------//

	for( i=size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		town_array[i]->distribute_demand();
	}
}
//----------- End of function TownArray::distribute_demand ---------//


//------- Begin of function TownArray::stop_attack_nation -----//

void TownArray::stop_attack_nation(short nationRecno)
{
	Town* townPtr;

	for(int i=size(); i>0; --i)
	{
		townPtr = (Town*) get_ptr(i);

		if( !townPtr )
			continue;

		if( townPtr->last_attack_nation_recno == nationRecno )
			townPtr->last_attack_nation_recno = 0;
	}
}
//----------- End of function TownArray::stop_attack_nation ---------//


//------- Begin of function TownArray::update_all_camp_link -----//

void TownArray::update_all_camp_link()
{
	Town* townPtr;

	for(int i=size(); i>0; --i)
	{
		townPtr = (Town*) get_ptr(i);

		if( !townPtr )
			continue;

		townPtr->update_camp_link();
	}
}
//----------- End of function TownArray::update_all_camp_link ---------//


//------- Begin of function TownArray::set_all_loyalty_to_target -----//
//
// Set the loyalty of all towns to their targets. For scenario creation
// and editing.
//
void TownArray::set_all_loyalty_to_target()
{
	Town* townPtr;

	for(int i=size(); i>0; --i)
	{
		townPtr = (Town*) get_ptr(i);

		if( !townPtr )
			continue;

		townPtr->update_target_loyalty();
		townPtr->loyalty = townPtr->target_loyalty;
	}
}
//----------- End of function TownArray::set_all_loyalty_to_target ---------//


//------- Begin of function TownArray::is_deleted -----//

int TownArray::is_deleted(int recNo)
{
	Town *townPtr = (Town*) get_ptr(recNo);

	if( !townPtr )
		return 1;

	return 0;
}
//--------- End of function TownArray::is_deleted ----//


#ifdef DEBUG

//------- Begin of function TownArray::operator[] -----//

Town* TownArray::operator[](int recNo)
{
	Town* townPtr = (Town*) get_ptr(recNo);

	if( !townPtr )
		err.run( "TownArray[] is deleted" );

	return townPtr;
}

//--------- End of function TownArray::operator[] ----//

#endif

