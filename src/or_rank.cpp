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

//Filename    : OR_RANK.CPP
//Description : Ranking report

#include <oinfo.h>
#include <odate.h>
#include <ofont.h>
#include <ogame.h>
#include <oconfig.h>
#include <ocampgn.h>
#include <ovbrowif.h>
#include <ovbrowse.h>
#include <onation.h>
#include <odate.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h> 
#include <ot_reps.h>

//------------- Define coordinations -----------//

//enum { NATION_BROWSE_X1 = ZOOM_X1+6,
//		 NATION_BROWSE_Y1 = ZOOM_Y1+6,
//		 NATION_BROWSE_X2 = ZOOM_X2-6,
//		 NATION_BROWSE_Y2 = NATION_BROWSE_Y1+220,
//	 };
#define NATION_BROWSE_X1 (ZOOM_X1+6)
#define NATION_BROWSE_Y1 (ZOOM_Y1+6)
#define NATION_BROWSE_X2 (ZOOM_X2-6)
#define NATION_BROWSE_Y2 (NATION_BROWSE_Y1+200)

//enum { NATION_SCORE_X1 = ZOOM_X1+6,
//		 NATION_SCORE_Y1 = NATION_BROWSE_Y2+6,
//		 NATION_SCORE_X2 = ZOOM_X2-6,
//		 NATION_SCORE_Y2 = NATION_SCORE_Y1+150,
//	 };
#define NATION_SCORE_X1 (ZOOM_X1+6)
#define NATION_SCORE_Y1 (NATION_BROWSE_Y2+6)
#define NATION_SCORE_X2 (ZOOM_X2-6)
#define NATION_SCORE_Y2 (NATION_SCORE_Y1+150)

//enum { NATION_GOAL_X1 = ZOOM_X1+6,
//		 NATION_GOAL_Y1 = NATION_SCORE_Y2+6,
//		 NATION_GOAL_X2 = ZOOM_X2-6,
//		 NATION_GOAL_Y2 = NATION_GOAL_Y1+110,
//	 };
#define NATION_GOAL_X1 (ZOOM_X1+6)
#define NATION_GOAL_Y1 (NATION_SCORE_Y2+6)
#define NATION_GOAL_X2 (ZOOM_X2-6)
#undef	NATION_GOAL_Y2
#define NATION_GOAL_Y2 (NATION_GOAL_Y1 +140) //(ZOOM_Y2-35)

//enum { PLAY_TIME_X1 = ZOOM_X1+6,
//		 PLAY_TIME_X2 = ZOOM_X2-6,
//	 };
#define PLAY_TIME_X1 (ZOOM_X1+6)
#define PLAY_TIME_X2 (ZOOM_X2-6)

//----------- Define static variables ----------//

static VBrowseIF 	browse_nation;
static VBrowseIF 	browse_goal;
static int		 	nation_rank_data_array[MAX_RANK_TYPE][MAX_NATION];
static int			browse_goal_recno = 1;

//----------- Define static functions ----------//

static void  put_nation_rec(int recNo, int x, int y, int refreshFlag);
static void  put_goal_rec(int recNo, int x, int y, int refreshFlag);
// static int   nation_filter_old(int recNo=0);
static void  disp_score();
static int	 disp_goal(int refreshFlag);
static void	 disp_play_time(int y1);

//--------- Begin of function Info::disp_rank ---------//
//
void Info::disp_rank(int refreshFlag)
{
	set_rank_data(1);		// 1-only set those nations that have contact with us

	int x=NATION_BROWSE_X1+9;
	int y=NATION_BROWSE_Y1+3;

	vga_back.d3_panel_up(NATION_BROWSE_X1, NATION_BROWSE_Y1, NATION_BROWSE_X2, NATION_BROWSE_Y1+32 );

#if(defined(CHINESE))
	font_bld.put( x	 , y+7, text_reports.str_nat_kingdom() ); // "Kingdom" );
	font_bld.put( x+180 +16, y+7, text_reports.str_population() ); // "Population" );
	font_bld.put( x+264 -8, y+7, text_reports.str_military() ); // "Military" );
	font_bld.put( x+332+6, y+7, text_reports.str_economy() ); //"Economy" );
	font_bld.put( x+406 +14, y+7 , text_reports.str_reputation() ); //"Reputation" );
	font_bld.put( x+484 -2, y+7, text_reports.str_overall() ); //"Overall" );
#else
	font_bld.put( x	 , y+7, text_reports.str_nat_kingdom() ); // "Kingdom" );
	font_bld.put( x+180, y+7, text_reports.str_population() ); // "Population" );
	font_bld.put( x+264, y+7, text_reports.str_military() ); // "Military" );
#if(defined(GERMAN))
	#define X_SHIFT -10
#else
	#define X_SHIFT 0
#endif
	font_bld.put( x+332+X_SHIFT, y+7, text_reports.str_economy() ); //"Economy" );
#undef X_SHIFT
	font_bld.put( x+406, y+7, text_reports.str_reputation() ); //"Reputation" );
	font_bld.put( x+484, y+7, text_reports.str_overall() ); //"Overall" );
#endif

	if( refreshFlag == INFO_REPAINT )
	{
		// ###### begin Gilbert 22/6 #######//
		// try to view 7 nation without scrolling
//		browse_nation.init( NATION_BROWSE_X1, NATION_BROWSE_Y1+34, NATION_BROWSE_X2, NATION_BROWSE_Y2,
//								  0, 22, info.nation_filter(), put_nation_rec, 1 );
		browse_nation.init( NATION_BROWSE_X1, NATION_BROWSE_Y1+34, NATION_BROWSE_X2, NATION_BROWSE_Y2,
								  0, 19, info.nation_filter(), put_nation_rec, 1 );
		// ###### end Gilbert 22/6 #######//

		browse_nation.open(browse_nation_recno);
	}
	else
	{
		browse_nation.paint();
		browse_nation.open(browse_nation_recno, info.nation_filter());
	}

	//----- display score -------//
	if (info.nation_filter() > 0)
		disp_score();

	//------ display goal -------//

	if( !game.game_has_ended )	// if the ending screen has already appeared once, don't display the goal
	{
		y = disp_goal(refreshFlag) + 6;
	//	y = NATION_GOAL_Y2+6;
	}
	else
	{
		y = NATION_GOAL_Y1;
	}

	//----- display total playing time -----//

	disp_play_time(y);
}
//----------- End of function Info::disp_rank -----------//


//--------- Begin of static function disp_score ---------//
//
static void disp_score()
{
	int x=NATION_SCORE_X1+6, y=NATION_SCORE_Y1+6;

//	static char* rankStrArray[] =
//	{ "Population Score", "Military Score", "Economic Score",
//	  "Reputation Score", "Fryhtan Battling Score" };

	vga_util.d3_panel_down( NATION_SCORE_X1, NATION_SCORE_Y1, NATION_SCORE_X2, NATION_SCORE_Y2 );

	//------ display individual scores ---------//

	int rankScore, totalScore=0;
	int viewNationRecno = info.nation_filter(browse_nation.recno());

	for( int i=0 ; i<MAX_RANK_TYPE ; i++, y+=16 )
	{
		rankScore   = info.get_rank_score(i+1, viewNationRecno);
		totalScore += rankScore;

		font_bld.put( x    , y, text_reports.str_score_type(i+1) ); // rankStrArray[i] );
		font_bld.put( x+300, y, rankScore, 1 );
	}

	vga_back.bar( x, y, x+340, y+1, V_BLACK );
	y+=4;

	//-------- display thte total score --------//

	font_bld.put( x    , y+2, text_reports.str_overall_score() ); // "Overall Score" );
	font_bld.put( x+300, y+2, totalScore, 1 );
	y+=20;

	vga_back.bar( x, y, x+340, y+1, V_BLACK );
	y+=4;

	//--- the following is only displayed in standalone game (and not in campaigns) ---//

	if( game.is_campaign_mode() )
	{
		//----- show campaign difficulty -----//

		String str;

		str = text_reports.str_campaign_difficulty();  // "Campaign Difficulty Level: ";
		str += game.campaign()->campaign_difficulty;

#ifdef DEBUG
	#if(defined(CHINESE) && defined(TRADITIONAL))
			str += " (¾Ô§Ð¡G";
	#elif(defined(CHINESE) && !defined(TRADITIONAL))
			str += " (Õ½ÒÛ£º";
	#else
			str += " (stage ";
	#endif
			str += game.campaign()->stage_id;

			if( game.campaign()->plot_id )
			{
	#if(defined(CHINESE) && defined(TRADITIONAL))
				str += " (³õ´º¡G";
	#elif(defined(CHINESE) && !defined(TRADITIONAL))
				str += " ³¡¾°£º ";
	#else
				str += " plot ";
	#endif

				str.add_char( game.campaign()->plot_category );
				str += game.campaign()->plot_id;
			}

			str += ")";
#endif

		font_bld.put( x, y+2, str );

		//------ show game starting date ---//

//		str  = "The game started on ";
//		str += date.date_str(info.game_start_date);
//		str += " and you have been playing for ";
//		str += info.year_passed;
//		if( info.year_passed > 1 )
//			str += " years.";
//		else
//			str += " year.";
		font_bld.put( x, y+20, text_reports.str_campaign_year_passed(
			date.date_str(info.game_start_date), info.year_passed) );
		return;
	}

	//-------- display the final score ---------//

	int	 difficultyRating = nation_array[viewNationRecno]->race_id >= 0 ?
			 config.human_difficulty_rating : config.monster_difficulty_rating;
	int	 finalScore = totalScore * difficultyRating / 100;
	String str;

	str  = text_reports.str_final_score(); // translate.process("Final Score");
	str += ":  ";
	str += totalScore;
	str += " X ";

	int x2 = font_bld.put( x, y+12, str ) + 5;

	str  = difficultyRating;
	// str += translate.process( "(Difficulty Rating)" );
	str += " (";
	str += text_reports.str_difficulty_rating(); 
	str += ")";

//	font_bld.center_put( x2, y+1, x2+156, y+15, str );
//	vga_back.bar( x2   , y+16, x2+156, y+17, V_BLACK );
//	font_bld.put( x2+65, y+19, 100 );
	int x3 = font_bld.put( x2, y+1, str );
	vga_back.bar( x2 , y+16, x3, y+17, V_BLACK );
	font_bld.center_put( x2, y+19, x3, y+19+font_bld.std_font_height, "100" );

	//------- if the player has cheated -------//

	if( nation_array[viewNationRecno]->cheat_enabled_flag )
	{
		str  = "X  0 ";
		// str += translate.process( "(Cheated)" );
		str += "(";
		str += text_reports.str_cheated();
		str += ")";
		str += "  ";

		finalScore = 0;
	}
	else
		str = "";

	str += "=  ";
	str += finalScore;

	// font_bld.put( x2+170, y+12, str);
	font_bld.put( x3+4, y+12, str);

	y+=36;
}
//----------- End of static function disp_score -----------//


//--------- Begin of static function disp_goal ---------//
//
static int disp_goal(int refreshFlag)
{
	//----- if the ending screen has already appeared once -----//

	if( game.game_has_ended )
		return NATION_GOAL_Y1;

	//------------------------------------//

	int x=NATION_GOAL_X1+6, y=NATION_GOAL_Y1+6;

	// ####### begin Gilbert 3/3 ########//
//	int goalCount = 1 + config.goal_destroy_monster +
//						 config.goal_population_flag +
//						 config.goal_economic_score_flag +
//						 config.goal_total_score_flag;
	// ####### end Gilbert 3/3 ########//

	//------ if the game is in campaign mode ------//

	if( game.is_campaign_mode() )
	{
		font_bld.text_width( game.campaign()->full_goal_text(), -1,
			NATION_GOAL_X2 - NATION_GOAL_X1 - 12);
		int textHeight = font_bld.text_height() + 6;
		int dispLines, totalLines;
/*
		font_bld.count_line( NATION_GOAL_X1+6, NATION_GOAL_Y1+6, NATION_GOAL_X2-12,
									NATION_GOAL_Y2 - 6, game.campaign()->full_goal_text(),
									2, dispLines, totalLines);
*/
		dispLines = 0; totalLines = 4;

		if (NATION_GOAL_Y1 + textHeight + 6 > NATION_GOAL_Y2)
		{
		//	vga_util.d3_panel_down( NATION_GOAL_X1, NATION_GOAL_Y1, NATION_GOAL_X2, NATION_GOAL_Y1 + textHeight +6 );
		//		font_bld.put_paragraph( NATION_GOAL_X1+6, NATION_GOAL_Y1+6,
		//	 	NATION_GOAL_X2-6, NATION_GOAL_Y2 - 6, game.campaign()->full_goal_text() );

			if( refreshFlag == INFO_REPAINT )
			{
				browse_goal.init( NATION_GOAL_X1, NATION_GOAL_Y1, NATION_GOAL_X2, NATION_GOAL_Y2,
					  0, 22, totalLines -dispLines +1, put_goal_rec, 1, 3, 0);
				browse_goal.open(browse_goal_recno);
			}
			else
			{
				browse_goal.paint();
				browse_goal.open(browse_goal_recno, totalLines -dispLines +1);
				font_bld.put_paragraph( NATION_GOAL_X1+6, NATION_GOAL_Y1+6,
					NATION_GOAL_X2-22, NATION_GOAL_Y2 - 6, game.campaign()->full_goal_text(),
					2, browse_goal_recno);
			}
			y = NATION_GOAL_Y2;
		}
		else
		{
			y = MIN( NATION_GOAL_Y1 + textHeight + font_bld.height() +6, NATION_GOAL_Y2 );

			vga_util.d3_panel_down( NATION_GOAL_X1, NATION_GOAL_Y1, NATION_GOAL_X2, y);		// one line space
			font_bld.put_paragraph( NATION_GOAL_X1+6, NATION_GOAL_Y1+6,
				NATION_GOAL_X2-6, NATION_GOAL_Y1 + textHeight, game.campaign()->full_goal_text() );
		}
	}
	else //-- if it is just a standalone game ----//
	{
		String str;

		// ###### begin Gilbert 26/6 ########//
		int lineSpacing = 16;
		int lineCount = 0;
		if( game.win_condition_count > 1 || game.win_deadline )
			lineCount++;		// GOAL: achieve one of the following in %n years
		lineCount += game.win_condition_count;
		int firstLoseCond = 0;
		// skip the default lose condition
		if( game.lose_condition_count > 0 && game.lose_condition[0].condition_id == E_DEFEAT_OWN_KINGDOM )
			firstLoseCond = 1;
		lineCount += game.lose_condition_count - firstLoseCond;

		// adjust line Spacing
		if( lineCount * lineSpacing > NATION_GOAL_Y2 - NATION_GOAL_Y1 - 3 )
		{
			y -= 2;
			lineSpacing = (NATION_GOAL_Y2 - NATION_GOAL_Y1 - 8) / lineCount;
			if( lineSpacing < 12 )
				lineSpacing = 12;
		}

		// adjust lineCount
		if( lineCount * lineSpacing > NATION_GOAL_Y2 - NATION_GOAL_Y1 - 3 )
		{
			lineCount = (NATION_GOAL_Y2 - NATION_GOAL_Y1 - 8) / lineSpacing;
		}

		// draw frame

		if( lineCount <= 0 )
			return y;

		vga_util.d3_panel_down( NATION_GOAL_X1, NATION_GOAL_Y1, NATION_GOAL_X2,
			y + (lineCount-1) * lineSpacing + 12 + 8);

		if( game.win_condition_count > 1 || game.win_deadline )
		{
			// GOAL: achieve one of the following in %n years

			if( game.win_deadline )
			{
				if( game.win_condition_count == 1 )
					str = text_reports.str_goal_single_timed(date.date_str(game.win_deadline,1)); // "GOAL: Achieve in n years.
				else if( game.win_require_all_condition )
					str = text_reports.str_goal_and_timed(date.date_str(game.win_deadline,1)); // "GOAL: Achieve ALL of the Following in n years";
				else
					str = text_reports.str_goal_or_timed(date.date_str(game.win_deadline,1)); // "GOAL: Achieve One of the Following in n years";
			}
			else
			{
				if( game.win_require_all_condition )
					str = text_reports.str_goal_and(); // "GOAL: Achieve ALL of the Following";
				else
					str = text_reports.str_goal_or(); // "GOAL: Achieve One of the Following";
			}

			font_bld.put( x, y, str );
			y+=lineSpacing;
			lineCount--;		

			for( int i=0 ; i<game.win_condition_count && lineCount > 0 ; ++i )
			{
				font_bld.put( x, y, game.win_condition[i].cond_str, 0, NATION_GOAL_X2-6 );
				y+=lineSpacing;
				--lineCount;
			}
		}
		else		// win condition takes only one line
		{
			str = text_reports.str_goal_(); //"GOAL: ";
			str += game.win_condition[0].cond_str;

			font_bld.put( x, y, str );
			y+=lineSpacing;
			--lineCount;
		}

		// draw a line to separate lose condition
		if( lineCount > 0 )
		{
			vga.active_buf->bar( x, y, NATION_GOAL_X2-6, y, V_BLACK );
		}

		for( int i=firstLoseCond ; i<game.lose_condition_count && lineCount > 0 ; ++i )
		{
			int x2 = x;
			if( i == firstLoseCond )
				x2 = font_bld.put( x2, y, text_reports.str_lose_() );
			font_bld.put( x2, y, game.lose_condition[i].cond_str, 0, NATION_GOAL_X2-6 );
			y+=lineSpacing;
			--lineCount;
		}

		// ###### end Gilbert 26/6 ########//

		y += 4;
	}
	return y;
}
//----------- End of static function disp_goal -----------//


//--------- Begin of static function disp_play_time ---------//
//
static void disp_play_time(int y1)
{
	if( y1+24 > ZOOM_Y2 )		// no space to display
		return;

	vga_util.d3_panel_down( PLAY_TIME_X1, y1, PLAY_TIME_X2, y1+24 );

	String str;

	str  = text_reports.str_total_play_time(); // "Total Playing Time";
//	str  = translate.process(str);
	str += ": ";
	str += info.play_time_str();

	font_bld.put( PLAY_TIME_X1+6, y1+6, str );
}
//----------- End of static function disp_play_time -----------//


//--------- Begin of function Info::detect_rank ---------//
//
void Info::detect_rank()
{
	//------- detect nation browser ------//

	if( browse_nation.detect() )
	{
		browse_nation_recno = browse_nation.recno();
		return;
	}

	if( browse_goal.detect() )
	{
		browse_goal_recno = browse_goal.recno();
		return;
	}
}
//----------- End of function Info::detect_rank -----------//


//-------- Begin of static function nation_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of nations of this nation
//
// 2. when <int> recNo is given :
//    - return the nation recno in nation_array of the given recno.
//
static int nation_filter_old(int recNo)
{
	int    	i, nationCount=0;
	Nation*  viewingNation = NULL;
	
	if( nation_array.player_recno )
		viewingNation = nation_array[info.viewing_nation_recno];

	for( i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) )
			continue;

		if( i==info.viewing_nation_recno ||
			 !viewingNation ||
			 viewingNation->get_relation(i)->has_contact )
		{
			nationCount++;
		}

		if( recNo && nationCount==recNo )
			return i;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return nationCount;
}
//----------- End of static function nation_filter -----------//


//-------- Begin of static function put_nation_rec --------//
//
static void put_nation_rec(int recNo, int x, int y, int refreshFlag)
{
	int	  nationRecno = info.nation_filter(recNo);
	Nation* nationPtr   = nation_array[nationRecno];

	x+=3;
	// ####### begin Gilbert 22/6 ########//
//	y+=5;
	y+=2;
	// ####### end Gilbert 22/6 ########//

	nationPtr->disp_nation_color(x, y+4);

	font_bld.put( x+20, y, nationPtr->nation_name(), 0, 205 );

	font_bld.put( x+210, y, info.get_rank_pos_str(1, nationRecno) );
	font_bld.put( x+270, y, info.get_rank_pos_str(2, nationRecno) );
	font_bld.put( x+352, y, info.get_rank_pos_str(3, nationRecno) );

	if( nationPtr->is_human() )			// only display reputation for humans
		font_bld.put( x+435, y, info.get_rank_pos_str(4, nationRecno) );

	font_bld.put( x+495, y, info.get_rank_pos_str(0, nationRecno) );		// 0-overall ranking
}
//----------- End of static function put_nation_rec -----------//

//-------- Begin of static function put_goal_rec --------//
//
static void put_goal_rec(int recNo, int x, int y, int refreshFlag)
{
/*	font_bld.put_paragraph( NATION_GOAL_X1+6, NATION_GOAL_Y1+6,
		 	NATION_GOAL_X2-12, NATION_GOAL_Y2 - 6, game.campaign()->full_goal_text(),
			2, 1);
/*	int	  nationRecno = info.nation_filter(recNo);
	Nation* nationPtr   = nation_array[nationRecno];

	x+=3;
	y+=5;

	nationPtr->disp_nation_color(x, y+4);

	font_bld.put( x+20, y, nationPtr->nation_name(), 0, 205 );

	font_bld.put( x+210, y, info.get_rank_pos_str(1, nationRecno) );
	font_bld.put( x+270, y, info.get_rank_pos_str(2, nationRecno) );
	font_bld.put( x+352, y, info.get_rank_pos_str(3, nationRecno) );

	if( nationPtr->is_human() )			// only display reputation for humans
		font_bld.put( x+435, y, info.get_rank_pos_str(4, nationRecno) );

	font_bld.put( x+500, y, info.get_rank_pos_str(0, nationRecno) );		// 0-overall ranking*/
}
//----------- End of static function put_goal_rec -----------//


//-------- Begin of function Info::set_rank_data --------//
//
// <int> onlyHasContact - if this is 1, then only nations
//								  that have contact with the viewing
//								  nation is counted. Otherwise all nations
//								  are counted.
//
void Info::set_rank_data(int onlyHasContact)
{
	Nation* viewingNation = NULL; 
	Nation* nationPtr;
	int 	  rankPos=0;

	if( nation_array.player_recno && !nation_array.is_deleted(info.viewing_nation_recno) )
		viewingNation = nation_array[info.viewing_nation_recno];

	memset( nation_rank_data_array, 0, sizeof(nation_rank_data_array) );

	for( int i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) )
			continue;

		if( onlyHasContact )
		{
			if( viewingNation && !viewingNation->get_relation(i)->has_contact )
				continue;
		}

		nationPtr = nation_array[i];

		nation_rank_data_array[0][i-1] = nationPtr->population_rating;

		nation_rank_data_array[1][i-1] = nationPtr->military_rating;

		nation_rank_data_array[2][i-1] = nationPtr->economic_rating;

		nation_rank_data_array[3][i-1] = (int) nationPtr->reputation;

		nation_rank_data_array[4][i-1] = (int) nationPtr->kill_monster_score;
	}
}
//----------- End of static function Info::set_rank_data -----------//


//-------- Begin of function Info::get_rank_pos_str --------//
//
// <int> rankType - 0  for overall ranking
//						  >0 a specific ranking
//
// <int> nationRecno - get the ranking of this nation
//
char* Info::get_rank_pos_str(int rankType, int nationRecno)
{
	Nation* viewingNation = NULL;
	int curNationRankData, rankData;
	int rankPos=1;

	if( rankType==0 )	// overall rating
		curNationRankData = get_total_score(nationRecno);
	else
		curNationRankData = nation_rank_data_array[rankType-1][nationRecno-1];

	if( nation_array.player_recno && !nation_array.is_deleted(info.viewing_nation_recno) )
		viewingNation = nation_array[info.viewing_nation_recno];

	for( int i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) || i == nationRecno )
			continue;

		if( viewingNation && !viewingNation->get_relation(i)->has_contact )
			continue;

		if( rankType==0 )
			rankData = get_total_score(i);
		else
			rankData = nation_rank_data_array[rankType-1][i-1];

		if( rankData > curNationRankData )		// if another nation's value is higher than the given nation's value
			rankPos++;
	}

	return m.num_th(rankPos);
}
//----------- End of function Info::get_rank_pos_str -----------//


//-------- Begin of function Info::get_rank_score --------//
//
// Get the score of the given nation in the given ranking type.
//
int Info::get_rank_score(int rankType, int nationRecno)
{
	int maxValue;

	switch( rankType )
	{
		case 1:     	// population
			maxValue = 100;
			break;

		case 2:        // military strength
			maxValue = 200;
			break;

		case 3:        // economic strength
			maxValue = 6000;
			break;

		case 4:        // reputation
			maxValue = 100;				// so the maximum score of the reputation portion is 50 only
			break;

		case 5: 			// monsters slain score
			maxValue = 1000;
			break;
	}

	int rankScore = 100 * nation_rank_data_array[rankType-1][nationRecno-1] / maxValue;

	return MAX(0, rankScore);
}
//----------- End of function Info::get_rank_score -----------//


//-------- Begin of function Info::get_total_score --------//
//
// Get the score of the given nation.
//
int Info::get_total_score(int nationRecno)
{
	int totalScore=0;

	for( int i=0 ; i<MAX_RANK_TYPE ; i++ )
	{
		totalScore += get_rank_score(i+1, nationRecno);
	}

	return totalScore;
}
//----------- End of function Info::get_total_score -----------//
