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

// Filename    : OF_SPEC.CPP
// Description : Firm Stable

#include <OF_SPEC.H>
#include <ALL.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OTOWN.H>

//--------- Begin of function FirmSpecial::FirmSpecial() ------//

FirmSpecial::FirmSpecial()
{
	memset( sizeof(FirmTrain)+(char *)this, 0, sizeof(FirmSpecial)-sizeof(FirmTrain) );
}
//--------- End of function FirmSpecial::FirmSpecial() ------//



// ---------- Begin of function FirmSpecial::can_set_rally_point ------//
//
// whether the firm can set rally point
//
// [int] destBaseObjRecno		- destination base obj recno, 0 for setting position
//
int FirmSpecial::can_set_rally_point(int destBaseObjRecno)
{
	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		Firm *firmPtr = base_obj_array[destBaseObjRecno]->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return firmPtr->is_human();
			}
		}
	}

	return 0;
}
// ---------- End of function FirmSpecial::can_set_rally_point ------//
