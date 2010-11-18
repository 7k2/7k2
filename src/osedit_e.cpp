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

// Filename    : OSEDIT_E.CPP
// Description : Scenario editor edit mode


#include <oseditor.h>
#include <omodeid.h>
#include <omouse.h>
#include <opower.h>
#include <oinfo.h>
#include <ounit.h>
#include <ofirm.h>
#include <otown.h>
#include <osite.h>
#include <oworldmt.h>



// ----- begin of function ScenarioEditro::init_edit_mode -------//
//
void ScenarioEditor::init_edit_mode()
{
	edit_mode_flag = 0;
}
// ----- end of function ScenarioEditro::init_edit_mode -------//


// ----- begin of function ScenarioEditro::deinit_edit_mode -------//
//
void ScenarioEditor::deinit_edit_mode()
{
	edit_mode_flag = 0;
}
// ----- end of function ScenarioEditro::deinit_edit_mode -------//


// ----- begin of function ScenarioEditor::is_edit_mode -------//
//
int ScenarioEditor::is_edit_mode()
{
	return edit_mode_flag;
}
// ----- end of function ScenarioEditor::is_edit_mode -------//


// ----- begin of function ScenarioEditro::disp_edit_main -------//
//
void ScenarioEditor::disp_edit_main(int refreshFlag)
{
	// disable scenario editor for info.disp or info.update to display
	// the interface of the selected object

	edit_mode_flag = enable_flag;
	char backupEnableFlag = enable_flag;
	disable();

	if( unit_array.selected_recno )
	{
		unit_array[unit_array.selected_recno]->disp_edit_mode( refreshFlag );
	}
	else if( town_array.selected_recno )
	{
		town_array[town_array.selected_recno]->disp_edit_mode( refreshFlag );
	}
	else if( firm_array.selected_recno )
	{
		int dispY1 = INFO_Y1;
		firm_array[firm_array.selected_recno]->disp_edit_mode( refreshFlag, dispY1 );
	}
	else if( site_array.selected_recno )
	{
		site_array[site_array.selected_recno]->disp_edit_mode( refreshFlag );
	}
	else
	{
		if( refreshFlag == INFO_REPAINT )
			info.disp();
		else
			info.update();
	}

	// restore enable flag 
	enable_flag = backupEnableFlag;
	edit_mode_flag = 0;
}
// ----- end of function ScenarioEditro::disp_edit_main -------//


// ----- begin of function ScenarioEditro::detect_edit_main -------//
//
void ScenarioEditor::detect_edit_main()
{
	// disable scenario editor for info.detect to detect 
	// the interface of the selected object

	char backupEnableFlag = enable_flag;
	disable();

	if( unit_array.selected_recno )
	{
		unit_array[unit_array.selected_recno]->detect_edit_mode();
	}
	else if( town_array.selected_recno )
	{
		town_array[town_array.selected_recno]->detect_edit_mode();
	}
	else if( firm_array.selected_recno )
	{
		firm_array[firm_array.selected_recno]->detect_edit_mode();
	}
	else if( site_array.selected_recno )
	{
		site_array[site_array.selected_recno]->detect_edit_mode();
	}
	else
	{
		info.detect();
	}

	// restore enable flag 

	enable_flag = backupEnableFlag;
}
// ----- end of function ScenarioEditro::detect_edit_main -------//


// ----- begin of function ScenarioEditro::disp_edit_view -------//
//
void ScenarioEditor::disp_edit_view()
{
	if( power.command_id )
		return;

	int drawMarker = 0;
	int validAction = 0;
	int markerX1, markerY1, markerX2, markerY2;

	if( mouse.in_area( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) )
	{
		int xLoc, yLoc;
		char mobileType;

		if( world.zoom_matrix->get_detect_location( mouse.cur_x, mouse.cur_y, 
			&xLoc, &yLoc, &mobileType) )
		{
			if( unit_array.selected_recno && !unit_array.is_deleted(unit_array.selected_recno)  )
			{
				Unit *unitPtr = unit_array[unit_array.selected_recno];
				if( world.check_unit_space( xLoc, yLoc, 
					xLoc+unitPtr->sprite_info->loc_width-1, yLoc+unitPtr->sprite_info->loc_height-1,
					unitPtr->mobile_type) )
				{
					validAction = 1;
				}
				markerX1 = xLoc;
				markerY1 = yLoc;
				markerX2 = xLoc + unitPtr->obj_loc_width() - 1;
				markerY2 = yLoc + unitPtr->obj_loc_height() - 1;
				drawMarker = 1;
			}
		}
	}

	// ------ draw marker -------//

	if( drawMarker )
	{
		draw_marker( validAction, markerX1, markerY1, markerX2, markerY2 );
	}
}
// ----- end of function ScenarioEditro::disp_edit_view -------//


// ----- begin of function ScenarioEditro::detect_edit_view -------//
//
int ScenarioEditor::detect_edit_view()
{
	if( power.command_id )
		return 0;

	// ------- move unit --------//

	if( mouse.double_click( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0 ) )
	{
		int xLoc, yLoc;
		char mobileType;

		if( world.zoom_matrix->get_detect_location( mouse.click_x(0), mouse.click_y(0), 
			&xLoc, &yLoc, &mobileType) )
		{
			if( unit_array.selected_recno && !unit_array.is_deleted(unit_array.selected_recno)  )
			{
				// ###### begin Gilbert 6/3 ########//
				unit_array[unit_array.selected_recno]->teleport(xLoc, yLoc);
				// ###### end Gilbert 6/3 ########//
			}
		}
		return 1;
	}

	return 0;
}
// ----- end of function ScenarioEditro::detect_edit_view -------//
