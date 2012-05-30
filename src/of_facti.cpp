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

//Filename    : OF_FACTI.CPP
//Description : Firm Factory - Interface functions

#include <of_fact.h>
#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <ostr.h>
#include <ounit.h>
#include <ofont.h>
#include <omouse.h>
#include <obutt3d.h>
#include <obuttcus.h>
#include <onation.h>
#include <orawres.h>
#include <oraceres.h>
#include <of_mine.h>
#include <of_mark.h>
#include <oremote.h>
#include <ose.h>
#include <oimgres.h>
#include <oconfig.h>
#include <obitmap.h>
#include <ot_firm.h>
#include <ohelp.h>

//------- define static vars -------//

static Button3D	button_change;
static int animate_seqno = 0;
static ButtonCustomGroup button_auto_switch_group(2);	// 0=manual, 1=auto

static short edit_raw_x1, edit_raw_y1, edit_raw_x2, edit_raw_y2, edit_raw_enable;
static short edit_product_x1, edit_product_y1, edit_product_x2, edit_product_y2, edit_product_enable;

static void i_disp_auto_switch_button(ButtonCustom *button, int repaintBody);


// ##### begin Gilbert 5/10 #######//

//--------- Begin of function FirmFactory::put_info ---------//
//
void FirmFactory::put_info(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MIFABASE") );

	FirmWork::put_info(refreshFlag);

	/*
	//---------- display info ------------//

	disp_basic_info(INFO_Y1, refreshFlag);

	if( !should_show_info() )
		return;

	disp_factory_info(INFO_Y1+54, refreshFlag);
	disp_worker_list(INFO_Y1+126, refreshFlag);
	disp_worker_info(INFO_Y1+190, refreshFlag);

	//------ display button -------//

	int x;

	if( is_own() && refreshFlag==INFO_REPAINT )
	{
		button_change_production.paint( INFO_X1, INFO_Y1+248, 'A', "CHGPROD" );
		x = INFO_X1+BUTTON_ACTION_WIDTH;
	}
	else
		x = INFO_X1;

	//---------- display spy button ----------//

	disp_spy_button(x, INFO_Y1+248, refreshFlag);
	*/

}
//----------- End of function FirmFactory::put_info -----------//

//--------- Begin of function FirmFactory::detect_info ---------//
//
void FirmFactory::detect_info()
{
	
	FirmWork::detect_info();

	/*

	//-------- detect basic info -----------//

	if( detect_basic_info() )
		return;

	//-------- detect workers ----------//

	if( detect_worker_list() )		// detect this when: it's the player's firm or the player has spies in this firm
	{
		disp_worker_list(INFO_Y1+126, INFO_UPDATE);
		disp_worker_info(INFO_Y1+190, INFO_UPDATE);
	}

	//-------- detect spy button ----------//

	detect_spy_button();

	if( !is_own() )
		return;

	//---- detect change production button -----//

	if( button_change_production.detect() )
	{	
		change_production();
		disp_factory_info(INFO_Y1+54, INFO_UPDATE);
		se_ctrl.immediate_sound("TURN_ON");
	}
	*/
 }
//----------- End of function FirmFactory::detect_info -----------//
// ##### end Gilbert 5/10 #######//

//------- Begin of function FirmFactory::draw -----------//
//
// Draw product stocks.
//
void FirmFactory::draw(int displayLayer)
{
	Firm::draw(displayLayer);

	if( !should_show_info() )
		return;

	if( under_construction )
		return;

	if( product_raw_id && displayLayer == 1 )
	{
		int cargoCount = MAX_CARGO * (int)stock_qty / (int)max_stock_qty;

		// draw_cargo( MAX(1,cargoCount), raw_res.small_product_icon(product_raw_id) );
	}
}
//--------- End of function FirmFactory::draw -----------//


// -------- begin of function FirmFactory::disp_firm_info --------//
//
void FirmFactory::disp_firm_info(int dispY1, int refreshFlag)
{
	//---------- display production info -------------//

//	String str; //, str2;

//	str = "Raw";
//	font_snds.center_put( INFO_X1+10, INFO_Y1+5, INFO_X1+96, INFO_Y1+30, 
//		text_firm.str_factory_raw(), 0, 1 );
	
//	str = "Ware";
//	font_snds.center_put( INFO_X2-86+12, INFO_Y1+20, INFO_X2, INFO_Y1+45,
//		text_firm.str_factory_ware(), 0, 1 );

//	str = raw_res[product_raw_id]->name;
//	font_snds.center_put( INFO_X1+10, INFO_Y1+20, INFO_X1+96, INFO_Y1+45, 
//		raw_res[product_raw_id]->name, 0, 1 );
//	font_snds.center_put( INFO_X2-86+12, INFO_Y1+5, INFO_X2, INFO_Y1+30, 
//		raw_res[product_raw_id]->name, 0, 1 );

	font_snds.center_put_paragraph( INFO_X1+10, INFO_Y1+5, INFO_X1+96, INFO_Y1+45,
		text_firm.str_factory_raw(product_raw_id) );
	font_snds.center_put_paragraph( INFO_X2-86, INFO_Y1+5, INFO_X2, INFO_Y1+45,
		text_firm.str_factory_ware(product_raw_id) );
	
//	str  = (int) raw_stock_qty;
//	str += " / ";
//	str += (int) max_raw_stock_qty;
	if (raw_stock_qty >= max_raw_stock_qty)
		vga.active_buf->put_bitmap( INFO_X1+33, INFO_Y1+40+15, image_icon.read("FULLVILL") );	
	font_snds.center_put( INFO_X1+10, INFO_Y1+40, INFO_X1+96, INFO_Y1+55, 
		m.format((int)raw_stock_qty) );

//	str  = (int) stock_qty;
//	str += " / ";
//	str += (int) max_stock_qty;
	if (stock_qty >= max_stock_qty)
		vga.active_buf->put_bitmap( INFO_X2-64+6, INFO_Y1+40+15, image_icon.read("FULLVILL") );	
	font_snds.center_put( INFO_X2-86+12, INFO_Y1+40, INFO_X2, INFO_Y1+55, 
		m.format((int)stock_qty) );
 
	if ( (int) production_30days() > 0 )
//	if ( is_operating() )
	{
		int limit;
		if ( (int) production_30days() >  0 && (int) production_30days() <= 10 )
			limit = 6;
		if ( (int) production_30days() > 10 && (int) production_30days() <= 25 )
			limit = 5;
		if ( (int) production_30days() > 25 && (int) production_30days() <= 45 )
			limit = 4;
		if ( (int) production_30days() > 45 && (int) production_30days() <= 70 )
			limit = 3;
		if ( (int) production_30days() > 70 && (int) production_30days() <=100 )
			limit = 2;
		if ( (int) production_30days() >100 )
			limit = 1;

		if ( animate_seqno >=(limit*3) )
			animate_seqno = 0;

		char animate[] = "GEAR-0";
		animate[5] = '1' + animate_seqno / limit;

		if (config.frame_speed != 0)
			animate_seqno ++;

		vga.active_buf->put_bitmap( INFO_X1+93, INFO_Y1+36, image_icon.read( animate ) );
	}
	else
		vga.active_buf->put_bitmap( INFO_X1+93, INFO_Y1+36, image_icon.read( "GEAR-1" ) );
	
	if (product_raw_id == 1)
	{
		vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "CLY-RW_L" ) );	
		vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "POTFACT" ) );
	}
	else
	if (product_raw_id == 2)
	{
		vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "COP-RW_L" ) );	
		vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "COPGDFAC" ) );
	}
	else
	if (product_raw_id == 3)
	{
		vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "IRN-RW_L" ) );	
		vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "IRNGDFAC" ) );
	}
	else
		err_here();

//	str  = "Monthly";
//	font_snds.center_put( INFO_X1+17, INFO_Y1+90, INFO_X2, INFO_Y1+115, str );
//	str  = "Production";
//	font_snds.center_put( INFO_X1+17, INFO_Y1+106, INFO_X2, INFO_Y1+121, str );
	font_snds.center_put_paragraph( INFO_X1+72, INFO_Y1+85, INFO_X1+169, INFO_Y1+121,
		text_firm.str_monthly_production(), 0, 0, 0 );

//	str = (int) production_30days();
	font_snds.center_put( INFO_X1+17, INFO_Y1+117, INFO_X2, INFO_Y1+137, //str );
		m.format((int) production_30days()) );
	
	//---------------- paint button ------------------//
//	int x1 = INFO_X1 +13;
//	int y1 = INFO_Y1 +235;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	if( refreshFlag == INFO_REPAINT )
	{
		button_change.create( x2, y2, 'A', "CHGPROD" );

		// manual button
		button_auto_switch_group[0].create( INFO_X1+3*BUTTON_DISTANCE+13, INFO_Y1+281,
			INFO_X1+3*BUTTON_DISTANCE+56-1, INFO_Y1+281+BUTTON_ACTION_HEIGHT-1,
			i_disp_auto_switch_button, ButtonCustomPara(this, 0), 0 );
		
		// auto button
		button_auto_switch_group[1].create( INFO_X1+2*BUTTON_DISTANCE+13, INFO_Y1+281,
			INFO_X1+2*BUTTON_DISTANCE+56-1, INFO_Y1+281+BUTTON_ACTION_HEIGHT-1,
			i_disp_auto_switch_button, ButtonCustomPara(this, 1), 0 );
	}

	help.set_help( INFO_X1+3*BUTTON_DISTANCE+13, INFO_Y1+281,
		INFO_X1+3*BUTTON_DISTANCE+56-1, INFO_Y1+281+BUTTON_ACTION_HEIGHT-1, "MANUAL" );		
	help.set_help( INFO_X1+2*BUTTON_DISTANCE+13, INFO_Y1+281,
		INFO_X1+2*BUTTON_DISTANCE+56-1, INFO_Y1+281+BUTTON_ACTION_HEIGHT-1, "AUTO" );		

	if ( is_own() )
	{
		button_change.paint();

		button_auto_switch_group.paint( auto_switch_flag );
	}
}
// -------- end of function FirmFactory::disp_firm_info --------//


// -------- begin of function FirmFactory::detect_firm_info --------//
//
void FirmFactory::detect_firm_info()
{
	if( is_own() )
	{
		if( button_change.detect() )
		{	
			// ##### begin Gilbert 5/10 #####//
			change_production(COMMAND_PLAYER);
			// disp_firm_info(INFO_Y1+54, INFO_UPDATE);
			se_ctrl.immediate_sound("TURN_ON");
			// ##### end Gilbert 5/10 ######//
		}

		if( button_auto_switch_group.detect() >= 0 )
		{
			if( !remote.is_enable() )
			{
				auto_switch_flag = button_auto_switch_group[button_auto_switch_group()].custom_para.value;
			}
			else
			{
				// packet structure : <firm recno> <product id>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_FACTORY_AUTO, 2*sizeof(short) );
				shortPtr[0] = firm_recno;
				shortPtr[1] = button_auto_switch_group[button_auto_switch_group()].custom_para.value;
			}
			se_ctrl.immediate_sound("TURN_ON");
		}
	}
}
// -------- end of function FirmFactory::disp_firm_info --------//


//--------- Begin of static function i_disp_auto_switch_button ---------//
//
static void i_disp_auto_switch_button(ButtonCustom *button, int repaintBody)
{
	// line 1
	int lineHeight = font_whbl.max_font_height;
	int y = button->y1;
	font_whbl.center_put( button->x1, y, button->x2, y+lineHeight-1,
		button->custom_para.value ? text_firm.str_factory_auto() : text_firm.str_factory_manual() );
	y += font_whbl.max_font_height;
//	font_whbl.center_put( button->x1, y, button->x2, y+lineHeight-1,
//		"Trade" );

	// mark pushed

	if( button->pushed_flag )
	{
//		vga.active_buf->rect( button->x1, button->y1, button->x2, button->y2, 1, VGA_LIGHT_BLUE );
		char *triangleUp = image_button.read( "TRI-UP" );

		// put at centre buttom
		vga.active_buf->put_bitmap_trans_decompress ( button->x1 + (button->x2-button->x1-((Bitmap *)triangleUp)->get_width() )/2,
			button->y2 - ((Bitmap *)triangleUp)->get_height(), triangleUp  );
	}
}
//--------- End of static function i_disp_auto_switch_button ---------//


// --------- begin of function FirmFactory::disp_edit_mode -------//
//
void FirmFactory::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	FirmWork::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit raw quantity

	edit_raw_enable = 1;
	x = font_whbl.put( x1, y, text_firm.str_factory_raw(product_raw_id), 0, x2 );
	x += 20;
	x = edit_raw_x2 = font_blue.put( (edit_raw_x1=x), (edit_raw_y1=y), (int)raw_stock_qty, 0, x2 );
	edit_raw_y2 = edit_raw_y1+font_blue.max_font_height;
	y += 18;

	// edit product quantity

	edit_product_enable = 1;
	x = font_whbl.put( x1, y, text_firm.str_factory_ware(product_raw_id), 0, x2 );
	x += 20;
	x = edit_product_x2 = font_blue.put( (edit_product_x1=x), (edit_product_y1=y), (int)stock_qty, 0, x2 );
	edit_product_y2 = edit_product_y1+font_blue.max_font_height;
	y += 18;
}
// --------- end of function FirmFactory::disp_edit_mode -------//


// --------- begin of function FirmFactory::detect_edit_mode -------//
//
int FirmFactory::detect_edit_mode()
{
	int rc;
	if( (rc = FirmWork::detect_edit_mode()) )
		return rc;

	// edit raw quantity

	if( edit_raw_enable )
	{
		if( mouse.any_click(edit_raw_x1, edit_raw_y1, edit_raw_x2, edit_raw_y2, LEFT_BUTTON) )
		{
			raw_stock_qty += 100.0f;
			if( raw_stock_qty > max_raw_stock_qty )
				raw_stock_qty = max_raw_stock_qty;
			return 1;
		}
		if( mouse.any_click(edit_raw_x1, edit_raw_y1, edit_raw_x2, edit_raw_y2, RIGHT_BUTTON) )
		{
			raw_stock_qty -= 100.0f;
			if( raw_stock_qty < 0.0f )
				raw_stock_qty = 0.0f;
			return 1;
		}
	}

	if( edit_product_enable )
	{
		if( mouse.any_click(edit_product_x1, edit_product_y1, edit_product_x2, edit_product_y2, LEFT_BUTTON) )
		{
			stock_qty += 100.0f;
			if( stock_qty > max_stock_qty )
				stock_qty = max_stock_qty;
			return 1;
		}
		if( mouse.any_click(edit_product_x1, edit_product_y1, edit_product_x2, edit_product_y2, RIGHT_BUTTON) )
		{
			stock_qty -= 100.0f;
			if( stock_qty < 0.0f )
				stock_qty = 0.0f;
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmFactory::detect_edit_mode -------//
