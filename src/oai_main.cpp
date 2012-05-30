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

//Filename   : OAI_MAIN.CPP
//Description: AI - main functions

#include <osys.h>
#include <onation.h>
#include <oworld.h>
#include <ogame.h>
#include <ospy.h>
#include <oconfig.h>
#include <ounit.h>
#include <ofirm.h>
#include <otown.h>
#include <otech.h>
#include <otechres.h>
#include <otalkres.h>
#include <of_mine.h>
#include <of_camp.h>
#include <oinfo.h>
#include <olog.h>
#include <oremote.h>
#include <math.h>

//--------- Begin of function Nation::Nation --------//

Nation::Nation() : action_array( sizeof(ActionNode), 30 )
{
   ai_town_array = NULL;
}
//---------- End of function Nation::Nation --------//


//--------- Begin of function Nation::~Nation --------//

Nation::~Nation()
{
   err_when( nation_recno );     // deinit() must be called first before this destructor is called
}
//---------- End of function Nation::~Nation --------//


//--------- Begin of function Nation::init --------//
void Nation::init(int nationType, int raceId, int colorSchemeId, DWORD playerId)
{
   NationBase::init(nationType, raceId, colorSchemeId, playerId);

   //----- init other AI vars -----//

	last_action_id = 0;

   ai_capture_enemy_town_recno = 0;
   ai_capture_enemy_town_start_attack_date = 0;
   ai_last_defend_action_date = 0;

   memset( firm_should_close_array, 0, sizeof(firm_should_close_array) );

   ai_base_town_count = 0;

   attack_camp_count = 0;

   //------ init AI info arrays -----//

   init_all_ai_info();

   //----- init AI personality ----//

	init_personalty();

	//----- set town grant policy ------//

	if( is_ai() )
		set_auto_grant_loyalty(REBEL_LOYALTY+1);	// prevent their people from rebelling
}
//---------- End of function Nation::init --------//


//--------- Begin of function Nation::deinit --------//

void Nation::deinit()
{
	NationBase::deinit();

	deinit_all_ai_info();
}
//---------- End of function Nation::deinit --------//


//--------- Begin of function Nation::init_all_ai_info --------//

void Nation::init_all_ai_info()
{
	err_when( ai_town_array );

   init_ai_info(&ai_town_array, ai_town_count, ai_town_size, AI_TOWN_INIT_SIZE);

   init_ai_info(&ai_base_array, ai_base_count, ai_base_size, AI_BASE_INIT_SIZE);
   init_ai_info(&ai_camp_array, ai_camp_count, ai_camp_size, AI_CAMP_INIT_SIZE);
   init_ai_info(&ai_mine_array, ai_mine_count, ai_mine_size, AI_MINE_INIT_SIZE);
   init_ai_info(&ai_factory_array, ai_factory_count, ai_factory_size, AI_FACTORY_INIT_SIZE);
   init_ai_info(&ai_market_array, ai_market_count, ai_market_size, AI_MARKET_INIT_SIZE);
   init_ai_info(&ai_research_array, ai_research_count, ai_research_size, AI_RESEARCH_INIT_SIZE);
   init_ai_info(&ai_war_array, ai_war_count, ai_war_size, AI_WAR_INIT_SIZE);
   init_ai_info(&ai_inn_array, ai_inn_count, ai_inn_size, AI_INN_INIT_SIZE);
   init_ai_info(&ai_spy_array, ai_spy_count, ai_spy_size, AI_SPY_INIT_SIZE);
   init_ai_info(&ai_special_array, ai_special_count, ai_special_size, AI_SPECIAL_INIT_SIZE);
	// ####### begin Gilbert 4/5 ########//
	init_ai_info(&ai_offend_array, ai_offend_count, ai_offend_size, AI_OFFEND_INIT_SIZE);
	// ####### end Gilbert 4/5 ##########//
	// ####### begin Gilbert 10/3 #######//
   init_ai_info(&ai_lishorr_array, ai_lishorr_count, ai_lishorr_size, AI_LISHORR_INIT_SIZE);
	// ####### end Gilbert 10/3 #######//
   init_ai_info(&ai_mons_train_array, ai_mons_train_count, ai_mons_train_size, AI_MONS_TRAIN_INIT_SIZE);
   init_ai_info(&ai_mons_alchemy_array, ai_mons_alchemy_count, ai_mons_alchemy_size, AI_MONS_ALCHEMY_INIT_SIZE);
   init_ai_info(&ai_mons_fort_array, ai_mons_fort_count, ai_mons_fort_size, AI_MONS_FORT_INIT_SIZE);
   init_ai_info(&ai_mons_animal_array, ai_mons_animal_count, ai_mons_animal_size, AI_MONS_ANIMAL_INIT_SIZE);
   init_ai_info(&ai_incubator_array, ai_incubator_count, ai_incubator_size, AI_INCUBATOR_INIT_SIZE);
   init_ai_info(&ai_mons_magic_array, ai_mons_magic_count, ai_mons_magic_size, AI_MONS_MAGIC_INIT_SIZE);
   init_ai_info(&ai_monster_array, ai_monster_count, ai_monster_size, AI_MONSTER_INIT_SIZE);

   init_ai_info(&ai_caravan_array, ai_caravan_count, ai_caravan_size, AI_CARAVAN_INIT_SIZE);
   init_ai_info(&ai_ship_array, ai_ship_count, ai_ship_size, AI_SHIP_INIT_SIZE);
   init_ai_info(&ai_general_array, ai_general_count, ai_general_size, AI_GENERAL_INIT_SIZE);
}
//---------- End of function Nation::init_all_ai_info --------//


//--------- Begin of function Nation::init_ai_info --------//
//
// <short**> aiInfoArrayPtr  - poniter to the AI info array.
// <short&>  aiInfoCount  - the count of the AI info array.
// <short&>  aiInfoSize   - the size of the AI info array.
// <int>     arrayInitSize - the init size of the array.
//
void Nation::init_ai_info(short** aiInfoArrayPtr, short& aiInfoCount, short& aiInfoSize, int arrayInitSize )
{
   *aiInfoArrayPtr = (short*) mem_add( sizeof(short) * arrayInitSize );

   memset( *aiInfoArrayPtr, 0, sizeof(short) * arrayInitSize );

   aiInfoCount = 0;
   aiInfoSize  = arrayInitSize;
}
//---------- End of function Nation::init_ai_info --------//


//--------- Begin of function Nation::deinit_all_ai_info --------//

void Nation::deinit_all_ai_info()
{
   err_when( !ai_town_array );

   //------- debug checking -------//

#ifdef DEBUG
   if( !sys.signal_exit_flag )
   {
      err_when( ai_town_count > 0 );
      err_when( ai_base_town_count > 0 );

      err_when( ai_base_count > 0 );
      err_when( ai_camp_count > 0 );
      err_when( ai_mine_count > 0 );
      err_when( ai_factory_count > 0 );
      err_when( ai_market_count > 0 );
      err_when( ai_research_count > 0 );
      err_when( ai_war_count > 0 );
      err_when( ai_inn_count > 0 );
      err_when( ai_spy_count > 0 );
      err_when( ai_special_count > 0 );
		// ###### begin Gilbert 4/5 #######//
      err_when( ai_offend_count > 0 );
		// ###### end Gilbert 4/5 ########//
		// ####### begin Gilbert 10/3 #######//
      err_when( ai_lishorr_count > 0 );
		// ####### end Gilbert 10/3 #######//
      err_when( ai_mons_train_count > 0 );
      err_when( ai_mons_alchemy_count > 0 );
      err_when( ai_mons_fort_count > 0 );
      err_when( ai_mons_animal_count > 0 );
      err_when( ai_incubator_count > 0 );
      err_when( ai_mons_magic_count > 0 );
      err_when( ai_monster_count > 0 );

      err_when( ai_caravan_count > 0 );
      err_when( ai_ship_count > 0 );
      err_when( ai_general_count > 0 );
   }
#endif

   //------- release array from memory -------//

   mem_del(ai_town_array);

   mem_del(ai_base_array);
   mem_del(ai_camp_array);
   mem_del(ai_mine_array);
   mem_del(ai_factory_array);
   mem_del(ai_market_array);
   mem_del(ai_research_array);
   mem_del(ai_war_array);
   mem_del(ai_inn_array);
	mem_del(ai_spy_array);
   mem_del(ai_special_array);
	mem_del(ai_offend_array);
	// ####### begin Gilbert 10/3 ############//
	mem_del(ai_lishorr_array);
	// ####### end Gilbert 10/3 ############//
	mem_del(ai_mons_train_array);
	mem_del(ai_mons_alchemy_array);
	mem_del(ai_mons_fort_array);
	mem_del(ai_mons_animal_array);
	mem_del(ai_incubator_array);
	mem_del(ai_mons_magic_array);
   mem_del(ai_monster_array);

   mem_del(ai_caravan_array);
   mem_del(ai_ship_array);
   mem_del(ai_general_array);
}
//---------- End of function Nation::deinit_all_ai_info --------//


//--------- Begin of function Nation::init_personalty --------//
void Nation::init_personalty()
{
   pref_force_projection     = m.random(101);
   pref_military_development = m.random(101);
   pref_economic_development = 100-pref_military_development;
   pref_inc_pop_by_capture   = m.random(101);
   pref_inc_pop_by_growth    = 100-pref_inc_pop_by_capture;
   pref_peacefulness         = m.random(101);
   pref_military_courage     = m.random(101);
   pref_territorial_cohesiveness = m.random(101);
   pref_trading_tendency     = m.random(101);
   pref_allying_tendency     = m.random(101);
   pref_honesty              = m.random(101);
	pref_town_defense         = m.random(101);
   pref_loyalty_concern      = m.random(101);
   pref_forgiveness          = m.random(101);
   pref_collect_tax          = m.random(101);
   pref_hire_unit            = m.random(101);
   pref_use_weapon           = m.random(101);
   pref_keep_general         = m.random(101);
   pref_keep_skilled_unit    = m.random(101);
   pref_diplomacy_retry      = m.random(101);
   pref_attack_monster       = m.random(101);
	pref_spy                  = m.random(101);
	pref_counter_spy          = m.random(101);

	pref_spy = MIN(60, pref_spy);   							// if the pref is too high, the AI will use it like crazy and will introduce weird strategies
	pref_counter_spy = MIN(60, pref_counter_spy);

   pref_cash_reserve         = m.random(101);
   pref_food_reserve         = m.random(101);
   pref_use_marine           = m.random(101);
   pref_unit_chase_distance  = 15+m.random(15);
   pref_repair_concern       = m.random(101);
   pref_scout                = m.random(101);
	pref_research             = m.random(101);
	pref_seat_of_power		  = m.random(101);
	pref_monster_aggressiveness = m.random(101);
	pref_monster_special		  = m.random(101);
	pref_live_points_reserve  = m.random(101);
	pref_surrender				  = m.random(101);
}
//---------- End of function Nation::init_personalty --------//


//--------- Begin of function Nation::process_ai --------//
void Nation::process_ai()
{
	//---- if the king has just been killed ----//

	int nationRecno = nation_recno;

	if( !king_unit_recno )
	{
		if( think_succeed_king() )
			return;

		if( think_surrender() )
			return;

		defeated();
		return;
	}

	//-------- think new research --------//

	if( research_tech_id==0 )
		think_new_research();

	//-------- process main AI actions ---------//

#if defined(ENABLE_LOG)
	String debugStr;
	debugStr = "Nation ";
	debugStr += nation_recno;
#endif

	static short intervalDaysArray[] = { 300, 90, 30, 15, 15 };

	int intervalDays = intervalDaysArray[config.ai_aggressiveness-OPTION_NONE];

	if( game.game_mode == GAME_TUTORIAL )
		intervalDays = 120;

	process_ai_main( (info.game_date-nation_recno*4) % intervalDays );

	LOG_MSG(debugStr);
	LOG_MSG(m.get_random_seed());

	if( nation_array.is_deleted(nationRecno) )      // the nation can have surrendered
      return;

   //------ process queued diplomatic messges first --------//

   if( (info.game_date-nation_recno)%3 == 0 )
   {
      LOG_MSG("begin process_action(0,ACTION_AI_PROCESS_TALK_MSG)");
      process_action(0, ACTION_AI_PROCESS_TALK_MSG);
      LOG_MSG("end process_action(0,ACTION_AI_PROCESS_TALK_MSG)");
      LOG_MSG(m.get_random_seed());

      if( nation_array.is_deleted(nationRecno) )      // the nation can have surrendered
         return;
	}

   //--------- process queued actions ----------//

   if( (info.game_date-nation_recno)%3 == 0 )
   {
      LOG_MSG("begin process_action()");
      process_action();
		LOG_MSG("end process_action()");
      LOG_MSG(m.get_random_seed());

		if( nation_array.is_deleted(nationRecno) )      // the nation can have surrendered
         return;
	}

   //--- process action that are on-going and need continous checking ---//

   process_on_going_action();

	//--------- cheat ---------//
   //
   // In tutorial mode only so that your opponent won't surrender
   // and you won't go to the end game screen.
   //
   //-------------------------//

	// ###### patch begin Gilbert 18/3 #######//
	// decrease money on single player, lowest aggressive
	if( !remote.is_enable() && game.game_mode != GAME_TUTORIAL
		&& config.ai_aggressiveness == OPTION_NONE )
	{
		if( cash > 1000 )
			add_cheat( (float)ceil(-cash/500) );	// reduce fewer -1.x -> -1
	}
	// ###### patch end Gilbert 18/3 #######//

	if( 1 || game.game_mode == GAME_TUTORIAL )	//BUGHERE
	{
      if( cash < 100 )
         add_cheat( (float)200+m.random(500) );

      if( food < 100 )
         food += 1000;
	}

   //----- think about updating relationship with other nations -----//

	if( info.game_date%360 == nation_recno%360 )
		ai_improve_relation();
}
//---------- End of function Nation::process_ai --------//


//------- Begin of function Nation::process_ai_main --------//
//
// Maximum action type: 15, as defined in intervalDaysArray[].
//
int Nation::process_ai_main(int mainActionId)
{
#if defined(ENABLE_LOG)
   String debugStr;
   debugStr = "Nation ";
   debugStr += nation_recno;
#endif

	switch(mainActionId)
	{
		case 0:
#if defined(ENABLE_LOG)
			debugStr += " think_build_firm";
#endif
			think_build_firm();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 1:
#if defined(ENABLE_LOG)
			debugStr += " think_capture";
#endif
			think_capture();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 2:
#if defined(ENABLE_LOG)
			debugStr += " think_explore";
#endif
			think_explore();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 3:        // think about expanding its military force
#if defined(ENABLE_LOG)
			debugStr += " think_military";
#endif
         think_military();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

      case 4:
#if defined(ENABLE_LOG)
         debugStr += " think_secret_attack";
#endif
         think_secret_attack();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 5:
#if defined(ENABLE_LOG)
			debugStr += " think_diplomacy";
#endif
			think_diplomacy();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 6:
#if defined(ENABLE_LOG)
			debugStr += " think_grand_plan";
#endif
			think_grand_plan();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;

		case 7:
#if defined(ENABLE_LOG)
			debugStr += " think_reduce_expense";
#endif
			think_reduce_expense();
			LOG_MSG(debugStr);
			LOG_MSG(m.get_random_seed());
			return 1;
	}

	return 0;
}
//---------- End of function Nation::process_ai_main --------//


//--------- Begin of function Nation::process_on_going_action --------//
//
// Process action that are on-going and need continous checking.
//
void Nation::process_on_going_action()
{
	//--- if the nation is in the process of trying to capture an enemy town ---//

   if( ai_capture_enemy_town_recno )
   {
      if( info.game_date%5 == nation_recno%5 )
			think_capturing_enemy_town();
   }

   //----- if the nation is in the process of attacking a target ----//

	if( attack_camp_count > 0 )
		ai_attack_target_execute(1);
}
//---------- End of function Nation::process_on_going_action --------//


//--------- Begin of function Nation::think_explore --------//

void Nation::think_explore()
{
}
//---------- End of function Nation::think_explore --------//


//-------- Begin of function Nation::think_succeed_king --------//
//
// return: <int> 1 - a unit succeed the king
//               0 - no unit available for succeeding the king,
//                   the nation is defeated.
//
int Nation::think_succeed_king()
{
   int      i, curRating, bestRating=0;
   Unit     *unitPtr, *bestUnitPtr=NULL;
	FirmCamp *firmCamp, *bestFirmCamp=NULL;
   int      bestWorkerId=0;

   //---- try to find the best successor from mobile units ----//

   for( i=unit_array.size() ; i>0 ; i-- )
   {
      if( unit_array.is_deleted(i) )
         continue;

      unitPtr = unit_array[i];

      if( unitPtr->nation_recno != nation_recno || !unitPtr->race_id )
         continue;

      if( !unitPtr->is_visible() && unitPtr->unit_mode != UNIT_MODE_OVERSEE )
			continue;

		if( !unitPtr->can_succeed_king() )
			continue;

		err_when( unitPtr->combat_level() <= 0 );

		curRating = unitPtr->skill_level() + unitPtr->combat_level()/4;

		if( unitPtr->race_id == race_id )
			curRating += 50;

		if( unitPtr->rank_id == RANK_GENERAL )
			curRating += 30;

      if( curRating > bestRating )
      {
         bestRating  = curRating;
         bestUnitPtr = unitPtr;
      }
   }

   //---- try to find the best successor from military camps ----//

   for( i=firm_array.size() ; i>0 ; i-- )
   {
      if( firm_array.is_deleted(i) )
         continue;

      firmCamp = firm_array[i]->cast_to_FirmCamp();

      if( !firmCamp || firmCamp->nation_recno != nation_recno )
         continue;

      //------ only military camps -------//

      Soldier* soldierPtr = firmCamp->soldier_array;

		for(int j=1 ; j<=firmCamp->soldier_count ; j++, soldierPtr++ )
		{
			if( !soldierPtr->can_succeed_king(nation_recno) )
				continue;

			curRating = soldierPtr->skill_level() + soldierPtr->combat_level()/4;

			if( soldierPtr->race_id == race_id )
				curRating += 50;

			if( soldierPtr->rank_id == RANK_GENERAL )
				curRating += 30;

			if( curRating > bestRating )
			{
				bestRating   = curRating;
				bestUnitPtr  = NULL;
				bestFirmCamp  = firmCamp;
				bestWorkerId = j;
			}
		}
	}

	//------- if the best successor is a mobile unit -------//

	if( bestUnitPtr )
	{
		//-- if the unit is in a command base or seat of power, mobilize it --//

		if( !bestUnitPtr->is_visible() )
		{
			err_when( bestUnitPtr->unit_mode != UNIT_MODE_OVERSEE );

			firmCamp = firm_array[bestUnitPtr->unit_mode_para]->cast_to_FirmCamp();

			err_when( !firmCamp );

			firmCamp->mobilize_overseer();

			err_when( bestUnitPtr->combat_level() <= 0 );
		}

		//---------- succeed the king -------------//

		if( bestUnitPtr->is_visible() )     // it may still be not visible if there is no space for the unit to be mobilized
		{
			if( bestUnitPtr->spy_recno && bestUnitPtr->true_nation_recno() == nation_recno )    // if this is a spy and he's our spy
				spy_array[bestUnitPtr->spy_recno]->drop_spy_identity();                          // revert the spy to a normal unit

			succeed_king( bestUnitPtr->sprite_recno );
			return 1;
		}
	}

	//------- if the best successor is a soldier in a camp -------//

	if( bestFirmCamp )
	{
		int unitRecno = bestFirmCamp->mobilize_soldier(bestWorkerId, COMMAND_AI);

		if( unitRecno )
		{
			succeed_king( unitRecno );
			return 1;
		}
	}

	return 0;
}
//---------- End of function Nation::think_succeed_king ---------//


//--------- Begin of function Nation::ai_improve_relation --------//
//
// This function is called once every year.
//
void Nation::ai_improve_relation()
{
   NationRelation* nationRelation;

   for( int i=nation_array.size() ; i>0 ; i-- )
   {
      if( nation_array.is_deleted(i) )
         continue;

      nationRelation = get_relation(i);

      if( nationRelation->status == RELATION_HOSTILE )
         continue;

      //--- It improves the AI relation with nations that have trade with us. ---//

      change_ai_relation_level( i, trade_rating(i) / 10 );

      //--- decrease the started_war_on_us_count once per year, gradually forgiving other nations' wrong doing ---//

      if( nationRelation->started_war_on_us_count > 0
          && m.random(5-pref_forgiveness/20) > 0 )
      {
         nationRelation->started_war_on_us_count--;
      }
   }
}
//---------- End of function Nation::ai_improve_relation --------//


//----- Begin of function Nation::think_new_research ------//
//
// Think about which technology to research.
//
void Nation::think_new_research()
{
	if( ai_research_count==0 || research_tech_id )
		return;

	//--- think if there is any research that we need most now ---//

	int bestTechId=0, bestTechClassId=0;

	//------ if we don't have enough food -------//

	if( tech_res[TECH_FARMING]->can_research(nation_recno) )
	{
		if( !ai_has_enough_food() ||
			 m.random(100-pref_food_reserve) < 10 )
		{
			bestTechId = TECH_FARMING;
		}
	}

	//------- research weapons --------//

	if( !bestTechId && !bestTechClassId )
	{
		if( pref_use_weapon > 50 )
		{
			int totalWeaponTech = tech_res.tech_class(TECH_CLASS_WEAPON)->total_tech_count(nation_recno);

			if( m.random(100 - pref_use_weapon + totalWeaponTech * 3 ) < 25 )
				bestTechClassId = TECH_CLASS_WEAPON;
		}
	}

	//------- research spying methodologies --------//

	if( !bestTechId && !bestTechClassId )
	{
		if( pref_spy > 50 )
		{
			int totalSpyTech = tech_res.tech_class(TECH_CLASS_SPYING)->total_tech_count(nation_recno);

			if( m.random(100 - pref_spy + totalSpyTech * 3 ) < 25 )
				bestTechClassId = TECH_CLASS_SPYING;
		}
	}

	//--- count the jobless population of each race we have ---//

	int bestRaceId=0, maxPop=0;
	int racePopArray[MAX_RACE];

	memset( racePopArray, 0, sizeof(racePopArray) );

	int i;
	for( i=0 ; i<ai_town_count ; i++ )
	{
		Town* townPtr = town_array[ ai_town_array[i] ];

		err_when( townPtr->race_id < 1 || townPtr->race_id > MAX_RACE );

		racePopArray[townPtr->race_id-1] += townPtr->jobless_population;
	}

	//---- research special unit training structures ----//

	if( !bestTechId && !bestTechClassId )
	{
		//--- pick the race with most jobless population and we don't have researched the special training structure for it yet ---//

		for( i=0 ; i<MAX_RACE ; i++ )
		{
			//-- if we haven't research the special unit training structure for this race yet --//

			if( tech_res[TECH_SPU_STRUCTURE(i+1)]->can_research(nation_recno) )
			{
				if( racePopArray[i] > maxPop )
				{
					maxPop 	  = racePopArray[i];
					bestRaceId = i+1;
				}
			}
		}

		if( bestRaceId )
			bestTechId = TECH_SPU_STRUCTURE(bestRaceId);
	}

	//---- randomly pick one by class then by tech in the class ------//

	TechInfo* techInfo;

	if( !bestTechId )
	{
		if( !bestTechClassId )
			bestTechClassId = m.random(tech_res.tech_class_count)+1;

		TechClass* techClass = tech_res.tech_class(bestTechClassId);

		int classSubId = m.random(techClass->tech_count)+1;

		for( int i=techClass->tech_count ; i>0 ; i-- )
		{
			if( ++classSubId > techClass->tech_count )
				classSubId = 1;

			int techId = techClass->first_tech_id + classSubId - 1;

			techInfo = tech_res[techId];

			if( techInfo->can_research(nation_recno) &&
				 ( !techInfo->race_id || racePopArray[techInfo->race_id-1]>0 ) )		// if this tech is race specific, only research it when we have the people of that race
			{
				bestTechId = techId;
				break;
			}
		}
	}

	//------ randomly pick one by individual tech (with race checking) -------//

	if( !bestTechId )
	{
		int techId = m.random(tech_res.tech_count)+1;

		for( int i=tech_res.tech_count ; i>0 ; i-- )
		{
			if( ++techId > tech_res.tech_count )
				techId = 1;

			techInfo = tech_res[techId];

			if( techInfo->can_research(nation_recno) &&
				 ( !techInfo->race_id || racePopArray[techInfo->race_id-1]>0 ) )		// if this tech is race specific, only research it when we have the people of that race
			{
				bestTechId = techId;
				break;
			}
		}
	}

	//------ randomly pick one by individual tech (without race checking) -------//

	if( !bestTechId )
	{
		int techId = m.random(tech_res.tech_count)+1;

		for( int i=tech_res.tech_count ; i>0 ; i-- )
		{
			if( ++techId > tech_res.tech_count )
				techId = 1;

			if( tech_res[techId]->can_research(nation_recno) )
			{
				bestTechId = techId;
				break;
			}
		}
	}

	//------------------------------------//

	if( bestTechId )
		start_research( bestTechId, COMMAND_AI );
}
//------ End of function Nation::think_new_research -------//
