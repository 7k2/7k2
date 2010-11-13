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

// filename    : OGAMMENU.H
// Description : temp class and constant for game menu

#ifndef __OGAMMENU_H
#define __OGAMMENU_H


#include <OGAMESET.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OHERORES.H>
#include <OITEMRES.H>


// ------ define button of coordinate ------ //

#define BUTTON1_X1	46
#define BUTTON1_Y1	526
#define BUTTON1_X2	185
#define BUTTON1_Y2	556
#define BUTTON2_X1	188
#define BUTTON2_Y1	BUTTON1_Y1
#define BUTTON2_X2   328
#define BUTTON2_Y2	BUTTON1_Y2
#define BUTTON3_X1	330
#define BUTTON3_Y1	BUTTON1_Y1 
#define BUTTON3_X2	470
#define BUTTON3_Y2	BUTTON1_Y2
#define BUTTON4_X1	473
#define BUTTON4_Y1	BUTTON1_Y1
#define BUTTON4_X2	612
#define BUTTON4_Y2	BUTTON1_Y2
#define BUTTON5_X1	614
#define BUTTON5_Y1	BUTTON1_Y1
#define BUTTON5_X2	754
#define BUTTON5_Y2	BUTTON1_Y2

#define BUTTON6_X1	42
#define BUTTON6_Y1	566
#define BUTTON6_X2	183
#define BUTTON6_Y2	599
#define BUTTON7_X1	185
#define BUTTON7_Y1	BUTTON6_Y1
#define BUTTON7_X2	327
#define BUTTON7_Y2	BUTTON6_Y2
#define BUTTON8_X1	329
#define BUTTON8_Y1	BUTTON6_Y1
#define BUTTON8_X2	471
#define BUTTON8_Y2	BUTTON6_Y2
#define BUTTON9_X1	473
#define BUTTON9_Y1	BUTTON6_Y1
#define BUTTON9_X2	615
#define BUTTON9_Y2	BUTTON6_Y2
#define BUTTONA_X1	617
#define BUTTONA_Y1	BUTTON6_Y1
#define BUTTONA_X2	759
#define BUTTONA_Y2	BUTTON6_Y2


// define bit flag for refreshFlag

#define SGOPTION_PAGE           0x40000000
#define SGOPTION_RACE           0x00000001
#define SGOPTION_COLOR          0x00000002
#define SGOPTION_AI_NATION      0x00000004
#define SGOPTION_DIFFICULTY     0x00000008

#define SGOPTION_TERRAIN        0x00000010
#define SGOPTION_LAND_MASS      0x00000020
#define SGOPTION_NAME_FIELD     0x00000040
#define SGOPTION_MAP_ID         0x00000080

#define SGOPTION_EXPLORED       0x00000100
#define SGOPTION_FOG            0x00000200
#define SGOPTION_TREASURE       0x00000400
#define SGOPTION_AI_TREASURE    0x00000800

#define SGOPTION_AI_AGGRESSIVE  0x00001000
#define SGOPTION_SPY_METHOD     0x00002000
#define SGOPTION_RANDOM_STARTUP 0x00004000
#define SGOPTION_BUILDING_SIZE  0x00008000

#define SGOPTION_RAW            0x00010000
#define SGOPTION_NEAR_RAW       0x00020000
#define SGOPTION_START_TOWN     0x00040000
#define SGOPTION_TOWN_STRENGTH  0x00080000

#define SGOPTION_TOWN_EMERGE    0x00100000
#define SGOPTION_KINGDOM_EMERGE 0x00200000
#define SGOPTION_RANDOM_EVENT   0x00400000
// ----- only for campaign menu -------//
#define SGOPTION_CAMPAIGN       0x00800000
// -----------------------------------//
#define SGOPTION_CLEAR_ENEMY    0x01000000
#define SGOPTION_CLEAR_MONSTER  0x02000000
#define SGOPTION_ENOUGH_PEOPLE  0x04000000
#define SGOPTION_ENOUGH_INCOME  0x08000000
#define SGOPTION_ENOUGH_SCORE   0x10000000
#define SGOPTION_TIME_LIMIT     0x20000000
#define SGOPTION_ALL            0x7fffffff

#define SGOPTION_ALL_OPTIONS    (SGOPTION_ALL & ~SGOPTION_PAGE & ~SGOPTION_MAP_ID)



// ------- constant for switch option button ----------//

#define OPTION_SWITCH_X1        708
#define OPTION_SWITCH_Y1        131
#define OPTION_SWITCH_X2        749
#define OPTION_SWITCH_Y2        163
#define OPTION_SWITCH_Y_SPACING  66

#define LSCROLL_X1               39
#define LSCROLL_Y1               69
#define LSCROLL_X2               94
#define LSCROLL_Y2              442
#define RSCROLL_X1              704
#define RSCROLL_Y1       LSCROLL_Y1
#define RSCROLL_X2              759
#define RSCROLL_Y2       LSCROLL_Y2
#define SCROLL_SHEET_X1  (LSCROLL_X2+1)
#define SCROLL_SHEET_Y1          81
#define SCROLL_SHEET_X2  (RSCROLL_X1-1)
#define SCROLL_SHEET_Y2         431


// -------- define temp class for loading resource -------//

class TempGameSet
{
private:
	char inited_flag;

public:
	TempGameSet(int s)
	{
		if( !(inited_flag = game_set.set_opened_flag) )
		{
			game_set.open_set(1);
		}
	}

	~TempGameSet()
	{
		if( !inited_flag )
			game_set.close_set();
	}
};

class TempUnitRes
{
private:
	char inited_flag;

public:
	TempUnitRes()
	{
		if( !(inited_flag = unit_res.init_flag) )
			unit_res.init();
	}

	~TempUnitRes()
	{
		if( !inited_flag )
			unit_res.deinit();
	}
};


class TempRaceRes
{
private:
	char inited_flag;

public:
	TempRaceRes()
	{
		if( !(inited_flag = race_res.init_flag) )
			race_res.init();
	}

	~TempRaceRes()
	{
		if( !inited_flag )
			race_res.deinit();
	}
};


class TempMonsterRes
{
private:
	char inited_flag;

public:
	TempMonsterRes()
	{
		if( !(inited_flag = monster_res.init_flag) )
			monster_res.init();
	}

	~TempMonsterRes()
	{
		if( !inited_flag )
			monster_res.deinit();
	}
};


class TempHeroRes
{
private:
	char inited_flag;

public:
	TempHeroRes()
	{
		if( !(inited_flag = hero_res.init_flag) )
			hero_res.init();
	}

	~TempHeroRes()
	{
		if( !inited_flag )
			hero_res.deinit();
	}
};


class TempItemRes
{
private:
	char inited_flag;

public:
	TempItemRes()
	{
		if( !(inited_flag = item_res.init_flag) )
			item_res.init();
	}

	~TempItemRes()
	{
		if( !inited_flag )
			item_res.deinit();
	}
};


#endif
