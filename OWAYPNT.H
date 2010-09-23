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