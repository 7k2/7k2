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

// filename    : OF_ANIM.CPP
// Description : FirmAnimal


#include <OF_ANIM.H>
#include <ONATION.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <OUNIT.H>


// ------ define constant --------//

#define UPDATE_INTERVAL 14

// ------ define static variable --------//

short FirmAnimal::selected_unit_id;
short FirmAnimal::animal_unit_id[ANIMAL_TYPE_COUNT] = { UNIT_JAGUAR, UNIT_GORILLA, UNIT_RHINO, };




// -------- begin of function FirmAnimal::FirmAnimal -------//
//
FirmAnimal::FirmAnimal()
{
	memset( (char *)this + sizeof(Firm), 0, sizeof(FirmAnimal) - sizeof(Firm) );
}
// -------- end of function FirmAnimal::FirmAnimal -------//


// -------- begin of function FirmAnimal::~FirmAnimal -------//
//
FirmAnimal::~FirmAnimal()
{
}
// -------- end of function FirmAnimal::~FirmAnimal -------//


// -------- begin of function FirmAnimal::init_derived -------//
//
void FirmAnimal::init_derived()
{
	Firm::init_derived();

	// load bitmap of all three animals unit

	for( int i = 0; i < ANIMAL_TYPE_COUNT; ++i )
	{
		int spriteId = unit_res[animal_unit_id[i]]->sprite_id;
		sprite_res[spriteId]->load_bitmap_res();
	}

	animal_unit_count = 0;
}
// -------- end of function FirmAnimal::init_derived -------//


// -------- begin of function FirmAnimal::deinit_derived -------//
//
void FirmAnimal::deinit_derived()
{
	// free bitmap of all three animals unit

	for( int i = 0; i < ANIMAL_TYPE_COUNT; ++i )
	{
		int spriteId = unit_res[animal_unit_id[i]]->sprite_id;
		sprite_res[spriteId]->free_bitmap_res();
	}

	Firm::deinit_derived();
}
// -------- end of function FirmAnimal::deinit_derived -------//


// -------- begin of function FirmAnimal::free_all_people -------//
//
void FirmAnimal::free_all_people()
{
	Firm::free_all_people();

	for(int i = animal_unit_count; i > 0; --i )
	{
		del_animal_unit(i);
	}
}
// -------- end of function FirmAnimal::free_all_people -------//


// -------- begin of function FirmAnimal::next_day -------//
//
void FirmAnimal::next_day()
{
	//-------- update the hire list ------------//

	if( info.game_date % UPDATE_INTERVAL == firm_recno % UPDATE_INTERVAL )
		update_add_hire_list();

	if( info.game_date % UPDATE_INTERVAL == firm_recno % UPDATE_INTERVAL )
		update_del_hire_list();

	//----------- run next_day() of the base class -------------//

	Firm::next_day();
}
// -------- end of function FirmAnimal::next_day -------//


// -------- begin of function FirmAnimal::can_hire -------//
//
int FirmAnimal::can_hire(int recNo)
{
	if( recNo < 1 || recNo > animal_unit_count )
		return 0;

	AnimalUnit *animalUnit = animal_unit_array + recNo - 1;

	if( nation_array.is_deleted(nation_recno) )
		return 0;

	Nation *nationPtr = nation_array[nation_recno];

	return unit_res[animalUnit->unit_id]->get_nation_tech_level(nation_recno) > 0
		&& nationPtr->cash >= animalUnit->hire_cost 
		&& nationPtr->live_points >= animalUnit->live_point_cost;
}
// -------- end of function FirmAnimal::can_hire -------//


// -------- begin of function FirmAnimal::kill_unit -------//
//
int FirmAnimal::kill_unit(int recNo)
{
	if( recNo < 1 || recNo > animal_unit_count )
		return 0;

	del_animal_unit(recNo);
	return 1;
}
// -------- end of function FirmAnimal::kill_unit -------//


// -------- begin of function FirmAnimal::hire -------//
//
int FirmAnimal::hire(short recNo)
{
	err_when( recNo < 1 );

	if( recNo > animal_unit_count )		// this may happen in a multiplayer game
		return 0;

	//--------- first check if you have enough money to hire ------//

	Nation* 	nationPtr = nation_array[nation_recno];
	AnimalUnit* animalUnit = animal_unit_array+recNo-1;
	int unitRecno = 0;

	if( unit_res[animalUnit->unit_id]->get_nation_tech_level(nation_recno) <= 0 )
		return 0;

	if( nationPtr->cash < animalUnit->hire_cost || nationPtr->live_points < animalUnit->live_point_cost )
		return 0;

	unitRecno = create_unit( animalUnit->unit_id );
	if( !unitRecno )
		return 0;		// no space for creating the unit

	nationPtr->add_expense(EXPENSE_HIRE_UNIT, animalUnit->hire_cost);
	nationPtr->change_live_points(- (float) animalUnit->live_point_cost);

	//-------- set skills of the unit --------//

	err_when( !unitRecno );

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->skill 	  = animalUnit->skill;
	unitPtr->hit_points = (float) animalUnit->max_hit_points();

	err_when( animalUnit->combat_level()<=0 || animalUnit->combat_level()>1000 );

	//----------------------------------------//
	//
	// Set loyalty of the hired unit
	//
	// = 30 + the nation's reputation / 2 + 30 if racially homogenous
	//
	//----------------------------------------//

	int unitLoyalty = 30 + (int)nationPtr->reputation/2;
//	if( race_res.is_same_race(unitPtr->race_id, nationPtr->race_id) )
//		unitLoyalty += 20;

	unitLoyalty = max( 40, unitLoyalty );
	unitLoyalty = min( 100, unitLoyalty );

	unitPtr->loyalty = unitLoyalty;

	// ------- set unique id -------//

	// ###### begin Gilbert 23/2 ######//
	unitPtr->unique_id = m.rand_long();
	// ###### end Gilbert 23/2 ######//

	// ------ set item -------//

	unitPtr->item = animalUnit->item;

	unitPtr->set_combat_level(-1);

	//---- remove the record from the hire list ----//

	del_animal_unit(recNo);

	return unitRecno;
}
// -------- end of function FirmAnimal::hire -------//


// -------- begin of function FirmAnimal::add_animal_unit -------//
//
void FirmAnimal::add_animal_unit(int unitId, int animalLevel)
{
	err_when( animal_unit_count >= MAX_ANIMAL_UNIT );

	// ---- collect occupied slot display_slot_id ------//

	char slotOccupiedFlag[MAX_DISPLAY_SLOT];
	memset( slotOccupiedFlag, 0, sizeof(slotOccupiedFlag) );

	for( int i = 0; i < animal_unit_count; ++i )
	{
		err_when( animal_unit_array[i].display_slot_id >= MAX_DISPLAY_SLOT );
		if( animal_unit_array[i].display_slot_id >= 0 )
			slotOccupiedFlag[animal_unit_array[i].display_slot_id] = 1;
	}

	// ---- set new animal ---------//

	AnimalUnit *animalUnit = animal_unit_array+animal_unit_count;
	UnitInfo *unitInfo = unit_res[unitId];

	memset( animalUnit, 0, sizeof(animalUnit) );

	animal_unit_count++;

	animalUnit->unit_id	  = unitId;
	switch( animalLevel )
	{
	case 0:
	case 1:	// 20-30
		animalUnit->skill.init(unitId, BASIC_COMBAT_TRAIN + m.random(11), 0 );
		break;
	case 2:	// 50-100
		animalUnit->skill.init(unitId, MAX_COMBAT_TRAIN/2 + m.random(MAX_COMBAT_TRAIN/2), 0 );
		break;
	case 3:	// 100-200
		animalUnit->skill.init(unitId, MAX_COMBAT_TRAIN + m.random(MAX_COMBAT_BATTLE-MAX_COMBAT_TRAIN), 0 );
		break;
	default:
		err_here();
	}

	animalUnit->item.clear();		// no item

	animalUnit->set_hire_cost();
	animalUnit->stay_count = MAX_ANIMAL_UNIT + m.random(MAX_ANIMAL_UNIT);
	// 10 + m.random(5);

	// ------ set display_slot_id ------//
	
	for( i = 0; i < MAX_DISPLAY_SLOT && slotOccupiedFlag[i]; ++i );

	if( i < MAX_DISPLAY_SLOT )
		animalUnit->display_slot_id = i;
	else
		animalUnit->display_slot_id = -1;	// not enough slot to display
}
// -------- end of function FirmAnimal::add_animal_unit -------//


// -------- begin of function FirmAnimal::del_animal_unit -------//
//
void FirmAnimal::del_animal_unit(int recNo)
{
	err_when( recNo<1 || recNo>MAX_ANIMAL_UNIT );

	err_when( animal_unit_count < 1 );
	err_when( animal_unit_count > MAX_ANIMAL_UNIT );

	AnimalUnit *animalUnit = animal_unit_array + recNo - 1;

	m.del_array_rec(animal_unit_array, animal_unit_count, sizeof(AnimalUnit), recNo);

	animal_unit_count--;

	// ------- update selected_recno -------//

	if( firm_recno == firm_array.selected_recno )
	{
		if( recNo == selected_unit_id )
			selected_unit_id = 0;	// reset
		else if( recNo < selected_unit_id )
			selected_unit_id--;

		if (selected_unit_id > animal_unit_count)
			selected_unit_id = animal_unit_count;
	}
}
// -------- end of function FirmAnimal::del_animal_unit -------//


//------- Begin of function FirmAnimal::update_add_hire_list ------//

void FirmAnimal::update_add_hire_list()
{
	err_when( animal_unit_count > MAX_ANIMAL_UNIT );

	if( animal_unit_count==MAX_ANIMAL_UNIT )
		return;

/*
	static short animalTechIdArray[] =
	{
		TECH_JAGUAR,
		TECH_GORILLA,
		TECH_RHINO
	};

	for( int i=0 ; i<ANIMAL_TYPE_COUNT ; i++ )
	{
		if( tech_res[ animalTechIdArray[i] ]->get_nation_tech_level(nation_recno) )
		{
			if( m.random( min(animal_unit_count,3) )==0 )
			{
				add_animal_unit( animal_unit_id[i] );
				// ###### begin Gilbert 29/12 ####//
				break;
				// ###### end Gilbert 29/12 ####//
			}
		}
	}
*/

	// ---- select unit id --------//

	// old scheme : select one of the any possible researched tech
//	short unitIdArray[ANIMAL_TYPE_COUNT];
//	short unitIdCount = 0;
//	for( int i = 0; i < ANIMAL_TYPE_COUNT; ++i )
//	{
//		if( !nation_recno || unit_res[animal_unit_id[i]]->get_nation_tech_level(nation_recno) )
//		{
//			unitIdArray[unitIdCount] = animal_unit_id[i];
//			unitIdCount++;
//		}
//	}
//	if( unitIdCount <= 0 )
//		return;
//	short addUnitId = unitIdArray[m.random(unitIdCount)];

	// new scheme : select any possbile scheme , skip if not research

	short addUnitId = animal_unit_id[m.random(ANIMAL_TYPE_COUNT)];

	// ---- select animal level -------//

	short maxTechLevel = !nation_recno ? 2 :
		unit_res[addUnitId]->get_nation_tech_level(nation_recno);

	if( maxTechLevel > 0 )
	{
		short addAnimalLevel = 1;

		if( maxTechLevel > 1 )
		{
			err_when( maxTechLevel > 3 );
			int r = m.random(31);

			if( r < 1 )			// 1/31 to get a level 3
				addAnimalLevel = 3;
			else if( r < 6 )	// 5/31 to get a level 2
				addAnimalLevel = 2;
			else					// 25/31 to get a level 1
				addAnimalLevel = 1;

			addAnimalLevel = min( addAnimalLevel, maxTechLevel );
		}

		add_animal_unit( addUnitId, addAnimalLevel );
	}
}

//------- End of function FirmAnimal::update_add_hire_list ------//


// -------- begin of function FirmAnimal::update_del_hire_list -------//
//
void FirmAnimal::update_del_hire_list()
{
	//------- existing units leave -------//

	for( int i=animal_unit_count ; i>0 && animal_unit_count>0 ; i-- )
	{
		if( --animal_unit_array[i-1].stay_count==0 )
		{
			del_animal_unit(i);
		}
	}
}
// -------- end of function FirmAnimal::update_del_hire_list -------//


// -------- begin of function FirmAnimal::set_hire_cost -------//
//
void AnimalUnit::set_hire_cost()
{
	UnitInfo *unitInfo = unit_res[unit_id];
	// #### patch begin Gilbert 13/7 ######//
	// hire_cost = unitInfo->build_cost * combat_level() / 30;
	hire_cost = unitInfo->build_cost * combat_level() / 15;
	// #### patch end Gilbert 13/7 ######//
	live_point_cost = unitInfo->build_live_points_cost;
}
// -------- end of function FirmAnimal::set_hire_cost -------//
