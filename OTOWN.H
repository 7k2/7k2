//Filename    : OTOWN.H
//Description : Header file of Object Town

#ifndef __OTOWN_H
#define __OTOWN_H

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

#ifndef __OPLACE_H
#include <OPLACE.H>
#endif

#ifndef __OSKILL_H
#include <OSKILL.H>
#endif

#ifndef __OFIRMID_H
#include <OFIRMID.H>
#endif

#ifndef __ORAWRES_H
#include <ORAWRES.H>
#endif

#ifndef __OTOWNRES_H
#include <OTOWNRES.H>
#endif

//------- define constant --------//

#define MAX_TOWN_GROWTH_POPULATION  100		// grow up to 60 units in a town
#define MAX_TOWN_POPULATION  			100		// max number of unitw in a town

//-------- Define constant -----------//

#define STD_TOWN_LOC_WIDTH       6
#define STD_TOWN_LOC_HEIGHT      6

#define MIN_RECRUIT_LOYALTY			   30 	// only when loyalty > this, the unit can be recruited
#define MIN_NATION_DEFEND_LOYALTY		30    // if the loyalty of the town people > this, they will defense against the attack
#define MIN_INDEPENDENT_DEFEND_LOYALTY 30
#define SURRENDER_LOYALTY				   30	   // when the loyalty of the town is <= this, and being attacked by a nation, the town will surrender
#define SURRENDER_RESISTANCE				30
#define REBEL_LOYALTY					   29    // if loyalty <= REBEL_LOYALTY, the unit may rebel

#define INDEPENDENT_LINK_RESISTANCE  	50	 	// only if the resistance of an independent town is lower than this, the town will enable its link with a foreign firm

#define TAX_PER_UNIT           	  		3	 	// amount of tax can be collected from each unit
#define COLLECT_TAX_LOYALTY_DECREASE 	10	 	// Reduce 10 loyalty points per tax collection

#define TOWN_REWARD_PER_UNIT		   	10
#define TOWN_REWARD_LOYALTY_INCREASE   10

#define IND_TOWN_GRANT_PER_UNIT			   	30		// independent town granting cost per unit
#define IND_TOWN_GRANT_RESISTANCE_DECREASE   10		// resistance decrease per grant

#define RECEIVED_HIT_PER_KILL			 	200	 // no. of received hits will result in one death of town people

#define MAX_TOWN_TARGETS					6
#define TOWN_TARGET_RANGE					6

// ------- define constant on wall -------//

#define MAX_WALL_LEVEL           2
#define MAX_WALL_PROGRESS     1000
#define LEVEL_ONE_WALL_COST	 500
#define LEVEL_TWO_WALL_COST	1000

//-------- Define class Town ----------//

class  Unit;
class  FirmWork;
class  FirmCamp;
struct InnUnit;

class Town : public Place
{
public:

	//------- basic vars --------//

	enum { TOWN_NAME_LEN=20 };

	short	town_recno;

	short	town_name_id;
//	char  town_name[TOWN_NAME_LEN+1];		// the user will be able to change the town name

	char	is_base_town;		// whether this town is base town or not
	char	construction_completion_percent;

	//------ town layout vars -------//

	short layout_id;           // town layout id.
	short first_slot_id;       // the first slot id. of the layout

	short slot_object_id_array[MAX_TOWN_LAYOUT_SLOT];  // the race id. of each slot building
	short max_layout_pop;		// maximum number of people the current layout supports

	//------ primary town game vars ------//

	char* town_name();

	short population;
	short jobless_population;

	short worker_population()		{ return population-jobless_population; }
	int	recruitable_pop(int recruitSpy);

	short population_growth;

	float loyalty;
	short	target_loyalty;
   short bonus_loyalty;

	float	resistance_array[MAX_NATION];
	short	target_resistance_array[MAX_NATION];

#ifdef DEBUG
	float	resistance(int nationRecno);
	short	target_resistance(int nationRecno);
#else
	float	resistance(int nationRecno)						{ return resistance_array[nationRecno-1]; }
	short	target_resistance(int nationRecno)				{ return target_resistance_array[nationRecno-1]; }
#endif

	short quality_of_life;

	//------- secondary game vars -------//

	short	spy_count;
	short	town_defender_count;			// no. of units currently defending this town
	float received_hit_count;		// no. of received hit by attackers, when this > RECEIVED_HIT_PER_KILL, a town people will be killed

	int 	possible_defender_combat_level(int attackerNationRecno);

	// ------ wall ---------- //

	char	current_wall_level;		// 0, 1 or 2
	char	target_wall_level;		// 0, 1 or 2
	char	target_wall_man_power;
	short upgrade_wall_progress;	// 0 to 10000

	// ------- archer vars -------//

	short	archers_energy;
	float	arrow_damage;
	short	target_count;
	short	target_base_obj_recno[MAX_TOWN_TARGETS];

	//-------- rebel vars -------//

	short rebel_recno;		// whether this town is controlled by a rebel
	int	last_rebel_date;

	//-------- penalty vars ----------//

	int   accumulated_collect_tax_penalty;
	int   accumulated_reward_penalty;
	int   accumulated_recruit_penalty;
	int   accumulated_enemy_grant_penalty;

	//----- independent town/unit vars -----//

	char	independ_town_nation_relation; // each bit n is high representing this independent town will attack nation n.
	short independent_unit_join_nation_min_rating;

	//------- auto policy -------------//

	short	auto_collect_tax_loyalty;		// auto collect tax if the loyalty reaches this level
	short	auto_grant_loyalty;				// auto grant if the loyalty drop below this level

	//------ inter-relationship -------//

	short  linked_firm_count;
	short  linked_town_count;

	short  linked_firm_array[MAX_LINKED_FIRM_TOWN];
	short  linked_town_array[MAX_LINKED_TOWN_TOWN];

	char   linked_firm_enable_array[MAX_LINKED_FIRM_TOWN];
	char   linked_town_enable_array[MAX_LINKED_TOWN_TOWN];

	int 	 closest_own_camp();

	char	 has_linked_own_camp;					 // whether the town has linked military camps of the same nation
	char	 has_linked_enemy_camp;				 // whether the town has linked military camps of the same nation

	//----------- AI vars ------------//

	char  ai_town;
	char	ai_link_checked; // AI check firm and town locatin by links, disable checking by setting this parameter to 1

	char	town_combat_level;						// combat level of the people in this town
	char	has_product_supply[MAX_PRODUCT];		// whether this town has the supply of these products

	//-------- casting function for BaseObj ---------//

	Town*	cast_to_Town()	{ return this; }

	virtual int obj_power() { return	possible_defender_combat_level(0); }

	//------ static class member var ------//

	static short  if_town_recno;

public:

	//----- init functions ------//

	Town();

	void 	init(int nationRecno, int raceId, int xLoc, int yLoc, int builderRecno=0);
	void  deinit();

	//------ process functions ------//

	void	process();
	void  next_day();
	void  next_month();

	//------ drawing functions -----//

	void  draw(int displayLayer=1);

	//----- interface functions ------//

	void  right_click(int xLoc, int yLoc);

	void  disp_info(int refreshFlag);
	void  detect_info();
	int	browse_selected_race_id();
	void 	disp_auto_loyalty();

	virtual int		disp_edit_mode(int refreshFlag);
	virtual void	detect_edit_mode();

	//----- functions for updating vars ------//

	void  update_target_loyalty();
	void  update_target_resistance();
	void  update_loyalty();
	void  update_resistance();
	void	update_product_supply();
	void	change_loyalty(float loyaltyChange);
	void	change_resistance(int nationRecno, float loyaltyChange);
	void	update_quality_of_life();

	//------ unit functions --------//

	void  assign_unit(int unitRecno);
	int   recruit(bool recruitWagon, char remoteAction);
	int  	recruit_dec_loyalty(int recruitCount, int decNow=1);

	int	can_recruit_peasant();
	int	can_train();
	int   can_migrate(int destTownRecno, int migrateNow=0);		 // if not called by Town::mirgate, don't set migrateNow to TRUE
	void	move_pop(Town* destTown);

	int 	create_rebel_unit(int isLeader);
	int   mobilize_town_people(int decPop, int mobilizeSpy);
	int	mobilize_defender(int attackerNationRecno);
	int   mobilize_wagon(int decPop, int wagonPopulation, short spyArrayCount, short *spyArray);
	void	reduce_defender_count();

	int	migrate_to(int destTownRecno, char remoteAction);

	//------- layout functions ------//

	void	auto_set_layout();
	int	think_layout_id();

	//------- misc functions --------//

	int  	form_new_nation();
	int 	camp_influence(FirmCamp* firmCamp);
	int 	camp_influence(Unit* unitPtr);
	int 	camp_influence(InnUnit* innUnit, int nationRecno);
	int 	camp_influence(int nationRecno, int raceId, int skillLevel);

	int 	is_pay_tribute_to_monster(int nationRecno=0);
	void	pay_tribute_to_monster();
	int 	total_tribute_to_monster(int nationRecno);
	float tribute_to_lair(int firmRecno, int lairCount);

	//----- link functions -------//

	void  setup_link();
	void  release_link();
	void  release_firm_link(int);
	void  release_town_link(int);
	int	linked_camp_count(bool hasOverseer);
	int 	can_toggle_firm_link(int firmRecno);
	void 	update_camp_link();
	int 	has_linked_camp(int nationRecno, int needOverseer);
	void  toggle_firm_link(int linkId, int toggleFlag, char remoteAction, int setBoth=0);
	void  toggle_town_link(int linkId, int toggleFlag, char remoteAction, int setBoth=0);

	char	get_linked_firm_status(int firmRecno);
	char	get_linked_town_status(int townRecno);


	//----- construction functions ------//

	void 	process_construction();
	void 	complete_construction();
	int 	builder_stay_after_construction();

	//------ population functions ------//

	void  init_pop(int addPop, int loyalty, int hasJob=0, int firstInit=0);
	void  inc_pop(int unitLoyalty);
	void  dec_pop();

	void 	unjob_town_people();
	void	kill_town_people(int attackerUnitRecno=0);

	//-------- drawing functions ---------//

	void  draw_selected();
	int   draw_detect_link_line(int);
	int   is_in_zoom_win();

	//------ nation related functions -------//

	void  set_nation(int nationRecno);
	void	set_race(int raceId);
	void 	set_nation_unjob_worker();
	void 	surrender(int toNationRecno);
	void	change_nation(int newNationRecno) { set_nation(newNationRecno); }	// BaseObj func

	//------ collect tax/grant functions -----//

	void	collect_yearly_tax();
	void  collect_tax(char remoteAction);
	void  reward(char remoteAction);

	void 	set_auto_collect_tax_loyalty(int loyaltyLevel);
	void 	set_auto_grant_loyalty(int loyaltyLevel);

	int 	can_grant_to_non_own_town(int grantNationRecno);
	int 	grant_to_non_own_town(int grantNationRecno, int remoteAction);

	// ------ wall functions ----------- //

	int	process_wall();		// upgrade or repair wall
	void	set_repair_wall(int repairFlag, char remoteAction);
	void 	set_current_wall_level(int newLevel);
	void	set_target_wall_level(int newLevel, char remoteAction);

	// ------ bullet functions -------//

	int		return_fire(BaseObj* targetObj);
	void		process_attack_target();
	void		process_scan_target();

	float		bullet_damage();
	short		bullet_radius();
	char		bullet_fire();
	short		bullet_id();
	short		bullet_speed();
	short		bullet_init_z();

	//-------- misc functions --------//

	void  distribute_demand();

	int	player_spy_count();
//	int	has_player_spy();
	int	explore_for_player();

	//----- derived function from BaseObj -----//

	virtual void 	being_attack_hit(BaseObj* attackerObj, float damagePoint);
	virtual bool	can_accept_assign(int unitRecno, int actionNationRecno= -1);
	virtual void	left_click();

	//------- file functions ---------//

	int   write_file(File*);
	int   read_file(File*);

	//-------- ai functions ---------//

	void  process_ai();
	void	think_collect_tax();
	void	think_reward();
	int	think_build_firm(int firmId, int maxFirm);
	int	think_build_market();
	int	think_build_fort();
	int 	think_build_research();
	int 	think_build_spy();
	int 	think_build_special();
	int 	think_build_war_factory();
	int 	think_build_base();
	int 	think_build_inn();
	int	think_build_wall();
	int 	think_ai_migrate();
	int 	think_ai_migrate_to_town();
	void	think_defense();
	int 	think_split_town();
	void	think_move_between_town();
	int 	think_attack_nearby_enemy();
	int 	think_attack_linked_enemy();
	int 	think_capture_enemy_town();
	int   think_scout();

	void 	update_base_town_status();
	int	new_base_town_status();

	int 	needed_anti_spy_level();

	int	should_ai_migrate();
	int   detect_enemy(int);

	int	protection_needed();			// an index from 0 to 100 indicating the military protection needed for this town
	int	protection_available();

	int 	ai_build_neighbor_firm(int firmId, int firmRaceId=0);
	int 	ai_settle_new(int raceId);

	//-------- independent town ai functions ---------//

	void	think_independent_town();
	void 	think_independent_set_link();
	int 	think_independent_build_wall();
	int  	think_independent_form_new_nation();
	int  	can_independent_form_new_nation();
	int  	think_independent_unit_join_nation();
	int  	independent_unit_join_nation(int toTownRecno);

	//--------- function for cheat key ----------//

	int	get_selected_race();

	//--------- function for changing town name ----------//

	int	input_town_name();
	
	//-------------- multiplayer checking codes ---------------//
	UCHAR crc8();
	void	clear_ptr();

	//-------------------------------//

private:

	//----- functions for init & deinit ----//

	void	init_resistance();
	void  set_world_matrix();
	void  restore_world_matrix();
	void 	establish_contact_with_player();

	//------- process functions ------//

	void 	process_food(int busyPeasant);
	void	process_auto();

	void  population_grow();

	//------- think functions ----------//

	void  think_migrate();
	int 	think_migrate_one(Town* targetTown, int townDistance);
	void  think_rebel();
	int	think_surrender();

	//------ town people functions ------//

	void 	migrate(int destTownRecno, int newLoyalty);
	void 	evenly_distribute_worker();
	int	transfer_worker(FirmWork* srcFirmWork, FirmWork* destFirmWork);

	//----- functions for updating vars -----//

	void  daily_update_loyalty();
	void  calc_loyalty();

	//------- drawing functions ---------//

	void  draw_flag(int,int);
	void  draw_farm(int,int,int,short*);

	//------ interface functions ---------//

//	void  disp_basic_info(int refreshFlag);
	void	disp_train_info(int refreshFlag);

	void  disp_main_menu(int refreshFlag);
	void  detect_main_menu();
	void 	disp_debug_resistance(int refreshFlag);

	void  disp_train_menu(int refreshFlag);
	void	disp_train_button(int y, int skillId, int buttonUp);
	void	disp_queue_button(int y, int skillId, int buttonUp);
	void  detect_train_menu();

	void  disp_auto_menu(int modeCollectTax, int refreshFlag);
	void  detect_auto_menu(int modeCollectTax);

	void  disp_man_power(int refreshFlag);
	void  detect_man_power();

	void  disp_spy_menu(int refreshFlag);
	void  detect_spy_menu();
};

//-------- Begin of class TownArray ------------//

class TownArray : public DynArrayB
{
public:
	int   selected_recno;      // the firm current being selected
	int	race_wander_pop_array[MAX_RACE];		// no. of wandering people of each race. They are people for setting up independent towns later

public:
	TownArray();
	~TownArray();

	void  init();
   void  deinit();

	//------ add/delete functions -------//

	int	generate_town(int nationRecno, int raceId, int xLoc, int yLoc);
	int   add_town(int nationRecno, int raceId, int xLoc, int yLoc, int builderRecno=0, bool noCost=false);
	void  del_town(int townRecno);
	Town* create_town();

	//------- drawing functions --------//

	void  draw();
	void  draw_dot(int filterRaceId=0);
	void	draw_profile();

	void	process();
	void	next_month();

	//--------- misc functions ---------//

	int	independent_town_resistance();
	void	think_new_independent_town();

	int 	think_town_loc(int maxTries, int& retXLoc, int& retYLoc, int rangeX1= -1, int rangeY1= -1, int rangeX2= -1, int rangeY2= -1);
	int   find_nearest_town(int xLoc, int yLoc, int nationRecno=0);

	int   build_town(int unitRecno, int xLoc, int yLoc);
	void  distribute_demand();

	void	stop_attack_nation(short nationRecno);
	void 	update_all_camp_link();
	void 	set_all_loyalty_to_target();

	//------- file functions --------//

	int   write_file(File*);
	int   read_file(File*);

	//------- array functions --------//

	int   is_deleted(int recNo);

   #ifdef DEBUG
		Town* operator[](int recNo);
	#else
		Town* operator[](int recNo)  { return (Town*) get_ptr(recNo); }
	#endif
};

extern TownArray town_array;

//---------------------------------------------------//

#endif
