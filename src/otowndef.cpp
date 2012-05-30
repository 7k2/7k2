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

// Filename    : OTOWNDEF.CPP
// Description : town defense

#include <otown.h>
#include <all.h>
#include <of_camp.h>
#include <ounit.h>
#include <orebel.h>

//--------- Begin of function Town::mobilize_defender -------//
//
// <int> attackerNationRecno - recno of the attacking nation.
//
// return: <int> the recno of the new defending unit.
//					  0 if no defending unit is mobilized.
//
int Town::mobilize_defender(int attackerNationRecno)
{
	if( population<=1 )		// do not call out defenders any more if there is only one unit left in the town, otherwise the town will be gone.
		return 0;

	//---- check if the current loyalty allows additional defender ----//
	//
	// if the loyalty is high, then there will be more town defenders
	//
	//-----------------------------------------------------------------//

	float curLoyalty;

	if( nation_recno )
	{
		curLoyalty = loyalty;
	}
	else
	{
		if( !attackerNationRecno )		// if independent units do not attack independent towns
			return 0;

		curLoyalty = resistance(attackerNationRecno);
	}

	//--- only mobilize new defenders when there aren't too many existing ones ---//

	if( rebel_recno )		// if this town is controlled by rebels
	{
		if( curLoyalty	< town_defender_count*2 )		// rebel towns have more rebel units coming out to defend
			return 0;
	}
	else
	{
		if( curLoyalty	< town_defender_count*5 )		// if no more defenders are allowed for the current loyalty
			return 0;
	}

	//----- check if the loyalty/resistance is high enough -----//

	if( nation_recno )
	{
		if( curLoyalty < MIN_NATION_DEFEND_LOYALTY )
			return 0;
	}
	else
	{
		if( curLoyalty < MIN_INDEPENDENT_DEFEND_LOYALTY )
			return 0;
	}

	//------ check if there are peasants to defend ------//

	if( recruitable_pop(0) == 0 ) 	// 0-don't recruit spies
		return 0;

	//---------- create a defender unit --------------------//

	//--------------------------------------------------------------//
	//								loyalty/resistance
	// decrease loyalty by: ------------------ x 2
	//								 town population
	//--------------------------------------------------------------//

	float loyaltyDec = 2 * curLoyalty / population;  // decrease in loyalty or resistance

	if( nation_recno )
	{
		change_loyalty( -loyaltyDec );
	}
	else
	{
		err_when( attackerNationRecno<1 || attackerNationRecno>MAX_NATION );

		resistance_array[attackerNationRecno-1] -= loyaltyDec;
	}

	//------- mobilize jobless people if there are any -------//

	int unitRecno = mobilize_town_people(1, 0);		// 1-dec pop, 0-don't mobilize spy town people

	Unit* unitPtr = unit_array[unitRecno];

	err_when( town_array.is_deleted(town_recno) );

	unitPtr->set_mode( UNIT_MODE_TOWN_DEFENDER, town_recno );

	unitPtr->skill.set_skill_level( (int) loyaltyDec );	// if the unit is a town defender, this var is temporary used for storing the loyalty that will be added back to the town if the defender returns to the town

	int combatLevel = town_combat_level + m.random(20) - 10;		// -10 to +10 random difference

	combatLevel = MIN(combatLevel, 100);
	combatLevel = MAX(combatLevel, 10);

	unitPtr->set_combat_level(combatLevel);

	// ###### patch begin Gilbert 29/3 ######//
	// mark the attacker nation recno, when unit back to town
	// town increases resistance
	if( !nation_recno )
		last_attack_nation_recno = attackerNationRecno;
	// ###### patch end Gilbert 29/3 ######//

	town_defender_count++;

	//------- if this town is controlled by rebels --------//

	if( rebel_recno )
		rebel_array[rebel_recno]->mobile_rebel_count++;		// increase the no. of mobile rebel units

	return unitRecno;
}
//---------- End of function Town::mobilize_defender -----//


//--------- Begin of function Town::reduce_defender_count -------//

void Town::reduce_defender_count()
{
	if( --town_defender_count==0 )
		independ_town_nation_relation = 0;

	//------- if this town is controlled by rebels --------//

	if( rebel_recno )
		rebel_array[rebel_recno]->mobile_rebel_count--;		// decrease the no. of mobile rebel units
}
//---------- End of function Town::reduce_defender_count -----//

