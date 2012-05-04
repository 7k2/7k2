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

// Filename    : OEND_CON.H
// Description : header file of End condition

#ifndef __OEND_CON_H
#define __OEND_CON_H

// ------ define condition_id ----------//

enum
{
	// ------ kingdom category -------//

	E_DEFEAT_ALL_OTHER_KINGDOMS = 0,		// default winning condition
	E_DEFEAT_OWN_KINGDOM,				// default lose condition
	E_DEFEAT_KINGDOM,
	E_FORM_ALLY_WITH_KINGDOM,
	E_DEFEAT_ALL_FRYHTAN,
	E_DEFEAT_EXCEPT_ALLIANCE,

	// ------- population / economy category ------//

	E_REACH_POPULATION,
	E_REACH_FRYHTAN,
	E_REACH_UNIT_COUNT,
	E_REACH_ECONOMY,
	E_REACH_MILITARY,
	E_REACH_TOTAL,
	E_KILL_MONSTER_SCORE,
	E_TRAIN_10_GENERAL,

	// ------- unit category --------//

	E_KILL_UNIT,
	E_KEEP_UNIT_ALIVE,
	E_MEET_UNIT,

	// ------- town category ---------//

	E_CONQUER_ALL_TOWN,
	E_CONQUER_TOWN,

	// ------- firm category ---------//

	E_DESTROY_FIRM,

	// ------- site category ----------//

	E_GET_SCROLL,

	// ------- item category ---------//

	E_ACQUIRE_ITEM,

	// ------- misc category ---------//

	E_NATION_SURVIVE,

	// ------ technology category -----//

	E_ACQUIRE_TECH,

	// ------- firm category ---------//

	E_CONQUER_ALL_FIRM,
	E_CONQUER_FIRM,

	// ------ keep it last --------//

	END_CONDITION_COUNT
};


// ------ define struct EndCondition --------//

#pragma pack(1)
struct EndCondition
{
public:
	enum { CONDITION_STR_LEN=100 };
	int	condition_id;
	int	para1;
	int	para2;
	int	time_limit;
	char	player_only;			// this condition only applies to the player
	char	cond_str[CONDITION_STR_LEN+1];
	char	result_str[CONDITION_STR_LEN+1];

	long	para1_unique_id;		// unique id of unit whose unit_recno is para1
	union
	{
		char	para1_unit_killed;
		char	para1_nation_took;		// nation recno of kingdom took the site
	};

public:
	void	init( int condId, int playerOnly, int p1, int p2, int yearLimit, int loseCond );
	int	is_condition_true(int nationRecno);
//	char* condition_true_str(int nationRecno);
	int	update_condition_on_killed(long uniqueId);		// check unit killed
	int	update_condition_on_mobilize(int newUnitRecno, long uniqueId);		// check unit re-mobilized
	int	update_condition_site_taken(int siteRecno, int nationRecno );
	int	update_condition_item_acquired(short itemId, int nationRecno );
	int	is_unit_in_condition(long uniqueId);

	void	update_strings(int loseCond);

private:
	void	set_condition_str( char *, int loseCond );
	void	set_result_str( char *, int loseCond );
};
#pragma pack()

// ------- define enum of win_or_lose -------//

enum
{
	EW_WIN_ONLY = 1,
	EW_LOSE_ONLY = 2,
	EW_WIN_LOSE = 3,
};

// ------- define enum of para?_type -------//

enum
{
	EP_NONE = 0,
	EP_0_100,
	EP_0_1000,
	EP_0_10000,
	EP_0_100000,

	EP_UNIT_ID,
	EP_ITEM_ID,
	EP_TECH_ID,

	EP_NATION_RECNO,
	EP_UNIT_RECNO,
	EP_FRIEND_UNIT_RECNO,
	EP_ENEMY_UNIT_RECNO,
	EP_TOWN_RECNO,
	EP_FRIEND_TOWN_RECNO,
	EP_ENEMY_TOWN_RECNO,
	EP_FIRM_RECNO,
	EP_FRIEND_FIRM_RECNO,
	EP_ENEMY_FIRM_RECNO,
	EP_SITE_RECNO,
	// ###### begin Gilbert 22/2 #########//
   // EP_YEARS, 
	// ###### end Gilbert 22/2 #########//
};

//------ define enum for time_limit_type ------//

enum
{
	ET_NONE,
	ET_OPTIONAL,
	ET_COMPULSORY,
};

// ------- define struct EndConditionInfo ------//

struct EndConditionInfo
{
	int	condition_id;
	int	negative_condition;
	char	win_or_lose;		// 1=win only, 2=lose only, 3=both
	char	para1_type;
	char	para2_type;
	char	time_limit_type;	// 0=no time limit 1=optional time limit 2=compulsory time limit
	const char* eng_name;			// remain for reduce modify, use name() instead
	char*	name();

	static EndConditionInfo end_condition_info[END_CONDITION_COUNT];
};

//----------------------------------------------//

#endif
