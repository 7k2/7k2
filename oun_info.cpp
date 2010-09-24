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

//Filename	 : OUN_INFO.CPP
//Description: Unit functions related to basic information

#include <OCONFIG.H>
#include <OTRANSL.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OGODRES.H>
#include <OF_CAMP.H>
#include <OHERORES.H>
#include <OSPY.H>
#include <ONATION.H>
#include <OPOWER.H>
#include <OUNIT.H>
#include <ITEMID.H>
#include <OGAME.H>
#include <OT_UNIT.H>


//--------- Begin of function Unit::region_id ---------//
//
// Return the region id. of this unit.
//
BYTE Unit::region_id()
{
   if( is_visible() )
   {
      return world.get_region_id( next_x_loc(), next_y_loc() );
	}
	else
	{
		if( unit_mode == UNIT_MODE_OVERSEE )
			return firm_array[unit_mode_para]->region_id;
	}

	return 0;
}
//----------- End of function Unit::region_id -----------//


//--------- Begin of function Unit::set_combat_level ---------//
//
// <int> combatLevel   new combat level or -1 for current combat level
//                     so as to update unit attribute of that combat level
void Unit::set_combat_level(int combatLevel)
{
	// ##### begin Gilbert 4/11 ######//
	if( combatLevel == -1 )		// update unit attribute of that combat level
		combatLevel = skill.actual_combat_level(NULL);		// get original combat
	// ##### end Gilbert 4/11 ######//

	err_when( combatLevel<=0 || combatLevel>1000 );

	int oldMaxHitPoints = max_hit_points();

	skill.set_combat_level(combatLevel);		// it will affect max_hit_points()

	hit_points = hit_points * max_hit_points() / oldMaxHitPoints;
	hit_points = min(hit_points, max_hit_points());

	// --------- update can_guard_flag -------//

	// ##### begin Gilbert 4/11 #######//
	if( combat_level() >= unit_res[unit_id]->guard_combat_level)
	// ##### end Gilbert 4/11 #######//
	{
		can_guard_flag = sprite_info->can_guard_flag;
#ifdef AMPLUS
		if( unit_id == UNIT_ZULU )
			can_guard_flag |= 4;			// shield during attack delay
#endif
	}
	else
	{
		can_guard_flag = 0;
	}

	if( combat_level() <= MAX_COMBAT_TRAIN )
		max_power = combat_level() + 50;
	else
		max_power = combat_level() / 10 + ( MAX_COMBAT_TRAIN + 50 - MAX_COMBAT_TRAIN / 10 );

	// ###### patch begin Gilbert 17/2 #####//
//	if( race_id == RACE_VIKING && nation_recno 
//		&& god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
	if( nation_recno && god_res[GOD_VIKING]->nation_prayer_count(nation_recno) > 0 )
	{
		// max_power += 20;
		max_power += 25;
	}
	// ###### patch end Gilbert 17/2 #####//

	cur_power = min(cur_power, max_power);
}
//-------- End of function Unit::set_combat_level -----------//


//--------- Begin of function Unit::set_rank ---------//
//
// Only if the unit has leadership skill, it can be a general or king.
//
void Unit::set_rank(int rankId)
{
	err_when( !race_id );

	if( rank_id == rankId )
		return;

   err_when( unit_mode != UNIT_MODE_REBEL && rankId >= RANK_GENERAL && is_civilian() );

   //------- promote --------//

   if( rankId > rank_id )
		change_loyalty(PROMOTE_LOYALTY_INCREASE);

	//------- demote -----------//

	else if( rankId < rank_id && rank_id != RANK_KING )      // no decrease in loyalty if a spy king hands his nation to his parent nation and become a general again
		change_loyalty(-DEMOTE_LOYALTY_DECREASE);

	//---- update nation_general_count_array[] ----//

	if( nation_recno )
	{
		UnitInfo* unitInfo = unit_res[unit_id];

		if( rank_id == RANK_GENERAL )    // if it was a general originally
			unitInfo->dec_nation_general_count(nation_recno);

		if( rankId == RANK_GENERAL )     // if the new rank is general
			unitInfo->inc_nation_general_count(nation_recno);

		//------ if demote a king to a unit ------//

		if( rank_id == RANK_KING && rankId != RANK_KING )
			unitInfo->inc_nation_unit_count(nation_recno);     // since kings are not included in nation_unit_count, when it is no longer a king, we need to re-increase it.

		//------ if promote a unit to a king ------//

		if( rank_id != RANK_KING && rankId == RANK_KING )
			unitInfo->dec_nation_unit_count(nation_recno);     // since kings are not included in nation_unit_count, we need to decrease it
	}

	//----- reset leader_unit_recno if demote a general to soldier ----//

	if( rank_id == RANK_GENERAL && rankId == RANK_SOLDIER )
	{
		//----- reset leader_unit_recno of the units he commands ----//

		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			Unit* unitPtr = (Unit*) unit_array.get_ptr(i);		// don't use is_deleted() as it filters out units that are currently dying 

			if( unitPtr && unitPtr->leader_unit_recno == sprite_recno )
			{
				unitPtr->leader_unit_recno = 0;
//				unitPtr->team_id = 0;
			}
		}

		//--------- deinit team_info ---------//

		err_when( !team_info );

		mem_del(team_info);
		team_info = NULL;
//		team_id   = 0;
	}

	//----- if this is a soldier being promoted to a general -----//

	else if( rank_id == RANK_SOLDIER && rankId == RANK_GENERAL )
	{
		//-- if this soldier is formerly commanded by a general, detech it ---//

		if( leader_unit_recno )
		{
			if( !unit_array.is_deleted(leader_unit_recno) )    // the leader unit may have been killed at the same time
			{
				err_when( !unit_array[leader_unit_recno]->team_info );
				unit_array[leader_unit_recno]->team_info->del_member(sprite_recno);
			}

			leader_unit_recno = 0;
		}
	}

	// ###### patch begin Gilbert 27/9 #######//
	// ------- clear royal in campaign mode -----//

	if( rankId == RANK_KING )
	{
		is_royal = 0;
	}
	// ###### patch end Gilbert 27/9 #######//

	//-------------- update AI info --------------//

	if( nation_recno )
	{
		if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
			nation_array[nation_recno]->del_general_info(sprite_recno);

		rank_id = rankId;

		if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
			nation_array[nation_recno]->add_general_info(sprite_recno);
	}
   else
   {
      rank_id = rankId;
   }

   //----- if this is a general/king ------//

   if( rank_id == RANK_GENERAL || rank_id == RANK_KING )
   {
      //--------- init team_info -------//

      if( !team_info )
      {
         team_info = (TeamInfo*) mem_add( sizeof(TeamInfo) );
			memset( team_info, 0, sizeof(TeamInfo) );
      }

      //--- set leadership if this unit does not have any now ----//

		if( skill_level() < 10 )
			skill.set_skill_level( + m.random(40) );
   }

   //------ refresh if the current unit is selected -----//

	if( unit_array.selected_recno == sprite_recno )
		info.disp();
}
//-------- End of function Unit::set_rank -----------//


//--------- Begin of function Unit::set_spy ---------//

void Unit::set_spy(int spyRecno)
{
	spy_recno = spyRecno;
}
//---------- End of function Unit::set_spy ---------//


//--------- Begin of function Unit::unit_name ---------//
//
// [int] withTitle - whether return a string with the title of the unit
//                   or not. (default: 1)
//
// [int] firstNameOnly - whether return the first word of the name only
//							(default: 0)
//
char* Unit::unit_name(int withTitle, int firstNameOnly)
{
//	static String str;

	UnitInfo* unitInfo = unit_res[unit_id];

	//------------------------------------//

	if( race_id && unit_id != UNIT_WAGON )
	{
		char *baseName;
		if( hero_id )
			baseName = hero_res[hero_id]->name;
		else if( rank_id == RANK_KING )		// use the player name
			baseName = nation_array[nation_recno]->king_name(firstNameOnly);
		else
			baseName = race_res[race_id]->get_name(name_id, firstNameOnly?1:0);

		if( withTitle )
		{
			if( unit_mode == UNIT_MODE_REBEL )
			{
				if( rank_id == RANK_GENERAL )
					return text_unit.str_rebel_unit_titled( race_id, baseName, rank_id );
			}
			else
			{
				if( rank_id != RANK_SOLDIER )	// human soldier has no "SOLIDER" has title
					return text_unit.str_unit_titled( race_id, baseName, rank_id );
			}
		}

		return baseName;
	}
	else
	{
		int kingAnimal = 0;
		if (unitInfo->unit_class == UNIT_CLASS_ANIMAL && attack_count > 1)
		{
			AttackInfo *attackInfo = attack_info_array + 1;
			if (combat_level() >= attackInfo->combat_level)
				kingAnimal = 1;
		}

		return text_unit.str_non_race_unit_name( unitInfo->name,
			(unitInfo->class_info.has_weapon_version ? get_weapon_version() : 0),
			(unitInfo->unit_class != UNIT_CLASS_GOD ? name_id : 0),		// don't put serial no. to god
			kingAnimal );
   }

//   return str;
}
//----------- End of function Unit::unit_name ---------//


//--------- Begin of function Unit::set_name ---------//
//
// Set the name id. of this unit.
//
void Unit::set_name(WORD newNameId)
{
	//------- free up the existing name id. ------//

	if( name_id )
	{
		if( is_human() )
			race_res[race_id]->free_name_id(name_id);
		else
			monster_res.free_name_id(name_id);
	}

	//------- set the new name id. ---------//

	name_id = newNameId;

	//-------- register usage of the new name id. ------//

	if( name_id )
	{
		if( is_human() )
			race_res[race_id]->use_name_id(name_id);
		else
			monster_res.use_name_id(name_id);
	}
}
//----------- End of function Unit::set_name ---------//


//--------- Begin of function Unit::is_own_spy ---------//
//
int Unit::is_own_spy()
{
	return spy_recno && nation_array.player_recno
		&& spy_array[spy_recno]->true_nation_recno == nation_array.player_recno;
}
//----------- End of function Unit::is_own_spy ---------//


// ###### begin Gilbert 4/11 #######//
//--------- Begin of function Unit::is_own ---------//
//
int Unit::is_own()
{
	return UnitB::is_own() || is_own_spy();
}
//----------- End of function Unit::is_own ---------//
// ###### end Gilbert 4/11 #######//


//--------- Begin of function Unit::is_nation ---------//
//
// Whether the unit belongs to the specific nation.
//
int Unit::is_nation(int nationRecno)
{
	if( nation_recno == nationRecno )
		return 1;

	if( spy_recno && spy_array[spy_recno]->true_nation_recno == nationRecno )
		return 1;

	return 0;
}
//----------- End of function Unit::is_nation ---------//


//--------- Begin of function Unit::is_civilian ---------//
//
int Unit::is_civilian()
{
	return unit_res[unit_id]->is_civilian;
}
//----------- End of function Unit::is_civilian ---------//


//----------- Begin of function Unit::is_camouflage ----------//
//
// pass viewerNation -1 to test if camouflage is on/off
//
int Unit::is_camouflage(int viewerNation)
{
	Spy *spyPtr;

	// #### begin Gilbert 31/3 #####//
	if( cur_action == SPRITE_ATTACK )		// don't be invisible while attacking
		return 0;
	// #### end Gilbert 31/3 #####//

	if( spy_recno && (spyPtr = spy_array[spy_recno]) && spyPtr->true_nation_recno != viewerNation && spyPtr->camouflage_count > 0)
		return 1;
	else if( item.ability(ITEM_ABILITY_INVISIBLE) && !is_nation(viewerNation) )
		return 1;		// do not call item.use_now(), not appropriate. Let next day to decrease item.para
	// ##### patch begin Gilbert 14/7 #####//
//	else if( unit_id == UNIT_JAPANESE_SPU && !is_nation(viewerNation) && rank_id != RANK_KING )
//		return 1;
	// ##### patch end Gilbert 14/7 #####//
	else
		return 0;
}
//----------- End of function Unit::is_visible ----------//


//----------- Begin of function Unit::is_stealth ----------//
int Unit::is_stealth()
{
	// ####### begin Gilbert 9/2 #######//
//	return (config.fog_of_war && world.get_loc(next_x_loc(), next_y_loc())->visibility() < unit_res[unit_id]->stealth)
//		|| spy_recno && is_camouflage( nation_array.player_recno );		// check spy_recno to reduce calling to is_camouflage()

	if( !is_visible() )
		return 1;

	// ######### begin Gilbert 31/3 #####//
	if( is_camouflage(nation_array.player_recno) )
	// ######### end Gilbert 31/3 #####//
		return 1;

	if( config.blacken_map )
	{
		if( config.fog_of_war )
		{
			if( loc_width == 1 )
			{
				return world.get_loc(next_x_loc(), next_y_loc())->visibility() < unit_res[unit_id]->stealth;
			}
			else
			{
				int x1 = next_x_loc();
				int y1 = next_y_loc();
				int x2 = x1 + loc_width - 1;
				int y2 = y1 + loc_height - 1;
				int stealthLevel = unit_res[unit_id]->stealth;
				for( int y = y1; y <= y2; ++y )
				{
					Location *locPtr = world.get_loc( x1, y );
					// ######## begin Gilbert 19/2 ##########//
					// for( int x = x1; y <= x2; ++x, ++locPtr )
					for( int x = x1; x <= x2; ++x, ++locPtr )
					// ######## end Gilbert 19/2 ##########//
					{
						if( locPtr->visibility() >= stealthLevel )
							return 0;
					}
				}
				return 1;
			}
		}
		else if( !config.explore_whole_map )
		{
			if( loc_width == 1 )
			{
				err_when( loc_height != 1 );
				return !world.get_loc(next_x_loc(), next_y_loc())->explored();
			}
			else
			{
				return !world.is_explored( next_x_loc(), next_y_loc(),
					next_x_loc() + loc_width - 1, next_y_loc() + loc_height - 1 );
			}
		}
	}

	return 0;
	// ####### end Gilbert 9/2 #######//
}
//----------- End of function Unit::is_stealth ----------//


//--------- Begin of function Unit::true_nation_recno ---------//
//
// The true nation recno of the unit, taking care of the
// situation where the unit is a spy.
//
int Unit::true_nation_recno()
{
   if( spy_recno )
      return spy_array[spy_recno]->true_nation_recno;
   else
      return nation_recno;
}
//----------- End of function Unit::true_nation_recno ---------//


//--------- Begin of function Unit::commander_power ---------//
//
// A commander's power is determined:
//
// -Population of the towns he controls
// -The employment rate of the towns he controls, the higher the
//  employment rate, the higher his power is
// -If there are any other commanders controls the towns at the same time.
// -the no. of soldiers led by the commander and their combat levels.
//
int Unit::commander_power()
{
	//---- if the commander is in a military camp -----//

	int commanderPower=0;

	if( unit_mode == UNIT_MODE_OVERSEE )
	{
		Firm* firmPtr = firm_array[unit_mode_para];

		if( firmPtr->cast_to_FirmCamp() )
		{
			Town* townPtr;

			for( int i=firmPtr->linked_town_count-1 ; i>=0 ; i-- )
			{
				if( firmPtr->linked_town_enable_array[i] == LINK_EE )
				{
					townPtr = town_array[firmPtr->linked_town_array[i]];

					int linkedCampCount = townPtr->linked_camp_count(true);

					if( linkedCampCount > 0 )
						commanderPower += townPtr->population / linkedCampCount;
				}
			}

			commanderPower += firmPtr->cast_to_FirmCamp()->soldier_count*3;		// 0 to 24
		}
		else if( firmPtr->firm_id == FIRM_BASE )
		{
			commanderPower = 60;
		}
	}
	else
	{
		commanderPower = team_info->member_count*3;		// 0 to 24
	}

	return commanderPower;
}
//----------- End of function Unit::commander_power -----------//



//--------- Begin of function Unit::commanded_soldier_count ---------//
//
// Return the no. of soldiers commanded by this unit.
//
int Unit::commanded_soldier_count()
{
	if( rank_id != RANK_GENERAL && rank_id != RANK_KING )
		return 0;

	//--------------------------------------//

	err_when( !team_info );

   int soldierCount=0;

   if( is_visible() )
   {
		soldierCount = team_info->member_count-1;

      if( soldierCount < 0 )  // member_count can be 0
         soldierCount = 0;
   }
   else
   {
      if( unit_mode == UNIT_MODE_OVERSEE )
      {
         FirmCamp* firmCamp = firm_array[unit_mode_para]->cast_to_FirmCamp();

         if( firmCamp )			// it can be an overseer of a seat of powre
				soldierCount = firmCamp->soldier_count;
      }
   }

   return soldierCount;
}
//----------- End of function Unit::commanded_soldier_count -----------//


//----------- Begin of function Unit::obj_power -----------//
//
// Return a power index of the weapon, this is for calculating
// the total combat level of a target.
//
int Unit::obj_power()
{
	UnitInfo* unitInfo = unit_res[unit_id];

	// ##### begin Gilbert 24/3 #######//
	// if( unitInfo->unit_class == UNIT_CLASS_WEAPON )
	if( !unitInfo->class_info.has_combat_level )
	{
		// return (int) hit_points + (unitInfo->weapon_power + get_weapon_version() - 1) * 15;
		int power = (int)hit_points + unitInfo->weapon_power * 15;
		if( unitInfo->class_info.has_weapon_version )
			power += (get_weapon_version() - 1) * 15;
		return power;
	}
	// ##### end Gilbert 24/3 #######//
	else
	{
		return (int) hit_points;
	}
}
//----------- End of function Unit::obj_power -----------//


//----------- Begin of function Unit::get_cur_loc -----------//
//
// <short&> xLoc, yLoc - reference vars for returning the
//								 location of this unit
//
// return : 0 - if this unit is invisible
//				1 - if a location has been returned.
//
int Unit::get_cur_loc(short& xLoc, short& yLoc)
{
	if( is_visible() )
	{
		xLoc = next_x_loc();		// update location
		yLoc = next_y_loc();
	}
	else if( unit_mode == UNIT_MODE_OVERSEE ||
				unit_mode==UNIT_MODE_CONSTRUCT_FIRM ||
				unit_mode == UNIT_MODE_IN_HARBOR )
	{
		Firm* firmPtr = firm_array[unit_mode_para];

		xLoc = firmPtr->center_x;
		yLoc = firmPtr->center_y;
	}
	else if( unit_mode==UNIT_MODE_CONSTRUCT_TOWN )
	{
		Town* townPtr = town_array[unit_mode_para];

		xLoc = townPtr->center_x;
		yLoc = townPtr->center_y;
	}
	else if( unit_mode == UNIT_MODE_ON_SHIP )
	{
		Unit* unitPtr = unit_array[unit_mode_para];

		if(unitPtr->is_visible())
		{
			xLoc = unitPtr->next_x_loc();
			yLoc = unitPtr->next_y_loc();
		}
		else
		{
			err_when(unitPtr->unit_mode!=UNIT_MODE_IN_HARBOR);
			Firm *firmPtr = firm_array[unitPtr->unit_mode_para];
			xLoc = firmPtr->center_x;
			yLoc = firmPtr->center_y;
		}
	}
	else
		return 0;

	return 1;
}
//----------- End of function Unit::get_cur_loc -----------//


//----- Begin of static function Unit::can_resign -----//
int Unit::can_resign()
{
	// ###### begin Gilbert 16/5 ########//
	return rank_id != RANK_KING
		&& !game.is_unit_in_condition(unique_id);
	// ###### end Gilbert 16/5 ########//
}
//------ End of static function Unit::can_resign ----//


//--------- Begin of function Unit::can_succeed_king ---------//
//
int Unit::can_succeed_king()
{
	if( !nation_recno || !race_id )
		return 0;

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->king_unit_recno )	// the king is still alive
		return 0;

	if( nationPtr->is_human() )
	{
		//-- only the nation is a human nation, only human military unit can succeed the king --//

		return is_human() && !is_civilian();
	}
	else
	{
		return monster_res[ nationPtr->monster_id() ]->unit_id == unit_id;
	}
}
//----------- End of function Unit::can_succeed_king -----------//


//---------- begin of function Unit::sprite_speed ------//
//
int Unit::sprite_speed()
{
	// return Sprite::sprite_speed() + item.ability( ITEM_ABILITY_SPEED );
	return sprite_info->speed + item.ability( ITEM_ABILITY_SPEED );
}
//---------- end of function Unit::sprite_speed ------//
