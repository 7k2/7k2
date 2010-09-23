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
