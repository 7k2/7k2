// Filename    : OU_WAGON.H
// Description : Header file of wagon unit

#ifndef __OU_WAGON_H
#define __OU_WAGON_H

#include <OUNIT.H>

//--------- Define constant ------------//

#define MAX_WAGON_POPULATION 10

//-------- Define class UnitWagon --------//

class UnitWagon : public Unit
{
public:
	int	population;
	short	spy_count;
	short	spy_recno[MAX_WAGON_POPULATION];

public:
	UnitWagon();
	~UnitWagon();

	void	init_derived();
	void	deinit();

	// --------- overloaded function ----------//

	void	change_nation(int newNationRecno );

	// --------- population functions ---------//

	void	set_race(int raceId)		{ race_id = raceId; }
	void	set_pop(int);		// also change hit point
	short mobilize_civilian( short spyRecno );

	// -------- spy function ---------//

	void	add_spy(short spyRecno);
	short	del_spy(short spyRecno=-1);			// return the first spy, and remove it from spy_array
	int	explore_for_player();

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitWagon*	cast_to_UnitWagon()	{ return this; }
};


#endif