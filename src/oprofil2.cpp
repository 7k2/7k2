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

// Filename    : OPROFIL2.H
// Description : PlayerProfile menu

#include <oprofile.h>
#include <all.h>
#include <odir.h>
#include <oblob.h>
#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <ovgabuf.h>
#include <ovgalock.h>
#include <omouse.h>
#include <omousecr.h>
#include <ofont.h>
#include <ogeta.h>
#include <key.h>
#include <oslidcus.h>
#include <obutton.h>
#include <obutt3d.h>
#include <oimgres.h>
#include <obox.h>
#include <ogameset.h>
#include <ounitres.h>
#include <oraceres.h>
#include <ot_gmenu.h>

#include <stdio.h>


#define PPOPTION_PAGE          0x40000000
#define PPOPTION_ENUM_PROFILE  0x00000001
#define PPOPTION_SCROLL_BAR    0x00000002
#define PPOPTION_GET_FIELDS    0x00000008
#define PPOPTION_NAME_FIELD    0x00000010
#define PPOPTION_DESC_FILED    0x00000020
#define PPOPTION_FILE_FIELD    0x00000040
#define PPOPTION_DIR_FIELD     0x00000080
#define PPOPTION_ALL           0x7fffffff

#define BROWSE_X1             131
#define BROWSE_Y1             155
#define BROWSE_X2             665
#define BROWSE_Y2             352
#define SCROLL_BAR_X1         (BROWSE_X1+14)
#define SCROLL_BAR_Y1         369
#define SCROLL_BAR_X2         (BROWSE_X2-14)
#define SCROLL_BAR_Y2         385
#define SCROLL_LINE_HEIGHT		33
#define SCROLL_LINE_COUNT		((BROWSE_Y2-BROWSE_Y1+1)/SCROLL_LINE_HEIGHT)
#define SCROLL_MAX_COLUMN		2

#define NAME_FIELD_X          (BROWSE_X1 + 5)
#define DESC_FIELD_X          (BROWSE_X1 + 155)
#define FILE_FIELD_X          (BROWSE_X1 + 490)
#define DIR_FIELD_X           (BROWSE_X1 + 590)

#define BUTTON1_X1	46
#define BUTTON1_Y1	526
#define BUTTON1_X2	185
#define BUTTON1_Y2	556
#define BUTTON2_X1	188
#define BUTTON2_Y1	BUTTON1_Y1
#define BUTTON2_X2   328
#define BUTTON2_Y2	BUTTON1_Y2
#define BUTTON3_X1	330
#define BUTTON3_Y1	BUTTON1_Y1 
#define BUTTON3_X2	470
#define BUTTON3_Y2	BUTTON1_Y2
#define BUTTON4_X1	473
#define BUTTON4_Y1	BUTTON1_Y1
#define BUTTON4_X2	612
#define BUTTON4_Y2	BUTTON1_Y2
#define BUTTON5_X1	614
#define BUTTON5_Y1	BUTTON1_Y1
#define BUTTON5_X2	754
#define BUTTON5_Y2	BUTTON1_Y2

#define BUTTON6_X1	42
#define BUTTON6_Y1	566
#define BUTTON6_X2	183
#define BUTTON6_Y2	599
#define BUTTON7_X1	185
#define BUTTON7_Y1	BUTTON6_Y1
#define BUTTON7_X2	327
#define BUTTON7_Y2	BUTTON6_Y2
#define BUTTON8_X1	329
#define BUTTON8_Y1	BUTTON6_Y1
#define BUTTON8_X2	471
#define BUTTON8_Y2	BUTTON6_Y2
#define BUTTON9_X1	473
#define BUTTON9_Y1	BUTTON6_Y1
#define BUTTON9_X2	615
#define BUTTON9_Y2	BUTTON6_Y2


enum
{
	PROFILE_MENU_MAIN,
	PROFILE_MENU_NEW,
	PROFILE_MENU_DEL,
};

static void disp_scroll_bar_func(SlideBar *scroll, int);

// ----- temp class to destruct game_set, unit_res and race_res -------//

class TempGameSet
{
private:
	char inited_flag;

public:
	TempGameSet(int s)
	{
		if( !(inited_flag = game_set.set_opened_flag) )
		{
			game_set.open_set(1);
		}
	}

	~TempGameSet()
	{
		if( !inited_flag )
			game_set.close_set();
	}
};

class TempUnitRes
{
private:
	char inited_flag;

public:
	TempUnitRes()
	{
		if( !(inited_flag = unit_res.init_flag) )
			unit_res.init();
	}

	~TempUnitRes()
	{
		if( !inited_flag )
			unit_res.deinit();
	}
};


class TempRaceRes
{
private:
	char inited_flag;

public:
	TempRaceRes()
	{
		if( !(inited_flag = race_res.init_flag) )
			race_res.init();
	}

	~TempRaceRes()
	{
		if( !inited_flag )
			race_res.deinit();
	}
};


// return 1=new profile, 2=profile loaded

int PlayerProfile::register_menu()
{
	long refreshFlag = PPOPTION_ALL;

	// ------- set mouse cursor, useful when auto-launch ------//

	mouse_cursor.set_icon(CURSOR_NORMAL);

	// -------- init unit_res and race_res -------//

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init

	// ------- initial this player_profile -------//

	if( is_registered() )		// try to load it again
	{
		String str;
		str = file_name;
		str += ".PRF";

		File f;
		if( !m.is_file_exist(str) || !f.file_open(str) || !reload() )
		{
			init();
		}
	}
	else
	{
		init();
	}

	// ------- directory of PRF file ------//

	Blob profileBlock;
	PlayerProfile *profileArray = NULL;
	int profileCount = 0;
	int selectedProfile = 0;
	int profileColumnCount = 0;

	// ------ init font --------//

	Font &fontThin = font_thin_black;
	Font &fontBlack = font_bold_black;
	Font &fontRed = font_bold_red;

	// ------- init scroll bar ------//

	SlideBar scrollBar;
	scrollBar.init_scroll( SCROLL_BAR_X1, SCROLL_BAR_Y1, 
		SCROLL_BAR_X2, SCROLL_BAR_Y2, SCROLL_MAX_COLUMN, disp_scroll_bar_func );

	// ------ input name field -------//

	char keyinPlayerName[PLAYER_NAME_LEN+1] = "";
	//GetAGroup getGroup(4);
	GetAGroup getGroup(1);
	GetA &playerNameField = getGroup[0];
	playerNameField.init( 200, 300, 600, keyinPlayerName, PLAYER_NAME_LEN, &fontBlack, 1, 1 );

//	GetA &descField = getGroup[1];
//	descField.init( 200, 440, 790, player_desc, PLAYER_DESC_LEN, &font_san, 0, 1 );
//	GetA &fileNameField = getGroup[2];
//	fileNameField.init( 200, 460, 400, file_name, SAVE_GAME_DIR_LEN, &font_san, 0, 1 );
//	GetA &dirNameField = getGroup[3];
//	dirNameField.init( 200, 480, 400, save_dir, SAVE_GAME_DIR_LEN, &font_san, 0, 1 );

	getGroup.set_focus(0, 0 );

	// -------- button -------- //

//	Button3D scrollUp, scrollDown;
//	scrollUp.create(SCROLL_BAR_X1, BROWSE_Y1, "SV-UP-U", "SV-UP-D");
//	scrollDown.create(SCROLL_BAR_X1, SCROLL_BAR_Y2+1, "SV-DW-U", "SV-DW-D");
	Button scrollLeft, scrollRight;
	scrollLeft.create_text(BROWSE_X1, SCROLL_BAR_Y1, SCROLL_BAR_X1-1, SCROLL_BAR_Y2, " ");
	scrollRight.create_text(SCROLL_BAR_X2+1, SCROLL_BAR_Y1, BROWSE_X2, SCROLL_BAR_Y2, " ");

//	Button startButton, loadButton, cancelButton;
//	startButton.create_text( 100, 530, 200, 555, "New" );
//	loadButton.create_text( 300, 530, 400, 555, "Load" );
//	cancelButton.create_text( 500, 530, 600, 555, "Cancel" );

	int menuMode = PROFILE_MENU_MAIN;

	VgaFrontLock vgaLock;

	while(1)
	{
		MSG msg;
		if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage( &msg, NULL, 0, 0))
			{
				sys.signal_exit_flag = 1;
				// BUGHERE : vga_front is unlocked
				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		else if( sys.paused_flag || !sys.active_flag )
		{
			WaitMessage();
			continue;
		}
		if( sys.need_redraw_flag )
		{
			refreshFlag = PPOPTION_ALL;
			sys.need_redraw_flag = 0;
		}

		VgaFrontReLock vgaReLock;

		sys.yield();
		mouse.get_event();

		// -------- display ----------//
		if( refreshFlag )
		{
			if( refreshFlag & PPOPTION_ENUM_PROFILE )
			{
				profileCount = 0;
				Directory profileDir;
				profileDir.read( "*.PRF", 1 );		// sort file name
				profileBlock.resize( sizeof(PlayerProfile) * profileDir.size() );
				profileArray = (PlayerProfile *) profileBlock.p();
				int i;
				for( i = 1; i <= profileDir.size(); ++i )
				{
					if( profileArray[profileCount].load(profileDir[i]->name) )
					{
						++profileCount;
					}
				}

				// ------ calculate column ------//

				profileColumnCount = (profileCount + SCROLL_LINE_COUNT-1) / SCROLL_LINE_COUNT;

				// ------ set slide bar -------//

				scrollBar.set( 0, profileColumnCount-1, 0 );

				// ------- find the latest profile ---------//

				if( selectedProfile == 0 && profileDir.size() > 0 )
				{
					int latestFile = 1;
					FILETIME latestTime = profileDir[1]->time;
					for( i = 2; i <= profileDir.size(); ++i )
					{
						if( profileDir[i]->time.dwHighDateTime > latestTime.dwHighDateTime
							|| profileDir[i]->time.dwHighDateTime == latestTime.dwHighDateTime
							&& (profileDir[i]->time.dwLowDateTime > latestTime.dwLowDateTime))
						{
							latestTime = profileDir[i]->time;
							latestFile = i;
						}
					}
					selectedProfile = latestFile;		// default to the latest file
					scrollBar.set_view_recno( (selectedProfile-1)/SCROLL_LINE_COUNT );
				}

				// ------ go to new profile directly ------//

				if( menuMode == PROFILE_MENU_MAIN && profileCount == 0 )
					menuMode = PROFILE_MENU_NEW; 
			}


			switch( menuMode )
			{
			case PROFILE_MENU_MAIN:
				if( refreshFlag & PPOPTION_PAGE )
				{
					vga.use_back();
					vga.disp_image_file("CHOOSE");
					fontBlack.center_put( 119, 95, 678, 118, text_game_menu.str_profile_s1() );
					// fontBlack.center_put( 113, 392, 663, 414, "If your name is not here, click on \"New Player\" below");
					fontBlack.center_put_paragraph( 113, 392, 688, 414, text_game_menu.str_profile_s2() );
					fontThin.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2, text_game_menu.str_finish_select_profile() );	// continue button
					fontThin.center_put_paragraph( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_new_profile() );	// new button
					fontThin.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_delete_profile() );		// delete button
					fontThin.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2, text_game_menu.str_cancel() );		// cancel button
					vga.use_front();
					vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				}

				if( refreshFlag & PPOPTION_SCROLL_BAR )
				{
					int columns = MIN( profileColumnCount, SCROLL_MAX_COLUMN );
					if( columns > 0 )
					{
						int columnWidth = (BROWSE_X2 - BROWSE_X1 + 1) / columns;	// column width
						int x1 = BROWSE_X1;

						for( int c = 0; c < columns; ++c, (x1+=columnWidth) )
						{
							int y = BROWSE_Y1;
							for( int i = 0; i < SCROLL_LINE_COUNT; ++i, y+= SCROLL_LINE_HEIGHT )
							{
								int rec = (scrollBar.view_recno + c) * SCROLL_LINE_COUNT + i + 1;

								// copy blank from back buffer
								vga.blt_buf( x1, y, x1+columnWidth-1, y+SCROLL_LINE_HEIGHT-1, 0 );

								if( rec > 0 && rec <= profileCount )
								{
									// choose font
									Font *fontPtr;

									if( rec == selectedProfile )
										fontPtr = &fontRed;
									else
										fontPtr = &fontBlack;

									// put profile player name
									fontPtr->center_put( x1, y, x1+columnWidth-1, y+SCROLL_LINE_HEIGHT-1, profileArray[rec-1].player_name );
								}
							}
						}
					}

					if( profileColumnCount > SCROLL_MAX_COLUMN )
					{
						scrollBar.paint();		// scroll bar and button
						scrollLeft.paint();
						scrollRight.paint();
					}
					else
					{
						vga.blt_buf( BROWSE_X1, SCROLL_BAR_Y1, BROWSE_X2, SCROLL_BAR_Y2, 0 );
					}
				}
				break;

			case PROFILE_MENU_NEW:		// ask new player
				if( refreshFlag & PPOPTION_PAGE )
				{
					vga.use_back();
					vga.disp_image_file("CHOOSE");
					fontBlack.center_put( 136, 187, 672, 217, text_game_menu.str_profile_new() );
					fontThin.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_finish_select_profile() );
					fontThin.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_cancel() );
					vga.use_front();
					vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				}

				if( refreshFlag & PPOPTION_GET_FIELDS )
				{
					getGroup.paint();
				}
				else
				{
					if( refreshFlag & PPOPTION_NAME_FIELD )
					{
						playerNameField.paint(0);
					}
					if( refreshFlag & PPOPTION_DESC_FILED )
					{
//						descField.paint(0);
					}
					if( refreshFlag & PPOPTION_FILE_FIELD )
					{
//						fileNameField.paint(0);
					}
					if( refreshFlag & PPOPTION_DIR_FIELD )
					{
//						dirNameField.paint(0);
					}
				}
				break;
			case PROFILE_MENU_DEL:
				if( refreshFlag & PPOPTION_PAGE )
				{
					vga.use_back();
					vga.disp_image_file("CHOOSE");
					fontBlack.center_put_paragraph( 136, 187, 672, 287, text_game_menu.str_profile_del() );
					if( profileArray[selectedProfile-1].login_name[0] )
					{
						fontBlack.center_put_paragraph( 136, 287, 672, 387, text_game_menu.str_profile_lost_login_info() );
					}
					fontThin.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_profile_del_yes() );
					fontThin.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_profile_del_no() );
					vga.use_front();
					vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				}
				break;
			default:
				err_here();
			}
			refreshFlag = 0;
		}

		sys.blt_virtual_buf();

		// ------- detect ---------//

		switch( menuMode )
		{
		case PROFILE_MENU_MAIN:
			{		
				// ------ detect player name list ------//

				int columns = MIN( profileColumnCount, SCROLL_MAX_COLUMN );
				if( columns > 0 )
				{
					int columnWidth = (BROWSE_X2 - BROWSE_X1 + 1) / columns;	// column width
					int x1 = BROWSE_X1;

					for( int c = 0; c < columns; ++c, (x1+=columnWidth) )
					{
						int y = BROWSE_Y1;
						for( int i = 0; i < SCROLL_LINE_COUNT; ++i, y+= SCROLL_LINE_HEIGHT )
						{
							int rec = (scrollBar.view_recno + c) * SCROLL_LINE_COUNT + i + 1;
							int clickCount;
							if( rec > 0 && rec <= profileCount && 
								(clickCount = mouse.any_click(x1, y, x1+columnWidth, y+SCROLL_LINE_HEIGHT-1)) )
							{
								if( selectedProfile != rec )
								{
									selectedProfile = rec;
									refreshFlag |= PPOPTION_SCROLL_BAR;
								}
								if( clickCount >= 2 )
								{
									*this = profileArray[selectedProfile-1];
									if( save() && re_create_directory() )	// call save, the file is latest for next time
										return 2;
								}
							}
						}
					}
				}

				// ------ detect scroll bar and scroll button ------//

				if( profileColumnCount > SCROLL_MAX_COLUMN )
				{

					if( scrollBar.detect() == 1 )
					{
						refreshFlag |= PPOPTION_SCROLL_BAR;
					}

					if( scrollLeft.detect() )
					{
						int oldViewRecno = scrollBar.view_recno;
						if( scrollBar.set_view_recno(scrollBar.view_recno -1) != oldViewRecno )
						{
							refreshFlag |= PPOPTION_SCROLL_BAR;
						}
					}

					if( scrollRight.detect() )
					{
						int oldViewRecno = scrollBar.view_recno;
						if( scrollBar.set_view_recno(scrollBar.view_recno +1) != oldViewRecno )
						{
							refreshFlag |= PPOPTION_SCROLL_BAR;
						}
					}
				}

				// ------- continue button --------//

				if( selectedProfile > 0 && selectedProfile <= profileCount 
					&& mouse.single_click(BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2) )
				{
					*this = profileArray[selectedProfile-1];
					if( save() && re_create_directory() )	// call save, the file is latest for next time
						return 2;
				}

				// ------- new button -------//

				if( mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
				{
					menuMode = PROFILE_MENU_NEW;
					refreshFlag = PPOPTION_ALL;
				}

				// ------ delete -------//

				if( selectedProfile > 0 && selectedProfile <= profileCount 
					&& mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
				{
					menuMode = PROFILE_MENU_DEL;
					refreshFlag = PPOPTION_ALL;
				}

				// ------ cancel --------//

				if( mouse.any_click(RIGHT_BUTTON)
					|| mouse.single_click(BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2) )
				{
					return 0;
				}
			}
			break;
		case PROFILE_MENU_NEW:

			// ------- detect geta fields group -------//
			{
				char oldPlayerName[PLAYER_NAME_LEN+1];
				strcpy( oldPlayerName, keyinPlayerName );
				getGroup.detect();

				if( strcmp(oldPlayerName, keyinPlayerName) != 0 )		// if player name changed, update file name field
				{
					// default_save_name( file_name, keyinPlayerName );
					// default_save_name( save_dir, keyinPlayerName );
//					fileNameField.text_changed();
//					dirNameField.text_changed();
					refreshFlag |= PPOPTION_FILE_FIELD | PPOPTION_DIR_FIELD;
				}
			}

			// ------- continue button --------//

			if( keyinPlayerName[0] && keyinPlayerName[0] != ' '
				&& (mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2)
				|| mouse.is_key_event() && mouse.key_code == KEY_RETURN) )
			{
				// checking same profile exist

				int i;
				for( i = 0; i < profileCount; ++i )
				{
					if( stricmp( profileArray[i].player_name, keyinPlayerName ) == 0 )
					{
						// box.msg( "Same profile has already been created" );
						box.msg( text_game_menu.str_profile_error_dup() );
						break;
					}
				}

				if( i >= profileCount )		// no same profile name exist
				{
					init();
					strcpy( player_name, keyinPlayerName );
					default_save_name( file_name, keyinPlayerName );
					default_save_name( save_dir, keyinPlayerName );

					// ------ create dir_save first ------//

					String str;
					str = DIR_SAVE;
					str = str.substr(0, str.len()-1);
					// remove backslash at the end
					if( !m.is_file_exist(str) )
					{
						if( !CreateDirectory( str, NULL ) )
						{
							// box.msg( "Error creating SAVE directory" );
							box.msg( text_game_menu.str_profile_error_dir(str) );
							return 0;
						}
					}

					// ----- set file_name ------ //

					str = file_name;
					str += ".PRF";
					if( m.is_file_exist(str) )
					{
						if( strlen(file_name) >= 4 )
							file_name[4] = '\0';		// cut to 4 char

						// pad trail underscore and "001"
						strcat( file_name, "_____001"+strlen(file_name) );
						str = file_name,
						str += ".PRF";
						int tryCount = 1;
						while( m.is_file_exist(str) )
						{
							++tryCount;
							if( tryCount >= 1000 )
							{
								// box.msg( "Error creating profile" );
								box.msg( text_game_menu.str_profile_error_create() );
								return 0;
							}
							file_name[7] = tryCount % 10 + '0';		// unit digit
							file_name[6] = (tryCount / 10) % 10 + '0';		// ten digit
							file_name[5] = (tryCount / 100) % 10 + '0';		// unit digit
							str = file_name;
							str += ".PRF";
						}
					}

					// ------ set directory -------//

					str = DIR_SAVE;
					str += save_dir;
					if( m.is_file_exist(str) )
					{
						if( strlen(save_dir) >= 4 )
							save_dir[4] = '\0';		// cut to 4 char

						// pad trail underscore and "001"
						strcat( save_dir, "_____001"+strlen(save_dir) );
						str = DIR_SAVE;
						str += save_dir;
						int tryCount = 1;
						while( m.is_file_exist(str) )
						{
							++tryCount;
							if( tryCount >= 1000 )
							{
								// box.msg( "Error creating profile" );
								box.msg( text_game_menu.str_profile_error_create() );
								return 0;
							}
							save_dir[7] = tryCount % 10 + '0';		// unit digit
							save_dir[6] = (tryCount / 10) % 10 + '0';		// ten digit
							save_dir[5] = (tryCount / 100) % 10 + '0';		// hundred digit
							str = DIR_SAVE;
							str += save_dir;
						}
					}

					// ------ create directory -------//

					BOOL dirCreated = CreateDirectory( str, NULL );
					if( !save() )
					{
						if( dirCreated )
							remove( str);	// clear the directory created
						// box.msg( "Error creating profile" );
						box.msg( text_game_menu.str_profile_error_create() );
					}
					else
					{
						return 1;
					}
				}
			}

			// ------- cancel button --------//

			if( mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) 
				|| mouse.any_click(RIGHT_BUTTON) )
			{
				if( profileCount > 0 )
				{
					// go back to profile main menu
					menuMode = PROFILE_MENU_MAIN;
					refreshFlag = PPOPTION_ALL;
				}
				else
				{
					// cancel
					return 0;
				}
			}
			break;

		case PROFILE_MENU_DEL:
			if( mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				String str;
				str = profileArray[selectedProfile-1].file_name;
				str += ".PRF";
				remove(str);

				// BUGHERE : delete file inside the directory 

				// ###### patch begin Gilbert 12/4 #######//
				str = DIR_SAVE;
				str += profileArray[selectedProfile-1].save_dir;
				remove( str );
				// ###### patch end Gilbert 12/4 #######//

				if(stricmp( file_name, profileArray[selectedProfile-1].file_name) == 0)
				{
					// deleting current profile, unregister profile
					player_name[0] = '\0';
					err_when( is_registered() );
				}

				// go back to profile main menu
				menuMode = PROFILE_MENU_MAIN;
				refreshFlag = PPOPTION_ALL;
			}

			if( mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) 
				|| mouse.any_click(RIGHT_BUTTON) )
			{
				// go back to profile main menu
				menuMode = PROFILE_MENU_MAIN;
				refreshFlag = PPOPTION_ALL;
			}

			break;
		default:
			err_here();
		}
	}	// end while

	return 0;
}


// -------- begin of function PlayerProfile::re_create_directory -----//
//
// re-create directory for current profile
// assume save_dir is generated in the save_dir
//
// return true if success
//
int PlayerProfile::re_create_directory()
{
	if( !is_registered() )
		return 0;

	char pathName[MAX_PATH+1];
	strncpy( pathName, save_game_path(NULL), MAX_PATH );

	// create each directory on the path

	char *beginPtr = pathName;
	char *searchPtr = NULL;
	do
	{
		char backupSearchPtr;
		searchPtr = strchr( beginPtr, '\\' );
		if( searchPtr )
		{
			backupSearchPtr = *searchPtr;
			*searchPtr = '\0';		// replace '\\' with '\0' (string terminator)
		}

		if( searchPtr != beginPtr				// if path is "\SAVE\GILBERT", searchPtr == beginPtr
			&& !m.is_file_exist(pathName) )	// not beginPtr, eg "SAVE\GILBERT'
		{
			if( !CreateDirectory( pathName, NULL ) )
			{
				//String str = "Error creating ";
				//str += pathName;
				//str += " directory";
				// box.msg( str );
				box.msg( text_game_menu.str_profile_error_dir(pathName) );
				return 0;
			}
		}

		// restore backupSearchPtr

		if( searchPtr )
		{
			*searchPtr = backupSearchPtr;
			beginPtr = searchPtr+1;		// beginPtr points to next character after '\\'
		}

	} while( searchPtr );

	return 1;
}
// -------- end of function PlayerProfile::re_create_directory -----//

static void disp_scroll_bar_func(SlideBar *scroll, int)
{
	short rectLeft = scroll->rect_left();
	short rectRight = scroll->rect_right();
	if( rectLeft > scroll->scrn_x1 )		// fill region above scroll button
		vga_front.bar( scroll->scrn_x1, scroll->scrn_y1, rectLeft-1, scroll->scrn_y2, V_BLACK );
	vga_front.bar( rectLeft, scroll->scrn_y1, rectRight, scroll->scrn_y2, VGA_YELLOW+1);
	if( rectRight - rectLeft > 6 )
	{
		vga_front.d3_panel_up(rectLeft, scroll->scrn_y1, rectRight, scroll->scrn_y2, 2,0);
	}
	if( rectRight < scroll->scrn_x2 )		// fill region below scroll button
		vga_front.bar( rectRight+1, scroll->scrn_y1, scroll->scrn_x2, scroll->scrn_y2, V_BLACK );
}


// ------- begin of function PlayerProfile::load_count_profiles ----//
//
// <PlayerProfile *> profileArray - array to store player profiles, NULL to count
// <int>             maxLoad      - maximum number of profiles to load
//
int PlayerProfile::load_count_profiles( PlayerProfile *profileArray, int maxLoad )
{
	Directory profileDir;
	profileDir.read( "*.PRF", 1 );		// sort file name

	int profileCount = 0;
	for( int i = 1; i <= profileDir.size() && profileCount < maxLoad; ++i )
	{
		if( !profileArray )
		{
			// counting
			PlayerProfile tempProfile;
			if( tempProfile.load(profileDir[i]->name) )
			{
				++profileCount;
			}
		}
		else
		{
			if( profileArray[profileCount].load(profileDir[i]->name) )
			{
				++profileCount;
			}
		}
	}

	return profileCount;
}


// return 2 if successful
// return 0 if fail
int PlayerProfile::load_by_login_name(char* loginName)
{
	int count = load_count_profiles(NULL, 1000 );

	if( count == 0 )
		return 0;

	Blob blobProfileArray( count * sizeof(PlayerProfile) );
	PlayerProfile *profileArray = (PlayerProfile *)blobProfileArray.p();

	count = load_count_profiles(profileArray, count );

	for( int i = 0; i < count; ++i  )
	{
		if( strcmp(loginName, profileArray[i].login_name) == 0 )
		{
			*this = profileArray[i];
			return 2;
//			if( save() && re_create_directory() )	// call save, the file is latest for next time
//				return 2;
		}
	}

	return 0;
}
