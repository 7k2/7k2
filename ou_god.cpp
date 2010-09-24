//Filename   : OU_GOD.CPP
//Description: God Unit

#include <windows.h>
#include <OU_GOD.H>
#include <OSYS.H>
#include <OGAME.H>
#include <OGODRES.H>
#include <OF_BASE.H>
#include <OF_CAMP.H>
#include <OF_INN.H>
#include <ORACERES.H>
#include <OWEATHER.H>
#include <OTORNADO.H>
#include <OSPY.H>
#include <math.h>

// -------- Begin of function UnitGod::UnitGod --------//
//
UnitGod::UnitGod()
{
	memset( (char *)this + sizeof(Unit), 0, sizeof(UnitGod)-sizeof(Unit) );
}
// -------- End of function UnitGod::UnitGod --------//


//--------- Begin of function UnitGod::can_attack_target --------//
//
int UnitGod::can_attack_target(int targetObjRecno)
{
	BaseObj* targetObj = base_obj_array[targetObjRecno];
	GodInfo* godInfo = god_res[god_id];

	//--- if the should_attack flag in NationRelation is false, return now ---//

	// ####### begin Gilbert 6/7 #########//
//	if( !nation_array[nation_recno]->get_relation_should_attack(targetObj->nation_recno) )
//		return 0;
	if( !nation_array.should_attack(nation_recno, targetObj->nation_recno) )
		return 0;

	//----- if the target is this Greater Being's worst enemy -----//

	if( godInfo->target_monster_id
		&& targetObj->monster_id() == godInfo->target_monster_id )
	// ####### end Gilbert 6/7 #######//
		return 1;

	//------- attack a normal target -------//

	if( godInfo->can_attack )
		return Unit::can_attack_target(targetObjRecno);
	else
		return 0;
}
//--------- End of function UnitGod::can_attack_target --------//


//------- Begin of function UnitGod::pre_process -------//
//
void UnitGod::pre_process()
{
	Unit::pre_process();

	//--- if the seat of power supporting this unit is destroyed, this unit dies ---//

	if( firm_array.is_deleted(base_firm_recno) )
	{
		hit_points=(float)0;
		set_die();
		return;
	}

	//---- this unit consume pray points as it exists ----//

	FirmBase* firmBase = (FirmBase*) firm_array[base_firm_recno];

	err_when( firmBase->firm_id != FIRM_BASE );

	// #### begin Gilbert 16/2 ######//
	// firmBase->pray_points -= (float) god_res[god_id]->exist_pray_points / 100;
	firmBase->pray_points -= (float) god_res[god_id]->exist_pray_points / 150;
	// #### end Gilbert 16/2 ######//

	if( firmBase->pray_points < 0 )
		firmBase->pray_points = (float) 0;

	//--------- update hit points --------//

	hit_points = (short) firmBase->pray_points;

	if( hit_points == 0 )
		set_die();
}
//-------- End of function UnitGod::pre_process -------//

//--------- Begin of function UnitGod::process_attack --------//
int UnitGod::process_attack()
{
	GodInfo* godInfo = god_res[god_id];
	if( godInfo->can_attack )
	{
		if( !Unit::process_attack() )		// return 1 if the unit just finished its current attack
			return 0;
	}
	else
	{
		if( !Sprite::process_attack() )		// return 1 if the unit just finished its current attack
			return 0;
	}

	// ###### patch begin Gilbert 9/11 #######//
	// consume_power_pray_points();
	// ###### patch end Gilbert 9/11 #######//

	return 1;
}
//---------- End of function UnitGod::process_attack ----------//

//--------- Begin of function UnitGod::cast_power ---------//
//
// <int> castXLoc, castYLoc - the location which the power should
//										be casted on.
//
void UnitGod::cast_power(int xLoc, int yLoc)
{
	err_when( !god_res[god_id]->can_cast_power );

	// ###### patch begin Gilbert 19/1 ######//
	if( !consume_power_pray_points() )
	{
		// allow ai to use below 
		if( !is_ai )
			return;
	}
	// ###### patch end Gilbert 19/1 ######//

	if( god_id == GOD_INDIAN )
	{
		indian_summon_tornado();
		return;
	}

	if( god_id == GOD_CARTH )
	{
		carth_cast_quake();
		return;
	}

	if( god_id == GOD_JAPANESE )
	{
		japanese_kill_spy();
		// ###### patch begin Gilbert 18/1 #####//
		// extra effect see japanese_cast_inn
		// return;
		// ###### patch end Gilbert 18/1 #####//
	}

	//------ cast power on the selected area ------//

	GodInfo* godInfo = god_res[god_id];

	int xLoc1 = xLoc - godInfo->cast_power_range + 1;
	int yLoc1 = yLoc - godInfo->cast_power_range + 1;
	int xLoc2 = xLoc + godInfo->cast_power_range - 1;
	int yLoc2 = yLoc + godInfo->cast_power_range - 1;

	// ####### begin Gilbert 16/12 #######//
	xLoc1 = max( xLoc1, 0 );
	yLoc1 = max( yLoc1, 0 );
	xLoc2 = min( xLoc2, MAX_WORLD_X_LOC-1 );
	yLoc2 = min( yLoc2, MAX_WORLD_Y_LOC-1 );
	// ####### end Gilbert 16/12 #######//

//	int t;
	int centerY = (yLoc1+yLoc2) / 2;
	Location* locPtr;

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		// ##### begin Gilbert 16/12 #######//
//		t=abs(yLoc-centerY)/2;
//		locPtr = world.get_loc(xLoc1+t, yLoc);
//		for( xLoc=xLoc1+t ; xLoc<=xLoc2-t ; xLoc++, locPtr++ )
//		{
//			if( xLoc>=0 && xLoc<MAX_WORLD_X_LOC &&
//				 yLoc>=0 && yLoc<MAX_WORLD_Y_LOC )
//			{
//				cast_on_loc(xLoc, yLoc);
//			}
//		}

		locPtr = world.get_loc(xLoc1, yLoc);
		for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
		{
			cast_on_loc(xLoc, yLoc);
		}
		// ##### end Gilbert 16/12 #######//
	}
}
//---------- End of function UnitGod::cast_power ----------//


//--------- Begin of function UnitGod::viking_summon_rain ---------//
//
//void UnitGod::viking_summon_rain()
//{
//	magic_weather.cast_rain(10, 8);         // 10 days, rain scale 8
//	magic_weather.cast_lightning(7);               // 7 days
//}
//---------- End of function UnitGod::viking_summon_rain ----------//


//--------- Begin of function UnitGod::indian_summon_tornado ---------//
//
void UnitGod::indian_summon_tornado()
{
	short xLoc = next_x_loc();
	short yLoc = next_y_loc();
	char dir = final_dir % 8;

	// put a tornado one location ahead
	if( dir == 0 || dir == 1 || dir == 7 )
		if( yLoc > 0 )
			yLoc--;
	if( dir >= 1 && dir <= 3 )
		if( xLoc < MAX_WORLD_X_LOC-1 )
			xLoc++;
	if( dir >= 3 && dir <= 5 )
		if( yLoc < MAX_WORLD_Y_LOC-1)
			yLoc++;
	if( dir >= 5 && dir <= 7 )
		if( xLoc > 0 )
			xLoc--;

	tornado_array.add_tornado( xLoc, yLoc, 600 );
//	magic_weather.cast_wind(10, 1, dir * 45 );              // 10 days
	magic_weather.cast_wind(5, 1, dir * 45 );              // 5 days
}
//---------- End of function UnitGod::indian_summon_tornado ----------//


//--------- Begin of function UnitGod::carth_cast_quake ---------//
//
void UnitGod::carth_cast_quake()
{
	magic_weather.cast_quake( next_x_loc(), next_y_loc(), 32);
}
//--------- End of function UnitGod::carth_cast_quake ---------//


//--------- Begin of function UnitGod::japanese_kill_spy ---------//
//
void UnitGod::japanese_kill_spy()
{
	int range = god_res[god_id]->cast_power_range;
	for(int i = spy_array.size(); i > 0; --i )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy *spyPtr = spy_array[i];

		// catch enemy spy changed to our nation

		if( (spyPtr->cloaked_nation_recno != nation_recno
			&& spyPtr->cloaked_nation_recno != 0 )
			|| spyPtr->true_nation_recno == nation_recno )
			continue;

		// check range
		int baseObjRecno = spyPtr->get_host_base_obj_recno();
		if( base_obj_array.is_deleted(baseObjRecno) 
			|| area_distance( base_obj_array[baseObjRecno] ) >= range )
			continue;

		// chance of escape

//		if( m.random(200) < spyPtr->spy_skill )
//		if( m.random(1000) >= 900 - spyPtr->spy_skill )		// 10% - 20% change of escape
//			continue;

		// BUGHERE: call get_killed or set_exposed?
		// spyPtr->get_killed();
		spyPtr->set_exposed(COMMAND_AUTO);
	}
}
//--------- End of function UnitGod::japanese_kill_spy ---------//


void UnitGod::japanese_cast_inn(int firmRecno)
{
	if( firm_array.is_deleted(firmRecno) )
		return;

	Firm *firmPtr = firm_array[firmRecno];
	FirmInn *firmInn;
	if( !(firmInn = firmPtr->cast_to_FirmInn()) )
		return;

	// half the price of hire unit

	for( int i = 0; i < firmInn->inn_unit_count; ++i )
	{
		InnUnit *innUnit = &firmInn->inn_unit_array[i];
		innUnit->set_hire_cost(); 	// recalculate so as to avoid casting it twice to get 1/4 
		// #### patch begin Gilbert 16/2 ######//
		// innUnit->hire_cost = innUnit->hire_cost / 2;
		innUnit->hire_cost = innUnit->hire_cost * 7 / 10;	// 30% off
		// #### patch end Gilbert 16/2 ######//
		if( innUnit->stay_count < 10 )
			innUnit->stay_count = 10;
	}
}


//--------- Begin of function UnitGod::cast_on_loc ---------//
//
void UnitGod::cast_on_loc(int castXLoc, int castYLoc)
{
	Location* locPtr = world.get_loc( castXLoc, castYLoc );

	//--- if there is any unit on the location ---//

	if( locPtr->unit_recno(UNIT_LAND) )
	{
		cast_on_unit( locPtr->unit_recno(UNIT_LAND), 1 );
	}
	else if( locPtr->unit_recno(UNIT_SEA) )
	{
		Unit* unitPtr = unit_array[ locPtr->unit_recno(UNIT_SEA) ];

		//-- only heal human units belonging to our nation in ships --//
/*
		if( unitPtr->nation_recno == nation_recno &&
			 unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_SHIP )
		{
			UnitMarine* unitMarine = (UnitMarine*) unitPtr;

			for( int i=0 ; i<unitMarine->unit_count ; i++ )
			{
				int divider = 4;		// the size of a ship is 4 locations (2x2)

				cast_on_unit( unitMarine->unit_recno_array[i], divider );		// the effects are weaken on ship units, only 50% of the original effects
			}
		}
*/	}

	//--------- on firms ---------//

	else if( locPtr->is_firm() )
	{
		Firm* firmPtr = firm_array[ locPtr->firm_recno() ];
		GodInfo *godInfo = god_res[god_id];
		int divider = min(firmPtr->loc_width(), godInfo->cast_power_range) 
			* min(firmPtr->loc_height(),  godInfo->cast_power_range);

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

			if( firmCamp->overseer_recno )
			{
				cast_on_unit( firmCamp->overseer_recno, divider );
			}
		}

		/*
		if( firmPtr->overseer_recno )
		{
			cast_on_unit( firmPtr->overseer_recno, divider );
		}

		if( firmPtr->worker_array && firm_res[firmPtr->firm_id]->live_in_town==0 )
		{
			Worker* workerPtr = firmPtr->worker_array;

			for( int i=0 ; i<firmPtr->worker_count ; i++, workerPtr++ )
			{
				cast_on_worker(workerPtr, firmPtr->nation_recno, divider);
			}
		}
		*/

		// ####### begin Gilbert 27/1 #######//
		if( god_id == GOD_PERSIAN )
		{
			if( is_own() )
				firmPtr->reveal_info = 1;
		}
		// ####### end Gilbert 27/1 #######//

		// ##### patch begin Gilbert 18/1 ########//
		if( god_id == GOD_JAPANESE && firmPtr->cast_to_FirmInn() )
		{
			japanese_cast_inn( firmPtr->firm_recno );
		}
		// ##### patch end Gilbert 18/1 ########//
	}

	//--------- on towns ----------//

	else if( locPtr->is_town() )
	{
		Town* townPtr = town_array[ locPtr->town_recno() ];
/*
		if( god_id == GOD_JAPANESE && townPtr->nation_recno != nation_recno)
		{
			int divider = STD_TOWN_LOC_WIDTH * STD_TOWN_LOC_HEIGHT;

			float changePoints = (float)7 + m.random(8);		// decrease 7 to 15 loyalty points instantly

			if( townPtr->nation_recno )
				townPtr->change_loyalty(-changePoints/divider);
			else
				townPtr->change_resistance(nation_recno, -changePoints/divider);
		}
		else 
*/
		if( god_id == GOD_EGYPTIAN && townPtr->nation_recno == nation_recno)
		{
			int headCount;
			// ##### begin Gilbert 18/1 ######//
			// 10 people but 150 pray points see ogodres.cpp
			for( headCount = 10; headCount > 0 && townPtr->population < MAX_TOWN_GROWTH_POPULATION; --headCount )
			// ##### end Gilbert 18/1 ######//
			{
				townPtr->inc_pop( (int) townPtr->loyalty);
			}
		}
	}
}
//---------- End of function UnitGod::cast_on_loc ----------//


//--------- Begin of function UnitGod::cast_on_unit ---------//
//
// <int> unitRecno - recno of the unit to cast on
// <int> divider   - divide the amount of effects by this number
//
void UnitGod::cast_on_unit(int unitRecno, int divider)
{
	switch(god_id)
	{
	case GOD_ROMAN:
		roman_cast_power( unitRecno, divider );
		break;

	case GOD_GREEK:
		greek_cast_power( unitRecno, divider );
		break;

	case GOD_MONGOL:
		mongol_cast_power( unitRecno, divider );
		break;

	case GOD_PERSIAN:
//		persian_cast_power( unitRecno, divider );
		break;

//	case GOD_JAPANESE:
//		japanese_cast_power( unitRecno, divider );
//		break;
/*
	case GOD_MAYA:
		maya_cast_power( unitRecno, divider );
		break;
*/

	case GOD_EGYPTIAN:
		egyptian_cast_power( unitRecno, divider);
		break;

	case GOD_INDIAN:
		indian_cast_power( unitRecno, divider);
		break;

	default:
		err_here();
	}
}
//---------- End of function UnitGod::cast_on_unit ----------//

/*
//--------- Begin of function UnitGod::cast_on_worker ---------//
//
// <Worker*> workerPtr - pointer to the worker to be affected by the effect.
//
void UnitGod::cast_on_worker(Worker* workerPtr, int nationRecno, int divider)
{
	switch(god_id)
	{
		case GOD_PERSIAN:
			persian_cast_power( workerPtr, nationRecno, divider );
			break;

		case GOD_JAPANESE:
			japanese_cast_power( workerPtr, nationRecno, divider );
			break;

		case GOD_MAYA:
			maya_cast_power( workerPtr, nationRecno, divider );
			break;

#ifdef AMPLUS
		case GOD_EGYPTIAN:
			egyptian_cast_power(workerPtr, nationRecno, divider);
			break;

		case GOD_INDIAN:
			indian_cast_power(workerPtr, nationRecno, divider);
			break;
	
		case GOD_ZULU:
			zulu_cast_power(workerPtr, nationRecno, divider);
			break;
#endif

		default:
			err_here();
	}
}
//---------- End of function UnitGod::cast_on_worker ----------//
*/


//--------- Begin of function UnitGod::roman_cast_power ---------//
//
void UnitGod::roman_cast_power(int unitRecno, int divider)
{
	// no effect
	Unit* unitPtr = unit_array[unitRecno];

	// ##### patch begin Gilbert 16/2 ########//
//	if( nation_recno == unitPtr->nation_recno && 
//		unitPtr->race_id == RACE_ROMAN && unitPtr->rank_id != RANK_SOLDIER)
	if( nation_recno == unitPtr->nation_recno && 
		unitPtr->is_human() && unitPtr->rank_id != RANK_SOLDIER)
	{
		unitPtr->skill.inc_skill_level( 30.0f / divider);
	}
	// ##### patch end Gilbert 16/2 ########//
}
//---------- End of function UnitGod::roman_cast_power ----------//



//--------- Begin of function UnitGod::greek_cast_power ---------//
//
void UnitGod::greek_cast_power(int unitRecno, int divider)
{
	if( unit_array.is_deleted(unitRecno) )
		return;

	Unit *unitPtr = unit_array[unitRecno];

	if( unitPtr->nation_recno != nation_recno )
		return;

	if( unitPtr->invulnerable_day_count < 30 )		// must ignore divider
		unitPtr->invulnerable_day_count = 30;
}
//--------- End of function UnitGod::greek_cast_power ---------//


//--------- Begin of function UnitGod::persian_cast_power ---------//
//
/*		// no longer healing effect
void UnitGod::persian_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only heal human units belonging to our nation --//

	if( unitPtr->nation_recno == nation_recno && unitPtr->race_id > 0 )
	{
		float changePoints = (float) unitPtr->max_hit_points() / (6+m.random(4));	 // divided by (6 to 9)

		changePoints = max( changePoints, 10 );

		unitPtr->change_hit_points( changePoints/divider );
	}
}
*/
//---------- End of function UnitGod::persian_cast_power ----------//

/*
//--------- Begin of function UnitGod::japanese_cast_power ---------//
//
void UnitGod::japanese_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only cast on enemy units -----//

	if( unitPtr->nation_recno != nation_recno && unitPtr->race_id > 0 )
	{
		int changePoints = 7 + m.random(8);		// decrease 7 to 15 loyalty points instantly

		unitPtr->change_loyalty( -max(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::japanese_cast_power ----------//
*/

//--------- Begin of function UnitGod::mongol_cast_power ---------//

void UnitGod::mongol_cast_power( int unitRecno, int divider )
{
	if( unit_array.is_deleted(unitRecno) )
		return;

	Unit *unitPtr = unit_array[unitRecno];

	if( unitPtr->rank_id == RANK_KING		// don't turn king, spy, or already own unit
		|| !unitPtr->is_visible()			// do not affect unit in building
		|| unitPtr->spy_recno 
		// ##### begin Gilbert 2/3 #####//
		|| unitPtr->nation_recno == 0			// independent units are not affected
		// ##### end Gilbert 2/3 #####//
		|| unitPtr->is_nation(nation_recno) )
		return;

	int turnProbability = 
		unitPtr->nation_recno ? 100 - (int)unitPtr->loyalty /2 : 75;
	// 100 loyalty has 50%, 0 loyalty has 100%, independent unit 50%

	if( divider > 1 && turnProbability > 0 && turnProbability < 100 )
	{
		// lower probability such that :
		// (1-new Probability) ^ divider = 1-old probability

		double p = double(100 - turnProbability) / 100.0;
		double q = exp( log(p) / (double) divider );

		turnProbability = 100 - int(q * 100.0);
	}

	if( m.random(100) >= turnProbability )
		return;

	unitPtr->betray( nation_recno );
}

//--------- End of function UnitGod::mongol_cast_power ---------//

/*
//--------- Begin of function UnitGod::maya_cast_power ---------//
//
void UnitGod::maya_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only cast on mayan units belonging to our nation --//

	if( unitPtr->nation_recno == nation_recno && unitPtr->race_id == RACE_MAYA )
	{
		int changePoints = 15 + m.random(10);		// add 15 to 25 points to its combat level instantly

		int newCombatLevel = unitPtr->combat_level() + changePoints/divider;

		if( newCombatLevel > 100 )
			newCombatLevel = 100;

		float oldHitPoints = unitPtr->hit_points;

		unitPtr->set_combat_level(newCombatLevel);

		unitPtr->hit_points = oldHitPoints;		// keep the hit points unchanged.
	}
}
//---------- End of function UnitGod::maya_cast_power ----------//


//--------- Begin of function UnitGod::persian_cast_power ---------//
//
void UnitGod::persian_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only heal human units belonging to our nation --//

	if( nationRecno == nation_recno && workerPtr->race_id > 0 )
	{
		int changePoints = workerPtr->max_hit_points() / (4+m.random(4));	 // divided by (4 to 7)

		changePoints = max( changePoints, 10 );

		workerPtr->change_hit_points( max(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::persian_cast_power ----------//


//--------- Begin of function UnitGod::japanese_cast_power ---------//
//
void UnitGod::japanese_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only cast on enemy units -----//

	if( nationRecno != nation_recno && workerPtr->race_id > 0 )
	{
		int changePoints = 7 + m.random(8);		// decrease 7 to 15 loyalty points instantly

		workerPtr->change_loyalty( -max(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::japanese_cast_power ----------//


//--------- Begin of function UnitGod::maya_cast_power ---------//
//
void UnitGod::maya_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only cast on mayan units belonging to our nation --//

	if( nationRecno == nation_recno && workerPtr->race_id == RACE_MAYA )
	{
		int changePoints = 15 + m.random(10);		// add 15 to 25 points to its combat level instantly

		int newCombatLevel = workerPtr->combat_level + max(1, changePoints/divider);

		if( newCombatLevel > 100 )
			newCombatLevel = 100;

		workerPtr->combat_level = newCombatLevel;
	}
}
//---------- End of function UnitGod::maya_cast_power ----------//
*/

//--------- Begin of function UnitGod::consume_power_pray_points ---------//
//
// ##### patch begin Gilbert 19/1 #######//
int UnitGod::consume_power_pray_points()
{
	FirmBase* firmBase = (FirmBase*) firm_array[base_firm_recno];

	err_when( firmBase->firm_id != FIRM_BASE );

	if( firmBase->pray_points < god_res[god_id]->power_pray_points - 10 )
	{
		// clear it to zero to avoid bugs
		hit_points = firmBase->pray_points = (float) 0;
		return 0;
	}
	else
	{
		firmBase->pray_points -= god_res[god_id]->power_pray_points;

		if( firmBase->pray_points < 0 )
			firmBase->pray_points = (float) 0;
		hit_points = firmBase->pray_points;
		return 1;
	}
}
// ##### patch end Gilbert 19/1 #######//
//---------- End of function UnitGod::consume_power_pray_points ----------//


//--------- Begin of function UnitGod::egyptian_cast_power ---------//
//
void UnitGod::egyptian_cast_power(int unitRecno, int divider)
{
	// no effect, except on a wagon
//	if( unit_array.is_deleted(unitRecno) )
//		return;
//	UnitWagon *wagonPtr = unit_array[unitRecno]->cast_to_UnitWagon();
//	if( !wagonPtr )
//		return;
//	int newPop = wagonPtr->population + 5/divider;
//	newPop = min(newPop, MAX_WAGON_POPULATION );
//	wagonPtr->set_pop(newPop);
}
//---------- End of function UnitGod::egyptian_cast_power ----------//

//--------- Begin of function UnitGod::indian_cast_power ---------//
//
void UnitGod::indian_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->is_visible() && nation_array.should_attack(nation_recno, unitPtr->nation_recno) )
	{
		unitPtr->change_loyalty(-30 + m.random(11));
	}
}
//---------- End of function UnitGod::indian_cast_power ----------//


/*
//--------- Begin of function UnitGod::egyptian_cast_power ---------//
//
void UnitGod::egyptian_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::egyptian_cast_power ----------//


//--------- Begin of function UnitGod::indian_cast_power ---------//
//
void UnitGod::indian_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::indian_cast_power ----------//


//--------- Begin of function UnitGod::zulu_cast_power ---------//
//
void UnitGod::zulu_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::zulu_cast_power ----------//
*/


//--------- Begin of function UnitGod::zulu_cast_power ---------//
//
// <int> testing      - testing only, no real action
// return true if cast magic
//
int UnitGod::right_click(int xLoc, int yLoc, int testing)
{
	if( !god_res[god_id]->can_cast_power )
		return 0;

	// check pray point
	FirmBase* firmBase = (FirmBase*) firm_array[base_firm_recno];
	err_when( firmBase->firm_id != FIRM_BASE );
	if( firmBase->pray_points < god_res[god_id]->power_pray_points )
		return 0;

	Location* locPtr = world.get_loc(xLoc, yLoc);
	int baseObjRecno = locPtr->base_obj_recno();

	if( !baseObjRecno )
		return 0;
	BaseObj *targetObj = base_obj_array[baseObjRecno];

	int flag = 0;

	switch( god_id )
	{
	case GOD_ROMAN:	// own general/king or own camp
		if( targetObj->nation_recno == nation_recno )
		{
			if( targetObj->cast_to_Unit() )
			{
				flag = targetObj->cast_to_Unit()->rank_id != RANK_SOLDIER;
			}
			else if( targetObj->cast_to_Firm() )
			{
				FirmCamp *camp;
				flag = (camp = targetObj->cast_to_Firm()->cast_to_FirmCamp())
					&& camp->overseer_recno;
			}
		}
		break;
	case GOD_MONGOL:	// enemy unit
		flag = targetObj->nation_recno != nation_recno
			&& targetObj->cast_to_Unit();
		break;
	case GOD_EGYPTIAN:	// own town
		flag = targetObj->nation_recno == nation_recno
			&& targetObj->cast_to_Town();
		break;
	case GOD_PERSIAN:		// enemy firm
		flag = targetObj->nation_recno != nation_recno
			&& targetObj->cast_to_Firm();
		break;
	case GOD_INDIAN:		// enemy object
		flag = targetObj->nation_recno != nation_recno;
		break;
	}

	if( flag )
	{
		if( !testing )
		{
			// no sound, UnitGod::right_click
			go_cast_power(xLoc, yLoc, COMMAND_PLAYER, 
				targetObj->cast_to_Unit() ? baseObjRecno : 0 );
		}
		return 1;
	}
	return 0;
}


// ##### patch begin Gilbert 9/11 ######//
// ------- begin of funtion UnitGod::attack_consume_power ------//
//
void UnitGod::attack_consume_power()
{
	consume_power_pray_points();

	Unit::attack_consume_power();
}
// ------- end of funtion UnitGod::attack_consume_power ------//
// ##### patch end Gilbert 9/11 ######//
