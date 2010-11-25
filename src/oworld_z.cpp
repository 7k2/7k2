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

//Filename    : OWORLD_Z.CPP
//Description : Object ZoomMatrix

#include <math.h>
#include <ovga.h>
#include <omodeid.h>
#include <osys.h>
#include <ofont.h>
#include <opfind.h>
#include <omonsres.h>
#include <oraceres.h>
#include <omouse.h>
#include <opower.h>
#include <ostr.h>
#include <oseditor.h>
#include <osite.h>
#include <ofirm.h>
#include <of_mine.h>
#include <of_fact.h>
#include <of_camp.h>
#include <of_train.h>
#include <of_inn.h>
#include <of_magi.h>
#include <of_mfort.h>
#include <ospy.h>
#include <otown.h>
#include <ogame.h>
#include <ounit.h>
#include <onation.h>
#include <osprite.h>
#include <obullet.h>
#include <oplant.h>
#include <oterrain.h>
#include <owallres.h>
#include <olightn.h>
#include <omagic.h>
#include <orain.h>
#include <osnow.h>
#include <owaypnt.h>
#include <oworld.h>
#include <oworldmt.h>
#include <oweather.h>
#include <oflame.h>
#include <ogodres.h>
#include <ou_god.h>
#include <oaudio.h>
#include <ohillres.h>
#include <otornado.h>
#include <osnowg.h>
#include <osnowres.h>
#include <oexpmask.h>
#include <ocoltbl.h>
#include	<orockres.h>
#include	<orock.h>
#include <oeffect.h>
#include <colcode.h>
#include <oanline.h>
#include <ofirmdie.h>
#include <oimgres.h>
#include <oconfig.h>
#include <oblob2.h>
#include <oblob2w.h>
#include <ounit.h>
#include <ovgalock.h>
#include <ot_firm.h>
#include <ot_unit.h>

// -------- define constant ----------//

#define IMAGE_BUF_LEFT_MARGIN 64
#define IMAGE_BUF_RIGHT_MARGIN 64
#define IMAGE_BUF_TOP_MARGIN 128
#define IMAGE_BUF_BOTTOM_MARGIN 128

//--------- Define static vars -----------//

static Rain 		rain;
static YLightning lightning;
static Snow			snow;
static int init_fire = -10;									// reset on new game and load game


// to be moved to member of ZoomMatrix

// rectangle (in abs coordinate) which is updated
static int last_abs_x1, last_abs_y1, last_abs_x2, last_abs_y2;
static int last_top_row, last_left_col, last_bottom_row, last_right_col;
// rectangle which need to be update
static int update_abs_x1, update_abs_y1, update_abs_x2, update_abs_y2;
// if a location touch update_abs_x/y,
// draw unless it is also touch last_abs_x/y


//-------- Declare static functions ---------//

static int sort_display_function( const void *a, const void *b );

static void draw_cubic_plane(int x, int y, LocationCorners, UCHAR);

static int test_inside_polygon(int x, int y, POINT *vertex, int verCount);

static void scroll_buffer( short *imageBuf, int truePitch, int srcX1, int srcY1, int srcX2, int srcY2, int shiftX, int shiftY, int fillColor=-1 );

// ------- define constant and variable for scanning hidden object

#define SCAN_HIDDEN_DISTANCE 3
#define SCAN_HIDDEN_BY_TOWN_DISTANCE 2
static int hidden_by_place(int xLoc, int yLoc, int spriteWidth, int spriteHeight);


//------- Define constant for object_type --------//

enum { OBJECT_UNIT,
		 OBJECT_POINTED_UNIT,
		 OBJECT_BULLET,
		 OBJECT_FIRM,
		 OBJECT_TOWN,
		 OBJECT_PLANT,
		 OBJECT_FIRE,
		 OBJECT_WALL,
		 OBJECT_TORNADO,
		 OBJECT_HILL,
		 OBJECT_ROCK,
		 OBJECT_EFFECT,
		 OBJECT_FIRM_DIE,
		 OBJECT_SITE,
		 OBJECT_DIRT,
	  };

// ------ define constant for draw_flag -------//
enum 
{
	OBJECT_DRAW_HIDDEN = 1,
	OBJECT_DRAW_POINTED = 2,
};

enum { LAND_DISP_LAYER_MASK=1,
       LAND_TOP_DISP_LAYER_MASK=2,
		 LAND_BOTTOM_DISP_LAYER_MASK=4,
		 AIR_DISP_LAYER_MASK=8,
     };

//---------- Define struct DisplaySort ----------//

struct DisplaySort
{
	char	object_type;
	// ### begin Gilbert 17/9 #####//
	char    draw_flags;                             // bit0 set = redraw semi-transparency, bit 1 = outline
	// ### end Gilbert 17/9 #####//
	short object_recno;
	short object_y2;
	short x_loc, y_loc;
};

//------------ begin of static function draw_unit_path_on_zoom_map -----------//

// ###### begin Gilbert 28/1 #######//
static void draw_unit_path_on_zoom_map(int displayLayer)
{
	int showAttackerPath = nation_array.player_recno
		&& god_res[GOD_JAPANESE]->nation_prayer_count(nation_array.player_recno);

	if( !(config.show_unit_path & 1) && !showAttackerPath )
		return;

	// unit path in 7k2 should be from square to square, straight path
	// doesn't mean straight line

	short playerNation = nation_array.player_recno;

	Unit *unitPtr;
	int i, j;

	for(i=unit_array.size(); i>0; --i)
	{
		if(unit_array.is_deleted(i))
			continue;

		unitPtr = unit_array[i];
		if(!unitPtr->is_visible())
			continue;

		// -------- determine mobile type --------//

		if( unitPtr->mobile_type == UNIT_LAND || unitPtr->mobile_type == UNIT_SEA )
		{
			if( !(displayLayer & LAND_DISP_LAYER_MASK) )
				continue;
		}
		else if( unitPtr->mobile_type == UNIT_AIR )
		{
			if( !(displayLayer & AIR_DISP_LAYER_MASK) )
				continue;
		}
		else
		{
			err_here();
			continue;
		}

		int animSeries;

		// ----- show unit path setting -------//

		if( (config.show_unit_path & 1) && unitPtr->selected_flag
			&& (config.show_ai_info || !playerNation || unitPtr->is_nation(playerNation))
			&& unitPtr->cur_action != SPRITE_IDLE)
		{
			animSeries = 0;
		}

		// ------ show unit targeting our nation  ----------//

		else if( showAttackerPath && unitPtr->cur_order.mode == UNIT_ATTACK
			&& unitPtr->cur_action != SPRITE_ATTACK		// if already attack, not need to show path
			&& !base_obj_array.is_deleted(unitPtr->cur_order.para)
			&& base_obj_array[unitPtr->cur_order.para]->nation_recno == playerNation )
		{
			animSeries = 1;
		}

		else
		{
			continue;
		}

		//--------------- draw unit's path ----------------//

		int locFromX, locFromY, locToX, locToY;
		int lineFromX, lineFromY, lineToX, lineToY;

		//------- draw the next straight path ------//

		if(unitPtr->cur_x!=unitPtr->go_x || unitPtr->cur_y!=unitPtr->go_y)
		{
			world.zoom_matrix->get_zoom_pos(unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), lineFromX, lineFromY);
			world.zoom_matrix->get_zoom_pos(unitPtr->go_x_loc(), unitPtr->go_y_loc(), lineToX, lineToY);

			anim_line.draw_line(&vga_back, lineFromX, lineFromY, lineToX, lineToY, 1, animSeries );
		}

		//------- draw the further straight path ------//

		if( unitPtr->cur_path )
		{
			locFromX = unitPtr->go_x_loc();
			locFromY = unitPtr->go_y_loc();

			for( j=unitPtr->cur_path_result_id ; j>0 ; j-- )
			{
				PathResult* pathResult = unitPtr->cur_path + j - 1;

				locToX = pathResult->loc_x;
				locToY = pathResult->loc_y;

				world.zoom_matrix->get_zoom_pos(locFromX, locFromY, lineFromX, lineFromY);
				world.zoom_matrix->get_zoom_pos(locToX, locToY, lineToX, lineToY);

				anim_line.draw_line(&vga_back, lineFromX, lineFromY, lineToX, lineToY, 1, animSeries );

				locFromX = locToX;
				locFromY = locToY;
			}
		}
	}
}
// ###### end Gilbert 28/1 #######//
//------------ end of static function draw_unit_path_on_zoom_map -----------//

//-------- Begin of function ZoomMatrix::ZoomMatrix ----------//

ZoomMatrix::ZoomMatrix() : land_disp_sort_array(sizeof(DisplaySort),100),
									air_disp_sort_array(sizeof(DisplaySort),50),
									land_top_disp_sort_array(sizeof(DisplaySort), 40),
									land_bottom_disp_sort_array(sizeof(DisplaySort), 20)
{
	init( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2,
			ZOOM_WIDTH, ZOOM_HEIGHT,
			ZOOM_LOC_X_WIDTH, ZOOM_LOC_X_HEIGHT, ZOOM_LOC_Y_WIDTH, ZOOM_LOC_Y_HEIGHT,
			RHOMBUS_LOCATION,			// isometric view
			1 );		// 1-create a background buffer

	// resize save_image_buf

	err_when( !save_image_buf );		// save image buffer is compulsory

	int saveWidth = image_width+IMAGE_BUF_LEFT_MARGIN+IMAGE_BUF_RIGHT_MARGIN;
	int saveHeight = image_height+IMAGE_BUF_TOP_MARGIN+IMAGE_BUF_BOTTOM_MARGIN;
	long saveSize = BitmapW::size( saveWidth, saveHeight);

	save_image_buf = (short *) mem_resize( save_image_buf, saveSize );
	memset( save_image_buf, 0, saveSize );
	((BitmapW *)save_image_buf)->init( saveWidth, saveHeight );

	last_draw_state = NULL;		// allocate memory in init_var
}
//---------- End of function ZoomMatrix::ZoomMatrix ----------//


ZoomMatrix::~ZoomMatrix()
{
	if( last_draw_state )
	{
		mem_del(last_draw_state);
		last_draw_state = NULL;
	}
}

//---------- Begin of function ZoomMatrix::init_para ------------//
void ZoomMatrix::init_para()
{
	init_rain = 0;
	rain.clear();
	rain.stop_rain();
	rain_channel_id = 0;
	wind_channel_id = 0;
	fire_channel_id = 0;
	last_fire_vol = 0;
	init_lightning = 0;
	init_snow = 0;
	last_brightness = 0;
	vibration = -1;
	init_magic = 0;
	init_magic2 = 0;


	// invalidate last_abs_x/y
	last_abs_x1 = 0; 
	last_abs_x2 = -1;
	last_abs_y1 = -0x1000;
	last_abs_y2 = -0x2000;		// such that touch testing always fails
	last_top_row = -1;
	last_left_col = 0;
	last_bottom_row = -2;
	last_right_col = -1;
}
//---------- End of function ZoomMatrix::init_para ----------//


//--------- Begin of function ZoomMatrix::init_var ----------//
void ZoomMatrix::init_var()
{
	Matrix::init_var();

	center_x = calc_cur_x((image_x2-image_x1)/2, (image_y2-image_y1)/2) / LOCATE_WIDTH;
	center_y = calc_cur_y((image_x2-image_x1)/2, (image_y2-image_y1)/2) / LOCATE_HEIGHT;

	last_draw_state = (BYTE *) mem_resize(last_draw_state, max_x_loc*max_y_loc*sizeof(BYTE) );
	memset( last_draw_state, 0, max_x_loc*max_y_loc*sizeof(BYTE) );
}
//--------- End of function ZoomMatrix::init_var ----------//


//--------- Begin of function ZoomMatrix::get_zoom_pos ----------//

void ZoomMatrix::get_zoom_pos(int locX, int locY, int& xPoint, int& yPoint)
{
	int x = locX * LOCATE_WIDTH + LOCATE_WIDTH/2;
	int y = locY * LOCATE_HEIGHT + LOCATE_HEIGHT/2;
	int z = world.interpolate_z( x, y );

	xPoint = ZOOM_X1 + calc_zoom_x( x, y, z );
	yPoint = ZOOM_Y1 + calc_zoom_y( x, y, z );
}
//--------- End of function ZoomMatrix::get_zoom_pos ----------//



//---------- Begin of function ZoomMatrix::draw ------------//
//
// Draw world map
//
void ZoomMatrix::draw()
{
	int i = 0;
	int       xLoc, yLoc, dispPower;
	char*     nationColorArray = nation_array.nation_power_color_array;

	int maxXLoc = top_x_loc + disp_x_loc;        // divide by 2 for world_info
	int maxYLoc = top_y_loc + disp_y_loc;

	dispPower = (world.map_matrix->map_mode == MAP_MODE_POWER &&
					 world.map_matrix->power_mode ) ||
					power.command_id == COMMAND_BUILD_FIRM ||
					power.command_id == COMMAND_SETTLE;

	sys.yield();

	//----------------------------------------------------//

	// set update region all
	update_abs_x1 = get_base_x();
	update_abs_y1 = get_base_y();
	update_abs_x2 = update_abs_x1 + image_width - 1;
	update_abs_y2 = update_abs_y1 + image_height - 1;

	if( update_abs_x1 == last_abs_x1 && update_abs_y1 == last_abs_y1 )
	{
		// no scrolling
	}
	else if( m.is_touch( update_abs_x1, update_abs_y1, update_abs_x2, update_abs_y2,
		last_abs_x1, last_abs_y1, last_abs_x2, last_abs_y2) )
	{
		// scrolling
		scroll_buffer( ((BitmapW *)save_image_buf)->get_ptr(), ((BitmapW *)save_image_buf)->get_true_pitch(),
			0, 0, ((BitmapW *)save_image_buf)->get_width() - 1, ((BitmapW *)save_image_buf)->get_height() - 1,	// full buffer
			last_abs_x1 - update_abs_x1, last_abs_y1 - update_abs_y1, vga_back.translate_color(V_BLACK) );
	}
	else
	{
		// complete new screen
		// fill black
		// vga_back.bar(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, V_BLACK);
//		IMGbar( ((BitmapW *)save_image_buf)->get_ptr() , ((BitmapW *)save_image_buf)->get_true_pitch(),
//			IMAGE_BUF_LEFT_MARGIN, IMAGE_BUF_TOP_MARGIN, 
//			image_width+IMAGE_BUF_LEFT_MARGIN-1, image_height+IMAGE_BUF_TOP_MARGIN-1, 0 );

		// skip filling if window is complete inside map
		if( disp_rect_type == 0 && (top_x_loc < 0 || top_x_loc+disp_x_loc > max_x_loc || top_y_loc < 0 || top_y_loc+disp_y_loc > max_y_loc )	// rectangular
			|| disp_rect_type == RHOMBUS_LOCATION && (top_x_loc-disp_x_loc<0 || top_x_loc+disp_y_loc >= max_x_loc || top_y_loc-disp_y_loc<0 || top_y_loc+disp_x_loc+disp_y_loc >= max_y_loc ) )	// rhombus, this condition is prudent to prefer fill black
		{
			IMGbar( ((BitmapW *)save_image_buf)->get_ptr() , ((BitmapW *)save_image_buf)->get_true_pitch(),
				0, 0, ((BitmapW *)save_image_buf)->get_width()-1, ((BitmapW *)save_image_buf)->get_height()-1, 
				vga_back.translate_color(V_BLACK) );
		}
	}

	// ###### begin Gilbert 12/2 ########//
//	sys.yield();
	// ###### end Gilbert 12/2 ########//

#ifdef DEBUG
	int cursorXLoc;
	int cursorYLoc;
	int cursorMarked = get_detect_location(mouse.cur_x, mouse.cur_y,
		&cursorXLoc, &cursorYLoc, NULL);
#endif

	// init : outerloop
	int rowXLoc = top_x_loc;
	int rowYLoc = top_y_loc;
	int topRow = top_x_loc + top_y_loc;
	int leftCol = top_x_loc - top_y_loc;
	int rowCount = 0;
	int maxColCount = 0;
	int minZoomY1;

	do		// outer loop
	{
		// init : inner loop
		minZoomY1 = ZOOM_HEIGHT;
		xLoc = rowXLoc;
		yLoc = rowYLoc;
		int maxZoomX2 = 0;
		int colCount = 0;
		do
		{
			int zoomY1;

			if( xLoc >= 0 && xLoc < max_x_loc &&
				yLoc >= 0 && yLoc < max_y_loc)
			{
				LocationCorners locCorner;
				get_loc_corner(xLoc, yLoc, &locCorner);

				// --------- test inside -----------//
				
				int locAbsX1 = calc_abs_x( xLoc * LOCATE_WIDTH, (yLoc+1) * LOCATE_HEIGHT, locCorner.bottom_left->get_altitude() );
				int locAbsY1 = calc_abs_y( xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_left->get_altitude() )
					+ locCorner.loc_ptr->min_y;
				int locAbsX2 = calc_abs_x( (xLoc+1)*LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_right->get_altitude() ) -1;
				int locAbsY2 = locAbsY1 - locCorner.loc_ptr->min_y + locCorner.loc_ptr->max_y;

				BYTE newDrawState;
				// see also omatrix2.cpp LocationCorner::render
				// ####### begin Gilbert 17/5 #######//
				if( !config.blacken_map || (config.explore_whole_map && !config.fog_of_war) )
					newDrawState = FULL_BRIGHT_MASK_ID;
				else
					newDrawState = explored_mask.make_mask_id( xLoc, yLoc );
				// ####### end Gilbert 17/5 #######//

				BYTE *lastDrawStatePtr = last_draw_state + max_x_loc * yLoc + xLoc;

				if( m.is_touch( locAbsX1, locAbsY1, locAbsX2, locAbsY2, update_abs_x1, update_abs_y1, update_abs_x2, update_abs_y2)
					&& ( !m.is_touch( locAbsX1, locAbsY1, locAbsX2, locAbsY2, last_abs_x1, last_abs_y1, last_abs_x2, last_abs_y2)
					|| topRow+rowCount < last_top_row || topRow+rowCount > last_bottom_row 
					|| leftCol+colCount < last_left_col || leftCol+colCount > last_right_col
					|| *lastDrawStatePtr != newDrawState ) )
					// so locations are simply not scanned in the loop last time, but inside last_abs_x/y
				{
					if( (*lastDrawStatePtr = newDrawState) != FULL_DARK_MASK_ID )
					{
						// ---------- get terrain texture ------------//

						TerrainInfo *terrainInfo = terrain_res[locCorner.loc_ptr->terrain_id];
						unsigned char cellId = (yLoc & 15) << 4 | (xLoc & 15);
						char *textureBitmap = NULL;

						if( locCorner.loc_ptr->is_road() )
						{
							TerrainInfo *roadInfo = terrain_res[locCorner.loc_ptr->road_terrain_id];
							if( roadInfo->lower_type == roadInfo->higher_type )
								textureBitmap = terrain_res.get_texture(roadInfo->lower_type, cellId)->bitmap_ptr;
							else
							{
								if( terrainInfo->lower_type == terrainInfo->higher_type )
								{
									textureBitmap = terrain_res.get_texture(terrainInfo->lower_type, cellId)->bitmap_ptr;
								}
								else
								{
									textureBitmap = terrain_res.mask_texture( terrainInfo->bitmap_ptr,		// mask
											terrain_res.get_texture(terrainInfo->lower_type, cellId)->bitmap_ptr,
											terrain_res.get_texture(terrainInfo->higher_type, cellId)->bitmap_ptr);
								}

								terrainInfo = terrain_res[locCorner.loc_ptr->road_terrain_id];
								textureBitmap = terrain_res.mask_texture( roadInfo->bitmap_ptr, textureBitmap,
									terrain_res.get_texture(TERRAIN_ROAD, cellId)->bitmap_ptr );
							}
						}
						else
						{
							if( terrainInfo->lower_type == terrainInfo->higher_type )
							{
								textureBitmap = terrain_res.get_texture(terrainInfo->lower_type, cellId)->bitmap_ptr;
							}
							else
							{
								textureBitmap = terrain_res.mask_texture( terrainInfo->bitmap_ptr,		// mask
										terrain_res.get_texture(terrainInfo->lower_type, cellId)->bitmap_ptr,
										terrain_res.get_texture(terrainInfo->higher_type, cellId)->bitmap_ptr);
							}
						}

						err_when( !textureBitmap );

//#ifdef DEBUG
//						if( !cursorMarked || cursorXLoc != xLoc || cursorYLoc != yLoc )
//#endif
						if( newDrawState == FULL_BRIGHT_MASK_ID
							|| newDrawState == HALF_BRIGHT_MASK_ID )
						{
							locCorner.render((BitmapW *)save_image_buf, textureBitmap,
								calc_zoom_x(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_LEFT_MARGIN,		// do not pass z
								calc_zoom_y(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_TOP_MARGIN, newDrawState );
						}
						else
						{
							locCorner.render_fog((BitmapW *)save_image_buf, textureBitmap,
								calc_zoom_x(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_LEFT_MARGIN,		// do not pass z
								calc_zoom_y(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_TOP_MARGIN,
								explored_mask.get_remap_mask(newDrawState) );
						}
					}
					else
					{
						// black square

						locCorner.render_special( (BitmapW *)save_image_buf, 1, NULL, V_BLACK, V_BLACK, 
							calc_zoom_x(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_LEFT_MARGIN,		// do not pass z
							calc_zoom_y(xLoc*LOCATE_WIDTH, yLoc*LOCATE_HEIGHT, 0) + IMAGE_BUF_TOP_MARGIN );
					}
				}

				zoomY1 = calc_zoom_y(xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT,
					locCorner.top_left->altitude) + locCorner.loc_ptr->min_y;	// not get_altitude

#ifdef DEBUG
				// draw pixel to show grid

				if( sys.testing_session && debug2_enable_flag )
				{
					int scrnX0 = ZOOM_X1 + calc_zoom_x( xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_left->altitude );
					int scrnY0 = ZOOM_Y1 + calc_zoom_y( xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_left->altitude );
					int scrnX1 = ZOOM_X1 + calc_zoom_x( (xLoc+1) * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_right->altitude );
					int scrnY1 = ZOOM_Y1 + calc_zoom_y( (xLoc+1) * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, locCorner.top_right->altitude );
					int scrnX2 = ZOOM_X1 + calc_zoom_x( xLoc * LOCATE_WIDTH, (yLoc+1) * LOCATE_HEIGHT, locCorner.bottom_left->altitude );
					int scrnY2 = ZOOM_Y1 + calc_zoom_y( xLoc * LOCATE_WIDTH, (yLoc+1) * LOCATE_HEIGHT, locCorner.bottom_left->altitude );

					vga_back.line(scrnX0, scrnY0, scrnX1, scrnY1, V_WHITE );
					vga_back.line(scrnX0, scrnY0, scrnX2, scrnY2, V_WHITE );
#ifdef DEBUG_JULIET
					if( !locCorner.loc_ptr->walkable() )
					{
						vga_back.line(scrnX1, scrnY1, scrnX2, scrnY2, V_WHITE);
					}
#endif
				}
#endif
			}
			else
			{
				zoomY1 = calc_zoom_y(xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, 0);
			}

			// if( zoomX2 > maxZoomX2 )
			//	maxZoomX2 = zoomX2;
			if( zoomY1 < minZoomY1 )
				minZoomY1 = zoomY1;		// if any of this row is inside the screen, draw next row

			// end : inner loop
			++xLoc;
			--yLoc;
			++colCount;
		} while((maxZoomX2 += ZOOM_LOC_X_WIDTH-ZOOM_LOC_Y_WIDTH) <= ZOOM_WIDTH);

		// end : outer loop
		// ##### begin Gilbert 11/2 ######//
		if( rowCount & 15 == 0 )
			sys.yield();
		// ##### end Gilbert 11/2 ######//

		if( rowCount & 1 )
		{
			++rowXLoc;
		}
		else
		{
			++rowYLoc;
		}
		++rowCount;
		if( colCount > maxColCount )
			maxColCount = colCount;
	} while(minZoomY1 < ZOOM_HEIGHT);		//if all square outside ZOOM_Y2, stop drawing

	// ##### begin Gilbert 11/2 ######//
	sys.yield();
	// ##### end Gilbert 11/2 ######//

	// set update region all
	last_abs_x1 = get_base_x();
	last_abs_y1 = get_base_y();
	last_abs_x2 = last_abs_x1 + image_width - 1;
	last_abs_y2 = last_abs_y1 + image_height - 1;
	last_top_row = topRow;
	last_bottom_row = topRow + rowCount - 1;
	last_left_col = leftCol;
	last_right_col = leftCol + maxColCount - 1;

	// put to back buffer

	vga_back.put_bitmapW_area( image_x1 - IMAGE_BUF_LEFT_MARGIN, image_y1 - IMAGE_BUF_TOP_MARGIN,
		save_image_buf, IMAGE_BUF_LEFT_MARGIN, IMAGE_BUF_TOP_MARGIN,
		image_width+IMAGE_BUF_LEFT_MARGIN-1, image_height+IMAGE_BUF_TOP_MARGIN-1 );

	//---------------------------------------------------//

	if( save_image_buf )
	{
		if( just_drawn_flag == 0 )
		{
			sys.yield();
		}
		just_drawn_flag++;
	}
	else
		just_drawn_flag = 1;
}
//------------ End of function ZoomMatrix::draw ------------//

//------------ Begin of function ZoomMatrix::disp ------------//
//
// Display the drawn world zoom on screen
// just_drawn_flag is interpreted differently than other Matrix object
//
void ZoomMatrix::disp()
{
#ifdef DEBUG
	unsigned long startTime = m.get_time();
#endif
	if( save_image_buf )
	{
		// just_drawn_flag = -1, need redraw, don't save save_image_buf
		// just_drawn_flag = 0, redraw, now redraw again, and save save_image_buf
		// just_drawn_flag = 1, don't redraw because it have been saved to save_image_buf
		if( just_drawn_flag >= 1)		// if the map has just been drawn in draw()
		{
			// err_when( image_width%4 != 0 );
			vga_back.put_bitmapW_area( image_x1-IMAGE_BUF_LEFT_MARGIN, 
				image_y1-IMAGE_BUF_TOP_MARGIN,
				save_image_buf,
				IMAGE_BUF_LEFT_MARGIN, IMAGE_BUF_TOP_MARGIN,
				image_width+IMAGE_BUF_LEFT_MARGIN-1, image_height+IMAGE_BUF_TOP_MARGIN-1 );
		}
		else
		{
			draw();
		}
	}
	else
	{
		draw();
		just_drawn_flag=0;
	}
#ifdef DEBUG
	startTime = m.get_time() - startTime;
	{
		VgaFrontLock vgaLock;
		startTime = 0;		// dummy for setting breaking point
	}
#endif
}
//------------ End of function ZoomMatrix::disp ------------//

//---------- Begin of function ZoomMatrix::draw_white_site ------------//
//
void ZoomMatrix::draw_white_site()
{
/*

	int       i=0, x, y, xLoc, yLoc;
	Location* locPtr;

	int maxXLoc = top_x_loc + disp_x_loc;        // divide by 2 for world_info
	int maxYLoc = top_y_loc + disp_y_loc;

	//------- draw occupied locations in whie ---------//

	for( y=image_y1,yLoc=top_y_loc ; yLoc<maxYLoc ; yLoc++, y+=loc_height )
	{
		locPtr = get_loc(top_x_loc,yLoc);

		for( x=image_x1,xLoc=top_x_loc ; xLoc<maxXLoc ; xLoc++, x+=loc_width, locPtr++ )
		{
			if(locPtr->unit_recno(UNIT_LAND) || locPtr->unit_recno(UNIT_SEA) || locPtr->unit_recno(UNIT_AIR))
				vga_back.bar( x, y, x+31, y+31, V_WHITE );
		}
	}
	*/
}
//------------ End of function ZoomMatrix::draw_white_site ------------//


//---------- Begin of function ZoomMatrix::draw_frame -----------//
//
void ZoomMatrix::draw_frame()
{
#ifdef DEBUG
	unsigned long startTime = m.get_time();
	unsigned long drawObjectTime = m.get_time();
#endif
	draw_objects();

#ifdef DEBUG
	drawObjectTime = m.get_time() - drawObjectTime;
	unsigned long miscDrawTime = m.get_time();
#endif

	draw_build_marker();
	if(scenario_editor.is_enable())
	{
		scenario_editor.disp_view();
	}

	info.draw_selected();

	disp_rally_point();
	disp_text();

	if( !config.explore_whole_map && config.blacken_map )
	{
		blacken_unexplored();
	}
	draw_weather_effects();
	draw_magic_effects();

#ifdef DEBUG
	miscDrawTime = m.get_time() - miscDrawTime;
#endif

}
//----------- End of function ZoomMatrix::draw_frame ------------//

void ZoomMatrix::draw_magic_effects()
{
	magic.init(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
	init_magic = 1 +init_magic % 6;
	init_magic2 = 1 +init_magic2 % 20;
	if (magic.effect_testing_flag == 1)
		magic.draw_magic_one(&vga_back, ZOOM_X2/2, ZOOM_Y2/2, vga.pixel_format_flag, init_magic2); 
//	if (magic.effect_testing_flag == 2)
//		magic.draw_magic_two(&vga_back, ZOOM_X2/2, ZOOM_Y1+50, ZOOM_X2/2, ZOOM_Y2-150, 15, 
//								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 3)
		magic.draw_magic_three(&vga_back, ZOOM_X2/2, ZOOM_Y1+50, ZOOM_X2/2, ZOOM_Y2-100, 
								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 4)
		magic.draw_magic_four(&vga_back, ZOOM_X2/2, ZOOM_Y1+70, ZOOM_X2/2, ZOOM_Y2-70, 
								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 5)
		magic.draw_magic_five(&vga_back, ZOOM_X2/2, ZOOM_Y2/2-60, ZOOM_X2/2, ZOOM_Y2/2+100, 
								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 6)
		magic.draw_magic_six(&vga_back, ZOOM_X2/2, ZOOM_Y1+50, ZOOM_X2/2, ZOOM_Y2-100, 
								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 7)
		magic.draw_magic_seven(&vga_back, ZOOM_X2/2, ZOOM_Y1+50, ZOOM_X2/2, ZOOM_Y2-100, 
								vga.pixel_format_flag, init_magic2);
	if (magic.effect_testing_flag == 8)
		magic.draw_magic_eight(&vga_back, ZOOM_X2/2, ZOOM_Y1+50, ZOOM_X2/2, ZOOM_Y2-100, 
								vga.pixel_format_flag, init_magic2);

}
//----------- End of function ZoomMatrix::draw_magic_effects ------------//
// #### end Ban 6/30 ######//

//---------- Begin of function ZoomMatrix::draw_weather_effects -----------//
//
void ZoomMatrix::draw_weather_effects()
{
	//---------- Earthquake -----------//

	if( weather.is_quake() )
	{
		if(vibration == -1)
		{
			// start of an earthquake
			vibration = weather.quake_rate(top_x_loc+center_x, top_y_loc+center_y)*16/100;
			if( config.sound_effect_flag && config.earthquake_audio)
                        {
                                RelVolume r(config.earthquake_volume,0);
                                audio.play_long_wav( DIR_SOUND"QUAKE.WAV", DsVolume(r) );
                        }
		}
		int vPitch = vga_back.buf_pitch();
		short *destBitmap = vga_back.buf_ptr(ZOOM_X1, ZOOM_Y1);
		short *srcBitmap = vga_back.buf_ptr(ZOOM_X1, ZOOM_Y1 + vibration);
		int lineCount = ZOOM_HEIGHT - vibration;

		// shift back buffer up

		if( vibration )
		{
			if( config.earthquake_visual)
			{
				for(int lineRun = 0; lineRun < lineCount; ++lineRun)
				{
					memcpy(destBitmap, srcBitmap, ZOOM_WIDTH * sizeof(short));
					destBitmap += vPitch;
					srcBitmap  += vPitch;
				}
			}
			vibration = 0;
		}
		else
		{
			vibration = weather.quake_rate(top_x_loc+center_x, top_y_loc+center_y)*16/100;
		}
	}
	else
	{
		vibration = -1;
	}

	
	//---------- raining --------//

	short newRainScale = weather.rain_scale();

	if( newRainScale != init_rain )
	{
		// BUGHERE : did not handle, wind change (direction/speed) during a rain
		if( newRainScale)
		{
			rain.start_rain(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, newRainScale,
				weather.wind_speed()*sin(weather.wind_direct_rad())/100.0);

			// turn on rain noise

			int relVolume = config.rain_volume + newRainScale;
			if( relVolume > 100)
				relVolume = 100;

			if( rain_channel_id == 0)	// from no rain to rain
			{
				if( config.sound_effect_flag && config.rain_audio)
                                {
				//	rain_channel_id = audio.play_loop_wav(DIR_SOUND"RAIN.WAV",11008*2, RelVolume(relVolume,0));
                                        RelVolume r(relVolume,0);
                                        rain_channel_id = audio.play_loop_wav(DIR_SOUND"RAIN.WAV",0, DsVolume(r));
                                }
			}
			else
			{
				// changing rain
				if( config.sound_effect_flag && config.rain_audio)
                                {
                                        RelVolume r(relVolume,0);
                                        audio.volume_loop_wav(rain_channel_id, DsVolume(r));
                                }
				else
				{
					// can't stop rain audio immediately
					// but at least stop it when rain change
					audio.stop_loop_wav(rain_channel_id);
					rain_channel_id = 0;
				}

			}

		}
		else
		{
			// rain stop, rain sound fade out
			rain.stop_rain();
			if( rain_channel_id )
			{
				audio.fade_out_loop_wav(rain_channel_id, 10);
			}
		}
		init_rain = newRainScale;
	}
	else
	{
		// rain stopped, check rain sound fade out
		if( newRainScale == 0 && rain_channel_id )
		{
			DsVolume dsVolume(audio.get_loop_wav_volume(rain_channel_id));
			AbsVolume absVolume(dsVolume);
			if( absVolume.abs_vol < 10 )
			{
				audio.stop_loop_wav(rain_channel_id);
				rain_channel_id = 0;
			}
		}
	}

	// ##### begin Gilbert 6/9 #######//
	if( config.frame_speed > 0)
	{
		rain.new_drops();
		if( config.rain_visual)
		{
			rain.draw_step(&vga_back);
		}
	}
	// ##### end Gilbert 6/9 #######//


	//---------- Lightning -----------//
	// world.lightning_signal == 0 (no lightning)
	// world.lightning_signal == 110, 109 (ready lightning)
	//                108, 107, 106, 105 (flashing), Sound effect start on 6
	//                104, 103, 102, 101 (rest, decaying lighting effect )
	//                100					 (decrease randomly)
	//                99 - 1             (rest states)
	// see world.process

	unsigned long mRandom = m.get_random_seed();
	if( world.lightning_signal >= 105 && world.lightning_signal <= 108)
	{
		if( !init_lightning )
		{
			Lightning::set_clip( ZOOM_X1+4, ZOOM_Y1-4, ZOOM_X2-4, ZOOM_Y2-4);

			// play sound
			if( world.lightning_signal == 108 && config.sound_effect_flag && config.lightning_audio)
                        {
                          RelVolume r(config.lightning_volume,0);
                          audio.play_long_wav(DIR_SOUND"THUNDER.WAV", DsVolume(r));
                        }

			// find the starting and ending point of the lightning
			lightning_x1 = Lightning::bound_x1 + 20 + short(mRandom % (Lightning::bound_x2-Lightning::bound_x1 - 40));
			lightning_y1 = Lightning::bound_y1 - 50;
			lightning_x2 = Lightning::bound_x1 + 10 + short(mRandom % (Lightning::bound_x2-Lightning::bound_x1 - 20));
			lightning_y2 = (Lightning::bound_y1+Lightning::bound_y2) / 2 +
				short(mRandom % ( (Lightning::bound_y2-Lightning::bound_y1) / 2));
			init_lightning = 1;
		}

		if (world.lightning_signal <= 110 || world.lightning_signal >= 101)
			vga_back.bar_alpha( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1, V_BLACK );

		lightning.init( lightning_x1, lightning_y1, lightning_x2, lightning_y2, 8);
		if( config.lightning_visual)
			lightning.draw_section(&vga_back, (109-world.lightning_signal)/4.0);
	}
	else
	{
		init_lightning = 0;
	}

	//------------ snowing ------------//
	short snowScale = weather.snow_scale();
	if( snowScale > 0 && init_snow == 0)
	{
		long backupSeed = m.get_random_seed();

		// start of snow
		snow.set_bound(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
		snow.init(weather.wind_speed()*sin(weather.wind_direct_rad())/200.0,
			snowScale+2);
		if( config.sound_effect_flag && config.snow_audio )
		{
			// audio.play_wav("SNOW", config.snow_volume);
		}
	}
	// ###### begin Gilbert 6/9 #######//
	if( snowScale > 0 && config.snow_visual && config.frame_speed > 0)
		snow.draw_step(&vga_back);
	// ###### end Gilbert 6/9 #######//
	init_snow = snowScale;

	//------------ brightness, effect of lightning and cloud -------//
	short newBrightness;
	short maxBrightness = config.lightning_brightness;
	if( config.lightning_visual && init_lightning > 107 )
	{
		newBrightness = -maxBrightness;
	}
	else if( config.lightning_visual && init_lightning >= 104 && init_lightning <= 107)
	{
		newBrightness = maxBrightness;
	}
	else if( config.lightning_visual && init_lightning >= 101 && init_lightning <= 103)
	{
		newBrightness = (init_lightning-100) * maxBrightness / 4;
	}
	else
	{
		newBrightness = -weather.cloud() * config.cloud_darkness ;
	}
	if( newBrightness != last_brightness )
	{
		vga.adjust_brightness(newBrightness);
		last_brightness = newBrightness;
	}
	
	// ------------- wind sound ----------//
	int windSpeed = weather.wind_speed();
	if( windSpeed >= 20)
	{
		int relVolume = config.wind_volume + 5 + windSpeed/4;
		if( relVolume > 100)
			relVolume = 100;
		if( wind_channel_id == 0)
		{
			if( config.sound_effect_flag && config.wind_audio )
                        {
				// ###### begin Gilbert 6/8 #######//
				// wind_channel_id = audio.play_loop_wav(DIR_SOUND"WIND.WAV",0, relVolume);
				// wind_channel_id = audio.play_loop_wav(DIR_SOUND"WIND.WAV",25088*2, RelVolume(relVolume,0));  // 25088 samples, 8-bit stereo, so *2
                                RelVolume r(relVolume,0);
                                wind_channel_id = audio.play_loop_wav(DIR_SOUND"WIND.WAV",0, DsVolume(r));
				// ###### end Gilbert 6/8 #######//
                        }
		}
		else
		{
			if( config.wind_audio)
			{
				RelVolume r(relVolume,0);
                                audio.volume_loop_wav(wind_channel_id, DsVolume(r));
			}
			else
			{
				audio.stop_loop_wav(wind_channel_id);
				wind_channel_id = 0;
			}
		}
	}
	else
	{
		if( wind_channel_id )
		{
			if( !audio.is_loop_wav_fading(wind_channel_id) )
			{
				audio.fade_out_loop_wav(wind_channel_id, 5);
			}
			else
			{
				DsVolume dsVolume = audio.get_loop_wav_volume(wind_channel_id);
				AbsVolume absVolume(dsVolume);
				if( absVolume.abs_vol < 5 )
				{
					audio.stop_loop_wav(wind_channel_id);
					wind_channel_id =0 ;

				}
			}
		}
	}

}
//----------- End of function ZoomMatrix::draw_weather_effects ------------//


//---------- Begin of function ZoomMatrix::draw_build_marker -----------//
//
void ZoomMatrix::draw_build_marker()
{
	if( !(mouse.cur_x >= ZOOM_X1 && mouse.cur_x <= ZOOM_X2 &&		// if the mouse is inside the zoom area
			mouse.cur_y >= ZOOM_Y1 && mouse.cur_y <= ZOOM_Y2) )
	{
		return;
	}

	if( power.win_opened )
		return;

	//------- COMMAND_GOD_CAST_POWER --------//

//	else if( power.command_id == COMMAND_GOD_CAST_POWER )
//	{
//		// draw_god_cast_range();
//		return;
//	}

	//----------------------------------------------//

	int xLoc, yLoc;
	int locWidth, locHeight;
	int validAction = 0;
	int radiusFlag = 0;
	int outsideWidth = INTER_PLACE_SPACE;

	Location* locPtr = NULL;
	if( get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
	{
		locPtr = get_loc(xLoc, yLoc);
	}
	else
		return;

	//------- if it's in firm building mode now ----//

	if( power.command_id == COMMAND_BUILD_FIRM )
	{
		FirmInfo* firmInfo = firm_res[power.command_para];

		locWidth  = firmInfo->loc_width;
		locHeight = firmInfo->loc_height;

		// ######### begin Gilbert 4/3 #######//
		// ------ snap to raw -------//
		if( locPtr->has_site() )
		{
			Site *sitePtr = site_array[locPtr->site_recno()];
			if( sitePtr->site_type == SITE_RAW )
			{
				xLoc = sitePtr->map_x_loc;
				yLoc = sitePtr->map_y_loc;
				locPtr = get_loc( xLoc, yLoc );
			}
		}
		// ######### end Gilbert 4/3 #######//

		validAction  = world.can_build_firm( xLoc, yLoc, power.command_para, unit_array.selected_recno );
	}

	else if( power.command_id == COMMAND_TRANSFORM_FORTRESS )
	{
		FirmInfo* firmInfo = firm_res[power.command_para];

		locWidth = firmInfo->loc_width;
		locHeight = firmInfo->loc_height;

		validAction = (~nation_array)->cash >= firmInfo->setup_cost
			&& (~nation_array)->live_points >= firmInfo->setup_live_points_cost;
		// ###### begin Gilbert 22/4 ########//
		if( validAction )
		{
			validAction = FirmMonsterFortress::can_build_fortress( unit_array.selected_recno, xLoc, yLoc, NULL );
		}
		// ###### end Gilbert 22/4 ########//
	}

	//------- if it's in settling mode now ----//

	else if( power.command_id == COMMAND_SETTLE && unit_array.selected_recno )
	{
		// assign to an existing town

		Unit* selectedUnit = unit_array[ unit_array.selected_recno ];

		if( locPtr->is_town() && town_array[locPtr->town_recno()]->nation_recno == selectedUnit->nation_recno )
			return;		// don't draw the settling mask.

		locWidth  = STD_TOWN_LOC_WIDTH;
		locHeight = STD_TOWN_LOC_HEIGHT;

		validAction  = world.can_build_town( xLoc, yLoc, unit_array.selected_recno );
	}
/*
	//------- COMMAND_BUILD_WALL --------//

	else if( power.command_id == COMMAND_BUILD_WALL )
	{
		// see also World::build_wall_tile
		locWidth  = 1;
		locHeight = 1;
		Location *locPtr;

		validAction = world.can_build_wall(xLoc, yLoc, nation_array.player_recno)
			|| ( locPtr->is_wall() && locPtr->is_wall_destructing() &&
			world.can_destruct_wall(xLoc, yLoc, nation_array.player_recno) );
	}

	//------- COMMAND_DESTRUCT_WALL --------//

	else if( power.command_id == COMMAND_DESTRUCT_WALL )
	{
		locWidth  = 1;
		locHeight = 1;
		// see also World::destruct_wall_tile
		validAction = world.can_destruct_wall(xLoc, yLoc, nation_array.player_recno)
			&& locPtr->is_wall_creating() ;
	}
*/
	else if( power.command_id == COMMAND_GOD_CAST_POWER )
	{
		UnitGod* unitPtr = unit_array[power.command_unit_recno]->cast_to_UnitGod();
		if( !unitPtr )
			return;

		GodInfo*	godInfo = god_res[ unitPtr->god_id ];

		radiusFlag = 1;
		locWidth = locHeight = godInfo->cast_power_range;
		validAction = 1;
	}
	// ###### begin Gilbert 5/3 ########//
	else if( power.command_id == COMMAND_USE_ITEM_ON_UNIT )
	{
		if( (validAction = locPtr->has_any_unit()) )
		{
			int targetMobileType = locPtr->has_any_unit();
			Unit *unitPtr = unit_array[locPtr->get_any_unit(targetMobileType)];
			// snap to unit
			xLoc = unitPtr->next_x_loc();
			yLoc = unitPtr->next_y_loc();
			locWidth = unitPtr->loc_width;
			locHeight = unitPtr->loc_height;
			locPtr = get_loc( xLoc, yLoc );
		}
		else
		{
			locWidth = locHeight = 1;
		}
		outsideWidth = 0;
	}
	else if( power.command_id == COMMAND_USE_ITEM_ON_TOWN )
	{
		if( (validAction = locPtr->is_town()) )
		{
			Town *townPtr = town_array[locPtr->town_recno()];
			// snap to town
			xLoc = townPtr->loc_x1;
			yLoc = townPtr->loc_y1;
			locWidth = townPtr->loc_width();
			locHeight = townPtr->loc_height();
			locPtr = get_loc( xLoc, yLoc );
		}
		else
		{
			locWidth = locHeight = 1;
		}
		outsideWidth = 0;
	}
	else if( power.command_id == COMMAND_USE_ITEM_ON_SPACE )
	{
		locWidth = locHeight = 1;
		validAction = 1;
		outsideWidth = 0;
	}
	// ###### end Gilbert 5/3 ########//
	else if( power.command_id == COMMAND_FIRM_MAGIC )
	{
		FirmMagic *firmMagic = firm_array[power.command_firm_recno]->cast_to_FirmMagic();
		if( !firmMagic )
			return;

		MagicInfo* magicInfo = FirmMagic::magic_info_array + power.command_para - 1;
		locWidth = locHeight = magicInfo->cast_radius + 1;
		radiusFlag = 1;
		validAction = 1;

		if( power.command_para == MAGIC_DESTROY_BUILDING )	// magic id
		{
			validAction = 0;

			if( locPtr->is_firm() )
			{
				Firm *firmPtr = firm_array[locPtr->firm_recno()];
				validAction = nation_array.should_attack(nation_array.player_recno, firmPtr->nation_recno);
				xLoc = firmPtr->loc_x1;
				yLoc = firmPtr->loc_y1;
				locWidth = firmPtr->loc_width();
				locHeight = firmPtr->loc_height();
				radiusFlag = 0;
				outsideWidth = 0;
			}
			else if( locPtr->is_town() )
			{
				Town *townPtr = town_array[locPtr->town_recno()];
				validAction = nation_array.should_attack(nation_array.player_recno, townPtr->nation_recno);
				xLoc = townPtr->loc_x1;
				yLoc = townPtr->loc_y1;
				locWidth = townPtr->loc_width();
				locHeight = townPtr->loc_height();
				radiusFlag = 0;
				outsideWidth = 0;
			}
		}
	}
	else if( power.command_id == COMMAND_MAGIC_TELEPORT_PRE )
	{
		locWidth = locHeight = 1;
		radiusFlag = 1;
		validAction = 1;
		validAction = 0;

		int targetUnitRecno;
		int mobileType;
		if( (targetUnitRecno = locPtr->get_any_unit(mobileType)) )
		{
			Unit *unitPtr = unit_array[targetUnitRecno];

			validAction = unitPtr->is_own();
			xLoc = unitPtr->next_x_loc();
			yLoc = unitPtr->next_y_loc();
			locWidth = unitPtr->loc_width;
			locHeight = unitPtr->loc_height;
			radiusFlag = 0;
			outsideWidth = 0;
		}
	}
	else if( power.command_id == COMMAND_MAGIC_TELEPORT_POST )
	{
		radiusFlag = 0;
		outsideWidth = 0;
		if( !unit_array.is_deleted(FirmMagic::teleport_unit_recno) )
		{
			Unit *unitPtr = unit_array[FirmMagic::teleport_unit_recno];
			locWidth = unitPtr->loc_width;
			locHeight = unitPtr->loc_height;
			validAction = world.check_unit_space( xLoc, yLoc,
				xLoc+locWidth-1, yLoc+locHeight-1, unitPtr->mobile_type);
		}
		else
		{
			validAction = 0;
			locWidth = locHeight = 1;
		}
	}
	else
		return;

	//---------- draw an highlight area -----------//

	// ##### begin Gilbert 3/11 #####//
	int innerX1 = xLoc;
	int innerY1 = yLoc;
	int innerX2 = xLoc + locWidth-1;
	int innerY2 = yLoc + locHeight-1;

	int locX1 = innerX1 - outsideWidth;
	int locY1 = innerY1 - outsideWidth;
	int locX2 = innerX2 + outsideWidth;
	int locY2 = innerY2 + outsideWidth;

	if( radiusFlag )
	{
		locX1 = innerX1 = xLoc - locWidth + 1;
		locY1 = innerY1 = yLoc - locHeight + 1;
		locX2 = innerX2 = xLoc + locWidth - 1;
		locY2 = innerY2 = yLoc + locHeight - 1;
	}

	if( locX1 < 0 )
		locX1 = 0;
	if( locY1 < 0 )
		locY1 = 0;
	if( locX2 >= max_x_loc )
		locX2 = max_x_loc - 1;
	if( locY2 >= max_y_loc )
		locY2 = max_y_loc - 1;
	// ##### end Gilbert 3/11 #####//

	int pixelColor;

	if( validAction )
		pixelColor = anim_line.get_series_color_array(-1)[2];
	else
		// ##### begin Gilbert 10/9 ######//
		pixelColor = V_RED;
		// ##### end Gilbert 10/9 ######//

	// ###### begin Gilbert 3/11 #####//
	// vga_back.pixelize( x1, y1, min(x2,ZOOM_X2), min(y2,ZOOM_Y2), pixelColor );
	for( yLoc = locY1; yLoc <= locY2; ++yLoc )
	{
		for( xLoc = locX1; xLoc <= locX2; ++xLoc )
		{
			static Blob2DW maskBitmap;
			LocationCorners lc;
			get_loc_corner(xLoc, yLoc, &lc);
			maskBitmap.clear();

			if( xLoc >= innerX1 && xLoc <= innerX2
				&& yLoc >= innerY1 && yLoc <= innerY2 )
			{
				// area of the build area
				lc.render_special(&maskBitmap, 1, NULL, pixelColor, TRANSPARENT_CODE);
			}
			else
			{
				// two squares outside the build area
				lc.render_special(&maskBitmap, 1, NULL, VGA_GRAY+2, TRANSPARENT_CODE);
			}

			put_bitmapW_offset(xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, lc.top_left->altitude,
				maskBitmap.bitmap_ptr(), maskBitmap.left_edge, maskBitmap.top_edge, 0, 0);
		}
	}
	// ###### end Gilbert 3/11 #####//

	//------- draw lines connected to towns and firms ---------//

	if( validAction )
	{
		if( power.command_id==COMMAND_BUILD_FIRM )
		{
			world.draw_link_line( power.command_para, 0, locX1, locY1, locX2, locY2);
		}
		else if( power.command_id==COMMAND_SETTLE )
		{
			world.draw_link_line( 0, 0, locX1, locY1, locX2, locY2);
		}
	}
}
//----------- End of function ZoomMatrix::draw_build_marker ------------//


//---------- Begin of function ZoomMatrix::draw_god_cast_range -----------//
//
void ZoomMatrix::draw_god_cast_range()
{
	#define GOD_CAST_RANGE_COLOR 	V_WHITE

	/*
	int   	 xLoc, yLoc, centerY, t;
	int		 x1, y1, x2, y2;
	Location* locPtr;

	Unit* 	unitPtr = unit_array[power.command_unit_recno];
	GodInfo*	godInfo = god_res[ ((UnitGod*)unitPtr)->god_id ];

	xLoc = (mouse.cur_x-ZOOM_X1)/ZOOM_LOC_WIDTH;
	yLoc = (mouse.cur_y-ZOOM_Y1)/ZOOM_LOC_HEIGHT;

	int xLoc1 = xLoc - godInfo->cast_power_range + 1;
	int yLoc1 = yLoc - godInfo->cast_power_range + 1;
	int xLoc2 = xLoc + godInfo->cast_power_range - 1;
	int yLoc2 = yLoc + godInfo->cast_power_range - 1;

	centerY = (yLoc1+yLoc2) / 2;

	//----- pixelize the area within which the power can casted ----//

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc#+ )
	{
		t=abs(yLoc-centerY)/2;

		for( xLoc=xLoc1+t ; xLoc<=xLoc2-t ; xLoc++, locPtr++ )
		{
			if( xLoc>=0 && xLoc<MAX_WORLD_X_LOC &&
				 yLoc>=0 && yLoc<MAX_WORLD_Y_LOC )
			{
				x1 = ZOOM_X1 + xLoc * ZOOM_LOC_WIDTH;
				y1 = ZOOM_Y1 + yLoc * ZOOM_LOC_HEIGHT;
				x2 = ZOOM_X1 + (xLoc+1) * ZOOM_LOC_WIDTH -1;
				y2 = ZOOM_Y1 + (yLoc+1) * ZOOM_LOC_HEIGHT-1;

				vga_back.pixelize( x1, y1, min(x2,ZOOM_X2), min(y2,ZOOM_Y2), GOD_CAST_RANGE_COLOR );
			}
		}
	}
	*/
}
//----------- End of function ZoomMatrix::draw_god_cast_range ------------//

//---------- Begin of function ZoomMatrix::disp_text -----------//
//
// Function for displaying town names and spy indicator.
//
void ZoomMatrix::disp_text()
{
	char		  pMobileType;           // pointing mobileType
	Location*  pLoc = power.test_detect(mouse.cur_x, mouse.cur_y, &pMobileType);

	//------- towns -------//

	int x, y, z, spyCount;
	String str;

	Town* townPtr;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		//-- if config.disp_extend_info==1, only show extended info when the mouse is over the object ---//

		int dispExtendInfo=0;

		if( config.disp_extend_info==1 )
		{
			if( pLoc && pLoc->is_town() && pLoc->town_recno()==i )
				dispExtendInfo=1;
		}
		else if( config.disp_extend_info==2 )
			dispExtendInfo=1;

		//-------------------------------------------//

		townPtr = town_array[i];

//		if( !world.get_loc(townPtr->center_x, townPtr->center_y)->explored() )
//			continue;
		if( townPtr->is_stealth() )
			continue;

		x = (townPtr->loc_x1+townPtr->loc_x2+1) * LOCATE_WIDTH /2;
		y = (townPtr->loc_y1+townPtr->loc_y2+1) * LOCATE_WIDTH /2;
		z = townPtr->altitude + 20;

		if( dispExtendInfo )
			z+=30;

		int zoomX = calc_zoom_x( x, y, z );
		int zoomY = calc_zoom_y( x, y, z );

		if( zoomX < -50 || zoomX > image_width+50 || zoomY < 0 || zoomY > image_height+36 )
			continue;

		spyCount = townPtr->player_spy_count();

		put_center_text( x, y, z, townPtr->town_name(), 1, &font_zoom );
		z-=16;

		if( dispExtendInfo )
		{
//			str = race_res[townPtr->race_id]->adjective;
//			if( townPtr->is_pay_tribute_to_monster() )
//				str += " Slave";
//			str += " Town";
//			put_center_text( x, y, z, str );

			if( townPtr->is_pay_tribute_to_monster() )
				put_center_text( x, y, z, text_firm.str_race_slave_town(race_res[townPtr->race_id]->adjective) );
			else
				put_center_text( x, y, z, text_firm.str_race_town(race_res[townPtr->race_id]->adjective) );

			z-=16;

			if( townPtr->population )
			{
				if( townPtr->nation_recno )
				{
					str  = text_unit.str_loyalty(); //"Loyalty: ";
					str += ": ";
					str += (int) townPtr->loyalty;
					str += " -> ";
					str += (int) townPtr->target_loyalty;

					put_center_text( x, y, z, str );
					z-=16;

				}
				else if( nation_array.player_recno )
				{
					str  = text_firm.str_resistance(); //"Resistance: ";
					str += ": ";
					str += (int) townPtr->resistance(nation_array.player_recno);

					if( townPtr->target_resistance(nation_array.player_recno) != -1 &&
						townPtr->target_resistance(nation_array.player_recno) <=
						townPtr->resistance(nation_array.player_recno) )
							// -1 means no change
					{
						str += " -> ";
						str += (int) townPtr->target_resistance(nation_array.player_recno);
					}

					put_center_text( x, y, z, str );
					z-=16;
				}
			}

			if( townPtr->is_own() || townPtr->nation_recno == 0)
			{
//				if( townPtr->jobless_population <= 1 )
//					str = "Peasant: ";
//				else
//					str = "Peasants: ";
//				str += townPtr->jobless_population;

//				if( townPtr->worker_population() <= 1 )
//					str += " Worker: ";
//				else
//					str += " Workers: ";
//				str += townPtr->worker_population();

				put_center_text( x, y, z, text_firm.str_town_info_pop(
					townPtr->jobless_population, townPtr->worker_population()) );
				z-=16;
			}
		}

		if( spyCount )
		{
			// put_center_text( x, y, z, spyCount==1 ? "(Your Spy)" : "(Your Spies)" );
			put_center_text( x, y, z, text_firm.str_spy_inside(spyCount) );

			z -= 16;
		}
	}

	//------ firms -------//

	Firm* firmPtr;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( firmPtr->is_stealth() )
			continue;

		//-- if config.disp_extend_info==1, only show extended info when the mouse is over the object ---//

		int dispExtendInfo=0;

		if( firmPtr->should_show_info() )
		{
			if( config.disp_extend_info==1 )
			{
				if( pLoc && pLoc->is_firm() && pLoc->firm_recno()==i )
					dispExtendInfo=1;
			}
			else if( config.disp_extend_info==2 )
				dispExtendInfo=1;
		}

		//------------------------------------------//

		if (dispExtendInfo && !world.map_matrix->zoom_map_building_disappear)
		{
			short curX1 = firmPtr->loc_x1 * LOCATE_WIDTH;
			short curY1 = firmPtr->loc_y1 * LOCATE_HEIGHT;
			short curX2 = (firmPtr->loc_x2 + 1) * LOCATE_WIDTH;
			short curY2 = (firmPtr->loc_y2 + 1) * LOCATE_HEIGHT;
			short curZ = firmPtr->altitude;
//			if ( firmPtr->firm_id == FIRM_FORT )
//				str = firmPtr->firm_name(0);
//			else
//				str = firmPtr->firm_name();
#if(defined(CHINESE))
			put_center_text( (curX1+curX2)/2 +16, (curY1+curY2)/2 +16, curZ, firmPtr->firm_name() );
#else
			put_center_text( (curX1+curX2)/2, (curY1+curY2)/2, curZ, firmPtr->firm_name() );
#endif
		}

		spyCount = 0;
		short spyRecno;

		x = (firmPtr->loc_x1+firmPtr->loc_x2+1) * LOCATE_WIDTH /2;
		y = (firmPtr->loc_y1+firmPtr->loc_y2+1) * LOCATE_HEIGHT /2;
		z = firmPtr->altitude+40;

		int zoomX = calc_zoom_x( x, y, z );
		int zoomY = calc_zoom_y( x, y, z );
		if( zoomX < -50 || zoomX > image_width+50 || zoomY < 0 || zoomY > image_height+20 )
			continue;

		//----------- camp ---------------//

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

			if( firmCamp->overseer_recno
				&& (spyRecno = unit_array[firmCamp->overseer_recno]->spy_recno)
				&& spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
			{
				++spyCount;
			}

			for( int i = 0; i < firmCamp->soldier_count; ++i )
			{
				if( (spyRecno = firmCamp->soldier_array[i].spy_recno)
					&& spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
				{
					++spyCount;
					break;
				}
			}

			//------ display extended info -------//

			if( dispExtendInfo )
			{
				// ###### begin Gilbert 10/2 #####//
				if( firmCamp->under_construction )
				{
					// nothing to display
				}
				else if( firmCamp->overseer_recno )
				// ###### end Gilbert 10/2 #####//
				{
					Unit* unitPtr = unit_array[firmCamp->overseer_recno];

					// ##### patch begin Gilbert 26/10 ######//
//					put_center_text( x, y, z, 
//						text_unit.str_race_title( unitPtr->race_id,
//						(unitPtr->is_human() ? race_res[unitPtr->race_id]->adjective : monster_res[unitPtr->monster_id()]->name),
//						unitPtr->rank_id ) );
					put_center_text( x, y, z, 
						text_unit.str_race_title( unitPtr->race_id,
						(unitPtr->is_human() ? race_res[unitPtr->race_id]->adjective : monster_res[unitPtr->monster_id()]->adjective),
						unitPtr->rank_id ) );
					// ##### patch end Gilbert 26/10 ######//
					z-=16;

					str  = text_unit.str_leadership(); //"Leadership: ";
					str += ": ";
					str += unitPtr->skill_level();

					put_center_text( x, y, z, str );
					z-=16;
				}
				else
				{
					put_center_text( x, y, z, text_firm.str_no_commander() ); //"No Commander" );
					z-=16;
				}
	/*
				str  = "Soldiers: ";
				str += firmCamp->soldier_count;

				if( firmCamp->cast_to_FirmFort() &&
					 firmCamp->cast_to_FirmFort()->current_archer_count > 0 )
				{
					str += " Archers: ";
					str += firmCamp->cast_to_FirmFort()->current_archer_count;
				}

				put_center_text( x, y, z, str );
				z-=16;*/
			}
		}

		//----------- mine ---------------//

		if( firmPtr->cast_to_FirmMine() )
		{
			FirmMine *firmMine = firmPtr->cast_to_FirmMine();
			//------ display extended info -------//

			if( dispExtendInfo )
			{
				if( firmMine->under_construction )
				{
					// nothing to display
				}
				else
				{
					// str = "Mining Raw ";
					// str += raw_res[firmMine->raw_id]->name;
					// str = "No Raw Under";
					put_center_text( x, y, z, text_firm.str_mine_info(firmMine->raw_id) );
					z-=16;
				}
			}
		}

		//----------- factory ---------------//

		if( firmPtr->cast_to_FirmFactory() )
		{
			FirmFactory *firmFactory = firmPtr->cast_to_FirmFactory();
			//------ display extended info -------//

			if( dispExtendInfo )
			{
				if( firmFactory->under_construction )
				{
					// nothing to display
				}
				else
				{
//					if (firmFactory->product_raw_id)
//					{
//						str = "Producing ";
//						str += raw_res[firmFactory->product_raw_id]->name;
//						str += " Ware";
//					}
//					else
//						str = "No Ware Under Production";			
					put_center_text( x, y, z, text_firm.str_factory_info(firmFactory->product_raw_id) );
					z-=16;
				}
			}
		}

		//-------- unit training firms ---------//

		if( firmPtr->cast_to_FirmTrain() )
		{
			FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();
			for( int i = 0; i < firmTrain->trainee_count; ++i )
			{
				if( (spyRecno = firmTrain->trainee_array[i].spy_recno)
					&& spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
				{
					++spyCount;
					break;
				}
			}
		}

		//------------- Inn -----------------//

		if( firmPtr->cast_to_FirmInn() )
		{
			FirmInn *firmInn = firmPtr->cast_to_FirmInn();
			for( int i = 0; i < firmInn->inn_unit_count; ++i )
			{
				if( (spyRecno = firmInn->inn_unit_array[i].spy_recno )
					&& spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
				{
					++spyCount;
					break;
				}
			}
		}

		//-------- display spy indicator ---------//

		if( spyCount > 0  )
		{
			// put_center_text( x, y, z, spyCount==1 ? "(Your Spy)" : "(Your Spies)" );
			put_center_text( x, y, z, text_firm.str_spy_inside(spyCount) );
		}
	}
}
//----------- End of function ZoomMatrix::disp_text ------------//

//---------- Begin of function ZoomMatrix::put_center_text -----------//
//
// <int>   x, y - center of the absolute position where the text should be put.
// <char*> str  - the display string.
// <char> black - using a transparent black blackground or not
//
void ZoomMatrix::put_center_text(int x, int y, int z, char* str, char black, Font* font_ptr)
{
	str = translate.process(str);

	static Blob2DW tempBuffer;

	if( !str[0] )
		return;

	short w = font_ptr->text_width(str);
	short h = font_ptr->max_font_height;

	if( w <= 0 )
		return;
	
	tempBuffer.clear();
	tempBuffer.resize(-w/2, -h/2, w, h);
	font_ptr->put_to_bufferW(tempBuffer.ptr->get_ptr(), tempBuffer.buf_true_pitch(), 0, 0, str);
//	font_news.put_to_bufferW(tempBuffer.ptr->get_ptr(), tempBuffer.buf_true_pitch(), 0, 0, str);

	// darken before the word
	int scrnX1 = image_x1 + calc_zoom_x( x, y, z) + tempBuffer.left_edge - 4;
	int scrnY1 = image_y1 + calc_zoom_y( x, y, z) + tempBuffer.top_edge - 2;
	int scrnX2 = scrnX1 + w - 1 + 8;
	int scrnY2 = scrnY1 + h - 1 + 4;
	if( scrnX1 <= image_x2 && scrnX2 >= image_x1 && scrnY1 <= image_y2 && scrnY2 >= image_y1 )
	{
		// clip
		scrnX1 = max( image_x1, scrnX1 );
		scrnY1 = max( image_y1, scrnY1 );
		scrnX2 = min( image_x2, scrnX2 );
		scrnY2 = min( image_y2, scrnY2 );

		if (black)
			vga_back.bar_alpha( scrnX1, scrnY1, scrnX2, scrnY2, 1, V_BLACK );

		put_bitmapW_offset(x, y, z, tempBuffer.bitmap_ptr(),
			tempBuffer.left_edge, tempBuffer.top_edge, 0, 0);
	}
}
//----------- End of function ZoomMatrix::put_center_text ------------//

//---------- Begin of function ZoomMatrix::blacken_unexplored -----------//
//
void ZoomMatrix::blacken_unexplored()
{
	/*
	//----------- black out unexplored area -------------//

	int leftLoc = top_x_loc;
	int topLoc = top_y_loc;
	int rightLoc = leftLoc + disp_x_loc - 1;
	int bottomLoc = topLoc + disp_y_loc % 1;
	int scrnY, scrnX;		// screen coordinate
	int x, y;				// x,y Location
	Location *thisRowLoc, *northRowLoc, *southRowLoc;

	scrnY = ZOOM_Y1;
	for( y = topLoc; y <= bottomLoc; ++y, scrnY += ZOOM_LOC_HEIGHT)
	{
		thisRowLoc = get_loc(leftLoc, y);
		northRowLoc = y > 0 ? get_loc(leftLoc, y-1) : thisRowLoc;
		southRowLoc = y+1 < max_y_loc ? get_loc(leftLoc, y+1): thisRowLoc;

		// load north bit into bit0, north west bit into bit 1
		int northRow = northRowLoc->explored() ? 1 : 0;
		int thisRow = thisRowLoc->explored() ? 1 : 0;
		int southRow = southRowLoc->explored() ? 1 : 0;

		if( leftLoc > 0)
		{
			northRow |= (northRowLoc-1)->explored() ? 2 : 0;
			thisRow  |= (thisRowLoc -1)->explored() ? 2 : 0;
			southRow |= (southRowLoc-1)->explored() ? 2 : 0;
		}
		else
		{
			// replicate bit 0 to bit 1;
			northRow *= 3;
			thisRow  *= 3;
			southRow *= 3;
		}

		scrnX = ZOOM_X1;
		for( x = leftLoc; x <= rightLoc; ++x, scrnX += ZOOM_LOC_WIDTH )
		{
			if( x+1 < max_x_loc)
			{
				northRow = (northRow << 1) | ((++northRowLoc)->explored() ? 1 : 0);
				thisRow  = (thisRow  << 1) | ((++thisRowLoc )->explored() ? 1 : 0);
				southRow = (southRow << 1) | ((++southRowLoc)->explored() ? 1 : 0);
			}
			else
			{
				// replicate bit 1
				northRow = (northRow << 1) | (northRow & 1);
				thisRow  = (thisRow  << 1) | (thisRow  & 1);
				southRow = (southRow << 1) | (southRow & 1);
			}

			// optional 
			// northRow &= 7;
			// thisRow &= 7;
			// southRow &= 7;

			// ---------- Draw mask to vgabuf --------//

			if( thisRow & 2)		// center square
			{
				explored_mask.draw(scrnX, scrnY, northRow, thisRow, southRow);
			}
			else
			{
				vga_back.black_32x32(scrnX, scrnY);
			}
		}
	}
	*/
}
//----------- End of function ZoomMatrix::blacken_unexplored ------------//


//---------- Begin of function ZoomMatrix::blacken_fog_of_war -----------//
//
void ZoomMatrix::blacken_fog_of_war()
{
	/*
	int leftLoc = top_x_loc;
	int topLoc = top_y_loc;
	int rightLoc = leftLoc + disp_x_loc - 1;
	int bottomLoc = topLoc + disp_y_loc - 1;
	int scrnY, scrnX;		// screen coordinate
	int x, y;				// x,y Location
	Location *thisRowLoc, *northRowLoc, *southRowLoc;

	if( config.fog_mask_method == 1)
	{
		// use fast method
		scrnY = ZOOM_Y1;
		for( y = topLoc; y <= bottomLoc; ++y, scrnY += ZOOM_LOC_HEIGHT)
		{
			thisRowLoc = get_loc(leftLoc,y);
			scrnX = ZOOM_X1;
			for( x = leftLoc; x <= rightLoc; ++x, scrnX += ZOOM_LOC_WIDTH, ++thisRowLoc )
			{
				if( !thisRowLoc->explored() )
				{
					vga_back.bar(scrnX, scrnY, scrnX+ZOOM_LOC_WIDTH-1, scrnY+ZOOM_LOC_HEIGHT-1, 0);
				}
				else
				{
					unsigned char v = thisRowLoc->visibility();
					if( v < MAX_VISIT_LEVEL-7)
					{
						// more visible draw 1/4 tone
						vga_back.pixelize_32x32(scrnX+1, scrnY, 0);
						vga_back.pixelize_32x32(scrnX, scrnY+1, 0);
					}
					// for visibility >= MAX_VISIT_LEVEL, draw nothing
				}
			}
		}
	}
	else
	{
		// use slow method
		scrnY = ZOOM_Y1;
		for( y = topLoc; y <= bottomLoc; ++y, scrnY += ZOOM_LOC_HEIGHT)
		{
			thisRowLoc = get_loc(leftLoc, y);
			northRowLoc = y > 0 ? get_loc(leftLoc, y-1) : thisRowLoc;
			southRowLoc = y+1 < max_y_loc ? get_loc(leftLoc, y+1): thisRowLoc;

			// load north bit into bit0, north west bit into bit 1
			// [2] = west, [1] = this, [0] = east
			unsigned char northRow[3];
			unsigned char thisRow[3];
			unsigned char southRow[3];
			northRow[0] = northRowLoc->visibility();
			thisRow[0] = thisRowLoc->visibility();
			southRow[0] = southRowLoc->visibility();

			if( leftLoc > 0)
			{
				northRow[1] = (northRowLoc-1)->visibility();
				thisRow[1] = (thisRowLoc-1)->visibility();
				southRow[1] = (southRowLoc-1)->visibility();
			}
			else
			{
				// copy [0] to [1]
				northRow[1] = northRow[0];
				thisRow[1] = thisRow[0];
				southRow[1] = southRow[0];
			}

			scrnX = ZOOM_X1;
			for( x = leftLoc; x <= rightLoc; ++x, scrnX += ZOOM_LOC_WIDTH )
			{
				// shift to west
				northRow[2] = northRow[1]; northRow[1] = northRow[0];
				thisRow[2] = thisRow[1]; thisRow[1] = thisRow[0];
				southRow[2] = southRow[1]; southRow[1] = southRow[0];

				// shift in east squares of each row
				if( x+1 < max_x_loc)
				{
					northRow[0] = (++northRowLoc)->visibility();
					thisRow[0] = (++thisRowLoc)->visibility();
					southRow[0] = (++southRowLoc)->visibility();
				}
				// if on the east of the map, simply replicate the eastest square

				// ---------- Draw mask to vgabuf --------//
				unsigned char midNorthRow[3];
				unsigned char midThisRow[3];
				unsigned char midSouthRow[3];
				midThisRow[2] = min( thisRow[2], thisRow[1]);
				midThisRow[0] = min( thisRow[0], thisRow[1]);
				midNorthRow[2] = min( min(northRow[2], northRow[1]), midThisRow[2] );
				midNorthRow[1] = min( northRow[1], thisRow[1]);
				midNorthRow[0] = min( min(northRow[0], northRow[1]), midThisRow[0] );
				midSouthRow[2] = min( min(southRow[2], southRow[1]), midThisRow[2] );
				midSouthRow[1] = min( southRow[1], thisRow[1]);
				midSouthRow[0] = min( min(southRow[0], southRow[1]), midThisRow[0] );
				unsigned char midMean = ((int) thisRow[0] + thisRow[2] +
					northRow[0] + northRow[1] + northRow[2] +
					southRow[0] + southRow[1] + southRow[2] ) /8;
				midThisRow[1] = min(thisRow[1], midMean );

				vga_back.fog_remap(scrnX, scrnY, (char **)explored_mask.brightness_table->get_table_array(),
					midNorthRow, midThisRow, midSouthRow);
			}
		}
	}
	*/
}
//---------- End of function ZoomMatrix::blacken_fog_of_war -----------//


//--------- Begin of function ZoomMatrix::draw_objects ---------//
//
// Draw the following types of objects on the zoom map in a sorted order.
//
// 1. Firms
// 2. Town sections
// 3. Sprites
//
void ZoomMatrix::draw_objects()
{

// #define SORT_OBJECT_METHOD 0	// foot of max curY
#define SORT_OBJECT_METHOD 1	// centre of absY


	//----- get the location of the zoom area ------//
	// match the same value in opower.cpp
#define DRAW_UPSIDE 4
#define DRAW_DOWNSIDE 24
#define DRAW_LEFT_RIGHT 6
  
	//---- add the objects on the zoom area to land_disp_sort_array in a sorted display order ---//

	DisplaySort  displaySort;
	//Unit*			 unitPtr;
	//Firm*			 firmPtr;
	//Town* 	 	 townPtr;
	//PlantBitmap* plantBitmap;
	//int			 innerY, drawY;

	int			 dispFire = 0;
	char			 pMobileType;           // pointing mobileType
	Location*	 pLoc = power.test_detect(mouse.cur_x, mouse.cur_y, &pMobileType);

	int rowXLoc = world.zoom_matrix->top_x_loc - DRAW_LEFT_RIGHT - DRAW_UPSIDE;
	int rowYLoc = world.zoom_matrix->top_y_loc + DRAW_LEFT_RIGHT - DRAW_UPSIDE;
	int maxRowCount = image_height / (ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT) * 2
		+ DRAW_DOWNSIDE * 2;
	int maxColCount = image_width / (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH) * 2
		+ DRAW_LEFT_RIGHT * 2;

	int rowCount, colCount;

	for( rowCount = -DRAW_UPSIDE*2; rowCount <= maxRowCount; (rowCount&1?++rowXLoc:++rowYLoc), rowCount++ )
	{
		int xLoc = rowXLoc;
		int yLoc = rowYLoc;

		for( colCount = -DRAW_LEFT_RIGHT*2; colCount <= maxColCount; ++xLoc, --yLoc, colCount += 2 )
		{
			if( xLoc >= 0 && xLoc < max_x_loc && yLoc >= 0 && yLoc < max_y_loc )
			{
				LocationCorners lc;
				get_loc_corner(xLoc, yLoc, &lc);
				Location *locPtr = lc.loc_ptr;

				//------- if there is an unit in the air --------//

				if( locPtr->unit_recno(UNIT_AIR) )
				{
					memset(&displaySort, 0, sizeof(displaySort));
					Unit *unitPtr = unit_array[locPtr->air_cargo_recno];

					if( !unitPtr->is_stealth() && unitPtr->next_x_loc() == xLoc && unitPtr->next_y_loc() == yLoc )
					{
#if(SORT_OBJECT_METHOD == 0)
						unitPtr->update_abs_pos();		// update its absolute position
#endif

						displaySort.object_type  = OBJECT_UNIT;
						displaySort.object_recno = locPtr->air_cargo_recno;
#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 	 = unitPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2	 = calc_abs_y(unitPtr->cur_x + LOCATE_WIDTH/2, 
							unitPtr->cur_y + LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						// ##### begin Gilbert 19/9 #######//
						// if(pLoc == locPtr && pMobileType == UNIT_AIR)	// BUGHERE : this part may fails if sprite size > 1x1
						if( pLoc && pLoc->unit_recno(pMobileType) == displaySort.object_recno )
						{
							displaySort.object_type  = OBJECT_POINTED_UNIT;
							displaySort.draw_flags |= OBJECT_DRAW_POINTED;		// outline flag
						}
						// ##### end Gilbert 19/9 #######//

						air_disp_sort_array.linkin(&displaySort);
					}
				}

				//------- if there is an unit on the land or sea -------//

				if( locPtr->unit_recno(UNIT_LAND) || locPtr->unit_recno(UNIT_SEA) )
				{
					memset(&displaySort, 0, sizeof(displaySort));
					Unit *unitPtr = unit_array[locPtr->cargo_recno];

					if( !unitPtr->is_stealth() && unitPtr->next_x_loc() == xLoc && unitPtr->next_y_loc() == yLoc )
					{

#if(SORT_OBJECT_METHOD == 0)
						unitPtr->update_abs_pos();		// update its absolute position
#endif

						displaySort.object_type  = OBJECT_UNIT;
						displaySort.object_recno = locPtr->cargo_recno;
#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 	 = unitPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2	 = calc_abs_y(unitPtr->cur_x + LOCATE_WIDTH/2, 
							unitPtr->cur_y + LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						// if( pLoc == locPtr && (pMobileType == UNIT_LAND || pMobileType == UNIT_SEA)) // BUGHERE : this part may fails if sprite size > 1x1
						if( pLoc && pLoc->unit_recno(pMobileType) && pLoc->unit_recno(pMobileType) == displaySort.object_recno )
						{
							displaySort.object_type = OBJECT_POINTED_UNIT;
							displaySort.draw_flags |= OBJECT_DRAW_POINTED;		// outline flag
						}

						// ---- calculate draw_flags ------//
						if( hidden_by_place( unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), 
								unitPtr->sprite_info->loc_width, unitPtr->sprite_info->loc_height) )
							displaySort.draw_flags |= OBJECT_DRAW_HIDDEN;

						land_disp_sort_array.linkin(&displaySort);
					}
				}

				//--------- if there is a firm on the location --------//

				else if( locPtr->is_firm() )
				{
					Firm *firmPtr = firm_array[locPtr->firm_recno()];

					if( !firmPtr->is_stealth() && xLoc==firmPtr->loc_x1 && yLoc==firmPtr->loc_y1 )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_FIRM;
						displaySort.object_recno = locPtr->firm_recno();

#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 = firmPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2 = calc_abs_y(
							(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
							(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						// ##### begin Gilbert 19/9 ######//
						if( pLoc && pLoc->is_firm() && pLoc->firm_recno() == displaySort.object_recno )
						{
							displaySort.draw_flags |= OBJECT_DRAW_POINTED;
						}
						// ##### end Gilbert 19/9 ######//

						land_bottom_disp_sort_array.linkin(&displaySort);
						land_disp_sort_array.linkin(&displaySort);
					}
				}

				//------ if there is a town section on the location -----//

				else if( locPtr->is_town() )
				{
					Town *townPtr = town_array[locPtr->town_recno()];

					if( !townPtr->is_stealth() && xLoc==townPtr->loc_x1 && yLoc==townPtr->loc_y1 )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_TOWN;
						displaySort.object_recno = locPtr->town_recno();

#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 = townPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2 = calc_abs_y(
							(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
							(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						// ##### begin Gilbert 19/9 ######//
						if( pLoc && pLoc->is_town() && pLoc->town_recno() == displaySort.object_recno )
						{
							displaySort.draw_flags |= OBJECT_DRAW_POINTED;
						}
						// ##### end Gilbert 19/9 ######//

						land_bottom_disp_sort_array.linkin(&displaySort);
						land_disp_sort_array.linkin(&displaySort);
					}
				}

				//------ if there is a plant on the location -----//

				else if( locPtr->is_plant() )
				{
					// if( !config.blacken_map || world.is_explored(xLoc, yLoc, xLoc, yLoc) )
					if( !config.blacken_map || locPtr->explored() )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_PLANT;
						displaySort.object_recno = locPtr->plant_id();
						displaySort.x_loc	    = xLoc;
						displaySort.y_loc	    = yLoc;

						PlantBitmap *plantBitmap = plant_res.get_bitmap(locPtr->plant_id());

						int innerX  = locPtr->plant_inner_x();
						int innerY  = locPtr->plant_inner_y();
						// BUGHERE : drawY ???
#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 = yLoc*LOCATE_HEIGHT + innerY-LOCATE_HEIGHT/2 + plantBitmap->offset_y + plantBitmap->bitmap_height - 1;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2 = calc_abs_y(xLoc*LOCATE_WIDTH+innerX,
							yLoc*LOCATE_HEIGHT+innerY, 0);
#else
	#error
#endif

						land_disp_sort_array.linkin(&displaySort);
					}
				}

				//------ if there is a wall on the location -------//

				else if( locPtr->is_wall() )
				{
					/*
					memset(&displaySort, 0, sizeof(displaySort));
					WallInfo *wallInfo = wall_res[locPtr->wall_id()];
					displaySort.object_type	= OBJECT_WALL;

					// high byte of object_recno stores nation_recno
					// low byte of object_recno stores wall_id
					displaySort.object_recno = wallInfo->draw_wall_id;
					if( locPtr->power_nation_recno > 0)
					{
						displaySort.object_recno += locPtr->power_nation_recno << 8;
					}
					if( ! wallInfo->is_gate())
					{
						// -------- non-gate square
						displaySort.object_y2 = yLoc* ZOOM_LOC_HEIGHT +ZOOM_LOC_HEIGHT-1;
						displaySort.x_loc = xLoc * ZOOM_LOC_WIDTH;
						displaySort.y_loc = yLoc * ZOOM_LOC_HEIGHT;
						land_disp_sort_array.linkin(&displaySort);
					}
					else
					{
						// -------- gate square ---------//
						displaySort.object_y2 = yLoc * ZOOM_LOC_HEIGHT + wallInfo->offset_y +
							wallInfo->bitmap_height() -1;
						displaySort.x_loc = xLoc + wallInfo->loc_off_x;
						displaySort.y_loc = yLoc + wallInfo->loc_off_y;
						if( xLoc == max( displaySort.x_loc, zoomXLoc1) &&
							 yLoc == max( displaySort.y_loc, zoomYLoc1) )
						{
							displaySort.x_loc = xLoc * ZOOM_LOC_WIDTH + wallInfo->offset_x;
							displaySort.y_loc = yLoc * ZOOM_LOC_HEIGHT + wallInfo->offset_y;
							land_disp_sort_array.linkin(&displaySort);
						}
					}
					*/
				}
				else if(locPtr->has_hill() && hill_res[locPtr->hill_id1()]->layer & 2 )
				{
					/*
					memset(&displaySort, 0, sizeof(displaySort));
					displaySort.object_type = OBJECT_HILL;
					displaySort.object_recno = locPtr->hill_id1();
					displaySort.object_y2 = (yLoc+1)*LOCATE_HEIGHT-1;
					displaySort.x_loc = xLoc;
					displaySort.y_loc = yLoc;
					land_disp_sort_array.linkin(&displaySort);
					*/
				}
				else if(locPtr->is_rock())
				{
					Rock *rockPtr = rock_array[locPtr->rock_array_recno()];
					if( !rockPtr->is_stealth() && xLoc==rockPtr->loc_x && yLoc==rockPtr->loc_y )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_ROCK;
						displaySort.object_recno = locPtr->rock_array_recno();

#if(SORT_OBJECT_METHOD == 0)
						// displaySort.object_y2 = rockPtr->abs_y2;
						err_here();
#elif(SORT_OBJECT_METHOD == 1)
						RockInfo *rockInfo = rock_res.get_rock_info(rockPtr->rock_recno);
						displaySort.object_y2 = calc_abs_y(
							(rockPtr->loc_x*2 +rockInfo->loc_width +1)*LOCATE_WIDTH/2,
							(rockPtr->loc_y*2 +rockInfo->loc_height +1)*LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						land_disp_sort_array.linkin(&displaySort);
					}

				}

				if( locPtr->has_site() && locPtr->walkable(3) )		// don't display if a building/object has already been built on the location
				{
					Site *sitePtr = site_array[locPtr->site_recno()];

					if( !sitePtr->is_stealth() && xLoc==sitePtr->map_x_loc && yLoc==sitePtr->map_y_loc )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_SITE;
						displaySort.object_recno = locPtr->site_recno();
						displaySort.x_loc = xLoc;
						displaySort.y_loc = yLoc;
#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 =	LOCATE_HEIGHT * yLoc;
#elif(SORT_OBJECT_METHOD == 1)
//						displaySort.object_y2 = calc_abs_y(xLoc+LOCATE_WIDTH/2, 
//							yLoc+LOCATE_HEIGHT/2, 0);
						displaySort.object_y2 = calc_abs_y(
							(sitePtr->map_x_loc + sitePtr->map_x2 + 1)*LOCATE_WIDTH/2,
							(sitePtr->map_y_loc + sitePtr->map_y2 + 1)*LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						if( pLoc && pLoc->has_site() && pLoc->site_recno() == displaySort.object_recno )
						{
							displaySort.draw_flags |= OBJECT_DRAW_POINTED;
						}

						if( sitePtr->map_x2 == sitePtr->map_x_loc &&	sitePtr->map_y2 == sitePtr->map_y_loc
							&& hidden_by_place(xLoc, yLoc, 1, 1 ) )
						{
							// only 1x1 site can reveal hidden
							displaySort.draw_flags |= OBJECT_DRAW_HIDDEN;
							land_disp_sort_array.linkin(&displaySort);			// hidden
						}
						else
							land_bottom_disp_sort_array.linkin(&displaySort);	// not hidden
					}
				}
				// ###### begin Gilbert 21/9 ######## //
				else if(locPtr->has_dirt())
				{
					Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
					if( !dirtPtr->is_stealth() && xLoc==dirtPtr->loc_x && yLoc==dirtPtr->loc_y )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type  = OBJECT_DIRT;
						displaySort.object_recno = locPtr->dirt_recno();

#if(SORT_OBJECT_METHOD == 0)
						// displaySort.object_y2 = dirtPtr->abs_y2;
						err_here();
#elif(SORT_OBJECT_METHOD == 1)
						RockInfo *rockInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
						displaySort.object_y2 = calc_abs_y(
							(dirtPtr->loc_x*2 +rockInfo->loc_width +1)*LOCATE_WIDTH/2,
							(dirtPtr->loc_y*2 +rockInfo->loc_height +1)*LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
						land_bottom_disp_sort_array.linkin(&displaySort);
					}
				}
				// ###### end Gilbert 21/9 ######## //

				#ifdef DEBUG2	
				if(debug_sim_game_type!=2 && locPtr->fire_str()>0)
				#else
				if( locPtr->fire_str() > 0 )
				#endif
				{
					// if( !config.blacken_map || world.is_explored(xLoc, yLoc, xLoc, yLoc) )
					if( !config.blacken_map || locPtr->explored() )
					{
						memset(&displaySort, 0, sizeof(displaySort));
						displaySort.object_type = OBJECT_FIRE;
						displaySort.object_recno = locPtr->fire_str();
						displaySort.x_loc	    = xLoc;
						displaySort.y_loc	    = yLoc;
						int innerX = LOCATE_WIDTH/2 + (((xLoc+11) * (yLoc+13)) % (LOCATE_WIDTH/2));
						int innerY = LOCATE_HEIGHT/2 + (((xLoc+13) * (yLoc+17)) % (LOCATE_HEIGHT/2));
#if(SORT_OBJECT_METHOD == 0)
						displaySort.object_y2 = (yLoc+1)*LOCATE_HEIGHT - innerY;
#elif(SORT_OBJECT_METHOD == 1)
						displaySort.object_y2 = calc_abs_y( xLoc*LOCATE_WIDTH + innerX,
							yLoc*LOCATE_HEIGHT + innerY, 0);
#else
	#error
#endif
						land_disp_sort_array.linkin(&displaySort);
						land_top_disp_sort_array.linkin(&displaySort);
						dispFire++;
					}
				}
			}
		}
	}

	//------ add bullet sprites to the display array -------//

	Bullet* bulletPtr;

	for( int i=bullet_array.size() ; i>0 ; i-- )
	{
		if( bullet_array.is_deleted(i) )
			continue;

		bulletPtr = bullet_array[i];

		if( bulletPtr->is_stealth() )
			continue;

		colCount = (bulletPtr->cur_x_loc() - top_x_loc) - (bulletPtr->cur_y_loc() - top_y_loc);
		rowCount = (bulletPtr->cur_x_loc() - top_x_loc) + (bulletPtr->cur_y_loc() - top_y_loc);

		if(( colCount < -DRAW_LEFT_RIGHT*2 || colCount > maxColCount ||
			rowCount < -DRAW_UPSIDE*2 || rowCount > maxRowCount ) &&
			(bulletPtr->sprite_info->sprite_sub_type != 'M'))
			continue;

		displaySort.object_type  = OBJECT_BULLET;
		displaySort.object_recno = i;
		if (bulletPtr->sprite_info->sprite_sub_type != 'M')
		{

			bulletPtr->update_abs_pos();		// update its absolute position

#if(SORT_OBJECT_METHOD == 0)
		displaySort.object_y2 	 = bulletPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
		displaySort.object_y2    = calc_abs_y(bulletPtr->cur_x + LOCATE_WIDTH/2,
			bulletPtr->cur_y + LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
		}


		switch( bulletPtr->display_layer() )
		{
		case AIR_DISP_LAYER_MASK:
			air_disp_sort_array.linkin(&displaySort);
			break;
		case LAND_BOTTOM_DISP_LAYER_MASK:
			land_bottom_disp_sort_array.linkin(&displaySort);
			break;
		case LAND_TOP_DISP_LAYER_MASK:
			land_top_disp_sort_array.linkin(&displaySort);
			break;
		case 0:
		case LAND_DISP_LAYER_MASK:

			// ---- calculate draw_flags ------//
			displaySort.draw_flags = hidden_by_place( 
				bulletPtr->cur_x_loc(), bulletPtr->cur_y_loc(), 
				1, 1 );

			land_disp_sort_array.linkin(&displaySort);
			break;
		default:
			err_here();
		}
	}

	// --------- draw tornado --------//
	Tornado *tornadoPtr;
	for( i=tornado_array.size(); i > 0; i--)
	{
		if( tornado_array.is_deleted(i) )
			continue;
		tornadoPtr = tornado_array[i];

		if( tornadoPtr->cur_x_loc() < 0 || tornadoPtr->cur_y_loc() < 0
			|| tornadoPtr->cur_x_loc() >= MAX_WORLD_X_LOC || tornadoPtr->cur_y_loc() >= MAX_WORLD_Y_LOC )
			continue;

		if( tornadoPtr->is_stealth() )		// check inside map before call is_stealth();
			continue;

		colCount = (tornadoPtr->cur_x_loc() - top_x_loc) - (tornadoPtr->cur_y_loc() - top_y_loc);
		rowCount = (tornadoPtr->cur_x_loc() - top_x_loc) + (tornadoPtr->cur_y_loc() - top_y_loc);

		if( colCount < -DRAW_LEFT_RIGHT*2 || colCount > maxColCount ||
			rowCount < -DRAW_UPSIDE*2 || rowCount > maxRowCount )
			continue;

#if(SORT_OBJECT_METHOD == 0)
		tornadoPtr->update_abs_pos();
#endif

		displaySort.object_type = OBJECT_TORNADO;
		displaySort.object_recno  = i;
#if(SORT_OBJECT_METHOD == 0)
		displaySort.object_y2 = tornadoPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
		displaySort.object_y2 = calc_abs_y(tornadoPtr->cur_x+LOCATE_WIDTH/2,
			tornadoPtr->cur_y+LOCATE_HEIGHT/2, 0);
#else
	#error
#endif
		air_disp_sort_array.linkin(&displaySort);
	}

	for( i=effect_array.size(); i > 0; i--)
	{
		if( effect_array.is_deleted(i) )
			continue;
		Effect *effectPtr = (Effect *)effect_array[i];

		if( effectPtr->is_stealth() )
			continue;

		colCount = (effectPtr->cur_x_loc() - top_x_loc) - (effectPtr->cur_y_loc() - top_y_loc);
		rowCount = (effectPtr->cur_x_loc() - top_x_loc) + (effectPtr->cur_y_loc() - top_y_loc);

		if( colCount < -DRAW_LEFT_RIGHT*2 || colCount > maxColCount ||
			rowCount < -DRAW_UPSIDE*2 || rowCount > maxRowCount )
			continue;

#if(SORT_OBJECT_METHOD == 0)
		effectPtr->update_abs_pos();
#endif

		displaySort.object_type = OBJECT_EFFECT;
		displaySort.object_recno  = i;
#if(SORT_OBJECT_METHOD == 0)
		displaySort.object_y2 = effectPtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
		displaySort.object_y2 = calc_abs_y(effectPtr->cur_x+LOCATE_WIDTH/2,
			effectPtr->cur_y+LOCATE_HEIGHT/2, 0);
#else
	#error
#endif

		switch( effectPtr->layer )
		{
		case AIR_DISP_LAYER_MASK:
			air_disp_sort_array.linkin(&displaySort);
			break;
		case LAND_BOTTOM_DISP_LAYER_MASK:
			land_bottom_disp_sort_array.linkin(&displaySort);
			break;
		case LAND_TOP_DISP_LAYER_MASK:
			land_top_disp_sort_array.linkin(&displaySort);
			break;
		case 0:
		case LAND_DISP_LAYER_MASK:
			land_disp_sort_array.linkin(&displaySort);
			break;
		default:
			err_here();
		}
	}

	
	for( i=firm_die_array.size(); i > 0; i--)
	{
		if( firm_die_array.is_deleted(i) )
			continue;

		FirmDie *firmDiePtr = (FirmDie *)firm_die_array[i];

		if( firmDiePtr->is_stealth() )
			continue;

	//	if( xLoc==firmDiePtr->loc_x1 && yLoc==firmDiePtr->loc_y1 )
		{
			memset(&displaySort, 0, sizeof(displaySort));
			displaySort.object_type  = OBJECT_FIRM_DIE;
			displaySort.object_recno = i;
		
#if(SORT_OBJECT_METHOD == 0)
			err_here(); // have not set abs_y2 for firmdie
			displaySort.object_y2 = firmDiePtr->abs_y2;
#elif(SORT_OBJECT_METHOD == 1)
			displaySort.object_y2 = calc_abs_y(
			(firmDiePtr->parent_loc_x1+firmDiePtr->parent_loc_x2+1)*LOCATE_WIDTH/2 + LOCATE_WIDTH/4,
			(firmDiePtr->parent_loc_y1+firmDiePtr->parent_loc_y2+1)*LOCATE_HEIGHT/2 + LOCATE_HEIGHT/4, 0);
#else
	#error
#endif
		//	air_disp_sort_array.linkin(&displaySort);
		//	land_top_disp_sort_array.linkin(&displaySort);
			land_bottom_disp_sort_array.linkin(&displaySort);
			land_disp_sort_array.linkin(&displaySort);
		}
	}
	


	//---------- quicksort the array -----------//

	land_disp_sort_array.quick_sort( sort_display_function );
	air_disp_sort_array.quick_sort( sort_display_function );
	land_top_disp_sort_array.quick_sort( sort_display_function );
	land_bottom_disp_sort_array.quick_sort( sort_display_function );

	// ##### begin Gilbert 9/10 ######//
	//------------ draw unit path and objects ---------------//

	draw_objects_now(&land_bottom_disp_sort_array, LAND_BOTTOM_DISP_LAYER_MASK);
	draw_unit_path_on_zoom_map(LAND_DISP_LAYER_MASK);
	draw_objects_now(&land_disp_sort_array,LAND_DISP_LAYER_MASK);
	draw_objects_now(&land_disp_sort_array,LAND_DISP_LAYER_MASK, 1);        // redraw hidden object
	draw_objects_now(&land_top_disp_sort_array,LAND_TOP_DISP_LAYER_MASK);

	draw_unit_path_on_zoom_map(AIR_DISP_LAYER_MASK);
	draw_objects_now(&air_disp_sort_array);
	way_point_array.draw(2);		// 2-draw on the zoom map

	//----------- clean up the array ----------//

	land_disp_sort_array.zap(0);		// 0-don't resize the array, keep its current size
	air_disp_sort_array.zap(0);		// 0-don't resize the array, keep its current size
	land_top_disp_sort_array.zap(0);
	land_bottom_disp_sort_array.zap(0);

	//----------- fire sound ----------//
	if(dispFire > 0)
	{
		int relVolume = 80 + dispFire/2;
		if( relVolume > 100)
			relVolume = 100;
		if( fire_channel_id == 0)
		{
			last_fire_vol = relVolume;
		//	fire_channel_id = audio.play_loop_wav( DIR_SOUND"FIRE.WAV",8447 *2, RelVolume(relVolume,0));
                        RelVolume r(relVolume,0);
                        fire_channel_id = audio.play_loop_wav( DIR_SOUND"FIRE.WAV",0, DsVolume(r));
		}
		else if( last_fire_vol - relVolume > 2 || last_fire_vol - relVolume < 2)
		{
			last_fire_vol = relVolume;
			RelVolume r(relVolume,0);
                        audio.volume_loop_wav(fire_channel_id, DsVolume(r));
		}
	}
	else
	{
		if( fire_channel_id != 0)
		{
			audio.stop_loop_wav(fire_channel_id);
			fire_channel_id = 0;
			last_fire_vol = 0;
		}
	}
}
//----------- End of function ZoomMatrix::draw_objects -----------//

static RGBColor identity_color_function(RGBColor col, int, int)
{
	return col;
}

//---------- Begin of function ZoomMatrix::draw_objects_now -----------//
//
void ZoomMatrix::draw_objects_now(DynArray* unitArray, int displayLayer, char flags)
{
	//------------ display objects ------------//

	DisplaySort *displaySortPtr;
	int 			i, dispCount = unitArray->size();
	char			firstFire[FLAME_GROW_STEP];
	memset( firstFire, 0, sizeof(firstFire));
	int			riseFirePara = 0;
	int			needFlushFire = weather.rain_scale() + weather.snow_scale();
	double hWindSpeed = weather.wind_speed()*sin(weather.wind_direct_rad());
	if( hWindSpeed >= 20.0)
		riseFirePara = 1;
	else if( hWindSpeed > -20.0)
		riseFirePara = 0;
	else
		riseFirePara = -1;

	if( init_fire <= flame[FLAME_GROW_STEP-1].map_height)
	{
		for( int f = FLAME_GROW_STEP-1 ; f >= 0; --f)
		{
			if( init_fire <= flame[f].map_height)
			{
				flame[f].rise(riseFirePara);
			}
			else
			{
				break;
			}
		}
		init_fire++;
	}

	int dispPower = (world.map_matrix->map_mode == MAP_MODE_POWER &&
						  world.map_matrix->power_mode == 1) ||
						 power.command_id == COMMAND_BUILD_FIRM ||
						 power.command_id == COMMAND_SETTLE;

	for( i=1 ; i<=dispCount ; i++ )
	{
		if( i%10==1 )
			sys.yield();

		displaySortPtr = (DisplaySort*) unitArray->get(i);

		if( flags && !(displaySortPtr->draw_flags & flags) )		// draw on flags || (displaySortPtr->draw_flags & flags)
			continue;

		switch(displaySortPtr->object_type)
		{
			case OBJECT_UNIT:
			case OBJECT_POINTED_UNIT:
				{
					Unit *unitPtr = unit_array[displaySortPtr->object_recno];

					// ------- draw sqaure for outline ------//

					if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED || unitPtr->selected_flag )
					{
						int animLineColorSeries = 2 + ( unitPtr->selected_flag | 
							displaySortPtr->draw_flags & OBJECT_DRAW_POINTED );
						short curX1 = unitPtr->cur_x;
						short curY1 = unitPtr->cur_y;
						short curX2 = curX1 + unitPtr->sprite_info->loc_width * LOCATE_WIDTH - 1;
						short curY2 = curY1 + unitPtr->sprite_info->loc_height * LOCATE_HEIGHT - 1;
						short curZ0 = world.interpolate_z(curX1, curY1);
						short curZ1 = world.interpolate_z(curX2, curY1);
						short curZ2 = world.interpolate_z(curX2, curY2);
						short curZ3 = world.interpolate_z(curX1, curY2);
						int rectX0 = ZOOM_X1 + calc_zoom_x( curX1, curY1, curZ0 );
						int rectY0 = ZOOM_Y1 + calc_zoom_y( curX1, curY1, curZ0 );
						int rectX1 = ZOOM_X1 + calc_zoom_x( curX2, curY1, curZ1 );
						int rectY1 = ZOOM_Y1 + calc_zoom_y( curX2, curY1, curZ1 );
						int rectX2 = ZOOM_X1 + calc_zoom_x( curX2, curY2, curZ2 );
						int rectY2 = ZOOM_Y1 + calc_zoom_y( curX2, curY2, curZ2 );
						int rectX3 = ZOOM_X1 + calc_zoom_x( curX1, curY2, curZ3 );
						int rectY3 = ZOOM_Y1 + calc_zoom_y( curX1, curY2, curZ3 );

						// draw a cubic for pointed
						if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED )
						{
							// draw a back line
							anim_line.draw_line( &vga_back, rectX0, rectY0,
								rectX0, rectY0+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
						}

						anim_line.draw_line( &vga_back, rectX0, rectY0, rectX1, rectY1, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX1, rectY1, rectX2, rectY2, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX2, rectY2, rectX3, rectY3, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX3, rectY3, rectX0, rectY0, 1, animLineColorSeries);

						// ------- draw unit ------//

						unitPtr->draw(	displaySortPtr->object_type == OBJECT_POINTED_UNIT,
							displaySortPtr->draw_flags & flags );

						if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED )
						{
							// draw a back line
							anim_line.draw_line( &vga_back, rectX1, rectY1,
								rectX1, rectY1+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
							anim_line.draw_line( &vga_back, rectX2, rectY2,
								rectX2, rectY2+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
							anim_line.draw_line( &vga_back, rectX3, rectY3,
								rectX3, rectY3+unitPtr->sprite_info->max_height, 1, animLineColorSeries);

							anim_line.draw_line( &vga_back, rectX0, rectY0+unitPtr->sprite_info->max_height, 
								rectX1, rectY1+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
							anim_line.draw_line( &vga_back, rectX1, rectY1+unitPtr->sprite_info->max_height,
								rectX2, rectY2+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
							anim_line.draw_line( &vga_back, rectX2, rectY2+unitPtr->sprite_info->max_height,
								rectX3, rectY3+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
							anim_line.draw_line( &vga_back, rectX3, rectY3+unitPtr->sprite_info->max_height,
								rectX0, rectY0+unitPtr->sprite_info->max_height, 1, animLineColorSeries);
						}
					}
					else
					{
						// ------- draw unit ------//

						unitPtr->draw(	displaySortPtr->object_type == OBJECT_POINTED_UNIT,
							displaySortPtr->draw_flags & flags );
					}
				}
				break;
		
			case OBJECT_BULLET:
				bullet_array[displaySortPtr->object_recno]->draw(0,
					displaySortPtr->draw_flags & flags );
				break;

			case OBJECT_FIRM:
				{
					firm_array[displaySortPtr->object_recno]->draw(displayLayer);
					Firm *firmPtr = firm_array[displaySortPtr->object_recno];
				//	firmPtr->draw(displayLayer);
					// ------- draw sqaure for outline ------//

					if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED || 
						firm_array.selected_recno == displaySortPtr->object_recno )
					{
						int animLineColorSeries = 2 + 
							( (firm_array.selected_recno == displaySortPtr->object_recno) | 
							displaySortPtr->draw_flags & OBJECT_DRAW_POINTED );

						short curX1 = firmPtr->loc_x1 * LOCATE_WIDTH;
						short curY1 = firmPtr->loc_y1 * LOCATE_HEIGHT;
						short curX2 = (firmPtr->loc_x2 + 1) * LOCATE_WIDTH - 1;
						short curY2 = (firmPtr->loc_y2 + 1) * LOCATE_HEIGHT - 1;
						short curZ = firmPtr->altitude;
						int rectX0 = ZOOM_X1 + calc_zoom_x( curX1, curY1, curZ );
						int rectY0 = ZOOM_Y1 + calc_zoom_y( curX1, curY1, curZ );
						int rectX1 = ZOOM_X1 + calc_zoom_x( curX2, curY1, curZ );
						int rectY1 = ZOOM_Y1 + calc_zoom_y( curX2, curY1, curZ );
						int rectX2 = ZOOM_X1 + calc_zoom_x( curX2, curY2, curZ );
						int rectY2 = ZOOM_Y1 + calc_zoom_y( curX2, curY2, curZ );
						int rectX3 = ZOOM_X1 + calc_zoom_x( curX1, curY2, curZ );
						int rectY3 = ZOOM_Y1 + calc_zoom_y( curX1, curY2, curZ );
						anim_line.draw_line( &vga_back, rectX0, rectY0, rectX1, rectY1, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX1, rectY1, rectX2, rectY2, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX2, rectY2, rectX3, rectY3, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX3, rectY3, rectX0, rectY0, 1, animLineColorSeries);
					}
				}
				break;

			case OBJECT_TOWN:
				{
					Town *townPtr = town_array[displaySortPtr->object_recno];
					townPtr->draw(displayLayer);

					// ------- draw sqaure for outline ------//

					if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED || 
						town_array.selected_recno == displaySortPtr->object_recno )
					{
						int animLineColorSeries = 2 + 
							( (town_array.selected_recno == displaySortPtr->object_recno) | 
							displaySortPtr->draw_flags & OBJECT_DRAW_POINTED );
						short curX1 = townPtr->loc_x1 * LOCATE_WIDTH;
						short curY1 = townPtr->loc_y1 * LOCATE_HEIGHT;
						short curX2 = (townPtr->loc_x2 + 1) * LOCATE_WIDTH - 1;
						short curY2 = (townPtr->loc_y2 + 1) * LOCATE_HEIGHT - 1;
						short curZ = townPtr->altitude;
						int rectX0 = ZOOM_X1 + calc_zoom_x( curX1, curY1, curZ );
						int rectY0 = ZOOM_Y1 + calc_zoom_y( curX1, curY1, curZ );
						int rectX1 = ZOOM_X1 + calc_zoom_x( curX2, curY1, curZ );
						int rectY1 = ZOOM_Y1 + calc_zoom_y( curX2, curY1, curZ );
						int rectX2 = ZOOM_X1 + calc_zoom_x( curX2, curY2, curZ );
						int rectY2 = ZOOM_Y1 + calc_zoom_y( curX2, curY2, curZ );
						int rectX3 = ZOOM_X1 + calc_zoom_x( curX1, curY2, curZ );
						int rectY3 = ZOOM_Y1 + calc_zoom_y( curX1, curY2, curZ );
						anim_line.draw_line( &vga_back, rectX0, rectY0, rectX1, rectY1, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX1, rectY1, rectX2, rectY2, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX2, rectY2, rectX3, rectY3, 1, animLineColorSeries);
						anim_line.draw_line( &vga_back, rectX3, rectY3, rectX0, rectY0, 1, animLineColorSeries);
					}

				}
				break;

			case OBJECT_PLANT:
				plant_res.get_bitmap(displaySortPtr->object_recno)
					->draw(displaySortPtr->x_loc, displaySortPtr->y_loc);
				break;

			case OBJECT_ROCK:
				// object_recno is rockArrayRecno
				rock_array[displaySortPtr->object_recno]->draw();
				break;

			case OBJECT_DIRT:
				dirt_array[displaySortPtr->object_recno]->draw();
				break;

			case OBJECT_FIRE:
				{
					int f = Flame::grade( displaySortPtr->object_recno );
					// when displayLayer = 0, no fire is assumed to be drawn
					// pass fireDisplayerLayer as 1 to this function
					err_when(!displayLayer);

					// ------- decide bitmap to draw ----
					// display flame[f], where f = (fire_str()-1) /25
					err_when(f >= FLAME_GROW_STEP);
					if( !firstFire[f])
					{
						firstFire[f] = 1;
						if( displayLayer == 1)
						{
							if( needFlushFire )
								flame[f].flush_point();
							flame[f].rise(riseFirePara);
							flame[f].gen_bitmap(0xe3);		// 0xb4
							flame[f].mask_bottom();
						}
						else
						{
							flame[f].mask_transparent();
						}
					}

					int innerX = LOCATE_WIDTH/2 + (((displaySortPtr->x_loc+11) * (displaySortPtr->y_loc+13)) % (LOCATE_WIDTH/2));
					int innerY = LOCATE_HEIGHT/2 + (((displaySortPtr->x_loc+13) * (displaySortPtr->y_loc+17)) % (LOCATE_HEIGHT/2));
					int x1 = displaySortPtr->x_loc * LOCATE_WIDTH + innerX;
					int y1 = displaySortPtr->y_loc * LOCATE_HEIGHT + innerY;

				//	short *colorRemapTable = flame[f].load_palette("FIRE");
					
				//	short *colorRemapTable = VgaBuf::default_remap_table;
				

/*
	String str;
	File palFile;
	ColorTable colorTable;
	short *colorRemapTable = NULL;

	str  = DIR_IMAGE;
	str += "FIRE.COL";

	if( palFile.file_open(str,0) )
	{ 
		BYTE palBuf[256][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 256, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 1, palBufDesc, identity_color_function );
		colorRemapTable = (short *)colorTable.get_table(0);

	}
*/


					put_bitmap_offset( x1, y1, world.interpolate_z(x1, y1), (char *)flame[f].bitmap,
						Flame::offset_x(f), Flame::offset_y(f),
						flame[f].color_remap_table, 0 , 0);
				//	put_bitmap_offset( x1, y1, world.interpolate_z(x1, y1), (char *)flame[f].bitmap,
				//		Flame::offset_x(f), Flame::offset_y(f),
				//		NULL, 0 , 0);
				}
				break;

			case OBJECT_WALL:
				{
					int nationRecno = displaySortPtr->object_recno >> 8;
					short *remapTable = game.get_color_remap_table(nationRecno, 0);
					wall_res[displaySortPtr->object_recno & 0xff]->draw_at(
						displaySortPtr->x_loc, displaySortPtr->y_loc, remapTable);
				}
				break;
			case OBJECT_TORNADO:
				tornado_array[displaySortPtr->object_recno]->draw(0, 
					displaySortPtr->draw_flags & flags );
				break;

			case OBJECT_HILL:
				{
					/*
					short xLoc = displaySortPtr->x_loc;
					short yLoc = displaySortPtr->y_loc;
					hill_res[displaySortPtr->object_recno]->draw(xLoc, yLoc, 2);
					
					// ------ draw power, because hill covers the power colour drawn ------//
					int nationRecno = get_loc(xLoc, yLoc)->power_nation_recno;
					if( dispPower && nationRecno > 0)
					{
						int x1 = xLoc*ZOOM_LOC_WIDTH - World::view_top_x;
						int y1 = yLoc*ZOOM_LOC_HEIGHT - World::view_top_y;
						if( x1 >= 0 && y1 >= 0 && x1 < ZOOM_WIDTH - (ZOOM_LOC_WIDTH-1) && y1 < ZOOM_HEIGHT - (ZOOM_LOC_HEIGHT-1))
						{
							vga_back.pixelize_32x32( x1 + ZOOM_X1, y1 + ZOOM_Y1,
								nation_array.nation_power_color_array[nationRecno] );
						}
					}
					*/
				}
				break;

			case OBJECT_EFFECT:
				effect_array[displaySortPtr->object_recno]->draw(0, 
					displaySortPtr->draw_flags & flags );
				break;

			case OBJECT_FIRM_DIE:
				firm_die_array[displaySortPtr->object_recno]->draw(displayLayer);
				break;

			case OBJECT_SITE:
			{
				// ------- draw sqaure for outline ------//

				Site *sitePtr = site_array[displaySortPtr->object_recno];

				if( displaySortPtr->draw_flags & OBJECT_DRAW_POINTED || site_array.selected_recno == displaySortPtr->object_recno )
				{
					int animLineColorSeries = 2 + ( (site_array.selected_recno == displaySortPtr->object_recno)
						| displaySortPtr->draw_flags & OBJECT_DRAW_POINTED );
					short curX1 = displaySortPtr->x_loc * LOCATE_WIDTH;
					short curY1 = displaySortPtr->y_loc * LOCATE_HEIGHT;
					short curX2 = (sitePtr->map_x2+1) * LOCATE_WIDTH - 1;
					short curY2 = (sitePtr->map_y2+1) * LOCATE_HEIGHT - 1;
					short curZ  = sitePtr->altitude;;
					int rectX0 = ZOOM_X1 + calc_zoom_x( curX1, curY1, curZ );
					int rectY0 = ZOOM_Y1 + calc_zoom_y( curX1, curY1, curZ );
					int rectX1 = ZOOM_X1 + calc_zoom_x( curX2, curY1, curZ );
					int rectY1 = ZOOM_Y1 + calc_zoom_y( curX2, curY1, curZ );
					int rectX2 = ZOOM_X1 + calc_zoom_x( curX2, curY2, curZ );
					int rectY2 = ZOOM_Y1 + calc_zoom_y( curX2, curY2, curZ );
					int rectX3 = ZOOM_X1 + calc_zoom_x( curX1, curY2, curZ );
					int rectY3 = ZOOM_Y1 + calc_zoom_y( curX1, curY2, curZ );
					anim_line.draw_line( &vga_back, rectX0, rectY0, rectX1, rectY1, 1, animLineColorSeries);
					anim_line.draw_line( &vga_back, rectX1, rectY1, rectX2, rectY2, 1, animLineColorSeries);
					anim_line.draw_line( &vga_back, rectX2, rectY2, rectX3, rectY3, 1, animLineColorSeries);
					anim_line.draw_line( &vga_back, rectX3, rectY3, rectX0, rectY0, 1, animLineColorSeries);
				}
			/*	int i = site_array[displaySortPtr->object_recno]->animation_count;
				if (i <100)
				{
					site_array[displaySortPtr->object_recno]->animation_count --;
					if (i > 0)
						i = 10 - i;
					else
						i = 10 + i;
					if (i == 0)
							site_array[displaySortPtr->object_recno]->animation_count = 100;
					i = i << 4;
				}
				else
					i = 0;*/

				sitePtr->draw(	displaySortPtr->x_loc * LOCATE_WIDTH, displaySortPtr->y_loc * LOCATE_HEIGHT,
					sitePtr->altitude, displaySortPtr->draw_flags & OBJECT_DRAW_HIDDEN );
				break;
			}
		}	
	}
}
//----------- End of function ZoomMatrix::draw_objects_now ------------//


//---------- Begin of function ZoomMatrix::scroll -----------//
//
// <int> xScroll - horizontal scroll step (negative:left, positive:right)
// <int> yScroll - vertical scroll step   (negative:left, positive:right)
//
void ZoomMatrix::scroll(int xScroll, int yScroll)
{
	Matrix::scroll(xScroll,yScroll);

	world.map_matrix->cur_x_loc = top_x_loc;
	world.map_matrix->cur_y_loc = top_y_loc;
}
//----------- End of function ZoomMatrix::scroll ------------//


//------ Begin of function sort_display_function ------//
//
static int sort_display_function( const void *a, const void *b )
{
	return ((DisplaySort*)a)->object_y2 - ((DisplaySort*)b)->object_y2;
}
//------- End of function sort_display_function ------//


//------ Begin of function ZoomMatrix::put_bitmap_clip ---------//
//
// Put a bitmap on the surface buffer
//
// <int>   x, y 			  - the location of the bitmap, in the current screen coordination
// <char*> bitmapPtr 	  - bitmap ptr
// [int]   compressedFlag - whether the bitmap is compressed or not
//									 (default: 0)
//
void ZoomMatrix::put_bitmap_clip(int x, int y, char* bitmapPtr, int compressedFlag)
{
	int x2 = x + ((Bitmap *)bitmapPtr)->get_width() - 1;
	int y2 = y + ((Bitmap *)bitmapPtr)->get_height() - 1;

	if( x2 < ZOOM_X1 || y2 < ZOOM_Y1 || x > ZOOM_X2 || y > ZOOM_Y2 )
		return;

	//---- only portion of the sprite is inside the view area ------//

	if( x < ZOOM_X1 || x2 > ZOOM_X2 || y < ZOOM_Y1 || y2 > ZOOM_Y2 )
	{
		if( compressedFlag )
		{
			vga_back.put_bitmap_area_trans_decompress( x, y, bitmapPtr,
				max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y );
		}
		else
		{
			vga_back.put_bitmap_area_trans( x, y, bitmapPtr,
				max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y );
		}
	}

	//---- the whole sprite is inside the view area ------//

	else
	{
		if( compressedFlag )
			vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
		else
			vga_back.put_bitmap_trans( x, y, bitmapPtr );
	}
}
//--------- End of function ZoomMatrix::put_bitmap_clip ---------//


//------ Begin of function ZoomMatrix::detect_bitmap_clip ---------//
//
// Detect clicking on the bitmap.
//
// return: <int> 0 - not detected
//					  1 - left clicked
//					  2 - right clicked
//
int ZoomMatrix::detect_bitmap_clip(int x, int y, char* bitmapPtr)
{
	int x2 = x + *((short*)bitmapPtr) 	  - 1;
	int y2 = y + *(((short*)bitmapPtr)+1) - 1;

	if( x2 < ZOOM_X1 || y2 < ZOOM_Y1 || x > ZOOM_X2 || y > ZOOM_Y2 )
		return 0;

	//---- only portion of the sprite is inside the view area ------//

	// return mouse.single_click( max(ZOOM_X1,x), max(ZOOM_Y1,y), min(ZOOM_X2,x2), min(ZOOM_Y2,y2), 2 );
	return mouse.any_click( max(ZOOM_X1,x), max(ZOOM_Y1,y), min(ZOOM_X2,x2), min(ZOOM_Y2,y2), 0 ) ? 1 :
	mouse.any_click( max(ZOOM_X1,x), max(ZOOM_Y1,y), min(ZOOM_X2,x2), min(ZOOM_Y2,y2), 1 ) ? 2 : 0;
}
//--------- End of function ZoomMatrix::detect_bitmap_clip ---------//


//------ Begin of function ZoomMatrix::put_bitmap_remap_clip ---------//
//
// Put a bitmap on the surface buffer
//
// <int>   x, y 				- the location of the bitmap
// <char*> bitmapPtr 		- bitmap ptr
// [char*] colorRemapTable - color remap table
// [int]   compressedFlag  - whether the bitmap is compressed or not
//									  (default: 0)
//
void ZoomMatrix::put_bitmap_remap_clip(int x, int y, char* bitmapPtr, short* colorRemapTable, int compressedFlag)
{
	int x2 = x + *((short*)bitmapPtr) 	  - 1;
	int y2 = y + *(((short*)bitmapPtr)+1) - 1;

	if( x2 < ZOOM_X1 || y2 < ZOOM_Y1 || x > ZOOM_X2 || y > ZOOM_Y2 )
		return;

	//---- only portion of the sprite is inside the view arec ------//

	if( x < ZOOM_X1 || x2 > ZOOM_X2 || y < ZOOM_Y1 || y2 > ZOOM_Y2 )
	{
		if( compressedFlag )
		{
			if( colorRemapTable )
			{
				vga_back.put_bitmap_area_trans_remap_decompress( x, y, bitmapPtr,
					max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y, colorRemapTable );
			}
			else
			{
				vga_back.put_bitmap_area_trans_decompress( x, y, bitmapPtr,
					max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y );
			}
		}
		else
		{
			if( colorRemapTable )
			{
				vga_back.put_bitmap_area_trans_remap( x, y, bitmapPtr,
					max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y, colorRemapTable );
			}
			else
			{
				vga_back.put_bitmap_area_trans( x, y, bitmapPtr,
					max(ZOOM_X1,x)-x, max(ZOOM_Y1,y)-y, min(ZOOM_X2,x2)-x, min(ZOOM_Y2,y2)-y );
			}
		}
	}

	//---- the whole sprite is inside the view area ------//

	else
	{
		if( compressedFlag )
		{
			if( colorRemapTable )
				vga_back.put_bitmap_trans_remap_decompress( x, y, bitmapPtr, colorRemapTable );
			else
				vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
		}
		else
		{
			if( colorRemapTable )
				vga_back.put_bitmap_trans_remap( x, y, bitmapPtr, colorRemapTable );
			else
				vga_back.put_bitmap_trans( x, y, bitmapPtr );
		}
	}
}
//--------- End of function ZoomMatrix::put_bitmap_remap_clip ---------//


//------ Begin of function ZoomMatrix::calc_zoom_x ---------//
//
// return coordination relative to zoom window
//
int ZoomMatrix::calc_zoom_x(int curX, int curY, int curZ)
{
#if( LOCATE_WIDTH==LOCATE_HEIGHT && ZOOM_LOC_X_WIDTH==LOCATE_WIDTH && ZOOM_LOC_Y_WIDTH==-LOCATE_HEIGHT && ZOOM_Z_WIDTH==0 )

	// optimized for the current value

	return (curX - (top_x_loc<<LOCATE_WIDTH_SHIFT)) - (curY - (top_y_loc<<LOCATE_HEIGHT_SHIFT))
		- (ZOOM_LOC_X_WIDTH + ZOOM_LOC_Y_WIDTH)/2;

#else

	// general form

	return (((curX - top_x_loc*LOCATE_WIDTH) * ZOOM_LOC_X_WIDTH * LOCATE_HEIGHT
		+ (curY - top_y_loc*LOCATE_HEIGHT) * ZOOM_LOC_Y_WIDTH * LOCATE_WIDTH)
		/ (LOCATE_WIDTH *  LOCATE_HEIGTH))
		+ curZ * ZOOM_Z_WIDTH
		- (ZOOM_LOC_X_WIDTH + ZOOM_LOC_Y_WIDTH)/2;
	// last term is because top_x_loc, and top_y_loc points to the
	// center of a Location on the top left of the window
#endif
}
//------ End of function ZoomMatrix::calc_zoom_x ---------//



//------ Begin of function ZoomMatrix::calc_zoom_y ---------//
//
// return coordination relative to zoom window
//
int ZoomMatrix::calc_zoom_y(int curX, int curY, int curZ)
{
#if(LOCATE_WIDTH==LOCATE_HEIGHT && ZOOM_LOC_X_HEIGHT*2==LOCATE_WIDTH && ZOOM_LOC_Y_HEIGHT*2==LOCATE_HEIGHT && ZOOM_Z_HEIGHT==-1)

	// optimized for the current value

	return ((curX - (top_x_loc<<LOCATE_WIDTH_SHIFT) + curY - (top_y_loc<<LOCATE_HEIGHT_SHIFT)) >> 1)
		- curZ 
		- (ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT)/2;
#else

	// general form

	return ((( curX - top_x_loc*LOCATE_WIDTH) * ZOOM_LOC_X_HEIGHT * LOCATE_HEIGHT
		+ (curY - top_y_loc*LOCATE_HEIGHT) * ZOOM_LOC_Y_HEIGHT * LOCATE_WIDTH)
		/(LOCATE_WIDTH * LOCATE_HEIGHT) )
		+ curZ * ZOOM_Z_HEIGHT
		- (ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT)/2;
	// last term is because top_x_loc, and top_y_loc points to the
	// center of a Location on the top left of the window
#endif
}


//------ Begin of function ZoomMatrix::calc_abs_x ---------//
//
// return x coordination relative to the map, independent of scrolling
//
int ZoomMatrix::calc_abs_x(int curX, int curY, int curZ)
{
#if( LOCATE_WIDTH==LOCATE_HEIGHT && ZOOM_LOC_X_WIDTH==LOCATE_WIDTH && ZOOM_LOC_Y_WIDTH==-LOCATE_HEIGHT && ZOOM_Z_WIDTH==0 )

	// optimized for the current value

	return curX - curY;

#else

	// general form

	return ((curX * ZOOM_LOC_X_WIDTH * LOCATE_HEIGHT
		+ curY * ZOOM_LOC_Y_WIDTH * LOCATE_WIDTH)
		// / (LOCATE_WIDTH *  LOCATE_HEIGTH)
		>> (LOCATE_WIDTH_SHIFT + LOCATE_HEIGHT_SHIFT) )
		+ curZ * ZOOM_Z_WIDTH;
#endif
}
//------ End of function ZoomMatrix::calc_abs_x ---------//


//------ Begin of function ZoomMatrix::calc_abs_y ---------//
//
// return y coordination relative to the map, independent of scrolling
//
int ZoomMatrix::calc_abs_y(int curX, int curY, int curZ)
{
#if(LOCATE_WIDTH==LOCATE_HEIGHT && ZOOM_LOC_X_HEIGHT*2==LOCATE_WIDTH && ZOOM_LOC_Y_HEIGHT*2==LOCATE_HEIGHT && ZOOM_Z_HEIGHT==-1)

	// optimized for the current value

	return ((curX + curY) >> 1) - curZ;

#else
	
	// general form

	return ((curX * ZOOM_LOC_X_HEIGHT * LOCATE_HEIGHT
		+ curY * ZOOM_LOC_Y_HEIGHT * LOCATE_WIDTH)
		>> (LOCATE_WIDTH_SHIFT + LOCATE_HEIGHT_SHIFT) )
		+ curZ * ZOOM_Z_HEIGHT;
#endif
}
//------ End of function ZoomMatrix::calc_abs_y ---------//


//------ Begin of function ZoomMatrix::get_base_x ---------//
//
// return coordination relative to zoom window
//
int ZoomMatrix::get_base_x()
{
	return top_x_loc*ZOOM_LOC_X_WIDTH
		+ top_y_loc*ZOOM_LOC_Y_WIDTH
		+ (ZOOM_LOC_X_WIDTH + ZOOM_LOC_Y_WIDTH)/2;		
	// last term is because top_x_loc, and top_y_loc points to the
	// center of a Location
}
//------ End of function ZoomMatrix::get_base_x ---------//


//------ Begin of function ZoomMatrix::get_base_y ---------//
//
// return coordination relative to zoom window
//
int ZoomMatrix::get_base_y()
{
	return top_x_loc*ZOOM_LOC_X_HEIGHT
		+ top_y_loc*ZOOM_LOC_Y_HEIGHT
		+ (ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT)/2;
	// last term is because top_x_loc, and top_y_loc points to the
	// center of a Location
}
//------ End of function ZoomMatrix::get_base_y ---------//


//------ Begin of function ZoomMatrix::calc_cur_x ---------//
//
// reverse of calc_abs_x/y, curZ has to be known
//
int ZoomMatrix::calc_cur_x(int absX, int absY, int curZ)
{
	return ( (absX - curZ*ZOOM_Z_WIDTH)*(LOCATE_WIDTH*ZOOM_LOC_Y_HEIGHT) 
		- (absY - curZ*ZOOM_Z_HEIGHT)*(LOCATE_WIDTH*ZOOM_LOC_Y_WIDTH) )
		/ (ZOOM_LOC_X_WIDTH*ZOOM_LOC_Y_HEIGHT - ZOOM_LOC_X_HEIGHT*ZOOM_LOC_Y_WIDTH);
}
//------ End of function ZoomMatrix::calc_cur_x ---------//


//------ Begin of function ZoomMatrix::calc_cur_y ---------//
//
// reverse of calc_abs_x/y, curZ has to be known
//
int ZoomMatrix::calc_cur_y(int absX, int absY, int curZ)
{
	return ( (absY - curZ*ZOOM_Z_HEIGHT)*(LOCATE_HEIGHT*ZOOM_LOC_X_WIDTH) 
		- (absX - curZ*ZOOM_Z_WIDTH)*(LOCATE_HEIGHT*ZOOM_LOC_X_HEIGHT) )
		/ (ZOOM_LOC_Y_HEIGHT*ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH*ZOOM_LOC_X_HEIGHT);
}
//------ End of function ZoomMatrix::calc_cur_x ---------//


//----------- Begin of function ZoomMatrix::get_detect_location ----------//
//
// find the location of the given screen coordinate
// return 1 if success
//
int ZoomMatrix::get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{

	if( scrnX < image_x1 || scrnX > image_x2 || scrnY < image_y1 || scrnY > image_y2)
		return 0;

	int zoomX = scrnX - image_x1;
	int zoomY = scrnY - image_y1;
	int absX = zoomX + get_base_x();
	int absY = zoomY + get_base_y();

	// find two locus of locations locAX, locAY, and locBX locBY
	// such that curX and curY of (locAX, locAY) is just on the left of the absX
	// and curX and curY of (locBX, locBY) is just on the right of the absX

	// the locus in parametric form is (locAX+N, locAY+N)
	// and (locBY+N, locBY+N)

	int locAX, locAY;
	int locBX, locBY;

	if( absX >= 0 )
	{
		// initial locAY and locBY are zero
		locAY = locBY = 0;
		locAX = absX / ZOOM_LOC_X_WIDTH;		// floor
		locBX = (absX + ZOOM_LOC_X_WIDTH-1)/ ZOOM_LOC_X_WIDTH;	// ceil
	}
	else
	{
		// initial locAX and locBX are zero
		locAX = locBX = 0;
		locAY = (absX + ZOOM_LOC_Y_WIDTH+1) / ZOOM_LOC_Y_WIDTH;
		locBY = absX / ZOOM_LOC_Y_WIDTH;
	}

	// find the inital N for (locAX,locAY) and (locBX,locBY)
	// assuming all location at height zero
	if( absY >= 0 )
	{
		int nA, nB;

		nA = nB = absY / (ZOOM_LOC_X_HEIGHT+ZOOM_LOC_Y_HEIGHT);
		nA -= (locAX + locAY);
		if( nA >= 0 )
		{
			if( locAX + nA >= max_x_loc )
				nA = max_x_loc - 1 - locAX;
			if( locAY + nA >= max_y_loc )
				nA = max_y_loc - 1 - locAY;
			locAX += nA;
			locAY += nA;
		}

		nB -= (locBX + locBY);
		if( nB >= 0 )
		{
			if( locBX + nB >= max_x_loc )
				nB = max_x_loc - 1 - locBX;
			if( locBY + nB >= max_y_loc )
				nB = max_y_loc - 1 - locBY;
			locBX += nB;
			locBY += nB;
		}
	}
	// if absY < 0, no need to reduce locAX, locAY, locBX, locBY,
	// because N cannot be smaller than zero, otherwise out of location boundary

	int curAX = locAX * LOCATE_WIDTH;
	int curAXNext = curAX + LOCATE_WIDTH;
	int curAY = locAY * LOCATE_WIDTH;
	int curAYNext = curAY + LOCATE_WIDTH;
	int curBX = locBX * LOCATE_WIDTH;
	int curBXNext = curBX + LOCATE_WIDTH;
	int curBY = locBY * LOCATE_WIDTH;
	int curBYNext = curBY + LOCATE_WIDTH;

	do
	{
		if( locAX < max_x_loc && locAY < max_y_loc )
		{
			LocationCorners lc;
			get_loc_corner( locAX, locAY, &lc );

			POINT vertex[4];

			err_when(curAX != locAX*LOCATE_WIDTH);
			err_when(curAY != locAY*LOCATE_WIDTH);

			int curZ;
			vertex[0].x = calc_abs_x( curAX, curAY, (curZ=lc.top_left->get_altitude()) );
			vertex[0].y = calc_abs_y( curAX, curAY, curZ );
			vertex[1].x = calc_abs_x( curAXNext, curAY, (curZ=lc.top_right->get_altitude()) );
			vertex[1].y = calc_abs_y( curAXNext, curAY, curZ );
			vertex[3].x = calc_abs_x( curAX, curAYNext, (curZ=lc.bottom_left->get_altitude()) );
			vertex[3].y = calc_abs_y( curAX, curAYNext, curZ );
			vertex[2].x = calc_abs_x( curAXNext, curAYNext, (curZ=lc.bottom_right->get_altitude()) );
			vertex[2].y = calc_abs_y( curAXNext, curAYNext, curZ );

			int minY = vertex[0].y;
			if( minY > vertex[1].y)
				minY = vertex[1].y;
			if( minY > vertex[2].y)
				minY = vertex[2].y;
			if( minY > vertex[3].y)
				minY = vertex[3].y;

			int maxY = vertex[2].y;
			if( maxY < vertex[0].y)
				maxY = vertex[0].y;
			if( maxY < vertex[1].y)
				maxY = vertex[1].y;
			if( maxY < vertex[3].y)
				maxY = vertex[3].y;

			if( absY < minY )
			{
				// no need to scan this locus any more
				locAX = max_x_loc;
				locAY = max_y_loc;
			}
			else
			{
				if( absY <= maxY && test_inside_polygon(absX, absY, vertex, 4) )
				{
					*locX = locAX;
					*locY = locAY;
					if(mobileType)
					{
						if( lc.loc_ptr->unit_recno(UNIT_AIR) )
							*mobileType = UNIT_AIR;
						else
							*mobileType = UNIT_LAND;
					}

					err_when( *locX < 0 || *locX > max_x_loc || *locY < 0 || *locY > max_y_loc );
					return 1;
				}
				curAX += LOCATE_WIDTH;
				curAXNext += LOCATE_WIDTH;
				curAY += LOCATE_HEIGHT;
				curAYNext += LOCATE_HEIGHT;
				++locAX;
				++locAY;
			}
		}

		if( locBX < max_x_loc && locBY < max_y_loc )
		{
			LocationCorners lc;
			get_loc_corner( locBX, locBY, &lc );

			POINT vertex[4];

			err_when(curBX != locBX*LOCATE_WIDTH);
			err_when(curBY != locBY*LOCATE_WIDTH);

			int curZ;
			vertex[0].x = calc_abs_x( curBX, curBY, (curZ=lc.top_left->get_altitude()) );
			vertex[0].y = calc_abs_y( curBX, curBY, curZ );
			vertex[1].x = calc_abs_x( curBXNext, curBY, (curZ=lc.top_right->get_altitude()) );
			vertex[1].y = calc_abs_y( curBXNext, curBY, curZ );
			vertex[3].x = calc_abs_x( curBX, curBYNext, (curZ=lc.bottom_left->get_altitude()) );
			vertex[3].y = calc_abs_y( curBX, curBYNext, curZ );
			vertex[2].x = calc_abs_x( curBXNext, curBYNext, (curZ=lc.bottom_right->get_altitude()) );
			vertex[2].y = calc_abs_y( curBXNext, curBYNext, curZ );

			int minY = vertex[0].y;
			if( minY > vertex[1].y)
				minY = vertex[1].y;
			if( minY > vertex[2].y)
				minY = vertex[2].y;
			if( minY > vertex[3].y)
				minY = vertex[3].y;

			int maxY = vertex[3].y;
			if( maxY < vertex[0].y)
				maxY = vertex[0].y;
			if( maxY < vertex[1].y)
				maxY = vertex[1].y;
			if( maxY < vertex[2].y)
				maxY = vertex[2].y;

			if( absY < minY )
			{
				// no need to scan this locus any more
				locBX = max_x_loc;
				locBY = max_y_loc;
			}
			else
			{
				if( absY <= maxY && test_inside_polygon(absX, absY, vertex, 4) )
				{
					*locX = locBX;
					*locY = locBY;
					if(mobileType)
					{
						if( lc.loc_ptr->unit_recno(UNIT_AIR) )
							*mobileType = UNIT_AIR;
						else
							*mobileType = UNIT_LAND;
					}
					err_when( *locX < 0 || *locX > max_x_loc || *locY < 0 || *locY > max_y_loc );
					return 1;
				}
				curBX += LOCATE_WIDTH;
				curBXNext += LOCATE_WIDTH;
				curBY += LOCATE_HEIGHT;
				curBYNext += LOCATE_HEIGHT;
				++locBX;
				++locBY;
			}
		}

	} while( locAX < max_x_loc && locAY < max_y_loc
		|| locBX < max_x_loc && locBY < max_y_loc );


	return 0;
}
//----------- End of function ZoomMatrix::get_detect_location ----------//


//----------- Begin of function ZoomMatrix::get_close_location ----------//
//
// find the location of the given screen coordinate
// given get_detect_location fails, it assumes the point is outside the map
// return 1 if success
//
int ZoomMatrix::get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{
	if( scrnX < image_x1 || scrnX > image_x2 || scrnY < image_y1 || scrnY > image_y2)
		return 0;

	int zoomX = scrnX - image_x1;
	int zoomY = scrnY - image_y1;
	int absX = zoomX + get_base_x();
	int absY = zoomY + get_base_y();

	int lowX = 0;
	int lowY = 0;
	int highX = max_x_loc-1;
	int highY = max_y_loc-1;

	if( absX >= 0 )
	{
		// top or right edge

		// get abs_y of top right corner
		int absYTopRight = calc_abs_y(max_x_loc*LOCATE_WIDTH, 0, get_corner(max_x_loc,0)->get_altitude() );

		if( absY < absYTopRight )
		{
			// top edge
			lowX = 0;
			highX = max_x_loc-1;
			lowY = highY = 0;
		}
		else
		{
			// right edge
			lowY = 0;
			highY = max_y_loc-1;
			lowX = highX = max_x_loc-1;
		}
	}
	else
	{
		// bottom or left edge

		// get abs_y of bottom left corner
		int absYBottomLeft = calc_abs_y(0, max_y_loc*LOCATE_HEIGHT, get_corner(0, max_y_loc)->get_altitude() );

		if( absY >= absYBottomLeft )
		{
			// bottom edge
			lowX = 0;
			highX = max_x_loc-1;
			lowY = highY = max_y_loc-1;
		}
		else
		{
			// left edge
			lowY = 0;
			highY = max_y_loc-1;
			lowX = highX = 0;
		}
	}

	// use absX to divide lowX,highX and lowY,highY
	// so that the 'close' location is respect of screen x coordinate
	int lowAbsX = calc_abs_x(lowX*LOCATE_WIDTH, lowY*LOCATE_HEIGHT, get_corner(lowX, lowY)->get_altitude() );
	int highAbsX = calc_abs_x((highX+1)*LOCATE_WIDTH, (highY+1)*LOCATE_HEIGHT, get_corner(highX+1, highY+1)->get_altitude() );
	int estLocX = ((highAbsX - absX) * lowX + (absX - lowAbsX) * highX) / (highAbsX - lowAbsX);
	int estLocY = ((highAbsX - absX) * lowY + (absX - lowAbsX) * highY) / (highAbsX - lowAbsX);
	if( estLocX < lowX )
		estLocX = lowX;
	if( estLocX > highX )
		estLocX = highX;
	if( estLocY < lowY )
		estLocY = lowY;
	if( estLocY > highY )
		estLocY = highY;
	*locX = estLocX;
	*locY = estLocY;
	if( get_loc(*locX, *locY)->unit_recno(UNIT_AIR) )
		*mobileType = UNIT_AIR;
	else
		*mobileType = UNIT_LAND;
	return 1;
}
//----------- End of function ZoomMatrix::get_close_location ----------//



//----------- Begin of function ZoomMatrix::put_bitmap_offset ----------//
//
// put 8-bit bitmap on the window
//
// <int> curX, curY, curZ     pixel coorinate relative to map
// <char *>bitmapPtr          bitmap
// <int> offsetX, offsetY     offset in screen coorindate
// [short *]colorRemapTable	color remap table (default:NULL)
// [int] mirror               need mirroring
// [int] compressed           0=uncompressed, 1=compressed, 2=uncompressed+alpha, bit 2= translucent
//
void ZoomMatrix::put_bitmap_offset(int curX, int curY, int curZ,
	char *bitmapPtr, int offsetX, int offsetY,
	short *colorRemapTable, int mirror, int compressed)
{
	short bitmapWidth = ((Bitmap *)bitmapPtr)->get_width();
	short bitmapHeight = ((Bitmap *)bitmapPtr)->get_height();
	int x1 = calc_zoom_x(curX, curY, curZ) + offsetX;
	int y1 = calc_zoom_y(curX, curY, curZ) + offsetY;
	int x2 = x1 + bitmapWidth - 1;
	int y2 = y1 + bitmapHeight - 1;

	// ------------- test if the whole bitmap outside window ---------//

	if( x1 >= image_width || x2 < 0 || y1 >= image_height || y2 < 0)
		return;

	// ------------- test if whole bitmap inside window ----------//

	if( x1 >= 0 && x2 < image_width && y1 >= 0 && y2 < image_height)
	{

		// ------- convert to screen coordinate -------//
		x1 += image_x1;
		y1 += image_y1;
#ifdef DEBUG
		x2 += image_x1;		// x2 and y2 are not used, but adjusted for debugging
		y2 += image_y1;
#endif

		switch(compressed)
		{
		case 0:		// uncompressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_trans_remap(x1, y1, bitmapPtr, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans_hmirror(x1, y1, bitmapPtr);
				else
					// vga_back.put_bitmap_trans_remap_hmirror(x1, y1, bitmapPtr, colorRemapTable);
					vga_back.put_bitmap_trans_remap_decompress_hmirror(x1, y1, bitmapPtr, colorRemapTable);
			}
			break;
		case 1:		// compressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans_decompress(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_trans_remap_decompress(x1, y1, bitmapPtr, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans_decompress_hmirror(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_trans_remap_decompress_hmirror(x1, y1, bitmapPtr, colorRemapTable);
			}
			break;
		case 2:		// uncompressed, blending bitmap
			// ignore coloRemapTable
			if( !mirror )
			{
				vga_back.put_bitmap_blend(x1, y1, bitmapPtr);
			}
			else
			{
				vga_back.put_bitmap_blend_hmirror(x1, y1, bitmapPtr);
			}
			break;
		case 0 | 4:		// uncompressed, translucent, not supported
			err_here();
			break;
		case 1 | 4:		// compressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_half_decompress(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_half_remap_decompress(x1, y1, bitmapPtr, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_half_decompress_hmirror(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_half_remap_decompress_hmirror(x1, y1, bitmapPtr, colorRemapTable);
			}
			break;
		case 2 | 4:		// uncompressed, blending bitmap
			// ignore coloRemapTable
			if( !mirror )
			{
				vga_back.put_bitmap_weak_blend(x1, y1, bitmapPtr);
			}
			else
			{
				vga_back.put_bitmap_weak_blend_hmirror(x1, y1, bitmapPtr);
			}
			break;
		default:
			err_here();
		}
	}
	else
	{
		// ------- determine clip window ----------//
		int srcX1 = x1 >= 0 ? 0 : -x1;
		int srcY1 = y1 >= 0 ? 0 : -y1;
		int srcX2 = x2 < image_width ? x2 - x1 : image_width - x1 - 1;
		int srcY2 = y2 < image_height ? y2 - y1 : image_height - y1 - 1;

		// ------- convert to screen coordinate -------//
		x1 += image_x1;
		y1 += image_y1;
#ifdef DEBUG
		x2 += image_x1;		// x2 and y2 are not used, but adjusted for debugging
		y2 += image_y1;
#endif

		switch(compressed)
		{
		case 0:		// uncompressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			break;
		case 1:		// compressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			break;
		case 2:		// uncompressed, blending
			// ignore coloRemapTable
			if( !mirror )
			{
				vga_back.put_bitmap_blend_area(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			else
			{
				vga_back.put_bitmap_blend_area_hmirror(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			break;
		case 0 | 4:		// uncompressed not supported
			err_here();
			break;
		case 1 | 4:		// compressed
			if( !mirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_half_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_half_remap_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_half_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_half_remap_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			break;
		case 2 | 4:		// uncompressed, blending
			// ignore coloRemapTable
			if( !mirror )
			{
				vga_back.put_bitmap_weak_blend_area(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			else
			{
				vga_back.put_bitmap_weak_blend_area_hmirror(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			break;
		default:
			err_here();
		}
	}
}
//----------- End of function ZoomMatrix::put_bitmap_offset ----------//


//----------- Begin of function ZoomMatrix::put_bitmapW_offset ----------//
//
// put 16-bit bitmap on the window
//
// <int> curX, curY, curZ     pixel coorinate relative to map
// <char *>bitmapPtr          bitmap
// <int> offsetX, offsetY     offset in screen coorindate
// [int] mirror               need mirroring
// [int] compressed           0=uncompressed, bit3=black mask

//
void ZoomMatrix::put_bitmapW_offset(int curX, int curY, int curZ,
	short *bitmapPtr, int offsetX, int offsetY,
	int mirror, int compressed)
{
	short bitmapWidth = ((BitmapW *)bitmapPtr)->get_width();
	short bitmapHeight = ((BitmapW *)bitmapPtr)->get_height();
	int x1 = calc_zoom_x(curX, curY, curZ) + offsetX;
	int y1 = calc_zoom_y(curX, curY, curZ) + offsetY;
	int x2 = x1 + bitmapWidth - 1;
	int y2 = y1 + bitmapHeight - 1;

	// ------------- test if the whole bitmap outside window ---------//

	if( x1 >= image_width || x2 < 0 || y1 >= image_height || y2 < 0)
		return;

	// ------------- test if whole bitmap inside window ----------//

	if( x1 >= 0 && x2 < image_width && y1 >= 0 && y2 < image_height)
	{

		// ------- convert to screen coordinate -------//
		x1 += image_x1;
		y1 += image_y1;
#ifdef DEBUG
		x2 += image_x1;		// x2 and y2 are not used, but adjusted for debugging
		y2 += image_y1;
#endif

		switch(compressed)
		{
		case 0:		// uncompressed
			if( !mirror )
			{
				vga_back.put_bitmapW_trans(x1, y1, bitmapPtr);
			}
			else
			{
				err_here();
				// vga_back.put_bitmapW_trans_hmirror(x1, y1, bitmapPtr);
			}
			break;

		case 0 | 8:		// uncompressed, draw black mask
			if( !mirror )
			{
				vga_back.put_bitmapW_trans_blacken( x1, y1, bitmapPtr);
			}
			else
			{
				err_here();
			}
			break;
 
		default:
			err_here();
		}
	}
	else
	{
		// ------- determine clip window ----------//
		int srcX1 = x1 >= 0 ? 0 : -x1;
		int srcY1 = y1 >= 0 ? 0 : -y1;
		int srcX2 = x2 < image_width ? x2 - x1 : image_width - x1 - 1;
		int srcY2 = y2 < image_height ? y2 - y1 : image_height - y1 - 1;

		// ------- convert to screen coordinate -------//
		x1 += image_x1;
		y1 += image_y1;
#ifdef DEBUG
		x2 += image_x1;		// x2 and y2 are not used, but adjusted for debugging
		y2 += image_y1;
#endif

		switch(compressed)
		{
		case 0:		// uncompressed
			if( !mirror )
			{
				vga_back.put_bitmapW_area_trans(x1, y1, bitmapPtr,
					srcX1, srcY1, srcX2, srcY2);
			}
			else
			{
				err_here();
				//vga_back.put_bitmapW_area_trans_hmirror(x1, y1, bitmapPtr,
				//	srcX1, srcY1, srcX2, srcY2);
			}
			break;

		case 0 | 8:		// uncompressed, draw black mask
			if( !mirror )
			{
				vga_back.put_bitmapW_area_trans_blacken( x1, y1, bitmapPtr,
					srcX1, srcY1, srcX2, srcY2);
			}
			else
			{
				err_here();
			}
			break;

		default:
			err_here();
		}
	}
}
//----------- End of function ZoomMatrix::put_bitmapW_offset ----------//


/*
static void draw_cubic_plane(int x, int y, LocationCorners lc, UCHAR color)
{
	x -= ZOOM_Z_WIDTH * lc.top_left->altitude;
	y -= ZOOM_Z_HEIGHT * lc.top_left->altitude;

	short &z0 = lc.top_left->altitude;
	short &z1 = lc.top_right->altitude;
	short &z2 = lc.bottom_left->altitude;
	short &z3 = lc.bottom_right->altitude;
	short &duz0w = lc.top_left->gradient_x;		// du/dz * LOCATE_WIDTH
	short &dvz0h = lc.top_left->gradient_y;
	short &duz1w = lc.top_right->gradient_x;
	short &dvz1h = lc.top_right->gradient_y;
	short &duz2w = lc.bottom_left->gradient_x;
	short &dvz2h = lc.bottom_left->gradient_y;
	short &duz3w = lc.bottom_right->gradient_x;
	short &dvz3h = lc.bottom_right->gradient_y;

	// polynomial coefficient
	// z = c00 + c10*u + c01*v + c20*u^2 + c11*u*v + c02*v^2
	//     + c30*u^3 + c21*u^2*v + c12*u*v^2 + c03*v^3
	// all coefficient is multiplied by 0x10000
	long c00 = z0 * C_MULTIPLIER;
	long c10 = duz0w * (C_MULTIPLIER / LOCATE_WIDTH);
	long c01 = dvz0h * (C_MULTIPLIER / LOCATE_HEIGHT);
	long c20 = ( 3 * (z1-z0) - duz1w - duz0w - duz0w ) * (C_MULTIPLIER / LOCATE_WIDTH / LOCATE_WIDTH);
	long c02 = ( 3 * (z2-z0) - dvz2h - dvz0h - dvz0h ) * (C_MULTIPLIER / LOCATE_WIDTH / LOCATE_WIDTH);
	long c30 = (duz1w + duz0w - (z1-z0) - (z1-z0)) * (C_MULTIPLIER / LOCATE_WIDTH / LOCATE_WIDTH / LOCATE_WIDTH);
	long c21 = (duz3w + duz0w - duz1w - duz2w) * (C_MULTIPLIER / 2 / LOCATE_WIDTH / LOCATE_WIDTH / LOCATE_HEIGHT);
	long c12 = (dvz3h + dvz0h - dvz1h - dvz2h) * (C_MULTIPLIER / 2 / LOCATE_WIDTH / LOCATE_HEIGHT / LOCATE_HEIGHT);
	long c03 = (dvz2h + dvz0h - (z2-z0) - (z2-z0)) * (C_MULTIPLIER / LOCATE_HEIGHT / LOCATE_HEIGHT / LOCATE_HEIGHT);
	long c11 = ( z3 + z0 - z1 - z2 ) * (C_MULTIPLIER / LOCATE_WIDTH / LOCATE_HEIGHT)
		- (c21 * LOCATE_WIDTH + c12 * LOCATE_HEIGHT);

	const lineCountPeriod = 4;
	int lineCount;
	int lineCountMod4 = 0;
	const UVMULTIPLIER = 4;
	long rowU, v;
	static long incU[lineCountPeriod] = { -2,  6, -2, -2 };
	// static long incV[lineCountPeriod] = {  2,  2,  2,  2 };
	int rowX = x;
	int rowY = y;
	static int incX[lineCountPeriod] = { -1,  1, -1, -1 };
	static int incY[lineCountPeriod] = {  0,  1,  0,  0 };

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// rowU initiall is 3/4, v is 1/4
	for( rowU = 3, v = 1; v < LOCATE_HEIGHT * UVMULTIPLIER; 
		rowU+=incU[lineCountMod4], v += 2,
		rowX+=incX[lineCountMod4], rowY+=incY[lineCountMod4],
		++lineCount, lineCountMod4 = lineCountMod4 < lineCountPeriod-1 ? lineCountMod4+1 : 0)
	{
		long u = rowU;
		int x = rowX;		// local x, y
		int y = rowY;

		long z =
			((c30*u + c20*UVMULTIPLIER)*u + c10*UVMULTIPLIER*UVMULTIPLIER) * u / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) + 
			((c03*v + c02*UVMULTIPLIER)*v + c01*UVMULTIPLIER*UVMULTIPLIER) * v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) +
			((c21*u + c12*v) + c11*UVMULTIPLIER)*u*v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER)
			+ c00;
		// du1z = 3*c30*u^2 + 2*c21*u*v + c22*v^2 + 2*c20*u + c11*v + c10
		long du1zM2 =		// dz/du at (u,v) multiplied by 2 * C_MULTIPLIER
			(((c30*u*3 + c20*2*UVMULTIPLIER ) * u +
			(c21*u*2 + c12*v + c11*UVMULTIPLIER) * v )
			/ ( UVMULTIPLIER * UVMULTIPLIER)
			+ c10) * 2;
		// du2z = 6*c30*u + 2*c21*v + 2*c20
		long du2zM2 =         // d2z/du2 at (u,v) multiplied by 2 * C_MULTIPLIER
			((c30*u*3 + c21*v) / UVMULTIPLIER + c20 ) * 4;
		// du3z = 6*c30
		long du3zM4D3 =         // d3z/du3 at (u,v) * 4/3 * C_MULTIPLIER
			c30 * 8;

		for( ; u < LOCATE_WIDTH * UVMULTIPLIER; 
			u += 2 * UVMULTIPLIER, x += 2, ++y)
		{
			vga_back.draw_pixel(x, y- z / C_MULTIPLIER, color);

			// update z, du1z, du2z of u+2, using taylor expansion
			// must update z, du1z then du3z from low to high derivative
			// z += du1z +2 * du1z + 2 * du2z + 4 * du3z /3;
			z += du1zM2 + du2zM2 + du3zM4D3;
			// du1z += 2 * du2z + 2 * du3z;
			du1zM2 += du2zM2 * 2 + du3zM4D3 * 3;
			// du2z += 2 * du3z
			du2zM2 += du3zM4D3 * 3;
			// du3z remains constant
		}
	}
}



*/


static void draw_cubic_plane(int zoomX, int zoomY, LocationCorners lc, UCHAR color)
{
	zoomX -= ZOOM_Z_WIDTH * lc.top_left->altitude;
	zoomY -= ZOOM_Z_HEIGHT * lc.top_left->altitude;

	long c00 = lc.loc_ptr->c00;
	long c10 = lc.loc_ptr->c10;
	long c01 = lc.loc_ptr->c01;
	long c20 = lc.loc_ptr->c20;
	long c11 = lc.loc_ptr->c11;
	long c02 = lc.loc_ptr->c02;
	long c30 = lc.loc_ptr->c30;
	long c21 = lc.loc_ptr->c21;
	long c12 = lc.loc_ptr->c12;
	long c03 = lc.loc_ptr->c03;

	const int lineCountPeriod = 4;
	// int lineCount;
	int lineCountMod4 = 0;
#define UVMULTIPLIER 4
#define UVMULTIPLIER_SHIFT 2
	long rowU, v;
	static long incU[lineCountPeriod] = { -2,  6, -2, -2 };
	// static long incV[lineCountPeriod] = {  2,  2,  2,  2 };
	int rowX = zoomX + ZOOM_X1;
	int rowY = zoomY + ZOOM_Y1;
	static int incX[lineCountPeriod] = { -1,  1, -1, -1 };
	static int incY[lineCountPeriod] = {  0,  1,  0,  0 };

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// variable for inner loop
	long u;
	int x,y;
	long z, du1zM2, du2zM2, du3zM4D3;

	short *bufPtr = vga_back.buf_ptr();
	long	bufPitch = vga_back.buf_pitch();


#if(0)
// #ifdef DEBUG
	// for verification
	u = 3;
	v = 1;

	long vz =
		((c30*u + c20*UVMULTIPLIER)*u + c10*UVMULTIPLIER*UVMULTIPLIER) * u / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) + 
		((c03*v + c02*UVMULTIPLIER)*v + c01*UVMULTIPLIER*UVMULTIPLIER) * v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) +
		((c21*u + c12*v) + c11*UVMULTIPLIER)*u*v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER)
		+ c00;
	// du1z = 3*c30*u^2 + 2*c21*u*v + c22*v^2 + 2*c20*u + c11*v + c10
	long vdu1zM2 =		// dz/du at (u,v) multiplied by 2 * C_MULTIPLIER
		(((c30*u*3 + c20*2*UVMULTIPLIER ) * u +
		(c21*u*2 + c12*v + c11*UVMULTIPLIER) * v )
		/ ( UVMULTIPLIER * UVMULTIPLIER)
		+ c10) * 2;
	// du2z = 6*c30*u + 2*c21*v + 2*c20
	long vdu2zM2 =         // d2z/du2 at (u,v) multiplied by 2 * C_MULTIPLIER
		((c30*u*3 + c21*v) / UVMULTIPLIER + c20 ) * 4;
	// du3z = 6*c30
	long vdu3zM4D3 =         // d3z/du3 at (u,v) * 4/3 * C_MULTIPLIER
		c30 * 8;
#endif

#ifdef ASM_FOR_MSVC
	_asm
	{
		// init outer loop
		mov	rowU, 3			// 3/4
		mov	v, 1				// 1/4
draw_cubic_plane_1:

		// init inner loop
		mov	ecx, rowU		// u = rowU
		mov	u, ecx
		mov	ebx, v
		mov	eax, rowX		// x = rowX
		mov	x, eax
		mov	edx, rowY		// y = rowY
		mov	y, edx

		//long z =
		//	((c30*u + c20*UVMULTIPLIER)*u + c10*UVMULTIPLIER*UVMULTIPLIER) * u / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) + 
		//	((c03*v + c02*UVMULTIPLIER)*v + c01*UVMULTIPLIER*UVMULTIPLIER) * v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER) +
		//	((c21*u + c12*v) + c11*UVMULTIPLIER)*u*v / (UVMULTIPLIER*UVMULTIPLIER*UVMULTIPLIER)
		//	+ c00;

		mov	eax, c30
		imul	ecx				; eax = c30 * u
		mov	edx, c20
		lea	eax, [eax + edx * UVMULTIPLIER]	; eax = c30*u + c20 * 4
		imul	ecx				; eax = (c30*u+c20*4) * u
		mov	edx, c10
		sal	edx, 2 * UVMULTIPLIER_SHIFT
		add	eax, edx			; eax = (c30*u+c20*4)*u + c10*16
		imul	ecx				; eax = ((c30*u+c20*4)*u+c10*16) * u
		sar	eax, 3 * UVMULTIPLIER_SHIFT	; / (UVMULTIPLIER^3)
		mov	edi, eax

		mov	eax, c03
		imul	ebx				; eax = c30 * u
		mov	edx, c02
		lea	eax, [eax + edx * UVMULTIPLIER]		; eax = c03*v + c02 * 4
		imul	ebx				; eax = (c03*v+c02*4) * v
		mov	edx, c01
		sal	edx, 2 * UVMULTIPLIER_SHIFT
		add	eax, edx			; eax = (c03*v+c02*4)*v + c01*16
		imul	ebx				; eax = ((c03*v+c02*4)*v+c01*16) * v
		sar	eax, 3 * UVMULTIPLIER_SHIFT	; / (UVMULTIPLIER^3)
		add	edi, eax

		push	edi
		mov	eax, c21
		imul	ecx
		mov	edi, eax
		mov	eax, c12
		imul	ebx
		add	eax, edi			; eax = (c21*u + c12*v)
		mov	edx, c11
		lea	eax, [eax + edx * UVMULTIPLIER]	; eax = (c21*u+c12*v) + c11 * 4
		imul	ecx
		pop	edi
		imul	ebx
		sar	eax, 3 * UVMULTIPLIER_SHIFT		; /(UVMULTIPLIER^3)
		add	edi, eax
		add	edi, c00
		
		mov	z, edi

		// long du1zM2 =		// dz/du at (u,v) multiplied by 2 * C_MULTIPLIER
		// (((c30*u*3 + c20*2*UVMULTIPLIER ) * u +
		// (c21*u*2 + c12*v + c11*UVMULTIPLIER) * v )
		// / ( UVMULTIPLIER * UVMULTIPLIER)
		// + c10) * 2;

		lea	eax, [ecx + 2*ecx]
		imul	c30
		mov	edx, c20
		lea	eax, [eax + edx * (2 * UVMULTIPLIER)]		; eax = (c30*u*3 + c20*2*4)
		imul	ecx
		mov	edi, eax

		push	edi
		lea	eax, [ecx+ecx]
		imul	c21			; eax = c21*u*2
		mov	edi, eax
		mov	eax, c12
		imul	ebx
		add	eax, edi		; eax = c21*u*2 + c12*v
		mov	edx, c11
		lea	eax, [eax + edx * UVMULTIPLIER]		; eax = c21*u*2+c12*v + c11*4
		pop	edi
		imul	eax, ebx		; eax = (c21*u*2+c12*v+c11*UVMULTIPLIER)*v

		add	edi, eax
		sar	edi, 2 * UVMULTIPLIER_SHIFT		; /(UVMULTIPLIER^2)
		add	edi, c10
		add	edi, edi
		mov	du1zM2, edi

		// long du2zM2 =         // d2z/du2 at (u,v) multiplied by 2 * C_MULTIPLIER
		// ((c30*u*3 + c21*v) / UVMULTIPLIER + c20 ) * 4;
		lea	eax, [ecx+ecx*2]
		imul	c30
		mov	edi, eax
		mov	eax, c21
		imul	ebx
		add	edi, eax
		sar	edi, UVMULTIPLIER_SHIFT		; / UVMULTIPLIER
		add	edi, c20
		lea	edi, [edi*4]
		mov	du2zM2, edi

		// long du3zM4D3 =         // d3z/du3 at (u,v) * 4/3 * C_MULTIPLIER
		//		c30 * 8;
		mov	eax, c30
		sal	eax, 3					; eax = c30 * 8
		mov	du3zM4D3,eax

		// edi is now bufPtr + x
		// cache z in ebx
		mov	edi, bufPtr
		add	edi, x
		mov	ebx, z

draw_cubic_plane_2:
		mov	edx, ebx
		mov	eax, y
		sar	edx, C_MULTIPLIER_SHIFT
		mov	ecx, bufPitch
		sub	eax, edx
		imul	ecx			; eax = (y - z / C_MULTIPLIER) * bufPitch
		mov	cl, color
		mov	[edi+eax], cl

		// inner loop increment
		//	z += du1zM2 + du2zM2 + du3zM4D3;
		// du1zM2 += du2zM2 * 2 + du3zM4D3 * 3;
		// du2zM2 += du3zM4D3 * 3;
		mov	ecx, du3zM4D3
		mov	edx, du2zM2
		add	ebx, ecx
		mov	eax, du1zM2
		add	ebx, edx
		lea	ecx, [ecx + 2 * ecx]		// ecx = du3zM4D3 * 3
		add	ebx, eax			// ebx(z) += du1zM2 + du2zM2 + du3zM4D3

		lea	eax, [eax + 2 * edx]
		add	edx, ecx			// edx (du2zM2) += du3zM4D3 * 3
		add	eax, ecx			// eax (du1zM2) += du2zM2*2 + du3zM4D3*3
		mov	du2zM2, edx
		mov	du1zM2, eax

		add	u, 2 * UVMULTIPLIER		; u += 2 * UVMULTIPLIER
		add	edi, 2							; x += 2
		inc	y

		// inner loop comparison
		cmp	u, LOCATE_WIDTH * UVMULTIPLIER
		jb		draw_cubic_plane_2

		// outer loop increment
		mov	eax, lineCountMod4
		mov	ebx, incU[eax * 4]
		add	rowU, ebx
		add	v, 2
		mov	ebx, incX[eax * 4]
		add	rowX, ebx
		mov	ebx, incY[eax * 4]
		add	rowY, ebx
		// inc	lineCount
		inc	eax				// increase lineCountMod4 
		and	eax, 3
		mov	lineCountMod4, eax

		// outer loop comparsion
		cmp	v, LOCATE_HEIGHT * UVMULTIPLIER
		jb		draw_cubic_plane_1;
	}
#endif
}



static int test_inside_polygon(int x, int y, POINT *vertex, int verCount)
{
	err_when( verCount < 3 );	// at least a triangle
	int j;
	int pos = 0;
	int neg = 0;
	int s;

	s = (y-vertex[verCount-1].y)*(vertex->x-vertex[verCount-1].x) -
		(x-vertex[verCount-1].x)*(vertex->y-vertex[verCount-1].y);
	if( s >= 0 )
		++pos;
	if( s <= 0 )
		++neg;
	for( j=1; j < verCount; ++j, ++vertex )
	{
		s = (y-vertex->y)*(vertex[1].x-vertex->x) -
			(x-vertex->x)*(vertex[1].y-vertex->y);
		if( s >= 0 )
			++pos;
		if( s <= 0 )
			--neg;
	}

	if( pos == verCount )
		return 1;
	else if( neg == verCount )
		return -1;
	else
		return 0;
}

// ##### begin Gilbert 25/1 ######//
static int hidden_by_place(int xLoc, int yLoc, int spriteWidth, int spriteHeight)
{
	//      x
	//    4   1
	//      7
	//    5   2
	//      8
	//    6   3
	//      9
	int dMax = SCAN_HIDDEN_DISTANCE;
	if( xLoc + dMax >= MAX_WORLD_X_LOC )
		dMax = MAX_WORLD_X_LOC - xLoc - 1;
	if( yLoc + dMax >= MAX_WORLD_Y_LOC )
		dMax = MAX_WORLD_Y_LOC - yLoc - 1;

	for( int c = 1; c <= 3; ++c )
	{
		int x = xLoc + (c & 1);		// c=1:x+1, c=2:y+1, c=3:x+1,y+1
		int y = yLoc + (c >> 1);
		int d = 1;
		int dPixel = 0;
		for( ; d <= dMax; ++d, ++x, ++y, dPixel -= (ZOOM_LOC_Y_HEIGHT + ZOOM_LOC_X_HEIGHT) )
		{
			Location *locPtr = world.get_loc( x, y );

			if( !(locPtr->loc_flag & LOCATE_BLOCK_MASK) )
				continue;

			if( locPtr->is_firm() )
			{
				if (world.map_matrix->zoom_map_building_disappear)
					return 0 ;	

				Firm *firmPtr = firm_array[locPtr->firm_recno()];
				short minOffsetY = firm_res.get_build(firmPtr->firm_build_id)->min_offset_y;

				if( minOffsetY <= dPixel )
					return 1;
			}

			else if( locPtr->is_town() )
			{
				if( d <= SCAN_HIDDEN_BY_TOWN_DISTANCE )
					return 1;
			}

			else if( locPtr->is_rock() )
			{
				Rock *rockPtr = rock_array[locPtr->rock_array_recno()];
				short rockBitmapRecno = rock_res.get_bitmap_recno(rockPtr->rock_recno, rockPtr->cur_frame);
				RockBitmapInfo *rockBitmapInfo = rock_res.get_bitmap_info( rockBitmapRecno );
				if( rockBitmapInfo->offset_y <= dPixel )
					return 1;
			}

			else if( locPtr->is_plant() )
			{
				PlantBitmap *plantBitmap = plant_res.get_bitmap( locPtr->plant_id() );
				// ignore inner_x(), inner_y() for fast
				if( plantBitmap->offset_y <= dPixel )
					return 1;
			}
		}
	}

	return 0;
}
// ##### end Gilbert 25/1 ######//


// ##### end Gilbert 17/9 ######//

// <int> winX1, winY1, winX2, winY2    - a scroll window, any scrolling cannot scroll out this window
//                                     - usually the whole buffer
// <int> shiftX, shiftY                - scroll size (in number of pixels), positive means move right/down
// [int]fillColor                      - color to fill the shifted area, negative to disable (default : -1)
static void scroll_buffer( short *imageBuf, int truePitch, int winX1, int winY1, int winX2, int winY2, int shiftX, int shiftY, int fillColor )
{
	if( shiftX == 0 && shiftY == 0 )
		return;

	// if shiftX is -1 or 1, the scrolling may produce unpredictable result
	// as the scrolling is using dword copy, i.e. copy two pixels by two pixels

	int srcX1 = winX1;
	int srcY1 = winY1;
	int srcX2 = winX2;
	int srcY2 = winY2;

	if( shiftX >= 0 )
		srcX2 -= shiftX;		// srcX2 decrease
	else
		srcX1 -= shiftX;		// srcX1 increase

	if( shiftY >= 0 )
		srcY2 -= shiftY;		// srcY2 decrease
	else
		srcY1 -= shiftY;		// srcY1 increase

	IMGscrollW( imageBuf, truePitch, srcX1, srcY1, srcX2, srcY2, srcX1+shiftX, srcY1+shiftY );

	if( fillColor < 0 )
		return;

	int destX1 = srcX1 + shiftX;
	int destY1 = srcY1 + shiftY;
	int destX2 = srcX2 + shiftX;
	int destY2 = srcY2 + shiftY;

	if( winX1 <= winX2 )
	{
		if( winY1 < destY1 )		// top side (including corners)
			IMGbar( imageBuf, truePitch, winX1, winY1, winX2, destY1-1, fillColor );

		if( destY2 < winY2 )		// bottom side (including corners)
			IMGbar( imageBuf, truePitch, winX1, destY2+1, winX2, winY2, fillColor );
	}

	if( destY1 <= destY2 )
	{
		if( winX1 < destX1 )		// left side (not including corners)
			IMGbar( imageBuf, truePitch, winX1, destY1, destX1-1, destY2, fillColor );

		if( destX2 < winX2 )		// right side (not including corners)
			IMGbar( imageBuf, truePitch, destX2+1, destY1, winX2, destY2, fillColor );
	}
}


// ---------- begin of function ZoomMatrix::disp_rally_point ---------//

void ZoomMatrix::disp_rally_point()
{
	int flag = 0;
	int curX;
	int curY;
	int curZ;

	// ------ display rally point of the selected firm ------//

	if( firm_array.selected_recno )
	{
		Firm *firmPtr = firm_array[firm_array.selected_recno];
		if( firmPtr->rally_enable_flag )
		{
			if( firmPtr->rally_dest_base_obj_recno && !base_obj_array.is_deleted(firmPtr->rally_dest_base_obj_recno) )
			{
				// ------- if destination is a place --------//

				BaseObj *destObj = base_obj_array[firmPtr->rally_dest_base_obj_recno];
				flag = 1;
				curX = (destObj->obj_loc_x1() + destObj->obj_loc_x2() + 1) * LOCATE_WIDTH / 2;
				curY = (destObj->obj_loc_y1() + destObj->obj_loc_y2() + 1) * LOCATE_HEIGHT / 2;
				if( destObj->cast_to_Place() )
				{
					curZ = destObj->cast_to_Place()->altitude;
				}
				else if( destObj->cast_to_Unit() )
				{
					curX = destObj->cast_to_Unit()->cur_x;
					curY = destObj->cast_to_Unit()->cur_y;
					curZ = destObj->cast_to_Unit()->get_z();
				}
				else
				{
					world.interpolate_z( curX, curY);
				}
			}
			else
			{
				// ------ destination is a space on the map --------//

				flag = 1;
				curX = firmPtr->rally_dest_x * LOCATE_WIDTH + LOCATE_WIDTH / 2;
				curY = firmPtr->rally_dest_y * LOCATE_HEIGHT + LOCATE_HEIGHT / 2;
				curZ = world.interpolate_z( curX, curY);
			}
		}
	}

	if( flag )
	{
		char *chPtr = image_icon.get_ptr("RALLY-PT");
		short chOffsetX = -29;
		short chOffsetY = -45;
		put_bitmap_offset( curX, curY, curZ, chPtr,
			chOffsetX, chOffsetY,
			NULL, 0, 0);
	}
}
// ---------- end of function ZoomMatrix::disp_rally_point ---------//

