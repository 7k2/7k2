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