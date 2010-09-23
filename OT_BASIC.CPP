// Filename    : OT_BASIC.CPP
// Description : text resource basic

#include <OT_BASIC.H>
#include <ALL.H>

void TextResBasic::init()
{
	TextResource::init( DIR_RES"T_BASIC.RES" );
}

// text id 1
// [LNGD]%2j{January|February|March|April|May|June|July|August|September|October|November|December} %1d, %3,4d
char*	TextResBasic::str_long_date(int day, int month, int year )
{
	return process( get_format_str( 1, "LNGD" ), day, month, year);
}

// text id 2
// [SHRD]%2j{Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec} %1d, %3,4d
char*	TextResBasic::str_short_date(int day, int month, int year )
{
	return process( get_format_str( 2, "SHRD" ), day, month, year);
}

// text id 3 - 14
// [MON1] - [MON9], [MONA], [MONB], [MONC]
char* TextResBasic::str_month_string(int month)
{
	switch( month )
	{
	case 1: return get_format_str( 3, "MON1" );
	case 2: return get_format_str( 4, "MON2" );
	case 3: return get_format_str( 5, "MON3" );
	case 4: return get_format_str( 6, "MON4" );
	case 5: return get_format_str( 7, "MON5" );
	case 6: return get_format_str( 8, "MON6" );
	case 7: return get_format_str( 9, "MON7" );
	case 8: return get_format_str( 10, "MON8" );
	case 9: return get_format_str( 11, "MON9" );
	case 10: return get_format_str( 12, "MONA" );
	case 11: return get_format_str( 13, "MONB" );
	case 12: return get_format_str( 14, "MONC" );
	default:
		return "?";
	}
}

// text id 15
// [BOX1]Ok
char* TextResBasic::str_box_ok()
{
	return get_format_str( 15, "BOX1" );
}

// text id 16
// [BOX2]Cancel
char* TextResBasic::str_box_cancel()
{
	return get_format_str( 16, "BOX2" );
}
