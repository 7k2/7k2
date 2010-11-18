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

//Filename    : OF_WAR.CPP
//Description : Firm War Factory - interface functions

#include <of_war.h>
#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
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
#include <otech.h>
#include <otechres.h>
#include <obitmap.h>
#include <ogame.h>
#include <oconfig.h>
#include <ot_firm.h>

static int animate_seqno =1;
static char	last_menu_mode;

static short edit_progress_x1, edit_progress_y1, edit_progress_x2, edit_progress_y2, edit_progress_enable;

//------------- Define constant ------------//

#ifdef AMPLUS
	#define BUILD_BUTTON_WIDTH 	201
	#define BUILD_BUTTON_HEIGHT 	46
	#define COUNT_BUTTON_OFFSET_X	156
	#define COUNT_BUTTON_OFFSET_Y 7
	#define COUNT_BUTTON_WIDTH		30
	#define COUNT_BUTTON_HEIGHT	30
	#define BUILD_UNIT_ICON_OFFSET_X 6
	#define BUILD_UNIT_ICON_OFFSET_Y 4
	#define BUILD_UNIT_NAME_OFFSET_X 60
	#define BUILD_UNIT_NAME_OFFSET_Y 12
#else
	#define BUILD_BUTTON_WIDTH 	201
	#define BUILD_BUTTON_HEIGHT 	56
	#define COUNT_BUTTON_OFFSET_X	156
	#define COUNT_BUTTON_OFFSET_Y 12
	#define COUNT_BUTTON_WIDTH		32
	#define COUNT_BUTTON_HEIGHT	32
	#define BUILD_UNIT_ICON_OFFSET_X 6
	#define BUILD_UNIT_ICON_OFFSET_Y 8
	#define BUILD_UNIT_NAME_OFFSET_X 60
	#define BUILD_UNIT_NAME_OFFSET_Y 18
#endif

//---------- Define constant ------------//

enum { WAR_MENU_MAIN,
		 WAR_MENU_BUILD,
	  };

//--------- define static variables ----------//

static Button3D 		button_select_build, button_select_build2;
static Button3D		button_cancel_build;
static Button3D		button_go_rally, button_clear_rally;
static char     		war_menu_mode;
static short			war_tech_class;
//static char     		disable_refresh=0;
static short	 		added_count;
static ButtonCustom 	button_weapon[MAX_WEAPON_TYPE];
static ButtonCustom 	button_queue_weapon[MAX_WEAPON_TYPE];
static Button3D	 	button_cancel;


// --------- declare static function ----------//

static void i_disp_build_button(ButtonCustom *button, int);
static void i_disp_queue_button(ButtonCustom *button, int);


//--------- Begin of function FirmWar::put_info ---------//
//

void FirmWar::put_info(int refreshFlag)
{
	if( refreshFlag==INFO_REPAINT )
	//if( refreshFlag==INFO_REPAINT && !disable_refresh )
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
//----------- End of function FirmWar::put_info -----------//


//--------- Begin of function FirmWar::detect_info ---------//
//
void FirmWar::detect_info()
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
//----------- End of function FirmWar::detect_info -----------//



//--------- Begin of function FirmWar::disp_war_info ---------//
//
void FirmWar::disp_war_info(int dispY1, int refreshFlag)
{
	static short lastUnitId=0;
	String str;

	if( refreshFlag==INFO_UPDATE && lastUnitId != build_unit_id )
	{
		lastUnitId = build_unit_id;
		info.disp();
	}

	//---------------- paint the panel --------------//

	// #### begin Gilbert 5/10 ######//
	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel_build.create(INFO_X1 +182, INFO_Y1 +5, "WRFC_X-U", "WRFC_X-D");
		button_cancel_build.set_help_code( "CANCELWP" );
	}
	// #### end Gilbert 5/10 ######//

	if( !build_unit_id )
	{
//		str = "No Weapon";
//		font_snds.put( INFO_X1+25, INFO_Y1+13, str );
//		str = "is under construction !";
//		font_snds.put( INFO_X1+25, INFO_Y1+31, str );
		font_snds.put_paragraph( INFO_X1+25, INFO_Y1+13, INFO_X1+140, INFO_Y1+46,
			text_firm.str_no_build_weapon() );

		// ##### begin Gilbert 5/10 #######//
		button_cancel_build.enable_flag = 0;
		// ##### end Gilbert 5/10 #######//
	}
	else
	{
		//-------- display the icon of the researching item ---------//

		UnitInfo* unitInfo = unit_res[build_unit_id];
		
	//	info.draw_unit_icon( INFO_X1 +53, INFO_Y1 +110, build_unit_id,
	//			nation_recno, INFO_X1+12, INFO_Y1+55, INFO_X2, INFO_Y2, 1);


		Bitmap *bitmapPtr = (Bitmap *) unitInfo->soldier_icon_ptr;
	//	int x1 = INFO_X1 +53 + unitInfo->unit_icon_offsetx;
	//	int y1 = INFO_Y1 +110 + unitInfo->unit_icon_offsety;
	//	int x2 = x1 + bitmapPtr->get_width() -1;
	//	int y2 = y1 + bitmapPtr->get_height() -1;
		
		int x1 = INFO_X1 + 51 - bitmapPtr->get_width()/2;
		int y1 = INFO_Y1 + 139 - bitmapPtr->get_height();
		int x2 = x1 + bitmapPtr->get_width() -1;
		int y2 = y1 + bitmapPtr->get_height() -1;
		
		int srcX1 = max(x1, INFO_X1+13)-x1;
		int srcY1 = 0;
		int srcX2 = min(x2, INFO_X2)-x1;
		int srcY2 = min(y2, INFO_Y2)-y1;
	
		short *colorRemapTable = game.get_color_remap_table(nation_recno, 1);
		vga.active_buf->put_bitmap_area_trans_remap_decompress(x1, y1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);

		str = unitInfo->name;
		// ##### begin Gilbert 24/3 #####//
		// if( unitInfo->unit_class == UNIT_CLASS_WEAPON )		// add version no.
		if( unitInfo->class_info.has_weapon_version )		// add version no.
		// ##### end Gilbert 24/3 #####//
		{
			int techLevel = unitInfo->get_nation_tech_level(nation_recno);
			if( techLevel > 1 )
			{
				str += " ";
				str += m.roman_number(techLevel);
			}
		}
		font_snds.center_put( INFO_X1+18, INFO_Y1+13, INFO_X2, INFO_Y1+25, str, 0, 1 );

		//-- unitInfo->build_days is the no. of days take to build the firm when productivity is 100, the actually no. of days will be longer if productivity is less than 100 --//
		// ##### begin Gilbert 30/12 #####//
		float buildProgressDays = build_progress_days;
//										  + (float) (sys.frame_count-last_process_build_frame_no) / FRAMES_PER_DAY
//										  * (float) (worker_count*6+productivity/2) / 100;
		// ##### end Gilbert 30/12 #####//

	//	str = (int) buildProgressDays;
	//	str += " / ";
	//	str += (int) unitInfo->build_days; 
	//	font_snds.put( INFO_X1+25, INFO_Y1+31, str );
		int x = font_snds.put( INFO_X1+100, INFO_Y1+31, (int) buildProgressDays );
		x = font_snds.put( x, INFO_Y1+31, " / " );
		font_snds.put( x, INFO_Y1+31, (int) unitInfo->build_days );

		// ##### begin Gilbert 5/10 #######//
		button_cancel_build.enable_flag = 1;
		button_cancel_build.paint();
		// ##### end Gilbert 5/10 #######//
	}
}
//----------- End of function FirmWar::disp_war_info -----------//

//--------- Begin of function FirmWar::disp_main_menu ---------//
//
void FirmWar::disp_main_menu(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("WRFCBASE") );

	// #### begin Gilbert 5/10 ######//

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
		button_select_build.create( x2, y2, 'A', "MAKEWEAP" );
		button_select_build.paint();
	}	
*/
	// #### end Gilbert 5/10 ######//
}
//----------- End of function FirmWar::disp_main_menu -----------//


//--------- Begin of function FirmWar::detect_main_menu ---------//
//
void FirmWar::detect_main_menu()
{
	detect_firm_info();
	if( !should_show_info() )
		return;

	if( !is_own() )
		return;

	//----------- detect worker -----------//
	detect_worker_list();
	detect_worker_info();


	//------ detect the select research button -------//

//	if( button_select_build.detect() )
//	{
//		war_menu_mode = WAR_MENU_BUILD;
	//	disable_refresh = 1;    // static var for disp_info() only
	//	info.disp();
	//	disable_refresh = 0;
//	}
}
//----------- End of function FirmWar::detect_main_menu -----------//


//--------- Begin of function FirmWar::disp_build_menu ---------//
//
void FirmWar::disp_build_menu(int refreshFlag)
{
	int x1 = INFO_X1+13;
	int y1 = INFO_Y1+5;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGWEAP") );


	if( refreshFlag == INFO_REPAINT )
	{
		added_count=0;
	}
	int b = 0;

	TechClass *techClass = tech_res.tech_class(war_tech_class);	// war_tech_class is set the which tech class of weapon to build

	// int x=INFO_X1+2, y=INFO_Y1;

	for( int techId = techClass->first_tech_id; techId<techClass->first_tech_id+techClass->tech_count; ++techId )
	{
		short unitId = tech_res[techId]->unit_id;
		if( !unitId )
			continue;

		UnitInfo* unitInfo = unit_res[unitId];

		if( unitInfo->unit_class != UNIT_CLASS_WEAPON )
			continue;

		if( refreshFlag == INFO_REPAINT )
		{
			err_when( b != added_count);

			button_queue_weapon[added_count].create(x1+20, y1+5, x1+49, y1+34, 
				i_disp_queue_button, ButtonCustomPara(this, unitId) );
			button_queue_weapon[added_count].set_help_code( "WEAPNUM" );

			button_weapon[added_count].create(x1, y1+35, x1+67, y1+114,
				i_disp_build_button, ButtonCustomPara(&button_queue_weapon[added_count], unitId) );

			added_count++;
			x1 += 68;
			if (added_count == 3)
			{
				x1 = INFO_X1+13;
				y1 = INFO_Y1+121;
			}

			err_when(added_count > MAX_WEAPON_TYPE);
		}

		if( unitInfo->get_nation_tech_level(nation_recno) > 0 )
			button_queue_weapon[b].visible_flag = button_weapon[b].visible_flag = 1;
		else
			button_queue_weapon[b].visible_flag = button_weapon[b].visible_flag = 0;

		button_queue_weapon[b].paint();
		button_weapon[b].paint();

		++b;
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
//----------- End of function FirmWar::disp_build_menu -----------//


//--------- Begin of function FirmWar::detect_build_menu ---------//
//
void FirmWar::detect_build_menu()
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
//----------- End of function FirmWar::detect_build_menu -----------//


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
		button->pushed_flag ? 4 : 1 );
	
	ButtonCustom *queueButton = (ButtonCustom *)button->custom_para.ptr;
	err_when( button->custom_para.value != queueButton->custom_para.value);
	//-------- display unit name --------//
	String str;
	str = unitInfo->name;

	// ##### begin Gilbert 24/3 #########//
	// if( unitInfo->unit_class == UNIT_CLASS_WEAPON )		// add version no.
	if( unitInfo->class_info.has_weapon_version )		// add version no.
	// ##### end Gilbert 24/3 #########//
	{
		FirmWar *warFactory = (FirmWar *)queueButton->custom_para.ptr;
		int techLevel = unitInfo->get_nation_tech_level(warFactory->nation_recno);
		if( techLevel > 1 )		// no space for version
		{
			str += " ";
			str += m.roman_number(techLevel);
		}
	}
	
//	font_whbl.center_put( x1-3, y1, x1 +67+3, y1+20, str, 0, 1 );
	font_whbl.center_put( x1-3, y1, x1 +67+3, y1+20, str, 0, 0 );
	font_whbl.center_put( x1-3, y2-22, x1 +67+3, y2-2, m.format(unit_res[unitId]->build_cost, 2));

	// display small button
	queueButton->paint(-1, repaintBody);
}
//--------- End of static function i_disp_build_button ---------//


//-------- Begin of static function i_disp_queue_button --------//
//
static void i_disp_queue_button(ButtonCustom *button, int repaintBody)
{
	FirmWar *warFactory = (FirmWar *)button->custom_para.ptr;

	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	vga.active_buf->put_bitmap( x1, y1, image_button.read( button->pushed_flag ? "NUM-D" : "NUM-U" ) );
	
	//----- count the no. of units queued for this weapon ------//

	short unitId = button->custom_para.value;
	int queuedCount=0;
	for( int i=0 ; i<warFactory->build_queue_count ; i++ )
	{
		if( warFactory->build_queue_array[i] == unitId )
			queuedCount++;
	}
	if( warFactory->build_unit_id == unitId)
		queuedCount++;

	font_bible.center_put( x1, y1, x2, y2, m.format(queuedCount));
}
//--------- End of static function i_disp_queue_button ---------//



// -------- begin of function FirmWar::disp_firm_info --------//
//
void FirmWar::disp_firm_info(int dispY1, int refreshFlag)
{
	String str;

	if( refreshFlag == INFO_REPAINT )
	{
		button_select_build.create( INFO_X1 +13, INFO_Y1 +281, 'A', "MAKEWEAP" );
		button_select_build2.create( INFO_X1 +13 + BUTTON_DISTANCE, INFO_Y1 +281, 'A', "MAKEWEAP" );

		// ###### begin Gilbert 4/5 ########//
		button_go_rally.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+281, 'A', "RALLY-GO" );
		button_clear_rally.create( INFO_X1+13+BUTTON_DISTANCE*3, INFO_Y1+281, 'A', "RALLY-NO" );
		// ###### end Gilbert 4/5 ########//
	}

	disp_war_info(INFO_Y1+54, refreshFlag);

	if ( (int) productivity > 0  && build_unit_id )
	{
		int limit;
		if ( (int) productivity >  0 && (int) productivity <= 10 )
			limit = 6;
		if ( (int) productivity > 10 && (int) productivity <= 25 )
			limit = 5;
		if ( (int) productivity > 25 && (int) productivity <= 45 )
			limit = 4;
		if ( (int) productivity > 45 && (int) productivity <= 70 )
			limit = 3;
		if ( (int) productivity > 70 && (int) productivity <=100 )
			limit = 2;
		if ( (int) productivity >100 )
			limit = 1;
		
		if ( animate_seqno >=(limit*3) )
			animate_seqno = 1;
		
		char animate[] = "GEARW-0";
		animate[6] = '3' - animate_seqno / limit;

		if (config.frame_speed != 0)
			animate_seqno ++;

		vga.active_buf->put_bitmap( INFO_X1+140, INFO_Y1+61, image_icon.read( animate ) );	
	}

	if( is_own() )
	{
		button_select_build.paint();

#ifdef DEMO
		button_select_build2.visible_flag = 0;
#else
		button_select_build2.visible_flag = nation_recno 
			&& tech_res.tech_class(TECH_CLASS_MEGA_WEAPON)->total_tech_count(nation_recno) > 0;
#endif
		button_select_build2.paint();

		// ------- display rally points button ------//

		if( (button_go_rally.visible_flag = rally_enable_flag != 0) )
			button_go_rally.paint();

		if( (button_clear_rally.visible_flag = rally_enable_flag != 0) )
			button_clear_rally.paint();
	}	
}
// -------- end of function FirmWar::disp_firm_info --------//


// -------- begin of function FirmWar::detect_firm_info --------//
//
void FirmWar::detect_firm_info()
{
	if( is_own() )
	{
		//---------- detect cancel build ------------//

		if(build_unit_id)
		{
			if(button_cancel_build.detect())
			{
				se_ctrl.immediate_sound("TURN_OFF");
				if( !remote.is_enable() )
					cancel_build_unit();
				else
				{
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_WAR_SKIP_WEAPON, sizeof(short) );
					shortPtr[0] = firm_recno;
				}
			}
		}

		//------ detect the select build button -------//

		if( button_select_build.detect() )
		{
			war_tech_class = TECH_CLASS_WEAPON;
			war_menu_mode = WAR_MENU_BUILD;
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( button_select_build2.detect() )
		{
			war_tech_class = TECH_CLASS_MEGA_WEAPON;
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
			se_ctrl.immediate_sound("TURN_OFF");
		}
	}
}
// -------- end of function FirmWar::disp_firm_info --------//


// --------- begin of function FirmWar::disp_edit_mode -------//
//
void FirmWar::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	FirmWork::disp_edit_mode( refreshFlag, y, dispBG);

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
// --------- end of function FirmWar::disp_edit_mode -------//


// --------- begin of function FirmWar::detect_edit_mode -------//
//
int FirmWar::detect_edit_mode()
{
	int rc;
	if( (rc = FirmWork::detect_edit_mode()) )
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
// --------- end of function FirmWar::detect_edit_mode -------//
