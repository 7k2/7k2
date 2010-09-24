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

// Filename    : OSEDITOR.CPP
// Description : Scenario editor

#include <OSEDITOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OIMGRES.H>
#include <OBLOB2W.H>
#include <OANLINE.H>
#include <COLCODE.H>
#include <OINFO.H>
#include <OWORLDMT.H>
#include <OCONFIG.H>
#include <OT_SEDIT.H>


// -------- define constant --------//

#define SEDIT_MODE_BUTTON_WIDTH 48
#define SEDIT_MODE_BUTTON_HEIGHT 20
#define SEDIT_MODE_BUTTON_X_SPACING 50

// ------ define static variable --------//

static char last_enable_flag;
static int last_edit_mode;
static int last_edit_sub_menu_mode;

//static char *edit_mode_button_label[SEDIT_MODE_COUNT] =
//{
//	"Off",
//	"Human Objects",
//	"Fryhtan Objects",
//	"Misc. Objects",
//	"Attributes Editor",
//	"Players Information",
//	"Diplomacy Relations",
//	"Victory Conditions",
//	"Hero Editor",
//};

// ------ define buttons ----------//


static ButtonGroup button_edit_mode(SEDIT_MODE_COUNT);
static Button button_blacken_map;
static Button button_menu_on;

// ------ begin of function ScenarioEditor::ScenarioEditor -------//
//
ScenarioEditor::ScenarioEditor()
{
	init_flag = 0;
	enable_flag = 0;
	open_flag = 0;
}

// ------ end of function ScenarioEditor::ScenarioEditor -------//


// ------ begin of function ScenarioEditor::~ScenarioEditor -------//
//
ScenarioEditor::~ScenarioEditor()
{
	deinit();
}
// ------ end of function ScenarioEditor::~ScenarioEditor -------//


// ------ begin of function ScenarioEditor::init -------//
//
void ScenarioEditor::init()
{
	deinit();

	enable_flag = 0;
	open_flag = 0;

	edit_mode = SEDIT_MODE_NONE;
	memset( edit_sub_menu_mode, 0, sizeof(edit_sub_menu_mode) );
	selected_nation = 0;

	// ------- init each menu mode ------//

//	init_terrain_mode();
	init_human_mode();
	init_monster_mode();
	init_misc_mode();
	init_edit_mode();
//	init_define_mode();
	init_player_mode();
	init_diplomacy_mode();
	init_victory_mode();
	init_hero_mode();

	init_flag = 1;
}
// ------ end of function ScenarioEditor::init -------//


// ------ begin of function ScenarioEditor::deinit -------//
//
void ScenarioEditor::deinit()
{
	if( init_flag )
	{
		disable();

		// ----- deinit each menu mode ------//

//		deinit_terrain_mode();
		deinit_human_mode();
		deinit_monster_mode();
		deinit_misc_mode();
		deinit_edit_mode();
//		deinit_define_mode();
		deinit_player_mode();
		deinit_diplomacy_mode();
		deinit_victory_mode();
		deinit_hero_mode();

		init_flag = 0;
	}
}
// ------ end of function ScenarioEditor::deinit -------//


// ------ begin of function ScenarioEditor::enable -------//
//
void ScenarioEditor::enable()
{
	if( init_flag && !enable_flag )
	{
		disp_menu_bar( INFO_REPAINT );
		enable_flag = 1;
		open_flag = 0;
	}
}
// ------ end of function ScenarioEditor::enable -------//


// ------ begin of function ScenarioEditor::disable -------//
//
void ScenarioEditor::disable()
{
	if( init_flag && enable_flag )
	{
		enable_flag = 0;
	}
}
// ------ end of function ScenarioEditor::disable -------//


// ------ begin of function ScenarioEditor::disp_menu_bar -------//
//
// display menu bar at the top of the screen
//
void ScenarioEditor::disp_menu_bar(int refreshFlag)
{

#ifdef USE_FLIP
	char useBack = vga.use_back_buf;
	vga.use_back();
#endif

	if( refreshFlag == INFO_REPAINT || last_enable_flag != enable_flag )
	{
		refreshFlag = INFO_REPAINT;
		last_enable_flag = enable_flag;
	}

	if( refreshFlag == INFO_REPAINT )
	{
		button_menu_on.create_text( ZOOM_X1+5, ZOOM_Y1+5, ZOOM_X1+105, ZOOM_Y1+45,
			text_editor.str_editor_menu(), // "Scenario Editor Menu",
			0, 0, 1 );
		button_menu_on.is_pushed = open_flag;
	}
		
	if( refreshFlag == INFO_REPAINT )
	{
		int x0 = ZOOM_X1+5;
//		int x0 = ZOOM_X1+20;
		int x = x0;
		int y = ZOOM_Y1+5;
		for( int i = 0; i < button_edit_mode.button_num; ++i )
		{
			// ####### begin Gilbert 22/1 #####//
//			button_edit_mode[i].set_font(&font_bld);
			// ####### end Gilbert 22/1 #####//
			
			button_edit_mode[i].create_text( x, y, x+100, y+40, 
				text_editor.str_mode(i), // edit_mode_button_label[i],
				1, 0, 1 );
		/*	x = button_edit_mode[i].x2;
			if( x >= ZOOM_X2 )
			{
				x = x0;
				y = button_edit_mode[i].y2 + 2;
				--i;			// create again at next line
			}
			else
				x += 2;		// inter button spacing*/

			y = button_edit_mode[i].y2 + 2;
		}

		x = x0;
		if( i > 0 )
			y = button_edit_mode[i-1].y2 + 2;
		// else use original y which is ZOOM_Y1 + 5
		button_blacken_map.create_text( x, y, x+100, y+40, 
			text_editor.str_blacken_map(), // "Blacken Map",
			0, 0, 1 );
	}

//	if (button_menu_on.is_pushed)
	if (open_flag)
	{
		button_edit_mode.paint(edit_mode);

		if( !config.explore_whole_map || config.fog_of_war )
		{
			button_blacken_map.paint( config.blacken_map );
		}
	}

	button_menu_on.paint();

#ifdef USE_FLIP
	if( !useBack )
		vga.use_front();
#endif
}
// ------ end of function ScenarioEditor::disp_menu_bar -------//


// ------ begin of function ScenarioEditor::detect_menu_bar -------//
//
// detect menu bar at the top of the screen
//
int ScenarioEditor::detect_menu_bar()
{
	if (button_menu_on.detect())
	{
		edit_mode = 0;
		open_flag = !open_flag;
		return 1;
	}

//	if (button_menu_on.is_pushed)
	if (open_flag)
	{
		int rc = button_edit_mode.detect();
		if( rc >= 0 )
		{
			edit_mode = rc;
			return 1;
		}

		if( !config.explore_whole_map || config.fog_of_war )
		{
			if( button_blacken_map.detect() )
			{
				config.blacken_map = button_blacken_map.is_pushed;
			}
		}
	}
	return 0;
}
// ------ end of function ScenarioEditor::detect_menu_bar -------//


// ------ begin of function ScenarioEditor::disp -------//
//
// display interface area
//
void ScenarioEditor::disp( int refreshFlag )
{
	if( refreshFlag == INFO_REPAINT 
		|| edit_mode != last_edit_mode
		|| edit_sub_menu_mode[edit_mode] != last_edit_sub_menu_mode )
	{
		refreshFlag = INFO_REPAINT;
		last_edit_mode = edit_mode;
		last_edit_sub_menu_mode = edit_sub_menu_mode[edit_mode];
	}

#ifdef USE_FLIP
	char useBack = vga.use_back_buf;
	vga.use_back();
#endif

	if( edit_mode != SEDIT_MODE_NONE )
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("SCEN-ED") );

	switch( edit_mode )
	{
	case SEDIT_MODE_NONE:
		break;

//	case SEDIT_MODE_TERRAIN:
//		disp_terrain_main(refreshFlag);
//		break;

	case SEDIT_MODE_HUMAN:
		disp_human_main(refreshFlag);
		break;

	case SEDIT_MODE_MONSTER:
		disp_monster_main(refreshFlag);
		break;

	case SEDIT_MODE_MISC:
		disp_misc_main(refreshFlag);
		break;

	case SEDIT_MODE_EDIT:
		disp_edit_main(refreshFlag);
		break;

//	case SEDIT_MODE_DEFINE:
//		disp_define_main(refreshFlag);
//		break;

	case SEDIT_MODE_PLAYER:
		disp_players_main(refreshFlag);
		break;

	case SEDIT_MODE_DIPLOMACY:
		disp_diplomacy_main(refreshFlag);
		break;

	case SEDIT_MODE_VICTORY:
		disp_victory_main(refreshFlag);
		break;

	case SEDIT_MODE_HERO:
		disp_hero_main(refreshFlag);
		break;

	default:
		err_here();
	}

#ifdef USE_FLIP
	if( !useBack )
		vga.use_front();
#endif
}
// ------ end of function ScenarioEditor::disp -------//


// ------ begin of function ScenarioEditor::detect -------//
//
// detect interface area
//
void ScenarioEditor::detect()
{
	switch( edit_mode )
	{
	case SEDIT_MODE_NONE:
		break;

//	case SEDIT_MODE_TERRAIN:
//		detect_terrain_main();
//		break;

	case SEDIT_MODE_HUMAN:
		detect_human_main();
		break;

	case SEDIT_MODE_MONSTER:
		detect_monster_main();
		break;

	case SEDIT_MODE_MISC:
		detect_misc_main();
		break;

	case SEDIT_MODE_EDIT:
		detect_edit_main();
		break;

//	case SEDIT_MODE_DEFINE:
//		detect_define_main();
//		break;

	case SEDIT_MODE_PLAYER:
		detect_players_main();
		break;

	case SEDIT_MODE_DIPLOMACY:
		detect_diplomacy_main();
		break;

	case SEDIT_MODE_VICTORY:
		detect_victory_main();
		break;

	case SEDIT_MODE_HERO:
		detect_hero_main();
		break;


	default:
		err_here();
	}
}
// ------ end of function ScenarioEditor::detect -------//


// ------ begin of function ScenarioEditor::disp_view -------//
//
// disp zoom area
//
void ScenarioEditor::disp_view()
{
	char useBack = vga.use_back_buf;
	vga.use_back();

	switch( edit_mode )
	{
	case SEDIT_MODE_NONE:
		break;

//	case SEDIT_MODE_TERRAIN:
//		disp_terrain_view();
//		break;

	case SEDIT_MODE_HUMAN:
		disp_human_view();
		break;

	case SEDIT_MODE_MONSTER:
		disp_monster_view();
		break;

	case SEDIT_MODE_MISC:
		disp_misc_view();
		break;

	case SEDIT_MODE_EDIT:
		disp_edit_view();
		break;

//	case SEDIT_MODE_DEFINE:
//		disp_define_view();
//		break;

	case SEDIT_MODE_PLAYER:
		disp_players_view();
		break;

	case SEDIT_MODE_DIPLOMACY:
		disp_diplomacy_view();
		break;

	case SEDIT_MODE_VICTORY:
		disp_victory_view();
		break;

	case SEDIT_MODE_HERO:
		disp_hero_view();
		break;

	default:
		err_here();
	}

	if( !useBack )
		vga.use_front();
}
// ------ end of function ScenarioEditor::disp_view -------//


// ------ begin of function ScenarioEditor::detect_view -------//
//
int ScenarioEditor::detect_view()
{
	switch( edit_mode )
	{
	case SEDIT_MODE_NONE:
		break;

//	case SEDIT_MODE_TERRAIN:
//		return detect_terrain_view();

	case SEDIT_MODE_HUMAN:
		return detect_human_view();

	case SEDIT_MODE_MONSTER:
		return detect_monster_view();

	case SEDIT_MODE_MISC:
		return detect_misc_view();

	case SEDIT_MODE_EDIT:
		return detect_edit_view();

//	case SEDIT_MODE_DEFINE:
//		return detect_define_view();

	case SEDIT_MODE_PLAYER:
		return detect_players_view();

	case SEDIT_MODE_DIPLOMACY:
		return detect_diplomacy_view();

	case SEDIT_MODE_VICTORY:
		return detect_victory_view();

	case SEDIT_MODE_HERO:
		return detect_hero_view();
		
	default:
		err_here();
	}

	return 0;
}
// ------ end of function ScenarioEditor::detect_view -------//


// ------ begin of function ScenarioEditor::draw_marker -------//
//
void ScenarioEditor::draw_marker( int validAction, int markerX1, int markerY1, int markerX2, int markerY2 )
{
	// ------ draw marker -------//

	if( markerX1 < 0 )
		markerX1 = 0;
	if( markerY1 < 0 )
		markerY1 = 0;
	if( markerX2 >= MAX_WORLD_X_LOC )
		markerX2 = MAX_WORLD_X_LOC-1;
	if( markerY2 >= MAX_WORLD_Y_LOC )
		markerY2 = MAX_WORLD_Y_LOC-1;

#ifdef DEBUG
	// draw rhombus frame to see terrain effect
	int animLineColorSeries = 3;

	short curX1 = markerX1 * LOCATE_WIDTH;
	short curY1 = markerY1 * LOCATE_HEIGHT;
	short curX2 = (markerX2 + 1) * LOCATE_WIDTH - 1;
	short curY2 = (markerY2 + 1) * LOCATE_HEIGHT - 1;
	short curZ = world.get_corner( markerX2+1, markerY2+1 )->get_altitude();
	int rectX0 = ZOOM_X1 + world.zoom_matrix->calc_zoom_x( curX1, curY1, curZ );
	int rectY0 = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y( curX1, curY1, curZ );
	int rectX1 = ZOOM_X1 + world.zoom_matrix->calc_zoom_x( curX2, curY1, curZ );
	int rectY1 = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y( curX2, curY1, curZ );
	int rectX2 = ZOOM_X1 + world.zoom_matrix->calc_zoom_x( curX2, curY2, curZ );
	int rectY2 = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y( curX2, curY2, curZ );
	int rectX3 = ZOOM_X1 + world.zoom_matrix->calc_zoom_x( curX1, curY2, curZ );
	int rectY3 = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y( curX1, curY2, curZ );
	anim_line.draw_line( &vga_back, rectX0, rectY0, rectX1, rectY1, 1, animLineColorSeries);
	anim_line.draw_line( &vga_back, rectX1, rectY1, rectX2, rectY2, 1, animLineColorSeries);
	anim_line.draw_line( &vga_back, rectX2, rectY2, rectX3, rectY3, 1, animLineColorSeries);
	anim_line.draw_line( &vga_back, rectX3, rectY3, rectX0, rectY0, 1, animLineColorSeries);

#endif

	Blob2DW maskBitmap;

	int pixelColor;
	if( validAction > 0 )
		pixelColor = anim_line.get_series_color_array(-1)[2];
	else if( validAction < 0 )
		pixelColor = V_BLACK;
	else
		pixelColor = V_RED;

	int xLoc, yLoc;

	for( yLoc = markerY1; yLoc <= markerY2; ++yLoc )
	{
		for( xLoc = markerX1; xLoc <= markerX2; ++xLoc )
		{
			LocationCorners lc;
			world.get_loc_corner(xLoc, yLoc, &lc);
			maskBitmap.clear();
			lc.render_special(&maskBitmap, 1, NULL, pixelColor, TRANSPARENT_CODE);
			world.zoom_matrix->put_bitmapW_offset(xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, lc.top_left->altitude,
				maskBitmap.bitmap_ptr(), maskBitmap.left_edge, maskBitmap.top_edge, 0, 0);
		}
	}
}
// ------ end of function ScenarioEditor::draw_marker -------//
