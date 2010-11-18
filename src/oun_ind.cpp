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

//Filename    : OUN_IND.CPP
//Description : Object independent Unit AI

#include <osys.h>
#include <ospy.h>
#include <orebel.h>
#include <ounit.h>
#include <oitemres.h>
#include <oconfig.h>
#include <of_camp.h>
#include <onation.h>

//--------- Begin of function Unit::think_independent_unit --------//
//
// Think about the action of an independent unit. It first tries to
// settle to a town. If not successful, it will disband itself.
//
void Unit::think_independent_unit()
{
	if( !is_all_stop() )
		return;

	if( unit_mode )		// don't let them think a new action if they have something to do currently
		return;

	//------ if this unit is a hero -------//

	if( hero_id )
	{
		think_independent_hero();
		return;
	}

	//--- don't process if it's a spy and the notify cloak flag is on ---//

	if( spy_recno )
	{
		//---------------------------------------------//
		//
		// If notify_cloaked_nation_flag is 0, the AI
		// won't control the unit.
		//
		// If notify_cloaked_nation_flag is 1, the AI
		// will control the unit. But not immediately,
		// it will do it once 5 days so the player can
		// have a chance to select the unit and set its
		// notify_cloaked_nation_flag back to 0 if the
		// player wants.
		//
		//---------------------------------------------//

		if( spy_array[spy_recno]->notify_cloaked_nation_flag==0 )
			return;

		if( info.game_date%5 != sprite_recno%5 )
			return;
	}

	//-------- if this is a rebel ----------//

	if( unit_mode == UNIT_MODE_REBEL )
	{
		Rebel* rebelPtr = rebel_array[unit_mode_para];

		//--- if the group this rebel belongs to already has a rebel town, assign to it now ---//

		if( rebelPtr->town_recno )
		{
			if( !town_array.is_deleted(rebelPtr->town_recno) )
			{
				Town* townPtr = town_array[rebelPtr->town_recno];

				err_when( townPtr->rebel_recno != rebelPtr->rebel_recno );

				assign(townPtr->loc_x1, townPtr->loc_y1);
			}

			return;			// don't do anything if the town has been destroyed, Rebel::next_day() will take care of it. 
		}
	}
	//---- look for towns to assign to -----//

	Town *townPtr, *bestTown=NULL;
	int  regionId = world.get_region_id( next_x_loc(), next_y_loc() );
	int  curRating, bestRating=0;
	int  curXLoc = next_x_loc(), curYLoc = next_y_loc();

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno ||
			 townPtr->population >= MAX_TOWN_POPULATION ||
			 townPtr->region_id != regionId ||
			 townPtr->race_id != race_id )
		{
			continue;
		}

		curRating = world.distance_rating(curXLoc, curYLoc,
						townPtr->center_x, townPtr->center_y );

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestTown   = townPtr;
		}
	}

	if( bestTown )
	{
		err_when( unit_mode==UNIT_MODE_REBEL && rebel_array[unit_mode_para]->town_recno &&
					 rebel_array[unit_mode_para]->town_recno != bestTown->town_recno );

		assign(bestTown->loc_x1, bestTown->loc_y1);
	}
	else
		resign(COMMAND_AI);
}
//---------- End of function Unit::think_independent_unit --------//


//--------- Begin of function Unit::think_independent_hero --------//
//
void Unit::think_independent_hero()
{
	//------ think about go to serve a kingdom -------//

	int bestNationRecno=0, curRating;

	int bestRating = (skill_level() + combat_level()/2) / 3;

	if( item.id )
		bestRating += (item_res.rareness(item.id)+1) * 10;

	//------------------------------------------------//

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		Nation* nationPtr = nation_array[i];

		curRating = (int) nationPtr->reputation + nationPtr->overall_rating;

		if( sprite_recno%2==0 )			// some heroes look at kill_monster_score, but some don't
			curRating += (int) nationPtr->kill_monster_score/10;

		if( race_id == nationPtr->race_id )
			curRating += 10;

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestNationRecno = i;
		}
	}

	if( bestNationRecno )
	{
		//------ change nation now --------//

		if( !betray(bestNationRecno) )
			return;

		//---- the unit moves close to the newly joined nation ----//

		ai_move_to_nearby_town();
		return;
	}

	//---- randomly locate a destination to walk to ----//

	int xLoc, yLoc, regionId = region_id();

	for( i=100 ; i>0 ; i-- )
	{
		xLoc = m.random(MAX_WORLD_X_LOC);
		yLoc = m.random(MAX_WORLD_Y_LOC);

		Location* locPtr = world.get_loc(xLoc, yLoc);

		if( locPtr->walkable() && locPtr->region_id == regionId )
			break;
	}

	if( i==0 )
		return;

	move(xLoc, yLoc);
}
//---------- End of function Unit::think_independent_hero --------//
