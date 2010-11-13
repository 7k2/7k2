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

// Filename    : OW_TRV.H
// Description : header file of world traverse, find units in range


#ifndef __OW_TRV_H
#define __OW_TRV_H

class BaseObj;

struct ScannedObj
{
	short unit_recno;
	short	sort_weight;
};

class WorldScanTraverser
{
	enum { DEFAULT_ARRAY_SIZE = 64 };

public:
	ScannedObj *unit_recno_array;
	int	traverse_count;		// run from 0 to scan_count-1
	int	scan_count;
	int	alloc_count;			// allocated size

private:
	ScannedObj default_recno_array[DEFAULT_ARRAY_SIZE];

public:
	WorldScanTraverser();
	~WorldScanTraverser();

	void	init( int xLoc, int yLoc, int locSize, int radius );
	int	is_finish();
	int	get_unit();
	void	next();

	// -------- for 2nd pass filtering ----------//

	void	restart();
	void	remove();
	void	pack_removed();

	// -------- 3rd pass setting weight function ---------//

	short	get_weight();
	void	set_weight(short);
	
	// -------- 4rd pass sorting ----------//

	void	sort_by_distance( int xLoc, int yLoc, int locSize );
	void	sort_by_distance( BaseObj *);
	void	sort_by_weight();

private:
	void	append(short unitRecno);
};

#endif
