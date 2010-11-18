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

// Filename    : OT_ENDC.H
// Description : header file of text resource end condition

#ifndef __OT_ENDC_H
#define __OT_ENDC_H

#include <otxtres.h>

struct EndCondition;

class TextResEndCondition : public TextResource
{
public:
	void	init();

	char*	str_end_condition_name( int conditionId );

	// used in Game::game_end
	char* str_all_condition_format_str();
	char* str_you_win_str( int nationRaceId, char *resultFormatStr );
	char* str_other_win_str( char* winNationName, char *resultFormatStr );
	char* str_lose_surrender( char* targetNationName, char *dateStr );
	char* str_lose_time_up();
	char* str_you_lose_str( int nationRaceId, char *resultFormatStr );
	char* str_you_lose_defeated( int nationRaceId );
	char* str_you_retire(char *dateStr );

	char* str_duration_of_kingdom();
	char* str_game_time();
	char* str_final_population();
	char* str_final_treasure();
	char* str_kill_soldiers();
	char* str_lose_soldiers(char* nationTitle);
	char* str_kill_monsters();
	char* str_lose_monsters(char* nationTitle);
	char* str_kill_weapons();
	char* str_lose_weapons(char* nationTitle);
	char* str_destroy_buildings();
	char* str_lose_buildings(char* nationTitle);
	char* str_kill_civilians();
	char* str_lose_civilians(char* nationTitle);
};

extern TextResEndCondition text_end_condition;
#endif