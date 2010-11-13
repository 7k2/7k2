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

//Filename    : ONATION.H
//Description : Header of class Nation

#ifndef __ONATION_H
#define __ONATION_H

#ifndef __ONATIONB_H
#include <ONATIONB.H>
#endif

#ifndef __OUNIT_H
#include <OUNIT.H>
#endif

#ifndef __OTOWN_H
#include <OTOWN.H>
#endif

//--------- AI maximum constants -------//

#define  MAX_AI_REGION				20

#define	AI_TOWN_INIT_SIZE    	60
#define	AI_BASE_TOWN_INIT_SIZE  60
#define	AI_BASE_INIT_SIZE			20
#define	AI_CAMP_INIT_SIZE			60
#define  AI_MINE_INIT_SIZE			10
#define	AI_FACTORY_INIT_SIZE	 	50
#define	AI_MARKET_INIT_SIZE		50
#define  AI_RESEARCH_INIT_SIZE	30
#define  AI_WAR_INIT_SIZE			30
#define	AI_INN_INIT_SIZE			10
#define	AI_SPY_INIT_SIZE			10
#define	AI_SPECIAL_INIT_SIZE		10
#define	AI_OFFEND_INIT_SIZE		10
#define	AI_LISHORR_INIT_SIZE    10
#define	AI_MONS_TRAIN_INIT_SIZE 10
#define	AI_MONS_ALCHEMY_INIT_SIZE	10
#define	AI_MONS_FORT_INIT_SIZE	10
#define	AI_MONS_ANIMAL_INIT_SIZE  10
#define	AI_INCUBATOR_INIT_SIZE  10
#define	AI_MONS_MAGIC_INIT_SIZE 10
#define	AI_MONSTER_INIT_SIZE		10
#define  AI_GENERAL_INIT_SIZE	 	50
#define  AI_CARAVAN_INIT_SIZE		100
#define  AI_SHIP_INIT_SIZE			50

#define	AI_TOWN_INC_SIZE    		30
#define	AI_BASE_TOWN_INC_SIZE   30
#define	AI_BASE_INC_SIZE			10
#define	AI_CAMP_INC_SIZE			10
#define  AI_MINE_INC_SIZE			10
#define	AI_FACTORY_INC_SIZE	 	10
#define	AI_MARKET_INC_SIZE		10
#define	AI_INN_INC_SIZE			10
#define  AI_RESEARCH_INC_SIZE		10
#define  AI_WAR_INC_SIZE			10
#define  AI_SPY_INC_SIZE			10
#define  AI_SPECIAL_INC_SIZE		10
#define	AI_OFFEND_INC_SIZE		10
#define	AI_LISHORR_INC_SIZE     10
#define	AI_MONS_TRAIN_INC_SIZE  10
#define	AI_MONS_ALCHEMY_INC_SIZE	10
#define	AI_MONS_FORT_INC_SIZE	10
#define  AI_MONS_ANIMAL_INC_SIZE 10
#define  AI_INCUBATOR_INC_SIZE   10
#define  AI_MONS_MAGIC_INC_SIZE  10
#define  AI_MONSTER_INC_SIZE		10
#define  AI_GENERAL_INC_SIZE	 	20
#define  AI_CARAVAN_INC_SIZE		30
#define  AI_SHIP_INC_SIZE			20

//----------- AI parameters ----------//

enum	{	STD_ACTION_RETRY_COUNT = 4,		// retry this number of times before giving up
			MAX_SCORE = 600,

			MIGRATE_DEMAND_SUPPLY_DIFF=25, // suppy > demand + MIGRATE_DEMAND_SUPPLY_DIFF
			MIGRATE_STOCK_QTY=150,

			TRADE_STOCK_QTY=125,
			MAX_TRADE_MARKET = 4,

			MAX_BASE_TOWN = 10,
		};

//-------- AI action constants ---------//

enum  {  ACTION_DYNAMIC,     		// for ActionNode::action_type
			ACTION_FIXED		};

enum	{	ACTION_AI_BUILD_FIRM=1,		// define ActionNode action_type
			ACTION_AI_ASSIGN_OVERSEER,
			ACTION_AI_ASSIGN_SOLDIER,
			ACTION_AI_ASSIGN_SPY,
			ACTION_AI_SCOUT,
			ACTION_AI_BUILD_TOWN,
			ACTION_AI_SETTLE_TOWN,
			ACTION_AI_PROCESS_TALK_MSG,
		};

//--------- define AIRegion ---------//

struct AIRegion
{
	BYTE	region_id;
	char  town_count;
	char  base_town_count;
};

//-------- define ActionNode structure -------//

struct ActionNode
{
	enum { MAX_ACTION_GROUP_UNIT = 9 };

	char 	action_mode;		// eg build firm, attack, etc
	char	action_type;
	short action_para;		// parameter for the action. e.g. firmId for AI_BUILD_FIRM
	short action_para2;		// parameter for the action. e.g. firm race id. for building FirmBase
	WORD	action_id;			// an unique id. for identifying this node

	long 	add_date;			// when this action is added
	short	unit_recno;

	short	action_x_loc;		// can be firm loc, or target loc, etc
	short	action_y_loc;
	short	ref_x_loc;			// reference x loc, eg the raw material location
	short	ref_y_loc;

	char	retry_count;	 				// number of term to wait before this action is removed from the array if it cannot be processed
	char	instance_count; 				// no. of times this action needs to be carried out

	short group_unit_array[MAX_ACTION_GROUP_UNIT]; 		// for group unit actions, the no. of units in the array is stored in instance_count

	char  processing_instance_count;
	char  processed_instance_count;

	long  next_retry_date; 				// continue processing this action after this date, this is used when training a unit for construction
};

//------- Define struct AttackCamp --------//

#define MAX_SUITABLE_ATTACK_CAMP    30    // total no. of useful camps
#define MAX_AI_ATTACK_TARGET			10		// attacking more than one target in an attack mission

struct AttackCamp
{
	short firm_recno;
	short combat_level;
	short distance;
	int   patrol_date;
};

//--------- Define class Nation ---------//

class  Firm;
class  Town;
class  Spy;
struct TalkMsg;

class Nation : public NationBase
{
public:
	short			nation_class_id;		// its values are defined in ONATION2.H

	DynArrayB	action_array;
	WORD			last_action_id; 	// a 16-bit id. for identifying ActionNode

public:
	Nation();
	~Nation();

	//------------------------------------------------------//
	// array used to store the the waiting and procesing actions
	//------------------------------------------------------//

	int			action_count() 			{ return action_array.size(); }
	ActionNode* get_action(int recNo);
	ActionNode* get_action_based_on_id(int actionId);
	int			is_action_deleted(int actionRecno);

	//------------------------------------------------------//
	// array used to store the info. of the firms
	//------------------------------------------------------//
	short*		ai_town_array;
	short* 		ai_base_array;
	short* 		ai_camp_array;
	short* 		ai_mine_array;
	short*		ai_factory_array;
	short*      ai_market_array;
	short*		ai_research_array;
	short*		ai_war_array;
	short*		ai_inn_array;
	short*		ai_spy_array;
	short*		ai_special_array;
	short*		ai_offend_array;
	short*		ai_lishorr_array;
	short*		ai_mons_train_array;
	short*		ai_mons_alchemy_array;
	short*		ai_mons_fort_array;
	short*      ai_mons_animal_array;
	short*      ai_incubator_array;
	short*      ai_mons_magic_array;
	short*		ai_monster_array;
	short*    	ai_general_array;
	short*		ai_caravan_array;
	short*		ai_ship_array;

	//--------------------------------------------------------//
	// parameters used to make decisions
	//--------------------------------------------------------//
	short			ai_town_size;
	short			ai_base_size;
	short			ai_camp_size;
	short			ai_mine_size;
	short			ai_factory_size;
	short			ai_market_size;
	short			ai_research_size;
	short			ai_war_size;
	short			ai_inn_size;
	short			ai_spy_size;
	short			ai_special_size;
	short			ai_offend_size;
	short			ai_lishorr_size;
	short			ai_mons_train_size;
	short			ai_mons_alchemy_size;
	short			ai_mons_fort_size;
	short			ai_mons_animal_size;
	short			ai_incubator_size;
	short			ai_mons_magic_size;
	short			ai_monster_size;
	short			ai_general_size;
	short			ai_caravan_size;
	short			ai_ship_size;

	short			ai_town_count;
	short			ai_base_count;
	short			ai_camp_count;
	short			ai_mine_count;
	short			ai_factory_count;
	short			ai_market_count;
	short			ai_research_count;
	short			ai_war_count;
	short			ai_inn_count;
	short			ai_spy_count;
	short			ai_special_count;
	short			ai_offend_count;
	short			ai_lishorr_count;
	short			ai_mons_train_count;
	short			ai_mons_alchemy_count;
	short			ai_mons_fort_count;
	short			ai_mons_animal_count;
	short			ai_incubator_count;
	short			ai_mons_magic_count;
	short			ai_monster_count;
	short			ai_general_count;
	short			ai_caravan_count;
	short			ai_ship_count;

	short			ai_base_town_count;

	short			firm_should_close_array[MAX_FIRM_TYPE];

	//------------------------------------------------------//
	// parameters about the nation itself
	//------------------------------------------------------//

	AIRegion		ai_region_array[MAX_AI_REGION];
	char			ai_region_count;

	//------------------------------------------------------//
	// AI unitalties
	//------------------------------------------------------//

	char 			pref_force_projection;
	char			pref_military_development;		// pref_military_development + pref_economic_development = 100
	char			pref_economic_development;
	char			pref_inc_pop_by_capture;		// pref_inc_pop_by_capture + pref_inc_pop_by_growth = 100
	char			pref_inc_pop_by_growth;
	char			pref_peacefulness;
	char			pref_military_courage;
	char			pref_territorial_cohesiveness;
	char			pref_trading_tendency;
	char			pref_allying_tendency;
	char			pref_honesty;
	char			pref_town_defense;
	char			pref_loyalty_concern;
	char			pref_forgiveness;
	char			pref_collect_tax;
	char			pref_hire_unit;
	char			pref_use_weapon;
	char			pref_keep_general;          	// whether to keep currently non-useful the general, or demote them.
	char			pref_keep_skilled_unit; 		// whether to keep currently non-useful skilled units, or assign them to towns.
	char			pref_diplomacy_retry;			// tedency to retry diplomatic actions after previous ones have been rejected.
	char			pref_attack_monster;
	char			pref_spy;
	char			pref_counter_spy;
	char			pref_food_reserve;
	char			pref_cash_reserve;
	char			pref_use_marine;
	char			pref_unit_chase_distance;
	char			pref_repair_concern;
	char			pref_scout;
	char			pref_research;
	char			pref_seat_of_power;
	char		   pref_monster_aggressiveness;
	char		   pref_monster_special;
	char			pref_live_points_reserve;
	char			pref_surrender;

	//------- AI action vars --------//

	short			ai_capture_enemy_town_recno;
	int  			ai_capture_enemy_town_plan_date;
	int  			ai_capture_enemy_town_start_attack_date;
	char			ai_capture_enemy_town_use_all_camp;

	int			ai_last_defend_action_date;

	short			ai_attack_target_x_loc[MAX_AI_ATTACK_TARGET];
	short			ai_attack_target_y_loc[MAX_AI_ATTACK_TARGET];
	short			ai_attack_target_count;
	short			ai_attack_target_nation_recno;		//	nation recno of the target

	AttackCamp  attack_camp_array[MAX_SUITABLE_ATTACK_CAMP];
	short			attack_camp_count;
	short			lead_attack_camp_recno;		// the firm recno of the lead attacking firm

	int			ai_last_build_lishorr_net_power;

public:
	//--------------------------------------------------------------//
	// functions to init. parameters and process ai actions
	//--------------------------------------------------------------//
	void			init(int nationType, int raceId, int colorSchemeId, DWORD playerId); // init local parameters
	void			deinit();

	void 			init_all_ai_info();
	void 			init_ai_info(short** aiInfoArray, short& aiInfoCount, short& aiInfoSize, int arrayInitSize );

	void 			deinit_all_ai_info();

	void			init_personalty();
	int			revealed_by_phoenix(int xLoc, int yLoc);		// override NationBase::revealed_by_phoenix for faster access

	virtual void	process_ai();							// entry point to start ai
	virtual int 	process_ai_main(int mainActionId);
	void 				process_on_going_action();

	//---------------------------------------------------------------//
	// main AI thinking functions
	//---------------------------------------------------------------//
	void			think_explore();
	int 			think_capture();
	void 			think_new_research();

	void			ai_improve_relation();

	//---------------------------------------------------------------//
	// functions for processing waiting actions
	//---------------------------------------------------------------//
	int			ai_build_firm(ActionNode*);
	int			ai_assign_overseer(ActionNode*);
	int 			ai_assign_soldier(ActionNode*);
	int			ai_scout(ActionNode*);
	int			ai_build_town(ActionNode* actionNode);
	int			ai_settle_town(ActionNode*);
	int			ai_assign_spy(ActionNode*);

	//-----------------------------------------------------------//
	// functions used to update internal parameters
	//-----------------------------------------------------------//
	short*		update_ai_firm_array(int firmId, int actionType, int actionRecno, int& arrayCount);

	short*		update_ai_array(short& aiInfoCount, short& aiInfoSize,
						short** aiInfoArray, int arrayIncSize, int actionType, int actionRecno);

	void			add_town_info(short townRecno);	// add town information
	void			del_town_info(short townRecno);	// remove town information
	void			add_firm_info(char firmId, short firmRecno);// add firm information
	void			del_firm_info(char firmId, short firmRecno);// remove useless information as firm is removed
	void 			add_general_info(short unitRecno);
	void 			del_general_info(short unitRecno);
	void 			add_caravan_info(short unitRecno);
	void 			del_caravan_info(short unitRecno);
	void 			add_ship_info(short unitRecno);
	void 			del_ship_info(short unitRecno);
	void			assign_firm_overseer(char firm_id, short firm_recno, short overseerUnitRecno);	// add overseer information
	void			remove_firm_overseer(char firm_id, short firm_recno);	// remove overseer information
	int 			is_caravan_exist(int firstMarket, int secondMarket, int setStopInternal=0);
	void 			update_ai_region();

	AIRegion* 	get_ai_region(int regionId);
	int 			has_base_town_in_region(int regionId);

	int			total_firm_worker(int firmId);
	int 			total_camp_unit_space(int freeSpaceOnly, int firmId=0);

	//-----------------------------------------------------------//
	// functions for building firms
	//-----------------------------------------------------------//
	void			think_build_firm();
	int 			think_build_mine();
	int 			think_attack_units_standing_on_mine();

	int  			think_destroy_raw_site_guard();
	int 			ai_supported_inn_count();
	int 			ai_has_should_close_camp(int regionId);
	int			ai_should_build_mine();

	//-----------------------------------------------------------//
	// functions used to locate position to build firms
	//-----------------------------------------------------------//
	int			seek_mine(short& xLoc, short& yLoc, short& refXLoc, short& refYLoc);
	void			seek_best_build_mine_location(short& xLoc, short& yLoc, short mapXLoc, short mapYLoc);
	void			cal_location_score(short x1, short y1, short width, short height, int& weight);
	int 			find_best_place_loc(short firmId, short refXLoc, short refYLoc, short& resultXLoc, short& resultYLoc, short distance = 0);

	//------------------------------------------------------------//
	// functions for dealing with the AI action array
	//------------------------------------------------------------//
	int 			add_action(short xLoc, short yLoc, short refXLoc, short refYLoc, int actionType, int actionPara, int instanceCount=1, int unitRecno=0, int actionPara2=0, short* groupUnitArray=NULL);
	int 			add_action(ActionNode* actionNode, int immediateProcess=0);
	void			del_action(int actionRecno);
	int			is_action_exist(short actionXLoc, short actionYLoc, short refXLoc, short refYLoc, int actionType, int actionPara, int unitRecno=0);
	int			is_action_exist(int actionType, int actionPara, int regionId=0);
	int			is_build_action_exist(int firmId, int xLoc, int yLoc);

	int 			process_action(int priorityActionRecno=0, int processActionMode=0);	// waiting --> processing
	int 			process_action_id(int actionId);

	void			action_finished(WORD aiActionId, short unitRecno=0, int actionFailure=0);
	void			action_failure(WORD aiActionId, short unitRecno=0);
	void 			auto_next_action(ActionNode* actionNode);

	void 			stop_unit_action(short unitRecno);
	int			check_firm_ready(short xLoc, short yLoc, int firmId=0); // check whether firm exists and belongs to our nation
	int			check_town_ready(short xLoc, short yLoc); // check whether town exists and belongs to our nation

	//------------------------------------------------------------//
	// functions used to find skilled units
	//------------------------------------------------------------//
	Unit*			ai_get_unit(int isCivilian, int raceId, ActionNode* actionNode);
	Unit* 		ai_find_unit(int isCivilian, int raceId, short destX, short destY, char& resultFlag, int actionId=0);
	Unit*			ai_find_unit_from_camp(int raceId, short destX, short destY);
	int   		hire_unit(int raceId, int isCivilian, int hireSpy, short destX, short destY);
	int			recruit_peasant(int raceId, short destX, short destY, int& recruitTownRecno);
	int 			recruit_in_firm_soldier(Firm* destFirmPtr, int preferedRaceId);
	int 			ai_should_hire_unit(int considerProfit);

	//------------------------------------------------------------//
	// other functions
	//------------------------------------------------------------//

	void			settle_to_other_town();
	int 			can_ai_build(int firmId);
	int 			think_succeed_king();
	int 			closest_enemy_firm_distance(int firmId, int xLoc, int yLoc);

	//------------------------------------------------------------//
	// military related functions
	//------------------------------------------------------------//

	void			think_military();

	int			think_secret_attack();
	int 			think_attack_town();
	int			think_close_camp();

	int 			ai_attack_target(int targetXLoc, int targetYLoc, int targetCombatLevel, int defenseMode=0, int justMoveToFlag=0, int attackerMinCombatLevel=0, int attackerCampRecno=0, int useAllCamp=0, int retainCombatLevel=0);
	void 			ai_attack_target_sync();
	void 			ai_attack_target_execute(int directAttack);
	int 			ai_attack_order_nearby_mobile(int targetXLoc, int targetYLoc, int targetCombatLevel);
	void			set_ai_attack_target(int targetXLoc, int targetYLoc, int defenseMode);

	int 			ai_sea_attack_target(int targetXLoc, int targetYLoc);

	void 			ai_attack_unit_in_area(int xLoc1, int yLoc1, int xLoc2, int yLoc2);
	int 			ai_defend(BaseObj* attackerObj);
	int 			ai_request_military_aid();

	void			reset_ai_attack_target();

	int			ai_should_expand_military();
	int 			ai_is_troop_need_new_camp();
	int 			ai_has_too_many_camp();

	int 			ai_should_attack_friendly(int friendlyNationRecno, int attackTemptation);

	void			enable_should_attack_on_target(int targetXLoc, int targetYLoc);

	//------------------------------------------------------------//
	// economic related functions
	//------------------------------------------------------------//

	void			think_reduce_expense();
	int 			surplus_supply_rating();
	int 			ai_trade_with_rating(int withNationRecno);
	int 			ai_should_spend(int importanceRating, float spendAmt=0, float spendLivePoints=0);
	int 			ai_should_spend_war(int enemyMilitaryRating, int considerCeaseFire=0);
	int 			ai_has_enough_food();
	int 			not_enough_food_index();

	//--------------------------------------------------------------//
	// functions for capturing independent and enemy towns
	//--------------------------------------------------------------//

	int 			think_capture_independent(int hireCapturer);
	virtual int capture_expected_resistance(int townRecno, int hireCapturer);
	virtual int start_capture(int townRecno, int hireCapturer);
	int 			find_best_capturer(int townRecno, int& bestResistanceReduce);
	int 			hire_best_capturer(int townRecno, int hireNow);
	int			mobilize_capturer(int unitRecno);

	int 			think_capture_new_enemy_town(Town* capturerTown, int useAllCamp=0);
	void 			think_capturing_enemy_town();

	int 			attack_enemy_town_defense(Town* targetTown, int useAllCamp=0);
	Town* 		think_capture_enemy_town_target(Town* capturerTown);
	int 			enemy_town_combat_level(Town* targetTown, int returnIfWar, int& hasWar);
	int 			enemy_firm_combat_level(Firm* targetFirm, int returnIfWar, int& hasWar);
	int 			mobile_defense_combat_level(int targetXLoc, int targetYLoc, int targetNationRecno, int returnIfWar, int& hasWar);

	int 			should_use_cash_to_capture();

	//--------------------------------------------------------------//
	// spy functions
	//--------------------------------------------------------------//

	int 			ai_assign_spy(int targetXLoc, int targetYLoc, int spyRaceId, int mobileOnly);
	int 			ai_assign_spy_to_town(int townRecno, int raceId=0);
	int 			ai_assign_spy_to_firm(int firmRecno);

	Spy* 			ai_find_spy(int targetXLoc, int targetYLoc, int targetRaceId, int isCivilian, int mobileOnly);
	int			train_spy(int raceId, int isCivilian, short destX, short destY);

	int 			think_assign_spy_target_camp(int raceId, int regionId);
	int 			think_assign_spy_target_inn(int regionId);
	int 			think_assign_spy_target_town(int raceId, int regionId);
	int 			think_assign_spy_own_town(int raceId, int regionId);

	//--------------------------------------------------------------//
	// strategic grand planning functions
	//--------------------------------------------------------------//

	void 			think_grand_plan();
	int 			total_alliance_military();
	int 			total_enemy_military();
	int 			total_enemy_count();

	int 	 		think_against_mine_monopoly();
	int 	 		think_ally_against_big_enemy();
	int 	 		think_unite_against_big_enemy();

	void 			think_deal_with_all_enemy();
	void 			think_deal_with_one_enemy(int enemyNationRecno);

	int 			think_eliminate_enemy_town(int enemyNationRecno);
	int 			think_eliminate_enemy_firm(int enemyNationRecno);
	int 			think_eliminate_enemy_unit(int enemyNationRecno);

	int 			think_attack_enemy_firm(int enemyNationRecno, int firmId);
	int 			think_surrender();

	int 			ai_surrender_to_rating(int nationRecno);

	//--------------------------------------------------------------//
	// functions for responsing to diplomatic messages
	//--------------------------------------------------------------//

	virtual void think_diplomacy();
	int 			think_trade_treaty();
	int			think_propose_friendly_treaty();
	int			think_propose_alliance_treaty();
	int 			think_end_treaty();
	int 			think_request_cease_war();
	int 			think_request_buy_food();
	int			think_declare_war();
	int			think_exchange_tech();
	int			think_demand_tribute_aid();
	int 			think_give_tribute_aid(TalkMsg* rejectedMsg);
	int			think_request_surrender();

	int			ai_process_talk_msg(ActionNode* actionNode);
	void 			ai_notify_reply(int talkMsgRecno);
	int 			should_diplomacy_retry(int talkId, int nationRecno);
	void			ai_end_treaty(int nationRecno);

	int			consider_talk_msg(TalkMsg* talkMsg);
	void			notify_talk_msg(TalkMsg* talkMsg);

	int 			consider_trade_treaty(int withNationRecno);
	int 			consider_friendly_treaty(int withNationRecno);
	int 			consider_alliance_treaty(int withNationRecno);
	int 			consider_military_aid(TalkMsg* talkMsg);
	int 			consider_trade_embargo(TalkMsg* talkMsg);
	int 			consider_cease_war(int withNationRecno);
	int 			consider_declare_war(TalkMsg* talkMsg);
	int 			consider_sell_food(TalkMsg* talkMsg);
	int 			consider_take_tribute(TalkMsg* talkMsg);
	int 			consider_give_tribute(TalkMsg* talkMsg);
	int 			consider_take_aid(TalkMsg* talkMsg);
	int 			consider_give_aid(TalkMsg* talkMsg);
	int 			consider_exchange_tech(TalkMsg* talkMsg);
	int			consider_accept_surrender_request(TalkMsg* talkMsg);

	int			consider_alliance_rating(int nationRecno);
	int 			should_consider_friendly(int withNationRecno);
	int 			ai_overall_relation_rating(int withNationRecno);

	//--------- file functions -----------//

	int 			write_file(File* filePtr);
	int			read_file(File* filePtr);
};

#ifndef __ONATIONA_H
#include <ONATIONA.H>
#endif

//-------------------------------------//

#endif
