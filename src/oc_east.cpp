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
//Description: CampaignEastWest

#include <all.h>
#include <oinfo.h>
#include <onation.h>
#include <oherores.h>
#include <ogodres.h>
#include <ostate.h>
#include <oc_east.h>
#include <ogame.h>
#include <ot_camp.h>

//----- Begin of function CampaignEastWest::init_new_campaign_derived -----//
//
// This function is called by Campaign::init_new_campaign().
//
void CampaignEastWest::init_new_campaign_derived()
{
	memset( sizeof(Campaign)+(char *)this, 0, sizeof(CampaignEastWest)-sizeof(Campaign) );

	//-----------------------------------//

	max_stage = 28;
	max_event = 23;

	//---- init common campaign vars ----//

	hero_res.disable_hero_appear();	// no heroes will randomly appear in this campaign
	hero_debut_state_count  = 1;		// The hero debut event will happen when the number of player states drops to this number
	player_beat_fryhtan_max = 1 + m.random(3);		// random 1-3
	use_fryhtan_max = 3 + m.random(3);

	//----- create state array -------//

	state_array.init(100, 100);
	state_array.generate(12);

	//------ create nations and init states -----//

	create_nation();

	set_state_nation();

	//-- enable the knowledge of a greater being for the player nation --//

	CampaignNation* cNation = get_nation(CAMPAIGN_PLAYER_NATION_RECNO);

	cNation->know_base_array[ cNation->race_id-1 ] = 1;

	//-----------------------------------------//

	err_when( state_array.nation_state_count(western_nation_recno)==0 );

	if( state_array.nation_state_count(western_nation_recno)==1 || m.random(3)==0 )
	{
		set_event(EVENT_RANDOM_POINT_1);
	}
	else
	{
		if( m.random(2)==0 )		// 50% chance of bypassing the first game
			set_stage(STAGE_FRYHTAN_INVASION_1);
		else
		{
			cur_monster_nation_recno = random_pick_monster_campaign_nation();

			set_event(EVENT_FRYHTAN_TAKEOVER);
		}
	}
}
//------ End of function CampaignEastWest::init_new_campaign_derived -----//


//----- Begin of function CampaignEastWest::create_nation -----//
//
void CampaignEastWest::create_nation()
{
	int monsterNationCount = 2 + m.random(2);		// 2 to 3 Fryhtan kingdoms

	western_nation_recno = 1;
	eastern_nation_recno = 2;

	add_nation(CAMPAIGN_NATION_OWN, config.race_id, config.player_nation_color);
	add_nation(CAMPAIGN_NATION_AI);

	//---------- set custom names -----------//

	get_nation(western_nation_recno)->set_custom_name( text_campaign.str_western_empire() ); // "Western Empire" );
	get_nation(eastern_nation_recno)->set_custom_name( text_campaign.str_eastern_empire() ); // "Eastern Empire" );
}
//------ End of function CampaignEastWest::create_nation -----//


//----- Begin of function CampaignEastWest::set_state_nation -----//
//
// Set the nation recno of all states.
//
void CampaignEastWest::set_state_nation()
{
	int monsterNationCount = 0;
	int maxMonsterNation = 2 + m.random(2);		// 2 to 3
	int monsterStateToAdd = 4 + m.random(3);		// 4 to 6

	//---- add Fryhtan states and create Fryhtan nations (including the player's) ----//

	int stateRecno = m.random(state_array.size())+1;

	int i;
	for( i=state_array.size() ; i>0 ; i-- )
	{
		if( ++stateRecno > state_array.size() )
			stateRecno = 1;

		StateInfo* stateInfo = state_array[stateRecno];

		//---- the Fryhtans states are on the left edge of the map only ---//

		if( stateInfo->center_x > MAX_STATE_MAP_WIDTH * 70 / 100 ||
			 !stateInfo->is_near_border() )
		{
			continue;
		}

		//--- there is a 33% chance that the new state is an extended area of one of the existing Fryhtan nations as opposite to the state of a new nation ----//

		if( m.random(3)==0 || monsterNationCount == maxMonsterNation )
		{
			for( int j=state_array.size() ; j>0 ; j-- )
			{
				StateInfo* stateInfo2 = state_array[j];

				if( stateInfo2->is_adjacent(stateRecno) &&
					 stateInfo2->campaign_nation_recno &&
					 get_nation(stateInfo2->campaign_nation_recno)->is_monster() )
				{
					stateInfo->campaign_nation_recno = stateInfo2->campaign_nation_recno;
					break;
				}
			}
		}

		//--- if it has been set, create a new Fryhtan kingdom ---//

		if( !stateInfo->campaign_nation_recno )
		{
			stateInfo->campaign_nation_recno = add_nation( CAMPAIGN_NATION_MONSTER );
		}

		//---- if all monster states that we need to add are added ----//

		if( --monsterStateToAdd == 0 )
			break;
	}

	//----- add states of the Eastern & Western Empires -----//

	for( i=state_array.size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = state_array[i];

		if( stateInfo->campaign_nation_recno )
			continue;

		if( stateInfo->center_x < MAX_STATE_MAP_WIDTH * 30 / 100 )
		{
			stateInfo->campaign_nation_recno = western_nation_recno;		// Western Empire
		}
		else
		{
			stateInfo->campaign_nation_recno = eastern_nation_recno;     // Eastern Empire
		}
	}

	//--- if western empire has no states, change one of the eastern to the western ---//

	int loopCount=0;

	while( state_array.nation_state_count(western_nation_recno) < 2 )
	{
		int changeStateRecno=0;
		int smallestX=MAX_STATE_MAP_WIDTH;

		for( i=state_array.size() ; i>0 ; i-- )
		{
			StateInfo* stateInfo = state_array[i];

			if( stateInfo->campaign_nation_recno == eastern_nation_recno &&
				 stateInfo->center_x < smallestX )
			{
				//---- if the western empire already has a state, then the new state must be next to the existing border ----//

				if( state_array.nation_state_count(western_nation_recno)==1 )
				{
					if( !state_array.is_adjacent_to_nation(i,western_nation_recno) )
						continue;
				}

				//-----------------------------------------//

				smallestX = stateInfo->center_x;
				changeStateRecno = i;
			}
		}

		if( !changeStateRecno && state_array.nation_state_count(western_nation_recno) > 0 )		// if western nation already has a state
			break;

		err_when( !changeStateRecno );

		state_array[changeStateRecno]->campaign_nation_recno = western_nation_recno;

		err_when( ++loopCount == 1000 );
	}

	err_when( state_array.nation_state_count(western_nation_recno)==0 );		// at least 2 states for the Western Empire
}
//----- End of function CampaignEastWest::set_state_nation -----//


//-------- Begin of function CampaignEastWest::create_pregame_object --------//
//
// This function is called by Battle::run().
//
void CampaignEastWest::create_pregame_object()
{
	switch( stage_id )
	{
		case 1:
			stage_1_create_game();
			break;

		case 2:
			stage_2_create_game();
			break;

		case 3:
			stage_3_create_game();
			break;

		case 4:
			stage_4_create_game();
			break;

		case 5:
			stage_5_create_game();
			break;

		case 6:
			stage_6_create_game();
			break;

		case 7:
			stage_7_create_game();
			break;

		case 8:
			stage_8_create_game();
			break;

		case 9:
			stage_9_create_game();
			break;

		case 10:
			stage_10_create_game();
			break;

		case 11:
			stage_11_create_game();
			break;

		case 12:
			stage_12_create_game();
			break;

		case 13:
			stage_13_create_game();
			break;

		case 14:
			stage_14_create_game();
			break;

		case 15:
			stage_15_create_game();
			break;

		case 16:
			stage_16_create_game();
			break;

		case 17:
			stage_17_create_game();
			break;

		case 18:
			stage_18_create_game();
			break;

		case 19:
			stage_19_create_game();
			break;

		case 20:
			stage_20_create_game();
			break;

		case 21:
			stage_21_create_game();
			break;

		case 22:
			stage_22_create_game();
			break;

		case 23:
			stage_23_create_game();
			break;
  
		case 24:
			stage_24_create_game();
			break;

		case 25:
			stage_25_create_game();
			break;

		case 26:
			stage_26_create_game();
			break;

		case 27:
			stage_27_create_game();
			break;

		case 28:
			stage_28_create_game();
			break;

		default:
			err_here();
	}

	//--- copy nation relation from campaign nation to game nation ----//

	fetch_nation_info();

	//--- hardwire relationship. This should be called after fetech_nation_info ---//

	switch( stage_id )
	{
		case 1:
			stage_1_init_relation();
			break;

		case 2:
			stage_2_init_relation();
			break;

		case 4:
			stage_4_init_relation();
			break;

		case 5:
			stage_5_init_relation();
			break;

		case 6:
			stage_6_init_relation();
			break;

		case 7:
			stage_7_init_relation();
			break;

		case 8:
			stage_8_init_relation();
			break;

		case 9:
			stage_9_init_relation();
			break;

		case 10:
			stage_10_init_relation();
			break;

		case 11:
			stage_11_init_relation();
			break;

		case 12:
			stage_12_init_relation();
			break;

		case 13:
			stage_13_init_relation();
			break;

		case 14:
			stage_14_init_relation();
			break;

		case 15:
			stage_15_init_relation();
			break;

		case 16:
			stage_16_init_relation();
			break;

		case 17:
			stage_17_init_relation();
			break;

		case 18:
			stage_18_init_relation();
			break;

		case 19:
			stage_19_init_relation();
			break;

		case 20:
			stage_20_init_relation();
			break;

		case 21:
			stage_21_init_relation();
			break;

		case 22:
			stage_22_init_relation();
			break;

		case 23:
			stage_23_init_relation();
			break;
  
		case 24:
			stage_24_init_relation();
			break;
	}

	//--------- generate roads -----------//

	create_road();
}
//------- End of function CampaignEastWest::create_pregame_object ------//


//------ Begin of function CampaignEastWest::process_game_result ------//
//
void CampaignEastWest::process_game_result()
{
	//---- update the campaign date ----//

	game_year = info.game_year+1;

	//--- add a bonus to the campaign score if the player has won the game ---//

	if( game.result_win_condition_id >= 0 )
	{
	//	enum { BONUS_MAX_YEAR = 20 };

		if( info.year_passed < BONUS_MAX_YEAR && !cheat_to_win_flag )
			campaign_score += BONUS_MAX_YEAR - info.year_passed;

		cheat_to_win_flag = 0;
	}
	else	//--- if the player loses, reset win_to_unveil_tech_id as the tech will only be unveil in the next game if the player won this game. ----//
	{
		win_to_unveil_tech_id = 0;
	}

	//-----------------------------------------------//

	switch( stage_id )
	{
		case 1:
			stage_1_process_game_result();
			break;

		case 2:
			stage_2_process_game_result();
			break;

		case 3:
			stage_3_process_game_result();
			break;

		case 4:
			stage_4_process_game_result();
			break;

		case 5:
			stage_5_process_game_result();
			break;

		case 6:
			stage_6_process_game_result();
			break;

		case 7:
			stage_7_process_game_result();
			break;

		case 8:
			stage_8_process_game_result();
			break;

		case 9:
			stage_9_process_game_result();
			break;

		case 10:
			stage_10_process_game_result();
			break;

		case 11:
			stage_11_process_game_result();
			break;

		case 12:
			stage_12_process_game_result();
			break;

		case 13:
			stage_13_process_game_result();
			break;

		case 14:
			stage_14_process_game_result();
			break;

		case 15:
			stage_15_process_game_result();
			break;

		case 16:
			stage_16_process_game_result();
			break;

		case 17:
			stage_17_process_game_result();
			break;

		case 18:
			stage_18_process_game_result();
			break;

		case 19:
			stage_19_process_game_result();
			break;

		case 20:
			stage_20_process_game_result();
			break;

		case 21:
			stage_21_process_game_result();
			break;

		case 22:
			stage_22_process_game_result();
			break;

		case 23:
			stage_23_process_game_result();
			break;

		case 24:
			stage_24_process_game_result();
			break;

		case 25:
			stage_25_process_game_result();
			break;

		case 26:
			stage_26_process_game_result();
			break;

		case 27:
			stage_27_process_game_result();
			break;

		case 28:
			stage_28_process_game_result();
			break;

		default:
			err_here();
	}

	//----------------------------------//

	catch_nation_info();		// copy nation relation from game nation to campaign nation
}
//------- End of function CampaignEastWest::process_game_result -------//


//------ Begin of function CampaignEastWest::disp_end_game_msg ------//
//
void CampaignEastWest::disp_end_game_msg()
{
	switch( stage_id )
	{
		case 1:
			stage_1_disp_end_game_msg();
			return;

		case 2:
			stage_2_disp_end_game_msg();
			return;

		case 3:
			stage_3_disp_end_game_msg();
			return;

		case 4:
			stage_4_disp_end_game_msg();
			return;

		case 5:
			stage_5_disp_end_game_msg();
			return;

		case 6:
			stage_6_disp_end_game_msg();
			return;

		case 7:
			stage_7_disp_end_game_msg();
			return;

		case 8:
			stage_8_disp_end_game_msg();
			return;

		case 9:
			stage_9_disp_end_game_msg();
			return;

		case 10:
			stage_10_disp_end_game_msg();
			return;

		case 11:
			stage_11_disp_end_game_msg();
			break;

		case 12:
			stage_12_disp_end_game_msg();
			break;

		case 13:
			stage_13_disp_end_game_msg();
			break;

		case 14:
			stage_14_disp_end_game_msg();
			break;

		case 15:
			stage_15_disp_end_game_msg();
			break;

		case 16:
			stage_16_disp_end_game_msg();
			break;

		case 17:
			stage_17_disp_end_game_msg();
			break;

		case 18:
			stage_18_disp_end_game_msg();
			break;

		case 19:
			stage_19_disp_end_game_msg();
			break;

		case 20:
			stage_20_disp_end_game_msg();
			break;

		case 21:
			stage_21_disp_end_game_msg();
			break;

		case 22:
			stage_22_disp_end_game_msg();
			break;

		case 23:
			stage_23_disp_end_game_msg();
			break;
  
		case 24:
			stage_24_disp_end_game_msg();
			break;

		case 25:
			stage_25_disp_end_game_msg();
			break;

		case 26:
			stage_26_disp_end_game_msg();
			break;

		case 27:
			stage_27_disp_end_game_msg();
			break;

		case 28:
			stage_28_disp_end_game_msg();
			break;

		default:
			err_here();
	}
}
//------- End of function CampaignEastWest::disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_prelude ------//
//
void CampaignEastWest::stage_prelude()
{
	mountain_layout_enabled = 0;
	set_cur_situation(0);

	config.terrain_set = 1;
	config.goal_alliance_win_flag = 1;

	game.add_win_condition( E_DEFEAT_EXCEPT_ALLIANCE, 1 );

	//-----------------------------------//

	switch( stage_id )
	{
		case 1:
			stage_1_prelude();
			break;

		case 2:
			stage_2_prelude();
			break;

		case 3:
			stage_3_prelude();
			break;

		case 4:
			stage_4_prelude();
			break;

		case 5:
			stage_5_prelude();
			break;

		case 6:
			stage_6_prelude();
			break;

		case 7:
			stage_7_prelude();
			break;

		case 8:
			stage_8_prelude();
			break;

		case 9:
			stage_9_prelude();
			break;

		case 10:
			stage_10_prelude();
			break;

		case 11:
			stage_11_prelude();
			break;

		case 12:
			stage_12_prelude();
			break;

		case 13:
			stage_13_prelude();
			break;

		case 14:
			stage_14_prelude();
			break;

		case 15:
			stage_15_prelude();
			break;

		case 16:
			stage_16_prelude();
			break;

		case 17:
			stage_17_prelude();
			break;

		case 18:
			stage_18_prelude();
			break;

		case 19:
			stage_19_prelude();
			break;

		case 20:
			stage_20_prelude();
			break;

		case 21:
			stage_21_prelude();
			break;

		case 22:
			stage_22_prelude();
			break;

		case 23:
			stage_23_prelude();
			break;
  
		case 24:
			stage_24_prelude();
			break;

		case 25:
			stage_25_prelude();
			break;

		case 26:
			stage_26_prelude();
			break;

		case 27:
			stage_27_prelude();
			break;

		case 28:
			stage_28_prelude();
			break;

		default:
			err_here();
	}
}
//------- End of function CampaignEastWest::stage_prelude -------//


//------- Begin of function CampaignEastWest::next_day -------//
//
void CampaignEastWest::next_day()
{
	switch( stage_id )
	{
		case 1:
			stage_1_next_day();
			break;

		case 2:
			stage_2_next_day();
			break;

		case 3:
			stage_3_next_day();
			break;

		case 4:
			stage_4_next_day();
			break;

		case 5:
			stage_5_next_day();
			break;

		case 6:
			stage_6_next_day();
			break;

		case 7:
			stage_7_next_day();
			break;

		case 8:
			stage_8_next_day();
			break;

		case 9:
			stage_9_next_day();
			break;

		case 10:
			stage_10_next_day();
			break;

		case 11:
			stage_11_next_day();
			break;

		case 12:
			stage_12_next_day();
			break;

		case 13:
			stage_13_next_day();
			break;

		case 14:
			stage_14_next_day();
			break;

		case 15:
			stage_15_next_day();
			break;

		case 16:
			stage_16_next_day();
			break;

		case 17:
			stage_17_next_day();
			break;

		case 18:
			stage_18_next_day();
			break;

		case 19:
			stage_19_next_day();
			break;

		case 20:
			stage_20_next_day();
			break;

		case 21:
			stage_21_next_day();
			break;

		case 22:
			stage_22_next_day();
			break;

		case 23:
			stage_23_next_day();
			break;

		case 24:
			stage_24_next_day();
			break;

		case 25:
			stage_25_next_day();
			break;

		case 26:
			stage_26_next_day();
			break;

		case 27:
			stage_27_next_day();
			break;

		case 28:
			stage_28_next_day();
			break;

		default:
			err_here();
	}
}
//------- End of function CampaignEastWest::next_day --------//


//-------- Begin of function CampaignEastWest::intro_text ------//

char* CampaignEastWest::intro_text()
{
	switch( stage_id )
	{
		case 2:
			return stage_2_intro_text();

		case 25:
			return stage_25_intro_text();

		case 26:
			return stage_26_intro_text();

		case 27:
			return stage_27_intro_text();

		case 28:
			return stage_28_intro_text();
	}

	return Campaign::intro_text();
}
//--------- End of function CampaignEastWest::intro_text -------//


//-------- Begin of function CampaignEastWest::goal_text ------//

char* CampaignEastWest::goal_text()
{
	switch( stage_id )
	{
		case 2:
			return stage_2_goal_text();

		case 25:
			return stage_25_goal_text();

		case 26:
			return stage_26_goal_text();

		case 27:
			return stage_27_goal_text();

		case 28:
			return stage_28_goal_text();
	}

	return Campaign::goal_text();
}
//--------- End of function CampaignEastWest::goal_text -------//


//-------- Begin of function CampaignEastWest::plot_text ------//

char* CampaignEastWest::plot_text()
{
	if( plot_category=='A' && plot_id==5 )
	{
		return substitute_text( res_plot.read("PLOTA5"),
			get_nation(cur_monster_nation_recno)->race_name(),
			get_nation(cur_monster_nation_recno)->king_name() );
	}

	return Campaign::plot_text();
}
//--------- End of function CampaignEastWest::plot_text -------//


//-------- Begin of function CampaignEastWest::stage_write_file ------//

void CampaignEastWest::stage_write_file(File* filePtr)
{
	switch( stage_id )
	{
		case 2:
			stage_2_write_file(filePtr);
			break;

		case 25:
			stage_25_write_file(filePtr);
			break;

		case 26:
			stage_26_write_file(filePtr);
			break;

		case 27:
			stage_27_write_file(filePtr);
			break;

		case 28:
			stage_28_write_file(filePtr);
			break;
	}
}
//--------- End of function CampaignEastWest::stage_write_file -------//


//-------- Begin of function CampaignEastWest::stage_read_file ------//

void CampaignEastWest::stage_read_file(File* filePtr)
{
	switch( stage_id )
	{
		case 2:
			stage_2_read_file(filePtr);
			break;

		case 25:
			stage_25_read_file(filePtr);
			break;

		case 26:
			stage_26_read_file(filePtr);
			break;

		case 27:
			stage_27_read_file(filePtr);
			break;

		case 28:
			stage_28_read_file(filePtr);
			break;
	}
}
//--------- End of function CampaignEastWest::stage_read_file -------//


//-------- Begin of function CampaignEastWest::del_nation ------//

void CampaignEastWest::del_nation(int nationRecno)
{
	Campaign::del_nation(nationRecno);

	//-------- reset vars --------//

	if( nationRecno==eastern_nation_recno )
		eastern_nation_recno = 0;

	if( nationRecno==cur_monster_nation_recno )
		cur_monster_nation_recno = 0;

	if( nationRecno==new_kingdom_nation_recno )
	{
		new_kingdom_nation_recno = 0;

		//--- if there are other independent kingdoms remaining, then set new_kingdom_nation_recno to it ---//

		for( int i=nation_count() ; i>0 ; i-- )
		{
			if( is_nation_deleted(i) )
				continue;

			if( get_nation(i)->is_human() &&
				 i != western_nation_recno && i != eastern_nation_recno )
			{
				new_kingdom_nation_recno = i;
				break;
			}
		}
	}
}
//-------- End of function CampaignEastWest::del_nation ------//
