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

//Filename		:	opreuse.cpp
//Description	:	Implementation of class PathReuse
//Owner			:	Juliet
 
#include "windows.h"
#include "all.h"
#include "oterrain.h"
#include "opreuse.h"
#include "math.h"
#include "ounit.h"
#include "oun_grp.h"
#include "opfind.h"
#include "onation.h"
 
//#define MAX_REUSE_NUMBER_OF_UNITS	100

//Define static members
static int leader_start_x;
static int leader_start_y;
static int leader_dest_x;
static int leader_dest_y;
static char mobile_type;
bool PathReuse::leader_path_found;
short PathReuse::being_used = 0;

static SaveLocation* save_loc_buf;

static char handle_power_nation_flag = 0;
static char nation_passable[MAX_NATION+1] = {0};
static int nation_recno;

//------ declare static functions --------//

static int is_location_walkable(int xLoc, int yLoc);
//------ Begin of static function is_location_walkable ------//

static int is_location_walkable(int xLoc, int yLoc)
{
	err_when(xLoc < 0);
	err_when(xLoc >= MAX_WORLD_X_LOC);
	err_when(yLoc < 0);
	err_when(yLoc >= MAX_WORLD_Y_LOC);

	Location* locPtr = world.get_loc(xLoc, yLoc);

	if( handle_power_nation_flag )
	{
		int powerNationRecno=locPtr->power_nation_recno;

		if( powerNationRecno && !nation_passable[powerNationRecno] )
			return 0;
	}

	return locPtr->can_move(mobile_type);
}
//------ End of static function is_location_walkable ------//

//-----------Begin of function PathReuse::PathReuse--------------//

PathReuse::PathReuse()
{
	the_units = (Unit**) mem_add(MAX_REUSE_NUMBER_OF_UNITS*sizeof(Unit*));
	int rnUnits = sizeof(ReuseNode*)*MAX_REUSE_NUMBER_OF_UNITS;
	cur_paths = (ReuseNode**) mem_add(rnUnits);
	cur_paths_head = (ReuseNode**) mem_add(rnUnits);
	size_x = (short*) mem_add(sizeof(short)*MAX_REUSE_NUMBER_OF_UNITS);
	size_y = (short*) mem_add(sizeof(short)*MAX_REUSE_NUMBER_OF_UNITS);
	rnUnits *= MAX_REUSE_NODE_NUM_PER_UNIT;
	reuse_node_arr = (ReuseNode*) mem_add(rnUnits);
	save_loc_buf = (SaveLocation*) mem_add(sizeof(SaveLocation) * PATH_FIND_LOC_BUFFER_SIZE );

	err_when(!the_units);
	err_when(!cur_paths);
	err_when(!cur_paths_head);
	err_when(!size_x);
	err_when(!size_y);
	err_when(!reuse_node_arr);
}
//-----------End of function PathReuse::PathReuse ----------------//


//-----------Begin of function PathReuse::~PathReuse------------//
PathReuse::~PathReuse()
{
	//check whether the basic variable are valid and the corresponding flag are turn on, if yes
	//deleled memory of the basic variable
	
	if (the_units) 
		mem_del(the_units);	 
	if (cur_paths)
	   mem_del(cur_paths);
	if(size_x)
		mem_del(size_x);
	if(size_y)
		mem_del(size_y);
	if(reuse_node_arr)
		mem_del(reuse_node_arr);
	if(cur_paths_head)
		mem_del(cur_paths_head);
	if(save_loc_buf)
		mem_del(save_loc_buf);
}
//----------End of function PathReuse~PathReuse--------------//


//-----------Begin of function PathReuse::init--------------//
//initializing variables before the class function is used.
//this is called by UnitGroup::cluster_unit_for_path_reuse

void PathReuse::init(UnitGroup* unitGroup)
{
	err_when( unitGroup->size()==0 );

	n_units = unitGroup->size(); //size of the group

//	err_when(n_units >= MAX_REUSE_NUMBER_OF_UNITS);
	err_when(n_units > MAX_REUSE_NUMBER_OF_UNITS);

	int i;

	mobile_type=0;

	//------ copy units from UnitGroup to class vars ----//

	for(i=1; i<=n_units; i++)
	{
		the_units[i-1] = unitGroup->get_unit(i);

		//--- set the mobile type of the unit group ---//

		int mobileType = unitGroup->get_unit(i)->mobile_type;

		if( mobile_type==0 )
		{
			mobile_type = mobileType;
		}
		else		// make sure all units in the group are of the same mobile type.
		{
			err_when( mobileType != mobile_type );
		}
	}

//	del_it = true;
	reset();
}
//-----------End of function PathReuse::init ----------------//

//-----------Begin of function PathReuse::init--------------//
//initializing variables before the class is used.

void PathReuse::init(Unit** units, int nUnits)
{
	err_when( nUnits == 0 );

	n_units = nUnits;

//	err_when(n_units >= MAX_REUSE_NUMBER_OF_UNITS);
	err_when(n_units > MAX_REUSE_NUMBER_OF_UNITS);

	
	int i;
	for(i = 0; i<n_units; i++)
		the_units[i] = units[i];
	
	//--- set the mobile type of the unit group ---//

	mobile_type = units[0]->mobile_type;

	reset();
}
//-----------End of function PathReuse::init ----------------//

//-----------Begin of function PathReuse::reset--------------//
//reset variables everytime path_reuse is used

void PathReuse::reset()
{
	int rnUnits = sizeof(ReuseNode*)*MAX_REUSE_NUMBER_OF_UNITS;
	::ZeroMemory(cur_paths, rnUnits); //set the memory allocated to zero
	::ZeroMemory(cur_paths_head, rnUnits); //and the set these memory to zero
	rnUnits*=MAX_REUSE_NODE_NUM_PER_UNIT;
	::ZeroMemory(reuse_node_arr, rnUnits); //set the memory to zero
	reuse_node_count = 0; //initialize the reuse_node_count to zero
}
//-----------End of function PathReuse::reset ----------------//

//-----------Begin of function PathReuse::set_handle_power_nation -----//
//set the flags to determine whether handle_power_nation is used.

void PathReuse::set_handle_power_nation(int handlePowerFlag, char* nationPassable, int nationRecno)
{
	handle_power_nation_flag = handlePowerFlag;

	if(nationPassable)
		memcpy(nation_passable+1, nationPassable, sizeof(char)*MAX_NATION);

	nation_recno = nationRecno;
}
//----------End of function PathReuse::set_handle_power_nation----------//

//----------Begin of function PathReuse::find_paths----------//
//main function called by unit group to use path reuse to find path.

void PathReuse::find_paths()
{
	//------------Define local variables------------//
	int sx, sy, dx, dy;
	int i;
	int pathNodeCount;
	PathResult* thePath;
	int theStepsRemaining;
	short is, js;
	int locX, locY;
	int temperoryLocationX, temperoryLocationY;
	int* oldLocX, *oldLocY;

	oldLocX = (int*)mem_add(n_units*sizeof(int));
	oldLocY = (int*)mem_add(n_units*sizeof(int));

	//de-occupy the units of the troop before finding the paths
	//since the location are actually no longer occupied once the troop is moving
	//the occupancy will only make pathfinding impossibles
	//since the troop members always obscure on the path
	err_when(being_used);
	being_used = 1;
	for(i=0; i<n_units; i++)	
	{
		locX = the_units[i]->next_x_loc();
		locY = the_units[i]->next_y_loc();
		oldLocX[i] = locX;
		oldLocY[i] = locY;
		size_x[i] = the_units[i]->obj_loc_width(); //get the size of the unit
		size_y[i] = the_units[i]->obj_loc_height(); //get the size of the unit
		for(is=0, temperoryLocationX = locX; is<size_x[i]; is++, temperoryLocationX++) 
		{
			for(js=0, temperoryLocationY = locY; js<size_y[i]; js++, temperoryLocationY++)
				world.get_loc(temperoryLocationX, temperoryLocationY)->remove_unit(mobile_type); //set occupied state to false
		}
	}

	leader_start_x = the_units[0]->next_x_loc();
	leader_start_y = the_units[0]->next_y_loc();
	leader_dest_x = the_units[0]->move_to_loc_x;
	leader_dest_y = the_units[0]->move_to_loc_y;

	//searching for the leader path
	find_leader_path(leader_start_x, leader_start_y, leader_dest_x , leader_dest_y);
	
	// note that the leader path, cur_paths[0] and hence leader_path, has already been "compressed"
	// because of this, you might not be able to check the occupancy of offset of nodes "removed" from the leader path

	int leaderDifferenceX = leader_dest_x - leader_start_x;
	int leaderDifferenceY = leader_dest_y - leader_start_y;
	int maxRightOutOfRange = 0, maxBottomOutOfRange = 0, maxLeftOutOfRange = 0, maxTopOutOfRange = 0;
	int diff;
	
	//check out of range
	for(i = 1; i < n_units; i++)
	{
		sx = the_units[i]->next_x_loc();
		sy = the_units[i]->next_y_loc();
		dx = leaderDifferenceX + sx;
		dy = leaderDifferenceY + sy;
		if((diff=dx-(MAX_WORLD_X_LOC-1)) >= maxRightOutOfRange)
			maxRightOutOfRange = diff;
		if(dx < maxLeftOutOfRange)
			maxLeftOutOfRange = dx;
		if((diff=dy-(MAX_WORLD_Y_LOC-1)) >= maxBottomOutOfRange)
			maxBottomOutOfRange = diff;
		if(dy < maxTopOutOfRange)
			maxTopOutOfRange = dy;
	}
	if(maxRightOutOfRange)
	{
		leader_dest_x -= maxRightOutOfRange;
		the_units[0]->move_to_loc_x = leader_dest_x;
		leaderDifferenceX = leader_dest_x - leader_start_x;
	}
	if(maxBottomOutOfRange)
	{
		leader_dest_y -= maxBottomOutOfRange;
		the_units[0]->move_to_loc_y = leader_dest_y;
		leaderDifferenceY = leader_dest_y - leader_start_y;
	}
	if(maxLeftOutOfRange)
	{
		leader_dest_x -= maxLeftOutOfRange;
		the_units[0]->move_to_loc_x = leader_dest_x;
		leaderDifferenceX = leader_dest_x - leader_start_x;
	}
	if(maxTopOutOfRange)
	{
		leader_dest_y -= maxTopOutOfRange;
		the_units[0]->move_to_loc_y = leader_dest_y;
		leaderDifferenceY = leader_dest_y - leader_start_y;
	}
	for(i = 1; i < n_units; i++)
	{
		sx = the_units[i]->next_x_loc(); //get the location of the unit
		sy = the_units[i]->next_y_loc();
		err_when(sx <0);
		err_when(sy <0);
		err_when(sx>=MAX_WORLD_X_LOC);
		err_when(sy>=MAX_WORLD_Y_LOC);

		
		//setting the destination of the unit to be of the same offset as the source location
		//this can make the troop try their best to maintain their troop shape
		dx = leaderDifferenceX + sx ; 
		dy = leaderDifferenceY + sy ;

		//correct the destination of the unit
		the_units[i]->move_to_loc_x = dx; 
		the_units[i]->move_to_loc_y = dy;
		err_when(dx <0);
		err_when(dy <0);
		err_when(dx>=MAX_WORLD_X_LOC);
		err_when(dy>=MAX_WORLD_Y_LOC);


		//finding the path of the unit other than the leader path
		find_other_path(sx, sy, dx, dy, i);
		//remove those paths that lead to duplicate nodes
		remove_path_to_duplicate_nodes(i);
		//smoothing the path
		smoothing_path(i);
		
		//call a second time since there may be duplicate nodes after smoothing the path.
		remove_path_to_duplicate_nodes(i);
		
		//retrieve the path and set variables for blocking.
		if(cur_paths[i] && (abs(cur_paths[i]->x-dx) <= size_x[i] 
			&& abs(cur_paths[i]->y-dy) <= size_y[i]))
			the_units[i]->set_no_longer_blocked();
		else
			the_units[i]->set_not_seriously_blocked();

		thePath = retrieve(pathNodeCount, theStepsRemaining, i);

		//delete old path and set variables for new path
		the_units[i]->del_old_path();
		the_units[i]->set_path(thePath);
		the_units[i]->set_cur_path_result_id(pathNodeCount);
		the_units[i]->set_steps_remaining(theStepsRemaining);
	}

	//retrieve and set up variable for the leader path
	thePath=retrieve(pathNodeCount, theStepsRemaining, 0);
	the_units[0]->del_old_path();
	the_units[0]->set_path(thePath);
	the_units[0]->set_cur_path_result_id(pathNodeCount);
	the_units[0]->set_steps_remaining(theStepsRemaining);

	//setting the troops occupation to true after pathfinding is achieved
	for(i=0; i<n_units; i++)	
	{
		for(is=0, temperoryLocationX = oldLocX[i]; is<size_x[i]; is++, temperoryLocationX++) 
		{
			for(js=0, temperoryLocationY = oldLocY[i]; js<size_y[i]; js++, temperoryLocationY++)
				world.get_loc(temperoryLocationX, temperoryLocationY)->set_unit(the_units[i]->sprite_recno, mobile_type); //set occupied state to false
		}
	}
	
	mem_del(oldLocX);
	mem_del(oldLocY);
	being_used = 0;
}
//--------------End of function PathReuse::find_paths------------------//

//--------------Begin of function PathReuse::set_offset---------------//
//
// This Function calculate the offset, that is, the relative position from the leader
// of all the units, so that their offset path can be found accordingly
//
// <int> sx - the x-coordinate of the source location
// <int> sy - the y-coordinate of the source location
// <int> dx - the x-coordinate of the destination
// <int> dy - the y-coordinate of the destination
// <int> i - the identity of the unit in the array
//
// return: <void> - there is no return variable for void
void PathReuse::set_offset(int sx, int sy, int dx, int dy, int i)
{
	if (!i) //I am the leader !!! 
	{
		start_offset_x = 0;
		start_offset_y = 0;
		dest_offset_x = 0;
		dest_offset_y = 0;
	}
	else
	{
		start_offset_x = sx - leader_start_x;
		start_offset_y = sy - leader_start_y;
		dest_offset_x = dx - leader_dest_x;
		dest_offset_y = dy - leader_dest_y;
	}
}
//--------------End of function PathReuse::set_offset------------------//

//--------------Begin of function PathReuse::FindLeaderPath------------//
// This function finds the leader path by means of path finding algorithm
// the detail of the pathfinding algorithm can be seen in the file pathfind.cpp and pathfind.h
//
// <int> sx - the x-coordinate of the source location
// <int> sy - the y-coordinate of the source location
// <int> dx - the x-coordinate of the destination
// <int> dy - the y-coordinate of the destination
//
// return: <bool> - true if the leader path is found, false if the leader path is not found
bool PathReuse::find_leader_path(int sx, int sy, int dx, int dy)
{
	//------ Define local variables--------//
	bool found;
	PathNode* node, *parent;
	ReuseNode* currentNode, *temperoryNode;
	int RangeX1, RangeX2, RangeY1, RangeY2;

	//setting the offset of the leader before finding its path
	set_offset(sx, sy, dx, dy, 0);
	the_units[0]->get_object_range(dx, dy, RangeX1, RangeY1, RangeX2, RangeY2);

	//use pathfinder to find the path of the leader
	path_finder.set_attribute(size_x[0], size_y[0], mobile_type, PFIND_IGNORE_UNIT);
	if(handle_power_nation_flag)
	{
		err_when(!nation_recno);
		Nation* nationPtr = nation_array[nation_recno];
		path_finder.set_handle_power_nation(handle_power_nation_flag, nationPtr->relation_passable_array);
	}
	else
		path_finder.set_handle_power_nation(0);

	if(found = path_finder.find_path(sx, sy, dx, dy, RangeX1, RangeY1, RangeX2, RangeY2, false))
	{
		node = path_finder.return_path_node();
	}
	else
	{
		//if the leader path cannot be found, the closest node to the destination should be used.
		node = path_finder.get_closest_node();
		the_units[0]->set_not_seriously_blocked();
		found = true;
	}
	if(!(node && node->parent) && handle_power_nation_flag)
	{
		handle_power_nation_flag = false;
		path_finder.set_handle_power_nation(0);
		if(found = path_finder.find_path(sx, sy, dx, dy, RangeX1, RangeY1, RangeX2, RangeY2, false))
			node = path_finder.return_path_node();
		else
		{
			node = path_finder.get_closest_node();
			the_units[0]->set_not_seriously_blocked();
			found = true;
		}
	}
	//this copies the path from the data structure of PathFinder to that of PathReuse
	if(node && node->parent) 
	{
		currentNode = reuse_node_arr + reuse_node_count++;
		currentNode->x = node->x;
		currentNode->y = node->y;	
		cur_paths[0] = currentNode; //cur_paths points to the destination of the path
					
		while(parent=node->parent)
		{
			node = parent; 
			temperoryNode = reuse_node_arr + reuse_node_count++;
			currentNode->parent = temperoryNode;
			temperoryNode->next = currentNode;
			currentNode = temperoryNode;
			currentNode->x = node->x;
			currentNode->y = node->y;
		}
		leader_path = currentNode; //leader_path points to the source of the path
		cur_paths_head[0] = leader_path; //cur_paths_head also points to the souurce of the path
		err_when(!currentNode); //as the source node should always be in the path, there should always
		//be a valid current node present, if it is not, it is an error.
	}
	else
	{
		//if even the closest cannot be found, we have to set all these pointer to NULL
		leader_path = NULL; 
		cur_paths[0] = NULL; 
		cur_paths_head[0] = NULL; 
		found = false; 
	}

	leader_path_found = found;
	return found;
}
//--------------End of function PathReuse::FindLeaderPath-------------//

//--------------Begin of of function PathReuse::FindOtherPath---------//	
// This function finds the paths of other units other than the leader
// there are three choices of the methods used
// The offset path method is used if the source offset is exactly the same as the
// destination offset, this is always true for troop, since I have set this. but
// more generally this may not be true. Another method is the join offset path method.
// The method is used whenever the source offset is not the same as the destination offset,
// and this actually seldom happen.
// Another method is the normal path finding, this should be done whenever the leader
// cannot be found, since the offset path relative to the leader path cannot be calculated
// by using the leader path, and each unit will then call pathfinding individually
//
// <int> sx - the x-coordinate of the source location
// <int> sy - the y-coordinate of the source location
// <int> dx - the x-coordinate of the destination
// <int> dy - the y-coordinate of the destination
// <int> i - the identity of the unit in the array
//
// return: <bool> - true if the path is found, false if the path is not found
bool PathReuse::find_other_path(int sx, int sy, int dx, int dy, int i)
{
	//setting the offset of the unit before doing path searching
	set_offset(sx, sy, dx, dy, i);
	
	if(leader_path_found)
	{
		if(start_offset_x == dest_offset_x && start_offset_y == dest_offset_y)
		{			
			offset_path(sx, sy, dx, dy, i);			
		}
		else 
		{
			join_offset_path(sx, sy, dx, dy, i);
		}
		return (cur_paths[i]?true:false); 
	}
	else 
	{
		return do_normal_path_find(sx, sy, dx, dy, i);
	}	
}
//---------------End of function PathReuse::FindOtherPath---------------//

//---------------Begin of PathReuse::offset_path-----------------------//
// This function finds the paths of the units other than the leader by the use of the offset path
// method. This method is used whenever the destination offset of the unit from the leader is the 
// same as the source offset of the unit from the leader. This is almost always the case, since these
// offset are set to be the same due to the troop should try to maintain its shape as close as
// possible. 
//
// <int> sx - the x-coordinate of the source location
// <int> sy - the y-coordinate of the source location
// <int> dx - the x-coordinate of the destination
// <int> dy - the y-coordinate of the destination
// <int> i - the identity of the unit in the array
//
// return: <void>-there is no return variable for void function
void PathReuse::offset_path(int sx, int sy, int dx, int dy, int i)
{
	//-----Define and initialize local variables--------//
	ReuseNode* currentNode = NULL, *temperoryNode = NULL;
	ReuseNode* node = NULL;
	node = leader_path;
	ReuseNode* next = NULL, *firstUnblocked = NULL;
	ReuseNode* lastUnblocked;
	int rx1, ry1, rx2, ry2; //destination ranges
	//--------------------------------------------------//

	the_units[i]->get_object_range(dx, dy, rx1, ry1, rx2, ry2);
	de_occupy_destination(sx, sy, rx1, ry1, rx2, ry2, i);

	int sizeOfReuseNode = sizeof(ReuseNode);  
	currentNode = reuse_node_arr + reuse_node_count++;
	cur_paths_head[i] = currentNode;
	
	//This calculates the first node of the offset path
	currentNode->x = node->x + dest_offset_x;
	currentNode->y = node->y + dest_offset_y;	
	err_when(currentNode->x != sx || currentNode->y !=sy);
	
	//this while loop calculates the offset path
	while(node->next)
	{
		//if the offset path has no obstacle, just use the offset path
		while(node->next && (!check_occupancy(node->next, i)))
		{
			node = node->next;
			temperoryNode = reuse_node_arr + reuse_node_count++;
			currentNode->next = temperoryNode;
			temperoryNode->parent = currentNode;
			currentNode = temperoryNode;
			currentNode->x = node->x + dest_offset_x;
			currentNode->y = node->y + dest_offset_y;
			err_when(currentNode->x < 0);
			err_when(currentNode->y < 0);
			err_when(currentNode->x >= MAX_WORLD_X_LOC);
			err_when(currentNode->y >= MAX_WORLD_Y_LOC);
		}
		
		//if is already the destination, no need to handle obstacle
		if(!(node->next))
			break;
		
		//finding the last unblocked node before the last blocking location and then find a path to it
		//if the path to the unblocked node is not valid, find the second last unblocked node and so on until a path is found
		lastUnblocked = search_last_unblocked_node(node, i); 
		while(lastUnblocked && !(next = find_path_to_unblocked_node(currentNode, lastUnblocked, i)))
				lastUnblocked = search_next_unblocked_from_last_blocked(node, lastUnblocked, i);
		
		if(lastUnblocked && next)
		{
			currentNode = next;
			node = lastUnblocked;			
		}
		else
		{
			//case of destination blocked
			//abort searching and stay where your are
			currentNode->next = NULL;
			break;
		}
	}
	//points cur_paths[i] to the destination node, for later on used by retrieve
	cur_paths[i] = currentNode;

	re_occupy_destination(sx, sy, rx1, ry1, rx2, ry2, i);

	err_when(!cur_paths[i]); //there should at least be the source node
}
//-----------------End of function PathReuse::offset_path------------------//

//-----------------Begin of function PathReuse::search_next_unblocked_from_last_blocked------//
// This function searches the next unblocked node from the last blocked node, this is done
// whenever the path to the last unblocked node cannot be found, then the next unblocked counted
// from the last unblocked node should then be used
//
// <ReuseNode*> blockednode - this is the first blocked node in the leader path
// <ReuseNode*> lastunblocked - this is the last unblocked node found by search_last_unblocked_node
// <int> i - this is the identity of the unit in the unit aarray
//
// return: <ReuseNode*> - the next unblocked node count from the last blocked node is returned

PathReuse::ReuseNode* PathReuse::search_next_unblocked_from_last_blocked(ReuseNode* blockednode, ReuseNode* lastUnblocked, int i)
{
	ReuseNode *searchNode = lastUnblocked->parent;
	while(check_occupancy(searchNode, i) && searchNode->parent)
	{
		searchNode = searchNode->parent;
		if(searchNode == blockednode)
			return NULL; //no more path
	}
	while(!check_occupancy(searchNode, i) && searchNode->parent)
		searchNode = searchNode->parent;

	//I'd like the unblocked one
	return searchNode->next;
}

//-----------------End of function PathReuse::search_next_unblocked_from_last_blocked---------//

//-----------------Begin of function PathReuse::check_occupancy------------------//
// This function checks whether the offset node is occupied
// <ReuseNode*> *node - this is the node to be checked in the leader path
// <int> i - this is the identity of the unit in the unit array
// return: <bool> - return true if the node is occupied
//						- return false if the node is not occupied
bool PathReuse::check_occupancy(ReuseNode *node, int i)
{
	// the reference place is set to be the offset node
	int placeX = node->x+dest_offset_x;
	int placeY = node->y+dest_offset_y;
	bool occ = false;
	int is, js;
	int tmpLocX, tmpLocY;
	
	is = 0;
	tmpLocX = placeX;
	// as long as one of the node in the vicinity of the size of the unit is occupied
	// the place cannot be walked by the unit
	while (!occ && is<size_x[i])
	{
		js = 0;
		tmpLocY = placeY;
		while(!occ && js < size_y[i])
		{
			if(tmpLocX<0 || tmpLocX >= MAX_WORLD_X_LOC || tmpLocY <0 || tmpLocY >= MAX_WORLD_Y_LOC || !is_location_walkable(tmpLocX, tmpLocY) )
				occ=true;
			js++;
			tmpLocY++;	
		}
		is++;
		tmpLocX++;
	}
	return occ;
}
//------------------End of function PathReuse::check_occupancy--------------------//

//------------------Begin of function check_this_location_occupied----------------//
// This function checks whether the said location is occupied or not
// <int> placeX - the x-coordinate of the location
// <int> placeY - the y-coordinate of the location
// <int> i - the identity of the unit in the unit array
// return: <bool> - true if the location is occupied
//						- false if the location is not occupied
bool PathReuse::check_this_location_occupied(int placeX, int placeY, int i)
{
	//------Define local variables-------//
	bool occ = false;
	int is, js;
	int tmpLocX, tmpLocY;
	
	is = 0;
	tmpLocX = placeX;
	// the location cannot be walked if one of the node in the vicinity of the unit size is blocked
	while (!occ && is<size_x[i])
	{
		js = 0;
		tmpLocY = placeY;
		while(!occ && js < size_y[i])
		{
			if(tmpLocX<0 || tmpLocX>=MAX_WORLD_X_LOC || tmpLocY<0 || tmpLocY>=MAX_WORLD_Y_LOC || !is_location_walkable(tmpLocX, tmpLocY))			
				occ=true;
			js++;
			tmpLocY++;
		}
		is++;
		tmpLocX++;
	}
	return occ;
}
//-------------------End of function check_this_location_occupied------------------//

//-------------------Begin of function PathReuse::search_last_unblocked_node-------//
// This function searches the last unblocked node of the offset path from the leader path
// this is done whenever the offset path is blocked, skip all the unblocked node, and
// find the last one. This helps to smooth the path
// <ReuseNode*> - this is the first blocked node of the offset path in the leader path
// <int> i - this is the identity of the unit in the unit array
// return: <ReuseNode*> - this return the last unblocked node
PathReuse::ReuseNode* PathReuse::search_last_unblocked_node(ReuseNode* blockedNode, int i)
{
	//------Define local variables------//
	ReuseNode* searchNode;
	searchNode = cur_paths[0];	//points to the destination
	
	//seeking for the first unblocked node from the tail, this can be the destination
	while(check_occupancy(searchNode, i) && searchNode->parent)
	{
		searchNode = searchNode->parent;
		if(searchNode == blockedNode) //whole path blocked after blocked node
			return NULL;
	}
	
	while(!check_occupancy(searchNode, i) && searchNode->parent)
	{
		searchNode = searchNode->parent;
		err_when(searchNode == blockedNode); //that mean the node next to blocked node is not blocked. should not be here
	}
	
	//since what come out is the blocked node, I want the unblocked one
	return searchNode->next;
}
//-------------------End of function PathReuse::search_last_unblocked_node---------//

//-------------------Begin of function PathReuse::search_first_unblocked_node------//
// This function searches the first unblocked node in the offset path if the offset path
// is blocked somewhere
// <ReuseNode* &> - reference of the blocked node is input
// return <ReuseNode*> - the first unblocked node in the offset path is returned
PathReuse::ReuseNode* PathReuse::search_first_unblocked_node(ReuseNode*& blockedNode, int i)
{
	// must ensure blockedNode has child (next)
	//------Define local variables -------//
	ReuseNode* searchNode;
	searchNode = blockedNode;
	
	if (!(searchNode = searchNode->next))
		return NULL;

	
	while(check_occupancy(searchNode, i) && searchNode->next)
		searchNode=searchNode->next;
		
	if(check_occupancy(searchNode, i) && !searchNode->next) // case of destination blocked
		return NULL;
	
	blockedNode = searchNode;
	return searchNode;
}
//-------------------End of function PathReuse::search_first_unblocked_node----------//

//-------------------Begin of function PathReuse::find_path_to_unblocked_node--------//
// This function finds the path from the blocked node in the offset path to the unblocked node of the offset path one it is found
// The path is found by using the path finding algorithm
// <ReuseNode*> blocked_node - this is the blocked node of the offset path in the leader path
// <ReuseNode*> firstUnblocked - this is the first unblocked node of the offset path in the leader path
// <int> i - this is the identity of the unit in the unit array
// return: <ReuseNode*>- the first node of the found path is returned
PathReuse::ReuseNode* PathReuse::find_path_to_unblocked_node(ReuseNode* blockedNode, ReuseNode* firstUnblocked, int i)
{
	//-------Define local variables -------//
	ReuseNode* unblockedPathNode, *nextNode;
	int RangeX1, RangeX2, RangeY1, RangeY2;
			
	int destX = firstUnblocked->x + dest_offset_x;
	int destY = firstUnblocked->y + dest_offset_y;
		
	// do the path finding
	bool found;
	the_units[i]->get_object_range(destX, destY, RangeX1, RangeY1, RangeX2, RangeY2);
	if(handle_power_nation_flag)
	{
		err_when(!nation_recno);
		Nation* nationPtr = nation_array[nation_recno];
		path_finder.set_handle_power_nation(handle_power_nation_flag, nationPtr->relation_passable_array);
	}
	else
		path_finder.set_handle_power_nation(0);

	path_finder.set_attribute(size_x[i], size_y[i], mobile_type, PFIND_IGNORE_UNIT);
	found = path_finder.find_path(blockedNode->x, blockedNode->y, destX, destY, RangeX1, RangeY1, RangeX2, RangeY2, false);

	if (!found)
		return NULL;
	PathNode* tmp = path_finder.return_path_node();
	
	//copy the paht to our data structure
	unblockedPathNode = reuse_node_arr + reuse_node_count++;
	unblockedPathNode->x = tmp->x;
	unblockedPathNode->y = tmp->y;
	ReuseNode* unblocked_path = unblockedPathNode; //destination

	//copy the path to our data structure
	while(tmp->parent)
	{
		tmp = tmp->parent;
		nextNode = reuse_node_arr + reuse_node_count++;
		unblockedPathNode->parent = nextNode;
		nextNode->next = unblockedPathNode;
		unblockedPathNode = nextNode;
		unblockedPathNode->x = tmp->x;
		unblockedPathNode->y = tmp->y;
	}
	
	//this should give back the source node
	err_when(unblockedPathNode->x != blockedNode->x || unblockedPathNode->y != blockedNode->y);
	
	//linking the path to unblocked node to the offset path
	nextNode = unblockedPathNode->next; //node next to source node	
	//mem_del(unblockedPathNode);
	nextNode->parent = blockedNode;
	blockedNode->next = nextNode;
	return unblocked_path;
}
//----------------End of function PathReuse::find_path_to_unblocked_node--------------//

//----------------Begin of function PathReuse::join_offset_path-----------------------//
void PathReuse::join_offset_path(int sx, int sy, int dx, int dy, int i)
{
	err_here();
}
//----------------End if function PathReuse::join_offset_path-------------------------//


//----------------Begin of function PathReuse::retrieve-------------------------------//
// This function retrieve the path. Nodes along the same direction are deleted.
// Only the number of steps and number of directions are stored.
//
// <int&> pathCount - reference var for returning the number of nodes in the
//				  			 returned path node array.
//
// <int&> totalSteps - reference var for returning the total number of
//							  steps needed for walking through the whole path.
//
// <int> i - this is the identity of the unit in the unit array
//
// return: <PathResult*> - the path in terms of steps and direction is returned

PathResult* PathReuse::retrieve(int& pathCount, int& totalSteps, int i)
{
	int n = 0;

	//no path
	if(!cur_paths[i])
		return NULL;

	// points to the destination 
	ReuseNode* node = cur_paths[i];
	ReuseNode* base = cur_paths[i];
	ReuseNode* parent = node->parent;

		// finds the direction of the first node from the destination
	if (!parent)
		return NULL;

	n = 1;

	int vx = parent->x - node->x;
	int vy = parent->y - node->y;

	node = parent;

	int vx2, vy2;

	//delete nodes along the same direction
	while (parent = node->parent)
	{
		vx2 = parent->x - node->x;
		vy2 = parent->y - node->y;

		if (vx != vx2 || vy != vy2)
		{
			base->parent = node;
			base = node;
			n++;
			vx = vx2;
			vy = vy2;
		}
		//else
			//mem_del(node);
		node = parent;
	}
	base->parent = node;

	if (!n)
		return NULL;

	//---- allocates memory for the result path -------//

	PathResult* results = (PathResult*) mem_add(sizeof(PathResult)*(n+1));

	pathCount = 0;
	totalSteps = 0;
	node = cur_paths[i];		// the first node is the destination.

	int oldX = node->x; // stores the old x value
	int oldY = node->y; // ditto, but y
	int diffX, diffY;		 // stores the new x, new y, maximum of abs, and count
	int loopCount=0;

	while(node)
	{
		results[pathCount].loc_x = node->x;
		results[pathCount].loc_y = node->y;
		err_when(results[pathCount].loc_x < 0);
		err_when(results[pathCount].loc_x >= MAX_WORLD_X_LOC);
		err_when(results[pathCount].loc_y < 0);
		err_when(results[pathCount].loc_y >= MAX_WORLD_Y_LOC);

		pathCount++;

		//--- calculate the total number of steps to walk through the whole path ---//

		diffX = oldX - node->x;
		diffY = oldY - node->y;

		err_when( abs(diffX) > 0 && abs(diffY) > 0 && abs(diffX) != abs(diffY) );

		totalSteps += max(abs(diffX), abs(diffY));

		oldX = node->x;
		oldY = node->y;

		node = node->parent;

		err_when( loopCount++ > 10000 );
	}

	pathCount--;		// the last node is the source location which we don't need in returning the result
	totalSteps++;		// one starting step which is not added in the above calculation 

	return results;
}
//-------------------End of function PathReuse::retrieve--------------------//


//-------------------PathReuse::do_normal_path_find-------------------------//
//
// This function allows the unit other than the leader unit to do the normal path searching
// after the path is found, it is then copy to our data structure for retrieve
// this is done whenever the leader path cannot be found
// <int> sx - the x-coordinate of the source location
// <int> sy - the y-coordinate of the source location
// <int> dx - the x-coordinate of the destination
// <int> dy - the y-coordinate of the destination
// <int> i - the identity of the unit in the array
// return: <bool> - true if the path is found, false if the path is not found
bool PathReuse::do_normal_path_find(int sx, int sy, int dx, int dy, int i)
{
	//---------Define local variables-----------//
	PathNode* node, *parent;
	ReuseNode* currentNode, *temperoryNode;
	int RangeX1, RangeY1, RangeX2, RangeY2;
	bool found;

	the_units[i]->get_object_range(dx, dy, RangeX1, RangeY1, RangeX2, RangeY2);
	// use the pathfinder to find the path
	if(handle_power_nation_flag)
	{
		err_when(!nation_recno);
		Nation* nationPtr = nation_array[nation_recno];
		path_finder.set_handle_power_nation(handle_power_nation_flag, nationPtr->relation_passable_array);
	}
	else
		path_finder.set_handle_power_nation(0);

	path_finder.set_attribute(size_x[i], size_y[i], mobile_type, PFIND_IGNORE_UNIT);
	found = path_finder.find_path(sx, sy, dx, dy, RangeX1, RangeY1, RangeX2, RangeY2, false);
	
	// if the path is found, it is then copied to our data structure
	if(!found && handle_power_nation_flag)
	{
		path_finder.set_handle_power_nation(0);
		found = path_finder.find_path(sx, sy, dx, dy, RangeX1, RangeY1, RangeX2, RangeY2, false);
	}

	if(found)
	{
		node = path_finder.return_path_node();
		currentNode = reuse_node_arr + reuse_node_count++;
		currentNode->x = node->x;
		currentNode->y = node->y;	
		cur_paths[i] = currentNode;
					
		//copying the path to our data structure
		while(parent=node->parent)
		{
			node = parent; 
			temperoryNode = reuse_node_arr + reuse_node_count++;
					
			currentNode->parent = temperoryNode;
			temperoryNode->next = currentNode;
			currentNode = temperoryNode;
			currentNode->x = node->x;
			currentNode->y = node->y;
		}
		
		err_when(!currentNode);
		cur_paths_head[i] = currentNode;
	}
	//return true if path is found, false otherwise
	return (cur_paths[i]?true:false);
}
//------------------End of function PathReuse::do_normal_pathfind---------------//

//The routines below are for the purpose of smoothing the paths
//------------------Begin of function PathReuse::remove_path_to_duplicate_nodes-------//
//if a path lead to a repeated position, that path should be deleted
// <int> i - this is the identity of the unit in the unit array
// return <void> - there is no return variable for void function
void PathReuse::remove_path_to_duplicate_nodes(int i)
{
	ReuseNode* theNode = cur_paths_head[i];
	ReuseNode* searchNode;
	// for each node from the head, if there is a corresponding node found from finding the tail
	/// the path in between is deleted
	while(theNode && theNode->next)
	{	
		searchNode = cur_paths[i];
		while(searchNode)
		{
			// same node !!!
			if(searchNode->next && searchNode->x == theNode->x && searchNode->y == theNode->y)
			{
				theNode->next = searchNode->next;
				searchNode->next->parent = theNode;
				break;
			}
			else if(searchNode->x == theNode->x && searchNode->y == theNode->y) //destination reached
			{
				theNode->parent->next = searchNode;
				searchNode->parent = theNode->parent;
				break;
			}
			searchNode = searchNode->parent;
		}

		theNode = theNode->next;	
	}
}
//-----------------End of function PathReuse::remove_path_to_duplicate_nodes--------------//

//-----------------Begin of function PathReuse::smoothing_path-----------------------//
// This function smoothes out right angle paths
// <int> i - this is the identity of the unit in the unit array
// return: <void> - there is no return variable for void function 
void PathReuse::smoothing_path(int i)
{
	ReuseNode* node = cur_paths[i];
	if(!node)
		return;

	ReuseNode* base = cur_paths[i];
	ReuseNode* parent = node->parent;
	ReuseNode* newnode;
	int avgx, avgy;

	if (!parent)
		return;
	int vx = parent->x - node->x;
	int vy = parent->y - node->y;

	node = parent;

	int vx2, vy2;

	//case with right angle in x-y direction
	while (parent = node->parent)
	{
		vx2 = parent->x - node->x;
		vy2 = parent->y - node->y;
		if(abs(vx2-vx) == 1 && abs(vy2-vy) == 1) //right angle
		{
			parent->next = node->next;
			node->next->parent = parent;
			vx2 += vx;
			vy2 += vy;
		}
		//case with right angle in 45 degree direction
		else if((abs(vx2-vx) ==0 && abs(vy2-vy) ==2) || (abs(vx2-vx) ==2 && abs(vy2-vy)==0)) //right angle
		{
			if(!check_this_location_occupied(avgx=(parent->x+node->next->x)/2, avgy=(parent->y+node->next->y)/2, i))
			{	
				newnode = reuse_node_arr+reuse_node_count++;
				newnode->x = avgx;
				newnode->y = avgy;
				newnode->parent = parent;
				newnode->next = node->next;
				parent->next = newnode;
				node->next->parent = newnode;
				vx2 = (vx2+vx)/2;
				vy2 = (vy2+vy)/2;
			}
		}

		vx = vx2;
		vy = vy2;
		node = parent;
	}
}	
//----------------End of function smoothing_path------------------------//

//----------------Begin function PathReuse::de_occupy_destination()-----//
void PathReuse::de_occupy_destination(int sourceX, int sourceY, int destRangeX1, int destRangeY1, int destRangeX2, int destRangeY2, int i)
{
	int xLoc, yLoc;
	Location* locPtr;
	int destRangeXCount=destRangeX2-destRangeX1+1;

	int index=0;

	for( yLoc=destRangeY1 ; yLoc<=destRangeY2 ; yLoc++ )
	{
		locPtr = world.get_loc(destRangeX1, yLoc);

		for( xLoc=destRangeX1 ; xLoc<=destRangeX2 ; xLoc++, locPtr++, index++ )
		{
			if( xLoc >= sourceX && xLoc < sourceX + size_x[i] &&	// overlap with the source location
				 yLoc >= sourceY && yLoc < sourceY + size_y[i] )
			{
				continue;
			}

			save_loc_buf[index].loc_flag 	 = locPtr->loc_flag;
			save_loc_buf[index].cargo_recno = locPtr->cargo_recno;

			locPtr->loc_flag = LOCATE_WALK_LAND;
			locPtr->cargo_recno = 0;
		}
	}
}
//------------- End of function PathReuse::de_occupy_destination-------------//

//------------- Begin of function PathReuse::re_occupy_destination-----------//
void PathReuse::re_occupy_destination(int sourceX, int sourceY, int destRangeX1, int destRangeY1, int destRangeX2, int destRangeY2, int i)
{
	int xLoc, yLoc;
	Location* locPtr;
	
	int index=0;

	for( yLoc=destRangeY1 ; yLoc<=destRangeY2 ; yLoc++ )
	{
		locPtr = world.get_loc(destRangeX1, yLoc);

		for( xLoc=destRangeX1 ; xLoc<=destRangeX2 ; xLoc++, locPtr++, index++ )
		{
			if( xLoc >= sourceX && xLoc < sourceX + size_x[i] &&
				 yLoc >= sourceY && yLoc < sourceY + size_y[i] )
			{
				continue;
			}

			locPtr->loc_flag 	  = save_loc_buf[index].loc_flag;
			locPtr->cargo_recno = save_loc_buf[index].cargo_recno;
		}
	}
}
//------------- End of function PathReuse::re_occupy_destination-------------//