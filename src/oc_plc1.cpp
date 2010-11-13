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

// Filename    : OC_PLC1.CPP
// Description : campaign plot c1

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OTALKRES.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <ITEMID.H>
#include <OITEMRES.H>
#include <OPOWER.H>


/*
Your units has the item to convert a unit to yours. (that means
you have serveral of this item.) You start with a small troop and
the enemy is far more powerful. You must use this item to convert an
enemy general and the troop led by him. You have little cash in this
game and there are no independent towns to assimilate.
*/


//---- Begin of function CampaignEastWest::plot_c1_create_game ----//

void CampaignEastWest::plot_c1_create_game()
{
	power.reset_selection();

	// create charm on any own unit

	const maxItemCount = 3;
	int itemCreated = maxItemCount;

	for( int i = 1; itemCreated > 0 && i <= unit_array.size(); ++i )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit *unitPtr = unit_array[i];

		if( unitPtr->is_own() && !unitPtr->item.id
			&& item_res.can_pick(ITEM_UNIT_CHANGE_NATION, i) )
		{
			unitPtr->item.set(ITEM_UNIT_CHANGE_NATION, item_res.random_para(ITEM_UNIT_CHANGE_NATION, m.rand()));

			// -------- select the unit ------//
			unitPtr->select( itemCreated == maxItemCount );

			--itemCreated;
		}
	}

	// --------- remove all independent town ---------//

	for( i = town_array.size(); i > 0; --i )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town *townPtr = town_array[i];

		if( townPtr->nation_recno == 0 )
		{
			town_array.del_town(i);
			continue;		// townPtr is invalide continue immediately
		}
	}
}
//---- End of function CampaignEastWest::plot_c1_create_game ----//


//---- Begin of function CampaignEastWest::plot_c1_next_day ----//
//
void CampaignEastWest::plot_c1_next_day()
{
}
//---- End of function CampaignEastWest::plot_c1_next_day ----//

