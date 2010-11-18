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

//Filename    : ONATIONA.H
//Description : Object nation array

#ifndef __ONATIONA_H
#define __ONATIONA_H

#ifndef __ODYNARRB_H
#include <odynarrb.h>
#endif

#ifndef __ONATION_H
#include <onation.h> 
#endif

//---- at least wait for 1 year after a nation is deleted before setting up a new nation. ---//

#define NEW_NATION_INTERVAL_DAYS		365

struct NewNationPara;

//---------- Define class NationArray -----------//

class NationArray : public DynArrayB
{
public:
	enum { CUSTOM_NAME_LEN=50 };

	short  	nation_count;    // no. of nations, it's different from nation_array.size() which is a DynArrayB
	short  	ai_nation_count;
	int		last_del_nation_date;
	int		last_new_nation_date;

	int		max_nation_population;		// the maximum population in a nation
	int		all_nation_population;		// total population of all nations.

   short		independent_town_count;
	short		independent_town_count_race_array[MAX_RACE];	// the no. of independent towns each race has

	int		max_nation_units;
	int		max_nation_humans;
	int		max_nation_generals;
	int		max_nation_weapons;
	int		max_nation_ships;
	int		max_nation_spies;

	int		max_nation_firms;
	int		max_nation_tech_level;

	int		max_population_rating;
	int		max_military_rating;
	int		max_economic_rating;
	int		max_reputation;
	int		max_kill_monster_score;
	int		max_overall_rating;

	short		max_population_nation_recno;
	short		max_military_nation_recno;
	short		max_economic_nation_recno;
	short		max_reputation_nation_recno;
	short		max_kill_monster_nation_recno;
	short		max_overall_nation_recno;

	int  	   last_alliance_id;
	int  		nation_peace_days;			// continuous peace among nations

	short  	player_recno;
	Nation* 	player_ptr;

	char		nation_color_array[MAX_NATION+1];
	char		nation_power_color_array[MAX_NATION+2];

	char		custom_nation_name_array[MAX_NATION][CUSTOM_NAME_LEN+1];
	char		custom_king_name_array[MAX_NATION][CUSTOM_NAME_LEN+1];

public:
	NationArray();
	~NationArray();

	void		init();
	void 		deinit();
	int  		nation_class_size();

	int		new_nation(int nationClassId, int nationType, int raceId, int colorSchemeId, unsigned long dpPlayerId=0);
	int		new_nation(NewNationPara &);
	int  		create_nation(int nationClassId);
	void 		del_nation(int);

	void 		disp_nation_color(int x, int y, int nationColor);

	int 		can_form_new_ai_nation();
	void		update_statistic();
	void 		update_military_rating();
	void 		update_total_human_count();

	void 		process();
	void 		next_month();
	void 		next_year();

	int 		random_unused_race();
	int 		random_unused_monster_type();
	int 		random_unused_color();

	int  		write_file(File*);
	int  		read_file(File*);

	//------- functions for setting custom names -------//

	void		set_custom_king_name(int nationRecno, char* nameStr);
	char*		get_custom_king_name(int nationNameId, int firstWordOnly=0);

	void		set_custom_nation_name(int nationRecno, char* nameStr);
	char*		get_custom_nation_name(int nationNameId);

	void		set_custom_both_name(int nationRecno, char* nameStr);

	//--------------------------------------------------//

	#ifdef DEBUG
		Nation* operator[](int recNo);
		Nation* operator~();
	#else
		Nation* operator[](int recNo)	{ return (Nation*) get_ptr(recNo); }
		Nation* operator~()				{ return (Nation*) get_ptr(player_recno); } // { return player_ptr; }
	#endif

	int   	is_deleted(int recNo)    { return get_ptr(recNo) == NULL; }

	Nation* get_unpacked_nation(int recNo);   // given a packed recno and return the unpacked nation ptr
	char		should_attack(short attackingNation, short attackedNation);
	void			draw_profile();
	// ###### begin Gilbert 15/3 #######//
	Nation*	get_campaign_nation(int campaignNationRecno);
	// ###### end Gilbert 15/3 #######//
};

  
// --------- define struct NewNationPara ----------//
  
struct NewNationPara
{
	short nation_recno;
	DWORD dp_player_id;
	short color_scheme;
	short race_id;
	char  player_name[NationArray::CUSTOM_NAME_LEN+1];
	short use_gem_stones;
	WORD	ranking;

	void init(short n, DWORD playerId, short scheme, short race, char *playerName, short useGemstone, WORD rank)
	{
		nation_recno = n;
		dp_player_id = playerId;
		color_scheme = scheme;
		race_id = race;
		strcpy(player_name, playerName);
		use_gem_stones = useGemstone;
		ranking = rank;
	}
};

extern NationArray nation_array;

//---------------------------------------------//

#endif
