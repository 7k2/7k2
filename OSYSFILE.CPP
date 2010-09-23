//Filename    : OSYSFILE.CPP
//Description : class Sys - file functions

#include <stdio.h>
#include <ALL.H>
#include <OSYS.H>
#include <OREMOTE.H>
#include <OMOUSE.H>
#include <OBOX.H>
#include <OGFILE.H>
#include <OT_GMENU.H>

//-------- Begin of function Sys::load_game --------//
//
void Sys::load_game()
{
   //--- load game not enabled in multiplayer game ---//

   if( remote.is_enable() )
      return;

   signal_exit_flag=1;     // for deinit functions to recognize that this is an end game deinitialization instead of a normal deinitialization

   int rc=0;

   // game_file_array.init("*.SAV");		// reload any save game file
   game_file_array.reload();		// reload any save game file
   game_file_array.menu(-2);               // save screen area to back buffer
   switch( game_file_array.menu(2) )
   {
      case 1:
         rc = 1;                 
			break;

      case 0:
         signal_exit_flag = 0;
         break;
         // case -1 and otherwise, left sys.signal_exit_flag 1 to exit the game
   }

   game_file_array.menu(-1);               // restore screen area from back buffer

   //-----------------------------------//
   if( rc == -1)
   {
      box.msg( text_game_menu.str_load_game_fail() ); //"Fail Loading Game" );
      return;
   }

   if( rc )    // if rc==0, leave signal_exit_flag 1, which the game will then quit
   {
      need_redraw_flag = 1;
      disp_frame();
      // disp_view_mode();
//    box.msg( "Game Loaded Successfully" );
      info.disp();
   }
}
//--------- End of function Sys::load_game ---------//


//-------- Begin of function Sys::save_game --------//
//
void Sys::save_game()
{
   if( nation_array.player_recno==0 )     // cannot save game when the player's kingdom has been destroyed
      return;

   if( remote.is_enable() )
   {
      DWORD *dwordPtr = (DWORD *)remote.new_send_queue_msg( MSG_REQUEST_SAVE, sizeof(DWORD) );
      *dwordPtr = remote.next_send_frame(nation_array.player_recno, sys.frame_count+remote.process_frame_delay)+2;
      return;
   }

   // game_file_array.init("*.SAV");                  // reload any save game file
   game_file_array.reload();                  // reload any save game file
   game_file_array.menu(-2);               // save screen area to back buffer

   if( game_file_array.menu(1) == 1 )
   {
      if( GameFile::last_read_success_flag )
         box.msg( text_game_menu.str_save_success() ); // "Game Saved Successfully" );
   }

   game_file_array.menu(-1);               // restore screen area from back buffer

	// ##### patch begin Gilbert 16/3 #######//
	info.disp();
	// ##### patch end Gilbert 16/3 #######//
}
//-------- End of function Sys::save_game --------//


//-------- Begin of function Sys::auto_save --------//
//
void Sys::auto_save()
{
	if( nation_array.player_recno == 0 )
      return;

   //---------- single player auto save ----------//

   if( !remote.is_enable() &&          // no auto save in a multiplayer game
       info.game_month%2==0 && info.game_day==1 && day_frame_count==0)
	{
/*    #ifdef DEBUG2
		if(1)
		#else
		if( sys.debug_session || sys.testing_session )
		#endif
*/

      game_file_array.auto_save();

      //-*********** syn game test ***********-//
      #ifdef DEBUG
         if(debug_seed_status_flag==DEBUG_SYN_AUTO_SAVE)
         {
            sp_write_seed();
            sp_close_seed_file();

            //if(info.game_date-info.game_start_date<=365)
            if(0)
            {
               sp_open_seed_file("nseed.rs");
               debug_seed_status_flag = DEBUG_SYN_AUTO_SAVE; // continue recording
            }
            else
            {
               debug_seed_status_flag = NO_DEBUG_SYN;
               //sp_load_seed_file();
               //SendMessage(main_hwnd, WM_KEYDOWN, 'L', 0);
               mouse.add_key_event(DIK_BACKSLASH, m.get_time()); // load file for comparison
            }
         }

         //debug_seed_status_flag = 2;
         //sp_seed_pos_reset();
         //sp_record_match_seed();
      #endif
      //-*********** syn game test ***********-//
   }

   // --------- multiplayer autosave game --------//

	// ###### patch begin Gilbert 23/1 #######//
   if( remote.is_enable() && remote.sync_test_level >= 0 &&			// disable autosave after un-sync
      day_frame_count==0 && info.game_day==1 && info.game_month%2==0 )
	// ###### patch end Gilbert 23/1 #######//
   {
      game_file_array.auto_save();
   }
}
//-------- End of function Sys::auto_save --------//


// --------- begin of function Sys::mp_request_save ----------//
void Sys::mp_request_save(DWORD frame)
{
	if( !mp_save_flag )
	{
		mp_save_flag = 1;
		mp_save_frame = frame;
	}
}
// --------- end of function Sys::mp_request_save ----------//


// --------- begin of function Sys::mp_clear_request_save ----------//
void Sys::mp_clear_request_save()
{
	mp_save_flag = 0;
	mp_save_frame = 0;
}
// --------- end of function Sys::mp_clear_request_save ----------//

