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

// Filename   : OW_TRV.CPP
// Description : world traverse, find units in range


#include <ow_trv.h>
#include <oworld.h>
#include <ounit.h>
#include <stdlib.h>

static int sort_center_x1;
static int sort_center_y1;
static int sort_center_x2;
static int sort_center_y2;
static BaseObj *sort_center_obj;

static int sort_unit_by_dist1( const void *, const void *);
static int sort_unit_by_dist2( const void *, const void *);
static int sort_unit_by_weight( const void *, const void *);

// ----- begin of function WorldScanTraverser::WorldScanTraverser -------//
//
WorldScanTraverser::WorldScanTraverser()
{
	scan_count = 0;
	traverse_count = 0;		// run from 0 to scan_count-1
	unit_recno_array = default_recno_array;
	alloc_count = DEFAULT_ARRAY_SIZE;

	// unit_recno_array points to default_recno_array
	// if scan count is larger than default_recno_array_SIZE
	// new a larger array
}
// ----- end of function WorldScanTraverser::WorldScanTraverser -------//


// ----- begin of function WorldScanTraverser::~WorldScanTraverser -------//
//
WorldScanTraverser::~WorldScanTraverser()
{
	if( unit_recno_array != default_recno_array )
	{
		mem_del(unit_recno_array);
	}
}
// ----- end of function WorldScanTraverser::~WorldScanTraverser -------//


// ----- begin of function WorldScanTraverser::init -------//
//
// locSize : size of center unit
//	
// include unit in the center
//
void WorldScanTraverser::init( int xLoc, int yLoc, int locSize, int radius)
{
	int area = (radius+locSize-1) * (radius+locSize-1);

	int x1, y1, x2, y2;

	x1 = xLoc - radius;
	if( x1 < 0 )
		x1 = 0;
	x2 = xLoc + locSize + radius - 1;
	if( x2 >= MAX_WORLD_X_LOC )
		x2 = MAX_WORLD_X_LOC - 1;
	y1 = yLoc - radius;
	if( y1 < 0 )
		y1 = 0;
	y2 = yLoc + locSize + radius - 1;
	if( y2 >= MAX_WORLD_Y_LOC )
		y2 = MAX_WORLD_Y_LOC - 1;

	// rectangle find

	for( int y = y1; y <= y2; ++y )
	{
		Location *locPtr = world.get_loc( x1, y );
		for( int x = x1; x <= x2; ++x, ++locPtr )
		{
			// add land unit

			int unitRecno = locPtr->unit_recno(UNIT_LAND);
			if( !unit_array.is_deleted(unitRecno) )	// reject if dying
			{	
				Unit *unitPtr = unit_array[unitRecno];

				err_when( unitPtr->loc_width == 1 && unitPtr->loc_height > 1 );

				if( unitPtr->loc_width == 1 
					|| x == MAX(unitPtr->obj_loc_x1(),x1) && y == MAX(unitPtr->obj_loc_y1(),y1) )	// add once
				{
					append(unitRecno);
				}
			}

			// add sea unit

			unitRecno = locPtr->unit_recno(UNIT_SEA);
			if( !unit_array.is_deleted(unitRecno) )	// reject if dying
			{	
				Unit *unitPtr = unit_array[unitRecno];

				err_when( unitPtr->loc_width == 1 && unitPtr->loc_height > 1 );

				if( unitPtr->loc_width == 1 
					|| x == MAX(unitPtr->obj_loc_x1(),x1) && y == MAX(unitPtr->obj_loc_y1(),y1) )
				{
					append(unitRecno);
				}
			}

			// add air unit

			unitRecno = locPtr->unit_recno(UNIT_AIR);
			if( !unit_array.is_deleted(unitRecno) )	// reject if dying
			{	
				Unit *unitPtr = unit_array[unitRecno];

				err_when( unitPtr->loc_width == 1 && unitPtr->loc_height > 1 );

				if( unitPtr->loc_width == 1 
					|| x == MAX(unitPtr->obj_loc_x1(),x1) && y == MAX(unitPtr->obj_loc_y1(),y1) )
				{
					append(unitRecno);
				}
			}

		}	// end for x
	}	// end for y

	traverse_count = 0;
}
// ----- end of function WorldScanTraverser::init -------//


// ----- begin of function WorldScanTraverser::append -------//
//
void WorldScanTraverser::append(short unitRecno)
{
	if( scan_count >= alloc_count )
	{
		// find new alloc_count

		alloc_count = alloc_count + DEFAULT_ARRAY_SIZE;
		err_when( alloc_count <= scan_count);	// still now enough

		if( unit_recno_array == default_recno_array )
		{
			// default array is not enough, use own array

			unit_recno_array = (ScannedObj *)mem_add( alloc_count*sizeof(*unit_recno_array) );
			memcpy( unit_recno_array, default_recno_array, scan_count*sizeof(*unit_recno_array) );
		}
		else
		{
			unit_recno_array = (ScannedObj *)mem_resize( unit_recno_array, alloc_count*sizeof(*unit_recno_array) );
		}
	}

	unit_recno_array[scan_count].unit_recno = unitRecno;
	++scan_count;
}
// ----- end of function WorldScanTraverser::append -------//


// ----- begin of function WorldScanTraverser::is_finish -------//
//
int WorldScanTraverser::is_finish()
{
	return traverse_count >= scan_count;
}
// ----- end of function WorldScanTraverser::is_finish -------//


// ----- begin of function WorldScanTraverser::get_unit -------//
//
int WorldScanTraverser::get_unit()
{
	err_when( is_finish() );
	return unit_recno_array[traverse_count].unit_recno;
}
// ----- end of function WorldScanTraverser::get_unit -------//


// ----- begin of function WorldScanTraverser::next -------//
//
void WorldScanTraverser::next()
{
	++traverse_count;
}
// ----- end of function WorldScanTraverser::next -------//


// ----- begin of function WorldScanTraverser::restart -------//
//
void WorldScanTraverser::restart()
{
	traverse_count = 0;
}
// ----- end of function WorldScanTraverser::restart -------//


// ----- begin of function WorldScanTraverser::remove -------//
//
// delete current element, delete the one which get_unit returns
//
// automatically traverse to next unit, don't call next() immediately
//
void WorldScanTraverser::remove()
{
	err_when( is_finish() );

// remove is not prefered as it take time to memmove
//	if( scan_count > 0 )
//	{
//		m.del_array_rec( unit_recno_array, scan_count, sizeof(*unit_recno_array), traverse_count+1 );
//		--scan_count;
//		--traverse_count;		// move backward so next() will advance traverse_count
//	}

	unit_recno_array[traverse_count].unit_recno = 0;
	
}
// ----- end of function WorldScanTraverser::remove -------//


// ----- begin of function WorldScanTraverser::pack_removed -------//
//
// note that the current pointer (traverse_count) is not maintained
//
void WorldScanTraverser::pack_removed()
{
	int i, j;

	int oldScanCount = scan_count;
	for( i = j = 0; i < oldScanCount; ++i )
	{
		if( unit_recno_array[i].unit_recno != 0 )
		{
			// if( i != j )		// avoid overwrite itself
			unit_recno_array[j] = unit_recno_array[i];
			++j;
		}
	}

	scan_count = j;
	traverse_count = 0;
}
// ----- end of function WorldScanTraverser::pack_removed -------//


// ----- begin of function WorldScanTraverser::get_weight -------//
//
short	WorldScanTraverser::get_weight()
{
	err_when( is_finish() );
	return unit_recno_array[traverse_count].sort_weight;
}
// ----- end of function WorldScanTraverser::get_weight -------//


// ----- begin of function WorldScanTraverser::set_weight -------//
//
void WorldScanTraverser::set_weight(short w)
{
	err_when( is_finish() );
	unit_recno_array[traverse_count].sort_weight = w;
}
// ----- end of function WorldScanTraverser::set_weight -------//


// ----- begin of function WorldScanTraverser::sort_by_distance -------//
//
// call pack_removed() before sort, if any unit deleted
//
void WorldScanTraverser::sort_by_distance( int xLoc, int yLoc, int locSize )
{
	sort_center_x1 = xLoc;
	sort_center_y1 = yLoc;
	sort_center_x2 = xLoc + locSize - 1;
	sort_center_y2 = yLoc + locSize - 1;
	qsort( unit_recno_array, scan_count, sizeof(*unit_recno_array), sort_unit_by_dist1 );
}
// ----- end of function WorldScanTraverser::sort_by_distance -------//


// ----- begin of function WorldScanTraverser::sort_by_distance -------//
//
void WorldScanTraverser::sort_by_distance( BaseObj *centerObj )
{
	sort_center_obj = centerObj;
	qsort( unit_recno_array, scan_count, sizeof(*unit_recno_array), sort_unit_by_dist2 );
}
// ----- end of function WorldScanTraverser::sort_by_distance -------//


// ----- begin of function WorldScanTraverser::sort_by_weight -------//
//
void WorldScanTraverser::sort_by_weight()
{
	qsort( unit_recno_array, scan_count, sizeof(*unit_recno_array), sort_unit_by_weight );
}
// ----- end of function WorldScanTraverser::sort_by_weight -------//


// ------ begin of static function sort_unit_by_dist1 -------//
//
static int sort_unit_by_dist1( const void *shortPtr1, const void *shortPtr2 )
{
	Unit *unitA = unit_array[ ((ScannedObj *)shortPtr1)->unit_recno ];
	Unit *unitB = unit_array[ ((ScannedObj *)shortPtr2)->unit_recno ];

	return m.area_distance( sort_center_x1, sort_center_y1, sort_center_x2, sort_center_y2,
		unitA->obj_loc_x1(), unitA->obj_loc_y1(), unitA->obj_loc_x2(), unitA->obj_loc_y2() )
		- m.area_distance( sort_center_x1, sort_center_y1, sort_center_x2, sort_center_y2,
		unitB->obj_loc_x1(), unitB->obj_loc_y1(), unitB->obj_loc_x2(), unitB->obj_loc_y2() );
}
// ------ begin of static function sort_unit_by_dist1 -------//


// ------ begin of static function sort_unit_by_dist2 -------//
//
// call pack_removed() before sort, if any unit deleted
//
static int sort_unit_by_dist2( const void *shortPtr1, const void *shortPtr2 )
{
	Unit *unitA = unit_array[ ((ScannedObj *)shortPtr1)->unit_recno ];
	Unit *unitB = unit_array[ ((ScannedObj *)shortPtr2)->unit_recno ];

	return sort_center_obj->area_distance(unitA) - sort_center_obj->area_distance(unitB);
}
// ------ end of static function sort_unit_by_dist2 -------//



// ------ begin of static function sort_unit_by_weight -------//
//
static int sort_unit_by_weight( const void *shortPtr1, const void *shortPtr2 )
{
	return ((ScannedObj *)shortPtr1)->sort_weight - ((ScannedObj *)shortPtr2)->sort_weight;
}
// ------ begin of static function sort_unit_by_weight -------//
