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

// Filename    : OF_MFORT.H
// Description : Header file of MonsterFortress

#ifndef __OF_MFORT_H
#define __OF_MFORT_H

#include <OFIRM.H>


#define MAX_EXTRA_BUILDER 4
#define MAX_FORTRESS_SOLDIER	4
#define MAX_FORTRESS_TARGET 4
#define FORTRESS_SCAN_RANGE 8

class Unit;

class FirmMonsterFortress : public Firm
{
public:
	short			support_unit_id;
	short			extra_builder_recno[MAX_EXTRA_BUILDER];
	short			archer_unit_recno[MAX_FORTRESS_SOLDIER];
	char			extra_builder_count;
	char    		archer_count;       			// number of soldiers currently working in the firm

	char			fire_archer_id;
	char			selected_archer_id;

	short			target_count;
	short			target_base_obj_recno[MAX_FORTRESS_TARGET];

	short			rock_id;
	float			max_hit_point_ratio;

	static short	swap_item_id;

public:
	FirmMonsterFortress(int supportUnitId);

	void		init_derived();
	void		deinit();

	//----------- process functions --------//

	void		next_day();
	void 		pay_expense();
	virtual void change_nation(int newNationRecno);

	//--------- interface functions --------//

	void 		put_info(int refreshFlag);
	void 		detect_info();
	int  		should_show_info();

	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	//--------- firm drawing functions ----------//

	void		draw(int displayLayer=1);

	// ----------- extra builder function ----------//

	int		is_extra_builder_full();
	int 		builder_stay_after_construction();
	void		assign_extra_builder(int unitRecno);
	int		mobilize_extra_builder(int builderId);
	void		kill_extra_builder(int builderId);

	static void	transform_from_units(int unitRecno, int firmXLoc, int firmYLoc, short* builderArray, char remoteAction);
	static bool check_transform_from_units(int unitRecno, int& firmXLoc, int& firmYLoc, short* builderArray);
	static bool can_build_fortress( int unitRecno, int firmXLoc, int firmYLoc, short* builderArray );		// similar to world.can_build_firm

	//------------ archer functions functions -----------//

	int		is_soldier_full();

	void 		assign_unit(int unitRecno);
	void		assign_soldier(int unitRecno);
	int	 	mobilize_soldier(int soldierId, char remoteAction);
	void		patrol();

	void		kill_all_soldier();
	void		kill_soldier(int soldierId);
	int		swap_item( int fromSoldierId, int toSoldierId, int verifyItemId, char remoteAction );

	// ----------------------------------------//

	int		kill_unit(int unitRecno);	// call kill_extra_builder or kill_soldier
	void		free_all_people();				// call when the firm destruct

	//--------- class casting function --------//

	virtual	FirmMonsterFortress *cast_to_FirmMonsterFortress() { return this; };

	//----- derived function from BaseObj -----//

	virtual bool	can_accept_assign(int unitRecno, int actionNationRecno= -1);

	// -------- function on auto attact -------//

	void		process_monster_firm();
	int		return_fire(BaseObj* targetObj);
	void		process_scan_target();

	Unit *	get_attacker_unit();
	float		bullet_damage();
	short		bullet_radius();
	char		bullet_fire();
	short		bullet_id();
	short		bullet_speed();
	short		bullet_init_z();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	//-------- interface functions ---------//

	virtual void disp_camp_info(int dispY1, int refreshFlag);
	virtual void detect_camp_info();
			  void disp_soldier_list(int dispY1, int refreshFlag, int dispSpyMenu);
			  int  detect_soldier_list(int selectSpyMenu);
			  void disp_soldier_info(int dispY1, int refreshFlag);
			  void detect_soldier_info();

	//-------- map matrix drawing function --------//
	
	virtual  int is_visible(int nationRecno);
};

#endif