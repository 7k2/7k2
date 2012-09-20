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

//Filename    : OGFILEA.CPP
//Description : Game File Array

#include <stdio.h>
#include <key.h>
#include <odir.h>
#include <osys.h>
#include <odate.h>
#include <omouse.h>
#include <omousecr.h>
#include <opower.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ounitres.h>
#include <oimgres.h>
#include <ovga.h>
#include <vga_util.h>
#include <ocoltbl.h>
#include <obox.h>
#include <ofont.h>
#include <oinfo.h>
#include <ogame.h>
#include <ogameset.h>
#include <ogfile.h>
#include <obutton.h>
#include <obutt3d.h>
#include <ogeta.h>
#include <oslidcus.h>
#include <oblob.h>
#include <ogammenu.h>
#include <obitmap.h>
#include <ocampgn.h>
#include <ot_gmenu.h>
#include <ot_basic.h>
#include <win32_compat.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(GameFile);


//--------- Define constant ---------//

//enum { FILE_MENU_WIDTH = 638,
//		 FILE_MENU_HEIGHT = 398 };
//enum { FILE_MAIN_MENU_X1 = 80,
//		 FILE_MAIN_MENU_Y1 = 175 };
//enum { FILE_IN_GAME_MENU_X1 = 80,
//		 FILE_IN_GAME_MENU_Y1 = 115 };
//enum { BROWSE_X1 = 34,
//		 BROWSE_Y1 = 31,
//		 BROWSE_REC_WIDTH  = 538,
//		 BROWSE_REC_HEIGHT = 62,
//		 BROWSE_X2 = BROWSE_X1 + BROWSE_REC_WIDTH - 1 };

//enum { SCROLL_X1 = 595,
//		 SCROLL_Y1 = 47,
//		 SCROLL_X2 = 609,
//		 SCROLL_Y2 = 324,
//		 SCROLL_WIDTH = SCROLL_X2 - SCROLL_X1 + 1,
//		 SCROLL_HEIGHT = SCROLL_Y2 - SCROLL_Y1 + 1 };

#define SLOT_WIDTH 560
#define SLOT_HEIGHT 76

//----- File name of the game file array --------//

#define MAX_BROWSE_DISP_REC	   4		// max. no. of records can be displayed in the saved game browser

#define HALL_OF_FAME_FILE_NAME  "hallfame.dat"

//------- Declare static vars & functions ----------//

static char    action_mode;
static char		pre_game;
static short	browse_recno;
static short	browse_top_recno;
static short	menu_x1, menu_y1;

static int     sort_game_file_function( const void *a, const void *b );
static int     last_game_recno();
static void    key_search();
static void		disp_scroll_bar_func(SlideVBar *scroll, int);

//------ Begin of function GameFileArray constuctor ------//

GameFileArray::GameFileArray() : DynArray( sizeof(GameFile), 10 )
{
	demo_format = 0;
/*
	#ifdef DEMO
		demo_format = 1;
	#endif

	#ifdef DEMO_DESIGN
		demo_format = 1;
	#endif
*/
	last_file_name[0] = '\0';
	save_default_dir[0] = '\0';
	strcpy( save_default_ext, "*.SAV");
	has_read_hall_of_fame = 0;

	memset( hall_fame_array[0], 0, sizeof(HallFame)*HALL_FAME_NUM );
	memset( hall_fame_array[1], 0, sizeof(HallFame)*HALL_FAME_NUM );
}
//-------- End of function GameFileArray constuctor ------//


//------ Begin of function GameFileArray::init ------//
//
// <char *>path - path of the save game, without '\\' at the end
// <char *>extStr - extension of the save game
//
void GameFileArray::init(const char *path, const char *extStr)
{
	//------------- Read Hall of Fame ------------//

	if( !has_read_hall_of_fame )		// only read once, GameFileArray::init() is called every time the load/save game menu is brought up.
	{
		read_hall_of_fame();
		has_read_hall_of_fame = 1;
	}

	//-- Load all headers of all saved game files in current directory --//

	strcpy( save_default_dir, path );
	// REMOVE '\\' at the end if any, 
	misc.rtrim( save_default_dir);		// trim space at the end first
	int dirLen = strlen(save_default_dir);
	if( dirLen > 1 && save_default_dir[dirLen-1] == '\\' )
		save_default_dir[dirLen-1] = '\0';

	strcpy( save_default_ext, extStr);

	load_all_game_header(save_default_dir, save_default_ext);
}
//-------- End of function GameFileArray::init ------//


// ------- begin of function GameFileArray::reload -------//
//
// load hall of fame and load_all_game_header from default directory
//
void GameFileArray::reload()
{
	load_all_game_header(save_default_dir, save_default_ext);
}
// ------- end of function GameFileArray::reload -------//


//------ Begin of function GameFileArray::deinit ------//

void GameFileArray::deinit()
{
}
//-------- End of function GameFileArray::deinit ------//


#define LSOPTION_SLOT(n) (1 << (n))
#define LSOPTION_ALL_SLOTS       0x0000ffff
#define LSOPTION_SCROLL          0x00010000
#define LSOPTION_PAGE_NO         0x00020000
#define LSOPTION_PAGE            0x40000000
#define LSOPTION_ALL             0x7fffffff


//-------- Begin of function GameFileArray::menu --------//
//
// <int> actionMode = -2 - save screen to back buffer
//                    -1 - restore screen to back buffer
//                    1 - save game
//                    2 - load game
//                    3 - load game from main menu
//
// <int *> recno    = if overwritting save game or load game acion 
//                    is succcessful, return the recno of GameFile
//
// return : <int> 1 - game loaded/saved
//                0 - user cancel loading/saving 
//               -1 - loading/saving error
//
int GameFileArray::menu(int actionMode, int *recno)
{
	enum { PAGE_WIDTH = 800, PAGE_HEIGHT = 600 };

	int bx = (VGA_WIDTH - PAGE_WIDTH) / 2;
	int by = (VGA_HEIGHT - PAGE_HEIGHT) / 2;

	if( actionMode == -2 || actionMode == -1)
	{
		// copy or restore screen to back buffer
		int scrnX1, scrnY1, scrnX2, scrnY2;
		scrnX1 = bx;
		scrnY1 = by;
		scrnX2 = scrnX1 + PAGE_WIDTH - 1;
		scrnY2 = scrnY1 + PAGE_HEIGHT - 1;

		mouse.hide_area( scrnX1, scrnY1, scrnX2, scrnY2);

		if( actionMode == -2 )
		{
			// save to back buffer
			// vga_back.blt_buf_fast( &vga_front, scrnX1, scrnY1, scrnX2, scrnY2);
		}
		else
		{
			// restore from back buffer
			// vga_front.blt_buf_fast( &vga_back, scrnX1, scrnY1, scrnX2, scrnY2);
			sys.need_redraw_flag = 1;
		}

		mouse.show_area();

		return 1;
	}

	// #### begin Gilbert 25/9 ########//
	//#ifdef DEMO          // No game saving in demo version
	//	box.msg( "Sorry, you cannot load/save game in the demo version." );
	//	return -1;
	//#endif
	// #### end Gilbert 25/9 ########//

	action_mode = actionMode;
	pre_game = 0;
	if( actionMode == 3 )		// actionMode = 3 means load game from main menu, so action_mode is still two
	{
		action_mode = 2;
		pre_game = 1;
	}

	if( action_mode==2 && size()==0 )
	{
		box.msg( text_game_menu.str_none_save_game() ); // "You haven't saved any games yet." );
		return 0;
	}

	// load race icon
	char deinitGameSet=0;
	char deinitUnitRes=0;
	char deinitRaceRes=0;
	char deinitMonsterRes=0;

	if( !game_set.set_opened_flag )
	{
		game_set.open_set(1);
		deinitGameSet = 1;
	}
	if( !unit_res.init_flag )
	{
		unit_res.init();
		deinitUnitRes = 1;
	}
	if( !race_res.init_flag )
	{
		race_res.init();
		deinitRaceRes = 1;
	}

	if( !monster_res.init_flag )
	{
		monster_res.init();
		// ##### begin Gilbert 30/10 ######//
		// deinitRaceRes = 1;
		deinitMonsterRes = 1;
		// ##### end Gilbert 30/10 ######//
	}

	char useBack = vga.use_back_buf;
	vga.use_front();

	//-------------------------------------//

//	int x=menu_x1, y=menu_y1+17;

	// vga_back.adjust_brightness( x, y, x+menu_x1-1, y+menu_y1-1, -6 );
//	vga_util.blt_buf( x, y, x+menu_x1-1, y+menu_y1-1, 0 );

	mouse_cursor.set_icon(CURSOR_NORMAL);
	mouse_cursor.set_frame(0);

	power.win_opened = 1;

//	int minRecno = action_mode == 1 ? 0 : 1;
	int minRecno = 1;

	//------ set current record no. -------//

	// ####### begin Gilbert 3/5 ########//
	int i;
	if( size() == 0 )
	{
		browse_recno = 0;
	}
	else
	{
		browse_recno = 1;
		for( i=1 ; i<=size() ; i++ )
		{
			if( stricmp(last_file_name, game_file_array[i]->file_name)==0 )
			{
				browse_recno = i;
				break;
			}
		}
	}
	// ####### end Gilbert 3/5 ########//

	//---------------------------------//

	browse_top_recno = minRecno;
	// in save game mode, browse_recno = 0 means selecting empty slot
	// in load game mode, browse_recno = 0 means nonthing is selected
	// browse_top_recno = browse_recno ? browse_recno : minRecno;

	// -------- generate palette ------//

	short colorRemapTable[0x100];
	{
		String str(DIR_IMAGE);
		if( pre_game )
			str += "CHOOSE.COL";
		else
			str += "CHOOSE2.COL";

		File palFile;
		palFile.file_open(str);
		ColorTable colorTable;

		BYTE palBuf[0x100][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	int retFlag = 0;
	int refreshFlag = LSOPTION_ALL;

#define SLOT_Y_SPACING 80
#define SLOT_X1(n) (bx + 119)
#define SLOT_Y1(n) (by + SLOT_Y_SPACING * (n) + 110)
#define SLOT_X2(n) (bx + 119 + SLOT_WIDTH - 1)
#define SLOT_Y2(n) (by + SLOT_Y_SPACING * (n) + 110 + SLOT_HEIGHT - 1)

	char *arrowBitmap = image_button.read("TRI-R");
	int pageUpX1 = bx + (LSCROLL_X2 + LSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageUpY1 = by + (LSCROLL_Y2 + LSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageUpX2 = pageUpX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageUpY2 = pageUpY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;
	int pageDownX1 = bx + (RSCROLL_X2 + RSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageDownY1 = by + (RSCROLL_Y2 + RSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageDownX2 = pageDownX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageDownY2 = pageDownY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;

	int pageNoX1 = bx + 556;
	int pageNoY1 = by + 87;
	int pageNoX2 = bx + 675;
	int pageNoY2 = by + 107;

	// ------- define save buffer area ----------//

	Blob2DW browseArea[MAX_BROWSE_DISP_REC];		// save bitmap of save game slot areas
	Blob2DW pageUpArea, pageDownArea;
	Blob2DW pageNoArea;

	// ------- define button object ----------//

	Button startButton, cancelButton, delButton;
	Button saveNewButton;

	if( !pre_game )
	{
		startButton.create_text( bx+BUTTON2_X1, by+BUTTON2_Y1, bx+BUTTON2_X2, by+BUTTON2_Y2, 
			action_mode == 1 ? text_game_menu.str_save() : text_game_menu.str_load() );
		if( action_mode == 1 )
			saveNewButton.create_text( bx+BUTTON3_X1, by+BUTTON3_Y1, bx+BUTTON3_X2, by+BUTTON3_Y2, 
				text_game_menu.str_save_new() );
		delButton.create_text( bx+BUTTON7_X1, by+BUTTON7_Y1, bx+BUTTON7_X2, by+BUTTON7_Y2, text_game_menu.str_delete_save() );
		cancelButton.create_text( bx+BUTTON4_X1, by+BUTTON4_Y1, bx+BUTTON4_X2, by+BUTTON4_Y2, text_game_menu.str_cancel() );
	}

//	int i;
	int page, maxPage;
	maxPage = (size()-minRecno+1 + MAX_BROWSE_DISP_REC-1) / MAX_BROWSE_DISP_REC;	// divide but round up
	page = (browse_recno - minRecno) / MAX_BROWSE_DISP_REC;

	while(1)
	{
		//---------- yield --------//

		sys.yield();

		mouse.get_event();

		// --------- display ----------//

		if( refreshFlag )
		{
			if( refreshFlag & LSOPTION_PAGE )
			{
				// ------ hide cursor ----//

				mouse.hide();

				// ------- display image --------//

				File imageFile;
				String str(DIR_IMAGE);
				if( pre_game )
				{
					str += "CHOOSE.ICN";
					imageFile.file_open(str);
					vga.active_buf->put_large_bitmap(bx, by, &imageFile, colorRemapTable);
				}
				else
				{
					str += "CHOOSE2.ICN";
					imageFile.file_open(str);
					vga.active_buf->put_large_bitmap_trans(bx, by, &imageFile, colorRemapTable);
				}

				// ------- display title -------//

				if( action_mode == 1 )
					font_bold_black.center_put( bx+SCROLL_SHEET_X1, by+SCROLL_SHEET_Y1, 
						bx+SCROLL_SHEET_X2, SLOT_Y1(0), text_game_menu.str_save_game() ); // "Save Game" );
				else if( action_mode == 2 )
					font_bold_black.center_put( bx+SCROLL_SHEET_X1, by+SCROLL_SHEET_Y1, 
						bx+SCROLL_SHEET_X2, SLOT_Y1(0), text_game_menu.str_load_game() ); // "Load Game" );

				// display save, save new/ load, delete cancel button

				if( pre_game )
				{
					if( action_mode == 1 )
					{
						font_thin_black.center_put( bx+BUTTON2_X1, by+BUTTON2_Y1, bx+BUTTON2_X2, by+BUTTON2_Y2, text_game_menu.str_save() ); // "Save" );
						font_thin_black.center_put( bx+BUTTON3_X1, by+BUTTON3_Y1, bx+BUTTON3_X2, by+BUTTON3_Y2, text_game_menu.str_save_new() ); // "Save New" );
					}
					else if( action_mode == 2 )
					{
						font_thin_black.center_put( bx+BUTTON2_X1, by+BUTTON2_Y1, bx+BUTTON2_X2, by+BUTTON2_Y2, text_game_menu.str_load() ); // "Load" );
					}
					font_thin_black.center_put( bx+BUTTON4_X1, by+BUTTON4_Y1, bx+BUTTON4_X2, by+BUTTON4_Y2, text_game_menu.str_cancel() ); // "Cancel" );
					font_thin_black.center_put( bx+BUTTON7_X1, by+BUTTON7_Y1, bx+BUTTON7_X2, by+BUTTON7_Y2, text_game_menu.str_delete_save() ); // "Delete" );
				}
				else
				{
					startButton.paint();
					if( action_mode == 1 )
						saveNewButton.paint();
					cancelButton.paint();
					delButton.paint();
				}

				// capture area

				for( i = 0; i < MAX_BROWSE_DISP_REC; ++i )
				{
					browseArea[i].clear();
					browseArea[i].resize(SLOT_X1(i), SLOT_Y1(i), SLOT_WIDTH, SLOT_HEIGHT );
					vga.active_buf->read_bitmapW( SLOT_X1(i), SLOT_Y1(i), SLOT_X2(i), SLOT_Y2(i), browseArea[i].bitmap_ptr() );
				}

				// capture page up area
				pageUpArea.clear();
				pageUpArea.resize( pageUpX1, pageUpY1, ((Bitmap *)arrowBitmap)->get_width(), ((Bitmap *)arrowBitmap)->get_height() );
				vga.active_buf->read_bitmapW( pageUpX1, pageUpY1, pageUpX2, pageUpY2, pageUpArea.bitmap_ptr() );
				pageDownArea.clear();
				pageDownArea.resize( pageDownX1, pageDownY1, ((Bitmap *)arrowBitmap)->get_width(), ((Bitmap *)arrowBitmap)->get_height() );
				vga.active_buf->read_bitmapW( pageDownX1, pageDownY1, pageDownX2, pageDownY2, pageDownArea.bitmap_ptr() );

				// capture page no area
				pageNoArea.clear();
				pageNoArea.resize( pageNoX1, pageNoY1, pageNoX2-pageNoX1+1, pageNoY2-pageNoY1+1 );
				vga.active_buf->read_bitmapW( pageNoX1, pageNoY1, pageNoX2, pageNoY2, pageNoArea.bitmap_ptr() );

				// ------ show cursor ----//

				mouse.show();
			}

			for( i = 0; i < MAX_BROWSE_DISP_REC; ++i )
			{
				if( refreshFlag & LSOPTION_SLOT(i) )
				{
					int browseSlotX1 = SLOT_X1(i);
					int browseSlotY1 = SLOT_Y1(i);
					int browseSlotX2 = SLOT_X2(i);
					int browseSlotY2 = SLOT_Y2(i);

					mouse.hide_area( browseSlotX1, browseSlotY1, browseSlotX2, browseSlotY2 );

					// draw save bitmap area
					vga_front.put_bitmapW_fast( browseSlotX1, browseSlotY1, browseArea[i].bitmap_ptr() );

					// draw slot content
					int rec = page * MAX_BROWSE_DISP_REC + i + minRecno;
					if( rec == 0 )
					{
						font_bold_black.center_put( browseSlotX1, browseSlotY1, browseSlotX2, browseSlotY2, text_game_menu.str_empty_game_slot() ); // "Empty Game Slot" );
					}
					else if( rec >= 1 && rec <= size() )
					{
						// remain game_file_array[rec]->disp_info() for file find
						operator[](rec)->disp_info( browseSlotX1, browseSlotY1 );		
					}

					// draw selected frame

					if( rec == browse_recno )
					{
						// draw black frame
						vga_front.rect( browseSlotX1, browseSlotY1, browseSlotX2, browseSlotY2, 2, V_BLACK );
					}

					mouse.show_area();
				}
			}

			if( refreshFlag & LSOPTION_PAGE_NO )
			{
				mouse.hide_area( pageNoX1, pageNoY1, pageNoX2, pageNoY2 );
//				String str;
//				str = "page ";
//				str += page+1;
//				str += "/";
//				str += maxPage;
				vga.active_buf->put_bitmapW( pageNoX1, pageNoY1, pageNoArea.bitmap_ptr() );		
				font_snds.center_put( pageNoX1, pageNoY1, pageNoX2, pageNoY2, text_game_menu.str_page_str(page+1, maxPage) );
				mouse.show_area();

				mouse.hide_area( pageUpX1, pageUpY1, pageUpX2, pageUpY2 );
				if( page > 0 )
					vga.active_buf->put_bitmap_trans_decompress_hmirror( pageUpX1, pageUpY1, arrowBitmap );
				else
					vga.active_buf->put_bitmapW( pageUpX1, pageUpY1, pageUpArea.bitmap_ptr() );
				mouse.show_area();

				mouse.hide_area( pageDownX1, pageDownY1, pageDownX2, pageDownY2 );
				if( page < maxPage-1 )
					vga.active_buf->put_bitmap_trans_decompress( pageDownX1, pageDownY1, arrowBitmap );
				else
					vga.active_buf->put_bitmapW( pageDownX1, pageDownY1, pageDownArea.bitmap_ptr() );
				mouse.show_area();
			}

			refreshFlag = 0;
		}

		sys.blt_virtual_buf();

		// ------ detect slots -------//

		int breakWhileFlag = 0;

		for( i = 0; i < MAX_BROWSE_DISP_REC; ++i )
		{
			int rec = page * MAX_BROWSE_DISP_REC + i + minRecno;
			int browseSlotX1 = SLOT_X1(i);
			int browseSlotY1 = SLOT_Y1(i);
			int browseSlotX2 = SLOT_X2(i);
			int browseSlotY2 = SLOT_Y2(i);

			if( rec < minRecno || rec > size() )
				continue;

			if( mouse.double_click(browseSlotX1, browseSlotY1, browseSlotX2, browseSlotY2) )
			{
				// double click on game slot
				if( browse_recno == rec )
				{
					browse_recno = rec;
					refreshFlag |= LSOPTION_SLOT(i);
					if( recno )
						*recno = browse_recno;
					breakWhileFlag = 1;	// signal to break while(1) loop
					retFlag = process_action(0);

//					if( retFlag < 0 )
//						box.msg("Error");
					break;		// BUGHERE : how to break while loop
				}
			}
			else if( mouse.single_click(browseSlotX1, browseSlotY1, browseSlotX2, browseSlotY2) )
			{
				// click on game slot
				if( browse_recno != rec )
				{
					// refresh old slot
					int oldSlot = browse_recno - page * MAX_BROWSE_DISP_REC - minRecno;
					if( oldSlot >= 0 && oldSlot < MAX_BROWSE_DISP_REC )
						refreshFlag |= LSOPTION_SLOT(oldSlot);

					// refresh new slot
					browse_recno = rec;
					refreshFlag |= LSOPTION_SLOT(i);
					break;
				}
			}
		}

		// ------- detect scroll button -------//

		if( page > 0 && mouse.any_click( bx+LSCROLL_X1, by+LSCROLL_Y1, bx+LSCROLL_X2, by+LSCROLL_Y2 ) )
		{
			page--;
			refreshFlag |= LSOPTION_ALL_SLOTS | LSOPTION_SCROLL | LSOPTION_PAGE_NO;
		}

		if( page < maxPage-1 && mouse.any_click( bx+RSCROLL_X1, by+RSCROLL_Y1, bx+RSCROLL_X2, by+RSCROLL_Y2 ) )
		{
			page++;
			refreshFlag |= LSOPTION_ALL_SLOTS | LSOPTION_SCROLL | LSOPTION_PAGE_NO;
		}

		// -------- detect button at bottom -------//

		if( (pre_game ? mouse.single_click( bx+BUTTON4_X1, by+BUTTON4_Y1, bx+BUTTON4_X2, by+BUTTON4_Y2) : cancelButton.detect())
			|| mouse.key_code == KEY_ESC || mouse.any_click(RIGHT_BUTTON) > 0)		// also when ESC key is pressed or right button
		{
			// cancel button or escape key
			refreshFlag = LSOPTION_ALL;
			retFlag = 0;
			breakWhileFlag = 1;
			break;		// break while(1)
		}
		else if( (action_mode == 1 || (action_mode == 2 && browse_recno))
			&& (pre_game ? mouse.single_click(bx+BUTTON2_X1, by+BUTTON2_Y1, bx+BUTTON2_X2, by+BUTTON2_Y2) : startButton.detect()) )
		{
			// save / load button
			refreshFlag = LSOPTION_ALL;
			if( recno )
				*recno = browse_recno;
			retFlag = process_action(0);
			if( retFlag != 0 )
			{
//				if( retFlag < 0 )
//					box.msg("Error");
				breakWhileFlag = 1;
				break;
			}
		}
		else if( action_mode == 1
			&& (pre_game ? mouse.single_click( bx+BUTTON3_X1, by+BUTTON3_Y1, bx+BUTTON3_X2, by+BUTTON3_Y2) : saveNewButton.detect()) )
		{
			// save new button
			refreshFlag = LSOPTION_ALL;
			retFlag = process_action(1);
			if( retFlag != 0 )
			{
//				if( retFlag < 0 )
//					box.msg("Error");
				breakWhileFlag = 1;
				break;
			}
		}
		else if( browse_recno 
			&& (pre_game ? mouse.single_click( bx+BUTTON7_X1, by+BUTTON7_Y1, bx+BUTTON7_X2, by+BUTTON7_Y2) : delButton.detect()) )
		{
			// delete save game button
			if( browse_recno != 0 )			// cannot del save game slot
			{
				del_game();
				if( browse_recno > size() )
				{
					browse_recno = size();
				}
//				if( browse_top_recno > size()-MAX_BROWSE_DISP_REC+1)
//					browse_top_recno = size()-MAX_BROWSE_DISP_REC+1;
//				if( browse_top_recno < minRecno )
//					browse_top_recno = minRecno;
				// scrollBar.set_view_recno(browse_top_recno);

				// recalculate page
				maxPage = (size()-minRecno+1 + MAX_BROWSE_DISP_REC-1) / MAX_BROWSE_DISP_REC;	// divide but round up
				if( page >= maxPage )
				{
					page = maxPage - 1;
					if( page < 0 )
						page = 0;
				}
				refreshFlag |= LSOPTION_ALL_SLOTS | LSOPTION_SCROLL | LSOPTION_PAGE_NO;

				if( action_mode == 2 && size()==0 )
				{
					box.msg( text_game_menu.str_none_save_game()); // "You haven't saved any games yet." );
					return 0;
				}
			}
			else
			{
				box.msg( text_game_menu.str_cannot_delete_slot()); // "Cannot delete this slot");
			}
		}

		if( breakWhileFlag )
			break;
	}

	if( useBack )
		vga.use_back();

	power.win_opened = 0;
	if( retFlag <= 0 )
	{
		// if load game is successful, no need to deinit resource
		if( deinitGameSet )
			game_set.close_set();
		if( deinitUnitRes )
			unit_res.deinit();
		if( deinitRaceRes )
			race_res.deinit();
		if( deinitMonsterRes )
			monster_res.deinit();
	}

	mouse.reset_click();
	return retFlag;

#undef SLOT_X1
#undef SLOT_Y1
#undef SLOT_X2
#undef SLOT_y2
}
//---------- End of function GameFileArray::menu --------//


//-------- Begin of function GameFileArray::disp_browse --------//
//
// Display saved game info on the browser.
//
void GameFileArray::disp_browse()
{
/*
	int lastRec = MIN(browse_top_recno+MAX_BROWSE_DISP_REC-1, size());

	int x = menu_x1 + BROWSE_X1;
	int y = menu_y1 + BROWSE_Y1;

	for( int i=browse_top_recno ; i<=lastRec ; i++, y+=BROWSE_REC_HEIGHT )
	{
		if( i==0 )
		{
			err_when( action_mode!=1 );
			font_bible.center_put( x, y, x+BROWSE_REC_WIDTH-1, y+BROWSE_REC_HEIGHT-1, "Empty Game Slot" );
		}
		else
		{
			game_file_array[i]->disp_info(x, y);
		}
		if( i == browse_recno )
		{
			vga_front.adjust_brightness(x,y,x+BROWSE_REC_WIDTH-1,y+BROWSE_REC_HEIGHT-1,-2);
			vga_front.put_bitmap_trans_decompress( x, y, image_button.read("LS-DWN"));
		}
	}
*/
}
//--------- End of function GameFileArray::disp_browse --------//


//-------- Begin of function GameFile::disp_info --------//
//
void GameFile::disp_info(int x, int y)
{
//	if( race_id > 0 )
//		vga_front.put_bitmap(x+10, y+10,	unit_res[ race_res[race_id]->infantry_unit_id ]->king_icon_ptr);
//	else
//		vga_front.put_bitmap(x+10, y+10,	unit_res[ monster_res[-race_id]->unit_id ]->king_icon_ptr);
//	x+=60;

	//------ display player color -----//

	nation_array.disp_nation_color( x+6, y+14, nation_color );

	//-------- display king name ------//

//	String str;
//	str  = translate.process("King ");
//	str += player_name;
//	font_bold_black.put( x+24, y+10, str );
	font_bold_black.put( x+24, y+10, text_game_menu.str_king_title(race_id, player_name) );

	//------- display game date --------//

//	str  = translate.process("Game Date: ");
//	str += date.date_str(game_date);
//	font_thin_black.put( x+24, y+30, str );
	int x2;
	x2 = font_thin_black.put( x+24, y+30, text_game_menu.str_game_date() );
	font_thin_black.put( x2, y+30, date.date_str(game_date) );

	// ----- display description ------//

	font_san.put( x+24, y+50, game_desc, 0, x+ SLOT_WIDTH -5 );

	//---------------------------------//

	// str  = translate.process("File Name: ");
	char shortFileName[MAX_PATH+1];
	misc.extract_file_name( shortFileName, file_name );		// get the file name only from a full path string
//	str += shortFileName;
//	font_small.put( x+380, y+12, str );
	x2 = font_small.put( x+380, y+12, text_game_menu.str_file_name() );
	font_small.put( x2, y+12, shortFileName );

	SYSTEMTIME sysTime;
	FILETIME localFileTime;
	FileTimeToLocalFileTime( &file_date, &localFileTime );
	FileTimeToSystemTime( &localFileTime, &sysTime );

//	str  = translate.process("File Date: ");
//	str += date.date_str(date.julian(sysTime.wYear, sysTime.wMonth,sysTime.wDay), 1);
//	str += " ";
//	str += date.time_str( sysTime.wHour * 100 + sysTime.wMinute );
//	font_small.put( x+380, y+32, str );
	x2 = font_small.put( x+380, y+32, text_game_menu.str_file_date() );
	x2 = font_small.put( x2, y+32, date.date_str(date.julian(sysTime.wYear, sysTime.wMonth,sysTime.wDay), 1) );
	x2 = font_small.put( x2+6, y+32, date.time_str( sysTime.wHour * 100 + sysTime.wMinute ) );
}
//--------- End of function GameFile::disp_info --------//


//------- Begin of function GameFileArray::process_action ------//
//
// [int] saveNew - save on a new game file
//                 (default : 0)
//
// return : <int> 1 - process ok
//                0 - process cancelled
//               -1 - save/load failed
//
int GameFileArray::process_action(int saveNew)
{
	//------------ save game --------------//

	if( action_mode == 1 )
	{
		if( saveNew || browse_recno==0 )   // save on empty slot
		{
			// #### begin Gilbert 20/1 ######//
			return save_new_game(NULL);
			// #### end Gilbert 20/1 ######//
		}
		else           // save on existing slot
		{
			if( !box.ask(text_game_menu.str_ask_overwrite()) )// "It will overwrite the existing saved game, proceed ?" ) )
				return 0;

			GameFile* gameFile = game_file_array[browse_recno];

			// ###### begin Gilbert 20/1 ########//
			if( !gameFile->ask_desc() )
				return 0;
			// ###### end Gilbert 20/1 ########//

			if( !gameFile->save_game(save_default_dir, NULL, NULL))
				return -1;

			strcpy( last_file_name, gameFile->file_name );
		}

		return 1;
	}

	//----------- load game -------------//

	else
	{
		GameFile* gameFile = game_file_array[browse_recno];

		// check gameFile->campaign_id. Campaign save game can be loaded
		// if and only if in campaign mode

		if( !sys.debug_session && gameFile->scenario_cheat_flag < sys.scenario_cheat_flag )
		{
			box.msg( text_game_menu.str_editor_cant_load() );// "Cannot load in scenario editor" );
			return 0;
		}

		// ###### begin Gilbert 9/4 ########//

		if( !pre_game && (game.is_campaign_mode() || gameFile->campaign_id) )
		{
			game.set_load_game_in_main_menu(gameFile->file_name);
			sys.signal_exit_flag = 2;		// quit to main menu and it will load game automatically
			return 1;
		}
		// ###### end Gilbert 9/4 ########//

		int rc = gameFile->load_game(save_default_dir, NULL);
		if( rc > 0 )
			strcpy( last_file_name, gameFile->file_name );
		return rc;
	}

	return 0;
}
//--------- End of function GameFileArray::process_action ------//


//-------- Begin of function GameFileArray::save_new_game -----//
//
// Save current game to a new saved game file immediately without
// prompting menu.
//
// Called by GameFileArray::process_action() and error handler.
//
// <char*> fileName - file name of the saved game
//
// return 1 - success
//        0 - cancel
//       -1 - fail
//
// ###### begin Gilbert 20/1 ########//
int GameFileArray::save_new_game(const char* fileName)
{
	GameFile  gameFile;
	GameFile* gameFilePtr;
	int       addFlag=1;
	int       gameFileRecno;

	memset( &gameFile, 0, sizeof(GameFile) );

	if( fileName )		// has a filename before extension
	{
		load_all_game_header( save_default_dir, save_default_ext );

		// ### begin Gilbert 26/5 #####//
		// fileName = player_profile.save_game_path(fileName);
		// ### end Gilbert 26/5 #####//

		if( save_default_dir[0] != '\0' )
		{
			strcpy( gameFile.file_name, save_default_dir );
			strcat( gameFile.file_name, PATH_DELIM );
			strcat( gameFile.file_name, fileName );
		}
		else
		{
			strcpy( gameFile.file_name, fileName );
		}

		// if fileName without extension append

		if( !strchr(fileName, '.' ) )
		{
			strcat(gameFile.file_name, strchr( save_default_ext, '.') );		// remove '*' of "*.SAV"
		}

		//----- check for overwriting an existing file ----//

		for( gameFileRecno=1 ; gameFileRecno<=game_file_array.size() ; gameFileRecno++ )
		{
			gameFilePtr = game_file_array[gameFileRecno];

			// ###### begin Gilbert 31/10 ######//
			if( stricmp(gameFilePtr->file_name, gameFile.file_name)==0 )      // if this file name already exist
			// ###### end Gilbert 31/10 ######//
			{
				addFlag=0;
				break;
			}
		}
		// strcpy( gameFile.file_name, fileName );		// now use gameFile.file_name instead
   }
	else
   {
		load_all_game_header( save_default_dir, save_default_ext );

		// find the extension begin with '.'
		err_when( !strchr(save_default_ext, '.') );
      gameFile.set_file_name(save_default_dir, strchr(save_default_ext, '.') );        // give it a new game_file_name based on current group name
   }

	// --------- ask description -------//

	if( !gameFile.ask_desc() )
		return 0;

	//----------- save game now ------------//

	if( gameFile.save_game(save_default_dir, NULL, NULL) )		// use gameFile.file_name
	{
		strcpy( last_file_name, gameFile.file_name );

		if( addFlag )
		{
         linkin(&gameFile);

			quick_sort( sort_game_file_function );
      }
		else
      {
			// #### begin Gilbert 31/10 ######//
         update(&gameFile, gameFileRecno);
			// #### end Gilbert 31/10 ######//
      }

		return 1;
   }

	return -1;

	// ###### end Gilbert 20/1 ########//
}
//-------- End of function GameFileArray::save_new_game -------//


//------- Begin of function GameFileArray::del_game ------//

void GameFileArray::del_game()
{
	int recNo = browse_recno;

//	if( !box.ask( "This saved game will be deleted, proceed ?" ) )
	if( !box.ask( text_game_menu.str_ask_delete_save()) )
		return;

	//---------------------------------------------------//

	remove(game_file_array[recNo]->file_name);

	go(recNo);
	linkout();

	go(recNo-1);    // skip back one record
}
//--------- End of function GameFileArray::del_game ------//


//------- Begin of function GameFileArray::write_hall_of_fame ------//
//
int GameFileArray::write_hall_of_fame()
{
	char full_path[MAX_PATH+1];
	int  rc;
	File file;

	if (!misc.path_cat(full_path, sys.dir_config, HALL_OF_FAME_FILE_NAME, MAX_PATH))
	{
		ERR("Path to the hall of fame too long.\n");
		return 0;
	}

	rc = file.file_create( full_path, 0, 1 );  // 0=don't handle error itself

	if( !rc )
		return 0;
																			// 1=allow the writing size and the read size to be different
	//--------- Write Hall of Fame ----------//

	if( rc )
		rc = (file.file_write( hall_fame_array[0], sizeof(HallFame) * HALL_FAME_NUM ) &&
				file.file_write( hall_fame_array[1], sizeof(HallFame) * HALL_FAME_NUM ));

	//------ write last saved game file name ------//

	if( rc )
		rc = file.file_write( last_file_name, MAX_PATH+1 );

	file.file_close();
	
	return rc;
}
//--------- End of function GameFileArray::write_hall_of_fame ------//


//------- Begin of function GameFileArray::read_hall_of_fame ------//
//
int GameFileArray::read_hall_of_fame()
{
	char full_path[MAX_PATH+1];
	int  rc;
	File file;

	if (!misc.path_cat(full_path, sys.dir_config, HALL_OF_FAME_FILE_NAME, MAX_PATH))
	{
		ERR("Path to the hall of fame too long.\n");
		return 0;
	}

	if( !misc.is_file_exist(full_path) )
		return 0;

	rc = file.file_open( full_path, 0, 1 );   // 0=don't handle error itself
                                                        // 1=allow the writing size and the read size to be different
	if( !rc )
		return 0;
																			// 1=allow the writing size and the read size to be different
	//--------- Read Hall of Fame ----------//

	if( rc )
		rc = (file.file_read( hall_fame_array[0], sizeof(HallFame) * HALL_FAME_NUM ) &&
				file.file_read( hall_fame_array[1], sizeof(HallFame) * HALL_FAME_NUM ));

	//------ read last saved game file name ------//

	if( rc )
		rc = file.file_read( last_file_name, MAX_PATH+1);

	file.file_close();

	return rc;
}
//--------- End of function GameFileArray::read_hall_of_fame ------//


//-------- Begin of function GameFileArray::load_all_game_header --------//
//
// Load all headers of all saved game files in current directory.
//
void GameFileArray::load_all_game_header(const char *path, const char *extStr)
{
	int       i;
	Directory gameDir;
	GameFile  gameFile;
	File      file;

	// ##### begin Gilbert 26/5 ######//
	String str;
	if( path && path[0] != '\0' )
	{
		str = path;
		str += PATH_DELIM;
		str += extStr;
	}
	else
	{
		str = extStr;
	}

	// gameDir.read( player_profile.save_game_path(extStr), 1 );  // 1-Sort file names
	gameDir.read( str, 1 );  // 1-Sort file names

	//-------- read in the headers of all game sets -------//

	zap();

	for( i=1 ; i<=gameDir.size() ; i++ )
	{
		// Directory store file name without path
		if( path && path[0] != '\0' )
		{
			str = path;
			str += PATH_DELIM;
			str += gameDir[i]->name;
		}
		else
		{
			str = gameDir[i]->name;
		}

		if( file.file_open( str, 1, 1 )      // last 1=allow varying read & write size
			&& file.file_read(&gameFile, sizeof(GameFile))
			&& gameFile.validate_header() )
		{
			strcpy( gameFile.file_name, file.file_name );  // in case that the name may be different
#ifndef NO_WINDOWS
			// Don't reset gameFile.file_date on Linux because
			// gameDir[i]->time is not set correctly.
			gameFile.file_date = gameDir[i]->time;
#endif
	      linkin(&gameFile);
		}
		file.file_close();
	}
	// ##### end Gilbert 26/5 ######//
}
//------ End of function GameFileArray::load_all_game_header --------//


// ----- begin of function GameFile::ask_desc -------//
//
// ask description
//
// return 1 if accept and set the GameFile::game_desc
//
int GameFile::ask_desc()
{
	char gameDesc[SAVE_GAME_DESC_LEN+1];
	strcpy( gameDesc, game_desc );

	Blob2DW saveArea;
	Blob2DW keyFieldArea;

	int boxWidth = 700;
	int boxHeight = 150;
	int boxX1 = (VGA_WIDTH - boxWidth) / 2;
	int boxX2 = boxX1 + boxWidth - 1;
	int boxY1 = (VGA_HEIGHT - boxHeight) / 2;
	int boxY2 = boxY1 + boxHeight - 1;

	char useBack = vga.use_back_buf;
	vga.use_front();

	// ------ capture save area -------//

	mouse.hide_area( boxX1, boxY1, boxX2, boxY2 );
	saveArea.clear();
	saveArea.resize( boxX1, boxY1, boxWidth, boxHeight );
	vga_front.read_bitmapW( boxX1, boxY1, boxX2, boxY2, saveArea.bitmap_ptr() );
	mouse.show_area();

	// ------ init get field -------//

	GetA descTextBox;
	descTextBox.init( boxX1+16, boxY1+50, boxX2-12, gameDesc, SAVE_GAME_DESC_LEN, &font_san, 0, 0 );

	keyFieldArea.resize( descTextBox.x, descTextBox.y, 
		descTextBox.x_limit-descTextBox.x+1, descTextBox.font_ptr->max_font_height );

	// ------ ok cancel button --------//

	const int buttonWidth = 100;
	const int buttonHeight = 24;
	Button button_ok, button_cancel;

	int x = (2*boxX1 + boxX2) / 3 - buttonWidth / 2;
	int y = boxY2 - buttonHeight - 10;
	button_ok.create_text( x, y, x+buttonWidth-1, y+buttonHeight-1, text_basic.str_box_ok() );
	x = (boxX1 + 2*boxX2) / 3 - buttonWidth / 2;
	button_cancel.create_text( x, y, x+buttonWidth-1, y+buttonHeight-1, text_basic.str_box_cancel() );

	// --------------------------------//

	int retFlag = 0;
	int refreshFlag = 1;

	while(1)
	{
		sys.yield();
		mouse.get_event();

		if( refreshFlag )
		{
			// ------ display message Box ------//

			vga_front.d3_panel_up( boxX1, boxY1, boxX2, boxY2 );
			vga_front.d3_panel_down( boxX1+4, boxY1+4, boxX2-4, boxY2-4 );

			// ------ display box title ------//

			font_bold_black.put( boxX1+16, boxY1+20, text_game_menu.str_ask_desc_save() ); // "Description of save game");

			// ------ set get field background -------//

			mouse.hide_area( boxX1, boxY1, boxX2, boxY2 );
			vga_front.read_bitmapW( keyFieldArea.left_edge, keyFieldArea.top_edge, 
				keyFieldArea.left_edge+keyFieldArea.width-1, keyFieldArea.top_edge+keyFieldArea.height-1,
				keyFieldArea.bitmap_ptr() );
			descTextBox.set_background( keyFieldArea.ptr );
			mouse.show_area();

			descTextBox.paint(1);

			// ------- paint button --------//

			button_ok.paint();
			button_cancel.paint();

			// --------------------------------//

			refreshFlag = 0;
		}

		sys.blt_virtual_buf();

		// ------- detect --------//

		if( descTextBox.detect() )
		{
			descTextBox.paint(1);
		}

		if( button_ok.detect(KEY_RETURN) )
		{
			retFlag = 1;
			break;
		}

		if( button_cancel.detect(KEY_ESC) )
		{
			retFlag = 0;
			break;
		}
	}

	// ------ restore save buffer ---------//

	vga_front.put_bitmapW( boxX1, boxY1, saveArea.bitmap_ptr() );

	// ------ update game_desc ----------//

	if( retFlag )
	{
		strcpy( game_desc, gameDesc );
	}

	if(useBack)
		vga.use_back();

#ifndef NO_REAL_TIME_UPDATE
	sys.blt_virtual_buf();
#endif

	return retFlag;
}
// ----- end of function GameFile::ask_desc -------//


//------ Begin of function sort_game_file_function ------//
//
static int sort_game_file_function( const void *a, const void *b )
{
	return strcmp( ((GameFile*)a)->file_name, ((GameFile*)b)->file_name );
}
//------- End of function sort_game_file_function ------//


//------ Begin of function last_game_recno ------//
//
static int last_game_recno()
{
	int i;

	for( i=game_file_array.size() ; i>0 ; i-- )
	{
		if( strcmp(game_file_array[i]->file_name, game_file_array.last_file_name)==0 )
			return i;
	}

	return 0;    // if none of them match, return 1 as the first record
}
//------- End of function last_game_recno ------//


//------- Begin of function GameFileArray::operator[] -----//

GameFile* GameFileArray::operator[](int recNo)
{
	GameFile* gameFile = (GameFile*) get(recNo);

	return gameFile;
}
//--------- End of function GameFileArray::operator[] ----//


//------- Begin of static function key_search --------//

static void key_search()
{
	int i;
	int searchKey = misc.upper(mouse.key_code);

	if( searchKey < '0' || searchKey > 'Z' )
		return;

	for( i=1 ; i<=game_file_array.size() ; i++ )
	{
		if( misc.upper(game_file_array[i]->file_name[0]) >= searchKey )
		{
			browse_recno = i+(action_mode==1);
			return;
		}
	}
}
//------- End of static function key_search --------//


//------- Begin of static function disp_scroll_bar_func --------//
static void disp_scroll_bar_func(SlideVBar *scroll, int)
{
	short rectTop = scroll->rect_top();
	short rectBottom = scroll->rect_bottom();

	vga_front.bar( scroll->scrn_x1, rectTop, scroll->scrn_x2, rectBottom, VGA_YELLOW+1);
	if( rectBottom - rectTop > 6 )
	{
		vga_front.d3_panel_up(scroll->scrn_x1, rectTop, scroll->scrn_x2, rectBottom,2,0);
	}
}
//------- End of static function disp_scroll_bar_func --------//


// ------ begin of function GameFileArray::auto_save ---------//
//
// save <path>\AUTO.xxx , but rename old auto.xxx to auto2.xxx
// if auto2.xxx exists, delete it before rename
//
// return 1 - success
//        0 - fail
//
int GameFileArray::auto_save()
{
	char *extStr = strchr( save_default_ext, '.' );
	err_when( !extStr );

	// prepare new auto save game name

	String str1(save_default_dir);
	if( save_default_dir[0] )
		str1 += PATH_DELIM;
	str1 += "AUTO";
	str1 += strchr( save_default_ext, '.' );

	// prepare backup auto save game name

	String str2(save_default_dir);
	if( save_default_dir[0] )
		str2 += PATH_DELIM;
	str2 += "AUTO2";
	str2 += strchr( save_default_ext, '.' );

      //--- rename the existing AUTO.xxx to AUTO2.xxx and save a new game ---//

   if( misc.is_file_exist( str1 ) )
   {
      if( misc.is_file_exist( str2 ) )      // if there is already an AUTO2.SVM, delete it
         remove( str2 );

      rename( str1, str2 );
   }

	return game_file.save_game( NULL, str1, "" );
}
// ------ end of function GameFileArray::auto_save ---------//
