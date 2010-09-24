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

// Filename    : OC_PLC3.CPP
// Description : campaign plot c3

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
#include <OF_CAMP.H>
#include <OSPY.H>


/*
You have planted spies in enemy military structures and it this
scenario and most of them are in the rank of soldiers, you must
make use of them to bribe enemy generals to grow your spy
network. You spies reported thatn the enemy will launch a
large assault against one weakly defended state of yours in 3 
years. You must make 5 of your spies become generals of the enemy
kingdom so that your spies can switch side during the battle
and dynamically change the situation
*/


//---- Begin of function CampaignEastWest::plot_c3_create_game ----//

void CampaignEastWest::plot_c3_create_game()
{
	int spyProb = 100;

	for( int i = 1; i <= firm_array.size(); ++i )
	{
		if( firm_array.is_deleted(i) )
			continue;

		int spyRecno = 0;

		Firm *firmPtr = firm_array[i];

		// skip own or independent

		if( firmPtr->is_own() || firmPtr->nation_recno == 0 )
			continue;

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *camp = firmPtr->cast_to_FirmCamp();

			if( m.random(100) < spyProb )
			{
				for(int j = 0; j < camp->soldier_count; ++j )
				{
					Soldier *soldierPtr = camp->soldier_array + j;

					int unitRecno;
					if( soldierPtr->is_human() && !soldierPtr->spy_recno
						&& (unitRecno = camp->mobilize_soldier(j+1, COMMAND_AUTO)) )
					{
						spyRecno = unit_array[unitRecno]->spy_recno
							= spy_array.add_spy(unitRecno, 50+m.random(40), nation_array.player_recno);

						// go back to camp
						camp->assign_unit(unitRecno);
						break;		// must break because the 
					}
				}
			}
		}

		if( spyRecno )
		{
			spyProb -= 10;
		}
	}
}
//---- End of function CampaignEastWest::plot_c3_create_game ----//


//---- Begin of function CampaignEastWest::plot_c3_next_day ----//

void CampaignEastWest::plot_c3_next_day()
{
}
//---- End of function CampaignEastWest::plot_c3_next_day ----//
