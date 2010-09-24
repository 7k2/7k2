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

//Filename    : OF_RESE.CPP
//Description : Firm Research

#include <OF_RESE.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <ODATE.H>
#include <OSTR.H>
#include <ONEWS.H>
#include <OFONT.H>
#include <OCONFIG.H>
#include <OBUTT3D.H>
#include <OMOUSE.H>
#include <OSYS.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <OREMOTE.H>
#include <OSE.H>
#include <OSERES.H>
#include <OBUTTCUS.H>
#include <OIMGRES.H>
#include <OBITMAP.H>
#include <OBITMAPW.H>
#include <OT_FIRM.H>

//------------- Define constant ------------//

// #### begin Gilbert 5/11 #####//
#define MAX_RESEARCH_OPTION		MAX_RACE
#define MAX_RESEARCH_OPTION_PAGE	8
// #### end Gilbert 5/11 #####//
#define RESEARCH_OPTION_HEIGHT 	48

//---------- Define constant ------------//

enum { RESEARCH_MENU_MAIN,
		 RESEARCH_MENU_RESEARCH_CLASS,
		 RESEARCH_MENU_RESEARCH,
	  };

//----------- Define static vars -------------//

static Button3D button_select_research;
static char     research_menu_mode;
static char     disable_refresh=0;
static ButtonCustom	button_research_array[MAX_RESEARCH_OPTION];
static ButtonCustom	button_research_class_array[MAX_TECH_CLASS];
static Button3D	button_cancel;
static int added_count;			// no. of buttons in button_research_array
static char     		last_menu_mode;
// ##### begin Gilbert 7/10 ######//
static short   research_list_start, research_list_end;
// ##### end Gilbert 7/10 ######//
static short edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, edit_progress_enable;

//---------- Declare static functions ---------//

static void i_disp_research_button(ButtonCustom *, int);
static void i_disp_research_class_button(ButtonCustom *, int);

//--------- Begin of function FirmResearch::put_info ---------//
//
void FirmResearch::put_info(int refreshFlag)
{
	if( refreshFlag==INFO_REPAINT )
		last_menu_mode = research_menu_mode = RESEARCH_MENU_MAIN;
	else
	{
		if( last_menu_mode != research_menu_mode )		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = research_menu_mode;
		}
	}

	switch( research_menu_mode )
	{
		case RESEARCH_MENU_MAIN:
			disp_main_menu(refreshFlag);
			break;

		case RESEARCH_MENU_RESEARCH_CLASS:
			disp_research_class_menu(refreshFlag);
			break;

		case RESEARCH_MENU_RESEARCH:
			disp_research_menu(refreshFlag);
			break;
	}
}
//----------- End of function FirmResearch::put_info -----------//


//--------- Begin of function FirmResearch::detect_info ---------//
//
void FirmResearch::detect_info()
{
	switch( research_menu_mode )
	{
		case RESEARCH_MENU_MAIN:
			detect_main_menu();
			break;

		case RESEARCH_MENU_RESEARCH_CLASS:
			detect_research_class_menu();
			break;

		case RESEARCH_MENU_RESEARCH:
			detect_research_menu();
			break;
	}
}
//----------- End of function FirmResearch::detect_info -----------//



//--------- Begin of function FirmResearch::disp_main_menu ---------//
//
void FirmResearch::disp_main_menu(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MIFABASE") );

	// ##### begin Gilbert 5/10 ######//

	FirmWork::put_info(refreshFlag);

/*
	if( !should_show_info() )
		return;
	disp_worker_list(INFO_Y1, refreshFlag);
	disp_firm_info(INFO_Y1, refreshFlag);

	if( selected_worker_id > worker_count )
		selected_worker_id = worker_count;

	if( selected_worker_id )
		disp_worker_info(INFO_Y1, refreshFlag);

	//	int x1 = INFO_X1 +13;
	//	int y1 = INFO_Y1 +235;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;
	if( is_own() )
	{
		button_select_research.create( x2, y2, 'A', "RESEARCH" );
		button_select_research.paint();
	}	
*/
	// ##### end Gilbert 5/10 ######//
}
//----------- End of function FirmResearch::disp_main_menu -----------//


//--------- Begin of function FirmResearch::detect_main_menu ---------//
//
void FirmResearch::detect_main_menu()
{
	// ##### begin Gilbert 5/10 #######//

	FirmWork::detect_info();

/*
	detect_firm_info();
	if( !should_show_info() )
		return;
	//----------- detect worker -----------//
	detect_worker_list();
	detect_worker_info();
	//-------- detect spy button ----------//

	if( !is_own() )
		return;

	//------ detect the select research button -------//

	if( button_select_research.detect() )
		research_menu_mode = RESEARCH_MENU_RESEARCH_CLASS;
*/
	// ##### end Gilbert 5/10 #######//
	
}
//----------- End of function FirmResearch::detect_main_menu -----------//


//--------- Begin of function FirmResearch::disp_research_class_menu ---------//
//
void FirmResearch::disp_research_class_menu(int refreshFlag)
{
	int classTechId;
	int x=INFO_X1+23, y=INFO_Y1+13;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;
	// String str;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	// ##### patch begin Gilbert 28/9 ########//
	//str = "Select a technology class to research";
	// font_snds.center_put( INFO_X1+7, INFO_Y1+13, INFO_X2, INFO_Y1+28, str );
	// font_snds.center_put( INFO_X1+7, INFO_Y1+13, INFO_X2, INFO_Y1+28, 
	//		text_firm.str_select_tech_class() );
	font_snds.put_paragraph( x, y, INFO_X2-6, INFO_Y1+52,
		text_firm.str_select_tech_class() );
	y = font_snds.next_text_y + 2;

	for( classTechId=FIRST_TECH_CLASS_RESEARCH ; classTechId<=LAST_TECH_CLASS_RESEARCH ; classTechId++ )
	{
		int b = classTechId-FIRST_TECH_CLASS_RESEARCH;
		if( tech_res.tech_class(classTechId)->researchable_tech_count(nation_array.player_recno) ==0 )
			continue;

		//-----------------------------------------//

		if( refreshFlag == INFO_REPAINT )
		{
//			button_research_class_array[b].create( x, y+(b+1)*20, x +187, y+(b+1)*20+17,
//				i_disp_research_class_button, ButtonCustomPara( this, classTechId) );
			button_research_class_array[b].create( x, y+b*20, x +187, y+b*20+17,
				i_disp_research_class_button, ButtonCustomPara( this, classTechId) );
		}

		button_research_class_array[classTechId-1].paint();
	}
	// ##### patch end Gilbert 28/9 ########//
	
	x2 +=  3 * BUTTON_DISTANCE;
	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( x2, y2, 'A', "CANCEL" );
	}

	button_cancel.paint();
}
//----------- End of function FirmResearch::disp_research_class_menu -----------//


//--------- Begin of function FirmResearch::detect_research_class_menu ---------//
//
void FirmResearch::detect_research_class_menu()
{
	// ##### begin Gilbert 30/12 #####//
//	char listEndPoint[5] = {1-4, 5-13, 14-16, 17-25, 26-31
//	char listEndPoint[6] = {1, 5, 14, 17, 26, 32};
	for( int i = FIRST_TECH_CLASS_RESEARCH; i <= LAST_TECH_CLASS_RESEARCH; ++i )
	{
		if( tech_res.tech_class(i)->researchable_tech_count(nation_array.player_recno)==0 )
			continue;

		//-----------------------------------------//

		if(button_research_class_array[i-FIRST_TECH_CLASS_RESEARCH].detect() )
		{
//			research_list_start = listEndPoint[i];
//			research_list_end = listEndPoint[i+1];
			research_list_start = tech_res.tech_class(i)->first_tech_id;
			research_list_end = research_list_start + tech_res.tech_class(i)->tech_count;
			research_menu_mode = RESEARCH_MENU_RESEARCH;
			se_ctrl.immediate_sound("TURN_ON");
			break;
		}
	}
	// ##### begin Gilbert 30/12 #####//
	
	// ##### begin Gilbert 8/10 ######//
	if( button_cancel.detect() || mouse.any_click(1) )
	// ##### end Gilbert 8/10 ######//
	{
		se_ctrl.immediate_sound("TURN_OFF");
		research_menu_mode = RESEARCH_MENU_MAIN;
	}

}
//----------- End of function FirmResearch::detect_research_class_menu -----------//


//--------- Begin of function FirmResearch::disp_research_menu ---------//
//
void FirmResearch::disp_research_menu(int refreshFlag)
{
	int techId;
	added_count=0;
//	String str;

	int x=INFO_X1+23, y=INFO_Y1+15;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	// ##### end Gilbert 5/11 ######//
	// if too many buttons, reduce ySpacing

	int ySpacing = 23;
	added_count = 0;
	for( techId = research_list_start ; techId<research_list_end ; techId++ )
	{
		if( tech_res[techId]->can_research(nation_recno) )
		{
			added_count++;
		}
	}
	if( added_count > 9 )
	{
		y -= 4;
		ySpacing = 18;
	}

	// ----- create button ------//

	added_count = 0;
	for( techId = research_list_start ; techId<research_list_end ; techId++ )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			// ##### begin Gilbert 5/11 ######//
			button_research_array[added_count].create( x, y +added_count*ySpacing, x +187, y +added_count*ySpacing +17,
				i_disp_research_button, ButtonCustomPara(this, techId) );
			// ##### end Gilbert 5/11 ######//
		}

		if( (button_research_array[added_count].enable_flag = tech_res[techId]->can_research(nation_recno)) )
		{
			button_research_array[added_count].paint();
			added_count++;
		}
	}
	
	if (added_count == 0)
	{
//		str = "All Technology in this";
//		font_snds.center_put( INFO_X1+7, INFO_Y1+11, INFO_X2, INFO_Y1+28, str );
//		str = "class has been researched !";
//		font_snds.center_put( INFO_X1+7, INFO_Y1+45, INFO_X2, INFO_Y1+62, str );

		font_snds.center_put_paragraph( INFO_X1+17, INFO_Y1+11, INFO_X2-12, INFO_Y1+62,
			text_firm.str_tech_class_finished(), DEFAULT_LINE_SPACE, 0, 0 );
	}

	x2 +=  3 * BUTTON_DISTANCE;
	button_cancel.paint( x2, y2, 'A', "CANCEL" );
}
//--------- End of function FirmResearch::disp_research_menu ---------//

//--------- Begin of function FirmResearch::detect_research_menu ---------//
//
void FirmResearch::detect_research_menu()
{
	for( int i = 0; i < added_count; ++i )
	{
		if(button_research_array[i].detect() )
		{
			// ###### begin Gilbert 5/10 #####//
			se_ctrl.immediate_sound("TURN_ON");
			// ###### end Gilbert 5/10 #####//
			int techId = button_research_array[i].custom_para.value;
			if( tech_res[techId]->can_research(nation_recno) )
			{
				nation_array[nation_recno]->start_research(techId, COMMAND_PLAYER);
				research_menu_mode = RESEARCH_MENU_MAIN;
				// ##### end Gilbert 5/10 ######//
				// info.disp();
				// ##### end Gilbert 5/10 ######//
			}
			break;
		}
	}
	// ##### begin Gilbert 8/10 ######//
	if( button_cancel.detect() || mouse.any_click(1) )
	// ##### end Gilbert 8/10 ######//
	{
		se_ctrl.immediate_sound("TURN_OFF");
		research_menu_mode = RESEARCH_MENU_RESEARCH_CLASS;
	}
}
//--------- Begin of function FirmResearch::detect_research_menu ---------//

//--------- Begin of static function i_disp_research_class_button ---------//

static void i_disp_research_class_button(ButtonCustom *button, int)
{
	int classTechId = button->custom_para.value;

	char* techDes = tech_res.tech_class(classTechId)->desc;

	if ( !button->enable_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, techDes );
		vga.active_buf->bar_alpha(button->x1+1, button->y1+1, button->x2-1, button->y2-1, 1, 0);
	}
	else if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDWN") );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1+1, button->y1-1, button->x2+1, button->y2-1, techDes );
	}
	else if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSEL") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, techDes );
	}
	else
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, techDes );
	}
}
//--------- End of static function i_disp_research_class_button ---------//


//-------- Begin of static function i_disp_research_button --------//
//
void i_disp_research_button(ButtonCustom *button, int repaintBody)
{
	int techId = button->custom_para.value;
	String str;
	TechInfo* techInfo = tech_res[techId];
	str = techInfo->tech_des();
	Firm *firmPtr = (Firm *) button->custom_para.ptr;
	int researchVersion = techInfo->get_nation_tech_level(firmPtr->nation_recno)+1;		// research the next non-researched version

	if( researchVersion > 1 )
	{
		str += " ";
		str += m.roman_number(researchVersion);
	}

	// ####### begin Gilbert 22/1 #######//
	short fontHeight = font_cara.font_height;		// backup font_height
	font_cara.use_max_height();

	if ( !button->enable_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP2") );
		font_cara.center_put(button->x1, button->y1, button->x2, button->y2, str );
		vga.active_buf->bar_alpha(button->x1, button->y1, button->x2, button->y2, 1, 0);
	}
	else
	if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDW2") );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_cara.center_put(button->x1+1, button->y1+1, button->x2+1, button->y2+1, str );
	}
	else
	if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSE2") );
		font_cara.center_put(button->x1, button->y1, button->x2, button->y2, str );
	}
	else
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP2") );
		font_cara.center_put(button->x1, button->y1, button->x2, button->y2, str );
	}
	font_cara.font_height = fontHeight;		// restore font height
	// ####### end Gilbert 22/1 #######//
}
//--------- End of static function i_disp_research_button ---------//


//--------- Begin of function FirmResearch::disp_research_info ---------//
//
void FirmResearch::disp_research_info(int dispY1, int refreshFlag)
{
	static short lastTechId=0;

	int techId = nation_array[nation_recno]->research_tech_id;

	if( refreshFlag==INFO_UPDATE && lastTechId != techId )
	{
		lastTechId = techId;
		info.disp();
	}

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+50 );

	if( !techId )
		return;

	int x=INFO_X1+4, y=dispY1+4;

	//-------- display the icon of the researching item ---------//

	TechInfo* techInfo = tech_res[techId];

	if( refreshFlag == INFO_REPAINT )
	{
		//----------- display text ------------//

		String str;

		str  = techInfo->tech_des();

		int researchVersion = techInfo->get_nation_tech_level(nation_recno)+1;		// research the next non-researched version

		if( researchVersion > 1 )
		{
			str += " ";
			str += m.roman_number(researchVersion);
		}

		font_san.put( x, y+4, str);
	}

	vga_front.indicator( 0, x-2, y+21, nation_array[nation_recno]->research_progress(), (float)100, VGA_GRAY );
}
//----------- End of function FirmResearch::disp_research_info -----------//


// -------- begin of function FirmResearch::disp_firm_info --------//
//
void FirmResearch::disp_firm_info(int dispY1, int refreshFlag)
{
	static short lastTechId=0;
	String str;
	
	int techId = nation_array[nation_recno]->research_tech_id;

	if( refreshFlag==INFO_UPDATE && lastTechId != techId )
	{
		lastTechId = techId;
		info.disp();
	}

	// ###### begin Gilbert 5/10 ######//

	//---------------- paint the panel --------------//
//	techId =1;
	if( !techId )
	{
//		str = "No Technology is";
//		font_snds.center_put( INFO_X1+7, INFO_Y1+31, INFO_X2, INFO_Y1+48, str );
//		str = "currently being";
//		font_snds.center_put( INFO_X1+7, INFO_Y1+60, INFO_X2, INFO_Y1+77, str );
//		str = "researched.";
//		font_snds.center_put( INFO_X1+7, INFO_Y1+89, INFO_X2, INFO_Y1+106, str );

		font_snds.center_put_paragraph( INFO_X1+45, INFO_Y1+31, INFO_X2-40, INFO_Y1+106,
			text_firm.str_no_research(), 16, 0, 0 );
	}
	else
	{
		//-------- display the icon of the researching item ---------//

		int x = INFO_X1;
		int y = INFO_Y1;

		TechInfo* techInfo = tech_res[techId];
		
		//----------- display text ------------//

		// str = "Research Progress";
		font_snds.center_put( INFO_X1+7, INFO_Y1+11, INFO_X2, INFO_Y1+28, 
			text_firm.str_research_progress() );

		// <tech> <version>
		str  = techInfo->tech_des();
		int researchVersion = techInfo->get_nation_tech_level(nation_recno)+1;		// research the next non-researched version
		if( researchVersion > 1 )
		{
			str += " ";
			str += m.roman_number(researchVersion);
		}
		font_snds.center_put( INFO_X1+7, INFO_Y1+28, INFO_X2, INFO_Y1+45, str );

		// str = "Completion Percent";
		font_snds.center_put( INFO_X1+7, INFO_Y1+97, INFO_X2, INFO_Y1+114,
			text_firm.str_completion_percent() );

		// <completion> %
		int researchProgress = (int) nation_array[nation_recno]->research_progress();
		str = (int) researchProgress;
		str += " %";
		font_snds.center_put( INFO_X1+7, INFO_Y1+97+18, INFO_X2, INFO_Y1+114+18, str );

		short*	hitPointBitmap =NULL;

		int ratio = researchProgress * 40 / 100;
		int size  = researchProgress * 76 / 100;

		//106 x 35 --- 15 to 90 ie. 0 to 40
//		hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );
//		if (ratio <11)
//			vga.active_buf->put_bitmap_trans( INFO_X1 +62, INFO_Y1 +59, image_spict.read("MTR_10"));
//		else
//		if (ratio <40)
//			vga.active_buf->put_bitmap_trans( INFO_X1 +62, INFO_Y1 +59, image_spict.read("MTR_39"));
//		else
//			vga.active_buf->put_bitmap_trans( INFO_X1 +62, INFO_Y1 +59, image_spict.read("MTR_40"));
				
//		vga.active_buf->read_bitmapW( INFO_X1 +62, INFO_Y1 +59, INFO_X1 +75 +size, INFO_Y1 +90, hitPointBitmap );
//		vga.active_buf->put_bitmap_trans( INFO_X1 +62, INFO_Y1 +59, image_spict.read("MTR_00"));
//		vga.active_buf->put_bitmapW( INFO_X1 +62, INFO_Y1 +59, hitPointBitmap );
//		mem_del( hitPointBitmap );

		// ----- put empty bar ------//

		vga.active_buf->put_bitmap_trans( INFO_X1 +62, INFO_Y1 +59, image_spict.read("MTR_00"));

		// ----- put bar content -----//

		if( size > 0 )
		{
			char *barBitmap;
			if (ratio <11)
				barBitmap = image_spict.read("MTR_10");
			else if (ratio <40)
				barBitmap = image_spict.read("MTR_39");
			else
				barBitmap = image_spict.read("MTR_40");
			vga.active_buf->put_bitmap_area_trans( INFO_X1+62, INFO_Y1+59, barBitmap,
				0, 0, 13+size, ((Bitmap *)barBitmap)->get_height()-1 );
		}
	}

	//	int x1 = INFO_X1 +13;
	//	int y1 = INFO_Y1 +235;
	// int x2 = INFO_X1 +13;
	// int y2 = INFO_Y1 +281;
	if( is_own() )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			if (!is_monster())
				button_select_research.create( INFO_X1 +13, INFO_Y1 +281, 'A', "RESEARCH" );
			else
				button_select_research.create( INFO_X1 +13, INFO_Y1 +281, 'A', "F_RESE" );
		}
		button_select_research.paint();
	}	
	// ###### end Gilbert 5/10 ######//
}
// -------- end of function FirmResearch::disp_firm_info --------//


// -------- begin of function FirmResearch::detect_firm_info --------//
//
void FirmResearch::detect_firm_info()
{
	// ##### begin Gilbert 5/10 ######//
	if( is_own() )
	{
		//------ detect the select research button -------//

		if( button_select_research.detect() )
		{
			research_menu_mode = RESEARCH_MENU_RESEARCH_CLASS;
			se_ctrl.immediate_sound("TURN_ON");
		}
	}
	// ##### end Gilbert 5/10 ######//
}
// -------- end of function FirmResearch::disp_firm_info --------//


// --------- begin of function FirmRese::disp_edit_mode -------//
//
void FirmResearch::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	FirmWork::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 5;

	// edit production progress

	edit_progress_enable = nation_recno && nation_array[nation_recno]->research_tech_id != 0;
	if( edit_progress_enable )
	{
		TechInfo *techInfo = tech_res[nation_array[nation_recno]->research_tech_id];
		String str;
		str  = techInfo->tech_des();
		int researchVersion = techInfo->get_nation_tech_level(nation_recno)+1;		// research the next non-researched version
		if( researchVersion > 1 )
		{
			str += " ";
			str += m.roman_number(researchVersion);
		}
		x = font_whbl.put( x1, y, str, 0, x2 );

		y += 15;		// str may be long, line feed here
		x = x1;

		x = edit_progress_x2 = font_blue.put( (edit_progress_x1=x), (edit_progress_y1=y), 
			m.format((int)nation_array[nation_recno]->research_progress(), 3), 0, x2 );
		edit_progress_y2 = edit_progress_y1+font_blue.max_font_height;

		y += 18;
	}
	else
	{
		y += 33;
	}
}
// --------- end of function FirmRese::disp_edit_mode -------//


// --------- begin of function FirmRese::detect_edit_mode -------//
//
int FirmResearch::detect_edit_mode()
{
	int rc;
	if( (rc = FirmWork::detect_edit_mode()) )
		return rc;

	// detect production progress

	if( edit_progress_enable && nation_recno )
	{
		float newProgress = nation_array[nation_recno]->research_progress();
		if( mouse.any_click(edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, LEFT_BUTTON) )
		{
			newProgress += 10.0f;	// increase 10%, never touch 100%
			if( newProgress > 99.99f )
				newProgress = 99.99f;
			nation_array[nation_recno]->set_research_progress( newProgress );
			return 1;
		}
		if( mouse.any_click(edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, RIGHT_BUTTON) )
		{
			newProgress -= 10.0f;	// decrease 10%
			if( newProgress < 0.0f )
				newProgress = 0.0f;
			nation_array[nation_recno]->set_research_progress( newProgress );
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmRese::detect_edit_mode -------//
