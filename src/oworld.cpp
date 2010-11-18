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

//Filename    : OWORLD.CPP
//Description : Object World

#include <oworld.h>
#include <all.h>
#include <oworldmt.h>

#include <osys.h>
#include <ogame.h>
#include <ovga.h>
#include <omodeid.h>
#include <ofont.h>
#include <omouse.h>
#include <omousecr.h>
#include <oconfig.h>
#include <ofirmres.h>
#include <ogodres.h>
#include <oplant.h>
#include <opower.h>
#include <osite.h>
#include <oinfo.h>
#include <otown.h>
#include <ounit.h>
#include <onation.h>
#include <oweather.h>
#include <oterrain.h>
#include <oanline.h>
#include <otornado.h>
#include <oseres.h>
#include <oremote.h>
#include <onews.h>
#include <opfind.h>
#include <orock.h>
#include <orockres.h>

//------------ Define static class variables ------------//

int   World::max_x_loc=MAX_MAP_WIDTH, World::max_y_loc=MAX_MAP_HEIGHT;
char  World::check_unit_space_avoid_mountain=0;

//----------- Begin of function World::World ----------//

World::World()
{
   memset( this, 0, sizeof(World) );
}
//------------- End of function World::World -----------//


//----------- Begin of function World::~World ----------//

World::~World()
{
   deinit();
}
//------------- End of function World::~World -----------//


//----------- Begin of function World::init ----------//

void World::init()
{
	//----------- initialize vars -------------//

	scan_fire_x = 0;
	scan_fire_y = 0;
	lightning_signal = 0;

   //------- initialize matrix objects -------//

   map_matrix  = new MapMatrix;
   zoom_matrix = new ZoomMatrix;

	map_matrix->init_para();
	zoom_matrix->init_para();
}
//------------- End of function World::init -----------//


//----------- Begin of function World::deinit ----------//

void World::deinit()
{
   if( map_matrix )
	{
      delete map_matrix;
      map_matrix = NULL;
   }

   if( zoom_matrix )
   {
      delete zoom_matrix;
      zoom_matrix = NULL;
   }

   if( loc_matrix )
   {
      mem_del( loc_matrix );
      loc_matrix  = NULL;
   }

	if( corner_matrix )
	{
		mem_del( corner_matrix );
		corner_matrix = NULL;
	}
}
//------------- End of function World::deinit -----------//


//--------- Begin of function World::assign_map ----------//
//
// After a map is loaded, assign_map() need to be called to
// initial map_matrix and zoom_matrix
//
void World::assign_map()
{
	//------------- assign map -------------//

   map_matrix-> assign_map(loc_matrix, max_x_loc, max_y_loc, corner_matrix );
	zoom_matrix->assign_map(loc_matrix, max_x_loc, max_y_loc, corner_matrix );

   //-------- set the zoom area box on map matrix ------//

   map_matrix->cur_x_loc = 0;
   map_matrix->cur_y_loc = 0;
   map_matrix->cur_cargo_width  = zoom_matrix->disp_x_loc;
   map_matrix->cur_cargo_height = zoom_matrix->disp_y_loc;
	map_matrix->cur_cargo_rect_type = zoom_matrix->disp_rect_type;
}
//----------- End of function World::assign_map ----------//


//----------- Begin of function World::paint ------------//
//
// Paint world window and scroll bars
//
void World::paint()
{
   map_matrix->paint();
   zoom_matrix->paint();
}
//----------- End of function World::paint ------------//


//----------- Begin of function World::refresh ------------//
//
void World::refresh()
{
   map_matrix->refresh();
   zoom_matrix->refresh();
}
//----------- End of function World::refresh ------------//


//----------- Begin of function World::disp_zoom ------------//
//
void World::disp_zoom()
{
	zoom_matrix->disp();
}
//----------- End of function World::disp_zoom ------------//


//----------- Begin of function World::process ------------//
//
// Called every frame
//
void World::process()
{
	//-------- process wall ----------//

	// ###### begin Gilbert 25/5 ########//
	// no more wall
	// form_world_wall();
	// ###### end Gilbert 25/5 ########//

	//-------- process fire -----------//

	// BUGHERE : set Location::flammability for every change in cargo

	world.spread_fire(weather);

	// ------- process visibility --------//
	process_visibility();

	//-------- process lightning ------//
	// ###### begin Gilbert 11/8 ########//
	if(lightning_signal== 0 && weather.is_lightning())
	{
		// ------- create new lightning ----------//
		lightning_signal = 110;
	}
	if( lightning_signal == 106 && config.weather_effect)
	{
		lightning_strike(m.random(MAX_MAP_WIDTH), m.random(MAX_MAP_HEIGHT), 1);
	}
	if(lightning_signal == 100)
		lightning_signal = 5 + m.random(10);
	else if( lightning_signal)
		lightning_signal--;
	// ###### end Gilbert 11/8 ########//

	//---------- process ambient sound ---------//

	if( sys.frame_count%10 == 0 )    // process once per ten frames
		process_ambient_sound();

	// --------- update scan fire x y ----------//
	if(++scan_fire_x >= SCAN_FIRE_DIST)
	{
		scan_fire_x = 0;
		if( ++scan_fire_y >= SCAN_FIRE_DIST)
			scan_fire_y =0;
	}

}
//----------- End of function World::process ------------//


//----------- Begin of function World::next_day ------------//
//
// Called every frame
//
void World::next_day()
{
   plant_ops();

   weather = weather_forecast[0];

	for(int foreDay=0; foreDay < MAX_WEATHER_FORECAST-1; ++foreDay)
   {
      weather_forecast[foreDay] = weather_forecast[foreDay+1];
   }

   weather_forecast[MAX_WEATHER_FORECAST-1].next_day();

	// ####### begin Gilbert 11/7 #########//
	magic_weather.next_day();
	// ####### end Gilbert 11/7 #########//

	if(weather.has_tornado() && config.weather_effect && config.random_event_frequency )
	{
		tornado_array.add_tornado(weather.tornado_x_loc(max_x_loc, max_y_loc),
			weather.tornado_y_loc(max_x_loc, max_y_loc), 600);
	}

	// ######## begin Gilbert 16/12 #######//
	if( weather.is_quake() && (config.random_event_frequency || magic_weather.is_quake) )
	// ######## end Gilbert 16/12 #######//
	{
		earth_quake();
	}

	//-------- Debug code: BUGHERE ----------//

	#ifdef DEBUG

	Location* locPtr = loc_matrix;

	for( int y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
	{
		for( int x=0 ; x<MAX_WORLD_X_LOC ; x++ )
		{
//			if( locPtr->unit_recno(UNIT_LAND) )
//			{
//				err_when( locPtr->unit_recno(UNIT_LAND) );
//			}
			// ####### begin Gilbert 26/2 ######//
			// more detailed checking
			int unitRecno;
			if( (unitRecno = locPtr->unit_recno(UNIT_LAND)) )
			{
				err_when( unit_array.is_truly_deleted(unitRecno) );
				Unit *unitPtr = unit_array[unitRecno];
				err_when( unitPtr->next_x_loc() > x || unitPtr->next_x_loc()+unitPtr->loc_width <= x);
				err_when( unitPtr->next_y_loc() > y || unitPtr->next_y_loc()+unitPtr->loc_height <= y);
			}
			// ####### end Gilbert 26/2 ######//

			locPtr++;
		}
	}

	#endif
}
//----------- End of function World::next_day ------------//


//----------- Begin of function World::detect ------------//
//
// Detect mouse action from user
//
// Return : 1 - mouse pressed on World area
//          0 - mouse not pressed on World area
//
int World::detect()
{
   if( map_matrix->detect() )
      return 1;

   if( zoom_matrix->detect() )
      return 1;

	if( detect_scroll() )
		return 1;

	// ##### begin Gilbert 16/9 #######//
	// return detect_firm_town();
	return 0;
	// ##### end Gilbert 16/9 #######//
}
//----------- End of function World::detect ------------//


//--------- Begin of function World::detect_scroll ---------//
//
// Detect if the mouse cursor is pushed towards the border
// of the screen to scroll the zoom window.
//
int World::detect_scroll()
{
	if( last_scroll_method != config.scroll_method )
	{
		last_scroll_method = config.scroll_method;

		// reset every scroll method

		last_scroll_flag = 0;
		return 0;
	}

	int inChatMode = sys.view_mode == MODE_NATION && info.nation_report_mode == NATION_REPORT_CHAT;

	switch( last_scroll_method )
	{
	case 0:
		{
			if( mouse_cursor.frame_flag )    // if it's now in frame selection mode
				return 0;

			if( next_scroll_time && m.get_time() < next_scroll_time )      // just scrolled not too long ago, wait for a little while before next scroll.
				return 0;

			int rc=0;

			//----- scroll left -----//

			// ##### begin Gilbert 30/3 #######//
			if( mouse.cur_x == MOUSE_X1 || !inChatMode && mouse.arrow_key_state & ARROW_LEFT_KEY_MASK )
			// ##### end Gilbert 30/3 #######//
			{
				// zoom_matrix->scroll(-1,0);
				zoom_matrix->scroll(-1, 1);
				rc = 1;
			}

			//---- scroll right -----//

			// ##### begin Gilbert 30/3 #######//
			if( mouse.cur_x == MOUSE_X2 || !inChatMode && mouse.arrow_key_state & ARROW_RIGHT_KEY_MASK )
			// ##### end Gilbert 30/3 #######//
			{
				zoom_matrix->scroll(1,-1);
				rc = 1;
			}

			//---- scroll top -------//

			// ##### begin Gilbert 30/3 #######//
			if( mouse.cur_y == MOUSE_Y1 || !inChatMode && mouse.arrow_key_state & ARROW_UP_KEY_MASK )
			// ##### end Gilbert 30/3 #######//
			{
				zoom_matrix->scroll(-1,-1);
				rc = 1;
			}

			//---- scroll bottom ----//

			// ##### begin Gilbert 30/3 #######//
			if( mouse.cur_y == MOUSE_Y2 || !inChatMode && mouse.arrow_key_state & ARROW_DOWN_KEY_MASK )
			// ##### end Gilbert 30/3 #######//
			{
				zoom_matrix->scroll(1,1);
				rc = 1;
			}

			//----- set next scroll time based on scroll_speed -----//
			//
			// slowest scroll speed: 500/1  = 500 milliseconds or 1/2 second
			// fastest scroll speed: 500/10 = 50  milliseconds or 1/20 second
			//
			//------------------------------------------------------//

			if( rc )
			{
				sys.zoom_need_redraw = 1;        // ask the zoom window to refresh next time
				next_scroll_time     = m.get_time() + 500/(config.scroll_speed+1);
			}

			return rc;
		}
		break;

	case 1:		// method 1, depend on time of scrolling
		{
			if( mouse_cursor.frame_flag )    // if it's now in frame selection mode
			{
				last_scroll_flag = 0;
				return 0;
			}

			// ------ get current scroll flags -------//

			char currentScrollFlag = 0;
			int vectorX = 0;
			int vectorY = 0;

			if( mouse.cur_x == MOUSE_X1 || !inChatMode && mouse.arrow_key_state & ARROW_LEFT_KEY_MASK )
			{
				currentScrollFlag |= 1;		// scroll left
				vectorX--;
				vectorY++;
			}
			if( mouse.cur_x == MOUSE_X2 || !inChatMode && mouse.arrow_key_state & ARROW_RIGHT_KEY_MASK )
			{
				currentScrollFlag |= 2;		// scroll right
				vectorX++;
				vectorY--;
			}
			if( mouse.cur_y == MOUSE_Y1 || !inChatMode && mouse.arrow_key_state & ARROW_UP_KEY_MASK )
			{
				currentScrollFlag |= 4;		// scroll up
				vectorX--;
				vectorY--;
			}
			if( mouse.cur_y == MOUSE_Y2 || !inChatMode && mouse.arrow_key_state & ARROW_DOWN_KEY_MASK )
			{
				currentScrollFlag |= 8;		// scroll down
				vectorX++;
				vectorY++;
			}


			// -------- actual scrolling ---------//

			int rc = 0;
			if( last_scroll_flag == currentScrollFlag && currentScrollFlag )
			{
				//----- set next scroll time based on scroll_speed -----//
				//
				// slowest scroll speed: 500/1  = 500 milliseconds or 1/2 second
				// fastest scroll speed: 500/10 = 50  milliseconds or 1/20 second
				//
				//------------------------------------------------------//

				DWORD step = m.get_time() - begin_scroll_time;
				step = step * config.scroll_speed / 500;
				if( step == 0 )		// scroll instantly
					step = 1;
				// step can be very large if holding for long while
				DWORD maxStep = max_x_loc + max_y_loc + 32;
				if( step > maxStep )
					step = maxStep;

				// record our own last_scroll_top_x/y_loc instead of using zoom_matrix->top_x/y_loc
				// because if in smooth scroll, zoom_matrix->top_x/y_loc doesn't follow closely

				int scrollX = vectorX * (int)step + begin_scroll_top_x_loc - last_scroll_top_x_loc;
				int scrollY = vectorY * (int)step + begin_scroll_top_y_loc - last_scroll_top_y_loc;

				// restrict scroll to 2
				if( scrollX > 2 )
					scrollX = 2;
				if( scrollX < -2 )
					scrollX = -2;
				if( scrollY > 2 )
					scrollY = 2;
				if( scrollY < -2 )
					scrollY = -2;

				if( scrollX != 0 || scrollY != 0 )
				{
					zoom_matrix->scroll( scrollX, scrollY );
					if( zoom_matrix->top_x_loc != last_scroll_top_x_loc
						|| zoom_matrix->top_y_loc != last_scroll_top_y_loc )
					{
						last_scroll_top_x_loc = zoom_matrix->top_x_loc;
						last_scroll_top_y_loc = zoom_matrix->top_y_loc;

						sys.zoom_need_redraw = 1;        // ask the zoom window to refresh next time
						rc = 1;
					}
				}
			}

			// ------- save info for next detect_scroll -------//

			if( last_scroll_flag != currentScrollFlag )
			{
				if( (last_scroll_flag = currentScrollFlag) )
				{
					begin_scroll_time = m.get_time();
					begin_scroll_top_x_loc = last_scroll_top_x_loc = zoom_matrix->top_x_loc;
					begin_scroll_top_y_loc = last_scroll_top_y_loc = zoom_matrix->top_y_loc;
				}
			}

			return rc;
		}

	default:
		err_here();
	}

	return 0;
}
//----------- End of function World::detect_scroll -----------//


//--------- Begin of function World::go_loc --------//
//
// Go to a specified location.
//
// <int> xLoc, yLoc - location to go to.
// [int] selectFlag - whether should the object on the location if
//							 there is one. (default: 0)
//
void World::go_loc(int xLoc, int yLoc, int selectFlag)
{
	//------- set location ---------//

	zoom_matrix->cur_x_loc = xLoc;
	zoom_matrix->cur_y_loc = yLoc;
	// ##### begin Gilbert 9/10 #####//
	// adjust map_matrix->cur_x_loc, cur_y_loc because of z
	int z = get_corner( xLoc, yLoc )->get_altitude();
#if (ZOOM_Z_WIDTH != 0)
	#error
#endif
	int locInc = z * ZOOM_Z_HEIGHT / ( ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT );

	map_matrix->cur_x_loc = xLoc - zoom_matrix->center_x + locInc;
	map_matrix->cur_y_loc = yLoc - zoom_matrix->center_y + locInc;
	// ##### end Gilbert 9/10 #####//
	//--------- refresh ------------//

	map_matrix->valid_cur_box();

	zoom_matrix->top_x_loc = map_matrix->cur_x_loc;
	zoom_matrix->top_y_loc = map_matrix->cur_y_loc;

	sys.zoom_need_redraw = 1;

	//---- if should select the object on the location ----//

	Location* locPtr = world.get_loc(xLoc, yLoc);

	switch( selectFlag )
	{
	case GO_LOC_SELECT_NONE:
		break;

	case GO_LOC_SELECT_LAND:		// sea also
		if( locPtr->unit_recno(UNIT_LAND) )
		{
			power.reset_selection();
			unit_array[locPtr->unit_recno(UNIT_LAND)]->select(true);		// true - set this to unit_array.selected_recno to this unit
			info.disp();
		}
		else if( locPtr->unit_recno(UNIT_SEA) )
		{
			power.reset_selection();
			unit_array[locPtr->unit_recno(UNIT_SEA)]->select(true);		// true - set this to unit_array.selected_recno to this unit
			info.disp();
		}
		else if( locPtr->is_firm() )
		{
			power.reset_selection();
			firm_array.selected_recno = locPtr->firm_recno();
			info.disp();
		}
		else if( locPtr->is_town() )
		{
			power.reset_selection();
			town_array.selected_recno = locPtr->town_recno();
			info.disp();
		}
		break;

	case GO_LOC_SELECT_AIR:
		if( locPtr->unit_recno(UNIT_AIR) )
		{
			power.reset_selection();
			unit_array[locPtr->unit_recno(UNIT_AIR)]->select(true);		// true - set this to unit_array.selected_recno to this unit
			info.disp();
		}
		break;

	case GO_LOC_SELECT_SITE:
		if( locPtr->has_site() )
		{
			power.reset_selection();
			site_array.selected_recno = locPtr->site_recno();
			info.disp();
		}
		break;

	case GO_LOC_SELECT_ANY_UNIT:
		if( locPtr->has_any_unit() )
		{
			int mobileType;
			int unitRecno = locPtr->get_any_unit( mobileType );
			power.reset_selection();
			unit_array[unitRecno]->select(true);		// true - set this to unit_array.selected_recno to this unit
			info.disp();
		}
		break;

	case GO_LOC_SELECT_FIRM:
		if( locPtr->is_firm() )
		{
			power.reset_selection();
			firm_array.selected_recno = locPtr->firm_recno();
			info.disp();
		}
		break;

	case GO_LOC_SELECT_TOWN:
		if( locPtr->is_town() )
		{
			power.reset_selection();
			town_array.selected_recno = locPtr->town_recno();
			info.disp();
		}
		break;

	default:
		err_here();		// fall through to GO_LOC_SELECT_ANY in release mode
	case GO_LOC_SELECT_ANY:
		if( locPtr->has_any_unit() )
		{
			int mobileType;
			int unitRecno = locPtr->get_any_unit( mobileType );
			power.reset_selection();
			unit_array[unitRecno]->select(true);		// true - set this to unit_array.selected_recno to this unit
		}
		else if( locPtr->is_firm() )
		{
			power.reset_selection();
			firm_array.selected_recno = locPtr->firm_recno();
		}
		else if( locPtr->is_town() )
		{
			power.reset_selection();
			town_array.selected_recno = locPtr->town_recno();
		}
		else if( locPtr->has_site() )
		{
			power.reset_selection();
			site_array.selected_recno = locPtr->site_recno();
		}
		info.disp();
		break;
	}
}
//----------- End of function World::go_loc --------//


//-------- Begin of function World::unveil ---------//
//
// Unveil all surrounding areas of the given object.
//
// <int> xLoc1, yLoc1 = the position of the object.
// <int> xLoc2, yLoc2 = the position of the object.
//
void World::unveil(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	if( config.explore_whole_map )
		return;

	xLoc1 = max( 0, xLoc1 - EXPLORE_RANGE);
	yLoc1 = max( 0, yLoc1 - EXPLORE_RANGE);
	xLoc2 = min( MAX_WORLD_X_LOC-1, xLoc2 + EXPLORE_RANGE);
	yLoc2 = min( MAX_WORLD_Y_LOC-1, yLoc2 + EXPLORE_RANGE);

	explore( xLoc1, yLoc1, xLoc2, yLoc2 );
}
//--------- End of function World::unveil ---------//


//-------- Begin of function World::explore ---------//
//
// Explore a specific area. No further exploration around the area.
//
// <int> xLoc1, yLoc1 = the position of the area.
// <int> xLoc2, yLoc2 = the position of the area.
//
void World::explore(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	if( config.explore_whole_map )
		return;

	int		xLoc, yLoc;
	Location* locPtr;
	BitmapW*	imageBuf = (BitmapW *)map_matrix->save_image_buf;
	short*	writePtr;
	int		tileYOffset;

	short	fireColor = vga_back.translate_color(FIRE_COLOR);
	short plantColor = vga_back.translate_color(plant_res.plant_map_color);
	short	seaColor = vga_back.translate_color(0x32);
//	short hillColor = vga_back.translate_color( V_BROWN );
	short rockColor = vga_back.translate_color( 0x59 );
	short shadowColor = vga_back.translate_color( VGA_GRAY+2 );
	short mapModeColor = vga_back.translate_color( VGA_GRAY+10 );
	short roadColor = vga_back.translate_color( VGA_GRAY+12 );
	short	nationColorArray[MAX_NATION+2];
	for( int n = 0; n < MAX_NATION+2; ++n )
		nationColorArray[n] = vga_back.translate_color(nation_array.nation_power_color_array[n]);

	// ##### begin Gilbert 22/9 #######//

	// pass 1 update mini-map

	int writePtrInc = ((BitmapW *)map_matrix->save_image_buf)->get_pitch() + 1;
	int pixelX = map_matrix->calc_loc_pos_x( xLoc1, yLoc1 );
	int pixelY = map_matrix->calc_loc_pos_y( xLoc1, yLoc1 );
	for( yLoc = yLoc1; yLoc<=yLoc2 ; (yLoc&1?++pixelY:--pixelX), ++yLoc )
	{
		writePtr = ((BitmapW *)map_matrix->save_image_buf)->get_ptr(pixelX, pixelY);
		
		tileYOffset = (yLoc & TERRAIN_TILE_Y_MASK) * TERRAIN_TILE_WIDTH;
		xLoc = xLoc1 & ~1;		// smaller even number
		locPtr = get_loc(xLoc, yLoc);
		for( ; xLoc<=xLoc2 ; (xLoc+=2), (locPtr+=2), (writePtr += writePtrInc) )
		{
			if( locPtr->explored() && locPtr[1].explored() )
				continue;

			//-------- draw pixel ----------//

			switch( world.map_matrix->map_mode )
			{
			case MAP_MODE_TERRAIN:
				// ###### begin Gilbert 10/2 ######//
				if( world.map_matrix->filter_object_flag || world.map_matrix->filter_nation_flag )		// do not anything except filtered objects when filters are on
					*writePtr = mapModeColor;
				else if( locPtr->fire_str() > 0)
				// ###### end Gilbert 10/2 ######//
					*writePtr = fireColor;
				else if( locPtr->is_plant() || xLoc+1<max_x_loc && locPtr[1].is_plant() )
					*writePtr = plantColor;
				else if( locPtr->is_rock() )
					*writePtr = rockColor;
				else if( locPtr->has_dirt() )
				{
					Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
					RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
					if (dirtInfo->rock_type == 'P') 
						*writePtr = seaColor;
					else
						*writePtr = rockColor;
				}

				else if( locPtr->is_road() )
					*writePtr = vga_back.translate_color(terrain_res.get_map_tile(locPtr->road_terrain_id)[tileYOffset + (xLoc & TERRAIN_TILE_X_MASK)]);
				else
				{
					if( xLoc < 2 || 
						terrain_res[locPtr->terrain_id]->average_type >=
						terrain_res[(locPtr-2)->terrain_id]->average_type )
					{
						// get terrain pixel from terrain_res
						*writePtr = vga_back.translate_color(terrain_res.get_map_tile(locPtr->terrain_id)[tileYOffset + (xLoc & TERRAIN_TILE_X_MASK)]);
					}
					else
					{
						*writePtr = shadowColor;
					}
				}
				break;

			// ##### begin Gilbert 2/11 #####//
			case MAP_MODE_TRADE:
			// ##### end Gilbert 2/11 #####//
			case MAP_MODE_SPOT:
/*				if( locPtr->sailable() )
					*writePtr = seaColor;
				else if( locPtr->is_rock() )
					*writePtr = rockColor;
				else if( locPtr->has_dirt() )
				{
					Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
					RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
					if (dirtInfo->rock_type == 'P') 
						*writePtr = seaColor;
					else
						*writePtr = rockColor;
				}
				
				else if( locPtr->is_plant() || xLoc+1<max_x_loc && locPtr[1].is_plant() )
					*writePtr = plantColor;
				else*/
					*writePtr = mapModeColor;
				break;

			case MAP_MODE_POWER:
				// ####### begin Gilbert 10/2 #########//
				if( world.map_matrix->filter_object_flag || world.map_matrix->filter_nation_flag )		// do not anything except filtered objects when filters are on
					*writePtr = mapModeColor;
				else if( locPtr->sailable() )
				// ####### end Gilbert 10/2 #########//
					*writePtr = seaColor;
				else if( locPtr->is_plant()  || xLoc+1<max_x_loc && locPtr[1].is_plant() )
					*writePtr = plantColor;
				else if( locPtr->is_rock() )
					*writePtr = rockColor;
/*				else if( locPtr->has_dirt() )
				{
					Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
					RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
					if (dirtInfo->rock_type == 'P') 
						*writePtr = seaColor;
					else
						*writePtr = rockColor;
				}*/

				else
				{
					if( world.map_matrix->power_mode )
						*writePtr = nationColorArray[locPtr->power_nation_recno];
					else
						*writePtr = nationColorArray[0];
				}
				break;

			case MAP_MODE_ALTITUDE:
				{
					Corner *cornerPtr = get_corner( xLoc, yLoc );
					short alt = cornerPtr->get_altitude();
					if( alt > 255 )
						alt = 255;
					else if( alt < 0 )
						alt = 0;
					*writePtr = vga.make_pixel((BYTE)alt, 128, 32);
				}
				break;

			default:
				err_here();
			}

		}
	}

	// pass 2 set explored_on

	for( yLoc=yLoc1 ; yLoc<=yLoc2; yLoc++ )
	{
		locPtr = get_loc(xLoc1, yLoc);
		for( xLoc=xLoc1 ; xLoc <= xLoc2; xLoc++, locPtr++ )
		{
			if( !locPtr->explored() )
			{
				locPtr->explored_on();

				//---- if the command base of the opponent revealed, establish contact ----//

				if( locPtr->is_firm() )
				{
					Firm* firmPtr = firm_array[locPtr->firm_recno()];

					if( firmPtr->nation_recno > 0 && nation_array.player_recno )
					{
						NationRelation *relation = (~nation_array)->get_relation(firmPtr->nation_recno);

						if( !relation->has_contact )
						{
							if( !remote.is_enable() )
							{
								(~nation_array)->establish_contact(firmPtr->nation_recno);
							}
							else
							{
								if( !relation->contact_msg_flag )
								{
									// packet structure : <player nation> <explored nation>
									short *shortPtr = (short *)remote.new_send_queue_msg(MSG_NATION_CONTACT, 2*sizeof(short));
									*shortPtr = nation_array.player_recno;
									shortPtr[1] = firmPtr->nation_recno;
									relation->contact_msg_flag = 1;
								}
							}
						}
					}
				}

				if( locPtr->is_town() )
				{
					Town* townPtr = town_array[locPtr->town_recno()];

					if( townPtr->nation_recno > 0 && nation_array.player_recno )
					{
						NationRelation *relation = (~nation_array)->get_relation(townPtr->nation_recno);

						if( !relation->has_contact )
						{
							if( !remote.is_enable() )
							{
								(~nation_array)->establish_contact(townPtr->nation_recno);
							}
							else
							{
								if( !relation->contact_msg_flag )
								{
									// packet structure : <player nation> <explored nation>
									short *shortPtr = (short *)remote.new_send_queue_msg(MSG_NATION_CONTACT, 2*sizeof(short));
									*shortPtr = nation_array.player_recno;
									shortPtr[1] = townPtr->nation_recno;
									relation->contact_msg_flag = 1;
								}
							}
						}
					}
				}

			}
		}
	}


	// ##### end Gilbert 22/9 #######//
}
//--------- End of function World::explore ---------//

//-------- Begin of function World::is_fully_explored ---------//
//
// Check if the whole area has been explored or not.
//
// <int> xLoc1, yLoc1 = the coordination of the area to explore
// <int> xLoc2, yLoc2 = the coordination of the area to explore
//
int World::is_fully_explored(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	if( config.explore_whole_map )
      return 1;

   int xLoc, yLoc;
   Location* locPtr;

   for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
   {
      locPtr = get_loc(xLoc1, yLoc);

      for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
      {
         if( !locPtr->explored() )
            return 0;
      }
   }

   return 1;
}
//--------- End of function World::is_fully_explored ---------//


//-------- Begin of function World::is_explored ---------//
//
// Check if the whole area has been explored or not.
//
// <int> xLoc1, yLoc1 = the coordination of the area to explore
// <int> xLoc2, yLoc2 = the coordination of the area to explore
//
int World::is_explored(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	if( config.explore_whole_map )
      return 1;

   int xLoc, yLoc;
   Location* locPtr;

   for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
   {
      locPtr = get_loc(xLoc1, yLoc);

      for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
      {
         if( locPtr->explored() )
            return 1;
      }
   }

	return 0;
}
//--------- End of function World::is_explored ---------//


//-------- Begin of function World::is_explored_quick ---------//
//
// Check if any of the four corner has been explored or not.
//
// <int> xLoc1, yLoc1 = the coordination of the area to explore
// <int> xLoc2, yLoc2 = the coordination of the area to explore
//
int World::is_explored_quick(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	if( config.explore_whole_map )
      return 1;

	if( xLoc1 == xLoc2 && yLoc1 == yLoc2 )
		return get_loc( xLoc1, yLoc1 )->explored();
	else
		return get_loc( xLoc1, yLoc1)->explored()
			|| get_loc( xLoc2, yLoc1)->explored()
			|| get_loc( xLoc1, yLoc2)->explored()
			|| get_loc( xLoc2, yLoc2)->explored();
}
//--------- End of function World::is_explored_quick ---------//



//----------- Begin of function World::load_map ------------//
//
// Load a custom map file.
//
void World::load_map(char* fileName)
{
   generate_map(0);

   return;

   //---------- initialize the map matrix --------//

   max_x_loc = 200;
   max_y_loc = 200;

   loc_matrix = (Location*) mem_resize( loc_matrix  , max_x_loc * max_y_loc * sizeof(Location) );

   memset( loc_matrix, 0, sizeof(Location) * max_x_loc * max_y_loc );

   int baseType = TERRAIN_DARK_DIRT;

   int terrainId    = terrain_res.scan(baseType, MIDDLE_MASK, baseType, MIDDLE_MASK, 
					  		 baseType, MIDDLE_MASK, baseType, MIDDLE_MASK, 1);     // 1-get the first instance

   for( int i=0 ; i<max_x_loc*max_y_loc ; i++ )
   {
      loc_matrix[i].terrain_id = terrainId+m.random(3);
   }

   assign_map();

   return;

   //---------- initialize the map matrix --------//

   max_x_loc = 120;
   max_y_loc = 120;

   loc_matrix = (Location*) mem_resize( loc_matrix  , max_x_loc * max_y_loc * sizeof(Location) );

   //-------------- read in the map -------------//

   File mapFile;

   mapFile.file_open(fileName);

   mapFile.file_read(loc_matrix, sizeof(Location)*MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC );

   mapFile.file_close();

   //----- assign the map to MapMatrix and ZoomMatrix -----//

   assign_map();
}
//----------- End of function World::load_map ------------//


//-------- Begin of function World::check_unit_space ------//
//
// To check whether or not the area bounded by the upper left
// corner (xLoc1, yLoc1) and lower right corner (xLoc2, yLoc2)
// can be built.
//
// <int>  xLoc1          = the upper left x
// <int>  yLoc1          = the upper left y
// <int>  xLoc2          = the lower right x
// <int>  yLoc2          = the lower right y
// [int]  mobileType		 = mobile type (default: UNIT_LAND)
// [int]  buildFlag 		 = whether the located area is for building a firm/town
//									if so, the location must no have any raw site.
//									(default: 0)
//
// return 1 for true. 0 for false
//
inline int World::check_unit_space(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int mobileType, int buildFlag)
{
	if(xLoc1<0 || xLoc1>=MAX_WORLD_X_LOC)
		return 0;
	if(yLoc1<0 || yLoc1>=MAX_WORLD_Y_LOC)
		return 0;
	if(xLoc2<0 || xLoc2>=MAX_WORLD_X_LOC)
		return 0;
	if(yLoc2<0 || yLoc2>=MAX_WORLD_Y_LOC)
		return 0;

	//----- also make sure that the 2 surrounding circles do not have any mountains or obstacles ---//

	Location* locPtr;
	int x, y;

	if( check_unit_space_avoid_mountain )
	{
		enum { CHECK_SURROUNDING_RANGE = 2 };

		int tyLoc1 = max( 0, yLoc1-CHECK_SURROUNDING_RANGE );
		int tyLoc2 = min( MAX_WORLD_Y_LOC-1, yLoc2+CHECK_SURROUNDING_RANGE );

		int txLoc1 = max( 0, xLoc1-CHECK_SURROUNDING_RANGE );
		int txLoc2 = min( MAX_WORLD_X_LOC-1, xLoc2+CHECK_SURROUNDING_RANGE );

		for(y=tyLoc1; y<=tyLoc2; y++)
		{
			locPtr = world.get_loc(txLoc1, y);

			for(x=txLoc1; x<=txLoc2; x++, locPtr++)
			{
				if( locPtr->is_rock() )
					return 0;
			}
		}
   }

	//-------------------------------------------//

	for(y=yLoc1; y<=yLoc2; y++)
	{
		locPtr = world.get_loc(xLoc1, y);

		for(x=xLoc1; x<=xLoc2; x++, locPtr++)
		{
			if( !locPtr->can_move(mobileType) ||
				 ( buildFlag && (locPtr->is_power_off() || locPtr->has_site()) ) ) 		// if build a firm/town, there must not be any sites in the area
			{
				return 0;
			}
		}
	}
/*
	//--- try to do a path search to the nearest town, if the search fails, we assume that location is isolated from outside ---//

	if( check_unit_space_avoid_mountain )
	{
		Town* bestTown=NULL;
		int	maxDistance=0xFFFF;

		for( int i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) )
				continue;

			Town* townPtr = town_array[i];

			int curDistance = m.points_distance(townPtr->center_x, townPtr->center_y, xLoc1, yLoc2 );

			if( curDistance < maxDistance )
			{
				maxDistance = curDistance;
				bestTown 	= townPtr;
			}
		}

		if( bestTown )
		{
			path_finder.find_path( xLoc1, yLoc1,
										  bestTown->center_x, bestTown->center_y,
										  bestTown->loc_x1, bestTown->loc_y1,
										  bestTown->loc_x2, bestTown->loc_y2 );

			if( !path_finder.is_path_found() )
			{
				if( ++check_unit_space_avoid_mountain > 100 )		// if too many path searching failures, give up avoiding mountains
					check_unit_space_avoid_mountain = 0;

				return 0;
			}
		}
	}
*/
	return 1;
}
//-------- End of function World::check_unit_space ------//


//-------- Begin of function World::locate_space ------//
//
// Locate an area in the world map around the firm to place
// the unit
//
// <int&> xLoc1          = the upper left x location of the building, also for returning the result location
// <int&> yLoc1          = the upper left y location of the building
// <int>  xLoc2          = the lower right x location of the building
// <int>  yLoc2          = the lower right y location of the building
// <int>  spaceLocWidth  = the location width of the required space
// <int>  spaceLocHeight = the location height of the required space
// [int]  mobileType		 = mobile type (default: UNIT_LAND)
// [int]  regionId		 = specify the region no. of the location to locate
//									(default: region no. of xLoc1, yLoc1)
// [int]  buildFlag 		 = whether the located area is for building a firm/town
//									if so, the location must no have any raw site.
//									(default: 0)
//
// return : <int> 1 - free space found
//                0 - free space not found
//
int World::locate_space(int& xLoc1, int& yLoc1, int xLoc2, int yLoc2,
								int spaceLocWidth, int spaceLocHeight, int mobileType, int regionId, int buildFlag)
{
	if( !regionId )
		regionId = get_loc(xLoc1, yLoc1)->region_id;

	int isPlateau = get_loc(xLoc1, yLoc1)->is_plateau();

	//-----------------------------------------------------------//
	// xLoc, yLoc is the adjusted upper left corner location of
	// the firm. with the adjustment, it is easier to do the following
	// checking.
	//-----------------------------------------------------------//

	Location* locPtr;
	int xLoc = xLoc1 - spaceLocWidth + 1;
	int yLoc = yLoc1 - spaceLocHeight + 1;

	if(xLoc < 0)
		xLoc = 0;
	if(yLoc < 0)
		yLoc = 0;

	int width   = xLoc2 - xLoc + 1;
	int height  = yLoc2 - yLoc + 1;
	int loopCount=0;

	while(1)
	{
		err_when( ++loopCount > MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC * 4 );

		//-----------------------------------------------------------//
		// step 1
		//-----------------------------------------------------------//
		int xOffset = width/2;
		int yOffset = height;
		int x, y;

		x = xLoc + xOffset;
		y = yLoc + yOffset;

		if(x>=0 && y>=0 && x+spaceLocWidth-1<MAX_WORLD_X_LOC && y+spaceLocHeight-1<MAX_WORLD_Y_LOC)
		{
			if(mobileType==UNIT_LAND || (x%2==0 && y%2==0))
			{
				locPtr = get_loc(x,y);

				if( locPtr->region_id == regionId &&
					 locPtr->is_plateau() == isPlateau && 
					 check_unit_space(x, y, x+spaceLocWidth-1, y+spaceLocHeight-1, mobileType, buildFlag))
				{
					xLoc1 = x;
					yLoc1 = y;
					return 1;
				}
			}
		}

		int sign = -1;
		int i, j, k, limit;

		//-----------------------------------------------------------//
		// step 2
		//-----------------------------------------------------------//
		//y = yLoc + yOffset;
		limit = width + 2;
		for(i=1; i<limit; i++)
		{
			xOffset += sign * i;
			x = xLoc + xOffset;

			if(x>=0 && y>=0 && x+spaceLocWidth-1<MAX_WORLD_X_LOC && y+spaceLocHeight-1<MAX_WORLD_Y_LOC)
			{
				if(mobileType==UNIT_LAND || (x%2==0 && y%2==0))
				{
					locPtr = get_loc(x,y);

					if( locPtr->region_id == regionId &&
						 locPtr->is_plateau() == isPlateau &&
						 check_unit_space(x, y, x+spaceLocWidth-1, y+spaceLocHeight-1, mobileType, buildFlag))
					{
						xLoc1 = x;
						yLoc1 = y;
						return 1;
					}
				}
			}

			sign *= -1;
		}

		//-----------------------------------------------------------//
		// step 3
		//-----------------------------------------------------------//
		i = limit-1;

		limit = (height+1)*2;
		int r = sign*i;
		int lastX = xOffset;
		//int lastY = yOffset;

		for(j=0; j<limit; j++)
		{
			if(j%2)
			{
				//x = xLoc + lastX;
				xOffset = lastX;
				x = xLoc + xOffset;
				//y = yLoc + yOffset;

				if(x>=0 && y>=0 && x+spaceLocWidth-1<MAX_WORLD_X_LOC && y+spaceLocHeight-1<MAX_WORLD_Y_LOC)
				{
					if(mobileType==UNIT_LAND || (x%2==0 && y%2==0))
					{
						locPtr = get_loc(x,y);

						if( locPtr->region_id == regionId &&
							 locPtr->is_plateau() == isPlateau &&
							 check_unit_space(x, y, x+spaceLocWidth-1, y+spaceLocHeight-1, mobileType, buildFlag))
						{
							xLoc1 = x;
							yLoc1 = y;
							return 1;
						}
					}
				}
			}
			else
			{
				xOffset = lastX + r;
				yOffset--;

				x = xLoc + xOffset;
				y = yLoc + yOffset;

				if(x>=0 && y>=0 && x+spaceLocWidth-1<MAX_WORLD_X_LOC && y+spaceLocHeight-1<MAX_WORLD_Y_LOC)
				{
					if(mobileType==UNIT_LAND || (x%2==0 && y%2==0))
					{
						locPtr = get_loc(x,y);

						if( locPtr->region_id == regionId &&
							 locPtr->is_plateau() == isPlateau &&
							 check_unit_space(x, y, x+spaceLocWidth-1, y+spaceLocHeight-1, mobileType, buildFlag))
						{
							xLoc1 = x;
							yLoc1 = y;
							return 1;
						}
					}
				}
			}
		}

		//-----------------------------------------------------------//
		// step 4
		//-----------------------------------------------------------//
		y = yLoc + yOffset;
		for(k=0; k<=width; k++)
		{
			sign *= -1;
			i--;
			r = sign*i;
			xOffset -= r;

			x = xLoc + xOffset;

			if(x>=0 && y>=0 && x+spaceLocWidth-1<MAX_WORLD_X_LOC && y+spaceLocHeight-1<MAX_WORLD_Y_LOC)
			{
				if(mobileType==UNIT_LAND || (x%2==0 && y%2==0))
				{
					locPtr = get_loc(x,y);

					if( locPtr->region_id == regionId &&
						 locPtr->is_plateau() == isPlateau &&
						 check_unit_space(x, y, x+spaceLocWidth-1, y+spaceLocHeight-1, mobileType, buildFlag))
					{
						xLoc1 = x;
						yLoc1 = y;
						return 1;
					}
				}
			}
		}

		//-----------------------------------------------------------//
		// re-init the parameters
		//-----------------------------------------------------------//
		if(xLoc<=0 && yLoc<=0 && width>=MAX_WORLD_X_LOC && height>=MAX_WORLD_Y_LOC)
			break;   // the whole map has been checked

		width += 2;
		height += 2;

		xLoc -= 1;
		yLoc -= 1;
		if(xLoc<0)
		{
			xLoc = 0;
			width--;
		}
		if(yLoc<0)
		{
			yLoc=0;
			height--;
		}

		if(xLoc+width>MAX_WORLD_X_LOC)
			width--;
		if(yLoc+height>MAX_WORLD_Y_LOC)
			height--;

		//if(width==xLoc2-xLoc1+spaceLocWidth && height==yLoc2-yLoc1+spaceLocHeight) // terminate the checking
		// return 0;
	}

	return 0;
}
//-------- End of function World::locate_space ------//


//-------- Begin of function World::locate_space_random ------//
//
// Locate an area of space in the world map randomly. Pick any
// space available in that area without a specific scanning order.
//
// <int&> xLoc1          = the scaning range, also for returning the result location
// <int&> yLoc1          = the scaning range
// <int>  xLoc2          = the scaning range
// <int>  yLoc2          = the scaning range
// <int>  spaceLocWidth  = the location width of the required space
// <int>  spaceLocHeight = the location height of the required space
// <int>  maxTries       = maximum no. of tries
// [int]  regionId		 = if this is specified, the result location will
//									be in this region.
// [int]  buildSite      = whether locating space for building a site
//                         (default: 0)
// [char] teraMask		 = terrain mask (default: 1)
//
// return : <int> 1 - free space found
//                0 - free space found
//
int World::locate_space_random(int& xLoc1, int& yLoc1, int xLoc2, int yLoc2,
								int spaceLocWidth, int spaceLocHeight, int maxTries,
								int regionId, int buildSite, char teraMask)
{
   int       i, x, y, xTemp, xLoc, yLoc, canBuildFlag;
   int       scanWidth  = xLoc2-xLoc1-spaceLocWidth+2; //xLoc2-xLoc1+1-spaceLocWidth+1;
   int       scanHeight = yLoc2-yLoc1-spaceLocHeight+2; //yLoc2-yLoc1+1-spaceLocHeight+1;
	Location* locPtr;

   for( i=0 ; i<maxTries ; i++ )
   {
      xLoc = xLoc1 + m.random(scanWidth);
      yLoc = yLoc1 + m.random(scanHeight);
		canBuildFlag=1;

		//---------- check if the area is all free ----------//

		xTemp = xLoc+spaceLocWidth-1;

		for( y=yLoc+spaceLocHeight-1; y>=yLoc; y-- )
		{
			locPtr = world.get_loc(xTemp, y);

			for(x=xTemp; x>=xLoc; x--, locPtr-- )
			{
				if( ( buildSite ? !locPtr->can_build_site(teraMask) : !locPtr->can_build_firm(teraMask) ) ||
					 locPtr->is_power_off() )
				{
					canBuildFlag=0;
					break;
				}
			}

			if(!canBuildFlag)
				break;
		}

		if( !canBuildFlag )
			continue;

		//------ check region id. ------------//

		locPtr = world.get_loc(xLoc, yLoc);

		if( regionId && locPtr->region_id != regionId )
			continue;

		//------------------------------------//

		xLoc1 = xLoc;
		yLoc1 = yLoc;

		err_when(buildSite && !locPtr->can_build_site(teraMask));//-*** hard codes for mine size 3x3
		return 1;
	}

	return 0;
}
//-------- End of function World::locate_space_random ------//


//-------- Begin of function World::can_build_firm ---------//
//
// Check if it is free to construct a building on the specific area.
//
// <int>   xLoc1, yLoc1 = the coordination of the area to can_build
// <int>   firmId       = id. of the firm
// [short] unitRecno    = the unit recno of the unit to build the firm
//								  if the builder unit stands on the building area, still consider the area as buildable
//								  (default: -1, do not take the builder into account)
//
int World::can_build_firm(int xLoc1, int yLoc1, int firmId, short unitRecno)
{
	if( xLoc1 < 0 || yLoc1 < 0 || xLoc1 > max_x_loc || yLoc1 > max_y_loc )	
		return 0;		

	//--- if the unit's region is different from the destination's region, do nothing ---//

	if( unitRecno > 0 )
	{
		Unit* unitPtr = unit_array[unitRecno];

		if( unitPtr->is_visible() &&
			 unitPtr->region_id() != get_loc(xLoc1, yLoc1)->region_id )
		{
			return 0;
		}
	}

	//------------------------------------------//

	FirmInfo* firmInfo = firm_res[firmId];

	int xLoc, yLoc;
	int xLoc2 = xLoc1 + firmInfo->loc_width - 1;
	int yLoc2 = yLoc1 + firmInfo->loc_height - 1;
	if(xLoc2>=max_x_loc || yLoc2>=max_y_loc)
		return 0;

	int xLoc3, yLoc3, xLoc4, yLoc4;
	xLoc3 = max( xLoc1-INTER_PLACE_SPACE, 0 );
	yLoc3 = max( yLoc1-INTER_PLACE_SPACE, 0 );
	xLoc4 = min( xLoc2+INTER_PLACE_SPACE, max_x_loc-1 );
	yLoc4 = min( yLoc2+INTER_PLACE_SPACE, max_y_loc-1 );

	//-----------------------------------------//

	Location* locPtr;
	char teraMask, pierFlag;

	switch(firmInfo->tera_type)
	{
	case 1:		// default : land firm
	case 2:		// sea firm
	case 3:		// land or sea firm
		teraMask = firmInfo->tera_type;
		for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
		{
			locPtr = get_loc(xLoc1, yLoc);

			for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
			{
				// allow the building unit to stand in the area
				if(!locPtr->can_build_firm(teraMask) &&
					// ##### begin Gilbert 5/11 ######//
					!locPtr->can_clear_obstacle() &&
					// ##### end Gilbert 5/11 ######//
					!(locPtr->unit_recno(UNIT_LAND) && locPtr->unit_recno(UNIT_LAND)==unitRecno) )
					return 0;

				// don't allow building on a site except a mine on a raw site
				if( locPtr->has_site() && !((firmId == FIRM_MINE || firmId == FIRM_ALCHEMY) && site_array[locPtr->site_recno()]->site_type == SITE_RAW) )
					return 0;
			}
		}

		// scan two space wider 

		for( yLoc=yLoc3 ; yLoc<=yLoc4 ; yLoc++ )
		{
			locPtr = get_loc(xLoc3, yLoc);
			for( xLoc=xLoc3 ; xLoc<=xLoc4 ; xLoc++, locPtr++ )
			{
				// ##### begin Gilbert 5/11 ######//
				if( !locPtr->walkable(teraMask) && !locPtr->can_clear_obstacle() )
				// ##### end Gilbert 5/11 ######//
					return 0;

				// don't allow building near a raw site

				if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW 
					&& (xLoc < xLoc1 || xLoc > xLoc2 || yLoc < yLoc1 || yLoc > yLoc2) )
					return 0;

			}
		}

		return 1;

	case 4:				// special firm, such as harbor
		// must be 3x3,
		// centre square of one side is land (teraMask=1),
		// two squares on that side can be land or sea (teraMask=3)
		// and other (6 squares) are sea (teraMask=2)
		if( firmInfo->loc_width != 3 ||
			firmInfo->loc_height != 3)
			return 0;

		pierFlag = 1|2|4|8;		// bit0=north, bit1=south, bit2=west, bit3=east
		for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
		{
			locPtr = get_loc(xLoc1, yLoc);

			for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
			{
				// don't allow building on a site except a mine on a raw site
				if( locPtr->has_site() && !( (firmId == FIRM_MINE || firmId == FIRM_ALCHEMY) && site_array[locPtr->site_recno()]->site_type == SITE_RAW) )
					return 0;

static char northPierTera[3][3] = { {2,2,2},{2,2,2},{3,1,3} };
static char southPierTera[3][3] = { {3,1,3},{2,2,2},{2,2,2} };
static char westPierTera[3][3] = { {2,2,3},{2,2,1},{2,2,3} };
static char eastPierTera[3][3] = { {3,2,2},{1,2,2},{3,2,2} };
				int x = xLoc - xLoc1;
				int y = yLoc - yLoc1;
				if(!locPtr->can_build_harbor(northPierTera[y][x]) )
					pierFlag &= ~1;
				if(!locPtr->can_build_harbor(southPierTera[y][x]) )
					pierFlag &= ~2;
				if(!locPtr->can_build_harbor(westPierTera[y][x]) )
					pierFlag &= ~4;
				if(!locPtr->can_build_harbor(eastPierTera[y][x]) )
					pierFlag &= ~8;
			}
		}
		err_when( pierFlag != 0 && pierFlag != 1 && pierFlag != 2 &&
			pierFlag != 4 && pierFlag != 8 );

		// scan two space wider 

		for( yLoc=yLoc3 ; yLoc<=yLoc4 ; yLoc++ )
		{
			locPtr = get_loc(xLoc3, yLoc);
			for( xLoc=xLoc3 ; xLoc<=xLoc4 ; xLoc++, locPtr++ )
			{
				// ##### begin Gilbert 5/11 #######//
				if( !locPtr->walkable(3) && !locPtr->can_clear_obstacle() )
				// ##### end Gilbert 5/11 #######//
					return 0;

				// don't allow building near a raw site

				if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW 
					&& (xLoc < xLoc1 || xLoc > xLoc2 || yLoc < yLoc1 || yLoc > yLoc2) )
					return 0;
			}
		}

		return pierFlag;
		break;

	// other tera_type here

	default:
		err_here();
		return 0;
	}
}
//--------- End of function World::can_build_firm ---------//


//-------- Begin of function World::can_build_town ---------//
//
// <int>   xLoc1, yLoc1 = the coordination of the area to can_build
// [short] unitRecno    = the unit recno of the unit to build the town
//								  if the builder unit stands on the building area, still consider the area as buildable
//								  (default: -1, do not take the builder into account)
//
int World::can_build_town(int xLoc1, int yLoc1, short unitRecno)
{
	int xLoc, yLoc;
	int xLoc2 = xLoc1 + STD_TOWN_LOC_WIDTH  - 1;
	int yLoc2 = yLoc1 + STD_TOWN_LOC_HEIGHT - 1;

	if(xLoc2>=max_x_loc || yLoc2>=max_y_loc)
		return 0;

	//--- if the unit's region is different from the destination's region, do nothing ---//

	if( unitRecno > 0 )
	{
		Unit* unitPtr = unit_array[unitRecno];

		if( unitPtr->is_visible() &&
			 unitPtr->region_id() != get_loc(xLoc1, yLoc1)->region_id )
		{
			return 0;
		}
	}

	//-------------------------------------------------------//

	Location* locPtr;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		locPtr = get_loc(xLoc1, yLoc);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			// allow the building unit to stand in the area

			if( !locPtr->can_build_town() &&
				// ###### begin Gilbert 5/11 ######//
				!locPtr->can_clear_obstacle() &&
				// ###### end Gilbert 5/11 ######//
				!(locPtr->unit_recno(UNIT_LAND) && locPtr->unit_recno(UNIT_LAND)==unitRecno) )
			{
				return 0;
			}
		}
	}

	// scan two space wider

	int xLoc3, yLoc3, xLoc4, yLoc4;
	xLoc3 = max( xLoc1-INTER_PLACE_SPACE, 0 );
	yLoc3 = max( yLoc1-INTER_PLACE_SPACE, 0 );
	xLoc4 = min( xLoc2+INTER_PLACE_SPACE, max_x_loc-1 );
	yLoc4 = min( yLoc2+INTER_PLACE_SPACE, max_y_loc-1 );

	for( yLoc=yLoc3 ; yLoc<=yLoc4 ; yLoc++ )
	{
		locPtr = get_loc(xLoc3, yLoc);

		for( xLoc=xLoc3 ; xLoc<=xLoc4 ; xLoc++, locPtr++ )
		{
			if( !locPtr->walkable(LOCATE_WALK_LAND) 
				// ###### begin Gilbert 5/11 ######//
				&& !locPtr->can_clear_obstacle() )
				// ###### end Gilbert 5/11 ######//
				return 0;

			// don't allow building near/on a raw site

			if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW )
				return 0;
		}
	}

	return 1;
}
//--------- End of function World::can_build_town ---------//
//-------- Begin of function World::can_build_site ---------//
//
int World::can_build_site(int xLoc1, int yLoc1, int siteWidth, int siteHeight, int outerSpace )
{
	Location *locPtr;

	int xLoc, yLoc;
	int xLoc2 = xLoc1 + siteWidth - 1;
	int yLoc2 = yLoc1 + siteHeight - 1;

	if( xLoc2 >= max_x_loc || yLoc2 >= max_y_loc )
		return 0;

	for( yLoc = yLoc1; yLoc <= yLoc2; ++yLoc )
	{
		locPtr = get_loc(xLoc1, yLoc);
		for( xLoc = xLoc1; xLoc <= xLoc2; ++xLoc, ++locPtr)
		{
			if( !locPtr->can_build_site() )
				return 0;
		}
	}

	// scan two space wider

	if( outerSpace > 0 )
	{
		int xLoc3, yLoc3, xLoc4, yLoc4;
		xLoc3 = max( xLoc1-outerSpace, 0 );
		yLoc3 = max( yLoc1-outerSpace, 0 );
		xLoc4 = min( xLoc2+outerSpace, max_x_loc-1 );
		yLoc4 = min( yLoc2+outerSpace, max_y_loc-1 );

		for( yLoc=yLoc3 ; yLoc<=yLoc4 ; ++yLoc )
		{
			locPtr = get_loc(xLoc3, yLoc);
			for( xLoc=xLoc3 ; xLoc<=xLoc4 ; ++xLoc, ++locPtr )
			{
				if( !locPtr->walkable(LOCATE_WALK_LAND) 
					&& !locPtr->can_clear_obstacle() )
					return 0;

				// no any other raw site in outer area
				if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW )
					return 0;
			}
		}
	}

	return 1;
}
//-------- End of function World::can_build_site ---------//



//-------- Begin of function World::can_build_wall ---------//
//
// <int> xLoc, yLoc  = the coordination of the area to can_build
// <int> nationRecno = recno of the builder nation.
//
int World::can_build_wall(int xLoc, int yLoc, short nationRecno)
{
	Location* locPtr = get_loc(xLoc, yLoc);

	return locPtr->can_build_wall() && locPtr->power_nation_recno == nationRecno;
}
//--------- End of function World::can_build_wall ---------//


//-------- Begin of function World::can_destruct_wall ---------//
//
// <int> xLoc, yLoc  = the coordination of the area to can_build
// <int> nationRecno = recno of the builder nation.
//
int World::can_destruct_wall(int xLoc, int yLoc, short nationRecno)
{
	Location* locPtr = get_loc(xLoc, yLoc);

	return locPtr->is_wall() && locPtr->power_nation_recno == nationRecno;
}
//--------- End of function World::can_destruct_wall ---------//


//---------- Begin of function World::draw_link_line -----------//
//
// <int> srcFirmId          - id. of the source firm.
//                            0 if the source is a town
// <int> srcTownRecno       - town recno of the source town
//                            0 if the source is a firm
// <int> srcXLoc1, srcYLoc1 - the location of the source area
//       srcXLoc2, srcYLoc2
//
// [int] giveEffectiveDis   - use this value as the effective distance if this is given
//										(default: 0)
//
void World::draw_link_line(int srcFirmId, int srcTownRecno, int srcXLoc1,
									int srcYLoc1, int srcXLoc2, int srcYLoc2, int givenEffectiveDis)
{
	if( srcFirmId == FIRM_INN ) 	// FirmInn's link is only for scan for neighbor inns quickly, the link line is not displayed
		return;

	//--------------------------------------//

	int srcXLoc = (srcXLoc1 + srcXLoc2)/2;
	int srcYLoc = (srcYLoc1 + srcYLoc2)/2;

	int altitude = world.get_corner(srcXLoc1, srcYLoc1)->get_altitude();
	int srcX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
		(srcXLoc1+srcXLoc2+1)*LOCATE_WIDTH/2, (srcYLoc1+srcYLoc2+1)*LOCATE_HEIGHT/2, 
		altitude);
	int srcY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
		(srcXLoc1+srcXLoc2+1)*LOCATE_WIDTH/2, (srcYLoc1+srcYLoc2+1)*LOCATE_HEIGHT/2, 
		altitude);

	//------- draw lines connected to town ---------//

	int   i, townX, townY, effectiveDis;
	Town* townPtr;

	if( givenEffectiveDis )
		effectiveDis = givenEffectiveDis;
	else
		effectiveDis = world.effective_distance(srcFirmId, 0);

	if( !srcFirmId || firm_res[srcFirmId]->is_linkable_to_town )    // don't draw link line to town if it's an inn
	{
		for( i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) || i == srcTownRecno )		// don't draw the same town recno
				continue;

			townPtr = town_array[i];

			//-------- check the distance --------//

			if( m.points_distance( townPtr->center_x, townPtr->center_y,
				 srcXLoc, srcYLoc ) > effectiveDis )
			{
				continue;
			}

			// ##### begin Gilbert 6/7 ########//
			if( townPtr->is_stealth() )
				continue;
			// ##### end Gilbert 6/7 ########//

			//------ check if both are on the same terrain type ------//

			// if( (world.get_loc(townPtr->center_x, townPtr->center_y)->is_plateau()==1)
			//	 != (world.get_loc(srcXLoc, srcYLoc)->is_plateau()==1) )
			// use loc_x1/y1 instead of center_x/y to comply with world.locate_space
			if( (world.get_loc(townPtr->loc_x1, townPtr->loc_y1)->is_plateau()==1)
				 != (world.get_loc(srcXLoc1, srcYLoc1)->is_plateau()==1) )
			{
				continue;
			}

			//---------- draw line now -----------//

			townX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
				(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
				(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
				townPtr->altitude);
			townY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
				(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
				(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
				townPtr->altitude);

			anim_line.draw_line(&vga_back, srcX, srcY, townX, townY);
		}
	}

	//------- draw lines connected to firms ---------//

	int   firmX, firmY, linkFlag;
	Firm* firmPtr;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
         continue;

		firmPtr = firm_array[i];

		//------ only link if the firms have relationship -----//

		if( srcFirmId )
			linkFlag = firm_res[firmPtr->firm_id]->is_linkable_to_firm(srcFirmId);
		else
			linkFlag = firm_res[firmPtr->firm_id]->is_linkable_to_town;

		if( !linkFlag )
			continue;

		//-------- check the distance --------//

		// ######## begin Gilbert 28/4 ##########//
		if( !givenEffectiveDis )
			effectiveDis = world.effective_distance(srcFirmId, firmPtr->firm_id);
		// ######## end Gilbert 28/4 ##########//

		if( m.points_distance( firmPtr->center_x, firmPtr->center_y, srcXLoc, srcYLoc ) > effectiveDis )
			continue;

		// ##### begin Gilbert 6/7 ########//
		if( firmPtr->is_stealth() )
			continue;
		// ##### end Gilbert 6/7 ########//

		//------ check if both are on the same terrain type ------//

		// if( (world.get_loc(firmPtr->center_x, firmPtr->center_y)->is_plateau()==1)
		//	 != (world.get_loc(srcXLoc, srcYLoc)->is_plateau()==1) )
		if( (world.get_loc(firmPtr->loc_x1, firmPtr->loc_y1)->is_plateau()==1)
			!= (world.get_loc(srcXLoc1, srcYLoc1)->is_plateau()==1) )
		{
			continue;
		}

		//---------- draw line now -----------//

		firmX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);
		firmY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);

		anim_line.draw_line(&vga_back, srcX, srcY, firmX, firmY);
	}
}
//----------- End of function World::draw_link_line ------------//


//-------- Begin of function World::set_surr_power_off ---------//
void World::set_surr_power_off(int xLoc, int yLoc)
{
	if(xLoc>0) // west
		get_loc(xLoc-1, yLoc)->set_power_off();

	if(xLoc<max_x_loc-1)
		get_loc(xLoc+1, yLoc)->set_power_off();
	
	if(yLoc>0) // north
		get_loc(xLoc, yLoc-1)->set_power_off();

	if(yLoc<max_y_loc-1) // south
		get_loc(xLoc, yLoc+1)->set_power_off();
}
//----------- End of function World::set_surr_power_off ------------//


//-------- Begin of function World::set_all_power ---------//
//
void World::set_all_power()
{
	//--------- set town's influence -----------//

	Town* townPtr;
	int i;

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( !townPtr->nation_recno )
			continue;

		//------- set the influence range of this town -----//

		set_power(townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2, (char)townPtr->nation_recno);
	}

	//--------- set firm's influence -----------//

	Firm* firmPtr;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( !firmPtr->nation_recno )
			continue;

		if( !firmPtr->should_set_power )
			continue;

		//------- set the influence range of this firm -----//

		set_power(firmPtr->loc_x1, firmPtr->loc_y1, firmPtr->loc_x2, firmPtr->loc_y2, (char)firmPtr->nation_recno);
	}
}
//--------- End of function World::set_all_power ---------//


//-------- Begin of function World::set_power ---------//
//
// <int> xLoc1, yLoc1, - area on the map which the power should be set
//		   xLoc2, yLoc2
//
// <int> nationRcno - nation recno
//
void World::set_power(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int nationRecno)
{
	//------- reset power_nation_recno first ------//

	// use xLoc1, yLoc1 instead of center location to comply with world.locate_space
	// int	plateauResult = (get_loc((xLoc1+xLoc2)/2, (yLoc1+yLoc2)/2)->is_plateau()==1);
	int	plateauResult = (get_loc(xLoc1, yLoc1)->is_plateau()==1);

	int   	 xLoc, yLoc, centerY, t;
	Location* locPtr = loc_matrix;

	xLoc1 = max( 0, xLoc1 - EFFECTIVE_POWER_DISTANCE+1);
	yLoc1 = max( 0, yLoc1 - EFFECTIVE_POWER_DISTANCE+1);
	xLoc2 = min( MAX_WORLD_X_LOC-1, xLoc2 + EFFECTIVE_POWER_DISTANCE-1);
	yLoc2 = min( MAX_WORLD_Y_LOC-1, yLoc2 + EFFECTIVE_POWER_DISTANCE-1);

	centerY = (yLoc1+yLoc2) / 2;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		t=abs(yLoc-centerY)/2;

		for( xLoc=xLoc1+t ; xLoc<=xLoc2-t ; xLoc++, locPtr++ )
		{
			locPtr = get_loc(xLoc, yLoc);

			if(locPtr->sailable())//if(!locPtr->walkable())
				continue;

			if(locPtr->is_power_off())
				continue;

			if((locPtr->is_plateau()==1) != plateauResult)
				continue;

			if(locPtr->power_nation_recno==0)
			{
				locPtr->power_nation_recno = nationRecno;
				sys.map_need_redraw = 1;						// request redrawing the map next time
			}
		}
	}
}
//--------- End of function World::set_power ---------//


//-------- Begin of function World::restore_power ---------//
//
// <int> xLoc1, yLoc1, - area on the map which the power should be restored
//		   xLoc2, yLoc2
//
// <int> townRecno, firmRecno - either one
//
void World::restore_power(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int townRecno, int firmRecno)
{
	int nationRecno;

	if( townRecno )
	{
		nationRecno = town_array[townRecno]->nation_recno;
		town_array[townRecno]->nation_recno = 0;
	}

	if( firmRecno )
	{
		nationRecno = firm_array[firmRecno]->nation_recno;
		firm_array[firmRecno]->nation_recno = 0;
	}

	//------- reset power_nation_recno first ------//

	int   	 xLoc, yLoc, centerY, t;
	Location* locPtr = loc_matrix;

	xLoc1 = max( 0, xLoc1 - EFFECTIVE_POWER_DISTANCE+1);
	yLoc1 = max( 0, yLoc1 - EFFECTIVE_POWER_DISTANCE+1);
	xLoc2 = min( MAX_WORLD_X_LOC-1, xLoc2 + EFFECTIVE_POWER_DISTANCE-1);
	yLoc2 = min( MAX_WORLD_Y_LOC-1, yLoc2 + EFFECTIVE_POWER_DISTANCE-1);

	centerY = (yLoc1+yLoc2) / 2;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		t=abs(yLoc-centerY)/2;

		for( xLoc=xLoc1+t ; xLoc<=xLoc2-t ; xLoc++, locPtr++ )
		{
			locPtr = get_loc(xLoc, yLoc);

			if( locPtr->power_nation_recno==nationRecno )
			{
				locPtr->power_nation_recno = 0;
				sys.map_need_redraw = 1;						// request redrawing the map next time
			}
		}
	}

	//--- if some power areas are freed up, see if neighbor towns/firms should take up these power areas ----//

	if( sys.map_need_redraw )	// when calls set_all_power(), the nation_recno of the calling firm must be reset
		set_all_power();

	//------- restore the nation recno of the calling town/firm -------//

	if( townRecno )
		town_array[townRecno]->nation_recno = nationRecno;

	if( firmRecno )
		firm_array[firmRecno]->nation_recno = nationRecno;
}
//--------- End of function World::restore_power ---------//


//-------- Begin of function World::detect_firm_town ---------//
//
int World::detect_firm_town()
{
	// ##### begin Gilbert 19/9 ########//
	// int rc = mouse.single_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 2);
	if( !mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 2) )
		return 0;
	// ##### end Gilbert 19/9 ########//

	//------ detect pressing on link enable/disable sign -----//

	Firm* firmPtr;

	if( firm_array.selected_recno )
	{
		firmPtr = firm_array[firm_array.selected_recno];

		if( firmPtr->should_show_info() &&				// only if should_show_info() is 1, we can detect links from this firm (it is not limited to player firms, as firms with player's workers should be allowed for resigning the player worker from the firm
			 firmPtr->draw_detect_link_line(1) )		// 1-detect action
		{
			return 1;
		}
	}

	Town* townPtr;

	if( town_array.selected_recno )
	{
		townPtr = town_array[town_array.selected_recno];

		if( townPtr->nation_recno==nation_array.player_recno &&
			 townPtr->draw_detect_link_line(1) )		// 1-detect action
		{
			return 1;
		}
	}

	// ####### begin Gilbert 12/9 #########// see Power::detect_select
/*
	//--------------- detect firm ------------------//

	if( rc==1 )			// left click 
	{
		int mouseAbsX = mouse.cur_x - ZOOM_X1 + World::view_top_x;
		int mouseAbsY = mouse.cur_y - ZOOM_Y1 + World::view_top_y;

		int  i;

		for( i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			firmPtr = firm_array[i];

			if( m.is_touch( mouseAbsX, mouseAbsY, mouseAbsX, mouseAbsY,
				firmPtr->abs_x1, firmPtr->abs_y1, firmPtr->abs_x2, firmPtr->abs_y2 ) )
			{
				power.reset_selection();
				firm_array.selected_recno = i;
				info.disp();

				// -------- sound effect -----------//

				if( firmPtr->nation_recno == nation_array.player_recno && se_res.mark_select_object_time() )
				{
					se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
						'F', firmPtr->firm_id, firmPtr->under_construction ? "SELU" : "SEL" );
				}
				return 1;
			}
		}

		//----------- detect town section --------------//

		for( i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) )
				continue;

			townPtr = town_array[i];

			if( m.is_touch( mouseAbsX, mouseAbsY, mouseAbsX, mouseAbsY,
				 townPtr->abs_x1, townPtr->abs_y1, townPtr->abs_x2, townPtr->abs_y2 ) )
			{
				power.reset_selection();
				town_array.selected_recno = i;
				info.disp();

				// -------- sound effect -----------//

				if( townPtr->nation_recno == nation_array.player_recno 
					&& se_res.mark_select_object_time() )
				{
					se_res.sound(townPtr->center_x, townPtr->center_y, 1,
						'T', 0, "SEL" );
				}
				return 1;
			}
		}
	}
	*/

	return 0;
}
//-------- End of function World::detect_firm_town ---------//


//-------- Begin of function World::earth_equake ------//
void World::earth_quake()
{
	Location *locPtr;
	int x,y;
	for(y = 0; y < max_y_loc; ++y)
	{
		locPtr = get_loc(0,y);
		for( x = 0; x < max_x_loc; ++x, ++locPtr)
		{
			if(locPtr->is_wall() )
			{
				locPtr->attack_wall( weather.quake_rate(x,y) /2 );
			}
		}
	}

	int firmDamage = 0;
	int firmDie = 0;
	int i;
	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) || !firm_res[firm_array[i]->firm_id]->buildable)
			continue;

		Firm *firmPtr = firm_array[i];
		x = firmPtr->center_x;
		y = firmPtr->center_y;
		// ####### begin gilbert 10/3 #########//
		if( weather.quake_rate(x,y) <= 0 )
			continue;
		// ####### end gilbert 10/3 #########//
		firmPtr->hit_points -= weather.quake_rate(x,y);
		if( firmPtr->is_own() )
			firmDamage++;
		if( firmPtr->hit_points <= 0)
		{
			firmPtr->hit_points = (float) 0;
			if( firmPtr->is_own() )
				firmDie++;
			se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
				'F', firmPtr->firm_id, "DIE" );
			firm_array.del_firm(i);
		}
	}

	int townDamage = 0;
	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town *townPtr = town_array[i];

		if( townPtr->population==0 )
			continue;

		int ownTown = townPtr->nation_recno == nation_array.player_recno;
		short beforePopulation = townPtr->population;

		int wallDamage = weather.quake_rate(townPtr->center_x, townPtr->center_y);
		// ####### begin gilbert 10/3 #########//
		if( wallDamage <= 0 )
			continue;
		// ####### end gilbert 10/3 #########//

		if( townPtr->hit_points >= wallDamage )
		{
			townPtr->hit_points -= wallDamage;
			wallDamage = wallDamage / 2;		// reduce causalty by half
		}
		else
		{
			wallDamage = wallDamage - (int)townPtr->hit_points / 2;
			townPtr->hit_points = (float) 0;
		}

		// ####### patch begin Gilbert 18/1 #######//
		// kill less people
		// short causalty = wallDamage / 10;
		short causalty = wallDamage / 20;
		// ####### patch end Gilbert 18/1 #######//
		for( ; causalty > 0 && townPtr->population>0 ; --causalty )
		{
			townPtr->kill_town_people(0);
		}
		causalty = beforePopulation - townPtr->population;

		if(ownTown)
			townDamage += causalty;
	}

	int unitDamage = 0;
	int unitDie = 0;
	for( i=unit_array.size(); i > 0; i-- )
	{
		if( unit_array.is_deleted(i))
			continue;

		Unit *unitPtr = unit_array[i];

		// ###### begin Gilbert 30/8 ######//
		// no damage to air unit , sea unit or overseer
		if( !unitPtr->is_visible() || unitPtr->mobile_type == UNIT_AIR
			|| unitPtr->mobile_type == UNIT_SEA)
			continue;
		// ###### end Gilbert 30/8 ######//

		// ####### patch begin Gilbert 18/1 #######//
//		float damage = (float) weather.quake_rate(unitPtr->cur_x_loc(),unitPtr->cur_y_loc() ) *
//			unitPtr->max_hit_points() / 200;
		float damage = (float) weather.quake_rate(unitPtr->cur_x_loc(),unitPtr->cur_y_loc() ) *
			unitPtr->max_hit_points() / 400;
		// ####### patch end Gilbert 18/1 #######//

		// ######## begin Gilbert 10/3 ########//
		if( damage <= 0 )
			continue;
		// ######## end Gilbert 10/3 ########//

		if( damage >= unitPtr->hit_points)
			damage = unitPtr->hit_points -1;
		if( damage < (float) 5)
			damage = (float) 5;

		unitPtr->hit_points -= damage;
		if( unitPtr->is_own() )
			unitDamage++;

		if( unitPtr->hit_points <= 0)
		{
			unitPtr->hit_points = (float) 0;
			if( unitPtr->is_own() )
				unitDie++;
		}
		else
		{
		/*
			if( unit_res[unitPtr->unit_id]->solider_id &&
				weather.quake_rate(unitPtr->cur_x_loc(),unitPtr->cur_y_loc()) >= 60)
			{
				((UnitVehicle *)unitPtr)->dismount();
			}
		*/
		}
	}

	news_array.earthquake_damage(unitDamage-unitDie, unitDie, townDamage, firmDamage-firmDie, firmDie);
}
//-------- End of function World::earth_equake ------//


//-------- Begin of function World::lightning_strike ------//
void World::lightning_strike(short cx, short cy, short radius)
{
	short x, y;
	for( y = cy-radius; y <= cy+radius; ++y)
	{
		if( y < 0 || y >= max_y_loc)
			continue;

		for( x = cx-radius; x <= cx+radius; ++x)
		{
			if( x < 0 || x >= max_x_loc)
				continue;

			Location *locPtr = get_loc(x,y);
			if( locPtr->is_plant() )
			{
				// ---- add a fire on it ------//
				locPtr->set_fire_str(80);
				// ##### begin Gilbert 11/8 #####//
				if( locPtr->can_set_fire() && locPtr->fire_str() < 5 )
					locPtr->set_fire_str(5);
				// ##### end Gilbert 11/8 #####//
			}
		}
	}

	// ------ check hitting units -------//
	int i;
	for( i=unit_array.size(); i > 0; i-- )
	{
		if( unit_array.is_deleted(i))
			continue;

		Unit *unitPtr = unit_array[i];

		// no damage to overseer
		if( !unitPtr->is_visible())
			continue;

		if( unitPtr->cur_x_loc() <= cx+ radius &&
			unitPtr->cur_x_loc() + unitPtr->sprite_info->loc_width > cx-radius &&
			unitPtr->cur_y_loc() <= cy+radius &&
			unitPtr->cur_y_loc() + unitPtr->sprite_info->loc_height > cy-radius )
		{
			unitPtr->hit_points -= (float) unitPtr->sprite_info->lightning_damage / ATTACK_SLOW_DOWN;

			// ---- add news -------//
			if( unitPtr->is_own() )
				news_array.lightning_damage(unitPtr->cur_x_loc(), unitPtr->cur_y_loc(),
					NEWS_LOC_UNIT, i, unitPtr->hit_points <= (float) 0);

			if( unitPtr->hit_points <= 0)
				unitPtr->hit_points = (float) 0;
		}
	}

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) || !firm_res[firm_array[i]->firm_id]->buildable)
			continue;

		Firm *firmPtr = firm_array[i];
		if( firmPtr->loc_x1 <= cx+radius &&
			firmPtr->loc_x2 >= cx-radius &&
			firmPtr->loc_y1 <= cy+radius &&
			firmPtr->loc_y2 >= cy-radius)
		{
			firmPtr->hit_points -= (float) 50 / ATTACK_SLOW_DOWN;

			// ---- add news -------//
			if( firmPtr->is_own() )
				news_array.lightning_damage(firmPtr->center_x, firmPtr->center_y,
					NEWS_LOC_FIRM, i, firmPtr->hit_points <= (float) 0);

			// ---- add a fire on it ------//
			Location *locPtr = get_loc(firmPtr->center_x, firmPtr->center_y);
			if( locPtr->can_set_fire() && locPtr->fire_str() < 5 )
				locPtr->set_fire_str(5);

			if( firmPtr->hit_points <= 0)
			{
				firmPtr->hit_points = (float) 0;
				se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
					'F', firmPtr->firm_id, "DIE" );
				firm_array.del_firm(i);
			}
		}
	}

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i))
			continue;

		Town *townPtr = town_array[i];

		if( townPtr->loc_x1 <= cx+radius &&
			townPtr->loc_x2 >= cx-radius &&
			townPtr->loc_y1 <= cy+radius &&
			townPtr->loc_y2 >= cy-radius)
		{
			// ---- add news -------//
			if( townPtr->nation_recno == nation_array.player_recno )
				news_array.lightning_damage(townPtr->center_x, townPtr->center_y,
					NEWS_LOC_TOWN, i, 0);

			// ---- add a fire on it ------//
			// ####### begin Gilbert 11/8 #########//
			Location *locPtr = get_loc(townPtr->center_x, townPtr->center_y);
			if( locPtr->can_set_fire() && locPtr->fire_str() < 5)
				locPtr->set_fire_str(5);
			// ####### end Gilbert 11/8 #########//

			townPtr->kill_town_people(0);
		}
	}
}
//-------- End of function World::lightning_strike -------//


// ------- Begin of function World::visit -----------//
// set the visit_level surrounding unit, town and firm
void World::visit(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int range, int extend)
{
	if(config.fog_of_war)
	{
		// ##### begin Gilbert 18/5 #########//
		// no more 'extend' to make it faster
		range += extend;
		extend = 0;
		// ##### end Gilbert 18/5 #########//

		// ##### begin Gilbert 18/5 #########//
		//int left   = max( 0, xLoc1 - range);
		//int top    = max( 0, yLoc1 - range);
		//int right  = min( MAX_WORLD_X_LOC-1, xLoc2 + range);
		//int bottom = min( MAX_WORLD_Y_LOC-1, yLoc2 + range);
		int left   = xLoc1 - range;
		int top    = yLoc1 - range;
		int right  = xLoc2 + range;
		int bottom = yLoc2 + range;

		if( left < 0 )
			left = 0;
		if( top < 0 )
			top = 0;
		if( right >= MAX_WORLD_X_LOC )
			right = MAX_WORLD_X_LOC-1;
		if( bottom >= MAX_WORLD_Y_LOC )
			bottom = MAX_WORLD_Y_LOC-1;
		// ##### end Gilbert 18/5 #########//

		// ----- mark the visit_level of the square around the unit ------//
		for( int yLoc=top ; yLoc<=bottom ; yLoc++ )
		{
			Location *locPtr = get_loc(left, yLoc);
			for( int xLoc=left ; xLoc<=right ; xLoc++, locPtr++ )
			{
				locPtr->set_visited();
			}
		}

		// ----- visit_level decreasing outside the visible range ------//
		if( extend > 0)
		{
			int visitLevel = FULL_VISIBILITY;
			int levelDrop = (FULL_VISIBILITY - EXPLORED_VISIBILITY) / (extend+1);
			xLoc1 -= range;
			xLoc2 += range;
			yLoc1 -= range;
			yLoc2 += range;
			for( ++range; extend > 0; --extend, ++range)
			{
				xLoc1--;
				xLoc2++;
				yLoc1--;
				yLoc2++;
				visitLevel -= levelDrop;
				visit_shell(xLoc1, yLoc1, xLoc2, yLoc2, visitLevel);
			}
		}
	}
}
// ------- End of function World::visit -----------//


// ------- Begin of function World::visit_shell -----------//
// set specific visit_level on the surrounding unit, town and firm
void World::visit_shell(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int visitLevel)
{
	int left   = max( 0, xLoc1 );
	int top    = max( 0, yLoc1 );
	int right  = min( MAX_WORLD_X_LOC-1, xLoc2);
	int bottom = min( MAX_WORLD_Y_LOC-1, yLoc2);

	// ------- top side ---------//
	if( yLoc1 >= 0)
	{
		Location *locPtr = get_loc( left, yLoc1);
		for( int x = left; x <= right; ++x, ++locPtr)
			locPtr->set_visited(visitLevel);
	}

	// ------- bottom side ---------//
	if( yLoc2 < max_y_loc)
	{
		Location *locPtr = get_loc( left, yLoc2);
		for( int x = left; x <= right; ++x, ++locPtr)
			locPtr->set_visited(visitLevel);
	}

	// ------- left side -----------//
	if( xLoc1 >= 0)
	{
		for( int y = top; y <= bottom; ++y)
		{
			get_loc(xLoc1,y)->set_visited(visitLevel);
		}
	}

	// ------- right side -----------//
	if( xLoc2 < max_x_loc)
	{
		for( int y = top; y <= bottom; ++y)
		{
			get_loc(xLoc2,y)->set_visited(visitLevel);
		}
	}

}
// ------- End of function World::visit_shell -----------//


//------- Begin of function World::process_visibility -----------//

void World::process_visibility()
{
	if( sys.frame_count & 1 )
	{
		// odd frame
		// ##### patch begin Gilbert 18/1 ######//
		// with japanese SOP, visit level won't drop
		if( config.fog_of_war &&
			!(nation_array.player_recno && god_res[GOD_JAPANESE]->nation_prayer_count(nation_array.player_recno) > 0) )
		// ##### patch end Gilbert 18/1 ######//
		{
#ifdef DEBUG
			unsigned long startTime = m.get_time();
#endif
//			for( int y = 0; y < max_y_loc; ++y)
//			{
//				Location *locPtr = get_loc(0,y);
//				for( int x = 0; x < max_x_loc; ++x, ++locPtr)
//				{
//					locPtr->dec_visibility();
//				}
//			}

			int count = max_x_loc * max_y_loc;
			const int sizeOfLoc = sizeof(Location);
			unsigned char *locVisitLevel = &get_loc(0,0)->visit_level;
			// #### begin Gilbert 8/2 #######//
			// unsigned char decVisitLevel = EXPLORED_VISIBILITY*2+1;
			unsigned char decVisitLevel = EXPLORED_VISIBILITY;
			_asm
			{
				mov	ecx, count
				mov	ebx, locVisitLevel
				mov	edx, sizeOfLoc
				mov	ah, decVisitLevel
process_visit_level_1:
				mov	al,[ebx]
				cmp	ah, al			// if(EXPLORED_VISIBILITY < al) al--;
				jnb	process_visit_level2
				dec	byte ptr [ebx]
process_visit_level2:
				add	ebx, edx
				loop	process_visit_level_1
			}

#ifdef DEBUG
			startTime = m.get_time() - startTime;
			startTime =  0; // set break point here
#endif
		}
	}
	else
	{
		// odd frame
		if( !config.explore_whole_map || config.fog_of_war)
		{
			sys.zoom_need_redraw = 1;	// request world.zoom_matrix->disp() to call draw()
		}
	}
	// #### end Gilbert 8/2 #####//
}
//------- End of function World::process_visibility -----------//

//--------- Begin of function World::disp_next --------//
//
// Display the next object of the same type.
//
// <int> seekDir : -1 - display the previous one in the list.
// 					  1 - display the next one in the list.
//
// <int> sameNation - whether display the next object of the same
//							 nation only or of any nation.
//
void World::disp_next(int seekDir, int sameNation)
{
	if( map_matrix->filter_object_flag || map_matrix->filter_nation_flag )
	{
		disp_next_with_filter(seekDir);
		return;
	}

	//--- if the selected one is a unit ----//

	if( unit_array.selected_recno )
	{
		int unitRecno = unit_array.selected_recno;
		Unit* unitPtr = unit_array[unit_array.selected_recno];
		int unitClass = unit_res[unitPtr->unit_id]->unit_class;
		int nationRecno = unitPtr->nation_recno;

		while(1)
		{
			if( seekDir < 0 )
			{
				unitRecno--;

				if( unitRecno < 1 )
					unitRecno = unit_array.size();
			}
			else
			{
				unitRecno++;

				if( unitRecno > unit_array.size() )
					unitRecno = 1;
			}

			if( unit_array.is_deleted(unitRecno) )
				continue;

			unitPtr = unit_array[unitRecno];

			if( !unitPtr->is_visible() )
				continue;

			//--- check if the location of the unit has been explored ---//

			// ###### begin Gilbert 31/5 ########//
			// if( !world.get_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc())->explored() )
			if( unitPtr->is_stealth() )
			// ###### end Gilbert 31/5 ########//
				continue;

			//-------- if are of the same nation --------//

			if( sameNation && unitPtr->nation_recno != nationRecno )
				continue;

			//---------------------------------//

			if( unit_res[unitPtr->unit_id]->unit_class == unitClass )
			{
				power.reset_selection();
				unitPtr->select();
				unit_array.selected_recno = unitRecno;
				// ####### begin Gilbert 31/5 #########//
				// info.disp();
				world.go_loc( unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), 
					unitPtr->mobile_type == UNIT_AIR ? GO_LOC_SELECT_AIR : GO_LOC_SELECT_LAND );
				// ####### end Gilbert 31/5 #########//
				return;
			}

			//--- if the recno loops back to the starting one ---//

			if( unitRecno == unit_array.selected_recno )
				break;
		}
	}

	//--- if the selected one is a firm ----//

	if( firm_array.selected_recno )
	{
		int firmRecno = firm_array.selected_recno;
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		int firmId = firmPtr->firm_id;
		int nationRecno = firmPtr->nation_recno;

		while(1)
		{
			if( seekDir < 0 )
			{
				firmRecno--;

				if( firmRecno < 1 )
					firmRecno = firm_array.size();
			}
			else
			{
				firmRecno++;

				if( firmRecno > firm_array.size() )
					firmRecno = 1;
			}

			if( firm_array.is_deleted(firmRecno) )
				continue;

			firmPtr = firm_array[firmRecno];

			//-------- if are of the same nation --------//

			if( sameNation && firmPtr->nation_recno != nationRecno )
				continue;

			//--- check if the location of this firm has been explored ---//

			// ####### begin Gilbert 31/5 #########//
			// if( !world.get_loc(firmPtr->center_x, firmPtr->center_y)->explored() )
			if( firmPtr->is_stealth() )
			// ####### end Gilbert 31/5 #########//
				continue;

			//---------------------------------//

			if( firmPtr->firm_id == firmId )
			{

				power.reset_selection();
				firm_array.selected_recno = firmRecno;
				// ######## begin Gilbert 31/5 ########//
				// info.disp();
				world.go_loc( firmPtr->center_x, firmPtr->center_y, GO_LOC_SELECT_FIRM );
				// ######## end Gilbert 31/5 ########//
				return;
			}

			//--- if the recno loops back to the starting one ---//

			if( firmRecno == firm_array.selected_recno )
				break;
		}
	}

	//--- if the selected one is a town ----//

	if( town_array.selected_recno )
	{
		int 	townRecno = town_array.selected_recno;
		int   nationRecno = town_array[townRecno]->nation_recno;
		Town* townPtr;
		int 	isSlaveTown = town_array[townRecno]->is_pay_tribute_to_monster(nation_array.player_recno);

		while(1)
		{
			if( seekDir < 0 )
			{
				townRecno--;

				if( townRecno < 1 )
					townRecno = town_array.size();
			}
			else
			{
				townRecno++;

				if( townRecno > town_array.size() )
					townRecno = 1;
			}

			if( town_array.is_deleted(townRecno) )
				continue;

			townPtr = town_array[townRecno];

			//-------- if are of the same nation --------//

			if( sameNation && townPtr->nation_recno != nationRecno )
				continue;

			//--- check if the location of this town has been explored ---//

			// ####### begin Gilbert 31/5 #########//
			// if( !world.get_loc(townPtr->center_x, townPtr->center_y)->explored() )
			if( townPtr->is_stealth() )
			// ####### end Gilbert 31/5 #########//
				continue;

			//---- if the originally selected town is a slave town, the next town must be a slave town also ----//

//			if( isSlaveTown && !townPtr->is_pay_tribute_to_monster(nation_array.player_recno) )
//				continue;

			//---------------------------------//

			power.reset_selection();
			town_array.selected_recno = townRecno;

			// ######## begin Gilbert 31/5 ########//
			world.go_loc( townPtr->center_x, townPtr->center_y, GO_LOC_SELECT_TOWN );
			// ######## end Gilbert 31/5 ########//
			return;

			//--- if the recno loops back to the starting one ---//

			if( townRecno == town_array.selected_recno )
				break;
		}
	}

	//--- if the selected one is a natural resource site ----//

	if( site_array.selected_recno )
	{
		int   siteRecno = site_array.selected_recno;
		Site* sitePtr   = site_array[site_array.selected_recno];
		int   siteType  = sitePtr->site_type;

		while(1)
		{
			if( seekDir < 0 )
			{
				siteRecno--;

				if( siteRecno < 1 )
					siteRecno = site_array.size();
			}
			else
			{
				siteRecno++;

				if( siteRecno > site_array.size() )
					siteRecno = 1;
			}

			if( site_array.is_deleted(siteRecno) )
				continue;

			sitePtr = site_array[siteRecno];

			//--- check if the location of this site has been explored ---//

			// ####### begin Gilbert 31/5 #########//
			// if( !world.get_loc(sitePtr->map_x_loc, sitePtr->map_y_loc)->explored() )
			if( !world.is_explored(sitePtr->map_x_loc, sitePtr->map_y_loc,	sitePtr->map_x2, sitePtr->map_y2) )
			// ####### end Gilbert 31/5 #########//
				continue;

			//---------------------------------//

			if( sitePtr->site_type == siteType )
			{
				power.reset_selection();
				site_array.selected_recno = siteRecno;

				// ####### begin Gilbert 31/5 ###########//
				world.go_loc( sitePtr->map_x_loc, sitePtr->map_y_loc, GO_LOC_SELECT_SITE );
				// ####### end Gilbert 31/5 ###########//
				return;
			}

			//--- if the recno loops back to the starting one ---//

			if( siteRecno == site_array.selected_recno )
				break;
		}
	}
}
//----------- End of function World::disp_next --------//


//--------- Begin of function World::disp_next_with_filter --------//
//
// Display the next object of the same type.
//
// <int> seekDir : -1 - display the previous one in the list.
// 					  1 - display the next one in the list.
//
void World::disp_next_with_filter(int seekDir)
{
	int objRecno=0;

	if( unit_array.selected_recno )
		objRecno = unit_array[unit_array.selected_recno]->base_obj_recno;

	if( firm_array.selected_recno )
		objRecno = firm_array[firm_array.selected_recno]->base_obj_recno;

	if( town_array.selected_recno )
		objRecno = town_array[town_array.selected_recno]->base_obj_recno;

	int curObjType = base_obj_array[objRecno]->obj_type;

	//-----------------------------------------//

	int objCount = base_obj_array.size();

	for( int i=objCount ; i>0 ; i-- )
	{
		if( seekDir < 0 )
		{
			objRecno--;

			if( objRecno < 1 )
				objRecno = objCount;
		}
		else
		{
			objRecno++;

			if( objRecno > objCount )
				objRecno = 1;
		}

		//-------------------------------------//

		if( base_obj_array.is_deleted(objRecno) )
			continue;

		BaseObj* baseObj = base_obj_array[objRecno];

		if( !baseObj->obj_is_visible() )
			continue;

		if( baseObj->obj_type != curObjType )
			continue;

		//--- check if the location of the unit has been explored ---//

		if( !world.get_loc(baseObj->obj_loc_x1(), baseObj->obj_loc_y1())->explored() )
			continue;

		//----- check object filter ------//

		if( map_matrix->filter_object_flag )
		{
			if( baseObj->obj_type != map_matrix->filter_object_type )
				continue;

			//------------------------------------//

			int objPara=0;

			switch( baseObj->obj_type )
			{
				case OBJ_UNIT:
					objPara = unit_res[ baseObj->cast_to_Unit()->unit_id ]->unit_class;
					break;

				case OBJ_FIRM:
					objPara = baseObj->cast_to_Firm()->firm_id;
					break;

				case OBJ_TOWN:
//					objPara = baseObj->cast_to_Town()->is_pay_tribute_to_monster(nation_array.player_recno);
					break;
			}

			if( objPara && objPara != map_matrix->filter_object_para )
				continue;
		}

		//---- check nation filter --------//

		if( map_matrix->filter_nation_flag )
		{
			if( baseObj->nation_recno != map_matrix->filter_nation_recno )
				continue;
		}

		//---- select this object -----//

		power.reset_selection();

		switch( baseObj->obj_type )
		{
			case OBJ_UNIT:
				baseObj->cast_to_Unit()->select();
				unit_array.selected_recno = baseObj->obj_extern_recno;
				break;

			case OBJ_FIRM:
				firm_array.selected_recno = baseObj->obj_extern_recno;
				break;

			case OBJ_TOWN:
				town_array.selected_recno = baseObj->obj_extern_recno;
				break;
		}

		world.go_loc( baseObj->obj_loc_x1(), baseObj->obj_loc_y1(), 1 );
		return;
	}
}
//----------- End of function World::disp_next_with_filter --------//

#ifdef DEBUG3

//--------- Begin of function World::get_loc --------//
//
Location* World::get_loc(int xLoc, int yLoc)
{
	err_when( xLoc<0 || xLoc>=MAX_WORLD_X_LOC );
	err_when( yLoc<0 || yLoc>=MAX_WORLD_Y_LOC );

	return loc_matrix + MAX_WORLD_X_LOC * yLoc + xLoc;
}
//----------- End of function World::get_loc --------//


//--------- Begin of function World::get_region_id --------//
//
BYTE World::get_region_id(int xLoc, int yLoc)
{
	err_when( xLoc<0 || xLoc>=MAX_WORLD_X_LOC );
	err_when( yLoc<0 || yLoc>=MAX_WORLD_Y_LOC );

	return loc_matrix[MAX_WORLD_X_LOC*yLoc+xLoc].region_id;
}
//----------- End of function World::get_region_id --------//

#endif

//--------- Begin of function World::is_adjacent_region --------//
//
// return true if any location adjacent to (x,y) is on a particular region
//
int World::is_adjacent_region(int x, int y, int regionId)
{
	if( y > 0 )
	{
		if( x > 0 )
		{
			if( get_region_id(x-1,y-1) == regionId )
				return 1;
		}
		if( get_region_id(x,y-1) == regionId )
			return 1;
		if( x < max_x_loc-1 )
		{
			if( get_region_id(x+1,y-1) == regionId )
				return 1;
		}
	}
	if( x > 0 )
	{
		if( get_region_id(x-1,y) == regionId )
			return 1;
	}
	if( x < max_x_loc-1 )
	{
		if( get_region_id(x+1,y) == regionId )
			return 1;
	}
	if( y < max_y_loc-1)
	{
		if( x > 0 )
		{
			if( get_region_id(x-1,y+1) == regionId )
				return 1;
		}
		if( get_region_id(x,y+1) == regionId )
			return 1;
		if( x < max_x_loc-1 )
		{
			if( get_region_id(x+1,y+1) == regionId )
				return 1;
		}
	}
	return 0;
}
//----------- End of function World::is_adjacent_region --------//


//--------- Begin of function World::get_loc_corner --------//

LocationCorners *World::get_loc_corner(int xLoc, int yLoc, LocationCorners *lc)
{
	err_when(xLoc < 0 || xLoc > max_x_loc);
	err_when(yLoc < 0 || yLoc > max_y_loc);

	int i = yLoc * max_x_loc + xLoc;
	lc->loc_ptr = loc_matrix+ i;
	i += yLoc;	// i = yLoc * (max_x_loc+1) + xLoc;
	lc->top_right = (lc->top_left = corner_matrix + i) + 1;
	i += max_x_loc+1;		// next row
	lc->bottom_right = (lc->bottom_left = corner_matrix + i) + 1;

	return lc;
}
//----------- End of function World::get_loc_corner --------//


//--------- Begin of function World::get_altitude --------//

int World::get_altitude(int xLoc, int yLoc)
{
	LocationCorners locCorner;

	return get_loc_corner( xLoc, yLoc, &locCorner)->top_left->altitude;
}
//----------- End of function World::get_altitude --------//


//--------- Begin of function World::interpolate_z --------//

int World::interpolate_z(int curX, int curY)
{
	LocationCorners locCorner;
	return get_loc_corner(curX / LOCATE_WIDTH, curY / LOCATE_HEIGHT, &locCorner)->interpolate_z(curX, curY);
}
//----------- End of function World::interpolate_z --------//


//------- Begin of function World::effective_distance ------//

int World::effective_distance(int placeId1, int placeId2)
{
	err_when( STD_TOWN_LOC_WIDTH != 6 ); // reduce town_town, firm_town, camp_town if town size is smaller in small building set

	static dist_town_town[2] = { 12, 12 }; // { 12, 9 }
	static dist_firm_town[2] = { 18, 16 }; // { 18, 15 }
	static dist_firm_firm[2] = { 18, 15 };
	static dist_camp_town[2] = { 12, 10 }; // { 12, 9 }
	static dist_lair_lair[2] = { 12, 9 };
	err_when( config.building_size <= 0 || config.building_size > 2 );	// can handle building_size 1 and 2

	#define EFFECTIVE_TOWN_TOWN_DISTANCE dist_town_town[config.building_size-1]     // The minimum distance between 2 towns
	#define EFFECTIVE_FIRM_TOWN_DISTANCE dist_firm_town[config.building_size-1]     // maximum distance between the town and the base until it's no longer considered as command base of the town
	#define EFFECTIVE_FIRM_FIRM_DISTANCE dist_firm_firm[config.building_size-1]     // maximum distance between the town and the base until it's no longer considered as command base of the town
	#define EFFECTIVE_CAMP_TOWN_DISTANCE dist_camp_town[config.building_size-1]
	#define EFFECTIVE_LAIR_LAIR_DISTANCE dist_lair_lair[config.building_size-1]

	if( placeId1==0 && placeId2==0 )
		return EFFECTIVE_TOWN_TOWN_DISTANCE;

	//---- swap placeId1 and placeId2 so that placeId1 is always firm and placeId2 is also town

	if( placeId2>0 && placeId1==0 )
	{
		int t=placeId1;
		placeId1 = placeId2;
		placeId2 = t;
	}

	//------ if one is firm and another is town ------//

	if( placeId1>0 && placeId2==0 )
	{
		if( placeId1==FIRM_CAMP || placeId1==FIRM_FORT ||
			 placeId1==FIRM_LAIR )
		{
			return EFFECTIVE_CAMP_TOWN_DISTANCE;
		}
		else
			return EFFECTIVE_FIRM_TOWN_DISTANCE;
	}

	//------- return firm to firm distance ---------//

	if( placeId1==FIRM_LAIR && placeId2==FIRM_LAIR )
		return EFFECTIVE_LAIR_LAIR_DISTANCE;
	else
		return EFFECTIVE_FIRM_FIRM_DISTANCE;

	#undef EFFECTIVE_TOWN_TOWN_DISTANCE
	#undef EFFECTIVE_FIRM_TOWN_DISTANCE
	#undef EFFECTIVE_FIRM_FIRM_DISTANCE
	#undef EFFECTIVE_CAMP_TOWN_DISTANCE
	#undef EFFECTIVE_LAIR_LAIR_DISTANCE
}
//------- End of function World::effective_distance ------//


//------- Begin of function World::net_attacker_power_in_area ------//
//
// Take into account of the mobile units around this target location
// when considering attacking it.
//
// <int> targetXLoc, targetYLoc - the target location
// <int> attackerNationRecno    - nation recno of the attacker
// <int> defenderNationRecno    - nation recno of the defender
// <int> returnIfWar	 		  	  - whether return -1 if there is war
//											 around the given area.
// <int&> hasWar					  - a var for returning whether there is war
//											 around the given area.
//											 1 - if there is war
//											 2 - if the attacker nation is involved in the war
// <int> scanRange 				  - the scanning range of the area
//
// return : >= 0 the defense rating of this location, the rating can be < 0,
//               if we have our own units there.
//
//          -1  don't attack this town because a battle is already
//              going on.
//
int World::net_attacker_power_in_area(int targetXLoc, int targetYLoc, int attackerNationRecno,
				int defenderNationRecno, int returnIfWar, int& hasWar, int scanRange)
{
	//--- the scanning distance is determined by the AI aggressiveness setting ---//

	int xLoc1 = targetXLoc - scanRange;
	int yLoc1 = targetYLoc - scanRange;
	int xLoc2 = targetXLoc + scanRange;
	int yLoc2 = targetYLoc + scanRange;

	xLoc1 = max( xLoc1, 0 );
	yLoc1 = max( yLoc1, 0 );
	xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
	yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );

	//------------------------------------------//

	float netCombatLevel=0;    // the higher the rating, the easier we can attack the target town.
	int   xLoc, yLoc;
	Unit* unitPtr;
	Location* locPtr;

	hasWar = 0;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		locPtr = world.get_loc(xLoc1, yLoc);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			BaseObj* baseObj=NULL;

			if( locPtr->unit_recno(UNIT_LAND) )
			{
				unitPtr = unit_array[ locPtr->unit_recno(UNIT_LAND) ];

				//--------------------------------------------------//
				// If there is already a battle going on in this town,
				// do not attack this town.
				//--------------------------------------------------//

				if( unitPtr->cur_action == SPRITE_ATTACK )
				{
					if( returnIfWar )
						return -1;

					if( unitPtr->nation_recno == attackerNationRecno )
						hasWar = 2;
					else
						hasWar = max(hasWar, 1);		// if hasWar is already 2, it stays as 2
				}

				//-- only count when the unit's order is moving or attacking. Assigning and other actions are not counted ---//

				if( unitPtr->cur_order.mode != UNIT_MOVE &&
					 unitPtr->cur_order.mode != UNIT_ATTACK )
				{
					continue;
				}

				baseObj = unitPtr;
			}
			else
			{
				int baseObjRecno = locPtr->base_obj_recno();

				if( !baseObjRecno )
					continue;

				baseObj = base_obj_array[baseObjRecno];

				if( !baseObj->obj_can_attack(1) )		// 1-aggressive attack only
					continue;

				//--- if the baseObj is a firm, only count obj_power() once, when the location is the center of the firm ---//

				if( locPtr->is_firm() )
				{
					Firm* firmPtr = firm_array[ locPtr->firm_recno() ];

					if( firmPtr->center_x != xLoc || firmPtr->center_y != yLoc )
						continue;
            }
			}

			if( baseObj )
			{
				//---- if this unit is an attacker -----//

				if( baseObj->nation_recno == attackerNationRecno )
				{
					netCombatLevel += baseObj->obj_power();
				}

				else if( baseObj->nation_recno == defenderNationRecno )
				{
					netCombatLevel -= baseObj->obj_power();
				}
			}
		}
	}

	return (int) netCombatLevel;
}
//-------- End of function World::net_attacker_power_in_area ------//


