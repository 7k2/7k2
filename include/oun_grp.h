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

//Filename    : OUN_GRP.H
//Description : Header file of class Unit

#ifndef __OUN_GRP_H
#define __OUN_GRP_H

#ifndef __ODYNARRB_H
#include <odynarrb.h>
#endif

#ifndef __OUNIT_H
#include <ounit.h>
#endif

//------- Define constant -----------//
//
// Note: if there is a leader unit in the group, it is always the first unit in the array
//

#define UNIT_GROUP_LEADER_ID		1

#define MAX_DEFINED_UNIT_GROUP	9		// maximum elements in unit_group_array[]

//------ Define formation types -----//

#define MAX_FORMATION   7

enum { FORMATION_VANGUARD=1,
		 FORMATION_LINE,
		 FORMATION_CIRCLE,
		 FORMATION_ARC,
		 FORMATION_STAGGERED,
		 FORMATION_PRE_SQUARE,
		 FORMATION_SANDWICH,
		 FORMATION_SQUARE,
	  };

enum { LEADER_AT_TOP=1,
		 LEADER_AT_BOTTOM,
		 LEADER_AT_LEFT,
	 	 LEADER_AT_RIGHT,
		 LEADER_AT_TOP_LEFT,
		 LEADER_AT_TOP_RIGHT,
		 LEADER_AT_BOTTOM_LEFT,
		 LEADER_AT_BOTTOM_RIGHT,
		};
//------ Define class UnitGroup ----//

class UnitGroup : public DynArray
{
public:
	char 			mobile_type;		// for passing vars between functions
	int			formation_mode;

public:
	UnitGroup();

	void 		set(short* unitRecnoArray, int unitCount);
	void 		set_to_selected();
	void 		select_all();

	int 		form_team();		// set team_info and leader_unit_recno of the units

	// ----- write member in UnitGroup to memory for multiplayer -------//

	int		get_write_len();
	void		write_mem( short *msg );
	void		read_mem( short *msg );
	int		validate_unit_array( int nationRecno );
	void 		validate();

	// -------------------------------//
	void		draw_profile();
	void		add_unit(short unitRecno);
	void		del_unit(short recNo);
	Unit*		get_unit(short recNo);
	int		has_unit(int unitRecno);
	int 		is_visible(short recNo);
	int 		is_unit_deleted(short recNo);

	void		stop(char remoteAction);
	void 		move(int destXLoc, int destYLoc, bool forceMoveFlag, char remoteAction);
	void		exe_move(int destXLoc, int destYLoc, bool forceMoveFlag);
	void		attack(int targetRecno, char remoteAction);
	void		exe_attack(int targetRecno);
	void 		assign(int destXLoc, int destYLoc, int targetMobileType, char remoteAction, int actionNationRecno= -1, bool forceMoveFlag=false);
	void		exe_assign(int destXLoc, int destYLoc, int targetMobileType, int actionNationRecno, bool forceMoveFlag);
	void 		return_camp(char remoteAction);
	void		transform_mfort(int destXLoc, int destYLoc, char remoteAction);

	void		right_click(int xLoc, int yLoc, char remoteAction, bool waypt, bool shiftAtk, bool forceMove);
	void 		formation(int formationType, char remoteAction);
	void		formation_turn(int keyCode, char remoteAction);
	void 		square_formation(int direction = 0);

private:

	//------ functions for formation -----//

	void 		exe_formation(int formationType);
	void		exe_formation_turn(int direction);
	void 		vanguard_formation(int direction =0);
	void 		line_formation(int direction = 0);
	void 		circle_formation(int direction = 0);
	void 		arc_formation(int direction =0);
	void 		staggered_formation(int direction = 0);
	void		square_preformation(int direction = 0);
	void		sandwich_formation(int direction = 0);

	//--------- other functions ----------//
	void		cluster_units_for_path_reuse(int destX, int destY, int centerX, int centerY);
	void		cluster_units_for_attack(int targetRecno, int targetLocX, int targetLocY, int targetWidth, int targetHeight);

	void		group_by_mobile_type();
	void 		run_action(int destXLoc, int destYLoc, int orderId, int orderPara, int actionNationRecno=-1, bool forceMoveFlag=false );
};

extern UnitGroup 	unit_group, unit_group_land, unit_group_air, unit_group_array[9], unit_group_remote;

//-----------------------------------//

#endif