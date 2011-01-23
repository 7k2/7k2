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

// Filename    : OT_TALK.H
// Description : Header file of talk res


#ifndef __OT_TALK_H
#define __OT_TALK_H

#include <otxtres.h>

class TextResTalk : public TextResource
{
public:
	void	init();

	const char* str_relation_status(char status );
	char* str_trade_treaty();

	char* str_too_many_msg();
	char* str_msg_sent();
	char* str_cancel_();
	char* str_continue();
	char* str_ask_tribute();
	char* str_ask_aid();
	char* str_ask_embargo();
	char* str_ask_declare_war();
	char* str_ask_buy_food();
	char* str_ask_food_price();
	char* str_ask_offer_tech();
	char* str_ask_request_tech();
	char* str_ask_surrender_price();
	char* str_ask_confirm_surrender( char* nationName );
	char* str_confirm_();
	char* str_accept_();
	char* str_reject_();

	char* str_msg_name(int talkMsgId);


};

extern TextResTalk text_talk;

#endif
