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

// Filename     : OF_MFORI.CPP
// Description  : Monster Fortress Interface functions


#include <of_mfort.h>
#include <ovga.h>
#include <omodeid.h>
#include <oimgres.h>
#include <omouse.h>
#include <ofont.h>
#include <obutton.h>
#include <obutt3d.h>
#include <osys.h>
#include <ose.h>
#include <oseres.h>
#include <oremote.h>
#include <oinfo.h>
#include <omonsres.h>
#include <orockres.h>
#include <onation.h>
#include <ounit.h>
#include <ospy.h>
#include <oitemres.h>
#include <obitmap.h>
#include <ot_unit.h>


// --------- define constant --------//

#define ARHCER_X_SPACING   48

//----------- Define static vars -------------//

static Button3D button_patrol, button_reward, button_transform_grokken;
static int      disp_soldier_list_y1;
static int      disp_soldier_info_y1;
static char		 disp_combat_or_skill;	// 0=normal, 1=combat, 2=skill, 3=spying, 4=loyalty
static char		 swap_item_src;
static char		 pointed_archer_id;
short FirmMonsterFortress::swap_item_id;

static short edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, edit_hp_enable;
static short edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, edit_loyalty_enable;
static short edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, edit_combat_enable;
static short edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, edit_max_combat_enable;
static short edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, edit_skill_enable;
static short edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, edit_max_skill_enable;
static short edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, edit_spy_enable;

//--------- Declare static functions ---------//

static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);


// ----- begin of function FirmMonsterFortress::put_info -----//
//
void FirmMonsterFortress::put_info(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	if( should_show_info() )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("STABBASE") );

		disp_camp_info(INFO_Y1, refreshFlag);

		disp_soldier_list(INFO_Y1, refreshFlag, 0);
		disp_soldier_info(INFO_Y1+178, refreshFlag);
		// disp_debug_info(this, refreshFlag);
	}
}
// ----- end of function FirmMonsterFortress::put_info -----//


// ----- begin of function FirmMonsterFortress::detect_info -----//
//
void FirmMonsterFortress::detect_info()
{
	// ##### patch begin Gilbert 7/8 #####//
	int firmRecno = firm_recno;
	// ##### patch end Gilbert 7/8 #####//

	Firm::detect_info();

	if( should_show_info() )
	{
		detect_camp_info();
		// ##### patch begin Gilbert 7/8 #####//
		if( firm_array.is_deleted(firmRecno) )		// in case button_transform_grokken button is pressed
			return;
		// ##### patch end Gilbert 7/8 #####//
		detect_soldier_list(0);
		detect_soldier_info();
	}
}
// ----- end of function FirmMonsterFortress::detect_info -----//


//--------- Begin of function FirmMonsterFortress::disp_camp_info ---------//
//
void FirmMonsterFortress::disp_camp_info(int dispY1, int refreshFlag)
{
	if( is_own() )
	{
		int x1 = INFO_X1 +13;
		int y1 = INFO_Y1 +235;
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		if( refreshFlag==INFO_REPAINT )
		{
			button_patrol.create( x2, y2, 'A', "PATROL" );
			button_transform_grokken.create( x1+BUTTON_DISTANCE, y1, 'A', "FORTRS-2" );
			button_reward.create( x1, y1, 'A', "F_REWARD" );
		}
		button_patrol.enable_flag = archer_count > 0;
		button_patrol.paint();

		button_transform_grokken.enable_flag = 1;
		button_transform_grokken.paint();

		if( nation_array[nation_recno]->cash >= REWARD_COST 
			&& selected_archer_id > 0 && selected_archer_id <= archer_count
			&& unit_array[archer_unit_recno[selected_archer_id-1]]->rank_id != RANK_KING )
			button_reward.enable_flag = 1;			// call paint
		else
			button_reward.enable_flag = 0;		// call paint
		button_reward.paint();


		// display number of supporter

		info.draw_unit_icon( INFO_X1+40, INFO_Y1+52,
			support_unit_id, nation_recno,
			INFO_X1+15, INFO_Y1+5, INFO_X1+68, INFO_Y1+84, 1 );

		String str;
		str = unit_res[support_unit_id]->name;
		str += " x ";
		str += extra_builder_count;
		font_snds.center_put( INFO_X1+88, INFO_Y1+15, INFO_X1+217, INFO_Y1+74, str, 0, 0 );
	}
}
//----------- End of function FirmMonsterFortress::disp_camp_info -----------//


//--------- Begin of function FirmMonsterFortress::detect_camp_info ---------//
//
void FirmMonsterFortress::detect_camp_info()
{
	if( is_own() )
	{
		//----------- detect patrol -----------//

		if( button_transform_grokken.detect() )
		{
			// ##### patch begin Gilbert 7/8 #####//
			se_ctrl.immediate_sound("TURN_ON");
			sell_firm(COMMAND_PLAYER);
			return;
			// ##### patch end Gilbert 7/8 #####//
		}
		
		if( button_patrol.detect() )
		{
			if(remote.is_enable())
			{
				// packet structure : <firm recno> <patrol_state>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_CAMP_PATROL, 2 * sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = 0;
			}
			else
			{
				patrol();
			}
		//	se_ctrl.immediate_sound("SORTIE");
			if( archer_count > 0 )
			{
				Unit *unitPtr = unit_array[archer_unit_recno[0]];
				se_res.sound( center_x, center_y, 1, 'S', unitPtr->sprite_id, "ACK");
			}
		}

		//----------- detect reward -----------//

		if( selected_archer_id > 0 && selected_archer_id <= archer_count  
			&& button_reward.detect() )
		{
			if( !remote.is_enable() )
			{
				unit_array[archer_unit_recno[selected_archer_id-1]]->reward(nation_array.player_recno);
			}
			else
			{
				// packet structure : <rewarding nation> <unit count> <unit recno> ...
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_REWARD, 3*sizeof(short));
				// reserve two short more in case selected_recno is slightly smalled then the actual selected
				shortPtr[0] = nation_array.player_recno;
				shortPtr[1] = 1;
				shortPtr[2] = archer_unit_recno[selected_archer_id-1];
			}
			se_ctrl.immediate_sound("TURN_ON");
		}
	}
}
//----------- End of function FirmMonsterFortress::detect_camp_info -----------//



//--------- Begin of function FirmMonsterFortress::disp_soldier_list ---------//
//
void FirmMonsterFortress::disp_soldier_list(int dispY1, int refreshFlag, int dispSpyMenu)
{
	disp_soldier_list_y1 = dispY1;

	if( refreshFlag == INFO_REPAINT )
	{
		swap_item_src = -1;		// reset from selecting swap target
	}
	pointed_archer_id = -1;

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;

		for( int i = inc>=0?1:archer_count; i >= 1 && i <= archer_count; i +=inc )
		{
			// display soldier i

			int x = INFO_X1 + 24 + (i-1) * ARHCER_X_SPACING;
			int y = INFO_Y1 + 136;
			int yHp = INFO_Y1 + 93;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 220;
			int windowY1 = INFO_Y1 + 89;
			int windowY2 = windowY1 + 80 - 1 ;

			int unitId;
			int hp;
			int maxHp;
			int combatLevel;
			int skillLevel;
			int loyalty;
			int ownSpy;
			int itemId;

			Unit *unitPtr = unit_array[archer_unit_recno[i-1]];
			unitId = unitPtr->unit_id;
			UnitInfo *unitInfo = unit_res[unitId];

			hp = (int) unitPtr->hit_points;
			maxHp = unitPtr->max_hit_points();
			combatLevel = (int) unitPtr->combat_level();
			skillLevel = (int) unitPtr->skill_level();

			if( unitPtr->rank_id == RANK_KING )
				loyalty = -1;		// king
			else
				loyalty = unitPtr->loyalty;

			ownSpy = unitPtr->is_own_spy() ? unitPtr->spy_recno : 0;
			itemId = unitPtr->item.id;

			if( dispSpyMenu && !ownSpy )			// skip displaying spy
				continue;

			// display that solider icon at x+SOLDIER_X_SPACING/2, y
			// draw a frame if selected

			if( inc < 0 )
			{
				// first round is descending draw to icon
				info.draw_unit_icon( x+ARHCER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 1 );
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+ARHCER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2,
					inAreaFlag | (i==selected_archer_id?3:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
					pointed_archer_id = i;
				}

				// display combat skill

				Font *font = &font_whbl;

				if( !dispSpyMenu )
				{
					if( disp_combat_or_skill )		// display skill level
					{
						font = &font_blue;
						int attribute = -1;
						switch( disp_combat_or_skill )
						{
						case 1:
							if( unitInfo->class_info.has_combat_level )
								attribute = combatLevel; 
							break;
						case 2:
							if( unitInfo->class_info.has_skill_level && skillLevel > 0 )
								attribute = skillLevel;
							break;
						case 4:
							if( unitInfo->class_info.has_loyalty )
								attribute = loyalty;
							break;
						default:
							err_here();
						}
						if( attribute >= 0 )	// hide attribute on some cases
							font->center_put( x, yHp, x+ARHCER_X_SPACING, 
								yHp+font->max_font_height, m.format(attribute) );
					}
					else if( ownSpy )		// display spy icon
					{
						vga.active_buf->put_bitmap_trans( x+ARHCER_X_SPACING/2-8, yHp-5, image_icon.read("U_SPY") );
					}
					else									// display combat skill
					{
						font->center_put( x, yHp, x+ARHCER_X_SPACING, yHp+font->max_font_height, 
							m.format(combatLevel) );
					}

					if( itemId )
					{
						char *iconPtr = item_res.item_unit_interface(itemId);
						if( iconPtr )
							vga.active_buf->put_bitmap_trans( x+ARHCER_X_SPACING/2-((Bitmap *)iconPtr)->get_width()/2,
							yHp +53, iconPtr );						
					}
				}
				else
				{
					// display spy skill
					err_when( !ownSpy );
					font_whbl.center_put( x, yHp, x+ARHCER_X_SPACING, yHp+font->max_font_height, 
						m.format(spy_array[ownSpy]->spy_skill) );
				}

				// display hit points bar

				disp_soldier_hit_points( x, yHp+font->max_font_height+2, x+ARHCER_X_SPACING*7/8-1, hp, maxHp );
			}
		}
	}
}
//----------- End of function FirmMonsterFortress::disp_soldier_list -----------//


//--------- Begin of function FirmMonsterFortress::detect_soldier_list ---------//
//
// <int> selecteSpyMenu        0=main menu; 1=selecting spy
// when selectSpyMenu is 0, return 1 if left click on a unit, return 2 if right click on a unit
// when selectSpyMenu is 1, return spy_recno of the clicked spy, 0 if no own spy is clicked
//
int FirmMonsterFortress::detect_soldier_list(int selectSpyMenu)
{
	int dispY1 = disp_soldier_list_y1;

	// display in ascending order to select the overseer first

	for( int i = 1; i <= archer_count; ++i )
	{
		// display soldier i

		int x = INFO_X1 + 24 + (i-1) * ARHCER_X_SPACING;
		int y = INFO_Y1 + 136;
		int yHp = INFO_Y1 + 93;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 89;
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		int hp;
		int maxHp;
		int combatLevel;
		int skillLevel;
		int loyalty;
		int ownSpy;
		int itemId;

		Unit *unitPtr = unit_array[archer_unit_recno[i-1]];
		unitId = unitPtr->unit_id;

		hp = (int) unitPtr->hit_points;
		maxHp = unitPtr->max_hit_points();
		combatLevel = (int) unitPtr->combat_level();
		skillLevel = (int) unitPtr->skill_level();

		if( unitPtr->rank_id != RANK_GENERAL )
			loyalty = -1;		// king or other(?)
		else
			loyalty = unitPtr->loyalty;

		ownSpy = unitPtr->is_own_spy() ? unitPtr->spy_recno : 0;
		itemId = unitPtr->item.id;

		if( selectSpyMenu && !ownSpy )			// skip displaying spy
			continue;

		int rc = info.draw_unit_icon( x+ARHCER_X_SPACING/2, y,
			unitId, nation_recno, 
			windowX1, windowY1, windowX2, windowY2, 24 );		// detect left button (8) and right button(16)

		if( !rc )
			continue;

		if( selectSpyMenu == 0 )
		{
			// -------- main menu ---------//

			if( rc & 8 )
			{
				// ----- left click select soldier/overseer -------//

				selected_archer_id = i;
				swap_item_src = selected_archer_id;		// turn on the selecting swap target
				swap_item_id = itemId;
				return 1;
			}
			else if( rc & 16 && is_own() )
			{
				// ------ right click mobilize solidier/overseer ------//

				mobilize_soldier(i, COMMAND_PLAYER);
				return 2;
			}
		}
		else if( selectSpyMenu == 1 )
		{
			if( rc & 8 && ownSpy )
			{
				selected_archer_id = i;
				return ownSpy;
			}
		}
	}

	// swaping item

	if( is_own() && selectSpyMenu == 0 && swap_item_src >= 0 && mouse.release_click() )
	{
		if( pointed_archer_id >= 0 )
		{
			swap_item( swap_item_src, pointed_archer_id, swap_item_id, COMMAND_PLAYER );
		}
		else
		{
			swap_item_src = -1;		// reset from selecting swap target
		}
	}

	return 0;
}
//----------- End of function FirmMonsterFortress::detect_soldier_list -----------//


//--------- Begin of function FirmMonsterFortress::disp_soldier_info ---------//
//
void FirmMonsterFortress::disp_soldier_info(int dispY1, int refreshFlag)
{
	disp_soldier_info_y1 = dispY1;

	if( selected_archer_id > 0 && selected_archer_id <= archer_count )
	{
		int x=INFO_X1+20, y=dispY1;
		int x2;

		Unit *archerPtr = unit_array[archer_unit_recno[selected_archer_id-1]];

		//--- don't display name_id, can spot spy -----//

		String str;

		str  = archerPtr->unit_name();

		if( archerPtr->race_id )
		{
			str += " (";

			err_when(archerPtr->is_human());	// cannot be human
			str += monster_res[archerPtr->monster_id()]->name;

			str += ")";
		}

		font_snds.put( x, y, str, 0, -1, 1 );

		//------------------------------------------------//

		// line spacing 24

		// ------- display loyalty ---------//

		UnitInfo *unitInfo = unit_res[archerPtr->unit_id];

		if( archerPtr->rank_id != RANK_KING && nation_recno && unitInfo->class_info.has_loyalty )
		{
			if (archerPtr->loyalty != archerPtr->target_loyalty)
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(archerPtr->loyalty, 4)) -
						font_snds.text_width(m.format(archerPtr->target_loyalty, 4)) -
						font_snds.text_width("11"),
						archerPtr->loyalty, archerPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
			else
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(archerPtr->loyalty, 4)),
						archerPtr->loyalty, archerPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
		}

		// ------- display combat ----------//

		if( unitInfo->class_info.has_combat_level )
		{
			x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, text_unit.str_combat_level() ) + 10;
		//	font_snds.put( x2, y+14, archerPtr->combat_level() );
			font_snds.right_put( INFO_X2-10, y+12, m.format(archerPtr->combat_level(),4) );
		}

		// ------- display leadership -------//

		if( unitInfo->class_info.has_skill_level && archerPtr->skill_level() > 0 )
		{
			x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, text_unit.str_leadership() ) + 10;
		//	font_snds.put( x2, y+28, archerPtr->skill_level() );
			font_snds.right_put( INFO_X2-10, y+26, m.format(archerPtr->skill_level(),4) );
		}

		// ----- display hit point ---------//

		x2 = font_snds.put( x, y+26, text_unit.str_hit_points() ) + 10;
		str = m.format((int) archerPtr->hit_points, 4);
		str += "/";
		str += m.format(archerPtr->max_hit_points(), 4);
		font_snds.right_put( INFO_X2-100, y+26, str );
	}
}
//--------- End of function FirmMonsterFortress::disp_soldier_info ---------//


//--------- Begin of function FirmMonsterFortress::detect_soldier_info ---------//
//
void FirmMonsterFortress::detect_soldier_info()
{
	int dispY1 = disp_soldier_info_y1;

	if( selected_archer_id > 0 && selected_archer_id <= archer_count )
	{
		int x=INFO_X1+20, y=dispY1;
		Unit *archerPtr = unit_array[archer_unit_recno[selected_archer_id-1]];
		UnitInfo *unitInfo = unit_res[archerPtr->unit_id];

		if( archerPtr->rank_id != RANK_KING && nation_recno && unitInfo->class_info.has_loyalty
			&& mouse.single_click( x, y+14, x+79 , y+27 ) )
		{
			// detect loyalty
			disp_combat_or_skill = 4;
		}
		else if( unitInfo->class_info.has_combat_level
			&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
		{
			// detect combat level
			disp_combat_or_skill = 1;
		}
		else if( unitInfo->class_info.has_skill_level && archerPtr->skill_level() > 0
			&& mouse.single_click( x+110, y+28, x+189, y+41 ) )
		{
			// detect skill
			disp_combat_or_skill = 2;
		}
		else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
		{
			disp_combat_or_skill = 0;
		}
	}
}
//--------- End of function FirmMonsterFortress::detect_soldier_info ---------//


//--------- Begin of function disp_soldier_hit_points ---------//
//
static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	//------- determine the hit bar type -------//

	#define HIT_BAR_TYPE_COUNT  3

	int  hit_bar_color_array[HIT_BAR_TYPE_COUNT] = { 0xA8, 0xB4, 0xAC };
	int  hit_bar_max_array[HIT_BAR_TYPE_COUNT] 	= { 50, 100, 200 };
	char hitBarColor;

	for( int i=0 ; i<HIT_BAR_TYPE_COUNT ; i++ )
	{
		if( maxHitPoints <= hit_bar_max_array[i] || i==HIT_BAR_TYPE_COUNT-1 )
		{
			hitBarColor = hit_bar_color_array[i];
			break;
		}
	}

	//------- draw the hit points bar -------//

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / MAX(hitPoints, maxHitPoints);

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

//	if( x1+barWidth <= x2 )
//		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga.active_buf->bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga.active_buf->bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_soldier_hit_points -----------//


// -------- begin of function FirmMonsterFortress::draw ---------//
void FirmMonsterFortress::draw(int displayLayer)
{
	if( !rock_id )
	{
		Firm::draw(displayLayer);
	}
	else
	{
		rock_res.draw(rock_id, loc_x1, loc_y1, altitude, 1 );
	}
}
// -------- end of function FirmMonsterFortress::draw ---------//


// --------- begin of function FirmMonsterFortress::disp_edit_mode -------//
//
void FirmMonsterFortress::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// display background here, set dispBG to 0

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	char backupDisp = disp_combat_or_skill;
	disp_combat_or_skill = 0;	// normal
	disp_soldier_list( INFO_Y1, refreshFlag, 0 );
	disp_combat_or_skill = backupDisp;

	y = INFO_Y1+175;

	// edit trainee

	edit_hp_enable = 0;
	edit_loyalty_enable = 0;
	edit_combat_enable = edit_max_combat_enable = 0;
	edit_skill_enable = edit_max_skill_enable = 0;

	if( selected_archer_id > 0 && selected_archer_id <= archer_count
		&& !unit_array.is_deleted(archer_unit_recno[selected_archer_id-1]) )
	{
		Unit *archerPtr = unit_array[archer_unit_recno[selected_archer_id-1]];
		UnitInfo *unitInfo = unit_res[archerPtr->unit_id];

		// display unit name name

		font_whbl.put( x1, y, archerPtr->unit_name(1), 0, x2, 1 );
		y += 15;

		// display hit point

		x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
		x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)archerPtr->hit_points, 0, x2 );
		edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
		edit_hp_enable = 1;
		x = font_whbl.put( x, y, " / ", 0, x2 );
		x = font_whbl.put( x, y, archerPtr->max_hit_points(), 0, x2 );
		y += 15;

		// disp loyalty
		if( unitInfo->class_info.has_loyalty ) // && rank_id != RANK_KING )
		{
			x = font_whbl.put( x1, y, text_unit.str_loyalty(), 0, x2 );		// "Loyalty"
			x = edit_loyalty_x2 = font_blue.put( (edit_loyalty_x1=x+20), (edit_loyalty_y1=y), archerPtr->loyalty, 0, x2 );
			edit_loyalty_y2 = edit_loyalty_y1 + font_blue.max_font_height;
			edit_loyalty_enable = 1;
			x = font_whbl.put( x, y, " -> ", 0, x2 );
			x = font_whbl.put( x, y, archerPtr->target_loyalty, 0, x2 );
		}
		y += 15;

		// display combat actual(original)
		if( unitInfo->class_info.has_combat_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
			x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), archerPtr->skill.actual_combat_level(NULL), 0, x2 );
			edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
			edit_combat_enable = 1;
			if( archerPtr->combat_level() != archerPtr->skill.actual_combat_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, archerPtr->combat_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum combat level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_combat_x2 = font_blue.put( (edit_max_combat_x1=x), (edit_max_combat_y1=y), archerPtr->skill.max_combat_level, 0, x2 );
				edit_max_combat_y2 = edit_max_combat_y1 + font_blue.max_font_height;
				edit_max_combat_enable = edit_combat_enable;
			}
		}
		y += 15;

		// display skill actual(original)
		if( unitInfo->class_info.has_skill_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );	// "Leadership"
			x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), archerPtr->skill.actual_skill_level(NULL), 0, x2 );
			edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height - 1;
			edit_skill_enable = 1;
			if( archerPtr->skill_level() != archerPtr->skill.actual_skill_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, archerPtr->skill_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum skill level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_skill_x2 = font_blue.put( (edit_max_skill_x1=x), (edit_max_skill_y1=y), archerPtr->skill.max_skill_level, 0, x2 );
				edit_max_skill_y2 = edit_max_skill_y1 + font_blue.max_font_height;
				edit_max_skill_enable = edit_skill_enable;
			}
		}
		y += 15;

		// display spy skill
		if( archerPtr->spy_recno )
		{
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
			x = edit_spy_x2 = font_blue.put( edit_spy_x1=x+20, edit_spy_y1=y, spy_array[archerPtr->spy_recno]->spy_skill, 0, x2 );
			edit_spy_y2 = edit_spy_y1 + font_blue.max_font_height;
			edit_spy_enable = 1;
		}
		y += 15;
	}
	else
	{
		y += 15 * 6;
	}
}
// --------- end of function FirmMonsterFortress::disp_edit_mode -------//


// --------- begin of function FirmMonsterFortress::detect_edit_mode -------//
//
int FirmMonsterFortress::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	if( detect_soldier_list(0) )
		return 1;

	if( selected_archer_id > 0 && selected_archer_id <= archer_count
		&& !unit_array.is_deleted(archer_unit_recno[selected_archer_id-1]) )
	{
		Unit *archerPtr = unit_array[archer_unit_recno[selected_archer_id-1]];

		if( edit_hp_enable )
		{
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
			{
				archerPtr->hit_points += 10.0f;
				if( archerPtr->hit_points > (float) archerPtr->max_hit_points() )
					archerPtr->hit_points = (float) archerPtr->max_hit_points();
				return 1;
			}
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
			{
				archerPtr->hit_points -= 10.0f;
				if( archerPtr->hit_points < 1.0f )
					archerPtr->hit_points = 1.0f;
				return 1;
			}
		}

		if( edit_loyalty_enable )
		{
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, LEFT_BUTTON) )
			{
				archerPtr->loyalty += 10;
				if( archerPtr->loyalty > 100 )
					archerPtr->loyalty = 100;
				return 1;
			}
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, RIGHT_BUTTON) )
			{
				archerPtr->loyalty -= 10;
				if( archerPtr->loyalty < 0 )
					archerPtr->loyalty = 0;
				return 1;
			}
		}

		if( edit_combat_enable )
		{
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, LEFT_BUTTON) )
			{
				int newCombat = (int)archerPtr->skill.actual_combat_level(NULL) + 5;
				if( newCombat > archerPtr->skill.max_combat_level )
					newCombat = archerPtr->skill.max_combat_level;
				archerPtr->set_combat_level(newCombat);
				return 1;
			}
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, RIGHT_BUTTON) )
			{
				int newCombat = (int)archerPtr->skill.actual_combat_level(NULL) - 5;
				if( newCombat < CITIZEN_COMBAT_LEVEL )
					newCombat = CITIZEN_COMBAT_LEVEL;
				archerPtr->set_combat_level(newCombat);
				return 1;
			}
		}

		if( edit_max_combat_enable )
		{
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, LEFT_BUTTON) )
			{
				int newLevel = archerPtr->skill.max_combat_level;
				newLevel += 20;
				if( newLevel > 900 )
					newLevel = 900;
				archerPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, RIGHT_BUTTON) )
			{
				int newLevel = archerPtr->skill.max_combat_level;
				newLevel -= 20;
				if( newLevel < MAX_COMBAT_TRAIN )
					newLevel = MAX_COMBAT_TRAIN;
				archerPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
		}

		if( edit_skill_enable )
		{
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, LEFT_BUTTON) )
			{
				archerPtr->skill.inc_skill_level(5.0f);
				return 1;
			}
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, RIGHT_BUTTON) )
			{
				int newSkill = (int)archerPtr->skill.actual_skill_level(NULL) - 5;
				if( newSkill < CITIZEN_SKILL_LEVEL )
					newSkill = CITIZEN_SKILL_LEVEL;
				archerPtr->skill.set_skill_level(newSkill);
				return 1;
			}
		}

		if( edit_max_skill_enable )
		{
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, LEFT_BUTTON) )
			{
				int newLevel = archerPtr->skill.max_skill_level;
				newLevel += 10;
				if( newLevel > 200 )
					newLevel = 200;
				archerPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, RIGHT_BUTTON) )
			{
				int newLevel = archerPtr->skill.max_skill_level;
				newLevel -= 20;
				if( newLevel < MAX_SKILL_TRAIN )
					newLevel = MAX_SKILL_TRAIN;
				archerPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
		}

		if( edit_spy_enable && archerPtr->spy_recno )
		{
			Spy *spyPtr = spy_array[archerPtr->spy_recno];
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, LEFT_BUTTON) )
			{
				if( (spyPtr->spy_skill += 5) > 100 )
				{
					spyPtr->spy_skill = 100;
					return 1;
				}
			}
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, RIGHT_BUTTON) )
			{
				if( (spyPtr->spy_skill -= 5) < 10 )
				{
					spyPtr->spy_skill = 10;
					return 1;
				}
			}
		}
	}

	return 0;
}
// --------- end of function FirmMonsterFortress::detect_edit_mode -------//
