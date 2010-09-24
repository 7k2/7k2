// Filename    : OT_TUTOR.CPP
// Description : tutorialgame menu text resource

#include <OT_TUTOR.H>
#include <ALL.H>


void TextResTutorial::init()
{
	TextResource::init( DIR_RES"T_TUTOR.RES" );
}

// text id 1
// [TUT1]Learning Campaign One
char* TextResTutorial::str_tutor_1()
{
	return get_format_str( 1, "TUT1" );
}

// text id 2
// [TUT2]Learning Campaign Two
char* TextResTutorial::str_tutor_2()
{
	return get_format_str( 2, "TUT2" );
}

// text id 3
// [TUT3]Learning Campaign Three
char* TextResTutorial::str_tutor_3()
{
	return get_format_str( 3, "TUT3" );
}

// text id 4
// [TUTQ]Learning Campaign Quick
char* TextResTutorial::str_tutor_quick()
{
	return get_format_str( 4, "TUTQ" );
}

// text id 5 - 10 reserved

// text id 11
// [CLKC]Click to continue.
char* TextResTutorial::str_click_to_continue()
{
	return get_format_str( 11, "CLCK" );
}

// text id 12
// [GUID]Learning Campaign Guidlines
char* TextResTutorial::str_guidelines()
{
	return get_format_str( 12, "GUID" );
}

// text id 13
// [<task><total tasks>TOFT]%1d of %2d
char* TextResTutorial::str_task_of_total( int curPage, int totalPage )
{
	return process( get_format_str( 13, "TOFT" ),
		curPage, totalPage );
}

// text id 14
// [<page><total page>POFT]Page %1d of %2d
char* TextResTutorial::str_page_of_total( int curPage, int totalPage )
{
	return process( get_format_str( 14, "POFT" ),
		curPage, totalPage );
}

// text id 15
// [CHIN]Close Hints
char* TextResTutorial::str_close_hints()
{
	return get_format_str( 15, "CHIN" );
}

// text id 16
// [NOTU]Tutorial files not found.
char*	TextResTutorial::str_no_tutorial()
{
	return get_format_str( 16, "NOTU" );
}
