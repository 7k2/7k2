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

// Filename    : OLINEAR.H
// Description : Header file of Linear increment / decrement


#ifndef __OLINEAR_H
#define __OLINEAR_H

class LinearCounter
{
public:
	int	dx;
	int	dy;
	int	x_dest;

	int	x;
	int	y;
	int	d;
	int	q;
	int	r;

public:
	LinearCounter();
	LinearCounter(int x0, int y0, int x1, int y1 );

	void	init(int x0, int y0, int x1, int y1 );
	int	is_end();
	void	inc();
};

#endif