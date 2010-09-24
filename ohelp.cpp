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

//Filename    : OHELP.CPP
//Description : Object Help

#include <OHELP.H>
#include <ALL.H>
#include <OSYS.H>
#include <OSTR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMUSIC.H>
#include <OFONT.H>
#include <OIMGRES.H>
#include <OINFO.H>
#include <OGAME.H>
#include <OUNIT.H>
#include <OBOX.H>
#include <OMOUSE.H>
#include <OREMOTE.H>
#include <OVBROWSE.H>
#include <OFILETXT.H>
#include <OCONFIG.H>
#include <OBLOB2W.H>
#include <OTUTOR.H>

//---------- Define constant -------------//

//#define HELP_BOX_COLOR      (VGA_GRAY+5)
#define HELP_BOX_COLOR      185

//#define HELP_INACTIVE_TIME  ((float) 0.8)		// when the mouse is inactive for one second, display help

//##### begin trevor 10-12 #####// 
// #define HELP_INACTIVE_TIME  ((float) 1.2)		// when the mouse is inactive for one second, display help
//##### end trevor 10-12 #####//
//##### begin Gilber 30/6 #####// 
#define HELP_INACTIVE_TIME  1500 // when the mouse is inactive for one second, display help
//##### end Gilbert 30/6 #####// 

enum { HELP_SCR_BUF_WIDTH  = 400,      // 400
		 HELP_SCR_BUF_HEIGHT = 200,      // 300
		 HELP_SCR_BUF_SIZE   = HELP_SCR_BUF_WIDTH * HELP_SCR_BUF_HEIGHT };

enum { MSG_LINE_SPACE = 4 };

enum { X_MARGIN = 10,
		 Y_MARGIN = 6  };

enum { MSG_WIN_WIDTH   = 390,
		 MSG_HEAD_HEIGHT = 16   };


//------- Begin of function Help::Help ----------//

Help::Help()
{
	memset( this, 0, sizeof(Help) );

	help_info_array = (HelpInfo*) mem_add( sizeof(HelpInfo) * MAX_HELP_INFO );
}
//------- Begin of function Help::Help ----------//


//------- Begin of function Help::~Help ----------//

Help::~Help()
{
	deinit();
}
//------- Begin of function Help::~Help ----------//


//------- Begin of function Help::init ----------//

void Help::init(char* resName)
{
	String str;

	str  = DIR_RES;
	str += resName;

	load( str );

	long_save_buf.init();
	short_front_buf.init();
	short_back_buf.init();
}
//------- Begin of function Help::init ----------//


//------- Begin of function Help::deinit ----------//

void Help::deinit()
{
	long_save_buf.deinit();
	short_front_buf.deinit();
	short_back_buf.deinit();

   if( help_info_array )
   {
		mem_del( help_info_array );
		help_info_array = NULL;
   }

	if( help_text_buf )
	{
		mem_del( help_text_buf );
		help_text_buf = NULL;
	}
}
//------- Begin of function Help::deinit ----------//


//------- Begin of function Help::load ----------//
//
// <char*> helpFileName = name of the help file.
//
void Help::load(char* helpFileName)
{
	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( helpFileName );

	int dataSize = fileTxt.file_size();

	if( dataSize > help_text_buf_size )
	{
		help_text_buf      = mem_resize( help_text_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		help_text_buf_size = dataSize;
	}

	//-------- read in help info one by one --------//

	HelpInfo* iptr    = help_info_array;
	char*     textPtr = help_text_buf;
	int       readLen, totalReadLen=0;    // total length of text read into the help_text_buf
	int		 loopCount=0;
	char*		 tokenStr;
	
	help_info_count=0;

	first_help_by_help_code = first_help_by_area = -1;
	last_help_by_help_code = last_help_by_area = 0;

	while( !fileTxt.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		// ##### begin Gilbert 9/9 ######//
		iptr->help_code[0] = NULL;		// to identitfy search help text by help_code or coordinate
		// ##### end Gilbert 9/9 ######//

		//--------- if it's a help code ----------//

		if( tokenStr[0] >= 'A' && tokenStr[0] <= 'Z' )
		{
			strncpy( iptr->help_code, tokenStr, iptr->HELP_CODE_LEN );
			iptr->help_code[iptr->HELP_CODE_LEN] = NULL;

			// ###### begin Gilbert 9/9 #######//
			if( first_help_by_help_code == -1)
				first_help_by_help_code = help_info_count;
			last_help_by_help_code = help_info_count+1;		// record one after last
			// ###### end Gilbert 9/9 #######//
		}

		//------ if it's a help area position ------//

		else if( tokenStr[0] >= '0' && tokenStr[0] <= '9' )
		{
			iptr->area800x600_x1 = (short) fileTxt.get_num();
			iptr->area800x600_y1 = (short) fileTxt.get_num();
			iptr->area800x600_x2 = (short) fileTxt.get_num();
			iptr->area800x600_y2 = (short) fileTxt.get_num();
			iptr->area1024x768_x1 = (short) fileTxt.get_num();
			iptr->area1024x768_y1 = (short) fileTxt.get_num();
			iptr->area1024x768_x2 = (short) fileTxt.get_num();
			iptr->area1024x768_y2 = (short) fileTxt.get_num();
			iptr->monster_human_interface = (short) fileTxt.get_num();// 0 when display for both interfaces
																						 // 1 when display only for monster interface
																						 // 2 when display only for human interface
			
			// ###### begin Gilbert 15/9 #######//
			if( first_help_by_area == -1)
				first_help_by_area = help_info_count;
			last_help_by_area = help_info_count+1;			// record one after last
			// ###### end Gilbert 15/9 #######//
		}
		else
			err_here();

		//---------- next line -----------//

		fileTxt.next_line();

		if( fileTxt.is_eof() )
			break;

		//--------------------------------------------//
		// get help title
		//--------------------------------------------//

		fileTxt.read_line( iptr->help_title, iptr->HELP_TITLE_LEN );

		//---------------------------------------------------------//
		// get help description
		//---------------------------------------------------------//

		readLen = fileTxt.read_paragraph(textPtr, help_text_buf_size-totalReadLen);

		iptr->help_text_ptr = textPtr;
		iptr->help_text_len = readLen;

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen>help_text_buf_size );

		//----------- next help block -------------//

		fileTxt.next_line();      // pass the page break line

		help_info_count++;
		iptr++;

		err_when( help_info_count >= MAX_HELP_INFO );
	}

	if( first_help_by_help_code == -1 )
		first_help_by_help_code = 0;
	if( first_help_by_area == -1 )
		first_help_by_area = 0;

	err_when( last_help_by_help_code > help_info_count );
	err_when( last_help_by_area > help_info_count );
}
//--------- End of function Help::load ----------//


//----------- Begin of function Help::disp --------//
//
// Display help message on the given screen location.
//
void Help::disp()
{
	//---- first check if we should disp the help now ------//

	if( !should_disp() )
	{
		help_code[0] = NULL;	// reset it everytime after displaying, if the mouse is still on the button, help_code will be set again.
		custom_help_title[0] = NULL;
		return;
	}

	int i;
	HelpInfo* helpInfo = NULL;

	// ------ button help -------//

	if( help_code[0] )
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_help_code);
		for( ; i<last_help_by_help_code; i++, helpInfo++ )
		{
			if( helpInfo->help_code[0] == help_code[0] &&
				 strcmp( helpInfo->help_code, help_code )==0 )
			{
				break;
			}
		}

		if( i >= last_help_by_help_code )		// not found
			helpInfo = NULL;
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
	}

	//-------- other interface help ---------//

	else
	{
		//--------- locate the help and display it ----------//

		switch( current_display_mode.mode_id )
		{
		case MODE_ID_800x600x16:
			helpInfo = help_info_array + (i = first_help_by_area);
			for( ; i<last_help_by_area ; i++, helpInfo++ )
			{
				if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->area800x600_x1, helpInfo->area800x600_y1,
					helpInfo->area800x600_x2, helpInfo->area800x600_y2) && 
					( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
						(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
						(helpInfo->monster_human_interface == 0) ) )
				{
					break;
				}
			}
			break;

		case MODE_ID_1024x768x16:
			helpInfo = help_info_array + (i = first_help_by_area);
			for( ; i<last_help_by_area ; i++, helpInfo++ )
			{
				if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->area1024x768_x1, helpInfo->area1024x768_y1,
					helpInfo->area1024x768_x2, helpInfo->area1024x768_y2) &&
					( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
						(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
						(helpInfo->monster_human_interface == 0) ) )
				{
					break;
				}
			}
			break;
		default:
			err_here();
		}

		if( i >= last_help_by_area )		// not found
			helpInfo = NULL;
	}

	//-------- button help ---------//

	if( help_code[0] && helpInfo )
	{
		disp_help( help_x, help_y,
			helpInfo->help_title, helpInfo->help_text_ptr );

		help_code[0] = NULL;		// reset it everytime after displaying, if the mouse is still on the button, help_code will be set again.
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
		disp_help(help_x, help_y, custom_help_title, custom_help_detail);
		custom_help_title[0] = NULL;
	}

	//-------- other interface help ---------//

	else if( helpInfo )
	{
		switch( current_display_mode.mode_id )
		{
		case MODE_ID_800x600x16:
			disp_help( (helpInfo->area800x600_x1+helpInfo->area800x600_x2)/2,
						(helpInfo->area800x600_y1+helpInfo->area800x600_y2)/2,
						 helpInfo->help_title, helpInfo->help_text_ptr );
			break;

		case MODE_ID_1024x768x16:
			disp_help( (helpInfo->area1024x768_x1+helpInfo->area1024x768_x2)/2,
						(helpInfo->area1024x768_y1+helpInfo->area1024x768_y2)/2,
						 helpInfo->help_title, helpInfo->help_text_ptr );
			break;

		default:
			err_here();
		}
	}
}
//------------ End of function Help::disp ----------//


//---------- Begin of function Help::disp_help ----------//
//
// <int>   centerX, centerY - the center position of the help area.
// <char*> helpTitle  - title of the help
// [char*] helpDetail - detail of the help.
//
void Help::disp_help(int centerX, int centerY, char* helpTitle, char* helpDetail)
{
	if( config.help_mode == NO_HELP )
		return;

	mouse.hide();

	//------ calculate the position of the help box ------//

	int winWidth, winHeight, dispHelpDetail=0;

	if( helpDetail && helpDetail[0] &&		  // with detailed help
		 config.help_mode == DETAIL_HELP )		  // Detailed Help
	{
		winWidth  = font_san.text_width(helpDetail, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		winHeight = Y_MARGIN*2 + font_san.height() + 11 + font_san.text_height(MSG_LINE_SPACE);    // text_width() must be called before calling text_height()

		int titleWidth = font_san.text_width(helpTitle, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		if( winWidth < titleWidth )
			winWidth = titleWidth;

		dispHelpDetail = 1;
	}
	else		// Help title only
	{
		winWidth  = font_san.text_width(helpTitle, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		winHeight = Y_MARGIN*2 + font_san.height() + 3;
	}

	//--- if the text is bigger than one text box can hold, use a scrollable text box ---//

	int x1, y1, x2, y2;

	if( winWidth * winHeight > HELP_SCR_BUF_SIZE )
	{
		x1 = max( 2, centerX - HELP_SCR_BUF_WIDTH  / 2 );
		y1 = max( 2, centerY - HELP_SCR_BUF_HEIGHT / 2 );

		x2 = x1 + HELP_SCR_BUF_WIDTH - 1;
		y2 = y1 + HELP_SCR_BUF_HEIGHT - 1;
	}
	else
	{
		x1 = max( 2, centerX - winWidth  / 2 );
		y1 = max( 2, centerY - winHeight / 2 );

		x2 = x1 + winWidth - 1;
		y2 = y1 + winHeight - 1;
	}

	if( x2 >= VGA_WIDTH )
	{
		x2 = VGA_WIDTH-10;
		x1 = x2-winWidth+1;
	}

	if( y2 >= VGA_HEIGHT )
	{
		y2 = VGA_HEIGHT-3;
		y1 = y2-winHeight+1;
	}

	// ######## begin Gilbert 21/12 #######//

	// -------- hide short help --------//

	hide_short_help(&vga_front);

	// ######## end Gilbert 21/12 #######//

	//------------- save the area --------------//

	long_save_buf.save_scr( x1, y1, x2, y2 ); 	// save the screen to the private buffer in Help

	//------- Draw box (and arrow if specified object) ------//

//	vga_front.bar( x1, y1, x2, y2, V_WHITE );

//	vga_front.bar( x1, y1, x2, y1+1, HELP_BOX_COLOR );        // Top border
//	vga_front.bar( x1, y2-1, x2, y2, HELP_BOX_COLOR );        // Bottom border
//	vga_front.bar( x1, y1, x1+1, y2, HELP_BOX_COLOR );        // Left border
//	vga_front.bar( x2-1, y1, x2, y2, HELP_BOX_COLOR );        // Right border

	//vga_front.rect( x1+2, y1+2, x2, y2, 3, V_BLACK );
	vga_front.bar( x1, y1, x2-3, y2-3, VgaBuf::color_up );
	vga_front.rect( x1, y1, x2-3, y2-3, 3, HELP_BOX_COLOR );
	vga_front.bar_alpha( x2-2, y1+2, x2, y2-3, 1, V_BLACK );
	vga_front.bar_alpha( x1+2, y2-2, x2, y2, 1, V_BLACK );

	//--------- disp help detail -----------//

	font_san.put( x1+X_MARGIN, y1+Y_MARGIN, helpTitle );

	if( dispHelpDetail )
	{
		int y = y1 + Y_MARGIN + font_san.height() + 4;

		// vga_front.bar( x1, y, x2, y+1, HELP_BOX_COLOR );  // line between description and help text
		vga_front.bar( x1, y, x2-3, y+1, 185 );  // line between description and help text

		font_san.put_paragraph( x1+X_MARGIN, y+4, x2-X_MARGIN, y2-Y_MARGIN, helpDetail, MSG_LINE_SPACE );
	}

//	if( sys.debug_session )
		sys.blt_virtual_buf();

	//--- in a single player game, pause the game when a help message is disp_helplayed ---//

	// ##### begin Gilbert 30/6 #########//
//	while( should_disp() )
//	{
//		sys.yield();
//		music.yield();
//		mouse.get_event();
//	}
	while(1)
	{
		sys.yield();
		music.yield();
		if( !should_disp() )		// so any mouse click will not be lost
			break;
		mouse.get_event();
	}
	// ##### end Gilbert 30/6 #########//

	long_save_buf.rest_scr();

	// ###### begin Gilbert 21/12 #######//
	// ---------- show short help -------//

	disp_short_help(&vga_front);
	// ##### end Gilbert 21/12 #######//

	mouse.show();

	sys.blt_virtual_buf();
}
//--------- End of function Help::disp_help ----------//


//--------- Begin of function Help::should_disp --------//
//
int Help::should_disp()
{
	if( config.help_mode < DETAIL_HELP || remote.is_enable())
		return 0;

	if( VBrowse::press_record )
		return 0;

	// ####### begin Gilbert 13/7 #########//
	if( last_mouse_x==mouse.cur_x && last_mouse_y==mouse.cur_y &&
		 !mouse.left_press && !mouse.right_press ) // && !mouse.any_click(2) )
	{
		if( m.get_time() - mouse_still_time >= HELP_INACTIVE_TIME )
	// ####### end Gilbert 13/7 #########//
		{
			return 1;
		}
	}
	else
	{
		last_mouse_x = mouse.cur_x;
		last_mouse_y = mouse.cur_y;
		mouse_still_time = m.get_time();
	}

	return 0;
}
//---------- End of function Help::should_disp ---------//


//--------- Begin of function Help::set_help --------//
//
void Help::set_help(int x1, int y1, int x2, int y2, char* helpCode)
{
	err_when( strlen(helpCode) > 8 );

	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	strcpy( help_code, helpCode );

	help_x = (x1+x2)/2;
	help_y = (y1+y2)/2;
	help_x1 = x1;
	help_y1 = y1;
	help_x2 = x2;
	help_y2 = y2;
}
//---------- End of function Help::set_help ---------//


//--------- Begin of function Help::set_unit_help --------//
//
void Help::set_unit_help(int unitId, int rankId, int x1, int y1, int x2, int y2)
{
	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	//-------- compose the help string --------//

	static String str;

#if(defined(SPANISH) || defined(FRENCH))
	str = "";
	if( rankId==RANK_KING )
		str = translate.process("King ");
	else if( rankId==RANK_GENERAL )
		str = translate.process("General ");
	str += unit_res[unitId]->name;
#else
	str = unit_res[unitId]->name;

	if( rankId==RANK_KING )
	{
		str += " ";
		str += translate.process( "King" );
	}
	else if( rankId==RANK_GENERAL )
	{
		str += " ";
		str += translate.process( "General" );
	}
#endif

	set_custom_help( x1, y1, x2, y2, str );
}
//---------- End of function Help::set_unit_help ---------//


//--------- Begin of function Help::set_custom_help --------//
//
// <int>   x1, y1, x2, y2 - the coordination of the help
// <char*> helpTitle  - the title of the help
// [char*] helpDetail - the detailed text of the help
//
void Help::set_custom_help(int x1, int y1, int x2, int y2, char* helpTitle, char* helpDetail)
{
	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	// ##### begin Gilbert 10/12 ######//
	// clear help code
	help_code[0] = '\0';
	// ##### end Gilbert 10/12 ######//
	help_x = (x1+x2)/2;
	help_y = (y1+y2)/2;
	help_x1 = x1;
	help_y1 = y1;
	help_x2 = x2;
	help_y2 = y2;

	strncpy( custom_help_title, helpTitle, CUSTOM_HELP_TITLE_LEN );
	custom_help_title[CUSTOM_HELP_TITLE_LEN] = NULL;

	if( helpDetail )
	{
		strncpy( custom_help_detail, helpDetail, CUSTOM_HELP_DETAIL_LEN );
		custom_help_detail[CUSTOM_HELP_DETAIL_LEN] = NULL;
	}
	else
	{
		custom_help_detail[0] = NULL;
	}
}
//---------- End of function Help::set_custom_help ---------//


//--------- Begin of function Help::disp_short_help --------//
//
void Help::disp_short_help(VgaBuf *vgaBuf)
{
	// ###### begin Gilbert 9/9 #######//

	// ----- display help title in short bar under info -------//

	int i;
	HelpInfo* helpInfo = NULL;

	// ------ button help -------//

	if( help_code[0] )
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_help_code);
		for( ; i<last_help_by_help_code; i++, helpInfo++ )
		{
			if( helpInfo->help_code[0] == help_code[0] &&
				 strcmp( helpInfo->help_code, help_code )==0 )
			{
				break;
			}
		}

		if( i >= last_help_by_help_code )		// not found
			helpInfo = NULL;
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
	}

	//-------- other interface help ---------//

	else
	{
		//--------- locate the help and display it ----------//

		switch( current_display_mode.mode_id )
		{
		case MODE_ID_800x600x16:
			helpInfo = help_info_array + (i = first_help_by_area);
			for( ; i<last_help_by_area ; i++, helpInfo++ )
			{
				if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->area800x600_x1, helpInfo->area800x600_y1,
					helpInfo->area800x600_x2, helpInfo->area800x600_y2)  &&
					!(game.game_mode == GAME_TUTORIAL && tutor.should_not_display_report_button_flag) &&		 
					( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
						(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
						(helpInfo->monster_human_interface == 0) ) )
				{
					help_x1 = helpInfo->area800x600_x1;
					help_y1 = helpInfo->area800x600_y1;
					help_x2 = helpInfo->area800x600_x2;
					help_y2 = helpInfo->area800x600_y2;
					break;
				}
			}
			break;

		case MODE_ID_1024x768x16:
			helpInfo = help_info_array + (i = first_help_by_area);
			for( ; i<last_help_by_area ; i++, helpInfo++ )
			{
				if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->area1024x768_x1, helpInfo->area1024x768_y1,
					helpInfo->area1024x768_x2, helpInfo->area1024x768_y2) &&
					!(game.game_mode == GAME_TUTORIAL && tutor.should_not_display_report_button_flag) &&
					( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
						(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
						(helpInfo->monster_human_interface == 0) ) )
				{
					help_x1 = helpInfo->area1024x768_x1;
					help_y1 = helpInfo->area1024x768_y1;
					help_x2 = helpInfo->area1024x768_x2;
					help_y2 = helpInfo->area1024x768_y2;
					break;
				}
			}
			break;
		default:
			err_here();
		}

		if( i >= last_help_by_area )		// not found
			helpInfo = NULL;
	}

	// ------ backup active_buf, use_back_buf ------//

	char *titleStr = NULL;

	if( helpInfo )
	{
		titleStr = helpInfo->help_title;
	}
	else if( custom_help_title[0] )
	{
		titleStr = custom_help_title;
	}

	if( titleStr )
	{
		short x1, y1, x2, y2;		// top left corner of the area
      //#### begin trevor 10-12 #####//
		int textWidth = font_san.text_width(titleStr) + X_MARGIN*2;
		int textHeight = font_san.text_height() + Y_MARGIN*2;
		//#### begin trevor 10-12 #####//
		x2 = help_x1 - 1;
		y2 = help_y1 - 1;
		x1 = x2 - textWidth + 1;
		y1 = y2 - textHeight + 1;
		if( y1 < 0 )
		{
			// try top border
			y1 = 0;
			y2 = y1 + textHeight - 1;
		}
		if( x1 < 0 )
		{
			// try right side
			x1 = help_x2 + 1;
			x2 = x1 + textWidth - 1;
		}

		// save buffer

		(vgaBuf->is_front ? short_front_buf : short_back_buf).save_scr( x1, y1, x2, y2, vgaBuf );

		if( config.help_mode >= BRIEF_HELP )
		{
			// for safety, still capture the screen even though short help is not displayed
			// may cause problem when changing help_mode

			// put text

			// vgaBuf->bar(x1, y1, x2, y2, V_WHITE);
			// vgaBuf->rect(x1, y1, x2, y2, 1, V_BLACK);
			vgaBuf->bar( x1, y1, x2-3, y2-3, VgaBuf::color_up );
			vgaBuf->rect( x1, y1, x2-3, y2-3, 3, HELP_BOX_COLOR );
			vgaBuf->bar_alpha( x2-2, y1+2, x2, y2-3, 1, V_BLACK );
			vgaBuf->bar_alpha( x1+2, y2-2, x2, y2, 1, V_BLACK );

			char useBackFlag = vga.use_back_buf;	//save use_back_buf
			VgaBuf *activeBuf = vga.active_buf;

			vga.use_back_buf = !vgaBuf->is_front;
			vga.active_buf = vgaBuf;

			font_san.center_put( x1, y1, x2-3, y2-3, titleStr );

			vga.use_back_buf = useBackFlag;	// restore use_back_buf
			vga.active_buf = activeBuf;
		}
	}
	// ###### end Gilbert 9/9 #######//
}
//--------- End of function Help::disp_short_help --------//


//--------- Begin of function Help::flip --------//
//
void Help::flip()
{
#if(defined(USE_FLIP))

	// ------ called when flip page ------//

	// exchange front and back save buf

	short_front_buf.swap(short_back_buf);

#endif

}
//--------- End of function Help::flip --------//


//--------- Begin of function Help::hide_short_help --------//
//
void Help::hide_short_help(VgaBuf *vgaBuf)
{
	(vgaBuf->is_front ? short_front_buf : short_back_buf).rest_scr(vgaBuf);
}
//--------- End of function Help::hide_short_help --------//


//--------- Begin of function Help::hide_area --------//
//
void Help::hide_area(int x1, int y1, int x2, int y2)
{
	// updating front buffer, hide the area first

	if( !short_front_buf.is_clear() )
	{
		short_front_buf.hide_area(x1, y1, x2, y2);
	}
}
//--------- End of function Help::hide_area --------//


//--------- Begin of function Help::show_area --------//
//
void Help::show_area()
{
	// updating front buffer, hide the area first

	if( !short_front_buf.is_clear() )
	{
		short_front_buf.show_area();
	}
}
//--------- End of function Help::show_area --------//


//--------- Begin of function HelpSaveScreen::HelpSaveScreen --------//
//
HelpSaveScreen::HelpSaveScreen()
{
	save_scr_buf = NULL;
	clear();
}
//--------- End of function HelpSaveScreen::HelpSaveScreen --------//


//--------- Begin of function HelpSaveScreen::~HelpSaveScreen --------//
//
HelpSaveScreen::~HelpSaveScreen()
{
	deinit();
}
//--------- End of function HelpSaveScreen::~HelpSaveScreen --------//


//--------- Begin of function HelpSaveScreen::init --------//
//
void HelpSaveScreen::init()
{
	deinit();

	hiding_flag = 0;
	hide_area_count = 0;
	save_scr_buf = new Blob2DW;
	clear();
}
//--------- End of function HelpSaveScreen::init --------//


//--------- Begin of function HelpSaveScreen::deinit --------//
//
void HelpSaveScreen::deinit()
{
	clear();
	if( save_scr_buf )
	{
		delete save_scr_buf;
		save_scr_buf = NULL;
	}
}
//--------- End of function HelpSaveScreen::deinit --------//


//--------- Begin of function HelpSaveScreen::swap --------//
//
void HelpSaveScreen::swap( HelpSaveScreen &s)
{
	// memory swap

	char t[sizeof(HelpSaveScreen)];
	memcpy( t, this, sizeof(HelpSaveScreen) );
	memcpy( this, &s, sizeof(HelpSaveScreen) );
	memcpy( &s, t, sizeof(HelpSaveScreen) );
}
//--------- End of function HelpSaveScreen::swap --------//


//--------- Begin of function HelpSaveScreen::save_scr --------//
//
void HelpSaveScreen::save_scr(int x1, int y1, int x2, int y2, VgaBuf* vgaBuf )
{
	if( !is_clear() )		// there is already a screen saved
		return;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	long saveSize = (long)(x2-x1+1) * (y2-y1+1);

	err_when( saveSize > HELP_SCR_BUF_SIZE );

	if( !vgaBuf )
		vgaBuf = &vga_front;

	save_scr_x1 = x1;
	save_scr_y1 = y1;
	save_scr_x2 = x2;
	save_scr_y2 = y2;

	if( vgaBuf->is_front )
		mouse.hide_area( x1, y1, x2, y2 );

	save_scr_buf->clear();
	save_scr_buf->resize( x1, y1, x2-x1+1, y2-y1+1 );
	vgaBuf->read_bitmapW( x1, y1, x2, y2, save_scr_buf->bitmap_ptr() );

	if( vgaBuf->is_front )
		mouse.show_area();

	unclear();
}
//--------- End of function HelpSaveScreen::save_scr --------//


//--------- Begin of function HelpSaveScreen::rest_scr --------//
//
void HelpSaveScreen::rest_scr(VgaBuf* vgaBuf, int keepUnclear )
{
	if( is_clear() )        // already restored, or not saved yet
		return;

	err_when( save_scr_x1>save_scr_x2 || save_scr_y1>save_scr_y2 ||
				 save_scr_x1<0 || save_scr_y1<0 || save_scr_x2>=VGA_WIDTH || save_scr_y2>=VGA_HEIGHT );

	if( !vgaBuf )
		vgaBuf = &vga_front;

//	mouse.hide_area( save_scr_x1, save_scr_y1, save_scr_x2, save_scr_y2 );

	vgaBuf->put_bitmapW( save_scr_x1, save_scr_y1, save_scr_buf->bitmap_ptr() );

//	mouse.show_area();

	if( !keepUnclear )
		clear();       // state that it has been restored.
}
//--------- End of function HelpSaveScreen::rest_scr --------//


//--------- Begin of function HelpSaveScreen::hide_area --------//
//
void HelpSaveScreen::hide_area(int x1, int y1, int x2, int y2 )
{
	if( is_clear() )		// there is already a screen saved
		return;

	++hide_area_count;

	if( !hiding_flag && m.is_touch( x1, y1, x2, y2,
		save_scr_x1, save_scr_y1, save_scr_x2, save_scr_y2 ) )
	{
		// don't handle mouse cursor
		vga.active_buf->put_bitmapW_fast( save_scr_x1, save_scr_y1, save_scr_buf->bitmap_ptr() );

		hiding_flag = hide_area_count;
	}
}
//--------- End of function HelpSaveScreen::hide_area --------//


//--------- Begin of function HelpSaveScreen::show_area --------//
//
void HelpSaveScreen::show_area()
{
	if( is_clear() )        // already restored, or not saved yet
		return;

	if( hiding_flag && hiding_flag == hide_area_count )
	{
		vga_front.read_bitmapW( save_scr_x1, save_scr_y1, save_scr_x2, save_scr_y2,
			save_scr_buf->bitmap_ptr() );
		hiding_flag = 0;
	}
	--hide_area_count;
	err_when( hide_area_count < 0 );
}
//--------- End of function HelpSaveScreen::show_area --------//


//--------- Begin of function HelpSaveScreen::clear --------//
//
void HelpSaveScreen::clear()
{
	clear_flag = 1;
}
//--------- End of function HelpSaveScreen::clear --------//


//--------- Begin of function HelpSaveScreen::unclear --------//
//
void HelpSaveScreen::unclear()
{
	clear_flag = 0;
}
//--------- End of function HelpSaveScreen::unclear --------//


//--------- Begin of function HelpSaveScreen::is_clear --------//
//
int HelpSaveScreen::is_clear()
{
	return clear_flag;
}
//--------- End of function HelpSaveScreen::is_clear --------//
