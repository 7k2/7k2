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

//Filename   : ON_HUMAN.CPP
//Description: Functions for the class NationHuman

#include <ONATION2.H>
#include <OGAME.H>

//--------- Begin of function NationHuman::process_ai --------//

void NationHuman::process_ai()
{
	//------- process parent class AI ------//

	int nationRecno = nation_recno;

	Nation::process_ai();

	if( nation_array.is_deleted(nationRecno) )
		return;

	//------ think about surrender and uniting against a big enemy ------//

	if( info.game_date%60 == nation_recno%60 )
	{
		if( think_surrender() )
			return;

		if( think_unite_against_big_enemy() )
			return;
	}
}
//---------- End of function NationHuman::process_ai --------//


//--------- Begin of function NationHuman::process_ai_main --------//

int NationHuman::process_ai_main(int mainActionId)
{
	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return 0;	

	if( Nation::process_ai_main(mainActionId) )
		return 1;

	return 0;
}
//---------- End of function NationHuman::process_ai_main --------//

