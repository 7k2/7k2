// Filename    : OC_PLC2.CPP
// Description : campaign plot c2

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
You start with units carrying items capable of turning an enemy
independent. The enemy is powerful and has a number of highly
populated towns linked with a network of economic and military
facilities. You start with a small troop. You must cause chaos
in the enemy towns by using items to incite revolutions.
*/


//---- Begin of function CampaignEastWest::plot_c2_create_game ----//

void CampaignEastWest::plot_c2_create_game()
{
	// ------ other item ITEM_TOWN_FREE randomly on the map -------//

	power.reset_selection();

	const maxItemCount = 3;
	int itemCreated = maxItemCount;

	for( int i = 1; itemCreated > 0 && i <= unit_array.size(); ++i )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit *unitPtr = unit_array[i];

		if( unitPtr->is_own() && !unitPtr->item.id
			&& item_res.can_pick(ITEM_TOWN_FREE, i) )
		{
			unitPtr->item.set(ITEM_TOWN_FREE, item_res.random_para(ITEM_TOWN_FREE, m.rand()));

			// -------- select the unit ------//
			unitPtr->select( itemCreated == maxItemCount );

			--itemCreated;
		}
	}
}
//---- End of function CampaignEastWest::plot_c2_create_game ----//


//---- Begin of function CampaignEastWest::plot_c2_next_day ----//
//
void CampaignEastWest::plot_c2_next_day()
{
}
//---- End of function CampaignEastWest::plot_c2_next_day ----//

