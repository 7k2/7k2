// Filename    : OF_TRAIN.CPP
// Description : FirmTrain

#include <OF_TRAIN.H>
#include <OCONFIG.H>
#include <OSYS.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OSPY.H>
#include <OPOWER.H>
#include <OGAME.H>
 
//------ define class member vars ------//

char FirmTrain::firm_menu_mode;

//-------- Begin of function FirmTrain::FirmTrain ------//
//
FirmTrain::FirmTrain()
{
	memset( sizeof(Firm)+ (char *)this, 0, sizeof(FirmTrain)-sizeof(Firm) );

	trainee_count = 0;
	selected_trainee_id = 0;
	player_spy_count = 0;
}
//-------- End of function FirmTrain::FirmTrain ------//


//-------- Begin of function FirmTrain::next_day ------//
//
void FirmTrain::next_day()
{
	Firm::next_day();

	// ### begin Gilbert 26/1 #####//
	validate_trainee();
	// ### end Gilbert 26/1 #####//

	//------- basic training ---------//

	if( config.fast_build && is_own() )
	{
		for( int i=0 ; i<10 ; i++ )
			basic_train();
	}
	else
		basic_train();

	//------ advanced training ------//

	if( info.game_date%30 == firm_recno%30 )			// once a week
	{
		advanced_train();		// potential advanced training
	}

	//------- debugging code -------//

	#ifdef DEBUG

	for( int i = 1; i <= trainee_count; ++i )
	{
		if( trainee_array[i-1].spy_recno )
		{
			Spy* spyPtr = spy_array[ trainee_array[i-1].spy_recno ];

			err_when( spyPtr->spy_place != SPY_FIRM );
			err_when( spyPtr->spy_place_para != firm_recno );
		}
	}

	#endif
}
//-------- End of function FirmTrain::next_day ------//


//-------- Begin of function FirmTrain::recruit_trainee ------//
//
//
void FirmTrain::recruit_trainee(int townRecno, int trainType, char remoteAction)
{
	if( !can_recruit(townRecno, trainType) )
		return;

	if( town_array.is_deleted(townRecno) || town_array[townRecno]->recruitable_pop(1) <= 0
		|| (firm_res[firm_id]->same_race_worker && town_array[townRecno]->race_id != race_id) )
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

	//----- get unit id. and check if there is enough cash to train the unit ----//

	Town *townPtr = town_array[townRecno];
	int  unitId;
	RaceInfo* raceInfo = race_res[townPtr->race_id];

	switch(trainType)
	{
		case TRAIN_UNIT_CIVILIAN:
			unitId = raceInfo->civilian_unit_id;
			break;

		case TRAIN_UNIT_INFANTRY:
			unitId = raceInfo->infantry_unit_id;
			break;

		case TRAIN_UNIT_SPECIAL:
			unitId = raceInfo->special_unit_id;
			break;

		default:
			err_here();
	}

	UnitInfo* unitInfo = unit_res[unitId];

	//-------- initialize trainee -----------//

	Trainee *traineePtr = trainee_array + trainee_count;

	++trainee_count;

	int spySeq, spyRecno;

	// if( townPtr->spy_count > 0 && (spySeq=m.random(townPtr->recruitable_pop(1))) < townPtr->spy_count )
	// use town population instead of recruitable_pop(1) because assume some spy is working, spy_count include those working

	if( townPtr->spy_count > 0 && (spySeq=m.random(townPtr->population)) < townPtr->spy_count )
		spyRecno = spy_array.find_town_spy(townRecno, 1+spySeq);
	else
		spyRecno = 0;

	traineePtr->init( townPtr->race_id, (char)townPtr->loyalty, spyRecno, trainType, townRecno );

	if( traineePtr->spy_recno )
		spy_array[traineePtr->spy_recno]->set_place(SPY_FIRM, firm_recno);

	//--------- reduce town population --------//

	townPtr->dec_pop();
	unitInfo->inc_nation_unit_count(nation_recno);

	//------- add expense ----------//

	if( nation_recno )
	{
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) unitInfo->build_cost );
	}
}
//-------- End of function FirmTrain::recruit_trainee ------//


//------- Begin of function FirmTrain::basic_train -------//
//
// Basic training - training a villager into a spy
//
void FirmTrain::basic_train()
{
	if( game.game_mode == GAME_TUTORIAL && 
		nation_recno != nation_array.player_recno)
		return;
	
	for( int i = 1; i <= trainee_count; ++i )
	{
		Trainee *traineePtr = trainee_array + i - 1;

		if( traineePtr->is_under_training )
		{
			traineePtr->basic_train(firm_recno);

			// -------- send to rally point -----//

			if( rally_enable_flag && !traineePtr->is_under_training )
			{
				int unitRecno = mobilize_trainee( i, COMMAND_AUTO );
				if( unitRecno )
				{
					send_to_rally_point( unitRecno );
				}
			}
			break;
		}
	}
}
//------- End of function FirmTrain::basic_train -------//


//-------- Begin of function FirmTrain::advanced_train ------//
//
void FirmTrain::advanced_train()
{
	if( game.game_mode == GAME_TUTORIAL && 
		nation_recno != nation_array.player_recno)
		return;
	
	//------- increase the solider's combat level -------//

	Trainee* traineePtr = trainee_array;

	for( int i=0 ; i<trainee_count ; i++, traineePtr++ )
	{
		if( traineePtr->is_under_training )
			continue;

		traineePtr->advanced_train(firm_recno);
	}
}
//-------- End of function FirmTrain::advanced_train ------//


//-------- Begin of function FirmTrain::mobilize_trainee ------//
//
// mobilize one trainee
// if traineeId == -1 mobilize all trainees, and return the smallest unit recno
//
int FirmTrain::mobilize_trainee(int traineeId, char remoteAction )
{
	//------ if the unit is still under training ------//

	// ###### begin Gilbert 26/1 #######//
//	if( traineeId >= 1 && trainee_array[traineeId-1].is_under_training )
//		return 0;
	// ###### end Gilbert 26/1 #######//

	//------------------------------------//

	if(!remoteAction && remote.is_enable() )
	{
		// ####### begin Gilbert 7/10 ######//
		// packet strcture : <firm_recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_WORKER, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = traineeId;
		return 0;
		// ####### end Gilbert 7/10 ######//
	}

	if( traineeId == -1 )
	{
		int rc = 0;
		for( int i = trainee_count; i > 0; --i )
		{
			if( trainee_array[i-1].is_under_training )
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

	// ######## begin Gilbert 26/1 ########//
	if( traineeId > trainee_count )
		return 0;		// may happen in multiplayer
	// ######## end Gilbert 26/1 ########//

	err_when( !trainee_array );    // this function shouldn't be called if this firm does not need trainee

	err_when( traineeId<1 || traineeId>trainee_count );

	//------- decrease trainee no. and create an unit -----//

	Trainee* traineePtr = trainee_array+traineeId-1;
	int	  unitRecno = 0;

//	if( traineePtr->race_id && traineePtr->name_id )
//	{
//		race_res[traineePtr->race_id]->free_name_id(traineePtr->name_id);
//		traineePtr->name_id = 0;
//	}

	Trainee thisTrainee = trainee_array[traineeId-1];

	// ##### begin Gilbert 26/1 ######//
	// err_when( thisTrainee.is_under_training );
	if( thisTrainee.is_under_training )
	{
		if( thisTrainee.is_human() )
		{
			// cancel training human
			// change unit id to civilian
//			if( nation_recno )
//				unit_res[thisTrainee.unit_id]->dec_nation_unit_count(nation_recno);
//			thisTrainee.unit_id = race_res[thisTrainee.race_id]->civilian_unit_id;
//			if( nation_recno )
//				unit_res[thisTrainee.unit_id]->inc_nation_unit_count(nation_recno);
//			Skill civilSkill;
//			civilSkill.init(thisTrainee.unit_id, CITIZEN_COMBAT_LEVEL, 0);
//			thisTrainee.skill = civilSkill;

			thisTrainee.skill.set_combat_level( CITIZEN_COMBAT_LEVEL );
			thisTrainee.spy_skill = 0;		// reset spy skill
			thisTrainee.change_unit_id(race_res[thisTrainee.race_id]->civilian_unit_id, nation_recno);
		}
		else
		{
			err_here();
		}
	}
	// ##### end Gilbert 26/1 ######//

	unitRecno = create_trainee_unit(thisTrainee);	 	// if he is a spy, create_trainee_unit wil call set_place(SPY_MOBILE)

	if(!unitRecno)
		return 0; // return 0 eg there is no space to create the unit

	//------- delete the record from the trainee_array ------//

	err_when( trainee_count > MAX_TRAINEE );
	err_when( selected_trainee_id > trainee_count );

	m.del_array_rec(trainee_array, trainee_count, sizeof(Trainee), traineeId);

	if( selected_trainee_id > traineeId || selected_trainee_id == trainee_count )
		selected_trainee_id--;

	trainee_count--;

	err_when( trainee_count < 0 );
	err_when( selected_trainee_id > trainee_count );

	return unitRecno;
}
//----------- End of function FirmTrain::mobilize_soldier -----------//


//------ begin of function FirmTrain::kill_trainee ----------//
//
void FirmTrain::kill_trainee(int traineeId )
{
	err_when( !trainee_array );    // this function shouldn't be called if this firm does not need trainee

	err_when( traineeId<1 || traineeId>trainee_count );

	//------- decrease trainee no. and create an unit -----//

	Trainee* traineePtr = trainee_array+traineeId-1;
	int	  unitRecno = 0;

//	if( traineePtr->race_id && traineePtr->name_id )
//	{
//		race_res[traineePtr->race_id]->free_name_id(traineePtr->name_id);
//		traineePtr->name_id = 0;
//	}

	//-------- if this trainee is a spy ---------//

	if( traineePtr->spy_recno )
	{
		spy_array[traineePtr->spy_recno]->set_place(SPY_UNDEFINED, 0);
		spy_array.del_spy( traineePtr->spy_recno );
	}

	//--------- decrease the unit count ---------//

	unit_res[traineePtr->unit_id]->dec_nation_unit_count(nation_recno);

	//------- delete the record from the trainee_array ------//

	err_when( trainee_count > MAX_TRAINEE );
	err_when( selected_trainee_id > trainee_count );

	m.del_array_rec(trainee_array, trainee_count, sizeof(Trainee), traineeId);

	if( selected_trainee_id > traineeId || selected_trainee_id == trainee_count )
		selected_trainee_id--;

	trainee_count--;

	if( trainee_count==0 )
		selected_trainee_id = 0;

	err_when( selected_trainee_id > trainee_count );
}
//------ end of function FirmTrain::kill_trainee ----------//


//-------- Begin of function FirmTrain::free_all_people ------//
//
void FirmTrain::free_all_people()
{
	if( !sys.signal_exit_flag )
	{
		for( int i = trainee_count; i > 0; --i )
		{
			// ####### begin Gilbert 26/1 ###//
			if( !trainee_array[i-1].is_under_training )
			{
				if( !mobilize_trainee( i, COMMAND_AUTO ) )
					kill_trainee( i );								// if the unit cannot be mobilized (can be because it's under training), kill it 
			}
			else
			{
				cancel_train_trainee( i, COMMAND_AUTO );
			}
			// ####### begin Gilbert 26/1 ###//
		}
	}
	else
	{
		for( int i = trainee_count; i > 0; --i )
		{
			kill_trainee( i );
		}
	}

	Firm::free_all_people();
}
//-------- End of function FirmTrain::free_all_people ------//


//-------- Begin of function FirmTrain::create_trainee_unit ------//
//
int FirmTrain::create_trainee_unit(Trainee& thisTrainee)
{
	//--------- copy the trainee's info --------//

	int unitLoyalty = thisTrainee.loyalty;

	//--------- decrease the unit count first because create_unit() will increase it ---------//

	unit_res[thisTrainee.unit_id]->dec_nation_unit_count(nation_recno);

	//------------ create an unit --------------//

	int unitId = thisTrainee.unit_id;
	int unitRecno = create_unit( unitId );      // this trainee no longer has a job as it has been resigned

	if( !unitRecno )
		return 0;

	Unit* unitPtr   = unit_array[unitRecno];
	UnitInfo *unitInfo = unit_res[unitId];

	//------- set the unit's parameters --------//

	unitPtr->skill 	  = thisTrainee.skill;
	unitPtr->hit_points = (float) thisTrainee.max_hit_points();

	err_when( unitPtr->combat_level()<=0 );

	unitPtr->loyalty    = unitLoyalty;
	unitPtr->rank_id    = RANK_SOLDIER;

//	if( unit_res[unitPtr->unit_id]->class_info.has_weapon_version )
//	{
//		unitPtr->set_weapon_version( thisTrainee.extra_para );	// restore nation contribution
//	}
//	else if( unitPtr->race_id )
//	{
//		unitPtr->cur_power = thisTrainee.extra_para;
//		if( unitPtr->cur_power < 0 )
//			unitPtr->cur_power = 0;
//		if( unitPtr->cur_power > 150 )
//			unitPtr->cur_power = 150;
//	}

	err_when( unitPtr->hit_points <= 0 );

	unitPtr->init_attack_info();

//	if( thisTrainee.name_id && thisTrainee.race_id )		// if this trainee is formerly an unit who has a name
//		unitPtr->set_name(thisTrainee.name_id);

	err_when( !unitPtr->is_visible() );

	//------ if the unit is a spy -------//

	if( thisTrainee.spy_recno )
	{
		Spy* spyPtr = spy_array[thisTrainee.spy_recno];

		unitPtr->spy_recno = thisTrainee.spy_recno;
		unitPtr->is_ai     = spyPtr->cloaked_nation_recno &&
									nation_array[spyPtr->cloaked_nation_recno]->is_ai();

		unitPtr->set_name(spyPtr->name_id);		// set the name id. of this unit
		// ####### begin Gilbert 24/2 #########//
		unitPtr->unique_id = spyPtr->unique_id;		// set_place also set unique_id
		// ####### end Gilbert 24/2 #########//

		spyPtr->set_place(SPY_MOBILE, unitRecno);
	}
	else if( thisTrainee.spy_skill > 0)
	{
		unitPtr->spy_recno = spy_array.add_spy(unitRecno, (int) thisTrainee.spy_skill);
	}

	// ###### begin Gilbert 4/11 ######//
	unitPtr->set_combat_level(-1);
	// ###### end Gilbert 4/11 ######//

	// ##### begin Gilbert 24/2 #######//
	// ------- update win/lose condition on changing of unit recno -----//

	game.update_condition_on_mobilize(unitRecno, unitPtr->unique_id );
	// ##### end Gilbert 24/2 #######//

	return unitRecno;
}
//-------- End of function FirmTrain::create_trainee_unit ------//


//-------- Begin of function FirmTrain::should_show_info ------//
//
// Whether information of this firm should be shown.
//
int FirmTrain::should_show_info()
{
	return player_spy_count > 0 || Firm::should_show_info();
}
//---------- End of function FirmTrain::should_show_info --------//


//-------- Begin of function FirmTrain::can_recruit ------//
//
// Return whether this firm can recruit a unit
//
int FirmTrain::can_recruit(int townRecno, int trainType)
{
	if( town_array.is_deleted(townRecno) )
		return 0;

	return nation_array[nation_recno]->cash >= 0 &&
			 trainee_count < MAX_TRAINEE &&
			 town_array[townRecno]->can_train();
}
//---------- End of function FirmTrain::can_recruit --------//


// ----- begin of function FirmTrain::can_set_active_link -------//
//
int FirmTrain::can_set_active_link(int townRecno)
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
// ----- end of function FirmTrain::can_set_active_link -------//


// ----- begin of function FirmTrain::set_active_link -------//
//
void FirmTrain::set_active_link(int townRecno, char remoteAction)
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
// ----- end of function FirmTrain::set_active_link -------//


//------- Begin of function FirmTrain::change_nation ---------//
//
void FirmTrain::change_nation(int newNationRecno)
{
	//--- update the UnitInfo vars of the soldiers in this firm ---//

	for( int i=0 ; i<trainee_count ; i++ )
		unit_res[ trainee_array[i].unit_id ]->unit_change_nation(newNationRecno, nation_recno, RANK_SOLDIER );

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmTrain::change_nation ---------//


//------- Begin of function FirmTrain::cancel_train_trainee ---------//
//
void FirmTrain::cancel_train_trainee(int traineeId, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_CANCEL_TRAIN, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = traineeId;
		return;
	}

	if( traineeId < 0 || traineeId > trainee_count )
		return;

	Trainee *traineePtr = &trainee_array[traineeId-1];

	if( !traineePtr->is_under_training )		// may happen in multiplayer
		return;

	int unitId = traineePtr->unit_id;
	// backup unit id as it will be mobilized

	int sourceTown = 0;
	Town *townPtr = NULL;

	// ----- if it he can put back to the origin town -----//

	if( traineePtr->source_town_recno
		&& !town_array.is_deleted(traineePtr->source_town_recno)
		&& (townPtr = town_array[traineePtr->source_town_recno])
		&& townPtr->race_id == traineePtr->race_id
		&& townPtr->nation_recno == nation_recno
		&& townPtr->population < MAX_TOWN_POPULATION )
	{
		sourceTown = traineePtr->source_town_recno;
	}

	// ----- scan any suitable linked town ------//

	if( !sourceTown )
	{
		for( int i = 0; i < linked_town_count; ++i )
		{
			err_when( town_array.is_deleted(linked_town_array[i]));
			townPtr = town_array[linked_town_array[i]];

			if( townPtr->race_id == traineePtr->race_id
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
		townPtr->inc_pop( traineePtr->loyalty );

		//------ if the unit is a spy -------//

		if( traineePtr->spy_recno > 0 )
		{
			spy_array[traineePtr->spy_recno]->set_place(SPY_TOWN, sourceTown);
			traineePtr->spy_recno = 0;		// reset it so kill_trainee() won't delete the spy
		}

		kill_trainee(traineeId);
	}

	// ------ mobilize to civilian -------//

	else if( mobilize_trainee(traineeId, COMMAND_AUTO) )
	{
	}

	else
	{
		// -------- kill trainee ------//

		kill_trainee(traineeId);
	}

	// ---- return money -----//

	if( nation_recno )
	{
		UnitInfo *unitInfo = unit_res[unitId];
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) -unitInfo->build_cost );
	}

}
//------- End of function FirmTrain::cancel_train_trainee ---------//


//------- Begin of function FirmTrain::validate_trainee ---------//
//
void FirmTrain::validate_trainee()
{
	// validate each trainee's source town recno

	for(int i = 1; i <= trainee_count; ++i )
	{
		Trainee *traineePtr = &trainee_array[i-1];
		if( traineePtr->source_town_recno )
		{
			if( town_array.is_deleted(traineePtr->source_town_recno)
				|| town_array[traineePtr->source_town_recno]->race_id != traineePtr->race_id
				|| town_array[traineePtr->source_town_recno]->nation_recno != nation_recno )
			{
				traineePtr->source_town_recno = 0;
			}
		}
	}
}
//------- End of function FirmTrain::validate_trainee ---------//
