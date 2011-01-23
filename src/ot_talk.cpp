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

// Filename    : OT_TALK.CPP
// Description : Text Resource Talk


#include <ot_talk.h>
#include <all.h>
#include <onationb.h>


void TextResTalk::init()
{
	TextResource::init( DIR_RES"T_TALK.RES" );
}


// text id 1 - 5
// [REL0]War
// [REL1]Unfriendly
// [REL2]Neutral
// [REL3]Friendly
// [REL4]Alliance
const char* TextResTalk::str_relation_status(char status)
{
	switch( status )
	{
	case RELATION_HOSTILE:
		return get_format_str( 1, "REL0" );
	case RELATION_TENSE:
		return get_format_str( 2, "REL1" );
	case RELATION_NEUTRAL:
		return get_format_str( 3, "REL2" );
	case RELATION_FRIENDLY:
		return get_format_str( 4, "REL3" );
	case RELATION_ALLIANCE:
		return get_format_str( 5, "REL4" );
	default:
		return "";
	};
}

// text id 6
// [TRAD]Trade Treaty
char* TextResTalk::str_trade_treaty()
{
	return get_format_str( 6, "TRAD" );
}


// text id 7 - 10 reserved


// text id 11 - 16 trade treaty
// [YPTT]You propose a trade treaty to %1s.
// [NPTT]%1s proposes a trade treaty to you.
// [NATT]%1s accepts your proposed trade treaty.
// [NRTT]%1s rejects your proposed trade treaty.
// [YATT]You accept the trade treaty proposed by %1s.
// [YRTT]You reject the trade treaty proposed by %1s.

// text id 17 - 22 friendly treaty
// [YPFT]You propose a friendly treaty to %1s.
// [NPFT]%1s proposes a friendly treaty to you.
// [NAFT]%1s accepts your proposed friendly treaty.
// [NRFT]%1s rejects your proposed friendly treaty.
// [YAFT]You accept the friendly treaty proposed by %1s.
// [YRFT]You reject the friendly treaty proposed by %1s.

// text id 23 - 28 alliance treaty
// [YPAT]You propose an alliance treaty to %1s.
// [NPAT]%1s proposes an alliance treaty to you.
// [NAAT]%1s accepts your proposed alliance treaty.
// [NRAT]%1s rejects your proposed alliance treaty.
// [YAAT]You accept the alliance treaty proposed by %1s.
// [YRAT]You reject the alliance treaty proposed by %1s.

// text id 29 - 30 end trade treaty
// [YETT]You terminate your trade treaty with %1s.
// [NETT]%1s terminates its trade treaty with you.

// text id 31 - 32 end friendly treaty
// [YEFT]You terminate your friendly treaty with %1s.
// [NEFT]%1s terminates its friendly treaty with you.

// text id 33 - 34 end alliance treaty
// [YEAT]You terminate your alliance treaty with %1s.
// [NEAT]%1s terminates its alliance treaty with you.

// text id 35 - 40 request military aid
// [YPMA]You request immediate military aid from %1s.
// [NPMA]%1s requests immediate military aid from you.
// [NAMA]%1s agrees to immediately send your requested military aid.
// [NRMA]%1s denies you your requested military aid.
// [YAMA]You agree to immediately send military aid to %1s.
// [YRMA]You refuse to send military aid to %1s.

// text id 41 - 46 request trade emargo
// [YPTE]You request %1s to join an embargo on trade with %2s.
// [NPTE]%1s requests you to join an embargo on trade with %2s.
// [NATE]%1s agrees to join an embargo on trade with %2s.
// [NRTE]%1s refuses to join an embargo on trade with %2s.
// [YATE]You agree to join an embargo on trade with %2s as requested by %1s.
// [YRTE]You refuse to join an embargo on trade with %2s as requested by %1s.

// text id 47 - 52 request cease war
// [YPCW]You request a cease-fire with %1s.
// [NPCW]%1s requests a cease-fire.
// [NACW]%1s agrees to a cease-fire.
// [NRCW]%1s refuses to a cease-fire.
// [YACW]You agree to a cease-fire with %1s.
// [YRCW]You refuse a cease-fire with %1s.

// text id 53 - 58 request declare war
// [YPDW]You request %1s declare war on %2s.
// [NPDW]%1s requests that you declare war on %2s.
// [NADW]%1s agrees to declare war on %2s.
// [NRDW]%1s refuses to declare war on %2s.
// [YADW]You agree to declare war on %2s.
// [YRDW]You refuse to declare war on %2s.

// text id 59 - 65 request buy food
// [YPBF]You request to purchase %2d units of food from %1s.
// [NPBF]%1s requests to purchase %2d units of food from you.
// [NABF]%1s agrees to sell %2d units of food to you.
// [NRBF]%1s refuses to sell %2d units of food to you.
// [YABF]You agree to sell %2d units of food to %1s.
// [YRBF]You refuse to sell %2d units of food to %1s.
// [FPRC]%1s offers %2,2d for 10 units of food.

// text id 66 reserved

// text id 67 - 68 declare war
// [YWAR]You declare war on %1s.
// [NWAR]%1s declares war on you.


// text id 69 - 74 offer tribute
// [YPOT]You offer %1s %2,2d in tribute.
// [NPOT]%1s offers you %2,2d in tribute.
// [NAOT]%1s accepts your tribute of %2,2d.
// [NROT]%1s rejects your tribute of %2,2d.
// [YAOT]You accept the %2,2d in tribute from %1s.
// [YROT]You reject the %2,2d in tribute from %1s.

// text id 75 - 80 offer aid
// [YPOA]You offer %1s %2,2d in aid.
// [NPOA]%1s offers you %2,2d in aid.
// [NAOA]%1s accepts your aid of %2,2d.
// [NROA]%1s rejects your aid of %2,2d.
// [YAOA]You accept the %2,2d in aid from %1s.
// [YROA]You reject the %2,2d in aid from %1s.

// text id 81 - 86 demand tribute
// [YPDT]You demand %2,2d in tribute from %1s.
// [NPDT]%1s demands %2,2d in tribute from you.
// [NADT]%1s agrees to pay you %2,2d in tribute.
// [NRDT]%1s refuses to pay you %2,2d in tribute.
// [YADT]You agree to pay %1s %2,2d in tribute.
// [YRDT]You refuse to pay %1s %2,2d in tribute.

// text id 87 - 92 request aid
// [YPDA]You request %2,2d in aid from %1s.
// [NPDA]%1s requests %2,2d in aid from you.
// [NADA]%1s agrees to give you %2,2d in aid.
// [NRDA]%1s refuses to give you %2,2d in aid.
// [YADA]You agree to give %1s %2,2d in aid.
// [YRDA]You refuse to give %1s %2,2d in aid.

// text id 93 - 98 exchange tech
// [YPXT]You offer %2s %3r technology to %1s in exchange for %4s %5r.
// [NPXT]%1s offers %2s %3r technology to you in exchange for your %4s %5r.
// [NAXT]%1s agrees to exchange technology with you.
// [NRXT]%1s refuses to exchange technology with you.
// [YAXT]You agree to exchange technology with %1s.
// [YRXT]You refuse to exchange technology with %1s.

// text id 99 - 104 request surrender
// [YPRS]You offer %2,2d for the throne of %1s.
// [NPRS]To unite our two Kingdoms under his rule, %1s offers %2,2d for your throne.
// [NARS]%1s agrees to take your money in exchange for his throne.
// [NRRS]%1s refuses to dishonor himself by selling his throne!
// [(never appears)YARS]You agree to take the money from %1s in exchange for your throne to.
// [YRRS]You refuse to dishonor yourself by selling your throne to %1s.

// text id 105 - 106 surrender
// [YSUR]You have surrendered to %1s.
// [NSUR]%1s has surrendered to you.

// text id 107 - 120 reserved

// text id 121
// [MANY]You've sent too many messages to this kingdom. You cannot send any new messages until the existing ones are processed.
char* TextResTalk::str_too_many_msg()
{
	return get_format_str( 121, "MANY" );
}

// text id 122
// [SENT]The message has been sent.
char* TextResTalk::str_msg_sent()
{
	return get_format_str( 122, "SENT" );
}

// text id 123
// [CNCL]Cancel.
char* TextResTalk::str_cancel_()
{
	return get_format_str( 123, "CNCL" );
}

// text id 124
// [CONT]Continue
char* TextResTalk::str_continue()
{
	return get_format_str( 124, "CONT" );
}

// text id 125
// [ASKT]How much tribute?
char* TextResTalk::str_ask_tribute()
{
	return get_format_str( 125, "ASKT" );
}

// text id 126
// [ASKA]How much aid?
char* TextResTalk::str_ask_aid()
{
	return get_format_str( 126, "ASKA" );
}

// text id 127
// [ASKE]Request an embargo on trade with which kingdom?
char* TextResTalk::str_ask_embargo()
{
	return get_format_str( 127, "ASKE" );
}

// text id 128
// [ASKW]Declare war on which kingdom?
char* TextResTalk::str_ask_declare_war()
{
	return get_format_str( 128, "ASKW" );
}

// text id 129
// [ASKF]How much food do you want to purchase?
char* TextResTalk::str_ask_buy_food()
{
	return get_format_str( 129, "ASKF" );
}

// text id 130
// [ASKP]How much do you offer for 10 units of food?
char* TextResTalk::str_ask_food_price()
{
	return get_format_str( 130, "ASKP" );
}

// text id 131
// [OFFT]Technology you offer?
char* TextResTalk::str_ask_offer_tech()
{
	return get_format_str( 131, "OFFT" );
}

// text id 132
// [REQT]Technology you request?
char* TextResTalk::str_ask_request_tech()
{
	return get_format_str( 132, "REQT" );
}

// text id 133
// [SURP]How much do you offer?
char* TextResTalk::str_ask_surrender_price()
{
	return get_format_str( 133, "SURP" );
}

// text id 134
// [CSUR]Do you really want to Surrender to %1s?
char* TextResTalk::str_ask_confirm_surrender( char* nationName )
{
	return process( get_format_str( 134, "CSUR" ), 
		nationName );
}

// text id 135
// [CFRM]Confirm.
char* TextResTalk::str_confirm_()
{
	return get_format_str( 135, "CFRM" );
}

// text id 136
// [ACCP]Accept.
char* TextResTalk::str_accept_()
{
	return get_format_str( 136, "ACCP" );
}

// text id 137
// [REJT]Reject.
char* TextResTalk::str_reject_()
{
	return get_format_str( 137, "REJT" );
}

// text id 138 - 160 reserved

// text id 161 - 179 (180 - 188 reserved)
// [TKM1] - [TKMJ]
char* TextResTalk::str_msg_name(int talkMsgId)
{
	err_when( talkMsgId <= 0 );

	if( talkMsgId == 1 )	// check only the first
	{
		return get_format_str( 160+talkMsgId, "TKM1" );
	}
	return get_format_str( 160+talkMsgId, NULL );
}
