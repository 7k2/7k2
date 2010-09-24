// Filename    : OT_BASIC.H
// Description : header file of text resource basic

#ifndef __OT_BASIC_H
#define __OT_BASIC_H

#include <OTXTRES.H>

class TextResBasic : public TextResource
{
public:
	void	init();

	// date class text
	char*	str_long_date(int day, int month, int year );
	char*	str_short_date(int day, int month, int year );
	char* str_month_string(int month);

	// box class text
	char* str_box_ok();
	char* str_box_cancel();
};

extern TextResBasic text_basic;

#endif