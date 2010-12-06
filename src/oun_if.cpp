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

//Filename    : OUN_IF.CPP
//Description : Unit Interface Functions

#include <oun_grp.h>
#include <key.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <omouse.h>
#include <ohelp.h>
#include <oinfo.h>
#include <ofirmres.h>
#include <oimgres.h>
#include <omonsres.h>
#include <otech.h>
#include <otechres.h>
#include <obutton.h>
#include <obutt3d.h>
#include <opower.h>
#include <ospy.h>
#include <osys.h>
#include <ogame.h>
#include <onation.h>
#include <oworld.h>
#include <oremote.h>
#include <ose.h>
#include <oseres.h>
#include <obuttcus.h>
#include <ou_mari.h>
#include <oconfig.h>
#include <osfrmres.h>
#include <oraceres.h>
#include <obitmapw.h>
#include <obitmap.h>
#include <ou_wagon.h>
#include <oitemres.h>
#include <of_camp.h>
#include <of_mfort.h>
#include <gamedef.h>
#include <oworldmt.h>
#include <ou_wagon.h>
#include <omisc.h>
#include <ot_unit.h>

//---------- Define constant ------------//

enum { UNIT_MENU_MAIN,
		 UNIT_MENU_BUILD,
		 UNIT_MENU_SET_AUTO_RETREAT,
		 UNIT_MENU_HIRE_HERO,
		 UNIT_MENU_HIRE_RESULT,
	  };

#define BURN_COMBAT_LEVEL		80

#define SPY_CLOAK_WIDTH			19

#define BUTTON_AUTO_RETREAT_COUNT	9

//---------- Define static variables ----------//
static char	last_menu_mode;
static Button3D 		button_test;
static Button3D 		button_build;
static ButtonCustom 	button_build_array[MAX_FIRM_TYPE];
static char 			button_build_flag[MAX_FIRM_TYPE];
static Button3D 		button_settle;
static Button3D 		button_assign;
static Button3D 		button_promote, button_demote;
static Button3D 		button_reward;
static Button3D 		button_return_camp;
static Button3D 		button_cancel;
static Button3D  		button_cancel2;
static Button3D 		button_resign;
static Button3D 		button_succeed_king;
static Button3D 		button_spy_notify;
static Button3D 		button_spy_drop_identity;
static Button3D		button_behavior_mode;
static Button   		button_change_color;
static Button3D		button_hire;
static Button3D		button_transform_fort;
static Button3D		button_formation[MAX_FORMATION];

//static Button			button_spy_item;
//static Button			button_drop_item;
//static Button			button_camouflage;
static Button			button_desc_item;
static Button			button_use_item;
static Button			button_set_royal;
static ButtonCustom	button_auto_retreat_array[BUTTON_AUTO_RETREAT_COUNT];
static ButtonCustom	button_auto_retreat_disabled;

static short item_menu_offset = 0;
static short last_item_menu_offset;
static char unit_menu_mode=UNIT_MENU_MAIN;
static short last_unit_recno=0;

char	Unit::hire_hero_result;
// ##### begin Gilbert 5/3 #####//
static short using_item_id;
static int last_power_command;

static short edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, edit_hp_enable;
static short edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, edit_loyalty_enable;
static short edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, edit_combat_enable;
static short edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, edit_max_combat_enable;
static short edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, edit_skill_enable;
static short edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, edit_max_skill_enable;
static short edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, edit_spy_enable;
// ##### end Gilbert 5/3 #####//

static short human_civilian_build_firm_button[] =
{
	FIRM_CAMP,
	FIRM_FORT,
	FIRM_MINE,
	FIRM_FACTORY,
	FIRM_MARKET,
	FIRM_INN,
	FIRM_RESEARCH,
	FIRM_WAR_FACTORY,
	FIRM_SPY,
	FIRM_SPECIAL,
	FIRM_BASE,
 	0,
 };
 
static short human_military_build_firm_button[] =
{
	FIRM_CAMP,
 	FIRM_FORT,
 	FIRM_WAR_FACTORY,
 	FIRM_SPY,
 	FIRM_SPECIAL,
	FIRM_OFFENSIVE_BUILDING_1,
	FIRM_OFFENSIVE_BUILDING_2,
	FIRM_OFFENSIVE_BUILDING_3,
	FIRM_OFFENSIVE_BUILDING_4,
 	0,
};

static short monster_build_firm_button[] =
{
	FIRM_LAIR,
	FIRM_BEE,
	FIRM_TERMITE,
	FIRM_LISHORR,
	FIRM_WILDE_LISHORR,
	FIRM_ALCHEMY,
	FIRM_INCUBATOR,
	FIRM_ANIMAL,
// ##### begin Gilbert 23/12 #######//
//	FIRM_FORTRESS,
// ##### end Gilbert 23/12 #######//
	FIRM_MAGIC,
	0,
};
 
//--------- Declare static functions ---------//

static void disp_auto_menu_button(ButtonCustom *, int);
static void disp_debug_info(Unit* unitPtr, int dispY1, int refreshFlag);
static void disp_firm_button(ButtonCustom *, int);

static void group_resign();
static void group_change_behavior_mode();
static void group_reward();
static void group_change_spy_notify_flag();
static void group_drop_spy_identity();

//--------- Begin of function Unit::disp_info ---------//
//
void Unit::disp_info(int refreshFlag)
{
	if( sprite_recno != last_unit_recno )
	{
		last_menu_mode = unit_menu_mode  = UNIT_MENU_MAIN;
		last_unit_recno = sprite_recno;
	}
	else
	{
		if( last_menu_mode != unit_menu_mode )		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = unit_menu_mode;
		}
	}

#ifdef USE_FLIP
	vga.use_back();
#endif

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );
	
	switch( unit_menu_mode )
	{
		case UNIT_MENU_MAIN:
			// ##### begin Gilbert 6/3 #######//
			if( refreshFlag == INFO_REPAINT || last_power_command != power.command_id )
			{
				refreshFlag = INFO_REPAINT;
				last_power_command = power.command_id;
			}
			// ##### end Gilbert 6/3 #######//
			switch( power.command_id )
			{
				case COMMAND_BUILD_FIRM:
					disp_build(refreshFlag);
					break;

				case COMMAND_SETTLE:
					disp_settle(refreshFlag);
					break;

				// ####### begin Gilbert 5/3 ########//
				case COMMAND_USE_ITEM_ON_UNIT:
				case COMMAND_USE_ITEM_ON_TOWN:
				case COMMAND_USE_ITEM_ON_SPACE:
					disp_use_item_menu(refreshFlag);
					break;
				// ####### end Gilbert 5/3 ########//

				default:
					disp_main_menu(refreshFlag);
			}
			break;

		case UNIT_MENU_BUILD:
			disp_build_menu(refreshFlag);
			break;

		case UNIT_MENU_SET_AUTO_RETREAT:
			disp_auto_menu(refreshFlag);
			break;

		case UNIT_MENU_HIRE_HERO:
			disp_hire_menu(refreshFlag);
			break;

		case UNIT_MENU_HIRE_RESULT:
			disp_hire_result(refreshFlag);
			break;
	}

#ifdef USE_FLIP
	vga.use_front();
#endif

}
//----------- End of function Unit::disp_info -----------//


//--------- Begin of function Unit::detect_info ---------//
//
void Unit::detect_info()
{
	switch( unit_menu_mode )
	{
		case UNIT_MENU_MAIN:
			switch( power.command_id )
			{
				case COMMAND_BUILD_FIRM:
					detect_build();
					break;

				case COMMAND_TRANSFORM_FORTRESS:
					detect_build();
					break;

				case COMMAND_SETTLE:
					detect_settle();
					break;

				// ####### begin Gilbert 5/3 ########//
				case COMMAND_USE_ITEM_ON_UNIT:
				case COMMAND_USE_ITEM_ON_TOWN:
				case COMMAND_USE_ITEM_ON_SPACE:
					detect_use_item_menu();
					break;
				// ####### end Gilbert 5/3 ########//

				default:
					detect_main_menu();
			}
			break;

		case UNIT_MENU_BUILD:
			detect_build_menu();
			break;

		case UNIT_MENU_SET_AUTO_RETREAT:
			detect_auto_menu();
			break;

// ##### begin Gilbert 19/12 #####//
		case UNIT_MENU_HIRE_HERO:
			detect_hire_menu();
			break;

		case UNIT_MENU_HIRE_RESULT:
			detect_hire_result();
			break;
// ##### end Gilbert 19/12 #####//
	}
}
//----------- End of function Unit::detect_info -----------//

//--------- Begin of function Unit::disp_main_menu ---------//
//
void Unit::disp_main_menu(int refreshFlag)
{
	int offsetX[10] = { 150, 130, 110,  90,  70, 140, 120, 100,  80,  60 };
	int offsetY[10] = { 130, 130, 130, 130, 130, 150, 150, 150, 150, 150 };
	int value[10]   = {   1,   3,   5,   7,   9,   2,   4,   6,   8,  10 };
	
//	if (unit_res[unit_id]->unit_class == UNIT_CLASS_WEAPON)
//		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("WEAPBASE") );
//	else
	if (unit_res[unit_id]->unit_class == UNIT_CLASS_WAGON)
	{
		Unit* unitPtr = unit_array[sprite_recno];
		UnitWagon *wagonPtr = (UnitWagon *)unitPtr;
	
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("WAGBASE") );
		for(int i= 0; i < 10; i++)
		{
			if ( value[i] <= wagonPtr->population )
				info.draw_unit_icon( INFO_X1 +offsetX[i], INFO_Y1 +offsetY[i], race_res[race_id]->civilian_unit_id,
					nation_recno, INFO_X1, INFO_Y1, INFO_X2, INFO_Y2, 1 );
			if ( value[i] <= wagonPtr->spy_count )
			{
				if (wagonPtr->spy_recno[value[i]-1] && spy_array[wagonPtr->spy_recno[value[i]-1]]->true_nation_recno == nation_array.player_recno)
				{
					char* bitmapPtr = image_icon.get_ptr("U_SPY");
					vga.active_buf->put_bitmap_trans( INFO_X1 +offsetX[i] -((Bitmap *)bitmapPtr)->get_width()/2,
						INFO_Y1 +offsetY[i] -((Bitmap *)bitmapPtr)->get_height()/2 -8, bitmapPtr );

				//	world.zoom_matrix->put_bitmap_offset( cur_x, cur_y, get_z(), iconPtr,
				//		-maxHitBarWidth/2 - ((Bitmap *)iconPtr)->get_width(), 
				//		spriteInfo->max_height + (spriteInfo->loc_width + spriteInfo->loc_height) * 6 + yOffset,
				//		NULL, 0, 0);
				}
			}
		}
		vga.active_buf->put_bitmap_trans( INFO_X1+ 37, INFO_Y1 +148, image_gameif.read("MASK") );
	//	vga.active_buf->put_bitmap_half_decompress( INFO_X1, INFO_Y1, image_gameif.read("WAGBASE") );
	} 
	else
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("UNITBASE") );

//	if( is_own_spy() && button_spy_item.is_pushed )		// spy menu
	if( is_own_spy() )		// spy menu
	{
		char *spyBox = image_icon.get_ptr("SPYBOX");
		vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+113, INFO_Y1+89, spyBox, game.get_color_remap_table(nation_recno, 0) );
		if( spy_array[spy_recno]->camouflage_count > 0 )
		{
			spyBox = image_icon.get_ptr("SPYCLOAK");
			vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+113+13, INFO_Y1+89+21, spyBox, game.get_color_remap_table(nation_recno, 0) );
		}

		if (config.show_debug_info)
			font_whbl.center_put( INFO_X1+122, INFO_Y1+90, INFO_X1+154, INFO_Y1+110, 
				m.format( spy_array[spy_recno]->camouflage_count > 0 ? spy_array[spy_recno]->camouflage_count : spy_array[spy_recno]->camouflage_power,
				4 ) );
		else
		if (spy_array[spy_recno]->can_camouflage())
			font_whbl.center_put( INFO_X1+132, INFO_Y1+90, INFO_X1+144, INFO_Y1+110, "C" );

		disp_spy_menu();
		item_menu_offset = 107;
	}
	else 
		item_menu_offset = 0;
	
	if( refreshFlag == INFO_REPAINT || last_item_menu_offset != item_menu_offset )
	{
		last_item_menu_offset = item_menu_offset;
		// button_drop_item.create_text(INFO_X1+176 - item_menu_offset, INFO_Y1+144, INFO_X1+213 - item_menu_offset, INFO_Y1+164, "drop" );
		button_use_item.set_font( &font_bld );
		button_use_item.create_text(INFO_X1+127 - item_menu_offset, INFO_Y1+120, INFO_X1+164 - item_menu_offset, INFO_Y1+140, text_unit.str_item_on() ); //"on" );
	}

	if( item.id )
		disp_item_menu();
	

//	if( button_spy_item.enable_flag )
//		button_spy_item.paint();
	// ##### end Gilbert 3/11 #######//

	//	disp_basic_info(10, refreshFlag);

	// display name and newly defined title for HUMAN unit
	// display name for non-player SHIP and CARAVAN unit
	// display name and title for user and independent SHIP and CARAVAN unit
	// display name and title for OTHER unit
	// display resign button
	// display hit point for all 
	disp_unit_profile(10, refreshFlag);

	// ###### begin Gilbert 19/12 ######//
	//----if ai shown is on ??-----------------------------//

	if( should_show_info() )
	{
	//	if( unit_res[unit_id]->race_id )
			disp_unit_info();
	}


	//----if the unit belongs to player continues-----------------------------//

//	if( is_own() )
	disp_button(refreshFlag);

	// ###### end Gilbert 19/12 ######//

#ifdef DEBUG
	if( config.show_ai_info && config.show_debug_info )
		disp_debug_info(this, INFO_Y2-68, refreshFlag);
#endif
}
//----------- End of function Unit::disp_main_menu -----------//


//-------- Begin of function Unit::should_show_info ------//
int Unit::should_show_info()
{
	if( config.show_ai_info || is_own() )
		return 1;

	//---- if there is a phoenix of the player over this firm ----//

	if( nation_array.player_recno &&
		 (~nation_array)->revealed_by_phoenix(next_x_loc(), next_y_loc()) )
	{
		return 1;
	}

	return 0;
}
//---------- End of function Unit::should_show_info --------//


//--------- Begin of function Unit::detect_main_menu ---------//
//
void Unit::detect_main_menu()
{
//	if( detect_basic_info() )
//		return;

	if( detect_unit_profile() )
		return;

	// ##### begin Gilbert 3/11 #####//
//	if( button_spy_item.detect() )
//		return;

//	if( is_own_spy() && button_spy_item.is_pushed )
	if( is_own_spy() )
		detect_spy_menu();

	if( item.id )
		detect_item_menu();
	// ##### end Gilbert 3/11 #####//

	// ##### begin Gilbert 19/12 #######//
	// if( is_own() )
	detect_button();
	// ##### end Gilbert 19/12 #######//
}
//----------- End of function Unit::detect_main_menu -----------//


//--------- Begin of function Unit::disp_basic_info ---------//
//
void Unit::disp_basic_info(int dispY1, int refreshFlag)
{
	// this routine only display the name for caravan and ship
	int dispName = unit_id == UNIT_CARAVAN || unit_res[unit_id]->unit_class == UNIT_CLASS_SHIP;		// only display names for caravans and ships as their names are not displayed in the other part of the interface
	if( nation_recno )
	{
		int dispTitle = is_own() || nation_recno==0;		// only display title for own units or rebel leader of independent units
			if( dispName )
			font_snds.center_put( INFO_X1+21, INFO_Y1+3, INFO_X2-2, INFO_Y1+30, unit_name(dispTitle) );
		//------- display the nation color box -------//
	}
	if( dispName )
		font_snds.center_put( INFO_X1, INFO_Y1, INFO_X1+100, INFO_Y1+21, unit_name() );		
}
//----------- End of function Unit::disp_basic_info -----------//


//--------- Begin of function Unit::detect_basic_info ---------//
//
int Unit::detect_basic_info()
{
/*
	//--- detect pressing on the name to center the unit on the screen ---//

	if( is_visible() && mouse.single_click( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+21 ) )
	{
		world.go_loc( next_x_loc(), next_y_loc() );
		return 1;
	}

	//-------- detect resign button ----------//

	// if( is_own() && rank_id != RANK_KING )
	if( nation_recno == nation_array.player_recno && rank_id != RANK_KING )
	{
		if( button_resign.detect(KEY_DEL) )
		{
			group_resign();
			return 1;
		}
	}
*/
	return 0;
}
//----------- End of function Unit::detect_basic_info -----------//

//--------- Begin of function Unit::disp_unit_profile ---------//
//
// <int> dispY1 - the top y coordination of the info area
// <int> refreshFlag
//
void Unit::disp_unit_profile(int dispY1, int refreshFlag)
{
	
	//--------- set help parameters --------//

	if( unit_res[unit_id]->unit_class != UNIT_CLASS_GOD )
	{
		String str;
		char *relationStr, *unitNameStr;
		relationStr = unitNameStr = "";
			
		if( nation_recno && nation_array.player_recno && 
			nation_recno != nation_array.player_recno)
		{
			NationRelation* nationRelation = nation_array[nation_recno]->get_relation(nation_array.player_recno);
			relationStr = text_unit.str_unit_relation( nationRelation->status );
		}

		switch( unit_res[unit_id]->unit_class )
		{
		case UNIT_CLASS_HUMAN:
			if( unit_mode == UNIT_MODE_REBEL )
			{
				unitNameStr = unit_name(0);	// rebel is put in str
				str = text_unit.str_race_rebel_title( race_res[race_id]->adjective, rank_id );
			}
			else
			{
				unitNameStr = unit_name(1);
				if( unit_mode == UNIT_MODE_TOWN_DEFENDER )
					str = text_unit.str_race_defender( race_id, race_res[race_id]->adjective );
				else if( hero_id )
					str = text_unit.str_race_hero( race_res[race_id]->adjective );
				else if( is_civilian() )
					str = text_unit.str_race_civilian( race_id, race_res[race_id]->adjective );
				else
					str = text_unit.str_race_soldier( race_id, race_res[race_id]->adjective );
			}

#if(defined(CHINESE))
			//SXM:Risk
			font_whbl.center_put( INFO_X1 +12, INFO_Y1 +3, INFO_X2, INFO_Y1 +21,
				unitNameStr, 0, 1 );
			font_zoom.center_put( INFO_X1 +12, INFO_Y1 +18, INFO_X2, INFO_Y1 +33, 
				text_unit.str_unit_desc( relationStr, str), 0, 0 );
#else
			font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21,
				unitNameStr, 0, 1 );
			font_zoom.center_put( INFO_X1 +12, INFO_Y1 +21, INFO_X2, INFO_Y1 +33, 
				text_unit.str_unit_desc( relationStr, str), 0, 0 );
#endif
			//SXM
			break;

		case UNIT_CLASS_CARAVAN:
			font_snds.center_put( INFO_X1 +82, INFO_Y1 +15 , INFO_X1 +222, INFO_Y1 +30, 
				text_unit.str_unit_desc( relationStr, unit_name(0) ), 0, 1 );
			break;

		case UNIT_CLASS_WAGON:
			font_whbl.center_put( INFO_X1 +12, INFO_Y1 +12, INFO_X2, INFO_Y1 +30, 
				text_unit.str_race_wagon(race_res[race_id]->adjective, is_own()?name_id:0 ),
				0, 1 );
			break;

		case UNIT_CLASS_WEAPON:
		case UNIT_CLASS_MONS_WEAPON:
			{
				int dispTitle = ( is_own() || nation_recno==0 );
				//str += unit_name(dispTitle);
				//font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, str, 0, 1 );
				font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, 
					text_unit.str_unit_desc(relationStr, unit_name(dispTitle) ),
					0, 1 );
			}
			break;

		case UNIT_CLASS_GOD:
			{
				//nothing, processed in ou_godi.cpp
			}
			break;

		case UNIT_CLASS_INSECT:
		case UNIT_CLASS_ANIMAL:
			if( nation_recno )
			{
				font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, 
					text_unit.str_king_s_unit( nation_array[nation_recno]->king_name(1), unit_name(0)),
					0, 1 );
				font_whbl.center_put( INFO_X1 +12, INFO_Y1 +21, INFO_X2, INFO_Y1 +33, 
					relationStr, 0, 1 );
			}
			else
			{
				font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21, 
					unit_name(0), 0, 1 );
			}
			break;

		case UNIT_CLASS_MONSTER:
			font_whbl.center_put( INFO_X1 +12, INFO_Y1 +9, INFO_X2, INFO_Y1 +21,
				unit_name( rank_id != RANK_SOLDIER ), 0, 1 );
			font_whbl.center_put( INFO_X1 +12, INFO_Y1 +21, INFO_X2, INFO_Y1 +33, 
				text_unit.str_unit_desc(relationStr, unit_res[unit_id]->name), 0, 1 );
			break;

		default:
			err_here();
		}
	}

	//-----Displaying the resign button-------------------//
	if ( nation_recno == nation_array.player_recno && can_resign())
	{
		if( refreshFlag == INFO_REPAINT )
		{
		//	if (unit_res[unit_id]->unit_class == UNIT_CLASS_WEAPON ||
		//		 unit_res[unit_id]->unit_class == UNIT_CLASS_CARAVAN ||
		//		 unit_res[unit_id]->unit_class == UNIT_CLASS_WAGON)
		//		button_resign.create( INFO_X1+188, INFO_Y1+50, "WEAPX-UP", "WEAPX-DN" );
		//	else
		//		button_resign.create( INFO_X1+80, INFO_Y1+127, "UNITX-UP", "UNITX-DN" );
			button_resign.create( INFO_X1+13, INFO_Y1-63, "UNITX-UP", "UNITX-DN" );
			button_resign.set_help_code( "RESIGN" );
		}
		else
			button_resign.paint();
	}
//	//-------- display hit points in numbers and the hit points bar --------//

	// ####### begin Gilbert 24/3 ########//
	if( unit_res[unit_id]->unit_class != UNIT_CLASS_GOD )
	{
		disp_hit_point();
	}
	// ####### end Gilbert 24/3 ########//

	//---------------- paint the panel --------------//

	if ( (item_menu_offset == 0 || !item.id ) && unit_res[unit_id]->unit_class !=  UNIT_CLASS_WAGON
		&& unit_res[unit_id]->unit_class != UNIT_CLASS_GOD )
	{
		int needMirror;
		int temp =cur_dir;
		sprite_info->load_bitmap_res();
		SpriteStop *stopAction = sprite_info->stop_array+3;
		cur_dir = 2;
		SpriteFrame* spriteFrame = cur_sprite_frame(&needMirror);
		cur_dir = temp;
		Bitmap* bitmapPtr = (Bitmap *) sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);
		short* colorRemapTable = sprite_info->get_color_remap_table(nation_recno, 0);
			
		int x1 = INFO_X1 +60 + spriteFrame->offset_x + 7;
	//	int y1 = INFO_Y1 +82 - bitmapPtr->get_height();
		int y1 = INFO_Y1 +140 + ZOOM_LOC_X_HEIGHT + spriteFrame->offset_y;// - (sprite_info->loc_width * ZOOM_LOC_X_HEIGHT+ sprite_info->loc_height * ZOOM_LOC_Y_HEIGHT);
		y1 = y1 - ((sprite_info->max_bitmap_height + sprite_info->max_height)>>2) - (sprite_info->loc_width * ZOOM_LOC_X_HEIGHT);

		if( unit_res[unit_id]->unit_class == UNIT_CLASS_CARAVAN )
		{
			x1 = INFO_X1 +37 + spriteFrame->offset_x;
			y1 = INFO_Y1 +57 + spriteFrame->offset_y;;
		}

		int x2 = x1 + bitmapPtr->get_width() -1;
		int y2 = y1 + bitmapPtr->get_height() -1;

		int srcX1 = max(x1, INFO_X1+15)-x1;
	//	int srcY1 = max(y1, INFO_Y1)-y1;
		int srcY1 = 0;
		int srcX2 = min(x2, INFO_X2)-x1;
		int srcY2 = min(y2, INFO_Y2)-y1;

		vga.active_buf->put_bitmap_area_trans_remap_decompress(x1, y1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);
		sprite_info->free_bitmap_res();
	}
}
//----------- End of function Unit::disp_unit_profile -----------//


//--------- Begin of function Unit::detect_unit_profile ---------//
//
int Unit::detect_unit_profile()
{
	if (is_own_spy() && mouse.single_click( INFO_X1 +119, INFO_Y1 +87, INFO_X1 +158, INFO_Y1+172 ))
	{
		if( spy_array[spy_recno]->camouflage_count > 0 )
			spy_array[spy_recno]->stop_camouflage(COMMAND_PLAYER);
		else if (spy_array[spy_recno]->can_camouflage())
			spy_array[spy_recno]->start_camouflage(COMMAND_PLAYER);
	}

	// ####### begin Gilbert 24/3 #######//
	switch( unit_res[unit_id]->unit_class )
	{
	case UNIT_CLASS_GOD:
	case UNIT_CLASS_WAGON:
		if( is_visible() && mouse.single_click( INFO_X1 +12, INFO_Y1 +6, INFO_X1 +215, INFO_Y1+221 ) )
		{
			world.go_loc( next_x_loc(), next_y_loc() );
			return 1;
		}
		break;

	case UNIT_CLASS_CARAVAN:
		if( is_visible() && mouse.single_click( INFO_X1 +12, INFO_Y1 +6, INFO_X1 +217, INFO_Y1+82 ) )
		{
			world.go_loc( next_x_loc(), next_y_loc() );
			return 1;
		}
		break;

	case UNIT_CLASS_WEAPON:
	case UNIT_CLASS_HUMAN:
	case UNIT_CLASS_MONSTER:
	case UNIT_CLASS_INSECT:
	case UNIT_CLASS_ANIMAL:
	case UNIT_CLASS_MONS_WEAPON:
	//	if( is_visible() && mouse.single_click( INFO_X1 +12, INFO_Y1 +6, INFO_X1 +215, INFO_Y1+221 )) 
		if( is_visible() && mouse.single_click( INFO_X1 +12, INFO_Y1 +6, INFO_X1 +215, INFO_Y1+172 )) 
		{
			if ((item.id || is_own_spy()) && mouse.single_click( INFO_X1 +119, INFO_Y1 +87, INFO_X1 +215, INFO_Y1+172 ))
				return 0;
			else
			if ((item.id && is_own_spy()) && mouse.single_click( INFO_X1 +12, INFO_Y1 +87, INFO_X1 +215, INFO_Y1+172 ))
				return 0;
			else
			if (unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN && is_royal &&
				mouse.single_click(INFO_X2-25, INFO_Y1+5, INFO_X2-5, INFO_Y1+25) && game.is_campaign_mode())
				return 0;
			else
			if (unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN && !is_royal &&
				mouse.single_click(INFO_X2-25, INFO_Y1+64, INFO_X2-5, INFO_Y1+84) && game.is_campaign_mode())
				return 0;
			else
			{
				world.go_loc( next_x_loc(), next_y_loc() );
				return 1;
			}
		}

		if( is_visible() )
		{
			int x=INFO_X1+20, y=INFO_Y1+178;
			help.set_help( x, y+14, x+79, y+27, "ULOYALTY" );
			help.set_help( x+110, y+14, x+189, y+27, "UCOMBAT" );
			help.set_help( x+110, y+28, x+189, y+41, "ULEADER" );
			help.set_help( x, y+28, x+79, y+41, "UHPOINT" );
		}

		break;
	}
	// ####### end Gilbert 24/3 #######//

	if( nation_recno == nation_array.player_recno && rank_id != RANK_KING )
	{
		if( button_resign.detect(KEY_DEL) )
		{
			group_resign();
			return 1;
		}
	}

	return 0;
}
//----------- End of function Unit::detect_unit_profile -----------//

//----------- Begin of function Unit::disp_hit_point -----------//
void Unit::disp_hit_point()
{
	int hitPoints;
	String str;
//	int offsetX = 0;
//	int offsetY = 0;

	if( hit_points > (float)0 && hit_points < (float)1 )
		hitPoints = 1;		// display 1 for value between 0 and 1
	// ###### begin Gilbert 4/5 ########//
	else if( hit_points < (float)0 )
		hitPoints = 0;
	// ###### end Gilbert 4/5 ########//
	else
		hitPoints = (int) hit_points;

	int barType;
	switch( unit_res[unit_id]->unit_class )
	{
	case UNIT_CLASS_HUMAN:
	case UNIT_CLASS_MONSTER:
	case UNIT_CLASS_WEAPON:
	case UNIT_CLASS_MONS_WEAPON:
	case UNIT_CLASS_INSECT:
	case UNIT_CLASS_ANIMAL:
	case UNIT_CLASS_WAGON:
		barType = 0;
		break;
	default:
		barType = 1;
	}

	if ( max_hit_points() ) 
	{
		int x = INFO_X1 + 80 +20;
		int y = INFO_Y1 + 49;

		if (barType == 0 )
		{
			x += -35;
			y += -14;
		}
		
		int ratio = hitPoints *40 / (int)max_hit_points();
		int size = hitPoints *76 / (int)max_hit_points();
		int xCut = 15 + size;
		
		//106 x 35 --- 15 to 90 ie. 0 to 40
		// hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	

		char *bitmapLeft, *bitmapRight;
		if( barType == 0 )
		{
			bitmapLeft = image_spict.read(ratio < 11 ? (char*)"MTR_B2" : ratio < 40 ? (char*)"MTR_B3" : (char*)"MTR_B4");
			bitmapRight = image_spict.read("MTR_B1");
		}
		else
		{
			bitmapLeft = image_spict.read(ratio < 11 ? (char*)"MTR_10" : ratio < 40 ? (char*)"MTR_39" : (char*)"MTR_40");
			bitmapRight = image_spict.read("MTR_00");
		}

		err_when( ((Bitmap *)bitmapLeft)->get_width() != 106 );
		err_when( ((Bitmap *)bitmapLeft)->get_height() != 35 );
		err_when( xCut <= 0 );

		mouse.hide_area( x, y, x+106-1, y+35-1);
		vga.active_buf->put_bitmap_area_trans( x, y, bitmapLeft, 0, 0, xCut-1, 35-1 );
		vga.active_buf->put_bitmap_area_trans( x, y, bitmapRight, xCut, 0, 106-1, 35-1 );
		mouse.show_area();

		if ( barType == 0)
		{
		//	str = (int)hitPoints;
			font_whbl.center_put( INFO_X1 +33, INFO_Y1 +45, INFO_X1 +65, INFO_Y1 +57, m.format((int)hitPoints,1));
		//	str = (int)max_hit_points();
			font_whbl.center_put( INFO_X1 +166, INFO_Y1 +45, INFO_X1 +200, INFO_Y1 +57, m.format((int)max_hit_points(),1) );
		}
		else
		{
			str = (int)hitPoints;
			str += " / ";
			str += (int)max_hit_points();
	
			font_snds.center_put( x, y-3, x+107, y+14, str );
		}
		
	/*	else
		{
			short*	hitPointBitmap =NULL;
			int ratio = 67 - hitPoints *67 / (int)max_hit_points();
			int size = hitPoints *67 / (int)max_hit_points();
			//48 x 80 --- 9 to 76 ie. 0 to 67
			hitPointBitmap = (short *)mem_add( BitmapW::size(48, 9+size) );	
			
			vga.active_buf->put_bitmap( INFO_X1 +17, INFO_Y1 +89, image_spict.read("BOD-00") );
			vga.active_buf->read_bitmapW( INFO_X1 +17, INFO_Y1 +89, INFO_X1 +64, INFO_Y1 +97 +size, hitPointBitmap );
			
			vga.active_buf->put_bitmap( INFO_X1 +17, INFO_Y1 +89, image_spict.read("BOD-67") );
			vga.active_buf->put_bitmapW( INFO_X1 +17, INFO_Y1 +89, hitPointBitmap );
		
			mem_del( hitPointBitmap );				
			str = hitPoints *100 / (int)max_hit_points();
			str += "%";
			font_snds.center_put( INFO_X1+76, INFO_Y1+101, INFO_X1+125, INFO_Y1+120, str );
		}*/
	}
}
//----------- End of function Unit::disp_hit_point -----------//

//--------- Begin of function Unit::disp_button ---------//
//
void Unit::disp_button(int refreshFlag)
{
//	int x1 = INFO_X1 +13;
//	int y1 = INFO_Y1 +235;
//	int x2 = INFO_X1 +13;
//	int y2 = INFO_Y1 +281;

	// ###### begin Gilbert 19/12 ######//

#define BUTTON_X(i) (INFO_X1+13+BUTTON_DISTANCE*i)
#define BUTTON_Y(j) (INFO_Y1+235+(281-235)*j)

	if( refreshFlag == INFO_REPAINT)
	{
		button_succeed_king.create( BUTTON_X(1), BUTTON_Y(0), 'A', "SUCCEED" );
		button_succeed_king.visible_flag = 0;

		button_behavior_mode.create( BUTTON_X(0), BUTTON_Y(1), 'A', "AGGRESS1" );
		button_behavior_mode.visible_flag = 0;

		button_settle.create( BUTTON_X(1), BUTTON_Y(1), 'A', "SETTLE" );
		button_settle.visible_flag = 0;

		button_build.create( BUTTON_X(0), BUTTON_Y(0), 'A', "BUILD" );
		button_build.visible_flag = 0;

		if (!is_monster())
			button_promote.create( BUTTON_X(1), BUTTON_Y(0), 'A', "PROMOTE" );
		else
			button_promote.create( BUTTON_X(1), BUTTON_Y(0), 'A', "F_PROMOT" );

		if (! is_monster() )
			button_demote.create( BUTTON_X(1), BUTTON_Y(0), 'A', "DEMOTE" );
		else
			button_demote.create( BUTTON_X(1), BUTTON_Y(0), 'A', "F_DEMOTE" );

		button_promote.visible_flag = button_demote.visible_flag = 0;

		if (!is_monster())
			button_reward.create( BUTTON_X(2), BUTTON_Y(0), 'A', "REWARD" );
		else
			button_reward.create( BUTTON_X(2), BUTTON_Y(0), 'A', "F_REWARD" );
		button_reward.visible_flag = 0;
		
		if (! is_monster() )
			button_return_camp.create( BUTTON_X(2), BUTTON_Y(1), 'A', "RETCAMP" );
		else
			button_return_camp.create( BUTTON_X(2), BUTTON_Y(1), 'A', "F_RETCMP" );
		button_return_camp.visible_flag = 0;

		button_spy_notify.create( BUTTON_X(3), BUTTON_Y(1), 'A', "SPYNOTI0" );
		button_spy_notify.visible_flag = 0;
		button_spy_drop_identity.create( BUTTON_X(3), BUTTON_Y(0), 'A', "NOSPY" );
		button_spy_drop_identity.visible_flag = 0;

	//	button_set_royal.create_text(INFO_X2-72, INFO_Y1+65, INFO_X2-50, INFO_Y1+85, "R", 0);
	//	button_set_royal.enable_flag = 0;

		button_hire.create( BUTTON_X(0), BUTTON_Y(0), 'A', "HIREUNIT" );
		button_hire.visible_flag = 0;

		button_transform_fort.create( BUTTON_X(3), BUTTON_Y(0), 'A', "FORTRS-1" );
		button_transform_fort.visible_flag = 0;

		char bup[10] = "FX-U";
		char bdw[10] = "FX-D";
		for (int i = 0; i < MAX_FORMATION-1; i ++)
		{
			bup[1] = '1' + i;
			bdw[1] = '1' + i;
			button_formation[i].create( INFO_X1 +i *28 +47, INFO_Y1 -29, bup, bdw );
			button_formation[i].visible_flag = team_info == NULL ? 0 : 1;
		}
	}

	if( nation_recno == nation_array.player_recno && nation_array.player_recno )
	{
		for (int i = 0; i < MAX_FORMATION-1; i ++)
			button_formation[i].paint();
		if (team_info && team_info->formation_id)
		{
			char bdw[10] = "FX-D";
			if (team_info->formation_id < 7)
			{
				bdw[1] = '0' + team_info->formation_id;
				vga.active_buf->put_bitmap_trans_decompress( 
					INFO_X1 +(team_info->formation_id - 1) *28 +47, INFO_Y1 -29,
					image_button.read(bdw));
			}
			else
			{
				bdw[1] = '6';
				vga.active_buf->put_bitmap_trans_decompress( 
					INFO_X1 +5 *28 +47, INFO_Y1 -29,
					image_button.read(bdw));
			}
		}
		//---- if currently in the mode of selecting a unit to succeed the king ----//

		button_succeed_king.visible_flag = nation_array.player_recno 
			&& nation_recno == nation_array.player_recno 
			&& can_succeed_king();
		button_succeed_king.paint();
		
		//------- display aggressive mode button ------//
		// Weapon also need this button

		if( (button_behavior_mode.visible_flag 
			= unit_res[unit_id]->unit_class != UNIT_CLASS_WAGON) )
		{
			char iconName[] = "AGGRESS0";
			iconName[7] += behavior_mode;

			// ##### begin Gilbert 29/5 #########//
			// bee change to defensive mode on default
			// button_behavior_mode.enable_flag = !is_die_after_attack();
			// ##### end Gilbert 29/5 #########//

			button_behavior_mode.update_bitmap(iconName);		//	update_bitmap calls paint
		}
		
		// -------- display settle button --------//

		button_settle.visible_flag = is_human() && rank_id != RANK_KING;
		button_settle.paint();

		// ------- build button ----------//

		if( (button_build.visible_flag = 
			unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN
			|| unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER) )
		{
			button_build.enable_flag = 0;
			int firmId;
			for( firmId=1; firmId<=MAX_FIRM_TYPE ; firmId++ )
			{
				if( firm_res[firmId]->can_build(sprite_recno, 0) )		// 0-don't check cash
					break;
			}

			if( firmId<=MAX_FIRM_TYPE &&
				 nation_recno == nation_array.player_recno ) 	// a spy cannot build structure for another nation
			{
				button_build.enable_flag = 1;
			}
		}
		button_build.paint();

		// ------- promote/demote button ------//

		button_promote.visible_flag = button_demote.visible_flag = 0;
		// ###### begin Gilbert 24/3 #######//
		if( !button_succeed_king.visible_flag )
//			&& (unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN || 
//			unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER) )
		// ###### end Gilbert 24/3 #######//
		{
			if( rank_id==RANK_SOLDIER && skill_level() > 0 && race_id && !is_civilian() )
			{
				if(unit_array.selected_count==1)
				{
					button_promote.visible_flag = 1;
				}
			} 
			else if( rank_id == RANK_GENERAL )
			{
				if( unit_array.selected_count==1 )
				{
					button_demote.visible_flag = 1;
				}
			}
		}
		button_promote.paint();
		button_demote.paint();

		//------------ "reward" button ---------//

		button_reward.visible_flag = 0;
		// ###### begin Gilbert 24/3 #########//
		if( unit_res[unit_id]->class_info.has_loyalty )
		// ###### end Gilbert 24/3 #########//
		{
			if( nation_recno == nation_array.player_recno &&	// you can't promote your spy in other nation
				 (rank_id != RANK_KING || unit_array.selected_count > 1))
			{
				button_reward.visible_flag = 1;
			}
		}
		button_reward.paint();

		//------ "Return Camp" button -------//

		button_return_camp.visible_flag = home_camp_firm_recno 
			&& !firm_array.is_deleted(home_camp_firm_recno)
			&& firm_array[home_camp_firm_recno]->region_id == region_id();
		button_return_camp.paint();
		if( auto_retreat_hit_point && home_camp_firm_recno)
		{
			vga.active_buf->bar_alpha( button_return_camp.x1, button_return_camp.y1+15, 
				button_return_camp.x1+BUTTON_ACTION_WIDTH-1, button_return_camp.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
			font_whbl.center_put( button_return_camp.x1, button_return_camp.y1, 
				button_return_camp.x1+BUTTON_ACTION_WIDTH-1, button_return_camp.y1+BUTTON_ACTION_HEIGHT-1,
									m.format(auto_retreat_hit_point,3));
		}

		//------- spy notify button ------//

		button_spy_notify.visible_flag = 0;
		button_spy_drop_identity.visible_flag = 0;

		if( is_own_spy() )
		{
			int notifyFlag = spy_array[spy_recno]->notify_cloaked_nation_flag;
			button_spy_notify.visible_flag = 1;
			button_spy_notify.update_bitmap( notifyFlag ? (char*)"SPYNOTI1" : (char*)"SPYNOTI0" );	// update_bitmap calls paint

			button_spy_drop_identity.visible_flag = 1;
			button_spy_drop_identity.paint();
		}

		// -------- display royal button --------//

		if( unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN	&& rank_id != RANK_KING )
		{
			if (is_royal)
				button_set_royal.create_text(INFO_X2-25, INFO_Y1+5, INFO_X2-5, INFO_Y1+25, "R", 0);
			else
				button_set_royal.create_text(INFO_X2-25, INFO_Y1+64, INFO_X2-5, INFO_Y1+84, "R", 0);
			
			button_set_royal.enable_flag = game.is_campaign_mode() ? 1 : 0;
			button_set_royal.is_pushed = is_royal;
						
			if (game.is_campaign_mode())
				button_set_royal.paint();
		}

		// -------- display transform to fort button ---------//

		if( (button_transform_fort.visible_flag = unit_id == UNIT_ROCK) )
		{
			button_transform_fort.enable_flag = nation_recno && firm_res[FIRM_FORTRESS]->get_nation_tech_level(nation_recno);
			button_transform_fort.paint();
		}

		//---- display button for changing nation color scheme ----//
	//	if( sys.debug_session )
	//		button_change_color.paint_text( INFO_X1+ 78, INFO_Y1 +60, "Change Nation Color" );
	}
	else if( is_own_spy() )
	{
		// -------- display settle button --------//

		button_settle.visible_flag = 0;
	
		//------------ "reward" button ---------//

		button_reward.visible_flag = 0;
		if( unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN || 
			unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER )
		{
			if( nation_recno == nation_array.player_recno &&	// you can't promote your spy in other nation
				 rank_id != RANK_KING )
			{
				button_reward.visible_flag = 1;
			}
		}
		button_reward.paint();

		button_succeed_king.visible_flag = 0;
		button_behavior_mode.visible_flag = 0; 
		button_build.visible_flag = 0;
		button_promote.visible_flag = 0;
		button_demote.visible_flag = 0;
		button_return_camp.visible_flag = 0;
		button_spy_notify.visible_flag = 0;
		button_spy_drop_identity.visible_flag = 0;
		button_transform_fort.visible_flag = 0;
	}
	else
	{
		if( (button_hire.visible_flag = is_hero_for_hire() && nation_array.player_recno ) )
		{
			button_hire.enable_flag = can_hire_hero(nation_array.player_recno);
			button_hire.paint();
		}
	}

#undef BUTTON_X
#undef BUTTON_Y

	// ###### end Gilbert 19/12 ######//
}
//----------- End of function Unit::disp_button -----------//

//--------- Begin of function Unit::detect_button ---------//
//
void Unit::detect_button()
{
	// ###### begin Gilbert 19/12 ######//
	if( is_own() )
	{
		for (int i = 0; i < MAX_FORMATION-1; i ++)
		{
			if( button_formation[i].detect() && team_info )
			{
				se_ctrl.immediate_sound("SORTIE");
				unit_group.set( team_info->member_unit_array, team_info->member_count );
				unit_group.formation(i + 1, COMMAND_PLAYER);
			}
		}
		help.set_help( INFO_X1 +47, INFO_Y1 -29, INFO_X1 +243, INFO_Y1, "FORMTION" );
	
		//---- if currently in the mode of selecting a unit to succeed the king ----//

		if( button_succeed_king.detect() )
		{
			if( !remote.is_enable() )
			{
				(~nation_array)->succeed_king(sprite_recno);
				info.disp();
			}
			else
			{
				// packet structure : <unit recno> <nation recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_SUCCEED_KING, 2*sizeof(short));
				*shortPtr = sprite_recno;
				shortPtr[1] = nation_array.player_recno;
			}
			se_ctrl.immediate_sound("TURN_ON");			
			return;
		}

		//--------- "return camp" button ---------//

		int rc;
		if( (home_camp_firm_recno) && (rc = button_return_camp.detect(0, 0, 1)) )
		{

			se_ctrl.immediate_sound("RETURN");
			se_res.sound( cur_x_loc(), cur_y_loc(), 1, 'S', sprite_id, "ACK");
			
			if( auto_retreat_hit_point && home_camp_firm_recno)
			{
				vga.active_buf->bar_alpha( button_return_camp.x1, button_return_camp.y1+15, 
					button_return_camp.x1+BUTTON_ACTION_WIDTH-1, button_return_camp.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
				font_whbl.center_put( button_return_camp.x1, button_return_camp.y1, 
					button_return_camp.x1+BUTTON_ACTION_WIDTH-1, button_return_camp.y1+BUTTON_ACTION_HEIGHT-1,
					m.format(auto_retreat_hit_point, 3) );
			}

			se_res.far_sound(next_x_loc(), next_y_loc(), 1, 'S', sprite_id, "ACK");

			if( rc == 1 )
			{
				unit_group.set_to_selected();
				unit_group.return_camp(COMMAND_PLAYER);
				return;
			}
			if(( rc == 2 ) && ( rank_id == RANK_GENERAL || rank_id == RANK_KING ))
			{
				unit_menu_mode = UNIT_MENU_SET_AUTO_RETREAT;
				return;
			}

		}

		//------- toggle unit aggressive mode button ------//

		if( button_behavior_mode.detect() )
		{
			group_change_behavior_mode();
			se_ctrl.immediate_sound("TURN_ON");
		}

		//-------- build button --------//

		if( button_build.detect() )
		{
			unit_menu_mode = UNIT_MENU_BUILD;
			se_ctrl.immediate_sound("CONSTR");
		//	info.disp();
			return;
		}

		//-------- settle button ---------//

		if( button_settle.detect() )
		{
			power.issue_command(COMMAND_SETTLE, sprite_recno);
			se_res.far_sound(next_x_loc(), next_y_loc(), 1, 'S', sprite_id, "ACK");
			info.disp();
			return;
		}

		//-------- detect promote/demote button --------//

		if( rank_id == RANK_SOLDIER )
		{
			if( button_promote.detect() )
			{
				if(!remote.is_enable() )
				{
					set_rank(RANK_GENERAL);
				}
				else
				{
					// packet structure : <unit recno> <new rank>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_SET_RANK, 2*sizeof(short));
					*shortPtr = sprite_recno;
					shortPtr[1] = RANK_GENERAL;
				}
				se_ctrl.immediate_sound("PROMOTE");
			}
		}
		else if( rank_id == RANK_GENERAL )
		{
			if( button_demote.detect() )
			{
				if( !remote.is_enable() )
				{
					set_rank(RANK_SOLDIER);
				}
				else
				{
					// packet structure : <unit recno> <new rank>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_SET_RANK, 2*sizeof(short));
					*shortPtr = sprite_recno;
					shortPtr[1] = RANK_SOLDIER;
				}
				se_ctrl.immediate_sound("DEMOTE");
			}
		}

		//------------ detect assign to firm button ------------//

		if( button_assign.detect() )
		{
			power.issue_command(COMMAND_ASSIGN, sprite_recno);
			se_res.far_sound(next_x_loc(), next_y_loc(), 1, 'S', sprite_id, "ACK");
		}

		//------------ "reward" button ---------//

		if( button_reward.detect() )
		{
			group_reward();
			se_ctrl.immediate_sound("TURN_ON");
		}

		//------- spy notify button ------//

		if( spy_recno && button_spy_notify.detect() )
		{
			group_change_spy_notify_flag();
			se_ctrl.immediate_sound("TURN_ON");
		}

		//------- drop spy identity button ------//

		if( spy_recno && button_spy_drop_identity.detect() )
		{
			group_drop_spy_identity();
			se_ctrl.immediate_sound("TURN_ON");
		}

		//--------- detect button for changing nation color -------//

		if( sys.debug_session && button_change_color.detect() )
		{
			Nation* nationPtr = nation_array[nation_recno];

			if( ++nationPtr->color_scheme_id > MAX_COLOR_SCHEME )
				nationPtr->color_scheme_id = 1;

			nationPtr->nation_color	= game.color_remap_array[nationPtr->color_scheme_id-1].main_color;
			se_ctrl.immediate_sound("TURN_ON");
		}

		// -------- detect royal button ------//

		if( unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN	&& rank_id != RANK_KING 
			&& button_set_royal.detect() )
		{
			if (is_royal)
				is_royal = 0;
			else
			{
				int totalRoyal = 0;
				int i;
				for( i=1; i <= unit_array.size(); ++i )
				{
					if( unit_array.is_deleted(i) )
						continue;
					Unit *unitPtr = unit_array[i];
					if( unitPtr->nation_recno != nation_array.player_recno )	// throw away cloaked spy
						continue;
					if( unitPtr->rank_id == RANK_KING || !unitPtr->is_royal )
						continue;
					totalRoyal ++;
				}
			
				for( i=1; i <= firm_array.size(); ++i )
				{
					if( firm_array.is_deleted(i) )
						continue;
					Firm *firmPtr = firm_array[i];
					if( firmPtr->nation_recno != nation_array.player_recno )
						continue;
					if( firmPtr->cast_to_FirmCamp())
					{
						FirmCamp *campPtr = firmPtr->cast_to_FirmCamp();
						for( int j = 0; j < campPtr->soldier_count; ++j )
						{
							Soldier *soldierPtr = &campPtr->soldier_array[j];
							if( !soldierPtr->is_royal )
								continue;
							totalRoyal ++;
						}
					}
				}

#ifdef DEBUG
				font_whbl.center_put(INFO_X2-30, INFO_Y1+65, INFO_X2-10, INFO_Y1+85,m.format(totalRoyal),0,1);
#endif
				if (totalRoyal < MAX_ROYAL_UNIT)
					is_royal = 1;
			}
			se_ctrl.immediate_sound("TURN_ON");
		}
	
		// --- detect transform to fort button ------//

		if( button_transform_fort.detect() )
		{
			int fortressXLoc=-1, fortressYLoc;

//			short builderUnit[MAX_EXTRA_BUILDER];
//			if(FirmMonsterFortress::check_transform_from_units(sprite_recno, fortressXLoc, fortressYLoc, builderUnit))
//				FirmMonsterFortress::transform_from_units(sprite_recno, fortressXLoc, fortressYLoc, builderUnit, COMMAND_PLAYER);
//			else
//				power.issue_command(COMMAND_TRANSFORM_FORTRESS, sprite_recno, FIRM_FORTRESS);

			// ###### begin Gilbert 23/3 ######//
			if( nation_array[nation_recno]->cash >= firm_res[FIRM_FORTRESS]->setup_cost 
				&& nation_array[nation_recno]->live_points >= firm_res[FIRM_FORTRESS]->setup_live_points_cost
				&& unit_array.selected_count >= 4 )
			{
				// count the at least 4 grokken

				int grokkenCount = 0;
				for( int i = 1; i <= unit_array.size(); ++i )
				{
					Unit *unitPtr;
					if( !unit_array.is_deleted(i)
						&& (unitPtr = unit_array[i])
						&& unitPtr->selected_flag
						&& unitPtr->nation_recno == nation_recno
						&& unitPtr->unit_id == unit_id )
					{
						if( ++grokkenCount >= 4 )
							break;
					}
				}

				if( grokkenCount >= 4 )
				{
					power.issue_command(COMMAND_TRANSFORM_FORTRESS, sprite_recno, FIRM_FORTRESS);
				}
			}
			se_ctrl.immediate_sound("TURN_ON");
			// ###### end Gilbert 23/3 ######//
		}

		if (unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN && is_royal)
		{
			help.set_help(INFO_X2-25, INFO_Y1+5, INFO_X2-5, INFO_Y1+25, "ROYALOFF");
		}	
		else
		if (unit_res[unit_id]->unit_class == UNIT_CLASS_HUMAN && !is_royal)
		{
			help.set_help(INFO_X2-25, INFO_Y1+64, INFO_X2-5, INFO_Y1+84, "ROYAL-ON");
		}
	}
	else	// not is_own()
	{
		// --- detect hire button ------//

		if( button_hire.detect() )
		{
			unit_menu_mode = UNIT_MENU_HIRE_HERO;
			se_ctrl.immediate_sound("TAXGRANT");
			return;
		}
	}
	// ###### end Gilbert 19/12 ######//

}
//----------- End of function Unit::detect_button -----------//


//----- Begin of static function group_change_behavior_mode -----//
//
static void group_change_behavior_mode()
{
	Unit* unitPtr = unit_array[unit_array.selected_recno];

	char newBehaviorMode = unitPtr->behavior_mode+1;

	if( newBehaviorMode > UNIT_STAND_GROUND )
		newBehaviorMode = UNIT_AGGRESSIVE;

	if( newBehaviorMode == UNIT_AGGRESSIVE )
		se_ctrl.immediate_sound("TURN_ON");
	else
		se_ctrl.immediate_sound("TURN_OFF");

	//-------- update icon ---------//

//	char iconName[] = "AGGRESS0";
//	iconName[7] += newBehaviorMode;
//	button_behavior_mode.update_bitmap( iconName );

	//------ group change behavior mode -------//

	short *shortPtr = NULL;
	int unitCount = 0;
	if( remote.is_enable() )
	{
		// packet structure : <new aggressive mode> <unit count> <unit recno> ...
		shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_CHANGE_AGGRESSIVE_MODE, (unit_array.selected_count+4)*sizeof(short));
		// reserve two short more in case selected_recno is slightly smalled then the actual selected
		*shortPtr = newBehaviorMode;
	}

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		//------ if this is a player spy --------//

		// ####### begin Gilbert 29/5 ########//
		// if( unitPtr->selected_flag && unitPtr->is_own() && !unitPtr->is_die_after_attack() )
		if( unitPtr->selected_flag && unitPtr->is_own() )
		// ####### end Gilbert 29/5 ########//
		{
			if( !remote.is_enable() )
			{
				unitPtr->behavior_mode = newBehaviorMode;
			}
			else
			{
				shortPtr[unitCount+2] = i;
			}
			++unitCount;
		}
	}

	if( remote.is_enable() )
	{
		err_when( unitCount > unit_array.selected_count );
		shortPtr[1] = unitCount;
	}
}
//------ End of static function group_change_behavior_mode ----//


//----- Begin of static function group_resign -----//
//
static void group_resign()
{
	Unit* unitPtr;

	//------ group chaning spy notify flag -------//

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		//------ if this is a player spy --------//

		if( unitPtr->selected_flag && unitPtr->is_own() && unitPtr->can_resign())
			unitPtr->resign(COMMAND_PLAYER);
	}
}
//------ End of static function group_resign ----//


//----- Begin of static function group_reward -----//
//
static void group_reward()
{
	Unit* unitPtr;

	//------ group chaning spy notify flag -------//

	short *shortPtr = NULL;
	int unitCount = 0;
	if( remote.is_enable() )
	{
		// packet structure : <rewarding nation> <unit count> <unit recno> ...
		shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_REWARD, (unit_array.selected_count+4)*sizeof(short));
		// reserve two short more in case selected_recno is slightly smalled then the actual selected
		shortPtr[0] = nation_array.player_recno;
	}

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		//------ if this is a player spy --------//

		if( unitPtr->selected_flag && unitPtr->is_own()
			&& unit_res[unitPtr->unit_id]->class_info.has_loyalty
			&& unitPtr->rank_id != RANK_KING )
		{
			if( !remote.is_enable() )
				unitPtr->reward(nation_array.player_recno);
			else
			{
				shortPtr[unitCount+2] = i;
			}
			++unitCount;
		}
	}

	if( remote.is_enable() )
	{
		err_when( unitCount > unit_array.selected_count );
		shortPtr[1] = unitCount;
	}

	se_ctrl.immediate_sound("TURN_ON");
}
//------ End of static function group_reward ----//


//----- Begin of static function group_change_spy_notify_flag -----//
//
static void group_change_spy_notify_flag()
{
	Unit* unitPtr = unit_array[unit_array.selected_recno];
	Spy* spyPtr = spy_array[ unitPtr->spy_recno ];
	char newNotifyFlag = !spyPtr->notify_cloaked_nation_flag;

	//------ group chaning spy notify flag -------//

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		//------ if this is a player spy --------//

		if( unitPtr->selected_flag && unitPtr->is_own_spy() )
		{
			if( !remote.is_enable() )
			{
				spy_array[unitPtr->spy_recno]->notify_cloaked_nation_flag = newNotifyFlag;
			}
			else
			{
				// packet structure : <spy recno> <new notify_cloaked_nation_flag>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CHANGE_NOTIFY_FLAG, sizeof(short)*2);
				*shortPtr = unitPtr->spy_recno;
				shortPtr[1] = newNotifyFlag;
			}
		}
	}

	//--------- update the spy bitmap ----------//

	button_spy_notify.update_bitmap( newNotifyFlag ? (char*)"SPYNOTI1" : (char*)"SPYNOTI0" );

	if( newNotifyFlag )
		se_ctrl.immediate_sound("TURN_ON");
	else
		se_ctrl.immediate_sound("TURN_OFF");
}
//------ End of static function group_change_spy_notify_flag ----//


//------ Begin of static function group_drop_spy_identity -------//
//
static void group_drop_spy_identity()
{
	Unit* unitPtr;

	//------ group drop spy identity -------//

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		//------ if this is a player spy --------//

		if( unitPtr->selected_flag && unitPtr->is_own_spy() )
		{
			if( !remote.is_enable() )
			{
				spy_array[unitPtr->spy_recno]->drop_spy_identity();
			}
			else
			{
				// packet structure : <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_DROP_IDENTITY, sizeof(short));
				shortPtr[0] = unitPtr->spy_recno;
			}
		}
	}

	se_ctrl.immediate_sound("TURN_OFF");
}
//------- End of static function group_drop_spy_identity -------//


//--------- Begin of function Unit::disp_build_menu ---------//
//
void Unit::disp_build_menu(int refreshFlag)
{
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	int firmId, x=INFO_X1+23, y=INFO_Y1+8;
	String str;

	//----- get the list of buttons for different firm types ----//
 	short* buildFirmButton;

	if( is_human() )
 	{
		if( is_civilian() )
 			buildFirmButton = human_civilian_build_firm_button;
		else
			buildFirmButton = human_military_build_firm_button;
 	}
 	else
 	{
 		buildFirmButton = monster_build_firm_button;
 	}
 
	//---------- paint buttons --------//

	for( int i = 0 ; buildFirmButton[i] ; ++i )
	{
		firmId = buildFirmButton[i];
		FirmInfo* firmInfo = firm_res[firmId];

		//----- see if there is a firm build available for the given race/monster type -----//

		int firmBuildId;

		if( is_monster() )
			firmBuildId = firmInfo->get_build_id( monster_res[monster_id()]->firm_build_code );
		else
			firmBuildId = firmInfo->get_build_id( race_res[race_id]->code );

		if( !firmBuildId )
			continue;

		if( firmId >= FIRM_OFFENSIVE_BUILDING_1 && firmId <= FIRM_OFFENSIVE_BUILDING_4 )
		{
			int techId = firmId - FIRM_OFFENSIVE_BUILDING_1 + TECH_OFFENSIVE_STRUCTURE_1;
			if( !tech_res[techId]->available_level
				&& (!nation_recno || !firm_res[firmId]->get_nation_tech_level(nation_recno)) )
				continue;
		}

		//--------------------------------------//

		if( refreshFlag == INFO_REPAINT )
		{
			button_build_array[i].create( x, y, x+187, y+17,
				disp_firm_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0),
				((int)race_id<<16) | firmId ));

			y += 18;
		}

		if( firmInfo->can_build(sprite_recno) )
		{
			button_build_array[i].enable_flag = 1;
			button_build_flag[i] = 1;
		}
		else
		{
			button_build_array[i].enable_flag = 0;
			button_build_flag[i] = 0;
		}

		button_build_array[i].paint();
	}

	if ( refreshFlag == INFO_REPAINT )
		button_cancel.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );

	button_cancel.paint();
}
//----------- End of function Unit::disp_build_menu -----------//

//--------- Begin of function Unit::detect_build_menu ---------//
//
void Unit::detect_build_menu()
{
 	short* buildFirmButton;
  
  	//----- get the list of buttons for different firm types ----//
 
 	if( is_human() )
 	{
 		if( is_civilian() )
 			buildFirmButton = human_civilian_build_firm_button;
 		else
 			buildFirmButton = human_military_build_firm_button;
 	}
 	else
 	{
 		buildFirmButton = monster_build_firm_button;
  	}


	//----------- detect build buttons ------------//

	int firmId, rc=0;

	for( int i=0; buildFirmButton[i] ; i++ )
	{
		firmId = buildFirmButton[i];

		//---- check if the unit is the right race to build this firm ----//
		if( button_build_flag[i] && firm_res[firmId]->can_build(sprite_recno) )
		{
			if( button_build_array[i].detect() )
			{
				se_ctrl.immediate_sound("CONSTR");
				power.issue_command(COMMAND_BUILD_FIRM, sprite_recno, firmId);
				rc = 1;
				break;
			}
		}
	}

	//---------- detect cancel button ----------//

	if( button_cancel.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		rc = 1;
	}

	if( rc )
	{
		unit_menu_mode = UNIT_MENU_MAIN;
		info.disp();
	}
}
//----------- End of function Unit::detect_build_menu -----------//


//--------- Begin of function Unit::disp_build ---------//
//
// Display the info when the player has selected the type of
// structure to build.
//
void Unit::disp_build(int refreshFlag)
{
//	String str;
//	str  = translate.process("Please select a location to build the ");
//	str += firm_res[power.command_para]->name;
//	str += ".";
//	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+25, INFO_X1 +216, INFO_Y1 +79, str );

	char *buildCode;
	if( race_id )
	{
		if( is_monster() )
			buildCode = monster_res[monster_id()]->firm_build_code;
		else
			buildCode = race_res[race_id]->code;
	}
	else
		err_here();

	int buildId = firm_res[power.command_para]->get_build_id( buildCode );
	
	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+25, INFO_X1 +216, INFO_Y1 +79, 
		text_unit.str_select_build( buildId ? 
		firm_res.get_build(buildId)->name : 
		firm_res[power.command_para]->name ) );

//	str  = translate.process("cost ");
//	str += m.format(firm_res[power.command_para]->setup_cost,2);
//	str += " + ";
//	str += m.format(firm_res[power.command_para]->setup_live_points_cost,1);
//	str += " life points";
	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+60, INFO_X1 +216, INFO_Y1 +80,
		text_unit.str_build_cost( 
		firm_res[power.command_para]->setup_cost, 
		firm_res[power.command_para]->setup_live_points_cost) );
	 
	if ( refreshFlag == INFO_REPAINT )
//		button_cancel2.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
		button_cancel2.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );
	button_cancel2.paint();	
}
//----------- End of function Unit::disp_build -----------//


//--------- Begin of function Unit::detect_build ---------//
//
void Unit::detect_build()
{
	if( button_cancel2.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		power.command_id = 0;
	}
}
//----------- End of function Unit::detect_build -----------//


//--------- Begin of function Unit::disp_settle ---------//
//
// Display the info when the player has selected the type of
// structure to build.
//
void Unit::disp_settle(int refreshFlag)
{
//	String str;
//	str  = translate.process("Please select a location to settle.");
	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+25, INFO_X1 +216, INFO_Y1 +79, 
		text_unit.str_select_settle() );

	if ( refreshFlag == INFO_REPAINT )
//		button_cancel2.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
		button_cancel2.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );
	button_cancel2.paint();	
}
//----------- End of function Unit::disp_settle -----------//

//--------- Begin of function Unit::detect_settle ---------//
//
void Unit::detect_settle()
{
	if( button_cancel2.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		power.command_id = 0;
	}
}
//----------- End of function Unit::detect_settle -----------//

//--------- Begin of function Unit::disp_unit_info ---------//
//
// Display the skill information of the people in the town.
//
// <int> 	dispY1		 - the top y coordination of the info area
// <int>    refreshFlag  - refresh flag
//
void Unit::disp_unit_info()
{
	#ifdef DEBUG
		if(debug2_enable_flag)
		{
			// ###### begin Gilbert 10/12 #######//
			// paramete to fon_san.field is too short 
			if(unit_res[unit_id]->unit_class == UNIT_CLASS_MONSTER)
			{
				int x = INFO_X1 +14, y = INFO_Y1 +236;
//				font_san.field( x, y, " " , x+2, sprite_recno, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+20, y, " " , x+22, next_x_loc(), 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+50, y, " " , x+52, next_y_loc(), 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+70, y, " " , x+72, nation_recno, 1, INFO_X2-2, INFO_REPAINT);

//				font_san.field( x+100, y, " " , x+102, cur_order.mode, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+120, y, " " , x+122, cur_order.para, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+140, y, " " , x+142, cur_order.loc_x, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+160, y, " " , x+162, cur_order.loc_y, 1, INFO_X2-2, INFO_REPAINT);
				y-=20;
//				font_san.field( x+100, y, " " , x+102, pushed_order.mode, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+120, y, " " , x+122, pushed_order.para, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+140, y, " " , x+142, pushed_order.loc_x, 1, INFO_X2-2, INFO_REPAINT);
//				font_san.field( x+160, y, " " , x+162, pushed_order.loc_y, 1, INFO_X2-2, INFO_REPAINT);
				y-=20;
//				font_san.field( x+160, y, " " , x+162, cur_action, 1, INFO_X2-2, INFO_REPAINT);
			}
			// ###### end Gilbert 10/12 #######//
		}
	#endif

	//--------------------------------------------//

//	if( unit_res[unit_id]->race_id==0 )
//		return;

	String str;
	int x2;

	UnitInfo *unitInfo = unit_res[unit_id];

	if (unitInfo->unit_class == UNIT_CLASS_WAGON)
	{
		str = text_unit.str_loyalty(); // "Loyalty ";
		str += " ";
		str += m.format((int)loyalty, 4);
		font_whbl.center_put(INFO_X1 +12, INFO_Y1 +60, INFO_X2, INFO_Y1 +80, str );
		return;
	}

	//--------- display loyalty ---------//
	if( rank_id != RANK_KING && (nation_recno || spy_recno) && unitInfo->class_info.has_loyalty )
	{
		if( is_own_spy() )	// only display spy loyalty instead of unit loyalty if this is a spy and this spy is ours
		{
		//	info.disp_loyalty( INFO_X1+20, INFO_Y1+208, INFO_X1+80, spy_array[spy_recno]->spy_loyalty, spy_array[spy_recno]->spy_loyalty, true_nation_recno(), INFO_REPAINT );
			info.disp_loyalty( INFO_X1+20, INFO_Y1+192, INFO_X2-99 - font_snds.text_width(m.format(spy_array[spy_recno]->spy_loyalty, 4)),
							spy_array[spy_recno]->spy_loyalty, spy_array[spy_recno]->spy_loyalty, true_nation_recno(), INFO_REPAINT );
		}
		else if( nation_recno )
		{
		//	info.disp_loyalty( INFO_X1+20, INFO_Y1+208, INFO_X1+80, (int)loyalty, (int)target_loyalty, nation_recno, INFO_REPAINT );
			if ((int)loyalty != (int)target_loyalty)
				info.disp_loyalty( INFO_X1+20, INFO_Y1+192, INFO_X2-99 - font_snds.text_width(m.format((int)loyalty, 4)) -
							font_snds.text_width(m.format((int)target_loyalty, 4)) -
							font_snds.text_width("11"),
							(int)loyalty, (int)target_loyalty, nation_recno, INFO_REPAINT );							
			else
				info.disp_loyalty( INFO_X1+20, INFO_Y1+192, INFO_X2-99 - font_snds.text_width(m.format((int)loyalty, 4)),
							(int)loyalty, (int)target_loyalty, nation_recno, INFO_REPAINT );							
		}
	}
	else
	{
	//	str = "Loyalty ??";
	//	font_snds.put( INFO_X1+20, INFO_Y1 +182, str, 1, INFO_X1+111 );
	}

	//--------- display combat level ----------//

//	if ( race_id )
	if( unitInfo->class_info.has_combat_level )
	{
		x2 = font_snds.put( INFO_X1+130, INFO_Y1+192, text_unit.str_combat_level() ); // "Combat" );
		font_snds.right_put( INFO_X2-10, INFO_Y1+192, m.format(combat_level(),4) );
	//	font_snds.put( x2+10, INFO_Y1+194, combat_level() );
	}

	//-------- display skill level --> leadership ---------//

	if ( unitInfo->class_info.has_skill_level && skill_level() > 0 )
	{
		x2 = font_snds.put( INFO_X1+130, INFO_Y1+206, text_unit.str_leadership() ); //"Leadership" );
		font_snds.right_put( INFO_X2-10, INFO_Y1+206, m.format(skill_level(),4) );
	//	font_snds.put( x2+10, INFO_Y1+208, skill_level() );
	}

	//-------- display hit point ---------//

	if( max_hit_points() )
	{
		x2 = font_snds.put( INFO_X1+20, INFO_Y1+206, text_unit.str_hit_points() ); // "Hit Points" );
		int hitPoints;
		if( hit_points > (float)0 && hit_points < (float)1 )
			hitPoints = 1;		// display 1 for value between 0 and 1
		else
			hitPoints = (int) hit_points;

		str = m.format(hitPoints, 4);
		str += "/";
		str += m.format(max_hit_points(), 4);
		font_snds.right_put( INFO_X2-100, INFO_Y1+206, str );
	}

	if( is_own_spy() )
	{
		x2 = font_snds.put( INFO_X1+130, INFO_Y1+178, text_unit.str_spy_skill() ); // "Spying" );
		font_snds.right_put( INFO_X2-10, INFO_Y1+178, m.format(spy_array[spy_recno]->spy_skill,4) );
	//	font_snds.put( x2+10, INFO_Y1+178, spy_array[spy_recno]->spy_skill);
	}
		
	
/*	//--------- display debug info ---------//

	str = "Contribution";
	x2 = font_snds.put( INFO_X1+20, INFO_Y1+182, str ) +10;
		
	if( !is_civilian() && rank_id != RANK_KING )
		str = nation_contribution;
		
	else
		str = "??";

	font_snds.put( x2, INFO_Y1+182, str );*/
}
//----------- End of function Unit::disp_unit_info -----------//



//--------- Begin of function Unit::disp_spy_menu ---------//
//
void Unit::disp_spy_menu()
{
	err_when( !spy_recno );

	static char  lastCanChangeCloak;
	static short lastNationCount;

	char  canChangeAnyCloak = can_spy_change_nation();
	
	//-- if the spy can always change back to its original color. So if it is currently cloaked as another nation, it should be able to revert back any time ---//

	char canChangeOwnCloak = canChangeAnyCloak;		// change back to its own cloak

	if( true_nation_recno() != nation_recno )
		canChangeOwnCloak = 1;

	//---------------------------------------------//

	if( canChangeOwnCloak != lastCanChangeCloak ||
	    nation_array.nation_count != lastNationCount )
	{
		lastCanChangeCloak = canChangeOwnCloak;
		lastNationCount	 = nation_array.nation_count;
		info.disp();
		return;
	}

	//---- if enemy nearby and cannot change cloak right now ---//

	if( !canChangeOwnCloak )
	{
		font_whbl.put_paragraph( INFO_X1+128, INFO_Y1+104, INFO_X1+217, INFO_Y1+165, 
			text_unit.str_no_change_cloak() );  // "Enemies Nearby !! Cannot change cloak." );
		return;
	}

	//-------------------Display Cloak for selection--------------------------//
	font_whbl.center_put( INFO_X1+158, INFO_Y1+150, INFO_X1+217, INFO_Y1+162, text_unit.str_spy_cloak() ); //"Spy Cloak:" );

	char *nationPict1 = image_spict.get_ptr("COL_UP");
	char *nationPict2 = image_spict.get_ptr("COL_DWN");

	Nation* nationPtr = nation_array[true_nation_recno()];

	int x=INFO_X1+158, y=INFO_Y1+92;
	int addedCount =-1;

	for( int i=0 ; i<=nation_array.size(); i++ )
	{
		//---- Checking when to stop painting the cloak icons------//
		if( canChangeAnyCloak )
		{
			if( i > 0 && i <= nation_array.size() )		// exclude independent town
			{
				if( nation_array.is_deleted(i) || !nationPtr->get_relation(i)->has_contact )
					continue;
			}
		}
		else if( canChangeOwnCloak )
		{
			if( i!=nation_recno && i!=true_nation_recno() )	// only display the current cloaked nation and its true nation
				continue;
		}
		else
			err_here();
		//------------------Displaying the cloaks------------------//
		addedCount ++;
		if (addedCount == 6)
			x = x + 9;
		// nation_recno == tells this unit's nation 
		if( i == nation_recno  )
			vga.active_buf->put_bitmap_trans_remap_decompress( x +( addedCount %3 )*SPY_CLOAK_WIDTH, y +( addedCount /3 )*SPY_CLOAK_WIDTH, nationPict2, game.get_color_remap_table(i, 0) );		
		else
			vga.active_buf->put_bitmap_trans_remap_decompress( x +( addedCount %3 )*SPY_CLOAK_WIDTH, y +( addedCount /3 )*SPY_CLOAK_WIDTH, nationPict1, game.get_color_remap_table(i, 0) );		
	}
}
//----------- End of function Unit::disp_spy_menu -----------//


//--------- Begin of function Unit::detect_spy_menu ---------//
//
void Unit::detect_spy_menu()
{
	Nation* nationPtr = nation_array[true_nation_recno()];

	int x=INFO_X1+158, y=INFO_Y1+92, nationRecno, changeFlag=0;

	char canChangeAnyCloak = can_spy_change_nation();
	char canChangeOwnCloak = canChangeAnyCloak;		// change back to its own cloak

	if( true_nation_recno() != nation_recno )
		canChangeOwnCloak = 1;

	if( !canChangeOwnCloak )
		return;

	int addedCount =-1;
	int i;
	for( i=0 ; i<=nation_array.size() ; i++ )
	{
		nationRecno = i;
		//---- Checking when to stop painting the cloak icons------//
		if( canChangeAnyCloak )
		{
			if( i > 0 && i <= nation_array.size() )		// exclude independent town
			{
				if( nation_array.is_deleted(i) || !nationPtr->get_relation(i)->has_contact )
					continue;
			}
		}
		else if( canChangeOwnCloak )
		{
			if( i!=nation_recno && i!=true_nation_recno() )	// only display the current cloaked nation and its true nation
				continue;
		}
		else
			err_here();
			//------------------Displaying the cloaks------------------//
		addedCount ++;
		if (addedCount == 6)
			x = x + 9;

		if( mouse.single_click(x +( addedCount %3 )*SPY_CLOAK_WIDTH, y +( addedCount /3 )*SPY_CLOAK_WIDTH,
									  x +( addedCount %3 +1 )*SPY_CLOAK_WIDTH -1, y +( addedCount /3 +1 )*SPY_CLOAK_WIDTH -1) )
		{
			changeFlag=1;
			break;
		}
	}

	if( !changeFlag )
		return;

	//--- group changing the cloaks of all of your spies currently selected ---//

	Unit* unitPtr;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->is_own_spy() && unitPtr->selected_flag )
			unitPtr->spy_change_nation(nationRecno, COMMAND_PLAYER);
	}
	disp_spy_menu();
}
//----------- End of function Unit::detect_spy_menu -----------//


// ---------- begin of static function disp_firm_button --------//
static void disp_firm_button(ButtonCustom *button, int)
{
	// ####### begin Gilbert 20/11 #####//
//	int firmId = (button->custom_para.value >> 5);
//	int raceId = (button->custom_para.value & 31);
	int firmId = button->custom_para.value & 0xff;
	int raceId = button->custom_para.value >> 16;
	int firmBuild = firm_res[firmId]->get_build_id(raceId);
	char *firmBuildName = firmBuild ? firm_res.get_build(firmBuild)->name : firm_res[firmId]->name;

	if ( !button->enable_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, firmBuildName );
		//vga_back.adjust_brightness( button->x1+1, button->y1+1, button->x2-1, button->y2-1, -5 );
		vga.active_buf->bar_alpha(button->x1+1, button->y1+1, button->x2-1, button->y2-1, 1, 0);
	}
	else if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDWN") );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1+1, button->y1-1, button->x2+1, button->y2-1, firmBuildName );
	}
	else if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSEL") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, firmBuildName );
	}
	else
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, firmBuildName );
	}
	// ####### end Gilbert 20/11 #####//
}
// ---------- end of static function disp_firm_button --------//


#ifdef DEBUG

//----------- Begin of static function disp_debug_info -----------//

static void disp_debug_info(Unit* unitPtr, int dispY1, int refreshFlag)
{
	if( unitPtr->is_own_spy() )
		return;		// don't display debug info as it will overlap with the buttons
	
	//---------------------------------------//

	static char* action_mode_str_array[] =
	{
		"Stop",
		"Move",
		"Attack",
		"Build Firm",
		"Settle Town",
		"Assign",
		"Go Cast Power",
		"Transform Fortress",
	};

	static char* unit_mode_str_array[] =
	{
		"",
		"Oversee",
		"Defend town",
		"Defend camp",
		"Construct Firm",
		"Construct Town",
		"Rebel",
		"Monster",
		"On Ship",
		"In Harbor",
	};

	static char* cur_action_str_array[] =
	{
		"Idle",
		"Ready to move",
		"Move",
		"Wait",
		"Attack",
		"Turn",
		"Ship extra move",
		"Die",
	};

	int x=INFO_X1+13, y=INFO_Y1+235;

	font_san.disp( INFO_X2-80, y   , unitPtr->sprite_recno, 1, INFO_X2-41 );
	font_san.disp( INFO_X2-80, y+16, unitPtr->cur_order.ai_action_id, 1, INFO_X2-41 );
	font_san.disp( INFO_X2-80, y+32, unitPtr->home_camp_firm_recno, 1, INFO_X2-41 );

	font_san.disp( INFO_X2-40, y   , unitPtr->leader_unit_recno, 1, INFO_X2-3 );

	font_san.disp( x, y	  , action_mode_str_array[unitPtr->cur_order.mode] , INFO_X2-81 );
	if(unitPtr->pushed_order.mode >= 0)
		font_san.disp( x, y+=16, action_mode_str_array[unitPtr->pushed_order.mode], INFO_X2-81 );
	font_san.disp( x, y+=16, unit_mode_str_array  [unitPtr->unit_mode]   , INFO_X2-81 );
	font_san.disp( x, y+=16, cur_action_str_array [unitPtr->cur_action-1]  , INFO_X2-81 );
}
//----------- End of static function disp_debug_info -----------//

#endif


// ----- begin of function Unit::disp_item_menu ------//
//
void Unit::disp_item_menu()
{
	if( !item.id )
		return;

	if( !should_show_info() )
		return;

	if (item_menu_offset == 0)
			vga.active_buf->put_bitmap( INFO_X1+113, INFO_Y1+89, image_icon.read("ITEMBOX") );
//	vga.active_buf->bar( INFO_X1+127, INFO_Y1+90, INFO_X1+127+92-1, INFO_Y1+90+73-1, V_BLACK);

	char *iconBitmap = item_res.item_icon(item.id);
	if( iconBitmap )
	{
		vga.active_buf->put_bitmap( INFO_X2-item_menu_offset-ITEM_ICON_WIDTH-10, INFO_Y1+90+16, iconBitmap );
	}

	font_whbl.put_paragraph( INFO_X1+127+3 - item_menu_offset, INFO_Y1+94, INFO_X1+218 - item_menu_offset, INFO_Y1+140,
		item.item_name() );
	
	if (is_own())
		font_whbl.put_paragraph( INFO_X1+127+2 - item_menu_offset, INFO_Y1+140, INFO_X1+218 - item_menu_offset, INFO_Y1+177,
			text_unit.str_item_drop() ); // "Right click to drop" );

	if( config.show_debug_info )
		font_whbl.put( INFO_X1+127+3 - item_menu_offset, INFO_Y1+124, item.para );

	if( !is_own() )
		return;

	// button_drop_item.paint();

	//button_use_item.visible_flag = 0;
	if ( item.can_turn() )
	{
		if( !item.is_turn_on() )
		{
			button_use_item.enable_flag = item.can_turn_on();
			button_use_item.set_body( text_unit.str_item_off() ); // "off" );
		}
		else
		{
			button_use_item.enable_flag = 1;
			button_use_item.set_body( text_unit.str_item_on() ); // "on" );
		}
		// button_use_item.visible_flag = 1;
		button_use_item.paint();
	}
	else if( item.can_use_manually() )
	{
		button_use_item.enable_flag = 1;
		button_use_item.set_body( text_unit.str_item_use() ); // "use" );
		// button_use_item.visible_flag = 1;
		button_use_item.paint();
	}
}
// ----- end of function Unit::disp_item_menu ------//

// ----- begin of function Unit::detect_item_menu ------//
//
void Unit::detect_item_menu()
{
	if( !item.id )
		return;

	if( !should_show_info() )
		return;

	if( !is_own() )
		return;

	// ----- detect right click to drop button --------//

//	if( button_drop_item.detect() )
	if( mouse.single_click( INFO_X2-item_menu_offset-ITEM_ICON_WIDTH-10, INFO_Y1+90+16,
		INFO_X2-item_menu_offset-10, INFO_Y1+90+16+ITEM_ICON_HEIGHT-1, 1) )
	{
		drop_item(COMMAND_PLAYER);
	}
	else
	{
		help.set_custom_help(INFO_X2-item_menu_offset-ITEM_ICON_WIDTH-10, INFO_Y1+90+16,
			INFO_X2-item_menu_offset-10, INFO_Y1+90+16+ITEM_ICON_HEIGHT-1,
			item.item_name(), item.item_desc());
	}

//	if( button_desc_item.detect() )
//	{
//		font_whbl.put_paragraph( INFO_X1+127+3 - item_menu_offset, INFO_Y1+94, INFO_X1+218 - item_menu_offset, INFO_Y1+170,
//			item.item_desc() );
//	}


	if( (item.can_turn() || item.can_use_manually()) && button_use_item.detect() )
	{
		// NEWCODEHERE

		// ###### begin Gilbert 5/3 ####//
		if( item.can_use_manually() > 0 )		// need parameter
		{
			power.issue_command(item.can_use_manually(), sprite_recno ,item.id );
			using_item_id = item.id;
		}
		// ###### end Gilbert 5/3 ####//
		else if( !remote.is_enable() )
		{
			if ( item.can_turn() )
			{
				if( item.is_turn_on() )
					item.turn_off();
				else
					item.turn_on();
			}
			else if( item.can_use_manually() )
			{
				// ###### begin Gilbert 8/1 ######//
				// item.use_now();
				item.use_manually(sprite_recno);
				// ###### end Gilbert 8/1 ######//
			}
		}
		else
		{
			// ###### begin Gilbert 5/3 ########//
			short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNIT_USE_ITEM, 3*sizeof(short) );
			// packet structure : <sprite recno> <0>
			*shortPtr = sprite_recno;
			shortPtr[1] = 0;
			shortPtr[2] = 0;
			// ###### end Gilbert 5/3 ########//
		}
	}
}
// ----- end of function Unit::detect_item_menu ------//

//--------- Begin of function Unit::disp_auto_menu ---------//
//
void Unit::disp_auto_menu(int refreshFlag)
{
	int curAutoLoyalty = auto_retreat_hit_point;

	Nation* nationPtr = nation_array[nation_recno];

	//---------- paint buttons ------------//

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("AUTOBASE") );

//	String str;
//	String headingStr;
	int i, x=INFO_X1+23, y=INFO_Y1+100;
	
//	headingStr = "Automatically Retreat this (Left-click) or all your Generals (Right-click) to Home Fort when hit point is below :";
	
//	str = headingStr;
	font_bld.center_put_paragraph( INFO_X1+17, INFO_Y1+20, INFO_X2-7, INFO_Y1+70,
		text_unit.str_auto_retreat_inst(), 0, 0, 0 );
//	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+8, INFO_X2-5, INFO_Y2-5, str );

	for( i=0; i<BUTTON_AUTO_RETREAT_COUNT; i++ )
	{
		if ( refreshFlag == INFO_REPAINT )
			button_auto_retreat_array[i].create( x +(i/5) *94, y +(i%5) *23, x +(i/5) *94 +91, y +(i%5) *23 +20,
					disp_auto_menu_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0), i ));
		button_auto_retreat_array[i].paint();
	}
	
	i = BUTTON_AUTO_RETREAT_COUNT;
	if ( refreshFlag == INFO_REPAINT )
		button_auto_retreat_disabled.create( x +(i/5) *94, y +(i%5) *23, x +(i/5) *94 +91, y +(i%5) *23 +20,
				disp_auto_menu_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0), i ));
	button_auto_retreat_disabled.paint();

	if ( refreshFlag == INFO_REPAINT )
	//	button_cancel2.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
		button_cancel2.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );
	button_cancel2.paint();
}
//----------- End of function Unit::disp_auto_menu -----------//

// ---------- Begin of static function disp_firm_button --------//
static void disp_auto_menu_button(ButtonCustom *button, int)
{
	String str;
	int paintInfo = button->custom_para.value;
	if (paintInfo == BUTTON_AUTO_RETREAT_COUNT)
		str = text_unit.str_disable_auto(); //"DISABLE";
	else
	{
		str = m.format( paintInfo *10 + 10, 3 );		// %
	}

	if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDDW3") );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1+1, button->y1, button->x2+1, button->y2, str );
	}
	else
	if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDSE3") );
		font_bld.center_put(button->x1, button->y1-1, button->x2, button->y2-1, str );
	}
	else
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDUP3") );
		font_bld.center_put(button->x1, button->y1-1, button->x2, button->y2-1, str );
	}
}
// ---------- End of static function disp_firm_button --------//

//--------- Begin of function Unit::detect_auto_menu ---------//
//
void Unit::detect_auto_menu()
{
	int i, rc=0, hitPointPercentage;

	for( i=0, hitPointPercentage=10 ; i<BUTTON_AUTO_RETREAT_COUNT ; hitPointPercentage+=10, i++ )
	{
		rc = button_auto_retreat_array[i].detect(0, 0, 1);

		if( rc )
			break; 
	}

	if( !rc )
	{
		rc = button_auto_retreat_disabled.detect(0, 0, 1);
		hitPointPercentage = 0;
	}

	//------ set new settings now -------//

	if( rc==1 )
	{
		if( !remote.is_enable() )
		{
			set_auto_retreat(hitPointPercentage);
		}
		else
		{
			// packet structure <sprite_recno> <hitPointPercentage>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_AUTO_RETREAT, 2*sizeof(short) );
			*shortPtr = sprite_recno;
			shortPtr[1] = hitPointPercentage;
		}
	}
	else if( rc==2 )
	{
		if (rank_id == RANK_KING)
		{	
			if( !remote.is_enable() )
			{
				set_auto_retreat(hitPointPercentage);
			}
			else
			{
				// packet structure <sprite_recno> <hitPointPercentage>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_AUTO_RETREAT, 2*sizeof(short) );
				*shortPtr = sprite_recno;
				shortPtr[1] = hitPointPercentage;
			}
		}

		if( !remote.is_enable() )
		{	
			// set all generals' auto retreat
			Nation* nationPtr = nation_array[nation_recno];
			nationPtr->set_auto_retreat(hitPointPercentage);
		}
		else
		{
			// set all generals' auto retreat
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_AUTO_RETREAT, 2*sizeof(short) );
			*shortPtr = -nation_recno;
			shortPtr[1] = hitPointPercentage;
		}
	}

	//--------------------------------------//

	if( button_cancel2.detect() || rc )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		unit_menu_mode = UNIT_MENU_MAIN;
	}
}
//----------- End of function Unit::detect_auto_menu -----------//

//----- Begin of function Unit::set_auto_retreat -----//
//
void Unit::set_auto_retreat(int hitPointPercentage)
{
	auto_retreat_hit_point = hitPointPercentage;
}
//------ End of function Unit::set_auto_retreat -----//


// ####### begin Gilbert 19/12 #######//
//------ Begin of function Unit::disp_hire_menu ------//
//
void Unit::disp_hire_menu(int refreshFlag)
{
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( refreshFlag == INFO_REPAINT )
	{
		button_hire.create(INFO_X1+13, INFO_Y1+281, 'A', "HIREUNIT" );
		button_cancel2.create(INFO_X1+13 + 3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "CANCEL" );
	}

//	String str;
//	str = unit_name();
//	str += " will take your color for ";
//	str += m.format( hero_hire_cost(), 2);
//	str += "\nHire him now?";

	font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
		text_unit.str_ask_hire( unit_name(), hero_hire_cost()) );

	button_hire.enable_flag = can_hire_hero(nation_array.player_recno);
	button_hire.paint();
	button_cancel2.paint();
}
//------ End of function Unit::disp_hire_menu ------//


//------ Begin of function Unit::disp_hire_menu ------//
//
void Unit::detect_hire_menu()
{
	// clear if not available for hire (hired by other people)

	if( !is_visible() || !is_hero_for_hire() || button_cancel2.detect() )
	{
		unit_menu_mode=UNIT_MENU_MAIN;
		return;
	}

	if( button_hire.detect() )
	{
		hire_hero_result = 0;

		se_ctrl.immediate_sound("TAXGRANT");

		if( !remote.is_enable() )
		{
			hire_hero(nation_array.player_recno);
			unit_menu_mode=UNIT_MENU_MAIN;
		}
		else
		{
			// ###### begin Gilbert 25/1 ########//
			short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNIT_HIRE_HERO, 2*sizeof(short));
			shortPtr[0] = sprite_recno;
			shortPtr[1] = nation_array.player_recno;
			unit_menu_mode = UNIT_MENU_HIRE_RESULT;
			// ###### end Gilbert 25/1 ########//
		}
		return;
	}
}
//------ End of function Unit::disp_hire_menu ------//



//------ Begin of function Unit::disp_hire_result ------//
//
void Unit::disp_hire_result(int refreshFlag)
{
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel2.create(INFO_X1+13 + 3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "CANCEL" );
	}

	if( hire_hero_result == 0 )
	{
		// waiting in multiplayer
	}
	else if( hire_hero_result > 0 )	// successful, 
	{
		// display standard unit interface
		// so no text to display
	}
	else		// if fail
	{
		if( is_hero_for_hire() )
		{
			// possibily don't have enough money

			font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
				// But you do not have enough money now." );
				text_unit.str_fail_hire_cash() );
		}
		else if( nation_recno != 0 )
		{
			// hired by other nation

			font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
				// "But he has been hired by other nation." );
				text_unit.str_fail_hire_late() );
		}
		else
		{
			font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
				// "Hire failed." );
				text_unit.str_fail_hire() );
		}
	}

	button_cancel2.paint();
}
//------ End of function Unit::disp_hire_result ------//


//------ Begin of function Unit::detect_hire_result ------//

void Unit::detect_hire_result()
{
	// if successful switch to main menu immediately

	if( hire_hero_result > 0 || button_cancel2.detect() )
	{
		unit_menu_mode = UNIT_MENU_MAIN;
	}
}
//------ End of function Unit::detect_hire_result ------//
// ####### end Gilbert 19/12 #######//


// ------ Begin of function Unit::disp_use_item_menu ------//
//
void Unit::disp_use_item_menu(int refreshFlag)
{
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel2.create(INFO_X1+13 + 3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "CANCEL" );
	}

	switch( power.command_id )
	{
	case COMMAND_USE_ITEM_ON_UNIT:
		font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
			text_unit.str_use_item_on_unit() ); // "Left click on target unit" );
		break;
	case COMMAND_USE_ITEM_ON_TOWN:
		font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
			text_unit.str_use_item_on_town() ); //"Left click on target town" );
		break;
	case COMMAND_USE_ITEM_ON_SPACE:
		font_snds.put_paragraph( INFO_X1+22, INFO_Y1+18, INFO_X2-10, INFO_Y1+200, 
			text_unit.str_use_item_on_pos() ); //"Left click on target position" );
		break;
	}

	button_cancel2.paint();
}
// ------ End of function Unit::disp_use_item_menu ------//


// ------ Begin of function Unit::detect_use_item_menu ------//
//
void Unit::detect_use_item_menu()
{
	if( item.id != using_item_id || button_cancel2.detect() )
	{
		unit_menu_mode = UNIT_MENU_MAIN;
		power.reset_command();
	}
}
// ------ End of function Unit::detect_use_item_menu ------//


// ------ Begin of function Unit::disp_edit_mode -----------//
//
// return y, so that sub-class disp_edit_mode can continue at bottom
//
int Unit::disp_edit_mode(int refreshFlag)
{
	UnitInfo *unitInfo = unit_res[unit_id];

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;
	int x;
	int y = INFO_Y1 + 10;

	// display unit id name

	font_whbl.put( x1, y, unit_res[unit_id]->name, 0, x2 );
	y += 20;

	// display unit name

	font_whbl.put( x1, y, unit_name(), 0, x2 );
	y += 20;

	// display hit point

	x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
	x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)hit_points, 0, x2 );
	edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
	edit_hp_enable = 1;
	x = font_whbl.put( x, y, " / ", 0, x2 );
	x = font_whbl.put( x, y, max_hit_points(), 0, x2 );
	y += 20;

	// disp loyalty
	edit_loyalty_enable = 0;
	if( unitInfo->class_info.has_loyalty && rank_id != RANK_KING )
	{
		x = font_whbl.put( x1, y, text_unit.str_loyalty(), 0, x2 );		// "Loyalty"
		x = edit_loyalty_x2 = font_blue.put( (edit_loyalty_x1=x+20), (edit_loyalty_y1=y), loyalty, 0, x2 );
		edit_loyalty_y2 = edit_loyalty_y1 + font_blue.max_font_height;
		edit_loyalty_enable = 1;
		x = font_whbl.put( x, y, " -> ", 0, x2 );
		x = font_whbl.put( x, y, target_loyalty, 0, x2 );
	}
	y += 20;

	// display combat actual(original)
	edit_combat_enable = edit_max_combat_enable = 0;
	if( unitInfo->class_info.has_combat_level )
	{
		x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
		x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), skill.actual_combat_level(NULL), 0, x2 );
		edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
		edit_combat_enable = 1;
		if( combat_level() != skill.actual_combat_level(NULL) )
		{
			x = font_whbl.put( x, y, "(", 0, x2 );
			x = font_whbl.put( x, y, combat_level(), 0, x2 );
			x = font_whbl.put( x, y, ")", 0, x2 );
		}
		if( sys.debug_session )		// maximum combat level is a higher secret
		{
			x = font_whbl.put( x, y, " / ", 0, x2 );
			x = edit_max_combat_x2 = font_blue.put( (edit_max_combat_x1=x), (edit_max_combat_y1=y), skill.max_combat_level, 0, x2 );
			edit_max_combat_y2 = edit_max_combat_y1 + font_blue.max_font_height;
			edit_max_combat_enable = edit_combat_enable;
		}
	}
	y += 20;

	// display skill actual(original)
	edit_skill_enable = edit_max_skill_enable = 0;
	if( unitInfo->class_info.has_skill_level )
	{
		x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );	// "Leadership"
		x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), skill.actual_skill_level(NULL), 0, x2 );
		edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height - 1;
		// ###### begin Gilbert 26/5 ##########//
		edit_skill_enable = 1; //skill.actual_skill_level(NULL) >= CITIZEN_SKILL_LEVEL;
		// ###### end Gilbert 26/5 ##########//
		if( skill_level() != skill.actual_skill_level(NULL) )
		{
			x = font_whbl.put( x, y, "(", 0, x2 );
			x = font_whbl.put( x, y, skill_level(), 0, x2 );
			x = font_whbl.put( x, y, ")", 0, x2 );
		}
		if( sys.debug_session )		// maximum skill level is a higher secret
		{
			x = font_whbl.put( x, y, " / ", 0, x2 );
			x = edit_max_skill_x2 = font_blue.put( (edit_max_skill_x1=x), (edit_max_skill_y1=y), skill.max_skill_level, 0, x2 );
			edit_max_skill_y2 = edit_max_skill_y1 + font_blue.max_font_height;
			edit_max_skill_enable = edit_skill_enable;
		}
	}
	y += 20;

	// display spy skill
	if( spy_recno )
	{
		x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
		x += 20;
		// put true nation recno
		int trueNationRecno = spy_array[spy_recno]->true_nation_recno;
		if( !nation_array.is_deleted(trueNationRecno) )
		{
			char trueNationChar[] = "1";
			trueNationChar[0] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[trueNationRecno]->color_scheme_id;
			x = font_whbl.put( x, y, trueNationChar, 0, x2 );
			x += 10;
		}
		x = edit_spy_x2 = font_blue.put( edit_spy_x1=x, edit_spy_y1=y, spy_array[spy_recno]->spy_skill, 0, x2 );
		edit_spy_y2 = edit_spy_y1 + font_blue.max_font_height;
		edit_spy_enable = 1;
	}
	else
	{
		edit_spy_enable = 0;
	}
	y += 20;

	// item
	if( item.id )
	{
		x = font_whbl.put( x1, y, text_unit.str_item(), 0, x2 );	// "Item"
		x = font_whbl.put( x+20, y, item.item_name(), 0, x2 );
		font_whbl.put( x1, y+16, item.item_desc(), 0, x2 );
	}
	y += 36;

	font_blue.put( x1, INFO_Y2-28, text_unit.str_left_inc() );  // "Left click to increase" );
	font_blue.put( x1, INFO_Y2-14, text_unit.str_right_dec() ); // "Right click to decrease" );

	return y;
}
// ------ End of function Unit::disp_edit_mode -----------//


// ------ Begin of function Unit::detect_edit_mode -----------//
//
void Unit::detect_edit_mode()
{
	if( edit_hp_enable )
	{
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
		{
			hit_points += 10.0f;
			if( hit_points > (float) max_hit_points() )
				hit_points = (float) max_hit_points();
		}
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
		{
			hit_points -= 10.0f;
			if( hit_points < 1.0f )
				hit_points = 1.0f;
		}
	}

	if( edit_loyalty_enable )
	{
		if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, LEFT_BUTTON) )
		{
			loyalty += 10;
			if( loyalty > 100 )
				loyalty = 100;
		}
		if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, RIGHT_BUTTON) )
		{
			loyalty -= 10;
			if( loyalty < 0 )
				loyalty = 0;
		}
	}

	if( edit_combat_enable )
	{
		if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, LEFT_BUTTON) )
		{
			int newCombat = (int)skill.actual_combat_level(NULL) + 5;
			if( newCombat > skill.max_combat_level )
				newCombat = skill.max_combat_level;
			set_combat_level(newCombat);
		}
		if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, RIGHT_BUTTON) )
		{
			int newCombat = (int)skill.actual_combat_level(NULL) - 5;
			if( newCombat < CITIZEN_COMBAT_LEVEL )
				newCombat = CITIZEN_COMBAT_LEVEL;
			set_combat_level(newCombat);
		}
	}

	if( edit_max_combat_enable )
	{
		if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, LEFT_BUTTON) )
		{
			int newLevel = skill.max_combat_level;
			newLevel += 20;
			if( newLevel > 900 )
				newLevel = 900;
			skill.set_max_combat_level(newLevel);
		}
		if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, RIGHT_BUTTON) )
		{
			int newLevel = skill.max_combat_level;
			newLevel -= 20;
			if( newLevel < MAX_COMBAT_TRAIN )
				newLevel = MAX_COMBAT_TRAIN;
			skill.set_max_combat_level(newLevel);
		}
	}

	if( edit_skill_enable )
	{
		if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, LEFT_BUTTON) )
		{
			skill.inc_skill_level(5.0f);
		}
		if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, RIGHT_BUTTON) )
		{
			int newSkill = (int)skill.actual_skill_level(NULL) - 5;
			if( newSkill < CITIZEN_SKILL_LEVEL )
				newSkill = CITIZEN_SKILL_LEVEL;
			skill.set_skill_level(newSkill);
		}
	}

	if( edit_max_skill_enable )
	{
		if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, LEFT_BUTTON) )
		{
			int newLevel = skill.max_skill_level;
			newLevel += 10;
			if( newLevel > 200 )
				newLevel = 200;
			skill.set_max_skill_level(newLevel);
		}
		if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, RIGHT_BUTTON) )
		{
			int newLevel = skill.max_skill_level;
			newLevel -= 20;
			if( newLevel < MAX_SKILL_TRAIN )
				newLevel = MAX_SKILL_TRAIN;
			skill.set_max_skill_level(newLevel);
		}
	}

	if( edit_spy_enable && spy_recno )
	{
		Spy *spyPtr = spy_array[spy_recno];
		if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, LEFT_BUTTON) )
		{
			if( (spyPtr->spy_skill += 5) > 100 )
			{
				spyPtr->spy_skill = 100;
			}
		}
		if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, RIGHT_BUTTON) )
		{
			if( (spyPtr->spy_skill -= 5) < 10 )
			{
				spyPtr->spy_skill = 10;
			}
		}
	}
}
// ------ End of function Unit::detect_edit_mode -----------//


// ------ Begin of function Unit::explore_for_player -------//
//
int Unit::explore_for_player()
{
	return is_visible() && UnitB::explore_for_player()
		&& !is_camouflage(nation_array.player_recno);
}
// ------ End of function Unit::explore_for_player -------//


// ------ begin of function Unit::send_hotkey --------//
//
// send hot key during sys::process_key
// pass keycode to button.detect() is not good becase
//  in report mode (especially chat mode), button.detect() should not detect keyboard
//
int Unit::send_hotkey(unsigned scanCode, unsigned skeyState)
{
	err_when( sprite_recno != unit_array.selected_recno );		// must be the main selected

	int keyCode;

	if( (keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, 0)) )
	{
		switch( keyCode )
		{
		case 'b':
			if( is_own() && button_build.visible_flag && button_build.enable_flag )
			{
				// put the action of button_build.detect() here
				unit_menu_mode = UNIT_MENU_BUILD;
				se_ctrl.immediate_sound("CONSTR");
				return 1;
			}
			break;
		}
	}

	return 0;
}
// ------ end of function Unit::send_hotkey --------//
