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

//Filename    : OWAYPNT.H
//Description : Header file of Object WayPoint

#ifndef __OWAYPNT_H
#define __OWAYPNT_H

#ifndef __ODYNARR_H
#include <ODYNARR.H>
#endif

#ifndef __OUN_GRP_H
#include <OUN_GRP.H>
#endif

//------ Define struct OneWayPoint ------//

struct OneWayPoint
{
	short	loc_x;
	short loc_y;
};


//------- Define class WayPoint ------//

class File;

class WayPoint : public DynArray
{
public:
	UnitGroup 	cur_unit_group;

	// if there are any other new member please update write_file and read_file

public:
	WayPoint();

	int  	process();

	void 	add(int xLoc, int yLoc);
	void 	del(int delRecno);

	void 	draw(int drawMode);

	OneWayPoint* get(int recNo);

	int	write_file(File *filePtr);
	int	read_file(File *filePtr);
};

//------- Define class WayPointArray ------//

class WayPointArray : public DynArray
{
public:
	WayPointArray();

	void			init();
   void			deinit();
	void 			process();
	void			add_point(int xLoc, int yLoc);
	void 			draw(int drawMode);

	int			write_file(File *filePtr);
	int			read_file(File *filePtr);

private:
	int 			add();
	void 			del(int delRecno);
	WayPoint* 	get(int recNo);
};

//------------------------------------//

extern	WayPointArray	way_point_array;

#endif