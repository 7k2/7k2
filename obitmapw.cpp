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

// Filename    : OBITMAPW.CPP

#include <OBITMAPW.H>
#include <ALL.H>


// ------ begin of function BitmapW::init ------//

void BitmapW::init(short w, short h)
{
	width_field = w;
	height_field = h;
}
// ------ end of function BitmapW::init ------//


// ------ begin of function BitmapW::get_pitch ------//

short BitmapW::get_pitch()
{
	return width_field;
}
// ------ end of function BitmapW::get_pitch ------//


// ------ begin of function BitmapW::get_true_pitch ------//

short BitmapW::get_true_pitch()
{
	return width_field * sizeof(short);
}
// ------ end of function BitmapW::get_pitch ------//


// ------ begin of function BitmapW::get_ptr -------//
short *BitmapW::get_ptr(short x, short y)
{
	return bitmap + y * width_field + x;
}
// ------ end of function BitmapW::get_ptr -------//

// ------ begin of function BitmapW::size ------//

long BitmapW::size()
{
	return sizeof(width_field)+sizeof(height_field)+ width_field*height_field*sizeof(short);
}
// ------ end of function BitmapW::size ------//


// ------ begin of function BitmapW::size ------//

long BitmapW::size(short w, short h)
{
	return sizeof(short)+sizeof(short)+ w*h*sizeof(short);
}
// ------ end of function BitmapW::size ------//


// ------ begin of function BitmapW::mem_add_bitmap ------//

BitmapW *BitmapW::mem_add_bitmap(short w, short h)
{
	BitmapW *bitmap = (BitmapW *)mem_add(size(w,h));
	bitmap->init(w,h);
	return bitmap;
}
// ------ end of function BitmapW::mem_add_bitmap ------//
