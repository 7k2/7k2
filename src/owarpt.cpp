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

// Filename    : OWARPT.CPP
// Description : War point


#include <OWARPT.H>
#include <COLOR.H>
#include <OVGABUF.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OMODEID.H>
#include <ALL.H>

// --------- define constant ---------//

// divide the map into zone, each zone has size WARPOINT_ZONE_SIZE

#define WARPOINT_STRENGTH 0x100000
#define WARPOINT_STRENGTH_MAX 0x1000000

#define DRAW_PHASE_PERIOD 16


// ---------- begin of function WarPoint::inc ----------//
void WarPoint::inc()
{
	if( (strength += WARPOINT_STRENGTH) > WARPOINT_STRENGTH_MAX )
		strength = WARPOINT_STRENGTH_MAX;
}
// ---------- end of function WarPoint::inc ----------//


// ---------- begin of function WarPoint::decay ----------//
inline void WarPoint::decay()
{
	strength >>= 1;
}
// ---------- end of function WarPoint::decay ----------//


// ------ begin of function WarPointArray::WarPointArray -------//
WarPointArray::WarPointArray()
{
	war_point = NULL;
	init_flag = 0;
}
// ------ end of function WarPointArray::WarPointArray -------//


// ------ begin of function WarPointArray::~WarPointArray -------//
WarPointArray::~WarPointArray()
{
	deinit();
}
// ------ end of function WarPointArray::~WarPointArray -------//


// ------ begin of function WarPointArray::init -------//
void WarPointArray::init()
{
	deinit();

	// allocate memory for war_point
	war_point = (WarPoint *)mem_add( sizeof(WarPoint) * WARPOINT_ZONE_COLUMN * WARPOINT_ZONE_ROW );
	memset(war_point, 0, sizeof(WarPoint) * WARPOINT_ZONE_COLUMN * WARPOINT_ZONE_ROW );

	active_zone_count = 0;
	draw_phase = 0;
	init_flag = 1;
}
// ------ end of function WarPointArray::init -------//


// ------ begin of function WarPointArray::deinit -------//
void WarPointArray::deinit()
{
	if( init_flag )
	{
		mem_del(war_point);
		init_flag = 0;
	}
}
// ------ end of function WarPointArray::deinit -------//


// ------ begin of function WarPointArray::draw_dot -------//
void WarPointArray::draw_dot()
{
	if( ++draw_phase >= DRAW_PHASE_PERIOD )
		draw_phase = 0;

	if( draw_phase & 1)
		return;

	static unsigned char dotColor[DRAW_PHASE_PERIOD/2] = 
		{ 0xa0, 0xa4, 0xa8, 0x00, 0xb0, 0xb4, 0xb8, 0x00 };
	// unsigned char color = dotColor[draw_phase / 2];
	short color = vga_back.translate_color(dotColor[draw_phase / 2]);

	int x,y;
	int vgaBufPitch = vga_back.buf_pitch();

	int rowWriteInc = ((world.map_matrix->loc_y_height * WARPOINT_ZONE_SIZE) >> LOC_XY_MUL_SHIFT)
		 * vgaBufPitch + ((world.map_matrix->loc_y_width * WARPOINT_ZONE_SIZE) >> LOC_XY_MUL_SHIFT);
	int colWriteInc = ((world.map_matrix->loc_x_height * WARPOINT_ZONE_SIZE) >> LOC_XY_MUL_SHIFT)
		 * vgaBufPitch + ((world.map_matrix->loc_x_width * WARPOINT_ZONE_SIZE) >> LOC_XY_MUL_SHIFT);

	// pointer for drawing a cross
	short *rowWritePtr1 = vga_back.buf_ptr(
		world.map_matrix->calc_loc_pos_x( 0, (WARPOINT_ZONE_SIZE-1)/2 ) + MAP_X1,
		world.map_matrix->calc_loc_pos_y( 0, (WARPOINT_ZONE_SIZE-1)/2 ) + MAP_Y1 );
	short *rowWritePtr2 = vga_back.buf_ptr(
		world.map_matrix->calc_loc_pos_x( (WARPOINT_ZONE_SIZE-1)/2, 0 ) + MAP_X1,
		world.map_matrix->calc_loc_pos_y( (WARPOINT_ZONE_SIZE-1)/2, 0 ) + MAP_Y1 );

 	for( y = 0; y < WARPOINT_ZONE_ROW; ++y, (rowWritePtr1 += rowWriteInc), (rowWritePtr2 += rowWriteInc) )
	{
		WarPoint *warPt = war_point + y * WARPOINT_ZONE_COLUMN;
		short *colWritePtr1 = rowWritePtr1;
		short *colWritePtr2 = rowWritePtr2;
		for( x = 0; x < WARPOINT_ZONE_COLUMN; ++x, ++warPt, (colWritePtr1 += colWriteInc), (colWritePtr2 += colWriteInc) )
		{
			if( warPt->strength > 0 )
			{
				// draw a cross, UNEXPLORED_COLOR is not needed to check
				short *writePtr1 = colWritePtr1;
				short *writePtr2 = colWritePtr2;
				for( int i = 0; i < WARPOINT_ZONE_SIZE/2; ++i, (writePtr1+=vgaBufPitch+1), (writePtr2+=vgaBufPitch-1) )
				{
					*writePtr1 = color;
					*writePtr2 = color;
				}
			}
		}
	}
}
// ------ end of function WarPointArray::draw_dot -------//


// ------ begin of function WarPointArray::process -------//
void WarPointArray::process()
{
	int activeCount = 0;
	register WarPoint *warPt = war_point;
	for( int i = WARPOINT_ZONE_COLUMN * WARPOINT_ZONE_ROW; i > 0; --i, warPt++)
	{
		warPt->decay();
		if( warPt->strength > 0 )
			activeCount++;		// faster to access local variable ?
	}
	active_zone_count = activeCount;
}
// ------ end of function WarPointArray::process -------//


// ------ begin of function WarPointArray::get_ptr -------//
WarPoint *WarPointArray::get_ptr(int xLoc, int yLoc)
{
	err_when(!init_flag);
	int c = xLoc / WARPOINT_ZONE_SIZE;
	int r = yLoc / WARPOINT_ZONE_SIZE;
	err_when( c < 0 || c >= WARPOINT_ZONE_COLUMN);
	err_when( r < 0 || r >= WARPOINT_ZONE_ROW);
	return war_point+ (r * WARPOINT_ZONE_COLUMN + c);
}
// ------ end of function WarPointArray::get_ptr -------//


// ------ begin of function WarPointArray::add_point -------//

void WarPointArray::add_point(int xLoc, int yLoc)
{
	get_ptr(xLoc, yLoc)->inc();
}
// ------ end of function WarPointArray::add_point -------//
