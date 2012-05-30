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

// Filename    : OTRAINEE.CPP
// Description : Trainee

#include <of_train.h>
#include <ofirm.h>
#include <onation.h>
#include <oraceres.h>
#include <ospy.h>
#include <ogodres.h>
#include <of_base.h>


//------ Begin of function Trainee::init --------//
//
void Trainee::init(int raceId, int initLoyalty, int spyRecno, int trainType, int srcTownRecno )
{
	memset( this, 0, sizeof( Trainee ) );

	race_id	  = raceId;
	loyalty    = initLoyalty;
	spy_recno  = spyRecno;
	train_type = trainType;
	source_town_recno = srcTownRecno;

	is_under_training = 1;

	//------- set vars that depends on training type ------//

	switch( train_type )
	{
		case TRAIN_UNIT_CIVILIAN:
			unit_id = (char) race_res[raceId]->civilian_unit_id;
			break;

		case TRAIN_UNIT_INFANTRY:
			unit_id = (char) race_res[raceId]->infantry_unit_id;
			// ##### begin Gilbert 28/6 #########/
			// train_combat_potential = 40 + m.random(20);
			train_combat_potential = 70 + m.random(30);
			// ##### end Gilbert 28/6 #########/
			train_skill_potential = 10 + m.random(10);
			break;

		case TRAIN_UNIT_SPECIAL:
			unit_id = (char) race_res[raceId]->special_unit_id;
			// ##### begin Gilbert 28/6 #########/
			// train_combat_potential = 40 + m.random(20);
			train_combat_potential = 70 + m.random(30);
			// ##### end Gilbert 28/6 #########/
			train_skill_potential = 10 + m.random(10);
			break;

		default:
			err_here();
	}

	//-------- init skill -------//

	skill.init(unit_id);
}
//-------- End of function Trainee::init ------//


//-------- Begin of function Trainee::basic_train ------//
//
// Non-basic training: train the unit if it has potential.
//
void Trainee::basic_train(int firmRecno)
{
	int trainedFlag=0;

	//------ train spy skill --------//

	if( train_spy_potential && spy_skill < BASIC_SKILL_TRAIN )
	{
		spy_skill += 1;

		if( spy_skill > MAX_SKILL_TRAIN )
			spy_skill = MAX_SKILL_TRAIN;

		trainedFlag=1;
	}

	//------ train combat --------//

	if( train_type == TRAIN_UNIT_CIVILIAN )
	{
		if( combat_level() < CITIZEN_COMBAT_LEVEL )
		{
			skill.inc_combat_level(1);
			trainedFlag=1;
		}
	}
	else		//----- military unit -----//
	{
		int basicCombatLevel = BASIC_COMBAT_TRAIN;
		int nationRecno = firm_array[firmRecno]->nation_recno;
		// ##### patch begin Gilbert 16/2 ######//
		// if( nationRecno && race_id == RACE_ROMAN 
		//	&& god_res[GOD_ROMAN]->nation_prayer_count(nationRecno) > 0 )
		if( nationRecno && god_res[GOD_ROMAN]->nation_prayer_count(nationRecno) > 0 )
		// ##### patch end Gilbert 16/2 ######//
		{
			basicCombatLevel += BASIC_COMBAT_TRAIN / 2;		// roman can train to combat level 30
		}

		if( train_combat_potential && combat_level() < basicCombatLevel )
		{
			float incPoints = (float) basicCombatLevel / unit_res[unit_id]->build_days;

			skill.inc_combat_level(incPoints);
			trainedFlag=1;
		}
	}

	//------ train leadership --------//

	if( train_skill_potential && skill_level() < BASIC_SKILL_TRAIN )
	{
		skill.inc_skill_level(1);
		trainedFlag=1;
	}

	if( !trainedFlag )                        // this unit has completed training
		is_under_training = 0;
}
//-------- End of function Trainee::basic_train ------//


//-------- Begin of function Trainee::advanced_train ------//
//
// Non-basic training: train the unit if it has potential.
//
void Trainee::advanced_train(int firmRecno)
{
	int nationRecno = firm_array[firmRecno]->nation_recno;

	//------ train spy skill --------//

	if( train_spy_potential && spy_skill < MAX_SKILL_TRAIN )
	{
		int incValue = (MAX_SKILL_TRAIN - (int)spy_skill)
			* train_spy_potential / 100;

		incValue = MAX(20,incValue);

		spy_skill += (float) incValue / 100;

		if( spy_skill > MAX_SKILL_TRAIN )
			spy_skill = MAX_SKILL_TRAIN;
	}

	//------ train combat --------//

	if( train_combat_potential && combat_level() < MAX_COMBAT_TRAIN )
	{
		int incValue = (MAX_COMBAT_TRAIN - (int)combat_level() )
			* train_combat_potential / 100;

		incValue = MAX(20,incValue);

		// ###### patch begin Gilbert 16/2 #######//
		// ------ effect of god ----------//

		//if( race_id == RACE_CELTIC && nationRecno 
		//	&& god_res[GOD_CELTIC]->nation_prayer_count(nationRecno) > 0 )
		if( nationRecno && god_res[GOD_CELTIC]->nation_prayer_count(nationRecno) > 0 )
		{
			if( race_id == RACE_CELTIC )
				incValue += incValue / 2;		// 50% skill increase in fort
			else
				incValue += incValue / 5;		// 20% skill increase in fort
		}
		// ###### patch end Gilbert 16/2 #######//

		// ###### patch begin Gilbert 23/12 #########//
		// penalty of egyptain
		if( race_id == RACE_EGYPTIAN && nationRecno
			&& god_res[GOD_EGYPTIAN]->nation_prayer_count(nationRecno) > 0 )
		{
			incValue = incValue * (MAX_WORKER*2) / (MAX_WORKER*2+god_res[GOD_EGYPTIAN]->nation_prayer_count(nationRecno));
		}
		// ###### patch end Gilbert 23/12 #########//

		skill.inc_combat_level( (float)incValue/100 );
	}

	//------ train leadership --------//

	if( train_skill_potential && skill_level() < MAX_SKILL_TRAIN )
	{
		int incValue = (MAX_SKILL_TRAIN - (int)skill_level() )
			* train_skill_potential / 100;

		skill.inc_skill_level( (float)incValue/100 );
	}
}
//-------- End of function Trainee::advanced_train	 ------//


//------ Begin of function Trainee::is_nation --------//
//
// Whether this soldier belongs to the specific nation.
//
// <int> firmRecno - the recno of the firm the soldier works in
// <int> nationRecno - the recno of th nation to check against. 
//
int Trainee::is_nation(int firmRecno, int nationRecno)
{
	if( spy_recno && spy_array[spy_recno]->true_nation_recno == nationRecno )
		return 1;

	return firm_array[firmRecno]->nation_recno == nationRecno;
}
//------ End of function Trainee::is_nation --------//


//------ Begin of function Trainee::is_own --------//
//
int Trainee::is_own(int firmRecno)
{
	return nation_array.player_recno && is_nation(firmRecno, nation_array.player_recno);
}
//------ End of function Trainee::is_own --------//


//------ Begin of function Trainee::is_own_spy --------//
//
int Trainee::is_own_spy()
{
	return spy_recno && nation_array.player_recno
		&& spy_array[spy_recno]->true_nation_recno == nation_array.player_recno;
}
//------ End of function Trainee::is_own_spy --------//


//------ Begin of function Trainee::true_nation_recno --------//
//
int Trainee::true_nation_recno(int firmRecno)
{
	if( spy_recno )
		return spy_array[spy_recno]->true_nation_recno;
	else
		return firm_array[firmRecno]->nation_recno;
}
//------ End of function Trainee::true_nation_recno --------//



//---------- Begin of function Trainee::change_loyalty --------//

void Trainee::change_loyalty(int loyaltyChange)
{
//	if( town_recno ) 		// for those live in town, their loyalty are based on town people loyalty.
//		return;

	int newLoyalty = loyalty + loyaltyChange;

	newLoyalty 		 = MIN( 100, newLoyalty );
	loyalty = MAX( 0, newLoyalty );
}
//---------- End of function Trainee::change_loyalty --------//


// ---------- Begin of function Trainee::change_unit_id ------//
//
void Trainee::change_unit_id(int unitId, int nationRecno)
{
	if( unitId == unit_id )
		return;

	// err_when( hero_id );
	// must not change race
	err_when( !race_id );
	err_when( race_id && race_id != unit_res[unitId]->race_id );

	Skill oldSkill = skill;

	// -------- increase/decrease unit count -----//

	if( nationRecno )
	{
		// increase before decrease
		unit_res[unitId]->inc_nation_unit_count(nationRecno);
		unit_res[unit_id]->dec_nation_unit_count(nationRecno);
	}

	// --------- change unit id ----------//

	unit_id = unitId;

	// ----- max_hit_points changed -----//
	
	// skill.std_hit_points = newUnitInfo->hit_points;
	skill.set_unit_id(unit_id);
}
// ---------- End of function Trainee::change_unit_id ------//
