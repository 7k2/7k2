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

// Filename    : OEXPMASK.CPP
// Description : explored area mask
// Owner       : Gilbert


#include <OEXPMASK.H>
#include <OFILE.H>
#include <OWORLDMT.H>
#include <OVGABUF.H>
#include <OSTR.H>
#include <OCOLTBL.H>
#include <OCONFIG.H>
#include <OSYS.H>

// --------- Define Constant ------//

#define REMAP_FILENAME "EXPREMAP.RES"

#define MASK_WIDTH 32
#define MASK_HEIGHT 32


BYTE min4( BYTE a, BYTE b, BYTE c, BYTE d)
{
	if( a <= b )
	{
		if( c <= d )
			return min( a, c );
		else
			return min( a, d );
	}
	else
	{
		if( c <= d )
			return min( b, c );
		else
			return min( b, d );
	}
}


// ------- Begin of function ExploredMask::init ---------//

void ExploredMask::init()
{
	// ------- read into exploration remap bitmap ------//

	String str;
	long fileSize;

	str = DIR_RES;
	str += REMAP_FILENAME;
	File remapFile;
	remapFile.file_open(str);
	fileSize = remapFile.file_size();
	remap_bitmap = (char *) mem_add(fileSize);
	remapFile.file_read(remap_bitmap, fileSize);
	remapFile.file_close();
}
// ------- End of function ExploredMask::init ---------//


// ------- Begin of function ExploredMask::deinit ---------//
void ExploredMask::deinit()
{
	mem_del(remap_bitmap);
}
// ------- End of function ExploredMask::deinit ---------//


// given visit level of a location and eight visit level of the adjacent locations
//
// convert each visit level to fog level (binary 00, 01, 10)
//
// bit 0,1 of maskId is the minimum of fog level of (nw, n, w, and center)
// bit 2,3 of maskId is the minimum of fog level of (n, ne, center and e)
// bit 4,5 of maskId is the minimum of fog level of (w, center, sw, s)
// bit 6,7 of maskId is the minimum of fog level of (center, e, s, se)
//

// get the fog mask (32x32 pixels of fog level (0,1,2) )
// no header
char *ExploredMask::get_remap_mask( BYTE maskId )
{
	return mask_id_to_serial_id(maskId) * (MASK_WIDTH * MASK_HEIGHT) + remap_bitmap;
}


BYTE ExploredMask::make_mask_id( BYTE nwVisitLevel, BYTE nVisitLevel, BYTE neVisitLevel,
		BYTE wVisitLevel, BYTE cVisitLevel, BYTE eVisitLevel,
		BYTE swVisitLevel, BYTE sVisitLevel, BYTE seVisitLevel )
{
	BYTE retMaskId = 0;

	// compare nw, n, w and c
	// compare nw with c; n with w
	// finally min(nw,c) and min(c,w)

	// use nwVisitLevel to store the final result
	if( cVisitLevel < nwVisitLevel )
		nwVisitLevel = cVisitLevel;
	if( nVisitLevel < wVisitLevel )
	{
		if( nVisitLevel < nwVisitLevel )
			nwVisitLevel = nVisitLevel;
	}
	else
	{
		if( wVisitLevel < nwVisitLevel )
			nwVisitLevel = wVisitLevel;
	}

	if( nwVisitLevel >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && nwVisitLevel < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1;		// fog
		else
			retMaskId |= 2;		// fully explored
	}

	if( cVisitLevel < neVisitLevel )
		neVisitLevel = cVisitLevel;
	if( nVisitLevel < eVisitLevel )
	{
		if( nVisitLevel < neVisitLevel )
			neVisitLevel = nVisitLevel;
	}
	else
	{
		if( eVisitLevel < neVisitLevel )
			neVisitLevel = eVisitLevel;
	}
	if( neVisitLevel >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && neVisitLevel < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 2;		// fog
		else
			retMaskId |= 2 << 2;		// fully explored
	}


	if( cVisitLevel < swVisitLevel )
		swVisitLevel = cVisitLevel;
	if( sVisitLevel < wVisitLevel )
	{
		if( sVisitLevel < swVisitLevel )
			swVisitLevel = sVisitLevel;
	}
	else
	{
		if( wVisitLevel < swVisitLevel )
			swVisitLevel = wVisitLevel;
	}
	if( swVisitLevel >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && swVisitLevel < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 4;		// fog
		else
			retMaskId |= 2 << 4;		// fully explored
	}

	if( cVisitLevel < seVisitLevel )
		seVisitLevel = cVisitLevel;
	if( sVisitLevel < eVisitLevel )
	{
		if( sVisitLevel < seVisitLevel )
			seVisitLevel = sVisitLevel;
	}
	else
	{
		if( eVisitLevel < seVisitLevel )
			seVisitLevel = eVisitLevel;
	}
	if( seVisitLevel >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && seVisitLevel < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 6;		// fog
		else
			retMaskId |= 2 << 6;		// fully explored
	}

	return retMaskId;
}


BYTE ExploredMask::make_mask_id( int xLoc, int yLoc )
{
	BYTE retMaskId = 0;

	err_when( xLoc < 0 || yLoc < 0 || xLoc >= MAX_WORLD_X_LOC || yLoc >= MAX_WORLD_Y_LOC );

	BYTE cVisitLevel;
	BYTE nwMin, neMin, swMin, seMin;
	Location *cLocPtr;

	cVisitLevel = (cLocPtr = world.get_loc( xLoc, yLoc ))->visit_level;

	if( yLoc <= 0 )
	{
		Location *sLocPtr = world.get_loc( xLoc, yLoc+1);
		BYTE sVisitLevel = sLocPtr->visit_level;

		if( xLoc <= 0 )
		{
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;
			BYTE seVisitLevel = (sLocPtr+1)->visit_level;

			nwMin = cVisitLevel;
			neMin = min(cVisitLevel, eVisitLevel );
			swMin = min(cVisitLevel, sVisitLevel );
			seMin = min4(cVisitLevel, seVisitLevel, sVisitLevel, eVisitLevel);

		}
		else if( xLoc >= MAX_WORLD_X_LOC-1 )
		{
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;
			BYTE swVisitLevel = (sLocPtr-1)->visit_level;

			nwMin = min( cVisitLevel, wVisitLevel );
			neMin = cVisitLevel;
			swMin = min4( cVisitLevel, swVisitLevel, sVisitLevel, wVisitLevel );
			seMin = min( cVisitLevel, sVisitLevel );
		}
		else
		{
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;
			BYTE swVisitLevel = (sLocPtr-1)->visit_level;
			BYTE seVisitLevel = (sLocPtr+1)->visit_level;

			nwMin = min( cVisitLevel, wVisitLevel );
			neMin = min( cVisitLevel, eVisitLevel );
			swMin = min4( cVisitLevel, swVisitLevel, sVisitLevel, wVisitLevel );
			seMin = min4(cVisitLevel, seVisitLevel, sVisitLevel, eVisitLevel);
		}
	}
	else if( yLoc >= MAX_WORLD_Y_LOC-1 )
	{
		Location *nLocPtr = world.get_loc( xLoc, yLoc-1);
		BYTE nVisitLevel = nLocPtr->visit_level;

		if( xLoc <= 0 )
		{
			BYTE neVisitLevel = (nLocPtr+1)->visit_level;
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;

			nwMin = min( cVisitLevel, nVisitLevel );
			neMin = min4( cVisitLevel, neVisitLevel, nVisitLevel, eVisitLevel );
			swMin = cVisitLevel;
			seMin = min( cVisitLevel, eVisitLevel );
		}
		else if( xLoc >= MAX_WORLD_X_LOC-1 )
		{
			BYTE nwVisitLevel = (nLocPtr-1)->visit_level;
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;

			nwMin = min4( cVisitLevel, nwVisitLevel, nVisitLevel, wVisitLevel );
			neMin = min( cVisitLevel, nVisitLevel );
			swMin = min( cVisitLevel, wVisitLevel );
			seMin = cVisitLevel;
		}
		else
		{
			BYTE nwVisitLevel = (nLocPtr-1)->visit_level;
			BYTE neVisitLevel = (nLocPtr+1)->visit_level;
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;

			nwMin = min4( cVisitLevel, nwVisitLevel, nVisitLevel, wVisitLevel );
			neMin = min4( cVisitLevel, neVisitLevel, nVisitLevel, eVisitLevel);
			swMin = min( cVisitLevel, wVisitLevel );
			seMin = min( cVisitLevel, eVisitLevel );
		}
	}
	else
	{
		Location *nLocPtr = world.get_loc( xLoc, yLoc-1);
		Location *sLocPtr = world.get_loc( xLoc, yLoc+1);

		BYTE nVisitLevel = nLocPtr->visit_level;
		BYTE sVisitLevel = sLocPtr->visit_level;

		if( xLoc <= 0 )
		{
			BYTE neVisitLevel = (nLocPtr+1)->visit_level;
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;
			BYTE seVisitLevel = (sLocPtr+1)->visit_level;

			nwMin = min( cVisitLevel, nVisitLevel );
			neMin = min4( cVisitLevel, neVisitLevel, nVisitLevel, eVisitLevel );
			swMin = min( cVisitLevel, sVisitLevel );
			seMin = min4( cVisitLevel, seVisitLevel, sVisitLevel, eVisitLevel );
		}
		else if( xLoc >= MAX_WORLD_X_LOC-1 )
		{
			BYTE nwVisitLevel = (nLocPtr-1)->visit_level;
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;
			BYTE swVisitLevel = (sLocPtr-1)->visit_level;

			nwMin = min4( cVisitLevel, nwVisitLevel, nVisitLevel, wVisitLevel );
			neMin = min( cVisitLevel, nVisitLevel );
			swMin = min4( cVisitLevel, swVisitLevel, sVisitLevel, wVisitLevel );
			seMin = min( cVisitLevel, sVisitLevel );
		}
		else
		{
			BYTE nwVisitLevel = (nLocPtr-1)->visit_level;
			BYTE neVisitLevel = (nLocPtr+1)->visit_level;
			BYTE wVisitLevel = (cLocPtr-1)->visit_level;
			BYTE eVisitLevel = (cLocPtr+1)->visit_level;
			BYTE swVisitLevel = (sLocPtr-1)->visit_level;
			BYTE seVisitLevel = (sLocPtr+1)->visit_level;

			nwMin = min4( cVisitLevel, nwVisitLevel, nVisitLevel, wVisitLevel );
			neMin = min4( cVisitLevel, neVisitLevel, nVisitLevel, eVisitLevel );
			swMin = min4( cVisitLevel, swVisitLevel, sVisitLevel, wVisitLevel );
			seMin = min4( cVisitLevel, seVisitLevel, sVisitLevel, eVisitLevel );
		}
	}

	if( nwMin >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && nwMin < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1;		// fog
		else
			retMaskId |= 2;		// fully explored
	}
	if( neMin >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && neMin < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 2;		// fog
		else
			retMaskId |= 2 << 2;		// fully explored
	}
	if( swMin >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && swMin < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 4;		// fog
		else
			retMaskId |= 2 << 4;		// fully explored
	}
	if( seMin >= EXPLORED_VISIBILITY )
	{
		if( config.fog_of_war && seMin < FULL_VISIBILITY-FRAMES_PER_DAY-1 )
			retMaskId |= 1 << 6;		// fog
		else
			retMaskId |= 2 << 6;		// fully explored
	}

	return retMaskId;
}


BYTE ExploredMask::mask_id_to_serial_id( BYTE maskId )
{
	// convert from 0x00-0xff to 0-80 ( 3 to power 4)

	static BYTE table[0x100] = 
	{
		 0,  1,  2,  0,  3,  4,  5,  0,  6,  7,  8,  0,  0,  0,  0,  0, 
		 9, 10, 11,  0, 12, 13, 14,  0, 15, 16, 17,  0,  0,  0,  0,  0, 
		18, 19, 20,  0, 21, 22, 23,  0, 24, 25, 26,  0,  0,  0,  0,  0, 
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		27, 28, 29,  0, 30, 31, 32,  0, 33, 34, 35,  0,  0,  0,  0,  0,
		36, 37, 38,  0, 39, 40, 41,  0, 42, 43, 44,  0,  0,  0,  0,  0,
		45, 46, 47,  0, 48, 49, 50,  0, 51, 52, 53,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		54, 55, 56,  0, 57, 58, 59,  0, 60, 61, 62,  0,  0,  0,  0,  0,
		63, 64, 65,  0, 66, 67, 68,  0, 69, 70, 71,  0,  0,  0,  0,  0,
		72, 73, 74,  0, 75, 76, 77,  0, 78, 79, 80,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	};

	// except the first zero, other zero are invalid
	err_when( table[maskId] == 0 && maskId != 0 );
	return table[maskId];
}


