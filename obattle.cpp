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

//Filename    : OBATTLE.CPP
//Description : Battle Object

#include <OBATTLE.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OVGALOCK.H>
#include <OWORLD.H>
#include <OPOWER.H>
#include <OCONFIG.H>
#include <OSPY.H>
#include <OMOUSE.H>
#include <OMUSIC.H>
#include <OSITE.H>
#include <ORACERES.H>
#include <OHERORES.H>
#include <OGODRES.H>
#include <OTECHRES.H>
#include <OITEMRES.H>
#include <OCONFIG.H>
#include <OREMOTE.H>
#include <OUNIT.H>
#include <OTOWN.H>
#include <OF_FORT.H>
#include <OF_LAIR.H>
#include <ONEWS.H>
#include <ONATION2.H>
#include <OMONSRES.H>
#include <OCAMPGN.H>
#include <OWALLRES.H>
#include <OSTATE.H>
#include <OINFO.H>
#include <OUNITALL.H>
#include <OGAME.H>
#include <OMOUSECR.H>
#include <OTERRAIN.H>
#include <OPLASMA.H>

//-------- Define static vars --------//

enum { TOWN_STATE_MAP_WIDTH  = 100,
		 TOWN_STATE_MAP_HEIGHT = 100 };

static StateArray town_state_array;		// a state array for setting the locations of nation towns.

//-------- Begin of function Battle::Battle --------//

Battle::Battle()
{
	reset_placement_range();
}
//--------- End of function Battle::Battle ---------//


//-------- Begin of function Battle::run --------//
//
// [NewNationPara*] mpGame - new game info, only for multiplayer game
//
// [int] mpPlayerCount - the number of human multiplayers
//								 (default: 0)
//
void Battle::run(NewNationPara *mpGame, int mpPlayerCount)
{
	int oldCursor = mouse_cursor.get_icon();
	mouse_cursor.set_icon(CURSOR_WAITING);

	//----------- save the current seed for generating map -----------//

	#ifdef DEBUG2
		File seedFile;
		char *chPtr = m.format(m.get_random_seed());
		seedFile.file_create("mapseed.rs");
		seedFile.file_write(chPtr, strlen(chPtr));
		seedFile.file_close();
	#endif

	//-------- generate the map ----------//

	world.generate_map(0);

	//-------- initialize heroes --------//

	unit_res.second_init();		// call it to reset unit counts
	race_res.second_init();		// it is called here because we want to reset name_used_array[] everytime
	hero_res.second_init();		// the second_init() of hero is called here instead of in Battle::second_init because hero_res will reset all appeared_flag excepts royal unit heroes

	//------ create pregame objects ------//

	reset_placement_range();

	if( game.is_campaign_mode() )
	{
		//--- unveil new tech to the player in each new stage ----//

		static lastStageId=0;

		if( lastStageId != game.campaign()->stage_id )
		{
			lastStageId = game.campaign()->stage_id;

			game.campaign()->unveil_new_tech();
		}

		//-------------------------//

		create_pregame_object();  		// no parameters for campaign game

		//------ generate mountains -----//

//		if( !game.campaign()->mountain_layout_enabled )
//			world.gen_mountains(0, 50);		// 50% the amount of mountains in a campaign game is 50% less than a normal game.
	}
	else
	{
		init_uniqueness();

		create_pregame_object(mpGame, mpPlayerCount);
		world.generate_map2();		// generate plants and other objects on the map

		create_random_item();

		//-- reset the cash and live points as it may be modified when creating game objects --//

		for( int i=nation_array.size() ; i>0 ; i-- )
		{
			if( nation_array.is_deleted(i) )
				continue;

			int gemStones = 0;
			if( mpGame )
			{
				for( int j = 0; j < mpPlayerCount; ++j )
					if( mpGame[j].nation_recno == i && mpGame[j].use_gem_stones )
					{
						gemStones = mpGame[j].use_gem_stones;
						break;
					}
			}
			nation_array[i]->init_cash(gemStones);
		}
	}

	//------- update nation statistic -------//

	nation_array.update_statistic();

	//------- reset nation financials -------//

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		nation_array[i]->reset_all_finance();
	}

	//------- set winning deadline -------//

	if( !game.is_campaign_mode() )
	{
		if( config.goal_destroy_monster )
		{
			// ###### begin Gilbert 15/7 ########//
			// game.add_win_condition( E_DEFEAT_ALL_FRYHTAN, 1, 0, 0, 0 );
			game.add_win_condition( E_DEFEAT_ALL_FRYHTAN, 0, 0, 0, 0 );
			// ###### end Gilbert 15/7 ########//
		}

		// ###### begin Gilbert 24/5 #########//
		// second parameter (player_only) to 1
		if( config.goal_population_flag )
		{
			game.add_win_condition( E_REACH_POPULATION,   0, 0, config.goal_population, 0 );
			game.add_win_condition( E_REACH_FRYHTAN,      0, 0, config.goal_population, 0 );
		}

		if( config.goal_economic_score_flag )
		{
			game.add_win_condition( E_REACH_ECONOMY,      0, 0, config.goal_economic_score, 0 );
		}

		if( config.goal_total_score_flag )
		{
			game.add_win_condition( E_REACH_TOTAL,         0, 0, config.goal_total_score, 0 );
		}
		// ###### end Gilbert 24/5 #########//

		// ########### begin Gilbert 20/7 ##########//
		// disallow cheat using 2 multi-players and allied win
		if( config.goal_alliance_win_flag && nation_array.packed_size() >= 4 )
		// ########### end Gilbert 20/7 ##########//
		{
			game.add_win_condition( E_DEFEAT_EXCEPT_ALLIANCE, 1, 0, 0, 0 );
		}
	}

	game.set_win_deadline();

	// check of nation_array.player_recno

	err_when( nation_array.player_recno
		&& (~nation_array)->nation_recno != nation_array.player_recno );

	//------- display a 100% progress bar -------//

	game.disp_gen_game_status(100);

	//---- reset config parameter ----//

	sys.set_speed(12);		// set the speed to normal.

	//---- reset cheats ----//

	config.fast_build = 0;
	config.blacken_map = 1;
	config.disable_ai_flag = 0;

#ifdef DEBUG
	config.show_ai_info = sys.debug_session;	// BUGHERE, for debugging only
	config.show_debug_info = 0;			// BUGHERE, for debugging only
	config.king_undie_flag = sys.debug_session && !mpGame && !game.is_campaign_mode();		// sys.testing_session && !mpGame;
#else
	config.show_ai_info = 0;
	config.show_debug_info = 0;
	config.king_undie_flag = 0;
#endif

	if( sys.testing_session )
		config.show_unit_path = 3;

	// enable tech and god, for multi-player testing
#if(0)
	for( i = 1; i < nation_array.size(); ++i )
	{
		if( !nation_array.is_deleted(i) && !nation_array[i]->is_ai() )
		{
			tech_res.inc_all_tech_level(i);
			god_res.enable_know_all(i);
		}
	}
#endif

	//----- center display on the player's unit/town ----//

	center_disp_on_player();

	//------- enable/disable sound effects -------//

	mouse_cursor.restore_icon(oldCursor);

	//--- give the control to the system main loop, start the game now ---//

	game.main_loop_all();
}
//--------- End of function Battle::run ---------//


//-------- Begin of function Battle::init_uniqueness --------//
//
// Initialize random uniqueness for each game or each campaign.
//
void Battle::init_uniqueness()
{
	unit_res.second_init();
//	race_res.second_init();		// race_res is initialized here instead of in race_res because campaign will use it (before a game is started and game.init() is called.)
	monster_res.second_init();
	god_res.second_init();
	tech_res.second_init();
}
//--------- End of function Battle::init_uniqueness ---------//


//-------- Begin of function Battle::set_placement_range --------//

void Battle::set_placement_range(int rangeX1, int rangeY1, int rangeX2, int rangeY2)
{
	err_when( rangeX1 < 0 || rangeX1 >= MAX_WORLD_X_LOC );
	err_when( rangeX2 < 0 || rangeX2 >= MAX_WORLD_X_LOC );
	err_when( rangeX1 < 0 || rangeY1 >= MAX_WORLD_Y_LOC );
	err_when( rangeY2 < 0 || rangeY2 >= MAX_WORLD_Y_LOC );

	err_when( rangeX1 > rangeX2 );
	err_when( rangeY1 > rangeY2 );

	placement_range_x1 = rangeX1;
	placement_range_y1 = rangeY1;
	placement_range_x2 = rangeX2;
	placement_range_y2 = rangeY2;
}
//--------- End of function Battle::set_placement_range ---------//


//-------- Begin of function Battle::reset_placement_range --------//

void Battle::reset_placement_range()
{
	placement_range_x1 = 0;
	placement_range_y1 = 0;
	placement_range_x2 = MAX_WORLD_X_LOC-1;
	placement_range_y2 = MAX_WORLD_Y_LOC-1;
}
//--------- End of function Battle::reset_placement_range ---------//


//-------- Begin of function Battle::create_pregame_object --------//
//
// Initialize pre-game objects - towns, sites, independent towns.
//
// <NewNationPara*> mpGame - new game info, only for multiplayer game
//
// <int> mpPlayerCount - the number of human multiplayers
//								 (default: 0)
//
void Battle::create_pregame_object(NewNationPara *mpGame, int mpPlayerCount)
{
	//----- init town_state_array ------//

	town_state_array.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	town_state_array.generate(9);

	//------- create player nation --------//

	if( !mpGame )
	{
		int nationRecno = nation_array.new_nation( NATION_HUMAN, NATION_OWN,
								config.race_id, config.player_nation_color );

		nation_array.set_custom_both_name( nationRecno, config.player_name );
	}
	else
	{
		for( int i = 0; i < mpPlayerCount; ++i )
		{
			int nationRecno = nation_array.new_nation(mpGame[i]);

			if( nationRecno != mpGame[i].nation_recno )
				err.run( "Unexpected nation recno created" );

			nation_array.set_custom_both_name( nationRecno, mpGame[i].player_name );
		}
	}

	//--------- create ai nations --------//

	if( mpGame )
	{
		int i=0;

		while( config.ai_nation_count() + mpPlayerCount > MAX_NATION, i++ )
		{
			if( i%2==0 )
				config.ai_human_nation_count--;
			else
				config.ai_monster_nation_count--;

			err_when( ++i > 100 );
		}
	}

	create_ai_nation();

	//---- every human nation has a scroll of power in the beginning ----//

	Nation* nationPtr;

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		nationPtr = nation_array[i];

		if( nationPtr->is_human() )
			god_res[nationPtr->race_id]->enable_know(i);
	}

	//------- create towns and units --------//

	int 	nationRecno, rc;
	int	noSpaceFlag=0;

	for( nationRecno=1 ; nationRecno<=nation_array.size() ; nationRecno++ )
	{
		if( nation_array.is_deleted(nationRecno) )
			continue;

		nationPtr = nation_array[nationRecno];

		if( nationPtr->race_id > 0 )					// if this is a human nation
			rc = create_one_human_asset(nationPtr);
		else
			rc = create_one_monster_asset(nationPtr);

		if( !rc )
		{
			noSpaceFlag = 1;
			break;
		}
	}

	// ---- establish contact in multiplayer -------//

	if( mpGame && !config.explore_whole_map )
	{
		for( nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno )
		{
			if( nation_array.is_deleted(nationRecno) )
				continue;
			nationPtr = nation_array[nationRecno];

			if( !nationPtr->is_own() && !nationPtr->is_remote() )
				continue;

			for( int n2 = nationRecno+1; n2 <= nation_array.size(); ++n2 )
			{
				if( nation_array.is_deleted(n2) )
					continue;
				Nation *n2Ptr = nation_array[n2];

				if( !n2Ptr->is_own() && !n2Ptr->is_remote() )
					continue;

				// ----- establish contact with nationRecno and n2 ---------//

				nationPtr->establish_contact(n2);
			}
		}
	}

	//--- if there is no space for creating new town/firm or unit, delete the unprocessed nations ---//

	if( noSpaceFlag )
	{
		for( ; nationRecno<=nation_array.size() ; nationRecno++ )
			nation_array.del_nation(nationRecno);		// no space for creating a town for the nation, so we have to delete the nation
	}

	game.disp_gen_game_status(5);

	//---- init the type of active monsters in this game ----//

	game.disp_gen_game_status(5);

	//------- create raw sites ----------//

	site_array.generate_raw_site(config.start_up_raw_site);

	game.disp_gen_game_status(5);

	//------ create independent towns -------//

	create_independent_town(config.start_up_independent_town);

	//------ create independent monster lairs ------//

	// ###### begin Gilbert 2/7 ######//
	// if( config.ai_monster_nation_count > 0 )			// only create independent Fryhtan lairs if the number of Fryhtan kingdoms is set to larger than 0
	create_monster_lair(0, MAX_MONSTER_TYPE);		// 0-independent fryhtan
	// ###### end Gilbert 2/7 ######//

	// --------- generate roads -----------//

	create_road();
}
//--------- End of function Battle::create_pregame_object ---------//


//-------- Begin of function Battle::create_ai_nation --------//
//
// Create AI nations.
//
void Battle::create_ai_nation()
{
	int raceId;

	// ####### begin Gilbert 29/1 #######//
	// in multiplayer, human nation + ai_human + ai_monster nation
	// may > MAX_NATION
	int aiHumanCount = config.ai_human_nation_count;
	int aiMonsterCount = config.ai_monster_nation_count;
	int remainCount = MAX_NATION - nation_array.packed_size();

	if( remainCount <= 0 )
		return;

	if( aiHumanCount + aiMonsterCount > remainCount )
	{
		// divide approximately according to proportion
		int total = aiHumanCount + aiMonsterCount;

		aiHumanCount = (aiHumanCount * remainCount + total - 1) / total;
		aiMonsterCount = remainCount - aiHumanCount;
		err_when( aiMonsterCount < 0);
	}

	//------- add human nations -----------//

	for( int i=0 ; i<aiHumanCount ; i++ )
	{
		if( config.random_start_up && !game.is_campaign_mode() )
			raceId = m.random(MAX_RACE)+1;
		else
			raceId = nation_array.random_unused_race();

		err_when( raceId < 1 || raceId > MAX_RACE );

		nation_array.new_nation( NATION_HUMAN, NATION_AI, raceId, nation_array.random_unused_color() );     // 2nd parameter = the race id., 3rd parameters = color scheme id.
	}

	//------- add Fryhtan nations --------//

	for( i=0 ; i<aiMonsterCount ; i++ )
	{
		if( config.random_start_up && !game.is_campaign_mode() )
			raceId = -(m.random(MAX_MONSTER_TYPE)+1);
		else
			raceId = -nation_array.random_unused_monster_type();

		nation_array.new_nation( NATION_MONSTER, NATION_AI, raceId, nation_array.random_unused_color() );     // 2nd parameter = the race id., 3rd parameters = color scheme id.
	}

	// ####### end Gilbert 29/1 #######//
}
//--------- End of function Battle::create_ai_nation ---------//


//-------- Begin of function Battle::create_one_human_asset --------//
//
// return: <int> 1 - created successfully
//					  0 - failed, because of no available space
//
int Battle::create_one_human_asset(Nation* nationPtr)
{
	//------ create startup towns and units ------//

	int townRecno = create_human_town_unit(nationPtr);

	if( !townRecno )
		return 0;

	//----- create skilled units if config.random_start_up is 1 -----//

	if( config.random_start_up && !game.is_campaign_mode() )
	{
		Town* townPtr = town_array[townRecno];

		int createCount = (60-townPtr->population)/3;		// the less population the villager has the more mobile units will be created

		for( int i=0 ; i<createCount ; i++ )
		{
			int unitId;

			if( m.random(2)==0 )
				unitId = race_res[nationPtr->race_id]->infantry_unit_id;
			else
				unitId = race_res[ m.random(MAX_RACE)+1 ]->infantry_unit_id;

			int rankId;

			if( m.random(4)==0 )				// lower probability
				rankId = RANK_GENERAL;
			else
				rankId = RANK_SOLDIER;

			if( !create_unit(nationPtr->nation_recno, unitId, rankId,
				 townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2 ) )
			{
				break;
			}
		}
	}

	//------ create mines near towns in the beginning -----//

	if( config.start_up_has_mine_nearby && !nationPtr->is_ai() )
		site_array.create_raw_site(0, townRecno);

	return 1;
}
//--------- End of function Battle::create_one_human_asset ---------//


//-------- Begin of function Battle::create_human_town_unit --------//
//
int Battle::create_human_town_unit(Nation* nationPtr)
{
	int 	townRecno, firmRecno;
	Town* townPtr;

	for( int i=10 ; i>0 ; i-- )		// try a maximum of 10 times
	{
		townRecno = create_human_town(nationPtr);

		//------- create military camp -------//

		townPtr = town_array[townRecno];

		firmRecno = firm_array.generate_firm(townPtr->loc_x1 + STD_TOWN_LOC_WIDTH + INTER_PLACE_SPACE,
			townPtr->loc_y1, nationPtr->nation_recno, FIRM_FORT, race_res[nationPtr->race_id]->code );

		if( firmRecno )
			break;

		// ###### begin Gilbert 7/7 #######//
		// undo create objects
		town_array.del_town(townRecno);
		// ###### end Gilbert 7/7 #######//
	}

	if( !firmRecno )
		return 0;

	FirmFort* firmFort = (FirmFort*) firm_array[firmRecno];

	//------- set active link ----------//

	firmFort->active_link_town_recno = townRecno;
	
	firmFort->current_archer_count = 5;
	firmFort->target_archer_count  = 5;

	memset( firmFort->last_archer_fire_frame, 0, sizeof(firmFort->last_archer_fire_frame) );

	firmFort->train_archer_progress = 0;

	//--------- create units ----------//

	int unitId = race_res[nationPtr->race_id]->infantry_unit_id;

	int unitRecno = create_unit(nationPtr->nation_recno,
		unitId, nationPtr->king_unit_recno==0 ? RANK_KING : RANK_GENERAL,
		townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2 );

	if( !unitRecno )
		return 0;

	if( !nationPtr->king_unit_recno )
		nationPtr->set_king(unitRecno, 1);		// 1-this is the first king of the nation

	if( nationPtr->race_id > 0 )		// if this is a human nation
		firmFort->assign_overseer(unitRecno);	// assign the king as the overseer of the command base

	//------ create soldiers in the fort ------//

	int soldierAddCount=7;		// having 7 soldiers in the beginning

	for( i=0 ; i<soldierAddCount ; i++ )
		firmFort->recruit_soldier(townRecno, false, COMMAND_AUTO, 1);		// 1-create at no cost

	Soldier* soldierPtr = firmFort->soldier_array;

	for( i=0 ; i<firmFort->soldier_count ; i++, soldierPtr++ )
	{
		soldierPtr->skill.set_combat_level( BASIC_COMBAT_TRAIN );
		soldierPtr->skill.set_skill_level( BASIC_SKILL_TRAIN );
		soldierPtr->hit_points = soldierPtr->max_hit_points();
		soldierPtr->complete_training();
	}

	//-----------------------------------------//

	return townRecno;
}
//--------- End of function Battle::create_human_town_unit ---------//


//-------- Begin of function Battle::create_human_town --------//
//
int Battle::create_human_town(Nation* nationPtr)
{
	int xLoc, yLoc;
	int townRecno=0;

	//---- attempt to locate an area for building the town using town_state_array ---//

	int stateRecno = m.random(town_state_array.size())+1;

	for( int i=town_state_array.size() ; i>0 ; i-- )
	{
		if( ++stateRecno > town_state_array.size() )
			stateRecno = 1;

		StateInfo* stateInfo = town_state_array[stateRecno];

		xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		if( xLoc < placement_range_x1 || yLoc < placement_range_y1 ||
			 xLoc > placement_range_x2 || yLoc > placement_range_y2 )
		{
			continue;
		}

		if( world.can_build_town(xLoc, yLoc) )
		{
			townRecno = create_town(nationPtr->nation_recno, nationPtr->race_id, xLoc, yLoc);

			if( townRecno )
				break;
		}
	}

	//---- if not successful with town_state_array, then randomly locate an area ----//

	if( !townRecno )
		townRecno = create_town_random_loc( nationPtr->nation_recno, nationPtr->race_id, xLoc, yLoc );

	return townRecno;
}
//--------- End of function Battle::create_human_town ---------//


//-------- Begin of function Battle::create_one_monster_asset --------//
//
int Battle::create_one_monster_asset(Nation* nationPtr)
{
	//------- locate for a space to build the lair ------//

	const int interPlaceSpace = 5;

	int xLoc=placement_range_x1;
	int yLoc=placement_range_y1;
	int maxTries = (placement_range_x2-placement_range_x1) * (placement_range_y2-placement_range_y1);
	FirmInfo* firmInfo = firm_res[FIRM_LAIR];

	if( !world.locate_space_random(xLoc, yLoc, placement_range_x2, placement_range_y2,
		 firmInfo->loc_width+interPlaceSpace*2, firmInfo->loc_height+interPlaceSpace*2, maxTries) )
	{
		return 0;
	}

	//---------- create the lair ----------//

	int firmRecno = create_firm(xLoc+interPlaceSpace, yLoc+interPlaceSpace,
						 nationPtr->nation_recno, FIRM_LAIR, nationPtr->race_id );
	
	if( !firmRecno )
		return 0;

	FirmLair* firmLair = firm_array[firmRecno]->cast_to_FirmLair();

	//--------- create the Fryhtans king ---------//

	int unitId = monster_res[nationPtr->monster_id()]->unit_id;

	int unitRecno = create_unit(nationPtr->nation_recno,
		unitId, nationPtr->king_unit_recno==0 ? RANK_KING : RANK_GENERAL,
		firmLair->loc_x1, firmLair->loc_y1, firmLair->loc_x2, firmLair->loc_y2 );

	if( !unitRecno )
		return 0;

	unit_array[unitRecno]->skill.set_skill_level(100);

	if( !nationPtr->king_unit_recno )
		nationPtr->set_king(unitRecno, 1);		// 1-this is the first king of the nation

	firmLair->assign_overseer(unitRecno);	// assign the king as the overseer of the command base

	//------ create Fryhtan soldiers in the fort ------//

	int addCount = 300 / ((int)unit_res[unitId]->hit_points * 20 / 100);		// 20 is the startup combat level

	for( int i=0 ; i<addCount ; i++ )
	{
		firmLair->recruit_soldier(COMMAND_AUTO);
	}

	Soldier* soldierPtr = firmLair->soldier_array;

	for( i=0 ; i<firmLair->soldier_count ; i++, soldierPtr++ )
	{
		soldierPtr->skill.set_combat_level( 20 );
		soldierPtr->skill.set_skill_level( 20 );
		soldierPtr->hit_points = soldierPtr->max_hit_points();
		soldierPtr->remain_build_days = 0;		// end basic training
	}

	return 1;
}
//--------- End of function Battle::create_one_monster_asset ---------//


//-------- Begin of function Battle::create_independent_town --------//
//
void Battle::create_independent_town(int addCount)
{
	//--- only a maximum of 7 different races for independent towns, the 7 races are determined by the races of the kingdoms in the game ----//

	//---- count the number of nations each race has ----//

	int raceId;
	int raceCountArray[MAX_RACE];

	memset( raceCountArray, 0, sizeof(raceCountArray) );

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		raceId = nation_array[i]->race_id;

		if( raceId > 0 )
			raceCountArray[raceId-1]++;
	}

	//---- count the number of races added ----//

	int raceTypeCount=0;

	// ####### begin Trevor 19/2 ########//
	for( i=0 ; i<MAX_RACE; i++ )
	// ####### end Trevor 19/2 ########//
	{
		if( raceCountArray[i] )
			raceTypeCount++;
	}

	//---- randomly add towns of other races until the total number of races is 7 ---//

	#define MAX_INDEPENDENT_TOWN_RACE_TYPE		7

	int loopCount=0;

	while(1)
	{
		err_when( ++loopCount > 10000 );

		raceId = m.random(MAX_RACE)+1;

		if( raceCountArray[raceId-1]==0 )
		{
			raceCountArray[raceId-1]++;

			if( ++raceTypeCount >= MAX_INDEPENDENT_TOWN_RACE_TYPE )
				break;
		}
	}

	//------ create independent towns now -------//

	raceId=0;

	for( i=0 ; i<1000 && addCount>0 ; i++ )		// 1000 is the maximum number of tries
	{
		if( ++raceId > MAX_RACE )
			raceId = 1;

		int xLoc, yLoc;

		if( raceCountArray[raceId-1] )
		{
			if( create_town_random_loc(0, raceId, xLoc, yLoc) )
				addCount--;
			else
				addCount=0;
		}
	}

	//----- finish generating town -------//

	game.disp_gen_game_status(10);
}
//--------- End of function Battle::create_independent_town ---------//


//-------- Begin of function Battle::create_road --------//
//
void Battle::create_road()
{
	if( town_array.size() > 2 )
	{
		Plasma roadMap;

		roadMap.init(MAX_WORLD_X_LOC, MAX_WORLD_Y_LOC, terrain_res.max_height(TERRAIN_DARK_GRASS));

		short town1, town2;
		int   townRecno;

		for( town1 = 1; town1 < town_array.size(); ++town1 )
		{
			if( town_array.is_deleted(town1) )
				continue;

			town2 = 0;
			int minDist = MAX_WORLD_X_LOC + MAX_WORLD_Y_LOC;

			for( townRecno = town1+1; townRecno <= town_array.size(); ++townRecno )
			{
				if( town_array.is_deleted(townRecno) )
					continue;

				int townDist = abs(town_array[townRecno]->center_x - town_array[town1]->center_x)
					+ abs(town_array[townRecno]->center_y - town_array[town1]->center_y);

				if( townDist < minDist && townDist > 0x10 )
				{
					minDist = townDist;
					town2 = townRecno;
				}
			}

			if( !town2 )
				continue;

			world.gen_road( roadMap, town_array[town1]->center_x, town_array[town1]->center_y,
				town_array[town2]->center_x, town_array[town2]->center_y );
		}

		world.set_road_id( roadMap );
	}

	//---------------------------------------//

	game.disp_gen_game_status(5);
}
//--------- End of function Battle::create_road ---------//

//-------- Begin of function Battle::create_unit --------//
//
// <int> nationRecno - nation recno
// <int> unitId		- unit id.
// <int> rankId		- rank id.
//
// [int] nextXLoc1, nextYLoc1 - if these are given, the unit will be created
//		   nextXLoc2, nextYLoc2	  next to this area. e.g. these can be
//									     coordinates of a town.
//
// return: <int> recno of the added unit.
//					  0 - if no unit is added.
//
int Battle::create_unit(int nationRecno, int unitId, int rankId, int nextXLoc1, int nextYLoc1, int nextXLoc2, int nextYLoc2 )
{
	SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];
	char 	teraMask = UnitRes::mobile_type_to_mask(unit_res[unitId]->mobile_type);
	int   xLoc, yLoc;

	//------ seek a location to create the unit -------//

	if( nextXLoc1 >= 0 )
	{
		xLoc = nextXLoc1;
		yLoc = nextYLoc1;

		world.check_unit_space_avoid_mountain = 1;	// set this for world.locate_space()_

		if( !world.locate_space( xLoc, yLoc, nextXLoc2, nextYLoc2,
										 spriteInfo->loc_width, spriteInfo->loc_height ) )
		{
			world.check_unit_space_avoid_mountain = 0;

			return 0;
		}

		world.check_unit_space_avoid_mountain = 0;
	}
	else
	{
		xLoc=placement_range_x1;
		yLoc=placement_range_y1;

		int maxTries = (placement_range_x2-placement_range_x1) * (placement_range_y2-placement_range_y1);

		if( !world.locate_space_random(xLoc, yLoc, placement_range_x2,
			 placement_range_y2, spriteInfo->loc_width, spriteInfo->loc_height,
			 maxTries, 0, 0, teraMask) )
		{
			return 0;
		}
	}

	//-------- add the unit now --------//

	int unitLoyalty = 80 + m.random(20);

	// ###### begin Gilbert 9/4 #########//
	// if create in campaign, use CampaignNation->king

	int unitRecno = 0;
	int cNationRecno;
	CampaignNation *cNationPtr;
	int createFromMember = 0;

	if( game.is_campaign_mode() && rankId == RANK_KING && nationRecno 
		&& (cNationRecno = nation_array[nationRecno]->campaign_nation_recno)
		&& (cNationPtr = game.campaign()->get_nation(cNationRecno))
		&& cNationPtr->king.unit_id )
	{
		unitRecno = cNationPtr->king.create_unit( nationRecno, xLoc, yLoc );
		createFromMember = 1;
	}
	else
	{
		unitRecno = unit_array.add_unit( unitId, nationRecno, rankId, unitLoyalty, xLoc, yLoc );
	}
	// ###### end Gilbert 9/4 #########//

	if( !unitRecno )
		return 0;

	//----------- set skill -------------//

	Unit* unitPtr = unit_array[unitRecno];

	// ###### begin Gilbert 9/4 #######//
	if( !createFromMember )
	{
		switch( rankId )
		{
			case RANK_KING:
				unitPtr->skill.set_skill_level(100);
				unitPtr->set_combat_level(100);
				break;

			case RANK_GENERAL:
				unitPtr->skill.set_skill_level(40 + m.random(50));		// 40 to 90
				unitPtr->skill.skill_potential = m.random(50);
				unitPtr->set_combat_level(30 + m.random(70));		// 30 to 100
				break;

			case RANK_SOLDIER:
				if( unitPtr->race_id )
				{
					unitPtr->skill.set_skill_level(10+m.random(10));	// 10 to 20
					unitPtr->skill.skill_potential = m.random(50);
					unitPtr->set_combat_level(20+m.random(60));		// 20 to 80
				}
				break;

			default:
				err_here();
		}
	}
	// ###### end Gilbert 9/4 #######//

	return unitRecno;
}
//--------- End of function Battle::create_unit ---------//


//-------- Begin of function Battle::create_town --------//
//
// <int> nationRecno = the nation recno of the town
// <int> raceId      = the race id. of the town
// <int> xLoc, yLoc  = location of the town
// [int] initPop 		= town population, if not specified, it will be randomly determined
//
// return: <int> townRecno - >0  the recno of the town created
//                           ==0 no town created
//
int Battle::create_town(int nationRecno, int raceId, int xLoc, int yLoc, int initPop)
{
	//--------------- create town ---------------//

	int townRecno = town_array.generate_town(nationRecno, raceId, xLoc, yLoc);

	Town* townPtr = town_array[townRecno];

	if( nationRecno )
	{
		if( !initPop )
		{
			if( config.random_start_up && !game.is_campaign_mode() )
				initPop = 30 + m.random(31);		// 30 to 60
			else
				initPop = 60;
		}

		townPtr->init_pop( initPop, 100, 0, 1 );		// 100-startup loyalty, last 1-first initialization at the beginning of the game
	}
	else
	{
		int townResistance = town_array.independent_town_resistance();

		if( !initPop )
			initPop = 30 + m.random(21);			// 30 to 50

		townPtr->init_pop( initPop, townResistance, 0, 1 ); 	// last 1-first initialization at the beginning of the game
	}

	//---------- set town layout -----------//

	townPtr->auto_set_layout();

	//--------- set town walls (for independent towns only) --------//

	if( townPtr->nation_recno==0 )
	{
		if( config.independent_town_resistance == OPTION_MODERATE )
		{
			int rc = m.random(11);		// 0 to 10

			if( rc >= 9 )			//	9 to 10
				townPtr->set_current_wall_level(2);

			else if( rc >= 4 )	// 4 to 8
				townPtr->set_current_wall_level(1);
		}

		else if( config.independent_town_resistance == OPTION_HIGH )
		{
			int rc = m.random(11);		// 0 to 10

			if( rc >= 7 )			//	7 to 10
				townPtr->set_current_wall_level(2);
			else 						// 0 to 6
				townPtr->set_current_wall_level(1);
		}
	}

	return townRecno;
}
//--------- End of function Battle::create_town ---------//


//-------- Begin of function Battle::create_town_random_loc --------//
//
// Create a town at a random location.
//
// <int> nationRecno = the nation recno of the town
// <int> raceId      = the race id. of the town
//
// <int&> xLoc = for the starting location of the town
// <int&> yLoc = for the starting location of the town
//
// [int] initPop = town population, if not specified, it will be randomly determined
//
// return: <int> townRecno - >0  the recno of the town created
//                           ==0 no town created
//
int Battle::create_town_random_loc(int nationRecno, int raceId, int& xLoc, int& yLoc, int initPop)
{
	//------- locate for a space to build the town ------//

	int maxTries = (placement_range_x2-placement_range_x1) * (placement_range_y2-placement_range_y1);

	if( !town_array.think_town_loc(maxTries, xLoc, yLoc,
		 placement_range_x1, placement_range_y1, placement_range_x2, placement_range_y2 ) )
	{
		return 0;
	}

	return create_town(nationRecno, raceId, xLoc, yLoc, initPop);
}
//--------- End of function Battle::create_town_random_loc ---------//


//-------- Begin of function Battle::create_town_next_to_place --------//
//
// <Place*> placePtr - pointer to the place to be built next to.
//
// <int> nationRecno - the nation recno of the town
// <int> raceId      - the race id. of the town
//
// [int] initPop = town population, if not specified, it will be randomly determined
//
// return: <int> townRecno - >0  the recno of the town created
//                           ==0 no town created
//
int Battle::create_town_next_to_place(Place* placePtr, int nationRecno, int raceId, int initPop)
{
	short buildXLoc, buildYLoc;

	Nation* callNationPtr;	// for calling find_best_place_loc() only 

	if( nationRecno )
		callNationPtr = nation_array[nationRecno];
	else
		callNationPtr = nation_array[placePtr->nation_recno];

	err_when( !callNationPtr );

	if( !callNationPtr->find_best_place_loc(0, placePtr->loc_x1, placePtr->loc_y1, buildXLoc, buildYLoc) )
		return 0;

	return create_town(nationRecno, raceId, buildXLoc, buildYLoc, initPop);
}
//--------- End of function Battle::create_town_next_to_place ---------//


//------- Begin of function Battle::create_firm -------//
//
// Create a specific firm next to a specific town.
//
// <int> xLoc, yLoc  - location of the firm
// <int> nationRecno - nation recno of the firm
// <int> firmId      - id. of the firm.
// <int> firmRaceId  - race id. of the firm.
//
int Battle::create_firm(int xLoc, int yLoc, int nationRecno, int firmId, int firmRaceId)
{
	//----- get the build code -----//

	char *buildCode;

	err_when( firmRaceId==0 );

	if( firmRaceId > 0 )
		buildCode = race_res[firmRaceId]->code;
	else
		buildCode = monster_res[-firmRaceId]->firm_build_code;

	//----- build the firm now ------//

	return firm_array.generate_firm(xLoc, yLoc, nationRecno, firmId, buildCode);
}
//-------- End of function Battle::create_firm -------//


//------- Begin of function Battle::create_firm_next_to_place -------//
//
// Create a specific firm next to a specific town.
//
// <Place*> placePtr 	- pointer to the place to be built next to.
// <int> 	firmId     	- id. of the firm.
// <int> 	firmRaceId 	- race id. of the firm.
// [int]    nationRecno - if not given, the nation of the place will be used
//								  (default: -1)
// [short]  distance    - the distance between the given place and
//								  new place.
//
int Battle::create_firm_next_to_place(Place* placePtr, int firmId,
								int firmRaceId, int nationRecno, short distance)
{
	short buildXLoc, buildYLoc;

	if( nationRecno == -1 )
		nationRecno = placePtr->nation_recno;

	Nation* nationPtr = nation_array[nationRecno];

	//-------------------------------------//

	if( !nationPtr->find_best_place_loc(firmId, placePtr->loc_x1, placePtr->loc_y1, buildXLoc, buildYLoc, distance) )
		return 0;

	//--- see if the new firm is within the effective distance of the given place ---//

	FirmInfo* firmInfo = firm_res[firmId];

	int effectiveDis = world.effective_distance(firmId, (placePtr->cast_to_Firm() ? placePtr->cast_to_Firm()->firm_id : 0) );

	if( m.points_distance( placePtr->center_x, placePtr->center_y,
		 buildXLoc+firmInfo->loc_width/2, buildYLoc+firmInfo->loc_height/2 )
		 > effectiveDis )
	{
		return 0;
	}

	//----- get the build code -----//

	char *buildCode;

	err_when( firmRaceId==0 );

	if( firmRaceId > 0 )
		buildCode = race_res[firmRaceId]->code;
	else
		buildCode = monster_res[-firmRaceId]->firm_build_code;

	//----- build the firm now ------//

	return firm_array.generate_firm(buildXLoc, buildYLoc, nationRecno, firmId, buildCode );
}
//-------- End of function Battle::create_firm_next_to_place -------//


//----- Begin of function Battle::create_monster_lair -----//
//
// <int> nationRecno - nation recno of the Fryhtan Lair
//							  0 for independent Fryhtan Lair
// <int> addCount		- the number of lairs to be added.
//
void Battle::create_monster_lair(int nationRecno, int addCount)
{
	FirmInfo* firmInfo = firm_res[FIRM_LAIR];
	int maxTries = (placement_range_x2-placement_range_x1) * (placement_range_y2-placement_range_y1);

	enum { LAIR_INTER_SPACE = 6 };

	for( int i=0 ; i<addCount ; i++ )
	{
		//------ locate space for the monster lair ------//

		int xLoc = placement_range_x1;
		int yLoc = placement_range_y1;

		if( !world.locate_space_random( xLoc, yLoc, placement_range_x2, placement_range_y2,
										 firmInfo->loc_width+LAIR_INTER_SPACE*2, firmInfo->loc_height+LAIR_INTER_SPACE*2, maxTries) )		// 5 space locations surrounding the building
		{
			break;
		}

		//---------- add the firm now -----------//

		int monsterId;

		if( nationRecno )
			monsterId = nation_array[nationRecno]->monster_id();
		else
			monsterId = m.random(MAX_MONSTER_TYPE)+1;

		int firmRecno = firm_array.generate_firm(xLoc+LAIR_INTER_SPACE, yLoc+LAIR_INTER_SPACE, nationRecno,
							 FIRM_LAIR, monster_res[monsterId]->firm_build_code );

		if( !firmRecno )		// no space for building the firm
			continue;

		FirmLair* firmLair = firm_array[firmRecno]->cast_to_FirmLair();

		//--------- create monster unit ----------//

		int unitId = monster_res[monsterId]->unit_id;

		xLoc = firmLair->loc_x1;
		yLoc = firmLair->loc_y1;

		SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];

		if( !world.locate_space( xLoc, yLoc, xLoc+firmInfo->loc_width-1, yLoc+firmInfo->loc_height-1,
										 spriteInfo->loc_width, spriteInfo->loc_height ) )
		{
			err_here();									// this shouldn't happen
			continue;
		}

		//--------- determine unit rank -------//

		int rankId;

		if( nationRecno==0 || nation_array[nationRecno]->king_unit_recno )		// if it is an independent nation or the nation unit already has a king, then set this unit to general
			rankId = RANK_GENERAL;
		else
			rankId = RANK_KING;

		//----------- add unit now -----------//

		int unitRecno = unit_array.add_unit( unitId, nationRecno, rankId, 0, xLoc, yLoc );

		if( !unitRecno )
		{
			err_here();
			continue;
		}

		Unit* unitPtr = unit_array[unitRecno];

		unitPtr->skill.set_skill_level( 20 + m.random(80) );
		unitPtr->set_combat_level( 50 + m.random(50) );

		firmLair->assign_overseer(unitRecno);	// assign the king as the overseer of the command base

		//------- create monster soldiers -----//

		int soldierCount = MAX_SOLDIER/4 + m.random(MAX_SOLDIER/3)+1;

		for( int j=0 ; j<soldierCount ; j++ )
			firmLair->recruit_soldier(COMMAND_AUTO);
	/*	
		Soldier* soldierPtr = firmLair->soldier_array;

		for( j=0 ; j<firmLair->soldier_count ; j++, soldierPtr++ )
		{
			soldierPtr->skill.set_combat_level( 30 + m.random(30) );
			soldierPtr->skill.set_skill_level( 10 + m.random(10) );
		}
	*/
	}
}
//------ End of function Battle::create_monster_lair -------//


//-------- Begin of function Battle::is_space --------//
//
// Check whether all locations in the given area are space
//
// return : <int> 1 - all are space
//                0 - not all are space
//
int Battle::is_space(int xLoc1, int yLoc1, int xLoc2, int yLoc2, char mobileType)
{
	int       xLoc, yLoc;
	Location* locPtr;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		locPtr = world.get_loc(xLoc1, yLoc);

		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			if( !locPtr->can_move(mobileType) )
				return 0;
		}
	}

	return 1;
}
//--------- End of function Battle::is_space ---------//


//-------- Begin of function Battle::center_disp_on_player --------//
//
void Battle::center_disp_on_player()
{
	//------ highlight the king ------//

	short xLoc, yLoc;

	unit_array[ (~nation_array)->king_unit_recno ]->get_cur_loc(xLoc, yLoc);

	world.go_loc( xLoc, yLoc );

/*
	//--- highlight the player's town/unit in the beginning of the game ---//

	Town* townPtr;

	for( int i=1 ; i<=town_array.size() ; i++ )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno == nation_array.player_recno )
		{
			world.go_loc( townPtr->loc_x1, townPtr->loc_y1 );
			break;
		}
	}

	if( i>town_array.size() )		// the player has no town
	{
		Unit* unitKing = unit_array[ (~nation_array)->king_unit_recno ];

		short xLoc, yLoc;
		unitKing->get_cur_loc(xLoc, yLoc);		// get the unit's location no matter whether the unit is in a building or an open space

		world.go_loc( xLoc, yLoc );
	}
*/
}
//--------- End of function Battle::center_disp_on_player ---------//


//-------- Begin of function Battle::inverse_placement_range --------//
//
void Battle::inverse_placement_range()
{
	//--- if it is horizontally split -----//

	if( placement_range_y1==0 && placement_range_y2==MAX_WORLD_Y_LOC-1 )
	{
		if( placement_range_x1==0 )		// if it is on the left side
		{
			placement_range_x1 = placement_range_x2;		// now we shift it to the right side
			placement_range_x2 = MAX_WORLD_X_LOC-1;
		}
		else		// if it is on the right side
		{
			placement_range_x2 -= placement_range_x1;	// now we shift it to the left side
			placement_range_x1 =  0;
		}
	}
	else		//----- if it is vertically split -------//
	{
		if( placement_range_y1==0 )		// if it is on the top side
		{
			placement_range_y1 = placement_range_y2;		// now we shift it to the bottom side
			placement_range_y2 = MAX_WORLD_Y_LOC-1;
		}
		else		// if it is on the right side
		{
			placement_range_y2 -= placement_range_y1;	// now we shift it to the top side
			placement_range_y1 =  0;
		}
	}
}
//--------- End of function Battle::inverse_placement_range ---------//


//-------- Begin of function Battle::run_loaded --------//
//
void Battle::run_loaded()
{ 
	//--- give the control to the system main loop, start the game now ---//

	game.main_loop_all(1);		// 1-is a loaded game
}
//--------- End of function Battle::run_loaded ---------//


//-------- Begin of function Battle::create_random_item --------//
//
// Create random items on the map for the player to pick up.
//
// Only available when Blackened Map is on
//
void Battle::create_random_item()
{
	// ##### begin Gilbert 9/2 #######//
//	if( !config.blacken_map )
	if( config.explore_whole_map )
	// ##### end Gilbert 9/2 #######//
		return;

	int addCount = 3 + m.random(4);		// 3 to 6 free items on the map
	int maxTries = (placement_range_x2-placement_range_x1) * (placement_range_y2-placement_range_y1);

	for( int i=0 ; i<addCount ; i++ )
	{
		//------ locate space for the monster lair ------//

		int xLoc = placement_range_x1;
		int yLoc = placement_range_y1;

		if( !world.locate_space_random( xLoc, yLoc, placement_range_x2, placement_range_y2,
										 3, 3, maxTries) )		// locate a space of 3x3 and add the item on the center
		{
			break;
		}

		// ##### begin Gilbert 24/12 #######//
//		int itemId = item_res.random_item_id(MIN_RARITY, MIN_RARITY+1);
//		if( !itemId )
//			break;
//		site_array.add_site( xLoc+1, yLoc+1, SITE_ITEM, itemId );

		Item randomItem;
		randomItem.init_random(MIN_RARITY, MIN_RARITY+1);

		if( !randomItem.id )
			break;

		site_array.add_site( xLoc+1, yLoc+1, SITE_ITEM, randomItem.id, randomItem.para );
		// ##### end Gilbert 24/12 #######//

	}
}
//--------- End of function Battle::create_random_item ---------//

