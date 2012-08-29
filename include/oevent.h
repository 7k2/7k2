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

// Filename    : OEVENT.H
// Description : header file of Random event


#ifndef __OEVENT_H
#define __OEVENT_H


// ------- define constant ---------//

enum 
{
	RANDOM_EVENT_FINISH_TECH = 1,
	RANDOM_EVENT_NEW_TECH,
	RANDOM_EVENT_FOOD_DESTROYED,
	RANDOM_EVENT_GENERAL_DIE,
	RANDOM_EVENT_UNIT_DIE,
	RANDOM_EVENT_SOLDIER_DIE,
	RANDOM_EVENT_UNIT_JOIN,
	RANDOM_EVENT_WAGON_JOIN,
	RANDOM_EVENT_ITEM_DISCOVER,
	RANDOM_EVENT_TOWN_PEOPLE_KILLED,
	RANDOM_EVENT_FACTORY_DESTROYED,
	RANDOM_EVENT_MINE_DESTROYED,
	RANDOM_EVENT_INN_DESTROYED,

	RANDOM_EVENT_LAST,
	RANDOM_EVENT_COUNT = RANDOM_EVENT_LAST - 1
};


// -------- define class RandomEvent ---------//

class File;

class RandomEvent
{
public:
	int	day_to_event;

	int	last_event_id;
	int	last_event_nation;		// 0 = all
	int	last_event_para1;
	int	last_event_para2;

private:
	int	random_seed;

public:
	void	init(int randomSeed);
	
	// -------- process function ------- //

	void	next_day();
	int	execute_event();		// then set last_event_id, last_event_para1, last_event_para2

	// --------- save game function -------//

	int	write_file( File * );
	int	read_file( File *);

private:
	int	random(int);
	int	create_boundary_unit(int unitId, int nationRecno );
};

extern RandomEvent random_event;

#endif

