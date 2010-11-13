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

//Filename    : OF_CAMP.H
//Description : Header of FirmCamp

#ifndef __OF_CAMP_H
#define __OF_CAMP_H

#ifndef __OSOLDIER_H
#include <OSOLDIER.H>
#endif

#ifndef __OFIRM_H
#include <OFIRM.H>
#endif

//-------- Define training parameters --------//

#define MAX_SOLDIER						15

//------- define firm menu modes --------//

enum { FIRM_MENU_MAIN,
		 FIRM_MENU_TRAIN,
		 FIRM_MENU_SPY,
		 FIRM_MENU_SELECT_BRIBER,
		 FIRM_MENU_SET_BRIBE_AMOUNT,
		 FIRM_MENU_BRIBE_RESULT,
		 FIRM_MENU_VIEW_SECRET,
		 FIRM_MENU_ASSASSINATE_CONFIRM,
		 FIRM_MENU_ASSASSINATE_RESULT,
		 FIRM_MENU_STEAL_TECH_CONFIRM,
		 FIRM_MENU_STEAL_TECH_RESULT,
		 FIRM_MENU_INCIDENT_NATION,
		 FIRM_MENU_INCIDENT_CONFIRM,
		 FIRM_MENU_INCIDENT_RESULT,
		 FIRM_MENU_CAMP_LAST 		// keep this last for subclass to go on
};

//------------ define constant -----------//

enum	{	INSIDE_CAMP=0,
			OUTSIDE_CAMP,
		};

//-------- Define struct DefenseUnit ----------//

struct DefenseUnit
{
	short	unit_recno;
	char	status;	// inside / outside the camp
};

//------- Define class FirmCamp --------//

class Town;
class Unit;

class FirmCamp : public Firm
{
public:
	//---- overseer & soldiers functions ----//

	short  		overseer_recno;

	Soldier	 	soldier_array[MAX_SOLDIER];
	char    		soldier_count;       			// number of soldiers currently working in the firm
	int			ready_soldier_count();			// don't count those under training

	char			selected_soldier_id;

	char	 		player_spy_count;
	char			defense_flag;

	//------ functions for returning values ----//

	int			majority_race();

	//---------- AI vars ----------//

	char 			patrol_unit_count;              	// for AI to get the recno of the patrol units
	short			patrol_unit_array[MAX_SOLDIER+1];

	char 			coming_unit_count;              	// for AI to get the recno of the coming units
	short			coming_unit_array[MAX_SOLDIER+1];

	short			ai_capture_town_recno;		// >0 if the troop is in the process of attacking the independent village for capturing it.
	char			ai_recruiting_soldier;

	char			is_attack_camp;

	//------ AI functions for returning values -----//

	int 			total_combat_level();
	int 			average_combat_level();
	virtual int ai_combat_level_needed();

	int	   	ai_has_excess_soldier();

	//--------- static vars ----------//

	static char  firm_menu_mode;
	static short action_spy_recno;	// recno of the spy that is doing the bribing or viewing secret reports of other nations
	static char  bribe_result;
	static char  assassinate_result;
	static short steal_tech_result;
	static char	 create_incident_result;
	static short action_target_recno;		// recno of bribe/assassinate/incident target
	static short spy_action_chance;
	static short swap_item_id;

public:
	FirmCamp();
	~FirmCamp();

	void		init_derived();
	void		deinit();

	//----------- process functions --------//

	void		next_day();

	//--------- interface functions --------//

	void 		put_info(int refreshFlag);
	void 		detect_info();
	int  		should_show_info();
	int		explore_for_player();
	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	//---- functions for patrol (moving out) -----//

	void		patrol(short patrolType = 0);
	int		patrol_all_soldier(int overseerRecno = 0, char patrolType = 0);
	void 		reset_unit_home_camp(int firmRecno);

	//------------ overseer & soldier functions -----------//

	virtual int	 is_soldier_full();

	void 		assign_unit(int unitRecno);
	void 		assign_overseer(int overseerRecno);
	void		assign_soldier(int unitRecno);

	int	 	mobilize_soldier(int soldierId, char remoteAction);
	int  		mobilize_overseer();

	void 		resign_overseer()		{ assign_overseer(0); }
	void		resign_all_soldier(int disappearFlag=0);
	int		resign_soldier(int soldierId);
	int		create_soldier_unit(Soldier& thisSoldier);

	void		kill_overseer();
	void		kill_all_soldier();
	void		kill_soldier(int soldierId);

	virtual void	cancel_train_soldier(int soldierId, char remoteAction);
	void		free_all_people();				// call when the firm destruct

	int		invoke_defense(BaseObj* attackerObj, int netAttackerPower);
	void		promote_soldier( int soldierId, char remoteAction );
	int		swap_item( int fromSoldierId, int toSoldierId, int verifyItemId, char remoteAction );

	// -------- virtual function affecting linked town ------ //

	virtual int resistance_adjustment()  { return 80; }		// FIRM_CAMP is 100%
	virtual int loyalty_adjustment()     { return 80; }		// FIRM_CAMP is 100%

	//------------ bribing functions -----------//

	int 		can_spy_bribe(int bribeSoldierId, int briberNationRecno);
	int 		spy_bribe(int bribeAmount, short briber, short soldierId);
	int 		spy_bribe_succeed_chance(int bribeAmount, short briberSpyRecno, short soldierId);
	int 		validate_cur_bribe();

	//--------- class casting function --------//

	virtual	FirmCamp *cast_to_FirmCamp() { return this; };

	//------------ misc functions -------------//

	void 		change_nation(int newNationRecno);
	int		can_assign_capture();
	void		capture_firm(int newNationRecno);
	void		reward(int soldierId, int remoteAction);
	void 		pay_expense();

	int 		builder_stay_after_construction();

	//----------- AI functions ----------//

	virtual void process_ai();
	virtual int	 ai_should_close();			// overloaded function
	void		ai_update_link_status();

	int 		cur_commander_leadership(int bestRaceId=0);
	int 		new_commander_leadership(int newRaceId, int newSkillLevel);

	//----- derived function from BaseObj -----//

	virtual bool	can_accept_assign(int unitRecno, int actionNationRecno= -1);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//------------------------------------------//

protected:

	//-------- functions for updating vars ---------//

	void 		update_influence();
	void		update_loyalty();

	//--------- process functions ---------//

	void		advanced_train();
	void 		recover_hit_point();
	void		consume_food();

	virtual void	basic_train();

	//----------- misc functions -------------//

	void 		reset_attack_camp(int firmRecno);

	void 		sort_soldier();
	void 		validate_patrol_unit();
	void		validate_soldier();

	int		can_soldier_capture(int captureNationRecno);

	//-------- interface functions ---------//

	virtual void disp_camp_info(int dispY1, int refreshFlag);
	virtual void detect_camp_info();

			  void disp_camp_upgrade(int dispY1, int refreshFlag);
			  void disp_soldier_list(int dispY1, int refreshFlag, int dispSpyMenu);
			  int  detect_soldier_list(int selectSpyMenu);
			  void disp_soldier_info(int dispY1, int refreshFlag);
			  void detect_soldier_info();
			  void disp_overseer_info(int dispY1, int refreshFlag);
			  void detect_overseer_info();
	virtual void disp_train_menu(int refreshFlag);
	virtual void detect_train_menu();

	//------- spy interface functions -----------//

			  void disp_spy_button(int x, int y, int refreshFlag);
			  void detect_spy_button();
			  void disp_spy_menu(int refreshFlag);
			  void detect_spy_menu();
			  int  can_player_spy_capture();
			  void disp_spy_info(int dispY1, int refreshFlag);

			  void disp_bribe_menu(int refreshFlag);
			  void detect_bribe_menu();
			  void disp_bribe_result(int refreshFlag);
			  void detect_bribe_result();
			  void spy_bribe(int bribeAmount);
			  void disp_bribe_unit(int dispY1);

			  void disp_assassinate_confirm(int refreshFlag);
			  void detect_assassinate_confirm();
			  void disp_assassinate_result(int refreshFlag);
			  void detect_assassinate_result();

			  void disp_steal_tech_confirm(int refreshFlag);
			  void detect_steal_tech_confirm();
			  void disp_steal_tech_result(int refreshFlag);
			  void detect_steal_tech_result();

			  void disp_incident_nation(int refreshFlag);
			  void detect_incident_nation();
			  void disp_incident_confirm(int refreshFlag);
			  void detect_incident_confirm();
			  void disp_incident_result(int refreshFlag);
			  void detect_incident_result();

	//-------------- AI functions --------------//

	virtual void think_recruit();
	int 		think_attack();
	virtual int ai_recruit(int recruitCount);
	void		ai_attack_town_defender(Unit*);
	int 		think_attack_invader();
	int 		think_attack_nearby_enemy();
	void 		think_change_town_link();

	void 		think_capture();
	int		think_capture_return();
	int 		think_capture_use_spy(Town* targetTown);
	int 		think_capture_use_spy2(Town* targetTown, int raceId, int curSpyLevel);
	int 		think_assign_better_overseer(Town* targetTown);
	int 		think_assign_better_overseer2(int targetTownRecno, int raceId);
	void 		process_ai_capturing();

	int 		think_capture_target_town();
	int 		ai_capture_independent_town(Town* targetTown, int defenseCombatLevel);
	int 		ai_capture_enemy_town(Town* targetTown, int defenseCombatLevel);
	int 		think_attack_enslaving_monster();

	int 		think_employ_to_capture();
	int		think_use_cash_to_capture();
	void 		think_linked_town_change_nation(int linkedTownRecno, int oldNationRecno, int newNationRecno);

	int 		think_assign_better_commander();
	int 		best_commander_race();
	int 		think_upgrade_to_fort();
};

//--------------------------------------//

#endif
