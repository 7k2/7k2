// Filename    : OC_PLD2.CPP
// Description : campaign plot d2

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
	for( int srcNation = nation_array.size(); srcNation > 0; --srcNation)
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
