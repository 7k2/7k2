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

//Filename    : OHERORES.H
//Description : Header file of object RaceRes

#ifndef __OHERORES_H
#define __OHERORES_H

#ifndef __ORESDB_H
#include <ORESDB.H>
#endif

//-------- Define constant ----------//

enum { HERO_COUNT_PER_RACE = 10 };		// The number of different heroes for each nationality

#define MAX_HERO (HERO_COUNT_PER_RACE*MAX_RACE)


//------------ Define struct HeroRec ---------------//

struct HeroRec
{
	enum { NAME_LEN=30, RACE_CODE_LEN=8, RACE_ID_LEN=2 };

	char name[NAME_LEN];
   char infantry_unit;
	char special_unit;
	char race_code[RACE_CODE_LEN];
	char race_id[RACE_ID_LEN];
};

//------------- Define struct HeroInfo --------------//

struct HeroInfo
{
public:
	enum  { NAME_LEN=30  };

	short hero_id;
	char 	name[NAME_LEN+1];
	char  infantry_unit;
	char  special_unit;

	char	race_id;

	char	for_hire;				// whether this hero can be hired or not. If it is 0, then this unit can only automatically go to serve a nation. But it can not be hired with money. It is randomly determined
	char 	appeared_flag;			// whether the hero has appeared in the game. It is 1 even the hero once appeared in the game but has been killed now
	char  historic_hero;			// 1-for heroes defined in HERO.DBF. 0-for heroes picked randomly
};

//----------- Define class HeroRes ---------------//

struct Item;

class HeroRes
{
public:
	char        init_flag;

	short       hero_count;
	HeroInfo*   hero_info_array;

	char        hero_appear_flag;		// if this is 1, hero will randomly appear from time to time. Its default is 1.

public:
	HeroRes();

	void        init();
	void        deinit();

	void 			second_init();

	void			next_day();

	void			disable_hero_appear()	{ hero_appear_flag=0; }
	void			enable_hero_appear()    { hero_appear_flag=1; }

	int 			create_hero(int xLoc, int yLoc, int nationRecno, int raceId,
								 int heroPower=0, int unitLoyalty=0, int forHire= -1, int historicHero= -1, int useReserve=0, int randomClass = 0);

	int 			create_powerful_unit(int xLoc, int yLoc, int nationRecno, int unitId,
								 int heroPower=0, int unitLoyalty=0, int historicHero= -1);

	int 			induce_hero_join_nation(int nationRecno, int inducePower);

	int 			hire_cost(int combatLevel, int maxCombatLevel, int skillLevel, int spySkill, Item* itemPtr);

	int         write_file(File*);
	int         read_file(File*);

	HeroInfo*   operator[](int heroId);      // pass heroId  as recno

private:
	void        load_hero_info();

	void 			think_generate_hero();
	int 			generate_hero();
	void			generate_fictional_hero();
};

extern HeroRes hero_res;

//----------------------------------------------------//

#endif
