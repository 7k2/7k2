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

//Filename    : OMOUSECR.H
//Description : Header file of Object Cursor resource

#ifndef __OMOUSECR_H
#define __OMOUSECR_H


// #include <OVGA.H>

#include <ORESDB.H>
#include <OBLOB2W.H>


// ------------ Define constant -------- // 

#define MAX_VGA_WIDTH 1024
#define MAX_VGA_HEIGHT 768


//------------ Define cursor id. ----------//

enum { CURSOR_CUSTOM = -1,
		 CURSOR_NORMAL =1,      // nothing selected, 
       CURSOR_NORMAL_C,			// nothing selected, pointing a friend object
		 CURSOR_NORMAL_O,			// nothing selected, pointing an enemy object
		 CURSOR_NORMAL_A,			// nothing selected, pointing an allied object

		 CURSOR_UNIT,				// selected a friend unit/town/firm  (walk)
		 CURSOR_UNIT_C,			// selected a friend unit, pointing a friend object
		 CURSOR_UNIT_O,			// selected a friend unit, pointing an enemy object (attack!)
		 CURSOR_UNIT_A,			// selected a friend unit, pointing an allied object (attack!)

		 CURSOR_WAITING,
		 CURSOR_BUILD,
		 CURSOR_DESTRUCT,
		 CURSOR_ASSIGN,
		 CURSOR_SET_STOP,
		 CURSOR_CANT_SET_STOP,
		 CURSOR_SHIP_STOP,
		 CURSOR_CANT_SHIP_STOP,
		 CURSOR_BURN,
		 CURSOR_SETTLE,
		 CURSOR_SETTLE_1,			// settle cursor for different color_scheme_id
		 CURSOR_SETTLE_2,
		 CURSOR_SETTLE_3,
		 CURSOR_SETTLE_4,
		 CURSOR_SETTLE_5,
		 CURSOR_SETTLE_6,
		 CURSOR_SETTLE_7,
		 CURSOR_ON_LINK,			// when pointing town/firm link
		 CURSOR_TRIGGER_EXPLODE,	// special of CURSOR_UNIT_C, when pointing at an explosive cart
		 CURSOR_CAPTURE_FIRM,
     };

enum { STRETCH_NW, STRETCH_NE, STRETCH_SW, STRECH_SE };	// mouse_stretch_dir


//------------ Define struct CursorRec ---------------//

struct CursorRec
{
	enum { HOT_SPOT_LEN=3, FILE_NAME_LEN=8, BITMAP_PTR_LEN=4 };

	char human_file_name[FILE_NAME_LEN];
	char human_hot_spot_x[HOT_SPOT_LEN];
	char human_hot_spot_y[HOT_SPOT_LEN];
	char human_bitmap_ptr[BITMAP_PTR_LEN];

	char monster_file_name[FILE_NAME_LEN];
	char monster_hot_spot_x[HOT_SPOT_LEN];
	char monster_hot_spot_y[HOT_SPOT_LEN];
	char monster_bitmap_ptr[BITMAP_PTR_LEN];
};

//------------- Define struct CursorInfo --------------//

struct CursorInfo
{
	short human_hot_spot_x;
	short human_hot_spot_y;
	char* human_bitmap_ptr;
	short monster_hot_spot_x;
	short monster_hot_spot_y;
	char* monster_bitmap_ptr;

};


//--------- Define class MouseCursor --------//

class Blob2DW;
class VgaBuf;

class MouseCursor
{
public:
	char  init_flag;
	char  hide_all_flag;
	char  hide_area_flag;

	short	cur_icon;

	int   hide_x1, hide_y1, hide_x2, hide_y2;
	int   cur_x1, cur_y1, cur_x2, cur_y2;

	int	hot_spot_x, hot_spot_y;

	int   icon_width;
	int   icon_height;

	char  cursor_shown;     // whether the cursor has been shown on the screen
	char  processing_flag;  // whether process() is being run or not, prevent nested call by interrupt
	char  wait_mode_count;  // allow nested wait cursor

	char 	*icon_ptr, *icon_data_ptr;
	Blob2DW *save_scr, *save_back_scr, *merge_buf;

	// ------ custom cursor setting ------- //

	char	*custom_icon_ptr;
	int	custom_hot_spot_x, custom_hot_spot_y;

	//------- nation selection frame --------//

	char  frame_flag;			// whether nation selection frame is on
	char  frame_shown;

	int	frame_x1, frame_y1, frame_x2, frame_y2;
	int	frame_origin_x, frame_origin_y;
	int	frame_border_x1, frame_border_y1, frame_border_x2, frame_border_y2;

	short	frame_top_save_scr[MAX_VGA_WIDTH+2];		// for saving the nation selection frame
	short	frame_bottom_save_scr[MAX_VGA_WIDTH+2];	// +4 is for the width & height info
	short	frame_left_save_scr[MAX_VGA_HEIGHT+2];
	short	frame_right_save_scr[MAX_VGA_HEIGHT+2];

	//------- cursor bitmap resource --------//

private:
	short    	 cursor_count;
	CursorInfo*  cursor_info_array;

	ResourceDb	 human_res_bitmap;
	ResourceDb	 monster_res_bitmap;

public:
	MouseCursor();
	~MouseCursor();

	void 	init();
	void 	deinit();

	void 	set_icon(int);
	void	define_custom_icon( char *, int hotX, int hotY );
	void  set_frame(char,char=0);
	void  set_frame_border(int,int,int,int);

	void 	process(int,int);
	void	disp_back_buf(int,int,int,int);
	void	before_flip();
	void	after_flip();

	int	get_icon()				{ return cur_icon; }
	void	restore_icon(int);

private:
	void	process_frame(int,int);
	void 	disp_frame(VgaBuf*);
	int	is_touch(int,int,int,int);

	void  load_cursor_info();
};

extern MouseCursor mouse_cursor;

//----------------------------------------------//

#endif

