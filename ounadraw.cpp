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

//Filename    : OUNADRAW.CPP
//Description : Object UnitArray draw functions.

#include <ALL.H>
#include <OVGA.H>
#include <OGAME.H>
#include <OCONFIG.H>
#include <OPFIND.H>
#include <OSYS.H>
#include <OMODEID.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OANLINE.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OU_CARA.H>
#include <OFIRM.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OGODRES.H>
#include <COLCODE.H>

//--------- Declare static functions ---------//

static void put_profile(int y, char* dispDes, int dispValue);

//--------- Begin of function UnitArray::draw_dot ---------//
//
// Draw 2x2 tiny squares on map window representing the location of the unit ---//
//
// [int] filterUnitClassId - if given, only display units whose type is filterUnitClassId
//
void UnitArray::draw_dot(int filterUnitClassId)
{
	int		i, j, mapX, mapY;
	short		nationColor;
	int		vgaBufPitch = vga_back.buf_pitch();
	short		playerNationRecno = nation_array.player_recno;

	short		lineFromX, lineFromY, lineToX, lineToY;

	short unexploredColor = vga_back.translate_color(UNEXPLORED_COLOR);
	short	nationColorArray[MAX_NATION+1];

	for( int n = 0; n < MAX_NATION+1; ++n )
		nationColorArray[n] = vga_back.translate_color(nation_array.nation_color_array[n]);

	// ##### begin Gilbert 2/11 ######//
	short	excitedColorArray[MAX_NATION+1];  //[excitedColorCount];
	short	slowExcitedColorArray[MAX_NATION+1];  //[excitedColorCount];

	for( i = 0; i <= MAX_NATION; ++i )
	{
		if( i == 0 || !nation_array.is_deleted(i) )
		{
			short *remapTable = game.get_color_remap_table(i, 0);
			excitedColorArray[i] = remapTable[0xe0 + (sys.frame_count % 4)];
			slowExcitedColorArray[i] = remapTable[0xe0 + (sys.frame_count % 8)/2];
		}
		else
		{
			excitedColorArray[i] = 
			slowExcitedColorArray[i] = vga_back.translate_color(V_WHITE);
		}
	}
	// ##### end Gilbert 2/11 ######//

	int arraySize = size();

	//------------- set boundary of anim_line to mini-map ------------//

	short animLineBoundX1 = anim_line.bound_x1;
	short animLineBoundY1 = anim_line.bound_y1;
	short animLineBoundX2 = anim_line.bound_x2;
	short animLineBoundY2 = anim_line.bound_y2;
	anim_line.bound_x1 = MAP_X1;
	anim_line.bound_y1 = MAP_Y1;
	anim_line.bound_x2 = MAP_X2;
	anim_line.bound_y2 = MAP_Y2;

	// #### begin Gilbert 29/1 ########//
	int showAttackerPath = nation_array.player_recno
		&& god_res[GOD_JAPANESE]->nation_prayer_count(nation_array.player_recno);
	int playerNation = nation_array.player_recno;
	// #### end Gilbert 29/1 ########//

	for(i=1; i<=arraySize; i++)
	{
		register Unit *unitPtr = (Unit*)get_ptr(i);

		if( !unitPtr || !unitPtr->is_visible() || unitPtr->is_stealth() )
			continue;

		int lineColor = TRANSPARENT_CODE;

		if( !unitPtr->selected_flag )		// if the unit is currently selected, ignore the filters
		{
			if( filterUnitClassId && unit_res[unitPtr->unit_id]->unit_class != filterUnitClassId )
				continue;

			// draw only caravan in trade mode
			if( world.map_matrix->map_mode == MAP_MODE_TRADE && unitPtr->unit_id != UNIT_CARAVAN )
				continue;

			// filter selected nation
			if( world.map_matrix->filter_nation_flag && world.map_matrix->filter_nation_recno != unitPtr->nation_recno )
				continue;
		}

		//---------------- draw unit path in mini_map ------------//

		if( (config.show_unit_path & 2) && unitPtr->selected_flag
			&& (config.show_ai_info || !playerNation || unitPtr->is_nation(playerNation))
			&& unitPtr->cur_action != SPRITE_IDLE)
		{
			if( unitPtr->mobile_type == UNIT_SEA )
				lineColor = V_WHITE;
			else
				lineColor = V_BLACK;
		}

		// ------ show unit targeting our nation  ----------//

		else if( showAttackerPath && unitPtr->cur_order.mode == UNIT_ATTACK
			&& unitPtr->cur_action != SPRITE_ATTACK		// if already attack, not need to show path
			&& !base_obj_array.is_deleted(unitPtr->cur_order.para)
			&& base_obj_array[unitPtr->cur_order.para]->nation_recno == playerNation )
		{
			lineColor = VGA_RED;
		}

		if( lineColor != TRANSPARENT_CODE )
		{
			//-----------------------------------------------------------//
			
			if(unitPtr->cur_x_loc()!=unitPtr->go_x_loc() || unitPtr->cur_y_loc()!=unitPtr->go_y_loc())
			{
				lineToX = world.map_matrix->calc_loc_pos_x(unitPtr->go_x_loc(), unitPtr->go_y_loc()) + MAP_X1;
				lineToY = world.map_matrix->calc_loc_pos_y(unitPtr->go_x_loc(), unitPtr->go_y_loc()) + MAP_Y1;
				lineFromX = world.map_matrix->calc_loc_pos_x(unitPtr->next_x_loc(), unitPtr->next_y_loc()) + MAP_X1;
				lineFromY = world.map_matrix->calc_loc_pos_y(unitPtr->next_x_loc(), unitPtr->next_y_loc()) + MAP_Y1;
				vga_back.line(lineFromX, lineFromY, lineToX, lineToY, lineColor);
			}

			//-----------------------------------------------------------//

			if( unitPtr->cur_path && unitPtr->cur_path_result_id > 0 )
			{
				int locFromX, locFromY, locToX, locToY;

				locFromX = unitPtr->go_x_loc();
				locFromY = unitPtr->go_y_loc();

				for( j=unitPtr->cur_path_result_id ; j>0 ; j-- )
				{
					PathResult* pathResult = unitPtr->cur_path + j - 1;

					locToX = pathResult->loc_x;
					locToY = pathResult->loc_y;

					lineFromX = world.map_matrix->calc_loc_pos_x(locFromX, locFromY) + MAP_X1;
					lineFromY = world.map_matrix->calc_loc_pos_y(locFromX, locFromY) + MAP_Y1;

					lineToX = world.map_matrix->calc_loc_pos_x(locToX, locToY) + MAP_X1;
					lineToY = world.map_matrix->calc_loc_pos_y(locToX, locToY) + MAP_Y1;

					vga_back.line(lineFromX, lineFromY, lineToX, lineToY, lineColor);

					locFromX = locToX;
					locFromY = locToY;
				}
			}
		}
	}

	//---------------- restore boundary setting of anim_line --------------//

	anim_line.bound_x1 = animLineBoundX1;
	anim_line.bound_y1 = animLineBoundY1;
	anim_line.bound_x2 = animLineBoundX2;
	anim_line.bound_y2 = animLineBoundY2;

	for(i=1; i<=arraySize; i++)
	{
		// #### begin Gilbert 3/11 ######//
		int dotSize;

		{ // minimize register variable at one time
			register Unit *unitPtr = (Unit*)get_ptr(i);

			if( !unitPtr || !unitPtr->is_visible() || unitPtr->is_stealth())
				continue;

			if( !unitPtr->selected_flag )		// if the unit is currently selected, ignore the filters
			{
				// draw only caravan in trade mode
				if( world.map_matrix->map_mode == MAP_MODE_TRADE && unitPtr->unit_id != UNIT_CARAVAN )
					continue;

				if( filterUnitClassId && unit_res[unitPtr->unit_id]->unit_class != filterUnitClassId )
					continue;

				// filter selected nation
				if( world.map_matrix->filter_nation_flag && world.map_matrix->filter_nation_recno != unitPtr->nation_recno )
					continue;
			}

			//---------------------------------//

			if( world.map_matrix->map_mode != MAP_MODE_TRADE )
			{
				if( unitPtr->cur_action == SPRITE_ATTACK )
					nationColor = excitedColorArray[unitPtr->nation_recno];
				else if( unitPtr->selected_flag )
					nationColor = slowExcitedColorArray[unitPtr->nation_recno];
				else
					nationColor = nationColorArray[unitPtr->nation_recno];
			}
			else
			{
				// if selected firm is mine, factory or market,
				// caravan linked to it flashes

				err_when( MAX_STOP_FOR_CARAVAN != 2 ); // current code check only two stops
				Firm *firmPtr;
				if( firm_array.selected_recno 
					&& (firmPtr = firm_array[firm_array.selected_recno])
					&& (firmPtr->cast_to_FirmMine() || firmPtr->cast_to_FirmFactory() || firmPtr->cast_to_FirmMarket())
					&& ( ((UnitCaravan *)unitPtr)->stop_array[0].firm_recno == firm_array.selected_recno
						|| ((UnitCaravan *)unitPtr)->stop_array[1].firm_recno == firm_array.selected_recno) )
				{
					nationColor = slowExcitedColorArray[unitPtr->nation_recno];
				}
				else
				{
					nationColor = nationColorArray[unitPtr->nation_recno];
				}
			}

			mapX = world.map_matrix->calc_loc_pos_x(unitPtr->cur_x_loc(), unitPtr->cur_y_loc()) + MAP_X1;
			mapY = world.map_matrix->calc_loc_pos_y(unitPtr->cur_x_loc(), unitPtr->cur_y_loc()) + MAP_Y1;
			dotSize = unitPtr->mobile_type == UNIT_LAND ? 2 : 3;
		}

		{ // minimize register variable at one time
			register short *writePtr = vga_back.buf_ptr(mapX, mapY);

			if( dotSize == 2 )
			{
				if( writePtr[0] != unexploredColor )
					writePtr[0] = nationColor;

				if( writePtr[1] != unexploredColor )
					writePtr[1] = nationColor;

				if( writePtr[vgaBufPitch] != unexploredColor )
					writePtr[vgaBufPitch] = nationColor;

				if( writePtr[vgaBufPitch+1] != unexploredColor )
					writePtr[vgaBufPitch+1] = nationColor;
			}
			else if( dotSize == 3 )
			{
				if( writePtr[-vgaBufPitch-1] != unexploredColor )
					writePtr[-vgaBufPitch-1] = nationColor;

				if( writePtr[-vgaBufPitch] != unexploredColor )
					writePtr[-vgaBufPitch] = nationColor;

				if( writePtr[-vgaBufPitch+1] != unexploredColor )
					writePtr[-vgaBufPitch+1] = nationColor;

				if( writePtr[-1] != unexploredColor )
					writePtr[-1] = nationColor;

				if( writePtr[0] != unexploredColor )
					writePtr[0] = nationColor;

				if( writePtr[1] != unexploredColor )
					writePtr[1] = nationColor;

				if( writePtr[vgaBufPitch-1] != unexploredColor )
					writePtr[vgaBufPitch-1] = nationColor;

				if( writePtr[vgaBufPitch] != unexploredColor )
					writePtr[vgaBufPitch] = nationColor;

				if( writePtr[vgaBufPitch+1] != unexploredColor )
					writePtr[vgaBufPitch+1] = nationColor;
			}
		}
		// #### end Gilbert 3/11 ######//
	} //end for
}
//----------- End of function UnitArray::draw_dot -----------//

/*
//----------- Begin function UnitArray::draw_profile---------//
void UnitArray::draw_profile()
{
#ifdef DEBUG
	static unsigned long lastDrawTime = m.get_time();

	if(m.get_time() >= lastDrawTime + 1000)
	{
		//----update pathfind profile-----//
		last_pathfind_profile_time = pathfind_profile_time;
		pathfind_profile_time = 0L;
		last_unit_ai_profile_time = unit_ai_profile_time;
		unit_ai_profile_time = 0L;

		lastDrawTime = m.get_time();
	}

	//---- draw unit process profile -----//
	int y=ZOOM_Y1+100;
	put_profile(y	  , "Unit AI:"	   	, last_unit_ai_profile_time );
	put_profile(y+=20, "PathFinder:"   	, last_pathfind_profile_time );
#endif
}
//----------- End of function UnitArray::draw_profile----------//
*/

//--------- Begin of function UnitArray::draw_profile ---------//
void UnitArray::draw_profile()
{
#ifdef DEBUG
	static unsigned long lastDrawTime = m.get_time();

	if(m.get_time() >= lastDrawTime + 1000)
	{
		//--------- update unit process profile ---------//
		last_unit_ai_profile_time = unit_ai_profile_time;
		unit_ai_profile_time = 0L;
		last_unit_profile_time = unit_profile_time;
		unit_profile_time = 0L;
		//----update pathfind profile-----//
		last_pathfind_profile_time = pathfind_profile_time;
		pathfind_profile_time = 0L;
		// ##### begin Gilbert 27/4 ########//
		last_pathfind_count = pathfind_count;
		pathfind_count = 0L;
		last_longest_pathfind_profile_time = longest_pathfind_profile_time;
		longest_pathfind_profile_time = 0L;
		last_longest_pathfind_unit_recno = longest_pathfind_unit_recno;
		longest_pathfind_unit_recno = 0L;
		// ##### end Gilbert 27/4 ########//
		
		//--------- update sprite process profile --------//
//		last_sprite_array_profile_time = sprite_array_profile_time;
//		sprite_array_profile_time = 0L;
		//------- update sprite cur_action profile ---------//
		last_sprite_idle_profile_time = sprite_idle_profile_time;
		sprite_idle_profile_time = 0L;
		last_sprite_move_profile_time = sprite_move_profile_time;
		sprite_move_profile_time = 0L;
		last_sprite_wait_profile_time = sprite_wait_profile_time;
		sprite_wait_profile_time = 0L;

		last_sprite_attack_profile_time = sprite_attack_profile_time;
		sprite_attack_profile_time = 0L;

		last_unit_attack_profile_time = unit_attack_profile_time;
		unit_attack_profile_time = 0L;

		last_unit_assign_profile_time = unit_assign_profile_time;
		unit_assign_profile_time = 0L;
		
		
		last_unit_process_town_defender_profile_time = unit_process_town_defender_profile_time;
		unit_process_town_defender_profile_time = 0L;

		last_unit_process_camp_defender_profile_time = unit_process_camp_defender_profile_time;
		unit_process_camp_defender_profile_time = 0L;

		last_unit_process_rebel_profile_time = unit_process_rebel_profile_time;
		unit_process_rebel_profile_time = 0L;

		last_unit_execute_move_profile_time = unit_execute_move_profile_time;
		unit_execute_move_profile_time = 0L;

		last_unit_execute_attack_profile_time = unit_execute_attack_profile_time;
		unit_execute_attack_profile_time = 0L;
		
		last_unit_execute_build_firm_profile_time = unit_execute_build_firm_profile_time;
		unit_execute_build_firm_profile_time = 0L;
		
		last_unit_execute_settle_town_profile_time = unit_execute_settle_town_profile_time;
		unit_execute_settle_town_profile_time = 0L;

		last_unit_execute_assign_profile_time = unit_execute_assign_profile_time;
		unit_execute_assign_profile_time = 0L;

		last_unit_cast_power_profile_time = unit_cast_power_profile_time;
		unit_cast_power_profile_time = 0L;

		last_unit_transform_fortress_profile_time = unit_transform_fortress_profile_time;
		unit_transform_fortress_profile_time = 0L;

		/*
		last_unit_check_build_firm_profile_time = unit_check_build_firm_profile_time;
		unit_check_build_firm_profile_time = 0L;
		*/
		lastDrawTime = m.get_time();
	}

	//------------ draw unit process profile -------------//

	int y=ZOOM_Y1+120;

	put_profile(y	  , "Unit AI:"	   	, last_unit_ai_profile_time );
	put_profile(y+=20, "Unit:"			, last_unit_profile_time );
//	put_profile(y+=20, "Sprite:"	   	, last_sprite_array_profile_time );
	put_profile(y+=20, "PathFinder:"   	, last_pathfind_profile_time );
	//####### begin Gilbert 27/4 #########//
	put_profile(y+=12, "count:"   	   , last_pathfind_count );
	put_profile(y+=12, "longest:"   	   , last_longest_pathfind_profile_time );
	//####### end Gilbert 27/4 #########//
	
	put_profile(y+=15, "Sprite Idle:"	, last_sprite_idle_profile_time );
	put_profile(y+=15, "Sprite Move:"	, last_sprite_move_profile_time );
	put_profile(y+=15, "Sprite Wait:"	, last_sprite_wait_profile_time );
	put_profile(y+=15, "Sprite Attack:" , last_sprite_attack_profile_time );
	
	put_profile(y+=15, "Unit Attack:"   , last_unit_attack_profile_time );
	put_profile(y+=15, "Unit Assign:"   , last_unit_assign_profile_time );
	

	put_profile(y+=15, "Defend Town"	, last_unit_process_town_defender_profile_time);
	put_profile(y+=15, "Defend Camp"	, last_unit_process_camp_defender_profile_time);
	put_profile(y+=15, "Rebel"			, last_unit_process_rebel_profile_time);
	put_profile(y+=15, "Execute Move"	, last_unit_execute_move_profile_time);

	put_profile(y+=15, "Execute Attack"	, last_unit_execute_attack_profile_time);
	

	put_profile(y+=15, "Build Firm"		, last_unit_execute_build_firm_profile_time);
//	put_profile(y+=15, "Check Build"	, last_unit_check_build_firm_profile_time);
	put_profile(y+=15, "Settle Town"	, last_unit_execute_settle_town_profile_time);

	put_profile(y+=15, "Execute Assign" ,last_unit_execute_assign_profile_time);
	put_profile(y+=15, "Cast Power"		, last_unit_cast_power_profile_time);
	put_profile(y+=15, "Fortress"		, last_unit_transform_fortress_profile_time);
	
	put_profile(y+=15, "Current MouX:" , mouse.cur_x);
	put_profile(y+=15, "Current MouY:" , mouse.cur_y);
	/*
	put_profile(y+=20, "ZOOM_X1" , ZOOM_X1);
	put_profile(y+=20, "ZOOM_X2" , ZOOM_X2);
	put_profile(y+=20, "ZOOM_Y1" , ZOOM_Y1);
	put_profile(y+=20, "ZOOM_Y2" , ZOOM_Y2);
	*/

#endif
}
//----------- End of function UnitArray::draw_profile -----------//

//--------- Begin of static function put_profile ---------//
//
static void put_profile(int y, char* dispDes, int dispValue)
{
	font_news.put( ZOOM_X1+10, y, dispDes );
	font_news.put( ZOOM_X1+120, y, dispValue );
}
//----------- End of static function put_profile -----------//

