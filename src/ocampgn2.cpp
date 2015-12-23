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

// Filename    : OCAMPGN.H
// Description : Campaign mode menu

#include <stdio.h>
#include <ocampgn.h>
#include <ogame.h>
#include <oinfo.h>
#include <obattle.h>
#include <omouse.h>
#include <osys.h>
#include <ovga.h>
#include <vga_util.h>
#include <ovgabuf.h>
#include <onation.h>
#include <omodeid.h>
#include <oworldmt.h>
#include <ofont.h>
#include <ogameset.h>
#include <ofiletxt.h>
#include <ostate.h>
#include <olonglog.h>
#include <ohelp.h>
#include <ogameset.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <osprtres.h>
#include <osfrmres.h>
#include <omagic.h>
#include <ovgabuf.h>
#include <oimgres.h>
#include <oaudio.h>
#include <oflc.h>
#include <ocoltbl.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <oblob2.h>
#include <oplasma.h>
#include <oc_au.h>
#include <ose.h>
#include <oseres.h>
#include <omusic.h>
#include <ofont.h>
#include <oblob2w.h>
#include <omatrix.h>
#include <omousecr.h>
#include <ot_camp.h>
#include <ot_talk.h>
#include <ofile.h>
#include <win32_compat.h>

///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//#include <ddraw.h>          // include DirectDraw stuff

// defines for screen parameters
#define SCREEN_WIDTH     800     // the width of the viewing surface
#define SCREEN_HEIGHT    525   // the height of the viewing surface
#define SCREEN_BPP       8       // the bits per pixel
#define SCREEN_COLORS    256     // the maximum number of colors

// defines for fixed point math
#define FIXP_SHIFT       12      // number of decimal places 20.12 	
#define FIXP_MUL         4096    // 2^12, used to convert reals

// defines for angles
#define PIE              ((double)3.14159265) // take a guess

#define ANGLE_360        (SCREEN_WIDTH * 360/40) // fov is 60 deg, so SCREEN_WIDTH * 360/60
#define ANGLE_180        (ANGLE_360/2)
#define ANGLE_120        (ANGLE_360/3)
#define ANGLE_90			 (ANGLE_360/4)
#define ANGLE_60         (ANGLE_360/6)
#define ANGLE_45         (ANGLE_360/8) 
#define ANGLE_30			 (ANGLE_360/12)
#define ANGLE_20         (ANGLE_360/18) 
#define ANGLE_15         (ANGLE_360/24)
#define ANGLE_10         (ANGLE_360/36)  
#define ANGLE_5          (ANGLE_360/72)
#define ANGLE_2          (ANGLE_360/180) 
#define ANGLE_1          (ANGLE_360/360) 
#define ANGLE_0          0
#define ANGLE_HALF_HFOV  (ANGLE_360/18)

// defines for height field
//Size of bitmap
#define HFIELD_WIDTH      512	// width of height field data map
#define HFIELD_HEIGHT     512   // height of height field data map  
#define HFIELD_BIT_SHIFT  9     // log base 2 of 512
//Size of bitmap
#define TERRAIN_SCALE_X2  2     // scaling factor for terrain

#define VIEWPLANE_DISTANCE (SCREEN_WIDTH/64)

#define START_X_POS      256    // starting viewpoint position
#define START_Y_POS      256 
#define START_Z_POS      700

#define START_PITCH      0*(SCREEN_HEIGHT/240)    // starting angular heading
#define START_HEADING    ANGLE_90

#define MAX_STEPS        400    // number of steps to cast ray

#define VP_X				 264;
#define VP_Y				 216;
#define VP_Z				7900;
#define VP_ANG_X				0;
#define VP_ANG_Y			1810;
#define VP_ANG_Z				0;

// TYPES /////////////////////////////////////////////////////////////////////
typedef unsigned char  UCHAR;

// the bitmap file structure /////////////////////////////////////////////////
#ifdef NO_WINDOWS
#pragma pack(1)
typedef struct {
	WORD  bfType;
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack()

typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;
	LONG  biWidth;
	LONG  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG  biXPelsPerMeter;
	LONG  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY;

const uint8_t PC_NOCOLLAPSE = 0x04;
#endif

typedef struct bitmap_file_tag
        {
        BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
        BITMAPINFOHEADER bitmapinfoheader;  // this is all the info is
        PALETTEENTRY     palette[256];      // we will store the palette here
        UCHAR            *buffer;           // this is a pointer to the data
        } bitmap_file, *bitmap_file_ptr;

typedef struct pixel_colour
			{
			short red;
			short green;
			short blue;
			} pixel_colour;

// PROTOTYPES ////////////////////////////////////////////////////////////////

static int curSpeechWavId = 0;
static unsigned seed = 35;				// last random number
static int load_bitmap_file(bitmap_file_ptr bitmap, const char *filename);
static int unload_bitmap_file(bitmap_file_ptr bitmap);
static void build_tables(void);
static unsigned rand_seed(void);
static char state_recno_of_screen[SCREEN_HEIGHT * SCREEN_WIDTH];

pixel_colour	pixel[HFIELD_WIDTH * HFIELD_HEIGHT];


UCHAR  			 *height_map_ptr   = NULL,  // pointer to height field height data
					 *color_map_ptr    = NULL;  // pointer to height field color data

bitmap_file     height_bmp_file,           // holds the height data
                color_bmp_file;            // holds the color data

	// this is very important, it is based on the fov and scaling
	// factors, it is the delta slope between two rays piercing
	// the viewplane that a single pixel from each other in the
	// same colum
int	dslope = (int)(((double)1/(double)VIEWPLANE_DISTANCE)*FIXP_MUL), 

	cos_look[ANGLE_360],                        // trig lookup tables, really only
	sin_look[ANGLE_360];                        // need onem but 2 is easier

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


//---------- define coordinates ---------//

enum { INTRO_TEXT_X1 = 10,
		 INTRO_TEXT_Y1 = 30,
		 INTRO_TEXT_X2 = 790,
		 INTRO_TEXT_Y2 = 590,
	  };

enum { INTRO_MAP_X1 = 10,
		 INTRO_MAP_Y1 = INTRO_TEXT_Y2 + 5,
		 INTRO_MAP_WIDTH  = 260,
		 INTRO_MAP_HEIGHT = 260  };

enum { DIALOG_TEXT_X1 = 180,
		 DIALOG_TEXT_Y1 = 5,
		 DIALOG_TEXT_X2 = 620,
		 DIALOG_TEXT_Y2 = 135 };

enum { NARRATIVE_TEXT_X1 = 20,
		 NARRATIVE_TEXT_Y1 = 5,
		 NARRATIVE_TEXT_X2 = 780,
		 NARRATIVE_TEXT_Y2 = 400 };

enum { MAIN_MAP_X1  = 20,
		 MAIN_MAP_Y1  = 180,
		 MAIN_MAP_X2  = 780,
		 MAIN_MAP_Y2  = MAIN_MAP_Y1+260 };

enum { RELATION_X1  = 20,
		 RELATION_Y1  = MAIN_MAP_Y2+10,
		 RELATION_X2  = 780,
		 RELATION_Y2  = RELATION_Y1+150 };

enum { RELATION_REC_WIDTH  = 250,
		 RELATION_REC_HEIGHT = 20   };

enum { LETTER_TEXT_X1 = 100,
		 LETTER_TEXT_Y1 = 50,
		 LETTER_TEXT_X2 = 700,
		 LETTER_TEXT_Y2 = 400 };

enum { LETTER_REPLY_X1 = 70,
		 LETTER_REPLY_Y1 = 410,
		 LETTER_REPLY_X2 = 730,
		 LETTER_REPLY_Y2 = 580  };

enum { MAP_UPPER_RIGHT_CORNER_X = 220+40,
		 MAP_UPPER_RIGHT_CORNER_Y = 162-20,
		 MAP_UPPER_LEFT_CORNER_X = 588-40,
		 MAP_UPPER_LEFT_CORNER_Y = 162-20,
		 MAP_LOWER_RIGHT_CORNER_X = -20,
		 MAP_LOWER_RIGHT_CORNER_Y = 449-40,
		 MAP_LOWER_LEFT_CORNER_X = 810,
		 MAP_LOWER_LEFT_CORNER_Y = 449-40};


//---------- Declare static functions -----------//

static char* 	get_next_paragraph(char* textPtr);

static void 	temp_open_res();
static void 	temp_close_res();

//---------- Declare static vars -----------//

static char		letter_refresh_flag = INFO_REPAINT;
static char		map[HFIELD_WIDTH][HFIELD_HEIGHT];

//---------- Define struct TextBlock ----------//

struct TextBlock
{
	char* text_ptr;
	short x1, y1, x2, y2;
	short nullified_pos;
};

#define MAX_TEXT_BLOCK   		 		6

#define CONTENT_TEXT_BLOCK_ID  		0
#define FIRST_REPLY_TEXT_BLOCK_ID   1

static short	  text_block_count;
static TextBlock text_block_array[MAX_TEXT_BLOCK];

// INLINE FUNCTIONS ///////////////////////////////////////////////////////////

inline int COS_LOOK(int theta)
{
// this inline function returns the cosine of the sent angle taking into
// consideration angles > 360 and < than 0

if (theta < 0)
	return(cos_look[theta + ANGLE_360]);
else
if (theta >= ANGLE_360)
	return(cos_look[theta - ANGLE_360]);
else
	return(cos_look[theta]);

} // end COS_LOOK

///////////////////////////////////////////////////////////////////////////////

inline int SIN_LOOK(int theta)
{
// this inline function returns the sine of the sent angle taking into
// consideration angles > 360 and < than 0

if (theta < 0)
	return(sin_look[theta + ANGLE_360]);
else
if (theta >= ANGLE_360)
	return(sin_look[theta - ANGLE_360]);
else
	return(sin_look[theta]);

} // end SIN_LOOK

static unsigned rand_seed()
{
	#define MULTIPLIER      0x015a4e35L
	#define INCREMENT       1
	seed = MULTIPLIER * seed + INCREMENT;
	return seed;
}

//-------- Begin of function Campaign::main_loop ------//

void Campaign::main_loop(int isLoadedGame)
{
	int screenNeedUpdate = 1;
	static int calledCount=0;
	int hasInited=0;
	int i, j, firstRun = 1;
	int stageId;
	
	calledCount++;

	//--- if this is a saved campaign and a campaign game was running in the saved game ---//

	if( game.init_flag )
	{
		sys.run(isLoadedGame);

		if( calledCount >= 2 )
		{
			calledCount--;
			return;
		}
		else
		{
			goto go_point1;
		}
	}

	//------------------------------------//

	while(1)
	{
		//------ race_res debugging code -------//

#ifdef DEBUG
		for( i=0 ; i<race_res.name_count ; i++ )
		{
			err_when( race_res.name_used_array[i] < 0 || race_res.name_used_array[i] > 10 );
		}
#endif

		//--------------------------------------//

		sys.signal_exit_flag = 0;

		for (i = 0; i < HFIELD_WIDTH; i++)
			for (j = 0; j < HFIELD_HEIGHT; j++)
				map[i][j] = smoothing_checking(i, j, HFIELD_BIT_SHIFT);

		if (screenNeedUpdate)
		{
			if (firstRun && !isLoadedGame)
			{
				disp_strategic_screen(1, 1);
				mouse.wait_press(1);
				disp_strategic_screen(1, 2);
				disp_strategic_screen(1, 3);
				disp_strategic_screen(1, 4);
				firstRun = 0;
			}
			else
				disp_strategic_screen();

			screenNeedUpdate = 0;
		}

		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		sys.blt_virtual_buf();
	
		//---- if an event should happen now ----//

		if( event_id )
		{
			int eventId = event_id;		// save it as it may be changed in process_event()

			if( process_event() )	// call the function of the derived class
				screenNeedUpdate = 1;
		}
		else
		{
			stageId = stage_id;

			saved_random_seed = misc.random_seed;

			stage_prelude();

			//------ play a game now -------//

				// --- hint for balance race_res.name_used_array and monster_res.name_used_array ----//
				//
				// call race_res.sum_name_used() and monster_res.sum_name_used()
				// to get the balance before game
				// and get the values again after game
				//
				// -------------------------------------------------------------//

			if( !auto_test_flag )
			{
				pick_royal_to_battle();
			}
			else		// if in auto test mode
			{
				if( can_use_fryhtan )				// always use fryhtan if the player can
					this_battle_use_fryhtan = 1;
			}

			game.init();
			hasInited = 1;

			Battle::run();

go_point1:
			if( sys.signal_exit_flag && sys.signal_exit_flag != 3 )
			{
				if( hasInited )
					game.deinit();

				break;
			}

			//----- process the result of the game -----//

			if( !replay_stage_flag )
			{
				sys.change_display_mode(MODE_ID_DEFAULT);
				for (i = 0; i < HFIELD_WIDTH; i++)
					for (j = 0; j < HFIELD_HEIGHT; j++)
						map[i][j] = smoothing_checking(i, j, HFIELD_BIT_SHIFT);
				disp_strategic_screen();

				process_game_result();

				// ###### begin Gilbert 7/5 ########//
				if( !auto_test_flag && game.result_win_condition_id >= 0 )			// don't update in auto test mode
				{
					update_royal_unit();
				}
				else
				{
					unpick_royal_to_battle();
				}
				// ###### end Gilbert 7/5 ########//
			}
			else
			{
				replay_stage_flag = 0;
				// ###### begin Gilbert 7/5 ########//
				unpick_royal_to_battle();
				// ###### end Gilbert 7/5 ########//
				screenNeedUpdate = 1;
			}

			//--------- update royal units ---------//

			// ###### begin Gilbert 7/5 ########//	// replay_stage_flag has been reset
//			if( !auto_test_flag )
//			{
//				if(!replay_stage_flag)		// don't update in auto test mode
//					update_royal_unit();
//				else
//					unpick_royal_to_battle();
//			}
			// ###### end Gilbert 7/5 ########//

			hasInited=0;

			//------- deinit the game --------//

			err_when( !sys.signal_exit_flag );

			game.deinit();
		}

		//--------------------------------//
		//	1-quit to windows
		// 2-quit to main menu
		// 3-quit a campaign game only, but still remain in the campaign
		//--------------------------------//

		if( sys.signal_exit_flag && sys.signal_exit_flag != 3 )
			break;
	}

	//----------------------------//

	calledCount--;
}
//-------- End of function Campaign::main_loop ------//


//-------- Begin of function Campaign::set_stage ------//

void Campaign::set_stage(int stageId)
{
#ifdef DEBUG
	if( game.debug_log && stageId )
		game.debug_log->printf( "Stage: %d\n\r", stageId );
#endif

	//----------------------------------//

	stage_id = stageId;
	plot_id = 0;

	set_event(0);

	//---- increase the run counter ----//

	if( stage_id )
	{
		err_when( stage_id < 1 || stage_id > MAX_STATE );

		stage_run_count_array[stage_id-1]++;

		total_stage_run_count++;
	}
}
//-------- End of function Campaign::set_stage ------//


//-------- Begin of function Campaign::set_event ------//

void Campaign::set_event(int eventId)
{
#ifdef DEBUG
	if( game.debug_log && eventId )
		game.debug_log->printf( "                Event: %d\n\r", eventId );
#endif

	event_id = eventId;

	//---- increase the run counter ----//

	if( event_id )
	{
		err_when( event_id < 1 || event_id > MAX_EVENT );

		event_run_count_array[event_id-1]++;

		total_event_run_count++;

		//----- reset stage vars ------//

		attacker_state_recno  = 0;
		attacker2_state_recno = 0;
		target_state_recno 	 = 0;
		this_battle_use_fryhtan = 0;
	}
}
//-------- End of function Campaign::set_event ------//


//------ Begin of function Campaign::disp_state_map ------//
//
// return 0 for cancel, other for selected scenario
//
void Campaign::disp_state_map(int dispX, int dispY, int dispWidt, int dispHeigh, int par)
{
	// par bit 0 = 0 = drawing the mini map
	//		 bit 0 = 1 = drawing the large map
	//		 other bits = intensity of boundary map & terrain map

	if ((par & 1) == 0)
	{
		par = par >>1;
		int dispX1 = INTRO_MAP_X1;
		int dispY1 = INTRO_MAP_Y1;
		int dispWidth = INTRO_MAP_WIDTH;
		int dispHeight = INTRO_MAP_HEIGHT;
											
		mouse.hide_area( dispX1, dispY1, dispX1+dispWidth-1, dispY1+dispHeight-1 );
		for (int i = 0; i< dispWidth; i++)
		{
			for (int j = 0; j< dispHeight; j++)
			{
				short *point = vga_back.buf_ptr(dispX1 + i, dispY1 + j);

				int xLoc = i * (state_array.max_x_loc-1) / dispWidth;
				int yLoc = j * (state_array.max_y_loc-1) / dispHeight;
				int xr = i * HFIELD_WIDTH / dispWidth;
				int yr = j * HFIELD_HEIGHT / dispHeight;

				short color = random_terrain_map.get_pix(xr, yr);
				
				StateLocation *stateLoc = state_array.get_loc(xLoc, yLoc);
				int s = stateLoc->state_recno;
				int n;
			//	int shadow = shadow_array[xr + (yr<<HFIELD_BIT_SHIFT)];
			
			//	if( yLoc == state_array.max_y_loc-1
			//	|| xLoc == state_array.max_x_loc-1 
			//	|| !s
			//	|| stateLoc[1].state_recno != s
			//	|| state_array.get_loc( xLoc, yLoc+1)->state_recno != s
			//	|| state_array.get_loc( xLoc+1, yLoc+1)->state_recno != s 
				if(!map[xr][yr]
				||	map[xr+1][yr] != map[xr][yr]
				||	map[xr][yr+1] != map[xr][yr]
				||	map[xr+1][yr+1] != map[xr][yr]
				||	map[xr+2][yr] != map[xr][yr]
				||	map[xr+2][yr+1] != map[xr][yr]
				||	map[xr+2][yr+2] != map[xr][yr]
				||	map[xr+1][yr+2] != map[xr][yr]
				||	map[xr+1][yr+1] != map[xr][yr]
						
				|| ((i & 1) == 0 && (j & 1) == 0 && par < 1)
				|| ((i & 1) == 0 && (j & 1) == 1 && par < 2)
				|| ((i & 1) == 1 && (j & 1) == 0 && par < 3)
				|| ((i & 1) == 1 && (j & 1) == 1 && par < 4) )
				{
				//	*point = magic.draw(color_bmp_file.palette[color].peRed,
				//		color_bmp_file.palette[color].peGreen, color_bmp_file.palette[color].peBlue,
				//		vga.pixel_format_flag);
				}
				else
				{
					if( (n = state_array[map[xr][yr]]->campaign_nation_recno) )
					{
						*point = VgaBuf::translate_color(get_nation(n)->nation_color);
					}
					else
					{
						*point = VgaBuf::translate_color(game.color_remap_array[0].main_color);
					}
				}
			}
		}
		mouse.show_area();
	}
	else
	{
		par = par >>1;
		int dispX1 = MAIN_MAP_X1;
		int dispY1 = MAIN_MAP_Y1;
		int dispWidth = MAIN_MAP_X2-MAIN_MAP_X1+1;
		int dispHeight = MAIN_MAP_Y2-MAIN_MAP_Y1+1;
			
		mouse_cursor.set_icon(CURSOR_WAITING);
		mouse_cursor.set_frame(0);
		sys.yield();
//		mouse.hide_area( dispX1, dispY1, dispX1+dispWidth-1, dispY1+dispHeight-1 );
	
//		int shift = -40;
//		render_terrain(START_X_POS + 8 + ((shift*COS_LOOK(START_HEADING)) >> FIXP_SHIFT), 
//			START_Y_POS + ((shift*SIN_LOOK(START_HEADING)) >> FIXP_SHIFT), //START_Y_POS
//			7900, 0, 10 + START_HEADING, 0, par); //START_Z_POS START_PITCH
		render_terrain(par);
		mouse_cursor.set_icon(CURSOR_NORMAL);
//		mouse.show_area();
	}
}
//------ End of function Campaign::disp_state_map ------//

//------ Begin of function Campaign::disp_intro ------//
//
// Display the campaign introduction screen.
//
void Campaign::disp_intro()
{
/*	load_bitmap_file(&color_bmp_file,"heightc3.bmp");
	color_map_ptr = color_bmp_file.buffer;
	int i, j;
	for (i = 0; i < HFIELD_WIDTH; i++)
		for (j = 0; j < HFIELD_HEIGHT; j++)
			map[i][j] = smoothing_checking(i, j, HFIELD_BIT_SHIFT);
	
	
	//-----------------------------//
	int par = 0;

	while(1)
	{
	//	par ++;
	//	if (par > 8)
	//		par = 0;
		par = 3;

		vga.use_back();

		vga_util.disp_image_file("CAMP2");

		disp_intro_text();

//		disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, par < 5 ? par<<1 : (8 - par)<<1 );

		vga.use_front();

		vga.flip();

		sys.blt_virtual_buf();

		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		sys.blt_virtual_buf();

		if( mouse.left_press || mouse.right_press )
			break;

		if( misc.upper(mouse.key_code)=='T' )		// testing run - no games will be played
		{
			auto_test_flag = 1;
			break;
		}

		if( detect_cheat_key() )
			break;
	}
	unload_bitmap_file(&color_bmp_file);*/

	int i;
	int counter = VGA_HEIGHT;
	mouse.hide();
	
	vga.use_back();
	vga_util.disp_image_file("frwin01");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
	sys.sleep(500);

	music.stop();
	play_speech("POEM1.WAV");
	
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < -100)
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_1(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_2(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_3(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_4(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_5(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_6(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_7(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_8(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
/*		if( config.music_flag )
		{
			if( !music.is_playing(1) )
				music.play(1, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();*/

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}
		
		if( mouse.right_press )
			break;

		sys.sleep(50);
	}

	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	
/*	vga.use_back();
	vga_util.disp_image_file("HUWIN01");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
	sys.sleep(500);
		
	counter = VGA_HEIGHT;
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < -100)
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_5(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_6(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_7(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_intro_8(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
//		if( config.music_flag )
//		{
//			if( !music.is_playing(1) )
//				music.play(1, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
//		}
//		else
//			music.stop();

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}

		if( mouse.right_press )
			break;

		sys.sleep(50);
	}
	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}*/
	mouse.show();
	stop_speech();

}
//------- End of function Campaign::disp_intro -------//

//------ Begin of function Campaign::disp_monster_defeated ------//
//
// Display the final victory campaign introduction screen.
//
void Campaign::disp_monster_defeated()
{
	int i;
	int counter = VGA_HEIGHT;
	
	mouse.hide();
	vga.use_back();
	vga_util.disp_image_file("huwin02");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
	sys.sleep(500);
	
	music.stop();
	play_speech("POEM2.WAV");
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < (VGA_HEIGHT>>1))
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_1(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_2(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_3(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_4(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
/*		if( config.music_flag )
		{
			if( !music.is_playing(2) )
				music.play(2, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();*/

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}
		
		if( mouse.right_press )
			break;

		sys.sleep(50);
	}

	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	
/*	vga.use_back();
	vga_util.disp_image_file("HUWIN03");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
//	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
//	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
//	sys.sleep(500);
		
//	counter = VGA_HEIGHT;
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < -100)
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_1(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_2(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_3(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_monster_fall_4(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
//		if( config.music_flag )
//		{
//			if( !music.is_playing(2) )
//				music.play(2, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
//		}
//		else
//			music.stop();

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}

		if( mouse.right_press )
			break;

		sys.sleep(50);
	}
	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}*/
	mouse.show();
	stop_speech();

}
//------- End of function Campaign::disp_monster_defeated -------//

//------ Begin of function Campaign::disp_final_victory ------//
//
// Display the final victory campaign introduction screen.
//
void Campaign::disp_final_victory()
{
	int i;
	int counter = VGA_HEIGHT;
	
	mouse.hide();
	vga.use_back();
	vga_util.disp_image_file("brief01");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
	sys.sleep(500);

	music.stop();
	play_speech("POEM3.WAV");	
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < -100)
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_1(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_2(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_3(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_4(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_5(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_6(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_7(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_8(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_9(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
/*		if( config.music_flag )
		{
			if( !music.is_playing(5) )
				music.play(5, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();*/

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}
		
		if( mouse.right_press )
			break;

		sys.sleep(50);
	}

	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	
/*	vga.use_back();
	vga_util.disp_image_file("HUWIN04");
	sys.yield();
	for (i = 0; i < 4; i++)
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga_front.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 4 - i, V_BLACK );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
	sys.sleep(500);
		
	counter = VGA_HEIGHT;
	while(1)
	{
		int tempCounter = counter;

		if ((counter -= 3) < -100)
		//	counter = VGA_HEIGHT;
			break;

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		vga.use_front();
		font_cmpo.space_width -= 4;	
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_5(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_6(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_7(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_8(), 1, &font_cmpo);
		tempCounter += font_cmpo.text_height();
		put_center_text(VGA_WIDTH>>1, tempCounter, text_campaign.str_victory_9(), 1, &font_cmpo);
		font_cmpo.space_width += 4;	
		sys.blt_virtual_buf();

		sys.yield();
//		if( config.music_flag )
//		{
//			if( !music.is_playing(5) )
//				music.play(5, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
//		}
//		else
//			music.stop();

		if( mouse.left_press )
		{
			counter -= 12;
			continue;
		}

		if( mouse.right_press )
			break;

		sys.sleep(50);
	}
	for (i = 0; i < 4; i++)
	{	
		vga_back.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 1, V_BLACK );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		sys.sleep(100);
	}*/
	mouse.show();
	stop_speech();
}
//------- End of function Campaign::disp_final_victory -------//

//---------- Begin of function Campaign::put_center_text -----------//
//
// <int>   x, y - center of the absolute position where the text should be put.
// <char*> str  - the display string.
// <char> black - using a transparent black blackground or not
//
void Campaign::put_center_text(int x, int y, const char* str, char black, Font* fontPtr, int darkness)
{
	int i, j;
	int barWidth = 10;
	int space = 5;
	unsigned level;
	seed = 35;
		

	if (black)
	{
		str = translate.process(str);

		static Blob2DW tempBuffer;

		short w = fontPtr->text_width(str);
		short h = fontPtr->max_font_height;

		int x1 = x - w/2;
		int y1 = y - h/2;
		int x2 = x1 + w - 1;
		int y2 = y1 + h - 1;

		if ((x1 > (VGA_WIDTH-1)) ||
			 (y1 > (VGA_HEIGHT-1)) ||
			 (x2 < 0) ||
			 (y2 < 0))
			 return;

		tempBuffer.clear();
		tempBuffer.resize(-w/2, -h/2, w, h);

		fontPtr->put_to_bufferW(tempBuffer.ptr->get_ptr(), tempBuffer.buf_true_pitch(), 0, 0, str);
		
		int srcX1 = (x1) < 0 ? 0 : (x1);
		int srcY1 = (y1+space) < 0 ? 0 : (y1+space);
		int srcX2 = (x2) >= VGA_WIDTH ? VGA_WIDTH -1 : (x2);
		int srcY2 = (y2-space) >= VGA_HEIGHT ? VGA_HEIGHT -1 : (y2-space);
		
		srcX1 = x1 >= 0 ? 0 : -x1;
		srcY1 = y1 >= 0 ? 0 : -y1;
		srcX2 = x2 < VGA_WIDTH ? x2 - x1 : VGA_WIDTH - x1 - 1;
		srcY2 = y2 < VGA_HEIGHT ? y2 - y1 : VGA_HEIGHT - y1 - 1;
		vga.active_buf->put_bitmapW_area_trans(x1, y1, tempBuffer.bitmap_ptr(),	srcX1, srcY1, srcX2, srcY2);
	}
	else
	{
		fontPtr->text_width( str, -1, 440 );
		int textHeight = fontPtr->text_height()+5;

	//	textHeight = MIN( textHeight, 560 );

		int barWidth = 15;
		int x1 = x + (ZOOM_WIDTH-460) / 2 -10;
		int x2 = x1 + 460 - 1 +10;
		int y1 = y + (ZOOM_HEIGHT-textHeight) / 2;
		int y2 = y1 + textHeight - 1;

		y1 = MAX (ZOOM_Y1+barWidth, y1);
		y2 = MIN (ZOOM_Y2-barWidth, y2);

		vga.active_buf->bar_alpha( x1, y1, x2, y2, darkness, V_BLACK );
		
	//	int barWidth = 15;

		for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //upper
		{
			level = 0;
			for (j = y1-barWidth; j < y1; j ++, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga.active_buf->bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}
		
		for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //lower
		{
			level = 0;
			for (j = y2+barWidth; j > y2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga.active_buf->bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}
		
		for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //right
		{
			level = 0;
			for (j = x1-barWidth; j < x1; j ++, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga.active_buf->bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}
		
		for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //left
		{
			level = 0;
			for (j = x2+barWidth; j > x2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga.active_buf->bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}
			
		fontPtr->put_paragraph( x1+10, y1, x2-10, y2, str );
	}
}
//----------- End of function Campaign::put_center_text ------------//

//------ Begin of function Campaign::detect_cheat_key ------//
//
int Campaign::detect_cheat_key()
{
	int keyCode = misc.upper(mouse.key_code);

	if( keyCode == 'E' || keyCode == 'S' )
	{
		char idStr[] = "00";

		idStr[0] = mouse.get_key();
		idStr[1] = mouse.get_key();

		int idNumber = atoi(idStr);

		if( keyCode=='E' )
		{
			if( idNumber <= max_event )
				set_event(idNumber);
		}
		else
		{
			if( idNumber <= max_stage )
				set_stage(idNumber);
		}

		return 1;
	}

	return 0;
}
//------- End of function Campaign::detect_cheat_key -------//


//------ Begin of function Campaign::disp_intro_text ------//
//
// Display the campaign introduction screen.
//
void Campaign::disp_intro_text()
{
	//------- get the tutor msgs from the resource file -------//

	char* textBuf = res_stage.read( "MAININTR" );

	//----- display the text on the first text area -------//

	font_scre.put_paragraph( INTRO_TEXT_X1, INTRO_TEXT_Y1,
									 INTRO_TEXT_X2, INTRO_TEXT_Y2, textBuf );
}
//------- End of function Campaign::disp_intro_text -------//

//------ Begin of function Campaign::disp_strategic_screen ------//
//
// [int] shouldBltBuf - whether it should blt buffer after
//								displaying the content. (default: 1)
//
// [int] terrainMapOnly - whether it should display state map also
//									(default : 0)
void Campaign::disp_strategic_screen(int shouldBltBuf, int terrainMapOnly)
{
	//--------------------------------//

	int useBack = vga.use_back_buf;

	if( !useBack )
		vga.use_back();

//	vga_back.bar(0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_WHITE);

	//---------- display text box ---------//

//	vga_back.rect( NARRATIVE_TEXT_X1, NARRATIVE_TEXT_Y1, NARRATIVE_TEXT_X2, NARRATIVE_TEXT_Y2, 3, V_BLACK );


	//---------- display relationship box ---------//

	disp_nation_info();

	//---------- display map --------------//

	load_bitmap_file(&color_bmp_file,"heightc3.bmp");
	color_map_ptr = color_bmp_file.buffer;
	build_tables();

	if (terrainMapOnly > 0 && terrainMapOnly < 5)
		disp_state_map(MAIN_MAP_X1, MAIN_MAP_Y1, MAIN_MAP_X2-MAIN_MAP_X1+1, MAIN_MAP_Y2-MAIN_MAP_Y1+1, (((terrainMapOnly-1)<<1) + 1) );
	else
		disp_state_map(MAIN_MAP_X1, MAIN_MAP_Y1, MAIN_MAP_X2-MAIN_MAP_X1+1, MAIN_MAP_Y2-MAIN_MAP_Y1+1, (3<<1) +1 );

	unload_bitmap_file(&color_bmp_file);

	//------ display campaign score -------//

	String str;
//	str = "Campaign Score To Date : ";
//	str += misc.format( campaign_score, 1 );
//	font_bld.center_put( 550, 480, 750, 500, text_campaign.str_campaign_score(campaign_score) );

	//------- display debug info --------//

	if( event_id )
	{
//		str  = "Event : ";
//		str += event_id;
//		str += " (Run count: ";
//		str += total_event_run_count;
//		str += ")";
		font_bld.center_put( 550, 460, 750, 480, text_campaign.str_event_count(event_id, total_event_run_count) );
	}
	else if( stage_id )
	{
//		str  = "Stage : ";
//		str += stage_id;
//		str += " (Run count: ";
//		str += total_stage_run_count;
//		str += ")";
		font_bld.center_put( 550, 460, 750, 480, text_campaign.str_stage_count(stage_id, total_stage_run_count) );
	}

	int x = 510, y = 530;
	int length;
	int score = campaign_score *campaign_difficulty /3;
	str = campaign_score;
	length = strlen(str);
	str = score;
	length += strlen(str);
	x -= length * font_bld.width();

//	str  = translate.process("Score");
	str  = text_campaign.str_score();
	str += " = ";
	str += campaign_score;
	str += " X ";

	x = font_bld.put( x, y+20, str )+5;

	str  = campaign_difficulty;
// str += " ";
//	str += translate.process( "(Difficulty Rating)" );
	str += " (";
	str += text_campaign.str_difficulty_rating();
	str += ")";

//	font_bld.center_put( x, y+9, x+140, y+6+font_bld.height(), str );
//	vga_back.bar( x, y+27, x+140, y+28, V_BLACK );
//	font_bld.put( x+70, y+30, 3 );

	int x2 = font_bld.put( x, y+9, str );
	vga_back.bar( x, y+27, x2, y+28, V_BLACK );
	font_bld.put( (x+x2-8)/2, y+30, 3 );

	str = "= ";
	str += score;
	// font_bld.put( x+150, y+18, str);
	font_bld.put( x2+6, y+18, str);

	//-------------------------------------//

	if( !useBack )
		vga.use_front();

	if( shouldBltBuf )
	{
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);
		sys.blt_virtual_buf();
	}
}
//------- End of function Campaign::disp_strategic_screen -------//


//------ Begin of function Campaign::disp_in_game_msg ------//
//
// Called by Game::game_end() to display a message when
// the game ends.
//
// <char>  - use front buffer
// <char*> - the message
// <...>   - a list of arguments for the message.
//
void Campaign::disp_in_game_msg(char* gameMsg, ...)
{
	if( auto_test_flag )
		return;

	char useFront = 1;
	enum { END_GAME_BOX_WIDTH  = 460,
			 MAX_END_GAME_BOX_HEIGHT = 560 };
	int i, j;
	unsigned level;

//	if (useFront)
//		vga.use_front();
//	else
//		vga.use_back();

	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	vga.use_front();
	short *saveBuf = vga_front.save_area(0, 0, VGA_WIDTH-1, ZOOM_Y1-1);

	//------- get text substitution arguments -----//

	va_list argPtr;        // the argument list structure

	va_start( argPtr, gameMsg );

	//------- merge the vars ----------//

	char* strBuf = mem_add( strlen(gameMsg)+200 );

	vsprintf( strBuf, gameMsg, argPtr );

	//-----------------------------------------//

	font_mid.text_width( strBuf, -1, END_GAME_BOX_WIDTH-20 );

	int textHeight = font_mid.text_height()+50;

	textHeight = MIN( textHeight, MAX_END_GAME_BOX_HEIGHT );

	int x1 = ZOOM_X1 + (ZOOM_WIDTH-END_GAME_BOX_WIDTH) / 2 -10;
	int x2 = x1 + END_GAME_BOX_WIDTH - 1 +10;
	int y1 = ZOOM_Y1 + (ZOOM_HEIGHT-textHeight) / 2 -20;
	int y2 = y1 + textHeight - 1 +20;

//	char darkness = info.display_campaign_mission_briefing > 0 ? 3 : 1;
	char darkness = 3;

	vga_front.bar_alpha( x1, y1, x2, y2, darkness, V_BLACK );
	
	int barWidth = 15;

	for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //upper
	{
		level = 0;
		for (j = y1-barWidth; j < y1; j ++, level ++)
		{
			rand_seed();
			if ((seed%barWidth) < level)
			{
				vga_front.bar_alpha( i, j, i, j, darkness, V_BLACK );
			}
		}
	}
	
	for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //lower
	{
		level = 0;
		for (j = y2+barWidth; j > y2; j --, level ++)
		{
			rand_seed();
			if ((seed%barWidth) < level)
			{
				vga_front.bar_alpha( i, j, i, j, darkness, V_BLACK );
			}
		}
	}
	
	for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //right
	{
		level = 0;
		for (j = x1-barWidth; j < x1; j ++, level ++)
		{
			rand_seed();
			if ((seed%barWidth) < level)
			{
				vga_front.bar_alpha( j, i, j, i, darkness, V_BLACK );
			}
		}
	}
	
	for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //left
	{
		level = 0;
		for (j = x2+barWidth; j > x2; j --, level ++)
		{
			rand_seed();
			if ((seed%barWidth) < level)
			{
				vga_front.bar_alpha( j, i, j, i, darkness, V_BLACK );
			}
		}
	}
		
	font_mid.put_paragraph( x1+10, y1+10, x2-10, y2-10 -40, strBuf );
	font_mid.put_paragraph( x1+10, y2-10-20, x2-10, y2-10, text_campaign.str_click_to_continue() ); // "Click to continue.");

	//-----------------------------------------//

	mem_del(strBuf);

//	if (!useFront)
//	{	
//		vga.use_front();
//		vga.flip();
//	}
	sys.blt_virtual_buf();

	mouse.wait_press();

	if (saveBuf)
		vga_front.rest_area(saveBuf, 1);
}
//------- End of function Campaign::disp_in_game_msg -------//

//------- Begin of function Campaign::disp_end_game_bonus -------//
//
//
void Campaign::disp_end_game_bonus(int maxYear, int yearPassed)
{ 
	if( cheat_to_win_flag || yearPassed >= maxYear )
		return;

	//---------------------------------------------//

/*	int x = ( vga_back.buf_width() - 444 )/ 2;
	int y = ( vga_back.buf_height() - 275 )/ 2;
	String str;
	
	//back_buffer graphic here//
	vga_back.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	image_menu.put_back(x, y, "BOUNS");
	//back_buffer graphic here//
	
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);
	
	//text here//
	font_hall.center_put_paragraph( x+100, y, x+344, y+170, "Your bonus, to be added to your campaign score, is" );

	str = maxYear - yearPassed;
	font_hall.center_put_paragraph( x, y, x+444, y+275, str );

	str = "That is ";
	str += maxYear;
	str += ", minus the number of years that it took to achieve the goal";
	
	font_hall.center_put_paragraph( x+70, y+105, x+374, y+275, str );
	//text here//

	sys.blt_virtual_buf();

	if( !game.campaign()->auto_test_flag )		// do not wait if auto_test_flag is on.
		mouse.wait_press(60);		// 60 seconds to time out*/

	String str;
//	str = "Your bonus, to be added to your campaign score, is ";
//	str += maxYear - yearPassed;
//	str += "\n\nThat is ";
//	str += maxYear;
//	str += ", minus the number of years that it took to achieve the goal.";
	str  = text_campaign.str_bonus( maxYear - yearPassed );
	str += "\n\n";
	str += text_campaign.str_bonus_formula( maxYear );
	
	disp_in_game_msg(str);
}
//------- End of function Campaign::disp_end_game_bonus -------//


//------ Begin of function Campaign::disp_narrative ------//
//
// <char*> dialogText  - pointer to the text read from resource file
// <...>				  	  - a list of arguments.
//
void Campaign::disp_narrative(char* dialogText, ...)
{
	if( !dialogText )
		return;
	//------- get text substitution arguments -----//

	va_list argPtr;        // the argument list structure

	va_start( argPtr, dialogText );

	//------- read the text ------------//

	int textSize = strlen(dialogText) + 200;

	char* strBuf = mem_add(textSize);

	vsprintf( strBuf, dialogText, argPtr );

	//--------- display the text --------//
	short* background_bitmap = NULL;
	background_bitmap = (short *)mem_add( BitmapW::size(NARRATIVE_TEXT_X2-NARRATIVE_TEXT_X1+1, 
											NARRATIVE_TEXT_Y2-NARRATIVE_TEXT_Y1+1) );
	vga_back.read_bitmapW( NARRATIVE_TEXT_X1, NARRATIVE_TEXT_Y1, NARRATIVE_TEXT_X2, NARRATIVE_TEXT_Y2, background_bitmap );

	disp_text( NARRATIVE_TEXT_X1, NARRATIVE_TEXT_Y1, NARRATIVE_TEXT_X2, NARRATIVE_TEXT_Y2,
				  strBuf );

	mem_del( strBuf );

	if( !auto_test_flag )
		mouse.wait_press();

	vga_back.put_bitmapW(NARRATIVE_TEXT_X1, NARRATIVE_TEXT_Y1, background_bitmap);
	vga_util.blt_buf( NARRATIVE_TEXT_X1, NARRATIVE_TEXT_Y1, NARRATIVE_TEXT_X2, NARRATIVE_TEXT_Y2, 0);
	sys.blt_virtual_buf();
	
	mem_del( background_bitmap );
}
//------- End of function Campaign::disp_narrative -------//


//------ Begin of function Campaign::disp_dialog ------//
//
// <int>   raceId      - race id. of the speaker
// <char*> dialogText  - pointer to the dialog text read from resource file
// <int>   refreshFlag - refresh flag
//
// return: <int> if this dialog needs a reply, the returned
//					  value is id. of the reply.
//
void Campaign::disp_dialog(int raceId, char* dialogText, int refreshFlag)
{
	return;

	disp_text( DIALOG_TEXT_X1, DIALOG_TEXT_Y1, DIALOG_TEXT_X2, DIALOG_TEXT_Y2,
				  dialogText, (refreshFlag==INFO_REPAINT ? raceId : 0) );

	if( refreshFlag == INFO_REPAINT )
	{
		//-----------------------------//
	 
		int par = 0;

		while(1)
		{
			par ++;

			if (par > 3)
			{
				par = 3;
			}
			else
			{
			//	vga.use_back();
			//	
			//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, par < 5 ? ((par<<1) + 1) : (((8 - par)<<1) + 1) );
			//
			//	vga.use_front();
			//
			//	vga.flip();
				mouse.hide();
				vga.use_back();
				disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
				vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				sys.blt_virtual_buf();
				vga.use_front();
				mouse.show();
			}

			sys.blt_virtual_buf();

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			sys.blt_virtual_buf();

			if( mouse.left_press || mouse.right_press )
				break;
		}		
	}	
}
//------- End of function Campaign::disp_dialog -------//


//------ Begin of function Campaign::disp_text ------//
//
// <int>   boxX1, boxY1,   - the coordinate of the text box.
//			  boxX2, boxY2
// <char*> dialogText  		- pointer to the dialog text read from resource file
// [int]   animationRaceId - if this is given, a speech animation will be displayed using the given race id.
//									  (default: 0)
//
// return: <int> if this dialog needs a reply, the returned
//					  value is id. of the reply.
//
void Campaign::disp_text(int boxX1, int boxY1, int boxX2, int boxY2,
								 char* dialogText, int animationRaceId)
{
	//------ get the dialog and reply string ------//

	err_when( !dialogText );

	char* textPtr = dialogText;

	text_block_count = 0;

	int i;
	for( i=0 ; i<MAX_TEXT_BLOCK ; i++ )
	{
		textPtr = get_next_paragraph(textPtr);

		if( !textPtr )
			break;

		for( int j= -1 ; ; j-- )
		{
			if( textPtr[j] != 0x0D && textPtr[j] != 0x0A && textPtr[j] != '~' )
			{
				err_when( textPtr+j < dialogText );

				if( textPtr+j < dialogText )
					break;

				textPtr[j+1] = '\0';
				text_block_array[i].nullified_pos = j+1;
				break;
			}
		}

		text_block_array[i].text_ptr = textPtr;
		text_block_count++;
	}

	//---------- display the dialog ----------//

	mouse.hide();
	vga.use_back();
	
	font_mid.put_paragraph( boxX1+8, boxY1+8, boxX2-8, boxY2-8, dialogText );

	vga_util.blt_buf( boxX1, boxY1, boxX2, boxY2, 0);

	font_mid.next_text_y += font_san.height() + DEFAULT_LINE_SPACE;

	mouse.show();

	// --------- display speaking movie ------ //

	if( animationRaceId )
	{
		play_speech_animation(animationRaceId);
	}

	//---------- display the dialog ----------//

	for( i=0 ; i<text_block_count-1 ; i++ )		// the last reply is for nullification only. It doesn't have any content
	{
		text_block_array[i].x1 = boxX1+6;
		text_block_array[i].y1 = font_san.next_text_y;

		font_san.put_paragraph( boxX1+8, font_san.next_text_y,
			boxX2-8, boxY2-8, text_block_array[i].text_ptr );

		text_block_array[i].x2 = boxX2-6;
		text_block_array[i].y2 = font_san.next_text_y;

		font_san.next_text_y += (font_san.height() + DEFAULT_LINE_SPACE);

		if( mouse.in_area( boxX1, text_block_array[i].y1, boxX2,
			 text_block_array[i].y2 ) )
		{
			int textWidth = font_san.text_width(text_block_array[i].text_ptr);

			vga_back.adjust_brightness( boxX1+6, text_block_array[i].y1,
				boxX1+8+textWidth, text_block_array[i].y2, -3 );
		}

	}

	//----- restore NULL to '~' -------//

	for( i=0 ; i<text_block_count ; i++ )
	{
		text_block_array[i].text_ptr[ text_block_array[i].nullified_pos ] = 0x0D;
	}

	//----------------------------------------//

	vga_util.blt_buf( boxX1, boxY1, boxX2, boxY2, 0);
	vga.use_front();

	sys.blt_virtual_buf();
}
//--------- End of function Campaign::disp_text ----------//


//------ Begin of function Campaign::play_speech_animation ------//
//
// <int> raceId - race id. of the speaker, can be a monster or a human
//
void Campaign::play_speech_animation(int raceId)
{
	Flc flc;
	String flcName;
	flcName = DIR_CAMPAIGN;

	if( raceId > 0 )
	{
		flcName += "HUMAN";
		flcName += raceId;
	}
	else
	{
		flcName += "MON";
		flcName += -raceId;
	}

	flcName += ".FLC";

	if( !misc.is_file_exist(flcName) || !flc.open_file(flcName) )
		return;

	//----------------------------------------//

	Blob2D blob;
	blob.resize(0, 0, flc.width(), flc.height() );

	for( int playCount = 2; playCount > 0; --playCount )
	{
		flc.rewind();
		flc.advance();
		flc.advance();

		for( ; flc.cur_frame() < flc.max_frame(); flc.advance() )
		{
			DWORD startTime = misc.get_time();

			RGBColor *palBuf = (RGBColor *)flc.get_palette();
			// process palette
			short hiColorPal[0x100];
			for(int i = 0; i < 0x100; ++i  )
			{
				hiColorPal[i] = vga.make_pixel(
					palBuf[i].red << 2, palBuf[i].green << 2, palBuf[i].blue << 2 );
			}

			flc.get_area( blob.buf_ptr(), 0, 0, flc.width()-1, flc.height()-1);
			vga_back.put_bitmap_remap( DIALOG_TEXT_X1-155, DIALOG_TEXT_Y1+3 , blob.bitmap_ptr(), hiColorPal );
			vga_util.blt_buf( DIALOG_TEXT_X1-155, DIALOG_TEXT_Y1+3,
				DIALOG_TEXT_X1-155+flc.width()-1, DIALOG_TEXT_Y1+3+flc.height() -1, 0 );

			// wait

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)

		}
	}
}
//------- End of function Campaign::play_speech_animation -------//


//------ Begin of function Campaign::detect_reply ------//
//
// return: <int> if this dialog needs a reply, the returned
//					  value is id. of the reply.
//
int Campaign::detect_reply()
{
	//------ if auto test mode is on ------//

	if( auto_test_flag )
		return misc.random(text_block_count)+1;

	//-------------------------------------//

	for( int i=0 ; i<text_block_count ; i++ )
	{
		if( mouse.single_click( text_block_array[i].x1, text_block_array[i].y1,
										text_block_array[i].x2, text_block_array[i].y2 ) )
		{
			se_ctrl.immediate_sound("TURN_ON");
			return i+1;
		}
	}

	return 0;
}
//------- End of function Campaign::detect_reply -------//


//------ Begin of function Campaign::substitute_text ------//
//
// Subsitute the given text with the given variables.
//
// [char*] dialogText  - pointer to the dialog text read from resource file
//								 (if not given, it use the last dialogText)
// <...>				  	  - a list of arguments.
//
// return: <char*>  this dialog needs a reply, the returned
//					  value is id. of the reply.
//
char* Campaign::substitute_text(char* dialogText, ...)
{
	//------- get text substitution arguments -----//

	va_list argPtr;        // the argument list structure

	va_start( argPtr, dialogText );

	//------- read the text ------------//

	int textSize = strlen(dialogText) + 200;

	if( textSize > text_buf_size )
	{
		text_buf = mem_resize(text_buf, textSize);
		text_buf_size = textSize;
	}

	vsprintf( text_buf, dialogText, argPtr );

	return text_buf;
}
//------ End of function Campaign::substitute_text ------//


//------ Begin of function Campaign::disp_letter ------//
//
// <char> isMonster	  - if this is 1, then the letter is from a monster kingdom
//
// [char*] dialogText  - pointer to the dialog text read from resource file
//								 (if not given, it use the last dialogText)
// <...>				  	  - a list of arguments.
//
// return: <int> if this dialog needs a reply, the returned
//					  value is id. of the reply.
//
void Campaign::disp_letter(char isMonster, char* dialogText, ...)
{
	vga.use_back();

	int refreshFlag = letter_refresh_flag;
	static Font* fontPtr;

	if( isMonster != -1 )		// -1 when this function is called by detect_letter() and no parameters are passed to this function. 
		fontPtr = &font_cmph;
	else
		fontPtr = &font_cmpf;

	if( dialogText )           			// it use the last dialogText
	{
		//------- get text substitution arguments -----//

		va_list argPtr;        // the argument list structure

		va_start( argPtr, dialogText );

		//------- read the text ------------//

		int textSize = strlen(dialogText) + 200;

		if( textSize > text_buf_size )
		{
			text_buf = mem_resize(text_buf, textSize);
			text_buf_size = textSize;
		}

		vsprintf( text_buf, dialogText, argPtr );

		//------ get the dialog and reply string ------//

		char* textPtr = text_buf;

		text_block_count = 0;

		for( int i=0 ; i<MAX_TEXT_BLOCK ; i++ )
		{
			textPtr = get_next_paragraph(textPtr);

			if( !textPtr )
				break;

			for( int j= -1 ; ; j-- )
			{
				if( textPtr[j] != 0x0D && textPtr[j] != 0x0A && textPtr[j] != '~' )
				{
					err_when( textPtr+j < text_buf );

					if( textPtr+j < text_buf )
						break;

					textPtr[j+1] = '\0';
					text_block_array[i].nullified_pos = j+1;
					break;
				}
			}

			text_block_array[i].text_ptr = textPtr;
			text_block_count++;
		}

		err_when( text_block_count==0 );
	}

	//--- display the preface - saying we have received a letter, and click a mouse button to see the letter ---//

	if( refreshFlag == INFO_REPAINT )
	{
		disp_strategic_screen(0);		// don't blt buffer after the display

		mouse.hide();

		font_mid.put_paragraph( NARRATIVE_TEXT_X1+8, NARRATIVE_TEXT_Y1+8,
			NARRATIVE_TEXT_X2, NARRATIVE_TEXT_Y2, text_buf );

		font_mid.put( NARRATIVE_TEXT_X1+8, font_mid.next_text_y+20, 
			text_campaign.str_click_to_read_msg() );
			//"Click to read message..." );

		mouse.show();

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);
		sys.blt_virtual_buf();

		if( !auto_test_flag )
			mouse.wait_press();

		//---------- display the dialog ----------//
		if (!vga.use_back_buf)
		{
			vga.use_back();
			vga_util.disp_image_file("letters");
			vga.use_front();
		}
		else
			vga_util.disp_image_file("letters");

		if( isMonster == 1 )
			font_cmpf.center_put_paragraph( LETTER_TEXT_X1, LETTER_TEXT_Y1, LETTER_TEXT_X2, LETTER_TEXT_Y2,
				text_block_array[CONTENT_TEXT_BLOCK_ID].text_ptr );
		else
			font_cmph.put_paragraph( LETTER_TEXT_X1, LETTER_TEXT_Y1, LETTER_TEXT_X2, LETTER_TEXT_Y2,
				text_block_array[CONTENT_TEXT_BLOCK_ID].text_ptr );

		// replay use different font to letter !
		font_cmpa.next_text_y = LETTER_REPLY_Y1+8;
	}

	//---------- display the dialog reply choices ----------//

	static int lastReplyChoice;

	if( refreshFlag == INFO_UPDATE )
	{
		int thisReplyChoice=0;

		for( int i=FIRST_REPLY_TEXT_BLOCK_ID ; i<text_block_count-1 ; i++ )
		{
			if( mouse.in_area( text_block_array[i].x1, text_block_array[i].y1-8,
									 text_block_array[i].x2, text_block_array[i].y2+8 ) )
			{
				thisReplyChoice = i+1;
				break;
			}
		}

		if( thisReplyChoice != lastReplyChoice )
		{
			lastReplyChoice = thisReplyChoice;
			refreshFlag = INFO_REPAINT;
			font_cmpa.next_text_y = text_block_array[FIRST_REPLY_TEXT_BLOCK_ID].y1;
		}
	}

	//-----------------------------------------------//

	if( refreshFlag == INFO_REPAINT )
	{
		lastReplyChoice=0;

		for( int i=FIRST_REPLY_TEXT_BLOCK_ID ; i<text_block_count-1 ; i++ )		// the last reply is for nullification only. It doesn't have any content
		{
			text_block_array[i].x1 = LETTER_REPLY_X1+8;
			text_block_array[i].y1 = //fontPtr->next_text_y;
											font_cmpa.next_text_y;

			font_cmpa.put_paragraph( LETTER_REPLY_X1+8, font_cmpa.next_text_y,
				LETTER_REPLY_X2-8, LETTER_REPLY_Y2, text_block_array[i].text_ptr );

			text_block_array[i].x2 = LETTER_REPLY_X2-8;
			text_block_array[i].y2 = //fontPtr->next_text_y;
											font_cmpa.next_text_y;

			font_cmpa.next_text_y += (font_cmpa.height() + DEFAULT_LINE_SPACE);

			if( mouse.in_area( LETTER_REPLY_X1, text_block_array[i].y1, LETTER_REPLY_X2,
				 text_block_array[i].y2 ) )
			{
		//		int textWidth = font_cmpa.text_width(text_block_array[i].text_ptr );
		//
		//		vga_back.adjust_brightness( text_block_array[i].x1-3, text_block_array[i].y1-3,
		//			text_block_array[i].x2+3, text_block_array[i].y2+3, -3 );

				lastReplyChoice = i+1;
			}
		}
	}

	//---------------------------------//

	if( refreshFlag == INFO_REPAINT )
	{
		//--- if this letter is read only, there is no need to reply it, then await the user clicking to continue ---//

		if( text_block_count==1 )		// only the main content text block
		{
			font_cmpa.put( LETTER_REPLY_X1+8, LETTER_REPLY_Y1+8,
				text_campaign.str_click_to_continue() ); // "Click to continue..." );
		}

		//----------------------------------------//

		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);

		if (lastReplyChoice > 0)
		{
			int textWidth = font_cmpa.text_width(text_block_array[lastReplyChoice-1].text_ptr );

			vga_front.adjust_brightness( text_block_array[lastReplyChoice-1].x1-3, text_block_array[lastReplyChoice-1].y1-3,
				text_block_array[lastReplyChoice-1].x2+3, text_block_array[lastReplyChoice-1].y2+3, -3 );
		}

		sys.blt_virtual_buf();

		//----------------------------------------//

		if( text_block_count==1 )		
		{
			if( !auto_test_flag )
				mouse.wait_press();

			disp_strategic_screen();		// close the letter and return to the strategic screen
		}
	}

	vga.use_front();
}
//------- End of function Campaign::disp_letter -------//


//------ Begin of function Campaign::detect_letter ------//
//
// return: <int> if this dialog needs a reply, the returned
//					  value is id. of the reply.
//
int Campaign::detect_letter()
{
	mouse.clear_event();

	while(1)
	{
		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		sys.blt_virtual_buf();

		//-------- display the letter ---------//

		letter_refresh_flag = INFO_UPDATE;

		disp_letter();

		letter_refresh_flag = INFO_REPAINT;

		//-------- if auto test mode is on --------//

		if( auto_test_flag )
		{
			return misc.random(text_block_count)-FIRST_REPLY_TEXT_BLOCK_ID+1;
		}

		//-------- detect mouse action --------//

		for( int i=FIRST_REPLY_TEXT_BLOCK_ID ; i<text_block_count-1 ; i++ )
		{
			//--- if the mouse clicks on a reply ----//

			if( mouse.single_click( text_block_array[i].x1, text_block_array[i].y1-8,
											text_block_array[i].x2, text_block_array[i].y2+8 ) )
			{
				disp_strategic_screen();		// close the letter and return to the strategic screen
				se_ctrl.immediate_sound("TURN_ON");
				return i-FIRST_REPLY_TEXT_BLOCK_ID+1;
			}
		}
	}

	return 0;
}
//------- End of function Campaign::detect_letter -------//


//------ Begin of static function get_next_paragraph ------//
//
// return: <char*> the pointer to the next paragraph.
//						 NULL if there is no next paragraph.
//
static char* get_next_paragraph(char* textPtr)
{
	int loopCount=0;

	for( ; *textPtr!=26 && *textPtr ; textPtr++ )
	{
#if(defined(CHINESE) && defined(TRADITIONAL) )
		if( *textPtr == '~' && (textPtr[1]=='\r' || textPtr[1]=='\0' || textPtr[1]=='\n') )
#else
		if( *textPtr == '~' )			// paragraph separator
#endif
		{
			return textPtr+3;		// skip '~' + new line characters
		}

		err_when( ++loopCount > 10000 );
	}

	return NULL;
}
//------- End of static function get_next_paragraph -------//


//------ Begin of function Campaign::disp_nation_info ------//
//
void Campaign::disp_nation_info()
{
	if (!vga.use_back_buf)
	{
		vga.use_back();
		vga_util.disp_image_file("campaign");
		vga.use_front();
	}
	else
		vga_util.disp_image_file("campaign");

//vga_back.adjust_brightness( 0, 0, VGA_WIDTH -1, MAIN_MAP_Y2+10, -6 );

	image_menu.put_back(0, RELATION_Y1, "CAMP");

	int 	 x=RELATION_X1+8, y=RELATION_Y1+18;
	String str;

	for( int i=1 ; i<=MAX_NATION ; i++ )
	{
		if( is_nation_deleted(i) )
			continue;

		CampaignNation* cNation = get_nation(i);

		str = cNation->nation_name();

		int curStatus = cNation->relation_array[CAMPAIGN_PLAYER_NATION_RECNO-1].status;

		if( i != CAMPAIGN_PLAYER_NATION_RECNO )
		{
			str += " (";
			// str += NationRelation::relation_status_str_array[curStatus];
			str += text_talk.str_relation_status(curStatus);
			str += ")";
		}

		vga_back.draw_d3_up_border( x, y, x+12, y+12 );
		vga_back.bar( x+3, y+3, x+9, y+9, cNation->nation_color );

		font_bld.put( x+16, y, str );

		y += RELATION_REC_HEIGHT;

		if( y+RELATION_REC_HEIGHT > RELATION_Y2 -10)
		{
			x += RELATION_REC_WIDTH;
			y = RELATION_Y1 + 18;
		}
	
	}
}
//------- End of function Campaign::disp_nation_info -------//

//-------- Begin of function Campaign::attack_state ------//
//
// <int> attackerStateRecno  - the recno of the attacking state
// <int> targetStateRecno    - the recno of the target state
// <int> attackResult    	  - -1 the attacker loses
//										  1 the attacker wins
//									     0 result unknown
// [int] firstStep           - the first step of the attack sequence
// [int] lastStep 			  - the last step of the attack sequence
//										 (default:0, display all steps of the sequence)
// [int] attacker2StateRecno - if this is given, there will be two attacking
//									    forces attacking the target.
//
// step 1: move
// step 2: attack
// step 3: attack until one is killed.
//
void Campaign::attack_state(int attackerStateRecno, int targetStateRecno,
									 int attackResult, int firstStep, int lastStep,
									 int attacker2StateRecno)
{
	int attackerNation = attackerStateRecno ? state_array[attackerStateRecno]->campaign_nation_recno : 0;
	int targetNation = state_array[targetStateRecno]->campaign_nation_recno;

	int attack2Result = attackResult;

//#ifdef DEBUG
#if(0)

	// randomly create another attacker for debugging
	if( !attacker2StateRecno )
	{
		attacker2StateRecno = 1 + random(state_array.size() - 2);
		// exclude attackerStateRecno and targetStateRecno
		if( attackerStateRecno < targetStateRecno )
		{
			if( attacker2StateRecno >= attackerStateRecno )
				attacker2StateRecno++;
			if( attacker2StateRecno >= targetStateRecno )
				attacker2StateRecno++;
		}
		else
		{
			if( attacker2StateRecno >= targetStateRecno )
				attacker2StateRecno++;
			if( attacker2StateRecno >= attackerStateRecno )
				attacker2StateRecno++;
		}
	}

	// random result
	if( attackResult != 0 )
	{
		attack2Result = random(2) ? 1 : -1;
//		attack2Result = -1;
	}
#endif

	int attacker2Nation = attacker2StateRecno ? state_array[attacker2StateRecno]->campaign_nation_recno : 0;

	//---- determine the attack and target unit id. ------//

	temp_open_res();

	int attackerUnitId = 0;
	if( attackerNation )
	{
		attackerUnitId = get_nation(attackerNation)->is_human() ?
			race_res[get_nation(attackerNation)->race_id]->infantry_unit_id :
			monster_res[get_nation(attackerNation)->monster_id()]->unit_id;
	}

	int targetUnitId = get_nation(targetNation)->is_human() ?
		race_res[get_nation(targetNation)->race_id]->infantry_unit_id :
		monster_res[get_nation(targetNation)->monster_id()]->unit_id;

	int attacker2UnitId = 0;
	if( attacker2StateRecno )
	{
		attacker2UnitId = get_nation(attacker2Nation)->is_human() ?
		race_res[get_nation(attacker2Nation)->race_id]->infantry_unit_id :
		monster_res[get_nation(attacker2Nation)->monster_id()]->unit_id;
	}

	// forced to update the background
	mouse.hide();
	vga.use_back();
	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga.use_front();
	mouse.show();
//	vga.flip();
//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
//	vga.use_front();
//	vga.flip();
	// forced to update the background
	
	//------ determine source & destination locations -------//
	int srcScrnX = attackerStateRecno ? state_array[attackerStateRecno]->center_x : 0;
	int srcScrnY = attackerStateRecno ? state_array[attackerStateRecno]->center_y : 0;
	int destScrnX = state_array[targetStateRecno]->center_x;
	int destScrnY = state_array[targetStateRecno]->center_y;
	int srcScrnX2 = attacker2StateRecno ? state_array[attacker2StateRecno]->center_x : 0;
	int srcScrnY2 = attacker2StateRecno ? state_array[attacker2StateRecno]->center_y : 0;

	//-------- show the attack animation now -----//

	// change the ownership of the nation if attackResult == 1
	// As attackResult is unknown in this stage, 
	// restore it later if attackResult != 1
	int oldNationRecno = 0;
	if( attackerStateRecno )
	{
		oldNationRecno = temp_state_change_nation(targetStateRecno, attackerNation);
		int i, j;
		for (i = 0; i < HFIELD_WIDTH; i++)
			for (j = 0; j < HFIELD_HEIGHT; j++)
				map[i][j] = smoothing_checking(i, j, HFIELD_BIT_SHIFT);
	}
	
	// ####### begin Gilbert 19/3 ########//
	// testing only
//#ifdef DEBUG
	int attackerCount = 0;
	CampaignAnimationUnit attackerArray[2], defender;
	if( attackerStateRecno )
	{
		attackerArray[0].init( attackerUnitId, get_nation(attackerNation)->color_scheme_id,
			srcScrnX, srcScrnY, attackResult );
		attackerCount++;
	}
	defender.init( targetUnitId, get_nation(targetNation)->color_scheme_id, 
		destScrnX, destScrnY, -attackResult );
	if( attacker2StateRecno )
	{
		attackerArray[1].init( attacker2UnitId, get_nation(attacker2Nation)->color_scheme_id,
			srcScrnX2, srcScrnY2, attack2Result );
		attackerCount++;
	}

#ifdef DEBUG
	// for testing single defender mission (no attacker)
//	attackerCount = 0;
//	defender.result = 0;
#endif

	attack_animation(attackerArray, attackerCount, &defender, firstStep, lastStep );

//#else
//	attack_animation(attackerUnitId, targetUnitId,
//						  get_nation(attackerNation)->color_scheme_id,
//						  get_nation(targetNation)->color_scheme_id,
//						  srcScrnX, srcScrnY, destScrnX, destScrnY, attackResult, firstStep, lastStep);
//#endif
	// ####### end Gilbert 19/3 ########//
	
	//--------- change state nation ----------//

	// restore the nation if attackResult != 1
	if( attackResult!=1 )
	{
		temp_state_change_nation(targetStateRecno, targetNation);
		int i, j;
		for (i = 0; i < HFIELD_WIDTH; i++)
			for (j = 0; j < HFIELD_HEIGHT; j++)
				map[i][j] = smoothing_checking(i, j, HFIELD_BIT_SHIFT);
	}
	else
	{
		if (oldNationRecno)
			del_nation(oldNationRecno);
	}
	
	temp_close_res();
}
//-------- End of function Campaign::attack_state ------//


//-------- Begin of function Campaign::rebel_attack_state ------//
//
// <int> stateRecno 		    - the recno of the state where rebellion happens
// <int> attackResult    	 - -1 the state government wins
//										 1 the rebels win
//									    0 result unknown
// [int] firstStep          - the first step of the attack sequence
// [int] lastStep 			 - the last step of the attack sequence
//										(default:0, display all steps of the sequence)
//
// step 1: move
// step 2: attack
// step 3: attack until one is killed.
//
void Campaign::rebel_attack_state(int stateRecno, int attackResult, int firstStep, int lastStep)
{
	int nationRecno = state_array[stateRecno]->campaign_nation_recno;

	err_when( !get_nation(nationRecno)->is_human() );		// the state must be owned by humans

	if( attackResult == -1 )		// the rebels win
		state_array[stateRecno]->campaign_nation_recno = 0;

	temp_open_res();

	RaceInfo* raceInfo = race_res[get_nation(nationRecno)->race_id];

	//------ determine source & destination locations -------//

	int scrnX = state_array[stateRecno]->center_x;
	int scrnY = state_array[stateRecno]->center_y;

	//-------- show the attack animation now -----//

	int unitSpace = 46 * state_array.max_x_loc / ( MAIN_MAP_X2-MAIN_MAP_X1+1 );

	mouse.hide();
	vga.use_back();
	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();
	vga.use_front();
	mouse.show();

//	vga.use_back();
//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
//	vga.flip();
//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
//	vga.use_front();
//	vga.flip();

	attack_animation( raceInfo->civilian_unit_id, raceInfo->infantry_unit_id,
							0, get_nation(nationRecno)->color_scheme_id,
							scrnX-unitSpace/2, scrnY, scrnX+unitSpace/2, scrnY, attackResult, firstStep, lastStep);

	temp_close_res();
}
//-------- End of function Campaign::rebel_attack_state ------//


//-------- Begin of static function temp_open_res ------//
//
static void temp_open_res()
{
	if( !game.init_flag )		// if the game has already been initialized, we don't have to initialize the resource
	{
		sprite_res.init();
		sprite_frame_res.init();
	}
}
//-------- End of static function temp_open_res ------//


//-------- Begin of static function temp_close_res ------//
//
static void temp_close_res()
{
	if( !game.init_flag )		// if the game has already been initialized
	{
		sprite_frame_res.deinit();
		sprite_res.deinit();
	}
}
//-------- End of static function temp_close_res ------//


//-------- Begin of function Campaign::attack_animation ------//
//
// <int> attackerUnitId - unit id. of the attacker
// <int> targetUnitId   - unit id. of the target
// <int> attackerColorSchemeId - attacker color scheme id.
// <int> targetColorSchemeId   - target color scheme id.
// <int> srcScrnX, srcScrnY   - the starting location of the attacker
// <int> destScrnX, destScrnY - the starting location of the target
// <int> attackResult			- attack result
// <int> firstStep, lastStep  - the first & last step of the animation
//
void Campaign::attack_animation(int attackerUnitId, int targetUnitId,
					int attackerColorSchemeId, int targetColorSchemeId,
					int srcScrnX, int srcScrnY, int destScrnX, int destScrnY,
					int attackResult, int firstStep, int lastStep)
{

#ifdef DEBUG
	int backupResult = attackResult;
	int backupLastStep = lastStep;
//	if( !attackResult )		// testing to see the whole animation
//	{
//		attackResult = 1;
//		lastStep = 3;
//	}
#endif

	// ------ setup clip window --------//

	int clipX1 = 0;
	int clipY1 = 0;
	int clipX2 = VGA_WIDTH -1;
	int clipY2 = VGA_HEIGHT -1;

	// ------ transform to screen coordinate ---------//
	int srcHeight = random_terrain_map.get_pix(srcScrnX, srcScrnY);
	int destHeight = random_terrain_map.get_pix(destScrnX, destScrnY);

	srcScrnX =(((MAP_LOWER_LEFT_CORNER_X - MAP_LOWER_RIGHT_CORNER_X) * srcScrnX / state_array.max_x_loc +MAP_LOWER_RIGHT_CORNER_X) - 
				  ((MAP_UPPER_LEFT_CORNER_X - MAP_UPPER_RIGHT_CORNER_X) * srcScrnX / state_array.max_x_loc +MAP_UPPER_RIGHT_CORNER_X))
				  * srcScrnY / state_array.max_y_loc +
				  ((MAP_UPPER_LEFT_CORNER_X - MAP_UPPER_RIGHT_CORNER_X) * srcScrnX / state_array.max_x_loc +MAP_UPPER_RIGHT_CORNER_X);

	srcScrnY = (MAP_LOWER_RIGHT_CORNER_Y - MAP_UPPER_RIGHT_CORNER_Y) * srcScrnY / state_array.max_y_loc
					+MAP_UPPER_RIGHT_CORNER_Y;// - (srcHeight/3);
	
	destScrnX =(((MAP_LOWER_LEFT_CORNER_X - MAP_LOWER_RIGHT_CORNER_X) * destScrnX / state_array.max_x_loc +MAP_LOWER_RIGHT_CORNER_X) - 
				  ((MAP_UPPER_LEFT_CORNER_X - MAP_UPPER_RIGHT_CORNER_X) * destScrnX / state_array.max_x_loc +MAP_UPPER_RIGHT_CORNER_X))
				  * destScrnY/ state_array.max_y_loc +
				  ((MAP_UPPER_LEFT_CORNER_X - MAP_UPPER_RIGHT_CORNER_X) * destScrnX / state_array.max_x_loc +MAP_UPPER_RIGHT_CORNER_X);

	destScrnY = (MAP_LOWER_RIGHT_CORNER_Y - MAP_UPPER_RIGHT_CORNER_Y) * destScrnY / state_array.max_y_loc
					+MAP_UPPER_RIGHT_CORNER_Y;// - (destHeight/3);

//	srcScrnX = MAIN_MAP_X1 + ( MAIN_MAP_X2-MAIN_MAP_X1+1 ) * srcScrnX / state_array.max_x_loc;
//	srcScrnY = MAIN_MAP_Y1 + ( MAIN_MAP_Y2-MAIN_MAP_Y1+1 ) * srcScrnY / state_array.max_y_loc;
//	destScrnX = MAIN_MAP_X1 + ( MAIN_MAP_X2-MAIN_MAP_X1+1 ) * destScrnX / state_array.max_x_loc;
//	destScrnY = MAIN_MAP_Y1 + ( MAIN_MAP_Y2-MAIN_MAP_Y1+1 ) * destScrnY / state_array.max_y_loc;

	// distance between two point
	int distScrn = misc.diagonal_distance(srcScrnX,srcScrnY, destScrnX,destScrnY );

	Sprite attackerUnit;
	attackerUnit.init( unit_res[attackerUnitId]->sprite_id, 0, 0 );
	attackerUnit.cur_x = srcScrnX - (attackerUnit.sprite_info->loc_width * ZOOM_LOC_X_WIDTH + attackerUnit.sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;
	attackerUnit.cur_y = srcScrnY - (attackerUnit.sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + attackerUnit.sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;
	attackerUnit.go_x = destScrnX - (attackerUnit.sprite_info->loc_width * ZOOM_LOC_X_WIDTH + attackerUnit.sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;
	attackerUnit.go_y = destScrnY - (attackerUnit.sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + attackerUnit.sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;
	attackerUnit.set_dir(srcScrnX, srcScrnY, destScrnX, destScrnY);
	attackerUnit.cur_dir = attackerUnit.final_dir
		= (attackerUnit.final_dir + 7) % 8;	// cancel the adjustment made by display_dir

	Sprite targetUnit;
	targetUnit.init( unit_res[targetUnitId]->sprite_id, 0, 0 );
	targetUnit.cur_x = destScrnX - (targetUnit.sprite_info->loc_width * ZOOM_LOC_X_WIDTH + targetUnit.sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;
	targetUnit.cur_y = destScrnY - (targetUnit.sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + targetUnit.sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;
	targetUnit.set_dir(destScrnX, destScrnY, srcScrnX, srcScrnY);
	targetUnit.cur_dir = targetUnit.final_dir
		= (targetUnit.final_dir + 7) % 8;	// cancel the adjustment made by display_dir

	// ----- declare save buffer -------//

	HelpSaveScreen attackerSaveScr, targetSaveScr;
	attackerSaveScr.init();
	targetSaveScr.init();

	int attackerX1, attackerY1, attackerX2, attackerY2;
	int targetX1, targetY1, targetX2, targetY2;
	int lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2;
	int lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2;
	lastAttackerX1 = -2000;
	lastTargetX1 = -2000;

	// ----- first step --------//

	attackerUnit.cur_action = SPRITE_MOVE;
	attackerUnit.cur_frame = 1;
	targetUnit.cur_action = SPRITE_IDLE;
	targetUnit.cur_frame = 1;

	while( abs( attackerUnit.go_x - attackerUnit.cur_x ) + 
		abs( attackerUnit.go_y - attackerUnit.cur_y ) > 64 )
	{
		// display sprites

		if( (!firstStep || firstStep <= 1 ) && (!lastStep || 1 <= lastStep) )
		{
			DWORD startTime = misc.get_time();

			// -------- save area first --------//

			if( attackerUnit.get_abs_rect( attackerUnit.cur_x, attackerUnit.cur_y, 
				clipX1, clipY1, clipX2, clipY2, &attackerX1, &attackerY1, &attackerX2, &attackerY2) )
			{
				attackerSaveScr.save_scr(attackerX1, attackerY1, attackerX2, attackerY2, &vga_back );
			}
			else
			{
				attackerSaveScr.clear();
			}

			if( targetUnit.get_abs_rect( targetUnit.cur_x, targetUnit.cur_y, 
				clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
			{
				targetSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
			}
			else
			{
				targetSaveScr.clear();
			}

			// ------ draw sprite ---------//

			if( attackerY2 >= targetY2 )
			{
				targetUnit.draw_abs( targetColorSchemeId, 
					targetUnit.cur_x, targetUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
			}
			else
			{
				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
				targetUnit.draw_abs( targetColorSchemeId, 
					targetUnit.cur_x, targetUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
			}

			// ----- blt to front --------//

			if( lastAttackerX1 >= 0 )	// restore front draw in last frame
				vga_util.blt_buf( lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2, 0 );
			if( lastTargetX1 >= 0 )
				vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
			vga_util.blt_buf( attackerX1, attackerY1, attackerX2, attackerY2, 0 );
			vga_util.blt_buf( targetX1, targetY1, targetX2, targetY2, 0 );

			lastAttackerX1 = attackerX1;
			lastAttackerY1 = attackerY1;
			lastAttackerX2 = attackerX2;
			lastAttackerY2 = attackerY2;
			lastTargetX1 = targetX1;
			lastTargetY1 = targetY1;
			lastTargetX2 = targetX2;
			lastTargetY2 = targetY2;

			// ----- restore back buffer --------//

			attackerSaveScr.rest_scr(&vga_back);
			targetSaveScr.rest_scr(&vga_back);

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
		}

		// move attacker 

		int closeDist = misc.diagonal_distance( attackerUnit.cur_x, attackerUnit.cur_y,
			attackerUnit.go_x, attackerUnit.go_y );
		attackerUnit.cur_x += attackerUnit.sprite_speed() * (attackerUnit.go_x - attackerUnit.cur_x) / closeDist;
		attackerUnit.cur_y += attackerUnit.sprite_speed() * (attackerUnit.go_y - attackerUnit.cur_y) / closeDist;
		if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_move()->frame_count )
			attackerUnit.cur_frame = 1;

		// animate defender

		if( ++targetUnit.cur_frame > targetUnit.cur_sprite_stop()->frame_count )
			targetUnit.cur_frame = 1;
	}

	// ------- step 2 -------- //

	attackerUnit.cur_action = SPRITE_ATTACK;
	attackerUnit.cur_frame = 1;
	attackerUnit.cur_attack = 0;
	attackerUnit.remain_attack_delay = 0;

	err_when( !unit_res[attackerUnitId]->first_attack );
	int attackerDelay = 
		unit_res.get_attack_info( unit_res[attackerUnitId]->first_attack )->attack_delay;

	targetUnit.cur_action = SPRITE_ATTACK;
	targetUnit.cur_frame = 1;
	targetUnit.cur_attack = 0;
	targetUnit.remain_attack_delay = 0;
	int targetDelay = 
		unit_res.get_attack_info( unit_res[targetUnitId]->first_attack )->attack_delay;

	int attackerFightCount = 5;
	int targetFightCount = 5;

	// if the result is known, win side has larger fightCount
	if( attackResult == 1 )
	{
		attackerFightCount += 20;
	}
	if( attackResult == -1 )
	{
		targetFightCount += 20;
	}

	while( attackerFightCount > 0 && targetFightCount > 0 )
	{
		// display sprites

		if( (!firstStep || firstStep <= 2 ) && (!lastStep || 2 <= lastStep) )
		{
			DWORD startTime = misc.get_time();

			// -------- save area first --------//

			if( attackerUnit.get_abs_rect( attackerUnit.cur_x, attackerUnit.cur_y, 
				clipX1, clipY1, clipX2, clipY2, &attackerX1, &attackerY1, &attackerX2, &attackerY2) )
			{
				attackerSaveScr.save_scr(attackerX1, attackerY1, attackerX2, attackerY2, &vga_back );
			}
			else
			{
				attackerSaveScr.clear();
			}

			if( targetUnit.get_abs_rect( targetUnit.cur_x, targetUnit.cur_y, 
				clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
			{
				targetSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
			}
			else
			{
				targetSaveScr.clear();
			}

			// ------ draw sprite ---------//

			if( attackerY2 >= targetY2 )
			{
				targetUnit.draw_abs( targetColorSchemeId, 
					targetUnit.cur_x, targetUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
			}
			else
			{
				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
				targetUnit.draw_abs( targetColorSchemeId, 
					targetUnit.cur_x, targetUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );
			}

			// ----- blt to front --------//

			if( lastAttackerX1 >= 0 )	// restore front draw in last frame
				vga_util.blt_buf( lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2, 0 );
			if( lastTargetX1 >= 0 )
				vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
			vga_util.blt_buf( attackerX1, attackerY1, attackerX2, attackerY2, 0 );
			vga_util.blt_buf( targetX1, targetY1, targetX2, targetY2, 0 );

			lastAttackerX1 = attackerX1;
			lastAttackerY1 = attackerY1;
			lastAttackerX2 = attackerX2;
			lastAttackerY2 = attackerY2;
			lastTargetX1 = targetX1;
			lastTargetY1 = targetY1;
			lastTargetX2 = targetX2;
			lastTargetY2 = targetY2;

			// ----- restore back buffer --------//

			attackerSaveScr.rest_scr(&vga_back);
			targetSaveScr.rest_scr(&vga_back);

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
		}

		// animate attacker 
		if( attackerUnit.remain_attack_delay > 0 )
		{
			--attackerUnit.remain_attack_delay;
		}
		else if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_attack()->frame_count )
		{
			attackerUnit.cur_frame = 1;
			attackerUnit.remain_attack_delay = attackerDelay;
			--attackerFightCount;
		}

		// animate defender

		if( targetUnit.remain_attack_delay > 0 )
		{
			--targetUnit.remain_attack_delay;
		}
		else if( ++targetUnit.cur_frame > targetUnit.cur_sprite_attack()->frame_count )
		{
			targetUnit.cur_frame = 1;
			targetUnit.remain_attack_delay = targetDelay;
			--targetFightCount;
		}
	}

	// ---------- third step ---------//

	if( attackResult != 0 )
	{
		if( attackResult == -1 )
		{
			attackerUnit.cur_action = SPRITE_DIE;
			attackerUnit.cur_frame = 1;
			attackerUnit.cur_attack = 0;
			attackerUnit.remain_attack_delay = 0;
		}

		if( attackResult == 1 )
		{
			targetUnit.cur_action = SPRITE_DIE;
			targetUnit.cur_frame = 1;
			targetUnit.cur_attack = 0;
			targetUnit.remain_attack_delay = 0;
		}

		int frameCount = 15;

		while( --frameCount > 0 )
		{
			// display sprites

			if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
			{
				DWORD startTime = misc.get_time();

				// -------- save area first --------//

				if( attackerUnit.get_abs_rect( attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2, &attackerX1, &attackerY1, &attackerX2, &attackerY2) )
				{
					attackerSaveScr.save_scr(attackerX1, attackerY1, attackerX2, attackerY2, &vga_back );
				}
				else
				{
					attackerSaveScr.clear();
				}

				if( targetUnit.get_abs_rect( targetUnit.cur_x, targetUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
				{
					targetSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
				}
				else
				{
					targetSaveScr.clear();
				}

				// ------ draw sprite ---------//

				if( attackerY2 >= targetY2 )
				{
					targetUnit.draw_abs( targetColorSchemeId, 
						targetUnit.cur_x, targetUnit.cur_y, 
						clipX1, clipY1, clipX2, clipY2 );
					attackerUnit.draw_abs( attackerColorSchemeId, 
						attackerUnit.cur_x, attackerUnit.cur_y, 
						clipX1, clipY1, clipX2, clipY2 );
				}
				else
				{
					attackerUnit.draw_abs( attackerColorSchemeId, 
						attackerUnit.cur_x, attackerUnit.cur_y, 
						clipX1, clipY1, clipX2, clipY2 );
					targetUnit.draw_abs( targetColorSchemeId, 
						targetUnit.cur_x, targetUnit.cur_y, 
						clipX1, clipY1, clipX2, clipY2 );
				}

				// ----- blt to front --------//

				if( lastAttackerX1 >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2, 0 );
				if( lastTargetX1 >= 0 )
					vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
				vga_util.blt_buf( attackerX1, attackerY1, attackerX2, attackerY2, 0 );
				vga_util.blt_buf( targetX1, targetY1, targetX2, targetY2, 0 );

				lastAttackerX1 = attackerX1;
				lastAttackerY1 = attackerY1;
				lastAttackerX2 = attackerX2;
				lastAttackerY2 = attackerY2;
				lastTargetX1 = targetX1;
				lastTargetY1 = targetY1;
				lastTargetX2 = targetX2;
				lastTargetY2 = targetY2;

				// ----- restore back buffer --------//

				attackerSaveScr.rest_scr(&vga_back);
				targetSaveScr.rest_scr(&vga_back);

				sys.yield();
				sys.blt_virtual_buf();

				DWORD endTime = misc.get_time();
				if( mouse.left_press || endTime - startTime >= 50 )
					Sleep(1);
				else
					Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
			}

			// animate attacker 
			switch( attackerUnit.cur_action )
			{
			case SPRITE_ATTACK:
				if( attackerUnit.remain_attack_delay > 0 )
				{
					--attackerUnit.remain_attack_delay;
					if( attackerUnit.remain_attack_delay == 0 )
					{
						attackerUnit.cur_action = SPRITE_IDLE;
						attackerUnit.cur_frame = 1;
					}
				}
				else if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_attack()->frame_count )
				{
					attackerUnit.cur_frame = 1;
					attackerUnit.remain_attack_delay = attackerDelay;
					if( attackerUnit.remain_attack_delay == 0 )
						attackerUnit.cur_action = SPRITE_IDLE;
				}
				break;

			case SPRITE_IDLE:
				if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_stop()->frame_count )
				{
					attackerUnit.cur_frame = 1;
				}
				break;

			case SPRITE_DIE:
				if( attackerUnit.cur_frame < attackerUnit.cur_sprite_die()->frame_count )
				{
					++attackerUnit.cur_frame;
				}
				break;
			}

			// animate target 
			switch( targetUnit.cur_action )
			{
			case SPRITE_ATTACK:
				if( targetUnit.remain_attack_delay > 0 )
				{
					--targetUnit.remain_attack_delay;
					if( targetUnit.remain_attack_delay == 0 )
					{
						targetUnit.cur_action = SPRITE_IDLE;
						targetUnit.cur_frame = 1;
					}
				}
				else if( ++targetUnit.cur_frame > targetUnit.cur_sprite_attack()->frame_count )
				{
					targetUnit.cur_frame = 1;
					targetUnit.remain_attack_delay = targetDelay;
					if( targetUnit.remain_attack_delay == 0 )
						targetUnit.cur_action = SPRITE_IDLE;
				}
				break;

			case SPRITE_IDLE:
				if( ++targetUnit.cur_frame > targetUnit.cur_sprite_stop()->frame_count )
				{
					targetUnit.cur_frame = 1;
				}
				break;

			case SPRITE_DIE:
				if( targetUnit.cur_frame < targetUnit.cur_sprite_die()->frame_count )
				{
					++targetUnit.cur_frame;
				}
				break;
			}
		}

	}

	// --------- third step second part ---------//

	if( attackResult == 1 )
	{
		// attackerUnit and go to take the state

		attackerUnit.cur_action = SPRITE_MOVE;
		attackerUnit.cur_frame = 1;

		int closeDist;

		while( (closeDist = misc.diagonal_distance( attackerUnit.cur_x, attackerUnit.cur_y,
			attackerUnit.go_x, attackerUnit.go_y)) > attackerUnit.sprite_speed() )
		{
			// display sprites

			if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
			{
				DWORD startTime = misc.get_time();

				// -------- save area first --------//

				if( attackerUnit.get_abs_rect( attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2, &attackerX1, &attackerY1, &attackerX2, &attackerY2) )
				{
					attackerSaveScr.save_scr(attackerX1, attackerY1, attackerX2, attackerY2, &vga_back );
				}
				else
				{
					attackerSaveScr.clear();
				}

				targetSaveScr.clear();

				// ------ draw sprite ---------//

				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );

				// ----- blt to front --------//

				if( lastAttackerX1 >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2, 0 );
				if( lastTargetX1 >= 0 )
					vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
				vga_util.blt_buf( attackerX1, attackerY1, attackerX2, attackerY2, 0 );
				vga_util.blt_buf( targetX1, targetY1, targetX2, targetY2, 0 );

				lastAttackerX1 = attackerX1;
				lastAttackerY1 = attackerY1;
				lastAttackerX2 = attackerX2;
				lastAttackerY2 = attackerY2;
				lastTargetX1 = targetX1;
				lastTargetY1 = targetY1;
				lastTargetX2 = targetX2;
				lastTargetY2 = targetY2;

				// ----- restore back buffer --------//

				attackerSaveScr.rest_scr(&vga_back);

				sys.yield();
				sys.blt_virtual_buf();

				DWORD endTime = misc.get_time();
				if( mouse.left_press || endTime - startTime >= 50 )
					Sleep(1);
				else
					Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
			}

			// move attacker 

			attackerUnit.cur_x += attackerUnit.sprite_speed() * (attackerUnit.go_x - attackerUnit.cur_x) / closeDist;
			attackerUnit.cur_y += attackerUnit.sprite_speed() * (attackerUnit.go_y - attackerUnit.cur_y) / closeDist;
			if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_move()->frame_count )
				attackerUnit.cur_frame = 1;
		}
	}

	// --------- third step third part ---------//

	if( attackResult == 1 )
	{
		// forced to update the background
	//	vga.use_back();
	//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	//	vga.flip();
	//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	//	vga.use_front();
	//	vga.flip();
		mouse.hide();
		vga.use_back();
		disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		vga.use_front();
		mouse.show();
		// forced to update the background

		// attackerUnit and go to take the state

		attackerUnit.cur_action = SPRITE_IDLE;
		attackerUnit.cur_frame = 1;

		int frameCount = 0;

		while( ++frameCount < 20 )
		{
			// display sprites

			if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
			{
				DWORD startTime = misc.get_time();

				// -------- save area first --------//

				if( attackerUnit.get_abs_rect( attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2, &attackerX1, &attackerY1, &attackerX2, &attackerY2) )
				{
					attackerSaveScr.save_scr(attackerX1, attackerY1, attackerX2, attackerY2, &vga_back );
				}
				else
				{
					attackerSaveScr.clear();
				}

				// ------ draw sprite ---------//

				attackerUnit.draw_abs( attackerColorSchemeId, 
					attackerUnit.cur_x, attackerUnit.cur_y, 
					clipX1, clipY1, clipX2, clipY2 );

				// ----- blt to front --------//

				if( frameCount == 1 )
				{
					// first frame only
					vga_util.blt_buf( MAIN_MAP_X1, MAIN_MAP_Y1, MAIN_MAP_X2, MAIN_MAP_Y2, 0 );
				}

				if( lastAttackerX1 >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1, lastAttackerY1, lastAttackerX2, lastAttackerY2, 0 );
				if( lastTargetX1 >= 0 )
					vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
				vga_util.blt_buf( attackerX1, attackerY1, attackerX2, attackerY2, 0 );
				vga_util.blt_buf( targetX1, targetY1, targetX2, targetY2, 0 );

				lastAttackerX1 = attackerX1;
				lastAttackerY1 = attackerY1;
				lastAttackerX2 = attackerX2;
				lastAttackerY2 = attackerY2;
				lastTargetX1 = targetX1;
				lastTargetY1 = targetY1;
				lastTargetX2 = targetX2;
				lastTargetY2 = targetY2;

				// ----- restore back buffer --------//

				attackerSaveScr.rest_scr(&vga_back);

				sys.yield();
				sys.blt_virtual_buf();

				DWORD endTime = misc.get_time();
				if( mouse.left_press || endTime - startTime >= 50 )
					Sleep(1);
				else
					Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
			}

			// animate attacker 

			if( ++attackerUnit.cur_frame > attackerUnit.cur_sprite_stop()->frame_count )
				attackerUnit.cur_frame = 1;
		}
	}

#ifdef DEBUG
	attackResult = backupResult;
	lastStep = backupLastStep;
#endif
}
//-------- End of function Campaign::attack_animation ------//


// ------- define struct SortRec for sorting ---------//

struct SortRec
{
	int rec_no;
	int sort_key;

	void init( int rec, int key )
	{
		rec_no = rec;
		sort_key = key;
	}
};

// -------- define sort function of SortRec --------//

int sortRecFunc( const void *a, const void *b)
{
	if( ((SortRec *)a)->sort_key == ((SortRec *)b)->sort_key )
		return ((SortRec *)a)->rec_no - ((SortRec *)b)->rec_no;
	return ((SortRec *)a)->sort_key - ((SortRec *)b)->sort_key;
}

// ------- Begin of function Campaign::attack_animation --------//
//
//
//
void Campaign::attack_animation( CampaignAnimationUnit *attackerArray,
					int attackerCount, CampaignAnimationUnit *defender,
					int firstStep, int lastStep )
{
	const int MAX_ATTACKER = 2;
	err_when( attackerCount > MAX_ATTACKER );

	int a;

	// ------ setup clip window --------//

	int clipX1 = 0;
	int clipY1 = 0;
	int clipX2 = VGA_WIDTH -1;
	int clipY2 = VGA_HEIGHT -1;

	// ------ calc distance between two point ------//

	int distScrn[MAX_ATTACKER];
	Sprite attackerUnit[MAX_ATTACKER], defenderUnit;
	int expectedSteps[MAX_ATTACKER];
	int attackerAttackDelay[MAX_ATTACKER], defenderAttackDelay;

	// ------ calc defender scrn_x/y ----------//

	err_when( !defender );
	int destScrnX = defender->cur_x;
	int destScrnY = defender->cur_y;

	calculate_terrain_location(destScrnX, destScrnY);					

	// ------- init attacker ------------//

	for( a = 0; a < attackerCount; ++a )
	{
		attackerUnit[a].init( unit_res[attackerArray[a].unit_id]->sprite_id, 0, 0 );

		// ------ calc attacker scrn_x/y ----------//

		int srcScrnX = attackerArray[a].cur_x;
		int srcScrnY = attackerArray[a].cur_y;

		calculate_terrain_location(srcScrnX, srcScrnY);					

		attackerArray[a].scrn_x = srcScrnX
			- (attackerUnit[a].sprite_info->loc_width * ZOOM_LOC_X_WIDTH + attackerUnit[a].sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;
		attackerArray[a].scrn_y = srcScrnY
			- (attackerUnit[a].sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + attackerUnit[a].sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;

		// ------ calc attacker go_scrn_x/y ---------//

		attackerArray[a].go_scrn_x = destScrnX - (attackerUnit[a].sprite_info->loc_width * ZOOM_LOC_X_WIDTH + attackerUnit[a].sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;
		attackerArray[a].go_scrn_y = destScrnY - (attackerUnit[a].sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + attackerUnit[a].sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;

		// ------- init sprite ----------//

		attackerUnit[a].cur_x = attackerArray[a].cur_x;
		attackerUnit[a].cur_y = attackerArray[a].cur_y;
		attackerUnit[a].go_x = defender->cur_x;
		attackerUnit[a].go_y = defender->cur_y;
		attackerUnit[a].set_dir( attackerArray[a].scrn_x, attackerArray[a].scrn_y, destScrnX, destScrnY );
		attackerUnit[a].cur_dir = attackerUnit[a].final_dir
			= (attackerUnit[a].final_dir + 7) % 8;	// cancel the adjustment made by display_dir
		attackerAttackDelay[a] = unit_res.get_attack_info( unit_res[attackerArray[a].unit_id]->first_attack )->attack_delay;

		// ------ estimate expectedSteps[a] --------//

		distScrn[a] = misc.diagonal_distance( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
			attackerArray[a].go_scrn_x, attackerArray[a].go_scrn_y );

		expectedSteps[a] = distScrn[a] / attackerUnit[a].sprite_speed();
	}

	int maxExpectedSteps = 0;
	for( a = 0; a < attackerCount; ++a )
	{
		if( expectedSteps[a] > maxExpectedSteps )
			maxExpectedSteps = expectedSteps[a];
	}

	// ------ calc defender scrn_x/y ----------//

	defenderUnit.init( unit_res[defender->unit_id]->sprite_id, 0, 0 );

	defender->scrn_x = defender->go_scrn_x = destScrnX
		- (defenderUnit.sprite_info->loc_width * ZOOM_LOC_X_WIDTH + defenderUnit.sprite_info->loc_height * ZOOM_LOC_Y_WIDTH) / 2;

	defender->scrn_y = defender->go_scrn_y = destScrnY
		- (defenderUnit.sprite_info->loc_width * ZOOM_LOC_X_HEIGHT + defenderUnit.sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT) / 2;

	// -------- init defender ------------//

	defenderUnit.cur_x = defender->cur_x;
	defenderUnit.cur_y = defender->cur_y;
	defenderUnit.go_x = defenderUnit.cur_x;
	defenderUnit.go_y = defenderUnit.cur_y;
	if( attackerCount > 0 )
	{
		defenderUnit.set_dir( defender->scrn_x, defender->scrn_y, attackerArray[0].scrn_x, attackerArray[0].scrn_y );
		defenderUnit.cur_dir = defenderUnit.final_dir
			= (defenderUnit.final_dir + 7) % 8;	// cancel the adjustment made by display_dir
	}
	else
	{
		// if no attacker, face south
		defenderUnit.cur_dir = defenderUnit.final_dir = 3;
	}
	defenderAttackDelay = unit_res.get_attack_info( unit_res[defender->unit_id]->first_attack )->attack_delay;

	// ----- declare save buffer -------//

	HelpSaveScreen attackerSaveScr[MAX_ATTACKER], defenderSaveScr;
	for( a = 0; a < attackerCount; ++a )
		attackerSaveScr[a].init();
	defenderSaveScr.init();

	int attackerX1[MAX_ATTACKER], attackerY1[MAX_ATTACKER], attackerX2[MAX_ATTACKER], attackerY2[MAX_ATTACKER];
	int targetX1, targetY1, targetX2, targetY2;
	int lastAttackerX1[MAX_ATTACKER], lastAttackerY1[MAX_ATTACKER], lastAttackerX2[MAX_ATTACKER], lastAttackerY2[MAX_ATTACKER];
	int lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2;
	for(a = 0; a < attackerCount; ++a )
		lastAttackerX1[a] = -2000;
	lastTargetX1 = -2000;

	// ------ first step ---------//
	// people are stopped at first, then attacker will approacher the defender
	// such that all attacker will reach the defender together

	for( a = 0; a < attackerCount; ++a )
	{
		attackerUnit[a].cur_action = SPRITE_IDLE;
		attackerUnit[a].cur_frame = 1;
	}
	defenderUnit.cur_action = SPRITE_IDLE;
	defenderUnit.cur_frame = 1;

	int stepCounter = 0;
	int waitCount = attackerCount;		// initial state
	int moveCount = 0;						// moving state
	int restCount = 0;						// rest state

	while( restCount < attackerCount )
	{
		// display sprites

		if( (!firstStep || firstStep <= 1 ) && (!lastStep || 1 <= lastStep) )
		{
			DWORD startTime = misc.get_time();

			// -------- save area first --------//

			for( a = 0; a < attackerCount; ++a )
			{
				if( attackerUnit[a].get_abs_rect( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
					clipX1, clipY1, clipX2, clipY2, &attackerX1[a], &attackerY1[a], &attackerX2[a], &attackerY2[a]) )
				{
					attackerSaveScr[a].save_scr(attackerX1[a], attackerY1[a], attackerX2[a], attackerY2[a], &vga_back );
				}
				else
				{
					attackerSaveScr[a].clear();
				}
			}	// end for

			if( defenderUnit.get_abs_rect(defender->scrn_x, defender->scrn_y,
				clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
			{
				defenderSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
			}
			else
			{
				defenderSaveScr.clear();
			}

			// ---------- sort by attackerY2 or targetY2 -----------//

			SortRec sortArray[MAX_ATTACKER+1];		// include defender
			for( a = 0; a < attackerCount; ++a )
				sortArray[a].init( a, attackerY2[a] );
			sortArray[a].init( a, targetY2 );
			int sortRecCount = a + 1;
			qsort( sortArray, sortRecCount, sizeof(SortRec), sortRecFunc );

			// ---------- draw sprite -----------//

			for( int i = 0; i < sortRecCount; ++i )
			{
				a = sortArray[i].rec_no;
				if( a < attackerCount )		// draw attacker
				{
					attackerUnit[a].draw_abs( attackerArray[a].color_scheme_id,
						attackerArray[a].scrn_x, attackerArray[a].scrn_y,
						clipX1, clipY1, clipX2, clipY2 );
				}
				else		// draw defender
				{
					defenderUnit.draw_abs( defender->color_scheme_id,
						defender->scrn_x, defender->scrn_y,
						clipX1, clipY1, clipX2, clipY2 );
				}
			}

			// ------- blt to front -------------//

			for( a = 0; a < attackerCount; ++a )
				if( lastAttackerX1[a] >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1[a], lastAttackerY1[a], lastAttackerX2[a], lastAttackerY2[a], 0 );
			if( lastTargetX1 >= 0 )
				vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
			for( a = 0; a < attackerCount; ++a )
				vga_util.blt_buf( lastAttackerX1[a]=attackerX1[a], lastAttackerY1[a]=attackerY1[a], lastAttackerX2[a]=attackerX2[a], lastAttackerY2[a]=attackerY2[a], 0 );
			vga_util.blt_buf( lastTargetX1=targetX1, lastTargetY1=targetY1, lastTargetX2=targetX2, lastTargetY2=targetY2, 0 );

			// ------ restore back buffer ---------//

			for( a = 0; a < attackerCount; ++a )
				attackerSaveScr[a].rest_scr(&vga_back);
			defenderSaveScr.rest_scr(&vga_back);

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
		}

		// move attacker 

		for( a = 0; a < attackerCount; ++a )
		{
			switch( attackerUnit[a].cur_action )
			{
			case SPRITE_IDLE:
				if( stepCounter >= maxExpectedSteps - expectedSteps[a] )		// close attacker start walking later
				{
					attackerUnit[a].cur_action = SPRITE_MOVE;
					attackerUnit[a].cur_frame = 1;

					--waitCount;
					err_when( waitCount < 0 );
					++moveCount;
				}
				else
				{
					if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_stop()->frame_count )
						attackerUnit[a].cur_frame = 1;
				}
				break;
			case SPRITE_MOVE:
				if( abs( attackerArray[a].scrn_x - defender->scrn_x ) 
					<= (attackerUnit[a].sprite_info->loc_width+defenderUnit.sprite_info->loc_width ) * (LOCATE_WIDTH/2)
					&& abs( attackerArray[a].scrn_y - defender->scrn_y) 
					<= (attackerUnit[a].sprite_info->loc_height+defenderUnit.sprite_info->loc_height ) * (LOCATE_HEIGHT/2) )
				{
					// close enough, start to attack
					attackerUnit[a].cur_action = SPRITE_ATTACK;
					attackerUnit[a].cur_frame = 1;
					attackerUnit[a].cur_attack = 0;
					attackerUnit[a].remain_attack_delay = 0;

					--moveCount;
					err_when( moveCount < 0 );
					++restCount;

					// if this the first attacker, set the defender to attack mode as well
					if( restCount == 1 )
					{
						defenderUnit.cur_action = SPRITE_ATTACK;
						defenderUnit.cur_frame = 1;
						defenderUnit.cur_attack = 0;
						defenderUnit.remain_attack_delay = 0;
					}
				}
				else
				{
					int closeDist = misc.diagonal_distance( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
						attackerArray[a].go_scrn_x, attackerArray[a].go_scrn_y );
					attackerArray[a].scrn_x += attackerUnit[a].sprite_speed() * (attackerArray[a].go_scrn_x - attackerArray[a].scrn_x) / closeDist;
					attackerArray[a].scrn_y += attackerUnit[a].sprite_speed() * (attackerArray[a].go_scrn_y - attackerArray[a].scrn_y) / closeDist;
					if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_move()->frame_count )
						attackerUnit[a].cur_frame = 1;
				}
				break;
			case SPRITE_ATTACK:
				{
					// wait for other player to reach this state
					// then waitCount = 0, moveCount = 0, restCount = attackerCount
					if( attackerUnit[a].remain_attack_delay > 0 )
					{
						--attackerUnit[a].remain_attack_delay;
					}
					else if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_attack()->frame_count )
					{
						attackerUnit[a].cur_frame = 1;
						attackerUnit[a].remain_attack_delay = unit_res.get_attack_info( unit_res[attackerArray[a].unit_id]->first_attack )->attack_delay;
					}
				}
				break;
			default:
				err_here();
			}
		}

		// animate defender

		if( defenderUnit.cur_action == SPRITE_IDLE )
		{
			if( ++defenderUnit.cur_frame > defenderUnit.cur_sprite_stop()->frame_count )
				defenderUnit.cur_frame = 1;
		}
		else if( defenderUnit.cur_action == SPRITE_ATTACK )
		{
			if( defenderUnit.remain_attack_delay > 0 )
			{
				--defenderUnit.remain_attack_delay;
			}
			else if( ++defenderUnit.cur_frame > defenderUnit.cur_sprite_attack()->frame_count )
			{
				defenderUnit.cur_frame = 1;
				defenderUnit.remain_attack_delay = unit_res.get_attack_info( unit_res[defender->unit_id]->first_attack )->attack_delay;
			}
		}

		stepCounter++;
	}

	// -------- step 2 --------//

	// all attackers is in attack action, defender face attackerUnit[0]

	int attackerFightCount[MAX_ATTACKER];
	for( a = 0; a < attackerCount; ++a )
		attackerFightCount[a] = 5;		// number of cycles of attack animation to complete
	int defenderFightCount = 5;
	int finishedAttack = attackerCount + 1;		// plus one for the defender

	// if the result is known, win side has larger fightCount

	while(finishedAttack > 0 )
	{
		// display sprites

		if( (!firstStep || firstStep <= 2 ) && (!lastStep || 2 <= lastStep) )
		{
			DWORD startTime = misc.get_time();

			// -------- save area first --------//

			for( a = 0; a < attackerCount; ++a )
			{
				if( attackerUnit[a].get_abs_rect( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
					clipX1, clipY1, clipX2, clipY2, &attackerX1[a], &attackerY1[a], &attackerX2[a], &attackerY2[a]) )
				{
					attackerSaveScr[a].save_scr(attackerX1[a], attackerY1[a], attackerX2[a], attackerY2[a], &vga_back );
				}
				else
				{
					attackerSaveScr[a].clear();
				}
			}	// end for

			if( defenderUnit.get_abs_rect(defender->scrn_x, defender->scrn_y,
				clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
			{
				defenderSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
			}
			else
			{
				defenderSaveScr.clear();
			}

			// ---------- sort by attackerY2 or targetY2 -----------//

			SortRec sortArray[MAX_ATTACKER+1];		// include defender
			for( a = 0; a < attackerCount; ++a )
				sortArray[a].init( a, attackerY2[a] );
			sortArray[a].init( a, targetY2 );
			int sortRecCount = a + 1;
			qsort( sortArray, sortRecCount, sizeof(SortRec), sortRecFunc );

			// ---------- draw sprite -----------//

			for( int i = 0; i < sortRecCount; ++i )
			{
				a = sortArray[i].rec_no;
				if( a < attackerCount )		// draw attacker
				{
					attackerUnit[a].draw_abs( attackerArray[a].color_scheme_id,
						attackerArray[a].scrn_x, attackerArray[a].scrn_y,
						clipX1, clipY1, clipX2, clipY2 );
				}
				else		// draw defender
				{
					defenderUnit.draw_abs( defender->color_scheme_id,
						defender->scrn_x, defender->scrn_y,
						clipX1, clipY1, clipX2, clipY2 );
				}
			}

			// ------- blt to front -------------//

			for( a = 0; a < attackerCount; ++a )
				if( lastAttackerX1[a] >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1[a], lastAttackerY1[a], lastAttackerX2[a], lastAttackerY2[a], 0 );
			if( lastTargetX1 >= 0 )
				vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
			for( a = 0; a < attackerCount; ++a )
				vga_util.blt_buf( lastAttackerX1[a]=attackerX1[a], lastAttackerY1[a]=attackerY1[a], lastAttackerX2[a]=attackerX2[a], lastAttackerY2[a]=attackerY2[a], 0 );
			vga_util.blt_buf( lastTargetX1=targetX1, lastTargetY1=targetY1, lastTargetX2=targetX2, lastTargetY2=targetY2, 0 );

			// ------ restore back buffer ---------//

			for( a = 0; a < attackerCount; ++a )
				attackerSaveScr[a].rest_scr(&vga_back);
			defenderSaveScr.rest_scr(&vga_back);

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
		}

		// animate attacker
		for( a = 0; a < attackerCount; ++a )
		{
			err_when( attackerUnit[a].cur_action != SPRITE_ATTACK );
			if( attackerUnit[a].remain_attack_delay > 0 )
			{
				--attackerUnit[a].remain_attack_delay;
			}
			else if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_attack()->frame_count )
			{
				attackerUnit[a].cur_frame = 1;
				attackerUnit[a].remain_attack_delay = attackerAttackDelay[a];
				if( --attackerFightCount[a] == 0 )	// must use == , instead of <=, so finishedAttack will not be reduced twice
				{
					finishedAttack--;
				}
			}
		}

		// animate defender

		if( defenderUnit.cur_action == SPRITE_ATTACK )
		{
			if( defenderUnit.remain_attack_delay > 0 )
			{
				--defenderUnit.remain_attack_delay;
			}
			else if( ++defenderUnit.cur_frame > defenderUnit.cur_sprite_attack()->frame_count )
			{
				defenderUnit.cur_frame = 1;
				defenderUnit.remain_attack_delay = defenderAttackDelay;
				if( defenderFightCount > 0 && --defenderFightCount == 0 )	// must use == , instead of <=, so finishedAttack will not be reduced twice
					finishedAttack--;
			}
		}
		else if( defenderUnit.cur_action == SPRITE_IDLE )
		{
			// when no attacker, it is remains idle
			if( --defenderFightCount == 0 )	// must use == , instead of <=, so finishedAttack will not be reduced twice
			{
				finishedAttack--;
			}
		}
	}

	// --------- third step ---------//

	// examine which attacker die, defender attack him first

	int attackerDieCount = 0;
	int defenderDieCount = 0;
	for( a = 0; a < attackerCount; ++a )
		if( attackerArray[a].result == -1 )
			attackerDieCount++;
	if( defender->result == -1 )
		defenderDieCount++;
	int attackerFinishedDieTarget = attackerDieCount;
	int defenderFinishedDieTarget = defenderDieCount;
	int attackerFinishedDieCount = 0;
	int defenderFinishedDieCount = 0;
	char attackerFinishedDieFlags[MAX_ATTACKER];
	memset( attackerFinishedDieFlags, 0, sizeof(attackerFinishedDieFlags) );
	char defenderFinishedDieFlag = 0;
	int HP_INIT = 3;
	int defenderTargetHP = HP_INIT;
	int defenderHP = 1;

	err_when( attackerDieCount >= attackerCount && defenderDieCount == 1); // all people die

	// defender attack the first attacker to be killed

	if( attackerDieCount > 0 )
	{
		for( a = 0; a < attackerCount; ++a )
		{
			if( attackerArray[a].result == -1 )
			{
				defenderUnit.set_dir(defender->scrn_x, defender->scrn_y,
					attackerArray[a].scrn_x, attackerArray[a].scrn_y);
				defenderUnit.cur_dir = defenderUnit.final_dir
					= (defenderUnit.final_dir + 7) % 8;	// cancel the adjustment made by display_dir
				break;
			}
		}
	}

	while( attackerFinishedDieCount < attackerFinishedDieTarget || defenderFinishedDieCount < defenderFinishedDieTarget )
	{
		// display sprites

		if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
		{
			DWORD startTime = misc.get_time();

			// -------- save area first --------//

			for( a = 0; a < attackerCount; ++a )
			{
				if( !attackerFinishedDieFlags[a] && 
					attackerUnit[a].get_abs_rect( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
					clipX1, clipY1, clipX2, clipY2, &attackerX1[a], &attackerY1[a], &attackerX2[a], &attackerY2[a]) )
				{
					attackerSaveScr[a].save_scr(attackerX1[a], attackerY1[a], attackerX2[a], attackerY2[a], &vga_back );
				}
				else
				{
					attackerSaveScr[a].clear();
				}
			}	// end for

			if( !defenderFinishedDieFlag &&
				defenderUnit.get_abs_rect(defender->scrn_x, defender->scrn_y,
				clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
			{
				defenderSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
			}
			else
			{
				defenderSaveScr.clear();
			}

			// ---------- sort by attackerY2 or targetY2 -----------//

			SortRec sortArray[MAX_ATTACKER+1];		// include defender
			for( a = 0; a < attackerCount; ++a )
				sortArray[a].init( a, attackerY2[a] );
			sortArray[a].init( a, targetY2 );
			int sortRecCount = a + 1;
			qsort( sortArray, sortRecCount, sizeof(SortRec), sortRecFunc );

			// ---------- draw sprite -----------//

			for( int i = 0; i < sortRecCount; ++i )
			{
				a = sortArray[i].rec_no;
				if( a < attackerCount )		// draw attacker
				{
					if( !attackerFinishedDieFlags[a] )
					{
						attackerUnit[a].draw_abs( attackerArray[a].color_scheme_id,
							attackerArray[a].scrn_x, attackerArray[a].scrn_y,
							clipX1, clipY1, clipX2, clipY2 );
					}
				}
				else		// draw defender
				{
					if( !defenderFinishedDieFlag )
					{
						defenderUnit.draw_abs( defender->color_scheme_id,
							defender->scrn_x, defender->scrn_y,
							clipX1, clipY1, clipX2, clipY2 );
					}
				}
			}

			// ------- blt to front -------------//

			for( a = 0; a < attackerCount; ++a )
				if( lastAttackerX1[a] >= 0 )	// restore front draw in last frame
					vga_util.blt_buf( lastAttackerX1[a], lastAttackerY1[a], lastAttackerX2[a], lastAttackerY2[a], 0 );
			if( lastTargetX1 >= 0 )
				vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
			for( a = 0; a < attackerCount; ++a )
				vga_util.blt_buf( lastAttackerX1[a]=attackerX1[a], lastAttackerY1[a]=attackerY1[a], lastAttackerX2[a]=attackerX2[a], lastAttackerY2[a]=attackerY2[a], 0 );
			vga_util.blt_buf( lastTargetX1=targetX1, lastTargetY1=targetY1, lastTargetX2=targetX2, lastTargetY2=targetY2, 0 );

			// ------ restore back buffer ---------//

			for( a = 0; a < attackerCount; ++a )
				attackerSaveScr[a].rest_scr(&vga_back);
			defenderSaveScr.rest_scr(&vga_back);

			sys.yield();
			sys.blt_virtual_buf();

			DWORD endTime = misc.get_time();
			if( mouse.left_press || endTime - startTime >= 50 )
				Sleep(1);
			else
				Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
		}

		// animate sprite
		// animate attacker
		for( a = 0; a < attackerCount; ++a )
		{
			switch( attackerUnit[a].cur_action )
			{
			case SPRITE_ATTACK:
				if( attackerUnit[a].remain_attack_delay > 0 )
				{
					--attackerUnit[a].remain_attack_delay;
				}
				else if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_attack()->frame_count )
				{
					attackerUnit[a].cur_frame = 1;
					attackerUnit[a].remain_attack_delay = attackerAttackDelay[a];

					// kill defender, but after all attackers who are destined to die die first
					if( attackerDieCount == 0 && defenderDieCount > 0 && --defenderHP <= 0 )
					{
						defenderUnit.cur_action = SPRITE_DIE;
						defenderUnit.cur_frame = 1;
						--defenderDieCount;			// all defender killed
					}

					if( defenderUnit.cur_action == SPRITE_DIE )		// the defender is dead
					{
						attackerUnit[a].cur_action = SPRITE_IDLE;
						attackerUnit[a].cur_frame = 1;
					}
				}
				break;
			case SPRITE_DIE:
				if( attackerUnit[a].cur_frame < attackerUnit[a].cur_sprite_die()->frame_count )
				{
					++attackerUnit[a].cur_frame;
				}
				else if( !attackerFinishedDieFlags[a] )		// after die animation finish, disappear
				{
					attackerFinishedDieFlags[a] = 1;		// set it so won't increase attackerFinishedDieCount again
					attackerFinishedDieCount++;
				}
				break;
			case SPRITE_IDLE:
				if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_stop()->frame_count )
				{
					attackerUnit[a].cur_frame = 1;
				}
				break;
			default:
				err_here();
			}
		}

		// animate defender

		switch( defenderUnit.cur_action )
		{
		case SPRITE_ATTACK:
			if( defenderUnit.remain_attack_delay > 0 )
			{
				--defenderUnit.remain_attack_delay;
			}
			else if( ++defenderUnit.cur_frame > defenderUnit.cur_sprite_attack()->frame_count )
			{
				defenderUnit.cur_frame = 1;
				defenderUnit.remain_attack_delay = defenderAttackDelay;

				if( attackerDieCount > 0 )
				{
					// hit one attacker
					for( a = 0; a < attackerCount; ++a )
					{
						if( attackerArray[a].result == -1 && attackerUnit[a].cur_action != SPRITE_DIE )
						{
							break;
						}
					}

					if( a < attackerCount && --defenderTargetHP <= 0 )
					{
						// kill one attacker

						attackerUnit[a].cur_action = SPRITE_DIE;
						attackerUnit[a].cur_frame = 1;
						--attackerDieCount;

						// select next attack target who will die

						if( attackerDieCount > 0 )
						{
							for( ; a < attackerCount; ++a )
							{
								if( attackerArray[a].result == -1 && attackerUnit[a].cur_action != SPRITE_DIE )
								{
									break;
								}
							}
						}

						if( attackerDieCount == 0 || a >= attackerCount )
						{
							// select next attack target who will not die
							for( a = 0; a < attackerCount && attackerUnit[a].cur_action == SPRITE_DIE; ++a );
						}

						if( a < attackerCount )
						{
							defenderUnit.set_dir(defender->scrn_x, defender->scrn_y,
								attackerArray[a].scrn_x, attackerArray[a].scrn_y);
							defenderUnit.cur_dir = defenderUnit.final_dir
								= (defenderUnit.final_dir + 7) % 8;	// cancel the adjustment made by display_dir
						}

						defenderTargetHP = HP_INIT;		// attack a few hit before attacker dies
						defenderHP = HP_INIT;		// defender can last for a few more hits before die
					}
				}
			}

			if( attackerDieCount == 0 )
			{
				// set to idle if all die
				for( a = 0; a < attackerCount && attackerUnit[a].cur_action == SPRITE_DIE; ++a );
				if( a >= attackerCount )		// all attackers die
				{
					defenderUnit.cur_action = SPRITE_IDLE;
					defenderUnit.cur_frame = 1;
				}
			}

			break;
		case SPRITE_DIE:
			if( defenderUnit.cur_frame < defenderUnit.cur_sprite_die()->frame_count )
			{
				++defenderUnit.cur_frame;
			}
			else if( !defenderFinishedDieFlag )		// after die animation finish, disappear
			{
				defenderFinishedDieFlag = 1;		// set it so won't increase defenderFinishedDieCount again
				defenderFinishedDieCount++;
			}
			break;
		case SPRITE_IDLE:
			if( ++defenderUnit.cur_frame > defenderUnit.cur_sprite_stop()->frame_count )
			{
				defenderUnit.cur_frame = 1;
			}
			break;
		default:
			err_here();
		}
	}

	// ---------- third step second part ---------//

	int newLord = 0;		// first attacker will take the land
	for( newLord = 0; newLord < attackerCount
		&& attackerArray[newLord].result != 1; ++newLord );

	if( newLord < attackerCount )
	{
		// BUGHERE : canceled the current attack action, probably
		attackerUnit[newLord].cur_action = SPRITE_MOVE;
		attackerUnit[newLord].cur_frame = 1;

		int closeDist;

		while( (closeDist = misc.diagonal_distance( attackerArray[newLord].scrn_x, attackerArray[newLord].scrn_y,
			attackerArray[newLord].go_scrn_x, attackerArray[newLord].go_scrn_y)) > attackerUnit[newLord].sprite_speed() )
		{
			// display sprites

			if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
			{
				DWORD startTime = misc.get_time();

				// -------- save area first --------//

				for( a = 0; a < attackerCount; ++a )
				{
					if( !attackerFinishedDieFlags[a] && 
						attackerUnit[a].get_abs_rect( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
						clipX1, clipY1, clipX2, clipY2, &attackerX1[a], &attackerY1[a], &attackerX2[a], &attackerY2[a]) )
					{
						attackerSaveScr[a].save_scr(attackerX1[a], attackerY1[a], attackerX2[a], attackerY2[a], &vga_back );
					}
					else
					{
						attackerSaveScr[a].clear();
					}
				}	// end for

				if( !defenderFinishedDieFlag &&
					defenderUnit.get_abs_rect(defender->scrn_x, defender->scrn_y,
					clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
				{
					defenderSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
				}
				else
				{
					defenderSaveScr.clear();
				}

				// ---------- sort by attackerY2 or targetY2 -----------//

				SortRec sortArray[MAX_ATTACKER+1];		// include defender
				for( a = 0; a < attackerCount; ++a )
					sortArray[a].init( a, attackerY2[a] );
				sortArray[a].init( a, targetY2 );
				int sortRecCount = a + 1;
				qsort( sortArray, sortRecCount, sizeof(SortRec), sortRecFunc );

				// ---------- draw sprite -----------//

				for( int i = 0; i < sortRecCount; ++i )
				{
					a = sortArray[i].rec_no;
					if( a < attackerCount )		// draw attacker
					{
						if( !attackerFinishedDieFlags[a] )
						{
							attackerUnit[a].draw_abs( attackerArray[a].color_scheme_id,
								attackerArray[a].scrn_x, attackerArray[a].scrn_y,
								clipX1, clipY1, clipX2, clipY2 );
						}
					}
					else		// draw defender
					{
						if( !defenderFinishedDieFlag )
						{
							defenderUnit.draw_abs( defender->color_scheme_id,
								defender->scrn_x, defender->scrn_y,
								clipX1, clipY1, clipX2, clipY2 );
						}
					}
				}

				// ------- blt to front -------------//

				for( a = 0; a < attackerCount; ++a )
					if( lastAttackerX1[a] >= 0 )	// restore front draw in last frame
						vga_util.blt_buf( lastAttackerX1[a], lastAttackerY1[a], lastAttackerX2[a], lastAttackerY2[a], 0 );
				if( lastTargetX1 >= 0 )
					vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
				for( a = 0; a < attackerCount; ++a )
					vga_util.blt_buf( lastAttackerX1[a]=attackerX1[a], lastAttackerY1[a]=attackerY1[a], lastAttackerX2[a]=attackerX2[a], lastAttackerY2[a]=attackerY2[a], 0 );
				vga_util.blt_buf( lastTargetX1=targetX1, lastTargetY1=targetY1, lastTargetX2=targetX2, lastTargetY2=targetY2, 0 );

				// ------ restore back buffer ---------//

				for( a = 0; a < attackerCount; ++a )
					attackerSaveScr[a].rest_scr(&vga_back);
				defenderSaveScr.rest_scr(&vga_back);

				sys.yield();
				sys.blt_virtual_buf();

				DWORD endTime = misc.get_time();
				if( mouse.left_press || endTime - startTime >= 50 )
					Sleep(1);
				else
					Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
			}

			// animate sprite

			for( a = 0; a < attackerCount; ++a )
			{
				if( !attackerFinishedDieFlags[a] )
				{
					switch( attackerUnit[a].cur_action )
					{
					case SPRITE_MOVE:
						err_when( newLord != a );
						attackerArray[a].scrn_x += attackerUnit[a].sprite_speed() * (attackerArray[a].go_scrn_x - attackerArray[a].scrn_x ) / closeDist;
						attackerArray[a].scrn_y += attackerUnit[a].sprite_speed() * (attackerArray[a].go_scrn_y - attackerArray[a].scrn_y ) / closeDist;
						if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_move()->frame_count )
							attackerUnit[a].cur_frame = 1;
						break;
					case SPRITE_ATTACK:
						// finish its attack
						if( attackerUnit[a].remain_attack_delay > 0 )
						{
							if( --attackerUnit[a].remain_attack_delay == 0 )
							{
								attackerUnit[a].cur_action = SPRITE_IDLE;
								attackerUnit[a].cur_frame = 1;
							}
						}
						else if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_attack()->frame_count )
						{
							attackerUnit[a].cur_action = SPRITE_IDLE;
							attackerUnit[a].cur_frame = 1;
						}
						break;
					case SPRITE_IDLE:
						if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_stop()->frame_count )
							attackerUnit[a].cur_frame = 1;
						break;
					default:
						err_here();
					}
				}
			}	// end for
			// defener should be dead and disappeared, no need to take care
		}

		// --------- third step third part ---------//

		// forced to update the background
		mouse.hide();
		vga.use_back();
		disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
		vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();
		vga.use_front();
		mouse.show();

	//	vga.use_back();
	//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	//	vga.flip();
	//	disp_state_map( INTRO_MAP_X1, INTRO_MAP_Y1, INTRO_MAP_WIDTH, INTRO_MAP_HEIGHT, (3<<1) +1 );
	//	vga.use_front();
	//	vga.flip();
		// forced to update the background

		// attackerUnit and go to take the state

		attackerUnit[newLord].cur_action = SPRITE_IDLE;
		attackerUnit[newLord].cur_frame = 1;

		int frameCount = 0;

		while( ++frameCount < 20 )
		{
			// display sprites

			if( (!firstStep || firstStep <= 3 ) && (!lastStep || 3 <= lastStep) )
			{
				DWORD startTime = misc.get_time();

				// -------- save area first --------//

				for( a = 0; a < attackerCount; ++a )
				{
					if( !attackerFinishedDieFlags[a] && 
						attackerUnit[a].get_abs_rect( attackerArray[a].scrn_x, attackerArray[a].scrn_y,
						clipX1, clipY1, clipX2, clipY2, &attackerX1[a], &attackerY1[a], &attackerX2[a], &attackerY2[a]) )
					{
						attackerSaveScr[a].save_scr(attackerX1[a], attackerY1[a], attackerX2[a], attackerY2[a], &vga_back );
					}
					else
					{
						attackerSaveScr[a].clear();
					}
				}	// end for

				if( !defenderFinishedDieFlag &&
					defenderUnit.get_abs_rect(defender->scrn_x, defender->scrn_y,
					clipX1, clipY1, clipX2, clipY2, &targetX1, &targetY1, &targetX2, &targetY2) )
				{
					defenderSaveScr.save_scr(targetX1, targetY1, targetX2, targetY2, &vga_back );
				}
				else
				{
					defenderSaveScr.clear();
				}

				// ---------- sort by attackerY2 or targetY2 -----------//

				SortRec sortArray[MAX_ATTACKER+1];		// include defender
				for( a = 0; a < attackerCount; ++a )
					sortArray[a].init( a, attackerY2[a] );
				sortArray[a].init( a, targetY2 );
				int sortRecCount = a + 1;
				qsort( sortArray, sortRecCount, sizeof(SortRec), sortRecFunc );

				// ---------- draw sprite -----------//

				for( int i = 0; i < sortRecCount; ++i )
				{
					a = sortArray[i].rec_no;
					if( a < attackerCount )		// draw attacker
					{
						if( !attackerFinishedDieFlags[a] )
						{
							attackerUnit[a].draw_abs( attackerArray[a].color_scheme_id,
								attackerArray[a].scrn_x, attackerArray[a].scrn_y,
								clipX1, clipY1, clipX2, clipY2 );
						}
					}
					else		// draw defender
					{
						if( !defenderFinishedDieFlag )
						{
							defenderUnit.draw_abs( defender->color_scheme_id,
								defender->scrn_x, defender->scrn_y,
								clipX1, clipY1, clipX2, clipY2 );
						}
					}
				}

				// ------- blt to front -------------//

				for( a = 0; a < attackerCount; ++a )
					if( lastAttackerX1[a] >= 0 )	// restore front draw in last frame
						vga_util.blt_buf( lastAttackerX1[a], lastAttackerY1[a], lastAttackerX2[a], lastAttackerY2[a], 0 );
				if( lastTargetX1 >= 0 )
					vga_util.blt_buf( lastTargetX1, lastTargetY1, lastTargetX2, lastTargetY2, 0 );
				for( a = 0; a < attackerCount; ++a )
					vga_util.blt_buf( lastAttackerX1[a]=attackerX1[a], lastAttackerY1[a]=attackerY1[a], lastAttackerX2[a]=attackerX2[a], lastAttackerY2[a]=attackerY2[a], 0 );
				vga_util.blt_buf( lastTargetX1=targetX1, lastTargetY1=targetY1, lastTargetX2=targetX2, lastTargetY2=targetY2, 0 );

				// ------ restore back buffer ---------//

				for( a = 0; a < attackerCount; ++a )
					attackerSaveScr[a].rest_scr(&vga_back);
				defenderSaveScr.rest_scr(&vga_back);

				sys.yield();
				sys.blt_virtual_buf();

				DWORD endTime = misc.get_time();
				if( mouse.left_press || endTime - startTime >= 50 )
					Sleep(1);
				else
					Sleep( 50 - (endTime - startTime) );	// at least Sleep(1)
			}

			// animate sprite

			for( a = 0; a < attackerCount; ++a )
			{
				if( !attackerFinishedDieFlags[a] )
				{
					switch( attackerUnit[a].cur_action )
					{
					case SPRITE_ATTACK:
						// finish its attack
						if( attackerUnit[a].remain_attack_delay > 0 )
						{
							if( --attackerUnit[a].remain_attack_delay == 0 )
							{
								attackerUnit[a].cur_action = SPRITE_IDLE;
								attackerUnit[a].cur_frame = 1;
							}
						}
						else if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_attack()->frame_count )
						{
							attackerUnit[a].cur_action = SPRITE_IDLE;
							attackerUnit[a].cur_frame = 1;
						}
						break;
					case SPRITE_IDLE:
						if( ++attackerUnit[a].cur_frame > attackerUnit[a].cur_sprite_stop()->frame_count )
							attackerUnit[a].cur_frame = 1;
						break;
					default:
						err_here();
					}
				}
			}	// end for
			// defener should be dead and disappeared, no need to take care
		}
	}
}
// ------- End of function Campaign::attack_animation --------//


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS //////////////////////////////////////////////////////////

static void build_tables(void)
{
	for (int curr_angle=0; curr_angle < ANGLE_360; curr_angle++)
	{
		double angle_rad = 2*PIE*(double)curr_angle/(double)ANGLE_360;
		cos_look[curr_angle] = (int)(cos(angle_rad) * FIXP_MUL);
		sin_look[curr_angle] = (int)(sin(angle_rad) * FIXP_MUL);
	}
}

static int load_bitmap_file(bitmap_file_ptr bitmap, const char *filename)
{
// this function opens a bitmap file and loads the data into bitmap
// this function will only work with non-compressed 8 bit palettized images
// it uses file handles instead of streams just for a change, no reason

#define BITMAP_ID        0x4D42       // this is the universal id for a bitmap
File     bitmap_file;
int      index;                       // looping index

String	str(DIR_IMAGE);
			str += filename;
//			str += ".COL";

// open the file if it exists
	if (bitmap_file.file_open(str) != 1)
		return 0;
 
// now load the bitmap file header
	bitmap_file.file_read(&bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER));

// test if this is a bitmap file
	if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
   {
		// close the file
	   bitmap_file.file_close();

	   // return error
		return(0);
   } // end if

// we know this is a bitmap, so read in all the sections

// load the bitmap file header
	bitmap_file.file_read(&bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER));

// now the palette
	bitmap_file.file_read(&bitmap->palette, 256*sizeof(PALETTEENTRY));

// now set all the flags in the palette correctly and fix the reverse BGR
	for (index=0; index<256; index++)
    {
		 int temp_color = bitmap->palette[index].peRed;
	    bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
		 bitmap->palette[index].peBlue = temp_color;
		bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
    } // end for index

// finally the image data itself
	bitmap_file.file_seek(-(int)(bitmap->bitmapinfoheader.biSizeImage), SEEK_END);

// allocate the memory for the image
	if (!(bitmap->buffer = new UCHAR [bitmap->bitmapinfoheader.biSizeImage]))
   {
   // close the file
		bitmap_file.file_close();

   // return error
		return(0);

   } // end if

// now read it in
	bitmap_file.file_read(bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage);

// bitmaps are usually upside down, so flip the image
int biWidth  = bitmap->bitmapinfoheader.biWidth,
    biHeight = bitmap->bitmapinfoheader.biHeight;

// allocate the temporary buffer
UCHAR *flip_buffer = new UCHAR[biWidth*biHeight];

// copy image to work area
memcpy(flip_buffer,bitmap->buffer,biWidth*biHeight);

// flip vertically
for (index=0; index<biHeight; index++)
    memcpy(&bitmap->buffer[((biHeight-1) - index)*biWidth],&flip_buffer[index * biWidth], biWidth);

// release the working memory
delete [] flip_buffer;

// close the file
bitmap_file.file_close();

// return success
return(1);

} // end Load_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

static int unload_bitmap_file(bitmap_file_ptr bitmap)
{
// this function releases all memory associated with "bitmap"
if (bitmap->buffer)
   {
   // release memory
   delete [] bitmap->buffer;

   // reset pointer
   bitmap->buffer = NULL;

   } // end if

// return success
return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

int Campaign::smoothing_checking(int xr, int yr, int bitShift)
{
	int yLoc = ((state_array.max_y_loc -1) * yr) >> bitShift;
	int xLoc = ((state_array.max_x_loc -1) * xr) >> bitShift;

	int state[9];
	int xShift[24] = {  0,+1,+1,+1, 0,-1,-1,-1,
								 0,+1,+2,+2,+2,+2,+2,+1, 0,-1,-2,-2,-2,-2,-2,-1 };

	int yShift[24] = { +1,+1, 0,-1,-1,-1, 0,+1,
								+2,+2,+2,+1, 0,-1,-2,-2,-2,-2,-2,-1, 0,+1,+2,+2 };
	int count = 0;

	if (yLoc < state_array.max_y_loc-2 && 
		 xLoc < state_array.max_x_loc-2 &&
		 yLoc > -1 && xLoc > -1)
		state[0] = state_array.get_loc( xLoc, yLoc )->state_recno;//Center
	else
		state[0] = 0;

//	return state[0];

	if(!state[0]
		||	state_array.get_loc( xLoc+1, yLoc   )->state_recno != state[0]
		||	state_array.get_loc( xLoc  , yLoc+1 )->state_recno != state[0]
		||	state_array.get_loc( xLoc+1, yLoc+1 )->state_recno != state[0]
		||	state_array.get_loc( xLoc+2, yLoc   )->state_recno != state[0]
		||	state_array.get_loc( xLoc+2, yLoc+1 )->state_recno != state[0]
		||	state_array.get_loc( xLoc+2, yLoc+2 )->state_recno != state[0]
		||	state_array.get_loc( xLoc+1, yLoc+2 )->state_recno != state[0]
		||	state_array.get_loc( xLoc+1, yLoc+1 )->state_recno != state[0])
	{						
		for (int i = 0; i < 24; i++)
		{
			int x = ((state_array.max_y_loc -1) * (xr + xShift[i])) >> bitShift;
			int y = ((state_array.max_y_loc -1) * (yr + yShift[i])) >> bitShift;

			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			
			if (	state_array.get_loc(x, y)->state_recno == state[0] )
				count ++;
			else
				state[1] = state_array.get_loc(x, y)->state_recno;
		}

		if (count < 13)
			return state[1];			
	}

	return state[0];
}

void Campaign::render_terrain(int par)
{
// this function renders the terrain at the given position and orientation

	int xr,                 // used to compute the point the ray intersects the
		 yr,                 // the height data
	    curr_column,        // current screen column being processed
		 curr_step,          // current step ray is at
	    raycast_ang,        // current angle of ray being cast
		 dx,dy,dz,           // general deltas for ray to move from pt to pt
	 	 curr_voxel_scale,   // current scaling factor to draw each voxel line
		 column_height,      // height of the column intersected and being rendered
		 curr_row,           // number of rows processed in current column
		 x_ray,y_ray,z_ray,  // the position of the tip of the ray
		 map_addr;           // temp var used to hold the addr of data bytes
		
	UCHAR color;            // color of pixel being rendered
	short *dest_column_ptr; // address screen pixel being rendered 
	short is_black = 0;

	int vp_x = VP_X;     
	int vp_y = VP_Y; 
	int vp_z = VP_Z; 
	int vp_ang_x = VP_ANG_X;
	int vp_ang_y = VP_ANG_Y;
	int vp_ang_z = VP_ANG_Z;
		  
	mouse_cursor.set_icon(CURSOR_WAITING);
	mouse_cursor.set_frame(0);
	sys.yield();

	// convert needed vars to fixed point
	vp_x = (vp_x << FIXP_SHIFT);
	vp_y = (vp_y << FIXP_SHIFT);
	vp_z = (vp_z << FIXP_SHIFT);

	// push down destination buffer to bottom of screen
	short *point;
	point = vga_back.buf_ptr(0, SCREEN_HEIGHT-76);
	
	// compute starting angle, at current angle plus half field of view
	raycast_ang = vp_ang_y + ANGLE_HALF_HFOV;

	// cast a ray for each column of the screen
	for (curr_column=0; curr_column < SCREEN_WIDTH; curr_column++)
	{
		// seed starting point for cast
		x_ray = vp_x;
		y_ray = vp_y;
		z_ray = vp_z;

		// compute deltas to project ray at, note the spherical cancelation factor
		dx = COS_LOOK(raycast_ang) <<2;
		dy = SIN_LOOK(raycast_ang) <<1;
	
		// dz is a bit complex, remember dz is the slope of the ray we are casting
		// therefore, we need to take into consideration the down angle, or
		//	x axis angle, the more we are looking down the larger the intial dz
		// must be

		dz = dslope * (vp_ang_x - SCREEN_HEIGHT);
	
		// reset current voxel scale 
		curr_voxel_scale = 0;

		// reset row
		curr_row = 0;

		// get starting address of bottom of current video column 
		dest_column_ptr = point; //dest_buffer;
				
		// enter into casting loop
		int realStep = 0;
		for (curr_step = 0; curr_step < MAX_STEPS; curr_step++)
		{
			// compute pixel in height map to process
			// note that the ray is converted back to an int
			// and it is clipped to to stay positive and in range
			xr = (x_ray  >> FIXP_SHIFT);
			yr = (y_ray  >> FIXP_SHIFT);

			if ((xr > -1) && (xr < (HFIELD_WIDTH-1)))
			{
				xr = (xr & (HFIELD_WIDTH-1));
				yr = (yr & (HFIELD_HEIGHT-1));
				yr = HFIELD_WIDTH - yr -1;

				map_addr = random_terrain_map.get_pix(xr, yr);
				int shadow = shadow_array[xr + (yr<<HFIELD_BIT_SHIFT)];

				// get current height in height map, note the conversion to fixed point
				// and the added multiplication factor used to scale the mountains
				
				column_height = (map_addr << (FIXP_SHIFT+TERRAIN_SCALE_X2));
				
				// test if column height is greater than current voxel height for current step
				// from intial projection point
				if (column_height > z_ray)
				{
					// we know that we have intersected a voxel column, therefore we must
					// render it until we have drawn enough pixels on the display such that
					// thier projection would be correct for the height of this voxel column
					// or until we have reached the top of the screen

					// get the color for the voxel
					color = map_addr;
					
					// draw vertical column voxel
					while(1)
					{
						// draw a pixel
					//	int yLoc = ((state_array.max_y_loc -1) * yr) >> HFIELD_BIT_SHIFT;
					//	int xLoc = ((state_array.max_x_loc -1) * xr) >> HFIELD_BIT_SHIFT;
						
					//	StateLocation *stateLoc = state_array.get_loc(xLoc, yLoc);
					//	int s = stateLoc->state_recno;
						int n;

						if ( yr < (HFIELD_HEIGHT >> 1) )
						{
							shadow = shadow - ((((HFIELD_HEIGHT >> 1) - yr) << 4) >> (HFIELD_BIT_SHIFT-1));
							if (shadow < 0)
								shadow = 0;
						}
									
						if(!map[xr][yr]
						||	map[xr+1][yr] != map[xr][yr]
						||	map[xr][yr+1] != map[xr][yr]
						||	map[xr+1][yr+1] != map[xr][yr]
						||	map[xr+2][yr] != map[xr][yr]
						||	map[xr+2][yr+1] != map[xr][yr]
						||	map[xr+2][yr+2] != map[xr][yr]
						||	map[xr+1][yr+2] != map[xr][yr]
						||	map[xr+1][yr+1] != map[xr][yr])
						{
							if (par != 3)
								*dest_column_ptr = magic.draw((color_bmp_file.palette[color].peRed * shadow) >>5,
									(color_bmp_file.palette[color].peGreen * shadow) >>5,
									(color_bmp_file.palette[color].peBlue * shadow) >>5,
									vga.pixel_format_flag);
							else
								*dest_column_ptr = magic.draw(125, 125, 125,
									vga.pixel_format_flag);
						}
						else
						if(((curr_column & 1) == 0 && (realStep & 1) == 0 && par < 1)
						|| ((curr_column & 1) == 0 && (realStep & 1) == 1 && par < 2)
						|| ((curr_column & 1) == 1 && (realStep & 1) == 0 && par < 3)
						|| ((curr_column & 1) == 1 && (realStep & 1) == 1 && par < 4))
						{
							*dest_column_ptr = magic.draw((color_bmp_file.palette[color].peRed * shadow) >>5,
								(color_bmp_file.palette[color].peGreen * shadow) >>5,
								(color_bmp_file.palette[color].peBlue * shadow) >>5,
								vga.pixel_format_flag);							
						}
						else
						{
							if( (n = state_array[map[xr][yr]]->campaign_nation_recno) )
							{
								short bufPtr = VgaBuf::translate_color(get_nation(n)->nation_color);
								*dest_column_ptr = magic.shading(NULL, &bufPtr, shadow, vga.pixel_format_flag);
							}
							else			
							{
								short bufPtr = VgaBuf::translate_color(game.color_remap_array[0].main_color);
								*dest_column_ptr = magic.shading(NULL, &bufPtr, shadow, vga.pixel_format_flag);
							}
						}

						// now we need to push the ray upward on z axis, so increment the slope
						dz+=dslope;
	
						// now translate the current z position of the ray by the current voxel
						// scale per unit
						z_ray+=curr_voxel_scale;

						// move up one video line
						dest_column_ptr-=vga_back.buf_pitch();
						
						realStep ++;

						// test if we are done with column
						if (++curr_row >= SCREEN_HEIGHT) 
						{
							// force exit of outer steping loop
							// chezzy, but better than GOTO!
							curr_step = MAX_STEPS;
							break;
						} // end if

						// test if we can break out of the loop
						if (z_ray > column_height) 
							break;
					} // end while
				} // end if
			}
		// update the position of the ray
		x_ray+=dx;
		y_ray+=dy;
		z_ray+=dz;

		// update the current voxel scale, remember each step out means the scale increases
		// by the delta scale
		curr_voxel_scale+=dslope;
		} // end for curr_step

	// advance video pointer to bottom of next column
	point ++;
	
	// advance to next angle
	raycast_ang--;	

	} // end for curr_col

	mouse_cursor.set_icon(CURSOR_NORMAL);

} // end Render_Terrain


void Campaign::calculate_terrain_location(int &xLoc, int &yLoc)					
{
	int xr,                 // used to compute the point the ray intersects the
		 yr,                 // the height data
	    curr_column,        // current screen column being processed
		 curr_step,          // current step ray is at
	    raycast_ang,        // current angle of ray being cast
		 dx,dy,dz,           // general deltas for ray to move from pt to pt
	 	 curr_voxel_scale,   // current scaling factor to draw each voxel line
		 column_height,      // height of the column intersected and being rendered
		 curr_row,           // number of rows processed in current column
		 x_ray,y_ray,z_ray,  // the position of the tip of the ray
		 map_addr;           // temp var used to hold the addr of data bytes
		
	int dest_column_ptr; // address screen pixel being rendered 
	short is_black = 0;

	int vp_x = VP_X;     
	int vp_y = VP_Y; 
	int vp_z = VP_Z; 
	int vp_ang_x = VP_ANG_X;
	int vp_ang_y = VP_ANG_Y;
	int vp_ang_z = VP_ANG_Z;

//	mouse_cursor.set_icon(CURSOR_WAITING);
//	mouse_cursor.set_frame(0);
//	sys.yield();

	vp_x = (vp_x << FIXP_SHIFT);
	vp_y = (vp_y << FIXP_SHIFT);
	vp_z = (vp_z << FIXP_SHIFT);

	int point;
	point = (SCREEN_HEIGHT-76) * SCREEN_WIDTH;

	raycast_ang = vp_ang_y + ANGLE_HALF_HFOV;

	for (curr_column=0; curr_column < SCREEN_WIDTH; curr_column++)
	{
		x_ray = vp_x;
		y_ray = vp_y;
		z_ray = vp_z;

		dx = COS_LOOK(raycast_ang) <<2;
		dy = SIN_LOOK(raycast_ang) <<1;
		dz = dslope * (vp_ang_x - SCREEN_HEIGHT);
	
		curr_voxel_scale = 0;

		curr_row = 0;

		dest_column_ptr = point; //dest_buffer;
		
		int realStep = 0;
		for (curr_step = 0; curr_step < MAX_STEPS; curr_step++)
		{
			xr = (x_ray >> FIXP_SHIFT);
			yr = (y_ray >> FIXP_SHIFT);

			if ((xr > -1) && (xr < (HFIELD_WIDTH-1)))
			{
				xr = (xr & (HFIELD_WIDTH-1));
				yr = (yr & (HFIELD_HEIGHT-1));
				yr = HFIELD_WIDTH - yr -1;

				map_addr = random_terrain_map.get_pix(xr, yr);
				column_height = (map_addr << (FIXP_SHIFT+TERRAIN_SCALE_X2));
				
				if (column_height > z_ray)
				{
					while(1)
					{
						if ((yLoc - 10) < ((state_array.max_y_loc -1) * yr) >> HFIELD_BIT_SHIFT &&
							 (yLoc) >= ((state_array.max_y_loc -1) * yr) >> HFIELD_BIT_SHIFT &&
							 (xLoc) <= ((state_array.max_x_loc -1) * xr) >> HFIELD_BIT_SHIFT &&
							 (xLoc + 10) > ((state_array.max_x_loc -1) * xr) >> HFIELD_BIT_SHIFT)

						{
							yLoc = dest_column_ptr / SCREEN_WIDTH;
							xLoc = dest_column_ptr % SCREEN_WIDTH;
							mouse_cursor.set_icon(CURSOR_NORMAL);
							return;
						}

						dz+=dslope;
						z_ray+=curr_voxel_scale;

						dest_column_ptr-=SCREEN_WIDTH;
						realStep ++;

						// test if we are done with column
						if (++curr_row >= SCREEN_HEIGHT) 
						{
							curr_step = MAX_STEPS;
							break;
						} // end if

						if (z_ray > column_height) 
							break;
					} // end while
				} // end if
			}
			// update the position of the ray
			x_ray+=dx;
			y_ray+=dy;
			z_ray+=dz;
			curr_voxel_scale+=dslope;
		} // end for curr_step

	point ++;
	raycast_ang--;	
	} // end for curr_col
	
//	mouse_cursor.set_icon(CURSOR_NORMAL);

} // end Render_Terrain


int Campaign::select_attackable_state(char *selectableStateArray, int selectableStateCount)
{
	int k;
	int loopCount = 0;
	short* originalTerrain =NULL;
	memset(state_recno_of_screen, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
	render_terrain(3);
	vga.use_back();
	put_center_text(VGA_WIDTH>>1, 30, 
		text_campaign.str_select_attack_state(), // "Please select a state to attack",
		1, &font_cmpo);
	originalTerrain = (short *)mem_add( BitmapW::size(VGA_WIDTH, VGA_HEIGHT) );	
	vga_back.read_bitmapW( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, originalTerrain );

	render_attackable_terrain(3, selectableStateArray, selectableStateCount);
	vga.use_back();
	put_center_text(VGA_WIDTH>>1, 30, 
		text_campaign.str_select_attack_state(), // "Please select a state to attack",
		1, &font_cmpo);
	vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();

   //### trevor 25/6 ####//

	if( auto_test_flag )
		return selectableStateArray[misc.random(selectableStateCount)];

	//### trevor 25/6 ####//

	while (1)
	{
		loopCount ++;
		if (loopCount > 9)
			loopCount = 0;


		for (k = 0; k < selectableStateCount; k++)
			if (state_recno_of_screen[mouse.cur_x + mouse.cur_y * SCREEN_WIDTH] == selectableStateArray[k])
			{
				mouse_cursor.set_icon(CURSOR_NORMAL_O);
				break;
			}
		
		if (k == selectableStateCount)
			mouse_cursor.set_icon(CURSOR_NORMAL);

		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		if (state_recno_of_screen[mouse.cur_x + mouse.cur_y * SCREEN_WIDTH] > 0)
		{
			String str;
			int cNationOfState = state_array[state_recno_of_screen[mouse.cur_x + mouse.cur_y * SCREEN_WIDTH]]->campaign_nation_recno;
			if (cNationOfState)
			{			
				CampaignNation* cNation = get_nation( cNationOfState );
				str  = text_campaign.str_state_of_cnation( cNation->nation_name() );
				
				int curStatus = cNation->relation_array[CAMPAIGN_PLAYER_NATION_RECNO-1].status;
		
				if( cNationOfState != CAMPAIGN_PLAYER_NATION_RECNO)
				{
					str += " (";
					str += text_talk.str_relation_status(curStatus);
					str += ") ";
				}
			}
			else
				str = text_campaign.str_independent_state();

			vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
			vga.use_front();

			if (loopCount > 4)
				vga.active_buf->put_bitmapW( 0, 0, originalTerrain );

			font_mid.center_put( 0, 70, VGA_WIDTH, 110, str );

			if (k < selectableStateCount)
				font_mid.center_put( 0, 100, VGA_WIDTH, 140, text_campaign.str_click_to_attack() );
			else
				font_mid.center_put( 0, 100, VGA_WIDTH, 140, text_campaign.str_cannot_attack() );

			sys.blt_virtual_buf();
		}
		else
		{
			vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
			vga.use_front();
				
			if (loopCount > 4)
				vga.active_buf->put_bitmapW( 0, 0, originalTerrain );
						
			sys.blt_virtual_buf();
		}
			
		if(( mouse.left_press || mouse.right_press ) && k < selectableStateCount)
		{
			mem_del( originalTerrain );
			return selectableStateArray[k];
		}
	}

	mem_del( originalTerrain );
	return 0;
}

void Campaign::render_attackable_terrain(int par, char *selectableStateArray, int selectableStateCount)
{
// this function renders the terrain at the given position and orientation

	int xr,                 // used to compute the point the ray intersects the
		 yr,                 // the height data
	    curr_column,        // current screen column being processed
		 curr_step,          // current step ray is at
	    raycast_ang,        // current angle of ray being cast
		 dx,dy,dz,           // general deltas for ray to move from pt to pt
	 	 curr_voxel_scale,   // current scaling factor to draw each voxel line
		 column_height,      // height of the column intersected and being rendered
		 curr_row,           // number of rows processed in current column
		 x_ray,y_ray,z_ray,  // the position of the tip of the ray
		 map_addr;           // temp var used to hold the addr of data bytes
		
	UCHAR color;            // color of pixel being rendered
	short *dest_column_ptr; // address screen pixel being rendered 
	int dest_column_ptr2; // address screen pixel being rendered 
	short is_black = 0;

	int vp_x = VP_X;     
	int vp_y = VP_Y; 
	int vp_z = VP_Z; 
	int vp_ang_x = VP_ANG_X;
	int vp_ang_y = VP_ANG_Y;
	int vp_ang_z = VP_ANG_Z;
		  
	mouse_cursor.set_icon(CURSOR_WAITING);
	mouse_cursor.set_frame(0);
	sys.yield();

	// convert needed vars to fixed point
	vp_x = (vp_x << FIXP_SHIFT);
	vp_y = (vp_y << FIXP_SHIFT);
	vp_z = (vp_z << FIXP_SHIFT);

	// push down destination buffer to bottom of screen
	short *point;
	point = vga_back.buf_ptr(0, SCREEN_HEIGHT-76);

	int point2;
	point2 = (SCREEN_HEIGHT-76) * SCREEN_WIDTH;

	// compute starting angle, at current angle plus half field of view
	raycast_ang = vp_ang_y + ANGLE_HALF_HFOV;

	// cast a ray for each column of the screen
	for (curr_column=0; curr_column < SCREEN_WIDTH; curr_column++)
	{
		// seed starting point for cast
		x_ray = vp_x;
		y_ray = vp_y;
		z_ray = vp_z;

		// compute deltas to project ray at, note the spherical cancelation factor
		dx = COS_LOOK(raycast_ang) <<2;
		dy = SIN_LOOK(raycast_ang) <<1;
	
		// dz is a bit complex, remember dz is the slope of the ray we are casting
		// therefore, we need to take into consideration the down angle, or
		//	x axis angle, the more we are looking down the larger the intial dz
		// must be

		dz = dslope * (vp_ang_x - SCREEN_HEIGHT);
	
		// reset current voxel scale 
		curr_voxel_scale = 0;

		// reset row
		curr_row = 0;

		// get starting address of bottom of current video column 
		dest_column_ptr = point; //dest_buffer;
		dest_column_ptr2 = point2; //dest_buffer;
		
		// enter into casting loop
		int realStep = 0;
		for (curr_step = 0; curr_step < MAX_STEPS; curr_step++)
		{
			// compute pixel in height map to process
			// note that the ray is converted back to an int
			// and it is clipped to to stay positive and in range
			xr = (x_ray  >> FIXP_SHIFT);
			yr = (y_ray  >> FIXP_SHIFT);

			if ((xr > -1) && (xr < (HFIELD_WIDTH-1)))
			{
				xr = (xr & (HFIELD_WIDTH-1));
				yr = (yr & (HFIELD_HEIGHT-1));
				yr = HFIELD_WIDTH - yr -1;

				map_addr = random_terrain_map.get_pix(xr, yr);
				int shadow = shadow_array[xr + (yr<<HFIELD_BIT_SHIFT)];

				// get current height in height map, note the conversion to fixed point
				// and the added multiplication factor used to scale the mountains
				
				column_height = (map_addr << (FIXP_SHIFT+TERRAIN_SCALE_X2));
				
				// test if column height is greater than current voxel height for current step
				// from intial projection point
				if (column_height > z_ray)
				{
					// we know that we have intersected a voxel column, therefore we must
					// render it until we have drawn enough pixels on the display such that
					// thier projection would be correct for the height of this voxel column
					// or until we have reached the top of the screen

					// get the color for the voxel
					color = map_addr;
					
					// draw vertical column voxel
					while(1)
					{
						// draw a pixel
					//	int yLoc = ((state_array.max_y_loc -1) * yr) >> HFIELD_BIT_SHIFT;
					//	int xLoc = ((state_array.max_x_loc -1) * xr) >> HFIELD_BIT_SHIFT;
						
					//	StateLocation *stateLoc = state_array.get_loc(xLoc, yLoc);
					//	int s = stateLoc->state_recno;
						int n;
						int modifiedShadow;

						if ( yr < (HFIELD_HEIGHT >> 1) )
						{
							shadow = shadow - ((((HFIELD_HEIGHT >> 1) - yr) << 4) >> (HFIELD_BIT_SHIFT-1));
							if (shadow < 0)
								shadow = 0;
						}

						modifiedShadow = shadow;
									
						if(!map[xr][yr]
						||	map[xr+1][yr] != map[xr][yr]
						||	map[xr][yr+1] != map[xr][yr]
						||	map[xr+1][yr+1] != map[xr][yr]
						||	map[xr+2][yr] != map[xr][yr]
						||	map[xr+2][yr+1] != map[xr][yr]
						||	map[xr+2][yr+2] != map[xr][yr]
						||	map[xr+1][yr+2] != map[xr][yr]
						||	map[xr+1][yr+1] != map[xr][yr])
						{
							if (par != 3)
								*dest_column_ptr = magic.draw((color_bmp_file.palette[color].peRed * modifiedShadow) >>5,
									(color_bmp_file.palette[color].peGreen * modifiedShadow) >>5,
									(color_bmp_file.palette[color].peBlue * modifiedShadow) >>5,
									vga.pixel_format_flag);
							else
								*dest_column_ptr = magic.draw(125, 125, 125,
									vga.pixel_format_flag);
						}
						else
						if(((curr_column & 1) == 0 && (realStep & 1) == 0 && par < 1)
						|| ((curr_column & 1) == 0 && (realStep & 1) == 1 && par < 2)
						|| ((curr_column & 1) == 1 && (realStep & 1) == 0 && par < 3)
						|| ((curr_column & 1) == 1 && (realStep & 1) == 1 && par < 4))
						{
							*dest_column_ptr = magic.draw((color_bmp_file.palette[color].peRed * modifiedShadow) >>5,
								(color_bmp_file.palette[color].peGreen * modifiedShadow) >>5,
								(color_bmp_file.palette[color].peBlue * modifiedShadow) >>5,
								vga.pixel_format_flag);							
							state_recno_of_screen[dest_column_ptr2] = map[xr][yr];
						}
						else
						{
							int intensity = 0;
							for (int k = 0; k < selectableStateCount; k++)
							{
								if (map[xr][yr] == selectableStateArray[k])
								{
									intensity = 10;
									break;
								}
							}
						
							if( (n = state_array[map[xr][yr]]->campaign_nation_recno) )
							{
								short bufPtr = VgaBuf::translate_color(get_nation(n)->nation_color);
							//	*dest_column_ptr = magic.shading(NULL, &bufPtr, modifiedShadow, vga.pixel_format_flag);
								state_recno_of_screen[dest_column_ptr2] = map[xr][yr];
								short temp = magic.shading(NULL, &bufPtr, modifiedShadow, vga.pixel_format_flag);
								*dest_column_ptr = magic.add_blend(&temp, intensity, intensity, intensity, vga.pixel_format_flag);
							}
							else			
							{
								short bufPtr = VgaBuf::translate_color(game.color_remap_array[0].main_color);
							//	*dest_column_ptr = magic.shading(NULL, &bufPtr, modifiedShadow, vga.pixel_format_flag);
								state_recno_of_screen[dest_column_ptr2] = map[xr][yr];
								short temp = magic.shading(NULL, &bufPtr, modifiedShadow, vga.pixel_format_flag);
								*dest_column_ptr = magic.add_blend(&temp, intensity, intensity, intensity, vga.pixel_format_flag);
							}
						}
									
						// now we need to push the ray upward on z axis, so increment the slope
						dz+=dslope;
	
						// now translate the current z position of the ray by the current voxel
						// scale per unit
						z_ray+=curr_voxel_scale;

						// move up one video line
						dest_column_ptr-=vga_back.buf_pitch();
						dest_column_ptr2-=SCREEN_WIDTH;

						realStep ++;

						// test if we are done with column
						if (++curr_row >= SCREEN_HEIGHT) 
						{
							// force exit of outer steping loop
							// chezzy, but better than GOTO!
							curr_step = MAX_STEPS;
							break;
						} // end if

						// test if we can break out of the loop
						if (z_ray > column_height) 
							break;
					} // end while
				} // end if
			}
		// update the position of the ray
		x_ray+=dx;
		y_ray+=dy;
		z_ray+=dz;

		// update the current voxel scale, remember each step out means the scale increases
		// by the delta scale
		curr_voxel_scale+=dslope;
		} // end for curr_step

	// advance video pointer to bottom of next column
	point ++;
	point2 ++;
	
	// advance to next angle
	raycast_ang--;	

	} // end for curr_col

	mouse_cursor.set_icon(CURSOR_NORMAL);

} // end Render_Terrain

//----------- Begin of function Campaign::play_speech ------------//
void Campaign::play_speech(const char* fileName)
{
	if( curSpeechWavId  )
		stop_speech();

	//---------------------------------//

	String str;
	str  = DIR_CAMPAIGN;
	str += "SOUND"PATH_DELIM;
	strcat(str, fileName);

	if( !misc.is_file_exist(str) )
		return;
	
	curSpeechWavId = audio.play_long_wav(str, DEF_REL_VOLUME);		// cur_speech_wav_id is the WAV id that is needed for stopping playing of the WAV file
}
//----------- End of function Campaign::play_speech ------------//

//----------- Begin of function Campaign::stop_speech ------------//
void Campaign::stop_speech()
{
	audio.stop_long_wav(curSpeechWavId);
	curSpeechWavId = 0;
}
//----------- End of function Campaign::stop_speech ------------//
