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

// Filename    : OUN_SPAB.CPP
// Description : unit special ability

#include <ounit.h>
#include <oun_spab.h>
#include <oinfo.h>

// number of days the ability last for
#define SPECIAL_ABILITY_DURATION 10

// number of days between ability start
#define SPECIAL_ABILITY_DELAY 15


int Unit::think_use_special_ability()
{
	int spLevel = 0;

	if( is_human() )
	{
		if( combat_level() > MAX_COMBAT_BATTLE )
			spLevel = combat_level() / 4;
		else if( skill.max_combat_level > MAX_COMBAT_BATTLE )		// potential hero can trigger special power
			spLevel = 1;
		else
			return 0;

		if( info.game_date - last_special_ability_start_date < SPECIAL_ABILITY_DELAY )
			return 0;

		if( m.random(100+spLevel) < 100 )
			return 0;

		use_special_ability();
		return is_special_ability_effective();
	}
	return 0;
}


int Unit::is_special_ability_effective()
{
	if( special_abilty_id &&
		info.game_date - last_special_ability_start_date < SPECIAL_ABILITY_DURATION )
		return special_abilty_id;

	return 0;
}


void Unit::use_special_ability()
{

	int abilityId = 0;

	switch( unit_id )
	{
	case UNIT_NORMAN_INF:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_NORMAN_SPU:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_VIKING_INF:
		abilityId = SPAB_FREEZE;
		break;
	case UNIT_VIKING_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_CELTIC_INF:
		abilityId = SPAB_FREEZE;
		break;
	case UNIT_CELTIC_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_ROMAN_INF:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_ROMAN_SPU:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_GREEK_INF:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_GREEK_SPU:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_CARTH_INF:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_CARTH_SPU:
		abilityId = SPAB_FREEZE;
		break;
	case UNIT_CHINESE_INF:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_CHINESE_SPU:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_JAPANESE_INF:
		abilityId = SPAB_FREEZE;
		break;
	case UNIT_JAPANESE_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_MONGOL_INF:
		abilityId = SPAB_FREEZE;
		break;
	case UNIT_MONGOL_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_EGYPTIAN_INF:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_EGYPTIAN_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_PERSIAN_INF:
		abilityId = SPAB_REFLECT_ARROW;
		break;
	case UNIT_PERSIAN_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_INDIAN_INF:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	case UNIT_INDIAN_SPU:
		abilityId = SPAB_MIRAGE_ARROW;
		break;
	}

	if( !abilityId )
		return;

	// mirage arrow has longer duration
	if( abilityId == SPAB_MIRAGE_ARROW )
		last_special_ability_start_date = info.game_date + SPECIAL_ABILITY_DURATION;
	else
		last_special_ability_start_date = info.game_date;
	special_abilty_id = abilityId;
}

