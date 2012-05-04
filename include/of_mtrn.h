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

// Filename    : OF_MTRN.H
// Description : Header file of FirmMonsterTrain

#ifndef __OF_MTRN_H
#define __OF_MTRN_H

#ifndef __OSKILL_H
#include <oskill.h>
#endif

#ifndef __OFIRM_H
#include <ofirm.h> 
#endif

//------- Define constant ---------//

enum { MAX_MONSTER_TRAINEE = 8 };
enum { MAX_MONSTER_TRAIN_TYPE = 5 };

enum
{
	FIRM_MONS_TRAIN_MENU_MAIN,
};


//------- Define struct MonsterTrainee ----/

#pragma pack(1)
struct MonsterTrainee
{
public:
	char	unit_id;
	char	loyalty;
	Skill	skill;

public:
	int	is_under_training()		{ return skill.actual_combat_level() < BASIC_COMBAT_TRAIN; }

	int	combat_level() 	{ return skill.actual_combat_level(); }
	int   skill_level()		{ return skill.actual_skill_level(); }
	int   max_hit_points()	{ return skill.actual_max_hit_points(); }
};
#pragma pack()

//------- Define class FirmMonsterTrain -------//

#pragma pack(1)
class FirmMonsterTrain : public Firm
{
public:
	char				train_type_count;
	char				train_unit_id[MAX_MONSTER_TRAIN_TYPE];

	MonsterTrainee trainee_array[MAX_MONSTER_TRAINEE];
	short				trainee_count;
	short				selected_trainee_id;

	static char		firm_menu_mode;

public:
	FirmMonsterTrain(int trainUnitId);
	FirmMonsterTrain(char *trainUnitIdArray);

	void 		next_day();

	//---- recruit and training functions -----//

	void 		recruit_trainee(int trainUnitId, char remoteAction);
	int 		can_recruit(int trainUnitId);
	virtual	int represent_attribute(int traineeId);
	int		mobilize_trainee(int traineeId, char remoteAction );
	void		kill_trainee(int traineeId );
	void		cancel_train_trainee( int trainUnitId, char remoteAction );

	//------- AI functions -------//

	void		process_ai();

	//------- type casting function --------//

	virtual  FirmMonsterTrain*	cast_to_FirmMonsterTrain()	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();
	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	//---------------------------------------//

	virtual void change_nation(int newNationRecno);
	void		free_all_people();

	// -------- rally point ----------//

	int		can_set_rally_point(int destBaseObjRecno=0);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	int 		create_trainee_unit(MonsterTrainee& thisTrainee);
	void 		basic_train();

	//------- interface functions ------//

	void		disp_firm_info(int dispY1, int refreshFlag );
	void		detect_firm_info();
	void		disp_trainee_list(int dispY1, int refreshFlag );
	int		detect_trainee_list();
	void		disp_trainee_info(int dispY1, int refreshFlag );
	void		detect_trainee_info();

	//-------- AI actions ---------//

	void		think_train_new();
	int		think_del();
	int 		think_transfer_unit();
};
#pragma pack()

//---------------------------------------//

#endif
