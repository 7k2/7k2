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

// Filename    : OC_PLOT2.CPP
// Description : sub-plot 2

#include <oc_east.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ofirmres.h>
#include <otechres.h>
#include <otech.h>
#include <onationa.h>
#include <onation2.h>


/*
This is a land controlled by the Fryhtans. You have several military
Structures left after your last battle with Fryhtans in the beginning.
(In fact, this was original your land, the Fryhtan invades it and took
over it. Now you are going to get it back.) However, all of the 
structures are badly damaged. You must repair them and put them into
good use. You start with a troop in the beginning and the Fryhtans are
powerful. The objective is to destroy the Fryhtans.
*/


void CampaignEastWest::plot_a2_create_game()
{
	Nation *monsterNation = nation_array[plot_enemy_nation_recno];

	// give money

	monsterNation->cash += 20000.0f;
	monsterNation->food += 20000.0f;
	monsterNation->live_points += 20000.0f;

	// give tech

	for( int techClassId = FIRST_TECH_CLASS_FRYHTAN; techClassId <= LAST_TECH_CLASS_FRYHTAN; ++techClassId )
	{
		tech_res.inc_tech_class_level( plot_enemy_nation_recno, techClassId );
	}

/*
	// create military building

	int lairAddCount = 6 + misc.random(3);						// add 6 to 8 Fryhtan Lairs
	int independentTownAddCount = 6 + misc.random(3);		// 6 to 8 independent towns
	int specialFirmCount= lairAddCount - misc.random(3);

	create_lair( 0, plot_enemy_nation_recno, lairAddCount, independentTownAddCount, specialFirmCount);
*/

	// --- build military building around non-slave independent town of player but badly damage ----//

	long limitBestWeight = 0x7fffffff;

	for( int fortCount = 3; fortCount > 0; --fortCount )
	{
		int bestTownRecno = 0;
		int bestWeight = 0;
		for( int townRecno = town_array.size(); townRecno > 0; --townRecno )
		{
			if( town_array.is_deleted(townRecno) )
				continue;

			Town *townPtr = town_array[townRecno];
			if( townPtr->nation_recno == 0
				&& !townPtr->is_pay_tribute_to_monster() )
			{
				int weight = 1000 - unit_array[(~nation_array)->king_unit_recno]->area_distance(townPtr) / 8
					+ townPtr->population + (townPtr->race_id==(~nation_array)->race_id ? 20 : 0);
				if( weight > bestWeight && weight < limitBestWeight )
				{
					bestWeight = weight;
					bestTownRecno = townRecno;
				}
			}
		}

		if( !bestTownRecno )
			break;					// no need to find another town again
		else							
		{
			// a suitable town is found
			// ---- create fort -------//

			int firmRecno = create_firm_next_to_place( town_array[bestTownRecno], 
				(~nation_array)->is_human() ? FIRM_FORT : FIRM_LAIR,
				(~nation_array)->race_id, nation_array.player_recno );

			// --- weaken building of human ---------//

			if( firmRecno )
			{
				Firm *firmPtr = firm_array[firmRecno];

				firmPtr->hit_points = misc.random((int)firmPtr->hit_points) / 2.0f + 1.0f;
			}

			limitBestWeight = bestWeight;
			// no matter we can create a fort or not, set maxBestWeight
			// so next fortCount will not select the same town
			// if another having the same bestWeight, we may miss it
		}
	}
}


void CampaignEastWest::plot_a2_next_day()
{

	// give food, money and live point if lack

	if( !nation_array.is_deleted(plot_enemy_nation_recno))
	{
		Nation *monsterNation = nation_array[plot_enemy_nation_recno];

		if( monsterNation->cash < 1000.0f )
			monsterNation->add_income(INCOME_CHEAT, 10.0f, 1);
		else if( monsterNation->cash < 5000.0f )
			monsterNation->add_income(INCOME_CHEAT, 2.0f, 1);

		if( monsterNation->food < 1000.0f )
			monsterNation->add_food( 10.0f );
		else if( monsterNation->food < 5000.0f )
			monsterNation->add_food( 2.0f );

		if( monsterNation->live_points < 1000.0f )
			monsterNation->change_live_points( 10.0f );
		else if( monsterNation->live_points < 5000.0f )
			monsterNation->change_live_points( 2.0f );
	}
}

