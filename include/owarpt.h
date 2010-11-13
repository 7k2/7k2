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

// Filename    : OWARPT.H
// Description : warning point on map

#ifndef __OWARPT_H
#define __OWARPT_H

//--------- define constant -----------//

#define WARPOINT_ZONE_SIZE 8
#define WARPOINT_ZONE_COLUMN ((MAX_MAP_WIDTH + WARPOINT_ZONE_SIZE -1) / WARPOINT_ZONE_SIZE)
#define WARPOINT_ZONE_ROW ((MAX_MAP_HEIGHT + WARPOINT_ZONE_SIZE -1) / WARPOINT_ZONE_SIZE)

//--------- struct Define WarPoint ----------//

typedef unsigned long DWORD;

struct WarPoint
{
	DWORD strength;

	void	inc();
	void	decay();
};

//------- define class WarPointArray --------//

class WarPointArray
{
public:
	WarPoint *war_point;
	int		active_zone_count;
	char		init_flag;
	char		draw_phase;

	WarPointArray();
	~WarPointArray();

	void	init();
	void	deinit();

	void	draw_dot();
	void	process();

	WarPoint *get_ptr(int xLoc, int yLoc);
	void	add_point(int xLoc, int yLoc);
};

extern WarPointArray war_point_array;

#endif