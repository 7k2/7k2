// Filename    : OT_TALK.H
// Description : Header file of talk res


#ifndef __OT_TALK_H
#define __OT_TALK_H

#include <OTXTRES.H>

class TextResTalk : public TextResource
{
public:
	void	init();

	char*	str_relation_status(char status );
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