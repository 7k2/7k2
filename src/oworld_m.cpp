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

//Filename    : OWORLD_M.CPP
//Description : Object MapMatrix

#include <oworldmt.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ohelp.h>
#include <oimgres.h>
#include <obutton.h>
#include <oplant.h>
#include <otown.h>
#include <onation.h>
#include <ofirm.h>
#include <osys.h>
#include <opower.h>
#include <ogame.h>
#include <oworld.h>
#include <oterrain.h>
#include <oanline.h>
#include <ovgabuf.h>
#include <obutt3d.h>
#include	<orock.h>
#include	<orockres.h>

static Button3D button_filter_object;
static Button3D button_filter_nation;


//-------- Begin of function MapMatrix::MapMatrix ----------//

MapMatrix::MapMatrix()
{
	init( MAP_X1, MAP_Y1, MAP_X2, MAP_Y2,
			MAP_WIDTH, MAP_HEIGHT,
			0.5, 0.5, -0.5, 0.5,
			1,			// isometric view
			1 );    // 1-create a background buffer
}
//---------- End of function MapMatrix::MapMatrix ----------//


//-------- Begin of function MapMatrix::~MapMatrix ----------//

MapMatrix::~MapMatrix()
{
}
//---------- End of function MapMatrix::~MapMatrix ----------//


//---------- Begin of function MapMatrix::init_para ------------//
void MapMatrix::init_para()
{
	last_map_mode = MAP_MODE_POWER;

	map_mode   = MAP_MODE_POWER;
	power_mode = 0;

	zoom_map_building_disappear = 0;

	filter_object_flag = 0;
	filter_object_type = 0;
	filter_object_para = 0;
	filter_object_locked = 0;

	filter_nation_flag = 0;
	filter_nation_locked = 0;

	button_filter_object.create(MAP_MODE_BUTTON_X1+56+6, MAP_MODE_BUTTON_Y1+8+19,
		"O-UP", "O-DN", 0 );
	button_filter_object.set_help_code( "OBJECT" );
	button_filter_nation.create(MAP_MODE_BUTTON_X1+30+17, MAP_MODE_BUTTON_Y1+9,
		"N-UP", "N-DN", 0 );
	button_filter_nation.set_help_code( "NATION" );
}
//---------- End of function MapMatrix::init_para ----------//


//--------- Begin of function MapMatrix::paint -----------//

void MapMatrix::paint()
{
	disp_mode_button();
}
//----------- End of function MapMatrix::paint ------------//


//------- Begin of function MapMatrix::disp_mode_button ---------//
//
// Display map mode buttons.
//
// [int] putFront - 1-display the buttons on the front buffer
//						  0-display the buttons on the back buffer
//						  (default: 0)
//
void MapMatrix::disp_mode_button(int putFront)
{
	const char* iconName;
	const char* iconName2 = NULL;

	switch(map_mode)
	{
		case MAP_MODE_TERRAIN:
			iconName = "MAP-1";
			break;

		case MAP_MODE_POWER:
			if( power_mode )
				iconName = "MAP-2B";
			else
				iconName = "MAP-2A";
			break;

		case MAP_MODE_TRADE:
		case MAP_MODE_SPOT:
			iconName = "MAP-3";
			break;

		default:
			err_here();
	}

	if (zoom_map_building_disappear)
		iconName2 = "MAP-4D";
	else
		iconName2 = "MAP-4U";

	if( putFront )
	{
		mouse.hide_area( MAP_MODE_BUTTON_X1, MAP_MODE_BUTTON_Y1, 
			MAP_MODE_BUTTON_X1+84-1, MAP_MODE_BUTTON_Y1+86-1 );
		help.short_front_buf.hide_area( MAP_MODE_BUTTON_X1, MAP_MODE_BUTTON_Y1, 
			MAP_MODE_BUTTON_X1+84-1, MAP_MODE_BUTTON_Y1+86-1 );
		image_button.put_front( MAP_MODE_BUTTON_X1, MAP_MODE_BUTTON_Y1, iconName, 1 );

		if (button_filter_object.pushed_flag)
			vga_front.put_bitmap_trans_decompress( MAP_MODE_BUTTON_X1+55, MAP_MODE_BUTTON_Y1+7,
				image_button.read("O-LOCK") );
	
		if (button_filter_nation.pushed_flag)
			vga_front.put_bitmap_trans_decompress( MAP_MODE_BUTTON_X1+30, MAP_MODE_BUTTON_Y1+7,
				image_button.read("N-LOCK") );

		char backupUseBackBuf = vga.use_back_buf;		// save use_back_buf
		vga.use_front();
		button_filter_object.paint( filter_object_flag );
		button_filter_nation.paint( filter_nation_flag );
		if( backupUseBackBuf )				// restore use_back_buf
			vga.use_back();
		else
			vga.use_front();
				
		help.short_front_buf.show_area();
		mouse.show_area();
		
		mouse.hide_area( MAP_MODE_BUTTON_X1-97, MAP_MODE_BUTTON_Y1+10, 
			MAP_MODE_BUTTON_X1-97+34-1, MAP_MODE_BUTTON_Y1+10+34-1 );
		help.short_front_buf.hide_area( MAP_MODE_BUTTON_X1-97, MAP_MODE_BUTTON_Y1+10, 
			MAP_MODE_BUTTON_X1-97+34-1, MAP_MODE_BUTTON_Y1+10+34-1 );
		image_button.put_front( MAP_MODE_BUTTON_X1-94, MAP_MODE_BUTTON_Y1+13, iconName2, 1 );
		help.short_front_buf.show_area();
		mouse.show_area();
	}
	else
	{
		help.short_back_buf.hide_area( MAP_MODE_BUTTON_X1, MAP_MODE_BUTTON_Y1, 
			MAP_MODE_BUTTON_X1+84-1, MAP_MODE_BUTTON_Y1+86-1 );
		image_button.put_back( MAP_MODE_BUTTON_X1, MAP_MODE_BUTTON_Y1, iconName, 1 );
		
		if (button_filter_object.pushed_flag)
			vga_back.put_bitmap_trans_decompress( MAP_MODE_BUTTON_X1+55, MAP_MODE_BUTTON_Y1+7,
				image_button.read("O-LOCK") );
	
		if (button_filter_nation.pushed_flag)
			vga_back.put_bitmap_trans_decompress( MAP_MODE_BUTTON_X1+30, MAP_MODE_BUTTON_Y1+7,
				image_button.read("N-LOCK") );

		char backupUseBackBuf = vga.use_back_buf;		// save use_back_buf
		vga.use_back();
		button_filter_object.paint( filter_object_flag );
		button_filter_nation.paint( filter_nation_flag );
		if( backupUseBackBuf )				// restore use_back_buf
			vga.use_back();
		else
			vga.use_front();

		help.short_back_buf.show_area();
	
		help.short_front_buf.hide_area( MAP_MODE_BUTTON_X1-97, MAP_MODE_BUTTON_Y1+10, 
			MAP_MODE_BUTTON_X1-97+34-1, MAP_MODE_BUTTON_Y1+10+34-1 );
		image_button.put_back( MAP_MODE_BUTTON_X1-94, MAP_MODE_BUTTON_Y1+13, iconName2, 1 );
		help.short_front_buf.show_area();
	}
}
//----------- End of function MapMatrix::disp_mode_button ------------//


//--------- Begin of function MapMatrix::detect ------------//

int MapMatrix::detect()
{
	int x=MAP_MODE_BUTTON_X1 + 4;
	int y=MAP_MODE_BUTTON_Y1 + 5;

	//#define MAP_MODE_BUTTON_WIDTH 24
	//#define MAP_MODE_BUTTON_HEIGHT 24

	for( int i=0 ; i<MAP_MODE_COUNT ; i++, x+=MAP_MODE_BUTTON_WIDTH, y+=MAP_MODE_BUTTON_HEIGHT )
	{
		if( mouse.single_click(	x, y, x+MAP_MODE_BUTTON_WIDTH-1, y+MAP_MODE_BUTTON_HEIGHT-1 ) )
		{
			toggle_map_mode(i);
			return 1;
		}
	}
	
	if( mouse.single_click(	MAP_MODE_BUTTON_X1-97, MAP_MODE_BUTTON_Y1+10, 
		MAP_MODE_BUTTON_X1-97+34-1, MAP_MODE_BUTTON_Y1+10+34-1 ) )
	{
		if (zoom_map_building_disappear)
			zoom_map_building_disappear = 0;
		else
			zoom_map_building_disappear = 1;
		
		disp_mode_button(1);		// 1-display the buttons on the front buffer.
	#ifdef USE_FLIP
		disp_mode_button(0);		// 0-display the buttons on the back buffer.
	#endif
		refresh();
		return 1;
	}
	
	int rc;

	if( (rc=button_filter_object.detect(0, 0, 1)) > 0 )		// 1-detect right button
	{
		if( rc==2 )		// right button - locked filter
		{
			filter_object_flag = 1;
			button_filter_object.pushed_flag = 1;
			button_filter_object.update_bitmap("O-DN");
			filter_object_locked = 1;
			set_filter_para();
		}
		else
		{
			filter_object_flag = button_filter_object.pushed_flag;
			button_filter_object.update_bitmap("O-DN2");
			filter_object_locked = 0;

			if( !filter_object_flag ) 
				filter_object_para = 0;
		}

		disp_mode_button(1);
#ifdef USE_FLIP
		disp_mode_button(0);	
#endif
		refresh();
	}
	else
		button_filter_object.set_help_code( "OBJECT" );

	if( (rc=button_filter_nation.detect(0, 0, 1)) > 0 )		// 1-detect right button
	{
		if( rc==2 )		// right button - locked filter
		{
			filter_nation_flag = 1;
			button_filter_nation.pushed_flag = 1;
			button_filter_nation.update_bitmap("N-DN");
			filter_nation_locked = 1;
			set_filter_para();
		}
		else
		{
			filter_nation_flag = button_filter_nation.pushed_flag;
			button_filter_nation.update_bitmap("N-DN2");
			filter_nation_locked = 0;
		}

		disp_mode_button(1);
#ifdef USE_FLIP
		disp_mode_button(0);		
#endif
		refresh();
	}
	else
		button_filter_nation.set_help_code( "NATION" );

	//----- detect clicking on the map -------//

	return detect_area();
}
//---------- End of function MapMatrix::detect ------------//


//---------- Begin of function MapMatrix::draw ------------//
//
// Draw world map
//
void MapMatrix::draw()
{
	draw_map();

	//------- save it to the buffer for later reuse ------//

	if( save_image_buf )
	{
		// #### begin Ban 8/12 #######//
		short *temp2 = save_image_buf +2;
		int temp3 = vga_back.buf_true_pitch() -400;
		short *temp6 = vga_back.buf_ptr(image_x1, image_y1);
#ifdef ASM_FOR_MSVC
		_asm
		{
			MOV AX , DS
			MOV	ES , AX
			MOV EAX, 200			// store it to ECX for later internal processing
			MOV EDI, temp2			// [EDI]=pointer of bitmap
			MOV	EDX, temp3			// EDX = lineDiff
			MOV	ESI, temp6			// [ESI]=pointer in screen
			CLD                     // clear direction flag for MOVSB
PutLine:	MOV ECX, 100
			REP MOVSD
			ADD	ESI, EDX			//; EDX = lineDiff
			DEC	EAX
			JNZ	PutLine		        //; decrease the remain height and loop
		}
#else
		const int lineDiff = temp3 / 2;
		for ( int i = 0; i < 200; ++i )
		{
			for ( int j = 0; j < 200; ++j )
			{
				*temp2 = *temp6;
				++temp2;
				++temp6;
			}
			temp6 += lineDiff;
		}
#endif
		// #### end Ban 8/12 #######//
		just_drawn_flag = 1;
	}
}
//------------ End of function MapMatrix::draw ------------//


//---------- Begin of function MapMatrix::draw_map ------------//
// see also World::explore
//
void MapMatrix::draw_map()
{
	//----------- draw map now ------------//

	sys.yield();

	short	fireColor = vga_back.translate_color(FIRE_COLOR);
	short plantColor = vga_back.translate_color(plant_res.plant_map_color);
	short	seaColor = vga_back.translate_color(0x32);
	short rockColor = vga_back.translate_color( 0x59 );
	short mapModeColor = vga_back.translate_color( VGA_GRAY+10 );
	short unexploredColor = vga_back.translate_color(UNEXPLORED_COLOR);
	short shadowColor = vga_back.translate_color( VGA_GRAY+2 );
	short roadColor = vga_back.translate_color( VGA_GRAY+12 );
	short	nationColorArray[MAX_NATION+2];
	for( int n = 0; n < MAX_NATION+2; ++n )
		nationColorArray[n] = vga_back.translate_color(nation_array.nation_power_color_array[n]);
	// #### begin Ban 8/12 #######//
	int temp3 = vga_back.buf_true_pitch() -2;
	short *temp6 = vga_back.buf_ptr((image_x1 + image_x2)/2, image_y1);
#ifdef ASM_FOR_MSVC
	_asm
	{
		MOV AX , DS
		MOV	ES , AX
		MOV	EDX, temp3		
		MOV	EDI, temp6
		CLD                 
		MOV	AX, word ptr UNEXPLORED_COLOR
		MOV	CX, AX				// processing the color for drawing
		SHL	EAX, 16
		MOV	AX, CX
		MOV	EBX, 100
		XOR ECX, ECX
PutLineUpper:					// draw upper triangle
		INC	ECX
		REP	STOSD
		MOV ECX, 101			// restore ECX after STOSD for updating EDI
		SUB ECX, EBX
		ADD	EDI, EDX			// updating EDI to start point of next line
		SUB EDI, ECX
		SUB EDI, ECX
		SUB EDI, ECX
		SUB EDI, ECX
		DEC	EBX					// decrease the remain height
		JNZ	PutLineUpper		// loop
		MOV	EBX, 99				// ready parameters for drawing lower triangle
		ADD EDX, 4
		ADD EDI, 4
PutLineLower:					// draw lower triangle
		MOV ECX, EBX
		REP	STOSD
		ADD	EDI, EDX			// updating EDI to start point of next line
		SUB EDI, EBX
		SUB EDI, EBX
		SUB EDI, EBX
		SUB EDI, EBX
		DEC	EBX					// decrease the remain height
		JNZ	PutLineLower		// loop
	}
#endif
	// #### end Ban 8/12 #######//
//	{	// traversal of location in MapMatrix
//		int pixelX = (image_x1 + image_x2 + 1) / 2;
//		int pixelY = image_y1;
//		int xLoc, yLoc;
//		int writePtrInc = vga_back.buf_pitch() + 1;
//		for( yLoc = 0; yLoc < max_y_loc; (yLoc&1?++pixelY:--pixelX) , ++yLoc )
//		{
//			short* writePtr  = vga_back.buf_ptr(pixelX, pixelY);
//			xLoc = 0;
//			Location *locPtr = get_loc( xLoc, yLoc );
//			for( xLoc = 0; xLoc < max_x_loc; (xLoc += 2), (writePtr += writePtrInc), (locPtr+=2) )
//			{
//			}
//		}
//	}

	switch(map_mode)
	{
	case MAP_MODE_TERRAIN:
		{
			int pixelX = (image_x1 + image_x2 + 1) / 2;
			int pixelY = image_y1;
			int xLoc, yLoc;
			int writePtrInc = vga_back.buf_pitch() + 1;
			for( yLoc = 0; yLoc < max_y_loc; (yLoc&1?++pixelY:--pixelX) , ++yLoc )
			{
				short* writePtr  = vga_back.buf_ptr(pixelX, pixelY);
				int tileYOffset = (yLoc & TERRAIN_TILE_Y_MASK) * TERRAIN_TILE_WIDTH;
				xLoc = 0;
				Location* locPtr = get_loc( xLoc, yLoc );
				for( ; xLoc < max_x_loc; (xLoc += 2), (locPtr += 2), (writePtr += writePtrInc) )
				{
					if( locPtr->explored() )
					{
						// ##### begin Gilbert 10/2 #######//
						if( filter_object_flag || filter_nation_flag )		// do not anything except filtered objects when filters are on
							*writePtr = mapModeColor;
						else if( locPtr->fire_str() > 0)
						// ##### end Gilbert 10/2 #######//
							*writePtr = fireColor;
						else if( locPtr->is_plant() || xLoc+1<max_x_loc && locPtr[1].is_plant() )
							*writePtr = plantColor;
						else if( locPtr->is_rock() )
							*writePtr = rockColor;
						else if( locPtr->has_dirt() )
						{
							Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
							RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
							if (dirtInfo->rock_type == 'P')
								*writePtr = seaColor;
							else
								*writePtr = rockColor;
						}

						else if( locPtr->is_road() )
							*writePtr = vga_back.translate_color(terrain_res.get_map_tile(locPtr->road_terrain_id)[tileYOffset + (xLoc & TERRAIN_TILE_X_MASK)]);
						else
						{
							if( xLoc < 2 || 
								terrain_res[locPtr->terrain_id]->average_type >=
								terrain_res[(locPtr-2)->terrain_id]->average_type )
							{
								// get terrain pixel from terrain_res
								*writePtr = vga_back.translate_color(terrain_res.get_map_tile(locPtr->terrain_id)[tileYOffset + (xLoc & TERRAIN_TILE_X_MASK)]);
							}
							else
							{
								*writePtr = shadowColor;
							}
						}
					}
					//else 	// not needed because filled black already
					//{
					//	*writePtr = unexploredColor;
					//}
				}
			}
		}
		break;


	// ##### begin Gilbert 2/11 #####//
	case MAP_MODE_TRADE:
	// ##### end Gilbert 2/11 #####//
	case MAP_MODE_SPOT:
		{	// traversal of location in MapMatrix
			int pixelX = (image_x1 + image_x2 + 1) / 2;
			int pixelY = image_y1;
			int xLoc, yLoc;
			int writePtrInc = vga_back.buf_pitch() + 1;
			for( yLoc = 0; yLoc < max_y_loc; (yLoc&1?++pixelY:--pixelX) , ++yLoc )
			{
				short* writePtr  = vga_back.buf_ptr(pixelX, pixelY);
				xLoc = 0;
				Location* locPtr = get_loc( xLoc, yLoc );
				for( ; xLoc < max_x_loc; (xLoc += 2), (locPtr +=2), (writePtr += writePtrInc) )
				{
					if( locPtr->explored() )
					{
						*writePtr = mapModeColor;
					}
				}
			}
		}
		break;

	case MAP_MODE_POWER:
		{
			int pixelX = (image_x1 + image_x2 + 1) / 2;
			int pixelY = image_y1;
			int xLoc, yLoc;
			int writePtrInc = vga_back.buf_pitch() + 1;
			for( yLoc = 0; yLoc < max_y_loc; (yLoc&1?++pixelY:--pixelX) , ++yLoc )
			{
				short* writePtr  = vga_back.buf_ptr(pixelX, pixelY);
				xLoc = 0;
				Location *locPtr = get_loc( xLoc, yLoc );
				for( xLoc = 0; xLoc < max_x_loc; (xLoc += 2), (locPtr +=2), (writePtr += writePtrInc) )
				{
					if( locPtr->explored() )
					{
						// ####### begin Gilbert 10/2 #########//
						if( filter_object_flag || filter_nation_flag )		// do not anything except filtered objects when filters are on
							*writePtr = mapModeColor;
						else if( locPtr->sailable() )
						// ####### end Gilbert 10/2 #########//
							*writePtr = seaColor;
						else if( locPtr->is_rock() || locPtr[1].is_rock() )
							*writePtr = rockColor;
/*						else if( locPtr->has_dirt() )
						{
							Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
							RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
							if (dirtInfo->rock_type == 'P')
								*writePtr = seaColor;
							else
								*writePtr = rockColor;
						}

						else if( locPtr->is_plant() || xLoc+1<max_x_loc && locPtr[1].is_plant() )
							*writePtr = plantColor;
*/						else
						// ###### begin Gilbert 21/12 ########//
						{
							if( power_mode )
								*writePtr = nationColorArray[locPtr->power_nation_recno];
							else
								*writePtr = nationColorArray[0];
						}
						// ###### end Gilbert 21/12 ########//
					}
				}
			}
		}
		break;

	case MAP_MODE_ALTITUDE:
		{
			int pixelX = (image_x1 + image_x2 + 1) / 2;
			int pixelY = image_y1;
			int xLoc, yLoc;
			int writePtrInc = vga_back.buf_pitch() + 1;
			for( yLoc = 0; yLoc < max_y_loc; (yLoc&1?++pixelY:--pixelX) , ++yLoc )
			{
				short* writePtr  = vga_back.buf_ptr(pixelX, pixelY);
				xLoc = 0;
				for( xLoc = 0; xLoc < max_x_loc; (xLoc += 2), (writePtr += writePtrInc) )
				{
					Corner *cornerPtr = get_corner( xLoc, yLoc );
					short alt = cornerPtr->get_altitude();
					if( alt > 255 )
						alt = 255;
					else if( alt < 0 )
						alt = 0;
					*writePtr = vga.make_pixel((BYTE)alt, 128, 32);
				}
			}
		}
		break;
	}

	sys.yield();
}
//------------ End of function MapMatrix::draw_map ------------//

//---------- Begin of function MapMatrix::set_filter_para ------------//
//
// Set the current filter parameters.
//
void MapMatrix::set_filter_para()
{
	//-------- object filter ---------//

	if( filter_object_flag )
	{
		if( unit_array.selected_recno )
		{
			filter_object_type = OBJ_UNIT;
			filter_object_para = unit_res[unit_array[unit_array.selected_recno]->unit_id]->unit_class;
		}
		else if( firm_array.selected_recno )
		{
			filter_object_type = OBJ_FIRM;
			filter_object_para = firm_array[firm_array.selected_recno]->firm_id;
		}
		else if( town_array.selected_recno )
		{
			filter_object_type = OBJ_TOWN;
			filter_object_para = town_array[town_array.selected_recno]->is_pay_tribute_to_monster(nation_array.player_recno);
		}
		else
			filter_object_type = 0;		// none type
	}

	//--------- nation filter ---------//

	if( filter_nation_flag )
	{
		if( unit_array.selected_recno )
			filter_nation_recno = unit_array[unit_array.selected_recno]->nation_recno;
		else if( firm_array.selected_recno )
			filter_nation_recno = firm_array[firm_array.selected_recno]->nation_recno;
		else if( town_array.selected_recno )
			filter_nation_recno = town_array[town_array.selected_recno]->nation_recno;
		else
			filter_nation_recno = -1;		// none type
	}
}
//------------ End of function MapMatrix::set_filter_para ------------//


//----------- Begin of function MapMatrix::disp ------------//
//
// Display the drawn world map on screen, update the location
// of the map-to-zoom area box.
//
void MapMatrix::disp()
{
	// ------- find set filter para -------//

	if( (filter_object_flag && !filter_object_locked) || 
		 (filter_nation_flag && !filter_nation_locked) )
	{
		set_filter_para();
	}

	//--------------------------------------------//

	if( !just_drawn_flag )		// if the map has just been drawn in draw()
	{
		if( save_image_buf && map_mode==last_map_mode )
		{
		// #### begin Ban 8/10 #######//
			short *temp2 = save_image_buf +101;
			int temp3 = vga_back.buf_true_pitch() -2;
			short *temp6 = vga_back.buf_ptr((image_x1 +image_x2)/2, image_y1);
#ifdef ASM_FOR_MSVC
		_asm
		{
			MOV AX , DS
			MOV	ES , AX
			MOV EAX, 100		// bitmap height
			MOV	EDX, temp3		// EDX = lineDiff
			MOV ESI, temp2		// [ESI]=pointer of bitmap
			MOV	EDI, temp6		// [EDI]=pointer in screen
			CLD					// clear direction flag for MOVSB
			XOR ECX, ECX
PutLineUpper:
			INC ECX
			MOV EBX, ECX
			REP MOVSD			// each time writes two pixels				
			ADD	EDI, EDX		// increase EDI
			SUB EDI, EBX
			SUB EDI, EBX
			SUB EDI, EBX
			SUB EDI, EBX
			ADD	ESI, 398		// increase ESI
			SUB ESI, EBX
			SUB ESI, EBX
			SUB ESI, EBX
			SUB ESI, EBX
			MOV ECX, EBX
			DEC	EAX				// decrease the remain height
			JNZ	PutLineUpper	// loop
			MOV EAX, 99			// bitmap height
			ADD EDX, 4
			ADD EDI, 4
			ADD ESI, 4
PutLineLower:
			MOV ECX, EAX
			REP MOVSD			// each time writes two pixels				
			ADD	EDI, EDX		// increase EDI
			SUB EDI, EAX
			SUB EDI, EAX
			SUB EDI, EAX
			SUB EDI, EAX
			ADD	ESI, 402		// increase ESI
			SUB ESI, EAX
			SUB ESI, EAX
			SUB ESI, EAX
			SUB ESI, EAX
			DEC	EAX				// decrease the remain height
			JNZ	PutLineLower	// loop
		}
#endif
		// #### end Ban 8/12 #######//
		}
		else
		{
			draw();
			last_map_mode = map_mode;
		}
	}
	just_drawn_flag=0;
}
//----------- End of function MapMatrix::disp ------------//


//----------- Begin of function MapMatrix::draw_square ------------//
//
// Calling sequences:
//
// 1. MapMatrix::disp()
// 2. SpriteArray::draw()
// 3. MapMatrix::draw_square()
//
void MapMatrix::draw_square()
{
	//-------- draw the map-to-zoom highlight box --------//

	///static int squareFrameCount=0, squareFrameStep=1;

/*
	int x0 = image_x1+world.zoom_matrix->calc_cur_x(
		world.zoom_matrix->get_base_x(), world.zoom_matrix->get_base_y())
		/ LOCATE_WIDTH * loc_width;
	int y0 = image_y1+world.zoom_matrix->calc_cur_y(
		world.zoom_matrix->get_base_x(), world.zoom_matrix->get_base_y())
		/ LOCATE_HEIGHT * loc_height;
	int x1 = image_x1+world.zoom_matrix->calc_cur_x(
		world.zoom_matrix->get_base_x() + world.zoom_matrix->image_width,
		world.zoom_matrix->get_base_y())
		/ LOCATE_WIDTH * loc_width;
	int y1 = image_y1+world.zoom_matrix->calc_cur_y(
		world.zoom_matrix->get_base_x() + world.zoom_matrix->image_height,
		world.zoom_matrix->get_base_y())
		/ LOCATE_HEIGHT * loc_height;
	int x2 = image_x1+world.zoom_matrix->calc_cur_x(
		world.zoom_matrix->get_base_x() + world.zoom_matrix->image_width,
		world.zoom_matrix->get_base_y() + world.zoom_matrix->image_height)
		/ LOCATE_WIDTH * loc_width;
	int y2 = image_y1+world.zoom_matrix->calc_cur_y(
		world.zoom_matrix->get_base_x() + world.zoom_matrix->image_height,
		world.zoom_matrix->get_base_y() + world.zoom_matrix->image_height)
		/ LOCATE_HEIGHT * loc_height;
	int x3 = image_x1+world.zoom_matrix->calc_cur_x(
		world.zoom_matrix->get_base_x(),
		world.zoom_matrix->get_base_y() + world.zoom_matrix->image_height)
		/ LOCATE_WIDTH * loc_width;
	int y3 = image_y1+world.zoom_matrix->calc_cur_y(
		world.zoom_matrix->get_base_x(),
		world.zoom_matrix->get_base_y() + world.zoom_matrix->image_height)
		/ LOCATE_HEIGHT * loc_height;
*/

	int x0, y0, x1, y1, x2, y2, x3, y3;

	if( !cur_cargo_rect_type )
	{
		x0 = image_x1 + calc_loc_pos_x( cur_x_loc, cur_y_loc );
		y0 = image_y1 + calc_loc_pos_y( cur_x_loc, cur_y_loc );
		x1 = image_x1 + calc_loc_pos_x( cur_x_loc + cur_cargo_width, cur_y_loc );
		y1 = image_y1 + calc_loc_pos_y( cur_x_loc + cur_cargo_width, cur_y_loc );
		x2 = image_x1 + calc_loc_pos_x( cur_x_loc + cur_cargo_width, cur_y_loc + cur_cargo_height );
		y2 = image_x1 + calc_loc_pos_y( cur_x_loc + cur_cargo_width, cur_y_loc + cur_cargo_height );
		x3 = image_x1 + calc_loc_pos_x( cur_x_loc, cur_y_loc + cur_cargo_height );
		y3 = image_y1 + calc_loc_pos_y( cur_x_loc, cur_y_loc + cur_cargo_height );
	}
	else
	{
		// ######### begin Gilbert 6/3 #########//
		int xLoc0 = cur_x_loc;
		int yLoc0 = cur_y_loc;
		int xLoc1 = cur_x_loc+get_x_loc(cur_cargo_width,0);
		int yLoc1 = cur_y_loc+get_y_loc(cur_cargo_width,0);
		int xLoc2 = cur_x_loc+get_x_loc(cur_cargo_width,cur_cargo_height);
		int yLoc2 = cur_y_loc+get_y_loc(cur_cargo_width,cur_cargo_height);
		int xLoc3 = cur_x_loc+get_x_loc(0,cur_cargo_height);
		int yLoc3 = cur_y_loc+get_y_loc(0,cur_cargo_height);

		// get height to shift
		int avgZ = 0;
		int countZ = 0;
		if( xLoc0 >= 0 && xLoc0 < max_x_loc && yLoc0 >= 0 && yLoc0 < max_y_loc )
		{
			avgZ += get_corner(xLoc0, yLoc0)->get_altitude();
			countZ++;
		}
		if( xLoc1 >= 1 && xLoc1 < max_x_loc && yLoc1 >= 1 && yLoc1 < max_y_loc )
		{
			avgZ += get_corner(xLoc1, yLoc1)->get_altitude();
			countZ++;
		}
		if( xLoc2 >= 2 && xLoc2 < max_x_loc && yLoc2 >= 2 && yLoc2 < max_y_loc )
		{
			avgZ += get_corner(xLoc2, yLoc2)->get_altitude();
			countZ++;
		}
		if( xLoc3 >= 3 && xLoc3 < max_x_loc && yLoc3 >= 3 && yLoc3 < max_y_loc )
		{
			avgZ += get_corner(xLoc3, yLoc3)->get_altitude();
			countZ++;
		}

		if( countZ > 0 )
			avgZ /= countZ;

		int yShift = avgZ / (ZOOM_LOC_X_HEIGHT + ZOOM_LOC_Y_HEIGHT);

		x0 = image_x1 + calc_loc_pos_x( xLoc0, yLoc0 );
		y0 = image_y1 + calc_loc_pos_y( xLoc0, yLoc0 ) + yShift;
		x1 = image_x1 + calc_loc_pos_x( xLoc1, yLoc1 );
		y1 = image_y1 + calc_loc_pos_y( xLoc1, yLoc1 ) + yShift;
		x2 = image_x1 + calc_loc_pos_x( xLoc2, yLoc2 );
		y2 = image_y1 + calc_loc_pos_y( xLoc2, yLoc2 ) + yShift;
		x3 = image_x1 + calc_loc_pos_x( xLoc3, yLoc3 );
		y3 = image_y1 + calc_loc_pos_y( xLoc3, yLoc3 ) + yShift;
		// ######### end Gilbert 6/3 #########//
	}

	// backup anim_line boundary
	short   boundBackX1 = anim_line.bound_x1;
	short	boundBackY1 = anim_line.bound_y1;
	short	boundBackX2 = anim_line.bound_x2;
	short	boundBackY2 = anim_line.bound_y2;
	short limitx;
	short limity;
	
	// #### begin Ban 8/12 #######//
	//
	// this part sets boundary of the four anim_lines for the new interface
	// however, in order to simplify the code, the following code is case dependence
	// in this case the matrix map must be 200 X 200 and the upper triangle must with
	// height = 100, width = 200 and the lower triangle must with height = 99 and width
	// = 198
	//
	// vga_back.rect( x1, y1, x2, y2, 1, VGA_YELLOW + squareFrameCount );
	if ((y1 < image_y1) || (y1 > (image_y2 -1)))
		limitx = -1;
	else
	if ((y1 >= image_y1) && (y1 < (image_y1 +100)))
		limitx = y1 -image_y1;
	else
	if ((y1 <= (image_y2 -1)) && (y1 > (image_y2 -100)))
		limitx = image_y2 -1 -y1;
	else err_here();
	anim_line.bound_x1 = (image_x2 + image_x1)/2 -limitx;
	anim_line.bound_y1 = image_y1;
	anim_line.bound_x2 = (image_x2 + image_x1 +2)/2 +limitx;
	anim_line.bound_y2 = image_y2-1;
	anim_line.draw_line( &vga_back, x0, y0, x1, y1, 1, -1 );
	// #### printing first line #### //
	
	if ((x1 < image_x1) || (x1 > (image_x2 -1)))
		limity = -1;
	else
	if ((x1 >= image_x1) && (x1 < (image_x1+100)))
		limity = x1 -image_x1;
	else
	if ((x1 <= (image_x2 -1)) && (x1 > (image_x2-101)))
		limity = image_x2 -1 -x1;
	else err_here();
	anim_line.bound_x1 = image_x1;
	anim_line.bound_y1 = (image_y2 -1 + image_y1)/2 -limity;
	anim_line.bound_x2 = image_x2 -1;
	anim_line.bound_y2 = (image_y2 -1 + image_y1)/2 +limity;
	anim_line.draw_line( &vga_back, x1, y1, x2, y2, 1, -1 );
	// #### printing second line #### //

	if ((y2 < image_y1) || (y2 > (image_y2 -1)))
		limitx = -1;
	else
	if ((y2 >= image_y1) && (y2 < (image_y1 +100)))
		limitx = y2 -image_y1;
	else
	if ((y2 <= (image_y2 -1)) && (y2 > (image_y2 -100)))
		limitx = image_y2 -1 -y2;
	else err_here();
	anim_line.bound_x1 = (image_x2 + image_x1)/2 -limitx;
	anim_line.bound_y1 = image_y1;
	anim_line.bound_x2 = (image_x2 + image_x1 +2)/2 +limitx;
	anim_line.bound_y2 = image_y2 -1;
	anim_line.draw_line( &vga_back, x2, y2, x3, y3, 1, -1 );
	// #### printing third line #### //

	if ((x3 < image_x1) || (x3 > (image_x2 -1)))
		limity = -1;
	else
	if ((x3 >= image_x1) && (x3 < (image_x1+100)))
		limity = x3 -image_x1;
	else
	if ((x3 <= (image_x2 -1)) && (x3 > (image_x2-101)))
		limity = image_x2 -1 -x3;
	else err_here();
	anim_line.bound_x1 = image_x1;
	anim_line.bound_y1 = (image_y2 -1 + image_y1)/2 -limity;
	anim_line.bound_x2 = image_x2 -1;
	anim_line.bound_y2 = (image_y2 -1 + image_y1)/2 +limity;
	anim_line.draw_line( &vga_back, x3, y3, x0, y0, 1, -1 );
	// #### printing last line #### //
	// #### end Ban 8/12 #######//

	// restore anim_line boundary
	anim_line.bound_x1 = boundBackX1;
	anim_line.bound_y1 = boundBackY1;
	anim_line.bound_x2 = boundBackX2;
	anim_line.bound_y2 = boundBackY2;

	//squareFrameCount += squareFrameStep;

	//if( squareFrameCount==0 )  // color with smaller number is brighter
	//	squareFrameStep  = 1;

	//if( squareFrameCount==6 ) // bi-directional color shift
	//	squareFrameStep  = -1;
}
//----------- End of function MapMatrix::draw_square ------------//


//-------- Begin of function MapMatrix::toggle_map_mode ------------//

void MapMatrix::toggle_map_mode(int modeId)
{
/*
	if( map_mode == modeId )
	{
		if( map_mode == MAP_MODE_POWER )		// clicking on a pressed button unclick the button
			power_mode = !power_mode;
	}
	else
	{
		map_mode = modeId;
	}
*/
	map_mode = modeId;

	disp_mode_button(1);		// 1-display the buttons on the front buffer.
#ifdef USE_FLIP
	disp_mode_button(0);		// 0-display the buttons on the back buffer.
#endif

	refresh();
}
//---------- End of function MapMatrix::toggle_map_mode ------------//


//----------- Begin of function MapMatrix::detect_area -----------//
//
// Detect for click on a new zoom area, update the zoom window as well
//
// return : 0 - no action
//          1 - pressed on new cargo
//          2 - pressed on new cargo and also scrolled window
//
int MapMatrix::detect_area()
{
//	if( !mouse.press_area( image_x1,image_y1,image_x2,image_y2, 2 ) &&
//		 !mouse.any_click( image_x1,image_y1,image_x2,image_y2, 2 ) )
//	{
//		return 0;
//	}

	int rc = 0;

	int lastXLoc = cur_x_loc;
	int lastYLoc = cur_y_loc;

	//--- if press left button, select zoom area ----//

	// ##### begin Gilbert 30/3 #######//
	if( mouse.any_click( image_x1,image_y1,image_x2,image_y2 ) ||
		 mouse.press_area( image_x1,image_y1,image_x2,image_y2, LEFT_BUTTON ) )
	// ##### end Gilbert 30/3 #######//
	{
		int xLoc;
		int yLoc;
		if( get_detect_location( mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
		{
			// ##### begin Gilbert 30/3 ########//
			mouse.set_boundary( image_x1, image_y1, image_x2, image_y2, 1 );
			// reset bound in power, probably...
			// ##### end Gilbert 30/3 ########//

			//-- if only single click, don't highlight new firm, only new area --//

			if( !cur_cargo_rect_type )
			{
				cur_x_loc = xLoc - cur_cargo_width/2;
				cur_y_loc = yLoc - cur_cargo_width/2;
			}
			else
			{
				cur_x_loc = xLoc - get_x_loc( cur_cargo_width/2, cur_cargo_height/2 );
				cur_y_loc = yLoc - get_y_loc( cur_cargo_width/2, cur_cargo_height/2 );
			}

			if( !valid_cur_box() )    // valid_cur_box() return 1 if it has refreshed, 0 if didn't
				disp();

			rc=1;
		}
	}

	// ----- reset boundary -------//

	if( mouse.release_click() 
		&& mouse.click_x() >= image_x1 && mouse.click_x() <= image_x2
		&& mouse.click_y() >= image_y1 && mouse.click_y() <= image_y2 )
	{
		// release anywhere, but click in the rectangle
		mouse.reset_boundary();
	}

	//------- if the view area has been changed -------//

	if( cur_x_loc != lastXLoc || cur_y_loc != lastYLoc )
	{
		world.zoom_matrix->top_x_loc = cur_x_loc;
		world.zoom_matrix->top_y_loc = cur_y_loc;

		sys.zoom_need_redraw = 1;
	}

	return rc;
}
//------------ End of function MapMatrix::detect_area ----------//


//----------- Begin of function MapMatrix::get_detect_location ----------//
//
// find the location of the given screen coordinate
// return 1 if success
//
int MapMatrix::get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{
	return Matrix::get_detect_location(scrnX, scrnY, locX, locY, mobileType);
	/*
	if( scrnX >= MAP_X1 && scrnX <= MAP_X2
		&& scrnY >= MAP_Y1 && scrnY <= MAP_Y2)
	{
		if( scrnX >= image_x1 && scrnX <= image_x2
			&& scrnY >= image_y1 && scrnY <= image_y2)
		{
			*locX = scrnX - image_x1;
			err_when(*locX < 0 || *locX >= max_x_loc );

			*locY = scrnY - image_y1;
			err_when(*locY < 0 || *locY >= max_y_loc );

			*mobileType = 0;
			return 1;
		}
	}

	return 0;
	*/
}
//----------- End of function MapMatrix::get_detect_location ----------//



//----------- Begin of function MapMatrix::get_detect_location ----------//
//
// find the location of the given screen coordinate
// it assumes the point is outside the map but inside the map window
// return 1 if success
//
int MapMatrix::get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{
	return 0;
}
//----------- End of function MapMatrix::get_detect_location ----------//
