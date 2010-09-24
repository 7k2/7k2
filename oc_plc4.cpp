// Filename    : OC_PLC4.CPP
// Description : campaign plot c4

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
#include <OSPY.H>


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
			for( int n = 1; n <= nation_array.size(); ++n )
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

