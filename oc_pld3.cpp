// Filename    : OC_PLD3.CPP
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
#include <OITEM.H>
#include <ITEMID.H>
#include <OFIRM.H>
#include <OF_CAMP.H>


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
	const period = 4;

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
