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

//Filename		:	opreuse.h
//Description	:	Header file of Object PathReuse
//Owner			:	Juliet
 
#ifndef __OPREUSE_H
#define __OPREUSE_H

#define MAX_REUSE_NODE_NUM_PER_UNIT 2000
#define MAX_REUSE_NUMBER_OF_UNITS	100

class UnitGroup;
class Unit;
struct PathResult;

//------------ Define Class PathReuse ----------//

class PathReuse
{
private:

	//----------Define class member struct ReuseNode----------//

	struct ReuseNode
	{
		short	 x, y;
		ReuseNode* parent;
		ReuseNode* next;
	};

	//----------Define private class member variables-------------//

	ReuseNode** cur_paths; //points to the tail of the path
	ReuseNode** cur_paths_head; //points to the head of the path, for smoothing purpose
	ReuseNode* reuse_node_arr; //reuse node array

	Unit** the_units;
	int n_units;	//number of units
	//bool they_can_be_deleted; //the unit should be deleted in the destructor

	static short being_used; //true if the PathReuse is being used

	//leader path: points to the head of the leader path
	//cur_paths[i]: points to the tail of the current path
	ReuseNode* leader_path; //the path of the leader

	//for the leader only
	static bool leader_path_found;
	static int leader_path_node_count;

	// for all people
	int reuse_node_count;
	short *size_x, *size_y;
	int start_offset_x;
	int start_offset_y;
	int dest_offset_x;
	int dest_offset_y;

	//-------------------------------------------------//

public:

	PathReuse();
	~PathReuse();

	void find_paths();
	void reset();
	void init(UnitGroup* aGroup);
	void init(Unit** aGroup, int aNumber);
	void set_handle_power_nation(int handlePowerFlag, char* nationPassable=NULL, int nationRecno = 0);

private:
	bool find_leader_path(int sx, int sy, int dx, int dy);
	bool find_other_path(int sx, int sy, int dx, int dy, int i);
	void set_offset(int sx, int sy, int dx, int dy, int i);
	void offset_path(int sx, int sy, int dx, int dy, int i);
	void join_offset_path(int sx, int sy, int dx, int dy, int i);
	bool do_normal_path_find(int sx, int sy, int dx, int dy, int i);
	bool check_occupancy(ReuseNode* node, int i);
	bool check_this_location_occupied(int placex, int placey, int i);
	void de_occupy_destination(int sx, int sy, int rx1, int ry1, int rx2, int ry2, int i);
	void re_occupy_destination(int sx, int sy, int rx1, int ry1, int rx2, int ry2, int i);
	ReuseNode* search_first_unblocked_node(ReuseNode*& blocked_node, int i);
	ReuseNode* find_path_to_unblocked_node(ReuseNode* blocked_node, ReuseNode* unblocked_node, int i);
	ReuseNode* search_last_unblocked_node(ReuseNode* blocked_node, int i);
	ReuseNode* search_next_unblocked_from_last_blocked(ReuseNode* blocked_node, ReuseNode* node, int i);
	PathResult* retrieve(int& pathCount, int& totalSteps, int i);
	//smoothing our path
	void remove_path_to_duplicate_nodes(int i);
	void smoothing_path(int i);
};
//-------------------- End class PathReuse --------------------//
#ifdef DEBUG
extern unsigned long path_reuse_profile_time;
extern unsigned long last_path_reuse_profile_time;
#endif

extern PathReuse path_reuse;

#endif