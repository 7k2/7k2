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

// Filename    : OT_UNIT.H
// Description : text resource on unit


#include <ot_unit.h>
#include <all.h>
#include <ounit.h>
#include <onationb.h>

void TextResUnit::init()
{
	TextResource::init(DIR_RES"T_UNIT.RES");
}

// title for human and fryhtan king, general and soldier
// text id 1
// [HK_N]King %1s
// text id 2
// [HG_N]General %1s
// text id 3
// [HS_N]Soldier %1s
// text id 4
// [FK_N]All High %1s
// text id 5
// [FG_N]Ordo %1s
// text id 6
// [FS_N]Croul %1s
const char* TextResUnit::str_unit_titled(int raceId, const char *baseName, int rankId )
{
	if( raceId == 0 )
		return baseName;		// no rank

	if( raceId >= 0 )
	{
		switch( rankId )
		{
		case RANK_SOLDIER:
			return process( get_format_str( 3, "HS_N" ), baseName );
		case RANK_GENERAL:
			return process( get_format_str( 2, "HG_N" ), baseName );
		case RANK_KING:
			return process( get_format_str( 1, "HK_N" ), baseName );
		default:
			return baseName;
		}
	}
	else 
	{
		switch( rankId )
		{
		case RANK_SOLDIER:
			return process( get_format_str( 6, "FS_N" ), baseName );
		case RANK_GENERAL:
			return process( get_format_str( 5, "FG_N" ), baseName );
		case RANK_KING:
			return process( get_format_str( 4, "FK_N" ), baseName );
		default:
			return baseName;
		}
	}
}

// title only
// text id 7
// [HK__]King
// text id 8
// [HG__]General
// text id 9
// [HS__]Soldier
// text id 10
// [FK__]All High
// text id 11
// [FG__]Ordo
// text id 12
// [FS__]Croul
const char* TextResUnit::str_title(int raceId, int rankId )
{
	if( raceId >= 0 )
	{
		switch( rankId )
		{
		case RANK_SOLDIER:
			return get_format_str( 9, "HS__" );
		case RANK_GENERAL:
			return get_format_str( 8, "HG__" );
		case RANK_KING:
			return get_format_str( 7, "HK__" );
		}
	}
	else
	{
		switch( rankId )
		{
		case RANK_SOLDIER:
			return get_format_str( 12, "FS__" );
		case RANK_GENERAL:
			return get_format_str( 11, "FG__" );
		case RANK_KING:
			return get_format_str( 10, "FK__" );
		}
	}

	return "";
}

// text id 13
// [RG_N]Rebel Leader %1s
// text id 14
// [RS_N]Rebel %1s
const char* TextResUnit::str_rebel_unit_titled(int raceId, const char *baseName, int rankId )
{
	switch( rankId )
	{
	case RANK_SOLDIER:
		return process( get_format_str( 13, "RG_N" ), baseName );
	case RANK_GENERAL:
		return process( get_format_str( 14, "RS_N" ), baseName );
	default:
		return baseName;
	}
}

// text id 15
// [RG__]Rebel Leader
// text id 16
// [RS__]Rebel
const char* TextResUnit::str_rebel_title(int raceId, int rankId )
{
	switch( rankId )
	{
	case RANK_SOLDIER:
		return get_format_str( 15, "RG__" );
	case RANK_GENERAL:
		return get_format_str( 16, "RS__" );
	}

	return "";
}

// text id 17
// [HN_N]%1s's Kingdom
// text id 18
// [FN_N]%1s's Kwyzan
char* TextResUnit::str_unit_nation(int raceId, const char *kingBaseName )
{
	if( raceId >= 0 )
		return process( get_format_str( 17, "HN_N" ), kingBaseName );
	else
		return process( get_format_str( 18, "FN_N" ), kingBaseName );
}

// text id 19
// [HN__]Kingdom
// text id 20
// [FN__]Kwyzan
char* TextResUnit::str_nation(int raceId)
{
	if( raceId >= 0 )
		return get_format_str( 19, "HN__" );
	else
		return get_format_str( 20, "FN__" );
}

// text id 21
// [COMB]Combat Level
char* TextResUnit::str_combat_level()
{
	return get_format_str( 21, "COMB" );
}

// text id 22
// [LEAD]Leadership
char* TextResUnit::str_leadership()
{
	return get_format_str( 22, "LEAD" );
}

// text id 23
// [HITP]Hit Points
char* TextResUnit::str_hit_points()
{
	return get_format_str( 23, "HITP" );
}

// text id 24
// [LOYL]Loyalty
char* TextResUnit::str_loyalty()
{
	return get_format_str( 24, "LOYL" );
}

// text id 25 (spy skill)
// [SPYS]Spying
char* TextResUnit::str_spy_skill()
{
	return get_format_str( 25, "SPYS" );
}

// text id 26
// [HERO]Hero
char* TextResUnit::str_hero()
{
	return get_format_str( 26, "HERO" );
}

// text id 27
// [RHER]%1s Hero
char* TextResUnit::str_race_hero( char *raceAdj )
{
	return process( get_format_str( 27, "RHER" ), raceAdj );
}

// text id 28
// [CIVT]Civilian %1s
char* TextResUnit::str_civilian_titled(const char *baseName)
{
	return process( get_format_str( 28, "CIVT" ), baseName );
}


// text id 29 - 40 reserved

// text id 41
// [CARM]Manual Trade
char* TextResUnit::str_caravan_manual()
{
	return get_format_str( 41, "CARM" );
}

// text id 42
// [CARA]Auto Trade
char* TextResUnit::str_caravan_auto()
{
	return get_format_str( 42, "CARA" );
}

// text id 43 - 54
// [GOD1] - [GODC]
char* TextResUnit::str_god_desc( int godId )
{
	char checkMark[] = "GOD0";
	if( godId <= 9 )
		checkMark[3] = '0' + godId;
	else
		checkMark[3] = 'A' + godId - 10;
	return get_format_str( 42 + godId, checkMark );
}


// text id 55 - 59
// [REL0]Enemy
// [REL1]Unfriendly
// [REL2]
// [REL3]Friendly
// [REL4]Allied
const char* TextResUnit::str_unit_relation( int nationStatus )
{
	switch( nationStatus )
	{
	case RELATION_HOSTILE:
		return get_format_str( 55, "REL0" );
	case RELATION_TENSE:
		return get_format_str( 56, "REL1" );
	case RELATION_NEUTRAL:
		return get_format_str( 57, "REL2" );
	case RELATION_FRIENDLY:
		return get_format_str( 58, "REL3" );
	case RELATION_ALLIANCE:
		return get_format_str( 59, "REL4" );
	default:
		return "";
	};
}

// text id 60
// [RDEF]%1s Defending Townsman
char* TextResUnit::str_race_defender(int raceId, char* raceAdj)
{
	return process( get_format_str( 60, "RDEF" ),
		raceAdj );
}

// text id 61
// [RREB]%1s Rebel
// text id 62
// [RRBL]%1s Rebel Leader
char* TextResUnit::str_race_rebel_title(char *raceAdj, int rankId )
{
	if( rankId == RANK_SOLDIER )
		return process( get_format_str( 61, "RREB" ), raceAdj );
	else
		return process( get_format_str( 62, "RRBL" ), raceAdj );
}

// text id 63
// [RWAG]%1s Wagon %2z
char* TextResUnit::str_race_wagon(char* raceAdj, int nameId)
{
	return process( get_format_str( 63, "RWAG" ),
		raceAdj, nameId );
}

// text id 64, 65
// [RSOL]%1s Soldier
// [RCRU]%1s Croul
char* TextResUnit::str_race_soldier(int raceId, char *raceAdj )
{
	if( raceId >= 0 )
		return process( get_format_str( 64, "RSOL" ),
			raceAdj );
	else
		return process( get_format_str( 65, "RCRU" ),
			raceAdj );
}

// text id 66
// [RCIV]%1s Civilian
char* TextResUnit::str_race_civilian(int raceId, char *raceAdj )
{
	return process( get_format_str( 66, "RCIV" ),
		raceAdj );
}


// text id 66 - 69 reserved

// text id 70
// [<relation><unit description>UNRD]%1s %2s
const char* TextResUnit::str_unit_desc(const char* relationStr, const char* descStr)
{
	if( relationStr && relationStr[0] )
		return process( get_format_str( 70, "UNRD"),
			relationStr, descStr );
	else
		return descStr;
}

// text id 71
// [<king name><unit name>KUNM]%1s's %2s
const char* TextResUnit::str_king_s_unit(const char* kingName, const char* unitName)
{
	return process( get_format_str( 71, "KUNM" ),
		kingName, unitName );
}


// text id 72
// [BLDI]Please select a location to build the %1s.
char* TextResUnit::str_select_build(char *firmName)
{
	return process( get_format_str( 72, "BLDI" ),
		firmName );
}

// text id 73
// [(cost $? + ? life points)BLDC]cost %1,2d%2?{ + |}%2z%2?{ life points|}
char* TextResUnit::str_build_cost( int buildCost, int buildLive )
{
	return process( get_format_str( 73, "BLDC" ),
		buildCost, buildLive );
}

// text id 74
// [SETT]Please select a location to settle.
char* TextResUnit::str_select_settle()
{
	return get_format_str( 74, "SETT" );
}

// text id 75
// [SPYC]Spy Cloak
char* TextResUnit::str_spy_cloak()
{
	return get_format_str( 75, "SPYC" );
}

// text id 76
// [NOCC]Enemies Nearby !! Cannot change cloak.
char* TextResUnit::str_no_change_cloak()
{
	return get_format_str( 76, "NOCC" );
}

// text id 77
// [IDRP]Right click to drop
char* TextResUnit::str_item_drop()
{
	return get_format_str( 77, "IDRP" );
}

// text id 78
// [IUSE]use
char* TextResUnit::str_item_use()
{
	return get_format_str( 78, "IUSE" );
}

// text id 79
// [I_ON]on
char* TextResUnit::str_item_on()
{
	return get_format_str( 79, "I_ON" );
}

// text id 80
// [IOFF]off
char* TextResUnit::str_item_off()
{
	return get_format_str( 80, "IOFF" );
}


// text id 81
// [RTRI]Automatically Retreat this (Left-click) or all your Generals (Right-click) to Home Fort when hit point is below :
char* TextResUnit::str_auto_retreat_inst()
{
	return get_format_str( 81, "RTRI" );
}

// text id 82
// [DISA]DISABLE
char* TextResUnit::str_disable_auto()
{
	return get_format_str( 82, "DISA" );
}

// text id 83
// [HIRH]%1s will take your color for %2,2d. Hire him now?
char* TextResUnit::str_ask_hire( const char* name, int price )
{
	return process( get_format_str( 83, "HIRH" ),
		name, price );
}

// text id 84
// [HIF1]But you do not have enough money now.
char* TextResUnit::str_fail_hire_cash()
{
	return get_format_str( 84, "HIF1" );
}

// text id 85
// [HIF2]But he has been hired by other nation.
char* TextResUnit::str_fail_hire_late()
{
	return get_format_str( 85, "HIF2" );
}

// text id 86
// [HIF3]Hire failed.
char* TextResUnit::str_fail_hire()
{
	return get_format_str( 86, "HIF3" );
}

// text id 87
// [ITUU]Left click on target unit
char* TextResUnit::str_use_item_on_unit()
{
	return get_format_str( 87, "ITUU" );
}

// text id 88
// [ITUT]Left click on target town
char* TextResUnit::str_use_item_on_town()
{
	return get_format_str( 88, "ITUT" );
}

// text id 89
// [ITUP]Left click target position
char* TextResUnit::str_use_item_on_pos()
{
	return get_format_str( 89, "ITUP" );
}

// text id 90
// [ITEM]Item
char* TextResUnit::str_item()
{
	return get_format_str( 90, "ITEM" );
}

// text id 91
// [LINC]Left click to increase
char* TextResUnit::str_left_inc()
{
	return get_format_str( 91, "LINC" );
}

// text id 92
// [RINC]Right click to increase
char* TextResUnit::str_right_dec()
{
	return get_format_str( 92, "RINC" );
}

// text id 93
// [e.g. King Jaguar 3 or Catapult III 6 NRNM]%4?{King |}%1s%2?{ |}%2r%3?{ |}%3z
char* TextResUnit::str_non_race_unit_name( const char* unitName, int version, int serial, int kingAnimal )
{
	if( version == 1 )
		version = 0;	// suppress if version 1

	return process( get_format_str( 93, "NRNM" ), 
		unitName, version, serial, kingAnimal );
}


// text id 94
// [<hero name>HERU]Hero %1s
char* TextResUnit::str_hero_titled( int raceId, const char *baseName )
{
	return process( get_format_str( 94, "HERU" ),
		baseName );

}

// text id 95 - 98
// [RKNG]%1s King
// [RGEN]%1s General
// [FKNG]%1s All High
// [FGEN]%1s Ordo
char* TextResUnit::str_race_title(int raceId, char *raceAdj, int rankId )
{
	if( raceId >= 0 )
	{
		switch( rankId )
		{
		case RANK_KING:
			return process( get_format_str( 95, "RKNG" ), raceAdj );
		case RANK_GENERAL:
			return process( get_format_str( 96, "RGEN" ), raceAdj );
		default:
			return process( get_format_str( 64, "RSOL" ), raceAdj );
		}
	}
	else
	{
		switch( rankId )
		{
		case RANK_KING:
			return process( get_format_str( 97, "FKNG" ), raceAdj );
		case RANK_GENERAL:
			return process( get_format_str( 98, "FGEN" ), raceAdj );
		default:
			return process( get_format_str( 65, "RCRU" ), raceAdj );
		}
	}
}
