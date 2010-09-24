//Filename   : OC_EAS24.CPP
//Description: CampaignEastWest - STAGE_EAST_JOINT_ATTACK_FRYHTAN

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <OF_CAMP.H>

//----- define constant ----------//

enum { STAGE_24_EASTERN_NATION_RECNO = 2,
		 STAGE_24_MONSTER_NATION_RECNO = 3  };

//---- Begin of function CampaignEastWest::stage_24_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno    - the target monster nation recno
// -attacker_state_recno	     - the player attacking state
// -attacker_state_recno2 		  - the eastern empire attacking state
// -target_state_recno			  - the monster target state
//
int CampaignEastWest::stage_24_create_game()
{
	init_random_plot('A');

	plot_enemy_nation_recno = STAGE_24_MONSTER_NATION_RECNO;

	//---- create game objects and vars ----//

	if( !stage_24_create_nation() )
		return 0;

	//------- init relationship -------//

	stage_24_init_relation();		// must init relationship first so that allied units are visible on the map

	//------ create lairs --------//

	if( !stage_24_create_lair() )
		return 0;

	//---- create the player force ----//

	create_troop(PLAYER_NATION_RECNO, 60-campaign_difficulty*6 );

	//---- create the eastern empire force ----//

	create_troop(STAGE_24_EASTERN_NATION_RECNO, 60-campaign_difficulty*6, 0, 0, 1 );		// last 1-create new units on the location next to the last bunch units created

	//------ init stage 1 vars -------//

	stage_24_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_24_create_game -----//


//---- Begin of function CampaignEastWest::stage_24_init_vars ----//

void CampaignEastWest::stage_24_init_vars()
{
	//----- set player nation vars -----//

	init_player_resource(50);

	init_nation_resource(STAGE_24_EASTERN_NATION_RECNO, 40);

	init_nation_resource(STAGE_24_MONSTER_NATION_RECNO, 40);

	//-------- add victory conditions -------//

	game.add_win_condition( E_DEFEAT_KINGDOM, 1, STAGE_24_MONSTER_NATION_RECNO );  	// the objective is to destroy the enemy kingdom
}
//---- End of function CampaignEastWest::stage_24_init_vars -----//


//---- Begin of function CampaignEastWest::stage_24_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_24_prelude()
{
//	disp_narrative( res_stage.read("24PRELUD") );

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2, attacker2_state_recno );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_24_prelude -----//


//---- Begin of function CampaignEastWest::stage_24_create_nation ----//

int CampaignEastWest::stage_24_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//------- create the Eastern nation --------//

	state_array[attacker2_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_24_create_nation ----//


//---- Begin of function CampaignEastWest::stage_24_create_lair ----//

int CampaignEastWest::stage_24_create_lair()
{
	int lairAddCount = 6 + m.random(3);						// add 6 to 7 Fryhtan Lairs
	int independentTownAddCount=0;
	int specialFirmCount = 3 + m.random(2);

	if( m.random(2)==0 )		// 50% chance having independent towns
		independentTownAddCount = 6 + m.random(3);		// 6 to 8 independent towns

	create_lair(cur_monster_nation_recno, STAGE_24_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount);

	return 1;
}
//---- End of function CampaignEastWest::stage_24_create_lair ----//


//------- Begin of function CampaignEastWest::stage_24_init_relation -------//
//
void CampaignEastWest::stage_24_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_24_MONSTER_NATION_RECNO);
	set_nation_allied(PLAYER_NATION_RECNO , STAGE_24_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_24_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_24_next_day -------//
//
void CampaignEastWest::stage_24_next_day()
{
	//------- call plot next day --------//

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_24_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_24_process_game_result ---//
//
void CampaignEastWest::stage_24_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		//----------------------------------------------------------------//
		// You empire and the eastern empire gets the conquered land
		// in an alternate way. (i.e. you get the land in the 1st winning
		// game and the eastern empire gets the land in the 2nd winning game.
		//----------------------------------------------------------------//

		if( ++east_joint_attack_fryhtan_win_count%2 == 1 ||
			 nation_array.is_deleted(STAGE_24_EASTERN_NATION_RECNO) )		// or the allied kingdom has been destroyed
		{
			player_beat_fryhtan_count++;		// the player takes the conquered land
		}

		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3, attacker2_state_recno );

		set_event( return_event_id );
	}

	//---- if the player lost the game ----//

	else
	{
		set_event( return_event_id );
	}
}
//---- End of function CampaignEastWest::stage_24_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_24_disp_end_game_msg ------//
//
void CampaignEastWest::stage_24_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("24WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("24LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_24_disp_end_game_msg -------//

