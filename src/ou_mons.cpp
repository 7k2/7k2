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

//Filename   : OU_MONS.CPP
//Description: Unit Monster

#include <OWORLD.H>
#include <OSITE.H>
#include <ONEWS.H>
#include <OTOWN.H>
#include <OFIRM.H>
#include <ONATION.H>
#include <OMONSRES.H>
#include <OU_MONS.H>
#include <OT_UNIT.H>


//--------- Begin of function UnitMonster::unit_name ---------//
//
// [int] withTitle - whether return a string with the title of the unit
//                   or not. (default: 1)
//
// [int] firstNameOnly - whether return the first word of the name only
//							(default: 0)
//
char* UnitMonster::unit_name(int withTitle, int firstNameOnly)
{
	char* monsterName;

	if( rank_id == RANK_KING )		// use the player name
		monsterName = nation_array[nation_recno]->king_name();
	else
		monsterName = monster_res.get_name(name_id);

	if (withTitle)
	{		
		return text_unit.str_unit_titled( race_id, monsterName, rank_id );
	}
	else
	{
		return monsterName;
	}
}
//--------- End of function UnitMonster::unit_name ---------//

