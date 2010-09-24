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

//Filename    : OFIRMIF.CPP
//Description : Firm interface routines

#include <OFIRM.H>
#include <OSTR.H>
#include <KEY.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OHELP.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBUTT3D.H>
#include <OBUTTON.H>
#include <OSLIDER.H>
#include <OCONFIG.H>
#include <ONATION.H>
#include <OINFO.H>
#include <ORACERES.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OTOWN.H>
#include <OWORLD.H>
#include <OREMOTE.H>
#include <OIMGRES.H>
#include <OGAME.H>
#include <OSYS.H>
#include <OSE.H>
#include <OSERES.H>
#include <OT_FIRM.H>
#include <OT_UNIT.H>

//---------- Define static variables ------------//

static Button3D button_sell, button_destruct, button_builder;
static short  	 pop_disp_y1;
static char	last_under_construction;
static int	last_should_show_info;
static int	last_is_own;

static int	edit_mode_y1;
static short edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, edit_hp_enable;

//---------- Declare static function ------------//

// static void disp_worker_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);

//--------- Begin of function Firm::disp_info_both ---------//
//
void Firm::disp_info_both(int refreshFlag)
{
#ifdef USE_FLIP
	vga.use_back();
#endif

	disp_basic_info(INFO_Y1, refreshFlag);

	// ##### begin Gilbert 7/9 ########//

	// ---- when under_construction/should_show_info/is_own() change, repaint ------- //

	if( refreshFlag == INFO_REPAINT 
		|| last_under_construction != under_construction
		|| last_should_show_info  != should_show_info()
		|| last_is_own          != is_own() )
	{
		last_under_construction = under_construction;
		last_should_show_info  = should_show_info();
		last_is_own          = is_own();
		// ##### begin Gilbert 5/1 #####//
		//if( refreshFlag != INFO_REPAINT )
		//{
		//	info.disp();
		//	return;
		//}
		refreshFlag = INFO_REPAINT;
		// ##### end Gilbert 5/1 #####//
	}
	// ##### end Gilbert 7/9 ########//

	if( under_construction )
	{
		// disp firm name
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("VILLCONS") );

		// font_vill.center_put( INFO_X1+113, INFO_Y1+152, INFO_X1+205, INFO_Y1+165, "Under" );
		// font_vill.center_put( INFO_X1+99, INFO_Y1+165, INFO_X1+217, INFO_Y1+185, "Construction" );
		font_vill.center_put_paragraph( INFO_X1+99, INFO_Y1+152, INFO_X1+217, INFO_Y1+185,
			text_firm.str_under_construction(), -5, 0, 0 );
		font_vill.center_put_paragraph( INFO_X1+20, INFO_Y1+103, INFO_X1+125, INFO_Y1+147, 
			firm_name(), -5, 0, 0 );

		// disp completion percentage
		int ratio( (int)hit_points * 100 / max_hit_points() );
		if( ratio < 0 )
			ratio = 0;
		if( ratio > 100 )
			ratio = 100;
		font_vill.center_put( INFO_X1+20, INFO_Y1+139, INFO_X1+125, INFO_Y1+171, 
			m.format(ratio, 3) );
	}
	else
		put_info(refreshFlag);			// virtual function

	#ifdef DEBUG
//		if(debug2_enable_flag)
//		{
//			font_san.d3_put( INFO_X1, INFO_Y2-30, INFO_X2, INFO_Y2, "" );
//			font_san.field( INFO_X1+10, INFO_Y2-20, " ", INFO_X1+20, firm_recno, 1, INFO_X2-10, refreshFlag);
//			font_san.field( INFO_X1+40, INFO_Y2-20, " ", INFO_X1+50, loc_x1, 1, INFO_X2-10, refreshFlag);
//			font_san.field( INFO_X1+70, INFO_Y2-20, " ", INFO_X1+80, loc_y1, 1, INFO_X2-10, refreshFlag);
//			font_san.field( INFO_X1+100, INFO_Y2-20, " ", INFO_X1+110, ai_link_checked, 1, INFO_X2-10, refreshFlag);
//		}
	#endif

#ifdef USE_FLIP
	vga.use_front();
#endif
}
//----------- End of function Firm::disp_info_both -----------//


//--------- Begin of function Firm::detect_info_both ---------//
//
// Called by Info::detect(). Detect both cases when the firm is
// under construction and is normal. 
//
void Firm::detect_info_both()
{
	// ###### begin Gilbert 24/2 ########//
	// detect_basic_info();
	if( detect_basic_info() )		// in case the firm is removed
		return;
	// ###### end Gilbert 24/2 ########//

	if( !under_construction )
		detect_info();
}
//----------- End of function Firm::detect_info_both -----------//


//--------- Begin of function Firm::disp_basic_info ---------//
//
void Firm::disp_basic_info(int dispY1, int refreshFlag)
{
	//------- display the name of the firm --------//
//	String str;
	
	// ------- display town name and hit point bar --------//
	if (firm_id != FIRM_WILDE_LISHORR && 
		 firm_id != FIRM_LISHORR &&
		 firm_id != FIRM_OFFENSIVE_BUILDING_3)
	{
//		if ( firm_id == FIRM_FORT )
//			str = firm_name(0);
//		else
//			str = firm_name();	
		font_snds.center_put( INFO_X1+15, INFO_Y1-19, INFO_X2, INFO_Y1-4, 
			firm_name(), 0, 1 );
		disp_hit_point(0);
		help.set_help( INFO_X1 +173, INFO_Y1 -62, INFO_X1 +202, INFO_Y1 -29, "F_ENENGY" );
	}

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	// ------- display button ------- //
	int x1 = INFO_X1 +13;
	int y1 = INFO_Y1 +235;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	// ------- create button ------- //
	int showRepairIcon = builder_recno && !under_construction && should_show_info() && hit_points < max_hit_points();

//	set_repair_flag( button_builder.pushed_flag, COMMAND_PLAYER );

	if( refreshFlag == INFO_REPAINT )
	{
		button_destruct.create( INFO_X1+16, INFO_Y1-61, "V_DEM-U", "V_DEM-D" );
		button_sell.create( INFO_X1+16, INFO_Y1-61, "V_SEL-U", "V_SEL-D" );
		button_sell.set_help_code( "SELLFIRM" );
	}

	if( nation_recno && nation_array.player_recno == nation_recno )
	{
		button_destruct.visible_flag = 0;
		button_sell.visible_flag = 0;

		if( under_construction || !can_sell() )
		{
			if( under_construction )
				button_destruct.set_help_code( "CANCELBLD" );
			else
				button_destruct.set_help_code( "DESTFIRM" );

			button_destruct.visible_flag = 1;
			button_destruct.paint();
		}
		else
		{
			button_sell.visible_flag = 1;
			button_sell.paint();
		}
	}
	// ####### begin Gilbert 30/11 #######//
	if( refreshFlag == INFO_REPAINT )
	{
		button_builder.create( INFO_X1+13, INFO_Y1-96, "REPAIRU", "REPAIRD", 0 );
		button_builder.set_help_code( "REPAIR" );
	}

	if( should_show_info() )
	{
		if( (button_builder.visible_flag = !under_construction && hit_points < max_hit_points() && firm_id != FIRM_LISHORR) )
		{
			if( repair_flag && hit_points < max_hit_points() )
			{
				button_builder.pushed_flag = repair_flag;
				button_builder.update_bitmap( ((sys.frame_count % 5) < 3) ? "REPAIRU" : "REPAIRD");
				button_builder.set_help_code( "REPAIRING" );
			}
			else
			{
				button_builder.pushed_flag = repair_flag;
				button_builder.update_bitmap( repair_flag ? "REPAIRU" : "REPAIRD");
				button_builder.set_help_code( "REPAIR" );
			}
		}
	}
	// ####### end Gilbert 30/11 #######//
}
//----------- End of function Firm::disp_basic_info -----------//


//--------- Begin of function Firm::detect_basic_info ---------//
//
int Firm::detect_basic_info()
{
	//--- click on the name area to center the map on it ---//

	if( mouse.single_click( INFO_X1+15, INFO_Y1-18, INFO_X2, INFO_Y1-3 ) )
	{
		world.go_loc( center_x, center_y );
		return 1;
	}
	
	//----------- Mobilize the builder ---------//

	// ###### begin Gilbert 30/11 ######//
//	int detectBuilder = builder_recno && !under_construction &&
//							  unit_array[builder_recno]->is_own();			// this is your unit in your firm or it is a spy of yours in an enemy firm

	int rc;
	if( is_own() && (rc = button_builder.detect(0, 0, 1)) )
	{
		if( under_construction )
		{
			if( !remote.is_enable() )
				set_builder(0);
			else
			{
				// packet structure : <firm recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_BUILDER, sizeof(short));
				*shortPtr = firm_recno;
			}
		}
		else
		{
			// ####### begin Gilbert 21/4 ##########//
			if( rc != 2 )
			{
				set_repair_flag( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			else	// right click to repair all buildings
			{
				nation_array[nation_recno]->repair_all_building( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			// ####### end Gilbert 21/4 ##########//
		}
		se_ctrl.immediate_sound("REPAIR");
		return 1;
	}
	// ###### end Gilbert 30/11 ######//

	//---------------------------------------//

	if( !is_own() )
		return 0;

	//---------- "Destruct" button -----------//

	// ####### begin Gilbert 27/1 #######//
	// if( button_destruct.detect(KEY_DEL) )
	if( button_destruct.detect() )	// if press del during chat, the firm destructed too
	// ####### end Gilbert 27/1 #######//
	{
		if( under_construction )
			cancel_construction(COMMAND_PLAYER);
		else
			destruct_firm(COMMAND_PLAYER);

		se_ctrl.immediate_sound("TURN_ON");

		return 1;
	}

	//------------ "Sell" button -------------//

	// ###### begin gilbert 27/1 ######//
	// if( button_sell.detect(KEY_DEL) )
	if( button_sell.detect() )
	// ###### end gilbert 27/1 ######//
	{
		sell_firm(COMMAND_PLAYER);
		se_ctrl.immediate_sound("TAXGRANT");
		return 1;
	}

	help.set_help( INFO_X1+16, INFO_Y1-28, INFO_X1+45, INFO_Y1+1, "COLCODE" );
	
	return 0;
}
//----------- End of function Firm::detect_basic_info -----------//

/*
//--------- Begin of function Firm::disp_worker_list ---------//
//
void Firm::disp_worker_list(int dispY1, int refreshFlag)
{
	pop_disp_y1 = dispY1;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+60 );

	//----------- display populatin distribution ---------//

	int overseerRaceId=0;

	if( overseer_recno )
		overseerRaceId = unit_array[overseer_recno]->race_id;

	if( selected_worker_id > worker_count )
		selected_worker_id = worker_count;

	//------ display population composition -------//

	int	  x, y;
	Worker* workerPtr = worker_array;
	static  char last_race_id_array[MAX_WORKER];
	static  char last_unit_id_array[MAX_WORKER];

	dispY1+=1;

	for( int i=0 ; i<MAX_WORKER ; i++, workerPtr++ )
	{
		x = INFO_X1+4+i%4*50;
		y = dispY1+i/4*29;

		if( i<worker_count )
		{
			if( refreshFlag==INFO_REPAINT ||
				 last_race_id_array[i] != workerPtr->race_id ||
				 last_unit_id_array[i] != workerPtr->unit_id )
			{
				vga_front.put_bitmap(x+2, y+2, workerPtr->small_icon_ptr());
			}

			//----- highlight the selected worker -------//

			if( selected_worker_id == i+1 )
				vga_front.rect( x, y, x+27, y+23, 2, V_YELLOW );
			else
				vga_front.rect( x, y, x+27, y+23, 2, vga_front.color_up );

			//------ display hit points bar --------//

			disp_worker_hit_points( x+2, y+24, x+25, workerPtr->hit_points, workerPtr->max_hit_points() );

			//----- display combat or skill level ------//

			char* spyIconName=NULL;

			if( workerPtr->spy_recno )
			{
				Spy* spyPtr = spy_array[workerPtr->spy_recno];

				//------ if this is the player's spy -------//

				if( nation_array.player_recno &&
					 spyPtr->true_nation_recno == nation_array.player_recno )
				{
					spyIconName = "U_SPY";
				}

				//--------------------------------------------//
				//
				// If this is an enemy spy and this firm belongs
				// to the player and there is a player's phoenix
				// over this firm and the spying skill of the spy
				// is low (below 40)
				//
				//--------------------------------------------//

//				else if( spyPtr->spy_skill < 40 &&
//							nation_recno == nation_array.player_recno &&
//							nation_array.player_recno &&
//					 (~nation_array)->revealed_by_phoenix(loc_x1, loc_y1) )
//				{
//					spyIconName = "ENEMYSPY";
//				}

			}

			//--------------------------------------//

			if( spyIconName )
			{
				vga_front.put_bitmap( x+30, y+6, image_icon.get_ptr(spyIconName) );
				vga.blt_buf( x+40, y+6, x+49, y+15, 0 );
				vga.blt_buf( x+30, y+16, x+49, y+26, 0 );
			}
			else
			{
				if( firm_id == FIRM_CAMP )
					font_san.disp(x+30, y+6, workerPtr->combat_level, 1, x+49);
				else
					font_san.disp(x+30, y+6, workerPtr->skill_level, 1, x+49);
			}

			last_race_id_array[i] = workerPtr->race_id;
			last_unit_id_array[i] = workerPtr->unit_id;

			//------- set help parameters ---------//

			if( mouse.in_area(x, y, x+27, y+23) )
				help.set_unit_help( workerPtr->unit_id, 0, x, y, x+27, y+23 );
		}
		else
		{
			if( last_race_id_array[i] != 0 || last_unit_id_array[i] != 0 )
			{
				vga.blt_buf( x, y, x+49, y+27, 0 );
				last_race_id_array[i] = 0;
				last_unit_id_array[i] = 0;
			}
		}
	}
}
//----------- End of function Firm::disp_worker_list -----------//


//--------- Begin of function Firm::detect_worker_list ---------//
//
int Firm::detect_worker_list()
{
	if( !should_show_info() )
		return 0;

	//------- detect buttons on hiring firm workers -------//

	int i, x, y;
	int liveInTown = firm_res[firm_id]->live_in_town;

	for( i=0 ; i<worker_count ; i++ )
	{
		x = INFO_X1+6+i%4*50;
		y = pop_disp_y1+1+i/4*29;

		switch( mouse.single_click(x, y, x+27, y+23, 2) )
		{
			case 1:         // left button to select worker
				selected_worker_id = i+1;
				return 1;

			case 2:
				if( is_own() )		// only if this is our own firm
				{
					//--- if the town where the unit lives belongs to the nation of this firm ---//

					mobilize_worker(i+1, COMMAND_PLAYER);
					return 1;
				}
				break;
		}
	}

	return 0;
}
//----------- End of function Firm::detect_worker_list -----------//


//--------- Begin of function disp_worker_hit_points ---------//
//
static void disp_worker_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
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

	int barWidth = (x2-x1+1) * hitPoints / max(hitPoints, maxHitPoints);

	vga_front.bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

	if( x1+barWidth <= x2 )
		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga_front.bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga_front.bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

	if( x1+barWidth+1 <= x2 )
		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga_front.bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

	if( x1+barWidth+1 <= x2 )
		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_worker_hit_points -----------//


//--------- Begin of function Firm::disp_worker_info ---------//
//
void Firm::disp_worker_info(int dispY1, int refreshFlag)
{
	static int lastSelected;

	if( selected_worker_id > worker_count )
		selected_worker_id = worker_count;

	if( lastSelected != selected_worker_id > 0 )
	{
		lastSelected = selected_worker_id > 0;
		info.disp();			// redisplay the interface
		return;
	}

	//------ if selected_worker_id==0, display overseer info -----//

	if( selected_worker_id==0 )		// display overseer info
	{
		disp_overseer_info(dispY1, refreshFlag);
		return;
	}

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
	{
		if( firm_id == FIRM_CAMP )		// the command base has one more field
			vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+71 );
		else
			vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+55 );
	}

	if( selected_worker_id > 0 )
	{
		int x=INFO_X1+4, y=dispY1+4;

		Worker* workerPtr = worker_array + selected_worker_id - 1;

		if( workerPtr->town_recno )		// FirmInfo::live_in_town is 1
		{
			Town* townPtr = town_array[workerPtr->town_recno];
			font_san.field( x, y, "Residence", x+100, townPtr->town_name(), INFO_X2-2, refreshFlag);
			y+=16;

			if( town_array[workerPtr->town_recno]->nation_recno == nation_recno &&
				 workerPtr->race_id )
			{
				info.disp_loyalty( x, y, x+60, workerPtr->loyalty(), workerPtr->target_loyalty(firm_recno), nation_recno, refreshFlag );
			}
		}
		else										// FirmInfo::live_in_town is 0
		{
			if( workerPtr->race_id )
				info.disp_loyalty( x, y, x+60, workerPtr->loyalty(), workerPtr->target_loyalty(firm_recno), nation_recno, refreshFlag );
		}

		y+=16;

		//----------------------------------------//

		String str;

		if( workerPtr->race_id )
			str = m.format(workerPtr->skill_level, 1);
		else
			str = "N/A";

		font_san.field( x, y, Skill::skill_str_array[workerPtr->skill_id-1],
			x+100, str, INFO_X2-2, refreshFlag);

		y+=16;

		//----------------------------------------//

		if( firm_id == FIRM_CAMP )
		{
			if( workerPtr->race_id )
				str = m.format(workerPtr->combat_level, 1);
			else
				str = "N/A";

			font_san.field( x, y, "Combat", x+100, str, INFO_X2-2, refreshFlag);
			y+=16;

			//---------------------------------------//

			str  = workerPtr->hit_points;
			str += "/";
			str += workerPtr->max_hit_points();

			font_san.field( x, y, "Hit Points", x+100, str, INFO_X2-2, refreshFlag);
		}
	}
}
//----------- End of function Firm::disp_worker_info -----------//


//--------- Begin of function Firm::disp_overseer_info ---------//
//
void Firm::disp_overseer_info(int dispY1, int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		if( firm_id == FIRM_CAMP )
			vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+71 );
		else
			vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+55 );
	}

	if( !overseer_recno )
		return;

	int x=INFO_X1+4, y=dispY1+4;

	Unit* unitPtr = unit_array[overseer_recno];

	if( unitPtr->rank_id != RANK_KING )
	{
		info.disp_loyalty( x, y, x+100, unitPtr->loyalty, unitPtr->target_loyalty, nation_recno, refreshFlag );
		y+=16;
	}

	font_san.field( x, y, Skill::skill_str_array[unitPtr->skill.skill_id-1],
		x+100, unitPtr->skill.skill_level , 1, INFO_X2-2, refreshFlag);

	y+=16;

	if( firm_id==FIRM_CAMP )		// only display combat level in camps and don't display it in seat of power
	{
		font_san.field( x, y, "Combat" , x+100, unitPtr->skill.combat_level, 1, INFO_X2-2, refreshFlag);
		y+=16;
	}

	String str;
	str  = (int) unitPtr->hit_points;
	str += "/";
	str += unitPtr->max_hit_points;

	font_san.field( x, y, "Hit Points", x+100, str, INFO_X2-2, refreshFlag);
}
//----------- End of function Firm::disp_overseer_info -----------//
*/

//----------- Begin of function Firm::disp_hit_point -----------//
void Firm::disp_hit_point(int dispY1)
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

	// ###### begin Gilbert 19/1 ########//
//	if( config.show_debug_info )
//	{
		if( max_hit_points() < 1000 )
			image_icon.put_back( INFO_X1 +173, INFO_Y1 -62, "REPAIRBX");
		font_san.put( INFO_X1+177, INFO_Y1-59, (int) hitPoints );
		font_san.put( INFO_X1+177, INFO_Y1-45, (int) max_hit_points() );
//	}
	// ###### end Gilbert 19/1 ########//
}
//----------- End of function Firm::disp_hit_point -----------//

//----------- Begin of function Firm::put_info -----------//
void Firm::put_info(int refreshFlag)
{
	vga.active_buf->put_bitmapW( INFO_X1, INFO_Y1, info.info_background_bitmap );
}
//----------- End of function Firm::put_info -----------//


// -------- begin of function Firm::disp_edit_mode -------//
//
// <int&> refreshFlag  INFO_REPAINT or INFO_UPDATE
// <int&> dispY1,      start y1 of editing
// [int]  dispBG,      whether to display background (default:true)
//
// beware int& refreshFlag, so it can inform subclass disp_edit_mode to change to INFO_REPAINT
//
void Firm::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if subclass disp_edit_mode already painted background,
	// pass dispBG=0
	if( dispBG )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("SCEN-ED") );
	}

	// put nation color code

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// set edit_mode_y1 for detect

	edit_mode_y1 = y;
	y += 15;

	// disp firm name

	disp_basic_info(INFO_Y1, refreshFlag);

	font_zoom.put( x1, y, firm_name(), 0, x2 );
	y += 18;

	// edit hit point

	edit_hp_enable = 1;
	x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
	x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)hit_points, 0, x2 );
	edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
	x = font_whbl.put( x, y, " / ", 0, x2 );
	x = font_whbl.put( x, y, max_hit_points(), 0, x2 );
	y += 18;

	// disp left click to inc, right click to dec.

	font_blue.put( x1, INFO_Y2-28, text_unit.str_left_inc() );  // "Left click to increase" );
	font_blue.put( x1, INFO_Y2-14, text_unit.str_right_dec() ); // "Right click to decrease" );
}
// -------- end of function Firm::disp_edit_mode -------//


// -------- begin of function Firm::detect_edit_mode -------//
//
int Firm::detect_edit_mode()
{
	if( edit_hp_enable )
	{
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
		{
			hit_points += 50.0f;
			if( hit_points > (float) max_hit_points() )
				hit_points = (float) max_hit_points();
			return 1;
		}
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
		{
			hit_points -= 50.0f;
			if( hit_points < 1.0f )
				hit_points = 1.0f;
			return 1;
		}
	}
	return 0;
}
// -------- end of function Firm::detect_edit_mode -------//

