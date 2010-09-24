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

//Filename    : OUNITRES.H
//Description : Header file of Object Unit resource

#ifndef __OUNITRES_H
#define __OUNITRES_H

#include <UNITTYPE.H>
#include <GAMEDEF.H>
#include <ORESDB.H>
#include <OATTACK.H>

typedef unsigned char BYTE;

#ifdef NO_DEBUG_UNIT
#undef DEBUG
#endif

//--------- Define Unit Classes --------//

enum { UNIT_CLASS_HUMAN = 'H',
		 UNIT_CLASS_CARAVAN = 'C',
		 UNIT_CLASS_WAGON = 'T',
		 UNIT_CLASS_WEAPON = 'W',
		 UNIT_CLASS_SHIP = 'S',
		 UNIT_CLASS_GOD = 'G',
		 UNIT_CLASS_MONSTER = 'M',
		 UNIT_CLASS_MONS_WEAPON = 'B',
		 UNIT_CLASS_INSECT = 'I',
		 UNIT_CLASS_ANIMAL = 'A',
	  };

//--------- Define constant ------------//

#define STD_UNIT_HIT_POINTS  200       // the hit points for all standard units

//--------- Define constant -----------//

enum { UNIT_SMALL_ICON_WIDTH=24,
		 UNIT_SMALL_ICON_HEIGHT=20,
		 UNIT_LARGE_ICON_WIDTH=46,
		 UNIT_LARGE_ICON_HEIGHT=38
	  };

// ------- Define struct UnitClassRec -------//

struct UnitClassRec
{
	enum { UNIT_CLASS_LEN=8, LIFE_LEN=2 };

	char unit_class_name[UNIT_CLASS_LEN];
	char has_combat_level;
	char has_skill_level;
	char has_spy_skill;
	char has_loyalty;
	char has_weapon_version;
	char monster_side;
	char life[2];
};

//-------- Define struct UnitRec ----------//

struct UnitRec
{
	enum { NAME_LEN=30, SPRITE_CODE_LEN=8, RACE_CODE_LEN=8, UNIT_CLASS_LEN=8, UNIT_PARA_LEN=3,
			 BUILD_DAYS_LEN=3, COST_LEN=3, CARRY_CAPACITY_LEN=3, FREE_WEAPON_COUNT_LEN=1,
			 FILE_NAME_LEN=8, BITMAP_PTR_LEN=4, SPRITE_ID_LEN=3, RACE_ID_LEN=3, OFFSET_LEN=4,
			 IS_WOOD_LEN=3 };

	char name[NAME_LEN];
	char sprite_code[SPRITE_CODE_LEN];
	char race_code[RACE_CODE_LEN];
	char unit_class[UNIT_CLASS_LEN];

	char mobile_type;

	char visual_range[UNIT_PARA_LEN];
	char visual_extend[UNIT_PARA_LEN];
	char stealth[UNIT_PARA_LEN];
	char hit_points[UNIT_PARA_LEN];
	char armor[UNIT_PARA_LEN];

	char build_days[BUILD_DAYS_LEN];
	char build_cost[COST_LEN];
	char build_live_points_cost[COST_LEN];
	char year_cost[COST_LEN];

	char weapon_power;		// an index from 1 to 9 indicating the powerfulness of the weapon

	char carry_unit_capacity[CARRY_CAPACITY_LEN];
	char carry_goods_capacity[CARRY_CAPACITY_LEN];

	char transform_unit[SPRITE_CODE_LEN];
	char transform_combat_level[UNIT_PARA_LEN];
	char guard_combat_level[UNIT_PARA_LEN];

	char large_icon_file_name[FILE_NAME_LEN];
	char large_icon_ptr[BITMAP_PTR_LEN];
	char general_icon_file_name[FILE_NAME_LEN];
	char general_icon_ptr[BITMAP_PTR_LEN];
	char king_icon_file_name[FILE_NAME_LEN];
	char king_icon_ptr[BITMAP_PTR_LEN];

	char small_icon_file_name[FILE_NAME_LEN];
	char small_icon_ptr[BITMAP_PTR_LEN];
	// ###### begin Gilbert 17/10 #######//
	char general_small_icon_file_name[FILE_NAME_LEN];
	char general_small_icon_ptr[BITMAP_PTR_LEN];
	char king_small_icon_file_name[FILE_NAME_LEN];
	char king_small_icon_ptr[BITMAP_PTR_LEN];
	// ###### end Gilbert 17/10 #######//

	// ###### begin Ban 2/9 #######//
	char unit_icon_file_name[FILE_NAME_LEN];
	char unit_icon_ptr[BITMAP_PTR_LEN];
	char unit_icon_offsetx[OFFSET_LEN];
	char unit_icon_offsety[OFFSET_LEN];
	// ###### end Ban 2/9 #######//

	char die_effect_sprite[SPRITE_CODE_LEN];

	char sprite_id[SPRITE_ID_LEN];
	char dll_sprite_id[SPRITE_ID_LEN];
	char race_id[RACE_ID_LEN];

	char vehicle_id[SPRITE_ID_LEN];
	char vehicle_unit_id[SPRITE_ID_LEN];

	char transform_unit_id[SPRITE_ID_LEN];
	char die_effect_id[UNIT_PARA_LEN];

	char first_attack[UNIT_PARA_LEN];
	char attack_count[UNIT_PARA_LEN];
	char sturdiness[UNIT_PARA_LEN];
	char is_wood[IS_WOOD_LEN]; 
};

//-------- Define struct UnitAttackRec ----------//

struct UnitAttackRec
{
	enum { SPRITE_CODE_LEN=8, UNIT_PARA_LEN=3, COMBAT_LEVEL_LEN=3, EFFECT_LOC_LEN =1,
			WOOD_FAVOUR_LEN = 3};

	char sprite_code[SPRITE_CODE_LEN];

	char attack_id[UNIT_PARA_LEN];
	char combat_level[COMBAT_LEVEL_LEN];

	char attack_delay[UNIT_PARA_LEN];
	char attack_range[UNIT_PARA_LEN];

	char attack_damage[UNIT_PARA_LEN];
	char pierce_damage[UNIT_PARA_LEN];
	char effect_out_frame[UNIT_PARA_LEN];
	char bullet_out_frame[UNIT_PARA_LEN];
	char bullet_delay[UNIT_PARA_LEN];
	char bullet_speed[UNIT_PARA_LEN];
	char bullet_radius[UNIT_PARA_LEN];
	char bullet_sprite_code[SPRITE_CODE_LEN];
	char bullet_sprite_id[UNIT_PARA_LEN];
	char dll_bullet_sprite_id[UNIT_PARA_LEN];
	char eqv_attack_next[UNIT_PARA_LEN];
	char min_power[UNIT_PARA_LEN];
	char consume_power[UNIT_PARA_LEN];
	char fire_radius[UNIT_PARA_LEN];
	char effect_code[SPRITE_CODE_LEN];
	char effect_id[UNIT_PARA_LEN];
	char effect2_code[SPRITE_CODE_LEN];
	char effect2_id[UNIT_PARA_LEN];
	char effect_dis[EFFECT_LOC_LEN];
	char sturdiness[UNIT_PARA_LEN];
	char exposiveness[UNIT_PARA_LEN];
	char heat[UNIT_PARA_LEN];
	char die_after_attack;
	char wood_favour[WOOD_FAVOUR_LEN];
};

// ------- Define struct UnitClassInfo -------//

struct UnitClassInfo
{
	char unit_class;
	char has_combat_level;
	char has_skill_level;
	char has_spy_skill;
	char has_loyalty;
	char has_weapon_version;
	char monster_side;
	char life;
};

//-------- Define struct UnitInfo ----------//

struct UnitInfo
{
	//-------- define constant ---------//

	enum { NAME_LEN=30 };

	char  name[NAME_LEN+1];

	short unit_id;
	short sprite_id;
	short dll_sprite_id;
	char  race_id;
	char  unit_class;
	char	is_civilian;

	int	is_human()		{ return race_id > 0; }
	int	is_monster()	{ return race_id < 0; }
	int	monster_id()	{ return -race_id; }

	char  mobile_type;

	BYTE  visual_range;
	BYTE	visual_extend;
	BYTE	stealth;
	BYTE  armor;

	short hit_points;

	// ###### begin Gilbert 19/1 ####//
	short	build_days;
	// ###### end Gilbert 19/1 ####//
	short build_cost;
	short	build_live_points_cost;
	short year_cost;

	char  weapon_power;		// an index from 1 to 9 indicating the powerfulness of the weapon

	char  carry_unit_capacity;
	short carry_goods_capacity;
	char  free_weapon_count;			// only for ships. It's the no. of free weapons can be loaded onto the ship

	char  vehicle_id;
	char  vehicle_unit_id;
	char  solider_id;

	char  transform_unit_id;
	char  transform_combat_level;
	char  guard_combat_level;

	short first_attack;
	char  attack_count;
	short	die_effect_id;
	DefenseAttribute defense_attribute;

	// char* large_icon_ptr;
	char* soldier_icon_ptr;
	char* general_icon_ptr;
	char* king_icon_ptr;

	// char* small_icon_ptr;
	char* soldier_small_icon_ptr;
	char* general_small_icon_ptr;
	char* king_small_icon_ptr;

	char* unit_icon_ptr;
	short unit_icon_offsetx;
	short unit_icon_offsety;

	//------- game vars -----------//

	char  nation_tech_level_array[MAX_NATION];			// each nation's tech level on this unit
	int   get_nation_tech_level(int nationRecno) 						{ return nation_tech_level_array[nationRecno-1]; }
	void  set_nation_tech_level(int nationRecno, char techLevel) 	{ nation_tech_level_array[nationRecno-1] = techLevel; }

	short nation_unit_count_array[MAX_NATION];			// mobile units + soldiers in camps, not including workers and prayers in bases
	short nation_general_count_array[MAX_NATION];

	// ------ unit class info ------//

	UnitClassInfo	class_info;		// own copy for modify(?) and faster access

public:
	int   is_loaded();	// whether the sprite data of this unit is in the memory or not
	char* get_large_icon_ptr(char rankId);
	char*	get_small_icon_ptr(char rankId);

	void	inc_nation_unit_count(int nationRecno);
	void	dec_nation_unit_count(int nationRecno);

	void	inc_nation_general_count(int nationRecno);
	void	dec_nation_general_count(int nationRecno);

	int	nation_all_unit_count(int nationRecno);

	void 	unit_change_nation(int newNationRecno, int oldNationRecno, int rankId);
};

//--------- Define struct AttackInfo ----------//

struct AttackInfo
{
	BYTE  combat_level;

	BYTE  attack_delay;
	BYTE  attack_range;

	BYTE  attack_damage;
   BYTE  pierce_damage;

	short effect_out_frame;    // on which attacking effect should be out
	short bullet_out_frame;    // on which attacking frames the bullet should be out
	char  bullet_delay;
	char  bullet_speed;
	char	bullet_radius;
	short	bullet_sprite_id;
	short	dll_bullet_sprite_id;
	char  eqv_attack_next;
	// cur_attack of the next equivalent attack
	// so as to cycle through several similar attacks
	short	min_power;
	short	consume_power;
	char	fire_radius;
	short	effect_id;
	short	effect2_id;
	char	effect_dis;
	AttackAttribute attack_attribute;
	char	die_after_attack;
};

//---------- Define class UnitRes ------------//

class UnitRes
{
public:
	char           init_flag;
	int				unit_class_count;
	int            unit_info_count;
	int            attack_info_count;

	UnitClassInfo* unit_class_array;
	UnitInfo*      unit_info_array;
	AttackInfo*    attack_info_array;

	ResourceDb     res_large_icon;
	ResourceDb     res_general_icon;
	ResourceDb     res_king_icon;
	ResourceDb     res_small_icon;
	// ###### begin Gilbert 17/10 #######//
	ResourceDb     res_general_small_icon;
	ResourceDb     res_king_small_icon;
	// ###### end Gilbert 17/10 #######//
	// ###### begin Ban 2/9 #######//
	ResourceDb     res_unit_icon;
	// ###### end Ban 2/9 #######//
	short				mobile_monster_count;

public:
	UnitRes()   { init_flag=0; }

	void  init();
	void  deinit();

	void	second_init();

	int 	write_file(File* filePtr);
	int	read_file(File* filePtr);

	#ifdef DEBUG
		UnitInfo*   operator[](int unitId);
		AttackInfo* get_attack_info(int attackId);
	#else
		UnitInfo*   operator[](int unitId)        { return unit_info_array+unitId-1; }
		AttackInfo* get_attack_info(int attackId) { return attack_info_array+attackId-1; }
	#endif

	static char	mobile_type_to_mask(int mobileType);

	char *unit_name(int flags, int unitId, int nameId, int heroId, int rankId, char unitMode );

private:
	void	load_class_info();
	void  load_info();
	void  load_attack_info();
	UnitClassInfo* get_class_info(int unitClassId);		// use UnitInfo::class_info instead of get_class_info
};

extern UnitRes unit_res;

//----------------------------------------------//

#endif

