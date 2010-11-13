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

// Filename    : OSOLDIER.CPP
// Description : soldier at Camp/Fort

#include <OSOLDIER.H>
#include <ALL.H>
#include <OSKILL.H>
#include <OUNITRES.H>
#include <OMONSRES.H>
#include <ORACERES.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OF_CAMP.H>
#include <OSPY.H>


//------- Begin of function Soldier::Soldier -----------//
//
Soldier::Soldier()
{
	memset( this, 0, sizeof(Soldier) );
}
//--------- End of function Soldier::Soldier -----------//


//---------- Begin of function Soldier::small_icon_ptr --------//

char* Soldier::small_icon_ptr()
{
	return unit_res[unit_id]->get_small_icon_ptr(rank_id);
}
//---------- End of function Soldier::small_icon_ptr --------//


//---------- Begin of function Soldier::change_loyalty --------//

void Soldier::change_loyalty(int loyaltyChange)
{
//	if( town_recno ) 		// for those live in town, their loyalty are based on town people loyalty.
//		return;

	int newLoyalty = loyalty + loyaltyChange;

	newLoyalty 		 = min( 100, newLoyalty );
	loyalty = max( 0, newLoyalty );
}
//---------- End of function Soldier::change_loyalty --------//


//---------- Begin of function Soldier::change_hit_points --------//

void Soldier::change_hit_points(int changePoints)
{
//	err_when( town_recno );		// for those live in town, their loyalty are based on town people loyalty.

	int newHitPoints = hit_points + changePoints;
	int maxHitPoints = max_hit_points();

	newHitPoints = min( maxHitPoints, newHitPoints );
	hit_points   = max( 0, newHitPoints );
}
//---------- End of function Soldier::change_hit_points --------//


//--------- Begin of function Soldier::max_attack_range ---------//
int Soldier::max_attack_range()
{
	int maxRange=0;
	AttackInfo *attackInfo = unit_res.get_attack_info(unit_res[unit_id]->first_attack);
	int attackCount = unit_res[unit_id]->attack_count;

	for(int i=0; i<attackCount; i++, attackInfo++)
	{
		if(combat_level() >= attackInfo->combat_level &&
			attackInfo->attack_range>maxRange)
			maxRange = attackInfo->attack_range;
	}
	
	return maxRange;
}
//--------- End of function Soldier::max_attack_range -----------//


//--------- Begin of function Soldier::is_nation ---------//
//
// Whether this soldier belongs to the specific nation.
//
// <int> firmRecno - the recno of the firm the soldier works in
// <int> nationRecno - the recno of th nation to check against. 
//
int Soldier::is_nation(int firmRecno, int nationRecno)
{
	if( spy_recno && spy_array[spy_recno]->true_nation_recno == nationRecno )
		return 1;

//	if( town_recno )
//		return town_array[town_recno]->nation_recno == nationRecno;
//	else
	return firm_array[firmRecno]->nation_recno == nationRecno;
}
//----------- End of function Soldier::is_nation ---------//


//--------- Begin of function Soldier::is_own ---------//
//
int Soldier::is_own(int firmRecno)
{
   return nation_array.player_recno && is_nation(firmRecno, nation_array.player_recno);
}
//--------- End of function Soldier::is_own ---------//


//--------- Begin of function Soldier::is_own_spy ---------//
//
int Soldier::is_own_spy()
{
   return spy_recno && nation_array.player_recno 
		&& spy_array[spy_recno]->true_nation_recno == nation_array.player_recno;
}
//--------- End of function Soldier::is_own_spy ---------//


//--------- Begin of function Soldier::true_nation_recno ---------//
//
int Soldier::true_nation_recno(int firmRecno)
{
   if( spy_recno )
      return spy_array[spy_recno]->true_nation_recno;
   else
      return firm_array[firmRecno]->nation_recno;
}
//--------- End of function Soldier::true_nation_recno ---------//


//------- Begin of function Soldier::target_loyalty ---------//
//
int Soldier::target_loyalty(int firmRecno)
{
	FirmCamp* firmPtr = firm_array[firmRecno]->cast_to_FirmCamp();
	err_when( !firmPtr );		// soldier must be in a camp/fort

	// ###### patch begin Gilbert 27/9 #######//
	// some event in campaign may kill the king
	if( firmPtr->overseer_recno && !unit_array.is_deleted(firmPtr->overseer_recno) )
	// ###### patch end Gilbert 27/9 #######//
	{
		Unit* overseerUnit = unit_array[firmPtr->overseer_recno];

		int overseerSkill = overseerUnit->skill_level();
		int targetLoyalty = 30 + overseerSkill/2;

		//---------------------------------------------------//
		//
		// Soldiers with higher combat and leadership skill
		// will get discontented if they are led by a general
		// with low leadership.
		//
		//---------------------------------------------------//

		targetLoyalty -= combat_level()/2;

		if( skill_level() > overseerSkill )
			targetLoyalty -= skill_level() - overseerSkill;

		if( overseerUnit->rank_id == RANK_KING )
			targetLoyalty += 20;

		if( race_res.is_same_race(race_id, overseerUnit->race_id) )
			targetLoyalty += 20;

		if( targetLoyalty < 0 )
			targetLoyalty = 0;

		if( targetLoyalty > 100 )
			targetLoyalty = 100;

		return targetLoyalty;
	}
	else	//-- if there is no overseer, just return the current loyalty --//
	{
		return loyalty;
	}
}
//-------- End of function Soldier::target_loyalty -----------//


//-------- Begin of function Soldier::is_under_training -------//
//
int Soldier::is_under_training()
{
	// return combat_level() < BASIC_COMBAT_TRAIN;
	return remain_build_days > 0;
}
//-------- End of function Soldier::is_under_training -------//


//----------- Begin of function Soldier::soldier_power -----------//
//
// Return a power index of this soldier.
//
int Soldier::soldier_power()
{
	UnitInfo* unitInfo = unit_res[unit_id];

	// ######## begin Gilbert 24/3 #########//
	// if( unitInfo->unit_class == UNIT_CLASS_WEAPON )
	if( !unitInfo->class_info.has_combat_level )
	{
		int power = hit_points + unitInfo->weapon_power * 15;
		if( unitInfo->class_info.has_weapon_version )
			power += (get_weapon_version() - 1) * 15;
		return power;
	}
	else
	{
		return (int) hit_points;
	}
	// ######## end Gilbert 24/3 #########//
}
//----------- End of function Soldier::soldier_power -----------//


//----------- Begin of function Soldier::can_succeed_king -----------//
//
// <int> the nationRecno - the recno of the nation which this soldier belongs
//									to. Since Soldier does not contain any info
//									about the nation. This info has to be passed by
//								   other functions.
//
int Soldier::can_succeed_king(int nationRecno)
{
	if( is_under_training() )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	if( nationPtr->king_unit_recno )	// the king is still alive
		return 0;

	if( nationPtr->is_human() )
	{
		//-- only the nation is a human nation, only human military unit can succeed the king --//

		return is_human();
	}
	else
	{
		return monster_res[ nationPtr->monster_id() ]->unit_id == unit_id;
	}
}
//----------- End of function Soldier::can_succeed_king -----------//


//----------- Begin of function Soldier::init_name -----------//
//
void Soldier::init_name()
{
	if( is_human() )
		name_id = race_res[race_id]->get_new_name_id();
	else if( is_monster() )
		name_id = monster_res.get_new_name_id();
}
//----------- End of function Soldier::init_name -----------//


//----------- Begin of function Soldier::free_name -----------//
//
void Soldier::free_name()
{
	if( name_id )
	{
		if( is_human() )
			race_res[race_id]->free_name_id(name_id);
		else if( is_monster() )
			monster_res.free_name_id(name_id);

		name_id = 0;
	}
}
//----------- End of function Soldier::free_name -----------//


// ---------- Begin of function Soldier::change_unit_id ------//
//
void Soldier::change_unit_id(int unitId, int nationRecno)
{
	// err_when( hero_id );
	// must not change race
	err_when( !race_id );
	err_when( race_id && race_id != unit_res[unitId]->race_id );

	float hpRatio = (float)hit_points / max_hit_points();
	Skill oldSkill = skill;

	// -------- increase/decrease unit count -----//

	if( nationRecno )
	{
		unit_res[unitId]->inc_nation_unit_count(nationRecno);
		unit_res[unit_id]->dec_nation_unit_count(nationRecno);
	}

	// --------- change unit id ----------//

	unit_id = unitId;

	// ----- max_hit_points changed -----//
	
	// skill.std_hit_points = newUnitInfo->hit_points;
	skill.set_unit_id(unit_id);

	// ----- current hit points changed --------//

	hit_points = short(hpRatio * max_hit_points());
}
// ---------- End of function Soldier::change_unit_id ------//
