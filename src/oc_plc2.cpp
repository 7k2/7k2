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

// Filename    : OC_PLC2.CPP
// Description : campaign plot c2

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <oraceres.h>
#include <onews.h>
#include <otalkres.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <itemid.h>
#include <oitemres.h>
#include <opower.h>


/*
You start with units carrying items capable of turning an enemy
independent. The enemy is powerful and has a number of highly
populated towns linked with a network of economic and military
facilities. You start with a small troop. You must cause chaos
in the enemy towns by using items to incite revolutions.
*/


//---- Begin of function CampaignEastWest::plot_c2_create_game ----//

void CampaignEastWest::plot_c2_create_game()
{
	// ------ other item ITEM_TOWN_FREE randomly on the map -------//

	power.reset_selection();

	const maxItemCount = 3;
	int itemCreated = maxItemCount;

	for( int i = 1; itemCreated > 0 && i <= unit_array.size(); ++i )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit *unitPtr = unit_array[i];

		if( unitPtr->is_own() && !unitPtr->item.id
			&& item_res.can_pick(ITEM_TOWN_FREE, i) )
		{
			unitPtr->item.set(ITEM_TOWN_FREE, item_res.random_para(ITEM_TOWN_FREE, m.rand()));

			// -------- select the unit ------//
			unitPtr->select( itemCreated == maxItemCount );

			--itemCreated;
		}
	}
}
//---- End of function CampaignEastWest::plot_c2_create_game ----//


//---- Begin of function CampaignEastWest::plot_c2_next_day ----//
//
void CampaignEastWest::plot_c2_next_day()
{
}
//---- End of function CampaignEastWest::plot_c2_next_day ----//

