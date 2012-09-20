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

//Filename   : OC_EAS28.CPP
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

static int goal_population, goal_economic_score, goal_year_limit;

//---- Begin of function CampaignEastWest::stage_28_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_28_create_game()
{
	init_random_plot(0);		// no plot

	//------- create objects and vars --------//

	if( !stage_28_create_nation() )
		return 0;

	//----- create independent towns ------//

	Battle::create_independent_town(5+misc.random(3));

	//---- create the player force and peasants ----//

	create_troop(PLAYER_NATION_RECNO, 16, 50 ); 		// 50-average combat level

	Nation* playerNation = nation_array[PLAYER_NATION_RECNO];

	Unit* kingUnit = unit_array[ playerNation->king_unit_recno ];

	//---- create a group of peasants with the player's nationality ---//

	int unitId = race_res[playerNation->race_id]->civilian_unit_id;
	int peasantCount = 25 + misc.random(6);

	for( int i=0 ; i<peasantCount ; i++ )
	{
		create_unit( PLAYER_NATION_RECNO, unitId, RANK_SOLDIER,
						 kingUnit->next_x_loc(), kingUnit->next_y_loc(),
						 kingUnit->next_x_loc(), kingUnit->next_y_loc() );
	}

	//------ init stage 1 vars -------//

	stage_28_init_vars();

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
//---- End of function CampaignEastWest::stage_28_create_game -----//


//---- Begin of function CampaignEastWest::stage_28_init_vars ----//

void CampaignEastWest::stage_28_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(60);

	//-------- add victory conditions -------//

	game.add_win_condition( E_REACH_POPULATION, 1, 0, goal_population	 , goal_year_limit );
	game.add_win_condition( E_REACH_ECONOMY   , 1, 0, goal_economic_score, goal_year_limit );

	game.win_require_all_condition = 1;
}
//---- End of function CampaignEastWest::stage_28_init_vars -----//


//---- Begin of function CampaignEastWest::stage_28_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_28_prelude()
{
	//-------- randomize goal settings --------//

	goal_population 	  = campaign_difficulty*100;
	goal_economic_score = campaign_difficulty*100;
	goal_year_limit     = 32 - campaign_difficulty*3;

	//---------------------------------------------//

	play_speech("PRE-28.WAV");
	disp_narrative( res_stage.read("28PRELUD"), goal_population, goal_economic_score, goal_year_limit );
	stop_speech();
	
	target_state_recno = random_pick_state(PLAYER_NATION_RECNO);

	flash_state( target_state_recno );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_28_prelude -----//


//---- Begin of function CampaignEastWest::stage_28_create_nation ----//

int CampaignEastWest::stage_28_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_28_create_nation ----//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_28_next_day -------//
//
void CampaignEastWest::stage_28_next_day()
{
	(this->*plot_next_day_FP)();

	//------ independent towns forming new kingdoms ----//

	if( misc.random( ( 12-campaign_difficulty*2+nation_array.size()*2 ) * 15 )==0 )
	{
		int townRecno = misc.random(town_array.size())+1;

		for( int i=town_array.size() ; i>0 ; i-- )
		{
			if( ++townRecno > town_array.size() )
				townRecno = 1;

			if( town_array.is_deleted(townRecno) )
				continue;

			Town* townPtr = town_array[townRecno];

			if( townPtr->can_independent_form_new_nation() )
			{
				townPtr->form_new_nation();
				break;
			}
		}
	}
}
//------- End of function CampaignEastWest::stage_28_next_day --------//


//---- Begin of function CampaignEastWest::stage_28_intro_text ----//

char* CampaignEastWest::stage_28_intro_text()
{
	return substitute_text( res_stage.read("28INTRO"), goal_population, goal_economic_score, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_28_intro_text ----//


//---- Begin of function CampaignEastWest::stage_28_goal_text ----//

char* CampaignEastWest::stage_28_goal_text()
{
	return substitute_text( res_stage.read("28GOAL"), goal_population, goal_economic_score, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_28_goal_text ----//


//============================================================//


//--- Begin of function CampaignEastWest::stage_28_process_game_result ---//
//
void CampaignEastWest::stage_28_process_game_result()
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

		misc.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_28_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_28_disp_end_game_msg ------//
//
void CampaignEastWest::stage_28_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("28WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("28LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_28_disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_28_write_file ------//
//
void CampaignEastWest::stage_28_write_file(File* filePtr)
{
	filePtr->file_put_long(goal_population);
	filePtr->file_put_long(goal_economic_score);
	filePtr->file_put_long(goal_year_limit);
}
//------- End of function CampaignEastWest::stage_28_write_file -------//


//------ Begin of function CampaignEastWest::stage_28_read_file ------//
//
void CampaignEastWest::stage_28_read_file(File* filePtr)
{
	goal_population = filePtr->file_get_long();
	goal_economic_score = filePtr->file_get_long();
	goal_year_limit = filePtr->file_get_long();
}
//------- End of function CampaignEastWest::stage_28_read_file -------//

