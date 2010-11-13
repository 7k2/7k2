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

//Filename    : OF_MAGIA.CPP
//Description : Firm Magic Tor

#include <ONATION.H>
#include <OINFO.H>
#include <OWORLDMT.H>
// #include <OWARPT.H>
#include <OF_MAGI.H>
#include <OW_TRV.H>
#include <math.h>

//--------- Begin of function FirmMagic::process_ai ---------//

void FirmMagic::process_ai()
{
	//---- think about casting a magic spell now ----//

	think_cast_magic();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmMagic::process_ai -----------//


//------- Begin of function FirmMagic::think_del -----------//
//
// Think about deleting this firm.
//
int FirmMagic::think_del()
{
	return 0;
}
//--------- End of function FirmMagic::think_del -----------//


//----- Begin of function FirmMagic::think_cast_magic ------//
//
void FirmMagic::think_cast_magic()
{
	//---- wait until the magic points are full ----//

	if( cast_delay < MAX_CAST_MAGIC_DELAY ||			// only cast magic when the nation is at war with an enemy
		 !nation_array[nation_recno]->is_at_war() )
	{
		return;
	}

	int random = m.random( magic_id_upper_limit - magic_id_lower_limit +1 ) + 
		magic_id_lower_limit;

	switch( random )
	{
		case MAGIC_DESTROY_BUILDING:
			think_destroy_building();
			break;

		case MAGIC_FREEZE_UNIT:
		case MAGIC_DAMAGE_AREA:
		case MAGIC_GOD_POWER:
			think_cast_to_area(random);
			break;

		case MAGIC_TELEPORT:
			// think_cast_to_area(MAGIC_FREEZE_UNIT);
			break;
		
		case MAGIC_FREEZE_LASER:
		case MAGIC_LASER:
			think_cast_to_top(random);
			break;
	}
}
//------ End of function FirmMagic::think_cast_magic -------//


//------- Begin of function FirmMagic::think_destroy_building -------//
//
void FirmMagic::think_destroy_building()
{
	int magicId = MAGIC_DESTROY_BUILDING;

	if( !can_cast(magicId) )
		return;

	//--- randomly select a building froma hostile nation to attack ---//

	for( int loopId=1 ; loopId<=2 ; loopId++ )
	{
		int firmRecno = m.random(firm_array.size())+1;
		int curRating, bestRating=0;
		Firm* bestFirm = NULL;

		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( ++firmRecno > firm_array.size() )
				firmRecno = 1;

			if( firm_array.is_deleted(firmRecno) )
				continue;

			Firm* firmPtr = firm_array[firmRecno];

			if( !firmPtr->nation_recno )
				continue;

			if( firmPtr->nation_ptr()->get_relation_status(nation_recno) != RELATION_HOSTILE )
				continue;

			//--- if loopId==1, select buildings that are already damaged to destroy ---//

			if( loopId==1 )
			{
				if( firmPtr->hit_points == firmPtr->max_hit_points() )
					continue;

				//--- select the most vulerable build to attack ---//

				curRating = (int) (100 - 100 * firmPtr->hit_points / firmPtr->max_hit_points());

				if( curRating > bestRating )
				{
					bestRating = curRating;
					bestFirm   = firmPtr;
				}
			}
			else //--- if loopId==2, randomly select any building to damage ---//
			{
				bestFirm = firmPtr;
				break;
			}
		}

		if( bestFirm )
		{
			cast_magic( magicId, bestFirm->center_x, bestFirm->center_y, 0, COMMAND_AI );
			return;
		}
	}
}
//-------- End of function FirmMagic::think_destroy_building --------//


//------- Begin of function FirmMagic::think_cast_to_area -------//
//
void FirmMagic::think_cast_to_area(int magicId)
{
	if( !can_cast(magicId) )
		return;

	//--- locate a battle involving our troops ---//

	int bestWeight = 0;
	int bestXLoc;
	int bestYLoc;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( !unitPtr->is_visible() )
			continue;

		if( unitPtr->nation_recno == nation_recno &&
			 unitPtr->last_attack_date >= info.game_date-1 )
		{
			int radius = magic_info_array[magicId-1].cast_radius;
			WorldScanTraverser scanner;
			int cx = unitPtr->next_x_loc();
			int cy = unitPtr->next_y_loc();
			scanner.init( cx, cy, unitPtr->loc_width, radius * 3 );
			int wArray[3][3];
			memset( wArray, 0, sizeof(wArray) );
			for( scanner.restart(); !scanner.is_finish(); scanner.next() )
			{
				Unit *targetUnit = unit_array[ scanner.get_unit() ];
				if( targetUnit->nation_recno != nation_recno
					&& nation_array.should_attack(nation_recno, targetUnit->nation_recno) )
				{
					// add weight to one of the nine cell
					int targetX = targetUnit->next_x_loc();
					int targetY = targetUnit->next_y_loc();
					int cellX = 1;
					int cellY = 1; // center cell
					if( targetY < cy-radius )
						cellY = 0; // north cell
					else if( targetY > cy+radius )
						cellY = 2;	// south cell
					if( targetX < cx-radius )
						cellX = 0;
					else if( targetX > cx+radius )
						cellX = 2;
					wArray[cellY][cellX]++;
				}
			}

			// maximize weight

			for( int cellY = 0; cellY < 3; ++cellY )
			{
				for( int cellX = 0; cellX < 3; ++cellX )
				{
					if( wArray[cellY][cellX] > bestWeight )
					{
						bestWeight = wArray[cellY][cellX];
						bestXLoc = cx + (cellX-1) * radius * 2;
						bestYLoc = cy + (cellY-1) * radius * 2;
					}
				}
			}

			// if bestWeight is good enough, fire now

			if( bestWeight >= 8 )
			{
				cast_magic( magicId, bestXLoc, bestYLoc, 0, COMMAND_AI );
				return;
			}
		}
	}

	if( bestWeight > 3 )
	{
		cast_magic( magicId, bestXLoc, bestYLoc, 0, COMMAND_AI );
		return;
	}

	// other consideration
}
//------ End of function FirmMagic::think_cast_to_area -------//


//------- Begin of function FirmMagic::think_cast_to_top -------//
//
void FirmMagic::think_cast_to_top(int magicId)
{
	if( !can_cast(magicId) )
		return;

	//--- locate surrounding, count of own unit and enemy unit ---//

	int ownUnit = 0;
	int friendUnit = 0;
	int enemyUnit = 0;

	WorldScanTraverser scanner;
	scanner.init( loc_x1, loc_y1, loc_width(), magic_info_array[magicId-1].cast_radius );
	err_when( magic_info_array[magicId-1].cast_range != 0 );	// assume magic is around the building

	for( scanner.restart(); !scanner.is_finish(); scanner.next() )
	{
		Unit* unitPtr = unit_array[ scanner.get_unit() ];

		if( unitPtr->nation_recno == nation_recno )
		{
			ownUnit++;
		}
		else if( nation_array.should_attack(nation_recno, unitPtr->nation_recno) )
		{
			if( unitPtr->cur_order.mode == UNIT_ATTACK
				&& !base_obj_array.is_deleted(unitPtr->cur_order.para)
				&& base_obj_array[unitPtr->cur_order.para]->nation_recno == nation_recno )
			{
				enemyUnit++;
			}
		}
		else
		{
			friendUnit++;
		}
	}


	if( enemyUnit > 10*hit_points/max_hit_points() + m.random(10)		// large number of enemy, or going to die, cast
		|| enemyUnit >= 5 && ownUnit >= 5 && (ownUnit-enemyUnit-5) < m.random(20) )	// close force, cast
	{
		cast_magic( magicId, center_x, center_y, 0, COMMAND_AI );
	}
}
//------ End of function FirmMagic::think_cast_to_top -------//
