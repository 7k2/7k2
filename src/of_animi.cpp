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

// Filename     : OF_ANIMI.CPP
// Descriptiion : Animal Habitat interface functions


#include <of_anim.h>
#include <all.h>
#include <osys.h>
#include <oinfo.h>
#include <obox.h>
#include <ovga.h>
#include <omodeid.h>
#include <ostr.h>
#include <oimgres.h>
#include <ofont.h>
#include <omouse.h>
#include <ohelp.h>
#include <obutt3d.h>
#include <oseres.h>
#include <oconfig.h>
#include <ogame.h>
#include <oremote.h>
#include <osprtres.h>
#include <osfrmres.h>
#include <oraceres.h>
#include <oherores.h>
#include <oworldmt.h>
#include <onation.h>
#include <ospy.h>
#include <oworld.h>
#include <ose.h>
#include <ot_firm.h>
#include <ot_unit.h>

// ----------- Define constant --------//

// #### begin Gilbert 8/1 #####//
#define UNIT_X_SPACING     38
// #define UNIT_Y_SPACING     76
// #define ANIMAL_PER_ROW      6
// #### end Gilbert 8/1 #####//

//----------- Define static variables ----------//

static Button3D	button_hire;

static int disp_unit_list_y1;

// -------- begin of function FirmAnimal::put_info --------//
//
void FirmAnimal::put_info(int refreshFlag)
{
	// ------- reset selected_unit_id ------//

	if( refreshFlag == INFO_REPAINT )
	{
		selected_unit_id = 0;
	}
	else
	{
		if( selected_unit_id < 0 )
			selected_unit_id = 0;
		if( selected_unit_id > animal_unit_count )
			selected_unit_id = animal_unit_count;
	}

	Firm::put_info(refreshFlag);

//	disp_basic_info(INFO_Y1, refreshFlag);

	if( refreshFlag == INFO_REPAINT )
	{
		button_hire.create(INFO_X1+13, INFO_Y1+235, 'A', "F_ANIMAL" );
		button_hire.enable_flag = 0;
	}

	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("ANIMBASE") );

	disp_unit_list( INFO_Y1, refreshFlag );

	disp_unit_info( INFO_Y1 + 165, refreshFlag );

	// ------ main interface --------//

	// ------ display hire button -------//

	button_hire.enable_flag = 0;

	if( !is_own() )
		return;

	if( selected_unit_id > 0 && selected_unit_id <= animal_unit_count )
	{
		AnimalUnit *animalUnit = animal_unit_array + selected_unit_id - 1;

		button_hire.enable_flag = can_hire(selected_unit_id);
	}

	button_hire.paint();
}
// -------- end of function FirmAnimal::put_info --------//


// -------- begin of function FirmAnimal::detect_info --------//
//
void FirmAnimal::detect_info()
{
//	firm_animal_ptr = this;

	Firm::detect_info();

//	if( detect_basic_info() )
//		return;

	if( !should_show_info() )
		return;

	detect_unit_list();

	if( !is_own() )
		return;

	// ------ detect hire button -------//

	if( button_hire.detect() 
		&& selected_unit_id > 0 && selected_unit_id <= animal_unit_count )
	{
		AnimalUnit *animalUnit = animal_unit_array + selected_unit_id - 1;
		int temp;

		se_ctrl.immediate_sound("TAXGRANT");

		se_res.far_sound(center_x, center_y, 1, 'S', 
			unit_res[animalUnit->unit_id]->sprite_id,
			"RDY" );
		if(remote.is_enable())
		{
			// packet structure : <firm recno>, <hire Id> <nation no> <spy escape>
			short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_INN_HIRE, 4*sizeof(short));
			shortPtr[0] = firm_recno;
			shortPtr[1] = selected_unit_id;
			shortPtr[2] = nation_recno;
			// ###### begin Gilbert 13/2 #####//
			shortPtr[3] = 0;
			// ###### end Gilbert 13/2 #####//
		}
		else
		{
			temp = selected_unit_id;
			hire(selected_unit_id);
		}

		// ------ clear selected_unit_id ------//

		if (temp > animal_unit_count)
			selected_unit_id = animal_unit_count;
		else
			selected_unit_id = temp;
	}

	if (selected_unit_id <= 0 && animal_unit_count > 0)
		selected_unit_id = animal_unit_count;
}
// -------- end of function FirmAnimal::detect_info --------//


// -------- begin of function FirmAnimal::disp_unit_list --------//
//
void FirmAnimal::disp_unit_list(int dispY1, int refreshFlag)
{
	disp_unit_list_y1 = dispY1;

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;

		for( int i = inc>=0?1:animal_unit_count; i > 0 && i <= animal_unit_count; i +=inc )
		{
			AnimalUnit *animalUnit = animal_unit_array + i - 1;

			// #### begin Gilbert 8/1 ######//
			// int row = (i-1) / ANIMAL_PER_ROW;
			// int x = INFO_X1 + 67 + (i-1) % ANIMAL_PER_ROW * UNIT_X_SPACING;
			// int y = INFO_Y1 + 44 + row * UNIT_Y_SPACING;
			int x = INFO_X1 + 22 + (i-1) * UNIT_X_SPACING;
			int y = INFO_Y1 + 120;
			int yHp = y - 42;

			int windowX1 = INFO_X1;
			int windowX2 = INFO_X2;
			int windowY1 = INFO_Y1;
			int windowY2 = INFO_Y2;
			// #### end Gilbert 8/1 #####//

			if( inc < 0 )
			{
				// first pass descending, draw unit icon

				info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
					animalUnit->unit_id, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 1 );
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
					animalUnit->unit_id, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_unit_id?2:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				int fontHeight = font_whbl.max_font_height;

				font_whbl.center_put( x, yHp, x+UNIT_X_SPACING, yHp+fontHeight, m.format(animalUnit->combat_level()) );
			}
		}
	}
}
// -------- end of function FirmAnimal::disp_unit_list --------//


// -------- begin of function FirmAnimal::detect_unit_list --------//
//
void FirmAnimal::detect_unit_list()
{
	int dispY1 = disp_unit_list_y1;

	// detect in ascending order 

	for( int i = 1; i <= animal_unit_count; ++i )
	{
		AnimalUnit *animalUnit = animal_unit_array + i - 1;

		// #### begin Gilbert 8/1 ######//
		// int row = (i-1) / ANIMAL_PER_ROW;
		// int x = INFO_X1 + 67 + (i-1) % ANIMAL_PER_ROW * UNIT_X_SPACING;
		// int y = INFO_Y1 + 44 + row * UNIT_Y_SPACING;
		int x = INFO_X1 + 22 + (i-1) * UNIT_X_SPACING;
		int y = INFO_Y1 + 120;
		int yHp = y - 42;

		int windowX1 = INFO_X1;
		int windowX2 = INFO_X2;
		int windowY1 = INFO_Y1;
		int windowY2 = INFO_Y2;
		// #### end Gilbert 8/1 #####//

		int rc = info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
				animalUnit->unit_id, nation_recno,
				windowX1, windowY1, windowX2, windowY2, 8 );		// detect left button

		if( rc & 8)
		{
			selected_unit_id = i;
			break;
		}
	}
}
// -------- end of function FirmAnimal::detect_unit_list --------//


// -------- begin of function FirmAnimal::disp_unit_info --------//
//
void FirmAnimal::disp_unit_info(int dispY1, int refreshFlag)
{
	if( !selected_unit_id || selected_unit_id > animal_unit_count)
		return;

	// #### begin Gilbert 8/1 ######//
	// int x = INFO_X1 + 83;
	int x = INFO_X1 + 20;
	// #### end Gilbert 8/1 ######//
	int y = dispY1;
	int x2;

	err_when( selected_unit_id <= 0 || selected_unit_id > animal_unit_count );

	AnimalUnit *animalUnit = animal_unit_array + selected_unit_id - 1;

	// display unit name

	font_snds.put( x, y, unit_res[animalUnit->unit_id]->name, 0, -1, 1 );

	y += 14;

	// display combat level

	{
		x2 = font_snds.put( x, y, text_unit.str_combat_level() ) + 10;
		// font_snds.put( x2, y, animalUnit->combat_level() );
		font_snds.right_put( x+90, y, m.format(animalUnit->combat_level(),4) );
	}

	y += 14;

	// display leadership skill

	if( animalUnit->skill_level() > 0 )
	{
		x2 = font_snds.put( x, y, text_unit.str_leadership() ) + 10;
		// font_snds.put( x2, y, animalUnit->skill_level() );
		font_snds.right_put( x+90, y, m.format(animalUnit->skill_level(),4) );
	}

	y += 14;

	// display hire cost

//	String str;
//	str = "Hiring Cost : ";
//	str += m.format(animalUnit->hire_cost, 2);
//	str += " + ";
//	str += animalUnit->live_point_cost;
//	str += " live points";

	font_snds.put( x, y, text_firm.str_hire_animal_cost(animalUnit->hire_cost, animalUnit->live_point_cost),
		0, INFO_X2-6 );
	
	// BUGHERE : display unit icon

//	info.draw_unit_icon( INFO_X1+38, INFO_Y1+228-32,
//		animalUnit->unit_id, nation_recno,
//		INFO_X1+14, INFO_Y1+157, INFO_X1+63, INFO_Y1+228, 1 );
}
// -------- end of function FirmAnimal::disp_unit_info --------//


// -------- begin of function FirmAnimal::draw --------//
//
void FirmAnimal::draw(int displayLayer)
{
	Firm::draw(displayLayer);

	// ------- draw animal ---------//

//	const unitIconCount = 5;
	int iconOffsetX[MAX_DISPLAY_SLOT*2] = { 90, 138, 90, 138,  42,
													  10, 50, 10, 50, 10};
	int iconOffsetY[MAX_DISPLAY_SLOT*2] = { 90, 90, 138, 138, 138,
													  0, 0, 30, 30, 60};

	const int dirId = 3;

	if( displayLayer & 1 )
	{

/*		// display one icon for each type of animal

		char animalCount[ANIMAL_TYPE_COUNT];
		memset(animalCount, 0, sizeof(animalCount) );

		int i, j;
		for( j = 0; j < animal_unit_count; ++j )
		{
			for( i = 0; i < ANIMAL_TYPE_COUNT && animal_unit_array[j].unit_id != animal_unit_id[i]; ++i );
			if( i < ANIMAL_TYPE_COUNT )
				animalCount[i]++;
		}

		// slot 1,2,3 display each animal

		for( i = 0; i < min(ANIMAL_TYPE_COUNT,unitIconCount) ; ++i )
		{
			if( animalCount[i] > 0 )
			{
				int spriteId = unit_res[animal_unit_id[i]]->sprite_id;
				SpriteInfo *spriteInfo = sprite_res[spriteId];
				const int dirId = 3;
				int spriteFrameRecno = spriteInfo->stop_array[dirId].frame_recno;
				int frameCount = spriteInfo->stop_array[dirId].frame_count;

				if( spriteFrameRecno )
				{
					spriteFrameRecno += (sys.frame_count + firm_recno) % frameCount;	// animation

					SpriteFrame *spriteFrame = sprite_frame_res[spriteFrameRecno];
					char* bitmapPtr = spriteInfo->res_bitmap.read_imported(spriteFrame->bitmap_offset);
					short* colorRemapTable = spriteInfo->get_color_remap_table( 0, 0);

					world.zoom_matrix->put_bitmap_offset( 
						loc_x1 * LOCATE_WIDTH + iconOffsetX[i], 
						loc_y1 * LOCATE_HEIGHT + iconOffsetY[i],
						altitude, bitmapPtr,
						spriteFrame->offset_x, spriteFrame->offset_y, 
						colorRemapTable, 0, 1 );
				}
			}
		}
*/

		// sort by slot id

		int i, j;

		char displayAnimalId[MAX_DISPLAY_SLOT];
		memset( displayAnimalId, 0, sizeof(displayAnimalId) );
		for( i = 0; i < animal_unit_count; ++i )
		{
			short slotId = animal_unit_array[i].display_slot_id;
			if( slotId >= 0 && slotId < MAX_DISPLAY_SLOT )
				displayAnimalId[slotId] = i+1;
		}

		// display one icon for one animal

		for( j = 0; j < MAX_DISPLAY_SLOT; ++j )
		{
			i = displayAnimalId[j] - 1;
			if( i < 0 )
				continue;

			AnimalUnit *animalPtr = animal_unit_array + i;

			err_when( j != animalPtr->display_slot_id );
			if( animalPtr->display_slot_id < 0 || animalPtr->display_slot_id >= MAX_DISPLAY_SLOT )
				continue;

			int spriteId = unit_res[animalPtr->unit_id]->sprite_id;
			SpriteInfo *spriteInfo = sprite_res[spriteId];
			int spriteFrameRecno = spriteInfo->stop_array[dirId].frame_recno;
			int frameCount = spriteInfo->stop_array[dirId].frame_count;

			if( spriteFrameRecno )
			{
				spriteFrameRecno += (sys.frame_count + firm_recno) % frameCount;	// animation

				SpriteFrame *spriteFrame = sprite_frame_res[spriteFrameRecno];
				char* bitmapPtr = spriteInfo->res_bitmap.read_imported(spriteFrame->bitmap_offset);
				short* colorRemapTable = spriteInfo->get_color_remap_table( 0, 0);

				if (config.building_size == 1)	
					world.zoom_matrix->put_bitmap_offset( 
						loc_x1 * LOCATE_WIDTH + iconOffsetX[animalPtr->display_slot_id], 
						loc_y1 * LOCATE_HEIGHT + iconOffsetY[animalPtr->display_slot_id],
						altitude, bitmapPtr,
						spriteFrame->offset_x, spriteFrame->offset_y, 
						colorRemapTable, 0, 1 );
				else
					world.zoom_matrix->put_bitmap_offset( 
						loc_x1 * LOCATE_WIDTH + iconOffsetX[animalPtr->display_slot_id + 5], 
						loc_y1 * LOCATE_HEIGHT + iconOffsetY[animalPtr->display_slot_id + 5],
						altitude, bitmapPtr,
						spriteFrame->offset_x, spriteFrame->offset_y, 
						colorRemapTable, 0, 1 );
			}
		}
	}
}
// -------- end of function FirmAnimal::draw --------//
