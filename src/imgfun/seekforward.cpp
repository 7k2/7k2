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

// Filename    : seekforward.cpp
// Description : Calculate number of pixels to skip

#include <colcode.h>

// BUGHERE : do not PRE_REMAP here, as some function do not need remap
//----------- BEGIN OF FUNCTION SeekForward -----
// change ESI, bytes started in compression key in AH
// to forward ECX points

void SeekForward(/*out*/ int *ppixelsToSkip, /*inout*/ int *pesi, char *bitmapBuf, int seekAmount)
{
	int esi = *pesi;
	int al;
	// decode the source image until we reach seekAmount pixels
	while (seekAmount > 0)
	{
		al = ((unsigned char*)bitmapBuf)[ esi++ ];		// load source byte
		if (al < MIN_TRANSPARENT_CODE)
		{
			--seekAmount;	// normal pixel
		}
		else if (al == MANY_TRANSPARENT_CODE)
		{
			seekAmount -= ((unsigned char*)bitmapBuf)[ esi++ ];		// skip many pixels
		}
		else
		{
			seekAmount -= 256 - al;					// skip (neg al) pixels
		}
	}
	*ppixelsToSkip = -seekAmount;
	*pesi = esi;
}
//----------- END OF FUNCTION SeekForward ------
