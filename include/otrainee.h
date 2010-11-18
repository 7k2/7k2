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

// Filename    : OTRAINEE.H
// Description : Header file of Trainee

#ifndef __OTRAINEE_H
#define __OTRAINEE_H

#include <oskill.h>

//------- define struct Trainee ---------//

struct Trainee
{
	char  train_type;
	bool	is_under_training;

	char	race_id;
	short unit_id;

	int	is_human()		{ return race_id>0; }
	int	is_monster()	{ return race_id<0; }
	int	monster_id()	{ return -race_id;  }

	char	loyalty;
	short	source_town_recno;
	short	spy_recno;		            // >0 if this unit is an enemy spy sneaked into the inn.

	//------ skill vars ------//

	Skill	skill;

	float spy_skill;     				// >0 if this unit is a spy with a spying skill for hire, he is added by FirmInn

	int	combat_level() 	{ return skill.actual_combat_level(); }
	int   skill_level()		{ return skill.actual_skill_level(); }
	int   max_hit_points()	{ return skill.actual_max_hit_points(); }

	//----- potential vars ------//

	char	train_skill_potential;
	char	train_combat_potential;
	char	train_spy_potential;

public:
	void	init(int raceId, int loyalty, int spyRecno, int trainType, int srcTownRecno);

	void	basic_train(int firmRecno);
	void	advanced_train(int firmRecno);

	int	is_nation(int firmRecno, int nationRecno);
	int	is_own(int firmRecno);
	int	is_own_spy();

	int	true_nation_recno(int firmRecno);            // the true nation recno of the unit, taking care of the situation where the unit is a spy

	//-------- action functions -----------//

	void  change_loyalty(int loyaltyChange);
	void	change_unit_id(int unitId, int nationRecno);
};

//-------------------------------------//

#endif