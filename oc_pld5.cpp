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

// Filename    : OC_PLD5.CPP
// Description : campaign plot d3

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
#include <OGODRES.H>


/*
Protect a seat of power from being destroyed by Fryhtan. This is the seat of power your
ancestor built and is of extreme importance to your kingdom and your people.
If this seat of power is destroyed, you will lose the game.
*/




//---- Begin of function CampaignEastWest::plot_d5_create_game ----//

void CampaignEastWest::plot_d5_create_game()
{
	// -------- build a seat of power -------//

	int trial = 100;
	int seatCount = 1;

	for( ; trial > 0 && seatCount > 0; --trial )
	{
		int townRecno;
		if( (townRecno = random_pick_town_with_camp( nation_array.player_recno, 8)) )		// only pick town with <= 8 links
		{
			int firmRecno;
			GodInfo *godInfo = god_res[town_array[townRecno]->race_id];
			int knowGodBefore = godInfo->is_nation_know(nation_array.player_recno);
			if( !knowGodBefore )
			{
				godInfo->enable_know(nation_array.player_recno);
			}

			if( (firmRecno = create_firm_next_to_place( town_array[townRecno], FIRM_BASE, town_array[townRecno]->race_id)) )
			{
				// ---------- add lose condition --------//

				game.add_lose_condition(E_DESTROY_FIRM, 1, firmRecno );

				--seatCount;
			}

			if( !knowGodBefore )
			{
				godInfo->disable_know(nation_array.player_recno);
			}
		}
	}
}
//---- End of function CampaignEastWest::plot_d5_create_game ----//


//---- Begin of function CampaignEastWest::plot_d5_next_day ----//

void CampaignEastWest::plot_d5_next_day()
{
}
//---- End of function CampaignEastWest::plot_d5_next_day ----//
