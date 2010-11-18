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

// Filename    : OT_GMENU.H
// Description : header file of game menu text resource


#ifndef __OT_GMENU_H
#define __OT_GMENU_H


#include <OTXTRES.H>

class TextResGameMenu : public TextResource
{
public:
	void	init();

	// version
	char*	str_version( char *verionStr );
	char*	str_demo_version();
	char*	str_beta_version();

	// main menu
	char* str_first_welcome();
	char* str_first_training();
	char* str_single_player();
	char* str_multi_player();
	char* str_scenario_editor();
	char* str_hall_of_fame();
	char* str_credits();
	char* str_player_register();
	char* str_web_site();
	char* str_quit();

	// single player menu
	char* str_training();
	char* str_new_campaign();
	char* str_new_single_game();
	char* str_load_game();
	char* str_load_scenario();
	char* str_cancel();

	// scenario editor menu
	char* str_new_game();
	// char* str_load_game();	// duplicated
	// char* str_cancel();		// duplicated

	// multi player menu
	// char* str_new_game();	// duplicated
	// char* str_load_game();	// duplicated
	// char* str_cancel();		// duplicated

	// player profile menu
	char* str_new_profile();
	char* str_delete_profile();
	char* str_finish_select_profile();
	// char* str_cancel();		// duplicated
	char* str_profile_s1();		// Please select your name from this list
	char* str_profile_s2();		// if your name is not here, click on 'new player' below
	char* str_profile_new();		// Please enter your name
	char* str_profile_del();		// Are you certain that you wish to delete this persno from the list?
	char* str_profile_del_yes();	// yes
	char* str_profile_del_no();	// no
	char* str_profile_lost_login_info();

	// profile error message
	char* str_profile_error_dup();	// Same profile has already been created
	char* str_profile_error_dir(char *dirName);	// Error creating SAVE directory
	char* str_profile_error_create();	// Error creating profile

	// game setting text
	char* str_species_humans();
	char* str_species_fryhtans();
	char* str_start();

	// basic game setting
	char* str_nationality();
	char* str_species();
	char* str_color();
	char* str_ai_nation_count();
	char* str_human();
	char* str_fryhtan();
	char* str_difficulty_level();
	char* str_difficulty_custom();
	char* str_level_in_score();
	char* str_terrain_set();		// "Terrain Type"
	char* str_terrain_set(char terrainSet);	// "Fertile" or "Badlands"
	char* str_building_set();		// "Building Size"
	char* str_building_set(char buildingSet);	// "Large" or "Small"
	char* str_kingdom_of();
	char* str_king_name();

	// advanced setting 1
	char* str_world_map();
	char* str_world_map(char exploreWholeMap);
	char* str_fog_of_war();
	char* str_your_treasure();
	char* str_ai_treasure();
	char* str_ai_aggressiveness();
	char* str_spy_methodology();
	char* str_random_kingdom();

	char* str_off_on(char flag);		// 0 = off, 1 = on
	char* str_treasure(char para);
	char* str_aggressiveness(char para);
	char* str_need_research(char flag);
	char* str_no_yes(char flag);
	char* str_few_to_many(char para);
	char* str_low_to_high(char para);
	char* str_never_to_frequent(char para);

	// advanced setting 2
	char* str_start_up_raw_site();
	char* str_raw_nearby();
	char* str_independent_town();
	char* str_town_resistance();
	char* str_new_town_emerge();
	char* str_new_kingdom_emerge();
	char* str_random_events();

	// goal setting
	char* str_goal();
	char* str_defeat_others();
	char* str_defeat_fryhtan_lairs();
	char* str_defeat_except_ally();
	char* str_reach_population();
	char* str_reach_economy();
	char* str_reach_total_score();
	char* str_goal_time_limit();
	char* str_goal_time_units();	// years

	// multi-player connection setting
	char* str_mp_select_service();
	char* str_mp_your_name();
	char* str_mp_save_file_name();
	char* str_mp_select_session();
	char* str_mp_latency();
	char* str_mp_gem_stones(int);
	char* str_continue();
	char* str_create();
	char* str_join();
	char* str_ready();
	char* str_change_auto_save();
	char* str_mp_error_dplay();
	char* str_mp_error_session();
	char* str_mp_error_join();
	char* str_mp_error_player();
	char* str_mp_wait_host();
	char* str_mp_host_ok();
	char* str_mp_host_abort();
	char* str_mp_refuse_reason(int reasonId);
	char* str_mp_cd_count(); 
	char* str_mp_lack_players( int curPlayers, int maxPlayers);
	char* str_mp_connect_str_corrupt();
	char* str_mp_seed_lost();
	char* str_mp_config_lost();
	char* str_mp_nation_info_lost();
	char* str_mp_host_not_recognize();
	char* str_mp_miss_connect_info();
	char* str_mp_nation_info_incorrect();
	char* str_mp_miss_reserved_1();
	char* str_mp_miss_reserved_2();
	char* str_mp_miss_reserved_3();
	char* str_mp_miss_reserved_4();
	char* str_mp_other_player_fail();
	char* str_mp_service_provider_name(int);	// 1=IPX, 2=TCP/IP, 3=MODEM, 4=SERIAL, 5=IM online
	char* str_mp_service_provider_long(int);	// ditto
	char* str_mp_confirm_quit();
	char* str_mp_gem_stones_inst(int);

	// choose scenario menu
	char* str_scenario_path_name(int pathId);
	char* str_bonus();
	char* str_none_scenario();
	char* str_brief_mode(int);

	// save game/ load game
	char* str_save_game();
	char* str_none_save_game();
	char* str_save();
	char* str_save_new();
	char* str_load();
	char* str_delete_save();
	char* str_empty_game_slot();
	char* str_page_str( int curPage, int maxPage );
	char* str_cannot_delete_slot();
	char* str_king_title(int raceId, char *kingName);
	char* str_game_date();
	char* str_file_date();
	char* str_file_name();
	char* str_ask_overwrite();
	char* str_editor_cant_load();
	char* str_ask_delete_save();
	char* str_ask_desc_save();
	char* str_out_of_disk_space();
	char* str_load_error_open();
	char* str_load_error_header();
	char* str_load_error_old();
	char* str_load_error_general();
	char* str_save_error_create();
	char* str_save_error_header();
	char* str_save_error_general();

	// default config player name
	char* str_default_player_name();

	// dplay lobby error message
	char* str_lobby_error_not_found();
	char* str_lobby_error_no_connect_str();

	// hall of fame text
	char* str_hall_of_fame_score();
	char* str_hall_of_fame_population();
	char* str_hall_of_fame_period();
	char* str_hall_of_fame_difficulty();

	// game end
	char* str_stay_in_game();

	// in game menu
	char* str_option();
	char* str_view_hot_key();
	char* str_restart();
	char* str_retire(int campaignMode);
	char* str_mission_briefing();
	char* str_cheat_to_win();
	char* str_quit_main();
	char* str_quit_game();
	char* str_continue_game();

	char* str_ask_restart();
	char* str_ask_retire(int campaignMode);
	char* str_ask_cheat_win();
	char* str_ask_quit_main();
	char* str_ask_quit_game();

	char* str_se_vol();
	char* str_music();
	char* str_help();
	char* str_display_news();
	char* str_game_speed();
	char* str_scroll_speed();
	char* str_report_bg();
	char* str_show_unit_id();
	char* str_show_unit_path();
	char* str_inside_info();
	char* str_scroll_method();

	char* str_help_options(int i);
	char* str_news_options(int i);
	char* str_report_options(int i);
	char* str_show_icon_options(int i);
	char* str_show_path_options(int i);
	char* str_inside_info_options(int i);
	char* str_scroll_method_options(int i);

	char* str_return();

	// capture screen
	char* str_capture_screen( char* fileName );

	// cheat message
	char* str_cheat_enable();
	char* str_cheat_tech();
	char* str_cheat_immortal(int);
	char* str_cheat_fast_build(int);
	char* str_cheat_ai_info(int);

	// save/load game message
	char* str_load_game_fail();
	char* str_save_success();

	// multi-player
	char* str_mp_wait_player(char* nationStr, char* colorCodeStr );
	char* str_mp_ranking( char *nationName, char colorScheme, int, int, int, int, int, int );
	char* str_hash_help(int line);

	// tips
	char* str_tips_next();
	char* str_tips_prev();
	char* str_tips_never();
	char* str_tips_close();
};

extern TextResGameMenu text_game_menu;
#endif
