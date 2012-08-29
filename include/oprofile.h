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

// Filename     : OPROFILE.H
// Description  : Header file of player profile


#ifndef __OPROFILE_H
#define __OPROFILE_H

#include <win32_compat.h>
#include <oitem.h>
#include <oskill.h>



class File;
class Unit;
class Spy;
struct Soldier;
  
// -------- define struct hero ---------//

#pragma pack(1)
class ProfileHero
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
	int		unique_id;              // unique number generated across campaign

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

	// ------- profile ---------- //

	char			on_duty;				// true if participated in a game, cannot be used in other game

public:
	int  		combat_level() 	{ return skill.actual_combat_level(&item); }
	int  		skill_level()		{ return skill.actual_skill_level(&item); }
	int  		max_hit_points()	{ return skill.actual_max_hit_points(&item); }
	char*		unit_name(int nationRecno, int withTitle=1, int firstNameOnly=0);

	void		update_unit(Unit *, Spy *);
	void		update_unit(Soldier *, Spy *, int firmRecno);
	int		create_unit(int nationRecno, int xLoc, int yLoc );
};
#pragma pack()


// -------- define class PlayerProfile --------//

#pragma pack(1)
class PlayerProfile
{
public:
	enum { PLAYER_NAME_LEN=20 };		// should be as same as NationBase::KING_NAME_LEN, and config::PLAYER_NAME_LEN
	enum { PLAYER_DESC_LEN=80, SAVE_GAME_DIR_LEN = 8 };
	enum { LOGIN_PASSWORD_LEN=20 };
	enum { MAX_PROFILE_HERO = 20, MAX_PROFILE_ITEM = 40 };
	enum { MAX_MONSTER_BOSS = 8 };

	char			player_name[PLAYER_NAME_LEN+1];
	char			player_desc[PLAYER_DESC_LEN+1];
	char			file_name[SAVE_GAME_DIR_LEN+1];
	char			save_dir[SAVE_GAME_DIR_LEN+1];

	char			hide_tips;

	// apply to lobby launched game
	char			mp_use_gem_stone; // 0 - not use, 1 - use
	char			mp_new_game_flag;	// 0 - not specified, 1 = new, 2 = load game
	char			mp_load_file_name[9];
	int			reserved_p3;

	// ----------------------------//

	BYTE			crypt_begin;

	// -------- hero roster and item store -------//

	int			hero_count;
	ProfileHero	hero_array[MAX_PROFILE_HERO];
	char			on_duty_array[MAX_PROFILE_HERO];			// on duty in the coming game
	int			item_count;
	Item			item_array[MAX_PROFILE_ITEM];

	// -------- statistic --------//

	char			king_rank;
	int			single_game_started;			// single player game
	int			single_game_won;				// meaningful?
	int			multi_game_started;			// multi player game
	int			multi_game_won;
	int			campaign_started;
	int			campaign_finish;
	short			monster_boss_defeat[MAX_MONSTER_BOSS];

	// -------- lobby client info ----------//

	char			login_name[PLAYER_NAME_LEN+1];		// empty string if not registered
	char			login_password[LOGIN_PASSWORD_LEN+1];
	WORD			icon_id;
	WORD			country_id;
	WORD			ranking;
	WORD			num_won_games;
	WORD			num_lose_games;
	WORD			num_start_games;
	WORD			gem_stones;
	BYTE			next_login_pass_id;	// a random number to check against server db same field, guard against loading old profile
	BYTE			demo_try_count;		// increase in demo version, number of new/load lobbied mp game started
	
	// -------- tutorial variable -------- //
	
	short			tutorial_finish_count;

	int			reserved_c0;
	int			reserved_c1;
	int			reserved_c2;
	int			reserved_c3;

	// ----------------------------//

	BYTE			crypt_end;

public:
	PlayerProfile();
	void			init();
	int			is_registered();

	int			save();
	int			load(char *fileName);
	int			reload();
	int			load_by_login_name(char* loginName);

	char *		save_game_path(char *wildcardStr);
	int			re_create_directory();

	// -------- encrypt ----------//

	void			encrypt(BYTE key);
	int			decrypt();				// return true if valid

	// -------- interface function --------//

	int			register_menu();					// return 1=new profile, 2=profile loaded
	int			select_profile_menu();
	void			hero_item_menu(int mode);		// 0=maintain item, 1=before game, 2=after game
	static int	load_count_profiles( PlayerProfile *profileArray, int maxLoad );		// null to count

private:

	ProfileHero *get_hero(int);
	void			del_hero(int);
	int			add_hero( ProfileHero * );

	Item *		get_item(int);
	void			del_item(int);
	int			add_item( Item * );

	// ------- private interface ---------//

	void			disp_hero(ProfileHero *);
	void			disp_hero(Unit *);
	void			disp_item(int x, int y, Item *);

	// ------- string function -------//

	void			default_save_name( char *saveName, char *playerName );
};
#pragma pack()

extern PlayerProfile player_profile;

#endif
