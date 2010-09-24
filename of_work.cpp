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

//Filename    : OF_WORK.CPP
//Description : FirmWork

#include <OF_WORK.H>
#include <OINFO.H>
#include <OSYS.H>
#include <OTOWN.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OSPY.H>


#define INIT_NEEDED_WORKER_COUNT 10


//---------- Begin of function FirmWork::FirmWork --------//
//
FirmWork::FirmWork()
{
	worker_count = 0;
	needed_worker_count = INIT_NEEDED_WORKER_COUNT;
	selected_worker_id = 0;

	productivity = 0;

	//--------- financial vars ---------//

	last_year_income = 0;
	cur_year_income  = 0;

	// -------- interface vars ---------//

	selected_worker_id = 0;
}
//----------- End of function FirmWork::FirmWork ---------//


//---------- Begin of function FirmWork::deinit_derived --------//
//
void FirmWork::deinit_derived()
{
}
//---------- End of function FirmWork::deinit_derived --------//


//---------- Begin of function FirmWork::free_all_people --------//
//
void FirmWork::free_all_people()
{
	// ###### begin Gilbert 15/9 #######//
	Firm::free_all_people();
	// ###### end Gilbert 15/9 #######//

	if( !sys.signal_exit_flag )
	{
		resign_all_worker();
	}
}
//---------- End of function FirmWork::free_all_people --------//


//---------- Begin of function FirmWork::next_day --------//
//
void FirmWork::next_day()
{
	Firm::next_day();

	//------ recruit workers ---------//

	if( info.game_date%5 == firm_recno%5 )    // recruit workers once 5 days
		recruit_worker();

	//--- reduce independent town resistance if their people work here ---//

	if( info.game_date%15 == firm_recno%15 )
		process_independent_town_worker();

	//----- calculate productivity -----//

	calc_productivity();
}
//----------- End of function FirmWork::next_day ---------//


//---------- Begin of function FirmWork::pay_expense --------//
//
void FirmWork::pay_expense()
{
	//----- paying salary to workers  -----//

	nation_array[nation_recno]->add_expense( EXPENSE_FIRM,
		(float) WORKER_YEAR_SALARY * worker_count / 365, 1 );

	Firm::pay_expense();
}
//---------- End of function FirmWork::pay_expense --------//


//------ Begin of function FirmWork::process_independent_town_worker -----//
//
// Process workers from independent towns.
//
// When workers work for a foreign firm, the overall resistance of
// the worker's town towards that nation decreases.
//
void FirmWork::process_independent_town_worker()
{
	#define RESISTANCE_DECREASE_PER_WORKER	 float(0.2)		// resistance decrease per month every 15 days

	Town* townPtr;

	for( int i=0 ; i<worker_count ; i++ )
	{
		err_when( !worker_array[i].town_recno );

		townPtr = town_array[ worker_array[i].town_recno ];

		if( townPtr->nation_recno==0 )		// if it's an independent town
		{
			townPtr->change_resistance( nation_recno, -RESISTANCE_DECREASE_PER_WORKER );
		}
	}
}
//------- End of function FirmWork::process_independent_town_worker ------//


//---------- Begin of function FirmWork::recruit_worker --------//
//
void FirmWork::recruit_worker()
{
	if( worker_count >= needed_worker_count )
		return;

	err_when( worker_count > needed_worker_count );
	err_when( needed_worker_count > MAX_WORKER );

	//-------- recruit workers from towns --------//

	Town* townPtr;
	int	recruitRating, bestRating=0, bestTownRecno=0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_enable_array[i] != LINK_EE )
			continue;

		townPtr = town_array[linked_town_array[i]];

		err_when( townPtr->nation_recno != nation_recno && townPtr->nation_recno != 0 );		// only recruit from own towns

		if( townPtr->jobless_population==0 )
			continue;

		int townDistance = m.points_distance(townPtr->center_x, townPtr->center_y, center_x, center_y);

		//-------- calculate the recruit rating --------//

		recruitRating = 100 * townPtr->jobless_population / townPtr->population		// relative value
							 + townPtr->jobless_population * 2;									// absolute value

		if( recruitRating > bestRating )
		{
			bestRating 	  = recruitRating;
			bestTownRecno = townPtr->town_recno;
		}
	}

	if( !bestTownRecno )
		return;

	//------- recruit now ---------//

	worker_array[worker_count++].town_recno = bestTownRecno;

	town_array[bestTownRecno]->jobless_population--;

	calc_productivity();
}
//----------- End of function FirmWork::recruit_worker ---------//


//------ Begin of function FirmWork::set_needed_worker_count ------//
//
void FirmWork::set_needed_worker_count(int neededWorkerCount, char remoteAction)
{
	if( neededWorkerCount > MAX_WORKER )
		neededWorkerCount = MAX_WORKER;

	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ######//
		// packet structure : <firm recno> <new needed worker count>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_SET_WORKER_COUNT, 2*sizeof(short) );
		*shortPtr = firm_recno;
		shortPtr[1] = neededWorkerCount;
		// ##### end Gilbert 7/10 ######//
		return;
	}

	//--- if the new needed worker count is larger than the existing number of workers ---//

	if( worker_count > neededWorkerCount )
	{
		//------ resign local worker -------//

		for(int i=worker_count-1; worker_count > neededWorkerCount && i >= 0 ; i-- )
		{
			Town* townPtr = town_array[ worker_array[i].town_recno ];

			resign_worker(i+1);
		}
	}

	needed_worker_count = neededWorkerCount;

	calc_productivity();
}
//------- End of function FirmWork::set_needed_worker_count -------//


//------ Begin of function FirmWork::resign_all_worker ------//
//
void FirmWork::resign_all_worker()
{
	Town* townPtr;

	for( int i=0 ; i<worker_count ; i++ )
	{
		townPtr = town_array[ worker_array[i].town_recno ];

		townPtr->jobless_population++;

		err_when( townPtr->jobless_population > townPtr->population );
	}

	worker_count=0;
	selected_worker_id = 0;

	calc_productivity();
}
//------- End of function FirmWork::resign_all_worker -------//


//--------- Begin of function FirmWork::resign_worker ---------//
//
// Resign a worker from the firm.
//
void FirmWork::resign_worker(int workerId)
{
	err_when( !worker_array );    // this function shouldn't be called if this firm does not need worker

	err_when( workerId<1 || workerId>worker_count );

	//------- decrease worker no. and create an unit -----//

	Town *townPtr = town_array[ worker_array[workerId-1].town_recno ];

	townPtr->jobless_population++;

	//------- delete the record from the worker_array ------//

	err_when( worker_count > MAX_WORKER );
	err_when( selected_worker_id > worker_count );

	m.del_array_rec(worker_array, worker_count, sizeof(Worker), workerId);

	if( selected_worker_id > workerId || selected_worker_id == worker_count )
		selected_worker_id--;

	worker_count--;

	err_when( worker_count < 0 );
	err_when( selected_worker_id > worker_count );

	calc_productivity();
}
//----------- End of function FirmWork::resign_worker -----------//


// --------- begin of function FirmWork::kill_all_worker --------//
//
void FirmWork::kill_all_worker()
{
	for( int w = worker_count-1; w >= 0; --w )
	{
		//------- decrease population -----//

		if( !town_array.is_deleted(worker_array[w].town_recno) )
		{
			Town *townPtr = town_array[ worker_array[w].town_recno ];

			// ###### begin Gilbert 18/1 #######//
			// ----- kill town spy -------//
			int spySeq;
			if( townPtr->spy_count > 0 && (spySeq = m.random(townPtr->population)) < townPtr->spy_count )
			{
				int spyRecno = spy_array.find_town_spy(townPtr->town_recno, spySeq+1 );
				spy_array.del_spy(spyRecno);
			}
			// ###### end Gilbert 18/1 #######//

			townPtr->population--;
		}
	}

	worker_count = 0;
	selected_worker_id = 0;

	calc_productivity();
}
// --------- end of function FirmWork::kill_all_worker --------//


//------ Begin of function FirmWork::calc_productivity ------//
//
void FirmWork::calc_productivity()
{
	//---- calculate productivity -----//

	productivity = 100 * worker_count / MAX_WORKER;		// the maximum productivity is 100
}
//------- End of function FirmWork::calc_productivity -------//


//------ Begin of function FirmWork::available_workforce ------//
//
// Return the total workforce available to this firm.
//
int FirmWork::available_workforce()
{
	int availableWorkforce=0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		Town* townPtr = town_array[ linked_town_array[i] ];

		availableWorkforce += townPtr->jobless_population;
	}

	return availableWorkforce;
}
//------- End of function FirmWork::available_workforce -------//
