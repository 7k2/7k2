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

#include <math.h>
#include <cstdlib>
#include <oisoarea.h>
#include <omisc.h>
#include <all.h>
#include <oworld.h>

//--------- Begin of function IsolateArea::IsolateArea ---------//
IsolateArea::IsolateArea()
{
	init();
}
//----------- End of function IsolateArea::IsolateArea -----------//


//--------- Begin of function IsolateArea::~IsolateArea ---------//
IsolateArea::~IsolateArea()
{
	deinit();
}
//----------- End of function IsolateArea::~IsolateArea -----------//


//--------- Begin of function IsolateArea::init ---------//
void IsolateArea::init()
{
	reset_area_info(&same_region);
	reset_area_info(&diff_region);

	regions_line_dir		= 0;
	regions_line_x_loc1	= -1;
	regions_line_y_loc1	= -1;
	regions_line_x_loc2	= -1;
	regions_line_y_loc2	= -1;
	region_check_x_offset = 0;
	region_check_y_offset = 0;
}
//----------- End of function IsolateArea::init -----------//


//--------- Begin of function IsolateArea::deinit ---------//
void IsolateArea::deinit()
{
}
//----------- End of function IsolateArea::deinit -----------//


//--------- Begin of function IsolateArea::reset_area_info ---------//
void IsolateArea::reset_area_info(AreaInfo *infoPtr)
{
	infoPtr->distance = 0x7FFF;
	infoPtr->with_data = 0;
	infoPtr->x_loc = -1;
	infoPtr->y_loc = -1;
}
//----------- End of function IsolateArea::reset_area_info -----------//


//--------- Begin of function IsolateArea::cal_direction ---------//
char IsolateArea::cal_direction(short sx, short sy, short dx, short dy)
{
	if(dx==sx)
	{
		if(dy>sy)
			return DIR_S;
		else
			return DIR_N;
	}
	else if(dx<sx)
	{
		if(dy==sy)
			return DIR_W;
		else if(dy>sy)
			return DIR_SW;
		else
			return DIR_NW;
	}
	else
	{
		if(dy==sy)
			return DIR_E;
		else if(dy>sy)
			return DIR_SE;
		else
			return DIR_NE;
	}

	return DIR_N;
}
//----------- End of function IsolateArea::cal_direction -----------//


//--------- Begin of function IsolateArea::to_regions_line_left ---------//
int IsolateArea::to_regions_line_left(char dir, int withVerticalCheck)
{
	switch(regions_line_dir)
	{
		case DIR_N:
				if(withVerticalCheck)
					return dir==DIR_N || dir==DIR_S || dir==DIR_SW || dir==DIR_W || dir==DIR_NW;
				else
					return dir==DIR_SW || dir==DIR_W || dir==DIR_NW;

		case DIR_NE:
				if(withVerticalCheck)
					return dir==DIR_NE || dir==DIR_SW || dir==DIR_W || dir==DIR_NW || dir==DIR_N;
				else
					return dir==DIR_W || dir==DIR_NW || dir==DIR_N;
				
		case DIR_E:
				if(withVerticalCheck)
					return dir==DIR_E || dir==DIR_W || dir==DIR_NW || dir==DIR_N || dir==DIR_NE;
				else
					return dir==DIR_NW || dir==DIR_N || dir==DIR_NE;

		case DIR_SE:
				if(withVerticalCheck)
					return dir==DIR_SE || dir==DIR_NW || dir==DIR_N || dir==DIR_NE || dir==DIR_E;
				else
					return dir==DIR_N || dir==DIR_NE || dir==DIR_E;

		case DIR_S:
				if(withVerticalCheck)
					return dir==DIR_S || dir==DIR_N || dir==DIR_NE || dir==DIR_E || dir==DIR_SE;
				else
					return dir==DIR_NE || dir==DIR_E || dir==DIR_SE;

		case DIR_SW:
				if(withVerticalCheck)
					return dir==DIR_SW || dir==DIR_NE || dir==DIR_E || dir==DIR_SE || dir==DIR_S;
				else
					return dir==DIR_E || dir==DIR_SE || dir==DIR_S;

		case DIR_W:
				if(withVerticalCheck)
					return dir==DIR_W || dir==DIR_E || dir==DIR_SE || dir==DIR_S || dir==DIR_SW;
				else
					return dir==DIR_SE || dir==DIR_S || dir==DIR_SW;

		case DIR_NW:
				if(withVerticalCheck)
					return dir==DIR_NW || dir==DIR_SE || dir==DIR_S || dir==DIR_SW || dir==DIR_W;
				else
					return dir==DIR_S || dir==DIR_SW || dir==DIR_W;
	}

	return 0;
}
//----------- End of function IsolateArea::to_regions_line_left -----------//


//--------- Begin of function IsolateArea::detect_isolate_area ---------//
// 1) used to detect reachable location for attacking unit and the location
//		checked should be the surrounding location of the unit
//
//-------------- not yet implemented ---------------//
//	2) movement
// 3) detect target to attack while the target is in isolated area. i.e,
//		the attacker should ignore this unit because of unreachable
//
int IsolateArea::detect_isolate_area(short refXLoc, short refYLoc, short spaceXLoc, short spaceYLoc)
{
	//--------- initialize private data member ---------//
	deinit();
	init();

	short dist = m.points_distance(refXLoc, refYLoc, spaceXLoc, spaceYLoc);
	short startXLoc1 = refXLoc;
	short startYLoc1 = refYLoc;
	short startXLoc2 = spaceXLoc;
	short startYLoc2 = spaceYLoc;

	if(dist>1)
	{
		//------------- adjust location for checking -----------//
		//-************************ codes here ******************************-//
		//... startXLoc1= .....//
	}

	err_when(m.points_distance(startXLoc1, startYLoc1, startXLoc2, startYLoc2)!=1);

	//--------- store useful information ----------//
	regions_line_x_loc1 = startXLoc1;
	regions_line_y_loc1 = startYLoc1;
	regions_line_x_loc2 = startXLoc2;
	regions_line_y_loc2 = startYLoc2;
	region_check_x_offset = startXLoc1-startXLoc2;
	region_check_y_offset = startYLoc1-startYLoc2;
	regions_line_dir = cal_direction(startXLoc1, startYLoc1, startXLoc2, startYLoc2);

	//--------------------------------------------------------------------------//
	// Note: first step checking is always to regions_line left hand side
	//--------------------------------------------------------------------------//
	//---------- initialize parameters -------------//
	char	processDir = regions_line_dir;
	short processXLoc = startXLoc2; // use as reference variable
	short processYLoc = startYLoc2; // use as reference variable
	#ifdef DEBUG
		int count=0;
	#endif

	process_move_around_boundary(processDir, processXLoc, processYLoc);
	if(processXLoc!=startXLoc2 || processYLoc!=startYLoc2)
	{
		//---------------------------------------------------------------//
		// more than one step
		//---------------------------------------------------------------//
		int firstStepXLoc = processXLoc;
		int firstStepYLoc = processYLoc;
		int cycle = 0;

		while(1)
		{
			#ifdef DEBUG
				err_when(count>20000);
				count++;
			#endif

			process_move_around_boundary(processDir, processXLoc, processYLoc);
			
			//-----------------------------------------------------------------------------//
			// consider the shap of 8 (turning 90 degree) and the (startXLoc2, startYLoc2)
			// is the touching point of the two circles.
			//-----------------------------------------------------------------------------//
			if(!cycle)
			{
				if(processXLoc==startXLoc2 && processYLoc==startYLoc2)
					cycle++;
			}
			else if(processXLoc==firstStepXLoc && processYLoc==firstStepYLoc)
				break;
			else
				cycle--;

			err_when(cycle<0 || cycle>1);
		}
	}
	#ifdef DEBUG
		int finalResult = is_isolate_area();
		return finalResult;
	#else
		return is_isolate_area();
	#endif
}
//----------- End of function IsolateArea::detect_isolate_area -----------//


//--------- Begin of function IsolateArea::process_move_around_boundary ---------//
// <char&>	dir	- the direction from prvious location to (xLoc, yLoc)
//	<short&>	xLoc	- current checking x location
//	<short&>	yLoc	- current checking y location
//
void IsolateArea::process_move_around_boundary(char& dir, short& xLoc,short& yLoc)
{
	//------------------------------------------------------------------------------//
	// process checking from left to right corresponding to the parameter dir
	//
	// There is 8 possible value for dir, but the checking offset can be summarized
	// in the xOffset and yOffset array.  The starting point in these arrays are
	// adjusted corresponding to each value of dir.
	//
	// Once the first offset is used, the rest offset can be calculated faster by using
	// xCorrOffset and yCorrOffset arrays.
	//------------------------------------------------------------------------------//
	#define DIR_NUM		8
	static int xOffset[DIR_NUM] = {-1, -1, -1,  0,  1,  1,  1,  0}; // correspoinding to DIR_N, i.e. 0
	static int yOffset[DIR_NUM] = { 1,  0, -1, -1, -1,  0,  1,  1}; // ditto
	
	static int xCorrOffset[DIR_NUM] = {-1,  0,  0,  1,  1,  0,  0, -1}; // corresponding offset from the previous offset in xOffset
	static int yCorrOffset[DIR_NUM] = { 0, -1, -1,  0,  0,  1,  1,  0}; // ditto

	Location *locPtr;
	short	dist;
	int	dirCount = dir, canMove = 0;
	short	xDiff, yDiff;
	char	testDir;

	short checkXLoc = xLoc + xOffset[dirCount]; // use xOffset[] for first step, and xCorrOffset for the rest
	short checkYLoc = yLoc + yOffset[dirCount]; // ditto, (since A+=B is faster than A=B+C)

	for(int i=0; i<8; i++, checkXLoc+=xCorrOffset[dirCount], checkYLoc+=yCorrOffset[dirCount])
	{
		if(++dirCount>=DIR_NUM) // plus one for next processing
		{
			dirCount -= DIR_NUM;
			err_when(dirCount<0 || dirCount>=DIR_NUM);
		}

		if(checkXLoc<0 || checkXLoc>=MAX_WORLD_X_LOC || checkYLoc<0 || checkYLoc>=MAX_WORLD_Y_LOC)
			continue;

		locPtr = world.get_loc(checkXLoc, checkYLoc);
		//----- check for UNIT_LAND only, improve later -------//
		if(!locPtr->walkable() || locPtr->cargo_recno)	//******** BUGHERE
			continue;

		//----------------------------------------//
		// next location found
		//----------------------------------------//
		dir = cal_direction(xLoc, yLoc, checkXLoc, checkYLoc);
		xLoc = checkXLoc;
		yLoc = checkYLoc;
		canMove++;
		break;
	}

	if(!canMove)
	{
		//-------- cannot move ---------//
		err_when(m.points_distance(xLoc, yLoc, regions_line_x_loc1, regions_line_y_loc1)>1);
		same_region.distance = 1;
		same_region.with_data++;
		same_region.x_loc = xLoc;
		same_region.y_loc = yLoc;

		dir = (regions_line_dir+6)%DIR_NUM;	// exit the loop
		return;
	}

	//------------------------------------------------------//
	// update information in same_region and diff_region
	//------------------------------------------------------//
	
	if(dir%2) // x case
	{
		xDiff = abs(regions_line_x_loc1-checkXLoc);
		yDiff = abs(regions_line_y_loc1-checkYLoc);
		if(xDiff%2 || yDiff%2)
			checkYLoc--;	// make some adjustment for checkYLoc
	}

	if(!xDiff || !yDiff || xDiff==yDiff)
	{
		testDir = cal_direction(regions_line_x_loc1, regions_line_y_loc1, checkXLoc, checkYLoc);

		if(testDir==regions_line_dir)
		{
			//------ on same region -------//
			if(to_regions_line_left(dir))
			{
				if(same_region.with_data && (xLoc!=regions_line_x_loc2 || yLoc!=regions_line_y_loc2))
					reset_area_info(&same_region);
			}
			else
			{		
				dist = m.points_distance(checkXLoc, checkYLoc, regions_line_x_loc1, regions_line_y_loc1);
				if(dist<same_region.distance)
				{
					same_region.distance = dist;
					same_region.with_data++;
					same_region.x_loc = checkXLoc;
					same_region.y_loc = checkYLoc;
				}
			}
		}
		else if((testDir+4)%DIR_NUM==regions_line_dir)
		{
			//----- on different region -----//
			if(to_regions_line_left(dir))
			{
				if(diff_region.with_data && (xLoc!=regions_line_x_loc2 || yLoc!=regions_line_y_loc2))
					reset_area_info(&diff_region);
			}
			else
			{
				dist = m.points_distance(checkXLoc, checkYLoc, regions_line_x_loc1, regions_line_y_loc1);
				if(dist<diff_region.distance)
				{
					diff_region.distance = dist;
					diff_region.with_data++;
					diff_region.x_loc = checkXLoc;
					diff_region.y_loc = checkYLoc;
				}
			}
		}
	}
}
//----------- End of function IsolateArea::process_move_around_boundary -----------//


//--------- Begin of function IsolateArea::is_isolate_area ---------//
// return 1 if area is Isolate from outside
// return 0 otherwise
//
int IsolateArea::is_isolate_area()
{
	return !diff_region.with_data;
}
//----------- End of function IsolateArea::is_isolate_area -----------//
