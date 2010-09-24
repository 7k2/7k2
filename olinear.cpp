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

// Filename    : OLINEAR.CPP
// Description : Linear increment / decrement

#include <OLINEAR.H>
#include <ALL.H>


// like a line drawing algorithm
// draw a line from a point (x0, y0) to (x1, y1)
// but |y1-y0| need not less than or equal to |x1-x0|
// x1 can be less than x0 for backward traversal

// 1. first call init to set the value of x0, y0, x1, y1
// 2. check the the count end by calling is_end()
// 3. after the value of y is taken, call inc() to 
//     increase (or decrease) x and update y 
// 4. go back to step 2


// ------- begin of function LinearCounter::LinearCount ------- //
//
LinearCounter::LinearCounter()
{
}
// ------- end of function LinearCounter::LinearCount ------- //


// ------- begin of function LinearCounter::LinearCount ------- //
//
LinearCounter::LinearCounter(int x0, int y0, int x1, int y1 )
{
	init( x0, y0, x1, y1 );
}
// ------- end of function LinearCounter::LinearCount ------- //


// ------- begin of function LinearCounter::init ------- //
//
void LinearCounter::init(int x0, int y0, int x1, int y1 )
{
	dx = x1-x0;
	dy = y1-y0;

	x_dest = x1;
	x = x0;
	y = y0;

	err_when( dx < 0 );

	if( dx > 0 )
	{
		q = dy / dx;		// quotient
		r = dy % dx;		// remainder

		err_when( q * dx + r != dy );

		if( dy >= 0 )
			d = r * 2 - dx;
		else
			d = r * 2 + dx;
	}
	else if( dx < 0 )
	{
		err_here();				// not support negative dx yet
	}
	else	// dx == 0
	{
		q = dy / dx;		// quotient
		r = dy % dx;		// remainder

		err_when( q * dx + r != dy );

		if( dy >= 0 )
			d = r * -2 - dx;
		else
			d = r * -2 + dx;
	}
}
// ------- end of function LinearCounter::init ------- //


// ------- begin of function LinearCounter::is_end ------- //
//
int LinearCounter::is_end()
{
	if( dx >= 0 )
		return x > x_dest;
	else
		return x < x_dest;
}
// ------- end of function LinearCounter::is_end ------- //


// ------- begin of function LinearCounter::inc ------- //
//
void LinearCounter::inc()
{
	if( dx > 0 )
	{
		if( dy >= 0 )
		{
			if( d < 0 )
			{
				y += q;
				d += r * 2;
			}
			else
			{
				y += q + 1;
				d += (r - dx) * 2;
			}
		}
		else
		{
			if( d >= 0 )
			{
				y += q;
				d += r * 2;
			}
			else
			{
				y += q - 1;
				d += (r + dx) * 2;
			}
		}
		++x;
	}
	else if( dx < 0 )
	{
		err_here();			// not tested
		if( dy < 0 )
		{
			if( d >= 0 )
			{
				y -= q;
				d -= r * 2;
			}
			else
			{
				y -= q + 1;
				d -= (r - dx) * 2;
			}
		}
		else
		{
			if( d < 0 )
			{
				y -= q;
				d -= r * 2;
			}
			else
			{
				y -= q - 1;
				d -= (r + dx) * 2;
			}
		}
		--x;
	}
	else	// dx == 0
	{
		++x;		// so that is_end return true after inc() once
	}
}
// ------- end of function LinearCounter::is_end ------- //
