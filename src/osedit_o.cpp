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

// Filename    : OSEDIT_O.CPP
// Description : Scenario editor hero

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
#include <oherores.h>
#include <ot_sedit.h>

// ------- define static variable --------------//

static ButtonGroup button_inf_or_spu(2);
static ButtonCustomGroup button_player_recno(1+MAX_NATION);
static SpinnerSmall spinner_race_group;
static VBrowseIF vbrowse_hero_id;
static int last_inf_or_spu;
static int browse_hero_width;
static int last_race_filter;

// -------- define static function for the browser -------//

static void disp_hero_id(int recNo,int x,int y,int refreshFlag);
static void i_disp_nation_button( ButtonCustom *, int );
static void i_disp_race_spinner(int recNo,int x,int y, int x2, int y2, int refreshFlag);


// ----- begin of function ScenarioEditor::init_hero_mode ------//
//
void ScenarioEditor::init_hero_mode()
{
	inf_or_spu = 0;		// 0 = infantry, 1 = special unit
	brush_player_recno = 1;
	
	// -------- filter unit_id to display -------//

	hero_race_filter = 0;
	hero_id_count = 0;
	hero_id_browse_recno = 1;
	hero_id_array = (short *)mem_add( hero_res.hero_count * sizeof(short) );
	hero_unit_id_array = (short *)mem_add( hero_res.hero_count * sizeof(short) );
	
	collect_hero_unit(hero_race_filter);

	spinner_race_group.vbrowse_enable_flag = 0;
}
// ----- end of function ScenarioEditor::init_hero_mode ------//


// ----- begin of function ScenarioEditor::deinit_hero_mode ------//
//
void ScenarioEditor::deinit_hero_mode()
{
	mem_del( hero_id_array );
	mem_del( hero_unit_id_array );
}
// ----- end of function ScenarioEditor::deinit_hero_mode ------//


// ----- begin of function ScenarioEditor::disp_hero_main ------//
//
void ScenarioEditor::disp_hero_main(int refreshFlag)
{
	String str;

	if( refreshFlag == INFO_REPAINT )
	{
		// infantry or structure unit button group

		button_inf_or_spu[0].create_text( INFO_X1+15, INFO_Y1+5, INFO_X1+110, INFO_Y1+25,
			text_editor.str_hero_infantry(), 0 ); // "Infantry", 0 );
		button_inf_or_spu[1].create_text( INFO_X1+115, INFO_Y1+5, INFO_X1+210, INFO_Y1+25,
			text_editor.str_hero_spu(), 0 ); // "Special", 0 );
	}

	font_zoom.put( INFO_X1+10, INFO_Y2-28, text_editor.str_double_left_add(), 0, INFO_X2-5);
	font_zoom.put( INFO_X1+10, INFO_Y2-14, text_editor.str_double_right_del(), 0, INFO_X2-5);

	button_inf_or_spu.paint(inf_or_spu);

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

	if( brush_player_recno && nation_array.is_deleted(brush_player_recno) )
	{
		brush_player_recno = 0;			// set to nation 0 if that nation is deleted
	}
	button_player_recno.paint(brush_player_recno);
	
	// display browser 

	if( refreshFlag == INFO_REPAINT || last_race_filter != hero_race_filter )
	{
		refreshFlag = INFO_REPAINT;
		last_race_filter = hero_race_filter;
		collect_hero_unit(hero_race_filter);
	}

	if( refreshFlag == INFO_REPAINT || last_inf_or_spu != inf_or_spu )
	{
		refreshFlag = INFO_REPAINT;
		last_inf_or_spu = inf_or_spu;
		collect_hero_unit(hero_race_filter);
	}

	if( refreshFlag == INFO_REPAINT )
	{
		// init unit browser
		vbrowse_hero_id.init( INFO_X1+5, INFO_Y1+80, INFO_X2-5, INFO_Y2-30,
			-1, 20, hero_id_count, disp_hero_id );
		vbrowse_hero_id.open(hero_id_browse_recno);
		browse_hero_width = vbrowse_hero_id.ix2 - vbrowse_hero_id.ix1 + 1;
	}
	vbrowse_hero_id.paint();
	vbrowse_hero_id.refresh();

	// init race filter button
	if( refreshFlag == INFO_REPAINT )
	{
		spinner_race_group.create( INFO_X1+80, INFO_Y1+58, INFO_X1+210, INFO_Y1+78, 247,
			0, 1+MAX_RACE, i_disp_race_spinner, hero_race_filter );
	}
	spinner_race_group.paint(hero_race_filter);

}
// ----- end of function ScenarioEditor::disp_hero_main ------//


// ----- begin of function ScenarioEditor::detect_hero_main ------//
//
void ScenarioEditor::detect_hero_main()
{
	// detect interface area
	int rc;

	if (!spinner_race_group.vbrowse_enable_flag)
	{
		rc = button_inf_or_spu.detect();
		if( rc >= 0 )
			inf_or_spu = rc;
		
		rc = button_player_recno.detect();
		if( rc >= 0 )
			brush_player_recno = rc;

		rc = vbrowse_hero_id.detect();
		hero_id_browse_recno = vbrowse_hero_id.recno();
	}

	rc = spinner_race_group.detect();
	if( rc >= 0 )
	{
		hero_race_filter = rc;
		return;
	}
}
// ----- end of function ScenarioEditor::detect_hero_main ------//


// ----- begin of function ScenarioEditor::disp_hero_view ------//
//
void ScenarioEditor::disp_hero_view()
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

			if( !vbrowse_hero_id.none_record )
			{
			/*	//----- determine the unit id. of the hero -----//
				HeroInfo *heroInfo = hero_res[hero_id_browse_recno];
				int unitId;
				if( heroInfo->infantry_unit && heroInfo->special_unit )
				{
				//	if( misc.random(2)==0 )
						unitId = race_res[heroInfo->race_id]->infantry_unit_id;
				//	else
				//		unitId = race_res[heroInfo->race_id]->special_unit_id;
				}
				else if( heroInfo->infantry_unit )
					unitId = race_res[heroInfo->race_id]->infantry_unit_id;

				else if( heroInfo->special_unit )
					unitId = race_res[heroInfo->race_id]->special_unit_id;*/

				int unitId = hero_unit_id_array[hero_id_browse_recno-1];
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
		}
	}

	// ------ draw marker -------//

	if( drawMarker )
	{
		draw_marker( validAction, markerX1, markerY1, markerX2, markerY2 );
	}
}
// ----- end of function ScenarioEditor::disp_hero_view ------//


// ----- begin of function ScenarioEditor::detect_hero_view ------//
//
int ScenarioEditor::detect_hero_view()
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
			if( !vbrowse_hero_id.none_record )
			{
			/*	//----- determine the unit id. of the hero -----//
				HeroInfo *heroInfo = hero_res[hero_id_browse_recno];

				if ( heroInfo->appeared_flag != 0 )
				{
					box.msg( "Cannot add existing Heros" );
					return 0;
				}
				
				int unitId;
				if( heroInfo->infantry_unit && heroInfo->special_unit )
				{
				//	if( misc.random(2)==0 )
						unitId = race_res[heroInfo->race_id]->infantry_unit_id;
				//	else
				//		unitId = race_res[heroInfo->race_id]->special_unit_id;
				}
				else if( heroInfo->infantry_unit )
					unitId = race_res[heroInfo->race_id]->infantry_unit_id;

				else if( heroInfo->special_unit )
					unitId = race_res[heroInfo->race_id]->special_unit_id;*/

				if ( hero_res[hero_id_array[hero_id_browse_recno-1]]->appeared_flag != 0 )
				{
#if(defined(CHINESE) && defined(TRADITIONAL))
					box.msg( "ぃ睰瞷筁璣动" );
#elif(defined(CHINESE) && !defined(TRADITIONAL))
					box.msg( "不能添加已出现过的英雄" );
#else
					box.msg( "Cannot add existing Heros" );
#endif
					return 0;
				}

				int unitId = hero_unit_id_array[hero_id_browse_recno-1];
				UnitInfo *unitInfo = unit_res[unitId];
				SpriteInfo *spriteInfo = sprite_res[unitInfo->sprite_id];
				if( world.check_unit_space(xLoc, yLoc, 
					xLoc+spriteInfo->loc_width-1, yLoc+spriteInfo->loc_height-1, unitInfo->mobile_type) )
				{

						int unitRecno = hero_res.create_powerful_unit(xLoc, yLoc, brush_player_recno, unitId, 50, 100);
						if( !unitRecno )
							return 0;
						unit_array[unitRecno]->hero_id = hero_res[hero_id_array[hero_id_browse_recno-1]]->hero_id;
						err_when( hero_res[hero_id_array[hero_id_browse_recno-1]]->appeared_flag != 0 );
						hero_res[hero_id_array[hero_id_browse_recno-1]]->appeared_flag = 1;
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
// ----- end of function ScenarioEditor::detect_hero_view ------//


// ----- begin of function ScenarioEditor::collect_hero_unit ------//
//
void ScenarioEditor::collect_hero_unit( int raceId )
{
	// save old selected unitId
	int oldSelectedHeroId = 0;
	if( hero_id_browse_recno >= 1 && hero_id_browse_recno <= hero_id_count )
		oldSelectedHeroId = hero_id_array[hero_id_browse_recno-1];
	
	hero_id_browse_recno = 1;		// reset browser recno

	hero_id_count = 0;
	for(int i = 1; i <= hero_res.hero_count; ++i )
	{
		//----- determine the unit id. of the hero -----//
		HeroInfo *heroInfo = hero_res[i];

		if (heroInfo->race_id != raceId && raceId > 0)
			continue;

		int unitId;
		if( heroInfo->infantry_unit && heroInfo->special_unit )
		{
			if( !inf_or_spu )
				unitId = race_res[heroInfo->race_id]->infantry_unit_id;
			else
				unitId = race_res[heroInfo->race_id]->special_unit_id;
		}
		else if( heroInfo->infantry_unit )
			if (!inf_or_spu)
				unitId = race_res[heroInfo->race_id]->infantry_unit_id;
			else
				continue;

		else if( heroInfo->special_unit )
			if (inf_or_spu)
				unitId = race_res[heroInfo->race_id]->special_unit_id;
			else
				continue;
		else
			err_here();

		hero_unit_id_array[hero_id_count] = unitId;
		hero_id_array[hero_id_count] = i;
		hero_id_count ++;
		
		// set the browse_recno to currently selected hero
		if( i == oldSelectedHeroId )
			hero_id_browse_recno = hero_id_count;
	}
}
// ----- end of function ScenarioEditor::collect_hero_unit ------//


// ----- begin of static function disp_hero_unit_id ------//
//
static void disp_hero_id(int recNo,int x,int y,int refreshFlag)
{
	if( recNo > 0 && recNo <= scenario_editor.hero_id_count )
		font_san.put( x+6, y+2, hero_res[scenario_editor.hero_id_array[recNo-1]]->name, 0,
			x+browse_hero_width-1);
}
// ----- end of static function disp_hero_unit_id ------//


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

