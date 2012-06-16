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

//Filename    : OTOWNIF.CPP
//Description : Town interface routines

#include <otown.h>
#include <oinfo.h>
#include <obox.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <osys.h>
#include <ohelp.h>
#include <ospy.h>
#include <ostr.h>
#include <ofont.h>
#include <omouse.h>
#include <ovbrowif.h>
#include <ogame.h>
#include <oconfig.h>
#include <onation.h>
#include <obutt3d.h>
#include <oimgres.h>
#include <orawres.h>
#include <oraceres.h>
#include <oworld.h>
#include <ounit.h>
#include <ou_wagon.h>
#include <oremote.h>
#include <ose.h>
#include <oseres.h>
#include <obuttcus.h>
#include <obitmapw.h>
#include <ounitres.h>
#include <oblob2w.h>
#include <ogeta.h>
#include <obutton.h>
#include <key.h>
#include <otownres.h>
#include <ot_basic.h>
#include <ot_firm.h>
#include <ot_unit.h>

//------------- Define coordinations -----------//

#define BUTTON_X1 (INFO_X1+13)
#define BUTTON_Y1 (INFO_Y1+235)
#define BUTTON_X2 (INFO_X2-10)
#define BUTTON_Y2 (INFO_Y2+3)

//---------- Define constant ------------//

enum { TOWN_MENU_MAIN,
		 TOWN_MENU_TRAIN,
		 TOWN_MENU_SPY,
		 TOWN_MENU_VIEW_SECRET,
		 TOWN_MENU_SET_AUTO_COLLECT_TAX,
		 TOWN_MENU_SET_AUTO_GRANT,
		 TOWN_MENU_SET_TARGET_WALL_MAN_POWER
	  };

#define BUTTON_ACTION_WIDTH 44
#define BUTTON_ACTION_HEIGHT 46

#define BUTTON_LOYALTY_COUNT	7
#define COUNT_BUTTON_OFFSET_X	165
#define COUNT_BUTTON_OFFSET_Y 5
#define COUNT_BUTTON_WIDTH		32
#define COUNT_BUTTON_HEIGHT	32

//----------- Define static variables ----------//

static VBrowseIF  browse_race;
static VBrowseIF2 browse_spy;
static Button3D   button_recruit, button_train, button_tax, button_grant, button_wagon;
static Button3D   button_spy, button_cancel, button_spy_mobilize, button_buildwall,
						  button_spy_reward, button_spy_action, button_spy_view_secret;
static Button3D	  button_destruct, button_builder;
static ButtonCustom	button_loyalty_array[BUTTON_LOYALTY_COUNT];
static ButtonCustom	button_loyalty_disabled;
static Button3D		button_cancel2;

static short      browse_race_recno=1, browse_race_town_recno=0;		// the town which the browse_race displays its info
static short      recruit_race_count;
// static short		spy_count;
static short      last_town_recno=0, last_rebel_recno=0;
static char 		last_has_linked_own_camp;
static char       town_menu_mode=TOWN_MENU_MAIN;
static char       disable_refresh=0;
static short 		action_spy_recno;	// recno of the spy that is doing the bribing or viewing secret reports of other nations
static short		edit_population_x1, edit_population_y1, edit_population_x2, edit_population_y2, edit_population_enable;
static short		edit_loyalty_x1[MAX_NATION], edit_loyalty_y1[MAX_NATION], edit_loyalty_x2[MAX_NATION], edit_loyalty_y2[MAX_NATION], edit_loyalty_enable[MAX_NATION];

//-------- Define static class member var ------//

short  Town::if_town_recno = 0;
static char	last_menu_mode;
// static char	spy_menu_repaint;

//----------- Define static functions ----------//

static void disp_auto_menu_button(ButtonCustom *, int);
static void disp_man_power_button(ButtonCustom *, int);
static int  race_filter(int recNo=0);
static int  spy_filter(int recNo=0);
static void put_race_rec(int recNo, int x, int y, int refreshFlag);
static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
static void i_disp_skill_button(ButtonCustom *button, int);
static void i_disp_queue_skill_button(ButtonCustom *button, int);
static int ask_desc();


//--------- Begin of function Town::disp_info ---------//
//
void Town::disp_info(int refreshFlag)
{
	if_town_recno = town_recno;

	// #### begin Gilbert 5/2 #####//
	// target_wall_man_power = 1;
	// #### end Gilbert 5/2 #####//

	if (refreshFlag == INFO_REPAINT)
	//if( town_recno != last_town_recno )//||
	//	(refreshFlag==INFO_REPAINT && !disable_refresh) )
	{
		last_menu_mode = town_menu_mode  = TOWN_MENU_MAIN;
		last_town_recno = town_recno;
	}
	else
	{
		if( last_menu_mode != town_menu_mode ) //|| spy_menu_repaint)		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = town_menu_mode;
			// spy_menu_repaint = 0;
		}
	}

#ifdef USE_FLIP
	vga.use_back();
#endif

	switch( town_menu_mode )
	{
		case TOWN_MENU_MAIN:
			disp_main_menu(refreshFlag);
			break;
	
		case TOWN_MENU_SPY:
			disp_spy_menu(refreshFlag);
			break;

		case TOWN_MENU_VIEW_SECRET:
			spy_array.disp_view_secret_menu(action_spy_recno, refreshFlag);
			break;

		case TOWN_MENU_SET_AUTO_COLLECT_TAX:
			disp_auto_menu(1, refreshFlag);
			break;

		case TOWN_MENU_SET_AUTO_GRANT:
			disp_auto_menu(0, refreshFlag);
			break;

		case TOWN_MENU_SET_TARGET_WALL_MAN_POWER:
			disp_man_power(refreshFlag);
			break;
	}
#ifdef USE_FLIP
	vga.use_front();
#endif
}
//----------- End of function Town::disp_info -----------//


//--------- Begin of function Town::detect_info ---------//
//
void Town::detect_info()
{
	if_town_recno = town_recno;

	switch( town_menu_mode )
	{
		case TOWN_MENU_MAIN:
			detect_main_menu();
			break;

		case TOWN_MENU_SPY:
			detect_spy_menu();
			break;

		case TOWN_MENU_VIEW_SECRET:
			if( spy_array.detect_view_secret_menu(action_spy_recno, nation_recno) )
			{
				town_menu_mode = TOWN_MENU_MAIN;
				se_ctrl.immediate_sound("TURN_ON");
			//	info.disp();
			}
			break;

		case TOWN_MENU_SET_AUTO_COLLECT_TAX:
			detect_auto_menu(1);
			break;

		case TOWN_MENU_SET_AUTO_GRANT:
			detect_auto_menu(0);
			break;

		case TOWN_MENU_SET_TARGET_WALL_MAN_POWER:
			detect_man_power();
			break;
	}
}
//----------- End of function Town::detect_info -----------//


//--------- Begin of function Town::disp_main_menu ---------//
//
void Town::disp_main_menu(int refreshFlag)
{
	if (under_construction)
	{
		// String str;
		char *nationPict = image_spict.get_ptr("V_COLCOD");
		vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );
	
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("VILLCONS") );
//		font_vill.space_width -= 4;
//		font_vill.center_put( INFO_X1+113, INFO_Y1+152, INFO_X1+205, INFO_Y1+165, "Under" );
//		font_vill.center_put( INFO_X1+99, INFO_Y1+165, INFO_X1+217, INFO_Y1+185, "Construction" );
//		font_vill.space_width += 4;
		font_vill.center_put_paragraph( INFO_X1+99, INFO_Y1+152, INFO_X1+217, INFO_Y1+185,
			text_firm.str_under_construction(), -5, 0, 0 );

		font_snds.center_put( INFO_X1+20, INFO_Y1+125, INFO_X1+125, INFO_Y1+145, 
			town_name(), 0, 1 );
		font_vill.center_put( INFO_X1+20, INFO_Y1+105, INFO_X1+125, INFO_Y1+125, 
			text_firm.str_town_of() );

		// str = construction_completion_percent;
		// str += "%";
		font_vill.center_put( INFO_X1+20, INFO_Y1+139, INFO_X1+125, INFO_Y1+171, 
			m.format(construction_completion_percent, 3) );

		if( nation_recno && nation_array.player_recno == nation_recno )
		{
			if( refreshFlag == INFO_REPAINT )
			{
				button_destruct.create( INFO_X1+16, INFO_Y1-61, "V_DEM-U", "V_DEM-D", 1 );
				button_destruct.set_help_code( "DESTFIRM" );
				button_destruct.enable_flag = 1;
	
				if (( current_wall_level > 0 ) && ( hit_points < max_hit_points() ))
				{
					button_builder.create( INFO_X1+13, INFO_Y1-96, "REPAIRU", "REPAIRD", 0 );
					button_builder.set_help_code( "REPAIR" );
					button_builder.enable_flag = 1;
				}
			}
			button_destruct.paint();
			if (( current_wall_level > 0 ) && ( hit_points < max_hit_points() ))
				button_builder.paint();
		}
		return;

	}

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("VILLBASE") );

	//--------- display basic info --------//
	String str;
	int tempx;
	
	// ------- display town name --------//
	// char str2[30] = "";
	// strcat( str2, town_name() );
	font_town.center_put( INFO_X1+11, INFO_Y1 +11 , INFO_X2, INFO_Y1 +29, 
		town_name(), 0, 1 );

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	// ------- display race name --------//
//	if (race_res[race_id]->name[0] == 'E' ||
//		 race_res[race_id]->name[0] == 'I')
//		 str = "An ";
//	else
//		 str = "A ";

//	str += race_res[race_id]->name;
//	if( is_pay_tribute_to_monster() )
//		str += " Slave";
//	str += " Town";
//	tempx = font_snds.center_put( INFO_X1 +11, INFO_Y1 +36 , INFO_X2, INFO_Y1 +48, str );

	// ##### patch begin Gilbert 23/10 #######//
	// before use race name instead of adjective
	font_snds.center_put( INFO_X1 +11, INFO_Y1 +36 , INFO_X2, INFO_Y1 +48, 
		(is_pay_tribute_to_monster() 
		? text_firm.str_race_slave_town(race_res[race_id]->adjective) 
		: text_firm.str_race_town(race_res[race_id]->adjective)) );
	// ##### patch end Gilbert 23/10 #######//

	// ------- display wall hit point ------- //
	// Town *townPtr = town_array[town_array.selected_recno];
	if ( current_wall_level > 0 ) 
	{
		float hitPoints;

		if( hit_points > (float)0 && hit_points < (float)1 )
			hitPoints = (float) 1;
		else
			hitPoints = hit_points;

		int ratio = (int)hitPoints *25 / (int)max_hit_points();
		char hitPointName[] = "BB-00";
		hitPointName[3] = '0' + ratio / 10;
		hitPointName[4] = '0' + ratio % 10;
		image_spict.put_back( INFO_X1 +140, INFO_Y1 -106, hitPointName);

		if( max_hit_points() < 1000 )
			image_icon.put_back( INFO_X1 +173, INFO_Y1 -62, "REPAIRBX");
		font_san.put( INFO_X1+177, INFO_Y1-59, (int) hitPoints );
		font_san.put( INFO_X1+177, INFO_Y1-45, (int) max_hit_points() );
		help.set_help( INFO_X1 +173, INFO_Y1 -62, INFO_X1 +202, INFO_Y1 -29, "T_ENENGY" );
	}
	else
		font_snds.center_put( INFO_X1+15, INFO_Y1-19, INFO_X2, INFO_Y1-4, 
			text_firm.str_no_town_wall() ); //"No Town Wall" );
	
	// ------- display population ---------//
	// str = "Population";
	font_vilb.put( INFO_X1+20, INFO_Y1 +94, text_firm.str_town_population() );

//	str = population;
	if (population == MAX_TOWN_POPULATION)
		vga.active_buf->put_bitmap( INFO_X1 +35, INFO_Y1 +117, image_icon.read( "FULLVILL" ));
	else	
		font_vill.put( INFO_X1 +49, INFO_Y1 +113, population );

	// ------- display population units ---------//
	int offsetX[6] = {  60,  46,  76,  36,  86,  60};
	int offsetY[6] = { 225-20, 221-20, 221-20, 218-20, 218-20, 215-20 };
	int dispPopulation;
	if (population > 60)
		dispPopulation = 60;
	else
		dispPopulation = population;

	for(int i= ((dispPopulation + 9)/10); i > 0; i--)
	{
		info.draw_unit_icon( INFO_X1 +offsetX[i-1], INFO_Y1 +offsetY[i-1], race_res[race_id]->civilian_unit_id,
			nation_recno, INFO_X1+12, INFO_Y1+163, INFO_X1+110, INFO_Y1+227, 1 );
	}

	help.set_help( INFO_X1+16, INFO_Y1 +96, INFO_X1+96, INFO_Y1 +138, "TPOP" );

//	int unitId = race_res[race_id]->civilian_unit_id;
//	SpriteInfo*  sprite_info = sprite_res[unit_res[unitId]->sprite_id];
//	sprite_info->load_bitmap_res();
//	int offsetX[6] = {  60,  46,  76,  36,  86,  60};
//	int offsetY[6] = { 225-20, 221-20, 221-20, 218-20, 218-20, 215-20 };
//	int stopArrayOffset[6] = { 4, 5, 3, 5, 3, 4 };
//	for(int i= ((population + 9)/10); i > 0; i--)
//	{
//		SpriteStop *stopAction = sprite_info->stop_array +stopArrayOffset[5-i];
//		SpriteFrame *spriteFrame = sprite_frame_res[stopAction->frame_recno];
//		char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);
//		short* colorRemapTable = sprite_info->get_color_remap_table(nation_recno, 0);
//		if (!((stopArrayOffset[5-i] < 8 || sprite_info->turn_resolution <= 8) ? (stopArrayOffset[5-i] & 7) >= 5 : (stopArrayOffset[5-i] & 7) >= 4))
//		{
//			if( !colorRemapTable )
//				vga_back.put_bitmap_trans_decompress(INFO_X1 +offsetX[5-i] +spriteFrame->offset_x, INFO_Y1 +offsetY[5-i] +spriteFrame->offset_y, bitmapPtr);
//			else
//				vga_back.put_bitmap_trans_remap_decompress(INFO_X1 +offsetX[5-i] +spriteFrame->offset_x, INFO_Y1 +offsetY[5-i] +spriteFrame->offset_y, bitmapPtr, colorRemapTable);
//		}
//		else
//		{
//			if( !colorRemapTable )
//				vga_back.put_bitmap_trans_decompress_hmirror(INFO_X1 +offsetX[5-i] +spriteFrame->offset_x, INFO_Y1 +offsetY[5-i] +spriteFrame->offset_y, bitmapPtr);
//			else
//				vga_back.put_bitmap_trans_remap_decompress_hmirror(INFO_X1 +offsetX[5-i] +spriteFrame->offset_x, INFO_Y1 +offsetY[5-i] +spriteFrame->offset_y, bitmapPtr, colorRemapTable);
//		}
//	}
	
	// ------- display jobless population ---------//
	// str = "Peasant";
	font_vilb.put( INFO_X1+160, INFO_Y1 +127, text_firm.str_town_peasant(), 0, INFO_X2-6 );

	// str = jobless_population;
	font_vill.center_put( INFO_X1 +155, INFO_Y1 +145, INFO_X1 +215, INFO_Y1 +157, 
		m.format(jobless_population) );
	help.set_help( INFO_X1+155, INFO_Y1 +130, INFO_X1+213, INFO_Y1 +163, "TPEASANT" );

	// ------- display worker population ---------//
	// str = "Worker";
	font_vilb.put( INFO_X1+88, INFO_Y1 +127, text_firm.str_town_worker() );

	// str = population - jobless_population;
	font_vill.center_put( INFO_X1 +70, INFO_Y1 +145, INFO_X1 +160, INFO_Y1 +157, 
		m.format(population - jobless_population) );
	help.set_help( INFO_X1+84, INFO_Y1 +130, INFO_X1+144, INFO_Y1 +164, "TWORKER" );
			
	// ------- display loyalty and resistance -------- //
if (population >0)
{
	int curLoyalty;
	int targetLoyalty;

	if( nation_recno )
	{
		// str = "Loyalty";
		font_vilb.put( INFO_X1+120, INFO_Y1 +169, text_unit.str_loyalty() );
 
		curLoyalty    = (int) loyalty;
		targetLoyalty = (int) target_loyalty;
		help.set_help( INFO_X1+112, INFO_Y1 +172, INFO_X1+181, INFO_Y1 +206, "TLOYALTY" );
	}
	else
	{
		// str = "Resistance";
		font_vilb.put( INFO_X1+112, INFO_Y1 +169, text_firm.str_resistance() );
		help.set_help( INFO_X1+112, INFO_Y1 +172, INFO_X1+181, INFO_Y1 +206, "TRESIST" );

	//	vga.active_buf->put_bitmap( INFO_X1 +113, INFO_Y1 +172, image_icon.read( "RESIST" ));

		if( config.show_ai_info )
		{
			if (info.default_viewing_nation_recno)
			{
				curLoyalty    = (int) resistance(info.default_viewing_nation_recno);
				targetLoyalty = target_resistance_array[info.default_viewing_nation_recno-1];
			}
			else
			{
				curLoyalty = 0;
				targetLoyalty = 0;
			}
		}
		else
		{
			if (nation_array.player_recno)
			{
				curLoyalty    = (int) resistance(nation_array.player_recno);
				targetLoyalty = target_resistance_array[nation_array.player_recno-1];
			}	
			else
			{
				curLoyalty = 0;
				targetLoyalty = 0;
			}
		}

		if( targetLoyalty > curLoyalty )	// resistance only decrease, won't increase
			targetLoyalty = -1;				// don't display the decrease target
	}
	int dispArrow=0;
	if( curLoyalty == targetLoyalty || targetLoyalty == -1 )	// only display up and down arrow for independent town's resistance
		str = curLoyalty;
	else
	{
		str  = curLoyalty;
		str += "    ";
		str += targetLoyalty;
		dispArrow=1;
	}
	tempx = font_vill.center_put( INFO_X1+111, INFO_Y1 +187, INFO_X1+182, INFO_Y1 +187+font_vill.height(), str );
	if( dispArrow )
	{
		tempx = tempx-font_vill.text_width( m.format(targetLoyalty) ) - 12;
		if( targetLoyalty > curLoyalty )
			vga.active_buf->put_bitmap_trans( tempx, INFO_Y1 +190, image_icon.read( "ARROWVUP" ));
		else
	//	if( targetLoyalty < curLoyalty )
			vga.active_buf->put_bitmap_trans( tempx, INFO_Y1 +190, image_icon.read( "ARROWVDN" ));
	}
}
	// ------- display button ------- //
	int x1 = INFO_X1 +13;
	int y1 = INFO_Y1 +235;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	// ------- create button ------- //
	// ------- check to display which button and remove which button ------- //
	// Town *townPtr = town_array[town_array.selected_recno];
	// townPtr->nation_recno = the town belongs to which player, 0 = independent
	// nation_array.player_recno = player no

	if( nation_array.player_recno )	//player not die yet
	{
		if( refreshFlag == INFO_REPAINT )
		{
			button_destruct.create( INFO_X1+16, INFO_Y1-61, "V_DEM-U", "V_DEM-D", 1 );
			button_destruct.set_help_code( "DESTFIRM" );
			button_destruct.enable_flag = 0;

			button_builder.create( INFO_X1+13, INFO_Y1-96, "REPAIRU", "REPAIRD", 0 );
			button_builder.set_help_code( "REPAIR" );
		//	button_builder.enable_flag = 0;

			button_recruit.create( x2, y2, 'A', "RECRUIT" );
			button_recruit.enable_flag = 0;
			x2 += BUTTON_DISTANCE;

			button_wagon.create( x2, y2, 'A', "WAGON" );
			button_wagon.enable_flag = 0;
			x2 += BUTTON_DISTANCE;

			if( target_wall_level >= 1)
				button_buildwall.create( x2, y2, 'A', "WALLS-2" );
			else
				button_buildwall.create( x2, y2, 'A', "WALLS-1" );

			button_buildwall.enable_flag = 0;
			x2 += BUTTON_DISTANCE;

			button_tax.create( x1, y1 , 'A', "COLLTAX" );
			button_tax.enable_flag = 0;
			x1 += BUTTON_DISTANCE;
		
			button_grant.create( x1, y1 , 'A', "GRANT" );
			button_grant.enable_flag = 0;
			x1 += BUTTON_DISTANCE;

			button_spy.create( x1, y1, 'A', "SPYMENU" );
			button_spy.enable_flag = 0;
			x1 += BUTTON_DISTANCE;
		}

		// #### begin Gilbert 24/2 ########//
		// if( nation_recno && nation_array.player_recno == nation_recno )
		if( is_own() )
		{
			if( (button_destruct.visible_flag = population == 0) )	// can destruct only empty town
			{
				button_destruct.enable_flag = 1;
				button_destruct.paint();
			}

			if ( (button_builder.visible_flag = 
				current_wall_level > 0 && hit_points < max_hit_points()) )
			{
		// #### end Gilbert 24/2 ########//
			
				if( repair_flag && hit_points < max_hit_points() )
				{
					// set pushed_flag to 1 as although repair_flag will not reset after selecting 
					// other things, pushed flag will do
					button_builder.pushed_flag = repair_flag;
					button_builder.update_bitmap( ((sys.frame_count % 5) < 3) ? (char*)"REPAIRU" : (char*)"REPAIRD");
				}
				else
				{
					button_builder.pushed_flag = repair_flag;
					button_builder.update_bitmap( repair_flag ? (char*)"REPAIRU" : (char*)"REPAIRD");
				}
			}
		
			button_recruit.enable_flag = can_recruit_peasant();
			button_recruit.paint();
			button_wagon.enable_flag = population > 0 ? 1 : 0;
			button_wagon.paint();
			
			button_buildwall.enable_flag = ((population > 0) && (current_wall_level < 2)) ? 1 : 0;
			if( target_wall_level >= 1 )
				button_buildwall.update_bitmap( "WALLS-2" );
			else
				button_buildwall.update_bitmap( "WALLS-1" );		// update_bitmap call paint
					
			if( hit_points < max_hit_points() && repair_flag )
			{
				vga.active_buf->bar_alpha( button_buildwall.x1, button_buildwall.y1, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-1, 1, 0 );
				// str = "Repair";
				font_whbl.center_put( button_buildwall.x1, button_buildwall.y1, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-21, text_firm.str_wall_repair() );
				// str = (int)townPtr->hit_points *100 / (int)townPtr->max_hit_points();
				// str += "%";
				font_whbl.center_put( button_buildwall.x1, button_buildwall.y1+21, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-1, 
					m.format( (int)hit_points*100/(int)max_hit_points(), 3) );
			}
			else if ( target_wall_level > current_wall_level )
			{	
				vga.active_buf->bar_alpha( button_buildwall.x1, button_buildwall.y1, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-1, 1, 0 );
				// str = "Building";
				font_whbl.center_put( button_buildwall.x1, button_buildwall.y1, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-21, text_firm.str_wall_building() );
				// str = upgrade_wall_progress *100 /MAX_WALL_PROGRESS;
				// str += "%";
				font_whbl.center_put( button_buildwall.x1, button_buildwall.y1+21, button_buildwall.x1+BUTTON_ACTION_WIDTH-1, button_buildwall.y1+BUTTON_ACTION_HEIGHT-1, 
					m.format( upgrade_wall_progress *100 /MAX_WALL_PROGRESS, 3) );
			}
			
			// ####### begin Gilbert 3/7 #########//
			if( (button_tax.visible_flag = button_grant.visible_flag = has_linked_own_camp) )
			{
				button_tax.enable_flag = ( loyalty >= 10 && population > 0 );
				button_tax.paint();

				// still enable when no cash, so as to change auto grant level
			//	button_grant.enable_flag = ((~nation_array)->cash > 0 && population > 0 );
				button_grant.enable_flag = ( population > 0 );
				button_grant.paint();

				disp_auto_loyalty();
			}
			// ####### end Gilbert 3/7 #########//
		}
		else if( can_grant_to_non_own_town(nation_array.player_recno) )
		{
			button_grant.enable_flag = ((~nation_array)->cash > 0 && population > 0 );
			button_grant.paint();
		}
	
		// -------- display the spy button ----------//
		int spyFlag = spy_count && spy_filter() > 0;
		if( spyFlag )
		{
			button_spy.enable_flag = 1;
			button_spy.paint();
		}
	}
	//---- if this town is paying tribute to a Fryhtan lair ---//

	if( nation_array.player_recno )
	{
		int nationRecno;

		if( config.show_ai_info )
			nationRecno = info.default_viewing_nation_recno;
		else
			nationRecno = nation_array.player_recno;

		int totalTribute = total_tribute_to_monster(nationRecno) * 12;

		if( totalTribute )
		{
			// String str;
			// str = "Annual tribute paid to you by this town: ";
			// str += m.format( totalTribute, 2 );
			vga.active_buf->bar( BUTTON_X1, BUTTON_Y1, BUTTON_X2, BUTTON_Y2, V_BLACK );
			font_whbl.put_paragraph( BUTTON_X1+10, BUTTON_Y1+20, BUTTON_X2-10, BUTTON_Y2-20, 
				text_firm.str_slave_tribute(totalTribute) );
		}
	}
}
//----------- End of function Town::disp_main_menu -----------//

//--------- Begin of function Town::disp_auto_loyalty ---------//
//
void Town::disp_auto_loyalty()
{
	if( auto_collect_tax_loyalty )
	{
		vga.active_buf->bar_alpha( button_tax.x1, button_tax.y1+15, button_tax.x1+BUTTON_ACTION_WIDTH-1, button_tax.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
		font_whbl.center_put( button_tax.x1, button_tax.y1, button_tax.x1+BUTTON_ACTION_WIDTH-1, button_tax.y1+BUTTON_ACTION_HEIGHT-1,
									m.format(auto_collect_tax_loyalty) );
	}
	if( auto_grant_loyalty )
	{
		vga.active_buf->bar_alpha( button_grant.x1, button_grant.y1+15, button_grant.x1+BUTTON_ACTION_WIDTH-1, button_grant.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
		font_whbl.center_put( button_grant.x1, button_grant.y1, button_grant.x1+BUTTON_ACTION_WIDTH-1, button_grant.y1+BUTTON_ACTION_HEIGHT-1,
									m.format(auto_grant_loyalty) );
	}
}
//----------- End of function Town::disp_auto_loyalty -----------//


//--------- Begin of function Town::detect_main_menu ---------//
//
void Town::detect_main_menu()
{
	//--- detect clicking on the name area to center the map on it ---//
	//-- three interface 1) auto_tax_collect, 2) auto_grant, 3) town_spy_menu
	if( mouse.single_click(INFO_X1 +11, INFO_Y1 +5 , INFO_X1 +217, INFO_Y1 +83) &&
		(!is_own() || remote.is_enable() || !mouse.single_click(INFO_X1 +23, INFO_Y1 +11, INFO_X1 +205, INFO_Y1 +28)) )
	{
		world.go_loc( center_x, center_y );
		return;
	}

	if( mouse.single_click(INFO_X1 +23, INFO_Y1 +11, INFO_X1 +205, INFO_Y1 +28) && is_own() &&
		!remote.is_enable() )
	{
		input_town_name();
		return;
	}

	if( nation_array.player_recno )
	{
		int rc;
		if( (rc = button_recruit.detect(0, 0, 1)))
		{
			recruit( 0, COMMAND_PLAYER );	// rc==2 means recruit wagon 
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( (rc = button_wagon.detect(0, 0, 1)))
		{
			recruit( 1, COMMAND_PLAYER );	// rc==2 means recruit wagon 
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( (rc = button_buildwall.detect(0, 0, 1)) )
		{
			se_ctrl.immediate_sound("TURN_ON");

			if( rc == 1 && target_wall_level < MAX_WALL_LEVEL )
			{
				set_target_wall_level( target_wall_level+1, COMMAND_PLAYER );
			}
			if( rc == 2 && target_wall_level > current_wall_level )
			{
				set_target_wall_level( target_wall_level-1, COMMAND_PLAYER );
			}
		}

		if( (rc = button_builder.detect(0, 0, 1)) )
		{
			se_ctrl.immediate_sound("REPAIR");
			// ###### begin Gilbert 21/4 ########//
			if( rc != 2 )
			{
				set_repair_wall( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			else	// right click to repair all buildings
			{
				nation_array[nation_recno]->repair_all_building( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			// ###### end Gilbert 21/4 ########//
		}

		if( (rc = button_destruct.detect(0, 0, 1)) )
		{
			se_ctrl.immediate_sound("TURN_ON");
			// ##### begin Gilbert 7/10 #######//
			if( !remote.is_enable() )
			{
				town_array.del_town(town_recno);
			}
			else
			{
				// packet structure <town recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_SELL, sizeof(short) );
				*shortPtr = town_recno;
			}
			// ##### end Gilbert 7/10 #######//
		}

		if( (rc = button_tax.detect(0, 0, 1)) )
		{
			disp_auto_loyalty();
			// ##### begin Gilbert 26/9 ########//
			se_ctrl.immediate_sound("TAXGRANT");
			// ##### end Gilbert 26/9 ########//

			if( rc == 1 )						// press left button
				collect_tax(COMMAND_PLAYER);

			if( rc == 2 )
			{
				town_menu_mode = TOWN_MENU_SET_AUTO_COLLECT_TAX;
			//	disable_refresh = 1;
			//	info.disp();
			//	disable_refresh = 0;
			}
		}

		if( nation_array.player_recno && can_grant_to_non_own_town(nation_array.player_recno) )
		{
			if( (rc = button_grant.detect()) )
			{
				se_ctrl.immediate_sound("TAXGRANT");
				grant_to_non_own_town(nation_array.player_recno, COMMAND_PLAYER);
			}
		}
		else if( nation_array.player_recno && nation_recno == nation_array.player_recno)
		{
			if( (rc = button_grant.detect(0, 0, 1)) )
			{
				disp_auto_loyalty();
				// ##### begin Gilbert 26/9 ########//
				se_ctrl.immediate_sound("TAXGRANT");
				// ##### end Gilbert 26/9 ########//
				
				if( rc == 1 )
				{
					town_array[town_recno]->reward(COMMAND_PLAYER);
				}

				if( rc == 2 )
				{
					town_menu_mode  = TOWN_MENU_SET_AUTO_GRANT;
				//	disable_refresh = 1;    // static var for disp_info() only
				//	info.disp();
				//	disable_refresh = 0;
				}
			}
		}

		if( button_spy.detect() )
		{
			se_ctrl.immediate_sound("TURN_ON");
			town_menu_mode = TOWN_MENU_SPY;
		//	disable_refresh = 1;    // static var for disp_info() only
		//	info.disp();
		//	disable_refresh = 0;
		}
	}
	help.set_help(INFO_X1 +23, INFO_Y1 +11, INFO_X1 +205, INFO_Y1 +28, "TOWNNAME");
}
//----------- End of function Town::detect_main_menu -----------//

void Town::disp_spy_menu(int refreshFlag)
{
	//---------- paint controls -----------//

	int playerSpyCount = spy_filter();
	if(playerSpyCount <= 0)
	{
		town_menu_mode   = TOWN_MENU_MAIN;
		return;
	}
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	
	//------ display browser field description -------//
	int x=INFO_X1 +55;
	int y=INFO_Y1 +10;

	font_whbl.right_put( x+22   , y, text_firm.str_town_spy_skill()); //"Skill" );
	font_whbl.put( x+32, y, text_unit.str_loyalty()); //"Loyalty" );
	font_whbl.put( x+105, y, text_firm.str_town_spy_mission()); //"Mission" );

	//------------ create browser ------------//

	if( refreshFlag == INFO_REPAINT )
	{
		browse_spy.init( INFO_X1 +13, INFO_Y1+40, INFO_X1+233, INFO_Y1 +219,
								0, 45, playerSpyCount, put_spy_rec );

	//	browse_spy.init( RACE_BROWSE_X1, RACE_BROWSE_Y1, RACE_BROWSE_X2, RACE_BROWSE_Y2,
	//							0, 25, playerSpyCount, put_spy_rec );

		browse_spy.open(1);
	}
//	browse_spy.init_var(playerSpyCount, browse_spy.rec_no);
	browse_spy.update(playerSpyCount);
	browse_spy.paint();
	
	//----------- create the paint button ----------//

	if( refreshFlag == INFO_REPAINT )
	{
		int x1 = INFO_X1 +13;
		int y1 = INFO_Y1 +235;
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;

		button_spy_mobilize.create( x2, y2, 'A', "MOBILSPY" );
		button_spy_mobilize.enable_flag = 0;
		x2+=BUTTON_DISTANCE;

		//--------- reward spy button --------//

		if (!is_monster())
			button_spy_reward.create( x1, y1, 'A', "REWARD" );
		else
			button_spy_reward.create( x1, y1, 'A', "F_REWARD" );
		button_spy_reward.enable_flag = 0;
		x1+=BUTTON_DISTANCE;

		button_spy_action.create( x1, y1, 'A', "CHG-MISS" );
		button_spy_action.enable_flag = 0;
		x1+=BUTTON_DISTANCE;
				
		button_spy_view_secret.create( x1, y1, 'A', "VSECRET" );
		button_spy_view_secret.enable_flag = 0;
		
		button_cancel.create( INFO_X1 +13 + 3*BUTTON_DISTANCE, y2, 'A', "PREVMENU" );
	}
	
	button_spy_mobilize.enable_flag = 1;
	button_spy_mobilize.paint();
	
	button_spy_reward.enable_flag = 1;
	button_spy_reward.paint();
	
	if( nation_recno != nation_array.player_recno )		// if the spy is in another nation's town
	{
		button_spy_action.enable_flag = 1;
		button_spy_action.paint();
	}	
	
	// view secret only in camp
//	if( nation_recno && nation_recno != nation_array.player_recno )
//	{
//		button_spy_view_secret.enable_flag = 1;
//		button_spy_view_secret.paint();
//	}
	
	button_cancel.paint();
}
//----------- End of function Town::disp_spy_menu -----------//


//--------- Begin of function Town::detect_spy_menu ---------//
//
void Town::detect_spy_menu()
{
	int playerSpyCount = spy_filter();
	if(playerSpyCount <= 0)
	{
		town_menu_mode   = TOWN_MENU_MAIN;
		return;
	}

	browse_spy.detect();
	Spy* spyPtr = spy_array[ spy_filter( browse_spy.recno() ) ];

	//------- mobilize spy --------//

	if( button_spy_mobilize.detect() )
	{
		se_ctrl.immediate_sound("TURN_ON");
		// spy_menu_repaint = 1;

		if( !remote.is_enable() )
		{
			if( spyPtr->mobilize_town_spy() )
			{
				spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it 
	//			disp_spy_menu( INFO_REPAINT );
			}
		}
		else
		{
			// packet structure <spy recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_TOWN, sizeof(short) );
			*shortPtr = spyPtr->spy_recno;
		}
	}

	//------ reward spy ---------//

	else if( button_spy_reward.detect() )
	{
		se_ctrl.immediate_sound("TURN_ON");
		spyPtr->reward(COMMAND_PLAYER);
	}

	//----- change spy action --------//

	if( nation_recno != nation_array.player_recno )		// if the spy is in another nation's town
	{
		if( button_spy_action.detect() )		// set action mode
		{
//			spy_menu_repaint = 1;
			se_ctrl.immediate_sound("TURN_ON");
			if( !remote.is_enable() )
			{
				spyPtr->set_next_action_mode();
		//		disp_spy_menu( INFO_UPDATE );
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CYCLE_ACTION, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
		}
	}

	//----- view secret report ------/

	if( nation_recno && nation_recno != nation_array.player_recno )
	{
		if( button_spy_view_secret.detect() )
		{
			se_ctrl.immediate_sound("TURN_ON");
			action_spy_recno = spyPtr->spy_recno;
			town_menu_mode   = TOWN_MENU_VIEW_SECRET;
		//	disable_refresh = 1;
		//	info.disp();
		//	disable_refresh = 0;
		}
	}

	//--------- cancel -----------//

	if( button_cancel.detect() )//|| mouse.any_click(1) )		// right click to cancel
	{
		//info.disp();
		town_menu_mode   = TOWN_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
	}
}
//----------- End of function Town::detect_spy_menu -----------//

//--------- Begin of function Town::disp_auto_menu ---------//
//
void Town::disp_auto_menu(int modeCollectTax, int refreshFlag)
{
	int curAutoLoyalty;

	Nation* nationPtr = nation_array[nation_recno];

	if( modeCollectTax )
		curAutoLoyalty =	auto_collect_tax_loyalty;
	else
		curAutoLoyalty =	auto_grant_loyalty;

	//---------- paint buttons ------------//

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("AUTOBASE") );
	
//	String str;
//	String headingStr;
	int i, x=INFO_X1+23, y=INFO_Y1+110;
	
//	if( modeCollectTax )
//		headingStr = "Automatically Collect Tax from this Town (Left - click) or all your Towns (Right - click) when the Loyalty reaches:";
//	else
//		headingStr = "Automatically Grant Money to this Town (Left - click) or all your Towns (Right - click) when the Loyalty drops below:";
//	str = headingStr;
	font_bld.center_put_paragraph( INFO_X1+17, INFO_Y1+20, INFO_X2-7, INFO_Y1+70, 
		modeCollectTax ? text_firm.str_auto_tax_inst() : text_firm.str_auto_grant_inst(),
		0, 0, 0 );

	for( i=0; i<BUTTON_LOYALTY_COUNT; i++ )
	{
		if ( refreshFlag == INFO_REPAINT )
			button_loyalty_array[i].create( x +(i>>2) *94, y +(i%4) *23, x +(i>>2) *94 +91, y +(i%4) *23 +20,
					disp_auto_menu_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0), i ));
		button_loyalty_array[i].paint();
	}
	
	i = BUTTON_LOYALTY_COUNT;
	if ( refreshFlag == INFO_REPAINT )
		button_loyalty_disabled.create( x +(i>>2) *94, y +(i%4) *23, x +(i>>2) *94 +91, y +(i%4) *23 +20,
				disp_auto_menu_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0), i ));
	button_loyalty_disabled.paint();

	if ( refreshFlag == INFO_REPAINT )
	//	button_cancel2.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
		button_cancel2.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );
	button_cancel2.paint();
}
//----------- End of function Town::disp_auto_menu -----------//

//--------- Begin of function Town::disp_man_power ---------//
//
void Town::disp_man_power(int refreshFlag)
{
	//---------- paint buttons ------------//

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
	Nation* nationPtr = nation_array[nation_recno];

//	String str;
//	String headingStr;
	int i, x=INFO_X1+23, y=INFO_Y1+52;
	
//	headingStr = "Please assign the number of jobless civilians (0 - 8) to build the town wall :";
//	str = headingStr;
	font_snds.put_paragraph( INFO_X1+20, INFO_Y1+8, INFO_X2-5, INFO_Y2-5, 
		text_firm.str_set_wall_crew() );

	for( i=0; i<9; i++ )
	{
		if ( refreshFlag == INFO_REPAINT )
			button_loyalty_array[i].create( x, y +i *19, x +187, y +i *19 +17,
					disp_man_power_button, ButtonCustomPara( game.get_color_remap_table(nation_recno,0), i ));
		button_loyalty_array[i].paint();
	}
	
	if ( refreshFlag == INFO_REPAINT )
		button_cancel2.create( INFO_X1 +13 + 3 * BUTTON_DISTANCE, INFO_Y1 +281, 'A', "CANCEL" );
	//	button_cancel2.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
	button_cancel2.paint();
}
//----------- End of function Town::disp_man_power -----------//

// ---------- Begin of static function disp_firm_button --------//
static void disp_auto_menu_button(ButtonCustom *button, int)
{
	String str;
	int paintInfo = button->custom_para.value;
	if (paintInfo == BUTTON_LOYALTY_COUNT)
		str = text_firm.str_disable_auto(); // "DISABLE";
	else
	{
		str = (int) (paintInfo *10 +30);
		if (town_menu_mode == TOWN_MENU_SET_AUTO_COLLECT_TAX)
			str = (int) (paintInfo *10 +40);		
	}

	if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDDW3") );
//		vga_util.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
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

// ---------- Begin of static function disp_man_power_button --------//
static void disp_man_power_button(ButtonCustom *button, int)
{
//	String str;
	int paintInfo = button->custom_para.value;
//	str = (int) paintInfo;
//	if (paintInfo >1)
//		str +=" Workers";
//	else 
//	if (paintInfo ==1)
//		str +=" Worker";
//	else
//		str ="No Worker";

	if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDDWN") );
//		vga_util.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_snds.center_put(button->x1+1, button->y1+2, button->x2+1, button->y2+2, 
			text_firm.str_no_of_worker(paintInfo) );
	}
	else
	if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDSEL") );
		font_snds.center_put(button->x1, button->y1+1, button->x2, button->y2+1, 
			text_firm.str_no_of_worker(paintInfo) );
	}
	else
	{
		vga.active_buf->put_bitmap( button->x1, button->y1, image_icon.read("BUILDUP") );
		font_snds.center_put(button->x1, button->y1+1, button->x2, button->y2+1, 
			text_firm.str_no_of_worker(paintInfo) );
	}
}
// ---------- End of static function disp_man_power_button --------//

//--------- Begin of function Town::detect_auto_menu ---------//
//
void Town::detect_auto_menu(int modeCollectTax)
{
	int i, rc=0, loyaltyLevel = 30;

	if (modeCollectTax)
		loyaltyLevel = 40;

	for( i=0; i<BUTTON_LOYALTY_COUNT ; loyaltyLevel+=10, i++ )
	{
		rc = button_loyalty_array[i].detect(0, 0, 1);

		if( rc )
		{
			se_ctrl.immediate_sound("TAXGRANT");
			break;
		}
	}

	if( !rc )
	{
		rc = button_loyalty_disabled.detect(0, 0, 1);
		loyaltyLevel = 0;
	}

	//------ set new settings now -------//

	if( rc==1 )
	{
		se_ctrl.immediate_sound("TURN_ON");
		if( modeCollectTax )
		{
			if( !remote.is_enable() )
			{
				set_auto_collect_tax_loyalty(loyaltyLevel);
			}
			else
			{
				// packet structure <town recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_TAX, 2*sizeof(short) );
				*shortPtr = town_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
		else
		{
			if( !remote.is_enable() )
			{
				set_auto_grant_loyalty(loyaltyLevel);
			}
			else
			{
				// packet structure <town recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_GRANT, 2*sizeof(short) );
				*shortPtr = town_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
	}
	else if( rc==2 )
	{
		// ####### begin Gilbert 11/9 ########//
		//----- set the national policy -----//
		se_ctrl.immediate_sound("TURN_ON");
		if( !remote.is_enable() )
		{
			Nation* nationPtr = nation_array[nation_recno];

			if( modeCollectTax )
				nationPtr->set_auto_collect_tax_loyalty(loyaltyLevel);
			else
				nationPtr->set_auto_grant_loyalty(loyaltyLevel);

		}
		else
		{
			err_when(!nation_recno);
			if( modeCollectTax )
			{
				// packet structure <-nation recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_TAX, 2*sizeof(short) );
				*shortPtr = -nation_recno;
				shortPtr[1] = loyaltyLevel;
			}
			else
			{
				// packet structure <-nation recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_GRANT, 2*sizeof(short) );
				*shortPtr = -nation_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
		// ####### end Gilbert 11/9 ########//
	}

	//--------------------------------------//

	if( button_cancel2.detect() || rc )
	{
		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_OFF");
		// ##### begin Gilbert 26/9 ########//
		town_menu_mode = TOWN_MENU_MAIN;
	//	info.disp();
	}
}
//----------- End of function Town::detect_auto_menu -----------//

//--------- Begin of function Town::detect_man_power ---------//
//
void Town::detect_man_power()
{
	int i;

	for( i=0; i<9; i++ )
	{
		if (button_loyalty_array[i].detect(0, 0, 1))
		{
			// ##### begin Gilbert 7/10 ######//
			err_here();
			if( !remote.is_enable() )
			{
				target_wall_man_power = i;
			}
			else
			{
				err_here();
			}
			// ##### end Gilbert 7/10 ######//
			se_ctrl.immediate_sound("TURN_OFF");
			town_menu_mode = TOWN_MENU_MAIN;
		//	info.disp();
		}
	}

	if( button_cancel2.detect())
	{
		se_ctrl.immediate_sound("TURN_OFF");
		town_menu_mode = TOWN_MENU_MAIN;
	//	info.disp();
	}
}
//----------- End of function Town::detect_man_power -----------//


//--------- Begin of function Town::player_spy_count ---------//
//
// Return the number of player spies in this town.
//
int Town::player_spy_count()
{
	if( !spy_count )
		return 0;

	//----- look for player spy in the spy_array -----//

	int  spyCount=0;
	Spy* spyPtr;

	for( int i=spy_array.size() ; i>=1 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==town_recno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			spyCount++;
		}
	}

	return spyCount;
}
//----------- End of function Town::player_spy_count -----------//


// --------- Begin of function Town::explore_for_player ------//
int Town::explore_for_player()
{
// ########## begin Gilbert 18/5 #########//
//	return Place::is_own()		// own or ally
//		|| player_spy_count() > 0;		// has spy inside
	return Place::explore_for_player()		// own or ally
		|| player_spy_count() > 0;		// has spy inside
// ########## end Gilbert 18/5 #########//
}
// --------- End of function Town::explore_for_player ------//


//--------- Begin of function spy_filter ---------//
//
static int spy_filter(int recNo)
{
	Spy* spyPtr;
	int  recCount=0;

	for( int i=spy_array.size() ; i>=1 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==Town::if_town_recno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			recCount++;
		}

		if( recNo && recCount==recNo )
			return i;
	}

	err_when( recNo );

	return recCount;
}
//----------- End of function spy_filter -----------//


//-------- Begin of static function put_spy_rec --------//
//
static void put_spy_rec(int recNo, int x, int y, int refreshFlag)
{
	int x2 = x+browse_spy.rec_width-1;
	String str;
	//-------- display icon of the spy unit -----//

	Spy* spyPtr = spy_array[ spy_filter(recNo) ];

	short nationRecno = town_array[town_array.selected_recno]->nation_recno;
	info.draw_unit_icon( x+10, y+15, race_res[spyPtr->race_id]->civilian_unit_id, nationRecno,
		INFO_X1, INFO_Y1, INFO_X2, INFO_Y2, 1);
	font_whbl.center_put( x, y, x+21, y+23, m.format(recNo,1));
	
	//--------- set help parameters --------//

//	if( mouse.in_area(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4) )
//	{
//		int unitId = race_res[spyPtr->race_id]->civilian_unit_id;
//		help.set_unit_help( unitId, 0, x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );
//	}

	//-------- display spy skill -------//
	str = spyPtr->spy_skill;
	font_whbl.center_put( INFO_X1+54, y+6, INFO_X1+76, y+20, str);

	//-------- display spy loyalty -------//
	str = spyPtr->spy_loyalty;
	font_whbl.center_put( INFO_X1+87, y+6, INFO_X1+127, y+20, str);

	//------ display the action mode of the spy ------//

//	vga_util.blt_buf( x+95, y+6, x2, y+5+font_san.height(), 0 );

	font_whbl.center_put( x+115, y+6, INFO_X2, y+20, spyPtr->action_str() );
}
//----------- End of static function put_spy_rec -----------//


int Town::input_town_name()
{
	#define SAVE_GAME_DESC_LEN 15
	char gameDesc[SAVE_GAME_DESC_LEN+1];
	strcpy( gameDesc,	town_name() );

	Blob2DW saveArea;
	Blob2DW keyFieldArea;

	int boxWidth = 500;
	int boxHeight = 78;

	int boxX1 = INFO_X1;
	int boxX2 = boxX1 + boxWidth - 1 -273;
	int boxY1 = INFO_Y1;
	int boxY2 = boxY1 + boxHeight - 1 + 15;

	char useBack = vga.use_back_buf;
	vga.use_front();

	// ------ capture save area -------//

	mouse.hide_area( boxX1, boxY1, boxX2, boxY2 );
	saveArea.clear();
	saveArea.resize( boxX1, boxY1, boxWidth, boxHeight );
	mouse.show_area();

	// ------ init get field -------//

	GetA descTextBox;
	vga_back.put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("VILLNAME") );
	descTextBox.init( boxX1+26, boxY1+12, boxX2-22, gameDesc, SAVE_GAME_DESC_LEN, &font_town, 1, 0 );
				
	// ------ ok cancel button --------//

	const int buttonWidth = 60;
	const int buttonHeight = 20;
	Button button_ok, button_cancel;

	int x = (2*boxX1 + boxX2) / 3 - buttonWidth / 2;
	int y = boxY2 - buttonHeight - 10;
	button_ok.create_text( x, y, x+buttonWidth-1, y+buttonHeight-1, text_basic.str_box_ok() );	// "Ok" );
	x = (boxX1 + 2*boxX2) / 3 - buttonWidth / 2 + 10;
	button_cancel.create_text( x, y, x+buttonWidth-1, y+buttonHeight-1, text_basic.str_box_cancel() );		// "Cancel" );
	
	// --------------------------------//

	int retFlag = 0;
	int refreshFlag = 1;

	while(1)
	{
		sys.yield();
		mouse.get_event();
		
		if( refreshFlag )
		{
			descTextBox.paint(1);

			// ------- paint button --------//

			button_ok.paint();
			button_cancel.paint();

			// --------------------------------//

			refreshFlag = 0;
		}

		sys.blt_virtual_buf();

		// ------- detect --------//

		if( descTextBox.detect() )
		{
			descTextBox.paint(1);
		}

		if( button_ok.detect(KEY_RETURN) )
		{
			retFlag = 1;
			break;
		}

		if( button_cancel.detect(KEY_ESC) )
		{
			retFlag = 0;
			break;
		}
	}

	// ------ restore save buffer ---------//

	vga_front.put_bitmapW( boxX1, boxY1, saveArea.bitmap_ptr() );

	// ------ update game_desc ----------//

	if( retFlag )
	{
		town_res.change_town_name( town_name_id, gameDesc );		
	}

	if(useBack)
		vga.use_back();

	return retFlag;
}
// ----- end of function GameFile::ask_desc -------//

// ------ Begin of function Town::disp_edit_mode -----------//
//
// return y, so that sub-class disp_edit_mode can continue at bottom
//
int Town::disp_edit_mode(int refreshFlag)
{
	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2;
	int x;
	int y = INFO_Y1 + 10;
	int i, curLoyalty, targetLoyalty;
//	String str;

	// ------- display race name --------//
//	if (race_res[race_id]->name[0] == 'E' ||
//		 race_res[race_id]->name[0] == 'I')
//		 str = "An ";
//	else
//		 str = "A ";
//	str += race_res[race_id]->name;
//	if( is_pay_tribute_to_monster() )
//		str += " Slave";
//	str += " Town";
		
	font_whbl.put( x1, y, 
		is_pay_tribute_to_monster() ? 
		text_firm.str_race_slave_town(race_res[race_id]->adjective) :
		text_firm.str_race_town(race_res[race_id]->adjective),
		0, x2 );
	y += 20;

	// display unit name
	font_whbl.put( x1, y, town_name(), 0, x2 );
	y += 20;

	// display population
	// x = font_whbl.put( x1, y, "Popluation", 0, x2 );
	x = font_whbl.put( x1, y, text_firm.str_town_population(), 0, x2 );
	x = edit_population_x2 = font_blue.put( (edit_population_x1=x+20), (edit_population_y1=y), population, 0, x2 );
	edit_population_y2 = edit_population_y1+font_blue.max_font_height;
	edit_population_enable = 1;
	x = font_whbl.put( x, y, " / ", 0, x2 );
	x = font_whbl.put( x, y, MAX_TOWN_POPULATION, 0, x2 );
	y += 20;

	// display loyalty
	for(i=1 ; i<=nation_array.size() ; i++ )
	{
		if (nation_array.is_deleted(i))
		{
			edit_loyalty_enable[i-1] = 0;
			continue;
		}
	
		Nation* nationPtr = nation_array[i];
		nationPtr->disp_nation_color(x1, y+3);

		if( nation_recno == i )
		{
			font_whbl.put( x1+15, y, text_firm.str_loyalty_to(nationPtr->nation_name()) , 0, x2 );
			curLoyalty    = (int) loyalty;
			targetLoyalty = (int) target_loyalty;
		}
		else
		{
			font_whbl.put( x1+15, y, text_firm.str_resistance_to(nationPtr->nation_name()), 0, x2 );
			curLoyalty    = (int) resistance(i);
			targetLoyalty = target_resistance_array[i-1];
		}
		if (targetLoyalty == -1)
			targetLoyalty = curLoyalty;

		y += font_blue.max_font_height;
		x = edit_loyalty_x2[i-1] = font_blue.put( (edit_loyalty_x1[i-1]=x1+50), 
			(edit_loyalty_y1[i-1]=y), curLoyalty, 0, x2 );
		edit_loyalty_y2[i-1] = edit_loyalty_y1[i-1]+font_blue.max_font_height;
		edit_loyalty_enable[i-1] = 1;
		x = font_whbl.put( x, y, " -> ", 0, x2 );
		x = font_whbl.put( x, y, targetLoyalty, 0, x2 );
		y += 15;
	}

	font_blue.put( x1, INFO_Y2-28, text_firm.str_left_inc() );
	font_blue.put( x1, INFO_Y2-14, text_firm.str_right_dec() );

	return y;
}
// ------ End of function Unit::disp_edit_mode -----------//


// ------ Begin of function Town::detect_edit_mode -----------//
//
void Town::detect_edit_mode()
{
	int i;
	if( edit_population_enable )
	{
		if( mouse.any_click(edit_population_x1, edit_population_y1, edit_population_x2, edit_population_y2, LEFT_BUTTON) )
		{
			if( population < MAX_TOWN_POPULATION )
			{
				// ##### begin Gilbert 6/7 #######//
				for( i = 10; i > 0 && population < MAX_TOWN_POPULATION; --i )
					inc_pop(loyalty);
				// ##### end Gilbert 6/7 #######//
				auto_set_layout();
			}
		}
		if( mouse.any_click(edit_population_x1, edit_population_y1, edit_population_x2, edit_population_y2, RIGHT_BUTTON) )
		{
			// ####### begin Gilbert 6/7 #####//
			for( i = 10; i > 0 && population > 0; --i )
				kill_town_people();
			auto_set_layout();
			// ####### end Gilbert 6/7 #####//
		}
	}
	for(i=1 ; i<=nation_array.size() ; i++ )
	{
		if (edit_loyalty_enable[i-1])
		{
			if( mouse.any_click(edit_loyalty_x1[i-1], edit_loyalty_y1[i-1], edit_loyalty_x2[i-1], edit_loyalty_y2[i-1], LEFT_BUTTON) )
			{
				if( nation_recno == i )
				{
					loyalty += 10;
					if( loyalty > 100 )
						loyalty = (float) 100;
				}
				else
				{
					resistance_array[i-1] += 10;
					if( resistance_array[i-1] > 100 )
					{
						resistance_array[i-1] = 100;
					}
				}
			}
			if( mouse.any_click(edit_loyalty_x1[i-1], edit_loyalty_y1[i-1], edit_loyalty_x2[i-1], edit_loyalty_y2[i-1], RIGHT_BUTTON) )
			{
				if( nation_recno == i )
				{
					loyalty -= 10;
					if( loyalty < 0 )
						loyalty = (float) 0;
				}
				else
				{
					resistance_array[i-1] -= 10;
					if( resistance_array[i-1] < 0 )
					{
						resistance_array[i-1] = 0;
					}
				}
			}
		}
	}
}
// ------ End of function Town::detect_edit_mode -----------//
