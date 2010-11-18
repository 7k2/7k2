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

// Filename    : OROCK.CPP
// Description : class Rock and RockArray
// Owner       : Gilbert


#include <OROCK.H>
#include <OROCKRES.H>
#include <OWORLD.H>
#include <OCONFIG.H>

// --------- define constant ----------//
#define ROCK_ALT_PATH 19


// ------- begin of function Rock::Rock -------//
//
Rock::Rock(short rockRecno, short xLoc, short yLoc)
{
	init( rockRecno, xLoc, yLoc );
}
// ------- end of function Rock::Rock -------//


// ------- begin of function Rock::init -------//
//
void Rock::init(short rockRecno, short xLoc, short yLoc)
{
	rock_recno = rockRecno;
	loc_x = xLoc;
	loc_y = yLoc;
	seed = (xLoc + yLoc + 3) * ( 2 * xLoc + 7 * yLoc + 5);

	// ------- random frame, random initial delay_remain  -----//
	RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
	cur_frame = 1 + (char) random(rockInfo->max_frame);

	RockBitmapInfo *rockBitmapInfo = 
		rock_res.get_bitmap_info(rock_res.get_bitmap_recno(rockRecno, cur_frame));
	delay_remain = 1 + (char) random(rockBitmapInfo->delay);

//	char initDelayCount = rock_res.get_anim_info(
//		rock_res.get_anim_recno(rockRecno, cur_frame))->delay;
//	delay_remain = 1 + (char) random(initDelayCount);
}
// ------- end of function Rock::init -------//


// ------- begin of function Rock::random -------//
//
unsigned Rock::random(unsigned bound)
{
   #define MULTIPLIER      0x015a4e35L
   #define INCREMENT       1
   seed = MULTIPLIER * seed + INCREMENT;
	return seed % bound;
}
// ------- end of function Rock::random -------//


// ------- begin of function Rock::process -------//
//
void Rock::process()
{
	if( --delay_remain <= 0)
	{
		cur_frame = rock_res.choose_next(rock_recno, cur_frame, random(ROCK_ALT_PATH) );
	//	delay_remain = rock_res.get_anim_info(rock_res.get_anim_recno(rock_recno, cur_frame))->delay;
		RockBitmapInfo *rockBitmapInfo = 
			rock_res.get_bitmap_info(rock_res.get_bitmap_recno(rock_recno, cur_frame));
		delay_remain = rockBitmapInfo->delay;
	}
}
// ------- end of function Rock::process -------//


// ------- begin of function Rock::draw -------//
//
void Rock::draw()
{
	RockInfo *rockInfo = rock_res.get_rock_info(rock_recno);
	rock_res.draw(rock_recno, loc_x, loc_y, 
		world.get_corner(loc_x + rockInfo->loc_width, loc_y + rockInfo->loc_height)->get_altitude(), cur_frame);
}
// ------- end of function Rock::draw -------//

//void Rock::draw_block(short xLoc, short yLoc)
//{
//	rock_res.draw_block(rock_recno, xLoc, yLoc, xLoc-loc_x, yLoc-loc_y, cur_frame);
//}


// ------- begin of function Rock::loc_x2 -------//
short	Rock::loc_x2()
{
	return loc_x + rock_res.get_rock_info(rock_recno)->loc_width - 1;
}
// ------- end of function Rock::loc_x2 -------//


// ------- begin of function Rock::loc_y2 -------//
//
short	Rock::loc_y2()
{
	return loc_y + rock_res.get_rock_info(rock_recno)->loc_height - 1;
}
// ------- end of function Rock::loc_y2 -------//


// ------- begin of function Rock::is_stealth -------//
//
int Rock::is_stealth()
{
	return config.blacken_map && !world.is_explored_quick( loc_x, loc_y, loc_x2(), loc_y2() );
}
// ------- end of function Rock::is_stealth -------//


// ------- begin of function Rock::rock_info -------//
//
RockInfo *Rock::rock_info()
{
	err_when( !rock_recno );
	return rock_res.get_rock_info(rock_recno);
}
// ------- end of function Rock::rock_info -------//


// ------- begin of function Rock::rock_bitmap_info -------//
//
RockBitmapInfo *Rock::rock_bitmap_info()
{
	err_when( !rock_recno );
	err_when( cur_frame <= 0 || cur_frame > rock_res.get_rock_info(rock_recno)->max_frame );

	return rock_res.get_bitmap_info(
		rock_res.get_bitmap_recno(rock_recno, cur_frame) );
}
// ------- end of function Rock::rock_bitmap_info -------//


// ------- begin of function RockArray::RockArray -------//
//
RockArray::RockArray(int initArraySize) : DynArrayB(sizeof(Rock),initArraySize, DEFAULT_REUSE_INTERVAL_DAYS)
{
}
// ------- end of function RockArray::RockArray -------//


// ------- begin of function RockArray::init -------//
//
void RockArray::init()
{
	zap();
}
// ------- end of function RockArray::init -------//


// ------- begin of function RockArray::deinit -------//
//
void RockArray::deinit()
{
}
// ------- end of function RockArray::deinit -------//


// ------- begin of function RockArray::is_deleted -------//
//
int RockArray::is_deleted(int i)
{
	Rock *rockPtr = (Rock *)get(i);
	return !rockPtr || !rockPtr->rock_recno;
}
// ------- end of function RockArray::is_deleted -------//


// ------- begin of function RockArray::process -------//
//
void RockArray::process()
{
	for( int i = 1; i <= size(); ++i)
	{
		if( !is_deleted(i) )
		{
			operator[](i)->process();
		}
	}
}
// ------- end of function RockArray::process -------//
