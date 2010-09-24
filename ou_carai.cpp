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

//Filename    : OU_CARAI.CPP
//Description : Unit Caravan - interface functions

#include <OU_CARA.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OSTR.H>
#include <OBITMAP.H>
#include <OHELP.H>
#include <OFONT.H>
#include <OINFO.H>
#include <OCONFIG.H>
#include <OIMGRES.H>
#include <OMOUSE.H>
#include <OGAME.H>
#include <OPOWER.H>
#include <OBUTTON.H>
#include <OBUTT3D.H>
#include <ONATION.H>
#include <OREMOTE.H>
#include <OF_MINE.H>
#include <OF_FACT.H>
#include <OBUTTCUS.H>
#include <OSE.H>
#include <OT_UNIT.H>

//------------- Define static vars ------------//

static Button3D		button_duplicate_caravan;
static ButtonCustom	button_set_stop[MAX_STOP_FOR_CARAVAN];
static ButtonCustom	button_go_stop[MAX_STOP_FOR_CARAVAN];
static ButtonCustom	button_cancel_stop[MAX_STOP_FOR_CARAVAN];
static ButtonCustom	button_reset_stop[MAX_STOP_FOR_CARAVAN];
static ButtonCustom	button_select_array[MAX_STOP_FOR_CARAVAN][MAX_GOODS_SELECT_BUTTON];

static ButtonCustomGroup button_auto_trade_group(2);	// 0=manual, 1=auto

static void				i_button_go_stop(ButtonCustom *button, int repaintBody);
static void				i_button_set_stop(ButtonCustom *button, int repaintBody);
static void				i_button_cancel_stop(ButtonCustom *button, int repaintBody);
static void				i_button_reset_stop(ButtonCustom *button, int repaintBody);
static void				i_disp_caravan_select_button(ButtonCustom *button, int repaintBody);
static void				i_disp_auto_trade_button(ButtonCustom *button, int repaintBody);

static int 				carv_gdd_x[MAX_PRODUCT+MAX_RAW+1] = {0, 26, 47, 77, 101, 127, 151};

//--------- Begin of function UnitCaravan::disp_info ---------//
//
void UnitCaravan::disp_info(int refreshFlag)
{
	//----- for multiplayer game, skip displaying information for the first frame --------//
//	if(remote.is_enable())
//	{
//		if(unit_array.mp_first_frame_to_select_caravan && // first frame
//			unit_array.mp_pre_selected_caravan_recno==sprite_recno) // is selected
//			return;
//	}

#ifdef USE_FLIP
	vga.use_back();
#endif

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("CARABASE") );

//	disp_basic_info(INFO_Y1, refreshFlag);
	disp_unit_profile(10, refreshFlag);

	if( !config.show_ai_info && !is_own() )
		return;

	disp_stop(INFO_Y1+54, refreshFlag);

	// display auto/manual button

	if( refreshFlag == INFO_REPAINT )
	{
		button_duplicate_caravan.create( INFO_X1+13, INFO_Y1+281, 'A', "CARACOPY" );
		
		// manual button
		button_auto_trade_group[0].create( INFO_X1+BUTTON_DISTANCE+13, INFO_Y1+235,
			INFO_X1+BUTTON_DISTANCE+56-1, INFO_Y1+280,
			i_disp_auto_trade_button, ButtonCustomPara(this, 0), 0 );

		// auto button
		button_auto_trade_group[1].create( INFO_X1+13, INFO_Y1+235,
			INFO_X1+56-1, INFO_Y1+280,
			i_disp_auto_trade_button, ButtonCustomPara(this, 1), 0 );
		button_auto_trade_group[1].set_help_code( "AUTOPICK" );
	}
	button_auto_trade_group.paint( default_market_trade_mode );
	
	button_duplicate_caravan.enable_flag = can_duplicate_caravan();
	button_duplicate_caravan.paint();

	disp_goods(INFO_Y1+235, refreshFlag);

#ifdef USE_FLIP
	vga.use_front();
#endif
}
//---------- End of function UnitCaravan::disp_info ----------//


//--------- Begin of function UnitCaravan::detect_info ---------//
//
void UnitCaravan::detect_info()
{
	if( detect_unit_profile() )
		return;

//	if(!is_visible())
//		return;

	if( detect_basic_info() )
		return;

	if( !is_own() && !config.show_ai_info )
		return;
	
	detect_stop();

	if( is_own() )
	{
		int i;
		int rc =  button_auto_trade_group.detect();
		if( rc >= 0 )
		{
			if( remote.is_enable() )
			{
				// ##### begin Gilbert 7/10 ######//
				// packet structure : <unit recno> <0=manual, 1=auto>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_U_CARA_SET_AUTO, 2*sizeof(short));
				*shortPtr = sprite_recno;
				shortPtr[1] = rc;
				return;
				// ##### end Gilbert 7/10 ######//
			}
			else
			{
				default_market_trade_mode = (char) rc;
				for( i = 0; i < MAX_STOP_FOR_CARAVAN; ++i )
					if( stop_array[i].firm_recno && stop_array[i].firm_id == FIRM_MARKET )
						set_stop_pick_up(i+1, rc ? AUTO_PICK_UP : MANUAL_PICK_UP, COMMAND_AUTO);
			}
		}

		if( button_duplicate_caravan.detect() )
		{
			se_ctrl.immediate_sound("CV-M3-5");
			duplicate_caravan(COMMAND_PLAYER);			
		}
	}
}
//---------- End of function UnitCaravan::detect_info ----------//


//--------- Begin of function UnitCaravan::disp_goods ---------//
//
void UnitCaravan::disp_goods(int dispY1, int refreshFlag)
{
	for(int productId = 1; productId <= MAX_PRODUCT; ++productId )
	{
		int x = INFO_X1+13+2*BUTTON_DISTANCE+3;
		int y = INFO_Y1+235+4+ (productId-1)*20;

//		vga.active_buf->put_bitmap_trans( x+2, y, raw_res.small_product_icon(productId) );
		font_whbl.right_put( x+BUTTON_ACTION_WIDTH-3, y, m.format(product_raw_qty_array[productId-1]) );
	}

	for(int rawId = 1; rawId <= MAX_RAW; ++rawId )
	{
		int x = INFO_X1+13+3*BUTTON_DISTANCE+3;
		int y = INFO_Y1+235+4+ (rawId-1)*20;

//		vga.active_buf->put_bitmap_trans( x+2, y, raw_res.small_raw_icon(rawId) );
		font_whbl.right_put( x+BUTTON_ACTION_WIDTH-3, y, m.format(raw_qty_array[rawId-1]) );
	}
}
//---------- End of function UnitCaravan::disp_goods ----------//


//--------- Begin of function UnitCaravan::disp_stop ---------//
//
void UnitCaravan::disp_stop(int dispY1, int refreshFlag)
{
	int y = INFO_Y1 + 94;

	int i, j;

	for( i = 0; i < MAX_STOP_FOR_CARAVAN; ++i, y += 70 )
	{
		TradeStop *tradeStop = stop_array+i;

		if( refreshFlag == INFO_REPAINT )
		{
			button_go_stop[i].create( INFO_X1+17, y+2, INFO_X1+167, y+35, i_button_go_stop,
				ButtonCustomPara( this, tradeStop->firm_recno ) );
			button_go_stop[i].set_help_code( "CGOSTOP" );

			button_set_stop[i].create( INFO_X1+169, y+2, INFO_X1+215, y+20, i_button_set_stop,
				ButtonCustomPara( NULL, 0 ) );
			button_set_stop[i].set_help_code( "CSETSTOP" );

			button_cancel_stop[i].create( INFO_X1+169, y+21, INFO_X1+215, y+39, i_button_cancel_stop,
				ButtonCustomPara( NULL, 0 ) );
			button_cancel_stop[i].set_help_code( "CDELSTOP" );

			button_reset_stop[i].create( INFO_X1+169, y+42, INFO_X1+215, y+60, i_button_reset_stop,
				ButtonCustomPara( NULL, 0 ) );
			button_reset_stop[i].set_help_code( "CCLRSEL" );

			for( j = 0; j < MAX_GOODS_SELECT_BUTTON; ++j )
				button_select_array[i][j].create( INFO_X1+17+carv_gdd_x[j], y+34, INFO_X1+17+carv_gdd_x[j+1]-1, y+61,
				i_disp_caravan_select_button, ButtonCustomPara( NULL, j ), 0 );
		}

		button_go_stop[i].custom_para = ButtonCustomPara( this, tradeStop->firm_recno );
		button_go_stop[i].paint();
		button_set_stop[i].paint();
		button_cancel_stop[i].paint();
		button_reset_stop[i].paint();

		Firm *firmPtr = NULL;
		if( !firm_array.is_deleted(tradeStop->firm_recno))
			firmPtr = firm_array[tradeStop->firm_recno];

		for( j = 0; j < MAX_GOODS_SELECT_BUTTON; ++j )
		{
			// display quantity on the firm

			int rawId = j+FIRST_GOODS_SELECT_BUTTON-PICK_UP_RAW_FIRST+1;
			if( rawId < 1 || rawId > MAX_RAW )
				rawId = 0;
			int productId = j+FIRST_GOODS_SELECT_BUTTON-PICK_UP_PRODUCT_FIRST+1;
			if( productId < 1 || productId > MAX_PRODUCT )
				productId = 0;

			int stock = -1;

			if( firmPtr )
			{
				if( firmPtr->cast_to_FirmMarket() )
				{
					MarketGoods *marketGoods;
					if( rawId )
					{
						marketGoods = firmPtr->cast_to_FirmMarket()->market_raw_array(rawId);
						err_when( marketGoods && marketGoods->raw_id != rawId );
					}
					else if( productId )
					{
						marketGoods = firmPtr->cast_to_FirmMarket()->market_product_array(productId);
						err_when( marketGoods && marketGoods->product_raw_id != productId );
					}
					else
					{
						err_here();
						marketGoods = NULL;
					}

					if( marketGoods )
						stock = (int) marketGoods->stock_qty;
				}
				else if( firmPtr->cast_to_FirmMine() )
				{
					if( rawId && firmPtr->cast_to_FirmMine()->raw_id == rawId )
					{
						stock = (int) firmPtr->cast_to_FirmMine()->stock_qty;
					}
				}
				else if( firmPtr->cast_to_FirmFactory() )
				{
					if( productId && firmPtr->cast_to_FirmFactory()->product_raw_id == productId )
					{
						stock = (int) firmPtr->cast_to_FirmFactory()->stock_qty;
					}
					//else if( rawId && firmPtr->cast_to_FirmFactory()->product_raw_id == rawId )
					//{
					// stock = (int) firmPtr->cast_to_FirmFactory()->raw_stock_qty;
					//}
				}
			}

			if( stock >= 0 )
			{
				// display button

				button_select_array[i][j].enable_flag = 1;
				button_select_array[i][j].paint( tradeStop->pick_up_array[j] );
			}
			else
			{
				button_select_array[i][j].enable_flag = 0;
				button_select_array[i][j].pushed_flag = tradeStop->pick_up_array[j];
			}
		}
	}
}
//---------- End of function UnitCaravan::disp_stop ----------//


//--------- Begin of function UnitCaravan::detect_stop ---------//
//
void UnitCaravan::detect_stop()
{
	int i, x=INFO_X1;

	for( i=0 ; i<MAX_STOP_FOR_CARAVAN ; i++ )
	{
		if( is_own() && button_set_stop[i].detect() )
			power.issue_command( COMMAND_SET_CARAVAN_STOP, sprite_recno, i+1 );		// i+1 - stop id., passed as a parameter of the command

		if( !stop_array[i].firm_recno )
			continue;

		if( button_go_stop[i].detect() )
		{
			Firm* firmPtr = firm_array[ stop_array[i].firm_recno ];
			world.go_loc( firmPtr->center_x, firmPtr->center_y );
		}

		if( !is_own() )
			continue;

		if(button_cancel_stop[i].detect())
		{
			if(is_visible())
			{
				del_stop(i+1, COMMAND_PLAYER);
				se_ctrl.immediate_sound("TURN_OFF");
			}
		}

		if(button_reset_stop[i].detect() )
		{
			if( is_visible() )
			{
				set_stop_pick_up(i+1, NO_PICK_UP, COMMAND_PLAYER);
				se_ctrl.immediate_sound("TURN_OFF");
			}
		}

		for(int b=0; b<MAX_GOODS_SELECT_BUTTON; ++b)
		{
			if(button_select_array[i][b].detect())
			{
				se_ctrl.immediate_sound(
					button_select_array[i][b].elastic_flag || button_select_array[i][b].pushed_flag ?
					"TURN_ON" : "TURN_OFF");

				set_stop_pick_up(i+1, b+FIRST_GOODS_SELECT_BUTTON, COMMAND_PLAYER); // b = 1 - MAX_PICK_UP_GOODS
			}
		}
	}
}
//---------- End of function UnitCaravan::detect_stop ----------//


//--------- Begin of static function i_button_go_stop ---------//
//
static void i_button_go_stop(ButtonCustom *button, int repaintBody)
{
	Firm *firmPtr = NULL;
	if( !firm_array.is_deleted(button->custom_para.value) )
	{
		firmPtr = firm_array[button->custom_para.value];
	}

	vga.active_buf->put_bitmap_remap( button->x1, button->y1, 

		// button resource name
		image_button.read( button->pushed_flag ? "CARA-STD" :
			mouse.in_area(button->x1, button->y1, button->x2, button->y2) ? "CARA-STH" : "CARA-STU" ),

		// remap table
		game.get_color_remap_table( (!firmPtr ? 0 : firmPtr->nation_recno), 0) );

	// put destination marker
	{
		UnitCaravan *unitPtr = (UnitCaravan *)button->custom_para.ptr;
		if( unitPtr->dest_stop_id > 0 && unitPtr->dest_stop_id <= MAX_STOP_FOR_CARAVAN 
			&& unitPtr->stop_array[unitPtr->dest_stop_id-1].firm_recno == button->custom_para.value )
		{
			vga.active_buf->put_bitmap_trans_decompress( button->x1+18, button->y1+6,
				image_button.read("TRI-R") );
		}
	}

	// put firm name

	if( firmPtr )
	{
		font_snds.put_paragraph( button->x1+34, button->y1+5+(button->pushed_flag?1:0), 
			button->x2-5, button->y2-5+(button->pushed_flag?1:0),
			firmPtr->firm_name() );
	}
}
//--------- End of static function i_button_go_stop ---------//


//--------- Begin of static function i_button_set_stop ---------//
//
static void i_button_set_stop(ButtonCustom *button, int repaintBody)
{
	vga.active_buf->put_bitmap( button->x1, button->y1, 

		// button resource name
		image_button.read( button->pushed_flag ? "CARA-SED" :
			mouse.in_area(button->x1, button->y1, button->x2, button->y2) ? "CARA-SEH" : "CARA-SEU" ) );
}
//--------- End of static function i_button_set_stop ---------//


//--------- Begin of static function i_button_cancel_stop ---------//
//
static void i_button_cancel_stop(ButtonCustom *button, int repaintBody)
{
	vga.active_buf->put_bitmap( button->x1, button->y1, 

		// button resource name
		image_button.read( button->pushed_flag ? "CARA-DED" :
			mouse.in_area(button->x1, button->y1, button->x2, button->y2) ? "CARA-DEH" : "CARA-DEU" ) );
}
//--------- End of static function i_button_cancel_stop ---------//


//--------- Begin of static function i_button_reset_stop ---------//
//
static void i_button_reset_stop(ButtonCustom *button, int repaintBody)
{
	vga.active_buf->put_bitmap( button->x1, button->y1, 

		// button resource name
		image_button.read( button->pushed_flag ? "CARA-RSD" :
			mouse.in_area(button->x1, button->y1, button->x2, button->y2) ? "CARA-RSH" : "CARA-RSU" ) );
}
//--------- End of static function i_button_reset_stop ---------//


//--------- Begin of static function i_disp_caravan_select_button ---------//
//
static void i_disp_caravan_select_button(ButtonCustom *button, int repaintBody)
{
	int j = button->custom_para.value;
//	char *bitmapPtr = image_button.read( button->enable_flag ? (button->pushed_flag ? "CARA-GDD" : "CARA-GDU") : "CARA-GDL" );
	char *bitmapPtr = image_button.read( button->enable_flag ? (button->pushed_flag ? "CARA-GDD" : "CARA-GDL") : "CARA-GDL" );
	vga.active_buf->put_bitmap_area_trans( button->x1-carv_gdd_x[j], button->y1, bitmapPtr, 
		carv_gdd_x[j], 0, carv_gdd_x[j+1]-1, ((Bitmap *)bitmapPtr)->get_height()-1 );
}
//--------- End of static function i_disp_caravan_select_button ---------//


//--------- Begin of static function i_disp_auto_trade_button ---------//
//
static void i_disp_auto_trade_button(ButtonCustom *button, int repaintBody)
{
	// line 1
	int lineHeight = font_whbl.max_font_height;
	int y = button->y1;
//	font_whbl.center_put( button->x1, y, button->x2, y+lineHeight-1,
//		button->custom_para.value ? "Auto" : "Manual" );
//	y += font_whbl.max_font_height;
//	font_whbl.center_put( button->x1, y, button->x2, y+lineHeight-1,
//		"Trade" );

	font_whbl.center_put_paragraph( button->x1, button->y1, button->x2, button->y2,
		button->custom_para.value ? text_unit.str_caravan_auto() : text_unit.str_caravan_manual(),
		DEFAULT_LINE_SPACE, 0, 0 );

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
//--------- End of static function i_disp_auto_trade_button ---------//
