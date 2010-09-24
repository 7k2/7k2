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

// Filename    : OC_PLD1.CPP
// Description : campaign plot d1

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
#include <OF_MINE.H>
#include <OF_ALCH.H>


/*
You have a strong military presence in the scenario. Unforturnately,
you don't have the sufficient economic facilites to provide you with
the income to pay the upkeep costs of the military force. Now the
enemy has embarked an invasion against you and they are coming. The
loyalty of your generals and soldiers continue to drop as you fail
to pay them salaries. (Solution to this: there are ample natural
resources at the southern part of the map. You must mine them to
increase your revenue. At the same time shut down some of the
existing buildings to save costs.) The objective is to completely
fend off the invading force
*/



//---- Begin of function CampaignEastWest::plot_d1_create_game ----//

void CampaignEastWest::plot_d1_create_game()
{
	// reduce cash

	(~nation_array)->cash = (~nation_array)->cash / 4;

	// reduce mineral if the mine is owned by the player

	for( int i = 1; i <= site_array.size(); ++i )
	{
		if( site_array.is_deleted(i) )
			continue;

		Site *sitePtr = site_array[i];
		if( sitePtr->site_type == SITE_RAW && sitePtr->has_mine )
		{
			Location *locPtr = world.get_loc(sitePtr->map_x_loc, sitePtr->map_y_loc);
			err_when( !locPtr->is_firm() );
			Firm *firmPtr = firm_array[locPtr->firm_recno()];
			if( firmPtr->is_own() )
			{
				sitePtr->reserve_qty = sitePtr->reserve_qty / 10;

				if( firmPtr->cast_to_FirmMine() )
				{
					firmPtr->cast_to_FirmMine()->reserve_qty = (float)sitePtr->reserve_qty;
				}
				else if( firmPtr->cast_to_FirmMonsterAlchemy() )
				{
					firmPtr->cast_to_FirmMonsterAlchemy()->reserve_qty = (float)sitePtr->reserve_qty;
				}
			}
		}
	}
}
//---- End of function CampaignEastWest::plot_d1_create_game ----//


//---- Begin of function CampaignEastWest::plot_d1_next_day ----//
//
void CampaignEastWest::plot_d1_next_day()
{
}
//---- End of function CampaignEastWest::plot_d1_next_day ----//
