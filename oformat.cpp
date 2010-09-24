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

//Filename		: OFORMAT.CPP
//Description	: Unit Group Formation

#include <OUN_GRP.H>
#include <ALL.H>
#include <OUNIT.H>
#include <MATH.H>

//--- Begin of function UnitGroup::vanguard_formation ---//
// This function calculate the location, and set the unit to move to, to form a
// vanguard formation.
// 
// <int> direction -- direction of the formation
//							 default = 0, for not specifying direction
//							
void UnitGroup::vanguard_formation(int direction)
{
//----------Define local variables------------//
	int nSelected = 0; 
	int locX, locY, i, j, t, tempLocX, tempLocY, tSelected =0;
	int temperoryX, temperoryY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int maxLocX, minLocX, maxLocY, minLocY;
	Unit* unitPtr;
	int finalDirection=0;
//--------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	maxLocX = 0;
	minLocX = MAX_WORLD_X_LOC;
	maxLocY = 0;
	minLocY = MAX_WORLD_Y_LOC;
	int nClose = 0, nRange = 0;
	Unit* general=NULL;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		else if(unitPtr->max_attack_range() > 1)
			nRange++;
		else
			nClose++;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();

		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}

	bool generalIsHere = true;

	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}

	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	while(nRange > nClose+4)
	{
		nRange-= 2;
		nClose+= 2;
	}
	if(nRange > 5)
	{
		nRange = 5;
		nClose = nSelected-nRange-1;
	}

	int centerX = general->next_x_loc();
	int centerY = general->next_y_loc();

	int ir = 0;
	int ic = 0;
	t = 1;

	Unit** rangeAttackUnits = (Unit**) mem_add_clear(nRange*sizeof(Unit*));
	Unit** closeAttackUnits = (Unit**) mem_add_clear(nClose*sizeof(Unit*));

	while(t<=nSelected)
	{
		unitPtr = get_unit(t++);
		if(!unitPtr->team_info && unitPtr->max_attack_range() > 1 && ir<nRange)
			rangeAttackUnits[ir++] = unitPtr;
		else if(!unitPtr->team_info)
			closeAttackUnits[ic++]  = unitPtr;
		err_when(ic>nClose);
	}
	err_when(ic != nClose);

	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<nClose; i++)
	{
		for(j=0; j<i; j++)
		{
			if(closeAttackUnits[j]->max_attack_range() > closeAttackUnits[i]->max_attack_range())
			{
				tempUnit = closeAttackUnits[i];
				closeAttackUnits[i] = closeAttackUnits[j];
				closeAttackUnits[j] = tempUnit;
			}
		}
	}

	int lengthRight = 0;
	int lengthLeft = 0;
	int widthRight = 0;
	int widthLeft = 0;
	int trueWidth, trueLength;

	for(ic=0; ic<nClose; ic++)
	{
		if(ic==0 || ic%2)
		{
			lengthRight += maximumSizeX;
			widthRight += maximumSizeY;
		}
		else
		{
			lengthLeft += maximumSizeX;
			widthLeft += maximumSizeY;
		}
		err_when(ic>0 && closeAttackUnits[ic]->max_attack_range() < closeAttackUnits[ic-1]->max_attack_range());
	}
		
	if(widthRight > widthLeft)
		trueWidth = widthRight;
	else
		trueWidth = widthLeft;

	if(lengthRight > lengthLeft)
		trueLength = lengthRight;
	else
		trueLength = lengthLeft;

	int diff; 
	bool check = false;
	int locX2, locY2;
	int tempOldX1, tempOldX2, tempOldY1, tempOldY2;
	
	if(!direction || direction == LEADER_AT_TOP)
	{	
		locX = centerX;
		if(!nRange)
			locY = centerY-2*maximumSizeY;
		else if(nRange < 5)
			locY = centerY-3*maximumSizeY;
		else if(nRange == 5)
			locY = centerY-4*maximumSizeY;
		else
			err_here();

		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locX-lengthLeft) < 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locY+trueWidth+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if(locY < 0)
		{
			centerY -= locY;
			locY = 0;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;

		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY + closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX;
			tempOldY2 = locY + closeAttackUnits[0]->obj_loc_height();
		}
		//--check whether the locations for forming formation are occupied--//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 += (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2=tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY2;
				tempOldY2 += (closeAttackUnits[ic]->obj_loc_height());
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		if(nRange)
		{
			locX2 = locX;
			locY2 = locY+2*maximumSizeY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2;
			tempOldY2 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			ir = 1;

			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2=tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY2;
					tempOldY2 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
			}
		}
		//--if the locations are not occupied-----//
		if (check || direction == LEADER_AT_TOP) 
		{
			finalDirection = LEADER_AT_TOP;
		} 
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		if(!nRange)
			locY = centerY+2*maximumSizeY;
		else if(nRange < 5)
			locY = centerY+3*maximumSizeY;
		else if(nRange == 5)
			locY = centerY+4*maximumSizeY;
		else
			err_here();

		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locX-lengthLeft) < 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locY+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if((diff = locY-trueWidth) < 0)
		{
			locY-=diff;
			centerY -= diff;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY;
			tempOldX2 = locX;
			tempOldY2 = locY;
		}

		//----check whether the locations for forming formation are occupied----//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY1 = tempOldY1-closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;					
			ic++;
		}
	
		if(nRange)
		{
			locX2 = locX;
			locY2 = locY-2*maximumSizeY;
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2;
			tempOldX2 = locX2;
			tempOldY2 = locY2;
			ir = 1;

			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY1 = tempOldY1-rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2 = tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;					
				ir++;
			}
		}
		//---if these locations are not occupied----//
		if (check || direction==LEADER_AT_BOTTOM) 
		{
			finalDirection = LEADER_AT_BOTTOM;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		locY = centerY;
		if(!nRange)
			locX = centerX + 2*maximumSizeX;
		else if(nRange < 5)
			locX = centerX + 3*maximumSizeX;
		else if(nRange == 5)
			locX = centerX + 4*maximumSizeX;
		else
			err_here();

		if((diff=locX+1-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locX-trueLength) < 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if((diff=locY-widthLeft) < 0)
		{
			locY-=diff;
			centerY -= diff;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX;
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX;
			tempOldY2 = locY;
		}

		//check whether the locations for forming formation are occupied//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = (tempOldX1 = tempOldX1-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 +=  (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			++ic;
		}	

		if(nRange)
		{
			locX2 = locX - 2*maximumSizeX;
			locY2 = locY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			ir = 1;
			tempOldX1 = locX2;
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2;
			tempOldY2 = locY2;

			//check whether the locations for forming formation are occupied//
			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = (tempOldX1 = tempOldX1-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 +=  (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2 = tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				++ir;
			}	
		}
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_RIGHT) 
		{
			finalDirection = LEADER_AT_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		locY = centerY;
		
		if(!nRange)
			locX = centerX-2*maximumSizeX;
		else if(nRange < 5)
			locX = centerX-3*maximumSizeX;
		else if(nRange == 5)
			locX = centerX-4*maximumSizeX;
		else
			err_here();

		if((diff=locX+trueLength+1-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if(locX < 0)
		{
			centerX -= locX;
			locX=0;
		}
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if((diff=locY-widthLeft)<0)
		{
			locY -= diff;
			centerY -= diff;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY + closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY2 = locY;
		}
		//---check whether the locations for forming formation are occupied---//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 += (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = tempOldX2;
				tempOldX2 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;					
			++ic;
		}

		if(nRange)
		{
			locX2 = locX + 2*maximumSizeX;
			locY2 = locY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY2 = locY2;
			ir = 1;
			
			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = tempOldX2;
					tempOldX2 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;					
				++ir;
			}
		}
		//---if these locations are occupied----//
		if (check || direction == LEADER_AT_LEFT) 
		{
			finalDirection = LEADER_AT_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		if(!nRange)
		{
			locX = centerX - 2*maximumSizeX;
			locY = centerY - 2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX - 3*maximumSizeX;
			locY = centerY - 3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX - 4*maximumSizeX;
			locY = centerY - 4*maximumSizeY;
		}
		else
			err_here();
 
		if((diff=locX+lengthLeft+1-MAX_WORLD_X_LOC) > 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if(locX < 0)
		{
			centerX -= locX;
			locX = 0;
		}
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if(locY < 0)
		{
			centerY -= locY;
			locY = 0;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX+closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
		}
		
		//check whether the locations for forming formation are occupied//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
				temperoryY = locY;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		if(nRange)
		{
			ir = 1;
			locX2 = locX + 2*maximumSizeX;
			locY2 = locY + 2*maximumSizeY;
			
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2+rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			
			//check whether the locations for forming formation are occupied//
			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = locX2;
					temperoryY = tempOldY1;
					tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
				}
				else
				{
					temperoryX = tempOldX1;
					tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
					temperoryY = locY2;
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
			}
		}
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_TOP_LEFT) 
		{
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		if(!nRange)
		{
			locX = centerX+2*maximumSizeX;
			locY = centerY-2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX+3*maximumSizeX;
			locY = centerY-3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX+4*maximumSizeX;
			locY = centerY-4*maximumSizeY;
		}
		else
			err_here();

		if((diff = locX+1-MAX_WORLD_X_LOC) >0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff = locX-lengthLeft) < 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff = locY+widthRight+1-MAX_WORLD_Y_LOC) >0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if(locY<0)
		{
			centerY -= locY;
			locY = 0;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX;
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
		}
		
		//check whether the locations for forming formation are occupied//
		while(check && ic<nClose)
		{
			if(ic%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			else
			{
				tempOldX1 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX1;
				temperoryY = locY;
			}

			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			
			ic++;
		}

		if(nRange)
		{
			ir = 1;
			locX2 = locX-2*maximumSizeX;
			locY2 = locY+2*maximumSizeY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2;
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			
			//check whether the locations for forming formation are occupied//
			while(check && ir<nRange)
			{
				if(ir%2)
				{
					temperoryX = locX2;
					temperoryY = tempOldY1;
					tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
				}
				else
				{
					tempOldX1 -= rangeAttackUnits[ir]->obj_loc_width();
					temperoryX = tempOldX1;
					temperoryY = locY2;
				}

				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				
				ir++;
			}
		}
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_TOP_RIGHT) 
		{
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		if(!nRange)
		{
			locX = centerX - 2*maximumSizeX;
			locY = centerY + 2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX - 3*maximumSizeX;
			locY = centerY + 3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX - 4*maximumSizeX;
			locY = centerY + 4*maximumSizeY;
		}
		else
			err_here();

		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC) > 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if(locX < 0)
		{
			centerX -= locX;
			locX =0;
		}
		if((diff = locY+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY -= diff;
			centerY -= diff;
		}
		if((diff = locY-widthLeft) < 0)
		{
			locY-=diff;
			centerY -= diff;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		if(nClose)
		{
			tempOldX1 = locX+closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY;
		}
		
		//check whether the locations for forming formation are occupied//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
				temperoryY = locY;
			}
			else
			{
				temperoryX = locX;
				tempOldY1 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY1;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		if(nRange)
		{
			locX2 = locX+2*maximumSizeX;
			locY2 = locY-2*maximumSizeY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			tempOldX1 = locX2+rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2;
			ir = 1;
			
			//check whether the locations for forming formation are occupied//
			while(check && ir < nRange)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
					temperoryY = locY2;
				}
				else
				{
					temperoryX = locX2;
					tempOldY1 -= rangeAttackUnits[ir]->obj_loc_height();
					temperoryY = tempOldY1;
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
			}
		}
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_BOTTOM_LEFT) 
		{
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		if(!nRange)
		{
			locX = centerX + 2*maximumSizeX;
			locY = centerY + 2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX + 3*maximumSizeX;
			locY = centerY + 3*maximumSizeY;
		}
		else if(nRange ==5)
		{
			locX = centerX + 4*maximumSizeX;
			locY = centerY + 4*maximumSizeY;
		}
		else
			err_here();

		if((diff=locX+1-MAX_WORLD_X_LOC) > 0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locX-lengthRight) <0)
		{
			locX-=diff;
			centerX -= diff;
		}
		if((diff=locY+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			centerY -= diff;
		}
		if((diff=locY-lengthLeft) < 0)
		{
			locY-=diff;
			centerY -= diff;
		}

		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;
		
		tempOldX1 = locX;
		tempOldY1 = locY;
		
		//check whether the locations for forming formation are occupied//
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				tempOldX1 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX1;
				temperoryY = locY;
			}
			else
			{
				temperoryX = locX;
				tempOldY1 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY1;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}					

		if(nRange)
		{
			locX2 = locX-2*maximumSizeX;
			locY2 = locY-2*maximumSizeY;

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			ir = 1;
			tempOldX1 = locX2;
			tempOldY1 = locY2;
			
			while(check && ir < nRange)
			{
				if(ir%2)
				{
					tempOldX1 -= rangeAttackUnits[ir]->obj_loc_width();
					temperoryX = tempOldX1;
					temperoryY = locY2;
				}
				else
				{
					temperoryX = locX2;
					tempOldY1 -= rangeAttackUnits[ir]->obj_loc_height();
					temperoryY = tempOldY1;
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
			}					
		}
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_BOTTOM_RIGHT) 
		{
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}
	//if all the location are occupied-//
	if (!check && !direction)
	{
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			for (i = 0, tempLocX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, tempLocX++)				
				for (j = 0, tempLocY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, tempLocY++)					
					world.get_loc(tempLocX, tempLocY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		if(rangeAttackUnits)
			mem_del(rangeAttackUnits);
		if(closeAttackUnits)
			mem_del(closeAttackUnits);
		return;
	}

	formation_mode = FORMATION_VANGUARD;

	if(!general->team_info->formation_direction)
		general->team_info->formation_direction = finalDirection;

	for(t=1; t<=nSelected; t++)
	{
		unitPtr = get_unit(t);
		unitPtr->set_in_formation();
	
		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(unitPtr->sprite_recno, mobile_type);				
	}
	if(finalDirection == LEADER_AT_TOP)
	{
		if(generalIsHere)
			general->move(centerX, centerY);

		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY + closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX;
			tempOldY2 = locY + closeAttackUnits[0]->obj_loc_height();
		}
		
		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 += (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2=tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY2;
				tempOldY2 += (closeAttackUnits[ic]->obj_loc_height());
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}

		if(nRange)
		{
			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2;
			tempOldY2 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2=tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY2;
					tempOldY2 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM)
	{
		if(generalIsHere )
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY;
			tempOldX2 = locX;
			tempOldY2 = locY;
		}

		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY1 = tempOldY1-closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}
		
		if(nRange)
		{
			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2;
			tempOldX2 = locX2;
			tempOldY2 = locY2;
			
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY1 = tempOldY1-rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2 = tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else if(finalDirection == LEADER_AT_RIGHT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX;
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX;
			tempOldY2 = locY;
		}

		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				temperoryX = (tempOldX1 = tempOldX1-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 +=  (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}	

		if(nRange)
		{
			tempOldX1 = locX2;
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2;
			tempOldY2 = locY2;

			//check whether the locations for forming formation are occupied//
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = (tempOldX1 = tempOldX1-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 +=  (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = (tempOldX2 = tempOldX2-rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);	
			}
		}
	}
	else if(finalDirection == LEADER_AT_LEFT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY + closeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX + closeAttackUnits[0]->obj_loc_width();
			tempOldY2 = locY;
		}
		//---check whether the locations for forming formation are occupied---//
		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = tempOldY1;
				tempOldY1 += (closeAttackUnits[ic]->obj_loc_height());
			}
			else
			{
				temperoryX = tempOldX2;
				tempOldX2 += (closeAttackUnits[ic]->obj_loc_width());
				temperoryY = (tempOldY2 = tempOldY2-closeAttackUnits[ic]->obj_loc_height());
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}
	
		if(nRange)
		{
			tempOldX1 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2 + rangeAttackUnits[0]->obj_loc_height();
			tempOldX2 = locX2 + rangeAttackUnits[0]->obj_loc_width();
			tempOldY2 = locY2;
			
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = tempOldY1;
					tempOldY1 += (rangeAttackUnits[ir]->obj_loc_height());
				}
				else
				{
					temperoryX = tempOldX2;
					tempOldX2 += (rangeAttackUnits[ir]->obj_loc_width());
					temperoryY = (tempOldY2 = tempOldY2-rangeAttackUnits[ir]->obj_loc_height());
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else if(finalDirection == LEADER_AT_TOP_LEFT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX+closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
		}
		
		//check whether the locations for forming formation are occupied//
		for(ic=1; ic<nClose;ic++)
		{
			if(ic%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
				temperoryY = locY;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}
		
		if(nRange)
		{
			tempOldX1 = locX2+rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			
			//check whether the locations for forming formation are occupied//
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = locX2;
					temperoryY = tempOldY1;
					tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
				}
				else
				{
					temperoryX = tempOldX1;
					tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
					temperoryY = locY2;
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else if(finalDirection == LEADER_AT_TOP_RIGHT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);

		if(nClose)
		{
			tempOldX1 = locX;
			tempOldY1 = locY+closeAttackUnits[0]->obj_loc_height();
		}
		
		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			else
			{
				tempOldX1 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX1;
				temperoryY = locY;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}
		
		if(nRange)
		{
			tempOldX1 = locX2;
			tempOldY1 = locY2+rangeAttackUnits[0]->obj_loc_height();
			
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = locX2;
					temperoryY = tempOldY1;
					tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
				}
				else
				{
					tempOldX1 -= rangeAttackUnits[ir]->obj_loc_width();
					temperoryX = tempOldX1;
					temperoryY = locY2;
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);			
			}
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM_LEFT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);
		
		if(nClose)
		{
			tempOldX1 = locX+closeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY;
		
			//check whether the locations for forming formation are occupied//
			for(ic=1; ic<nClose; ic++)
			{
				if(ic%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
					temperoryY = locY;
				}
				else
				{
					temperoryX = locX;
					tempOldY1 -= closeAttackUnits[ic]->obj_loc_height();
					temperoryY = tempOldY1;
				}
				closeAttackUnits[ic]->move(temperoryX, temperoryY);
			}
		}
		
		if(nRange)
		{
			tempOldX1 = locX2+rangeAttackUnits[0]->obj_loc_width();
			tempOldY1 = locY2;
				
			//check whether the locations for forming formation are occupied//
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					temperoryX = tempOldX1;
					tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
					temperoryY = locY2;
				}
				else
				{
					temperoryX = locX2;
					tempOldY1 -= rangeAttackUnits[ir]->obj_loc_height();
					temperoryY = tempOldY1;
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM_RIGHT)
	{
		if(generalIsHere)
			general->move(centerX, centerY);
		if(nClose)
			closeAttackUnits[0]->move(locX, locY);
		if(nRange)
			rangeAttackUnits[0]->move(locX2, locY2);
		
		tempOldX1 = locX;
		tempOldY1 = locY;
		
		//check whether the locations for forming formation are occupied//
		for(ic=1; ic<nClose; ic++)
		{
			if(ic%2)
			{
				tempOldX1 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX1;
				temperoryY = locY;
			}
			else
			{
				temperoryX = locX;
				tempOldY1 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY1;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}					
	
		if(nRange)
		{
			tempOldX1 = locX2;
			tempOldY1 = locY2;
			
			for(ir=1; ir<nRange; ir++)
			{
				if(ir%2)
				{
					tempOldX1 -= rangeAttackUnits[ir]->obj_loc_width();
					temperoryX = tempOldX1;
					temperoryY = locY2;
				}
				else
				{
					temperoryX = locX2;
					tempOldY1 -= rangeAttackUnits[ir]->obj_loc_height();
					temperoryY = tempOldY1;
				}
				rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			}
		}
	}
	else
		err_here();

	if(closeAttackUnits)
		mem_del(closeAttackUnits);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
}
//--- End of function UnitGroup::vanguard_formation ---//

//--- Begin of function UnitGroup::line_formation ---//
// This function calculate the locations and set the unit to move to form a line formation
// if longline is true, long line formation will be form.
// if longline is false, short line formation will be form.
void UnitGroup::line_formation(int direction)
{
	//--------------Define local variables----------------//
	int nSelected = 0;
	int locX, locY, i, j, t,temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected=0;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int* sizeArrayX=NULL, *sizeArrayY=NULL;
	Unit* unitPtr;
	int finalDirection=0;
	//-----------------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
	sizeArrayX = (int*)mem_add(nSelected*sizeof(int));
	sizeArrayY = (int*)mem_add(nSelected*sizeof(int));
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
	Unit* general = NULL;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		sortedArray[t-1] = unitPtr;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}

	if(!general)
		general = unit_array[get_unit(1)->leader_unit_recno];

	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;
	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<nSelected; i++)
	{
		for(j=0; j<i; j++)
		{
			if(sortedArray[j]->max_attack_range() > sortedArray[i]->max_attack_range())
			{
				tempUnit = sortedArray[i];
				sortedArray[i] = sortedArray[j];
				sortedArray[j] = tempUnit;
			}
		}
	}
	
	int lengthRight = 0;
	int lengthLeft = 0;
	int widthRight = 0;
	int widthLeft = 0;
	int trueWidth, trueLength;

	for(i=0; i<nSelected; i++)
	{
		sizeArrayX[i] = sortedArray[i]->obj_loc_width();
		sizeArrayY[i] = sortedArray[i]->obj_loc_height();
		if(i==0 || i%2)
		{
			lengthRight += sizeArrayX[i];
			widthRight += sizeArrayY[i];
		}
		else
		{
			lengthLeft += sizeArrayX[i];
			widthLeft += sizeArrayY[i];
		}
		err_when(i>0 && sortedArray[i]->max_attack_range() < sortedArray[i-1]->max_attack_range());
	}

	if(widthRight > widthLeft)
		trueWidth = widthRight;
	else
		trueWidth = widthLeft;

	if(lengthRight > lengthLeft)
		trueLength = lengthRight;
	else
		trueLength = lengthLeft;

	int diff;
		
	bool check = false;
	bool leaderAtMiddleWithSpanAlongXOK=false;
	bool leaderAtMiddleWithSpanAlongYOK=false;
	bool leaderAtMiddleWithSpanAlongPositiveXAndPositiveY = false;
	bool leaderAtMiddleWithSpanAlongPositiveXAndNegativeY = false;
	int theSecondTemperoryX, theSecondTemperoryY;
	int tempOldLocX1, tempOldLocY2, tempOldLocX2, tempOldLocY1;
	
	if(!direction || direction == LEADER_AT_LEFT || direction == LEADER_AT_RIGHT)
	{
		locX = centerX;
		locY = centerY;
		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft-1) < 0)
			locX-=diff;
		if((diff=locY+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if(locY < 0)
			locY = 0;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;					

		tempOldLocX1 = locX+sizeArrayX[0];
		tempOldLocX2 = locX;
		
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldLocX1;
				tempOldLocX1 += sizeArrayX[tSelected];
			}
			else
				temperoryX = (tempOldLocX2 = tempOldLocX2-sizeArrayX[tSelected]);

			temperoryY = locY;
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;
			
			tSelected++;
		}
		if(check || direction == LEADER_AT_LEFT || direction == LEADER_AT_RIGHT)
		{
			leaderAtMiddleWithSpanAlongXOK = true;
			finalDirection = LEADER_AT_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY;
		if((diff=locX+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if(locX < 0)
			locX=0;
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft-1) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;					
		
		tempOldLocY1 = locY+sizeArrayY[0];
		tempOldLocY2 = locY;
		while(check && tSelected < nSelected)
		{
			temperoryX = locX;
			if(tSelected%2)
			{
				temperoryY = tempOldLocY1;
				tempOldLocY1+=sizeArrayY[tSelected];
			}
			else
				temperoryY = (tempOldLocY2 = tempOldLocY2-sizeArrayY[tSelected]);
			
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;
	
			tSelected++;
		}
		if(check || direction == LEADER_AT_TOP || direction == LEADER_AT_BOTTOM)
		{
			leaderAtMiddleWithSpanAlongYOK = true;
			finalDirection = LEADER_AT_TOP;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_TOP_LEFT || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX;
		locY = centerY;
		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft-1) < 0)
			locX-=diff;
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft-1) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;					

		tempOldLocX1 = locX+sizeArrayX[0];
		tempOldLocY1 = locY+sizeArrayY[0];
		tempOldLocX2 = locX;
		tempOldLocY2 = locY;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldLocX1;
				tempOldLocX1 += sizeArrayX[tSelected];
				temperoryY = tempOldLocY1;
				tempOldLocY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldLocX2 -= sizeArrayX[tSelected];
				temperoryX = tempOldLocX2;
				tempOldLocY2 -= sizeArrayY[tSelected];
				temperoryY = tempOldLocY2;
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;
			tSelected++;
		}
		//if these locations are not occupied//
		if(check || direction == LEADER_AT_TOP_LEFT || direction == LEADER_AT_BOTTOM_RIGHT) 
		{
			leaderAtMiddleWithSpanAlongPositiveXAndPositiveY = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_TOP_RIGHT || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX;
		locY = centerY+1;
		if((diff=locX+lengthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft-1) < 0)
			locX-=diff;
		if((diff=locY+widthRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft-1) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;					

		tempOldLocX1 = locX+sizeArrayX[0];
		tempOldLocY1 = locY+sizeArrayY[0];
		tempOldLocX2 = locX;
		tempOldLocY2 = locY;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldLocX1;
				tempOldLocX1 += sizeArrayX[tSelected];
				tempOldLocY1 -= sizeArrayY[tSelected];
				temperoryY = tempOldLocY1;
			}
			else
			{
				tempOldLocX2 -= sizeArrayX[tSelected];
				temperoryX = tempOldLocX2;
				temperoryY = tempOldLocY2;
				tempOldLocX2 += sizeArrayY[tSelected];
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;
			tSelected++;
		}
		//if these locations are not occupied//
		if(check || direction == LEADER_AT_TOP_RIGHT || direction == LEADER_AT_BOTTOM_LEFT)
		{
			leaderAtMiddleWithSpanAlongPositiveXAndNegativeY = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}

	//if all the formations are not allowed//			
	if(!check && !direction)
	{  
		if(sizeArrayX)
			mem_del(sizeArrayX);
		if(sizeArrayY)
			mem_del(sizeArrayY);
		if(sortedArray)
			mem_del(sortedArray);
		
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
				for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
					world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		return;
	}

	formation_mode = FORMATION_LINE;

	tSelected = 0;

	int oldLocX1 = locX+sizeArrayX[0];
	int oldLocY1 = locY+sizeArrayX[0];
	int oldLocX2 = locX+sizeArrayX[0];
	int oldLocY2 = locY+sizeArrayY[0];
	
	if(leaderAtMiddleWithSpanAlongPositiveXAndNegativeY)
	{
		oldLocY2 = oldLocY1 = locY;
	}
	
	//set the units to move to the location for formation//
	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		for (i = 0, temperoryLocationForInDirectionX = sortedArray[tSelected]->next_x_loc(); i < sizeArrayX[tSelected]; i++, temperoryLocationForInDirectionX++)				
			for (j = 0, temperoryLocationForInDirectionY = sortedArray[tSelected]->next_y_loc(); j < sizeArrayY[tSelected]; j++, temperoryLocationForInDirectionY++)					
				world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(leaderAtMiddleWithSpanAlongXOK)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(oldLocX1, locY);
				oldLocX1 += sizeArrayX[tSelected];
			}
			else
				sortedArray[tSelected]->move((oldLocX2 = oldLocX2-sizeArrayX[tSelected]), locY);
		}
		else if(leaderAtMiddleWithSpanAlongYOK)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(locX, oldLocY1);
				oldLocY1 += sizeArrayY[tSelected];
			}
			else
				sortedArray[tSelected]->move(locX, (oldLocY2 = oldLocY2-sizeArrayY[tSelected]));
		}
		else if(leaderAtMiddleWithSpanAlongPositiveXAndPositiveY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(oldLocX1, oldLocY1);
				oldLocX1 += sizeArrayX[tSelected];
				oldLocY1 += sizeArrayY[tSelected];
			}
			else
				sortedArray[tSelected]->move((oldLocX2 = oldLocX2-sizeArrayX[tSelected]), oldLocY2 = oldLocY2-sizeArrayY[tSelected]);
		}
		else if(leaderAtMiddleWithSpanAlongPositiveXAndNegativeY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(oldLocX1, (oldLocY1=oldLocY1-sizeArrayY[tSelected]));
				oldLocX1 += sizeArrayX[tSelected];
			}
			else
			{
				sortedArray[tSelected]->move((oldLocX2 = oldLocX2-sizeArrayX[tSelected]), oldLocY2);
				oldLocY2 += sizeArrayY[tSelected];
			}
		}
		else
		{
			err_here();
		}
		
		//set the locations originally occupied by the units to be true
		sortedArray[tSelected]->set_in_formation();
	}	
			
	if(sizeArrayX)
		mem_del(sizeArrayX);
	if(sizeArrayY)
		mem_del(sizeArrayY);
	if(sortedArray)
		mem_del(sortedArray);
}	
//--- End of function UnitGroup::line_formation ---//

//--- Begin of function UnitGroup::circle_formation ---//
//This function calculates the location and set the unit to move to form a circle formation
void UnitGroup::circle_formation(int direction)
{
	//--------------Define local variables------------//
	int radius, centerX, centerY;
	int nSelected = 0; 
	int t, tSelected =0;
	int temperoryX, temperoryY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	float* sineArray=NULL, *cosineArray=NULL;
	int ind;
	float angle;
	Unit* unitPtr;
	//---------------------------------------------------//

	direction = 0;

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
	
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	int nRange=0, nClose=0; //number of units with range attack and close attack respectively
	Unit* general=NULL;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		else if(unitPtr->max_attack_range() > 1)
			nRange++;
		else
			nClose++;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
	}		

	centerX = (maxLocX+minLocX)/2;
	centerY = (maxLocY+minLocY)/2;
	//set unit of range attack to the middle of the circle, but the number cannot exceed 4, otherwise the formation will be very ugly
	if(nRange > 4)
		nRange = 4;

	bool generalIsHere = true;

	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}

	centerX = general->next_x_loc();
	centerY = general->next_y_loc();

	int ir = 0;
	t = 1;
	int ic = 0;
	int nOther = nSelected-nRange-(int)generalIsHere;
	Unit** rangeAttackUnits = (Unit**) mem_add_clear(nRange*sizeof(Unit*));
	Unit** unitsArray = (Unit**) mem_add_clear(nOther*sizeof(Unit*));

	while(t<=nSelected)
	{
		unitPtr = get_unit(t++);
		if(!unitPtr->team_info && unitPtr->max_attack_range() > 1 && ir<nRange)
			rangeAttackUnits[ir++] = unitPtr;
		else if(!unitPtr->team_info)
			unitsArray[ic++]  = unitPtr;
		err_when(ic>nOther);
	}

	int diff;
		
	//Define the radius
	if(nOther<4)
		radius = maximumSizeY/2+4;
	else
		radius = ((nOther-1)/5+2)*maximumSizeY; // this should be a jolly good radius

	//write the sine and cosine values into an array//
	angle = 6.2832f/((float)nOther);  // the magic number is 2 pi
	sineArray = (float*) mem_add(nOther*sizeof(float));
	cosineArray = (float*) mem_add(nOther*sizeof(float));
	for(ind = 0; ind<nOther; ind++)
	{
		sineArray[ind] = (float) (radius*sin(ind*angle));
		cosineArray[ind] = (float) (radius*cos(ind*angle));
	}

	int generalSizeX = general->obj_loc_width();
	int generalSizeY = general->obj_loc_height();

	///checking out of boundary case
	if((diff=centerX+radius+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
			centerX-=diff;
	if((diff = centerX-radius) < 0)
			centerX = radius;
	if((diff=centerY+radius+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			centerY-=diff;
	if((diff=centerY-radius) < 0)
			centerY = radius;
		
	formation_mode = FORMATION_CIRCLE;
	if(generalIsHere)
	{
		general->move(centerX, centerY);
		general->set_in_formation();
	}
	
	for(ir= 0; ir<nRange; ir++)
	{
		if(ir==0)
			rangeAttackUnits[ir]->move(centerX-rangeAttackUnits[ir]->obj_loc_width(), centerY-rangeAttackUnits[ir]->obj_loc_height());
		if(ir==1)
			rangeAttackUnits[ir]->move(centerX+generalSizeX, centerY-rangeAttackUnits[ir]->obj_loc_height());
		if(ir==2)
			rangeAttackUnits[ir]->move(centerX-rangeAttackUnits[ir]->obj_loc_width(), centerY+generalSizeY);
		if(ir==3)
			rangeAttackUnits[ir]->move(centerX+generalSizeX, centerY+generalSizeY);
		
		rangeAttackUnits[ir]->set_in_formation();
	}

	for(tSelected = 0; tSelected<nOther; tSelected++)
	{
		//set the unit to move to the location for forming formation//
		unitsArray[tSelected]->move((int)(centerX-cosineArray[tSelected]), 
				(int)(centerY+sineArray[tSelected]));

		unitsArray[tSelected]->set_in_formation();
	}
			
	if(sineArray)
		mem_del(sineArray);
	if(cosineArray)
		mem_del(cosineArray);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
	if(unitsArray)
		mem_del(unitsArray);
}
//--- End of function UnitGroup::circle_formation ---//

//--- Begin of function UnitGroup::arc_formation ---//
void UnitGroup::arc_formation(int direction)
{
	//--------------Define local variables------------//
	int centerX, centerY, circleCenterX, circleCenterY;
	int nSelected = 0; 
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int ind;
	float angleC, angleR;
	int radiusC, radiusR;
	bool xDirection, yDirection;
	int xMultiplicativeFactor, yMultiplicativeFactor;
	Unit* unitPtr;
	int finalDirection = 0;
//---------------------------------------------------//
	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
	
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int nRange=0, nClose=0;
	Unit* general=NULL;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		else if(unitPtr->max_attack_range() > 1)
			nRange++;
		else
			nClose++;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}		

	bool generalIsHere = true;

	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}

	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;
			
	while(nRange > nClose+4)
	{
		nRange-= 2;
		nClose+= 2;
	}
	if(nRange > 5)
	{
		nRange = 5;
		nClose = nSelected-nRange-1;
	}

	centerX = general->next_x_loc();
	centerY = general->next_y_loc();

	int ir = 0;
	int ic = 0;
	t = 1;
	Unit** rangeAttackUnits = (Unit**) mem_add_clear(nRange*sizeof(Unit*));
	Unit** closeAttackUnits = (Unit**) mem_add_clear(nClose*sizeof(Unit*));

	while(t<=nSelected)
	{
		unitPtr = get_unit(t++);
		if(!unitPtr->team_info && unitPtr->max_attack_range() > 1 && ir<nRange)
			rangeAttackUnits[ir++] = unitPtr;
		else if(!unitPtr->team_info)
			closeAttackUnits[ic++]  = unitPtr;
		err_when(ic>nClose);
	}
	err_when(ic != nClose);

	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<nClose; i++)
	{
		for(j=0; j<i; j++)
		{
			if(closeAttackUnits[j]->max_attack_range() > closeAttackUnits[i]->max_attack_range())
			{
				tempUnit = closeAttackUnits[i];
				closeAttackUnits[i] = closeAttackUnits[j];
				closeAttackUnits[j] = tempUnit;
			}
		}
	}
	
	//Define the radii
	if(nClose<4)
		radiusC = 3*(maximumSizeY/2+1);
	else if(nClose < 8)
		radiusC = ((nClose-1)/2+2)*maximumSizeY;
	else
		radiusC = ((nClose-1)/2+4)*maximumSizeY; 
	
	if(nRange<4)
		radiusR = 3*(maximumSizeY/2+1);
	else if(nRange <=5)
		radiusR = ((nRange)/2+2)*maximumSizeY;
	else
		err_here();

	int arraySizeC = nClose/2+1;
	int arraySizeR = nRange/2+1;
	//write the sine and cosine values into an array//
	angleC = 2.5f/((float)nClose);  //2.5 means (2 *pi*2)/5
	angleR = 2.5f/((float)nRange);
	const float piOverFour = (float)(3.14/4.);

	float* sineArrayC=NULL, *cosineArrayC=NULL;
	float* sineArrayR=NULL, *cosineArrayR=NULL;
	float* sineAdd45ArrayC=NULL, *cosineAdd45ArrayC=NULL;
	float* sineSub45ArrayC=NULL, *cosineSub45ArrayC=NULL;
	float* sineAdd45ArrayR=NULL, *cosineAdd45ArrayR=NULL;
	float* sineSub45ArrayR=NULL, *cosineSub45ArrayR=NULL;

	if(!direction || direction == LEADER_AT_TOP || direction == LEADER_AT_BOTTOM || 
		direction == LEADER_AT_RIGHT || direction == LEADER_AT_LEFT)
	{
		sineArrayC = (float*) mem_add_clear(arraySizeC*sizeof(float));
		cosineArrayC = (float*) mem_add_clear(arraySizeC*sizeof(float));
		sineArrayR = (float*) mem_add_clear(arraySizeR*sizeof(float));
		cosineArrayR = (float*) mem_add_clear(arraySizeR*sizeof(float));
		for(ind = 0; ind<arraySizeC; ind++)
		{
			sineArrayC[ind] = (float)(radiusC*sin(ind*angleC));
			cosineArrayC[ind] = (float) (radiusC*cos(ind*angleC));
		}
		for(ind = 0; ind<arraySizeR; ind++)
		{
			sineArrayR[ind] = (float)(radiusR*sin(ind*angleR));
			cosineArrayR[ind] = (float) (radiusR*cos(ind*angleR));
		}
	}
	
	if(!direction || direction == LEADER_AT_TOP_LEFT || direction == LEADER_AT_TOP_RIGHT ||
		direction == LEADER_AT_BOTTOM_LEFT || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		sineAdd45ArrayC = (float*)mem_add_clear(arraySizeC*sizeof(float));
		cosineAdd45ArrayC = (float*)mem_add_clear(arraySizeC*sizeof(float));
		sineSub45ArrayC = (float*)mem_add_clear(arraySizeC*sizeof(float));
		cosineSub45ArrayC = (float*)mem_add_clear(arraySizeC*sizeof(float));
		sineAdd45ArrayR = (float*)mem_add_clear(arraySizeR*sizeof(float));
		cosineAdd45ArrayR = (float*)mem_add_clear(arraySizeR*sizeof(float));
		sineSub45ArrayR = (float*)mem_add_clear(arraySizeR*sizeof(float));
		cosineSub45ArrayR = (float*)mem_add_clear(arraySizeR*sizeof(float));
		for(ind = 0; ind<arraySizeC; ind++)
		{
			sineAdd45ArrayC[ind] = (float) (radiusC*sin(ind*angleC+piOverFour));
			cosineAdd45ArrayC[ind] = (float) (radiusC*cos(ind*angleC+piOverFour));
			sineSub45ArrayC[ind] = (float) (radiusC*sin(ind*angleC-piOverFour));
			cosineSub45ArrayC[ind] = (float) (radiusC*cos(ind*angleC-piOverFour));
		}
		for(ind = 0; ind<arraySizeR; ind++)
		{
			sineAdd45ArrayR[ind] = (float) (radiusR*sin(ind*angleR+piOverFour));
			cosineAdd45ArrayR[ind] = (float) (radiusR*cos(ind*angleR+piOverFour));
			sineSub45ArrayR[ind] = (float) (radiusR*sin(ind*angleR-piOverFour));
			cosineSub45ArrayR[ind] = (float) (radiusR*cos(ind*angleR-piOverFour));
		}
	}
	//check orientation
	int arrayIndex;
	int theSign;
	bool check = false;
	bool slantPosYPosX = false;
	bool slantPosYNegX = false;
	bool slantNegYPosX = false;
	bool slantNegYNegX = false;

	xDirection = false;
	yDirection = false;
	int locX2, locY2;
		
	int diff; 

	if(!direction || direction == LEADER_AT_BOTTOM) 
	{
		//circleCenterX = centerX;
		//circleCenterY = (int)((float)centerY-((float)radius)*0.5f-sinPiOverTenTimesRadius*0.5f)+1;
		
		locX = centerX;
		circleCenterX = locX;
		//locY = circleCenterY+radius;
		if(!nRange)
			locY = centerY+4*maximumSizeY;
		else if(nRange < 5)
			locY = centerY+5*maximumSizeY;
		else if(nRange == 5)
			locY = centerY+6*maximumSizeY;

		circleCenterY = locY - radiusC;

		if((diff=(int)(circleCenterX+radiusC+maximumSizeX+1-MAX_WORLD_X_LOC))>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
			centerX -= diff;
		}
		if((diff=(int)(circleCenterX-radiusC)) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
			centerX -= diff;
		}
		if((diff=locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			circleCenterY=locY-radiusC;
			centerY -= diff;
		}
		if((diff=(int)(circleCenterY+radiusC)) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+radiusC;
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;
		
		theSign = 1;
		arrayIndex = 1;
		//check whether the locations for forming formation are occupied
		while(check && ic<nClose)
		{
			temperoryX = (int)(circleCenterX+theSign*sineArrayC[arrayIndex]);
			temperoryY = (int)(circleCenterY+cosineArrayC[arrayIndex]);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX;
			locY2 = circleCenterY+radiusR;
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			//check whether the locations for forming formation are occupied
			while(check && ir<nRange)
			{
				temperoryX = (int)(circleCenterX+theSign*sineArrayR[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineArrayR[arrayIndex]);
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		} 
		//if these location are not occpied//
		if(check || direction == LEADER_AT_BOTTOM)
		{
			yDirection = true;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}
	if((!check && !direction) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
//		circleCenterX = (int)((float)centerX-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
//		circleCenterY = (int)((float)centerY-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		
//		locX = circleCenterX+(int)sineAdd45Array[0];		
//		locY = circleCenterY+(int)cosineAdd45Array[0];
		if(!nRange)
		{
			locX = centerX+2*maximumSizeX;
			locY = centerY+2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX+3*maximumSizeX;
			locY = centerY+3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX+4*maximumSizeX;
			locY = centerY+4*maximumSizeY;
		}
		else
			err_here();

		circleCenterX = locX-(int)sineAdd45ArrayC[0];
		circleCenterY = locY-(int)cosineAdd45ArrayC[0];

		if((diff=circleCenterX+radiusC+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterX-(int)sineSub45ArrayC[nClose/2]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterY+radiusC+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineAdd45ArrayC[0];
			centerY -= diff;
		}
		if((diff=circleCenterY+(int)cosineAdd45ArrayC[nClose/2]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineAdd45ArrayC[0];
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign = 1;
		arrayIndex = 1;
		
		while(check && ic < nClose)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineAdd45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineAdd45ArrayC[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineSub45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineSub45ArrayC[arrayIndex]);
			}
			if(closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX+(int)sineAdd45ArrayR[0];
			locY2 = circleCenterY+(int)cosineAdd45ArrayR[0];

			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			
			ir = 1;
			theSign = 1;
			arrayIndex = 1;
			
			while(check && ir < nRange)
			{
				if(theSign > 0)
				{
					temperoryX = (int)(circleCenterX+sineAdd45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY+cosineAdd45ArrayR[arrayIndex]);
				}
				else
				{
					temperoryX = (int)(circleCenterX-sineSub45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY+cosineSub45ArrayR[arrayIndex]);
				}
				if(rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}
		if(check || direction == LEADER_AT_BOTTOM_RIGHT)
		{
			slantPosYPosX = true;
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_BOTTOM_LEFT)
	{
//		circleCenterX = (int)((float)centerX+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
//		circleCenterY = (int)((float)centerY-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		if(!nRange)
		{
			locX = centerX-2*maximumSizeX;
			locY = centerY+2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX-3*maximumSizeX;
			locY = centerY+3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX-4*maximumSizeX;
			locY = centerY+4*maximumSizeY;
		}
		else
			err_here();

		circleCenterX = locX - (int)sineSub45ArrayC[0];		
		circleCenterY = locY - (int)cosineSub45ArrayC[0];

		if((diff=circleCenterX+(int)sineSub45ArrayC[nClose/2]+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterX-radiusC) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterY+radiusC+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineSub45ArrayC[0];
			centerY -= diff;
		}
		if((diff=circleCenterY+(int)cosineAdd45ArrayC[nClose/2]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineSub45ArrayC[0];
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign = 1;
		arrayIndex = 1;

		while(check && ic< nClose)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineSub45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineSub45ArrayC[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineAdd45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineAdd45ArrayC[arrayIndex]);
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX-(int)sineSub45ArrayR[0];
			locY2 = circleCenterY+(int)cosineSub45ArrayR[0];
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			ir = 1;
			theSign = 1;
			arrayIndex = 1;
			while(check && ir< nRange)
			{
				if(theSign > 0)
				{
					temperoryX = (int)(circleCenterX+sineSub45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY+cosineSub45ArrayR[arrayIndex]);
				}
				else
				{
					temperoryX = (int)(circleCenterX-sineAdd45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY+cosineAdd45ArrayR[arrayIndex]);
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}
		if(check || direction == LEADER_AT_BOTTOM_LEFT)
		{
			slantPosYNegX = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP)
	{
		circleCenterX = centerX;
		locX = centerX;
		//circleCenterY = (int)((float)centerY+((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)+1;
		if(!nRange)
			locY = centerY-4*maximumSizeY;
		else if(nRange < 5)
			locY = centerY-5*maximumSizeY;
		else if(nRange == 5)
			locY = centerY-6*maximumSizeY;
		
		circleCenterY = locY + radiusC;

		if((diff=circleCenterX+radiusC+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
			centerX -= diff;
		}
		if((diff=circleCenterX-radiusC) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
			centerX -= diff;
		}
		if((diff=circleCenterY-radiusC+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY=circleCenterY+radiusC;
			centerY -= diff;
		}
		if(locY < 0)
		{
			centerY += locY;
			locY = 0;
			circleCenterY=locY+radiusC;
		}
		
		//check forming possible
		ic = 1;
		check = true;
		
		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && ic<nClose)
		{
			temperoryX = (int)(circleCenterX+theSign*sineArrayC[arrayIndex]);
			temperoryY = (int)(circleCenterY-cosineArrayC[arrayIndex]);
			if(closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX;
			locY2 = circleCenterY-radiusR;
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			//check whether the locations for forming formation are occupied
			while(check && ir<nRange)
			{
				temperoryX = (int)(circleCenterX+theSign*sineArrayR[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineArrayR[arrayIndex]);
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}
		//if these location are not occpied//
		if(check || direction == LEADER_AT_TOP)
		{
			yDirection = true;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_TOP;
		}
	}

	if((!check && !direction) || direction == LEADER_AT_TOP_RIGHT)
	{
//		circleCenterX = (int)((float)centerX-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
//		circleCenterY = (int)((float)centerY+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
	
		if(!nRange)
		{
			locX = centerX + 2*maximumSizeX;
			locY = centerY - 2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX + 3*maximumSizeX;
			locY = centerY - 3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX + 4*maximumSizeX;
			locY = centerY - 4*maximumSizeY;
		}

		circleCenterX=locX-(int)sineAdd45ArrayC[0];		
		circleCenterY=locY+(int)cosineAdd45ArrayC[0];

		if((diff=circleCenterX+radiusC+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterX-(int)sineSub45ArrayC[nClose/2]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterY+(int)cosineSub45ArrayC[nClose/2]+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineAdd45ArrayC[0];
			centerY -= diff;
		}
		if((diff=circleCenterY-radiusC) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineAdd45ArrayC[0];
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check =false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;
	
		theSign = 1;
		arrayIndex = 1;

		while(check && ic < nClose)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineAdd45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineAdd45ArrayC[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineSub45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineSub45ArrayC[arrayIndex]);
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX+(int)sineAdd45ArrayR[0];
			locY2 = circleCenterY-(int)cosineAdd45ArrayR[0];
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			while(check && ir < nRange)
			{
				if(theSign > 0)
				{
					temperoryX = (int)(circleCenterX+sineAdd45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY-cosineAdd45ArrayR[arrayIndex]);
				}
				else
				{
					temperoryX = (int)(circleCenterX-sineSub45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY-cosineSub45ArrayR[arrayIndex]);
				}
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}

		if(check || direction == LEADER_AT_TOP_RIGHT)
		{
			slantNegYPosX = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}
	
	if(!check && (!direction || direction == LEADER_AT_TOP_LEFT))
	{
//		circleCenterX = (int)((float)centerX+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
//		circleCenterY = (int)((float)centerY+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
		if(!nRange)
		{
			locX = centerX - 2*maximumSizeX;
			locY = centerY - 2*maximumSizeY;
		}
		else if(nRange < 5)
		{
			locX = centerX - 3*maximumSizeX;
			locY = centerY - 3*maximumSizeY;
		}
		else if(nRange == 5)
		{
			locX = centerX - 4*maximumSizeX;
			locY = centerY - 4*maximumSizeY;
		}
		else
			err_here();
		
		circleCenterX = locX -(int)sineSub45ArrayC[0];		
		circleCenterY = locY +(int)cosineSub45ArrayC[0];

		if((diff=circleCenterX+(int)sineSub45ArrayC[nClose/2]+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterX-radiusC) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45ArrayC[0];
			centerX -= diff;
		}
		if((diff=circleCenterY+(int)cosineSub45ArrayC[nClose/2]+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineSub45ArrayC[0];
			centerY -= diff;
		}
		if((diff=circleCenterY-radiusC) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineSub45ArrayC[0];
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign = 1;
		arrayIndex = 1;

		while(check && ic < nClose)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineSub45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineSub45ArrayC[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineAdd45ArrayC[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineAdd45ArrayC[arrayIndex]);
			}
			
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;

			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX+(int)sineSub45ArrayR[0];
			locY2 = circleCenterY-(int)cosineSub45ArrayR[0];
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;
			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			while(check && ir < nRange)
			{
				if(theSign > 0)
				{
					temperoryX = (int)(circleCenterX+sineSub45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY-cosineSub45ArrayR[arrayIndex]);
				}
				else
				{
					temperoryX = (int)(circleCenterX-sineAdd45ArrayR[arrayIndex]);
					temperoryY = (int)(circleCenterY-cosineAdd45ArrayR[arrayIndex]);
				}
				
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;

				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}
		if(check || direction == LEADER_AT_TOP_LEFT)
		{
			slantNegYNegX = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
//		circleCenterX = (int)((float)centerX+((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)+1;
		circleCenterY = centerY;
		
//		locX = circleCenterX-radius;
		locY = circleCenterY;

		if(!nRange)
			locX = centerX-4*maximumSizeX;
		else if(nRange < 5)
			locX = centerX-5*maximumSizeX;
		else if(nRange == 5)
			locX = centerX-6*maximumSizeX;
		else
			err_here();
		
		circleCenterX = locX+radiusC;

		if((diff=circleCenterX-radiusC+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX-radiusC;
			centerX -= diff;
		}
		if(locX <0)
		{
			locX = 0;
			circleCenterX = locX+radiusC;
			centerX += locX;
		}
		if((diff=locY+radiusC+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
			centerY -= diff;
		}
		if((diff=circleCenterY-radiusC) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && ic<nClose)
		{
			temperoryX = (int)(circleCenterX-cosineArrayC[arrayIndex]);
			temperoryY = (int)(circleCenterY+theSign*sineArrayC[arrayIndex]);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX-radiusR;
			locY2 = circleCenterY;
			if(!rangeAttackUnits[0]->can_move(locX2, locY2))
				check = false;

			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			while(check && ir<nRange)
			{
				temperoryX = (int)(circleCenterX-cosineArrayR[arrayIndex]);
				temperoryY = (int)(circleCenterY+theSign*sineArrayR[arrayIndex]);
				if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}

		//if these location are not occpied//
		if(check || direction == LEADER_AT_LEFT)
		{
			xDirection = true;
			xMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_LEFT;
		}
	}
	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		//circleCenterX = (int)((float)centerX-((float)radius)*0.5f-sinPiOverTenTimesRadius*0.5f)+1;
		circleCenterY = centerY;
		
		//locX = circleCenterX+radius;
		locY = circleCenterY;
		if(!nRange)
			locX = centerX+4*maximumSizeX;
		else if(nRange < 5)
			locX = centerX+5*maximumSizeX;
		else if(nRange == 5)
			locX = centerX+6*maximumSizeX;
		else
			err_here();

		circleCenterX = locX-radiusC;

		if((diff=locX+maximumSizeX+1-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			circleCenterX = locX-radiusC;
			centerX -= diff;
		}
		if((diff=circleCenterX+radiusC) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+radiusC;
			centerX -= diff;
		}
		if((diff=locY+radiusC+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
			centerY -= diff;
		}
		if((diff=circleCenterY-radiusC) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
			centerY -= diff;
		}
		
		//check forming possible
		ic = 1;
		check = true;

		if(generalIsHere && !general->can_move(centerX, centerY))
			check = false;
		if(nClose && !closeAttackUnits[0]->can_move(locX, locY))
			check = false;

		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && ic<nClose)
		{
			temperoryX = (int)(circleCenterX+cosineArrayC[arrayIndex]);
			temperoryY = (int)(circleCenterY+theSign*sineArrayC[arrayIndex]);
			if(closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(nRange)
		{
			locX2 = circleCenterX+radiusR;
			locY2 = circleCenterY;
			if(!rangeAttackUnits[0]->can_move(temperoryX, temperoryY))
				check = false;
			theSign = 1;
			arrayIndex = 1;
			ir = 1;
			while(check && ir<nRange)
			{
				temperoryX = (int)(circleCenterX+cosineArrayR[arrayIndex]);
				temperoryY = (int)(circleCenterY+theSign*sineArrayR[arrayIndex]);
				if(rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
					check = false;
				ir++;
				if(theSign < 0)
					arrayIndex++;
				theSign = - theSign;
			}
		}
		//if these location are not occpied//
		if(check || direction == LEADER_AT_RIGHT)
		{
			xDirection = true;
			xMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_RIGHT;
		}
	}
	if(!check && !direction)
	{
		//BUG_HERE: Hey! Anyone else would like to be the leader ? 
		if(sineArrayC)
			mem_del(sineArrayC);
		if(cosineArrayC)
			mem_del(cosineArrayC);
		if(sineAdd45ArrayC)
			mem_del(sineAdd45ArrayC);
		if(cosineAdd45ArrayC)
			mem_del(cosineAdd45ArrayC);
		if(sineSub45ArrayC)
			mem_del(sineSub45ArrayC);
		if(cosineSub45ArrayC)
			mem_del(cosineSub45ArrayC);
		if(sineArrayR)
			mem_del(sineArrayR);
		if(cosineArrayR)
			mem_del(cosineArrayR);
		if(sineAdd45ArrayR)
			mem_del(sineAdd45ArrayR);
		if(cosineAdd45ArrayR)
			mem_del(cosineAdd45ArrayR);
		if(sineSub45ArrayR)
			mem_del(sineSub45ArrayR);
		if(cosineSub45ArrayR)
			mem_del(cosineSub45ArrayR);
		if(closeAttackUnits)
			mem_del(closeAttackUnits);
		if(rangeAttackUnits)
			mem_del(rangeAttackUnits);

		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
		
			for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
				for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
					world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		return;			
	}
	
	formation_mode = FORMATION_ARC;
	if(!general->team_info->formation_direction)
		general->team_info->formation_direction = finalDirection;

	for(t=1; t<=nSelected; t++)
	{
		unitPtr = get_unit(t);
		unitPtr->set_in_formation();
	
		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(unitPtr->sprite_recno, mobile_type);				
	}

	if(generalIsHere)
		general->move(centerX, centerY);

	//set the unit to move to the location for forming formation//
	if(yDirection)
	{
		arrayIndex = 0;
		theSign = -1;

		for(ic = 0; ic<nClose; ic++)
		{
			closeAttackUnits[ic]->move(((int)(circleCenterX+theSign*sineArrayC[arrayIndex])), 
				((int)(circleCenterY+yMultiplicativeFactor*cosineArrayC[arrayIndex])));
		
			if(theSign <0)
				arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;

			for(ir = 0; ir<nRange; ir++)
			{
				rangeAttackUnits[ir]->move(((int)(circleCenterX+theSign*sineArrayR[arrayIndex])), 
					((int)(circleCenterY+yMultiplicativeFactor*cosineArrayR[arrayIndex])));
			
				if(theSign <0)
					arrayIndex++;

				theSign = -theSign;
			}
		}
	}
	else if(xDirection)
	{
		arrayIndex = 0;
		theSign = -1;

		for(ic = 0; ic<nClose; ic++)
		{
			closeAttackUnits[ic]->move(((int)(circleCenterX+xMultiplicativeFactor*cosineArrayC[arrayIndex])), 
				((int)(circleCenterY+theSign*sineArrayC[arrayIndex])));

			if(theSign < 0)
				arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;
			for(ir = 0; ir<nRange; ir++)
			{
				rangeAttackUnits[ir]->move(((int)(circleCenterX+xMultiplicativeFactor*cosineArrayR[arrayIndex])), 
				((int)(circleCenterY+theSign*sineArrayR[arrayIndex])));

				if(theSign < 0)
					arrayIndex++;

				theSign = -theSign;
			}
		}
	}
	else if(slantPosYPosX)
	{
		arrayIndex = 0;
		theSign = -1;

		for(ic = 0; ic<nClose; ic++)
		{
			if(!ic)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineAdd45ArrayC[0]), (int)(circleCenterY+cosineAdd45ArrayC[0]));
			else if(theSign> 0)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineAdd45ArrayC[arrayIndex]), 
					(int)(circleCenterY+cosineAdd45ArrayC[arrayIndex]));
			else
				closeAttackUnits[ic]->move((int)(circleCenterX-sineSub45ArrayC[arrayIndex]),
					(int)(circleCenterY+cosineSub45ArrayC[arrayIndex]));
				
			if(theSign < 0)
					arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;

			for(ir = 0; ir<nRange; ir++)
			{
				if(!ir)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineAdd45ArrayR[0]), (int)(circleCenterY+cosineAdd45ArrayR[0]));
				else if(theSign> 0)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineAdd45ArrayR[arrayIndex]), 
						(int)(circleCenterY+cosineAdd45ArrayR[arrayIndex]));
				else
					rangeAttackUnits[ir]->move((int)(circleCenterX-sineSub45ArrayR[arrayIndex]),
						(int)(circleCenterY+cosineSub45ArrayR[arrayIndex]));
					
				if(theSign < 0)
					arrayIndex++;

				theSign = -theSign;
			}
		}
	}
	else if(slantPosYNegX)
	{
		arrayIndex = 0;
		theSign = -1;
		for(ic = 0; ic<nClose; ic++)
		{
			if(!ic)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineSub45ArrayC[0]), (int)(circleCenterY+cosineSub45ArrayC[0]));
			else if(theSign> 0)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineSub45ArrayC[arrayIndex]), 
					(int)(circleCenterY+cosineSub45ArrayC[arrayIndex]));
			else
				closeAttackUnits[ic]->move((int)(circleCenterX-sineAdd45ArrayC[arrayIndex]),
					(int)(circleCenterY+cosineAdd45ArrayC[arrayIndex]));
			
			if(theSign < 0)
					arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;
			for(ir = 0; ir<nRange; ir++)
			{
				if(!ir)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineSub45ArrayR[0]), (int)(circleCenterY+cosineSub45ArrayR[0]));
				else if(theSign> 0)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineSub45ArrayR[arrayIndex]), 
						(int)(circleCenterY+cosineSub45ArrayR[arrayIndex]));
				else
					rangeAttackUnits[ir]->move((int)(circleCenterX-sineAdd45ArrayR[arrayIndex]),
						(int)(circleCenterY+cosineAdd45ArrayR[arrayIndex]));
				
				if(theSign < 0)
						arrayIndex++;

				theSign = -theSign;
			}
		}
	}
	else if(slantNegYPosX)
	{
		arrayIndex = 0;
		theSign = -1;
		for(ic = 0; ic<nClose; ic++)
		{
			if(!ic)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineAdd45ArrayC[0]), (int)(circleCenterY-cosineAdd45ArrayC[0]));
			else if(theSign> 0)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineAdd45ArrayC[arrayIndex]), 
					(int)(circleCenterY-cosineAdd45ArrayC[arrayIndex]));
			else
				closeAttackUnits[ic]->move((int)(circleCenterX-sineSub45ArrayC[arrayIndex]),
					(int)(circleCenterY-cosineSub45ArrayC[arrayIndex]));

			if(theSign < 0)
					arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;
			for(ir = 0; ir<nRange; ir++)
			{
				if(!ir)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineAdd45ArrayR[0]), (int)(circleCenterY-cosineAdd45ArrayR[0]));
				else if(theSign> 0)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineAdd45ArrayR[arrayIndex]), 
						(int)(circleCenterY-cosineAdd45ArrayR[arrayIndex]));
				else
					rangeAttackUnits[ir]->move((int)(circleCenterX-sineSub45ArrayR[arrayIndex]),
						(int)(circleCenterY-cosineSub45ArrayR[arrayIndex]));

				if(theSign < 0)
						arrayIndex++;

				theSign = -theSign;
			}
		}
	}
	else if(slantNegYNegX)
	{
		arrayIndex = 0;
		theSign = -1;
		for(ic = 0; ic<nClose; ic++)
		{
			if(!ic)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineSub45ArrayC[0]), (int)(circleCenterY-cosineSub45ArrayC[0]));
			else if(theSign> 0)
				closeAttackUnits[ic]->move((int)(circleCenterX+sineSub45ArrayC[arrayIndex]), 
					(int)(circleCenterY-cosineSub45ArrayC[arrayIndex]));
			else
				closeAttackUnits[ic]->move((int)(circleCenterX-sineAdd45ArrayC[arrayIndex]),
					(int)(circleCenterY-cosineAdd45ArrayC[arrayIndex]));
			if(theSign < 0)
					arrayIndex++;

			theSign = -theSign;
		}
		if(nRange)
		{
			arrayIndex = 0;
			theSign = -1;
			for(ir = 0; ir<nRange; ir++)
			{
				if(!ir)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineSub45ArrayR[0]), (int)(circleCenterY-cosineSub45ArrayR[0]));
				else if(theSign> 0)
					rangeAttackUnits[ir]->move((int)(circleCenterX+sineSub45ArrayR[arrayIndex]), 
						(int)(circleCenterY-cosineSub45ArrayR[arrayIndex]));
				else
					rangeAttackUnits[ir]->move((int)(circleCenterX-sineAdd45ArrayR[arrayIndex]),
						(int)(circleCenterY-cosineAdd45ArrayR[arrayIndex]));
				
				if(theSign < 0)
						arrayIndex++;

				theSign = -theSign;
			}
		}
	}
					
	if(sineArrayC)
		mem_del(sineArrayC);
	if(cosineArrayC)
		mem_del(cosineArrayC);
	if(sineAdd45ArrayC)
		mem_del(sineAdd45ArrayC);
	if(cosineAdd45ArrayC)
		mem_del(cosineAdd45ArrayC);
	if(sineSub45ArrayC)
		mem_del(sineSub45ArrayC);
	if(cosineSub45ArrayC)
		mem_del(cosineSub45ArrayC);
	if(sineArrayR)
		mem_del(sineArrayR);
	if(cosineArrayR)
		mem_del(cosineArrayR);
	if(sineAdd45ArrayR)
		mem_del(sineAdd45ArrayR);
	if(cosineAdd45ArrayR)
		mem_del(cosineAdd45ArrayR);
	if(sineSub45ArrayR)
		mem_del(sineSub45ArrayR);
	if(cosineSub45ArrayR)
		mem_del(cosineSub45ArrayR);

	if(closeAttackUnits)
		mem_del(closeAttackUnits);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
}
//--- End of function UnitGroup::arc_formation ---//

//------Begin function UnitGroup::staggered_formation------//
//This function calculates the position and set the unit to move to the locations to form
//a staggered line formation
void UnitGroup::staggered_formation(int direction)
{
	//------------Define local variables---------//
	int nSelected = 0; 
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int* sizeArrayX=0, *sizeArrayY=0;
	int finalDirection;
	Unit* unitPtr;
	//-------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
	sizeArrayX = (int*)mem_add(nSelected*sizeof(int));
	sizeArrayY = (int*)mem_add(nSelected*sizeof(int));
//-------------------------------------//	
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	int nRange=0, nClose=0; //number of units with range attack and close attack respectively
	Unit* general=NULL;
	
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		else if(unitPtr->max_attack_range() > 1)
			nRange++;
		else
			nClose++;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}		

	bool generalIsHere = true;
	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}
	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	int centerX = general->next_x_loc();//(maxLocX+minLocX)/2;
	int centerY = general->next_y_loc();//(maxLocY+minLocY)/2;

	Unit** closeAttackUnits = (Unit**)mem_add(nClose*sizeof(Unit*));
	Unit** rangeAttackUnits = (Unit**)mem_add(nRange*sizeof(Unit*));
	
	int ic = 0, ir =0;

	int diff;

	for(t=1; t<=nSelected; t++)
	{
		unitPtr = get_unit(t);
		if(unitPtr->team_info) //is the general
			continue;
		if(unitPtr->max_attack_range() > 1)
			rangeAttackUnits[ir++] = unitPtr;
		else
			closeAttackUnits[ic++] = unitPtr;
	}
	err_when(ir != nRange || ic != nClose);

	Unit** sortedArray = (Unit**)mem_add(nSelected*sizeof(Unit*));
	ic = 0;
	ir = 0;

	if(generalIsHere)
		sortedArray[0] = general;
	else
	{
		if(nRange)
			sortedArray[0] = rangeAttackUnits[ir++];
		else
			sortedArray[0] = closeAttackUnits[ic++];
	}

	for(t=1; t<nSelected; t++)
	{
		if(t%4 == 3)
		{
			if(ir < nRange)
			{
				sortedArray[t] = rangeAttackUnits[ir++];
				if(ir < nRange)
				{	
					t++;
					sortedArray[t] = rangeAttackUnits[ir++];
				}
			}
			else
				sortedArray[t] = closeAttackUnits[ic++];
		}
		else
		{
			if(ic < nClose)
				sortedArray[t] = closeAttackUnits[ic++];
			else
				sortedArray[t] = rangeAttackUnits[ir++];
		}
	}
	err_when(ic != nClose || ir != nRange);

	int widthLeft=0, widthRight =0, lengthLeft = 0, lengthRight = 0;
	for(t=0; t<nSelected; t++)
	{
		sizeArrayX[t] = sortedArray[t]->obj_loc_width();
		sizeArrayY[t] = sortedArray[t]->obj_loc_height();
		if(t%2)
		{
			widthLeft += sizeArrayX[t];
			lengthLeft += sizeArrayY[t];
		}
		else
		{
			widthRight += sizeArrayX[t];
			lengthRight += sizeArrayY[t];
		}
	}

	//---FIRST, we must determine the direction in which the formation should be oriented---//
	bool leaderAtMiddleSpanX = false;
	bool leaderAtMiddleSpanY = false;
	bool slantBottomLeft = false;
	bool slantBottomRight = false;
	bool slantTopLeft = false;
	bool slantTopRight = false;

	int multiplySign = 1;
	
	bool check = false;

	int tempOldX1, tempOldX2;
	int tempOldY1, tempOldY2;
	
	if(!direction || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX-widthLeft) < 0)
			locX-=diff;
		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+2*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX+sizeArrayX[0];
		tempOldX2 = locX;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += sizeArrayX[tSelected];
			}
			else
			{
				tempOldX2 -= sizeArrayX[tSelected];
				temperoryX = tempOldX2;
			}
					
			temperoryY = locY+maximumSizeY*(((tSelected+1)/2)%2);
			
			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_BOTTOM)
		{
			leaderAtMiddleSpanX = true;
			multiplySign = 1;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX-widthLeft) < 0)
			locX-=diff;
		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX+sizeArrayX[0];
		tempOldX2 = locX;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += sizeArrayX[tSelected];
			}
			else
			{
				tempOldX2 -= sizeArrayX[tSelected];
				temperoryX = tempOldX2;
			}
					
			temperoryY = locY-maximumSizeY*(((tSelected+1)/2)%2);

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_TOP)
		{
			leaderAtMiddleSpanX = true;
			multiplySign = -1;
			finalDirection = LEADER_AT_TOP;
		}
	}

	if((!direction && !check )|| direction == LEADER_AT_RIGHT)
	{
		locX = centerX;
		locY = centerY;

		if(locX < 0)
			locX =0;
		if((diff=locX+2*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+lengthLeft+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-lengthRight+sizeArrayY[0]) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldY1 = locY+sizeArrayY[0];
		tempOldY2 = locY;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryY = tempOldY1;
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldY2 -= sizeArrayY[tSelected];
				temperoryY = tempOldY2;
			}
					
			temperoryX = locX+maximumSizeX*((tSelected/2)%2);

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_RIGHT)
		{
			leaderAtMiddleSpanY = true;
			multiplySign = 1;
			finalDirection = LEADER_AT_RIGHT;
		}
	}

	if((!direction && !check )|| direction == LEADER_AT_LEFT)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX-maximumSizeX) < 0)
			locX -=diff;
		if((diff=locX+maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+lengthLeft+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-lengthRight+sizeArrayY[0]) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldY1 = locY+sizeArrayY[0];
		tempOldY2 = locY;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryY = tempOldY1;
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldY2 -= sizeArrayY[tSelected];
				temperoryY = tempOldY2;
			}
					
			temperoryX = locX-maximumSizeX*((tSelected/2)%2);

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_LEFT)
		{
			leaderAtMiddleSpanY = true;
			multiplySign = -1;
			finalDirection = LEADER_AT_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX+((nSelected+2)/4+1)*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locY+((nSelected-1)/4+1)*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locX-((nSelected-1)/4+1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-(nSelected/4)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locX;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				if(tSelected%4==3)
					tempOldX1+=maximumSizeX;
				else
					tempOldY1+=maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if(tSelected%4)
					tempOldX2-=maximumSizeX;
				else
					tempOldY2-=maximumSizeY;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_BOTTOM_LEFT)
		{
			slantBottomLeft = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX+((nSelected+3)/4+1)*maximumSizeX+1-MAX_WORLD_X_LOC) >0)
			locX -= diff;
		if((diff=locY+((nSelected+1)/4+1)*maximumSizeY+1-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locX-(nSelected/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-((nSelected+2)/4)*maximumSizeY) <0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locX;

		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				if(tSelected%4==3)
					tempOldY1+=maximumSizeY;
				else
					tempOldX1+=maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if(tSelected%4)
					tempOldY2-=maximumSizeY;
				else
					tempOldX2-=maximumSizeX;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_TOP_RIGHT)
		{
			slantTopRight = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX;
		locY = centerY;
		
		if((diff=locX+((nSelected+2)/4+1)*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locY+((nSelected+3)/4+1)*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locX-((nSelected+1)/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-(nSelected/4)*maximumSizeY) <0)
			locY -= diff;

		tSelected = 1;
		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;

		while(check && tSelected < nSelected)
		{
			if(tSelected % 2)
			{
				if(tSelected%4 == 3)
					tempOldX1 -= maximumSizeX;
				else
					tempOldY1 += maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if(tSelected%4)
					tempOldX2 += maximumSizeX;
				else
					tempOldY2 -= maximumSizeY;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_BOTTOM_RIGHT)
		{
			slantBottomRight = true;
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX+((nSelected/4)+1)*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locY+((nSelected+1)/4+1)*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locX-((nSelected+3)/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-((nSelected+2)/4)*maximumSizeY) < 0)
			locY -= diff;

		check = true;

		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tSelected = 1;

		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;

		while(check && tSelected < nSelected)
		{
			if(tSelected % 2)
			{
				if(tSelected%4 == 3)
					tempOldY1 += maximumSizeY;
				else
					tempOldX1 -= maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if(tSelected%4)
					tempOldY2 -= maximumSizeY;
				else
					tempOldX2 += maximumSizeX;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}

			if(!sortedArray[tSelected]->can_move(temperoryX, temperoryY))
				check = false;

			tSelected++;
		}
		if(check || direction == LEADER_AT_TOP_LEFT)
		{
			slantTopLeft = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	
	//---if all places for all formation are occupied---//
	if (!check && !direction)
	{
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			
			for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
				for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
					world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		if(sizeArrayX)
			mem_del(sizeArrayX);
		if(sizeArrayY)
			mem_del(sizeArrayY);
		if(closeAttackUnits)
			mem_del(closeAttackUnits);
		if(rangeAttackUnits)
			mem_del(rangeAttackUnits);
		if(sortedArray)
			mem_del(sortedArray);
		return;
	}

	formation_mode = FORMATION_STAGGERED;

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = sortedArray[tSelected]->next_x_loc(); i < sizeArrayX[tSelected]; i++, tempLocX++)				
			for (j = 0, tempLocY = sortedArray[tSelected]->next_y_loc(); j < sizeArrayY[tSelected]; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}

	//---set the units to move to the location assigned in order to form a formation---//
	tSelected = 0;

	if(leaderAtMiddleSpanX)
	{
		tempOldX1 = locX+sizeArrayX[0];
		tempOldX2 = locX+sizeArrayX[0];
	}
	else if(leaderAtMiddleSpanY)
	{
		tempOldY1 = locY+sizeArrayY[0];
		tempOldY2 = locY+sizeArrayY[0];
	}
	else if(slantBottomLeft || slantBottomRight)
	{
		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locY+maximumSizeY;
	}
	else if(slantTopRight)
	{
		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX+maximumSizeX;
		tempOldY2 = locY;
	}
	else if(slantTopLeft)
	{
		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX-maximumSizeX;
		tempOldY2 = locY;
	}
		
	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(leaderAtMiddleSpanX)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(tempOldX1, (locY+multiplySign*maximumSizeY*(((tSelected+1)/2)%2)));
				tempOldX1 += sizeArrayX[tSelected];
			}
			else
			{
				tempOldX2 -= sizeArrayX[tSelected];
				sortedArray[tSelected]->move(tempOldX2, (locY+multiplySign*maximumSizeY*(((tSelected+1)/2)%2)));
			}
		}
		else if(leaderAtMiddleSpanY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move((locX+multiplySign*maximumSizeX*(((tSelected+1)/2)%2)), tempOldY1);
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldY2 -= sizeArrayY[tSelected];
				sortedArray[tSelected]->move((locX+multiplySign*maximumSizeX*(((tSelected+1)/2)%2)), tempOldY2);
			}
		}
		else if(slantBottomLeft)
		{
			if(tSelected%2)
			{
				if(tSelected%4 == 3)
					tempOldX1+= maximumSizeX;
				else
					tempOldY1+= maximumSizeY;
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
			}
			else
			{
				if(tSelected%4)
					tempOldX2-=maximumSizeX;
				else
					tempOldY2-=maximumSizeY;
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}
		}
		else if(slantBottomRight)
		{
			if(tSelected%2)
			{
				if(tSelected%4 == 3)
					tempOldX1 -= maximumSizeX;
				else
					tempOldY1 += maximumSizeY;
				
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
			}
			else
			{
				if(tSelected%4)
					tempOldX2 += maximumSizeX;
				else
					tempOldY2 -= maximumSizeY;

				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}
		}
		else if(slantTopLeft)
		{
			if(tSelected % 2)
			{
				if(tSelected%4 == 3)
					tempOldY1 += maximumSizeY;
				else
					tempOldX1 -= maximumSizeX;

				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
			}
			else
			{
				if(tSelected%4)
					tempOldY2 -= maximumSizeY;
				else
					tempOldX2 += maximumSizeX;
				
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}

		}
		else if(slantTopRight)
		{
			if(tSelected%2)
			{
				if(tSelected%4==3)
					tempOldY1+=maximumSizeY;
				else
					tempOldX1+=maximumSizeX;
				
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
			}
			else
			{
				if(tSelected%4)
					tempOldY2-=maximumSizeY;
				else
					tempOldX2-=maximumSizeX;
				
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}
		}
		else
			err_here();

		sortedArray[tSelected]->set_in_formation();
		//----set the occupancy of the locations originally occupied by the troop to be true--//
	}	
		
	if(sizeArrayX)
		mem_del(sizeArrayX);
	if(sizeArrayY)
		mem_del(sizeArrayY);
	if(closeAttackUnits)
		mem_del(closeAttackUnits);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
	if(sortedArray)
		mem_del(sortedArray);
}
//--------End function UnitGroup::staggered_formation------//

//--------Begin of function UnitGroup::square_formatiion-----//
void UnitGroup::square_formation(int direction)
{
	//------------Define local variables------------//
	int nSelected = 0; 
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int numberOfLines;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int formationSpaceX, formationSpaceY;
	Unit* unitPtr;
	int finalDirection=0;
	//----------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	Unit** sortedArrayTmp = (Unit**) mem_add_clear((nSelected)*sizeof(Unit*));
	int unitIndex=0;
	Unit* general=0;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(!unitPtr->team_info)
			sortedArrayTmp[unitIndex++] = unitPtr;
		else
			general = unitPtr;
		
		err_when(unitIndex > nSelected);
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}		

	bool generalIsHere = true;

	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}
	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	int centerX = ((maxLocX+minLocX)%2)?(maxLocX+minLocX+1)/2:(maxLocX+minLocX)/2;
	int centerY = ((maxLocY+minLocY)%2)?(maxLocY+minLocY+1)/2:(maxLocY+minLocY)/2;
	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<(nSelected-(int)(generalIsHere)); i++)
	{
		for(j=0; j<i; j++)
		{
			if(sortedArrayTmp[j]->max_attack_range() > sortedArrayTmp[i]->max_attack_range())
			{
				tempUnit = sortedArrayTmp[i];
				sortedArrayTmp[i] = sortedArrayTmp[j];
				sortedArrayTmp[j] = tempUnit;
			}
		}
	}
	
	//----calculate the line numbers----//
	numberOfLines = (nSelected/5+2);
	if (numberOfLines > 4)	numberOfLines = 4;
	//----------------------------------//

	//----calculate the row numbers-----//
	int rows = (nSelected-1)/numberOfLines;
	int numberOfRows = rows +1;
	//----------------------------------//

	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
	int midPosition;
	if(numberOfLines == 2)
		midPosition = nSelected-1;
	else if(numberOfLines == 3)
		midPosition = 4;
	else if(numberOfLines == 4)
		midPosition = 9;
	else
		err_here();

	for(i=0; i<nSelected; i++)
	{
		if(i<midPosition)
			sortedArray[i] = sortedArrayTmp[i];
		else if(i== midPosition && generalIsHere)
			sortedArray[i] = general;
		else
			sortedArray[i] = sortedArrayTmp[i-((int)generalIsHere)];
	}

	mem_del(sortedArrayTmp);

	formationSpaceX = maximumSizeX;
	formationSpaceY = maximumSizeY;
	bool check = false;
	int xMultiplicativeFactor, yMultiplicativeFactor;
	int theSecondTemperoryX, theSecondTemperoryY;
	//leader at lower left//
	int diff;

	if (!direction || direction == LEADER_AT_TOP)
	{
		locX = centerX - (numberOfLines/2)*formationSpaceX;
		locY = centerY - (numberOfRows/2)*formationSpaceY;
		
		if((diff = locX+numberOfLines*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff = locY+numberOfRows*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_TOP) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_TOP;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		locX = centerX+((numberOfRows-1)/2)*formationSpaceX;
		locY = centerY-(numberOfLines/2)*formationSpaceY;

		if((diff=locX+maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+numberOfLines*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_RIGHT) 
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_RIGHT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX+((numberOfLines-1)/2)*maximumSizeX;
		locY = centerY+((numberOfRows-1)/2)*maximumSizeY;

		if((diff = locX+maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-(numberOfLines-1)*maximumSizeX) <0)
			locX -= diff;
		if((diff = locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_BOTTOM) 
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		locX = centerX-(numberOfRows/2)*maximumSizeX;
		locY = centerY+((numberOfLines-1)/2)*maximumSizeY;

		if((diff=locX+numberOfRows*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff=locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_LEFT) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_LEFT;
		}
	}
	
	//slant formations
	int firstX, firstY;
	bool slant = false;
	
	int firstXIncrement, firstYIncrement;

	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		locX = centerX;
		locY = centerY-(numberOfRows-1)*maximumSizeY;

		if((diff=locX+numberOfLines*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+(numberOfLines+numberOfRows-1)*maximumSizeY+1-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if(locY<0)
			locY = 0;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		firstX = locX;
		firstY = locY;
		while(check && tSelected<nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX-=maximumSizeX;
				firstY+=maximumSizeY;
			}
			temperoryX = firstX+(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY+(tSelected%numberOfLines)*maximumSizeY;
			
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_TOP_RIGHT)
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			firstXIncrement = -1;
			firstYIncrement = 1;
			slant = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		locX = centerX-(numberOfRows-1)*maximumSizeX;
		locY = centerY;

		if((diff=locX+(numberOfLines+numberOfRows-1)*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX -= diff;
		if((diff=locY+numberOfRows*maximumSizeY+1-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
	
		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX+=maximumSizeX;
				firstY+=maximumSizeY;
			}
			temperoryX = firstX+(tSelected%numberOfLines)*maximumSizeX;	
			temperoryY = firstY-(tSelected%numberOfLines)*maximumSizeY;
			
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_TOP_LEFT) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = -1;
			firstXIncrement = 1;
			firstYIncrement = 1;
			slant = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX;
		locY = centerY+(numberOfRows-1)*maximumSizeY;

		if((diff=locX+numberOfRows*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines+numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;

		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX += maximumSizeX;
				firstY -= maximumSizeY;
			}
			temperoryX = firstX-(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY-(tSelected%numberOfLines)*maximumSizeY;
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_BOTTOM_LEFT)
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = -1;
			firstXIncrement = 1;
			firstYIncrement = -1;
			slant = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX+(numberOfRows-1)*maximumSizeX;
		locY = centerY;

		if((diff=locX+maximumSizeX+1-MAX_WORLD_X_LOC) >0 )
			locX -= diff;
		if((diff=locX-(numberOfLines+numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff = locY+numberOfLines*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;

		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX -= maximumSizeX;
				firstY -= maximumSizeY;
			}
			temperoryX = firstX-(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY+(tSelected%numberOfLines)*maximumSizeY;
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_BOTTOM_RIGHT)
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = 1;
			firstXIncrement = -1;
			firstYIncrement = -1;
			slant = true;
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}
	//if all the formation mode are not allowed//
	if (!check && !direction)
	{
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			
			for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
				for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
					world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		if(sortedArray)
			mem_del(sortedArray);
		return;
	}

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = sortedArray[tSelected]->next_x_loc(); i < sortedArray[tSelected]->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = sortedArray[tSelected]->next_y_loc(); j < sortedArray[tSelected]->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}

	formation_mode = FORMATION_SQUARE;
	tSelected = 0;

	if(!slant)
	{
		xMultiplicativeFactor *= formationSpaceX;
		yMultiplicativeFactor *= formationSpaceY;
	}
	firstX = locX;
	firstY = locY;
	
	//set the units to move to the assigned location to form a square formation
	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(!slant)
		{
			if(yMultiplicativeFactor == xMultiplicativeFactor)
				sortedArray[tSelected]->move((locX+(xMultiplicativeFactor)*(tSelected%numberOfLines)), (locY+(yMultiplicativeFactor)*(tSelected/numberOfLines)));
			else
				sortedArray[tSelected]->move((locX+(xMultiplicativeFactor)*(tSelected/numberOfLines)), (locY+(yMultiplicativeFactor)*(tSelected%numberOfLines)));
		}
		else
		{
			if(tSelected && !(tSelected%numberOfLines))
			{
				firstX += (firstXIncrement*maximumSizeX);
				firstY += (firstYIncrement*maximumSizeY);
			}
			sortedArray[tSelected]->move(firstX+xMultiplicativeFactor*(tSelected%numberOfLines)*maximumSizeX, firstY+yMultiplicativeFactor*(tSelected%numberOfLines)*maximumSizeY);
		}

		//set the units' angle to be the same as the leader's angle
		sortedArray[tSelected]->set_in_formation();
	}
	if(sortedArray)
		mem_del(sortedArray);
}
//--------------End of function UnitGroup::square_formation--------//

//--------Begin of function UnitGroup::square_formatiion-----//
void UnitGroup::square_preformation(int direction)
{
	//------------Define local variables------------//
	int nSelected = 0; 
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int numberOfLines;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int formationSpaceX, formationSpaceY;
	Unit* unitPtr;
	Unit* general=0;
	int finalDirection=0;
	//----------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	Unit** sortedArrayTmp = (Unit**) mem_add((nSelected)*sizeof(Unit*));
	int unitIndex=0;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(!unitPtr->team_info)
			sortedArrayTmp[unitIndex++] = unitPtr;
		else
			general = unitPtr;

		err_when(unitIndex > nSelected);
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}	

	bool generalIsHere = true;
	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}
	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;
	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<(nSelected-((int)generalIsHere)); i++)
	{
		for(j=0; j<i; j++)
		{
			if(sortedArrayTmp[j]->max_attack_range() > sortedArrayTmp[i]->max_attack_range())
			{
				tempUnit = sortedArrayTmp[i];
				sortedArrayTmp[i] = sortedArrayTmp[j];
				sortedArrayTmp[j] = tempUnit;
			}
		}
	}

	numberOfLines = (nSelected/5+2);
	if (numberOfLines > 4)	numberOfLines = 4;
	//----------------------------------//

	//----calculate the row numbers-----//
	int rows = (nSelected-1)/numberOfLines;
	int numberOfRows = rows +1;
	//----------------------------------//

		
	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
	int midPosition;
	if(numberOfLines == 2)
		midPosition = nSelected-1;
	else if(numberOfLines == 3)
		midPosition = 4;
	else if(numberOfLines == 4)
		midPosition = 9;
	else
		err_here();

	for(i=0; i<nSelected; i++)
	{
		if(i<midPosition)
			sortedArray[i] = sortedArrayTmp[i];
		else if(i== midPosition && generalIsHere)
			sortedArray[i] = general;
		else
			sortedArray[i] = sortedArrayTmp[i-((int)generalIsHere)];
	}

	mem_del(sortedArrayTmp);
	//----calculate the line numbers----//

	formationSpaceX = 2*maximumSizeX;
	formationSpaceY = 2*maximumSizeY;
	bool check = false;
	int xMultiplicativeFactor, yMultiplicativeFactor;
	int theSecondTemperoryX, theSecondTemperoryY;
	//leader at lower left//
	int diff;

	if (!direction || direction == LEADER_AT_TOP)
	{
		locX = centerX - (numberOfLines/2)*formationSpaceX;
		locY = centerY - (numberOfRows/2)*formationSpaceY;
		
		if((diff = locX+numberOfLines*formationSpaceX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff = locY+numberOfRows*formationSpaceY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_TOP) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_TOP;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		locX = centerX+(numberOfRows/2)*formationSpaceX;
		locY = centerY-(numberOfLines/2)*formationSpaceY;

		if((diff=locX+formationSpaceX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfRows-1)*formationSpaceX) < 0)
			locX -= diff;
		if((diff=locY+numberOfLines*formationSpaceY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_RIGHT) 
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_RIGHT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX+(numberOfLines/2)*maximumSizeX;
		locY = centerY+(numberOfRows/2)*maximumSizeY;

		if((diff = locX+formationSpaceX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-(numberOfLines-1)*formationSpaceX) <0)
			locX -= diff;
		if((diff = locY+formationSpaceY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*formationSpaceY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_BOTTOM) 
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		locX = centerX-(numberOfRows/2)*formationSpaceX;
		locY = centerY+(numberOfLines/2)*formationSpaceY;

		if((diff=locX+numberOfRows*formationSpaceX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff=locY+formationSpaceY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*formationSpaceY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		//if not occupied//
		if (check || direction == LEADER_AT_LEFT) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_LEFT;
		}
	}
	
	//slant formations
	int firstX, firstY;
	bool slant = false;
	
	int firstXIncrement, firstYIncrement;

	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		locX = centerX;
		locY = centerY-(numberOfRows-1)*maximumSizeY;

		if((diff=locX+numberOfLines*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+(numberOfLines+numberOfRows-1)*maximumSizeY+1-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if(locY<0)
			locY = 0;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
		
		firstX = locX;
		firstY = locY;
		while(check && tSelected<nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX-=maximumSizeX;
				firstY+=maximumSizeY;
			}
			temperoryX = firstX+(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY+(tSelected%numberOfLines)*maximumSizeY;
			
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_TOP_RIGHT)
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			firstXIncrement = -1;
			firstYIncrement = 1;
			slant = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		locX = centerX-(numberOfRows-1)*maximumSizeX;
		locY = centerY;

		if((diff=locX+(numberOfLines+numberOfRows-1)*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX -= diff;
		if((diff=locY+numberOfRows*maximumSizeY+1-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;
	
		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX+=maximumSizeX;
				firstY+=maximumSizeY;
			}
			temperoryX = firstX+(tSelected%numberOfLines)*maximumSizeX;	
			temperoryY = firstY-(tSelected%numberOfLines)*maximumSizeY;
			
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_TOP_LEFT) 
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = -1;
			firstXIncrement = 1;
			firstYIncrement = 1;
			slant = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX;
		locY = centerY+(numberOfRows-1)*maximumSizeY;

		if((diff=locX+numberOfRows*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines+numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;

		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX += maximumSizeX;
				firstY -= maximumSizeY;
			}
			temperoryX = firstX-(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY-(tSelected%numberOfLines)*maximumSizeY;
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_BOTTOM_LEFT)
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = -1;
			firstXIncrement = 1;
			firstYIncrement = -1;
			slant = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX+(numberOfRows-1)*maximumSizeX;
		locY = centerY;

		if((diff=locX+maximumSizeX+1-MAX_WORLD_X_LOC) >0 )
			locX -= diff;
		if((diff=locX-(numberOfLines+numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff = locY+numberOfLines*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
					check = false;

		firstX = locX;
		firstY = locY;
		while(check && tSelected < nSelected)
		{
			if(!(tSelected%numberOfLines))
			{
				firstX -= maximumSizeX;
				firstY -= maximumSizeY;
			}
			temperoryX = firstX-(tSelected%numberOfLines)*maximumSizeX;
			temperoryY = firstY+(tSelected%numberOfLines)*maximumSizeY;
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(mobile_type))
						check = false;					
			++tSelected;
		}
		if(check || direction == LEADER_AT_BOTTOM_RIGHT)
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = 1;
			firstXIncrement = -1;
			firstYIncrement = -1;
			slant = true;
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}
	//if all the formation mode are not allowed//
	if (!check && !direction)
	{
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			
			for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
				for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
					world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		if(sortedArray)
			mem_del(sortedArray);
		return;
	}

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = sortedArray[tSelected]->next_x_loc(); i < sortedArray[tSelected]->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = sortedArray[tSelected]->next_y_loc(); j < sortedArray[tSelected]->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}

	formation_mode = FORMATION_SQUARE;
	tSelected = 0;

	if(!slant)
	{
		xMultiplicativeFactor *= formationSpaceX;
		yMultiplicativeFactor *= formationSpaceY;
	}
	firstX = locX;
	firstY = locY;
	
	//set the units to move to the assigned location to form a square formation
	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(!slant)
		{
			if(yMultiplicativeFactor == xMultiplicativeFactor)
				sortedArray[tSelected]->move((locX+(xMultiplicativeFactor)*(tSelected%numberOfLines)), (locY+(yMultiplicativeFactor)*(tSelected/numberOfLines)));
			else
				sortedArray[tSelected]->move((locX+(xMultiplicativeFactor)*(tSelected/numberOfLines)), (locY+(yMultiplicativeFactor)*(tSelected%numberOfLines)));
		}
		else
		{
			if(tSelected && !(tSelected%numberOfLines))
			{
				firstX += (firstXIncrement*maximumSizeX);
				firstY += (firstYIncrement*maximumSizeY);
			}
			sortedArray[tSelected]->move(firstX+xMultiplicativeFactor*(tSelected%numberOfLines)*maximumSizeX, firstY+yMultiplicativeFactor*(tSelected%numberOfLines)*maximumSizeY);
		}

		//set the units' angle to be the same as the leader's angle
		sortedArray[tSelected]->set_in_formation();
	}
	if(sortedArray)
		mem_del(sortedArray);
}
//--------------End of function UnitGroup::square_formation--------//

//--------------Begin function UnitGroup::sandwich_formation-------//
void UnitGroup::sandwich_formation(int direction)
{
	//----------Define local variables------------//
	int nSelected = 0; 
	int locX, locY, i, j, t, tempLocX, tempLocY, tSelected =0;
	int temperoryX, temperoryY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	int maxLocX, minLocX, maxLocY, minLocY;
	int* sizeArrayX=0, *sizeArrayY=0;
	Unit* unitPtr;
	int finalDirection=0;
	int nClose, nRange;
//--------------------------------------------//

	if(size() < 2)
		return;
//----------Some initializations-------//	
	nSelected = size();
//-------------------------------------//
	int sizeIndex = 0;
	maximumSizeX = 0;
	maximumSizeY = 0;
	maxLocX = 0;
	minLocX = MAX_WORLD_X_LOC;
	maxLocY = 0;
	minLocY = MAX_WORLD_Y_LOC;
	Unit* general=0;
	int unitIndex=0;
	nRange = 0;
	nClose = 0;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
		else if(unitPtr->max_attack_range() > 1)
			nRange++;
		else
			nClose++;
		//--Determine the maximum size of the unit in the troop---//
		temperorySizeX = unitPtr->obj_loc_width();
		temperorySizeY = unitPtr->obj_loc_height();
		if (maximumSizeX < temperorySizeX) 
			maximumSizeX = temperorySizeX;
		if (maximumSizeY < temperorySizeY) 
			maximumSizeY = temperorySizeY;
		temperoryX = unitPtr->next_x_loc();
		temperoryY = unitPtr->next_y_loc();
		if (maxLocX < temperoryX)
			maxLocX = temperoryX;
		if (minLocX > temperoryX)
			minLocX = temperoryX;
		if (maxLocY < temperoryY)
			maxLocY = temperoryY;
		if (minLocY > temperoryY)
			minLocY = temperoryY;
		//--set the occupancy of the place where is originally occupied by the troop to be false--//
		for (i = 0, tempLocX = temperoryX; i < temperorySizeX; i++, tempLocX++)				
			for (j = 0, tempLocY = temperoryY; j < temperorySizeY; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->remove_unit(mobile_type);
	}		

	bool generalIsHere = true;
	if(!general)
	{
		general = unit_array[get_unit(1)->leader_unit_recno];
		generalIsHere = false;
	}
	if(!direction && general->team_info->formation_direction)
		direction = general->team_info->formation_direction;

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;
	Unit** closeAttackUnits = NULL;
	Unit** rangeAttackUnits = NULL;

	if(nClose)
		closeAttackUnits = (Unit**)mem_add(nClose*sizeof(Unit*));
	if(nRange)
		rangeAttackUnits = (Unit**)mem_add(nRange*sizeof(Unit*));
	
	int ic = 0, ir =0;

	for(t=1; t<=nSelected; t++)
	{
		unitPtr = get_unit(t);
		if(unitPtr->team_info) //is the general
			continue;
		if(unitPtr->max_attack_range() > 1)
			rangeAttackUnits[ir++] = unitPtr;
		else
			closeAttackUnits[ic++] = unitPtr;
	}
	err_when(ir != nRange || ic != nClose);

	int widthRightForClose=0, widthLeftForClose=0;
	int heightRightForClose=0, heightLeftForClose=0;

	for(i=0; i<nClose; i++)
	{
		if(i%2 || i==0)
		{
			widthRightForClose+=closeAttackUnits[i]->obj_loc_width();
			heightRightForClose+=closeAttackUnits[i]->obj_loc_height();
		}
		else
		{
			widthLeftForClose+=closeAttackUnits[i]->obj_loc_width();
			heightLeftForClose+=closeAttackUnits[i]->obj_loc_height();
		}
	}
	int widthRightForRange=0, widthLeftForRange=0;
	int heightRightForRange=0, heightLeftForRange=0;

	for(i=0; i<nRange; i++)
	{
		if(i%2 || i==0)
		{
			widthRightForRange+=rangeAttackUnits[i]->obj_loc_width();
			heightRightForRange+=rangeAttackUnits[i]->obj_loc_height();
		}
		else
		{
			widthLeftForRange+=rangeAttackUnits[i]->obj_loc_width();
			heightLeftForRange+=rangeAttackUnits[i]->obj_loc_height();
		}
	}
	if (nRange)
	{
		widthLeftForRange += rangeAttackUnits[0]->obj_loc_width();
		heightLeftForRange += rangeAttackUnits[0]->obj_loc_height();
	}

	int widthRight, widthLeft;
	int heightRight, heightLeft;
	if(nClose>nRange)
	{
		widthRight = widthRightForClose+general->obj_loc_width();
		heightRight = heightRightForClose+general->obj_loc_height();
		widthLeft = widthLeftForClose;
		heightLeft = heightLeftForClose;
	}
	else
	{
		widthRight = widthRightForRange;
		heightRight = heightRightForRange;
		widthLeft = widthLeftForRange;
		heightLeft = heightLeftForRange;
	}
	int diff;
	bool check = false;
	int tempOldX1, tempOldX2;
	int tempOldY1, tempOldY2;

	if(!direction || direction == LEADER_AT_TOP)
	{
		locX = centerX;
		locY = centerY+(nRange?0:1);
		if((diff = locX+widthRight+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-widthLeft-1) < 0)
			locX -= diff;
		if((diff = locY+2*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-maximumSizeY-1) < 0)
			locY -= diff;
		
		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;
		
		tempOldX1 = locX+general->obj_loc_width();
		tempOldX2 = locX;

		ic = 0;
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				tempOldX2 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
			}
			temperoryY = locY-maximumSizeY*(((ic+2)/2)%2);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}
		ir = 0;
		tempOldX1 = locX;
		tempOldX2 = locX;
		while(check && ir < nRange)
		{
			if(ir%2)
			{
				tempOldX2 -= rangeAttackUnits[ir]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
			}
			temperoryY = locY+maximumSizeY;
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}
		if(check || direction == LEADER_AT_TOP)
			finalDirection = LEADER_AT_TOP;
	}
	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		locX = centerX+(nRange?0:1);
		locY = centerY;
		if((diff = locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-heightLeft-1) < 0)
			locY -= diff;
		if((diff = locX+2*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-maximumSizeX-1) < 0)
			locX -= diff;
		
		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;
		
		tempOldY1 = locY+general->obj_loc_height();
		tempOldY2 = locY;

		ic = 0;
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				tempOldY2 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			temperoryX = locX-maximumSizeX*(((ic+2)/2)%2);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}
		ir = 0;
		tempOldY1 = locY;
		tempOldY2 = locY;
		while(check && ir < nRange)
		{
			if(ir%2)
			{
				tempOldY2 -= rangeAttackUnits[ir]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
			}
			temperoryX = locY+maximumSizeX;
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}
		if(check || direction == LEADER_AT_LEFT)
			finalDirection = LEADER_AT_LEFT;
	}
	if((!direction && !check)|| direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY+(nClose?0:1);
		if((diff = locX+widthRight+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-widthLeft-1) < 0)
			locX -= diff;
		if((diff = locY+2*maximumSizeY+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-maximumSizeY-1) < 0)
			locY -= diff;
		
		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;
		
		tempOldX1 = locX+general->obj_loc_width();
		tempOldX2 = locX;

		ic = 0;
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				tempOldX2 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
			}
			temperoryY = locY+maximumSizeY*(((ic+2)/2)%2);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}
		ir = 0;
		tempOldX1 = locX;
		tempOldX2 = locX;
		while(check && ir < nRange)
		{
			if(ir%2)
			{
				tempOldX2 -= rangeAttackUnits[ir]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
			}
			temperoryY = locY-maximumSizeY;
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}
		if(check || direction == LEADER_AT_BOTTOM)
			finalDirection = LEADER_AT_BOTTOM;
	}
	if((!direction && !check)|| direction == LEADER_AT_RIGHT)
	{
		locX = centerX+(nClose?0:1);
		locY = centerY;
		if((diff = locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-heightLeft-1) < 0)
			locY -= diff;
		if((diff = locX+2*maximumSizeX+1-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-maximumSizeX-1) < 0)
			locX -= diff;
		
		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;
		
		tempOldY1 = locY+general->obj_loc_height();
		tempOldY2 = locY;

		ic = 0;
		while(check && ic < nClose)
		{
			if(ic%2)
			{
				tempOldY2 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			temperoryX = locX+maximumSizeX*(((ic+2)/2)%2);
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}
		ir = 0;
		tempOldY1 = locY;
		tempOldY2 = locY;
		while(check && ir < nRange)
		{
			if(ir%2)
			{
				tempOldY2 -= rangeAttackUnits[ir]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
			}
			temperoryX = locX-maximumSizeX;
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}
		if(check || direction == LEADER_AT_RIGHT)
			finalDirection = LEADER_AT_RIGHT;
	}
	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		locX = general->next_x_loc();
		locY = general->next_y_loc();
		widthRightForClose = ((nClose+3)/4+1)*maximumSizeX;
		widthLeftForClose = (nClose/4)*maximumSizeX;
		heightRightForClose = ((nClose+1)/4+1)*maximumSizeY;
		heightLeftForClose = ((nClose+2)/4)*maximumSizeY;
		
		widthRight = (widthRightForClose>widthRightForRange)?widthRightForClose:widthRightForRange;
		widthLeft = (widthLeftForClose>widthLeftForRange)?widthLeftForClose:widthLeftForRange;
		heightRight = (heightRightForClose>heightRightForRange)?heightRightForClose:(heightRightForRange);
		heightLeft = (heightLeftForClose>heightLeftForRange)?heightLeftForClose:(heightLeftForRange);
		
		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-widthLeft-maximumSizeX-1) < 0)
			locX-=diff;
		if((diff=locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-heightLeft-maximumSizeY-1) < 0)
			locY-=diff;

		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locX;

		ic = 0;
		while(check && ic < nClose)
		{
			if((ic+1)%2)
			{
				if((ic+1)%4==3)
					tempOldY1+=maximumSizeY;
				else
					tempOldX1+=maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldY2-=maximumSizeY;
				else
					tempOldX2-=maximumSizeX;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		tempOldX1 = locX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX;
		tempOldY2 = locY+maximumSizeY;
		
		ir = 0;
		while(check && ir < nRange)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				temperoryY = tempOldY1;
				tempOldY1 += maximumSizeY;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				tempOldY2 -= maximumSizeY;
				temperoryY = tempOldY2;
			}
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}

		if(check || direction == LEADER_AT_TOP_RIGHT)
			finalDirection = LEADER_AT_TOP_RIGHT;
	}
	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		locX = general->next_x_loc();
		locY = general->next_y_loc();

		widthRightForClose = ((nSelected/4)+1)*maximumSizeX;
		heightRightForClose = ((nSelected+1)/4+1)*maximumSizeY;
		widthLeftForClose = ((nSelected+3)/4)*maximumSizeX;
		heightLeftForClose = ((nSelected+2)/4)*maximumSizeY;

		widthRight = (widthRightForClose>widthRightForRange)?widthRightForClose:widthRightForRange;
		widthLeft = (widthLeftForClose>widthLeftForRange)?widthLeftForClose:widthLeftForRange;
		heightRight = (heightRightForClose>heightRightForRange)?heightRightForClose:(heightRightForRange);
		heightLeft = (heightLeftForClose>heightLeftForRange)?heightLeftForClose:(heightLeftForRange);
		
		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-widthLeft-maximumSizeX-1) < 0)
			locX-=diff;
		if((diff=locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-heightLeft-maximumSizeY-1) < 0)
			locY-=diff;

		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;
		
		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;
		ic = 0;

		while(check && ic < nClose)
		{
			if((ic+1) % 2)
			{
				if((ic+1) %4 == 3)
					tempOldY1 += maximumSizeY;
				else
					tempOldX1 -= maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldY2 -= maximumSizeY;
				else
					tempOldX2 += maximumSizeX;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		tempOldX1 = locX+maximumSizeX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX+maximumSizeX;
		tempOldY2 = locY+maximumSizeY;
		ir = 0;

		//check whether the locations for forming formation are occupied
		while(check && ir < nRange)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				tempOldY1 -= maximumSizeY;
				temperoryY = tempOldY1;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
				tempOldY2 += maximumSizeY;
			}
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}

		if(check || direction == LEADER_AT_TOP_LEFT)
			finalDirection = LEADER_AT_TOP_LEFT;
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = general->next_x_loc();
		locY = general->next_y_loc();

		widthRightForClose = ((nClose+2)/4+1)*maximumSizeX;
		widthLeftForClose = ((nClose-1)/4+1)*maximumSizeX;	
		heightRightForClose = ((nClose-1)/4+1)*maximumSizeY;
		heightLeftForClose = (nClose/4)*maximumSizeY;

		widthRight = (widthRightForClose>widthRightForRange)?widthRightForClose:widthRightForRange;
		widthLeft = (widthLeftForClose>widthLeftForRange)?widthLeftForClose:widthLeftForRange;
		heightRight = (heightRightForClose>heightRightForRange)?heightRightForClose:(heightRightForRange);
		heightLeft = (heightLeftForClose>heightLeftForRange)?heightLeftForClose:(heightLeftForRange);
		
		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-widthLeft-maximumSizeX-1) < 0)
			locX-=diff;
		if((diff=locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-heightLeft-maximumSizeY-1) < 0)
			locY-=diff;

		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locX;
		ic = 0;

		while(check && ic < nClose)
		{
			if((ic+1)%2)
			{
				if((ic+1)%4==3)
					tempOldX1+=maximumSizeX;
				else
					tempOldY1+=maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldX2-=maximumSizeX;
				else
					tempOldY2-=maximumSizeY;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		tempOldX1 = locX;
		tempOldY1 = locY-maximumSizeY;
		tempOldX2 = locX;
		tempOldY2 = locY-maximumSizeY;
		
		ir = 0;
		while(check && ir < nRange)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				temperoryY = tempOldY1;
				tempOldY1 += maximumSizeY;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				tempOldY2 -= maximumSizeY;
				temperoryY = tempOldY2;
			}
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}

		if(check || direction == LEADER_AT_BOTTOM_LEFT)
			finalDirection = LEADER_AT_BOTTOM_LEFT;
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = general->next_x_loc();
		locY = general->next_y_loc();

		widthRightForClose = ((nClose+2)/4+1)*maximumSizeX;
		widthLeftForClose = ((nClose+1)/4)*maximumSizeX;
		heightRightForClose = ((nClose+3)/4+1)*maximumSizeY;
		heightLeftForClose = (nClose/4)*maximumSizeY;

		widthRight = (widthRightForClose>widthRightForRange)?widthRightForClose:widthRightForRange;
		widthLeft = (widthLeftForClose>widthLeftForRange)?widthLeftForClose:widthLeftForRange;
		heightRight = (heightRightForClose>heightRightForRange)?heightRightForClose:(heightRightForRange);
		heightLeft = (heightLeftForClose>heightLeftForRange)?heightLeftForClose:(heightLeftForRange);

		if((diff=locX+widthRight+1-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-widthLeft-maximumSizeX-1) < 0)
			locX-=diff;
		if((diff=locY+heightRight+1-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-heightLeft-maximumSizeY-1) < 0)
			locY-=diff;

		check = true;
		if(generalIsHere && !general->can_move(locX, locY))
			check = false;

		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;
		ic = 0;

		while(check && ic < nClose)
		{
			if((ic+1) % 2)
			{
				if((ic+1)%4 == 3)
					tempOldX1 -= maximumSizeX;
				else
					tempOldY1 += maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldX2 += maximumSizeX;
				else
					tempOldY2 -= maximumSizeY;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			if(!closeAttackUnits[ic]->can_move(temperoryX, temperoryY))
				check = false;
			ic++;
		}

		tempOldX1 = locX+maximumSizeX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX+2*maximumSizeX;
		tempOldY2 = locY;
		ir = 0;

		//check whether the locations for forming formation are occupied
		while(check && ir < nRange)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				tempOldY1 -= maximumSizeY;
				temperoryY = tempOldY1;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
				tempOldY2 += maximumSizeY;
			}
			if(!rangeAttackUnits[ir]->can_move(temperoryX, temperoryY))
				check = false;
			ir++;
		}

		if(check || direction == LEADER_AT_BOTTOM_RIGHT)
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
	}
	if(!check && !direction)
	{
		for(t=1; t<=nSelected; t++)
		{
			unitPtr = get_unit(t);
			for (i = 0, tempLocX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, tempLocX++)				
				for (j = 0, tempLocY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, tempLocY++)					
					world.get_loc(tempLocX, tempLocY)->set_unit(unitPtr->sprite_recno, mobile_type);				
		}
		if(closeAttackUnits)
			mem_del(closeAttackUnits);
		if(rangeAttackUnits)
			mem_del(rangeAttackUnits);
		return;
	}
	
	formation_mode = FORMATION_SANDWICH;
	if(!general->team_info->formation_direction)
		general->team_info->formation_direction = finalDirection;

	for(t=1; t<=nSelected; t++)
	{
		unitPtr = get_unit(t);
		unitPtr->set_in_formation();
	
		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(unitPtr->sprite_recno, mobile_type);				
	}

	if(finalDirection == LEADER_AT_TOP)
	{
		if(generalIsHere)
			general->move(locX, locY);
		tempOldX1 = locX+general->obj_loc_width();
		tempOldX2 = locX;
		for(ic = 0; ic < nClose; ic++)
		{
			if(ic%2)
			{
				tempOldX2 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
			}
			temperoryY = locY-maximumSizeY*(((ic+2)/2)%2);
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
			closeAttackUnits[ic]->set_in_formation();
		}
		tempOldX1 = locX;
		tempOldX2 = locX;
		for(ir = 0; ir < nRange; ir++)
		{
			if(ir%2)
			{
				tempOldX2 -= rangeAttackUnits[ir]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
			}
			temperoryY = locY+maximumSizeY;
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			rangeAttackUnits[ir]->set_in_formation();
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM)
	{
		if(generalIsHere)
			general->move(locX, locY);
		tempOldX1 = locX+general->obj_loc_width();
		tempOldX2 = locX;
		for(ic = 0; ic < nClose; ic++)
		{
			if(ic%2)
			{
				tempOldX2 -= closeAttackUnits[ic]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += closeAttackUnits[ic]->obj_loc_width();
			}
			temperoryY = locY+maximumSizeY*(((ic+2)/2)%2);
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
			closeAttackUnits[ic]->set_in_formation();
		}
		tempOldX1 = locX;
		tempOldX2 = locX;
		for(ir = 0; ir < nRange; ir++)
		{
			if(ir%2)
			{
				tempOldX2 -= rangeAttackUnits[ir]->obj_loc_width();
				temperoryX = tempOldX2;
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += rangeAttackUnits[ir]->obj_loc_width();
			}
			temperoryY = locY-maximumSizeY;
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			rangeAttackUnits[ir]->set_in_formation();
		}
	}
	else if(finalDirection == LEADER_AT_LEFT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		tempOldY1 = locY+general->obj_loc_height();
		tempOldY2 = locY;
		for(ic = 0; ic < nClose; ic++)
		{
			if(ic%2)
			{
				tempOldY2 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			temperoryX = locX-maximumSizeX*(((ic+2)/2)%2);
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
			closeAttackUnits[ic]->set_in_formation();
		}
		tempOldY1 = locY;
		tempOldY2 = locY;
		for(ir = 0; ir < nRange; ir++)
		{
			if(ir%2)
			{
				tempOldY2 -= rangeAttackUnits[ir]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
			}
			temperoryX = locX+maximumSizeX;
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			rangeAttackUnits[ir]->set_in_formation();
		}
	}
	else if(finalDirection == LEADER_AT_RIGHT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		tempOldY1 = locY+general->obj_loc_height();
		tempOldY2 = locY;
		for(ic = 0; ic < nClose; ic++)
		{
			if(ic%2)
			{
				tempOldY2 -= closeAttackUnits[ic]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += closeAttackUnits[ic]->obj_loc_height();
			}
			temperoryX = locX+maximumSizeX*(((ic+2)/2)%2);
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
			closeAttackUnits[ic]->set_in_formation();
		}
		tempOldY1 = locY;
		tempOldY2 = locY;
		for(ir = 0; ir < nRange; ir++)
		{
			if(ir%2)
			{
				tempOldY2 -= rangeAttackUnits[ir]->obj_loc_height();
				temperoryY = tempOldY2;
			}
			else
			{
				temperoryY = tempOldY1;
				tempOldY1 += rangeAttackUnits[ir]->obj_loc_height();
			}
			temperoryX = locX-maximumSizeX;
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);
			rangeAttackUnits[ir]->set_in_formation();
		}
	}
	else if(finalDirection == LEADER_AT_TOP_RIGHT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		tempOldX1 = locX;
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locY;

		for(ic=0; ic<nClose; ic++)
		{
			if((ic+1)%2)
			{
				if((ic+1)%4==3)
					tempOldY1+=maximumSizeY;
				else
					tempOldX1+=maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldY2-=maximumSizeY;
				else
					tempOldX2-=maximumSizeX;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}
		tempOldX1 = locX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX;
		tempOldY2 = locY+maximumSizeY;
		
		for(ir=0; ir<nRange; ir++)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				temperoryY = tempOldY1;
				tempOldY1 += maximumSizeY;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				tempOldY2 -= maximumSizeY;
				temperoryY = tempOldY2;
			}
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);
		}
	}
	else if(finalDirection == LEADER_AT_TOP_LEFT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		
		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;
		
		for(ic=0; ic<nClose; ic++)
		{
			if((ic+1) % 2)
			{
				if((ic+1) %4 == 3)
					tempOldY1 += maximumSizeY;
				else
					tempOldX1 -= maximumSizeX;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldY2 -= maximumSizeY;
				else
					tempOldX2 += maximumSizeX;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}

		tempOldX1 = locX+maximumSizeX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX+maximumSizeX;
		tempOldY2 = locY+maximumSizeY;
		
		//check whether the locations for forming formation are occupied
		for(ir=0; ir<nRange; ir++)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				tempOldY1 -= maximumSizeY;
				temperoryY = tempOldY1;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
				tempOldY2 += maximumSizeY;
			}
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);	
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM_LEFT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		
		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;
		for(ic=0; ic<nClose; ic++)
		{
			if((ic+1)%2)
			{
				if((ic+1)%4==3)
					tempOldX1+=maximumSizeX;
				else
					tempOldY1+=maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldX2-=maximumSizeX;
				else
					tempOldY2-=maximumSizeY;

				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}

		tempOldX1 = locX;
		tempOldY1 = locY-maximumSizeY;
		tempOldX2 = locX;
		tempOldY2 = locY-maximumSizeY;
		
		for(ir = 0; ir<nRange; ir++)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				temperoryY = tempOldY1;
				tempOldY1 += maximumSizeY;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				tempOldY2 -= maximumSizeY;
				temperoryY = tempOldY2;
			}
			//to fix those irritating bugs !!
			temperoryX = max(0, temperoryX);
			temperoryX = min(temperoryX, MAX_WORLD_X_LOC -1);
			temperoryY = max(0, temperoryY);
			temperoryY = min(temperoryY, MAX_WORLD_Y_LOC -1);
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);	
		}
	}
	else if(finalDirection == LEADER_AT_BOTTOM_RIGHT)
	{
		if(generalIsHere)
			general->move(locX, locY);
		
		tempOldX1 = locX;
		tempOldX2 = locX;
		tempOldY1 = locY;
		tempOldY2 = locY;
		for(ic=0; ic<nClose; ic++)
		{
			if((ic+1) % 2)
			{
				if((ic+1)%4 == 3)
					tempOldX1 -= maximumSizeX;
				else
					tempOldY1 += maximumSizeY;
				
				temperoryX = tempOldX1;
				temperoryY = tempOldY1;
			}
			else
			{
				if((ic+1)%4)
					tempOldX2 += maximumSizeX;
				else
					tempOldY2 -= maximumSizeY;
				
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
			}
			closeAttackUnits[ic]->move(temperoryX, temperoryY);
		}

		tempOldX1 = locX-maximumSizeX;
		tempOldY1 = locY+maximumSizeY;
		tempOldX2 = locX-maximumSizeX;
		tempOldY2 = locY+maximumSizeY;
		
		//check whether the locations for forming formation are occupied
		for(ir=0; ir<nRange; ir++)
		{
			if((ir+1)%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += maximumSizeX;
				tempOldY1 -= maximumSizeY;
				temperoryY = tempOldY1;
			}
			else
			{
				tempOldX2 -= maximumSizeX;
				temperoryX = tempOldX2;
				temperoryY = tempOldY2;
				tempOldY2 += maximumSizeY;
			}
			//to fix those irritating bugs !
			temperoryX = max(0, temperoryX);
			temperoryX = min(temperoryX, MAX_WORLD_X_LOC -1);
			temperoryY = max(0, temperoryY);
			temperoryY = min(temperoryY, MAX_WORLD_Y_LOC -1);
			rangeAttackUnits[ir]->move(temperoryX, temperoryY);	
		}
	}
	else 
		err_here();

	if(closeAttackUnits)
		mem_del(closeAttackUnits);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
}
