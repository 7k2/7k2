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

//Filename	 : OFIRMAT.CPP
//Description: Firm attacking and being attacked functions

#include <OSERES.H>
#include <ONEWS.H>
#include <OBULLET.H>
#include <ONATION.H>
#include <OF_LAIR.H> 


//--------- Begin of function Firm::being_attack_hit --------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
// <float>	  damagePoint - the points of damage made.
//
void Firm::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	//---- call base class function ------//

	Place::being_attack_hit(attackerObj, damagePoint);

	//------ react by attacking the attacker ----//

//	if( can_attack() )
//		auto_react_attack(attackerObj);
}
//---------- End of function Firm::being_attack_hit ----------//


//------- Begin of function Firm::being_killed ------//
//
// <BaseObj*> attackerObj - this can be NULL if the attacker no longer
//									 exists (the damage is caused by a bullet.)
//
void Firm::being_killed(BaseObj* attackerObj)
{
	se_res.sound(center_x, center_y, 1, 'F', firm_id, "DIE" );

	if( nation_recno == nation_array.player_recno && attackerObj )		//BUGHERE
		news_array.firm_destroyed(firm_recno, attackerObj);

	// ######## begin Gilbert 17/6 ########//
	if( nation_recno == 0 && firm_id == FIRM_LAIR && is_monster() )
	{
		news_array.monster_firm_destroyed( monster_id(), center_x, center_y );
	}
	// ######## end Gilbert 17/6 ########//

	if( nation_recno )
	{
		if( attackerObj && attackerObj->nation_recno )
			nation_array[attackerObj->nation_recno]->enemy_firm_destroyed++;

		if( nation_recno )
			nation_array[nation_recno]->own_firm_destroyed++;
	}

	//-----------------------------------------//

	if( attackerObj && attackerObj->nation_recno )
	{
		Nation* attackerNation = nation_array[attackerObj->nation_recno];

		//-- destroying a monster firm raise the attacking nation's reputation --//

		if( is_monster() )
		{
			float repIncrease = (float) max_hit_points() / 600;

			//-- if the lair is enslaving the towns, increase the reputation bonus --//

			if( cast_to_FirmLair() )
			{
				int tributeAmount = cast_to_FirmLair()->collect_town_tribute(0);

				repIncrease += (float) tributeAmount / 600;
			}

			attackerNation->change_reputation(repIncrease);

			//--- when destroying an enslaving Fryhtan lair, the independent towns enslaved by it will reduce its resistance towards you by 30 points ---//

			if( cast_to_FirmLair() )
			{
				int   i;
				Town* townPtr;

				for( i=0 ; i<linked_town_count ; i++ )
				{
					if(town_array.is_deleted(linked_town_array[i]))
						continue;

					townPtr = town_array[linked_town_array[i]];

					//--- if it is a linked independent town ---//

					if( townPtr->nation_recno == 0 &&
						 // ####### begin Gilbert 9/3 ########//
						 nation_recno &&
						 // ####### end Gilbert 9/3 ########//
						 townPtr->resistance(nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
					{
						townPtr->change_resistance(attackerObj->nation_recno, -30);
					}
				}
			}
		}

		//------ destroyng a building gives money ------//

		float killMoney = (float) (firm_res[firm_id]->setup_cost + firm_recno%100) / 3;		// add some randomness

		attackerNation->add_income( INCOME_TREASURE, killMoney );
		attackerNation->increased_cash = killMoney;
	}

	firm_array.del_firm(firm_recno);
}
//--------- End of function Firm::being_killed -------//

