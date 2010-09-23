// Filename    : OT_NEWS.H
// Description : header file of text resource news


#ifndef __OT_NEWS_H
#define __OT_NEWS_H

#include <OTXTRES.H>

class TextResNews : public TextResource
{
public:
	void	init();

	char* str_town_of_nation( char* townName, char* nationName);
	char* str_firm_near_town( char* firmName, char* townName );
	char* str_firm_of_nation( char* firmName, char* nationName);
	char* str_firm_near_town_of_nation( char *firmName, char *townName, char* nationName );

	char* str_time_left_ym( int years, int months );
};

extern TextResNews text_news;


#endif
