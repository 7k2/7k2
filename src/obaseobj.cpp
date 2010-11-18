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

//Filename   : OBASEOBJ.CPP
//Description: BaseObj Array

#include <owarpt.h>
#include <obaseobj.h>
#include <ofirm.h>
#include <onation.h>
#include <ose.h>
#include <ovolume.h>
#include <onews.h>

//-------- Begin of function BaseObj::BaseObj --------//

BaseObj::BaseObj()
{
	// preserve virtual pointer
	memset( (char *)this + sizeof(void *), 0, sizeof(BaseObj) - sizeof(void *) );
}
//--------- End of function BaseObj::BaseObj --------//


//--------- Begin of function BaseObj::nation_ptr ---------//
//
Nation* BaseObj::nation_ptr()
{
	if( nation_recno )
		return nation_array[nation_recno];
	else
		return NULL;
}
//----------- End of function BaseObj::nation_ptr -----------//


//--------- Begin of function BaseObj::is_own --------//
//
int BaseObj::is_own()
{
	return nation_array.player_recno && nation_array.player_recno == nation_recno;
}
//--------- End of function BaseObj::is_own --------//


//--------- Begin of function BaseObj::being_attack_hit --------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
// <float>	  damagePoint - the points of damage made.
//
void BaseObj::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	err_when( !is_visible() );

	//-------- reduce it points -------//

//	damagePoint -= obj_armor();

	if( damagePoint > 0.0 )
	{
		float originDamagePoint = damagePoint;
		damagePoint -= obj_armor();
		if( damagePoint <= -1.0f )
		{
			damagePoint = -1.0f / damagePoint;		// if defense too strong, damage = 1/(obj_armor - damage)
		}
		else if( damagePoint < 1.0f )
		{
			damagePoint = 1.0f;
		}

		if( damagePoint > originDamagePoint )		// cannot be greater than the original damage
			damagePoint = originDamagePoint;
	}
	else
	{
		damagePoint = 0.0f;
	}

	err_when( damagePoint < 0.0f );

	//----- buildings take longer to destroy -----//

	if( obj_type == OBJ_FIRM )
		damagePoint /= 5;

//	int alreadyKilled = hit_points <= 0 ? 1 : 0;
	hit_points -= damagePoint / ATTACK_SLOW_DOWN;

	//------- set other vars --------//

	being_attack_hit2(attackerObj);


	//---- if the hit points has been reduced to zero or below ----//

	if( hit_points <= 0 )//&& !alreadyKilled )
	{
		hit_points = 0;
		being_killed(attackerObj);
	}

	//------ debug code -------//

	#ifdef DEBUG

	if( attackerObj && attackerObj->obj_is_visible())
	{
		err_when(
			( world.get_loc( attackerObj->obj_loc_x1(), attackerObj->obj_loc_y1() )
			->base_obj_recno(UNIT_AIR) != attackerObj->base_obj_recno ) &&
			( world.get_loc( attackerObj->obj_loc_x1(), attackerObj->obj_loc_y1() )
			->base_obj_recno(UNIT_LAND) != attackerObj->base_obj_recno ) &&
			( world.get_loc( attackerObj->obj_loc_x1(), attackerObj->obj_loc_y1() )
			->base_obj_recno(UNIT_SEA) != attackerObj->base_obj_recno ) 
			);
	}
	#endif
}
//---------- End of function BaseObj::being_attack_hit ----------//


//--------- Begin of function BaseObj::being_attack_hit2 --------//
//
// If the derived class does not call being_attack_hit(), it still
// has to call being_attack_hit2(), because some vars must be set
// for an attack action.
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void BaseObj::being_attack_hit2(BaseObj* attackerObj)
{
	//-------- set at war status --------//

	if( attackerObj )
	{
		if( nation_recno )
		{
			nation_ptr()->set_at_war_today();
			nation_ptr()->being_attacked(attackerObj);
		}

		if( attackerObj->nation_recno )
			attackerObj->nation_ptr()->set_at_war_today();
	}

	//------- add war point ---------//

	if( is_own() )
	{
		// ######## begin Gilbert 8/4 #########//
		war_point_array.add_point( obj_loc_x1(), obj_loc_y1() );
		if( obj_loc_x2() > obj_loc_x1()
			|| obj_loc_y2() > obj_loc_y1() )
		{
			war_point_array.add_point( obj_loc_x2(), obj_loc_y1() );
			war_point_array.add_point( obj_loc_x1(), obj_loc_y2() );
			war_point_array.add_point( obj_loc_x2(), obj_loc_y2() );
		}
		// ######## end Gilbert 8/4 #########//

		//------- add effect when building first comes under attack ---------//
		if ((info.game_date - last_attack_date) > 6 && is_own())
		{
			if( obj_type == OBJ_FIRM )
			{
				RelVolume relVolume(PosVolume(obj_loc_x1(), obj_loc_y1()));
				if( relVolume.rel_vol < 180)
					relVolume.rel_vol = 180;

				se_ctrl.request("BLD_ATK", relVolume);
				
				if( attackerObj ) 
					news_array.firm_attacked(cast_to_Firm()->firm_recno, attackerObj);
			}
			else
			if( obj_type == OBJ_TOWN )
			{
				RelVolume relVolume(PosVolume(obj_loc_x1(), obj_loc_y1()));
				if( relVolume.rel_vol < 180)
					relVolume.rel_vol = 180;

				se_ctrl.request("TOWN_ATK", relVolume);

				if( attackerObj ) 
					news_array.town_attacked(cast_to_Town()->town_name(), 
							cast_to_Town()->center_x,
							cast_to_Town()->center_y, attackerObj);
			}
		}
	}

	//----- set last_attack_date -----//

	last_attack_date = info.game_date;

	if( attackerObj )
	{
		last_attack_nation_recno = attackerObj->nation_recno;
		attackerObj->last_attack_date = info.game_date;
	}

	//-------- increase battling fryhtan score --------//

	if( is_monster() && attackerObj && attackerObj->nation_recno )
	{
		nation_array[attackerObj->nation_recno]->kill_monster_score += (float) 0.1;
	}
}
//---------- End of function BaseObj::being_attack_hit2 ----------//


//--------- Begin of function BaseObj::area_distance --------//
//
// distance between object, see m.area_distance
//
int BaseObj::area_distance( BaseObj *objPtr)
{
//	err_when( !obj_is_visible() );				// when a team member may ask for help, this function will be called, so the error checking should not be here. 
//	err_when( !objPtr->obj_is_visible() );

	register BaseObj *targetPtr = objPtr;

	return m.area_distance( obj_loc_x1(), obj_loc_y1(), obj_loc_x2(), obj_loc_y2(),
		targetPtr->obj_loc_x1(), targetPtr->obj_loc_y1(),
		targetPtr->obj_loc_x2(), targetPtr->obj_loc_y2() );
}
//--------- End of function BaseObj::area_distance --------//


//--------- Begin of function BaseObj::being_killed --------//
//
// Fryhtan killing a human gets life points.
//
void BaseObj::being_killed(BaseObj* attackerObj)
{
	if (already_killed)
		return;
	else
		already_killed = 1;

	//--- if the attacker belongs to a Fryhtan nation, killing units gets live points ---//
	
	if( attackerObj && attackerObj->nation_recno &&
		 nation_array[attackerObj->nation_recno]->is_monster() )
	{
		// ##### patch begin Gilbert 28/3 ###### //
		float gainedLivePoints= calc_live_points_bonus();

		//--- if the attacker is not a main monster unit type, only gain 50% of the live points ---//

		if( !attackerObj->is_monster() )
			gainedLivePoints /= 2;

		// ##### patch end Gilbert 28/3 ###### //

		nation_array[attackerObj->nation_recno]->change_live_points( gainedLivePoints );
	}
}
//--------- End of function BaseObj::being_killed --------//



// -------- begin of function BaseObj::calc_live_points_bonus ----//

float BaseObj::calc_live_points_bonus()
{
	float gainedLivePoints= 0.0f;

	if( cast_to_Unit() )
	{
		//-- human: live points gained = max_hit_points() --//

		if( cast_to_Unit()->is_human() )
			gainedLivePoints = (float) max_hit_points();

		//-- monster: live points gained = max_hit_points()/2 --//

		else if( cast_to_Unit()->is_monster() )
			gainedLivePoints = (float) max_hit_points()/2;
	}
	else if( cast_to_Firm() )
	{
		//-- firm: live points gained = max_hit_points()/2 --//

		if( cast_to_Firm()->firm_id == FIRM_LISHORR )
		{
			gainedLivePoints = (float) max_hit_points()/2;
		}
	}

	//--- Fryhtans will get increasingly more life points as the game passes ---//

	// ####### patch begin Gilbert 28/3 #####//
	// float incBonus = (float) (info.game_date-info.game_start_date) / (365*20);		// life points will double in 20 years.
	// incBonus = min(incBonus, 100);
	// gainedLivePoints += gainedLivePoints * incBonus;

	float incBonus = (float) (info.game_date-info.game_start_date) / (365*5);		// life points will double in 5 years.
	incBonus = min(incBonus, 4);
	gainedLivePoints += gainedLivePoints * incBonus;
	// ####### patch end Gilbert 28/3 #####//

	return gainedLivePoints;
}

// -------- end of function BaseObj::calc_live_points_bonus ----//


// -------- Begin of function BaseObj::is_stealth ---------//
//
int BaseObj::is_stealth()
{
	return 0;
}
// -------- End of function BaseObj::is_stealth ---------//


// -------- begin of function BaseObj::explore_for_player --------//
//
int BaseObj::explore_for_player()
{
	return is_own() 
		|| (nation_recno && nation_array[nation_recno]->is_allied_with_player);
}
// -------- end of function BaseObj::explore_for_player --------//


// -------- begin of function BaseObj::explore_for_player --------//
//
void BaseObj::change_nation(int newNationRecno)
{
	nation_recno = newNationRecno;
}
// -------- end of function BaseObj::explore_for_player --------//
