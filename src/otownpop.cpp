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

//Filename    : OTOWN.CPP
//Description : Object Town - functions for processing population

#include <otown.h>
#include <onews.h>
#include <of_mark.h>
#include <oconfig.h>
#include <of_camp.h>
#include <of_work.h>
#include <onews.h>
#include <oremote.h>
#include <oseres.h>
#include <ose.h>
#include <oraceres.h>
#include <ogodres.h>
#include <ospy.h>
#include <ounit.h>
#include <ou_wagon.h>
#include <onation.h>
#include <oitemres.h>
#include <oitem.h>
#include <itemid.h>


//--------- Begin of function Town::init_pop ----------//
//
// Initialize population of the town.
//
// <int> addPop  - the no. of people to add.
// <int> addLoyalty - loyalty or resistance of the people to add to the town
// [int] hasJob	  - whether the units will have jobs or not after moving into this town
//							 (default: 0)
// [int] firstInit - first initialization at the beginning of the game
//						   (default: 0)
//
void Town::init_pop(int addPop, int addLoyalty, int hasJob, int firstInit)
{
	if(population>=MAX_TOWN_POPULATION)
		return;

	int addPopulation = MIN(addPop, MAX_TOWN_POPULATION-population);

	//-------- update population ---------//

	population += addPopulation;

	err_when( population > MAX_TOWN_POPULATION );

	if( !hasJob )
		jobless_population += addPopulation;

	err_when( jobless_population > MAX_TOWN_POPULATION );

	//------- update loyalty --------//

	err_when( loyalty<0 || loyalty>100 );

	if( firstInit )	// first initialization at the beginning of the game
	{
		if( nation_recno )
		{
			loyalty = (float) addLoyalty;
			target_loyalty = addLoyalty;
		}
		else
		{
			for( int j=0 ; j<MAX_NATION ; j++)							// reset resistance for non-existing races
			{
				resistance_array[j] = (float) addLoyalty;
				target_resistance_array[j] = addLoyalty;
			}
		}
	}
	else
	{
		if( nation_recno )
		{
			loyalty = ( loyalty * (population-addPopulation)
							+ (float) addLoyalty * addPopulation )
							/ population;

			target_loyalty = (short) loyalty;
		}
		else
		{
			for( int j=0 ; j<MAX_NATION ; j++)							// reset resistance for non-existing races
			{
				resistance_array[j] =
					  ( resistance_array[j] * (population-addPopulation)
					  + (float) addLoyalty * addPopulation ) / population;

				target_resistance_array[j] = (short) resistance_array[j];
			}
		}
	}

	err_when( addLoyalty<0 || addLoyalty>100 );
	err_when( loyalty<0 || loyalty>100 );
	err_when( target_loyalty<0 || target_loyalty>100 );

	//---------- update town parameters ----------//

	town_array.distribute_demand();

	if( nation_recno )
	{
		update_target_loyalty();
		update_loyalty();
	}
	else
	{
		update_target_resistance();
		update_resistance();
	}
}
//--------- End of function Town::init_pop ----------//


//--------- Begin of function Town::inc_pop -------//
//
// <int> unitLoyalty - loyalty of the unit.
//
void Town::inc_pop(int unitLoyalty)
{
	err_when( unitLoyalty < 0 );

	//---------- increase population ----------//

	population++;
	jobless_population++;

	err_when( population > MAX_TOWN_POPULATION );
	err_when( jobless_population > MAX_TOWN_POPULATION );

	//------- update loyalty --------//

	if( nation_recno )		// if the unit has an unit
	{
		err_when( loyalty<0 || loyalty>100 );

		loyalty = ( loyalty * (population-1) + (float) unitLoyalty ) / population;

		err_when( unitLoyalty<0 || unitLoyalty>100 );
		err_when( loyalty<0 || loyalty>100 );
	}

   //------ update target loyalty ---------//

	update_target_loyalty();

	//-- if the race's population exceeds the capacity of the town layout --//

	if( population > max_layout_pop )
		auto_set_layout();
}
//---------- End of function Town::inc_pop --------//


//--------- Begin of function Town::dec_pop -------//
//
void Town::dec_pop()
{
	//--- if all people are employed, we first need to make one unemployed before decreasing the population ---//

	if( jobless_population==0 )
		unjob_town_people();		// ask a unit in the town who is currently at work to quit his job

	err_when( population < jobless_population );		// population must be larger than jobless_population

	//------ decrease population now -------//

	population--;
	jobless_population--;

	//------ update target loyalty ---------//

	update_target_loyalty();

	//-- if the race's population drops to too low, change the town layout --//

	if( population <= max_layout_pop - POPULATION_PER_HOUSE
		&& population != 0 )
	{
		auto_set_layout();
	}
}
//---------- End of function Town::dec_pop --------//


//--------- Begin of function Town::unjob_town_people -------//
//
// Ask a unit in the town who is currently at work to quit his job.
//
void Town::unjob_town_people()
{
	FirmWork *firmWork, *bestFirm=NULL;
	int		curRating, bestRating= -1;

	int i;
	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

		if( !firmWork )
			continue;

		int j;
		for( j=0 ; j<firmWork->worker_count ; j++ )
		{
			if( firmWork->worker_array[j].town_recno == town_recno )
				break;
		}

		if( j == firmWork->worker_count )		// nobody in the firm resides in this town
			continue;

		//------ calculate the rating --------//
		//
		// There is a higher chance of resign workers in firms with:
		//
		// - a longer distance from the town
		// - no productivity because of capacity or linkage problems.
		// - low under-employment rate - having most or all of the needed employees.
		// - high headcount, so that firms with low headcount have a smaller chance of losing employees.
		//
		//------------------------------------//

		curRating = world.distance_rating( center_x, center_y, firmWork->center_x, firmWork->center_y );

		curRating += firmWork->worker_count + 10 + 100 * firmWork->worker_count / firmWork->needed_worker_count;	 // two factors are taken into account here: the underemployment rate of the firm and the number of workers actually working here.

		if( firmWork->productivity==0 )	// there is no productivity in the firm, obviously due to some linkage or capacity problems, have a higher priority for resigning workers from these firm
			curRating += 100;

		//------------------------------------//

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestFirm   = firmWork;
		}
	}

	err_when( !bestFirm );		// there needs to be one

	//----- resign now -----//

	for( i=0 ; i<bestFirm->worker_count ; i++ )
	{
		if( bestFirm->worker_array[i].town_recno == town_recno )
		{
			bestFirm->resign_worker(i+1);
			return;
		}
	}

	err_here();		// shouldn't go here
}
//---------- End of function Town::unjob_town_people --------//


//--------- Begin of function Town::population_grow --------//
//
void Town::population_grow()
{
//	if( town_defender_count )
//		return;

	if( population >= MAX_TOWN_GROWTH_POPULATION || population >= MAX_TOWN_POPULATION )
		return;

	//-------- population growth by birth ---------//

	//-- the population growth in an independent town is slower than in a nation town ---//

	// ##### begin Gilbert 19/11 ######//

	int growthInc = 0;

	if( nation_recno )
		growthInc += population * (100+quality_of_life/2) / 100;		// quality of life can increase the population growth by a maximum of 50%.
	else
		growthInc += population / 2;		// slower population growth for independent towns

	growthInc /= 2;

	// ###### begin Gilbert 5/7 ########//
	if( nation_recno && race_id == RACE_EGYPTIAN )		// GOD EFFECT
	// ###### end Gilbert 5/7 ########//
	{
		growthInc += growthInc * god_res[GOD_EGYPTIAN]->nation_prayer_count(nation_recno) / (MAX_WORKER*3) ;
	}

	if( is_pay_tribute_to_monster() )		// if the town is paying tributes to monsters, the population growth is reduced by half 
		growthInc /= 2;

	population_growth += growthInc;
	// ##### end Gilbert 19/11 ######//

	int loopCount=0;

	while( population_growth > 100 )
	{
		err_when( loopCount++ > 100 );

		population_growth -= 100;

		population++;
		jobless_population++;

		err_when( population > MAX_TOWN_POPULATION );
		err_when( jobless_population > MAX_TOWN_POPULATION );

		//---------------------------------------//

		if(population>=MAX_TOWN_POPULATION)
			break;
	}

	//-- if the race's population exceeds the capacity of the town layout --//

	if( population > max_layout_pop )
		auto_set_layout();
}
//----------- End of function Town::population_grow --------//


//---------- Begin of function Town::update_target_loyalty --------//
//
// Update loyalty for nation towns.
//
void Town::update_target_loyalty()
{
	if( !nation_recno )		// return if independent towns
		return;

	//----- update loyalty of individual races -------//
	//
	// Loyalty is determined by:
	//
	// - whether the people are racially homogeneous to the king
	// - the nation's reputation
	// - command bases overseeing the town.
	// - quality of life
	// - employment rate
	//
	// Quality of life is determined by:
	//
	// - The provision of goods to the villagers. A more constant
	//	  supply and a bigger variety of goods give to high quality of life.
	//
	//------------------------------------------------//

	//------- set target loyalty of each race --------//

	Nation* nationPtr = nation_array[nation_recno];
	int 	  i, targetLoyalty;
	int 	  nationRaceId = nationPtr->race_id;

	//------- calculate the target loyalty -------//

	targetLoyalty = 10 +	bonus_loyalty +					// 10 base points + loyalty bonus which is set when a village change nation itself and the bonus will decrease over time
						 (int)nationPtr->reputation/2 +		// -50 to +50
						 quality_of_life/4;						// 0 to 25

	//---- employment help increase loyalty ----//

	if( population > 0 )
		targetLoyalty += 30 - 30 * jobless_population / population;		// +0 to +30

	if( race_res.is_same_race(race_id, nationRaceId) )
		targetLoyalty += 20;

	if( targetLoyalty < 0 )		// targetLoyalty can be negative if there are hostile races conflicts
		targetLoyalty = 0;

	if( targetLoyalty > 100 )
		targetLoyalty = 100;

	//----------------------------------------//

	target_loyalty = targetLoyalty;

	err_when( targetLoyalty<0 || targetLoyalty>100 );

	//----- process command bases that have influence on this town -----//

	int     baseInfluence, thisInfluence, commanderRaceId;
	Firm*   firmPtr;
	FirmCamp* firmCamp;
	Nation  *baseNationPtr;
	Unit*   unitPtr;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		firmPtr = firm_array[linked_firm_array[i]];

		if( !(firmCamp = firmPtr->cast_to_FirmCamp()) )
			continue;

		if( !firmCamp->overseer_recno || !firmCamp->nation_recno )
			continue;

		//-------- get nation and commander info ------------//

		unitPtr = unit_array[firmCamp->overseer_recno];
		commanderRaceId = unitPtr->race_id;

		baseNationPtr = nation_array[firmCamp->nation_recno];

		//------ if this race is the overseer's race -------//

		baseInfluence = unitPtr->skill_level()/3;		// 0 to 33
		// ###### begin Gilbert 16/12 ######//
		// no need because skill_leve() already contain the enhanced skill level
		//baseInfluence += unitPtr->item.ability(ITEM_ABILITY_SKILL_LEVEL) / 3;
		// ###### end Gilbert 16/12 ######//

		if( unitPtr->rank_id == RANK_KING )			// 20 points bonus for king
			baseInfluence += 20;

		//---- if the overseer's race is the same as this race ---//

		thisInfluence = baseInfluence;

		if( race_res.is_same_race(unitPtr->race_id, race_id) )
			thisInfluence += 8;

		//--- if the overseer's nation's race is the same as this race ---//

		if( race_res.is_same_race(baseNationPtr->race_id, race_id) )
			thisInfluence += 8;

		//------------------------------------------//

		if( firmCamp->nation_recno == nation_recno )	// if the command base belongs to the same nation
		{
			target_loyalty += thisInfluence * firmCamp->loyalty_adjustment()/100;
		}
		else if( unitPtr->race_id == race_id )		// for enemy camps, only decrease same race peasants
		{
			// -------- if it has wall influence is reduced --------- //

			if( current_wall_level > 0 )
				thisInfluence -= 20 * current_wall_level * (int)hit_points / max_hit_points();

			target_loyalty -= thisInfluence * firmCamp->loyalty_adjustment()/100;
		}
	}

	if( target_loyalty < 0 )
		target_loyalty = 0;

	if( target_loyalty > 100 )
		target_loyalty = 100;

	//------- update link status to linked enemy camps -------//

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( !firmPtr->cast_to_FirmCamp() )
			continue;

		//------------------------------------------//
		// If this town is linked to a own camp,
		// disable all links to enemy camps, otherwise
		// enable all links to enemy camps.
		//------------------------------------------//

		if( firmPtr->nation_recno != nation_recno )
			toggle_firm_link( i+1, !has_linked_own_camp, COMMAND_AUTO );
	}
}
//----------- End of function Town::update_target_loyalty ---------//


//------ Begin of function Town::update_loyalty -----//
//
// Update loyalty and resistance towards the target.
//
void Town::update_loyalty()
{
	if( !nation_recno || population==0 )
		return;

	//------------- update loyalty -------------//

	float loyaltyInc, loyaltyDec;

	if( loyalty < target_loyalty )
	{
		//--- if this town is linked to enemy camps, but no own camps, no increase in loyalty, only decrease ---//

		if( !has_linked_own_camp && has_linked_enemy_camp )
			return;

		//-------------------------------------//

		// ###### patch begin Gilbert 23/12 ######//
		//loyaltyInc = (target_loyalty-loyalty) / 30;
		// change_loyalty( MAX(loyaltyInc, (float)0.5) );

		// add penalty for egyptain god
		loyaltyInc = (target_loyalty-loyalty) / 30;
		if( loyaltyInc < (float)0.5 )
			loyaltyInc = (float)0.5;
		if( race_id == RACE_EGYPTIAN && god_res[GOD_EGYPTIAN]->nation_prayer_count(nation_recno) > 0 )
			loyaltyInc = loyaltyInc * (MAX_WORKER*2) / ( (MAX_WORKER*2) + god_res[GOD_EGYPTIAN]->nation_prayer_count(nation_recno));
		change_loyalty( loyaltyInc );
		// ###### end begin Gilbert 23/12 ######//
	}
	else if( loyalty > target_loyalty )
	{
		loyaltyDec = (loyalty-target_loyalty) / 30;

		change_loyalty( -MAX(loyaltyDec, (float)0.5) );
	}
}
//------- End of function Town::update_loyalty ------//


//------ Begin of function Town::change_loyalty -----//
//
// <float> loyaltyChange - the amount of change in loyalty.
//
void Town::change_loyalty(float loyaltyChange)
{
	loyalty += loyaltyChange;

	loyalty = MIN( 100, loyalty );
	loyalty = MAX(   0, loyalty );
}
//------- End of function Town::change_loyalty ------//


//------ Begin of function Town::change_resistance -----//
//
// <int>   nationRecno   	 - recno of the nation which the resistance is towards
// <float> resistanceChange - the amount of change in resistance.
//
void Town::change_resistance(int nationRecno, float resistanceChange)
{
	err_when( nation_array.is_deleted(nationRecno) );

	float newResistance = resistance_array[nationRecno-1] + resistanceChange;

	newResistance = MIN( 100, newResistance );
	newResistance = MAX(   0, newResistance );

	resistance_array[nationRecno-1] = newResistance;
}
//------- End of function Town::change_resistance ------//


//------ Begin of function Town::update_target_resistance ------//
//
// Influence from a command base to a town (simplied version)
// ----------------------------------------------------------
// -Reputation of the nation
// -Race of the general and the king (whether its race is the same as the town people's race)
// -Leadership of the general
//
// If the conqueror's race id is the same as the town people's race id.
//   Influence = reputation/2
// else
//   Influence = reputation/4
//
// If the general's race is the same as the town people's race,
//   Influence += general leadership/2
// else
//   Influence += general leadership/4
//
void Town::update_target_resistance()
{
	int i, j;

	if( population==0 || linked_firm_count==0 )
		return;

	for( j=0 ; j<MAX_NATION ; j++ )
		target_resistance_array[j] = -1;		// -1 means influence is not present

	//---- if this town is controlled by rebels, no decrease in resistance ----//

	if( rebel_recno )
		return;

	//----- count the command base that has influence on this town -----//

	int newValue;
	FirmCamp* firmCamp;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		firmCamp = firm_array[linked_firm_array[i]]->cast_to_FirmCamp();

		if( !firmCamp || !firmCamp->overseer_recno || !firmCamp->nation_recno )
			continue;

		//-------- get nation and commander info ------------//

		short &curValue = target_resistance_array[firmCamp->nation_recno-1];

		// -------- if it has wall influence is reduced --------- //

		int infAdj = 100;

		if( current_wall_level > 0 && hit_points > 0.0f )
		{
			infAdj -= (int)hit_points / 10;
		}

		infAdj = infAdj * firmCamp->resistance_adjustment() / 100;

		newValue = 100-camp_influence(firmCamp)*infAdj/100;

		// ###### begin Gilbert 19/11 ######//
		// ------- effect of god --------//
		if( firmCamp->nation_recno && god_res[GOD_MONGOL]->nation_prayer_count(firmCamp->nation_recno) > 0 )
			newValue -= 20;
		// ###### end Gilbert 19/11 ######//

		newValue = MAX(newValue, 0);

		// -------- take the minimum target resistance -------- //

		err_when( newValue < 0 );

		if( curValue == -1 || newValue < curValue )		// need to do this comparsion as there may be more than one command bases of the same nation linked to this town, we use the one with the most influence.
			curValue = newValue;
	}
}
//------- End of function Town::update_target_resistance ------//


//------ Begin of function Town::update_resistance -----//
//
// Update loyalty and resistance towards the target.
//
void Town::update_resistance()
{
	//------------- update resistance ----------------//

	int	j, maxNation = nation_array.size();
	float targetResistance;

	for(j=0; j<maxNation; j++)
	{
		if( nation_array.is_deleted(j+1) )
			continue;

		if( target_resistance_array[j] >= 0 )
		{
			targetResistance = (float) target_resistance_array[j];

			if(resistance_array[j] > targetResistance)		// only decrease, no increase for resistance
			{
				float decValue = (resistance_array[j]-targetResistance) / 30;

				//--- the resistance decrease rate is affected by the Resistance setting ---//

				if( config.independent_town_resistance == OPTION_MODERATE )
					decValue = decValue * 2 / 3;		// 66% of the original

				if( config.independent_town_resistance == OPTION_HIGH )
					decValue = decValue / 3;			// 33% of the original

				resistance_array[j] -= MAX(1, decValue);

				if(resistance_array[j] < targetResistance) // avoid resistance oscillate between taregtLoyalty-1 and taregtLoyalty+1
					resistance_array[j] = (float)targetResistance;
			}
		}

		//---- surrender if the resistance drops below 1 ----//

		if( resistance_array[j] < SURRENDER_RESISTANCE )          		// also values between consider 0 and 1 as zero as they are displayed as 0 in the interface
		{
			if( think_surrender() )
				break;
		}
	}
}
//------- End of function Town::update_resistance ------//


//------- Begin of function Town::migrate ---------//
//
// People migrate from one town to another.
//
// <int> destTownRecno - recno of the destination town.
// <int> newLoyalty    - loyalty of the unit in the target town.
//
// return: <int> 1 - migrated successfully
//               0 - no building space in the target town for that race
//
void Town::migrate(int destTownRecno, int newLoyalty)
{
	Town* destTown = town_array[destTownRecno];

	err_when( destTown->race_id != race_id );

	if(destTown->population>=MAX_TOWN_POPULATION)
		return;

	//------- decrease the population of this town ------//

	dec_pop();

	//--------- increase the population of the target town ------//

	destTown->inc_pop(newLoyalty);
}
//-------- End of function Town::migrate -----------//


//------- Begin of function Town::migrate_to ---------//

int Town::migrate_to(int destTownRecno, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <dest town recno>
		// ##### begin Gilbert 7/10 ######//
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_MIGRATE, 2*sizeof(short));
		// ##### end Gilbert 7/10 ######//
		shortPtr[0] = town_recno;
		shortPtr[1] = destTownRecno;
		return 0;
	}

	return can_migrate(destTownRecno, 1);		// 1- migrate now, 1-allow migrate spy
}
//-------- End of function Town::migrate_to -----------//


//------- Begin of function Town::can_migrate ---------//
//
// Check if it is okay for migrating one unit from
// the current town to the given town.
//
// <int> destTownRecno - the recno of the town the people
//								 will migrate to.
//
// [int] migrateNow 	  - migrate now or not if the result is true
//								 (default: 0)
//
// return: <int> 1 - migration allowed
//					  0 - migration not allowed
//
int Town::can_migrate(int destTownRecno, int migrateNow)
{
	Town* destTown = town_array[destTownRecno];

	// ###### begin Gilbert 20/11 #####//
	// err_when( destTown->race_id != race_id );
	if( destTown->race_id != race_id )
		return 0;
	// ###### end Gilbert 20/11 #####//

	// ####### begin Gilbert 19/4 #######//
	if( destTown->under_construction)
		return 0;
	// ####### end Gilbert 19/4 #######//

	if( destTown->population>=MAX_TOWN_POPULATION )
		return 0;

	//---- if there are still jobless units ----//

	if( recruitable_pop(1) > 0 )		// 1-allow migrate spy
	{
		if( migrateNow )
			move_pop(destTown);		

		return 1;
	}
	else
		return 0;
}
//-------- End of function Town::can_migrate -----------//


//------- Begin of function Town::move_pop ---------//
//
// This function is called by Town::can_migrate() only to move a unit
// from one town to another.
//
void Town::move_pop(Town* destTown)
{
	if( population==0 )
		return;

	err_when( spy_count > population );

	//--- if the only pop of this race in the source town are spies ---//

	if(  spy_count > 0 && spy_count == population )
	{
		int spySeqId = m.random( spy_count ) + 1;		// randomly pick one of the spies

		int spyRecno = spy_array.find_town_spy(town_recno, spySeqId);

		err_when( !spyRecno );

		spy_array[spyRecno]->spy_place_para = destTown->town_recno;		// set the place_para of the spy

		spy_count--;
		destTown->spy_count++;
	}

	//--- if all people in town are employed, we need to migrate an employed unit and update his employment record in the firm he works for ---//

	int moveWorkerSuccess=0;

	if( jobless_population == 0 )
	{
		for( int i=0 ; i<linked_firm_count ; i++ )
		{
			FirmWork* firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

			if( !firmWork )
				continue;

			//-- get the distance between the firm and the target town ---//

			int newDistance = m.points_distance( firmWork->center_x, firmWork->center_y,
									destTown->center_x, destTown->center_y  );

			//--- the firm must be within effective distance of the target town ---//

			if( newDistance > world.effective_distance(firmWork->firm_id, 0) )
				continue;

			//---- find a worker in the firm who resides in this town ----//

			for( int j=0 ; j<firmWork->worker_count ; j++ )
			{
				if( firmWork->worker_array[j].town_recno == town_recno )
				{
					firmWork->worker_array[j].town_recno = destTown->town_recno;
					moveWorkerSuccess=1;
					break;
				}
			}

			if( moveWorkerSuccess )
				break;
		}
	}

	//-------- move population now-----------//

	if( moveWorkerSuccess )					// if the migrated unit can still work in the original firm, decrease the jobless population counter 
		jobless_population++;

	dec_pop();
	destTown->inc_pop( (int)loyalty );

	if( moveWorkerSuccess )					// if the migrated unit can still work in the original firm, decrease the jobless population counter 
		destTown->jobless_population--;

	err_when( spy_count > population );
	err_when( destTown->spy_count > destTown->population );
}
//-------- End of function Town::move_pop -----------//


//--------- Begin of function Town::assign_unit --------//
//
void Town::assign_unit(int unitRecno)
{
	//---- if the town is still under construction ----//

	if( under_construction )
		return;

	//-----------------------------------------//

	Unit* unitPtr = unit_array[unitRecno];

	err_when( race_id != unitPtr->race_id );

	int unitPopulation = 1;
	UnitWagon *wagonPtr = NULL;

	if( unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_WAGON )
	{
		wagonPtr = (UnitWagon *)unitPtr;
		unitPopulation = wagonPtr->population;
		err_when(unitPopulation <= 0);
		err_when(unitPopulation > MAX_WAGON_POPULATION);
	}

	//--- if aftr assigning the wagon, the population exceeds the maximum, don't assign ---//

	if( population + unitPopulation > MAX_TOWN_POPULATION )
		return;

	//------ if the unit is a general, demote it first -------//

	err_when( unitPtr->rank_id == RANK_KING );

	if( unitPtr->rank_id == RANK_GENERAL )
		unitPtr->set_rank(RANK_SOLDIER);

	//-------- increase population -------//

	int i;
	for( i=0 ; i<unitPopulation ; i++ )
		inc_pop(unitPtr->loyalty);

	//---- free the unit's name from the name database ----//

	if( wagonPtr )
	{
	}
	else
	{
		if( unitPtr->name_id )
		{
			race_res[unitPtr->race_id]->free_name_id( unitPtr->name_id );
			unitPtr->name_id = 0;
		}		
	}

	//----- if it's a town defending unit -----//

	if( unitPtr->unit_mode == UNIT_MODE_TOWN_DEFENDER )
	{
		#define RESISTANCE_INCREASE  2   // if the defender defeat the attackers and return the town with victory, the resistance will further increase

		//---------------------------------------------//
		//
		// If this unit is a defender of the town, add back the
		// loyalty which was deducted from the defender left the
		// town.
		//
		//---------------------------------------------//

		if( unitPtr->nation_recno == nation_recno &&
			 unitPtr->unit_mode_para == town_recno )
		{
			//-- if the unit is a town defender, skill.skill_level is temporary used for storing the loyalty that will be added back to the town if the defender returns to the town

			int loyaltyInc = unitPtr->skill_level();

			if( nation_recno )			// set the loyalty later for nation_recno > 0
			{
				change_loyalty( (float) loyaltyInc );
			}
			else
			{
				for( i=0 ; i<MAX_NATION ; i++ )		// set the resistance
				{
					//--- if the town is already paying tribute to the nation, don't increase the resistance to that nation ---//

					// ###### patch begin Gilbert 29/3 ######//
					if( last_attack_nation_recno == i+1		// Unit::mobilize_defender set the last_attack_nation_recno
						&& !is_pay_tribute_to_monster(i+1) )
					// ###### patch end Gilbert 29/3 ######//
					{
						float newResistance = resistance_array[i] + loyaltyInc + RESISTANCE_INCREASE;
						resistance_array[i] = MIN(newResistance, 100);
					}
				}
			}
		}
	}

	//------ if the unit is a spy -------//

	if( unitPtr->spy_recno > 0 )
	{
		spy_array[unitPtr->spy_recno]->set_place(SPY_TOWN, town_recno);
		unitPtr->spy_recno = 0;		// reset it so Unit::deinit() won't delete the spy
	}

	if( wagonPtr )
	{	
		int loopCount = 0;

		for(i = wagonPtr->spy_count; i > 0; --i )
		{
			short spyRecno = wagonPtr->spy_recno[i-1];
			err_when( spy_array.is_deleted(spyRecno) );
			spy_array[spyRecno]->set_place( SPY_TOWN, town_recno );
			err_when( loopCount++ > MAX_WAGON_POPULATION );
		}
	}

	//----- if this is an independent town -----//

	if( nation_recno==0 )		// update the town people's combat level with this unit's combat level
	{
		if( wagonPtr )
			town_combat_level = ( (int)town_combat_level*(population-unitPopulation) + unitPopulation * CITIZEN_COMBAT_LEVEL ) / population;
		else
			town_combat_level = ( (int)town_combat_level*(population-1) + (int)unitPtr->combat_level() ) / population;
	}

	//--------- delete the unit --------//

	unit_array.disappear_in_town(unitRecno, town_recno);		// the unit disappear from the map because it has moved into a town
}
//---------- End of function Town::assign_unit --------//


//-------- Begin of function Town::mobilize_town_people --------//
//
// Call out a citizen of the town population and make it
// as an unit.
//
// <int> decPop - whether decrease the population or not,
//						if 0, only a unit will be created and no population will
//						be reduced.
// <int> mobileSpyFlag - whether spies will be mobilized
//
// return: <int> recno of the unit created.
//
int Town::mobilize_town_people(int decPop, int mobileSpyFlag)
{
	if( recruitable_pop(mobileSpyFlag)==0 )
		return 0;

	err_when( recruitable_pop(mobileSpyFlag)==0 );

	//----look for an empty locatino for the unit to stand ----//
	//--- scan for the 5 rows right below the building ---//

	//--- for debug only to test the effect of special unit
	int            unitId = race_res[race_id]->civilian_unit_id;

	SpriteInfo*    spriteInfo = sprite_res[unit_res[unitId]->sprite_id];
	int            xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results

	if( !world.locate_space( xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return 0;

	//---------- add the unit now -----------//

	int unitRecno = unit_array.add_unit( unitId,
						 nation_recno, RANK_SOLDIER, (int)loyalty, xLoc, yLoc );

	//------- set the unit's parameters --------//

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_combat_level(CITIZEN_COMBAT_LEVEL);

	//-------- decrease the town's population ------//

	if( decPop )
		dec_pop();

	return unitRecno;
}
//--------- End of function Town::mobilize_town_people ---------//


//-------- Begin of function Town::mobilize_wagon --------//
//
// Call out a wagon of citizens of the town population and make it
// as an unit.
//
// <int> decPop - whether decrease the population or not,
//						if 0, only a unit will be created and no population will
//						be reduced.
// <int> mobileSpyFlag - whether spies will be mobilized
//
// return: <int> recno of the unit created.
//
int Town::mobilize_wagon(int decPop, int wagonPopulation, short spyArrayCount, short *spyArray)
{
	//---- if no jobless people, make workers and overseers jobless ----//

	int peopleFreed = recruitable_pop(1);

	peopleFreed = MIN( peopleFreed, wagonPopulation );

	//----look for an empty location for the unit to stand ----//
	//--- scan for the 5 rows right below the building ---//

	int            unitId = UNIT_WAGON;
	SpriteInfo*    spriteInfo = sprite_res[unit_res[unitId]->sprite_id];
	int            xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results

	if( !world.locate_space( xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return 0;

	//---------- add the unit now -----------//

	int unitRecno = unit_array.add_unit( unitId,
						 nation_recno, RANK_SOLDIER, (int)loyalty, xLoc, yLoc );

	//------- set the unit's parameters --------//

	UnitWagon* unitPtr = (UnitWagon *)unit_array[unitRecno];

	unitPtr->set_race(race_id);
	unitPtr->set_pop( peopleFreed );

	//-------- put spy in spy array ---------//

	int i;
	for( i = 0; i < spyArrayCount && i < peopleFreed; ++i )
	{
		spy_array[spyArray[i]]->set_place(SPY_WAGON, unitRecno);
	}

	//-------- decrease the town's population ------//

	if( decPop )
	{
		for( i = peopleFreed; i > 0; --i )
			dec_pop();
	}

	return unitRecno;
}
//--------- End of function Town::mobilize_wagon ---------//


//--------- Begin of function Town::kill_town_people -------//
//
void Town::kill_town_people(int attackerNationRecno)
{
	if( population == 0 )
		return;

	// ###### begin Gilbert 18/6 ########//
	//------ the killed unit can be a spy -----//
//	if( m.random(population) < spy_count )
//	{
//		int spyRecno = spy_array.find_town_spy(town_recno, m.random(spy_count)+1 );
//		spy_array.del_spy(spyRecno);
//	}
	// ###### end Gilbert 18/6 ########//

	//---- killing civilian people decreases loyalty -----//

	if( nation_recno && attackerNationRecno )					// your people's loyalty decreases because you cannot protect them.
		nation_array[nation_recno]->civilian_killed(race_id, 0);		// but only when your units are killed by enemies, neutral disasters are not counted

	if( attackerNationRecno )        //	the attacker's people's loyalty decreases because of the killing actions.
		nation_array[attackerNationRecno]->civilian_killed(race_id, 1);		// the nation is the attacking one

	// -------- sound effect ---------//

	se_res.sound( center_x, center_y, 1, 'R', race_id, "DIE" );

	//-------- decrease population now --------//

	// ###### begin Gilbert 18/6 ########//
	//------ the killed unit can be a spy -----//
	if( m.random(population) < spy_count )
	{
		int spyRecno = spy_array.find_town_spy(town_recno, m.random(spy_count)+1 );
		// spy_array.del_spy(spyRecno);

		Spy *spyPtr = spy_array[spyRecno];
		spyPtr->get_killed(1);		// 1 - display news
	}
	else		// Spy::get_killed() will call town's dec_pop();
	{
		dec_pop();
	}
	// ###### end Gilbert 18/6 ########//

	//---- the attacker absorbs the live points of the victim ----//

	if( attackerNationRecno )
	{
		Nation* nationPtr = nation_array[attackerNationRecno];

		if( nationPtr->is_monster() )
			nationPtr->change_live_points( CITIZEN_COMBAT_LEVEL * 2 * 2 );		// 1st * 2 - combat level to hit points, 2nd * 2 - killing humans gives twice as much live points
	}
}
//---------- End of function Town::kill_town_people -----//


//------ Begin of function Town::update_quality_of_life -------//
//
// Quality of life is determined by:
//
// - The provision of goods to the villagers. A more constant
//	  supply and a bigger variety of goods give to high quality of life.
//
void Town::update_quality_of_life()
{
	Firm* 		firmPtr;
	FirmMarket* firmMarket;

	//--- calculate the estimated total purchase from this town ----//

	float townDemand = (float) jobless_population * (float) PEASANT_GOODS_MONTH_DEMAND
							 + (float) worker_population() * (float) WORKER_GOODS_MONTH_DEMAND;

	float totalPurchase = (float) 0;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		firmPtr = firm_array[ linked_firm_array[i] ];

		if( firmPtr->firm_id != FIRM_MARKET )
			continue;

		firmMarket = (FirmMarket*) firmPtr;

		//-------------------------------------//

		MarketGoods* marketGoods = firmMarket->market_goods_array;

		for( int j=0 ; j<MAX_MARKET_GOODS ; j++, marketGoods++ )
		{
			if( !marketGoods->product_raw_id || marketGoods->month_demand==0 )
				continue;

			float monthSaleQty = marketGoods->sale_qty_30days();

			if( monthSaleQty > marketGoods->month_demand )
			{
				totalPurchase += townDemand;
			}
			else if( marketGoods->month_demand > townDemand )
			{
				totalPurchase += monthSaleQty * townDemand / marketGoods->month_demand;
			}
			else
				totalPurchase += monthSaleQty;
		}
	}

	//------ return the quality of life ------//

	quality_of_life = int( (float)100 * totalPurchase / (townDemand * MAX_PRODUCT) );
}
//----------- End of function Town::update_quality_of_life ---------//

//--------- Begin of function Town::recruit ---------//
//
// <bool> recruitWagon - Whether the unit going to be recruited is a
//								 wagon. If not, it is then a civilian unit.
//
// <char> remoteAction - remote action type
//
// return: <int> recno of the recruited unit
//
int Town::recruit(bool recruitWagon, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <recruitWagon>
		// ##### begin Gilbert 7/10 ######//
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 2*sizeof(short));
		shortPtr[0] = town_recno;
		shortPtr[1] = recruitWagon ? 1 : 0;
		// ##### end Gilbert 7/10 ######//
		return 0;
	}

	//---- check if there are units of the race ready for training ----//

	int recruitCount = recruitable_pop(1);		// 1-also recruit spy. Don't exclude them from recruitment

	if( recruitWagon )
		recruitCount = MIN( recruitCount, MAX_WAGON_POPULATION );
	else
		recruitCount = MIN( recruitCount, 1 );

	if( recruitCount==1 )    // if only one unit is recruited, never recruit a wagon
		recruitWagon = 0;

	if( recruitCount == 0 )
		return 0;

	err_when( recruitCount < 0 );

	//--- if there are spies in this town, chances are that they will be mobilized ---//

	int   spyArrayCount = 0;
	short spyArray[MAX_WAGON_POPULATION];

	if( spy_count > 0 )
	{
		// ######## begin Gilbert 27/1 ##########//
		//-----------------------------------------------------//
		// Spies from other nations will first be mobilized,
		// when all peasants and spies are mobilized and
		// the only ones left in the town are spies from our
		// nation, then mobilize them finally.
		//-----------------------------------------------------//

		// ----- collect some counter spy and enemy, first 10 are enough ----//

		int counterSpyCount = 0;
		short counterSpyArray[MAX_WAGON_POPULATION];
		int enemySpyCount = 0;
		short enemySpyArray[MAX_WAGON_POPULATION];

		int spyRecno = m.random(spy_array.size());

		for( int spyCount = spy_array.size(); spyCount > 0 ; --spyCount )
		{
			if( ++spyRecno > spy_array.size() )
				spyRecno = 1;

			if( spy_array.is_deleted(spyRecno) )
				continue;

			Spy *spyPtr = spy_array[spyRecno];

			if( spyPtr->spy_place == SPY_TOWN 
				&& spyPtr->spy_place_para == town_recno )
			{
				if( spyPtr->true_nation_recno == nation_recno )
				{
					if( counterSpyCount < MAX_WAGON_POPULATION )
						counterSpyArray[counterSpyCount++] = spyRecno;
					else
						counterSpyCount++;		// still increase to get correct probability
				}
				else
				{
					if( enemySpyCount < MAX_WAGON_POPULATION )
						enemySpyArray[enemySpyCount++] = spyRecno;
					else
						enemySpyCount++;			// still increase to get correct probability
				}
			}
		}

		// ------- put enemy spy first -------//

		err_when( recruitCount > sizeof(enemySpyArray) / sizeof(enemySpyArray[0]) );
		err_when( recruitCount > sizeof(counterSpyArray) / sizeof(counterSpyArray[0]) );

		short *enemySpyRecnoPtr = enemySpyArray;
		short *counterSpyRecnoPtr = counterSpyArray;
		for( int i = 0; i < recruitCount; ++i )
		{
			if( population-i-counterSpyCount > 0 )
			{
				// -------- put enemy spy -------//

				if( m.random(population-i-counterSpyCount) < enemySpyCount )
				{
					spyArray[spyArrayCount++] = *enemySpyRecnoPtr;
					// don't take last enemySpyArray because enemySpyCount may be
					// greater than size of enemySpyArray
					--enemySpyCount;
					++enemySpyRecnoPtr;
				}
			}
			else
			{
				// ------- put counter spy at last -------//

				spyArray[spyArrayCount++] = *counterSpyRecnoPtr;
				// don't take last counterSpyArray because counterSpyCount may be
				// greater than size of counterSpyArray
				--counterSpyCount;
				++counterSpyRecnoPtr;
			}
		}
		// ######## end Gilbert 27/1 ##########//
	}

	//------ decrease loyalty -------//

	recruit_dec_loyalty(recruitCount, 1);		// 1-decrease loyalty immediately

	//------- create the mobilized unit ------//

	int unitRecno;

	if( recruitCount==1 )		// recruit a single unit
	{
		err_when( spyArrayCount > 1 );

		if( spyArrayCount )
			unitRecno = spy_array[spyArray[0]]->mobilize_town_spy(1);		// 1-decrease population immediately
		else
			unitRecno = mobilize_town_people(1, 0); 	// the 1st parameter is whether decreasing the population immediately, if decrease immediately in recruit mode, not in training mode, 2nd para 0-don't mobilize spies
	}
	else
	{
		// mobilize wagon
		unitRecno = mobilize_wagon(1, recruitCount, spyArrayCount, spyArray ); 	//1-decrease population immediately
	}

	if( !unitRecno )		// creation is not successful
		return 0;

	//-------- produce sound ---------//

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->is_own() )
		unitPtr->sound("RDY");

	return unitRecno;
}
//----------- End of function Town::recruit -----------//


//--------- Begin of function Town::recruit_dec_loyalty ---------//
//
// Decrease loyalty when an unit is recruited.
// This function is called by recruit() and Firm::pull_town_people()
//
// <int> recruitCount - the number of people recruited
//								if it is > 1, they are recruited to form a wagon.
//
// <int> decNow - decrease now, if it is 0, just return the
//						loyalty to be decreased without actual decreasing.
//						(default: 1)
//
// return: <int> - the loyalty decreased or to be decreased.
//
int Town::recruit_dec_loyalty(int recruitCount, int decNow)
{
	float loyaltyDec;

	if( population > 0 )
		loyaltyDec = MIN( 5, (float) MAX_TOWN_POPULATION / population );
	else
		loyaltyDec = 0;

	//------ recruitment without training decreases loyalty --------//

	if( decNow )
	{
		loyaltyDec += accumulated_recruit_penalty/5;
		loyaltyDec = MIN(loyaltyDec, 10);

		accumulated_recruit_penalty += 5;

		//-------------------------------------//

		change_loyalty( -loyaltyDec );
		//race_loyalty_array[raceId-1] -= loyaltyDec;
		//if( race_loyalty_array[raceId-1] < 0 )
		//	race_loyalty_array[raceId-1] = (float) 0;
	}

	return (int) loyaltyDec;
}
//----------- End of function Town::recruit_dec_loyalty -----------//


//-------- Begin of function Town::recruitable_pop ------//
//
// Return the number of units can be recruited from this town.
//
// People who currently have a job will also can recruited
// if there are no jobless peasants in the town. Though,
// jobless peasants will be recruited first.
//
// <int> recruitSpy - whether spies will be recruited or not.
//							 if no, spies are not counted in the
//							 total no. of units that can be recruited.
//
int Town::recruitable_pop(int recruitSpy)
{
	short recruitableCount = population;

	if( !recruitSpy )
	{
		recruitableCount -= spy_count;

		if( recruitableCount == -1 )		// it may have been reduced twice if the unit being trained is a spy
			recruitableCount = 0;
	}

	err_when( recruitableCount < 0 );

	return recruitableCount;
}
//---------- End of function Town::recruitable_pop --------//


//-------- Begin of function Town::evenly_distribute_worker ------//
//
// Try to achieve even workforce distribution by transferring
// workers from high employment firms to low employment firms.
//
void Town::evenly_distribute_worker()
{
	//--- if there is still available workforce, do nothing ---//

	if( jobless_population > 0 )
		return;

	//--- calculate the average employment percentage of all linked FirmWork ---//

	int totalEmploymentRate=0, totalFirmWork=0;
	FirmWork* firmWork;

	int i;
	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

		// ####### begin Gilbert 22/4 #########//
		if( !firmWork || firmWork->needed_worker_count == 0)
			continue;
		// ####### end Gilbert 22/4 #########//

		totalFirmWork++;
		totalEmploymentRate += 100 * firmWork->worker_count / firmWork->needed_worker_count;
	}

	if( totalFirmWork==0 )
		return;

	int avgEmploymentRate = totalEmploymentRate / totalFirmWork;

	//------ try to achieve even distribution of workers  -----//

	int employmentRate;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

		// ####### begin Gilbert 22/4 #########//
		if( !firmWork || firmWork->needed_worker_count == 0)
			continue;
		// ####### end Gilbert 22/4 #########//

		employmentRate = 100 * firmWork->worker_count / firmWork->needed_worker_count;

		if( employmentRate >= avgEmploymentRate )
			continue;

		//--- if the employment rate in this firm is below average, try to increase it by transfering workers from other firms to it ----//

		for( int j=0 ; j<linked_firm_count ; j++ )
		{
			if( i==j )
				continue;

			FirmWork* otherFirmWork = firm_array[linked_firm_array[j]]->cast_to_FirmWork();

			if( !otherFirmWork || otherFirmWork->needed_worker_count==0 )
				continue;

			employmentRate = 100 * otherFirmWork->worker_count / otherFirmWork->needed_worker_count;

			//--- only transfer workers from firms with higher than average employment ---//

			if( employmentRate <= avgEmploymentRate )
				continue;

			//-----------------------------------------------//
			//
			// Only transfer when:
			//
			// - the other firm's employment rate is still higher than
			//   or equal to the average after the transfer.
			//
			// - the gain in employment rate of this firm is larger
			//   than the drop in employment rate of the other firm.
			//
			//-----------------------------------------------//

			if( 100 * (otherFirmWork->worker_count-1) / otherFirmWork->needed_worker_count
				 >= avgEmploymentRate ||
				firmWork->needed_worker_count < otherFirmWork->needed_worker_count )
			{
				//-------- transfer now ---------//

				if( transfer_worker(firmWork, otherFirmWork) )
					return;
			}
		}
	}
}
//---------- End of function Town::evenly_distribute_worker --------//


//-------- Begin of function Town::can_accept_assign ------//
//
// Return whether this town accepts a new unit.
//
// <int> unitRecno - recno of the unit
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
bool Town::can_accept_assign(int unitRecno, int actionNationRecno)
{
	if( under_construction )
		return false;

	if( population >= MAX_TOWN_POPULATION )
		return false;

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->rank_id == RANK_KING )		// cannot assign a king to a town
		return false;

	return unitPtr->race_id == race_id &&
			 unitPtr->nation_recno == nation_recno;
}
//---------- End of function Town::can_accept_assign --------//


//-------- Begin of function Town::transfer_worker ------//
//
// Transfer a worker residing in this town from one firm to another.
//
int Town::transfer_worker(FirmWork* srcFirmWork, FirmWork* destFirmWork)
{
	for( int i=0 ; i<srcFirmWork->worker_count ; i++ )
	{
		if( srcFirmWork->worker_array[i].town_recno == town_recno )
		{
			srcFirmWork->resign_worker(i+1);
			destFirmWork->recruit_worker();
			return 1;
		}
	}

	return 0;
}
//---------- End of function Town::transfer_worker --------//

