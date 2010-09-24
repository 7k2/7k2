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

// Filename    : OC_PLA3.CPP
// Description : sub-plot 3

#include <OC_EAST.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OFIRMRES.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <ONATIONA.H>
#include <ONATION2.H>
#include <OF_BASE.H>
#include <OTALKRES.H>
#include <OGODRES.H>


/*
There is a tribe of Viking in the northwestern part of the map.
The land is being terrorized by Fryhtans. (the Fryhtans are at the
center of the map.) The Viking tribe possess 5 Viking Seat of Power
which are all filled with Viking peasants. The tribe will declare
loyalty to you if you elimiate their rival tribe Mongol, who is
located in the southwestern part of the map. You start at the east
of the map. The strategy is to help the Viking to eliminate the
Mongol and have them turn towards you. Using their seat of power,
you can invoke Viking Greater Beings to help you destroy the 
Fryhtans. The objective is to destroy the Fryhtans.
*/


void CampaignEastWest::plot_a3_create_game()
{
	// one viking nation and one mongol nation

	plot_nation_recno1 = nation_array.new_nation( NATION_HUMAN, NATION_AI, RACE_VIKING, nation_array.random_unused_color() );
	god_res[GOD_VIKING]->enable_know(plot_nation_recno1);

	plot_nation_recno2 = nation_array.new_nation( NATION_HUMAN, NATION_AI, RACE_MONGOL, nation_array.random_unused_color() );

	// create viking race

	{
		Nation* vikingNation = nation_array[plot_nation_recno1];

		//---- create towns for the rebel nation ----//

		int townAddCount   = 3;
		int hasFortPercent = 100;

		create_town( plot_nation_recno1, townAddCount, hasFortPercent );

		int townRecno = random_pick_town_with_camp(plot_nation_recno1, 2);		// only pick towns with <= 2 links

		// change town population to 100

		if( townRecno )
		{
			town_array[townRecno]->init_pop( MAX_TOWN_POPULATION, 100 );

			create_economic_firm(townRecno);
      }

		// -------- build 4 viking seat of power -------//

		int trial = 100;
		int seatCount = 4;

		for( ; trial > 0 && seatCount > 0; --trial )
		{
			if( (townRecno = random_pick_town_with_camp(plot_nation_recno1, 8)) )		// only pick town with <= 8 links
			{
				int firmRecno;
				if( (firmRecno = create_firm_next_to_place( town_array[townRecno], FIRM_BASE, vikingNation->race_id)) )
				{
					FirmBase *firmPtr = firm_array[firmRecno]->cast_to_FirmBase();

					if( firmPtr )
					{
						firmPtr->set_needed_worker_count( MAX_WORKER, COMMAND_AUTO );
					}
					--seatCount;
				}
			}
		}

		//--- hard-wire the viking 's relation with the Fryhtan ---//

		static short neverConsiderArray1[] =
		{
			TALK_PROPOSE_TRADE_TREATY,
			TALK_PROPOSE_FRIENDLY_TREATY,
			TALK_PROPOSE_ALLIANCE_TREATY,
			0
		};

		vikingNation->get_relation(plot_enemy_nation_recno)->set_ai_never_consider(neverConsiderArray1);

		//--- hard-wire the viking's relation with the player ---//

		static short neverConsiderArray2[] =
		{
			TALK_PROPOSE_ALLIANCE_TREATY,
			TALK_DECLARE_WAR,
			TALK_SURRENDER,
			0
		};

		vikingNation->get_relation(nation_array.player_recno)->set_ai_never_consider(neverConsiderArray2);

		//--- hard-wire the viking's relation with the mongol nation ---//

		static short neverConsiderArray3[] =
		{
			TALK_PROPOSE_TRADE_TREATY,
			TALK_PROPOSE_FRIENDLY_TREATY,
			TALK_PROPOSE_ALLIANCE_TREATY,
			TALK_SURRENDER,
			0
		};

		vikingNation->get_relation(plot_nation_recno2)->set_ai_never_consider(neverConsiderArray3);
	}

	// create mongol race

	{
		Nation* mongolNation = nation_array[plot_nation_recno2];

		//---- create towns for the rebel nation ----//

		int townAddCount   = 2 + m.random(2);		// 2 to 3 towns
		int hasFortPercent = 100;

		create_town( plot_nation_recno2, townAddCount, hasFortPercent );

		int townRecno1 = king_oversee_town_recno(plot_nation_recno2);
		int townRecno2 = random_pick_town_with_camp(plot_nation_recno2, 2);		// only pick towns with <= 2 links

		if( !create_military_firm(townRecno1) )
			return;

		if( townRecno2 && !create_economic_firm(townRecno2) )
			return;

		//--- hard-wire the mongol 's relation with the Fryhtan ---//

		static short neverConsiderArray1[] =
		{
			TALK_PROPOSE_TRADE_TREATY,
			TALK_PROPOSE_FRIENDLY_TREATY,
			TALK_PROPOSE_ALLIANCE_TREATY,
			0
		};

		mongolNation->get_relation(plot_enemy_nation_recno)->set_ai_never_consider(neverConsiderArray1);

		//--- hard-wire the rebel's relation with the player ---//

		static short neverConsiderArray2[] =
		{
			TALK_PROPOSE_ALLIANCE_TREATY,
			TALK_DECLARE_WAR,
			TALK_SURRENDER,
			0
		};

		mongolNation->get_relation(nation_array.player_recno)->set_ai_never_consider(neverConsiderArray2);

		//--- hard-wire the mongol 's relation with the viking nation ---//

		static short neverConsiderArray3[] =
		{
			TALK_PROPOSE_TRADE_TREATY,
			TALK_PROPOSE_FRIENDLY_TREATY,
			TALK_PROPOSE_ALLIANCE_TREATY,
			TALK_SURRENDER,
			0
		};

		mongolNation->get_relation(plot_nation_recno1)->set_ai_never_consider(neverConsiderArray3);
	}

	// disable know of the nation
	// such that no scroll left after either king is killed
	god_res[GOD_VIKING]->disable_know(plot_nation_recno1);

}


void CampaignEastWest::plot_a3_next_day()
{
	// viking kingdom survive and mongol kingdom destroyed
	if( !nation_array.is_deleted(plot_nation_recno1)
		&& nation_array.is_deleted(plot_nation_recno2) )
	{
		// fryhtan is not defeated

		if( !nation_array.is_deleted(plot_enemy_nation_recno) )
		{
			NationRelation* nationRelation = nation_array[plot_nation_recno1]->get_relation(nation_array.player_recno);

			short resetWStr[] = { TALK_PROPOSE_ALLIANCE_TREATY, 0 };
			nationRelation->reset_ai_never_consider(resetWStr);
			short setWStr[] = { TALK_END_ALLIANCE_TREATY, 0 };
			nationRelation->set_ai_never_consider(setWStr);

			// if alliance is set

			if( nationRelation->status == RELATION_ALLIANCE )
			{
				// enhance ai relation so it can accept player request declare war with frythan

				nation_array[plot_nation_recno1]->
					change_ai_relation_level(nation_array.player_recno, +100 );

				// hardware not to cease fire with fryhtan kingdom

				short resetWStr2[] = { TALK_REQUEST_CEASE_WAR, 0 };
				nation_array[plot_nation_recno1]->get_relation(plot_enemy_nation_recno)
					->set_ai_never_consider( resetWStr2 );
			}

			// propose an alliance treaty

			else if( info.game_date % 15 == 0 )
			{
				if( nationRelation->status == RELATION_FRIENDLY
					|| nationRelation->status == RELATION_NEUTRAL)
				{
					talk_res.ai_send_talk_msg( nation_array.player_recno,
						plot_nation_recno1, TALK_PROPOSE_ALLIANCE_TREATY );
				}
			}
		}

		// fryhtan is defeated

		else
		{
			NationRelation* nationRelation = nation_array[plot_nation_recno1]->get_relation(nation_array.player_recno);

			nationRelation->ai_never_consider[TALK_SURRENDER-1] = 0;

			talk_res.ai_send_talk_msg( nation_array.player_recno,
				plot_nation_recno1, TALK_SURRENDER );

			plot_nation_recno1 = 0;
		}
	}
}

