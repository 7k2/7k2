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

// Filename    : OT_FIRM.H
// Description : header file of Text resource, Firm


#ifndef __OT_FIRM_H
#define __OT_FIRM_H

#include <OTXTRES.H>

struct Item;

class TextResFirm : public TextResource
{
public:
	void	init();

	// firm base

	char* str_base_prayer_effect( int godId );
	char* str_pray_points();

	// firm camp

	char* str_patrol_state( int patrolState);
	char* str_bribe_chance( int chance ); // 0-100 to mean 0% to 100%
	char* str_bribe_amount( int bribeAmount);  // $nnnnn
	char* str_ask_bribe_unit( char *unitName );
	char* str_bribe_success();
	char* str_bribe_fail();
	char* str_bribe_execute();
	char* str_ask_assassin( char* title );
	char* str_assassin_chance( int chance );
	char* str_yes();
	char* str_no();
	char* str_assassin_success();
	char* str_assassin_escape();
	char* str_assassin_executed();
	char* str_assassin_fail();
	char* str_ask_steal_tech();
	char* str_steal_tech_chance( int chance);
	char* str_steal_tech_success( char* techName, int techLevel );
	char* str_steal_tech_fail();
	char* str_steal_tech_executed();
	char* str_steal_tech_no_yield();
	char* str_incident_choose();
	char* str_ask_incident();
	char* str_incident_chance( int chance );
	char* str_incident_success();
	char* str_incident_fail();
	char* str_incident_execute();
	char* str_spy_steal_info();
	char* str_no_commander();

	// factory
	char* str_factory_raw( int rawId);
	char* str_factory_ware( int rawId );
	char* str_monthly_production();
	char* str_factory_auto();
	char* str_factory_manual();
	char* str_factory_info(int rawId);

	// inn
	char* str_buy_item_help( Item *itemPtr );

	// market
	char* str_no_trade();
	char* str_product_name( int rawId );
	char* str_yearly_income( int incomeAmount );

	// mine
	char* str_unmined_raw();
	char* str_mined_raw();
	char* str_mine_info(int rawId);
	char* str_mine_no_raw();

	// offensive structure
	char* str_cast_magic_instruct( int magicId );
	char* str_cast_teleport_source();
	char* str_cast_cost_cash( int cost );
	char* str_teleport_cost_cash( int cost);

	// research 
	char* str_select_tech_class();
	char* str_tech_class_finished();
	char* str_no_research();
	char* str_research_progress();
	char* str_completion_percent();

	// war factory
	char* str_no_build_weapon();

	// magic tor
	char* str_cast_cost_live_points( int cost );
	char* str_teleport_cost_live_points( int cost);
	char* str_cast_energy();

	// lair
	char* str_lair_research(int livePoints);
	char* str_buy_lair( int cost, int enoughFlag );
	char* str_buy_lair_short_cash();

	// alchemy 
	char* str_alchemy_raw( int rawId );
	char* str_estimated_resource(int reservedQty);
	char* str_alchemy_idle();
	char* str_conversion( char* techName );

	// animal
	char* str_hire_animal_cost( int cash, int livePoints );

	// general firm
	char* str_firm_name( int firmBuildId, char *closeTownName, int instanceId );
	char* str_under_construction();

	// site, scroll of power
	char* str_scroll_if_1( char* raceName );
	char* str_scroll_if_2( char* raceName, char* firmBuildName, char* godName );
	// site, gold
	char* str_treasure_if_1();
	char* str_treasure_if_2();
	// site, scrol of weapon 
	char* str_blueprint_if_1();
	char* str_blueprint_if_2();

	// town
	char* str_town_of();
	char* str_race_town( char* raceName );
	char* str_race_slave_town( char* raceName );
	char* str_no_town_wall();
	char* str_town_population();
	char* str_town_worker();
	char* str_town_peasant();
	char* str_resistance();
	char* str_wall_repair();
	char* str_wall_building();
	char* str_slave_tribute( int amount );
	char* str_town_spy_skill();
	char* str_town_spy_mission();
	char* str_auto_tax_inst();
	char* str_auto_grant_inst();
	char* str_set_wall_crew();
	char* str_disable_auto();
	char* str_no_of_worker( int w );
	char* str_town_info_pop( int peasant, int worker );
	char* str_spy_inside( int spyCount );

	// town edit mode
	char* str_loyalty_to(char *nationName);
	char* str_resistance_to(char *nationName);
	char* str_left_inc();
	char* str_right_dec();
};

extern TextResFirm text_firm;

#endif