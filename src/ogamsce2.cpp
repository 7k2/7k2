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

// Filename    : OGAMSCE2.CPP
// Description : select scenario menu


#include <ogame.h>
#include <osys.h>
#include <omouse.h>
#include <omousecr.h>
#include <color.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <ovgabuf.h>
#include <ovgalock.h>
#include <ogammenu.h>
#include <obitmap.h>
#include <oimgres.h>
#include <obutton.h>
#include <obutt3d.h>
#include <oslidcus.h>
#include <obox.h>
#include <opower.h>
#include <ofont.h>
#include <key.h>
#include <oblob.h>
#include <oinfo.h>
#include <ofiletxt.h>
#include <ovqueue.h>
#include <ogeta.h>
#include <omusic.h>
#include <oprofile.h>
#include <oconfig.h>
#include <ojpeg.h>
#include <ot_gmenu.h>

// --------- declare static funtion --------//

static void disp_scroll_bar_func(SlideVBar *scroll, int);


//enum { TUTOR_MENU_X1 = 0,
//		 TUTOR_MENU_Y1 = 0,
//		 TUTOR_MENU_WIDTH = VGA_WIDTH,
//		 TUTOR_MENU_HEIGHT = VGA_HEIGHT };
#define TUTOR_MENU_X1 0
#define TUTOR_MENU_Y1 0
#define TUTOR_MENU_WIDTH VGA_WIDTH
#define TUTOR_MENU_HEIGHT VGA_HEIGHT

enum { SCROLL_X1 = 757,
		 SCROLL_Y1 = 352,
		 SCROLL_X2 = 770,
		 SCROLL_Y2 = 492 };

enum { BROWSE_X1 = 30,
		 BROWSE_Y1 = 336,
		 BROWSE_REC_WIDTH  = 725,
		 BROWSE_REC_HEIGHT = 44,
		 BROWSE_X2 = BROWSE_X1 + BROWSE_REC_WIDTH - 1,
		 MAX_BROWSE_DISP_REC = 4 };

enum { TEXT_AREA_X1 = SCROLL_SHEET_X1 + 30,
		 TEXT_AREA_Y1 = 356,
		 TEXT_AREA_X2 = SCROLL_SHEET_X2 - 45,
		 TEXT_AREA_Y2 = SCROLL_SHEET_Y2 - 2,
		 TEXT_AREA_WIDTH = TEXT_AREA_X2 - TEXT_AREA_X1 + 1,
		 TEXT_AREA_HEIGHT = TEXT_AREA_Y2 - TEXT_AREA_Y1 + 1,
};

enum { TEXT_SCROLL_X1 = SCROLL_SHEET_X2 - 44,
	    TEXT_SCROLL_Y1 = TEXT_AREA_Y1,
		 TEXT_SCROLL_X2 = SCROLL_SHEET_X2 - 30,
		 TEXT_SCROLL_Y2 = TEXT_AREA_Y2,
};

enum { TEXT_OFFSET_X = 11,
		 TEXT_OFFSET_Y = 9 };

enum { PIC_AREA_X1 = 200,
		 PIC_AREA_Y1 = 150,
		 PIC_AREA_X2 = 599,
		 PIC_AREA_Y2 = 349,
};

enum { TITLE_TEXT_X1 = 136,
		 TITLE_TEXT_Y1 = 120,
		 TITLE_TEXT_X2 = 679,
		 TITLE_TEXT_Y2 = 140
};


#define TUOPTION_BROWSE(s)     (1 << (s))
#define TUOPTION_ALL_BROWSE    0x0000ffff
#define TUOPTION_PAGE          0x40000000
#define TUOPTION_TEXT_AREA     0x00020000
#define TUOPTION_PIC_AREA      0x00040000
#define TUOPTION_SCROLL        0x00080000
#define TUOPTION_TEXT_SCROLL   0x00100000
#define TUOPTION_TEXT_BUFFER   0x00200000
// ##### begin Gilbert 2/7 ########//
#define TUOPTION_SCEN_DETAIL   0x00ff0000
#define TUOPTION_NAME_FIELD    0x10000000
#define TUOPTION_ENUMERATE     0x20000000
// ##### end Gilbert 2/7 ########//
#define TUOPTION_ALL           0xffffffff

//---------- Begin of function Game::select_scenario ----------//
//
// Select a scenario. 
//
// <int>    scenCount		   - no. of available scenarios. 
// <char**> scenFileNameArray - an array of <char*> pointing to the file name of the
//										  available scenarios. 
//
// return : <int> >0 - id. of the scenario selected. 
//                 0 - cancel
//
int Game::select_scenario(int scenCount, ScenInfo* scenInfoArray)
{
	if( scenCount==0 )
	{
		box.msg( text_game_menu.str_none_scenario() ); // "Scenario files not found." );
		return 0;
	}

	//-------------------------------------//

	// ##### begin Gilbert 4/11 ########//
	// stop any music
	music.stop();
	// ##### end Gilbert 4/11 ########//

//	int menuX1 = TUTOR_MENU_X1;
//	int menuY1 = TUTOR_MENU_Y1;

	mouse_cursor.set_icon(CURSOR_NORMAL);

	power.win_opened = 1;

	int i, j;
	int scenCountArray[MAX_SCENARIO_PATH];
	memset( scenCountArray, 0, sizeof(scenCountArray) );	// enable

	Button groupEnableButton[MAX_SCENARIO_PATH];
//	ButtonGroup groupEnableButton(MAX_SCENARIO_PATH);

	// create one button for each path
	{
		// char *groupEnableButtonStr[MAX_SCENARIO_PATH] = { "Custom", "Pre-made",	};
		const int groupButtonWidth = 100;
		const int groupButtonHeight = 20;
		int x1 = SCROLL_SHEET_X1+40;

		for( i = 0; i < MAX_SCENARIO_PATH; ++i, (x1+=groupButtonWidth) )
		{
			// inelastic and enabled at the beginning
			groupEnableButton[i].create_text( x1, SCROLL_SHEET_Y2+20, 
				x1 + groupButtonWidth-1, SCROLL_SHEET_Y2+20+groupButtonHeight-1, 
				text_game_menu.str_scenario_path_name(i), 0, 1 );
		}
	}

	// now count number of scenario in each path

	for( i = 0; i < scenCount; ++i )
	{
		j = scenInfoArray[i].dir_id;

		// drop auto.scn and auto2.scn in dir 0
		if( j == 0 && (stricmp( scenInfoArray[i].file_name, "AUTO.SCN") == 0
			|| stricmp( scenInfoArray[i].file_name, "AUTO2.SCN") == 0) )
			continue;

		if( j >= 0 && j < MAX_SCENARIO_PATH )
			scenCountArray[j]++;
	}

	// disable button which has no scenario

	for( j = 0; j < MAX_SCENARIO_PATH; ++j )
	{
		if( scenCountArray[j] == 0 )
		{
			groupEnableButton[j].enable_flag = 0;
			groupEnableButton[j].is_pushed = 0;
		}
	}

	for( j = 0; j < MAX_SCENARIO_PATH && scenCountArray[j] == 0; ++j )
	if( j >= MAX_SCENARIO_PATH )		// all scenario are empty (when auto.scn not considered)
	{
		box.msg( text_game_menu.str_none_scenario() ); // "Scenario files not found." );
		return 0;
	}

	int briefMode = 0;
	Button briefModeButton;
	briefModeButton.create_text( SCROLL_SHEET_X2-140, SCROLL_SHEET_Y2+42, 
		SCROLL_SHEET_X2-40, SCROLL_SHEET_Y2+62, text_game_menu.str_brief_mode(briefMode), 0 );

#ifdef DEMO
	briefModeButton.visible_flag = 0;
	for( i = 0; i < MAX_SCENARIO_PATH; ++i )
		groupEnableButton[i].visible_flag = 0;
#endif

	int currentScenCount = 0;			// sum of scenCountArray which are enabled
	// int scenIndex[scenCount];
	Blob blobScenIndex( sizeof(int)*scenCount );
	int *scenIndex = (int *)blobScenIndex.p();		// gives the index in scenInfoArray, given 0 to currentScenCount-1

	int minRecno = 1;
	int browseRecno = minRecno;

	int maxPage = 1;
	int page = 0;		// page is between 0 to maxPage, so add 1 before display
	int itemPerPage[2] = { 1, 8 };		// itemPerPage[briefMode]

	// take name from profile
	
	if( player_profile.is_registered() )
	{
		strcpy( config.player_name, player_profile.player_name );
	}

	//--------------------------------------//
//	Button3D scrollUp, scrollDown, startButton, cancelButton;
	int retFlag = 0;
	int refreshFlag = TUOPTION_ALL;

//	char *arrowBitmap = image_button.read("TRI-R");
	char *arrowBitmap = image_button.read("SCROLL-R");
	char *arrowBitmap2 = image_button.read("SCROLL-L");
	int pageUpX1 = (LSCROLL_X2 + LSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageUpY1 = (LSCROLL_Y2 + LSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageUpX2 = pageUpX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageUpY2 = pageUpY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;
	int pageDownX1 = (RSCROLL_X2 + RSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageDownY1 = (RSCROLL_Y2 + RSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageDownX2 = pageDownX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageDownY2 = pageDownY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;

	int pageNoX1 = 635;
	int pageNoY1 = 87;
	int pageNoX2 = 675;
	int pageNoY2 = 107;

//	scrollUp.create(menuX1+SCROLL_X1,menuY1+SCROLL_Y1-17, "SV-UP-U", "SV-UP-D", 1, 0);
//	scrollDown.create(menuX1+SCROLL_X1,menuY1+SCROLL_Y2+1, "SV-DW-U", "SV-DW-D", 1, 0);
//	startButton.create(menuX1+373, menuY1+529, "START-U", "START-D",1, 0);
//	cancelButton.create(menuX1+548, menuY1+529, "CANCEL-U", "CANCEL-D", 1, 0);

//	SlideVBar scrollBar;
//	scrollBar.init_scroll(menuX1+SCROLL_X1, menuY1+SCROLL_Y1, menuX1+SCROLL_X2, menuY1+SCROLL_Y2,
//		MAX_BROWSE_DISP_REC, disp_scroll_bar_func);
//	scrollBar.set(minRecno, scenCount, minRecno);

//	scrollBar.set_view_recno(browseRecno - MAX_BROWSE_DISP_REC/2);
	Button3D textScrollUp, textScrollDown;
	textScrollUp.create(TEXT_SCROLL_X1, TEXT_SCROLL_Y1-17,
		"SV-UP-U", "SV-UP-D", 1, 0);
	textScrollDown.create(TEXT_SCROLL_X1,TEXT_SCROLL_Y2+1, 
		"SV-DW-U", "SV-DW-D", 1, 0);
	VLenQueue textBuffer;
	*(textBuffer.reserve(1)) = '\0';

	Font &textFont = font_cara_w;
	const int TEXT_LINE_SPACE = 2;
	const int ESTIMATED_LINE_IN_TEXT_AREA =
		(TEXT_AREA_Y2 - TEXT_AREA_Y1 + 1) / (textFont.font_height + TEXT_LINE_SPACE);

	SlideVBar textScrollBar;
	textScrollBar.init_scroll(TEXT_SCROLL_X1, TEXT_SCROLL_Y1, 
		TEXT_SCROLL_X2, TEXT_SCROLL_Y2,
		ESTIMATED_LINE_IN_TEXT_AREA, disp_scroll_bar_func);

	GetA playerNameField;
	playerNameField.init( 364, 89, 632,
		config.player_name, config.PLAYER_NAME_LEN, &font_bold_black, 0, 1);

	Jpeg jpeg;

	VgaFrontLock vgaLock;

	while(1)
	{
#ifndef NO_WINDOWS  // FIXME
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
#endif
		if( sys.need_redraw_flag )
		{
			refreshFlag = TUOPTION_ALL;
			sys.need_redraw_flag = 0;
		}

		VgaFrontReLock vgaReLock;

		//---------- yield --------//

		sys.yield();

		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();


		// --------- display ----------//

		if( refreshFlag )
		{
			if( refreshFlag & TUOPTION_PAGE )
			{
				vga.use_back();
				vga_util.disp_image_file("CHOOSE");

				font_bold_black.right_put( playerNameField.x, playerNameField.y,
					text_game_menu.str_kingdom_of() ); // "Kingdom of " );

				// ----- display start, cancel button ------//

				font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
					text_game_menu.str_start() ); // "Start" );
				font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
					text_game_menu.str_cancel() ); // "Cancel" );

				// ----- display level, bonus ---------//

				font_bld.center_put( TITLE_TEXT_X2-100, TITLE_TEXT_Y1-25, TITLE_TEXT_X2-50, TITLE_TEXT_Y2-8, text_game_menu.str_level_in_score() );
				font_bld.center_put( TITLE_TEXT_X2-50, TITLE_TEXT_Y1-25, TITLE_TEXT_X2, TITLE_TEXT_Y2-8, text_game_menu.str_bonus() );

				vga.use_front();
				vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );

				for(j = 0; j < MAX_SCENARIO_PATH; ++j )
				{
					groupEnableButton[j].paint();
				}

				briefModeButton.set_body( text_game_menu.str_brief_mode(briefMode) );
				briefModeButton.paint(briefMode);
			}

			if( refreshFlag & TUOPTION_ENUMERATE )
			{
				// now count number of scenario in each path
				int selectedScenario = scenIndex[browseRecno-1];	// need to set the new browseRecno
				browseRecno = 1;		// reset new browseRecno
				currentScenCount = 0;
				for( i = 0; i < scenCount; ++i )
				{
					j = scenInfoArray[i].dir_id;

					// drop auto.scn and auto2.scn in dir 0
					if( j == 0 && (stricmp( scenInfoArray[i].file_name, "AUTO.SCN") == 0
						|| stricmp( scenInfoArray[i].file_name, "AUTO2.SCN") == 0) )
						continue;

					if( j >= 0 && j < MAX_SCENARIO_PATH )
					{
						if( groupEnableButton[j].is_pushed )
						{
							scenIndex[currentScenCount] = i;
							currentScenCount++;

							// update browseRecno
							if( i == selectedScenario )
								browseRecno = currentScenCount;		// set new browseRecno to the one before change
						}
					}
				}
				err_when( currentScenCount > scenCount );

				// recalcuate maxPage

				maxPage = (currentScenCount+itemPerPage[briefMode]-1) / itemPerPage[briefMode];
				if( maxPage <= 1 )
					maxPage = 1;			// at least one page				

				// set page, depend of browseRecno
				if( browseRecno > 0 )
					page = (browseRecno-1) / itemPerPage[briefMode];
				else
					page = 0;
			}

			char* scenFileName;
			char	pictName[20];
			char	textName[20];
			char* pathName;

			scenFileName = scenInfoArray[scenIndex[browseRecno-1]].file_name;
			misc.change_file_ext( pictName, scenFileName, "scp" );
			misc.change_file_ext( textName, scenFileName, "SCT" );
			pathName = DIR_SCENARIO_PATH(scenInfoArray[scenIndex[browseRecno-1]].dir_id);
			err_when( ! *pathName );

			if( (refreshFlag & TUOPTION_PIC_AREA) && briefMode == 0 )
			{
				String str;

				str = pathName;
				str += pictName;

				if( browseRecno && misc.is_file_exist(str) )
				{
					jpeg.put_to_buf( &vga_front, PIC_AREA_X1, PIC_AREA_Y1, str );
				}
				else
				{
					// draw the background
					vga_util.blt_buf( PIC_AREA_X1, PIC_AREA_Y1, PIC_AREA_X2, PIC_AREA_Y2, 0 );
				}
			}

			if( refreshFlag & TUOPTION_TEXT_BUFFER )
			{
				// load text buffer
				String str;

				str = pathName;
				str += textName;

				if( browseRecno && misc.is_file_exist(str) )
				{
					File textFile;
					int dataSize;
					textFile.file_open(str);
					// ##### patch begin Gilbert 2/2 ####//
					dataSize = textFile.file_size();

					FileTxt fileTxt( &textFile, dataSize );  // initialize fileTxt with an existing file stream

					fileTxt.next_line();		// skip the title lines
					fileTxt.next_line();
					fileTxt.next_line();
					fileTxt.next_line();

					textBuffer.clear();
					fileTxt.read_paragraph(textBuffer.reserve(dataSize+8), dataSize);
					// ##### end begin Gilbert 2/2 ####//

			      int dispLines;    // no. of lines can be displayed on the area
			      int totalLines;   // total no. of lines of the text

					textFont.count_line( TEXT_AREA_X1, TEXT_AREA_Y1,
						TEXT_AREA_X2, TEXT_AREA_Y2,
						textBuffer.queue_buf, TEXT_LINE_SPACE, dispLines, totalLines );

					// textScrollBar.view_size = dispLines;
					textScrollBar.set(1, totalLines ,1);
					refreshFlag |= TUOPTION_TEXT_SCROLL;
				}
				else
				{
					// clear the text buffer
					textBuffer.clear();
					*textBuffer.reserve(1) = '\0';
					textScrollBar.set(1, 1 ,1);
					refreshFlag |= TUOPTION_TEXT_SCROLL;
				}
			}

			if( refreshFlag & TUOPTION_TEXT_AREA )
			{
				vga_util.blt_buf( TEXT_AREA_X1, TEXT_AREA_Y1, TEXT_AREA_X2, TEXT_AREA_Y2, 0 );
				if( textBuffer.queue_buf[0] != '\0' )
				{
					textFont.put_paragraph(TEXT_AREA_X1, TEXT_AREA_Y1, TEXT_AREA_X2, TEXT_AREA_Y2,
						textBuffer.queue_buf, TEXT_LINE_SPACE, textScrollBar.view_recno );		// 4 - space between lines
				}
			}

			if( refreshFlag & TUOPTION_TEXT_SCROLL )
			{
				// display scroll bar
				if( textScrollBar.max_recno > ESTIMATED_LINE_IN_TEXT_AREA )
					textScrollBar.paint();
				else
					vga_util.blt_buf( textScrollBar.scrn_x1, textScrollBar.scrn_y1,
						textScrollBar.scrn_x2, textScrollBar.scrn_y2, 0 );
			}

			if( refreshFlag & TUOPTION_SCROLL )
			{
				// display scroll bar
				// scrollBar.paint();
			}

			if( refreshFlag & TUOPTION_ALL_BROWSE )
			{
				int rec = page * itemPerPage[briefMode] + 1; // browseRecno;
				int x1 = TITLE_TEXT_X1;
				int y1 = TITLE_TEXT_Y1;
				int x2 = TITLE_TEXT_X2;
				int y2 = TITLE_TEXT_Y2;
				int rowHeight = TITLE_TEXT_Y2 - TITLE_TEXT_Y1 + 9;

				for( i = itemPerPage[briefMode]; i > 0; --i, ++rec, (y1+=rowHeight), (y2+=rowHeight) )
				{
					vga_util.blt_buf( x1, y1, x2, y2, 0 );

					if( rec >= 1 && rec <= currentScenCount )
					{
						ScenInfo *scenInfo = &scenInfoArray[scenIndex[rec-1]];

						//----- display the scenario name -----//

						if( scenInfo->scen_name[0] )
						{
							(browseRecno==rec?font_bold_red:font_bold_black).center_put(
								x1, y1, x2-50, y2, scenInfo->scen_name );

							//---- display the scenario difficulty and bonus points ----//

							textFont.center_put( x2-100, y1, x2-50, y2, misc.format(scenInfo->goal_difficulty), 1 );
							textFont.center_put( x2-50, y1, x2, y2, misc.format(scenInfo->goal_score_bonus), 1 );
						}
						else		// description not found, display file name
						{
							(browseRecno==rec?font_bold_red:font_bold_black).center_put(
								x1, y1, x2-50, y2,	scenInfo->file_name );
						}
					}
				}
			}

			if( refreshFlag & TUOPTION_NAME_FIELD )
				playerNameField.paint();

			String str;
			str = page+1; //browseRecno;
			str += "/";
			str += maxPage; // currentScenCount;
			font_cara_w.center_put( pageNoX1, pageNoY1, pageNoX2, pageNoY2, str, 1 );

			if( page > 0 )
			{
				mouse.hide_area( pageUpX1, pageUpY1, pageUpX2, pageUpY2 );
			//	vga.active_buf->put_bitmap_trans_decompress_hmirror( pageUpX1, pageUpY1, arrowBitmap );
				vga.active_buf->put_bitmap_trans_decompress( pageUpX1, pageUpY1, arrowBitmap2 );
				mouse.show_area();
			}
			else
				vga_util.blt_buf( pageUpX1, pageUpY1, pageUpX2, pageUpY2, 0 );

			if( page < maxPage-1)
			{
				mouse.hide_area( pageDownX1, pageDownY1, pageDownX2, pageDownY2 );
				vga.active_buf->put_bitmap_trans_decompress( pageDownX1, pageDownY1, arrowBitmap );
				mouse.show_area();
			}
			else
				vga_util.blt_buf( pageDownX1, pageDownY1, pageDownX2, pageDownY2, 0 );

			refreshFlag = 0;
		}

		sys.blt_virtual_buf();

		if( mouse.any_click(LSCROLL_X1, LSCROLL_Y1, LSCROLL_X2, LSCROLL_Y2) )
		{
			if( page > 0 )
			{
				--page;
				if( itemPerPage[briefMode] == 1 )		// change browseRecno as well
					browseRecno = page+1;
				refreshFlag |= TUOPTION_ALL & ~TUOPTION_ENUMERATE;
			}
		}
		else if( mouse.any_click(RSCROLL_X1, RSCROLL_Y1, RSCROLL_X2, RSCROLL_Y2) )
		{
			if( page < maxPage-1 )
			{
				++page;
				if( itemPerPage[briefMode] == 1 )		// change browseRecno as well
					browseRecno = page+1;
				refreshFlag |= TUOPTION_ALL & ~TUOPTION_ENUMERATE;
			}
		}
		else if( textScrollBar.max_recno > ESTIMATED_LINE_IN_TEXT_AREA
			&& textScrollBar.detect() == 1 )
		{
			refreshFlag |= TUOPTION_TEXT_SCROLL | TUOPTION_TEXT_AREA;
		}
		else if( textScrollUp.detect() )
		{
			// click on scroll up
			int oldValue = textScrollBar.view_recno;
			if( oldValue != textScrollBar.set_view_recno(oldValue-1) )
				refreshFlag |= TUOPTION_TEXT_SCROLL | TUOPTION_TEXT_AREA;
		}
		else if( textScrollDown.detect() )
		{
			// click on scroll down
			int oldValue = textScrollBar.view_recno;
			if( oldValue != textScrollBar.set_view_recno(oldValue+1) )
				refreshFlag |= TUOPTION_TEXT_SCROLL | TUOPTION_TEXT_AREA;
		}
		else if( playerNameField.detect() )
		{
			// load button
			refreshFlag = TUOPTION_NAME_FIELD;
		}
		else if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
		{
			// cancel button or escape key
			refreshFlag = TUOPTION_ALL;
			retFlag = 0;
			break;		// break while(1)
		}
		else if( mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
		{
			// load button
			refreshFlag = TUOPTION_ALL;
			retFlag = scenIndex[browseRecno-1]+1;
			break;
		}

		// detect scroll area
		{
			int recBegin = page * itemPerPage[briefMode] + 1; // browseRecno;
			int rec = recBegin;
			int x1 = TITLE_TEXT_X1;
			int y1 = TITLE_TEXT_Y1;
			int x2 = TITLE_TEXT_X2;
			int y2 = TITLE_TEXT_Y2;
			int rowHeight = TITLE_TEXT_Y2 - TITLE_TEXT_Y1 + 9;
			int exitFlag = 0;

			for( i = itemPerPage[briefMode]; i > 0; --i, ++rec, (y1+=rowHeight), (y2+=rowHeight) )
			{
				if( rec < 1 || rec > currentScenCount )
					continue;

				if( mouse.double_click( x1, y1, x2, y2 ) )
				{
					exitFlag = 1;
					retFlag = scenIndex[browseRecno-1]+1;
					break;
				}
				else if( mouse.single_click( x1, y1, x2, y2 ) )
				{
					if( rec != browseRecno )
					{
						if( browseRecno-recBegin >= 0 && browseRecno-recBegin < itemPerPage[briefMode] )
							refreshFlag |= TUOPTION_BROWSE(browseRecno-recBegin);
						browseRecno = rec;
						refreshFlag |= TUOPTION_SCEN_DETAIL | TUOPTION_BROWSE(browseRecno-recBegin);
					}
				}
			}

			if( exitFlag )
				break;		// break while(1) loop
		}

		// detect group enable button

		for( j = 0; j < MAX_SCENARIO_PATH; ++j )
		{
			int rc;
			if( (rc = groupEnableButton[j].detect(0, 0, 2)) )	// detect right click 
			{
				// cannot clear all buttons
				if( rc == 2 )
				{
					for( i = 0; i < MAX_SCENARIO_PATH && !groupEnableButton[i].is_pushed; ++i );
					if( i >= MAX_SCENARIO_PATH )
					{
						// roll back the change
						groupEnableButton[j].paint(1);
						break;
					}
				}

				if( rc == 1 )		// left click, disable all other buttons and enable this button
				{
					for( i = 0; i < MAX_SCENARIO_PATH; ++i )
					{
						if( groupEnableButton[i].enable_flag )
							groupEnableButton[i].paint( i==j );
					}
				}

				// update currentScenCount and scenIndex
				refreshFlag = TUOPTION_ALL;
				break;		// j may be overwritten, must break now
			}
		}

		if( briefModeButton.detect() )
		{
			briefMode = briefModeButton.is_pushed ? 1 : 0;
			refreshFlag = TUOPTION_ALL;
		}
	}

	power.win_opened = 0;

	return retFlag;
}
//------------ End of function Game::select_scenario -----------//


static void disp_scroll_bar_func(SlideVBar *scroll, int)
{
	short rectTop = scroll->rect_top();
	short rectBottom = scroll->rect_bottom();
	vga_front.bar( scroll->scrn_x1, rectTop, scroll->scrn_x2, rectBottom, VGA_YELLOW+1);
	if( rectBottom - rectTop > 10 )
	{
		vga_front.d3_panel_up(scroll->scrn_x1, rectTop, scroll->scrn_x2, rectBottom,2,0);
	}
	if( rectTop > scroll->scrn_y1 )		// fill regin above the scroll button
	{
		vga_front.bar( scroll->scrn_x1, scroll->scrn_y1, scroll->scrn_x2, rectTop-1, V_BLACK );
	}
	if( scroll->scrn_y2 > rectBottom )		// fill regin above the scroll button
	{
		vga_front.bar( scroll->scrn_x1, rectBottom+1, scroll->scrn_x2, scroll->scrn_y2, V_BLACK );
	}
}


