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

// Filename    : OC_PLA6.CPP
// Description : sub-plot 6


#include <oc_east.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ofirmres.h>
#include <otechres.h>
#include <otech.h>
#include <onationa.h>
#include <onation2.h>
#include <osite.h>
#include <itemid.h>
#include <oitemres.h>


/*
There are indepedent towns with high population but strong resistance in the southern
part of the map. The base of the Fryhtans are in the northern part of the map. Your troop
starts in the middle of the map and you have items capable of turning towns to yours in
the beginning. You must use the items to turn the indepedent towns into yours and develop
a strong military base to help you battle the Fryhtans
*/

void CampaignEastWest::plot_a6_create_game()
{
	// the largest independent population town

	const maxSelectedTownCount = 3;
	int selectedTownCount = 0;
	int townRecnoArray[maxSelectedTownCount];

	for( int i = 1; i <= town_array.size(); ++i )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town *townPtr = town_array[i];

		if( townPtr->nation_recno )		// independent
			continue;

		int r;
		if( selectedTownCount < maxSelectedTownCount )
		{
			townRecnoArray[selectedTownCount] = i;
		}
		else if( (r = m.random(selectedTownCount+1)) < maxSelectedTownCount )
		{
			// replace with any 
			townRecnoArray[r] = i;
		}
		selectedTownCount++;
	}

	if( selectedTownCount > maxSelectedTownCount )
		selectedTownCount = maxSelectedTownCount;

	// ---- add resistance and population of the selected towns -------//

	for( i = 0 ; i < selectedTownCount; ++i )
	{
		Town *townPtr = town_array[townRecnoArray[i]];

		int newPop = townPtr->population + 20 + m.random(20);
		if( newPop > MAX_TOWN_POPULATION )
			newPop = MAX_TOWN_POPULATION;

		for( int j = newPop - townPtr->population; j > 0; --j )
		{
			townPtr->inc_pop( 50 );
		}
		townPtr->change_resistance( nation_array.player_recno, 30.0f );
	}

	// ------ other item ITEM_TOWN_CHANGE_NATION randomly on the map -------//

	int itemToCreate = 0;
	int nearDist[maxSelectedTownCount];
	int itemX[maxSelectedTownCount];
	int itemY[maxSelectedTownCount];

	short kingX, kingY;

	if( !unit_array[(~nation_array)->king_unit_recno]->get_cur_loc(kingX, kingY) )
		return;

	int trial = 200;

	// find the nearest 3 location close to the king

	while( --trial > 0 )
	{
		int xLoc = m.random(MAX_WORLD_X_LOC);
		int yLoc = m.random(MAX_WORLD_Y_LOC);
		int dist = m.points_distance( xLoc, yLoc, kingX, kingY );

		if( dist >= 10 &&	world.can_build_site( xLoc, yLoc, 1, 1, 3) )		// some space there
		{
			// avoid too close with other itemX/Y

			int minItemDist = MAX_WORLD_X_LOC + MAX_WORLD_Y_LOC;
			for( int j = 0; j < itemToCreate; ++j )
			{
				int itemDist = m.points_distance( xLoc, yLoc, itemX[j], itemY[j]);
				if( itemDist < minItemDist )
					minItemDist = itemDist;
			}

			if( minItemDist < 10 )
				continue;			// reject this xLoc/yLoc

			// add to nearDist[], itemX[], itemY[];

			for( j = 0; j < itemToCreate; ++j )
			{
				if( dist < nearDist[j] )
					break;
			}

			if( j < maxSelectedTownCount )
			{
				// shift record back
				if( j+1 < maxSelectedTownCount )
				{
					memmove( &nearDist[j+1], &nearDist[j], (char *)&nearDist[maxSelectedTownCount] - (char *)&nearDist[j+1] );
					memmove( &itemX[j+1], &itemX[j], (char *)&itemX[maxSelectedTownCount] - (char *)&itemX[j+1] );
					memmove( &itemY[j+1], &itemY[j], (char *)&itemY[maxSelectedTownCount] - (char *)&itemY[j+1] );
				}

				itemX[j] = xLoc;
				itemY[j] = yLoc;
				nearDist[j] = dist;
				
				if( itemToCreate < maxSelectedTownCount )
					++itemToCreate;
			}
		}
	}

	for( i = 0; i < itemToCreate; ++i )
	{
		site_array.add_site( itemX[i], itemY[i], SITE_ITEM, ITEM_TOWN_CHANGE_NATION, item_res.random_para(ITEM_TOWN_CHANGE_NATION, m.rand()) );
	}
}


void CampaignEastWest::plot_a6_next_day()
{
}
