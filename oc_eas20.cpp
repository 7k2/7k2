//Filename   : OC_EAS20.CPP
//Description: CampaignEastWest - STAGE_ATTACK_EASTERN_EMPIRE_2

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <OGODRES.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <OF_CAMP.H>

//------ Define constant ------//

#define STAGE_20_EASTERN_NATION_RECNO	2


//---- Begin of function CampaignEastWest::stage_20_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -new_kingdom_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_20_create_game()
{
	// ###### begin Gilbert 1/4 ########//
	init_random_plot('C');
	// ###### end Gilbert 1/4 ########//

	if( !stage_20_create_nation() )
		return 0;

	if( !stage_20_create_town() )
		return 0;

	if( !stage_20_create_firm() )
		return 0;

	create_offensive_structure( STAGE_20_EASTERN_NATION_RECNO, campaign_difficulty );		// build 2 structures if the difficulty > 3

	//------ create the player troop -------//

	create_troop(PLAYER_NATION_RECNO, 86-campaign_difficulty*6);

	//------ init stage 1 vars -------//

	stage_20_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_20_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	// ###### begin Gilbert 1/4 ########//
	(this->*plot_create_game_FP)();
	// ###### end Gilbert 1/4 ########//
	return 1;
}
//---- End of function CampaignEastWest::stage_20_create_game -----//


//---- Begin of function CampaignEastWest::stage_20_init_vars ----//

void CampaignEastWest::stage_20_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(50);

	init_nation_resource(STAGE_20_EASTERN_NATION_RECNO, 70);
}
//---- End of function CampaignEastWest::stage_20_init_vars -----//


//---- Begin of function CampaignEastWest::stage_20_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_20_prelude()
{
	play_speech("PRE-20.WAV");
	disp_narrative( res_stage.read("20PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_20_prelude -----//


//---- Begin of function CampaignEastWest::stage_20_create_nation ----//

int CampaignEastWest::stage_20_create_nation()
{
	state_array[attacker_state_recno]->add_game_nation();
	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_20_create_nation ----//


//---- Begin of function CampaignEastWest::stage_20_create_town ----//

int CampaignEastWest::stage_20_create_town()
{
	int townAddCount   = 5 + m.random(3);
	int hasFortPercent = 90;

	return create_town( STAGE_20_EASTERN_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_20_create_town ----//


//---- Begin of function CampaignEastWest::stage_20_create_firm ----//

int CampaignEastWest::stage_20_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[STAGE_20_EASTERN_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(STAGE_20_EASTERN_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(STAGE_20_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

	if( townRecno2 && m.random(2)==0 )		// swap the town in 50% chance
	{
		int t=townRecno1;
		townRecno1 = townRecno2;
		townRecno2 = t;
	}

	//--- while the races of towns are random, the two main towns' race has to be the king's race ---//

	town_array[townRecno1]->set_race(nationPtr->race_id);

	if( townRecno2 )
		town_array[townRecno2]->set_race(nationPtr->race_id);

	create_economic_firm(townRecno1);

	if( townRecno2 )
		create_military_firm(townRecno2);

	//----- four possible situations -------//

	int eastSituation = m.random(2)+1;

	//-------------------------------------------------//
	// The target state has an emphasis on weapon
	// development and production. There are a number
	// of war factories and a large number of weapons.
	//-------------------------------------------------//

	if( eastSituation==1 )
	{
		//---- build a war factory next to the town with mostly economic facilities.

		create_work_firm( town_array[townRecno1], FIRM_WAR_FACTORY );

		//-- fill most of the forts with weapons -----//

		for( int i=0 ; i<nationPtr->ai_camp_count ; i++ )
		{
			FirmCamp* firmCamp = firm_array[ nationPtr->ai_camp_array[i] ]->cast_to_FirmCamp();

			int addCount = MAX_SOLDIER - firmCamp->soldier_count - m.random(3);		// don't always full up, sometimes leave a free slots empty

			for( int j=addCount ; j>0 ; j-- )
				add_weapon_to_camp( firmCamp->firm_recno );
		}

		//---- give it more cash upfront ------//

		nationPtr->cash += 1000 + 1000 * campaign_difficulty;
	}

	//-------------------------------------------------//
	// The target state has an emphasis on economic
	// development, it is vastly rich. And can use money
	// to boost up its military rapidly.
	//-------------------------------------------------//

	if( eastSituation==2 )
	{
		//--- create one more series of economic buildings ---//

		int townRecno2 = random_pick_town_with_camp(STAGE_20_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

		if( townRecno2 )
			create_economic_firm(townRecno2);

		//---- give it more cash upfront ------//

		nationPtr->cash += 2000 + 2000 * campaign_difficulty;
	}

	return 1;
}
//---- End of function CampaignEastWest::stage_20_create_firm ----//


//------- Begin of function CampaignEastWest::stage_20_init_relation -------//
//
void CampaignEastWest::stage_20_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_20_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_20_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_20_next_day -------//
//
void CampaignEastWest::stage_20_next_day()
{
	// ###### begin Gilbert 1/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 1/4 ########//
}
//------- End of function CampaignEastWest::stage_20_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_20_process_game_result ---//
//
void CampaignEastWest::stage_20_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		//--- if there is an independent state, 66% chance that it will ---//
		//--- turn towards you in admiration of your victory and power ----//

		if( state_array.nation_state_count(0) > 0 && m.random(3)>0 )
			set_event( EVENT_INDEPENDENT_SURRENDER );
		else
			set_event( EVENT_RANDOM_POINT_4 );
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_20_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_20_disp_end_game_msg ------//
//
void CampaignEastWest::stage_20_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("20WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("20LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_20_disp_end_game_msg -------//

