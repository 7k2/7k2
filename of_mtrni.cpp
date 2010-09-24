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

// Filename    : OF_MTRNI.CPP
// Description : FirmMonsterTrain Interface

#include <OF_MTRN.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OIMGRES.H>
#include <OBUTT3D.H>
#include <OREMOTE.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OCONFIG.H>
#include <OINFO.H>
#include <ONATION.H>
#include <OSPY.H> 
#include <OITEMRES.H>
#include <OBITMAP.H>
#include <OSE.H>
#include <OSERES.H>
#include <OT_UNIT.H>

// --------- define constant --------//

#define TRAINEE_X_SPACING   24

// --------- define static variable ------//

static int disp_trainee_list_y1;
static int disp_trainee_info_y1;
// int FirmMonsterTrain::cur_train_type;			// parameter unitPara to recruit_train, determine to train civilian spy or infantry spy
// int FirmMonsterTrain::required_linked_firm[TRAIN_UNIT_TYPE_MAX] = { 0, FIRM_FORT, FIRM_SPECIAL };

static Button3D	button_mobilize, button_patrol;
static Button3D	button_train[MAX_MONSTER_TRAIN_TYPE];
static char	disp_combat_or_skill;	// 0=normal, 1=combat, 2=skill, 3=spying, 4=loyalty
static Button3D		button_go_rally, button_clear_rally;

static short edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, edit_loyalty_enable;
static short edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, edit_combat_enable;
static short edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, edit_skill_enable;
static short edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, edit_spy_enable;


// ------- begin of function FirmMonsterTrain::put_info -------//
//
void FirmMonsterTrain::put_info(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		firm_menu_mode = FIRM_MONS_TRAIN_MENU_MAIN;
		disp_combat_or_skill = 0;
	}

	Firm::put_info(refreshFlag);

	switch( firm_menu_mode )
	{
	case FIRM_MONS_TRAIN_MENU_MAIN:

		if( !should_show_info() )
			return;
	
		err_when(train_type_count <= 0);
		
		vga.active_buf->put_bitmap(INFO_X1, INFO_Y1, image_gameif.read("STABBASE"));
		disp_firm_info(INFO_Y1, refreshFlag);
		disp_trainee_list(INFO_Y1, refreshFlag );

		if (train_type_count == 1 )
		{
			int unitId = train_unit_id[0];

			info.draw_unit_icon( INFO_X1+40, INFO_Y1+52,	unitId, nation_recno,
				INFO_X1+15, INFO_Y1+5, INFO_X1+68, INFO_Y1+84, 1 );
			font_snds.center_put( INFO_X1+88, INFO_Y1+15, INFO_X1+217, INFO_Y1+74, unit_res[unitId]->name, 0, 0 );
		}

		if( selected_trainee_id > trainee_count )
			selected_trainee_id = trainee_count;

		if( selected_trainee_id )
			disp_trainee_info(INFO_Y1+178, refreshFlag );
		break;
	}
}
// ------- end of function FirmMonsterTrain::put_info -------//

// ------- begin of function FirmMonsterTrain::detect_info -------//
//
void FirmMonsterTrain::detect_info()
{
	switch( firm_menu_mode )
	{
	case FIRM_MONS_TRAIN_MENU_MAIN:
		detect_firm_info();
		detect_trainee_list();

		if( selected_trainee_id > trainee_count )
			selected_trainee_id = trainee_count;

		if( selected_trainee_id )
			detect_trainee_info();
		break;
	}
}
// ------- end of function FirmMonsterTrain::put_info -------//



// ------- begin of function FirmMonsterTrain::disp_firm_info -------//
//
void FirmMonsterTrain::disp_firm_info(int dispY1, int refreshFlag )
{
	int i;

	if( refreshFlag == INFO_REPAINT )
	{
		button_mobilize.create( INFO_X1+13, INFO_Y1+235, 'A', "MOBILIZE" );
		button_mobilize.enable_flag = 0;
		if ((train_type_count == 1) && (train_unit_id[0] == UNIT_BEE))
			button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "PATROL-B" );
		else
		if ((train_type_count == 1) && (train_unit_id[0] == UNIT_TERMITE))
			button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "PATROL-T" );
		else
			button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "PATROL" );

		button_patrol.enable_flag = 0;
		for( i = 0; i < train_type_count; ++i )
		{
			if( i < 4 )		// 4 buttons at bottom line
			{
				if ((train_type_count == 1) && (train_unit_id[0] == UNIT_BEE))
					button_train[i].create( INFO_X1+13+i*BUTTON_DISTANCE, INFO_Y1+281, 'A', "TRAIN-B" );
				else
				if ((train_type_count == 1) && (train_unit_id[0] == UNIT_TERMITE))
					button_train[i].create( INFO_X1+13+i*BUTTON_DISTANCE, INFO_Y1+281, 'A', "TRAIN-T" );
				else
					button_train[i].create( INFO_X1+13+i*BUTTON_DISTANCE, INFO_Y1+281, 'A', "F_TRAIN" );
			}
			else
			{
				if ((train_type_count == 1) && (train_unit_id[0] == UNIT_BEE))
					button_train[i].create( INFO_X1+13+(7-i)*BUTTON_DISTANCE, INFO_Y1+235, 'A', "TRAIN-B" );
				else
				if ((train_type_count == 1) && (train_unit_id[0] == UNIT_TERMITE))
					button_train[i].create( INFO_X1+13+(7-i)*BUTTON_DISTANCE, INFO_Y1+235, 'A', "TRAIN-T" );
				else
					button_train[i].create( INFO_X1+13+(7-i)*BUTTON_DISTANCE, INFO_Y1+235, 'A', "F_TRAIN" );
			}
			button_train[i].enable_flag = 0;
		}

		// ###### begin Gilbert 11/3 ########//
		button_go_rally.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+235, 'A', "RALLY-GO" );
		button_clear_rally.create( INFO_X1+13+BUTTON_DISTANCE*3, INFO_Y1+235, 'A', "RALLY-NO" );
		// ###### end Gilbert 11/3 ########//
	}

	if( !should_show_info() )
		return;

	// ------ display mobilize button -------//

	button_mobilize.enable_flag = 0;

	if( is_own() )
	{
		if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
		{
			MonsterTrainee *traineePtr = trainee_array + selected_trainee_id - 1;
			button_mobilize.enable_flag = !traineePtr->is_under_training();
		}

		button_mobilize.paint();

		// ----- display mobilize all button -------//

		for( i = 0; i < trainee_count && trainee_array[i].is_under_training(); ++i );
		button_patrol.enable_flag = i < trainee_count;
		button_patrol.paint();

		// ----- display train button ----------// 

		for( i = 0; i < train_type_count; ++i )
		{
			button_train[i].enable_flag = can_recruit(train_unit_id[i]);
			button_train[i].paint();
		}

		// ------- display rally points button ------//

		if( (button_go_rally.visible_flag = rally_enable_flag != 0) )
			button_go_rally.paint();

		if( (button_clear_rally.visible_flag = rally_enable_flag != 0) )
			button_clear_rally.paint();
	}
}
// ------- end of function FirmMonsterTrain::disp_firm_info -------//


// ------- begin of function FirmMonsterTrain::detect_firm_info -------//
//
void FirmMonsterTrain::detect_firm_info()
{
	int i;

	if( !should_show_info() )
		return;

	// since both button_mobilize and button_spy_mobilize share the same place
	// if one button is enable, the other button detect() is not called at all to avoid incorrect help text

	if( button_mobilize.detect() 
		&& selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		// ####### begin Gilbert 26/1 #######//
		if( !trainee_array[selected_trainee_id-1].is_under_training() )
			mobilize_trainee( selected_trainee_id, COMMAND_PLAYER );
		else
			cancel_train_trainee( selected_trainee_id, COMMAND_PLAYER );
		// ####### end Gilbert 26/1 #######//

		se_ctrl.immediate_sound("TURN_ON");
	}
	else if( button_patrol.detect() && trainee_count > 0 )
	{
		mobilize_trainee( -1, COMMAND_PLAYER );
//		se_ctrl.immediate_sound("SORTIE");
		if (trainee_count > 0)
		{
			MonsterTrainee *traineePtr = &trainee_array[0];
			int unitId = traineePtr->unit_id;
			UnitInfo *unitInfo = unit_res[unitId];
			se_res.sound( center_x, center_y, 1, 'S', unitInfo->sprite_id, "ACK");
		}			
	}
	else 
	{
		for( i = 0; i < train_type_count; ++i )
		{
			if( button_train[i].detect() )
			{
				recruit_trainee( train_unit_id[i], COMMAND_PLAYER );
			//	se_ctrl.immediate_sound("TURN_ON");
				se_ctrl.immediate_sound("ARM-ALRT");
				break;
			}
		}
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
// ------- end of function FirmMonsterTrain::detect_firm_info -------//


// ------- begin of function FirmMonsterTrain::disp_trainee_list -------//
//
void FirmMonsterTrain::disp_trainee_list(int dispY1, int refreshFlag )
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
			MonsterTrainee *traineePtr = &trainee_array[i-1];
			unitId = traineePtr->unit_id;
			UnitInfo *unitInfo = unit_res[unitId];
			representAttribute = represent_attribute(i);
			ownSpy = 0;		// traineePtr->is_own_spy() ? traineePtr->spy_recno : 0;

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
				info.draw_unit_icon( x+TRAINEE_X_SPACING/2, y,
					unitId, nation_recno, windowX1, windowY1, windowX2, windowY2, 
					(traineePtr->is_under_training()) ? (((20 - traineePtr->combat_level()) <<6)+ 33) : 1);
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
						// case 3: attribute = (int) traineePtr->spy_skill; break;
						case 4: 
							if( unitInfo->class_info.has_loyalty )
								attribute = traineePtr->loyalty;
							break;
						default:
							err_here();
							break;
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
// ------- end of function FirmMonsterTrain::disp_trainee_list -------//


// ------- begin of function FirmMonsterTrain::detect_trainee_list -------//
//
// int FirmMonsterTrain::detect_trainee_list(int selectSpyMenu)
int FirmMonsterTrain::detect_trainee_list()
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
		MonsterTrainee *traineePtr = &trainee_array[i-1];
		unitId = traineePtr->unit_id;
		representAttribute = represent_attribute(i);
		ownSpy = 0;	 //traineePtr->is_own_spy() ? traineePtr->spy_recno : 0;

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
				// ####### begin Gilbert 26/1 #######//
				if( !trainee_array[i-1].is_under_training() )
					mobilize_trainee(i, COMMAND_PLAYER);
				else
					cancel_train_trainee(i, COMMAND_PLAYER);
				// ####### end Gilbert 26/1 #######//
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
// ------- end of function FirmMonsterTrain::detect_trainee_list -------//


// ------- begin of function FirmMonsterTrain::disp_trainee_info -------//
//
void FirmMonsterTrain::disp_trainee_info(int dispY1, int refreshFlag )
{
	disp_trainee_info_y1 = dispY1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		int x=INFO_X1+20, y=dispY1;
		int x2;

		MonsterTrainee* traineePtr = trainee_array + selected_trainee_id - 1;

		// --- don't display name_id, can spot spy ----- //

		font_snds.put( x, y, unit_res[traineePtr->unit_id]->name, 0, -1, 1 );

		// line spacing 24

		// ------- display loyalty ---------//

		if( unit_res[traineePtr->unit_id]->class_info.has_loyalty )
		{
		//	info.disp_loyalty( x, y+14, x+60, traineePtr->loyalty, traineePtr->loyalty, nation_recno, refreshFlag );
			info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(traineePtr->loyalty, 4)),
							traineePtr->loyalty, traineePtr->loyalty, nation_recno, refreshFlag );
		}
				
		// ------- display combat ----------//

		if( unit_res[traineePtr->unit_id]->class_info.has_combat_level )
		{
			x2 = font_snds.put( x+110, y+12, text_unit.str_combat_level() ) + 10;
			font_snds.right_put( INFO_X2-10, y+12, m.format(traineePtr->combat_level(),4) );
		//	font_snds.put( x2, y+14, traineePtr->combat_level() );
		}

		// ------- display leadership -------//

		if( unit_res[traineePtr->unit_id]->class_info.has_skill_level
			&& traineePtr->skill_level() > 0 )
		{
			x2 = font_snds.put( x+110, y+26, text_unit.str_leadership() ) + 10;
			font_snds.right_put( INFO_X2-10, y+26, m.format(traineePtr->skill_level(),4) );
	//		font_snds.put( x2, y+28, traineePtr->skill_level() );
		}

		// ------- display spy skill --------//

//		if( traineePtr->spy_skill > 0 )
//		{
//			x2 = font_snds.put( x, y+28, "Spying" ) + 10;
//			font_snds.put( x2, y+28, (int) traineePtr->spy_skill );
//		}
//		else if( traineePtr->is_own_spy() )
//		{
//			x2 = font_snds.put( x, y+28, "Spying" ) + 10;
//			font_snds.put( x2, y+28, spy_array[traineePtr->spy_recno]->spy_skill );
//		}
	}
}
// ------- end of function FirmMonsterTrain::disp_trainee_info -------//


// ------- begin of function FirmMonsterTrain::detect_trainee_info -------//
//
void FirmMonsterTrain::detect_trainee_info()
{
	int dispY1 = disp_trainee_info_y1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		int x=INFO_X1+20, y=dispY1;
		MonsterTrainee* traineePtr = trainee_array + selected_trainee_id - 1;

		UnitInfo *unitInfo = unit_res[traineePtr->unit_id];

		if( unitInfo->class_info.has_loyalty 
			&& mouse.single_click( x, y+14, x+79, y+27 ) )
		{
			disp_combat_or_skill = 4;		// detect loyalty
		}
//		else if( traineePtr->spy_skill > 0 && mouse.single_click( x, y+28, x+79, y+41 ) )
//		{
//			disp_combat_or_skill = 3;		// detect spying skill
//		}
		else if( traineePtr->skill_level() && mouse.single_click( x+110, y+28, x+189, y+41 ) )
		{
			disp_combat_or_skill = 2;		// detect leadership
		}
		else if( mouse.single_click( x+110, y+14, x+189, y+27 ) )
		{
			disp_combat_or_skill = 1;		// detect combat skill
		}
		else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
		{
			disp_combat_or_skill = 0;
		}
	}
}
// ------- end of function FirmMonsterTrain::detect_trainee_info -------//


// ------- begin of function FirmMonsterTrain::represent_attribute --------//
//
int FirmMonsterTrain::represent_attribute(int traineeId)
{
	err_when( traineeId <= 0 || traineeId > trainee_count );
	MonsterTrainee *traineePtr = trainee_array + traineeId - 1;
	return traineePtr->combat_level();
}
// ------- end of function FirmMonsterTrain::represent_attribute --------//


// --------- begin of function FirmMonsterTrain::disp_edit_mode -------//
//
void FirmMonsterTrain::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
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
	edit_combat_enable = 0;
	edit_skill_enable = 0;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		MonsterTrainee *traineePtr = &trainee_array[selected_trainee_id-1];
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

		if( unitInfo->class_info.has_combat_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
			x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), traineePtr->skill.actual_combat_level(NULL), 0, x2 );
			edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
			edit_combat_enable = 1;
		}
		y += 18;

		// display leadership

		if( unitInfo->class_info.has_skill_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );//	"Leadership"
			x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), traineePtr->skill.actual_skill_level(NULL), 0, x2 );
			edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height;
			edit_skill_enable = 1;
		}
		y += 18;
	}
	else
	{
		y += 18 * 4;
	}
}
// --------- end of function FirmMonsterTrain::disp_edit_mode -------//


// --------- begin of function FirmMonsterTrain::detect_edit_mode -------//
//
int FirmMonsterTrain::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	if( detect_trainee_list() )
		return 1;

	if( selected_trainee_id > 0 && selected_trainee_id <= trainee_count )
	{
		MonsterTrainee *traineePtr = &trainee_array[selected_trainee_id-1];

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
				if( !traineePtr->is_under_training() )
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
				if( !traineePtr->is_under_training() )
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
	}

	return 0;
}
// --------- end of function FirmMonsterTrain::detect_edit_mode -------//
