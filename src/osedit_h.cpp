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

// Filename    : OSEDIT_H.CPP
// Description : Scenario editor human

// update any to to OSEDIT_M.CPP as well

#include <oseditor.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <obutton.h>
#include <obuttcus.h>
#include <ospin_s.h>
#include <ovbrowif.h>
#include <ogame.h>
#include <opower.h>
#include <ogodres.h>
#include <oraceres.h>
#include <ounitres.h>
#include <ofirmres.h>
#include <oworldmt.h>
#include <onationa.h>
#include <ou_wagon.h>
#include <obox.h>
#include <ot_sedit.h>

// ------- define static variable --------------//

static ButtonGroup button_unit_or_struct(2);
static ButtonCustomGroup button_player_recno(1+MAX_NATION);
//static ButtonGroup button_race_group(1+MAX_RACE);
static SpinnerSmall spinner_race_group;
static VBrowseIF vbrowse_unit_id;
static VBrowseIF vbrowse_firm_build;
static int last_unit_or_struct;
static int browse_unit_width;
static int browse_firm_width;
static int last_race_filter;

// -------- define static function for the browser -------//

static void disp_human_unit_id(int recNo,int x,int y,int refreshFlag);
static void disp_human_firm_group(int recNo,int x,int y,int refreshFlag);
static void i_disp_nation_button( ButtonCustom *, int );
static void i_disp_race_spinner(int recNo,int x,int y, int x2, int y2, int refreshFlag);


// ----- begin of function ScenarioEditor::init_human_mode ------//
//
void ScenarioEditor::init_human_mode()
{
	unit_or_struct = 0;		// 0 = unit, 1 = structure
	brush_player_recno = 1;
	
	// -------- filter unit_id to display -------//

	human_race_filter = 0;		// all race
	human_unit_id_count = 0;
	human_unit_id_browse_recno = 1;
	human_unit_id_array = (short *)mem_add( unit_res.unit_info_count * sizeof(short) );
	collect_human_unit(human_race_filter);

	// ------- filter firm_build_id to display ------ //

	human_firm_group_count = 0;
	human_firm_group_browse_recno = 1;
	human_firm_group_array = (short *) mem_add( (race_res.race_count+firm_res.firm_group_count) * sizeof(short) );
	collect_human_firm(human_race_filter);
	spinner_race_group.vbrowse_enable_flag = 0;
}
// ----- end of function ScenarioEditor::init_human_mode ------//


// ----- begin of function ScenarioEditor::deinit_human_mode ------//
//
void ScenarioEditor::deinit_human_mode()
{
	mem_del( human_unit_id_array );
	mem_del( human_firm_group_array );
}
// ----- end of function ScenarioEditor::deinit_human_mode ------//


// ----- begin of function ScenarioEditor::disp_human_main ------//
//
void ScenarioEditor::disp_human_main(int refreshFlag)
{
	String str;

	if( refreshFlag == INFO_REPAINT )
	{
		// unit or structure button group

		button_unit_or_struct[0].create_text( INFO_X1+15, INFO_Y1+5, INFO_X1+110, INFO_Y1+25,
			text_editor.str_unit(), 0 ); //"Unit", 0 );
		button_unit_or_struct[1].create_text( INFO_X1+115, INFO_Y1+5, INFO_X1+210, INFO_Y1+25,
			text_editor.str_structure(), 0 ); //"Structure", 0 );
	}

	font_zoom.put( INFO_X1+10, INFO_Y2-28, text_editor.str_double_left_add(), 0, INFO_X2-5);
	font_zoom.put( INFO_X1+10, INFO_Y2-14, text_editor.str_double_right_del(), 0, INFO_X2-5);

	button_unit_or_struct.paint(unit_or_struct);

	{
		// player number button group

		for( int nationRecno = 0; nationRecno <= MAX_NATION; ++nationRecno )
		{
			if( refreshFlag == INFO_REPAINT )
			{
				button_player_recno[nationRecno].create( INFO_X1+16+nationRecno*25, INFO_Y1+30,
					INFO_X1+16+(nationRecno+1)*25-2, INFO_Y1+57, i_disp_nation_button,
					ButtonCustomPara(NULL, nationRecno), 0 );
			}
			button_player_recno[nationRecno].visible_flag = 
				button_player_recno[nationRecno].enable_flag = 
				!nationRecno || !nation_array.is_deleted(nationRecno);		// nation 0 always enable
		}
	}

	if( brush_player_recno && nation_array.is_deleted(brush_player_recno) )
	{
		brush_player_recno = 0;			// set to nation 0 if that nation is deleted
	}
	button_player_recno.paint(brush_player_recno);
	
	// display browser 

	if( refreshFlag == INFO_REPAINT || last_race_filter != human_race_filter )
	{
		refreshFlag = INFO_REPAINT;
		last_race_filter = human_race_filter;
		collect_human_unit(human_race_filter);
		collect_human_firm(human_race_filter);
	}

	if( refreshFlag == INFO_REPAINT || last_unit_or_struct != unit_or_struct )
	{
		refreshFlag = INFO_REPAINT;
		last_unit_or_struct = unit_or_struct;
	}

	if( !unit_or_struct )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			// init unit browser
			vbrowse_unit_id.init( INFO_X1+5, INFO_Y1+80, INFO_X2-5, INFO_Y2-30,
				-1, 20, human_unit_id_count, disp_human_unit_id );
			vbrowse_unit_id.open(human_unit_id_browse_recno);
			browse_unit_width = vbrowse_unit_id.ix2 - vbrowse_unit_id.ix1 + 1;
		}
		vbrowse_unit_id.paint();
		vbrowse_unit_id.refresh();
	}
	else
	{
		if( refreshFlag == INFO_REPAINT )
		{
			// init firm browser
			vbrowse_firm_build.init( INFO_X1+5, INFO_Y1+80, INFO_X2-5, INFO_Y2-30,
				-1, 20, human_firm_group_count, disp_human_firm_group );
			vbrowse_firm_build.open(human_firm_group_browse_recno);
			browse_firm_width = vbrowse_firm_build.ix2 - vbrowse_firm_build.ix1 + 1;
		}
		vbrowse_firm_build.paint();
		vbrowse_firm_build.refresh();
	}

	// init race filter button

	if( refreshFlag == INFO_REPAINT )
	{
//		int raceLength = ((INFO_X2-4) - (INFO_X1+5) + 1) / (1+MAX_RACE);
//		char raceAbbr[2] = " ";
//		for( int r = 0; r <= MAX_RACE; ++r )
//		{
//			if( r > 0 )
//				raceAbbr[0] = race_res[r]->name[0];
//			button_race_group[r].create_text( INFO_X1+5+r*raceLength, INFO_Y1+58,
//				INFO_X1+5+(r+1)*raceLength-1, INFO_Y1+78, raceAbbr );
//		}
		spinner_race_group.create( INFO_X1+80, INFO_Y1+58, INFO_X1+210, INFO_Y1+78, 247,
			0, 1+MAX_RACE, i_disp_race_spinner, human_race_filter );
	}
	// button_race_group.paint(human_race_filter);
	spinner_race_group.paint(human_race_filter);
}
// ----- end of function ScenarioEditor::disp_human_main ------//


// ----- begin of function ScenarioEditor::detect_human_main ------//
//
void ScenarioEditor::detect_human_main()
{
	// detect interface area
	int rc;

	if (!spinner_race_group.vbrowse_enable_flag)
	{
		rc = button_unit_or_struct.detect();
		if( rc >= 0 )
			unit_or_struct = rc;

		rc = button_player_recno.detect();
		if( rc >= 0 )
			brush_player_recno = rc;

		if( !unit_or_struct )
		{
			rc = vbrowse_unit_id.detect();
			// ####### begin Gilbert 22/2 ########//
	//		if( rc > 0 )
	//			human_unit_id_browse_recno = rc;
			human_unit_id_browse_recno = vbrowse_unit_id.recno();
			// ####### end Gilbert 22/2 ########//
		}
		else
		{
			// ####### begin Gilbert 22/2 ########//
			rc = vbrowse_firm_build.detect();
	//		if( rc > 0 )
	//			human_firm_group_browse_recno = rc;
			human_firm_group_browse_recno = vbrowse_firm_build.recno();
			// ####### end Gilbert 22/2 ########//
		}
	}

	//	rc = button_race_group.detect();
	rc = spinner_race_group.detect();
	if( rc >= 0 )
	{
		human_race_filter = rc;
		return;
	}
}
// ----- end of function ScenarioEditor::detect_human_main ------//


// ----- begin of function ScenarioEditor::disp_human_view ------//
//
void ScenarioEditor::disp_human_view()
{
	if( power.command_id )
		return;

	int drawMarker = 0;
	int validAction = 0;
	int markerX1, markerY1, markerX2, markerY2;

	int xLoc, yLoc;
	char mobileType;

	if( mouse.in_area(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) )
	{
		// destruction marker if there is a human

		Location *locPtr = power.test_detect(mouse.cur_x, mouse.cur_y, &mobileType, &xLoc, &yLoc);
		
		if( locPtr && locPtr->base_obj_recno() )
		{
			BaseObj *baseObj = base_obj_array[locPtr->base_obj_recno()];
			drawMarker = 1;
			validAction = 1;
			markerX1 = baseObj->obj_loc_x1();
			markerY1 = baseObj->obj_loc_y1();
			markerX2 = baseObj->obj_loc_x2();
			markerY2 = baseObj->obj_loc_y2();
		}

		// construction marker

		else if( world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
		{
			locPtr = world.get_loc( xLoc, yLoc );

			if( unit_or_struct == 0 && !vbrowse_unit_id.none_record )
			{
				// put unit human_unit_id_array[human_unit_id_browse_recno-1]
				// on (xLoc, yLoc)
				int unitId = human_unit_id_array[human_unit_id_browse_recno-1];
				UnitInfo *unitInfo = unit_res[unitId];
				SpriteInfo *spriteInfo = sprite_res[unitInfo->sprite_id];
				if( world.check_unit_space(xLoc, yLoc, 
					xLoc+spriteInfo->loc_width-1, yLoc+spriteInfo->loc_height-1, unitInfo->mobile_type) )
				{
					validAction = 1;
				}
				markerX1 = xLoc;
				markerY1 = yLoc;
				markerX2 = xLoc + spriteInfo->loc_width -1;
				markerY2 = yLoc + spriteInfo->loc_height -1;
				drawMarker = markerX2 < MAX_WORLD_X_LOC && markerY2 < MAX_WORLD_Y_LOC;
			}
			else if( unit_or_struct == 1 && !vbrowse_firm_build.none_record )
			{
				err_when( human_firm_group_array[human_firm_group_browse_recno-1] == 0 );
				if( human_firm_group_array[human_firm_group_browse_recno-1] < 0 )
				{
					// put town of race -human_firm_group_array[human_firm_group_browse_recno]
					// (xLoc, yLoc)

					if( world.can_build_town(xLoc, yLoc, 0) )
					{
						validAction = 1;
					}
					markerX1 = xLoc;
					markerY1 = yLoc;
					markerX2 = xLoc + STD_TOWN_LOC_WIDTH - 1;
					markerY2 = yLoc + STD_TOWN_LOC_HEIGHT - 1;
					drawMarker = markerX2 < MAX_WORLD_X_LOC && markerY2 < MAX_WORLD_Y_LOC;
				}
				else
				{
					// put firm group human_firm_group_array[human_firm_group_browse_recno]
					// on (xLoc, yLoc)

					FirmGroup *firmGroup = firm_res.get_group(human_firm_group_array[human_firm_group_browse_recno-1]);
					if( world.can_build_firm( xLoc, yLoc, firmGroup->firm_id, 0) )
					{
						validAction = 1;
					}
					markerX1 = xLoc;
					markerY1 = yLoc;
					markerX2 = xLoc + firm_res[firmGroup->firm_id]->loc_width - 1;
					markerY2 = yLoc + firm_res[firmGroup->firm_id]->loc_height - 1;
					drawMarker = markerX2 < MAX_WORLD_X_LOC && markerY2 < MAX_WORLD_Y_LOC;
				}
			}
		}
	}

	// ------ draw marker -------//

	if( drawMarker )
	{
		draw_marker( validAction, markerX1, markerY1, markerX2, markerY2 );
	}
}
// ----- end of function ScenarioEditor::disp_human_view ------//


// ----- begin of function ScenarioEditor::detect_human_view ------//
//
int ScenarioEditor::detect_human_view()
{
	if( power.command_id )
		return 0;

	// detect map area

	if( mouse.double_click( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0 ) )	// left click
	{
		int xLoc, yLoc;
		if( world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0),
			&xLoc, &yLoc, NULL) )
		{
			if( unit_or_struct == 0 && !vbrowse_unit_id.none_record )
			{
				// put unit human_unit_id_array[human_unit_id_browse_recno-1]
				// on (xLoc, yLoc)
				int unitId = human_unit_id_array[human_unit_id_browse_recno-1];
				UnitInfo *unitInfo = unit_res[unitId];
				SpriteInfo *spriteInfo = sprite_res[unitInfo->sprite_id];
				if( world.check_unit_space(xLoc, yLoc, 
					xLoc+spriteInfo->loc_width-1, yLoc+spriteInfo->loc_height-1, unitInfo->mobile_type) )
				{
					int unitRecno = unit_array.add_unit( unitId, brush_player_recno, RANK_SOLDIER, 100, xLoc, yLoc );

					if( unitRecno && unitId == UNIT_WAGON )
					{
						err_when( !human_race_filter );
						UnitWagon* unitPtr = (UnitWagon *)unit_array[unitRecno];
						unitPtr->race_id = human_race_filter;
						unitPtr->set_pop( MAX_WAGON_POPULATION );
					}
				}
			}
			else if( unit_or_struct == 1 && !vbrowse_firm_build.none_record )
			{
				err_when( human_firm_group_array[human_firm_group_browse_recno] == 0 );
				if( human_firm_group_array[human_firm_group_browse_recno-1] < 0 )
				{
					// put town of race -human_firm_group_array[human_firm_group_browse_recno]
					// (xLoc, yLoc)

					if( world.can_build_town(xLoc, yLoc, 0) )
					{
						int raceId = -human_firm_group_array[human_firm_group_browse_recno-1];
						int townRecno = town_array.generate_town(brush_player_recno, raceId, xLoc, yLoc);
						if( townRecno )
						{
							Town *townPtr = town_array[townRecno];
							townPtr->construction_completion_percent = 100;
							townPtr->under_construction = 0;

							// init_pop
							if( brush_player_recno )
								townPtr->init_pop( 0, 100, 0, 1 );
							else
								townPtr->init_pop( 0, town_array.independent_town_resistance(), 0, 1 );

							townPtr->auto_set_layout();
						}
					}
				}
				else
				{
					// put firm group human_firm_group_array[human_firm_group_browse_recno-1]
					// on (xLoc, yLoc)

					FirmGroup *firmGroup = firm_res.get_group(human_firm_group_array[human_firm_group_browse_recno-1]);
					if( world.can_build_firm( xLoc, yLoc, firmGroup->firm_id, 0) )
					{
						if (brush_player_recno == 0)
						{
							box.msg( text_editor.str_no_independent() ); // "Cannot add any Independent structures" );
						}
						else
						{
							if( firmGroup->firm_id == FIRM_BASE )		// if it's a seat of power, give the nation a scroll of power first
								god_res[firmGroup->race_id]->enable_know(brush_player_recno);

							firm_array.generate_firm( xLoc, yLoc, brush_player_recno, firmGroup->firm_id, firmGroup->race_code );
						}
					}
				}
			}
		}
		return 1;
	}

	if( mouse.double_click( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1) )	// right click to remove
	{
		int xLoc, yLoc;
		char mobileType;
		Location* locPtr = power.test_detect(mouse.click_x(1), mouse.click_y(1), &mobileType, &xLoc, &yLoc );
		if( locPtr )
		{
			if( locPtr->unit_recno(mobileType) )		// remove unit
			{
				int unitRecno = locPtr->unit_recno(mobileType);
				if( !unit_array.is_deleted(unitRecno) )
				{
					unit_array[unitRecno]->hit_points = 0.0f;
				}
			}
			else if( locPtr->is_firm() )			// remove firm
			{
				int firmRecno = locPtr->firm_recno();
				if( !firm_array.is_deleted(firmRecno) )
				{
					firm_array.del_firm( firmRecno );
				}
			}
			else if( locPtr->is_town() )			// remove town
			{
				int townRecno = locPtr->town_recno();
				if( !town_array.is_deleted(townRecno) )
				{
					town_array.del_town(townRecno);
				}
			}
		}
		return 1;
	}

	return 0;
}
// ----- end of function ScenarioEditor::detect_human_view ------//


// ----- begin of function ScenarioEditor::collect_human_unit ------//
//
void ScenarioEditor::collect_human_unit( int raceId )
{
	// save old selected unitId
	int oldSelectedUnitId = 0;
	if( human_unit_id_browse_recno >= 1 && human_unit_id_browse_recno <= human_unit_id_count )
	{
		oldSelectedUnitId = human_unit_id_array[human_unit_id_browse_recno-1];
	}
	human_unit_id_browse_recno = 1;		// reset browser recno

	human_unit_id_count = 0;
	for(int i = 1; i <= unit_res.unit_info_count; ++i )
	{
		UnitInfo *unitInfo = unit_res[i];

		// human only
		if( unitInfo->class_info.monster_side )
			continue;

		switch( unitInfo->unit_class )
		{
		case UNIT_CLASS_HUMAN:
		case UNIT_CLASS_CARAVAN:
		case UNIT_CLASS_WEAPON:
		case UNIT_CLASS_SHIP:
			if( !raceId || unitInfo->race_id == raceId )
			{
				human_unit_id_array[human_unit_id_count++] = i;

				if( i == oldSelectedUnitId )
					human_unit_id_browse_recno = human_unit_id_count;	// after human_unit_id_count incremented
			}
			break;

		case UNIT_CLASS_WAGON:
			if( raceId )
			{
				human_unit_id_array[human_unit_id_count++] = i;

				if( i == oldSelectedUnitId )
					human_unit_id_browse_recno = human_unit_id_count;	// after human_unit_id_count incremented
			}
			break;

		case UNIT_CLASS_GOD:
			// don't add god, as it disappear immediately
			break;

		default:
			err_here();
		}
	}
}
// ----- end of function ScenarioEditor::collect_human_unit ------//


// ----- begin of function ScenarioEditor::collect_human_firm ------//
//
void ScenarioEditor::collect_human_firm( int raceId )
{
	int i;

	// save old selected firm Group
	int oldSelectedFirmGroup = 0;
	if( human_firm_group_browse_recno >= 1 && human_firm_group_browse_recno <= human_firm_group_count )
	{
		oldSelectedFirmGroup = human_firm_group_array[human_firm_group_browse_recno-1];
	}
	human_firm_group_browse_recno = 1;		// reset browser recno

	human_firm_group_count = 0;

	// every race, add a town
	for( i = 1; i <= race_res.race_count; ++i )
	{
		if( !raceId || i == raceId )
		{
			human_firm_group_array[human_firm_group_count++] = -i; // insert record for towns 
			if( oldSelectedFirmGroup == -i )
			{
				human_firm_group_browse_recno = human_firm_group_count;	// increased human_firm_group_count
			}
		}
	}

	for( i = 1; i <= firm_res.firm_group_count; ++i )
	{
		FirmGroup *firmGroup = firm_res.get_group(i);
		if( firmGroup->race_id >= 0 && (!raceId || firmGroup->race_id == raceId) )
		{
			human_firm_group_array[human_firm_group_count++] = i;
			if( oldSelectedFirmGroup == i )
			{
				human_firm_group_browse_recno = human_firm_group_count;	// increased human_firm_group_count
			}
		}
	}
}
// ----- end of function ScenarioEditor::collect_human_firm ------//


// ----- begin of static function disp_human_unit_id ------//
//
static void disp_human_unit_id(int recNo,int x,int y,int refreshFlag)
{
	if( recNo > 0 && recNo <= scenario_editor.human_unit_id_count )
		font_san.put( x+6, y+2, unit_res[scenario_editor.human_unit_id_array[recNo-1]]->name, 0,
			x+browse_unit_width-1);
}
// ----- end of static function disp_human_unit_id ------//


// ----- begin of static function disp_human_firm_group ------//
//
static void disp_human_firm_group(int recNo,int x,int y,int refreshFlag)
{
	if( recNo > 0 && recNo <= scenario_editor.human_firm_group_count )
	{
		int firmGroupId = scenario_editor.human_firm_group_array[recNo-1];
		err_when( firmGroupId == 0 );

		String str;

		if( firmGroupId > 0 )		// firm name
		{
			FirmGroup *firmGroup = firm_res.get_group( scenario_editor.human_firm_group_array[recNo-1] );
#if(defined(GERMAN))
			// GERMAN : format is <firm> <race>
			FirmInfo *firmInfo = firm_res[firmGroup->firm_id];
			int firmBuildId = firmInfo->get_build_id(firmGroup->race_code);
			if( firmBuildId )
				str = firm_res.get_build(firmBuildId)->name;
			else
				str = firmInfo->name;
			if (firmGroup->race_id)
			{
				str += " ";
				str += race_res[firmGroup->race_id]->adjective;
			}
#else
			// US : format is <race> <firm>
			if (firmGroup->race_id)
				str = race_res[firmGroup->race_id]->adjective;
			else
				str = text_editor.str_human(); //"Human";
			FirmInfo *firmInfo = firm_res[firmGroup->firm_id];
			int firmBuildId = firmInfo->get_build_id(firmGroup->race_code);
			if( firmBuildId )
			{
				str += " ";
				str += firm_res.get_build(firmBuildId)->name;
			}
			else
			{
				str += " ";
				str += firmInfo->name;
			}
#endif
			font_san.put( x+6, y+2, str, 0, x+browse_firm_width-1 );
		}
		else if( firmGroupId < 0 )		// town
		{
			// town of race_id (-firmBuildId)
			// str = race_res[-firmGroupId]->adjective;
			// str += " town";
			str = text_editor.str_race_town( race_res[-firmGroupId]->adjective );
			font_san.put( x+6, y+2, str, 0, x+browse_firm_width-1 );
		}
	}
}
// ----- end of static function disp_human_firm_group ------//


// ----- begin of static function i_disp_nation_button ------//
//
static void i_disp_nation_button( ButtonCustom *button, int )
{
	int nationRecno = button->custom_para.value;
	char nationColor = nationRecno && !nation_array.is_deleted(nationRecno) ? 
		nation_array.nation_color_array[nationRecno] : nation_array.nation_color_array[0];

	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( x1, y1, x2, y2 );
		vga.active_buf->bar_fast( x1+6, y1+6, x2-6, y2-6, nationColor );
	}
	else
	{
		vga.active_buf->d3_panel_down( x1, y1, x2, y2 );
		vga.active_buf->bar_fast( x1+7, y1+7, x2-5, y2-5, nationColor );
	}
}
// ----- end of static function i_disp_nation_button ------//

// ----- begin of static function i_disp_race_spinner ------//
static void i_disp_race_spinner(int recNo,int x,int y, int x2, int y2, int refreshFlag)
{
	if( recNo == 0 )
	{
		font_snds.put( x+6, y, 
			text_editor.str_all(), // "All",
			0, x2);
	}
	else
	{
		font_snds.put( x+6, y, race_res[recNo]->name, 0, x2);
	}
}
// ----- end of static function i_disp_race_spinner ------//

