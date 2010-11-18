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

// Filename    : OSEDIT_T.CPP
// Description : Scenario editor, terrain mode

#include <OSEDITOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <COLOR.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OSLIDER.H>
#include <OPLASMA.H>
#include <OINFO.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OWORLDMT.H>




// ------ define brush type -------//

enum
{
	BRUSH_TYPE_ELEVATION,
	BRUSH_TYPE_TERRAIN,
	BRUSH_TYPE_ROAD,
	BRUSH_TYPE_HILL,
	BRUSH_TYPE_PLANT,
	BRUSH_TYPE_CLEAR,

	BRUSH_TYPE_COUNT		// keep it last
};


#define BRUSH_TYPE_Y_SPACING 22

// ----- define static variable -------//

static Button button_generate, button_blank;
static Slider slider_brush_size;
static Slider slider_brush_pressure;
static ButtonGroup button_brush_type(BRUSH_TYPE_COUNT);
static char *button_brush_type_label[BRUSH_TYPE_COUNT] = 
{
	"Elevation", "Terrain", "Road", "Mountain", "Plant", "Clear" 
};


// -------- declare static function -----------//

static void brush_plasma( Plasma &plasma, int xLoc, int yLoc, int range, int pressure, 
	int low, int high, int maxDiff );



void ScenarioEditor::init_terrain_mode()
{
	// ------ init terrain data ------//

//	height_map.init(MAX_MAP_WIDTH, MAX_MAP_HEIGHT);
//	terrain_map.init(MAX_MAP_WIDTH, MAX_MAP_HEIGHT);
//	road_map.init(MAX_MAP_WIDTH, MAX_MAP_HEIGHT, terrain_res.min_height(TERRAIN_ROAD)-10 );
//	hill_map.init(MAX_MAP_WIDTH-1, MAX_MAP_HEIGHT-1);
//	plant_map.init(MAX_MAP_WIDTH-1, MAX_MAP_HEIGHT-1);

	brush_size = 5;		// MAX=10
	brush_pressure = 5;		// MAX=10
	brush_type = 0;		// elevation
}


void ScenarioEditor::deinit_terrain_mode()
{
	// ------ deinit terrain data ------//

//	height_map.deinit();
//	terrain_map.deinit();
//	road_map.deinit();
//	hill_map.deinit();
//	plant_map.deinit();
}


void ScenarioEditor::disp_terrain_main(int refreshFlag)
{
	int y = INFO_Y1+6;

	// ------ generate map or blank map button --------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_generate.create_text(INFO_X1+15, y, INFO_X1+114, y+20, "Generate");
		button_blank.create_text(INFO_X1+125, y, INFO_X1+214, y+20, "Blank Map");
	}
	button_generate.paint();
	button_blank.paint();
	y+=25;

	// -------- brush size slider --------//

	if( refreshFlag == INFO_REPAINT )
		slider_brush_size.init( INFO_X1+100, y, 100, 20, VGA_RED, &brush_size, 10);
	font_whbl.center_put( INFO_X1, y, INFO_X1+100, y+20, "Brush Size" );
	slider_brush_size.paint();
	y+=25;

	// -------- brush pressure slider ------//

	if( refreshFlag == INFO_REPAINT )
		slider_brush_pressure.init( INFO_X1+100, y, 100, 20, VGA_DARK_BLUE, &brush_pressure, 10);
	font_whbl.center_put( INFO_X1, y, INFO_X1+100, y+20, "Brush Pressure" );
	slider_brush_pressure.paint();
	y+=25;

	// ----- brush type slider --------//

	if( refreshFlag == INFO_REPAINT )
	{
		int i;
		for( i = 0; i < BRUSH_TYPE_COUNT; ++i )
		{
			button_brush_type[i].create_text(INFO_X1+100, y+i*BRUSH_TYPE_Y_SPACING,
				INFO_X2-10, y+i*BRUSH_TYPE_Y_SPACING + BRUSH_TYPE_Y_SPACING-3,
				button_brush_type_label[i], 0 );
		}
	}
	font_whbl.center_put(INFO_X1+5, y, INFO_X1+100, y+BRUSH_TYPE_Y_SPACING, "Brush Type :");
	button_brush_type.paint(brush_type);
	y += BRUSH_TYPE_COUNT * BRUSH_TYPE_Y_SPACING;
}


void ScenarioEditor::detect_terrain_main()
{
	// ------ detect interface area ------//

	if( button_generate.detect() )
	{
	}

	if( button_blank.detect() )
	{
	}

	slider_brush_size.detect();		// automatically set brush_size;
	slider_brush_pressure.detect();		// automatically set brush_pressure

	int rc;
	rc = button_brush_type.detect();
	if( rc >= 0 )
		brush_type = rc;
}


void ScenarioEditor::disp_terrain_view()
{
}


int ScenarioEditor::detect_terrain_view()
{
	// ------ detect mini-map action -------- //

	int rc;
	if( (rc = mouse.press_area(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2)) )
	{
		int xLoc, yLoc;
		if( world.map_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL ) )
		{
			// if left button is pressed rc = 1; if right button is pressed, rc = 2

			switch( brush_type )
			{
			case BRUSH_TYPE_ELEVATION:
//				brush_plasma( height_map, xLoc, yLoc, brush_size, 
//					rc!=2 ? brush_pressure : -brush_pressure, 0, 255, 32);
				break;
			case BRUSH_TYPE_TERRAIN:
//				brush_plasma( terrain_map, xLoc, yLoc, brush_size, 
//					rc!=2 ? brush_pressure : -brush_pressure, 
//					terrain_res.min_height(1), terrain_res.max_height(MAX_GEN_TERRAIN_TYPE), 32);
				break;
			case BRUSH_TYPE_ROAD:
//				brush_plasma( terrain_map, xLoc, yLoc, brush_size, 
//					rc!=2 ? brush_pressure : -brush_pressure, 
//					terrain_res.min_height(TERRAIN_ROAD)-10, terrain_res.max_height(TERRAIN_ROAD), 32);
				break;
			case BRUSH_TYPE_HILL:
				//brush_plasma( hill_map, xLoc, yLoc, brush_size, 
				//	rc!=2 ? brush_pressure : -brush_pressure, 
				//	0, 255, 32);
//				if( rc != 2 )
//					hill_map.plot( xLoc, yLoc, 255 );
//				else
//					hill_map.plot( xLoc, yLoc, 0 );
				break;
			case BRUSH_TYPE_PLANT:
//				if( rc != 2 )
//				{
//					// randomly scan a plant to put
//					int plantId = plant_res.scan( ZONE_TROPICAL | ZONE_TEMPERATE, 0, 0 );
//					if( plantId )
//					{
//						PlantInfo *plantInfo = plant_res[plantId];
//						int plantBitmapId = plantInfo->first_bitmap + m.random( plantInfo->bitmap_count );
//						plant_map.plot( xLoc, yLoc, plantBitmapId );
//					}
//				}
//				else
//				{
//					plant_map.plot( xLoc, yLoc, 0 );
//				}
				break;
			case BRUSH_TYPE_CLEAR:
//				hill_map.plot( xLoc, yLoc, 0 );
//				plant_map.plot( xLoc, yLoc, 0 );
				break;
			default:
				err_here();
			}
		}

		return 1;
	}

	return 0;
}



static void brush_plasma( Plasma &plasma, int xLoc, int yLoc, int range, int pressure, 
	int low, int high, int maxDiff )
{
/*
	int startXLoc = curXLoc-terrain_change_range;
	int endXLoc   = curXLoc+terrain_change_range;
	int startYLoc = curYLoc-terrain_change_range;
	int endYLoc   = curYLoc+terrain_change_range;

	startYLoc = max(startYLoc, 0);
	endYLoc   = min(endYLoc, MAX_WORLD_Y_LOC-1);

	int xLoc, yLoc, xLoc1, xLoc2, t;
	Location *locPtr;

	static short tool_min_height[] = { 0					  , MIN_GRASS_HEIGHT , MIN_HILL_HEIGHT };
	static short tool_max_height[] = { MIN_GRASS_HEIGHT-1, MIN_HILL_HEIGHT-1, 255 };
	short  minHeight, maxHeight;

	for( yLoc=startYLoc; yLoc<=endYLoc ; yLoc++ )
	{
		xLoc1 = startXLoc+abs(curYLoc-yLoc)/2;
		xLoc2 = endXLoc-abs(curYLoc-yLoc)/2;

		xLoc1 = max(xLoc1, 0);
		xLoc2 = min(xLoc2, MAX_WORLD_X_LOC-1);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++ )
		{
			locPtr = map_edit_buf+yLoc*MAX_WORLD_X_LOC+xLoc;

			//---------- set min & max height ---------//

			if( button_keep_type.is_pushed() )		// keep the original terrain type
			{
				if( locPtr->id < MIN_GRASS_HEIGHT )
				{
					minHeight = 0;
					maxHeight = MIN_GRASS_HEIGHT-1;
				}
				else if( locPtr->id < MIN_HILL_HEIGHT )
				{
					minHeight = MIN_GRASS_HEIGHT;
					maxHeight = MIN_HILL_HEIGHT-1;
				}
				else
				{
					minHeight = MIN_HILL_HEIGHT;
					maxHeight = 255;
				}

				//------ a terrain type has been specified --------//

				if( button_tool.button_pressed >= 0 )
				{
					//--- if the terrain type of the current location is different from the specified type, do nothing here ---//

					if( minHeight != tool_min_height[button_tool.button_pressed] )
						continue;
				}
			}
			else if( button_tool.button_pressed >= 0 )
			{
				minHeight = tool_min_height[button_tool.button_pressed];
				maxHeight = tool_max_height[button_tool.button_pressed];
			}
			else
			{
				minHeight = 0;
				maxHeight = 255;
			}

			//------------- adjust height -------------//

			if( changeLevel > 0 )   // raising
				locPtr->id += max(0, changeLevel - abs(curXLoc-xLoc)*2/3 - abs(curYLoc-yLoc)*2/3 );
			else
				locPtr->id += min(0, changeLevel + abs(curXLoc-xLoc)*2/3 + abs(curYLoc-yLoc)*2/3 );

			if( locPtr->id < minHeight )
				locPtr->id = minHeight+m.random(10);

			if( locPtr->id > maxHeight ||
				 locPtr->id > maxHeight-4+(abs(xLoc-curXLoc)+abs(yLoc-curYLoc))/6 )    // don't create ice mountains in terrain editing
			{
				locPtr->id = maxHeight-9+m.random(10);
			}
		}

		//--- only copy the modified portion to the map location matrix ---//

		memcpy( loc_matrix+yLoc*MAX_WORLD_X_LOC+max(startXLoc,0),
				  map_edit_buf+yLoc*MAX_WORLD_X_LOC+max(startXLoc,0),
				  (min(endXLoc,MAX_WORLD_X_LOC-1)-max(startXLoc,0)+1) * sizeof(Location) );
	}
*/
	for( int r = range; r >= 0; --r )
	{
		// check different of point and point(s) outside within maxDiff

		int x, y;
		int circum = 8*r;

		// traverse the boundary of square of size 2*r+1, from the top left corner
		// to find the max difference

		x = xLoc - r;
		y = yLoc - r;
		int rangeInc = pressure * 4 / (4+r);
		for( int c = circum; c < circum; ++c, 
			(c<2*r?++x : (c<4*r?++y : (c<6*r?--x : --y))) )
		{
			if( x >= 0 && x <= plasma.max_x && y >= 0 && y <= plasma.max_y )	// include max_x, max_y
			{
				int heightxy = plasma.get_pix( x, y );
				int maxAdj = 0;
				int minAdj = high;

				// scan adjacent point to find the minimum and maximum

				int adjX, adjY;
				static offsetX[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };
				static offsetY[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };
				for( int sur = 0; sur < 8; ++sur )
				{
					adjX = x+offsetX[sur];
					adjY = y+offsetY[sur];
					if( adjX >= 0 && adjX <= plasma.max_x && adjY >= 0 && adjY <= plasma.max_y 
						&& m.points_distance(adjX, adjY, xLoc, yLoc) > r )	// adjacent point outside checking range
					{
						int heightAdj = plasma.get_pix(adjX, adjY);
						if( heightAdj > maxAdj )
							maxAdj = heightAdj;
						if( heightAdj < minAdj )
							minAdj = heightAdj;
					}
				}

				heightxy += rangeInc;

				// maintain adjacent points difference less than maxDiff

				if( heightxy > minAdj + maxDiff )
					heightxy = minAdj + maxDiff;
				if( heightxy < maxAdj - maxDiff )
					heightxy = maxAdj - maxDiff;

				// maintain point between low and high

				if( heightxy > high )
					heightxy = high;
				if( heightxy < low )
					heightxy = low;

				// update pixel

				plasma.plot(x,y, heightxy);
			}
		}
	}
}
