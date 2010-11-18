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

// Filename    : OT_CAMP.H
// Description : header file of campaign text resource


#ifndef __OT_CAMP_H
#define __OT_CAMP_H


#include <otxtres.h>


class TextResCampaign : public TextResource
{
public:
	void	init();

	// select campaign menu

	char* str_campaign_name(int campaignId);
	char* str_intro_1();
	char* str_intro_2();
	char* str_intro_3();
	char* str_intro_4();
	char* str_intro_5();
	char* str_intro_6();
	char* str_intro_7();
	char* str_intro_8();

	char* str_monster_fall_1();
	char* str_monster_fall_2();
	char* str_monster_fall_3();
	char* str_monster_fall_4();

	char* str_victory_1();
	char* str_victory_2();
	char* str_victory_3();
	char* str_victory_4();
	char* str_victory_5();
	char* str_victory_6();
	char* str_victory_7();
	char* str_victory_8();
	char* str_victory_9();

	char* str_campaign_score(int campaignScore);
	char* str_event_count(int eventId, int totalEventRunCount);
	char* str_stage_count(int stageId, int totalStageRunCount);
	char* str_score();
	char* str_difficulty_rating();
	char* str_click_to_continue();
	char* str_bonus(int bonus);
	char* str_bonus_formula(int maxYear);
	char* str_click_to_read_msg();

	// select royal menu
	char* str_royal_units();
	char* str_select_royal_help_1();
	char* str_select_royal_help_2();	// reserved
	char* str_done();
	char* str_restore();
	char* str_random_select();
	char* str_deselect_all();
	char* str_delete_royal();
	char* str_human_force();
	char* str_fryhtan_force();
	char* str_ask_fryhtan();
	char* str_max_royal_on_field(int maxRoyal);
	char* str_max_royal_in_store(int maxRoyal);
	char* str_confirm_drop_item();
	char* str_confirm_end_royal();

	// strategic map
	char* str_select_attack_state();
	char* str_state_of_cnation( char* cNationName );
	char* str_independent_state();
	char* str_click_to_attack();
	char* str_cannot_attack();

	// string specific to CampaignEastWest
	char* str_eastern_empire();
	char* str_western_empire();
};

extern TextResCampaign text_campaign;

#endif
