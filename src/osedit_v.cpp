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

// Filename    : OSEDIT_V.CPP
// Description : Scenario editor, victory condition

#include <OSEDITOR.H>
#include <ALL.H>
#include <OFONT.H>
#include <ODATE.H>
#include <OVBROWIF.H>
#include <OBUTTON.H>
#include <OMODEID.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OUNITRES.H>
#include <OITEM.H>
#include <OITEMRES.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OSITE.H>
#include <OUNIT.H>
#include <OFIRM.H>
#include <OTOWN.H>
#include <ITEMID.H>
#include <OT_SEDIT.H>


// ----- enum for menu mode ------//

enum
{
	SE_VICTORY_MAIN = 0,
	SE_VICTORY_WIN_COND = 1,
	SE_VICTORY_LOSE_COND = 2,
	SE_VICTORY_SEL_UNIT_ID,
	SE_VICTORY_SEL_ITEM_ID,
	SE_VICTORY_SEL_TECH_ID,
	SE_VICTORY_SCALAR,
	SE_VICTORY_YEAR_LIMIT,
	SE_VICTORY_MESSAGE,
};

// ----- static variable -------//

static VBrowseIF browse_win_condition;
static VBrowseIF browse_lose_condition;
static Button button_add_win_condition;
static Button button_del_win_condition;
static Button button_add_lose_condition;
static Button button_del_lose_condition;
static Button button_set_win_require_all_condition;

static VBrowseIF browse_win_id;
static VBrowseIF browse_lose_id;
static VBrowseIF browse_unit_id;
static VBrowseIF browse_item_id;
static VBrowseIF browse_tech_id;

static Button button_next;
static Button button_cancel;
static Button button_scalar_up;
static Button button_scalar_down;
static Button button_scalar_up10;
static Button button_scalar_down10;
static Button button_year_up;
static Button button_year_down;
static ButtonGroup button_win_or_lose(2);
static Button button_deadline_up, button_deadline_down;


// ----- parameter for adding win/lose condition ------//

static int selected_win_condition;
static int selected_lose_condition;
static int selected_unit_id;
static int selected_item_id;
static int selected_tech_id;
static int selected_recno;
static int selected_scalar;
static int selected_scalar_max;
static int selected_year_limit;
static int selected_year_min;
static int current_para;		// 1=para1 or 2=para2, 3=time limit

static int filtered_win_condition_count;
static int filtered_win_condition[END_CONDITION_COUNT];
static int filtered_lose_condition_count;
static int filtered_lose_condition[END_CONDITION_COUNT];

static String message_str;		// message for the SE_VICTORY_MESSAGE mode
static int message_sub_mode;	// 0 = "back", 1="next","cancel"
static int last_win_or_lose;

// ------- define static function ---------//

static void disp_win_condition(int recNo,int x,int y,int refreshFlag);
static void disp_lose_condition(int recNo,int x,int y,int refreshFlag);
static void disp_win_cond_id(int recNo,int x,int y,int refreshFlag);
static void disp_lose_cond_id(int recNo,int x,int y,int refreshFlag);
static void disp_unit_id(int recNo,int x,int y,int refreshFlag);
static void disp_item_id(int recNo,int x,int y,int refreshFlag);
static void disp_tech_id(int recNo,int x,int y,int refreshFlag);

static int proceed_victory_menu();
static void add_condition();


// ---- begin of function ScenarioEditor::init_victory_mode ------//
//
void ScenarioEditor::init_victory_mode()
{
//	memset(win_condition, 0, sizeof(win_condition) );
//	memset(lose_condition, 0, sizeof(lose_condition) );

	// ------ default win / lose condition -------//

//	win_condition[0].init(E_DEFEAT_ALL_OTHER_KINGDOMS, 0, 0, 0 );
//	win_condition_count = 1;
//	lose_condition[0].init( E_DEFEAT_OWN_KINGDOM, 0, 0, 0 );
//	lose_condition_count = 1;

	// ------- find all win condition and lose conditions ------//

	filtered_win_condition_count = 0;
	filtered_lose_condition_count = 0;

	for( int i = 2; i < END_CONDITION_COUNT; ++i )		// skip the first two condition
	{
		if( EndConditionInfo::end_condition_info[i].win_or_lose & EW_WIN_ONLY )
			filtered_win_condition[filtered_win_condition_count++] = i;

		if( EndConditionInfo::end_condition_info[i].win_or_lose & EW_LOSE_ONLY )
			filtered_lose_condition[filtered_lose_condition_count++] = i;
	}

	win_condition_browse_recno = lose_condition_browse_recno = 1;
	win_id_browse_recno = lose_id_browse_recno = 1;
	unit_id_browse_recno = item_id_browse_recno = tech_id_browse_recno = 1;
	win_or_lose = 0;		// win 
}
// ---- end of function ScenarioEditor::init_victory_mode ------//


// ---- begin of function ScenarioEditor::deinit_victory_mode ------//
//
void ScenarioEditor::deinit_victory_mode()
{
}
// ---- end of function ScenarioEditor::deinit_victory_mode ------//


// ---- begin of function ScenarioEditor::disp_victory_mode ------//
//
void ScenarioEditor::disp_victory_main(int refreshFlag)
{
	switch( edit_sub_menu_mode[edit_mode] )
	{
	case SE_VICTORY_MAIN:		// victory main menu
		if( refreshFlag == INFO_REPAINT )
		{
			// win or lose button group

			button_win_or_lose[0].create_text( INFO_X1+15, INFO_Y1+5, INFO_X1+110, INFO_Y1+25,
				text_editor.str_victory(), 0 );		// "Victory"
			button_win_or_lose[1].create_text( INFO_X1+115, INFO_Y1+5, INFO_X1+210, INFO_Y1+25,
				text_editor.str_loss(), 0 );		// "Loss"
		}

		button_win_or_lose.paint( win_or_lose );

		if( refreshFlag == INFO_REPAINT || last_win_or_lose != win_or_lose )
		{
			refreshFlag = INFO_REPAINT;
			last_win_or_lose = win_or_lose;
		}

		if( win_or_lose == 0 )		// win
		{
			if( refreshFlag == INFO_REPAINT )
			{
				browse_win_condition.init(INFO_X1, INFO_Y1+32, INFO_X2, INFO_Y1+240, -1, 36, game.win_condition_count, disp_win_condition );
				browse_win_condition.open(win_condition_browse_recno);
				button_add_win_condition.create_text(INFO_X1+20, INFO_Y1+252, INFO_X1+100, INFO_Y1+272, text_editor.str_add() ); //"Add");
				button_del_win_condition.create_text(INFO_X1+120, INFO_Y1+252, INFO_X1+200, INFO_Y1+272, text_editor.str_delete()); // "Delete");
				button_set_win_require_all_condition.create_text(INFO_X1+12, INFO_Y1+275, INFO_X2-12, INFO_Y1+295, text_editor.str_require_all(), 0 ); //"Require All");
			}
			browse_win_condition.paint();
			browse_win_condition.refresh();

			button_add_win_condition.enable_flag = game.win_condition_count < MAX_WIN_CONDITION;
			button_add_win_condition.paint();
			button_del_win_condition.enable_flag = browse_win_condition.recno() > 1;
			button_del_win_condition.paint();

			if( (button_set_win_require_all_condition.visible_flag = game.win_condition_count > 1) )
			{
				button_set_win_require_all_condition.enable_flag = 1;
				button_set_win_require_all_condition.is_pushed = game.win_require_all_condition != 0;
				if( !button_set_win_require_all_condition.is_pushed )
					button_set_win_require_all_condition.set_body( text_editor.str_require_any() ); //"Any Condition is true" );
				else
					button_set_win_require_all_condition.set_body( text_editor.str_require_all() ); //"All Conditions are true" );
				button_set_win_require_all_condition.paint();
			}
		}
		else					// lose
		{
			if( refreshFlag == INFO_REPAINT )
			{
				browse_lose_condition.init(INFO_X1, INFO_Y1+32, INFO_X2, INFO_Y1+240, -1, 36, game.lose_condition_count, disp_lose_condition );
				browse_lose_condition.open(lose_condition_browse_recno);
				button_add_lose_condition.create_text(INFO_X1+20, INFO_Y1+252, INFO_X1+100, INFO_Y1+272, text_editor.str_add() ); //"Add");
				button_del_lose_condition.create_text(INFO_X1+120, INFO_Y1+252, INFO_X1+200, INFO_Y1+272, text_editor.str_delete()); // "Delete");
			}
			browse_lose_condition.paint();
			browse_lose_condition.refresh();

			button_add_lose_condition.enable_flag = game.lose_condition_count < MAX_LOSE_CONDITION;
			button_add_lose_condition.paint();
			button_del_lose_condition.enable_flag = browse_lose_condition.recno() > 1;
			button_del_lose_condition.paint();
		}

		// display time limit
		if( game.win_deadline )
		{
			int x2 = font_whbl.put( INFO_X1+12, INFO_Y1+302, text_editor.str_deadline() );
			x2 = font_whbl.put( x2+6, INFO_Y1+302, date.date_str(game.win_deadline, 1));	// 1-short date

			x2 += 20;
			if( refreshFlag == INFO_REPAINT )
			{
				button_deadline_up.create_text( x2, INFO_Y1+302, x2+10, INFO_Y1+311, "+" );
				button_deadline_down.create_text( x2, INFO_Y1+312, x2+10, INFO_Y1+321, "-" );
			}
			button_deadline_up.paint();
			button_deadline_down.paint();
		}
		break;

	case SE_VICTORY_WIN_COND:		// select winning condition
		if( refreshFlag == INFO_REPAINT )
		{
			browse_win_id.init(INFO_X1, INFO_Y1+6, INFO_X2, INFO_Y1+220, -1, 20,
				filtered_win_condition_count, disp_win_cond_id );
			// ####### begin Gilbert 22/2 ########//
			browse_win_id.open(win_id_browse_recno);
			// ####### end Gilbert 22/2 ########//
			button_next.create_text(INFO_X1+20, INFO_Y1+230, INFO_X1+100, INFO_Y1+250, text_editor.str_next() ); // "Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+230, INFO_X1+200, INFO_Y1+250, text_editor.str_cancel() ); // "Cancel" );
		}
		browse_win_id.paint();
		// ####### begin Gilbert 22/2 ########//
		browse_win_id.refresh();
		// ####### end Gilbert 22/2 ########//
		button_next.paint();
		button_cancel.paint();
		// display the full text of winning condition
		if( browse_win_id.recno() > 0 && browse_win_id.recno() <= browse_win_id.total_rec() )
		{
			font_whbl.put_paragraph( INFO_X1+8, INFO_Y1+260, INFO_X2-8, INFO_Y1+315,
				EndConditionInfo::end_condition_info[filtered_win_condition[browse_win_id.recno()-1]].name() );
		}
		break;

	case SE_VICTORY_LOSE_COND:		// select losing condition
		if( refreshFlag == INFO_REPAINT )
		{
			browse_lose_id.init(INFO_X1, INFO_Y1+6, INFO_X2, INFO_Y1+220, -1, 20,
				filtered_lose_condition_count, disp_lose_cond_id );
			// ####### begin Gilbert 22/2 ########//
			browse_lose_id.open(lose_id_browse_recno);
			// ####### begin Gilbert 22/2 ########//
			button_next.create_text(INFO_X1+20, INFO_Y1+230, INFO_X1+100, INFO_Y1+250, text_editor.str_next() ); // "Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+230, INFO_X1+200, INFO_Y1+250, text_editor.str_cancel() ); //"Cancel" );
		}
		browse_lose_id.paint();
		// ####### begin Gilbert 22/2 ########//
		browse_lose_id.refresh();
		// ####### end Gilbert 22/2 ########//
		button_next.paint();
		button_cancel.paint();
		// display the full text of winning condition
		if( browse_lose_id.recno() > 0 && browse_lose_id.recno() <= browse_lose_id.total_rec() )
		{
			font_whbl.put_paragraph( INFO_X1+8, INFO_Y1+260, INFO_X2-8, INFO_Y1+315,
				EndConditionInfo::end_condition_info[filtered_lose_condition[browse_lose_id.recno()-1]].name() );
		}
		break;

	case SE_VICTORY_SEL_UNIT_ID:	// select unit id
		if( refreshFlag == INFO_REPAINT )
		{
			browse_unit_id.init(INFO_X1, INFO_Y1+6, INFO_X2, INFO_Y1+220, -1, 20,
				unit_res.unit_info_count, disp_unit_id );
			// ##### begin Gilbert 22/2 #######//
			browse_unit_id.open(unit_id_browse_recno);
			// ##### end Gilbert 22/2 #######//
			button_next.create_text(INFO_X1+20, INFO_Y1+230, INFO_X1+100, INFO_Y1+250, text_editor.str_next() ); //"Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+230, INFO_X1+200, INFO_Y1+250, text_editor.str_cancel() ); // "Cancel" );
		}
		browse_unit_id.paint();
		// ##### begin Gilbert 22/2 #######//
		browse_unit_id.refresh();
		// ##### end Gilbert 22/2 #######//
		button_next.paint();
		button_cancel.paint();
		break;

	case SE_VICTORY_SEL_ITEM_ID:
		if( refreshFlag == INFO_REPAINT )
		{
			browse_item_id.init(INFO_X1, INFO_Y1+6, INFO_X2, INFO_Y1+220, -1, 20,
				ITEM_TYPE_COUNT, disp_item_id );
			// ##### begin Gilbert 22/2 #######//
			browse_item_id.open(item_id_browse_recno);
			// ##### end Gilbert 22/2 #######//
			button_next.create_text(INFO_X1+20, INFO_Y1+230, INFO_X1+100, INFO_Y1+250, text_editor.str_next() ); //"Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+230, INFO_X1+200, INFO_Y1+250, text_editor.str_cancel() ); // "Cancel" );
		}
		browse_item_id.paint();
		// ##### begin Gilbert 22/2 #######//
		browse_item_id.refresh();
		// ##### end Gilbert 22/2 #######//
		button_next.paint();
		button_cancel.paint();
		break;

	case SE_VICTORY_SEL_TECH_ID:
		if( refreshFlag == INFO_REPAINT )
		{
			browse_tech_id.init(INFO_X1, INFO_Y1+6, INFO_X2, INFO_Y1+220, -1, 20,
				MAX_TECH, disp_tech_id );
			browse_tech_id.open(tech_id_browse_recno);
			button_next.create_text(INFO_X1+20, INFO_Y1+230, INFO_X1+100, INFO_Y1+250, text_editor.str_next() ); //"Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+230, INFO_X1+200, INFO_Y1+250, text_editor.str_cancel() ); // "Cancel" );
		}
		browse_tech_id.paint();
		browse_tech_id.refresh();
		button_next.paint();
		button_cancel.paint();
		break;


	case SE_VICTORY_SCALAR:		// input a scalar quantity
		if( refreshFlag == INFO_REPAINT )
		{
			button_scalar_up10.create_text(INFO_X1+130, INFO_Y1+10, INFO_X1+150-1, INFO_Y1+25, "++");
			button_scalar_up.create_text(INFO_X1+150, INFO_Y1+10, INFO_X1+170-1, INFO_Y1+25, "+");
			button_scalar_down.create_text(INFO_X1+170, INFO_Y1+10, INFO_X1+190-1, INFO_Y1+25, "-");
			button_scalar_down10.create_text(INFO_X1+190, INFO_Y1+10, INFO_X1+210-1, INFO_Y1+25, "--");
			button_next.create_text(INFO_X1+20, INFO_Y1+30, INFO_X1+100, INFO_Y1+50, text_editor.str_next() ); //"Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+30, INFO_X1+200, INFO_Y1+50, text_editor.str_cancel() ); //"Cancel" );
		}
		font_whbl.right_put( INFO_X1+120, INFO_Y1+12, m.format(selected_scalar) );
		button_scalar_up10.paint();
		button_scalar_up.paint();
		button_scalar_down.paint();
		button_scalar_down10.paint();
		button_next.paint();
		button_cancel.paint();
		break;

	case SE_VICTORY_YEAR_LIMIT:	// input years limit
		if( refreshFlag == INFO_REPAINT )
		{
			button_year_up.create_text(INFO_X1+150, INFO_Y1+10, INFO_X1+170, INFO_Y1+25, "+");
			button_year_down.create_text(INFO_X1+180, INFO_Y1+10, INFO_X1+200, INFO_Y1+25, "-");
			button_next.create_text(INFO_X1+20, INFO_Y1+30, INFO_X1+100, INFO_Y1+50, text_editor.str_next() ); //"Next" );
			button_cancel.create_text(INFO_X1+120, INFO_Y1+30, INFO_X1+200, INFO_Y1+50, text_editor.str_cancel() ); //"Cancel" );
		}
		if( selected_year_limit )
		{
//			int x2 =	font_whbl.put( INFO_X1+80, INFO_Y1+12, selected_year_limit );
//			font_whbl.put( x2, INFO_Y1+12, selected_year_limit > 1 ? "years" : "year" );
			font_whbl.put( INFO_X1+80, INFO_Y1+12, text_editor.str_time_limit(selected_year_limit) );
		}
		else
		{
			font_whbl.right_put( INFO_X1+145, INFO_Y1+12, text_editor.str_no_time_limit() ); //"No time limit" );
		}

		button_year_up.paint();
		button_year_down.paint();
		button_next.paint();
		button_cancel.paint();
		break;

	case SE_VICTORY_MESSAGE:
		{
			font_whbl.put_paragraph( INFO_X1+15, INFO_Y1+5, INFO_X2-5, INFO_Y1+100, message_str );
			if( refreshFlag == INFO_REPAINT )
			{
				if( message_sub_mode == 0 )
				{
					button_next.create_text(INFO_X1+20, INFO_Y1+110, text_editor.str_back() ); //"Back");
				}
				else
				{
					button_next.create_text(INFO_X1+20, INFO_Y1+110, INFO_X1+100, INFO_Y1+130, text_editor.str_next() ); //"Next" );
					button_cancel.create_text(INFO_X1+120, INFO_Y1+110, INFO_X1+200, INFO_Y1+130, text_editor.str_cancel() ); // "Cancel" );
				}
			}
			button_next.paint();
			if( message_sub_mode == 1 )
				button_cancel.paint();
		}
		break;

	default:
		err_here();
	}
}
// ---- end of function ScenarioEditor::disp_victory_mode ------//


// ---- begin of function ScenarioEditor::detect_victory_mode ------//
//
void ScenarioEditor::detect_victory_main()
{
	int rc;

	switch( edit_sub_menu_mode[edit_mode] )
	{
	case SE_VICTORY_MAIN:			// victory mode main menu
		current_para = 0;

		// detect win or lose button group
		rc = button_win_or_lose.detect();
		if( rc >= 0 )
		{
			win_or_lose = rc;
			return;
		}

		if( win_or_lose == 0 )		// win 
		{
			browse_win_condition.detect();
			win_condition_browse_recno = browse_win_condition.recno();
			if( button_add_win_condition.detect())
			{
				edit_sub_menu_mode[edit_mode] = SE_VICTORY_WIN_COND;	// switch to selecting winning condition
				return;
			}

			if( button_del_win_condition.detect())			// remove selected winning condition
			{
				if( browse_win_condition.recno() > 1 && browse_win_condition.recno() <= game.win_condition_count )
				{
					game.del_win_condition( browse_win_condition.recno() );
					if( win_condition_browse_recno > game.win_condition_count )
						win_condition_browse_recno = game.win_condition_count;
					browse_win_condition.open(win_condition_browse_recno, game.win_condition_count);
				}
			}

			if( button_set_win_require_all_condition.detect() )
			{
				game.win_require_all_condition = button_set_win_require_all_condition.is_pushed;
			}
		}
		else								// lose
		{
			browse_lose_condition.detect();
			lose_condition_browse_recno = browse_lose_condition.recno();

			if( button_add_lose_condition.detect() )		// switch to selecting losing condition
			{
				edit_sub_menu_mode[edit_mode] = SE_VICTORY_LOSE_COND;
				return;
			}

			if( button_del_lose_condition.detect() )		// remove selected losing condition
			{
				if( browse_lose_condition.recno() > 1 && browse_lose_condition.recno() <= game.lose_condition_count )
				{
					game.del_lose_condition( browse_lose_condition.recno() );
					if( lose_condition_browse_recno > game.lose_condition_count )
						lose_condition_browse_recno = game.lose_condition_count;
					browse_lose_condition.open(lose_condition_browse_recno, game.lose_condition_count);
				}
			}
		}

		//  detect deadline
		if( game.win_deadline )
		{
			if( button_deadline_up.detect() )
			{
				game.win_deadline = date.add_months( game.win_deadline, 12 );	 // next year
			}
			if( button_deadline_down.detect() )
			{
				int newDate = date.add_months( game.win_deadline, -12 );	 // prev year
				if( newDate > info.game_date )		// must be the future date
					game.win_deadline = newDate;
			}
		}
		break;

	case SE_VICTORY_WIN_COND:		// select a winning condition
		// ###### begin Gilbert 22/2 #########//
		// if( (rc=browse_win_id.detect()) )
		//		win_id_browse_recno = rc;
		browse_win_id.detect();
		win_id_browse_recno = browse_win_id.recno();
		// ###### end Gilbert 22/2 #########//
		if( button_next.detect() && !browse_win_id.none_record && browse_win_id.recno() )
		{
			selected_win_condition = filtered_win_condition[browse_win_id.recno()-1];
			selected_lose_condition = 0;

			current_para = 1;			// process para1
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_LOSE_COND:		// select a losing condition
		// ###### begin Gilbert 22/2 #########//
		// if( (rc = browse_lose_id.detect()) )
		// 	lose_id_browse_recno = rc;
		browse_lose_id.detect();
		lose_id_browse_recno = browse_lose_id.recno();
		// ###### end Gilbert 22/2 #########//

		if( button_next.detect() && !browse_lose_id.none_record && browse_lose_id.recno() )
		{
			selected_lose_condition = filtered_lose_condition[browse_lose_id.recno()-1];
			selected_win_condition = 0;

			current_para = 1;			// process para1
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_SEL_UNIT_ID:	// select a unit
		// ###### begin Gilbert 22/2 #########//
		// if( (rc=browse_unit_id.detect()) )
		//	unit_id_browse_recno = rc;
		browse_unit_id.detect();
		unit_id_browse_recno = browse_unit_id.recno();
		// ###### end Gilbert 22/2 #########//

		if( button_next.detect() )
		{
			selected_unit_id = unit_id_browse_recno;
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_SEL_ITEM_ID:
		// ###### begin Gilbert 22/2 #########//
		//if( (rc=browse_item_id.detect()) )
		//	item_id_browse_recno = rc;
		browse_item_id.detect();
		item_id_browse_recno = browse_item_id.recno();
		// ###### end Gilbert 22/2 #########//

		if( button_next.detect() )
		{
			selected_item_id = item_id_browse_recno;
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_SEL_TECH_ID:
		browse_tech_id.detect();
		tech_id_browse_recno = browse_tech_id.recno();

		if( button_next.detect() )
		{
			selected_tech_id = tech_id_browse_recno;
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_SCALAR:		// select a scalar quantity
		if( button_scalar_up10.detect() )
		{
			selected_scalar += selected_scalar_max / 10;
			if( selected_scalar > selected_scalar_max )
				selected_scalar = selected_scalar_max;
		}
		if( button_scalar_up.detect() )
		{
			selected_scalar += selected_scalar_max / 100;
			if( selected_scalar > selected_scalar_max )
				selected_scalar = selected_scalar_max;
		}
		if( button_scalar_down.detect() )
		{
			selected_scalar -= selected_scalar_max / 100;
			if( selected_scalar < 0 )
				selected_scalar = 0;
		}
		if( button_scalar_down10.detect() )
		{
			selected_scalar -= selected_scalar_max / 10;
			if( selected_scalar < 0 )
				selected_scalar = 0;
		}

		if( button_next.detect() )
		{
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_YEAR_LIMIT:		// select year limit
		if( button_year_up.detect() )
		{
			++selected_year_limit;
		}
		if( button_year_down.detect() )
		{
			--selected_year_limit;
			if( selected_year_limit < selected_year_min )
				selected_year_limit = selected_year_min;
		}

		if( button_next.detect() )
		{
			if( proceed_victory_menu() )
				add_condition();
		}

		if( button_cancel.detect() )
		{
			edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
			return;
		}
		break;

	case SE_VICTORY_MESSAGE:
		{
			if( button_next.detect() )
			{
				if( proceed_victory_menu() )
					add_condition();
			}
			if( message_sub_mode && button_cancel.detect() )
			{
				edit_sub_menu_mode[edit_mode] = SE_VICTORY_MAIN;
				return;
			}
		}
		break;

	default:
		err_here();
	}
}
// ---- end of function ScenarioEditor::detect_victory_mode ------//


// ---- begin of function ScenarioEditor::disp_victory_view ------//
//
void ScenarioEditor::disp_victory_view()
{
}
// ---- end of function ScenarioEditor::disp_victory_view ------//


// ---- begin of function ScenarioEditor::detect_victory_view ------//
//
int ScenarioEditor::detect_victory_view()
{
	return 0;
}
// ---- end of function ScenarioEditor::detect_victory_view ------//


// ---- begin of static function disp_win_condition ------//
//
static void disp_win_condition(int recNo,int x,int y,int refreshFlag)
{
	// ###### begin Gilbert 22/2 #######//
	font_snds.put_paragraph( x+6, y+2, x+195, y+34, game.win_condition[recNo-1].cond_str );		// 0=win condition
	// ###### end Gilbert 22/2 #######//
}
// ---- end of static function disp_win_condition ------//


// ---- begin of static function disp_lose_condition ------//
//
static void disp_lose_condition(int recNo,int x,int y,int refreshFlag)
{
	// ###### begin Gilbert 22/2 #######//
	font_snds.put_paragraph( x+6, y+2, x+195, y+34, game.lose_condition[recNo-1].cond_str );	// 1=lose condition
	// ###### end Gilbert 22/2 #######//
}
// ---- end of static function disp_lose_condition ------//


// ---- begin of static function disp_win_cond_id ------//
//
static void disp_win_cond_id(int recNo,int x,int y,int refreshFlag)
{
	font_snds.put( x+6, y+2, EndConditionInfo::end_condition_info[filtered_win_condition[recNo-1]].name(), 0, x+194 );
}
// ---- end of static function disp_win_cond_id ------//


// ---- begin of static function disp_lose_cond_id ------//
//
static void disp_lose_cond_id(int recNo,int x,int y,int refreshFlag)
{
	font_snds.put( x+6, y+2, EndConditionInfo::end_condition_info[filtered_lose_condition[recNo-1]].name(), 0, x+194 );
}
// ---- end of static function disp_lose_cond_id ------//


// ---- begin of static function disp_unit_id ------//
//
static void disp_unit_id(int recNo,int x,int y,int refreshFlag)
{
	// ###### patch begin Gilbert 27/9 ########//
	font_snds.put( x+6, y+2, unit_res[recNo]->name, 0, x+194 );
	// ###### patch end Gilbert 27/9 ########//
}
// ---- end of static function disp_unit_id ------//


// ---- begin of static function disp_item_id ------//
//
static void disp_item_id(int recNo,int x,int y,int refreshFlag)
{
	// ###### patch begin Gilbert 27/9 ########//
	font_snds.put( x+6, y+2, item_res.item_name(recNo, 0), 0, x+194 );
	// ###### patch end Gilbert 27/9 ########//
}
// ---- end of static function disp_item_id ------//


// ---- begin of static function disp_tech_id ------//
//
static void disp_tech_id(int recNo,int x,int y,int refreshFlag)
{
	// ###### patch begin Gilbert 27/9 ########//
	font_snds.put( x+6, y+2, tech_res[recNo]->tech_des(), 0, x+194 );
	// ###### patch end Gilbert 27/9 ########//
}
// ---- end of static function disp_tech_id ------//


// ----- begin of static function proceed_victory_menu ------//
//
// return true if condition is complete
//
static int proceed_victory_menu()
{
	err_when( selected_win_condition && selected_lose_condition );		// cannot be both non-zero
	EndConditionInfo &condInfo = EndConditionInfo::end_condition_info[selected_win_condition+selected_lose_condition];

	int failed = 0;
	int skip = 0; 

	// check if the condition is for win or loss only

	if( selected_win_condition && !(condInfo.win_or_lose & EW_WIN_ONLY)
		|| selected_lose_condition && !(condInfo.win_or_lose & EW_LOSE_ONLY) )
	{
		failed = 1;
	}

	// check next menu

	else if( current_para >= 1 && current_para <= 2 )
	{
		switch( current_para==1 ? condInfo.para1_type : condInfo.para2_type )
		{
		case EP_NONE:
			skip = 1;
			break;

		case EP_0_100:
			selected_scalar = 5;
			selected_scalar_max = 100;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SCALAR;
			break;

		case EP_0_1000:
			selected_scalar = 50;
			selected_scalar_max = 1000;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SCALAR;
			break;

		case EP_0_10000:
			selected_scalar = 500;
			selected_scalar_max = 10000;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SCALAR;
			break;

		case EP_0_100000:
			selected_scalar = 5000;
			selected_scalar_max = 100000;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SCALAR;
			break;

		case EP_UNIT_ID:
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SEL_UNIT_ID;
			break;

		case EP_ITEM_ID:
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SEL_ITEM_ID;
			break;

		case EP_TECH_ID:
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_SEL_TECH_ID;
			break;

		case EP_NATION_RECNO:	// get select object
			message_str = text_editor.str_select_nation(); //"Select target nation by selecting any object of that nation";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_UNIT_RECNO:
			message_str = text_editor.str_select_unit(); //"Select target unit";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_FRIEND_UNIT_RECNO:
			message_str = text_editor.str_select_friendly_unit(); // "Select a friendly unit";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_ENEMY_UNIT_RECNO:
			message_str = text_editor.str_select_unit(); // "Select target unit";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_TOWN_RECNO:
			message_str = text_editor.str_select_town(); //"Select target town";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_FRIEND_TOWN_RECNO:
			message_str = text_editor.str_select_friendly_town(); // "Select a friendly town";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_ENEMY_TOWN_RECNO:
			message_str = text_editor.str_select_town(); // "Select target town";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_FIRM_RECNO:
			message_str = text_editor.str_select_firm(); // "Select target building";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_FRIEND_FIRM_RECNO:
			message_str = text_editor.str_select_friendly_firm(); // "Select a friendly building";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_ENEMY_FIRM_RECNO:
			message_str = text_editor.str_select_firm(); // "Select target building";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		case EP_SITE_RECNO:
			message_str = text_editor.str_select_item(); // "Select target item";
			message_sub_mode = 1;
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MESSAGE;
			break;

		default:
			err_here();
			failed = 1;
		}
	}
	else if( current_para == 3 )		// ask time limit
	{
		if( condInfo.time_limit_type	!= ET_NONE )
		{
			if( condInfo.time_limit_type == ET_OPTIONAL )
			{
				selected_year_limit = 0;
				selected_year_min = 0;
			}
			else
			{
				selected_year_limit = 10;
				selected_year_min = 1;
			}
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_YEAR_LIMIT;
		}
		else
		{
			selected_year_limit = 0;
			skip = 1;
		}
	}

	// if skip is clear, edit_sub_menu_mode is changed

	if( failed )		// go back to main menu
	{
		scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MAIN;
		return 0;
	}
	else
	{
		if( current_para >= 4 )		// finish a condition
		{
			scenario_editor.edit_sub_menu_mode[scenario_editor.edit_mode] = SE_VICTORY_MAIN;
			return 1;
		}
		else 
		{
			++current_para;
			if( skip )		// skip next question
				return proceed_victory_menu();
			else		// not finish, proceed to next question
				return 0;
		}
	}
}
// ----- end of static function proceed_victory_menu ------//


// ----- begin of static function add_condition ------//
//
static void add_condition()
{
	// parameter are: selected_win/lose_condition, selected_recno, selected_scalar, game_time_limit

	err_when( selected_win_condition && selected_lose_condition );
	EndConditionInfo &condInfo = EndConditionInfo::end_condition_info[selected_win_condition+selected_lose_condition];

	err_when( selected_win_condition && selected_lose_condition );

	// ------ get selected_recno -------//

	int failed = 0;
	switch( condInfo.para1_type )
	{
	case EP_UNIT_ID:
		selected_recno = selected_unit_id;
		break;

	case EP_ITEM_ID:
		selected_recno = selected_item_id;
		break;

	case EP_TECH_ID:
		selected_recno = selected_tech_id;
		break;

	case EP_NATION_RECNO:	// get select object
		if( unit_array.selected_recno && unit_array[unit_array.selected_recno]->nation_recno )
			selected_recno = unit_array[unit_array.selected_recno]->nation_recno;
		else if( firm_array.selected_recno && firm_array[firm_array.selected_recno]->nation_recno )
			selected_recno = firm_array[firm_array.selected_recno]->nation_recno;
		else if( town_array.selected_recno && town_array[town_array.selected_recno]->nation_recno )
			selected_recno = town_array[town_array.selected_recno]->nation_recno;
		else
			failed = 1;
		break;

	case EP_UNIT_RECNO:
		if( unit_array.selected_recno )
			selected_recno = unit_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_FRIEND_UNIT_RECNO:
		if( unit_array.selected_recno && unit_array[unit_array.selected_recno]->is_own() )
			selected_recno = unit_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_ENEMY_UNIT_RECNO:
		if( unit_array.selected_recno && !unit_array[unit_array.selected_recno]->is_own() )
			selected_recno = unit_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_TOWN_RECNO:
		if( town_array.selected_recno )
			selected_recno = town_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_FRIEND_TOWN_RECNO:
		if( town_array.selected_recno && town_array[town_array.selected_recno]->is_own() )
			selected_recno = town_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_ENEMY_TOWN_RECNO:
		if( town_array.selected_recno && !town_array[town_array.selected_recno]->is_own() )
			selected_recno = town_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_FIRM_RECNO:
		if( firm_array.selected_recno )
			selected_recno = firm_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_FRIEND_FIRM_RECNO:
		if( firm_array.selected_recno && firm_array[firm_array.selected_recno]->is_own() )
			selected_recno = firm_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_ENEMY_FIRM_RECNO:
		if( firm_array.selected_recno && !firm_array[firm_array.selected_recno]->is_own() )
			selected_recno = firm_array.selected_recno;
		else
			failed = 1;
		break;

	case EP_SITE_RECNO:
		if( site_array.selected_recno )
			selected_recno = site_array.selected_recno;
		else
			failed = 1;
		break;
	}

	if( failed )
		return;

	// ------ init new condition ---------//

//	EndCondition *condPtr;
	if( selected_win_condition )
	{
		if( game.win_condition_count >= MAX_WIN_CONDITION )
			return;

		game.add_win_condition( selected_win_condition, 1, 
			selected_recno, selected_scalar,	selected_year_limit );
	}
	else if( selected_lose_condition )
	{
		if( game.lose_condition_count >= MAX_LOSE_CONDITION )
			return;

		game.add_lose_condition( selected_lose_condition, 1,
			selected_recno, selected_scalar,	selected_year_limit );
	}
}
// ----- end of static function add_condition ------//
