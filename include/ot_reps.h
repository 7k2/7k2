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

// Filename    : OT_REPS.H
// Description : header file of Text resource reports


#ifndef __OT_REPS_H
#define __OT_REPS_H

#include <OTXTRES.H>

class TextResReports : public TextResource
{
public:
	void	init();

	char* str_population();
	char* str_military();
	char* str_economy();
	char* str_reputation();
	char* str_fryhtan_battling();

	char* str_population_score();
	char* str_military_score();
	char* str_economy_score();
	char* str_reputation_score();
	char* str_fryhtan_battling_score();
	char* str_total_score();
	char* str_final_score();
	char* str_difficulty_rating();
	char* str_scenario_bonus();
	char* str_cheated();

	char* str_duration_yd( int years, int days );
	char* str_duration_ym( int years, int months );
	char* str_duration_hm( int hours, int minutes );

	// report names

	char* str_report_mode(int viewMode);

	// kingdom report
	char* str_nat_kingdom();
	char* str_nat_reputation();
	char* str_nat_status();
	char* str_nat_allow_attack();
	char* str_nat_trade_treaty();
	char* str_nat_trade_amount();
	char* str_nat_sub_mode(int subMode);
	char* str_no_yes(char flag);
	char* str_disconnect();

	// kingdom report, information
	char* str_king_race(char raceId);
	char* str_your_food();
	char* str_your_treasure();
	char* str_your_live_points();
	char* str_your_peace_time();

	char* str_enemy_import_from(char* nationName);
	char* str_enemy_export_to(char* nationName);
	char* str_enemy_peace_time(char* nationName);
	char* str_enemy_status_duration(char* statusStr);
	char* str_enemy_allow_attack(char* nationName);
	char* str_ally_cash(char* nationName);
	char* str_ally_food(char* nationName);
	char* str_enemy_relation(char *nationName);

	char* str_send_from();
	char* str_send_to();
	char* str_on_date(char* dateStr);

	char* str_chat_inst();
	char* str_chat_recipient(int);
	char* str_hide_nation_detail();

	// town report
	char* str_town();
	char* str_citizens();
	char* str_peasants();
	char* str_nationality();
	char* str_structure();
	char* str_unit_cost();
	char* str_no_of_structures();
	char* str_yearly_expense();
	char* str_yearly_income();
	char* str_total_towns(int);
	char* str_total_citizens(int);
	char* str_total_peasant(int);
	char* str_total_other_units(int);
	char* str_total_population(int);
	char* str_total();

	// economy report
	char* str_income_item();
	char* str_expense_item();
	char* str_total_yearly_income();
	char* str_total_yearly_expense();
	char* str_yearly_balance();
	char* str_income_desc(int incomeType, int isMonster );
	char* str_expense_desc(int expenseType, int isMonster ); 

	// trade report
	char* str_trade_stop(int);
	char* str_goods_carried();
	char* str_total_caravan(int count);

	// military report
	char* str_mil_commander();
	char* str_mil_commanded_soldiers();
	char* str_mil_status();
	char* str_mil_unit_type();
	char* str_mil_no_of_units();
	char* str_total_commander(int);
	char* str_total_soldier_under(int);
	char* str_total_soldier(int);
	char* str_total_unit(int);
	char* str_mil_unit_status(int unitMode );

	char* str_technology();
	char* str_tech_present_version();
	char* str_tech_researching_version();
	char* str_tech_research_progress();
	char* str_tech_acq_scroll(int);
	char* str_tech_greater_being();
	char* str_tech_espionage_();

	char* str_spy_name();
	char* str_spy_cloak();
	char* str_spy_location();
	char* str_skill();
	char* str_spy_action();
	char* str_total_spy(int);
	char* str_spy_action_mode( int spyActionMode );
	char* str_counter_spy_action_mode();

	// ranking report
	char* str_overall();
	char* str_score_type( int rankType );
	char* str_overall_score();
	char* str_goal_and();
	char* str_goal_or();
	char* str_goal_();
	char* str_goal_single_timed( char *endDateStr );
	char* str_goal_and_timed( char *endDateStr );
	char* str_goal_or_timed( char *endDateStr );
	char* str_lose_();
	char* str_total_play_time();
	char* str_campaign_difficulty();
	char* str_campaign_year_passed( char *startDateStr, int yearPassed );
};

extern TextResReports text_reports;

#endif