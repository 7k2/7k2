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

//Filename    : ONATION2.H
//Description : Header for derived Nation classes

#ifndef __ONATION2_H
#define __ONATION2_H

#ifndef __ONATION_H
#include <ONATION.H>
#endif

//------ Define dervied nation class id.  --------//

enum { NATION_HUMAN = 1,
		 NATION_MONSTER,					// Fryhtan nations in standalone games
		 NATION_EAST_WEST_HUMAN1,		// The human nation in the EastWest campaign
};

//------ Define class NatoinHuman -------//

class NationHuman : public Nation
{
public:
	virtual void	process_ai();
	virtual int 	process_ai_main(int mainActionId);


};

//------ Define class NationMonster -------//

class NationMonster : public Nation
{
public:
	virtual void	process_ai();

protected:
	virtual int 	process_ai_main(int mainActionId);
	virtual void 	think_diplomacy();
			void 	think_attack_hostile_nation();
			void 	think_buy_independent_lair();
			int		think_acquire_live_points();
};

//-----------------------------------------//

#endif