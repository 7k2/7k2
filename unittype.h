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

//Filename    : UNITTYPE.H
//Description : Header for defining unit types

#ifndef __UNITTYPE_H
#define __UNITTYPE_H

//---------- Define unit types ------------//

enum
{
	UNIT_NORMAN_CIVIL=1,
	UNIT_NORMAN_INF,
	UNIT_NORMAN_SPU,
	UNIT_VIKING_CIVIL,
	UNIT_VIKING_INF,
	UNIT_VIKING_SPU,
	UNIT_CELTIC_CIVIL,
	UNIT_CELTIC_INF,
	UNIT_CELTIC_SPU,
	UNIT_ROMAN_CIVIL,
	UNIT_ROMAN_INF,
	UNIT_ROMAN_SPU,
	UNIT_GREEK_CIVIL,
	UNIT_GREEK_INF,
	UNIT_GREEK_SPU,
	UNIT_CARTH_CIVIL,
	UNIT_CARTH_INF,
	UNIT_CARTH_SPU,
	UNIT_CHINESE_CIVIL,
	UNIT_CHINESE_INF,
	UNIT_CHINESE_SPU,
	UNIT_JAPANESE_CIVIL,
	UNIT_JAPANESE_INF,
	UNIT_JAPANESE_SPU,
	UNIT_MONGOL_CIVIL,
	UNIT_MONGOL_INF,
	UNIT_MONGOL_SPU,
	UNIT_EGYPTIAN_CIVIL,
	UNIT_EGYPTIAN_INF,
	UNIT_EGYPTIAN_SPU,
	UNIT_PERSIAN_CIVIL,
	UNIT_PERSIAN_INF,
	UNIT_PERSIAN_SPU,
	UNIT_INDIAN_CIVIL,
	UNIT_INDIAN_INF,
	UNIT_INDIAN_SPU,
	UNIT_WAGON,
	UNIT_CARAVAN,
	UNIT_CATAPULT,
	UNIT_BALLISTA,
	UNIT_FLAMETHROWER,
	UNIT_CANNON,
	UNIT_EXPLOSIVE_CART,
	UNIT_F_BALLISTA,

	UNIT_M_CANNON,
	UNIT_M_F_BALL,
	UNIT_M_SAW,

	UNIT_F_CANNON,
	UNIT_FR_BALLISTA,
	UNIT_F_CATAPULT,

	UNIT_NORMAN_GOD,
	UNIT_VIKING_GOD,
	UNIT_CELTIC_GOD,
	UNIT_ROMAN_GOD,
	UNIT_GREEK_GOD,
	UNIT_CARTH_GOD,
	UNIT_CHINESE_GOD,
	UNIT_JAPANESE_GOD,
	UNIT_MONGOL_GOD,
	UNIT_EGYPTIAN_GOD,
	UNIT_PERSIAN_GOD,
	UNIT_INDIAN_GOD,

	UNIT_PLANT,
	UNIT_BROO,
	UNIT_DEMON,
	UNIT_MOSSE,
	UNIT_ROCK,
	UNIT_SKULLAR,
	UNIT_CRACK,

	UNIT_BEE,
	UNIT_TERMITE,
	UNIT_JAGUAR,
	UNIT_GORILLA,
	UNIT_RHINO,
	UNIT_LAST					// keep it last
};

//-------- Define type count --------//

enum { MAX_UNIT_TYPE = UNIT_LAST-1,
		 MAX_WEAPON_TYPE = 6,		// no. of types of weapons
		 MAX_SHIP_TYPE = 0, 			// no. of types of ships
	  };

//------------------------------------//

#endif