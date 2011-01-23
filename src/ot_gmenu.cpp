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

// Filename    : OT_GMENU.CPP
// Description : game menu text resource

#include <ot_gmenu.h>
#include <all.h>
#include <oconfig.h>
#include <ofont.h>


void TextResGameMenu::init()
{
	TextResource::init( DIR_RES"T_GMENU.RES" );
}


// text id 1
// [VERS]Version %1s
char*	TextResGameMenu::str_version( const char *versionStr )
{
	return process( get_format_str( 1, "VERS" ), versionStr );
}

// text id 2
// [VERD]Demo Version
char*	TextResGameMenu::str_demo_version()
{
	return get_format_str( 2, "VERD" );
}

// text id 3
// [VERB]This is a Beta version. Unauthorized distribution of this Beta is strictly prohibited.
char*	TextResGameMenu::str_beta_version()
{
	return get_format_str( 3, "VERB" );
}

// text id 4
// [SPLR]Single Player
char*	TextResGameMenu::str_single_player()
{
	return get_format_str( 4, "SPLR" );
}

// text id 5
// [MPLR]Multi Player
char *TextResGameMenu::str_multi_player()
{
	return get_format_str( 5, "MPLR" );
}

// text id 6
// [SCED]Scenario Editor
char *TextResGameMenu::str_scenario_editor()
{
	return get_format_str( 6, "SCED" );
}

// text id 7
// [HOFM]Hall of Fame
char *TextResGameMenu::str_hall_of_fame()
{
	return get_format_str( 7, "HOFM" );
}

// text id 8
// [CRED]Credits
char* TextResGameMenu::str_credits()
{
	return get_format_str( 8, "CRED" );
}

// text id 9
// [PREG]Player Register
char* TextResGameMenu::str_player_register()
{
	return get_format_str( 9, "PREG" );
}

// text id 10
// [WEBS]Web Site
char* TextResGameMenu::str_web_site()
{
	return get_format_str( 10, "WEBS" );
}

// text id 11
// [QUIT]Quit
char* TextResGameMenu::str_quit()
{
	return get_format_str( 11, "QUIT" );
}

// text id 12
// [TRAN]Training
char *TextResGameMenu::str_training()
{
	return get_format_str( 12, "TRAN" );
}

// text id 13
// [NEWC]New Campaign
char *TextResGameMenu::str_new_campaign()
{
	return get_format_str( 13, "NEWC" );
}

// text id 14
// [NEWS]New Single Game
char *TextResGameMenu::str_new_single_game()
{
	return get_format_str( 14, "NEWS" );
}

// text id 15
// [LO_G]Load Game
char *TextResGameMenu::str_load_game()
{
	return get_format_str( 15, "LO_G" );
}

// text id 15
//// [LOSC]Load Scenario
// [LOSC]Play Scenario
char *TextResGameMenu::str_load_scenario()
{
	return get_format_str( 16, "LOSC" );
}

// text id 17
// [CANC]Cancel
char *TextResGameMenu::str_cancel()
{
	return get_format_str( 17, "CANC" );
}

// text id 18
// [NEWG]New Game
char *TextResGameMenu::str_new_game()
{
	return get_format_str( 18, "NEWG" );
}

// text id 19
// [NEWP]New Player
char *TextResGameMenu::str_new_profile()
{
	return get_format_str( 19, "NEWP" );
}

// text id 20
// [DELP]Delete
char *TextResGameMenu::str_delete_profile()
{
	return get_format_str( 20, "DELP" );
}

// text id 21, finish select profile
// [SELP]Continue
char *TextResGameMenu::str_finish_select_profile()
{
	return get_format_str( 21, "SELP" );
}

// text id 22
// [PRF1]Please select your name from this list
char* TextResGameMenu::str_profile_s1()
{
	return get_format_str( 22, "PRF1" );
}

// text id 23
// [PRF2]If your name is not here, click on "New Player" below
char* TextResGameMenu::str_profile_s2()
{
	return get_format_str( 23, "PRF2" );
}

// text id 24
// [PRFN]Please enter your name
char* TextResGameMenu::str_profile_new()
{
	return get_format_str( 24, "PRFN" );
}

// text id 25
// [PRFD]Are you Certain that you wish to delete this person from the list?
char *TextResGameMenu::str_profile_del()
{
	return get_format_str( 25, "PRFD" );
}

// text id 26
// [PRDY]Yes
char* TextResGameMenu::str_profile_del_yes()
{
	return get_format_str( 26, "PRDY" );
}

// text id 27
// [PRDN]No
char* TextResGameMenu::str_profile_del_no()
{
	return get_format_str( 27, "PRDN" );
}

// text id 28
// [PRED]Same profile has already been created
char *TextResGameMenu::str_profile_error_dup()
{
	return get_format_str( 28, "PRED" );
}

// text id 29
// [PREI]Error creating %1s directory
char* TextResGameMenu::str_profile_error_dir(char *dirName)
{
	return process( get_format_str( 29, "PREI" ), dirName );
}

// text id 30
// [PREC]Error creating profile
char* TextResGameMenu::str_profile_error_create()
{
	return get_format_str( 30, "PREC" );
}

// text id 31, text in choosing humans and fryhtans
// [SP_H]Humans
char* TextResGameMenu::str_species_humans()
{
	return get_format_str( 31, "SP_H" );
}

// text id 32, text in choosing humans and fryhtans
// [SP_F]Fryhtans
char* TextResGameMenu::str_species_fryhtans()
{
	return get_format_str( 32, "SP_F" );
}

// text id 33
// [STRT]Start
char* TextResGameMenu::str_start()
{
	return get_format_str( 33, "STRT" );
}


// text id 34
// [NATH]Nationality
char* TextResGameMenu::str_nationality()
{
	return get_format_str( 34, "NAHS" );
}

// text id 35
// [NATF]Fryhtan Species
char* TextResGameMenu::str_species()
{
	return get_format_str( 35, "NAFS" );
}

// text id 36
// [COLR]Color
char* TextResGameMenu::str_color()
{
	return get_format_str( 36, "COLR" );
}

// text id 37
// [AIN#]Computer Controlled Kingdoms
char* TextResGameMenu::str_ai_nation_count()
{
	return get_format_str( 37, "AIN#" );
}

// text id 38
// [HAH_]Human
char* TextResGameMenu::str_human()
{
	return get_format_str( 38, "NAH_" );
}

// text id 39
// [HAF_]Fryhtan
char* TextResGameMenu::str_fryhtan()
{
	return get_format_str( 39, "NAF_" );
}

// text id 40
// [DIFF]Difficulty Level
char* TextResGameMenu::str_difficulty_level()
{
	return get_format_str( 40, "DIFF" );
}

// text id 41
// [DIFC]Custom
char* TextResGameMenu::str_difficulty_custom()
{
	return get_format_str( 41, "DIFC" );
}

// text id 42
// [LEVE]Level
char* TextResGameMenu::str_level_in_score()
{
	return get_format_str( 42, "LEVE" );
}

// text id 43
// [TER_]Terrain Set
char* TextResGameMenu::str_terrain_set()
{
	return get_format_str( 43, "TER_" );
}

// text id 44
// [TER1]Fertile
// text id 45
// [TER2]Badlands
const char* TextResGameMenu::str_terrain_set(char terrainSet)
{
	switch( terrainSet )
	{
	case 1:
		return get_format_str( 44, "TER1" );
	case 2:
		return get_format_str( 45, "TER2" );
	default:
		err_here();
		return "?";
	}
}

// text id 46
// [BLD_]Building Size
char* TextResGameMenu::str_building_set()
{
	return get_format_str( 46, "BLD_" );
}

// text id 47
// [BLD1]Large
// text id 48
// [BLD2]Small
const char* TextResGameMenu::str_building_set(char buildingSet)
{
	switch( buildingSet )
	{
	case 1:
		return get_format_str( 47, "BLD1" );
	case 2:
		return get_format_str( 48, "BLD2" );
	default:
		err_here();
		return "?";
	}
}

// text id 49
// [KDOF]Kingdom of
char* TextResGameMenu::str_kingdom_of()
{
	return get_format_str( 49, "KDOF" );
}

// text id 50
// [KGOF]King of
char* TextResGameMenu::str_king_name()
{
	return get_format_str( 50, "KGOF" );
}

// text id 51
// [EXPL]World Map
char* TextResGameMenu::str_world_map()
{
	return get_format_str( 51, "EXPL" );
}

// text id 52
// [EXP0]Unexplored
// text id 53
// [EXP1]Explored
char* TextResGameMenu::str_world_map(char exploreWholeMap)
{
	if( !exploreWholeMap )
		return get_format_str( 52, "EXP0" );
	else
		return get_format_str( 53, "EXP1" );
}

// text id 54
// [FOGW]Fog of War
char* TextResGameMenu::str_fog_of_war()
{
	return get_format_str( 54, "FOGW" );
}

// text id 55
// [PTRE]Your Treasure
char* TextResGameMenu::str_your_treasure()
{
	return get_format_str( 55, "PTRE" );
}

// text id 56
// [ATRE]Computer's Treasure
char* TextResGameMenu::str_ai_treasure()
{
	return get_format_str( 56, "ATRE" );
}

// text id 57
// [AAGG]Computer's Aggressiveness
char* TextResGameMenu::str_ai_aggressiveness()
{
	return get_format_str( 57, "AAGG" );
}

// text id 58
// [SPYM]Espionage Methodologies
char* TextResGameMenu::str_spy_methodology()
{
	return get_format_str( 58, "SPYM" );
}

// text id 59
// [RNDK]Random Kingdoms
char* TextResGameMenu::str_random_kingdom()
{
	return get_format_str( 59, "RNDK" );
}

// text id 60
// [COFF]off
// text id 61
// [C_ON]on
char* TextResGameMenu::str_off_on(char flag)
{
	if(!flag)
		return get_format_str( 60, "COFF" );
	else
		return get_format_str( 61, "C_ON" );
}

// text id 62
// [SML0]None
// text id 63
// [SML1]Small
// text id 64
// [SML2]Medium
// text id 65
// [SML3]Large
// text id 66
// [SML4]Huge
const char* TextResGameMenu::str_treasure(char para)
{
	switch( para )
	{
	case OPTION_NONE:
		return get_format_str( 62, "SML0" );
	case OPTION_LOW:
		return get_format_str( 63, "SML1" );
	case OPTION_MODERATE:
		return get_format_str( 64, "SML2" );
	case OPTION_HIGH:
		return get_format_str( 65, "SML3" );
	case OPTION_VERY_HIGH:
		return get_format_str( 66, "SML4" );
	default:
		err_here();
		return "?";
	}
}

// text id 67
// [AGG0]Very Low
// text id 68
// [AGG1]Low
// text id 69
// [AGG2]Medium
// text id 70
// [AGG3]Great
// text id 71
// [AGG4]Very Great
const char* TextResGameMenu::str_aggressiveness(char para)
{
	switch(para)
	{
	case OPTION_NONE:
		return get_format_str( 67, "AGG0" );
	case OPTION_LOW:
		return get_format_str( 68, "AGG1" );
	case OPTION_MODERATE:
		return get_format_str( 69, "AGG2" );
	case OPTION_HIGH:
		return get_format_str( 70, "AGG3" );
	case OPTION_VERY_HIGH:
		return get_format_str( 71, "AGG4" );
	default:
		err_here();
		return "?";
	}
}

// text id 72
// [SPY0]Must Research
// text id 73
// [SPY1]All Available
char* TextResGameMenu::str_need_research(char flag)
{
	if( !flag )
		return get_format_str( 72, "SPY0" );
	else
		return get_format_str( 73, "SPY1" );
}

// text id 74
// [CFGN]No
// text id 75
// [CFGY]Yes
char* TextResGameMenu::str_no_yes(char flag)
{
	if( !flag )
		return get_format_str( 74, "CFGN" );
	else
		return get_format_str( 75, "CFGY" );
}

// text id 76
// [FEW1]Few
// text id 77
// [FEW2]More
// text id 78
// [FEW3]Many
const char* TextResGameMenu::str_few_to_many(char para)
{
	switch(para)
	{
	case 1:
		return get_format_str( 76, "FEW1" );
	case 2:
		return get_format_str( 77, "FEW2" );
	case 3:
		return get_format_str( 78, "FEW3" );
	default:
		err_here();
		return "?";
	}
}

// text id 79
// [LOW1]Low
// text id 80
// [LOW2]Medium
// text id 81
// [LOW3]High

const char* TextResGameMenu::str_low_to_high(char para)
{
	switch(para)
	{
	case 1:
		return get_format_str( 79, "LOW1" );
	case 2:
		return get_format_str( 80, "LOW2" );
	case 3:
		return get_format_str( 81, "LOW3" );
	default:
		err_here();
		return "?";
	}
}

// text id 82
// [FRQ0]Never
// text id 83
// [FRQ1]Seldom
// text id 84
// [FRQ2]Occasional
// text id 85
// [FRQ3]Frequent
const char* TextResGameMenu::str_never_to_frequent(char para)
{
	switch( para)
	{
	case OPTION_NONE:
		return get_format_str( 82, "FRQ0" );
	case OPTION_LOW:
		return get_format_str( 83, "FRQ1" );
	case OPTION_MODERATE:
		return get_format_str( 84, "FRQ2" );
	case OPTION_HIGH:
		return get_format_str( 85, "FRQ3" );
	default:
		err_here();
		return "?";
	}
}

// text id 86
// [RAW#]Natural Resources at Start
char* TextResGameMenu::str_start_up_raw_site()
{
	return get_format_str( 86, "RAW#" );
}

// text id 87
// [RAWN]Natural Resources Nearby
char* TextResGameMenu::str_raw_nearby()
{
	return get_format_str( 87, "RAWN" );
}

// text id 88
// [INDT]Independent Towns
char* TextResGameMenu::str_independent_town()
{
	return get_format_str( 88, "INDT" );
}

// text id 89
// [INDR]Independent Town Resistance
char* TextResGameMenu::str_town_resistance()
{
	return get_format_str( 89, "INDR" );
}

// text id 90
// [NEWT]New Towns Emerge
char* TextResGameMenu::str_new_town_emerge()
{
	return get_format_str( 90, "NEWT" );
}

// text id 91
// [NEWK]New Kingdoms Emerge
char* TextResGameMenu::str_new_kingdom_emerge()
{
	return get_format_str( 91, "NEWK" );
}

// text id 92
// [RNDE]Random Events
char* TextResGameMenu::str_random_events()
{
	return get_format_str( 92, "RNDE" );
}

// text id 93
// [DEAO]Defeated All Others
char* TextResGameMenu::str_defeat_others()
{
	return get_format_str( 93, "DEAO" );
}

// text id 94
// [DEFL]Destroyed All Independent Fryhtan Lairs
char* TextResGameMenu::str_defeat_fryhtan_lairs()
{
	return get_format_str( 94, "DEFL" );
}

// text id 95
// [DE-A]Defeated All Except One Allied Kingdom
char* TextResGameMenu::str_defeat_except_ally()
{
	return get_format_str( 95, "DE-A" );
}


// text id 96
// [RPOP]Achieved A Population Of
char* TextResGameMenu::str_reach_population()
{
	return get_format_str( 96, "RPOP" );
}

// text id 97
// [RECO]Reached An Economic Score Of
char* TextResGameMenu::str_reach_economy()
{
	return get_format_str( 97, "RECO" );
}

// text id 98
// [RSCO]Reached A Total Score Of
char* TextResGameMenu::str_reach_total_score()
{
	return get_format_str( 98, "RSCO" );
}

// text id 99
// [TLIM]Achieved The Selected Within
char* TextResGameMenu::str_goal_time_limit()
{
	return get_format_str( 99, "TLIM" );
}

// text id 100
// [YEAR]Years
char* TextResGameMenu::str_goal_time_units()
{
	return get_format_str( 100, "YEAR" );
}


// text id 101
// [MPSP]Select Connection Method
char* TextResGameMenu::str_mp_select_service()
{
	return get_format_str( 101, "MPSP" );
}

// text id 102
// [MPYN]Your Name
char* TextResGameMenu::str_mp_your_name()
{
	return get_format_str( 102, "MPYN" );
}

// text id 103
// [MPSG]Save Game File Name
char* TextResGameMenu::str_mp_save_file_name()
{
	return get_format_str( 103, "MPSG" );
}

// text id 104
// [MPSS]Select A Game
char* TextResGameMenu::str_mp_select_session()
{
	return get_format_str( 104, "MPSS" );
}

// text id 105
// [MPNL]Network Latency
char* TextResGameMenu::str_mp_latency()
{
	return get_format_str( 105, "MPNL" );
}

// text id 106
// [CONT]Continue
char* TextResGameMenu::str_continue()
{
	return get_format_str( 106, "CONT" );
}

// text id 107
// [CREA]Create
char* TextResGameMenu::str_create()
{
	return get_format_str( 107, "CREA" );
}

// text id 108
// [JOIN]Join
char* TextResGameMenu::str_join()
{
	return get_format_str( 108, "JOIN" );
}

// text id 109
// [RDY_]Ready
char* TextResGameMenu::str_ready()
{
	return get_format_str( 109, "RDY_" );
}

// text id 110
// [WASV]It is not recommended to use this save game file name, do you wish to continue?
char* TextResGameMenu::str_change_auto_save()
{
	return get_format_str( 110, "WASV" );
}

// text id 111
// [MPED]Cannot initialize DirectPlay.
char* TextResGameMenu::str_mp_error_dplay()
{
	return get_format_str( 111, "MPED" );
}

// text id 112
// [MPES]Cannot create session
char* TextResGameMenu::str_mp_error_session()
{
	return get_format_str( 112, "MPES" );
}

// text id 113
// [MPEJ]Cannot join session
char* TextResGameMenu::str_mp_error_join()
{
	return get_format_str( 113, "MPEJ" );
}

// text id 114
// [MPEP]Cannot create player
char* TextResGameMenu::str_mp_error_player()
{
	return get_format_str( 114, "MPEP" );
}

// text id 115
// [MPWH]Waiting response from host...
char* TextResGameMenu::str_mp_wait_host()
{
	return get_format_str(115, "MPWH" );
}

// text id 116
// [MPOK]Ok
char* TextResGameMenu::str_mp_host_ok()
{
	return get_format_str(116, "MPOK" );
}

// text id 117
// [MPHA]The host has aborted the game
char* TextResGameMenu::str_mp_host_abort()
{
	return get_format_str( 117, "MPHA" );
}


// text id 118
// [RFU0]The host refuses you
// text id 119
// [RFU1]Cannot join a new game session
// text id 120
// [RFU2]Cannot join a load game session
// text id 121
// [RFU3]Cannot join because the save game is different from the host
// text id 122
// [RFU4]Cannot join because the host does not accept your game version
// text id 123
// [RFU5]Cannot join because the session is full
// text id 124 to 127
// [RFU6] to [RFU9]reserved
char* TextResGameMenu::str_mp_refuse_reason(int reasonId)
{
	switch( reasonId )
	{
	case 1: //REFUSE_HOST_NEW_GAME:
		return get_format_str( 119, "RFU1");
	case 2: //REFUSE_HOST_LOAD_GAME:
		return get_format_str( 120, "RFU2");
	case 3: //REFUSE_LOAD_GAME_UNSYNC:
		return get_format_str( 121, "RFU3");
	case 4: //REFUSE_VERSION:
		return get_format_str( 122, "RFU4");
	case 5: //REFUSE_SEAT_FULL:
		return get_format_str( 123, "RFU5");
	case 0: //REFUSE_REASON_OTHER:
	default:
		return get_format_str(118, "RFU0" );
	}
}


// text id 128
// [CD4P]There are not enough Seven Kingdoms II CDROMs to start the game. Each CDROM supports four players.
char* TextResGameMenu::str_mp_cd_count()
{
	return get_format_str(128, "CD4P" );
}

// text id 129
// [MPLP]This multiplayer saved game needs %2d human players while now there %1p{is|are} only %1d human %1p{player|players}. The game cannot start.
// text id 130
// [MPGP]This multiplayer saved game can only support %2d human %2p{player|players} while now there are %1d human players. The game cannot start.
char* TextResGameMenu::str_mp_lack_players( int curPlayers, int maxPlayers)
{
	if( curPlayers < maxPlayers )
		return process( get_format_str(129, "MPLP"), curPlayers, maxPlayers );
	else
		return process( get_format_str(130, "MPGP"), curPlayers, maxPlayers );
}

// text id 131
// [CNSC]Connection string from host is corrupted
char* TextResGameMenu::str_mp_connect_str_corrupt()
{
	return get_format_str( 131, "CNSC" );
}

// text id 132
// [SEDL]Cannot get random seeds from the host.
char* TextResGameMenu::str_mp_seed_lost()
{
	return get_format_str( 132, "SEDL" );
}

// text id 133
// [CFGL]Cannot get game configuration info from the host.
char* TextResGameMenu::str_mp_config_lost()
{
	return get_format_str( 133, "CFGL" );
}

// text id 134
// [N_IL]Cannot get kingdom information from the host.
char* TextResGameMenu::str_mp_nation_info_lost()
{
	return get_format_str( 134, "N_IL" );
}

// text id 135
// [-RCG]The host cannot recognize your machine.
char* TextResGameMenu::str_mp_host_not_recognize()
{
	return get_format_str( 135, "-RCG" );
}

// text id 136
// [-CII]Cannot receive important information from the host
char* TextResGameMenu::str_mp_miss_connect_info()
{
	return get_format_str( 136, "-CII" );
}

// text id 137
// [N_II]Incorrect kingdom information received from the host.
char* TextResGameMenu::str_mp_nation_info_incorrect()
{
	return get_format_str( 137, "N_II" );
}


// text id 138
// [MPO1]Reserved
char* TextResGameMenu::str_mp_miss_reserved_1()
{
	return get_format_str( 138, "MPO1" );
}

// text id 139
// [MPO2]Reserved
char* TextResGameMenu::str_mp_miss_reserved_2()
{
	return get_format_str( 139, "MPO2" );
}

// text id 140
// [MPO3]Reserved
char* TextResGameMenu::str_mp_miss_reserved_3()
{
	return get_format_str( 140, "MPO3" );
}

// text id 141
// [MPO4]Reserved
char* TextResGameMenu::str_mp_miss_reserved_4()
{
	return get_format_str( 141, "MPO4" );
}

// text id 142
// [MPPL]Some player(s) encountered errors when initializing the game.
char* TextResGameMenu::str_mp_other_player_fail()
{
	return get_format_str( 142, "MPPL" );
}

// text id 143
// [SCP0]Custom
// text id 144
// [SCP1]Pre-made
// text id 145
// [SCP#]Supp. %1d
char* TextResGameMenu::str_scenario_path_name(int pathId)
{
	switch( pathId )
	{
	case 0:
		return get_format_str( 143, "SCP0" );
	case 1:
		return get_format_str( 144, "SCP1" );
	default:
		return process( get_format_str( 145, "SCP#"), pathId-1 );
	}
}

// text id 146
// [BONU]Bonus
char* TextResGameMenu::str_bonus()
{
	return get_format_str( 146, "BONU" );
}

// text id 147
// [0SCN]Scenario files not found.
char* TextResGameMenu::str_none_scenario()
{
	return get_format_str( 147, "0SCN" );
}

// text id 148
// [SV_G]Save Game
char* TextResGameMenu::str_save_game()
{
	return get_format_str( 148, "SV_G" );
}

// text id 149
// [0SAV]You haven't saved any games yet.
char* TextResGameMenu::str_none_save_game()
{
	return get_format_str( 149, "0SAV" );
}

// text id 150
// [SAVE]Save
char* TextResGameMenu::str_save()
{
	return get_format_str( 150, "SAVE" );
}

// text id 151
// [SAVN]Save New
char* TextResGameMenu::str_save_new()
{
	return get_format_str( 151, "SAVN" );
}

// text id 152
// [LOAD]Load
char* TextResGameMenu::str_load()
{
	return get_format_str( 152, "LOAD" );
}

// text id 153
// [DELS]Delete
char* TextResGameMenu::str_delete_save()
{
	return get_format_str( 153, "DELS" );
}

// text id 154
// [EMGS]Empty Game Slot
char* TextResGameMenu::str_empty_game_slot()
{
	return get_format_str( 154, "EMGS" );
}

// text id 155
// [PAGE]page %1/%2
char* TextResGameMenu::str_page_str( int curPage, int maxPage )
{
	return process( get_format_str( 155, "PAGE" ), curPage, maxPage );
}

// text id 156
// [-DSL]Cannot delete this slot
char* TextResGameMenu::str_cannot_delete_slot()
{
	return get_format_str( 156, "-DSL" );
}

// text id 157
// [KTIT]%1?{King|All High} %2s
char* TextResGameMenu::str_king_title(int raceId, char *kingName)
{
	return process( get_format_str( 157, "KTIT" ), raceId >= 0, kingName);
}

// text id 158
// [GDAT]Game Date: 
char* TextResGameMenu::str_game_date()
{
	return get_format_str( 158, "GDAT" );
}

// text id 159
// [FDAT]File Date: 
char* TextResGameMenu::str_file_date()
{
	return get_format_str( 159, "FDAT" );
}

// text id 160
// [FNAM]File Name: 
char* TextResGameMenu::str_file_name()
{
	return get_format_str( 160, "FNAM" );
}

// text id 161
// [AOVW]It will overwrite the existing saved game, proceed ?
char* TextResGameMenu::str_ask_overwrite()
{
	return get_format_str( 161, "AOVW" );
}

// text id 162
// [SE-L]Cannot load in scenario editor
char* TextResGameMenu::str_editor_cant_load()
{
	return get_format_str( 162, "SE-L" );
}

// text id 163
// [ADLS]This saved game will be deleted, proceed ?
char* TextResGameMenu::str_ask_delete_save()
{
	return get_format_str( 163, "ADLS" );
}

// text id 164
// [DESV]Description of save game
char* TextResGameMenu::str_ask_desc_save()
{
	return get_format_str( 164, "DESV" );
}

// text id 165
// [OUTD]Insufficient disk space ! The game is not saved.
char* TextResGameMenu::str_out_of_disk_space()
{
	return get_format_str( 165, "OUTD" );
}

// text id 166
// [LE_O]Cannot open save game file
char* TextResGameMenu::str_load_error_open()
{
	return get_format_str( 166, "LE_O" );
}

// text id 167
// [LE_H]Cannot read file header
char* TextResGameMenu::str_load_error_header()
{
	return get_format_str( 167, "LE_H" );
}

// text id 168
// [LE_P]Save game incompatible
char* TextResGameMenu::str_load_error_old()
{
	return get_format_str( 168, "LE_P" );
}

// text id 169
// [LE_G]Load game error
char* TextResGameMenu::str_load_error_general()
{
	return get_format_str( 169, "LE_G" );
}

// text id 170
// [SE_C]Error creating saved game file.
char* TextResGameMenu::str_save_error_create()
{
	return get_format_str( 170, "SE_C" );
}

// text id 171
// [SE_H]Error creating saved game header.
char* TextResGameMenu::str_save_error_header()
{
	return get_format_str( 171, "SE_H" );
}

// text id 172
// [SE_G]Error writing saved game data.
char* TextResGameMenu::str_save_error_general()
{
	return get_format_str( 172, "SE_G" );
}

// text id 173 // default player name
// [DFPN]New Player
char* TextResGameMenu::str_default_player_name()
{
	return get_format_str( 173, "DFPN" );
}

// text id 174
// [LOB?]Cannot connect to lobby
char* TextResGameMenu::str_lobby_error_not_found()
{
	return get_format_str( 174, "LOB?" );
}

// text id 175
// [LOBC]Cannot get connection string from lobby
char* TextResGameMenu::str_lobby_error_no_connect_str()
{
	return get_format_str( 175, "LOBC" );
}

// text id 176
// [LOB1]Lobby error reserved 1

// text id 177
// [LOB2]Lobby error reserved 2

// text id 178
// [LOB3]Lobby error reserved 3

// text id 179
// [LOB4]Lobby error reserved 4

// text id 180
// [HOFS]Score: 
char* TextResGameMenu::str_hall_of_fame_score()
{
	return get_format_str( 180, "HOFS" );
}

// text id 181
// [HOFP]Population: 
char* TextResGameMenu::str_hall_of_fame_population()
{
	return get_format_str( 181, "HOFP" );
}

// text id 182
// [HOFT]Period: 
char* TextResGameMenu::str_hall_of_fame_period()
{
	return get_format_str( 182, "HOFT" );
}

// text id 183
// [HOFD]Difficulty: 
char* TextResGameMenu::str_hall_of_fame_difficulty()
{
	return get_format_str( 183, "HOFD" );
}

// text id 184
// [1WEL]Welcome to Seven Kingdoms II. You must register your name first.
char* TextResGameMenu::str_first_welcome()
{
	return get_format_str( 184, "1WEL" );
}

// text id 185
// [1LES]You are about to start a Training Game. It will help you to get up to speed with the game quickly.
char* TextResGameMenu::str_first_training()
{
	return get_format_str( 185, "1LES" );
}

// text id 186
// [STAY]Do you want to continue to stay in the game?
char* TextResGameMenu::str_stay_in_game()
{
	return get_format_str( 186, "STAY" );
}

// text id 187
// [OPTN]Options
char* TextResGameMenu::str_option()
{
	return get_format_str( 187, "OPTN" );
}

// text id 188
// [RSTT]Restart Game
char* TextResGameMenu::str_restart()
{
	return get_format_str( 188, "RSTT" );
}

// text id 189
// [RETI]Retire
// text id 190
// [RETR]Retreat
char* TextResGameMenu::str_retire(int campaignMode)
{
	if( !campaignMode )
	{
		return get_format_str( 189, "RETI" );
	}
	else
	{
		return get_format_str( 190, "RETR" );
	}
}

// text id 191
// [MISB]Mission Briefing
char* TextResGameMenu::str_mission_briefing()
{
	return get_format_str( 191, "MISB" );
}

// text id 192
// [CWIN]Campaign Cheat to Win
char* TextResGameMenu::str_cheat_to_win()
{
	return get_format_str( 192, "CWIN" );
}

// text id 193
// [QTMM]Quit to Main Menu
char* TextResGameMenu::str_quit_main()
{
	return get_format_str( 193, "QTMM" );
}

// text id 194
// [QWIN]Quit to Windows
char* TextResGameMenu::str_quit_game()
{
	return get_format_str( 194, "QWIN" );
}

// text id 195
// [CONG]Continue Game
char* TextResGameMenu::str_continue_game()
{
	return get_format_str( 195, "CONG" );
}

// text id 196
// [ASRS]Do you really want to restart?
char* TextResGameMenu::str_ask_restart()
{
	return get_format_str( 196, "ASRS" );
}

// text id 197
// [ARTI]Do you really want to retire?
// text id 198
// [ARTR]Do you really want to retreat?
char* TextResGameMenu::str_ask_retire(int campaignMode)
{
	if( !campaignMode )
		return get_format_str( 197, "ARTI" );
	else
		return get_format_str( 198, "ARTR" );
}

// text id 199
// [ACWI]Do you really want to win this game by cheating?
char* TextResGameMenu::str_ask_cheat_win()
{
	return get_format_str( 199, "ACWI" );
}

// text id 200
// [AQMM]Do you really want to quit to the Main Menu?
char* TextResGameMenu::str_ask_quit_main()
{
	return get_format_str( 200, "AQMM" );
}

// text id 201
// [AQWI]Do you really want to quit to Windows?
char* TextResGameMenu::str_ask_quit_game()
{
	return get_format_str( 201, "AQWI" );
}


// text id 202
// [SEVO]Sound Effects Volume
char* TextResGameMenu::str_se_vol()
{
	return get_format_str( 202, "SEVO" );
}

// text id 203
// [MUSI]Music
char* TextResGameMenu::str_music()
{
	return get_format_str( 203, "MUSI" );
}

// text id 204
// [HELP]Help
char* TextResGameMenu::str_help()
{
	return get_format_str( 204, "HELP" );
}

// text id 205
// [DNEW]Display News
char* TextResGameMenu::str_display_news()
{
	return get_format_str( 205, "DNEW" );
}

// text id 206
// [GSPD]Game Speed
char* TextResGameMenu::str_game_speed()
{
	return get_format_str( 206, "GSPD" );
}

// text id 207
// [SSPD]Scrolling Speed
char* TextResGameMenu::str_scroll_speed()
{
	return get_format_str( 207, "SSPD" );
}

// text id 208
// [REPB]Reports Background
char* TextResGameMenu::str_report_bg()
{
	return get_format_str( 208, "REPB" );
}

// text id 209
// [SUID]Show Unit Identities
char* TextResGameMenu::str_show_unit_id()
{
	return get_format_str( 209, "SUID" );
}

// text id 210
// [SUPA]Show Unit Paths
char* TextResGameMenu::str_show_unit_path()
{
	return get_format_str( 210, "SUPA" );
}

// text id 211
// [IINF]Inside Information
char* TextResGameMenu::str_inside_info()
{
	return get_format_str( 211, "IINF" );
}

// text id 212
// [SMTD]Scrolling Method
char* TextResGameMenu::str_scroll_method()
{
	return get_format_str( 212, "SMTD" );
}

// text id 213 - 215
// [HLP0]None
// [HLP1]Brief
// [HLP2]Detailed
const char* TextResGameMenu::str_help_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 213, "HLP0" );
	case 1:
		return get_format_str( 214, "HLP1" );
	case 2:
		return get_format_str( 215, "HLP2" );
	default:
		return "";
	}
}

// text id 216 - 217
// [NEW0]Major
// [NEW1]All
const char* TextResGameMenu::str_news_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 216, "NEW0" );
	case 1:
		return get_format_str( 217, "NEW1" );
	default:
		return "";
	}
}

// text id 218 - 219
// [REP0]Transparent
// [REP1]Opaque
const char* TextResGameMenu::str_report_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 218, "REP0" );
	case 1:
		return get_format_str( 219, "REP1" );
	default:
		return "";
	}
}

// text id 220 - 221
// [SUI0]Cursor
// [SUI1]Always
const char* TextResGameMenu::str_show_icon_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 220, "SUI0" );
	case 1:
		return get_format_str( 221, "SUI1" );
	default:
		return "";
	}
}

// text id 222 - 225
// [SUP0]None
// [SUP1]Main Map
// [SUP2]World Map
// [SUP3]Both
const char* TextResGameMenu::str_show_path_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 222, "SUP0" );
	case 1:
		return get_format_str( 223, "SUP1" );
	case 2:
		return get_format_str( 224, "SUP2" );
	case 3:
		return get_format_str( 225, "SUP3" );
	default:
		return ""; 
	}
}

// text id 226 - 228
// [IIF0]None
// [IIF1]Cursor
// [IIF2]Always
const char* TextResGameMenu::str_inside_info_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 226, "IIF0" );
	case 1:
		return get_format_str( 227, "IIF1" );
	case 2:
		return get_format_str( 228, "IIF2" );
	default:
		return ""; 
	}
}

// text id 229 - 230
// [SMD0]Comfort
// [SMD1]Speed
const char* TextResGameMenu::str_scroll_method_options(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 229, "SMD0" );
	case 1:
		return get_format_str( 230, "SMD1" );
	default:
		return "";
	}
}

// text id 231
// [RETU]Return 
char* TextResGameMenu::str_return()
{
	return get_format_str( 231, "RETU" );
}


// text id 232 - 239 reserved

// text id 240
// [CAPF]The current screen has been written to file %1s.
char* TextResGameMenu::str_capture_screen( char* fileName )
{
	return process( get_format_str( 240, "CAPF" ),
		fileName );
}

// text id 241
// [CHEA]Cheat Mode Enabled.
char* TextResGameMenu::str_cheat_enable()
{
	return get_format_str( 241, "CHEA" );
}

// text id 242
// [CHTG]Your technology has advanced.
char* TextResGameMenu::str_cheat_tech()
{
	return get_format_str( 242, "CHTG" );
}

// text id 243
// [CH_M]King immortal mode is now disabled.
// text id 244
// [CHIM]Your king is now immortal.
char* TextResGameMenu::str_cheat_immortal(int flag)
{
	if( !flag )
		return get_format_str( 243, "CH_M" );
	else
		return get_format_str( 244, "CHIM" );
}

// text id 245, 246
// [CHSB]Fast build is now disabled
// [CHFB]Fast build is now enabled
char* TextResGameMenu::str_cheat_fast_build(int flag)
{
	if( !flag )
		return get_format_str( 245, "CHSB" );
	else
		return get_format_str( 246, "CHFB" );
}

// text id 247, 248
// [CHHI]Now AI info will not be displayed.
// [CHSI]Now AI info will be displayed.
char* TextResGameMenu::str_cheat_ai_info(int flag)
{
	if( !flag )
		return get_format_str( 247, "CHHI" );
	else
		return get_format_str( 248, "CHSI" );
}

// text id 249 - 255 reserved

// text id 256
// [LOFL]Fail Loading Game
char* TextResGameMenu::str_load_game_fail()
{
	return get_format_str( 256, "LOFL" );
}

// text id 257
// [SVSU]Game Saved Successfully
char* TextResGameMenu::str_save_success()
{
	return get_format_str( 257, "SVSU" );
}

// text id 258
// [WAIT]Waiting for %1s %2s
char* TextResGameMenu::str_mp_wait_player(char* nationStr, char* colorCodeStr )
{
	return process( get_format_str( 258, "WAIT" ),
		nationStr, colorCodeStr );
}

// text id 259 - 269

// text id 270
// [TIPN]Next Tip
char* TextResGameMenu::str_tips_next()
{
	return get_format_str( 270, "TIPN" );
}

// text id 271
// [TIPP]Previous Tip
char* TextResGameMenu::str_tips_prev()
{
	return get_format_str( 271, "TIPP" );
}

// text id 272
// [TIPD]Don't Show Tips
char* TextResGameMenu::str_tips_never()
{
	return get_format_str( 272, "TIPD" );
}

// text id 273
// [TIPC]Close
char* TextResGameMenu::str_tips_close()
{
	return get_format_str( 273, "TIPC" );
}


// text id 274
// [GOAW]You will be victorious when you have:
char* TextResGameMenu::str_goal()
{
	return get_format_str( 274, "GOAW" );
}

// text id 275 - 276
// [BRF0]Detail
// [BRF1]Summary
char* TextResGameMenu::str_brief_mode(int briefMode)
{
	if( briefMode == 0 )
		return get_format_str( 275, "BRF0" );
	else
		return get_format_str( 276, "BRF1" );
}

// text id 277 - 280
// [IPXS]IPX
// [TCPS]TCP/IP
// [MODS]Modem
// [COMS]Serial
// text id 281 - 285 reserved
const char* TextResGameMenu::str_mp_service_provider_name(int sp)
{
	switch( sp )
	{
	case 1:
		return get_format_str( 277, "IPXS" );
	case 2:
		return get_format_str( 278, "TCPS" );
	case 3:
		return get_format_str( 279, "MODS" );
	case 4:
		return get_format_str( 280, "COMS" );
	case 5:
		return "IMOnline";	// do not let Ubi to translate get_format_str( 281, "IMOS" );
	default:
		return "";
	};
}

// text id 286 - 289
// [IPXL]Join a local area network game
// [TCPL]Join a local area network or Internet game
// [MODL]Join with another player by Modem
// [COML]Join with another player linked by serial cable
// text id 290 - 294 reserved
const char* TextResGameMenu::str_mp_service_provider_long(int sp)
{
	switch( sp )
	{
	case 1:
		return get_format_str( 286, "IPXL" );
	case 2:
		return get_format_str( 287, "TCPL" );
	case 3:
		return get_format_str( 288, "MODL" );
	case 4:
		return get_format_str( 289, "COML" );
	case 5:
		return "Join Interactive Magic Online"; //get_format_str( 290, "IMOL" );
	default:
		return "";
	};
}

// text id 295
// [VEHK]View Hot Key
char* TextResGameMenu::str_view_hot_key()
{
	return get_format_str( 295, "VEHK" );
}

// text id 296
// [GEMS]Gem Stones (%1d)
char* TextResGameMenu::str_mp_gem_stones( int gemStones)
{
	return process( get_format_str( 296, "GEMS" ),
		gemStones );
}

// text id 297
// [MPAQ]Do you really want to cancel this game and quit?
char* TextResGameMenu::str_mp_confirm_quit()
{
	return get_format_str( 297, "MPAQ" );
}

// text id 298
// [PRFL]Warning: This will delete all information in your multiplayer lobby account that is linked to this player profile.
char* TextResGameMenu::str_profile_lost_login_info()
{
	return get_format_str( 298, "PRFL" );
}

// text id 299
// [#rL0]%1s %2s : P:%3d M:%4d E:%5d R:%6d FB:%7d Total:%8d
char* TextResGameMenu::str_mp_ranking( char *nationName, char colorScheme, int ps, int ms, int es, int rs, int fbs, int ts )
{
	char colorBoxStr[2] = " ";
	colorBoxStr[0] = FIRST_NATION_COLOR_CODE_IN_TEXT + colorScheme;

	return process( get_format_str( 299, "#rL0" ),
		nationName, colorBoxStr, ps, ms, es, rs, fbs, ts );
}

// text id 300 - 301 (multiplayer load game #? help)
// [#?L0]#r - ranking of players
// [#?L1]#? - help
const char* TextResGameMenu::str_hash_help(int line)
{
	switch( line )
	{
	case 0:
		return get_format_str( 300, "#?L0" );
	case 1:
		return get_format_str( 301, "#?L1" );
	default:
		return "";
	}
}

// text id 302 - 319 reserved


// text id 320
// [GEMI]Increase starting treasure %1,3d
char* TextResGameMenu::str_mp_gem_stones_inst(int gain)
{
	return process( get_format_str( 320, "GEMI" ),
		gain );
}
