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

// Filename    : OTIPS.H
// Description : tips resource


#include <otips.h>
#include <all.h>
#include <ofiletxt.h>
#include <ovqueue.h>
#include <ovga.h>
#include <vga_util.h>
#include <ofont.h>
#include <oimgres.h>
#include <omouse.h>
#include <oprofile.h>
#include <oconfig.h>
#include <ot_gmenu.h>


#define TIPS_FILE_NAME "TIPS.RES"
#define MAX_TIPS_LEN 2000

#define TIPS_TEXT_WIDTH 350
#define TIPS_TEXT_MIN_HEIGHT 66
#define TIPS_BORDER_WIDTH 8
#define TIPS_BORDER_HEIGHT 6
#define TIPS_BUTTON_WIDTH 150
#define TIPS_BUTTON_HEIGHT 22

#define TICK_WIDTH 20
#define TICK_HEIGHT 20


static int tips_box_x = -1;
static int tips_box_y;

TipsRes::TipsRes()
{
	memset( this, 0, sizeof(TipsRes) );
}

TipsRes::~TipsRes()
{
	deinit();
}


void TipsRes::init()
{
	deinit();

	load_info();

	init_flag = 1;
	current_tips = 0;
	box_active = 1;		// = config.tips_active
}


void TipsRes::deinit()
{
	if( init_flag )
	{
		if( tips_text )
		{
			mem_del( tips_text);
			tips_text = NULL;
		}

		if( tips_info_array )
		{
			mem_del( tips_info_array );
			tips_info_array = NULL;
		}

		if( tips_class_array )
		{
			mem_del( tips_class_array );
			tips_class_array = NULL;
		}

		init_flag = 0;
	}
}


void TipsRes::load_info()
{
	String str;
	str = DIR_RES;
	str += TIPS_FILE_NAME;

	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( str );

	int dataSize = fileTxt.file_size();

	//-------- read in help info one by one --------//

//	TipsInfo* iptr    = tips_info_array;
//	char*     textPtr = tips_text;
	int       readLen, totalReadLen=0;    // total length of text read into the help_text_buf
	int		 loopCount=0;
	char*		 tokenStr;

	char		 textPtr[MAX_TIPS_LEN+1];
	VLenQueue tempText;
	VLenQueue tempTipsInfo;		// resizable array
	VLenQueue tempTipsClass;
	tips_info_count=0;
	tips_class_count=0;

	//first_help_by_help_code = first_help_by_area = -1;
	//last_help_by_help_code = last_help_by_area = 0;

	int classFirstRec = 0;
	int classRecCount = 0;

	while( !fileTxt.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		if( tokenStr[0] == '@' )		// start of a class
		{
			if( tips_class_count > 0 )		// flush last class
			{
				TipsClass *tipsClassPtr = (TipsClass *)tempTipsClass.reserve(sizeof(TipsClass));
				memset( tipsClassPtr, 0, sizeof(TipsClass) );
				tipsClassPtr->tips_class = tips_class_count;
				tipsClassPtr->first_tips = classFirstRec;
				tipsClassPtr->tips_count = classRecCount;
				tipsClassPtr->is_active  = 0;
			}

			++tips_class_count;
			classFirstRec = 0;		// don't know
			classRecCount = 0;

			//---------- next line -----------//

			fileTxt.next_line();

			if( fileTxt.is_eof() )
				break;
		}
		else
		{
			err_when( tips_class_count == 0 );		// first line must be begin with @

			++tips_info_count;
			if( !classFirstRec )
				classFirstRec = tips_info_count;		// set the first record
			++classRecCount;

			// get tips description
			// and append the text

			readLen = fileTxt.read_paragraph(textPtr, MAX_TIPS_LEN);
			char *t = tempText.reserve(readLen+1);		// one more for null char
			strncpy( t, textPtr, readLen);
			t[readLen] = '\0';

			// -----------------------//
			// set tips info
			// ------------------------//

			TipsInfo *tipsInfo = (TipsInfo *)tempTipsInfo.reserve(sizeof(TipsInfo));
			tipsInfo->text_start_offset = t - tempText.queue_buf;
			tipsInfo->tips_class = tips_class_count;
			tipsInfo->line_count = 0;		// unknown

			//---------- next line -----------//

			fileTxt.next_line();		// needed?

			if( fileTxt.is_eof() )
				break;
		}
	}

	// flush class tips class

	if( classRecCount > 0 )
	{
		TipsClass *tipsClassPtr = (TipsClass *)tempTipsClass.reserve(sizeof(TipsClass));
		memset( tipsClassPtr, 0, sizeof(TipsClass) );
		tipsClassPtr->tips_class = tips_class_count;
		tipsClassPtr->first_tips = classFirstRec;
		tipsClassPtr->tips_count = classRecCount;
		tipsClassPtr->is_active  = 0;
	}

	// copy tempText to tips_text

	tips_text = (char *)mem_add( tempText.length() );
	memcpy( tips_text, tempText.queue_buf, tempText.length() );

	// copy tempTipsInfo to tips_info_array; tempTipsClass to tips_info_class

	tips_info_array = (TipsInfo *)mem_add( tempTipsInfo.length() );
	memcpy( tips_info_array, tempTipsInfo.queue_buf, tempTipsInfo.length() );
	err_when( (unsigned) tempTipsInfo.length() != tips_info_count * sizeof(TipsInfo) );		// assume 1 byte alignment

	tips_class_array = (TipsClass *)mem_add( tempTipsClass.length() );
	memcpy( tips_class_array, tempTipsClass.queue_buf, tempTipsClass.length() );
	err_when( (unsigned) tempTipsClass.length() != tips_class_count * sizeof(TipsClass) );	// assume 1 byte alignment
}


TipsClass *TipsRes::get_class(int classId)
{
	err_when( classId < 0 || classId > tips_class_count );

	return tips_class_array + classId - 1;
}


void TipsRes::deactive_all_classes()
{
	for( int c = 0; c < tips_class_count; ++c )
	{
		tips_class_array[c].is_active = 0;
	}
}


TipsInfo *TipsRes::operator[](int tipsId)
{
	err_when( tipsId < 0 || tipsId > tips_info_count );

	return tips_info_array + tipsId - 1;
}


char *TipsRes::get_text(int tipsId)
{
	TipsInfo *tipsInfo = operator[](tipsId);

	return tips_text + tipsInfo->text_start_offset; 
}


void TipsRes::random_select()
{
	// select from active class

	int c;
	int count = 0;
	for( c = 1 ; c <= tips_class_count; ++c )
	{
		if( get_class(c)->is_active )
		{
			count += get_class(c)->tips_count;
		}
	}

	current_tips = 0;

	if( count > 0 )
	{
		// int chosen = misc.random(count) + 1;		// change random seed cuase bug in game.init
		int chosen = (misc.get_time() >> 2) % count + 1;
		int count = 0;
		current_tips = 0;
		for( c = 1 ; c <= tips_class_count; ++c )
		{
			if( get_class(c)->is_active )
			{
				count += get_class(c)->tips_count;

				if( count >= chosen)
				{
					TipsClass *tipsClass = get_class(c);

					// BUGHERE : not so sure
					current_tips = tipsClass->first_tips + tipsClass->tips_count - 1 + chosen - count;
					break;
				}
			}
		}
	}

}


void TipsRes::next()
{
	if( !current_tips )
		return;

	int oldCurrent = current_tips;
	int currentClass = operator[](oldCurrent)->tips_class;
	// TipsClass *tipsClass = get_class(currentClass);

	// BUGHERE : behave not correct when current class is not active

	// jump to next class, see if the class is active
	if( ++current_tips >= get_class(currentClass)->first_tips + get_class(currentClass)->tips_count )
	{
		// find next active class

		int cc;
		for( cc = tips_class_count; cc > 0; --cc )
		{
			if( ++currentClass > tips_class_count )
				currentClass = 1;

			if( get_class(currentClass)->is_active )
				break;
		}

		if( cc <= 0 )		// not found
		{
			// restore to old
			current_tips = oldCurrent;
		}
		else
		{
			current_tips = get_class(currentClass)->first_tips;
			err_when( current_tips < 1 || current_tips > tips_info_count );
		}
	}
}


void TipsRes::prev()
{
	if( !current_tips )
		return;

	int oldCurrent = current_tips;
	int currentClass = operator[](oldCurrent)->tips_class;
	// TipsClass *tipsClass = get_class(currentClass);

	// BUGHERE : behave not correct when current class is not active

	// jump to next class, see if the class is active
	if( --current_tips < get_class(currentClass)->first_tips )
	{
		// find prev active class

		int cc;
		for( cc = tips_class_count; cc > 0; --cc )
		{
			if( --currentClass <= 0 )
				currentClass = tips_class_count;

			if( get_class(currentClass)->is_active )
				break;
		}

		if( cc <= 0 )		// not found
		{
			// restore to old
			current_tips = oldCurrent;
		}
		else
		{
			current_tips = get_class(currentClass)->first_tips + get_class(currentClass)->tips_count - 1;
			err_when( current_tips < 1 || current_tips > tips_info_count );
		}
	}
}




void TipsRes::disp(int x, int y )
{
	tips_box_x = x;
	tips_box_y = y;

	if( !current_tips || !box_active )
		return;

	// get text width and height

	char *textPtr = get_text(current_tips);
	font_cara_w.text_width( textPtr, -1, 
		TIPS_TEXT_WIDTH - TIPS_BORDER_WIDTH*2 );		// left and right side

	int textHeight = font_cara_w.text_height();
	int boxHeight = TIPS_TEXT_MIN_HEIGHT;
	if( boxHeight < textHeight + TIPS_BORDER_HEIGHT*2 )		// top and bottom border
	{
		boxHeight = textHeight + TIPS_BORDER_HEIGHT*2;
	}

	char backupUseBack = vga.use_back_buf;

	// put panel

	vga.use_back();
	Vga::opaque_flag = config.opaque_report;
	vga_util.d3_panel_down( x, y, x+TIPS_TEXT_WIDTH-1, y+boxHeight-1);

	font_cara_w.put_paragraph( x+TIPS_BORDER_WIDTH, y+TIPS_BORDER_HEIGHT,
		x+TIPS_TEXT_WIDTH-TIPS_BORDER_WIDTH-1, y+boxHeight-TIPS_BORDER_HEIGHT+1,		// reserve higher
		textPtr);

	Vga::opaque_flag = 1;

	// put next tip button

	int xb = x + TIPS_TEXT_WIDTH;
	int yb = y;

	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
		vga_util.d3_panel_down( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	else
		vga_util.d3_panel_up( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	font_bld.center_put( xb, yb-4, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1, 
		text_game_menu.str_tips_next() ); //"Next Tip" );

	// put prev tip button

	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
		vga_util.d3_panel_down( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	else
		vga_util.d3_panel_up( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	font_bld.center_put( xb, yb-4, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1,
		text_game_menu.str_tips_prev() ); // "Previous Tip" );

	// put don't show tips button, put a tick if player_profile.hide_tips is on

	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
		vga_util.d3_panel_down( xb+TICK_WIDTH, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	else
		vga_util.d3_panel_up( xb+TICK_WIDTH, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );

	if( !player_profile.hide_tips )
	{
		vga.active_buf->d3_panel_up( xb, yb, xb+TICK_WIDTH-1, yb+TICK_HEIGHT-1 );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_decompress( xb+4, yb+4, image_button.read("TICK") );
		vga.active_buf->d3_panel_down( xb, yb, xb+TICK_WIDTH-1, yb+TICK_WIDTH-1, 4, 0 );
	}

	font_bld.center_put( xb+TICK_WIDTH, yb-4, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1,
		text_game_menu.str_tips_never() ); // "Don't Show Tips" );

	// put close
	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
		vga_util.d3_panel_down( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	else
		vga_util.d3_panel_up( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1 );
	font_bld.center_put( xb, yb-4, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1,
		text_game_menu.str_tips_close() ); // "Close" );

	Vga::opaque_flag = 0;
	if( !backupUseBack )
		vga.use_front();
}

int TipsRes::detect()
{
	int x = tips_box_x;
	int y = tips_box_y;
	if( x < 0 )		// detect before display
		return 0;

	if( !current_tips || !box_active )
		return 0;

	// detect next tip button

	int xb = x + TIPS_TEXT_WIDTH;
	int yb = y;

	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
	{
		next();
		return 1;
	}

	// detect previous tip button

	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
	{
		prev();
		return 1;
	}

	// put don't show tips button

	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
	{
		// set the player_profile
		player_profile.hide_tips = !player_profile.hide_tips;
		return 1;
	}

	// put close
	yb += TIPS_BUTTON_HEIGHT;
	if( mouse.any_click( xb, yb, xb+TIPS_BUTTON_WIDTH-1, yb+TIPS_BUTTON_HEIGHT-1) )
	{
		// save player profile
#ifdef DEBUG
#else
		player_profile.save();
#endif
		box_active = 0;
		return 1;
	}

	return 0;
}


void TipsClass::set_active()
{
	is_active = 1;
}

