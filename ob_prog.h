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

// Filename    : OB_PROG.H
// Description : header file for BulletProgram

#ifndef __OB_PROG_H
#define __OB_PROG_H

#include <OBULLET.H>

class BulletProgram : public Bullet
{
public:
	int delay_step;

public:
	BulletProgram();
	~BulletProgram();
	void 	init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType);
	void 	process_move();
	int	process_die();
	char  display_layer();
	void	draw(int outLine, int drawFlag);
	void  update_abs_pos();
	void  cur_sprite_frame();
	int	reflect( int baseObjRecno ) { return 0;}

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();
};

#endif