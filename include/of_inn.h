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

//Filename    : OF_INN.H
//Description : Header of FirmInn

#ifndef __OF_INN_H
#define __OF_INN_H

#ifndef __OUNIT_H
#include <ounit.h>
#endif

#ifndef __OSKILL_H
#include <oskill.h>
#endif

#ifndef __OFIRM_H
#include <ofirm.h>
#endif

//----------- Define constant --------------//

#define MAX_INN_UNIT 				12  		// maximum no. of units allowed in the hire waiting list
#define MAX_INN_UNIT_PER_REGION 	24			// maximum no. of inn units in any single region


//------- define struct InnUnit ---------//

#pragma pack(1)
struct InnUnit
{
public:
	short unit_id;
	short hero_id;
	int	unique_id;

	short	spy_skill;		// >0 if this unit is a spy with a spying skill for hire, he is added by FirmInn
	short spy_recno;		// >0 if this unit is an enemy spy sneaked into the inn.

	short hire_cost;
	short stay_count;		// how long this unit is going to stay until it leaves the inn if you do not hire him.

	Skill skill;
	int  	combat_level() 	{ return skill.actual_combat_level(&item); }
	int  	skill_level()		{ return skill.actual_skill_level(&item); }
	int  	max_hit_points()	{ return skill.actual_max_hit_points(&item); }

	float hit_points;

	Item  item;

public:
	void	set_hire_cost();

	int	is_own_spy();
	int	true_nation_recno();
};
#pragma pack()

//------- Define class FirmInn --------//

#pragma pack(1)
class FirmInn : public Firm
{
public:
	InnUnit		inn_unit_array[MAX_INN_UNIT];
	short			inn_unit_count;

	static short	selected_unit_id;

public:
	FirmInn();
	~FirmInn();

	void 		init_derived();
	void		deinit_derived();
	void		free_all_people();				// call when the firm destruct
	void 		del_inn_unit(int recNo);

	// ------- interface functions -------//

	void 		put_info(int);
	void 		detect_info();
	int  		should_show_info();
//	void		put_det(int);
	int		explore_for_player();

	// ------ process function -------//

	void		next_day();
	void		assign_unit(int unitRecno);
	int		kill_unit(int recNo);
	void		recover_hit_point();

	int		hire(short recNo, int spyEscape=0, short exitFirmRecno=0);
	int		buy_item( short recNo, int checkingFlag );

	// ------- type casting function -------//

	virtual	FirmInn* cast_to_FirmInn() { return this; };

	// ------ ai function --------//

	void		process_ai();

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//----- derived function from BaseObj -----//

	bool		can_accept_assign(int unitRecno, int actionNationRecno= -1);
	int		count_soldier();
	int		count_spy();

private:
	int 		should_add_inn_unit();

	int		transfer_inn_unit(int recNo);
	void 		add_inn_unit(int raceId);

	void 		update_add_hire_list();
	void 		update_del_hire_list();

	// ------ protected interface function -------//

	void 		disp_unit_list(int dispY1, int refreshFlag);
	void 		detect_unit_list();
	void		disp_unit_info(int dispY1, int refreshFlag);

	//-------- AI actions ---------//

	int		think_del();
	int 		think_hire_spy();
	int 		think_assign_spy_to(int raceId, int innUnitRecno);
	int 		think_hire_general();
	int 		think_assign_general_to(int raceId, int innUnitRecno);
};
#pragma pack()

//--------------------------------------//

#endif
