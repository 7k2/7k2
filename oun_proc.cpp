//Filename	 : OUN_PROC.CPP
//Description: Unit functions related to unit processing

#include <OCONFIG.H>
#include <OGAME.H>
#include <OSYS.H>
#include <OSPY.H>
#include <OSITE.H>
#include <OGODRES.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OREMOTE.H>
#include <OUNITRES.H>
#include <ORACERES.H> 
#include <ONATION.H>
#include <OPOWER.H>
#include <ONEWS.H>
#include <OFIRM.H>
#include <OUNIT.H>
#include <ITEMID.H>
#include <OWEATHER.H>

//--------- Begin of function Unit::next_day ---------//
//
void Unit::next_day()
{
	int unitRecno = sprite_recno;

   err_when( unit_array.is_deleted(unitRecno) );

	err_when( race_id && !is_visible() && unit_mode==0 );

	err_when( team_info && !mem.get_mem_size(team_info) );

	// ##### begin Gilbert 2/3 #####//
	err_when( race_id && unit_id != UNIT_WAGON && !name_id );
	// ##### end Gilbert 2/3 #####//

	//------- functions for non-independent nations only ------//

   if( nation_recno )
   {
      pay_expense();

		if( unit_array.is_deleted(unitRecno) )		// if its hit points go down to 0, is_deleted() will return 1.
			return;

		//------- update loyalty -------------//

		if( info.game_date%30 == sprite_recno%30 )
		{
			update_loyalty();
		   err_when( unit_array.is_deleted(unitRecno) );
		}

		//------- think about rebeling -------------//

		if( info.game_date%15 == sprite_recno%15 )
		{
			if( think_betray() )
				return;
		}
	}

	//------- recover from damage -------//

	if( info.game_date%4 == sprite_recno%4 )   // recover points per 4 days 
	{
		process_recover();
		err_when( unit_array.is_deleted(unitRecno) );
	}

      //------- restore cur_power --------//

	cur_power += 5;

	if( cur_power > max_power)
		cur_power = max_power;

	// ####### begin Gilbert 9/5 #########//
	// ------ process special ability --------//

	// if( is_human() && combat_level() >= MAX_COMBAT_BATTLE )
	think_use_special_ability();			// call only in next_day
	// ####### end Gilbert 9/5 #########//

	// ------ process magic effect ---------//

	// ##### patch begin Gilbert 17/1 ########//
	// reduce invulnerable_day_count, if attack on that day
	if( invulnerable_day_count > 10 )
	{
		--invulnerable_day_count;
	}
	else if( invulnerable_day_count > 0 )
	{
		if( last_attack_date==info.game_date || last_attack_date+1==info.game_date )
			--invulnerable_day_count;
	}
	// ##### patch end Gilbert 17/1 ########//

   //------- king undie flag (for testing games only) --------//

   if( config.king_undie_flag && rank_id == RANK_KING &&
       nation_recno && !nation_array[nation_recno]->is_ai() )
   {
		hit_points = (float) max_hit_points();
		// ####### begin Gilbert 27/1 ######//
		if( invulnerable_day_count == 0 )
			++invulnerable_day_count;
		// ####### end Gilbert 27/1 ######//
	}

	//-------- if aggresive_mode is 1 --------//

	if( is_visible() )
		think_aggressive_action();

	// --------- process item -------//
	
	item.next_day();

	//---------- debug ------------//

#ifdef DEBUG
   err_when( race_id &&
		unit_res[unit_id]->unit_class != UNIT_CLASS_HUMAN && 
		unit_res[unit_id]->unit_class != UNIT_CLASS_MONSTER && 
		unit_res[unit_id]->unit_class != UNIT_CLASS_WAGON );

	if( unit_mode == UNIT_MODE_CONSTRUCT_TOWN )
		err_when( town_array[unit_mode_para]->builder_recno != sprite_recno );

	err_when( unit_mode==UNIT_MODE_REBEL && nation_recno );		// rebels must be independent

	//------ debug: spy ----------//

	if( spy_recno )
	{
		err_when( spy_array.is_deleted(spy_recno) );

		Spy* spyPtr = spy_array[spy_recno];

		err_when( nation_recno != spyPtr->cloaked_nation_recno );

		if( unit_mode == UNIT_MODE_OVERSEE )
		{
			err_when( spyPtr->spy_place != SPY_FIRM );
			err_when( spyPtr->spy_place_para != unit_mode_para );
		}
		else
		{
			err_when( spyPtr->spy_place != SPY_MOBILE );
			err_when( spyPtr->spy_place_para != sprite_recno );
		}

		// ####### begin Gilbert 24/2 ########//
		err_when( unique_id != spyPtr->unique_id );
		// ####### end Gilbert 24/2 ########//
	}

	//------ debug: team ----------//

	if( leader_unit_recno )
	{
      Unit* unitPtr = unit_array[leader_unit_recno];

		err_when( unitPtr->rank_id != RANK_GENERAL && unitPtr->rank_id != RANK_KING );
	}

	err_when( (rank_id == RANK_GENERAL || rank_id == RANK_KING) &&
				 !team_info );

	if( team_info )
	{
		for( int i=0 ; i<team_info->member_count ; i++ )
		{
			Unit* unitPtr = unit_array[team_info->member_unit_array[i]];

			if( unitPtr->sprite_recno == sprite_recno )		// the same unit
				continue;

			err_when( unitPtr->leader_unit_recno != sprite_recno );
		}
	}

	if( leader_unit_recno && unit_mode != UNIT_MODE_REBEL )		// in rebel mode, the leader_unit_recno is not linked to team_info
	{
		Unit* leaderUnit = unit_array[leader_unit_recno];

		err_when( !leaderUnit->team_info );

		for( int i=0 ; i<leaderUnit->team_info->member_count ; i++ )
		{
			if( leaderUnit->team_info->member_unit_array[i] == sprite_recno )
				break;
		}

		err_when( i==leaderUnit->team_info->member_count );		// not found

		err_when( unit_array.is_truly_deleted(leader_unit_recno) );
		err_when( unit_array[leader_unit_recno]->nation_recno != nation_recno );
//		err_when( unit_array[leader_unit_recno]->team_id != team_id );
	}

	//------ debug: AI action ----------//

	if( cur_order.ai_action_id )
	{
		Nation* nationPtr = nation_array[nation_recno];

		for( int actionRecno=nationPtr->action_count() ; actionRecno>0 ; actionRecno-- )
		{
			if( nationPtr->is_action_deleted(actionRecno) )
				continue;

			ActionNode* actionNode = nationPtr->get_action(actionRecno);

			if( cur_order.ai_action_id == actionNode->action_id )
			{
				err_when( actionNode->processing_instance_count<1 );
				break;
			}
		}
	}

	//---------------------------------//

	err_when( hit_points > max_hit_points() );
	err_when( max_hit_points() == 0 );

   err_when( combat_level()<0 );
	err_when( combat_level()>1000 );

	err_when( unit_mode==UNIT_MODE_REBEL && spy_recno );			// no rebel spies
	err_when( unit_mode==UNIT_MODE_REBEL && nation_recno );		// all rebels must be independent units

	err_when( unit_mode==UNIT_MODE_TOWN_DEFENDER && spy_recno );			// no rebel spies

	err_when( loyalty < 0 || loyalty > 100 );

	err_when( nation_contribution < 0 );
	err_when( nation_contribution > MAX_NATION_CONTRIBUTION );

	err_when( is_ai && ( !nation_recno || !nation_array[nation_recno]->is_ai() ) );

#else		// fix bug on fly in the release version

	// ######## begin Gilbert 5/2 #####//
//	if( combat_level() > skill.max_combat_level )
//		combat_level() = skill.max_combat_level;
	if( skill.actual_combat_level(NULL) > skill.max_combat_level )		// raw combat level
		set_combat_level( skill.max_combat_level );
	// ######## end Gilbert 5/2 #####//

#endif
}
//----------- End of function Unit::next_day -----------//

//--------- Begin of function Unit::process_recover ---------//
//
void Unit::process_recover()
{
	if( hit_points==0.0f || hit_points == max_hit_points() )      // this unit is dead already
		return;

	err_when( hit_points > max_hit_points() );

#define RECOVER_SLOW_DOWN 0.5f

	float hitPointsInc = 0.0f;

	
	//---- overseers in firms and ships in harbors recover faster ----//
	
	if( unit_mode == UNIT_MODE_OVERSEE ||
		 unit_mode == UNIT_MODE_IN_HARBOR )
	{
		hitPointsInc = 2.0f * RECOVER_SLOW_DOWN;
	}

	//------ for units on ships --------//

	else if( unit_mode == UNIT_MODE_ON_SHIP )
	{
		//--- if the ship where the unit is on is in the harbor, the unit recovers faster ---//

		if( unit_array[unit_mode_para]->unit_mode == UNIT_MODE_IN_HARBOR )
			hitPointsInc = 2.0f * RECOVER_SLOW_DOWN;
		else
			hitPointsInc = 1.0f * RECOVER_SLOW_DOWN;
	}

	//----- only recover when the unit is not moving -----//

	else if( cur_action == SPRITE_IDLE )
	{
		hitPointsInc = 1.0f * RECOVER_SLOW_DOWN;
	}
	else
		hitPointsInc = 0;


	// --------- recovery bonus --------//

	if( hitPointsInc > 0.0f )
	{
		// high max_hit_points recover faster

		hitPointsInc += RECOVER_SLOW_DOWN * max_hit_points() / 200;
	}

	if( unit_id == UNIT_PLANT && weather.cloud() && is_visible() )
		hitPointsInc += 3.0f * RECOVER_SLOW_DOWN;
	
	// ------- bonus from item  ---------//

	hitPointsInc += RECOVER_SLOW_DOWN * item.ability(ITEM_ABILITY_RECOVERY);

	//---------- recover now -----------//

	if( hitPointsInc > 0 )
	{
		hit_points += hitPointsInc;

		if( hit_points > max_hit_points() )
	      hit_points = (float) max_hit_points();
	}
}
//----------- End of function Unit::process_recover -----------//


//--------- Begin of function Unit::update_loyalty ---------//
//
// How loyalty of units are updated:
//
// General: in a military camp - updated in FirmCamp::update_loyalty()
//          mobile - no update
//
// Soldiers led by a general: in a military camp - updated in FirmCamp::update_loyalty()
//                            mobile - updated here
//
// Other units: no update.
//
void Unit::update_loyalty()
{
	if( !nation_recno || rank_id==RANK_KING || !race_id )
		return;

	if( unit_mode == UNIT_MODE_CONSTRUCT_FIRM || 		// constructor worker will not change their loyalty when they are in a building
		 unit_mode == UNIT_MODE_CONSTRUCT_TOWN )
	{
		return;
   }

	//--- the loyalty of civilians does not change when they are mobile ---//

	if( is_civilian() )
	{
		target_loyalty = loyalty;
		return;
	}

	//-------- if this is a general ---------//

	Nation* ownNation = nation_array[nation_recno];
	int 	  rc=0;

	if( rank_id==RANK_GENERAL )
	{
		//----- the general's power affect his loyalty ----//

		int targetLoyalty = commander_power();

		//----- the king's race affects the general's loyalty ----//

		if( ownNation->race_id == race_id )
			targetLoyalty += 20;

		//----- the kingdom's reputation affects the general's loyalty ----//

		targetLoyalty += (int)ownNation->reputation/4;

		//--- the king's leadership also affect the general's loyalty -----//

		if( ownNation->king_unit_recno )
			targetLoyalty += unit_array[ownNation->king_unit_recno]->skill_level() / 4;

		//-- if the unit is rewarded less than the amount of contribution he made, he will become unhappy --//

		if( nation_contribution > total_reward*2 )
		{
			int decLoyalty = (nation_contribution - total_reward*2)/2;
			targetLoyalty -= min(50, decLoyalty);		// this affect 50 points at maximum
		}

		targetLoyalty = min( targetLoyalty, 100 );
		target_loyalty = max( targetLoyalty, 0 );

		//----- if this unit is a spy, set its fake loyalty ------//

		if( spy_recno )		// it should never go below the rebel level
			target_loyalty = max( 30+sprite_recno%10, target_loyalty );
	}

	//-------- if this is a soldier ---------//

	else if( rank_id==RANK_SOLDIER )
	{
		if( leader_unit_recno )
		{
			//----------------------------------------//
			//
			// If this soldier is led by a general,
			// the targeted loyalty
			//
			// = race friendliness between the unit and the general / 2
			//   + the leader unit's leadership / 2
			//
			//----------------------------------------//

			if( unit_array.is_deleted(leader_unit_recno) )
			{
				leader_unit_recno = 0;
				return;
			}

			Unit* leaderUnit = unit_array[leader_unit_recno];

			int targetLoyalty = 30 + leaderUnit->skill_level();

			//---------------------------------------------------//
			//
			// Soldiers with higher combat and leadership skill
			// will get discontented if they are led by a general
			// with low leadership.
			//
			//---------------------------------------------------//

			targetLoyalty -= combat_level()/2;
			targetLoyalty -= skill_level();

			if( leaderUnit->rank_id == RANK_KING )
				targetLoyalty += 20;

			if( race_res.is_same_race(race_id, leaderUnit->race_id) )
				targetLoyalty += 20;

			if( targetLoyalty < 0 )
				targetLoyalty = 0;

			targetLoyalty = min( targetLoyalty, 100 );
			target_loyalty = max( targetLoyalty, 0 );
		}
		else
		{
			target_loyalty = 0;
		}
	}

	//--------- update loyalty ---------//

	err_when( target_loyalty < 0 || target_loyalty > 100 );

	if( target_loyalty > loyalty )      // only increase, no decrease. Decrease are caused by events. Increases are made gradually
	{
		int incValue = (target_loyalty - loyalty)/10;

		int newLoyalty = (int) loyalty + max(1, incValue);

		if( newLoyalty > target_loyalty )
			newLoyalty = target_loyalty;

		loyalty = newLoyalty;
	}
	else if( target_loyalty < loyalty )      // only increase, no decrease. Decrease are caused by events. Increases are made gradually
	{
		loyalty--;
	}

	err_when( loyalty < 0 || loyalty > 100 );
}
//-------- End of function Unit::update_loyalty -----------//


//--------- Begin of function Unit::king_die ---------//
//
void Unit::king_die()
{
	//----- drop power of scroll when the king dies -----//

	int raceId = m.random(MAX_RACE)+1;

	for( int i=1 ; i<MAX_RACE ; i++ )
	{
		if( ++raceId > MAX_RACE )
			raceId = 1;

		if( god_res[raceId]->is_nation_know( nation_recno ) )
			break;
	}

	if( i<MAX_RACE )
	{
		short xLoc, yLoc;

		if( get_cur_loc(xLoc, yLoc) )
			site_array.add_site( xLoc, yLoc, SITE_SCROLL, raceId );

		god_res[raceId]->disable_know(nation_recno);		// the nation lost its scroll
	}

	// ---- drop scroll of weapon ------//

	TechClass *techClass = tech_res.tech_class(TECH_CLASS_MEGA_WEAPON);
	if( techClass->tech_count > 0 )
	{
		int techId = techClass->first_tech_id + m.random(techClass->tech_count);

		for( i=1 ; i<=techClass->tech_count; i++ )
		{
			if( ++techId > techClass->first_tech_id + techClass->tech_count - 1 )
				techId = techClass->first_tech_id;

			if( tech_res[techId]->get_nation_tech_level(nation_recno) )
				break;
		}

		if( i<=techClass->tech_count )
		{
			short xLoc, yLoc;

			if( get_cur_loc(xLoc, yLoc) && site_array.add_site( xLoc, yLoc, SITE_WEAPON_BLUEPRINT, techId) )
			{
				tech_res[techId]->disable_know(nation_recno);		// the nation lost its scroll
			}
		}
	}

   //--------- add news ---------//

	news_array.king_die(nation_recno);

   //--- see if the units, firms and towns of the nation are all destroyed ---//

   Nation* nationPtr = nation_array[nation_recno];

	nationPtr->king_unit_recno = 0;
}
//----------- End of function Unit::king_die -----------//


//--------- Begin of function Unit::general_die ---------//
//
void Unit::general_die()
{
   //--------- add news ---------//

   if( nation_recno == nation_array.player_recno )
      news_array.general_die(sprite_recno);

	// ####### begin Gilbert 17/6 ##########//
	// news message for independent fryhtan
	if( nation_recno == 0 && is_monster() )
	{
		news_array.monster_king_killed( monster_id(), next_x_loc(), next_y_loc() );
	}
	// ####### end Gilbert 17/6 ##########//
}
//----------- End of function Unit::general_die -----------//


//--------- Begin of function Unit::think_betray ---------//
//
int Unit::think_betray()
{
	int unitRecno = sprite_recno;

	err_when( unit_array.is_deleted(unitRecno) );

	if( !race_id )
		return 0;

	if( unit_mode==UNIT_MODE_TOWN_DEFENDER || unit_mode==UNIT_MODE_CAMP_DEFENDER )
		return 0;

	if( spy_recno )			// spies do not betray here, spy has its own functions for betrayal
		return 0;

	//----- if the unit is in training or is constructing a building, do not rebel -------//

	if( !is_visible() && unit_mode != UNIT_MODE_OVERSEE )
		return 0;

	if( loyalty >= UNIT_BETRAY_LOYALTY )      // you when unit is
		return 0;

	if( !unit_res[unit_id]->race_id || !nation_recno ||
		 rank_id==RANK_KING || spy_recno )
	{
		return 0;
	}

	err_when(unit_res[unit_id]->unit_class == UNIT_CLASS_GOD);
	err_when(unit_id==UNIT_CARAVAN);

	//------ turn towards other nation --------//

	int    i, bestNationRecno=0, nationScore, bestScore=loyalty;      // the score must be larger than the current loyalty
	Nation *curNation, *nationPtr;
	int	 unitRegionId = region_id();

	if( loyalty==0 )        // if the loyalty is 0, it will definitely betray
		bestScore = -100;

	curNation = nation_array[nation_recno];

	for( i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		if( !curNation->get_relation(i)->has_contact || i==nation_recno )
			continue;

		nationPtr = nation_array[i];

		if( nationPtr->is_human() != is_human() )
			continue;

		//--- only if the nation has a base town in the region where the unit stands ---//

		if( !region_array.nation_has_base_town(unitRegionId, i) )
			continue;

		//------------------------------------------------//

		nationScore = (int) nationPtr->reputation
						  + (nationPtr->overall_rating - curNation->overall_rating);

		if( race_res.is_same_race(nationPtr->race_id, race_id) )
			nationScore += 30;

		if( nationScore > bestScore )
		{
			bestScore       = nationScore;
			bestNationRecno = i;
		}
	}

	err_when( unit_array.is_deleted(unitRecno) );

	if( bestNationRecno )
	{
		return betray(bestNationRecno);
	}
	else if( loyalty==0 )
	{
		//----------------------------------------------//
		// If there is no good nation to turn towards to and
		// the loyalty has dropped to 0, resign itself and
		// leave the nation.
		//
		// However, if the unit is spy, it will stay with the
		// nation as it has never been really loyal to the nation.
		//---------------------------------------------//

		if( rank_id != RANK_KING && is_visible() &&
			 !spy_recno )
		{
			resign(COMMAND_AUTO);
			return 1;
		}
	}

	return 0;
}
//-------- End of function Unit::think_betray -----------//

//--------- Begin of function Unit::betray ---------//
//
// If this unit is a spy, this function betray() will be
// called by Unit::spy_change_nation() or Firm::capture_firm().
//
// If this is not a spy, this function will only be called
// by think_betray() and other nation deinit functions.
//
int Unit::betray(int newNationRecno)
{
	//----- if this is a spy, call spy_change_nation -----//

	if( spy_recno &&
		 spy_array[spy_recno]->cloaked_nation_recno != newNationRecno ) 	// cloaked_nation_recno == newNationRecno if betray() is called by spy_change_nation() already
	{
		spy_change_nation(newNationRecno, COMMAND_AUTO);
		return 1;
	}

	//-------------------------------//

	err_when( newNationRecno && nation_array[newNationRecno]->is_human() &&		// monsters in firms should not betray to humans
			    is_monster() && unit_mode == UNIT_MODE_OVERSEE );		

	int unitRecno = sprite_recno;

	err_when( unit_array.is_truly_deleted(unitRecno) );

	err_when( rank_id == RANK_KING );

	if( nation_recno == newNationRecno )
		return 0;

	if( unit_mode == UNIT_MODE_CONSTRUCT_FIRM ||	 // don't change nation when the unit is constructing a firm
		 unit_mode == UNIT_MODE_CONSTRUCT_TOWN ||	 
		 unit_mode == UNIT_MODE_ON_SHIP   )  	// don't change nation when the unit is constructing a firm
	{
		return 0;
	}

	//--- special case: units in Monster Fortress cannot change nation ---//

	if( unit_mode == UNIT_MODE_OVERSEE &&
		 firm_array[unit_mode_para]->firm_id == FIRM_FORTRESS )
	{
		return 0;
	}

	//---------- add news -----------//

	if( nation_recno == nation_array.player_recno ||
		 newNationRecno == nation_array.player_recno )
	{
		//--- if this is a spy, don't display news message for betrayal as it is already displayed in Unit::spy_change_nation() ---//

		if( !spy_recno )
			news_array.unit_betray(sprite_recno, newNationRecno);
	}

	//------ change nation now ------//

	err_when( unit_array.is_truly_deleted(unitRecno) );

	change_nation(newNationRecno);

	err_when( unit_array.is_truly_deleted(unitRecno) );

	//-------- set the loyalty of the unit -------//

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		loyalty = UNIT_BETRAY_LOYALTY + 10 + m.random(20);

		if( nationPtr->reputation > 0 )
			change_loyalty( (int) nationPtr->reputation );

		if( race_res.is_same_race( nationPtr->race_id, race_id ) )
			change_loyalty( 30 );

		err_when( loyalty < 0 || loyalty > 100 );

		update_loyalty();		// update target loyalty
	}
	else  //------ if change to independent rebel -------//
	{
		loyalty = 0;      // no loyalty needed
	}

	//--- if this unit is a general, change nation for the units he commands ---//

	if( rank_id==RANK_GENERAL )
	{
		err_when( !team_info );

		for( int i=0 ; i<team_info->member_count ; i++ )
		{
			int memberUnitRecno = team_info->member_unit_array[i];

			if( memberUnitRecno == unitRecno )		// this is the unit itself
				continue;

			Unit* unitPtr = unit_array[memberUnitRecno];

			if( !unitPtr->is_visible() )
				continue;

			if( unitPtr->spy_recno )		// if the unit is a spy
				unitPtr->spy_change_nation(newNationRecno, COMMAND_AUTO);
			else
				unitPtr->change_nation(newNationRecno);
		}
	}

	err_when( unit_array.is_truly_deleted(unitRecno) );

	//------ go to meet the new master -------//

	if( is_visible() && nation_recno )
	{
		if( !spy_recno || spy_array[spy_recno]->notify_cloaked_nation_flag )
		{
			if( is_civilian() )
				ai_move_to_nearby_town();
			else
				ai_move_to_nearby_firm(FIRM_CAMP, FIRM_FORT);
		}
	}

	err_when( unit_array.is_truly_deleted(unitRecno) );

	return 1;
}
//-------- End of function Unit::betray -----------//


//--------- Begin of function Unit::change_nation ---------//
//
// This function is called when a unit change nation.
// It is not necessarily a result of betray, when a spy
// hands over his new nation to his parent nation, this
// function will also be called.
//
// <int> newNationRecno - change the nation of the unit.
//
void Unit::change_nation(int newNationRecno)
{
	err_when( newNationRecno && nation_array.is_deleted(newNationRecno) );
	err_when( unit_mode == UNIT_MODE_REBEL );		// rebels do not change nation

	//--- special case: units in Monster Fortress cannot change nation ---//

	if( unit_mode == UNIT_MODE_OVERSEE &&
		 firm_array[unit_mode_para]->firm_id == FIRM_FORTRESS )
	{
		return;
	}

	//---------------------------------//

	int oldAiUnit = is_ai;
	int oldNationRecno = nation_recno;

	//-- if the player is giving a command to this unit, cancel the command --//

   if( nation_recno == nation_array.player_recno &&
		 sprite_recno == unit_array.selected_recno &&
		 power.command_id )
   {
		power.command_id = 0;
   }

	//---------- stop all action to attack this unit ------------//

	unit_array.stop_attack_obj(base_obj_recno);

	//---- update nation_unit_count_array[] ----//

	unit_res[unit_id]->unit_change_nation(newNationRecno, nation_recno, rank_id);

	//------- if the nation has an AI action -------//

	stop_order();       // clear the existing order

	//---------------- update vars ----------------//

//	unit_group_id = unit_array.cur_group_id++;      // separate from the current group
	nation_recno  = newNationRecno;

	home_camp_firm_recno  = 0;					// reset it

	if( race_id )
	{
		nation_contribution = 0;            // contribution to the nation
		total_reward        = 0;
	}
	
	// #### begin Gilbert 24/12 #######//

	// -------- reset royal -------//

	is_royal = 0;

	// #### end Gilbert 24/12 #######//

	//-------- if change to one of the existing nations ------//

	is_ai = nation_recno && nation_array[nation_recno]->is_ai();

	//------------ update AI info --------------//

	if( oldNationRecno )
	{
		Nation* nationPtr = nation_array[oldNationRecno];

		if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
			nationPtr->del_general_info(sprite_recno);

		else if( unit_res[unit_id]->unit_class == UNIT_CLASS_CARAVAN )
			nationPtr->del_caravan_info(sprite_recno);

		else if( unit_res[unit_id]->unit_class == UNIT_CLASS_SHIP )
			nationPtr->del_ship_info(sprite_recno);
	}

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
			nationPtr->add_general_info(sprite_recno);

		else if( unit_res[unit_id]->unit_class == UNIT_CLASS_CARAVAN )
			nationPtr->add_caravan_info(sprite_recno);

		else if( unit_res[unit_id]->unit_class == UNIT_CLASS_SHIP )
			nationPtr->add_ship_info(sprite_recno);
	}

	//------ if this unit oversees a firm -----//

	if( unit_mode==UNIT_MODE_OVERSEE )
		firm_array[unit_mode_para]->change_nation(newNationRecno);

	//----- this unit was defending the town before it gets killed ----//

	else if( unit_mode==UNIT_MODE_TOWN_DEFENDER )
	{
		if( !town_array.is_deleted(unit_mode_para) )
			town_array[unit_mode_para]->reduce_defender_count();

		set_mode(0);   // reset unit mode
	}

	//---- if the unit is no longer the same nation as the leader ----//

	if( leader_unit_recno )
	{
		Unit* leaderUnit = unit_array[leader_unit_recno];

		if( leaderUnit->nation_recno != nation_recno )
		{
			err_when( !leaderUnit->team_info );
			leaderUnit->team_info->del_member(sprite_recno);
			leader_unit_recno = 0;
//			team_id = 0;
		}
	}

	// ##### begin Gilbert 9/2 #######//
	explore_area();
	// ##### end Gilbert 9/2 #######//

	//------ if it is currently selected -------//

	if( selected_flag )
		info.disp();

	err_when( spy_recno && spy_array[spy_recno]->cloaked_nation_recno != nation_recno );

	if( spy_recno && spy_array[spy_recno]->cloaked_nation_recno != nation_recno )		// BUGHERE - fix bug on-fly
		spy_array[spy_recno]->cloaked_nation_recno = nation_recno;
}
//----------- End of function Unit::change_nation -----------//


//--------- Begin of function Unit::pay_expense ---------//
//
void Unit::pay_expense()
{
   if( game.game_mode == GAME_TEST )      // no deduction in testing game
      return;

   if( !nation_recno )
      return;

   //--- if it's a mobile spy or the spy is in its own firm, no need to pay salary here as Spy::pay_expense() will do that ---//
   //
   // -If your spies are mobile:
   //  >your nation pays them 1 food and $5 dollars per month
   //
   // -If your spies are in an enemy's town or firm:
   //  >the enemy pays them 1 food and the normal salary of their jobs.
   //
   //  >your nation pays them $5 dollars per month. (your nation pays them no food)
   //
   // -If your spies are in your own town or firm:
   //  >your nation pays them 1 food and $5 dollars per month
   //
   //------------------------------------------------------//

	if( spy_recno )
   {
      if( is_visible() )      // the cost will be deducted in spy_array
         return;

      if( unit_mode == UNIT_MODE_OVERSEE &&
          firm_array[unit_mode_para]->nation_recno == true_nation_recno() )
      {
         return;
      }
   }

   //---------- if it's a human unit -----------//
   //
   // The unit is paid even during its training period in a town
   //
   //-------------------------------------------//

   Nation* nationPtr = nation_array[nation_recno];

	if( unit_res[unit_id]->race_id > 0 )
	{
		if( rank_id != RANK_KING )
		{
			//---------- reduce cash -----------//

			if( nationPtr->cash > 0 )
			{
				if( rank_id == RANK_SOLDIER )
					nationPtr->add_expense( EXPENSE_MOBILE_UNIT, (float) SOLDIER_YEAR_SALARY / 365, 1 );

				if( rank_id == RANK_GENERAL )
					nationPtr->add_expense( EXPENSE_GENERAL, (float) GENERAL_YEAR_SALARY / 365, 1 );
			}

			//---------- reduce food -----------//

			if( nationPtr->food > 0 )
				nationPtr->consume_food((float) UNIT_FOOD_YEAR_CONSUMPTION / 365);
			else
			{
				if( info.game_date%NO_FOOD_LOYALTY_DECREASE_INTERVAL == 0 )		// decrease 1 loyalty point every 2 days
					change_loyalty(-1);
			}
		}
	}
	else if( unit_res[unit_id]->is_monster() )
	{
		//--- currently not cost for monsters ----//
	}
	else  //----- it's a non-human unit ------//
	{
		if( nationPtr->cash > 0 )
		{
			int expenseType;

			switch(unit_res[unit_id]->unit_class)
			{
				case UNIT_CLASS_WEAPON:
					expenseType = EXPENSE_WEAPON;
					break;

				case UNIT_CLASS_SHIP:
					err_here();
//					expenseType = EXPENSE_SHIP;
					break;

				case UNIT_CLASS_CARAVAN:
					expenseType = EXPENSE_CARAVAN;
					break;

				default:
					expenseType = EXPENSE_MOBILE_UNIT;
			}

			nationPtr->add_expense( expenseType, (float) unit_res[unit_id]->year_cost / 365, 1 );
		}
		else     // decrease hit points if the nation cannot pay the unit
		{
			if( unit_res[unit_id]->unit_class != UNIT_CLASS_CARAVAN )		// Even when caravans are not paid, they still stay in your service.
			{
				if( hit_points > 0 )
				{
					hit_points--;

					if( hit_points < 0 )
						hit_points = (float) 0;

					//--- when the hit points drop to zero and the unit is destroyed ---//

					if( hit_points==0 )
					{
						if( nation_recno == nation_array.player_recno )
						{
							int unitClass = unit_res[unit_id]->unit_class;

							// ###### begin Gilbert 24/3 #######//
							if( unitClass==UNIT_CLASS_WEAPON || unitClass==UNIT_CLASS_MONS_WEAPON )
								news_array.weapon_ship_worn_out(unit_id, get_weapon_version());
							// ###### end Gilbert 24/3 #######//

							else if( unitClass==UNIT_CLASS_SHIP )
								news_array.weapon_ship_worn_out(unit_id, 0);
						}
					}
				}
			}
      }
   }
}
//----------- End of function Unit::pay_expense -----------//


//--------- Begin of function Unit::change_hit_points ---------//
//
void Unit::change_hit_points(float changePoints)
{
   hit_points += changePoints;

   if( hit_points < 0 )
      hit_points = (float) 0;

	if( hit_points > max_hit_points() )
      hit_points = (float) max_hit_points();
}
//-------- End of function Unit::change_hit_points -----------//


//--------- Begin of function Unit::change_loyalty ---------//
//
// <int> changeAmt - amount of loyalty to be changed.
//
void Unit::change_loyalty(int changeAmt)
{
   int newLoyalty = loyalty + changeAmt;

   newLoyalty = max(0, newLoyalty);

   loyalty = min(100, newLoyalty);
}
//----------- End of function Unit::change_loyalty -----------//


//--------- Begin of function Unit::reward ---------//
//
// <int> rewardNationRecno - the nation which does this reward.
//
void Unit::reward(int rewardNationRecno)
{
	if( nation_array[rewardNationRecno]->cash < REWARD_COST )
		return;

	// ###### begin Ban 5/5 ######//
	if (rank_id == RANK_KING || !unit_res[unit_id]->class_info.has_loyalty )
			return;
	// ###### end Ban 5/5 ######//

   //--------- if this is a spy ---------//

   if( spy_recno && true_nation_recno() == rewardNationRecno )    // if the spy's owning nation rewards the spy
   {
      spy_array[spy_recno]->change_loyalty(REWARD_LOYALTY_INCREASE);
	}

	//--- if this spy's nation_recno & true_nation_recno() are both == rewardNationRecno, it's true loyalty and cloaked loyalty will both be increased ---//

	if( nation_recno == rewardNationRecno )
	{
		total_reward += REWARD_COST;

		change_loyalty(REWARD_LOYALTY_INCREASE);
	}

   nation_array[rewardNationRecno]->add_expense(EXPENSE_REWARD_UNIT, (float)REWARD_COST);
}
//----------- End of function Unit::reward -----------//


//--------- Begin of function Unit::spy_change_nation ---------//
//
// Change the deceiving nation recno of a spy unit which you control.
//
// <int>  newNationRecno - the new nation the spy changes its cloack to 
// <char> remoteAction   - remote action type 
//
void Unit::spy_change_nation(int newNationRecno, char remoteAction)
{
	if( newNationRecno == nation_recno )
		return;

	if( newNationRecno && nation_array.is_deleted(newNationRecno) )      // this can happen in a multiplayer message
		return;

	//------- if this is a remote action -------//

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure <unit recno> <new nation Recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_SPY_NATION, 2*sizeof(short) );
		*shortPtr = sprite_recno;
		shortPtr[1] = newNationRecno;
		return;
	}

	//----- update the var in Spy ------//

	Spy* spyPtr = spy_array[spy_recno];

	//--- when a spy change cloak to another nation, he can't cloak as a general, he must become a soldier first ---//

	if( is_visible() &&					// if the spy is a commander in a camp, don't set its rank to soldier
		 rank_id == RANK_GENERAL &&
		 newNationRecno != spyPtr->true_nation_recno )
	{
		set_rank(RANK_SOLDIER);
	}

	//---------------------------------------------------//
	//
	// If this spy unit is a general or an overseer of the
	// cloaked nation, when he changes nation, that will
	// inevitably be noticed by the cloaked nation.
	//
	//---------------------------------------------------//

	if( spyPtr->true_nation_recno != nation_array.player_recno )	// only send news message if he is not the player's own spy
	{
		if( rank_id == RANK_GENERAL || unit_mode == UNIT_MODE_OVERSEE ||
			 spyPtr->notify_cloaked_nation_flag )
		{
			//-- if this spy's cloaked nation is the player's nation, the player will be notified --//

			if( nation_recno == nation_array.player_recno )
				news_array.unit_betray(sprite_recno, newNationRecno);
		}

		//---- send news to the cloaked nation if notify flag is on ---//

		if( spyPtr->notify_cloaked_nation_flag )
		{
			if( newNationRecno == nation_array.player_recno )    			// cloaked as the player's nation
				news_array.unit_betray(sprite_recno, newNationRecno);
		}
	}

	//--------- change nation recno now --------//

	spyPtr->cloaked_nation_recno = newNationRecno;

	betray(newNationRecno);		// call the betray function to change natino. There is no difference between a spy changing nation and a unit truly betrays
}
//----------- End of function Unit::spy_change_nation -----------//


//--------- Begin of function Unit::can_spy_change_nation ---------//
//
// Whether the spy unit can change its spy cloak now or not.
//
// If there are enemy nearby, the unit cannot change its cloak.
//
int Unit::can_spy_change_nation()
{
	if( !spy_recno )
		return 0;

	//--------------------------------------------//

	int xLoc1=cur_x_loc()-SPY_ENEMY_RANGE, yLoc1=cur_y_loc()-SPY_ENEMY_RANGE;
	int xLoc2=cur_x_loc()+SPY_ENEMY_RANGE, yLoc2=cur_y_loc()+SPY_ENEMY_RANGE;

	xLoc1 = max(0, xLoc1);
	yLoc1 = max(0, yLoc1);
	xLoc2 = min(MAX_WORLD_X_LOC-1, xLoc2);
	yLoc2 = min(MAX_WORLD_Y_LOC-1, yLoc2);

	int       xLoc, yLoc;
	int       unitRecno, trueNationRecno = true_nation_recno();
   Location* locPtr;

   for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
   {
      locPtr = world.get_loc(xLoc1, yLoc);

      for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
      {
			if( locPtr->unit_recno(UNIT_LAND) )
            unitRecno = locPtr->unit_recno(UNIT_LAND);

			else if( locPtr->unit_recno(UNIT_SEA) )
				unitRecno = locPtr->unit_recno(UNIT_SEA);

			else if( locPtr->unit_recno(UNIT_AIR) )
            unitRecno = locPtr->unit_recno(UNIT_AIR);

         else
            continue;

         if( unit_array.is_deleted(unitRecno) )    // the unit is dying, its recno is still in the location
            continue;

			Unit* unitPtr = unit_array[unitRecno];

			if( unitPtr->true_nation_recno() != trueNationRecno &&
				 unitPtr->nation_recno != trueNationRecno )
			{
				return 0;
			}
		}
	}

	return 1;
}
//----------- End of function Unit::can_spy_change_nation -----------//


//--------- Begin of function Unit::resign ---------//
//
// Resign the unit.
//
void Unit::resign(int remoteAction)
{
   if( !remoteAction && remote.is_enable() )
   {
      // packet structure : <unit recno> <nation recno>
      short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_RESIGN, 2*sizeof(short));
      *shortPtr = sprite_recno;
		shortPtr[1] = nation_array.player_recno;

      return;
   }

   //---- increase the wandering count when a unit is disbanded ----//

   if( is_human() )
		town_array.race_wander_pop_array[race_id-1] += 2;     // disbanding one resulted in two wandering units to make the effect more significant

	//--- if the unit is visible, stop the current order ----//

	if( is_visible() )
		stop_order();

	//--- if the spy is resigned by an enemy, display a message ---//

	if( spy_recno && true_nation_recno() != nation_recno )		// the spy is cloaked in an enemy nation when it is resigned
	{
		//------ decrease reputation ------//

		nation_array[true_nation_recno()]->change_reputation((float)-SPY_KILLED_REPUTATION_DECREASE);

		//------- add news message -------//
		// #### begin Gilbert 24/12 ######//
//		if( nation_recno == nation_array.player_recno )		// display when the player has revealed an enemy spy
//			info.spy_execute = 1;
		// #### end Gilbert 24/12 ######//


		if( true_nation_recno() == nation_array.player_recno ||		// display when the player's spy is revealed or the player has revealed an enemy spy
			 nation_recno == nation_array.player_recno )
		{
			//--- If a spy is caught, the spy's nation's reputation wil decrease ---//

			news_array.spy_killed(spy_recno);
		}
	}

	//----------------------------------------------//

	if( rank_id == RANK_GENERAL )			// if this is a general, news_array.general_die() will be called, set news_add_flag to 0 to suppress the display of thew news
		news_array.news_add_flag=0;

	// #### patch begin Gilbert 28/3 ######//
	// sharing between all other fryhtan kingdoms
	{
		int monsterKingdomsCount = 0;
		for( int nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno )
		{
			if( nationRecno != nation_recno
				&& !nation_array.is_deleted(nationRecno)
				&& nation_array[nationRecno]->is_monster() )
			{
				monsterKingdomsCount++;
			}
		}

		if( monsterKingdomsCount > 0 )
		{
			float gainLivePoints = calc_live_points_bonus();
			for( nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno )
			{
				if( nationRecno != nation_recno
					&& !nation_array.is_deleted(nationRecno)
					&& nation_array[nationRecno]->is_monster() )
				{
					nation_array[nationRecno]->change_live_points( gainLivePoints / monsterKingdomsCount );
				}
			}
		}
	}
	// #### patch end Gilbert 28/3 ######//

	unit_array.del( sprite_recno );

	news_array.news_add_flag=1;
}
//----------- End of function Unit::resign -----------//



// -------- begin of function Unit::explore_area --------//
//
// explore whole rectangle
// called when init_sprite, change nation, spy::end_camouflage
//
void Unit::explore_area()
{
	//------ unveil the area where the unit is added to ------//

	if( explore_for_player() )
	{
		int scoutRange = item.ability(ITEM_ABILITY_SCOUT_RANGE);
		// do not call use_now, let next_day to use its energy

		world.unveil( next_x_loc()-scoutRange, next_y_loc()-scoutRange,
			next_x_loc() + loc_width + scoutRange -1,
			next_y_loc() + loc_height + scoutRange -1 );

		world.visit(next_x_loc(), next_y_loc(), next_x_loc()+loc_width-1,
			next_y_loc()+loc_height-1, unit_res[unit_id]->visual_range + scoutRange,
			unit_res[unit_id]->visual_extend);
	}
}
// -------- end of function Unit::explore_area --------//


// -------- begin of function Unit::explore_on_move --------//
//
// explore outside only
// called by process_move
//
void Unit::explore_on_move(int curNextXLoc, int curNextYLoc, int newNextXLoc, int newNextYLoc )
{
	if( !config.explore_whole_map && is_visible() )
	{
		if( explore_for_player() )
		{
			int scoutRange = item.ability(ITEM_ABILITY_SCOUT_RANGE);
			// do not call item.use_now()

			int xLoc1 = max(0,newNextXLoc-EXPLORE_RANGE-scoutRange);
			int yLoc1 = max(0,newNextYLoc-EXPLORE_RANGE-scoutRange);
			int xLoc2 = min(MAX_WORLD_X_LOC-1, newNextXLoc+EXPLORE_RANGE+scoutRange);
			int yLoc2 = min(MAX_WORLD_Y_LOC-1, newNextYLoc+EXPLORE_RANGE+scoutRange);
			int exploreWidth = scoutRange;

			if( newNextYLoc < curNextYLoc )			// if move upwards, explore upper area
				world.explore(xLoc1, yLoc1, xLoc2, yLoc1+exploreWidth);

			else if( newNextYLoc > curNextYLoc )	// if move downwards, explore lower area
				world.explore(xLoc1, yLoc2-exploreWidth, xLoc2, yLoc2);

			if( newNextXLoc < curNextXLoc )        // if move towards left, explore left area
				world.explore(xLoc1, yLoc1, xLoc1+exploreWidth, yLoc2);

			else if( newNextXLoc > curNextXLoc )   // if move towards right, explore right area
				world.explore(xLoc2-exploreWidth, yLoc1, xLoc2, yLoc2);
		}

		// ----- explore small area area when showing attacker path -------//

		else if( cur_order.mode == UNIT_ATTACK
			&& nation_array.player_recno && god_res[GOD_JAPANESE]->nation_prayer_count(nation_array.player_recno)
			&& !base_obj_array.is_deleted(cur_order.para)
			&& base_obj_array[cur_order.para]->nation_recno == nation_array.player_recno )
		{
			int xLoc1 = max(0,newNextXLoc-2);
			int yLoc1 = max(0,newNextYLoc-2);
			int xLoc2 = min(MAX_WORLD_X_LOC-1, newNextXLoc+2);
			int yLoc2 = min(MAX_WORLD_Y_LOC-1, newNextYLoc+2);

			int exploreWidth = 2;

			if( newNextYLoc < curNextYLoc )			// if move upwards, explore upper area
				world.explore(xLoc1, yLoc1, xLoc2, yLoc1+exploreWidth);

			else if( newNextYLoc > curNextYLoc )	// if move downwards, explore lower area
				world.explore(xLoc1, yLoc2-exploreWidth, xLoc2, yLoc2);

			if( newNextXLoc < curNextXLoc )        // if move towards left, explore left area
				world.explore(xLoc1, yLoc1, xLoc1+exploreWidth, yLoc2);

			else if( newNextXLoc > curNextXLoc )   // if move towards right, explore right area
				world.explore(xLoc2-exploreWidth, yLoc1, xLoc2, yLoc2);
		}
	}
}
// -------- end of function Unit::explore_on_move --------//


// -------- begin of function Unit::visit_area --------//
//
// called by preprocess
//
void Unit::visit_area()
{
	if( config.fog_of_war && is_visible() )
	{
		if( explore_for_player() )
		{
			world.visit(obj_loc_x1(), obj_loc_y1(), obj_loc_x2(), obj_loc_y2(),
				unit_res[unit_id]->visual_range + item.ability(ITEM_ABILITY_SCOUT_RANGE),
				unit_res[unit_id]->visual_extend);
		}

		// ----- visit small area when showing attacker path -------//

		else if( cur_order.mode == UNIT_ATTACK
			&& nation_array.player_recno && god_res[GOD_JAPANESE]->nation_prayer_count(nation_array.player_recno)
			&& !base_obj_array.is_deleted(cur_order.para)
			&& base_obj_array[cur_order.para]->nation_recno == nation_array.player_recno )
		{
			world.visit(obj_loc_x1(), obj_loc_y1(), obj_loc_x2(), obj_loc_y2(),
				1, 1 );
		}
	}
}
// -------- end of function Unit::visit_area --------//
