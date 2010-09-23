//Filename    : OF_SPYA.CPP
//Description : Firm espionage college - AI functions

#include <ONATION.H>
#include <OINFO.H>
#include <OTOWN.H>
#include <OF_SPY.H>

//--------- Begin of function FirmSpy::process_ai ---------//

void FirmSpy::process_ai()
{
	//---- think about training a new spy  ----//

	if( info.game_date%30==firm_recno%30 )
	{
		think_train_new();
	}

	//--- think about assigning missions to existing spies ----//

	if( info.game_date%30==(firm_recno+15)%30 )
	{
		think_spy_action();
	}

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmSpy::process_ai -----------//


//------- Begin of function FirmSpy::think_del -----------//
//
// Think about deleting this firm.
//
int FirmSpy::think_del()
{
	if( linked_town_count==0 )
	{
		ai_del_firm();
		return 1;
	}

	return 0;
}
//--------- End of function FirmSpy::think_del -----------//


//----- Begin of function FirmSpy::think_train_new ------//
//
// Think about training a new spy.
//
void FirmSpy::think_train_new()
{
	//---- don't recruit spy if we are low in cash or losing money ----//

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->total_population < 30-nationPtr->pref_spy/10 )		// don't use spies if the population is too low, we need to use have people to grow population
		return;

	if( nationPtr->total_spy_count > nationPtr->total_population * (10+nationPtr->pref_spy/5) / 100 )		// 10% to 30%
		return;

	if( !nationPtr->ai_should_spend(25+nationPtr->pref_spy/2) )		// 0 to 50
		return;

	//--- the expense of spies should not be too large ---//

	if( nationPtr->expense_365days(EXPENSE_SPY) >
		 nationPtr->expense_365days() * (50+nationPtr->pref_spy) / 400 )
	{
		return;
	}

	//----- determine which town to train the special unit from ---//

	int linkId = m.random( linked_town_count );

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( ++linkId >= linked_town_count )
			linkId = 0;

		Town* townPtr = town_array[linked_town_array[linkId]];

		//------ think about training a civilian spy -------//

		if( m.random(2)==0 )
		{
			if( has_spy(townPtr->race_id, true) )		// true - is civilian
				continue;

			if( can_train_from_town(townPtr->town_recno, required_linked_firm[TRAIN_UNIT_CIVILIAN]) )
			{
				int rc, counterSpy;

				if( nationPtr->pref_counter_spy < 25 )
					counterSpy = m.random(3)==0;		// 33% chance

				else if( nationPtr->pref_counter_spy < 75 )
					counterSpy = m.random(2)==0;		// 50% chance

				else
					counterSpy = m.random(3)!=0;		// 66% chance

				if( counterSpy )
					rc = nationPtr->think_assign_spy_own_town(townPtr->race_id, region_id);
				else
					rc = nationPtr->think_assign_spy_target_town(townPtr->race_id, region_id);

				if( rc )
				{
					recruit_trainee(townPtr->town_recno, TRAIN_UNIT_CIVILIAN, COMMAND_AI);
					return;
				}
			}
		}
		else
		{
			//------ think about training a military spy -------//

			if( has_spy(townPtr->race_id, false) )		// false - is not civilian
				continue;

			int trainType = m.random(2)==0 ? TRAIN_UNIT_INFANTRY : TRAIN_UNIT_SPECIAL;

			if( can_train_from_town(townPtr->town_recno, required_linked_firm[trainType]) )
			{
				if( nationPtr->think_assign_spy_target_camp(townPtr->race_id, region_id) )
				{
					recruit_trainee(townPtr->town_recno, trainType, COMMAND_AI);
					return;
				}
			}
		}
	}
}
//------ End of function FirmSpy::think_train_new -------//


//----- Begin of function FirmSpy::has_spy ------//
//
// Return whether this firm has a spy of the specific race and type.
//
int FirmSpy::has_spy(int raceId, int isCivilian)
{
	for( int i=0 ; i<trainee_count ; i++ )
	{
		if( trainee_array[i].race_id == raceId &&
			 unit_res[ trainee_array[i].unit_id ]->is_civilian == isCivilian )
		{
			return 1;
		}
	}

	return 0;
}
//------ End of function FirmSpy::has_spy -------//


//----- Begin of function FirmSpy::think_spy_action ------//
//
// Think about assign missions to trained spies in this firm.
//
void FirmSpy::think_spy_action()
{
	Nation* nationPtr = nation_array[nation_recno];
	int traineeId = m.random(trainee_count)+1;

	//--- if the nation's reputation is negative, then only do not produce spies too often ---//

	if( nationPtr->reputation < 0 )
	{
		if( m.random(-nationPtr->reputation/5) != 0 )
			return;
	}

	//-----------------------------------------//

	for( int i=0 ; i<trainee_count ; i++ )
	{
		if( ++traineeId > trainee_count )
			traineeId = 1;

		Trainee* traineePtr = trainee_array+traineeId-1;

		if( traineePtr->is_under_training )
			continue;

		//----- if this is a civilian ------//

		Place* placePtr = NULL;

		if( unit_res[traineePtr->unit_id]->is_civilian )
		{
			int townRecno, counterSpy;

			if( nationPtr->pref_counter_spy < 25 )
				counterSpy = m.random(3)==0;		// 33% chance

			else if( nationPtr->pref_counter_spy < 75 )
				counterSpy = m.random(2)==0;		// 50% chance

			else
				counterSpy = m.random(3)!=0;		// 66% chance

			if( counterSpy )
				townRecno = nationPtr->think_assign_spy_own_town(traineePtr->race_id, region_id);
			else
				townRecno = nationPtr->think_assign_spy_target_town(traineePtr->race_id, region_id);

			if( townRecno )
				placePtr = town_array[townRecno];
		}
		else
		{
			//---- try assign it to a camp ----//

			int firmRecno = nationPtr->think_assign_spy_target_camp(traineePtr->race_id, region_id);

			//--- if can't assign to a camp, try assigning it to an inn ---//

			if( !firmRecno )
				firmRecno = nationPtr->think_assign_spy_target_inn(region_id);

			if( firmRecno )
				placePtr = firm_array[firmRecno];
		}

		if( !placePtr )
			continue;

		//------ assign the spy to a place --------//

		int unitRecno = mobilize_trainee(traineeId, COMMAND_AI);

		if( !unitRecno )
			continue;

		if( nationPtr->add_action( placePtr->loc_x1, placePtr->loc_y1,
											-1, -1, ACTION_AI_ASSIGN_SPY, placePtr->nation_recno, 1, unitRecno ) )
		{
			return;
		}
	}
}
//------ End of function FirmSpy::think_spy_action -------//

