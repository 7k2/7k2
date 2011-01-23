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

// Filename    : OT_REPS.CPP
// description : Text resource reports

#include <ot_reps.h>
#include <all.h>
#include <osys.h>
#include <all.h>
#include <oinfo.h>
#include <ounit.h>
#include <ot_unit.h>
#include <onationb.h>
#include <ospy.h>


void TextResReports::init()
{
	TextResource::init( DIR_RES"T_REPS.RES" );
}


// text id 1
// [POPU]Population
char* TextResReports::str_population()
{
	return get_format_str( 1, "POPU" );
}

// text id 2
// [MILI]Military
char* TextResReports::str_military()
{
	return get_format_str( 2, "MILI" );
}

// text id 3
// [ECON]Economy
char* TextResReports::str_economy()
{
	return get_format_str( 3, "ECON" );
}

// text id 4
// [REPU]Reputation
char* TextResReports::str_reputation()
{
	return get_format_str( 4, "REPU" );
}

// text id 5
// [FRBA]Fryhtan Battling
char* TextResReports::str_fryhtan_battling()
{
	return get_format_str( 5, "FRBA" );
}

// text id 6
// [POPS]Population Score
char* TextResReports::str_population_score()
{
	return get_format_str( 6, "POPS" );
}




// text id 7
// [MILS]Military Score
char* TextResReports::str_military_score()
{
	return get_format_str( 7, "MILS" );
}

// text id 8
// [ECOS]Economic Score
char* TextResReports::str_economy_score()
{
	return get_format_str( 8, "ECOS" );
}

// text id 9
// [REPS]Reputation Score
char* TextResReports::str_reputation_score()
{
	return get_format_str( 9, "REPS" );
}

// text id 10
// [FRBS]Fryhtan Battling Score
char* TextResReports::str_fryhtan_battling_score()
{
	return get_format_str( 10, "FRBS" );
}

// text id 11
// [TOTS]Total Score
char* TextResReports::str_total_score()
{
	return get_format_str( 11, "TOTS" );
}

// text id 12
// [FINS]Final Score
char* TextResReports::str_final_score()
{
	return get_format_str( 12, "FINS" );
}

// text id 13
// [DIFR]Difficulty Rating
char* TextResReports::str_difficulty_rating()
{
	return get_format_str( 13, "DIFR" );
}

// text id 14
// [SBON]Bonus
char* TextResReports::str_scenario_bonus()
{
	return get_format_str( 14, "SBON" );
}

// text id 15
// [CHEA]Cheated
char* TextResReports::str_cheated()
{
	return get_format_str( 15, "CHEA" );
}

// text id 16
// [DUYD]%1d %1p{year|years} %2d %2p{day|days}
// text id 17
// [DUDD]%1d %1p{day|days}
char* TextResReports::str_duration_yd( int years, int days )
{
	if( years > 0 )
	{
		return process( get_format_str( 16, "DUYD" ), years, days );
	}
	else
	{
		return process( get_format_str( 17, "DUDD" ), days );
	}
}

// text id 18
// [DUYD]%1d %1p{year|years} %2d %2p{month|months}
// text id 19
// [DUDD]%1d %1p{month|months}
char* TextResReports::str_duration_ym( int years, int months )
{
	if( years > 0 )
	{
		return process( get_format_str( 18, "DUYM" ), years, months );
	}
	else
	{
		return process( get_format_str( 19, "DUMM" ), months );
	}
}

// text id 20
// [DUHM]%1z%1q{| hour and | hours and }%2d %2p{minute|minutes}
char* TextResReports::str_duration_hm( int hours, int minutes )
{
	return process( get_format_str( 20, "DUHM" ),
		hours, minutes );
}

// text id 21 - 29 reserved


// text id 30 - 38 name of report modes
// [REP1] - [REP9]
const char* TextResReports::str_report_mode(int viewMode )
{
	switch( viewMode )
	{
	case MODE_NATION:
		return get_format_str( 30, "REP1" );
	case MODE_TOWN:
		return get_format_str( 31, "REP2" );
	case MODE_ECONOMY:
		return get_format_str( 32, "REP3" );
	case MODE_TRADE:
		return get_format_str( 33, "REP4" );
	case MODE_MILITARY:
		return get_format_str( 34, "REP5" );
	case MODE_TECH:
		return get_format_str( 35, "REP6" );
	case MODE_SPY:
		return get_format_str( 36, "REP7" );
	case MODE_RANK:
		return get_format_str( 37, "REP8" );
	case MODE_NEWS_LOG:
		return get_format_str( 38, "REP9" );
	case MODE_AI_ACTION:
		return "AI Action";		// debugging text
	default:
		return "";
	}
}

// text id 39
// [NKIN]Kingdom
char* TextResReports::str_nat_kingdom()
{
	return get_format_str( 39, "NKIN" );
}

// text id 40
// [NREP]Reputation
char* TextResReports::str_nat_reputation()
{
	return get_format_str( 40, "NREP" );
}

// text id 41
// [NSTA]Status
char* TextResReports::str_nat_status()
{
	return get_format_str( 41, "NSTA" );
}

// text id 42
// [NAAT]Allow Attack
char* TextResReports::str_nat_allow_attack()
{
	return get_format_str( 42, "NAAT" );
}

// text id 43
// [NTRT]Trade Treaty
char* TextResReports::str_nat_trade_treaty()
{
	return get_format_str( 43, "NTRT" );
}

// text id 44
// [NTAM]Trade Amount
char* TextResReports::str_nat_trade_amount()
{
	return get_format_str( 44, "NTAM" );
}


// text id 45 - 48
// [RP11]Information
// [RP12]Diplomacy
// [RP13]Chat
// [RP14]Diplomatic Log
const char* TextResReports::str_nat_sub_mode(int subMode)
{
	switch(subMode)
	{
	case NATION_REPORT_INFO:
		return get_format_str( 45, "RP11" );
	case NATION_REPORT_TALK:
		return get_format_str( 46, "RP12" );
	case NATION_REPORT_CHAT:
		return get_format_str( 47, "RP13" );
	case NATION_REPORT_TALK_LOG:
		return get_format_str( 48, "RP14" );
	case NATION_REPORT_DEBUG:
		return "Debug";		// debuggin text
	default:
		err_here();
		return "";
	}
}

// text id 49
// [NO__]No
// text id 50
// [YES_]Yes
char* TextResReports::str_no_yes(char flag)
{
	if( !flag )
		return get_format_str( 49, "NO__" );
	else
		return get_format_str( 50, "YES_" );
}

// text id 51
// [(remote player disconnected)DISC]Disconnect
char* TextResReports::str_disconnect()
{
	return get_format_str( 51, "DISC" );
}

// text id 52
// [<King>KRAC]Nationality of the %1s
// text id 53
// [<All High>FSPE]Species of the %1s Fryhtan
char* TextResReports::str_king_race(char raceId)
{
	if( raceId >= 0 )
	{
		return process( get_format_str( 52, "KRAC"),
			text_unit.str_title(raceId, RANK_KING) );
	}
	else
	{
		return process( get_format_str( 53, "FSPE" ),
			text_unit.str_title(raceId, RANK_KING) );
	}
}

// text id 54
// [YFOO]Your Food
char* TextResReports::str_your_food()
{
	return get_format_str( 54, "YFOO" );
}

// text id 55
// [YCAS]Your Treasure
char* TextResReports::str_your_treasure()
{
	return get_format_str( 55, "YCAS" );
}

// text id 56
// [YLPS]Your Life Points
char* TextResReports::str_your_live_points()
{
	return get_format_str( 56, "YLPS" );
}

// text id 57
// [YCPT]Your Continuous Peace Time
char* TextResReports::str_your_peace_time()
{
	return get_format_str( 57, "YCPT" );
}


// text id 58
// [NIMP]Your Yearly Import from %1s
char* TextResReports::str_enemy_import_from(char* nationName)
{
	return process( get_format_str( 58, "NIMP" ), 
		nationName );
}

// text id 59
// [NEXP]Your Yearly Export to %1s
char* TextResReports::str_enemy_export_to(char* nationName)
{
	return process( get_format_str( 59, "NEXP" ),
		nationName );
}

// text id 60
// [NCPT]Continuous Peace Time of %1s
char* TextResReports::str_enemy_peace_time(char* nationName)
{
	return process( get_format_str( 60, "NCPT" ),
		nationName );
}

// text id 61
// [NSTT]Duration of %1s Status
char* TextResReports::str_enemy_status_duration(const char* statusStr)
{
	return process( get_format_str( 61, "NSTT" ),
		statusStr );
}

// text id 62
// [NAAT]Allow Your Units to Attack %1s
char* TextResReports::str_enemy_allow_attack(const char* nationName)
{
	return process( get_format_str( 62, "NAAT" ),
		nationName );
}

// text id 63
// [NCAS]%1s 's Treasure
char* TextResReports::str_ally_cash(char* nationName)
{
	return process( get_format_str( 63, "NCAS" ),
		nationName );
}

// text id 64
// [NFOO]%1s 's Food
char* TextResReports::str_ally_food(char* nationName)
{
	return process( get_format_str( 64, "NFOO" ),
		nationName );
}

// text id 65
// [NREL]%1s's Diplomatic Status with Other Kingdoms:
char* TextResReports::str_enemy_relation(char *nationName)
{
	return process( get_format_str( 65, "NREL"),
		nationName );
}

// text id 66
// [FROM]From
char* TextResReports::str_send_from()
{
	return get_format_str( 66, "FROM" );
}

// text id 67
// [TO__]To
char* TextResReports::str_send_to()
{
	return get_format_str( 67, "TO__" );
}

// text id 68
// [OND:]on %1s :
char* TextResReports::str_on_date(char* dateStr)
{
	return process( get_format_str( 68, "OND:" ),
		dateStr );
}

// text id 69
// [CHTI]Please enter your chat message and press <Enter> to send.
char* TextResReports::str_chat_inst()
{
	return get_format_str( 69, "CHTI" );
}

// text id 70
// [(Send to the selected kingdom)CHR0]Selected
// text id 71
// [(Send to all allied human controlled kingdoms)CHR1]Allies
// text id 72
// [(Send to all human controlled kingdoms)CHR2]All
const char* TextResReports::str_chat_recipient(int i)
{
	switch(i)
	{
	case CHAT_RECEIVER_CURRENT:
		return get_format_str( 70, "CHR0" );
	case CHAT_RECEIVER_ALL_ALLY:
		return get_format_str( 71, "CHR1" );
	case CHAT_RECEIVER_ALL_HUMAN:
		return get_format_str( 72, "CHR2" );
	}

	err_here();
	return "";
}

// text id 73
// [TOWN]Town
char* TextResReports::str_town()
{
	return get_format_str( 73, "TOWN" );
}

// text id 74
// [CITZ]Citizens
char* TextResReports::str_citizens()
{
	return get_format_str( 74, "CITZ" );
}

// text id 75
// [PEAS]Peasants
char* TextResReports::str_peasants()
{
	return get_format_str( 75, "PEAS" );
}

// text id 76
// [NATL]Nationality
char* TextResReports::str_nationality()
{
	return get_format_str( 76, "NATL" );
}

// text id 77
// [STRU]Structure
char* TextResReports::str_structure()
{
	return get_format_str( 77, "STRU" );
}

// text id 78
// [UNCO]Unit Cost
char* TextResReports::str_unit_cost()
{
	return get_format_str( 78, "UNCO" );
}

// text id 79
// [SRU#]No. of Structures
char* TextResReports::str_no_of_structures()
{
	return get_format_str( 79, "SRU#" );
}

// text id 80
// [YEXP]Yearly Expense
char* TextResReports::str_yearly_expense()
{
	return get_format_str( 80, "YEXP" );
}

// text id 81
// [YINC]Yearly Income
char* TextResReports::str_yearly_income()
{
	return get_format_str( 81, "YINC" );
}

// text id 82
// [TTWN]Total %1p{Town|Towns}: %1d
char* TextResReports::str_total_towns(int townNum)
{
	return process( get_format_str( 82, "TTWN" ),
		townNum );
}

// text id 83
// [TCTZ]Total %1p{Citizen|Citizens}: %1d
char* TextResReports::str_total_citizens(int num)
{
	return process( get_format_str( 83, "TCTZ" ),
		num );
}

// text id 84
// [TPEA]Total %1p{Peasant|Peasants}: %1d
char* TextResReports::str_total_peasant(int num)
{
	return process( get_format_str( 84, "TPEA" ),
		num );
}

// text id 85
// [TOUN]Total Other Units: %1d
char* TextResReports::str_total_other_units(int num)
{
	return process( get_format_str( 85, "TOUN" ),
		num );
}

// text id 86
// [TPOP]Total Population: %1d
char* TextResReports::str_total_population(int population)
{
	return process( get_format_str( 86, "TPOP" ),
		population );
}

// text id 87
// [TOTL]Total
char* TextResReports::str_total()
{
	return get_format_str( 87, "TOTL" );
}

// text id 88
// [INCI]Income Item
char* TextResReports::str_income_item()
{
	return get_format_str( 88, "INCI" );
}

// text id 89
// [EXPI]Expense Item
char* TextResReports::str_expense_item()
{
	return get_format_str( 89, "EXPI" );
}

// text id 90
// [TYIN]Total Yearly Income
char* TextResReports::str_total_yearly_income()
{
	return get_format_str( 90, "TYIN" );
}

// text id 91
// [TYEP]Total Yearly Expense
char* TextResReports::str_total_yearly_expense()
{
	return get_format_str( 91, "TYEP" );
}

// text id 92
// [YBAL]Yearly Balance
char* TextResReports::str_yearly_balance()
{
	return get_format_str( 92, "YBAL" );
}

// text id 93 - 100, 109 (101 - 108 reserved)
// [IND0]Sale of Goods
// [IND1]Exports
// [IND2]Taxes
// [IND3]Recovered Treasure
// [IND4]Sale of Buildings
// [IND5]Aid/Tribute from Other Kingdoms
// [IND6]Conversion
// [IND7]Cheating
// 
// [INF5]Aid/Tribute from Other Kingdoms/Slave Towns
char* TextResReports::str_income_desc(int incomeType, int isMonster )
{
	if( isMonster )
	{
		if( incomeType == INCOME_TRIBUTE )
			return get_format_str( 109, "INF5" );
	}

	if( incomeType == INCOME_SELL_GOODS )
	{	// check only the first
		return get_format_str( 93+incomeType, "IND0" );
	}
	return get_format_str( 93+incomeType, NULL );
}


// text id 110 - 126 (127-139 reserved)
// [EXP0]General Expenses
// [EXP1]Spy Expenses
// [EXP2]Other Mobile Human Unit Expenses
// [EXP3]Caravan Expenses
// [EXP4]Weapons Expenses
// [EXP5]Construction and Repair Costs
// [EXP6]Building Expenses
// [EXP7]Town Expenses
// [EXP8]Training Units
// [EXP9]Hiring Units
// [EXPa]Honoring Units
// [EXPb]Grants to Your Citizens
// [EXPc]Grants to Other People
// [EXPd]Imports
// [EXPe]Aid/Tribute to Other Kingdoms
// [EXPf]Bribes
// [EXPg]Conversion
char* TextResReports::str_expense_desc(int expenseType, int isMonster )
{
	if( expenseType == EXPENSE_GENERAL )
	{
		// check only the first
		return get_format_str( 110+expenseType, "EXP0" );
	}
	return get_format_str( 110+expenseType, NULL );
}


// text id 140 - 142
// [TST1]Stop 1
// [TST2]Stop 2
// [TST3]Stop 3
const char* TextResReports::str_trade_stop(int stopId)
{
	switch( stopId )
	{
	case 1:
		return get_format_str( 140, "TST1" );
	case 2:
		return get_format_str( 141, "TST2" );
	case 3:
		return get_format_str( 142, "TST3" );
	default:
		err_here();
		return "";
	}
}

// text id 143
// [GDCR]Goods Carried
char* TextResReports::str_goods_carried()
{
	return get_format_str( 143, "GDCR" );
}

// text id 144
// [TCAR]Total %1p{Caravan|Caravans}: %1d
char* TextResReports::str_total_caravan( int count)
{
	return process( get_format_str( 144, "TCAR" ),
		count );
}

// text id 145
// [COMD]Commander
char* TextResReports::str_mil_commander()
{
	return get_format_str( 145, "COMD" );
}

// text id 146
// [CMDS]Commanded Soldiers
char* TextResReports::str_mil_commanded_soldiers()
{
	return get_format_str( 146, "CMDS" );
}

// text id 147
// [STAT]Status
char* TextResReports::str_mil_status()
{
	return get_format_str( 147, "STAT" );
}

// text id 148
// [UTYP]Unit Type
char* TextResReports::str_mil_unit_type()
{
	return get_format_str( 148, "UTYP" );
}

// text id 149
// [UNI#]Unit Types
char* TextResReports::str_mil_no_of_units()
{
	return get_format_str( 149, "UNI#" );
}

// text id 150
// [TCMD]Total %1p{Commander|Commanders}: %1d
char* TextResReports::str_total_commander(int num)
{
	return process( get_format_str( 150, "TCMD" ),
		num );
}

// text id 151
// [TSUB]Total %1p{Soldier|Soldiers} Under Command: %1d
char* TextResReports::str_total_soldier_under(int num)
{
	return process( get_format_str( 151, "TSUB" ),
		num );
}

// text id 152
// [TSLD]Total %1p{Soldier|Soldiers}: %1d
char* TextResReports::str_total_soldier(int num)
{
	return process( get_format_str( 152, "TSLD" ),
		num );
}

// text id 153
// [TUNI]Total Units: %1d
char* TextResReports::str_total_unit(int num)
{
	return process( get_format_str( 153, "TUNI" ),
		num );
}

// text id 154 - 156 ( 157 - 159 reserved)
// [UMOB]Mobile
// [UINF]In Fort
// [UINS]On Ship
char* TextResReports::str_mil_unit_status(int unitMode )
{
	switch( unitMode )
	{
	case UNIT_MODE_OVERSEE:
		return get_format_str( 155, "UINF" );
	case UNIT_MODE_ON_SHIP:
		return get_format_str( 156, "UINS" );
	default:
		return get_format_str( 154, "UMOB" );
	}
}

// text id 160
// [TECH]Technology
char* TextResReports::str_technology()
{
	return get_format_str( 160, "TECH" );

}

// text id 161
// [PRVE]Present Version
char* TextResReports::str_tech_present_version()
{
	return get_format_str( 161, "PRVE" );
}

// text id 162
// [REVE]Researching Version
char* TextResReports::str_tech_researching_version()
{
	return get_format_str( 162, "REVE" );
}

// text id 163
// [REPR]Research Progress
char* TextResReports::str_tech_research_progress()
{
	return get_format_str( 163, "REPR" );
}

// text id 164
// [ACQS]Acquired %1p{Scroll|Scrolls}:
char* TextResReports::str_tech_acq_scroll(int num)
{
	return process( get_format_str( 164, "ACQS" ),
		num);
}

// text id 165
// [GRBE]Greater Being
char* TextResReports::str_tech_greater_being()
{
	return get_format_str( 165, "GRBE" );
}

// text id 166
// [ESP:]Espionage: 
char* TextResReports::str_tech_espionage_()
{
	return get_format_str( 166, "ESP:" );
}

// text id 167
// [SPNM]Spy Name
char* TextResReports::str_spy_name()
{
	return get_format_str( 167, "SPNM" );
}

// text id 168
// [SPCL]Cloak
char* TextResReports::str_spy_cloak()
{
	return get_format_str( 168, "SPCL" );
}

// text id 169
// [SPLO]Location
char* TextResReports::str_spy_location()
{
	return get_format_str( 169, "SPLO" );
}

// text id 170
// [SPSK]Skill
char* TextResReports::str_skill()
{
	return get_format_str( 170, "SPSK" );
}

// text id 171
// [SPAC]Action
char* TextResReports::str_spy_action()
{
	return get_format_str( 171, "SPAC" );
}

// text id 172
// [TSPY]Total %1p{Spy|Spies}: %1d
char* TextResReports::str_total_spy(int num)
{
	return process( get_format_str( 172, "TSPY" ), 
		num );
}

// text id 173, 174 (175-177 reserved)
// [SLEP]Sleep
// [AGIT]Agitate
const char* TextResReports::str_spy_action_mode( int spyActionMode )
{
	switch( spyActionMode )
	{
	case SPY_IDLE:
		return get_format_str( 173, "SLEP" );
	case SPY_SOW_DISSENT:
		return get_format_str( 174, "AGIT" );
	default:
		err_here();
		return "";
	}
}

// text id 178
// [DSPY]Counter-spy
char* TextResReports::str_counter_spy_action_mode()
{
	return get_format_str( 178, "DSPY" );
}

// text id 179
// [OVRA]Overall
char* TextResReports::str_overall()
{
	return get_format_str( 179, "OVRA" );
}

// text id 180 - 184 (185-189 reserved)
// [RNK1]Population Score
// [RNK2]Military Score
// [RNK3]Economic Score
// [RNK4]Reputation Score
// [RNK5]Fryhtan Battling Score
char* TextResReports::str_score_type( int rankType )
{
	if( rankType == 1 )	// check mark only on the first
	{
		return get_format_str( 179+rankType, "RNK1" );
	}
	if( rankType == 0 )	// overall
	{
		return get_format_str( 190, "OVAS" );
	}
	err_when( rankType > 5 );
	return get_format_str( 179+rankType, NULL );
}

// text id 190
// [OVAS]Overall Score
char* TextResReports::str_overall_score()
{
	return get_format_str( 190, "OVAS" );
}

// text id 191
// [GAND]GOAL: Achieve ALL of the Following
char* TextResReports::str_goal_and()
{
	return get_format_str( 191, "GAND" );
}

// text id 192
// [G_OR]GOAL: Achieve One of the Following
char* TextResReports::str_goal_or()
{
	return get_format_str( 192, "G_OR" );
}

// text id 193
// [GOAL]GOAL: 
char* TextResReports::str_goal_()
{
	return get_format_str( 193, "GOAL" );
}

// text id 194
// [TPLT]Total Playing Time
char* TextResReports::str_total_play_time()
{
	return get_format_str( 194, "TPLT" );
}

// text id 195
// [G1_T]Achieve before %1s
char* TextResReports::str_goal_single_timed( char *endDateStr )
{
	return process( get_format_str( 195, "G1_T" ), endDateStr );
}

// text id 196
// [GA_T]GOAL: Achieve ALL of the Following before %1s
char* TextResReports::str_goal_and_timed( char *endDateStr )
{
	return process( get_format_str( 196, "GA_T" ), endDateStr );
}

// text id 197
// [GO_T]GOAL: Achieve One of the Following before %1s
char* TextResReports::str_goal_or_timed( char *endDateStr )
{
	return process( get_format_str( 197, "GO_T" ), endDateStr );
}

// text id 198
// [LOSE]LOSE: 
char* TextResReports::str_lose_()
{
	return get_format_str( 198, "LOSE" );
}

// text id 199 reserved

// text id 200
// [CDIF]Campaign Difficulty Level: 
char* TextResReports::str_campaign_difficulty()
{
	return get_format_str( 200, "CDIF" );
}

// text id 201
// [CYPA]The game started on %1s and you have been playing for %2d %2p{year|years}.
char* TextResReports::str_campaign_year_passed( char* startDateStr, int yearPassed )
{
	return process( get_format_str( 201, "CYPA" ),
		startDateStr, yearPassed );
}


// text id 202
// [HIKD]Hide Kingdoms' detail
char* TextResReports::str_hide_nation_detail()
{
	return get_format_str( 202, "HIKD" );
}
