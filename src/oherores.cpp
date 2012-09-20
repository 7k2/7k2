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

//Filename    : OHERORES.CPP
//Description : Hero resource object

#include <oherores.h>
#include <all.h>
#include <onation.h>
#include <oinfo.h>
#include <otown.h>
#include <ogame.h>
#include <ocampgn.h>
#include <osys.h>
#include <oraceres.h>
#include <oitemres.h>
#include <ogameset.h>
#include <oskill.h>
#include <ounit.h>
#include <ounitres.h>
#include <otransl.h>

//---------- #define constant ------------//

#define HERO_DB   		"HERO"

//------- Begin of function HeroRes::HeroRes -----------//

HeroRes::HeroRes()
{
	init_flag=0;
}
//--------- End of function HeroRes::HeroRes -----------//


//---------- Begin of function HeroRes::init -----------//
//
// This function must be called after a map is generated.
//
void HeroRes::init()
{
	deinit();

	//------- load database information --------//

	load_hero_info();

	generate_fictional_hero();

	hero_appear_flag = 1;

	init_flag=1;
}
//---------- End of function HeroRes::init -----------//


//---------- Begin of function HeroRes::deinit -----------//

void HeroRes::deinit()
{
	if( init_flag )
	{
		mem_del(hero_info_array);

		init_flag=0;
	}
}
//---------- End of function HeroRes::deinit -----------//


//------- Begin of function HeroRes::load_hero_info -------//
//
// Read in information of HERO.DBF into memory array
//
void HeroRes::load_hero_info()
{
	HeroRec  *heroRec;
	HeroInfo *heroInfo;
	int      i;
	Database *dbHero = game_set.open_db(HERO_DB);

	hero_count = (short) dbHero->rec_count();

	err_when( hero_count > MAX_HERO );

	hero_info_array = (HeroInfo*) mem_add( sizeof(HeroInfo) * MAX_HERO );

	//------ read in hero information array -------//

	memset( hero_info_array, 0, sizeof(HeroInfo) * MAX_HERO );

	for( i=0 ; i<hero_count ; i++ )
	{
		heroRec  = (HeroRec*) dbHero->read(i+1);
		heroInfo = hero_info_array+i;

		heroInfo->hero_id = i+1;

		heroInfo->infantry_unit = heroRec->infantry_unit=='1';
		heroInfo->special_unit  = heroRec->special_unit=='1';

		err_when( !heroInfo->infantry_unit && !heroInfo->special_unit );

		misc.rtrim_fld( heroInfo->name, heroRec->name, heroRec->NAME_LEN );
		translate.multi_to_win(heroInfo->name, heroInfo->NAME_LEN);

		heroInfo->race_id = misc.atoi(heroRec->race_id, heroRec->RACE_ID_LEN);
		heroInfo->historic_hero = 1;		// this is a historic hero as opposite to a fictional one
	}
}
//--------- End of function HeroRes::load_hero_info ---------//


//-------- Begin of function HeroRes::generate_fictional_hero --------//

void HeroRes::generate_fictional_hero()
{
	// ##### begin Gilbert 14/3 ########//
	// ----- backup seed so all machine have same set of fictional hero (for multi-player game) ----//

	long backupSeed = misc.get_random_seed();
	misc.set_random_seed(4);			// set a constant seed
	// ##### end Gilbert 14/3 ########//

	// ###### begin Gilbert 1/3 #####//
	int oldHeroCount = hero_count;
	int nameIdArray[MAX_HERO];
	// ###### end Gilbert 1/3 #####//

	for( int raceId=1 ; raceId<=MAX_RACE ; raceId++ )
	{
		//--- count the number of historic heroes ---//

		int heroCount=0;

		int i;
		for( i=1 ; i<=hero_count ; i++ )
		{
			if( operator[](i)->race_id == raceId )
				heroCount++;
		}

		//---- add fictional heroes ------//

		for( i=heroCount ; i<HERO_COUNT_PER_RACE ; i++ )
		{
			// ##### begin Gilbert 1/3 ######//
			if( hero_count >= MAX_HERO )
			{
				break;
			}
			// ##### end Gilbert 1/3 ######//
			HeroInfo* heroInfo = hero_info_array + hero_count;
			hero_count++;

			heroInfo->hero_id = hero_count;
			heroInfo->race_id = raceId;

			if( misc.random(2)==0 )
				heroInfo->infantry_unit = 1;
			else
				heroInfo->special_unit = 1;

			int nameId = race_res[raceId]->get_new_name_id();
			// ##### begin Gilbert 1/3 #####//
			err_when( hero_count-oldHeroCount-1 < 0 || hero_count-oldHeroCount-1 >= MAX_HERO );
			nameIdArray[hero_count-oldHeroCount-1] = nameId;
			// ##### end Gilbert 1/3 #####//

			strncpy( heroInfo->name, race_res[raceId]->get_name(nameId), heroInfo->NAME_LEN );
			heroInfo->name[heroInfo->NAME_LEN] = '\0';
		}
	}

	// ######## begin Gilbert 1/3 ######//

	// ------ free name occupied --------//
	// free name after generated so names are less likely to collide //

	for( int j = oldHeroCount+1; j <= hero_count; ++j )
	{
		HeroInfo *heroInfo = hero_info_array + j - 1;
		if( heroInfo->race_id && nameIdArray[j-oldHeroCount-1] )
		{
			race_res[heroInfo->race_id]->free_name_id( nameIdArray[j-oldHeroCount-1] );
		}
	}
	// ######## end Gilbert 1/3 ######//

	// ##### begin Gilbert 14/3 ########//
	// ----- restore seed ----//

	misc.set_random_seed(backupSeed);
	// ##### end Gilbert 14/3 ########//
}
//---------- End of function HeroRes::generate_fictional_hero -------//


//---------- Begin of function HeroRes::second_init -----------//

void HeroRes::second_init()
{
	//------- randomize the for_hire flag ------//

	int i;
	for( i=1 ; i<=hero_count ; i++ )
	{
		HeroInfo* heroInfo = operator[](i);

		heroInfo->for_hire = misc.random(2);
		heroInfo->appeared_flag = 0;
	}

	//--- if this is a campaign, and if any of the royal units are heroes, we should set the appeared_flag of HeroRes ---//

	if( game.is_campaign_mode() )
	{
		Campaign* campaignPtr = game.campaign();
		int       heroId;

		for( i=MAX_NATION ; i>0 ; i-- )
		{
			if( campaignPtr->is_nation_deleted(i) )
				continue;

			CampaignNation* cNation = campaignPtr->get_nation(i);

			if( (heroId = cNation->king.hero_id) > 0 )
				operator[](heroId)->appeared_flag = 1;

			for( int j=0 ; j<cNation->noble_count ; j++ )
			{
				if( (heroId=cNation->noble[j].hero_id) > 0 )
					operator[](heroId)->appeared_flag = 1;
			}
		}
	}
}
//---------- End of function HeroRes::second_init -----------//


//---------- Begin of function HeroRes::operator[] -----------//

HeroInfo* HeroRes::operator[](int heroId)
{
	err_if( heroId<1 || heroId>hero_count )
		err_now( "HeroRes::operator[]" );

	return hero_info_array+heroId-1;
}

//------------ End of function HeroRes::operator[] -----------//


//---------- Begin of function HeroRes::create_hero -----------//
//
// <int> xLoc, yLoc  - the location where the hero is created.
// <int> nationRecno - nation recno of the unit
// <int> raceId		- the race id. of the unit
// [int] heroPower 	- the overall power of the unit, ranging from 0 to 100.
//							  (default: random)
// [int] unitLoyalty - the loyalty of the unit (default: random)
// [int] forHire 		- whether the created hero is for hire.
//							  if so, then only hero with for_hire == forHire
//							  will be created.
//							  (default: -1, ignore for_hire)
// [int] historicHero - if this is given, the hero's historic_hero var
//								must be the same as the given one. (default: -1)
// [int] useReserve   - use the reserved hero, for campaign only. We need to reserve a hero for Stage 6 - Hero Debut
//							 	(default: 0)
//	[int] randomClass	 - if the hero can be both infantry or special unit, this flag decides whether randomly select
//							 	or not (default: 0 not randomly select)
//								(1 select infantry unit)
//								(2 select special unit)
//
// return: <int> the recno of the unit created.
//
int HeroRes::create_hero(int xLoc, int yLoc, int nationRecno, int raceId,
								 int heroPower, int unitLoyalty, int forHire, int historicHero, int useReserve, int randomClass)

{
	//---- pick a hero who hasn't appeared in the game yet ----//

	HeroInfo* heroInfo;

	short* filterHeroArray = (short*) mem_add( sizeof(short) * hero_count );
	int    filterHeroCount = 0;

	for( int i=1 ; i<=hero_count ; i++ )
	{
		heroInfo = operator[](i);

		if( heroInfo->appeared_flag )
			continue;

		if( raceId && heroInfo->race_id != raceId )
			continue;

		if( forHire != -1 && heroInfo->for_hire != forHire )
			continue;

		if( historicHero != -1 && heroInfo->historic_hero != historicHero )
			continue;

		filterHeroArray[filterHeroCount++] = i;
	}

	if( filterHeroCount==0 )
	{
		mem_del( filterHeroArray );
		return 0;
	}

	if( game.is_campaign_mode() )
	{
		if( useReserve )		// use the reserved hero, for campaign only. We need to reserve a hero for Stage 6 - Hero Debut
			heroInfo = hero_res[ filterHeroArray[filterHeroCount-1] ];		// use the last one
		else
			heroInfo = hero_res[ filterHeroArray[misc.random(filterHeroCount-1)] ];		// -1 exclude the last one
	}
	else
		heroInfo = hero_res[ filterHeroArray[misc.random(filterHeroCount)] ];

	mem_del( filterHeroArray );

	//----- determine the unit id. of the hero -----//

	int unitId;

	if( heroInfo->infantry_unit && heroInfo->special_unit )
	{
		if (!randomClass)
		{
			if( misc.random(2)==0 )
				unitId = race_res[heroInfo->race_id]->infantry_unit_id;
			else
				unitId = race_res[heroInfo->race_id]->special_unit_id;
		}
		else if (randomClass == 1)
			unitId = race_res[heroInfo->race_id]->infantry_unit_id;
		else 
			unitId = race_res[heroInfo->race_id]->special_unit_id;
	}
	else if( heroInfo->infantry_unit )
		unitId = race_res[heroInfo->race_id]->infantry_unit_id;

	else if( heroInfo->special_unit )
		unitId = race_res[heroInfo->race_id]->special_unit_id;

	else
		err_here();

	//--------- create a hero unit ----------//

	int unitRecno = create_powerful_unit(xLoc, yLoc, nationRecno, unitId,
						 heroPower, unitLoyalty);

	if( !unitRecno )
		return 0;

	unit_array[unitRecno]->hero_id = heroInfo->hero_id;

	// ###### begin Gilbert 14/3 #########//
#ifdef DEBUG
	if( !heroInfo->historic_hero )
	{
		int c = 0;		// set break point here
	}
#endif
	// ###### end Gilbert 14/3 #########//

	err_when( heroInfo->appeared_flag != 0 );

	heroInfo->appeared_flag = 1;

	return unitRecno;
}
//---------- End of function HeroRes::create_hero -----------//


//---------- Begin of function HeroRes::create_powerful_unit -----------//
//
// Create a powerful unit, though, the unit does not have to be a hero.
// It is called by create_hero() and FirmInn::add_inn_unit().
//
// <int> xLoc, yLoc  - the location where the hero is created.
// <int> nationRecno - nation recno of the unit
// <int> unitId		- the unit id. of the unit
// [int] heroPower 	- the overall power of the unit, ranging from 0 to 100.
//							  (default: random)
// [int] unitLoyalty - the loyalty of the unit (default: random)
// [int] historicHero - if this is given, the hero's historic_hero var
//								must be the same as the given one. (default: -1)
//
// return: <int> the recno of the unit created.
//
int HeroRes::create_powerful_unit(int xLoc, int yLoc, int nationRecno, int unitId,
								 int heroPower, int unitLoyalty, int historicHero)
{
	//------ create the hero unit now -------//

	if( !unitLoyalty )
		unitLoyalty = 50 + misc.random(40);

	int unitRecno = unit_array.add_unit( unitId, nationRecno, RANK_SOLDIER, unitLoyalty, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	//----- if the hero's power is not given, randomize it now -----//

	if( !heroPower )
	{
		int rc = misc.random(10);

		if( rc==0 )
		{
			heroPower = 50 + misc.random(50);
		}
		else if( rc <= 2 )		// 1 to 2
		{
			heroPower = 50 + misc.random(35);
		}
		else if( rc <= 5 )		// 3 to 5
		{
			heroPower = 50 + misc.random(25);
		}
		else							// 6 to 9
		{
			heroPower = 50 + misc.random(10);
		}

		//---- historic heroes have higher than usual power ----//

		if( historicHero==1 )
			heroPower = MAX( heroPower, 70 );		// at least 70
	}

	//------ set the item the hero carries ------//

	if( heroPower > 35 )		// only add items if the hero power is larger than 35
	{
		int itemRarity = MAX_RARITY * heroPower / 100;

		unitPtr->item.init_random(MIN_RARITY + misc.random(itemRarity-MIN_RARITY)+1,
			itemRarity, unitPtr->unit_id, &unitPtr->skill);

		err_when( !unitPtr->item.id );
	}

	//----- initialize hero skill and items -------//

	// ######## begin Gilbert 13/4 #### //
	int combatLevel = 100 + misc.random(400);
	int skillLevel  = 100 + (500-combatLevel)/3;

	combatLevel = combatLevel * heroPower / 100;
	skillLevel  = skillLevel * heroPower / 100;
	// ######## end Gilbert 13/4 #### //

	combatLevel = MAX( BASIC_COMBAT_TRAIN, combatLevel );
	skillLevel  = MAX( BASIC_SKILL_TRAIN , skillLevel );

	int maxCombatLevel = combatLevel * (100+misc.random(30)) / 100;
	maxCombatLevel = MAX( MAX_COMBAT_BATTLE, maxCombatLevel );

	int maxSkillLevel = skillLevel * (100+misc.random(30)) / 100;
	maxSkillLevel = MAX( MAX_SKILL_TRAIN, maxSkillLevel );

	unitPtr->skill.set_max_skill_level( maxSkillLevel );
	unitPtr->skill.set_skill_level( skillLevel );

	unitPtr->skill.set_max_combat_level( maxCombatLevel );
	unitPtr->set_combat_level( combatLevel );

	return unitRecno;
}
//---------- End of function HeroRes::create_powerful_unit -----------//


//---------- Begin of function HeroRes::next_day -----------//

void HeroRes::next_day()
{
	if( hero_appear_flag )
	{
		think_generate_hero();
   }
}
//---------- End of function HeroRes::next_day -----------//


//---------- Begin of function HeroRes::think_generate_hero -----------//

void HeroRes::think_generate_hero()
{
	#define MAX_INDEPENDENT_HERO 	20			// the game will only have a maximum of 20 heroes at a time

	// ######## begin Gilbert 20/4 ##########//

	if( info.game_date%20 != 0 )
		return;

	// count number of human nation

	int humanNationCount = 0;
	for( int n = 1; n < nation_array.size(); ++n )
	{
		if( !nation_array.is_deleted(n) && nation_array[n]->is_human() )
		{
			++humanNationCount;
		}
	}

	// ########### begin Gilbert 26/4 ###########//
	int yearPassed = (info.game_date - info.game_start_date) / 365;
	// ########### end Gilbert 26/4 ###########//

	if( humanNationCount > 0 && misc.random(90+yearPassed*2) >= humanNationCount	// more human kingdom higher chance.
		|| humanNationCount <= 0 && misc.random(120+yearPassed*2) != 0 )		// If no human kingdom, still a small chance
		return;

//	if( info.game_date%10 != 0 || misc.random(30) != 0 )
//		return;
	// ######## end Gilbert 20/4 ##########//

	int heroCount=0;

	//--- count the number of existing independent heros ---//

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->hero_id && unitPtr->nation_recno==0 )
			heroCount++;
	}

	if( heroCount + misc.random(5) >= MAX_INDEPENDENT_HERO )
		return;

	generate_hero();
}
//---------- End of function HeroRes::think_generate_hero -----------//


//---------- Begin of function HeroRes::generate_hero -----------//
//
// return: <int> unit recno of the hero created.
//
int HeroRes::generate_hero()
{
	//-------- heroes come from towns ----------//

	//----- first try to locate an independent town -----//

	int   townRecno = misc.random(town_array.size())+1;
	Town* townPtr;

	int i;
	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( ++townRecno > town_array.size() )
			townRecno = 1;

		if( town_array.is_deleted(townRecno) )
			continue;

		townPtr = town_array[townRecno];

		if( townPtr->nation_recno == 0 )
			break;
	}

	//----- if there is no independent town, locate a nation town ----//

	if( i==0 )
	{
		townRecno = misc.random(town_array.size())+1;

		for( i=town_array.size() ; i>0 ; i-- )
		{
			if( ++townRecno > town_array.size() )
				townRecno = 1;

			if( town_array.is_deleted(townRecno) )
				continue;

			break;
		}
	}

	if( i==0 )
		return 0;

	townPtr = town_array[townRecno];

	//----- create heroes who wander around on the map ------//

	int xLoc = townPtr->loc_x1;
	int yLoc = townPtr->loc_y1;

	if( world.locate_space(xLoc, yLoc, townPtr->loc_x2, townPtr->loc_y2,
			1, 1, UNIT_LAND, townPtr->region_id) )
	{
		return create_hero( xLoc, yLoc, 0, townPtr->race_id );
	}

	return 0;
}
//---------- End of function HeroRes::generate_hero -----------//


//---------- Begin of function HeroRes::induce_hero_join_nation ---------//
//
// This function is called when the player has done something significant
// (like killing a Fryhtan All-high) that may induce the hero to
// come to serve the kingdom.
//
// <int> nationRecno - the recno of the nation
// <int> inducePower - the power of the induction 0 to 100
//
int HeroRes::induce_hero_join_nation(int nationRecno, int inducePower)
{
	//------- scan all independent heros --------//

	Nation* nationPtr = nation_array[nationRecno];
	int 	  heroPower;

	if( nationPtr->is_monster() )		// heroes do not serve Fryhtan kingdoms
		return 0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( !unitPtr->hero_id || unitPtr->nation_recno!=0 )
			continue;

		heroPower = (unitPtr->skill_level() + unitPtr->combat_level()/2) / 3;

		if( unitPtr->item.id )
			heroPower += (item_res.rareness(unitPtr->item.id)+1) * 10;

		if( i%2==0 )			// some heroes look at kill_monster_score, but some don't
			heroPower -= (int) nationPtr->kill_monster_score/10;

		if( nationPtr->reputation > 0 )
			heroPower -= (int) nationPtr->reputation/3;
		else
			heroPower -= (int) nationPtr->reputation;		// negative reputation results in higher heroPower, which has a negative effect on getting the hero

		if( unitPtr->race_id == nationPtr->race_id )
			heroPower -= 10;

		if( inducePower >= heroPower )
		{
			//------ change nation now --------//

			if( unitPtr->betray(nationRecno) )
			{
				//---- the unit moves close to the newly joined nation ----//

				unitPtr->ai_move_to_nearby_town();
				return unitPtr->sprite_recno;
			}
		}
	}

	return 0;
}
//---------- End of function HeroRes::induce_hero_join_nation -----------//


//--------- Begin of function HeroRes::hire_cost ---------//
//
// Return the hiring cost based on the parameters passed.
//
// It is used by Inn and independent heroes.
//
int HeroRes::hire_cost(int combatLevel, int maxCombatLevel,
							  int skillLevel, int spySkill, Item* itemPtr)
{
	long hireCost  = 0;

	// --------- basic cost --------//

	// ##### patch begin Gilbert 13/7 ######//
	// hireCost = combatLevel + skillLevel*2;
	hireCost = combatLevel + skillLevel*2 + 100;
	// ##### patch end Gilbert 13/7 ######//

	// --------- combat skill cost ---------//

	if( combatLevel > MAX_COMBAT_TRAIN )		// extra cost for hero
	{
		hireCost += combatLevel * 2 - MAX_COMBAT_TRAIN;
		hireCost += maxCombatLevel;
	}

	// ------- skill cost ----------//

	if( skillLevel > 0 )				// if this unit is one with leadership skill
	{
		hireCost += skillLevel;

		for( int i=10; i < skillLevel ; i+=20 )		// increase the hiring cost with bigger steps when the skill level gets higher
			hireCost += i/5;
	}

	// -------- spy skill cost ----------//

	if( spySkill > 0 )		// if this unit is a spy
	{
		hireCost += spySkill;

		for( int i=10; i < spySkill ; i+=20 )		// increase the hiring cost with bigger steps when the skill level gets higher
			hireCost += i/5;
	}

	hireCost *= 2;

	// ------- item cost ---------//

	hireCost += itemPtr->cost();

	if( hireCost > 30000 )
		hireCost = 30000;			// touching the short integer limit

	return hireCost;
}
//----------- End of function HeroRes::hire_cost -----------//
