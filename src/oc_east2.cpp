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

//Filename   : OC_EAST.CPP
//Description: CampaignEastWest - STAGE_FRYHTAN_INVASION_2

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
#include <of_camp.h>

//----- define constant ----------//

enum { STAGE_2_MONSTER_NATION_RECNO = 2  };

//------ define static vars --------//

static short goal_kill_monster_score;

//---- Begin of function CampaignEastWest::stage_2_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_2_create_game()
{
	init_random_plot('D');

	if( !stage_2_create_nation() )
		return 0;

	if( !stage_2_create_town() )
		return 0;

	if( !stage_2_create_firm() )
		return 0;

	//------ init stage 1 vars -------//

	stage_2_init_vars();

	//---- generate plants and other objects on the map -----//

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_2_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_2_create_game -----//


//---- Begin of function CampaignEastWest::stage_2_init_vars ----//

void CampaignEastWest::stage_2_init_vars()
{
	//------ init stage 1 vars -------//

	rebel_count = MAX(1, campaign_difficulty-1);
	next_rebel_date = info.game_date + 30 + misc.random(30);

	general_defect_count = MAX(1, campaign_difficulty/2);
	next_general_defect_date = info.game_date + 30 + misc.random(30);

	//----- init monster reinforcement settings -----//

	static short reinforceCountArray[] = { 10, 25, 50, 85, 130 };

	monster_reinforcement_count = reinforceCountArray[campaign_difficulty-1];		// this is the number of units, not the number of times of reinforcement

	next_monster_reinforcement_date = info.game_date + (MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1) * 20;

	//------- set player nation vars -------//

	init_player_resource(60);

	init_nation_resource(STAGE_2_MONSTER_NATION_RECNO, 50);

	//-------- add victory conditions -------//

	game.add_win_condition( E_KILL_MONSTER_SCORE, 1, 0, goal_kill_monster_score );			// 1-this condition only applies to the player.
}
//---- End of function CampaignEastWest::stage_2_init_vars -----//


//---- Begin of function CampaignEastWest::stage_2_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_2_prelude()
{
	goal_kill_monster_score = 50 + campaign_difficulty * 70;

	play_speech("PRE-02.WAV");
	disp_narrative( res_stage.read("2PRELUDE"), goal_kill_monster_score );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	mountain_layout_enabled = 1;

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_2_prelude -----//


//---- Begin of function CampaignEastWest::stage_2_create_nation ----//

int CampaignEastWest::stage_2_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	//------- create monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_2_create_nation ----//


//---- Begin of function CampaignEastWest::stage_2_create_town ----//

int CampaignEastWest::stage_2_create_town()
{
	reset_mountain_layout();

	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	enum { MOUNTAIN_GAP_SIZE = 20 };

	int rangeX1, rangeY1, rangeX2, rangeY2;

	if( misc.random(2)==0 )		// divided horizontally
	{
		int mountainX;

		if( misc.random(2)==0 )
		{
			rangeX1 = 0;
			rangeX2 = 70;

			mountainX = 70;

         which_edge = 4;		// monster reinforcement comes from the right edge 
		}
		else
		{
			rangeX1 = 30;
			rangeX2 = TOWN_STATE_MAP_WIDTH-1;

			mountainX = 30;

			which_edge = 3;		// monster reinforcement comes from the left edge
		}

		rangeY1 = 0;
		rangeY2 = TOWN_STATE_MAP_HEIGHT-1;

		//----- a vertical mountain -----//

		int gap1Y1 = TOWN_STATE_MAP_HEIGHT/3 + misc.random(20)-10;
		int gap1Y2 = gap1Y1 + MOUNTAIN_GAP_SIZE;

		int gap2Y1 = TOWN_STATE_MAP_HEIGHT*2/3 + misc.random(20)-10;
		int gap2Y2 = gap2Y1 + MOUNTAIN_GAP_SIZE;

		for( int i=0 ; i<TOWN_STATE_MAP_HEIGHT ; i++ )
		{
			if( i >= gap1Y1 && i<= gap1Y2 )
				continue;

			if( i >= gap2Y1 && i<= gap2Y2 )
				continue;

			set_on_mountain_layout(mountainX  , i, TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
			set_on_mountain_layout(mountainX+1, i, TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
			set_on_mountain_layout(mountainX+2, i, TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
		}
	}
	else							// divided vertically
	{
		int mountainY;

		if( misc.random(2)==0 )
		{
			rangeY1 = 0;
			rangeY2 = 70;

			mountainY = 70;

			which_edge = 2;		// monster reinforcement comes from the bottom edge
		}
		else
		{
			rangeY1 = 30;
			rangeY2 = TOWN_STATE_MAP_HEIGHT-1;

			mountainY = 30;

			which_edge = 1;		// monster reinforcement comes from the top edge
		}

		rangeX1 = 0;
		rangeX2 = TOWN_STATE_MAP_WIDTH-1;

		//----- a horizontal mountain -----//

		int gap1X1 = TOWN_STATE_MAP_WIDTH/3 + misc.random(20)-10;
		int gap1X2 = gap1X1 + MOUNTAIN_GAP_SIZE;

		int gap2X1 = TOWN_STATE_MAP_WIDTH*2/3 + misc.random(20)-10;
		int gap2X2 = gap2X1 + MOUNTAIN_GAP_SIZE;

		for( int i=0 ; i<TOWN_STATE_MAP_WIDTH ; i++ )
		{
			if( i >= gap1X1 && i<= gap1X2 )
				continue;

			if( i >= gap2X1 && i<= gap2X2 )
				continue;

			set_on_mountain_layout(i, mountainY  , TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
			set_on_mountain_layout(i, mountainY+1, TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
			set_on_mountain_layout(i, mountainY+2, TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
		}
	}

	world.gen_mountains( mountain_layout );		// generate mountains now

	//------ generate a StateArray ---------//

	int playerTownAddCount  = 5 + misc.random(3);
	int monsterLairAddCount = 3;

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(13);

	for( int i=townStateArray.size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		//-------- add player towns --------//

		if( stateInfo->center_x >= rangeX1 &&
			 stateInfo->center_x <= rangeX2 &&
			 stateInfo->center_y >= rangeY1 &&
			 stateInfo->center_y <= rangeY2 )
		{
			if( playerTownAddCount > 0 )
			{
				int buildFort = 1;

				if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, buildFort) )
					playerTownAddCount--;
			}
		}

		//------ add monster lairs -------//

		else
		{
			if( monsterLairAddCount > 0 )
			{
				int soldierCount = MAX_SOLDIER;
				int soldierCombatLevel = 25 + campaign_difficulty*5;

				if( create_monster_lair(xLoc, yLoc, STAGE_2_MONSTER_NATION_RECNO,
					 soldierCount, soldierCombatLevel) )
				{
					monsterLairAddCount--;
				}
			}
		}
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_2_create_town ----//


//---- Begin of function CampaignEastWest::stage_2_create_firm ----//

int CampaignEastWest::stage_2_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[PLAYER_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(PLAYER_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links

	if( townRecno2 && misc.random(2)==0 )		// swap the town in 50% chance
	{
		int t=townRecno1;
		townRecno1 = townRecno2;
		townRecno2 = t;
	}

	//--- while the races of towns are random, the two main towns' race has to be the king's race ---//

	town_array[townRecno1]->set_race(nationPtr->race_id);

	if( townRecno2 )
		town_array[townRecno2]->set_race(nationPtr->race_id);

	if( !create_economic_firm(townRecno1) )
		return 0;

	if( townRecno2 && !create_military_firm(townRecno2) )
		return 0;

	return 1;
}
//---- End of function CampaignEastWest::stage_2_create_firm ----//


//------- Begin of function CampaignEastWest::stage_2_init_relation -------//
//
void CampaignEastWest::stage_2_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_2_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_2_init_relation --------//


//---- Begin of function CampaignEastWest::stage_2_intro_text ----//

char* CampaignEastWest::stage_2_intro_text()
{
	return substitute_text( res_stage.read("2INTRO"), goal_kill_monster_score );
}
//---- End of function CampaignEastWest::stage_2_intro_text ----//


//---- Begin of function CampaignEastWest::stage_2_goal_text ----//

char* CampaignEastWest::stage_2_goal_text()
{
	return substitute_text( res_stage.read("2GOAL"), goal_kill_monster_score );
}
//---- End of function CampaignEastWest::stage_2_goal_text ----//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_2_next_day -------//
//
void CampaignEastWest::stage_2_next_day()
{
	//------ plot next day ----------//

	(this->*plot_next_day_FP)();

	//-----------------------------------//

	if( should_rebel(30) )
		town_defect(PLAYER_NATION_RECNO);

	if( should_general_defect(30) )
		general_defect_form_new_nation(PLAYER_NATION_RECNO);

	if( !nation_array.is_deleted(STAGE_2_MONSTER_NATION_RECNO) )
	{
		monster_reinforcement_to_lair(which_edge);

		monster_reinforcement(STAGE_2_MONSTER_NATION_RECNO, which_edge);
	}
}
//------- End of function CampaignEastWest::stage_2_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_2_process_game_result ---//
//
void CampaignEastWest::stage_2_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		Nation* playerNation = (~nation_array);

		//-- although the player survive, the Fryhtan will eventually win and take over the state --//

		if ( game.result_win_condition_id == E_DEFEAT_ALL_OTHER_KINGDOMS )
		{
			hero_debut_state_count++;				// this is not likely to happen though
			player_beat_fryhtan_count++;
		}
		else if( game.result_win_condition_id == E_KILL_MONSTER_SCORE )
		{
			disp_narrative( res_stage.read("2POST") );
			attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );
		}
		else
			err_here();

		set_event( EVENT_RANDOM_POINT_1 );
	}

	//---- if the player lost the game ----//

	else
	{
		// retry - so no code here.

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		misc.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_2_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_2_disp_end_game_msg ------//
//
void CampaignEastWest::stage_2_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		Nation* playerNation = (~nation_array);

		if( game.result_win_condition_id == E_KILL_MONSTER_SCORE )
		{
			disp_in_game_msg( res_stage.read("2WIN2"), goal_kill_monster_score );
		}
		else if ( game.result_win_condition_id == E_DEFEAT_ALL_OTHER_KINGDOMS )
		{
			disp_in_game_msg( res_stage.read("2WIN1") );
		}
		else
		{
#ifdef DEBUG
			err.run("Win id. %d unidentified, stage: %d", game.result_win_condition_id, stage_id );
#endif
		}
	}
	else
	{
		disp_in_game_msg( res_stage.read("2LOSE"), goal_kill_monster_score );
	}
}
//------- End of function CampaignEastWest::stage_2_disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_2_write_file ------//
//
void CampaignEastWest::stage_2_write_file(File* filePtr)
{
	filePtr->file_put_long(goal_kill_monster_score);
}
//------- End of function CampaignEastWest::stage_2_write_file -------//


//------ Begin of function CampaignEastWest::stage_2_read_file ------//
//
void CampaignEastWest::stage_2_read_file(File* filePtr)
{
	goal_kill_monster_score = (short) filePtr->file_get_long();
}
//------- End of function CampaignEastWest::stage_2_read_file -------//

