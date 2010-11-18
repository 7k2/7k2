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

// Filename    : OC_PLD3.CPP
// Description : campaign plot d3

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
#include <oitem.h>
#include <itemid.h>
#include <ofirm.h>
#include <of_camp.h>


/*
There has been rumors about enemies with invisibility ability
getting close to your cities. Train and prepare your troops
well to fend them off if they do come.
*/



//---- Begin of function CampaignEastWest::plot_d3_create_game ----//

void CampaignEastWest::plot_d3_create_game()
{
}
//---- End of function CampaignEastWest::plot_d3_create_game ----//


//---- Begin of function CampaignEastWest::plot_d3_next_day ----//
//
void CampaignEastWest::plot_d3_next_day()
{
	const int period = 4;

	// whenever soldier enter lair, give/refill unseen torque soldier

	for( int i = info.game_date % period; i <= firm_array.size(); i += period )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm *firmPtr = firm_array[i];

		// non-independent enemy
		if( firmPtr->is_own() || firmPtr->nation_recno == 0 || !firmPtr->is_monster() )
			continue;

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *camp = firmPtr->cast_to_FirmCamp();

			// ----- give/refill general item ------//
			
			if( camp->overseer_recno )
			{
				Unit *general = unit_array[camp->overseer_recno];
				if( !general->item.id || general->item.id == ITEM_INVISIBILITY_SX )
				{
					general->item.set(ITEM_INVISIBILITY_SX, 100 );	// 100 days of invisibility
					// if turned on, switch if off automatically
				}
			}

			// ----- give/refill soldiers item -----//

			for( int j = 0; j < camp->soldier_count; ++j )
			{
				Soldier *soldierPtr = camp->soldier_array + j;
				if( soldierPtr->is_under_training() )
					continue;

				if( !soldierPtr->item.id || soldierPtr->item.id == ITEM_INVISIBILITY_SX )
				{
					soldierPtr->item.set(ITEM_INVISIBILITY_SX, 100 );	// 100 days of invisibility
					// if turned on, switch if off automatically
				}
			}
		}
	}
}
//---- End of function CampaignEastWest::plot_d3_next_day ----//
