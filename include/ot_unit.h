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
// Description : header file of text resource on unit

#ifndef __OT_UNIT_H
#define __OT_UNIT_H

#include <otxtres.h>

class TextResUnit : public TextResource
{
public:
	void	init();

	// unit title
	char* str_unit_titled(int raceId, char *baseName, int rankId );
	char* str_title(int raceId, int rankId );
	char* str_rebel_unit_titled(int raceId, char *baseName, int rankId );
	char* str_rebel_title(int raceId, int rankId);
	char* str_civilian_titled(char *baseName);
	char* str_unit_nation(int raceId, char *kingBaseName );
	char* str_nation(int raceId);
	char* str_race_defender(int raceId, char* raceAdj);
	char* str_race_rebel_title( char *raceAdj, int rankId );
	char* str_race_wagon(char* raceAdj, int nameId);
	char* str_race_soldier(int raceId, char *raceAdj );
	char* str_race_civilian(int raceId, char *raceAdj ); 
	char* str_race_title(int raceId, char *raceAdj, int rankId );
	char* str_non_race_unit_name( char* unitName, int version, int serial, int kingAnimal );

	char* str_hero();
	char* str_race_hero( char *raceAdj );
	char* str_unit_relation( int nationStatus );
	char* str_unit_desc( char* relationStr, char* descStr );
	char* str_king_s_unit(char* kingName, char* unitName );
	char* str_hero_titled( int raceId, char *baseName );

	// attribute
	char* str_combat_level();
	char* str_leadership();
	char* str_hit_points();
	char* str_loyalty();
	char* str_spy_skill();

	// caravan
	char* str_caravan_manual();
	char* str_caravan_auto();

	// god
	char* str_god_desc( int godId );

	// build menu
	char* str_select_build(char *firmName);
	char* str_build_cost( int buildCost, int buildLive );
	char* str_select_settle();

	// spy menu
	char* str_spy_cloak();
	char* str_no_change_cloak();

	// item menu
	char* str_item_drop();
	char* str_item_use();
	char* str_item_on();
	char* str_item_off();
	char* str_use_item_on_unit();
	char* str_use_item_on_town();
	char* str_use_item_on_pos();
	char* str_item();

	// auto retreat menu
	char* str_auto_retreat_inst();
	char* str_disable_auto();

	// hire hero menu
	char* str_ask_hire( char* name, int price );
	char* str_fail_hire_cash();
	char* str_fail_hire_late();
	char* str_fail_hire();

	// editor
	char* str_left_inc();
	char* str_right_dec();
};

extern TextResUnit text_unit;

#endif