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

// Filename    : OF_FORTI.CPP
// Description : Fort Interface

#include <ONATION.H>
#include <OF_FORT.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OBITMAP.H>
#include <OFONT.H>
#include <OUNITRES.H>
#include <OIMGRES.H>
#include <OINFO.H>
#include <OBUTT3D.H>
#include <OBUTTCUS.H>
#include <ORACERES.H>
#include <OTOWN.H>
#include <OHELP.H>
#include <OSE.H>
#include <OSERES.H>

// ------- define constant -------//

#define TOWER_COUNT_BUTTON_WIDTH 30
#define TOWER_COUNT_BUTTON_HEIGHT 30

#define SOLDIER_PER_ROW      8
#define SOLDIER_X_SPACING   24
#define SOLDIER_Y_SPACING   86

// ------- declare static variable --------//

static Button3D button_train;
static ButtonCustom button_tower_count;

// ------- declare static function -------//

static void disp_tower_count(ButtonCustom *, int repaintBody);


//--------- Begin of function FirmCamp::disp_camp_info ---------//
//
void FirmFort::disp_camp_info(int dispY1, int refreshFlag)
{
	FirmCamp::disp_camp_info(dispY1, refreshFlag);

	// ----- display train button -------//

	if( refreshFlag == INFO_REPAINT )
	{
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		if (!is_monster())
			button_train.create( INFO_X1+13, INFO_Y1+281, 'A', "TRAIN" );
		else
			button_train.create( INFO_X1+13, INFO_Y1+281, 'A', "F_TRAIN" );
	}

	// scan any available population to train

	// ##### begin Gilbert 23/12 #######//
	if( (button_train.visible_flag = is_own()) )
	{
		button_train.enable_flag = can_recruit(active_link_town_recno);
		button_train.paint();
	}
	// ##### end Gilbert 23/12 #######//

	// ------- display tower count -----//

	if( refreshFlag == INFO_REPAINT )
	{
		button_tower_count.create( INFO_X1+13+3*BUTTON_DISTANCE+6, INFO_Y1+235+6,
			INFO_X1+13+3*BUTTON_DISTANCE+6+TOWER_COUNT_BUTTON_WIDTH-1,
			INFO_Y1+235+6+TOWER_COUNT_BUTTON_HEIGHT-1,
			disp_tower_count, ButtonCustomPara( NULL, 0 ) );

//#ifdef DEBUG
		// BUGHERE : ARM_TOWERS is longer than 8 characters
		// button_tower_count.set_help_code("ARM_TOWERS");
		button_tower_count.set_help_code("ARM_TOWE");
//#endif
	}

	button_tower_count.enable_flag = is_own();
	button_tower_count.custom_para = ButtonCustomPara( NULL, target_archer_count );
	button_tower_count.paint();

	// ------- display towers ----------//

	err_when( !image_spict.read_all );			// must read_all
	char *towerFull = image_spict.read("TOWER-F");
	char *towerEmpty = image_spict.read("TOWER-E");
	char *towerBuilding = image_spict.read("TOWER-D");
	int towerHeight = ((Bitmap *)towerFull)->get_height();
	err_when( ((Bitmap *)towerEmpty)->get_height() != towerHeight );
	err_when( ((Bitmap *)towerBuilding)->get_height() != towerHeight );
	int towerWidth = ((Bitmap *)towerFull)->get_width();

//	static int towerXTable[MAX_FORT_ARCHER] = { 180, 202, 191, 180, 202 };
//	static int towerYTable[MAX_FORT_ARCHER] = { 280, 280, 292, 304, 304 };
	static int towerXTable[MAX_FORT_ARCHER] = { 180, 202, 191, 180, 202 };
	static int towerYTable[MAX_FORT_ARCHER] = { 304, 304, 292, 280, 280 };

	for( int i = 0; i < MAX_FORT_ARCHER; ++i )
	{
		int towerX = INFO_X1 + towerXTable[i];
		int towerY = INFO_Y1 + towerYTable[i];

		if( i < current_archer_count )
		{
			// display full tower
			vga.active_buf->put_bitmap( towerX, towerY, towerFull );
		}
		else if( i == current_archer_count )
		{
			// display building tower
			vga.active_buf->put_bitmap( towerX, towerY, towerEmpty );

			// display portion of towerFull at bottom
			int subTowerHeight = train_archer_progress * towerHeight / MAX_TRAIN_ARCHER_PROGRESS;
			if( subTowerHeight > 0 )
			{
				if( vga.active_buf->is_front )
					mouse.hide_area( towerX, towerY, towerX+towerWidth-1, towerY+towerHeight-1 );
				vga.active_buf->put_bitmap_area( towerX, towerY, towerBuilding, 
					0, towerHeight-subTowerHeight, towerWidth-1, towerHeight-1 );
				if( vga.active_buf->is_front )
					mouse.show_area();
			}
		}
		else
		{
			// display disabled tower	
			vga.active_buf->put_bitmap( towerX, towerY, towerEmpty );
		}
	}

	help.set_help( INFO_X1+176, INFO_Y1+278, INFO_X1+176+39, INFO_Y1+278+48, "TOWERS" );
}
//----------- End of function FirmCamp::disp_camp_info -----------//


//--------- Begin of function FirmCamp::detect_camp_info ---------//
//
void FirmFort::detect_camp_info()
{
	FirmCamp::detect_camp_info();

	int rc = button_train.detect(0, 0, 1);
	if( rc )
	{
		if( !town_array.is_deleted(active_link_town_recno) 
			&& town_array[active_link_town_recno]->recruitable_pop(1) > 0)
		{
			recruit_soldier( active_link_town_recno, 0, COMMAND_PLAYER );
		}
		else
		{
			short townRecnoArray[MAX_RACE];
			int raceCount = scan_best_recruitable_town(townRecnoArray);
			if( rc == 1 && raceCount == 1 )	// only one race, recruit from that town
			{
				err_when( !townRecnoArray[0] );
				recruit_soldier( townRecnoArray[0], 0, COMMAND_PLAYER );
			}
			else if( rc == 2 || rc == 1 && raceCount > 1 )
			{
				firm_menu_mode = FIRM_MENU_TRAIN;
			}
		}
	//	se_ctrl.immediate_sound("TURN_ON");
		se_ctrl.immediate_sound("ARM-ALRT");
	}

	switch( button_tower_count.detect(0, 0, 1) )
	{
		case 1:		// left click increase
		{
			set_target_archer( -1, COMMAND_PLAYER );
			se_ctrl.immediate_sound("ARM-ALRT");
			break;
		}
		case 2:		// right click decrease
		{
			set_target_archer( -2, COMMAND_PLAYER );
			se_ctrl.immediate_sound("TURN_OFF");
			break;
		}
	}

}
//----------- End of function FirmCamp::detect_camp_info -----------//


//--------- Begin of function FirmCamp::disp_train_menu ---------//
//
void FirmFort::disp_train_menu(int refreshFlag)
{
	short townRecnoArray[MAX_RACE];
	int raceCount = scan_best_recruitable_town( townRecnoArray );

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;
		for( int i = inc>=0?0:raceCount-1; i>=0 && i<raceCount; i+=inc )
		{
			Town *townPtr = town_array[townRecnoArray[i]];
			int raceId = townPtr->race_id;

			// display soldier i

			int row = (raceId-1)/SOLDIER_PER_ROW;
			int x = INFO_X1 + 18 + ((raceId-1) % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
			int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
			int yLoyalty = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 220;
			int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
			int windowY2 = windowY1 + 80 - 1 ;

			int unitId = race_res[raceId]->infantry_unit_id;

			err_when( townPtr->nation_recno != nation_recno );

			if( inc < 0 )
			{
				// first round is descending draw to icon

				info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, townPtr->nation_recno,
					windowX1, windowY1, windowX2, windowY2, 1 );
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, townPtr->nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_soldier_id?3:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				// display loyalty

				font_whbl.center_put( x, yLoyalty, x+SOLDIER_X_SPACING, yLoyalty+font_snds.max_font_height,
					m.format((int) townPtr->loyalty) );
			}
		}
	}
}
//--------- End of function FirmCamp::disp_train_menu ---------//


//--------- Begin of function FirmCamp::detect_train_menu ---------//
//
void FirmFort::detect_train_menu()
{
	short townRecnoArray[MAX_RACE];
	int raceCount = scan_best_recruitable_town( townRecnoArray );

	if( mouse.any_click(1) )		// any right click
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		return;
	}

	// display in ascending order to make the overseer on top

	for( int i = 0; i < raceCount; ++i )
	{
		Town *townPtr = town_array[townRecnoArray[i]];
		int raceId = townPtr->race_id;

		// display soldier i

		int row = (raceId-1)/SOLDIER_PER_ROW;
		int x = INFO_X1 + 18 + ((raceId-1) % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
		int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
		int yLoyalty = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId = race_res[raceId]->infantry_unit_id;

		err_when( townPtr->nation_recno != nation_recno );

		if( info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
			unitId, townPtr->nation_recno,
			windowX1, windowY1, windowX2, windowY2, 8 ) & 8 )
		{
			recruit_soldier( townRecnoArray[i], 0, COMMAND_PLAYER );
			break;
		}
	}
}
//--------- End of function FirmCamp::detect_train_menu ---------//


//--------- Begin of static function disp_tower_count ---------//
//
static void disp_tower_count(ButtonCustom *button, int repaintBody)
{
	vga.active_buf->put_bitmap( button->x1, button->y1, image_button.read(
		button->pushed_flag ? "NUM-D" : "NUM-U" ) );

	font_bible.center_put( button->x1, button->y1, button->x2, button->y2, 
		button->custom_para.ptr ? (char *)button->custom_para.ptr : m.format(button->custom_para.value) );
}
//--------- End of static function disp_tower_count ---------//
