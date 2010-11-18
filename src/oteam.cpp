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

//Filename: OTEAM.CPP
//Description: Functions for the Team class

#include <ounit.h>


//----------- Begin of function TeamInfo::TeamInfo ---------//

TeamInfo::TeamInfo()
{
	memset( this, 0, sizeof(TeamInfo) );
}
//----------- End of function TeamInfo::TeamInfo ---------//


//--------- Begin of function TeamInfo::add_member ---------//
//
// Add a unit to this team.
//
void TeamInfo::add_member(int unitRecno)
{
	err_when( member_count >= MAX_TEAM_MEMBER );

	member_unit_array[member_count++] = unitRecno;
}
//----------- End of function TeamInfo::add_member -----------//


//--------- Begin of function TeamInfo::del_member ---------//
//
// Delete a specific member of the team led by this leader.
//
void TeamInfo::del_member(int unitRecno)
{
	err_when( member_count > MAX_TEAM_MEMBER );

	for( int i=0 ; i<member_count ; i++ )
	{
		if( member_unit_array[i] == unitRecno )
		{
			m.del_array_rec( member_unit_array, member_count,
								  sizeof( member_unit_array[0] ), i+1 );

			member_count--;
			return;
		}
	}

	//-------------------------------------------------------//
	//
	// Note: for rebels and monsters, although they have
   //       leader_unit_recno, their team_info is not used.
	//       So del_member() won't be able to match the
   //       unit in its member_unit_array[].
   //
   //-------------------------------------------------------//
}
//----------- End of function TeamInfo::del_member -----------//


//--------- Begin of function TeamInfo::validate ---------//
//
// Validate the member in this commander's team. If there
// are any units with hit_points <= 0, delete them.
//
// Those unit may just be killed, so soon that the Unit's set_die()
// function hsa been called yet. validate() function must
// be called before all issunig any new team actions.
//
void TeamInfo::validate()
{
	int unitRecno;

	for( int i=member_count-1 ; i>=0 ; i-- )
	{
		unitRecno = member_unit_array[i];

		if( unit_array.is_deleted(unitRecno) )
		{
			err_when( member_count > MAX_TEAM_MEMBER );

			m.del_array_rec( member_unit_array, member_count,
								  sizeof( member_unit_array[0] ), i+1 );

			member_count--;
		}
   }
}
//----------- End of function TeamInfo::validate -----------//


//--------- Begin of function TeamInfo::reset ---------//
//
void TeamInfo::reset()
{
	int unitRecno;

	for( int i=member_count-1 ; i>=0 ; i-- )
	{
		unitRecno = member_unit_array[i];

		if( unit_array.is_deleted(unitRecno) )
			continue;

		unit_array[unitRecno]->leader_unit_recno = 0;
	}

   member_count = 0;
}
//----------- End of function TeamInfo::reset -----------//

