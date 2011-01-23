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

//Filename    : ONATIONB.H
//Description : class NationBase

#ifndef __ONATIONB_H
#define __ONATIONB_H

#ifndef __OINFO_H
#include <oinfo.h>
#endif

#ifndef __ORAWRES_H
#include <orawres.h>
#endif

#ifndef __OTALKMSG_H
#include <otalkmsg.h>
#endif

#ifndef __OUNITRES_H
#include <ounitres.h>
#endif

//---------- Define constant -----------//

enum { NATION_OWN=1, NATION_REMOTE, NATION_AI };       // NATION_MISC=Government

enum { RELATION_HOSTILE=0, RELATION_TENSE, RELATION_NEUTRAL, RELATION_FRIENDLY, RELATION_ALLIANCE };

enum { RELATION_LEVEL_PER_STATUS = 20 };		// relation_level = status * NATION_LEVEL_PER_STATUS

enum { PLAYER_NATION_RECNO=1 };

//---------- import type ----------//

enum { IMPORT_TYPE_COUNT = 3 };

enum { IMPORT_RAW,
		 IMPORT_PRODUCT,
		 IMPORT_TOTAL
	  };

//--------- income types --------//

enum { INCOME_TYPE_COUNT = 8 };

enum { INCOME_SELL_GOODS,
		 INCOME_EXPORTS,
		 INCOME_TAX,
		 INCOME_TREASURE,
		 INCOME_SELL_FIRM,
		 INCOME_TRIBUTE,
		 INCOME_CONVERSION,
		 INCOME_CHEAT                           // keep last
	  };

//--------- expense types --------//

enum { EXPENSE_TYPE_COUNT = 17 };

enum { EXPENSE_GENERAL,
		 EXPENSE_SPY,
		 EXPENSE_MOBILE_UNIT,
		 EXPENSE_CARAVAN,
		 EXPENSE_WEAPON,
		 EXPENSE_CONSTRUCTION,
		 EXPENSE_FIRM,
		 EXPENSE_TOWN,
		 EXPENSE_TRAIN_UNIT,
		 EXPENSE_HIRE_UNIT,
		 EXPENSE_REWARD_UNIT,
		 EXPENSE_GRANT_OWN_TOWN,
		 EXPENSE_GRANT_OTHER_TOWN,
		 EXPENSE_IMPORTS,
		 EXPENSE_TRIBUTE,
		 EXPENSE_BRIBE,
		 EXPENSE_CONVERSION,
	  };

//----- Define struct NationRelation -------//

struct NationRelation			// many-to-many relationships between nations
{
	char  has_contact;			// whether this nation has been contacted or not
	char	should_attack;			// whether units should automatically attack units/firms of this nation when the relationship is hostile

	char	trade_treaty;			// whether allow trading with this nation

	char  status;
	const char* status_str(); // { return relation_status_str_array[status]; }

	int	is_friendly()		{ return status >= RELATION_FRIENDLY && status <= RELATION_ALLIANCE; }

	int	last_change_status_date;

	char* status_duration_str();

	char  ai_relation_level;		// AI's subjectively relation levels towards the others, the opposite nation's relation level is not the same as this
	char	ai_secret_attack;
	char	ai_demand_trade_treaty;

	float good_relation_duration_rating;		// a rating indicate how long does a good relation (friendly/alliance) lasts
	short	started_war_on_us_count;				// how many times this nation has started a war with us, the more the times the worse this nation is.

	float cur_year_import[IMPORT_TYPE_COUNT];
	float last_year_import[IMPORT_TYPE_COUNT];
	float lifetime_import[IMPORT_TYPE_COUNT];

	float import_365days(int importType)	{ return last_year_import[importType]*(365-info.year_day)/365 +
															cur_year_import[importType]; }

	int	last_talk_reject_date_array[MAX_TALK_TYPE];		// the date which the last diplomatic request was rejected.
	int 	never_accept_until_date_array[MAX_TALK_TYPE];
	int	last_military_aid_date;
	char	ai_never_consider[MAX_TALK_TYPE];		// if this is 1, the AI will never consider this diplomatic option

	void	set_never_accept_until_date(int talkId, int dayCount);

	void 	set_ai_never_consider(short* neverConsiderArray);
	void	reset_ai_never_consider(short* neverConsiderArray=NULL);

	int	last_give_gift_date;				// the last date which the current nation give tribute, aid or technology to this nation
	short total_given_gift_amount;		// the total amount of gift the current nation has given to this nation

	char	contact_msg_flag;										// used only in multiplayer

//	static char* relation_status_str_array[5];
};

//---------- Define class Nation -----------//

class BaseObj;
class Unit;
class Firm;

class NationBase
{
public:
	enum { NATION_NAME_LEN=50 };

	short	nation_recno;
	char  nation_type;
	short	campaign_nation_recno;		// if >0, it points to the CampaignNation in a campaign

	char  race_id;

	int	is_human()		{ return race_id>0; }
	int	is_monster()	{ return race_id<0; }
	int	monster_id()	{ return -race_id;  }

	char  color_scheme_id;
	char	nation_color;				// main color of the nation, based on from color_scheme_id

	short	king_unit_recno;			// recno of the king
	short king_leadership;

	char  nation_name_str[NATION_NAME_LEN+1];		// for nation_name()'s use
	char* nation_name();
	const char* king_name(int firstWordOnly=0);

	DWORD player_id;				   // player id for multiplayer game

	char  next_frame_ready;				// for indicating whether the next frame is ready or not
	short last_caravan_id;				// id. of the nation's caravan.

	short	nation_firm_count;			// total no. of firms the nation has built
	int   last_build_firm_date;

	char	know_base_array[MAX_RACE];		// whether the unit knows how to constructure seat of power or not
	char	base_count_array[MAX_RACE];   // no. of seat of power this nation has

	char	is_at_war_today;
	char	is_at_war_yesterday;
	int	last_war_date;
	short last_attacker_obj_recno;
	int	last_independent_unit_join_date;

	int	peaceful_days()			{ return info.game_date - last_war_date; }
	char*	peace_duration_str();

	void	set_at_war_today()	{ is_at_war_today=1; }
	int	is_at_war()				{ return is_at_war_today || is_at_war_yesterday; }

	char  cheat_enabled_flag;

	//----------------------------------//

	float		 cash;
	float		 food;
	float		 increased_cash;

	char*		 cash_str();
	char*		 total_cash_str();
	char*		 annual_cash_str();
	char*		 food_str();
	char*		 total_food_str();
	char*		 annual_food_str();

	void		 reset_all_finance();		// for scenario creation and editing, reset all current year finances

	int		 yearly_food_consumption() { return UNIT_FOOD_YEAR_CONSUMPTION * all_population(); }
	int		 yearly_food_production() 	{ return PEASANT_FOOD_YEAR_PRODUCTION * total_jobless_population; }
	int       yearly_food_change() 	   { return yearly_food_production() - yearly_food_consumption(); }

	float		 reputation;				// can be negative, means bad reputation
	float		 live_points;				// for monster nations only
	float		 kill_monster_score;
	
	//------- unit auto retreat -------------//

	short		 auto_retreat_hit_point;		// auto retreat if hit point reaches this presentage
	void		 set_auto_retreat(int hitPointPercentage);
	
	//------- town auto policy -------------//

	short		 auto_collect_tax_loyalty;		// auto collect tax if the loyalty reaches this level
	short		 auto_grant_loyalty;				// auto grant if the loyalty drop below this level

	//------- research vars --------//

	short 	research_tech_id;					   // the id. of the tech this firm is currently researching
	// ###### begin Gilbert 14/3 #######//
//	float 	research_progress;		// percent completed on researching the current technology
	float		fraction_research_progress;		// fractional part of research progress
	float		research_progress();		// percent completed on researching the current technology
	void		set_research_progress(float);
	// ###### end Gilbert 14/3 #######//

	//----- yearly income, expense and profit ------//

	float		 cur_year_profit;
	float		 last_year_profit;
	float		 profit_365days()				{ return last_year_profit*(365-info.year_day)/365 +
													  cur_year_profit; }
	float		 cur_year_fixed_income;
	float		 last_year_fixed_income;
	float		 fixed_income_365days()		{ return last_year_fixed_income*(365-info.year_day)/365 +
													  cur_year_fixed_income; }
	float		 cur_year_fixed_expense;
	float		 last_year_fixed_expense;
	float		 fixed_expense_365days()	{ return last_year_fixed_expense*(365-info.year_day)/365 +
													  cur_year_fixed_expense; }

	float		 fixed_profit_365days()		{ return fixed_income_365days() - fixed_expense_365days(); }

	//------- yearly income ------//

	float		 cur_year_income_array[INCOME_TYPE_COUNT];
	float		 last_year_income_array[INCOME_TYPE_COUNT];

	float		 cur_year_income;		// total income
	float		 last_year_income;

	float		 income_365days()				{ return last_year_income*(365-info.year_day)/365 + cur_year_income; }

	float 	 income_365days(int incomeType)	{ return last_year_income_array[incomeType] * (365-info.year_day) / 365 +
																  cur_year_income_array[incomeType]; }
	float		 true_income_365days();		// total income, excluding cheats

	//------- yearly expense ------//

	float		 cur_year_expense_array[EXPENSE_TYPE_COUNT];
	float		 last_year_expense_array[EXPENSE_TYPE_COUNT];

	float		 cur_year_expense;		// total expense
	float		 last_year_expense;

	float		 expense_365days()				{ return last_year_expense*(365-info.year_day)/365 + cur_year_expense; }

	float 	 expense_365days(int expenseType)	{ return last_year_expense_array[expenseType] * (365-info.year_day) / 365 +
																  cur_year_expense_array[expenseType]; }
	//------- yearly expense ------//

	float		 cur_year_cheat;		// total cheat
	float		 last_year_cheat;

	float 	 cheat_365days()			{ return last_year_cheat*(365-info.year_day)/365 + cur_year_cheat; }

	float		 true_profit_365days() 	{ return profit_365days() - cheat_365days(); }

	//----- yearly food in, out and change ------//

	float		 cur_year_food_in;
	float		 last_year_food_in;

	float		 cur_year_food_out;
	float		 last_year_food_out;

	float		 cur_year_food_change;
	float		 last_year_food_change;
	float		 food_change_365days()		{ return last_year_food_change*(365-info.year_day)/365 +
													  cur_year_food_change; }
	//----- yearly reputatino change ------//

	float     cur_year_reputation_change;
	float     last_year_reputation_change;

	float		 reputation_change_365days() { return last_year_reputation_change*(365-info.year_day)/365 +
														 cur_year_reputation_change; }

	float     cur_year_live_points_change;
	float     last_year_live_points_change;

	float		 live_points_change_365days() { return last_year_live_points_change*(365-info.year_day)/365 +
														 cur_year_live_points_change; }

	//--------- inter-nation relationship -----------//

	NationRelation relation_array[MAX_NATION];				// inter-relationship with other nations
	char		 relation_status_array[MAX_NATION];				// replace status in struct NationRelation
	char		 relation_passable_array[MAX_NATION]; 			// for seeking to indicate whether passing other nation region
	char		 relation_should_attack_array[MAX_NATION];
	char		 is_allied_with_player;								// for fast access in visiting world functions

	void		 set_relation_status(short nationRecno, char newStatus, char recursiveCall=0);
	char		 get_relation_status(short nationRecno);
	void		 set_relation_passable(short nationRecno, char status);
	char		 get_relation_passable(short nationRecno);
	void		 set_relation_should_attack(short nationRecno, char newValue, char remoteAction);
	char		 get_relation_should_attack(short nationRecno);

	void		 set_trade_treaty(int nationRecno, char allowFlag);
	void		 establish_contact(int nationRecno);
	void		 change_ai_relation_level(short nationRecno, int levelChange);

	#ifdef DEBUG
		NationRelation* get_relation(int nationRecno);
	#else
		NationRelation* get_relation(int nationRecno) { return relation_array+nationRecno-1; }
	#endif

	//--------- total yearly trade amount --------//

	float		 total_year_trade(int nationRecno);
	int 		 trade_rating(int nationRecno);

	//---------- statistic ------------//

	int  		 total_population;
	int		 total_jobless_population;
	int		 all_population()				{ return total_population + total_human_count + total_monster_count; }

	int		 total_unit_count;
	int		 total_human_count;
	int		 total_monster_count;
	int		 total_general_count;
	int		 total_weapon_count;
	int		 total_ship_count;
	int		 total_firm_count;
	int		 total_spy_count;
	int		 total_ship_combat_level;

	short		 largest_town_recno;		// the recno of the biggest town of this nation
	short		 largest_town_pop;

	int 		 total_tech_level(int unitClass=0);
	int 		 base_town_count_in_region(int regionId);

	short		 raw_count_array[MAX_RAW];		// no. of natural resources site this nation possesses

	short		 last_unit_name_id_array[MAX_UNIT_TYPE];

	//--------- rank ratings ---------//

	int		 population_rating;
	int		 military_rating;
	int		 economic_rating;
   int		 overall_rating;

	void		 update_nation_rating();

	int		 get_population_rating();
	int		 get_economic_rating();
	int		 get_overall_rating();

	int		 population_rank_rating();
	int		 military_rank_rating();
	int		 economic_rank_rating();
	int		 reputation_rank_rating();
	int		 kill_monster_rank_rating();
	int		 overall_rank_rating();

	//------ additional statistic ------//

	int		 enemy_soldier_killed;
	int		 own_soldier_killed;
	int		 enemy_frythan_killed;
	int		 own_frythan_killed;
	int		 enemy_civilian_killed;
	int		 own_civilian_killed;
	int		 enemy_weapon_destroyed;
	int		 own_weapon_destroyed;
	int		 enemy_ship_destroyed;
	int		 own_ship_destroyed;
	int		 enemy_firm_destroyed;
	int		 own_firm_destroyed;

	//---------- functions -------------//

	int			goal_destroy_nation_achieved();
	int			goal_destroy_monster_achieved();
	int			goal_population_achieved();
	int			goal_economic_score_achieved();
	int			goal_total_score_achieved();

	int			is_own()    	{ return nation_type==NATION_OWN;    }
	int			is_ai()			{ return nation_type==NATION_AI;		 }
	int			is_remote() 	{ return nation_type==NATION_REMOTE; }

public:
	NationBase();
	~NationBase();

	virtual	void init(int nationType, int raceId, int colorSchemeId, DWORD playerId=0);
   virtual  void deinit();

	void		init_cash( int gemStones );		// gemStones to used in lobbied multi-player
	void 		init_relation(int relationNationRecno);

	void 		close_all_firm();
	void		deinit_all_unit();
	void 		succeed_king(int kingUnitRecno);
	void 		set_king(int kingUnitRecno, int firstKing);
	void 		hand_over_to(int handoverNationRecno);

	void 		disp_nation_color(int x, int y);

	void 		next_day();
	void 		next_month();
	void 		next_year();

	//--- functions pertaining to food, cash, reputation and live points ---//

	void		add_income(int incomeType, float incomeAmt, int fixedIncome=0);
	void		add_expense(int expenseType, float expenseAmt, int fixedExpense=0);
	void		add_cheat(float cheatAmt);
	void		change_reputation(float);
	void 		change_live_points(float);

	void		add_food(float);
	void		consume_food(float);
	void		import_goods(int importType, int nationRecno, float importAmt);

	void 		set_auto_collect_tax_loyalty(int loyaltyLevel);
	void 		set_auto_grant_loyalty(int loyaltyLevel);

	// ####### begin Gilbert 21/4 #########//
	void		repair_all_building(int flag, char remoteAction );
	// ####### end Gilbert 21/4 #########//

	//------ diplomacy functions ------//

	void 		form_friendly_treaty(int nationRecno);
	void 		form_alliance_treaty(int nationRecno);
	void 		end_treaty(int endTreatyNationRecno, int newStatus);
	void 		give_tribute(int toNationRecno, int tributeAmt);
	void 		give_tech(int toNationRecno, int techId, int techVersion);

	void		surrender(int toNationRecno);
	void		defeated();

	//-------- misc functions --------//

	int		has_king_successor_candidate();		// whether the nation has any people (but not counting the king). If no, then the nation is going to end.

	void 		being_attacked(BaseObj* attackerObj);
	void 		civilian_killed(int civilianRaceId, int isAttacker, int killedCount=1);
	void 		change_all_people_loyalty(float loyaltyChange, int raceId=0);

	int		check_win(int checkOnlyFlag=0);
	int		check_lose(int checkOnlyFlag=0);

	int		revealed_by_phoenix(int xLoc, int yLoc);		// will be overloaded by Nation::revealed_by_phoenix for faster access

	//------ research functions ------//

	void 		start_research(int techId, char remoteAction);
	int  		progress_research(float newProgress);

	//--------- file functions -----------//

	int 		write_file(File* filePtr);
	int		read_file(File* filePtr);

	//--------- multiplayer checking codes ---------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();
};

//--------------------------------------//

extern short nation_hand_over_flag; // 0 for no nation hand over, 1-MAX_NATION for nation hand over

#endif
