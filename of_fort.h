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

// Filename    : OF_FORT.H
// Description : Header file of FirmFort
 
#ifndef __OF_FORT_H
#define __OF_FORT_H

#include <OF_CAMP.H>

//------ define constant -------//

#define MAX_FORT_ARCHER 5
#define MAX_TRAIN_ARCHER_PROGRESS 60

#define TRAIN_ARCHER_COST 120
#define COST_PER_ARCHER 24
#define MIN_ARHCER_DELAY 5
#define MAX_ARCHER_SIMULTANEOUS 2

#define FORT_BULLET_DAMAGE 14
#define FORT_BULLET_SPEED 18
#define FORT_BULLET_INIT_Z 32
#define FORT_BULLET_RANGE 8

typedef unsigned long DWORD;

//-------- define class FirmFort --------//

class FirmFort : public FirmCamp
{
public:

	// ------ archers variables --------//

	char		current_archer_count;
	char		target_archer_count;
	short		train_archer_progress;
	DWORD		last_archer_fire_frame[MAX_FORT_ARCHER];

	//---------- AI vars ----------//

	char		ai_recruiting_soldier;

public:
	FirmFort();

	void		init_derived();

	// -------- virtual function affecting linked town ------ //

	int		resistance_adjustment();
	int		loyalty_adjustment();

	// -------- derived function --------//

	virtual int				obj_can_attack(int offensiveAttackOnly)			{ return offensiveAttackOnly ? 0 : current_archer_count>0 ; }
	virtual FirmFort*		cast_to_FirmFort()		{ return this; }

	// -------- overloaded interface functions ----------//

	void		disp_camp_info(int dispY1, int refreshFlag);
	void		detect_camp_info();
	void		disp_train_menu(int refreshFlag);
	void		detect_train_menu();

	// ------- function of processing ------//

	void		next_day();
	void		pay_expense();

	//----- derived function from BaseObj -----//

	virtual void 	being_attack_hit(BaseObj* attackerObj, float damagePoint);

	//---------- recuit soldier functions ----------//

	void		recruit_soldier(int townRecno, bool specialUnit, char remoteAction, char noCost=0 );
	int		can_set_active_link(int townRecno);
	void		set_active_link(int townRecno, char remoteAction);
	int 		can_recruit(int townRecno);
	void		cancel_train_soldier(int soldierId, char remoteAction);


	// -------- function on archer -------//

	void		set_target_archer(int newTargetArcher, char remoteAction);
	void		process_train_archer();
	int		return_fire(BaseObj* targetObj);
	void		kill_archer();

	// ------- function returns to Bullet::init --------//

	float		bullet_damage();
	short		bullet_radius();
	char		bullet_fire();
	short		bullet_id();
	short		bullet_speed();
	short		bullet_init_z();

	//----------- AI functions ----------//

	void		process_ai();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

private:
	//-------------- AI functions --------------//

	virtual int  ai_recruit(int recruitCount);

	int 		think_train_archer();
};

//-------------------------------------------//

#endif