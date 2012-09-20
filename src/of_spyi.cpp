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

// Filename     :OF_SPYI.CPP
// Description  : FirmSpy interface


#include <of_spy.h>
#include <all.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <oinfo.h>
#include <oraceres.h>
#include <obutt3d.h>
#include <otown.h>
#include <ose.h>
#include <oseres.h>


// --------- define constant --------//

#define TRAINEE_X_SPACING   24

// --------- define static variable --------//

static Button3D button_train_civ;
static Button3D button_train_inf;
static Button3D button_train_spu;


// ------- begin of function FirmSpy::put_info --------//
//
void FirmSpy::put_info(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	FirmTrain::put_info(refreshFlag);
}
// ------- end of function FirmSpy::put_info --------//


// ------- begin of function FirmSpy::detect_info --------//
//
void FirmSpy::detect_info()
{
	FirmTrain::detect_info();
}
// ------- end of function FirmSpy::detect_info --------//


// ------- begin of function FirmSpy::disp_firm_info --------//
//
void FirmSpy::disp_firm_info(int dispY1, int refreshFlag )
{
	FirmTrain::disp_firm_info(dispY1, refreshFlag );

	// ------- display train button ------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_train_civ.create( INFO_X1+13, INFO_Y1+281, 'A', "TRAIN-C" );
		button_train_civ.enable_flag = 0;
		button_train_inf.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "TRAIN-I" );
		button_train_inf.enable_flag = 0;
		button_train_spu.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', "TRAIN-S" );
		button_train_spu.enable_flag = 0;
	}

	if( !should_show_info() )
		return;

	if( is_own() )
	{
		button_train_civ.enable_flag = can_recruit(active_link_town_recno, TRAIN_UNIT_CIVILIAN);
		button_train_inf.enable_flag = can_recruit(active_link_town_recno, TRAIN_UNIT_INFANTRY);
		button_train_spu.enable_flag = can_recruit(active_link_town_recno, TRAIN_UNIT_SPECIAL);

		button_train_civ.paint();
		button_train_inf.paint();
		button_train_spu.paint();
	}
}
// ------- end of function FirmSpy::disp_firm_info --------//


// ------- begin of function FirmSpy::detect_firm_info --------//
//
void FirmSpy::detect_firm_info()
{
	FirmTrain::detect_firm_info();

	if( !should_show_info() )
		return;

	// ------- detect train button -------//

	int rc;
	int requiredLinkedFirmId = 0;

	if( (rc = button_train_civ.detect(0, 0, 1)) )
		cur_train_type = 0;
	else if( (rc = button_train_inf.detect(0, 0, 1)) )
		cur_train_type = 1;
	else if( (rc = button_train_spu.detect(0, 0, 1)) )
		cur_train_type = 2;

	if( rc )
	{
		if( !town_array.is_deleted(active_link_town_recno) 
			&& town_array[active_link_town_recno]->recruitable_pop(1) > 0)
		{
			recruit_trainee( active_link_town_recno, cur_train_type, COMMAND_PLAYER );
			se_ctrl.immediate_sound("TURN_ON");
		}
		else
		{
			short townRecnoArray[MAX_RACE];
			int raceCount = scan_best_recruitable_town(townRecnoArray, 0, required_linked_firm[cur_train_type]);

			if( rc == 1 && raceCount == 1 )	// only one race, recruit from that town
			{
				err_when( !townRecnoArray[0] );
				recruit_trainee( townRecnoArray[0], cur_train_type, COMMAND_PLAYER );
			//	se_ctrl.immediate_sound("TURN_ON");
				se_ctrl.immediate_sound("ARM-ALRT");
			}
			else if( rc == 2 || rc == 1 && raceCount > 1 )
			{
				firm_menu_mode = FIRM_TRAIN_MENU_TRAIN;
				se_ctrl.immediate_sound("TURN_OFF");
			}
		}
	}
}
// ------- end of function FirmSpy::detect_firm_info --------//


// ------- begin of function FirmSpy::disp_train_menu ------//
//
void FirmSpy::disp_train_menu(int refreshFlag)
{
	FirmTrain::disp_train_menu( refreshFlag );
	/*
	short townRecnoArray[MAX_RACE];

	int requiredLinkedFirmId = 0;
	int raceCount = scan_best_recruitable_town( townRecnoArray, 0, required_linked_firm[cur_train_type] );

	// display in descending order to make the overseer on top

	for( int i = raceCount-1; i >=0; --i )
	{
		Town *townPtr = town_array[townRecnoArray[i]];
		int raceId = townPtr->race_id;

		// display trainee i

		int x = INFO_X1 + 18 + (i-1) * TRAINEE_X_SPACING;
		int y = INFO_Y1 + 136;
		int yLoyalty = INFO_Y1 + 93;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 89;
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		switch( cur_train_type )
		{
		case TRAIN_UNIT_CIVILIAN:
			race_res[raceId]->civilian_unit_id;
			break;
		case TRAIN_UNIT_INFANTRY:
			race_res[raceId]->infantry_unit_id;
			break;
		case TRAIN_UNIT_SPECIAL:
			race_res[raceId]->special_unit_id;
			break;
		default:
			err_here();
		}

		err_when( townPtr->nation_recno != nation_recno );

		info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
			unitId, townPtr->nation_recno,
			windowX1, windowY1, windowX2, windowY2, 2 );

		font_whbl.center_put( x, yLoyalty, x+TRAINEE_X_SPACING, yLoyalty+font_snds.max_font_height,
			misc.format((int) townPtr->loyalty) );
	}
	*/
}
// ------- end of function FirmSpy::disp_train_menu ------//


// ------- begin of function FirmSpy::detect_train_menu ------//
//
void FirmSpy::detect_train_menu()
{
	FirmTrain::detect_train_menu();

	/*
	short townRecnoArray[MAX_RACE];
	int raceCount = scan_best_recruitable_town( townRecnoArray, 0, required_linked_firm[cur_train_type] );

	if( mouse.any_click(RIGHT_BUTTON) )
	{
		firm_menu_mode = FIRM_TRAIN_MENU_MAIN;
		return;
	}

	// display in ascending order to make the overseer on top

	for( int i = 0; i < raceCount; ++i )
	{
		Town *townPtr = town_array[townRecnoArray[i]];
		int raceId = townPtr->race_id;

		// display trainee i

		int x = INFO_X1 + 18 + (i-1) * TRAINEE_X_SPACING;
		int y = INFO_Y1 + 136;
		int yLoyalty = INFO_Y1 + 93;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 89;
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		switch( cur_train_type )
		{
		case TRAIN_UNIT_CIVILIAN:
			race_res[raceId]->civilian_unit_id;
			break;
		case TRAIN_UNIT_INFANTRY:
			race_res[raceId]->infantry_unit_id;
			break;
		case TRAIN_UNIT_SPECIAL:
			race_res[raceId]->special_unit_id;
			break;
		default:
			err_here();
		}

		err_when( townPtr->nation_recno != nation_recno );

		if( info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
			unitId, townPtr->nation_recno,
			windowX1, windowY1, windowX2, windowY2, 4 ) == 1 )
		{
			recruit_trainee( townRecnoArray[i], cur_train_type, COMMAND_PLAYER );
			break;
		}
	}
	*/
}
// ------- end of function FirmSpy::detect_train_menu ------//


// ------- begin of function FirmSpy::disp_spy_menu ------//
//
void FirmSpy::disp_spy_menu(int refreshFlag)
{
	err_here();
}
// ------- end of function FirmSpy::disp_spy_menu ------//


// ------- begin of function FirmSpy::detect_spy_menu ------//
//
void FirmSpy::detect_spy_menu()
{
	err_here();
}
// ------- end of function FirmSpy::detect_spy_menu ------//


// ------- begin of function FirmSpy::represent_attribute --------//
//
int FirmSpy::represent_attribute(int traineeId)
{
	err_when( traineeId <= 0 || traineeId > trainee_count );

	return (int) trainee_array[traineeId-1].spy_skill;
}
// ------- end of function FirmSpy::represent_attribute --------//
