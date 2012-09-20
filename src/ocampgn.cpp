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

// Filename     : OCAMPGN.CPP
// Description  : Campaign class

#include <ocampgn.h>
#include <oinfo.h>
#include <osys.h>
#include <ostr.h>
#include <osite.h>
#include <onation.h>
#include <oraceres.h>
#include <oherores.h>
#include <ogame.h>
#include <oconfig.h>
#include <odate.h>
#include <omouse.h>
#include <omonsres.h>
#include <ostate.h>
#include <ovga.h>
#include <ogameset.h>
#include <ohelp.h>
#include <oworldmt.h>
#include <oplasma.h>
#include <ot_reps.h>

#include <stdlib.h>
#include <time.h>


//------- define campaign code --------//

static const char* campaign_code_array[] =
{
	"EAST",
	"TUTORIAL",
};

//----- Define static class member vars -----//

ResourceIdx Campaign::res_stage;
ResourceIdx Campaign::res_event;
ResourceIdx Campaign::res_plot;

//------- Begin of static function square -----//

inline static double square(double d)
{
	return d * d;
}

//--------- End of static function square -----//


//-------- Begin of function Campaign::Campaign ------//

Campaign::Campaign()
{
	memset( this, 0, sizeof(Campaign) );
}
//-------- End of function Campaign::Campaign ------//


//-------- Begin of function Campaign::~Campaign ------//

Campaign::~Campaign()
{
	deinit();

	if( text_buf )
	{
		mem_del( text_buf );
		text_buf = NULL;
	}

	if( text_buf2 )
	{
		mem_del( text_buf2 );
		text_buf2 = NULL;
	}

	if( shadow_array )
	{
		mem_del(shadow_array);
		shadow_array = NULL;
	}
}
//-------- End of function Campaign::~Campaign ------//


//-------- Begin of function Campaign::init ------//

void Campaign::init(int campaignId)
{
	memset( (char*)this + sizeof(void*), 0, sizeof(Campaign)-sizeof(void*) );

	shadow_array = (short*) mem_resize( shadow_array,
		MAIN_TERRAIN_WIDTH * MAIN_TERRAIN_HEIGHT * sizeof(short) );
	memset( shadow_array, 0, MAIN_TERRAIN_WIDTH * MAIN_TERRAIN_HEIGHT * sizeof(short) );

	//----- set campaign id. and code -------//

	campaign_id = campaignId;

	strcpy( campaign_code, campaign_code_array[campaignId-1] );

	//------- init campaign situation -------//

	init_situation();

	//----- open campaign dialog resource file -------//

	String str;

	str = DIR_CAMPAIGN;
	str += campaign_code;
	str += ".STG";

	res_stage.init(str, 1);  	// 1-read all into buffer

	//----- open campaign dialog resource file -------//

	str = DIR_CAMPAIGN;
	str += campaign_code;
	str += ".EVT";

	res_event.init(str, 1);  	// 1-read all into buffer

	//----- open campaign plot resource file -------//

	str = DIR_CAMPAIGN;
	str += campaign_code;
	str += ".PLT";

	res_plot.init(str, 1);  	// 1-read all into buffer

	init_flag = 1;

	//------ set campaign auto test flag --------//

	auto_test_flag = misc.is_file_exist("CTEST.SYS" );
}
//-------- End of function Campaign::init ------//


//-------- Begin of function Campaign::deinit ------//

void Campaign::deinit()
{
	if( !init_flag )
		return;

	for( int i = 1; i <= MAX_NATION; ++i )
	{
		if( !is_nation_deleted(i) )
			del_nation(i);
	}

	//----- deinit resource files -----//

	res_stage.deinit();
	res_event.deinit();
	res_plot.deinit();

	//------- deinit state_array ------//

	state_array.deinit();

	init_flag = 0;
}
//-------- End of function Campaign::deinit ------//


//------- Begin of function Campaign::init_situation ------//

void Campaign::init_situation()
{
	CampaignSituation* cs;

	//----- set the campaign vars to the standard value (50) ----//

	memset( campaign_situation_array, 50, sizeof(campaign_situation_array) );

	//------------------------------------------------//
	//
	// Situation 1: mines are running out of minerals.
	//
	//------------------------------------------------//

	cs = campaign_situation_array;

	cs->cash_level 	   = 100;
	cs->mine_raw_level   = 10;
	cs->site_raw_level   = 30;

	//------------------------------------------------//
	//
	// Situation 2: wealthy but low food and low population
	//
	//------------------------------------------------//

	cs = campaign_situation_array+1;

	cs->cash_level 	   = 80;
	cs->food_level       = 10;
	cs->population_level = 25;
	cs->economy_level    = 80;

	//------------------------------------------------//
	//
	// Situation 3: strong military but low cash and poor economy
	//
	//------------------------------------------------//

	cs = campaign_situation_array+2;

	cs->cash_level 	   = 20;
	cs->economy_level    = 20;
	cs->military_level   = 80;

	//------------------------------------------------//
	//
	// Situation 4: high loyalty, low population, low cash, poor economy
	//
	//------------------------------------------------//

	cs = campaign_situation_array+3;

	cs->cash_level 	   = 25;
	cs->population_level = 30;
	cs->economy_level    = 25;
	cs->town_loyalty_level = 80;
	cs->unit_loyalty_level = 80;

	//------------------------------------------------//
	//
	// Situation 5: high tech, good economy, poor military.
	//
	//------------------------------------------------//

	cs = campaign_situation_array+4;

	cs->tech_level       = 90;
	cs->economy_level    = 70;
	cs->military_level   = 15;

	//------------------------------------------------//
	//
	// Situation 6: high military, high loyalty, high tech
	//					 low cash, low food, low raw
	//
	//------------------------------------------------//

	cs = campaign_situation_array+5;

	cs->cash_level 	   = 25;
	cs->food_level       = 25;
	cs->mine_raw_level   = 25;
	cs->site_raw_level   = 25;
	cs->tech_level       = 75;
	cs->military_level   = 75;
}
//--------- End of function Campaign::init_situation ------//


//------- Begin of function Campaign::set_cur_situation ------//

void Campaign::set_cur_situation(int situationId)
{
	err_when( situationId < 0 || situationId > CAMPAIGN_SITUATION_COUNT );

	has_situation = situationId>0;

	if( situationId==0 )
		return;

	memcpy( &cur_situation, campaign_situation_array+situationId-1, sizeof(CampaignSituation) );

	//------- randomize cur situation vars ------//

	cur_situation.cash_level 	    += misc.random(11) - 5;
	cur_situation.food_level       += misc.random(11) - 5;
	cur_situation.mine_raw_level   += misc.random(11) - 5;
	cur_situation.site_raw_level   += misc.random(11) - 5;
	cur_situation.population_level += misc.random(11) - 5;
	cur_situation.reputation_level += misc.random(11) - 5;
	cur_situation.tech_level       += misc.random(11) - 5;
	cur_situation.economy_level    += misc.random(11) - 5;
	cur_situation.military_level   += misc.random(11) - 5;
	cur_situation.town_loyalty_level += misc.random(11) - 5;
	cur_situation.unit_loyalty_level += misc.random(11) - 5;
}
//--------- End of function Campaign::set_cur_situation ------//


//------- Begin of function Campaign::set_std_situation ------//

void Campaign::set_std_situation()
{
	has_situation = 1;

	memset( &cur_situation, 50, sizeof(cur_situation) );		// set every var to 50
}
//--------- End of function Campaign::set_std_situation ------//


//-------- Begin of function Campaign::init_new_campaign ------//
//
// This function is called when the player has set the settings
// of the campaign and press the Start button.
//
void Campaign::init_new_campaign()
{
	//----- init random seed -------//

	int randomSeed;
	randomSeed = time(NULL);
	randomSeed = (int) _rotr( randomSeed, 4 );

	if( randomSeed < 0 )
		randomSeed = ~randomSeed;

	if( randomSeed == 0 )
		randomSeed = 1;

	random_seed = randomSeed;

	//------- init date -------//

	game_year  = INFO_DEFAULT_YEAR;
	game_month = INFO_DEFAULT_MONTH;
	game_day   = INFO_DEFAULT_DAY;

	//------- set campaign_difficulty ------//

	set_campaign_difficulty(config.campaign_difficulty);

	// ######## begin Gilbert 9/4 ##########//
	//----- initialize random uniqueness -------//
	race_res.second_init();		// it is called here because we want to reset name_used_array[] everytime
	hero_res.second_init();		// the second_init() of hero is called here instead of in Battle::second_init because hero_res will reset all appeared_flag excepts royal unit heroes
	init_uniqueness();		// a Battle function, call functions like hero_res.init_for_hire()
	// ######## end Gilbert 9/4 ##########//

	//------- init nations ----------//

	int i;
	for( i = 0; i < MAX_NATION; ++i )
	{
		nation[i].init();
	}

	//----- initialize random uniqueness -------//

	// init_uniqueness();		// a Battle function, call functions like hero_res.init_for_hire()

	//--------------------------------//

	init_new_campaign_derived();

	//------ generate plasma terrain map --------//
	// size of this terrain 512 x 512, because the render_terrain() 
	// program only support this size //
	random_terrain_map.init(MAIN_TERRAIN_WIDTH, MAIN_TERRAIN_HEIGHT);
	random_terrain_map.generate( misc.random(2), 5, misc.rand() );
	int prevHeight = random_terrain_map.get_pix(0, 0);
	
	for(i = 0; i <MAIN_TERRAIN_WIDTH; i++)
	{
		for(int j = 0; j <MAIN_TERRAIN_HEIGHT; j++)
		{
			int height = random_terrain_map.get_pix(i, j);
			if (height > prevHeight)
				shadow_array[j * MAIN_TERRAIN_WIDTH + i] = 32;//16 * abs(height-prevHeight) / 8; 
         else
				shadow_array[j * MAIN_TERRAIN_WIDTH + i] = 32 - 16 * abs(height-prevHeight) / 8; 
			prevHeight = height;
	   }
	}

	//------ display introduction screen --------//

	disp_intro();
}
//-------- End of function Campaign::init_new_campaign ------//

//-------- Begin of function Campaign::random ------//

unsigned long Campaign::random(unsigned long maxNum)
{
	err_if( maxNum < 0 || maxNum > 0x7FFF )
		err_now( "Campaign::random()" );

	#define MULTIPLIER      0x015a4e35L
	#define INCREMENT       1
	#define RANDOM_MAX      0x7FFFU

	random_seed = MULTIPLIER * random_seed + INCREMENT;

	return maxNum * ((random_seed >> 16) & RANDOM_MAX) / (RANDOM_MAX+1);
}

//-------- End of function Campaign::random ------//


//-------- Begin of function Campaign::randomf ------//

double Campaign::randomf(double maxNum)
{
	random_seed = MULTIPLIER * random_seed + INCREMENT;

#define RANDOM_MAX_DOUBLE 0x7FFFFFFFU
	return maxNum * (random_seed & RANDOM_MAX_DOUBLE) / (RANDOM_MAX_DOUBLE + 1.0);
}

//-------- End of function Campaign::randomf ------//


//-------- Begin of function Campaign::rand ------//

unsigned long Campaign::rand()
{
	random_seed = MULTIPLIER * random_seed + INCREMENT;

	return random_seed;
}
//-------- End of function Campaign::rand ------//


//-------- Begin of function Campaign::is_nation_deleted ------//

int Campaign::is_nation_deleted(int nationRecno)
{
	err_when( nationRecno < 0 || nationRecno > MAX_NATION );

	return nation[nationRecno-1].campaign_nation_recno == 0;
}
//-------- End of function Campaign::is_nation_deleted ------//


//-------- Begin of function Campaign::new_nation ------//

int Campaign::new_nation()
{
	for(int n = 1; n <= MAX_NATION; ++n )
	{
		if( is_nation_deleted(n) )
		{
			nation[n-1].campaign_nation_recno = n;		// mark used
			return n;
		}
	}
	return 0;
}
//-------- End of function Campaign::new_nation ------//


//-------- Begin of function Campaign::get_nation ------//

CampaignNation *Campaign::get_nation(int nationRecno)
{
	if( nationRecno > 0 && nationRecno <= MAX_NATION )
	{
		err_when( is_nation_deleted(nationRecno) );
		return &nation[nationRecno-1];
	}
	else
	{
		err.run("Campaign::get_nation()");
		return NULL;
	}
}
//-------- End of function Campaign::get_nation ------//


//-------- Begin of function Campaign::del_nation ------//

void Campaign::del_nation(int nationRecno)
{
	if( nationRecno > 0 && nationRecno <= MAX_NATION )
	{
		nation[nationRecno-1].deinit();
		nation[nationRecno-1].campaign_nation_recno = 0;
	}
	else
	{
		err_here();
	}
}
//-------- End of function Campaign::del_nation ------//


//-------- Begin of function Campaign::add_nation ------//
//
// <int>   nationType    - the type of nation to be added
// [int]   raceId 	    - 0 if let the program picks one (default: 0)
// [int]   colorSchemeId - 0 if let the program picks one (default: 0)
//
// return: <int> the recno of the newly added CampaignNation
//					  0 if failure, it occurs if out of usable colors.
//
int Campaign::add_nation(int nationType, int raceId, int colorSchemeId)
{
	//---- if raceId is 0, pick one randomly ----//

	if( raceId==0 )
	{
		raceId = random_unused_race( nationType==CAMPAIGN_NATION_MONSTER );
	}

	//---- if colorSchemeId is 0, pick one randomly ----//

	if( colorSchemeId==0 )
	{
		colorSchemeId = random_unused_color();

		if( !colorSchemeId )
			return 0;
	}

	//------ add the nation now ------//

	int newNationRecno = new_nation();

	err_when( !newNationRecno );

	CampaignNation *nationPtr = get_nation(newNationRecno);

	nationPtr->init_nation( nationType, raceId, colorSchemeId);

	return newNationRecno;
}
//-------- End of function Campaign::add_nation ------//


//-------- Begin of function Campaign::random_unused_race ------//
//
// <bool> isMonster - true if we want to get monster id. instead of human race id.
//
int Campaign::random_unused_race(bool isMonster)
{
	//--- get a statistic of races currently used ---//

	char* raceUsed;

	if( isMonster )
	{
		raceUsed = mem_add( MAX_MONSTER_TYPE );
		memset(raceUsed, 0, MAX_MONSTER_TYPE );
	}
	else
	{
		raceUsed = mem_add( MAX_RACE );
		memset(raceUsed, 0, MAX_RACE );
	}

	char raceCount = 0;

	for( int i=nation_count() ; i>0 ; i-- )
	{
		if( is_nation_deleted(i) )
			continue;

		int thisRaceId = get_nation(i)->race_id;

		if( isMonster )
		{
			if( thisRaceId < 0 )
				raceUsed[ (-thisRaceId)-1 ]++;
		}
		else
		{
			if( thisRaceId > 0 )
				raceUsed[thisRaceId-1]++;
		}
	}

	//------ randomly pick one -------//

	int raceId;
	for( raceId = 1; raceId <= MAX_RACE; ++raceId )
	{
		if( raceUsed[raceId-1] == 0 )
			++raceCount;
	}

	if( raceCount > 0)
	{
		raceCount = (char)random(raceCount);
		// choose the raceCount-th raceId which raceUsed[raceId-1]==0
		for( raceId = 1; raceId <= MAX_RACE; ++raceId )
		{
			if( raceUsed[raceId-1] == 0 )
			{
				if( raceCount == 0 )
					break;

				--raceCount;
			}
		}
		err_when(raceId > MAX_RACE);
	}
	else
	{
		raceId = 1+(char)random(MAX_RACE);
	}

	mem_del( raceUsed );

	if( isMonster )
		return -raceId;
	else
		return raceId;
}
//-------- End of function Campaign::random_unused_race ------//


//------ Begin of function Campaign::random_unused_color ------//
//
// return: <int> 0 - if no available color scheme.
//
int Campaign::random_unused_color()
{
	//--- get a statistic of races currently used ---//

	char colorUsed[MAX_RACE];
	memset(colorUsed, 0, sizeof(colorUsed));

	for( int i=nation_count() ; i>0 ; i-- )
	{
		if( is_nation_deleted(i) )
			continue;

		int thisColorId = get_nation(i)->color_scheme_id;

		err_when( thisColorId < 1 || thisColorId > MAX_RACE );

		colorUsed[thisColorId-1]++;
	}

	//------ pick a color now --------//

	int colorCount = 0;

	int colorId;
	for( colorId = 1; colorId <= MAX_COLOR_SCHEME; ++colorId )
	{
		if( colorUsed[colorId-1] == 0 )
			++colorCount;
	}

	if( colorCount > 0 )
	{
		colorCount = random(colorCount);

		for( colorId = 1; colorId <= MAX_COLOR_SCHEME; ++colorId )
		{
			if( colorUsed[colorId-1] == 0 )
			{
				if( colorCount == 0 )
					break;

				--colorCount;
			}
		}

		err_when(colorCount > MAX_COLOR_SCHEME );

		return colorId;
	}
	else
		return 0;
}
//-------- End of function Campaign::random_unused_color ------//


//-------- Begin of function Campaign::packed_nation_count ------//

int Campaign::packed_nation_count()
{
	int nationCount = 0;

	for( int i=1 ; i<=MAX_NATION ; i++ )
	{
		if( !is_nation_deleted(i) )
			nationCount++;
	}

	return nationCount;
}
//-------- End of function Campaign::packed_nation_count ------//


//-------- Begin of function Campaign::state_change_nation ------//
//
void Campaign::state_change_nation(int stateRecno, int newNationRecno)
{
	int oldNationRecno = state_array[stateRecno]->campaign_nation_recno;

	state_array[stateRecno]->campaign_nation_recno = newNationRecno;

	//---- check if the nation still has any states left, if not, remove the nation ----//

	int i;
	for( i=state_array.size() ; i>0 ; i-- )
	{
		if( state_array[i]->campaign_nation_recno == oldNationRecno )
			break;
	}

	if( i==0 && oldNationRecno )		// the nation has no states left
	{
		err_when( oldNationRecno == CAMPAIGN_PLAYER_NATION_RECNO );		// this should not happen

		del_nation(oldNationRecno);
	}
}
//-------- End of function Campaign::state_change_nation ------//

//-------- Begin of function Campaign::temp_state_change_nation ------//
//
int Campaign::temp_state_change_nation(int stateRecno, int newNationRecno)
{
	int oldNationRecno = state_array[stateRecno]->campaign_nation_recno;

	state_array[stateRecno]->campaign_nation_recno = newNationRecno;

	//---- check if the nation still has any states left, if not, remove the nation ----//

	int i;
	for( i=state_array.size() ; i>0 ; i-- )
	{
		if( state_array[i]->campaign_nation_recno == oldNationRecno )
			break;
	}

	if( i==0 )		// the nation has no states left
	{
		return oldNationRecno;
	}

	return 0;
}
//-------- End of function Campaign::state_change_nation ------//


//-------- Begin of function Campaign::intro_text ------//

char* Campaign::intro_text()
{
	String str;

	str = stage_id;
	str += "INTRO";

	return res_stage.read(str);
}
//--------- End of function Campaign::intro_text -------//


//-------- Begin of function Campaign::plot_text ------//

char* Campaign::plot_text()
{
	String str;

	str = "PLOT";
	str.add_char(plot_category);
	str += plot_id;

	return res_plot.read(str);
}
//--------- End of function Campaign::plot_text -------//


//-------- Begin of function Campaign::goal_text ------//

char* Campaign::goal_text()
{
	String str;

	str = stage_id;
	str += "GOAL";

	return res_stage.read(str);
}
//--------- End of function Campaign::goal_text -------//


//-------- Begin of function Campaign::full_goal_text ------//

char* Campaign::full_goal_text()
{
	char* goalText = goal_text();
	char* plotText = plot_text();

	int textSize = 20 + strlen(goalText);
	
	if( plotText )
		textSize += strlen(plotText);

	err_when( !goalText );

	if( textSize > text_buf2_size )
	{
		if( !text_buf2 )
			text_buf2 = mem_add( textSize );
		else
			text_buf2 = mem_resize( text_buf2, textSize );

		text_buf2_size = textSize;
	}

	// strcpy( text_buf2, "GOAL: " );
	strcpy( text_buf2, text_reports.str_goal_() );
	strcat( text_buf2, goalText );

	if( plotText )
	{
		strcat( text_buf2, "\n" );
		strcat( text_buf2, plotText );
	}

	err_when( (int) strlen(text_buf2) > textSize-1 );

	return text_buf2;
}
//--------- End of function Campaign::full_goal_text -------//


//---- Begin of function Campaign::random_pick_monster_campaign_nation ---//
//
// Randomly pick a monster nation.
//
// [int] excludedStateRecno - if this state is given, then this state
//										will be excluded from the player state list.
//										(default: 0)
//
// [int] nextToNationRecno - if this is true, then a monster nation
//										will be picked regardless of whether it is
//										next to the player nation or not.
//										(default: CAMPAIGN_PLAYER_NATION_RECNO)
//
// [int] mustNextTo - if this is 1, then the returning state must be next
//							 to nextToNationRecno (default: 0)
//
int Campaign::random_pick_monster_campaign_nation(int excludedStateRecno, int nextToNationRecno, int mustNextTo)
{
	int monsterNationArray[MAX_NATION];
	int monsterNationCount=0, anyMonsterNationCount=0;

	int i;
	for( i=1 ; i<=MAX_NATION ; i++ )
	{
		if( is_nation_deleted(i) )
			continue;

		if( !get_nation(i)->is_monster() )
			continue;

		anyMonsterNationCount++;

		//--- only if this monster nation has a state next to the player's nation ---//

		if( !random_pick_attack_state(i, nextToNationRecno, excludedStateRecno) )
			continue;

		//--------------------------------------------//

		monsterNationArray[monsterNationCount++] = i;
	}

	//---- if there are no monster states next to the player's kingdom ----//

	if( monsterNationCount==0 && !mustNextTo )
	{
		//-- if there are still monster nations but none of them is next to the player's nation, then still attack them. --//

		if( anyMonsterNationCount>0 )
		{
			for( i=state_array.size() ; i>0 ; i-- )
			{
				int campaignNationRecno = state_array[i]->campaign_nation_recno;

				if( campaignNationRecno && get_nation(campaignNationRecno)->is_monster() )
					return i;
			}
		}

		return 0;
	}

	return monsterNationArray[ misc.random(monsterNationCount) ];
}
//---- End of function Campaign::random_pick_monster_campaign_nation -----//


//---- Begin of function Campaign::random_pick_attack_state ----//
//
// <int> attackNationRecno - the attacker CampaignNation recno.
//									  0 if it can be any campaign nation.
//
// <int> targetNationRecno - the target CampaignNation recno
//
// [int] excludedTargetStateRecno - if this state is given, then this state
//												will be excluded from the player state list.
//
// Randomly select attacker_state_recno & target_state_recno.
//
// return: <int> 1 or 0 - whether an attacking and a target state
//								  is selected.
//
int Campaign::random_pick_attack_state(int attackNationRecno, int targetNationRecno, int excludedTargetStateRecno)
{
	int attackerStateArray[MAX_STATE];		// possible attacker state
	int attackerStateCount=0;

	attacker_state_recno = 0;
	target_state_recno 	= 0;

	//-------- get a list of possible attacker states -------//

	int i;
	for( i=1 ; i<=state_array.size() ; i++ )
	{
		StateInfo* stateInfo = state_array[i];

		if( attackNationRecno &&
			 stateInfo->campaign_nation_recno != attackNationRecno )
		{
			continue;
		}

		//-- if this state is adjacent to one of the target nation's state --//

		for( int j=1 ; j<=state_array.size() ; j++ )
		{
			if( stateInfo->is_adjacent(j) &&
				 state_array[j]->campaign_nation_recno == targetNationRecno &&
				 (!excludedTargetStateRecno || excludedTargetStateRecno != j) )
			{
				attackerStateArray[attackerStateCount++] = i;
				break;
			}
		}
	}

	if( attackerStateCount==0 )
		return 0;

	//------ randomly pick an attacker state -----//

	attacker_state_recno = attackerStateArray[ misc.random(attackerStateCount) ];

	//------- randomly pick a target state -------//

	StateInfo* attackerState = state_array[attacker_state_recno];

	target_state_recno = misc.random(state_array.size())+1;

	for( i=1 ; i<=state_array.size() ; i++ )
	{
		if( ++target_state_recno > state_array.size() )
			target_state_recno = 1;

		if( attackerState->is_adjacent(target_state_recno) &&
			 state_array[target_state_recno]->campaign_nation_recno == targetNationRecno )
		{
			break;
		}
	}

	err_when( i>state_array.size() );		// no target state is selected.

	return 1;
}
//---- End of function Campaign::random_pick_attack_state -----//


//---- Begin of function Campaign::random_pick_state ----//
//
// Randomly pick a state of the specific nation.
//
// <int> nationRecno		    - randomly pick a state of this nation
// [int] neighborStateRecno - if this is given, the picked state must
//							  			be next to this state.
//
int Campaign::random_pick_state(int nationRecno, int neighborStateRecno)
{
	int stateArray[MAX_STATE];		// possible attacker state
	int stateCount=0;

	//-------- get a list of possible attacker states -------//

	for( int i=1 ; i<=state_array.size() ; i++ )
	{
		StateInfo* stateInfo = state_array[i];

		if( stateInfo->campaign_nation_recno != nationRecno )
			continue;

		if( neighborStateRecno && !stateInfo->is_adjacent(neighborStateRecno) )
			continue;

		stateArray[stateCount++] = i;
	}

	if( stateCount==0 )
		return 0;

	return stateArray[misc.random(stateCount)];
}
//---- End of function Campaign::random_pick_state -----//


//---- Begin of function Campaign::random_pick_town_with_camp ----//
//
// <int> maxLink = pick only towns with links less than this value
//
int Campaign::random_pick_town_with_camp(int nationRecno, int maxLink)
{
	int 	townRecno = misc.random(town_array.size()) + 1;
	Town* townPtr = NULL;

	for( int i=town_array.size() ; i>0 ; i-- )		// locate a town closest to the mine
	{
		if( ++townRecno > town_array.size() )
			townRecno = 1;

		if( town_array.is_deleted(townRecno) )
			continue;

		townPtr = town_array[townRecno];

		if( townPtr->nation_recno == nationRecno &&
			 townPtr->has_linked_own_camp &&
			 townPtr->linked_firm_count <= maxLink )
		{
			return townRecno;
		}
	}

	return 0;
}
//---- End of function Campaign::random_pick_town_with_camp ----//


//--------- Begin of function Campaign::set_king --------//
//
// <int> nationRecno - nation recno of the king
// <int> kingUnitRecno - unit recno of the king
// <int> firstKing - whether this is the first king of the nation.
//
void Campaign::set_king(int nationRecno, int kingUnitRecno, int firstKing)
{
	Nation* nationPtr = nation_array[nationRecno];

	//--- change the name of the unit to the default king name ---//

	if( nationPtr->campaign_nation_recno )			// this nation can be one that does not exist in the campaign. 
	{
		int kingNameId = get_nation( nationPtr->campaign_nation_recno )->king_name_id;

		unit_array[kingUnitRecno]->set_name(kingNameId);
	}

	//----- call the set_king function in NationB -----//

	nationPtr->set_king(kingUnitRecno, firstKing);
}
//----------- End of function Campaign::set_king ---------//


//--------- Begin of function Campaign::has_stage_run --------//
//
int Campaign::has_stage_run(int stageId)
{
	err_when( stageId < 1 || stageId > MAX_STAGE );

	return stage_run_count_array[stageId-1];
}
//----------- End of function Campaign::has_stage_run ---------//


//--------- Begin of function Campaign::has_event_run --------//
//
int Campaign::has_event_run(int eventId)
{
	err_when( eventId < 1 || eventId > MAX_EVENT );

	return event_run_count_array[eventId-1];
}
//----------- End of function Campaign::has_event_run ---------//


//--------- Begin of function Campaign::run_event_once --------//
//
int Campaign::run_event_once(int eventId)
{
	if( !has_event_run(eventId) )
	{
		set_event(eventId);
		return 1;
	}

	return 0;
}
//----------- End of function Campaign::run_event_once ---------//


//--------- Begin of function Campaign::run_stage_once --------//
//
int Campaign::run_stage_once(int stageId)
{
	if( !has_stage_run(stageId) )
	{
		set_stage(stageId);
		return 1;
	}

	return 0;
}
//----------- End of function Campaign::run_stage_once ---------//


//-------- Begin of function Campaign::set_relation_status ------//
//
// <int> nationRecno1 - recno of the nation to set relation with.
// <int> natoinRecno2
// <int> newStatus	 - new relation status.
//
void Campaign::set_relation_status(int nationRecno1, int nationRecno2, int newStatus)
{
	get_nation(nationRecno1)->relation_array[nationRecno2-1].status = newStatus;
	get_nation(nationRecno2)->relation_array[nationRecno1-1].status = newStatus;
}
//-------- End of function Campaign::set_relation_status ------//


//-------- Begin of function Campaign::reset_mountain_layout ------//
//
void Campaign::reset_mountain_layout()
{
	memset( mountain_layout, 0, sizeof(mountain_layout) );
}
//-------- End of function Campaign::reset_mountain_layout ------//


//-------- Begin of function Campaign::set_on_mountain_layout ------//
//
// <int> x, y 				- the location in mountain_layout to set to 1.
// [int] totalX, totalY - if this is given, the x, y given is in this scale and
//								  it will be converted to the scale of mountain_layout.
//
void Campaign::set_on_mountain_layout(int x, int y, int totalX, int totalY)
{
	if( totalX )
	{
		x = x * MOUNTAIN_LAYOUT_X_COUNT / totalX;
		y = y * MOUNTAIN_LAYOUT_Y_COUNT / totalY;
	}

	mountain_layout[y * MOUNTAIN_LAYOUT_X_COUNT + x] = 1;
}
//-------- End of function Campaign::set_on_mountain_layout ------//


//-------- Begin of function Campaign::set_campaign_difficulty ------//
//
void Campaign::set_campaign_difficulty(int campaignDifficulty)
{
	campaign_difficulty = campaignDifficulty;

	config.ai_aggressiveness = campaign_difficulty-MIN_CAMPAIGN_DIFFICULTY;		// config.ai_aggressive starts at 0 (Very Low)
}
//-------- End of function Campaign::set_campaign_difficulty ------//

