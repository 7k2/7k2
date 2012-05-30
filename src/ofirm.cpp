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

//Filename    : OFIRM.CPP
//Description : Object Firm

#include <ovga.h>
#include <odate.h>
#include <omlink.h>
#include <oworld.h>
#include <oworldmt.h>
#include <opower.h>
#include <oconfig.h>
#include <ogame.h>
#include <ounit.h>
#include <onews.h>
#include <osys.h>
#include <ospy.h>
#include <osite.h>
#include <oinfo.h>
#include <onews.h>
#include <onation.h>
#include <ofirm.h>
#include <oraceres.h>
#include <otown.h>
#include <osprite.h>
#include <ofirmres.h>
#include <of_mark.h>
#include <oremote.h>
#include <of_camp.h>
#include <oseres.h>
#include <orebel.h>
#include <ofirmdie.h>
#include <ounitres.h>
#include <oeffect.h>
#include <oseditor.h>
#include <obullet.h>
#include <ot_firm.h>

//--------- Begin of function Firm::Firm --------//
//
// After created a Firm, you must either call Firm::set_world_matrix()
// to set the record no. of the firm in the matrix or set it yourself
//
// NOTE : this function will be called by firm_array.read_file()
//        it CANNOT change any settings in nation_array
//
Firm::Firm()
{
	memset( (char *)this + sizeof(void *), 0, sizeof(Firm) - sizeof(void *) );
}
//----------- End of function Firm::Firm ---------//


//--------- Begin of function Firm::~Firm --------//
//
// Two ways to terminate a Firm :
//
// 1.call Firm::deinit() first and then delete the firm
// 2.delete the firm directly
//
Firm::~Firm()
{
	if( sys.quick_exit_flag() )
		return;

	deinit();
}
//----------- End of function Firm::~Firm --------//


//--------- Begin of function Firm::init --------//
//
// It will initialize vars, and set the world matrix.
// Before calling init(), firm_recno should be set
//
// Note : it will set world matrix regardless the existing location content,
//        so you must ensure that the location is clean by calling
//        world.zoom_matrix->add_firm_test()
//
// <int> xLoc, yLoc  = the location of firm in the world map
// <int> nationRecno = the recno of nation which build this firm
// <int> firmId      = id(type) of the firm
// [char*] buildCode = the build code of the firm, no need to give if the firm just have one build type
// [short] builderRecno = recno of the builder unit
//
void Firm::init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno)
{
	FirmInfo* firmInfo = firm_res[firmId];

	firm_id = firmId;

	if( buildCode )
		firm_build_id = firmInfo->get_build_id(buildCode);
	
	if( !buildCode || !firm_build_id )					// some buildings like human offensive structures do not have different buildings for different nationality, so firm_build_id will be 0 
		firm_build_id = firmInfo->first_build_id;

	//----------- set vars -------------//

	nation_recno   = nationRecno;
	setup_date     = info.game_date;

	//----- set the firm's absolute positions on the map -----//

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	FirmBitmap* firmBitmap;
	
	race_id = firmInfo->get_race_id(buildCode);

	if( !race_id )			// some firms like human offensive structures do not have race_id.
		race_id = nation_array[nation_recno]->race_id;

	err_when( is_human() == (firmId >= FIRM_LAIR && firmId <= FIRM_MAGIC) );		// error when a human attempts to build a monster firm or a mosnter attempts to build a human firm

	loc_x1 = xLoc;
	loc_y1 = yLoc;
	loc_x2 = loc_x1 + firmBuild->loc_width  - 1;
	loc_y2 = loc_y1 + firmBuild->loc_height - 1;

	center_x = (loc_x1 + loc_x2) / 2;
	center_y = (loc_y1 + loc_y2) / 2;

	region_id = world.get_region_id( center_x, center_y );

	// get the height of the top left corner
	// altitude = world.get_corner(xLoc, yLoc)->get_altitude();
	altitude = world.get_corner(loc_x2+1, loc_y2+1)->get_altitude();

	abs_x1 = world.zoom_matrix->calc_abs_x(xLoc * LOCATE_WIDTH,
		yLoc * LOCATE_HEIGHT, altitude) + firmBuild->min_offset_x;
	abs_y1 = world.zoom_matrix->calc_abs_y(xLoc * LOCATE_WIDTH,
		yLoc * LOCATE_HEIGHT, altitude) + firmBuild->min_offset_y;
   abs_x2 = abs_x1 + firmBuild->max_bitmap_width  - 1;
   abs_y2 = abs_y1 + firmBuild->max_bitmap_height - 1;

   //--------- set animation frame vars ---------//

	int bitmapCount;
	int firstBitmap;
	int frameCount = firmBuild->frame_count;
	int aniPartCount = firmBuild->ani_part_count;
	// ##### begin Gilbert 30/10 ######//
	err_when( aniPartCount < 0 );
	err_when( aniPartCount > MAX_FRAME_COUNTERS );
	// firm_cur_frame = (char*) mem_add( sizeof(char) *aniPartCount);
	// firm_remain_frame_delay = (char*) mem_add( sizeof(char) *aniPartCount);
	// ##### end Gilbert 30/10 ######//
	
	// even not animation_full_size will have aniPartCount = 1 (set in firmres.cpp)
	for (int i = 0; i < aniPartCount ; i++)
	{
		firm_cur_frame[i] = 1;
		
		if( firmBuild->animate_full_size )
			// animate_full_size only supports animation of looping
			// it does not support random animation and in fact
			// it is now only used for no animation buildings
			firm_remain_frame_delay[i] = (char) firmBuild->frame_delay(1);
		else
		{
			// not animate_full_size supports animation of looping and
			// random animation and partial animation
			// unlike the animate_full_size animation, it search starting
			// from the 2nd frame for bitmaps (since now 1 frame may has more 
			// than 1 bitmap) with same animation part no. of the currently
			// searching part no., i. Then assign this animation part's delay and 
			// set its current_frame to the first found frame
			for ( int k=1; k <frameCount; k++ )
			{
				firstBitmap = firmBuild->first_bitmap(k+1);
				bitmapCount = firmBuild->bitmap_count(k+1);

				int j;
				for ( j=0; j <bitmapCount; j++ )
				{
					firmBitmap = firm_res.get_bitmap(firstBitmap + j);
					if( firmBitmap )
					{
						if (firmBitmap->ani_part == (i + 1))
						{
							firm_remain_frame_delay[i] = firmBitmap->delay;
							firm_cur_frame[i] = k + 1;
							break;
						}
					}
				}
				if (j < bitmapCount)
					break;
			}
		}
	}
		 
   //--------- initialize gaming vars ----------//

   hit_points     = (float) 0;
   place_max_hit_points = firmInfo->max_hit_points;

	// #### begin Gilbert 6/11 ######//
	defense_attribute = firmBuild->defense_attribute;
	// #### end Gilbert 6/11 ######//

	//------ set construction and builder -------//

	under_construction = firmInfo->buildable;       // whether the firm is under construction, if the firm is not buildable it is completed in the first place

	if( !under_construction )			// if this firm doesn't been to be constructed, set its hit points to the maximum
		hit_points = place_max_hit_points;

	if( builderRecno )
		set_builder(builderRecno);
	else
		builder_recno = 0;

	//------ update firm counter -------//

   firmInfo->total_firm_count++;
	
	if( nation_recno )
		firmInfo->inc_nation_firm_count(nation_recno);

   //-------------------------------------------//

   if( nation_recno > 0 )
   {
		Nation* nationPtr = nation_array[nation_recno];

      is_ai = nationPtr->is_ai();
		ai_processed = 1;

      //--------- increase firm counter -----------//

      nationPtr->nation_firm_count++;

      //-------- update last build date ------------//

      nationPtr->last_build_firm_date = info.game_date;
   }
   else
   {
      is_ai = 0;
      ai_processed = 0;
   }

   ai_status = FIRM_WITHOUT_ACTION;
   ai_link_checked = 1;       // check the connected firms if ai_link_checked = 0;

   //--------------------------------------------//

	active_link_town_recno = 0;

	setup_link();

	set_world_matrix();

	init_name();

	//----------- init AI -----------//

	if( nation_recno )
		nation_array[nation_recno]->add_firm_info(firm_id, firm_recno);

	//-------- init derived ---------//

	init_derived();         // init_derived() before set_world_matrix() so that init_derived has access to the original land info.

	
	//-------- dynamic loading of firm bitmaps ---------//
	firmBuild->load_bitmap_res();

}
//----------- End of function Firm::init ---------//


//--------- Begin of function Firm::deinit --------//
//
void Firm::deinit()
{
	if( !firm_recno )    // already deleted
		return;

	if( power.command_firm_recno == firm_recno )
		power.reset_command();

	// ##### begin Gilbert 30/10 ######//
//	mem_del(firm_cur_frame);
//	firm_cur_frame = NULL;
//	mem_del(firm_remain_frame_delay);
//	firm_remain_frame_delay = NULL;
	// ##### end Gilbert 30/10 ######//
	
	is_being_deleted = 1;	// whether the place is currently being deleted, if it is set to true, some functions will be executed in an exceptional way, it is set in deinit()

	Place::deinit();			// parent class deinit()

	deinit_derived();

	//------- delete AI info ----------//

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		nationPtr->del_firm_info(firm_id, firm_recno);

		if(is_ai)
		{
			if( should_close_flag )
				nationPtr->firm_should_close_array[firm_id-1]--;

			err_when( nationPtr->firm_should_close_array[firm_id-1] < 0 );

		}
	}

	//--------- clean up related stuff -----------//

	restore_world_matrix();
	release_link();

	//-------- dynamic unloading of firm bitmaps ---------//

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	firmBuild->free_bitmap_res();

	//------ all workers and the overseer resign ------//

	if( !sys.signal_exit_flag )
	{
	//	char* name1 = "Wilde Lishorr";
	//	char* name2 = "Lishorr";
	//	if( !under_construction && hit_points < 1 && strcmp(name1, firm_name()) && strcmp(name2, firm_name()) )
		if( !under_construction && hit_points < 1 )
		{
			// -------- create a firm die record ------//
			// can be called as soon as restore_world_matrix

			static int effectId = sprite_res.search_sprite( "FIRE_EFF" );
			err_when( !effectId );
			Effect::create(effectId, loc_x1 *LOCATE_WIDTH, loc_y1 *LOCATE_HEIGHT, SPRITE_IDLE, 1, 8, 0);
					
			FirmDie firmDie3;
			if (firmBuild->loc_width == 3 && firmBuild->loc_height == 3)
				firmDie3.init(this, 6);
			else
			if (firmBuild->loc_width == 6 && firmBuild->loc_height == 6)
				firmDie3.init(this, 5);
			else
				firmDie3.init(this, 4);
			firm_die_array.add(&firmDie3);
		}
	}

	free_all_people();
	//--------- decrease firm counter -----------//

	if( nation_recno )
		nation_array[nation_recno]->nation_firm_count--;

	//------ update firm counter -------//

	FirmInfo* firmInfo = firm_res[firm_id];

	firmInfo->total_firm_count--;

	if( nation_recno )
		firmInfo->dec_nation_firm_count(nation_recno);

	//------- update town border ---------//

	loc_x1 = -1;      // mark deleted

	//------- if the current firm is the selected -----//

	if( firm_array.selected_recno == firm_recno )
	{
		firm_array.selected_recno = 0;
      info.disp();
   }

   //-------------------------------------------------//

   firm_recno = 0;

	is_being_deleted = 0;


	//-------- dynamic unloading of firm bitmaps ---------//

//	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	
	if( firm_id )
	{
//		firmBuild->free_bitmap_res();
		firm_id = 0;
	}
}
//----------- End of function Firm::deinit ---------//


//--------- Begin of function Firm::init_name --------//
//
// Set the name of this firm. Name related vars are set.
//
void Firm::init_name()
{
	char t=firm_res[firm_id]->short_name[0];

	if( t==' ' || !t )		// if this firm does not have any short name, display the full name without displaying the town name together
		return;

	//---- find the closest town and set closest_town_name_id -----//

	closest_town_name_id = get_closest_town_name_id();

	//--------- set firm_name_instance_id -----------//

	char  usedInstanceArray[256];
	Firm* firmPtr;

	memset( usedInstanceArray, 0, sizeof(usedInstanceArray) );

	int i;
	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( firmPtr->firm_id == firm_id &&
			 firmPtr->closest_town_name_id == closest_town_name_id &&
			 firmPtr->firm_name_instance_id )
		{
			usedInstanceArray[firmPtr->firm_name_instance_id-1] = 1;
		}
	}

	for( i=0 ; i<256 ; i++ )		// get the smallest id. which are not used by existing firms
	{
		if( !usedInstanceArray[i] )
		{
			firm_name_instance_id = i+1;
			break;
		}
	}
}
//--------- End of function Firm::init_name --------//


//------- Begin of function Firm::get_closest_town_name_id -----------//
//
// return the name id. of the closest town.
//
int Firm::get_closest_town_name_id()
{
	//---- find the closest town and set closest_town_name_id -----//

	int 	townDistance, minTownDistance=0x7FFF;
	int   closestTownNameId=0;
	Town* townPtr;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		townDistance = m.points_distance( townPtr->center_x, townPtr->center_y,
							center_x, center_y );

		if( townDistance < minTownDistance )
		{
			minTownDistance = townDistance;
			closestTownNameId = townPtr->town_name_id;
		}
	}

	return closestTownNameId;
}
//--------- End of function Firm::get_closest_town_name_id -----------//

//------- Begin of function Firm::firm_name -----------//
//
char* Firm::firm_name()
{
	static String str;

	// ##### begin Gilbert 4/9 ###### //
	if( !closest_town_name_id )
	{
		str = firm_res.get_build(firm_build_id)->name;
	}
	else
	{
		str = text_firm.str_firm_name( firm_build_id, town_res.get_name(closest_town_name_id), firm_name_instance_id );
	}
	// ##### end Gilbert 4/9 ###### //

	return str;
}
//--------- End of function Firm::firm_name -----------//

//--------- Begin of function Firm::set_world_matrix --------//
//
// Set the cargo id of current firm int he world matrix
//
void Firm::set_world_matrix()
{
	//--- if a nation set up a firm in a location that the player has explored, contact between the nation and the player is established ---//

	int xLoc, yLoc;

	// ##### begin Gilbert 5/11 #######//
	
	// ------ clear obstacle -------//

	for( yLoc=MAX(loc_y1-INTER_PLACE_SPACE, 0); yLoc<=loc_y2+INTER_PLACE_SPACE && yLoc<world.max_y_loc; yLoc ++ )
	{
		for( xLoc=MAX(loc_x1-INTER_PLACE_SPACE, 0); xLoc<=loc_x2+INTER_PLACE_SPACE && xLoc<world.max_x_loc; xLoc ++ )
		{
			world.get_loc(xLoc, yLoc)->clear_obstacle();
		}
	}

	// ------- set matrix ------- //
	// ##### end Gilbert 5/11 #######//

	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
		{
			world.get_loc(xLoc, yLoc)->set_firm(firm_recno);
		}
	}

	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	establish_contact_with_player();

	//------------ reveal new land ----------//

	// ######### begin Gilbert 1/4 #######//
	if( explore_for_player() )
	// ######### end Gilbert 1/4 #######//
	{
		world.unveil( loc_x1, loc_y1, loc_x2, loc_y2 );
		world.visit( loc_x1, loc_y1, loc_x2, loc_y2, EXPLORE_RANGE-1 );
	}

	//-------- set should_set_power --------//

	should_set_power = get_should_set_power();

	//---- set this town's influence on the map ----//

	if( should_set_power )
		world.set_power(loc_x1, loc_y1, loc_x2, loc_y2, nation_recno);

	//---- if the newly built firm is visual in the zoom window, redraw the zoom buffer ----//

	if( is_in_zoom_win() )
		sys.zoom_need_redraw = 1;  // set the flag on so it will be redrawn in the next frame
}
//----------- End of function Firm::set_world_matrix --------//


//--------- Begin of function Firm::get_should_set_power --------//
//
int Firm::get_should_set_power()
{
	int shouldSetPower = 1;

	if( firm_id == FIRM_MARKET )
	{
		//--- don't set power for a market if it's linked to another nation's town ---//

		Town *townPtr;

		shouldSetPower = 0;

		//--- only set the shouldSetPower to 1 if the market is linked to a firm of ours ---//

		for( int i=0 ; i<linked_town_count ; i++ )
		{
			townPtr = town_array[ linked_town_array[i] ];

			if( townPtr->nation_recno == nation_recno )
			{
				shouldSetPower = 1;
				break;
			}
		}
	}

	return shouldSetPower;
}
//----------- End of function Firm::get_should_set_power --------//


//------- Begin of function Firm::establish_contact_with_player --------//
//
// See if the town's location is an explored area, establish contact
// with the player.
//
void Firm::establish_contact_with_player()
{
	if( !nation_recno )
		return;

	int xLoc, yLoc;
	Location* locPtr;

	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
		{
			locPtr = world.get_loc(xLoc, yLoc);

			locPtr->set_firm(firm_recno);

			if( locPtr->explored() && nation_array.player_recno )
			{
				NationRelation *relation = (~nation_array)->get_relation(nation_recno);

				if( !remote.is_enable() )
				{
					relation->has_contact = 1;
				}
				else
				{
					if( !relation->has_contact && !relation->contact_msg_flag )
					{
						// packet structure : <player nation> <explored nation>
						short *shortPtr = (short *)remote.new_send_queue_msg(MSG_NATION_CONTACT, 2*sizeof(short));
						*shortPtr = nation_array.player_recno;
						shortPtr[1] = nation_recno;
						relation->contact_msg_flag = 1;
					}
				}
			}
		}
	}
}
//-------- End of function Firm::establish_contact_with_player --------//


//--------- Begin of function Firm::restore_world_matrix --------//
//
// When the firm is destroyed, restore the original land id
//
void Firm::restore_world_matrix()
{
   int xLoc, yLoc;

   for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
   {
      for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
      {
         err_when( world.get_loc(xLoc,yLoc)->firm_recno() != firm_recno );

			world.get_loc(xLoc,yLoc)->remove_firm();
      }
	}

	//---- restore this town's influence on the map ----//

	if( should_set_power ) 			// no power region for harbor as it build on coast which cannot be set with power region
		world.restore_power(loc_x1, loc_y1, loc_x2, loc_y2, 0, firm_recno);

   //---- if the newly built firm is visual in the zoom window, redraw the zoom buffer ----//

   if( is_in_zoom_win() )
      sys.zoom_need_redraw = 1;
}
//----------- End of function Firm::restore_world_matrix --------//


//---------- Begin of function Firm::process_animation --------//
//
void Firm::process_animation()
{
   //-------- process animation ----------//

   FirmBuild* firmBuild  = firm_res.get_build(firm_build_id);
	FirmBitmap* firmBitmap;
   int frameCount = firmBuild->frame_count;
	int firstBitmap;
	int bitmapCount;

	if( hit_points < (max_hit_points() * 0.1))
	{
		if (m.random(100) > 65)
		{
			int effectId = sprite_res.search_sprite( "FIRMDIE" );
			if( effectId )
			{
				Effect::create(effectId, 
					(loc_x1 + m.random(20)%(loc_x2 -loc_x1 +1)) *LOCATE_WIDTH,
					(loc_y1 + m.random(20)%(loc_y2 -loc_y1 +1)) *LOCATE_HEIGHT,
					SPRITE_DIE, 1, 2, 0); 
			}
		}
	}
	else
	if( hit_points < (max_hit_points() * 0.3))
	{
		if (m.random(100) > 80)
		{	
			int effectId = sprite_res.search_sprite( "FIRMDIE" );
			if( effectId )
			{
				Effect::create(effectId, 
					(loc_x1 + m.random(20)%(loc_x2 -loc_x1 +1)) *LOCATE_WIDTH,
					(loc_y1 + m.random(20)%(loc_y2 -loc_y1 +1)) *LOCATE_HEIGHT,
					SPRITE_DIE, 1, 2, 0);
			}
		}
	}

   if( frameCount==1 )     // no animation for this firm
      return;

   //---------- next frame -----------//
	int aniPartCount = firmBuild->ani_part_count;
	for (int i = 0; i < aniPartCount ; i++)
	{
		firm_remain_frame_delay[i] --;
		if (firm_remain_frame_delay[i] < 0)
		{
			// animate_full_size only supports animation of looping
			// it does not support random animation and in fact
			// it is now only used for no animation buildings
			// but it support different frame different delay

			if( firmBuild->animate_full_size )
			{
				if( ++firm_cur_frame[i] > frameCount )
					firm_cur_frame[i] = 1;
				firm_remain_frame_delay[i] = (char) firmBuild->frame_delay(firm_cur_frame[i]);
			}
			else
			{
			// not animate_full_size supports animation of looping and
			// random animation and partial animation
			// however different frames of each parts must has same delay
		
				for ( int k=1; k <frameCount; k++ )
				{
					firstBitmap = firmBuild->first_bitmap(k+1);
					bitmapCount = firmBuild->bitmap_count(k+1);

					int j;
					for ( j=0; j <bitmapCount; j++ )
					{
						firmBitmap = firm_res.get_bitmap(firstBitmap + j);
						if( firmBitmap )
						{
							if (firmBitmap->ani_part == (i + 1))
							{
								// unlike the animate_full_size animation, it search starting
								// from the 2nd frame for bitmaps (since now 1 frame may has more 
								// than 1 bitmap) with same animation part no. of the currently
								// searching part no., i. This has no relationship with the 
								// current_frame. Then restore this animation part's delay and 
								// set its current_frame either randomly or sequentially
			
								firm_remain_frame_delay[i] = firmBitmap->delay;
								if (firmBitmap->random_flag)
									firm_cur_frame[i] = m.random(100) % frameCount +1;
								else
								if( ++firm_cur_frame[i] > frameCount )
									firm_cur_frame[i] = 1;
								
								if((firm_cur_frame[i] == 1) && !( firmBuild->use_first_frame ))
									firm_cur_frame[i] = 2;
								break;
							}
						}
					}
					if (j < bitmapCount)
						break;
				}
			}
		}
	}
}

//---------- End of function Firm::process_animation --------//

//---------- Begin of function Firm::next_day --------//
//
void Firm::next_day()
{
	if( !nation_recno )
		return;

#ifdef DEBUG			//BUGHERE

	err_when( is_human() == (firm_id >= FIRM_LAIR && firm_id <= FIRM_MAGIC) );		// error when a human attempts to build a monster firm or a mosnter attempts to build a human firm

	//------ validate linked towns ------//

	int lastLinkedTown=0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		Town* townPtr = town_array[linked_town_array[i]];

		err_when( townPtr->town_recno == lastLinkedTown );

		lastLinkedTown = townPtr->town_recno;

		int j;
		for( j=0; j<townPtr->linked_firm_count ; j++ )
		{
			if( townPtr->linked_firm_array[j] == firm_recno )
				break;
		}

		err_when( j==townPtr->linked_firm_count );		// the link is brokwn, only one side has it
	}

#endif

	//------ think about updating link status -------//
	//
	// This part must be done here instead of in
	// process_ai() because it will be too late to do
	// it in process_ai() as the next_day() will call
	// first and some wrong goods may be input to markets.
	//
	//-----------------------------------------------//

	if( is_ai )
	{
		if( info.game_date%30==firm_recno%30 || !ai_link_checked )	// once 30 days or when the link has been changed.
		{
			ai_update_link_status();
			ai_link_checked = 1;
		}
	}

	//-------- pay expenses ----------//

	pay_expense();

	//--------- repairing ----------//

	process_repair();

	//------ catching spies -------//

	if( info.game_date%30 == firm_recno%30 )
		spy_array.catch_spy(SPY_FIRM, firm_recno);

	//--- recheck no_neighbor_space after a period, there may be new space available now ---//

	if( no_neighbor_space && info.game_date%180 == firm_recno%180 )
	{
		short buildXLoc, buildYLoc;

		if( nation_array[nation_recno]->find_best_place_loc(FIRM_INN, loc_x1, loc_y1, buildXLoc, buildYLoc) )		// whether it's FIRM_INN or not really doesn't matter, just any firm type will do
			no_neighbor_space = 0;
	}

	// ##### begin Gilbert 16/11 #######//
	// ------ reset active_link_town_recno if deleted --------//

	if( town_array.is_deleted(active_link_town_recno) )
	{
		active_link_town_recno = 0;
	}
	// ##### end Gilbert 16/11 #######//

	// ####### begin Gilbert 4/3 ########//

	// ------ clear rally destination if deleted -------//

	if( rally_enable_flag && rally_dest_base_obj_recno
		&& base_obj_array.is_deleted(rally_dest_base_obj_recno) )
	{
		clear_rally_point(COMMAND_AUTO);
	}

	// ####### end Gilbert 4/3 ########//

	if( hit_points < (max_hit_points() * 0.1))
	{
		if (m.random(100) > 20)
		{
			FirmDie firmDieFire1;
			firmDieFire1.init(this, 3, m.random(100));
			firm_die_array.add(&firmDieFire1);
		}
		if (m.random(100) > 40)
		{
			FirmDie firmDieFire2;
			firmDieFire2.init(this, 3, m.random(100));
			firm_die_array.add(&firmDieFire2);
		}
	//	if (m.random(100) > 30)
	//		bullet_array.add_bullet(this, this);
	}
	else
	if( hit_points < (max_hit_points() * 0.3))
	{
		if (m.random(100) > 30)
		{
			FirmDie firmDieFire3;
			firmDieFire3.init(this, 3, m.random(100));
			firm_die_array.add(&firmDieFire3);
		}
	//	if (m.random(100) > 20)
	//		bullet_array.add_bullet(this, this);
	}
}
//----------- End of function Firm::next_day ---------//


//---------- Begin of function Firm::next_month --------//
//
void Firm::next_month()
{
	//------ update nation power recno ------//

	int newShouldSetPower = get_should_set_power();

	if( newShouldSetPower == should_set_power )
		return;

	if( should_set_power )
		world.restore_power(loc_x1, loc_y1, loc_x2, loc_y2, 0, firm_recno);

	should_set_power = newShouldSetPower;

	if( should_set_power )
		world.set_power(loc_x1, loc_y1, loc_x2, loc_y2, nation_recno);
}
//----------- End of function Firm::next_month ---------//


//---------- Begin of function Firm::next_year --------//
//
void Firm::next_year()
{
	//------- post income data --------//

	last_year_income = cur_year_income;
	cur_year_income  = (float) 0;
}
//----------- End of function Firm::next_year ---------//

//---------- Begin of function Firm::process_repair --------//
//
void Firm::process_repair()
{
	if( repair_flag && hit_points < max_hit_points()
		&& info.game_date > last_attack_date+1	)			// can only do construction when the firm is not under attack //
		
	{
		if( nation_recno )
		{
			Nation* nationPtr = nation_array[nation_recno];

			float repairCost = (float) firm_res[firm_id]->setup_cost
									 * REPAIR_POINTS_PER_DAY / max_hit_points();

			float repairLiveCost = (float) firm_res[firm_id]->setup_live_points_cost
										  * REPAIR_POINTS_PER_DAY / max_hit_points();

			if( nationPtr->cash < repairCost || nationPtr->live_points < repairLiveCost )
				return;

			nationPtr->add_expense( EXPENSE_CONSTRUCTION, repairCost, 1 );
			nationPtr->change_live_points( -repairLiveCost );
		}

		hit_points += REPAIR_POINTS_PER_DAY;

		if( hit_points >= max_hit_points() )
		{
			hit_points = (float) max_hit_points();
			set_repair_flag( 0, COMMAND_AUTO );
		}
	}
}
//----------- End of function Firm::process_repair ---------//


//---------- Begin of function Firm::add_income --------//
//
void Firm::add_income(int incomeType, float incomeAmt)
{
	cur_year_income += incomeAmt;

	nation_array[nation_recno]->add_income(incomeType, incomeAmt, 1);
}
//----------- End of function Firm::add_income ---------//


//--------- Begin of function Firm::year_expense ---------//
//
// Return the yearly expense for this firm.
//
int Firm::year_expense()
{
	return firm_res[firm_id]->year_cost;
}
//----------- End of function Firm::year_expense -----------//


//---------- Begin of function Firm::pay_expense --------//
//
void Firm::pay_expense()
{
	if( !nation_recno )
		return;

	Nation* nationPtr = nation_array[nation_recno];

	//-------- fixed expenses ---------//

	float dayExpense = (float) year_expense() / 365;

	if( nationPtr->cash >= dayExpense )
	{
		nationPtr->add_expense( EXPENSE_FIRM, dayExpense, 1 );
	}
	else
	{
		if( hit_points > 0 )
			hit_points--;

		if( hit_points < 0 )
			hit_points = (float) 0;

		//--- when the hit points drop to zero and the firm is destroyed ---//

		if( hit_points==0 && nation_recno == nation_array.player_recno )
			news_array.firm_worn_out(firm_recno);
	}
}
//----------- End of function Firm::pay_expense ---------//



//--------- Begin of function Firm::sell_firm ---------//

void Firm::sell_firm(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_SELL, sizeof(short));
		*shortPtr = firm_recno;
		return;
	}
	//------- sell at 50% of the original cost -------//

	Nation* nationPtr = nation_array[nation_recno];

	float sellIncome = (float) firm_res[firm_id]->setup_cost / 2 * (int) hit_points / (int) max_hit_points();
	float sellLiveIncome = (float) firm_res[firm_id]->setup_live_points_cost / 2 * (int) hit_points / (int) max_hit_points();

	nationPtr->add_income(INCOME_SELL_FIRM, sellIncome);

	if( nationPtr->is_monster() )
		nationPtr->change_live_points( sellLiveIncome );

	se_res.sound(center_x, center_y, 1, 'F', firm_id, "SELL" );

   firm_array.del_firm(firm_recno);
}
//----------- End of function Firm::sell_firm -----------//


//--------- Begin of function Firm::destruct_firm ---------//

void Firm::destruct_firm(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_DESTRUCT, sizeof(short));
		*shortPtr = firm_recno;
		return;
	}

	se_res.sound(center_x, center_y, 1, 'F', firm_id, "DEST" );

	firm_array.del_firm(firm_recno);
}
//----------- End of function Firm::destruct_firm -----------//


//---------- Begin of function Firm::process_construction --------//
//
void Firm::process_construction()
{
	if( !under_construction )
		return;

	//--- can only do construction when the firm is not under attack ---//

	if( info.game_date <= last_attack_date+1 )
		return;

	if( sys.frame_count%2!=0 )		// one build every 2 frames
		return;

	//------ increase the construction progress ------//

	Unit *unitPtr = NULL;		// builder may be missing if built by scenario editor
	if( builder_recno )
	{
		unitPtr = unit_array[builder_recno];

		hit_points+=2;

		if( config.fast_build && nation_recno==nation_array.player_recno )
			hit_points += 10;
	}

	//------- when the construction is complete ----------//

	if( hit_points >= max_hit_points() )     // finished construction
	{
		hit_points = (float) max_hit_points();

		under_construction = 0;
		setup_date = info.game_date;

		// ##### begin Gilbert 15/10 ######//
		if( unitPtr && nation_recno == nation_array.player_recno )
			se_res.far_sound(center_x, center_y, 1, 'S', unitPtr->sprite_id,
				"FINS", 'F',  firm_id);
		// ##### end Gilbert 15/10 ######//

		err_when(builder_recno<=0 || unit_array.is_deleted(builder_recno));
		err_when(unitPtr->nation_recno!=nation_recno);

		//---------------------------------------//

		// ##### begin Gilbert 15/10 ######//
		if( builder_recno && builder_stay_after_construction() )	// attempt to assign the unit into the firm. return 1 if assigned successfully
		// ##### end Gilbert 15/10 ######//
			builder_recno = 0;
		else
			set_builder(0);		// if cannot assign it to the firm, mobilize it.
	}

	err_when (hit_points < 0 || hit_points > max_hit_points() );
}
//---------- End of function Firm::process_construction --------//


//------- Begin of function Firm::complete_construction -----------//
//
// Complete construction instantly.
//
void Firm::complete_construction()
{
	if( under_construction )
	{
		hit_points = (float) max_hit_points();
		under_construction = 0;

		setup_date = info.game_date;
	}
}
//--------- End of function Firm::complete_construction -----------//


//--------- Begin of function Firm::cancel_construction ---------//
//
// Cancel construction
//
void Firm::cancel_construction(char remoteAction)
{
	if( !remoteAction && remote.is_enable())
	{
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_CANCEL, sizeof(short));
		shortPtr[0] = firm_recno;
		return;
	}
	//------ get half of the construction cost back -------//

	Nation* nationPtr = nation_array[nation_recno];

	nationPtr->add_expense( EXPENSE_CONSTRUCTION, (float) -firm_res[firm_id]->setup_cost/2 );

	if( nationPtr->is_monster() )
		nationPtr->change_live_points( (float) firm_res[firm_id]->setup_live_points_cost/2 );

   firm_array.del_firm(firm_recno);
}
//----------- End of function Firm::cancel_construction -----------//

//------- Begin of function Firm::start_upgrade -----------//
//
// <int> upgradeFirmId - id. of the firm type to be upgraded to.
//
void Firm::start_upgrade(int upgradeFirmId, char remoteAction)
{
// 	err_when( upgrading_firm_id );		// if the upgrade process has already started
	if( upgrading_firm_id )		// if the upgrade process has already started
		return;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_FIRM_UPGRADE, 2*sizeof(short));
		// packet structure : <firm recno> <upgradeFirmId>
		shortPtr[0] = firm_recno;
		shortPtr[1] = upgradeFirmId;
		return;
	}

	upgrading_firm_id = upgradeFirmId;

	Nation* nationPtr = nation_array[nation_recno];

	nationPtr->add_expense( EXPENSE_CONSTRUCTION,
		(float)firm_res[upgradeFirmId]->setup_cost );				// setup cost of the firm

	nationPtr->change_live_points( (float)-firm_res[upgradeFirmId]->setup_live_points_cost );
}
//--------- End of function Firm::start_upgrade -----------//

//------- Begin of function Firm::process_upgrade -----------//
//
// return <int> 1 - the upgrade is complete
//				    0 - the upgrade is not yet complete 
//
int Firm::process_upgrade()
{
	if( !upgrading_firm_id )
		return 0;

	//--- can only process upgrade when the firm is not under attack ---//

	if( info.game_date <= last_attack_date+1 )
		return 0;

	if( sys.frame_count%2!=0 )		// one build every 8 frames
		return 0;

	//------ increase the upgrade progress ------//

	upgrade_completion_percent++;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		upgrade_completion_percent += 10;

	//------- when the upgrade is complete ----------//

	if( upgrade_completion_percent >= 100 )
	{
		firm_array.upgrade_firm( firm_recno, upgrading_firm_id );
		return 1;
	}
	else
		return 0;
}
//--------- End of function Firm::process_upgrade -----------//



//-------- Begin of function Firm::should_show_info ------//
//
// Whether information of this firm should be shown.
//
int Firm::should_show_info()
{
	if( config.show_ai_info || nation_recno==nation_array.player_recno
		|| reveal_info
		// ###### begin gilbert 9/3 ######//
		|| scenario_editor.is_edit_mode() )
		// ###### end gilbert 9/3 ######//
		return 1;

	//------ if the builder is a spy of the player ------//

	if( builder_recno )
	{
		if( unit_array[builder_recno]->true_nation_recno() == nation_array.player_recno )
			return 1;
	}

	//---- if there is a phoenix of the player over this firm ----//

	// ##### patch begin Gilbert 16/2 ######//
//	if( nation_array.player_recno && (~nation_array)->revealed_by_phoenix(loc_x1, loc_y1) )
//		return 1;
	if( nation_array.player_recno && (~nation_array)->revealed_by_phoenix(center_x, center_y) )
		return 1;
	// ##### patch end Gilbert 16/2 ######//

	return 0;
}
//---------- End of function Firm::should_show_info --------//

//------- Begin of function Firm::change_nation ---------//
//
void Firm::change_nation(int newNationRecno)
{
	if( nation_recno == newNationRecno )
      return;

	//---------- stop all attack actions to this firm ----------//

	unit_array.stop_attack_obj(base_obj_recno);
	rebel_array.stop_attack_firm(firm_recno);

	Nation *oldNationPtr = NULL;
	Nation *newNationPtr = NULL;

	if( nation_recno )
		oldNationPtr = nation_array[nation_recno];

	if( newNationRecno )
		newNationPtr = nation_array[newNationRecno];

	//------ if there is a builder in this firm, change its nation also ----//

	if( builder_recno )
	{
		Unit* unitPtr = unit_array[builder_recno];

		unitPtr->change_nation(newNationRecno);

		//--- if this is a spy, chance its cloak ----//

		if( unitPtr->spy_recno )
			spy_array[unitPtr->spy_recno]->cloaked_nation_recno = newNationRecno;
	}

	//---------- stop all actions attacking this firm --------//

	unit_array.stop_attack_obj(base_obj_recno);

	//---- update nation_unit_count_array[] ----//

	FirmInfo* firmInfo = firm_res[firm_id];

	if( nation_recno )
		firmInfo->dec_nation_firm_count(nation_recno);

	if( newNationRecno )
		firmInfo->inc_nation_firm_count(newNationRecno);

	//---- reset should_close_flag -----//

	if( is_ai )
	{
		if( should_close_flag )
		{
			if( oldNationPtr )
			{
				oldNationPtr->firm_should_close_array[firm_id-1]--;
				err_when( oldNationPtr->firm_should_close_array[firm_id-1] < 0 );
			}

			should_close_flag = 0;
		}
	}

	//------- update player_spy_count -------//

	spy_array.update_firm_spy_count(firm_recno);

	//--- update the cloaked_nation_recno of all spies in the firm ---//

	spy_array.change_cloaked_nation(SPY_FIRM, firm_recno, nation_recno, newNationRecno);		// check the cloaked nation recno of all spies in the firm

	//-----------------------------------------//

	if( oldNationPtr )
		oldNationPtr->del_firm_info(firm_id, firm_recno);

	//------ update power nation recno ----------//

	if( should_set_power )
		world.restore_power(loc_x1, loc_y1, loc_x2, loc_y2, 0, firm_recno);

	should_set_power = get_should_set_power();

	if( should_set_power )
		world.set_power(loc_x1, loc_y1, loc_x2, loc_y2, newNationRecno);        // set power of the new nation

	//------------ update link --------------//

	release_link();		// need to update link because firms are only linked to firms of the same nation

	nation_recno = newNationRecno;

	setup_link();

	//---------------------------------------//

	if (!newNationRecno)
		is_ai = 1;
	else
	{
		is_ai = nation_array[nation_recno]->is_ai();
		newNationPtr->add_firm_info(firm_id, firm_recno);
	}

	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	establish_contact_with_player();

	//---- reset the action mode of all spies in this town ----//

	spy_array.set_action_mode( SPY_FIRM, firm_recno, SPY_IDLE );      // we need to reset it. e.g. when we have captured an enemy town, SPY_SOW_DISSENT action must be reset to SPY_IDLE

	//-- refresh display if this firm is currently selected --//

	if( firm_array.selected_recno == firm_recno )
		info.disp();

	// ####### begin Gilbert 5/7 ########//
	if( explore_for_player() )
	{
		world.unveil( loc_x1, loc_y1, loc_x2, loc_y2 );
		world.visit( loc_x1, loc_y1, loc_x2, loc_y2, EXPLORE_RANGE-1 );
	}
	// ####### end Gilbert 5/7 ########//
}
//-------- End of function Firm::change_nation ---------//


//-------- Begin of function Firm::can_assign_capture ---------//
//
// return if this firm can be captured,
// none of the building except CAMP and FORT can be captured
//
int Firm::can_assign_capture()
{
	return 0;
}
//-------- End of function Firm::can_assign_capture ---------//


//--------- Begin of function Firm::kill_builder ---------//

void Firm::kill_builder(short builderRecno)
{
	// ####### begin Gilbert 24/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(unit_array[builderRecno]->unique_id);		// check unit killed
	// ####### end Gilbert 24/2 ########//

	unit_array.del(builderRecno);
}
//----------- End of function Firm::kill_builder -----------//


// ------- begin of function Firm::set_repair_flag -------//
//
void Firm::set_repair_flag(char repairFlag, char remoteAction)
{
	if( firm_id == FIRM_LISHORR )		// Lishorr cannot be repaired.
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// ###### begin Gilbert 7/10 #######//
		// packet structure : <firm recno> <new setting>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_SET_REPAIR, 2*sizeof(short));
		*shortPtr = firm_recno;
		shortPtr[1] = repairFlag;
		// ###### end Gilbert 7/10 #######//
		return;
	}

	repair_flag = repairFlag;
}
// ------- end of function Firm::set_repair_flag -------//


// ------- begin of function Firm::set_rally_point -------//
//
void Firm::set_rally_point(int destBaseObjRecno, int x, int y, char remoteAction)
{
	if( destBaseObjRecno && base_obj_array.is_deleted(destBaseObjRecno)
		|| !can_set_rally_point(destBaseObjRecno) )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <dest baseObjRecno> <x> <y>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_SET_RALLY_DEST, 4 * sizeof(short));
		*shortPtr = firm_recno;
		shortPtr[1] = destBaseObjRecno;
		shortPtr[2] = x;
		shortPtr[3] = y;
		return;
	}

	rally_enable_flag = 1;
	rally_dest_base_obj_recno = destBaseObjRecno;
	rally_dest_x = x;
	rally_dest_y = y;
}
// ------- end of function Firm::set_rally_point -------//


// ------- begin of function Firm::clear_rally_point -------//
//
void Firm::clear_rally_point(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_CLEAR_RALLY_DEST, sizeof(short));
		*shortPtr = firm_recno;
		return;
	}

	rally_enable_flag = 0;
}
// ------- end of function Firm::clear_rally_point -------//


// ------- begin of function Firm::send_to_rally_point -------//
//
// return true on successful send the command
//
int Firm::send_to_rally_point( int unitRecno )
{
	if( unitRecno && rally_enable_flag )
	{
		err_when( unit_array.is_deleted(unitRecno) );

		if( rally_dest_base_obj_recno
			&& !base_obj_array.is_deleted(rally_dest_base_obj_recno)
			&& base_obj_array[rally_dest_base_obj_recno]->can_accept_assign(unitRecno, nation_recno) )
		{
			unit_array[unitRecno]->assign(rally_dest_x, rally_dest_y, nation_recno );
			return 1;
		}
		else
		{
			unit_array[unitRecno]->move(rally_dest_x, rally_dest_y );
			return 1;
		}
	}

	return 0;
}
// ------- end of function Firm::send_to_rally_point -------//
