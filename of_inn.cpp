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

//Filename    : OF_INN.CPP
//Description : Firm Military Inn

#include <OF_INN.H>
#include <OINFO.H>
#include <OMODEID.H>
#include <OSYS.H>
#include <OSTR.H>
#include <OHELP.H>
#include <OGAME.H>
#include <OTOWN.H>
#include <ONATION.H>
#include <ORAWRES.H>
#include <OHERORES.H>
#include <ORACERES.H>
#include <OITEMRES.H>
#include <ITEMID.H>
#include <OWORLD.H>
#include <OSPY.H>
#include <OREMOTE.H>
#include <OSERES.H>
#include <ORACERES.H>
#include <OSITE.H>


//--------- Begin of function FirmInn::FirmInn ---------//
//
FirmInn::FirmInn()
{
}
//----------- End of function FirmInn::FirmInn -----------//


//--------- Begin of function FirmInn::~FirmInn ---------//
//
FirmInn::~FirmInn()
{
}
//----------- End of function FirmInn::~FirmInn -----------//


//--------- Begin of function FirmInn::init_derived ---------//
//
void FirmInn::init_derived()
{
	Firm::init_derived();
	inn_unit_count = 0;
}
//----------- End of function FirmInn::init_derived -----------//


//--------- Begin of function FirmInn::deinit_derived ---------//
//
void FirmInn::deinit_derived()
{
	Firm::deinit_derived();
}
//----------- End of function FirmInn::deinit_derived -----------//


//----------- Begin of function FirmInn::free_all_people --------//
//
void FirmInn::free_all_people()
{
	Firm::free_all_people();

	if( !sys.signal_exit_flag )
	{
		for(int i = inn_unit_count; i > 0; --i )
		{
			if( inn_unit_array[i-1].spy_recno && (transfer_inn_unit(i) || hire(i, 1)) )
			{
				// spy try to transfer to hire inn, if fails, escape from the inn
			}
			else
			{
				del_inn_unit(i);
			}
		}
	}
	else
	{
		for(int i = inn_unit_count; i > 0; --i )
		{
			del_inn_unit(i);
		}
	}
}
//----------- End of function FirmInn::free_all_people --------//


//--------- Begin of function FirmInn::assign_unit ---------//
//
void FirmInn::assign_unit(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	//--- can only assign if the unit is a spy cloaked into independent ---//

	if( !can_accept_assign(unitRecno) )
		return;

	//---------------------------------------------//

	if( inn_unit_count == MAX_INN_UNIT 
		|| count_soldier() >= MAX_INN_UNIT/2 )
		return;

	err_when( inn_unit_count >= MAX_INN_UNIT );

	//--------- add the InnUnit --------//

	InnUnit *innUnit = inn_unit_array+inn_unit_count;
	memset( innUnit, 0, sizeof(InnUnit) );

	inn_unit_count++;

	//--------- set InnUnit vars -----------------//

	innUnit->unit_id    = unitPtr->unit_id;
	innUnit->skill      = unitPtr->skill;
	innUnit->hit_points = unitPtr->hit_points;
	innUnit->hero_id    = unitPtr->hero_id;
	unitPtr->hero_id = 0;
	innUnit->unique_id  = unitPtr->unique_id;
	innUnit->item       = unitPtr->item;
	unitPtr->item.clear();

	int spyRecno = innUnit->spy_recno = unitPtr->spy_recno;
	spy_array[spyRecno]->set_place(SPY_FIRM, firm_recno);
	unitPtr->spy_recno = 0;				// prevent Unit::deinit call spy_array.del_spy

	innUnit->set_hire_cost();
	innUnit->stay_count = 10 + m.random(5);
	unitPtr->deinit_sprite();
	unit_array.disappear_in_firm(unitRecno);
}
//----------- End of function FirmInn::assign_unit -----------//


//--------- Begin of function FirmInn::kill_unit ---------//
//
int FirmInn::kill_unit(int recNo)
{
	if( recNo < 1 || recNo > inn_unit_count )
		return 0;

	del_inn_unit(recNo);
	return 1;
}
//--------- End of function FirmInn::kill_unit---------//


//--------- Begin of function FirmInn::hire ---------//
//
// <short> recNo			inn unit recno
// [int] spyEscape      whether this is an instance which a spy escape from the inn, instead of hiring a unit. i.e. no hire cost required
//								(default: 0)
// [short] exitFirmRecno  exit inn's firm recno if not same as this inn (see transfer_inn_unit)
//                      (default :0)
//
int FirmInn::hire(short recNo, int spyEscape, short exitFirmRecno)
{
	err_when( recNo < 1 );

	if( recNo > inn_unit_count )		// this may happen in a multiplayer game
		return 0;

	//--------- first check if you have enough money to hire ------//

	Nation* 	nationPtr = nation_array[nation_recno];
	InnUnit* innUnit = inn_unit_array+recNo-1;
	int unitRecno = 0;

	err_when( exitFirmRecno && firm_array.is_deleted(exitFirmRecno) );

	if( innUnit->spy_recno && (spyEscape 											// no expense if mobilizing spy
		|| innUnit->true_nation_recno() == nation_recno) )	// no expense if it is own spy
	{
		// free, no need to play money
		unitRecno = (exitFirmRecno ? firm_array[exitFirmRecno] : this)->create_unit(innUnit->unit_id );
		if( !unitRecno )
			return 0;		// no space for creating the unit
	}
	else
	{
		if( nationPtr->cash < innUnit->hire_cost )
			return 0;
		unitRecno = (exitFirmRecno ? firm_array[exitFirmRecno] : this)->create_unit( innUnit->unit_id );
		if( !unitRecno )
			return 0;		// no space for creating the unit
		nationPtr->add_expense(EXPENSE_HIRE_UNIT, innUnit->hire_cost);
	}

	//-------- set skills of the unit --------//

	err_when( !unitRecno );

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->skill 	  = innUnit->skill;
	unitPtr->hit_points = innUnit->hit_points;

	err_when( innUnit->combat_level()<=0 || innUnit->combat_level()>1000 );

	//----------------------------------------//
	//
	// Set loyalty of the hired unit
	//
	// = 30 + the nation's reputation / 2 + 30 if racially homogenous
	//
	//----------------------------------------//

	int unitLoyalty = 30 + (int)nationPtr->reputation/2;
	if( innUnit->spy_recno && (spyEscape
		|| innUnit->true_nation_recno() == nation_recno) )
	{
		unitLoyalty = spy_array[innUnit->spy_recno]->spy_loyalty;
	}

	if( race_res.is_same_race(unitPtr->race_id, nationPtr->race_id) )
		unitLoyalty += 20;

	unitLoyalty = max( 40, unitLoyalty );
	unitLoyalty = min( 100, unitLoyalty );

	unitPtr->loyalty = unitLoyalty;
	// ######## begin Gilbert 24/2 #######//
	unitPtr->unique_id = innUnit->unique_id;		// set unique before add_spy/set_spy_place
	// ######## end Gilbert 24/2 #######//

	//----------------------------------------------------//
	//
	// If spy_skill>0, this unit is a spy with a
	// spying skill for hire, he is added by FirmInn
	//
	//----------------------------------------------------//

	if( innUnit->spy_skill > 0 )
	{
		unitPtr->spy_recno = spy_array.add_spy(unitRecno, innUnit->spy_skill);
		spy_array[unitPtr->spy_recno]->spy_loyalty = unitLoyalty;		// this is the spy's true loyalty towards you
	}

	//----------------------------------------------------//
	//
	// If spy_recno>0, this unit is an enemy spy sneaked
	// into the inn.
	//
	//----------------------------------------------------//

	else if( innUnit->spy_recno )
	{
		err_when( spy_array.is_deleted(innUnit->spy_recno) );
		Spy *spyPtr = spy_array[innUnit->spy_recno];
		unitPtr->spy_recno = innUnit->spy_recno;
		innUnit->spy_recno = 0;								// prevent del_inn_unit call spy_array.del_spy
		unitPtr->loyalty = unitLoyalty;					// this is his fake loyalty towards you
		// ##### begin Gilbert 1/3 ########//
		if( unitPtr->race_id && spyPtr->name_id )
		{
			unitPtr->set_name( spyPtr->name_id );
		}
		// ##### end Gilbert 1/3 ########//
		spy_array[unitPtr->spy_recno]->set_place(SPY_MOBILE, unitRecno);

		if( spyEscape )
		{
			unitPtr->spy_change_nation(spyPtr->true_nation_recno, COMMAND_AUTO);
		}
	}

	// ------ hero id -------//

	unitPtr->hero_id = innUnit->hero_id;
	innUnit->hero_id = 0;
	// ##### begin Gilbert 24/2 #######//
	// unitPtr->unique_id = innUnit->unique_id;
	// ##### end Gilbert 24/2 #######//

	// ------ set item -------//

	unitPtr->item = innUnit->item;

	// ###### begin Gilbert 4/11 ######//
	unitPtr->set_combat_level(-1);
	// ###### end Gilbert 4/11 ######//

	//---- remove the record from the hire list ----//

	del_inn_unit(recNo);

	// ##### begin Gilbert 24/2 #######//
	// ------- update win/lose condition on changing of unit recno -----//

	game.update_condition_on_mobilize(unitRecno, unitPtr->unique_id );
	// ##### end Gilbert 24/2 #######//

	return unitRecno;
}
//----------- End of function FirmInn::hire -----------//


//--------- Begin of function FirmInn::next_day ---------//
//
void FirmInn::next_day()
{
	//----------- run next_day() of the base class -------------//

	Firm::next_day();

	//-------- update the hire list ------------//

	int updateInterveal = 10 + info.year_passed*2;		// there will be less and less units to hire as the game passes

	if( info.game_date % updateInterveal == firm_recno % updateInterveal )
		update_add_hire_list();

	if( info.game_date % updateInterveal == firm_recno % updateInterveal )
		update_del_hire_list();

	//--------- recover hit points ----------//

	if( info.game_date%15 == firm_recno%15 )
	{
		recover_hit_point();
	}
}
//----------- End of function FirmInn::next_day -----------//


//---------- Begin of function FirmInn::update_add_hire_list --------//
//
void FirmInn::update_add_hire_list()
{
	//-------- new units come by --------//

	if( inn_unit_count < MAX_INN_UNIT-1 )		// -1 so that there is always one free slot for spies
	{
		if( should_add_inn_unit() )
		{
			add_inn_unit( m.random(MAX_RACE)+1 );
		}
	}
}
//----------- End of function FirmInn::update_add_hire_list ---------//


//--------- Begin of function FirmInn::update_del_hire_list --------//
//
void FirmInn::update_del_hire_list()
{
	//------- existing units leave -------//

	for( int i=inn_unit_count ; i>0 && inn_unit_count>0 ; i-- )
	{
		if( --inn_unit_array[i-1].stay_count==0 )
		{
			if( inn_unit_array[i-1].spy_recno && (transfer_inn_unit(i) || hire(i, 1)) )
			{
				// spy try to transfer to hire inn, if fails, escape from the inn
			}
			else
			{
				del_inn_unit(i);
			}
/*
			if( firm_recno == firm_array.selected_recno )
			{
				if( browse_hire.recno() > i && browse_hire.recno() > 1 )
					browse_hire.refresh( browse_hire.recno()-1, inn_unit_count );
			}
*/		}
	}
}
//--------- End of function FirmInn::update_del_hire_list ---------//


//-------- Begin of function FirmInn::should_add_inn_unit --------//
//
int FirmInn::should_add_inn_unit()
{
	FirmInn* firmInn;
	int 		totalInnUnit = inn_unit_count;

	for(int i=0; i<linked_firm_count; i++)
	{
		firmInn = (FirmInn*) firm_array[linked_firm_array[i]];   // links between inns are stored in linked_firm_array[] for quick scan only

		err_when( firmInn->firm_id != FIRM_INN );

		totalInnUnit += firmInn->inn_unit_count;
	}

	return totalInnUnit < MAX_INN_UNIT_PER_REGION;
}
//--------- End of function FirmInn::should_add_inn_unit ---------//


//--------- Begin of function FirmInn::add_inn_unit ---------//

void FirmInn::add_inn_unit(int raceId)
{
	err_when( inn_unit_count >= MAX_INN_UNIT );

	InnUnit *innUnit = inn_unit_array+inn_unit_count;
	RaceInfo *raceInfo = race_res[raceId];

	int spyFlag = 0;	// must determine before inn_unit_count increase

	if( count_soldier() >= MAX_INN_UNIT/2 )
		spyFlag = 1;
	else if( count_spy() >= MAX_INN_UNIT/2 )
		spyFlag = 0;
	else
		spyFlag = m.random(2);

	memset( innUnit, 0, sizeof(InnUnit) );

	inn_unit_count++;

	//---- determine whether the unit is a spy or a military unit ----//

	if( !spyFlag )		// a military unit
	{
		//------- create a unit now -----------//

		int unitRecno;

		if( m.random(20)==0 )		// 5% have a hero
		{
			unitRecno = hero_res.create_hero( -1, -1, 0, 0, 0, 0, 1);		// -1 - create the unit as an invisible unit
		}																								// last 1 - the heroes are for hire, check the HeroInfo::for_hire before adding it
		else		//---- just create a unit, not a hero  -----//
		{
			int unitId;

			int heroPower = 20 + m.random(20);

			if( m.random(2)==0 )
				unitId = raceInfo->infantry_unit_id;
			else
				unitId = raceInfo->special_unit_id;

			unitRecno = hero_res.create_powerful_unit( -1, -1, 0, unitId, heroPower);		// -1 - create the unit as an invisible unit
		}

		//--------------------------------------//

		if( !unitRecno )
		{
			inn_unit_count--;
			return;
		}

		//--------------------------------------//

		Unit* unitPtr = unit_array[unitRecno];

		err_when( !unitPtr->unit_id );

		innUnit->hero_id 	  = unitPtr->hero_id;
		innUnit->unit_id	  = unitPtr->unit_id;
		innUnit->skill      = unitPtr->skill;
		innUnit->hit_points = unitPtr->hit_points;
		innUnit->item       = unitPtr->item;
		unitPtr->item.clear();

		unit_array.del(unitRecno);		// delete the unit as it was created temporarily for setting skill and item values
	}
	else							// a spy
	{
		switch( m.random(3) )
		{
		case 0:					// civilian spy
			innUnit->unit_id = (char) raceInfo->civilian_unit_id;
			innUnit->skill.init(innUnit->unit_id, CITIZEN_COMBAT_LEVEL, 0);
			innUnit->spy_skill			 = 30+m.random(40);
			break;
		case 1:					// infantry spy
			innUnit->unit_id = (char) raceInfo->infantry_unit_id;
			innUnit->skill.init(innUnit->unit_id, 20+m.random(30), 20+m.random(30));	// a spy also have leadership and combat ability, but lower than units with leadership as their main skills
			innUnit->spy_skill			 = 30+m.random(40);
			break;
		case 2:					// special unit spy
			innUnit->unit_id = (char) raceInfo->special_unit_id;
			innUnit->skill.init(innUnit->unit_id, 30+m.random(30), 10+m.random(20));	// a spy also have leadership and combat ability, but lower than units with leadership as their main skills
			innUnit->spy_skill			 = 30+m.random(40);
			break;
		default:
			err_here();
		}

		if( m.random(4)==0 )
			innUnit->spy_skill += m.random(30);

		else if( m.random(2)==0 )
			innUnit->spy_skill += m.random(15);
	}

	err_when( !innUnit->unit_id );

	// ------ randomly assign a unique id --------//

	// ###### begin Gilbert 23/2 ######//
	innUnit->unique_id = m.rand_long();
	// ###### end Gilbert 23/2 ######//

	// ------ set current hit point -------//

	innUnit->hit_points = (float) innUnit->max_hit_points();

	//-------------------------------------------//

	innUnit->set_hire_cost();
	innUnit->stay_count = 10 + m.random(5);
}
//----------- End of function FirmInn::add_inn_unit -----------//


//--------- Begin of function InnUnit::set_hire_cost ---------//
//
void InnUnit::set_hire_cost()
{
	hire_cost = hero_res.hire_cost( combat_level(), skill.max_combat_level,
					skill_level(), spy_skill, &item );
}
//----------- End of function InnUnit::set_hire_cost -----------//


// --------- Begin of function InnUnit::is_own_spy ----------//
//
int InnUnit::is_own_spy()
{
	return spy_recno && spy_array[spy_recno]->true_nation_recno == nation_array.player_recno;
}
// --------- End function InnUnit::is_own_spy ----------//


// --------- Begin of function InnUnit::true_nation_recno ----------//
//
int InnUnit::true_nation_recno()
{
	if( spy_recno )
		return spy_array[spy_recno]->true_nation_recno;
	return 0;
}
// --------- End of function InnUnit::true_nation_recno ----------//


//--------- Begin of function FirmInn::del_inn_unit ---------//

void FirmInn::del_inn_unit(int recNo)
{
	err_when( recNo<1 || recNo>MAX_INN_UNIT );

	err_when( inn_unit_count < 1 );
	err_when( inn_unit_count > MAX_INN_UNIT );

	InnUnit *innUnit = inn_unit_array + recNo - 1;

	//------ if the unit is a hero, reset the appeared_flag ---//

	if( innUnit->hero_id )
	{
		err_when( hero_res[innUnit->hero_id]->appeared_flag != 1 );

		hero_res[innUnit->hero_id]->appeared_flag = 0;
	}

	//------------------------------------------//

	if( innUnit->spy_recno )
	{
		// BUGHERE : news nessage?
		spy_array.del_spy(innUnit->spy_recno);
	}

	// ######## begin Gilbert 6/1 ########//
	InnUnit selectedInnUnit;
	int recoverSelected = 0;
	if( firm_array.selected_recno == firm_recno )
	{
		if( selected_unit_id > 0 && selected_unit_id <= inn_unit_count )
		{
			if( selected_unit_id > recNo )
			{
				selectedInnUnit = inn_unit_array[selected_unit_id-1];
				recoverSelected = 1;
			}
			else if( selected_unit_id == recNo )
			{
				selected_unit_id = 0;	// not selected anything if deleting selected unit
			}
		}
	}
	// ######## end Gilbert 6/1 ########//

	m.del_array_rec(inn_unit_array, inn_unit_count, sizeof(InnUnit), recNo);

	inn_unit_count--;

	// ###### begin Gilbert 6/1 #####//

	//--- keep the selected unit to the same unit before delete ---//

	if( firm_array.selected_recno == firm_recno )
	{
		if( recoverSelected )
		{
			for( --selected_unit_id; selected_unit_id > 0 && memcmp(&inn_unit_array[selected_unit_id-1], &selectedInnUnit, sizeof(InnUnit)); --selected_unit_id );
		}
	}
	// ###### end Gilbert 6/1 #####//
}
//----------- End of function FirmInn::del_inn_unit -----------//


//-------- Begin of function FirmInn::transfer_inn_unit --------//
//
int FirmInn::transfer_inn_unit(int recNo)
{
	err_when( recNo < 1 );
	
	if( recNo > inn_unit_count )
		return 0;

	InnUnit *innUnit = inn_unit_array+recNo-1;

	if( !innUnit->spy_recno )
		return 0;					// only spy considers transfer

	Spy *spyPtr = NULL;
	if( innUnit->spy_recno && !spy_array.is_deleted(innUnit->spy_recno) )
		spyPtr = spy_array[innUnit->spy_recno];

	// -------- scan any inn firm in world ----------//

	int ownFirmRecno = 0;
	int anyOwnInn = 0;
	int firmRecno = 1+m.random(firm_array.size());

	for( int firmCount = firm_array.size(); firmCount > 0; --firmCount, ++firmRecno )
	{
		if( firmRecno > firm_array.size() )
			firmRecno -= firm_array.size();
		err_when(firmRecno < 1 || firmRecno > firm_array.size());

		if( firm_array.is_deleted(firmRecno) )
			continue;

		FirmInn *firmInn = firm_array[firmRecno]->cast_to_FirmInn();

		if( !firmInn )
			continue;

		if( spyPtr && firmInn->nation_recno == spyPtr->true_nation_recno )
		{
			if( !anyOwnInn )
				anyOwnInn = firmRecno;
		}

		if( firmRecno == firm_recno )		// never go back to the same inn
			continue;

		if( firmInn->inn_unit_count >= MAX_INN_UNIT )
			continue;

		if( spyPtr && firmInn->nation_recno == spyPtr->true_nation_recno )
		{
			if( !ownFirmRecno )
				ownFirmRecno = firmRecno;
			continue;							// spy tend not to go back to own firmInn, unless no other choice
		}

		break;
	}

	if( firmCount <= 0 )				// if no other firm found, go back to own firm
		firmRecno = ownFirmRecno;

	if( firmRecno )					// if no any firm to go return false
	{
		// ------ transfer inn unit -------//

		FirmInn *firmInn = firm_array[firmRecno]->cast_to_FirmInn();		
		err_when( !firmInn );
		InnUnit *newInnUnit = firmInn->inn_unit_array + firmInn->inn_unit_count;
		firmInn->inn_unit_count++;
		*newInnUnit = *innUnit;

		// ------- generate new stay count ------//

		newInnUnit->stay_count = 10 + m.random(5);
		newInnUnit->set_hire_cost();

		// ------ trasfer Spy ------- //

		if( spyPtr )
			spyPtr->set_place(SPY_FIRM, firmRecno);
		innUnit->spy_recno = 0;					// prevent del_inn_unit call spy_array.del_spy

		// ------ delete inn unit of current inn ------//

		del_inn_unit(recNo);
		innUnit = newInnUnit;			// set innUnit to the transferred place for later use
	}
	else if( (firmRecno = anyOwnInn) )
	{
		// mobilize in another any own inn

		hire( recNo, 1, firmRecno );
		innUnit = NULL;
	}

	return firmRecno;
}
//-------- End of function FirmInn::transfer_inn_unit --------//


//------- Begin of function FirmInn::recover_hit_point -------//
//
// Units recover their hit points. Units assigned to this firm
// by players will have lower than maximum hit points
//
void FirmInn::recover_hit_point()
{
	InnUnit* innUnit = inn_unit_array;

	for( int i=0 ; i<inn_unit_count ; i++, innUnit++ )
	{
		int hpInc = 1;
		short maxHp = innUnit->max_hit_points();

		//------- item effect -------- //

		hpInc += ( 1 + innUnit->item.ability(ITEM_ABILITY_RECOVERY) ) / 2;

		//------- increase soldier hit points --------//

		if( innUnit->hit_points < maxHp )
		{
			innUnit->hit_points += hpInc;

			if( innUnit->hit_points > maxHp )
				innUnit->hit_points = maxHp;
		}
	}
}
//------- End of function FirmInn::recover_hit_point -------//


//-------- Begin of function FirmInn::can_accept_assign ------//
//
// Return whether this firm accepts a new unit.
//
// <int> unitRecno - recno of the unit
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
bool FirmInn::can_accept_assign(int unitRecno, int actionNationRecno)
{
	if( under_construction )
		return false;

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->rank_id == RANK_KING )		// it cannot be a king
		return false;

	return unitPtr->spy_recno && !unitPtr->is_civilian() &&		// it must be a military unit 
			 unitPtr->nation_recno==0;										// it must be an independent unit and it must be a spy
}
//---------- End of function FirmInn::can_accept_assign --------//


//-------- Begin of function FirmInn::count_soldier ------//
//
int FirmInn::count_soldier()
{
	int soldierCount = 0;
	for( int i = 0; i < inn_unit_count; ++i )
	{
		if( inn_unit_array[i].spy_skill <= 0 )
			++soldierCount;
	}
	return soldierCount;
}
//-------- End of function FirmInn::count_soldier ------//


//-------- Begin of function FirmInn::count_spy ------//
//
int FirmInn::count_spy()
{
	int spyCount = 0;
	for( int i = 0; i < inn_unit_count; ++i )
	{
		if( inn_unit_array[i].spy_skill > 0 )
			++spyCount;
	}
	return spyCount;
}
//-------- End of function FirmInn::count_spy ------//


// ------- Begin of function FirmInn::buy_item ------//
//
// checkingFlag : false = buy , 1 = check if can buy
// 
int FirmInn::buy_item( short recNo, int checkingFlag )
{
	err_when( recNo < 1 );

	if( recNo > inn_unit_count )		// this may happen in a multiplayer game
		return 0;

	//--------- first check if you have enough money to hire, unless own spy ------//

	Nation* 	nationPtr = nation_array[nation_recno];
	InnUnit* innUnit = inn_unit_array+recNo-1;

	if( !innUnit->item.id )
		return 0;

	if( innUnit->true_nation_recno() != nation_recno && nationPtr->cash < innUnit->item.cost() )
		return 0;

	if( !checkingFlag )
	{
		// drop the item outside

		if( site_array.add_site( loc_x2, loc_y2, SITE_ITEM, innUnit->item.id, innUnit->item.para )
			|| site_array.add_site( loc_x2, loc_y1, SITE_ITEM, innUnit->item.id, innUnit->item.para )
			|| site_array.add_site( loc_x1, loc_y2, SITE_ITEM, innUnit->item.id, innUnit->item.para )
			|| site_array.add_site( loc_x1, loc_y1, SITE_ITEM, innUnit->item.id, innUnit->item.para ) )
		{
			// reduce nation cash

			if( innUnit->true_nation_recno() != nation_recno )
			{
				nationPtr->add_expense( EXPENSE_HIRE_UNIT, (float) innUnit->item.cost(), 0 );
			}

			// clear item

			innUnit->item.clear();

			// recalc hire cost

			innUnit->set_hire_cost();

			return 1;
		}

		return 0;
	}

	return 1;
}
// ------- End of function FirmInn::buy_item ------//
