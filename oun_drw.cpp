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

//Filename    : OPERSDRW.CPP
//Description : Object Unit Drawing routines

#include <OSYS.H>
#include <OBITMAP.H>
#include <OBITMAPW.H>
#include <OVGA.H>
#include <ONATION.H>
#include <OCONFIG.H>
#include <OIMGRES.H>
#include <OBITMAPW.H>
#include <OSPRITE.H>
#include <OSPRTRES.H>
#include <OSFRMRES.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OUNIT.H>
#include <OGAME.H>
#include <OITEMRES.H>
#include <math.h>

//--------- Begin of function Unit::draw ---------//
//
void Unit::draw(int, int drawFlag)
{
	//--------- draw sprite on the zoom window ---------//

	int needMirror;

	SpriteFrame* spriteFrame = cur_sprite_frame(&needMirror);

	err_when( !sprite_info->res_bitmap.init_flag );

	char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);

	//------- get the color remap table for this sprite ------//

	short* colorRemapTable = sprite_info->get_color_remap_table(nation_recno, 0); 		// selected_flag);

	// ######## begin Gilbert  12/1 #####//
	// ------ test camouflag on -------- //

	if( is_camouflage(-1) )		// test camouflag on
	{
		drawFlag |= 1;				// make it half transparent
	}

	// ######## end Gilbert  12/1 #####//

	//------- draw it on zoom matrix ----------//

	world.zoom_matrix->put_bitmap_offset(
		cur_x, cur_y, get_z(),
		bitmapPtr, spriteFrame->offset_x, spriteFrame->offset_y,
		colorRemapTable, needMirror, (sprite_info->remap_bitmap_flag ? 2 : 1) | (drawFlag << 2) );
		
	if (is_freezed())
	{
		if(((Bitmap *)bitmapPtr)->get_height() < 66 &&
			((Bitmap *)bitmapPtr)->get_width() < 46)
		{
			bitmapPtr = image_spict.get_ptr("ICE");
			world.zoom_matrix->put_bitmap_offset(cur_x, cur_y, get_z(),
			//	bitmapPtr, -((Bitmap *)bitmapPtr)->get_width()/2, spriteFrame->offset_y,
				bitmapPtr, -((Bitmap *)bitmapPtr)->get_width()/2, -((Bitmap *)bitmapPtr)->get_height()+25,
				NULL, 0, 5);
		}
		else 
		if(((Bitmap *)bitmapPtr)->get_height() < 93)
		{
			bitmapPtr = image_spict.get_ptr("ICE2");
			world.zoom_matrix->put_bitmap_offset(cur_x, cur_y, get_z(),
				bitmapPtr, -((Bitmap *)bitmapPtr)->get_width()/2, spriteFrame->offset_y-10,
			//	bitmapPtr, -((Bitmap *)bitmapPtr)->get_width()/2, -((Bitmap *)bitmapPtr)->get_height()+50,
				NULL, 0, 5);
		}
		else
		{
			bitmapPtr = image_spict.get_ptr("ICE3");
			world.zoom_matrix->put_bitmap_offset(cur_x, cur_y, get_z(),
				bitmapPtr, 20-((Bitmap *)bitmapPtr)->get_width()/2, spriteFrame->offset_y-20,
			//	bitmapPtr, -((Bitmap *)bitmapPtr)->get_width()/2, -((Bitmap *)bitmapPtr)->get_height()+50,
				NULL, 0, 5);
		}
	}
	//---- display additional info for selected units ----//
	if( selected_flag )
		draw_selected();
	
	else
	if(( is_own() || config.show_ai_info || skill_level() > 0 ) && config.show_all_unit_icon )
		draw_rank_icon();
	

	SpriteInfo* spriteInfo = sprite_res[unit_res[unit_id]->sprite_id];
	if( max_power < 0 )
	{
		if ((config.frame_speed != 0) && (sys.frame_count%2 == abs(max_power%2)))
			max_power --;
		world.zoom_matrix->put_center_text(cur_x, cur_y, 
			get_z() - 12 * ((max_power) + 10), m.format( cur_power, 2 ), 0, &font_sop);
	}
	UnitInfo* unitInfo = unit_res[unit_id];
	
	switch( unit_id )
	{
	case UNIT_BEE:
	case UNIT_TERMITE:
	case UNIT_JAGUAR:
	case UNIT_GORILLA:
	case UNIT_RHINO:
		{
			char *bitmapPtr = image_icon.get_ptr("M_COLCOD");
			world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), bitmapPtr,
				-((Bitmap *)bitmapPtr)->get_width()>>1, 
				spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 -((Bitmap *)bitmapPtr)->get_height(),
				 game.get_color_remap_table(nation_recno, 0), 0, 0);
		}
	}	
}
//----------- End of function Unit::draw -----------//


//--------- Begin of function Unit::draw_selected ---------//
//
// Draw a square around the sprite if it has been selected.
//
void Unit::draw_selected()
{
	//------- determine the hit bar type -------//

	#define HIT_BAR_TYPE_COUNT  3

	int  hit_bar_color_array[HIT_BAR_TYPE_COUNT] = { 0xA8, 0xB4, 0xAC };
	int  hit_bar_max_array[HIT_BAR_TYPE_COUNT] 	= { 50, 100, 200 };
	char hitBarColor;
	int  hitBarMax;
	
	int  maxHitPoints = max_hit_points();

	for( int i=0 ; i<HIT_BAR_TYPE_COUNT ; i++ )
	{
		if( maxHitPoints <= hit_bar_max_array[i] || i==HIT_BAR_TYPE_COUNT-1 )
		{
			hitBarColor = hit_bar_color_array[i];
			hitBarMax   = max( maxHitPoints, hit_bar_max_array[i] );
			break;
		}
	}

	//----- draw the hit point bar in a buffer -----//

	enum { HIT_BAR_HEIGHT=3 };

	enum { HIT_BAR_LIGHT_BORDER = 0,
			 HIT_BAR_DARK_BORDER  = 3,
			 HIT_BAR_BODY 		    = 1 };

	enum { NO_BAR_LIGHT_BORDER = 0x40+11,
			 NO_BAR_DARK_BORDER  = 0x40+3,
			 NO_BAR_BODY 		   = 0x40+7 };

	//----- set the display coordination and width -----//

	int maxHitBarWidth;

	int dispZ = ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT;

	if( mobile_type == UNIT_LAND )
	{
		if( unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN )
			maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2 - 11;
		else
			maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2 - 5;
	}
	else
	{
		maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2;

		if( mobile_type == UNIT_AIR )
		{
			dispZ += ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT;
		}
	}

	//----------- set other vars -----------//

	int   curBarWidth = maxHitBarWidth * (int)max_hit_points() / hitBarMax;
	int   pointX = (curBarWidth-1) * (int) hit_points / (int)max_hit_points();	// the separating point between the area with hit point and the area without
	// ######## begin Gilbert 4/5 ########//
	if( pointX < 0 )
		pointX = 0;
	// ######## end Gilbert 4/5 ########//

	err_when( max_hit_points() > hitBarMax );
	err_when( hit_points > max_hit_points() );

	((BitmapW *)sys.common_data_buf)->init(curBarWidth, HIT_BAR_HEIGHT);

	short *dataPtr = ((BitmapW *)sys.common_data_buf)->get_ptr();
	int truePitch = ((BitmapW *)sys.common_data_buf)->get_true_pitch();

	//--------- draw the hit bar ------------//

	IMGbar( dataPtr, truePitch, 0, 0, pointX, 0, vga_back.translate_color(hitBarColor+HIT_BAR_LIGHT_BORDER) );		// top - with hit

	if( pointX < curBarWidth-1 )
		IMGbar( dataPtr, truePitch, pointX+1, 0, curBarWidth-1, 0, vga_back.translate_color(NO_BAR_LIGHT_BORDER) );		// top - without hit

	IMGbar( dataPtr, truePitch, 0, 0, 0, 2, vga_back.translate_color(hitBarColor+HIT_BAR_LIGHT_BORDER) );					// left - with hit

	IMGbar( dataPtr, truePitch, 1, 2, pointX, 2, vga_back.translate_color(hitBarColor+HIT_BAR_DARK_BORDER) );	// bottom - with hit

	if( pointX < curBarWidth-1 )
		IMGbar( dataPtr, truePitch, pointX+1, 2, curBarWidth-1, 2, vga_back.translate_color(NO_BAR_DARK_BORDER) );		// bottom - without hit

	if( pointX == curBarWidth - 1 )
		IMGbar( dataPtr, truePitch, curBarWidth-1, 1, curBarWidth-1, 1, vga_back.translate_color(hitBarColor+HIT_BAR_DARK_BORDER) );	// right -with hit
	else
		IMGbar( dataPtr, truePitch, curBarWidth-1, 1, curBarWidth-1, 1, vga_back.translate_color(NO_BAR_DARK_BORDER) );				// right -without hit

	IMGbar( dataPtr, truePitch, 1, 1, min(pointX,curBarWidth-2), 1, vga_back.translate_color(hitBarColor+HIT_BAR_BODY) );	// bar body

	if( pointX < curBarWidth - 2 )
		IMGbar( dataPtr, truePitch, pointX+1, 1, curBarWidth-2, 1, vga_back.translate_color(NO_BAR_BODY) );	// bar body

	//--------- display the bar now ---------//

	SpriteInfo* spriteInfo = sprite_res[unit_res[unit_id]->sprite_id];

	world.zoom_matrix->put_bitmapW_offset( cur_x, cur_y, get_z(), (short *)sys.common_data_buf,
		-maxHitBarWidth/2, spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6, 0, 0);

	//----- display rank icon (only for own units) -----//

//	if( is_own() || config.show_ai_info || skill_level() > 0 || rank_id > RANK_SOLDIER )
	if( is_own() || config.show_ai_info || skill_level() > 0 )
	{
		draw_rank_icon();
	}
}
//----------- End of function Unit::draw_selected -----------//

//----------- Begin of function Unit::draw_rank_icon ----------//

void Unit::draw_rank_icon()
{
//	if( !race_id )		
//		return;
// not only for human units

	int maxHitBarWidth;

	int dispZ = ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT;

	if( mobile_type == UNIT_LAND )
	{
		if( unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN )
			maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2 - 11;
		else
			maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2 - 5;
	}
	else
	{
		maxHitBarWidth = (ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH)/2;

		if( mobile_type == UNIT_AIR )
		{
			dispZ += ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT;
		}
	}

	//------- general/king icon -------//

	int yOffset = 0;
	SpriteInfo* spriteInfo = sprite_res[unit_res[unit_id]->sprite_id];

/*	if( is_own() )
	{
		String str;

		switch( rank_id )
		{
			case RANK_KING:
			{
			//	if (race_id != 0)
					str = "U_KING";
			//	else
			//		str = "M_KING";
				break;
			}

			case RANK_GENERAL:
				str = "U_GENE";
				break;
		}

		if(*str)
		{
			char *iconPtr = image_icon.get_ptr(str);
			world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
				-maxHitBarWidth/2 - ((Bitmap *)iconPtr)->get_width(),
				spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6,
				NULL, 0, 0);

			yOffset += ((Bitmap *)iconPtr)->get_height();		// put spy icon lower
		}
	}

	//----------- spy icon ------------//

	if( spy_recno &&
		 (true_nation_recno() == nation_array.player_recno ||
		  config.show_ai_info) )
	{
		char *iconPtr = image_icon.get_ptr("U_SPY");
		world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
			-maxHitBarWidth/2 - ((Bitmap *)iconPtr)->get_width(), 
			spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 + yOffset,
			NULL, 0, 0);
	}*/

	char* iconName = NULL;
	if( is_own() )
	{
		switch( rank_id )
		{
			case RANK_KING:
			{
				if( spy_recno &&
					true_nation_recno() == nation_array.player_recno )
				{
					iconName = "U_S_K";
				}
				else if( hero_id && is_own() )
				{
					iconName = "U_H_K";
				}
				else
				{
					iconName = "U_KING";
				}	

				break;
			}

			case RANK_GENERAL:
			{
				if( spy_recno &&
					true_nation_recno() == nation_array.player_recno )
				{
					iconName = "U_S_G";
				}
				else if( hero_id && is_own() )
				{
					iconName = "U_H_G";
				}
				else
				{
					iconName = "U_GENE";
				}
				
				break;
			}
		}
	}

	if (!iconName)
	{
		if( spy_recno &&
		 (true_nation_recno() == nation_array.player_recno ||
			  config.show_ai_info) )
		{
			iconName = "U_SPY";
		}
		else if( hero_id  && is_own() )
		{
			iconName = "U_HERO";
		}
	//	else
	//		err_here();
	}	
	
	char *iconPtr;
	if (iconName && (iconPtr = image_icon.get_ptr(iconName)) )
	{
		world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
			-((Bitmap *)iconPtr)->get_width()/2, 
			spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 -((Bitmap *)iconPtr)->get_height(),
			NULL, 0, 0);
	}

	if( is_own() || config.show_ai_info )
	{
		if( item.id && (iconPtr = item_res.item_unit_interface(item.id)) )
			world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
				-maxHitBarWidth/2 -((Bitmap *)iconPtr)->get_width(), 
				spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 -((Bitmap *)iconPtr)->get_height()/2,
				NULL, 0, 0);
	}		
	
	if( (is_own() || config.show_ai_info) && is_royal )
	{
		iconPtr = image_icon.get_ptr("U_ROYAL");
		if( iconPtr )
			world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
				maxHitBarWidth/2, 
				spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 -((Bitmap *)iconPtr)->get_height()/2,
				NULL, 0, 0);
	}
}
//----------- End of function Unit::draw_rank_icon ----------//

