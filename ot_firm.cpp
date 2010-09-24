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

// Filename    : OT_FIRM.CPP
// Description : Text resource firm


#include <OT_FIRM.H>
#include <ALL.H>
#include <OGODRES.H>
#include <OITEM.H>
#include <OF_OFF.H>


void TextResFirm::init()
{
	TextResource::init( DIR_RES"T_FIRM.RES" );
}

// text id 1 - 12
// [BAG1]When taxes are gathered from Norman subjects, their loyalty will decrease by 7 points instead of 10 points.
// [BAG2]Viking soldiers will now gain power-attack skills 25 points earlier than normal.
// [BAG3]Your skill of your Deruvids will now increase at a much greater rate.
// [BAG4]All Roman military units will now be trained with 10 more points of combat skill.
// [BAG5]Advanced walls will automatically be built around all of your towns.  Damaged walls will automatically be repaired at no cost.
// [BAG6]Your farmers will now produce 30%% more food than those of other kingdoms, enabling you to support more soldiers and workers.
// [BAG7]You will now receive a 20%% greater profit from all sales in your markets.
// [BAG8]In explored areas you will now be able to see all enemies who have been ordered to attack you, as well as their intended paths.
// [BAG9]Resistance to you in all independent towns has now dropped by 20 points.
// [BAGA]The rate of population growth in all of your Egyptian towns is now 30%% greater.
// [BAGB]When other Kingdoms break treaties with you, they will suffer twice the normal penalties.
// [BAGC]Your research projects will now be completed in 30% less time.

char* TextResFirm::str_base_prayer_effect( int godId )
{
	switch( godId )
	{
	case GOD_NORMAN:
		return get_format_str( 1, "BAG1" );
	case GOD_VIKING:
		return get_format_str( 2, "BAG2" );
	case GOD_CELTIC:
		return get_format_str( 3, "BAG3" );
	case GOD_ROMAN:
		return get_format_str( 4, "BAG4" );
	case GOD_GREEK:
		return get_format_str( 5, "BAG5" );
	case GOD_CARTH:
		return get_format_str( 6, "BAG6" );
	case GOD_CHINESE:
		return get_format_str( 7, "BAG7" );
	case GOD_JAPANESE:
		return get_format_str( 8, "BAG8" );
	case GOD_MONGOL:
		return get_format_str( 9, "BAG9" );
	case GOD_EGYPTIAN:
		return get_format_str( 10, "BAGA" );
	case GOD_PERSIAN:
		return get_format_str( 11, "BAGB" );
	case GOD_INDIAN:
		return get_format_str( 12, "BAGC" );
	default:
		err_here();
		return "";
	}
}

// text 13 - 37 ( 38 - 43 reserved )
// [BAW1] - [BAWV] Wish result text, see of_basew.cpp


// text id 44
// [BAW_]No Effect!


// text id 45, 46
// [CMP1]Sortie No Leader
// [CMP2]Sortie No Injured
char* TextResFirm::str_patrol_state( int patrolState)
{
	switch( patrolState )
	{
	case 1:
		return get_format_str( 45, "CMP1" );
	case 2:
		return get_format_str( 46, "CMP2" );
	default:
		return "";
	}
}

// text id 47
// [CMBC](%1,3d chance)
char* TextResFirm::str_bribe_chance( int chance )
{
	return process( get_format_str( 47, "CMBC" ), chance );
}

// text id 48
// [CMBA]Offer %1,2d
char* TextResFirm::str_bribe_amount( int bribeAmount)
{
	return process( get_format_str( 48, "CMBA" ), bribeAmount );
}

// text id 49
// [CMB?]Bribe %1s ?
char* TextResFirm::str_ask_bribe_unit( char *unitName )
{
	return process( get_format_str( 49, "CMB?" ), unitName );
}

// text id 50
// [CMBY]Bribing Succeeded.
char* TextResFirm::str_bribe_success()
{
	return get_format_str( 50, "CMBY" );
}

// text id 51
// [CMBN]Bribing Failed.
char* TextResFirm::str_bribe_fail()
{
	return get_format_str( 51, "CMBN" );
}

// text id 52
// [CMBK]Your Spy Was Caught And Executed.
char* TextResFirm::str_bribe_execute()
{
	return get_format_str( 52, "CMBK" );
}

// text id 53
// [CMAA]Will you really attempt to assassinate the %1s?
char* TextResFirm::str_ask_assassin( char* title )
{
	return process( get_format_str( 53, "CMAA" ), title );
}

// text id 54
// [CMAC]You have a %1,3d chance of success!
char* TextResFirm::str_assassin_chance( int chance )
{
	return process( get_format_str( 54, "CMAC" ), chance );
}

// text id 55
// [YES_]Yes
char* TextResFirm::str_yes()
{
	return get_format_str( 55, "YES_" );
}

// text id 56
// [NO__]No
char* TextResFirm::str_no()
{
	return get_format_str( 56, "NO__" );
}

// text id 57
// [CMAS]Assassination Succeeded.
char* TextResFirm::str_assassin_success()
{
	return get_format_str( 57, "CMAS" );
}

// text id 58
// [CMAE]Your Spy Escaped.
char* TextResFirm::str_assassin_escape()
{
	return get_format_str( 58, "CMAE" );
}

// text id 59
// [CMAX]Your Spy Was Caught And Executed.
char* TextResFirm::str_assassin_executed()
{
	return get_format_str( 59, "CMAX" );
}

// text id 60
// [CMAF]Assassination Failed.
char* TextResFirm::str_assassin_fail()
{
	return get_format_str( 60, "CMAF" );
}

// text id 61
// [CMTA]Do you really want to steal technology?
char* TextResFirm::str_ask_steal_tech()
{
	return get_format_str( 61, "CMTA" );
}

// text id 62
// [CMTS]You have a %1,3d chance of success!
char* TextResFirm::str_steal_tech_chance(int chance)
{
	return process( get_format_str( 62, "CMTS" ), chance );
}

// text id 63
// [CMTY]You have stolen technology %1s %2r
char* TextResFirm::str_steal_tech_success( char* techName, int techLevel)
{
	return process( get_format_str( 63, "CMTY" ), techName, techLevel );
}

// text id 64
// [CMTN]Steal Technology Failed.
char* TextResFirm::str_steal_tech_fail()
{
	return get_format_str( 64, "CMTN" );
}

// text id 65
// [CMTK]Your Spy Was Caught And Executed.
char* TextResFirm::str_steal_tech_executed()
{
	return get_format_str( 65, "CMTK" );
}

// text id 66
// [CMTZ]Cannot find any useful technology.
char* TextResFirm::str_steal_tech_no_yield()
{
	return get_format_str( 66, "CMTZ" );
}

// text id 67
// [CMW1]Choose other Kingdom to be involved in the Incident
char* TextResFirm::str_incident_choose()
{
	return get_format_str( 67, "CMW1" );
}

// text id 68
// [CMWA]Do you really want to create an incident?
char* TextResFirm::str_ask_incident()
{
	return get_format_str( 68, "CMWA" );
}

// text id 69
// [CMWC](%1,3d chance)
char* TextResFirm::str_incident_chance( int chance )
{
	return process( get_format_str( 69, "CMWC" ), chance );
}

// text id 70
// [CMWY]Create Incident Successful.
char* TextResFirm::str_incident_success()
{
	return get_format_str( 70, "CMWY" );
}

// text id 71
// [CMWF]Create Incident Failed.
char* TextResFirm::str_incident_fail()
{
	return get_format_str( 71, "CMWF" );
}

// text id 72
// [CMWX]Your Spy Was Caught And Executed.
char* TextResFirm::str_incident_execute()
{
	return get_format_str( 72, "CMWX" );
}

// text id 73 - 80
// reserved for firm camp


// text id 81 - 83
// [FAR1]Raw Clay
// [FAR2]Raw Copper
// [FAR3]Raw Iron
char* TextResFirm::str_factory_raw( int rawId )
{
	switch( rawId )
	{
	case RAW_CLAY:
		return get_format_str( 81, "FAR1" );
	case RAW_COPPER:
		return get_format_str( 82, "FAR2" );
	case RAW_IRON:
		return get_format_str( 83, "FAR3" );
	default:
		return "";
	}
}

// text id 84 - 86
// [FAW1]Pottery
// [FAW2]Copper Ware
// [FAW3]Iron Ware
char* TextResFirm::str_factory_ware( int rawId )
{
	switch( rawId )
	{
	case RAW_CLAY:
		return get_format_str( 84, "FAW1" );
	case RAW_COPPER:
		return get_format_str( 85, "FAW2" );
	case RAW_IRON:
		return get_format_str( 86, "FAW3" );
	default:
		return "";
	}
}

// text id 87
// [FAMP]Monthly Production
char* TextResFirm::str_monthly_production()
{
	return get_format_str( 87, "FAMP" );
}

// text id 88
// [FAAU]Auto
char* TextResFirm::str_factory_auto()
{
	return get_format_str( 88, "FAAU" );
}

// text id 89
// [FAMA]Manual
char* TextResFirm::str_factory_manual()
{
	return get_format_str( 89, "FAMA" );
}


// text id 90
// [INBI]Buy %1s
char* TextResFirm::str_buy_item_help( Item *itemPtr )
{
	String itemName;
	itemName = itemPtr->item_name();	// avoid item_name() use str_buf
	return process( get_format_str( 90, "INBI" ), (char *)itemName );
}

// text id 91
// [MANT]You're not permitted to trade with this market.
char* TextResFirm::str_no_trade()
{
	return get_format_str( 91, "MANT" );
}

// text id 92 - 94
// [MAP1]Clay Products
// [MAP2]Copper Products
// [MAP3]Iron Products
char* TextResFirm::str_product_name(int rawId )
{
	switch( rawId )
	{
	case RAW_CLAY:
		return get_format_str( 92, "MAP1" );
	case RAW_COPPER:
		return get_format_str( 93, "MAP2" );
	case RAW_IRON:
		return get_format_str( 94, "MAP3" );
	default:
		return "";
	}
}

// text id 95
// [MAYI]Yearly Income %1d
char* TextResFirm::str_yearly_income( int incomeAmount )
{
	return process( get_format_str( 95, "MAYI"), incomeAmount );
}

// text id 96
// [MIUR]Unmined Raw
char* TextResFirm::str_unmined_raw()
{
	return get_format_str( 96, "MIUR" );
}

// text id 97
// [MIMR]Mined Raw
char* TextResFirm::str_mined_raw()
{
	return get_format_str( 97, "MIMR" );
}

// text id 98 - 106
// [OFM1]Select the Building that you wish to Damage
// [OFM2]Select the Units that you wish to Freeze
// [OFM3]Select the Object that you wish to Wound
// [OFM4]Select the Destination for the Teleported Unit
// [OFM5]Not used
// [OFM6]Select the Objects that you wish to Wound
// [OFM7]Not used
// [OFM8]Reserved
// [OFM9]Reserved
char* TextResFirm::str_cast_magic_instruct( int magicId )
{
	switch( magicId )
	{
	case MAGIC_DESTROY_BUILDING:
		return get_format_str( 98, "OFM1" );
	case MAGIC_FREEZE_UNIT:
		return get_format_str( 99, "OFM2" );
	case MAGIC_DAMAGE_AREA:
		return get_format_str( 100, "OFM3" );
	case MAGIC_TELEPORT:
		return get_format_str( 101, "OFM4" );
	case MAGIC_FREEZE_LASER:
		return get_format_str( 102, "OFM5" );
	case MAGIC_GOD_POWER:
		return get_format_str( 103, "OFM6" );
	case MAGIC_LASER:
		return get_format_str( 104, "OFM7" );
	default:
		return "";
	}
}

// text id 107
// [OFMS]Select a Unit to Teleport
char* TextResFirm::str_cast_teleport_source()
{
	return get_format_str( 107, "OFMS" );
}

// text id 108
// [OFCC]This Action will Cost %1,2d
char* TextResFirm::str_cast_cost_cash( int cost )
{
	return process( get_format_str( 108, "OFCC"), cost );
}

// text id 109
// [OFTC]Teleportation will Cost %1,2d
char* TextResFirm::str_teleport_cost_cash( int cost )
{
	return process( get_format_str( 109, "OFTC"), cost );
}

// text id 110
// [TSSC]Select a technology class to research
char* TextResFirm::str_select_tech_class()
{
	return get_format_str( 110, "TSSC" );
}

// text id 111
// [TSFC]All Technology in this class has been researched !
char* TextResFirm::str_tech_class_finished()
{
	return get_format_str( 111, "TSFC" );
}

// text id 112
// [TSNO]No Technology is currently being researched.
char* TextResFirm::str_no_research()
{
	return get_format_str( 112, "TSNO" );
}

// text id 113
// [TSRP]Research Progress
char* TextResFirm::str_research_progress()
{
	return get_format_str( 113, "TSRP" );
}

// text id 114
// [TSCP]Completion Percent
char* TextResFirm::str_completion_percent()
{
	return get_format_str( 114, "TSCP" );
}

// text id 115
// [WFNW]No Weapon is under construction !
char* TextResFirm::str_no_build_weapon()
{
	return get_format_str( 115, "WFNW" );
}

// text id 116
// [MTCL]This Action will Cost %1d Life Points
char* TextResFirm::str_cast_cost_live_points( int cost )
{
	return process( get_format_str( 116, "MTCL" ), cost );
}

// text id 117
// [MTTL]Teleportation will Cost %1d Life Points
char* TextResFirm::str_teleport_cost_live_points( int cost)
{
	return process( get_format_str( 117, "MTTL" ), cost );
}

// text id 118
// [LARE]Life Point Cost : %1d
char* TextResFirm::str_lair_research(int livePoints)
{
	return process( get_format_str( 118, "LARE" ), livePoints );
}

// text id 119
// [ALR1]Clay Deposit
// text id 120
// [ALR2]Copper Deposit
// text id 121
// [ALR3]Iron Deposit
char* TextResFirm::str_alchemy_raw(int rawId )
{
	switch( rawId )
	{
	case RAW_CLAY:
		return get_format_str( 119, "ALR1" );
	case RAW_COPPER:
		return get_format_str( 120, "ALR2" );
	case RAW_IRON:
		return get_format_str( 121, "ALR3" );
	default:
		return "";
	}
}

// text id 122
// [ALRQ]Estimated Reserves %1z
char* TextResFirm::str_estimated_resource(int reservedQty)
{
	return process( get_format_str( 122, "ALRQ"), reservedQty );
}

// text id 123
// [ALID]No conversion in progress
char* TextResFirm::str_alchemy_idle()
{
	return get_format_str( 123, "ALID" );
}

// text id 124
// [ALCS]Converting %1s
char* TextResFirm::str_conversion( char* techName )
{
	return process( get_format_str( 124, "ALCS" ), techName );
}

// text id 125
// [ANHC]Hiring Cost : %1,2d + %2d Life points
char* TextResFirm::str_hire_animal_cost( int cash, int livePoints )
{
	return process( get_format_str( 125, "ANHC" ), cash, livePoints );
}

// text id 126 - 133
// [FIRN]%2s %1s
// [FIR#]%2s %1s %3d
// [FORN]%1s %2s
// [FOR#]%1s %2s %3d
// [FX3N]
// [FX3#]
// [FX4N]
// [FX4#]
char* TextResFirm::str_firm_name( int firmBuildId, char *closeTownName, int instanceId )
{
	FirmBuild *firmBuild = firm_res.get_build(firmBuildId);
	if( closeTownName )
	{
		switch( firmBuild->firm_id )
		{
		case FIRM_FORT:
			if( instanceId <= 1 )
				return process( get_format_str( 128, "FORN" ), firmBuild->name, closeTownName );
			else
				return process( get_format_str( 129, "FOR#" ), firmBuild->name, closeTownName, instanceId );
		default:
			if( instanceId <= 1 )
				return process( get_format_str( 126, "FIRN" ), firmBuild->name, closeTownName );
			else
				return process( get_format_str( 127, "FIR#" ), firmBuild->name, closeTownName, instanceId );
		}
	}

	return firmBuild->name;
}


// text id 134
// [UNDC]Under Construction
char* TextResFirm::str_under_construction()
{
	return get_format_str( 134, "UNDC" );
}

// text id 135
// [STIF]Which category of information do you wish to view?
char* TextResFirm::str_spy_steal_info()
{
	return get_format_str( 135, "STIF" );
}

// text id 136 - 139 reserved

// text id 140
// [SCRL]*%1s Scroll of Power*
char* TextResFirm::str_scroll_if_1( char* raceName )
{
	return process( get_format_str( 140, "SCRL" ),
		raceName );
}

// text id 141
// [SCR2]Owners may build the %1s %2s and Invoke %3s
char* TextResFirm::str_scroll_if_2( char* raceName, char* firmBuildName, char* godName )
{
	return process( get_format_str( 141, "SCR2" ),
		raceName, firmBuildName, godName );
}

// text id 142
// [SGOL]Treasure
char* TextResFirm::str_treasure_if_1()
{
	return get_format_str( 142, "SGOL" );
}

// text id 143
// [SGOW]Worth
char* TextResFirm::str_treasure_if_2()
{
	return get_format_str( 143, "SGOW" );
}

// text id 144
// [SOW1]Scroll of Weapon
char* TextResFirm::str_blueprint_if_1()
{
	return get_format_str( 144, "SOW1" );
}

// text id 145
// [SOW2]Technology
char* TextResFirm::str_blueprint_if_2()
{
	return get_format_str( 145, "SOW2" );
}


// text id 151
// [TWNF]Town of
char* TextResFirm::str_town_of()
{
	return get_format_str( 151, "TWNF" );
}

// text id 152
// [<race>RTWN]%1s Town
char* TextResFirm::str_race_town( char* raceName )
{
	return process( get_format_str( 152, "RTWN" ),
		raceName );
}

// text id 153
// [<race>STWN]%1s Slave Town
char* TextResFirm::str_race_slave_town( char* raceName )
{
	return process( get_format_str( 153, "STWN" ),
		raceName );
}

// text id 154
// [NWAL]No Town Wall
char* TextResFirm::str_no_town_wall()
{
	return get_format_str( 154, "NWAL" );
}

// text id 155
// [TPOP]Population
char* TextResFirm::str_town_population()
{
	return get_format_str( 155, "TPOP" );
}


// text id 156
// [TWRK]Worker
char* TextResFirm::str_town_worker()
{
	return get_format_str( 156, "TWRK" );
}

// text id 157
// [TPES]Peasant
char* TextResFirm::str_town_peasant()
{
	return get_format_str( 157, "TPES" );
}

// text id 158
// [RSST]Resistance
char* TextResFirm::str_resistance()
{
	return get_format_str( 158, "RSST" );
}

// text id 159
// [(wall repair)WRPR]Repair
char* TextResFirm::str_wall_repair()
{
	return get_format_str( 159, "WRPR" );
}

// text id 160
// [(wall building)WBLD]Building
char* TextResFirm::str_wall_building()
{
	return get_format_str( 160, "WBLD" );
}

// text id 161
// [SLVT]Annual tribute paid to you by this town: %1,2d
char* TextResFirm::str_slave_tribute( int amount )
{
	return process( get_format_str( 161, "SLVT" ),
		amount );
}

// text id 162
// [TSKL]Skill
char* TextResFirm::str_town_spy_skill()
{
	return get_format_str( 162, "TSKL");
}

// text id 163
// [SMIS]Mission
char* TextResFirm::str_town_spy_mission()
{
	return get_format_str( 163, "SMIS" );
}

// text id 164
// [AUTT]Automatically Collect Tax from this Town (Left - click) or all your Towns (Right - click) when the Loyalty reaches:
char* TextResFirm::str_auto_tax_inst()
{
	return get_format_str( 164, "AUTT" );
}

// text id 165
// [AUTG]Automatically Grant Money to this Town (Left - click) or all your Towns (Right - click) when the Loyalty drops below:
char* TextResFirm::str_auto_grant_inst()
{
	return get_format_str( 165, "AUTG" );
}

// text id 166
// [(not used)WMAN]Please assign the number of jobless civilians (0 - 8) to build the town wall :
char* TextResFirm::str_set_wall_crew()
{
	return get_format_str( 166, "WMAN" );
}

// text id 167
// [DISA]DISABLE
char* TextResFirm::str_disable_auto()
{
	return get_format_str( 167, "DISA" );
}

// text id 168
// [NOWK]%1z%1q{No Worker| Worker| Workers}
char* TextResFirm::str_no_of_worker( int w )
{
	return get_format_str( 168, "NOWK" );
}

// text id 169
// [LOYT]Loyalty to %1s
char* TextResFirm::str_loyalty_to(char *nationName)
{
	return process( get_format_str( 169, "LOYT" ),
		nationName );
}

// text id 170
// [RSST]Resistance to %1s
char* TextResFirm::str_resistance_to(char *nationName)
{
	return process( get_format_str( 170, "RSST" ),
		nationName );
}

// text id 171
// [LINC]Left click to increase
char* TextResFirm::str_left_inc()
{
	return get_format_str( 171, "LINC" );
}

// text id 172
// [RINC]Right click to increase
char* TextResFirm::str_right_dec()
{
	return get_format_str( 172, "RINC" );
}


// text id 173
// [TIIF]%1q{|Peasant: |Peasants: }%1z %2q{|Worker: |Workers: }%2z
char* TextResFirm::str_town_info_pop( int peasant, int worker )
{
	return process( get_format_str( 173, "TIIF" ),
		peasant, worker );
}

// text id 174
// [YSPY](Your %1p{Spy|Spyies})
char* TextResFirm::str_spy_inside( int spyCount )
{
	return process( get_format_str( 174, "YSPY" ),
		spyCount );
}

// text id 175
// [NOCM]No Commander
char* TextResFirm::str_no_commander()
{
	return get_format_str( 175, "NOCM" );
}

// text id 176
// [<raw>MINR]Mining %1s
// text id 177
// [MIN0]No Raw Under
char* TextResFirm::str_mine_info(int rawId)
{
	if( rawId )
	{
		return process( get_format_str( 176, "MINR" ),
			str_factory_raw(rawId) );
	}
	else
		return get_format_str( 177, "MIN0" );
}

// text id 178
// [MINN]No Natural Resources near !! You cannot mine in here.
char* TextResFirm::str_mine_no_raw()
{
	return get_format_str( 178, "MINN" );
}

// text id 179
// [<product name>FACR]Producing %1s
// text id 180
// [FACN]No Ware Under Production
char* TextResFirm::str_factory_info(int rawId)
{
	if( rawId )
		return process( get_format_str( 179, "FACR" ),
			str_factory_ware(rawId) );
	else
		return get_format_str( 180, "FACN" );
}


// text id 181
// [PRAY]Pray Points
char* TextResFirm::str_pray_points()
{
	return get_format_str( 181, "PRAY" );
}

// text id 182
// [ENER]Energy
char* TextResFirm::str_cast_energy()
{
	return get_format_str( 182, "ENER" );
}

// text id 183
// [BUYL]It costs %1,2d to buy the loyalty of this lair.%2?{ Proceed? |}
char* TextResFirm::str_buy_lair( int cost, int enoughFlag )
{
	return process( get_format_str( 183, "BUYL"), cost, enoughFlag );
}


// text id 184
// [BLNC]You do not have enough money!
char* TextResFirm::str_buy_lair_short_cash()
{
	return get_format_str( 184, "BLNC" );
}
