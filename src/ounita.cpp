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

//Filename   : OUNITA.CPP
//Description: Unit Array

#include <oconfig.h>
#include <ogodres.h>
#include <orebel.h>
#include <omouse.h>
#include <osys.h>
#include <oun_grp.h>
#include <ounit.h>
#include <ounitall.h>

//--------- Begin of function UnitArray::UnitArray ---------//
//
// <int> initArraySize - the initial size of this array.
//
UnitArray::UnitArray(int initArraySize) : SpriteArray(initArraySize)
{
}
//----------- End of function UnitArray::UnitArray -----------//


//--------- Begin of function UnitArray::init ---------//
//
void UnitArray::init()
{
	selected_recno = 0;
   selected_count = 0;

	//------------ init sprite_array ------------//

	SpriteArray::init();
}
//----------- End of function UnitArray::init -----------//


//--------- Begin of function UnitArray::deinit ---------//
//
void UnitArray::deinit()
{
	if( size()==0 )
		return;

	//----- delete units ------//

	for( int i=1 ; i<=size() ; i++ )
	{
		if( is_truly_deleted(i) )
			continue;

		del(i);
	}

	//-------- zap the array -----------//

	zap();
}
//----------- End of function UnitArray::deinit -----------//


//--------- Begin of function UnitArray::add_unit ---------//
//
// <int> unitId               - the id. of the unit
// <int> nationRecno          - the recno of the nation that the unit belongs to
// [int] rankId					- rank id. of the unit (none for non-human unit)
// [int] unitLoyalty			   - loyalty of the unit  (none for non-human unit)
// [int] startXLoc, startYLoc - the starting location of the unit
//										  (if startXLoc < 0, this is a unit for hire, and is not a unit of the game yet. init_sprite() won't be called for this unit)
//										  (default: -1, -1)
// [int] remoteAction			- whether this is a remote action or not.
//
// return : <int> - the recno of the unit added.
//
int UnitArray::add_unit(int unitId, int nationRecno, int rankId, int unitLoyalty, int startXLoc, int startYLoc)
{
	//-------- create and initialize Unit -------//

	Unit* unitPtr = create_unit(unitId);

	base_obj_array.add( unitPtr );		// add it to base_obj_array

	//----------- add to SpriteArray ------------//

	unitPtr->init(unitId, nationRecno, rankId, unitLoyalty, startXLoc, startYLoc);

	//-------------------------------------------//

	return unitPtr->sprite_recno;
}
//----------- End of function UnitArray::add_unit -----------//


//-------- Begin of function UnitArray::create_unit --------//
//
Unit* UnitArray::create_unit(int unitId)
{
	Unit* 	 unitPtr;
	UnitInfo* unitInfo = unit_res[unitId];

	switch(unitId)
	{
		case UNIT_CARAVAN:
			unitPtr = new UnitCaravan;
			break;

//		case UNIT_VESSEL:
//		case UNIT_TRANSPORT:
//		case UNIT_CARAVEL:
//		case UNIT_GALLEON:
//			unitPtr = new UnitMarine;
//			break;

		case UNIT_WAGON:
			unitPtr = new UnitWagon;
			break;

		case UNIT_EXPLOSIVE_CART:
			unitPtr = new UnitExpCart;
			break;

		default:
			if( unitInfo->is_monster() )
				unitPtr = new UnitMonster;

			else if( god_res.is_god_unit(unitId) )
				unitPtr = new UnitGod;

			else
				unitPtr = new Unit;
	}

	SpriteArray::add(unitPtr);			// it set Sprite::sprite_recno

 	return unitPtr;
}
//----------- End of function UnitArray::create_unit ---------//


//-------- Begin of function UnitArray::del --------//
//
void UnitArray::del(int unitRecno)
{
	//--- del its link from base_obj_array ----//

	base_obj_array.del(unit_array[unitRecno]->base_obj_recno);

	SpriteArray::del(unitRecno);
}
//----------- End of function UnitArray::del ---------//


//-------- Begin of function UnitArray::unit_class_size --------//
//
// Return the size of the class.
//
int UnitArray::unit_class_size(int unitId)
{
	UnitInfo* unitInfo = unit_res[unitId];

	switch(unitId)
	{
		case UNIT_CARAVAN:
			return sizeof(UnitCaravan);

//		case UNIT_VESSEL:
//		case UNIT_TRANSPORT:
//		case UNIT_CARAVEL:
//		case UNIT_GALLEON:
//			return sizeof(UnitMarine);

		case UNIT_WAGON:
			return sizeof(UnitWagon);
			break;

		case UNIT_EXPLOSIVE_CART:
			return sizeof(UnitExpCart);

		default:
			if( unitInfo->is_monster() )
				return sizeof(UnitMonster);

			else if( god_res.is_god_unit(unitId) )
				return sizeof(UnitGod);

			else
				return sizeof(Unit);
	}
}
//----------- End of function UnitArray::unit_class_size ---------//


//-------- Begin of function UnitArray::disappear_in_town --------//
//
// The unit disappear from the map because it has moved into a town.
//
void UnitArray::disappear_in_town(int unitRecno, int townRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->unit_mode == UNIT_MODE_REBEL )
		rebel_array.settle_town(unitRecno, townRecno);

	del(unitRecno);		// delete it from unit_array
}
//----------- End of function UnitArray::disappear_in_town ---------//


//-------- Begin of function UnitArray::disappear_in_firm --------//
//
// The unit disappear from the map because it has moved into a town.
//
void UnitArray::disappear_in_firm(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	err_when( unitPtr->unit_mode == UNIT_MODE_REBEL );		// this shouldn't happen

	del(unitRecno);		// delete it from unit_array
}
//----------- End of function UnitArray::disappear_in_firm ---------//


//-------- Begin of function UnitArray::die --------//
//
void UnitArray::die(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->unit_mode == UNIT_MODE_REBEL )
		rebel_array.drop_rebel_identity(unitRecno);

	unitPtr->die();

	del(unitRecno);		// delete it from unit_array
}
//----------- End of function UnitArray::die ---------//


//--------- Begin of function UnitArray::process ---------//
//
// This function is called every frame.
//
void UnitArray::process()
{
	#define SYS_YIELD_INTERVAL	50
	#define YEAR_FRAME_COUNT	365*FRAMES_PER_DAY // choose a value that is multiply of FRAMES_PER_DAY

	Unit* unitPtr;
	int i;
	int arraySize = size();

	int sysFrameCount = int(sys.frame_count%FRAMES_PER_DAY);
	int yearFrameCount = int(sys.frame_count%YEAR_FRAME_COUNT);

	int compareI = arraySize%FRAMES_PER_DAY;

	if(compareI < sysFrameCount)
		compareI += FRAMES_PER_DAY;

	int sysYieldCount = arraySize - arraySize%SYS_YIELD_INTERVAL;

	for(i=arraySize; i; --i, compareI--) // for(i=arraySize; i>0; --i, compareI++) or //for(i=1; i<=arraySize; i++, compareI++)
	{
		//-------- system yield ---------//

		if(i==sysYieldCount)
		{
			sysYieldCount -= SYS_YIELD_INTERVAL;
			sys.yield();
		}

		//-------------------------------//
		if(compareI == sysFrameCount)
		{
			compareI += FRAMES_PER_DAY;

			if( is_deleted(i) )
				continue;

			unitPtr = (Unit*) get_ptr(i);
			if(unitPtr->is_freezed())
				continue;

			if (unitPtr->home_camp_firm_recno && unitPtr->rank_id == RANK_GENERAL && unitPtr->auto_retreat_hit_point > 0 &&
				 unitPtr->hit_points < (unitPtr->max_hit_points() * unitPtr->auto_retreat_hit_point / 100))
			{
				unitPtr->return_camp();
			}
			//-------------- reset ignore_power_nation ------------//

			if(compareI==yearFrameCount && unitPtr->unit_id!=UNIT_CARAVAN)
				unitPtr->ignore_power_nation = 0;

			unitPtr->next_day();       // although this function is called every frame, next_day() & process_ai() is only called once per day

			if( is_deleted(i) )
				continue; 

			#ifdef DEBUG
			if(config.disable_ai_flag==0 && unitPtr->is_ai)
			#else
			if( unitPtr->is_ai )
			#endif
			{
				#ifdef DEBUG
				unsigned long profileAiStartTime = m.get_time();
				#endif

				unitPtr->process_ai();

				#ifdef DEBUG
				unit_ai_profile_time += m.get_time() - profileAiStartTime;
				#endif
			}

			//----- if it's an independent unit -------//

			else if( !unitPtr->nation_recno && unitPtr->race_id && !unitPtr->spy_recno)
				unitPtr->think_independent_unit();
		}
	}

	//------- process Sprite ---------//

	SpriteArray::process();

	//--------------------------------//

	#ifdef DEBUG

	int selectedCount=0;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_truly_deleted(i) )
			continue;

		if( unit_array[i]->selected_flag )
			selectedCount++;
	}

	err_when( selectedCount != unit_array.selected_count );

	err_when( unit_array.selected_recno>0 && unit_array.selected_count==0 );

	#endif
}
//----------- End of function UnitArray::process -----------//


//--------- Begin of function UnitArray::stop_all_war ---------//
//
// Stop all the units that is in war with the specified nation
// and stop all the units of the specified nation that is in war
// with other nations
//
// <short>	stopNationRecno - the nation_renco of the specified nation
//
void UnitArray::stop_all_war(short stopNationRecno)
{
	for( int i=size() ; i>0 ; i-- )
	{
		if( is_deleted(i) )
			continue;

		Unit* unitPtr = operator[](i);

		if( !unitPtr->is_visible() )
			continue;

		//---- if this unit is attacking somebody ------/

		if( unitPtr->cur_order.mode == UNIT_ATTACK )
		{
			if( unitPtr->nation_recno == stopNationRecno )
			{
				unitPtr->stop_order();
				continue;
			}

			//--- if the target is already killed ---//

			if( !base_obj_array.is_deleted(unitPtr->cur_order.para) &&
				 base_obj_array[unitPtr->cur_order.para]->nation_recno == stopNationRecno )
			{
				unitPtr->stop_order();
				continue;
			}
		}

		//--- if this unit has a pushed order, also check it ----//

		if( !unitPtr->has_pushed_order() )
			continue;

		if( unitPtr->pushed_order.mode == UNIT_ATTACK )
		{
			if( unitPtr->nation_recno == stopNationRecno )
			{
				unitPtr->stop_order();
				continue;
			}

			//--- if the target is already killed ---//

			if( !base_obj_array.is_deleted(unitPtr->pushed_order.para) &&
				 base_obj_array[unitPtr->pushed_order.para]->nation_recno == stopNationRecno )
			{
				unitPtr->stop_order();
				continue;
			}
		}
	}
}
//----------- End of function UnitArray::stop_all_war -----------//


//--------- Begin of function UnitArray::stop_war_between ---------//
//
// Stop the war between the two nations specified
//
// <short>	nationReno1	-	the nation_renco of one of the nation
// <short>	nationReno2	-	the nation_renco of the other nation
//									if it is 0, then stop wars between the
//									specific nation and any other nation.
//
void UnitArray::stop_war_between(short nationRecno1, short nationRecno2)
{
	for( int i=size() ; i>0 ; i-- )
	{
		if( is_deleted(i) )
			continue;

		Unit* unitPtr = operator[](i);

		if( !unitPtr->is_visible() )
			continue;

		if( unitPtr->nation_recno != nationRecno1 &&
			 unitPtr->nation_recno != nationRecno2 )
		{
			continue;
		}

		//---- if this unit is attacking somebody ------/

		if( unitPtr->cur_order.mode == UNIT_ATTACK )
		{
			if( !base_obj_array.is_deleted(unitPtr->cur_order.para) )
			{
				int targetNationRecno = base_obj_array[unitPtr->cur_order.para]->nation_recno;

				if( targetNationRecno == nationRecno1 ||
					 targetNationRecno == nationRecno2 )
				{
					unitPtr->stop_order();
					continue;
				}
			}
		}

		//--- if this unit has a pushed order, also check it ----//

		if( !unitPtr->has_pushed_order() )
			continue;

		if( unitPtr->pushed_order.mode == UNIT_ATTACK )
		{
			if( !base_obj_array.is_deleted(unitPtr->pushed_order.para) )
			{
				int targetNationRecno = base_obj_array[unitPtr->pushed_order.para]->nation_recno;

				if( targetNationRecno == nationRecno1 ||
					 targetNationRecno == nationRecno2 )
				{
					unitPtr->stop_order();
					continue;
				}
			}
		}
	}
}
//----------- End of function UnitArray::stop_war_between -----------//


//--------- Begin of function UnitArray::stop_attack_obj ---------//
//
// stop all units that is attacking the specified unit
//
// <int> baseObjRecno - baseObjRecno
// [int] nationRecno  - if this is given, only units of this unit
//							   will be ordered to stop attack. Otherwise,
//								stop all units.
//
void UnitArray::stop_attack_obj(int baseObjRecno, int nationRecno)
{
	if( nation_hand_over_flag )
		return;

	for(int i=size(); i>0; --i)
	{
		if(is_deleted(i))
			continue;

		Unit* unitPtr = operator[](i);

		if( !unitPtr->is_visible() )
			continue;

		if( nationRecno && nationRecno != unitPtr->nation_recno )
			continue;

		if( unitPtr->cur_order.mode == UNIT_ATTACK &&
			 unitPtr->cur_order.para == baseObjRecno )
		{
			unitPtr->stop_order();
		}

		else if( unitPtr->pushed_order.mode == UNIT_ATTACK &&
					unitPtr->pushed_order.para == baseObjRecno )
		{
			unitPtr->stop_order();
		}
	}
}
//----------- End of function UnitArray::stop_attack_obj -----------//


#ifdef DEBUG

//------- Begin of function UnitArray::operator[] -----//

Unit* UnitArray::operator[](int recNo)
{
	Unit* unitPtr = (Unit*) get_ptr(recNo);

	if( !unitPtr )
		err.run( "UnitArray[] is deleted" );

	return unitPtr;
}

//--------- End of function UnitArray::operator[] ----//

#endif

//------- Begin of function UnitArray::is_deleted -----//

int UnitArray::is_deleted(int recNo)
{
	Unit *unitPtr = (Unit*) get_ptr(recNo);

	if( !unitPtr )
		return 1;

	if( unitPtr->hit_points<=0 || unitPtr->cur_action==SPRITE_DIE )
		return 1;

	return 0;
}
//--------- End of function UnitArray::is_deleted ----//


//------- Begin of function UnitArray::is_truly_deleted -----//

int UnitArray::is_truly_deleted(int recNo)
{
	return get_ptr(recNo) == NULL;
}
//--------- End of function UnitArray::is_truly_deleted ----//

