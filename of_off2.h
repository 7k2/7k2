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

// Filename    : OF_OFF2.H
// Description : Header file of FirmFort

#ifndef __OF_OFF2_H
#define __OF_OFF2_H

#include <OFIRM.H>

//------ Define class FirmLair ------//
#define OFFENSIVE2_BULLET_DAMAGE 100
#define OFFENSIVE2_BULLET_SPEED 16
#define OFFENSIVE2_BULLET_INIT_Z 32

class FirmOffensive2 : public Firm
{
public:
	int		animation_offset;
	short		base_firm_build_id;
	int		firm_attack_animation_delay;
	int		firm_attack_delay;
	short		target_obj_recno;
	short		return_target_obj_recno;
	char		attack_object;
	short		plant_id;
	short		camouflage_flag;
	short		propagate_count;

public:
	FirmOffensive2();

	void		init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno);
	void		deinit();

	//--------- firm drawing functions ----------//

	void		draw(int displayLayer=1);

	virtual int	obj_can_attack(int offensiveAttackOnly)	{ return 1; }
	virtual int obj_power() 	{ return	(int) hit_points*2; }

	// -------- function on auto attact -------//

	void		process_monster_firm();
	int		attack_target(BaseObj* targetObj);
	int		return_fire(BaseObj* targetObj);
	void		right_click(int xLoc, int yLoc);
	
	// ------- function returns to Bullet::init --------//

	int		get_dir(int curX, int curY, int destX, int destY);
	float		bullet_damage();
	short		bullet_radius();
	char		bullet_fire();
	short		bullet_id();
	short		bullet_speed();
	short		bullet_init_z();

	// -------- function on reproduction -------//
	int		find_best_place_loc(short buildFirmId, short refXLoc, short refYLoc, short& resultXLoc, short& resultYLoc, short distance);
	int		create_firm_next_to_place(Place* placePtr, int firmId, int firmRaceId, int nationRecno, short distance);

	// -------- function on interface ---------//
	void		put_info(int refreshFlag);
/*	void		detect_info();
	void		disp_main_menu(int refreshFlag);
	void		detect_main_menu();
	void		disp_attack_menu(int refreshFlag);
	void		detect_attack_menu();*/

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	// -------- type casting function --------//

	virtual  FirmOffensive2*		cast_to_FirmOffensive2()		{ return this; }

};

//-----------------------------------------------//

#endif