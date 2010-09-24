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

// Filename    : OSEDIT_P.CPP
// Description : Scenario editor, player mode


#include <OSEDITOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OBLOB2W.H>
#include <OBITMAPW.H>
#include <OMOUSE.H>
#include <KEY.H>
#include <OSYS.H>
#include <OMUSIC.H>
#include <OVBROWIF.H>
#include <OBUTTON.H>
#include <OBUTTCUS.H>
#include <OGETA.H>
#include <OINFO.H>
#include <OGAME.H>
#include <ONATIONA.H>
#include <ONATION2.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OT_SEDIT.H>


// ----- define static variable ---------//

static VBrowseIF browse_nation;
static int last_brush_player;
static Button button_cash_up, button_cash_down;
static Button button_food_up, button_food_down;
static Button button_reput_up, button_reput_down;
static Button button_live_up, button_live_down;
static ButtonGroup button_adjustment(3);		// fine, normal, coarse
static ButtonGroup button_player_type(2);



// ----- declare static function ---------//

static void disp_nation_rec(int recNo,int x,int y,int refreshFlag);
static void disp_color_button(ButtonCustom *, int );

// ----- begin of function ScenarioEditor::init_player_mode ------//
//
void ScenarioEditor::init_player_mode()
{
}
// ----- end of function ScenarioEditor::init_player_mode ------//


// ----- begin of function ScenarioEditor::deinit_player_mode ------//
//
void ScenarioEditor::deinit_player_mode()
{
}
// ----- end of function ScenarioEditor::deinit_player_mode ------//


// ----- begin of function ScenarioEditor::disp_players_main ------//
//
void ScenarioEditor::disp_players_main(int refreshFlag)
{
	// ------- display scroll area ----------//

	if( refreshFlag == INFO_REPAINT )
	{
		browse_nation.init( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+168,
			-1, 32, MAX_NATION, disp_nation_rec );
		browse_nation.open(brush_player_recno);
	}
	browse_nation.paint();
	// ######## begin Gilbert 22/2 #######//
	// browse_nation.refresh(brush_player_recno);
	browse_nation.refresh();
	// ######## end Gilbert 22/2 #######//

	// ------- display on interface area ---------//

	if( refreshFlag == INFO_REPAINT || last_brush_player != brush_player_recno )
	{
		refreshFlag = INFO_REPAINT;
		last_brush_player = brush_player_recno;
	}

	// ------ display king name -------//

	int y = INFO_Y1+170;

//	if( !nation_array.is_deleted(brush_player_recno) )
//		font_whbl.put( font_whbl.put( INFO_X1+20, y, text_editor.str_king_name()) // "King Name" )
//		+10, y, nation_array[brush_player_recno]->king_name() ) ;

	if( !nation_array.is_deleted(brush_player_recno) )
	{
		vga.active_buf->d3_panel_up( INFO_X1+5, y, INFO_X2-5, y+8+font_cara_w.max_font_height );
		font_cara_w.center_put( INFO_X1+5, y, INFO_X2-5, y+8+font_cara_w.max_font_height,
			nation_array[brush_player_recno]->nation_name() );
	}

	y+=30;

	// ------- treasure, food, reputation --------//
	if( refreshFlag == INFO_REPAINT )
	{
		button_cash_up.create_text(    INFO_X1+140, y,    INFO_X1+150, y+9,  "+" );
		button_cash_down.create_text(  INFO_X1+140, y+10, INFO_X1+150, y+19, "-" );
		button_food_up.create_text(    INFO_X1+140, y+22, INFO_X1+150, y+31, "+" );
		button_food_down.create_text(  INFO_X1+140, y+32, INFO_X1+150, y+41, "-" );
		button_reput_up.create_text(   INFO_X1+140, y+44, INFO_X1+150, y+53, "+" );
		button_reput_down.create_text( INFO_X1+140, y+54, INFO_X1+150, y+63, "-" );
		button_live_up.create_text(    INFO_X1+140, y+66, INFO_X1+150, y+75, "+" );
		button_live_down.create_text(  INFO_X1+140, y+76, INFO_X1+150, y+85, "-" );

//		button_adjustment[0].create_text( INFO_X1+152, y, INFO_X1+214, y+18, text_editor.str_fine_coarse(0) ); // "Fine" );
//		button_adjustment[1].create_text( INFO_X1+152, y+20, INFO_X1+214, y+38, text_editor.str_fine_coarse(1) ); // "Medium" );
//		button_adjustment[2].create_text( INFO_X1+152, y+40, INFO_X1+214, y+58, text_editor.str_fine_coarse(2) ); // "Coarse" );
	}

	if( !nation_array.is_deleted(brush_player_recno) )
	{
		vga.active_buf->bar( INFO_X1+15, y, INFO_X2-10, y+66, V_BLACK );

		Nation *nationPtr = nation_array[brush_player_recno];

		// -------- treasure --------//

		font_whbl.put( INFO_X1+15, y, text_editor.str_player_treasure() ); //"Treasure" );
		font_whbl.right_put( INFO_X1+140-4, y, m.format( (int)nationPtr->cash, 2) );
		button_cash_up.paint();
		button_cash_down.paint();

		// --------- food ----------//

		font_whbl.put( INFO_X1+15, y+22, text_editor.str_player_food() ); // "Food" );
		font_whbl.right_put( INFO_X1+140-4, y+22, m.format((int)nationPtr->food) );
		button_food_up.paint();
		button_food_down.paint();

		// --------- reputation -------//

		font_whbl.put( INFO_X1+15, y+44, text_editor.str_player_reputation() ); // "Reputation" );
		font_whbl.right_put( INFO_X1+140-4, y+44, m.format( (int)nationPtr->reputation) );
		button_reput_up.paint();
		button_reput_down.paint();

		// --------- live points ---------//

		font_whbl.put( INFO_X1+15, y+66, text_editor.str_player_live_points() ); // "Life Points" );
		font_whbl.right_put( INFO_X1+140-4, y+66, m.format( (int)nationPtr->live_points) );
		button_live_up.paint();
		button_live_down.paint();
	}

	y += 87;
	if( refreshFlag == INFO_REPAINT )
	{
		button_adjustment[0].create_text( INFO_X1+147, y+18, INFO_X1+216, y+38, text_editor.str_fine_coarse(0) ); // "Fine" );
		button_adjustment[1].create_text( INFO_X1+76, y+18, INFO_X1+145, y+38, text_editor.str_fine_coarse(1) ); // "Medium" );
		button_adjustment[2].create_text( INFO_X1+5, y+18, INFO_X1+74, y+38, text_editor.str_fine_coarse(2) ); // "Coarse" );
	}
	font_whbl.put( INFO_X1+15, y, text_editor.str_adjustment() );
	button_adjustment.paint();

	// ------- player type --------//

	if( refreshFlag == INFO_REPAINT )
	{
//		button_player_type[0].create_text( INFO_X1+50, y, INFO_X1+100, y+18, "Either" );
//		button_player_type[1].create_text( INFO_X1+50, y, INFO_X1+100, y+18, "Computer" );
	}
	if( !nation_array.is_deleted(brush_player_recno) )
	{
//		button_player_type.paint( nation_array[brush_player_recno]->is_ai() );
	}
}
// ----- end of function ScenarioEditor::disp_players_main ------//


// ----- begin of function ScenarioEditor::detect_players_main ------//
//
void ScenarioEditor::detect_players_main()
{
	int rc;

	// ------- detect scroll area ----------//

	if( (rc = browse_nation.detect()) > 0 )
	{
		// ###### begin Gilbert 22/2 #######//
		// brush_player_recno = rc;
		// ###### end Gilbert 22/2 #######//
	}
	brush_player_recno = browse_nation.recno();

	if( nation_array.is_deleted(brush_player_recno) )
		return;

	Nation *nationPtr = nation_array[brush_player_recno];

	int y = INFO_Y1+170;

	if( mouse.single_click( INFO_X1+5, y, INFO_X2-5, y+8+font_cara_w.max_font_height ) )
	{
		// create geta to edit nation name

		const int yOffs = 4;
		const int lrMargin = 5;
		const FIELD_LEN = 30;

		mouse.hide();

		// paint the background again
		int y2 = y+8+font_cara_w.max_font_height;
		vga.active_buf->d3_panel_up( INFO_X1+5, y, INFO_X2-5, y2 );
		Blob2DW backGroundBuf;
		backGroundBuf.resize( 0, 0, (INFO_X2-5)-(INFO_X1+5+lrMargin)+1, y2-(y+yOffs)+1 );
		vga.active_buf->read_bitmapW( INFO_X1+5+lrMargin, y+yOffs, INFO_X2-5, y2, 
			backGroundBuf.bitmap_ptr() );		// get a wider background

		mouse.show();

		char nationStr[FIELD_LEN+1];
		strncpy( nationStr, nationPtr->nation_name(), FIELD_LEN );
		nationStr[FIELD_LEN] = '\0';

		GetA nation_name_field;
		nation_name_field.init( INFO_X1+5+lrMargin, y+yOffs, INFO_X2-5-lrMargin, nationStr, 
			FIELD_LEN, &font_cara_w, 1, 1 );
		nation_name_field.set_background( backGroundBuf.ptr );
		nation_name_field.paint();

		while(1)
		{
			sys.yield();
			mouse.get_event();
			music.yield();

			int rc = nation_name_field.detect();
			if( rc == KEY_RETURN )
			{
				nation_array.set_custom_nation_name( brush_player_recno, nationStr );
				break;
			}
			else if( rc == KEY_ESC )
			{
				break;		// cancel
			}

			if( rc )
				nation_name_field.paint(1);

			sys.blt_virtual_buf();
		}
	}

	int cashAdjustment[3] = { 100, 1000, 10000 };
	if( button_cash_up.detect() )
	{
		nationPtr->cash += cashAdjustment[button_adjustment()];
	}
	if( button_cash_down.detect() )
	{
		nationPtr->cash -= cashAdjustment[button_adjustment()];
	}

	int foodAdjustment[3] = { 100, 1000, 10000 };
	if( button_food_up.detect() )
	{
		nationPtr->food += foodAdjustment[button_adjustment()];
	}
	if( button_food_down.detect() )
	{
		nationPtr->food -= foodAdjustment[button_adjustment()];
	}

	int reputationAdjustment[3] = { 1, 10, 100 };
	if( button_reput_up.detect() )
	{
		nationPtr->reputation += reputationAdjustment[button_adjustment()];
		if( nationPtr->reputation > 100 )
			nationPtr->reputation = 100;
	}
	if( button_reput_down.detect() )
	{
		nationPtr->reputation -= reputationAdjustment[button_adjustment()];
		if( nationPtr->reputation < -100 )
			nationPtr->reputation = -100;
	}

	if( nationPtr->is_monster() )
	{
		int lifeAdjustment[3] = { 100, 1000, 10000 };
		if( button_live_up.detect() )
		{
			nationPtr->live_points += lifeAdjustment[button_adjustment()];
		}
		if( button_live_down.detect() )
		{
			nationPtr->live_points -= lifeAdjustment[button_adjustment()];
		}
	}

	button_adjustment.detect();

//	if( (rc = button_player_type.detect()) >= 0 )
//	{
//		nation_array[brush_player_recno]->nation_type = rc ? NATION_AI : NATION_REMOTE;
//	}
}
// ----- end of function ScenarioEditor::detect_players_main ------//


// ----- end of function ScenarioEditor::disp_players_view ------//
//
void ScenarioEditor::disp_players_view()
{
}
// ----- end of function ScenarioEditor::disp_players_view ------//


// ----- begin of function ScenarioEditor::detect_players_view ------//
//
int ScenarioEditor::detect_players_view()
{
	return 0;
}
// ----- end of function ScenarioEditor::detect_players_view ------//


// ----- begin of function ScenarioEditor::disp_nation_rec ------//
//
static void disp_nation_rec(int recNo,int x,int y,int refreshFlag)
{
	if( nation_array.is_deleted(recNo) )
	{
		font_snds.put( x+6, y+2, recNo );
		font_snds.put( x+16, y+2, text_editor.str_player_disable() ); // "(Disabled)" );
	}
	else
	{
		Nation *nationPtr = nation_array[recNo];

		int x2;
		vga.active_buf->bar( x+2, y+2, x+15, y+17, nationPtr->nation_color );
		font_snds.put( x+6, y+2, recNo );
		font_snds.put( x+18,  y+2, nationPtr->king_name(), 0, x+130 );

		if( nationPtr->is_human() )
			font_snds.put( x+132, y+2, race_res[nationPtr->race_id]->name, 0, x+198 );
		else
			font_snds.put( x+132, y+2, monster_res[nationPtr->monster_id()]->name, 0, x+198 );

		font_snds.put( x+18,  y+16, m.format((int)nationPtr->cash,2) );

		char charStr[] = "?";
			//SXM:Risk
#if(defined(CHINESE))
		charStr[0] = 'F';
#else
		charStr[0] = text_editor.str_player_food()[0];
#endif
		x2 = font_snds.put( x+90, y+16, charStr );
		font_snds.put( x2+5,  y+16, (int)nationPtr->food );

		if( !nationPtr->is_monster() )
		{
#if(defined(CHINESE))
			charStr[0] = 'R';
#else
			charStr[0] = text_editor.str_player_reputation()[0];
#endif
			x2 = font_snds.put( x+160, y+16, charStr );
			font_snds.put( x2+5, y+16, (int)nationPtr->reputation );
		}
		else
		{
#if(defined(CHINESE))
			charStr[0] = 'L';
#else
			charStr[0] = text_editor.str_player_live_points()[0];
#endif
			//SXM:Risk
			x2 = font_snds.put( x+146, y+16, charStr );
			font_snds.put( x2+5, y+16, (int)nationPtr->live_points );
		}

//		font_snds.put( x+1, y+2, (int)nation_array[recNo]->is_ai() ? "Computer" : "Either" );
	}
}
// ----- end of function ScenarioEditor::disp_nation_rec ------//


// ----- begin of function ScenarioEditor::disp_color_button ------//
//
static void disp_color_button(ButtonCustom *button, int )
{
	vga.active_buf->bar( button->x1, button->y1, button->x2, button->y2, 
		game.color_remap_array[button->custom_para.value].main_color);

	if( button->custom_para.value == 0 )
	{
		font_san.center_put( button->x1, button->y1, button->x2, button->y2, "X");
	}

	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2, 2, 0 );
	}
	else
	{
		vga.active_buf->d3_panel_down( button->x1, button->y1, button->x2, button->y2, 2, 0 );
	}
}
// ----- end of function ScenarioEditor::disp_color_button ------//
