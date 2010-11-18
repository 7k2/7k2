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

// Filename    : ITEMID.H
// Description : header file of item id


#ifndef __ITEMID_H
#define __ITEMID_H


//------- Define constant ---------//

enum		// item name
{
	ITEM_SWORD = 1,
	ITEM_BOW,
	ITEM_ARMOR,
	ITEM_COMBAT,
	ITEM_LEADER,
	ITEM_LIGHT_SWORD,
	ITEM_REGENERATE,
	ITEM_BOOTS,
	ITEM_FORCE_FIELD,
	ITEM_INVINCIBLE,
	ITEM_DEC_LOYALTY,
	ITEM_FREEZER,
	ITEM_SUMMON_SCROLL,
	ITEM_INVISIBILITY,
	ITEM_SCOUT_SCOPE,
	ITEM_BERSERK_POTION,
	ITEM_TRAIN_BOOK,
	ITEM_MONSTER1_BOOK,
	ITEM_MONSTER2_BOOK,
	ITEM_MONSTER3_BOOK,
	ITEM_MONSTER4_BOOK,
	ITEM_MONSTER5_BOOK,
	ITEM_MONSTER6_BOOK,
	ITEM_MONSTER7_BOOK,
	ITEM_UNIT_CHANGE_NATION,
	ITEM_TOWN_CHANGE_NATION,
	ITEM_TOWN_FREE,		// change to independent
	ITEM_TELEPORT,
	ITEM_INVISIBILITY_SX,

	ITEM_TYPE_MAX,
	ITEM_TYPE_COUNT = ITEM_TYPE_MAX - 1,
};


enum		// item ability
{
	// attack ability
	ITEM_ABILITY_MELEE_DAMAGE = 1,
	ITEM_ABILITY_ARROW_DAMAGE,
	ITEM_ABILITY_DAMAGE,
	ITEM_ABILITY_ATTACK_SPEED,
	ITEM_ABILITY_DEC_LOYALTY,
	ITEM_ABILITY_FREEZE,
	ITEM_ABILITY_ANTI_MONSTER,

	// defense ability
	ITEM_ABILITY_ARMOR,
	ITEM_ABILITY_ABSORB_HIT,
	ITEM_ABILITY_RECOVERY,
	ITEM_ABILITY_INVINCIBLE,

	// movement ability
	ITEM_ABILITY_SPEED,
	ITEM_ABILITY_INVISIBLE,
	ITEM_ABILITY_SCOUT_RANGE,

	// attribute 
	ITEM_ABILITY_COMBAT_LEVEL,
	ITEM_ABILITY_SKILL_LEVEL,
	ITEM_ABILITY_TRAIN,

	// magic/summon
	ITEM_ABILITY_SUMMON,
	ITEM_ABILITY_UNIT_BETRAY,
	ITEM_ABILITY_TOWN_BETRAY,
	ITEM_ABILITY_TOWN_FREE,
	ITEM_ABILITY_TELEPORT,
};

enum		// item para_type
{
	ITEM_PARA_NONE,		// ignore para
	ITEM_PARA_ENHANCE,	// increase ability
	ITEM_PARA_USE,			// unrechargable use, clear itself when para <= 0
	ITEM_PARA_USE_MANUAL,	// manually use item
	ITEM_PARA_CHARGE,		// rechargeable, consume when use, recover every day
	ITEM_PARA_USE_TIMED,		// before use, para is positive. When start using, para is oppositive negative, and increasing by para_value1.
	ITEM_PARA_CHARGE_TIMED,		// similar to USE_TIME_LIMIT, but recover when not using
};


#endif
