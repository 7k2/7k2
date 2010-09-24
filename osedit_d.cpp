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

// Filename    : OSEDIT_D.CPP
// Description : Scenario editor, define area mode

#include <OSEDITOR.H>
#include <ALL.H>
#include <COLOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OSLIDER.H>
#include <OBUTTCUS.H>
#include <OINFO.H>
#include <OWORLDMT.H>
#include <ONATIONA.H>
#include <stdlib.h>
#include <OBUTTON.H>



// ----- define static variable -------//

static Slider slider_area_brush_size;
static ButtonCustomGroup button_nation(MAX_NATION);
static Button button_clear;

// ------ declare static function ------//

static int get_nation_power( int x, int y );
static void set_nation_power( int x, int y, int n );

static void	count_nation_power( long *nationAreaArray );
static void spray_nation_power( long *nationAreaArray, int nationRecno, int xLoc, int yLoc, int areaBrushSize );
static void clear_nation_power( long *nationAreaArray, int nationRecno );
static void disp_nation_button_func(ButtonCustom *, int repaintBody);


// ----- begin of function ScenarioEditor::init_define_mode ------//
//
void ScenarioEditor::init_define_mode()
{
	memset( nation_area, 0, sizeof(nation_area) );

	area_brush_size = 3;
	area_brush_nation = 1;

	count_nation_power( nation_area );	// count power_nation_recno of the map
}
// ----- end of function ScenarioEditor::init_define_mode ------//


// ----- begin of function ScenarioEditor::deinit_define_mode ------//
//
void ScenarioEditor::deinit_define_mode()
{
}
// ----- end of function ScenarioEditor::deinit_define_mode ------//


// ----- begin of function ScenarioEditor::disp_define_main  ------//
//
// display interace area
//
void ScenarioEditor::disp_define_main(int refreshFlag)
{
	const Y_SPACING = 30;

	int y;

	// ----- display slider ------//

	font_whbl.put( INFO_X1+20, INFO_Y1+14, "Brush size :" );
	if( refreshFlag == INFO_REPAINT )
	{
		slider_area_brush_size.init( INFO_X1+100, INFO_Y1+8, 
			100, 30, V_YELLOW, &area_brush_size, 10 );
	}
	slider_area_brush_size.paint();

	// ------ display nation button --------- //

		y = INFO_Y1 + 50;
	for( int nationRecno = 1; nationRecno <= MAX_NATION; ++nationRecno, y += Y_SPACING )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			button_nation[nationRecno-1].create( INFO_X1+15, y, INFO_X2-10, y+Y_SPACING-5,
				disp_nation_button_func, ButtonCustomPara( NULL, nationRecno), 0 );
		}
		button_nation[nationRecno-1].enable_flag = !nation_array.is_deleted(nationRecno);
	}
	button_nation.paint(area_brush_nation-1);

	// ------ display clear button --------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_clear.create_text( INFO_X1+100, y, INFO_X2-10, y+20-1, "Clear" );
	}
	button_clear.paint();

}
// ----- end of function ScenarioEditor::disp_define_main  ------//


// ----- begin of function ScenarioEditor::detect_define_main  ------//
//
// detect interface area
//
void ScenarioEditor::detect_define_main()
{
	int rc;

	// ----- detect slider ------//

	slider_area_brush_size.detect();

	// ------ detect nation button --------- //

	if( (rc = button_nation.detect()) >= 0 )
		area_brush_nation = rc + 1;

	// ------ detect clear button --------//

	if( button_clear.detect() )
	{
		clear_nation_power( nation_area, area_brush_nation );
	}
}
// ----- end of function ScenarioEditor::detect_define_main  ------//


// ----- begin of function ScenarioEditor::disp_define_view ------//
//
// disp in zoom matrix
//
void ScenarioEditor::disp_define_view()
{
}
// ----- end of function ScenarioEditor::disp_define_view ------//


// ----- begin of function ScenarioEditor::detect_define_view ------//
//
// detect in zoom matrix
//
int ScenarioEditor::detect_define_view()
{
	// ------ detect mini-map ---------//

	int rc;

	if( (rc = mouse.press_area( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 2)) )
	{
		int xLoc, yLoc;
		if( world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
		{
			spray_nation_power( nation_area, rc != 2 ? area_brush_nation : 0,		// 0 for remove nation power
				xLoc, yLoc, area_brush_size );
		}

		return 1;
	}
	return 0;
}
// ----- end of function ScenarioEditor::detect_define_view ------//


// ----- begin of static function get_nation_power ------//
//
static int get_nation_power( int x, int y )
{
	return world.get_loc( x,y )->power_nation_recno;
}
// ----- end of static function get_nation_power ------//


// ----- begin of static function set_nation_power ------//
//
static void set_nation_power( int x, int y, int n )
{
	world.get_loc( x,y)->power_nation_recno = n;
}
// ----- end of static function set_nation_power ------//


// ----- begin of static function count_nation_power ------//
//
static void	count_nation_power( long *nationAreaArray )
{
	memset( nationAreaArray, 0, sizeof(long) * MAX_NATION );

	for( int y = 0; y < MAX_WORLD_Y_LOC; ++y )
	{
		for( int x = 0; x < MAX_WORLD_X_LOC; ++x )
		{
			char powerNationRecno = get_nation_power(x,y);
			if( powerNationRecno >= 1 && powerNationRecno <= MAX_NATION )
			{
				++nationAreaArray[powerNationRecno-1];
			}
		}
	}
}
// ----- end of static function count_nation_power ------//


// ----- begin of static function spray_nation_power ------//
//
static void spray_nation_power( long *nationAreaArray, int nationRecno, int xLoc, int yLoc, int areaBrushSize )
{
	int x1 = xLoc - areaBrushSize;
	int x2 = xLoc + areaBrushSize;
	int y1 = yLoc - areaBrushSize;
	int y2 = yLoc + areaBrushSize;

	// ----- clip -------//

	x1 = max( x1, 0 );
	x2 = min( x2, MAX_WORLD_X_LOC );
	y1 = max( y1, 0 );
	y2 = min( y2, MAX_WORLD_Y_LOC );

	int x, y;

	for( y = y1; y <= y2; ++y )
	{
		x = x1;
		for( ; x <= x2; ++x )
		{
			int oldNationRecno = get_nation_power(x,y);

			// ------ decrease original nation power count -------//

			if( oldNationRecno >= 1 && oldNationRecno <= MAX_NATION )
			{
				--nationAreaArray[oldNationRecno-1];
			}

			//------- set new nation power --------//

			set_nation_power(x, y, nationRecno);

			// ------ increase new nation power count ------//

			if( nationRecno >= 1 && nationRecno <= MAX_NATION )
			{
				++nationAreaArray[nationRecno-1];
			}
		}
	}
}
// ----- end of static function spray_nation_power ------//


// ----- begin of static function clear_nation_power ------//
//
static void clear_nation_power( long *nationAreaArray, int nationRecno )
{
	err_when( nationRecno < 1 || nationRecno > MAX_NATION );

	for( int y = 0; y < MAX_WORLD_Y_LOC; ++y )
	{
		for( int x = 0; x < MAX_WORLD_X_LOC; ++x )
		{
			char powerNationRecno = get_nation_power(x, y);
			if( powerNationRecno == nationRecno )
			{
				--nationAreaArray[powerNationRecno-1];
			}
			set_nation_power(x, y, 0 );
		}
	}

	err_when(nationAreaArray[nationRecno-1] != 0);
}
// ----- end of static function clear_nation_power ------//


// ----- begin of static function disp_nation_button_func ------//
//
void disp_nation_button_func( ButtonCustom *button, int )
{
	int nationRecno = button->custom_para.value;

	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( button->x1, button->y1, 
			button->x2, button->y2 );
	}
	else
	{
		vga.active_buf->d3_panel_down( button->x1, button->y1, 
			button->x2, button->y2 );
	}

	if( nation_array.is_deleted(nationRecno) )
	{
		font_snds.center_put( button->x1, button->y1, 
			button->x2, button->y2, "Not Used" );
	}
	else
	{
		int p = button->pushed_flag ? 1 : 0;
		char nationColor = nation_array[nationRecno]->nation_color;
		long areaSize = scenario_editor.nation_area[nationRecno-1];

		vga.active_buf->bar( button->x1+p+5, button->y1+p+5, button->x1+p+22,
			button->y2+p-5, nationColor );

		int x2 = font_snds.put( button->x1+p+30, button->y1+p+7, "Size : ");
		font_snds.put( x2, button->y1+p+7, areaSize );
	}
}
// ----- end of static function disp_nation_button_func ------//
