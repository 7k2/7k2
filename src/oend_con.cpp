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

// Filename    : OEND_CON.CPP
// Description : End Condition


#include <OEND_CON.H>
#include <OSTR.H>
#include <ONATION.H>
#include <OUNITRES.H>
#include <OUNIT.H>
#include <OFIRM.H>
#include <OTRANSL.H>
#include <OITEMRES.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OSITE.H>
#include <OT_UNIT.H>
#include <OT_ENDC.H>

#define MEET_UNIT_RANGE 2


//------- define static vars -----------//

EndConditionInfo EndConditionInfo::end_condition_info[END_CONDITION_COUNT] =
{
	{ E_DEFEAT_ALL_OTHER_KINGDOMS, E_DEFEAT_OWN_KINGDOM, EW_WIN_ONLY,  EP_NONE,          EP_NONE,         ET_OPTIONAL,   "Defeat all other Kingdoms" },
	{ E_DEFEAT_OWN_KINGDOM, E_DEFEAT_ALL_OTHER_KINGDOMS, EW_LOSE_ONLY, EP_NONE,          EP_NONE,         ET_OPTIONAL,   "Your Kingdom is defeated"  },
	{ E_DEFEAT_KINGDOM,                              -1, EW_WIN_ONLY,  EP_NATION_RECNO,  EP_NONE,         ET_OPTIONAL,   "Defeat a certain Kingdom" },
	{ E_FORM_ALLY_WITH_KINGDOM,                      -1, EW_WIN_ONLY,  EP_NATION_RECNO,  EP_NONE,         ET_OPTIONAL,   "Ally with a certain Kingdom" },
	{ E_DEFEAT_ALL_FRYHTAN,                          -1, EW_WIN_ONLY,  EP_NONE,          EP_NONE,         ET_OPTIONAL,   "Defeat all Fryhtan" },
	{ E_DEFEAT_EXCEPT_ALLIANCE,                      -1, EW_WIN_ONLY,  EP_NONE,          EP_NONE,         ET_NONE,       "Defeat all except one allied Kingdom" },
	{ E_REACH_POPULATION,                            -1, EW_WIN_ONLY,  EP_NONE,          EP_0_10000,      ET_OPTIONAL,   "Population exceed a number" },
	{ E_REACH_FRYHTAN,                               -1, EW_WIN_ONLY,  EP_NONE,          EP_0_10000,      ET_OPTIONAL,   "Fryhtan population exceed a number" },
	{ E_REACH_UNIT_COUNT,                            -1, EW_WIN_ONLY,  EP_UNIT_ID,       EP_0_1000,       ET_OPTIONAL,   "Number of a unit exceed a number" },
	{ E_REACH_ECONOMY,                               -1, EW_WIN_ONLY,  EP_NONE,	       EP_0_10000,      ET_OPTIONAL,   "Economic score exceeds a level" },
	{ E_REACH_MILITARY,	                            -1, EW_WIN_ONLY,  EP_NONE,	       EP_0_10000,      ET_OPTIONAL,   "Military score exceeds a level" },
	{ E_REACH_TOTAL,	                               -1, EW_WIN_ONLY,  EP_NONE,	       EP_0_10000,      ET_OPTIONAL,   "Total score exceeds a level" },
	{ E_KILL_MONSTER_SCORE,	                         -1, EW_WIN_ONLY,  EP_NONE,	       EP_0_10000,      ET_OPTIONAL,   "Fryhtan battling score exceeds a level" },
	{ E_TRAIN_10_GENERAL,	                         -1, EW_WIN_ONLY,  EP_NONE,	       EP_0_100,        ET_OPTIONAL,   "Train 10 Generals" },
	{ E_KILL_UNIT,                    E_KEEP_UNIT_ALIVE, EW_WIN_LOSE,  EP_UNIT_RECNO,    EP_NONE,         ET_OPTIONAL,   "A certain unit is killed" },
	{ E_KEEP_UNIT_ALIVE,                    E_KILL_UNIT, EW_WIN_LOSE,  EP_UNIT_RECNO,    EP_NONE,         ET_COMPULSORY, "A certain unit survives" },
	{ E_MEET_UNIT,                                   -1, EW_WIN_ONLY,  EP_UNIT_RECNO,    EP_NONE,         ET_OPTIONAL,   "Meet a certain unit" },
	{ E_CONQUER_ALL_TOWN,                            -1, EW_WIN_ONLY,  EP_NONE,          EP_NONE,         ET_OPTIONAL,   "Conquer all indepedent towns" },
	{ E_CONQUER_TOWN,                                -1, EW_WIN_LOSE,  EP_TOWN_RECNO,    EP_NONE,         ET_OPTIONAL,   "Conquer a certain town" },
	{ E_DESTROY_FIRM,                                -1, EW_WIN_LOSE,  EP_FIRM_RECNO,    EP_NONE,         ET_OPTIONAL,   "Destroy a certain building" },
	{ E_GET_SCROLL,                                  -1, EW_WIN_ONLY,  EP_SITE_RECNO,    EP_NONE,         ET_OPTIONAL,   "Get ancient scroll" },
	{ E_ACQUIRE_ITEM,                                -1, EW_WIN_ONLY,  EP_ITEM_ID,       EP_NONE,         ET_OPTIONAL,   "Acquire a certain item" },
	{ E_NATION_SURVIVE, 										 -1, EW_WIN_LOSE,  EP_NONE,          EP_0_100,        ET_NONE,       "Your kingdom survives" },
	// ######### begin Gilbert 2/3 ########//
	{ E_ACQUIRE_TECH, 										 -1, EW_WIN_ONLY,  EP_TECH_ID,       EP_NONE,         ET_OPTIONAL,   "Acquire technology" },
	// ######### end Gilbert 2/3 ########//
	{ E_CONQUER_ALL_FIRM,                            -1, EW_WIN_ONLY,  EP_NONE,          EP_NONE,         ET_OPTIONAL,   "Conquer all firms" },
	{ E_CONQUER_FIRM,                                -1, EW_WIN_LOSE,  EP_FIRM_RECNO,    EP_NONE,         ET_OPTIONAL,   "Conquer a certain firm" },

};

//------ Begin of function EndCondition::init ---------//
//
// <int> condId 	  - game end condition id.
// <int> playerOnly - whether this condition only applies to the player
// [int] p1     	  - parameter 1
// [int] p2     	  - parameter 2
// [int] yearLimit  - year limit
//
void EndCondition::init( int condId, int playerOnly, int p1, int p2, int yearLimit, int loseCond )
{
	condition_id = condId;
	player_only = playerOnly;
	para1 = p1;
	para2 = p2;
	time_limit = yearLimit;

	para1_unique_id = 0;
	para1_unit_killed = 0;

	cond_str[0] = '\0';
	result_str[0] = '\0';

	// win condition
	err_when( !loseCond && !(EndConditionInfo::end_condition_info[condition_id].win_or_lose & EW_WIN_ONLY) );
	// loss condition
	err_when( loseCond && !(EndConditionInfo::end_condition_info[condition_id].win_or_lose & EW_LOSE_ONLY) );

	set_condition_str( cond_str, loseCond );
	set_result_str( result_str, loseCond );

	// ------- set unique unit id ------//

	switch( EndConditionInfo::end_condition_info[condition_id].para1_type )
	{
	case EP_UNIT_RECNO:
	case EP_FRIEND_UNIT_RECNO:
	case EP_ENEMY_UNIT_RECNO:
		if( unit_array.is_deleted(para1) )
			para1_unit_killed = 1;
		else
			para1_unique_id = unit_array[para1]->unique_id;
		break;
	}
}
//------ End of function EndCondition::init ---------//


//------ Begin of function EndCondition::set_condition_str ---------//

void EndCondition::set_condition_str(char *s, int loseCond)
{
	String str;

//	str = text_end_condition.str_set_condition_str( this, loseCond );

	switch( condition_id )
	{
	case E_DEFEAT_ALL_OTHER_KINGDOMS:		// Defeat all other Kingdoms
		str = text_end_condition.get_format_str( 33, "EC00" );
		break;
	case E_DEFEAT_OWN_KINGDOM:					// Your Kingdom is defeated
		str = text_end_condition.get_format_str( 34, "EC01" );
		break;
	case E_DEFEAT_KINGDOM:						// Defeat <nation name>
		str = text_end_condition.process( text_end_condition.get_format_str( 35, "EC02"),
			nation_array.is_deleted(para1) ? "_" : nation_array[para1]->nation_name() );
		break;
	case E_FORM_ALLY_WITH_KINGDOM:			// Ally with <nation name>
		str = text_end_condition.process( text_end_condition.get_format_str( 36, "EC03"),
			nation_array.is_deleted(para1) ? "_" : nation_array[para1]->nation_name() );
		break;
	case E_DEFEAT_ALL_FRYHTAN:					// Defeat all Fryhtan Lairs
		str = text_end_condition.get_format_str( 37, "EC04" );
		break;
	case E_DEFEAT_EXCEPT_ALLIANCE:			// Defeat all except one allied Kingdom
		str = text_end_condition.get_format_str( 38, "EC05" );
		break;
	case E_REACH_POPULATION:					// Population reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 39, "EC06"),
			para2 );
		break;
	case E_REACH_FRYHTAN:						// Fryhtan population reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 40, "EC07"),
			para2 );
		break;
	case E_REACH_UNIT_COUNT:					// Own <number> of <unit name>(s)
		str = text_end_condition.process( text_end_condition.get_format_str( 41, "EC08"),
			unit_res[para1]->name, para2 );
		break;
	case E_REACH_ECONOMY:						// Economic score reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 42, "EC09"),
			para2 );
		break;
	case E_REACH_MILITARY:						// Military score reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 43, "EC10"),
			para2 );
		break;
	case E_REACH_TOTAL	:						// Total score reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 44, "EC11"),
			para2 );
		break;
	case E_KILL_MONSTER_SCORE:						// Fryhtan battling score reaches <number>
		str = text_end_condition.process( text_end_condition.get_format_str( 45, "EC12"),
			para2 );
		break;
	case E_TRAIN_10_GENERAL:						// Train 10 Generals of Leadership <number> or above
		str = text_end_condition.process( text_end_condition.get_format_str( 46, "EC13"),
			para2 );
		break;
	case E_KILL_UNIT:								// Kill <unit name> of <nation name> / <unit name> of <nation name> get killed
		// BUGHERE : para1 str may no longer exist
		err_when( unit_array.is_truly_deleted(para1) );
		if( !loseCond )
		{
			int nationRecno = unit_array[para1]->nation_recno;
			if( nationRecno == 0 )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 49, "cC14"),
					unit_array[para1]->unit_name() );
			}
			else if( nationRecno == nation_array.player_recno )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 48, "bC14"),
					unit_array[para1]->unit_name(), text_unit.str_nation((~nation_array)->race_id) );
			}
			else
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 47, "aC14"),
					unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
			}
		}
		else
		{
			int nationRecno = unit_array[para1]->nation_recno;
			if( nationRecno == 0 )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 52, "fC14"),
					unit_array[para1]->unit_name() );
			}
			else if( nationRecno == nation_array.player_recno )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 51, "eC14"),
					unit_array[para1]->unit_name(), text_unit.str_nation((~nation_array)->race_id) );
			}
			else
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 50, "dC14"),
					unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
			}
		}
		break;
	case E_KEEP_UNIT_ALIVE:						// Rescue <unit name> of <nation name> / Failed to kill <unit name> of <nation name>
		// BUGHERE : para1 str may no longer exist
		err_when( unit_array.is_truly_deleted(para1) );
		if( !loseCond )
		{
			int nationRecno = unit_array[para1]->nation_recno;
			if( nationRecno == 0 )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 55, "cC15"),
					unit_array[para1]->unit_name() );
			}
			else if( nationRecno == nation_array.player_recno )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 54, "bC15"),
					unit_array[para1]->unit_name(), text_unit.str_nation((~nation_array)->race_id) );
			}
			else
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 53, "aC15"),
					unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
			}
		}
		else
		{
			int nationRecno = unit_array[para1]->nation_recno;
			if( nationRecno == 0 )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 58, "fC15"),
					unit_array[para1]->unit_name() );
			}
			else if( nationRecno == nation_array.player_recno )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 57, "eC15"),
					unit_array[para1]->unit_name(), text_unit.str_nation((~nation_array)->race_id) );
			}
			else
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 56, "dC15"),
					unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
			}
		}
		break;
	case E_MEET_UNIT:					// Meet <unit name> of <nation name>
		// BUGHERE : para1 str may no longer exist
		err_when( unit_array.is_truly_deleted(para1) );
		{
			int nationRecno = unit_array[para1]->nation_recno;
			if( nationRecno == 0 )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 61, "cC16"),
					unit_array[para1]->unit_name() );
			}
			else if( nationRecno == nation_array.player_recno )
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 60, "bC16"),
					unit_array[para1]->unit_name(), text_unit.str_nation((~nation_array)->race_id) );
			}
			else
			{
				str = text_end_condition.process( text_end_condition.get_format_str( 59, "aC16"),
					unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
			}
		}
		break;
	case E_CONQUER_ALL_TOWN:			// Conquer all independent towns
		str = text_end_condition.get_format_str( 62, "EC17" );
		break;
	case E_CONQUER_TOWN:					// Conquer town <town name>
		// BUGHERE : para1 str may no longer exist
		err_when( town_array.is_deleted(para1) );
		str = text_end_condition.process( text_end_condition.get_format_str( 63, "EC18"),
			town_array[para1]->town_name() );
		break;
	case E_DESTROY_FIRM:					// Destroy <building name>
		// BUGHERE : para1 str may no longer exist
		err_when( firm_array.is_deleted(para1) );
		str = text_end_condition.process( text_end_condition.get_format_str( 64, "EC19"),
			firm_array[para1]->firm_name() );
		break;
	case E_GET_SCROLL:					// Acquire ancient scroll
		str = text_end_condition.get_format_str( 65, "EC20" );
		break;
	case E_ACQUIRE_ITEM:					// Acquire item <item name>
		str = text_end_condition.process( text_end_condition.get_format_str( 66, "EC21" ),
			item_res.item_name( para1, 0) );
		break;
	case E_NATION_SURVIVE:				// Your kingdom survives for <number> year(s) / Your kingdom cannot win in <number> year(s)
		if( !loseCond )
		{
			str = text_end_condition.process( text_end_condition.get_format_str( 67, "EC22" ),
				text_unit.str_nation((~nation_array)->race_id), para2 );
		}
		else
		{
			str = text_end_condition.process( text_end_condition.get_format_str( 71, "aC22" ),
				text_unit.str_nation((~nation_array)->race_id), para2 );
		}
		break;
	case E_ACQUIRE_TECH:					// Acquire <technology name> technology
		str = text_end_condition.process( text_end_condition.get_format_str( 68, "EC23" ),
			tech_res[para1]->tech_des() );
		break;
	case E_CONQUER_ALL_FIRM:			// Conquer all firms
		str = text_end_condition.get_format_str( 69, "EC24" );
		break;	
	case E_CONQUER_FIRM:					// Conquer firm <firm name>
		// BUGHERE : para1 str may no longer exist
		err_when( firm_array.is_deleted(para1) );
		str = text_end_condition.process( text_end_condition.get_format_str( 70, "EC25" ),
			firm_array[para1]->firm_name() );
		break;

	default:
		err_here();
	}

	if( time_limit )
	{
		// <condition str> in <time limit> year(s)
		str = text_end_condition.process( text_end_condition.get_format_str( 77, "ECTM" ),
			(char *)str, time_limit );		// cast String type explicitly in variable argument
	}

	strncpy( s, str, CONDITION_STR_LEN );
	s[CONDITION_STR_LEN] = '\0';
}
//------ End of function EndCondition::set_condition_str ---------//


//------ Begin of function EndCondition::is_condition_true ---------//

int EndCondition::is_condition_true(int nationRecno)
{
//	if( player_only && nationRecno != nation_array.player_recno )
//		return 0;

	//---------------------------------------//

	Nation* nationPtr=NULL;

	if( !nation_array.is_deleted(nationRecno) )
		nationPtr = nation_array[nationRecno];

	// ####### begin Gilbert 22/2 #########//

	if( player_only && nationPtr && nationPtr->is_ai() )
		return 0;

	if( time_limit && info.game_date-info.game_start_date > time_limit *365 )
		return 0;
	// ####### end Gilbert 22/2 #########//

	switch( condition_id )
	{
		case E_DEFEAT_ALL_OTHER_KINGDOMS:
			return nationPtr && nation_array.nation_count==1;

		case E_DEFEAT_OWN_KINGDOM:
			return !nationPtr ||
					 ( nationPtr->king_unit_recno==0 &&
					 	!nationPtr->has_king_successor_candidate() );

		case E_DEFEAT_KINGDOM:
			return nation_array.is_deleted(para1);

		case E_FORM_ALLY_WITH_KINGDOM:
			return nationPtr && nationRecno != para1 &&
					 nationPtr->get_relation_status(para1) == RELATION_ALLIANCE;

		case E_DEFEAT_ALL_FRYHTAN:
		{
			// ######## begin Gilbert 24/2 #######//
			// for( int i=nation_array.size() ; i>0 ; i-- )
			//{
			//	if( nation_array.is_deleted(i) )
			//		continue;
			//	if( nation_array[i]->is_monster() )
			//		return 0;
			//}
			int i;

			// find number of independent lair, quick method

			FirmInfo *firmInfo = firm_res[FIRM_LAIR];
			int firmCount = firmInfo->total_firm_count;
			for( i = 1; i <= MAX_NATION; ++i )
				firmCount -= firmInfo->get_nation_firm_count(i);

			if( firmCount > 0 )
				return 0;

			// find number of indepedent lair, slow method
			for( i = firm_array.size(); i>0; i-- )
			{
				if( firm_array.is_deleted(i) )
					continue;
				Firm *firmPtr = firm_array[i];
				if( firmPtr->nation_recno == 0 && firmPtr->firm_id == FIRM_LAIR )			// independent lair
					return 0;
			}

			// consider win if the player get the highest score

			int highScore = 0;
			for( i = 1 ; i <= nation_array.size(); ++i )
			{
				if( nation_array.is_deleted(i) )
					continue;
				if( (int)nation_array[i]->kill_monster_score > highScore )
					highScore = (int)nation_array[i]->kill_monster_score;
			}

			// if two have same score, also win
			return nationPtr && nationPtr->kill_monster_score >= highScore;
		}

		case E_DEFEAT_EXCEPT_ALLIANCE:
			{
				if( !nationPtr )
					return 0;

				int nonAllyCount = 0;
				int allyCount = 0;
				for( int n2 = 1; n2 <= nation_array.size(); ++n2 )
				{
					if( n2 == nationRecno || nation_array.is_deleted(n2) )
						continue;

					if( nationPtr->get_relation_status(n2) == RELATION_ALLIANCE )
						++allyCount;
					else
						++nonAllyCount;
				}

				return nonAllyCount == 0 && allyCount == 1;
			}
			return 0;

		case E_REACH_POPULATION:
			// ### begin Gilbert 22/2 #######//
			return nationPtr && info.get_rank_score(1, nationRecno) >= para2;
			// ### end Gilbert 22/2 #######//

		case E_REACH_FRYHTAN:
			// ### begin Gilbert 22/2 #######//
			// return nationPtr && nationPtr->kill_monster_score >= para2;
			return nationPtr && nationPtr->total_monster_count >= para2;
			// ### end Gilbert 22/2 #######//

		case E_REACH_UNIT_COUNT:
			return nationPtr &&
					 unit_res[para1]->nation_unit_count_array[nationRecno-1] +
			// ####### begin Gilbert 22/2 ######//
					 unit_res[para1]->nation_general_count_array[nationRecno-1] >= para2;
			// ####### end Gilbert 22/2 ######//

		case E_REACH_ECONOMY:
			// ####### begin Gilbert 22/2 ######//
			return nationPtr && info.get_rank_score(3, nationRecno) >= para2;
			// ####### end Gilbert 22/2 ######//

		case E_REACH_MILITARY:
			// ####### begin Gilbert 22/2 ######//
			return nationPtr && info.get_rank_score(2, nationRecno) >= para2;
			// ####### end Gilbert 22/2 ######//

		case E_REACH_TOTAL:
			// ####### begin Gilbert 10/6 ########//
//			return nationPtr && nationPtr->overall_rating >= para2;
			if( !nationPtr )
				return 0;
			else
			{
				int t = 0;
				for( int rankType = 1; rankType <= 5; ++rankType )
				{
					t += info.get_rank_score(rankType, nationRecno);
				}
				return t >= para2;
			}
			// ####### end Gilbert 10/6 ########//

		case E_KILL_MONSTER_SCORE:
			// ####### begin Gilbert 22/2 ######//
		//	return nationPtr && nationPtr->kill_monster_score >= para2;
			return nationPtr && info.get_rank_score(5, nationRecno) >= para2;
			// ####### end Gilbert 22/2 ######//

		case E_TRAIN_10_GENERAL:
			if( nationPtr )
			{
				int count = 0;
				for( int i = nationPtr->ai_general_count; i > 0; --i )
				{
					if( unit_array.is_deleted(nationPtr->ai_general_array[i-1]) )
						continue;

					Unit *unitPtr = unit_array[nationPtr->ai_general_array[i-1]];

					// reject king
					if( unitPtr->rank_id == RANK_KING || unitPtr->skill_level() < para2 )
						continue;

					if( ++count >= 10 )
						return 1;
				}
			}
			return 0;

		case E_KILL_UNIT:
			return para1_unit_killed;

		case E_KEEP_UNIT_ALIVE:
			err_when( !time_limit );
			if( info.game_date-info.game_start_date == time_limit *365
				&& !para1_unit_killed )
				return 1;
			return 0;

		case E_MEET_UNIT:					// Meet <unit name> of <nation name>
			// BUGHERE : cannot handle a unit mobilized from a camp
			if( !unit_array.is_deleted(para1) )
			{
				Unit *unitPtr = unit_array[para1];
				if( unitPtr->unique_id == para1_unique_id && unitPtr->is_visible() )
				{
					int scanX1 = unitPtr->obj_loc_x1() - MEET_UNIT_RANGE;
					scanX1 = max(scanX1, 0);
					int scanY1 = unitPtr->obj_loc_y1() - MEET_UNIT_RANGE;
					scanY1 = max(scanY1, 0);
					int scanX2 = unitPtr->obj_loc_x2() + MEET_UNIT_RANGE;
					scanX2 = min(scanX2, MAX_WORLD_X_LOC-1);
					int scanY2 = unitPtr->obj_loc_y2() + MEET_UNIT_RANGE;
					scanY2 = min(scanY2, MAX_WORLD_Y_LOC-1);

					for( int y = scanY1; y <= scanY2; ++y )
					{
						Location *locPtr = world.get_loc( scanX1, y );
						for( int x = scanX1; x <= scanX2; ++x, ++locPtr )
						{
							int touchUnitRecno;
							if( (touchUnitRecno = locPtr->unit_recno(UNIT_AIR))
								&& unit_array[touchUnitRecno]->nation_recno == nationRecno	// avoid two nation win together
								|| (touchUnitRecno = locPtr->unit_recno(UNIT_LAND))
								&& unit_array[touchUnitRecno]->nation_recno == nationRecno 
								|| (touchUnitRecno = locPtr->unit_recno(UNIT_SEA))
								&& unit_array[touchUnitRecno]->nation_recno == nationRecno )
							{
								return 1;
							}
						}
					}
				}
			}
			return 0;

		case E_CONQUER_ALL_TOWN:			// Conquer all independent towns
			{
				for( int townRecno = town_array.size(); townRecno > 0; --townRecno )
				{
					// if there exist any non-deleted independent towns which is not salve town, 
					// return FALSE
					if( !town_array.is_deleted(townRecno) && town_array[townRecno]->nation_recno == 0 &&
						!town_array[townRecno]->is_pay_tribute_to_monster() )
						return 0;
				}
			}
			return 1;
			break;

		case E_CONQUER_TOWN:					// Conquer town <town name>
			if( town_array.is_deleted(para1) )
				para1 = 0;				// clear para1 so any new town of the same town recno will never meet the condition
			else
				return (town_array[para1]->nation_recno == nationRecno ||
							town_array[para1]->is_pay_tribute_to_monster(nationRecno));
			break;

		case E_DESTROY_FIRM:					// Destroy <building name>
			if( firm_array.is_deleted(para1) )
			{
				para1 = 0;			// clear it so is_deleted array return true
				return 1;
			}
			break;

		case E_GET_SCROLL:					// Acquire ancient scroll
			if( site_array.is_deleted(para1) )
				para1 = 0;				// clear para1 so any new site of same recno will never meet the condition
			return para1_nation_took == nationRecno;
			break;

		case E_ACQUIRE_ITEM:					// Acquire item <item name>
			return para1_nation_took == nationRecno;
			break;

		case E_NATION_SURVIVE:
		// BUGHERE : not ready
			return nationPtr &&
					 info.game_date-info.game_start_date > para2*365;

		// ####### begin Gilbert 2/3 ######//
		case E_ACQUIRE_TECH:
			return tech_res[para1]->get_nation_tech_level(nationRecno) > 0;
		// ##### end gilbert 2/3 #########//

		case E_CONQUER_ALL_FIRM:			// Conquer all independent firms
			{
				for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
				{
					// if there exist any non-deleted firm which does not belong to the player, 
					if( !firm_array.is_deleted(firmRecno) && firm_array[firmRecno]->nation_recno != nationRecno )
						return 0;
				}
			}
			return 1;
			break;

		case E_CONQUER_FIRM:					// Conquer firm <firm name>
			if( firm_array.is_deleted(para1) )
				para1 = 0;				// clear para1 so any new firm of the same town recno will never meet the condition
			else
				return (firm_array[para1]->nation_recno == nationRecno);
			break;

		default:
			err_here();
	}

	return 0;
}
//------ End of function EndCondition::is_condition_true ---------//


//------ Begin of function EndCondition::set_result_str ---------//

void EndCondition::set_result_str(char *s, int loseCond)
{
	String str;

	str = "";

	// assume str will be printed after "Your Kingdom" or winner/loser nation name

//	if( nationRecno == nation_array.player_recno )
//		str = translate.process("Your Kingdom");
//	else
//		str = nation_array[nationRecno]->nation_name();
//	str += " ";

	//---------------------------------------//

	// even if non parameter, still process the format string to reduce %%1s to %1s
	switch( condition_id )
	{
		case E_DEFEAT_ALL_OTHER_KINGDOMS:		// has Defeated All Other Kingdoms
			str += text_end_condition.process( text_end_condition.get_format_str( 78, "ER00" ));
			break;
		case E_DEFEAT_OWN_KINGDOM:					// has been Defeated
			str += text_end_condition.process( text_end_condition.get_format_str( 79, "ER01" ));
			break;
		case E_DEFEAT_KINGDOM:						// has Defeated <nation name>
			str += text_end_condition.process( text_end_condition.get_format_str( 80, "ER02" ),
				nation_array[para1]->nation_name() );
			break;
		case E_FORM_ALLY_WITH_KINGDOM:			// has Allied with <nation name>
			str += text_end_condition.process( text_end_condition.get_format_str( 81, "ER03" ),
				nation_array[para1]->nation_name() );
			break;
		case E_DEFEAT_ALL_FRYHTAN:					// has Defeated all Fryhtan Lairs
			str += text_end_condition.process( text_end_condition.get_format_str( 82, "ER04" ));
			break;
		case E_DEFEAT_EXCEPT_ALLIANCE:			// has Defeated all except one Allied Kingdom
			str += text_end_condition.process( text_end_condition.get_format_str( 83, "ER05" ));
			break;
		case E_REACH_POPULATION:					// reaches Population of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 84, "ER06" ),
				para2 );
			break;
		case E_REACH_FRYHTAN:						// reaches Fryhtan Population of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 85, "ER07" ),
				para2 );
			break;
		case E_REACH_UNIT_COUNT:					// owns <number> <unit name>(s)
			str += text_end_condition.process( text_end_condition.get_format_str( 86, "ER08" ),
				unit_res[para1]->name, para2 );
			break;
		case E_REACH_ECONOMY:						// reaches Economy Score of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 87, "ER09" ),
				para2 );
			break;
		case E_REACH_MILITARY:						// reaches Military Score of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 88, "ER10" ),
				para2 );
			break;
		case E_REACH_TOTAL:							// reaches Total Score of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 89, "ER11" ),
				para2 );
			break;
		case E_KILL_MONSTER_SCORE:					// reaches Fryhtan Battling Score of <number>
			str += text_end_condition.process( text_end_condition.get_format_str( 90, "ER12" ),
				para2 );
			break;
		case E_TRAIN_10_GENERAL:					// has Trained 10 Generals of Leadership <number> or above
			str += text_end_condition.process( text_end_condition.get_format_str( 91, "ER13" ),
				para2 );
			break;
		case E_KILL_UNIT:								// has killed <unit name> of <nation name> / failed to protect <unit name> of <nation name>
			// BUGHERE : para1 str may no longer exist
			if( unit_array.is_truly_deleted(para1) )
			{
				err_here();
				return;
			}
			if( !loseCond )
			{
				int nationRecno = unit_array[para1]->nation_recno;
				if( nationRecno == 0 )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 94, "cR14" ),
						unit_array[para1]->unit_name() );
				}
				else if( nationRecno == nation_array.player_recno )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 93, "bR14" ),
						unit_array[para1]->unit_name(), text_unit.str_nation( (~nation_array)->race_id )  );
				}
				else
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 92, "aR14" ),
						unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
				}
			}
			else
			{
				int nationRecno = unit_array[para1]->nation_recno;
				if( nationRecno == 0 )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 97, "fR14" ),
						unit_array[para1]->unit_name() );
				}
				else if( nationRecno == nation_array.player_recno )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 96, "eR14" ),
						unit_array[para1]->unit_name(), text_unit.str_nation( (~nation_array)->race_id )  );
				}
				else
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 95, "dR14" ),
						unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
				}
			}
			break;
		case E_KEEP_UNIT_ALIVE:						// has Protected <unit name> of <nation name> / has Failed to Kill <unit name> of <nation name>
			// BUGHERE : para1 str may no longer exist
			if( unit_array.is_truly_deleted(para1) )
			{
				err_here();
				return;
			}
			if( !loseCond )
			{
				int nationRecno = unit_array[para1]->nation_recno;
				if( nationRecno == 0 )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 100, "cR15" ),
						unit_array[para1]->unit_name() );
				}
				else if( nationRecno == nation_array.player_recno )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 99, "bR15" ),
						unit_array[para1]->unit_name(), text_unit.str_nation( (~nation_array)->race_id )  );
				}
				else
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 98, "aR15" ),
						unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
				}
			}
			else
			{
				int nationRecno = unit_array[para1]->nation_recno;
				if( nationRecno == 0 )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 103, "fR15" ),
						unit_array[para1]->unit_name() );
				}
				else if( nationRecno == nation_array.player_recno )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 102, "eR15" ),
						unit_array[para1]->unit_name(), text_unit.str_nation( (~nation_array)->race_id )  );
				}
				else
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 101, "dR15" ),
						unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
				}
			}
			break;
		case E_MEET_UNIT:					// has Met <unit name> of <nation name>
			// BUGHERE : para1 str may no longer exist
			if( unit_array.is_truly_deleted(para1) )
			{
				err_here();
				return;
			}
			{
				int nationRecno = unit_array[para1]->nation_recno;
				if( nationRecno == 0 )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 106, "cR16" ),
						unit_array[para1]->unit_name() );
				}
				else if( nationRecno == nation_array.player_recno )
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 105, "bR16" ),
						unit_array[para1]->unit_name(), text_unit.str_nation( (~nation_array)->race_id )  );
				}
				else
				{
					str += text_end_condition.process( text_end_condition.get_format_str( 104, "aR16" ),
						unit_array[para1]->unit_name(), nation_array[nationRecno]->nation_name() );
				}
			}
			break;
		case E_CONQUER_ALL_TOWN:			// has Conquered All Independent Towns
			str += text_end_condition.process( text_end_condition.get_format_str( 107, "ER17" ));
			break;
		case E_CONQUER_TOWN:					// has Conquered Town <town name>
			// BUGHERE : para1 str may no longer exist
			if( town_array.is_deleted(para1) )
			{
				err_here();
				return;
			}
			str += text_end_condition.process( text_end_condition.get_format_str( 108, "ER18" ),
				town_array[para1]->town_name() );
			break;
		case E_DESTROY_FIRM:					// has Destroyed <building name>
			// BUGHERE : para1 str may no longer exist
			if( firm_array.is_deleted(para1) )
			{
				err_here();
				return;
			}
			str += text_end_condition.process( text_end_condition.get_format_str( 109, "ER19" ),
				firm_array[para1]->firm_name() );
			break;
		case E_GET_SCROLL:					// Acquire the Ancient Scroll
			str += text_end_condition.process( text_end_condition.get_format_str( 110, "ER20" ));
			break;
		case E_ACQUIRE_ITEM:					// has Acquired Item <item name>
			str += text_end_condition.process( text_end_condition.get_format_str( 111, "ER21" ),
				item_res.item_name( para1, 0) );
			break;
		case E_NATION_SURVIVE:				// has Survived for <number> Year(s) / cannot Win in <number> Year(s)
			if( !loseCond )
			{
				str += text_end_condition.process( text_end_condition.get_format_str( 112, "ER22" ),
					para2 );
			}
			else
			{
				str += text_end_condition.process( text_end_condition.get_format_str( 116, "aR22" ),
					para2 );
			}
			break;
		case E_ACQUIRE_TECH:
			str += text_end_condition.process( text_end_condition.get_format_str( 113, "ER23" ),
				tech_res[para1]->tech_des() );
			break;
		case E_CONQUER_ALL_FIRM:			// has Conquered All Firms
			str += text_end_condition.process( text_end_condition.get_format_str( 114, "ER24" ));
			break;
		case E_CONQUER_FIRM:					// has Conquered Firm <firm name>
			// BUGHERE : para1 str may no longer exist
			if( firm_array.is_deleted(para1) )
			{
				err_here();
				return;
			}
			str += text_end_condition.process( text_end_condition.get_format_str( 115, "ER25" ),
				firm_array[para1]->firm_name() );
			break;

		default:
			err_here();
	}

	strncpy( s, str, CONDITION_STR_LEN );
	s[CONDITION_STR_LEN] = '\0';
}
//------ End of function EndCondition::set_result_str ---------//


// ----- begin of function EndCondition::update_condition_on_killed -------//
//
int EndCondition::update_condition_on_killed(long uniqueId)
{
	// ------- set unique unit id ------//

	switch( EndConditionInfo::end_condition_info[condition_id].para1_type )
	{
	case EP_UNIT_RECNO:
	case EP_FRIEND_UNIT_RECNO:
	case EP_ENEMY_UNIT_RECNO:
		if( para1_unique_id == uniqueId )
		{
			para1_unit_killed = 1;
			return 1;
		}
		break;
	}

	return 0;
}
// ----- end of function EndCondition::update_condition_on_killed -------//


// ----- begin of function EndCondition::update_condition_on_mobilize -------//
//
int EndCondition::update_condition_on_mobilize(int newUnitRecno, long uniqueId)
{
	// ------- set unique unit id ------//

	switch( EndConditionInfo::end_condition_info[condition_id].para1_type )
	{
	case EP_UNIT_RECNO:
	case EP_FRIEND_UNIT_RECNO:
	case EP_ENEMY_UNIT_RECNO:
		if( para1_unique_id == uniqueId && !para1_unit_killed )
		{
			para1 = newUnitRecno;
			return 1;
		}
		break;
	}

	return 0;
}
// ----- end of function EndCondition::update_condition_on_mobilize -------//


// ----- begin of function EndCondition::update_condition_site_taken -------//
//
int EndCondition::update_condition_site_taken(int siteRecno, int nationRecno )
{
	// ------- set unique unit id ------//

	if( EndConditionInfo::end_condition_info[condition_id].para1_type == EP_SITE_RECNO
		&& para1 == siteRecno
		&& !para1_nation_took )
	{
		para1_nation_took = nationRecno;
		return 1;
	}

	return 0;
}
// ----- end of function EndCondition::update_condition_site_taken -------//


// ----- begin of function EndCondition::update_condition_item_acquired -------//
//
int EndCondition::update_condition_item_acquired(short itemId, int nationRecno )
{
	// ------- set unique unit id ------//

	if( EndConditionInfo::end_condition_info[condition_id].para1_type == EP_ITEM_ID
		&& para1 == itemId
		&& !para1_nation_took )
	{
		para1_nation_took = nationRecno;
		return 1;
	}

	return 0;
}
// ----- end of function EndCondition::update_condition_site_taken -------//


// ----- begin of function EndCondition::is_unit_in_condition -------//
//
int EndCondition::is_unit_in_condition(long uniqueId)
{
	// ------- set unique unit id ------//

	switch( EndConditionInfo::end_condition_info[condition_id].para1_type )
	{
	case EP_UNIT_RECNO:
	case EP_FRIEND_UNIT_RECNO:
	case EP_ENEMY_UNIT_RECNO:
		if( para1_unique_id == uniqueId )
		{
			return 1;
		}
		break;
	}

	return 0;
}
// ----- end of function EndCondition::is_unit_in_condition -------//


// ----- begin of function EndConditionInfo::name -------//
//
char *EndConditionInfo::name()
{
	return text_end_condition.str_end_condition_name( condition_id );
}
// ----- end of function EndConditionInfo::name -------//


// ----- begin of function EndCondition::update_strings -------//
//
void EndCondition::update_strings(int loseCond)
{
	set_condition_str( cond_str, loseCond );
	set_result_str( result_str, loseCond );
}
// ----- end of function EndCondition::update_strings -------//

