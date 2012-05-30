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

//Filename    : OSYSDET.CPP
//Description : class Sys - detect functions

#include <osys.h>
#include <oinfo.h>
#include <key.h>
#include <color.h>
#include <omodeid.h>
#include <ocampgn.h>
#include <oconfig.h>
#include <omouse.h>
#include <oremote.h>
#include <ooptmenu.h>
#include <oingmenu.h>
#include <ogodres.h>
#include <otips.h>
#include <ogame.h>
#include <omagic.h>
#include <otutor.h>
#include <obox.h>
#include <osite.h>
#include <oworld.h>
#include <oworldmt.h>
#include <onews.h>
#include <ospy.h>
#include <onation.h>
#include <of_base.h>
#include <of_camp.h>
#include <otechres.h>
#include <oun_grp.h>
#include <ot_gmenu.h>


//------- Define static functions -----------//

static void locate_king_general(int rankId);
static void locate_spy();
static void locate_royal();
static void	locate_unit_class(int unitClass, int nationRecno=-1);		// nationRecno == -1 for all nations
static void locate_firm_id(char *firmIdString, int nationRecno=-1);

static int  detect_scenario_cheat_key(unsigned scanCode, unsigned skeyState);
static int  get_mouse_loc_in_zoom_map(int &x, int &y);
static void	hide_map();
static void locate_independent_hero();
static void locate_free_item(int checkExplore=1);

typedef int (*FilterUnitFunc)(Unit *unitPtr, int para1);
static int filter_unit_id(Unit *unitPtr, int para1);
static int	filter_unit( FilterUnitFunc filterFunc, int para1, int negate=0);

//-------- Begin of function Sys::detect --------//
//
void Sys::detect()
{
	//--- only detect at the even frames when in report mode ---//

	if( view_mode != MODE_NORMAL &&
		 report_disp_frame_no )
	{
		return;
	}

	//--------------------------------------//

	mouse.get_event();

	if( option_menu.is_active() )
	{
		option_menu.detect();
		return;
	}

	if( in_game_menu.is_active() )
	{
		in_game_menu.detect();
		return;
	}

	if( VBrowse::press_record )		// if the user is pulling the records of the browser up and down, calling detect() routines of other controls will confuse it
	{
		VBrowse::press_vbrowse_ptr->detect();
	}

	if( mouse.is_key_event() )
	{
		process_key(mouse.scan_code, mouse.event_skey_state);
	}

	detect_button();		// detect main buttons on the screen

	// ###### begin Gilbert 21/5 ##########//
	if( view_mode == MODE_NORMAL )
	{
		if( tips_res.detect() )
			return;
	}
	// ###### end Gilbert 21/5 ##########//

	detect_view();

	//--------- campaign cheat ---------//

	if( game.is_campaign_mode() && game.campaign()->auto_test_flag )
	{
		if( mouse.right_press )		// disable auto test when the right mouse button is pressed
		{
			game.campaign()->auto_test_flag = 0;
		}
		else
		{
			if( game.campaign()->auto_test_flag )		// if in auto test mode, we must randomize the seed as retry will restore the original seed. 
				m.randomize();

			if( m.random(3)>0 )		// 2/3 chance winning
			{
				game.result_win_condition_recno = m.random(game.win_condition_count);
				game.result_win_condition_id = game.win_condition[game.result_win_condition_recno].condition_id;
				game.game_end(nation_array.player_recno);
			}
			else
			{
				game.result_lose_condition_recno = m.random(game.lose_condition_count);
				game.result_lose_condition_id = game.lose_condition[game.result_lose_condition_recno].condition_id;
				game.game_end(0);
			}
		}
	}
}
//--------- End of function Sys::detect ---------//


//-------- Begin of function Sys::process_key --------//
//
void Sys::process_key(unsigned scanCode, unsigned skeyState)
{
	detect_function_key(scanCode, skeyState);

	detect_campaign_cheat_key(scanCode, skeyState);		// BUGHERE

	//----- don't detect letter keys when in chat mode ----//

	if( !(view_mode == MODE_NATION &&
			info.nation_report_mode == NATION_REPORT_CHAT) )
	{
		if( sys.debug_session || sys.testing_session || scenario_cheat_flag )
		{
			detect_cheat_key(scanCode, skeyState);
         detect_debug_cheat_key(scanCode, skeyState);

         if( detect_scenario_cheat_key(scanCode, skeyState) )
            return;
      }
      else
      {
         if( nation_array.player_recno && !remote.is_enable() )      // not allowed in multiplayer mode
         {
            if( (~nation_array)->cheat_enabled_flag )
            {
               detect_cheat_key(scanCode, skeyState);
#ifdef DEBUG
		         detect_debug_cheat_key(scanCode, skeyState);
#endif
            }
            else
				{
					// if( detect_key_str(1, "!!!@@@###") )
					if( detect_key_str(1, "!!##%%&&") )
					{
                  box.msg( text_game_menu.str_cheat_enable() ); // "Cheat Mode Enabled." );
                  (~nation_array)->cheat_enabled_flag = 1;
					}
            }
			}
		}

      detect_letter_key(scanCode, skeyState);

		detect_set_speed(scanCode, skeyState);                     // set the speed of the game
	}
}
//--------- End of function Sys::process_key ---------//


//-------- Begin of function Sys::detect_letter_key --------//
//
void Sys::detect_letter_key(unsigned scanCode, unsigned skeyState)
{
	int keyCode;

	//------ key for formation -------//

	if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, 0)))
	{
		if( unit_array.selected_recno &&
			 keyCode >= '0' && keyCode <= '0' + MAX_FORMATION )
		{
			if( unit_array.selected_recno )
			{
				Unit* unitPtr = unit_array[unit_array.selected_recno];

				if( unitPtr->team_info )
				{
					// ###### begin Gilbert 25/2 #######//
					// do not set formation_id here, let unit_group.formation set it (for multiplayer game)
//					unitPtr->team_info->formation_id = keyCode-'0';
//					if( keyCode > '0' )		// '0' resets formation
//					{
//						unit_group.set( unitPtr->team_info->member_unit_array, unitPtr->team_info->member_count );
//						unit_group.formation(unitPtr->team_info->formation_id, COMMAND_PLAYER);
//					}
					unit_group.set( unitPtr->team_info->member_unit_array, unitPtr->team_info->member_count );
					unit_group.formation(keyCode-'0', COMMAND_PLAYER);
					// ###### end Gilbert 25/2 #######//
				}
			}

			return;
		}
	} 

	/*
	if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_ON_NUMPAD)))
	{
		if( unit_array.selected_recno )
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];

			if(unitPtr->team_info)
			{
				unit_group.set(unitPtr->team_info->member_unit_array, unitPtr->team_info->member_count );
				unit_group.formation_turn(keyCode);
			}
		}
		return;
	}
	*/

	//------ key for turning formation direction -------//

	if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_ON_NUMPAD)))
	{
		if( unit_array.selected_recno )
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];

			if(unitPtr->team_info)
			{
				// ######## begin Gilbert 25/2 ########//
//				switch(keyCode)
//				{
//					case '9':
//						unitPtr->team_info->formation_direction = LEADER_AT_TOP;
//						break;
//					case '8':
//						unitPtr->team_info->formation_direction = LEADER_AT_TOP_LEFT;
//						break;
//					case '7':
//						unitPtr->team_info->formation_direction = LEADER_AT_LEFT;
//						break;
//					case '6':
//						unitPtr->team_info->formation_direction = LEADER_AT_TOP_RIGHT;
//						break;
//					case '4':
//						unitPtr->team_info->formation_direction = LEADER_AT_BOTTOM_LEFT;
//						break;
//					case '3':
//						unitPtr->team_info->formation_direction = LEADER_AT_RIGHT;
//						break;
//					case '2':
//						unitPtr->team_info->formation_direction = LEADER_AT_BOTTOM_RIGHT;
//						break;
//					case '1':
//						unitPtr->team_info->formation_direction = LEADER_AT_BOTTOM;
//						break;
//					default:
//						return;
//				}
//				unit_group.set(unitPtr->team_info->member_unit_array, unitPtr->team_info->member_count );
//				unit_group.formation_turn(unitPtr->team_info->formation_direction, COMMAND_PLAYER);

				int newFormationDirection = 0;
				switch(keyCode)
				{
					case '9':
						newFormationDirection = LEADER_AT_TOP;
						break;
					case '8':
						newFormationDirection = LEADER_AT_TOP_LEFT;
						break;
					case '7':
						newFormationDirection = LEADER_AT_LEFT;
						break;
					case '6':
						newFormationDirection = LEADER_AT_TOP_RIGHT;
						break;
					case '4':
						newFormationDirection = LEADER_AT_BOTTOM_LEFT;
						break;
					case '3':
						newFormationDirection = LEADER_AT_RIGHT;
						break;
					case '2':
						newFormationDirection = LEADER_AT_BOTTOM_RIGHT;
						break;
					case '1':
						newFormationDirection = LEADER_AT_BOTTOM;
						break;
				}
				if( newFormationDirection )
				{
					unit_group.set(unitPtr->team_info->member_unit_array, unitPtr->team_info->member_count );
					unit_group.formation_turn(newFormationDirection, COMMAND_PLAYER);
					return;
				}
				// ######## end Gilbert 25/2 ########//
			}
		}
		// return;
	}

	//----- key for recalling group -------//

	if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_CTRL)))
	{
		if( keyCode >= '1' && keyCode <= '9' )
		{
			unit_group_array[keyCode-'1'].set_to_selected();		// unit_group_array[] start with 0, it is not a DynArray
			return;
		}
	}

	if((keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_ALT)))
	{
		if( keyCode >= '1' && keyCode <= '9' )
		{
			unit_group_array[keyCode-'1'].validate_unit_array(nation_array.player_recno);
			unit_group_array[keyCode-'1'].select_all();		// unit_group_array[] start with 0, it is not a DynArray
			return;
		}
	}

	//-------------------------------------------//

   if( (keyCode = mouse.is_key(scanCode, 
		skeyState & ~CAP_LOCK_STATE_MASK,		// discard cap lock
		(WORD) 0, K_CHAR_KEY )) && 
		(game.game_mode != GAME_TUTORIAL || tutor.allow_this_hot_key(keyCode)))
   {
		// ####### begin Gilbert 1/6 #########//
//		keyCode = m.lower(keyCode);
		// ####### end Gilbert 1/6 #########//

      switch(keyCode)
      {
		case KEY_ESC:
         set_view_mode(MODE_NORMAL);
			info.display_hot_key = 0;
		   break;

		case 'a':		// find artifact on the ground
			locate_free_item(1);
			break;

		case 'b':		// filter bee and then cycle factory
			// filter bee first
			if( filter_unit(filter_unit_id, UNIT_BEE) )
			{
				// empty
			}
			else
			{
				char firmIdString[] = { FIRM_FACTORY, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		// ####### begin Gilbert 1/6 #########//
		case 'B':		// filter bee and then cycle factory
			// filter out bee
			filter_unit(filter_unit_id, UNIT_BEE, 1);
			break;
		// ####### end Gilbert 1/6 #########//

		case 'c':		// search caravan
			locate_unit_class( UNIT_CLASS_CARAVAN, nation_array.player_recno );
			break;

		case 'd':		// search seat of power
			{
				char firmIdString[] = { FIRM_BASE, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'e':		// cycle spy college
			{
				char firmIdString[] = { FIRM_SPY, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'f':		// search camp, fort, lair, grokken fortress
			{
				char firmIdString[] = { FIRM_FORT, FIRM_CAMP, FIRM_LAIR, FIRM_FORTRESS, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

      case 'g':		// search general
         locate_king_general(RANK_GENERAL);
         break;

		case 'h':		// locate stable
			{
				char firmIdString[] = { FIRM_SPECIAL, FIRM_BEE, FIRM_TERMITE, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'i':		// locate inn
			{
				char firmIdString[] = { FIRM_INN, FIRM_ANIMAL, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
//		case 'i':	//------ toggle extended info display ----//
//			if( ++config.disp_extend_info > 2 )
//				config.disp_extend_info = 0;
			break;

		case 'j':		// jump to a location with natural resource
         site_array.go_to_a_raw_site();
			break;

      case 'k':		// search king
         locate_king_general(RANK_KING);
			break;

      case 'l':		// load game
			load_game();
			break;

		case 'm':		// search market
			{
				char firmIdString[] = { FIRM_MARKET, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'n':		// search offensive structure magic tor, lishorrs
			{
				char firmIdString[] = { FIRM_MAGIC, FIRM_LISHORR,
					FIRM_OFFENSIVE_BUILDING_1, FIRM_OFFENSIVE_BUILDING_2,
					FIRM_OFFENSIVE_BUILDING_3, FIRM_OFFENSIVE_BUILDING_4,	0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

      case 'o':		// bring up the option menu  
			option_menu.enter(!remote.is_enable());
			break;

		// case 'p':		// pause
		//	break;

      case 'q':	// keys for toggling map mode
			{
				int newMapMode = world.map_matrix->map_mode + 1;
				if( newMapMode >= MAP_MODE_COUNT )
					newMapMode = 0;
				world.map_matrix->toggle_map_mode(newMapMode);
			}
         break;

		case 'r':
			locate_royal();
			break;

      case 's':	//---- keys for saving and loading game -----//
         save_game();
         break;

		case 't':		// search tower of science
			{
				char firmIdString[] = { FIRM_RESEARCH, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'u':		// search mine
			{
				char firmIdString[] = { FIRM_MINE, FIRM_ALCHEMY, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'v':		// search war factory and fryhtan strcture
			{
				char firmIdString[] = { FIRM_WAR_FACTORY, FIRM_INCUBATOR, 0 };
				locate_firm_id(firmIdString, nation_array.player_recno);
			}
			break;

		case 'w':		// search wagon
			locate_unit_class( UNIT_CLASS_WAGON, nation_array.player_recno );
			break;

		case 'x':	// clear news messages 
			news_array.clear_news_disp();
			break;

      case 'y':	// locate spy
         locate_spy();
         break;

		case 'z':
			{
				locate_unit_class( UNIT_CLASS_GOD, nation_array.player_recno );
			// if the selected unit is a god, cast magic
				if( unit_array.selected_recno && !unit_array.is_deleted(unit_array.selected_recno) )
				{
					Unit *unitPtr = unit_array[unit_array.selected_recno];
					unitPtr->send_hotkey( scanCode, skeyState );
				}
			}
			break;

		//------ Enter to stop all selected units ----//

		case KEY_TAB:
			unit_group.set_to_selected();
			unit_group.stop(COMMAND_PLAYER);
			break;

		case KEY_RETURN:
			if( remote.is_enable() )		// chat in multiplayer, pause in single player
			{
				// change to chat mode 
				info.nation_report_mode = NATION_REPORT_CHAT;
				set_view_mode(MODE_NATION);
			}
			break;

		//--------- forward/backward tutorial text block --------//

      case ',':
         if( game.game_mode == GAME_TUTORIAL )
            tutor.prev_text_block();
         break;

      case '.':
         if( game.game_mode == GAME_TUTORIAL )
            tutor.next_text_block();
         break;     

//		case KEY_UP:
		case KEY_PGUP:
			if (info.display_hot_key > 0)
				info.display_hot_key = 3;
			else
				if (game.game_mode != GAME_TUTORIAL)
					world.disp_next(-1, 0);    // previous same object type of any nation
			break;

//		case KEY_DOWN:
		case KEY_PGDN:
			if (info.display_hot_key > 0)
				info.display_hot_key = 4;
			else
				if (game.game_mode != GAME_TUTORIAL)
					world.disp_next(1, 0);     // next same object type of any nation
		   break;

//		case KEY_LEFT:
		case KEY_HOME:
			world.disp_next(-1, 1);    // prevous same object type of the same nation
         break;

//		case KEY_RIGHT:
		case KEY_END:
         world.disp_next(1, 1);     // next same object type of the same nation
         break;
      }
   }

	// control key combination, notice that control keys are cheat keys
   if( (keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_CTRL)) )
	{
		const int SCROLL_MAGNITUDE_H = 4;
		const int SCROLL_MAGNITUDE_V = 8;

		switch(keyCode)
		{
		case KEY_UP:
			world.zoom_matrix->scroll(-SCROLL_MAGNITUDE_V,-SCROLL_MAGNITUDE_V);
			sys.zoom_need_redraw = 1;
			break;

		case KEY_DOWN:
			world.zoom_matrix->scroll(SCROLL_MAGNITUDE_V,SCROLL_MAGNITUDE_V);
			sys.zoom_need_redraw = 1;
         break;

		case KEY_LEFT:
			world.zoom_matrix->scroll(-SCROLL_MAGNITUDE_H, SCROLL_MAGNITUDE_H);
			sys.zoom_need_redraw = 1;
         break;

		case KEY_RIGHT:
         world.zoom_matrix->scroll(SCROLL_MAGNITUDE_H,-SCROLL_MAGNITUDE_H);
			sys.zoom_need_redraw = 1;
         break;
		}
	}

}
//--------- End of function Sys::detect_letter_key ---------//


//-------- Begin of function Sys::detect_function_key --------//
//
void Sys::detect_function_key(unsigned scanCode, unsigned skeyState)
{
   int keyCode;

   if( (keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_UNIQUE_KEY)) )
   {
      switch(keyCode)
      {
		case KEY_ESC:
         set_view_mode(MODE_NORMAL);
         break;

		case KEY_F1:
			if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_NATION )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =1;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_NATION);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F2:
			if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_TOWN )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =2;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_TOWN);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F3:
			if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_ECONOMY )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =3;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_ECONOMY);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F4:
         if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_TRADE )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =4;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_TRADE);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F5:
         if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_MILITARY )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =5;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_MILITARY);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F6:
         if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_TECH )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =6;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_TECH);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F7:
         if( game.game_mode != GAME_TUTORIAL || tutor.selectively_detect_flag == 0 )
			{
				if( view_mode==MODE_SPY )
					set_view_mode(MODE_NORMAL);
				else
				// ####### begin Gilbert 19/4 #########//
				{
					int filteredNation =7;
					int chatNationRecno;
					if( (skeyState & SHIFT_KEY_MASK)	
						&& filteredNation <= info.nation_filter()
						&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
					{
						info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
						// BUGHERE : may need to set info.viewer_nation_recno
						info.player_reply_chat(chatNationRecno);
					}
					else
					{
						set_view_mode(MODE_SPY);
					}
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F8:
         if( view_mode==MODE_RANK )
				set_view_mode(MODE_NORMAL);
			else
			// ####### begin Gilbert 19/4 #########//
			{
				int filteredNation =8;
				int chatNationRecno;
				if( (skeyState & SHIFT_KEY_MASK)	
					&& filteredNation <= info.nation_filter()
					&& nation_array[chatNationRecno = info.nation_filter(filteredNation)]->is_remote() )	// shift-f1 go to chat mode directly
				{
					info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
					// BUGHERE : may need to set info.viewer_nation_recno
					info.player_reply_chat(chatNationRecno);
				}
				else
				{
					set_view_mode(MODE_RANK);
				}
			}
			// ####### begin Gilbert 19/4 #########//
         break;
      case KEY_F9:
         if( view_mode==MODE_NEWS_LOG )
            set_view_mode(MODE_NORMAL);
			else
				set_view_mode(MODE_NEWS_LOG);
			break;

		case KEY_F10:
			in_game_menu.enter(!remote.is_enable());
			break;

		case KEY_F11:
			capture_screen();
			break;
		}
	}
}
//--------- End of function Sys::detect_function_key ---------//


//-------- Begin of function Sys::detect_cheat_key --------//
//
void Sys::detect_cheat_key(unsigned scanCode, unsigned skeyState)
{
	if( remote.is_enable() )      // no cheat keys in multiplayer games
      return;

   int keyCode = mouse.is_key( scanCode, skeyState, (WORD) 0, K_IS_CTRL | K_CHAR_KEY );

   if( !keyCode )    // since all keys concern are printable
      return;

   keyCode = m.lower(keyCode);

   switch( keyCode )
   {
      //-------- cheat keys ---------//

      case 'c':      // add cash
         if( nation_array.player_recno )
			{
            (~nation_array)->add_cheat((float)1000);
				(~nation_array)->increased_cash = (float) 1000;
				if( (~nation_array)->is_monster() )
					(~nation_array)->change_live_points((float)1000);
			}
         break;

      case '\\':     // add food
         if( nation_array.player_recno )
            (~nation_array)->add_food((float)1000);
         break;

		case '/':     // add live points
			if( nation_array.player_recno && (~nation_array)->is_monster() )
				(~nation_array)->change_live_points((float)1000);
         break;

		case 't':
			tech_res.inc_all_tech_level(nation_array.player_recno);
         god_res.enable_know_all(nation_array.player_recno);
         // box.msg( "Your technology has advanced.\nYou can now invoke all Greater Beings." );
			box.msg( text_game_menu.str_cheat_tech() );
			break;

      case 'm':
			world.unveil(0, 0, MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1);
         world.visit(0, 0, MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1, 0, 0);
         break;

      case ';':   // increase town population
         if( town_array.selected_recno )
			{
				Town *townPtr = town_array[town_array.selected_recno];
				townPtr->init_pop(10, 100);
				townPtr->auto_set_layout();
			}
			break;

      case 'u':
			config.king_undie_flag = !config.king_undie_flag;

//			if( config.king_undie_flag )
//				box.msg( "Your king is now immortal." );
//			else
//          box.msg( "King immortal mode is now disabled." );
			box.msg( text_game_menu.str_cheat_immortal(config.king_undie_flag) );
         break;

		case '+':
			//---- if it's a seat of power, set pray points to full ----//

			if( firm_array.selected_recno )
			{
				Firm* firmPtr = firm_array[firm_array.selected_recno];

				if( firmPtr->firm_id == FIRM_BASE )
				{
					((FirmBase*)firmPtr)->pray_points = (float) MAX_PRAY_POINTS;
					info.disp();
				}

				if( firmPtr->firm_id == FIRM_RESEARCH )
				{
					Nation *nationPtr;
					if( (nationPtr = firmPtr->nation_ptr())
						&& nationPtr->research_tech_id
						&& nationPtr->research_progress() < 99.0f )
					{
						nationPtr->set_research_progress(99.0f);
					}
				}
			}

			//---- if it's a spy, increase the spy skill to 100 -----//

			else if( unit_array.selected_recno )
			{
				int spyRecno = unit_array[ unit_array.selected_recno ]->spy_recno;

				if( spyRecno )
					spy_array[spyRecno]->spy_skill = 100;
			}
			break;

		case 'b':      // finish building a firm instantly or increase the hit points of a firm to its max
			if( firm_array.selected_recno )
			{
				Firm* firmPtr = firm_array[firm_array.selected_recno];
				firmPtr->hit_points = (float) firmPtr->max_hit_points();
			}

			else if( town_array.selected_recno )
			{
				Town* townPtr = town_array[town_array.selected_recno];
				if( townPtr->under_construction )		// building
					townPtr->construction_completion_percent = 100;
				else if( townPtr->upgrade_wall_progress > 0 )		// building wall
					townPtr->upgrade_wall_progress = MAX_WALL_PROGRESS;
			}
			break;

		case 'z':      // toggle fast_build
			config.fast_build = !config.fast_build;

//			if( !config.fast_build )
//				box.msg( "Fast build is now disabled" );
//			else
//				box.msg( "Fast build is now enabled" );
			box.msg( text_game_menu.str_cheat_fast_build(config.fast_build) );
			break;

		//----- increase Fryhtan battling score -------//

		case '!':
			if( nation_array.player_recno )
				(~nation_array)->kill_monster_score += 100;
			break;
/*
		//----- increase the combat level -------//

		case '[':
			if( unit_array.selected_recno )
			{
				Unit* unitPtr = unit_array[unit_array.selected_recno];

				unitPtr->set_combat_level( MIN(100, unitPtr->combat_level()+20) );
			}
			break;

		//----- increase the skill level of the unit -------//

		case ']':
			if( unit_array.selected_recno )
			{
				Unit* unitPtr = unit_array[unit_array.selected_recno];

				if( unitPtr->skill_level() )
					unitPtr->skill.set_skill_level( MIN(MAX_SKILL_TRAIN, unitPtr->skill_level()+20) );
			}
			break;

		//----- increase the spying skill -------//

		case '\'':
			if( unit_array.selected_recno )
			{
				Unit* unitPtr = unit_array[unit_array.selected_recno];

				if( unitPtr->spy_recno )
				{
					Spy* spyPtr = spy_array[unitPtr->spy_recno];

					spyPtr->spy_skill = MIN(100, spyPtr->spy_skill+20);
				}
			}
			break;
*/
	}
}
//--------- End of function Sys::detect_cheat_key ---------//


//-------- Begin of function Sys::detect_campaign_cheat_key --------//
//
void Sys::detect_campaign_cheat_key(unsigned scanCode, unsigned skeyState)
{
	if( !game.is_campaign_mode() )
		return;

   int keyCode = mouse.is_key( scanCode, skeyState, (WORD) 0, K_CHAR_KEY );

   if( !keyCode )    // since all keys concern are printable
      return;

	//------ win or lose a campaign instantly ------//

	static char campaignCheat=0;

	if( keyCode=='`' )
	{
		campaignCheat = 1;
		return;
	}

	if( keyCode=='@' )
	{
		campaignCheat = 2;
		return;
	}

	if( campaignCheat==1 )
	{
		if( m.lower(keyCode) == 'a' )
		{
			game.campaign()->auto_test_flag = 1;
		}

		//--- shift 1 to 5 - win a campaign game instantly ---//

		else if( keyCode >= '1' &&
			 keyCode <= MIN( '5', '1'+game.win_condition_count-1 ) )
		{
			game.result_win_condition_recno = keyCode - '1';
			game.result_win_condition_id = game.win_condition[game.result_win_condition_recno].condition_id;
			game.game_end(nation_array.player_recno);
		}

		//--- shift 6 to 9 - lose a campaign game instantly ---//

		if( keyCode >= '6' &&
			 keyCode <= MIN( '9', '6'+game.lose_condition_count-1 ) )
		{
			game.result_lose_condition_recno = keyCode - '6';
			game.result_lose_condition_id = game.lose_condition[game.result_lose_condition_recno].condition_id;
			game.game_end(0);
		}
	}

	//---- change campaign difficulty and restart the game ----//

	if( campaignCheat==2 )
	{
		if( keyCode >= '1' && keyCode <= '5' )
		{
			game.campaign()->campaign_difficulty = keyCode-'0';

			sys.signal_exit_flag = 3;
			game.campaign()->replay_stage_flag = 1;
//			game.campaign()->keep_plot_flag = 1;
		}
	}

	campaignCheat = 0;
}
//--------- End of function Sys::detect_campaign_cheat_key ---------//


//-------- Begin of function Sys::detect_debug_cheat_key --------//
//
void Sys::detect_debug_cheat_key(unsigned scanCode, unsigned skeyState)
{
	if( remote.is_enable() )      // no cheat keys in multiplayer games
		return;

	int keyCode = mouse.is_key( scanCode, skeyState, (WORD) 0, K_IS_CTRL | K_CHAR_KEY );

	if( !keyCode )    // since all keys concern are printable
		return;

	keyCode = m.lower(keyCode);

	switch( keyCode )
	{
/*
      case 'j':      // allow all nations to have all god creatures
         for( i=1; i<=nation_array.size() ; i++ )
         {
            if( !nation_array.is_deleted(i) )
               god_res.enable_know_all(i);
         }
			box.msg( "Now knowledge of seat of power is available to all nations." );
			break;
*/
		case 'n':
			// #### begin Gilbert 9/2 #######//
			config.blacken_map = !config.blacken_map;
			// config.fog_of_war  = config.blacken_map;
			// #### end Gilbert 9/2 #######//
			break;

		case '=':      // set default report nation
			if( !(skeyState & SHIFT_KEY_MASK) )
			{
				if( firm_array.selected_recno )
				{
					int nationRecno = firm_array[firm_array.selected_recno]->nation_recno;

					if( nationRecno )
						info.default_viewing_nation_recno = nationRecno;
				}
				else if( town_array.selected_recno )
				{
					int nationRecno = town_array[town_array.selected_recno]->nation_recno;

					if( nationRecno )
						info.default_viewing_nation_recno = nationRecno;
				}
				else if( unit_array.selected_recno )
				{
					int nationRecno = unit_array[unit_array.selected_recno]->nation_recno;

					if( nationRecno )
						info.default_viewing_nation_recno = nationRecno;
				}
			}
         break;

      case 'a':
         if( view_mode==MODE_AI_ACTION )
				set_view_mode(MODE_NORMAL);
         else
            set_view_mode(MODE_AI_ACTION);
         break;

      //-----------------------------------//
/*
      case 't':   // next town layout
         if( town_array.selected_recno )
            town_array[town_array.selected_recno]->auto_set_layout();
         break;
*/
      //-------------------------------//

		case 'h':		// locate independent hero
			locate_independent_hero();
			break;

		case 'i':			// locate items on the map
			locate_free_item(0);
			break;

		case 'j':
			magic.test_flag();
			break;
/*
		case 'i':
			config.disable_ai_flag = !config.disable_ai_flag;

			if( config.disable_ai_flag )
				box.msg( "AI is now disabled" );
			else
				box.msg( "AI is now enabled" );
			break;
*/
		case 'd':
			config.show_ai_info = !config.show_ai_info;
			config.show_debug_info = config.show_ai_info;

			info.disp();

			//if( config.show_ai_info )
			//	box.msg( "Now AI info will be displayed." );
			//else
			//	box.msg( "Now AI info will not be displayed." );
			box.msg( text_game_menu.str_cheat_ai_info(config.show_ai_info) );
			break;
/*
		case '/':
			config.show_all_unit_icon = !config.show_all_unit_icon;

			if( config.show_all_unit_icon )
				box.msg( "Now all unit icons will be displayed." );
			else
				box.msg( "Now all unit icons will not be displayed." );
			break;
*/
#ifdef DEBUG
/*
		case '`':
         sys.testing_session = !sys.testing_session;

         if( sys.testing_session )
            box.msg( "sys.testing_session is now 1." );
         else
            box.msg( "sys.testing_session is now 0." );
         break;

      case '\r':
         if(debug2_enable_flag)
				debug2_enable_flag = 0;
			else
				debug2_enable_flag = 1;
			break;

	  //-*********** syn game test ***********-//
      case '\'':
         //if(debug2_enable_flag && debug_sim_game_type)
			//game_file_array[0]->load_game("syn.sav");
         game_file.load_game("syn.sav");
         sp_load_seed_file();
         debug_seed_status_flag = DEBUG_SYN_AUTO_LOAD;
			break;

      case '[':
         if(m.is_file_exist("SYN.SYS"))
			{
            debug_seed_status_flag = DEBUG_SYN_AUTO_SAVE;
            sp_seed_pos_reset();
            sp_record_match_seed();
            sp_create_seed_file("nseed.rs");

            game_file.save_game("syn.sav");
         }
         break;

      case ']':
         if(debug_seed_status_flag==NO_DEBUG_SYN)
         {
            if(m.is_file_exist("SYN.SYS"))
            {
               debug_seed_status_flag = DEBUG_SYN_LOAD_AND_COMPARE_ONCE;
               game_file.load_game("syn.sav");
               sp_load_seed_file();
            }
            else
					debug_seed_status_flag = NO_DEBUG_SYN;
         }
         break;
*/       //-*********** syn game test ***********-//
#endif
	}

	//---- F12 - restart the campaign game ----//

	if( game.is_campaign_mode() && mouse.is_key( scanCode, skeyState, KEY_F12, 0) )
	{
		sys.signal_exit_flag = 3;
		game.campaign()->replay_stage_flag = 1;
	}
}
//--------- End of function Sys::detect_debug_cheat_key ---------//


//-------- Start of function detect_scenario_cheat_key -------------//
//
// Cheat keys for designing scenarios.
//
static int detect_scenario_cheat_key(unsigned scanCode, unsigned skeyState)
{
   if( remote.is_enable() )      // no cheat keys in multiplayer games
      return 0;

   int keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, K_IS_CTRL | K_IS_ALT );	// control shift key

	if( !keyCode )
      return 0;

   //------------------------------------------//

   int keyProcessed = 0;

   Firm *firmPtr;
   Unit *unitPtr;
   Town *townPtr;
   Nation *nationPtr;
   Site *sitePtr;
   Location *locPtr;
   int i, j, curXLoc, curYLoc;

   switch(keyCode)
   {
      case 'p': //-------- get scroll of power for the race of selected unit --------//
         if(unit_array.selected_recno)
         {
            unitPtr = unit_array[unit_array.selected_recno];
            if(unitPtr->nation_recno==nation_array.player_recno && unitPtr->race_id)
            {
               god_res[unitPtr->race_id]->enable_know(unitPtr->nation_recno);
               //box.msg( "Get Scroll of Power of selected unit for his race" );
               keyProcessed++;
            }
			}
         keyProcessed++;
         break;

		case 't': //-------- get all technology except scrolls of power -------//

			tech_res.inc_all_tech_level(nation_array.player_recno);
			//box.msg( "Your technology has advanced." );
			keyProcessed++;
			break;

		case 'q': //-- decrease population of a selected race in a selected village by 10 --//
         if(town_array.selected_recno)
         {
            townPtr = town_array[town_array.selected_recno];
            if(townPtr->nation_recno == nation_array.player_recno)
            {
					// i = townPtr->get_selected_race();
					i = townPtr->race_id;
               if(i && townPtr->population)
               {
                  for(j=10; j>0 && !town_array.is_deleted(townPtr->town_recno); --j)
                     townPtr->kill_town_people(0);

                  //box.msg( "Population decrease by 10." );
                  keyProcessed++;
               }
               townPtr->auto_set_layout();
            }
         }
         keyProcessed++;
         break;

		case 'e': //-------- decrease the reputation by 10 -----------//
         nationPtr = nation_array[nation_array.player_recno];
         nationPtr->reputation -= 10;
         if(nationPtr->reputation < -100)
            nationPtr->reputation = (float) -100;

         //box.msg( "Reputation decrease by 10." );
         keyProcessed++;
         break;

      case 'r': //-------- increase the reputation by 10 -----------//
         nationPtr = nation_array[nation_array.player_recno];
         nationPtr->reputation += 10;
         if(nationPtr->reputation > 100)
            nationPtr->reputation = (float) 100;

         //box.msg( "Reputation increase by 10." );
			keyProcessed++;
			break;

      case 'j': //--------- damage a building by 20 pt -----------//
         if(firm_array.selected_recno)
         {
            firmPtr = firm_array[firm_array.selected_recno];
            if(firmPtr->nation_recno==nation_array.player_recno)
            {
               firmPtr->hit_points -= 20;
               if(firmPtr->hit_points < 1)
                  firmPtr->hit_points = (float) 1;
					//box.msg( "damage firm by 20 points." );
               keyProcessed++;
            }
         }
         keyProcessed++;
         break;

      case 'k': //--------- repair a building by 20 pt -----------//
         if(firm_array.selected_recno)
         {
            firmPtr = firm_array[firm_array.selected_recno];
            if(firmPtr->nation_recno==nation_array.player_recno)
            {
               firmPtr->hit_points += 20;
               if(firmPtr->hit_points > firmPtr->max_hit_points())
                  firmPtr->hit_points = (float) firmPtr->max_hit_points();
               //box.msg( "Repair firm by 20 points." );
               keyProcessed++;
            }
         }
         keyProcessed++;
         break;

      case 'x': //------ decrease cash by 1000 --------//
			nationPtr = nation_array[nation_array.player_recno];
         nationPtr->cash -= 1000;
         if(nationPtr->cash < 0)
            nationPtr->cash = (float) 0;
         //box.msg( "Decrease cash by 1000." );
         keyProcessed++;
         break;

      case 'c': //------ decrease food by 1000 --------//
			nationPtr = nation_array[nation_array.player_recno];
         nationPtr->food -= 1000;
         if(nationPtr->food < 0)
            nationPtr->food = (float) 0;
         //box.msg( "Decrease food by 1000." );
         keyProcessed++;
         break;

      case 'm': //----- add natural resource to cursor pos / remove existing resource ------//
         if(get_mouse_loc_in_zoom_map(curXLoc, curYLoc))
         {
            locPtr = world.get_loc(curXLoc, curYLoc);
				if(locPtr->has_site()) // remove site
            {
               i = locPtr->site_recno();
               sitePtr = site_array[i];
               if(!sitePtr->has_mine)
               {
                  site_array.del_site(i);
                  //box.msg( "Site deleted." );
                  keyProcessed++;
               }
            }
            else if(locPtr->can_build_site(1) && !locPtr->is_power_off()) // add site
            {
               i = MAX_RAW_RESERVE_QTY * (50 + m.random(50)) / 100;
               site_array.add_site(curXLoc, curYLoc, SITE_RAW, m.random(MAX_RAW)+1, i);
               //box.msg( "Site added." );
               keyProcessed++;
            }
         }
         keyProcessed++;
         break;

		case 'b': //------------ add reserve of natural resource by 100 ----------//
			if(get_mouse_loc_in_zoom_map(curXLoc, curYLoc))
			{
            locPtr = world.get_loc(curXLoc, curYLoc);
            if(locPtr->has_site())
            {
               i = locPtr->site_recno();
               sitePtr = site_array[i];
               if(!sitePtr->has_mine)
               {
						sitePtr->reserve_qty += 100;
                  //box.msg( "increase reserve by 100." );
                  keyProcessed++;

                  info.disp();
               }
            }
         }
         keyProcessed++;
         break;

      case 'v': //------------ reduce reserve of natural resource by 100 ----------//
         if(get_mouse_loc_in_zoom_map(curXLoc, curYLoc))
			{
            locPtr = world.get_loc(curXLoc, curYLoc);
            if(locPtr->has_site())
            {
               i = locPtr->site_recno();
               sitePtr = site_array[i];
               if(!sitePtr->has_mine && sitePtr->reserve_qty>100)
               {
                  sitePtr->reserve_qty -= 100;
                  //box.msg( "reduce reserve by 100." );
                  keyProcessed++;

                  info.disp();
               }
            }
         }
         keyProcessed++;
			break;

		case 'h':
			hide_map();
			keyProcessed++;
			break;

      case 'z': //------------ put the selected unit to the cursor position ------------//
         if(unit_array.selected_recno)
         {
            unitPtr = unit_array[unit_array.selected_recno];
            if(get_mouse_loc_in_zoom_map(curXLoc, curYLoc))
            {
               if(unitPtr->mobile_type!=UNIT_LAND)
               {
						curXLoc = (curXLoc/2) * 2;
                  curYLoc = (curYLoc/2) * 2;
               }

               locPtr = world.get_loc(curXLoc, curYLoc);
               if(locPtr->can_move(unitPtr->mobile_type))
					{
						world.get_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc() )->remove_unit( unitPtr->mobile_type );
						unitPtr->stop_order();
						unitPtr->next_x = curXLoc << ZOOM_X_SHIFT_COUNT;
						unitPtr->next_y = curYLoc << ZOOM_Y_SHIFT_COUNT;
						unitPtr->cur_x = unitPtr->go_x = unitPtr->next_x;
						unitPtr->cur_y = unitPtr->go_y = unitPtr->next_y;
						world.get_loc(curXLoc, curYLoc)->set_unit( unitPtr->sprite_recno, unitPtr->mobile_type );
						keyProcessed++;
               }
            }
         }
         keyProcessed++;
         break;

      case 's':
         if( info.default_viewing_nation_recno &&
             nation_array.player_recno &&
             nation_array[info.default_viewing_nation_recno]->is_ai() )
         {
            nation_array[info.default_viewing_nation_recno]->surrender(nation_array.player_recno);
			}
         keyProcessed++;
         break;
   }

	return keyProcessed;
}
//--------- End of function detect_scenario_cheat_key ---------------//


//-------- Begin of function Sys::detect_set_speed --------//
//
int Sys::detect_set_speed(unsigned scanCode, unsigned skeyState)
{
	int keyCode = mouse.is_key( scanCode, skeyState, (WORD) 0, K_CHAR_KEY );

	if( !keyCode )    // since all keys concern are printable
		return 0;

	//------ get the current speed mode -------//

	int curSpeedMode;
	int newSpeedMode = -1;

	if( config.frame_speed==99 )
		curSpeedMode = 9;
	else
		curSpeedMode = config.frame_speed / 3;

	//----- '[' to decrease game speed -----//

	if( keyCode == '[' && curSpeedMode > 1 )
	{
		newSpeedMode = curSpeedMode-1;
		info.game_speed_info_display_delay = 10;

		if(newSpeedMode == 0)
			info.game_pause_info_display_delay = -1;
	}

	//----- ']' to increase game speed -----//

	else if( keyCode == ']' && curSpeedMode < 9 )
	{
		newSpeedMode = curSpeedMode+1;
		info.game_speed_info_display_delay = 10;

		if(curSpeedMode == 0)
			info.game_pause_info_display_delay = 10;
	}

	//----- 'P' or space to pause and resume game ----//

	else if( m.lower(keyCode)=='p' || keyCode==' ' )
	{
		if( curSpeedMode == 0 )		// resume game
		{
			newSpeedMode = last_speed_mode;

			if( newSpeedMode == 0 )		// if the last speed mode is also 0, set to 3, the normal speed
				newSpeedMode = 3;

			info.game_pause_info_display_delay = 10;
		}
		else		// pause game
		{
			newSpeedMode = 0;
			last_speed_mode = curSpeedMode;
			info.game_pause_info_display_delay = -1;
		}
	}

	//------ set the speed now -------//

	if( newSpeedMode >= 0 )
	{
		if( newSpeedMode == 9 )
			set_speed(99);
		else
			set_speed(newSpeedMode * 3);

		return 1;
	}

	return 0;
}
//--------- End of function Sys::detect_set_speed ---------//


//--------- Begin of function Sys::detect_key_str --------//
//
// Detect for continous input of a string from the keyboard
//
// <int>   keyStrId = the id. of the key string
//                    each id has its individual key_str_pos
// <char*> keyStr   = the string to detect
//
// return : <int> 1 - complete string detected
//                0 - not detected
//
int Sys::detect_key_str(int keyStrId, const char* keyStr)
{
   err_when( keyStrId < 0 || keyStrId >= MAX_KEY_STR );

   unsigned char* keyStr2 = (unsigned char*) keyStr;

   if( mouse.key_code == keyStr2[key_str_pos[keyStrId]] )
      key_str_pos[keyStrId]++;
   else
      key_str_pos[keyStrId]=0;    // when one key unmatched, reset the counter

	if( key_str_pos[keyStrId] >= (int) strlen(keyStr) )
   {
      key_str_pos[keyStrId]=0;    // the full string has been entered successfully without any mistakes
      return 1;
   }
   else
      return 0;
}
//----------- End of function Sys::detect_key_str --------//


//-------- Start of function locate_king_general -------------//
//
static void locate_king_general(int rankId)
{
	if( !nation_array.player_recno )
		return;

	int unitRecno = 0;
	if(unit_array.selected_recno)
		unitRecno = unit_array.selected_recno;
	else if(rankId!=RANK_KING && firm_array.selected_recno)
	{
		FirmCamp *firmCamp = firm_array[firm_array.selected_recno]->cast_to_FirmCamp();
		if( firmCamp && firmCamp->overseer_recno )
		{
			unitRecno = firmCamp->overseer_recno;
		}
   }

   for( int i=unit_array.size() ; i>0 ; i-- )
   {
      if( ++unitRecno > unit_array.size() )
         unitRecno = 1;

      if( unit_array.is_deleted(unitRecno) )
         continue;

      Unit* unitPtr = unit_array[unitRecno];

      if( unitPtr->nation_recno == nation_array.player_recno &&
          unitPtr->rank_id == rankId )
      {
         short xLoc, yLoc;

			if( unitPtr->get_cur_loc(xLoc, yLoc) )
         {
            world.go_loc(xLoc, yLoc, 1);
            return;
         }
      }
   }
}
//--------- End of function locate_king_general ---------------//


//-------- Start of function locate_spy -------------//
//
static void locate_spy()
{
   if( !nation_array.player_recno )
      return;

   int unitRecno = unit_array.selected_recno;

   for( int i=unit_array.size() ; i>0 ; i-- )
	{
      if( ++unitRecno > unit_array.size() )
         unitRecno = 1;

      if( unit_array.is_deleted(unitRecno) )
         continue;

      Unit* unitPtr = unit_array[unitRecno];

      if( unitPtr->true_nation_recno() == nation_array.player_recno &&
          unitPtr->spy_recno && unitPtr->is_visible() )
      {
         short xLoc, yLoc;

         if( unitPtr->get_cur_loc(xLoc, yLoc) )
         {
            world.go_loc(xLoc, yLoc, 1);
            return;
         }
      }
   }
}
//--------- End of function locate_spy ---------------//


// -------- begin of function locate_royal -----------//
static void locate_royal()
{
	if( !nation_array.player_recno )
		return;

	if( !game.is_campaign_mode() )
		return;

	int unitRecno = unit_array.selected_recno;
	int firmRecno = firm_array.selected_recno;
	int selectedSoldierId = 0;
	int maxSoldier;

	if( firmRecno && !firm_array.is_deleted(firmRecno) )
	{
		if( firm_array[firmRecno]->cast_to_FirmCamp() )
		{
			selectedSoldierId = firm_array[firmRecno]->cast_to_FirmCamp()->selected_soldier_id;
		}
	}
	int initSelectedSoldierId = selectedSoldierId;

	int royalFound = 0;
	int loopCount = 5000;

	int findRoyalFlag = 0;
	while(--loopCount > 0 )
	{
		err_when( unitRecno && firmRecno );		// either unit and firm is selected

		// go to next candidate

		if( unitRecno )
		{
			if( !unit_array.is_deleted(unitRecno) && unit_array[unitRecno]->is_visible() )
			{
				// not include general in fort
				if( findRoyalFlag && unit_array[unitRecno]->is_royal )
				{
					royalFound = 1;
					break;
				}

			}

			if( ++unitRecno > unit_array.size() )
			{
				firmRecno = 1;		// find royal in firm in next turn
				selectedSoldierId = 0;
				unitRecno = 0;
			}
		}
		else if( firmRecno )
		{
			int nextFirm = 1;				// clear to indicate this firm may have royal

			if( !firm_array.is_deleted(firmRecno) )
			{
				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();
					maxSoldier = camp->soldier_count;
					nextFirm = 0;

					// check general or any soldier is royal
					if( findRoyalFlag )
					{
						if( selectedSoldierId == 0 )
						{
							if( camp->overseer_recno && unit_array[camp->overseer_recno]->is_royal )
							{
								royalFound = 1;
								break;
							}
						}
						else if( selectedSoldierId <= maxSoldier )
						{
							if( camp->soldier_array[selectedSoldierId-1].is_royal )
							{
								royalFound = 1;
								break;
							}
						}
					}
				}
				// else other kind of firms...
			}

			if( !nextFirm )
			{
				if( ++selectedSoldierId > maxSoldier )
				{
					nextFirm = 1;
				}
			}

			if( nextFirm )
			{
				selectedSoldierId = 0;
				if( ++firmRecno > firm_array.size() )
				{
					firmRecno = 0;		// find royal in unit array
					unitRecno = 1;
				}
			}
		}

		// determine loop end condition

		if( unitRecno && unit_array.selected_recno == unitRecno
			|| firmRecno && firm_array.selected_recno == firmRecno && selectedSoldierId == initSelectedSoldierId )
		{
			break;
		}

		findRoyalFlag = 1;		// find royal start from the second iteration
	} 

	if( royalFound )
	{
		err_when( unitRecno && firmRecno );		// either unit and firm is selected

		if( firmRecno )
		{
			Firm *firmPtr = firm_array[(firm_array.selected_recno = firmRecno)];

			// ----- set selected soldier id -------//

			if( firmPtr->cast_to_FirmCamp() )
			{
				firmPtr->cast_to_FirmCamp()->selected_soldier_id = selectedSoldierId;
			}
			world.go_loc( firmPtr->center_x, firmPtr->center_y, 1);
		}
		else if( unitRecno )
		{
         short xLoc, yLoc;
			if( unit_array[unitRecno]->get_cur_loc(xLoc, yLoc) )
         {
            world.go_loc(xLoc, yLoc, 1);
         }
		}
	}
}
// -------- end of function locate_royal -----------//


//-------- Start of function get_mouse_loc_in_zoom_map -------------//

static int get_mouse_loc_in_zoom_map(int &x, int &y)
{
	int mouseX = mouse.cur_x;
	int mouseY = mouse.cur_y;
	if(mouseX >= ZOOM_X1 && mouseX <= ZOOM_X2 && mouseY >= ZOOM_Y1 && mouseY <= ZOOM_Y2)
	{
		char dummy;
		return world.zoom_matrix->get_detect_location(mouseX, mouseY, &x, &y, &dummy);
	}

	return 0; // out of zoom map boundary
}
//--------- End of function get_mouse_loc_in_zoom_map ---------------//


//-------- Start of function hide_map -------------//
//
// Hide map except for areas around your village, people.
//
static void hide_map()
{
   Location *locPtr;
   int i, j;

    if( config.explore_whole_map )      // no action if the setting of the map is explored
		return;

//	vga_back.bar(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2, UNEXPLORED_COLOR);
	for(j=0; j<MAX_WORLD_Y_LOC; ++j)
	{
		locPtr = world.get_loc(0, j);
		for(i=0; i<MAX_WORLD_X_LOC; ++i, locPtr++)
			locPtr->explored_off();
	}

	// initialize relation

	if( nation_array.player_recno )
	{
		for(i=1; i<=nation_array.size(); ++i)
		{
			if( i==nation_array.player_recno || nation_array.is_deleted(i) )	// reject player
				continue;

			(~nation_array)->init_relation(i);
			nation_array[i]->init_relation(nation_array.player_recno);
		}
	}

	// re-explore

	for(i = base_obj_array.size(); i > 0; --i )
	{
		if( base_obj_array.is_deleted(i) )
			continue;

		BaseObj *baseObj = base_obj_array[i];
		if( baseObj->is_visible() && baseObj->explore_for_player() )
		{
			// unit has special functions to explore
			if( baseObj->cast_to_Unit() )
			{
				baseObj->cast_to_Unit()->explore_area();
			}
			else
			{
				world.unveil( baseObj->obj_loc_x1(), baseObj->obj_loc_y1(), baseObj->obj_loc_x2(), baseObj->obj_loc_y2() );
				world.visit( baseObj->obj_loc_x1(), baseObj->obj_loc_y1(), baseObj->obj_loc_x2(), baseObj->obj_loc_y2(), EXPLORE_RANGE-1 );
			}
		}
	}

	sys.map_need_redraw = 1;
	sys.zoom_need_redraw = 1;
}
//--------- End of function hide_map ---------------//


//------- Start of function locate_independent_hero ---------//
//
static void locate_independent_hero()
{
	int unitRecno = unit_array.selected_recno;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( ++unitRecno > unit_array.size() )
			unitRecno = 1;

		if( unit_array.is_deleted(unitRecno) )
			continue;

		Unit* unitPtr = unit_array[unitRecno];

		if( unitPtr->hero_id && unitPtr->is_visible() && unitPtr->nation_recno==0 )
		{
			world.go_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc(), 1);
			return;
		}
	}
}
//--------- End of function locate_independent_hero ---------//


//------- Start of function locate_free_item ---------//
//
static void locate_free_item(int checkExplore)
{
	int siteRecno = site_array.selected_recno;

	for( int i=site_array.size() ; i>0 ; i-- )
	{
		if( ++siteRecno > site_array.size() )
			siteRecno = 1;

		if( site_array.is_deleted(siteRecno) )
			continue;

		Site* sitePtr = site_array[siteRecno];

		if( sitePtr->site_type == SITE_ITEM
			&& (!checkExplore || world.is_explored( sitePtr->map_x_loc, sitePtr->map_y_loc, sitePtr->map_x2, sitePtr->map_y2)) )
		{
			// if an unit is stepping on the item,
			// unit is selected after world.go_loc, instead of the site
			// then this can break the chain
			world.go_loc(sitePtr->map_x_loc, sitePtr->map_y_loc, GO_LOC_SELECT_SITE);
			return;
		}
	}
}
//--------- End of function locate_free_item ---------//


// -------- begin of function locate_unit_class -------//
//
// nationRecno == -1 for all nations
static void	locate_unit_class(int unitClass, int nationRecno)
{
	int unitRecno = unit_array.selected_recno;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( ++unitRecno > unit_array.size() )
			unitRecno = 1;

		if( unit_array.is_deleted(unitRecno) )
			continue;

		Unit* unitPtr = unit_array[unitRecno];

		if( unit_res[unitPtr->unit_id]->unit_class == unitClass &&
			(nationRecno == -1 || unitPtr->nation_recno == nationRecno) )
		{
			world.go_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc(), 
				unitPtr->mobile_type == UNIT_AIR ? GO_LOC_SELECT_AIR : GO_LOC_SELECT_LAND );
			return;
		}
	}
}
// -------- end of function locate_unit_id -------//


// -------- begin of function locate_firm_id -------//
//
// <char *>firmIdString		- firm id to will be selected
// [int] nationRecno			- nation to be selected, -1 for all
//
static void locate_firm_id(char *firmIdString, int nationRecno)
{
	int firmRecno = firm_array.selected_recno;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( ++firmRecno > firm_array.size() )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm* firmPtr = firm_array[firmRecno];

		if( strchr(firmIdString, (char)firmPtr->firm_id) 
			&& (nationRecno == -1 || firmPtr->nation_recno == nationRecno) )
		{
			world.go_loc(firmPtr->center_x, firmPtr->center_y, GO_LOC_SELECT_FIRM );
			return;
		}
	}
}
// -------- end of function locate_firm_id -------//


// ------- begin of function filter_unit_id --------//
//
static int filter_unit_id(Unit *unitPtr, int para1)
{
	return unitPtr->unit_id == para1;
}
// ------- end of function filter_unit_id --------//


// ------- begin of function filter_unit ---------//
//
// <int> para1         - a number passed to filterFunc
// [int] negate        - if true, select if filterFunc return false
//
static int filter_unit( FilterUnitFunc filterFunc, int para1, int negate )
{
	if( !unit_array.selected_recno )
		return 0;

	int unitCount = 0;
	int i;
	for(i = unit_array.size(); i > 0; --i )
	{
		if( unit_array.is_truly_deleted(i) )
			continue;
		Unit *unitPtr = unit_array[i];
		if( !unitPtr->selected_flag )
			continue;

		if( !negate && (*filterFunc)(unitPtr, para1)		// negate xor (*filterFunc)(...)
			|| negate && !(*filterFunc)(unitPtr, para1) )
			unitCount++;
	}

	if( unitCount == 0 )
		return 0;		// don't change the unit selected

	for(i = 1; i <= unit_array.size(); ++i )
	{
		if( unit_array.is_truly_deleted(i) )
			continue;
		Unit *unitPtr = unit_array[i];
		if( !unitPtr->selected_flag )
			continue;
		if( !negate && !(*filterFunc)(unitPtr, para1)		// negate eqv (*filterFunc)(...)
			|| negate && (*filterFunc)(unitPtr, para1) )
		{
			unitPtr->deselect();
		}
	}

	return 1;
}
// ------- end of function filter_unit ---------//
