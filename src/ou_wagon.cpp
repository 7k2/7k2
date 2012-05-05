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

// Filename    : OU_WAGON.CPP
// Description : Class Wagon


#include <ou_wagon.h>
#include <all.h>
#include <osys.h>
#include <ospy.h>
#include <onation.h>
#include <onews.h>
#include <oraceres.h>

// ------ begin of function UnitWagon::UnitWagon --------//
//
UnitWagon::UnitWagon()
{
	init_derived();
}
// ------ end of function UnitWagon::UnitWagon --------//


// ------ begin of function UnitWagon::init_derived --------//
//
void UnitWagon::init_derived()
{
	population = 0;		// instead of zero to maintain NationBase::total_human_count;
	spy_count = 0;
	memset(spy_recno, 0, sizeof(short) * MAX_WAGON_POPULATION );
}
// ------ end of function UnitWagon::init_derived --------//

// ------ begin of function UnitWagon::~UnitWagon --------//
//
UnitWagon::~UnitWagon()
{
	deinit();
}
// ------ end of function UnitWagon::~UnitWagon --------//

// ------ begin of function UnitWagon::deinit --------//
//
void UnitWagon::deinit()
{
	// ------ set population die ------- //

	if( nation_recno && !nation_array.is_deleted(nation_recno) )
	{
		nation_array[nation_recno]->total_human_count -= population;
		population = 0;		// avoid called deinit twice and reduce twice
	}

	// ------- set spy die ---------//

	for( int i = spy_count-1; i >= 0; --i )
	{
		short spyRecno = spy_recno[i];		// spy_count decreased

		if( spy_array.is_deleted(spyRecno) )
			continue;

		Spy *spyPtr = spy_array[spyRecno];


		//--- if the spy is killed in enemy kingdom, display a message ---//

		if( spyPtr->true_nation_recno != spyPtr->cloaked_nation_recno )
		{
			//------ decrease reputation ------//

			// nation_array[spyPtr->true_nation_recno]->change_reputation((float)-SPY_KILLED_REPUTATION_DECREASE);

			//------- add news message -------//

			if( !sys.signal_exit_flag 
				&& spyPtr->true_nation_recno == nation_array.player_recno )	// display when the player's spy is revealed or the player has revealed an enemy spy
			{
				news_array.spy_killed(spyRecno);
			}
		}

		// remove spy in spy_array

		spy_array.del_spy(spyRecno);
	}
}
// ------ end of function UnitWagon::deinit --------//



// ------ begin of function UnitWagon::set_pop --------//
//
// change the population of the wagon
// also change hit_points
// <int> p          - new population
//
void UnitWagon::set_pop(int p)
{
	int popChange = p - population;
	population = p;

	UnitInfo *unitInfo = unit_res[unit_id];

	// ------- change nationCount on human ------//

	if( nation_recno && !nation_array.is_deleted(nation_recno) )
	{
		nation_array[nation_recno]->total_human_count += popChange;
	}
/*
	// ------- change maximum hit points --------//

	int oldMaxHitPoints = max_hit_points();
	max_hit_points = unitInfo->hit_points * p / MAX_WAGON_POPULATION;
	hit_points = hit_points * max_hit_points / oldMaxHitPoints;
	hit_points = min(hit_points, max_hit_points);
*/
}
// ------ end of function UnitWagon::set_pop --------//


// ------ begin of function UnitWagon::add_spy --------//
//
// if any population is a spy call this function
// to record the spy recno
//
void UnitWagon::add_spy(short spyRecno)
{
	err_when( spy_count >= MAX_WAGON_POPULATION );		// no space for spy

	if( spy_count < MAX_WAGON_POPULATION )
	{
		spy_recno[spy_count] = spyRecno;
		++spy_count;
	}
}
// ------ end of function UnitWagon::add_spy --------//


// ------ begin of function UnitWagon::del_spy --------//
//
// <short> spyRecno			-1=the first spy on the array, >0 any specific spy recno
//                         (default:-1)
// return : the spy removed
///
short	UnitWagon::del_spy(short spyRecno)
{
	if( spy_count <= 0 )
		return 0;

	if( spyRecno < 0 )
	{
		spyRecno = spy_recno[0];
		m.del_array_rec(spy_recno, MAX_WAGON_POPULATION, sizeof(spy_recno[0]), 1);
	}
	else if( spyRecno > 0 )
	{
		int i;
		for(i=0; i < MAX_WAGON_POPULATION && spy_recno[i] != spyRecno; ++i);
		if( i >= MAX_WAGON_POPULATION )		// spy not found
			return 0;
		m.del_array_rec(spy_recno, MAX_WAGON_POPULATION, sizeof(spy_recno[0]), i+1);
	}
	else
	{
		err_here();
	}

	--spy_count;
	return spyRecno;
}
// ------ end of function UnitWagon::del_spy --------//


// ------ begin of function UnitWagon::explore_for_player -----//
//
int UnitWagon::explore_for_player()
{
	if( Unit::explore_for_player() )
		return 1;

	// if any spy, also explore

	for( int i = 0; i < spy_count; ++i )
	{
		if( spy_array[spy_recno[i]]->true_nation_recno == nation_array.player_recno )
		{
			return 1;
		}
	}
	return 0;
}
// ------ end of function UnitWagon::explore_for_player -----//


// ------ begin of function UnitWagon::change_nation -----//
//
void UnitWagon::change_nation(int newNationRecno )
{
	int oldNationRecno = nation_recno;

	// -------- change population -----------//

	if( oldNationRecno && !nation_array.is_deleted(oldNationRecno) )
	{
		nation_array[oldNationRecno]->total_human_count -= population;
		err_when(nation_array[oldNationRecno]->total_human_count < 0);
	}

	// ---------------------------------------//

	Unit::change_nation(newNationRecno);

	// -------- change cloaked nation of each spy ----------//

	for( int i = 0; i < spy_count; ++i )
	{
		// spy_array[spy_recno[i]]->change_cloaked_nation(newNationRecno);
		spy_array[spy_recno[i]]->cloaked_nation_recno = newNationRecno;
	}

	// --------- change population ----------//

	if( newNationRecno && !nation_array.is_deleted(newNationRecno) )
	{
		nation_array[newNationRecno]->total_human_count += population;
		err_when(nation_array[newNationRecno]->total_human_count < 0);
	}

}
// ------ end of function UnitWagon::change_nation -----//


// ------ begin of function UnitWagon::mobilize_civilian -------//
//
short UnitWagon::mobilize_civilian( short spyRecno )
{
	if( spyRecno == 0 )
	{
		// force to select a spy
		if( population <= spy_count )
		{
			err_when( spy_count == 0 );
			spyRecno = spy_recno[spy_count-1];
		}
	}
	else
	{
		// check if the spy is inside
		int i;
		for( i = 0; i < spy_count && spyRecno != spy_recno[i]; ++i );
		if( i >= spy_count )		// spy not found
			return 0;
	}

	// create a civilian unit

	err_when( !race_id );
	int civUnitId = race_res[race_id]->civilian_unit_id;

	int civUnitRecno = 0;
	SpriteInfo *civSpriteInfo = sprite_res[unit_res[civUnitId]->sprite_id];
	for( int r = 1; r < 16; ++r )
	{
		int x1 = obj_loc_x1() - r;
		int y1 = obj_loc_y1() - r;
		int x2 = obj_loc_x2() + r;
		int y2 = obj_loc_y2() + r;

		if( x1 < 0 )
			x1 = 0;
		if( x2 >= MAX_WORLD_X_LOC )
			x2 = MAX_WORLD_X_LOC-1;
		if( y1 < 0 )
			y1 = 0;
		if( y2 >= MAX_WORLD_Y_LOC )
			y2 = MAX_WORLD_Y_LOC-1;

		for( int y = y2; y >= y1; --y )
		{
			int x = x2;
			Location *locPtr = world.get_loc( x, y );
			for( ; x >= x1; --x, --locPtr )
			{
				if( world.check_unit_space(x, y, x+civSpriteInfo->loc_width-1, y+civSpriteInfo->loc_height-1) )
				{
					err_when( civUnitRecno );
					civUnitRecno = unit_array.add_unit( civUnitId, nation_recno, RANK_SOLDIER, loyalty, x, y );
					if( civUnitRecno )
						break;
				}
			}

			if( civUnitRecno )
				break;
		}

		if( civUnitRecno )
			break;
	}

	if( !civUnitRecno )
		return 0;
	
	// set spy place

	if( spyRecno )
	{
		spy_array[spyRecno]->set_place( SPY_MOBILE, civUnitRecno );
		unit_array[civUnitRecno]->spy_recno = spyRecno;
		// del_spy(spyRecno); // called by set_place already
	}

	// set population

	set_pop( population-1 );

	if( population <= 0 )
	{
		hit_points = 0.0f;		// going to be deleted
	}

	return civUnitRecno;
}

// ------ end of function UnitWagon::mobilize_civilian -------//
