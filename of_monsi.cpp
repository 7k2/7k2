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

//Filename    : OF_MONSI.CPP
//Description : Firm Monster

#include <OF_MONS.H>
#include <OMONSRES.H>
#include <OU_MONS.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <ODATE.H>
#include <OSTR.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OSITE.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OTOWN.H>
#include <ONATION.H>



//--------- Begin of function FirmMonster::put_info ---------//
//
void FirmMonster::put_info(int refreshFlag)
{
/*
	disp_basic_info(INFO_Y1, refreshFlag);

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;

	disp_monster_info(INFO_Y1+54, refreshFlag);
*/
}
//----------- End of function FirmMonster::put_info -----------//


//--------- Begin of function FirmMonster::detect_info ---------//
//
void FirmMonster::detect_info()
{
/*
	if( detect_basic_info() )
		return;

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;
*/
}
//----------- End of function FirmMonster::detect_info -----------//


/*
//--------- Begin of function FirmMonster::disp_monster_info ---------//
//
void FirmMonster::disp_monster_info(int dispY1, int refreshFlag)
{
	return;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+22 );

	int x=INFO_X1+4, y=dispY1+3;
}
//----------- End of function FirmMonster::disp_monster_info -----------//
*/

