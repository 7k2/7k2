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

// Filename    : OF_MFORK.CPP
// Description : Monster Fortress attack

#include <OF_MFORT.H>
#include <OUNIT.H>
#include <OWORLD.H>
#include <OBULLET.H>
#include <ONATION.H>


int FirmMonsterFortress::return_fire( BaseObj *attackerObj )
{
	// remember the target

	if( attackerObj )
	{
		if( target_count < MAX_FORTRESS_TARGET )
		{
			target_base_obj_recno[target_count] = attackerObj->base_obj_recno;
			target_count++;
			return 1;
		}
		else if( m.random(100) < 20 )		// 20% to preempt
		{
			target_base_obj_recno[m.random(MAX_FORTRESS_TARGET)] = attackerObj->base_obj_recno;
			return 1;
		}
	}

	return 0;
}


void FirmMonsterFortress::process_monster_firm()
{
	// fire at target

	int fireCount = m.random(3)-1;		// may be negative, also mean zero

	// archer's remain_attack_delay decreases in sprite_array.process
	// so not need to decrease here

	if( target_count <= 0 )
		return;

	int i = m.random(target_count);
	for( int a = 0; a < archer_count && fireCount > 0; ++a )
	{
		Unit *archerPtr = unit_array[archer_unit_recno[a]];
		if( archerPtr->remain_attack_delay > 0 )
			continue;

		for( int j = 0; j < target_count; ++j, ++i )
		{
			if( i >= target_count )
				i = 0;

			short targetObjRecno = target_base_obj_recno[i];

			if( base_obj_array.is_deleted(targetObjRecno) )
				continue;

			BaseObj *targetObj = base_obj_array[targetObjRecno];

			if( !targetObj->obj_is_visible() || !archerPtr->can_attack_target(targetObjRecno) )
				continue;
			break;
		}

		if( j < target_count )
		{
			BaseObj *targetObj = base_obj_array[target_base_obj_recno[i]];

			// find the maximum damage range attack

			int curAttack;
			int maxDamageAttack;
			int maxDamage = 0;

			for( curAttack = 0; curAttack < archerPtr->attack_count; ++curAttack )
			{
				AttackInfo *attackInfo = &archerPtr->attack_info_array[curAttack];

				if( attackInfo->attack_range > 2 && archerPtr->can_attack_with(curAttack) )	// don't allow spitfire to attack
				{
					AttackInfo *attackInfo = &archerPtr->attack_info_array[curAttack];
					if( attackInfo->attack_damage > maxDamage )
					{
						maxDamage = attackInfo->attack_damage;
						maxDamageAttack = curAttack;
					}
				}
			}

			if( maxDamage )
			{
				fire_archer_id = a+1;		// for get_attacker_unit to get attacking unit

				AttackInfo *attackInfo = &archerPtr->attack_info_array[maxDamageAttack];
				archerPtr->cur_attack = maxDamageAttack;
				err_when( !archerPtr->cur_attack_info()->bullet_sprite_id );

				// ------ add bullet --------//

				int bulletRecno = 0;
				if( targetObj->cast_to_Unit() )
					bulletRecno = bullet_array.add_bullet( this, targetObj->cast_to_Unit() );
				else if( targetObj->cast_to_Place() )
					bulletRecno = bullet_array.add_bullet( this, targetObj->cast_to_Place() );

				if( bulletRecno )
				{
					// ----- change attribute of archer  --------//

					archerPtr->remain_attack_delay = attackInfo->attack_delay + attackInfo->bullet_out_frame;
					archerPtr->cur_power -= attackInfo->min_power;

					// ----- change attribute of bullet ------//

					Bullet *bulletPtr = bullet_array[bulletRecno];
					bulletPtr->parent_recno = archerPtr->sprite_recno;
					bulletPtr->parent_base_obj_recno = archerPtr->base_obj_recno;
					bulletPtr->attack_attribute = attackInfo->attack_attribute;

					fireCount--;
				}

				fire_archer_id = 0;			// clear it as soon as bullet fired
			}
		}	// if( j < target_count )
	}	// for(a)
}


void FirmMonsterFortress::process_scan_target()
{
	// removed deleted or out of range target

	short srcCount = target_count;
	short srcTarget[MAX_FORTRESS_TARGET];
	err_when( sizeof(target_base_obj_recno) != sizeof(srcTarget) );
	memcpy( srcTarget, target_base_obj_recno, sizeof(srcTarget) );

	target_count = 0;
	for( int i = 0; i < srcCount; ++i )
	{
		if( base_obj_array.is_deleted(srcTarget[i]) )
			continue;

		BaseObj *targetObj = base_obj_array[srcTarget[i]];

		if( !targetObj->obj_is_visible() )
			continue;

		// --- if no archer can attack also remove -----//

		for( int a = 0; a < archer_count && !unit_array[archer_unit_recno[a]]->can_attack_target(targetObj->base_obj_recno); ++a );
		if( a >= archer_count )
			continue;		// no archer can attack

		//------------------------------------------------//

		if( area_distance( targetObj ) > FORTRESS_SCAN_RANGE )
			continue;

		// ##### begin Gilbert 4/2 ######//
		// ----- skip camouflaged unit ------//

		Unit *unitPtr = targetObj->cast_to_Unit();
		if( unitPtr && unitPtr->is_camouflage(nation_recno) )
			continue;
		// ##### end Gilbert 4/2 ######//

		target_base_obj_recno[target_count] = srcTarget[i];
		target_count++;
	}

	if( target_count < MAX_FORTRESS_TARGET && archer_count > 0 )		// don't find new target
	{
		int xLoc1 = loc_x1 - FORTRESS_SCAN_RANGE;
		int yLoc1 = loc_y1 - FORTRESS_SCAN_RANGE;
		int xLoc2 = loc_x2 + FORTRESS_SCAN_RANGE;
		int yLoc2 = loc_y2 + FORTRESS_SCAN_RANGE;

		// reduce scanning range

		switch( info.game_date % 4 )
		{
		case 0: xLoc2 = center_x;   break;		// left half
		case 1: xLoc1 = center_x+1; break;		// right half
		case 2: yLoc2 = center_y;   break;		// top half
		case 3: yLoc1 = center_y+1; break;		// bottom half
		}
		
		xLoc1 = max( xLoc1, 0 );
		yLoc1 = max( yLoc1, 0 );
		xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
		yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );

		//------------------------------------------//
		
		int 		xLoc, yLoc;
		Unit* 	unitPtr;
		Location* locPtr;

		for( yLoc=yLoc1; yLoc<=yLoc2 && target_count < MAX_FORTRESS_TARGET; yLoc++ )
		{
			locPtr = world.get_loc(xLoc1, yLoc);

			for( xLoc=xLoc1; xLoc<=xLoc2 && target_count < MAX_FORTRESS_TARGET; xLoc++, locPtr++ )
			{
				//--- if there is an enemy unit here ---// 

				int scannedUnitRecno;
				if( (scannedUnitRecno = locPtr->unit_recno(UNIT_LAND)) && !unit_array.is_deleted(scannedUnitRecno) )
				{
					unitPtr = unit_array[scannedUnitRecno];

					if( unitPtr->hit_points <= 0.0f )
						continue;

					// ----- skip camouflaged unit ------//

					if( unitPtr->is_camouflage(nation_recno) )
						continue;

					// if in target list , skip

					for( int i = 0; i < target_count && target_base_obj_recno[i] != unitPtr->base_obj_recno; ++i );
					if( i < target_count )
						continue;		// inside the list

					for( int a = 0; a < archer_count && !unit_array[archer_unit_recno[a]]->can_attack_target(unitPtr->base_obj_recno); ++a );
					if( a >= archer_count )
						continue;		// no archer can attack

					return_fire( unitPtr );		// add unit in the list
				}
			}
		}
	}
}


/*
// ----- begin of function FirmMonsterFortress::process_monster_firm -----//
//
void FirmMonsterFortress::process_monster_firm()
{
	enum { SCAN_X_RANGE = 6,
			 SCAN_Y_RANGE = 6  };

	int i, j;

	// ----- pack valid target --------//

	for( i = j = 0; i < MAX_FORTRESS_SOLDIER; ++i )
	{
		BaseObj *targetObj;

		if( !base_obj_array.is_deleted(target_obj_recno[i]) 
			&& area_distance(targetObj = base_obj_array[target_obj_recno[i]]) <= SCAN_X_RANGE )
		{
			target_obj_recno[j] = target_obj_recno[i];	// pack valid locked target
			j++;
		}
	}

	err_when( j > i );
	
	// ------ seek new target --------//

	if( j < archer_count )		// no need to seek new target yet
	{

//	animation_offset = 0;
	
		int xLoc1 = loc_x1 - SCAN_X_RANGE;
		int yLoc1 = loc_y1 - SCAN_Y_RANGE;
		int xLoc2 = loc_x2 + SCAN_X_RANGE;
		int yLoc2 = loc_y2 + SCAN_Y_RANGE;

		// reduce scanning range

		switch( info.game_date % 4 )
		{
		case 0: xLoc2 = center_x;   break;		// left half
		case 1: xLoc1 = center_x+1; break;		// right half
		case 2: yLoc2 = center_y;   break;		// top half
		case 3: yLoc1 = center_y+1; break;		// bottom half
		}
		
		xLoc1 = max( xLoc1, 0 );
		yLoc1 = max( yLoc1, 0 );
		xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
		yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );

		//------------------------------------------//
		
		int 		xLoc, yLoc;
		Unit* 	unitPtr;
		Location* locPtr;

		for( yLoc=yLoc1 ; yLoc<=yLoc2 && j < archer_count ; yLoc++ )
		{
			locPtr = world.get_loc(xLoc1, yLoc);

			for( xLoc=xLoc1 ; xLoc<=xLoc2 && j < archer_count ; xLoc++, locPtr++ )
			{
				//--- if there is an enemy unit here ---// 

				if( locPtr->unit_recno(UNIT_LAND) )
				{
					unitPtr = unit_array[ locPtr->unit_recno(UNIT_LAND) ];

					if( !unitPtr->nation_recno )
						continue;

					//--- if the unit is idle and he is our enemy ---//

					if( nation_array.should_attack(nation_recno, unitPtr->nation_recno) 
						&& unitPtr->hit_points > 0.0f )
					{
						// count if exist in target_obj_recno
						short targetBaseObjRecno = unitPtr->base_obj_recno;
						for( i = 0; i < j && target_obj_recno[i] != targetBaseObjRecno; ++i );
						if( i >= j )
						{
							// not exist in the array
							err_when( j >= MAX_FORTRESS_SOLDIER );
							target_obj_recno[j] = targetBaseObjRecno;
							++j;		// SHOOT him in next for loop
						}
					}
				}

			}
		}
	
	}

	// ----- fire at each target --------//

	for( i = 0; i < j; ++i )
	{
		return_fire( base_obj_array[target_obj_recno[i]] );
	}

	// ----- fill trailing target_obj_recno ------//

	for( i = j; i < MAX_FORTRESS_SOLDIER; ++i )
	{
		target_obj_recno[i] = 0;
	}

}
// ----- end of function FirmMonsterFortress::process_monster_firm -----//


// ----- begin of function FirmMonsterFortress::return_fire -----//
//
int FirmMonsterFortress::return_fire(BaseObj* targetObj)
{
	for(int i = 0; i < archer_count; ++i )
	{
		Unit *archerPtr = unit_array[archer_unit_recno[i]];
		if( archerPtr->remain_attack_delay == 0 )
		{

			// find the maximum damage range attack

			int curAttack;
			int maxDamageAttack;
			int maxDamage = 0;

			for( curAttack = 0; curAttack < archerPtr->attack_count; ++curAttack )
			{
				AttackInfo *attackInfo = &archerPtr->attack_info_array[curAttack];
				// better to call can_attack_with, but protected
				if( attackInfo->attack_range >= 2
					&& archerPtr->combat_level() >= attackInfo->combat_level 
					&& archerPtr->cur_power >= attackInfo->min_power )
				{
					if( attackInfo->attack_damage > maxDamage )
					{
						maxDamage = attackInfo->attack_damage;
						maxDamageAttack = curAttack;
					}
				}
			}

			if( maxDamage )
			{
				fire_archer_id = i+1;		// for get_attacker_unit to get attacking unit

				AttackInfo *attackInfo = &archerPtr->attack_info_array[maxDamageAttack];
				archerPtr->cur_attack = maxDamageAttack;
				err_when( !archerPtr->cur_attack_info()->bullet_sprite_id );

				// ------ add bullet --------//

				int bulletRecno = 0;
				if( targetObj->cast_to_Unit() )
					bulletRecno = bullet_array.add_bullet( this, targetObj->cast_to_Unit() );
				else if( targetObj->cast_to_Firm() )
					bulletRecno = bullet_array.add_bullet( this, targetObj->cast_to_Firm() );

				if( bulletRecno )
				{
					// ----- change attribute of archer  --------//

					archerPtr->remain_attack_delay = attackInfo->attack_delay + attackInfo->bullet_out_frame;
					archerPtr->cur_power -= attackInfo->min_power;

					// ----- change attribute of bullet ------//

					Bullet *bulletPtr = bullet_array[bulletRecno];
					bulletPtr->parent_recno = archerPtr->sprite_recno;
					bulletPtr->parent_base_obj_recno = archerPtr->base_obj_recno;
					bulletPtr->attack_attribute = attackInfo->attack_attribute;
				}

				fire_archer_id = 0;			// clear it as soon as bullet fired
				return bulletRecno;
			}
		}
	}

	return 0;
}
// ----- end of function FirmMonsterFortress::return_fire -----//
*/

// ----- begin of function FirmMonsterFortress::get_attacker_unit -----//

Unit *FirmMonsterFortress::get_attacker_unit()
{
	if( fire_archer_id > 0 && fire_archer_id <= archer_count )
	{
		return unit_array[archer_unit_recno[fire_archer_id-1]];
	}
	else
	{
		err_here();
		return NULL;
	}
}
// ----- end of function FirmMonsterFortress::get_attacker_unit -----//


// ----- begin of function FirmMonsterFortress::bullet_damage -----//
//
float FirmMonsterFortress::bullet_damage()
{
	return get_attacker_unit()->actual_damage();
}
// ----- end of function FirmMonsterFortress::bullet_damage -----//


// ----- begin of function FirmMonsterFortress::bullet_radius -----//
//
short FirmMonsterFortress::bullet_radius()
{
	return get_attacker_unit()->cur_attack_info()->bullet_radius;
}
// ----- end of function FirmMonsterFortress::bullet_radius -----//


// ----- begin of function FirmMonsterFortress::bullet_fire -----//
//
char FirmMonsterFortress::bullet_fire()
{
	return get_attacker_unit()->cur_attack_info()->fire_radius;
}
// ----- end of function FirmMonsterFortress::bullet_fire -----//


// ----- begin of function FirmMonsterFortress::bullet_id -----//
//
short FirmMonsterFortress::bullet_id()
{
	return get_attacker_unit()->cur_attack_info()->bullet_sprite_id;
}
// ----- end of function FirmMonsterFortress::bullet_id -----//


// ----- begin of function FirmMonsterFortress::bullet_speed -----//
//
short FirmMonsterFortress::bullet_speed()
{
	return get_attacker_unit()->cur_attack_info()->bullet_speed;
}
// ----- end of function FirmMonsterFortress::bullet_speed -----//


// ----- begin of function FirmMonsterFortress::bullet_init_z -----//
//
short FirmMonsterFortress::bullet_init_z()
{
	return altitude + 32;
}
// ----- end of function FirmMonsterFortress::bullet_init_z -----//
