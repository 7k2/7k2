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

// Filename    : OTOWNAT.CPP
// Description : town attack

#include <OTOWN.H>
#include <ALL.H>
#include <ONATIONA.H>
#include <OBULLET.H>
#include <OSYS.H>

#define TOWN_BULLET_DAMAGE_BASE 6
#define TOWN_BULLET_DAMAGE_LOYAL 8
#define TOWN_BULLET_SPEED 15
#define TOWN_BULLET_INIT_Z 24


// ------- begin of function Town::return_fire ------//
//
// add to target_base_obj_recno
//
int Town::return_fire( BaseObj *attackerObj )
{
	if( !attackerObj )
		return 0;

	//--- only return fire when the resistance or loyalty > 0 ---//

	if( nation_recno )
	{
		if( loyalty < 1 )
			return 0;
	}
	else
	{
		if( attackerObj->nation_recno && resistance(attackerObj->nation_recno) < 1 )
			return 0;
	}

	//-------------------------------------------------//

	if( target_count < MAX_TOWN_TARGETS )
	{
		target_base_obj_recno[target_count] = attackerObj->base_obj_recno;
		target_count++;
		return 1;
	}
	else if( m.random(100) < 20 )		// 20% to preempt
	{
		target_base_obj_recno[m.random(MAX_TOWN_TARGETS)] = attackerObj->base_obj_recno;
	}

	return 0;
}
// ------- end of function Town::return_fire ------//


// ------- begin of function Town::process_attack_target ------//
//
// called by process to shoot target
//
void Town::process_attack_target()
{
	int fireCount;

	if( target_count <= 0 )
		return;

	for( fireCount = (m.random(3)==0) ; fireCount > 0; --fireCount )
	{
		int i = m.random(target_count);

		if( archers_energy <= 0 )
			break;

		// scan target to shoot

		short targetObjRecno = target_base_obj_recno[i];

		if( base_obj_array.is_deleted(targetObjRecno) )
			continue;

		BaseObj *targetObj = base_obj_array[targetObjRecno];

		if( !targetObj->obj_is_visible() )
			continue;

		if( area_distance( targetObj ) > TOWN_TARGET_RANGE + 4 )	// allow longer return fire range
			continue;

		//--- if the resistance towards the nation of the target is 0, don't attack it ---//

		if( !nation_array.should_attack(nation_recno, targetObj->nation_recno) )
			continue;

		if( nation_recno==0 &&
			 targetObj->nation_recno &&
			 resistance(targetObj->nation_recno) < 1 )
		{
			continue;
		}

		// ------ calculate arrow damage -----//

		if( nation_recno )		// non-independent town
			arrow_damage = loyalty * (TOWN_BULLET_DAMAGE_LOYAL / 100.0f) + TOWN_BULLET_DAMAGE_BASE;
		else if( targetObj->nation_recno)		// independent town
			arrow_damage = resistance_array[targetObj->nation_recno-1] * (TOWN_BULLET_DAMAGE_LOYAL / 100.0f) + TOWN_BULLET_DAMAGE_BASE;
		else		// independent town attacks independent unit
			arrow_damage = TOWN_BULLET_DAMAGE_LOYAL + TOWN_BULLET_DAMAGE_BASE;

		if( targetObj->cast_to_Unit() && bullet_array.add_bullet(this, targetObj->cast_to_Unit())
			|| targetObj->cast_to_Place() && bullet_array.add_bullet(this, targetObj->cast_to_Place()) )
		{
			archers_energy >>= 2;
		}
	}
}
// ------- end of function Town::process_attack_target ------//


// ------- begin of function Town::process_scan_target ------//

void Town::process_scan_target()
{
	// removed deleted or out of range target

	short srcCount = target_count;
	short srcTarget[MAX_TOWN_TARGETS];
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

		//--- if the resistance towards the nation of the target is 0, don't attack it ---//

		if( nation_recno==0 &&
			 targetObj->nation_recno &&
			 resistance(targetObj->nation_recno) < 1 )
		{
			continue;
		}

		//------------------------------------------------//

		if( area_distance( targetObj ) > TOWN_TARGET_RANGE )
			continue;

		if( !nation_array.should_attack(nation_recno, targetObj->nation_recno) )
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
}
// ------- end of function Town::process_scan_target ------//



// ------- begin of function Town::bullet_damage ------//
//
float Town::bullet_damage()
{
	return arrow_damage;
}
// ------- end of function Town::bullet_damage ------//


// ------- begin of function Town::bullet_radius ------//
//
short Town::bullet_radius()
{
	return LOCATE_WIDTH - 2;
}
// ------- endof function Town::bullet_radius ------//


// ------- begin of function Town::bullet_fire ------//
//
char Town::bullet_fire()
{
	return 0;
}
// ------- end of function Town::bullet_fire ------//


// ------- begin of function Town::bullet_id ------//
//
short Town::bullet_id()
{
	static spriteId;

	if ((sys.frame_count+town_recno) & 1)
		spriteId = sprite_res.search_sprite( "ROCK" );
	else
		spriteId = sprite_res.search_sprite( "BRICK" );

	err_when( !spriteId );

	return spriteId;
}
// ------- end of function Town::bullet_id ------//


// ------- begin of function Town::bullet_speed ------//
//
short Town::bullet_speed()
{
	return TOWN_BULLET_SPEED;
}
// ------- end of function Town::bullet_speed ------//


// ------- begin of function Town::bullet_init_z ------//
//
short Town::bullet_init_z()
{
	return altitude + TOWN_BULLET_INIT_Z;
}
// ------- end of function Town::bullet_init_z ------//

