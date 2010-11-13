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

//Filename    : OFIRMID.H
//Description : Identity no. of all firm types

#ifndef __OFIRMID_H
#define __OFIRMID_H

//--- Define the firm id no. according to the order in FIRM.DBF ---//

enum 
{ 
	FIRM_BASE=1,
	FIRM_FORT,
	FIRM_CAMP,
	FIRM_MINE,
	FIRM_FACTORY,
	FIRM_MARKET,
	FIRM_INN,
	FIRM_RESEARCH,
	FIRM_WAR_FACTORY,
	FIRM_SPY,
	FIRM_SPECIAL,
	FIRM_LAIR,
	FIRM_BEE,
	FIRM_TERMITE,
	FIRM_LISHORR,
	FIRM_WILDE_LISHORR,
	FIRM_ALCHEMY,
	FIRM_INCUBATOR,
	FIRM_ANIMAL,
	FIRM_FORTRESS,
	FIRM_MAGIC,
	FIRM_OFFENSIVE_BUILDING_1,
	FIRM_OFFENSIVE_BUILDING_2,
	FIRM_OFFENSIVE_BUILDING_3,
	FIRM_OFFENSIVE_BUILDING_4,
	FIRM_LAST,				// keep this the last
	MAX_FIRM_TYPE = FIRM_LAST-1
};

enum { FIRST_MONSTER_SPECIAL_FIRM = FIRM_BEE,
		 LAST_MONSTER_SPECIAL_FIRM  = FIRM_MAGIC };

//------------------------------------------//

#endif
