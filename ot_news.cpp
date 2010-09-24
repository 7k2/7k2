// Filename    : OT_NEWS.H
// Description : text resource news

#include <OT_NEWS.H>
#include <ALL.H>


void TextResNews::init()
{
	TextResource::init( DIR_RES"T_NEWS.RES" );
}

char* TextResNews::str_town_of_nation( char* townName, char* nationName)
{
	return process( get_format_str( 1, "TOFK"), townName, nationName );
}

char* TextResNews::str_firm_near_town( char* firmName, char* townName )
{
	return process( get_format_str( 2, "FOFT"), firmName, townName );
}

char* TextResNews::str_firm_of_nation( char* firmName, char* nationName)
{
	return process( get_format_str( 3, "FOFK"), firmName, nationName );
}

char* TextResNews::str_firm_near_town_of_nation( char *firmName, char *townName, char* nationName )
{
	return process( get_format_str( 4, "FOTK"), firmName, townName, nationName );
}


char* TextResNews::str_time_left_ym( int years, int months )
{
	return process( get_format_str( 5, "TLEF"), years, years && months, months );
}

