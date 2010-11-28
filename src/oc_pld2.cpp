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

// Filename    : OC_PLD2.CPP
// Description : campaign plot d2

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


/*
The invading force causes chaos in your empire by using items to
turn to your towns into independent ones. The ensuing civil war
weakens the strength of your kingdom and makes it much more
vulnerable to the invasion. You must do whatever you can to
prevent the state from falling into the hand of the invaders
*/



//---- Begin of function CampaignEastWest::plot_d2_create_game ----//

void CampaignEastWest::plot_d2_create_game()
{
	// find a non-own nation
	int srcNation;
	for( srcNation = nation_array.size(); srcNation > 0; --srcNation)
	{
		if( !nation_array.is_deleted(srcNation)
			&& !nation_array[srcNation]->is_own() )
		{
			break;
		}
	}

	//
	int unitId;
	short kingXLoc = -1, kingYLoc;
	if(srcNation)
	{
		Nation *nationPtr = nation_array[srcNation];
		if( nationPtr->is_human() )
			unitId = race_res[nationPtr->race_id]->infantry_unit_id;
		else
			unitId = monster_res[nationPtr->monster_id()]->unit_id;

		if( nationPtr->king_unit_recno )
		{
			unit_array[nationPtr->king_unit_recno]->get_cur_loc( kingXLoc, kingYLoc);
		}
	}
	else
		unitId = 0;


	Town *targetTown;

	// find a town and turn it independent

	int count = 1;
	for(int townRecno = 1; townRecno <= town_array.size() && count > 0; ++townRecno )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		targetTown = town_array[townRecno];
		if( targetTown->is_own() )
		{
			targetTown->set_nation(0);

			// ----- create a monster next to town -------//

			if( unitId && kingXLoc >= 0)
			{
				int unitRecno = targetTown->create_unit(unitId);
				if( unitRecno )
				{
					unit_array[unitRecno]->betray(srcNation);
					unit_array[unitRecno]->move(kingXLoc, kingYLoc, true);
				}
			}

			--count;
		}
	}
}
//---- End of function CampaignEastWest::plot_d2_create_game ----//


//---- Begin of function CampaignEastWest::plot_d2_next_day ----//

void CampaignEastWest::plot_d2_next_day()
{
}
//---- End of function CampaignEastWest::plot_d2_next_day ----//
