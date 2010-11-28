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

//Filename    : OGAMEND.CPP
//Description : Game ending screen

#include <ogame.h>
#include <ovga.h>
#include <omodeid.h>
#include <ovgalock.h>
#include <odate.h>
#include <oremote.h>
#include <obox.h>
#include <oconfig.h>
#include <oserial.h>
#include <ocampgn.h>
#include <oprofile.h>
#include <ostr.h>
#include <osys.h>
#include <ofont.h>
#include <omouse.h>
#include <oimgres.h>
#include <oraceres.h>
#include <ogfile.h>
#include <onation.h>
#include <omousecr.h>
#include <omusic.h>
#include <ooptmenu.h>
#include <oingmenu.h>
#include <opower.h>
#include <omonsres.h>
#include <olinear.h>
#include <ot_endc.h>
#include <ot_gmenu.h>
#include <ot_unit.h>
#include <ot_reps.h>


//-------- Declare static vars & functions ---------//

static int  disp_score(int winFlag);
static void disp_win_str(int winNationRecno);
static void disp_losing_str(int surrenderToNationRecno);
static void disp_retire_str();
static void disp_ranking();
static void disp_stat(int);

static void put_stat(int y, char* desStr, char* dispStr);
static void put_stat(int y, char* desStr, int dispValue);
static void put_ranking(int y, int nationRecno);


//---------- Begin of function Game::game_end --------//
//
// <int> winNationRecno - the recno of the nation that won the game.
//								  0 - if you are just defeated or surrender
//										to another kingdom, and no other kingdom
//									   has won yet.
// [int] playerDestroyed - whether the player's nation has been destroyed or not. 
//									(default: 0)
// [int] surrenderToNationRecno - the nation your surrender to.
// [int] retireFlag		 - 1 if the player retires
//
void Game::game_end(int winNationRecno, int playerDestroyed, int surrenderToNationRecno, int retireFlag)
{
	//--- if the player has already won/lost the game and is just staying/observing the game ---//

	if( game_has_ended && !retireFlag )		// don't repeat displaying the winning/losing screen
		return;

	//------ quit any menu mode ------//

	if( option_menu.is_active() )
	{
		option_menu.abort();
	}

	if( ::in_game_menu.is_active() )
	{
		::in_game_menu.abort();
	}

	//--- if in campaign mode, display custom winning or losing messages ---//

	if( game.is_campaign_mode() )
	{
		game.campaign()->disp_end_game_msg();
		
		if( game.result_win_condition_id >= 0 )
		{
		//	vga.flip();
			game.campaign()->disp_end_game_bonus(BONUS_MAX_YEAR, info.year_passed);
		}
	}

	// ####### begin Gilbert 15/7 #########//
	int realWin = nation_array.player_recno && winNationRecno == nation_array.player_recno;
	// ####### end Gilbert 15/7 #########//

	// generate a serial number, to guard against bonus of winning again using load game

	if( remote.is_enable() && remote.is_lobbied()
		&& serial_repository.remove_last(generated_serial) )
	{
		player_profile.reload();

		if( realWin )
			++player_profile.num_won_games;
		else
			++player_profile.num_lose_games;

		// distribute gem stones to winners

		int winners = 0;
		int losers = 0;
		char winnerArray[MAX_NATION];
		char loserArray[MAX_NATION];
		char isPlayerWin = 0;

		// count number of non-ai winners
		int i;
		for( i = 1; i <= MAX_NATION; ++i )
		{
			if( !nation_array.is_deleted(i) && !nation_array[i]->is_ai() 
				&& nation_array[i]->check_win(1) ) // 1-checking only
			{
				winnerArray[winners] = i;
				winners++;
				if( nation_array.player_recno == i )
					isPlayerWin = 1;
			}
			else	// otherwise loser, notice player dropped may become ai, new nation formed is also ai
			{
				loserArray[losers] = i;
				losers++;
			}
		}

		if( total_gem_stones > 0 )
		{
			if( winners == 1 )
			{
				if( winnerArray[0] == nation_array.player_recno )
				{
					// take all the gems
						player_profile.gem_stones += total_gem_stones;
				}
			}
			else if( winners > 1 )
			{
				// distribute gemstone in a non-random manner

				LinearCounter linear( 0, 0, winners, total_gem_stones );
				int lastY = 0;
				for( linear.inc(); !linear.is_end(); linear.inc() )
				{
					if( winnerArray[linear.x-1] == nation_array.player_recno )
						player_profile.gem_stones += linear.y - lastY;
					lastY = linear.y;
				}
			}
		}

		if( max_ranking > 0 )
		{
			long rankInc = 0;

			WORD maxLoserRank = 0;
			long sumLoserRank = 0;		// loser contribute 5% of their rank
			for( i = 0; i < losers; ++i )
			{
				int r = player_ranking[loserArray[i]-1];
				int p = r / 20;
				if( p == 0 && r > 0 )		// lose 1 rank until zero
					p = 1;
				sumLoserRank += p;
				if( r > maxLoserRank )
					maxLoserRank = r;
//				if( loserArray[i] == nation_array.player_recno )
//				{
//					rankInc = -p;
//				}
			}

			if( isPlayerWin )
			{
				if( sumLoserRank == 0 )		// no loser
					rankInc = 0;
				else if( player_profile.ranking >= maxLoserRank )
					rankInc = 1;
				else if( maxLoserRank - player_profile.ranking < 10 )
					rankInc = 2;
				else if( maxLoserRank - player_profile.ranking < 100 )
					rankInc = 3;
				else
					rankInc = 4;
			}

			// distribute to winners
//			LinearCounter linear( 0, 0, winners, sumLoserRank );
//			int lastY = 0;
//			for( linear.inc(); !linear.is_end(); linear.inc() )
//			{
//				if( winnerArray[linear.x-1] == nation_array.player_recno )
//					rankInc = linear.y - lastY;
//				lastY = linear.y;
//			}

			// player_rank[nation_array.player_recno-1] may not be as same as player_profile.ranking

			// avoid overflow
			err_when( sizeof(player_profile.ranking) != sizeof(WORD) );
			if( rankInc > 0 )
			{
				if( (long)player_profile.ranking > 0xffff - rankInc )
					player_profile.ranking = 0xffff;
				else
					player_profile.ranking += (WORD)rankInc;
			}
			else if( rankInc < 0 )
			{
				if( (long)player_profile.ranking < -rankInc )
					player_profile.ranking = 0;
				else
					player_profile.ranking += (WORD)rankInc;		// += 'ocz rankInc is negative.
			}
		}

		// update profile
		player_profile.save();
	}

	//------ set the quit siginal -------//

	if( game.is_campaign_mode() )
		sys.signal_exit_flag = 3;	// 3-quit a campaign game only, but still remain in the campaign
	else
		sys.signal_exit_flag = 2;	// 2-quit to main menu

	mouse_cursor.set_frame(0);
	mouse_cursor.set_icon(CURSOR_NORMAL);

	info.save_game_scr();

	// -------- change display mode ---------//

	sys.change_display_mode(MODE_ID_DEFAULT);

	int useBackBuf = vga.use_back_buf;

	vga.use_front();

	//------- display the winning/losing picture -------//

	int songId = 4;
	int winFlag = 0;

	if( !retireFlag && !game.is_campaign_mode() &&
			game.game_mode != GAME_TUTORIAL )		// don't display this when retire
	{
		char* fileName;

		// ####### begin Gilbert 5/7 ########//
		// if( winNationRecno && (winNationRecno == nation_array.player_recno) )
		if( realWin )
		// ####### end Gilbert 5/7 ########//
		{
			if ((~nation_array)->race_id > 0)
				fileName = race_res[(~nation_array)->race_id]->code;
			else
				fileName = monster_res[(~nation_array)->monster_id()]->firm_build_code;
			songId = 5;
			winFlag = 1;
		}
		else
		{
			if ((~nation_array)->race_id > 0)
				fileName = "LOSEGAME";
			else
				fileName = "LOSEGAM2";
		}
		vga.disp_image_file(fileName);		//BUGHERE

		music.play(songId, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0);
		mouse.wait_press(60);		// 60 seconds to time out
	}
	else
	{
		music.play(songId, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0);
	}

	//------- display the statistic -------//

	vga.disp_image_file("RESULTS");

	int headingShown=0;

	if( !game.is_campaign_mode() )
	{
		if( winNationRecno )
		{
			disp_win_str(winNationRecno);
         headingShown=1;
		}
		else if( retireFlag )
		{
			disp_retire_str();
			headingShown=1;
		}
		else
		{
			disp_losing_str(surrenderToNationRecno);
			headingShown=1;
		}
	}

	disp_stat( headingShown ? 160 : 130 );

	if( !(game.is_campaign_mode() && game.campaign()->auto_test_flag) )		// do not wait if auto_test_flag is on.
		mouse.wait_press(60);		// 60 seconds to time out
	
	//-------- display ranking and score ----------//

	int totalScore = 0;

	if( !game.is_campaign_mode() )
	{
		vga.disp_image_file("RESULTS");

		info.set_rank_data(0);		// count all nations, not only those that have contact with the player

		disp_ranking();

		totalScore = disp_score(winFlag);

		mouse.wait_press(60);		// 60 seconds to time out
	}

	//--- if the player has managed to get into the hall of fame ---//

	if( !game.is_campaign_mode() && !game_has_ended && game.game_mode != GAME_TUTORIAL )
	{
		if( !game_file_array.add_hall_of_fame(totalScore, (config.race_id < 0) ) )
			vga.finish_disp_image_file();		// if add_hall_of_fame() has displayed the bitmap, it should have called vga.finish_disp_image_file() already
	}
	else
	{
		vga.finish_disp_image_file();
	}

	//--------- set game_has_ended to 1 --------//

	music.stop();
	game_has_ended = 1;

	//----------- reset all goals -----------//

	config.goal_destroy_monster     = 0;
	config.goal_population_flag     = 0;
	config.goal_economic_score_flag = 0;
	config.goal_total_score_flag 	  = 0;
	config.goal_year_limit_flag     = 0;

	//--- otherwise, ask if the player wants to stay in the game ---//

	#ifndef DEMO		// cannot continue to stay in the game in the demo version

	if( !retireFlag && !game.is_campaign_mode() && !remote.is_enable() )		// can't stay in the game in a multiplayer game
	{
		vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );		// clear the screen

		char powerWinFlag = power.win_opened;

		power.win_opened = 1;

		if( box.ask( text_game_menu.str_stay_in_game(), 
			text_game_menu.str_no_yes(1),	text_game_menu.str_no_yes(0)) )	// "Yes", "No"
			sys.signal_exit_flag = 0;

		power.win_opened = powerWinFlag;
	}

	#endif

	//-------- restore display mode ---------//

	sys.change_display_mode(config.display_mode_id);

	//-------- if it quits now ----------//

	if( sys.signal_exit_flag )
	{
		info.free_game_scr();

		vga.use_back_buf = useBackBuf;
	}
	else
	{
		//---- otherwise restore the screen and continue to play ----//

		info.rest_game_scr();

		vga.use_back_buf = useBackBuf;

		//---- reveal the whole world for staying in the game after being destroyed ----//

		if( playerDestroyed && !retireFlag )
		{
			world.unveil(0, 0, MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1);
			world.visit(0, 0, MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1, 0, 0);

			config.blacken_map = 0;
			config.fog_of_war  = 0;
		}
	}
}
//----------- End of function Game::game_end ---------//


//----------- Begin of static function disp_win_str -----------//

static void disp_win_str(int winNationRecno)
{
	err_when( game.result_win_condition_id < 0 ||
				 game.result_win_condition_id > END_CONDITION_COUNT );

	String winStr;

	// ######## begin Gilbert 10/6 ###########//
//	if( winNationRecno == nation_array.player_recno )
//		winStr = translate.process("Your Kingdom");
//	else
//		winStr = nation_array[winNationRecno]->nation_name();
//	if( game.win_require_all_condition )
//		winStr += " has completed all tasks";
//	else
//		winStr += game.win_condition[game.result_win_condition_recno].result_str;

	char *winFormatStr;
	if( game.win_require_all_condition )
		winFormatStr = text_end_condition.str_all_condition_format_str();
	else
		winFormatStr = game.win_condition[game.result_win_condition_recno].result_str;
	if( winNationRecno == nation_array.player_recno )
		winStr = text_end_condition.str_you_win_str( (~nation_array)->race_id, winFormatStr );
	else
		winStr = text_end_condition.str_other_win_str( nation_array[winNationRecno]->nation_name(), winFormatStr );
	// ######## end Gilbert 10/6 ###########//

	font_scre.center_put(0, 0, VGA_WIDTH-1, 139, winStr );
}
//----------- End of static function disp_win_str -----------//


//----------- Begin of static function disp_losing_str -----------//

static void disp_losing_str(int surrenderToNationRecno)
{
	String str;

	if( surrenderToNationRecno )		// you surrender to another kingdom
	{
//		str  = translate.process( "You Surrendered to " );
//		str += nation_array[surrenderToNationRecno]->nation_name();
//		str += " ";
//		str += translate.process( "on ");
//		str += date.date_str(info.game_date);
//		str += ".";

		str  = text_end_condition.str_lose_surrender( 
			nation_array[surrenderToNationRecno]->nation_name(),
			date.date_str(info.game_date) );
	}

	// You failed to achieve the goal within the time limit

	else if( game.win_deadline && info.game_date >= game.win_deadline )
	{
		// str = "Your Kingdom has Failed to Achieve its Goal Within the Time Limit.";
		str = text_end_condition.str_lose_time_up();
	}
	else if ( game.result_lose_condition_id >= 0 )
	{
		err_when( game.result_lose_condition_id >= END_CONDITION_COUNT );
//		str  = "Your Kingdom";
//		str += game.lose_condition[game.result_lose_condition_recno].result_str;
		char *winFormatStr = game.lose_condition[game.result_lose_condition_recno].result_str;
		str = text_end_condition.str_you_lose_str( (~nation_array)->race_id, winFormatStr );
	}
	else		// you're defeated by another kingdom
	{
		// str = "Your Kingdom has Gone Down to Ignominious Defeat !";
		str = text_end_condition.str_you_lose_defeated( (~nation_array)->race_id );
	}

	font_scre.center_put(0, 0, VGA_WIDTH-1, 139, str );
}
//----------- End of static function disp_losing_str -----------//


//----------- Begin of static function disp_retire_str -----------//

static void disp_retire_str()
{
	font_scre.center_put(0, 0, VGA_WIDTH-1, 139, 
		text_end_condition.str_you_retire(date.date_str(info.game_date)) );
}

//----------- End of static function disp_retire_str -----------//


//-------- Begin of static function disp_stat --------//
//
static void disp_stat(int y)
{
	Nation* nationPtr = ~nation_array;

	char* nationTitle = text_unit.str_nation(nationPtr->race_id);

	put_stat( y		, text_end_condition.str_duration_of_kingdom(), info.game_duration_str() );
	put_stat( y+=20, text_end_condition.str_game_time(), info.play_time_str() );

	put_stat( y+=30, text_end_condition.str_final_population(), nationPtr->all_population() );
	put_stat( y+=20, text_end_condition.str_final_treasure(), m.format((int)nationPtr->cash,2) );

	put_stat( y+=30, text_end_condition.str_kill_soldiers(), nationPtr->enemy_soldier_killed );
	put_stat( y+=20, text_end_condition.str_lose_soldiers(nationTitle), nationPtr->own_soldier_killed );

	put_stat( y+=30, text_end_condition.str_kill_monsters(), nationPtr->enemy_frythan_killed );
	put_stat( y+=20, text_end_condition.str_lose_monsters(nationTitle), nationPtr->own_frythan_killed );
	
	put_stat( y+=30, text_end_condition.str_kill_weapons(), nationPtr->enemy_weapon_destroyed );
	put_stat( y+=20, text_end_condition.str_lose_weapons(nationTitle), nationPtr->own_weapon_destroyed );

//	put_stat( y+=30, "Enemy Ships Sunk"   , nationPtr->enemy_ship_destroyed );
//	put_stat( y+=20, "Kingdom's Ships Missing", nationPtr->own_ship_destroyed );

	put_stat( y+=30, text_end_condition.str_destroy_buildings(), nationPtr->enemy_firm_destroyed );
	put_stat( y+=20, text_end_condition.str_lose_buildings(nationTitle), nationPtr->own_firm_destroyed );

	put_stat( y+=30, text_end_condition.str_kill_civilians(), nationPtr->enemy_civilian_killed );
	put_stat( y+=20, text_end_condition.str_lose_civilians(nationTitle), nationPtr->own_civilian_killed );
}
//----------- End of static function disp_stat -----------//


//-------- Begin of static function put_stat --------//
//
static void put_stat(int y, char* desStr, char* dispStr)
{
	font_scre.put( 140, y, desStr );
	font_scre.put( 530, y, dispStr );
}
//----------- End of static function put_stat -----------//


//-------- Begin of static function put_stat --------//
//
static void put_stat(int y, char* desStr, int dispValue)
{
	font_scre.put( 140, y, desStr );
	font_scre.put( 530, y, m.format(dispValue) );
}
//----------- End of static function put_stat -----------//


//-------- Begin of static function disp_ranking --------//
//
static void disp_ranking()
{
	//--------- display descriptions ---------//

	int x=20, y=76;

#if(defined(CHINESE))
	font_scre.put( x+20 +100, y+7, text_unit.str_nation(0)); // "Kingdom" );
	font_scre.put( x+260 +30 +24, y+7, text_reports.str_population()); // "Population" );
	font_scre.put( x+370 +15 +16, y+7, text_reports.str_military()); //"Military" );
	
	font_scre.put( x+470-15 +18, y+7, text_reports.str_economy()); // "Economy" );
	font_scre.put(x+562-25 +24, y+7, text_reports.str_reputation()); // "Reputation" );

	font_scre.center_put_paragraph( x+670-32 -7, y +6, x+707, y+28,
		text_reports.str_fryhtan_battling(), 0, 0, 0 );
#else
	font_scre.put( x+20 +100, y+7, text_unit.str_nation(0)); // "Kingdom" );
	font_scre.put( x+260 +30, y+7, text_reports.str_population()); // "Population" );
	font_scre.put( x+370 +15, y+7, text_reports.str_military()); //"Military" );
	
	font_scre.put( x+470-15, y+7, text_reports.str_economy()); // "Economy" );
	font_scre.put(x+562-25, y+7, text_reports.str_reputation()); // "Reputation" );

//	font_scre.put( x+670-32, y   , "Fryhtan" );
//	font_scre.put( x+670-32, y+14, "Battling" );
	font_scre.center_put_paragraph( x+670-32, y, x+707, y+28,
		text_reports.str_fryhtan_battling(), 0, 0, 0 );
#endif

	//--------- display rankings -----------//

	put_ranking(y+=36, nation_array.player_recno);

	for( int i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) || i==nation_array.player_recno )
			continue;

		put_ranking( y+=30, i );
	}
}
//----------- End of static function disp_ranking -----------//


//-------- Begin of static function put_ranking --------//
//
static void put_ranking(int y, int nationRecno)
{
	Nation* nationPtr = nation_array[nationRecno];

	int x=20;

	nationPtr->disp_nation_color(x+73, y+5);

	font_scre.put( x+20 +73, y, nationPtr->nation_name(), 0, x+260+25 );

	int y2 = y+font_scre.height()-1;

	font_scre.center_put( x+260 +30, y, x+340+30, y2, info.get_rank_pos_str(1, nationRecno) );
	font_scre.center_put( x+370 +15, y, x+435+15, y2, info.get_rank_pos_str(2, nationRecno) );
	font_scre.center_put( x+470 -15, y, x+540-15, y2, info.get_rank_pos_str(3, nationRecno) );
	font_scre.center_put( x+562 -25, y, x+640-25, y2, info.get_rank_pos_str(4, nationRecno) );
	font_scre.center_put( x+670 -32, y, x+730-32, y2, info.get_rank_pos_str(5, nationRecno) );
}
//----------- End of static function put_ranking -----------//


//--------- Begin of static function disp_score ---------//
//
static int disp_score(int winFlag)
{
	int x=200, y=360;

	char* rankStrArray[] =
	{
		text_reports.str_population_score(), // "Population Score",
		text_reports.str_military_score(), // "Military Score",
		text_reports.str_economy_score(), // "Economic Score",
		text_reports.str_reputation_score(), // "Reputation Score",
		text_reports.str_fryhtan_battling_score(), // "Fryhtan Battling Score"
	};

	//------ display individual scores ---------//

	int rankScore, totalScore=0;
	int viewNationRecno = nation_array.player_recno;

	for( int i=0 ; i<MAX_RANK_TYPE ; i++, y+=22 )
	{
		rankScore   = info.get_rank_score(i+1, viewNationRecno);
		totalScore += rankScore;

		font_scre.put( x, y, rankStrArray[i] );
		font_scre.right_put( x+330, y, m.format(rankScore) );
	}

	vga_front.bar( x, y, x+340, y+1, V_BLACK );
//	y+=7;
	y+=2;

	//-------- display thte total score --------//

	font_scre.put( x    , y+2, text_reports.str_total_score() ); // "Total Score" );
	font_scre.right_put( x+330, y, m.format(totalScore) );
//	y+=28;
	y+=20;

	vga_front.bar( x, y, x+340, y+1, V_BLACK );
	y+=4;

	//-------- display the final score ---------//

	int	 difficultyRating = config.race_id >= 0 ? config.human_difficulty_rating : config.monster_difficulty_rating;
	int	 finalScore = totalScore * difficultyRating / 100;
	String str;

	str  = text_reports.str_final_score(); // translate.process("Final Score");
	str += ":  ";
	str += totalScore;
	str += " X ";

	x = font_scre.put( x, y+20, str )+5;

	str  = difficultyRating;
	str += " (";
	str += text_reports.str_difficulty_rating(); // translate.process( "(Difficulty Rating)" );
	str += ")";

	font_scre.center_put( x, y+4, x+170, y+1+font_scre.height(), str );
	vga_front.bar( x, y+27, x+170, y+28, V_BLACK );
	font_scre.put( x+70, y+30, 100 );

	//---- if this is a scenario and there are score bonus ----//

	str = "";

	if( info.goal_score_bonus && winFlag &&
		 !nation_array[viewNationRecno]->cheat_enabled_flag )		// if cheated, don't display bonus score, as there is no space for displaying both
	{
		str += "+  ";
		str += info.goal_score_bonus;
		str += " (";
		str += text_reports.str_scenario_bonus();
		str += ")  ";
		finalScore += info.goal_score_bonus;
	}

	//------- if the player has cheated -------//

	if( nation_array[viewNationRecno]->cheat_enabled_flag )
	{
		str  = "X  0 ";
		str += "(";
		str += text_reports.str_cheated();
		str += ")  ";
		finalScore = 0;
	}

	str += "=  ";
	str += finalScore;

	font_scre.put( x+180, y+18, str);

	return finalScore;
}
//----------- End of static function disp_score -----------//
