// Filename    : OPFIND.H
// Description : Header file for the Pathfinding algorithm

#ifndef __OPFIND_H 
#define __OPFIND_H

#ifndef __ALL_H
#include <ALL.H>
#endif

#define MAX_CHILD_NODE 8  // maximum 8 directions
#define PATH_FIND_LOC_BUFFER_SIZE	(30*30)		// a buffer for backing up location data before overwritting them for searching

// parameter for searchFlags in set_attribute
enum
{
	PFIND_IGNORE_UNIT = 1,
};

//------- define SaveLocation -------//

struct SaveLocation	// for saving Location before they are modified for researching
{
	unsigned short loc_flag;
	short         cargo_recno;
};

//---- define struct Path -----//

struct PathResult
{
	short loc_x, loc_y;
};

//---- define struct PathNode ----//

struct PathNode  // private PathNode structure
{
	int 	f, h; 	// h = f + g as in A*
	short g;
	short x, y; 	// coordinates
	char	closed_flag;
	char	child_count;

	PathNode* parent; // parent of a node
	PathNode* child[MAX_CHILD_NODE]; // childs of a node
	PathNode* next; // next ptr
};

//-------- define class PathFinder -----//

class PathFinder
{
friend class PathReuse; 	// this is a friend class of PathFinder

public:
	PathFinder();
	~PathFinder();

	void init();
	void deinit();

	//----- functions for setting path search attributes -----//

	void set_attribute(short x, short y, char mobileType, int searchFlags);
	void set_handle_power_nation(int handlePowerFlag, char* nationPassable=NULL);

	//----- functions for finding paths and retrieve results ----//

	bool find_path(int sourceX, int sourceY, int destX, int destY,
		int destRangeX1, int destRangeY1, int destRangeX2, int destRangeY2, bool raiseIt=1, bool raiseDest = 1, int maxTries=0, int stopAtRange=0);

	PathResult* retrieve(int& pathCount, int& totalSteps);
	int			path_step();
	bool 			is_path_found();

	//------------------------------------//

private:

	bool 	find_path2(int sourceX, int sourceY, int destinationX, int destinationY, int maxTries, int stopAtRange);
	void 	reset(); 				// resets the pathfinder so that it's ready for next use
	PathNode* best_node(); 			// returns the best node so far
	void 	gen_childs(PathNode*); 	// generates all childs for the node
	void 	gen_child(PathNode*, int, int); // generates a child for the node
	PathNode* get_from_list(int x, int y, PathNode*); // gets node from open/closed list
	PathNode* get_closest_node(); 	// calls get_cloest node when no path was found
	void 	propagate_down(PathNode *); // propagate down the cost
	void 	insert(PathNode* succ); 	// insert a node to the list

	void 	push(PathNode* n); 		// pushes the stack with the given node
	PathNode* pop(); 					// pops the stack
	PathNode* return_path_node();  // returns the unprocessed path
};

extern PathFinder path_finder;
#ifdef DEBUG
extern unsigned long pathfind_profile_time;
extern unsigned long last_pathfind_profile_time;
// ######## begin Gilbert 27/4 #######//
extern unsigned long pathfind_count;
extern unsigned long last_pathfind_count;
extern unsigned long longest_pathfind_profile_time;
extern unsigned long last_longest_pathfind_profile_time;
extern unsigned long longest_pathfind_unit_recno;
extern unsigned long last_longest_pathfind_unit_recno;
// ######## end Gilbert 27/4 #######//
#endif

#endif