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

// Filename    : OT_ITEM.CPP
// Description : text resource item

#include <OT_ITEM.H>
#include <ALL.H>


void TextResItem::init()
{
	TextResource::init( DIR_RES"T_ITEM.RES" );
}


char*	TextResItem::str_item_name(int itemId)
{
	if( !itemId )
		return "";

	static char checkMark[CHECK_MARK_STRLEN+1] = "N000";
	err_when( itemId >= 100);
	checkMark[2] = itemId / 10 + '0';
	checkMark[3] = itemId % 10 + '0';
	return get_format_str( 0+itemId, checkMark );
}

char*	TextResItem::str_item_desc(int itemId)
{
	if( !itemId )
		return "";

	static char checkMark[CHECK_MARK_STRLEN+1] = "D000";
	err_when( itemId >= 100);
	checkMark[2] = itemId / 10 + '0';
	checkMark[3] = itemId % 10 + '0';
	return get_format_str( 40+itemId, checkMark );
}

