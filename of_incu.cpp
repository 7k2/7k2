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

// Filename    : OF_INCU.CPP
// Description : Incubator


#include <OF_INCU.H>
#include <ALL.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OSERES.H>
#include <ONATION.H>
#include <OUNIT.H>


short FirmIncubator::incubate_unit_id[MAX_INCUBATE_TYPE] =
{
	// ###### begin Gilbert 26/1 #######//
	UNIT_FR_BALLISTA,
	UNIT_F_CATAPULT,
	UNIT_F_CANNON,
	// ###### end Gilbert 26/1 #######//
};


// ------ begin of function FirmIncubator::FirmIncubator -------//
//
FirmIncubator::FirmIncubator()
{
	memset( (char *)this + sizeof(Firm), 0, sizeof(FirmIncubator) - sizeof(Firm) );
}
// ------ end of function FirmIncubator::FirmIncubator -------//


// ------ begin of function FirmIncubator::~FirmIncubator -------//

FirmIncubator::~FirmIncubator()
{
}
// ------ end of function FirmIncubator::~FirmIncubator -------//


// ------ begin of function FirmIncubator::add_queue -------//
//
void FirmIncubator::add_queue(int unitId)
{
	if( build_queue_count+(build_unit_id>0)==MAX_BUILD_QUEUE )
		return;

	build_queue_array[build_queue_count++] = unitId;
}
// ------ end of function FirmIncubator::add_queue -------//


// ------ begin of function FirmIncubator::remove_queue -------//
//
void FirmIncubator::remove_queue(int unitId)
{
	for( int i=build_queue_count-1 ; i>=0 ; i-- )
	{
		if( build_queue_array[i] == unitId )
		{
			err_when( build_queue_count > MAX_BUILD_QUEUE );

			m.del_array_rec( build_queue_array, build_queue_count, sizeof(build_queue_array[0]), i+1 );

			build_queue_count--;
			return;
		}
	}

	if(build_unit_id==unitId)
		cancel_build_unit();
}
// ------ end of function FirmIncubator::remove_queue -------//


// ------ begin of function FirmIncubator::cancel_build_unit -------//
//
void FirmIncubator::cancel_build_unit()
{
	build_unit_id = 0;
}
// ------ end of function FirmIncubator::cancel_build_unit -------//


// ------ begin of function FirmIncubator::next_day -------//
//
void FirmIncubator::next_day()
{
	//--------- process building weapon -------//

	if( build_unit_id )
		process_build();
	else
		process_queue();

	// -------- call next day of subclass ---------//

	Firm::next_day();
}
// ------ end of function FirmIncubator::next_day -------//


// ------ begin of function FirmIncubator::process_build -------//
//
void FirmIncubator::process_build()
{
	err_when( !build_unit_id );

	UnitInfo* unitInfo = unit_res[build_unit_id];
	int   totalBuildDays = unitInfo->build_days;

	build_progress_days += 1.0f;

	//---------------------------------------------------//

	last_process_build_frame_no = sys.frame_count;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		build_progress_days += (float) 50;

	if( build_progress_days > totalBuildDays )
	{
		SpriteInfo*	spriteInfo = sprite_res[ unitInfo->sprite_id ];
		int 			xLoc=loc_x1; // xLoc & yLoc are used for returning results
		int 			yLoc=loc_y1;

		if( !world.locate_space(xLoc, yLoc, loc_x2, loc_y2,
			 spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type) )
		{
			build_progress_days = (float)(totalBuildDays + 1);
			return;
		}

		int unitRecno = unit_array.add_unit( build_unit_id, nation_recno, 0, 0, xLoc, yLoc );

		// -------- assign to rally point -------//

		if( unitRecno )
			send_to_rally_point( unitRecno );

		// ------------------------------------//

		if( is_own() )
			se_res.far_sound(center_x, center_y, 1, 'F', firm_id, "FINS", 'S', unit_res[build_unit_id]->sprite_id);

		build_unit_id = 0;
	}
}
// ------ end of function FirmIncubator::process_build -------//


// ------ begin of function FirmIncubator::process_queue -------//
//
void FirmIncubator::process_queue()
{
	if( build_queue_count==0 )
		return;

	//--- first check if the nation has enough money to build the weapon ---//

	Nation* nationPtr = nation_array[nation_recno];
	build_unit_id = build_queue_array[0];

	if( nationPtr->cash < unit_res[build_unit_id]->build_cost 
		|| nationPtr->live_points < unit_res[build_unit_id]->build_live_points_cost )
	{
		build_unit_id = 0;
		return;
	}

	nationPtr->add_expense( EXPENSE_WEAPON, unit_res[build_unit_id]->build_cost, 1);
	nationPtr->change_live_points( float(-unit_res[build_unit_id]->build_live_points_cost) );

	err_when( build_queue_count > MAX_BUILD_QUEUE );

	m.del_array_rec( build_queue_array, build_queue_count, sizeof(build_queue_array[0]), 1 );

	build_queue_count--;

	//------- set building parameters -------//

	last_process_build_frame_no = sys.frame_count;
	build_progress_days = (float) 0;
}
// ------ end of function FirmIncubator::process_queue -------//


// ---------- Begin of function FirmIncubator::can_set_rally_point ------//
//
// whether the firm can set rally point
//
// [int] destBaseObjRecno		- destination base obj recno, 0 for setting position
//
int FirmIncubator::can_set_rally_point(int destBaseObjRecno)
{
	if( rally_enable_flag )		// must clear rally point before setting new one
		return 0;

	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		Firm *firmPtr = base_obj_array[destBaseObjRecno]->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return 1;
			}
		}
	}

	return 0;
}
// ---------- End of function FirmIncubator::can_set_rally_point ------//
