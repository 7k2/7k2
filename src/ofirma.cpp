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

//Filename    : OFIRMA.CPP
//Description : Object Firm Array

#include <ofirma.h>
#include <ovga.h>
#include <omodeid.h>
#include <onation.h>
#include <osys.h>
#include <ogame.h>
#include <oconfig.h>
#include <oworldmt.h>
#include <oraceres.h>
#include <otown.h>
#include <ofirmall.h>
#include <ofirmres.h>
#include <ogodres.h>
#include <oinfo.h>
#include <oseres.h>
#include <olog.h>
#include <oworld.h>
#include <ou_cara.h>

//### begin alex 22/9 ###//
#ifdef DEBUG
#include <ofont.h>

static unsigned long	last_firm_ai_profile_time = 0L;
static unsigned long	firm_ai_profile_time = 0L;
static unsigned long	last_firm_profile_time = 0L;
static unsigned long	firm_profile_time = 0L;
#endif
//#### end alex 22/9 ####//

//----------- Note ----------------//
//
// go(), start(), end(), fwd(), bkwd() will affect
// the current record no. pointer which will be used by other function
//
// To process all elements in the array, get(recNo) should be used.
//
// Because the sizeof different derived Firm objects are quite largely
// different, it is better to link in pointer instead of the object
// body.
//
// The DynArrayB contain the pointer to Firm objects.
//
//---------------------------------//


//--------- Begin of function FirmArray::FirmArray ---------//
//
FirmArray::FirmArray() : DynArrayB(sizeof(Firm*), 50, DEFAULT_REUSE_INTERVAL_DAYS)
{
}
//----------- End of function FirmArray::FirmArray ---------//


//--------- Begin of function FirmArray::~FirmArray ---------//
//
FirmArray::~FirmArray()
{
   deinit();
}
//----------- End of function FirmArray::~FirmArray ---------//


//--------- Begin of function FirmArray::init ---------//

void FirmArray::init()
{
	process_recno = 1;
}
//---------- End of function FirmArray::init ---------//


//--------- Begin of function FirmArray::deinit ---------//
//
// All firms should be deleted when the system terminated
//
// Chain : delete nation --> delete firm --> delete job --> delete item
//
// Some data is corrupted when the some firms still exist when
// the system terminated
//
void FirmArray::deinit()
{
	if( size()==0 )
		return;

	//----------------------------------//

	int i;

	for( i=1 ; i<=size() ; i++ )
	{
		if( !firm_array.is_deleted(i) )
			firm_array.del_firm(i);
	}

	//----------------------------------//

	zap();       // zap the DynArrayB
}
//----------- End of function FirmArray::deinit ---------//


//--------- Begin of function FirmArray::generate_firm ---------//
//
// Generate a firm for new games or when editing scenarios.
//
// <int> xLoc        = the x location of the firm to be built
// <int> yLoc        = the y location of the firm to be built
// <int> nationRecno = the nation which builds this firm
// <int> firmId      = firm type id.
// [char*] buildCode = the build code of the firm, no need to give if the firm just have one build type
//
// Return : <int> the record no. of the newly added firm
//
int FirmArray::generate_firm(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode)
{
	int firmRecno = build_firm(xLoc, yLoc, nationRecno, firmId, buildCode, 0, false, true);		// true-no cost for building the firm

	if( !firmRecno )		// no space for building the firm
		return 0;

	firm_array[firmRecno]->complete_construction();

	return firmRecno;
}
//---------- End of function FirmArray::generate_firm ---------//


//--------- Begin of function FirmArray::build_firm ---------//
//
// build_firm() will be called by Nation and FirmRes when setting up
// new firm.
//
// <int> xLoc        = the x location of the firm to be built
// <int> yLoc        = the y location of the firm to be built
// <int> nationRecno = the nation which builds this firm
// <int> firmId      = firm type id.
// [short] builderRecno = recno of the builder unit
// [bool] isUpgrade  = whether this action is to upgrade a firm
//							  (default: false)
// [bool] noCost		= no cost for building the firm (default: false)
//
// Return : <int> the record no. of the newly added firm
//
int FirmArray::build_firm(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno, bool isUpgrade, bool noCost)
{
	if( !world.can_build_firm(xLoc, yLoc, firmId) )
		return 0;

	//--------- check if there is enough cash ----------//

	if( nationRecno && !isUpgrade )
	{
		FirmInfo* firmInfo  = firm_res[firmId];
		Nation*   nationPtr = nation_array[nationRecno];

		if( nationRecno && !noCost )
		{
			if( nationPtr->cash < firmInfo->setup_cost )
				return 0;
			
			if( nationPtr->live_points < firmInfo->setup_live_points_cost )
				return 0;
		}
	}

	//---------- create and build the firm -------------//

	int firmRecno = create_firm(firmId);

	base_obj_array.add( firm_array[firmRecno] );

	firm_array[firmRecno]->init( xLoc, yLoc, nationRecno, firmId, buildCode, builderRecno);

	// Firm::init() will set world matrix, it will use Firm::firm_recno to set the location cargo

	//----------- add building cost ----------//

	if( nationRecno && !isUpgrade && !noCost )
	{
		Nation* nationPtr = nation_array[nationRecno];
		FirmInfo* firmInfo = firm_res[firmId];

		nationPtr->add_expense( EXPENSE_CONSTRUCTION, (float)firmInfo->setup_cost);		// setup cost of the firm
		nationPtr->change_live_points( (float) -firmInfo->setup_live_points_cost );
	}

//	world.plant_limit = world.plant_limit - m.random(10);

	return firmRecno;
}
//---------- End of function FirmArray::build_firm ---------//

//--------- Begin of function FirmArray::create_firm ---------//
//
// create_firm() will be called directly by :
//
// 1. FirmArray::build_firm()  for setting up a new firm
// 2. FirmArray::read_file() when loading game.
//
// <int> firmId = firm type id
//
// Return : <int> the record no. of the newly added firm
//
int FirmArray::create_firm(int firmId)
{
	Firm* firmPtr;

//	static char firmAnimalTrainArray[] = { UNIT_JAGUAR, UNIT_GORILLA, UNIT_RHINO, 0 };
	static char firmIncubatorTrainArray[] = { UNIT_BALLISTA, UNIT_CATAPULT, UNIT_CANNON, 0 };

	switch(firmId)
	{
		case FIRM_BASE:
			firmPtr = new FirmBase;
			break;

		case FIRM_FORT:
			firmPtr = new FirmFort;
			break;

		case FIRM_CAMP:
			firmPtr = new FirmCamp;
			break;

		case FIRM_FACTORY:
			firmPtr = new FirmFactory;
			break;

		case FIRM_INN:
			firmPtr = new FirmInn;
			break;

		case FIRM_MARKET:
			firmPtr = new FirmMarket;
			break;

		case FIRM_MINE:
			firmPtr = new FirmMine;
			break;

		case FIRM_RESEARCH:
			firmPtr = new FirmResearch;
			break;

		case FIRM_WAR_FACTORY:
			firmPtr = new FirmWar;
			break;

//		case FIRM_HARBOR:
//			firmPtr = new FirmHarbor;
//			break;

		case FIRM_SPY:
			firmPtr = new FirmSpy;
			break;

		case FIRM_SPECIAL:
			firmPtr = new FirmSpecial;
			break;

		case FIRM_LAIR:
			firmPtr = new FirmLair;
			break;

		case FIRM_BEE:
			firmPtr = new FirmMonsterTrain(UNIT_BEE);
			break;

		case FIRM_TERMITE:
			firmPtr = new FirmMonsterTrain(UNIT_TERMITE);
			break;
		
		case FIRM_LISHORR:
		case FIRM_WILDE_LISHORR:
			firmPtr = new FirmLishorr;
			break;

		case FIRM_ALCHEMY:
			firmPtr = new FirmMonsterAlchemy;
			break;

		case FIRM_INCUBATOR:
			firmPtr = new FirmIncubator;
			break;

		case FIRM_ANIMAL:
			firmPtr = new FirmAnimal;
			break;

		case FIRM_FORTRESS:
			firmPtr = new FirmMonsterFortress(UNIT_ROCK);
			break;

		case FIRM_MAGIC:
			firmPtr = new FirmMagic;
			break;

		case FIRM_OFFENSIVE_BUILDING_1:
		case FIRM_OFFENSIVE_BUILDING_2:
		case FIRM_OFFENSIVE_BUILDING_4:
			firmPtr = new FirmOffensive;
			break;

		case FIRM_OFFENSIVE_BUILDING_3:	
			firmPtr = new FirmOffensive2;
			break;

		default:
			err_now("FirmArray::create_firm()");
	}
 
	//----------------------------------------//

	linkin(&firmPtr);
	firmPtr->firm_recno = recno();

	return firmPtr->firm_recno;
}
//----------- End of function FirmArray::create_firm ---------//

//--------- Begin of function FirmArray::firm_class_size ---------//
//
// Return the size of the specified class.
// This function will be called by FirmArray::write_file()
//
// <int> id    = the id of the job
//
int FirmArray::firm_class_size(int id) 
{
	switch(id)
	{
		case FIRM_BASE:
			return sizeof(FirmBase);

		case FIRM_FORT:
			return sizeof(FirmFort);

		case FIRM_CAMP:
			return sizeof(FirmCamp);

		case FIRM_FACTORY:
			return sizeof(FirmFactory);

		case FIRM_INN:
			return sizeof(FirmInn);

		case FIRM_MARKET:
			return sizeof(FirmMarket);

		case FIRM_MINE:
			return sizeof(FirmMine);

		case FIRM_RESEARCH:
			return sizeof(FirmResearch);

		case FIRM_WAR_FACTORY:
			return sizeof(FirmWar);

//		case FIRM_HARBOR:
//			return sizeof(FirmHarbor);

		case FIRM_SPY:
			return sizeof(FirmSpy);

		case FIRM_SPECIAL:
			return sizeof(FirmSpecial);

		case FIRM_LAIR:
			return sizeof(FirmLair);

		case FIRM_BEE:
			return sizeof(FirmMonsterTrain);

		case FIRM_TERMITE:
			return sizeof(FirmMonsterTrain);

		case FIRM_LISHORR:
		case FIRM_WILDE_LISHORR:
			return sizeof(FirmLishorr);

		case FIRM_ALCHEMY:
			return sizeof(FirmMonsterAlchemy);

		case FIRM_INCUBATOR:
			return sizeof(FirmIncubator);

		case FIRM_ANIMAL:
			return sizeof(FirmAnimal);

		case FIRM_FORTRESS:
			return sizeof(FirmMonsterFortress);

		case FIRM_MAGIC:
			return sizeof(FirmMagic);

		case FIRM_OFFENSIVE_BUILDING_1:
		case FIRM_OFFENSIVE_BUILDING_2:
		case FIRM_OFFENSIVE_BUILDING_4:
			return sizeof(FirmOffensive);

		case FIRM_OFFENSIVE_BUILDING_3:
			return sizeof(FirmOffensive2);

		default:
			err_now( "FirmArray::firm_class_size" );
	}

	return 0;
}
//----------- End of function FirmArray::firm_class_size ---------//


//--------- Begin of function FirmArray::del_firm ---------//
//
// Warning : After calling this function, the recno() is still
//           pointing to the deleted record.
//           So go() to a new record to prevent running NULL object
//
// <int> recNo = the no. of the record to be deleted
//               (default : current record no.)
//
void FirmArray::del_firm(int recNo)
{
   Firm* firmPtr = firm_array[recNo];

	//--- del its link from base_obj_array ----//

	base_obj_array.del(firmPtr->base_obj_recno);

	//---- delete the object and detach it from firm_array ---//

	int	xLoc = firmPtr->center_x;
	int	yLoc = firmPtr->center_y;

	if( !sys.quick_exit_flag() )
		firmPtr->deinit();

	delete firmPtr;

	linkout(recNo);

//	world.plant_limit = world.plant_limit + m.random(5);
}
//----------- End of function FirmArray::del_firm ---------//


//--------- Begin of function FirmArray::process ---------//
//
// Process all firm in firm_array for action and movement for next frame
//
// Return : 1 - all firm in the FirmArray has been processed
//          0 - only some has been processed, not all
//
int FirmArray::process()
{
   int  i;
	Firm *firmPtr;

	// ####### begin Gilbert 19/11 ######//
	if( sys.day_frame_count == 0 )
	{
		god_res.update_prayer_count();		// count no. of prayer of each god, each nation
	}
	// ####### end Gilbert 19/11 ######//

	//----- each time process some firm only ------//

	for( i=1 ; i<=size() ; i++ )
	{
		firmPtr = (Firm*) get_ptr(i);

		if( !firmPtr )    // the firm has been deleted
			continue;

		err_when(firmPtr->firm_recno!=i);

		//-------- system yield ---------//

		if( i%20==1 )
			sys.yield();

#if defined(ENABLE_LOG)
		String logStr;
		logStr = "begin process firm ";
		logStr += firmPtr->firm_recno;
		logStr += " nation=";
		logStr += firmPtr->nation_recno;
		LOG_MSG(logStr);
#endif
	
		if(i==50)
		{

			FirmMarket *mPtr = (FirmMarket*) firmPtr;
			MarketGoods *marketGoods = mPtr->market_goods_array;
			marketGoods++;

			if(marketGoods->stock_qty)
				int debug = 0;
		}

		// ##### begin Gilbert 18/5 ########//

		//-------- process visibility -----------//

		if( i%FRAMES_PER_DAY == int(sys.frame_count%FRAMES_PER_DAY) )	// only process each firm once per day
		{
			// before under construction checking 

			if( firmPtr->explore_for_player() )
			{
				world.visit( firmPtr->loc_x1, firmPtr->loc_y1, firmPtr->loc_x2, firmPtr->loc_y2, EXPLORE_RANGE-1 );
			}
		}
		// ##### begin Gilbert 18/5 ########//


		//------ if the firm is under construction ------//

		if( firmPtr->under_construction )
		{
			LOG_MSG(" process_construction");
			firmPtr->process_construction();
			LOG_MSG(m.get_random_seed() );
			continue;
		}

		//------ if the firm is being upgrade ------//

		if( firmPtr->upgrading_firm_id )
		{
			if( firmPtr->process_upgrade() )
				continue;
		}

		// ###### begin Gilbert 4/1 #######//
		firmPtr->process();
		// ###### end Gilbert 4/1 #######//

		//--------- process and process_ai firms ----------//

		if( i%FRAMES_PER_DAY == int(sys.frame_count%FRAMES_PER_DAY) )	// only process each firm once per day
		{
			#ifdef DEBUG
			unsigned long profileStartTime = m.get_time();
			#endif

//			//-------- process visibility -----------//
//
//			if( firmPtr->explore_for_player() )
//			{
//				world.visit( firmPtr->loc_x1, firmPtr->loc_y1, firmPtr->loc_x2, firmPtr->loc_y2, EXPLORE_RANGE-1 );
//			}

			LOG_MSG(" next_day");
			firmPtr->next_day();
			LOG_MSG(m.get_random_seed() );

			#ifdef DEBUG
			firm_profile_time += m.get_time() - profileStartTime;
			#endif

			//-- if the hit points drop to zero, the firm should be deleted --//

			if( firmPtr->hit_points <=0 )
			{
				se_res.sound( firmPtr->center_x, firmPtr->center_y, 1, 'F', firmPtr->firm_id, "DEST" );
				del_firm( firmPtr->firm_recno );
				continue;
			}

			//--------- process AI ------------//

			#ifdef DEBUG
			if( config.disable_ai_flag==0 && firmPtr->is_ai )
			#else
			if( firmPtr->is_ai )
			#endif
			{
				LOG_MSG(" process_common_ai");
				firmPtr->process_common_ai();
				LOG_MSG(m.get_random_seed() );

				#ifdef DEBUG
				unsigned long profileAiStartTime = m.get_time();
				#endif

				LOG_MSG(" process_ai");
				firmPtr->process_ai();
				LOG_MSG(m.get_random_seed());

				#ifdef DEBUG
				firm_ai_profile_time += m.get_time() - profileAiStartTime;
				#endif

				if( is_deleted(i) )		// the firm may have been deleted in process_ai()
					continue;
			}
		}

		//-------- process animation ---------//

		LOG_MSG(" process_animation");
		firmPtr->process_animation();
		LOG_MSG( m.get_random_seed() );

		//-------- process monster firm ---------//

		LOG_MSG(" process_animation");
		firmPtr->process_monster_firm();
		LOG_MSG( m.get_random_seed() );

	}

	return 0;
}
//----------- End of function FirmArray::process ---------//


//--------- Begin of function FirmArray::next_month ---------//
//
void FirmArray::next_month()
{
	int	 i;
	Firm*  firmPtr;

	LOG_MSG("begin FirmArray::next_month");
	LOG_MSG(m.get_random_seed() );
	for(i=1; i <=size() ; i++)
	{
		firmPtr = (Firm*)get_ptr(i);

		if( firmPtr && !firmPtr->under_construction )
		{
			LOG_MSG("Firm next_month");
			LOG_MSG( i );
			firmPtr->next_month();
			LOG_MSG(m.get_random_seed() );
		}
	}
	LOG_MSG("end FirmArray::next_month");
	LOG_MSG(m.get_random_seed() );
}
//----------- End of function FirmArray::next_month -----------//


//--------- Begin of function FirmArray::next_year ---------//
//
void FirmArray::next_year()
{
	int	 i;
	Firm*  firmPtr;

	LOG_MSG("begin FirmArray::next_year");
	LOG_MSG(m.get_random_seed() );
	for(i=1; i <=size() ; i++)
	{
		firmPtr = (Firm*)get_ptr(i);

		if( firmPtr && !firmPtr->under_construction )
		{
			LOG_MSG("Firm next_month");
			LOG_MSG( i );
			firmPtr->next_year();
			LOG_MSG(m.get_random_seed() );
		}
	}
	LOG_MSG("end FirmArray::next_year");
	LOG_MSG(m.get_random_seed() );
}
//----------- End of function FirmArray::next_year -----------//


//--------- Begin of function FirmArray::draw_dot ---------//
//
// Draw tiny dots on map window representing the location of the firm
//
// [int] filterFirmId - if given, only display firms whose type is filterFirmId
//
void FirmArray::draw_dot(int filterFirmId)
{
	int	  		i, x, y;
	short			nationColor;
	int			vgaBufPitch = vga_back.buf_pitch();

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

	// ------ draw dots for each firm -------//

	for(i=1; i <=size() ; i++)
	{
		short *rowWritePtr;
		int firmWidth;
		int firmHeight;

		{ // minimize register variable at one time
			register Firm* firmPtr = (Firm*)get_ptr(i);

			if( !firmPtr )
				continue;

			// never show lishorr, wilde lishorr or fortress with camouflage ability
			if(!firmPtr->is_visible(nation_array.player_recno))
				continue;

			if( firm_array.selected_recno != i)		// if the firm is currently selected, ignore all filter
			{
				//--------- check filter ---------//

				if( filterFirmId )
				{
					if( filterFirmId == FIRM_FORT || filterFirmId == FIRM_CAMP )		// Camp and Fort are the same group
					{
						if( firmPtr->firm_id != FIRM_FORT && firmPtr->firm_id != FIRM_CAMP )
							continue;
					}
					else
					{
						if( firmPtr->firm_id != filterFirmId )
							continue;
					}
				}

				//--- shows only mine, factory and market -----//

				if( world.map_matrix->map_mode == MAP_MODE_TRADE
					&& !(firmPtr->cast_to_FirmMine() || firmPtr->cast_to_FirmFactory() || firmPtr->cast_to_FirmMarket()) )
					continue;

				// filter selecFirmMonsterTrain(UNIT_BEE);ted nation
				if( world.map_matrix->filter_nation_flag && world.map_matrix->filter_nation_recno != firmPtr->nation_recno )
					continue;
			}

			//-----------------------------------------//

			FirmBuild* firmBuild = firm_res.get_build(firmPtr->firm_build_id);

			if( world.map_matrix->map_mode != MAP_MODE_TRADE )
			{
				if( info.game_date - firmPtr->last_attack_date <= 2 )
					nationColor = excitedColorArray[firmPtr->nation_recno];
				else if( selected_recno == i )
					nationColor = slowExcitedColorArray[firmPtr->nation_recno];
				else
					nationColor = nationColorArray[firmPtr->nation_recno];
			}
			else
			{
				err_when( MAX_STOP_FOR_CARAVAN != 2 ); // current code check only two stops

				// if selected caravan linked to this firm, flash

				Unit *unitPtr;
				if( unit_array.selected_recno 
					&& (unitPtr = unit_array[unit_array.selected_recno])
					&& unitPtr->unit_id == UNIT_CARAVAN
					&& ( ((UnitCaravan *)unitPtr)->stop_array[0].firm_recno == i 
						|| ((UnitCaravan *)unitPtr)->stop_array[1].firm_recno == i) )
				{
					nationColor = slowExcitedColorArray[firmPtr->nation_recno];
				}

				// ----- if selected firm link to this firm, flash -------//

				else if( firm_array.selected_recno != i
					&& firmPtr->get_linked_firm_status(firm_array.selected_recno) == LINK_EE )
				{
					nationColor = slowExcitedColorArray[firmPtr->nation_recno];
				}

				// ----- if selected town linked to this firm, flash ------//

				else if( town_array.selected_recno
					&& firmPtr->get_linked_town_status(town_array.selected_recno) == LINK_EE )
				{
					nationColor = slowExcitedColorArray[firmPtr->nation_recno];
				}
				else
				{
					nationColor = nationColorArray[firmPtr->nation_recno];
				}
			}

			firmWidth = firmBuild->loc_width;
			firmHeight = firmBuild->loc_height;

			rowWritePtr = vga_back.buf_ptr( 
				world.map_matrix->calc_loc_pos_x(firmPtr->loc_x1, firmPtr->loc_y1) + MAP_X1,
				world.map_matrix->calc_loc_pos_y(firmPtr->loc_x1, firmPtr->loc_y1) + MAP_Y1 );
		}

		{ // minimize register variable at one time
			register short *writePtr;

			for( y = 0; y < firmHeight; (rowWritePtr+=y&1?vgaBufPitch:-1), ++y )
			{
				writePtr = rowWritePtr;
				for( x = 0; x < firmWidth; (writePtr+=vgaBufPitch+1),x+=2 )
				{
					if( *writePtr != unexploredColor )
						*writePtr = nationColor;
				}
				if( *writePtr != unexploredColor)
					*writePtr = shadowColor;
			}
			writePtr = rowWritePtr;
			for( x = 0; x < firmWidth; (writePtr+=vgaBufPitch+1),x+=2 )
			{
				if( *writePtr != unexploredColor )
					*writePtr = shadowColor;
			}
		}
	} // end for

	// ###### end Gilbert 3/11 ######//
}
//----------- End of function FirmArray::draw_dot -----------//


//--------- Begin of function FirmArray::draw_profile ---------//
void FirmArray::draw_profile()
{
#ifdef DEBUG	
	static unsigned long lastDrawTime = m.get_time();

	if(m.get_time() >= lastDrawTime + 1000)
	{
		last_firm_profile_time = firm_profile_time;
		lastDrawTime = m.get_time();
		firm_profile_time = 0L;
	}

	String str;
	str  = "Firm  : ";
	font_news.disp( ZOOM_X1+10, ZOOM_Y1+60, str, MAP_X2);

	str = "";
	str += last_firm_ai_profile_time;
	font_news.disp( ZOOM_X1+60, ZOOM_Y1+60, str, MAP_X2);

	str = "";
	str += last_firm_profile_time;
	font_news.disp( ZOOM_X1+100, ZOOM_Y1+60, str, MAP_X2);
#endif
}
//----------- End of function FirmArray::draw_profile -----------//

#ifdef DEBUG

//------- Begin of function FirmArray::operator() -----//

Firm* FirmArray::operator()()
{
   Firm* firmPtr = (Firm*) get_ptr();   // if recno()==0, get_ptr() returns NULL

	err_if( !firmPtr )
      err_now( "FirmArray[recno()] is deleted" );

   return firmPtr;
}

//--------- End of function FirmArray::operator() ----//


//------- Begin of function FirmArray::operator[] -----//

Firm* FirmArray::operator[](int recNo)
{
	Firm* firmPtr = (Firm*) get_ptr(recNo);

	if( !firmPtr )
      err.run( "FirmArray[] is deleted" );

   return firmPtr;
}

//--------- End of function FirmArray::operator[] ----//

#endif

//---- Begin of function FirmArray::upgrade_firm ----//
//
// Upgrade a firm from one type to another. The upgraded firm
// must be derived from the original firm.
//
// <int> oldFirmRecno - the recno of the old firm.
// <int> newFirmId	 - the id. of the new firm.
//
void FirmArray::upgrade_firm(int oldFirmRecno, int newFirmId)
{
	int oldFirmId = firm_array[oldFirmRecno]->firm_id;

	int oldFirmSize = firm_class_size(oldFirmId);
	int newFirmSize = firm_class_size(newFirmId);

	// ##### begin Gilbert 4/11 ######//
	int oldSelectedRecno = firm_array.selected_recno;	// del_firm may clear firm_array.selected_recno
	// ##### end Gilbert 4/11 ######//

	//---- backup this FirmArray first -----//

	Firm* oldFirmBackup;

	Firm* oldFirm = firm_array[oldFirmRecno];

	oldFirmBackup = (Firm*) mem_add( oldFirmSize );	// use mem_add() instead of new new bypass calling construcnewr and destrucnewr

	memcpy( oldFirmBackup, oldFirm, oldFirmSize );

	//--- reset the content of the derived class before deinit it so that it won't call deinit functions unnecessarily ---//

	if( oldFirm->firm_id == FIRM_CAMP )
	{
		memset( (char*)oldFirm + sizeof(Firm), 0, sizeof(FirmCamp) - sizeof(Firm) );
	}

	//--- delete this FirmArray and create a new FirmFort ---//

	del_firm( oldFirmRecno );

	disable_reuse_interval();		// temporarily disable reuse interval checking new let the new fort use the same recno as the deleted camp.
	base_obj_array.disable_reuse_interval();		// temporarily disable reuse interval checking new let the new fort use the same recno as the deleted camp.

	int newFirmRecno = build_firm( oldFirmBackup->loc_x1, oldFirmBackup->loc_y1,
		oldFirmBackup->nation_recno, newFirmId, race_res[oldFirmBackup->race_id]->code, 0, 1 );		// 1-upgrade firm 

	enable_reuse_interval();
	base_obj_array.enable_reuse_interval();

	Firm* newFirm = firm_array[newFirmRecno];

	//---- copy the content of the original camp to the new fort ----//

	oldFirmBackup->upgrading_firm_id = 0;
	oldFirmBackup->firm_id 				= newFirm->firm_id;     	// as we copy firmBackUp to newFirm, we need to preserve some vars
	oldFirmBackup->firm_build_id 		= newFirm->firm_build_id;
	oldFirmBackup->base_obj_recno		= newFirm->base_obj_recno;

	// ####### begin Gilbert 4/11 ######//
	// selected_recno = newFirmRecno;
	// ####### end Gilbert 4/11 ######//

	memcpy( (char*)newFirm + sizeof(void*), (char*)oldFirmBackup + sizeof(void*),
			  oldFirmSize - sizeof(void*) );

	//------ free oldFirmBackup -------//

	mem_del( oldFirmBackup );
	
	// ##### begin Gilbert 4/11 ######//
	if( oldSelectedRecno == oldFirmRecno )
	{
		firm_array.selected_recno = newFirmRecno;
		info.disp();
	}
	// ##### end Gilbert 4/11 ######//
}
//----- End of function FirmArray::upgrade_firm -----//
