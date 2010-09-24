// Filename    : OC_PLA7.CPP
// Description : sub-plot 7


#include <OC_EAST.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OFIRMRES.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <ONATIONA.H>
#include <ONATION2.H>
#include <OTALKRES.H>
#include <OF_FORT.H>
#include <OF_SPEC.H>


/*
There is rebel group in the game. They are Carthaginians and own a
large number of elephant cavalry. Elephant cavalries fill their 
forts and there are three to five special unit training structures
keeping producing new Elephant cavalries. They are your alliance
and once your reputation reach 50, they all will turn towards you
(surrender to you in the game's sense.) You can accelerate the
increase of the reputation by killing Fryhtans. While you are
weak in the beginning, with the help of there rebels, you stand
a great chance at defeating the Fryhtans.
*/


void CampaignEastWest::plot_a7_create_game()
{
	//------- create the rebel nation -------//

	int raceId = RACE_CARTH;

	plot_nation_recno1 = nation_array.new_nation( NATION_HUMAN, NATION_AI, raceId, nation_array.random_unused_color() );

	Nation* rebelNation 	= nation_array[plot_nation_recno1];

	// ----- give tech --------//

	tech_res[TECH_SPU_STRUCTURE(raceId)]->set_nation_tech_level( plot_nation_recno1, 1);
	tech_res[TECH_SPU_BERSERK(raceId)]->set_nation_tech_level( plot_nation_recno1, 1);

	//---- create towns for the rebel nation ----//

	int townAddCount   = 2 + m.random(2);		// 2 to 3 towns
	int hasFortPercent = 100;

	create_town( plot_nation_recno1, townAddCount, hasFortPercent );

	// ----- create special structure --------//

	for( int townRecno = 1; townRecno <= town_array.size(); ++townRecno )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		Town *townPtr = town_array[townRecno];
		if( townPtr->nation_recno != plot_nation_recno1 )
			continue;

		create_firm_next_to_place( townPtr, FIRM_SPECIAL, raceId );
	}

	// ----- create fort and special structure full of special unit -------//

	for( int firmRecno = 1; firmRecno <= firm_array.size(); ++firmRecno )
	{
		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm *firmPtr = firm_array[firmRecno];
		if( firmPtr->nation_recno != plot_nation_recno1 )
			continue;

		if( firmPtr->cast_to_FirmFort() )
		{
			FirmFort *firmFort = firmPtr->cast_to_FirmFort();

			for( int s = firmFort->soldier_count; s > 0; --s )
			{
				Soldier *soldierPtr = firmFort->soldier_array + s - 1;

				if( soldierPtr->race_id == raceId )
				{
					soldierPtr->change_unit_id( race_res[soldierPtr->race_id]->special_unit_id, firmPtr->nation_recno );
				}
			}
		}
		else if( firmPtr->cast_to_FirmSpecial() )
		{
			FirmSpecial *firmSpecial = firmPtr->cast_to_FirmSpecial();

			short bestTown[MAX_RACE];
			for( int s = MAX_TRAINEE - 2 - m.random(2) ; s > 0 && firmSpecial->scan_best_recruitable_town(bestTown) > 0; --s )
			{
				firmSpecial->recruit_trainee( bestTown[0], TRAIN_UNIT_SPECIAL, COMMAND_AUTO );
			}

			// finish all basic training

			for( s = 1; s <= firmSpecial->trainee_count; ++s )
			{
				Trainee *traineePtr = firmSpecial->trainee_array + s - 1;
				if( traineePtr->is_under_training )
				{
					traineePtr->skill.set_combat_level(BASIC_COMBAT_TRAIN);
					traineePtr->skill.set_skill_level(BASIC_SKILL_TRAIN);
				}
			}
		}
		else		// other firm
		{
			err_here();
		}
	}

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
		TALK_END_ALLIANCE_TREATY,
		TALK_END_TRADE_TREATY,
		TALK_DECLARE_WAR,
		TALK_SURRENDER,
		0
	};

	rebelNation->get_relation(nation_array.player_recno)->set_ai_never_consider(neverConsiderArray2);

	// ------- rebel nation is allied with the player initially --------//

	rebelNation->set_relation_status( nation_array.player_recno, RELATION_ALLIANCE );
}



void CampaignEastWest::plot_a7_next_day()
{
	if( plot_nation_recno1 == 0				// it has already surrendered.
		 || nation_array.is_deleted(plot_nation_recno1) )
	{
		return;
	}

   //----------------------------------------------//

	Nation* playerNation = ~nation_array;
	Nation* rebelNation 	= nation_array[plot_nation_recno1];

	NationRelation* nationRelation = rebelNation->get_relation(nation_array.player_recno);

	//--- the rebel will propose an alliance treaty to the player if reputation >= 50 -----//

	if( playerNation->reputation >= 50.f
		&& nationRelation->status == RELATION_ALLIANCE )
	{
		short considerAgainArray[] = { TALK_SURRENDER, 0 };
		nationRelation->reset_ai_never_consider(considerAgainArray);

		talk_res.ai_send_talk_msg(playerNation->nation_recno,
			plot_nation_recno1, TALK_SURRENDER );

		plot_nation_recno1 = 0;
	}
}



