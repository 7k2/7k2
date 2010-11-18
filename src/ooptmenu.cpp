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

// Filename    : OOPTMENU.CPP
// Description : in-game option menu (async version)

#include <ooptmenu.h>
#include <ovga.h>
#include <ovgabuf.h>
#include <omodeid.h>
#include <oinfo.h>
#include <obitmap.h>
#include <ocoltbl.h>
#include <oraceres.h>
#include <omouse.h>
#include <omousecr.h>
#include <key.h>
#include <opower.h>
#include <oimgres.h>
#include <ofont.h>
#include <oaudio.h>
#include <omusic.h>
#include <osys.h>
#include <ogammenu.h>
#include <ot_gmenu.h>


enum { BASIC_OPTION_X_SPACE = 78,
		 BASIC_OPTION_HEIGHT = 32 };

enum { COLOR_OPTION_X_SPACE = 35,
		 COLOR_OPTION_HEIGHT = 32 };

enum { SERVICE_OPTION_X_SPACE = 180,
		 SERVICE_OPTION_HEIGHT = 139 };

enum { SLIDE_BUTTON_WIDTH = 16,
		 SLIDE_BUTTON_HEIGHT = 16 };

enum { PAGE_WIDTH = 800, PAGE_HEIGHT = 600 };

//static char race_table[MAX_RACE] =		// race translation table
//{
//	RACE_CHINESE, RACE_GREEK, RACE_JAPANESE, RACE_CARTH,
//	RACE_PERSIAN, RACE_NORMAN, RACE_VIKING
//};

//static char reverse_race_table[MAX_RACE] =		// race translation table
//{
//	5, 3, 1, 6, 4, 0, 2 
//};

//static char *music_group_str[] = { "Off", "On" };
//static char *help_group_str[] = { "None", "Brief", "Detailed" };
//static char *news_group_str[] = { "Major", "All" };
//static char *report_group_str[] = { "Transparent", "Opaque" };
//static char *show_icon_group_str[] = { "Cursor", "Always" };
//static char *show_path_group_str[] = { "No", "Main Map", "World Map", "Both" };
//static char *inside_info_group_str[] = { "None", "Cursor", "Always" };
//static char *scroll_method_group_str[] = { "Comfort", "Speed" };


//static void disp_virtual_button(ButtonCustom *button, int);
static void disp_text_button(ButtonCustom *button, int);
static void disp_slide_bar(SlideBar *slideBar, int);


// return 1 if ok, config is changed
#define IGOPTION_SE_VOL          0x00000001
#define IGOPTION_MUSIC_VOL       0x00000002
#define IGOPTION_RACE            0x00000004
#define IGOPTION_HELP            0x00000008
#define IGOPTION_NEWS            0x00000010
#define IGOPTION_GAME_SPEED      0x00000020
#define IGOPTION_SCROLL_SPEED    0x00000040
#define IGOPTION_REPORT          0x00000080
#define IGOPTION_SHOW_ICON       0x00000100
#define IGOPTION_DRAW_PATH       0x00000200
#define IGOPTION_MAP_ID          0x00000400
#define IGOPTION_INSIDE_INFO     0x00000800
#define IGOPTION_SCROLL_METHOD	0x00001000
#define IGOPTION_PAGE            0x40000000
#define IGOPTION_ALL             0x7FFFFFFF



OptionMenu::OptionMenu() : music_group(2), help_group(3), news_group(2), report_group(2) ,
	show_icon_group(2), show_path_group(4), inside_info_group(3),
	scroll_method_group(2)
{
	active_flag = 0;
	color_remap_table = NULL;
	background_bitmap = NULL;

}


OptionMenu::~OptionMenu()
{
	if( color_remap_table )
	{
		mem_del( color_remap_table );
		color_remap_table = NULL;
	}

	if( background_bitmap )
	{
		mem_del( background_bitmap );
		background_bitmap = NULL;
	}
}


void OptionMenu::set_active()
{
	active_flag = 1;

	// load palette and bitmap so no need to load every time

	if( !color_remap_table )
	{
		color_remap_table = (short *)mem_add( sizeof(*color_remap_table)*0x100 );

		ColorTable colorTable;
		{
			File palFile;
			{	// save stack space for str and palBuf
				String str = DIR_IMAGE;
				str += "CHOOSE2.COL";
				palFile.file_open(str);
			}
			{
				BYTE palBuf[0x100][3];
				palFile.file_seek(8);     				// bypass the header info
				palFile.file_read(palBuf, sizeof(palBuf));
				palFile.file_close();

				// ------- palette description -------------//

				PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

				//-------- create color remap table ---------//

				colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
			}
		}

		memcpy( color_remap_table, colorTable.get_table(0), sizeof(*color_remap_table)*0x100 );
	}

	if( !background_bitmap )
	{
		File imageFile;
		String str = DIR_IMAGE;
		str += "CHOOSE2.ICN";
		imageFile.file_open(str);
		background_bitmap = (char *)mem_add( imageFile.file_size() );
		imageFile.file_read( background_bitmap, imageFile.file_size() );
	}
}


void OptionMenu::clear_active()
{
	if( background_bitmap )
	{
		mem_del( background_bitmap );
		background_bitmap = NULL;
	}

	if( color_remap_table )
	{
		mem_del( color_remap_table );
		color_remap_table = NULL;
	}

	active_flag = 0;
}


void OptionMenu::enter(char untilExitFlag)
{
	if( is_active() )
		return;

	int bx = (VGA_WIDTH - PAGE_WIDTH) / 2;
	int by = (VGA_HEIGHT - PAGE_HEIGHT) / 2;

	int i;
	refresh_flag = IGOPTION_ALL;
	update_flag = 0;
	// active_flag = 1;
	set_active();

	info.save_game_scr();

	Config& tempConfig = config;
	old_config = config;

	// -------- initialize sound effect volume --------//
//	se_vol_slide.init_slide(264, 123, 420, 123+SLIDE_BUTTON_HEIGHT-1, 
//		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	se_vol_slide.init_slide(bx+140, by+118, bx+140+10*SLIDE_BUTTON_WIDTH+9*2, by+118+SLIDE_BUTTON_HEIGHT-1, 
		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	se_vol_slide.set(0, 100, tempConfig.sound_effect_flag ? tempConfig.sound_effect_volume : 0);

	// -------- initialize music volume --------//
//	music_vol_slide.init_slide(566, 123, 722, 123+SLIDE_BUTTON_HEIGHT-1, 
//		SLIDE_BUTTON_WIDTH, disp_slide_bar);
//	music_vol_slide.init_slide(bx+140, by+173, bx+140+10*SLIDE_BUTTON_WIDTH+9*2, by+173+SLIDE_BUTTON_HEIGHT-1, 
//		SLIDE_BUTTON_WIDTH, disp_slide_bar);
//	music_vol_slide.set(0, 100, tempConfig.music_flag ? tempConfig.wav_music_volume : 0);

	int cx = bx + 124;
	int cy = by + 173;
	for( i = 0; i < 2; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_no_yes(i)) + 9;
		music_group[i].create(cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// -------- initialize frame speed volume --------//
//	frame_speed_slide.init_slide(196, 410, 352, 410+SLIDE_BUTTON_HEIGHT-1, 
//		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	frame_speed_slide.init_slide(bx+140, by+338, bx+140+10*SLIDE_BUTTON_WIDTH+9*2, by+338+SLIDE_BUTTON_HEIGHT-1, 
		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	frame_speed_slide.set(0, 31, tempConfig.frame_speed <= 30 ? tempConfig.frame_speed: 31);
	// use frame 31 to represent full speed (i.e. 99)

	// -------- initialize scroll speed volume --------//
//	scroll_speed_slide.init_slide(196, 454, 352, 454+SLIDE_BUTTON_HEIGHT-1, 
//		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	scroll_speed_slide.init_slide(bx+140, by+398, bx+140+10*SLIDE_BUTTON_WIDTH+9*2, by+398+SLIDE_BUTTON_HEIGHT-1, 
		SLIDE_BUTTON_WIDTH, disp_slide_bar);
	scroll_speed_slide.set(0, 10, tempConfig.scroll_speed );

	// --------- initialize race buttons ---------- //

//	for( i = 0; i < MAX_RACE; ++i )
//	{
//		race_button[i].create(181+i*BASIC_OPTION_X_SPACE, 162,
//			181+(i+1)*BASIC_OPTION_X_SPACE-1, 162+BASIC_OPTION_HEIGHT-1,
//			disp_virtual_button, ButtonCustomPara(NULL, race_table[i]));
//	}

	// --------- initialize help button group ---------- //

	cx = bx + 124;
	cy = by + 226;
	for( i = 0; i < 3; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_help_options(i)) + 9;
		help_group[i].create(cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_help_options(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// --------- initialize news button group ---------- //

	cx = bx + 124;
	cy = by + 281;
	for( i = 0; i < 2; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_news_options(i)) + 9;
		news_group[i].create(cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_news_options(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// --------- initialize report button group ---------- //

	cx = bx + 417;
	cy = by + 116;
	for( i = 0; i < 2; ++i )
	{
		int cx2 = cx + font_thin_black.text_width( text_game_menu.str_report_options(i) ) + 9;
		report_group[i].create( cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_report_options(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// --------- initialize show icon button group ---------- //

	cx = bx + 417;
	cy = by + 181;
	for( i = 0; i < 2; ++i )
	{
		int cx2 = cx + font_thin_black.text_width( text_game_menu.str_show_icon_options(i) ) + 9;
		show_icon_group[i].create( cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_show_icon_options(i), i), 0, 0);
		cx = cx2 + 10;
	}

	// --------- initialize show path button group ---------- //

	cx = bx + 417;
	cy = by + 256;
	for( i = 0; i < 4; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_show_path_options(i)) + 9;
		show_path_group[i].create( cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_show_path_options(i), i), 0, 0);
		if( i & 1 )
		{
			cx = bx+417;
			cy += font_thin_black.max_font_height + 3;
		}
		else
		{
			// ####### begin Gilbert 7/6 ########//
			cx = cx2 + 20;
			// ####### end Gilbert 7/6 ########//
		}
	}

	// --------- initialize inside info button group ----------//

	cx = bx + 417;
	cy = by + 331;
	for( i = 0; i < 3; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_inside_info_options(i)) + 9;
		inside_info_group[i].create( cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara( text_game_menu.str_inside_info_options(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// -------- initialize scroll method group -------//

	cx = bx + 417;
	cy = by + 391;
	for( i = 0; i < 2; ++i )
	{
		int cx2 = cx + font_thin_black.text_width(text_game_menu.str_scroll_method_options(i)) + 9;
		scroll_method_group[i].create( cx, cy, cx2, cy + font_thin_black.max_font_height+3,
			disp_text_button, ButtonCustomPara(text_game_menu.str_scroll_method_options(i), i), 0, 0);
		cx = cx2 + 5;
	}

	// --------- other buttons --------//
	start_button.create_text( bx+BUTTON2_X1, by+BUTTON2_Y1, bx+BUTTON2_X2, by+BUTTON2_Y2, text_game_menu.str_return() );
	cancel_button.create_text( bx+BUTTON4_X1, by+BUTTON4_Y1, bx+BUTTON4_X2, by+BUTTON4_Y2, text_game_menu.str_cancel() );

	mouse_cursor.set_icon(CURSOR_NORMAL);
	mouse_cursor.set_frame(0);

   power.win_opened = 1;

	if( untilExitFlag )
	{
		while( is_active() )
		{
			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing() )
					music.play( music.random_bgm_track(music.song_id), sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			disp();
			sys.blt_virtual_buf();
			music.yield();
			detect();
		}
	}
}


void OptionMenu::disp(int needRepaint)
{
	if( !active_flag )
		return;

	err_when( !color_remap_table || !background_bitmap );		// call set_active to load palette and bitmap

	if( needRepaint )
		refresh_flag = IGOPTION_ALL;

	int bx = (VGA_WIDTH - PAGE_WIDTH) / 2;
	int by = (VGA_HEIGHT - PAGE_HEIGHT) / 2;

//	int i;
	Config &tempConfig = config;

	// ------- display --------//
	if(refresh_flag)
	{
		if( refresh_flag & IGOPTION_PAGE )
		{
			vga_back.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );

			// image_interface.put_to_buf( &vga_back, "OPTIONS");
			vga_back.put_bitmap_trans_remap( bx, by, background_bitmap, color_remap_table );

			vga.use_back();

			font_bold_black.put( bx+124, by+98,  text_game_menu.str_se_vol()); // "Sound Effects Volume" );
//			font_bold_black.put( bx+124, by+153, "Music Volume" );
			font_bold_black.put( bx+124, by+153, text_game_menu.str_music()); // "Music" );
			font_bold_black.put( bx+124, by+208, text_game_menu.str_help()); // "Help" );
			font_bold_black.put( bx+124, by+263, text_game_menu.str_display_news()); // "Display News" );
			font_bold_black.put( bx+124, by+318, text_game_menu.str_game_speed()); //"Game Speed" );
			font_bold_black.put( bx+124, by+378, text_game_menu.str_scroll_speed()); //"Scrolling Speed" );

			font_bold_black.put( bx+417, by+98, text_game_menu.str_report_bg()); // "Reports Background" );
			font_bold_black.put( bx+417, by+163, text_game_menu.str_show_unit_id()); // "Show Unit Identities" );
			font_bold_black.put( bx+417, by+238, text_game_menu.str_show_unit_path()); //"Show Unit Paths" );
			font_bold_black.put( bx+417, by+313, text_game_menu.str_inside_info()); //"Inside Information" ); 
			font_bold_black.put( bx+417, by+373, text_game_menu.str_scroll_method()); //"Scrolling Method" ); 

			start_button.paint();
			cancel_button.paint();

			vga.use_front();

			vga.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,0);
		}

		if( refresh_flag & IGOPTION_SE_VOL )
		{
			se_vol_slide.paint(tempConfig.sound_effect_flag ? (tempConfig.sound_effect_volume) : 0);
		}
		if( refresh_flag & IGOPTION_MUSIC_VOL )
		{
			// music_vol_slide.paint(tempConfig.music_flag ? percent_to_slide_volume(tempConfig.wav_music_volume) : 0);
			music_group.paint(tempConfig.music_flag);
		}
//		if( refresh_flag & IGOPTION_RACE )
//		{
//			for( i = 0; i < MAX_RACE; ++i )
//				race_button[i].paint();
//		}
		if( refresh_flag & IGOPTION_HELP )
		{
			help_group.paint(tempConfig.help_mode);
		}
		if( refresh_flag & IGOPTION_NEWS )
		{
			news_group.paint(tempConfig.disp_news_flag);
		}
		if( refresh_flag & IGOPTION_GAME_SPEED )
		{
			frame_speed_slide.paint(tempConfig.frame_speed <= 30 ? tempConfig.frame_speed: 31);
		}
		if( refresh_flag & IGOPTION_SCROLL_SPEED )
		{
			scroll_speed_slide.paint(tempConfig.scroll_speed);
		}
		if( refresh_flag & IGOPTION_REPORT )
		{
			report_group.paint(tempConfig.opaque_report);
		}
		if( refresh_flag & IGOPTION_SHOW_ICON )
		{
			show_icon_group.paint(tempConfig.show_all_unit_icon);
		}
		if( refresh_flag & IGOPTION_DRAW_PATH )
		{
			show_path_group.paint(tempConfig.show_unit_path);
		}
		if( refresh_flag & IGOPTION_INSIDE_INFO )
		{
			inside_info_group.paint(tempConfig.disp_extend_info);
		}
		if( refresh_flag & IGOPTION_SCROLL_METHOD )
		{
			scroll_method_group.paint(tempConfig.scroll_method);
		}
		if( refresh_flag & IGOPTION_MAP_ID )
		{
			//int x2;
			//x2 = font_san.put(521, 64, "Map ID : ", 1);
			//x2 = font_san.put(x2, 64, info.random_seed, 1);
		}

		refresh_flag = 0;
	}
}



int OptionMenu::detect()
{
	if( !active_flag )
		return 0;

	int bx = (VGA_WIDTH - PAGE_WIDTH) / 2;
	int by = (VGA_HEIGHT - PAGE_HEIGHT) / 2;

	Config &tempConfig = config;

//	int i;
	int retFlag1 = 1;

	if( se_vol_slide.detect() == 1)
	{
		tempConfig.sound_effect_flag = se_vol_slide.view_recno > 0;
		if( se_vol_slide.view_recno > 0)
			tempConfig.sound_effect_volume = slide_to_percent_volume(se_vol_slide.view_recno);
		else
			tempConfig.sound_effect_volume = 1;		// never set sound_effect_volume = 0
		audio.set_wav_volume(tempConfig.sound_effect_volume);

		// change music volume, sound effect volume may change music volume
		if( tempConfig.music_flag )
		{
			music.change_volume( tempConfig.wav_music_volume);
		}
	}
//	else if( music_vol_slide.detect() == 1)
//	{
//		tempConfig.music_flag = music_vol_slide.view_recno > 0;
//		tempConfig.wav_music_volume = slide_to_percent_volume(music_vol_slide.view_recno);
//		if( tempConfig.music_flag )
//		{
//			music.change_volume( tempConfig.wav_music_volume );
//		}
//	}
	else if( music_group.detect() >= 0 )
	{
		tempConfig.music_flag = music_group[music_group()].custom_para.value;
	}
	else if( frame_speed_slide.detect() == 1)
	{
		tempConfig.frame_speed = frame_speed_slide.view_recno <= 30 ? frame_speed_slide.view_recno : 99;
	}
	else if( scroll_speed_slide.detect() == 1)
	{
		tempConfig.scroll_speed = scroll_speed_slide.view_recno;
	}
	else
		retFlag1 = 0;

	int retFlag2 = 0;
//	for( i = 0; i < MAX_RACE; ++i )
//	{
//		if( race_button[i].detect() )
//		{
//			if( config.music_flag )
//			{
//				music.play( race_button[i].custom_para.value + 1,
//					sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
//			}
//			else
//			{
//				// stop any music playing
//				music.stop();
//			}
//			retFlag2 = 1;
//		}
//	}

	int retFlag3 = 1;
	if( help_group.detect() >= 0)
	{
		tempConfig.help_mode = help_group[help_group()].custom_para.value;
		//refresh_flag |= IGOPTION_HELP;
		update_flag |= IGOPTION_HELP;
	}
	else if( news_group.detect() >= 0)
	{
		tempConfig.disp_news_flag = news_group[news_group()].custom_para.value;
		//refresh_flag |= IGOPTION_HELP;
		update_flag |= IGOPTION_HELP;
	}
	else if( report_group.detect() >= 0)
	{
		tempConfig.opaque_report = report_group[report_group()].custom_para.value;
		//refresh_flag |= IGOPTION_REPORT;
		update_flag |= IGOPTION_REPORT;
	}
	else if( show_icon_group.detect() >= 0)
	{
		tempConfig.show_all_unit_icon = show_icon_group[show_icon_group()].custom_para.value;
		//refresh_flag |= IGOPTION_SHOW_ICON;
		update_flag |= IGOPTION_SHOW_ICON;
	}
	else if( show_path_group.detect() >= 0)
	{
		tempConfig.show_unit_path = show_path_group[show_path_group()].custom_para.value;
		//refresh_flag |= IGOPTION_DRAW_PATH;
		update_flag |= IGOPTION_DRAW_PATH;
	}
	else if( inside_info_group.detect() >= 0)
	{
		tempConfig.disp_extend_info = inside_info_group[inside_info_group()].custom_para.value;
		update_flag |= IGOPTION_INSIDE_INFO;
	}
	else if( scroll_method_group.detect() >= 0)
	{
		tempConfig.scroll_method = scroll_method_group[scroll_method_group()].custom_para.value;
		update_flag |= IGOPTION_SCROLL_METHOD;
	}
	else if( start_button.detect(KEY_RETURN) )
	{
		// detect return button

		if( &config != &tempConfig)
			config = tempConfig;

		exit(1);

		if( update_flag )
		{
			// save config
			Config fileConfig;
			if( !fileConfig.load("CONFIG.DAT") )
				fileConfig.init();
			fileConfig.change_preference(tempConfig);
			fileConfig.save("CONFIG.DAT");
		}

	}
	else if( cancel_button.detect(KEY_ESC) )
	{
		config = old_config;

		exit(0);
	}
	else
		retFlag3 = 0;

	return retFlag1 || retFlag2 || retFlag3;
}

// 0 = abort
// 1 = accept
void OptionMenu::exit(int action)
{

	// reflect the effect of config.music_flag, config.wav_music_volume
	audio.set_wav_volume(config.sound_effect_volume);
	if( config.music_flag )
	{
		if( music.is_playing() )
		{
			music.change_volume(config.wav_music_volume);
		}
	}
	else
	{
		music.stop();
	}

	// temporary disable active_flag for info.rest_game_scr to call info.disp
	active_flag = 0;
	info.rest_game_scr();
	active_flag = 1;

   power.win_opened = 0;
	// active_flag = 0;
	clear_active();

	return;
}


void OptionMenu::abort()
{
	info.rest_game_scr();
   power.win_opened = 0;
	// active_flag = 0;
	clear_active();
}

// ---------- begin of static function disp_text_button -----//
static void disp_text_button(ButtonCustom *button, int)
{
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
//	fontPtr->space_width -= 8;	
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	fontPtr->put( button->x1, button->y1, (char *)button->custom_para.ptr, 1, button->x2 );
//	fontPtr->space_width += 8;	
}
// ---------- end of static function disp_text_button -----//


// ---------- begin of static function disp_slide_bar  -----//
static void disp_slide_bar(SlideBar *slideBar, int)
{
	char *bitmap0 = image_button.read( "BALSLID0" );
	char *bitmap1 = image_button.read( "BALSLID1" );
	err_when( ((Bitmap *)bitmap0)->get_width() != ((Bitmap *)bitmap1)->get_width() );
	err_when( ((Bitmap *)bitmap0)->get_height() != ((Bitmap *)bitmap1)->get_height() );
	int wBase = ((Bitmap *)bitmap0)->get_width();
	int hBase = ((Bitmap *)bitmap0)->get_height();
	int w = wBase * ( slideBar->view_recno - slideBar->min_recno ) / ( slideBar->max_recno - slideBar->min_recno );
	if( w < 0 )
		w = 0;
	if( w > wBase )
		w = wBase;

	// not using rect_left() and rect_right() because bar_width is not appropriate for this kind of slidebar

	mouse.hide_area( slideBar->scrn_x1, slideBar->scrn_y1, slideBar->scrn_x2, slideBar->scrn_y2 );
	if( w > 0 )
		vga.active_buf->put_bitmap_area_trans_decompress( slideBar->scrn_x1, slideBar->scrn_y1, bitmap0, 0, 0, w-1, hBase-1 );
	if( w < wBase )
		vga.active_buf->put_bitmap_area_trans_decompress( slideBar->scrn_x1, slideBar->scrn_y1, bitmap1, w, 0, wBase-1, hBase-1 );
	mouse.show_area();
}
// ---------- end of static function disp_slide_bar  -----//


int OptionMenu::slide_to_percent_volume(int slideVolume)
{
	switch( slideVolume / 10)
	{
	case 0:
		return slideVolume * 5;
	case 1:
	case 2:
	case 3:
		return slideVolume+40;
		break;

	case 4:
	case 5:
		return slideVolume/2 + 60;
		break;

	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		return slideVolume/4+75;
		break;

	default:
		err_here();
		return slideVolume;
	}
}

// slideVolume  0    10   20   30   40   50   60   70   80   90   100
//              !----!----!----!----!----!----!----!----!----!----!
// percentVoume 0    50             80        90                  100

int OptionMenu::percent_to_slide_volume(int percentVolume)
{
	switch(percentVolume/10)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return percentVolume/5;

	case 5:
	case 6:
	case 7:
		return percentVolume - 40;

	case 8:
		return (percentVolume-60) * 2;

	case 9:
	case 10:
		return (percentVolume-75) * 4;

	default:
		err_here();
		return percentVolume;
	}
}
