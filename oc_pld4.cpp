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

// Filename    : OC_PLd4.CPP
// Description : campaign plot d4

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


/*
Protect a town of yours until your reinforcement comes. Your city is poorly 
defended in the beginning and the attacking force are much more powerful
than your defending force. You must hold off the attackers and await the
arrival of your reinforcement. The objective is to destroy the enemy.
*/




//---- Begin of function CampaignEastWest::plot_d4_create_game ----//

void CampaignEastWest::plot_d4_create_game()
{
	// find place for reinforcement, place farthest away from your towns

	const searchAreaWidth = 8;
	const searchAreaHeight = 8;

	int avgDist = 0;
	int countDist = 0;

	int trial;

	// maximize the distance between all towns

	int bestXLoc, bestYLoc;
	int bestSumDist2 = 0;

	for( trial = 100; trial > 0; --trial )
	{
		int xLoc = m.random(MAX_WORLD_X_LOC - searchAreaWidth);
		int yLoc = m.random(MAX_WORLD_Y_LOC - searchAreaHeight);

		if( !world.check_unit_space(xLoc, yLoc, xLoc+searchAreaWidth-1, yLoc+searchAreaHeight-1) )
			continue;

		int sumDist2 = 0;

		// find avg distance between each town

		for(int t = 1; t <= town_array.size(); ++t )
		{
			if( town_array.is_deleted(t) )
				continue;

			Town *townPtr = town_array[t];

			if( townPtr->is_own() )
			{
				int d = m.points_distance( townPtr->center_x, townPtr->center_y, xLoc+searchAreaWidth/2, yLoc+searchAreaHeight/2 );
				sumDist2 += d*d;
			}
		}

		if( sumDist2 > bestSumDist2 )
		{
			bestSumDist2 = sumDist2;
			bestXLoc = xLoc;
			bestYLoc = yLoc;
		}
	}

	// ----- add reinforcement to -----------//

	if( bestSumDist2 > 0 )
	{
		// create two groups

		create_unit_group(nation_array.player_recno, MAX_TEAM_MEMBER, 0, 0, 0, bestXLoc, bestYLoc);
		create_unit_group(nation_array.player_recno, MAX_TEAM_MEMBER, 0, 0, 0, bestXLoc, bestYLoc);
	}
}
//---- End of function CampaignEastWest::plot_d4_create_game ----//


//---- Begin of function CampaignEastWest::plot_d4_next_day ----//

void CampaignEastWest::plot_d4_next_day()
{
}
//---- End of function CampaignEastWest::plot_d4_next_day ----//
