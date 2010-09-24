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

// Filename    : OF_SPY.CPP
// Description : Firm College

#include <OF_SPY.H>
#include <ALL.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OTOWN.H>
#include <OSPY.H>

//-------- Define constant ----------//

#define TRAIN_SKILL_COST 30

//------- Begin of function FirmSpy::FirmSpy -------//

FirmSpy::FirmSpy()
{
	memset( sizeof(FirmTrain)+(char *)this, 0, sizeof(FirmSpy)-sizeof(FirmTrain) );
}

//------- End of function FirmSpy::FirmSpy -------//


//------- Begin of function FirmSpy::recruit_trainee -------//

void FirmSpy::recruit_trainee(int townRecno, int trainType, char remoteAction )
{
	if( !can_recruit(townRecno, trainType) )
		return;

	if( town_array.is_deleted(townRecno) || town_array[townRecno]->recruitable_pop(1) <= 0
		|| (firm_res[firm_id]->same_race_worker && town_array[townRecno]->race_id != race_id) )
	{
		return;
	}

	if( nation_recno && nation_array[nation_recno]->cash < (float)TRAIN_SKILL_COST )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ######//
		// packet structure : <firm recno> <source town recno> <train type>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_RECRUIT, 3*sizeof(short) );
		*shortPtr = firm_recno;
		shortPtr[1] = townRecno;
		shortPtr[2] = trainType;
		// ##### end Gilbert 7/10 ######//
		return;
	}

	Town *townPtr = town_array[townRecno];

	// -------- train only non spy -------- //

	//--- if all remaining units are spies, when you try to train one, all of them will become mobilized ---//

	err_when( townPtr->spy_count > townPtr->population );
	if( townPtr->spy_count == townPtr->population)
	{
		while( townPtr->spy_count > 0 )
		{
			int spyRecno = spy_array.find_town_spy(townRecno, 1);
			err_when( !spyRecno );

			Spy* spyPtr = spy_array[spyRecno];
			if( !spyPtr->mobilize_town_spy() )
				break;
			spyPtr->change_cloaked_nation( spyPtr->true_nation_recno );
		}
		return;
	}

	// -------- initialize spy trainee -----------//

	Trainee *traineePtr = trainee_array + trainee_count;
	++trainee_count;

	traineePtr->init( townPtr->race_id, (char)townPtr->loyalty, 0, trainType, townRecno );
	// ######## begin Gilbert 28/6 #########//
	if( trainType != TRAIN_UNIT_CIVILIAN )
	{
		if( traineePtr->train_combat_potential >= 20)
			traineePtr->train_combat_potential -= 20;
		traineePtr->train_spy_potential = 40 + m.random(20);
	}
	else
	{
		traineePtr->train_spy_potential = 70 + m.random(30);
	}
	// ######## end Gilbert 28/6 #########//

	// --------- reduce town population --------//

	UnitInfo* unitInfo = unit_res[traineePtr->unit_id];

	townPtr->dec_pop();
	unitInfo->inc_nation_unit_count(nation_recno);

	// ------- add expense ----------//

	if( nation_recno )
	{
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) unitInfo->build_cost );
	}
}
//------- End of function FirmSpy::recruit_trainee -------//


//------- Begin of function FirmSpy::can_recruit -------//
//
// Return whether the specific type of spy can be trained in this firm.
//
// <int> trainType - the type of spy unit to be trained
//							It can be TRAIN_UNIT_???.
//
int FirmSpy::can_recruit(int townRecno, int trainType)
{
	if( !FirmTrain::can_recruit(townRecno, trainType) )
		return 0;

	return can_train_from_town(townRecno, required_linked_firm[trainType]);
}
//-------- End of function FirmSpy::can_recruit --------//


//-------- Begin of function FirmSpy::advanced_train ------//
//
void FirmSpy::advanced_train()
{
	//--- train twice as fast for espionage colleges ---//

	FirmTrain::advanced_train();
	FirmTrain::advanced_train();
}
//-------- End of function FirmSpy::advanced_train ------//


// ------ Begin of function FirmSpy::can_set_rally_point -------//
//
int FirmSpy::can_set_rally_point(int destBaseObjRecno)
{
	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		BaseObj *baseObjPtr = base_obj_array[destBaseObjRecno];

		Firm *firmPtr = baseObjPtr->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return firmPtr->is_human();		// only accept human
			}
			else if( firmPtr->cast_to_FirmInn() )
			{
				return 1;
			}
		}

		Town *townPtr = baseObjPtr->cast_to_Town();
		if( townPtr )
		{
			return 1;
		}
	}

	return 0;
}
// ------ End of function FirmSpy::can_set_rally_point -------//


// ------ Begin of function FirmSpy::send_to_rally_point -------//
//
int FirmSpy::send_to_rally_point( int unitRecno )
{
	if( unitRecno && rally_enable_flag )
	{
		err_when( unit_array.is_deleted(unitRecno) );

		if( rally_dest_base_obj_recno
			&& !base_obj_array.is_deleted(rally_dest_base_obj_recno) )
		{
			BaseObj *destPtr = base_obj_array[rally_dest_base_obj_recno];
			if( destPtr->can_accept_assign(unitRecno, nation_recno) )
			{
				unit_array[unitRecno]->assign(rally_dest_x, rally_dest_y, nation_recno );
				return 1;
			}

			// ------- try to change nation to dest nation ------//

			Unit *unitPtr = unit_array[unitRecno];

			unitPtr->spy_change_nation(destPtr->nation_recno, COMMAND_AUTO);
			if( destPtr->can_accept_assign(unitRecno, nation_recno) )
			{
				unit_array[unitRecno]->assign(rally_dest_x, rally_dest_y, nation_recno );
				return 1;
			}

			// ------- try to change nation to independent ------//

			unitPtr->spy_change_nation(0, COMMAND_AUTO);
			if( destPtr->can_accept_assign(unitRecno, nation_recno) )
			{
				unit_array[unitRecno]->assign(rally_dest_x, rally_dest_y, nation_recno );
				return 1;
			}

			// ------ change back to original nation --------//

			unitPtr->spy_change_nation( unitPtr->true_nation_recno(), nation_recno );
			return 0;
		}
		else
		{
			unit_array[unitRecno]->move(rally_dest_x, rally_dest_y );
			return 1;
		}
	}

	return 0;
}
// ------ End of function FirmSpy::send_to_rally_point -------//

