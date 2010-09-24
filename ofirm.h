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

//Filename    : OFIRM.H
//Description : Header file of Object Firm

#ifndef __OFIRM_H
#define __OFIRM_H

#ifndef __GAMEDEF_H
#include <GAMEDEF.H>
#endif
 
#ifndef __OPLACE_H
#include <OPLACE.H>
#endif

#ifndef __OFIRMA_H
#include <OFIRMA.H>
#endif

#ifndef __OINFO_H
#include <OINFO.H>
#endif

#ifndef __OFIRMID_H
#include <OFIRMID.H>
#endif

#ifndef __OFIRMRES_H
#include <OFIRMRES.H>
#endif

#ifndef __OSKILL_H
#include <OSKILL.H>
#endif

//----------- Define constant ------------//

#define CAN_SELL_HIT_POINTS_PERCENT	 80		// only when the firm's hit points is higher than this can the firm be sold
#define MAX_FRAME_COUNTERS		16

//------- define parameters for ai_status, used for AI ------//

enum	{	FIRM_WITHOUT_ACTION=0,
			FACTORY_RELOCATE,
			MARKET_FOR_SELL,
			CAMP_IN_DEFENSE,
		};

//-------- define values for Firm::bribe_result -------//

enum { BRIBE_NONE,
		 BRIBE_SUCCEED,
		 BRIBE_FAIL,
	  };

//-------- define values for Firm::assassinate_result -------//

enum { ASSASSINATE_FAIL,
		 ASSASSINATE_SUCCEED_AT_LARGE,		// assassination succeed and the assassinator is at large
		 ASSASSINATE_SUCCEED_KILLED,			// assassination succeed and the assassinator is caught and executed
	  };

//------ Declare derived classes ---------//

class FirmWork;
class FirmBase;
class FirmCamp;
class FirmFort;
class FirmMine;
class FirmFactory;
class FirmMarket;
class FirmInn;
class FirmResearch;
class FirmWar;
class FirmTrain;
class FirmSpy;
class FirmSpecial;
class	FirmLair;
class FirmMonsterTrain;
class FirmMonsterAlchemy;
class FirmLishorr;
class FirmMonsterFortress;
class FirmAnimal;
class FirmIncubator;
class FirmMagic;
class FirmOffensive;
class FirmOffensive2;

//----------- Define class Firm ------------//

class Firm: public Place
{
public:
	//-------- basic vars --------------//

	short	 firm_recno;

	char   firm_id;            // Firm ID, meanings are defined in OFIRMID.H
	short  firm_build_id;

	short	 upgrading_firm_id;	// if this firm is being upgraded, this is the id. of the upgrade firm. 
	short	 upgrade_completion_percent;

	int	 can_sell() 		{ return hit_points >= (int) max_hit_points() * CAN_SELL_HIT_POINTS_PERCENT / 100; }

	//-------- firm name vars ---------//

	short	 closest_town_name_id;			// name id. of the town that is closest to this firm when it is built
	short	 firm_name_instance_id;
	virtual char*	 firm_name();

	//-------- firm animation vars ---------//

	char	firm_cur_frame[MAX_FRAME_COUNTERS];
	char  firm_remain_frame_delay[MAX_FRAME_COUNTERS];

	//------ inter-relationship -------//

	char 	 linked_firm_count;
	char 	 linked_town_count;

	short	 linked_firm_array[MAX_LINKED_FIRM_FIRM];
	short	 linked_town_array[MAX_LINKED_FIRM_TOWN];

	char 	 linked_firm_enable_array[MAX_LINKED_FIRM_FIRM];
	char 	 linked_town_enable_array[MAX_LINKED_FIRM_TOWN];

	short	 active_link_town_recno;

	//--------- financial vars ---------//

	float  	last_year_income;
	float  	cur_year_income;

	float  	income_365days()      { return last_year_income*(365-info.year_day)/365 +
														 cur_year_income; }
	//---------- misc vars ------------//

	char	 should_set_power;
	char	 reveal_info;				// set if it has been casted by own persian god

	// --------- rally point vars -------//

	short rally_enable_flag;
	short	rally_dest_base_obj_recno;		// if zero use x, y
	short	rally_dest_x;
	short	rally_dest_y;

	//----------- AI vars ------------//

	char	 ai_processed;			// some ai actions are processed once only in the processing day. To prevent multiple checking in the processing day
	char	 ai_status;
	char	 ai_link_checked;		// AI checks firms and towns location by links, disable checking by setting this parameter to 1
	char	 ai_sell_flag;			// this is true if the AI has queued the command to sell this firm

	char	 should_close_flag;
	char	 ai_should_build_factory_count;

	//------ AI functions for returning values -----//

	virtual int total_combat_level()		{ return 0; }

	//-------- casting function for BaseObj ---------//

	virtual int obj_power()			{ return total_combat_level(); }

	Firm*  	cast_to_Firm()	{ return this; }

public:
	Firm();
	virtual ~Firm();

	//-------- initialization functions ---------//

	virtual void init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode=NULL, short builderRecno=0);
	virtual void deinit();

	virtual void init_derived()   {;}
	virtual void deinit_derived() {;}

	void		init_name();
	int 		get_closest_town_name_id();

	void 		start_upgrade(int upgradeFirmId, char remoteAction);

	//------ functions for setting world matrix -------//

	void     set_world_matrix();
	void     restore_world_matrix();
	int		get_should_set_power();

	//------ functions for setting and releasing links -----//

	void		setup_link();
	void		release_link();
	void		release_firm_link(int);
	void		release_town_link(int);

	int 		can_toggle_town_link(int townRecno);
	int 		can_toggle_firm_link(int firmRecno);

	void 		toggle_firm_link(int linkId, int toggleFlag, char remoteAction, int setBoth=0);
	void 		toggle_town_link(int linkId, int toggleFlag, char remoteAction, int setBoth=0);

	char		get_linked_firm_status(int firmRecno);
	char		get_linked_town_status(int townRecno);

	int 		scan_best_recruitable_town(short *townRecnoArray, int raceId=0, int requiredLinkedFirmId=0);
	int 		can_train_from_town(int townRecno, int requiredLinkedFirmId, int raceId=0);

	virtual int	can_set_active_link(int townRecno) { return 0; }
	virtual void set_active_link(int townRecno, char remoteAction);	// 0 to clear; may not change

	//------- construction related functions -----//

	void		kill_builder(short builderRecno);
	virtual void sell_firm(char remoteAction);
	void		destruct_firm(char remoteAction);
	virtual void free_all_people()	{;}		 		// call when the firm is destroyed
	void		set_repair_flag(char repairFlag, char remoteAction);

	void		cancel_construction(char remoteAction);
	void		complete_construction();				// complete construction instantly

	//------ related to capture function -------//

	virtual int	can_assign_capture();

	//--------- firm drawing functions ----------//

	virtual void draw(int displayLayer=1);
	virtual void draw_full_size(int displayLayer=1);
	virtual void draw_frame(char * firmCurFrame, int displayLayer=1);
			  void draw_selected();
			  int  draw_detect_link_line(int actionDetect);
			  int  construction_frame();			// for under construction only

	virtual int  is_operating() { return 1; }

	//----------- interface functions -----------//

	virtual void right_click(int xLoc, int yLoc);

			  void disp_info_both(int refreshFlag);
			  void detect_info_both();

	virtual void put_info(int refreshFlag);//		{;}
	virtual void detect_info()						{;}

	virtual  int  should_show_info();

			  void disp_basic_info(int dispY1, int refreshFlag);
			  int  detect_basic_info();

			  void disp_hit_point(int dispY1);

	virtual void disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	virtual int  detect_edit_mode();

	//---------- process functions -----------//

			  void process_animation();
			  void process_construction();
			  int	 process_upgrade();
			  void process_repair();

			  void process_common_ai();
	virtual void process_ai()      {;}
	virtual void process_tell()    {;}
	virtual void process_monster_firm()    {;}

	virtual void process()         {;}
	virtual void next_day();
	virtual void next_month();
	virtual void next_year();

	//--------- financial functions -----------//

	void				add_income(int incomeType, float incomeAmt);
	virtual int		year_expense();
	virtual void 	pay_expense();

	//--------- misc functions -----------//

	void 				establish_contact_with_player();
	virtual void 	change_nation(int newNationRecno);
	int      		is_in_zoom_win();
	void 				reward(int workerId, int remoteAction);

	virtual int 	builder_stay_after_construction()	{ return 0;}		// called by process_construction() after the building is completely built. Derived class like FirmCamp will have a function body other than return 0.

	// -------- rally point ----------//

	virtual int		can_set_rally_point(int destBaseObjRecno=0) { return 0; }
	void				set_rally_point(int destBaseObjRecno, int x, int y, char remoteAction);
	void				clear_rally_point(char remoteAction);
	virtual int		send_to_rally_point( int unitRecno );

	//------------ file functions -----------//

	virtual int  write_derived_file(File*);
	virtual int  read_derived_file(File*);

	//-------- derived class casting functions -------//

	virtual FirmWork*		cast_to_FirmWork()		{ return 0; }
	virtual FirmBase*		cast_to_FirmBase()		{ return 0; }
	virtual FirmCamp*		cast_to_FirmCamp()		{ return 0; }
	virtual FirmFort*		cast_to_FirmFort()		{ return 0; }
	virtual FirmMine*		cast_to_FirmMine()		{ return 0; }
	virtual FirmFactory*	cast_to_FirmFactory()	{ return 0; }
	virtual FirmMarket*	cast_to_FirmMarket()		{ return 0; }
	virtual FirmInn*		cast_to_FirmInn()			{ return 0; }
	virtual FirmResearch* cast_to_FirmResearch() { return 0; }
	virtual FirmWar*		cast_to_FirmWar()			{ return 0; }
	virtual FirmTrain*	cast_to_FirmTrain()     { return 0; }
	virtual FirmSpy*	cast_to_FirmSpy()	{ return 0; }
	virtual FirmSpecial*	cast_to_FirmSpecial()		{ return 0; }
	virtual FirmLair*		cast_to_FirmLair()		{ return 0; }
	virtual FirmMonsterTrain* cast_to_FirmMonsterTrain() { return 0; }
	virtual FirmMonsterAlchemy* cast_to_FirmMonsterAlchemy() { return 0; }
	virtual FirmLishorr* cast_to_FirmLishorr() { return 0; }
	virtual FirmMonsterFortress* cast_to_FirmMonsterFortress() { return 0; }
	virtual FirmAnimal*  cast_to_FirmAnimal() { return 0; }
	virtual FirmIncubator* cast_to_FirmIncubator() { return 0; }
	virtual FirmMagic* cast_to_FirmMagic() { return 0; }
	virtual FirmOffensive* cast_to_FirmOffensive() { return 0; }
	virtual FirmOffensive2* cast_to_FirmOffensive2() { return 0; }

	//---------- AI functions ----------//

			  void	ai_del_firm();
			  int 	ai_recruit_worker();
	virtual int	   ai_has_excess_worker()	 { return 0; }		// whether the AI has excess workers on this firm or not
			  int		think_build_factory(int rawId);
	virtual int		ai_should_close();
			  int 	ai_build_neighbor_firm(int firmId);
	virtual void	ai_update_link_status()		{;}
			  int 	think_hire_inn_unit();
	virtual void 	think_linked_town_change_nation(int linkedTownRecno, int oldNationRecno, int newNationRecno);
			  void 	ai_firm_captured(int capturerNationRecno);

	//----- derived function from BaseObj -----//

	virtual void	left_click();

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//-------- functions derived from BaseObj ---------//

	virtual void 	being_attack_hit(BaseObj* attackerObj, float damagePoint);
	virtual void 	being_killed(BaseObj* attackerObj);

	//-------- functions derived from BaseObj ---------//

	virtual int		is_visible(int nationRecno) { return 1; }
};

//------------------------------------------//

#endif

