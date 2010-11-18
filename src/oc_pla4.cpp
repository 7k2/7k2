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

// Filename    : OC_PLA4.CPP
// Description : sub-plot 4

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
#include <of_camp.h>


/*
There is an area rich with natural resources afar from you. There is
a big Fryhtan presence in land and a smaller one on the ohter part 
of the land. The smaller Fryhtan presence guards the entrance to the
natural resources. You start with a troop and limited resources and
you must pass through the smaller Fryhtan presence to build mines on
the resources, develop your military and ultimately defeat the bigger
Fryhtan. The two Fryhtan presence can be of the same kingdom.
*/


void CampaignEastWest::plot_a4_create_game()
{
	plot_nation_recno1 = nation_array.new_nation( NATION_MONSTER, NATION_AI, -MONSTER_MINOTAUROS, nation_array.random_unused_color() );
	Nation* minotaurNation 	= nation_array[plot_nation_recno1];

	// -------- give tech to gold and tech to live point tech -------//

	tech_res.inc_tech_class_level( plot_nation_recno1, TECH_CLASS_ALCHEMY );

	// -------- create main lair --------//

	for( int i = 1; i <= site_array.size(); ++i )
	{
		if( site_array.is_deleted(i) )
			continue;

		Site *sitePtr = site_array[i];

		if( sitePtr->site_type != SITE_RAW || sitePtr->has_mine )
			continue;

		// -------- create alchemy tor at each mine --------//

		if( world.can_build_firm(sitePtr->map_x_loc, sitePtr->map_y_loc, FIRM_ALCHEMY) )
		{
			int firmRecno = firm_array.generate_firm(sitePtr->map_x_loc, sitePtr->map_y_loc, plot_nation_recno1, FIRM_ALCHEMY, monster_res[MONSTER_MINOTAUROS]->firm_build_code );

			//--------- create lair near alchemy tor -----------//

			Firm *minePtr;
			short buildXLoc, buildYLoc;
			if( firmRecno && (minePtr = firm_array[firmRecno] )
				&& minotaurNation->find_best_place_loc(FIRM_LAIR, minePtr->loc_x1, minePtr->loc_y1, buildXLoc, buildYLoc) )
			{
				create_monster_lair( buildXLoc, buildYLoc, plot_nation_recno1, 
					MAX_SOLDIER - m.random(MAX_SOLDIER/2), 20 + campaign_difficulty*10 );
			}
		}
	}
}


void CampaignEastWest::plot_a4_next_day()
{
}

