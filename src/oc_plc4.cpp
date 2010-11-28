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

// Filename    : OC_PLC4.CPP
// Description : campaign plot c4

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
#include <ospy.h>


/*
You have spies with high spying skill and assassination ability
*/


//---- Begin of function CampaignEastWest::plot_c4_create_game ----//

void CampaignEastWest::plot_c4_create_game()
{
	int areaX1, areaY1, areaX2, areaY2;

	if( unit_array.is_deleted((~nation_array)->king_unit_recno) )
		return;

	Unit *kingPtr = unit_array[ (~nation_array)->king_unit_recno ];
	err_when( !kingPtr->is_visible() );

	int spyCount = m.random(3) + 4;
	int radius;
	for( radius = 6; spyCount > 0 && radius <= 20; (radius+=2) )
	{
		areaX1 = kingPtr->next_x_loc() - radius;
		if( areaX1 < 0 )
			areaX1 = 0;
		areaY1 = kingPtr->next_y_loc() - radius;
		if( areaY1 < 0 )
			areaY1 = 0;
		areaX2 = kingPtr->next_x_loc() + radius;
		if( areaX2 >= MAX_WORLD_X_LOC )
			areaX2 = MAX_WORLD_X_LOC-1;
		areaY2 = kingPtr->next_y_loc() + radius;
		if( areaY2 >= MAX_WORLD_Y_LOC )
			areaY2 = MAX_WORLD_Y_LOC-1;

		int trial = 10 + radius;
		for( trial = 10 + radius ; spyCount > 0 && trial > 0; --trial )
		{
			// randomly select race of an enemy human kingdom
			int nationRecnoArray[MAX_NATION];
			int nationRecnoCount = 0;
			int n;
			for( n = 1; n <= nation_array.size(); ++n )
			{
				if( nation_array.is_deleted(n) 
					|| n == nation_array.player_recno 
					|| !nation_array[n]->is_human() )
					continue;

				nationRecnoArray[nationRecnoCount] = n;
				nationRecnoCount++;
			}

			if( nationRecnoCount <= 0 )
				break;

			n = nationRecnoCount > 1 ? nationRecnoArray[m.random(nationRecnoCount)] : nationRecnoArray[0];

			// ----- create a unit of that race --------//

			int unitRecno = create_unit( nation_array.player_recno, race_res[nation_array[n]->race_id]->infantry_unit_id, RANK_SOLDIER,
				areaX1, areaY1, areaX2, areaY2 );
			if (unitRecno )
			{
				unit_array[unitRecno]->spy_recno = spy_array.add_spy( unitRecno, 50 + m.random(40));
				spyCount--;
			}
		}
	}
}
//---- End of function CampaignEastWest::plot_c4_create_game ----//



//---- Begin of function CampaignEastWest::plot_c4_next_day ----//

void CampaignEastWest::plot_c4_next_day()
{
}
//---- End of function CampaignEastWest::plot_c4_next_day ----//

