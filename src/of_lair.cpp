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
// Description : Firm Monster Lair

#include <all.h>
#include <gamedef.h>
#include <osys.h>
#include <oremote.h>
#include <onation.h>
#include <ounitres.h>
#include <omonsres.h>
#include <otechres.h>
#include <onews.h>
#include <of_lair.h> 

//------ begin of function FirmLair::FirmLair -------//
//
FirmLair::FirmLair()
{ 
	memset( sizeof(FirmCamp)+(char *)this, 0, sizeof(FirmLair)-sizeof(FirmCamp) );
}
//------ end of function FirmLair::FirmLair -------//


//------ begin of function FirmLair::init_derived -------//
//
void FirmLair::init_derived()
{
	FirmCamp::init_derived();
}
//------ end of function FirmLair::init_derived -------//


//--------- Begin of function FirmLair::next_day ---------//
//
void FirmLair::next_day()
{
	FirmCamp::next_day();

	if( (unsigned)info.game_date % 16 == (unsigned)firm_recno % 16 )
		natural_breed();

	err_when( nation_recno && nation_array[nation_recno]->is_human() ); 	// humans should not have any firm lairs
}
//----------- End of function FirmLair::next_day -----------//


//----------- Begin of function FirmLair::recruit_solder -----------//
//
// <char> remoteAction - remote action flag
// <char> noCost		  - if this is 1, the unit will be created with no cost.
//
void FirmLair::recruit_soldier(char remoteAction, char noCost)
{
	if( !can_recruit() )
		return;

	if( nation_recno && nation_array[nation_recno]->cash < (float)RECRUIT_MONSTER_COST )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ######//
		// packet structure : <firm recno> [source town recno] [train type]
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_RECRUIT, sizeof(short) );
		*shortPtr = firm_recno;
		// ##### end Gilbert 7/10 ######//
		return;
	}

	Soldier *soldierPtr = soldier_array + soldier_count;
	soldier_count++;

	// -------- initialize solder -----------//

	memset( soldierPtr, 0, sizeof(Soldier) );

	soldierPtr->loyalty = 60;
	soldierPtr->race_id = race_id;
	soldierPtr->unit_id = (char)monster_res[monster_id()]->unit_id;
	soldierPtr->rank_id = RANK_SOLDIER;
	soldierPtr->remain_build_days = unit_res[soldierPtr->unit_id]->build_days;
	soldierPtr->init_name();
	// ####### begin Gilbert 2/3 ######//
	soldierPtr->unique_id = m.rand_long();
	// ####### end Gilbert 2/3 ######//
	soldierPtr->source_town_recno = 0;

	// ------ setting new skill ---------//

	soldierPtr->skill.init(soldierPtr->unit_id);
	soldierPtr->hit_points = soldierPtr->max_hit_points();

	//------ increase nation soldier count ---------//

	UnitInfo* unitInfo = unit_res[soldierPtr->unit_id];

	if( nation_recno )
		unitInfo->inc_nation_unit_count(nation_recno);

	// ------- add expense ----------//

	if( nation_recno && !noCost )
	{
		nation_array[nation_recno]->add_expense(EXPENSE_TRAIN_UNIT, (float) unitInfo->build_cost );
		nation_array[nation_recno]->change_live_points( (float) -unitInfo->build_live_points_cost );
	}
}
//----------- End of function FirmLair::recruit_solder -----------//


//------ begin of function FirmLair::can_recruit -------//
//
int FirmLair::can_recruit()
{
	if( is_soldier_full() )
		return 0;

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];
		UnitInfo* unitInfo = unit_res[ monster_res[monster_id()]->unit_id ];

		return nationPtr->cash >= unitInfo->build_cost &&
				 nationPtr->live_points >= unitInfo->build_live_points_cost;
	}
	else
	{
		return 1;
	}
}
//------ end of function FirmLair::can_recruit ---------//


// #### begin Gilbert 26/1 ########//
//------- Begin of function FirmLair::cancel_train_soldier -------//
//
// Cancel training units.
// ignore soldierId and cancel always the last soldier
//
void FirmLair::cancel_train_soldier(int soldierId, char remoteAction)
{
	// ##### begin Gilbert 7/10 ######//
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_CANCEL_TRAIN, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = soldierId;
		return;
	}
	// ##### end Gilbert 7/10 ######//

	// ------ scan the from specified soldier and backward ------//

	if( soldierId > soldier_count )
		soldierId = soldier_count;

	for( ; soldierId > 0; --soldierId )
	{
		if( soldier_array[soldierId-1].is_under_training() )
		{
			int unitId = soldier_array[soldierId-1].unit_id;
			// backup unit id as it will be killed

			kill_soldier(soldierId);

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
// #### end Gilbert 26/1 ########//
//------- End of function FirmLair::cancel_train_soldier -------//


//------ begin of function FirmLair::pay_expense -------//
//
void FirmLair::pay_expense()
{
	FirmCamp::pay_expense();
}
//------ end of function FirmLair::pay_expense -------//


//------ begin of function FirmLair::being_attack_hit -------//
//
void FirmLair::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	err_when( base_obj_array.is_deleted(base_obj_recno) );

	//---- call parent class function -----//

	int firmRecno = firm_recno;

	FirmCamp::being_attack_hit(attackerObj, damagePoint);

	if( firm_array.is_deleted(firmRecno) )
		return;

	//---- if this is an independent lair, FirmCamp::being_attack_hit() will not invoke defense ---//

	if( !nation_recno && attackerObj )
	{
		//---- reckon the net attacking power in the battling area ----//

		int hasWar;			// ref var for returning the result

		int netAttackerPower = world.net_attacker_power_in_area(center_x, center_y,
								  attackerObj->nation_recno, nation_recno, 0, hasWar, EFFECTIVE_DEFEND_DISTANCE);

		invoke_defense(attackerObj, netAttackerPower);
	}
}
//------ end of function FirmLair::being_attack_hit -------//


//------ begin of function FirmLair::collect_town_tribute -------//
//
// Collect tributes from independent villages linked to this
// lair with resistance that is lower than 30.
//
// [int] collectNow - whether collect the tribute now or not.
//							 (default: 1)
//
// return: the amount collected or can be collected.
//
int FirmLair::collect_town_tribute(int collectNow)
{
	if( !nation_recno )
		return 0;

	int   i, totalTribute=0;
	Town* townPtr;

	for( i=0 ; i<linked_town_count ; i++ )
	{
		if(town_array.is_deleted(linked_town_array[i]))
			continue;

		townPtr = town_array[linked_town_array[i]];

		//--- if it is a linked independent town ---//

		if( linked_town_enable_array[i] == LINK_EE &&
			 townPtr->nation_recno == 0 &&
			 townPtr->resistance(nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			totalTribute += townPtr->population * MONSTER_TOWN_TRIBUTE_PER_UNIT;
		}
	}

	if( collectNow )
		nation_array[nation_recno]->add_income( INCOME_TRIBUTE, (float) totalTribute );

	return totalTribute;
}
//------ end of function FirmLair::collect_town_tribute -------//


//-------- Begin of function FirmLair::can_accept_assign ------//
//
// Return whether this firm accepts a new unit.
//
// <int> unitRecno - recno of the unit
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
bool FirmLair::can_accept_assign(int unitRecno, int actionNationRecno)
{
	if( under_construction )
		return false;

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->nation_recno != nation_recno )
		return false;

	// ##### begin Gilbert 24/3 #######//
	// if( unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER )
	if( unit_res[unitPtr->unit_id]->class_info.monster_side )
	// ##### end Gilbert 24/3 #######//
		return true;
	else
		return false;
}
//---------- End of function FirmLair::can_accept_assign --------//

// --------- begin of function FirmLari::buy_tech ---------//
//
void FirmLair::buy_tech(int techId, char remoteAction)
{
	if( !nation_recno )
		return;

	TechInfo *techInfo = tech_res[techId];

	// only accept tech of same race

	if( techInfo->race_id != race_id || !techInfo->can_research(nation_recno) )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_LAIR_BUY_TECH, 2*sizeof(short));
		// packet structure : <firm recno> <tech id>
		shortPtr[0] = firm_recno;
		shortPtr[1] = techId;
		return;
	}

	int newTechLevel = techInfo->get_nation_tech_level(nation_recno) + 1;
	techInfo->set_nation_tech_level(nation_recno, newTechLevel);

	// ###### begin Gilbert 8/2 ########//
//	if( is_own() )
//	{
		// news_array.tech_researched( techId, newTechLevel );
//	}
	// ###### end Gilbert 8/2 ########//

	// --------- reduce live point --------//

	nation_array[nation_recno]->change_live_points( (float) -techInfo->live_points_cost() );
}
// --------- end of function FirmLari::buy_tech ---------//

//----- Begin of function FirmLair::can_grant_to_non_own_lair -----//
//
// <int> grantNationRecno - the recno of the nation that
//									 is going to grant this lair.
//
// <int> checkCash		  - whether check if the nation has enough cash.
//									 if it is 0, then cash checking will be ignored.
//
int FirmLair::can_grant_to_non_own_lair(int grantNationRecno, int checkCash)
{
	if( nation_recno )		// can only grant to independent lairs
		return 0;

	if( !nation_array[grantNationRecno]->is_monster() )		// only monster nations can grant to independent lairs
		return 0;

	if( checkCash )
	{
		if( nation_array[grantNationRecno]->cash < grant_to_non_own_lair_cost() )
			return 0;
	}

	//---- check if there is a lair of the granting nation linked to it -----//

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		FirmLair* firmLair = firm_array[linked_firm_array[i]]->cast_to_FirmLair();

		if( firmLair &&
			 firmLair->nation_recno == grantNationRecno &&
			 firmLair->overseer_recno )
		{
			return 1;
		}
	}

	return 0;
}
//------ End of function FirmLair::can_grant_to_non_own_lair -----//


//----- Begin of function FirmLair::grant_to_non_own_lair_cost -----//
//
int FirmLair::grant_to_non_own_lair_cost()
{
	int grantCost = 1000 + obj_power() * 5;

	return max(2000, grantCost);
}
//------ End of function FirmLair::grant_to_non_own_lair_cost -----//


//----- Begin of function FirmLair::grant_to_non_own_lair -----//
//
// <int> grantNationRecno - the recno of the nation that
//									 is going to grant this lair.
//
int FirmLair::grant_to_non_own_lair(int grantNationRecno, int remoteAction)
{
	if( !can_grant_to_non_own_lair(grantNationRecno, 1) )		// 1-check cash 
		return 0;

	Nation* grantNation = nation_array[grantNationRecno];

	if( !remoteAction && remote.is_enable() )
	{
		// packet struct : <firm recno> <grant nation recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_LAIR_GRANT, 2*sizeof(short));
		shortPtr[0] = firm_recno;
		shortPtr[1] = grantNationRecno;
		return 1;
	}

	//----------- decrease cash ------------//

	grantNation->add_expense(EXPENSE_GRANT_OTHER_TOWN, (float)grant_to_non_own_lair_cost() );

	//--------- change nation now ----------//

	if( overseer_recno )
		unit_array[overseer_recno]->betray(grantNationRecno);		// change the nation of the overseer which will then change the nation of the firm
	else
		change_nation(grantNationRecno);

	return 1;
}
//------ End of function FirmLair::grant_to_non_own_lair -----//


//------- Begin of function FirmLair::change_nation ---------//
//
void FirmLair::change_nation(int newNationRecno)
{
	FirmCamp::change_nation(newNationRecno);
}
//-------- End of function FirmLair::change_nation ---------//



// #### patch begin Gilbert 17/2 ######//
//------- Begin of function FirmLair::natural_breed ---------//
//
void FirmLair::natural_breed()
{
	if( is_soldier_full() )
		return;

	if( !is_monster() )
		return;

	// count the number of soldier with the same race
	int unitCount = 0;
	if( !unit_array.is_deleted(overseer_recno)
		&& unit_array[overseer_recno]->race_id == race_id )
		unitCount++;
	for( int i = 0; i < soldier_count; ++i )
	{
		Soldier *soldierPtr = &soldier_array[i];
		if( soldierPtr->race_id == race_id && !soldierPtr->is_under_training() )
			unitCount++;
	}

	if( unitCount >= 2 && unitCount > m.random(100) )
		recruit_soldier( COMMAND_AUTO, 1 );		// natural breed, so no cost
}
//------- End of function FirmLair::natural_breed ---------//
// #### patch end Gilbert 17/2 ######//
