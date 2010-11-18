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

//Filename    : OSITE2.CPP
//Description : Object Site Material - display info functions

#include <osite.h>
#include <ovga.h>
#include <omodeid.h>
// #include <oinfo.h>
#include <omouse.h>
#include <ofont.h>
#include <osys.h>
#include <oworld.h>
#include <oworldmt.h>
#include <oimgres.h>
#include <orawres.h>
#include <ogodres.h>
#include <oraceres.h>
#include <oitemres.h>
#include <otechres.h>
#include <obitmap.h>
#include <onation.h>
#include <ogame.h>
#include <ofirmres.h>
#include <ot_firm.h>
#include <ot_sedit.h>

static short edit_id_x1, edit_id_y1, edit_id_x2, edit_id_y2, edit_id_enable;
static short edit_res_x1, edit_res_y1, edit_res_x2, edit_res_y2, edit_res_enable;

//--------- Begin of function Site::disp_info ---------//
//
void Site::disp_info(int refreshFlag)
{
#ifdef USE_FLIP
	vga.use_back();
#endif

#define LINE1_Y (INFO_Y1+20)
#define LINE2_Y (INFO_Y1+40)
#define LINE3_Y (INFO_Y1+60)
#define LINE4_Y (INFO_Y1+80)
//	if( refreshFlag != INFO_REPAINT )
//		return;

   //------- natural resource site ------//

	// #### begin Gilbert 21/10 #######//

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( site_type == SITE_RAW )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
		// String str = raw_res[object_id]->name;
		// str += " Deposit";
		// font_whbl.center_put( INFO_X1+20, LINE1_Y, INFO_X2-10, LINE1_Y+20, str );
		font_whbl.center_put( INFO_X1+20, LINE1_Y, INFO_X2-10, LINE1_Y+20, 
			text_firm.str_alchemy_raw(object_id) );

		//str = "Estimated Reserves ";
		// str += m.format(reserve_qty, 1);
		//font_whbl.center_put( INFO_X1+20, LINE2_Y, INFO_X2-10, LINE2_Y+20, str );
		font_whbl.center_put( INFO_X1+20, LINE2_Y, INFO_X2-10, LINE2_Y+20,
			text_firm.str_estimated_resource(reserve_qty) ); 

		vga.active_buf->put_bitmap_trans( INFO_X1+13, INFO_Y1+129, raw_res.interface_raw_icon(object_id) );
	}

	//--------- scroll of power --------//

	else if( site_type == SITE_SCROLL )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

		GodInfo* godInfo = god_res[object_id];

		UnitInfo* unitInfo = unit_res[godInfo->unit_id];
		Bitmap *bitmapPtr = (Bitmap *) unitInfo->soldier_icon_ptr;
		
		int x1 = (INFO_X1 + INFO_X2)/2 - bitmapPtr->get_width()/2;
		int y1 = INFO_Y2 - 100 - bitmapPtr->get_height();
		int x2 = x1 + bitmapPtr->get_width() -1;
		int y2 = y1 + bitmapPtr->get_height() -1;
		
		int srcX1 = max(x1, INFO_X1+13)-x1;
		int srcY1 = 0;
		int srcX2 = min(x2, INFO_X2)-x1;
		int srcY2 = min(y2, INFO_Y2)-y1;
	
		short *colorRemapTable = game.get_color_remap_table(0, 1);
		vga.active_buf->put_bitmap_area_trans_remap_decompress(x1, y1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);

//		String str = "*";
//		str += race_res[godInfo->race_id]->name;
//		str += " Scroll of Power*";
//		font_whbl.center_put( INFO_X1+20, LINE1_Y-10, INFO_X2-10, LINE1_Y+20-10, str );
		font_whbl.center_put( INFO_X1+20, LINE1_Y-10, INFO_X2-10, LINE1_Y+20-10,
			text_firm.str_scroll_if_1(race_res[godInfo->race_id]->name) );

//		str = "Owners may build the";
//		font_whbl.center_put( INFO_X1+20, LINE2_Y-10, INFO_X2-10, LINE2_Y+20-10, str );
//		str = race_res[godInfo->race_id]->name;
//		str += " Seat of Power";
//		font_whbl.center_put( INFO_X1+20, LINE3_Y-10, INFO_X2-10, LINE3_Y+20-10, str );
//		str = "and Invoke ";
//		str += unit_res[godInfo->unit_id]->name;
//		font_whbl.center_put( INFO_X1+20, LINE4_Y-10, INFO_X2-10, LINE4_Y+20-10, str );

		font_whbl.center_put_paragraph( INFO_X1+20, LINE2_Y-10, INFO_X2-10, LINE4_Y+20-10, 
			text_firm.str_scroll_if_2( race_res[godInfo->race_id]->name,
				firm_res.get_build(firm_res[FIRM_BASE]->get_build_id(godInfo->race_id))->name,
				unit_res[godInfo->unit_id]->name),
			6, 0, 0 );
	}

	//----------- gold coins -----------//

	else if( site_type == SITE_GOLD_COIN )
	{
		font_snds.put( INFO_X1+20, LINE1_Y, text_firm.str_treasure_if_1() ); // "Treasure" );

		font_snds.put( INFO_X1+20, LINE2_Y, text_firm.str_treasure_if_2() ); // "Worth" );
		font_snds.put( INFO_X1+120, LINE2_Y, m.format( object_id, 2) );
	}
	else if( site_type == SITE_ITEM )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
	//	font_snds.put( INFO_X1+20, LINE1_Y, "Item" );
	//	font_snds.put( INFO_X1+20, LINE2_Y, item_res.item_name( object_id, reserve_qty ), 0, -1, 1 );
	//	font_snds.put( INFO_X1+20, LINE3_Y, item_res.item_desc( object_id, reserve_qty ) );

		font_whbl.center_put( INFO_X1+20, LINE1_Y, INFO_X2-10, LINE1_Y+20, item_res.item_name( object_id, reserve_qty ) );
		font_whbl.center_put( INFO_X1+20, LINE2_Y, INFO_X2-10, LINE2_Y+20, item_res.item_desc( object_id, reserve_qty ) );
		image_gameif.put_back( INFO_X1+13, INFO_Y1+129, "ITEMBASE");
		char *iconBitmap = item_res.item_interface(object_id);
		if( iconBitmap )
		{
			vga.active_buf->put_bitmap_trans_decompress( INFO_X1+13, INFO_Y1+129, iconBitmap );
		}

	//	image_gameif.put_front( INFO_X1+13, INFO_Y1+129, item_info_array[object_id-1].bitmap_idx);
	}
	else if( site_type == SITE_WEAPON_BLUEPRINT )
	{
		TechInfo *techInfo = tech_res[object_id];

		font_snds.put( INFO_X1+20, LINE1_Y, text_firm.str_blueprint_if_1() ); // "Scroll of Weapon" );

		font_snds.put( INFO_X1+20, LINE2_Y, text_firm.str_blueprint_if_2() ); // "Technology" );
		font_snds.put( INFO_X1+100, LINE2_Y, techInfo->tech_des(), 0, -1, 1 );
	}
	// #### end Gilbert 21/10 #######//

#ifdef USE_FLIP
	vga.use_front();
#endif
}
//----------- End of function Site::disp_info -----------//


//--------- Begin of function Site::detect_info ---------//
//
void Site::detect_info()
{
	// ------ click the text area, view the site -----//

	if( mouse.single_click( INFO_X1+16, INFO_Y1+10, INFO_X2-6, INFO_Y1+87) )
	{
		world.go_loc( map_x_loc, map_y_loc );
	}
}
//----------- End of function Site::detect_info -----------//


//--------- Begin of function Site::draw ---------//
//
void Site::draw(int curX, int curY, int curZ, int drawFlag)
{

	const SITE_DRAW_OFFSET_X = -LOCATE_WIDTH/2 - (-ZOOM_LOC_X_WIDTH/2 + -ZOOM_LOC_Y_WIDTH/2);
	const SITE_DRAW_OFFSET_Y = -LOCATE_HEIGHT/2 - (-ZOOM_LOC_X_HEIGHT/2 + -ZOOM_LOC_Y_HEIGHT/2);
	int offsetX = SITE_DRAW_OFFSET_X;
	int offsetY = SITE_DRAW_OFFSET_Y;

	char* bmpPtr = NULL;

	switch( site_type )
	{
	case SITE_RAW:
		{
			RawInfo *rawInfo = raw_res[object_id];
			bmpPtr = rawInfo->map_icon_ptr;
			offsetX = rawInfo->map_icon_offset_x;
			offsetY = rawInfo->map_icon_offset_y;
		}
		break;

	case SITE_SCROLL:
		{
			char iconName[]="SCROLLxx";
			iconName[6] = race_res[object_id]->code[0];
			iconName[7] = race_res[object_id]->code[1];
			bmpPtr = image_spict.get_ptr(iconName);

			err_when( !bmpPtr );                                 //BUGHERE, the icons are not complete yet
			//	bmpPtr = image_spict.get_ptr("SCROLL-C");
			
			offsetX = -23;
			offsetY = -8;

			break;
		}

	case SITE_GOLD_COIN:
		{
			// ##### begin Gilbert 30/8 ######//
			#define MAX_COINS_TYPE  8
			// ##### end Gilbert 30/8 ######//

			char iconName[]="COINS-0";
			// ###### begin Gilbert 15/7 ######//
			iconName[6] = '1' + site_recno % MAX_COINS_TYPE;
			// ###### end Gilbert 15/7 ######//
			bmpPtr = image_spict.get_ptr(iconName);
			break;
		}

	case SITE_ITEM:
		{
			bmpPtr = item_res.item_bitmap( object_id );
			if( bmpPtr )
			{
				// put at the bottom center of 64x32 location
				offsetX = -((Bitmap *)bmpPtr)->get_width() / 2;
				offsetY = ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT - ((Bitmap *)bmpPtr)->get_height();
			}
			else
			{
				image_spict.get_ptr("COINS-3");
			}
			break;
		}

	case SITE_WEAPON_BLUEPRINT:
		{
			char iconName[]="WEAP-SCR";
			bmpPtr = image_spict.get_ptr(iconName);
			break;
		}
	}

	// vga_back.put_bitmap_trans( x, y, bmpPtr );

	err_when( !bmpPtr );

	world.zoom_matrix->put_bitmap_offset(curX, curY, curZ, bmpPtr,
		offsetX, offsetY, 
		NULL, 0, 1 | (drawFlag << 2));
}
//----------- End of function Site::draw -----------//


//------- Begin of function Site::draw_selected -----------//
//
// Draw a square around the raw material site on the map.
//
void Site::draw_selected()
{
	Location* locPtr = world.get_loc(map_x_loc, map_y_loc);

	if( locPtr->is_firm() || locPtr->is_town() )	// do not draw the selection frame if there is a firm or town built on the top of the site
		return;

	//------ calculate frame coordinations ---------//
/*
	int x1 = map_x_loc * ZOOM_LOC_WIDTH;
	int y1 = map_y_loc * ZOOM_LOC_HEIGHT;
	int x2 = x1 + ZOOM_LOC_WIDTH  - 1;
	int y2 = y1 + ZOOM_LOC_HEIGHT - 1;

	x1 = x1 - World::view_top_x + ZOOM_X1;
	y1 = y1 - World::view_top_y + ZOOM_Y1;
	x2 = x2 - World::view_top_x + ZOOM_X1;
	y2 = y2 - World::view_top_y + ZOOM_Y1;

	//------------ draw the square frame now ------------//

	if( x1>=ZOOM_X1 && y1>=ZOOM_Y1 && x2<=ZOOM_X2 && y2<=ZOOM_Y2 )
	{
		vga_back.rect( x1, y1, x2, y2, 1, OWN_SELECT_FRAME_COLOR );
	}
	*/
}
//--------- End of function Site::draw_selected -----------//


//--------- Begin of function SiteArray::draw_dot ---------//
//
// Draw 2x2 tiny squares on map window representing the
// location of raw materials sites.
//
void SiteArray::draw_dot()
{
	int	  i, x, y;

	int		vgaBufPitch = vga_back.buf_pitch();
	short		unexploredColor = vga_back.translate_color(UNEXPLORED_COLOR);
	short		siteColor = vga_back.translate_color(SITE_COLOR);

	for(i=1; i <=size() ; i++)
	{
		if( is_deleted(i) )
			continue;

		short *rowWritePtr;
		int siteWidth, siteHeight;

		{
			register Site *rawPtr = operator[](i);

			if( rawPtr->has_mine )		// don't draw it if there is a mine on it.
				continue;

			rowWritePtr = vga_back.buf_ptr(
				world.map_matrix->calc_loc_pos_x(rawPtr->map_x_loc, rawPtr->map_y_loc) + MAP_X1,
				world.map_matrix->calc_loc_pos_y(rawPtr->map_x_loc, rawPtr->map_y_loc) + MAP_Y1 );
			siteWidth = rawPtr->map_x2 - rawPtr->map_x_loc + 1;
			siteHeight = rawPtr->map_y2 - rawPtr->map_y_loc + 1;
		}

		if( siteWidth == 1 && siteHeight == 1 )
		{
			register short *writePtr = rowWritePtr;

			if( writePtr[-vgaBufPitch] != unexploredColor )
				writePtr[-vgaBufPitch] = siteColor;

			if( writePtr[-1] != unexploredColor )
				writePtr[-1] = siteColor;

			if( writePtr[0] != unexploredColor )
				writePtr[0] = siteColor;

			if( writePtr[1] != unexploredColor )
				writePtr[1] = siteColor;

			if( writePtr[vgaBufPitch] != unexploredColor )
				writePtr[vgaBufPitch] = siteColor;
		}
		else
		{ // minimize register variable at one time
			register short *writePtr;

			for( y = 0; y < siteHeight; (rowWritePtr+=y&1?vgaBufPitch:-1), ++y )
			{
				writePtr = rowWritePtr;
				for( x = 0; x < siteWidth; (writePtr+=vgaBufPitch+1),x+=2 )
				{
					if( *writePtr != unexploredColor )
						*writePtr = siteColor;
				}
				if( *writePtr != unexploredColor)
					*writePtr = siteColor;
			}
			writePtr = rowWritePtr;
			for( x = 0; x < siteWidth; (writePtr+=vgaBufPitch+1),x+=2 )
			{
				if( *writePtr != unexploredColor )
					*writePtr = siteColor;
			}
		}
	}
}
//----------- End of function SiteArray::draw_dot -----------//


// --------- Begin of function Site::disp_edit_mode --------//
//
void Site::disp_edit_mode(int refreshFlag)
{
	// short object_id;		// id. of the object,
	// int   reserve_qty;	// for raw material only

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	int y = INFO_Y1+15;

	edit_id_enable = 0;
	edit_res_enable = 0;

	switch( site_type )
	{
	case SITE_RAW:
		// display raw resource name
		font_whbl.put( x1, y, text_firm.str_alchemy_raw(object_id) );
		y += 20;

		// display quantity
		edit_res_enable = !has_mine;
		x = font_whbl.put( x1, y, text_firm.str_estimated_resource(0), 0, x2 );
		x = edit_res_x2 = (edit_res_enable?font_blue:font_whbl).put( (edit_res_x1=x+20), (edit_res_y1=y), reserve_qty, 0, x2 );
		edit_res_y2 = edit_res_y1+font_blue.max_font_height;
		y += 20;
		break;

	case SITE_SCROLL:
		{
			GodInfo* godInfo = god_res[object_id];
			font_whbl.put( x1, y, text_firm.str_scroll_if_1(race_res[godInfo->race_id]->name) );
			y += 20;
		}
		break;

	case SITE_GOLD_COIN:
		{
			font_whbl.put( x1, y, text_firm.str_treasure_if_1(), 0, x2 ); // "Treasure" );
			y += 20;

			edit_id_enable = 1;
			x = font_whbl.put( x1, y, text_firm.str_treasure_if_2(), 0, x2 );	// "Worth" );
			x = edit_id_x2 = font_blue.put( (edit_id_x1=x+20), (edit_id_y1=y), m.format(object_id, 2), 0, x2 );
			edit_id_y2 = edit_id_y1+font_blue.max_font_height;
			y += 20;

			edit_res_enable = 1;
			x = font_whbl.put( x1, y, text_editor.str_player_live_points(), 0, x2 );	// "Life Points" );
			x = edit_res_x2 = font_blue.put( (edit_res_x1=x+20), (edit_res_y1=y), reserve_qty, 0, x2 );
			edit_res_y2 = edit_res_y1+font_blue.max_font_height;
			y += 20;
		}
		break;

	case SITE_ITEM:
		font_whbl.put( x1, y, item_res.item_name(object_id, reserve_qty), 0, x2 );
		y += 20;

		font_whbl.put( x1, y, item_res.item_desc(object_id, reserve_qty), 0, x2 );
		y += 20;

		if( sys.debug_session )
		{
			edit_res_enable = 1;
			x = edit_res_x2 = font_blue.put( (edit_res_x1=x1), (edit_res_y1=y), reserve_qty, 0, x2 );
			edit_res_y2 = edit_res_y1+font_blue.max_font_height;
		}
		y += 20;
		break;

	case SITE_WEAPON_BLUEPRINT:
		font_whbl.put( x1, y, text_firm.str_blueprint_if_1(), 0, x2 );
		y += 20;
		x = font_whbl.put( x1, y, tech_res[object_id]->tech_des(), 0, x2 );
		y += 20;
		break;

	default:
		err_here();	// edit mode not supported
	}
}
// --------- End of function Site::disp_edit_mode --------//


// --------- Begin of function Site::detect_edit_mode --------//
//
int Site::detect_edit_mode()
{
	int idLow = 1;
	int idHigh = 1000;
	int idInc = 1;

	int resLow = 0;
	int resHigh = 10000;
	int resInc = 100;

	switch( site_type )
	{
	case SITE_RAW:
		err_when( edit_id_enable );
		resLow = MAX_RAW_RESERVE_QTY / 20;
		resHigh = MAX_RAW_RESERVE_QTY;
		resInc = 1000;
		break;

	case SITE_SCROLL:
		err_when( edit_id_enable );
		err_when( edit_res_enable );
		break;

	case SITE_GOLD_COIN:
		idLow = 0;
		idHigh = 10000;
		idInc = 100;
		resLow = 0;
		resHigh = 10000;
		resInc = 100;
		break;

	case SITE_ITEM:
		err_when( edit_id_enable );
		resLow = 0;
		resHigh = 10000;
		resInc = 1;
		break;

	case SITE_WEAPON_BLUEPRINT:
		err_when( edit_id_enable );
		err_when( edit_res_enable );
		break;

	default:
		err_here();
	}

	if( edit_id_enable )
	{
		if( mouse.any_click(edit_id_x1, edit_id_y1, edit_id_x2, edit_id_y2, LEFT_BUTTON) )
		{
			if( object_id > idHigh - idInc )
				object_id = idHigh;
			else
				object_id += idInc;
			return 1;
		}
		if( mouse.any_click(edit_id_x1, edit_id_y1, edit_id_x2, edit_id_y2, RIGHT_BUTTON) )
		{
			if( object_id >= idLow )
			{
				if( object_id < idLow + idInc )
					object_id = idLow;
				else
					object_id -= idInc;
			}
			return 1;
		}
	}

	if( edit_res_enable )
	{
		if( mouse.any_click(edit_res_x1, edit_res_y1, edit_res_x2, edit_res_y2, LEFT_BUTTON) )
		{
			if( reserve_qty > resHigh - resInc )
				reserve_qty = resHigh;
			else
				reserve_qty += resInc;
			return 1;
		}
		if( mouse.any_click(edit_res_x1, edit_res_y1, edit_res_x2, edit_res_y2, RIGHT_BUTTON) )
		{
			if( reserve_qty >= resLow )
			{
				if( reserve_qty < resLow + resInc )
					reserve_qty = resLow;
				else
					reserve_qty -= resInc;
			}
			return 1;
		}
	}

	return 0;
}
// --------- End of function Site::detect_edit_mode --------//
