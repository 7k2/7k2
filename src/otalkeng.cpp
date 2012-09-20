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

//Filename    : OTALKENG.CPP
//Description : English version of the talk messages

// #if( !defined(GERMAN) && !defined(FRENCH) && !defined(SPANISH) )

#include <omouse.h>
#include <osys.h>
#include <ovga.h>
#include <oinfo.h>
#include <ofont.h>
#include <otechres.h>
#include <ogameset.h>
#include <onews.h>
#include <onation.h>
#include <otalkres.h>
#include <ot_talk.h>

//-------- define static vars --------//

static String str;						// these vars don't have to be saved as their are only used by msg_str() for passing vars to other functions
static short  viewing_nation_recno;
static char   should_disp_reply;
static char   disp_second_line;


//------- Begin of function TalkMsg::msg_str --------//
//
// Return the text of this message.
//
// <int> viewingNationRecno = the recno of the nation viewing this message
// [int] dispReply 			 = whether display the reply if there is one
//										(default: 1)
// [int] dispSecondLine		 = whether should display the second line of the
//										message (default: 0)
//
char* TalkMsg::msg_str(int viewingNationRecno, int dispReply, int dispSecondLine)
{
	viewing_nation_recno = viewingNationRecno;
	should_disp_reply		= dispReply;
	disp_second_line		= dispSecondLine;

	//-------- compose the message str -------//

	switch(talk_id)
	{
		case TALK_PROPOSE_TRADE_TREATY:
			propose_treaty("trade");
			break;

		case TALK_PROPOSE_FRIENDLY_TREATY:
			propose_treaty("friendly");
			break;

		case TALK_PROPOSE_ALLIANCE_TREATY:
			propose_treaty("alliance");
			break;

		case TALK_END_TRADE_TREATY:
			end_treaty("trade");
			break;

		case TALK_END_FRIENDLY_TREATY:
			end_treaty("friendly");
			break;

		case TALK_END_ALLIANCE_TREATY:
			end_treaty("alliance");
			break;

		case TALK_REQUEST_MILITARY_AID:
			request_military_aid();
			break;

		case TALK_REQUEST_TRADE_EMBARGO:
			request_trade_embargo();
			break;

		case TALK_REQUEST_CEASE_WAR:
			request_cease_war();
			break;

		case TALK_REQUEST_DECLARE_WAR:
			request_declare_war();
			break;

		case TALK_REQUEST_BUY_FOOD:
			request_buy_food();
			break;

		case TALK_DECLARE_WAR:
			declare_war();
			break;

		case TALK_GIVE_TRIBUTE:
			give_tribute("tribute");
			break;

		case TALK_DEMAND_TRIBUTE:
			demand_tribute(0);		// 1-is tribute, not aid
			break;

		case TALK_GIVE_AID:
			give_tribute("aid");
			break;

		case TALK_DEMAND_AID:
			demand_tribute(1);		// 1-is aid, not tribute
			break;

		case TALK_EXCHANGE_TECH:
			exchange_tech();
			break;

		case TALK_REQUEST_SURRENDER:
			request_surrender();
			break;

		case TALK_SURRENDER:
			surrender();
			break;

		default:
			err_here();
	}

	return str;
}
//-------- End of function TalkMsg::msg_str ---------//


//----- Begin of function TalkMsg::propose_treaty ------//
//
// talk_para1 - duration of the treaty (no. of years).
//
void TalkMsg::propose_treaty(const char* treatyTypeStr)
{
	//---------------------------------------------//
	//
	// Send:
	//
	// <King>'s Kingdom proposes a/an friendly/alliance treaty to you.
	// You propose a/an friendly/alliance treaty to <King>'s Kingdom.
	//
	// Reply:
	//
	// <King>'s Kingdom accepts/rejects your proposed
	// friendly/alliance treaty.
	//
	// You accept/reject the friendly/alliance treaty
	// proposed by <King>'s Kingdom.
	//
	//---------------------------------------------//

/*	String treatyStr;
	char*  articleStr;

	if( treatyTypeStr[0] == 'a' )
		articleStr = "an ";
	else
		articleStr = "a ";

	treatyStr  = treatyTypeStr;
	treatyStr += " treaty ";

	//--------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You propose ";
			str += articleStr;
			str += treatyStr;
			str += " to ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str =  from_nation_name();
			str += " proposes ";
			str += articleStr;
			str += treatyStr;
			str += " to you.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepts your proposed ";
			else
				str += " rejects your proposed ";

			str += treatyStr;
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You accept the ";
			else
				str = "You reject the ";

			str += treatyStr;

			str += " proposed by ";
			str += from_nation_name();
			str += ".";
		}
	}
*/
	// treatyTypeStr is ignored

	switch( talk_id )
	{
	case TALK_PROPOSE_TRADE_TREATY:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 11, "YPTT" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 12, "NPTT" ),
					from_nation_name() );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 13, "NATT" ),
						to_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 14, "NRTT" ),
						to_nation_name() );
			}
			else
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 15, "YATT" ),
						from_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 16, "YRTT" ),
						from_nation_name() );
			}
		}
		break;
	case TALK_PROPOSE_FRIENDLY_TREATY:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 17, "YPFT" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 18, "NPFT" ),
					from_nation_name() );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 19, "NAFT" ),
						to_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 20, "NRFT" ),
						to_nation_name() );
			}
			else
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 21, "YAFT" ),
						from_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 22, "YRFT" ),
						from_nation_name() );
			}
		}
		break;
	case TALK_PROPOSE_ALLIANCE_TREATY:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 23, "YPAT" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 24, "NPAT" ),
					from_nation_name() );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 25, "NAAT" ),
						to_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 26, "NRAT" ),
						to_nation_name() );
			}
			else
			{
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 27, "YAAT" ),
						from_nation_name() );
				else
					str = text_talk.process( text_talk.get_format_str( 28, "YRAT" ),
						from_nation_name() );
			}
		}
		break;
	default:
		err_here();
		str = "";
	}
}
//------- End of function TalkMsg::propose_treaty ------//


//----- Begin of function TalkMsg::end_treaty ------//
//
// talk_para1 - treaty type, RELATION_FRIENDLY or RELATION_ALLIANCE.
//
void TalkMsg::end_treaty(const char* treatyTypeStr)
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom terminates its friendly/alliance treaty with you.
	// You terminate your friendly/alliance treaty with <King>'s Kingdom.
	//
	//---------------------------------------------//

/*
	if( viewing_nation_recno == from_nation_recno )
	{
		str  = "You terminate your ";
		str += treatyTypeStr;
		str += " treaty with ";
		str +=  to_nation_name();
		str += ".";
	}
	else
	{
		str  = from_nation_name();
		str += " terminates its ";
		str += treatyTypeStr;
		str += " treaty with you.";
	}
*/
	switch( talk_id )
	{
	case TALK_END_TRADE_TREATY:
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 29, "YETT" ),
				to_nation_name() );
		else
			str = text_talk.process( text_talk.get_format_str( 30, "NETT" ),
				from_nation_name() );
		break;
	case TALK_END_FRIENDLY_TREATY:
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 31, "YEFT" ),
				to_nation_name() );
		else
			str = text_talk.process( text_talk.get_format_str( 32, "NEFT" ),
				from_nation_name() );
		break;
	case TALK_END_ALLIANCE_TREATY:
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 33, "YEAT" ),
				to_nation_name() );
		else
			str = text_talk.process( text_talk.get_format_str( 34, "NEAT" ),
				from_nation_name() );
		break;
	default:
		err_here();
		str = "";
	}
}
//------- End of function TalkMsg::end_treaty ------//


//----- Begin of function TalkMsg::request_cease_war ------//
//
void TalkMsg::request_cease_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests a cease-fire.
	// You request a cease-fire with <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees to a cease-fire.
	// <King>'s Kingdom refuses a cease-fire.
	// You agree to a cease-fire with <King>'s Kingdom.
	// You refuse a cease-fire with <King>'s Kingdom.
	//
	//---------------------------------------------//
/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You request a cease-fire with ";
			str +=  to_nation_name();
			str += ".";
		}
		else
		{
			str =  from_nation_name();
			str += " requests a cease-fire.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees to";
			else
				str += " refuses";

			str += " a cease-fire.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree to";
			else
				str = "You refuse";

			str += " a cease-fire with ";
			str += from_nation_name();
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 47, "YPCW" ),
				to_nation_name() );
		else
			str = text_talk.process( text_talk.get_format_str( 48, "NPCW" ),
				from_nation_name() );
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 49, "NACW" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 50, "NRCW" ),
					to_nation_name() );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 51, "YACW" ),
					from_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 52, "YRCW" ),
					from_nation_name() );
	}
}
//------- End of function TalkMsg::request_cease_war ------//


//----- Begin of function TalkMsg::request_declare_war ------//
//
// talk_para1 - the recno of the nation to declare war with.
//
void TalkMsg::request_declare_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests that you declare war on <King B>'s Kingdom.
	// You request <King>'s Kingdom to declare war on <King B>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to declare war on <King B>'s Kingdom.
	// You agree/refuse to declare war on <King B>'s Kingdom.
	//
	//---------------------------------------------//

/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You request ";
			str +=  to_nation_name();
			str += " to";
		}
		else
		{
			str = from_nation_name();
			str += " requests that you";
		}

		str += " declare war on ";
		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees";
			else
				str += " refuses";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree";
			else
				str = "You refuse";
		}

		str += " to declare war on ";

		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 53, "YPDW" ),
				to_nation_name(), third_nation_name(talk_para1) );
		else
			str = text_talk.process( text_talk.get_format_str( 54, "NPDW" ),
				from_nation_name(), third_nation_name(talk_para1) );
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 55, "NADW" ),
					to_nation_name(), third_nation_name(talk_para1) );
			else
				str = text_talk.process( text_talk.get_format_str( 56, "NRDW" ),
					to_nation_name(), third_nation_name(talk_para1) );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 57, "YADW" ),
					from_nation_name(), third_nation_name(talk_para1) );
			else
				str = text_talk.process( text_talk.get_format_str( 58, "YRDW" ),
					from_nation_name(), third_nation_name(talk_para1) );
	}
}
//------- End of function TalkMsg::request_declare_war ------//


//----- Begin of function TalkMsg::request_buy_food ------//
//
// talk_para1 - the qty of food the nation wants to buy.
// talk_para2 - price offered for 10 qty of food.
//
void TalkMsg::request_buy_food()
{
	//---- display the second line in the reply question ----//
	//
	// <King>'s Kingdom offers $10 for 10 units of food.
	//
	//-------------------------------------------------------//

	if( disp_second_line )
	{
//		str =  from_nation_name();
//		str += " offers ";
//		str += misc.format(talk_para2,2);
//		str += " for 10 units of food.";

		str = text_talk.process( text_talk.get_format_str( 65, "FPRC" ),
			from_nation_name(), talk_para2 );
		return;
	}

	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests to purchase <amount>
	// units of food from you.
	//
	// You request to purchase <amount> units of food
	// from <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to sell <amount> units
	// of food to you.
	//
	// You agree/refuse to sell <amount> units of food to
	// <King>'s Kingdom.
	//
	//---------------------------------------------//
/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You request to purchase ";
			str += talk_para1;
			str += " units of food from ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str =  from_nation_name();
			str += " requests to purchase ";
			str += talk_para1;
			str += " units of food from you.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees";
			else
				str += " refuses";

			str += " to sell ";
			str += talk_para1;
			str += " units of food to you.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree";
			else
				str = "You refuse";

			str += " to sell ";
			str += talk_para1;
			str += " units of food to ";
			str += from_nation_name();
			str += ".";
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 59, "YPBF" ),
				to_nation_name(), talk_para1 );
		else
			str = text_talk.process( text_talk.get_format_str( 60, "NPBF" ),
				from_nation_name(), talk_para1 );
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 61, "NABF" ),
					to_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 62, "NRBF" ),
					to_nation_name(), talk_para1 );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 63, "YABF" ),
					from_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 64, "YRBF" ),
					from_nation_name(), talk_para1 );
	}
}
//------- End of function TalkMsg::request_buy_food ------//


//----- Begin of function TalkMsg::declare_war ------//
//
void TalkMsg::declare_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom declares war on you.
	// You declare war on <King>'s Kingdom.
	//
	//---------------------------------------------//
/*
	if( viewing_nation_recno == from_nation_recno )
	{
		str  = "You declare war on ";
		str += to_nation_name();
		str += ".";
	}
	else
	{
		str  = from_nation_name();
		str += " declares war on you.";
	}
*/

	if( viewing_nation_recno == from_nation_recno )
	{
		str = text_talk.process( text_talk.get_format_str( 67, "YWAR" ),
			to_nation_name() );
	}
	else
	{
		str = text_talk.process( text_talk.get_format_str( 68, "NWAR" ),
			from_nation_name() );
	}
}
//------- End of function TalkMsg::declare_war ------//


//----- Begin of function TalkMsg::give_tribute ------//
//
// <char*> tributeStr - either "tribute" or "aid".
//
// talk_para1 - amount of the tribute.
//
void TalkMsg::give_tribute(const char* tributeStr)
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offers you <$999> in aid/tribute.
	// You offer <King>'s Kingdom <$999> in aid/tribute.
	//
	// Reply:
	// <King>'s Kingdom accepts/rejects your aid/tribute of <$999>.
	// You accept/reject the <$999> in aid/tribute from <King>'s Kingdom.
	//
	//---------------------------------------------//

/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You offer ";
			str += to_nation_name();
			str += " ";
			str += misc.format(talk_para1, 2);
			str += " in ";
			str += tributeStr;
			str += ".";
		}
		else
		{
			str  = from_nation_name();
			str += " offers you ";
			str += misc.format(talk_para1, 2);
			str += " in ";
			str += tributeStr;
			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepts your ";
			else
				str += " rejects your ";

			str += tributeStr;
			str += " of ";
			str += misc.format(talk_para1, 2);
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You accept the ";
			else
				str = "You reject the ";

			str += misc.format(talk_para1, 2);
			str += " in ";
			str += tributeStr;
			str += " from ";
			str += from_nation_name();
			str += ".";
		}
	}
*/
	// ignore tributeStr

	switch( talk_id )
	{
	case TALK_GIVE_TRIBUTE:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 69, "YPOT" ),
					to_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 70, "NPOT" ),
					from_nation_name(), talk_para1 );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 71, "NAOT" ),
						to_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 72, "NROT" ),
						to_nation_name(), talk_para1 );
			else
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 73, "YAOT" ),
						from_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 74, "YROT" ),
						from_nation_name(), talk_para1 );
		}
		break;
	case TALK_GIVE_AID:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 75, "YPOA" ),
					to_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 76, "NPOA" ),
					from_nation_name(), talk_para1 );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 77, "NAOA" ),
						to_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 78, "NROA" ),
						to_nation_name(), talk_para1 );
			else
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 79, "YAOA" ),
						from_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 80, "YROA" ),
						from_nation_name(), talk_para1 );
		}
		break;
	default:
		err_here();
		str = "";
	}
}
//------- End of function TalkMsg::give_tribute ------//


//----- Begin of function TalkMsg::demand_tribute ------//
//
// <int> isAid - 1 if it's a aid, 0 if it's a tribute.
//
// talk_para1 - the amount of the tribute.
//
void TalkMsg::demand_tribute(int isAid)
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests/demands <tribute amount> in aid/tribute
	// from you.
	//
	// You request/demand <tribute amount> in aid/tribute from
	// <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to give/pay you <tribute amount>
	// in aid/tribute.
	//
	// You agree/refuse to give/pay <King>'s Kingdom <tribute amount>
	// in aid/tribute.
	//
	//---------------------------------------------//

/*
	char* aidStr;

	if( isAid )
		aidStr = "aid";
	else
		aidStr = "tribute";

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			if( isAid )
				str = "You request ";
			else
				str = "You demand ";

			str += misc.format(talk_para1,2);
			str += " in ";
			str += aidStr;
			str += " from ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str =  from_nation_name();

			if( isAid )
				str += " requests ";
			else
				str += " demands ";

			str += misc.format(talk_para1,2);
			str += " in ";
			str += aidStr;
			str += " from you.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees";
			else
				str += " refuses";

			if( isAid )
				str += " to give you ";
			else
				str += " to pay you ";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree";
			else
				str = "You refuse";

			if( isAid )
				str += " to give ";
			else
				str += " to pay ";

			str += from_nation_name();
			str += " ";
		}

		str += misc.format(talk_para1,2);
		str += " in ";
		str += aidStr;
		str += ".";
	}
*/
	switch( talk_id )
	{
	case TALK_DEMAND_TRIBUTE:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 81, "YPDT" ),
					to_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 82, "NPDT" ),
					from_nation_name(), talk_para1 );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 83, "NADT" ),
						to_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 84, "NRDT" ),
						to_nation_name(), talk_para1 );
			else
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 85, "YADT" ),
						from_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 86, "YRDT" ),
						from_nation_name(), talk_para1 );
		}
		break;
	case TALK_DEMAND_AID:
		if( reply_type == REPLY_WAITING || !should_disp_reply )
		{
			if( viewing_nation_recno == from_nation_recno )
				str = text_talk.process( text_talk.get_format_str( 87, "YPDA" ),
					to_nation_name(), talk_para1 );
			else
				str = text_talk.process( text_talk.get_format_str( 88, "NPDA" ),
					from_nation_name(), talk_para1 );
		}
		else
		{
			if( viewing_nation_recno == from_nation_recno )
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 89, "NADA" ),
						to_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 90, "NRDA" ),
						to_nation_name(), talk_para1 );
			else
				if( reply_type == REPLY_ACCEPT )
					str = text_talk.process( text_talk.get_format_str( 91, "YADA" ),
						from_nation_name(), talk_para1 );
				else
					str = text_talk.process( text_talk.get_format_str( 92, "YRDA" ),
						from_nation_name(), talk_para1 );
		}
		break;
	default:
		err_here();
		str = "";
	}
}
//------- End of function TalkMsg::demand_tribute ------//


//----- Begin of function TalkMsg::exchange_tech ------//
//
// talk_para1 - id. of the tech the talking nation offers
// talk_para2 - id. of the tech the talking nation requests
// talk_para3 - level of the tech the talking nation offers
// talk_para4 - level of the tech the talking nation requests
//
void TalkMsg::exchange_tech()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offers <tech><version> technology to you
	// in exchange for your <tech><version> technology.
	//
	// You offer <tech><version> technology to <King>'s Kingdom
	// in exchange for <tech><version> technology.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to exchange technology with you.
	//
	// You agree/refuse to exchange technology with <King>'s Kingdom.
	//
	//---------------------------------------------//
/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You offer ";
			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )
			{
				str += " ";
				str += misc.roman_number(talk_para2);
			}

			str += " technology to ";
			str += to_nation_name();
			str += " in exchange for ";

			str += tech_res[talk_para3]->tech_des();

			if( talk_para4 )
			{
				str += " ";
				str += misc.roman_number(talk_para4);
			}

			str += ".";
		}
		else
		{
			str  = from_nation_name();
			str += " offers ";
			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )		// Ships do not have different versions
			{
				str += " ";
				str += misc.roman_number(talk_para2);
			}

			str += " technology to you in exchange for your ";

			str += tech_res[talk_para3]->tech_des();

			if( talk_para4 )
			{
				str += " ";
				str += misc.roman_number(talk_para4);
			}

			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees to exchange technology with you.";
			else
				str += " refuses to exchange technology with you.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree to exchange technology with ";
			else
				str = "You refuse to exchange technology with ";

			str += from_nation_name();
			str += ".";
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		// ####### begin Gilbert 30/6 ########//
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 93, "YPXT" ),
				to_nation_name(), tech_res[talk_para1]->tech_des(), talk_para3,
				tech_res[talk_para2]->tech_des(), talk_para4 );
		else
			str = text_talk.process( text_talk.get_format_str( 94, "NPXT" ),
				from_nation_name(), tech_res[talk_para1]->tech_des(), talk_para3,
				tech_res[talk_para2]->tech_des(), talk_para4 );
		// ####### end Gilbert 30/6 ########//
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 95, "NAXT" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 96, "NRXT" ),
					to_nation_name() );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 97, "YAXT" ),
					from_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 98, "YRXT" ),
					from_nation_name() );
	}
}
//------- End of function TalkMsg::exchange_tech ------//


//----- Begin of function TalkMsg::request_military_aid ------//
//
void TalkMsg::request_military_aid()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests immediate military aid from you.
	// You request immediate military aid from <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees to immediately send your requested
	// military aid.
	// <King>'s Kingdom denies you your requested military aid.
	//
	// You agree to immediately send military aid to <King>'s Kingdom.
	// You refuse to send military aid to <King>'s Kingdom.
	//
	//---------------------------------------------//
/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You request immediate military aid from ";
			str +=  to_nation_name();
			str += ".";
		}
		else
		{
			str =  from_nation_name();
			str += " requests immediate military aid from you.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees to immediately send your requested military aid.";
			else
				str += " denies you your requested military aid.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree to immediately send military aid to ";
			else
				str = "You refuse to send military aid to ";

			str += from_nation_name();
			str += ".";
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 35, "YPMA" ),
				to_nation_name() );
		else
			str = text_talk.process( text_talk.get_format_str( 36, "NPMA" ),
				from_nation_name() );
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 37, "NAMA" ),
					to_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 38, "NRMA" ),
					to_nation_name() );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 39, "YAMA" ),
					from_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 40, "YRMA" ),
					from_nation_name() );
	}
}
//------- End of function TalkMsg::request_military_aid ------//


//----- Begin of function TalkMsg::request_trade_embargo ------//
//
// talk_para1 - the nation to have a trade embargo on.
//
void TalkMsg::request_trade_embargo()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests you to join an embargo on trade with
	// <King B>'s Kingdom.
	//
	// You request <King>'s Kingdom to join an embargo on trade with
	// <King B>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to join an embargo on trade
	// with <King B>'s Kingdom.
	//
	// You agree/refuse to join an embargo on trade with <King B>'s Kingdom
	// as requested by <King>'s Kingdom.
	//
	//---------------------------------------------//

/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You request ";
			str += to_nation_name();
		}
		else
		{
			str  = from_nation_name();
			str += " requests you";
		}

		str += " to join an embargo on trade with ";
		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees";
			else
				str += " refuses";

			str += " to join an embargo on trade with ";
			if (nation_array.is_deleted(talk_para1))
			{
				str += "the destroyed ";
				str += nation_array.custom_nation_name_array[talk_para1];
			}
			else
			{
				str += nation_array.custom_nation_name_array[talk_para1];
					//nation_array[talk_para1]->nation_name();
				str += nation_color_code_str(talk_para1);
			}
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "You agree";
			else
				str = "You refuse";

			str += " to join an embargo on trade with ";
			if (nation_array.is_deleted(talk_para1))
			{
				str += "the destroyed ";
				str += nation_array.custom_nation_name_array[talk_para1];
			}
			else
			{
				str += nation_array.custom_nation_name_array[talk_para1];
					//nation_array[talk_para1]->nation_name();
				str += nation_color_code_str(talk_para1);
			}
			str += " as requested by ";
			str += from_nation_name();
			str += ".";
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 41, "YPTE" ),
				to_nation_name(), third_nation_name(talk_para1) );
		else
			str = text_talk.process( text_talk.get_format_str( 42, "NPTE" ),
				from_nation_name(), third_nation_name(talk_para1) );
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 43, "NATE" ),
					to_nation_name(), third_nation_name(talk_para1) );
			else
				str = text_talk.process( text_talk.get_format_str( 44, "NRTE" ),
					to_nation_name(), third_nation_name(talk_para1) );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 45, "YATE" ),
					from_nation_name(), third_nation_name(talk_para1) );
			else
				str = text_talk.process( text_talk.get_format_str( 46, "YRTE" ),
					from_nation_name(), third_nation_name(talk_para1) );
	}
}
//------- End of function TalkMsg::request_trade_embargo ------//


//----- Begin of function TalkMsg::request_surrender ------//
//
void TalkMsg::request_surrender()
{
	//---------------------------------------------//
	//
	// Send:
	//
	// To unite our two Kingdoms under his rule, King
	// <King name> offers <amount> for your throne.
	//
	// You offer <amount> for the throne of <King>'s
	// Kingdom.
	//
	// Reply:
	//
	// King <king name> refuses to dishonor himself by
	// selling his throne!
	//
	// King <king name> agrees to take your money in
	// exchange for his throne.
	//
	// You refuse to dishonor yourself by selling your
	// throne to <King>'s kingdom.
	//
	//---------------------------------------------//
/*
	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "You offer ";
			str += talk_para1*10;	// *10 to restore its original value.
			str += " for the throne of ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "To unite our two Kingdoms under his rule, ";
			str += from_king_name();
			str += " offers ";
			str += talk_para1*10;	// *10 to restore its original value.
			str += " for your throne.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = to_king_name();

			if( reply_type == REPLY_ACCEPT )
				str += " agrees to take your money in exchange for his throne.";
			else
				str += " refuses to dishonor himself by selling his throne!";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
			{
				str = "You agree to take the money in exchange for your throne.";
			}
			else
			{
				str  = "You refuse to dishonor yourself by selling your throne to ";
				str += from_nation_name();
				str += ".";
			}
		}
	}
*/

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
			str = text_talk.process( text_talk.get_format_str( 99, "YPRS" ),
				to_nation_name(), talk_para1*10 );	// talk_para1*10 to restore its original value.
		else
			str = text_talk.process( text_talk.get_format_str( 100, "NPRS" ),
				from_king_name(), talk_para1*10 ); // talk_para1*10 to restore its original value.
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 101, "NARS" ),
					to_king_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 102, "NRRS" ),
					to_king_name() );
		else
			if( reply_type == REPLY_ACCEPT )
				str = text_talk.process( text_talk.get_format_str( 103, "YARS" ),
					from_nation_name() );
			else
				str = text_talk.process( text_talk.get_format_str( 104, "YRRS" ),
					from_nation_name() );
	}
}
//------- End of function TalkMsg::request_surrender ------//


//----- Begin of function TalkMsg::surrender ------//
//
void TalkMsg::surrender()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offerrrendered to you.
	// You have surrendered to <King>'s Kingdom.
	//
	//---------------------------------------------//

	if( viewing_nation_recno == from_nation_recno )
	{
//		str  = "You have surrendered to ";
//		str += to_nation_name();
//		str += ".";
		str = text_talk.process( text_talk.get_format_str( 105, "YSUR" ),
			to_nation_name() );
	}
	else
	{
		// str  = from_nation_name();
		// str += " has surrendered to you.";
		str = text_talk.process( text_talk.get_format_str( 106, "NSUR" ),
			from_nation_name() );
	}
}
//------- End of function TalkMsg::surrender ------//

// #endif
