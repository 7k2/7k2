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

// Filename    : OF_INCUI.CPP
// Description : interface functions of Incubator


#include <of_incu.h>
#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <ocoltbl.h>
#include <ostr.h>
#include <oconfig.h>
#include <ofont.h>
#include <omouse.h>
#include <obutt3d.h>
#include <ounit.h>
#include <osys.h>
#include <onation.h>
#include <oremote.h>
#include <oimgres.h>
#include <ose.h>
#include <oseres.h>
#include <obuttcus.h>
#include <ot_firm.h>


static int animate_seqno =1;
static char	last_menu_mode;

//---------- Define constant ------------//

enum { WAR_MENU_MAIN,
		 WAR_MENU_BUILD,
	  };

//--------- define static variables ----------//

static Button3D 		button_select_build;
static Button3D		button_cancel_build;
static Button3D		button_go_rally, button_clear_rally;
static char     		war_menu_mode;
static short	 		added_count;
static ButtonCustom 	button_weapon[MAX_WEAPON_TYPE];
static ButtonCustom 	button_queue_weapon[MAX_WEAPON_TYPE];
static Button3D	 	button_cancel;

static short edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, edit_progress_enable;

// --------- declare static function ----------//

static void i_disp_build_button(ButtonCustom *button, int);
static void i_disp_queue_button(ButtonCustom *button, int);


void FirmIncubator::put_info(int refreshFlag)
{
	if( refreshFlag==INFO_REPAINT )
		last_menu_mode = war_menu_mode = WAR_MENU_MAIN;
	else
	{
		if( last_menu_mode != war_menu_mode )		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = war_menu_mode;
		}
	}

	switch( war_menu_mode )
	{
		case WAR_MENU_MAIN:
			disp_main_menu(refreshFlag);
			break;

		case WAR_MENU_BUILD:
			disp_build_menu(refreshFlag);
			break;
	}
}

void FirmIncubator::detect_info()
{
	switch( war_menu_mode )
	{
		case WAR_MENU_MAIN:
			detect_main_menu();
			break;

		case WAR_MENU_BUILD:
			detect_build_menu();
			break;
	}
}



void FirmIncubator::disp_main_menu(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("WRFCBASE") );

	disp_firm_info(INFO_Y1, refreshFlag);
}


void FirmIncubator::detect_main_menu()
{
	Firm::detect_info();

	detect_firm_info();
}


void FirmIncubator::disp_firm_info(int dispY1, int refreshFlag)
{
	String str;

	if( refreshFlag == INFO_REPAINT )
	{
		button_select_build.create( INFO_X1 +13, INFO_Y1 +281, 'A', "MAKEWEAP" );

		// ###### begin Gilbert 11/3 ########//
		button_go_rally.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+235, 'A', "RALLY-GO" );
		button_clear_rally.create( INFO_X1+13+BUTTON_DISTANCE*3, INFO_Y1+235, 'A', "RALLY-NO" );
		// ###### end Gilbert 11/3 ########//
	}

	disp_war_info(INFO_Y1+54, refreshFlag);

	if ( build_unit_id )
	{
		int limit = 2;
		
		if ( animate_seqno >=(limit*3) )
			animate_seqno = 1;
		
		char animate[] = "GEARW-0";
		animate[6] = '3' - animate_seqno / limit;
		
		animate_seqno ++;

		vga.active_buf->put_bitmap( INFO_X1+140, INFO_Y1+61, image_icon.read( animate ) );	
	}

	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;
	button_select_build.enable_flag = 0;
	if( is_own() )
	{
		button_select_build.enable_flag = 1;
		button_select_build.paint();

		// ------- display rally points button ------//

		if( (button_go_rally.visible_flag = rally_enable_flag != 0) )
			button_go_rally.paint();

		if( (button_clear_rally.visible_flag = rally_enable_flag != 0) )
			button_clear_rally.paint();
	}	

}

void FirmIncubator::detect_firm_info()
{
	if( is_own() )
	{
		//---------- detect cancel build ------------//

		if(build_unit_id)
		{
			if(button_cancel_build.detect())
			{
				if( !remote.is_enable() )
					cancel_build_unit();
				else
				{
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_WAR_SKIP_WEAPON, sizeof(short) );
					shortPtr[0] = firm_recno;
				}
				se_ctrl.immediate_sound("TURN_OFF");
			}
		}

		//------ detect the select build button -------//

		if( button_select_build.detect() )
		{
			war_menu_mode = WAR_MENU_BUILD;
			se_ctrl.immediate_sound("TURN_ON");
		}

		// ------- display rally points button ------//

		if( button_go_rally.detect() )
		{
			world.go_loc( rally_dest_x, rally_dest_y, 0 );
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( button_clear_rally.detect() )
		{
			clear_rally_point(COMMAND_PLAYER);
			se_ctrl.immediate_sound("TURN_ON");
		}
	}
}


void FirmIncubator::disp_war_info(int dispY1, int refreshFlag)
{
	static short lastUnitId=0;
	String str;

	if( refreshFlag==INFO_UPDATE && lastUnitId != build_unit_id )
	{
		lastUnitId = build_unit_id;
		info.disp();
	}

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel_build.create(INFO_X1 +182, INFO_Y1 +5, "WRFC_X-U", "WRFC_X-D");
		button_cancel_build.set_help_code( "CANCELWP" );
	}

	if( !build_unit_id )
	{
//		str = "No Weapon";
//		font_snds.put( INFO_X1+25, INFO_Y1+13, str );
//		str = "is under construction !";
//		font_snds.put( INFO_X1+25, INFO_Y1+31, str );
		font_snds.put_paragraph( INFO_X1+25, INFO_Y1+13, INFO_X1+140, INFO_Y1+46,
			text_firm.str_no_build_weapon() );

		button_cancel_build.enable_flag = 0;
	}
	else
	{

		//-------- display the icon of the researching item ---------//

		UnitInfo* unitInfo = unit_res[build_unit_id];
		
		info.draw_unit_icon( INFO_X1 +53, INFO_Y1 +110, build_unit_id,
				nation_recno, INFO_X1+12, INFO_Y1+55, INFO_X2, INFO_Y2, 1);

		str = unitInfo->name;
		int techLevel = unitInfo->get_nation_tech_level(nation_recno);
		if( techLevel > 1 )
		{
			str += " ";
			str += m.roman_number(techLevel);
		}
		font_snds.put( INFO_X1+25, INFO_Y1+13, str, 0, -1, 1 );

		//-- unitInfo->build_days is the no. of days take to build the firm when productivity is 100, the actually no. of days will be longer if productivity is less than 100 --//
		float buildProgressDays = build_progress_days;

		int x = font_snds.put( INFO_X1+25, INFO_Y1+31, (int) buildProgressDays );
		x = font_snds.put( x, INFO_Y1+31, " / " );
		font_snds.put( x, INFO_Y1+31, (int) unitInfo->build_days );

		button_cancel_build.enable_flag = 1;
		button_cancel_build.paint();
	}
}


void FirmIncubator::disp_build_menu(int refreshFlag)
{
	int x1 = INFO_X1+13;
	int y1 = INFO_Y1+5;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGWEAP") );

	added_count = 0;
	int unitId, x=INFO_X1+2, y=INFO_Y1;
	UnitInfo* unitInfo;

	for( int i = 0; i < MAX_INCUBATE_TYPE; ++i )
	{
		unitId = incubate_unit_id[i];
		unitInfo = unit_res[unitId];

		if( refreshFlag == INFO_REPAINT )
		{
			button_queue_weapon[added_count].create(x1+20, y1+5, x1+49, y1+34, 
				i_disp_queue_button, ButtonCustomPara(this, unitId) );
			button_weapon[added_count].create(x1, y1+35, x1+67, y1+114,
				i_disp_build_button, ButtonCustomPara(&button_queue_weapon[added_count], unitId) );
		}

		button_weapon[added_count].visible_flag 
			= button_queue_weapon[added_count].visible_flag 
			= unitInfo->get_nation_tech_level(nation_recno) > 0;

		button_weapon[added_count].paint();
		button_queue_weapon[added_count].paint();

		added_count++;

		x1 += 68;
		if (added_count == 3)
		{
			x1 = INFO_X1+13;
			y1 = INFO_Y1+121;
		}
	}

	if( refreshFlag==INFO_REPAINT )
	{
		// ##### begin Gilbert 8/2 ######//
		int x1 = INFO_X1 +13 +BUTTON_DISTANCE*3;
		int y1 = INFO_Y1 +281;
		// button_cancel.create( x1+7, y1+9, "OK-U", "OK-D" );
		button_cancel.create( x1, y1, 'A', "CANCEL" );
		// ##### end Gilbert 8/2 ######//
	}
	button_cancel.paint();
}


void FirmIncubator::detect_build_menu()
{
	int 	 	 unitId, rc, quitFlag;
	UnitInfo* unitInfo;

	for( int b = 0; b < added_count; ++b)
	{
		unitId = button_weapon[b].custom_para.value;
		unitInfo = unit_res[unitId];

		//------ detect pressing on the small queue count button -------//

		rc = 0;
		quitFlag = 0;

		if( (rc = button_queue_weapon[b].detect(0,0,2)) != 0 )		// both button
		{
			quitFlag = 0;		// don't quit the menu right after pressing the button
		}

		//------ detect pressing on the big button -------//

		else if( (rc = button_weapon[b].detect(0,0,2)) != 0 )
		{
			quitFlag = 1;		// quit the menu right after pressing the button
		}

		//------- process the action --------//

		if( rc > 0 )
		{
			if( rc==1 )		// left button
			{
				if( remote.is_enable() )
				{
					// packet structure : <firm recno> <unit Id>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_WAR_BUILD_WEAPON, 2*sizeof(short) );
					shortPtr[0] = firm_recno;
					shortPtr[1] = unitId;
				}
				else
					add_queue(unitId);
				se_ctrl.immediate_sound("TURN_ON");
			}
			else 				// right button - remove queue
			{
				if( remote.is_enable() )
				{
					// packet structure : <firm recno> <unit Id>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_WAR_CANCEL_WEAPON, 2*sizeof(short) );
					shortPtr[0] = firm_recno;
					shortPtr[1] = unitId;
				}
				else
					remove_queue(unitId);
				se_ctrl.immediate_sound("TURN_OFF");
			}

			if( quitFlag )
				war_menu_mode = WAR_MENU_MAIN;
			return;
		}

	}
	//------ detect the cancel button --------//

	if( button_cancel.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		war_menu_mode = WAR_MENU_MAIN;
	}
}



void FirmIncubator::disp_build_button(int y, int unitId, int buttonUp)
{
}

void FirmIncubator::disp_queue_button(int y, int unitId, int buttonUp)
{
}

//-------- Begin of function i_disp_build_button --------//
//
static void i_disp_build_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;
	short unitId = button->custom_para.value;
	UnitInfo* unitInfo = unit_res[unitId];
	
	short nationRecno = firm_array[firm_array.selected_recno]->nation_recno;
	
	info.draw_unit_icon( x1+34, y1+46, unitId, nationRecno, INFO_X1, INFO_Y1, INFO_X2, INFO_Y2,
		button->pushed_flag ? 4 : 1 ) ;
	
	ButtonCustom *queueButton = (ButtonCustom *)button->custom_para.ptr;
	err_when( button->custom_para.value != queueButton->custom_para.value);
	//-------- display unit name --------//
	String str;
	str = unitInfo->name;

	FirmIncubator *incubator = (FirmIncubator *)queueButton->custom_para.ptr;
	int techLevel = unitInfo->get_nation_tech_level(incubator->nation_recno);
	if( techLevel > 1 )		// no space for version
	{
		str += " ";
		str += m.roman_number(techLevel);
	}

	// font_whbl.center_put( x1, y1, x1 +67, y1+20, str, 0, 1 );
	font_whbl.center_put( x1, y1, x1 +67, y1+20, str, 0, 0 );
	font_whbl.center_put( x1-3, y2-22, x1 +67+3, y2-2, m.format(unit_res[unitId]->build_cost, 2));

	// display small button
	queueButton->paint(-1, repaintBody);
}
//--------- End of static function i_disp_build_button ---------//


//-------- Begin of static function i_disp_queue_button --------//
//
static void i_disp_queue_button(ButtonCustom *button, int repaintBody)
{
	FirmIncubator *incubator = (FirmIncubator *)button->custom_para.ptr;

	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	char *bitmapPtr = image_button.read( button->pushed_flag ? (char*)"NUM-D" : (char*)"NUM-U" );
	vga.active_buf->put_bitmap_trans_decompress( x1, y1, bitmapPtr );
	
	//----- count the no. of units queued for this weapon ------//

	short unitId = button->custom_para.value;
	int queuedCount=0;
	for( int i=0 ; i<incubator->build_queue_count ; i++ )
	{
		if( incubator->build_queue_array[i] == unitId )
			queuedCount++;
	}
	if( incubator->build_unit_id == unitId)
		queuedCount++;

	font_bible.center_put( x1, y1, x2, y2, m.format(queuedCount));
}
//--------- End of static function i_disp_queue_button ---------//



// --------- begin of function FirmIncubator::disp_edit_mode -------//
//
void FirmIncubator::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit production progress

	edit_progress_enable = build_unit_id != 0;
	if( edit_progress_enable )
	{
		x = font_whbl.put( x1, y, unit_res[build_unit_id]->name, 0, x2 );
		x += 20;
		x = edit_progress_x2 = font_blue.put( (edit_progress_x1=x), (edit_progress_y1=y), (int)build_progress_days, 0, x2 );
		edit_progress_y2 = edit_progress_y1+font_blue.max_font_height;
		x = font_whbl.put( x, y, " / ", 0, x2 );
		x = font_whbl.put( x, y, unit_res[build_unit_id]->build_days, 0, x2 );
	}
	y += 18;
}
// --------- end of function FirmIncubator::disp_edit_mode -------//


// --------- begin of function FirmIncubator::detect_edit_mode -------//
//
int FirmIncubator::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	// detect production progress

	if( edit_progress_enable && build_unit_id )
	{
		if( mouse.any_click(edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, LEFT_BUTTON) )
		{
			build_progress_days += 10.0f;
			if( build_progress_days > unit_res[build_unit_id]->build_days )
				build_progress_days = unit_res[build_unit_id]->build_days;
			return 1;
		}
		if( mouse.any_click(edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, RIGHT_BUTTON) )
		{
			build_progress_days -= 10.0f;
			if( build_progress_days < 0.0f )
				build_progress_days = 0.0f;
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmIncubator::detect_edit_mode -------//
