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

// Filename    : OT_ENDC.CPP
// Description : Text Resource end condition


#include <ot_endc.h>
#include <all.h>
#include <oend_con.h>
#include <ot_unit.h>


void TextResEndCondition::init()
{
	TextResource::init( DIR_RES"T_ENDC.RES" );
}

// text id 1 - 26 (27-32 reserved)
// [EN00] - [EN25] (26-31 reserved)
char*	TextResEndCondition::str_end_condition_name( int conditionId )
{
//	static char checkMark[] = "EN00";
//	err_when(conditionId < 0 || conditionId > 99 );
//	checkMark[2] = '0' + conditionId / 10;
//	checkMark[3] = '0' + conditionId % 10;

	err_when( conditionId < 0 || conditionId >= END_CONDITION_COUNT ); 
	return get_format_str( 1+conditionId, NULL );
}

// see OEND_CON.CPP

// for set_condition_str
// text id 33 - 70 (71-76 reserved)
// [EC00] - [EC25] (26-31 reserved)

// text id 77
// [ECTM]%1s in %2d %2p{year|years}

// for set_result_str
// text id 78 - 115 (116-122 reserved)
// [ER00] - [ER25] (26-31 reserved)


// text id 123
// [WINA]%1s has completed all tasks
char* TextResEndCondition::str_all_condition_format_str()
{
	return get_format_str( 123, "WINA" );
}

// text id 124
// [YOUR]Your %1s
char* TextResEndCondition::str_you_win_str( int nationRaceId, char *resultFormatStr )
{
	String tempStr;
	// to format Your Kingdom / Your Kwyzan
	tempStr = process( get_format_str( 124, "YOUR" ), text_unit.str_nation(nationRaceId) );

	return process( resultFormatStr, (char *)tempStr );
}

char* TextResEndCondition::str_other_win_str( char* winNationName, char *resultFormatStr )
{
	return process( resultFormatStr, winNationName );
}

// text id 125
// [YSUR]You Surrendered to %1s on %2s.
char* TextResEndCondition::str_lose_surrender(char* targetNationName, char* dateStr)
{
	return process( get_format_str( 125, "YSUR" ), targetNationName, dateStr);
}

// text id 126
// [NOTM]Your Kingdom has Failed to Achieve its Goal Within the Time Limit.
char* TextResEndCondition::str_lose_time_up()
{
	return get_format_str( 126, "NOTM" );
}


char* TextResEndCondition::str_you_lose_str( int nationRaceId, char *resultFormatStr )
{
	String tempStr;
	// to format Your Kingdom / Your Kwyzan
	tempStr = process( get_format_str( 124, "YOUR" ), text_unit.str_nation(nationRaceId) );

	return process( resultFormatStr, (char *)tempStr );
}

// text id 127
// [YDEF]Your Kingdom has Gone Down to Ignominious Defeat !
char* TextResEndCondition::str_you_lose_defeated(int nationRaceId )
{
	return get_format_str( 127, "YDEF" );
}

// text id 128
// [RETI]You Retired on %1s 
char* TextResEndCondition::str_you_retire(char* dateStr )
{
	return process( get_format_str( 128, "RETI" ), dateStr );
}

// text id 129
// [DURR]Duration of Your Rule
char* TextResEndCondition::str_duration_of_kingdom()
{
	return get_format_str( 129, "DURR" );
}

// text id 130
// [TGMT]Total Gaming Time
char* TextResEndCondition::str_game_time()
{
	return get_format_str( 130, "TGMT" );
}

// text id 131
// [FPOP]Final Population
char* TextResEndCondition::str_final_population()
{
	return get_format_str( 131, "FPOP" );
}

// text id 132
// [FCAS]Final Treasure
char* TextResEndCondition::str_final_treasure()
{
	return get_format_str( 132, "FCAS" );
}

// text id 133
// [KSOL]Enemy Soldiers Dispatched
char* TextResEndCondition::str_kill_soldiers()
{
	return get_format_str( 133, "KSOL" );
}

// text id 134
// [LSOL]%1s's Soldiers Martyred
char* TextResEndCondition::str_lose_soldiers(char* nationTitle)
{
	return process( get_format_str( 134, "LSOL" ), nationTitle);
}

// text id 135
// [KMON]Enemy Frythans Dispatched
char* TextResEndCondition::str_kill_monsters()
{
	return get_format_str( 135, "KMON" );
}

// text id 136
// [LMON]%1s's Frythans Martyred
char* TextResEndCondition::str_lose_monsters(char* nationTitle)
{
	return process( get_format_str( 136, "LMON" ), nationTitle);
}

// text id 137
// [KWEP]Enemy Weapons Destroyed
char* TextResEndCondition::str_kill_weapons()
{
	return get_format_str( 137, "KWEP" );
}

// text id 138
// [LWEP]%1s's Weapons Rendered Obsolete
char* TextResEndCondition::str_lose_weapons(char* nationTitle)
{
	return process( get_format_str( 138, "LWEP" ), nationTitle);
}

// text id 139 - 140
// reserved for ships

// text id 141
// [KBLD]Enemy Buildings Destroyed
char* TextResEndCondition::str_destroy_buildings()
{
	return get_format_str( 141, "KBLD" );
}

// text id 142
// [LBLD]%1s's Buildings Cleared
char* TextResEndCondition::str_lose_buildings(char* nationTitle)
{
	return process( get_format_str( 142, "LBLD" ), nationTitle);
}

// text id 143
// [KCIV]Enemy Civilians Collaterally Damaged
char* TextResEndCondition::str_kill_civilians()
{
	return get_format_str( 143, "KCIV" );
}

// text id 144
// [LCIV]%1s's Civilians Cruelly Murdered
char* TextResEndCondition::str_lose_civilians(char* nationTitle)
{
	return process( get_format_str( 144, "LCIV" ), nationTitle);
}
