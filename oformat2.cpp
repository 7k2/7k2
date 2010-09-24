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
	int* sizeArrayX=0, *sizeArrayY=0;
	Unit* unitPtr;
	int finalDirection=0;
//--------------------------------------------//

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
	maxLocX = 0;
	minLocX = MAX_WORLD_X_LOC;
	maxLocY = 0;
	minLocY = MAX_WORLD_Y_LOC;
	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
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

	bool xSpanWithPositiveY = false;
	bool xSpanWithNegativeY = false;
	bool ySpanWithPositiveX = false;
	bool ySpanWithNegativeX = false;
	
	bool check = false;
	int theSecondTemperoryX, theSecondTemperoryY;
	int tempOldX1, tempOldX2, tempOldY1, tempOldY2;
	
	if(!direction || direction == LEADER_AT_TOP)
	{	
		locX = centerX;
		locY = centerY-trueWidth/2;
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft) < 0)
			locX-=diff;
		if((diff=locY+trueWidth-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if(locY < 0)
			locY = 0;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX + sizeArrayX[0];
		tempOldY1 = locY + sizeArrayY[0];
		tempOldX2 = locX;
		tempOldY2 = locY + sizeArrayY[0];
		//--check whether the locations for forming formation are occupied--//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (sizeArrayX[tSelected]);
				temperoryY = tempOldY1;
				tempOldY1 += (sizeArrayY[tSelected]);
			}
			else
			{
				temperoryX = (tempOldX2=tempOldX2-sizeArrayX[tSelected]);
				temperoryY = tempOldY2;
				tempOldY2 += (sizeArrayY[tSelected]);
			}
				
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}
		//--if the locations are not occupied-----//
		if (check || direction == LEADER_AT_TOP) 
		{
			xSpanWithPositiveY = true;
			finalDirection = LEADER_AT_TOP;
		}
	}
	if((!direction && !check) || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY+trueWidth/2;
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft) < 0)
			locX-=diff;
		if((diff=locY-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff = locY-trueWidth) < 0)
			locY-=diff;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX + sizeArrayX[0];
		tempOldY1 = locY;
		tempOldX2 = locX;
		tempOldY2 = locY;

		//----check whether the locations for forming formation are occupied----//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (sizeArrayX[tSelected]);
				temperoryY = (tempOldY1 = tempOldY1-sizeArrayY[tSelected]);
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-sizeArrayX[tSelected]);
				temperoryY = (tempOldY2 = tempOldY2-sizeArrayY[tSelected]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}
		//---if these locations are not occupied----//
		if (check || direction==LEADER_AT_BOTTOM) 
		{
			xSpanWithNegativeY = true;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		locX = centerX+trueLength/2;
		locY = centerY;
		if((diff=locX-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-trueLength) < 0)
			locX-=diff;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft) < 0)
			locY-=diff;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX;
		tempOldY1 = locY+sizeArrayY[0];
		tempOldX2 = locX;
		tempOldY2 = locY;

		//check whether the locations for forming formation are occupied//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = (tempOldX1 = tempOldX1-sizeArrayX[tSelected]);
				temperoryY = tempOldY1;
				tempOldY1 +=  (sizeArrayY[tSelected]);
			}
			else
			{
				temperoryX = (tempOldX2 = tempOldX2-sizeArrayX[tSelected]);
				temperoryY = (tempOldY2 = tempOldY2-sizeArrayY[tSelected]);
			}

			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}					
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_RIGHT) 
		{
			ySpanWithNegativeX = true;
			finalDirection = LEADER_AT_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		locX = centerX-trueLength/2;
		locY = centerY;
		if((diff=locX+trueLength-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if(locX < 0)
			locX=0;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft)<0)
			locY -= diff;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX + sizeArrayX[0];
		tempOldY1 = locY + sizeArrayY[0];
		tempOldX2 = locX + sizeArrayX[0];
		tempOldY2 = locY;
		//---check whether the locations for forming formation are occupied---//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += (sizeArrayX[tSelected]);
				temperoryY = tempOldY1;
				tempOldY1 += (sizeArrayY[tSelected]);
			}
			else
			{
				temperoryX = tempOldX2;
				tempOldX2 += (sizeArrayX[tSelected]);
				temperoryY = (tempOldY2 = tempOldY2-sizeArrayY[tSelected]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}							
		//---if these locations are occupied----//
		if (check || direction == LEADER_AT_LEFT) 
		{
			ySpanWithPositiveX = true;
			finalDirection = LEADER_AT_LEFT;
		}
	}

	bool slantPosXPosY = false;
	bool slantNegXPosY = false;
	bool slantPosXNegY = false;
	bool slantNegXNegY = false;

	if((!direction && !check) || direction == LEADER_AT_TOP_LEFT)
	{
		locX = centerX-lengthLeft/2;
		locY = centerY-widthRight/2;
		if((diff=locX+lengthLeft-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if(locX < 0)
			locX = 0;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if(locY < 0)
			locY = 0;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX+sizeArrayX[0];
		tempOldY1 = locY+sizeArrayY[0];
		
		//check whether the locations for forming formation are occupied//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				temperoryX = tempOldX1;
				tempOldX1 += sizeArrayX[tSelected];
				temperoryY = locY;
			}

			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}					
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_TOP_LEFT) 
		{
			slantPosXPosY = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_TOP_RIGHT)
	{
		locX = centerX+(lengthLeft+sizeArrayX[0])/2;
		locY = centerY-widthRight/2;
		if((diff = locX-MAX_WORLD_X_LOC) >0)
			locX-=diff;
		if((diff = locX-lengthLeft) < 0)
			locX-=diff;
		if((diff = locY+widthRight-MAX_WORLD_Y_LOC) >0)
			locY-=diff;
		if(locY<0)
			locY = 0;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX;
		tempOldY1 = locY+sizeArrayY[0];
		
		//check whether the locations for forming formation are occupied//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = locX;
				temperoryY = tempOldY1;
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldX1 -= sizeArrayX[tSelected];
				temperoryX = tempOldX1;
				temperoryY = locY;
			}

			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}					
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_TOP_RIGHT) 
		{
			slantNegXPosY = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX-lengthRight/2;
		locY = centerY+(widthLeft+sizeArrayY[0])/2;
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if(locX < 0)
			locX =0;
		if((diff = locY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-widthLeft) < 0)
			locY-=diff;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX+sizeArrayX[0];
		tempOldY1 = locY;
		
		//check whether the locations for forming formation are occupied//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				temperoryX = tempOldX1;
				tempOldX1 += sizeArrayX[tSelected];
				temperoryY = locY;
			}
			else
			{
				temperoryX = locX;
				tempOldY1 -= sizeArrayY[tSelected];
				temperoryY = tempOldY1;
			}

			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}					
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_BOTTOM_LEFT) 
		{
			slantPosXNegY = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}
	
	if((!direction && !check) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX+lengthRight/2;
		locY = centerY+(widthLeft+sizeArrayY[0])/2;
		if((diff=locX-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locX-lengthRight) <0)
			locX-=diff;
		if((diff=locY-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-lengthLeft) < 0)
			locY-=diff;

		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		tempOldX1 = locX;
		tempOldY1 = locY;
		
		//check whether the locations for forming formation are occupied//
		while(check && tSelected < nSelected)
		{
			if(tSelected%2)
			{
				tempOldX1 -= sizeArrayX[tSelected];
				temperoryX = tempOldX1;
				temperoryY = locY;
			}
			else
			{
				temperoryX = locX;
				tempOldY1 -= sizeArrayY[tSelected];
				temperoryY = tempOldY1;
			}

			for (i = 0, theSecondTemperoryX = temperoryX; i < sizeArrayX[tSelected]; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}					
		//---if these locations are not occupied--//
		if (check || direction == LEADER_AT_BOTTOM_RIGHT) 
		{
			slantNegXNegY = true;
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
		if(sizeArrayX)
			mem_del(sizeArrayX);
		if(sizeArrayY)
			mem_del(sizeArrayY);
		if(sortedArray)
			mem_del(sortedArray);

		return;
	}

	formation_mode = FORMATION_VANGUARD;

	tSelected = 0;
	
	if(ySpanWithNegativeX)
		tempOldX1 = locX;
	else
		tempOldX1 = locX + sizeArrayX[0];
	if(ySpanWithPositiveX)
		tempOldX2 = locX;
	else
		tempOldX2 = locX + sizeArrayX[0];
	if(xSpanWithNegativeY)
		tempOldY1 = locY;
	else
		tempOldY1 = locY + sizeArrayY[0];
	if(xSpanWithPositiveY)
		tempOldY2 = locY;
	else
		tempOldY2 = locY + sizeArrayY[0];
	if(slantPosXPosY)
	{
		tempOldX1 = locX;
		tempOldY1 = locY+sizeArrayY[0]; 
	}
	else if(slantNegXPosY)
	{
		tempOldX1 = locX+sizeArrayX[0];
		tempOldY1 = locY+sizeArrayY[0];
	}
	else if(slantPosXNegY)
	{
		tempOldX1 = locX+sizeArrayX[0];
		tempOldY1 = locY+sizeArrayY[0];
	}
	else if(slantNegXNegY)
	{
		tempOldX1 = locX;
		tempOldY1 = locY+sizeArrayY[0];
	}

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = sortedArray[tSelected]->next_x_loc(); i < sizeArrayX[tSelected]; i++, tempLocX++)				
			for (j = 0, tempLocY = sortedArray[tSelected]->next_y_loc(); j < sizeArrayY[tSelected]; j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(xSpanWithPositiveY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
				tempOldX1 += (sizeArrayX[tSelected]);
				tempOldY1 += (sizeArrayY[tSelected]);
			}
			else
			{
				tempOldX2 -= (sizeArrayX[tSelected]);
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
				tempOldY2 += (sizeArrayY[tSelected]);
			}
		}
		else if(xSpanWithNegativeY)
		{
			if(tSelected%2)
			{
				tempOldY1 -= (sizeArrayY[tSelected]);
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
				tempOldX1 += (sizeArrayX[tSelected]);
			}
			else
			{
				tempOldX2 -= (sizeArrayX[tSelected]);
				tempOldY2 -= (sizeArrayY[tSelected]);
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}
		}
		else if(ySpanWithNegativeX)
		{
			if(tSelected%2)
			{
				tempOldX1 -= (sizeArrayX[tSelected]);
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
				tempOldY1 += (sizeArrayY[tSelected]);
			}
			else
			{
				tempOldX2 -= (sizeArrayX[tSelected]);
				tempOldY2 -= (sizeArrayY[tSelected]);
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
			}
		}
		else if(ySpanWithPositiveX)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(tempOldX1, tempOldY1);
				tempOldX1 += (sizeArrayX[tSelected]);
				tempOldY1 += (sizeArrayY[tSelected]);
			}
			else
			{
				tempOldY2 -= (sizeArrayY[tSelected]);
				sortedArray[tSelected]->move(tempOldX2, tempOldY2);
				tempOldX2 += (sizeArrayX[tSelected]);
			}
		}
		else if(slantPosXPosY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(locX, tempOldY1);
				tempOldY1+=sizeArrayY[tSelected];
			}
			else
			{
				sortedArray[tSelected]->move(tempOldX1, locY);
				tempOldX1 += sizeArrayX[tSelected];
			}
		}
		else if(slantNegXPosY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(locX, tempOldY1);
				tempOldY1 += sizeArrayY[tSelected];
			}
			else
			{
				tempOldX1 -= sizeArrayX[tSelected];
				sortedArray[tSelected]->move(tempOldX1, locY);
			}
		}
		else if(slantPosXNegY)
		{
			if(tSelected%2)
			{
				sortedArray[tSelected]->move(tempOldX1, locY);
				tempOldX1 += sizeArrayX[tSelected];
			}
			else
			{
				tempOldY1 -= sizeArrayY[tSelected];
				sortedArray[tSelected]->move(locX, tempOldY1);
			}
		}
		else if(slantNegXNegY)
		{
			if(tSelected%2)
			{
				tempOldX1 -= sizeArrayX[tSelected];
				sortedArray[tSelected]->move(tempOldX1, locY);
			}
			else
			{
				tempOldY1 -= sizeArrayY[tSelected];
				sortedArray[tSelected]->move(locX, tempOldY1);
			}
		}

		sortedArray[tSelected]->set_in_formation();
	}	
		
	if(sizeArrayX)
		mem_del(sizeArrayX);
	if(sizeArrayY)
		mem_del(sizeArrayY);
	if(sortedArray)
		mem_del(sortedArray);
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
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
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
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft) < 0)
			locX-=diff;
		if((diff=locY-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if(locY < 0)
			locY = 0;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			
			tSelected++;
		}
		if(check)
		{
			leaderAtMiddleWithSpanAlongXOK = true;
			finalDirection = LEADER_AT_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY;
		if((diff=locX-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if(locX < 0)
			locX=0;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
	
			tSelected++;
		}
		if(check)
		{
			leaderAtMiddleWithSpanAlongYOK = true;
			finalDirection = LEADER_AT_TOP;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_TOP_LEFT || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		locX = centerX;
		locY = centerY;
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft) < 0)
			locX-=diff;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these locations are not occupied//
		if(check)
		{
			leaderAtMiddleWithSpanAlongPositiveXAndPositiveY = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_TOP_RIGHT || direction == LEADER_AT_BOTTOM_LEFT)
	{
		locX = centerX;
		locY = centerY+1;
		if((diff=locX+lengthRight-MAX_WORLD_X_LOC)>0)
			locX-=diff;
		if((diff=locX-lengthLeft) < 0)
			locX-=diff;
		if((diff=locY+widthRight-MAX_WORLD_Y_LOC) > 0)
			locY-=diff;
		if((diff=locY-widthLeft) < 0)
			locY-=diff;
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sizeArrayX[0]; i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sizeArrayY[0]; j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these locations are not occupied//
		if(check)
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
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int xMultiplicativeFactor, yMultiplicativeFactor;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	float* sineArray=NULL, *cosineArray=NULL;
	int ind;
	float angle;
	bool xDirection, yDirection;
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
	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
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

	centerX = (maxLocX+minLocX)/2;
	centerY = (maxLocY+minLocY)/2;

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

	int diff;
	
	//Define the radius
	if(nSelected<4)
		radius = maximumSizeY/2+2;
	else
		radius = ((nSelected-1)/5+2)*maximumSizeY; // this should be a jolly good radius

	//write the sine and cosine values into an array//
	angle = 6.2832f/((float)nSelected);  // the magic number is 2 pi
	sineArray = (float*) mem_add(nSelected*sizeof(float));
	cosineArray = (float*) mem_add(nSelected*sizeof(float));
	for(ind = 0; ind<nSelected; ind++)
	{
		sineArray[ind] = (float) (radius*sin(ind*angle));
		cosineArray[ind] = (float) (radius*cos(ind*angle));
	}
	//check orientation
	bool check = false;
	xDirection = false;
	yDirection = false;
	int theSecondTemperoryX, theSecondTemperoryY;

	//Determine the orientation of the formation
	
	//right of the leader is ok
	if(!direction || direction == LEADER_AT_LEFT)
	{
		locX = centerX-radius;
		locY = centerY;
		
		if((diff=centerX+radius+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			centerX-=diff;
			locX = centerX-radius;
		}
		if(locX < 0)
		{
			locX=0;
			centerX = radius;
		}
		if((diff=centerY+radius+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			centerY-=diff;
			locY = centerY;
		}
		if((diff=centerY-radius) < 0)
		{
			centerY-=diff;
			locY = centerY;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < get_unit(1)->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < get_unit(1)->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(centerX-cosineArray[tSelected]);
			temperoryY = (int)(centerY+sineArray[tSelected]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < get_unit(tSelected+1)->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < get_unit(tSelected+1)->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these location are not occpied//
		if(check)
		{
			xMultiplicativeFactor = -1;
			yMultiplicativeFactor = 1;
			xDirection = true;
		}
	}

	//left of the leader is ok
	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		locX = centerX+radius;
		locY = centerY;

		if((diff=centerX+radius+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			centerX-=diff;
			locX = centerX+radius;
		}
		if((diff=centerX-radius) < 0)
		{
			centerX-=diff;
			locX = centerX+radius;
		}
		if((diff=centerY+radius+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			centerY-=diff;
			locY = centerY;
		}
		if((diff=centerY-radius) < 0)
		{
			centerY-=diff;
			locY = centerY;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < get_unit(1)->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < get_unit(1)->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = (int)(centerX+cosineArray[tSelected]);
			temperoryY = (int)(centerY+sineArray[tSelected]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < get_unit(tSelected+1)->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < get_unit(tSelected+1)->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these location are not occupied//
		if(check)
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			xDirection = true;
		}
	}
	//upper of the leader is ok//
	if((!direction && !check) || direction == LEADER_AT_BOTTOM)
	{	
		locX = centerX;
		locY = centerY+radius;

		if((diff=centerX+radius+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			centerX-=diff;
			locX = centerX;
		}
		if((diff=centerX-radius) <0)
		{
			centerX-=diff;
			locX=centerX;
		}
		if((diff=centerY+radius+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			centerY-=diff;
			locY = centerY+radius;
		}
		if((diff=centerY-radius) < 0)
		{
			centerY-=diff;
			locY = centerY+radius;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < get_unit(1)->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < get_unit(1)->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
	
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(centerX+sineArray[tSelected]);
			temperoryY = (int)(centerY+cosineArray[tSelected]);
			for (i = 0, theSecondTemperoryX = get_unit(tSelected+1)->obj_loc_width(); i < maximumSizeX; i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < get_unit(tSelected+1)->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these locations are not occupied//
		if(check)
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = 1;
			yDirection = true;
		}
	}
	//lower of the leader is ok
	if((!direction && !check) || direction == LEADER_AT_TOP)
	{
		locX = centerX;
		locY = centerY-radius;

		if((diff=centerX+radius+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			centerX-=diff;
			locX = centerX;
		}
		if((diff=centerX-radius) <0)
		{
			centerX-=diff;
			locX=centerX;
		}
		if((diff=centerY+radius+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			centerY-=diff;
			locY = centerY-radius;
		}
		if((diff=centerY-radius) < 0)
		{
			centerY-=diff;
			locY = centerY-radius;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < get_unit(1)->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < get_unit(1)->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(centerX+sineArray[tSelected]);
			temperoryY = (int)(centerY-cosineArray[tSelected]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < get_unit(tSelected+1)->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < get_unit(tSelected+1)->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
		}
		//if these loction are not occupied//
		if(check)
		{
			xMultiplicativeFactor = 1;
			yMultiplicativeFactor = -1;
			yDirection = true;
		}
	}
	if(!check)
	{
		if(sineArray)
			mem_del(sineArray);
		if(cosineArray)
			mem_del(cosineArray);
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

	formation_mode = FORMATION_CIRCLE;

	for(tSelected = 0; tSelected<nSelected; tSelected++)
	{
		unitPtr=get_unit(tSelected+1);
		for (i = 0, temperoryLocationForInDirectionX = unitPtr->next_x_loc(); i < unitPtr->obj_loc_width(); i++, temperoryLocationForInDirectionX++)				
			for (j = 0, temperoryLocationForInDirectionY = unitPtr->next_y_loc(); j < unitPtr->obj_loc_height(); j++, temperoryLocationForInDirectionY++)					
				world.get_loc(temperoryLocationForInDirectionX, temperoryLocationForInDirectionY)->set_unit(unitPtr->sprite_recno, mobile_type);				
	}

	for(tSelected = 0; tSelected<nSelected; tSelected++)
	{
		unitPtr = get_unit(tSelected+1);

		//set the unit to move to the location for forming formation//
		if(xDirection)
			unitPtr->move((int)(centerX+xMultiplicativeFactor*cosineArray[tSelected]), 
				(int)(centerY+yMultiplicativeFactor*sineArray[tSelected]));
		
		else if(yDirection)
			unitPtr->move((int)(centerX+xMultiplicativeFactor*sineArray[tSelected]), 
				(int)(centerY+yMultiplicativeFactor*cosineArray[tSelected]));
	
		//set the occupancy of the location originally occupied by the troop to be true//
		unitPtr->set_in_formation();
	}
			
	if(sineArray)
		mem_del(sineArray);
	if(cosineArray)
		mem_del(cosineArray);
	if(sortedArray)
		mem_del(sortedArray);
}
//--- End of function UnitGroup::circle_formation ---//

//--- Begin of function UnitGroup::arc_formation ---//
void UnitGroup::arc_formation(int direction)
{
	//--------------Define local variables------------//
	int radius, centerX, centerY, circleCenterX, circleCenterY;
	int nSelected = 0; 
	int locX, locY, i, j, t, temperoryLocationForInDirectionX, temperoryLocationForInDirectionY, tSelected =0;
	int temperoryX, temperoryY;
	int tempLocX, tempLocY;
	int maximumSizeX, maximumSizeY, temperorySizeX, temperorySizeY;
	float* sineArray=NULL, *cosineArray=NULL;
	float* sineAdd45Array=NULL, *cosineAdd45Array=NULL;
	float* sineSub45Array=NULL, *cosineSub45Array=NULL;
	int ind;
	float angle;
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
	int maxLocX = 0;
	int minLocX = MAX_WORLD_X_LOC;
	int maxLocY = 0;
	int minLocY = MAX_WORLD_Y_LOC;
	Unit** sortedArray = (Unit**) mem_add(nSelected*sizeof(Unit*));
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
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

	centerX = (maxLocX+minLocX)/2;
	centerY = (maxLocY+minLocY)/2;

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
	
	//Define the radius
	if(nSelected<4)
		radius = 3*(maximumSizeY/2+1);
	else if(nSelected < 8)
		radius = ((nSelected-1)/2+2)*maximumSizeY;
	else
		radius = ((nSelected-1)/2+4)*maximumSizeY; 
		
	int halfOfNumberSelected = nSelected/2;
	int arraySize = halfOfNumberSelected+1;
	//leader is at the lower vertex;
	//write the sine and cosine values into an array//
	angle = 2.5f/((float)nSelected);  //2.5 means (2 *pi*2)/5
	const float piOverFour = (float)(3.14/4.);
	const float sinPiOverTenTimesRadius = (float)(radius*sin(0.314));
	const float sinPiOverFour = (float)(sin(piOverFour));
	sineArray = (float*) mem_add_clear(arraySize*sizeof(float));
	cosineArray = (float*) mem_add_clear(arraySize*sizeof(float));
	sineAdd45Array = (float*)mem_add_clear(arraySize*sizeof(float));
	cosineAdd45Array = (float*)mem_add_clear(arraySize*sizeof(float));
	sineSub45Array = (float*)mem_add_clear(arraySize*sizeof(float));
	cosineSub45Array = (float*)mem_add_clear(arraySize*sizeof(float));

	for(ind = 0; ind<arraySize; ind++)
	{
		sineArray[ind] = (float)(radius*sin(ind*angle));
		cosineArray[ind] = (float) (radius*cos(ind*angle));
		sineAdd45Array[ind] = (float) (radius*sin(ind*angle+piOverFour));
		cosineAdd45Array[ind] = (float) (radius*cos(ind*angle+piOverFour));
		sineSub45Array[ind] = (float) (radius*sin(ind*angle-piOverFour));
		cosineSub45Array[ind] = (float) (radius*cos(ind*angle-piOverFour));
	}
	//check orientation
	int arrayIndex;
	int theSign;
	int theSecondTemperoryX, theSecondTemperoryY;
	bool check = false;
	bool slantPosYPosX = false;
	bool slantPosYNegX = false;
	bool slantNegYPosX = false;
	bool slantNegYNegX = false;

	xDirection = false;
	yDirection = false;
		
	int diff; 

	if(!direction || direction == LEADER_AT_BOTTOM) 
	{
		circleCenterX = centerX;
		circleCenterY = (int)((float)centerY-((float)radius)*0.5f-sinPiOverTenTimesRadius*0.5f)+1;
		
		locX = circleCenterX;
		locY = circleCenterY+radius;
		
		if((diff=(int)(circleCenterX+sineArray[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC))>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
		}
		if((diff=(int)(circleCenterX-sineArray[halfOfNumberSelected])) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
		}
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			locY-=diff;
			circleCenterY=locY-radius;
		}
		if((diff=(int)(circleCenterY+cosineArray[halfOfNumberSelected])) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+radius;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
		
		theSign = 1;
		arrayIndex = 1;
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(circleCenterX+theSign*sineArray[arrayIndex]);
			temperoryY = (int)(circleCenterY+cosineArray[arrayIndex]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		//if these location are not occpied//
		if(check)
		{
			yDirection = true;
			yMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_BOTTOM;
		}
	}
	if((!check && !direction) || direction == LEADER_AT_BOTTOM_RIGHT)
	{
		circleCenterX = (int)((float)centerX-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		circleCenterY = (int)((float)centerY-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		
		locX = circleCenterX+(int)sineAdd45Array[0];		
		locY = circleCenterY+(int)cosineAdd45Array[0];

		if((diff=circleCenterX+(int)sineAdd45Array[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45Array[0];
		}
		if((diff=circleCenterX-(int)sineSub45Array[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45Array[0];
		}
		if((diff=circleCenterY+(int)cosineSub45Array[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineAdd45Array[0];
		}
		if((diff=circleCenterY+(int)cosineAdd45Array[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineAdd45Array[0];
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
	
		theSign = 1;
		arrayIndex = 1;
		
		while(check && tSelected < nSelected)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineAdd45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineAdd45Array[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineSub45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineSub45Array[arrayIndex]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(check)
		{
			slantPosYPosX = true;
			finalDirection = LEADER_AT_BOTTOM_RIGHT;
		}
	}
	
	if((!check && !direction) || direction == LEADER_AT_BOTTOM_LEFT)
	{
		circleCenterX = (int)((float)centerX+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
		circleCenterY = (int)((float)centerY-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		
		locX = circleCenterX+(int)sineSub45Array[0];		
		locY = circleCenterY+(int)cosineSub45Array[0];

		if((diff=circleCenterX+(int)sineSub45Array[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45Array[0];
		}
		if((diff=circleCenterX-(int)sineAdd45Array[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45Array[0];
		}
		if((diff=circleCenterY+(int)cosineSub45Array[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineSub45Array[0];
		}
		if((diff=circleCenterY+(int)cosineAdd45Array[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY+(int)cosineSub45Array[0];
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
		
		theSign = 1;
		arrayIndex = 1;

		while(check && tSelected < nSelected)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineSub45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineSub45Array[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineAdd45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY+cosineAdd45Array[arrayIndex]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(check)
		{
			slantPosYNegX = true;
			finalDirection = LEADER_AT_BOTTOM_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_TOP)
	{
		circleCenterX = centerX;
		circleCenterY = (int)((float)centerY+((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)+1;
		
		locX = circleCenterX;
		locY = circleCenterY-radius;
		
		if((diff=circleCenterX+(int)sineArray[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
		}
		if((diff=circleCenterX-(int)sineArray[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX;
		}
		if((diff=circleCenterY-(int)cosineArray[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY=circleCenterY+radius;
		}
		if(locY < 0)
		{
			locY = 0;
			circleCenterY=locY-radius;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
		
		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(circleCenterX+theSign*sineArray[arrayIndex]);
			temperoryY = (int)(circleCenterY-cosineArray[arrayIndex]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		//if these location are not occpied//
		if(check)
		{
			yDirection = true;
			yMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_TOP;
		}
	}

	if((!check && !direction) || direction == LEADER_AT_TOP_RIGHT)
	{
		circleCenterX = (int)((float)centerX-(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)+3;
		circleCenterY = (int)((float)centerY+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
		
		locX = circleCenterX+(int)sineAdd45Array[0];		
		locY = circleCenterY-(int)cosineAdd45Array[0];

		if((diff=circleCenterX+(int)sineAdd45Array[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45Array[0];
		}
		if((diff=circleCenterX-(int)sineSub45Array[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineAdd45Array[0];
		}
		if((diff=circleCenterY-(int)cosineSub45Array[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineAdd45Array[0];
		}
		if((diff=circleCenterY-(int)cosineAdd45Array[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineAdd45Array[0];
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
	
		theSign = 1;
		arrayIndex = 1;

		while(check && tSelected < nSelected)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineAdd45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineAdd45Array[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineSub45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineSub45Array[arrayIndex]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(check)
		{
			slantNegYPosX = true;
			finalDirection = LEADER_AT_TOP_RIGHT;
		}
	}
	
	if(!check && (!direction || direction == LEADER_AT_TOP_LEFT))
	{
		circleCenterX = (int)((float)centerX+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
		circleCenterY = (int)((float)centerY+(((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)*sinPiOverFour)-1;
		
		locX = circleCenterX+(int)sineSub45Array[0];		
		locY = circleCenterY-(int)cosineSub45Array[0];

		if((diff=circleCenterX+(int)sineSub45Array[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45Array[0];
		}
		if((diff=circleCenterX-(int)sineAdd45Array[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+(int)sineSub45Array[0];
		}
		if((diff=circleCenterY-(int)cosineSub45Array[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineSub45Array[0];
		}
		if((diff=circleCenterY-(int)cosineAdd45Array[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY-(int)cosineSub45Array[0];
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		theSign = 1;
		arrayIndex = 1;

		while(check && tSelected < nSelected)
		{
			if(theSign > 0)
			{
				temperoryX = (int)(circleCenterX+sineSub45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineSub45Array[arrayIndex]);
			}
			else
			{
				temperoryX = (int)(circleCenterX-sineAdd45Array[arrayIndex]);
				temperoryY = (int)(circleCenterY-cosineAdd45Array[arrayIndex]);
			}
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		if(check)
		{
			slantNegYNegX = true;
			finalDirection = LEADER_AT_TOP_LEFT;
		}
	}

	if((!direction && !check) || direction == LEADER_AT_LEFT)
	{
		circleCenterX = (int)((float)centerX+((float)radius)*0.5f+sinPiOverTenTimesRadius*0.5f)+1;
		circleCenterY = centerY;
		
		locX = circleCenterX-radius;
		locY = circleCenterY;
		
		if((diff=circleCenterX-(int)cosineArray[halfOfNumberSelected]+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			circleCenterX-=diff;
			locX = circleCenterX-radius;
		}
		if(locX <0)
		{
			locX = 0;
			circleCenterX = locX+radius;
		}
		if((diff=locY+(int)sineArray[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
		}
		if((diff=circleCenterY-(int)sineArray[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					

		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(circleCenterX-cosineArray[arrayIndex]);
			temperoryY = (int)(circleCenterY+theSign*sineArray[arrayIndex]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		//if these location are not occpied//
		if(check)
		{
			xDirection = true;
			xMultiplicativeFactor = -1;
			finalDirection = LEADER_AT_LEFT;
		}
	}
	if((!direction && !check) || direction == LEADER_AT_RIGHT)
	{
		circleCenterX = (int)((float)centerX-((float)radius)*0.5f-sinPiOverTenTimesRadius*0.5f)+1;
		circleCenterY = centerY;
		
		locX = circleCenterX+radius;
		locY = circleCenterY;
		
		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC)>0)
		{
			locX-=diff;
			circleCenterX = locX-radius;
		}
		if((diff=circleCenterX+(int)cosineArray[halfOfNumberSelected]) <0)
		{
			circleCenterX-=diff;
			locX = circleCenterX+radius;
		}
		if((diff=locY+(int)sineArray[halfOfNumberSelected]+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
		}
		if((diff=circleCenterY-(int)sineArray[halfOfNumberSelected]) < 0)
		{
			circleCenterY-=diff;
			locY = circleCenterY;
		}
		
		//check forming possible
		tSelected = 1;
		check = true;

		for (i = 0, theSecondTemperoryX = locX; i < sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)				
			for (j = 0, theSecondTemperoryY = locY; j < sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;					
		
		theSign=1;
		arrayIndex = 1;
		
		//check whether the locations for forming formation are occupied
		while(check && tSelected<nSelected)
		{
			temperoryX = (int)(circleCenterX+cosineArray[arrayIndex]);
			temperoryY = (int)(circleCenterY+theSign*sineArray[arrayIndex]);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;
			tSelected++;
			if(theSign < 0)
				arrayIndex++;
			theSign = - theSign;
		}
		//if these location are not occpied//
		if(check)
		{
			xDirection = true;
			xMultiplicativeFactor = 1;
			finalDirection = LEADER_AT_RIGHT;
		}
	}
	if(!check)
	{
		//BUG_HERE: Hey! Anyone else would like to be the leader ? 
		if(sineArray)
			mem_del(sineArray);
		if(cosineArray)
			mem_del(cosineArray);
		if(sineAdd45Array)
			mem_del(sineAdd45Array);
		if(cosineAdd45Array)
			mem_del(cosineAdd45Array);
		if(sineSub45Array)
			mem_del(sineSub45Array);
		if(cosineSub45Array)
			mem_del(cosineSub45Array);
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
	
	formation_mode = FORMATION_ARC;
	arrayIndex = 0;
	theSign = -1;

	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		if(sortedArray[tSelected]->team_info && !sortedArray[tSelected]->team_info->formation_direction)
			sortedArray[tSelected]->team_info->formation_direction = finalDirection;

		//setting the occupancy of the lcoations originally occupied by the units to be true
		for (i = 0, tempLocX = sortedArray[tSelected]->next_x_loc(); i < sortedArray[tSelected]->obj_loc_width(); i++, tempLocX++)				
			for (j = 0, tempLocY = sortedArray[tSelected]->next_y_loc(); j < sortedArray[tSelected]->obj_loc_height(); j++, tempLocY++)					
				world.get_loc(tempLocX, tempLocY)->set_unit(sortedArray[tSelected]->sprite_recno, mobile_type);				
	}
		
	for(tSelected=0; tSelected<nSelected; tSelected++)
	{
		//set the unit to move to the location for forming formation//
		if(yDirection)
			sortedArray[tSelected]->move(((int)(circleCenterX+theSign*sineArray[arrayIndex])), 
				((int)(circleCenterY+yMultiplicativeFactor*cosineArray[arrayIndex])));
		else if(xDirection)
			sortedArray[tSelected]->move(((int)(circleCenterX+xMultiplicativeFactor*cosineArray[arrayIndex])), 
				((int)(circleCenterY+theSign*sineArray[arrayIndex])));
		else if(slantPosYPosX)
		{
			if(!tSelected)
				sortedArray[tSelected]->move((int)(circleCenterX+sineAdd45Array[0]), (int)(circleCenterY+cosineAdd45Array[0]));
			else if(theSign> 0)
				sortedArray[tSelected]->move((int)(circleCenterX+sineAdd45Array[arrayIndex]), 
					(int)(circleCenterY+cosineAdd45Array[arrayIndex]));
			else
				sortedArray[tSelected]->move((int)(circleCenterX-sineSub45Array[arrayIndex]),
					(int)(circleCenterY+cosineSub45Array[arrayIndex]));
		}
		else if(slantPosYNegX)
		{
			if(!tSelected)
				sortedArray[tSelected]->move((int)(circleCenterX+sineSub45Array[0]), (int)(circleCenterY+cosineSub45Array[0]));
			else if(theSign> 0)
				sortedArray[tSelected]->move((int)(circleCenterX+sineSub45Array[arrayIndex]), 
					(int)(circleCenterY+cosineSub45Array[arrayIndex]));
			else
				sortedArray[tSelected]->move((int)(circleCenterX-sineAdd45Array[arrayIndex]),
					(int)(circleCenterY+cosineAdd45Array[arrayIndex]));
		}
		else if(slantNegYPosX)
		{
			if(!tSelected)
				sortedArray[tSelected]->move((int)(circleCenterX+sineAdd45Array[0]), (int)(circleCenterY-cosineAdd45Array[0]));
			else if(theSign> 0)
				sortedArray[tSelected]->move((int)(circleCenterX+sineAdd45Array[arrayIndex]), 
					(int)(circleCenterY-cosineAdd45Array[arrayIndex]));
			else
				sortedArray[tSelected]->move((int)(circleCenterX-sineSub45Array[arrayIndex]),
					(int)(circleCenterY-cosineSub45Array[arrayIndex]));
		}
		else if(slantNegYNegX)
		{
			if(!tSelected)
				sortedArray[tSelected]->move((int)(circleCenterX+sineSub45Array[0]), (int)(circleCenterY-cosineSub45Array[0]));
			else if(theSign> 0)
				sortedArray[tSelected]->move((int)(circleCenterX+sineSub45Array[arrayIndex]), 
					(int)(circleCenterY-cosineSub45Array[arrayIndex]));
			else
				sortedArray[tSelected]->move((int)(circleCenterX-sineAdd45Array[arrayIndex]),
					(int)(circleCenterY-cosineAdd45Array[arrayIndex]));
		}
					
		sortedArray[tSelected]->set_in_formation();
	
		if(theSign <0)
			arrayIndex++;

		theSign = -theSign;
	}
	if(sineArray)
		mem_del(sineArray);
	if(cosineArray)
		mem_del(cosineArray);
	if(sineAdd45Array)
		mem_del(sineAdd45Array);
	if(cosineAdd45Array)
		mem_del(cosineAdd45Array);
	if(sineSub45Array)
		mem_del(sineSub45Array);
	if(cosineSub45Array)
		mem_del(cosineSub45Array);
	if(sortedArray)
		mem_del(sortedArray);
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
	Unit* general;
	
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

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;

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

	sortedArray[0] = general;
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

	int theSecondTemperoryX, theSecondTemperoryY;
	int tempOldX1, tempOldX2;
	int tempOldY1, tempOldY2;
	
	if(!direction || direction == LEADER_AT_BOTTOM)
	{
		locX = centerX;
		locY = centerY;

		if((diff=locX-widthLeft) < 0)
			locX-=diff;
		if((diff=locX+widthRight-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+2*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		locY = centerY+1;

		if((diff=locX-widthLeft) < 0)
			locX-=diff;
		if((diff=locX+widthRight-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		if((diff=locX+2*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+lengthLeft-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-lengthRight+sizeArrayY[0]) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
				
		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		locX = centerX+1;
		locY = centerY;

		if((diff=locX-maximumSizeX) < 0)
			locX -=diff;
		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX-=diff;
		if((diff=locY+lengthLeft-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-lengthRight+sizeArrayY[0]) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
				
		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+((nSelected+2)/4+1)*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locY+((nSelected-1)/4+1)*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locX-((nSelected-1)/4+1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-(nSelected/4)*maximumSizeY) < 0)
			locY -= diff;
		
		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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
			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+((nSelected+3)/4+1)*maximumSizeX-MAX_WORLD_X_LOC) >0)
			locX -= diff;
		if((diff=locY+((nSelected+1)/4+1)*maximumSizeY-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locX-(nSelected/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-((nSelected+2)/4)*maximumSizeY) <0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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
			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		
		if((diff=locX+((nSelected+2)/4+1)*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locY+((nSelected+3)/4+1)*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locX-((nSelected+1)/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-(nSelected/4)*maximumSizeY) <0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		locX = centerX+1;
		locY = centerY;

		if((diff=locX+(nSelected/4)+1)*maximumSizeX-MAX_WORLD_X_LOC > 0)
			locX -= diff;
		if((diff=locY+((nSelected+1)/4+1)*maximumSizeY)-MAX_WORLD_Y_LOC > 0)
			locY -= diff;
		if((diff=locX-((nSelected+3)/4)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY-((nSelected+2)/4)*maximumSizeY) < 0)
			locY -= diff;
		
		for(i=0, theSecondTemperoryX = locX; i<general->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<general->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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

			for(i=0, theSecondTemperoryX = temperoryX; i<sizeArrayX[tSelected]; i++, theSecondTemperoryX++)
				for(j=0, theSecondTemperoryY = temperoryY; j<sizeArrayY[tSelected]; j++, theSecondTemperoryY++)
					if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
	if (!check)
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
	Unit** sortedArrayTmp = (Unit**) mem_add((nSelected-1)*sizeof(Unit*));
	int unitIndex=0;
	Unit* general;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(!unitPtr->team_info)
			sortedArrayTmp[unitIndex++] = unitPtr;
		else
			general = unitPtr;
		err_when(unitIndex>= nSelected);
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

	int centerX = ((maxLocX+minLocX)%2)?(maxLocX+minLocX+1)/2:(maxLocX+minLocX)/2;
	int centerY = ((maxLocY+minLocY)%2)?(maxLocY+minLocY+1)/2:(maxLocY+minLocY)/2;
	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<(nSelected-1); i++)
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
		else if(i== midPosition)
			sortedArray[i] = general;
		else
			sortedArray[i] = sortedArrayTmp[i-1];
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
		
		if((diff = locX+numberOfLines*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff = locY+numberOfRows*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+numberOfLines*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff = locX+maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-(numberOfLines-1)*maximumSizeX) <0)
			locX -= diff;
		if((diff = locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+numberOfRows*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+numberOfLines*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+(numberOfLines+numberOfRows-1)*maximumSizeY-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if(locY<0)
			locY = 0;
		
		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+(numberOfLines+numberOfRows-1)*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX -= diff;
		if((diff=locY+numberOfRows*maximumSizeY-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;
		
		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
	
		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}
		if(check && direction == LEADER_AT_TOP_LEFT) 
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

		if((diff=locX+numberOfRows*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines+numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC) >0 )
			locX -= diff;
		if((diff=locX-(numberOfLines+numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff = locY+numberOfLines*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
			finalDirection == LEADER_AT_BOTTOM_RIGHT;
		}
	}
	//if all the formation mode are not allowed//
	if (!check)
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
	Unit* general;
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
	Unit** sortedArrayTmp = (Unit**) mem_add((nSelected-1)*sizeof(Unit*));
	int unitIndex=0;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(!unitPtr->team_info)
			sortedArrayTmp[unitIndex++] = unitPtr;
		else
			general = unitPtr;
		err_when(unitIndex >= nSelected);
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

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;
	//sort units by attack_range, since we want close attack units at the front
	Unit* tempUnit;
	for(i=0; i<(nSelected-1); i++)
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
		else if(i== midPosition)
			sortedArray[i] = general;
		else
			sortedArray[i] = sortedArrayTmp[i-1];
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
		
		if((diff = locX+numberOfLines*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff = locY+numberOfRows*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY =0 ;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+numberOfLines*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if(locY < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY+formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff = locX+maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff = locX-(numberOfLines-1)*maximumSizeX) <0)
			locX -= diff;
		if((diff = locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX-formationSpaceX*(tSelected%numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected/numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
		locY = centerY+(numberOfLines/2)*maximumSizeY;

		if((diff=locX+numberOfRows*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX = 0;
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
		//check whether the locations for forming formation are occupied
		while(check && tSelected < nSelected)
		{
			temperoryX = locX+formationSpaceX*(tSelected/numberOfLines);
			temperoryY = locY-formationSpaceY*(tSelected%numberOfLines);
			for (i = 0, theSecondTemperoryX = temperoryX; i < sortedArray[tSelected]->obj_loc_width(); i++, theSecondTemperoryX++)				
				for (j = 0, theSecondTemperoryY = temperoryY; j < sortedArray[tSelected]->obj_loc_height(); j++, theSecondTemperoryY++)
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+numberOfLines*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+(numberOfLines+numberOfRows-1)*maximumSizeY-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if(locY<0)
			locY = 0;
		
		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
		
		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+(numberOfLines+numberOfRows-1)*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if(locX < 0)
			locX -= diff;
		if((diff=locY+numberOfRows*maximumSizeY-MAX_WORLD_Y_LOC) >0)
			locY -= diff;
		if((diff=locY-(numberOfLines-1)*maximumSizeY) < 0)
			locY -= diff;
		
		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;
	
		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
						check = false;					
			++tSelected;
		}
		if(check && direction == LEADER_AT_TOP_LEFT) 
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

		if((diff=locX+numberOfRows*maximumSizeX-MAX_WORLD_X_LOC) > 0)
			locX -= diff;
		if((diff=locX-(numberOfLines-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff=locY+maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff=locY-(numberOfLines+numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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

		if((diff=locX+maximumSizeX-MAX_WORLD_X_LOC) >0 )
			locX -= diff;
		if((diff=locX-(numberOfLines+numberOfRows-1)*maximumSizeX) < 0)
			locX -= diff;
		if((diff = locY+numberOfLines*maximumSizeY-MAX_WORLD_Y_LOC) > 0)
			locY -= diff;
		if((diff = locY-(numberOfRows-1)*maximumSizeY) < 0)
			locY -= diff;

		for(i=0, theSecondTemperoryX = locX; i<sortedArray[0]->obj_loc_width(); i++, theSecondTemperoryX++)
			for(j=0, theSecondTemperoryY = locY; j<sortedArray[0]->obj_loc_height(); j++, theSecondTemperoryY++)
				if(!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
					check = false;

		tSelected = 1;
		check = true;
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
					if (!world.get_loc(theSecondTemperoryX, theSecondTemperoryY)->can_move(UNIT_LAND))
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
			finalDirection == LEADER_AT_BOTTOM_RIGHT;
		}
	}
	//if all the formation mode are not allowed//
	if (!check)
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
void sandwich_formation(int direction)
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
//--------------------------------------------//

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
	maxLocX = 0;
	minLocX = MAX_WORLD_X_LOC;
	maxLocY = 0;
	minLocY = MAX_WORLD_Y_LOC;
	Unit* general;
	int unitIndex=0;
//-------------------------------------//
	for (t = 1; t <= nSelected; t++)
	{		 
		unitPtr = get_unit(t);
		if(unitPtr->team_info)
			general = unitPtr;
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

	int centerX = (maxLocX+minLocX)/2;
	int centerY = (maxLocY+minLocY)/2;

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
	
	

	if(closeAttackUnits)
		mem_del(closeAttackUnits);
	if(rangeAttackUnits)
		mem_del(rangeAttackUnits);
}



