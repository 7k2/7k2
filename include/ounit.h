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

//Filename    : OUNIT.H
//Description : Header file of Object Unit

#ifndef __OUNIT_H
#define __OUNIT_H

#ifndef __OSKILL_H
#include <oskill.h>
#endif

#ifndef __OUNITRES_H
#include <ounitres.h>
#endif

#ifndef __OSPRITE_H
#include <osprite.h>
#endif

#ifndef __OITEM_H
#include <oitem.h>
#endif

//-------------- unit rank -------------//

enum { MAX_RANK=3 };

enum { RANK_SOLDIER,
		 RANK_GENERAL,
		 RANK_KING,
	  };

//------------- unit salary -----------//

enum { SOLDIER_YEAR_SALARY  = 10,
		 GENERAL_YEAR_SALARY  = 50,
		 SPY_YEAR_SALARY      = 100 };

//------- other constant ----------//

#define  EFFECTIVE_LEADING_DISTANCE 		10
#define  EFFECTIVE_DEFEND_DISTANCE			10
#define 	MAX_TEAM_MEMBER  						16 	// maximum no. of units a general/king can lead
#define  MAX_NATION_CONTRIBUTION  		10000 	// there is an upper limit nation_contribution as it is a <short>

//-------- unit movement constant parameters -------//

#define	MAX_UNIT_BLOCKED_TIME		8
#define	MAX_UNIT_WAIT_STATE_FIRST  1
#define	MAX_UNIT_WAIT_STATE			30

#define  SEEK_PATH_FAIL_INCREMENT	10

//----- define values for UnitOrder::mode -----//

enum { UNIT_STOP,
		 UNIT_MOVE,
		 UNIT_ATTACK,				// para=BaseObj recno of the target, loc_x & loc_y=the current location of the target
		 UNIT_BUILD_FIRM,			// para=firm id. of the firm, loc_x & loc_y=the firm location
		 UNIT_SETTLE_TOWN,		// para=no used, loc_x & loc_y=the town location
		 UNIT_ASSIGN,				// para=BaseObj recno of the place to be assigned to, loc_x & loc_y=the assign destination
		 UNIT_GO_CAST_POWER,
		 UNIT_TRANSFORM_FORTRESS,
	  };

//--------- unit mode ------------//

enum { UNIT_MODE_OVERSEE=1,         // unit_mode_para is the recno of the firm the unit is overseeing
		 UNIT_MODE_TOWN_DEFENDER,     // unit_mode_para is the recno of the town the unit is defending
		 UNIT_MODE_CAMP_DEFENDER,     // unit_mode_para is the recno of the camp where the unit is from
		 UNIT_MODE_CONSTRUCT_FIRM,    // unit_mode_para is the recno of the firm the unit is constructing
		 UNIT_MODE_CONSTRUCT_TOWN,    // unit_mode_para is the recno of the town the unit is constructing
		 UNIT_MODE_REBEL,             // unit_mode_para is the recno of the rebel group the unit belongs to
		 UNIT_MODE_MONSTER,           // unit_mode_para is the recno of the firm recno of the monster firm it belongs to
       UNIT_MODE_ON_SHIP,           // unit_mode_para is the recno of the ship unit this unit is on
		 UNIT_MODE_IN_HARBOR,         // for ships only, unit_mode_para is the recno of the harbor this marine unit is in
	  };

//--------- behavior_mode -------//

enum { UNIT_AGGRESSIVE=1,
		 UNIT_DEFENSIVE,
		 UNIT_STAND_GROUND
	  };

//------- define struct UnitOrder -----//

struct UnitOrder
{
public:
	char    	mode;
	short   	para;
	short   	loc_x;
	short   	loc_y;
	short		action_nation_recno;		// the nation that gave this order.
	WORD		ai_action_id;
	WORD		name_id;			// for storing name_id of the target unit if the order is attacking a unit

public:
	void 		set(int newMode, int newPara=0, int newXLoc=0, int newYLoc=0);

	int 		place_loc_x2();
	int 		place_loc_y2();
};


//----------- Define TeamInfo -------------//

struct TeamInfo
{
public:
	char  member_count;
	short member_unit_array[MAX_TEAM_MEMBER];
	int   ai_last_request_defense_date;
	char	formation_id;		// if this is > 0, the team has a formation
	int   formation_direction;

public:
	TeamInfo();

	void	add_member(int unitRecno);
	void 	del_member(int unitRecno);
	void 	validate();
	void	reset();
};

//----------- Define class Unit -----------//

struct PathResult;
class File;

class UnitB : public Sprite
{
friend class PathReuse;

public:
	//--------- profile vars ----------//

	short    	unit_id;				// use sprite_recno for the recno of the unit

	//-------- selection vars -----//

	char     	selected_flag;       // whether the unit has been selected or not

	//----- location info -------//

	char			loc_width;
	char			loc_height;
	int			wait_state;
	int			retry_state;
	UnitOrder 	cur_order;
	virtual int is_visible()      { return cur_x >= 0; }     // whether the unit is visible on the map, it is not invisable if cur_x == -1

	//------- location functions for BaseObj --------//

	int 			obj_loc_width() 	{ return loc_width; }
	int 			obj_loc_height()  { return loc_height; }

	int			obj_loc_x1()		{ return next_x_loc(); }
	int			obj_loc_y1()		{ return next_y_loc(); }
	int			obj_loc_x2()		{ return next_x_loc() + loc_width - 1; }
	int			obj_loc_y2()		{ return next_y_loc() + loc_height - 1; }

	int			obj_mobile_type()	{ return mobile_type; }
	int			obj_is_visible()	{ return is_visible(); }

	//---- functions for returning status ----//

	bool			is_dead();

	//----- movement vars ----//

	short			move_to_loc_x;		// the destination location of the current move_to() call, for re-searching the path when it is blocked.
	short			move_to_loc_y;
	short			wait_count;

	//------ path var -------//

	PathResult*	cur_path;
	short    	cur_path_result_id;
	int			steps_remaining;

	//---- other movement and path seeking vars ----//

	char			seek_path_fail_count;
	char			ignore_power_nation;
	int				number_of_times_being_blocked;
	bool			check_formation;
//	bool			check_freezed;
//	int				freeze_frame_left;

	//----------------------------------------------//

public:
	UnitB();
	virtual ~UnitB();

	//------- movement functions -----//

	void 			 move_to(int destXLoc, int destYLoc, bool raiseDest=1);
	virtual void stop_move();			// it should not be virtual as if it is called at the UnitB level, it will just reset movement. If it is called at the Unit level, it will also reset action_mode

	void 			 next_move();

	//---- blocking functions -----//

	bool			handle_blocking();

	//--- check whether the unit is in formation, if yes, should retry until it reach destination ---//

	bool 			is_in_formation() {return check_formation;}
//	bool			is_freezed() {return check_freezed;}
	void 			set_in_formation() {check_formation = true;}
	bool			can_move(int locX, int locY);
	bool			can_move2(int locX, int locY);
	void 			set_no_longer_in_formation() {check_formation = false;}

	//------- multiplayer checking codes --------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void 			process_move();
	void 			process_wait();
	void			process_idle();

	int   		set_next(int newNextX, int newNextY, int para, int blockedChecked);
	void 			set_idle();
	void 			set_wait();
	void 			set_turn();

	short			is_moving()			{return cur_path_result_id;}
	int 			checking_who_occupy_the_place_i_want_to_go(int checkLocX, int checkLocY);
	int			checking_who_occupy_my_next_tile(int checkLocX, int checkLocY);

private:
	void 			get_object_range(int curXLoc, int curYLoc, int &rangeX1, int &rangeY1, int &rangeX2, int &rangeY2);

	//---blocking functions--//

	void			set_no_longer_blocked() {number_of_times_being_blocked = 0;}
	void			set_not_seriously_blocked() {number_of_times_being_blocked = MAX_UNIT_BLOCKED_TIME-3;}
	void			set_first_time_blocked() {number_of_times_being_blocked = 1;}
	bool			is_blocked() {return (number_of_times_being_blocked != 0);}
	void  		process_blocked();

	//---- functions for Path Reuse Only ----//

	void 			del_old_path();
	void			set_path(PathResult* aPath) {cur_path = aPath;}
	void 			set_cur_path_result_id(short count) {cur_path_result_id = count;}
	void  		set_steps_remaining(int remainSteps) {steps_remaining = remainSteps;}
};

//----------- Define class Unit -----------//

class  UnitArray;
struct AttackInfo;
class  UnitGod;
class	 UnitCaravan;
class  UnitWagon;
class  UnitExpCart;
class  UnitMonster;

class Unit : public UnitB
{
friend class UnitArray;
friend class UnitGroup;

public:

	//--------- profile vars ----------//

	char     	rank_id;

	WORD     	name_id;          // id. of the unit's name in RaceRes::first_name_array;
	virtual const char* unit_name(int withTitle=1, int firstNameOnly=0);

	short			hero_id;				// >0 if this is a hero
	char			is_royal;
	long			unique_id;              // unique number generated across campaign
	char     	loyalty;
	char     	target_loyalty;

	short    	spy_recno;              // spy parameters

	int			is_own();
   int 			is_own_spy();
	int  			is_nation(int nationRecno);
	int 			is_civilian();
	virtual int is_camouflage(int viewerNation);				// same result across all players
	int			is_stealth();

	virtual int true_nation_recno();

	BYTE     	region_id();
	int			obj_region_id()	{ return region_id(); }

	//-------- skill vars ----------//

	Skill    	skill;

	int  			combat_level() 	{ return skill.actual_combat_level(&item); }
	int  			skill_level()		{ return skill.actual_skill_level(&item); }
	int  			max_hit_points()	{ return skill.actual_max_hit_points(&item); }

	//------ unit mode vars --------//

	char        unit_mode;
	short       unit_mode_para;      // if unit_mode==UNIT_MODE_REBEL, unit_mode_para is rebel_recno this unit belongs to
	short       rebel_recno()        { return unit_mode==UNIT_MODE_REBEL ? unit_mode_para : 0; }

	//------- politics vars ---------//

	short    	nation_contribution;    // contribution to the nation
	short    	total_reward;           // total amount of reward you have given to the unit
	int			commander_power();
	virtual int	obj_power();

	//--------- Team vars --------//

	TeamInfo*   team_info;				// for commanders only
	short       leader_unit_recno;   // recno of this unit's leader
	int         commanded_soldier_count();
	int			in_team()			{ return leader_unit_recno || team_info; }

	//-------- item vars --------//

	Item	 		item;

	//------- weapon info ----------//

	int      	get_weapon_version()                   { return nation_contribution; }
	void     	set_weapon_version(int weaponVersion)  { nation_contribution = weaponVersion; }

	// ------- magic effect vars -------//

	short			invulnerable_day_count;

	//------- order vars -------//
	
	UnitOrder 	pushed_order;
	UnitOrder	saved_order;

	void			push_order();
	void			pop_order();
	int			has_pushed_order();
	void 			clear_pushed_order();

	void		   set_ai_action_id(int aiActionId) 	{ cur_order.ai_action_id = aiActionId; }

	//------- attack parameters --------//

	AttackInfo* attack_info_array;
	AttackInfo*	cur_attack_info()		{ return attack_info_array + cur_attack; }
	char        attack_count;
	char        attack_range()			{ return attack_info_array[cur_attack].attack_range; }

	short       range_attack_x_loc;     // -1 for unable to do range_attack, use to store previous range attack location
	short       range_attack_y_loc;     // -1 for unable to do range_attack, use to store previous range attack location

	short 		attack_loc_offset_x;
	short			attack_loc_offset_y;
   
	UCHAR			attack_dir;

	short       cur_power;              // power for power attack
	short       max_power;

	int			max_attack_range();

	short			original_target_x_loc;	// for AI and player units
	short			original_target_y_loc;
	int			last_ask_attack_date;		 // moved from newly added
	void 			set_original_target(int xLoc, int yLoc);

	//------- other movement and attack behavior vars ------//

	char        can_guard_flag;         // bit0= standing guard, bit1=moving guard
													// copy from sprite_info->can_guard_flag when skill.combat level is high enough
	int   		can_stand_guard()  	{ return can_guard_flag & 1;}
	int   		can_move_guard()  	{ return can_guard_flag & 2;}
	int   		can_attack_guard()  	{ return can_guard_flag & 4;}

	bool        force_move_flag;
	bool			has_way_point;
	short			home_camp_firm_recno;
	char			behavior_mode;
	int			get_behavior_mode()	{ return is_in_formation() ? UNIT_STAND_GROUND : behavior_mode; }		// if formation is on, it's always in stand ground mode
	int			auto_retreat_hit_point;

	//---------- AI vars ---------//

	short			ai_original_target_x_loc;	// for AI units only
	short			ai_original_target_y_loc;
	bool			ai_no_suitable_action;

	// -------- special ability ----------//

	int			last_special_ability_start_date;
	char			special_abilty_id;			// see oun_spab.h
	char			special_abilty_para;			// depend on special_ability_id

	// -------- newly added member ----------//

	// int			last_ask_attack_date;		 // date which ask team member or neighbour to attack
	char			in_ai_attack_mission;	// whether this unit is in an AI attack mission 

	//------- functions for BaseObj -------//

	virtual float obj_armor();

	static char hire_hero_result;

public:
	Unit();
	virtual ~Unit();

	void 				init(int unitId, int nationRecno, int rankId, int unitLoyalty, int startXLoc, int startYLoc);
	void				deinit();

	void 				init_sprite(int startXLoc, int startYLoc);
	void 				deinit_sprite(int keepSelected=0);

	void 				init_attack_info();

	virtual	void	init_derived()		{;}
	virtual	void	deinit_derived()	{;}

	void				select(bool mainSelect=false);
	void 				deselect();

	// ----- attack info function --------//

	virtual int		obj_can_attack(int offensiveAttackOnly)	{ return can_attack(); }
	int				can_attack();//		{ return attack_count > 0; }
	virtual int 	can_attack_target(int baseObjRecno);
	int 				can_attack_with(int attackId);
	int 				can_attack_with(AttackInfo *attackInfo);
	virtual float	actual_damage();

	//------ functions pertaining to game info -------//

	void 				set_mode(char modeId, short modePara=0);
	void				set_name(WORD newNameId);
	void  			set_combat_level(int combatLevel);
	void 				set_rank(int rankId);
	void 				set_spy(int spyRecno);
	int 				get_cur_loc(short& xLoc, short& yLoc);

	//----------- gameplay functions ----------//

	void 				next_day();

	int 				betray(int newNationRecno);
	void 				change_nation(int newNationRecno);
	void 				change_hit_points(float changePoints);
	void 				change_loyalty(int changeAmt);
	void 				reward(int rewardNationRecno);
	void 				spy_change_nation(int newNationRecno, char remoteAction);
	int 				can_spy_change_nation();
	void 				resign(int remoteAction);
	int 				can_resign();
	void 				update_loyalty();
	int 				think_betray();
	int 				can_succeed_king();

	//--------- direct action functions -------//

	virtual void	left_click(int shiftPressed);
	int				select_team();		// select all members in the team

	void 				move(int destXLoc, int destYLoc, bool forceMoveFlag=false);
	void 				attack(short targetRecno, bool resetOrder);
	void 				build_firm(int buildXLoc, int buildYLoc, int firmId, char remoteAction);
	void 				settle_town(int townXLoc, int townYLoc, char remoteAction);
	void 				assign(int destXLoc, int destYLoc, int actionNationRecno= -1, bool forceMoveFlag=false);
	void 				go_cast_power(int castXLoc, int castYLoc, char remoteAction, int targetBaseObjRecno=0);
	int 				return_camp();

	int				teleport(int xLoc, int yLoc );
	virtual void   stop_move();			// it should not be virtual as if it is called at the UnitB level, it will just reset movement. If it is called at the Unit level, it will also reset action_mode
	virtual void 	stop_order();
	virtual int		sprite_speed();

	// --------- explore and visit --------//

	void				explore_area();				// explore whole rectangle
	void				explore_on_move(int curNextXLoc, int curNextYLoc, int newNextXLoc, int newNextYLoc );			// explore outside only
	void				visit_area();

	//---------- item functions --------//

	int 				pick_item(short itemId, int itemPara);
	void 				drop_item(char remoteAction);

	// -------- special ability ----------//

	int				is_special_ability_effective();
	int				think_use_special_ability();
	void				use_special_ability();

	//----------- AI functions ---------//

	virtual void	process_ai();
	void 				think_independent_unit();
	void 				think_independent_hero();
	int 				is_all_stop();
	int 				think_normal_human_action();
	int 				think_normal_monster_action();
	void 				ai_move_to_nearby_town(int shouldAssign=0);
	void 				ai_move_to_nearby_firm(int firmId, int firmId2=0);
	int 				ai_build_firm();
	int				ai_attack_next_target();

	//----------- other action functions -------//
 
	void				pre_move(int destXLoc, int destYLoc);
	int 				set_attack(short targetRecno, bool resetOrder);
	int  				set_assign(int destXLoc, int destYLoc, int actionNationRecno, bool forceMoveFlag);
	void 				set_move(int destXLoc, int destYLoc, bool forceMoveFlag=false);
	int 				set_settle_town(int townXLoc, int townYLoc);
	int					set_go_cast_power(int castXLoc, int castYLoc, int targetBaseObjRecno );
	void 				sound(const char* soundCode);
	virtual void		die()         {;}
	virtual void 		being_attack_hit(BaseObj* attackerObj, float damagePoint);
	void				add_die_effect();
	void				freeze(int freezeFrame);
//	void				unfreeze();

	//--------- hero functions --------//

	int 				is_hero_for_hire();
	int 				hero_hire_cost();
	int 				can_hire_hero(int nationRecno);
	int 				hire_hero(int nationRecno);

	//--------- interface functions ---------//

	virtual void 	disp_info(int refreshFlag);
	virtual void 	disp_unit_profile(int dispY1, int refreshFlag);
	virtual void	disp_auto_menu(int refreshFlag);

	virtual int  	detect_unit_profile();
	virtual void 	detect_info();
	virtual void 	detect_auto_menu();
	virtual int		send_hotkey(unsigned scanCode, unsigned skeyState);	// called by sys::process_key and functions it calls
	int			 	should_show_info();
	// ##### begin Gilbert 21/12 #######//
	int				is_die_after_attack();
	// ##### end Gilbert 21/12 #######//
	int				explore_for_player();

	virtual int		disp_edit_mode(int refreshFlag);
	virtual void	detect_edit_mode();

	void 			draw(int outline, int drawFlag);

	void			set_auto_retreat(int hitPointPercentage);

	//--------- file functions ---------//

	int				write_file(File *);
	int				read_file(File *);

	virtual int 	write_derived_file(File* filePtr);
	virtual int 	read_derived_file(File* filePtr);

	//------- casting functions --------//

	Unit*						cast_to_Unit()				{ return this; }	// derived from BaseObj
	virtual UnitGod*		cast_to_UnitGod()			{ return 0; }
	virtual UnitMonster*	cast_to_UnitMonster()	{ return 0; }
	virtual UnitCaravan*	cast_to_UnitCaravan()	{ return 0; }
	virtual UnitWagon*	cast_to_UnitWagon()		{ return 0; }
	virtual UnitExpCart*	cast_to_UnitExpCart()	{ return 0; }

	//------- multiplayer checking codes --------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	//--------- init functions ----------//

	void 				init_unit_id(int unitId);
	void 				init_ai();

	void				deinit_unit_id();
	void 				deinit_ai();
	void 				deinit_unit_mode();

	//----------- gameplay functions --------//

	virtual void	pre_process();
	void 				process_recover();
	void 				king_die();
	void 				general_die();
	void 				pay_expense();

	//-------- attack functions ---------//

	void 				execute_attack();
	void 				attack_now(BaseObj* targetObj, int attackDistance);
	virtual int 	process_attack();

	int 				validate_attack_target();
//	int				can_attack()		{ return attack_count > 0; }
//	virtual int 	can_attack_target(int baseObjRecno);
	int				update_attack_path_dist();
	void				target_move(BaseObj* targetObj);
	void				find_new_location_for_attack(BaseObj* targetObj);
	void				find_new_position_for_range_attack(BaseObj* targetObj);
	void				close_attack_location(BaseObj* targetObj, int& locX, int& locY);

	virtual void 	auto_react_attack(BaseObj* attackerObj);

	virtual void 	being_killed(BaseObj* attackerObj);
	void 				update_being_killed_statistic(BaseObj* attackerObj);
	void 				being_killed_leave_money(BaseObj* attackerObj);
	void 				being_killed_subordinate_betray(BaseObj* attackerObj);
	int 				most_populated_nation_nearby(int scanRange);

	void 				choose_best_attack_mode(int attackDistance, char targetMobileType);
	int 				cal_distance(int targetXLoc, int targetYLoc, int targetWidth, int targetHeight);
	void 				set_attack_dir(short curX, short curY, short targetX, short targetY);
//	int 				can_attack_with(int attackId);
//	int 				can_attack_with(AttackInfo *attackInfo);
	void 				cycle_eqv_attack();
	void				set_remain_attack_delay();
	virtual void	attack_consume_power();

	void 				add_attack_effect();
	void 				get_hit_x_y(short *xPtr, short *yPtr, char distance);

	//------ order functions -----//

	virtual void	stop_cur_order();
	virtual void 	completed_order();

	void 				execute_move();
	void 				execute_build_firm();
	int 				build_firm_now();
	void 				execute_settle_town();
	void 				execute_assign();
	int 				validate_assign();
	void 				execute_go_cast_power();
	void				execute_transform_fortress();

	void				set_die();

	//------ mode functions -------//

	void 				process_mode_town_defender();
	void 				process_mode_camp_defender();
	void 				process_mode_rebel();


	//-------- AI functions -------//

	int 				think_stop_chase();
	int 				ai_escape_fire();
	void 				think_spy_action();
	int 				think_king_action();
	int 				think_general_action();
	int 				think_leader_action();
	int 				think_assign_soldier_to_camp();
	int 				think_assign_human_to_town();
	int 				think_weapon_action();
	int 				think_assign_weapon_to_camp();
	int 				think_build_fort();
	int 				think_reward();
	void 				ai_leader_being_attacked(BaseObj* attackerObj);
	int 				think_king_flee();
	int 				think_general_flee();
	int 				ai_settle_new_town();
	int 				ai_handle_seek_path_fail();
	int 				think_aggressive_action();
	int 				think_aggressive_attack(int extendedRange=0, int fullDetect=0);
	int 				should_aggressive_attack(int targetObjRecno);
	int				ask_team_help_attack(BaseObj* attackerObj);
	int				ask_neighbour_help_attack(BaseObj* attackerObj);
	int				think_use_item();

	//------ interface functions --------//

	void  			disp_main_menu(int refreshFlag);
	void  			detect_main_menu();
	void  			disp_build_menu(int);
	void  			detect_build_menu();
	void  			disp_button(int refreshFlag);
	void  			detect_button();
	void  			disp_build(int);
	void  			detect_build();
	void  			disp_settle(int);
	void  			detect_settle();
	void  			disp_unit_info();
	void  			disp_basic_info(int dispY1, int refreshFlag);
	int   			detect_basic_info();
	void  			disp_spy_menu();
	void  			detect_spy_menu();
	int   			spy_menu_height();
	void  			disp_hit_point();
	void				disp_item_menu();
	void				detect_item_menu();
	void				disp_hire_menu(int refreshFlag);
	void				detect_hire_menu();
	void				disp_hire_result(int refreshFlag);
	void				detect_hire_result();
	void				disp_use_item_menu(int refreshFlag);
	void				detect_use_item_menu();

	void 				draw_selected();
	void 				draw_rank_icon();

	//------- game logic functions --------//

	void  			gain_experience();

	//------ UnitOrder related functions -----//

	int 				is_next_to_order_place();
	int 				is_in_order_place(int extraLoc=0);
};

//------- Define class UnitArray ---------//

class UnitArray : public SpriteArray
{
public:
	short selected_recno;		// if more than one unit is selected, it is always set to the unit with the highest rank.
	int 	selected_count;

public:
	UnitArray(int);

	void  	init();
	void  	deinit();

	void		process();
	void		draw_dot(int filterUnitId=0);
	void		draw_profile();

	int   	add_unit(int unitId, int nationRecno, int rankId=0, int unitLoyalty=0, int startXLoc= -1, int startYLoc= -1);
	Unit* 	create_unit(int unitId);
	int		unit_class_size(int unitId);
	void		del(int unitRecno);

	void  	disappear_in_town(int unitRecno, int townRecno);
	void  	disappear_in_firm(int unitRecno);
	void  	die(int unitRecno);

	void 		stop_all_war(short stopNationRecno);
	void 		stop_war_between(short nationRecno1, short nationRecno2);
	void 		stop_attack_obj(int baseObjRecno, int nationRecno=0);

	#ifdef DEBUG
		Unit* operator[](int recNo);
	#else
		Unit* operator[](int recNo)   { return (Unit*) get_ptr(recNo); }
	#endif

	int   	is_deleted(int recNo);
	int   	is_truly_deleted(int recNo);

	//------- file functions --------//

	int			write_file(File *);
	int			read_file(File *);

private:
	void 		group_by_mobile_type();
};

//------------------------------------------//

extern UnitArray unit_array;
extern unsigned long	unit_process_town_defender_profile_time;
extern unsigned long	unit_process_camp_defender_profile_time;
extern unsigned long	unit_process_rebel_profile_time;
extern unsigned long	unit_execute_move_profile_time;
extern unsigned long	unit_execute_attack_profile_time;
extern unsigned long	unit_execute_build_firm_profile_time;
extern unsigned long	unit_execute_settle_town_profile_time;
extern unsigned long	unit_execute_assign_profile_time; 
extern unsigned long	unit_check_build_firm_profile_time;
extern unsigned long	unit_cast_power_profile_time;
extern unsigned long	unit_transform_fortress_profile_time;
extern unsigned long	last_unit_process_town_defender_profile_time;
extern unsigned long	last_unit_process_camp_defender_profile_time;
extern unsigned long	last_unit_process_rebel_profile_time;
extern unsigned long	last_unit_execute_move_profile_time;
extern unsigned long	last_unit_execute_attack_profile_time;
extern unsigned long	last_unit_execute_build_firm_profile_time;
extern unsigned long	last_unit_execute_settle_town_profile_time;
extern unsigned long	last_unit_execute_assign_profile_time; 
extern unsigned long	last_unit_check_build_firm_profile_time;
extern unsigned long	last_unit_cast_power_profile_time;
extern unsigned long	last_unit_transform_fortress_profile_time;

#endif

