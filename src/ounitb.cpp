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

//Filename	 : OUNITB.CPP
//Description: Functions in the UnitB class

#include <windows.h>
#include <opfind.h>
#include <onation.h>
#include <ounitres.h>
#include <ounit.h> 
#include <oconfig.h>
#include <itemid.h>
#include <olocate.h>
#include <ogodres.h>

#define	MAX_RETRY_STATE	5
static short move_x_pixel_array[] = { 0, LOCATE_WIDTH, LOCATE_WIDTH, LOCATE_WIDTH, 0, -LOCATE_WIDTH, -LOCATE_WIDTH, -LOCATE_WIDTH };
static short move_y_pixel_array[] = { -LOCATE_HEIGHT, -LOCATE_HEIGHT, 0, LOCATE_HEIGHT, LOCATE_HEIGHT, LOCATE_HEIGHT, 0, -LOCATE_HEIGHT };


//--------- Begin of function UnitB::UnitB ---------//
//
UnitB::UnitB()
{
	memset( sizeof(Sprite) + (char *)this, 0, sizeof(UnitB) - sizeof(Sprite) );
}
//----------- End of function UnitB::UnitB -----------//


//--------- Begin of function UnitB::~UnitB ---------//
//
UnitB::~UnitB()
{
	if(cur_path)
		mem_del(cur_path);
	cur_path = NULL;
}
//----------- End of function UnitB::~UnitB -----------//


//--------- Begin of function UnitB::move_to ---------//
//
// Move a unit to a specified location
//
// <int>  destXLoc - the x-coordinate of the destination
// <int>  destYLoc - the y-coordinate of the destination
// <bool> raiseDest - default is true, if not want to raise destination, false
// 
void UnitB::move_to(int destXLoc, int destYLoc, bool raiseDest)
{
	// ###### begin Gilbert 22/3 #######//
	// other people may ask this unit to move away while it is dead

	if( hit_points <= 0 || cur_action == SPRITE_DIE )
		return;
	// ###### end Gilbert 22/3 #######//

//	err_when(destXLoc < 0 || destXLoc >= MAX_WORLD_X_LOC || destYLoc < 0 || destYLoc >= MAX_WORLD_Y_LOC);
	destXLoc = min(destXLoc, MAX_WORLD_X_LOC-1);
	destXLoc = max(destXLoc, 0);
	destYLoc = min(destYLoc, MAX_WORLD_Y_LOC-1);
	destYLoc = max(destYLoc, 0);

	stop_move();

	//--- save the destination for later possible retries in searching when it is blocked ----//

	move_to_loc_x = destXLoc;
	move_to_loc_y = destYLoc;

	if((destXLoc == next_x_loc()) && (destYLoc == next_y_loc()))
		return;

	//------- search the path ---------//

//#ifdef DEBUG
//	unsigned long pathFinderStartTime = m.get_time();
//#endif
	path_finder.set_attribute(loc_width, loc_height, mobile_type, 0);

	// ###### begin Gilbert 31/5 #######//
	err_when( cur_order.mode == UNIT_ATTACK && !cast_to_Unit() );
	int stopAtRange = cur_order.mode == UNIT_ATTACK ? cast_to_Unit()->attack_range()-1 : 0;
	// subtract one attack_range so close attack can search near the target
	// and range attack search a little closer
	// ###### end Gilbert 31/5 #######//

	//----- code for handle nation power option -----//

	bool handlePowerNationFlag = false;

	path_finder.set_handle_power_nation(0);

	if( nation_recno )
	{
		Nation *nationPtr = nation_array[nation_recno];

		int srcLocPowerNation  = world.get_loc(next_x_loc(), next_y_loc())->power_nation_recno;
		int destLocPowerNation = world.get_loc(destXLoc, destYLoc)->power_nation_recno;

		//--- only if unit stands on a passable location and the destination is passable, we handle power nation ---//

		if( (!srcLocPowerNation  || nationPtr->get_relation_passable(srcLocPowerNation)) &&
			 (!destLocPowerNation || nationPtr->get_relation_passable(destLocPowerNation)) )
		{
			path_finder.set_handle_power_nation(1, nationPtr->relation_passable_array );
			handlePowerNationFlag = true;
		}
	}

	//--------- find the path now ---------//

	int rangeX1, rangeY1, rangeX2, rangeY2;

	get_object_range(destXLoc, destYLoc, rangeX1, rangeY1, rangeX2, rangeY2);

	// ######## begin Gilbert 31/5 ##########//
	path_finder.find_path(next_x_loc(), next_y_loc(), destXLoc, destYLoc,
								 rangeX1, rangeY1, rangeX2, rangeY2, 1, raiseDest,
								 0, stopAtRange );
	// ######## end Gilbert 31/5 ##########//

	if( path_finder.is_path_found() )
	{
		set_no_longer_blocked();

		seek_path_fail_count=0;		// reset the failure counter
	}
	else	//---- path searching failed ----//
	{
		if(!is_blocked())
			set_not_seriously_blocked();

		//--- if the path search failed, increase the failure counter ---//

//		if( seek_path_fail_count < 100 )		// prevent numeric overflow
//			seek_path_fail_count++;
	}

	//-----------------------------------------//

	int pathNodeCount;
	int totalSteps;			// total number of steps to walk through the whole path.

	if( cur_path )
		mem_del( cur_path );

	cur_path = path_finder.retrieve(pathNodeCount, totalSteps);  // retrieves the path

	// ###### begin Gilbert 11/5 #########//
	if( cur_path )
	{
		cur_path_result_id = pathNodeCount;		// start with the last node
		steps_remaining = totalSteps;
	}
	else
	{
		cur_path_result_id = 0;
		steps_remaining = 0;
	}
	// ###### end Gilbert 11/5 #########//

//#ifdef DEBUG
//	// ######## begin Gilbert 27/4 #######//
//	pathFinderStartTime = m.get_time() - pathFinderStartTime;
//	pathfind_profile_time += pathFinderStartTime;
//	pathfind_count++;
//	if( pathFinderStartTime > longest_pathfind_profile_time )
//	{
//		longest_pathfind_profile_time = pathFinderStartTime;
//		longest_pathfind_unit_recno = sprite_recno;
//	}
//	// ######## begin Gilbert 27/4 #######//
//#endif

	if(!cur_path)
	{
		if(handlePowerNationFlag)
		{
			// ##### begin Gilber 27/4 ########//
//#ifdef DEBUG
			// unsigned long pathFinderStartTime = m.get_time();
			// pathFinderStartTime = m.get_time();
//#endif
			// ##### end Gilber 27/4 ########//
			// ###### begin Gilbert 12/5 ########//
			// path_finder.set_attribute(loc_width, loc_height, mobile_type, PFIND_IGNORE_UNIT );
			// ###### end Gilbert 12/5 ########//
			path_finder.set_handle_power_nation(0);
			handlePowerNationFlag = false;

			// ######## begin Gilbert 31/5 ##########//
			path_finder.find_path(next_x_loc(), next_y_loc(), destXLoc, destYLoc,
								 rangeX1, rangeY1, rangeX2, rangeY2, 1, raiseDest,
								 0, stopAtRange );
			// ######## end Gilbert 31/5 ##########//

			if( path_finder.is_path_found() )
			{
				set_no_longer_blocked();

				seek_path_fail_count=0;		// reset the failure counter
			}
			else	//---- path searching failed ----//
			{
				if(!is_blocked())
					set_not_seriously_blocked();
			}
			if( cur_path )
				mem_del( cur_path );

			cur_path = path_finder.retrieve(pathNodeCount, totalSteps);  // retrieves the path

			// ###### begin Gilbert 11/5 #########//
			if( cur_path )
			{
				cur_path_result_id = pathNodeCount;		// start with the last node
				steps_remaining = totalSteps;
			}
			else
			{
				cur_path_result_id = 0;
				steps_remaining = 0;
			}
			// ###### end Gilbert 11/5 #########//
//#ifdef DEBUG
//			// ######## begin Gilbert 27/4 #######//
//			pathFinderStartTime = m.get_time() - pathFinderStartTime;
//			pathfind_profile_time += pathFinderStartTime;
//			pathfind_count++;
//			if( pathFinderStartTime > longest_pathfind_profile_time )
//			{
//				longest_pathfind_profile_time = pathFinderStartTime;
//				longest_pathfind_unit_recno = sprite_recno;
//			}
//			// ######## begin Gilbert 27/4 #######//
//#endif
			if(!cur_path)
			{
				if(!handle_blocking())
					set_no_longer_blocked();	
				if(seek_path_fail_count <= 100*SEEK_PATH_FAIL_INCREMENT)
					seek_path_fail_count+=SEEK_PATH_FAIL_INCREMENT;		// 10 is the single increment unit. It will be reduce by 1 each time in Unit::attack()
			}
		}
		else
		{
			if(!handle_blocking())
				set_no_longer_blocked();
			if(seek_path_fail_count <= 100*SEEK_PATH_FAIL_INCREMENT)
				seek_path_fail_count+=SEEK_PATH_FAIL_INCREMENT;
		}
	}

	//--------- start the first move ---------//

	if( cur_path && cur_action != SPRITE_MOVE )			// if it is previously moving, don't call next_move() as it has still yet to finish its remaining pixel move to snap to grids
	{
		cur_action = SPRITE_MOVE;
		cur_frame = 1;
		retry_state = 0;
	}
	else
	{
		// ###### begin Gilbert 15/5 #########//
		if( cur_x == next_x && cur_y == next_y )
		{
			wait_count = 0;
			set_wait(); 
		}
		// ###### end Gilbert 15/5 #########//
	}
}
//----------- End of function UnitB::move_to -----------//


//--------- Begin of function UnitB::process_move --------//
// process unit movement
//
void UnitB::process_move()
{
	//----- if the sprite has reach the destintion ----//

	if( cur_x==go_x && cur_y==go_y )
	{
		err_when( cur_x!=next_x || cur_y!=next_y );	// if the unit reach its destination, then	cur_? == next_? == go_?

		if( cur_path_result_id > 0)		// 0 means it has completed all its move
		{
			next_move();

			if( cur_action != SPRITE_MOVE )     // if next_move() is not successful, the movement has been stopped
				return;

			if(cur_action==SPRITE_MOVE && cur_x==go_x && cur_y==go_y)
				next_move();
		}
	}

	err_when(cur_x-next_x!=0 && cur_y-next_y!=0 && abs(next_x-cur_x)!=abs(next_y-cur_y));

	//------- call Sprite::process_move --------//

	Sprite::process_move();

	err_when( cur_x < 0 || cur_y < 0 || cur_x >= MAX_WORLD_X_LOC * LOCATE_WIDTH || cur_y >= MAX_WORLD_Y_LOC * LOCATE_HEIGHT );
	//---- reset the wait counter when the unit moves ----//

	wait_count = 0;
}
//---------- End of function UnitB::process_move ----------//

//---------- Begin of function UnitB::process_blocked -----//
// 
// retry move to destination if the unit is blocked
//
void UnitB::process_blocked()
{
	if(number_of_times_being_blocked++ > MAX_UNIT_BLOCKED_TIME  || 
		(next_x_loc() == move_to_loc_x && next_y_loc() == move_to_loc_y) || 
		((abs(move_to_loc_x - next_x_loc()) <= obj_loc_width()) && 
		 (abs(move_to_loc_y - next_y_loc()) <= obj_loc_height()) && 
		 !can_move(move_to_loc_x, move_to_loc_y) &&
		 !checking_who_occupy_the_place_i_want_to_go(move_to_loc_x, move_to_loc_y)))
	{
		set_no_longer_blocked();
		stop_move();
	}
	else
		move_to(move_to_loc_x, move_to_loc_y);
}
//--------- End of function UnitB::process_blocked ------//

//--------- Begin of function UnitB::next_move ---------//
//
//	If there is unprocessed node(s) in the result_node_array,
// then next unprocessed node will be set to be the next location
// to move to. (i.e. go_? = location of the unprocessed node)
//
void UnitB::next_move()
{
	if( cur_path == NULL || !cur_path_result_id )
		return;

	//------------ all nodes are visited --------------//

	err_when(cur_x!=next_x || cur_y!=next_y);

	set_idle();

	//---- order the unit to move to the next checkpoint following the path ----//

	PathResult* pathResult = cur_path + cur_path_result_id - 1;
	cur_path_result_id--;

	err_when( pathResult->loc_x == cur_x_loc() && pathResult->loc_y == cur_y_loc() );

	sprite_move( pathResult->loc_x * LOCATE_WIDTH, pathResult->loc_y * LOCATE_HEIGHT );

	err_when(cur_x==go_x && cur_y==go_y && (cur_x!=next_x || cur_y!=next_y));
}
//----------- End of function UnitB::next_move -----------//


//------------- Begin of function UnitB::set_idle --------------//
// set parameters for unit idle
//
void UnitB::set_idle()
{
	err_when(cur_x<0);
	err_when(cur_x!=next_x || cur_y!=next_y || next_x!=go_x || next_y!=go_y);
	err_when(cur_x%LOCATE_WIDTH || cur_y%LOCATE_HEIGHT);

	go_x = next_x;
	go_y = next_y;

	cur_action = SPRITE_IDLE;
}
//----------------- End of function UnitB::set_idle ----------------//


//------------- Begin of function UnitB::set_wait --------------//
//
// set parameters for unit idle
//
void UnitB::set_wait()
{
	set_cur(next_x, next_y);   // align the sprite to 32x32 location when it stops
	
	cur_action = SPRITE_WAIT;
	cur_frame  = 1;
	wait_count++;		// this will be reset when the unit moves
}
//----------------- End of function UnitB::set_wait ----------------//


//------------- Begin of function UnitB::set_turn --------------//
// set parameters for unit turning
//
void UnitB::set_turn()
{
	err_when(cur_x<0);
	err_when(next_x!=cur_x || next_y!=cur_y);

	cur_action = SPRITE_TURN;
}
//----------------- End of function UnitB::set_turn ----------------//

//--------- Begin of function UnitB::set_next --------//
//
//	set the next coordinates to move to
//
// <int> newNextX, newNextY	- next coordinate to move to
// <int> para						- used to count the result_path_dist
// <int> ignoreBlockCheck		- whether ignore blocking check or not
//
int UnitB::set_next(int newNextX, int newNextY, int para, int ignoreBlockCheck)
{
	static char callingCount=0;

	int curNextXLoc = next_x_loc();
	int curNextYLoc = next_y_loc();
	int newNextXLoc = newNextX >> ZOOM_X_SHIFT_COUNT;
	int newNextYLoc = newNextY >> ZOOM_Y_SHIFT_COUNT;
	//------begin Juliet 14/10-----//
	int tempEndX = newNextXLoc+obj_loc_width()-1;
	int tempEndY = newNextYLoc+obj_loc_height()-1;
	//------end Juliet 14/10-------//
	short w, h, x, y;

	callingCount++;          // this function is being called, prevent recursive calling

	if(curNextXLoc!=newNextXLoc || curNextYLoc!=newNextYLoc)
	{
		if(!is_dir_correct())
		{
			set_turn();
			return 1;
		}
	}

	//----------- blocking check ---------//

	if( !ignoreBlockCheck )
	{
		int isBlocked=0;

		for(h=0, y=newNextYLoc; h<loc_height && !isBlocked ; h++, y++)
		{
			for(w=0, x=newNextXLoc; w<loc_width; w++, x++)
			{
				Location* locPtr = world.get_loc(x,y);

				if( locPtr->can_move(mobile_type) )
					continue;

				//--- this location is occupied by the unit itself ---//

				if( locPtr->unit_recno(mobile_type) == sprite_recno )
					continue;

				isBlocked = 1;
				break;
			}
		}

		if( isBlocked )
		{
			//----- if we have already reach the destination -----//

			
			int rangeX1, rangeY1, rangeX2, rangeY2;

			get_object_range(move_to_loc_x, move_to_loc_y, rangeX1, rangeY1, rangeX2, rangeY2);

			if( m.is_touch( rangeX1, rangeY1, rangeX2, rangeY2,
								 newNextXLoc, newNextYLoc, newNextXLoc+loc_width-1, newNextYLoc+loc_height-1 ) )
			{
				stop_move();
			}
			else //------- otherwise wait ---------//
			{
				set_wait();
			}
			
			callingCount--;
			return 0;
		}
	}

	//--------- set the sprite_recno in new locations -----------//

	for(h=0, y=curNextYLoc; h<loc_height; h++, y++)
	{
		for(w=0, x=curNextXLoc; w<loc_width; w++, x++)
			world.get_loc(x,y)->remove_unit(mobile_type);
	}

	for(h=0, y=newNextYLoc; h<loc_height; h++, y++)
	{
		for(w=0, x=newNextXLoc; w<loc_width; w++, x++)
			world.get_loc(x,y)->set_unit(sprite_recno,mobile_type);
	}

	//----- set the new next coordinate ------//

	next_x = newNextX;
	next_y = newNextY;

	callingCount--;

	if( steps_remaining > 0 )		// steps_remaining can be decreased to negative as set_next() is called when stopping a unit, which is an addition to the steps it is supposed to move 
		steps_remaining--;

	// --------- explore map ---------//

	if( cast_to_Unit() )
		cast_to_Unit()->explore_on_move(curNextXLoc, curNextYLoc, newNextXLoc, newNextYLoc );

	return 1;
}
//---------- End of function UnitB::set_next ----------//

//--------- Begin of function UnitB::stop_move ---------//
//
void UnitB::stop_move()
{
	switch( cur_action )
	{
		//----- if the unit is moving right now, ask it to stop as soon as possible -----//

		case SPRITE_READY_TO_MOVE:
			set_idle();
			break;

		case SPRITE_TURN:
		case SPRITE_WAIT:
			go_x = next_x;
			go_y = next_y;
			final_dir = cur_dir;
			turn_delay = 0;
			wait_state = 0;
			set_idle();
			break;

		case SPRITE_MOVE:
			go_x = next_x;
			go_y = next_y;
			if(cur_x==next_x && cur_y==next_y)
				set_idle();
			break;

		//--- if its current action is SPRITE_ATTACK, stop immediately ---//

		case SPRITE_ATTACK:
			set_next(cur_x, cur_y, 0, 1);
			go_x = next_x;
			go_y = next_y;
			set_idle();
			cur_frame  = 1;
			break;
	}

	//-------- reset parameters --------//

	cur_path_result_id = 0;		// reset it so that if the mode is SPRITE_MOVE, it won't use the old path
}
//----------- End of function UnitB::stop_move -----------//

//--------- Begin of function UnitB::process_wait ---------//

void UnitB::process_wait()
{
	err_when(cur_action != SPRITE_WAIT);

	//move if the next tile is ready
	int stepMagn = move_step_magn();
	if(can_move((cur_x+stepMagn*move_x_pixel_array[final_dir])/LOCATE_WIDTH, (cur_y+stepMagn*move_y_pixel_array[final_dir])/LOCATE_WIDTH))
	{
		wait_state = 0;
		cur_action = SPRITE_MOVE;
		return;
	}
	else //find a new path if the thing occupying the next tile no longer move
	{
/*		int occUnitNum;
		Unit* occStuff;
		occUnitNum = checking_who_occupy_my_next_tile((cur_x+stepMagn*move_x_pixel_array[final_dir])/LOCATE_WIDTH, (cur_y+stepMagn*move_y_pixel_array[final_dir])/LOCATE_WIDTH);

		//##### begin trevor 21/1 #####//

		enum{ SEARCH_NEW_PATH_PROBABILITY = 10 };

		if( (!occUnitNum || (occUnitNum && (occStuff=unit_array[occUnitNum])
			 && (occStuff->cur_action != SPRITE_MOVE
			 && occStuff->cur_action != SPRITE_WAIT)))
			 || (SEARCH_NEW_PATH_PROBABILITY > m.random(100) && sprite_speed() <= occStuff->sprite_speed() && !is_in_formation()) )
		{
			wait_state = 0;
			move_to(move_to_loc_x, move_to_loc_y);
		//	retry_state++;
			return;
		}

		//##### end trevor 21/1 #####//*/
	}
	//find a new path if wait too long	
	// ####### begin Gilbert 28/4 ########//
	if(wait_state++ > (retry_state <= MAX_UNIT_WAIT_STATE ? retry_state+MAX_UNIT_WAIT_STATE_FIRST : MAX_UNIT_WAIT_STATE) )
	{
		wait_state = 0;

		int backupRetryState = retry_state;
		move_to(move_to_loc_x, move_to_loc_y);
		retry_state = backupRetryState + 1;		// restore and increase
	}
	// ####### end Gilbert 28/4 ########//
	//if retrying too many times, stop move
	if(retry_state > MAX_RETRY_STATE) 
	{
		stop_move();
		retry_state = 0;
	}
}
//----------- End of function UnitB::process_wait -----------//


//----------- Begin of function UnitB::process_idle----------//
//
// when a unit is idling, if it is block, process block
// if it is in formation but fnot yet reach destination, go there again
//
void UnitB::process_idle()
{
	cur_action = SPRITE_IDLE;
	if(is_blocked() && cur_order.mode != UNIT_ATTACK)
		process_blocked();
	else
	if(/*is_in_formation() && */
		is_blocked() &&
		((next_x_loc() != move_to_loc_x) || (next_y_loc() != move_to_loc_y)) &&
		can_move(move_to_loc_x, move_to_loc_y))

		move_to(move_to_loc_x, move_to_loc_y);
	
	else
		Sprite::process_idle();
}
//---------- End of function UnitB::process_idle--------------//

//--------- Begin of function UnitB::get_object_range ---------//
//
// Get the full range of the area occupied by the object.
//
// <int>  objXLoc, objYLoc - the location of the object
// <int&> rangeX1, rangeY1, rangeX2, rangeY2 - vars for returning the range
//
void UnitB::get_object_range(int objXLoc, int objYLoc, int &rangeX1, int &rangeY1, int &rangeX2, int &rangeY2)
{
	if(can_move(objXLoc, objYLoc))
	{
		rangeX1 = objXLoc;
		rangeY1 = objYLoc;
		rangeX2 = objXLoc + loc_width - 1;
		rangeY2 = objYLoc + loc_height - 1;
		return;
	}
	//---------------------------------------//

//	int* objLocFlag = (int*) mem_add_clear(loc_width*loc_height*sizeof(int));
//	int* objCargoRecno = (int*) mem_add_clear(loc_width*loc_height*sizeof(int));

//	int ind = 0;
//	while(is<obj_loc_width())
//	{
//		js = 0;
//		while(js < obj_loc_height())
//		{
//			locPtr = world.get_loc(objXLoc+is, objYLoc+js);
//			ind = is*loc_height+js;
//			if(mobile_type == UNIT_AIR)
//				objCargoRecno[ind] = locPtr->air_cargo_recno;
//			else
//				objCargoRecno[ind] = locPtr->cargo_recno;
//			objLocFlag[ind] = locPtr->loc_flag;
//			// BUGHERE : objLocFlag is not longer zero (Gilbert 26/4)
//			if(!objLocFlag[ind] && objCargoRecno[ind] == sprite_recno) //it is myself
//				objCargoRecno[ind] = 0;
//			js++;
//		}
//		is++;
//	}

	
#ifdef DEBUG
//	int checkCargo = 0;
//	int checkFlag = 0;
//	int index = 0;
//	while(!checkCargo && !checkFlag && index < (loc_width*loc_height))
//	{
//		checkCargo = objCargoRecno[index];
//		checkFlag = objLocFlag[index];
//		index++;
//	}
//	err_when(!checkCargo && !checkFlag);
#endif

	//------ scan up -------//

	if(loc_width == 1 && loc_height == 1)
	{
		Location *locPtr = world.get_loc(objXLoc, objYLoc);
		unsigned short objLocFlag = locPtr->loc_flag;
		short objCargoRecno = mobile_type == UNIT_AIR ? locPtr->air_cargo_recno : locPtr->cargo_recno;

		for( rangeY1=((objYLoc>=1)?(objYLoc-1):0) ; rangeY1>=0 ; rangeY1-- )
		{
			if((world.get_loc(objXLoc, rangeY1)->loc_flag) != objLocFlag ||
				(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, rangeY1)->air_cargo_recno != objCargoRecno) : (world.get_loc(objXLoc, rangeY1)->cargo_recno != objCargoRecno)))
			{
				rangeY1++;		// go back to the last one which is valid 
				break;
			}
		}

		//------ scan down -------//

		for( rangeY2=((objYLoc<MAX_WORLD_Y_LOC-1)?(objYLoc+1):(MAX_WORLD_Y_LOC-1)) ; rangeY2<MAX_WORLD_Y_LOC ; rangeY2++ )
		{
			if((world.get_loc(objXLoc, rangeY2)->loc_flag ) != objLocFlag ||
				(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, rangeY2)->air_cargo_recno != objCargoRecno) : (world.get_loc(objXLoc, rangeY2)->cargo_recno != objCargoRecno)))
			{
				rangeY2--;		// go back to the last one which is valid 
				break;
			}
		}

		//------ scan left -------//

		for( rangeX1=((objXLoc>=1)?(objXLoc-1):0) ; rangeX1>=0 ; rangeX1-- )
		{
			if((world.get_loc(rangeX1, objYLoc)->loc_flag ) != objLocFlag ||
				 (mobile_type == UNIT_AIR ? (world.get_loc(rangeX1, objYLoc)->air_cargo_recno != objCargoRecno) : (world.get_loc(rangeX1, objYLoc)->cargo_recno != objCargoRecno)))
			{
				rangeX1++;		// go back to the last one which is valid
				break;
			}
		}

		//------ scan right -------//

		for( rangeX2=((objXLoc<MAX_WORLD_X_LOC-1)?(objXLoc+1):(MAX_WORLD_X_LOC-1)) ; rangeX2<MAX_WORLD_X_LOC ; rangeX2++ )
		{
			if((world.get_loc(rangeX2, objYLoc)->loc_flag ) != objLocFlag ||
				 (mobile_type == UNIT_AIR ? (world.get_loc(rangeX2, objYLoc)->air_cargo_recno != objCargoRecno) : (world.get_loc(rangeX2, objYLoc)->cargo_recno != objCargoRecno)))
			{
				rangeX2--;		// go back to the last one which is valid
				break;
			}
		}
	}
	else //2 x 2  BUGHERE: only for 2x2 but not for other size other than 2x2 and 1x1 ( 3x3 may not work)
	{
		unsigned short objLocFlagTL, objLocFlagTR, objLocFlagBL, objLocFlagBR; // top left, top right, bottom ...
		short objCargoRecnoTL, objCargoRecnoTR, objCargoRecnoBL, objCargoRecnoBR ;	// top left, top right, bottom ...

		// get flag and cargo recno of top left corner
		Location *locPtr = world.get_loc(objXLoc, objYLoc);
		objLocFlagTL = locPtr->loc_flag;
		objCargoRecnoTL = mobile_type == UNIT_AIR ? locPtr->air_cargo_recno : locPtr->cargo_recno;

		// get flag and cargo recno of top right corner
		if( objXLoc+loc_width <= MAX_WORLD_X_LOC )
		{
			locPtr = world.get_loc(objXLoc+loc_width-1, objYLoc);
			objLocFlagTR = locPtr->loc_flag;
			objCargoRecnoTR = mobile_type == UNIT_AIR ? locPtr->air_cargo_recno : locPtr->cargo_recno;
		}
		else
		{
			objLocFlagTR = objLocFlagTL;
			objCargoRecnoTR = objCargoRecnoTL;
		}

		// get flag and cargo recno of bottom left corner
		if( objYLoc+loc_height <= MAX_WORLD_Y_LOC )
		{
			locPtr = world.get_loc(objXLoc, objYLoc+loc_height-1);
			objLocFlagBL = locPtr->loc_flag;
			objCargoRecnoBL = mobile_type == UNIT_AIR ? locPtr->air_cargo_recno : locPtr->cargo_recno;
		}
		else
		{
			objLocFlagBL = objLocFlagTL;
			objCargoRecnoBL = objCargoRecnoTL;
		}

		// get flag and cargo recno of bottom left corner
		if( objXLoc+loc_width <= MAX_WORLD_X_LOC && objYLoc+loc_height <= MAX_WORLD_Y_LOC )
		{
			locPtr = world.get_loc(objXLoc+loc_width-1, objYLoc+loc_height-1);
			objLocFlagBR = locPtr->loc_flag;
			objCargoRecnoBR = mobile_type == UNIT_AIR ? locPtr->air_cargo_recno : locPtr->cargo_recno;
		}
		else
		{
			objLocFlagBR = objLocFlagTL;
			objCargoRecnoBR = objCargoRecnoTL;
		}

		//it is very hard coded !
		//scan up//
		if(objCargoRecnoTL == objCargoRecnoTR && objLocFlagTL == objLocFlagTR)
		{
			if(objCargoRecnoTL && (objLocFlagTL & LOCATE_BLOCK_MASK))
			{
				for(rangeY1=((objYLoc>=1)?(objYLoc-1):0); rangeY1 >= 0; rangeY1--)
				{
					if((world.get_loc(objXLoc, rangeY1)->loc_flag ) != objLocFlagTL ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, rangeY1)->air_cargo_recno != objCargoRecnoTL) : (world.get_loc(objXLoc, rangeY1)->cargo_recno != objCargoRecnoTL)))
					{
						rangeY1++;
						break;
					}
				}
			}
			else
				rangeY1 = objYLoc;
		}
		else
		{
			int tmpRangeY1, tmpRangeY2;
			if(objCargoRecnoTL && (objLocFlagTL & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeY1=((objYLoc>=1)?(objYLoc-1):0); tmpRangeY1 >= 0; tmpRangeY1--)
				{
					if((world.get_loc(objXLoc, tmpRangeY1)->loc_flag ) != objLocFlagTL ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, tmpRangeY1)->air_cargo_recno != objCargoRecnoTL) : (world.get_loc(objXLoc, tmpRangeY1)->cargo_recno != objCargoRecnoTL)))
					{
						tmpRangeY1++;
						break;
					}
				}
			}
			else
				tmpRangeY1 = objYLoc;

			if(objCargoRecnoTR && (objLocFlagTR & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeY2=((objYLoc>=1)?(objYLoc-1):0); tmpRangeY2 >= 0; tmpRangeY2--)
				{
					if((world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->loc_flag ) != objLocFlagTR ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->air_cargo_recno != objCargoRecnoTR) : (world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->cargo_recno != objCargoRecnoTR)))
					{
						tmpRangeY2++;
						break;
					}
				}
			}
			else
				tmpRangeY2 = objYLoc;

			rangeY1 = min(tmpRangeY1, tmpRangeY2);
		}
		//scan down//
		if(objCargoRecnoBL == objCargoRecnoBR && objLocFlagBL == objLocFlagBR )
		{
			if(objCargoRecnoBL && (objLocFlagBL & LOCATE_BLOCK_MASK))
			{
				for(rangeY2=((objYLoc+loc_height<MAX_WORLD_Y_LOC)?objYLoc+loc_height:MAX_WORLD_Y_LOC-1); rangeY2<MAX_WORLD_Y_LOC; rangeY2++)
				{
					if((world.get_loc(objXLoc, rangeY2)->loc_flag ) != objLocFlagBL ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, rangeY2)->air_cargo_recno != objCargoRecnoBL) : (world.get_loc(objXLoc, rangeY2)->cargo_recno != objCargoRecnoBL)))
					{
						rangeY2--;
						break;
					}
				}
			}
			else
				rangeY2 = objYLoc+loc_height-1;
		}
		else
		{
			int tmpRangeY1, tmpRangeY2;
			if(objCargoRecnoBL && (objLocFlagBL & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeY1=((objYLoc+loc_height<MAX_WORLD_Y_LOC)?objYLoc+loc_height:MAX_WORLD_Y_LOC-1); tmpRangeY1<MAX_WORLD_Y_LOC; tmpRangeY1++)
				{
					if((world.get_loc(objXLoc, tmpRangeY1)->loc_flag ) != objLocFlagBL ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc, tmpRangeY1)->air_cargo_recno != objCargoRecnoBL) : (world.get_loc(objXLoc, tmpRangeY1)->cargo_recno != objCargoRecnoBL)))
					{
						tmpRangeY1--;
						break;
					}
				}
			}
			else
				tmpRangeY1 = objYLoc+loc_height-1;

			if(objCargoRecnoBR && (objLocFlagBR & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeY2=((objYLoc+loc_height<MAX_WORLD_Y_LOC)?objYLoc+loc_height:MAX_WORLD_Y_LOC-1); tmpRangeY2<MAX_WORLD_Y_LOC; tmpRangeY2++)
				{
					if((world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->loc_flag ) != objLocFlagBR ||
						(mobile_type == UNIT_AIR ? (world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->air_cargo_recno != objCargoRecnoBR) : (world.get_loc(objXLoc+loc_width-1, tmpRangeY2)->cargo_recno != objCargoRecnoBR)))
					{
						tmpRangeY2--;
						break;
					}
				}
			}
			else
				tmpRangeY2 = objYLoc+loc_height-1;

			rangeY2 = max(tmpRangeY1, tmpRangeY2);
		}
		//scan left//
		if(objCargoRecnoTL == objCargoRecnoBL && objLocFlagTL == objLocFlagBL )
		{
			if(objCargoRecnoTL && (objLocFlagTL & LOCATE_BLOCK_MASK))
			{
				for(rangeX1=((objXLoc>=1)?(objXLoc-1):0); rangeX1 >= 0; rangeX1--)
				{
					if((world.get_loc(rangeX1, objYLoc)->loc_flag ) != objLocFlagTL || 
						(mobile_type == UNIT_AIR ? (world.get_loc(rangeX1, objYLoc)->air_cargo_recno != objCargoRecnoTL) : (world.get_loc(rangeX1, objYLoc)->cargo_recno != objCargoRecnoTL)))
					{
						rangeX1++;
						break;
					}
				}
			}
			else
				rangeX1 = objXLoc;
		}
		else
		{
			int tmpRangeX1, tmpRangeX2;
			if(objCargoRecnoTL && (objLocFlagTL & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeX1=((objXLoc>=1)?(objXLoc-1):0); tmpRangeX1>=0; tmpRangeX1--)
				{
					if((world.get_loc(tmpRangeX1, objYLoc)->loc_flag ) != objLocFlagTL ||
						(mobile_type == UNIT_AIR ? (world.get_loc(tmpRangeX1, objYLoc)->air_cargo_recno != objCargoRecnoTL): (world.get_loc(tmpRangeX1, objYLoc)->cargo_recno != objCargoRecnoTL)))
					{
						tmpRangeX1++;
						break;
					}
				}
			}
			else
				tmpRangeX1 = objXLoc;

			if(objCargoRecnoBL && (objLocFlagBL & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeX2=((objXLoc>=1)?(objXLoc-1):0); tmpRangeX2>=0; tmpRangeX2--)
				{
					if((world.get_loc(tmpRangeX2, objYLoc+loc_height-1)->loc_flag ) != objLocFlagBL || 
						(mobile_type == UNIT_AIR ? (world.get_loc(tmpRangeX2, objYLoc+loc_height-1)->air_cargo_recno != objCargoRecnoBL):(world.get_loc(tmpRangeX2, objYLoc+loc_height-1)->cargo_recno != objCargoRecnoBL)))
					{
						tmpRangeX2++;
						break;
					}
				}
			}
			else
				tmpRangeX2 = objXLoc;

			rangeX1 = min(tmpRangeX1, tmpRangeX2);
		}
		//scan right//
		if(objCargoRecnoTR == objCargoRecnoBR && objLocFlagTR == objLocFlagBR ) 
		{
			if(objCargoRecnoTR && (objLocFlagTR & LOCATE_BLOCK_MASK))
			{
				for(rangeX2=((objXLoc+loc_width<MAX_WORLD_X_LOC)?objXLoc+loc_width:MAX_WORLD_X_LOC-1); rangeX2<MAX_WORLD_X_LOC; rangeX2++)
				{
					if((world.get_loc(rangeX2, objYLoc)->loc_flag ) != objLocFlagTR || 
						(mobile_type == UNIT_AIR ? (world.get_loc(rangeX2, objYLoc)->air_cargo_recno != objCargoRecnoTR):(world.get_loc(rangeX2, objYLoc)->cargo_recno != objCargoRecnoTR)))
					{
						rangeX2--;
						break;
					}
				}
			}
			else
				rangeX2 =  objXLoc+loc_width-1;
		}
		else
		{
			int tmpRangeX1, tmpRangeX2;
			if(objCargoRecnoTR && (objLocFlagTR & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeX1 = ((objXLoc+loc_width<MAX_WORLD_X_LOC)?objXLoc+loc_width:MAX_WORLD_X_LOC-1); tmpRangeX1<MAX_WORLD_X_LOC; tmpRangeX1++)
				{
					if((world.get_loc(tmpRangeX1, objYLoc)->loc_flag ) != objLocFlagTR ||
						(mobile_type == UNIT_AIR ? (world.get_loc(tmpRangeX1, objYLoc)->air_cargo_recno != objCargoRecnoTR):(world.get_loc(tmpRangeX1, objYLoc)->cargo_recno != objCargoRecnoTR)))
					{
						tmpRangeX1--;
						break;
					}
				}
			}
			else
				tmpRangeX1 = objXLoc+loc_width-1;

			if(objCargoRecnoBR && (objLocFlagBR & LOCATE_BLOCK_MASK))
			{
				for(tmpRangeX2 = ((objXLoc+loc_width<MAX_WORLD_X_LOC)?objXLoc+loc_width:MAX_WORLD_X_LOC-1); tmpRangeX2<MAX_WORLD_X_LOC; tmpRangeX2++)
				{
					if( world.get_loc(tmpRangeX2, objYLoc+loc_height-1)->loc_flag != objLocFlagBR	||	
						(mobile_type == UNIT_AIR ? (world.get_loc(tmpRangeX2, objYLoc)->air_cargo_recno != objCargoRecnoBR):(world.get_loc(tmpRangeX2, objYLoc)->cargo_recno != objCargoRecnoBR)))
					{
						tmpRangeX2--;
						break;
					}
				}
			}
			else
				tmpRangeX2 = objXLoc+loc_width-1;

			rangeX2 = max(tmpRangeX1, tmpRangeX2);
		}
	}

	rangeX1=max(rangeX1, 0);
	rangeY1=max(rangeY1, 0);
	rangeX2=min(rangeX2, MAX_WORLD_X_LOC-1);
	rangeY2=min(rangeY2, MAX_WORLD_Y_LOC-1);

	err_when((rangeX2-rangeX1+1) > 30);
	err_when((rangeY2-rangeY1+1) > 30);

//	mem_del(objLocFlag);
//	mem_del(objCargoRecno);
}
//----------- End of function UnitB::get_object_range -----------//

//----------- Begin of function UnitB::del_old_path -------------//
//
// This function deletes the old path whenever a new one is to be found
//
void UnitB::del_old_path()
{
	if(cur_path)
		mem_del(cur_path);
	cur_path = NULL;
}
//----------- End of function UnitB::del_old_path --------------//

//------------Begin of function Unit::handle_blocking-------------//
//
//This function function handles blocking situation
// return type <bool> - return true if handle successfully
//						return false if handle unsuccessfully
bool UnitB::handle_blocking()
{
	
	//---------Defining and initializing local variables-----------//
	int occupyingUnitRecordNumber;
	Unit* theOccupyingObject=NULL;
	int occLocX, occLocY;
	int theMoveToPlaceX, theMoveToPlaceY;
	bool handleSuccessfully=false;
	int is, js;
	//--------------------------------------------------------------//
	
	//setting the unit himself as not occupying the tile, otherwise it will found out that
	//he himself occupying the tile
		
	int lX = next_x_loc();
	int lY = next_y_loc();

	for(is = 0; is<obj_loc_width(); is++)
		for(js=0; js<obj_loc_height(); js++)
			world.get_loc(lX+is, lY+js)->remove_unit(mobile_type);
	
	//some local variables for use
	int checkingLocX=lX;
	int checkingLocY=lY;
	int directionX=0, directionY=0;
	int directionAngle;
	int destX = move_to_loc_x;
	int destY = move_to_loc_y;

	//would like to check first the direction of the destination, it makes more sense
	int diffX = destX-lX;
	int diffY = destY-lY;

	int stepSizeX=0, stepSizeY=0;
	
	if(diffX >= 0)
	{
		if(diffY<=0)
		{
			if(diffY >= -diffX)
			{
				checkingLocX++;
				directionX=1;
			}
			else
			{
				checkingLocY--;
				directionY=-1;
			}
		}
		else
		{
			if(diffY <= diffX)
			{
				checkingLocX++;
				directionX=1;
			}
			else
			{
				checkingLocY++;
				directionY =1;
			}
		}
	}
	else
	{
		if(diffY<=0)
		{
			if(diffY <= diffX)
			{
				checkingLocX--;
				directionX = -1;
			}
			else
			{
				checkingLocY--;
				directionY = -1;
			}
		}
		else
		{
			if(diffY <= -diffX)
			{
				checkingLocX--;
				directionX = -1;
			}
			else
			{
				checkingLocY++;
				directionY = 1;
			}
		}
	}
	
	if(!diffX && !diffY)
	{
		lX = next_x_loc();
		lY = next_y_loc();
		for(is = 0; is< obj_loc_width(); is++)
			for(js=0; js<obj_loc_height(); js++)
				world.get_loc(lX+is, lY+js)->set_unit(sprite_recno, mobile_type);
		
		return false;
	}

	directionAngle = m.angle_lookup(directionX, directionY);

	int firstTimeAngle = directionAngle;
	
	//loop over all the directiion of the blocked unit

	while(!handleSuccessfully && directionAngle < (firstTimeAngle+360))
	{
		occupyingUnitRecordNumber = checking_who_occupy_the_place_i_want_to_go(checkingLocX, checkingLocY);

		if(occupyingUnitRecordNumber)
			theOccupyingObject = unit_array[occupyingUnitRecordNumber];
		else
			theOccupyingObject = NULL;

		//do it only when it is of the same nationality and is a unit and is not a troop
		//let the blocking unit to move to the direction of the blocked unit want to go to

		if(theOccupyingObject && theOccupyingObject->nation_recno == nation_recno)
		{
			stepSizeX = obj_loc_width();
			stepSizeY = obj_loc_height();
			occLocX = theOccupyingObject->next_x_loc();
			occLocY = theOccupyingObject->next_y_loc();

			stepSizeX *= directionX;
			stepSizeY *= directionY;

			theMoveToPlaceX = occLocX+stepSizeX;
			theMoveToPlaceY = occLocY+stepSizeY;
			
			Location* locPtr = world.get_loc(theMoveToPlaceX, theMoveToPlaceY);
					
			//if the blocking unit successfully walk away, the blocked unit can then try to find a new path to the destination
			if( theOccupyingObject->can_move(theMoveToPlaceX, theMoveToPlaceY) || 
				theOccupyingObject->checking_who_occupy_the_place_i_want_to_go(theMoveToPlaceX, theMoveToPlaceY))
			{
				theOccupyingObject->move_to(theMoveToPlaceX, theMoveToPlaceY, false);
				if(theOccupyingObject->cur_action == SPRITE_MOVE || theOccupyingObject->cur_action == SPRITE_WAIT)
				{
					stop_move(); 
					set_first_time_blocked();
					handleSuccessfully = true;
				}
			}
		}
		
		directionAngle+=45;
		m.xy_lookup(directionAngle, directionX, directionY);
		checkingLocX = lX+directionX;
		checkingLocY = lY+directionY;
	}
	
	//set back the unit to occupy the tile after handling blocking.
	lX = next_x_loc();
	lY = next_y_loc();
	for(is = 0; is< obj_loc_width(); is++)
		for(js=0; js<obj_loc_height(); js++)
			world.get_loc(lX+is, lY+js)->set_unit(sprite_recno, mobile_type);

	return handleSuccessfully;
}
//------------End of function Unit::handle_blocking---------------//

//------------Begin of function Unit::checking_who_occupy_the_place_i_want_to_go----------//
//
// This function checks who occupy the place that the unit wanted to go to
//
// <int> checkingLocX and checkingLocY ----the coordinate of the tile that the unit wanted to go
//
// return type <int> ----- the unit record number of the unit that occupys the tile is returned
//
int UnitB::checking_who_occupy_the_place_i_want_to_go(int checkingLocX, int checkingLocY)
{
	int is, js, occupyingUnitRecordNumber = 0;
	Unit* theOccupyingStuff;
	int temperoryCheckingLocX, temperoryCheckingLocY;

	//return if the checking location is out of boundary
	if(checkingLocX >= MAX_WORLD_X_LOC || checkingLocX < 0
		|| checkingLocY >= MAX_WORLD_Y_LOC || checkingLocY <0)
		return 0;

	//only found those unit that are of the same nation of the said unit and is not moving
	// it is because only this type of units should walk away and give out the position
	// if the occupying unit is moving, no need to consider since the tile will soon be freed

	is = 0;
	temperoryCheckingLocX = checkingLocX;

	while(!occupyingUnitRecordNumber && is<obj_loc_width())
	{
		if(temperoryCheckingLocX >= MAX_WORLD_X_LOC || temperoryCheckingLocX < 0)
		{
			is++;
			temperoryCheckingLocX++;
			continue;
		}

		js=0;
		temperoryCheckingLocY = checkingLocY;
		while(!occupyingUnitRecordNumber&& js<obj_loc_height())
		{
			if(temperoryCheckingLocY >= MAX_WORLD_Y_LOC || temperoryCheckingLocY < 0)
			{	
				js++;
				temperoryCheckingLocY++;
				continue;
			}
			
			occupyingUnitRecordNumber = world.get_loc(temperoryCheckingLocX, temperoryCheckingLocY)->unit_recno(mobile_type);
			if(occupyingUnitRecordNumber 
				&& ((occupyingUnitRecordNumber == sprite_recno) 
				|| (theOccupyingStuff = unit_array[occupyingUnitRecordNumber])->nation_recno != nation_recno 
				|| theOccupyingStuff->cur_action == SPRITE_MOVE 
				|| theOccupyingStuff->cur_action == SPRITE_WAIT 
				|| theOccupyingStuff->cur_action == SPRITE_ATTACK))
				occupyingUnitRecordNumber = 0;
			js++;
			temperoryCheckingLocY++;
		}
		is++;
		temperoryCheckingLocX++;
	}
	return occupyingUnitRecordNumber;
}

//-----------End of function Unit::checking_who_occupy_the_place_i_want_to_go-------------------------//


//--------- Begin of function UnitB::is_dead ---------//

bool UnitB::is_dead()
{
	return hit_points<=0 || cur_action==SPRITE_DIE;
}
//----------- End of function UnitB::is_dead -----------//

//------------ Begin of function Unit::can_move -------------------//
//
// check whether a specific location can go walk to 
//
// <int> locX - x-coordinate
// <int> locY - y-coordinate
//
// return: <bool> - return true if the lcoation can be walk
//					return false if the location cannot be walk
bool UnitB::can_move(int locX, int locY)
{
	// ###### begin Gilbert 20/4 ##########//
	Location* locPtr;
//	int i, j;

//	for(i=0; i<obj_loc_width(); i++)
//	{
//		for(j=0; j<obj_loc_height(); j++)
//		{
//			locPtr = world.get_loc(locX+i, locY+j);
//			if(locX+i>= MAX_WORLD_X_LOC || locX+i <0 || locY+j >= MAX_WORLD_Y_LOC || locY+j < 0 || (!locPtr->can_move(obj_mobile_type()) && (locPtr->unit_recno(obj_mobile_type()) != sprite_recno)))
//				return false;
//		}
//	}

	if( loc_width == 1 )
	{
		err_when( loc_height != 1);
		if( locX<0 || locY<0 || locX >= MAX_WORLD_X_LOC || locY >= MAX_WORLD_Y_LOC )
			return false;
		if( !(locPtr=world.get_loc(locX, locY))->can_move(mobile_type)
			&& locPtr->unit_recno(mobile_type) != sprite_recno )
			return false;
	}
	else
	{
		int i, j;
		if( locX<0 || locY<0 || locX+loc_width > MAX_WORLD_X_LOC || locY+loc_height > MAX_WORLD_Y_LOC )
			return false;

		for( j = 0; j < loc_height; ++j )
		{
			locPtr = world.get_loc( locX, locY+j);
			for( i = 0; i < loc_width; ++i, ++locPtr )
			{
				if( !locPtr->can_move(mobile_type)
					&& locPtr->unit_recno(mobile_type) != sprite_recno )
					return false;
			}
		}
	}
	// ###### end Gilbert 20/4 ##########//

	return true;
}
//------------ End of function Unit::can_move ---------------------//

//------------ Begin of function Unit::can_move2 -------------------//
// 
// check a specific location can be walk, taking care that if that location is occupied
// by a walking or waiting unit, that location will later on be freed.
//
// <int> locX - x-coordinate
// <int> locY - y-coordinate
//
// return: <bool> - true if the location ccan be walked to
//					false otherwise
//
bool UnitB::can_move2(int locX, int locY)
{
	Location* locPtr;
	int occRecno;
	Unit* unitPtr;
	int i, j;

	// ####### begin Gilbert 20/4 ########//

//	for(i=0; i<obj_loc_width(); i++)
//	{
//		for(j=0; j<obj_loc_height(); j++)
//		{
//			locPtr = world.get_loc(locX+i, locY+j);
//			if(locX+i>= MAX_WORLD_X_LOC || locX+i <0 || locY+j >= MAX_WORLD_Y_LOC || locY+j < 0 )
//				return false;
//			else if(!locPtr->can_move(mobile_type))
//			{
//				occRecno = locPtr->unit_recno(mobile_type);
//				if(!occRecno) //not a unit
//					return false;
//				else if(occRecno != sprite_recno)
//				{
//					unitPtr = unit_array[occRecno];
//					if(unitPtr->cur_action == SPRITE_WAIT || unitPtr->cur_action == SPRITE_MOVE)
//						continue;
//					else
//						return false;
//				}
//			}
//		}
//	}

	if( locX < 0 || locY < 0 || locX+loc_width > MAX_WORLD_X_LOC || locY+loc_height > MAX_WORLD_Y_LOC )
		return false;

	for(i=0; i<loc_width; i++)
	{
		for(j=0; j<loc_height; j++)
		{
			locPtr = world.get_loc(locX+i, locY+j);

			if(!locPtr->can_move(mobile_type))
			{
				occRecno = locPtr->unit_recno(mobile_type);
				if(!occRecno) //not a unit
					return false;

				if(occRecno != sprite_recno)
				{
					unitPtr = unit_array[occRecno];
					if(unitPtr->cur_action != SPRITE_WAIT && unitPtr->cur_action != SPRITE_MOVE)
						return false;
				}
			}
		}
	}
	// ####### end Gilbert 20/4 ########//

	return true;
}
//------------ End of function Unit::can_move2 ---------------------//

//------------ Begin of function UnitB::checking_who_occupy_my_next_tile-------//
int UnitB::checking_who_occupy_my_next_tile(int checkingLocX, int checkingLocY)
{
	int is, js, occupyingUnitRecordNumber = 0;
	int temperoryCheckingLocX, temperoryCheckingLocY;

	//return if the checking location is out of boundary
	if(checkingLocX >= MAX_WORLD_X_LOC || checkingLocX < 0
		|| checkingLocY >= MAX_WORLD_Y_LOC || checkingLocY <0)
		return 0;

	//only found those unit that are of the same nation of the said unit and is not moving
	// it is because only this type of units should walk away and give out the position
	// if the occupying unit is moving, no need to consider since the tile will soon be freed

	is = 0;
	temperoryCheckingLocX = checkingLocX;

	while(!occupyingUnitRecordNumber && is<obj_loc_width())
	{
		if(temperoryCheckingLocX >= MAX_WORLD_X_LOC || temperoryCheckingLocX < 0)
		{
			is++;
			temperoryCheckingLocX++;
			continue;
		}

		js=0;
		temperoryCheckingLocY = checkingLocY;
		while(!occupyingUnitRecordNumber&& js<obj_loc_height())
		{
			if(temperoryCheckingLocY >= MAX_WORLD_Y_LOC || temperoryCheckingLocY < 0)
			{	
				js++;
				temperoryCheckingLocY++;
				continue;
			}
			occupyingUnitRecordNumber = world.get_loc(temperoryCheckingLocX, temperoryCheckingLocY)->unit_recno(mobile_type);
			js++;
			temperoryCheckingLocY++;
		}
		is++;
		temperoryCheckingLocX++;
	}
	return occupyingUnitRecordNumber;
}
// ---- end of function UnitB::checking who occupy my next tile -------------------//