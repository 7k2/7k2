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

// Filename    : OC_EAST.H
// Description : Header file of class CampaignEastWest

#ifndef __OC_EAST_H
#define __OC_EAST_H

#ifndef __OCAMPGN_H
#include <OCAMPGN.H>
#endif

//--------- Define variable type ----------//

class CampaignEastWest;

typedef void (CampaignEastWest::*CampaignEastWestFP)();

//-------- define class CampaignEastWest ----------//

class Firm;
class Town;

class CampaignEastWest : public Campaign
{
public:
	enum { STAGE_FRYHTAN_INVASION_1=1,  	// 1
			 STAGE_FRYHTAN_INVASION_2,			// 2
			 STAGE_BUILD_UP,                 // 3
			 STAGE_BATTLE_EAST,            	// 4
			 STAGE_DEFENSE,                	// 5
			 STAGE_HERO_DEBUT,               // 6
			 STAGE_FIGHT_BACK_1,             // 7
			 STAGE_FIGHT_BACK_2,             // 8
			 STAGE_EASTERN_EMPIRE_INVADE_OUR_LAND, // 9
			 STAGE_OPPORTUNISTIC_ATTACK,     // 10
			 STAGE_FRYHTAN_JOINT_ATTACK,     // 11
			 STAGE_FRYHTAN_RAGE_ATTACK,      // 12
			 STAGE_JOINT_FIGHT_BACK,         // 13
			 STAGE_DEEP_INTO_FRYHTAN_LAND,   // 14
			 STAGE_ACQUIRE_MEGA_WEAPON,      // 15
			 STAGE_USE_MEGA_WEAPON,          // 16
			 STAGE_TERMINATE_FRYHTAN,        // 17
			 STAGE_JOINT_TERMINATE_FRYHTAN,  // 18
			 STAGE_ATTACK_EASTERN_EMPIRE_1,  // 19
			 STAGE_ATTACK_EASTERN_EMPIRE_2,  // 20
			 STAGE_NEW_KINGDOM_INVASION,     // 21
			 STAGE_ATTACK_NEW_KINGDOM,       // 22
			 STAGE_LAST_BATTLE,              // 23
			 STAGE_EAST_JOINT_ATTACK_FRYHTAN,// 24
			 STAGE_UPRISING,          			// 25
			 STAGE_TRAIN_LEADERS,				// 26
			 STAGE_GROW_POPULATION,				// 27
			 STAGE_RESTORE_PROSPERITY,			// 28
		  };

	enum { EVENT_FRYHTAN_TAKEOVER=1,       		// 1
			 EVENT_RANDOM_POINT_1,                 // 2
			 EVENT_STATE_BETRAYAL,                 // 3
			 EVENT_FRYHTAN_OFFER_PEACE,            // 4
			 EVENT_GIVE_THRONE,                    // 5
			 EVENT_RANDOM_POINT_2,                 // 6
			 EVENT_EASTERN_EMPIRE_OFFER_HELP,      // 7
			 EVENT_FRYHTAN_ATTACK_FRYHTAN,         // 8
			 EVENT_FRYHTAN_ASK_FOR_JOINT_ATTACK,   // 9
			 EVENT_RANDOM_POINT_3,                 // 10
			 EVENT_FRYHTAN_THINK_SURRENDER,        // 11
			 EVENT_ASK_EASTERN_EMPIRE_FOR_HELP,    // 12
			 EVENT_ATTEMPT_TO_UNITE,               // 13
			 EVENT_RANDOM_POINT_4,                 // 14
			 EVENT_ULTIMATE_VICTORY,               // 15
			 EVENT_FRYHTAN_INVADE_EAST,				// 16
			 EVENT_EAST_ASK_FOR_JOINT_ATTACK,		// 17
			 EVENT_FRYHTAN_BREAK_UP,					// 18
			 EVENT_PLAYER_TAKE_OVER_FRYHTAN,			// 19
			 EVENT_DISBAND_FRYHTAN_ARMY,				// 20
			 EVENT_INDEPENDENT_SURRENDER, 			// 21
			 EVENT_FRYHTAN_ARMY_REVOLT,				// 22
			 EVENT_PLAYER_TAKE_OVER_EAST,				// 23
		  };

	enum { PLOT_REBEL_JOIN_FORCE = 1,
		  };

	//--------------------------------------//

public:
	short		hero_unit_recno;

protected:
	short		western_nation_recno;
	short		eastern_nation_recno;
	short 	cur_monster_nation_recno;
	short		target_monster_nation_recno;		// the target Fryhtan nation of the Human-Fryhtan joint attack game
	short		new_kingdom_nation_recno;

	//---- common campaign vars ----//

	short		hero_debut_state_count;			// The hero debut event will happen when the number of player states drops to this number
	char		fryhtan_joint_attack_reward;
	char		which_edge;                   // for determining which edge the reinforcement should come from
	char		defeated_eastern_empire_invasion;		// this is true if the player has defeated an invasion from the eastern empire
	short		mega_tech_id;
	short		player_beat_fryhtan_count;
	short		player_beat_fryhtan_max;
	short		monster_take_over_east_count;
	short		east_joint_attack_fryhtan_win_count;
	short		saved_player_state_count;
	short		use_fryhtan_count;
	short		use_fryhtan_max;
	char		disband_fryhtan_reply;		// 1 - agree, -1 - refuse, 2-refused, but the Fryhtan has revolted and form a new kingdom. So disband_fryhtan_reply is no longer useful.
	char		fryhtan_surrendered_state_recno;		// a state that Fryhtans have surrendered to you.

	//--- campaign plot vars ----//

	CampaignEastWestFP	plot_create_game_FP;
	CampaignEastWestFP	plot_next_day_FP;

	static	CampaignEastWestFP plot_create_game_FP_array[];
	static	CampaignEastWestFP plot_next_day_FP_array[];

	short		plot_enemy_nation_recno;
	short		plot_nation_recno1;
	short		plot_nation_recno2;

	//--------------------------------------//

public:
	virtual void 	next_day();
	virtual void 	del_nation(int nationRecno);

	virtual char*	goal_text();
	virtual char*	intro_text();
	virtual char*	plot_text();

	//------- event functions ---------//

	int		event_fryhtan_takeover();
	int		event_random_point_1();
	int		event_state_betrayal();
	int 		event_fryhtan_offer_peace();
	int		event_give_throne();
	int		event_random_point_2();
	int		event_eastern_empire_offer_help();
	int		event_fryhtan_attack_fryhtan();
	int		event_fryhtan_ask_for_joint_attack();
	int		event_random_point_3();
	int		event_fryhtan_think_surrender();
	int		event_ask_eastern_empire_for_help();
	int		event_attempt_to_unite();
	int		event_random_point_4();
	int 		event_ultimate_victory();
	int 		event_fryhtan_invade_east();
	int 		event_east_ask_for_joint_attack();
	int 		event_fryhtan_break_up();
	int 		event_player_take_over_fryhtan();
	int 		event_disband_fryhtan_army();
	int 		event_independent_surrender();
	int 		event_fryhtan_army_revolt();
	int		event_player_take_over_east();

	// ------ save game function -------//

	virtual int		write_derived_file(File *filePtr);
	virtual int		read_derived_file(File *filePtr);

			  void 	stage_write_file(File* filePtr);
			  void 	stage_read_file(File* filePtr);

	//-------- plot functions ------//

	void		plot_null()					{;}
	void		plot_a1_create_game();
	void		plot_a1_next_day();
	void		plot_a2_create_game();
	void		plot_a2_next_day();
	void		plot_a3_create_game();
	void		plot_a3_next_day();
	void		plot_a4_create_game();
	void		plot_a4_next_day();
	void		plot_a5_create_game();
	void		plot_a5_next_day();
	void		plot_a6_create_game();
	void		plot_a6_next_day();
	void		plot_a7_create_game();
	void		plot_a7_next_day();

	void		plot_c1_create_game();
	void		plot_c1_next_day();
	void		plot_c2_create_game();
	void		plot_c2_next_day();
	void		plot_c3_create_game();
	void		plot_c3_next_day();
	void		plot_c4_create_game();
	void		plot_c4_next_day();

	void		plot_d1_create_game();
	void		plot_d1_next_day();
	void		plot_d2_create_game();
	void		plot_d2_next_day();
	void		plot_d3_create_game();
	void		plot_d3_next_day();
	void		plot_d4_create_game();
	void		plot_d4_next_day();
	void		plot_d5_create_game();
	void		plot_d5_next_day();

protected:
	virtual void 	init_new_campaign_derived();
	virtual void 	create_pregame_object();
	virtual void 	stage_prelude();
	virtual int  	process_event();
	virtual void  	disp_end_game_msg();

private:
	void 		create_nation();
	void 		set_state_nation();

	int 		get_triangle_states1(int& humanStateRecno, int& monsterStateRecno1, int& monsterStateRecno2);
	int 		get_triangle_states2(int& humanStateRecno, int& eastStateRecno, int& monsterStateRecno);

	int 		player_pick_attack_target(int targetType);

	virtual void process_game_result();

	void 		init_random_plot(char plotCategory);

	//---- stage 1 init functions -------//

	void		stage_1_prelude();
	int 		stage_1_create_game();
	int 		stage_1_create_nation();
	int 		stage_1_create_town();
	int 		stage_1_create_firm();
	void 		stage_1_init_vars();
	void		stage_1_init_relation();

	//--- stage 1 process functions -----//

	void 		stage_1_next_day();
	void 		stage_1_process_game_result();
	void  	stage_1_disp_end_game_msg();

	//---- stage 2 init functions -------//

	void		stage_2_prelude();
	int 		stage_2_create_game();
	int 		stage_2_create_nation();
	int 		stage_2_create_town();
	int 		stage_2_create_firm();
	void 		stage_2_init_vars();
	void		stage_2_init_relation();

	//--- stage 2 process functions -----//

	void 		stage_2_next_day();
	void 		stage_2_process_game_result();
	void  	stage_2_disp_end_game_msg();
	char* 	stage_2_goal_text();
	char* 	stage_2_intro_text();
	void 		stage_2_write_file(File* filePtr);
	void 		stage_2_read_file(File* filePtr);

	//---- stage 3 init functions -------//

	void		stage_3_prelude();
	int 		stage_3_create_game();
	int 		stage_3_create_nation();
	int 		stage_3_create_town();
	int 		stage_3_create_firm();
	void 		stage_3_init_vars();

	//--- stage 3 process functions -----//

	void 		stage_3_next_day();
	void 		stage_3_process_game_result();
	void  	stage_3_disp_end_game_msg();

	//---- stage 4 init functions -------//

	void		stage_4_prelude();
	int 		stage_4_create_game();
	int 		stage_4_create_nation();
	int 		stage_4_create_town();
	int 		stage_4_create_firm();
	void 		stage_4_init_vars();
	void		stage_4_init_relation();

	//--- stage 4 process functions -----//

	void 		stage_4_next_day();
	void 		stage_4_process_game_result();
	void  	stage_4_disp_end_game_msg();

	//---- stage 5 init functions -------//

	void		stage_5_prelude();
	int 		stage_5_create_game();
	int 		stage_5_create_nation();
	int 		stage_5_create_town();
	int 		stage_5_create_firm();
	void 		stage_5_init_vars();
	void		stage_5_init_relation();

	//--- stage 5 process functions -----//

	void 		stage_5_next_day();
	void 		stage_5_process_game_result();
	void  	stage_5_disp_end_game_msg();

	//---- stage 6 init functions -------//

	void		stage_6_prelude();
	int 		stage_6_create_game();
	int 		stage_6_create_nation();
	int 		stage_6_create_town();
	int 		stage_6_create_firm();
	void 		stage_6_init_vars();
	void		stage_6_init_relation();

	//--- stage 6 process functions -----//

	void 		stage_6_next_day();
	void 		stage_6_create_hero();;
	void 		stage_6_process_game_result();
	void  	stage_6_disp_end_game_msg();

	//---- stage 7 init functions -------//

	void		stage_7_prelude();
	int 		stage_7_create_game();
	int 		stage_7_create_nation();
	int 		stage_7_create_lair();
	void 		stage_7_init_vars();
	void		stage_7_init_relation();

	//--- stage 7 process functions -----//

	void 		stage_7_next_day();
	void 		stage_7_process_game_result();
	void  	stage_7_disp_end_game_msg();

	//---- stage 8 init functions -------//

	void		stage_8_prelude();
	int 		stage_8_create_game();
	int 		stage_8_create_nation();
	int 		stage_8_create_lair();
	void 		stage_8_init_vars();
	void		stage_8_init_relation();

	//--- stage 8 process functions -----//

	void 		stage_8_next_day();
	void 		stage_8_process_game_result();
	void  	stage_8_disp_end_game_msg();

	//---- stage 9 init functions -------//

	void		stage_9_prelude();
	int 		stage_9_create_game();
	int 		stage_9_create_nation();
	int 		stage_9_create_town();
	int 		stage_9_create_firm();
	void 		stage_9_init_vars();
	void		stage_9_init_relation();

	//--- stage 9 process functions -----//

	void 		stage_9_next_day();
	void 		stage_9_process_game_result();
	void  	stage_9_disp_end_game_msg();

	//---- stage 10 init functions -------//

	void		stage_10_prelude();
	int 		stage_10_create_game();
	int 		stage_10_create_nation();
	int 		stage_10_create_lair();
	void 		stage_10_init_vars();
	void		stage_10_init_relation();

	//--- stage 10 process functions -----//

	void 		stage_10_next_day();
	void 		stage_10_monster_reinforcement();
	void 		stage_10_process_game_result();
	void  	stage_10_disp_end_game_msg();

	//---- stage 11 init functions -------//

	void		stage_11_prelude();
	int 		stage_11_create_game();
	int 		stage_11_create_nation();
	int 		stage_11_create_lair();
	void 		stage_11_init_vars();
	void		stage_11_init_relation();

	//--- stage 11 process functions -----//

	void 		stage_11_next_day();
	void 		stage_11_process_game_result();
	void  	stage_11_disp_end_game_msg();

	//---- stage 12 init functions -------//

	void		stage_12_prelude();
	int 		stage_12_create_game();
	int 		stage_12_create_nation();
	int 		stage_12_create_town();
	int 		stage_12_create_firm();
	void 		stage_12_init_vars();
	void		stage_12_init_relation();

	//--- stage 12 process functions -----//

	void 		stage_12_next_day();
	void 		stage_12_process_game_result();
	void  	stage_12_disp_end_game_msg();

	//---- stage 13 init functions -------//

	void		stage_13_prelude();
	int 		stage_13_create_game();
	int 		stage_13_create_nation();
	int 		stage_13_create_lair();
	void 		stage_13_init_vars();
	void		stage_13_init_relation();

	//--- stage 13 process functions -----//

	void 		stage_13_next_day();
	void 		stage_13_process_game_result();
	void  	stage_13_disp_end_game_msg();

	//---- stage 14 init functions -------//

	void		stage_14_prelude();
	int 		stage_14_create_game();
	int 		stage_14_create_nation();
	int 		stage_14_create_lair();
	void 		stage_14_init_vars();
	void		stage_14_init_relation();

	//--- stage 14 process functions -----//

	void 		stage_14_next_day();
	void 		stage_14_process_game_result();
	void  	stage_14_disp_end_game_msg();

	//---- stage 15 init functions -------//

	void		stage_15_prelude();
	int 		stage_15_create_game();
	int 		stage_15_create_nation();
	int 		stage_15_create_lair();
	void 		stage_15_init_vars();
	void		stage_15_init_relation();

	//--- stage 15 process functions -----//

	void 		stage_15_next_day();
	void 		stage_15_process_game_result();
	void  	stage_15_disp_end_game_msg();

	//---- stage 16 init functions -------//

	void		stage_16_prelude();
	int 		stage_16_create_game();
	int 		stage_16_create_nation();
	int 		stage_16_create_lair();
	void 		stage_16_init_vars();
	void		stage_16_init_relation();

	//--- stage 16 process functions -----//

	void 		stage_16_next_day();
	void 		stage_16_process_game_result();
	void  	stage_16_disp_end_game_msg();

	//---- stage 17 init functions -------//

	void		stage_17_prelude();
	int 		stage_17_create_game();
	int 		stage_17_create_nation();
	int 		stage_17_create_lair();
	void 		stage_17_init_vars();
	void		stage_17_init_relation();

	//--- stage 17 process functions -----//

	void 		stage_17_next_day();
	void 		stage_17_process_game_result();
	void  	stage_17_disp_end_game_msg();

	//---- stage 18 init functions -------//

	void		stage_18_prelude();
	int 		stage_18_create_game();
	int 		stage_18_create_nation();
	int 		stage_18_create_lair();
	void 		stage_18_init_vars();
	void		stage_18_init_relation();

	//--- stage 18 process functions -----//

	void 		stage_18_next_day();
	void 		stage_18_process_game_result();
	void  	stage_18_disp_end_game_msg();

	//---- stage 19 init functions -------//

	void		stage_19_prelude();
	int 		stage_19_create_game();
	int 		stage_19_create_nation();
	int 		stage_19_create_town();
	int 		stage_19_create_firm();
	int 		stage_19_create_lair();
	void 		stage_19_init_vars();
	void		stage_19_init_relation();

	//--- stage 19 process functions -----//

	void 		stage_19_next_day();
	void 		stage_19_process_game_result();
	void  	stage_19_disp_end_game_msg();

	//---- stage 20 init functions -------//

	void		stage_20_prelude();
	int 		stage_20_create_game();
	int 		stage_20_create_nation();
	int 		stage_20_create_town();
	int 		stage_20_create_firm();
	void 		stage_20_init_vars();
	void		stage_20_init_relation();

	//--- stage 20 process functions -----//

	void 		stage_20_next_day();
	void 		stage_20_process_game_result();
	void  	stage_20_disp_end_game_msg();

	//---- stage 21 init functions -------//

	void		stage_21_prelude();
	int 		stage_21_create_game();
	int 		stage_21_create_nation();
	int 		stage_21_create_town();
	int 		stage_21_create_firm();
	void 		stage_21_init_vars();
	void		stage_21_init_relation();

	//--- stage 21 process functions -----//

	void 		stage_21_next_day();
	void 		stage_21_process_game_result();
	void  	stage_21_disp_end_game_msg();

	//---- stage 22 init functions -------//

	void		stage_22_prelude();
	int 		stage_22_create_game();
	int 		stage_22_create_nation();
	int 		stage_22_create_town();
	int 		stage_22_create_firm();
	void 		stage_22_init_vars();
	void		stage_22_init_relation();

	//--- stage 22 process functions -----//

	void 		stage_22_next_day();
	void 		stage_22_process_game_result();
	void  	stage_22_disp_end_game_msg();

	//---- stage 23 init functions -------//

	void		stage_23_prelude();
	int 		stage_23_create_game();
	int 		stage_23_create_nation();
	int 		stage_23_create_town();
	int 		stage_23_create_firm();
	void 		stage_23_init_vars();
	void		stage_23_init_relation();

	//--- stage 23 process functions -----//

	void 		stage_23_next_day();
	void 		stage_23_process_game_result();
	void  	stage_23_disp_end_game_msg();

	//---- stage 24 init functions -------//

	void		stage_24_prelude();
	int 		stage_24_create_game();
	int 		stage_24_create_nation();
	int 		stage_24_create_lair();
	void 		stage_24_init_vars();
	void		stage_24_init_relation();

	//--- stage 24 process functions -----//

	void 		stage_24_next_day();
	void 		stage_24_process_game_result();
	void  	stage_24_disp_end_game_msg();

	//---- stage 25 init functions -------//

	void		stage_25_prelude();
	int 		stage_25_create_game();
	int 		stage_25_create_nation();
	int 		stage_25_create_town();
	int 		stage_25_create_firm();
	void 		stage_25_init_vars();
	char* 	stage_25_goal_text();
	char* 	stage_25_intro_text();
	void 		stage_25_write_file(File* filePtr);
	void 		stage_25_read_file(File* filePtr);

	//--- stage 25 process functions -----//

	void 		stage_25_next_day();
	void 		stage_25_process_game_result();
	void  	stage_25_disp_end_game_msg();

	//---- stage 26 init functions -------//

	void		stage_26_prelude();
	int 		stage_26_create_game();
	int 		stage_26_create_nation();
	int 		stage_26_create_town();
	int 		stage_26_create_firm();
	void 		stage_26_init_vars();
	char* 	stage_26_goal_text();
	char* 	stage_26_intro_text();
	void 		stage_26_write_file(File* filePtr);
	void 		stage_26_read_file(File* filePtr);

	//--- stage 26 process functions -----//

	void 		stage_26_next_day();
	void 		stage_26_process_game_result();
	void  	stage_26_disp_end_game_msg();

	//---- stage 27 init functions -------//

	void		stage_27_prelude();
	int 		stage_27_create_game();
	int 		stage_27_create_nation();
	int 		stage_27_create_town();
	int 		stage_27_create_firm();
	void		stage_27_create_egyptian_rebel();
	void 		stage_27_init_vars();
	char* 	stage_27_goal_text();
	char* 	stage_27_intro_text();
	void 		stage_27_write_file(File* filePtr);
	void 		stage_27_read_file(File* filePtr);

	//--- stage 27 process functions -----//

	void 		stage_27_next_day();
	void 		stage_27_process_game_result();
	void  	stage_27_disp_end_game_msg();

	//---- stage 28 init functions -------//

	void		stage_28_prelude();
	int 		stage_28_create_game();
	int 		stage_28_create_nation();
	void 		stage_28_init_vars();
	char* 	stage_28_goal_text();
	char* 	stage_28_intro_text();
	void 		stage_28_write_file(File* filePtr);
	void 		stage_28_read_file(File* filePtr);

	//--- stage 28 process functions -----//

	void 		stage_28_next_day();
	void 		stage_28_process_game_result();
	void  	stage_28_disp_end_game_msg();
};

//--------------------------------------------------//

#endif