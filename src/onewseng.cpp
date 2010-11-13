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

//Filename    : ONEWS3.CPP
//Description : Object news msg generating routines

// #if( !defined(GERMAN) && !defined(FRENCH) && !defined(SPANISH) )

#include <ostr.h>
#include <otown.h>
#include <ofirmres.h>
#include <onation.h>
#include <ounit.h>
#include <ospy.h>
#include <ofont.h>
#include <otech.h>
#include <otechres.h>
#include <omonsres.h>
#include <oremote.h>
#include <ogodres.h>
#include <oraceres.h>
#include <otalkres.h>
#include <onews.h>
#include <ot_news.h>
#include <ot_unit.h>

//--------- Define variable type ----------//

typedef void (News::*NewsFP)();

//-------- Define struct NewsInfo -------//

struct NewsInfo
{
	NewsFP news_function_ptr;
	char	 is_major;					// whether this is a major news or not
};

//------ Define function pointers array ------//

static NewsInfo news_info_array[] =
{
	{ &News::diplomacy, 1 },
	{ &News::town_rebel, 1 },
	{ &News::migrate, 0 },
	{ &News::new_nation, 1 },
	{ &News::nation_destroyed, 1 },
	{ &News::nation_surrender, 1 },
	{ &News::king_die, 1 },
	{ &News::new_king, 1 },
	{ &News::firm_destroyed, 0 },
	{ &News::firm_attacked, 0 },
	{ &News::firm_captured, 0 },
	{ &News::town_destroyed, 0 },
	{ &News::town_attacked, 0 },
	{ &News::town_abandoned, 0 },
	{ &News::town_surrendered, 0 },

	{ &News::firm_idle, 0 },
	
	{ &News::monster_king_killed, 0 },
	{ &News::monster_firm_destroyed, 0 },
	{ &News::scroll_acquired, 1 },
	{ &News::monster_gold_acquired, 0 },
	{ &News::your_spy_killed, 1 },
	{ &News::enemy_spy_killed, 1 },
	{ &News::unit_betray, 1 },
	{ &News::unit_assassinated, 1 },
   { &News::assassinator_caught, 1 },
	{ &News::general_die, 1 },
	{ &News::general_form_new_nation, 1 },
	{ &News::raw_exhaust, 1 },
	{ &News::tech_researched, 1 },
	{ &News::lightning_damage, 1 },
	{ &News::earthquake_damage, 1 },
	{ &News::goal_deadline, 1 },
	{ &News::weapon_ship_worn_out, 0 },
	{ &News::firm_worn_out, 0 },
	{ &News::event_finish_tech, 1 },
	{ &News::event_new_tech, 1 },
	{ &News::event_food_destroyed, 1 },
	{ &News::event_unit_die, 1 },
	{ &News::event_soldier_die, 1 },
	{ &News::event_item_found, 1 },
	{ &News::event_town_people_killed, 1 },
	{ &News::event_factory_destroyed, 1 },
	{ &News::event_mine_destroyed, 1 },
	{ &News::event_inn_destroyed, 1 },
	{ &News::tornado_hit, 0 },
	{ &News::blueprint_acquired, 1 },
	// ##### begin Gilbert 16/4 #####//
	{ &News::magic_tower_attack, 1 },
	// ##### end Gilbert 16/4 #####//
	{ &News::chat_msg, 1 },
	{ &News::multi_retire, 1 },
	{ &News::multi_quit_game, 1 },
	{ &News::multi_save_game, 1 },
	{ &News::multi_connection_lost, 1 },
};

//------- Define static variables --------//

static String str;

//------ Begin of function News::msg -----//
//
// Return the msg string of current news.
//
char* News::msg()
{
	NewsFP newsFP = news_info_array[id-1].news_function_ptr;

	(this->*newsFP)();   // call the corrsponding function to return the news msg

	return str;
}
//------- End of function News::msg -----//


//------ Begin of function News::is_major -----//
//
// Whether this is a major news or not.
//
int News::is_major()
{
	if( id==NEWS_TOWN_REBEL )
	{
		//-- only rebellions happening in the player's town are considered as major news --//

		return nation_array.player_recno &&
				 nation_recno1 == nation_array.player_recno;
	}
	else
	{
		return news_info_array[id-1].is_major;
	}
}
//------- End of function News::is_major -----//


//------ Begin of function News::diplomacy -----//
//
// Diplomatic messages from other nations.
//
// short_para1 = the recno of TalkMsg in talk_res.talk_msg_array.
//
void News::diplomacy()
{
	err_when( talk_res.is_talk_msg_deleted(short_para1) );

	TalkMsg* talkMsgPtr = talk_res.get_talk_msg(short_para1);

	str = talkMsgPtr->msg_str(nation_array.player_recno);
}
//------- End of function News::diplomacy -----//


//------ Begin of function News::town_rebel -----//
//
// Generate a news that a firm is built
//
// nation_name1() - the name of the nation the rebel took place.
//
// short_para1 = the town name id.
// short_para2 = no. of rebels
//
void News::town_rebel()
{
	//---------------- Text Format -----------------//
	//
	// <no. of rebel> Peasants in <town name> in
	// <nation name> is/are rebelling.
	//
	//----------------------------------------------//

	err_when( short_para2 < 1 );
/*
	str = short_para2;

	if( short_para2 == 1 )
		str += " Peasant in ";
	else
		str += " Peasants in ";

	str += town_res.get_name(short_para1);
	str += " in ";
	str += nation_name1();

	if( short_para2 > 1 )
		str += " are rebelling.";
	else
		str += " is rebelling.";
*/
	str = text_news.process( text_news.get_format_str( 11, "TRBL" ),
		town_res.get_name(short_para1), short_para2, nation_name1() );
}
//------- End of function News::town_rebel -----//


//------ Begin of function News::migrate -------//
//
// nation_name1() - from nation
// nation_name2() - to nation
//
// short_para1 = the town name id. that the worker migrates from
// short_para2 = the town name id. that the worker migrates to
// short_para3 = race id. of the migrated worker/peasant
// short_para4 = no. of people migrated
// short_para5 = the firm id. that the worker works for
//
// (not used)
//
void News::migrate()
{
	//---------------- Text Format -----------------//
	//
	// A <racial> <worker name>/Peasant has emigrated from your
	// town of <town name> to <town name> in <nation name>.
	//
	// A <racial> <worker name>/Peasant has immigrated from
	// <town name> in <nation name> to your town of <town name>.
	//
	//----------------------------------------------//
/*
	if( short_para4 == 1 )
		str = "A";
	else
		str = m.format(short_para4);

	str += " ";
	str += race_res[short_para3]->name;
	str += " ";

	if( short_para5 )
		str += firm_res[short_para5]->worker_title;
	else
		str += "Peasant";

	if( short_para4 == 1 )
		str += " has";
	else
		str += "s have";

	//------------------------------------//

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )		// from player nation to another nation
	{
		str += " emigrated from your town of ";
		str += town_res.get_name(short_para1);
		str += " to ";
		str += town_res.get_name(short_para2);

		if( nation_recno2 )		// only if it is not an independent town
		{
			str += " in ";
			str += nation_name2();
		}

		str += ".";
	}
	else
	{
		str += " immigrated from ";
		str += town_res.get_name(short_para1);

		if( nation_recno1 )
		{
			str += " in ";
			str += nation_name1();
		}

		str += " to your town of ";
		str += town_res.get_name(short_para2);
		str += ".";
	}
*/

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		String toTownName;
		if( nation_recno2 )
			toTownName = text_news.str_town_of_nation( town_res.get_name(short_para2), nation_name2() );
		else
			toTownName = town_res.get_name(short_para2);

		str = text_news.process( text_news.get_format_str( 12, "MIGE"), town_res.get_name(short_para1), (char *)toTownName,
			race_res[short_para3]->name, short_para4, firm_res[short_para5]->worker_title );
	}
	else
	{
		String fromTownName;
		if( nation_recno1 )
			fromTownName = text_news.str_town_of_nation( town_res.get_name(short_para1), nation_name1() );
		else
			fromTownName = town_res.get_name(short_para1);
		str = text_news.process( text_news.get_format_str( 13, "MIGI"), (char *)fromTownName, town_res.get_name(short_para2),
			race_res[short_para3]->name, short_para4, firm_res[short_para5]->worker_title );
	}
}
//------- End of function News::migrate --------//


//------ Begin of function News::new_nation -----//
//
// king_name1() - name of the king of the new kingdom.
//
void News::new_nation()
{
	//---------------- Text Format -----------------//
	//
	// A new Kingdom has emerged under the leadership of <king Name>.
	//
	//----------------------------------------------//

//	str  = "A new Kingdom has emerged under the leadership of ";
//	str += king_name1(1);
//	str += ".";

	str = text_news.process( text_news.get_format_str( 14, "NEWK" ),
		king_name1(1) );
}
//------- End of function News::new_nation -----//


//------ Begin of function News::nation_destroyed -----//
//
// nation_name1() - name of the destroyed nation.
//
void News::nation_destroyed()
{
	//---------------- Text Format -----------------//
	//
	// <King>'s Kingdom has been destroyed.
	//
	//----------------------------------------------//

//	str  = nation_name1();
//	str += " has been destroyed.";

	str  = text_news.process( text_news.get_format_str( 15, "DE_K" ),
		nation_name1() );
}
//------- End of function News::nation_destroyed -----//


//------ Begin of function News::nation_surrender -----//
//
// nation_name1() - name of the surrendering nation.
// nation_name2() - name of the nation to surrender.
//
void News::nation_surrender()
{
	//---------------- Text Format -----------------//
	//
	// <King>'s Kingdom has surrendered to <King B>'s Kingdom.
	//
	// <King>'s Kingdom has surrendered to you.
	//
	//----------------------------------------------//

//	str  = nation_name1();
//	str += " has surrendered to ";
//	if( nation_array.player_recno && nation_recno2 == nation_array.player_recno )
//	{
//		str += "you.";
//	}
//	else
//	{
//		str += nation_name2();
//		str += ".";
//	}

	if( nation_array.player_recno && nation_recno2 == nation_array.player_recno )
		str = text_news.process( text_news.get_format_str( 16, "SURY" ),
			nation_name1() );
	else
		str = text_news.process( text_news.get_format_str( 17, "SURO" ),
			nation_name1(), nation_name2() );
}
//------- End of function News::nation_surrender -----//


//------ Begin of function News::king_die -----//
//
// king_name1() - the nation whose king has died.
//
void News::king_die()
{
	//---------------- Text Format -----------------//
	//
	// Your King/All High, <king name>, has been slain.
	//
	// OR
	//
	// King/All High <king name> of <nation name> has been slain.
	//
	//----------------------------------------------//

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
//		if( nation_race_id1 >= 0 )
//			str  = "Your King, ";
//		else
//			str  = "Your All High, ";
//		str += king_name1();
//		str += ", has been slain.";

		str = text_news.process( text_news.get_format_str( 18, "YDIE" ),
			text_unit.str_title(nation_race_id1, RANK_KING), king_name1() );
	}
	else
	{
//		if( nation_race_id1 >= 0 )
//			str  = "King ";
//		else
//			str  = "All High ";
//		str += king_name1();
//		str += " of ";
//		str += nation_name1();
//		str += " has been slain.";

		str = text_news.process( text_news.get_format_str( 19, "KDIE" ),
			text_unit.str_title(nation_race_id1, RANK_KING), king_name1(), nation_name1() );
	}
}
//------- End of function News::king_die -----//


//------ Begin of function News::new_king -----//
//
// nation_name1() - name of the nation where there is a new king.
//
// short_para1 - race id. of the new king.
// short_para2 - name id. of the new king.
//
void News::new_king()
{
	//---------------- Text Format -----------------//
	//
	// <unit name> has ascended the throne as your new King/All High.
	//
	// OR
	//
	// <unit name> has ascended the throne as the new King/All High of
	// <nation name>.
	//
	//----------------------------------------------//

//	str = get_unit_name(short_para1, short_para2);
	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
//		if( short_para1 >= 0 )
//			str += " has ascended the throne as your new King.";
//		else
//			str += " has ascended the throne as your new All High.";

		str = text_news.process( text_news.get_format_str( 20, "YKNG" ),
			get_unit_name(short_para1, short_para2), text_unit.str_title(short_para1, RANK_KING));
	}
	else
	{
//		if( short_para1 >= 0 )
//			str += " has ascended the throne as the new King of ";
//		else
//			str += " has ascended the throne as the new All High of ";
//		str += nation_name1();
//		str += ".";

		str = text_news.process( text_news.get_format_str( 21, "KKNG" ),
			get_unit_name(short_para1, short_para2), text_unit.str_title(short_para1, RANK_KING),
			nation_name1() );
	}
}
//------- End of function News::new_king -----//


//------ Begin of function News::firm_destroyed -----//
//
// short_para1 - build id. of the firm destroyed.
// short_para2 - id of the town where the firm is located.
// short_para3 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void News::firm_destroyed()
{
	//---------------- Text Format -----------------//
	//
	// Your <firm type> near <town name> has been destroyed by <kingdom name>.
	// Your <firm type> near <town name> has been destroyed by Rebels.
	// Your <firm type> near <town name> has been destroyed by Fryhtans.
	//
	//----------------------------------------------//
/*
	str  = "Your ";
	str += firm_res.get_build(short_para1)->name;
	if( short_para2 > 0 )			// it is possible that there is no town in the map
	{
		str += " near ";
		str += town_res.get_name(short_para2);
	}
	str += " has been destroyed";
	switch( short_para3 )
	{
		case DESTROYER_NATION:
			str += " by ";
			str += nation_name2();
			str += ".";
			break;

		case DESTROYER_REBEL:
			str += " by Rebels.";
			break;

		case DESTROYER_MONSTER:
			str += " by Fryhtans.";
			break;

		case DESTROYER_UNKNOWN:
			str += ".";
			break;
	}
*/
	String firmPhrase;
	if( short_para2 > 0 )			// it is possible that there is no town in the map
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	switch( short_para3 )
	{
	case DESTROYER_NATION:
		str = text_news.process( text_news.get_format_str( 22, "FDEN" ),
			(char *)firmPhrase, nation_name2() );
		break;
	case DESTROYER_REBEL:
		str = text_news.process( text_news.get_format_str( 23, "FDER" ),
			(char *)firmPhrase );
		break;
	case DESTROYER_MONSTER:
		str = text_news.process( text_news.get_format_str( 24, "FDEF" ),
			(char *)firmPhrase );
		break;
	case DESTROYER_UNKNOWN:
	default:
		str = text_news.process( text_news.get_format_str( 25, "FDE?" ),
			(char *)firmPhrase );
		break;
	}
}
//------- End of function News::firm_destroyed -----//

//------ Begin of function News::firm_attacked -----//
//
// short_para1 - build id. of the firm attacked.
// short_para2 - id of the town where the firm is located.
// short_para3 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void News::firm_attacked()
{
	//---------------- Text Format -----------------//
	//
	// Your <firm type> near <town name> has been attacked by <kingdom name>.
	// Your <firm type> near <town name> has been attacked by Rebels.
	// Your <firm type> near <town name> has been attacked by Fryhtans.
	//
	//----------------------------------------------//
/*
	str  = "Your ";
	str += firm_res.get_build(short_para1)->name;

	if( short_para2 > 0 )			// it is possible that there is no town in the map
	{
		str += " near ";
		str += town_res.get_name(short_para2);
	}

	str += " has been attacked";

	switch( short_para3 )
	{
		case DESTROYER_NATION:
			str += " by ";
			str += nation_name2();
			str += ".";
			break;

		case DESTROYER_REBEL:
			str += " by Rebels.";
			break;

		case DESTROYER_MONSTER:
			str += " by Fryhtans.";
			break;

		case DESTROYER_UNKNOWN:
			str += ".";
			break;
	}
*/
	String firmPhrase;
	if( short_para2 > 0 )			// it is possible that there is no town in the map
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	switch( short_para3 )
	{
	case DESTROYER_NATION:
		str = text_news.process( text_news.get_format_str( 26, "FATN" ),
			(char *)firmPhrase, nation_name2() );
		break;
	case DESTROYER_REBEL:
		str = text_news.process( text_news.get_format_str( 27, "FATR" ),
			(char *)firmPhrase );
		break;
	case DESTROYER_MONSTER:
		str = text_news.process( text_news.get_format_str( 28, "FATF" ),
			(char *)firmPhrase );
		break;
	case DESTROYER_UNKNOWN:
	default:
		str = text_news.process( text_news.get_format_str( 29, "FAT?" ),
			(char *)firmPhrase );
		break;
	}
}
//------- End of function News::firm_attacked -----//

//------ Begin of function News::firm_captured -----//
//
// nation_name2() - name of the nation that took over your firm.
//
// short_para1 - firm build id. of the firm took over
// short_para2 - id of the town where the firm is located.
// short_para3 - whether the capturer of the firm is a spy.
//
void News::firm_captured()
{
	//---------------- Text Format -----------------//
	//
	// Your <firm type> near <town name> has been
	// captured by <kingdom name>.
	//
	// If the capturer is a spy:
	//
	// Your <firm type> near <town name> has been
	// captured by a spy from <kingdom name>.
	//
	//----------------------------------------------//
/*
	str  = "Your ";
	str += firm_res.get_build(short_para1)->name;

	if( short_para2 > 0 )			// it is possible that there is no town in the map
	{
		str += " near ";
		str += town_res.get_name(short_para2);
	}

	str += " has been captured by ";

	if( short_para3 )
		str += "a spy from ";

	str += nation_name2();
	str += ".";
*/

	String firmPhrase;
	if( short_para2 > 0 )			// it is possible that there is no town in the map
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	if( !short_para3 )		// short_para3 = capture by spy
		str = text_news.process( text_news.get_format_str( 30, "FCAN" ),
			(char *)firmPhrase, nation_name2() );
	else
		str = text_news.process( text_news.get_format_str( 31, "FCAS" ),
			(char *)firmPhrase, nation_name2() );
}
//------- End of function News::firm_captured -----//


//------ Begin of function News::town_destroyed -----//
//
// str_para1 - name id. of the town
// short_para2 - destroyer type
//
void News::town_destroyed()
{
	//---------------- Text Format -----------------//
	//
	// Your town of <name name> has been destroyed by <kingdom name>.
	// Your town of <name name> has been destroyed by Rebels.
	// Your town of <name name> has been destroyed by Fryhtans.
	//
	//----------------------------------------------//
/*
	str  = "Your town of ";
//	if( short_para1 )
//	{
		// str += town_res.get_name(short_para1);
		str += str_para1;
//	}
	str += " has been destroyed";

	switch( short_para2 )
	{
		case DESTROYER_NATION:
			str += " by ";
			str += nation_name2();
			str += ".";
			break;

		case DESTROYER_REBEL:
			str += " by Rebels.";
			break;

		case DESTROYER_MONSTER:
			str += " by Fryhtans.";
			break;

		case DESTROYER_UNKNOWN:
			str += ".";
			break;
	}
*/

	switch( short_para2 )
	{
	case DESTROYER_NATION:
		str = text_news.process( text_news.get_format_str( 32, "TDEN" ),
			str_para1, nation_name2() );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_REBEL:
		str = text_news.process( text_news.get_format_str( 33, "TDER" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_MONSTER:
		str = text_news.process( text_news.get_format_str( 34, "TDEF" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_UNKNOWN:
	default:
		str = text_news.process( text_news.get_format_str( 35, "TDE?" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	}
}
//------- End of function News::town_destroyed -----//

//------ Begin of function News::town_attacked -----//
//
// str_para1 - name id. of the town
// short_para2 - destroyer type
//
void News::town_attacked()
{
	//---------------- Text Format -----------------//
	//
	// Your town of <name name> has been attacked by <kingdom name>.
	// Your town of <name name> has been attacked by Rebels.
	// Your town of <name name> has been attacked by Fryhtans.
	//
	//----------------------------------------------//

/*
	str  = "Your town of ";
//	if( short_para1 )
//	{
		// str += town_res.get_name(short_para1);
		str += str_para1;
//	}
	str += " has been attacked";

	switch( short_para2 )
	{
		case DESTROYER_NATION:
			str += " by ";
			str += nation_name2();
			str += ".";
			break;

		case DESTROYER_REBEL:
			str += " by Rebels.";
			break;

		case DESTROYER_MONSTER:
			str += " by Fryhtans.";
			break;

		case DESTROYER_UNKNOWN:
			str += ".";
			break;
	}
*/

	switch( short_para2 )
	{
	case DESTROYER_NATION:
		str = text_news.process( text_news.get_format_str( 36, "TATN" ),
			str_para1, nation_name2() );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_REBEL:
		str = text_news.process( text_news.get_format_str( 37, "TATR" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_MONSTER:
		str = text_news.process( text_news.get_format_str( 38, "TATF" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	case DESTROYER_UNKNOWN:
	default:
		str = text_news.process( text_news.get_format_str( 39, "TAT?" ),
			str_para1 );	// town_res.get_name(short_para1);
		break;
	}
}
//------- End of function News::town_attacked -----//

//------ Begin of function News::town_abandoned -----//
//
// short_para1 - name id of the town
//
// (not used)
//
void News::town_abandoned()
{
	//---------------- Text Format -----------------//
	//
	// Your town of <town name> has been abandoned by
	// its people.
	//
	//----------------------------------------------//

//	str  = "Your town of ";
//	str += town_res.get_name(short_para1);
//	str += " has been abandoned by its people.";

	str = text_news.process( text_news.get_format_str( 40, "TABD" ),
		town_res.get_name(short_para1) );
}
//------- End of function News::town_abandoned -----//


//------ Begin of function News::town_surrendered -----//
//
// short_para1 - name id. of the surrendering town.
//
// nation_name1() - name of the nation the town surrenders to.
// nation_name2() - name of the nation of the surrendering town.
//
void News::town_surrendered()
{
	//---------------- Text Format -----------------//
	//
	// The town of <town name> in <King's Kingdom> has
	// surrendered to you.
	//
	// The independent town of <town name> has
	// surrendered to you.
	//
	// Your town of <town name> has surrendered
	// to <King's Kingdom>.
	//
	// Your town of <town name> has declared
	// independence.
	//
	//----------------------------------------------//
/*
	if( nation_array.player_recno && nation_recno2 == nation_array.player_recno )
	{
		str  = "Your town of ";
		str += town_res.get_name(short_para1);

		if( nation_recno1 )		// your town surrenders to another nation
		{
			str += " has surrendered to ";
			str += nation_name1();
			str += ".";
		}
		else							// your town declares independence
		{
			str += " has declared independence.";
		}
	}
	else
	{
		if( nation_recno2 )
		{
			str  = "The town of ";
			str += town_res.get_name(short_para1);
			str += " in ";
			str += nation_name2();
			str += " has surrendered to you.";
		}
		else // nation_recno2 == 0, it's an independent town
		{
			str  = "The independent town of ";
			str += town_res.get_name(short_para1);
			str += " has surrendered to you.";
		}
	}
*/
	if( nation_array.player_recno && nation_recno2 == nation_array.player_recno )
	{
		if( nation_recno1 )		// your town surrenders to another nation
		{
			str = text_news.process( text_news.get_format_str( 41, "YTSN" ),
				town_res.get_name(short_para1), nation_name1() );
		}
		else							// your town declares independence
		{
			str = text_news.process( text_news.get_format_str( 42, "YTSI" ),
				town_res.get_name(short_para1) );
		}
	}
	else
	{
		if( nation_recno2 )
		{
			str = text_news.process( text_news.get_format_str( 43, "NTSY" ),
				town_res.get_name(short_para1), nation_name2() );
		}
		else // nation_recno2 == 0, it's an independent town
		{
			str = text_news.process( text_news.get_format_str( 44, "ITSY" ),
				town_res.get_name(short_para1) );
		}
	}
}
//------- End of function News::town_surrendered -----//

//------ Begin of function News::firm_idle -----//
//
// short_para1 - build id. of the firm attacked.
// short_para2 - id of the town where the firm is located.
//
void News::firm_idle()
{
	//---------------- Text Format -----------------//
	//
	// Your <firm type> near <town name> is idle.
	//
	//----------------------------------------------//

//	str  = "Your ";
//	str += firm_res.get_build(short_para1)->name;
//	if( short_para2 > 0 )			// it is possible that there is no town in the map
//	{
//		str += " near ";
//		str += town_res.get_name(short_para2);
//	}
//	str += " is idle.";

	String firmPhrase;
	if( short_para2 > 0 )			// it is possible that there is no town in the map
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	str = text_news.process( text_news.get_format_str( 45, "IDLE" ),
		(char *)firmPhrase );
}
//------- End of function News::firm_idle -----//

//------ Begin of function News::monster_king_killed -----//
//
// short_para1 - monster id.
//
// (not used)
//
void News::monster_king_killed()
{
	//---------------- Text Format -----------------//
	//
	// An All High <monster type name> has been slain.
	//
	//----------------------------------------------//

//	str  = "An All High ";
//	str += monster_res[short_para1]->name;
//	str += " has been slain.";

	str  = text_news.process( text_news.get_format_str( 46, "FKDE" ),
		monster_res[short_para1]->name );
}
//------- End of function News::monster_king_killed -----//


//------ Begin of function News::monster_firm_destroyed -----//
//
// short_para1 - monster id.
//
void News::monster_firm_destroyed()
{
	//---------------- Text Format -----------------//
	//
	// An independent <monster type name> Lair has been destroyed.
	//
	//----------------------------------------------//

//	char* nameStr = monster_res[short_para1]->name;
//	if( nameStr[0] == 'I' )		// "An Ick Lair"
//		str = "An ";
//	else
//		str = "A ";
//	str += nameStr;
//	str += " Lair has been destroyed.";

	str = text_news.process( text_news.get_format_str( 47, "FLDE" ),
		monster_res[short_para1]->name );
}
//------- End of function News::monster_firm_destroyed -----//


//------ Begin of function News::scroll_acquired -----//
//
// nation_name1() - the nation that has acquired the scroll.
//
// short_para1 = the race id. of the scroll.
//
void News::scroll_acquired()
{
	//---------------- Text Format -----------------//
	//
	// You have acquired the <race name> Scroll of Power.
	// <nation name> has acquired the <race name> Scroll of Power.
	//
	//----------------------------------------------//

//	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
//	{
//		str  = "You have acquired the ";
//	}
//	else
//	{
//		str  = nation_name1();
//		str += " has acquired the ";
//	}
//	str += race_res[short_para1]->adjective;
//	str += " Scroll of Power.";

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str = text_news.process( text_news.get_format_str( 48, "YSOP" ),
			race_res[short_para1]->adjective );
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 49, "NSOP" ),
			race_res[short_para1]->adjective, nation_name1() );
	}
}
//------- End of function News::scroll_acquired -----//


//------ Begin of function News::monster_gold_acquired -----//
//
// nation_name1() - the nation that has acquired the monster treasure.
//
// short_para1 = amount of gold.
//
void News::monster_gold_acquired()
{
	//---------------- Text Format -----------------//
	//
	// // Your have recovered <treasure amount> worth of treasure from the Fryhtans.
	// Your have recovered <treasure amount> worth of treasure.
	//
	//----------------------------------------------//

//	str  = "You have recovered ";
//	str += m.format(short_para1,2);
//	str += " worth of treasure from the Fryhtans.";

	str = text_news.process( text_news.get_format_str( 50, "YTRE" ),
		short_para1 );
}
//------- End of function News::monster_gold_acquired -----//


//------ Begin of function News::your_spy_killed -----//
//
// nation_name1() - your nation.
// nation_name2() - the nation that killed your spy.
//
// short_para1 - firm build id. if it's a firm
//					  0 if it's a town
//					  race id if spy place == SPY_MOBILE 
// short_para2 - the town id.
//					  name id if spy place == SPY_MOBILE
// short_para3 - spy place
//
void News::your_spy_killed()
{
	//---------------- Text Format -----------------//
	//
	// Your spy has been exposed and executed on his mission to
	// <town name> [ in <nation name> ].  (no nation name for independent town.)
	//
	// Your spy has been exposed and executed on his mission to
	// a <firm type name> near <town name> in <nation name>.
	//
	// Your spy <spy name> has been exposed and executed on his mission to
	// <nation name>.
	//
	//----------------------------------------------//
/*
	if( short_para3 == SPY_FIRM )
	{
		str  = "Your spy has been exposed and executed on his mission to a ";
		str += firm_res.get_build(short_para1)->name;

		if( short_para2 > 0 )			// it is possible that there is no town in the map
		{
			str += " near ";
			str += town_res.get_name(short_para2);
		}

		if( nation_recno2 )		// not for independent town.
		{
			str += " in ";
			str += nation_name2();
		}
	}
	else if( short_para3 == SPY_TOWN )
	{
		str  = "Your spy has been exposed and executed on his mission to ";
		str += town_res.get_name(short_para2);

		if( nation_recno2 )		// not for independent town.
		{
			str += " in ";
			str += nation_name2();
		}
	}
	else
	{
		str  = "Your spy ";
		str += get_unit_name(short_para1, short_para2);
		str += " has been exposed and executed on his mission";

		if( nation_recno2 )		// not for independent town.
		{
			str += " to ";
			str += nation_name2();
		}
	}

	str += ".";
*/

	if( short_para3 == SPY_FIRM )
	{
		String firmPhrase;

		if( short_para2 > 0 && nation_recno2 )
		{
			firmPhrase = text_news.str_firm_near_town_of_nation(
				firm_res.get_build(short_para1)->name,
				town_res.get_name(short_para2),
				nation_name2() );
		}
		else if( short_para2 > 0 )			// it is possible that there is no town in the map
		{
			firmPhrase = text_news.str_firm_near_town(
				firm_res.get_build(short_para1)->name,
				town_res.get_name(short_para2) );
		}
		else if( nation_recno2 )		// not for independent town.
		{
			firmPhrase = text_news.str_firm_of_nation(
				firm_res.get_build(short_para1)->name,
				nation_name2() );
		}
		else
		{
			firmPhrase = firm_res.get_build(short_para1)->name;
		}

		str = text_news.process( text_news.get_format_str( 51, "YSPF" ),
			(char *)firmPhrase );
	}
	else if( short_para3 == SPY_TOWN )
	{
		String townPhrase;
		if( nation_recno2 )		// not for independent town.
			townPhrase = text_news.str_town_of_nation( 
				town_res.get_name(short_para2),
				nation_name2() );
		else
			townPhrase = town_res.get_name(short_para2);

		str = text_news.process( text_news.get_format_str( 52, "YSPT" ),
			(char *)townPhrase );
	}
	else
	{
		if( nation_recno2 )		// not for independent town.
		{
			str = text_news.process( text_news.get_format_str( 53, "YSPN" ),
				get_unit_name(short_para1, short_para2), nation_name2() );
		}
		else
		{
			str = text_news.process( text_news.get_format_str( 54, "YSPI" ),
				get_unit_name(short_para1, short_para2) );
		}
	}
}
//------- End of function News::your_spy_killed -----//


//------ Begin of function News::enemy_spy_killed -----//
//
// nation_name1() - your nation.
// nation_name2() - the nation that the spy belongs to.
//
// short_para1 - firm build id. if it's a firm
//					  0 if it's a town
// short_para2 - town name id. if it's a town
//					  0 if it's a firm
// short_para3 - id of the town where the firm is located.
//
void News::enemy_spy_killed()
{
	//---------------- Text Format -----------------//
	//
	// A spy from <kingdom> has been uncovered and executed in your
	// <firm type> near <town name>.
	//
	// A spy from <kingdom> has been uncovered and executed in your
	// town of <town name>.
	//
	// Spy <spy name> from <kingdom> has been uncovered and executed.
	//
	//----------------------------------------------//
/*
	if( short_para3==SPY_FIRM || short_para3==SPY_TOWN )
	{
		str  = "A spy from ";
		str += nation_name2();
		str += " has been uncovered and executed in your ";

		if( short_para3==SPY_FIRM )
		{
			str += firm_res.get_build(short_para1)->name;

			if( short_para2 > 0 )			// it is possible that there is no town in the map
			{
				str += " near ";
				str += town_res.get_name(short_para2);
			}

			str += ".";
		}
		else
		{
			str += "town of ";
			str += town_res.get_name(short_para2);
			str += ".";
		}
	}
	else
	{
		str  = "Spy ";
		str += get_unit_name(short_para1, short_para2);
		str += " from ";
		str += nation_name2();
		str += " has been uncovered and executed.";
	}
*/

	if( short_para3==SPY_FIRM )
	{
		String firmPhrase;
		if( short_para2 > 0 )			// it is possible that there is no town in the map
			firmPhrase = text_news.str_firm_near_town( 
				firm_res.get_build(short_para1)->name,
				town_res.get_name(short_para2) );
		else
			firmPhrase = firm_res.get_build(short_para1)->name;

		str = text_news.process( text_news.get_format_str( 55, "NSPF" ),
			nation_name2(), (char *)firmPhrase );
	}
	else if( short_para3==SPY_TOWN )
	{
		str = text_news.process( text_news.get_format_str( 56, "NSPT" ),
			nation_name2(), town_res.get_name(short_para2) );
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 57, "NSP?" ),
			get_unit_name(short_para1, short_para2), nation_name2() );
	}
}
//------- End of function News::enemy_spy_killed -----//


//------ Begin of function News::unit_betray -----//
//
// Only for mobile units or generals in command base.
//
// nation_name1() - the nation that the unit originally belongs to.
// nation_name2() - the nation that the unit has turned towards.
//
// short_para1 - race id. of the unit
// short_para2 - name id. of the unit
// short_para3 - rank id. of the unit
// short_para4 - unit id. of the unit
//
void News::unit_betray()
{
	//---------------- Text Format -----------------//
	//
	// [General] <unit name> has betrayed you and turned towards
	// <nation name>.
	//
	// [General] <unit name> has renounced you and become independent.
	//
	// [General] <unit name> of <nation name> has surrendered to your
	// forces. (Soldier)
	// <unit name> of <nation name> has defected to your Kingdom. (Civilian)
	//
	// Independent unit/Hero <unit name> has joined your forces.
	//
	//----------------------------------------------//
/*
	if( nation_recno1 == 0 )		// independent unit joining your force
	{
		if( short_para2 < 0 )		// heroes have negative name id.
			str = "Hero ";
		else
			str = "Independent unit ";

		str += get_unit_name(short_para1, short_para2);
		str += " has joined your force.";

		return;
	}

	//------------------------------------//

	if( short_para3==RANK_GENERAL )
		str = "General ";
	else
		str = "";

	str += get_unit_name(short_para1, short_para2);

	//---------------------------------//

	if( nation_recno2 == 0 )		// became an independent unit
	{
		str += " has renounced you and become independent.";
	}
	else
	{
		if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
		{
			str += " has betrayed you and turned towards ";
			str += nation_name2();
			str += ".";
		}
		else
		{
			str += " of ";
			str += nation_name1();
			// ####### begin Gilbert 21/1 #######//
			if( short_para1 > 0 && (short_para4 == UNIT_WAGON				// human, wagon or civilian
				|| race_res[short_para1]->civilian_unit_id == short_para4 ) )
				str += " has defected to your Kingdom.";	// civilian
			else
				str += " has surrendered to your forces.";	// soldier or other
			// ####### end Gilbert 21/1 #######//
		}
	}
*/
	if( nation_recno1 == 0 )		// independent unit joining your force
	{
		// ###### begin Gilbert 30/6 #######//
		if( short_para4 == UNIT_WAGON )	// race wagon
			str = text_news.process( text_news.get_format_str( 137, "IWBY" ),
				race_res[short_para1]->name, text_unit.str_nation(nation_race_id2) );
		else if( short_para2 >= 0 )		// independent unit
		// ###### end Gilbert 30/6 #######//
			str = text_news.process( text_news.get_format_str( 58, "IUBY" ),
				get_unit_name(short_para1, short_para2) );
		else								// independent hero
			str = text_news.process( text_news.get_format_str( 59, "IHBY" ),
				get_unit_name(short_para1, short_para2) );
	}
	else 
	{
		String unitPhrase;

		// #### begin Gilbert 29/6 ##########//
		if( short_para3 != RANK_SOLDIER )
		{
			unitPhrase = text_unit.str_unit_titled( short_para1, 
				get_unit_name(short_para1, short_para2), short_para3 );
		}
		else
		{
			unitPhrase = get_unit_name(short_para1, short_para2);
		}
		// #### end Gilbert 29/6 ##########//

		if( nation_recno2 == 0 )		// became an independent unit
		{
			// your unit betray to independent
			str = text_news.process( text_news.get_format_str( 60, "YUBI" ),
				(char *)unitPhrase );
		}
		else if( nation_array.player_recno && nation_recno1 == nation_array.player_recno
			|| nation_array.player_recno == 0 )		// case right after defeated
		{
			// your unit betray to a kingdom
			str = text_news.process( text_news.get_format_str( 61, "YUBN" ),
				(char *)unitPhrase, nation_name2() );
		}
		// other nation joined you
		else if( short_para1 > 0 && (short_para4 == UNIT_WAGON				// human, wagon or civilian
			|| race_res[short_para1]->civilian_unit_id == short_para4 ) )
		{
			str = text_news.process( text_news.get_format_str( 62, "NCBY" ),
				(char *)unitPhrase, nation_name1(), text_unit.str_nation(nation_race_id2) );
		}
		else		// military unit
		{
			str = text_news.process( text_news.get_format_str( 63, "NSBY" ),
				(char *)unitPhrase, nation_name1() );
		}
	}
}
//------- End of function News::unit_betray -----//


//------ Begin of function News::general_die -----//
//
// short_para1 - race id. of your general
// short_para2 - name id. of your general
//
void News::general_die()
{
	//---------------- Text Format -----------------//
	//
	// Your general, <general name>, has been slain.
	//
	//----------------------------------------------//

//	str  = "Your general, ";
//	str += get_unit_single_name(short_para1, short_para2);
//	str += ", has been slain.";

	str = text_news.process( text_news.get_format_str( 64, "GDIE" ),
		text_unit.str_title(short_para1, RANK_GENERAL), get_unit_single_name(short_para1, short_para2) );
}
//------- End of function News::general_die -----//


//------ Begin of function News::general_form_new_nation -----//
//
// short_para1 - race id. of your general
// short_para2 - name id. of your general
//
// nation_name1() - the nation that the unit originally belongs to.
//
void News::general_form_new_nation()
{
	//---------------- Text Format -----------------//
	//
	// Your general, <general name>, has declared
	// independence and established a new kingdom.
	//
	// General <general name> of <nation name> has declared
	// independence and established a new kingdom.
	//
	//----------------------------------------------//
/*
	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str  = "Your general, ";
		str += get_unit_single_name(short_para1, short_para2);
		str += ",";
	}
	else
	{
		str  = "General ";
		str += get_unit_single_name(short_para1, short_para2);
		str += " of ";
		str += nation_name1();
	}
	str += " has declared independence and established a new kingdom.";
*/

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str = text_news.process( text_news.get_format_str( 65, "YGNK" ),
			text_unit.str_title(short_para1, RANK_GENERAL),
			get_unit_single_name(short_para1, short_para2),
			text_unit.str_nation(short_para1) );
	}
	else
	{
		String unitPhrase;
		unitPhrase = text_unit.str_unit_titled(short_para1, 
			get_unit_single_name(short_para1, short_para2), RANK_GENERAL);
		str = text_news.process( text_news.get_format_str( 66, "NGNK" ),
			(char *)unitPhrase, nation_name1(), 
			text_unit.str_nation(short_para1) );
	}
}
//------- End of function News::general_form_new_nation -----//


//------ Begin of function News::unit_assassinated -----//
//
// short_para1 - rank id. of assassinated unit
// short_para2 - race id. of assassinated unit
// short_para3 - name id. of assassinated unit
// short_para4 - whether the enemy spy has been killed or not.
//
void News::unit_assassinated()
{
	//---------------- Text Format -----------------//
	//
	// Your King/All High, <king name>, has been assassinated by an enemy spy.
	// Your general/Ordo, <general name>, has been assassinated by an enemy spy.
	//
	// [The enemy spy has been killed.]
	//
	//----------------------------------------------//
/*
	// ####### begin Gilbert 19/2 #######//
	if( short_para1 == RANK_KING )
	{
		str  = short_para2 >= 0 ? "Your King, " : "Your All High, ";
		// str += get_unit_single_name( short_para2, short_para3 );
		str += king_name1();
	}
	else
	{
		str  = short_para2 >= 0 ? "Your general, " : "Your Ordo, ";
		str += get_unit_single_name( short_para2, short_para3 );
	}
	// ####### end Gilbert 19/2 #######//

	str += ", has been assassinated by an enemy spy.";

	if( short_para4 )
		str += " The enemy spy has been killed.";
*/

	if( short_para1 == RANK_KING )
	{
		str = text_news.process( text_news.get_format_str( 67, "ASSN" ),
			text_unit.str_title(short_para2, short_para1), king_name1() );
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 67, "ASSN" ),
			text_unit.str_title(short_para2, short_para1), 
			get_unit_single_name(short_para2, short_para3 ) );
	}

	if( short_para4 )
	{
		str += " ";
		str += text_news.get_format_str( 68, "ASSK" );
	}
}
//------- End of function News::unit_assassinated -----//


//------ Begin of function News::assassinator_caught -----//
//
// short_para1 - rank id. of the assassinating target.
// short_para2 - race id. of the assassinating target.
//
void News::assassinator_caught()
{
	//---------------- Text Format -----------------//
	//
	// An enemy spy has been killed while attempting
	// to assassinate your King/General.
	//
	//----------------------------------------------//

//	str = "An enemy spy has been killed while attempting to assassinate your ";
//	if( short_para1 == RANK_KING )
//		str += "King.";
//	else
//		str += "General.";

	str = text_news.process( text_news.get_format_str( 69, "NASF" ),
		text_unit.str_title(short_para2, short_para1) );
}
//------- End of function News::assassinator_caught -----//


//------ Begin of function News::raw_exhaust -----//
//
// short_para1 - raw id.
// short_para2 - firm build id
//
void News::raw_exhaust()
{
	//---------------- Text Format -----------------//
	//
	//// Your <raw type> Mine has exhausted its <raw type> deposit.
	// Your Mine/Alchemist Tor has exhausted its <raw type> deposit.
	//
	//----------------------------------------------//

//	str  = "Your ";
//	str += raw_res[short_para1]->name;
//	str += " Mine has exhausted its ";
//	str += raw_res[short_para1]->name;
//	str += " deposit.";

	str = text_news.process( text_news.get_format_str( 70, "RWEX" ),
		raw_res[short_para1]->name, firm_res.get_build(short_para2)->name );
}
//------- End of function News::raw_exhaust -----//


//------ Begin of function News::tech_researched -----//
//
// short_para1 - tech id.
// short_para2 - tech version.
//
void News::tech_researched()
{
	//---------------- Text Format -----------------//
	//
	// Your scientists have finished their <technology>
	// [Level/Mark <version>] research.
	//
	//----------------------------------------------//

/*
	str  = "Your scientists have finished their ";
	str += tech_res[short_para1]->tech_des();

	if( tech_res[short_para1]->max_tech_level(0) > 1 )		// if the tech has more than one level
	{
		if( tech_res[short_para1]->class_id == TECH_CLASS_WEAPON )		// Weapon use "Mark", others use "Level"
			str += " Mark ";
		else
			str += " Level ";

		str += m.roman_number(short_para2);
	}

	str += " research.";
*/

	if( tech_res[short_para1]->max_tech_level(0) > 1 )		// if the tech has more than one level
	{
		switch( tech_res[short_para1]->class_id )
		{
		case TECH_CLASS_WEAPON:
		case TECH_CLASS_INCUBATION:
			str = text_news.process( text_news.get_format_str( 71, "TECW" ),
				tech_res[short_para1]->tech_des(), short_para2 );
			break;
		default:
			str = text_news.process( text_news.get_format_str( 72, "TECH" ),
				tech_res[short_para1]->tech_des(), short_para2 );
		}
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 73, "TEC1" ),
			tech_res[short_para1]->tech_des() );
	}
}
//------- End of function News::tech_researched -----//


//------ Begin of function News::lightning_damage -----//
//
// depend on short_para1 :
//
// short_para1 = NEWS_LOC_UNIT
//		short_para2 = race id of the victim
//		short_para3 = name id of the victim
//		short_para4 = rank id of the victim
// short_para1 = NEWS_LOC_FIRM
//    short_para2 = firm build id of the victim firm
//    short_para3 = town nearby
// short_para1 = NEWS_LOC_TOWN
//    short_para3 = town name id
//
// short_para5 = kill/destroy flag
//
void News::lightning_damage()
{
	//---------------- Text Format -----------------//
	//
	// Your <unit name> has been struck and injured/killed by lightning.
	//
	// Your <firm name> near <town name> has been struck/destroyed by lightning.
	//
	// Your town <town name> has been struck/destroyed by lightning.
	//
	//----------------------------------------------//
/*
	str = "Your ";

	switch( short_para1 )
	{
	case NEWS_LOC_UNIT:
		if( short_para4 == RANK_KING )
		{
			str += short_para2 >= 0 ? "King " : "All High ";
		}
		else if( short_para4 == RANK_GENERAL )
		{
			str += short_para2 >= 0 ? "General " : "Ordo ";
			str += get_unit_name(short_para2, short_para3);
		}
		else
		{
			str += "unit ";
			str += get_unit_name(short_para2, short_para3);
		}
		break;

	case NEWS_LOC_FIRM:
		str += firm_res.get_build(short_para2)->name;
		if( short_para3 )
		{
			str += " near ";
			str += town_res.get_name(short_para3);
		}
		break;

	case NEWS_LOC_TOWN:
		str += "town ";
		str += town_res.get_name(short_para3);
		break;

	default:
		err_here();
	}

	//----------------------------------//

	str += " has been ";

	if( short_para1==NEWS_LOC_UNIT )
	{
		if( short_para5 )
			str += "struck and killed ";
		else
			str += "struck and injured ";
	}
	else
	{
		if( short_para5 )
			str += "destroyed ";
		else
			str += "struck ";
	}

	str += "by lightning.";
*/

	switch( short_para1 )
	{
	case NEWS_LOC_UNIT:
		{
			String unitPhrase;
			if( short_para4 == RANK_KING )					// King/All High, no name
				unitPhrase = text_unit.str_title( short_para2, short_para4 );
			else if( short_para4 == RANK_GENERAL )			// General/Ordo
				unitPhrase = text_unit.str_unit_titled( short_para2, 
					get_unit_name(short_para2, short_para3), short_para4 );
			else
				unitPhrase = get_unit_name(short_para2, short_para3);

			if( !short_para5 )
			{	// injured
				str = text_news.process( text_news.get_format_str( 74, "LIHU"),
					(char *)unitPhrase );
			}
			else
			{	// killed
				str = text_news.process( text_news.get_format_str( 75, "LIKU"),
					(char *)unitPhrase );
			}
		}
		break;

	case NEWS_LOC_FIRM:
		{
			String firmPhrase;
			if( short_para3 )
				firmPhrase = text_news.str_firm_near_town( 
					firm_res.get_build(short_para2)->name,
					town_res.get_name(short_para3) );
			else
				firmPhrase = firm_res.get_build(short_para2)->name;

			if( !short_para5 )
			{	// injured
				str = text_news.process( text_news.get_format_str( 76, "LIHF"),
					(char *)firmPhrase );
			}
			else
			{	// killed
				str = text_news.process( text_news.get_format_str( 77, "LIKF"),
					(char *)firmPhrase );
			}
		}
		break;

	case NEWS_LOC_TOWN:
		if( !short_para5 )
		{	// injured
			str = text_news.process( text_news.get_format_str( 78, "LIHT"),
				town_res.get_name(short_para3) );
		}
		else
		{	// killed
			str = text_news.process( text_news.get_format_str( 79, "LIKT"),
				town_res.get_name(short_para3) );
		}
		break;

	default:
		err_here();
	}
}
//------- End of function News::lightning_damage -----//


//------ Begin of function News::earthquake_damage -----//
//
void News::earthquake_damage()
{
	//---------------- Text Format -----------------//
	//
	// <number> units of your Kingdom has/have been injured and <number> killed
	// in an earthquake.
	//
	// <number> of your citizens has/have been killed in an earthquake.
	//
	// <number> of your buildings has/have been damaged and <number> destroyed
	// in an earthquake.
	//
	//----------------------------------------------//

/*
	int conjunction = 0;
	str = "";

	if( short_para1 == 1)
	{
		str += short_para2;
		if( short_para2 == 1 )
			str += " unit of your kingdom has";
		else
			str += " units of your kingdom have";
		str += " been injured";

		if( short_para3 > 0)
		{
			str += " and ";
			str += short_para3;
			str += " killed";
		}

		str += " in an earthquake";
	}
	else if( short_para1 == 2 )
	{
		if( short_para2 > 0)
		{
			str += short_para2;
			str += " of your citizen";
			if( short_para2 == 1)
				str += " has";
			else
				str += "s have";
			str += " been killed in an earthquake";
		}
	}
	else if( short_para1 == 3)
	{
		str += short_para2;
		str += " of your building";
		if( short_para2 == 1)
			str += " has";
		else
			str += "s have";
		str += " been damaged";

		if( short_para3 > 0)
		{
			str += " and ";
			str += short_para3;
			str += " destroyed";
		}
		str += " in an earthquake.";
	}
	else
		err_here();
*/

	if( short_para1 == 1)
	{
		char* playerNationTitle = text_unit.str_nation( nation_race_id1 );
		if( short_para3 == 0)
			str = text_news.process( text_news.get_format_str( 80, "EQUI" ),
				short_para2, playerNationTitle );
		else
			str = text_news.process( text_news.get_format_str( 81, "EQUK" ),
				short_para2, playerNationTitle, short_para3 );
	}
	else if( short_para1 == 2)
	{
		str = text_news.process( text_news.get_format_str( 82, "EQTK" ),
			short_para2 );
	}
	else if( short_para1 == 3)
	{
		if( short_para3 == 0)
			str = text_news.process( text_news.get_format_str( 83, "EQFI" ),
				short_para2);
		else
			str = text_news.process( text_news.get_format_str( 84, "EQFK" ),
				short_para2, short_para3 );
	}
	else
	{
		err_here();
		str = "";
	}
}
//------- End of function News::earthquake_damage -----//


//------ Begin of function News::goal_deadline -----//
//
// Display a warning message as the deadline of the goals approaches.
//
// short_para1 - years left before the deadline.
// short_para2 - months left before the deadline.
//
void News::goal_deadline()
{
	//---------------- Text Format -----------------//
	//
	// Make haste! You have only <year> year[s] and <month> month[s]
	// left to achieve your goal.
	//
	//----------------------------------------------//
/*
	str = "Make haste! You have only ";

	if( short_para1 )
	{
		str += short_para1;

		if( short_para1 > 1 )
			str += " years ";
		else
			str += " year ";
	}

	if( short_para1 && short_para2 )
		str += "and ";

	if( short_para2 )
	{
		str += short_para2;

		if( short_para2 > 1 )
			str += " months ";
		else
			str += " month ";
	}

	str += "left to achieve your goal.";
*/
	String timeLeftStr;
	timeLeftStr = text_news.str_time_left_ym( short_para1, short_para2 );

	str = text_news.process( text_news.get_format_str( 85, "HAST" ),
		(char *)timeLeftStr );
}
//------- End of function News::goal_deadline -----//


//------ Begin of function News::weapon_ship_worn_out -----//
//
// Your weapon or ship worn out and destroyed due to lack of money for
// maintenance.
//
// short_para1 - unit id. of the weapon
// short_para2 - level of the weapon
//
void News::weapon_ship_worn_out()
{
	//---------------- Text Format -----------------//
	//
	// A <weapon name> <weapon level> of yours has broken
	// down due to the lack of maintenance funds.
	//
	//----------------------------------------------//

//	str  = "A ";
//	str += unit_res[short_para1]->name;
//	if( short_para2 )
//	{
//		str += " ";
//		str += m.roman_number(short_para2);
//	}
//	str += " of yours has broken down due to the lack of maintenance funds.";

	str = text_news.process( text_news.get_format_str( 86, "WEPO" ),
		unit_res[short_para1]->name, short_para2 );
}
//------- End of function News::weapon_ship_worn_out -----//


//------ Begin of function News::firm_worn_out -----//
//
// short_para1 - firm build id. of the firm destroyed.
// short_para2 - id of the town where the firm is located.
//
void News::firm_worn_out()
{
	//---------------- Text Format -----------------//
	//
	// Your <firm type> near <town name> has fallen into
	// disrepair due to the lack of maintenance funds.
	//
	//----------------------------------------------//

//	str  = "Your ";
//	str += firm_res.get_build(short_para1)->name;
//	if( short_para2 > 0 )			// it is possible that there is no town in the map
//	{
//		str += " near ";
//		str += town_res.get_name(short_para2);
//	}
//	str += " has fallen into disrepair due to the lack of maintenance funds.";

	String firmPhrase;
	if( short_para2 > 0 )
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	str = text_news.process( text_news.get_format_str( 87, "FIMO" ),
		(char *)firmPhrase );
}
//------- End of function News::firm_worn_out -----//


//###### begin Gilbert 11/1 #######//
// ------ begin of function News::event_finish_tech -------//
//
// short_para1 - tech id
// short_para2 - tech version
//
void News::event_finish_tech()
{
	//---------------- Text Format -----------------//
	//
	// A research breakthrough has been achieved 
	// and your current project has been completed!
	//
	// ---------------------------------------------//

//	str = "A research breakthrough has been achieved and your current project has been completed!";
	str = text_news.get_format_str( 88, "EVT1" );
}
// ------ end of function News::event_finish_tech -------//


// ------ begin of function News::event_new_tech -------//
//
// short_para1 - tech id
// short_para2 - tech version
//
void News::event_new_tech()
{
	//---------------- Text Format -----------------//
	//
	// A lost scroll has been found giving details of 
	// (technology) technology!
	//
	// ---------------------------------------------//

	TechInfo *techInfo = tech_res[short_para1];
//	str = "A lost scroll has been found giving details of ";
//	str += techInfo->tech_des();
//	if( techInfo->max_tech_level(0) > 1 )
//	{
//		str += " ";
//		str += m.roman_number(short_para2);
//	}
//	str += " technology!";

	str = text_news.process( text_news.get_format_str( 89, "EVT2" ),
		techInfo->tech_des(), techInfo->max_tech_level(0)>1 ? short_para2 : 0 );
}
// ------ end of function News::event_new_tech -------//


// ------ begin of function News::event_food_destroyed -------//
//
// short_para1 - food destroyed
//
void News::event_food_destroyed()
{
	//---------------- Text Format -----------------//
	//
	// A plague of rats has infested your granaries!
	// You have lost (number %) of your food stores!
	//
	// ---------------------------------------------//

//	str = "A plague of rats has infested your granaries! You have lost ";
//	str += short_para1;
//	str += " bushels of your food stores!";

	str = text_news.process( text_news.get_format_str( 90, "EVFO" ),
		short_para1 );
}
// ------ end of function News::event_food_destroyed -------//


// ------ begin of function News::event_unit_die -------//
//
// short_para1 - race of the person died
// short_para2 - name id of the person died
// short_para3 - rank of the person died
//
void News::event_unit_die()
{
	// -------- text format --------//
	//
	// (name) in the field have died of disease.
	// Your General, (name), has succumbed to the plague.
	//
	// -----------------------------//
/*
	switch( short_para3 )
	{
	case RANK_SOLDIER:
		str = get_unit_single_name(short_para1, short_para2);
		str += " in the field have died of disease.";
		break;
	case RANK_GENERAL:
		str = "Your general, ";
		str += get_unit_single_name(short_para1, short_para2);
		str += ", has succumbed to the plague.";
		break;
	case RANK_KING:
		str = "Your King has succumbed to the plague.";
		break;
	default:
		err_here();
		break;
	}
*/

	switch( short_para3 )
	{
	case RANK_KING:
		str = text_news.process( text_news.get_format_str( 91, "EVKD" ),
			text_unit.str_title(short_para1, short_para3) );
		break;
	case RANK_GENERAL:
		str = text_news.process( text_news.get_format_str( 92, "EVGD" ),
			text_unit.str_title(short_para1, short_para3),
			get_unit_single_name(short_para1, short_para2) );
		break;
	case RANK_SOLDIER:
		str = text_news.process( text_news.get_format_str( 93, "EVUD" ),
			get_unit_single_name(short_para1, short_para2) );
		break;
	default:
		err_here();
		break;
	}
}
// ------ end of function News::event_unit_die -------//


// ------ begin of function News::event_soldier_die -------//
//
// short_para1 - firm build id
// short_para2 - town name id
// short_para3 - people died
// short_para4 - firm race id
//
void News::event_soldier_die()
{
	// -------- text format --------//
	//
	// (number) soldiers in (Camp/Fort) has/have died of the plague
	// (number) crouls has/have died in their (lair) of the plague
	// 
	// -----------------------------//
/*
	if( short_para4 >= 0 )		// race_id
	{
		str = short_para3;
		str += " soldiers in ";
		str += firm_res.get_build(short_para1)->name;
		if( short_para3 <= 1 )
			str += " has";
		else
			str += " have";
		str += " died of the plague";
	}
	else
	{
		str = short_para3;
		if( short_para3 <= 1 )
			str += " croul has died in his ";
		else
			str += " crouls have died in their ";
		str += firm_res.get_build(short_para1)->name;
		str += " of the plague";
	}
*/
	if( short_para4 >= 0 )		// race_id
	{
		str = text_news.process( text_news.get_format_str( 94, "EVHP" ),
			short_para3, firm_res.get_build(short_para1)->name );
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 95, "EVFP" ),
			short_para3, firm_res.get_build(short_para1)->name );
	}
}
// ------ end of function News::event_soldier_die -------//


// ------ begin of function News::event_item_found -------//
//
// short_para1 - town name id
//
void News::event_item_found()
{
	// -------- text format --------//
	//
	// A cache of artifacts has been dug up 
	// near your town of (name)!
	//
	// -----------------------------//

//	str = "A cache of artifacts has been dug up near your town of ";
//	str += town_res.get_name(short_para1);
//	str += "!";

	str = text_news.process( text_news.get_format_str( 96, "EVIF" ),
		town_res.get_name(short_para1) );
}
// ------ end of function News::event_item_found -------//


// ------ begin of function News::event_town_people_killed -------//
//
// short_para1 - town name id
// short_para2 - people killed
//
void News::event_town_people_killed()
{
	// ------- text format --------//
	//
	// A plague has killed (number) people in your town of (town name).
	//
	// ----------------------------//
/*
	str = "A plague has killed ";
	str += short_para2;
	if( short_para2 <= 1 )
		str += " person";
	else
		str += " people";
	str += " in your town of ";
	str += town_res.get_name(short_para1);
*/

	str = text_news.process( text_news.get_format_str( 97, "EVTP" ),
		short_para2, town_res.get_name(short_para1) );
}
// ------ end of function News::event_town_people_killed -------//


// ------ begin of function News::event_factory_destroyed -------//
//
// short_para1 - firm build id
// short_para2 - nearest town name id
// short_para3 - people killed
//
void News::event_factory_destroyed()
{
	// ------- text format --------//
	//
	// A (factory or war factory) has been destroyed in an accident.
	// 1 worker has lost his life./(number) workers have lost their lives.
	//
	// ----------------------------//
/*
	str = "A ";
	str += firm_res.get_build(short_para1)->name;
	str += " has been destroyed in an accident. ";
	if( short_para3 > 0 )
	{
		str += short_para3;
		if( short_para3 <= 1 )
			str += " worker has lost his life.";
		else
			str += " workers have lost their lives.";
	}
*/

	str = text_news.process( text_news.get_format_str( 98, "EVFD" ),
		firm_res.get_build(short_para1)->name, short_para3 );
}
// ------ end of function News::event_factory_destroyed -------//


// ------ begin of function News::event_mine_destroyed -------//
//
// short_para1 - firm build id
// short_para2 - nearest town name id
// short_para3 - people killed
//
void News::event_mine_destroyed()
{
	// ------- text format --------//
	//
	// There has been an explosion in your (material) mine, 
	// collapsing it and causing the death of (number) worker(s).
	//
	// -------------------------------//
/*
	str = "There has been an explosion in your mine, collapsing it";
	if( short_para3 <= 0 )
		str += ".";
	else if( short_para3 == 1 )
		str += " and causing the death of 1 worker.";
	else
	{
		str += " and causing the death of ";
		str += short_para3;
		str += " workers.";
	}
*/
	str = text_news.process( text_news.get_format_str( 99, "EVMD" ),
		short_para3 );
}
// ------ end of function News::event_mine_destroyed -------//


// ------ begin of function News::event_inn_destroyed -------//
//
// short_para1 - firm build id
// short_para2 - town name id
//
void News::event_inn_destroyed()
{
	// ------- text format --------//
	//
	// A terrible brawl has destroyed your inn near (town name).
	//
	// ----------------------------//
/*
	str = "A terrible brawl has destroyed your ";
	str += firm_res.get_build(short_para1)->name;

	if( short_para2 > 0 )			// it is possible that there is no town in the map
	{
		str += " near ";
		str += town_res.get_name(short_para2);
	}
*/
	String firmPhrase;
	if( short_para2 > 0 )
		firmPhrase = text_news.str_firm_near_town( 
			firm_res.get_build(short_para1)->name,
			town_res.get_name(short_para2) );
	else
		firmPhrase = firm_res.get_build(short_para1)->name;

	str = text_news.process( text_news.get_format_str( 100, "EVID" ),
		(char *)firmPhrase );
}
// ------ end of function News::event_inn_destroyed -------//


//------ Begin of function News::tornado_hit -----//
//
void News::tornado_hit()
{
	// ----------- text format --------//
	//
	// A tornado has been spotted! Take all necessary precautions!
	//
	// --------------------------------//

	// str = "A tornado has been spotted! Take all necessary precautions!";

	str = text_news.get_format_str( 109, "TORD" );
}
//------ End of function News::tornado_hit -----//


//------ Begin of function News::blueprint_acquired -----//
//
// nation_name1() - the nation that has acquired the blueprint.
//
// short_para1 = the tech id of the scroll
//
void News::blueprint_acquired()
{
	//---------------- Text Format -----------------//
	//
	// You have acquired the scroll of <tech name>.
	// <nation name> has acquired the scroll of <tech name>.
	//
	//----------------------------------------------//
/*
	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str  = "You have acquired the scroll of ";
	}
	else
	{
		str  = nation_name1();
		str += " has acquired the scroll of ";
	}
	str += tech_res[short_para1]->tech_des();
	str += ".";
*/

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str = text_news.process( text_news.get_format_str( 110, "YGWS" ),
			tech_res[short_para1]->tech_des() );
	}
	else
	{
		str = text_news.process( text_news.get_format_str( 111, "NGWS" ),
			tech_res[short_para1]->tech_des(), nation_name1() );
	}
}
//------- End of function News::blueprint_acquired -----//


//------ Begin of function News::magic_tower_attack -----//
//
// short_para1 - build id. of the firm, 0 if town or unit
// short_para2 - name id of the town where the firm is located.( 0 if unit)
// short_para3 - target's object type OBJ_UNIT/ OBJ_TOWN/ OBJ_FIRM
// loc_x, loc_y = xLoc, yLoc
//
void News::magic_tower_attack()
{
	//---------------- Text Format -----------------//
	//
	// A Mage Tor has fired at your unit/<building name>/<town name>.
	// A Mage Tor has fired at <nation name>/an independent structure.
	//
	//----------------------------------------------//
/*
	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		str  = "A ";
		str += firm_res[FIRM_MAGIC]->name;
		str += " has fired at your ";

		switch( short_para3 )
		{
		case OBJ_UNIT:
			str += "unit.";
			break;
		case OBJ_FIRM:
			str += firm_res.get_build(short_para1)->name;
			if( short_para2 > 0 )			// it is possible that there is no town in the map
			{
				str += " near ";
				str += town_res.get_name(short_para2);
			}
			str += ".";
			break;
		case OBJ_TOWN:
			str += "town ";
			str += town_res.get_name(short_para2);
			str += ".";
			break;
		default:
			str = "A Magic Tor has fired at your Kingdom.";
		}
	}
	else
	{
		str  = "A ";
		str += firm_res[FIRM_MAGIC]->name;
		str += " has fired at ";
		if( nation_recno1 )
			str += nation_name1();
		else
			str += "an independent structure";
		str += ".";
	}
*/

	char *magicTowerName = firm_res[FIRM_MAGIC]->name;

	if( nation_array.player_recno && nation_recno1 == nation_array.player_recno )
	{
		switch( short_para3 )
		{
		case OBJ_UNIT:
			str = text_news.process( text_news.get_format_str( 112, "MGYU" ),
				magicTowerName );
			break;
		case OBJ_FIRM:
			{
				String firmPhrase;
				if( short_para2 > 0 )
					firmPhrase = text_news.str_firm_near_town(
						firm_res.get_build(short_para1)->name,
						town_res.get_name(short_para2) );
				else
					firmPhrase = firm_res.get_build(short_para1)->name;

				str = text_news.process( text_news.get_format_str( 113, "MGYF" ),
					magicTowerName, (char *)firmPhrase );
			}
			break;
		case OBJ_TOWN:
			str = text_news.process( text_news.get_format_str( 114, "MGYT" ),
				magicTowerName, town_res.get_name(short_para2) );
			break;
		default:
			str = text_news.process( text_news.get_format_str( 115, "MGY?" ),
				magicTowerName, text_unit.str_nation(nation_race_id1) );
		}
	}
	else
	{
		if( nation_recno1 )
		{
			str = text_news.process( text_news.get_format_str( 116, "MGYN" ),
				magicTowerName, nation_name1() );
		}
		else
		{
			str = text_news.process( text_news.get_format_str( 117, "MGYI" ),
				magicTowerName );
		}
	}
}
//------- End of function News::magic_tower_attack -----//


//------ Begin of function News::chat_msg -----//
//
// short_para1 - id. of the chat msg in Info::remote_chat_str_array[]
//
// nation_name1() - the nation from which this chat message is sent.
//
void News::chat_msg()
{
	str = info.remote_chat_array[short_para1-1].chat_str;
}
//------- End of function News::chat_msg -----//


//------ Begin of function News::multi_retire -----//
//
// This function is called when a human player retires.
//
// nation_name1() - the nation that has retired.
//
void News::multi_retire()
{
	//---------------- Text Format -----------------//
	//
	// <Kingdom name> has retired and quited the game.
	//
	//----------------------------------------------//

//	str  = nation_name1();
//	str += " has retired and quited the game.";

	str = text_news.process( text_news.get_format_str( 133, "MP_R" ),
		nation_name1() );
}
//------- End of function News::multi_retire -----//


//------ Begin of function News::multi_quit_game -----//
//
// This function is called when a human player quits the game.
//
void News::multi_quit_game()
{
	//---------------- Text Format -----------------//
	//
	// <Kingdom name> has quited the game.
	//
	//----------------------------------------------//

//	str  = nation_name1();
//	str += " has quited the game.";

	str = text_news.process( text_news.get_format_str( 134, "MP_Q" ),
		nation_name1() );
}
//------- End of function News::multi_quit_game -----//


//------ Begin of function News::multi_save_game -----//
//
// This function is called when a human player calls for saving the game.
//
void News::multi_save_game()
{
	//---------------- Text Format -----------------//
	//
	// The current game has been saved to <file name>.
	//
	//----------------------------------------------//

//	str  = "The current game has been saved to ";
//	str += remote.save_file_name;
//	str += ".";

	str = text_news.process( text_news.get_format_str( 135, "MP_S" ),
		remote.save_file_name );
}
//------- End of function News::multi_save_game -----//


//------ Begin of function News::multi_connection_lost -----//
//
// This function is called when a human player's connection has been lost.
//
void News::multi_connection_lost()
{
	//---------------- Text Format -----------------//
	//
	// The connectino with <kingdom name> has been lost.
	//
	//----------------------------------------------//

//	str  = "The connection with ";
//	str += nation_name1();
//	str += " has been lost.";

	str = text_news.process( text_news.get_format_str( 136, "MP_L" ),
		nation_name1() );
}
//------- End of function News::multi_connection_lost -----//


//------ Begin of function News::nation_name1 -----//
//
char* News::nation_name1()
{
	static String str;

	str = nation_array.get_custom_nation_name(nation_recno1);

	//------ add color bar -------//

	char colorCodeStr[] = " 0";

	colorCodeStr[1] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_color1;

	str += colorCodeStr;

	return str;
}
//------- End of function News::nation_name1 -----//


//------ Begin of function News::nation_name2 -----//
//
char* News::nation_name2()
{
	static String str;

	str = nation_array.get_custom_nation_name(nation_recno2);

	//------ add color bar -------//

	char colorCodeStr[] = " 0";

	colorCodeStr[1] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_color2;

	str += colorCodeStr;

	return str;
}
//------- End of function News::nation_name2 -----//


//------ Begin of function News::king_name1 -----//
//
// [int] addColor - add color bar at the end of the king name
//						  (default: 0)
//
char* News::king_name1(int addColor)
{
	static String str;

	str = nation_array.get_custom_king_name(nation_recno1);

	//------ add color bar -------//

	if( addColor )
	{
		char colorCodeStr[] = " 0";
		colorCodeStr[1] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_color1;

		str += colorCodeStr;
	}

	return str;
}
//------- End of function News::king_name1 -----//


//------ Begin of function News::king_name2 -----//
//
// [int] addColor - add color bar at the end of the king name
//						  (default: 0)
//
char* News::king_name2(int addColor)
{
	static String str;

	str = nation_array.get_custom_king_name(nation_recno2);

	//------ add color bar -------//

	if( addColor )
	{
		char colorCodeStr[] = " 0";
		colorCodeStr[1] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_color2;

		str += colorCodeStr;
	}

	return str;
}
//------- End of function News::king_name2 -----//


// #endif
