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
 *
 *Filename    : ic.cpp
 *Description : copy a rectange from one surface to another
 *
 * converted to C++
 */

#include <cstring>

//--------- BEGIN OF FUNCTION IMGcopyW -----------
//
// Copy a rectange from one surface to another
//
// Note : No border checking is made in this function.
//        Placing an icon outside image buffer will cause serious BUG.
//
// short *imageBuf  - the pointer to the display surface buffer
// int  imgPitch    - pitch of the display surface buffer
// short *backBuf   - the pointer to the source surface buffer
// int  backPitch   - pitch of the source display surface buffer
// int  x1,y1       - the top left vertex of the bar ( note : width < 1024)
// int  x2,y2       - the bottom right vertex of the bar
//

void IMGcopyW(short *imageBuf, int imgPitch, short *backBuf, int backPitch, int x1, int y1, int x2, int y2)
{
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	int dest = y1*(imgPitch/2) + x1;
	int src = y1*(backPitch/2) + x1;
	for (int j=0; j<height; ++j, dest+=imgPitch/2, src+=backPitch/2)
	{
		memcpy( &imageBuf[ dest ], &backBuf[ src ], width*2 );
	}
}

//---------- END OF FUNCTION IMGcopyW ------------
