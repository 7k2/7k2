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
#include <omodeid.h>
#include <oconfig.h>
#include <omouse.h>
#include <omousecr.h>
#include <oremote.h>
#include <opower.h>
#include <owaypnt.h>
#include <ocampgn.h>
#include <ovga.h>
#include <ovgalock.h>
#include <oingmenu.h>
#include <ooptmenu.h>
#include <ose.h>
#include <ogame.h>
#include <ofirmdie.h>
#include <otalkres.h>
#include <oherores.h>
#include <ounit.h>
#include <obullet.h>
#include <ofirm.h>
#include <otown.h>
#include <otornado.h>
#include <owarpt.h>
#include <osite.h>
#include <orebel.h>
#include <osnowg.h>
#include <orock.h>
#include <oeffect.h>
#include <oweather.h>
#include <oevent.h>
#include <olog.h>
#include <ospy.h>
#include <ofont.h>
#include <ocrc_sto.h>
#include <oerrctrl.h>
#include <omusic.h>
#include <oaudio.h>
#include <ogfile.h>
#include <olog.h>
#include <olonglog.h>
#include <onews.h>
#include <onation.h>
#include <oseditor.h>
#include <otips.h>
#include <oprofile.h>
#include <ot_gmenu.h>


//----------- Define static variables ------------//

static unsigned long last_frame_time=0, last_resend_time=0;
static char          remote_send_success_flag=1;

//-------- Begin of function Sys::run --------//
//
void Sys::run(int isLoadedGame)
{
	sys_flag  = SYS_RUN;
	view_mode = MODE_NORMAL;

#ifdef DEBUG_JULIET
	config.show_debug_info = TRUE;
#endif

	// ###### begin Gilbert 21/5 #########//
	if( !remote.is_enable() )
	{
		if( scenario_cheat_flag )
		{
			scenario_editor.init();
			scenario_editor.enable_flag = 1;
		}
		else
		{
			if( game.game_mode != GAME_TUTORIAL && !isLoadedGame )
			{
				// ------- set tips -------//
				if( config.race_id > 0 )
					tips_res.get_class( TIPS_CLASS_HUMAN )->set_active();
				if( config.race_id < 0 )
					tips_res.get_class( TIPS_CLASS_FRYHTAN )->set_active();
				if( game.is_campaign_mode() )
					tips_res.get_class( TIPS_CLASS_CAMPAIGN )->set_active();
				tips_res.random_select();
				tips_res.box_active = !player_profile.hide_tips 
					|| (mouse.skey_state & SHIFT_KEY_MASK);
				// sys.scenario_cheat_flag means scenario editor is going to enter
				// and scenario editor menu overlapped with tips box
			}
		}
	}
	// ###### end Gilbert 21/5 #########//

	// ###### begin Ban 13/7 #########//
	if ( remote.is_enable() )
	{
		if( !isLoadedGame )
		{
			// ------- set tips -------//
			if( config.race_id > 0 )
				tips_res.get_class( TIPS_CLASS_HUMAN )->set_active();
			if( config.race_id < 0 )
				tips_res.get_class( TIPS_CLASS_FRYHTAN )->set_active();
			tips_res.random_select();
			tips_res.box_active = !player_profile.hide_tips 
				|| (mouse.skey_state & SHIFT_KEY_MASK);			
		}
	}
	// ###### end Ban 13/7 #########//

	//------ reset mouse ---------//

	mouse.clear_event();
	mouse_cursor.set_frame(0);

	//-------- play music ---------//

	music.play( music.random_bgm_track(music.song_id), sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );

	//-- enable power after the game objets has been initialized --//

   power.enable();      // enable power, which handle mouse inputs

   //----- sys::disp_frame() will redraw everything when this flag is set to 1 ----//

   sys.need_redraw_flag = 1;

	// ##### begin Gilbert 18/1 #######//
//	option_menu.active_flag = 0;
	option_menu.clear_active();
//	in_game_menu.active_flag = 0;
	in_game_menu.clear_active();
	// ##### end Gilbert 18/1 #######//

	sys.disp_frame(game.is_campaign_mode() && !isLoadedGame);		

	//----------- run the main loop -----------//

	main_loop(isLoadedGame);

	scenario_editor.deinit();

	//-----------------------------------------//

	m.unlock_seed();
}
//--------- End of function Sys::run --------//


//-------- Begin of function Sys::main_loop --------//
//
void Sys::main_loop(int isLoadedGame)
{
	MSG msg;

	//-------- reset day_frame_count -------//

	if( !isLoadedGame )
	{
		day_frame_count = 0;       // for determining when the day counter should be increased.
		frame_count = 1;
	}

	//----- initialize these vars for every game -----//

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( !nation_array.is_deleted(i) )
			nation_array[i]->next_frame_ready = 0;
	}

	remote.packet_send_count    = 0;
	remote.packet_receive_count = 0;

	last_frame_time = m.get_time()+60000;     // plus 60 seconds buffer for game loading/starting time
	is_sync_frame   = 0;

	//----------------------------------------------//
	mp_clear_request_save();
	remote.enable_poll_msg();
	remote.enable_process_queue();
	remote_send_success_flag = 1;

#ifdef DEBUG
	char longLogSuffix = 'A';
	if( remote.is_enable() )
	{
		if(long_log)
			delete long_log;
		long_log = new LongLog(longLogSuffix);
	}
#endif

   //-*********** syn game test ***********-//
   #ifdef DEBUG
   if(debug_seed_status_flag==DEBUG_SYN_LOAD_AND_COMPARE_ONCE)
      sp_load_seed_file();
   #endif
   //-*********** syn game test ***********-//

	VgaFrontLock vgaLock;

   // ------- establish_contact again --------//
   // if the game saved after NationRelation::contact_msg_flag set, but
   // remote players may have not receive MSG_NATION_CONTACT
   //
	// send MSG_NATION_CONTACT now

	if( !config.explore_whole_map && nation_array.player_recno &&
		!nation_array.is_deleted(nation_array.player_recno) )
	{
		for(short nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno )
		{
			if( nationRecno == nation_array.player_recno ||
				nation_array.is_deleted(nationRecno) )
				continue;

			NationRelation *relation = (~nation_array)->get_relation(nationRecno);
			if( relation->contact_msg_flag && !relation->has_contact)
         {
            // packet structure : <player nation> <explored nation>
            short *shortPtr = (short *)remote.new_send_queue_msg(MSG_NATION_CONTACT, 2*sizeof(short));
            *shortPtr = nation_array.player_recno;
            shortPtr[1] = nationRecno;
         }
      }
   }

	// ##### begin Gilbert 18/1 #######//
//	option_menu.active_flag = 0;
	option_menu.clear_active();
//	in_game_menu.active_flag = 0;
	in_game_menu.clear_active();
	// ##### end Gilbert 18/1 #######//

	DWORD lastDispFrameTime = m.get_time();
	// skipFrameMax : 0 for disable skip frame, // at most skip 1 frame
	int	skipFrameMax = remote.is_enable() ? 1 : 0;	
	int	skipFrameCount = skipFrameMax;	// so disp frame for the first frame
	unsigned long exceededDispTime = 0;

	DWORD firstUnreadyTime = 0;

	while( 1 )
	{
		if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage( &msg, NULL, 0, 0))
			{
				signal_exit_flag = 1;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ####### begin Gilbert 11/5 #######//
		else if ( !paused_flag && active_flag || use_true_front && remote.is_enable() )	// if switch tasked, still run in multiplay mode
		// ####### end Gilbert 11/5 #######//
		{
#ifdef DEBUG
			static unsigned long lastStartTime, lastEndTime, startTime;
			unsigned long processTime = 0;
			lastStartTime = startTime;
			startTime = m.get_time();
#endif
			int rc = 0;

			if( sys.signal_exit_flag )
				break;

			VgaFrontReLock vgaReLock;

			yield();       // could be improved, give back the control to Windows, so it can do some OS management. Maybe call WaitMessage() here and set up a timer to get messages regularly.

			// ####### begin Gilbert 11/5 #######//
			if ( !paused_flag && active_flag )
			{
				detect();
			}
			// ####### end Gilbert 11/5 #######//

			if( sys.signal_exit_flag )
				break;

			//--------------------------------//

			DWORD markTime = m.get_time();      // a time taken earlier than should_next_frame takes

			// ###### begin Gilbert 12/2 ######//
			if( remote.is_enable() && (testing_session || debug_session) )
			{
				String str;

				str  = "Player: ";
				str += nation_array.player_recno;
				str += "/";
				str += nation_array.size();

				str += " Send:";
				str += remote.packet_send_count;
				str += " Recv:";
				str += remote.packet_receive_count;
				str += " Frame:";
				str += frame_count;

				// font_san.disp is surprisingly slow
				font_san.disp( ZOOM_X1, 4, str, ZOOM_X1+300);
			}
			// ###### end Gilbert 12/2 ######//

			int unreadyPlayerFlag = 0;

			if( config.frame_speed>0 )              // 0-frozen
			{
				if( remote.is_enable() )      // && is_sync_frame )
				{
					remote.poll_msg();
					m.unlock_seed();
					rc = is_mp_sync(&unreadyPlayerFlag);         // if all players are synchronized
					m.lock_seed();
				}
				else
					rc = should_next_frame();

				if( rc )
				{
					LOG_BEGIN;
					m.unlock_seed();

#ifdef DEBUG
					if( remote.is_enable() )
					{
						long_log->printf("begin process frame %d\n", frame_count);
					}
#endif

#ifdef DEBUG
					processTime = m.get_time();
#endif
					process();
#ifdef DEBUG
					processTime = m.get_time() - processTime;
#endif

               if(remote.is_enable() )
                  m.lock_seed();    // such that random seed is unchanged outside sys::process()
					LOG_END;

               // -------- compare objects' crc --------- //
					if( remote.is_enable() && (remote.sync_test_level & 2) &&(frame_count % (remote.get_process_frame_delay()+3)) == 0)
               {
                  // cannot compare every frame, as PROCESS_FRAME_DELAY >= 1
                  crc_store.record_all();
						crc_store.send_all();
					}

				}
         }

			// ------- display graduately, keep on displaying --------- //
			// ####### begin Gilbert 11/5 #######//
			if ( !paused_flag && active_flag )
			{
				if( rc )
				{
					// ######## begin Gilbert 13/2 #######//
					// if the time after disp_frame exceeded the expected time limit (1000/config.frame_speed),
					// acculumate the exceeded time
					// if the acculumated time exceeded time of one frame (1000/config.frame_speed),
					// skip frame
					unsigned long targetFrameTime = config.frame_speed > 0 ? (1000/config.frame_speed) : 1000;

					if( skipFrameCount >= skipFrameMax
						|| exceededDispTime < targetFrameTime )
					{
						m.lock_seed();
						disp_frame();
						m.unlock_seed();

						unsigned long dispFrameTime = m.get_time();
						if( dispFrameTime - lastDispFrameTime >= targetFrameTime )
						{
							// excess time, accumulate into exceededDispTime
							exceededDispTime += dispFrameTime - lastDispFrameTime - targetFrameTime;
						}
						else
						{
							exceededDispTime = 0;
						}
						skipFrameCount = 0;
						lastDispFrameTime = dispFrameTime;
					}
					else
					{
						// skip frame
						skipFrameCount++;
						exceededDispTime = 0;
					}
					// ######## end Gilbert 13/2 #######//

					// reset firstUnreadyTime
					firstUnreadyTime = 0;
				}
				else
				{
					// set firstUnreadyTime, begin of a delay
					if( !firstUnreadyTime )
						firstUnreadyTime = m.get_time();

					if( config.frame_speed == 0 || markTime-lastDispFrameTime >= DWORD(1000/config.frame_speed) 
						|| zoom_need_redraw || map_need_redraw
						)
					{
						// on second condition, it should be happened when
						// in multiplayer, where should_next_frame passed
						// but is_mp_sync not passed
						disp_frame();
						skipFrameCount = 0;
						lastDispFrameTime = markTime;

						// display player not ready
						if( firstUnreadyTime && m.get_time() - firstUnreadyTime > 5000 )
						{
							int y = ZOOM_Y1 + 10;
							int x = ZOOM_X1 + 10;
							for( int nationRecno = 1; nationRecno <= MAX_NATION; ++nationRecno )
							{
								if( (unreadyPlayerFlag & (1 << (nationRecno-1))) && !nation_array.is_deleted(nationRecno) )
								{
									//int x2 = font_news.put( x, y, "Waiting for ");
									//x2 = font_news.put( x2, y, nation_array[nationRecno]->nation_name() );
									char colorCodeStr[] = " 0";
									colorCodeStr[1] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[nationRecno]->color_scheme_id;
									//x2 = font_news.put( x2, y, colorCodeStr );

									int x2 = font_news.put( x, y, text_game_menu.str_mp_wait_player( nation_array[nationRecno]->nation_name(), colorCodeStr) );
									blt_virtual_buf_area( x, y, x2, y+font_news.max_font_height );
									y += font_news.height() + 5;
								}
							}
						}

					}
				}
			}
			// ####### end Gilbert 11/5 #######//


         // ----------- detect sond is ended, play another -----------//

         if( config.frame_speed == 0 || day_frame_count == 0)
            music.yield();

#ifdef DEBUG
         if( rc && remote.is_enable() && day_frame_count == 0 )
         {
            if( long_log)
               delete long_log;

            if( ++longLogSuffix > 'Z' )
				{
               longLogSuffix = 'A';
            }
				long_log = new LongLog(longLogSuffix);
            long_log->printf("Game Date : %d/%d/%d\n", info.game_month, info.game_day, info.game_year);
         }
#endif

         if(rc)
			{
            //-*********** syn game test ***********-//
				//-------------------------------------------------------------//
				// record random seed for comparison
				//-------------------------------------------------------------//
				#ifdef DEBUG
					if(debug_seed_status_flag==DEBUG_SYN_LOAD_AND_COMPARE_ONCE ||
						debug_seed_status_flag==DEBUG_SYN_AUTO_LOAD)
						sp_compare_seed();
					else if(debug_seed_status_flag==DEBUG_SYN_AUTO_SAVE)
						sp_record_seed();
				#endif
				//-*********** syn game test ***********-//

            //------ auto save -------//

            auto_save();
         }

         //------ detect save game triggered by remote player ------//

         if( mp_save_flag && mp_save_frame == frame_count )
         {
            mp_clear_request_save();            // clear request first before save game

				if( nation_array.player_recno )     // only save then the player is still in the game
				{
					// game_file.save_game( player_profile.save_game_path(remote.save_file_name) );
					game_file.save_game( game_file_array.save_default_dir, remote.save_file_name, "");

					news_array.multi_save_game();
				}
			}

#ifdef DEBUG
			unsigned long interLoopTime = startTime - lastEndTime;	// we may also interested in this timing

			lastEndTime = m.get_time();
			unsigned long loopTime = lastEndTime - startTime;
			loopTime = 0;		// set break point here
#endif

			if( sys.signal_exit_flag )
				break;
		}
		else
		{
			WaitMessage();
		}
	}

	// ##### begin Gilbert 18/1 #######//
//	option_menu.active_flag = 0;
	option_menu.clear_active();
//	in_game_menu.active_flag = 0;
	in_game_menu.clear_active();
	// ##### end Gilbert 18/1 #######//

#ifdef DEBUG
	if(remote.is_enable())
	{
		if(long_log)
			delete long_log;
		long_log = NULL;
	}
#endif

	music.stop();
	remote.disable_process_queue();
	remote.disable_poll_msg();
	mp_clear_request_save();
}
//--------- End of function Sys::main_loop --------//


//-------- Begin of function Sys::pause --------//
//
void Sys::pause()
{
   if( paused_flag )
      return;

   InvalidateRect(vga.main_hwnd, NULL, TRUE);

   paused_flag = TRUE;
}
//--------- End of function Sys::pause ---------//


//-------- Begin of function Sys::unpause --------//
//
void Sys::unpause()
{
   if( !paused_flag )
      return;

   if( !restore() )
   {
      //-----------------------------------------------------//
		//  we are unable to restore, this can happen when
      //  the screen resolution or bitdepth has changed
		//  we just reload all the art again and re-create
      //  the front and back buffers.  this is a little
      //  overkill we could handle a screen res change by
      //  just recreating the front and back buffers we dont
      //  need to redo the art, but this is way easier.
      //-----------------------------------------------------//

      if (init_directx())
      {
         if( !restore() )     // if still not successful, quit
            return;
      }
   }

	mouse.update_skey_state();       // update ctrl/shift/alt key state after switch task

   //---- restore the saved screen before killing the focus ----//

   paused_flag = FALSE;
}
//--------- End of function Sys::unpause ---------//


//-------- Begin of function Sys::sleep --------//
//
void Sys::sleep(int time)
{
	int time2 = time>>2;
	for (int i = time2; i > 0; i--)
	{
		Sleep(4);
		yield();
	}

	Sleep(time-time2);
	yield();
}
//-------- End of function Sys::sleep --------//

//-------- Begin of function Sys::restore --------//
//
int Sys::restore()
{
   if( !vga_front.restore_buf() )
      return 0;

   if( !vga_back.restore_buf() )
      return 0;

//   if( sys.debug_session )
   if( sys.use_true_front )
   {
      if( !vga_true_front.restore_buf() )
         return 0;
   }

   return 1;
}
//--------- End of function Sys::restore ---------//


//-------- Begin of function Sys::yield --------//
//
void Sys::yield()
{
   static int isYielding=0;

   if( isYielding )
      return;

   isYielding=1;

	// ###### begin Gilbert 11/2 #######//
#ifdef DEBUG
	const unsigned short remoteTimeGroup = 32;
	static unsigned long remoteTimeFreq[remoteTimeGroup] = 
	{ 
		0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,
	};
	unsigned long remoteTime;
#endif
	// ###### end Gilbert 11/2 #######//

#ifdef ENABLE_WINE_HACKS
   GetInputState();
#endif
   mouse.poll_event();

   audio.yield();

   if( remote.is_enable() )
   {
#ifdef DEBUG
		remoteTime = m.get_time();
#endif

		remote.poll_msg();

      remote.process_specific_msg(MSG_SET_SPEED);        // need to test it here for restoring the speed from frozen to normal
		// ####### begin Gilbert 28/6 #######//
      remote.process_specific_msg(MSG_CHAT);
		// ####### end Gilbert 28/6 #######//

      if( config.frame_speed > 0 )
      {
         remote.process_specific_msg(MSG_TELL_SEND_TIME);
         remote.process_specific_msg(MSG_REQUEST_RESEND);
      }

      //-------- display debug info -----------//
		// moved to main_loop
//		if( power.enable_flag && (testing_session || debug_session) )
//		{
//			String str;
//
//			str  = "Player: ";
//			str += nation_array.player_recno;
//			str += "/";
//			str += nation_array.size();
//
//			str += " Send:";
//			str += remote.packet_send_count;
//			str += " Recv:";
//			str += remote.packet_receive_count;
//			str += " Frame:";
//			str += frame_count;
//
//			// font_san.disp is surprisingly slow
//			font_san.disp( ZOOM_X1, 4, str, ZOOM_X1+300);
//		}

#ifdef DEBUG
		remoteTime = m.get_time() - remoteTime;

		if( power.enable_flag )
		{
			if( remoteTime >= remoteTimeGroup )
				remoteTimeFreq[remoteTimeGroup-1]++;
			else
				remoteTimeFreq[remoteTime]++;
		}
#endif
   }

   isYielding=0;
}
//--------- End of function Sys::yield ---------//


//-------- Begin of function Sys::is_mp_sync --------//
//
// Multiplayer synchronization.
//
// Check all players are ready to proceed to the next frame.
//
int Sys::is_mp_sync( int *unreadyPlayerFlag)
{
   #define RESEND_TIME_OUT            2000    // if the other machines still aren't ready after 2 seconds, send the notification again
   #define RESEND_AGAIN_TIME_OUT      1000    // keep resending if no responses
   #define CONNECTION_LOST_TIME_OUT  20000    // ask for connection lost handling aftering waiting for 5 seconds.

   //---- if we haven't been ready for the next frame yet ----//

#ifdef DEBUG
   int n;
   DEBUG_LOG("begin nation's next_frame_ready");
   for (n = 1; n <= nation_array.size(); ++n)
   {
      DEBUG_LOG(nation_array[n]->next_frame_ready);
   }
   DEBUG_LOG("end nation's next_frame_ready");
#endif

	if( unreadyPlayerFlag )
		*unreadyPlayerFlag = 0;

   // if last remote.send was fail, attempt to send it again
   if( !nation_array.player_recno )
   {
      // observation mode
      if( !should_next_frame() )
         return 0;
      remote_send_success_flag = 1;
   }
   else if( remote_send_success_flag 
		&& remote.has_send_frame(nation_array.player_recno, frame_count)
		&& (~nation_array)->next_frame_ready==0 )
	{
      DEBUG_LOG("Local player not ready");
      if( !should_next_frame() )    // not ready to proceed yet
         return 0;

      //------------ queue MSG_NEXT_FRAME ----------//

      short* shortPtr = (short*) remote.new_send_queue_msg(MSG_NEXT_FRAME, sizeof(short));

      shortPtr[0] = nation_array.player_recno;  // short_para1 is the nation recno of the current player

      //------------ queue MSG_QUEUE_TRAILER ----------//

      shortPtr = (short*) remote.new_send_queue_msg(MSG_QUEUE_TRAILER, sizeof(short));
      shortPtr[0] = nation_array.player_recno;  // short_para1 is the nation recno of the current player

      //------ copy all queued action to our own receive buffer to merge with other player's action ----//

      remote.append_send_to_receive();

      //--- copy the whole queue to a buffer in case of resend request from other players ---//

      remote.copy_send_to_backup();

		//---------- send out all messages in the queue ---------//

      remote_send_success_flag = remote.send_queue_now();               // if not sent successfully, try again next time

      if( remote_send_success_flag )      // still failed, try again next time
      {
         DEBUG_LOG("first send sucess" );
         remote.init_send_queue(frame_count+1, nation_array.player_recno);    // frame_count, initialize for next frame's send queue
         // sent random seed
         char *p = (char *)remote.new_send_queue_msg(MSG_TELL_RANDOM_SEED, sizeof(short)+sizeof(long));
         *(short *)p = nation_array.player_recno;
         p += sizeof(short);
         *(long *)p = m.get_random_seed();
      }
      else
      {
         // re_transmit as quickly as possible
         ec_remote.re_transmit(5);
      }
   }
   else
	{
      DEBUG_LOG("Local player nation ready");
   }

   //----- if previous sending was not successful, send again now -----//

   if( !remote_send_success_flag )
   {
      remote_send_success_flag = remote.send_queue_now();               // if not sent successfully, try again next time

      if( remote_send_success_flag )      // still failed, try again next time
      {
         DEBUG_LOG("resending ok");
         remote.init_send_queue(frame_count+1, nation_array.player_recno);    // frame_count, initialize for next frame's send queue
         // sent random seed
         char *p = (char *)remote.new_send_queue_msg(MSG_TELL_RANDOM_SEED, sizeof(short)+sizeof(long));
         *(short *)p = nation_array.player_recno;
         p += sizeof(short);
         *(long *)p = m.get_random_seed();
      }
      else
      {
         // re_transmit as quickly as possible
         ec_remote.re_transmit(5);
         DEBUG_LOG("resending not ok");
         return 0;
      }
   }

   //------ pre_process MSG_NEXT_FRAME in the queue -----//

   remote.process_specific_msg(MSG_NEXT_FRAME);

#ifdef DEBUG
   DEBUG_LOG("begin nation's next_frame_ready");
   for (n = 1; n <= nation_array.size(); ++n)
   {
      DEBUG_LOG(nation_array[n]->next_frame_ready);
   }
   DEBUG_LOG("end nation's next_frame_ready");
#endif

   //------ check if all remote players are ready to proceed -----//

   int     nationRecno;
	Nation* nationPtr;

   for( nationRecno=nation_array.size() ; nationRecno>0 ; nationRecno-- )
   {
      if( nation_array.is_deleted(nationRecno) )
         continue;

      nationPtr = nation_array[nationRecno];

      //------- if some remote machines are not ready yet -------//

      if( nationPtr->is_remote() &&
			(remote.has_send_frame(nationRecno, frame_count) && !nationPtr->next_frame_ready) )
		{
			if( unreadyPlayerFlag )
				*unreadyPlayerFlag |= ( 1 << (nationRecno-1) );
         break;
		}
   }

   //------- if some remote machines are not ready yet -------//

   if( nationRecno>0 )
   {
      DEBUG_LOG("a nation not ready");
		DEBUG_LOG(nationRecno);

		if( m.get_time() >= last_frame_time+RESEND_TIME_OUT )
		{
			//---- if it has been time out for too long, carry out connection lost handling ---//

			if( // m.get_time() >= last_frame_time+CONNECTION_LOST_TIME_OUT ||
				!ec_remote.is_player_valid(nationRecno))
         {
            DEBUG_LOG( "Connection Lost" );
				news_array.multi_connection_lost(nationRecno);
				// may allow save game here, ask user whether to save the game
				nationPtr->nation_type = NATION_AI;    // let computer take over the nation
				nation_array.ai_nation_count++;
			}
		}

      return 0;
   }

   //--------------------------------------------------------//
   //
   // When all players are ready to proceed to the next frame
   //
	// As we have already know all players are ready, we can
	// reset the next_frame_ready flag for all nations.
   //
   //--------------------------------------------------------//

   DEBUG_LOG("all nation ready");
   for( int i=nation_array.size() ; i>0 ; i-- )
   {
      if( nation_array.is_deleted(i) )
         continue;

      nation_array[i]->next_frame_ready=0;      // -- instead of set to 0, set it may be 2 if it has just received an notifying signal for the further next frame from a player as it also sent out a next frame ready msg to all other players
   }

   //--------- process msgs in the receive queue ----------//

   remote.process_receive_queue();

#ifdef DEBUG
   DEBUG_LOG("begin nation's next_frame_ready");
   for (n = 1; n <= nation_array.size(); ++n)
   {
      DEBUG_LOG(nation_array[n]->next_frame_ready);
   }
   DEBUG_LOG("end nation's next_frame_ready");
#endif

   //-------- record this frame's time -------//

   last_frame_time  = m.get_time();
   last_resend_time = 0;

   return 1;
}
//---------- End of function Sys::is_mp_sync --------//


//-------- Begin of function Sys::should_next_frame --------//
//
// Check if it's now the time for processing the next frame.
//
int Sys::should_next_frame()
{
   //----- special modes: 0-frozen, 9-fastest possible -----//

	if( config.frame_speed==99 )
      return 1;

   if( config.frame_speed==0 )
      return 0;

   //---- check if it's now the time for processing the next frame ----//

   DWORD curTime = m.get_time();

   if( next_frame_time )      // if next_frame_time==0, it's the first frame of the game
   {
      if( next_frame_time < 1000 )  // the DWORD variable has been overflow
      {
         if( curTime < next_frame_time || curTime >= 1000 )    // >= 1000 if the curTime has been overflow yet, wait for it to overflow so we can compare it when next_frame_time
            return 0;
      }
      else     // normal non-overflow case
      {
         if( curTime < next_frame_time )
            return 0;
      }
   }

   //--- Time between frames = 1000 milliseconds / frames per second ---//

   next_frame_time = curTime + 1000 / config.frame_speed;

   return 1;
}
//--------- End of function Sys::should_next_frame ---------//


//-------- Begin of function Sys::process --------//
//
void Sys::process()
{
	//---- if any of the DirectDraw buffers is lost, restore it ----//

	if( vga_front.is_buf_lost() || vga_back.is_buf_lost() )
	{
		if (!restore())
			pause();
	}

	//------- update frame count and is_sync_frame --------//

	frame_count++;
	is_sync_frame = frame_count%3==0;	// check if sychronization should take place at this frame (for handling one sync per n frames)

	//------- play sound effect ------//

	se_ctrl.flush();

	//--------- process objects -----------//

	base_obj_array.debug_validate();

	way_point_array.process();

	LOG_MSG(m.get_random_seed());
	LOG_MSG("begin unit_array.process()");
	unit_array.process();
	LOG_MSG("end unit_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin firm_array.process()");
	firm_array.process();
	LOG_MSG("end firm_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin town_array.process()");
	town_array.process();
	LOG_MSG("end town_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin nation_array.process()");
	nation_array.process();
	LOG_MSG("end nation_array.process()");
	LOG_MSG(m.get_random_seed());

	if( sys.quick_exit_flag() )
		return;

	LOG_MSG("begin bullet_array.process()");
	bullet_array.process();
	LOG_MSG("end bullet_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin world.process()");
	world.process();
	LOG_MSG("end world.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin tornado_array.process()");
	tornado_array.process();
	LOG_MSG("begin tornado_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin snow_ground_array.process()");
	snow_ground_array.process();
	LOG_MSG("end snow_ground_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin rock_array.process()");
	rock_array.process();
	LOG_MSG("end rock_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin dirt_array.process()");
	dirt_array.process();
	LOG_MSG("end dirt_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin effect_array.process()");
	effect_array.process();
	LOG_MSG("end effect_array.process()");
	LOG_MSG(m.get_random_seed());

	LOG_MSG("begin war_point_array.process()");
	war_point_array.process();
	LOG_MSG("end war_point_array.process()");

	LOG_MSG("begin firm_die.process()");
	firm_die_array.process();
	LOG_MSG("end firm_die.process()");
	
	//------ check if it's time for the next day ------//

	if( ++day_frame_count >= FRAMES_PER_DAY )
	{
		LOG_MSG("begin info.next_day()");
		info.next_day();
		LOG_MSG("end info.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin world.next_day()");
		world.next_day();
		LOG_MSG("end world.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin site_array.next_day()");
		site_array.next_day();
		LOG_MSG("end site_array.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin rebel_array.next_day()");
		rebel_array.next_day();
		LOG_MSG("end rebel_array.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin spy_array.next_day()");
		spy_array.next_day();
		LOG_MSG("end spy_array.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin sprite_res.update_speed()");
		if( config.weather_effect)
			sprite_res.update_speed();
		LOG_MSG("end sprite_res.update_speed()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin raw_res.next_day()");
		raw_res.next_day();
		LOG_MSG("end raw_res.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin talk_res.next_day()");
		talk_res.next_day();
		LOG_MSG("end talk_res.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin hero_res.next_day()");
		hero_res.next_day();
		LOG_MSG("end hero_res.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin region_array.next_day()");
		region_array.next_day();
		LOG_MSG("end region_array.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin random_event.next_day()");
		random_event.next_day();
		LOG_MSG("end random_event.next_day()");
		LOG_MSG(m.get_random_seed());

		LOG_MSG("begin campaign.next_day()");
		if( game.is_campaign_mode() )
			game.campaign()->next_day();
		LOG_MSG("end campaign.next_day()");
		LOG_MSG(m.get_random_seed());

		day_frame_count = 0;
	}

	// moved to sys.main_loop
//#ifdef DEBUG
//	unsigned long dispFrameTime = m.get_time();
//#endif
	//------ display the current frame ------//
//	LOG_MSG("begin sys.disp_frame");
//	m.lock_seed();
//	disp_frame();
//	m.unlock_seed();
//	LOG_MSG("end sys.disp_frame");
//	LOG_MSG(m.get_random_seed() );
//#ifdef DEBUG
//	dispFrameTime = m.get_time() - dispFrameTime;
//#endif
}
//--------- End of function Sys::process ---------//


//-------- Begin of function Sys::set_speed --------//
//
void Sys::set_speed(int frameSpeed, int remoteCall)
{
   //--------- if multiplayer, update remote players setting -------//

   if( remote.is_enable() && !remoteCall )
   {
      RemoteMsg *remoteMsg = remote.new_msg(MSG_SET_SPEED, sizeof(short));

		*((short*)remoteMsg->data_buf) = frameSpeed;

      remote.send_free_msg( remoteMsg );     // send out the message and free it after finishing sending
   }

   //---------- set the speed now ----------//

   if( config.frame_speed==0 )                   // if it's currently frozen, set last_frame_time to avoid incorrect timeout
      last_frame_time = m.get_time();

   config.frame_speed = frameSpeed;
}
//--------- End of function Sys::set_speed ---------//

