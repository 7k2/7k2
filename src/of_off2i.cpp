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

// Filename     : OF_OFF2.CPP
// Description : eating flower interface function


#include <of_off2.h>
#include <ovga.h> 
#include <omodeid.h>
#include <oimgres.h>
#include <ogame.h>
#include <obitmapw.h>
#include <obitmap.h>
#include <ofont.h>
#include <oconfig.h>

//----------- Begin of function FirmOffensive2::put_info -----------//

void FirmOffensive2::put_info(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	int hitPoints;
	String str;
	int offsetX = 0;
	int offsetY = 0;

	if( hit_points > (float)0 && hit_points < (float)1 )
		hitPoints = 1;		// display 1 for value between 0 and 1
	else
		hitPoints = (int) hit_points;

	if ( max_hit_points() ) 
	{
		offsetX = -35;
		offsetY = -14;
		short*	hitPointBitmap =NULL;
		int ratio = hitPoints *40 / (int)max_hit_points();
		int size = hitPoints *76 / (int)max_hit_points();
		
		//106 x 35 --- 15 to 90 ie. 0 to 40
		hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	
		if (ratio <11)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_B2"));
		else
		if (ratio <40)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_B3"));
		else
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_B4"));
			
		vga.active_buf->read_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, INFO_X1 +94 +20 +size +offsetX, INFO_Y1 +80 +offsetY, hitPointBitmap );
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_B1"));
		vga.active_buf->put_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, hitPointBitmap );
		mem_del( hitPointBitmap );				
			
		font_whbl.center_put( INFO_X1 +43, INFO_Y1 +45, INFO_X1 +65, INFO_Y1 +57, m.format((int)hitPoints,4));
		font_whbl.center_put( INFO_X1 +169, INFO_Y1 +45, INFO_X1 +191, INFO_Y1 +57, m.format((int)max_hit_points(),4) );
	}

	// font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, "Offensive Building 3", 0, 1 );
	font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, firm_name(), 0, 1 );

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	short *colorRemapTable = firmBuild->get_color_remap_table(nation_recno, firm_array.selected_recno == firm_recno);
	colorRemapTable = firm_res.calc_color_remap_table( colorRemapTable, 1.0f );

	FirmBitmap* firmBitmap = firm_res.get_bitmap(firmBuild->first_bitmap(1));
	if( firmBitmap )
	{
		Bitmap* bitmapPtr = (Bitmap *) firmBuild->res_bitmap.read_imported(firmBitmap->bitmap_ptr);
	
		int x1;
		int y1;
		int srcX2;
		int srcY2;
		
		if (config.building_size == 1)	
		{
			x1 = INFO_X1 +130;
			y1 = INFO_Y1 +140;
		}
		else
		{
			x1 = INFO_X1 +120;
			y1 = INFO_Y1 +142;
		}
		
		x1 += firmBitmap->offset_x;
		y1 += firmBitmap->offset_y;
		int x2 = x1 + bitmapPtr->get_width() -1;
		int y2 = y1 + bitmapPtr->get_height() -1;
		int srcX1 = MAX(x1, INFO_X1+15)-x1;
		int srcY1 = 0;

		if (config.building_size == 1)	
		{
			srcX2 = MIN(x2, INFO_X2)-x1;
			srcY2 = MIN(y2, INFO_Y1+227)-y1;
		}
		else
		{
			srcX2 = MIN(x2, INFO_X2)-x1;
			srcY2 = MIN(y2, INFO_Y2)-y1;
		}

		vga.active_buf->put_bitmap_area_trans_remap_decompress(x1, y1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);
	}
/*
	if (config.building_size == 1)	
		return;

	firmBitmap = firm_res.get_bitmap(firmBuild->first_bitmap(firm_cur_frame[0]));
	if( firmBitmap && firmBitmap->display_layer == 1 )
	{
		Bitmap* bitmapPtr = (Bitmap *) firmBuild->res_bitmap.read_imported(firmBitmap->bitmap_ptr);
		
		int x1 = INFO_X1 +120 +firmBitmap->offset_x;
		int y1 = INFO_Y1 +142 +firmBitmap->offset_y;
		int x2 = x1 + bitmapPtr->get_width() -1;
		int y2 = y1 + bitmapPtr->get_height() -1;
		int srcX1 = MAX(x1, INFO_X1+15)-x1;
		int srcY1 = 0;
		int srcX2 = MIN(x2, INFO_X2)-x1;
		int srcY2 = MIN(y2, INFO_Y2)-y1;

		vga.active_buf->put_bitmap_area_trans_remap_decompress(x1, y1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);

	//	vga_back.put_bitmap_trans_remap_decompress(INFO_X1 +113 +firmBitmap->offset_x,
	//		INFO_Y1 +158 +firmBitmap->offset_y, bitmapPtr, colorRemapTable);
	}*/
}
//----------- End of function FirmOffensive2::put_info -----------//


