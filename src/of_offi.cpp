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

// Filename     : OF_OFFI.CPP
// Description : magic tower interface function


#include <of_off.h>
#include <of_magi.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <oimgres.h>
#include <obutt3d.h>
#include <obuttcus.h>
#include <ohelp.h>
#include <oinfo.h>
#include <opower.h>
#include <otech.h>
#include <otechres.h>
#include <oworldmt.h>
#include <ounit.h>
#include <onation.h>
#include <obitmapw.h>
#include <obitmap.h>
#include <osys.h>
#include <ose.h>
#include <oseres.h>
#include <ot_firm.h>

// ------- define static variable ------//

static char menu_mode;
static char last_menu_mode;
static char menu_mode_para;

// Button3D button_magic[MAGIC_COUNT-2];
//static ButtonCustom button_magic[1];  //bug
static Button3D button_cancel;
static Button3D button_cast;

//// ------- declare static function -------//
//static void disp_magic_button(ButtonCustom *, int);

void FirmOffensive::put_info(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
		last_menu_mode = menu_mode = FIRM_MAGIC_MENU_MAIN;
	else if( last_menu_mode != menu_mode )
	{
		refreshFlag = INFO_REPAINT;
		last_menu_mode = menu_mode;
	}

	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_MAIN:
		disp_main_menu(refreshFlag);
		break;

	case FIRM_MAGIC_MENU_CAST:
	case FIRM_MAGIC_MENU_UNIT:
		disp_magic_menu(refreshFlag);
		break;

	default:
		err_here();
	}
}

void FirmOffensive::detect_info()
{
	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_MAIN:
		detect_main_menu();
		break;

	case FIRM_MAGIC_MENU_CAST:
	case FIRM_MAGIC_MENU_UNIT:
		detect_magic_menu();
		break;

	default:
		err_here();
	}
}


void FirmOffensive::disp_main_menu(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	if( refreshFlag == INFO_REPAINT )
		button_cast.create( INFO_X1 +13, INFO_Y1 +259, 'A', "GBA-1" );
	
	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	// display delay

	if( cast_delay > 0 )
	{
		short*	hitPointBitmap =NULL;
		int ratio = cast_delay *40 / cast_delay_max;
		int size = cast_delay *76 / cast_delay_max;
		
		//106 x 35 --- 15 to 90 ie. 0 to 40
		int offsetX = -35;
		int offsetY = -40;
		hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	
		if (ratio <11)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_10"));
		else
		if (ratio <40)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_39"));
		else
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_40"));
		
		vga.active_buf->read_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, INFO_X1 +94 +20 +size +offsetX, INFO_Y1 +80 +offsetY, hitPointBitmap );
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_00"));
		vga.active_buf->put_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, hitPointBitmap );
		mem_del( hitPointBitmap );				
	}

	if( !is_own() )
		return;

	char fileName[] = "GBA-1";
	
	if (button_cast.enable_flag)
		fileName[4] = '1' + (char) (sys.frame_count%3);

	button_cast.update_bitmap( fileName );
	button_cast.enable_flag = can_cast(magic_id_upper_limit);
	button_cast.paint();
}

void FirmOffensive::detect_main_menu()
{
	Firm::detect_info();

	if( !is_own() )
		return;

	String str;

	if( button_cast.detect() )
	{
		menu_mode_para = magic_id_upper_limit;
		se_ctrl.immediate_sound("TURN_ON");
		switch(magic_id_upper_limit) //bug
		{
			case MAGIC_FREEZE_LASER:
				cast_magic( MAGIC_FREEZE_LASER, center_x, center_y, 0, COMMAND_PLAYER );
				break;

			case MAGIC_GOD_POWER:
				menu_mode = FIRM_MAGIC_MENU_CAST;
				power.issue_command(COMMAND_FIRM_MAGIC, firm_recno, menu_mode_para);
				break;

			case MAGIC_LASER:
				cast_magic( MAGIC_LASER, center_x, center_y, 0, COMMAND_PLAYER );
				break;

			default:
				err_here();
		}
	}

	switch(magic_id_upper_limit) //bug
	{
		case MAGIC_FREEZE_LASER:
			help.set_help( INFO_X1+13, INFO_Y1+261, INFO_X1+13+44, INFO_Y1+280+48, "MAGIC7" );
			break;

		case MAGIC_GOD_POWER:
			help.set_help( INFO_X1+13, INFO_Y1+261, INFO_X1+13+44, INFO_Y1+280+48, "MAGIC6" );
			break;

		case MAGIC_LASER:
			help.set_help( INFO_X1+13, INFO_Y1+261, INFO_X1+13+44, INFO_Y1+280+48, "MAGIC5" );
			break;

		default:
			err_here();
	}
}


void FirmOffensive::disp_magic_menu(int refreshFlag)
{
	// ----- create cancel button -------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
	}

	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap(INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( !is_own() )
		return;

	// display instruction
//	String str;

	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_CAST:
		font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +20, INFO_X2 -10, INFO_Y1 +60, 
			text_firm.str_cast_magic_instruct(menu_mode_para), 2, 0, 0);
		break;

	case FIRM_MAGIC_MENU_UNIT:
		// str = "Select a Unit to Teleport";
		font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +20, INFO_X2 -10, INFO_Y1 +60, 
			text_firm.str_cast_teleport_source(), 2, 0, 0);
		break;

	default:
		err_here();
	}

	// display magic name

//	font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +60, INFO_X2 -10, INFO_Y1 +80,
//		tech_res[magic_info_array[menu_mode_para-1].tech_id]->tech_des(), 2, 0, 0);

	// display cost

	if( menu_mode == FIRM_MAGIC_MENU_CAST )
	{
		char mobileType;
		int pointedXLoc;
		int pointedYLoc;

		if( world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &pointedXLoc, &pointedYLoc, &mobileType) )
		{
			//	int x2 = font_snds.put( INFO_X1+20, INFO_Y1+70, "Life Point Cost : ");
			//	font_snds.put( x2, INFO_Y1+70, calc_live_point_cost( menu_mode_para, pointedXLoc, pointedYLoc, teleport_unit_recno) );
			switch( menu_mode_para )
			{
			case MAGIC_DESTROY_BUILDING:
			case MAGIC_FREEZE_UNIT:
			case MAGIC_DAMAGE_AREA:
			case MAGIC_GOD_POWER:
				font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +80, INFO_X2 -10, INFO_Y1 +120, 
					text_firm.str_cast_cost_cash(calc_cash_cost( menu_mode_para, pointedXLoc, pointedYLoc, 0)),
					2, 0, 0);
				break;

			case MAGIC_TELEPORT:
				font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +80, INFO_X2 -10, INFO_Y1 +120,
					text_firm.str_teleport_cost_cash(calc_cash_cost( menu_mode_para, pointedXLoc, pointedYLoc, teleport_unit_recno)),
					2, 0, 0);
				break;
			}
		}
	}

	// display cancel button

	button_cancel.paint();
}


void FirmOffensive::detect_magic_menu()
{
	// detect cancel button

	if( button_cancel.detect() )
	{
		menu_mode = FIRM_MAGIC_MENU_MAIN;
		menu_mode_para = 0;
		// ##### begin Gilbert 26/1 #####//
		power.reset_command();
		se_ctrl.immediate_sound("TURN_OFF");
		// ##### end Gilbert 26/1 #####//
	}
}
/*

// ---------- begin of static function disp_firm_button --------//
static void disp_magic_button(ButtonCustom *button, int)
{
	char *magicName = tech_res[FirmMagic::magic_info_array[button->custom_para.value-1 +4].tech_id]->tech_des();

	if ( !button->enable_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
		vga.active_buf->bar_alpha(button->x1+1, button->y1+1, button->x2-1, button->y2-1, 1, 0);
	}
	else if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDWN") );
//		vga_util.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1+1, button->y1-1, button->x2+1, button->y2-1, magicName );
	}
	else if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSEL") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
	}
	else
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
	}
}
// ---------- end of static function disp_firm_button --------//
*/
