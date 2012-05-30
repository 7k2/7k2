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

//Filename    : OTOWNDRW.CPP
//Description : Town drawing routines

#include <otown.h>
#include <otwalres.h>
#include <ovga.h>
#include <omodeid.h>
#include <osys.h>
#include <ofont.h>
#include <omouse.h>
#include <obutton.h>
#include <oinfo.h>
#include <oimgres.h>
#include <opower.h>
#include <oworld.h>
#include <oworldmt.h>
#include <ogame.h>
#include <oremote.h>
#include <oanline.h>
#include <oplant.h>
#include <onation.h>
#include <of_lair.h>
#include <ose.h>
#include <ocoltbl.h>
#include <obitmap.h>
#include <ohelp.h>
#include <ou_wagon.h>

// ------ define constant -------- //
// x of bottom_left corner
const int TOWN_ADJ_OFFSET_X = STD_TOWN_LOC_HEIGHT*ZOOM_LOC_Y_WIDTH - 25;
// y of top_left_corner
const int TOWN_ADJ_OFFSET_Y = -7;

//------- Begin of function Town::draw -----------//
//
// Draw the town section on the zoom map
//
// [int] displayLayer   : 1 = normal layer          (default : 1)
//                      : 2 = layer above the town
//                      : 4 = layer below the town
//
void Town::draw(int displayLayer)
{
	TownLayout* townLayout = town_res.get_layout(layout_id);
	TownSlot*   townSlot   = town_res.get_slot(townLayout->first_slot_recno);

	int sectionX1 = loc_x1 * LOCATE_WIDTH;
	int sectionY1 = loc_y1 * LOCATE_HEIGHT;
	short *remapTable;

	if( under_construction )
	{
		// ##### begin Gilbert 9/10 ######//
		remapTable = (short *)vga.vga_color_table->get_table(	
			construction_completion_percent * MAX_BRIGHTNESS_ADJUST_DEGREE / 2 / 100 
			 + (-MAX_BRIGHTNESS_ADJUST_DEGREE + MAX_BRIGHTNESS_ADJUST_DEGREE*4/10) );		// brightness change from 40% to 90%
		// ##### end Gilbert 9/10 ######//
	}
	else
		remapTable = NULL;

	if( displayLayer==4 )
	{
		// ------- draw ground ---------//

		world.zoom_matrix->put_bitmap_offset(sectionX1, sectionY1, altitude, townLayout->ground_bitmap_ptr,
			TOWN_ADJ_OFFSET_X, TOWN_ADJ_OFFSET_Y,
			remapTable, 0, 1);

		//-------- draw farm --------//

		for( int i=0 ; i<townLayout->slot_count ; i++, townSlot++ )
		{
			//----- build_type==0 if plants -----//

			switch(townSlot->build_type)
			{
				case TOWN_OBJECT_FARM:
					draw_farm(sectionX1+townSlot->base_x, sectionY1+townSlot->base_y, townSlot->build_code, remapTable );
					break;
			}
		}
	}

	else if( displayLayer == 1 )
	{
		//-------- draw back wall ------------//

		if( (hit_points >= max_hit_points())
			&& target_wall_level > current_wall_level )
		{

			int wall_construction_percent = upgrade_wall_progress *100 /MAX_WALL_PROGRESS;

			
			short* remapTable = (short *)vga.vga_color_table->get_table(	
				MAX_BRIGHTNESS_ADJUST_DEGREE - 
				(wall_construction_percent) * MAX_BRIGHTNESS_ADJUST_DEGREE / 100);
		
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, current_wall_level + 1,
				100, 100, 'B' );		// wall back sides

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					remapTable, 0, 5 );
			}
		}
		else
		if(current_wall_level > 0 && hit_points > 0.0f)
		{
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, current_wall_level,
				hit_points, (float) max_hit_points(), 'B' );		// wall back sides

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					NULL, 0, 1 );
			}
		}
		else
		{
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, 0, 1, 1, 'B' );		// wall back sides

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					NULL, 0, 1 );
			}
		}


		//-------- draw plants, buildings and flags --------//

		for( int i=0 ; i<townLayout->slot_count ; i++, townSlot++ )
		{
			//----- build_type==0 if plants -----//

			switch(townSlot->build_type)
			{
				//----- build_type>0 if town buildings -----//

				case TOWN_OBJECT_HOUSE:
				{
					town_res.get_build( slot_object_id_array[i] )
						->draw(town_recno, sectionX1+townSlot->base_x, sectionY1+townSlot->base_y, 
						under_construction ? construction_completion_percent : 100);
					break;
				}

				case TOWN_OBJECT_PLANT:
				{
					plant_res.get_bitmap( slot_object_id_array[i] )
						->draw_at(sectionX1+townSlot->base_x, sectionY1+townSlot->base_y, altitude, 
						under_construction ? construction_completion_percent : 100);
					break;
				}
				//case TOWN_OBJECT_FARM:
				//	draw_farm(sectionX1+townSlot->base_x, sectionY1+townSlot->base_y, townSlot->build_code );
				//	break;

				case TOWN_OBJECT_FLAG:
				{
					if( is_pay_tribute_to_monster() )
					{
						Firm* firmPtr;
						for( int i=0 ; i<linked_firm_count ; i++ )
						{
							firmPtr = firm_array[linked_firm_array[i]];

							if( firmPtr->firm_id == FIRM_LAIR &&
								 firmPtr->nation_recno &&
								 !firmPtr->under_construction &&
								 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
							{
								char flagName[] = "FLAGM001";
								flagName[7] = '0' + (char) ((sys.frame_count+town_recno)%10);
								short* colorRemapTable = game.get_color_remap_table(firmPtr->nation_recno, 0);

								char *flagBitmap = image_spict.get_ptr(flagName);
								world.zoom_matrix->put_bitmap_offset(sectionX1+townSlot->base_x, sectionY1+townSlot->base_y,
									altitude, flagBitmap, -9, -97, colorRemapTable, 0, 1);
							}
						}
					}
					else										
					if( nation_recno )
						draw_flag(sectionX1+townSlot->base_x, sectionY1+townSlot->base_y);
					break;
				}
			}
		}

		//-------- draw front wall ------------//

		if( (hit_points >= max_hit_points())
			&& target_wall_level > current_wall_level )
		{
			int wall_construction_percent = upgrade_wall_progress *100 /MAX_WALL_PROGRESS;

			short* remapTable = (short *)vga.vga_color_table->get_table(	
				MAX_BRIGHTNESS_ADJUST_DEGREE - 
				(wall_construction_percent) * MAX_BRIGHTNESS_ADJUST_DEGREE / 100);
				
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, current_wall_level + 1,
				100, 100, 'F' );		// wall back sides

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					remapTable, 0, 5 );
			}
		}
		else
		if(current_wall_level > 0 && hit_points > 0.0f)
		{
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, current_wall_level,
				hit_points, (float) max_hit_points(), 'F' );		// wall back sides

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					NULL, 0, 1 );
			}
		}
		else
		{
			TownWallBitmapInfo *townWallBitmapInfo = town_wall_res.search( race_id, 0, 1, 1, 'F' );

			if( townWallBitmapInfo )
			{
				world.zoom_matrix->put_bitmap_offset( loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude,
					townWallBitmapInfo->bitmap_ptr, 
					townWallBitmapInfo->offset_x, townWallBitmapInfo->offset_y,
					NULL, 0, 1 );
			}
		}
	}
}
//-------- End of function Town::draw -----------//


//------- Begin of function Town::draw_flag -----------//
//
// Draw the town section on the zoom map
//
void Town::draw_flag(int curX, int curY)
{
	char flagName[] = "FLAGXX01";

	flagName[4] = '0' + (char) race_id /10 ;
	flagName[5] = '0' + (char) race_id %10 ;

	flagName[7] = '0' + (char) ((sys.frame_count+town_recno)%10);

	short* colorRemapTable = game.get_color_remap_table(nation_recno, 0);

//	int drawX = absBaseX - world.view_top_x + ZOOM_X1 - 9;
//	int drawY = absBaseY - world.view_top_y + ZOOM_Y1 - 97;
//	world.zoom_matrix->put_bitmap_remap_clip(drawX, drawY, image_spict.get_ptr(flagName), colorRemapTable, 1);	// 1-the bitmap is compressed
	char *flagBitmap = image_spict.get_ptr(flagName);
	world.zoom_matrix->put_bitmap_offset(curX, curY, altitude, flagBitmap, -9, -97,
		colorRemapTable, 0, 1);
}
//-------- End of function Town::draw_flag -----------//


//------- Begin of function Town::draw_farm -----------//
//
// Draw farming field.
//
void Town::draw_farm(int curX, int curY, int farmId, short *remapTable)
{
	err_when( farmId<1 || farmId>9 );

	char farmName[] = "FARM-1";

	farmName[5] = '0' + farmId;

//	int drawX = absBaseX - world.view_top_x + ZOOM_X1;
//	int drawY = absBaseY - world.view_top_y + ZOOM_Y1;
//	world.zoom_matrix->put_bitmap_clip(drawX, drawY, image_spict.get_ptr(farmName), 1);	// 1-the bitmap is compressed
	char *farmBitmap = image_spict.get_ptr(farmName);

	world.zoom_matrix->put_bitmap(curX, curY, altitude, farmBitmap,
		remapTable, 0, 1);
}
//-------- End of function Town::draw_farm -----------//


//------- Begin of function Town::is_in_zoom_win -----------//
//
// Whether the town section is in the current zoom window.
//
int Town::is_in_zoom_win()
{
	return
		world.zoom_matrix->calc_zoom_y(loc_x1*LOCATE_WIDTH, loc_y1*LOCATE_HEIGHT, altitude) >= 0 &&
		world.zoom_matrix->calc_zoom_x(loc_x1*LOCATE_WIDTH, (loc_y2+1)*LOCATE_HEIGHT-1, altitude) >= 0 &&
		world.zoom_matrix->calc_zoom_x((loc_x2+1)*LOCATE_WIDTH-1, loc_y1*LOCATE_HEIGHT, altitude) <= ZOOM_WIDTH &&
		world.zoom_matrix->calc_zoom_y((loc_x2+1)*LOCATE_WIDTH-1, (loc_y2+1)*LOCATE_HEIGHT-1, altitude) <= ZOOM_HEIGHT;
}
//--------- End of function Town::is_in_zoom_win -----------//


//------- Begin of function Town::draw_selected -----------//
//
// Draw a square around the town section on the map.
//
void Town::draw_selected()
{
	draw_detect_link_line(0);		// 0-the action is draw only, not detecting
}
//--------- End of function Town::draw_selected -----------//

//------- Begin of function Town::draw_detect_link_line ---------//
//
// [int] actionDetect - 0 - this is a draw action
//								1 - this is a detect action
//                      (default: 0)
//
// return: <int> 1 - detected
//					  0 - not detected
//
int Town::draw_detect_link_line(int actionDetect)
{
	int 	i, firmX, firmY, townX, townY;
	Firm* firmPtr;
	Town* townPtr;

	//-------- set source points ----------//

	int srcX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
		(loc_x1+loc_x2+1)*LOCATE_WIDTH/2, (loc_y1+loc_y2+1)*LOCATE_HEIGHT/2, 
		altitude);
	int srcY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
		(loc_x1+loc_x2+1)*LOCATE_WIDTH/2, (loc_y1+loc_y2+1)*LOCATE_HEIGHT/2, 
		altitude);

	//------ draw lines to linked firms ---------//

	char* bitmapPtr;
//	char  goodLinkName[9] = "GOODLINK";

//	goodLinkName[7] = '1'+(char)(sys.frame_count%3);

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		firmX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);
		firmY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
			(firmPtr->loc_x1+firmPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(firmPtr->loc_y1+firmPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			firmPtr->altitude);

		// ##### begin Gilbert 4/3 #########//
		if( !actionDetect )
		{
			//--- if this is a Fryhtan Lair, draw links to submission towns with a think line ---//

			int thickLineFlag=0;

			if( firmPtr->is_own() && firmPtr->active_link_town_recno == town_recno )
				thickLineFlag = 1;

			if( firmPtr->firm_id == FIRM_LAIR && firmPtr->nation_recno &&
				 nation_recno == 0 &&
				 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
			{
				thickLineFlag = 1;
			}

			//-------- draw the line now -------//

			if( thickLineFlag )
			{
				anim_line.thick_line(&vga_back, srcX, srcY, firmX, firmY, linked_firm_enable_array[i]==LINK_EE, -1 );
			}
			else
			{
				anim_line.draw_line(&vga_back, srcX, srcY, firmX, firmY, linked_firm_enable_array[i]==LINK_EE, 0 );
			}
		}
		// ##### end Gilbert 4/3 #########//

		//---------------------------------------//

		if( can_toggle_firm_link(linked_firm_array[i]) )
			bitmapPtr = power.get_link_icon( linked_firm_enable_array[i], nation_recno==firmPtr->nation_recno );
		else if( firmPtr->can_set_active_link(town_recno) )
			bitmapPtr = image_icon.read( firmPtr->active_link_town_recno == town_recno ? (char*)"LINK_ACT" : (char*)"LINK_DEA" );
		else
			continue;

		//------ draw the link icon and detect mouse action -----//
		if( firmPtr->cast_to_FirmCamp() )
			help.set_help( firmX-16, firmY-16, firmX-16+((Bitmap *)bitmapPtr)->get_width(),
				firmY-16+((Bitmap *)bitmapPtr)->get_height(), "TOWNLOCK" );
		else
			help.set_help( firmX-16, firmY-16, firmX-16+((Bitmap *)bitmapPtr)->get_width(),
				firmY-16+((Bitmap *)bitmapPtr)->get_height(), "WORKLOCK" );

		if( actionDetect )
		{
			int rc = world.zoom_matrix->detect_bitmap_clip( firmX-16, firmY-16, bitmapPtr );

			if( rc )
				mouse.reset_click();		// reset queued mouse click for fast single clicking

			//------ left clicking to toggle link -------//

			if( rc==1 && nation_recno==nation_array.player_recno )
			{
				// ####### begin Gilbert 4/3 ########//
				if( firmPtr->can_set_active_link(town_recno) )
				{
					// -------- set active link of fort -------//

					firmPtr->set_active_link( town_recno, COMMAND_PLAYER );
				}
				else
				// ####### end Gilbert 4/3 ########//
				{
					if( linked_firm_enable_array[i] & LINK_ED )
					{
						toggle_firm_link( i+1, 0, COMMAND_PLAYER );
						// ###### begin Gilbert 25/9 #######//
						se_ctrl.immediate_sound("TURN_OFF");
						// ###### end Gilbert 25/9 #######//
					}
					else
					{
						toggle_firm_link( i+1, 1, COMMAND_PLAYER );
						// ###### begin Gilbert 25/9 #######//
						se_ctrl.immediate_sound("TURN_ON");
						// ###### end Gilbert 25/9 #######//
					}

					if( firmPtr->cast_to_FirmCamp() && !remote.is_enable())
					{
						if( nation_recno )
							update_target_loyalty();
						else
							update_target_resistance();

						update_camp_link();
					}
				}
				return 1;
			}
		}
		else
		{
			if( nation_recno == nation_array.player_recno )
				world.zoom_matrix->put_bitmap_clip( firmX-16, firmY-16, bitmapPtr );
		}
	}

	//------ draw lines to linked towns ---------//

	int townRecno;

	for( i=0 ; i<linked_town_count ; i++ )
	{
		townRecno = linked_town_array[i];

		townPtr = town_array[townRecno];

//		townX = ( ZOOM_X1 + (townPtr->loc_x1-world.zoom_matrix->top_x_loc) * ZOOM_LOC_WIDTH
//				  + ZOOM_X1 + (townPtr->loc_x2-world.zoom_matrix->top_x_loc+1) * ZOOM_LOC_WIDTH ) / 2;
//		townY = ( ZOOM_Y1 + (townPtr->loc_y1-world.zoom_matrix->top_y_loc) * ZOOM_LOC_HEIGHT
//				  + ZOOM_Y1 + (townPtr->loc_y2-world.zoom_matrix->top_y_loc+1) * ZOOM_LOC_HEIGHT ) / 2;
		townX = ZOOM_X1 + world.zoom_matrix->calc_zoom_x(
			(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			townPtr->altitude);
		townY = ZOOM_Y1 + world.zoom_matrix->calc_zoom_y(
			(townPtr->loc_x1+townPtr->loc_x2+1)*LOCATE_WIDTH/2,
			(townPtr->loc_y1+townPtr->loc_y2+1)*LOCATE_HEIGHT/2, 
			townPtr->altitude);

		anim_line.draw_line(&vga_back, srcX, srcY, townX, townY, linked_town_enable_array[i]==LINK_EE );

		//------- detect on the migration icon -------//

		if( nation_recno == nation_array.player_recno &&
			 nation_recno == townPtr->nation_recno &&
			 can_migrate(townRecno) )
		{
			char  migrateName[9] = "MIGRATE1";
			int phase = sys.frame_count % 8;
			migrateName[7] = char( '1'+ phase );
				
			bitmapPtr = image_icon.get_ptr(migrateName);
		//	bitmapPtr = image_icon.get_ptr("MIGRATE");

			if( actionDetect )
			{
				// ##### begin Gilbert 3/3 #######//
				int rc;
				if( (rc = world.zoom_matrix->detect_bitmap_clip( townX-23, townY-25, bitmapPtr )) )
				{
					mouse.reset_click();		// reset queued mouse click for fast single clicking

					err_when(town_array[townRecno]->population>MAX_TOWN_POPULATION);

					// calculate number of people to move instead of while(population > 0) migrate_to(...);
					// because in multiplayer mode, population is not decrease at the moment
					int movePop = 1;
					if( rc == 2 )			// right click
					{
						movePop = MIN(population,MAX_WAGON_POPULATION);		// move 10 as if moving wagon
						if( movePop > MAX_TOWN_POPULATION - town_array[townRecno]->population )
							movePop = MAX_TOWN_POPULATION - town_array[townRecno]->population;
					}
					for( ; movePop > 0; --movePop )
						migrate_to(townRecno, COMMAND_PLAYER);
					se_ctrl.immediate_sound("PULL_MAN");
					return 1;
				}
				// ##### end Gilbert 3/3 #######//
				help.set_help( townX-23, townY-25, townX-23+((Bitmap *)bitmapPtr)->get_width(),
					townY-25+((Bitmap *)bitmapPtr)->get_height(), "MIGRATE" );
			}
			else
			{
				world.zoom_matrix->put_bitmap_clip( townX-23, townY-25, bitmapPtr );
			}
		}
	}

	return 0;
}
//------- End of function Town::draw_detect_link_line ---------//


//------- Begin of function Town::can_toggle_firm_link ---------//
//
// Return whether this town can toggle the link to the given
// firm.
// 
int Town::can_toggle_firm_link(int firmRecno)
{
	if( !nation_recno )		// cannot toggle for independent town
		return 0;

	//------------------------------------//

	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_array[i] != firmRecno )
			continue;

		firmPtr = firm_array[linked_firm_array[i]];

		// ####### begin Gilbert 23/11 ######//
//		switch( firmPtr->firm_id )
//		{
//			//-- you can only toggle a link to a camp if the camp is yours --//
//			case FIRM_CAMP:
//				return firmPtr->nation_recno == nation_recno;
//			//--- town to market link is governed by trade treaty and cannot be toggled ---//
//			case FIRM_MARKET:		
//				return 0;			// !nation_array[nation_recno]->get_relation(firmPtr->nation_recno)->trade_treaty;
//			default:
//				return firm_res[firmPtr->firm_id]->is_linkable_to_town;
//		}

		if( firmPtr->cast_to_FirmCamp() || firmPtr->can_set_active_link(town_recno) )
			return 0;							// town cannot can toggle link from a camp
		else if( firmPtr->cast_to_FirmMarket() )
			return 0;						// town to market link is governed by trade treaty and cannot be toggled
		else
			return firm_res[firmPtr->firm_id]->is_linkable_to_town;
		// ####### end Gilbert 23/11 ######//
	}

	return 0;
}
//------- Begin of function Town::can_toggle_firm_link ---------//


//-------- Begin of function Town::auto_set_layout ------//
//
void Town::auto_set_layout()
{
	//---- locate a layout whose population size is closest to this town -----//

	int 		   layoutId;
	TownLayout* townLayout;

	{
		int firstSuitLayout, suitLayoutCount, suitBuildCount;
		suitBuildCount = (population + POPULATION_PER_HOUSE - 1) / POPULATION_PER_HOUSE;
		if( suitBuildCount <= 0 )
			suitBuildCount = 1;

		firstSuitLayout = 0;
		suitLayoutCount = 0;

		for( layoutId=1 ; layoutId<=town_res.town_layout_count ; layoutId++ )	// scan from the least densed layout to the most densed layout
		{
			// find the first segment of layouts suitable for this population

			townLayout = town_res.get_layout(layoutId);

			if( townLayout->build_count == suitBuildCount )
			{
				if( !firstSuitLayout )
					firstSuitLayout = layoutId;
				suitLayoutCount++;
			}
			else
			{
				if( firstSuitLayout )
					break;
			}
		}

		if( suitLayoutCount <= 0)
			layoutId = town_res.town_layout_count;	// if none layout is suitable, use the largest layout
		else
			layoutId = firstSuitLayout + town_recno % suitLayoutCount;
	}

	layout_id = layoutId;

	//------- set plants in the town layout -------//

	townLayout = town_res.get_layout(layoutId);
	TownSlot*  firstTownSlot = town_res.get_slot(townLayout->first_slot_recno);
	memset( slot_object_id_array, 0, sizeof(slot_object_id_array) );

	for( int i=0 ; i<townLayout->slot_count ; i++ )
	{
		switch(firstTownSlot[i].build_type)
		{
		case TOWN_OBJECT_PLANT:
			slot_object_id_array[i] = plant_res.scan(0, 'T', 0);    // 'T'-town only, 1st 0-any zone area, 2nd 0-any terain type, 3rd-age level
			break;

		case TOWN_OBJECT_FARM:
			slot_object_id_array[i] = firstTownSlot[i].build_code;
			err_when( slot_object_id_array[i] < 1 || slot_object_id_array[i] > 9 );
			break;

		case TOWN_OBJECT_HOUSE:
			slot_object_id_array[i] = town_res.scan_build( townLayout->first_slot_recno+i, race_id );
			err_when( town_res.get_build( slot_object_id_array[i] )->race_id != race_id );
			break;
		}
	}

	//----- set the maximum number of people this layout can hold -----//

	max_layout_pop = townLayout->build_count * POPULATION_PER_HOUSE;
}
//---------- End of function Town::auto_set_layout --------//


//--------- Begin of function Town::left_click ---------//
//
// This function is called when the mouse left-click on this town.
//
void Town::left_click()
{
	town_array.selected_recno = town_recno;
}
//--------- End of function Town::left_click ---------//


//--------- Begin of function Town::right_click ---------//
//
// This function is called when the current town is selected
// and the mouse right-click on a locatoin.
//
void Town::right_click(int xLoc, int yLoc)
{
	town_array.selected_recno = town_recno;
}
//--------- End of function Town::right_click ---------//
