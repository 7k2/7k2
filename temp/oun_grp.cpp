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

#include <ALL.H>
#include <OMOUSE.H>
#include <OPOWER.H>
#include <ONATION.H>
#include <OWAYPNT.H>
#include <OUN_GRP.H>
#include <OPREUSE.H>
#include <OREMOTE.H>
#include <OFONT.H>
#include <OF_MFORT.H>

static void put_profile(int y, char* dispDes, int dispValue);

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
	const memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(short) - 1) / sizeof(short);

	// <member in UnitGroup> <unit count> <unit recno> ...
	return memberSize * sizeof(short) + sizeof(short) + ele_size * size();
}
//--------- End of function UnitGroup::get_write_len ---------//


//--------- Begin of function UnitGroup::write_mem ---------//
//
void UnitGroup::write_mem( short *msg )
{
	const memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(*msg) - 1) / sizeof(*msg);
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
	const memberSize = (sizeof(*this) - sizeof(DynArray) + sizeof(*msg) - 1) / sizeof(*msg);
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
		int xLoc1 = max(targetLocX-minAttackRange, 0);
		int yLoc1 = max(targetLocY-minAttackRange, 0);
		int xLoc2 = min(targetLocX+targetWidth-1+minAttackRange, MAX_WORLD_X_LOC-1);
		int yLoc2 = min(targetLocY+targetHeight-1+minAttackRange, MAX_WORLD_Y_LOC-1);
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
	run_action( destXLoc, destYLoc, UNIT_MOVE, 0, forceMoveFlag );
}
//---------------End of function UnitGroup::move---------------------//


//----------- Begin of function UnitGroup::assign ---------------//
//
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
void UnitGroup::assign(int destXLoc, int destYLoc, int targetMobileType, char remoteAction, int actionNationRecno)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <xLoc> <yLoc> <write_mem...>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_ASSIGN, sizeof(short)*2 + get_write_len() );
		shortPtr[0] = destXLoc;
		shortPtr[1] = destYLoc;
		write_mem(shortPtr+2);
		return;
	}

	group_by_mobile_type();

	if(unit_group_land.size())
		unit_group_land.exe_assign(destXLoc, destYLoc, targetMobileType, actionNationRecno);
	if(unit_group_air.size())
		unit_group_air.exe_assign(destXLoc, destYLoc, targetMobileType, actionNationRecno);
}

void UnitGroup::exe_assign(int destXLoc, int destYLoc, int targetMobileType, int actionNationRecno)
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
		unitPtr->assign(destXLoc, destYLoc, actionNationRecno);
		return;
	}

	Location* locPtr;

	locPtr = world.get_loc(destXLoc, destYLoc);
	int baseObjRecno = locPtr->base_obj_recno(targetMobileType); 
	BaseObj* thePlace = base_obj_array[baseObjRecno];
	int destWidth = thePlace->obj_loc_width();
	int destHeight = thePlace->obj_loc_height();

	for(int i = 1; i<=size(); i++)
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
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno );
		return;
	}
	if(!right && !midX) //all at left side
	{
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno );
		return;
	}
	if(!top && !midY) //all at bottom side
	{
		run_action( destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno);
		return;
	}
	if(!bottom && !midY) //all at top side
	{
		run_action(destXLoc, destYLoc, UNIT_ASSIGN, baseObjRecno);
		return;
	}

	for(i=1; i<=size(); i++)
	{
		unitPtr = get_unit(i);
		unitPtr->assign(destXLoc, destYLoc, actionNationRecno);
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

	for(int i = 1; i <= size() && checkIndex < MAX_EXTRA_BUILDER; i++)
	{
		Unit *unitPtr = get_unit(i);

		if(unitPtr->unit_id == cmdUnit->unit_id && unitPtr->nation_recno == cmdUnit->nation_recno)
		{
			builderUnit[checkIndex] = unitPtr->sprite_recno;
			checkIndex++;
		}
	}
	if(checkIndex == MAX_EXTRA_BUILDER)
	{
		for(i = 0; i < checkIndex; i++)
		{
			int locX = destXLoc+(i%2)*(cmdUnit->obj_loc_width());
			int locY = destYLoc+(i/2)*(cmdUnit->obj_loc_height());
			Unit *unitPtr = unit_array[builderUnit[i]];
			unitPtr->move_to(locX, locY);
			unitPtr->cur_order.set(UNIT_TRANSFORM_FORTRESS, 0, locX, locY);
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
// [bool] forceMoveFlag		  - force move flag (default: false)
//
void UnitGroup::run_action(int destXLoc, int destYLoc, int orderId, int orderPara, bool forceMoveFlag)
{
	//--------------------------------------------------------------//

	int	  minXLoc=0xFFFF, minYLoc=0xFFFF, maxXLoc=0, maxYLoc=0;
	int	  moveOffsetX=0, moveOffsetY=0;
	Unit* unitPtr;
	bool isInFormation = false;
	int nationRecno = 0;
	int centerXLoc, centerYLoc;
	int thisDestXLoc, thisDestYLoc;
	
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

		moveOffsetX = destXLoc - centerXLoc;
		moveOffsetY = destYLoc - centerYLoc;
	}

	//-------------------------------------//

	for( int i=1 ; i<=size() ; i++ )
	{
		unitPtr = get_unit(i);
		unitPtr->stop_move();

		thisDestXLoc = unitPtr->obj_loc_x1() + moveOffsetX;
		thisDestYLoc = unitPtr->obj_loc_y1() + moveOffsetY;

		thisDestXLoc = max( 0, thisDestXLoc );
		thisDestYLoc = max( 0, thisDestYLoc );
		thisDestXLoc = min( MAX_WORLD_X_LOC-unitPtr->obj_loc_width(), thisDestXLoc );
		thisDestYLoc = min( MAX_WORLD_Y_LOC-unitPtr->obj_loc_height(), thisDestYLoc );

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
		cluster_units_for_path_reuse(max(0, destXLoc), max(0, destYLoc));

	for(i=1; i<=size(); i++)
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
				rc = unitPtr->set_assign(destXLoc, destYLoc);
				break;

			case UNIT_SETTLE_TOWN:
				rc = unitPtr->set_settle_town(destXLoc, destYLoc);
				break;
		}

		//--- if the order is a move order or the attack or assign order failed, change it to a move order ---//

		if( !rc )
			unitPtr->set_move(destXLoc, destYLoc, forceMoveFlag);

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
		unit_array[unit_array.selected_recno]->sound("ACK");

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

	Unit* unitPtr = get_unit(1);
	int curNationRecno = unitPtr->nation_recno;   	// the nation recno of the selected unit, it is not necessarily the player's nation, as the unit can be a spy

	//----- if click on a location with an object -----//
	// ###### begin Gilbert 19/2 #########//
	if( baseObjRecno && !base_obj_array[baseObjRecno]->is_stealth() )
	// ###### end Gilbert 19/2 #########//
	{
		int targetNationRecno = base_obj_array[baseObjRecno]->nation_recno;
		int attackNow=0;

		//--- Special case: if the target is a Porcupine, own units can attack it ---//

		
		if( targetNationRecno == nation_array.player_recno )
		{
			Unit* thisUnit = base_obj_array[baseObjRecno]->cast_to_Unit();

			if( thisUnit && thisUnit->unit_id == UNIT_EXPLOSIVE_CART && shiftKeyAttack )
				attackNow=1;
		}

		//--- if the target obj's nation is the same as this unit, assign ---//

		if( !attackNow && 
			// ######## begin Gilbert 24/2 ####### //
			// targetNationRecno==curNationRecno &&			// may be different from trevor's version
			  base_obj_array[baseObjRecno]->can_accept_assign(unitPtr->sprite_recno, nation_array.player_recno) )
			  // this is for the cases like inn where the assignee must be independent while the target is not independent 
			// ######## end Gilbert 24/2 ####### //
		{
			//--- whether the target object accepts this unit assigning to it ---//

			assign(xLoc, yLoc, base_obj_array[baseObjRecno]->obj_mobile_type(), remoteAction, nation_array.player_recno);
//			if( unit_group_land.size() > 0 )
//				unit_group_land.assign( xLoc, yLoc, remoteAction, nation_array.player_recno );
//
//			if( unit_group_air.size() > 0 )
//				unit_group_air.assign( xLoc, yLoc, remoteAction, nation_array.player_recno );

			return;
		}
		else //--- if the nations are different, attack -----//
		{
			/*
			if( unit_group_land.size() > 0 )
				unit_group_land.attack( baseObjRecno, remoteAction );

			if( unit_group_air.size() > 0 )
				unit_group_air.attack( baseObjRecno, remoteAction );
			*/
			attack(baseObjRecno, remoteAction);

			return;
		}
	}

	//---- if all conditions failed, move to the destination ----//
/*
	if( unit_group_land.size() > 0 )
		unit_group_land.move( xLoc, yLoc, forceMoveFlag, remoteAction );

	if( unit_group_air.size() > 0 )
		unit_group_air.move( xLoc, yLoc, forceMoveFlag, remoteAction );
*/
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

	for( int i=size() ; i>0 ; i-- )
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
	for( int i=size() ; i>0 ; i-- )
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
void UnitGroup::cluster_units_for_path_reuse(int destX, int destY)
{
	int ind=0, ind2, ind3;  // some variables for indexing		
	int minx, miny, maxx, maxy;
	Unit* unitPtr;
	int thisLocX, thisLocY;
	int tmpX, tmpY;
	int nationRecno = get_unit(1)->nation_recno;

	int unitsSelected = size();
	Unit** units = (Unit **) mem_add(sizeof(Unit*)*unitsSelected);
	BOOL* processed = (BOOL *) mem_add(sizeof(BOOL)*unitsSelected);
	::ZeroMemory(processed, sizeof(BOOL)*unitsSelected);

	tmpX = destX;
	tmpY = destY;
	ind = 0;
	// we now cluster units together if they were close together
	const int clusterLimit = 3;
	int theSign = 1;

	while(ind < unitsSelected)
	{
		if (!processed[ind])
		{
			unitPtr = get_unit(ind+1);
			processed[ind] = true;
			units[ind2 = 0] = unitPtr;
			ind3 = ind+1;
			minx = maxx = unitPtr->next_x_loc();
			miny = maxy = unitPtr->next_y_loc();
			while (ind3 < unitsSelected)
			{
				if (!processed[ind3])
				{
					thisLocX = get_unit(ind3+1)->next_x_loc();
					thisLocY = get_unit(ind3+1)->next_y_loc();
					// formula for finding if an additional unit can be part of a bigger cluster
					if ( (abs(minx - thisLocX) < clusterLimit ||
							abs(maxx - thisLocX) < clusterLimit )&&
						  (abs(miny - thisLocY) < clusterLimit ||
							abs(maxy - thisLocY) < clusterLimit ))
					{
						units[++ind2] = get_unit(ind3+1);
						processed[ind3] = true;
						ind3 = ind;
						if (thisLocX < minx) minx = thisLocX;
						if (thisLocX > maxx) maxx = thisLocX;
						if (thisLocY < miny) miny = thisLocY;
						if (thisLocY > maxy) maxy = thisLocY;
					}
				}
				ind3++;
			}
			// don't call PathReuse directly, instead call go_to
			units[0]->move_to_loc_x = tmpX;
			units[0]->move_to_loc_y = tmpY;
			err_when(tmpX < 0);
			err_when(tmpY < 0);
			err_when(tmpX >= MAX_WORLD_X_LOC);
			err_when(tmpY >= MAX_WORLD_Y_LOC);
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
			path_reuse.init(units, ind2+1);
			path_reuse.find_paths();
#ifdef DEBUG
			path_reuse_profile_time += m.get_time() - pathReuseStartTime;
#endif
			int index;
			int theX, theY;
			int theSizeX, theSizeY;
			int rightmostDest, lowermostDest;
			rightmostDest = lowermostDest = -9999; //garbage
			for(index = 0; index < ind2+1; index++)
			{
				if((theX = (units[index]->move_to_loc_x)) > rightmostDest)
				{
					rightmostDest = theX;
					theSizeX = units[index]->obj_loc_width();
				}
				if((theY = (units[index]->move_to_loc_y)) > lowermostDest)
				{
					lowermostDest = theY;
					theSizeY = units[index]->obj_loc_height();
				}
			}
			if(theSign%2 && (rightmostDest+theSizeX<MAX_WORLD_X_LOC))
				tmpX = rightmostDest+theSizeX;
			else
			{
				if(lowermostDest+theSizeY < MAX_WORLD_Y_LOC)
				{
					tmpX = destX;
					tmpY = lowermostDest+theSizeY;
				}
				else if(rightmostDest+theSizeX<MAX_WORLD_X_LOC)
				{
					tmpY = destY;
					tmpX = rightmostDest+theSizeX;
				}
				else
				{
					tmpX = destX;
					tmpY = destY;
				}
			}
			theSign++;
		}
		ind++;			
	}
	if(units)
		mem_del(units);
	if(processed)
		mem_del(processed);

}
//--------------- end of function UnitGroup::cluster_unit_for_path_reuse-------//

//--------- Begin of static function put_profile ---------//
//
static void put_profile(int y, char* dispDes, int dispValue)
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

	for(int i = 1; i<=size(); i++)
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

	for( int i=size() ; i>0 ; i-- )
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
