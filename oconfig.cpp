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

//Filename    : OCONFIG.CPP
//Description : Config Object

#include <OCONFIG.H>
#include <OSYS.H>
#include <OHELP.H>
#include <ONATION.H>
#include <OMONSRES.H>
#include <OUNITRES.H>
#include <OFIRMRES.H> 
#include <OMODEID.H>
#include <OGAME.H>
//#include <OT_GMENU.H>

// ------- define difficult table ---------//

static char  table_ai_human_nation_count[8]   = { 1, 2, 2, 3, 3, 3, 3, 3 };
static char  table_ai_monster_nation_count[8] = { 1, 1, 2, 2, 3, 3, 3, 3 };
static short table_start_up_cash[8] = { OPTION_VERY_HIGH, OPTION_VERY_HIGH, OPTION_HIGH, OPTION_MODERATE, OPTION_MODERATE, OPTION_MODERATE, OPTION_LOW, OPTION_LOW };
static short table_ai_start_up_cash[8] = { OPTION_LOW, OPTION_LOW, OPTION_MODERATE, OPTION_MODERATE, OPTION_MODERATE, OPTION_HIGH, OPTION_VERY_HIGH, OPTION_VERY_HIGH };
static char  table_ai_aggressiveness[8] = { OPTION_NONE, OPTION_NONE, OPTION_LOW, OPTION_MODERATE, OPTION_MODERATE, OPTION_HIGH, OPTION_HIGH, OPTION_VERY_HIGH };
static short table_start_up_independent_town[8] = { 30, 30, 30, 15, 15, 15, 7, 7 };
static short table_start_up_raw_site[8] = { 7, 7, 7, 6, 5, 5, 4, 3 };
static char  table_explore_whole_map[8] = { 1, 1, 1, 1, 0, 0, 0, 0 };
static char  table_fog_of_war[8] = { 0, 0, 0, 0, 0, 1, 1, 1 };
static char  table_new_independent_town_emerge[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
static char  table_independent_town_resistance[8] = { OPTION_LOW, OPTION_LOW, OPTION_MODERATE, OPTION_MODERATE, OPTION_MODERATE, OPTION_HIGH, OPTION_HIGH, OPTION_HIGH };
static char  table_random_event_frequency[8] = { OPTION_NONE, OPTION_NONE, OPTION_LOW, OPTION_LOW, OPTION_LOW, OPTION_MODERATE, OPTION_MODERATE, OPTION_MODERATE };
static char  table_new_nation_emerge[8] = { 0, 0, 0, 1, 1, 1, 1, 1 };
static char  table_monster_type[8] = { OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT, OPTION_MONSTER_INTELLIGENT };
static char  table_start_up_has_mine_nearby[8] = { 1, 1, 1, 0, 0, 0, 0, 0 };
static char  table_random_start_up[8] = { 0, 0, 0, 0, 0, 0, 1, 1 };
static char  table_spy_methodology[8] = { 1, 1, 1, 1, 0, 0, 0, 0 };

//--------- Begin of function Config::init -----------//

void Config::init()
{
//	err_when( Config::PLAYER_NAME_LEN != NationArray::CUSTOM_NAME_LEN );

	default_game_setting();
	default_cheat_setting();
	default_local_game_setting();
	default_preference();
}
//--------- End of function Config::init --------//


//--------- Begin of function Config::deinit -----------//

void Config::deinit()
{
	config.save("CONFIG.DAT");		// save the config when the game quits

	default_game_setting();
	default_local_game_setting();
	default_preference();
}
//--------- End of function Config::deinit --------//


//--------- Begin of function Config::default_game_setting ---------//
void Config::default_game_setting()
{
	// -------- GLOBAL GAME SETTING -------- //

	ai_human_nation_count = 4;
	ai_monster_nation_count = 2;
	start_up_cash = OPTION_MODERATE;
	ai_start_up_cash = OPTION_MODERATE;
	ai_aggressiveness = OPTION_MODERATE;
	start_up_independent_town = 15;
	start_up_raw_site = 6;
	difficulty_level = OPTION_CUSTOM;

	explore_whole_map = 1;
	fog_of_war   = 0;

	terrain_set = 1;
	building_size = 1;
	latitude = 45;
	weather_effect = 1;		// damage done by weather
	land_mass = OPTION_MODERATE;

	new_independent_town_emerge = 1;
	independent_town_resistance = OPTION_MODERATE;
	random_event_frequency = OPTION_NONE;
	monster_type = OPTION_MONSTER_INTELLIGENT;
	new_nation_emerge = 1;
	start_up_has_mine_nearby = 0;
	random_start_up = 0;
	spy_methodology = 0;
	random_event_frequency = OPTION_LOW;

	change_difficulty(OPTION_EASIEST+1);	// OPTION_VERY_EASY

	//-------- goal --------//

	goal_destroy_monster = 0;
	goal_population_flag = 0;
	goal_economic_score_flag = 0;
	goal_total_score_flag = 0;
	goal_year_limit_flag = 0;
	goal_alliance_win_flag = 0;

	// ####### begin Gilbert 3/3 ########//
	goal_population = 300;
	goal_economic_score = 1200;
	goal_total_score = 1800;
	goal_year_limit = 20;
	// ####### end Gilbert 3/3 ########//

	// game setting on fire
	fire_spread_rate = 0;          // 0 to disable, 10 for normal
	wind_spread_fire_rate = 5;     // 0 to disable, 5 for normal
	fire_fade_rate = 2;            // 1 for slow, 2 for fast
	fire_restore_prob = 80;        // 0 to 100, 5 for normal (with spreading)
	rain_reduce_fire_rate = 5;     // 0 to 20, 5 for normal
	fire_damage = 2;               // 0 to disable 2 for normal

	// ------- campaign setting ------//

	campaign_difficulty = 1;
	building_size = 2;
}
//--------- End of function Config::default_game_setting ---------//


// -------- Begin of function Config::clear_goals ---------//
//
void Config::clear_goals()
{
	goal_destroy_monster = 0;
	goal_population_flag = 0;
	goal_economic_score_flag = 0;
	goal_total_score_flag = 0;
	goal_year_limit_flag = 0;
	goal_alliance_win_flag = 0;
}
// -------- End of function Config::clear_goals ---------//


//--------- Begin of function Config::default_cheat_setting ---------//

void Config::default_cheat_setting()
{
	show_ai_info = sys.debug_session;
	show_debug_info = sys.debug_session;
	fast_build = sys.debug_session;
	disable_ai_flag = 0;
	king_undie_flag = sys.testing_session;
}
//--------- End of function Config::default_cheat_setting ---------//


//----- Begin of function Config::default_campaign_setting -----//
//
void Config::default_campaign_setting()
{
	default_game_setting();

	ai_aggressiveness = OPTION_MODERATE;
	new_independent_town_emerge = 0;
	random_event_frequency = OPTION_LOW;
	spy_methodology = 0;
}
//------ End of function Config::default_campaign_setting -----//


//------ Begin of function Config::default_local_game_setting ------//

void Config::default_local_game_setting()
{
	race_id = 1;
#if(defined(ITALIAN))
	strcpy(player_name, "Nuovo");
#elif(defined(SPANISH))
	strcpy(player_name, "Nuevo Jugador");
#elif(defined(FRENCH))
	strcpy(player_name, "Nouveau Joueur");
#elif(defined(GERMAN))
	strcpy(player_name, "Neuer Spieler" );
#else
	strcpy(player_name, "New Player");
#endif
	// do not call text_game_menu because text_game_menu may be deinited
//	strcpy(player_name, text_game_menu.str_default_player_name() );

	player_nation_color = 1;
	expired_flag = 0;
}
//--------- End of function Config::default_local_game_setting ---------//


//--------- Begin of function Config::default_preference ---------//

void Config::default_preference()
{
	opaque_report = 0;			// opaque report instead of transparent report
	disp_news_flag = OPTION_DISPLAY_ALL_NEWS;

	scroll_speed = 5;
	frame_speed	 = 12;
	scroll_method = 0;

	help_mode = DETAIL_HELP;
	disp_extend_info = 0;
	show_all_unit_icon = 1;		// 0:show icon when pointed, 1:always
	// ###### begin Gilbert 10/5 ###########//
	show_unit_path = 0;			// bit 0 show unit path on ZoomMatrix, bit 1 for MapMatrix
	// ###### end Gilbert 10/5 ###########//

	// music setting
	music_flag = 1;
	cd_music_volume = 100;
	wav_music_volume = 100;

	// sound effect setting
	sound_effect_flag = 1;
	sound_effect_volume = 100;
	pan_control = 1;

	// weather visual effect flags
	lightning_visual = 1;
	earthquake_visual = 1;
	rain_visual = 1;
	snow_visual = 1;
	snow_ground = 0;			// 0=disable, 1=i_snow, 2=snow_res

	// weather audio effect flags
	lightning_audio = 1;
	earthquake_audio = 1;
	rain_audio = 1;
	snow_audio = 0;				// not used
	wind_audio = 1;

	// weather visual effect parameters
	lightning_brightness = 20;
	cloud_darkness = 5;

	// weather audio effect parameters
	lightning_volume = 100;
	earthquake_volume = 100;
	rain_volume = 90;
	snow_volume = 100;
	wind_volume = 70;

	// other
	blacken_map  = 1;
	explore_mask_method = 2;
	fog_mask_method = 2;

	// display mode
	display_mode_id = MODE_ID_DEFAULT;			// see OMODEID.H

	enable_weather_visual();
	enable_weather_audio();
	cloud_darkness = 0;


}
//--------- End of function Config::default_preference ---------//


//--------- Begin of function Config::change_game_setting --------//
// for synchronize the game setting with the host before a multiplayer game
void Config::change_game_setting( Config &c )
{
	//-------- game settings  ---------//
	ai_human_nation_count  = c.ai_human_nation_count;
	ai_monster_nation_count  = c.ai_monster_nation_count;
	start_up_cash          = c.start_up_cash;
	// start_up_food          = c.start_up_food;
	ai_start_up_cash       = c.ai_start_up_cash;
	// ai_start_up_food       = c.ai_start_up_food;
	ai_aggressiveness      = c.ai_aggressiveness;
	start_up_independent_town = c.start_up_independent_town;
	start_up_raw_site      = c.start_up_raw_site;
	difficulty_level       = c.difficulty_level;

	explore_whole_map      = c.explore_whole_map;
	fog_of_war             = c.fog_of_war;

	terrain_set            = c.terrain_set;
	building_size          = c.building_size;
	latitude               = c.latitude;
	weather_effect         = c.weather_effect;
	land_mass              = c.land_mass;

	new_independent_town_emerge = c.new_independent_town_emerge;
	independent_town_resistance = c.independent_town_resistance;
	random_event_frequency = c.random_event_frequency;
	monster_type           = c.monster_type;
	new_nation_emerge      = c.new_nation_emerge;
	start_up_has_mine_nearby = c.start_up_has_mine_nearby;
	random_start_up 		  = c.random_start_up;
	spy_methodology        = c.spy_methodology;

	// --------- goal ---------//

	goal_destroy_monster      = c.goal_destroy_monster;
	goal_population_flag      = c.goal_population_flag;
	goal_economic_score_flag  = c.goal_economic_score_flag;
	goal_total_score_flag  	  = c.goal_total_score_flag;
	goal_year_limit_flag   	  = c.goal_year_limit_flag;
	goal_alliance_win_flag    = c.goal_alliance_win_flag;
	goal_population        	  = c.goal_population;
	goal_economic_score       = c.goal_economic_score;
	goal_total_score       	  = c.goal_total_score;
	goal_year_limit        	  = c.goal_year_limit;

	// ------- game setting on fire ---------//

	fire_spread_rate       = c.fire_spread_rate;
	wind_spread_fire_rate  = c.wind_spread_fire_rate;
	fire_fade_rate         = c.fire_fade_rate;
	fire_restore_prob      = c.fire_restore_prob;
	rain_reduce_fire_rate  = c.rain_reduce_fire_rate;
	fire_damage            = c.fire_damage;

	// -------- campaign setting ------------//

	campaign_difficulty    = c.campaign_difficulty;
}
//--------- End of function Config::change_game_setting --------//


//--------- Begin of function Config::change_game_setting --------//
// for saving config after changing option
void Config::change_preference( Config &c )
{
	opaque_report         = c.opaque_report;
	disp_news_flag        = c.disp_news_flag;

	scroll_speed          = c.scroll_speed;
	frame_speed           = c.frame_speed;
	scroll_method			 = c.scroll_method;

	help_mode             = c.help_mode;
	disp_extend_info      = c.disp_extend_info;
	show_all_unit_icon    = c.show_all_unit_icon;
	show_unit_path        = c.show_unit_path;

	//------- sound effect --------//

	music_flag            = c.music_flag;
	cd_music_volume       = c.cd_music_volume;
	wav_music_volume      = c.wav_music_volume;
	sound_effect_flag     = c.sound_effect_flag;
	sound_effect_volume   = c.sound_effect_volume;
	pan_control           = c.pan_control;

	//------- weather visual effect flags -------//

	lightning_visual      = c.lightning_visual;
	earthquake_visual     = c.earthquake_visual;
	rain_visual           = c.rain_visual;
	snow_visual           = c.snow_visual;
	snow_ground           = c.snow_ground;

	//-------- weather audio effect flags -------//

	lightning_audio       = c.lightning_audio;
	earthquake_audio      = c.earthquake_audio;
	rain_audio            = c.rain_audio;
	snow_audio            = c.snow_audio;
	wind_audio            = c.wind_audio;

	//--------- weather visual effect parameters --------//

	lightning_brightness  = c.lightning_brightness;
	cloud_darkness        = c.cloud_darkness;

	//-------- weather audio effect parameters ----------//

	lightning_volume = c.lightning_volume;
	earthquake_volume = c.earthquake_volume;
	rain_volume = c.rain_volume;
	snow_volume = c.snow_volume;
	wind_volume = c.wind_volume;

	//------------ map prefernce -------------//

	blacken_map = c.blacken_map;
	explore_mask_method = c.explore_mask_method;
	fog_mask_method = c.fog_mask_method;
	
	// ---------- display mode -----------//

	display_mode_id = c.display_mode_id;
}


//--------- Begin of function Config::enable_weather_visual --------//
void Config::enable_weather_visual()
{
	lightning_visual = 1;
	earthquake_visual = 1;
	rain_visual = 1;
	snow_visual = 1;
	snow_ground = 0;			// 0=disable, 1=i_snow, 2=snow_res
	cloud_darkness = 1;
}
//--------- End of function Config::enable_weather_visual --------//


//--------- Begin of function Config::disable_weather_visual --------//
void Config::disable_weather_visual()
{
	lightning_visual = 0;
	earthquake_visual = 0;
	rain_visual = 0;
	snow_visual = 0;
	snow_ground = 0;			// 0=disable, 1=i_snow, 2=snow_res
	cloud_darkness = 0;
}
//--------- End of function Config::disable_weather_visual --------//


//--------- Begin of function Config::enable_weather_audio --------//
void Config::enable_weather_audio()
{
	lightning_audio = 1;
	earthquake_audio = 1;
	rain_audio = 1;
	snow_audio = 1;				// not used
	wind_audio = 1;
}
//--------- End of function Config::enable_weather_audio --------//


//--------- Begin of function Config::disable_weather_audio --------//
void Config::disable_weather_audio()
{
	lightning_audio = 0;
	earthquake_audio = 0;
	rain_audio = 0;
	snow_audio = 0;				// not used
	wind_audio = 0;
}
//--------- End of function Config::disable_weather_audio --------//


//--------- Begin of function Config::save -------------//
int Config::save(char *filename)
{
	File configFile;

	if( !configFile.file_create(filename) )
		return 0;

	int retFlag = write_file(&configFile);

	configFile.file_close();

	return retFlag;

}
//--------- End of function Config::save -------------//


//--------- Begin of function Config::load -------------//
//
// if load() fails, call init to re-initialize it
//
int Config::load(char *filename)
{
	File configFile;

	if( !m.is_file_exist(filename) || !configFile.file_open(filename) )
		return 0;

	int retFlag = 0;

	// check file size is the same
	if( configFile.file_size() == sizeof(Config) )
	{
		retFlag = read_file(&configFile);
	}

	configFile.file_close();

	return retFlag;
}
//--------- End of function Config::load -------------//


//--------- Begin of function Config::reset_game_setting ---------//
void Config::reset_cheat_setting()
{
	show_ai_info = 0;
	show_debug_info = 0;
	fast_build = 0;
	disable_ai_flag = 0;
	king_undie_flag = 0;
}
//--------- End of function Config::reset_game_setting ---------//


//------- Begin of function Config::single_player_difficulty --------//
//
int Config::single_player_difficulty(int raceId)
{
	if( raceId == 0 )
		raceId = race_id;
	// sometimes, race is not assigned in multiplayer, assume human

	int score = 10;
	score += (ai_human_nation_count+ai_monster_nation_count) * 6;
	if( !explore_whole_map)
		score += 7;
	if( fog_of_war )
		score += 7;

	if( raceId >= 0 )
		score += (7 - start_up_raw_site) * 5;
	else
		score += start_up_raw_site * 2;		// more site human are stronger, fryhtan is harder to play

//	if( start_up_cash <= SMALL_STARTUP_RESOURCE )
//		score += 16;
//	else if( start_up_cash < LARGE_STARTUP_RESOURCE )
//		score += 8;
//	else
//		score += 0;
	score += (4 - start_up_cash) * 6;

//	if( ai_start_up_cash <= SMALL_STARTUP_RESOURCE )
//		score += 0;
//	else if( ai_start_up_cash < LARGE_STARTUP_RESOURCE )
//		score += 8;
//	else
//		score += 16;
	score += (ai_start_up_cash - 1 ) * 6;

	score += (ai_aggressiveness -1) * 10;
	score += (independent_town_resistance -1) * 5;
	if( new_nation_emerge )
		score += 6;
	score += random_event_frequency * 2;
//	switch( monster_type )
//	{
//	case OPTION_MONSTER_NONE:
//		break;
//	case OPTION_MONSTER_SAVAGE:
//		score += 6;
//		break;
//	case OPTION_MONSTER_INTELLIGENT:
//		score += 16;
//		break;
//	default:
//		err_here();
//	}

	if( raceId >= 0 )
	{
		if( !start_up_has_mine_nearby )
			score += 6;
	}
//	else
//	{
//		if( start_up_has_mine_nearby )		// stronger human, weaker fryhtan
//			score += 2;
//	}

	if( raceId >= 0 )
	{
		if( !spy_methodology )
			score += 2;
	}
	else
	{
		// no effect on fryhtan
	}

	return score;
}
//------- End of function Config::single_player_difficulty --------//


//------- Begin of function Config::multi_player_difficulty --------//
int Config::multi_player_difficulty(int remotePlayers, int raceId)
{
	short totalOpp = ai_nation_count() + remotePlayers;
	if( totalOpp > MAX_NATION-1 )
		totalOpp = MAX_NATION-1;
	return single_player_difficulty(raceId) + (totalOpp - ai_nation_count()) * 6;
}
//------- End of function Config::multi_player_difficulty --------//


//------- Begin of function Config::multi_player_difficulty --------//
void Config::change_difficulty(int difficulty)
{
	err_when( difficulty < OPTION_EASIEST || difficulty > OPTION_HARDEST );
	
	difficulty_level = difficulty;

	ai_human_nation_count       = table_ai_human_nation_count[difficulty];
	ai_monster_nation_count       = table_ai_monster_nation_count[difficulty];
	start_up_cash               = table_start_up_cash[difficulty];
	// start_up_food               = table_start_up_food[difficulty];
	ai_start_up_cash            = table_ai_start_up_cash[difficulty];
	// ai_start_up_food            = table_ai_start_up_food[difficulty];
	ai_aggressiveness           = table_ai_aggressiveness[difficulty];
	start_up_independent_town   = table_start_up_independent_town[difficulty];
	start_up_raw_site           = table_start_up_raw_site[difficulty];
	explore_whole_map           = table_explore_whole_map[difficulty];
	fog_of_war                  = table_fog_of_war[difficulty];

	new_independent_town_emerge = table_new_independent_town_emerge[difficulty];
	independent_town_resistance = table_independent_town_resistance[difficulty];
	random_event_frequency      = table_random_event_frequency[difficulty];
	new_nation_emerge           = table_new_nation_emerge[difficulty];
	monster_type                = table_monster_type[difficulty];
	start_up_has_mine_nearby    = table_start_up_has_mine_nearby[difficulty];
	random_start_up    			 = table_random_start_up[difficulty];
	spy_methodology             = table_spy_methodology[difficulty];
}
//------- End of function Config::change_difficulty --------//



// validate the range of config
int Config::validate()
{
	//	human_difficulty_rating;	// not checked, as set at starting game
	// monster_difficulty_rating;	// not checked, as set at starting game

	int invalidPoint = 0;

	if( ai_human_nation_count < 0 || ai_monster_nation_count < 0
		|| ai_nation_count() >= MAX_NATION )
		invalidPoint = 0x100;
	if( start_up_cash < OPTION_LOW || start_up_cash > OPTION_VERY_HIGH )
		invalidPoint = 0x101;
	if( ai_start_up_cash < OPTION_LOW || ai_start_up_cash > OPTION_VERY_HIGH )
		invalidPoint = 0x102;
	if( ai_aggressiveness < OPTION_NONE || ai_aggressiveness > OPTION_VERY_HIGH )
		invalidPoint = 0x103;
	if( start_up_independent_town < 0 || start_up_independent_town > 30 )
		invalidPoint = 0x104;
	if( start_up_raw_site < 1 || start_up_raw_site > 7 )
		invalidPoint = 0x105;
	if( difficulty_level != OPTION_CUSTOM && 
		(difficulty_level < OPTION_EASIEST || difficulty_level > OPTION_HARDEST) )
		invalidPoint = 0x106;

	//-------- option settings ----------//

	// boolean value, correct
	explore_whole_map = explore_whole_map ? 1 : 0;
	fog_of_war = fog_of_war ? 1 : 0;
	if( terrain_set < 1 || fog_of_war > 2 )
		invalidPoint = 0x120;
	if( building_size < 1 || building_size > 2 )
		invalidPoint = 0x121;
	if( latitude < -180 || latitude > 180 )
		invalidPoint = 0x122;
	weather_effect = weather_effect ? 1 : 0;
	// land_mass;		// not used any more

	new_independent_town_emerge = new_independent_town_emerge ? 1 : 0;
	if( independent_town_resistance < OPTION_LOW || independent_town_resistance > OPTION_HIGH )
		invalidPoint = 0x123;
	if( random_event_frequency < OPTION_NONE || random_event_frequency > OPTION_HIGH )
		invalidPoint = 0x124;
	new_nation_emerge = new_nation_emerge ? 1 : 0;
	if( monster_type < OPTION_MONSTER_NONE || monster_type > OPTION_MONSTER_INTELLIGENT )
		invalidPoint = 0x125;
	start_up_has_mine_nearby = start_up_has_mine_nearby ? 1 : 0;
	random_start_up = random_start_up ? 1 : 0;
	spy_methodology = spy_methodology ? 1 : 0;

	//--------- goal ----------//

	// need not verified
//	char			goal_destroy_monster;
//	char			goal_population_flag;
//	char			goal_economic_score_flag;
//	char			goal_total_score_flag;
//	char			goal_year_limit_flag;
//	char			goal_alliance_win_flag;
//	int			goal_population;
//	int  			goal_economic_score;
//	int			goal_total_score;
//	int			goal_year_limit;

	//------- game setting on fire ---------//

	if( fire_spread_rate < 0 || fire_spread_rate > 20 )
		invalidPoint = 0x160;	// 0 to disable, 10 for normal
	if( wind_spread_fire_rate < 0 || wind_spread_fire_rate > 20 )
		invalidPoint = 0x161;			// 0 to disable, 5 for normal
	if( fire_fade_rate < 1 || fire_fade_rate > 10 )
		invalidPoint = 0x162;            // 1 for slow, 2 for fast
	if( fire_restore_prob < 0 )
		invalidPoint = 0x163;				// 0 to 100, 5 for normal
	if( rain_reduce_fire_rate < 0 || rain_reduce_fire_rate > 20 )
		invalidPoint = 0x164;     // 0 to 20, 5 for normal
	if( fire_damage < 0 || fire_damage > 100 )
		invalidPoint = 0x165;               // 0 to disable 2 for normal

	// ------ campaign game setting ------//

	if( campaign_difficulty < 1 || campaign_difficulty > 5 )
		invalidPoint = 0x180;

	//--------- CHEAT GAME SETTING --------//
	// need not verify
	// show_ai_info;
	// show_debug_info;
	// fast_build;							// fast building everything
	// disable_ai_flag;
	// king_undie_flag;					// for testing game only
	
	//--------- LOCAL GAME SETTING --------//

	if( race_id > MAX_RACE || race_id < -MAX_MONSTER_TYPE )	// allow 0
		invalidPoint = 0x200;
	if( !memchr(player_name, '\0', PLAYER_NAME_LEN+1))
		invalidPoint = 0x201;
	if( player_nation_color < 0 || player_nation_color > MAX_COLOR_SCHEME)	// allow 0
		invalidPoint = 0x202;
	// expired_flag;

	//--------- PREFERENCE --------//

	if( opaque_report < 0 || opaque_report > 1 )
		invalidPoint = 0x300;
	if( disp_news_flag != OPTION_DISPLAY_MAJOR_NEWS && disp_news_flag != OPTION_DISPLAY_ALL_NEWS )
		invalidPoint = 0x301;

	if( scroll_speed < 1 || scroll_speed > 10 )
		invalidPoint = 0x302;				// map scrolling speed. 1-slowest, 10-fastest
	if( frame_speed < 0 )
		invalidPoint = 0x303;					// game speed, the desired frames per second
	if( scroll_method < 0 || scroll_method > 1 )
		invalidPoint = 0x304;
	if( help_mode < NO_HELP || help_mode > DETAIL_HELP )
		invalidPoint = 0x305;
	if( disp_extend_info < 0 || disp_extend_info > 2 )
		invalidPoint = 0x306;
	show_all_unit_icon = show_all_unit_icon ? 1 : 0;		// 0:show icon when pointed, 1:always
	// not checked
	// show_unit_path;				// bit 0 show unit path on ZoomMatrix, bit 1 for MapMatrix

	//------- sound effect --------//

	// music_flag;
	if( cd_music_volume < 0 || cd_music_volume > 100 )
		invalidPoint = 0x320;    	// a value from 0 to 100
	if( wav_music_volume < 0 || wav_music_volume > 100 )
		invalidPoint = 0x321;		// a value from 0 to 100
	// char			sound_effect_flag;
	if( sound_effect_volume < 0 || sound_effect_volume > 100 )
		invalidPoint = 0x322; 	// a value from 0 to 100
	// char			pan_control;                            // mono speaker should disable pan_control

	//------- weather visual effect flags -------//

	// lightning_visual;
	// earthquake_visual;
	// rain_visual;
	// snow_visual;
	// snow_ground;			// 0=disable, 1=i_snow, 2=snow_res

	//-------- weather audio effect flags -------//

	// lightning_audio;
	// earthquake_audio;
	// rain_audio;
	// snow_audio;				// not used
	// wind_audio;

	//--------- weather visual effect parameters --------//

	// lightning_brightness;	// 0, 20, 40 or 60
	// cloud_darkness;		// 0 to 5, 0 to disable cloud darkness

	//-------- weather audio effect parameters ----------//

	if( lightning_volume < 0 )
		invalidPoint = 0x380;		// default 100
	if( earthquake_volume < 0 )
		invalidPoint = 0x381;	// default 100
	if( rain_volume < 0 )
		invalidPoint = 0x382;			// default 90
	if( snow_volume < 0 )
		invalidPoint = 0x383;			// default 100
	if( wind_volume < 0 )
		invalidPoint = 0x384;			// default 70

	//------------ map prefernce -------------//

	// blacken_map;				// whether the map is blackened at the first place
	if( explore_mask_method < 0 || explore_mask_method > 2 )
		invalidPoint = 0x3a0;					// 0 for none, 1 for masking, 2 for remapping
	if( fog_mask_method < 0 || fog_mask_method > 2 )
		invalidPoint = 0x3a1;					// 1 for fast masking, 2 for slow remapping

	// ----------- display mode -----------//

	if( display_mode_id != MODE_ID_800x600x16 && display_mode_id != MODE_ID_1024x768x16 )
		invalidPoint = 0x3c0;

	if( invalidPoint != 0 )
		return 0;			// set break point here
	else
		return 1;
}

