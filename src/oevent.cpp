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

// Filename    : OEVENT.CPP
// Description : random event


#include <oevent.h>
#include <all.h>
#include <oconfig.h>
#include <otech.h>
#include <otechres.h>
#include <itemid.h>
#include <oitemres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ounitres.h>
#include <osprtres.h>
#include <ogame.h>
#include <onation.h>
#include <osite.h>
#include <ofirmall.h>
#include <ounitall.h>
#include <otown.h>
#include <onews.h>

#ifdef DEBUG
// define this macro if random event (preferably) happen on this nation
// undefine FIXED_NATION if not wanted to
// #define FIXED_NATION 1
#endif

// try not to use m.random

void RandomEvent::init(int randomSeed)
{
	last_event_id = 0;
	last_event_nation = 0;
	last_event_para1 = 0;
	last_event_para2 = 0;

	random_seed = randomSeed;

	// first event comes later
#ifdef DEBUG
	// debug mode comes more frequent
	day_to_event = 500 - 50 * config.random_event_frequency - random(50);
#else
	day_to_event = 1000 - 200 * config.random_event_frequency - random(200);
#endif
}
	

void RandomEvent::next_day()
{
	if( config.random_event_frequency == 0 )
		return;

	if( --day_to_event <= 0 )
	{
		if( execute_event() )
		{
#ifdef DEBUG
			day_to_event = 500 - 50 * config.random_event_frequency - random(25);
#else
			day_to_event = 1000 - 200 * config.random_event_frequency - random(100);
#endif
		}
		else
		{
			// no effect happen, try again soon

			day_to_event = 10 + random(10);
		}
	}
	
	err_when( day_to_event <= 0 );		// must queue for next day-to-event
}


// then set last_event_id, last_event_para1, last_event_para2
// return 0 if no event happen
int RandomEvent::execute_event()
{
	last_event_id = 0;
	last_event_para1 = 0;
	last_event_para2 = 0;

	int eventId = random(RANDOM_EVENT_COUNT)+1;

	switch( eventId )
	{
	case RANDOM_EVENT_FINISH_TECH:
		{
			int nationRecno = random(nation_array.size()) + 1;
#ifdef FIXED_NATION
			nationRecno = FIXED_NATION;
#endif
			for(int t = nation_array.size(); t > 0; --t, ++nationRecno )
			{
				if(nationRecno > nation_array.size() )
					nationRecno = 1;

				Nation* nationPtr;
				int techId;
				if( !nation_array.is_deleted(nationRecno)
					&& (nationPtr = nation_array[nationRecno])
					&& (techId = nationPtr->research_tech_id) )
				{
					nationPtr->progress_research(100.0f);

					// ------ add news message ---------//

					if( nationPtr->is_own() )
					{
						news_array.event_finish_tech( techId, tech_res[techId]->get_nation_tech_level(nationRecno) );
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = nationRecno;
					last_event_para1 = techId;
					break;
				}
			}
		}
		break;

	case RANDOM_EVENT_NEW_TECH:
		{
			int nationRecno = random(nation_array.size()) + 1;	// no need to try other nation
#ifdef FIXED_NATION
			nationRecno = FIXED_NATION;
#endif
			for(int t = nation_array.size(); t > 0; --t, ++nationRecno )
			{
				if(nationRecno > nation_array.size() )
					nationRecno = 1;

				if( !nation_array.is_deleted(nationRecno) )
					break;
			}

			if( !nation_array.is_deleted(nationRecno) )
			{
				Nation *nationPtr = nation_array[nationRecno];
				short validTechIdCount = 0;
				short validTechId[MAX_TECH];
				short techId;
				for( techId = 1; techId <= MAX_TECH; ++techId )
				{
					if( tech_res[techId]->can_research(nationRecno) &&
						 nationPtr->research_tech_id != techId )				// if the nation is currently researching this tech, don't consider it
					{
						validTechId[validTechIdCount++] = techId;
					}
				}

				if( validTechIdCount > 0 )
				{
					techId = validTechId[random(validTechIdCount)];
					TechInfo *techInfo = tech_res[techId];
					int newTechLevel = techInfo->get_nation_tech_level(nationRecno)+1;
					techInfo->set_nation_tech_level(nationRecno, newTechLevel );

					// ------ add news message ---------//

					if( nationPtr->is_own() )
					{
						news_array.event_new_tech( techId, newTechLevel );
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = nationRecno;
					last_event_para1 = techId;
				}
			}
		}
		break;

	case RANDOM_EVENT_FOOD_DESTROYED:
		{
			int nationRecno = random(nation_array.size()) + 1;	// no need to try other nation
#ifdef FIXED_NATION
			nationRecno = FIXED_NATION;
#endif
			for(int t = nation_array.size(); t > 0; --t, ++nationRecno )
			{
				if(nationRecno > nation_array.size() )
					nationRecno = 1;

				if( !nation_array.is_deleted(nationRecno) )
					break;
			}

			Nation *nationPtr;
			if( !nation_array.is_deleted(nationRecno) 
				&& (nationPtr = nation_array[nationRecno])
				&& nationPtr->food > 500.0f )
			{
				int foodLost = 0;
				if( nationPtr->food >= 10000.0f )
					foodLost = 2000 + random(2000);
				else
					foodLost = 100 + random((int)nationPtr->food/4);
				nationPtr->consume_food( (float)foodLost );

				// ------ add news message ---------//

				if( nationPtr->is_own() )
				{
					news_array.event_food_destroyed( foodLost );
				}

				// ------- record event happen ------//

				last_event_id = eventId;
				last_event_nation = nationRecno;
				last_event_para1 = foodLost;
			}
		}
		break;

	case RANDOM_EVENT_GENERAL_DIE:
	case RANDOM_EVENT_UNIT_DIE:
		{
			int unitRecno = random(unit_array.size()) + 1;
			for( int t = unit_array.size(); t > 0; --t, ++unitRecno )
			{
				if( unitRecno > unit_array.size() )
					unitRecno = 1;

				Unit *unitPtr;
				if( !unit_array.is_deleted(unitRecno)
					&& (unitPtr = unit_array[unitRecno])
					&& unitPtr->nation_recno
#ifdef FIXED_NATION
					&& unitPtr->nation_recno == FIXED_NATION
#endif
					&& (eventId == RANDOM_EVENT_GENERAL_DIE && unitPtr->rank_id == RANK_GENERAL
						|| eventId == RANDOM_EVENT_UNIT_DIE && unitPtr->rank_id == RANK_SOLDIER )
					&& ( unitPtr->is_human() && unitPtr->unit_id != UNIT_WAGON
						|| unitPtr->is_monster() && random(2)<1) 		// monster has 1/2 chance survive
					&& !game.is_unit_in_condition(unitPtr->unique_id) )		// unit affecting win/lose can't die
				{
					if( unitPtr->is_visible() )		// mobile
					{
						// ------ add news message ---------//

						if( unitPtr->is_own() )
						{
							news_array.event_unit_die( unitRecno );
						}

						// ------- record event happen ------//

						last_event_id = eventId;
						last_event_nation = unitPtr->nation_recno;
						last_event_para1 = unitPtr->race_id;		// do not remember unit recno, it will be killed
						last_event_para2 = unitPtr->hero_id ? -unitPtr->hero_id : unitPtr->name_id;
						unitPtr->hit_points = 0.0f;
						break;
					}
					else if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
					{
						Firm *firmPtr = firm_array[unitPtr->unit_mode_para];

						if( firmPtr->cast_to_FirmCamp() )
						{
							// ------ add news message ---------//

							if( unitPtr->is_own() )
							{
								news_array.event_unit_die( unitRecno );
							}

							// ------- record event happen ------//

							last_event_id = eventId;
							last_event_nation = unitPtr->nation_recno;
							last_event_para1 = unitPtr->race_id;		// do not remember unit recno, it will be killed
							last_event_para2 = unitPtr->hero_id ? -unitPtr->hero_id : unitPtr->name_id;
							firmPtr->cast_to_FirmCamp()->kill_overseer();
							break;
						}
						else if( firmPtr->cast_to_FirmMonsterFortress() )
						{
							// ------ add news message ---------//

							if( unitPtr->is_own() )
							{
								news_array.event_unit_die( unitRecno );
							}

							// ------- record event happen ------//

							last_event_id = eventId;
							last_event_nation = unitPtr->nation_recno;
							last_event_para1 = unitPtr->race_id;		// do not remember unit recno, it will be killed
							last_event_para2 = unitPtr->hero_id ? -unitPtr->hero_id : unitPtr->name_id;
							firmPtr->cast_to_FirmMonsterFortress()->kill_unit(unitRecno);
							break;
						}
						else
						{
							// other case
						}
					}
				}
			}
		}
		break;

	case RANDOM_EVENT_SOLDIER_DIE:
		{
			int firmRecno = random(firm_array.size()) +1;
			for( int t = firm_array.size(); t > 0; --t, ++firmRecno )
			{
				if( firmRecno > firm_array.size() )
					firmRecno = 1;
				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];
				
				if( !firmPtr->nation_recno || firmPtr->under_construction )
					continue;
#ifdef FIXED_NATION
				if( firmPtr->nation_recno != FIXED_NATION )
					continue;
#endif

				int peopleKill = 0;

				// -------- kill soldier in camp -------//

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();
					for( int s = camp->soldier_count; s > 0; --s )
					{
						Soldier *soldierPtr = camp->soldier_array + s - 1;
						if( (soldierPtr->is_human() && random(soldierPtr->max_hit_points()*2) > (int)soldierPtr->hit_points
							|| soldierPtr->is_monster() && random(soldierPtr->max_hit_points()*3) > (int)soldierPtr->hit_points *2 )
							&& !game.is_unit_in_condition(soldierPtr->unique_id) )		// unit affecting win/lose can't die
						{
							camp->kill_soldier(s);
							peopleKill++;
						}
					}

					// ------- kill archer ------- //

					if( firmPtr->cast_to_FirmFort() )
					{
						FirmFort *fort = firmPtr->cast_to_FirmFort();
						if( random(MAX_FORT_ARCHER) < fort->current_archer_count )
						{
							fort->kill_archer();
							peopleKill++;
						}
					}
				}
				else if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *train = firmPtr->cast_to_FirmTrain();
					for( int s = train->trainee_count; s > 0; --s )
					{
						Trainee *traineePtr = train->trainee_array + s - 1;
						if( traineePtr->race_id && random(100) > 50 )		// half chance
						{
							train->kill_trainee(s);
							peopleKill++;
						}
					}
				}
				else if( firmPtr->cast_to_FirmMonsterFortress() )
				{
					FirmMonsterFortress *mfort = firmPtr->cast_to_FirmMonsterFortress();
					for( int s = mfort->archer_count; s > 0; --s )
					{
						Unit *archerPtr = unit_array[mfort->archer_unit_recno[s-1]];
						if( archerPtr->rank_id != RANK_KING && archerPtr->race_id 
							&& random(archerPtr->max_hit_points()*3) > (int)archerPtr->hit_points*2 )
						{
							mfort->kill_soldier(s);
							peopleKill++;
						}
					}
				}
				else if( firmPtr->cast_to_FirmMonsterTrain() )
				{
					FirmMonsterTrain *train = firmPtr->cast_to_FirmMonsterTrain();
					for( int s = train->trainee_count; s > 0; --s )
					{
						MonsterTrainee *traineePtr = train->trainee_array + s - 1;
						if( random(100) > 50 )		// half chance
						{
							train->kill_trainee(s);
							peopleKill++;
						}
					}
				}

				if( peopleKill > 0 )
				{
					// ------ add news message ---------//

					if( firmPtr->is_own() )
					{
						news_array.event_soldier_die( firmRecno, peopleKill );
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = firmPtr->nation_recno;
					last_event_para1 = firmRecno;
					last_event_para2 = peopleKill;
					break;
				}
			}
		}
		break;

	case RANDOM_EVENT_UNIT_JOIN:
		{
			// -------- find nation -------//

			int nationRecno = 0;
			if( nation_array.packed_size() > 0)
			{
				nationRecno = nation_array.packed_recno(random(nation_array.packed_size())+1);
#ifdef FIXED_NATION
				nationRecno = FIXED_NATION;
#endif
				if( !nation_array.is_deleted(nationRecno) )
				{
					Nation *nationPtr = nation_array[nationRecno];

					int raceId = random(MAX_RACE+MAX_MONSTER_TYPE)+1;
					int unitId;
					int combatLevel = BASIC_SKILL_TRAIN + random(MAX_SKILL_TRAIN-BASIC_SKILL_TRAIN);
					int skillLevel = BASIC_COMBAT_TRAIN + random(MAX_COMBAT_TRAIN-BASIC_COMBAT_TRAIN);
					if( raceId <= MAX_RACE )
					{
						switch(random(3))
						{
						case 0:
							unitId = race_res[raceId]->civilian_unit_id;
							combatLevel = CITIZEN_COMBAT_LEVEL;
							skillLevel = CITIZEN_SKILL_LEVEL;
							break;
						case 1:
							unitId = race_res[raceId]->infantry_unit_id;
							break;
						case 2:
							unitId = race_res[raceId]->special_unit_id;
							break;
						default:
							err_here();
						}
					}
					else	// monsterId = raceId - MAX_RACE
					{
						raceId = -(raceId - MAX_RACE);
						unitId = monster_res[-raceId]->unit_id;
					}
					int unitRecno = create_boundary_unit( unitId, 0 );		// indepent at first

					if( unitRecno )
					{
						// set combat level, skill level, etc.

						Unit *unitPtr = unit_array[unitRecno];
						unitPtr->skill.set_skill_level( skillLevel );
						unitPtr->set_combat_level( combatLevel );
						unitPtr->betray(nationRecno);

//						if( unitPtr->cur_path && unitPtr->is_blocked() )
//						{
//							// cannot reach town, resign it
//							unitPtr->resign( COMMAND_AUTO );
//						}
//						else
						{
							// no news message because betray generate message

							// go to a town or fort?

							last_event_id = eventId;
							last_event_nation = nationRecno;
							last_event_para1 = unitRecno;
						}
					}
				}
			}
		}
		break;

	case RANDOM_EVENT_WAGON_JOIN:
		{
			// -------- find nation -------//

			int nationRecno = 0;
			if( nation_array.packed_size() > 0)
			{
				nationRecno = nation_array.packed_recno(random(nation_array.packed_size())+1);
#ifdef FIXED_NATION
				nationRecno = FIXED_NATION;
#endif

				if( !nation_array.is_deleted(nationRecno) )
				{
					Nation *nationPtr = nation_array[nationRecno];

					int raceId = random(MAX_RACE)+1;
					int unitId = UNIT_WAGON;
					int unitRecno = create_boundary_unit( unitId, 0 );		// independent first

					if( unitRecno )
					{
						UnitWagon *wagonPtr = unit_array[unitRecno]->cast_to_UnitWagon();
						err_when( !wagonPtr );

						// set population

						err_when(MAX_WAGON_POPULATION<=3);
						wagonPtr->set_race( raceId );
						wagonPtr->set_pop(MAX_WAGON_POPULATION-random(3));
						wagonPtr->betray(nationRecno);

//						if( unitPtr->cur_path && unitPtr->is_blocked() )
//						{
//							// cannot reach town, resign it
//							unitPtr->resign( COMMAND_AUTO );
//						}
//						else
						{
							// no news message because betray generate message

							last_event_id = eventId;
							last_event_nation = nationRecno;
							last_event_para1 = unitRecno;
						}
					}
				}
			}
		}
		break;

	case RANDOM_EVENT_ITEM_DISCOVER:
		{
			int townRecno = random(town_array.size()) + 1;
			for( int t = town_array.size(); t > 0; --t, ++townRecno )
			{
				if( townRecno > town_array.size() )
					townRecno = 1;

				if( town_array.is_deleted(townRecno) )
					continue;

				Town *townPtr = town_array[townRecno];
				if( !townPtr->nation_recno )
					continue;
#ifdef FIXED_NATION
				if( townPtr->nation_recno != FIXED_NATION )
					continue;
#endif

				int itemCreated = 0;
				for( int i = 3+random(2); i > 0; --i )
				{
					int itemId = item_res.random_item_id(MIN_RARITY, MIN_RARITY+random(MAX_RARITY-MIN_RARITY+1));
					if( itemId && site_array.add_site(townPtr->loc_x2-i, townPtr->loc_y2+1, SITE_ITEM, itemId, item_res.random_para(itemId, random(0x7fff))) )
					{
						itemCreated++;
					}
				}

				if( itemCreated > 0 )
				{
					// ------ add news message ---------//

					if( townPtr->is_own() )
					{
						news_array.event_item_found( townRecno );
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = townPtr->nation_recno;
					last_event_para1 = townRecno;
					break;
				}
			}
		}
		break;

	case RANDOM_EVENT_TOWN_PEOPLE_KILLED:
		{
			int townRecno = random(town_array.size()) + 1;
			for( int t = town_array.size(); t > 0; --t, ++townRecno )
			{
				if( townRecno > town_array.size() )
					townRecno = 1;

				if( town_array.is_deleted(townRecno) )
					continue;

				Town *townPtr = town_array[townRecno];
				if( !townPtr->nation_recno || townPtr->population <= 10 )
					continue;

#ifdef FIXED_NATION
				if( townPtr->nation_recno != FIXED_NATION )
					continue;
#endif
				if( townPtr->under_construction )
					continue;

				int oldPop = townPtr->population;
				for( int k = 10+random(11); k > 0; --k )
					townPtr->kill_town_people();

				if( townPtr->population < oldPop )
				{
					// ------ add news message ---------//

					if( townPtr->is_own() )
					{
						news_array.event_town_people_killed( townRecno, oldPop - townPtr->population );
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = townPtr->nation_recno;
					last_event_para1 = townRecno;
					last_event_para2 = oldPop - townPtr->population;		// number of people died
					break;
				}
			}
		}
		break;

	case RANDOM_EVENT_FACTORY_DESTROYED:
	case RANDOM_EVENT_MINE_DESTROYED:
	case RANDOM_EVENT_INN_DESTROYED:
		{
			int firmRecno = random(firm_array.size()) +1;
			for( int t = firm_array.size(); t > 0; --t, ++firmRecno )
			{
				if( firmRecno > firm_array.size() )
					firmRecno = 1;
				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];
				
				if( !firmPtr->nation_recno || firmPtr->under_construction )
					continue;
#ifdef FIXED_NATION
				if( firmPtr->nation_recno != FIXED_NATION )
					continue;
#endif

				if( firmPtr->under_construction )
					continue;

				int killFlag = 0;
				int peopleKill = 0;

				switch( firmPtr->firm_id )
				{
				case FIRM_FACTORY:
				case FIRM_WAR_FACTORY:
				case FIRM_INCUBATOR:
				case FIRM_MAGIC:
				case FIRM_ALCHEMY:
					killFlag = (eventId == RANDOM_EVENT_FACTORY_DESTROYED);
					break;

				case FIRM_MINE:
					killFlag = (eventId == RANDOM_EVENT_MINE_DESTROYED);
					break;

				case FIRM_INN:
				case FIRM_ANIMAL:
					killFlag = (eventId == RANDOM_EVENT_INN_DESTROYED);
					break;

				}

				if( killFlag )
				{
					if( firmPtr->cast_to_FirmWork() )
					{
						// kill all worker instead of resign them to back to home town
						FirmWork *firmWork = firmPtr->cast_to_FirmWork();
						peopleKill = firmWork->worker_count;
						firmWork->kill_all_worker();
						peopleKill -= firmWork->worker_count;
					}
					firmPtr->hit_points = 0.0f;
					firmPtr->set_repair_flag(0, COMMAND_AUTO);		// make it unsurvivable

					// ------ add news message ---------//

					if( firmPtr->is_own() )
					{
						switch( eventId )
						{
						case RANDOM_EVENT_FACTORY_DESTROYED:
							news_array.event_factory_destroyed(firmRecno, peopleKill);
							break;
						case RANDOM_EVENT_MINE_DESTROYED:
							news_array.event_mine_destroyed(firmRecno, peopleKill);
							break;
						case RANDOM_EVENT_INN_DESTROYED:
							news_array.event_inn_destroyed(firmRecno);
							break;
						}
					}

					// ------- record event happen ------//

					last_event_id = eventId;
					last_event_nation = firmPtr->nation_recno;
					last_event_para1 = firmRecno;
					last_event_para2 = peopleKill;

					break;
				}
			}
		}
		break;

	default:
		err_here();
	}

	// -------- add news nessage --------//

	if( last_event_id && nation_array.player_recno 
		&& (!last_event_nation || last_event_nation == nation_array.player_recno) )
	{
		// BUGHERE
		// add news message
	}

	return last_event_id;
}


int RandomEvent::random(int maxNum)
{
	err_when( maxNum < 0 || maxNum > 0x7FFF );

   #define MULTIPLIER      0x015a4e35L
   #define INCREMENT       1
   #define RANDOM_MAX      0x7FFFU

   random_seed = MULTIPLIER * random_seed + INCREMENT;

   return maxNum * ((random_seed >> 16) & RANDOM_MAX) / (RANDOM_MAX+1);
}



int RandomEvent::create_boundary_unit( int unitId, int nationRecno )
{
	UnitInfo *unitInfo = unit_res[unitId];
	SpriteInfo *spriteInfo = sprite_res[unitInfo->sprite_id];
	int locWidth = spriteInfo->loc_width;
	int locHeight = spriteInfo->loc_height;

	int xMax = MAX_WORLD_X_LOC - locWidth;			// x can be 0 to xMax
	int yMax = MAX_WORLD_Y_LOC - locHeight;		// y can be 0 to yMax

	int xyMax = xMax + yMax;
	int xyxMax = xMax + yMax + xMax;
	int pMax = 2 * xMax + 2 * yMax;					// 4 side minus 4 duplicated corner

	// find a spot on the perimeter

	int p = random( pMax );								// p between 0 to pMax-1

	// 0      1      2  ...  xMax
	// 2xMax+2yMax-1         xMax+1
	// ...                   xMax+2
	// 2xMax+yMax+1          ...
	// 2xMax+yMax ...        xMax+yMax

	for( int t = 16; t > 0; --t, ++p )
	{
		if( p >= pMax )
			p -= pMax;

		int x, y;
		if( p < xMax )
		{
			x = p;
			y = 0;
		}
		else if( p < xyMax )
		{
			x = xMax;
			y = p - xMax;
		}
		else if( p < xyxMax )
		{
			x = xyxMax - p;
			y = yMax;
		}
		else
		{
			x = 0;
			y = pMax - p;
		}
		err_when( x < 0 || x > xMax );
		err_when( y < 0 || y > yMax );

		int outerX1 = x - 4;
		outerX1 = MAX( outerX1, 0 );
		int outerY1 = y - 4;
		outerY1 = MAX( outerY1, 0 );
		int outerX2 = x + 5;
		outerX2 = MIN( outerX2, MAX_WORLD_X_LOC-1 );
		int outerY2 = y + 5;
		outerY2 = MIN( outerY2, MAX_WORLD_Y_LOC-1 );

		int unitRecno;
		if( world.check_unit_space( outerX1, outerY1, outerX2, outerY2, unitInfo->mobile_type) 
			&& (unitRecno = unit_array.add_unit(unitId, nationRecno, RANK_SOLDIER, 100, x, y)) )
		{
			return unitRecno;
		}
	}

	return 0;
}
