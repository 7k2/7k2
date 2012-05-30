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

//Filename   : OPLACE.CPP
//Description: Class Place

#include <osys.h>
#include <oplace.h>
#include <onation.h>
#include <ounit.h>
#include <osprtres.h>
#include <otown.h>
#include <of_camp.h>
#include <of_lair.h>
#include <oconfig.h>
#include <ogame.h>


//----------- define static parameters -------------//

char Place::is_being_deleted = 0; // true only when the firm is to be removed from the firm_array


//-------- Begin of function Place::Place --------//

Place::Place()
{
	memset( sizeof(BaseObj) + (char *)this, 0, sizeof(Place) - sizeof(BaseObj) );
}
//--------- End of function Place::Place --------//


//-------- Begin of function Place::deinit --------//

void Place::deinit()
{
	//----- handle the builder if the town is currently under construction

	if( builder_recno )			// included in free_all_people
	{
		if( !sys.signal_exit_flag )
			mobilize_builder(builder_recno);
		else
			kill_builder(builder_recno);

		builder_recno = 0;
	}
}
//--------- End of function Place::deinit --------//


//---------- Begin of function Place::set_builder --------//
//
// <short> newBuilderRecno - >0 the recno of the new builder unit.
//									   0 just remove the existing builder, do not assign new one.
//
// return: <int> 0-the old builder is killed
//					  1-the builder is changed successfully.
//
int Place::set_builder(short newBuilderRecno)
{
	err_when( under_construction && newBuilderRecno==0 );		// can't remove the construction worker when the firm is under construction

	//------------------------------------//

	short oldBuilderRecno = builder_recno; // store the old builder recno

	builder_recno = newBuilderRecno;

	//-------- assign the new builder ---------//

	if(builder_recno)
	{
		Unit* unitPtr = unit_array[builder_recno];

		if(unitPtr->is_visible())	 // is visible if the unit is not inside the firm location
		{
			builder_region_id = world.get_region_id( unitPtr->cur_x_loc(), unitPtr->cur_y_loc() );
			unitPtr->deinit_sprite();
		}

		err_when( unitPtr->unit_mode != 0 && unitPtr->unit_mode != UNIT_MODE_REBEL );		// UNIT_MODE_REBEL is exceptional as rebels on settle down to a town 

		if( obj_type == OBJ_FIRM )
			unitPtr->set_mode( UNIT_MODE_CONSTRUCT_FIRM, obj_extern_recno );
		else
			unitPtr->set_mode( UNIT_MODE_CONSTRUCT_TOWN, obj_extern_recno );
	}

	if(oldBuilderRecno)
		mobilize_builder(oldBuilderRecno);

	return 1;
}
//---------- End of function Place::set_builder --------//


//--------- Begin of function Place::mobilize_builder --------//

int Place::mobilize_builder(short unitRecno)
{
	//----------- mobilize the builder -------------//
	Unit* unitPtr = unit_array[unitRecno];

	SpriteInfo *spriteInfo = unitPtr->sprite_info;
	int xLoc=loc_x1, yLoc=loc_y1;

	if(!locate_space(is_being_deleted, xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height, unitPtr->mobile_type, builder_region_id) &&
		!world.locate_space(xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height, unitPtr->mobile_type, builder_region_id))
	{
		kill_builder(unitRecno);
		return 0;
	}

	unitPtr->init_sprite(xLoc, yLoc);

	err_when( unitPtr->unit_mode != UNIT_MODE_CONSTRUCT_FIRM &&
				 unitPtr->unit_mode != UNIT_MODE_CONSTRUCT_TOWN );

	unitPtr->set_mode(0);
	return 1;
}
//----------- End of function Place::mobilize_builder --------//


//--------- Begin of function Place::kill_builder ---------//

void Place::kill_builder(short builderRecno)
{
	// ####### begin Gilbert 24/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(unit_array[builderRecno]->unique_id);		// check unit killed
	// ####### end Gilbert 24/2 ########//

	unit_array.del(builderRecno);
}
//----------- End of function Place::kill_builder -----------//


//--------- Begin of function Place::locate_space ---------//

int Place::locate_space(int removeFirm, int &xLoc, int &yLoc, int xLoc2, int yLoc2, int width, int height, int mobileType, int regionId)
{
	int checkXLoc, checkYLoc;

	if(removeFirm)
	{
		//--- if the place is about to be removed, also count the space currently occupied by the place ---//
		int x1 = xLoc;
		int y1 = yLoc;
		int x2 = xLoc2;
		int y2 = yLoc2;

		for(checkYLoc=y1; checkYLoc<=y2; checkYLoc++)
		{
			for(checkXLoc=x1; checkXLoc<=x2; checkXLoc++)
			{
				int occupiedFlag=0;

				for( int y=0 ; y<height && !occupiedFlag ; y++ )
				{
					for( int x=0 ; x<width ; x++ )
					{
						if( !world.get_loc(checkXLoc+x, checkYLoc+y)->can_move(mobileType) )
						{
							occupiedFlag = 1;
							break;
						}
					}
				}

				if( !occupiedFlag )
				{
					xLoc = checkXLoc;
					yLoc = checkYLoc;
					return 1;
				}
			}
		}
	}
	else
	{
		checkXLoc = loc_x1;
		checkYLoc = loc_y1;

		if(!world.locate_space(checkXLoc, checkYLoc, xLoc2, yLoc2, width, height, mobileType, regionId))
		{
			return 0;
		}
		else
		{
			xLoc = checkXLoc;
			yLoc = checkYLoc;
			return 1;
		}
	}

	return 0;
}
//----------- End of function Place::locate_space -----------//


//-------- Begin of function Place::create_unit ---------//
//
// Create a unit on a given location.
//
// return: <int> recno of the unit created.
//
int Place::create_unit(int unitId)
{
	int xLoc, yLoc;

	if( !create_unit_locate_space(unitId, xLoc, yLoc) )
		return 0;

	//------------ add the unit now ----------------//

	int unitNationRecno;

	switch( obj_type )
	{
		case OBJ_TOWN:
			unitNationRecno = town_array[obj_extern_recno]->nation_recno;
			break;

		case OBJ_FIRM:
			unitNationRecno = firm_array[obj_extern_recno]->nation_recno;
			break;

		default:
			err_here();
	}

	int unitRecno = unit_array.add_unit( unitId, unitNationRecno, RANK_SOLDIER, 0, xLoc, yLoc );

	return unitRecno;
}
//-------- End of function Place::create_unit ---------//


//---------- Begin of function Place::create_unit_locate_space --------//
//
// Locate a space for a new unit.
//
// <int>  unitId  	- id. of the unit
// <int&> xLoc, yLoc - reference vars for returning the location.
//
// return: <int> 1-successful, 0-failed.
//
int Place::create_unit_locate_space(int unitId, int& xLoc, int& yLoc)
{
	if( !is_being_deleted )		// if the place is being deleted, create the units in the area of the building
	{
/*
		//---- code for a troop facing south -----//

		//------ create a unit on the inner 3x3 rows -----//

		static short xOffset[] = { 0, -1, 1 };

		xLoc = center_x;
		yLoc = loc_y2+1;

		for( yLoc=loc_y2+1 ; yLoc<=loc_y2+3 ; yLoc++ )
		{
			for( int i=0 ; i<3 ; i++ )
			{
				xLoc = center_x + xOffset[i];

				if( can_create_unit(unitId, xLoc, yLoc) )
					return 1;
			}
		}

		//------- create a unit on the outer rows -------//

		for( int i=0 ; i<10 ; i++ )		// maximum 10 columns
		{
			if( i%2==0 )
				xLoc = center_x-2-i/2;
			else
				xLoc = center_x+2+i/2;

			for( int y=1 ; y<=3 ; y++ )
			{
				yLoc = loc_y2 + y;

				if( can_create_unit(unitId, xLoc, yLoc) )
					return 1;
			}
		}
*/
		//---- code for a troop facing east -----//

		//------ create a unit on the inner 3x3 rows -----//

		static short yOffset[] = { 0, -1, 1 };

		yLoc = center_y;
		xLoc = loc_x2+1;

		for( xLoc=loc_x2+1 ; xLoc<=loc_x2+3 ; xLoc++ )
		{
			for( int i=0 ; i<3 ; i++ )
			{
				yLoc = center_y + yOffset[i];

				if( can_create_unit(unitId, xLoc, yLoc) )
					return 1;
			}
		}

		//------- create a unit on the outer rows -------//

		for( int i=0 ; i<10 ; i++ )		// maximum 10 columns
		{
			if( i%2==0 )
				yLoc = center_y-2-i/2;
			else
				yLoc = center_y+2+i/2;

			for( int x=1 ; x<=3 ; x++ )
			{
				xLoc = loc_x2 + x;

				if( can_create_unit(unitId, xLoc, yLoc) )
					return 1;
			}
		}
	}

	//----look for an empty locatino for the unit to stand ----//
	//--- scan for the 5 rows right below the building ---//

	xLoc=loc_x1;
	yLoc=loc_y1;     // xLoc & yLoc are used for returning results

	UnitInfo*   unitInfo   = unit_res[unitId];
	SpriteInfo* spriteInfo = sprite_res[unitInfo->sprite_id];

	for (int r = 0; r <= 20; r++)
	{
		xLoc= MAX(loc_x1 -r, 0);
		yLoc= MAX(loc_y1 -r, 0);     // xLoc & yLoc are used for returning results
	
		if( locate_space(is_being_deleted, xLoc, yLoc, 
			MIN(loc_x2 +r, world.max_x_loc-1), MIN(loc_y2 +r, world.max_y_loc-1),
			 spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type) )
		{
			return 1;
		}

	}
	return 0;
}
//----------- End of function Place::create_unit_locate_space ---------//


//---------- Begin of function Place::can_create_unit --------//
//
// <int> unitId     - the id. of the unit to be created
// <int> xLoc, yLoc - location of the unit
//
// Return whether a unit can be created on the given location.
//
int Place::can_create_unit(int unitId, int xLoc, int yLoc)
{
	UnitInfo*   unitInfo   = unit_res[unitId];
	SpriteInfo* spriteInfo = sprite_res[unitInfo->sprite_id];

	for( int y=0 ; y<spriteInfo->loc_height ; y++ )
	{
		for( int x=0 ; x<spriteInfo->loc_width ; x++ )
		{
			int thisXLoc = xLoc+x;
			int thisYLoc = yLoc+y;

			if( thisXLoc < 0 || thisXLoc >= MAX_WORLD_X_LOC ||
				 thisYLoc < 0 || thisYLoc >= MAX_WORLD_Y_LOC )
			{
				return 0;
			}

			if( !world.get_loc(thisXLoc, thisYLoc)->can_move(unitInfo->mobile_type) )
				return 0;
		}
	}

	return 1;
}
//---------- End of function Place::can_create_unit --------//


//-------- Begin of function Place::assign_unit ---------//
//
// put a unit into building
//
void Place::assign_unit(int unitRecno)
{
	// ----- drop item on hand ------//

	if( !unit_array.is_deleted(unitRecno) )
	{
		unit_array[unitRecno]->drop_item(COMMAND_AUTO);
	}
	return;		// no action
}
//-------- End of function Place::assign_unit ---------//

//-------- Begin of function Place::being_attack_hit ---------//

void Place::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	int baseObjRecno = base_obj_recno;

	//--- if this is a Killer Bee which can only attack one, multiply its demages against building ---//

	if( attackerObj && attackerObj->cast_to_Unit() && attackerObj->cast_to_Unit()->unit_id == UNIT_BEE )
		damagePoint *= 5;

	//------ call parent class function -------//

	BaseObj::being_attack_hit(attackerObj, damagePoint);

	if( base_obj_array.is_deleted(baseObjRecno) )
		return;

	// ##### begin Gilbert 10/11 #####//
	return_fire( attackerObj );
	// ##### end Gilbert 10/11 #####//

	//---- call out defenders from neighboring camps to help ----//

	invoke_camp_defense(attackerObj);

	if( base_obj_array.is_deleted(baseObjRecno) )
		return;

	//---------- AI defend ----------//

	if( nation_recno && is_ai )
	{
		if( attackerObj && attackerObj->nation_recno == nation_recno )		// this can happen when the unit has just changed nation
			return;

		nation_array[nation_recno]->ai_defend(attackerObj);
	}
}
//-------- End of function Place::being_attack_hit ---------//


//-------- Begin of function Place::invoke_camp_defense ---------//
//
// Call out defenders from neighboring camps to help.
//
void Place::invoke_camp_defense(BaseObj* attackerObj)
{
	if( !attackerObj )
		return;

	//----- if this is an independent town -----//

	if( !nation_recno )
	{
		//---- if this is a slave town ----//

		if( cast_to_Town() && cast_to_Town()->is_pay_tribute_to_monster() )
		{
			//---- reckon the net attacking power in the battling area ----//

			int hasWar;			// ref var for returning the result

			int netAttackerPower = world.net_attacker_power_in_area(center_x, center_y,
									  attackerObj->nation_recno, nation_recno, 0, hasWar, EFFECTIVE_DEFEND_DISTANCE);

			//-- no need to call for reinforcement if the defense force is more powerful than the attacking force --//

			if( netAttackerPower <= 0 )
				return;

			//------ call enslaving lair's AI to protect it ------//

			Town* thisTown = cast_to_Town();

			for( int i=0 ; i<thisTown->linked_firm_count ; i++ )
			{
				//--- if this linked firm is a lair enslaving the town ---//

				if( thisTown->tribute_to_lair( thisTown->linked_firm_array[i], 1 ) )
				{
					FirmLair* firmLair = firm_array[ thisTown->linked_firm_array[i] ]->cast_to_FirmLair();

					if( firmLair && firmLair->is_ai )
						firmLair->invoke_defense(attackerObj, netAttackerPower);
				}
			}
		}

		return;
	}

	//---- reckon the net attacking power in the battling area ----//

	int hasWar;			// ref var for returning the result

	int netAttackerPower = world.net_attacker_power_in_area(center_x, center_y,
							  attackerObj->nation_recno, nation_recno, 0, hasWar, EFFECTIVE_DEFEND_DISTANCE);

	//-- no need to call for reinforcement if the defense force is more powerful than the attacking force --//

	if( netAttackerPower <= 0 )
		return;

	//--------- call for reinforcement now -------//

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		//--- look for our camp ----//

		// ###### begin Gilbert 15/4 #########//
		// if( !firmPtr->cast_to_FirmCamp() || firmPtr->nation_recno!=nation_recno )
		if( !firmPtr->cast_to_FirmCamp() || firmPtr->nation_recno!=nation_recno || nation_recno == 0 )
		// ###### end Gilbert 15/4 #########//
			continue;

		//--- only if the camp's within the effective defense distance ---//

		if( m.points_distance(firmPtr->center_x, firmPtr->center_y, center_x, center_y )
			 > EFFECTIVE_DEFEND_DISTANCE )
		{
			continue;
		}

		//----- ask the camp to send out defenders now ------//

		int defensePower = firmPtr->cast_to_FirmCamp()->invoke_defense(attackerObj, netAttackerPower);

		netAttackerPower -= defensePower;

		if( netAttackerPower <= 0 )
			break;
	}
}
//-------- End of function Place::invoke_camp_defense ---------//


// ------ Begin of function Place::is_stealth -------//
//
int Place::is_stealth()
{
	return config.blacken_map && !world.is_explored_quick( loc_x1, loc_y1, loc_x2, loc_y2 );
}
// ------ End of function Place::is_stealth -------//

