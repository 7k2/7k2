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

// Filename    : OU_WAGON.H
// Description : Header file of wagon unit

#ifndef __OU_WAGON_H
#define __OU_WAGON_H

#include <ounit.h>

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