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

//Filename    : OBULLETA.CPP
//Description : Object Bullet Array
//Owner		  : Alex

#include <ovga.h>
#include <ounit.h>
#include <obullet.h>
#include <oworld.h>
#include <otown.h>
#include <of_fort.h>
#include <of_lish.h>
#include <oworldmt.h>
#include <ob_proj.h>
#include <ob_homin.h>
#include <ob_flame.h>
#include <ob_prog.h>
#include <math.h>

//--------- Begin of function BulletArray::BulletArray ---------//
//
// <int> initArraySize - the initial size of this array.
//
BulletArray::BulletArray(int initArraySize) : SpriteArray(initArraySize)
{
}
//----------- End of function BulletArray::BulletArray -----------//


//--------- Begin of function BulletArray::create_bullet ---------//
//
// return: <int> the recno of the bullet created.
//
int BulletArray::create_bullet(short spriteId, Bullet** bpp)
{
	Bullet* bulletPtr;
	SpriteInfo *spriteInfo = sprite_res[spriteId];
	err_when(!spriteInfo);
	err_when(spriteInfo->sprite_type != 'B');
	switch(spriteInfo->sprite_sub_type)
	{
	case 0:
	case ' ':
		bulletPtr = new Bullet;
		break;
	case 'P':
		bulletPtr = new Projectile;
		break;
	case 'H':
		bulletPtr = new BulletHoming;
		break;
	case 'F':
		bulletPtr = new BulletFlame;
		break;
	case 'M':
		bulletPtr = new BulletProgram;
		break;
	
	default:
		err_here();			// undefined bullet type
		if( bpp )
			*bpp = NULL;
		return 0;
	}

	add(bulletPtr);
	if( bpp )
		*bpp = bulletPtr;
	return recno();
}
//----------- End of function BulletArray::create_bullet -----------//


//--------- Begin of function BulletArray::bullet_class_size ---------//
int BulletArray::bullet_class_size(int spriteId)
{
	SpriteInfo *spriteInfo = sprite_res[spriteId];
	err_when(spriteInfo->sprite_type != 'B');
	switch(spriteInfo->sprite_sub_type)
	{
	case 0:
	case ' ':
		return sizeof(Bullet);
	case 'P':
		return sizeof(Projectile);
	case 'H':
		return sizeof(BulletHoming);
	case 'F':
		return sizeof(BulletFlame);
	case 'M':
		return sizeof(BulletProgram);

	default:
		err_here();			// undefined bullet type
		return 0;
	}
}
//----------- End of function BulletArray::bullet_class_size -----------//

//--------- Begin of function BulletArray::bullet_path_possible ---------//
// For default bullet:
//		if the bullet is not blocked by building in its path, this
//		function return 1 for success, otherwise return 0.
// For projectile bullet:
//		always return 1
//
int BulletArray::bullet_path_possible(short startXLoc, short startYLoc, char attackerMobileType,
												  short destXLoc, short destYLoc, char targetMobileType,
												  char bulletSpeed, short bulletSpriteId, Unit* emitter)
{
	// ##### begin Gilbert 6/5 ######//
	return 1;
	// ##### end Gilbert 6/5 ######//

	if(attackerMobileType==UNIT_AIR || targetMobileType==UNIT_AIR)
		return 1;

	//-------- skip the checking for projectile -----------//
	SpriteInfo *spriteInfo = sprite_res[bulletSpriteId];
	if(spriteInfo->sprite_sub_type == 'P')
		return 1;

	err_when(spriteInfo->sprite_sub_type == 'P');

	//----------------------- define variables ---------------//

	int originX	= startXLoc*LOCATE_WIDTH;
	int originY	= startYLoc*LOCATE_HEIGHT;
	int goX		= destXLoc*LOCATE_WIDTH;
	int goY		= destYLoc*LOCATE_HEIGHT;

	int xStep	= (goX - originX)/bulletSpeed;
	int yStep	= (goY - originY)/bulletSpeed;

	int totalStep = max(1, max(abs(xStep), abs(yStep)));
	int curStep = 0;

	//------------------------------------------------------//
	// if the path of the bullet is blocked, return 0
	//------------------------------------------------------//
	int curX = originX + LOCATE_WIDTH/2;
	int curY = originY + LOCATE_HEIGHT/2;
	int curXLoc, curYLoc;
	Location *locPtr;

	while(curStep++<totalStep)
	{
		err_when(curStep > 200);

		curX += xStep;
		curY += yStep;

		curXLoc = curX/LOCATE_WIDTH;
		curYLoc = curY/LOCATE_HEIGHT;

		if(curXLoc==startXLoc && curYLoc==startYLoc)
			continue;

		if(curXLoc==destXLoc && curYLoc==destYLoc)
			break;	// is destination

		locPtr = world.get_loc(curXLoc, curYLoc);

		if(!emitter)
		{
			if(!locPtr->walkable(3))
				return 0;
		}
		else
		{
			if(!locPtr->walkable(3) && (locPtr->unit_recno(emitter->mobile_type) != emitter->sprite_recno))
				return 0;
		}
	}

	return 1;
}
//----------- End of function BulletArray::bullet_path_possible -----------//

//--------- Begin of function BulletArray::add_bullet_possible ---------//
// call bullet_possible to check whether bullets emitted from this position
// can reach the target location with blocking
//
// return 1 if bullets are able to reach the target location
// return 0 if not
//
int BulletArray::add_bullet_possible(short startXLoc, short startYLoc, char attackerMobileType,
												 short targetXLoc, short targetYLoc, char targetMobileType,
												 short targetWidth, short targetHeight, short& resultXLoc, short& resultYLoc,
												 char bulletSpeed, short bulletSpriteId, Unit* emitter)
{
	resultXLoc = resultYLoc = -1;

	//----------------------------------------------------------------------//
	// for target with size 1x1
	//----------------------------------------------------------------------//
	if(targetWidth==1 && targetHeight==1)
	{
		if(bullet_path_possible(startXLoc, startYLoc, attackerMobileType, targetXLoc, targetYLoc, targetMobileType, bulletSpeed, bulletSpriteId, emitter))
		{
			resultXLoc = targetXLoc;
			resultYLoc = targetYLoc;
			return 1;
		}
		else 
			return 0;
	}

	err_when(targetWidth==1 && targetHeight==1);
	//----------------------------------------------------------------------//
	// choose the closest corner to be the default attacking point of range attack
	//
	// generalized case for range-attack is coded below. Work for target with
	// size > 1x1 
	//----------------------------------------------------------------------//

	//-------------- define parameters --------------------//
	short	adjWidth	= targetWidth-1;	// adjusted width;
	short	adjHeight = targetHeight-1; // adjusted height
	short xOffset=0, yOffset=0;	// the 1st(default) location for range attack
	short atEdge = 0;	// i.e. the attacking point is at the corner or edge of the target
							// 1 for at the edge, 0 for at corner
		
	//----------------------------------------------------------------------//
	// determine initial xOffset
	//----------------------------------------------------------------------//
	if(startXLoc <= targetXLoc)
		xOffset = 0; // the left hand side of the target
	else if(startXLoc >= targetXLoc+adjWidth)
		xOffset = adjWidth; // the right hand side of the target
	else
	{
		xOffset = startXLoc - targetXLoc; // in the middle(vertical) of the target
		atEdge++;
	}

	//----------------------------------------------------------------------//
	// determine initial yOffset
	//----------------------------------------------------------------------//
	if(startYLoc <= targetYLoc)
		yOffset = 0;	// the upper of the target
	else if(startYLoc >= targetYLoc+adjHeight)
		yOffset = adjHeight;
	else
	{
		yOffset = startYLoc - targetYLoc; // in the middle(horizontal) of the target
		atEdge++;
	}

	//----------------------------------------------------------------------//
	// checking whether it is possible to add bullet
	//----------------------------------------------------------------------//
	if(bullet_path_possible(startXLoc, startYLoc, attackerMobileType, targetXLoc+xOffset, targetYLoc+yOffset, targetMobileType, bulletSpeed, bulletSpriteId, emitter))
	{
		resultXLoc = targetXLoc+xOffset;
		resultYLoc = targetYLoc+yOffset;
		return 1;
	}

	short	leftXOffset, leftYOffset;
	short	rightXOffset, rightYOffset;
	short leftX, leftY, rightX, rightY;
	short found=0;	// found a location to attack the target and the path is not blocked
	short end=0;
	
	if(atEdge)	// only check one edge of the target
	{
		if(xOffset==0 || xOffset==adjWidth) // horizontal edge
		{
			leftYOffset = rightYOffset = 0;
			leftXOffset = 1;
			rightXOffset = -1;
		}
		else if(yOffset==0 || yOffset==adjHeight) // vertical edge
		{
			leftXOffset = rightXOffset = 0;
			leftYOffset = 1;
			rightYOffset = -1;
		}
		else
			err_here();	// the sprite is within the target ???
	}
	else	// at the corner,	need to check two edges of the target
	{
		leftYOffset = rightXOffset = 0;
		leftXOffset = (xOffset==0) ? 1 : -1;
		rightYOffset = (yOffset==0) ? 1 : -1;
	}

	leftX = rightX = xOffset;
	leftY = rightY = yOffset;

	while(!found)
	{
		end = 0;
		
		//-------------------------------------------//
		// for the leftX, leftY
		//-------------------------------------------//
		leftX += leftXOffset;
		leftY += leftYOffset;
		if(leftX>=0 && leftX<targetWidth && leftY>=0 && leftY<targetHeight)
		{
			if(bullet_path_possible(startXLoc, startYLoc, attackerMobileType, targetXLoc+leftX, targetYLoc+leftY, targetMobileType, bulletSpeed, bulletSpriteId, emitter))
			{
				resultXLoc = targetXLoc+leftX;
				resultYLoc = targetYLoc+leftY;
				return 1;
			}
		}
		else
			end++;

		//-------------------------------------------//
		// for the rightX, rightY
		//-------------------------------------------//
		rightX += rightXOffset;
		rightY += rightYOffset;
		if(rightX>=0 && rightX<targetWidth && rightY>=0 && rightY<targetHeight)
		{
			if(bullet_path_possible(startXLoc, startYLoc, attackerMobileType, targetXLoc+rightX, targetYLoc+rightY, targetMobileType, bulletSpeed, bulletSpriteId, emitter))
			{
				resultXLoc = targetXLoc+rightX;
				resultYLoc = targetYLoc+rightY;
				return 1;
			}
		}
		else
		{
			if(end)	// all locations have been checked, all are blocked
				return 0;
		}
	}

	return 0;
}
//----------- End of function BulletArray::add_bullet_possible -----------//

//--------- Begin of function Bullet::add_bullet ---------//
// Overload of add_bullet()
//
// <Firm*> parentFirm - pointer to the attacking target Firm
// <Unit*> targetUnit - pointer to the target Unit
//
//	return 1 if bullet is added successfully otherwise return 0.
short BulletArray::add_bullet(Firm* parentFirm, Unit* targetUnit)
{
	Bullet *bulletPtr;

	// ####### begin Gilbert 1/12 ######// 
	err_when( !parentFirm->bullet_id() );
	create_bullet( parentFirm->bullet_id(), &bulletPtr );
	// ####### end Gilbert 1/12 ######// 
	
	bulletPtr->init( BULLET_BY_FIRM, parentFirm->firm_recno,
		targetUnit->next_x_loc(), targetUnit->next_y_loc(), targetUnit->mobile_type);
	return bulletPtr->sprite_recno;
}
//----------- End of function BulletArray::add_bullet -----------//


//--------- Begin of function Bullet::add_bullet ---------//
// Overload of add_bullet()
//
// <Unit*> parentFirm - pointer to the attacking target Firm
// <Unit*> targetFirm - pointer to the target Firm
//
//	return 1 if bullet is added successfully otherwise return 0.
short BulletArray::add_bullet(Place* parentPlace, Place* targetPlace)
{
	Bullet *bulletPtr;

/*	if (parentPlace == targetPlace && 
		(parentPlace->bullet_id() == 0 || sprite_res[parentPlace->bullet_id()]->sprite_sub_type != 'M'))
	{
		static spriteId = sprite_res.search_sprite( "FIRMDIE" );
		err_when( !spriteId );
		create_bullet( spriteId, &bulletPtr );
	}
	else*/
	{
		err_when( !parentPlace->bullet_id() );
		create_bullet( parentPlace->bullet_id(), &bulletPtr );
	}

	if( parentPlace->cast_to_Firm() )
	{
		bulletPtr->init( BULLET_BY_FIRM, parentPlace->cast_to_Firm()->firm_recno,
			targetPlace->center_x, targetPlace->center_y, UNIT_LAND);
	}
	else if( parentPlace->cast_to_Town() )
	{
		bulletPtr->init( BULLET_BY_TOWN, parentPlace->cast_to_Town()->town_recno,
			targetPlace->center_x, targetPlace->center_y, UNIT_LAND);
	}
	return bulletPtr->sprite_recno;

	return 0;
}
//----------- End of function BulletArray::add_bullet -----------//

//--------- Begin of function BulletArray::add_bullet ---------//
//same as add_bullet(Unit*, Unit*)
//overloaded for used for attacking baseObj
short BulletArray::add_bullet(Unit* parentUnit, BaseObj* targetObj)
{
	//------------------------------------------------------//
	// define parameters
	//------------------------------------------------------//
	//SpriteInfo *targetSpriteInfo = targetUnit->sprite_info;
	AttackInfo *attackInfo = parentUnit->attack_info_array+parentUnit->cur_attack;
	short attackXLoc	= parentUnit->range_attack_x_loc;
	short attackYLoc	= parentUnit->range_attack_y_loc;
	short targetXLoc = targetObj->obj_loc_x1();
	short targetYLoc = targetObj->obj_loc_y1();

	if(attackXLoc >= targetXLoc && attackXLoc < targetXLoc+targetObj->obj_loc_width() &&
		attackYLoc >= targetYLoc && attackYLoc < targetYLoc+targetObj->obj_loc_height())
	{
		//-------------------------------------------------------//
		// the previous used range attack destination can be reused,
		// time is saved 'cos no need to check for bullet_path_possible()
		//-------------------------------------------------------//
		short bulletId = (parentUnit->attack_info_array+parentUnit->cur_attack)->bullet_sprite_id;
		Bullet* bulletPtr;
		create_bullet( bulletId, &bulletPtr);
		bulletPtr->init(BULLET_BY_UNIT, parentUnit->sprite_recno, attackXLoc, attackYLoc, targetObj->obj_mobile_type());
		err_when(parentUnit->cur_dir!=parentUnit->final_dir);
		parentUnit->get_dir(parentUnit->next_x_loc(), parentUnit->next_y_loc(), attackXLoc, attackYLoc);
		return bulletPtr->sprite_recno;
	}

	err_here();
	return 0;
}
 //----------- End of function BulletArray::add_bullet -----------//



//--------- Begin of function Bullet::add_bullet ---------//
// Overload of add_bullet()
//
// <Firm*> parentFirm - pointer to the attacking target Firm
// <Unit*> targetUnit - pointer to the target Unit
//
//	return 1 if bullet is added successfully otherwise return 0.
short BulletArray::add_bullet(Town* parentTown, Unit* targetUnit)
{
	Bullet *bulletPtr;
	create_bullet( parentTown->bullet_id(), &bulletPtr );
	bulletPtr->init( BULLET_BY_TOWN, parentTown->town_recno,
		targetUnit->next_x_loc(), targetUnit->next_y_loc(), targetUnit->mobile_type);
	return bulletPtr->sprite_recno;
}
//----------- End of function BulletArray::add_bullet -----------//
