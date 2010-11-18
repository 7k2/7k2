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

//Filename    : OFIRMDRW.CPP
//Description : Firm drawing routines

#include <ofirm.h>
#include <of_base.h>
#include <colcode.h>
#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <onation.h>
#include <osprite.h>
#include <oinfo.h>
#include <ofirmres.h>
#include <orawres.h>
#include <opower.h>
#include <ogame.h>
#include <oanline.h>
#include <oimgres.h>
#include <oworld.h>
#include <oworldmt.h>
#include <ose.h>
#include <oremote.h>
#include <of_lair.h>
#include <of_fort.h>
#include <of_lish.h>
#include <obitmap.h>
#include <oblob2w.h>
#include <olocate.h>
#include <ohelp.h>
#include <oseditor.h>

//------- define static vars -------//

struct Point
{
	short	x;
	short y;
};

static Point slot_point_array[] =
{
	{  6,  6 },
	{  6, 14 },
	{  6, 22 },
	{ 14,  6 },
	{ 14, 14 },
	{ 14, 22 },
	{ 22,  6 },
	{ 22, 14 },
	{ 22, 22 },
};

//------- Begin of function Firm::draw -----------//
//
// Draw the firm on the map
//
// [int] displayLayer   : 1 = same layer with units (default : 1)
//                      : 2 = layer above units
//                      : 4 = layer below units
//
void Firm::draw(int displayLayer)
{
//	String str;
	if (world.map_matrix->zoom_map_building_disappear)
	{	
		if (displayLayer != 4)	// only display bottom layer
			return;

		int animLineColorSeries = 0;
		
		short curX1 = loc_x1 * LOCATE_WIDTH;
		short curY1 = loc_y1 * LOCATE_HEIGHT;
		short curX2 = (loc_x2 + 1) * LOCATE_WIDTH;
		short curY2 = (loc_y2 + 1) * LOCATE_HEIGHT;
		short curZ = altitude;
		Blob2DW maskBitmap;
		int pixelColor = nation_array.nation_color_array[nation_recno];

		int xLoc, yLoc;
		LocationCorners lc;
				
		for( yLoc = loc_y1; yLoc <= loc_y2; ++yLoc )
		{
			for( xLoc = loc_x1; xLoc <= loc_x2; ++xLoc )
			{
				world.get_loc_corner(xLoc, yLoc, &lc);
				maskBitmap.clear();
				lc.render_special(&maskBitmap, 1, NULL, pixelColor, TRANSPARENT_CODE);
			
				world.zoom_matrix->put_bitmapW_offset(xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, lc.top_left->altitude,
					maskBitmap.bitmap_ptr(), maskBitmap.left_edge, maskBitmap.top_edge, 0, 0);
			}
		}

		world.zoom_matrix->put_center_text( (curX1+curX2)/2, (curY1+curY2)/2, curZ, firm_name() );
	
		return;
	}
			
	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);

	// ##### begin Gilbert 21/9 ######//
	short *colorRemapTable = firmBuild->get_color_remap_table(nation_recno, firm_array.selected_recno == firm_recno);
	if( under_construction )
	{
		colorRemapTable = firm_res.calc_color_remap_table( colorRemapTable, 
			hit_points / max_hit_points() );
	}
	else
	{
		colorRemapTable = firm_res.calc_color_remap_table( colorRemapTable, 1.0f );
	}
	// ##### end Gilbert 21/9 ######//

	// if in construction, don't draw ground unless the last construction frame
	if( firmBuild->ground_bitmap_recno &&
		(!under_construction || construction_frame() >= firmBuild->under_construction_bitmap_count-1))
	{
		// ###### begin Gilbert 21/9 ########//
		firm_res.get_bitmap(firmBuild->ground_bitmap_recno)
			->draw_at(loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude, colorRemapTable, displayLayer, firmBuild);
		// ###### end Gilbert 21/9 ########//
	}

	if( firmBuild->animate_full_size )
	{
		draw_full_size(displayLayer);
	}
	else
	{
		draw_frame(firm_cur_frame, displayLayer);
/*		if( under_construction )
		{
			draw_full_size(displayLayer);
		}
//		else if( !is_operating() )
//		{
//			if( firm_res.get_bitmap(firmBuild->idle_bitmap_recno) )
//				draw_full_size(displayLayer);
//			else
//			{
//				draw_frame(1, displayLayer);
//				draw_frame(2, displayLayer);
//			}
//		}
		else
		{
			//if (cur_frame != 1)
			//	draw_frame(1, 0, displayLayer);				// the first frame is the common frame for multi-segment bitmaps
			draw_frame(firm_cur_frame, displayLayer);
		}*/
	}
}
//--------- End of function Firm::draw -----------//


//------- Begin of function Firm::draw_full_size -----------//
//
// Draw the firm on the map
//
void Firm::draw_full_size(int displayLayer)
{
	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
//	if (cast_to_FirmLishorr())
//		firmBuild = firmBuild + cast_to_FirmLishorr()->animation_offset;

	//------- get the color remap table for this bitmap ------//

	short *stdRemapTable = game.get_color_remap_table(nation_recno, firm_array.selected_recno == firm_recno);
	// ##### begin Gilbert 21/9 ######//
	// short *colorRemapTable = firmBuild->get_color_remap_table(nation_recno, firm_array.selected_recno == firm_recno);
	short *colorRemapTable = firm_res.cache_color_remap_table;	// set in firm_res.calc_color_remap_table
	// ##### end Gilbert 21/9 ######//

	// ------ draw flags behind the building -------//
	if( under_construction )
	{
#define FLAG_WIDTH 9
#define FLAG_HEIGHT 25
		char *flagBitmapPtr = image_spict.get_ptr("FLAG-S0");
		// int drawX = loc_x1 * ZOOM_LOC_WIDTH - world.view_top_x + ZOOM_X1;
		// int drawY = loc_y1 * ZOOM_LOC_HEIGHT - world.view_top_y + ZOOM_Y1;
		world.zoom_matrix->put_bitmap_offset(
			loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, 
			altitude,//world.get_corner(loc_x1, loc_y1)->get_altitude(),
			flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
	}

	//---------- get the bitmap pointer ----------//

	FirmBitmap* firmBitmap = NULL;

	if( under_construction )
	{
		int buildFraction = construction_frame();
		firmBitmap = firm_res.get_bitmap(firmBuild->under_construction_bitmap_recno
			+ buildFraction);
	}

	else if( !is_operating() )                      // is_operating() is a virtual function
		firmBitmap = firm_res.get_bitmap(firmBuild->idle_bitmap_recno);

	else
		firmBitmap = firm_res.get_bitmap(firmBuild->first_bitmap(firm_cur_frame[0]));

	// ###### begin Gilbert 21/9 ######//
	// ------ check if the display layer is correct ---------//
//	if( !firmBitmap || !(firmBitmap->display_layer & displayLayer) )
//		return;

//	world.zoom_matrix->put_bitmap_offset(loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT,
//		altitude, firmBitmap->bitmap_ptr, 
//		firmBitmap->offset_x, firmBitmap->offset_y,
//		colorRemapTable, 0, 1);

	if( firmBitmap && firmBitmap->display_layer != 4 )
	{
//		firmBitmap->draw_at( loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude,
//			colorRemapTable, displayLayer, cast_to_FirmLishorr() ? firmBuild - cast_to_FirmLishorr()->animation_offset : firmBuild);
		firmBitmap->draw_at( loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude,
			colorRemapTable, displayLayer, firmBuild);
	}

	// ###### end Gilbert 21/9 ######//

	// ------ draw flags in front of the building -------//
	if( under_construction )
	{
		char *flagBitmapPtr = image_spict.get_ptr("FLAG-S0");
		world.zoom_matrix->put_bitmap_offset(
			(loc_x2+1) * LOCATE_WIDTH - 1, loc_y1 * LOCATE_HEIGHT,
			altitude,//world.get_corner(loc_x2+1, loc_y1)->get_altitude(), 
			flagBitmapPtr, 
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
		world.zoom_matrix->put_bitmap_offset(
			loc_x1 * LOCATE_WIDTH, (loc_y2+1) * LOCATE_HEIGHT - 1,
			altitude,//world.get_corner(loc_x1, loc_y2+1)->get_altitude(), 
			flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
		world.zoom_matrix->put_bitmap_offset(
			(loc_x2+1) * LOCATE_WIDTH - 1, (loc_y2+1) * LOCATE_HEIGHT - 1,
			altitude,//world.get_corner(loc_x2+1, loc_y2+1)->get_altitude(),	
			flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
	}
}
//--------- End of function Firm::draw_full_size -----------//


//------- Begin of function Firm::draw_frame -----------//
//
// Draw a specific frame of the firm.
//
void Firm::draw_frame(char* firmCurFrame, int displayLayer)
{
	//---------- draw animation now ------------//

	FirmBuild*  firmBuild = firm_res.get_build(firm_build_id);
	FirmBitmap* firmBitmap;

	int i, j;

	int aniPartCount = firmBuild->ani_part_count;

	short *colorRemapTable = firm_res.cache_color_remap_table;	// set in firm_res.calc_color_remap_table
	short *stdRemapTable = game.get_color_remap_table(nation_recno, firm_array.selected_recno == firm_recno);

	// ------ draw flags behind the building -------//
	if( under_construction )
	{
#define FLAG_WIDTH 9
#define FLAG_HEIGHT 25
		char *flagBitmapPtr = image_spict.get_ptr("FLAG-S0");
		world.zoom_matrix->put_bitmap_offset(
			loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, 
			world.get_corner(loc_x1, loc_y1)->get_altitude(), flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
	}


	if( under_construction || !is_operating() )
//	if( under_construction )
	{
		int frameCount = firmBuild->frame_count;
		int firstBitmap;
		int bitmapCount;
		int aniPartCount = firmBuild->ani_part_count;
		for (int i = 0; i < aniPartCount ; i++)
		{
			for ( int k=1; k <frameCount; k++ )
			{
				firstBitmap = firmBuild->first_bitmap(k+1);
				bitmapCount = firmBuild->bitmap_count(k+1);

				for ( int j=0; j <bitmapCount; j++ )
				{
					firmBitmap = firm_res.get_bitmap(firstBitmap + j);
					if( firmBitmap )
					{
						if (firmBitmap->ani_part == (i + 1))
						{
							if (firmBuild->use_first_frame)
								firm_cur_frame[i] = 1;
							else
								firm_cur_frame[i] = 2;
							break;
						}
					}
				}
				if (j < bitmapCount)
					break;
			}
		}
	}
	int firstBitmap = firmBuild->first_bitmap(1);
	firmBitmap = firm_res.get_bitmap(firstBitmap);
	if( firmBitmap && firmBitmap->display_layer != 4 )
		firmBitmap->draw_at(loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude,
			colorRemapTable, displayLayer, firmBuild);

	for( i=1 ; i<=aniPartCount ; i++ )
	{
		if (firmCurFrame[i-1] > 1)
		{
			firstBitmap = firmBuild->first_bitmap(firmCurFrame[i-1]);
			int bitmapCount = firmBuild->bitmap_count(firmCurFrame[i-1]);
		
			for ( j=0; j <bitmapCount; j++ )
			{
				firmBitmap = firm_res.get_bitmap(firstBitmap + j);
				if( firmBitmap && firmBitmap->display_layer != 4 )
					if (firmBitmap->ani_part == i)
					{
						firmBitmap->draw_at(loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude,
						colorRemapTable, displayLayer, firmBuild);
					}
			}
		}
	}

		// ------ draw flags in front of the building -------//
	if( under_construction )
	{
		char *flagBitmapPtr = image_spict.get_ptr("FLAG-S0");
		world.zoom_matrix->put_bitmap_offset(
			(loc_x2+1) * LOCATE_WIDTH - 1, loc_y1 * LOCATE_HEIGHT,
			world.get_corner(loc_x2+1, loc_y1)->get_altitude(), flagBitmapPtr, 
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
		world.zoom_matrix->put_bitmap_offset(
			loc_x1 * LOCATE_WIDTH, (loc_y2+1) * LOCATE_HEIGHT - 1,
			world.get_corner(loc_x1, loc_y2+1)->get_altitude(), flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
		world.zoom_matrix->put_bitmap_offset(
			(loc_x2+1) * LOCATE_WIDTH - 1, (loc_y2+1) * LOCATE_HEIGHT - 1,
			world.get_corner(loc_x2+1, loc_y2+1)->get_altitude(),	flagBitmapPtr,
			-FLAG_WIDTH/2, -FLAG_HEIGHT,
			stdRemapTable, 0, 1);
	}
}
//--------- End of function Firm::draw_frame -----------//

//------- Begin of function Firm::draw_detect_link_line ---------//
//
// [int] actionDetect - 0 - this is a draw action
//								1 - this is a detect action
//                      (default: 0)
//
// return: <int> 1 - detected
//					  0 - not detected
//
int Firm::draw_detect_link_line(int actionDetect)
{
	if( firm_id == FIRM_INN ) 	// FirmInn's link is only for scan for neighbor inns quickly, the link line is not displayed
		return 0;

	//--------------------------------------//

	int 	i, firmX, firmY, townX, townY;
	Firm* firmPtr;
	Town* townPtr;
	FirmInfo* firmInfo = firm_res[firm_id];

	//-------- set source points ----------//

	int srcX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
		(loc_x1+loc_x2+1)*LOCATE_WIDTH/2, (loc_y1+loc_y2+1)*LOCATE_HEIGHT/2, 
		altitude);
	int srcY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
		(loc_x1+loc_x2+1)*LOCATE_WIDTH/2, (loc_y1+loc_y2+1)*LOCATE_HEIGHT/2, 
		altitude);

	//------ draw lines to linked firms ---------//

	char* bitmapPtr;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		firmX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);
		firmY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);

		if( !actionDetect )
		{
			anim_line.draw_line(&vga_back, srcX, srcY, firmX, firmY, linked_firm_enable_array[i]==LINK_EE );
		}

		//----- check if this firm can toggle link or not -----//

		if( !can_toggle_firm_link(firmPtr->firm_recno) )
			continue;

		//------ if the link is switchable -------//

		bitmapPtr = power.get_link_icon( linked_firm_enable_array[i], nation_recno==firmPtr->nation_recno );
		help.set_help( firmX-16, firmY-16, firmX-16+((Bitmap *)bitmapPtr)->get_width(),
				firmY-16 +((Bitmap *)bitmapPtr)->get_height(), "FIRMLOCK" );

		if( actionDetect )
		{
			if( is_own() && world.zoom_matrix->detect_bitmap_clip( firmX-16, firmY-16, bitmapPtr ) )
		//	if( is_own() && world.zoom_matrix->detect_bitmap_clip( firmX-11, firmY-11, bitmapPtr ) )
			{
				if( linked_firm_enable_array[i] & LINK_ED )
				{
					toggle_firm_link( i+1, 0, COMMAND_PLAYER );
					se_ctrl.immediate_sound("TURN_OFF");
				}
				else
				{
					toggle_firm_link( i+1, 1, COMMAND_PLAYER );
					se_ctrl.immediate_sound("TURN_ON");
				}
				return 1;
			}
		}
		else
		{
			if( nation_recno == nation_array.player_recno )
			//	world.zoom_matrix->put_bitmap_clip( firmX-11, firmY-11, bitmapPtr );
				world.zoom_matrix->put_bitmap_clip( firmX-16, firmY-16, bitmapPtr );
		}
	}

	//------ draw lines to linked towns ---------//

	for( i=0 ; i<linked_town_count ; i++ )
	{
		townPtr = town_array[linked_town_array[i]];

		townX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
			(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2,
			townPtr->altitude);
		townY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
			(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2,
			townPtr->altitude);

		if( !actionDetect )
		{
			int flag =0;

			if( is_own() && active_link_town_recno == linked_town_array[i] )
				flag = 1;

			//--- if this is a Fryhtan Lair, draw links to submission towns with a think line ---//

			if( firm_id == FIRM_LAIR && nation_recno &&
				 townPtr->nation_recno == 0 &&
				 townPtr->resistance(nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
			{
				flag = 1;
			}

			if ( !flag )
			{
				anim_line.draw_line(&vga_back, srcX, srcY, townX, townY, linked_town_enable_array[i]==LINK_EE, 0 );
			}
			else
			{
				anim_line.thick_line(&vga_back, srcX, srcY, townX, townY, linked_town_enable_array[i]==LINK_EE, -1 );
			}
		}

		//----- check if this firm can toggle link or not -----//

		if( can_toggle_town_link(linked_town_array[i]) )
			bitmapPtr = power.get_link_icon( linked_town_enable_array[i], nation_recno==townPtr->nation_recno );
		else if( can_set_active_link(linked_town_array[i]) )
		{
			bitmapPtr = image_icon.read( active_link_town_recno == linked_town_array[i] ? "LINK_ACT" : "LINK_DEA" );
			help.set_help( townX-16, townY-16, townX+14, townY+14, "TOWNLOCK" );
		}
		else
			continue;

		//--------- draw link symbol -----------//

		if (!cast_to_FirmCamp())
			help.set_help( townX-16, townY-16, townX-16+((Bitmap *)bitmapPtr)->get_width(),
				townY-16 +((Bitmap *)bitmapPtr)->get_height(), "WORKLOCK" );


		if( actionDetect )
		{
		//	int rc = world.zoom_matrix->detect_bitmap_clip( townX-11, townY-11, bitmapPtr );
			int rc = world.zoom_matrix->detect_bitmap_clip( townX-16, townY-16, bitmapPtr );

			//------ left clicking to toggle link -------//

			if( rc==1 && is_own() )
			{
				if( can_set_active_link(linked_town_array[i]) )
				{
					// -------- set active link of fort -------//

					set_active_link( linked_town_array[i], COMMAND_PLAYER );
				}
				else 
				{
					if( linked_town_enable_array[i] & LINK_ED )
					{
						toggle_town_link( i+1, 0, COMMAND_PLAYER );
						se_ctrl.immediate_sound("TURN_OFF");
					}
					else
					{
						toggle_town_link( i+1, 1, COMMAND_PLAYER );
						se_ctrl.immediate_sound("TURN_ON");
					}

					//
					// update RemoteMsg::firm_toggle_link_town()
					//
					if( cast_to_FirmCamp() && !remote.is_enable())
					{
						if( townPtr->nation_recno )
							townPtr->update_target_loyalty();
						else
							townPtr->update_target_resistance();

						townPtr->update_camp_link();
					}
				}
				return 1;
			}
		}
		else
		{
			if( nation_recno == nation_array.player_recno )
			{
				world.zoom_matrix->put_bitmap_clip( townX-16, townY-16, bitmapPtr );
			}
		}
	}

	return 0;
}
//------- End of function Firm::draw_detect_link_line ---------//


//------- Begin of function Firm::is_in_zoom_win -----------//
//
// Whether the firm is in the current zoom window.
//
int Firm::is_in_zoom_win()
{
	// BUGHERE not implemented

	/*

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);

	//-------- check if the firm is within the view area --------//

	int x1 = abs_x1 - World::view_top_x;

	if( x1 <= -firmBuild->max_bitmap_width || x1 >= ZOOM_WIDTH )	// out of the view area, not even a slight part of it appears in the view area
		return 0;

	int y1 = abs_y1 - World::view_top_y;

	if( y1 <= -firmBuild->max_bitmap_height || y1 >= ZOOM_HEIGHT )
		return 0;

	*/
	return 1;
}
//--------- End of function Firm::is_in_zoom_win -----------//



//------- Begin of function Firm::draw_selected -----------//
//
// Draw a square around the firm on the map.
//
void Firm::draw_selected()
{
/*
	//------ calculate frame coordinations ---------//

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);

	int x1 = loc_x1 * ZOOM_LOC_WIDTH;
	int y1 = loc_y1 * ZOOM_LOC_HEIGHT;
	int x2 = (loc_x1+firmBuild->loc_width)  * ZOOM_LOC_WIDTH  - 1;
	int y2 = (loc_y1+firmBuild->loc_height) * ZOOM_LOC_HEIGHT - 1;

	x1 = x1 - World::view_top_x + ZOOM_X1;
	y1 = y1 - World::view_top_y + ZOOM_Y1;
	x2 = x2 - World::view_top_x + ZOOM_X1;
	y2 = y2 - World::view_top_y + ZOOM_Y1;

	//------------- set frame color -------------//

	char frameColor;

	if( nation_recno == nation_array.player_recno )
		frameColor = OWN_SELECT_FRAME_COLOR;
	else
		frameColor = ENEMY_SELECT_FRAME_COLOR;

	//------------ draw the square frame now ------------//

	if( m.is_touch( x1, y1, x2, y2, ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2 ) )
	{
		//------- Only draw_selected the portion within the zoom window area ----//

		if( y1 >= ZOOM_Y1 )		// square top
			vga_back.bar( max(x1,ZOOM_X1), y1, min(x2,ZOOM_X2), y1, frameColor );

		if( y2 <= ZOOM_Y2 )		// square bottom
			vga_back.bar( max(x1,ZOOM_X1), y2, min(x2,ZOOM_X2), y2, frameColor );

		if( x1 >= ZOOM_X1 )		// square left
			vga_back.bar( x1, max(y1,ZOOM_Y1), x1, min(y2,ZOOM_Y2), frameColor );

		if( y1 <= ZOOM_X2 )		// square left
			vga_back.bar( x2, max(y1,ZOOM_Y1), x2, min(y2,ZOOM_Y2), frameColor );

      //------------ display hit point bar -----------//

		if( firm_res[firm_id]->buildable )
		{
			x1 = x1+1;
			y1 = max( y2-4, ZOOM_Y1 );
			x2 = x2-1;
			y2 = min( y2-1, ZOOM_Y2 );

			if( x1<=ZOOM_X2 && x2>=ZOOM_X1 && y1<=ZOOM_Y2 && y2>=ZOOM_Y1 )
			{
				int barWidth = (x2-x1+1) * hit_points / max_hit_points;

				if( hit_points > 0 && x1+barWidth-1 >= ZOOM_X1)
				{
					vga_back.bar( max(x1,ZOOM_X1), y1  , min(x1+barWidth-1,ZOOM_X2), y1, frameColor );
					vga_back.bar( max(x1,ZOOM_X1), y1+1, min(x1+barWidth-1,ZOOM_X2), y2, V_GREEN );
				}
			}
		}
	}
*/
	//------- draw lines connected to town ---------//

	draw_detect_link_line(0);
}
//--------- End of function Firm::draw_selected -----------//


/*
//------- Begin of function Firm::draw_cargo -----------//
//
// Draw cargos of stock.
//
// <int>   cargoCount 	  - the no. of cargos to be drawn
// <char*> cargoBitmapPtr - bitmap ptr to the cargo
//
void Firm::draw_cargo(int cargoCount, char* cargoBitmapPtr)
{
	//------ display a pile of raw materials ------//

	int x = loc_x1*LOCATE_WIDTH;
	int y = loc_y1*LOCATE_HEIGHT;

	for( int i=0 ; i<cargoCount ; i++ )
	{
		world.zoom_matrix->put_bitmap(
			x+slot_point_array[i].x, 
			y+slot_point_array[i].y,
			altitude, cargoBitmapPtr, NULL, 0, 0);
	}
}
//--------- End of function Firm::draw_cargo -----------//
*/

//------- Begin of function Firm::can_toggle_town_link ---------//
//
int Firm::can_toggle_town_link(int townRecno)
{
	if( cast_to_FirmMarket() )
		return 0;

	if( cast_to_FirmCamp() && town_array[townRecno]->nation_recno == nation_recno)
		return 0;		// same nation cannot toggle

	if( can_set_active_link(townRecno) )
		return 0;

	//--- nation firms hiring independent villagers -----//
	if( cast_to_FirmLair() && town_array[townRecno]->nation_recno==0 && nation_recno )
		return 0;		// exclude slave town

	if( town_array[townRecno]->nation_recno==0 && nation_recno )
		return town_array[townRecno]->resistance(nation_recno) <= INDEPENDENT_LINK_RESISTANCE;

	return 1;
}
//------- Begin of function Firm::can_toggle_town_link ---------//


//------- Begin of function Firm::can_toggle_firm_link ---------//
//
int Firm::can_toggle_firm_link(int firmRecno)
{
	Firm* firmPtr = firm_array[firmRecno];

	//----- lair to lair links are not togglable ----//

	if( firm_id==FIRM_LAIR && firmPtr->firm_id==FIRM_LAIR )
		return 0;

	return firm_res[firm_id]->is_linkable_to_firm(firmPtr->firm_id);
}
//------- End of function Firm::can_toggle_firm_link ---------//


//------- Begin of function Firm::construction_frame ---------//
//
// return 0 to hitfirmBuild->under_construction_bitmap_count-1 
// according to hit_points and max_hitpoints
//
int Firm::construction_frame()
{
	err_when(!under_construction);
	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	int r = int( hit_points * firmBuild->under_construction_bitmap_count / max_hit_points()); 
	err_when( r < 0);
	if( r >= firmBuild->under_construction_bitmap_count )
		r = firmBuild->under_construction_bitmap_count-1;
	return r;
}
//------- End of function Firm::construction_frame ---------//


//--------- Begin of function Firm::left_click ---------//
//
// This function is called when the mouse left-click on this firm.
//
void Firm::left_click()
{
	firm_array.selected_recno = firm_recno;
}
//--------- End of function Firm::left_click ---------//

//--------- Begin of function Firm::right_click ---------//
//
// This function is called when the current firm is selected
// and the mouse right-click on a locatoin.
//
void Firm::right_click(int xLoc, int yLoc)
{
	firm_array.selected_recno = firm_recno;

	// ###### begin Gilbert 28/3 ########//
	// disable right click in edit mode
	if( scenario_editor.is_edit_mode() )
		return;
	// ###### end Gilbert 28/3 ########//

	if( can_set_rally_point() )
	{
		Location *locPtr = world.get_loc(xLoc, yLoc);
		int destBaseObj = 0 ;
		if( locPtr->is_firm() || locPtr->is_town() )
		{	
			destBaseObj = locPtr->base_obj_recno( UNIT_LAND );
		}

		set_rally_point( destBaseObj, xLoc, yLoc, COMMAND_PLAYER );
		se_ctrl.immediate_sound("TURN_ON");
	}
}
//--------- End of function Firm::right_click ---------//
