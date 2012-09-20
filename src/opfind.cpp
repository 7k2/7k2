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

// Filename: OPFIND.CPP
// Description: Implementation of the Pathfinding algorithm

#include <cstdlib>
#include <all.h>
#include <opfind.h>
#include <oworldmt.h> 
#include <osprite.h>
#include <osys.h>

//------- define constant --------//

#define MAX_TRIES 		1000 	// max. num of re-attempts
#define MAX_STACK_NUM	5000 	// maximum stack number
#define MAX_NODE_NUM		6000 	// maximum node number
#define SAFE_NODE_COUNT_BASE  0x7fffff00

#define MAX_MAP_WIDTH_A 0x100
#define MAX_MAP_HEIGHT_A (MAX_MAP_HEIGHT+1)
#if(MAX_MAP_WIDTH_A < MAX_MAP_WIDTH+1)
	#error
#endif

#define USE_NODE_COUNT_BASE
#define USE_PRIORITY_QUEUE

#define MAX_UNIT_WIDTH 2
#define MAX_UNIT_HEIGHT 2

// coefficient when path is not found, see get_closest_node
#define H_TO_G_RATIO 4

// ------- define function pointer type --------//

typedef int (*CanWalkFunc)(int xLoc, int yLoc);

//----- define static members --------//

static short size_x, size_y;	// store the size of the unit whose path is to be found
static char  mobile_type;
static int	 search_flags;
static int	 node_matrix[MAX_MAP_HEIGHT_A][MAX_MAP_WIDTH_A];		// stores the node index number of each location
static int 	 dest_x, dest_y;				// destination of unit
static int	 sour_x, sour_y;
static int 	 stack_pos;						// stack position
static int 	 node_count;					// node count
static int 	 node_count_base;				// base node count

// static bool temp_occ[(MAX_UNIT_HEIGHT+2)*(MAX_UNIT_WIDTH+2)];		// on when PathFinder is being used, off when it isn't
										// stores the temporary occupany in the area associated with the unit

#ifdef USE_PRIORITY_QUEUE
	#define MAX_F (MAX_MAP_HEIGHT_A+MAX_MAP_WIDTH_A)
	static PathNode* open[MAX_F+1];		// one linked-list for each f
	static int low_queue;
	static int high_queue;

	static void insert_priority_queue(PathNode *n);
	static PathNode* remove_head_priority_queue();
#else
	static PathNode* open;  // open list
#endif

static PathNode* closed; // closed list
static PathNode* cur_path; // path found so far

static PathNode* stack[MAX_STACK_NUM]; // stack
static PathNode node_arr[MAX_NODE_NUM]; // node array used in optimisation

static SaveLocation* save_loc_buf;

static char handle_power_nation_flag=0;
static char	nation_passable[MAX_NATION+1] = {0}; // Note: position 0 is not used for faster access
static CanWalkFunc can_walk_func = NULL;

//------ declare static functions --------//

static int is_location_walkable(int xLoc, int yLoc);
static int is_location_accessible(int xLoc, int yLoc);

static void normalize_node_count_base()
{
	if( node_count_base == 0)
		return;

	for( int j = 0; j < MAX_MAP_HEIGHT_A; ++j )
	{
		int *p = node_matrix[j];
		for( int i = 0; i < MAX_MAP_WIDTH_A; ++i )
		{
			if( p[i] > node_count_base )
				p[i] -= node_count_base;
			else
				p[i] = 0;
		}
	}

	node_count_base = 0;
}


// Gilbert : Improvment
// 1. node_arr, can be a static array like stack ,memset in reset() may be removed
// 2. node_matrix can use MAX_MAP_WIDTH instead of MAX_WORLD_X_LOC to reduce non-constant multiplication
//    node_matrix can also be a static array if using MAX_MAP_WIDTH
//    memset in reset() may be reduce by using base_node_count method :
//       store node_matrix[y][x] = node_count + base_node_count instead of node_matrix[y][x] = node_count;
//       base_node_count is updated (increase by node_count) after a path searching
//    or change type of node_matrix from int to short, given MAX_NODE_NUM is within 0x7fff
// 3. temp_occ can also change to static array
// 4. explore the node which is the current direction has higher priority

static int pfind_dist( int srcX, int srcY)
{
	// circle distance
	// return (dest_x - srcX)*(dest_x - srcX) + (dest_y - srcY)*(dest_y - srcY);

	int dx, dy;

	if( (dx = dest_x - srcX) < 0 )		// abs( srcX, dest_x )
		dx = -dx;

	if( (dy = dest_y - srcY) < 0 )		// abs( srcY, dest_y )
		dy = -dy;

	// square distance
//	return MAX( dx, dy );

// octagon distance
//	if( dx > dy )
//		return dx + (dy+7) / 8;
//	else
//		return dy + (dx+7) / 8;

	// rhombus distance
	return dx + dy;
}


// used in get_closest_node
static int pfind_close_dist( int srcX, int srcY)
{
	int dx, dy;

	if( (dx = dest_x - srcX) < 0 )		// abs( srcX, dest_x )
		dx = -dx;

	if( (dy = dest_y - srcY) < 0 )		// abs( srcY, dest_y )
		dy = -dy;

	// square distance
	return MAX( dx, dy );

	// rhombus distance
	// return dx + dy;
}


//------ Begin of static function is_location_walkable ------//

static int is_location_walkable(int xLoc, int yLoc)
{
	err_when(xLoc < 0);
	err_when(yLoc < 0);
	err_when(xLoc >= MAX_WORLD_X_LOC);
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


//------ Begin of static function is_location_accessible ------//

static int is_location_accessible(int xLoc, int yLoc)
{
	err_when(xLoc < 0);
	err_when(yLoc < 0);
	err_when(xLoc >= MAX_WORLD_X_LOC);
	err_when(yLoc >= MAX_WORLD_Y_LOC);

	Location* locPtr = world.get_loc(xLoc, yLoc);

	if( handle_power_nation_flag )
	{
		int powerNationRecno=locPtr->power_nation_recno;

		if( powerNationRecno && !nation_passable[powerNationRecno] )
			return 0;
	}

	return locPtr->is_accessible(mobile_type);
}
//------ End of static function is_location_accessible ------//


//-- begin of PathFinder constructor --//

PathFinder::PathFinder()
{
	// initialise basic variables
#ifdef USE_PRIORITY_QUEUE
	low_queue = 0;
	high_queue = 0;
	memset( open, 0, sizeof(open) );
#else
	open = NULL;
#endif

	closed = NULL;
	cur_path = NULL;
	node_count = 0;
	stack_pos = 0;
	size_x = size_y = 0;
//	temp_occ = NULL;
//	node_matrix = NULL;
}
//---- end of PathFinder constructor ----///


//------ begin of PathFinder destructor ----///
PathFinder::~PathFinder()
{
	deinit();
}
//-----end of PathFinder destructor ----///


//------------begin of function PathFinder::init ---------//
//
// initiate class before use
//
void PathFinder::init()
{
	// initialise the node_arr;
	// node_arr 	= (PathNode*) mem_add(MAX_NODE_NUM * sizeof(PathNode));
	// node_matrix = (int *) mem_add(sizeof(int)*(MAX_WORLD_X_LOC+1) *(MAX_WORLD_Y_LOC+1));

	save_loc_buf = (SaveLocation*) mem_add(sizeof(SaveLocation) * PATH_FIND_LOC_BUFFER_SIZE );
	node_count_base = 0;
}
//------------end of function PathFinder::init ---------//


//-----------begin of function PathFinder::deinit ---------//
//
// deinit class after use
//
void PathFinder::deinit()
{
	// deletes memory allocated
//	if(node_arr)
//	{
//		mem_del(node_arr);
//		node_arr = NULL;
//	}

//	if (temp_occ)
//	{
//		mem_del(temp_occ);
//		temp_occ = NULL;
//	}

//	if (node_matrix)
//	{
//		mem_del(node_matrix);
//		node_matrix = NULL;
//	}

	if( save_loc_buf )
	{
		mem_del(save_loc_buf);
		save_loc_buf = NULL;
   }
}
//------end of function PathFinder::deinit--//


//----------begin of function PathFinder::reset ---------//
//
// reset class each time use
//
void PathFinder::reset()
{
#ifdef USE_NODE_COUNT_BASE
	node_count_base += node_count;
#endif

	// resets some path variables
#ifdef USE_PRIORITY_QUEUE
	low_queue = 0;
	high_queue = 0;
	memset( open, 0, sizeof(open) );
#else
	open = NULL;
#endif
	closed = NULL;
	node_count = 0;
	stack_pos = 0;
	cur_path = NULL;

	// zero out the memory of node_arr
//	memset(node_arr, 0, MAX_NODE_NUM * sizeof(PathNode));
//	memset(node_matrix, 0, sizeof(int)*(MAX_WORLD_X_LOC+1) *(MAX_WORLD_Y_LOC+1) );

#ifdef USE_NODE_COUNT_BASE
	if( node_count_base >= SAFE_NODE_COUNT_BASE - MAX_NODE_NUM )
	{
		//normalize_node_count_base();
		memset(node_matrix, 0, sizeof(int)*MAX_MAP_WIDTH_A*MAX_MAP_HEIGHT_A );
		node_count_base = 0;
	}
#else
	memset(node_matrix, 0, sizeof(int)*MAX_MAP_WIDTH_A*MAX_MAP_HEIGHT_A );
#endif
}
//----------end of function PathFinder::reset ---------//


//----------begin of function PathFinder::set_attribute---------//
//
// set attributes of the path finder
//
void PathFinder::set_attribute(short x, short y, char mobileType, int searchFlags)
{
	if (size_x != x || size_y != y)
	{
		size_x = x;
		size_y = y;

		err_when( size_x > MAX_UNIT_WIDTH );
		err_when( size_y > MAX_UNIT_HEIGHT );

		// temp_occ must be of size (2+x)*(2+y)
		// temp_occ = (bool *) mem_resize(temp_occ, sizeof(bool)*(2+x)*(2+y));
		// memset( temp_occ, 0, sizeof(temp_occ) );
	}

	mobile_type = mobileType;
	search_flags = searchFlags;

	if( search_flags & PFIND_IGNORE_UNIT )
		can_walk_func = is_location_accessible;
	else
		can_walk_func = is_location_walkable;
}
//--------end of function PathFinder::set_attribute--------//////


//-------- Begin of function PathFinder::set_handle_power_nation ---------//
//
// set up flags for whether handle power nation is used
//
void PathFinder::set_handle_power_nation(int handlePowerFlag, char* nationPassable)
{
	handle_power_nation_flag = handlePowerFlag;

	if( nationPassable )
		memcpy(nation_passable+1, nationPassable, sizeof(char)*MAX_NATION);
}

//--------- End of function PathFinder::set_handle_power_nation ---------//


//-------- End of function PathFinder::find_path--------//////
//
// <int> sourceX 		- the integral x-coordinate of the source location
// <int> sourceY 	   - the integral y-coordinate of the source location
// <int> destX 	   - the integral x-coordinate of the destination
// <int> destY		   - the integral y-coordinate of the destination
//
//	<int> destRangeX1, destRangeY1, destRangeX2, destRangeX3
//							- the range of the destination,
//							  for normal movement, it is the area occupied by the building
//							  for range attack, it is the area occupied by the building plus attack range
//
// [bool] raiseIt 	- whether the source location should be temporarily set unoccupied during the search
//						 	  (this is 0 when called by the reuse function because reuse does not have the unit in the source location)
//						 	  (default: true)
//
// [int]  maxTries   - number of node to tries, avoid excessive path searching in aggressive attack (default:0 - use default)
// [int]  stopAtRange - stop find path when range is less then (for range attack)
//
// returns: true if a path was found, false if not
//
bool PathFinder::find_path(int sourceX, int sourceY, int destX, int destY,
									int destRangeX1, int destRangeY1, int destRangeX2, int destRangeY2, bool raiseIt, bool raiseDest, int maxTries, int stopAtRange)
{
#ifdef DEBUG
	unsigned long pathFinderStartTime = misc.get_time();
#endif

	int i, j, unitRecno=0;

	//----- set the source location unoccupied -------//

	if( raiseIt )
	{
		for (i = 0; i < size_x; i++)
		{
			for (j = 0; j < size_y; j++)
			{
				if( !unitRecno )
					unitRecno = world.get_loc(sourceX+i, sourceY+j)->unit_recno(mobile_type);

				world.get_loc(sourceX+i, sourceY+j)->remove_unit(mobile_type);
			}
		}
	}

	//---- set the destination range unoccupied -----//

	err_when( (destRangeX2-destRangeX1+1) * (destRangeY2-destRangeY1+1) > PATH_FIND_LOC_BUFFER_SIZE );

	int xLoc, yLoc;
	int destRangeXCount=destRangeX2-destRangeX1+1;
	Location* locPtr;

	if(raiseDest)
	{
		i=0;

		for( yLoc=destRangeY1 ; yLoc<=destRangeY2 ; yLoc++ )
		{
			Location* locPtr = world.get_loc(destRangeX1, yLoc);

			for( xLoc=destRangeX1 ; xLoc<=destRangeX2 ; xLoc++, locPtr++, i++ )
			{
				if( xLoc >= sourceX && xLoc < sourceX + size_x &&	// overlap with the source location
					 yLoc >= sourceY && yLoc < sourceY + size_y )
				{
					continue;
				}

				save_loc_buf[i].loc_flag 	 = locPtr->loc_flag;
				save_loc_buf[i].cargo_recno = locPtr->cargo_recno;

				locPtr->loc_flag = LOCATE_WALK_LAND;
				locPtr->cargo_recno = 0;
			}
		}
	}

	// ------ set default max tries -------//

	if( maxTries <= 0 )
	{
		maxTries = MAX_TRIES;
	}

	//--------- find the path now --------//

	bool rc = find_path2(sourceX, sourceY, destX, destY, maxTries, stopAtRange);

	//----- restore the source location occupancy status -------//

	if(raiseIt)
	{
		for (i = 0; i < size_x; i++)
		{
			for (j = 0; j < size_y; j++)
				world.get_loc(sourceX+i, sourceY+j)->set_unit(unitRecno, mobile_type);
		}
	}

	//------ restore destination range occupancy status --------//

	if(raiseDest)
	{
		i=0;

		for( yLoc=destRangeY1 ; yLoc<=destRangeY2 ; yLoc++ )
		{
			locPtr = world.get_loc(destRangeX1, yLoc);

			for( xLoc=destRangeX1 ; xLoc<=destRangeX2 ; xLoc++, locPtr++, i++ )
			{
				if( xLoc >= sourceX && xLoc < sourceX + size_x &&
					 yLoc >= sourceY && yLoc < sourceY + size_y )
				{
					continue;
				}

				locPtr->loc_flag 	  = save_loc_buf[i].loc_flag;
				locPtr->cargo_recno = save_loc_buf[i].cargo_recno;
			}
		}
	}

#ifdef DEBUG
	// ######## begin Gilbert 27/4 #######//
	pathFinderStartTime = misc.get_time() - pathFinderStartTime;
	pathfind_profile_time += pathFinderStartTime;
	pathfind_count++;
	if( pathFinderStartTime > longest_pathfind_profile_time )
	{
		longest_pathfind_profile_time = pathFinderStartTime;
		longest_pathfind_unit_recno = unitRecno;
	}
	// ######## begin Gilbert 27/4 #######//
#endif

	return rc;
}
//-------- End of function PathFinder::find_path--------//////


//--------begin  of function PathFinder::find_path--------//////
//
// <int> sourceX - the integral x-coordinate of the source location
// <int> sourceY - the integral y-coordinate of the source location
// <int> destX - the integral x-coordinate of the destination
// <int> destY - the integral y-coordinate of the destination
// <int>  maxTries   - number of node to tries, avoid excessive path searching in aggressive attack (default:0 - use default)
// <int>  stopAtRange - stop find path when range is less then (for range attack)
//
// returns: true if a path was found, false if not
//
bool PathFinder::find_path2(int sourceX, int sourceY, int destX, int destY, int maxTries, int stopAtRange)
{
	// POTENTIAL BUG_HERE: because this function first sets the location unoccupied
	// and then occupied again after path searching has been done, this function should
	// be used with caution when used for finding a path from an arbitrary location to another

	reset();  // resets member variables

	//--- calculates the distance from the destination to the source ---

	// int dist = (destX-sourceX)*(destX-sourceX) + (destY-sourceY)*(destY-sourceY);
	dest_x = destX;
	dest_y = destY;
	sour_x = sourceX;
	sour_y = sourceY;
	int dist = pfind_dist( sourceX, sourceY);

	// if dest was occupied, or if the distance was too short, don't do it ---/

	err_when(destX < 0);
	err_when(destY < 0);
	err_when(destX >= MAX_WORLD_X_LOC);
	err_when(destY >= MAX_WORLD_Y_LOC);

	// do not use *can_walk_func
	if (!is_location_walkable(destX, destY) && dist < 1)
	{
		return false;
	}

	//----- initialise values of node -------//

	PathNode* node;
	node = node_arr + node_count++;
	memset(node, 0, sizeof(PathNode));

	node->g = 0;
	node->h = dist;
	node->f = node->g + node->h;
	node->x = sourceX;
	node->y = sourceY;

#ifdef USE_NODE_COUNT_BASE
	node_matrix[sourceY][sourceX] = node_count + node_count_base;  // node_count has been increased
	err_when( node_count_base + node_count > SAFE_NODE_COUNT_BASE );
//	if( node_count_base + node_count >= SAFE_NODE_COUNT_BASE )	// assume reset() can prevent
//		normalize_node_count_base();
#else
	node_matrix[sourceY][sourceX] = node_count;
#endif

	//----- points open list to node ------//

#ifdef USE_PRIORITY_QUEUE
	insert_priority_queue(node);
	low_queue = high_queue;		// for fast access
#else
	open = node;
#endif

	if ( misc.points_distance(sourceX, sourceY, destX, destY) <= stopAtRange )
	{
		// if src = dest, just returns the src/dest node
		cur_path = node;

		return true;
	}

	PathNode* bestPathNode;
//	dest_x = destX;
//	dest_y = destY;
//	sour_x = sourceX;
//	sour_y = sourceY;

	long yieldCount = 400;

	//----- only try for MAX_TRIES times -------//

	for (int tries = maxTries; tries > 0; --tries)
	{
		// gets the best node so far
		bestPathNode = best_node();

		// if there's no best node, abort
		if (!bestPathNode)
		{
			return false;
		}

		// if we've found the destination, return successfully
		// if (bestPathNode->x == destX && bestPathNode->y == destY) //destination found
		if ( misc.points_distance(bestPathNode->x, bestPathNode->y, destX, destY) <= stopAtRange )
		{
			cur_path = bestPathNode;
			return true;
		}

		// generates children
		gen_childs(bestPathNode);

		if( --yieldCount == 0 )
		{
			sys.yield();
			yieldCount = 500;
		}
	}
	err_when(stack_pos); //should be zero all time, all pushes should have been popped

	return false;
}
//-- end of function PathFinder::find_path2 ---///


//----- Begin of function PathFinder::is_path_found -----//
//
// check whether path is found
//
bool PathFinder::is_path_found()
{
	return cur_path != NULL;
}
//----- End of function PathFinder::is_path_found -----//


//----- Begin of function PathFinder::return_path_node -----//
//
// returns the destination node
//
PathNode* PathFinder::return_path_node()
{
	return cur_path;
}
//----- End of function PathFinder::return_path_node -----//


///--- begin of function PathFinder::retrieve  ---//
//
// This function should be called after find_path to retreive a path
// it returns the path retrieved.
//
// <int&> pathCount - reference var for returning the number of nodes in the
//				  			 returned path node array.
//
// <int&> totalSteps - reference var for returning the total number of
//							  steps needed for walking through the whole path.
//
// return: <Path*> - path node array. The first node is the destination
//							location. The last node is the first movement point.
//
PathResult* PathFinder::retrieve(int& pathCount, int& totalSteps)
{
	// n will be modified to reflect the number of direction changes

	int n = 0;

	if (!cur_path)
	{
		// Path was not found ; returns the closest path
		cur_path = get_closest_node();
		if (!cur_path)
			return NULL;
	}

	// node variable for traversing the list
	PathNode* node = cur_path;
	// the base of the cur_path
	PathNode* base = cur_path;
	PathNode* parent = node->parent; // parent of the node

	if (!parent)
		return NULL;

	n = 1; ///BUG_HERE: There was a bug here, since the n=1 state iis originally before the if statememt

	int directionOfPathSoFarX = parent->x - node->x;
	int directionOfPathSoFarY = parent->y - node->y;

	node = parent;

	int directionOfPathSoFarX2, directionOfPathSoFarY2; // store the current direction so far

	//----------------- retrieves the path NOW ----------------------//

	while (parent = node->parent)
	{
		// if directions are not the same
		directionOfPathSoFarX2 = parent->x - node->x;
		directionOfPathSoFarY2 = parent->y - node->y;

		if (directionOfPathSoFarX != directionOfPathSoFarX2 ||
			 directionOfPathSoFarY != directionOfPathSoFarY2)
		{
			base->parent = node;
			base = node; 
			n++;
			directionOfPathSoFarX = directionOfPathSoFarX2;
			directionOfPathSoFarY = directionOfPathSoFarY2;
		}
		node = parent;
	}

	base->parent = node;

	if (!n)
		return NULL;

	//---- allocates memory for the result path -------//

	PathResult* results = (PathResult *) mem_add(sizeof(PathResult)*(n+1));

	pathCount = 0;
	totalSteps = 0;
	node = cur_path;		// the first node is the destination.

	int oldX = node->x; // stores the old x value
	int oldY = node->y; // ditto, but y
	int diffX, diffY;		 // stores the new x, new y, maximum of abs, and count
	int loopCount=0;

	while(node)
	{
		results[pathCount].loc_x = node->x;
		results[pathCount].loc_y = node->y;

		pathCount++;

		//--- calculate the total number of steps to walk through the whole path ---//

		diffX = oldX - node->x;
		diffY = oldY - node->y;

		err_when( abs(diffX) > 0 && abs(diffY) > 0 && abs(diffX) != abs(diffY) );

		totalSteps += MAX(abs(diffX), abs(diffY));

		oldX = node->x;
		oldY = node->y;

		node = node->parent;

		err_when( loopCount++ > 10000 );
	}

	pathCount--;		// the last node is the source location which we don't need in returning the result
	totalSteps++;		// one starting step which is not added in the above calculation 

	return results;
}
//-------- End of function PathFinder::retrieve ---//


//--- begin of function PathFinder::path_step -----//
//
int PathFinder::path_step()
{
	if (!cur_path)
		// Path was not found ; returns the closest path
		return get_closest_node()->g;
	else
		return cur_path->g;
}
//--- end of function PathFinder::path_step -----//


//--- begin of function PathFinder::best_node -----//

PathNode* PathFinder::best_node()
{
	// a temporary node for traversing the list
   PathNode* temp;

#ifdef USE_PRIORITY_QUEUE
	temp = remove_head_priority_queue();
	if( !temp )
		return NULL;
#else
	if (!open)
		return NULL;

	temp = open;
	open = temp->next;
#endif

	temp->next = closed;
	temp->closed_flag = 1;		// moved to closed list, set closed_flag to 1
	closed = temp;

	return temp;
}
//--- end of function PathFinder::best_node---/////


//--- begin of function PathFinder::gen_childs---/////
void PathFinder::gen_childs(PathNode* n)
{
	int x, y, w, h;
	int nx = n->x;
	int ny = n->y;

	// which explore node first depend of the current direction

	static char init_explore_dir_array[MAX_DIR] = { DIR_N, DIR_NW, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE };
	static char explore_dir_array[MAX_DIR][MAX_DIR] =		// [initial direction][next search direction]
	{
		{ DIR_N , DIR_NE, DIR_NW, DIR_E , DIR_W , DIR_SE, DIR_SW, DIR_S  },
		{ DIR_NE, DIR_E , DIR_N , DIR_SE, DIR_NW, DIR_S , DIR_W , DIR_SW },
		{ DIR_E , DIR_SE, DIR_NE, DIR_S , DIR_N , DIR_SW, DIR_NW, DIR_W  },
		{ DIR_SE, DIR_S , DIR_E , DIR_SW, DIR_NE, DIR_W , DIR_N , DIR_NW },
		{ DIR_S , DIR_SW, DIR_SE, DIR_W , DIR_E , DIR_NW, DIR_NE, DIR_N  },
		{ DIR_SW, DIR_W , DIR_S , DIR_NW, DIR_SE, DIR_N , DIR_E , DIR_NE },
		{ DIR_W , DIR_NW, DIR_SW, DIR_N , DIR_S , DIR_NE, DIR_SE, DIR_E  },
		{ DIR_NW, DIR_N , DIR_W , DIR_NE, DIR_SW, DIR_E , DIR_S , DIR_SE },
	};

	int	dirSeqCount;
	char *dirSeqArray;
	PathNode* parent = n->parent;

	if( !parent )
	{
		// root node
		dirSeqCount = MAX_DIR;
		dirSeqArray = explore_dir_array[Sprite::get_dir(sour_x, sour_y, dest_x, dest_y)];	// overall direction
	}
	else
	{
		int goX;
		int goY;
		goX = n->x - parent->x;
		goY = n->y - parent->y;
		dirSeqCount = MAX_DIR - 3;		// exclude last 3 direction last it go backward (180 deg turn)
		// and 2 more direction are not necessary, mostly covered by previous node (135 deg turn)
		// actually, further 2 more (90 deg turn) rarely useful

		if( goY < 0 )
		{
			// north, north east or north west
			if( goX < 0 )
				dirSeqArray = explore_dir_array[DIR_NW];
			else if( goX > 0 )
				dirSeqArray = explore_dir_array[DIR_NE];
			else
				dirSeqArray = explore_dir_array[DIR_N];
		}
		else if( goY > 0 )
		{
			// south, south east or south west
			if( goX < 0 )
				dirSeqArray = explore_dir_array[DIR_SW];
			else if( goX > 0 )
				dirSeqArray = explore_dir_array[DIR_SE];
			else
				dirSeqArray = explore_dir_array[DIR_S];
		}
		else
		{
			// east or west
			if( goX < 0 )
				dirSeqArray = explore_dir_array[DIR_W];		// west
			else if( goX > 0 )
				dirSeqArray = explore_dir_array[DIR_E];		// east
			else
			{
				err_here();		// zero...? 
				dirSeqCount = MAX_DIR;						// center or uncertain
				dirSeqArray = init_explore_dir_array;
			}
		}
	}

	// reverse direction, such that less preferred put on the bottom of "open" link list
	// see the behaviour insert() function

	for( int dirSeq = dirSeqCount-1; dirSeq >= 0; --dirSeq )
	{
		switch( dirSeqArray[dirSeq] )
		{
		case DIR_N:
			if( ny > 0 )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny-1][nx] - node_count_base) > 0 )
#else
				if( node_matrix[ny-1][nx] > 0 )
#endif
				{
					gen_child(n, nx, ny-1);
				}
				else if( size_x == 1 )
				{
					if( (*can_walk_func)(nx, ny-1) )
						gen_child(n, nx, ny-1);
				}
				else
				{
					x = nx;
					y = ny - 1;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					if( w == 0 )
						gen_child(n, nx, ny-1);
				}
			}
			break;

		case DIR_NE:
			// checks the NE direction		
			if( ny > 0 && nx + size_x < MAX_WORLD_X_LOC )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny-1][nx+1] - node_count_base) > 0 )
#else
				if( node_matrix[ny-1][nx+1] > 0 )
#endif
				{
					gen_child(n, nx+1, ny-1);
				}
				else if( size_x == 1 && size_y == 1 )
				{
					if( (*can_walk_func)(nx+1, ny-1) )
						gen_child(n, nx+1, ny-1);
				}
				else
				{
					x = nx + 1;
					y = ny - 1;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					x = nx + size_x;
					y = ny - 1;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( w == 0 && h == 0)
						gen_child(n, nx+1, ny-1);
				}
			}
			break;

		case DIR_E:
			if( nx + size_x < MAX_WORLD_X_LOC )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny][nx+1] - node_count_base) > 0 )
#else
				if( node_matrix[ny][nx+1] > 0 )
#endif
				{
					gen_child(n, nx+1, ny);
				}
				else if( size_y == 1 )
				{
					if( (*can_walk_func)(nx+1, ny) )
						gen_child(n, nx+1, ny);
				}
				else
				{
					x = nx + size_x;
					y = ny;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last y will > MAX_WORLD_Y_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( h == 0)
						gen_child(n, nx+1, ny);
				}
			}
			break;

		case DIR_SE:
			// checks the SE direction
			if( ny + size_y < MAX_WORLD_Y_LOC && nx + size_x < MAX_WORLD_X_LOC )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny+1][nx+1] - node_count_base) > 0 )
#else
				if( node_matrix[ny+1][nx+1] > 0 )
#endif
				{
					gen_child(n, nx+1, ny+1);
				}
				else if( size_x == 1 && size_y == 1 )
				{
					if( (*can_walk_func)(nx+1, ny+1) )
						gen_child(n, nx+1, ny+1);
				}
				else
				{
					x = nx + 1;
					y = ny + size_y;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					x = nx + size_x;
					y = ny + 1;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last y will > MAX_WORLD_Y_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( w == 0 && h == 0)
						gen_child(n, nx+1, ny+1);
				}
			}
			break;

		case DIR_S:
			// checks the S direction
			if( ny + size_y < MAX_WORLD_Y_LOC )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny+1][nx] - node_count_base) > 0 )
#else
				if( node_matrix[ny+1][nx] > 0 )
#endif
				{
					gen_child(n, nx, ny+1);
				}
				else if( size_x == 1 )
				{
					if( (*can_walk_func)(nx, ny+1) )
						gen_child(n, nx, ny+1);
				}
				else
				{
					x = nx;
					y = ny + size_y;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					if( w == 0 )
						gen_child(n, nx, ny+1);
				}
			}
			break;

		case DIR_SW:
			// checks the SW direction
			if( ny + size_y < MAX_WORLD_Y_LOC && nx > 0 )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny+1][nx-1] - node_count_base) > 0 )
#else
				if( node_matrix[ny+1][nx-1] > 0 )
#endif
				{
					gen_child(n, nx-1, ny+1);
				}
				else if( size_x == 1 && size_y == 1 )
				{
					if( (*can_walk_func)(nx-1, ny+1) )
						gen_child(n, nx-1, ny+1);
				}
				else
				{
					x = nx - 1;
					y = ny + size_y;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					x = nx - 1;
					y = ny + 1;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last y will > MAX_WORLD_Y_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( w == 0 && h == 0)
						gen_child(n, nx-1, ny+1);
				}
			}
			break;

		case DIR_W:
			if( nx > 0 )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny][nx-1] - node_count_base) > 0 )
#else
				if( node_matrix[ny][nx-1] > 0 )
#endif
				{
					gen_child(n, nx-1, ny);
				}
				else if( size_y == 1 )
				{
					if( (*can_walk_func)(nx-1, ny) )
						gen_child(n, nx-1, ny);
				}
				else
				{
					x = nx - 1;
					y = ny;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last y will > MAX_WORLD_Y_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( h == 0)
						gen_child(n, nx-1, ny);
				}
			}
			break;

		case DIR_NW:
			// checks the NW direction
			if( ny > 0 && nx > 0 )
			{
				// if a node has been created, that mean it is not occupied
#ifdef USE_NODE_COUNT_BASE
				if( (node_matrix[ny-1][nx-1] - node_count_base) > 0 )
#else
				if( node_matrix[ny-1][nx-1] > 0 )
#endif
				{
					gen_child(n, nx-1, ny-1);
				}
				else if( size_x == 1 && size_y == 1 )
				{
					if( (*can_walk_func)(nx-1, ny-1) )
						gen_child(n, nx-1, ny-1);
				}
				else
				{
					x = nx - 1;
					y = ny - 1;
					w = size_x;
					err_when( x + w > MAX_WORLD_X_LOC );		// last x will > MAX_WORLD_X_LOC
					for( ; w > 0 && (*can_walk_func)(x, y); --w, ++x );
					x = nx - 1;
					y = ny - 1;
					h = size_y;
					err_when( y + h > MAX_WORLD_Y_LOC );		// last y will > MAX_WORLD_Y_LOC
					for( ; h > 0 && (*can_walk_func)(x, y); --h, ++y );
					if( w == 0 && h == 0)
						gen_child(n, nx-1, ny-1);
				}
			}
			break;

		default:
			err_here();
		}	// end switch
	} // end for
}
// ---end of function PathFinder::gen_childs---/////


// ---begin of function PathFinder::gen_child---/////
void PathFinder::gen_child(PathNode* n, int x, int y)
{
	// returns if parent is the same as node
	if (n->parent && n->parent->x == x && y == n->parent->y)
		return;
	//--- increments cost
	int g = n->g + 1;
//	short c;
	PathNode* old;
	PathNode* succ;
	
	// -------- check if a node already found on the map --------//

	int tempRecNum;
#ifdef USE_NODE_COUNT_BASE
	if ((tempRecNum = node_matrix[y][x] - node_count_base) > 0)
#else
	if ((tempRecNum = node_matirix[y][x]) > 0)
#endif
	{
		old = node_arr+tempRecNum -1;

		//--- checks if the node is in the open list ---//

		if( !old->closed_flag )
		{
//			for(c = 0; c< MAX_CHILD_NODE && n->child[c]; c++);
//				n->child[c] = old;
			n->child[n->child_count++] = old;
			err_when( n->child_count > MAX_CHILD_NODE );

			if (g < old->g)
			{
				old->parent = n;
				old->g = g;
				old->f = g+ old->h;
				// if (old->child[0])
#ifdef USE_PRIORITY_QUEUE
				// propagate_down does not support updating priority queue
#else
				if (old->child_count)				// seems no need to propagate_down (Gilbert)
					propagate_down(old);
#endif
			}
			else if (g == old->g)
			{
				//---- improvement so that it might take a path with the same cost as the one being calculated, even though it might not improve (nor worsen) the cost
//				if (misc.rand()%2)
//				{
//					old->parent = n;
//
//					if (old->child[0])
//						propagate_down(old);
//	
//				}
			}
		}
		else // if (old = get_from_list(x, y, closed)) //if the node is in the closed list
		{
//			for(c = 0; c< MAX_CHILD_NODE && n->child[c]; c++);
//				n->child[c] = old;
			n->child[n->child_count++] = old;
			err_when( n->child_count > MAX_CHILD_NODE );

			if (g < old->g)
			{
				old->parent = n;
				old->g = g;
				old->f = g+ old->h;
				// if cost was lower, propagate down to ensure correct operation
				propagate_down(old);
			}
			else if (g == old->g)
			{
				//--- impprovement so that the pathfinder might consider equally good paths of the same cost
//				if (misc.rand()%2)
//				{
//					old->parent = n;
//					propagate_down(old);
//				}
			}
		}
	}
	else //if the node is not processed yet, gen it !
	{
		err_when( node_count >= MAX_NODE_NUM);
		succ = node_arr + node_count++;

		memset(succ, 0, sizeof(PathNode));

		succ->parent = n;
		// succ->g = g;
		//succ->h = MAX(abs(x-dest_x), abs(y-dest_y));
		// succ->h =(x-dest_x)*(x-dest_x)+(y-dest_y)*(y-dest_y);
		// succ->h = pfind_dist( x, y );
		succ->f = (succ->g = g) + (succ->h = pfind_dist(x, y));
		succ->x = x;
		succ->y = y;
#ifdef USE_NODE_COUNT_BASE
		node_matrix[y][x] = node_count + node_count_base;		// node_count has been increased
		err_when( node_count_base + node_count > SAFE_NODE_COUNT_BASE );
//		if( node_count_base + node_count >= SAFE_NODE_COUNT_BASE )	// assume reset() can prevent
//			normalize_node_count_base();
#else
		node_matrix[y][x] = node_count;
#endif
		insert(succ);
//		for(c = 0; c< MAX_CHILD_NODE && n->child[c]; c++);
//			n->child[c] = succ;
		n->child[n->child_count++] = succ;
	}
}
// ---end of function PathFinder::gen_child---/////

// ---begin of function PathFinder::get_from_list---/////
PathNode* PathFinder::get_from_list(int x, int y, PathNode* list)
{
	PathNode* tmp = list;
	while (tmp)
	{
		if (tmp->x == x && tmp->y == y)
			return tmp;
		else
			tmp = tmp->next;
	}
	return NULL;
}
// ---end of function PathFinder::get_from_list---/////

// ---begin of function PathFinder::get_cloest_node---/////
PathNode* PathFinder::get_closest_node()
{
	//--- this function is generally called after pathfinding has failed, so that the cloest node that can be reached can be found
	PathNode* nodePointer;
	PathNode* shortestPathNodeSoFar = NULL;
	// initialised shortestDistance to a large number first
	int shortestDistance; // dist;
	short shortestG;
	
	//destX = dest_x - sour_x;
	//destY = dest_y - sour_y;
	// dist = destX*destX+destY*destY;
	// dist = pfind_dist( sour_x, sour_y );
	shortestDistance = pfind_close_dist(node_arr->x, node_arr->y);		//	root node distance
	shortestG = node_arr->g;
	shortestPathNodeSoFar = node_arr;

	int dist;

#ifdef USE_PRIORITY_QUEUE
	// first examines the open list
	for( int q = low_queue; q <= high_queue; ++q )
	{
		nodePointer = open[q];
		while (nodePointer)
		{
			dist = pfind_close_dist( nodePointer->x, nodePointer->y );
			// also take g into account, if h are equal
			if (dist < shortestDistance
				|| dist == shortestDistance && nodePointer->g < shortestG )
			// ########## begin Gilbert 31/5 #########//
//			bool replaceFlag;
//			if( dist <= 1 )		// if it can reach next, it handle specially
//			{
//				if( shortestDistance <= 1 )
//					replaceFlag = nodePointer->g < shortestG;
//				else
//					replaceFlag = true;
//			}
//			else
//			{
//				if( shortestDistance <= 1 )
//					replaceFlag = false;
//				else
//					replaceFlag = H_TO_G_RATIO*dist+nodePointer->g < H_TO_G_RATIO*shortestDistance+shortestG;
//				// avoid take a very long path to get only a little closer
//				// if one path take  2 steps to get distance 3
//				// and one path take 7 steps to get distance 2
//				// take the first one
//			}
//			if( replaceFlag )
			// ########## end Gilbert 31/5 #########//
			{
				shortestDistance = dist;
				shortestG = nodePointer->g;
				shortestPathNodeSoFar = nodePointer;
			}
			nodePointer = nodePointer->next;
		}
	}
	// then examines the closed list
	nodePointer = closed;
	while (nodePointer)
	{
		dist = pfind_close_dist( nodePointer->x, nodePointer->y );
		// also take g into account, if h are equal
		if (dist < shortestDistance
			|| dist == shortestDistance && nodePointer->g < shortestG )
		{
			shortestDistance = dist;
			shortestG = nodePointer->g;
			shortestPathNodeSoFar = nodePointer;
		}
		nodePointer = nodePointer->next;
	}
#else
	// first examines the open list,
	// then examines the closed list
	for (int i = 0, nodePointer = open; i < 2; i++, nodePointer= closed)
	{
		while (nodePointer)
		{
			dist = pfind_close_dist( nodePointer->x, nodePointer->y );
			// also take g into account, if h are equal
			if (dist < shortestDistance
				|| dist == shortestDistance && nodePointer->g < shortestG )
			{
				shortestDistance = dist;
				shortestG = nodePointer->g;
				shortestPathNodeSoFar = nodePointer;
			}
			nodePointer = nodePointer->next;
		}
	}
#endif
	return shortestPathNodeSoFar;
}
// ---end of function PathFinder::get_cloest_node---/////

// ---begin of function PathFinder::propagate_down---/////
void PathFinder::propagate_down(PathNode* node)
{
	int c, g = node->g;
	PathNode* child, *father;

	for (c = 0; c < 8; c++)
   {
   	if ((child = node->child[c]) == NULL )   
			//------- create alias for faster access.
          break;
		if (g+1 < child->g)
		{
	   	 child->g = g+1;
	   	 child->f = child->g + child->h;
	  	    child->parent = node;           // reset parent to new path.
	       push(child);                 // Now the Child's branch need to be
		}     // checked out. Remember the new cost must be propagated down.
   }

	while(stack_pos)
	{
		// the father is found by popping the stack
		father = pop();
		for (c = 0; c<8; c++)
		{
	   	if ( (child = father->child[c]) == NULL ) 
	  			break;
	      if ( father->g+1 < child->g ) // there are no children, or that the g value of
	  		{                          // the child is equal or better than the cost we're propagating
	      	child->g = father->g+1;
	    		child->f = child->g+child->h;
	    		child->parent = father;
	    		push(child);  //--- push child onto the stack
			}
	   }
	}
}
// ---end of function PathFinder::propagate_down---/////

// ---begin of function PathFinder::push---/////
void PathFinder::push(PathNode* n)
{
	stack[stack_pos++] = n;
	err_when(stack_pos >= MAX_STACK_NUM);
}
// ---end of function PathFinder::push --/////

// ---begin of function PathFinder: pop-///
PathNode* PathFinder::pop()
{
	return stack[--stack_pos];
}
//--end of function PathFinder::pop ---///

// ---begin of function PathFinder: insert-----///
void PathFinder::insert(PathNode* succ)
{
	err_when( succ->closed_flag != 0 );		// assume closed_flag is 0 to save a memory write

#ifdef USE_PRIORITY_QUEUE
	insert_priority_queue(succ);
#else
	if (!open)
	{
		open = succ;
		return;
	}
	int f = succ->f;
	// ##### begin Gilbert 29/4 #######//
	// come late should put at the front of the link list
//	if (!open || open->f > f)
	if (!open || open->f >= f)
	// ##### end Gilbert 29/4 #######//
	{
		succ->next = open;
		open = succ;
		return;
	}
	PathNode* temp1, *temp2;
	temp1 = open;
	temp2 = open->next;
	while (temp2 && (temp2->f < f))
	{
		temp1 = temp2;
		temp2 = temp2->next;
	}
	succ->next = temp2;
	temp1->next = succ;
#endif
} 

//--- End of function PathFinder: insert-----//




#ifdef USE_PRIORITY_QUEUE
static void insert_priority_queue(PathNode *n)
{
	register int q = n->f;

	err_when( q < 0 );
	if( q < MAX_F )
	{
		// update low_queue, high_queue
		if( q < low_queue )
			low_queue = q;
		if( q > high_queue )
			high_queue = q;

		// push at the front of queue(stack?)
		n->next = open[q];
		open[q] = n;
	}
	else
	{
		int f = q;		// real n->f
		q = MAX_F;
		// update low_queue, high_queue
		if( q < low_queue )
			low_queue = q;
		if( q > high_queue )
			high_queue = q;

		// come late should put at the front of the link list
		if( !open[q] || open[q]->f >= f )
		{
			// add at the begining of a queue
			n->next = open[q];
			open[q] = n;
		}
		else
		{
			// add at middle or end of queue
			PathNode* temp1, *temp2;
			temp1 = open[q];
			temp2 = open[q]->next;
			while (temp2 && (temp2->f < f))
			{
				temp1 = temp2;
				temp2 = temp2->next;
			}
			n->next = temp2;
			temp1->next = n;
		}
	}

	err_when( low_queue < 0 );
	err_when( high_queue > MAX_F );
}


static PathNode* remove_head_priority_queue()
{
	register int f;
	PathNode *rn = NULL;

	for( f = low_queue; f <= high_queue; ++f )
	{
		if( (rn = open[f]) )
		{
			open[f] = rn->next;

			// skip 
			// open[f] = rn->next;
			// may need to inc low_queue further, but avoid empty queue
			//while( !open[f] && low_queue <= high_queue )
			//{
			//	++low_queue;
			//}
			if( !(open[f] = rn->next) )		// remove the head of queue
			{
				// may need to inc low_queue further, but avoid empty queue
				low_queue = f+1;		// may need to inc low_queue further, but avoid null
			}
			else
			{
				low_queue = f;
			}
			break;
		}
	}

	return rn;
}
#endif
