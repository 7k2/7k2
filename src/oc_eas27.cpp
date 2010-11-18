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

//Filename   : OC_EAS27.CPP
//Description: CampaignEastWest - STAGE_GROW_POPULATION

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


//---------- Define static vars ------------//

static int goal_population, goal_year_limit;

//---- Begin of function CampaignEastWest::stage_27_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_27_create_game()
{
	// ###### begin Gilbert 9/4 ########//
	init_random_plot(0);		// no plot
	// ###### end Gilbert 9/4 ########//

	//--- randomly init the current situation -----//

	set_cur_situation( m.random(CAMPAIGN_SITUATION_COUNT)+1 );

	//------- create objects and vars --------//

	if( !stage_27_create_nation() )
		return 0;

	if( !stage_27_create_town() )
		return 0;

	if( !stage_27_create_firm() )
		return 0;

	//------ init stage 1 vars -------//

	stage_27_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//----- general defection takes place, forming a new nation -------//

	general_defect_form_new_nation(PLAYER_NATION_RECNO);

	//------ create Egyptian rebel forces -----//

	stage_27_create_egyptian_rebel();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_27_create_game -----//


//---- Begin of function CampaignEastWest::stage_27_init_vars ----//

void CampaignEastWest::stage_27_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(50);

	//-------- add victory conditions -------//

	game.reset_win_condition();		// so that only achieving E_REACH_POPULATION will win the game.

	game.add_win_condition( E_REACH_POPULATION, 1, 0, goal_population, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_27_init_vars -----//


//---- Begin of function CampaignEastWest::stage_27_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_27_prelude()
{
	//-------- randomize goal settings --------//

	goal_population = 200 + campaign_difficulty*100;
	goal_year_limit = 12 - campaign_difficulty;

	//---------------------------------------------//
	play_speech("PRE-27.WAV");
	disp_narrative( res_stage.read("27PRELUD") );
	stop_speech();

	target_state_recno = random_pick_state(PLAYER_NATION_RECNO);

	flash_state( target_state_recno );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_27_prelude -----//


//---- Begin of function CampaignEastWest::stage_27_create_nation ----//

int CampaignEastWest::stage_27_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_27_create_nation ----//


//---- Begin of function CampaignEastWest::stage_27_create_town ----//

int CampaignEastWest::stage_27_create_town()
{
	Battle::create_independent_town(4+m.random(3));		// 4-6 independent towns at the beginning

	int townAddCount   = 4;
	int hasFortPercent = 100;

	return create_town(PLAYER_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_27_create_town ----//


//---- Begin of function CampaignEastWest::stage_27_create_firm ----//

int CampaignEastWest::stage_27_create_firm()
{
	int townRecno1 = king_oversee_town_recno(PLAYER_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links

	create_economic_firm(townRecno1);

	if( townRecno2 )
		create_military_firm(townRecno2, 100);

	return 1;
}
//---- End of function CampaignEastWest::stage_27_create_firm ----//


//--- Begin of function CampaignEastWest::stage_27_create_egyptian_rebel --//

void CampaignEastWest::stage_27_create_egyptian_rebel()
{
	int nationRecno = nation_array.new_nation( NATION_HUMAN, NATION_AI, RACE_EGYPTIAN, nation_array.random_unused_color() );     // 2nd parameter = the race id., 3rd parameters = color scheme id.

	create_town(nationRecno, 2, 100);		// create two towns

	//----- create firms next to the towns ----//

	int townRecno1 = king_oversee_town_recno(nationRecno);
	int townRecno2 = random_pick_town_with_camp(nationRecno, 2);		// only pick towns with <= 2 links

	create_economic_firm(townRecno1);

	if( townRecno2 )
		create_military_firm(townRecno2, 30+campaign_difficulty*10);

	//------- add Egyptian scroll of power to the nation -----//

	god_res[RACE_EGYPTIAN]->enable_know(nationRecno);
}
//---- End of function CampaignEastWest::stage_27_create_egyptian_rebel ---//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_27_next_day -------//
//
void CampaignEastWest::stage_27_next_day()
{
	// ###### begin Gilbert 9/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 9/4 ########//
}
//------- End of function CampaignEastWest::stage_27_next_day --------//


//---- Begin of function CampaignEastWest::stage_27_intro_text ----//

char* CampaignEastWest::stage_27_intro_text()
{
	return substitute_text( res_stage.read("27INTRO"), goal_population, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_27_intro_text ----//


//---- Begin of function CampaignEastWest::stage_27_goal_text ----//

char* CampaignEastWest::stage_27_goal_text()
{
	return substitute_text( res_stage.read("27GOAL"), goal_population, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_27_goal_text ----//


//============================================================//


//--- Begin of function CampaignEastWest::stage_27_process_game_result ---//
//
void CampaignEastWest::stage_27_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		err_when( !return_event_id );

		set_event( return_event_id );
	}

	//---- if the player lost the game ----//

	else
	{
		// do nothing as the player must retry the game.

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_27_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_27_disp_end_game_msg ------//
//
void CampaignEastWest::stage_27_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("27WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("27LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_27_disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_27_write_file ------//
//
void CampaignEastWest::stage_27_write_file(File* filePtr)
{
	filePtr->file_put_long(goal_population);
	filePtr->file_put_long(goal_year_limit);
}
//------- End of function CampaignEastWest::stage_27_write_file -------//


//------ Begin of function CampaignEastWest::stage_27_read_file ------//
//
void CampaignEastWest::stage_27_read_file(File* filePtr)
{
	goal_population = filePtr->file_get_long();
	goal_year_limit = filePtr->file_get_long();
}
//------- End of function CampaignEastWest::stage_27_read_file -------//

