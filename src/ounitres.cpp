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

//Filename    : OUNITRES.CPP
//Description : Object Sprite Frame Resource

#include <ounitres.h>
#include <all.h>
#include <ostr.h>
#include <osys.h>
#include <ogameset.h>
#include <onation.h>
#include <osprtres.h>
#include <oworldmt.h>
#include <otransl.h>
#include <oresx.h>
#include <oraceres.h>
#include <omonsres.h>
#include <oherores.h>
#include <ot_unit.h>

#ifdef NO_DEBUG_UNIT
#undef err_when
#undef err_here
#undef err_if
#undef err_else
#undef err_now
#define err_when(cond)
#define err_here()
#define err_if(cond)
#define err_else
#define err_now(msg)
#undef DEBUG
#endif

//-------- define file name -----------//

#define UNIT_CLASS_DB     "UNITCLAS"
#define UNIT_DB    		  "UNIT"
#define UNIT_ATTACK_DB    "UNITATTK"

//-------- Begin of function UnitRes::init ---------//

void UnitRes::init()
{
	deinit();

	//----- open unit bitmap resource file -------//

	String str;

	str  = DIR_RES;
	str += "I_UNITLI.RES";

	res_large_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITGI.RES";

	res_general_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITKI.RES";

	res_king_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITSI.RES";

	res_small_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITTI.RES";

	res_general_small_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITUI.RES";

	res_king_small_icon.init_imported(str, 1);  // 1-don't read all into buffer

	str  = DIR_RES;
	str += "I_UNITVI.RES";

	res_unit_icon.init_imported(str, 1);  // 1-don't read all into buffer

	//------- load database information --------//

	load_class_info();
	load_info();
	load_attack_info();

	//---------- set vars -----------//

	mobile_monster_count = 0;

	init_flag=1;

	//-------- debug ---------//

#ifdef DEBUG

	UnitInfo* unitInfo = unit_info_array;

	for( int i=0 ; i<unit_info_count ; i++, unitInfo++ )
	{
		for( int j=0 ; j<unitInfo->attack_count ; j++ )
		{
			err_when( attack_info_array[ unitInfo->first_attack+j-1 ].eqv_attack_next > unitInfo->attack_count );
		}
	}

#endif
}
//--------- End of function UnitRes::init ----------//


//-------- Begin of function UnitRes::deinit ---------//

void UnitRes::deinit()
{
	if( init_flag )
	{
		mem_del(unit_info_array);
		mem_del(attack_info_array);
		mem_del(unit_class_array);

		res_large_icon.deinit();
		res_general_icon.deinit();
		res_king_icon.deinit();
		res_small_icon.deinit();
		res_general_small_icon.deinit();
		res_king_small_icon.deinit();

		init_flag=0;
	}
}
//--------- End of function UnitRes::deinit ----------//


//-------- Begin of function UnitRes::second_init ---------//

void UnitRes::second_init()
{
	UnitInfo* unitInfo = unit_info_array;

	for( int i=0 ; i<unit_info_count ; i++, unitInfo++ )
	{
		memset( unitInfo->nation_tech_level_array, 0, sizeof(unitInfo->nation_tech_level_array) );
		memset( unitInfo->nation_unit_count_array, 0, sizeof(unitInfo->nation_unit_count_array) );
		memset( unitInfo->nation_general_count_array, 0, sizeof(unitInfo->nation_general_count_array) );
	}
}
//--------- End of function UnitRes::second_init ----------//


//------- Begin of function UnitRes::load_info ---------//

void UnitRes::load_info()
{
	Database  *dbUnit = game_set.open_db(UNIT_DB);
	UnitRec 	 *unitRec;
	UnitInfo  *unitInfo;
	long		 bitmapOffset;
	int		 i;

	unit_info_count = dbUnit->rec_count();
	unit_info_array = (UnitInfo*) mem_add(sizeof(UnitInfo) * unit_info_count);

	memset( unit_info_array, 0, sizeof(UnitInfo)*unit_info_count );

	//--------- read in frame information ---------//

	for( i=0 ; i<dbUnit->rec_count() ; i++ )
	{
		unitRec  = (UnitRec*) dbUnit->read(i+1);
		unitInfo = unit_info_array+i;

		m.rtrim_fld( unitInfo->name, unitRec->name, unitRec->NAME_LEN );
		translate.multi_to_win(unitInfo->name, unitInfo->NAME_LEN);

		unitInfo->unit_id 	   = i+1;
		unitInfo->sprite_id	   = m.atoi(unitRec->sprite_id, unitRec->SPRITE_ID_LEN);
		unitInfo->dll_sprite_id	= m.atoi(unitRec->dll_sprite_id, unitRec->SPRITE_ID_LEN);
		unitInfo->race_id   	   = m.atoi(unitRec->race_id  , unitRec->RACE_ID_LEN);

		unitInfo->unit_class 	= unitRec->unit_class[0];
		unitInfo->mobile_type   = unitRec->mobile_type;

		unitInfo->visual_range  = m.atoi(unitRec->visual_range, unitRec->UNIT_PARA_LEN);
		unitInfo->visual_extend = m.atoi(unitRec->visual_extend , unitRec->UNIT_PARA_LEN);
		unitInfo->stealth       = m.atoi(unitRec->stealth       , unitRec->UNIT_PARA_LEN);
		unitInfo->hit_points    = m.atoi(unitRec->hit_points  , unitRec->UNIT_PARA_LEN);
		unitInfo->armor         = m.atoi(unitRec->armor       , unitRec->UNIT_PARA_LEN);

		unitInfo->build_days    = m.atoi(unitRec->build_days, unitRec->BUILD_DAYS_LEN);
		unitInfo->build_cost    = m.atoi(unitRec->build_cost, unitRec->COST_LEN);
		unitInfo->build_live_points_cost = m.atoi(unitRec->build_live_points_cost, unitRec->COST_LEN);
		unitInfo->year_cost     = m.atoi(unitRec->year_cost, unitRec->COST_LEN);

		if( unitInfo->race_id < 0 )     // BUGHERE
		{
			unitInfo->build_cost += unitInfo->build_cost;
			unitInfo->build_live_points_cost += unitInfo->build_live_points_cost;
		}

		if( unitInfo->unit_class == UNIT_CLASS_WEAPON )
			unitInfo->weapon_power = unitRec->weapon_power-'0';

		unitInfo->carry_unit_capacity  = m.atoi(unitRec->carry_unit_capacity, unitRec->CARRY_CAPACITY_LEN);
		unitInfo->carry_goods_capacity = m.atoi(unitRec->carry_goods_capacity, unitRec->CARRY_CAPACITY_LEN);

		unitInfo->transform_unit_id 	   = m.atoi(unitRec->transform_unit_id	    , unitRec->SPRITE_ID_LEN);
		unitInfo->transform_combat_level = m.atoi(unitRec->transform_combat_level, unitRec->UNIT_PARA_LEN);
		unitInfo->guard_combat_level     = m.atoi(unitRec->guard_combat_level, unitRec->UNIT_PARA_LEN);

		// ---- set class_info -----//

		UnitClassInfo *unitClassInfo = get_class_info(unitInfo->unit_class);
		if( unitClassInfo )
		{
			unitInfo->class_info = *unitClassInfo;
		}
		else
		{
			err_here();
			memset( &unitInfo->class_info, 0, sizeof(unitInfo->class_info) );
		}

		// ----- set bitmap pointers -------//

		memcpy( &bitmapOffset, unitRec->large_icon_ptr, sizeof(long) );
		unitInfo->soldier_icon_ptr = res_large_icon.read_imported(bitmapOffset);

		if( unitRec->general_icon_file_name[0] != '\0' && unitRec->general_icon_file_name[0] != ' ')
		{
			memcpy( &bitmapOffset, unitRec->general_icon_ptr, sizeof(long) );
			unitInfo->general_icon_ptr = res_general_icon.read_imported(bitmapOffset);
		}
		else
		{
			unitInfo->general_icon_ptr = unitInfo->soldier_icon_ptr;
		}

		if( unitRec->king_icon_file_name[0] != '\0' && unitRec->king_icon_file_name[0] != ' ')
		{
			memcpy( &bitmapOffset, unitRec->king_icon_ptr, sizeof(long) );
			unitInfo->king_icon_ptr = res_king_icon.read_imported(bitmapOffset);
		}
		else
		{
			unitInfo->king_icon_ptr = unitInfo->soldier_icon_ptr;
		}

		memcpy( &bitmapOffset, unitRec->small_icon_ptr, sizeof(long) );
		unitInfo->soldier_small_icon_ptr = res_small_icon.read_imported(bitmapOffset);

		if( unitRec->general_small_icon_file_name[0] != '\0' && unitRec->general_small_icon_file_name[0] != ' ')
		{
			memcpy( &bitmapOffset, unitRec->general_small_icon_ptr, sizeof(long) );
			unitInfo->general_small_icon_ptr = res_general_small_icon.read_imported(bitmapOffset);
		}
		else
		{
			unitInfo->general_small_icon_ptr = unitInfo->soldier_small_icon_ptr;
		}

		if( unitRec->king_small_icon_file_name[0] != '\0' && unitRec->king_small_icon_file_name[0] != ' ')
		{
			memcpy( &bitmapOffset, unitRec->king_small_icon_ptr, sizeof(long) );
			unitInfo->king_small_icon_ptr = res_king_small_icon.read_imported(bitmapOffset);
		}
		else
		{
			unitInfo->king_small_icon_ptr = unitInfo->soldier_small_icon_ptr;
		}
		if( unitRec->unit_icon_file_name[0] != '\0' && unitRec->unit_icon_file_name[0] != ' ')
		{
			memcpy( &bitmapOffset, unitRec->unit_icon_ptr, sizeof(long) );
			unitInfo->unit_icon_ptr = res_unit_icon.read_imported(bitmapOffset);
		}
		unitInfo->unit_icon_offsetx = m.atoi(unitRec->unit_icon_offsetx, unitRec->OFFSET_LEN);
		unitInfo->unit_icon_offsety = m.atoi(unitRec->unit_icon_offsety, unitRec->OFFSET_LEN);

		unitInfo->unit_icon_offsetx += -LOCATE_WIDTH/2 - (-ZOOM_LOC_X_WIDTH/2 + -ZOOM_LOC_Y_WIDTH/2);
		unitInfo->unit_icon_offsety += -LOCATE_HEIGHT/2 - (-ZOOM_LOC_X_HEIGHT/2 + -ZOOM_LOC_Y_HEIGHT/2);

		unitInfo->first_attack = m.atoi(unitRec->first_attack, unitRec->UNIT_PARA_LEN);
		unitInfo->attack_count = m.atoi(unitRec->attack_count, unitRec->UNIT_PARA_LEN);
		unitInfo->die_effect_id = m.atoi(unitRec->die_effect_id, unitRec->UNIT_PARA_LEN);

		unitInfo->defense_attribute.init(m.atoi(unitRec->sturdiness, unitRec->UNIT_PARA_LEN),
			m.atoi(unitRec->is_wood, unitRec->IS_WOOD_LEN));
		
		memset( unitInfo->nation_tech_level_array, 1, sizeof(unitInfo->nation_tech_level_array) );

		//------ set is_civilian for wagons ------//

		if( unitInfo->unit_class == UNIT_CLASS_WAGON )
			unitInfo->is_civilian = true;
	}

	//--------- set vehicle info  ---------//

	for( i=0 ; i<unit_info_count ; i++ )
	{
		unitInfo = unit_info_array+i;

		if( unitInfo->vehicle_unit_id )
		{
			unit_info_array[ unitInfo->vehicle_unit_id-1 ].vehicle_id = unitInfo->vehicle_id;
			unit_info_array[ unitInfo->vehicle_unit_id-1 ].solider_id = i+1;
		}
	}
}
//-------- End of function UnitRes::load_info ---------//


//------- Begin of function UnitRes::load_attack_info ---------//

void UnitRes::load_attack_info()
{
	Database  		 *dbUnitAttack = game_set.open_db(UNIT_ATTACK_DB);
	UnitAttackRec 	 *attackRec;
	AttackInfo  	 *attackInfo;
	int		 		 i;

	attack_info_count = dbUnitAttack->rec_count();
	attack_info_array = (AttackInfo*) mem_add(sizeof(AttackInfo) * attack_info_count);

	memset( attack_info_array, 0, sizeof(AttackInfo)*attack_info_count );

	//--------- read in frame information ---------//

	for( i=0 ; i<dbUnitAttack->rec_count() ; i++ )
	{
		attackRec  = (UnitAttackRec*) dbUnitAttack->read(i+1);
		attackInfo = attack_info_array+i;

		attackInfo->combat_level     = m.atoi(attackRec->combat_level 	  , attackRec->COMBAT_LEVEL_LEN);
		attackInfo->attack_delay     = m.atoi(attackRec->attack_delay 	  , attackRec->UNIT_PARA_LEN);
		attackInfo->attack_range     = m.atoi(attackRec->attack_range 	  , attackRec->UNIT_PARA_LEN);
		attackInfo->attack_damage    = m.atoi(attackRec->attack_damage	  , attackRec->UNIT_PARA_LEN);
		attackInfo->pierce_damage 	  = m.atoi(attackRec->pierce_damage	  , attackRec->UNIT_PARA_LEN);
		attackInfo->effect_out_frame = m.atoi(attackRec->effect_out_frame, attackRec->UNIT_PARA_LEN);
		attackInfo->bullet_out_frame = m.atoi(attackRec->bullet_out_frame, attackRec->UNIT_PARA_LEN);
		if (attackInfo->effect_out_frame == 0)
			attackInfo->bullet_out_frame = attackInfo->bullet_out_frame;
		attackInfo->bullet_delay 	  = m.atoi(attackRec->bullet_delay	  , attackRec->UNIT_PARA_LEN);
		attackInfo->bullet_speed 	  = m.atoi(attackRec->bullet_speed	  , attackRec->UNIT_PARA_LEN);
		attackInfo->bullet_radius    = m.atoi(attackRec->bullet_radius       , attackRec->UNIT_PARA_LEN);
		attackInfo->bullet_sprite_id = m.atoi(attackRec->bullet_sprite_id, attackRec->UNIT_PARA_LEN);
		if ((attackInfo->attack_range > 1) && (attackInfo->bullet_sprite_id == 0))
			err_here();

		attackInfo->dll_bullet_sprite_id = m.atoi(attackRec->dll_bullet_sprite_id, attackRec->UNIT_PARA_LEN);
		attackInfo->eqv_attack_next  = m.atoi(attackRec->eqv_attack_next, attackRec->UNIT_PARA_LEN);

		attackInfo->min_power        = m.atoi(attackRec->min_power, attackRec->UNIT_PARA_LEN);
		attackInfo->consume_power    = m.atoi(attackRec->consume_power, attackRec->UNIT_PARA_LEN);
		attackInfo->fire_radius      = m.atoi(attackRec->fire_radius, attackRec->UNIT_PARA_LEN);
		attackInfo->effect_id        = m.atoi(attackRec->effect_id, attackRec->UNIT_PARA_LEN);
		attackInfo->effect2_id       = m.atoi(attackRec->effect2_id, attackRec->UNIT_PARA_LEN);
		attackInfo->effect_dis       = m.atoi(attackRec->effect_dis, attackRec->EFFECT_LOC_LEN);

		attackInfo->attack_attribute.init(m.atoi(attackRec->sturdiness, attackRec->UNIT_PARA_LEN),
													 m.atoi(attackRec->exposiveness, attackRec->UNIT_PARA_LEN),
													 m.atoi(attackRec->heat, attackRec->UNIT_PARA_LEN),
													 m.atoi(attackRec->wood_favour, attackRec->WOOD_FAVOUR_LEN));
		
		attackInfo->die_after_attack = (attackRec->die_after_attack == 'Y') ? 1:0;
	}
}
//-------- End of function UnitRes::load_attack_info ---------//


//-------- Begin of function UnitRes::mobile_type_to_mask --------//

char UnitRes::mobile_type_to_mask(int mobileType)
{
	switch( mobileType)
	{
		case UNIT_LAND:
			return 1;

		case UNIT_SEA:
			return 2;

		case UNIT_AIR:
			return 3;

		default:
			err_here();
			return 0;
	}
}
//-------- End of function UnitRes::mobile_type_to_mask --------//


//-------- Begin of function UnitInfo::is_loaded -------//

int UnitInfo::is_loaded()
{
	return sprite_res[sprite_id]->is_loaded(); 
}
//--------- End of function UnitInfo::is_loaded --------//


//---- Begin of function UnitInfo::inc_nation_unit_count ----//

void UnitInfo::inc_nation_unit_count(int nationRecno)
{
	err_when( nationRecno<1 || nationRecno>nation_array.size() );

	nation_unit_count_array[nationRecno-1]++;

	//------- increase the nation's unit count ---------//

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->total_unit_count++;

	if( unit_class==UNIT_CLASS_WEAPON )
	{
		nationPtr->total_weapon_count++;
	}
	else if( unit_class==UNIT_CLASS_SHIP )
	{
		nationPtr->total_ship_count++;
	}
	else if( unit_class==UNIT_CLASS_WAGON )
	{
		// UnitWagon will update nationPtr->total_human_count itself
	}
	else if( is_human() )
	{
		nationPtr->total_human_count++;
	}
	else if( is_monster() )
	{
		nationPtr->total_monster_count++;
	}
}
//----- End of function UnitInfo::inc_nation_unit_count -----//


//---- Begin of function UnitInfo::dec_nation_unit_count ----//

void UnitInfo::dec_nation_unit_count(int nationRecno)
{
	err_when( nationRecno<0 || nationRecno>MAX_NATION );

	if( nationRecno )
	{
		nation_unit_count_array[nationRecno-1]--;

		err_when( nation_unit_count_array[nationRecno-1] < 0 );

		//------ decrease the nation's unit count -------//

		Nation* nationPtr = nation_array[nationRecno];

		nationPtr->total_unit_count--;

		err_when( nationPtr->total_unit_count < 0 );

		if( unit_class==UNIT_CLASS_WEAPON )
		{
			nationPtr->total_weapon_count--;

			err_when( nationPtr->total_weapon_count < 0 );
		}
		else if( unit_class==UNIT_CLASS_SHIP )
		{
			nationPtr->total_ship_count--;

			err_when( nationPtr->total_ship_count < 0 );
		}
		else if( unit_class==UNIT_CLASS_WAGON )
		{
			// UnitWagon will update nationPtr->total_human_count itself
		}
		else if( is_human() )
		{
			nationPtr->total_human_count--;

			err_when( nationPtr->total_human_count < 0 );

			if( nationPtr->total_human_count < 0 )
				nationPtr->total_human_count = 0;
		}
		else if( is_monster() )
		{
			nationPtr->total_monster_count--;

			err_when( nationPtr->total_monster_count < 0 );

			if( nationPtr->total_monster_count < 0 )
				nationPtr->total_monster_count = 0;
		}
	}
}
//----- End of function UnitInfo::dec_nation_unit_count -----//


//---- Begin of function UnitInfo::inc_nation_general_count ----//

void UnitInfo::inc_nation_general_count(int nationRecno)
{
	err_when( nationRecno<1 || nationRecno>nation_array.size() );

	nation_general_count_array[nationRecno-1]++;

	nation_array[nationRecno]->total_general_count++;
}
//----- End of function UnitInfo::inc_nation_general_count -----//


//---- Begin of function UnitInfo::dec_nation_general_count ----//

void UnitInfo::dec_nation_general_count(int nationRecno)
{
	err_when( nationRecno<0 || nationRecno>MAX_NATION );

	if( nationRecno )
	{
		nation_general_count_array[nationRecno-1]--;

		nation_array[nationRecno]->total_general_count--;
	}

	err_when( nation_general_count_array[nationRecno-1] < 0 );
}
//----- End of function UnitInfo::dec_nation_general_count -----//

//---- Begin of function UnitInfo::nation_all_unit_count ----//
//
// Return the number of generals and units the given nation has.
//
int UnitInfo::nation_all_unit_count(int nationRecno)
{
	err_when( nationRecno<0 || nationRecno>MAX_NATION );

	return nation_general_count_array[nationRecno-1] +
			 nation_unit_count_array[nationRecno-1];
}
//----- End of function UnitInfo::nation_all_unit_count -----//

//-------- Begin of function UnitInfo::unit_change_nation -------//
//
// Call this function when a unit changes its nation. This
// function updates the unit count vars in the UnitInfo.
//
// <int> newNationRecno - the new nation recno
// <int> oldNationRecno - the original nation recno
// <int> rankId			- the rank of the unit.
//
void UnitInfo::unit_change_nation(int newNationRecno, int oldNationRecno, int rankId)
{
	//---- update nation_unit_count_array[] ----//

	if( oldNationRecno )
	{
		if( rankId != RANK_KING )
			dec_nation_unit_count(oldNationRecno);

		if( rankId == RANK_GENERAL )
			dec_nation_general_count(oldNationRecno);
	}

	if( newNationRecno )
	{
		if( rankId != RANK_KING )
			inc_nation_unit_count(newNationRecno);

		if( rankId == RANK_GENERAL )		// if the new rank is general
			inc_nation_general_count(newNationRecno);
	}
}
//--------- End of function UnitInfo::unit_change_nation --------//

#ifdef DEBUG

//-------- Begin of function UnitRes::get_attack_info -------//

AttackInfo* UnitRes::get_attack_info(int attackId)
{
	if( attackId<1 || attackId>attack_info_count )
		err.run( "UnitRes::get_attack_info[]" );

	return attack_info_array+attackId-1;
}

//--------- End of function UnitRes::get_attack_info --------//


//-------- Begin of function UnitRes::operator[] -------//

UnitInfo* UnitRes::operator[](int unitId)
{
	if( unitId<1 || unitId>unit_info_count )
		err.run( "UnitRes::operator[]" );

	return unit_info_array+unitId-1;
}

//--------- End of function UnitRes::operator[] --------//

#endif

//-------- Begin of function UnitInfo::get_large_icon_ptr -------//

char *UnitInfo::get_large_icon_ptr(char rankId)
{
	switch( rankId )
	{
	case RANK_KING:
		return king_icon_ptr;
	case RANK_GENERAL:
		return general_icon_ptr;
	case RANK_SOLDIER:
	default:
		return soldier_icon_ptr;
	}
}
//-------- End of function UnitInfo::get_large_icon_ptr -------//


//-------- Begin of function UnitInfo::get_small_icon_ptr --------//

char *UnitInfo::get_small_icon_ptr(char rankId)
{
	switch( rankId )
	{
	case RANK_KING:
		return king_small_icon_ptr;
	case RANK_GENERAL:
		return general_small_icon_ptr;
	case RANK_SOLDIER:
	default:
		return soldier_small_icon_ptr;
	}
}
// -------- End of function UnitInfo::get_small_icon_ptr --------//
//
// if rankId is RANK_KING pass nation_recno to nameId
//
// int flags : bit 0 = short name (human) or without serial number
//             bit 1 = title for general or king, rebel leader, ordo, all high
//             bit 2 = title for civilian, soldier, and which defined for bit 1 (detailed title)
char *UnitRes::unit_name(int flags, int unitId, int nameId, int heroId, int rankId, char unitMode )
{
	static String str;
	str = "";

	err_when( !race_res.init_flag );
	err_when( !monster_res.init_flag );

	int firstNameOnly = flags & 1;
	UnitInfo* unitInfo = unit_res[unitId];
	int raceId = unitInfo->race_id;		
	// wagon is a special case, raceId is not zero
	// but non-zero in Unit class

	//------------------------------------//

	if( raceId && unitInfo->unit_class != UNIT_CLASS_WAGON )
	{
		char *baseName;
		if( heroId )
			baseName = hero_res[heroId]->name;
		else if( rankId == RANK_KING )		// use the player name
			baseName = nation_array.get_custom_king_name(nameId, firstNameOnly);
		else if( raceId > 0 )			// human
			baseName = race_res[raceId]->get_name(nameId, firstNameOnly?1:0);
		else if( raceId < 0 )
			baseName = monster_res.get_name(nameId);
		else
		{
			err_here();
		}

		// unit title

		if( flags & 3 )
		{
			if( unitMode == UNIT_MODE_REBEL )
			{
				if( rankId == RANK_KING || rankId == RANK_GENERAL || flags & 2 )	// flags & 2 : detail title
					return text_unit.str_rebel_unit_titled( raceId, baseName, rankId );
			}
			else if( raceId )		// human or fryhtan
			{
				if( rankId == RANK_KING || rankId == RANK_GENERAL )
					return text_unit.str_unit_titled( raceId, baseName, rankId );
				else if( flags & 2 )			// detailed title
				{
					if( heroId )
						return text_unit.str_hero_titled( raceId, baseName );
					else if( unitInfo->is_civilian )
						return text_unit.str_civilian_titled(baseName);
					else
						return text_unit.str_unit_titled( raceId, baseName, rankId );
				}
			}
		}

		return baseName;
	}
	else
	{
		str = unitInfo->name;

		//--- for weapons, the rank_id is used to store the version of the weapon ---//

//		if( unitInfo->unit_class == UNIT_CLASS_WEAPON && get_weapon_version() > 1 )
//		{
//			str += " ";
//			str += m.roman_number(get_weapon_version());
//		}

		if( !firstNameOnly && unitInfo->unit_class != UNIT_CLASS_GOD )		
		{
			str += " ";
			str += nameId;     // name id is the series no. of the unit
		}

		return text_unit.str_non_race_unit_name( unitInfo->name, 
			0,			// version not available
			(!firstNameOnly && unitInfo->unit_class != UNIT_CLASS_GOD ? nameId : 0),	// God doesn't have any series no.
			0 );
   }
}


// --------- begin of function UnitRes::load_class_info ----------//
//
void UnitRes::load_class_info()
{
	Database  *dbUnitClass = game_set.open_db(UNIT_CLASS_DB);
	UnitClassRec 	 *unitClassRec;
	UnitClassInfo  *unitClassInfo;

	unit_class_count = dbUnitClass->rec_count();
	unit_class_array = (UnitClassInfo*) mem_add(sizeof(UnitClassInfo) * unit_class_count);

	memset( unit_class_array, 0, sizeof(UnitClassInfo)*unit_class_count );

	//--------- read in frame information ---------//

	for( int i=0 ; i<unit_class_count ; i++ )
	{
		unitClassRec  = (UnitClassRec*) dbUnitClass->read(i+1);
		unitClassInfo = unit_class_array+i;

		unitClassInfo->unit_class = unitClassRec->unit_class_name[0];
		unitClassInfo->has_combat_level = unitClassRec->has_combat_level != ' ' && unitClassRec->has_combat_level != '0';
		unitClassInfo->has_skill_level = unitClassRec->has_skill_level != ' ' && unitClassRec->has_skill_level != '0';
		unitClassInfo->has_spy_skill = unitClassRec->has_spy_skill != ' ' && unitClassRec->has_spy_skill != '0';
		unitClassInfo->has_loyalty = unitClassRec->has_loyalty != ' ' && unitClassRec->has_loyalty != '0';
		unitClassInfo->has_weapon_version = unitClassRec->has_weapon_version != ' ' && unitClassRec->has_weapon_version != '0';
		unitClassInfo->monster_side = unitClassRec->monster_side != ' ' && unitClassRec->monster_side != '0';
		unitClassInfo->life = (char)m.atoi(unitClassRec->life, unitClassRec->LIFE_LEN );
	}
}
// --------- end of function UnitRes::load_class_info ----------//


// --------- begin of function UnitRes::get_class_info ----------//
//
UnitClassInfo* UnitRes::get_class_info(int unitClassId)
{
	for(int i = 0; i < unit_class_count; ++i )
	{
		if( unit_class_array[i].unit_class == unitClassId )
			return &unit_class_array[i];
	}

	err_here();		// not found
	return NULL;
}
// --------- end of function UnitRes::get_class_info ----------//
