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

//Filename   : OCAMPGN4.CPP
//Description: Campaign functions

#include <ocampgn.h>
#include <oinfo.h>
#include <osys.h>
#include <ostr.h>
#include <ounit.h>
#include <oun_grp.h>
#include <otown.h>
#include <onews.h>
#include <ofirmall.h>
#include <osite.h>
#include <onation.h>
#include <onation2.h>
#include <otech.h>
#include <otechres.h>
#include <oraceres.h>
#include <ogame.h>
#include <oconfig.h>
#include <odate.h>
#include <omouse.h>
#include <omonsres.h>
#include <ou_wagon.h>
#include <ostate.h>
#include <ovga.h>
#include <ohelp.h>
#include <oworldmt.h>

//------ define static vars ------//

static int create_unit_x_loc, create_unit_y_loc;

//---- Begin of function Campaign::init_player_resource ----//
//
// Initialize the startup resource of the player.
//
// [int] resourceLevel - resource level, from 0 to 100.
//								 (default: 50)
//
// If cur_situation is defined, it will use the settings in cur_situation.
//
void Campaign::init_player_resource(int resourceLevel)
{
	//------- set player nation vars -------//

	Nation* nationPtr = nation_array[PLAYER_NATION_RECNO];

	int t = MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1;
	int cashLevel, foodLevel, reputationLevel;

	if( has_situation )
	{
		cashLevel = cur_situation.cash_level;
		foodLevel = cur_situation.food_level;
		reputationLevel = cur_situation.reputation_level;
	}
	else
	{
		cashLevel = resourceLevel;
		foodLevel = resourceLevel;
		reputationLevel = resourceLevel;
	}

	//-----------------------------------------//

	int resourceAmt = (20+cashLevel) * 50;
	nationPtr->cash = float( resourceAmt * t + misc.random(resourceAmt) );

	resourceAmt = foodLevel * 30;
	nationPtr->food = float( resourceAmt * t + misc.random(resourceAmt) );

	resourceAmt = reputationLevel / 5;
	nationPtr->reputation = float( resourceAmt * t + misc.random(resourceAmt) );

	town_array.set_all_loyalty_to_target();	// set the loyalty of all towns to their targets.
}
//---- End of function Campaign::init_player_resource ----//


//---- Begin of function Campaign::init_nation_resource ----//
//
// Initialize the resource of an enemy kingdom.
//
// <int> nationRecno   - nation recno of the enemy kingdom.
// [int] resourceLevel - resource level, from 0 to 100.
//								 (default: 50)
//
// If cur_situation is defined, it will use the settings in cur_situation.
//
void Campaign::init_nation_resource(int nationRecno, int resourceLevel)
{
	//------- set player nation vars -------//

	Nation* nationPtr = nation_array[nationRecno];

	int t;

	//--- if this nation is the player nation or a nation in alliance with the player, then the higher the difficulty level, the less resource the nation will get ---//

	if( nationRecno == PLAYER_NATION_RECNO ||
		 (~nation_array)->get_relation_status(nationRecno) == RELATION_ALLIANCE )
	{
		t = MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1;
	}
	else //--- if this nation is an enemy, the higher the difficulty level, the more resource it will get ---//
	{
		t = campaign_difficulty-1;
	}

	int cashLevel  = resourceLevel;
	int foodLevel  = resourceLevel;
	int livePoints = resourceLevel;
	int reputationLevel = resourceLevel;

	//-----------------------------------------//

	int resourceAmt = cashLevel * 70;
	nationPtr->cash = float( resourceAmt * t + misc.random(resourceAmt) );

	resourceAmt = foodLevel * 35;
	nationPtr->food = float( resourceAmt * t + misc.random(resourceAmt) );

	if( nationPtr->is_monster() )
	{
		resourceAmt = livePoints * 35;
		nationPtr->live_points = float( resourceAmt * t + misc.random(resourceAmt) );
	}
	else
	{
		resourceAmt = reputationLevel / 5;
		nationPtr->reputation = float( resourceAmt * t + misc.random(resourceAmt) );
	}

	town_array.set_all_loyalty_to_target();	// set the loyalty of all towns to their targets.
}
//---- End of function Campaign::init_nation_resource ----//

/*
//------- Begin of function Campaign::random_init_human_tech -------//
//
void Campaign::random_init_human_tech(int nationRecno)
{
	enum { MAX_INIT_TECH = 5 };

	char shouldBuildArray[MAX_INIT_TECH];

	if( has_situation )
	{
		make_should_build_array( shouldBuildArray, MAX_INIT_TECH, cur_situation.tech_level );
	}
	else
	{
		memset( shouldBuildArray, 1, sizeof(shouldBuildArray) );
	}

	if( shouldBuildArray[0] )
		tech_res.tech_class(TECH_CLASS_PRODUCTION)->random_init(nationRecno, 0, 2);

	if( shouldBuildArray[1] )
		tech_res.tech_class(TECH_CLASS_SPYING)->random_init(nationRecno, 0, 2);

	if( shouldBuildArray[2] )
		tech_res.tech_class(TECH_CLASS_INFANTRY)->random_init(nationRecno, 0, 2);

	if( shouldBuildArray[3] )
	{
		tech_res.tech_class(TECH_CLASS_SPU_STRUCTURE)->random_init(nationRecno, 1, 1);
		tech_res.tech_class(TECH_CLASS_SPU_BERSERKER)->random_init(nationRecno, 0, 1);
	}

	if( shouldBuildArray[4] )
		tech_res.tech_class(TECH_CLASS_WEAPON)->random_init(nationRecno, 1, 3);
}
//------- End of function Campaign::random_init_human_tech --------//
*/

//---- Begin of function Campaign::king_oversee_town_recno ----//
//
// Return the town currently overseen by the king.
//
int Campaign::king_oversee_town_recno(int nationRecno)
{
	int kingRecno = nation_array[nationRecno]->king_unit_recno;

	//-- if the king is not added (This can be due to insufficient space for building forts on the map --//

	if( !kingRecno )		// add a unit as the king
	{
		int xLoc, yLoc;

		//--- locate a town of the nation ----//

		Town* townPtr;

		int i;
		for( i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) )
				continue;

			townPtr = town_array[i];

			if( townPtr->nation_recno == nationRecno )
				break;
		}

		if( i>0 )
		{
			xLoc = townPtr->loc_x1;
			yLoc = townPtr->loc_y1;
		}
		else
		{
			if( !world.locate_space_random(xLoc, yLoc, MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1,
				 1, 1, MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC) )
			{
				return 0;
			}
		}

		//---- create the king unit ---------//

		int raceId = nation_array[nationRecno]->race_id;
		int unitId;

		if( raceId > 0 )
			unitId = race_res[raceId]->infantry_unit_id;
		else
			unitId = monster_res[-raceId]->unit_id;

		kingRecno = create_unit(nationRecno, unitId, RANK_KING, xLoc, yLoc, xLoc, yLoc );

		if( !kingRecno )
	      	return 0;

		//------ since the king is not in a fort, return a town belonging to the kingdom randomly ----//
			
		for( i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) )
				continue;

			if( town_array[i]->nation_recno == nationRecno )
				return i;
		}
	}

	//-------------------------------------------//

	Unit* unitKing = unit_array[kingRecno];

	err_when( unitKing->unit_mode != UNIT_MODE_OVERSEE );

	Firm* firmPtr = firm_array[unitKing->unit_mode_para];

	for( int i=0 ; i<firmPtr->linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ firmPtr->linked_town_array[i] ];

		if( townPtr->nation_recno == firmPtr->nation_recno )
			return townPtr->town_recno;
	}

	return 0;
}
//---- End of function Campaign::king_oversee_town_recno ----//


//---- Begin of function Campaign::create_work_firm ----//
//
// Return the town currently overseen by the king.
//
// <Place*> placePtr - the place where the new firm should be built next to.
//
int Campaign::create_work_firm(Place* placePtr, int firmId)
{
	Nation* nationPtr = nation_array[ placePtr->nation_recno ];

	//------- create war factory ----------//

	int firmRecno = create_firm_next_to_place( placePtr, firmId, nationPtr->race_id );

	if( !firmRecno )
		return 0;

	//------- set war factory parameters --------//

	FirmWork* firmWork = firm_array[firmRecno]->cast_to_FirmWork();

	firmWork->set_needed_worker_count( 10 + misc.random(11), COMMAND_AUTO);

	for( int i=0 ; i<firmWork->needed_worker_count ; i++ )
		firmWork->recruit_worker();

	return firmWork->firm_recno;
}
//---- End of function Campaign::create_work_firm ----//


//---- Begin of function Campaign::create_nation_town ----//
//
// <int> nationRecno    - nation of the town
// <int> xLoc, yLoc		- location of the town
// <int> buildFort		- whether build a fort or not.
// [int] sameRaceTown   - whether the nationality of the town
//								  should be the same as the nation.
//								  (default: 1)
// [int] soldierAddCount - number of soldiers to be added to the fort.
// [int] combatLevel     - the average combat level of the soldiers in the fort.
//
int Campaign::create_nation_town(int nationRecno, int xLoc, int yLoc,
											int buildFort, int sameRaceTown, int soldierAddCount, int combatLevel)
{
	if( !world.can_build_town(xLoc, yLoc) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	//---------- create the town ----------//

	int raceId = nationPtr->race_id;

	if( !sameRaceTown && misc.random(2)==0 )
		raceId = misc.random(MAX_RACE)+1;

	int townRecno = town_array.generate_town(nationPtr->nation_recno, raceId, xLoc, yLoc);

	Town* townPtr = town_array[townRecno];

	//------------------------------------//

	int initPop, initLoyalty;

	if( has_situation )
	{
		initLoyalty = cur_situation.town_loyalty_level + misc.random(20);
		initLoyalty = MIN(100, initLoyalty);

		initPop = MAX_TOWN_POPULATION * cur_situation.population_level / 100 + misc.random(20) - 10;

		initPop = MAX( initPop, 10 );
		initPop = MIN( initPop, MAX_TOWN_POPULATION );
	}
	else
	{
		initLoyalty = 60 + misc.random(30);
		initPop 	   = 30+misc.random(30);
	}

	//------------------------------------//

	townPtr->init_pop( initPop, initLoyalty, 0, 1 );	// last 1-first initialization at the beginning of the game

	townPtr->auto_set_layout();
	townPtr->bonus_loyalty = 30;

	//--------- create a fort next to the town --------//

	if( !buildFort )
		return 1;

	int firmRecno = create_firm_next_to_place( townPtr, FIRM_FORT, nationPtr->race_id );

	if( !firmRecno )
		return 1;

	FirmFort* firmFort = firm_array[firmRecno]->cast_to_FirmFort();

	//--------- create a commander for the fort ----------//

	int unitId = race_res[nationPtr->race_id]->infantry_unit_id;

	int unitRecno = create_unit(nationPtr->nation_recno,
		unitId, nationPtr->king_unit_recno==0 ? RANK_KING : RANK_GENERAL,
		firmFort->loc_x1, firmFort->loc_y1, firmFort->loc_x2, firmFort->loc_y2 );

	if( !unitRecno )		// return 1 because a town has already be set up
		return 1;

	if( has_situation )
	{
		int newLoyalty = cur_situation.unit_loyalty_level + misc.random(30);
		int maxLoyalty = 90 + misc.random(10);		// don't set to 100 too often
		unit_array[unitRecno]->loyalty = MIN( newLoyalty, maxLoyalty );
	}

	if( !nationPtr->king_unit_recno )
		set_king(nationRecno, unitRecno, 1);		// 1-this is the first king of the nation

	firm_array[firmRecno]->cast_to_FirmCamp()->assign_overseer(unitRecno);	// assign the king as the overseer of the command base

	//------ create soldiers in the fort ------//

	if(!soldierAddCount)
		return 1;

	int i;
	for( i=0 ; i<soldierAddCount ; i++ )
		firmFort->recruit_soldier(townRecno, false, COMMAND_AUTO, 1);		// 1-create at no cost

	Soldier* soldierPtr = firmFort->soldier_array;

	for( i=0 ; i<firmFort->soldier_count ; i++, soldierPtr++ )
	{
		int thisCombatLevel;

		if( combatLevel )		// if a combat level is specified for setting up this fort.
		{
			thisCombatLevel = combatLevel + misc.random(20) - 10;

			thisCombatLevel = MIN(thisCombatLevel, 100);
			thisCombatLevel = MAX(thisCombatLevel, 0);
		}
		else
		{
			thisCombatLevel = 30 + misc.random(30);
		}

		soldierPtr->skill.set_combat_level( thisCombatLevel );
		soldierPtr->skill.set_skill_level( 20 + misc.random(10) );
		soldierPtr->hit_points = soldierPtr->max_hit_points();
		soldierPtr->complete_training();

		if( has_situation )
		{
			int newLoyalty = cur_situation.unit_loyalty_level + misc.random(20);
			soldierPtr->loyalty = MIN( newLoyalty, 100 );
		}
	}

	return 1;
}
//---- End of function Campaign::create_nation_town ----//


//---- Begin of function Campaign::create_independent_town ----//

int Campaign::create_independent_town(int xLoc, int yLoc, int raceId, int initPop, int initResistance)
{
	if( !world.can_build_town(xLoc, yLoc) )
		return 0;

	//---------- create the town ----------//

	int townRecno = town_array.generate_town(0, raceId, xLoc, yLoc);

	Town* townPtr = town_array[townRecno];

	townPtr->init_pop( initPop, initResistance, 0, 1 );	// last 1-first initialization at the beginning of the game

	townPtr->auto_set_layout();

	return townRecno;
}
//---- End of function Campaign::create_independent_town ----//


//---- Begin of function Campaign::create_monster_lair ----//
//
// Create the Fryhtan nation (attacker) and its objects.
//
// <int> xLoc, yLoc 	   - the location of the lair.
// <int> nationRecno    - nation recno of the lair
// <int> soldierCount   - the number of soldiers in the lairs.
// <int> avgCombatLevel - the average combat level of the soldiers.
// [int] monsterId 		- monster id. of the lair
//								  (if not given, the race of the nation is used.)
//
// return: <int> >0  - recno of the lair created. 
//					  ==0 - no lair is created.
//
int Campaign::create_monster_lair(int xLoc, int yLoc, int nationRecno, int soldierCount, int avgCombatLevel, int monsterId)
{
	if( !world.can_build_firm(xLoc, yLoc, FIRM_LAIR) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];		// the monster nation

	//---------- create the lair ----------//

	int raceId;

	if( monsterId )
		raceId = -monsterId;
	else
		raceId = nationPtr->race_id;

	int firmRecno = create_firm(xLoc, yLoc, nationRecno, FIRM_LAIR, raceId );

	FirmLair* firmLair = firm_array[firmRecno]->cast_to_FirmLair();

	//--------- create the Fryhtans king ---------//

	int unitId = monster_res[firmLair->monster_id()]->unit_id;

	int unitRecno = create_unit(nationRecno,
		unitId, nationPtr->king_unit_recno==0 ? RANK_KING : RANK_GENERAL,
		firmLair->loc_x1, firmLair->loc_y1, firmLair->loc_x2, firmLair->loc_y2 );

	if( !unitRecno )
		return 0;

	if( has_situation )
	{
		int newLoyalty = cur_situation.unit_loyalty_level + misc.random(30);
		unit_array[unitRecno]->loyalty = MIN( newLoyalty, 100 );
	}

	if( !nationPtr->king_unit_recno )
		nationPtr->set_king(unitRecno, 1);		// 1-this is the first king of the nation

	firmLair->assign_overseer(unitRecno);	// assign the king as the overseer of the command base

	//--- adjust soldier count for different abilities in Fryhtan Species ---//

	UnitInfo* unitInfo = unit_res[unitId];

	int const stdMonsterHitPoint = 300;

	soldierCount = soldierCount * stdMonsterHitPoint / unitInfo->hit_points;

	soldierCount = MIN(soldierCount, MAX_SOLDIER);
	soldierCount = MAX(soldierCount, 1);

	//------ create Fryhtan soldiers in the fort ------//

	int i;
	for( i=0 ; i<soldierCount ; i++ )
		firmLair->recruit_soldier(COMMAND_AUTO, 1);		// 1-no cost

	Soldier* soldierPtr = firmLair->soldier_array;

	for( i=0 ; i<firmLair->soldier_count ; i++, soldierPtr++ )
	{
		int combatLevel = avgCombatLevel + misc.random(30) - 15;

		combatLevel = MIN( combatLevel, 100 );
		combatLevel = MAX( combatLevel, 20  );

		int skillLevel = avgCombatLevel + misc.random(20) - 10;

		skillLevel = MIN( skillLevel, 100 );
		skillLevel = MAX( skillLevel, 20  );

		soldierPtr->skill.set_combat_level( combatLevel );
		soldierPtr->skill.set_skill_level( skillLevel );
		soldierPtr->hit_points = soldierPtr->max_hit_points();
		soldierPtr->complete_training();

		if( has_situation )
		{
			int newLoyalty = cur_situation.unit_loyalty_level + misc.random(20);
			soldierPtr->loyalty = MIN( newLoyalty, 100 );
		}
	}

	return firmRecno;
}
//---- End of function Campaign::create_monster_lair ----//


//---- Begin of function Campaign::create_slave_town ----//
//
// Create a slave town and link it to the given Fryhtan Lair.
//
// <int> firmRecno - recno of the Fryhtan Lair
// <int> raceId 	 - race id. of the slave town.
//
// return: recno of the created town.
//
int Campaign::create_slave_town(int firmRecno, int raceId)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( !firmPtr->nation_recno );

	//-------- create a town ---------//

	int initPop = 30 + misc.random(30);

	int townRecno = create_town_next_to_place( firmPtr, 0, raceId, initPop );

	if( !townRecno )
		return 0;

	//------ make it a slave town ------//

	Town* townPtr = town_array[townRecno];

	townPtr->resistance_array[ firmPtr->nation_recno-1 ] = 0;

	return townRecno;
}
//---- End of function Campaign::create_slave_town ----//


//---- Begin of function Campaign::create_economic_firm ----//
//
// Create economic firms next to the given town.
//
// <int> townRecno - the town recno.
//
int Campaign::create_economic_firm(int townRecno)
{
	int 	  i;
	Town*   townPtr = town_array[townRecno];
	Nation* nationPtr = nation_array[townPtr->nation_recno];

	//------- build mine -------//

	int siteRecno = site_array.create_raw_site(townPtr->region_id, townRecno);

	if( !siteRecno )
		return 0;

	Site* sitePtr = site_array[siteRecno];

	int firmRecno = create_firm(sitePtr->map_x_loc, sitePtr->map_y_loc, nationPtr->nation_recno, FIRM_MINE, nationPtr->race_id );

	//------- set mine parameters --------//

	FirmMine* firmMine = firm_array[firmRecno]->cast_to_FirmMine();

	firmMine->set_needed_worker_count( 6 + misc.random(6), COMMAND_AUTO);

	for( i=0 ; i<firmMine->needed_worker_count ; i++ )
		firmMine->recruit_worker();

	firmMine->stock_qty   =  float( 100 + misc.random(400) );

	if( has_situation )
		firmMine->reserve_qty = firmMine->reserve_qty * cur_situation.mine_raw_level / 100;
	else
		firmMine->reserve_qty -= firmMine->stock_qty;

	//-------- build factory next to the mine ---------//

	firmRecno = create_work_firm( firmMine, FIRM_FACTORY );

	if( !firmRecno )
		return 0;

	FirmFactory* firmFactory = firm_array[firmRecno]->cast_to_FirmFactory();

	firmFactory->stock_qty = float( 100 + misc.random(400) );

	//------- build market ----------//

	create_firm_next_to_place( firmFactory, FIRM_MARKET, nationPtr->race_id );

	if( !firmRecno )
		return 0;

	return 1;
}
//---- End of function Campaign::create_economic_firm ----//


//---- Begin of function Campaign::make_should_build_array ----//
//
// <char*> arrayPtr   - the pointer to a pre-defined array
// <int>   arraySize  - size of the array
// <int>   buildLevel - the build level, from 0 to 100.
//
void Campaign::make_should_build_array(char* arrayPtr, int arraySize, int buildLevel)
{
	memset( arrayPtr, 0, arraySize );

	int buildCount = arraySize * buildLevel / 100;
	int loopCount=0;

	for( int i=0 ; i<buildCount ; )
	{
		int j=misc.random(arraySize);

		if( !arrayPtr[j] )
		{
			arrayPtr[j] = 1;
			i++;
		}

		err_when( ++loopCount > 10000 );
	}
}
//---- End of function Campaign::make_should_build_array ----//


//---- Begin of function Campaign::create_military_firm ----//
//
// Create military firms next to the given town.
//
// <int> townRecno 	- the town recno.
// [int] powerRating - the power of the military firm, a rating from 0 to 100
//							  (default: 100)
//
int Campaign::create_military_firm(int townRecno, int powerRating)
{
	err_when( powerRating > 100 );

	int 	  i, firmRecno;
	Town*   townPtr   = town_array[townRecno];
	Nation* nationPtr = nation_array[townPtr->nation_recno];

	//----- determine what firms should be built and what shouldn't ----//

	enum { MAX_BUILD_MILITARY_FIRM = 4 };

	char shouldBuildArray[MAX_BUILD_MILITARY_FIRM];

	make_should_build_array( shouldBuildArray, MAX_BUILD_MILITARY_FIRM, powerRating );

	//------- create special training structures -------//

	if( shouldBuildArray[0] &&
		 firm_res[FIRM_SPECIAL]->nation_can_build(nationPtr->nation_recno, nationPtr->race_id, 0) )		// 0-don't check cash
	{
		firmRecno = create_firm_next_to_place( townPtr, FIRM_SPECIAL, townPtr->race_id );

		if( firmRecno )
		{
			FirmSpecial* firmSpecial = firm_array[firmRecno]->cast_to_FirmSpecial();

			//---------- recruit special units ----------//

			int unitCount = MAX_TRAINEE/2 + misc.random(MAX_TRAINEE/2) + 1;

			for( i=unitCount ; i>0 ; i-- )
			{
				firmSpecial->recruit_trainee(townRecno, TRAIN_UNIT_SPECIAL, COMMAND_AUTO);
			}
		}
	}

	//------- create tower of science ----------//

	if( shouldBuildArray[1] )
	{
		create_work_firm( townPtr, FIRM_RESEARCH );
	}

	//------- create war factory ----------//

	if( shouldBuildArray[2] &&
		 tech_res.tech_class(TECH_CLASS_WEAPON)->total_tech_count(PLAYER_NATION_RECNO) > 0 )
	{
		create_work_firm( townPtr, FIRM_WAR_FACTORY );
	}

	//------- create seat of power ----------//

	if( shouldBuildArray[3] && powerRating >= 70 )
	{
		if( firm_res[FIRM_BASE]->nation_can_build(nationPtr->nation_recno, nationPtr->race_id) )
		{
			create_work_firm( townPtr, FIRM_BASE );
		}
	}

	return 1;
}
//---- End of function Campaign::create_military_firm ----//


//---- Begin of function Campaign::random_edge_location ----//
//
// Return a random location near the edge of the map.
//
// <int&> xLoc, yLoc - the reference vars for returning values.
// [int]  whichEdge  - if this is given, the location will be
//							  on the given edge only.
//							  1-Top, 2-Bottom, 3-Left, 4-Right
//
void Campaign::random_edge_location(int& xLoc, int& yLoc, int whichEdge)
{
	if( whichEdge )
	{
		switch( whichEdge )
		{
			case 1:		// top edge
				xLoc = misc.random(MAX_WORLD_X_LOC);
				yLoc = 2;
				break;

			case 2: 		// bottom edge
				xLoc = misc.random(MAX_WORLD_X_LOC);
				yLoc = MAX_WORLD_Y_LOC-2;
				break;

			case 3:		// left edge
				xLoc = 2;
				yLoc = misc.random(MAX_WORLD_Y_LOC);
				break;

			case 4: 		// right edge
				xLoc = MAX_WORLD_X_LOC-2;
				yLoc = misc.random(MAX_WORLD_Y_LOC);
				break;
		}
	}
	else
	{
		//--- determine which edge of the map the reinforcement comes from ---//

		if( misc.random(2)==0 )
		{
			xLoc = misc.random(2)==0 ? 2 : MAX_WORLD_X_LOC-3;
			yLoc = misc.random(MAX_WORLD_Y_LOC);
		}
		else
		{
			xLoc = misc.random(MAX_WORLD_X_LOC);
			yLoc = misc.random(2)==0 ? 2 : MAX_WORLD_Y_LOC-3;
		}
	}
}
//---- End of function Campaign::random_edge_location ----//


//---- Begin of function Campaign::most_state_nation ----//
//
// Return the nation with the most states.
//
// <int> raceType - 1 - any human nation
//						  0 - any nation
//						 -1 - any monster nation
//
// [int] nextToCampaignNation - if this is given, the selected nation must
//										  be geographically next to nextToNation.
//
// return: the campaign nation recno of the nation with the most states.
//
int Campaign::most_state_nation(int raceType, int nextToCampaignNation)
{
	int maxState=0, bestNationRecno=0;

	for( int i=nation_count() ; i>0 ; i-- )
	{
		if( is_nation_deleted(i) )
			continue;

		if( raceType==1 && !get_nation(i)->is_human() )
			continue;

		if( raceType == -1 && !get_nation(i)->is_monster() )
			continue;

		if( nextToCampaignNation && !state_array.are_nation_adjacent(i, nextToCampaignNation) )
			continue;

		int t = state_array.nation_state_count(i);

		if( t > maxState )
		{
			maxState = t;
			bestNationRecno = i;
		}
	}

	return bestNationRecno;
}
//---- End of function Campaign::most_state_nation ----//


//---- Begin of function Campaign::least_state_nation ----//
//
// Return the nation with the most states.
//
// <int> raceType - 1 - any human nation
//						  0 - any nation
//						 -1 - any monster nation
//
// [int] nextToCampaignNation - if this is given, the selected nation must
//										  be geographically next to nextToNation.
//
// return: the campaign nation recno of the nation with the most states.
//
int Campaign::least_state_nation(int raceType, int nextToCampaignNation)
{
	int minState=1000, bestNationRecno=0;

	for( int i=nation_count() ; i>0 ; i-- )
	{
		if( is_nation_deleted(i) )
			continue;

		if( raceType==1 && !get_nation(i)->is_human() )
			continue;

		if( raceType == -1 && !get_nation(i)->is_monster() )
			continue;

		if( nextToCampaignNation && !state_array.are_nation_adjacent(i, nextToCampaignNation) )
			continue;

		int t = state_array.nation_state_count(i);

		if( t < minState )
		{
			minState = t;
			bestNationRecno = i;
		}
	}

	return bestNationRecno;
}
//---- End of function Campaign::least_state_nation ----//


//--- Begin of function Campaign::create_troop ---//
//
// <int> nationRecno 	- recno of the attacking monster nation
// <int> totalUnit		- total number of monsters to be added.
// [int] avgCombatLevel - the average combat level of the troop
// [int] unitId			- if this is given, the troop will encompass this unit types only.
// [char] useLastLocation - if this is 1, then new units will be created next to units created last time. Otherwise, a random location will be selected.
//									 (default:0)
//
void Campaign::create_troop(int nationRecno, int totalUnit, int avgCombatLevel, int unitId, char useLastLocation)
{
	if( nationRecno==PLAYER_NATION_RECNO && this_battle_use_fryhtan )
	{
		//---- create a human troop of 50% of the given size ----//

		create_troop2(nationRecno, totalUnit/2, avgCombatLevel, unitId, useLastLocation); 		// 1/2 of the total size

		//---- create a fryhtan troop of 50% of the given size ----//

		unitId = monster_res[surrendered_fryhtan_id]->unit_id;

		create_troop2(nationRecno, totalUnit/2, avgCombatLevel, unitId, useLastLocation );
	}
	else
	{
		create_troop2(nationRecno, totalUnit, avgCombatLevel, unitId, useLastLocation); 		// 1/2 of the total size
	}

	//----- create wagon for human players or allied human kingdoms ------//

	if( nationRecno==PLAYER_NATION_RECNO ||
		 ( (~nation_array)->get_relation_status(nationRecno) == RELATION_ALLIANCE &&
			nation_array[nationRecno]->is_human() ) )
	{
		create_wagon(nationRecno, 60-campaign_difficulty*10+misc.random(10), 1);		// 1-useLastLocation
	}
}
//------ End of function Campaign::create_troop ----//


//--- Begin of function Campaign::create_troop2 ---//
//
// <int> nationRecno 	- recno of the attacking monster nation
// <int> totalUnit		- total number of monsters to be added.
// [int] avgCombatLevel - the average combat level of the troop
// [int] unitId			- if this is given, the troop will encompass this unit types only.
// [char] useLastLocation - if this is 1, then new units will be created next to units created last time. Otherwise, a random location will be selected.
//									 (default:0)
//
void Campaign::create_troop2(int nationRecno, int totalUnit, int avgCombatLevel, int unitId, char useLastLocation)
{
	int xLoc, yLoc;

	if( useLastLocation )
	{
		xLoc = create_unit_x_loc;
		yLoc = create_unit_y_loc;
	}
	else
	{
		random_edge_location(xLoc, yLoc);
	}

	//------ create monsters ---------//

	int teamCount = totalUnit / MAX_TEAM_MEMBER;

	if( totalUnit % MAX_TEAM_MEMBER > 0 )
		teamCount++;

	int teamSize = totalUnit / teamCount;

	for( int i=0 ; i<teamCount ; i++ )
	{
		create_unit_group( nationRecno, teamSize+misc.random(3)-1, unitId, 1, 0, xLoc, yLoc );		// create two groups of monsters

		//--- set the location for the next team ----//

		if( xLoc==2 || xLoc==MAX_WORLD_X_LOC-3 )
		{
			yLoc += 5;
			yLoc = MIN( MAX_WORLD_Y_LOC-1, yLoc );
		}

		if( yLoc==2 || yLoc==MAX_WORLD_Y_LOC-3 )
		{
			xLoc += 5;
			xLoc = MIN( MAX_WORLD_X_LOC-1, xLoc );
		}
	}

	create_unit_x_loc = xLoc;
	create_unit_y_loc = yLoc;

	//----- if the average combat level is specified -----//

	if( avgCombatLevel )
	{
		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( unit_array.is_deleted(i) )
				continue;

			Unit* unitPtr = unit_array[i];

			if( unitPtr->nation_recno != nationRecno )
				continue;

			//----------------------------------//

			int combatLevel;

			if( unitPtr->rank_id==RANK_GENERAL )
			{
				combatLevel = avgCombatLevel + 30 + misc.random(30) - 15;
			}
			else if( unitPtr->rank_id==RANK_SOLDIER )
			{
				combatLevel = avgCombatLevel + misc.random(30) - 15;
			}
			else
				continue;

			//----------------------------------//

			combatLevel = MIN(combatLevel, 100);
			combatLevel = MAX(combatLevel, 20);

			unitPtr->set_combat_level(combatLevel);
		}
	}
}
//------ End of function Campaign::create_troop2 ----//


//--- Begin of function Campaign::create_wagon ---//
//
// <int> nationRecno 	- recno of the attacking monster nation
// <int> totalPop		- total number of people to be added.
// [char] useLastLocation - if this is 1, then new units will be created next to units created last time. Otherwise, a random location will be selected.
//									 (default:0)
//
void Campaign::create_wagon(int nationRecno, int totalPop, char useLastLocation)
{
	int xLoc, yLoc;

	if( useLastLocation )
	{
		xLoc = create_unit_x_loc;
		yLoc = create_unit_y_loc;
	}
	else
	{
		random_edge_location(xLoc, yLoc);
	}

	//----- create wagons -----//

	int wagonCount = totalPop / MAX_WAGON_POPULATION;

	if( totalPop % MAX_WAGON_POPULATION > 0 )
		wagonCount++;

	int wagonSize = totalPop / wagonCount;

	for( int i=0 ; i<wagonCount ; i++ )
	{
		int unitRecno = create_unit( nationRecno, UNIT_WAGON, RANK_SOLDIER, xLoc, yLoc, xLoc, yLoc );

		if( !unitRecno )
			break;

		UnitWagon* unitWagon = unit_array[unitRecno]->cast_to_UnitWagon();

		unitWagon->set_race( nation_array[nationRecno]->race_id );
		unitWagon->set_pop(wagonSize);

		//--- set the location for the next team ----//

		if( xLoc==2 || xLoc==MAX_WORLD_X_LOC-3 )
		{
			yLoc += 2;
			yLoc = MIN( MAX_WORLD_Y_LOC-1, yLoc );
		}

		if( yLoc==2 || yLoc==MAX_WORLD_Y_LOC-3 )
		{
			xLoc += 2;
			xLoc = MIN( MAX_WORLD_X_LOC-1, xLoc );
		}
	}

	create_unit_x_loc = xLoc;
	create_unit_y_loc = yLoc;
}
//------ End of function Campaign::create_wagon ----//


//---- Begin of function Campaign::create_unit_group ----//
//
// <int> nationRecno - create a group of units of this nation.
// <int> addCount    - number of units to be created. (the maximum is MAX_TEAM_MEMBER)
// [int] unitId 	   - id. of the unit to be created
//							  (if this is not given, the main unit id. of the nation will be used.)
// [int] adjustCount - if this is 1, then the addCount will be adjusted based on the strength
//							  of the unit. (default: 0)
// [int] whichEdge   - if this is given, the location will be
//							  on the given edge only.
//							  1-Top, 2-Bottom, 3-Left, 4-Right
// [int] xLoc, yLoc  - if this is given, the troop will be created
//							  around this location.
//
// Return: <int> the number of units created.
//
// Note: when a group is created successfully, the units will be added
//			to unit_group.
//
int Campaign::create_unit_group(int nationRecno, int addCount, int unitId,
										  int adjustCount, int whichEdge, int xLoc, int yLoc)
{
	addCount = MIN( addCount, MAX_TEAM_MEMBER );

	//-------- determine the unit type to be created --------//

	Nation* nationPtr = nation_array[nationRecno];

	if( !unitId )
	{
		if( nationPtr->is_human() )
			unitId = race_res[nationPtr->race_id]->infantry_unit_id;
		else
			unitId = monster_res[nationPtr->monster_id()]->unit_id;
	}

	//---- if the unit is a monster, adjust addCount based on the strength of the unit, more powerful units will have fewer units ----//

	if( adjustCount )
	{
		UnitInfo* unitInfo = unit_res[unitId];

		int const stdMonsterHitPoint = 300;

		if( unitInfo->is_monster() )
			addCount = addCount * stdMonsterHitPoint / unitInfo->hit_points;

		addCount = MIN(addCount, MAX_TEAM_MEMBER);
		addCount = MAX(addCount, 1);
	}

	//--- determine which edge of the map the reinforcement comes from ---//

	if( xLoc < 0 )
		random_edge_location(xLoc, yLoc, whichEdge);

	//---------- create units now ----------//

	unit_group.zap();

	for( int i=0 ; i<addCount ; i++ )
	{
		int rankId;

		if( (i==0 || nationPtr->king_unit_recno==0) && unit_res[unitId]->race_id )		// don't set it to General for weapons and other non-human units.
		{
			if( nationPtr->king_unit_recno==0 )
				rankId = RANK_KING;
			else
				rankId = RANK_GENERAL;
		}
		else
		{
			rankId = RANK_SOLDIER;
		}

		//----------------------------------------------//

		int unitRecno = create_unit( nationRecno, unitId, rankId, xLoc, yLoc, xLoc, yLoc );

		if( !unitRecno )
			break;

		if( !nationPtr->king_unit_recno )
			set_king(nationRecno, unitRecno, 1);		// 1-this is the first king of the nation

		unit_group.add_unit(unitRecno);
	}

	if( unit_res[unitId]->race_id )		// only form a team when there is a human leader in the team
		unit_group.form_team();		// the unit group forms a team, with the highest ranked unit being the leader

	return unit_group.size();
}
//---- End of function Campaign::create_unit_group ----//


//---- Begin of function Campaign::capture_linked_place ----//
//
// Capture all towns linked to the given firm.
// Firms linked to the newly captured towns are also captured.
//
// <int> mainFirmRecno - the firm recno.
//
void Campaign::capture_linked_place(int mainFirmRecno)
{
	Firm* mainFirm = firm_array[mainFirmRecno];

	for( int i=0 ; i<mainFirm->linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ mainFirm->linked_town_array[i] ];

		townPtr->set_nation(mainFirm->nation_recno);

		townPtr->bonus_loyalty = 30; 		// set a bonus loyalty towards the new nation so the town won't rebel too soon.

		//---- firms linked to the town are also captured ----//

		for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* firmPtr = firm_array[ townPtr->linked_firm_array[j] ];

			if( firmPtr->cast_to_FirmCamp() )		// don't capture camps
				continue;

			firmPtr->change_nation(mainFirm->nation_recno);
		}
	}
}
//---- End of function Campaign::capture_linked_place ----//


//------- Begin of function Campaign::set_nation_hostile -------//
//
// Set the given two nations hostile to each other.
//
void Campaign::set_nation_hostile(int nationRecno1, int nationRecno2)
{
	Nation* nationPtr1 = nation_array[nationRecno1];
	Nation* nationPtr2 = nation_array[nationRecno2];

	nationPtr1->set_relation_should_attack(nationRecno2, 1, COMMAND_AUTO);
	nationPtr2->set_relation_should_attack(nationRecno1, 1, COMMAND_AUTO);

	nationPtr1->set_relation_status(nationRecno2, RELATION_HOSTILE);

	nationPtr2->change_ai_relation_level(nationRecno1, -100);

	//---- the enemy should never cease fire ----//

	if( nationPtr1->nation_type == NATION_AI )
		nationPtr1->get_relation(nationRecno2)->ai_never_consider[TALK_REQUEST_CEASE_WAR-1] = 1;

	if( nationPtr2->nation_type == NATION_AI )
		nationPtr2->get_relation(nationRecno1)->ai_never_consider[TALK_REQUEST_CEASE_WAR-1] = 1;
}
//------- End of function Campaign::set_nation_hostile --------//


//------- Begin of function Campaign::set_nation_allied -------//
//
// Set the given two nations allied to each other.
//
void Campaign::set_nation_allied(int nationRecno1, int nationRecno2)
{
	Nation* nationPtr1 = nation_array[nationRecno1];
	Nation* nationPtr2 = nation_array[nationRecno2];

	nationPtr1->set_relation_should_attack(nationRecno2, 0, COMMAND_AUTO);
	nationPtr2->set_relation_should_attack(nationRecno1, 0, COMMAND_AUTO);

	nationPtr1->form_alliance_treaty(nationRecno2);

	nationPtr2->change_ai_relation_level(nationRecno1, 100);

   nationPtr1->get_relation(nationRecno2)->has_contact = 1;
	nationPtr2->get_relation(nationRecno1)->has_contact = 1;

	//---- the enemy should declare war or terminate alliance treaty ----//

	static short neverConsiderArray[] =
	{
		TALK_END_ALLIANCE_TREATY,
		TALK_DECLARE_WAR,
		TALK_REQUEST_SURRENDER,
		TALK_SURRENDER,
		0
	};

	if( nationPtr1->nation_type == NATION_AI )
		nationPtr1->get_relation(nationRecno2)->set_ai_never_consider(neverConsiderArray);

	if( nationPtr2->nation_type == NATION_AI )
		nationPtr2->get_relation(nationRecno1)->set_ai_never_consider(neverConsiderArray);
}
//------- End of function Campaign::set_nation_allied --------//


//------- Begin of function Campaign::should_general_defect -------//
//
// <int> intervalDays - the number of days before the next defection happens.
//
int Campaign::should_general_defect(int intervalDays)
{
	if( general_defect_count==0 )
		return 0;

	if( nation_array.ai_nation_count == MAX_NATION )		// we cannot add new nations
		return 0;

	if( info.game_date < next_general_defect_date )
		return 0;

	general_defect_count--;
	next_general_defect_date = info.game_date + intervalDays + misc.random(intervalDays*2);
	next_rebel_date = info.game_date + intervalDays + misc.random(intervalDays*2);

	return 1;
}
//------- End of function Campaign::should_general_defect --------//


//------- Begin of function Campaign::should_rebel -------//
//
// <int> intervalDays - the number of days before the next rebel happens.
//
int Campaign::should_rebel(int intervalDays)
{
	if( rebel_count==0 )
		return 0;

	if( info.game_date < next_rebel_date )
		return 0;

	rebel_count--;

	next_general_defect_date += misc.random(intervalDays);
	next_rebel_date = info.game_date + intervalDays + misc.random(intervalDays*2);

	return 1;
}
//------- End of function Campaign::should_rebel --------//


//------ Begin of function Campaign::general_defect_form_new_nation ------//
//
// <int> nationRecno - generals of this nation will defect and form new nations.
//
// return: <int> the recno of the new nation.
//
int Campaign::general_defect_form_new_nation(int nationRecno)
{
	//---- determine which general is going to defect ----//

	Nation* nationPtr = nation_array[nationRecno];
	Unit*   unitPtr;
	int 	  generalSeq = misc.random(nationPtr->ai_general_count);

	int i;
	for( i=0 ; i<nationPtr->ai_general_count ; i++ )
	{
		if( ++generalSeq >=nationPtr->ai_general_count )
			generalSeq=0;

		unitPtr = unit_array[ nationPtr->ai_general_array[generalSeq] ];

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE &&
			 unitPtr->is_human() &&
			 unitPtr->rank_id != RANK_KING )
		{
			break;
		}
	}

	if( i==nationPtr->ai_general_count )
		return 0;

	//---------- add news ----------//

	news_array.general_form_new_nation(unitPtr->sprite_recno);
	news_array.disable();		// suppress all other news

	//-------- the general defects now ---------//

	int newNationRecno = nation_array.new_nation( NATION_HUMAN, NATION_AI, unitPtr->race_id, nation_array.random_unused_color() );

	unitPtr->betray(newNationRecno);

	Nation* newNation = nation_array[newNationRecno];

	newNation->set_king(unitPtr->sprite_recno, 1);

	newNation->pref_surrender = 0;			// they will never surrender

	//----- towns linked to the camp are also captured ----//

	capture_linked_place( unitPtr->unit_mode_para);

	//------ set vars so that the new nation will not start sending diplomatic to the player nation (the nation it defected from) too soon ---//

	NationRelation* nationRelation = newNation->get_relation(nationRecno);

	for( i=0 ; i<MAX_TALK_TYPE ; i++ )
	{
		nationRelation->never_accept_until_date_array[i] = info.game_date + misc.random(90) + 90;
	}

	news_array.enable();

	//---- the new nation should be hostile with the human player's nation ----//

	set_nation_hostile(nationRecno, newNationRecno);

	return newNationRecno;
}
//------ End of function Campaign::general_defect_form_new_nation -------//


//------ Begin of function Campaign::general_defect_to_other_nation ------//
//
// <int> fromNationRecno - randomly select a general of this nation
// <int> toNationRecno	 - change the general to this nation.
//
int Campaign::general_defect_to_other_nation(int fromNationRecno, int toNationRecno)
{
	//---- determine which general is going to defect ----//

	Nation* nationPtr = nation_array[fromNationRecno];
	Unit*   unitPtr;
	int 	  generalSeq = misc.random(nationPtr->ai_general_count);

	for( int i=0 ; i<nationPtr->ai_general_count ; i++ )
	{
		if( ++generalSeq >= nationPtr->ai_general_count )
			generalSeq=0;

		unitPtr = unit_array[ nationPtr->ai_general_array[generalSeq] ];

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE &&
			 unitPtr->is_human() &&
			 unitPtr->rank_id != RANK_KING )
		{
			return unitPtr->betray(toNationRecno);
		}
	}

	return 0;
}
//------ End of function Campaign::general_defect_to_other_nation -------//


//------- Begin of function Campaign::town_defect -------//
//
// When this function is called, a town of the given nation will defect.
//
// <int> nationRecno - towns of this nation will defect
//
// [int] defectToNationRecno - if this is given, the town will be defected
//									    to this nation.
//
int Campaign::town_defect(int nationRecno, int defectToNationRecno)
{
	//---- determine which town is going to rebel ----//

	Nation* nationPtr = nation_array[nationRecno];
	Town* townPtr, *bestTown=NULL;
	int townSeq = misc.random(nationPtr->ai_town_count);

	for( int loopCount=1 ; loopCount<=2 && !bestTown ; loopCount++ )
	{
		for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
		{
			if( ++townSeq >=nationPtr->ai_town_count )
				townSeq=0;

			townPtr = town_array[ nationPtr->ai_town_array[townSeq] ];

			if( townPtr->population==0 )
				continue;

			//--- only towns that do not have linked own camps ---//

			if( !townPtr->has_linked_own_camp || loopCount==2 )	// in the 2nd loop, ignore checking linked camps
			{
				bestTown = townPtr;
				break;
			}
		}
	}

	if( !bestTown )
		return 0;

	//-------- change the town to independent now ----//

	if( defectToNationRecno==0 && misc.random(2)==0 )
	{
		//----- randomly pick a nation to defect to ------//

		int nationRecno = misc.random(nation_array.size())+1;

		for( int i=nation_array.size() ; i>0 ; i-- )
		{
			if( ++nationRecno > nation_array.size() )
				nationRecno = 1;

			if( !nation_array.is_deleted(nationRecno) &&
				 nationRecno != nationRecno &&
				 nation_array[nationRecno]->is_human() )		// only defect to humans
			{
				defectToNationRecno = nationRecno;
				break;
			}
		}
	}

	if( defectToNationRecno )							// no suitable nation found
		townPtr->surrender(defectToNationRecno);
	else
		townPtr->surrender(0);

	return townPtr->town_recno;
}
//------- End of function Campaign::town_defect --------//


//--- Begin of function Campaign::monster_reinforcement ---//
//
// Summon monster reinforcement.
//
// <int>  nationRecno - the game nation recno of the monster nation
//							   to have reinforcement.
//
// [int]  whichEdge  - if this is given, the location will be
//							  on the given edge only.
//							  1-Top, 2-Bottom, 3-Left, 4-Right
//
void Campaign::monster_reinforcement(int nationRecno, int whichEdge)
{
	if( monster_reinforcement_count==0 ||
		 info.game_date < next_monster_reinforcement_date )
	{
		return;
	}

	int createCount = 3 + campaign_difficulty*3 + misc.random(5);

	createCount = MIN(createCount, monster_reinforcement_count);

	int createdCount = create_unit_group( nationRecno, createCount, 0, 0, whichEdge );

	monster_reinforcement_count -= createdCount;

	if( monster_reinforcement_count < 0 )
		monster_reinforcement_count = 0;

	next_monster_reinforcement_date = info.game_date +
		(MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1) * 60;
}
//------ End of function Campaign::monster_reinforcement ----//


//--- Begin of function Campaign::monster_reinforcement_to_lair ---//
//
// Summon monster reinforcement to lairs. Reinforcements will only
// come when there are lairs that are at least half empty.
//
// [int]  whichEdge  - if this is given, the location will be
//							  on the given edge only.
//							  1-Top, 2-Bottom, 3-Left, 4-Right
//
void Campaign::monster_reinforcement_to_lair(int whichEdge)
{
	if( monster_reinforcement_count==0 )
		return;

	//------- locate for a lair that is at least half empty --------//

	FirmLair* firmLair;

	int i;
	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmLair = firm_array[i]->cast_to_FirmLair();

		if( !firmLair )
			continue;

		if( firmLair->setup_date+(firmLair->firm_recno%30) == info.game_date &&
			 !firmLair->under_construction &&
			 MAX_SOLDIER - firmLair->soldier_count >= 8  )
		{
			break;
		}
	}

	if( i==0 )
		return;

	//------ create a group of unit and assign them to the firm ------//

	int createCount = MAX_SOLDIER - firmLair->soldier_count - misc.random(5);

	createCount = MIN(createCount, monster_reinforcement_count);

	int createdCount = create_unit_group( firmLair->nation_recno, createCount,
							 0, 0, whichEdge );

	monster_reinforcement_count -= createdCount;

	if( monster_reinforcement_count < 0 )
		monster_reinforcement_count = 0;

	if( unit_group.size() > 0 )
		unit_group.assign( firmLair->loc_x1, firmLair->loc_y1, UNIT_LAND, COMMAND_AUTO );

	next_monster_reinforcement_date = info.game_date +
		(MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1) * 60;
}
//------ End of function Campaign::monster_reinforcement_to_lair ----//


//--- Begin of function Campaign::create_raw_site ---//
//
void Campaign::create_raw_site()
{
	int siteCount;

	if( has_situation )
		siteCount = 5 * cur_situation.site_raw_level / 100;
	else
		siteCount = site_array.size() + 2 + misc.random(2);		// create 2 to 3 sites

	site_array.generate_raw_site(siteCount);		// 3 to 5 raw material deposits at the startup
}
//------ End of function Campaign::create_raw_site ----//


//--- Begin of function Campaign::think_enemy_attack ---//
//
// Think about enemy attacks when the nation does not have any camps.
// As the AI does not handle this situation.
//
// <int> attackNationRecno - recno of the attacking monster nation
// <int> targetNationRecno	- recno of the target nation
// <int> attackFrequency   - attack frequency, 0-lowest, 100-highest
//									  (default: 50)
//
void Campaign::think_enemy_attack(int attackNationRecno, int targetNationRecno, int attackFrequency)
{
	static short thinkIntervalArray[] = { 150, 80, 30, 10, 5 };

	int thinkInterval = thinkIntervalArray[campaign_difficulty-1] * (110-attackFrequency) / 50;

	if( (info.game_date+1-info.game_start_date) % thinkInterval > 0 )		// think about it once every 5 days. So there are possibilities for the AI to do something else in the interval.
		return;

	if( nation_array.is_deleted(attackNationRecno) )
		return;

	//---- only think attack when the nation does not have any camps ----//

	Nation* attackNation = nation_array[attackNationRecno];

	int i;
	for( i=attackNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		if( !firm_array[ attackNation->ai_camp_array[i] ]->under_construction )
			break;
	}

	if( i >= 0 )		// if i>=0, that means the monster does have some lairs
		return;

	//------- set units to unit_group and order them to attack ------//

	Unit* unitPtr;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno == attackNationRecno &&
			 unitPtr->rank_id >= RANK_GENERAL &&
			 unitPtr->is_all_stop() )
		{
			think_enemy_attack2(targetNationRecno, unitPtr);
		}
	}
}
//---- End of function Campaign::think_enemy_attack ----//


//--- Begin of function Campaign::think_enemy_attack2 ---//
//
// <int>   targetNationRecno	 - recno of the target nation
// <Unit*> unitPtr - think attack with this commander unit.
//
void Campaign::think_enemy_attack2(int targetNationRecno, Unit* unitPtr)
{
	//--- locate the closest town to attack ---//

	Town* townPtr; 
	int townRecno = misc.random(town_array.size())+1;

	int i;
	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( ++townRecno > town_array.size() )
			townRecno=1;

		if( town_array.is_deleted(townRecno) )
			continue;

		townPtr = town_array[townRecno];

		if( townPtr->nation_recno == targetNationRecno )	  //  only attack player towns
			break;
	}

	if( i==0 )
		return;

	//--- if the attacking commander unit is in a fort ---//

	if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
	{
		FirmCamp* firmCamp = firm_array[unitPtr->unit_mode_para]->cast_to_FirmCamp();

		err_when( !firmCamp );

		firmCamp->patrol();
	}

	if( !unitPtr->is_visible() )
		return;

	//------ group the units and order them to attack ------//

	unit_group.zap();

	for( int j=0 ; j<unitPtr->team_info->member_count ; j++ )
	{
		int unitRecno = unitPtr->team_info->member_unit_array[j];

		if( unit_array[unitRecno]->is_visible() )
			unit_group.add_unit(unitRecno);
	}

	if( unit_group.size() > 0 )
		unit_group.attack( townPtr->base_obj_recno, COMMAND_AUTO );
}
//---- End of function Campaign::think_enemy_attack2 ----//


//--- Begin of function Campaign::create_monster_special_firm ---//
//
// Create a monster special structure next to the given firm.
// The nation and monster type of the special firm should be
// the same as the given firm.
//
// <int> firmRecno - recno of the given firm.
//
int Campaign::create_monster_special_firm(int firmRecno)
{
	Firm* firmPtr = firm_array[firmRecno];
	int nationRecno = firmPtr->nation_recno;
	Nation* nationPtr = nation_array[nationRecno];

	for( int firmId=FIRST_MONSTER_SPECIAL_FIRM ; firmId<=LAST_MONSTER_SPECIAL_FIRM ; firmId++ )
	{
		FirmInfo* firmInfo = firm_res[firmId];

		//---- set the tech level first -------//

		int curTechLevel = firmInfo->get_nation_tech_level(nationRecno);
		int newTechLevel = MAX(1, curTechLevel);

		firmInfo->set_nation_tech_level( nationRecno, newTechLevel );

		//---- set the tech level in tech_res ----//

		int i;
		for( i=1 ; i<=tech_res.tech_count ; i++ )
		{
			if( tech_res[i]->firm_id == firmId )
			{
				tech_res[i]->set_nation_tech_level(nationRecno, newTechLevel);
				break;
			}
		}

		err_when( i>tech_res.tech_count );

		//------- build the firm now ----------//

		if( firmInfo->get_build_id( firmPtr->race_id )
			 && firmInfo->nation_can_build(nationRecno, firmPtr->race_id, 0) )
		{
			//------- special case handling ---------//

			if( firmId == FIRM_ALCHEMY )		// only build one alchemy
			{
				if( nationPtr->ai_mons_alchemy_count >= 1 )
					return 0;
			}

			if( firmId == FIRM_INCUBATOR )		// only build a maximum of 2 incubators
			{
				if( nationPtr->ai_incubator_count >= 2 )
					return 0;
			}

			if( firmId == FIRM_ANIMAL )		// only build a maximum of 2 animal hiring structures.
			{
				if( nationPtr->ai_mons_animal_count >= 2 )
					return 0;
			}

			else if( firmId == FIRM_FORTRESS )		// never build any fortresses
				return 0;

			//-------- build the firm now --------//

			return create_firm_next_to_place(firmPtr, firmId, firmPtr->race_id, nationRecno);
		}
	}

	return 0;
}
//---- End of function Campaign::create_monster_special_firm ----//

/*

//--- Begin of function Campaign::random_init_monster_tech ---//
//
// Initialize monster technology.
//
// <int> monsterNationRecno - recno of the monster nation
///
// <int> ownTechChance - how many percent of tech of this Fryhtan species
//							    will be made available to the monster nation in
//								 the beginning.
//
// <int> otherTechChance - how many percent of tech of other Fryhtan species
//							      will be made available to the monster nation in
//								   the beginning.
//
void Campaign::random_init_monster_tech(int monsterNationRecno, int ownTechChance, int otherTechChance)
{
	Nation* monsterNation = nation_array[monsterNationRecno];

	err_when( !monsterNation->is_monster() );


	for( int i=1 ; i<=MAX_TECH ; i++ )
	{
		TechInfo* techInfo = tech_res[i];

		int techChance;

		if( techInfo->race_id == monsterNation->race_id )
			techChance = ownTechChance;
		else
			techChance = otherTechChance;

		if( techChance > misc.random(100) )
		{
			int maxTechLevel = techInfo->max_tech_level(monsterNationRecno);

			techInfo->set_nation_tech_level( monsterNationRecno, misc.random(maxTechLevel)+1 );

			//---- if this tech has parent tech, also enable the child tech ----//

			techInfo->enable_parent_tech(monsterNationRecno);
		}
	}
}
//---- End of function Campaign::random_init_monster_tech ----//

*/

//---- Begin of function Campaign::create_lair ----//
//
// <int> campaignNationRecno		- campaign nation recno
// <int> nationRecno					- nation recno
// <int> lairAddCount				- no. of lairs to be added
// <int> independentTownAddCount - no. of independent towns to be added
// <int> specialFirmCount			- no. of special Fryhtan structures to be added
// [int] soldierCombatLevel	   - if given, the combat level of soldiers in the lair will be around this number
//
int Campaign::create_lair(int campaignNationRecno, int nationRecno,
		int lairAddCount, int independentTownAddCount, int specialFirmCount, int soldierCombatLevel)
{
	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//-------------------------------------//

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(20);

	for( int i=townStateArray.size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		//------- create a monster lair --------//

		if( lairAddCount > 0 )
		{
			int soldierCount = MAX_SOLDIER/2 + misc.random(MAX_SOLDIER/4);
			int thisCombatLevel;

			if( soldierCombatLevel )
				thisCombatLevel = soldierCombatLevel + misc.random(30) - 15;
			else
				thisCombatLevel = 30 + campaign_difficulty*10;

			int monsterId;

			if( misc.random(2)==0 )
				monsterId = nation_array[nationRecno]->monster_id();
			else
				monsterId =	misc.random(MAX_MONSTER_TYPE)+1;

			//--- adjust the soldier combat level to balance power differences in different Fryhtan species ---//

			UnitInfo* unitInfo = unit_res[ monster_res[monsterId]->unit_id ];

			int const stdMonsterHitPoint = 300;

			thisCombatLevel = thisCombatLevel * stdMonsterHitPoint / unitInfo->hit_points;
			thisCombatLevel = MIN( 100, thisCombatLevel );
			thisCombatLevel = MAX( 20, thisCombatLevel );

			//---------------------------------------------//

			int firmRecno = create_monster_lair(xLoc, yLoc, nationRecno,
								 soldierCount, thisCombatLevel, monsterId);

			if( !firmRecno )
				continue;

			//---------- create slave town ---------//

			int raceId;

			if( misc.random(2)==0 )
				raceId = nation_array[PLAYER_NATION_RECNO]->race_id;
			else
				raceId = misc.random(MAX_RACE)+1;

			create_slave_town(firmRecno, raceId);

			lairAddCount--;

			//---- create special Fryhtan structures -----//

			if( specialFirmCount > 0 )
			{
				create_monster_special_firm(firmRecno);
				specialFirmCount--;
			}
		}

		//----- add independent towns -------//

		else if( independentTownAddCount > 0 )
		{
			int raceId = misc.random(MAX_RACE)+1;
			int initPopulation = 30 + misc.random(30);
			int initResistance = 60 + misc.random(40);

			create_independent_town(xLoc, yLoc, raceId, initPopulation, initResistance);

			independentTownAddCount--;
		}
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function Campaign::create_lair ----//


//--- Begin of function Campaign::unveil_new_tech ---//
//
// Unveil new tech to the player in a campaign game.
//
// Unveil two new tech levels or the tech to build special structures
// or the tech to perform berserker attack for special units.
//
void Campaign::unveil_new_tech()
{
	//---- unveil new tech for humans -----//

	int unveiledTech=0;

	int i;
	for( i=0 ; i<10 ; i++ )		// maximum 10 tries
	{
		if( misc.random(6) == 0 )		// 1/6 chance to unveil special units and special structures
		{
			if( unveil_spu_tech() )
				break;
		}
		else
		{
			short techClassIdArray[] = {
				TECH_CLASS_PRODUCTION, TECH_CLASS_INFANTRY, TECH_CLASS_WEAPON, TECH_CLASS_SPYING };

			//--- only unveil espionage tech if the scenario has human opponents ---//

			int hasHumanOpponent=0;

			for( int j=2 ; j<=nation_array.size() ; j++ )
			{
				if( nation_array.is_deleted(j) )
					continue;

				if( nation_array[j]->is_human() )
				{
					hasHumanOpponent=1;
					break;
				}
			}

			if( unveil_one_tech(techClassIdArray, 4 + hasHumanOpponent) )
			{
				if( ++unveiledTech == 3 )
					break;
			}
		}
	}

	//---- unveil new tech for monsters -----//

	unveiledTech = 0;

	for( i=0 ; i<10 ; i++ )		// maximum 10 tries
	{
		short techClassIdArray[] = {
			TECH_CLASS_PLANT, TECH_CLASS_INSECT, TECH_CLASS_ANIMAL,
			TECH_CLASS_ALCHEMY, TECH_CLASS_FORTRESS,  TECH_CLASS_INCUBATION,
			TECH_CLASS_MAGIC };

		if( unveil_one_tech(techClassIdArray, 7) )
		{
			if( ++unveiledTech == 2 )
				break;
		}
	}

	//--- unveil a specific tech (this is set in the last game that the player own) ---//

	if( win_to_unveil_tech_id )
	{
		TechInfo* techInfo = tech_res[win_to_unveil_tech_id];

		techInfo->available_level = MAX(techInfo->available_level, 1);

		win_to_unveil_tech_id = 0;
	}
}
//---- End of function Campaign::unveil_new_tech ----//


//--- Begin of function Campaign::unveil_one_tech ---//
//
int Campaign::unveil_one_tech(short* techClassIdArray, int techClassCount)
{
	int techClassSeq = misc.random(techClassCount);

	for( int i=0 ; i<techClassCount ; i++ )
	{
		if( ++techClassSeq >= techClassCount )
			techClassSeq = 1;

		int techClassId = techClassIdArray[techClassSeq];

		//------- only give new Spy tech to research if the game is against a human kingdom ---//

		if( techClassId == TECH_CLASS_SPYING )
		{
			if( target_state_recno &&
				 get_nation( state_array[target_state_recno]->campaign_nation_recno )->is_monster() )
			{
				continue;
			}
		}

		//---------- pick the tech ---------//

		TechClass* techClass = tech_res.tech_class(techClassId);
		TechInfo*  techInfo  = tech_res[techClass->first_tech_id];

		if( misc.random(2)==0 )
		{
			//----- 50% chance of unveiling a new tech ----//

			for( int j=0 ; j<techClass->tech_count ; j++, techInfo++ )
			{
				if( techInfo->available_level==0 )
				{
					techInfo->available_level++;
					return 1;
				}
			}
		}
		else
		{
			//---- 50% chance of unveiling a new level of an existing tech ----//

			for( int j=0 ; j<techClass->tech_count ; j++, techInfo++ )
			{
				if( techInfo->available_level < techInfo->common_max_tech_level )
				{
					techInfo->available_level++;
					return 1;
				}
			}
		}
	}

	return 0;
}
//---- End of function Campaign::unveil_one_tech ----//


//--- Begin of function Campaign::unveil_spu_tech ---//
//
int Campaign::unveil_spu_tech()
{
	//--- first see if the special structure tech class is already available ---//

	int techClassId = TECH_CLASS_SPU_STRUCTURE;

	TechClass* techClass = tech_res.tech_class(techClassId);

	//--- if it's already avaiable, then try the special unit berserker tech class ---//

	if( tech_res[techClass->first_tech_id]->available_level )
	{
		techClassId = TECH_CLASS_SPU_BERSERKER;
		techClass = tech_res.tech_class(techClassId);
	}

	//---- if it's also available, do nothing ----//

	if( tech_res[techClass->first_tech_id]->available_level )
		return 0;

	//---------- make all tech in the class available --------//

	TechInfo*  techInfo = tech_res[techClass->first_tech_id];

	for( int i=0 ; i<techClass->tech_count ; i++, techInfo++ )
		techInfo->available_level = 1;

	return 1;
}
//---- End of function Campaign::unveil_spu_tech ----//


//---- Begin of function Campaign::create_town ----//

int Campaign::create_town(int nationRecno, int townAddCount, int hasFortPercent, int combatLevel)
{
	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(30);

	for( int i=townStateArray.size() ; i>0 && townAddCount > 0 ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		int buildFort = (i % (100/hasFortPercent)) == 0;

		if( create_nation_town(nationRecno, xLoc, yLoc, buildFort, 1, 100, combatLevel) )
			townAddCount--;
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function Campaign::create_town ----//


//---- Begin of function Campaign::add_weapon_to_camp ----//
//
// Randomly add a weapon that the player can produce to the given camp or fort.
//
int Campaign::add_weapon_to_camp(int campFirmRecno)
{
	FirmCamp* firmCamp = firm_array[campFirmRecno]->cast_to_FirmCamp();

	TechClass *techClass = tech_res.tech_class(TECH_CLASS_WEAPON);	// war_tech_class is set the which tech class of weapon to build

	//---- if the player has none of the weapon technology yet, add the basic one ----//

	if( tech_res[techClass->first_tech_id]->get_nation_tech_level(firmCamp->nation_recno)==0 )
	{
		tech_res[techClass->first_tech_id]->set_nation_tech_level(firmCamp->nation_recno, 1 );
	}

	//--- randomly select a weapon that the AI can produce ---//

	short weaponArray[MAX_WEAPON_TYPE];		// available weapon array and count
	short weaponCount=0;

	for( int techId = techClass->first_tech_id; techId<techClass->first_tech_id+techClass->tech_count; ++techId )
	{
		short unitId = tech_res[techId]->unit_id;

		if( !unitId )
			continue;

		UnitInfo* unitInfo = unit_res[unitId];

		if( unitInfo->unit_class != UNIT_CLASS_WEAPON )
			continue;

		if( unitInfo->get_nation_tech_level(firmCamp->nation_recno) > 0 )
		{
			weaponArray[weaponCount++] = unitId;
		}
	}

	err_when( !weaponCount );

	int weaponUnitId = weaponArray[ misc.random(weaponCount) ];

	//------- create a weapon now -----------//

	UnitInfo*   unitInfo = unit_res[weaponUnitId];
	SpriteInfo*	spriteInfo = sprite_res[unitInfo->sprite_id];
	int 			xLoc=firmCamp->loc_x1; // xLoc & yLoc are used for returning results
	int 			yLoc=firmCamp->loc_y1;

	if( !world.locate_space(xLoc, yLoc, firmCamp->loc_x2, firmCamp->loc_y2,
		 spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type) )
	{
		return 0;
	}

	int unitRecno = unit_array.add_unit( weaponUnitId, firmCamp->nation_recno, 0, 0, xLoc, yLoc );

   firmCamp->assign_soldier(unitRecno);

	return 1;
}
//---- End of function Campaign::add_weapon_to_camp ----//


//----- Begin of function Campaign::rebel_form_nation ----//
//
// Rebels in independent towns form new kingdoms.
//
void Campaign::rebel_form_nation()
{
	if( nation_array.nation_count == MAX_NATION )
		return;

	//----- randomly pick an independent town -----//

	int townRecno = misc.random(town_array.size())+1;

	int i;
	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( ++townRecno > town_array.size() )
			townRecno = 1;

		if( town_array.is_deleted(townRecno) )
			continue;

		if( town_array[townRecno]->nation_recno==0 )
			break;
	}

	if( i==0 )
		return;

	//------ form a new nation now -------//

	Town* townPtr = town_array[townRecno];

	int nationRecno = townPtr->form_new_nation();

	if( !nationRecno )
		return;

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->pref_surrender = 0;			// the nation will never surrender

	//------ start with abundance of cash and food ----//

	nationPtr->cash = (float) 3000 + campaign_difficulty*3000 + misc.random(3000);
	nationPtr->food = (float) 1500 + campaign_difficulty*1500 + misc.random(1500);

	//------ set vars so that the new nation will not start sending diplomatic to the player nation (the nation it defected from) too soon ---//

	NationRelation* nationRelation = nationPtr->get_relation(nation_array.player_recno);

	for( i=0 ; i<MAX_TALK_TYPE ; i++ )
	{
		nationRelation->never_accept_until_date_array[i] = info.game_date + misc.random(90) + 90;
	}
}
//------- End of function Campaign::rebel_form_nation ----//


// ------ Begin of function Campaign::create_royal_units ------//
//
void Campaign::create_royal_units(int campaignNationRecno)
{
	CampaignNation *cNation;
	Nation *nationPtr;

	short kingXLoc, kingYLoc;
	Unit *kingPtr;

	if( !is_nation_deleted(campaignNationRecno)
		&& (cNation = get_nation(campaignNationRecno) )
		&& (nationPtr = nation_array.get_campaign_nation(campaignNationRecno))
		&& !unit_array.is_deleted(nationPtr->king_unit_recno)
		&& (kingPtr = unit_array[nationPtr->king_unit_recno])->get_cur_loc(kingXLoc, kingYLoc) )
	{
		for( int i = 0; i < cNation->noble_count; ++i )
		{
			CampaignMember *unit = cNation->noble + i;

			if( unit->sprite_recno != -1 )
				continue;

			int xLoc = kingXLoc;
			int yLoc = kingYLoc;

			UnitInfo* unitInfo = unit_res[unit->unit_id];
			SpriteInfo* spriteInfo = sprite_res[unitInfo->sprite_id];

			// locate space

			int spaceFlag = 0;
			for (int r = 2; r <= 20; r += 2)
			{
				if( kingPtr->is_visible() )
				{
					xLoc = kingXLoc - r;
					yLoc = kingYLoc - r;
				}
				else
				{
					xLoc= kingXLoc;		// no long kingXLoc -r, prefer at the bottom of fort
					yLoc= kingYLoc;     // xLoc & yLoc are used for returning results
				}
			
				if( world.locate_space(xLoc, yLoc, 
					MIN(kingXLoc+r, world.max_x_loc-1), MIN(kingYLoc +r, world.max_y_loc-1),
					 spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type) )
				{
					spaceFlag = 1;
					break;
				}
			}

			if( spaceFlag )
			{
				if( unit->create_unit( nationPtr->nation_recno, xLoc, yLoc) > 0)
				{
					Unit *royalUnit = unit_array[unit->sprite_recno];

					// set royal initially
					royalUnit->is_royal = 1;

					// assign to fort if any space
					if( kingPtr->unit_mode == UNIT_MODE_OVERSEE
						&& firm_array[kingPtr->unit_mode_para]->can_accept_assign(royalUnit->sprite_recno) )
					{
						firm_array[kingPtr->unit_mode_para]->assign_unit( royalUnit->sprite_recno );
					}
				}
			}
			else
			{
				unit->sprite_recno = 0;		// keep that royal for next stage
			}
		}
	}
	else
	{
		// cannot find king, no royal can be created, keep them for next stage

		for( int i = 0; i < cNation->noble_count; ++i )
		{
			CampaignMember *unit = cNation->noble + i;

			if( unit->sprite_recno != -1 )
				continue;

			unit->sprite_recno = 0;				// keep that royal for next stage
		}
	}
}
// ------ End of function Campaign::create_royal_units ------//


//---- Begin of function Campaign::create_offensive_structure ----//
//
void Campaign::create_offensive_structure(int nationRecno, int buildCount)
{
	//--- enable the nation's technology first -------//

	Nation* nationPtr = nation_array[nationRecno];

	int t = misc.random(4);

	int firmId = FIRM_OFFENSIVE_BUILDING_1 + t;
	int techId = TECH_OFFENSIVE_STRUCTURE_1 + t;

	win_to_unveil_tech_id = techId;		// if the player wins the campaign game, this tech id. will be unveiled in the next game.

	tech_res[techId]->set_nation_tech_level(nationRecno, 1);

	//------ build offensive structures now -------//

	for( int i=100 ; i>0 ; i-- )		// maximum 100 tries
	{
		//---- first build an offense structure next to the king's town ---//

		int townRecno = king_oversee_town_recno(nationRecno);

		if( townRecno )
		{
			create_firm_next_to_place( town_array[townRecno], firmId, nationPtr->race_id );

			if( --buildCount == 0 )
				return;
		}

		//----- then build it next to other towns ----//

		while(1)
		{
			townRecno = random_pick_town_with_camp(nationRecno, 10);		// only pick towns with <= 10 links

			if( !townRecno )
				break;

			create_firm_next_to_place( town_array[townRecno], firmId, nationPtr->race_id );

			if( --buildCount == 0 )
				return;
		}
	}
}
//---- End of function Campaign::create_offensive_structure ----//


//---- Begin of function Campaign::create_offensive_structure ----//
//
void Campaign::create_offensive_structure(int townRecno)
{
	//--- enable the nation's technology first -------//

	Town*   townPtr   = town_array[townRecno];
	Nation* nationPtr = nation_array[townPtr->nation_recno];

	int t = misc.random(4);

	int firmId = FIRM_OFFENSIVE_BUILDING_1 + t;
	int techId = TECH_OFFENSIVE_STRUCTURE_1 + t;

	win_to_unveil_tech_id = techId;		// if the player wins the campaign game, this tech id. will be unveiled in the next game.

	tech_res[techId]->set_nation_tech_level(townPtr->nation_recno, 1);

	create_firm_next_to_place( townPtr, firmId, nationPtr->race_id );
}
//---- End of function Campaign::create_offensive_structure ----//


//--- Begin of function Campaign::create_rebel_nation --//
//
// Return: <int> recno of the rebel nation created.
//
int Campaign::create_rebel_nation(int townCount, int hasFortPercent, int militaryStrength)
{
	int nationRecno = nation_array.new_nation( NATION_HUMAN, NATION_AI, nation_array.random_unused_race(), nation_array.random_unused_color() );     // 2nd parameter = the race id., 3rd parameters = color scheme id.

	create_town(nationRecno, townCount, hasFortPercent);

	//----- create firms next to the towns ----//

	int townRecno1 = king_oversee_town_recno(nationRecno);
	int townRecno2 = random_pick_town_with_camp(nationRecno, 2);		// only pick towns with <= 2 links

	create_economic_firm(townRecno1);

	if( townRecno2 )
		create_military_firm(townRecno2, militaryStrength);

	return nationRecno;
}
//---- End of function Campaign::create_rebel_nation ---//


