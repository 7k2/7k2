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

// Filename    : OEXPMASK.H
// Description : Header file of explored area mask
// Owner       : Gilbert

#ifndef __OEXPMASK_H
#define __OEXPMASK_H

struct Location;
class ColorTable;

typedef unsigned char BYTE;

#define FULL_DARK_MASK_ID 0x00
#define FULL_BRIGHT_MASK_ID 0xaa
#define HALF_BRIGHT_MASK_ID 0x55

class ExploredMask
{
public:
	char *remap_bitmap;

public:
	void init();
	void deinit();

	char *get_remap_mask( BYTE maskId );

	static BYTE make_mask_id( BYTE nwVisitLevel, BYTE nVisitLevel, BYTE neVisitLevel,
		BYTE wVisitLevel, BYTE cVisitLevel, BYTE eVisitLevel,
		BYTE swVisitLevel, BYTE sVisitLevel, BYTE seVisitLevel );

	static BYTE make_mask_id( int xLoc, int yLoc );

private:
	static BYTE mask_id_to_serial_id( BYTE maskId );
};

extern ExploredMask explored_mask;

#endif