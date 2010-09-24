//Filename   : OF_MTRN.CPP
//Description: Class of FirmMonsterTrain

#include <OF_MTRN.H>
#include <ALL.H>
#include <OSYS.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OUNITRES.H>
#include <OCONFIG.H>
 
// -------- defind static variable ---------//

char FirmMonsterTrain::firm_menu_mode;

//-------- Begin of function FirmMonsterTrain::FirmMonsterTrain ------//
//
FirmMonsterTrain::FirmMonsterTrain(int trainUnitId)
{
	memset( sizeof(Firm)+ (char *)this, 0, sizeof(FirmMonsterTrain)-sizeof(Firm) );

	trainee_count = 0;
	selected_trainee_id = 0;
	train_type_count = 1;
	train_unit_id[0] = trainUnitId;
}
//-------- End of function FirmMonsterTrain::FirmMonsterTrain ------//


//-------- Begin of function FirmMonsterTrain::FirmMonsterTrain ------//
//
// trainUnitIdArary, zero-terminated array of train unit id
//
FirmMonsterTrain::FirmMonsterTrain(char *trainUnitIdArray)
{
	memset( sizeof(Firm)+ (char *)this, 0, sizeof(FirmMonsterTrain)-sizeof(Firm) );

	trainee_count = 0;
	selected_trainee_id = 0;
	train_type_count = strlen(trainUnitIdArray);
	strncpy( train_unit_id, trainUnitIdArray, MAX_MONSTER_TRAIN_TYPE);
}
//-------- End of function FirmMonsterTrain::FirmMonsterTrain ------//


//-------- Begin of function FirmMonsterTrain::next_day ------//
//
void FirmMonsterTrain::next_day()
{
	Firm::next_day();

	if( config.fast_build && is_own() )
	{
		for( int i=0 ; i<10 ; i++ )
			basic_train();
	}
	else
	{
		basic_train();		// basic training
	}
}
//-------- End of function FirmMonsterTrain::next_day ------//
                                           

//-------- Begin of function FirmMonsterTrain::recruit_trainee ------//
//
void FirmMonsterTrain::recruit_trainee(int trainUnitId, char remoteAction)
{
	err_when( !train_type_count );

	if( !can_recruit(trainUnitId) )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <source town recno> <train type>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_RECRUIT, 3*sizeof(short) );
		*shortPtr = firm_recno;
		shortPtr[1] = 0;
		shortPtr[2] = trainUnitId;
		return;
	}

	//-------- initialize MonsterTrainee -----------//

	err_when( trainee_count > MAX_MONSTER_TRAINEE );

	MonsterTrainee *monsterTrainee = trainee_array + trainee_count;

	trainee_count++;

	monsterTrainee->unit_id = trainUnitId;
	monsterTrainee->loyalty = 70 + m.random(20);
	monsterTrainee->skill.init(trainUnitId);

	//------- add expense ----------//

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];
		UnitInfo* unitInfo = unit_res[trainUnitId];

		nationPtr->add_expense(EXPENSE_TRAIN_UNIT, (float) unitInfo->build_cost );
		nationPtr->change_live_points((float) -unitInfo->build_live_points_cost );
	}
}
//-------- End of function FirmMonsterTrain::recruit_trainee ------//


//-------- Begin of function FirmMonsterTrain::can_recruit ------//
//
int FirmMonsterTrain::can_recruit(int trainUnitId)
{
	if( trainee_count == MAX_MONSTER_TRAINEE )
		return 0;

	// ------- check trainUnitId in train_unit_id -------//

	err_when( !m.str_chr( train_unit_id, trainUnitId, 1, train_type_count) );

	// ------ check nation tech level --------//

	if( !nation_recno || unit_res[trainUnitId]->get_nation_tech_level(nation_recno) == 0 )
		return 0;

	// ------ check cost ------//

	Nation*   nationPtr = nation_array[nation_recno];
	UnitInfo* unitInfo  = unit_res[trainUnitId];

	return nationPtr->cash >= unitInfo->build_cost &&
			 nationPtr->live_points >= unitInfo->build_live_points_cost;
}
//---------- End of function FirmMonsterTrain::can_recruit --------//


//------- Begin of function FirmMonsterTrain::basic_train -------//
//
void FirmMonsterTrain::basic_train()
{
	for( int i = 1; i <= trainee_count; ++i )
	{
		MonsterTrainee *monsterTrainee = trainee_array + i - 1;

		if( monsterTrainee->is_under_training() )
		{
			float incPoints = (float) BASIC_COMBAT_TRAIN / unit_res[monsterTrainee->unit_id]->build_days;

			monsterTrainee->skill.inc_combat_level(incPoints);

			//---- if the unit has completed training ----//

			if( !monsterTrainee->is_under_training() )
			{
				unit_res[monsterTrainee->unit_id]->inc_nation_unit_count(nation_recno);

				// -------- send to rally point -----//

				if( rally_enable_flag && !monsterTrainee->is_under_training() )
				{
					int unitRecno = mobilize_trainee( i, COMMAND_AUTO );
					if( unitRecno )
					{
						send_to_rally_point( unitRecno );
					}
				}
			}

			break;		// only train one unit at a time.
		}
	}
}
//------- End of function FirmMonsterTrain::basic_train -------//


//-------- Begin of function FirmMonsterTrain::mobilize_trainee ------//
//
// mobilize one MonsterTrainee
// if traineeId == -1 mobilize all trainees, and return the smallest unit recno
//
int FirmMonsterTrain::mobilize_trainee(int traineeId, char remoteAction )
{
	//------ if the unit is still under training ------//

	if( traineeId >= 1 && trainee_array[traineeId-1].is_under_training() )
		return 0;

	if(!remoteAction && remote.is_enable() )
	{
		// packet strcture : <firm_recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_WORKER, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = traineeId;
		return 0;
	}

	//------------------------------------//

	if( traineeId == -1 )
	{
		int rc = 0;
		for( int i = trainee_count; i > 0; --i )
		{
			if( trainee_array[i-1].is_under_training() )
				continue;

			int unitRecno = mobilize_trainee(i, COMMAND_AUTO );

			if( unitRecno )
			{
				if( !rc || unitRecno < rc )
					rc = unitRecno;

				// set mobilized unit selected
				if( nation_recno == nation_array.player_recno )
				{
					if( !unit_array.selected_recno || unitRecno < unit_array.selected_recno )
						unit_array.selected_recno = unitRecno;

					unit_array[unitRecno]->select();
				}
			}
		}

		if( rc && nation_recno == nation_array.player_recno
			&& firm_array.selected_recno == firm_recno )
		{
			firm_array.selected_recno = 0;
			info.disp();
		}
		return rc;
	}

	//-------------------------------------------//

	err_when( !trainee_array );    // this function shouldn't be called if this firm does not need MonsterTrainee

	err_when( traineeId<1 || traineeId>trainee_count );

	//------- decrease MonsterTrainee no. and create an unit -----//

	MonsterTrainee* traineePtr = trainee_array+traineeId-1;
	int	  unitRecno = 0;

	MonsterTrainee thisTrainee = trainee_array[traineeId-1];

	err_when( thisTrainee.is_under_training() );

	unitRecno = create_trainee_unit(thisTrainee);	 	// if he is a spy, create_trainee_unit wil call set_place(SPY_MOBILE)

	if(!unitRecno)
		return 0; // return 0 eg there is no space to create the unit

	//------- delete the record from the trainee_array ------//

	err_when( trainee_count > MAX_MONSTER_TRAINEE );
	err_when( selected_trainee_id > trainee_count );

	m.del_array_rec(trainee_array, trainee_count, sizeof(MonsterTrainee), traineeId);

	if( selected_trainee_id > traineeId || selected_trainee_id == trainee_count )
		selected_trainee_id--;

	trainee_count--;

	err_when( trainee_count < 0 );
	err_when( selected_trainee_id > trainee_count );

	return unitRecno;
}
//----------- End of function FirmMonsterTrain::mobilize_soldier -----------//


//------ begin of function FirmMonsterTrain::kill_trainee ----------//
//
void FirmMonsterTrain::kill_trainee(int traineeId )
{
	err_when( !trainee_array );    // this function shouldn't be called if this firm does not need MonsterTrainee

	err_when( traineeId<1 || traineeId>trainee_count );

	//------- decrease MonsterTrainee no. and create an unit -----//

	MonsterTrainee* traineePtr = trainee_array+traineeId-1;
	int	  unitRecno = 0;

	//--------- decrease the unit count ---------//

	// ####### begin Gilbert 27/11 ######//
	if( !traineePtr->is_under_training() )
		unit_res[traineePtr->unit_id]->dec_nation_unit_count(nation_recno);
	// ####### end Gilbert 27/11 ######//

	//------- delete the record from the trainee_array ------//

	err_when( trainee_count > MAX_MONSTER_TRAINEE );
	err_when( selected_trainee_id > trainee_count );

	m.del_array_rec(trainee_array, trainee_count, sizeof(MonsterTrainee), traineeId);

	if( selected_trainee_id > traineeId || selected_trainee_id == trainee_count )
		selected_trainee_id--;

	trainee_count--;

	if( trainee_count==0 )
		selected_trainee_id = 0;

	err_when( selected_trainee_id > trainee_count );
}
//------ end of function FirmMonsterTrain::kill_trainee ----------//


//-------- Begin of function FirmMonsterTrain::free_all_people ------//
//
void FirmMonsterTrain::free_all_people()
{
	if( !sys.signal_exit_flag )
	{
		for( int i = trainee_count; i > 0; --i )
		{
			if( !mobilize_trainee(i, COMMAND_AUTO) )
				kill_trainee(i);
		}
	}
	else
	{
		for( int i = trainee_count; i > 0; --i )
		{
			kill_trainee(i);
		}
	}

	Firm::free_all_people();
}
//-------- End of function FirmMonsterTrain::free_all_people ------//


//-------- Begin of function FirmMonsterTrain::create_trainee_unit ------//
//
int FirmMonsterTrain::create_trainee_unit(MonsterTrainee& thisTrainee)
{
	//------------ create an unit --------------//

	int unitRecno = create_unit( thisTrainee.unit_id );      // this MonsterTrainee no longer has a job as it has been resigned

	if( !unitRecno )
		return 0;

	Unit* unitPtr   = unit_array[unitRecno];
	UnitInfo *unitInfo = unit_res[thisTrainee.unit_id];

	//------- set the unit's parameters --------//

	unitPtr->skill 	  = thisTrainee.skill;
	unitPtr->hit_points = (float) thisTrainee.skill.actual_max_hit_points();

	unitPtr->loyalty    = thisTrainee.loyalty;
	unitPtr->rank_id    = RANK_SOLDIER;

	unitPtr->init_attack_info();

	err_when( unitPtr->hit_points <= 0 );
	err_when( !unitPtr->is_visible() );

	unitPtr->set_combat_level(-1);	// -1 for current combat level so as to update unit attribute of that combat level

	// #### patch begin Gilbert 29/8 #####//
	//--- decrease the unit count as it is doubled counted when a new unit is created ---//

	if( nation_recno )
		unitInfo->dec_nation_unit_count(nation_recno);
	// #### patch end Gilbert 29/8 #####//

	return unitRecno;
}
//-------- End of function FirmMonsterTrain::create_trainee_unit ------//


//------- Begin of function FirmMonsterTrain::change_nation ---------//
//
void FirmMonsterTrain::change_nation(int newNationRecno)
{
	//--- update the UnitInfo vars of the soldiers in this firm ---//

	// ###### begin Gilbert 14/6 ########## //
	for( int i=0 ; i<trainee_count ; i++ )
	{
		if( !trainee_array[i].is_under_training() )	// inc_nation_unit_count when training finish
			unit_res[ trainee_array[i].unit_id ]->unit_change_nation(newNationRecno, nation_recno, RANK_SOLDIER );
	}
	// ###### end Gilbert 14/6 ########## //

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmMonsterTrain::change_nation ---------//



void FirmMonsterTrain::cancel_train_trainee( int traineeId, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_CANCEL_TRAIN, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = traineeId;
		return;
	}

	// ------ scan the specified trainee and backward ------//

	if( traineeId > trainee_count )
		traineeId = trainee_count;

	for( ; traineeId > 0; --traineeId )
	{
		if( trainee_array[traineeId-1].is_under_training() )
		{
			int unitId = trainee_array[traineeId-1].unit_id;
			// backup unit id as it will be killed

			kill_trainee(traineeId);

			// ------- return expense ----------//

			if( nation_recno )
			{
				UnitInfo *unitInfo = unit_res[unitId];
				nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) -unitInfo->build_cost );
				nation_array[nation_recno]->change_live_points( (float) unitInfo->build_live_points_cost );
			}
			break;
		}
	}
}


// ---------- Begin of function FirmMonsterTrain::can_set_rally_point ------//
//
// whether the firm can set rally point
//
// [int] destBaseObjRecno		- destination base obj recno, 0 for setting position
//
int FirmMonsterTrain::can_set_rally_point(int destBaseObjRecno)
{
	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		Firm *firmPtr = base_obj_array[destBaseObjRecno]->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return firmPtr->is_monster();
			}
		}
	}

	return 0;
}
// ---------- End of function FirmMonsterTrain::can_set_rally_point ------//
