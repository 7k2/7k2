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

// Filename    : OF_LAIRI.CPP
// Description : Monster Lair Interface

#include <of_lair.h>
#include <ovga.h>
#include <omodeid.h>
#include <omonsres.h>
#include <onation.h>
#include <omouse.h>
#include <obitmap.h>
#include <ofont.h>
#include <oimgres.h>
#include <ounitres.h>
#include <otechres.h>
#include <oinfo.h>
#include <obutton.h>
#include <obutt3d.h>
#include <obuttcus.h>
#include <ose.h>
#include <oseres.h>
#include <ot_firm.h>
#include <ot_basic.h>

// -------- define constant --------//

#define MAX_LAIR_RESEARCH_OPTION 5

// ------- declare static variable --------//

static char	last_menu_mode;
static Button3D button_train, button_select_research, button_grant;

static int added_count;			// no. of buttons in button_research_array
static ButtonCustom	button_research_array[MAX_LAIR_RESEARCH_OPTION];
static Button3D	button_cancel;
static Button		button_yes, button_no;

// -------- declare static function ---------//

static void i_disp_research_button(ButtonCustom *, int);


//--------- Begin of function FirmLair::put_info ---------//
//
void FirmLair::put_info(int refreshFlag)
{
	FirmCamp::put_info(refreshFlag);

	// change refreshFlag to INFO_REPAINT if menu mode changed

	if( refreshFlag == INFO_REPAINT || last_menu_mode != firm_menu_mode )
	{
		refreshFlag = INFO_REPAINT;
		last_menu_mode = firm_menu_mode;
	}

	// handle other menu mode

	switch( firm_menu_mode )
	{
	case FIRM_LAIR_MENU_RESEARCH:
		disp_research_menu(refreshFlag);
		break;

	case FIRM_LAIR_MENU_GRANT:
		disp_grant_menu(refreshFlag);
		break;
	}
}
//--------- End of function FirmLair::put_info ---------//


//--------- Begin of function FirmLair::detect_info ---------//
//
void FirmLair::detect_info()
{
	FirmCamp::detect_info();

	switch( firm_menu_mode )
	{
	case FIRM_LAIR_MENU_RESEARCH:
		detect_research_menu();
		break;

	case FIRM_LAIR_MENU_GRANT:
		detect_grant_menu();
		break;
	}
}
//--------- End of function FirmLair::detect_info ---------//


//--------- Begin of function FirmLair::disp_camp_info ---------//
//
void FirmLair::disp_camp_info(int dispY1, int refreshFlag)
{
	FirmCamp::disp_camp_info(dispY1, refreshFlag);

	//------ display train button -------//

	if( refreshFlag == INFO_REPAINT )
	{
		// ######## begin Gilbert 31/12 ######//
//		button_train.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+281, 'A', "TRAIN" );
//		button_select_research.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "RESEARCH" );

		button_train.create( INFO_X1+13, INFO_Y1+281, 'A', "F_TRAIN" );
		button_select_research.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "F_RESE" );
		button_grant.create( INFO_X1+13, INFO_Y1+235, 'A', "F_GRANT" );
		// ######## end Gilbert 31/12 ######//
	}

	//-------------------------------------//

	if( nation_recno )
	{
		if( is_own() )
		{
			button_train.enable_flag = can_recruit();
			button_train.paint();

			button_select_research.enable_flag = tech_res.researchable_tech_count(nation_recno, race_id, 0) > 0;		// 0-don't check live points
			button_select_research.paint();
		}
	}
	else
	{
		// ###### begin Gilbert 30/12 ######//
		if( (button_grant.visible_flag = (char)nation_array.player_recno) )
		{
			button_grant.enable_flag = can_grant_to_non_own_lair(nation_array.player_recno, 0); 		// 0-don't check if the player has enough cash or not
		}
		// ###### end Gilbert 30/12 ######//
		button_grant.paint();
	}
}
//----------- End of function FirmLair::disp_camp_info -----------//


//--------- Begin of function FirmLair::detect_camp_info ---------//
//
void FirmLair::detect_camp_info()
{
	FirmCamp::detect_camp_info();

	if( nation_recno )
	{
		if( is_own() )
		{
			int rc=button_train.detect(0, 0, 1);	// 1-detect right button

			if( rc==1 )                            // left button
			{	
				recruit_soldier( COMMAND_PLAYER );
			//	se_ctrl.immediate_sound("TURN_ON");
				se_ctrl.immediate_sound("ARM-ALRT");
			}

			// ###### begin Gilbert 26/1 #######//
			else if( rc==2 )
			{
				if( soldier_count > 0 )
				{
					cancel_train_soldier( soldier_count, COMMAND_PLAYER );	// cancel the last soldier
					se_ctrl.immediate_sound("TURN_OFF");
				}
			}
			// ###### end Gilbert 26/1 #######//

			if( button_select_research.detect() )
			{
				firm_menu_mode = FIRM_LAIR_MENU_RESEARCH;		// change to research menu
				se_ctrl.immediate_sound("BUY-SCIE");
				return;
			}
		}
	}
	else
	{
		if( button_grant.detect() )
		{
			firm_menu_mode = FIRM_LAIR_MENU_GRANT;
			se_ctrl.immediate_sound("TURN_ON");
			return;
		}
	}
}
//----------- End of function FirmLair::detect_camp_info -----------//


//--------- Begin of function FirmLair::disp_research_menu ---------//
//
void FirmLair::disp_research_menu(int refreshFlag)
{
	int x=INFO_X1+23, y=INFO_Y1+15;
	int ySpacing = 40;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	// ---------- create research button -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		added_count = 0;

		for( int techId = 1; techId <= tech_res.tech_count && added_count < MAX_LAIR_RESEARCH_OPTION; ++techId )
		{
			TechInfo *techInfo = tech_res[techId];

			if( techInfo->race_id == race_id && tech_res[techId]->can_research(nation_recno, 0) )
			{
				button_research_array[added_count].create( x, y +added_count*ySpacing, x +187, y +added_count*ySpacing +34,
					i_disp_research_button, ButtonCustomPara(this, techId) );

				++added_count;
			}
		}
	}

	// -------- display research button --------//

	for( int i = 0; i < added_count; ++i )
	{
		int techId = button_research_array[i].custom_para.value;

		button_research_array[i].enable_flag = nation_recno && tech_res[techId]->can_research(nation_recno);
		button_research_array[i].paint();
	}

	x2 +=  3 * BUTTON_DISTANCE;
	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( x2, y2, 'A', "CANCEL" );
	}

	button_cancel.paint();
}
//--------- End of function FirmLair::disp_research_menu ---------//


//--------- Begin of function FirmLair::detect_research_menu ---------//
//
void FirmLair::detect_research_menu()
{
	// -------- detect research button --------//

	if( nation_recno )
	{
		for( int i = 0; i < added_count; ++i )
		{
			if( button_research_array[i].detect() )
			{
				int techId = button_research_array[i].custom_para.value;
				buy_tech( techId, COMMAND_PLAYER );

				disp_research_menu(INFO_REPAINT);
				se_ctrl.immediate_sound("TURN_ON");
				return;
			}
		}
	}

	if( button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}
}
//--------- End of function FirmLair::detect_research_menu ---------//


//-------- Begin of static function i_disp_research_button --------//
//
void i_disp_research_button(ButtonCustom *button, int repaintBody)
{
	int techId = button->custom_para.value;
	String str; //, str2;
	TechInfo* techInfo = tech_res[techId];
	Firm *firmPtr = (Firm *) button->custom_para.ptr;

	int researchVersion = techInfo->get_nation_tech_level(firmPtr->nation_recno)+1;		// research the next non-researched version
	// ###### begin Gilbert 22/12 ######//
	int maxTechLevel = techInfo->max_tech_level( firmPtr->nation_recno );
	if( researchVersion > maxTechLevel )
		researchVersion = maxTechLevel;
	// ###### end Gilbert 22/12 ######//

	int midY = (button->y1 + button->y2) / 2; 

	str = techInfo->tech_des();
	if( researchVersion > 1 )
	{
		str += " ";
		str += m.roman_number(researchVersion);
	}
//	str2 = "Life Point Cost : ";
//	str2 += techInfo->live_points_cost();

	short fontHeight = font_bld.font_height;		// backup font_height
	font_bld.use_max_height();

	if ( !button->enable_flag )
	{
		// vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2 );
		font_bld.center_put(button->x1, button->y1+2, button->x2, midY, str );
		font_bld.center_put(button->x1, midY, button->x2, button->y2-2, 
			text_firm.str_lair_research(techInfo->live_points_cost()) );
		vga.active_buf->bar_alpha(button->x1, button->y1, button->x2, button->y2, 1, 0);
	}
	else
	if ( button->pushed_flag )
	{
		// vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDWN") );
		vga.active_buf->d3_panel_down( button->x1, button->y1, button->x2, button->y2 );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1, button->y1+2, button->x2, midY, str );
		font_bld.center_put(button->x1, midY, button->x2, button->y2-2,
			text_firm.str_lair_research(techInfo->live_points_cost()) );
	}
	else
	if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		// vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSEL") );
		vga.active_buf->bar( button->x1, button->y1, button->x2, button->y2, V_WHITE );
		vga.active_buf->draw_d3_up_border(button->x1, button->y1, button->x2, button->y2);
		font_bld.center_put(button->x1, button->y1+2, button->x2, midY, str );
		font_bld.center_put(button->x1, midY, button->x2, button->y2-2,
			text_firm.str_lair_research(techInfo->live_points_cost()) );
	}
	else
	{
		// vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDUP") );
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2 );
		font_bld.center_put(button->x1, button->y1+2, button->x2, midY, str );
		font_bld.center_put(button->x1, midY, button->x2, button->y2-2, 
			text_firm.str_lair_research(techInfo->live_points_cost()) );
	}

	font_bld.font_height = fontHeight;		// restore font height
}
//--------- End of static function i_disp_research_button ---------//


//--------- Begin of function FirmLair::disp_grant_menu ---------//
//
void FirmLair::disp_grant_menu(int refreshFlag)
{
	int x=INFO_X1+23, y=INFO_Y1+15;
	int ySpacing = 40;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

//	String str;
//	str  = "It costs $";
//	str += grant_to_non_own_lair_cost();
//	str += " to buy the loyalty of this lair.";
//	if( can_grant_to_non_own_lair(nation_array.player_recno, 1) )
//		str += " Proceed? ";
//	font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, str, 2, 0, 0);

	font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, 
		text_firm.str_buy_lair( grant_to_non_own_lair_cost(), can_grant_to_non_own_lair(nation_array.player_recno, 1)),
		2, 0, 0);

	if( !can_grant_to_non_own_lair(nation_array.player_recno, 1) )
	{
//		str = "You do not have enough money!";
//		font_whbl.center_put_paragraph( INFO_X1 +20 , INFO_Y1 + 80, INFO_X2 -10, INFO_Y1 +100, str, 2, 0, 0 );
		font_whbl.center_put_paragraph( INFO_X1 +20 , INFO_Y1 + 80, INFO_X2 -10, INFO_Y1 +120, 
			text_firm.str_buy_lair_short_cash(), 2, 0, 0 );
	}

	if( can_grant_to_non_own_lair(nation_array.player_recno, 1) )
	{
		button_yes.paint_text( INFO_X1+50, INFO_Y1+100, text_firm.str_yes() ); // "Yes" );
		button_no.paint_text( INFO_X1+155, INFO_Y1+100, text_firm.str_no() ); //  "No" );
	}
	else
	{
		button_yes.reset();
		button_no.paint_text( INFO_X1+102, INFO_Y1+122, text_basic.str_box_ok() ); // "Ok" );
	}
}
//--------- End of function FirmLair::disp_grant_menu ---------//


//--------- Begin of function FirmLair::detect_grant_menu ---------//
//
void FirmLair::detect_grant_menu()
{
	if( button_yes.detect() )
	{
		grant_to_non_own_lair(nation_array.player_recno, COMMAND_PLAYER);
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TAXGRANT");
		return;
	}

	if( button_no.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}
}
//--------- End of function FirmLair::detect_grant_menu ---------//


//-------- Begin of function FirmLair::should_show_info ------//
//
// Whether information of this firm should be shown.
//
int FirmLair::should_show_info()
{
	if (FirmCamp::should_show_info() || can_grant_to_non_own_lair(nation_array.player_recno, 0))
		return 1;
	else 
		return 0;
}
//---------- End of function FirmLair::should_show_info --------//

