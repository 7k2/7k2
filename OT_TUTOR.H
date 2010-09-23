// Filename    : OT_TUTOR.H
// Description : header file of tutorial text resource


#ifndef __OT_TUTOR_H
#define __OT_TUTOR_H


#include <OTXTRES.H>


class TextResTutorial : public TextResource
{
public:
	void	init();

	// select tutorial menu

	char* str_tutor_1();
	char* str_tutor_2();
	char* str_tutor_3();
	char* str_tutor_quick();

	char* str_click_to_continue();
	char* str_guidelines();
	char* str_task_of_total( int curPage, int totalPage );
	char* str_page_of_total( int curPage, int totalPage );
	char* str_close_hints();

	// select tutor
	char*	str_no_tutorial();
};

extern TextResTutorial text_tutorial;

#endif
