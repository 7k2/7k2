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

//Filename    : OF_BASE2.CPP
//Description : Firm Base - part 2, the AI functions

#include <ONATION.H>
#include <OTOWN.H>
#include <OF_BASE.H>

//--------- Begin of function FirmBase::process_ai ---------//

void FirmBase::process_ai()
{
	think_invoke_god();

	//---- call parent class process_ai() ------//

	FirmWork::process_ai();
}
//----------- End of function FirmBase::process_ai -----------//


//--------- Begin of function FirmBase::think_invoke_god ---------//

void FirmBase::think_invoke_god()
{
	if( pray_points < MAX_PRAY_POINTS || !can_invoke() )
		return;

	invoke_god();
}
//----------- End of function FirmBase::think_invoke_god -----------//


//------- Begin of function FirmBase::think_adjust_workforce -------//
//
// Think about adjusting the workforce by increasing or decreasing
// needed_worker_count.
//
int FirmBase::think_adjust_workforce()
{
	Nation* nationPtr = nation_array[nation_recno];

	//---- if we have enough food -----//

	if( nationPtr->ai_has_enough_food() )
	{
		if( needed_worker_count < MAX_WORKER )
		{
			set_needed_worker_count( needed_worker_count+1, COMMAND_AI );
			return 1;
		}
	}

	//---- if we are running out of food -----//

	else
	{
		if( needed_worker_count > MAX_WORKER/2 )
		{
			set_needed_worker_count( needed_worker_count-1, COMMAND_AI );
			return 1;
		}
	}

	return 0;
}
//--------- End of function FirmBase::think_adjust_workforce -------//




