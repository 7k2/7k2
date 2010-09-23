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
	long	day_to_event;

	int	last_event_id;
	int	last_event_nation;		// 0 = all
	int	last_event_para1;
	int	last_event_para2;

private:
	long	random_seed;

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

