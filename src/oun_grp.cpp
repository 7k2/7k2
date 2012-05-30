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

//Filename   : OUN_GRP.CPP
//Description: UnitGroup class

#include <all.h>
#include <omouse.h>
#include <opower.h>
#include <onation.h>
#include <owaypnt.h>
#include <oun_grp.h>
#include <opreuse.h>
#include <oremote.h>
#include <ofont.h>
#include <ou_god.h>
#include <of_mfort.h>

static void put_profile(int y, const char* dispDes, int dispValue);

// define struct ClusterInfo for cluster_units_for_path_reuse

struct ClusterInfo
{
	int old_minx;
	int old_miny;
	int old_maxx;
	int old_maxy;
	int new_minx;
	int new_miny;
	int new_maxx;
	int new_maxy;
	int count;

	void init( int minx, int miny, int maxx, int maxy, int unitCount)
	{
		old_minx = new_minx = minx;
		old_miny = new_miny = miny;
		old_maxx = new_maxx = maxx;
		old_maxy = new_maxy = maxy;
		count = unitCount;
	}
};

// ------- declare static function

static void pack_clusters( ClusterInfo *clusterArray, int clusterCount, int centerX=-1, int centerY=-1 );

//--------- Begin of function UnitGroup::UnitGroup ---------//
//
UnitGroup::UnitGroup() : DynArray( sizeof(short), 100 )
{
}
//----------- End of function UnitGroup::UnitGroup -----------//

//--------- Begin of function UnitGroup::add_unit ---------//
//
void UnitGroup::add_unit(short unitRecno)
{
	err_when( unit_array.is_deleted(unitRecno) );

	linkin(&unitRecno);
}
//----------- End of function UnitGroup::add_unit -----------//

//--------- Begin of function UnitGroup::del_unit ---------//
//
void UnitGroup::del_unit(short recNo)
{
	err_when( recNo < 1 || recNo > size() );

	linkout(recNo);
}
//----------- End of function UnitGroup::del_unit -----------//

//--------- Begin of function UnitGroup::is_visible ---------//
//
// Whether the unit is still visible.
//
int UnitGroup::is_visible(short recNo)
{
	err_when( recNo < 1 || recNo > size() );

	int unitRecno = *((short*)get(recNo));

	return !unit_array.is_deleted(unitRecno) &&
			 unit_array[unitRecno]->is_visible();
}
//----------- End of function UnitGroup::is_visible -----------//


//--------- Begin of function UnitGroup::validate ---------//
//
// Validate all units in UnitGroup and remove delted ones.
//
void UnitGroup::validate()
{
	for( int i=size() ; i>0 ; i-- )
	{
		if( is_unit_deleted(i) )
			del_unit(i);
	}
}
//----------- End of function UnitGroup::validate -----------//


//--------- Begin of function UnitGroup::is_unit_deleted ---------//
//
// Return whether the specific unit in unit_group is deleted in unit_array[].
//
int UnitGroup::is_unit_deleted(short recNo)
{
	err_when( recNo < 1 || recNo > size() );

	return unit_array.is_deleted( *((short*)get(recNo)) );
}
//----------- End of function UnitGroup::is_unit_deleted -----------//


//--------- Begin of function UnitGroup::get_unit ---------//
//
Unit* UnitGroup::get_unit(short recNo)
{
	err_when( recNo < 1 || recNo > size() );

	return unit_array[ *((short*)get(recNo)) ]; 
}
//----------- End of function UnitGroup::get_unit -----------//


//--------- Begin of function UnitGroup::has_unit ---------//
//
// Return whether the given unit is in this UnitGroup.
//
int UnitGroup::has_unit(int unitRecno)
{
	for( int i=size() ; i>0 ; i-- )
	{
		if( get_unit(i)->sprite_recno == unitRecno )
			return 1;
	}

	return 0;
}
//----------- End of function UnitGroup::has_unit -----------//


//--------- Begin of function UnitGroup::set ---------//
//
// Set UnitGroup with the given unit list.
//
void UnitGroup::set(short* unitRecnoArray, int unitCount)
{
	zap();

	for( int i=0 ; i<unitCount ; i++ )
	{
		// ####### begin Gilbert 16/3 #######//
		if( !unit_array.is_deleted(unitRecnoArray[i]) )
		{
			add_unit( unitRecnoArray[i] );
		}
		// ####### end Gilbert 16/3 #######//
	}
}
//----------- End of function UnitGroup::set -----------//


//--------- Begin of function UnitGroup::set_to_selected ---------//
//
// Add all selected units in unit_array into this group.
//
void UnitGroup::set_to_selected()
{
	zap();

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		if( unit_array[i]->selected_flag )
			add_unit(i);
	}
}
//----------- End of function UnitGroup::set_to_selected -----------//


//--------- Begin of function UnitGroup::select_all ---------//
//
// Set all units in this group to "selected" in unit_array.
//
void UnitGroup::select_all()
{
	power.reset_selection();

	for( int i=size() ; i>0 ; i-- )
	{
		if( is_unit_deleted(i) )
			continue;

		Unit* unitPtr = get_unit(i);

		if( unitPtr->nation_recno == nation_array.player_recno &&
			 unitPtr->is_visible() )
		{
		//	unitPtr->selected_flag = 1;
			unitPtr->select();

			//--- set unit_array.selected_recno to the unit with the highest rank ---//

			if( unit_array.selected_recno==0 ||
				 unitPtr->rank_id > unit_array[unit_array.selected_recno]->rank_id )
			{
				unit_array.selected_recno = unitPtr->sprite_recno;
			}
		}
	}
}
//----------- End of function UnitGroup::select_all -----------//



//--------- Begin of function UnitGroup::get_write_len ---------//
//
// get the size (in byte) of membery written by write_mem
//
int UnitGroup::get_write_len()
{
	const int memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(short) - 1) / sizeof(short);

	// <member in UnitGroup> <unit count> <unit recno> ...
	return memberSize * sizeof(short) + sizeof(short) + ele_size * size();
}
//--------- End of function UnitGroup::get_write_len ---------//


//--------- Begin of function UnitGroup::write_mem ---------//
//
void UnitGroup::write_mem( short *msg )
{
	const int memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(*msg) - 1) / sizeof(*msg);
	memcpy( msg, ((char *)this) + sizeof(DynArray), sizeof(*this) - sizeof(DynArray) );
	msg[memberSize] = size();

	err_when( ele_size != sizeof(short) );		// parameter type of set(short *, int), so it can be loaded easily
	memcpy( msg+memberSize+1, body_buf, sizeof(short) * size() );
}
//--------- End of function UnitGroup::write_mem ---------//


//--------- Begin of function UnitGroup::read_mem ---------//
//
void UnitGroup::read_mem( short *msg )
{
	// restore member units
	const int memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(*msg) - 1) / sizeof(*msg);
	set( msg+memberSize+1, msg[memberSize] );

	// restore data member
	memcpy( ((char *)this) + sizeof(DynArray), msg, sizeof(*this) - sizeof(DynArray) );
}
//--------- End of function UnitGroup::read_mem ---------//


//--------- Begin of function UnitGroup::validate_unit_array ---------//
//
// Validate all units in selectedUnitArray, remove deleted units from
// selectedUnitArray.
//
// <int> nationRecno				player nation recno or remote player nation recno
// return number of valid units
//
// also update validate_selected_unit_array in REMOTEM.CPP
//
int UnitGroup::validate_unit_array( int nationRecno )
{
	int initSize = last_ele;		// keep an instant of size()
	int initCurPos = cur_pos;		// keep an instant of cur_pos

	err_when( ele_size != sizeof(short) );
	register short *readPtr = (short *)body_buf;
	register short *writePtr = (short *)body_buf;
	last_ele = 0;
	cur_pos = 0;

	for( int i=1 ; i <= initSize; ++i, ++readPtr )
	{
		Unit* unitPtr;
		if( !unit_array.is_deleted(*readPtr) && (unitPtr = unit_array[*readPtr])
			&& unitPtr->is_visible() && unitPtr->is_nation(nationRecno) )
		{
			*writePtr = *readPtr;
			++writePtr;
			++last_ele;
			if( initCurPos == i )		// update cur_pos
				cur_pos = last_ele;		// increased last_ele
		}
	}

	err_when( last_ele > initSize );
	return last_ele;
}
//--------- End of function UnitGroup::validate_unit_array ---------//


//--------- Begin of function UnitGroup::stop ---------//
//
void UnitGroup::stop(char remoteAction)
{
	// ###### begin Gilbert 19/11 ######//
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_STOP, get_write_len() );
		write_mem(shortPtr);
		return;
	}
	// ###### end Gilbert 19/11 ######//

	for( int i=size() ; i>0 ; i-- )
	{
		get_unit(i)->stop_order();
	}
}
//--------- End of function UnitGroup::stop ---------//


//----------- Begin of function UnitGroup::attack --------------//
//
// assign location for a group of units to attack
//

void UnitGroup::attack(int targetObjRecno, char remoteAction)
{
	if( base_obj_array.is_deleted(targetObjRecno) )		// may be deleted after multiplayer message delay
		return;

	//----- player can only order his own units to attack, cannot order his spies cloaked in other nations to attack, as this can easy lead nations into wars ----//

	if( remoteAction==COMMAND_PLAYER )
	{
		for(int i=size() ; i>0 ; i-- )
		{
			if( get_unit(i)->nation_recno != nation_array.player_recno )
				del_unit(i);
		}
	}

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <target baseobj recno> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_ATTACK, sizeof(short) + get_write_len() );
		shortPtr[0] = targetObjRecno;
		write_mem(shortPtr+1);
		return;
	}
 
	group_by_mobile_type();

	if(unit_group_land.size())
		unit_group_land.exe_attack(targetObjRecno);
	if(unit_group_air.size())
		unit_group_air.exe_attack(targetObjRecno);
	
}

void UnitGroup::exe_attack(int targetObjRecno)
{
	BaseObj *targetObj = base_obj_array[targetObjRecno];

	int targetWidth = targetObj->obj_loc_width();
	int targetHeight = targetObj->obj_loc_height();
	int targetLocX = targetObj->obj_loc_x1();
	int targetLocY = targetObj->obj_loc_y1();
	/*
	int testLocX, testLocY;
	int index, unitIndex;
	int maxWidth=0, maxHeight=0;
	Unit* unitPtr;
	int w, h;
	int rangeAttack=0, closeAttack=0;
	int minAttackRange = 0;
	int distance;
	Unit** arrayForRangeAttack=NULL, **arrayForCloseAttack=NULL;
	int numberAttacking = 0;
	*/

	//--looking for the maximum range, size of the units ---//
	//--and counting how many units will do range attack and close attack respectively---//
	/*
	for(int i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		if(!unitPtr->can_attack())
			continue;

		//---------------------------------------------//

		if(unitPtr->cur_action == SPRITE_ATTACK && unitPtr->cur_order.para == targetObjRecno)
		{
			numberAttacking++;
			continue;
		}
		// ##### begin Gilbert 4/11 ######//
		// distance = unitPtr->cal_distance(targetLocX, targetLocY, targetWidth, targetHeight);
		distance = unitPtr->area_distance(targetObj);
		// ##### end Gilbert 4/11 ######//
		unitPtr->choose_best_attack_mode(distance, targetObj->obj_mobile_type());
		if(unitPtr->obj_loc_width() > maxWidth)
			maxWidth = unitPtr->obj_loc_width();
		if(unitPtr->obj_loc_height() > maxHeight)
			maxHeight = unitPtr->obj_loc_height();
		if((unitPtr->attack_range() < minAttackRange) && (unitPtr->attack_range()>1))
			minAttackRange = unitPtr->attack_range();
		if(unitPtr->attack_range() > 1)
			rangeAttack++;
		else
			closeAttack++;
	}
	//---------------------------------------------------------//
//	err_when(!rangeAttack && !closeAttack);
	if(!rangeAttack && !closeAttack)
		return;

	if(rangeAttack)
		arrayForRangeAttack = (Unit**) mem_add(sizeof(Unit*)*rangeAttack);
	if(closeAttack)
		arrayForCloseAttack = (Unit**) mem_add(sizeof(Unit*)*closeAttack);

	int iRa = 0, iCa = 0;
	//---- separate the units into two arrays by means of their attacking mode ---//
	for(i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		if(!unitPtr->can_attack())
			continue;

		if(unitPtr->cur_action == SPRITE_ATTACK && unitPtr->cur_order.para == targetObjRecno)
			continue;

		if(unitPtr->attack_range()>1)
			arrayForRangeAttack[iRa++] = unitPtr;
		else
			arrayForCloseAttack[iCa++] = unitPtr;
	}

	err_when(!maxWidth);
	err_when(!maxHeight);

	//----- assign a good location for the units to undergo range attack -----//
	if(rangeAttack)
	{
		int xLoc1 = MAX(targetLocX-minAttackRange, 0);
		int yLoc1 = MAX(targetLocY-minAttackRange, 0);
		int xLoc2 = MIN(targetLocX+targetWidth-1+minAttackRange, MAX_WORLD_X_LOC-1);
		int yLoc2 = MIN(targetLocY+targetHeight-1+minAttackRange, MAX_WORLD_Y_LOC-1);
		int checkXLoc, checkYLoc;
		unitIndex = 0;
		unitPtr = arrayForRangeAttack[unitIndex];

		//-------- checking for surrounding location ----------//
		for(checkXLoc=xLoc1; checkXLoc<=xLoc2; checkXLoc++)
		{
			if(unitPtr->can_move(checkXLoc, yLoc1))
			{
				unitPtr->attack_loc_offset_x = checkXLoc-targetLocX;
				unitPtr->attack_loc_offset_y = yLoc1-targetLocY;
				if(unitPtr->obj_loc_width()>1)
					checkXLoc += (unitPtr->obj_loc_width()-1);
				if(unitIndex<rangeAttack-1)
					unitPtr = arrayForRangeAttack[++unitIndex];
				else
					break;
			}
		}

		if(unitIndex < rangeAttack)
		{
			for(checkYLoc=yLoc1+1; checkYLoc<yLoc2; checkYLoc++)
			{
				if(unitPtr->can_move(xLoc1, checkYLoc))
				{
					unitPtr->attack_loc_offset_x = xLoc1-targetLocX;
					unitPtr->attack_loc_offset_y = checkYLoc-targetLocY;
					if(unitPtr->obj_loc_height() > 1)
						checkYLoc += (unitPtr->obj_loc_height()-1);
					if(unitIndex < rangeAttack-1)
						unitPtr = arrayForRangeAttack[++unitIndex];
					else
						break;
				}
			}
		}

		if(unitIndex < rangeAttack)
		{
			for(checkXLoc=xLoc1; checkXLoc<=xLoc2; checkXLoc++)
			{
				if(unitPtr->can_move(checkXLoc, yLoc2))
				{
					unitPtr->attack_loc_offset_x = checkXLoc-targetLocX;
					unitPtr->attack_loc_offset_y = yLoc2-targetLocY;
					if(unitPtr->obj_loc_width() > 1)
						checkXLoc+= (unitPtr->obj_loc_width()-1);
					if(unitIndex < rangeAttack-1)
						unitPtr= arrayForRangeAttack[++unitIndex];
					else
						break;
				}
			}
		}
		
		if(unitIndex < rangeAttack)
		{
			for(checkYLoc=yLoc1+1; checkYLoc<yLoc2; checkYLoc++)
			{
				if(unitPtr->can_move(xLoc2, checkYLoc))
				{
					unitPtr->attack_loc_offset_x = xLoc2-targetLocX;
					unitPtr->attack_loc_offset_y = checkYLoc-targetLocY;
					if(unitPtr->obj_loc_height() > 1)
						checkYLoc+=(unitPtr->obj_loc_height()-1);
					if(unitIndex < rangeAttack-1)
						unitPtr = arrayForRangeAttack[++unitIndex];
					else
						break;
				}
			}
		}
		if(unitIndex < rangeAttack)
		{
			for(; unitIndex<rangeAttack; unitIndex++)
			{
				unitPtr = arrayForRangeAttack[unitIndex];
				//--settting attack_loc_offset_? to 0 will make them cannot attack --//
				unitPtr->attack_loc_offset_x = 0;
				unitPtr->attack_loc_offset_y = 0;
			}
		}
	}
	
	//---- finding a good location for the units undergoing close attack -----//
	if(closeAttack)
	{
		unitIndex = 0;
		unitPtr = arrayForCloseAttack[unitIndex];

		if((testLocY=targetLocY-maxHeight) >= 0) //has top edge
		{
			
			w = unitPtr->obj_loc_width();
			while(w>=0)
			{
				if(targetLocX >= w)
				{
					index = -w;
					break;
				}
				w--;
			}
			err_when(index>0);

			for(; index<targetWidth; index++)
			{
				if(unitPtr->can_move(targetLocX+index, testLocY))
				{
					unitPtr->attack_loc_offset_x = index;
					unitPtr->attack_loc_offset_y = -maxHeight;
					if(unitPtr->obj_loc_width() > 1)
						index += (unitPtr->obj_loc_width()-1);
					if(unitIndex < closeAttack-1)
						unitPtr = arrayForCloseAttack[++unitIndex]; //get_next_unit
					else 
						break;
				}
			}
		}
		if((unitIndex < closeAttack) && ((testLocX = targetLocX + targetWidth) <= MAX_WORLD_X_LOC-maxWidth)) //has right edge
		{
			h = unitPtr->obj_loc_height();
			while(h>=0)
			{
				if(targetLocY >= h)
				{
					index = -h;
					break;
				}
				h--;
			}
			err_when(index>0);
			
			for(; index<targetHeight; index++)
			{
				if(unitPtr->can_move(testLocX, targetLocY+index))
				{
					unitPtr->attack_loc_offset_x = targetWidth;
					unitPtr->attack_loc_offset_y = index;
					if(unitPtr->obj_loc_height() > 1)
						index += (unitPtr->obj_loc_height()-1);
					if(unitIndex < closeAttack-1)
						unitPtr = arrayForCloseAttack[++unitIndex]; //get next unit
					else
						break;
				}
			}
		}
		if((unitIndex < closeAttack) && ((testLocY = targetLocY + targetHeight) <= MAX_WORLD_Y_LOC-maxHeight)) //has bottom edge
		{
			w = unitPtr->obj_loc_width();
			while(w>=0)
			{
				if(targetLocX+targetWidth<=MAX_WORLD_X_LOC-w)
				{
					index = targetWidth-(unitPtr->obj_loc_width()-w);
					break;
				}
				w--;
			}

			for(; index>=0; index--)
			{
				if(unitPtr->can_move(targetLocX+index, testLocY))
				{
					unitPtr->attack_loc_offset_x = index;
					unitPtr->attack_loc_offset_y = targetHeight;
					if(unitIndex < closeAttack-1)
					{
						unitPtr = arrayForCloseAttack[++unitIndex];
						if(unitPtr->obj_loc_width() > 1)
							index -= (unitPtr->obj_loc_width()-1);
					}
					else
						break;
				}
			}
		}
		if(unitIndex < closeAttack && ((testLocX = targetLocX-maxWidth) >=0 )) //has left edge
		{

			h = unitPtr->obj_loc_height();
			while(h>=0)
			{
				if(targetLocY+targetHeight<=MAX_WORLD_Y_LOC-h)
				{
					index = targetHeight-(unitPtr->obj_loc_height()-h);
					break;
				}
				h--;
			}
			
			for(; index>=0; index--)
			{
				if(unitPtr->can_move(testLocX, targetLocY+index)) 
				{
					unitPtr->attack_loc_offset_x = -1;
					unitPtr->attack_loc_offset_y = index;
					if(unitIndex < closeAttack-1)
					{
						unitPtr = arrayForCloseAttack[++unitIndex];
						if(unitPtr->obj_loc_height() > 1)
							index -= (unitPtr->obj_loc_height()-1);
					}
					else
						break;
				}
			}
		}

		//----for those who cannot find a good position, cannot attack
		//---- setting the attack_loc_offset_? to 0 means they cannot attack -----//
		if(unitIndex < closeAttack)
		{
			while(unitIndex < closeAttack-1)
			{
				unitPtr->attack_loc_offset_x = 0;
				unitPtr->attack_loc_offset_y = 0;
				unitPtr = arrayForCloseAttack[++unitIndex];
			}
			unitPtr->attack_loc_offset_x = 0;
			unitPtr->attack_loc_offset_y = 0;
		}
	}
	*/
	cluster_units_for_attack(targetObjRecno, targetLocX, targetLocY, targetWidth, targetHeight);	
	/*		
	if(arrayForRangeAttack) 
		mem_del(arrayForRangeAttack);
	if(arrayForCloseAttack)
		mem_del(arrayForCloseAttack);
	*/
}
//----------- End of function UnitGroup::attack --------------//

//-----------Begin of function UnitGroup::move--------------//
//
// This function designate a troop to move to the given destination.
// PathReuse is called by the troop to find the path to the destination
//
// <int>  destXLoc, destYLoc - the destination to move to.
// <bool> forceMoveFlag		  - force move flag (default: false)
// <char> remoteAction       - remote action
//
void UnitGroup::move(int destXLoc, int destYLoc, bool forceMoveFlag, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <xLoc> <yLoc> <forceMoveFlag> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_MOVE, sizeof(short)*3 + get_write_len() );
		shortPtr[0] = destXLoc;
		shortPtr[1] = destYLoc;
		shortPtr[2] = forceMoveFlag ? 1 : 0;
		write_mem(shortPtr+3);
		return;
	}
	group_by_mobile_type();

	if(unit_group_land.size())
		unit_group_land.exe_move(destXLoc, destYLoc, forceMoveFlag);
	if(unit_group_air.size())
		unit_group_air.exe_move(destXLoc, destYLoc, forceMoveFlag);
}

void UnitGroup::exe_move(int destXLoc, int destYLoc, bool forceMoveFlag)
{
	run_action( destXLoc, destYLoc, UNIT_MOVE, 0, -1, forceMoveFlag );
}
//---------------End of function UnitGroup::move---------------------//


//----------- Begin of function UnitGroup::assign ---------------//
//
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
void UnitGroup::assign(int destXLoc, int destYLoc, int targetMobileType, char remoteAction, int actionNationRecno, bool forceMoveFlag)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <xLoc> <yLoc> <action Nation Recno> <force move> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_ASSIGN, sizeof(short)*5 + get_write_len() );
		shortPtr[0] = destXLoc;
		shortPtr[1] = destYLoc;
		shortPtr[2] = targetMobileType;
		shortPtr[3] = actionNationRecno;
		shortPtr[4] = forceMoveFlag ? 1 : 0;
		write_mem(shortPtr+5);
		return;
	}

	group_by_mobile_type();

	if(unit_group_land.size())
		unit_group_land.exe_assign(destXLoc, destYLoc, targetMobileType, actionNationRecno, forceMoveFlag);
	if(unit_group_air.size())
		unit_group_air.exe_assign(destXLoc, destYLoc, targetMobileType, actionNationRecno, forceMoveFlag);
}

void UnitGroup::exe_assign(int destXLoc, int destYLoc, int targetMobileType, int actionNationRecno, bool forceMoveFlag)
{
	//---------------------------------------//
	Unit* unitPtr;
	int locX, locY;

	int right = 0;
	int left = 0;
	int midX = 0;
	int top = 0;
	int bottom = 0;
	int midY = 0;

	if(size() == 1)
	{
		unitPtr = get_unit(1);
		unitPtr->set_no_longer_in_formation();
		unitPtr->assign(destXLoc, destYLoc, actionNationRecno, forceMoveFlag);
		return;
	}

	Location* locPtr;

	locPtr = world.get_loc(destXLoc, destYLoc);
	int baseObjRecno = locPtr->base_obj_recno(targetMobileType); 
	// ###### begin Gilbert 6/5 #######//
	if( !baseObjRecno )
		return;
	// ###### end Gilbert 6/5 #######//
	BaseObj* thePlace = base_obj_array[baseObjRecno];
	int destWidth = thePlace->obj_loc_width();
	int destHeight = thePlace->obj_loc_height();

	int i;
	for(i = 1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		unitPtr->set_no_longer_in_formation();
		locX = unitPtr->next_x_loc();
		locY = unitPtr->next_y_loc();

		if(locX < destXLoc) //left
			left++;
		else if(locX > destXLoc+destWidth-1) //right
			right++;
		else
			midX++;
		if(locY < destYLoc) //top
			top++;
		else if(locY > destYLoc+destHeight-1) //bottom
			bottom++;
		else
			midY++;
	}
	if(!left && !midX) //all at right side
	{
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno, actionNationRecno, forceMoveFlag );
		return;
	}
	if(!right && !midX) //all at left side
	{
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno, actionNationRecno, forceMoveFlag );
		return;
	}
	if(!top && !midY) //all at bottom side
	{
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno, actionNationRecno, forceMoveFlag);
		return;
	}
	if(!bottom && !midY) //all at top side
	{
		run_action(destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno, actionNationRecno, forceMoveFlag);
		return;
	}

	for(i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		unitPtr->assign(destXLoc, destYLoc, actionNationRecno, forceMoveFlag);
	}
}
//------------ End of function UnitGroup::assign ----------------//


// ####### begin Gilbert 25/1 ########//
// -------- begin of function UnitGroup::transform_mfort --------//
//
// convert to grokken units to FirmMonsterFortress
//
void UnitGroup::transform_mfort(int destXLoc, int destYLoc, char remoteAction)
{
	if( size() == 0 )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <xLoc> <yLoc> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNITS_GO_TRANSFORM_MFORT, sizeof(short)*2 + get_write_len() );
		shortPtr[0] = destXLoc;
		shortPtr[1] = destYLoc;
		write_mem(shortPtr+2);
		return;
	}

	short builderUnit[MAX_EXTRA_BUILDER];		// = (short*) mem_add_clear(MAX_EXTRA_BUILDER*sizeof(short));
	int checkIndex = 0;
	Unit *cmdUnit = get_unit(1);

	int i;
	for(i = 1; i <= size() && checkIndex < MAX_EXTRA_BUILDER; i++)
	{
		Unit *unitPtr = get_unit(i);

		if(unitPtr->unit_id == cmdUnit->unit_id && unitPtr->nation_recno == cmdUnit->nation_recno
			&& unitPtr->is_visible() )
		{
			builderUnit[checkIndex] = unitPtr->sprite_recno;
			checkIndex++;
		}
	}

	if(checkIndex == MAX_EXTRA_BUILDER)
	{
		long dist2[MAX_EXTRA_BUILDER][MAX_EXTRA_BUILDER];	// distance between each builder and each destination
		// dist[ builder i][ place j ]

		int j;

		int destX[MAX_EXTRA_BUILDER], destY[MAX_EXTRA_BUILDER];
		err_when( MAX_EXTRA_BUILDER != 4 );
		destX[0] = destXLoc;				// top left
		destY[0] = destYLoc;
		destX[1] = destXLoc + 2;		// top right
		destY[1] = destYLoc;
		destX[2] = destXLoc;				// bottom left
		destY[2] = destYLoc + 2;
		destX[3] = destXLoc + 2;		// bottom right
		destY[3] = destYLoc + 2;

		// find distance

		for( i = 0; i < checkIndex; i++ )
		{
			Unit *unitPtr = unit_array[builderUnit[i]];
			long srcX = unitPtr->next_x_loc();
			long srcY = unitPtr->next_y_loc();

			for( j = 0; j < checkIndex; j++ )
			{	// don't use points distance
				dist2[i][j] = (srcX-destX[j])*(srcX-destX[j]) + (srcY-destY[j])*(srcY-destY[j]);
			}
		}

		// find a set to minimize sum of dist2

		int bestSet[MAX_EXTRA_BUILDER] = { 0, 1, 2, 3 };	// builderUnit i to place i
		int minSumDist2 = dist2[bestSet[0]][0] + dist2[bestSet[1]][1] + dist2[bestSet[2]][2] + dist2[bestSet[3]][3];

		for( int i0 = 0; i0 < MAX_EXTRA_BUILDER; i0++ )
		{
			for( int i1 = 0; i1 < MAX_EXTRA_BUILDER; i1++ )
			{
				if( i1 == i0 )
					continue;
				for( int i2 = 0; i2 < MAX_EXTRA_BUILDER; i2++ )
				{
					if( i2 == i0 || i2 == i1 )
						continue;
					for( int i3 = 0; i3 < MAX_EXTRA_BUILDER; i3++ )
					{
						if( i3 == i0 || i3 == i1 || i3 == i2 )
							continue;
						int sumDist2 = dist2[i0][0] + dist2[i1][1] + dist2[i2][2] + dist2[i3][3];
						if( sumDist2 < minSumDist2 )
						{
							minSumDist2 = sumDist2;
							bestSet[0] = i0;
							bestSet[1] = i1;
							bestSet[2] = i2;
							bestSet[3] = i3;
						}
					}
				}
			}
		}


		for(j = 0; j < MAX_EXTRA_BUILDER; j++)
		{
			Unit *unitPtr = unit_array[builderUnit[bestSet[j]]];
			unitPtr->move_to(destX[j], destY[j]);
			unitPtr->cur_order.set(UNIT_TRANSFORM_FORTRESS, 0, destX[j], destY[j]);
		}
	}
}
// -------- end of function UnitGroup::transform_mfort --------//
// ####### end Gilbert 25/1 ########//

//-----------Begin of function UnitGroup::run_action--------------//
//
// This function designate a troop to move to the given destination.
// PathReuse is called by the troop to find the path to the destination
//
// <int>  destXLoc, destYLoc - the destination to move to.
// <int>  orderId            - order id. of the action
// <int>  orderPara			  - order para of the action.
// [int]  actionNationRecno  - nation recno order the action, (default:-1) // need for assign
// [bool] forceMoveFlag		  - force move flag (default: false)
//
void UnitGroup::run_action(int destXLoc, int destYLoc, int orderId, int orderPara, int actionNationRecno, bool forceMoveFlag)
{
	//--------------------------------------------------------------//

	int	  minXLoc=0xFFFF, minYLoc=0xFFFF, maxXLoc=0, maxYLoc=0;
	int	  moveOffsetX=0, moveOffsetY=0;
	Unit* unitPtr;
	bool isInFormation = false;
	int nationRecno = 0;
	int centerXLoc, centerYLoc;
	int thisDestXLoc, thisDestYLoc;

	// ####### begin Gilbert 15/5 ########//
	// for attack and assign
	// cur_order set to destXLoc,destYLoc
	// but move to destCenterX, destCenterY
	int destCenterX = destXLoc;
	int destCenterY = destYLoc;
	switch( orderId )
	{
	case UNIT_ATTACK:
	case UNIT_ASSIGN:
		if( !base_obj_array.is_deleted( orderPara) )
		{
			BaseObj *destObj = base_obj_array[orderPara];
			destCenterX = (destObj->obj_loc_x1() + destObj->obj_loc_x2())/2;
			destCenterY = (destObj->obj_loc_y1() + destObj->obj_loc_y2())/2;
		}
		break;
	}
	// ####### end Gilbert 15/5 ########//
	
	if( size() > 0 )
	{
		for( int i=1 ; i<=size() ; i++ )
		{
			unitPtr = get_unit(i);
			err_when(unitPtr->next_x_loc() >= MAX_WORLD_X_LOC);

			if(i==1)
				nationRecno = unitPtr->nation_recno;

			if( unitPtr->obj_loc_x1() < minXLoc )
				minXLoc = unitPtr->obj_loc_x1();

			if( unitPtr->obj_loc_y1() < minYLoc )
				minYLoc = unitPtr->obj_loc_y1();

			if( unitPtr->obj_loc_x2() > maxXLoc )
				maxXLoc = unitPtr->obj_loc_x2();

			if( unitPtr->obj_loc_y2() > maxYLoc )
				maxYLoc = unitPtr->obj_loc_y2();

			if(unitPtr->is_in_formation())
				isInFormation = true;
		}

		centerXLoc = (maxXLoc+minXLoc)/2;
		centerYLoc = (maxYLoc+minYLoc)/2;

		// ####### begin Gilbert 15/5 #########//
		// moveOffsetX = destXLoc - centerXLoc;
		// moveOffsetY = destYLoc - centerYLoc;
		moveOffsetX = destCenterX - centerXLoc;
		moveOffsetY = destCenterY - centerYLoc;
		// ####### end Gilbert 15/5 #########//
	}

	//-------------------------------------//

	for( int i=1 ; i<=size() ; i++ )
	{
		unitPtr = get_unit(i);
		unitPtr->stop_move();

		thisDestXLoc = unitPtr->obj_loc_x1() + moveOffsetX;
		thisDestYLoc = unitPtr->obj_loc_y1() + moveOffsetY;

		thisDestXLoc = MAX( 0, thisDestXLoc );
		thisDestYLoc = MAX( 0, thisDestYLoc );
		thisDestXLoc = MIN( MAX_WORLD_X_LOC-unitPtr->obj_loc_width(), thisDestXLoc );
		thisDestYLoc = MIN( MAX_WORLD_Y_LOC-unitPtr->obj_loc_height(), thisDestYLoc );

		unitPtr->pre_move(thisDestXLoc, thisDestYLoc);
	}

	//------ call PathReuse to seek the path -------//
	if(isInFormation)
	{
#ifdef DEBUG
		unsigned long pathReuseStartTime = m.get_time();
#endif
		path_reuse.set_handle_power_nation(0);
		if(nationRecno)
		{
			Nation* nationPtr = nation_array[nationRecno];
			int srcLocPowerNation = world.get_loc(centerXLoc, centerYLoc)->power_nation_recno;
			int destLocPowerNation = world.get_loc(thisDestXLoc, thisDestYLoc)->power_nation_recno;
			if( (!srcLocPowerNation  || nationPtr->get_relation_passable(srcLocPowerNation)) &&
				 (!destLocPowerNation || nationPtr->get_relation_passable(destLocPowerNation)) )
			{
				path_reuse.set_handle_power_nation(1, nationPtr->relation_passable_array, nationRecno );
			}
		}
		path_reuse.init(this);
		path_reuse.find_paths();
#ifdef DEBUG
		path_reuse_profile_time += m.get_time()-pathReuseStartTime;
#endif
	}
	else
		// ###### begin Gilbert 15/5 ########//
		// cluster_units_for_path_reuse(MAX(0, destXLoc), MAX(0, destYLoc), centerXLoc, centerYLoc);
		cluster_units_for_path_reuse(MAX(0, destCenterX), MAX(0, destCenterY), centerXLoc, centerYLoc);
		// ###### end Gilbert 15/5 ########//

	for(int i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);

		//--- if path_reuse.find_paths() failed to create a path for this unit ---//

		if( !unitPtr->cur_path )
		{
			unitPtr->handle_blocking();
			continue;
		}

		int savePathResultId = unitPtr->cur_path_result_id;

		//------ set the action now -------//

		int rc=0;

		switch( orderId )
		{
			case UNIT_ATTACK:
			{
				int attackTargetRecno = orderPara;

				if( attackTargetRecno != unitPtr->base_obj_recno )
					rc = unitPtr->set_attack(attackTargetRecno, true);		// true - reset current order
				break;
			}

			case UNIT_ASSIGN:
				rc = unitPtr->set_assign(destXLoc, destYLoc, actionNationRecno, forceMoveFlag);
				break;

			case UNIT_SETTLE_TOWN:
				rc = unitPtr->set_settle_town(destXLoc, destYLoc);
				break;
		}

		//--- if the order is a move order or the attack or assign order failed, change it to a move order ---//

		// ######## begin Gilbert 15/5 ########//
		if( !rc )
//			unitPtr->set_move(destXLoc, destYLoc, forceMoveFlag);
			unitPtr->set_move(destCenterX, destCenterY, forceMoveFlag);
		// ######## end Gilbert 15/5 ########//

		unitPtr->cur_path_result_id = savePathResultId; 	// restore it as UnitB::stop_move() resets it

		if( unitPtr->cur_action != SPRITE_MOVE )
			unitPtr->next_move();
	}
}
//---------------End of function UnitGroup::run_action---------------------//


//----------- Begin of function UnitGroup::return_camp ---------------//
//
void UnitGroup::return_camp(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNITS_RETURN_CAMP, get_write_len() );
		write_mem(shortPtr);
		return;
	}

	Unit* unitPtr;

	for( int i=1 ; i<=size() ; i++ )
	{
		unitPtr = get_unit(i);

		if( unitPtr->home_camp_firm_recno )
			unitPtr->return_camp();
	}
}
//------------ End of function UnitGroup::return_camp ----------------//


//--------- Begin of function UnitGroup::right_click ---------//
//
// This function is called when this unit is currenlty selected
// and the player right clicks on a location.
//
void UnitGroup::right_click(int xLoc, int yLoc, char remoteAction, bool addWayPoint, bool shiftKeyAttack, bool forceMoveFlag )
{
	//### begin juliet 28/1/99 ###//
	if(size() == 0)
		return;
	//### end juliet 28/1/99 #####//

	err_when( &unit_group_land==this || &unit_group_air==this );		// unit_group_land & unit_group_air are for storing the result, this UnitGroup cannot be one of them

	//--- reset has_way_point if this is a player action ---//

	if( remoteAction == COMMAND_PLAYER )
	{
	//	unit_array[unit_array.selected_recno]->sound("ACK");

		//---- if ALT is pressed, add a way point ----//

		if( addWayPoint )
		{
			if( !remote.is_enable() )		// disable way point in multiplayer
			{
				way_point_array.add_point(xLoc, yLoc);
			}
			return;
		}

		//--- reset has_way_point when a new order is issued by the player ---//

//		if( !remoteAction && remote.is_enable() )
//		{
//			// packet structure : <write_mem>
//			short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNITS_CLEAR_HAS_WAY_POINT, get_write_len() );
//			write_mem(shortPtr);
//		}
//		else
//		{
			for( int i=size() ; i>0 ; i-- )
				get_unit(i)->has_way_point = false;
//		}
	}

	//--------- group the unit by mobile type -------//

//	group_by_mobile_type();

	//-------- check the clicked location --------//

	Location* locPtr = world.get_loc(xLoc, yLoc);

	int baseObjRecno = locPtr->base_obj_recno();

	Unit* unitPtr = NULL;
	for( int i = 1; i <= size(); ++i )
	{
		if( is_visible(i) )
		{
			unitPtr = get_unit(i);
			break;
		}
	}
	if( !unitPtr )
		return;
	int curNationRecno = unitPtr->nation_recno;   	// the nation recno of the selected unit, it is not necessarily the player's nation, as the unit can be a spy

	//----- if click on a location with an object -----//
	if( baseObjRecno && !base_obj_array[baseObjRecno]->is_stealth() )
	{
		int targetNationRecno = base_obj_array[baseObjRecno]->nation_recno;
		int attackNow=0;

		if( size() == 1 && unitPtr->cast_to_UnitGod() 
			&& unitPtr->cast_to_UnitGod()->right_click(xLoc, yLoc, 0) )
		{
			if (remoteAction == COMMAND_PLAYER && unitPtr->is_own())
				unitPtr->sound("ACK");
			return;
		}

		//--- Special case: if the target is a Porcupine, own units can attack it ---//

		
		if( targetNationRecno == nation_array.player_recno )
		{
			Unit* thisUnit = base_obj_array[baseObjRecno]->cast_to_Unit();

			if( thisUnit && thisUnit->unit_id == UNIT_EXPLOSIVE_CART && shiftKeyAttack )
			{
				attackNow=1;
			}
		}

		//--- if the target obj's nation is the same as this unit, assign ---//

		if( !attackNow && 
			// targetNationRecno==curNationRecno &&			// may be different from trevor's version
			  base_obj_array[baseObjRecno]->can_accept_assign(unitPtr->sprite_recno, nation_array.player_recno) )
			  // this is for the cases like inn where the assignee must be independent while the target is not independent 
		{
			//--- whether the target object accepts this unit assigning to it ---//

			assign(xLoc, yLoc, base_obj_array[baseObjRecno]->obj_mobile_type(), remoteAction, nation_array.player_recno, forceMoveFlag);
			if (remoteAction == COMMAND_PLAYER && unitPtr->is_own())
				unitPtr->sound("ACK");
			return;
		}
		else //--- if the nations are different, attack -----//
		{
			attack(baseObjRecno, remoteAction);

			if (remoteAction == COMMAND_PLAYER && unitPtr->is_own())
				unitPtr->sound("ATK");
			return;
		}
	}

	//---- if all conditions failed, move to the destination ----//

	if (remoteAction == COMMAND_PLAYER && unitPtr->is_own())
		unitPtr->sound("ACK");
	move(xLoc, yLoc, forceMoveFlag, remoteAction);
}
//--------- End of function UnitGroup::right_click ---------//

//--------- Begin of function UnitGroup::group_by_mobile_type ---------//
//
void UnitGroup::group_by_mobile_type()
{
	err_when( &unit_group_land==this || &unit_group_air==this );		// unit_group_land & unit_group_air are for storing the result, this UnitGroup cannot be one of them

	//--- add the selected units to UnitGroup ----//

	unit_group_land.zap();
	unit_group_air.zap();

	for( int i=size() ; i>0 ; i-- )
	{
		Unit* unitPtr = get_unit(i);

		if( unit_array.is_deleted(unitPtr->sprite_recno) )
			continue;

		if( unitPtr->mobile_type == UNIT_LAND )
			unit_group_land.add_unit(unitPtr->sprite_recno);
		else
			unit_group_air.add_unit(unitPtr->sprite_recno);
	}

	unit_group_land.mobile_type  = UNIT_LAND;
	unit_group_air.mobile_type = UNIT_AIR;

	err_when( (size() > 0) && (unit_group_land.size()==0 && unit_group_air.size()==0) );		// there should be at least one unit selected
}
//--------- End of function UnitGroup::group_by_mobile_type ---------//


//--------- Begin of function UnitGroup::formation ---------//
//
// set the units to form formation
//
void UnitGroup::formation(int formationType, char remoteAction)
{
	//-- only form formation when there is a general in the group --//

	int i;
	for( i=size() ; i>0 ; i-- )
	{
		Unit* unitPtr = get_unit(i);

		if( unitPtr->rank_id >= RANK_GENERAL )
			break;
	}

	if( i==0 )		// no general in the group.
		return;

	//---------------------------------//

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <formation id> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNITS_FORMATION, sizeof(short) + get_write_len() );
		shortPtr[0] = formationType;
		write_mem(shortPtr+1);
		return;
	}

	// ##### begin Gilbert 25/2 ######//

	// -------- set team_info's formation_id ------//

	for( i=size() ; i>0 ; i-- )
	{
		Unit* unitPtr = get_unit(i);

		if( unitPtr->team_info )
		{
			unitPtr->team_info->formation_id = formationType;
		}
	}

	// ------- if any formation execute now --------//

	if( formationType != 0 )
	{
		group_by_mobile_type();

		//-------- set formation now ---------//

		unit_group_land.exe_formation(formationType);
		unit_group_air.exe_formation(formationType);
	}
	// ##### end Gilbert 25/2 ######//
}
//--------- End of function UnitGroup::formation ---------//


//----------- Begin of function UnitGroup::exe_formation ----------//
//
void UnitGroup::exe_formation(int formationType)
{
	if( size()==0 )
		return;

	err_when( mobile_type != UNIT_LAND && mobile_type != UNIT_AIR );	// mobile_type must be set first before calling this function

	switch(formationType)
	{
		case FORMATION_LINE:
			line_formation();
			break;

		case FORMATION_CIRCLE:
			circle_formation();
			break;

		case FORMATION_VANGUARD:
			vanguard_formation();
			break;

		case FORMATION_ARC:
			arc_formation();
			break;

		case FORMATION_STAGGERED:
			staggered_formation();
			break;

		case FORMATION_PRE_SQUARE:
			square_preformation();
			break;

		case FORMATION_SQUARE:
			square_formation();
			break;

		case FORMATION_SANDWICH:
			sandwich_formation();
			break;

		default:
			err_here();
	}
}
//----------End if function UnitGroup::exe_formation---------------//

//----------Begin of function UnitGroup::formation_turn--------//
//
void UnitGroup::formation_turn(int direction, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <direction id> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNITS_FORM_TURN, sizeof(short) + get_write_len() );
		shortPtr[0] = direction;
		write_mem(shortPtr+1);
		return;
	}

		//-- only form formation when there is a general in the group --//
	int i;
	for( i=size() ; i>0 ; i-- )
	{
		Unit* unitPtr = get_unit(i);

		if( unitPtr->rank_id >= RANK_GENERAL )
			break;
	}

	if( i==0 )		// no general in the group.
		return;

	group_by_mobile_type();

	if(unit_group_land.size())
		unit_group_land.exe_formation_turn(direction);
	if(unit_group_air.size())
		unit_group_air.exe_formation_turn(direction);
}

void UnitGroup::exe_formation_turn(int direction)
{
	Unit* unitPtr = NULL;
	int formationMode=0;

	for(int i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		
		if(unitPtr->team_info)
		{
			formationMode = unitPtr->team_info->formation_id;
			break;
		}
		else if(unitPtr->leader_unit_recno && !unit_array.is_deleted(unitPtr->leader_unit_recno) &&
			(unitPtr = unit_array[unitPtr->leader_unit_recno])->team_info)
		{
			formationMode = unitPtr->team_info->formation_id;
			break;
		}
	}

//	if(!formationMode)
//		formationMode = unit_array[get_unit(1)->leader_unit_recno]->team_info->formation_id;

	switch(formationMode)
	{
		case FORMATION_VANGUARD:
			vanguard_formation(direction);
			break;
		case FORMATION_LINE:
			line_formation(direction);
			break;
		case FORMATION_CIRCLE:
			circle_formation(direction);
			break;
		case FORMATION_ARC:
			arc_formation(direction);
			break;
		case FORMATION_STAGGERED:
			staggered_formation(direction);
			break;
		case FORMATION_SANDWICH:	
			sandwich_formation(direction);
			break;
		case FORMATION_SQUARE:
			square_formation(direction);
			break;
		default:
			return;
	}

	if(unitPtr && unitPtr->team_info)
		unitPtr->team_info->formation_direction = direction;
}
//--------------- end of function UnitGroup::formation_turn------------//


//--------------- begin function UnitGroup::cluster_unit_for_path_reuse--------//
//
// cluster units that are close together to use path reuse together
//
void UnitGroup::cluster_units_for_path_reuse(int destX, int destY, int centerX, int centerY)
{
	int ind=0, ind2, ind3;  // some variables for indexing		
	int minx, miny, maxx, maxy;
	Unit* unitPtr;
	int nationRecno = get_unit(1)->nation_recno;

	int unitsSelected = size();
	// remember to mem_del at the end of function
//	Unit** units = (Unit **) mem_add(sizeof(Unit*)*unitsSelected);
//	BOOL* processed = (BOOL *) mem_add(sizeof(BOOL)*unitsSelected);
//	::ZeroMemory(processed, sizeof(BOOL)*unitsSelected);

	// path reuse has limit, so change use local auto array
	Unit *units[MAX_REUSE_NUMBER_OF_UNITS];
	static VLenQueue vLenQueueProcessed;		// reduce mem_add
	vLenQueueProcessed.clear();

	#define MAX_CLUSTERS 255
	// processed store which clusters a unit belongs to
	unsigned char *processed = (unsigned char *)vLenQueueProcessed.reserve(unitsSelected*sizeof(*processed));
	memset( processed, 0, unitsSelected*sizeof(*processed) );

	ind = 0;
	// we now cluster units together if they were close together
	const int clusterLimit = 3;
	// int theSign = 1;

	unsigned char clusterId = 0;
	ClusterInfo clusterArray[MAX_CLUSTERS];

	while(ind < unitsSelected)
	{
		if (!processed[ind])
		{
			++clusterId;
			unitPtr = get_unit(ind+1);
			processed[ind] = clusterId;
			// units[ind2 = 0] = unitPtr;
			ind2 = 0;
			ind3 = ind+1;
			minx = unitPtr->obj_loc_x1();
			miny = unitPtr->obj_loc_y1();
			maxx = unitPtr->obj_loc_x2();
			maxy = unitPtr->obj_loc_y2();

			while (ind3 < unitsSelected )
			{
				if (!processed[ind3])
				{
					Unit *thisUnit = get_unit(ind3+1);
					int thisMinX, thisMinY, thisMaxX, thisMaxY;

					thisMinX = thisUnit->obj_loc_x1();
					thisMinY = thisUnit->obj_loc_y1();
					thisMaxX = thisUnit->obj_loc_x2();
					thisMaxY = thisUnit->obj_loc_y2();
					// formula for finding if an additional unit can be part of a bigger cluster
					// ####### begin Gilbert 11/5 ######//
//					if ( (abs(minx - thisLocX) < clusterLimit ||
//							abs(maxx - thisLocX) < clusterLimit )&&
//						  (abs(miny - thisLocY) < clusterLimit ||
//							abs(maxy - thisLocY) < clusterLimit ))
					if( thisMaxX   + clusterLimit > minx
						&& thisMinX - clusterLimit < maxx
						&& thisMaxY + clusterLimit > miny
						&& thisMinY - clusterLimit < maxy )
					// ####### end Gilbert 11/5 ######//
					{
						//units[++ind2] = get_unit(ind3+1);
						++ind2;
						processed[ind3] = clusterId;
						ind3 = ind;		// restart to find again

						// update boundary of cluster
						if (thisMinX < minx) minx = thisMinX;
						if (thisMaxX > maxx) maxx = thisMaxX;
						if (thisMinY < miny) miny = thisMinY;
						if (thisMaxY > maxy) maxy = thisMaxY;

						// path reuse has a limit
						// number of unit in units array is ind2+1
						// if( ind2 >= MAX_REUSE_NUMBER_OF_UNITS-1 )
						//		break;
					}
				}
				ind3++;
			} // end while

			// finish finding a cluster

			clusterArray[clusterId-1].init( minx ,miny, maxx, maxy, ind2+1 );		// number of units is ind2+1

			if( clusterId >= MAX_CLUSTERS )
				break;
		}
		ind++;
	}

	// pack cluster together

	pack_clusters( clusterArray, clusterId, centerX, centerY );

	// if any of the cluster destination is outside map boundary, destX, destY

	int tmpX = destX;
	int tmpY = destY;

	int c = 0;
	if( clusterId > 0 )
	{
		ClusterInfo *clusterInfo = clusterArray + c;

		// find the new boundary of all clusters
		int minMinX = clusterInfo->new_minx;
		int maxMaxX = clusterInfo->new_maxx;
		int minMinY = clusterInfo->new_miny;
		int maxMaxY = clusterInfo->new_maxy;

		for( ; c < clusterId; ++c, ++clusterInfo )
		{
			if( clusterInfo->new_minx < minMinX )
				minMinX = clusterInfo->new_minx;
			if( clusterInfo->new_maxx > maxMaxX )
				maxMaxX = clusterInfo->new_maxx;
			if( clusterInfo->new_miny < minMinY )
				minMinY = clusterInfo->new_miny;
			if( clusterInfo->new_maxy > maxMaxY )
				maxMaxY = clusterInfo->new_maxy;
		}
		
		if( minMinX + tmpX - centerX < 0 )
			tmpX = centerX - minMinX;
		if( maxMaxX + tmpX - centerX >= MAX_WORLD_X_LOC )
			tmpX = MAX_WORLD_X_LOC - 1 + centerX - maxMaxX;
		if( minMinY + tmpY - centerY < 0 )
			tmpY = centerY - minMinY;
		if( maxMaxY + tmpY - centerY >= MAX_WORLD_Y_LOC )
			tmpY = MAX_WORLD_Y_LOC - 1 + centerY - maxMaxY;

		err_when(tmpX < 0);
		err_when(tmpY < 0);
		err_when(tmpX >= MAX_WORLD_X_LOC);
		err_when(tmpY >= MAX_WORLD_Y_LOC);
	}

	// do not use destX, destY any more

	// call path reuse for each cluster

	for( c = 0; c < clusterId; ++c )
	{
			// don't call PathReuse directly, instead call go_to
			// ###### begin Gilbert 13/5 #######//
			// already set in run_action
//			units[0]->move_to_loc_x = tmpX;
//			units[0]->move_to_loc_y = tmpY;
			// ###### end Gilbert 13/5 #######//

		ClusterInfo *clusterInfo = clusterArray + c;

		// make units array to pass to find_paths
		// modify move_to_loc_x/y

		// a cluster may have contain too many units ( > MAX_REUSE_NUMBER_OF_UNITS)
		// which path_reuse cannot handle
		// divide into batches

		int index = 0;
		while( index < unitsSelected )
		{
			int checkCount = 0;
			for( ; index < unitsSelected && checkCount < MAX_REUSE_NUMBER_OF_UNITS; ++index )
			{
				// procesed[index] means rejected, do not move it
				// err_when( !processed[index] );		// not inside any cluster

				if( processed[index] != c+1 )
					continue;

				units[checkCount] = get_unit(index+1);
				checkCount++;
			}

			err_when( clusterInfo->count < MAX_REUSE_NUMBER_OF_UNITS && checkCount != clusterInfo->count );
			err_when( checkCount > MAX_REUSE_NUMBER_OF_UNITS );

			if( checkCount > 0 )
			{
#ifdef DEBUG
				unsigned long pathReuseStartTime = m.get_time();
#endif
				path_reuse.set_handle_power_nation(0);
				if(nationRecno)
				{
					Nation* nationPtr = nation_array[nationRecno];
					int srcLocPowerNation = world.get_loc(units[0]->next_x_loc(), units[0]->next_y_loc())->power_nation_recno;
					int destLocPowerNation = world.get_loc(tmpX, tmpY)->power_nation_recno;
					if( (!srcLocPowerNation  || nationPtr->get_relation_passable(srcLocPowerNation)) &&
						 (!destLocPowerNation || nationPtr->get_relation_passable(destLocPowerNation)) )
					{
						path_reuse.set_handle_power_nation(1, nationPtr->relation_passable_array, nationRecno );
					}
				}

				// don't call PathReuse directly, instead call go_to
				// consider adjustment of each cluster, which is result of pack_cluster 
				// represented in new_minx - new_minx  / new_miny - new_miny

				units[0]->move_to_loc_x = units[0]->obj_loc_x1() + tmpX - centerX + 
					clusterInfo->new_minx - clusterInfo->old_minx;
				units[0]->move_to_loc_y = units[0]->obj_loc_y1() + tmpY - centerY +
					clusterInfo->new_miny - clusterInfo->old_miny;

				err_when( units[0]->move_to_loc_x < 0 );
				err_when( units[0]->move_to_loc_x + units[0]->loc_width > MAX_WORLD_X_LOC );
				err_when( units[0]->move_to_loc_y < 0 );
				err_when( units[0]->move_to_loc_y + units[0]->loc_height > MAX_WORLD_Y_LOC );

				path_reuse.init(units, checkCount );
				path_reuse.find_paths();
#ifdef DEBUG
				path_reuse_profile_time += m.get_time() - pathReuseStartTime;
#endif
			}
		}
	}
//	if(units)
//		mem_del(units);
//	if(processed)
//		mem_del(processed);
}
//--------------- end of function UnitGroup::cluster_unit_for_path_reuse-------//

//--------- Begin of static function put_profile ---------//
//
static void put_profile(int y, const char* dispDes, int dispValue)
{
	font_news.put( ZOOM_X1+10, y, dispDes );
	font_news.put( ZOOM_X1+120, y, dispValue );
}
//----------- End of static function put_profile -----------//

//----------- Begin function UnitArray::draw_profile---------//
void UnitGroup::draw_profile()
{
#ifdef DEBUG
	static unsigned long lastDrawTime = m.get_time();

	if(m.get_time() >= lastDrawTime + 1000)
	{
		//----update pathfind profile-----//
		last_path_reuse_profile_time = path_reuse_profile_time;
		path_reuse_profile_time = 0L;

		lastDrawTime = m.get_time();
	}

	//---- draw unit process profile -----//
	int y=ZOOM_Y1+100;
	put_profile(y, "PathReuse:"   	, last_path_reuse_profile_time );
#endif
}
//----------- End of function UnitArray::draw_profile----------//

//----------- Begin of function UnitGroup::cluster_units_for_attack----//
//
// cluster units that are in the same direction of the target to attack
//
void UnitGroup::cluster_units_for_attack(int targetRecno, int targetLocX, int targetLocY, int targetWidth, int targetHeight)
{
	Unit* unitPtr;
	int locX, locY;

	int right = 0;
	int left = 0;
	int midX = 0;
	int top = 0;
	int bottom = 0;
	int midY = 0;

	if(size() == 1)
	{
		unitPtr = get_unit(1);
		if(unitPtr->can_attack())
			unitPtr->attack(targetRecno, true);
		return;
	}

	int i;
	for(i = 1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		locX = unitPtr->next_x_loc();
		locY = unitPtr->next_y_loc();

		if(locX < targetLocX) //left
			left++;
		else if(locX > targetLocX+targetWidth-1) //right
			right++;
		else
			midX++;
		if(locY < targetLocY) //top
			top++;
		else if(locY > targetLocY+targetHeight-1) //bottom
			bottom++;
		else
			midY++;
	}
	if(!left && !midX) //all at right side
	{
		run_action( targetLocX, targetLocY, UNIT_ATTACK, targetRecno );
		return;
	}
	if(!right && !midX) //all at left side
	{
		run_action( targetLocX, targetLocY, UNIT_ATTACK, targetRecno );
		return;
	}
	if(!top && !midY) //all at bottom side
	{
		run_action( targetLocX, targetLocY, UNIT_ATTACK, targetRecno);
		return;
	}
	if(!bottom && !midY) //all at top side
	{
		run_action(targetLocX, targetLocY, UNIT_ATTACK, targetRecno);
		return;
	}

	for(i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		if(unitPtr->cur_action == SPRITE_ATTACK && unitPtr->cur_order.para == targetRecno)
			continue;
		if(unitPtr->can_attack())
			unitPtr->attack(targetRecno, true);
	}
}
//----------- End of function UnitGroup::cluster_units_for_attack -----//


//--------- Begin of function UnitGroup::form_team ---------//
//
// The unit group forms a team, with the highest ranked unit
// being the leader.
//
// <int> 1 - a team is formed successfully.
//		   0 - no team is formed
//
int UnitGroup::form_team()
{
	//--- make the unit with the highest rank the leader ---//

	int 	highestRank=0;
	Unit  *unitPtr, *leaderUnit=NULL;

	int i;
	for( i=size() ; i>0 ; i-- )
	{
		unitPtr = get_unit(i);

		if( unitPtr->rank_id >= highestRank )
		{
			highestRank = unitPtr->rank_id;
			leaderUnit  = unitPtr;
		}
	}

	if( !leaderUnit )
		return 0;

	err_when( !leaderUnit->team_info );

	//------ form a team with other units --------//

	leaderUnit->team_info->reset();
	leaderUnit->team_info->add_member(leaderUnit->sprite_recno);		// add itself to the team first

	for( i=size() ; i>0 ; i-- )
	{
		unitPtr = get_unit(i);

		if( unitPtr->rank_id == RANK_SOLDIER )
		{
			leaderUnit->team_info->add_member(unitPtr->sprite_recno);
			unitPtr->leader_unit_recno = leaderUnit->sprite_recno;
		}
	}

	return 1;
}
//----------- End of function UnitGroup::form_team -----------//


// ---------- begin of static function pack_clusters ---------//
//
// move clusters closer together
// change only new_xxxx members
//
static void pack_clusters( ClusterInfo *clusterArray, int clusterCount, int centerX, int centerY )
{
	if( clusterCount <= 1 )
		return;

	int i, j;

	if( centerX == -1 && centerY == -1 )
	{
		centerX = 0;
		centerY = 0;

		// not use weight mean, to avoid overflow
		for( i = 0; i < clusterCount; ++i )
		{
			centerX += clusterArray[i].new_minx + clusterArray[i].new_maxx;
			centerY += clusterArray[i].new_miny + clusterArray[i].new_maxy;
		}

		centerX = centerX / clusterCount / 2;
		centerY = centerY / clusterCount / 2;
	}

	int loopCount = 4;
	int moveFlag = 0;

	do
	{
		moveFlag = 0;

		for( i = 0; i < clusterCount; ++i )
		{
			int &iminx = clusterArray[i].new_minx;
			int &iminy = clusterArray[i].new_miny;
			int &imaxx = clusterArray[i].new_maxx;
			int &imaxy = clusterArray[i].new_maxy;
			int iclusterSize = clusterArray[i].count;

			// find the distance the area can translate
			// find the minimum distance between other areas
			// it is the area it can move without colliding with other area

			int freeDist = m.area_distance( iminx, iminy, imaxx, imaxy,
				centerX, centerY, centerX, centerY );

			if( freeDist == 0 )		// overlapped don't consider ( if possible, translate together...)
				continue;

			for( j = 0; j < clusterCount; ++j )
			{
				if( j == i )
					continue;

				// one less to the measure the gap between areas
				int dist = m.area_distance( iminx, iminy, imaxx, imaxy,
					clusterArray[j].new_minx, clusterArray[j].new_miny, 
					clusterArray[j].new_maxx, clusterArray[j].new_maxy ) - 1; 

				// if either cluster is large
				// further decrease the gap so large group keep separated a little
				if( iclusterSize > MAX_TEAM_MEMBER
					|| clusterArray[j].count > MAX_TEAM_MEMBER )
				{
					dist -= 1;
				}

				if( dist < freeDist )
					freeDist = dist;
			}
			// move towards center

			if( freeDist > 0 )
			{
				if( iminx > centerX )
				{
					iminx -= freeDist;		// will not shift over centerX because 
					imaxx -= freeDist;		// distance of cluster[i] and centerX is considered
					moveFlag = 1;
				} 
				else if( imaxx < centerX )
				{
					iminx += freeDist;
					imaxx += freeDist;
					moveFlag = 1;
				}

				if( iminy > centerY )
				{
					iminy -= freeDist;
					imaxy -= freeDist;
					moveFlag = 1;
				}
				if( imaxy < centerY )
				{
					iminy += freeDist;
					imaxy += freeDist;
					moveFlag = 1;
				}
			}
		}
	} while(moveFlag && --loopCount > 0);
}

// ---------- end of static function pack_clusters ---------//
