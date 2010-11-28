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

//Filename    : OF_CAMP.CPP
//Description : Firm Military Camp

#include <of_camp.h>
#include <ovga.h>
#include <oconfig.h>
#include <omodeid.h> 
#include <oimgres.h>
#include <ohelp.h>
#include <osys.h>
#include <onews.h>
#include <ostr.h>
#include <opower.h>
#include <ounit.h>
#include <omonsres.h>
#include <ospy.h>
#include <onation.h>
#include <oraceres.h>
#include <ogodres.h>
#include <oremote.h>
#include <oseres.h>
#include <ose.h>
#include <oitemres.h>
#include <oitem.h>
#include <itemid.h>
#include <ogame.h>
#include <of_base.h>
#include <stdlib.h>

//----------- Define static vars -------------//

static FirmCamp*	cur_firm_ptr;    // used for calling sorting functions

//--------- Declare static functions ---------//

static int  	sort_soldier_function( const void *a, const void *b );
static int  	sort_soldier_id_function( const void *a, const void *b );

//---------- define static member vars -------------//

char  FirmCamp::firm_menu_mode=FIRM_MENU_MAIN;		// whether the firm is in spy menu mode
short FirmCamp::action_spy_recno;
char	FirmCamp::bribe_result=BRIBE_NONE;
char	FirmCamp::assassinate_result=0;
short FirmCamp::steal_tech_result = 0;
char	FirmCamp::create_incident_result = 0;
short FirmCamp::action_target_recno;		// recno of bribe/assassinate target
short FirmCamp::spy_action_chance;

//--------- Begin of function FirmCamp::FirmCamp ---------//
//
FirmCamp::FirmCamp()
{
	memset( sizeof(Firm) + (char *)this, 0, sizeof(FirmCamp)-sizeof(Firm) );

	defense_flag = 1;		// default enabled
}
//----------- End of function FirmCamp::FirmCamp -----------//


// ---------- Begin of function FirmCamp::~FirmCamp --------//
//
FirmCamp::~FirmCamp()
{
	// has own destructor instead of deinit_derived
	// because Firm::~Firm skip calling deinit when sys.signal_exit_flag

	if( sys.signal_exit_flag && game.is_campaign_mode() )
	{
		// free name id

		for( int i = 1; i <= soldier_count; ++i )
		{
			soldier_array[i-1].free_name();
		}
	}

	// don't call deinit as Firm::~Firm() will call deinit
}
// ---------- End of function FirmCamp::~FirmCamp --------//


//--------- Begin of function FirmCamp::init_derived ---------//
void FirmCamp::init_derived()
{
	patrol_unit_count = 0;
	coming_unit_count = 0;

	ai_capture_town_recno = 0;
	ai_recruiting_soldier = 1;
}
//--------- ENd of function FirmCamp::init_derived ---------//

//--------- Begin of function FirmCamp::deinit ---------//
//
void FirmCamp::deinit()
{
	int firmRecno = firm_recno;		// save the firm_recno first for reset_unit_home_camp()

	Firm::deinit();

	//-------------------------------------------//

	update_influence();

	//---- reset all units whose home_camp_firm_recno is this firm ----//

	reset_unit_home_camp(firmRecno);		// this must be called at last as Firm::deinit() will create new units.

	//--- if this camp is in the Nation::attack_camp_array[], remove it now ---//

	reset_attack_camp(firmRecno);
}
//----------- End of function FirmCamp::deinit -----------//


//--------- Begin of function FirmCamp::next_day ---------//
//
void FirmCamp::next_day()
{
	//----- call next_day() of the base class -----//

	Firm::next_day();

	//----- update the patrol_unit_array -----//

	validate_patrol_unit();
	validate_soldier();

	//------- update loyalty --------//

	if( info.game_date%30 == firm_recno%30 )
		update_loyalty();

	//-------- consume food --------//

	if( soldier_count>0 )
		consume_food();

	// ------- process train ---------//

	if( config.fast_build && is_own() )
	{
		for( int i=0 ; i<100 ; i++ )
			basic_train();
	}
	else
		basic_train();

	//--------------------------------------//

	if( info.game_date%30 == firm_recno%30 )			// once a week
	{
		advanced_train();
	}

	if( info.game_date%8 == firm_recno%8 )
	{
		recover_hit_point();
	}

	// ------ process item --------//

	Soldier* soldierPtr = soldier_array;

	for( int i=0 ; i<soldier_count ; i++, soldierPtr++ )
	{
		soldierPtr->item.next_day();
	}

	//----------- debugging code -----------//

	err_when( overseer_recno && unit_array[overseer_recno]->skill_level()==0 );

	err_when( overseer_recno && unit_array[overseer_recno]->nation_recno != nation_recno );
}
//----------- End of function FirmCamp::next_day -----------//


//--------- Begin of function FirmCamp::ready_soldier_count ---------//
//
// Don't count those under training.
//
int FirmCamp::ready_soldier_count()
{
	int soldierCount=0;

	for( int i=0 ; i<soldier_count ; i++ )
	{
		if( !soldier_array[i].is_under_training() )
			soldierCount++;
	}

	return soldierCount;
}
//----------- End of function FirmCamp::ready_soldier_count -----------//


//--------- Begin of function FirmCamp::reset_attack_camp ---------//
//
void FirmCamp::reset_attack_camp(int firmRecno)
{
	//--- if this camp is in the Nation::attack_camp_array[], remove it now ---//

	if( is_ai )
	{
		Nation* nationPtr = nation_array[nation_recno];

		for( int i=0 ; i<nationPtr->attack_camp_count ; i++ )
		{
			if( nationPtr->attack_camp_array[i].firm_recno == firmRecno )
			{
				m.del_array_rec(nationPtr->attack_camp_array, nationPtr->attack_camp_count, sizeof(AttackCamp), i+1 );
				nationPtr->attack_camp_count--;
				break;
			}
		}
	}
}
//----------- End of function FirmCamp::reset_attack_camp -----------//


//------- Begin of function FirmCamp::validate_patrol_unit ---------//
//
void FirmCamp::validate_patrol_unit()
{
	int unitRecno;
	Unit* unitPtr;

	err_when( patrol_unit_count > MAX_SOLDIER+1 );

	for( int i=patrol_unit_count ; i>0 ; i-- )
	{
		unitRecno = patrol_unit_array[i-1];

		if( unit_array.is_deleted(unitRecno) ||
			 (unitPtr=unit_array[unitRecno])->is_visible()==0 ||
			 unitPtr->nation_recno != nation_recno )
		{
			err_when( patrol_unit_count > MAX_SOLDIER+1 );

			m.del_array_rec( patrol_unit_array, patrol_unit_count, sizeof(patrol_unit_array[0]), i );

			err_when( patrol_unit_count==0 );		// it's already 0
			patrol_unit_count--;

			err_when( patrol_unit_count < 0 );
			err_when( patrol_unit_count > MAX_SOLDIER+1 );
		}
	}
}
//-------- End of function FirmCamp::validate_patrol_unit ---------//


//--------- Begin of function FirmCamp::reset_unit_home_camp ---------//
//
void FirmCamp::reset_unit_home_camp(int firmRecno)
{
	//---- reset all units whose home_camp_firm_recno is this firm ----//

	Unit* unitPtr;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->home_camp_firm_recno == firmRecno )
			unitPtr->home_camp_firm_recno = 0;
	}
}
//----------- End of function FirmCamp::reset_unit_home_camp -----------//


//------- Begin of function FirmCamp::change_nation ---------//
//
void FirmCamp::change_nation(int newNationRecno)
{
	//--- update the UnitInfo vars of the soldiers in this firm ---//

	for( int i=0 ; i<soldier_count ; i++ )
		unit_res[ soldier_array[i].unit_id ]->unit_change_nation(newNationRecno, nation_recno, soldier_array[i].rank_id );

	//----- reset unit's home camp to this firm -----//

	reset_unit_home_camp(firm_recno);

	//--- if this camp is in the Nation::attack_camp_array[], remove it now ---//

	reset_attack_camp(firm_recno);

	//---- reset AI vars --------//

	ai_capture_town_recno = 0;
	ai_recruiting_soldier = 0;

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmCamp::change_nation ---------//


//--------- Begin of function FirmCamp::update_influence ---------//
//
// Update this camp's influence on neighbor towns.
//
void FirmCamp::update_influence()
{
	int   i;
	Town* townPtr;

	for( i=0 ; i<linked_town_count ; i++ )
	{
		if(town_array.is_deleted(linked_town_array[i]))
			continue;

		townPtr = town_array[linked_town_array[i]];

		if( linked_town_enable_array[i] == LINK_EE )
		{
			if( townPtr->nation_recno )
				townPtr->update_target_loyalty();
			else
				townPtr->update_target_resistance();
		}
	}
}
//----------- End of function FirmCamp::update_influence -----------//


//------- Begin of function FirmCamp::basic_train -------//
//
// Training a villager into a soldier.
//
void FirmCamp::basic_train()
{
	if( game.game_mode == GAME_TUTORIAL && 
		nation_recno != nation_array.player_recno)
		return;
	
	for( int i = 1; i <= soldier_count; ++i )
	{
		Soldier *soldierPtr = soldier_array + i - 1;

		// ###### begin Gilbert 19/11 #######//
		if( soldierPtr->is_under_training() )
		{
			int basicCombatLevel = BASIC_COMBAT_TRAIN;
			// ###### patch begin Gilbert 16/2 ######//
			// if( nation_recno && soldierPtr->race_id == RACE_ROMAN 
			//	&& god_res[GOD_ROMAN]->nation_prayer_count(nation_recno) > 0 )
			if( nation_recno && god_res[GOD_ROMAN]->nation_prayer_count(nation_recno) > 0 )
			{
				basicCombatLevel += BASIC_COMBAT_TRAIN / 2;		// roman can train to combat level 30
			}
			// ###### patch end Gilbert 16/2 ######//

			int totalBuildDays = unit_res[soldierPtr->unit_id]->build_days;
			soldierPtr->skill.inc_combat_level( ((float) basicCombatLevel + 0.125f)/ totalBuildDays );
			soldierPtr->skill.inc_skill_level( ((float) BASIC_SKILL_TRAIN + 0.125f) / totalBuildDays );
			// add a little more to avoid truncation error

			soldierPtr->hit_points = soldierPtr->max_hit_points();

			if( soldierPtr->hit_points > soldierPtr->max_hit_points() )
				soldierPtr->hit_points = soldierPtr->max_hit_points();

			--soldierPtr->remain_build_days;
			err_when( soldierPtr->remain_build_days < 0 );

		// ###### end Gilbert 19/11 #######//

			break;		// only train one unit at a time.
		}
	}
}
//------- End of function FirmCamp::basic_train -------//


//------- Begin of function FirmCamp::advanced_train -------//
//
// Increase the leadership and ocmbat level of the general and the soldiers.
//
void FirmCamp::advanced_train()
{
	if( game.game_mode == GAME_TUTORIAL && 
		nation_recno != nation_array.player_recno)
		return;
	
	if( !overseer_recno )
		return;

	Unit* overseerUnit = unit_array[overseer_recno];

	int overseerSkill = (int) overseerUnit->skill_level();
								// + overseerUnit->item.ability( ITEM_ABILITY_SKILL_LEVEL );
								// skill_level() already contain enhancement
	int incValue;

	//------- increase the commander's leadership ---------//

	if( soldier_count > 0
		 && (int) overseerUnit->skill_level() < MAX_SKILL_TRAIN ) // do not use overseerSkill
	{
		//-- the more soldiers this commander has, the higher the leadership will increase ---//

		incValue = 5 * soldier_count
					  * (int) overseerUnit->hit_points / overseerUnit->max_hit_points()
					  * (100+overseerUnit->skill.skill_potential*2) / 100;

		// AI_CHEAT

		if( is_ai && config.ai_aggressiveness > OPTION_MODERATE )
			incValue += incValue * (config.ai_aggressiveness-OPTION_MODERATE) / 3;		// 33% faster if aggressivness is high, 66% faster if aggressiveness is very high

		overseerUnit->skill.inc_skill_level( (float)incValue/100 );
	}

	//------- increase the commander's combat level ---------//

	if( overseerUnit->combat_level() < MAX_COMBAT_TRAIN )
	{
		incValue = 10 * (int) overseerUnit->hit_points / overseerUnit->max_hit_points()
					  * (100+overseerUnit->skill.skill_potential*2) / 100;

		// ------ effect of god ----------//

		// ##### patch begin Gilbert 16/2 ######//
//		if( overseerUnit->race_id == RACE_CELTIC && nation_recno 
//			&& god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
		if( nation_recno && god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
		{
			if( overseerUnit->race_id == RACE_CELTIC )
				incValue += incValue / 2;		// 50% skill increase in fort
			else
				incValue += incValue / 5;		// 20% skill increase in fort
		}
		// ##### patch end Gilbert 16/2 ######//

		if( is_ai && config.ai_aggressiveness > OPTION_MODERATE )
			incValue += incValue * (config.ai_aggressiveness-OPTION_MODERATE) / 5;		// 20% faster if aggressivness is high, 40% faster if aggressiveness is very high

		overseerUnit->skill.inc_combat_level( (float)incValue/100 );
	}

	//------- increase the solider's combat level -------//

	for( int i=0 ; i<soldier_count ; i++ )
	{
		Soldier* soldierPtr = soldier_array + i;

		#ifdef DEBUG

		err_when( !soldierPtr->name_id );

		err_when( soldierPtr->is_monster() &&
					 monster_res.name_used_array[soldierPtr->name_id-1] < 1 );

		#endif

		if( soldierPtr->is_under_training() )		// skip unit under initial training
			continue;

		//------- increase soldier skill -----------//

		if( soldierPtr->combat_level() < overseerSkill
			&& soldierPtr->combat_level() < MAX_COMBAT_TRAIN )
		{
			incValue = max(20, overseerSkill - (int)soldierPtr->combat_level())
						  * soldierPtr->hit_points / soldierPtr->max_hit_points()
						  * (100+soldierPtr->skill.skill_potential*2+overseerUnit->item.ability(ITEM_ABILITY_TRAIN)) / 100;

			// ------ effect of god ----------//

			// ###### patch begin Gilbert 16/2 ######//
			//if( soldierPtr->race_id == RACE_CELTIC && nation_recno 
			//	&& god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
			if( nation_recno 
				&& god_res[GOD_CELTIC]->nation_prayer_count(nation_recno) > 0 )
			{
				if( soldierPtr->race_id == RACE_CELTIC )
					incValue += incValue / 2;		// 50% skill increase in fort
				else
					incValue += incValue / 5;		// 20% skill increase in fort
			}
			// ###### patch end Gilbert 16/2 ######//

			// ###### patch begin Gilbert 23/12 #########//
			// penalty of egyptain
			if( soldierPtr->race_id == RACE_EGYPTIAN && nation_recno
				&& god_res[GOD_EGYPTIAN]->nation_prayer_count(nation_recno) > 0 )
			{
				incValue = incValue * (MAX_WORKER*2) / (MAX_WORKER*2+god_res[GOD_EGYPTIAN]->nation_prayer_count(nation_recno));
			}
			// ###### patch end Gilbert 23/12 #########//

			if( is_ai && config.ai_aggressiveness > OPTION_MODERATE )
				incValue += incValue * (config.ai_aggressiveness-OPTION_MODERATE) / 5;		// 20% faster if aggressivness is high, 40% faster if aggressiveness is very high

			soldierPtr->skill.inc_combat_level( (float)incValue/100 );
		}

		//-- if the soldier has leadership potential, he learns leadership --//

		if( soldierPtr->skill.skill_potential > 0
			 && soldierPtr->skill_level() < MAX_SKILL_TRAIN )
		{
			incValue = (int) max(50, overseerSkill-soldierPtr->skill_level())
						  * soldierPtr->hit_points / soldierPtr->max_hit_points()
						  * soldierPtr->skill.skill_potential*2 / 100;

			if( is_ai && config.ai_aggressiveness > OPTION_MODERATE )
				incValue += incValue * (config.ai_aggressiveness-OPTION_MODERATE) / 5;		// 20% faster if aggressivness is high, 40% faster if aggressiveness is very high

			soldierPtr->skill.inc_skill_level( (float)incValue/100 );
		}
	}
}
//-------- End of function FirmCamp::advanced_train --------//


//------- Begin of function FirmCamp::recover_hit_point -------//
//
// Soldiers recover their hit points.
//
// No need to recover the hit points of the general here as
// this is taken care in the Unit class function of the general.
//
void FirmCamp::recover_hit_point()
{
	Soldier* soldierPtr = soldier_array;

	for( int i=0 ; i<soldier_count ; i++, soldierPtr++ )
	{
		int hpInc = 1;
		short maxHp = soldierPtr->max_hit_points();

		//------- item effect -------- //

		hpInc += ( 1 + soldierPtr->item.ability(ITEM_ABILITY_RECOVERY) ) / 2;

		// ####### begin Gilbert 24/6 ########//
		// high max_hit_points recover faster
		// integer algorithm to increase maxHp / 200

		// /8 because recover_hit_point is called every 8 days
		// %200 for to recover maxHp/200 
		int cycle = (info.game_date / 8 ) % 200;
		cycle += 200;		// because cycle is going to be subtracted, avoid negative
		hpInc += (cycle * maxHp / 200) - ((cycle - 1) * maxHp / 200);

		// ####### end Gilbert 24/6 ########//

		//------- increase soldier hit points --------//

		if( soldierPtr->hit_points < maxHp )
		{
			soldierPtr->hit_points += hpInc;

			if( soldierPtr->hit_points > maxHp )
				soldierPtr->hit_points = maxHp;
		}
	}
}
//------- End of function FirmCamp::recover_hit_point -------//


//--------- Begin of function FirmCamp::patrol ---------//
//
//		patrolType = 0 when sortie all
//						 1 when sortie no leader
//						 2 when sortie no injury
//
void FirmCamp::patrol(short patrolType)
{
	err_when( 1+MAX_SOLDIER != MAX_TEAM_MEMBER);
	err_when( !overseer_recno && !soldier_count );
	err_when( is_ai && ai_capture_town_recno );		// ai_capture_town_recno is set after patrol() is called

	if( nation_recno == nation_array.player_recno )
		power.reset_selection();

	//------------------------------------------------------------//
	// If the commander in this camp has units under his lead
	// outside and he is now going to lead a new team, then
	// the old team members should be reset.
	//------------------------------------------------------------//

	if( overseer_recno )
		unit_array[overseer_recno]->team_info->reset();

	//------------------------------------------------------------//
	// mobilize the overseer first, then the soldiers
	//------------------------------------------------------------//

	short overseerRecno = overseer_recno;

	if ((patrolType == 0) || 
		 (patrolType == 2 && overseerRecno && 
			unit_array[overseerRecno]->hit_points >= 
			unit_array[overseerRecno]->max_hit_points() - 25))
		assign_overseer(0);
	else
	// must reset if no leader can be patrol
		overseerRecno = 0;

	if( patrol_all_soldier(overseerRecno, patrolType) && overseerRecno )
	{
		Unit* unitPtr = unit_array[overseerRecno];

		err_when(unitPtr->rank_id!=RANK_GENERAL && unitPtr->rank_id!=RANK_KING);

		if( nation_recno == nation_array.player_recno )
			unitPtr->select(true);
	}

	//---------------------------------------------------//

	if( overseerRecno && !overseer_recno ) // has overseer and the overseer is mobilized
	{
		Unit* overseerUnit = unit_array[overseerRecno];

		if(overseerUnit->is_own() )
		{
			se_res.sound( overseerUnit->cur_x_loc(), overseerUnit->cur_y_loc(), 1,
				'S', overseerUnit->sprite_id, "SEL");
		}

		err_when( patrol_unit_count > MAX_SOLDIER );

		//--- add the overseer into the patrol_unit_array[] of this camp ---//

		patrol_unit_array[patrol_unit_count++] = overseerRecno;

		err_when( patrol_unit_count > MAX_SOLDIER+1 );

		//------- set the team_info of the overseer -------//

		err_when( !overseerUnit->team_info );

		for( int i=0 ; i<patrol_unit_count ; i++ )
		{
			overseerUnit->team_info->member_unit_array[i] = patrol_unit_array[i];

			if( patrol_unit_array[i] != overseerRecno )		// don't set the overseer himself
				unit_array[ patrol_unit_array[i] ]->leader_unit_recno = overseerRecno;
		}

		overseerUnit->team_info->member_count = patrol_unit_count;
	}

	//-------- display info --------//

	if( nation_recno == nation_array.player_recno )		// for player's camp, patrol() can only be called when the player presses the button.
		info.disp();

	err_when( patrol_unit_count < 0 );
	err_when( patrol_unit_count > MAX_SOLDIER+1 );
}
//----------- End of function FirmCamp::patrol -----------//

//--------- Begin of function FirmCamp::patrol_all_soldier ---------//
//
// [int] overseerRecno - if this is given, this will be used instead
//								 of looking at overseer_recno.
//
// return 1 if there is enough space for patroling all soldiers
// return 0 otherwise
//
int FirmCamp::patrol_all_soldier(int overseerRecno, char patrolType)
{
	err_when(!soldier_array);    // this function shouldn't be called if this firm does not need soldier

	//------- detect buttons on hiring firm soldiers -------//

	err_when(soldier_count>MAX_SOLDIER);

	#ifdef DEBUG
		int loopCount=0;
	#endif

	short unitRecno;
	int mobileSoldierId = 1;

	patrol_unit_count = 0;		// reset it, it will be increased later

	for( mobileSoldierId = 1; mobileSoldierId <= soldier_count ; ++mobileSoldierId )
	{
		err_when(++loopCount > 100);

		if( soldier_array[mobileSoldierId-1].is_under_training() )
			continue;

		if( (soldier_array[mobileSoldierId-1].hit_points < 
			  soldier_array[mobileSoldierId-1].max_hit_points() - 25) &&
			  patrolType == 2
			)
			continue;

		unitRecno = mobilize_soldier(mobileSoldierId, COMMAND_AUTO);

		patrol_unit_array[patrol_unit_count++] = unitRecno;
		err_when(patrol_unit_count>MAX_SOLDIER);

		if(!unitRecno)
			return 0; // keep the rest soldiers as there is no space for creating the unit

		--mobileSoldierId;	// if can mobilize successfully, mobileSoldierId is unchanged

//		if( !overseerRecno )
		if( !overseerRecno && patrolType == 0)
			overseerRecno = overseer_recno;

		Unit* unitPtr = unit_array[unitRecno];
		if(overseerRecno)
		{			
			unitPtr->update_loyalty();							// the unit is just assigned to a new leader, set its target loyalty

			err_when( unit_array[overseerRecno]->rank_id != RANK_KING &&
					  unit_array[overseerRecno]->rank_id != RANK_GENERAL );
			if( nation_recno == nation_array.player_recno )
				unitPtr->select();		
		}
		else
			if( nation_recno == nation_array.player_recno )	
				unitPtr->select(true);
	}

	return 1;
}
//----------- End of function FirmCamp::patrol_all_soldier -----------//


//-------- Begin of function FirmCamp::is_soldier_full ------//
//
int FirmCamp::is_soldier_full()
{
	if( is_ai ) 
		return soldier_count + patrol_unit_count + coming_unit_count >= MAX_SOLDIER;
	else
		return soldier_count >= MAX_SOLDIER;
}
//----------- End of function FirmCamp::is_soldier_full ---------//


//--------- Begin of function FirmCamp::sort_soldier ---------//
//
// Sort units in camp by their leaderhip.
//
void FirmCamp::sort_soldier()
{
	if( soldier_count > 1 )
	{
		//--- prepare soldier_id_array[] for later preserving the currently soldier selected ---//

		Soldier selectedSoldier;
		if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
			selectedSoldier = soldier_array[selected_soldier_id-1];

		cur_firm_ptr = this;
		qsort( soldier_array, soldier_count, sizeof(Soldier), sort_soldier_function );

		//---- scan soldier_array[] for preserving the currently soldier selected ---//

		if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
		{
			for( selected_soldier_id=soldier_count ; 
				selected_soldier_id>0 && memcmp( &selectedSoldier, &soldier_array[selected_soldier_id-1], sizeof(Soldier));
				--selected_soldier_id );
			err_when( !selected_soldier_id );
		}
	}
}
//----------- End of function FirmCamp::sort_soldier -----------//


//--------- Begin of function sort_soldier_function ---------//
//
static int sort_soldier_function( const void *va, const void *vb )
{
	Soldier *a = (Soldier *)va;
	Soldier *b = (Soldier *)vb;

	int rc = a->is_under_training() - b->is_under_training();

	if( rc == 0 )
		return int( b->skill_level() - a->skill_level() );
	else
		return rc;
}
//----------- End of function sort_soldier_function -----------//


//--------- Begin of function sort_soldier_id_function ---------//
//
static int sort_soldier_id_function( const void *va, const void *vb )
{
	Soldier *a = &cur_firm_ptr->soldier_array[*((short*)va) - 1];
	Soldier *b = &cur_firm_ptr->soldier_array[*((short*)vb) - 1];

	int rc = a->is_under_training() - b->is_under_training();

	if( rc == 0 )
		return int( b->skill_level() - a->skill_level() );
	else
		return rc;
}
//----------- End of function sort_soldier_id_function -----------//


//--------- Begin of function FirmCamp::reward ---------//
//
// Only military camp has the "reward" option and not the other firms also
// because soldiers in other firms live in the towns and their loyalty
// are based on the town they live. Military camp is not linked to a town.
//
// <int> soldierId	 - 0 - commander, >0 - id. of the soldier
// <int> remoteAction - either COMMAND_PLAYER or COMMAND_REMOTE
//
void FirmCamp::reward(int soldierId, int remoteAction)
{
	if( remoteAction==COMMAND_PLAYER && remote.is_enable() )
	{
		if( !remoteAction && remote.is_enable() )
		{
			// packet structure : <firm recno> <soldier id>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_REWARD, 2*sizeof(short) );
			*shortPtr = firm_recno;
			shortPtr[1] = soldierId;
		}
	}
	else
	{
		if( soldierId == 0 )
		{
			if( overseer_recno )
				unit_array[overseer_recno]->reward(nation_recno);
		}
		else
		{
			err_when( soldierId < 1 || soldierId > soldier_count );

			soldier_array[soldierId-1].change_loyalty( REWARD_LOYALTY_INCREASE );

			nation_array[nation_recno]->add_expense( EXPENSE_REWARD_UNIT, (float)REWARD_COST);
		}
	}
}
//----------- End of function FirmCamp::reward -----------//


//-------- Begin of function FirmCamp::majority_race ------//
//
int FirmCamp::majority_race()
{
	//--- if there is a overseer, return the overseer's race ---//

	if( overseer_recno )
		return unit_array[overseer_recno]->race_id;

	if( soldier_count==0 )
		return 0;

	//----- count the no. people in each race ------//

	char raceCountArray[MAX_RACE];
	char monsterRaceCountArray[MAX_MONSTER_TYPE+8];		// reserve more space...

	memset( raceCountArray, 0, sizeof(raceCountArray) );
	memset( monsterRaceCountArray, 0, sizeof(monsterRaceCountArray) );

	int i;
	for( i=0 ; i<soldier_count ; i++ )
	{
		if( soldier_array[i].is_human() )
			raceCountArray[ soldier_array[i].race_id-1 ]++;
		else if( soldier_array[i].is_monster() )
			monsterRaceCountArray[ soldier_array[i].monster_id()-1 ]++;
	}

	//---------------------------------------------//

	int mostRaceCount=0, mostRaceId=0;

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( raceCountArray[i] > mostRaceCount )
		{
			mostRaceCount = raceCountArray[i];
			mostRaceId 	  = i+1;
		}
	}
	for( i=0; i < MAX_MONSTER_TYPE; i++ )
	{
		if( monsterRaceCountArray[i] > mostRaceCount )
		{
			mostRaceCount = monsterRaceCountArray[i];
			mostRaceId 	  = -(i+1);
		}
	}

	return mostRaceId;
}
//---------- End of function FirmCamp::majority_race --------//


//-------- Begin of function FirmCamp::should_show_info ------//
//
// Whether information of this firm should be shown.
//
int FirmCamp::should_show_info()
{
	// ####### begin Gilbert 22/2 #########//
	// if bribe/assassinate failed, player_spy_count may be zero (if no other spy)
	if( firm_array.selected_recno == firm_recno )
	{
		if( firm_menu_mode == FIRM_MENU_BRIBE_RESULT
			|| firm_menu_mode == FIRM_MENU_ASSASSINATE_RESULT
			|| firm_menu_mode == FIRM_MENU_STEAL_TECH_RESULT
			|| firm_menu_mode == FIRM_MENU_INCIDENT_RESULT )
		{
			return 1;
		}
	}
	// ####### end Gilbert 22/2 #########//

	return player_spy_count > 0 || Firm::should_show_info();
}
//---------- End of function FirmCamp::should_show_info --------//


//--------- Begin of function FirmCamp::can_assign_capture --------//
//
// Return whether new units assigned to this firm can capture
// this firm.
//
int FirmCamp::can_assign_capture()
{
	return (overseer_recno==0 && soldier_count==0);
}
//--------- End of function FirmCamp::can_assign_capture --------//


//--------- Begin of function FirmCamp::capture_firm --------//
//
// The firm is being captured by another nation.
//
void FirmCamp::capture_firm(int newNationRecno)
{
	if( nation_recno == nation_array.player_recno )
		news_array.firm_captured(firm_recno, newNationRecno, 0);		// 0 - the capturer is not a spy

	//-------- if this is an AI firm --------//

	if( is_ai )
		ai_firm_captured(newNationRecno);

	//------------------------------------------//
	//
	// If there is an overseer in this firm, then the only
	// unit who can capture this firm will be the overseer only,
	// so calling its betray() function will capture the whole
	// firm already.
	//
	//------------------------------------------//

	if( overseer_recno && unit_array[overseer_recno]->spy_recno )
		unit_array[overseer_recno]->spy_change_nation(newNationRecno, COMMAND_AUTO);
	else
		change_nation(newNationRecno);
}
//--------- End of function FirmCamp::capture_firm --------//


//---------- Begin of function FirmCamp::update_loyalty --------//
//
void FirmCamp::update_loyalty()
{
	//----- update loyalty of the soldiers -----//

	Soldier* soldierPtr = soldier_array;

	for( int i=0 ; i<soldier_count ; i++, soldierPtr++ )
	{
		int targetLoyalty = soldierPtr->target_loyalty(firm_recno);

		if( targetLoyalty > soldierPtr->loyalty )
		{
			int incValue = (targetLoyalty - soldierPtr->loyalty)/10;

			int newLoyalty = (int) soldierPtr->loyalty + max(1, incValue);

			if( newLoyalty > targetLoyalty )
				newLoyalty = targetLoyalty;

			soldierPtr->loyalty = newLoyalty;
		}
		else if( targetLoyalty < soldierPtr->loyalty )
		{
			soldierPtr->loyalty--;
		}
	}
}
//----------- End of function FirmCamp::update_loyalty ---------//


//--------- Begin of function FirmCamp::consume_food ---------//
//
void FirmCamp::consume_food()
{
	if( !nation_recno )
		return;

	if( nation_array[nation_recno]->food > 0 )
	{
		int humanUnitCount=0;

		for( int i=0 ; i<soldier_count ; i++ )
		{
			if( soldier_array[i].is_human() )		// only humans consume food
				humanUnitCount++;
		}

		nation_array[nation_recno]->consume_food((float) humanUnitCount * UNIT_FOOD_YEAR_CONSUMPTION / 365);
	}
	else	//--- decrease loyalty if the food has been run out ---//
	{
		if( info.game_date%NO_FOOD_LOYALTY_DECREASE_INTERVAL == 0 )		// decrease 1 loyalty point every 2 days
		{
			for( int i=0 ; i<soldier_count ; i++ )
			{
				if( soldier_array[i].is_human() )
					soldier_array[i].change_loyalty(-1);
			}
		}
	}
}
//----------- End of function FirmCamp::consume_food -----------//


//---------- Begin of function FirmCamp::pay_expense --------//
//
// pay_expense() is called by Firm::next_day()
//
void FirmCamp::pay_expense()
{
	if( !nation_recno )
		return;

	Firm::pay_expense();

	//-------- pay expenses to human units and weapons --------//

	Soldier* soldierPtr = soldier_array;
	Nation* nationPtr = nation_array[nation_recno];

	for( int i=1 ; i<=soldier_count ; i++, soldierPtr++ )
	{
		if( soldierPtr->is_under_training() )
			continue;

		// #### begin Gilbert 24/3 #####//
		// int unitClass = unit_res[soldierPtr->unit_id]->unit_class;

		//------ if the unit is a weapon -------//

		// if( unitClass == UNIT_CLASS_WEAPON )
		// one has life reduce hp when no food
		// one has no life, reduce hp when no money
		if( !unit_res[soldierPtr->unit_id]->class_info.life )
		// #### end Gilbert 24/3 #####//
		{
			if( nationPtr->cash > 0 )
			{
				nationPtr->add_expense( EXPENSE_WEAPON,
					(float) unit_res[soldierPtr->unit_id]->year_cost / 365, 1 );
			}
			else
			{
				//-- if it's a weapon, decrease hit points if the nation cannot pay the unit --//

				if( soldierPtr->hit_points > 0 )
					soldierPtr->hit_points--;

				if( soldierPtr->hit_points == 0 )
					kill_soldier(i);		// if its hit points is zero, delete it

				err_when( soldierPtr->hit_points < 0 );
			}
		}
	}
}
//----------- End of function FirmCamp::pay_expense ---------//


//---- Begin of function FirmCamp::builder_stay_after_construction ----//
//
// This function is called by Firm::process_construction()
// to assign the builder into the firm after the construction
// of the firm is complete.
//
// return: <int> 1 - the builder has been successfully assigned
//							into the firm.
//					  0 - failed.
//
int FirmCamp::builder_stay_after_construction()
{
	if( !builder_recno )
		return 0;

	Unit *unitPtr = unit_array[builder_recno];

	if( unitPtr->is_civilian() )		// only assign military unit
		return 0;

	unitPtr->set_mode(0);			// reset it from UNIT_MODE_CONSTRUCT_???

	assign_unit(builder_recno);

	builder_recno = 0;

	return soldier_count || overseer_recno;
}
//----- End of function FirmCamp::builder_stay_after_construction -----//
