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

// Filename    : OF_FORT.CPP
// Description : Firm Fort

#include <OF_FORT.H>
#include <ALL.H>
#include <GAMEDEF.H>
#include <OSYS.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OBULLET.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OSPY.H>
#include <OCONFIG.H>

//------ begin of function FirmFort::FirmFort -------//
//
FirmFort::FirmFort()
{
	memset( sizeof(FirmCamp)+(char *)this, 0, sizeof(FirmFort)-sizeof(FirmCamp) );
}
//------ end of function FirmFort::FirmFort -------//


//------ begin of function FirmFort::init_derived -------//
//
void FirmFort::init_derived()
{
	FirmCamp::init_derived();

	current_archer_count = 0;
	target_archer_count = 0;

	train_archer_progress = 0;
	memset( last_archer_fire_frame, 0, sizeof(last_archer_fire_frame) );
}
//------ end of function FirmFort::init_derived -------//


//------ begin of function FirmFort::resistance_adjustment -------//
//
int FirmFort::resistance_adjustment()
{
	return 100;
}
//------ end of function FirmFort::resistance_adjustment -------//


//------ begin of function FirmFort::loyalty_adjustment -------//
//
int FirmFort::loyalty_adjustment()
{
	return 100;
}
//------ end of function FirmFort::loyalty_adjustment -------//


//----------- Begin of function FirmFort::recruit_solder -----------//
//
// recruit soldier from town
// interface/ai should find townRecno
// refer to Firm::pull_town_people in 7k1
//
void FirmFort::recruit_soldier(int townRecno, bool specialUnit, char remoteAction, char noCost )
{
	if( !can_recruit(townRecno) )
		return;

	if( town_array.is_deleted(townRecno) || town_array[townRecno]->recruitable_pop(1) <= 0 )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ######//
		// packet structure : <firm recno> <source town recno> <train type>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_RECRUIT, 3*sizeof(short) );
		*shortPtr = firm_recno;
		shortPtr[1] = townRecno;
		shortPtr[2] = specialUnit ? 1 : 0;
		// ##### end Gilbert 7/10 ######//
		return;
	}

	//----- get unit id. and check if there is enough cash to train the unit ----//

	Town *townPtr = town_array[townRecno];
	int  unitId;
	
	if( !specialUnit )
		unitId = race_res[townPtr->race_id]->infantry_unit_id;
	else
		unitId = race_res[townPtr->race_id]->special_unit_id;

	UnitInfo* unitInfo = unit_res[unitId];

	if( nation_recno && nation_array[nation_recno]->cash < (float)unitInfo->build_cost )
		return;

	//-------- initialize solder -----------//

	Soldier *soldierPtr = soldier_array + soldier_count;
	++soldier_count;

	memset( soldierPtr, 0, sizeof(Soldier) );
	soldierPtr->loyalty = (char) townPtr->loyalty;
	soldierPtr->race_id = townPtr->race_id;
	soldierPtr->unit_id = unitId;
	soldierPtr->rank_id = RANK_SOLDIER;
	soldierPtr->remain_build_days = unitInfo->build_days;
	// ###### begin Gilbert 23/2 ######//
	soldierPtr->unique_id = m.rand_long();
	// ###### end Gilbert 23/2 ######//
	soldierPtr->init_name();
	// ####### begin Gilbert 26/1 ######//
	soldierPtr->source_town_recno = townRecno;
	// ####### end Gilbert 26/1 ######//

	//------ setting new skill ---------//

	soldierPtr->skill.init(soldierPtr->unit_id);
	soldierPtr->hit_points = soldierPtr->max_hit_points();

	//------ determine if it is a spy ----------//

	int spySeq;
	if( townPtr->spy_count > 0 && (spySeq=m.random(townPtr->recruitable_pop(1))) < townPtr->spy_count )
	{
		soldierPtr->spy_recno = spy_array.find_town_spy(townRecno, 1+spySeq);
	}
	else
		soldierPtr->spy_recno = 0;

	if( soldierPtr->spy_recno )
	{
		// ####### begin Gilbert 24/2 ########//
		soldierPtr->unique_id = spy_array[soldierPtr->spy_recno]->unique_id;
		// ####### end Gilbert 24/2 ########//
		spy_array[soldierPtr->spy_recno]->set_place(SPY_FIRM, firm_recno);
	}


	//--------- reduce town population --------//

	townPtr->dec_pop();
	unitInfo->inc_nation_unit_count(nation_recno);

	//------- add expense ----------//

	if( nation_recno && !noCost )
	{
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) unitInfo->build_cost );
	}
}
//----------- End of function FirmFort::recruit_solder -----------//


//------ begin of function FirmFort::can_recruit -------//
//
int FirmFort::can_recruit(int townRecno)
{
	if( town_array.is_deleted(townRecno) )
		return 0;

	return nation_array[nation_recno]->cash > 0 &&
			 !is_soldier_full() &&
			 town_array[townRecno]->can_train();
}
//------ end of function FirmFort::can_recruit -------//


//------ begin of function FirmFort::next_day -------//
//
void FirmFort::next_day()
{
	FirmCamp::next_day();

	process_train_archer();
}
//------ end of function FirmFort::next_day -------//


//------ begin of function FirmFort::pay_expense -------//
//
void FirmFort::pay_expense()
{
	float dayExpense = current_archer_count * (float)COST_PER_ARCHER / 365;

	if( current_archer_count > 0 && nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		if( nationPtr->cash >= dayExpense )
		{
			nationPtr->add_expense( EXPENSE_FIRM, dayExpense, 1);
		}
		else
		{
			//---- dismiss archer ----- //

			int loopCount = 0;
			while( current_archer_count > 0 )
			{
				kill_archer();
				err_when(++loopCount > MAX_FORT_ARCHER + 2);
			}
		}
	}

	FirmCamp::pay_expense();
}
//------ end of function FirmFort::pay_expense -------//


//------ begin of function FirmFort::being_attack_hit -------//
//
void FirmFort::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	//------- fire back --------//

	// moved to place::being_attack_hit
	//return_fire( attackerObj );

	//----- randomly remove an arhcer ------//

	if( current_archer_count > 0 )
	{
		int randomChance = (int)hit_points-current_archer_count*5;

		if( randomChance <= 0 || m.random(randomChance)==0 )
			kill_archer();
	}

	//---- call parent class function -----//

	FirmCamp::being_attack_hit(attackerObj, damagePoint);
}
//------ end of function FirmFort::being_attack_hit -------//


//------ begin of function FirmFort::set_target_archer -------//
//
// if newTargetArcher is -1, increase by 1
// if newTargetArcher is -2, decrease by 1
void FirmFort::set_target_archer(int newTargetArcher, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ######//
		// packet structure : <firm recno> <new target archer>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_FORT_TARGET_ARCHER, 2*sizeof(short) );
		*shortPtr = firm_recno;
		shortPtr[1] = newTargetArcher;
		// ##### end Gilbert 7/10 ######//
		return;
	}

	if( newTargetArcher == -1 )
	{
		if( target_archer_count < MAX_FORT_ARCHER )
			++target_archer_count;
	}
	else if( newTargetArcher == -2 )
	{
		//### begin trevor 24/10 ###//
		if( target_archer_count > current_archer_count )
			--target_archer_count;
		//### end trevor 24/10 ###//
	}
	else if( newTargetArcher >= 0 && newTargetArcher <= MAX_FORT_ARCHER )
		target_archer_count = newTargetArcher;

	err_when( target_archer_count < 0 || target_archer_count > MAX_FORT_ARCHER );
}
//------ end of function FirmFort::set_target_archer -------//


//------ begin of function FirmFort::process_train_archer -------//
//
void FirmFort::process_train_archer()
{
	if( current_archer_count < target_archer_count 
		&& (!nation_recno || nation_array[nation_recno]->cash > TRAIN_ARCHER_COST) )
	{
		if( ++train_archer_progress > MAX_TRAIN_ARCHER_PROGRESS 
			|| (config.fast_build && nation_recno == nation_array.player_recno) )
		{
			//-------- increase archer -------//

			train_archer_progress = 0;
			last_archer_fire_frame[current_archer_count] = 0;
			++current_archer_count;
		}

		//--------- add expense -------//

		if( nation_recno )
		{
			nation_array[nation_recno]->add_expense(
				EXPENSE_FIRM, (float)TRAIN_ARCHER_COST/MAX_TRAIN_ARCHER_PROGRESS, 0 );
		}
	}
}
//------ end of function FirmFort::process_train_archer -------//


//------ begin of function FirmFort::kill_archer -------//
//
void FirmFort::kill_archer()
{
	// remove the last archer

	if( current_archer_count > 0 )
	{
		--current_archer_count;
	}
}
//------ end of function FirmFort::kill_archer -------//


//------ begin of function FirmFort::return_fire -------//
//
int FirmFort::return_fire(BaseObj* targetObj)
{
	// ###### begin Gilbert 29/3 #######//
	if( !current_archer_count || !targetObj || !targetObj->is_visible() 
		|| area_distance(targetObj) > FORT_BULLET_RANGE )
		return 0;
	// ###### end Gilbert 29/3 #######//

	//------- select archer ------//

	int archerId = 0;
	int archerCount = 0;		// no of archer shoot in this frame

	for(int a = 0; a < current_archer_count; ++a )
	{
		DWORD frameCount = sys.frame_count - last_archer_fire_frame[a];

		if( frameCount == 0 )
			++archerCount;

		if( !archerId && frameCount > MIN_ARHCER_DELAY )
			archerId = a+1;
	}

	if( archerId && archerCount < MAX_ARCHER_SIMULTANEOUS )
	{
		// ###### begin Gilbert 29/3 #######//
		if( targetObj->cast_to_Unit()
			&& bullet_array.add_bullet(this, targetObj->cast_to_Unit())
			|| targetObj->cast_to_Place()
			&& bullet_array.add_bullet(this, targetObj->cast_to_Place()) )
		{
			last_archer_fire_frame[archerId-1] = sys.frame_count;
			return archerId;
		}
		// ###### end Gilbert 29/3 #######//
	}

	return 0;
}
//------ end of function FirmFort::return_fire -------//

//------ begin of function FirmFort::bullet_damage -------//

float FirmFort::bullet_damage()
{
	// ###### begin Gilbert 22/10 ######//
	// return FORT_BULLET_DAMAGE / ATTACK_SLOW_DOWN;
	return FORT_BULLET_DAMAGE;			// divided by ATTACK_SLOW_DOWN in reducing damage
	// ###### end Gilbert 22/10 ######//
}
//------ end of function FirmFort::bullet_damage -------//


//------ begin of function FirmFort::bullet_radius -------//

short FirmFort::bullet_radius()
{
	return LOCATE_WIDTH + 8;
}
//------ end of function FirmFort::bullet_radius -------//


//------ begin of function FirmFort::bullet_fire -------//

char FirmFort::bullet_fire()
{
	return 0;
}
//------ end of function FirmFort::bullet_fire -------//


//------ begin of function FirmFort::bullet_id -------//

short FirmFort::bullet_id()
{
	static spriteId = sprite_res.search_sprite( "ARROW" );
	err_when( !spriteId );

	return spriteId;
}
//------ end of function FirmFort::bullet_id -------//


//------ begin of function FirmFort::return_fire -------//

short FirmFort::bullet_speed()
{
	return FORT_BULLET_SPEED;
}
//------ end of function FirmFort::return_fire -------//

//------ begin of function FirmFort::bullet_init_z -------//

short FirmFort::bullet_init_z()
{
	// ###### begin Gilbert 23/10 ######//
	return altitude + FORT_BULLET_INIT_Z;
	// ###### end Gilbert 23/10 ######//
}
//------ end of function FirmFort::bullet_init_z -------//


// ----- begin of function FirmFort::can_set_active_link -------//
//
int FirmFort::can_set_active_link(int townRecno)
{
	if( !townRecno )
		return 1;		// can clear active link

	if( town_array.is_deleted(townRecno) )
		return 0;

	Town *townPtr = town_array[townRecno];

	FirmInfo *firmInfo = firm_res[firm_id];
	if( nation_recno == townPtr->nation_recno
		&& (!firmInfo->same_race_worker || race_res.is_same_race(race_id,townPtr->race_id)) )
	{
		return 1;
	}

	return 0;
}
// ----- end of function FirmFort::can_set_active_link -------//


// ----- begin of function FirmFort::set_active_link -------//
//
void FirmFort::set_active_link(int townRecno, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <town recno>
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_FIRM_SET_ACTIVE_LINK, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = townRecno;
		return;
	}

	if( !townRecno || town_array.is_deleted(townRecno) )
	{
		active_link_town_recno = 0;
	}
	else if(can_set_active_link(townRecno) )
	{
		active_link_town_recno = townRecno;
	}
}
// ----- end of function FirmFort::set_active_link -------//


// -------- begin of function FirmFort::cancel_train_soldier --------//
//
void FirmFort::cancel_train_soldier( int soldierId, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_CANCEL_TRAIN, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = soldierId;
		return;
	}

	if( soldierId < 0 || soldierId > soldier_count )
		return;

	Soldier *soldierPtr = &soldier_array[soldierId-1];

	if( !soldierPtr->is_under_training() )		// may happen in multiplayer
		return;

	int unitId = soldierPtr->unit_id;
	// backup unit id as it will be mobilized

	int sourceTown = 0;
	Town *townPtr = NULL;

	// ----- if it he can put back to the origin town -----//

	if( soldierPtr->source_town_recno
		&& !town_array.is_deleted(soldierPtr->source_town_recno)
		&& (townPtr = town_array[soldierPtr->source_town_recno])
		&& townPtr->race_id == soldierPtr->race_id
		&& townPtr->nation_recno == nation_recno
		&& townPtr->population < MAX_TOWN_POPULATION )
	{
		sourceTown = soldierPtr->source_town_recno;
	}

	// ----- scan any suitable linked town ------//

	if( !sourceTown )
	{
		for( int i = 0; i < linked_town_count; ++i )
		{
			err_when( town_array.is_deleted(linked_town_array[i]));
			townPtr = town_array[linked_town_array[i]];

			if( townPtr->race_id == soldierPtr->race_id
				&& townPtr->nation_recno == nation_recno
				&& townPtr->population < MAX_TOWN_POPULATION )
			{
				sourceTown = townPtr->town_recno;
				break;
			}
		}
	}

	// ---- return to town -------//

	if( sourceTown )
	{
		err_when( unit_res[unitId]->unit_class == UNIT_CLASS_WAGON );

		Town *townPtr = town_array[sourceTown];
		townPtr->inc_pop( soldierPtr->loyalty );

		//------ if the unit is a spy -------//

		if( soldierPtr->spy_recno > 0 )
		{
			spy_array[soldierPtr->spy_recno]->set_place(SPY_TOWN, sourceTown);
			soldierPtr->spy_recno = 0;		// reset it so kill_soldier() won't delete the spy
		}

		kill_soldier(soldierId);
	}

	// ------ mobilize to civilian -------//

	else if( mobilize_soldier(soldierId, COMMAND_AUTO) )
	{
	}

	else
	{
		// -------- kill soldier ------//

		kill_soldier(soldierId);
	}

	// ---- return money -----//

	if( nation_recno )
	{
		UnitInfo *unitInfo = unit_res[unitId];
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) -unitInfo->build_cost );
	}
}
// -------- end of function FirmFort::cancel_train_soldier --------//
