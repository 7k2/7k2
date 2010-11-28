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

// Filename    : OF_TRAII.CPP
// Description : FirmTrain Interfac

#include <of_train.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <oimgres.h>
#include <obutt3d.h>
#include <oremote.h>
#include <ounitres.h>
#include <oraceres.h>
#include <oconfig.h>
#include <oinfo.h>
#include <onation.h> 
#include <ospy.h>
#include <ose.h>
#include <oseres.h>
#include <ot_unit.h>

// --------- define constant --------//

#define TRAINEE_X_SPACING   24

// --------- define static variable ------//

static int disp_trainee_list_y1;
static int disp_trainee_info_y1;
int FirmTrain::cur_train_type;			// parameter unitPara to recruit_train, determine to train civilian spy or infantry spy
int FirmTrain::required_linked_firm[TRAIN_UNIT_TYPE_MAX] = { 0, FIRM_FORT, FIRM_SPECIAL };

// #### begin Gilbert 2/10 #####//
static Button3D	button_mobilize, button_spy_mobilize, button_patrol;
// #### end Gilbert 2/10 #####//
static int button_mobilize_visible, button_spy_mobilize_visible;
static char	disp_combat_or_skill;	// 0=normal, 1=combat, 2=skill, 3=spying, 4=loyalty
static Button3D button_go_rally, button_clear_rally;

static short edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, edit_loyalty_enable;
static short edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, edit_combat_enable;
static short edit_combat_pot_x1, edit_combat_pot_y1, edit_combat_pot_x2, edit_combat_pot_y2, edit_combat_pot_enable;
static short edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, edit_skill_enable;
static short edit_skill_pot_x1, edit_skill_pot_y1, edit_skill_pot_x2, edit_skill_pot_y2, edit_skill_pot_enable;
static short edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, edit_spy_enable;
static short edit_spy_pot_x1, edit_spy_pot_y1, edit_spy_pot_x2, edit_spy_pot_y2, edit_spy_pot_enable;


// ------- begin of function FirmTrain::put_info -------//
//
void FirmTrain::put_info(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		firm_menu_mode = FIRM_TRAIN_MENU_MAIN;
		disp_combat_or_skill = 0;
	}

	Firm::put_info(refreshFlag);

	switch( firm_menu_mode )
	{
	case FIRM_TRAIN_MENU_MAIN:
		vga.active_buf->put_bitmap(INFO_X1, INFO_Y1, image_gameif.read("STABBASE"));
		disp_firm_info(INFO_Y1, refreshFlag);
		disp_trainee_list(INFO_Y1, refreshFlag );

		if( selected_trainee_id > trainee_count )
			selected_trainee_id = trainee_count;

		if( selected_trainee_id )
			disp_trainee_info(INFO_Y1+178, refreshFlag );
		break;

	case FIRM_TRAIN_MENU_TRAIN:
		vga.active_buf->put_bitmap(INFO_X1, INFO_Y1, image_gameif.read("STABBASE"));
		disp_train_menu(refreshFlag);
		break;

	case FIRM_TRAIN_MENU_SPY:
		disp_spy_menu(refreshFlag);
		break;
	}
}
// ------- end of function FirmTrain::put_info -------//


// ------- begin of function FirmTrain::detect_info -------//
//
void FirmTrain::detect_info()
{
	switch( firm_menu_mode )
	{
	case FIRM_TRAIN_MENU_MAIN:
		detect_firm_info();
		detect_trainee_list();

		if( selected_trainee_id > trainee_count )
			selected_trainee_id = trainee_count;

		if( selected_trainee_id )
			detect_trainee_info();
		break;

	case FIRM_TRAIN_MENU_TRAIN:
		detect_train_menu();
		break;

	case FIRM_TRAIN_MENU_SPY:
		detect_spy_menu();
		break;
	}
}
// ------- end of function FirmTrain::put_info -------//



// ------- begin of function FirmTrain::disp_firm_info -------//
//
void FirmTrain::disp_firm_info(int dispY1, int refreshFlag )
{
	if( refreshFlag == INFO_REPAINT )
	{
		button_mobilize.create( INFO_X1+13, INFO_Y1+235, 'A', "MOBILIZE" );
		button_mobilize.enable_flag = 0;
		button_spy_mobilize.create( INFO_X1+13, INFO_Y1+235, 'A', "MOBILSPY" );
		button_spy_mobilize.enable_flag = 0;
		button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "PATROL" );
		button_patrol.enable_flag = 0;

		// ###### begin Gilbert 11/3 ########//
		button_go_rally.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+235, 'A', "RALLY-GO" );
		button_clear_rally.create( INFO_X1+13+BUTTON_DISTANCE*3, INFO_Y1+235, 'A', "RALLY-NO" );
		// ###### end Gilbert 11/3 ########//
	}

	button_mobilize_visible =  button_spy_mobilize_visible = 0;

	if( !should_show_info() )
		return;

	// ------ display mobilize button -------//

	button_mobilize.enable_flag = button_spy_mobilize.enable_flag = 0;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		Trainee *traineePtr = trainee_array + selected_trainee_id - 1;

		if( is_own() )
		{
			button_mobilize.enable_flag = !traineePtr->is_under_training;
			button_mobilize.paint();
			button_mobilize_visible = 1;
		}
		else
		{
			button_spy_mobilize.enable_flag = traineePtr->is_own_spy() && !traineePtr->is_under_training;
			button_spy_mobilize.paint();
			button_spy_mobilize_visible = 1;
		}
	}
	else
	{
		if( is_own() )
		{
			button_mobilize.paint();
			button_mobilize_visible = 1;
		}
		else
		{
			button_spy_mobilize.paint();
			button_spy_mobilize_visible = 1;
		}
	}

	// ----- display mobilize all button -------//

	if( is_own() )
	{
		int i;
		for( i = 0; i < trainee_count && trainee_array[i].is_under_training; ++i );
		button_patrol.enable_flag = i < trainee_count;
		button_patrol.paint();

		// ------- display rally points button ------//

		if( (button_go_rally.visible_flag = rally_enable_flag != 0) )
			button_go_rally.paint();

		if( (button_clear_rally.visible_flag = rally_enable_flag != 0) )
			button_clear_rally.paint();
	}
}
// ------- end of function FirmTrain::disp_firm_info -------//


// ------- begin of function FirmTrain::detect_firm_info -------//
//
void FirmTrain::detect_firm_info()
{
	if( !should_show_info() )
		return;

	// since both button_mobilize and button_spy_mobilize share the same place
	// if one button is enable, the other button detect() is not called at all to avoid incorrect help text

	if( button_mobilize_visible && button_mobilize.detect() 
		&& selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		// ####### begin Gilbert 26/1 ###//
		if( !trainee_array[selected_trainee_id].is_under_training )
			mobilize_trainee( selected_trainee_id, COMMAND_PLAYER );
		else
			cancel_train_trainee( selected_trainee_id, COMMAND_PLAYER );
		// ####### begin Gilbert 26/1 ###//
		se_ctrl.immediate_sound("TURN_ON");
	}
	// ##### begin Gilbert 2/11 ######//
	else if( button_patrol.detect() && trainee_count > 0 )
	// ##### end Gilbert 2/11 ######//
	{
		mobilize_trainee( -1, COMMAND_PLAYER );
	//	se_ctrl.immediate_sound("SORTIE");
		if (trainee_count > 0)
		{
			Trainee *traineePtr = &trainee_array[0];
			int unitId = traineePtr->unit_id;
			UnitInfo *unitInfo = unit_res[unitId];
			se_res.sound( center_x, center_y, 1, 'S', unitInfo->sprite_id, "ACK");
		}
	}
	else if( button_spy_mobilize_visible && button_spy_mobilize.detect()
		&& selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		err_when( spy_array.is_deleted(trainee_array[selected_trainee_id-1].spy_recno) );
		Spy *spyPtr = spy_array[trainee_array[selected_trainee_id-1].spy_recno];
		if( !remote.is_enable() )
		{
			if( spyPtr->mobilize_firm_spy() )
			{
				spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it
			}
		}
		else
		{
			// packet structure <spy recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_FIRM, sizeof(short) );
			*shortPtr = spyPtr->spy_recno;
		}
		se_ctrl.immediate_sound("TURN_ON");
	}

	// ------- display rally points button ------//

	if( button_go_rally.detect() )
	{
		world.go_loc( rally_dest_x, rally_dest_y, 0 );
		se_ctrl.immediate_sound("TURN_ON");
	}

	if( button_clear_rally.detect() )
	{
		clear_rally_point(COMMAND_PLAYER);
		se_ctrl.immediate_sound("TURN_OFF");
	}
}
// ------- end of function FirmTrain::detect_firm_info -------//


// ------- begin of function FirmTrain::disp_trainee_list -------//
//
void FirmTrain::disp_trainee_list(int dispY1, int refreshFlag )
{
	disp_trainee_list_y1 = dispY1;
	
	int dispSpyMenu = 0;

	// display in descending order to make the overseer on top

	int inAreaFlag = 4;

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		for( int i = inc>=0?1:trainee_count; i>0 && i<=trainee_count; i+=inc )
		{
			// display trainee i

			int x = INFO_X1 + 18 + (i-1) * TRAINEE_X_SPACING;
			int y = INFO_Y1 + 136;
			int yHp = INFO_Y1 + 93;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 220;
			int windowY1 = INFO_Y1 + 89;
			int windowY2 = windowY1 + 80 - 1 ;

			int unitId;
			int representAttribute;
			int ownSpy;

			// trainee
			Trainee *traineePtr = &trainee_array[i-1];
			unitId = traineePtr->unit_id;
			UnitInfo *unitInfo = unit_res[unitId];
			representAttribute = represent_attribute(i);
			ownSpy = traineePtr->is_own_spy() ? traineePtr->spy_recno : 0;

			if( dispSpyMenu && !ownSpy )			// skip displaying non-own spy
				continue;

			// display that solider icon at x+TRAINEE_X_SPACING/2, y
			// draw a frame if selected

			if( inc < 0 )
			{
				// first round is descending draw to icon

				// ####### begin Gilbert 18/11 ######//
//				info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
//					unitId, nation_recno, windowX1, windowY1, windowX2, windowY2, 
//					(traineePtr->combat_level() < 20) ? (((20 - traineePtr->combat_level()) <<6)+ 33) : 1);

				int darkness = BASIC_SKILL_TRAIN - representAttribute;
				if( darkness < 0 )
					darkness = 0;
				info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
					unitId, nation_recno, windowX1, windowY1, windowX2, windowY2, 
					(traineePtr->is_under_training) ? ((darkness<<6)+ 33) : 1);
				// ####### end Gilbert 18/11 ######//
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_trainee_id?3:0) ) & 4)
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				Font *font = &font_whbl;

				// display represent attribute

				if( !dispSpyMenu )
				{
					if( disp_combat_or_skill )
					{
						int attribute = -1;
						font = &font_blue;
						switch( disp_combat_or_skill )
						{
						case 1:
							if( unitInfo->class_info.has_combat_level )
								attribute = traineePtr->combat_level();
							break;
						case 2:
							if( unitInfo->class_info.has_skill_level && traineePtr->skill_level() > 0 )
								attribute = traineePtr->skill_level();
							break;
						case 3:
							attribute = (int) traineePtr->spy_skill;
							break;
						case 4: 
							if( unitInfo->class_info.has_loyalty )
								attribute = traineePtr->loyalty;
							break;
						default: err_here(); break;
						}
						if( attribute >= 0 )
						{
							font->center_put( x, yHp, x+TRAINEE_X_SPACING, yHp+font->max_font_height, 
								m.format(attribute) );
						}
					}
					else if( ownSpy )		// display spy icon
						vga.active_buf->put_bitmap_trans( x+TRAINEE_X_SPACING/2-8, yHp-5, image_icon.read("U_SPY") );
					else					// display combat skill
						font->center_put( x, yHp, x+TRAINEE_X_SPACING, yHp+font->max_font_height, 
							m.format(representAttribute) );
				}
				else
				{
					// display spy skill
					err_when( !ownSpy );
					font_whbl.center_put( x, yHp, x+TRAINEE_X_SPACING, yHp+font->max_font_height, 
						m.format(spy_array[ownSpy]->spy_skill) );
				}
			}
		}
	}
}
// ------- end of function FirmTrain::disp_trainee_list -------//


// ------- begin of function FirmTrain::detect_trainee_list -------//
//
// int FirmTrain::detect_trainee_list(int selectSpyMenu)
int FirmTrain::detect_trainee_list()
{
	int selectSpyMenu = 0;

	int dispY1 = disp_trainee_list_y1;

	// display in ascending order to select the overseer first

	for( int i = 1; i <= trainee_count; ++i )
	{
		// display trainee i

		int x = INFO_X1 + 18 + (i-1) * TRAINEE_X_SPACING;
		int y = INFO_Y1 + 136;
		int yHp = INFO_Y1 + 93;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 89;
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		int representAttribute;
		int ownSpy;

		// trainee
		Trainee *traineePtr = &trainee_array[i-1];
		unitId = traineePtr->unit_id;
		representAttribute = represent_attribute(i);
		ownSpy = traineePtr->is_own_spy() ? traineePtr->spy_recno : 0;

		if( selectSpyMenu && !ownSpy )
			continue;

		int rc = info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
			unitId, nation_recno, 
			windowX1, windowY1, windowX2, windowY2, 24 );			// detect left button (8) and right button (16)

		if( !rc )
			continue;

		if( selectSpyMenu == 0 )
		{
			// -------- main menu ---------//

			if( rc & 8 )
			{
				// ----- left click select trainee/overseer -------//

				selected_trainee_id = i;
				return 1;
			}
			else if( rc & 16 && is_own() )
			{
				// ------ right click mobilize solidier/overseer ------//
				// ####### begin Gilbert 26/1 ###//
				if( !trainee_array[i-1].is_under_training )
					mobilize_trainee(i, COMMAND_PLAYER);
				else
					cancel_train_trainee(i, COMMAND_PLAYER);
				// ####### begin Gilbert 26/1 ###//
				return 2;
			}
		}
		else if( selectSpyMenu == 1 )
		{
			if( rc & 8 && ownSpy )
			{
				selected_trainee_id = i;
				return ownSpy;
			}
		}
	}

	return 0;
}
// ------- end of function FirmTrain::detect_trainee_list -------//


// ------- begin of function FirmTrain::disp_trainee_info -------//
//
void FirmTrain::disp_trainee_info(int dispY1, int refreshFlag )
{
	disp_trainee_info_y1 = dispY1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		int x=INFO_X1+20, y=dispY1;
		int x2;

		Trainee* traineePtr = trainee_array + selected_trainee_id - 1;

		// --- don't display name_id, can spot spy ----- //

		font_snds.put( x, y, unit_res[traineePtr->unit_id]->name, 0, -1, 1 );

		// line spacing 24
		if( traineePtr->race_id )
		{

			err_when( traineePtr->unit_id == UNIT_WAGON );

			// ------- display loyalty ---------//
			if( unit_res[traineePtr->unit_id]->class_info.has_loyalty )
			{
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(traineePtr->loyalty, 4)),
									//		font_snds.text_width(m.format(traineePtr->loyalty, 4)) -
									//		font_snds.text_width("11"),
											traineePtr->loyalty, traineePtr->loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
			}
										
		//	info.disp_loyalty( x, y+14, x+60, traineePtr->loyalty, traineePtr->loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );

			// ------- display combat ----------//

			if( unit_res[traineePtr->unit_id]->class_info.has_combat_level )
			{
				x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, text_unit.str_combat_level() ) + 10;
				font_snds.right_put( INFO_X2-10, y+12, m.format(traineePtr->combat_level(), 4) );
			//	font_snds.put( x2, y+14, traineePtr->combat_level() );
			}

			// ------- display leadership -------//

			if( unit_res[traineePtr->unit_id]->class_info.has_skill_level
				&& traineePtr->skill_level() > 0 )
			{
				x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, text_unit.str_leadership() ) + 10;
				font_snds.right_put( INFO_X2-10, y+26, m.format(traineePtr->skill_level(), 4) );
			//	font_snds.put( x2, y+28, traineePtr->skill_level() );
			}

			// ------- display spy skill --------//
			if( traineePtr->spy_skill > 0 )
			{
				x2 = (disp_combat_or_skill==3?font_blu2:font_snds).put( x, y+26, text_unit.str_spy_skill() ) + 10;
				font_snds.right_put( INFO_X2-100, y+26, m.format((int)traineePtr->spy_skill, 4) );
			//	font_snds.put( x2, y+28, (int) traineePtr->spy_skill );
			}
			else if( traineePtr->is_own_spy() )
			{
				x2 = (disp_combat_or_skill==3?font_blu2:font_snds).put( x, y+26, text_unit.str_spy_skill() ) + 10;
				font_snds.right_put( INFO_X2-100, y+26, m.format(spy_array[traineePtr->spy_recno]->spy_skill, 4) );
			//	font_snds.put( x2, y+28, spy_array[traineePtr->spy_recno]->spy_skill );
			}
		}
//		else
//			font_san.field( x, y, "Loyalty", x+100, "N/A", INFO_X2-2, refreshFlag );		// no loyalty because it does not belong to your empire
	}
}
// ------- end of function FirmTrain::disp_trainee_info -------//


// ------- begin of function FirmTrain::detect_trainee_info -------//
//
void FirmTrain::detect_trainee_info()
{
	int dispY1 = disp_trainee_info_y1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		int x=INFO_X1+20, y=dispY1;
		Trainee* traineePtr = trainee_array + selected_trainee_id - 1;
		UnitInfo *unitInfo = unit_res[traineePtr->unit_id];

		if( unitInfo->class_info.has_loyalty
			&& mouse.single_click( x, y+14, x+79, y+27 ) )
		{
			disp_combat_or_skill = 4;		// detect loyalty
		}
		else if( traineePtr->spy_skill > 0 && mouse.single_click( x, y+28, x+79, y+41 ) )
		{
			disp_combat_or_skill = 3;		// detect spying skill
		}
		else if( traineePtr->skill_level() && mouse.single_click( x+110, y+28, x+189, y+41 ) )
		{
			disp_combat_or_skill = 2;		// detect leadership
		}
		else if( unitInfo->class_info.has_combat_level
			&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
		{
			disp_combat_or_skill = 1;		// detect combat skill
		}
		else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
		{
			disp_combat_or_skill = 0;
		}
	}
}
// ------- end of function FirmTrain::detect_trainee_info -------//


// ------- begin of function FirmTrain::disp_train_menu ------//
//
void FirmTrain::disp_train_menu(int refreshFlag)
{
	short townRecnoArray[MAX_RACE];
	int raceCount = scan_best_recruitable_town( townRecnoArray, 0, required_linked_firm[cur_train_type] );

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

			// display trainee i

			//	int x = INFO_X1 + 18 + (raceId-1) * TRAINEE_X_SPACING;
			// not enough space to hold 12 people, so display sequence number
			int x = INFO_X1 + 18 + i * TRAINEE_X_SPACING;
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
				unitId = race_res[raceId]->civilian_unit_id;
				break;
			case TRAIN_UNIT_INFANTRY:
				unitId = race_res[raceId]->infantry_unit_id;
				break;
			case TRAIN_UNIT_SPECIAL:
				unitId = race_res[raceId]->special_unit_id;
				break;
			}

			err_when( townPtr->nation_recno != nation_recno );

			if( inc < 0 )
			{
				// first round is descending draw to icon

				info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
					unitId, townPtr->nation_recno,
					windowX1, windowY1, windowX2, windowY2, 1 );
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
					unitId, townPtr->nation_recno,
					windowX1, windowY1, windowX2, windowY2, inAreaFlag) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				// display loyalty
				font_whbl.center_put( x, yLoyalty, x+TRAINEE_X_SPACING, yLoyalty+font_snds.max_font_height,
					m.format((int) townPtr->loyalty) );
			}
		}
	}
}
// ------- end of function FirmTrain::disp_train_menu ------//


// ------- begin of function FirmTrain::detect_train_menu ------//
//
void FirmTrain::detect_train_menu()
{
	short townRecnoArray[MAX_RACE];
	int raceCount = scan_best_recruitable_town( townRecnoArray, 0, required_linked_firm[cur_train_type] );

	if( mouse.any_click(1) )		// any right click
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

		//	int x = INFO_X1 + 18 + (raceId-1) * TRAINEE_X_SPACING;
		// not enough space to hold 12 people, so display sequence number
		int x = INFO_X1 + 18 + i * TRAINEE_X_SPACING;
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
			unitId = race_res[raceId]->civilian_unit_id;
			break;
		case TRAIN_UNIT_INFANTRY:
			unitId = race_res[raceId]->infantry_unit_id;
			break;
		case TRAIN_UNIT_SPECIAL:
			unitId = race_res[raceId]->special_unit_id;
			break;
		}

		err_when( townPtr->nation_recno != nation_recno );

		if( info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
			unitId, townPtr->nation_recno,
			windowX1, windowY1, windowX2, windowY2, 8 ) & 8 )		// detect left button
		{
			recruit_trainee( townRecnoArray[i], cur_train_type, COMMAND_PLAYER );
			break;
		}
	}
}
// ------- end of function FirmTrain::detect_train_menu ------//


// ------- begin of function FirmTrain::disp_spy_menu ------//
//
void FirmTrain::disp_spy_menu(int refreshFlag)
{
	err_here();
}
// ------- end of function FirmTrain::disp_spy_menu ------//


// ------- begin of function FirmTrain::detect_spy_menu ------//
//
void FirmTrain::detect_spy_menu()
{
	err_here();
}
// ------- end of function FirmTrain::detect_spy_menu ------//


// ---- begin of function FirmTrain::explore_for_player --------//
//
int FirmTrain::explore_for_player()
{
	return Firm::explore_for_player()
		|| player_spy_count > 0;
}
// ---- end of function FirmTrain::explore_for_player --------//


// --------- begin of function FirmTrain::disp_edit_mode -------//
//
void FirmTrain::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	char backupDisp = disp_combat_or_skill;
	disp_combat_or_skill = 0;	// normal
	disp_trainee_list( INFO_Y1, refreshFlag );
	disp_combat_or_skill = backupDisp;

	y = INFO_Y1+175;

	// edit trainee

	edit_loyalty_enable = 0;
	edit_combat_enable = edit_combat_pot_enable = 0;
	edit_skill_enable = edit_skill_pot_enable = 0;
	edit_spy_enable = edit_spy_pot_enable = 0;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		Trainee *traineePtr = &trainee_array[selected_trainee_id-1];
		UnitInfo *unitInfo = unit_res[traineePtr->unit_id];

		// display unit name

		font_whbl.put( x1, y, unit_res[traineePtr->unit_id]->name, 0, x2 );
		y += 18;

		// display loyalty

		if( unitInfo->class_info.has_loyalty ) // && rank_id != RANK_KING )
		{
			x = font_whbl.put( x1, y, text_unit.str_loyalty(), 0, x2 );		// "Loyalty"
			x = edit_loyalty_x2 = font_blue.put( (edit_loyalty_x1=x+20), (edit_loyalty_y1=y), traineePtr->loyalty, 0, x2 );
			edit_loyalty_y2 = edit_loyalty_y1 + font_blue.max_font_height;
			edit_loyalty_enable = 1;
		}
		y += 18;

		// display combat actual(original)

		if( unitInfo->class_info.has_combat_level && traineePtr->train_combat_potential > 0 )
		{
			x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
			x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), traineePtr->skill.actual_combat_level(NULL), 0, x2 );
			edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
			edit_combat_enable = 1;

			x = font_whbl.put( x, y, " (", 0, x2 );
			x = edit_combat_pot_x2 = font_blue.put( (edit_combat_pot_x1=x), (edit_combat_pot_y1=y), m.format(traineePtr->train_combat_potential,3), 0, x2 );
			edit_combat_pot_y2 = edit_combat_pot_y1 + font_blue.max_font_height;
			edit_combat_pot_enable = 1;
			x = font_whbl.put( x, y, ")", 0, x2 );
		}
		y += 18;

		// display leadership

		if( unitInfo->class_info.has_skill_level && traineePtr->train_skill_potential > 0 )
		{
			x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );//	"Leadership"
			x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), traineePtr->skill.actual_skill_level(NULL), 0, x2 );
			edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height;
			edit_skill_enable = 1;

			x = font_whbl.put( x, y, " (", 0, x2 );
			x = edit_skill_pot_x2 = font_blue.put( (edit_skill_pot_x1=x), (edit_skill_pot_y1=y), m.format(traineePtr->train_skill_potential,3), 0, x2 );
			edit_skill_pot_y2 = edit_skill_pot_y1 + font_blue.max_font_height;
			edit_skill_pot_enable = 1;
			x = font_whbl.put( x, y, ")", 0, x2 );
		}
		y += 18;

		// display spy skill

		if( traineePtr->train_spy_potential > 0 )
		{
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );//	"Spying"
			x = edit_spy_x2 = font_blue.put( (edit_spy_x1=x+20), (edit_spy_y1=y), (int)traineePtr->spy_skill, 0, x2 );
			edit_spy_y2 = edit_spy_y1 + font_blue.max_font_height;
			edit_spy_enable = 1;

			x = font_whbl.put( x, y, " (", 0, x2 );
			x = edit_spy_pot_x2 = font_blue.put( (edit_spy_pot_x1=x), (edit_spy_pot_y1=y), m.format(traineePtr->train_spy_potential,3), 0, x2 );
			edit_spy_pot_y2 = edit_spy_pot_y1 + font_blue.max_font_height;
			edit_spy_pot_enable = 1;
			x = font_whbl.put( x, y, ")", 0, x2 );
		}
		y += 18;
	}
	else
	{
		y += 18 * 5;
	}
}
// --------- end of function FirmTrain::disp_edit_mode -------//


// --------- begin of function FirmTrain::detect_edit_mode -------//
//
int FirmTrain::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	if( detect_trainee_list() )
		return 1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		Trainee *traineePtr = &trainee_array[selected_trainee_id-1];

		// edit loyalty

		if( edit_loyalty_enable )
		{
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, LEFT_BUTTON) )
			{
				traineePtr->loyalty += 10;
				if( traineePtr->loyalty > 100 )
					traineePtr->loyalty = 100;
				return 1;
			}
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, RIGHT_BUTTON) )
			{
				traineePtr->loyalty -= 10;
				if( traineePtr->loyalty < 0 )
					traineePtr->loyalty = 0;
				return 1;
			}
		}

		// edit combat

		if( edit_combat_enable )
		{
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, LEFT_BUTTON) )
			{
				traineePtr->skill.inc_combat_level(5.0f);
				return 1;
			}
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, RIGHT_BUTTON) )
			{
				// cannot decrease when basic traine
				if( !traineePtr->is_under_training )
				{
					int newCombat = traineePtr->skill.actual_combat_level(NULL) - 5;
					if( newCombat < BASIC_COMBAT_TRAIN )
						traineePtr->skill.set_combat_level(BASIC_COMBAT_TRAIN);
					else
						traineePtr->skill.set_combat_level(newCombat);
				}
				return 1;
			}
		}

		// edit combat potential

		if( edit_combat_pot_enable )
		{
			if( mouse.any_click(edit_combat_pot_x1, edit_combat_pot_y1, edit_combat_pot_x2, edit_combat_pot_y2, LEFT_BUTTON) )
			{
				traineePtr->train_combat_potential += 5;
				if( traineePtr->train_combat_potential > 100 )
					traineePtr->train_combat_potential = 100;
				return 1;
			}
			if( mouse.any_click(edit_combat_pot_x1, edit_combat_pot_y1, edit_combat_pot_x2, edit_combat_pot_y2, RIGHT_BUTTON) )
			{
				traineePtr->train_combat_potential -= 5;
				if( traineePtr->train_combat_potential < 5 )	// smallest is 5
					traineePtr->train_combat_potential = 5;
				return 1;
			}
		}

		// edit leadership

		if( edit_skill_enable )
		{
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, LEFT_BUTTON) )
			{
				traineePtr->skill.inc_skill_level(5.0f);
				return 1;
			}
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, RIGHT_BUTTON) )
			{
				// cannot decrease when basic traine
				if( !traineePtr->is_under_training )
				{
					int newSkill = traineePtr->skill.actual_skill_level(NULL) - 5;
					if( newSkill < BASIC_SKILL_TRAIN )
						traineePtr->skill.set_skill_level(BASIC_SKILL_TRAIN);
					else
						traineePtr->skill.set_skill_level(newSkill);
				}
				return 1;
			}
		}

		// edit skill potential

		if( edit_skill_pot_enable )
		{
			if( mouse.any_click(edit_skill_pot_x1, edit_skill_pot_y1, edit_skill_pot_x2, edit_skill_pot_y2, LEFT_BUTTON) )
			{
				traineePtr->train_skill_potential += 5;
				if( traineePtr->train_skill_potential > 100 )
					traineePtr->train_skill_potential = 100;
				return 1;
			}
			if( mouse.any_click(edit_skill_pot_x1, edit_skill_pot_y1, edit_skill_pot_x2, edit_skill_pot_y2, RIGHT_BUTTON) )
			{
				traineePtr->train_skill_potential -= 5;
				if( traineePtr->train_skill_potential < 5 )	// smallest is 5
					traineePtr->train_skill_potential = 5;
				return 1;
			}
		}

		// edit spy skill

		if( edit_spy_enable )
		{
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, LEFT_BUTTON) )
			{
				traineePtr->spy_skill += 5.0f;
				if( traineePtr->spy_skill > 100.0f )
					traineePtr->spy_skill = 100.0f;
				return 1;
			}
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, RIGHT_BUTTON) )
			{
				// cannot decrease when basic traine
				if( !traineePtr->is_under_training )
				{
					traineePtr->spy_skill -= 5.0f;
					if( traineePtr->skill.actual_combat_level(NULL) < BASIC_SKILL_TRAIN )
						traineePtr->skill.set_combat_level(BASIC_SKILL_TRAIN);
				}
				return 1;
			}
		}

		// edit spy potential

		if( edit_spy_pot_enable )
		{
			if( mouse.any_click(edit_spy_pot_x1, edit_spy_pot_y1, edit_spy_pot_x2, edit_spy_pot_y2, LEFT_BUTTON) )
			{
				traineePtr->train_spy_potential += 5;
				if( traineePtr->train_spy_potential > 100 )
					traineePtr->train_spy_potential = 100;
				return 1;
			}
			if( mouse.any_click(edit_spy_pot_x1, edit_spy_pot_y1, edit_spy_pot_x2, edit_spy_pot_y2, RIGHT_BUTTON) )
			{
				traineePtr->train_spy_potential -= 5;
				if( traineePtr->train_spy_potential < 5 )	// smallest is 5
					traineePtr->train_spy_potential = 5;
				return 1;
			}
		}
	}

	return 0;
}
// --------- end of function FirmTrain::detect_edit_mode -------//
