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

// Filename    : OPROFILE.H
// Description : player profile 

#include <oprofile.h>
#include <crc.h>
#include <all.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <oherores.h>
#include <ounit.h>
#include <ospy.h>
#include <osoldier.h>
#include <otransl.h>
#include <ot_unit.h>

#include <stdlib.h>


// ------- begin of function PlayerProfile::PlayerProfile -------//
//
PlayerProfile::PlayerProfile()
{
	memset( this, 0, sizeof(*this) );
}
// ------- end of function PlayerProfile::PlayerProfile -------//


// ------- begin of function PlayerProfile::init -------//
//
void PlayerProfile::init()
{
	memset( this, 0, sizeof(*this) );

	crypt_begin = 0;		// unencrypted

	// -------- hero roster and item store -------//

	hero_count = 0;
	item_count = 0;
	tutorial_finish_count = 0;
}
// ------- end of function PlayerProfile::init -------//


// ------- begin of function PlayerProfile::is_registered -------//
//
int PlayerProfile::is_registered()
{
	return player_name[0];
}
// ------- end of function PlayerProfile::is_registered -------//


// ------- begin of function PlayerProfile::save -------//
//
// save to <file_name>.PRF
//
// return true if success
//
int PlayerProfile::save()
{
	// randomly put a encrypt key

	encrypt( BYTE(m.get_time() % 192) + 32 | 7);

	File f;
	String str;
	str = file_name;
	str += ".PRF";

	f.file_create( str );
	int rc = f.file_write( this, sizeof(*this) );
	f.file_close();

	if( !decrypt() )
	{
		err_here();
	}

	return rc;
}
// ------- end of function PlayerProfile::save -------//


// ------- begin of function PlayerProfile::load -------//
//
// return true if success
//
int PlayerProfile::load(char *fileName)
{
	PlayerProfile tmp;

	File f;
	f.file_open(fileName);

	if( f.file_read(&tmp, sizeof(tmp)) && tmp.decrypt() )
	{
		*this = tmp;
		// set file_name to fileName without extension
		strncpy( file_name, fileName, SAVE_GAME_DIR_LEN );
		file_name[SAVE_GAME_DIR_LEN] = '\0';
		char *dotPos;
		if( (dotPos = strchr( file_name, '.' )) )
			*dotPos = '\0';
		return 1;
	}
	else
	{
		return 0;
	}
}
// ------- end of function PlayerProfile::load -------//


// ------- begin of function PlayerProfile::reload -------//
//
// reload current profile
//
// return true on success
//
int PlayerProfile::reload()
{
	String str;
	str = file_name;
	str += ".PRF";

	return load(str);
}
// ------- end of function PlayerProfile::reload -------//


// ------- begin of function PlayerProfile::save_game_path -------//
//
// if wildcardStr is not null , return "SAVE\<save_dir>\<wildcardStr>"
// if wildcardStr is null, return "SAVE\<save_dir>"
//
char *PlayerProfile::save_game_path(char *wildcardStr)
{
	static char retStr[MAX_PATH+1];

	if( !is_registered() )
	{
		if( wildcardStr )
			return wildcardStr;
		else
			retStr[0] = '\0';		// null string
	}
	else
	{
		strcpy( retStr, DIR_SAVE );
		strcat( retStr, save_dir );

		if( wildcardStr )
		{
			strcat( retStr, "\\" );
			strcat( retStr, wildcardStr );
		}
	}

	return retStr;
}
// ------- end of function PlayerProfile::save_game_path -------//


// ------- begin of function PlayerProfile::encrypt -------//
//
// encrypt from crypt_begin to crypt_end
//
void PlayerProfile::encrypt(BYTE key)
{
	crypt_begin = 0;

	// set crc check sum at crypt_end

	crypt_end = crc8( &crypt_begin, &crypt_end - &crypt_begin );

	// ------ actual encryption -------//

	crypt_begin = key;

	err_when( &crypt_end <= &crypt_begin );

	// ------ pass 1 forward addition -------//

	unsigned char *b;
	BYTE runKey = key;
	for( b = &crypt_begin+1; b <= &crypt_end; ++b )
	{
		runKey += key;
		*b = *b + runKey;
	}

	// ----- pass 2 foward accumulation ------//

	for( b = &crypt_begin+1; b <= &crypt_end-1; ++b )
	{
		b[1] += *b;
	}

	// ----- pass 3 backward accumulation ------//

	for( b = &crypt_end-1; b >= &crypt_begin+1; --b )
	{
		*b += b[1];
	}
}
// ------- end of function PlayerProfile::encrypt -------//


// ------- begin of function PlayerProfile::decrypt -------//
//
// return true if valid
int PlayerProfile::decrypt()
{
	unsigned char *b;

	// ------ pass 3 inverse backward accumulation -------//

	for( b = &crypt_begin+1; b <= &crypt_end-1; ++b )
	{
		*b -= b[1];
	}

	// ------ pass 2 inverse forward accumulation --------//

	for( b = &crypt_end-1; b >= &crypt_begin+1; --b )
	{
		b[1] -= *b;
	}

	// ------ pass 1 forward addition -------//

	unsigned char key = crypt_begin;

	err_when( &crypt_end <= &crypt_begin );

	BYTE runKey = key;
	for( b = &crypt_begin+1; b <= &crypt_end; ++b )
	{
		runKey += key;
		*b = *b - runKey;
	}

	crypt_begin = 0;

	// set crc check sum at crypt_end

	return crc8( &crypt_begin, &crypt_end - &crypt_begin + 1 ) == 0;
}
// ------- end of function PlayerProfile::decrypt -------//


// ------- begin of function PlayerProfile::get_hero -------//
//
ProfileHero *PlayerProfile::get_hero(int i)
{
	if( i > 0 && i <= hero_count )
	{
		return hero_array + i - 1;
	}
	else
	{
		return NULL;
	}
}
// ------- end of function PlayerProfile::get_hero -------//


// ------- begin of function PlayerProfile::del_hero -------//
//
void PlayerProfile::del_hero(int i)
{
	if( i > 0 && i <= hero_count )
	{
		m.del_array_rec( hero_array, MAX_PROFILE_HERO, sizeof(ProfileHero), i );
		m.del_array_rec( on_duty_array, MAX_PROFILE_HERO, sizeof(char), i );
		--hero_count;
	}
	else
	{
		err_here();
	}
}
// ------- end of function PlayerProfile::del_hero -------//


// ------- begin of function PlayerProfile::add_hero -------//
//
// return recno, 0 if full
int PlayerProfile::add_hero(ProfileHero *hero)
{
	if( hero_count < MAX_PROFILE_HERO )
	{
		hero_array[hero_count] = *hero;
		on_duty_array[hero_count] = 0;
		return ++hero_count;
	}
	return 0;
}
// ------- end of function PlayerProfile::add_hero -------//


// ------- begin of function PlayerProfile::get_item -------//
//
Item *PlayerProfile::get_item(int i)
{
	if( i > 0 && i <= item_count )
	{
		return item_array + i - 1;
	}
	else
	{
		return NULL;
	}
}
// ------- end of function PlayerProfile::get_item -------//


// ------- begin of function PlayerProfile::del_item -------//
//
void PlayerProfile::del_item(int i)
{
	if( i > 0 && i <= item_count )
	{
		m.del_array_rec( item_array, MAX_PROFILE_ITEM, sizeof(Item), i );
		--item_count;
	}
	else
	{
		err_here();
	}
}
// ------- end of function PlayerProfile::del_item -------//


// ------- begin of function PlayerProfile::add_item -------//
//
// return recno, 0 if full
int PlayerProfile::add_item(Item *item)
{
	if( item_count < MAX_PROFILE_ITEM )
	{
		item_array[item_count] = *item;
		return ++item_count;
	}
	return 0;
}
// ------- end of function PlayerProfile::add_item -------//


// ------- begin of function PlayerProfile::default_save_name -------//
//
// generate default save path from player name
//
void PlayerProfile::default_save_name( char *saveName, char *playerName )
{
	char *p = playerName;
	int i = 0;
	for( i = 0; *p && i < SAVE_GAME_DIR_LEN; ++p )
	{
		if( *p == ' ' )
		{
			// skip this char
		}
		else if( *p >= 'A' && *p <= 'Z'
			|| *p >= 'a' && *p <= 'z' 
			|| *p >= '0' && *p <= '9' )
		{
			saveName[i] = *p;
			++i;
		}
		else	// other character
		{
			saveName[i] = '_';
			++i;
		}
	}
	
	if( i == 0 )
	{
		strcpy( saveName, "default" );
	}
	else
	{
		saveName[i] = '\0';		// null terminate
	}
}
// ------- end of function PlayerProfile::default_save_name -------//


//----- Begin of function ProfileHero::update_unit ------//

void ProfileHero::update_unit(Unit *unitPtr, Spy *spyPtr)
{
	sprite_recno        = 0;
	unit_id             = unitPtr->unit_id;
	rank_id             = unitPtr->rank_id;
	race_id             = unitPtr->race_id;
	// nation_recno     = unitPtr->nation_recno;
	name_id             = unitPtr->name_id;
	hero_id             = unitPtr->hero_id;
	unique_id           = unitPtr->unique_id;

	loyalty             = unitPtr->loyalty;
	target_loyalty      = unitPtr->target_loyalty;
	skill               = unitPtr->skill;
	nation_contribution = unitPtr->nation_contribution;
	total_reward        = unitPtr->total_reward;

	item                = unitPtr->item;

	// spy attribute
	if( !spyPtr )
	{
		is_spy = 0;
		spy_skill = 0;
		spy_loyalty = 0;
		// true_nation_recno = 0;
		cloaked_nation_recno = 0;
	}
	else
	{
		is_spy = 1;
		spy_skill = spyPtr->spy_skill;
		spy_loyalty = spyPtr->spy_loyalty;
		// true_nation_recno = spyPtr->true_nation_recno;
		cloaked_nation_recno = spyPtr->cloaked_nation_recno;
	}

	// update king

	if( unitPtr->rank_id == RANK_KING )
	{
		// BUGHERE : update king name here
	}
}
//----- End of function ProfileHero::update_unit ------//


//----- Begin of function ProfileHero::update_unit ------//

void ProfileHero::update_unit(Soldier *soldierPtr, Spy *spyPtr, int firmRecno)
{
	sprite_recno        = 0;
	unit_id             = soldierPtr->unit_id;
	rank_id             = soldierPtr->rank_id;
	race_id             = soldierPtr->race_id;
	// nation_recno     = soldierPtr->nation_recno;
	name_id             = soldierPtr->name_id;
	hero_id             = soldierPtr->hero_id;
	unique_id           = soldierPtr->unique_id;

	loyalty             = soldierPtr->loyalty;
	target_loyalty      = soldierPtr->target_loyalty(firmRecno);
	skill               = soldierPtr->skill;
	nation_contribution = 0;
	total_reward        = 0;

	item                = soldierPtr->item;

	// spy attribute
	if( !spyPtr )
	{
		is_spy = 0;
		spy_skill = 0;
		spy_loyalty = 0;
		// true_nation_recno = 0;
		cloaked_nation_recno = 0;
	}
	else
	{
		is_spy = 1;
		spy_skill = spyPtr->spy_skill;
		spy_loyalty = spyPtr->spy_loyalty;
		// true_nation_recno = spyPtr->true_nation_recno;
		cloaked_nation_recno = spyPtr->cloaked_nation_recno;
	}
}
//----- End of function ProfileHero::update_unit ------//


// ---- Begin of function ProfileHero::create_unit ------//

int ProfileHero::create_unit(int nationRecno, int xLoc, int yLoc)
{
	int unitRecno = unit_array.add_unit( unit_id, nationRecno, rank_id, loyalty, xLoc, yLoc );

	if( unitRecno )
	{
		Unit *unitPtr =  unit_array[unitRecno];

		unitPtr->set_name( name_id );
		unitPtr->hero_id    = hero_id;
		unitPtr->unique_id  = unique_id;
		unitPtr->skill      = skill;
		unitPtr->nation_contribution = nation_contribution;
		unitPtr->total_reward = total_reward;
		unitPtr->item       = item;

		if( is_spy )
		{
			unitPtr->spy_recno = spy_array.add_spy(unitRecno, spy_skill );
		}
		unitPtr->set_combat_level(-1);

		sprite_recno = unitPtr->sprite_recno;		// mark this unit has been put to the game
	}

	return unitRecno;
}
// ---- End of function ProfileHero::create_unit ------//


// ---- begin of function ProfileHero::unit_name -------//
//
char* ProfileHero::unit_name(int nationRecno, int withTitle, int firstNameOnly)
{
	static String str;

	err_when( !unit_res.init_flag );
	UnitInfo* unitInfo = unit_res[unit_id];

	str = "";

	//------------------------------------//

	if( is_human() && unit_id != UNIT_WAGON )
	{
		err_when( !race_res.init_flag );

		char *baseName;
		if( hero_id )
		{
			baseName = hero_res[hero_id]->name;
		}
		else
		{
			if( rank_id == RANK_KING )		// use the player name
			{
				// baseName = game.campaign()->get_nation(nationRecno)->king_name();
				err_here();
				baseName = "";
			}
			else
				baseName = race_res[race_id]->get_name(name_id, firstNameOnly?1:0);
		}

		if( withTitle && rank_id != RANK_SOLDIER )		// soldier has no title
			str = text_unit.str_unit_titled( race_id, baseName, rank_id );
		else
			str = baseName;
	}
	else if( is_monster() )
	{
		char *baseName;
		if( rank_id == RANK_KING )		// use the player name
		{
			//baseName = game.campaign()->get_nation(nationRecno)->king_name();
			err_here();
			baseName = "";
		}
		else
			baseName = monster_res.get_name(name_id);

		if( withTitle )
			str = text_unit.str_unit_titled( race_id, baseName, rank_id );
		else
			str = baseName;
	}
	else
	{
		str = unitInfo->name;
	}

   return str;

}
// ---- end of function ProfileHero::unit_name -------//
