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

// Filename    : OT_TUTOR.H
// Description : header file of tutorial text resource


#ifndef __OT_TUTOR_H
#define __OT_TUTOR_H


#include <otxtres.h>


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
