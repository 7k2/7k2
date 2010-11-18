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

// Filename    : OSEDIT_F.CPP
// Description : Scenario editor monster

// update any to to OSEDIT_H.CPP as well


#include <OSEDITOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBOX.H>
#include <OBUTTON.H>
#include <OBUTTCUS.H>
#include <OSPIN_S.H>
#include <OVBROWIF.H>
#include <OGAME.H>
#include <OPOWER.H>
#include <OINFO.H>
#include <OUNITRES.H>
#include <OFIRMRES.H>
#include <OMONSRES.H>
#include <OWORLDMT.H>
#include <ONATIONA.H>
#include <OT_SEDIT.H>


// ------- define static variable --------------//

static ButtonGroup button_unit_or_struct(2);
static ButtonCustomGroup button_player_recno(1+MAX_NATION);
// static ButtonGroup button_race_group(1+MAX_MONSTER_TYPE);

static SpinnerSmall spinner_race_group;
static VBrowseIF vbrowse_unit_id;
static VBrowseIF vbrowse_firm_build;
static int last_unit_or_struct;
static int browse_unit_width;
static int browse_firm_width;
static int last_race_filter;

// -------- define static function for the browser -------//

static void disp_monster_unit_id(int recNo,int x,int y,int refreshFlag);
static void disp_monster_firm_group(int recNo,int x,int y,int refreshFlag);
static void i_disp_nation_button( ButtonCustom *, int );
static void i_disp_race_spinner(int recNo,int x,int y, int x2, int y2, int refreshFlag);


// ----- begin of function ScenarioEditor::init_monster_mode ------//
//
void ScenarioEditor::init_monster_mode()
{
	unit_or_struct = 0;		// 0 = unit, 1 = structure
	brush_player_recno = 1;

	// -------- filter unit_id to display -------//

	monster_race_filter = 0;	// all race
	monster_unit_id_count = 0;
	monster_unit_id_browse_recno = 1;
	monster_unit_id_array = (short *)mem_add( unit_res.unit_info_count * sizeof(short) );
	collect_monster_unit(monster_race_filter);

	// ------- filter firm_build_id to display ------ //

	monster_firm_group_count = 0;
	monster_firm_group_browse_recno = 1;
	monster_firm_group_array = (short *) mem_add( firm_res.firm_group_count * sizeof(short) );
	collect_monster_firm(monster_race_filter);
	spinner_race_group.vbrowse_enable_flag = 0;
}
// ----- end of function ScenarioEditor::init_monster_mode ------//


// ----- begin of function ScenarioEditor::deinit_monster_mode ------//
//
void ScenarioEditor::deinit_monster_mode()
{
	mem_del( monster_unit_id_array );
	mem_del( monster_firm_group_array );
}
// ----- end of function ScenarioEditor::deinit_monster_mode ------//


// ----- begin of function ScenarioEditor::disp_monster_main ------//
//
void ScenarioEditor::disp_monster_main(int refreshFlag)
{
	String str;

	if( refreshFlag == INFO_REPAINT )
	{
		// unit or structure button group

		button_unit_or_struct[0].create_text( INFO_X1+15, INFO_Y1+5, INFO_X1+110, INFO_Y1+25,
			text_editor.str_unit(), 0 ); // "Unit", 0 );
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

	if( refreshFlag == INFO_REPAINT || last_race_filter != monster_race_filter )
	{
		refreshFlag = INFO_REPAINT;
		last_race_filter = monster_race_filter;
		collect_monster_unit(monster_race_filter);
		collect_monster_firm(monster_race_filter);
	}

	if( refreshFlag == INFO_REPAINT || last_unit_or_struct != unit_or_struct )
	{
		refreshFlag = INFO_REPAINT;
		last_unit_or_struct = unit_or_struct;
	}

	// display browser 

	if( !unit_or_struct )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			// init unit browser
			vbrowse_unit_id.init( INFO_X1+5, INFO_Y1+80, INFO_X2-5, INFO_Y2-30,
				-1, 20, monster_unit_id_count, disp_monster_unit_id );
			vbrowse_unit_id.open(monster_unit_id_browse_recno);
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
				-1, 20, monster_firm_group_count, disp_monster_firm_group );
			vbrowse_firm_build.open(monster_firm_group_browse_recno);
			browse_firm_width = vbrowse_firm_build.ix2 - vbrowse_firm_build.ix1 + 1;
		}
		vbrowse_firm_build.paint();
		vbrowse_firm_build.refresh();
	}

	// init race filter button

	if( refreshFlag == INFO_REPAINT )
	{
//		int raceLength = ((INFO_X2-4) - (INFO_X1+5) + 1) / (1+MAX_MONSTER_TYPE);
//		char raceAbbr[2] = " ";
//		for( int r = 0; r <= MAX_MONSTER_TYPE; ++r )
//		{
//			if( r > 0 ) 
//				raceAbbr[0] = monster_res[r]->name[0];
//			button_race_group[r].create_text( INFO_X1+5+r*raceLength, INFO_Y1+58,
//				INFO_X1+5+(r+1)*raceLength-1, INFO_Y1+78, raceAbbr );
//		}
		spinner_race_group.create( INFO_X1+80, INFO_Y1+58, INFO_X1+210, INFO_Y1+78, 200,//146,
			0, 1+MAX_MONSTER_TYPE, i_disp_race_spinner, -monster_race_filter );
	}
	// button_race_group.paint(-monster_race_filter);
	spinner_race_group.paint(-monster_race_filter);

}
// ----- end of function ScenarioEditor::disp_monster_main ------//


// ----- begin of function ScenarioEditor::detect_monster_main ------//
//
void ScenarioEditor::detect_monster_main()
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
			// ###### begin gilbert 22/2 ########//
			//if( rc > 0 )
			//	monster_unit_id_browse_recno = rc;
			monster_unit_id_browse_recno = vbrowse_unit_id.recno();
			// ###### end gilbert 22/2 ########//
		}
		else
		{
			rc = vbrowse_firm_build.detect();
			// ###### begin Gilbert 22/2 #########//
			// if( rc > 0 )
			//		monster_firm_group_browse_recno = rc;
			monster_firm_group_browse_recno = vbrowse_firm_build.recno();
			// ###### end Gilbert 22/2 #########//
		}
	}

	//	rc = button_race_group.detect();
	rc = spinner_race_group.detect();
	if( rc >= 0 )
	{
		monster_race_filter = -rc;
		return;
	}
}
// ----- end of function ScenarioEditor::detect_monster_main ------//


// ----- begin of function ScenarioEditor::disp_monster_view ------//
//
void ScenarioEditor::disp_monster_view()
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
				// put unit monster_unit_id_array[monster_unit_id_browse_recno-1]
				// on (xLoc, yLoc)
				int unitId = monster_unit_id_array[monster_unit_id_browse_recno-1];
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
				err_when( monster_firm_group_array[monster_firm_group_browse_recno-1] == 0 );
				// put firm group monster_firm_group_array[monster_firm_group_browse_recno]
				// on (xLoc, yLoc)

				FirmGroup *firmGroup = firm_res.get_group(monster_firm_group_array[monster_firm_group_browse_recno-1]);
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

	// ------ draw marker -------//

	if( drawMarker )
	{
		draw_marker( validAction, markerX1, markerY1, markerX2, markerY2 );
	}
}
// ----- end of function ScenarioEditor::disp_monster_view ------//


// ----- begin of function ScenarioEditor::detect_monster_view ------//
//
int ScenarioEditor::detect_monster_view()
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
				// put unit monster_unit_id_array[monster_unit_id_browse_recno-1]
				// on (xLoc, yLoc)
				int unitId = monster_unit_id_array[monster_unit_id_browse_recno-1];
				UnitInfo *unitInfo = unit_res[unitId];
				SpriteInfo *spriteInfo = sprite_res[unitInfo->sprite_id];
				if( world.check_unit_space(xLoc, yLoc, 
					xLoc+spriteInfo->loc_width-1, yLoc+spriteInfo->loc_height-1, unitInfo->mobile_type) )
				{
					unit_array.add_unit( unitId, brush_player_recno, RANK_SOLDIER, 100, xLoc, yLoc );
				}
			}
			else if( unit_or_struct == 1 && !vbrowse_firm_build.none_record )
			{
				err_when( monster_firm_group_array[monster_firm_group_browse_recno-1] == 0 );
				// put firm group monster_firm_group_array[monster_firm_group_browse_recno]
				// on (xLoc, yLoc)

				FirmGroup *firmGroup = firm_res.get_group(monster_firm_group_array[monster_firm_group_browse_recno-1]);
				if( world.can_build_firm( xLoc, yLoc, firmGroup->firm_id, 0) )
				{
					if( brush_player_recno && nation_array[brush_player_recno]->is_human() 
						&& firm_res[firmGroup->firm_id]->setup_live_points_cost > 0 )
					{
						box.msg( text_editor.str_no_human() ); // "Human Kingdom cannot possess Fryhtan structure" );
					}
					else if (brush_player_recno == 0 && firmGroup->firm_id != FIRM_LAIR)
					{
						box.msg( text_editor.str_no_independent() ); // "Cannot add any Independent structures" );
					}
					else
					{
						firm_array.generate_firm( xLoc, yLoc, brush_player_recno, firmGroup->firm_id, firmGroup->race_code );
					}
				}
			}
		}
		return 1;
	}

	// ------ remove object -------//

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
					unit_array[unitRecno]->hit_points = 0.0f;
			}
			else if( locPtr->is_firm() )				// remove firm
			{
				int firmRecno = locPtr->firm_recno();
				if( !firm_array.is_deleted(firmRecno) )
				{
					firm_array.del_firm( firmRecno );
				}
			}
			else if( locPtr->is_town() )				// remove town
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
// ----- end of function ScenarioEditor::detect_monster_view ------//


// ----- begin of function ScenarioEditor::collect_monster_unit ------//
//
void ScenarioEditor::collect_monster_unit(int raceId)
{
	// save old selected unitId
	int oldSelectedUnitId = 0;
	if( monster_unit_id_browse_recno >= 1 && monster_unit_id_browse_recno <= monster_unit_id_count )
	{
		oldSelectedUnitId = monster_unit_id_array[monster_unit_id_browse_recno-1];
	}
	monster_unit_id_browse_recno = 1;		// reset browser recno

	monster_unit_id_count = 0;
	for(int i = 1; i <= unit_res.unit_info_count; ++i )
	{
		UnitInfo *unitInfo = unit_res[i];
		// #### begin Gilbert 24/3 ########//
		if( !unitInfo->class_info.monster_side )
			continue;
		
		switch( unitInfo->unit_class )
		{
		case UNIT_CLASS_MONSTER:
			if( !raceId || unitInfo->race_id == raceId )
			{	
				monster_unit_id_array[monster_unit_id_count++] = i;
				if( i == oldSelectedUnitId )
					monster_unit_id_browse_recno = monster_unit_id_count;	// after monster_unit_id_count incremented
			}
			break;
		
		case UNIT_CLASS_MONS_WEAPON:
			if( !raceId || MONSTER_EZPINEZ == -raceId )
			{	
				monster_unit_id_array[monster_unit_id_count++] = i;
				if( i == oldSelectedUnitId )
					monster_unit_id_browse_recno = monster_unit_id_count;	// after monster_unit_id_count incremented
			}
			break;
		
		case UNIT_CLASS_INSECT:
			if( !raceId || MONSTER_BREGMA == -raceId )
			{	
				monster_unit_id_array[monster_unit_id_count++] = i;
				if( i == oldSelectedUnitId )
					monster_unit_id_browse_recno = monster_unit_id_count;	// after monster_unit_id_count incremented
			}
			break;

		case UNIT_CLASS_ANIMAL:
			if( !raceId || MONSTER_KERASSOS == -raceId )
			{	
				monster_unit_id_array[monster_unit_id_count++] = i;
				if( i == oldSelectedUnitId )
					monster_unit_id_browse_recno = monster_unit_id_count;	// after monster_unit_id_count incremented
			}
			break;

		default:
			if( !raceId )
			{	
				monster_unit_id_array[monster_unit_id_count++] = i;
				if( i == oldSelectedUnitId )
					monster_unit_id_browse_recno = monster_unit_id_count;	// after monster_unit_id_count incremented
			}
		}
		// #### end Gilbert 24/3 ########//
	}
}
// ----- end of function ScenarioEditor::collect_monster_unit ------//


// ----- begin of function ScenarioEditor::collect_monster_firm ------//
//
void ScenarioEditor::collect_monster_firm(int raceId)
{
	// save old selected firm Group
	int oldSelectedFirmGroup = 0;
	if( monster_firm_group_browse_recno >= 1 && monster_firm_group_browse_recno <= monster_firm_group_count )
	{
		oldSelectedFirmGroup = monster_firm_group_array[monster_firm_group_browse_recno-1];
	}
	monster_firm_group_browse_recno = 1;		// reset browser recno

	monster_firm_group_count = 0;
	for( int i = 1; i <= firm_res.firm_group_count; ++i )
	{
		FirmGroup *firmGroup = firm_res.get_group(i);

		if( firmGroup->race_id < 0 && (!raceId || firmGroup->race_id == raceId) )
		{
			monster_firm_group_array[monster_firm_group_count++] = i;
			if( oldSelectedFirmGroup == i )
			{
				monster_firm_group_browse_recno = monster_firm_group_count;	// increased monster_firm_group_count
			}
		}
	}
}
// ----- end of function ScenarioEditor::collect_monster_firm ------//


// ----- begin of static function disp_monster_unit_id ------//
//
static void disp_monster_unit_id(int recNo,int x,int y,int refreshFlag)
{
	if( recNo > 0 && recNo <= scenario_editor.monster_unit_id_count )
		font_san.put( x+6, y+2, unit_res[scenario_editor.monster_unit_id_array[recNo-1]]->name, 0,
			x+browse_unit_width-1 );
}
// ----- end of static function disp_monster_unit_id ------//


// ----- begin of static function disp_monster_firm_group ------//
//
static void disp_monster_firm_group(int recNo,int x,int y,int refreshFlag)
{
	if( recNo > 0 && recNo <= scenario_editor.monster_firm_group_count )
	{
		String str;
		FirmGroup *firmGroup = firm_res.get_group( scenario_editor.monster_firm_group_array[recNo-1] );
#if(0)
		// format is <firm> <race>
		FirmInfo *firmInfo = firm_res[firmGroup->firm_id];
		int firmBuildId = firmInfo->get_build_id(firmGroup->race_code);
		if( firmBuildId )
			str = firm_res.get_build(firmBuildId)->name;
		else
			str += firmInfo->name;
		if( firmGroup->firm_id != FIRM_LAIR )		// name in firm build already has monster name
		{
			str += " ";
			str += monster_res[-firmGroup->race_id]->name;
		}
#else
		// US and GERMAN : format is <race> <firm>
		if( firmGroup->firm_id != FIRM_LAIR )		// name in firm build already has monster name
			str  = monster_res[-firmGroup->race_id]->name;
		else
			str  = "";
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

		font_san.put(x+6, y+2, str, 0, x+browse_firm_width-1 );
	}
}
// ----- end of static function disp_monster_firm_group ------//


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
			text_editor.str_all(), //"All",
			0, x2);
	}
	else
	{
		font_snds.put( x+6, y, monster_res[recNo]->name, 0, x2);
	}
}
// ----- end of static function i_disp_race_spinner ------//
