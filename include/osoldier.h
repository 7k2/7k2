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

//Filename    : OSOLDIER.H
//Description : Header of Soldier

#ifndef __OSOLDIER_H
#define __OSOLDIER_H

#ifndef __OSKILL_H
#include <oskill.h>
#endif

//------------- Define struct Soldier ------------//

struct Soldier
{
public:
	Soldier();

	char  race_id;
	char  rank_id;
	short unit_id;
	short	name_id;

	int	is_human()		{ return race_id>0; }
	int	is_monster()	{ return race_id<0; }
	int	monster_id()	{ return -race_id;  }

	short spy_recno;

	short	hero_id;                                // >0 if this is a hero
	long	unique_id;              // unique number generated across campaign

	char	loyalty;
	char 	is_royal;		// whether the unit is a royal unit
	short hit_points;

	short extra_para;			// weapon version for weapons and power attack points for human units
	short	remain_build_days;
	short	source_town_recno;		// town which comes from
	int   get_weapon_version()    { return extra_para; }
	int 	soldier_power(); 			// Return a power index of this soldier.

	//------- Skill vars --------//

	Skill	skill;

	int  	combat_level() 	{ return skill.actual_combat_level(&item); }
	int  	skill_level()		{ return skill.actual_skill_level(&item); }
	int  	max_hit_points()	{ return skill.actual_max_hit_points(&item); }

	//------ item vars ---------//

	Item 	item;

public:

	void	init_name();
	void	free_name();

	//------ functions for returning values ------//

	int	target_loyalty(int firmRecno);
	int	is_nation(int firmRecno, int nationRecno);

	char *small_icon_ptr();
	int	max_attack_range();
	int	is_under_training();
	int	can_succeed_king(int nationRecno);

	int	is_own(int firmRecno);
	int	is_own_spy();
	int	true_nation_recno(int firmRecno);            // the true nation recno of the unit, taking care of the situation where the unit is a spy

	//-------- action functions -----------//

	void  change_loyalty(int loyaltyChange);
	void	change_hit_points(int changePoints);
	void	change_unit_id(int unitId, int nationRecno);

	void	complete_training() 	{ remain_build_days=0; }
};

//--------------------------------------//

#endif
