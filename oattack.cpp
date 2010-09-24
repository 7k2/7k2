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

// Filename   : OATTACK.CPP
// Description : attack attribute

#include <OATTACK.H>


float AttackAttribute::effectiveness ( DefenseAttribute &da )
{
	return 
		( ((int)sturdiness + explosiveness)/2 * da.sturdiness
		+ (100 - da.sturdiness) * heat + wood_favour * da.is_wood ) / 10000.0f + 1.0f;
}
