//Filename   : OC_PLOT1.CPP
//Description: CampaignEastWest - PLOT_REBEL_JOIN_FORCE

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
#include <OF_CAMP.H>


//---- Begin of function CampaignEastWest::plot_a1_create_game ----//

void CampaignEastWest::plot_a1_create_game()
{
	//------- create the rebel nation -------//

	int raceId = m.random(MAX_RACE)+1;

	plot_nation_recno1 = nation_array.new_nation( NATION_HUMAN, NATION_AI, raceId, nation_array.random_unused_color() );

	Nation* rebelNation 	= nation_array[plot_nation_recno1];

	//---- create towns for the rebel nation ----//

	int townAddCount   = 2 + m.random(2);		// 2 to 3 towns
	int hasFortPercent = 100;

	create_town( plot_nation_recno1, townAddCount, hasFortPercent );

	int townRecno1 = king_oversee_town_recno(plot_nation_recno1);
	int townRecno2 = random_pick_town_with_camp(plot_nation_recno1, 2);		// only pick towns with <= 2 links

	if( !create_military_firm(townRecno1) )
		return;

	if( townRecno2 && !create_economic_firm(townRecno2) )
		return;

	//--- hard-wire the rebel's relation with the Fryhtan ---//

	static short neverConsiderArray1[] =
	{
		TALK_PROPOSE_TRADE_TREATY,
		TALK_PROPOSE_FRIENDLY_TREATY,
		TALK_PROPOSE_ALLIANCE_TREATY,
		0
	};

	rebelNation->get_relation(plot_enemy_nation_recno)->set_ai_never_consider(neverConsiderArray1);

	//--- hard-wire the rebel's relation with the player ---//

	static short neverConsiderArray2[] =
	{
		TALK_PROPOSE_ALLIANCE_TREATY,
		TALK_DECLARE_WAR,
		TALK_SURRENDER,
		0
	};

	rebelNation->get_relation(nation_array.player_recno)->set_ai_never_consider(neverConsiderArray2);
}
//---- End of function CampaignEastWest::plot_a1_create_game ----//


//---- Begin of function CampaignEastWest::plot_a1_next_day ----//

void CampaignEastWest::plot_a1_next_day()
{
	if( plot_nation_recno1==0		// it has already surrendered.
		|| nation_array.is_deleted(plot_nation_recno1) )
		return;

	Nation* playerNation = ~nation_array;
	Nation* rebelNation 	= nation_array[plot_nation_recno1];

	NationRelation* nationRelation = rebelNation->get_relation(nation_array.player_recno);

	//--- the rebel will propose an alliance treaty to the player if the player's Fryhtan battle score > 100 ---//

	if( playerNation->kill_monster_score >= 100 &&
		 nationRelation->ai_never_consider[TALK_PROPOSE_ALLIANCE_TREATY-1] )		// check ai_never_cosinder[] so that the action of proposing an alliance treaty is only done once
	{
		nationRelation->ai_never_consider[TALK_PROPOSE_ALLIANCE_TREATY-1] = 0;

		static short neverConsiderArray[] =
		{
			TALK_END_TRADE_TREATY,
			TALK_END_FRIENDLY_TREATY,
			TALK_END_ALLIANCE_TREATY,
			0
		};

		nationRelation->set_ai_never_consider(neverConsiderArray);

		talk_res.ai_send_talk_msg(playerNation->nation_recno,
			plot_nation_recno1, TALK_PROPOSE_ALLIANCE_TREATY );
	}

	//-- if the player's Fryhtan battle score > 300 and is allied with the rebel, the rebel will surrender to the player --//

	if( playerNation->kill_monster_score >= 300 &&
		 nationRelation->status == RELATION_ALLIANCE )
	{
		nationRelation->ai_never_consider[TALK_SURRENDER-1] = 0;

		talk_res.ai_send_talk_msg(playerNation->nation_recno,
			plot_nation_recno1, TALK_SURRENDER );

		plot_nation_recno1 = 0;
	}
}
//---- End of function CampaignEastWest::plot_a1_next_day ----//

