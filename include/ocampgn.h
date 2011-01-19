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

// Filename    : OCAMPGN.H
// Description : Header file of campaign class

#ifndef __OCAMPGN_H
#define __OCAMPGN_H

#include <gamedef.h>
#include <oskill.h>
#include <odynarr.h>
#include <obattle.h>
#include <oconfig.h>
#include <otech.h>
#include <oresx.h>
#include <oplasma.h>
#include <ofont.h>

//----------- define constant -------------//

#define MAX_NOBLE 			20
#define MAX_NEXT_SCENARIO 	3
#define MAX_NATION_POWER 	200
#define AVG_NATION_POWER 	20

#define MAX_STAGE    		100
#define MAX_EVENT				100

#define MAIN_TERRAIN_WIDTH	512
#define MAIN_TERRAIN_HEIGHT 512

enum { CAMPAIGN_NATION_OWN=1, CAMPAIGN_NATION_MONSTER, CAMPAIGN_NATION_AI };

enum { CAMPAIGN_PLAYER_NATION_RECNO=1 };

enum { MIN_CAMPAIGN_DIFFICULTY=1,
		 MAX_CAMPAIGN_DIFFICULTY=5  };

enum { BONUS_MAX_YEAR = 20 };

//------- define minister id. --------//

#define MINISTER_COUNT 		6				// the maximum between human and Fryhtans

enum { MINISTER_OF_WAR=1,					// id. for human ministers
		 MINISTER_OF_FOREIGN_AFFAIRS,
		 MINISTER_OF_FOREIGN_AFFAIRS2,
		 MINISTER_OF_FOREIGN_AFFAIRS2_ASSIST,
		 MINISTER_OF_FOREIGN_AFFAIRS3_ASSIST,
		 MINISTER_HOME,
	  };

enum { MINISTER_ORDO=1,						// id. for Fryhtan ministers - they share the same array
	  };

//------- define mountain layout constants -------//

enum { MOUNTAIN_LAYOUT_X_COUNT = 20,
		 MOUNTAIN_LAYOUT_Y_COUNT = 20  };

//--------- Define campaign id. -------//

enum { CAMPAIGN_EAST_WEST = 1,
		CAMPAIGN_TUTORIAL_HUMAN, 
		CAMPAIGN_FRYHTAN,
		CAMPAIGN_TUTORIAL_FRYHTAN};

//----- Define struct CampaignSituation -----//

enum { CAMPAIGN_SITUATION_COUNT = 6 };

struct CampaignSituation
{
	char 	cash_level;		// 0 to 100
	char	food_level;
	char  mine_raw_level;		// raw material reserves of existing mines
	char	site_raw_level;		//	mineral deposits of sites
	char 	population_level;
	char	reputation_level;
	char	tech_level;
	char	economy_level;			// development level of economic facilities
	char	military_level;
	char	town_loyalty_level;
	char	unit_loyalty_level;
	char	unit_combat_level;	 // combat level of existing units and soldiers in forts
};

//----------- define class CampaignMember ----------//

class Unit;
class Spy;
struct Soldier;

class CampaignMember
{
public:
	short		sprite_recno;			// inside a battle, the unitRecno created in unit_array
	short		unit_id;
	char		rank_id;
	char		race_id;
	int		is_human()		{ return race_id>0; }
	int		is_monster()	{ return race_id<0; }
	int		monster_id()	{ return -race_id;  }
	WORD		name_id;          // id. of the unit's name in RaceRes::first_name_array;
	short		hero_id;                                // >0 if this is a hero
	long		unique_id;              // unique number generated across campaign

	char		loyalty;
	char		target_loyalty;
	Skill		skill;
	short		nation_contribution;    // contribution to the nation
	short		total_reward;           // total amount of reward you have given to the unit

	Item		item;

	// spy attribute
	char		is_spy;
	char		spy_skill;
	char		spy_loyalty;			// the spy's loyalty to his true home nation
	char		cloaked_nation_recno;

public:
	int  		combat_level() 	{ return skill.actual_combat_level(&item); }
	int  		skill_level()		{ return skill.actual_skill_level(&item); }
	int  		max_hit_points()	{ return skill.actual_max_hit_points(&item); }
	char*		unit_name(int nationRecno, int withTitle=1, int firstNameOnly=0);

	void		update_unit(Unit *, Spy *);
	void		update_unit(Soldier *, Spy *, int firmRecno);
	int		create_unit(int nationRecno, int xLoc, int yLoc );
};

//----------- define class CampaignNationRelation --------//

class CampaignNationRelation
{
public:
	char	should_attack;			// whether units should automatically attack units/firms of this nation when the relationship is hostile
	char	trade_treaty;			// whether allow trading with this nation
	char  status;
};

//----------- define class CampaignNation --------//

class CampaignNation
{
public:
	enum { CUSTOM_NAME_LEN=50 };

	char		campaign_nation_recno;
	char		nation_type;

	char		custom_name[CUSTOM_NAME_LEN+1];	 // if custom_name[0] is not NULL, use custom name instead of the normal name

	int 		king_name_id;
   char* 	king_name(); 
	char* 	nation_name()	{ return custom_name; }

	char		race_id;
	int		is_human()		{ return race_id>0; }
	int		is_monster()	{ return race_id<0; }
	int		monster_id()	{ return -race_id;  }

	char* 	race_name();

	char		color_scheme_id;
	char		nation_color;				// main color of the nation, based on from color_scheme_id

	WORD		minister_name_array[MINISTER_COUNT];
	char*		minister_name(int ministerId);

	CampaignMember	king;
	CampaignMember noble[MAX_NOBLE+MAX_ROYAL_UNIT];		// reserve space for royal added in battle
	int		noble_count;

	char		know_base_array[MAX_RACE];		// whether the unit knows how to constructure seat of power or not

	char     tech_level_array[MAX_TECH];			// each nation's the technology level of this unit,
	char	   research_progress_array[MAX_TECH];

	CampaignNationRelation relation_array[MAX_NATION];				// inter-relationship with other nations

	int		get_relation_status(int nationRecno)		{ return relation_array[nationRecno-1].status; }

public:
	void		init();
	void		init_nation(int nationType, int raceId, int colorSchemeId);
	void		deinit();

	void		set_custom_name(char* customName);
};

//----------- define class Campaign ----------//

class File;
class CampaignAnimationUnit;

class Campaign : public Battle
{
public:
	enum	{ CAMPAIGN_CODE_LEN=8, DIALOG_CODE_LEN=8, TEXT_CODE_LEN=8 };
	enum  { CUSTOM_NAME_LEN=50 };

	char		init_flag;
	int		campaign_id;
	char		campaign_code[CAMPAIGN_CODE_LEN+1];

	CampaignNation	nation[MAX_NATION];

	char*		text_buf;
	short		text_buf_size;
	char*		text_buf2;
	short		text_buf2_size;
	char		auto_test_flag;
	char		replay_stage_flag;		// when this is set to 1, the stage game will be restarted.
	char		cheat_to_win_flag;
	char		keep_plot_flag;

	char		old_king_name[CUSTOM_NAME_LEN+1];

	//--------------------------------//

	int  		game_year;				// starting game date of next scenario
	int  		game_month;
	int  		game_day;

	short		stage_id;
	short	 	event_id;
	short		return_event_id;

	char		plot_category;
	short		plot_id;

	short		max_stage;
	short		max_event;

	int		campaign_score;
	char 		mountain_layout[MOUNTAIN_LAYOUT_X_COUNT*MOUNTAIN_LAYOUT_Y_COUNT];
	char		mountain_layout_enabled;

	long		saved_random_seed;

	//---- how many times a stage game or an event has been run ----//

	int 		stage_run_count_array[MAX_STAGE];
	int		event_run_count_array[MAX_EVENT];

	char		total_stage_run_count;
	char		total_event_run_count;

	//------ campaign game setting ------//

	char		campaign_difficulty;					// the true campaign difficulty that scenarios are generated based on.

	//------- campaign game info -------//

	short		attacker_state_recno;
	short		attacker2_state_recno;		// this is used when there are more than one attacking nation involved in a battle
	short		target_state_recno;

	//------- campaign game vars -------//

	short		rebel_count;
	int  		next_rebel_date;

	short		general_defect_count;
	int  		next_general_defect_date;
	int		monster_reinforcement_count;
	int		next_monster_reinforcement_date;
	short		win_to_unveil_tech_id;

	//--- vars for surrendered Fryhtans ---//

	char		can_use_fryhtan;
	char		surrendered_fryhtan_id;			// the monster id. of the surrendered fryhtans
	char		this_battle_use_fryhtan;		// whether the player wants to use Fryhtans in this battle

	//-------- random terrain map -------//

	Plasma	random_terrain_map;
//	short		shadow_array[MAIN_TERRAIN_WIDTH * MAIN_TERRAIN_HEIGHT];
	short*		shadow_array;	
	//-------- campaign situations -------//

	char 					 has_situation;
	CampaignSituation  cur_situation;
	CampaignSituation  campaign_situation_array[CAMPAIGN_SITUATION_COUNT];

	//------- resource files -----------//

	static ResourceIdx res_stage, res_event, res_plot;

private:
	unsigned long	random_seed;

public:
	Campaign();
	~Campaign();

	void				init(int campaignId);
	void				deinit();

	void				init_new_campaign();
	void				main_loop(int isLoadedGame);

	virtual void	next_day()		{;}

	void 				disp_in_game_msg(char* gameMsg, ...);
	virtual void 	disp_end_game_msg()		{;}
	void		disp_end_game_bonus(int maxYear, int yearPassed);

	virtual char*	intro_text();
	virtual char*	plot_text();
	virtual char*	goal_text();
	char*				full_goal_text();

	char*				king_name() 	{ return get_nation(CAMPAIGN_PLAYER_NATION_RECNO)->king_name(); }

	char*				minister_name(int ministerId)
						{ return get_nation(CAMPAIGN_PLAYER_NATION_RECNO)->minister_name(ministerId); }

	unsigned long	random(unsigned long);

	int				is_nation_deleted(int nationRecno);
	CampaignNation	*get_nation(int nationRecno);

	void 				reset_mountain_layout();
	void 				set_on_mountain_layout(int x, int y, int totalX=0, int totalY=0);

	int				smoothing_checking(int xr, int yr, int bitShift);
	void				render_terrain(int par);
	void				calculate_terrain_location(int &xLoc, int &yLoc);
	void				render_attackable_terrain(int par, char *selectableStateArray, int selectableStateCount);

	void				set_campaign_difficulty(int campaignDifficulty);
	void 				init_nation_resource(int nationRecno, int resourceLevel);

	void				unveil_new_tech();

	void				play_speech(const char* fileName);
	void				stop_speech();

	// -------- save game function ---------//

	virtual int		write_derived_file(File *filePtr) { return 1; }
	virtual int		read_derived_file(File *filePtr) { return 1; }

	// -------- text display function ---------//

	static void		put_center_text(int x, int y, const char* str, char black, Font* fontPtr, int darkness = 1);

protected:
	virtual void	init_new_campaign_derived()		{;}
			  void 	init_situation();
			  void	set_cur_situation(int situationId);
			  void	set_std_situation();

			  void 	set_event(int eventId);
			  void	set_stage(int stageId);

			  int		has_event_run(int eventId);
			  int		has_stage_run(int stageId);

			  int 	event_run_count(int eventId) { return has_event_run(eventId); }		// has_event_run() actually return the number of times the event has been run
			  int 	stage_run_count(int stageId) { return has_stage_run(stageId); }

			  int 	run_event_once(int eventId);
			  int 	run_stage_once(int stageId);

			  int 	detect_cheat_key();

	//----- functions for tech availability -----//

	int 		unveil_one_tech(short* techClassArray, int techClassCount);
	int 		unveil_spu_tech();

	//----- functions for generating random numbers -----//

	double			randomf(double);
	unsigned long	rand();

	//--------- functions on nation --------------//

	int		nation_count()		{ return MAX_NATION; }
	int		packed_nation_count();
	int		new_nation();
	virtual void	del_nation(int nationRecno);

	int  		add_nation(int nationType, int raceId=0, int colorSchemeId=0);
	int 		random_unused_race(bool isMonster);
	int 		random_unused_color();

	void 		set_king(int nationRecno, int kingUnitRecno, int firstKing);
	void		update_royal_unit();
	void		select_royal_menu(CampaignMember *, CampaignMember *, int& count);
	void		pick_royal_to_battle();
	void		unpick_royal_to_battle();               // call on restart stage

	void 		catch_nation_info();
	void 		fetch_nation_info();

	int		get_relation_status(int nationRecno)
				{ return get_nation(CAMPAIGN_PLAYER_NATION_RECNO)->get_relation_status(nationRecno); }

	void		set_relation_status(int nationRecno1, int nationRecno2, int newStatus);

	//------- state functions ----------//
																// update game_date from info; after game finish
	void 		attack_state(int attackerStateRecno, int targetStateRecno, int attackResult, int firstStep=0, int lastStep=0, int attacker2StateRecno=0);
	void		flash_state(int stateRecno)	{;}
	void 		rebel_attack_state(int stateRecno, int attackResult, int firstStep, int lastStep);

	void 		attack_animation(int attackerUnitId, int targetUnitId,
					int attackerColorSchemeId, int targetColorSchemeId,
					int srcScrnX, int srcScrnY, int destScrnX, int destScrnY, int attackResult, int firstStep, int lastStep);

	void 		attack_animation( CampaignAnimationUnit *attackerArray,
					int attackerCount, CampaignAnimationUnit *defenderUnit,
					int firstStep, int lastStep );

	void 		state_change_nation(int stateRecno, int newNationRecno);
	int 		temp_state_change_nation(int stateRecno, int newNationRecno);
	int 		random_pick_attack_state(int attackNationRecno, int targetNationRecno, int excludedStateRecno=0);
	int 		random_pick_state(int nationRecno, int neighborStateRecno=0);
	int 		random_pick_monster_campaign_nation(int excludedStateRecno=0, int nextToNationRecno=CAMPAIGN_PLAYER_NATION_RECNO, int mustNextTo=0);

	int		select_attackable_state(char *selectableStateArray, int selectableStateCount);

	//---- function for processing the result of the last game ----//

	virtual void stage_prelude()				{;}
	virtual int  process_event()				{return 0;}
	virtual void process_game_result()		{;}

	//------------ display functions -------------//

	void		disp_state_map(int dispX1, int dispY1, int dispWidth, int dispHeight, int par);
	void 		disp_intro();
	void 		disp_intro_text();
	void		disp_monster_defeated();
	void 		disp_final_victory();
	void 		disp_strategic_screen(int shouldBltBuf=1, int terrainMapOnly = 0);
	void 		disp_nation_info();
	char* 	substitute_text(char* dialogText, ...);
	void 		disp_letter(char isMonster= -1, char* dialogText=NULL, ...);
	int		detect_letter();
	void 		disp_narrative(char* textCode, ...);
	void 		disp_dialog(int raceId, char* textCode, int refreshFlag);
	void 		disp_text(int boxX1, int boxY1, int boxX2, int boxY2, char* dialogText, int animationRaceId=0);
	int 		detect_reply();
	void 		play_speech_animation(int raceId);
			
	//-------- functions for creating scenarios --------//

	void 		init_player_resource(int resourceLevel=50);
//	void		random_init_human_tech(int nationRecno);;
//	void		random_init_monster_tech(int monsterNationRecno, int ownTechChance, int otherTechChance);
	void 		make_should_build_array(char* arrayPtr, int arraySize, int buildLevel);

	void 		create_raw_site();
	int 		create_nation_town(int nationRecno, int xLoc, int yLoc, int buildFort, int sameRaceTown=1, int soldierAddCount=100, int combatLevel=0);
	int 		create_independent_town(int xLoc, int yLoc, int raceId, int initPop, int initResistance);
	int 		create_monster_lair(int xLoc, int yLoc, int nationRecno, int soldierCount, int avgCombatLevel, int monsterId=0);
	int  		create_monster_special_firm(int firmRecno);
	int 		create_slave_town(int firmRecno, int raceId);
	int 		create_economic_firm(int townRecno);
	int 		create_military_firm(int townRecno, int powerRating=100);
	int 		create_unit_group(int nationRecno, int addCount, int unitId=0, int adjustCount=0, int whichEdge=0, int xLoc= -1, int yLoc= -1);
	void		create_royal_units(int campaignNationRecno);

	void		random_edge_location(int& xLoc, int& yLoc, int whichEdge=0);
	int 		random_pick_town_with_camp(int nationRecno, int maxLink);
	int 		king_oversee_town_recno(int nationRecno);
	int   	create_work_firm(Place* placePtr, int firmId);

	int 		most_state_nation(int raceType, int nextToCampaignNation=0);
	int 		least_state_nation(int raceType, int nextToCampaignNation=0);
	void 		set_nation_hostile(int nationRecno1, int nationRecno2);
	void 		set_nation_allied(int nationRecno1, int nationRecno2);
	void 		create_troop(int nationRecno, int totalUnit, int avgCombatLevel=0, int unitId=0, char useLastLocation=0);
	void 		create_troop2(int nationRecno, int totalUnit, int avgCombatLevel=0, int unitId=0, char useLastLocation=0);
	void		create_wagon(int nationRecno, int totalPop, char useLastLocation);
	int 		create_lair(int campaignNationRecno, int nationRecno, int lairAddCount, int independentTownAddCount, int specialFirmCount, int soldierCombatLevel=0);
	int 		create_town(int nationRecno, int townAddCount, int hasFortPercent, int combatLevel=0);
	void 		create_offensive_structure(int nationRecno, int buildCount);
	void 		create_offensive_structure(int townRecno);
	int 		create_rebel_nation(int townCount, int hasFortPercent, int militaryStrength);

	int 		add_weapon_to_camp(int campFirmRecno);

	//-------- functions for processing scenarios -------//

	int 		should_general_defect(int intervalDays);
	int 		should_rebel(int intervalDays);

	void 		capture_linked_place(int firmRecno);
	int  		general_defect_form_new_nation(int nationRecno);
	int 		general_defect_to_other_nation(int fromNationRecno, int toNationRecno);;
	void 		rebel_form_nation();
	int 		town_defect(int nationRecno, int defectToNationRecno=0);
	void 		monster_reinforcement_to_lair(int whichEdge=0);
	void 		monster_reinforcement(int nationRecno, int whichEdge=0);

	void 		think_enemy_attack(int monsterNationRecno, int targetNationRecno, int attackFrequency=50);
	void 		think_enemy_attack2(int targetNationRecno, Unit* unitPtr);
};

//--------------------------------------------//

#endif
