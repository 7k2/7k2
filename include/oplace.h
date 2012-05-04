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

//Filename   : OPLACE.H
//Description: Class Place - it is the base class of Firm and Town

#ifndef __OPLACE_H
#define __OPLACE_H

#include <obaseobj.h>
#include <gamedef.h>

//-------- Define class Place --------//

#pragma pack(1)
class Place : public BaseObj
{
public:
	int	setup_date;					// the setup date of this town
	char* setup_years_str(int=0); // return the no. of years this firm has been setup in string

	virtual int	majority_race()			{ return race_id; }

	float	 place_max_hit_points;
	char	 no_neighbor_space;			// no space to build firms/towns next to this town

	//--------- location vars ----------//

	unsigned char region_id;

	short loc_x1, loc_y1, loc_x2, loc_y2;
	short abs_x1, abs_y1, abs_x2, abs_y2;
	short	altitude;

	short loc_width()    { return loc_x2-loc_x1+1; }
	short loc_height()   { return loc_y2-loc_y1+1; }

	short center_x;
	short center_y;

	//------- location functions for BaseObj --------//

	int 	obj_loc_width() 	{ return loc_width(); }
	int 	obj_loc_height()  { return loc_height(); }

	int	obj_loc_x1()		{ return loc_x1; }
	int	obj_loc_y1()		{ return loc_y1; }
	int	obj_loc_x2()		{ return loc_x2; }
	int	obj_loc_y2()		{ return loc_y2; }

	int	obj_mobile_type()	{ return UNIT_LAND; }
	int	obj_region_id()	{ return region_id; }

	int 	max_hit_points()	{ return (int) place_max_hit_points; }

	virtual Place*  cast_to_Place() 		{ return this; }

	//------- animation vars ---------//

	char  cur_frame;          // current animation frame id.
	char  remain_frame_delay;

	//------- construction vars --------//

	char	under_construction;		// whether the firm is under construction

	short	builder_recno;				// the recno of the builder
	unsigned char builder_region_id;		// the original region no. of builder
	char	repair_flag;					// set 1 to auto repair, repair wall for town

	static char	is_being_deleted;		// whether the place is currently being deleted, if it is set to true, some functions will be executed in an exceptional way, it is set in deinit()

public:
	Place();

	virtual void  deinit();

	//---- functions for setting/mobilizing builders -----//

	int 	set_builder(short newBuilderRecno);
	int 	mobilize_builder(short recno);
	void  kill_builder(short builderRecno);

	//------ functions for locating space and creating units around the place ----//

	int	locate_space(int removeFirm, int &xLoc, int &yLoc, int xLoc2, int yLoc2, int width, int height, int mobileType=UNIT_LAND, int regionId=0);
	int	create_unit(int unitId);

	//----- derived function from BaseObj -----//

	virtual void  assign_unit(int unitRecno);
	virtual void  being_attack_hit(BaseObj* attackerObj, float damagePoint);

	// ----- return fire functions ------//

	virtual int		return_fire(BaseObj* targetObj) { return 0; }
	virtual float	bullet_damage() { return 0.0f; }
	virtual short	bullet_radius() { return 0; }
	virtual char	bullet_fire()   { return 0; }
	virtual short	bullet_id()     { return 0; }
	virtual short	bullet_speed()  { return 0; }
	virtual short	bullet_init_z() { return 0; }

	// ------ function on visiblity --------//

	int	is_stealth();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void 	invoke_camp_defense(BaseObj* attackerObj);

   //---- functions for creating units -----//

	int 	can_create_unit(int unitId, int xLoc, int yLoc);
	int 	create_unit_locate_space(int unitId, int& xLoc, int& yLoc);
};
#pragma pack()

//------------------------------------//

#endif
