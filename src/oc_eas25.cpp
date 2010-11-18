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

//Filename   : OC_EAS25.CPP
//Description: CampaignEastWest - STAGE_UPRISING

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <ogodres.h>
#include <oraceres.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <ofirm.h>


//----- define constant ----------//

enum { STAGE_25_REBEL_NATION_RECNO = 2  };

//---------- Define static vars ------------//

static int goal_overall_score, goal_year_limit;

//---- Begin of function CampaignEastWest::stage_25_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_25_create_game()
{
	init_random_plot(0);		// no plot

	//---------- set to situation 6 ----------------//
	//
	// Situation 6: high military, high loyalty, high tech
	//					 low cash, low food, low raw
	//
	//----------------------------------------------//

	set_cur_situation(6);

	//------- create objects and vars --------//

	if( !stage_25_create_nation() )
		return 0;

	if( !stage_25_create_town() )
		return 0;

	if( !stage_25_create_firm() )
		return 0;

	//---- create rebel nations -------//

	for( int i=0 ; i < campaign_difficulty ; i++ )
	{
		create_rebel_nation(2, 50, 20+campaign_difficulty*10 );
	}

	//------ init stage 1 vars -------//

	stage_25_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//----- general defection takes place -------//

	general_defect_form_new_nation(PLAYER_NATION_RECNO);

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_25_create_game -----//


//---- Begin of function CampaignEastWest::stage_25_init_vars ----//

void CampaignEastWest::stage_25_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(50);

	init_nation_resource(STAGE_25_REBEL_NATION_RECNO, 60);

	//-------- add victory conditions -------//

	game.add_win_condition( E_REACH_TOTAL, 1, 0, goal_overall_score, goal_year_limit );

	game.win_require_all_condition = 1;
}
//---- End of function CampaignEastWest::stage_25_init_vars -----//


//---- Begin of function CampaignEastWest::stage_25_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_25_prelude()
{
	//-------- randomize goal settings --------//

	goal_overall_score = campaign_difficulty*200;
	goal_year_limit    = 16-campaign_difficulty;

	//---------------------------------------------//

	play_speech("PRE-25.WAV");
	disp_narrative( res_stage.read("25PRELUD") );
	stop_speech();

	target_state_recno = random_pick_state(PLAYER_NATION_RECNO);

	rebel_attack_state( target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_25_prelude -----//


//---- Begin of function CampaignEastWest::stage_25_create_nation ----//

int CampaignEastWest::stage_25_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_25_create_nation ----//


//---- Begin of function CampaignEastWest::stage_25_create_town ----//

int CampaignEastWest::stage_25_create_town()
{
	int townAddCount   = 4 + m.random(2);		// 4 to 5
	int hasFortPercent = 50;

	return create_town(PLAYER_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_25_create_town ----//


//---- Begin of function CampaignEastWest::stage_25_create_firm ----//

int CampaignEastWest::stage_25_create_firm()
{
	int townRecno1 = king_oversee_town_recno(PLAYER_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links

	create_military_firm(townRecno1, 80);

	if( townRecno2 )
		create_military_firm(townRecno2, 80);     // this is the one that will rebel later

	return 1;
}
//---- End of function CampaignEastWest::stage_25_create_firm ----//


//---- Begin of function CampaignEastWest::stage_25_intro_text ----//

char* CampaignEastWest::stage_25_intro_text()
{
	return substitute_text( res_stage.read("25INTRO"), goal_overall_score, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_25_intro_text ----//


//---- Begin of function CampaignEastWest::stage_25_goal_text ----//

char* CampaignEastWest::stage_25_goal_text()
{
	return substitute_text( res_stage.read("25GOAL"), goal_overall_score, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_25_goal_text ----//



//=================================================================//


//------- Begin of function CampaignEastWest::stage_25_next_day -------//
//
void CampaignEastWest::stage_25_next_day()
{
	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_25_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_25_process_game_result ---//
//
void CampaignEastWest::stage_25_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		set_event( EVENT_RANDOM_POINT_4 );
	}

	//---- if the player lost the game ----//

	else
	{
		// do nothing as the player must retry the game.

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_25_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_25_disp_end_game_msg ------//
//
void CampaignEastWest::stage_25_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("25WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("25LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_25_disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_25_write_file ------//
//
void CampaignEastWest::stage_25_write_file(File* filePtr)
{
	filePtr->file_put_long(goal_year_limit);
	filePtr->file_put_long(goal_overall_score);
}
//------- End of function CampaignEastWest::stage_25_write_file -------//


//------ Begin of function CampaignEastWest::stage_25_read_file ------//
//
void CampaignEastWest::stage_25_read_file(File* filePtr)
{
	goal_year_limit = filePtr->file_get_long();
	goal_overall_score = filePtr->file_get_long();
}
//------- End of function CampaignEastWest::stage_25_read_file -------//

