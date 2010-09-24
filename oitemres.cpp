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

// Filename    : OITEMRES.CPP
// Description : Item resource


#include <OITEMRES.H>
#include <OITEM.H>
#include <ITEMID.H>
#include <ALL.H>
#include <OCOLTBL.H>
#include <OVGA.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OSPRTRES.H>
#include <OUNIT.H>
#include <OCONFIG.H>
#include <OSE.H>
#include <OPOWER.H>
#include <OTOWN.H>
#include <OT_ITEM.H>

//--------- define static variable ----------//

// String item_str;

//-------- define constant table ---------//
//
// do nore read item_info_array directly, some item may override the value in the table
//
static ItemInfo item_info_array[ITEM_TYPE_COUNT] = 
{
//	    item_id	       name			rare   cost		ability_type            value   para_type	      use    use_min recover  max  magic_id/para init_c/r  bmp name, idx, desc
	{ ITEM_SWORD,  "Berserker Sword", 0,   100, ITEM_ABILITY_MELEE_DAMAGE,    5, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "SWORD_2",  0, 0, 0, 0, "Close attack +5" },
	{ ITEM_BOW,   "Bow of Odysseus",  0,   100, ITEM_ABILITY_ARROW_DAMAGE,    5, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "BOW_1",    0, 0, 0, 0, "Range attack +5" },
	{ ITEM_ARMOR,  "Aegis Shield",    0,   100, ITEM_ABILITY_ARMOR,           5, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "SHIELD_1", 0, 0, 0, 0, "Defense +5"      },
	{ ITEM_COMBAT,    "Excalibur",    0,   200, ITEM_ABILITY_COMBAT_LEVEL,   30, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "SWORD_1",  0, 0, 0, 0, "Combat level +30" },
	{ ITEM_LEADER, "The Art of War",  2,   300, ITEM_ABILITY_SKILL_LEVEL,    20, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "BOOK_1",   0, 0, 0, 0, "Leadership +20" },
	{ ITEM_LIGHT_SWORD, "Raptorsclaw",0,   100, ITEM_ABILITY_ATTACK_SPEED,    2, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "SWORD_3",  0, 0, 0, 0, "Attack speed +2" },
	{ ITEM_REGENERATE,"Ring of Life", 1,   200, ITEM_ABILITY_RECOVERY,        4, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "RING_1",   0, 0, 0, 0, "Recover life" },
	{ ITEM_BOOTS,  "Boots of Hermes", 0,   150, ITEM_ABILITY_SPEED,           2, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "BOOT_1",   0, 0, 0, 0, "Speed +2" },
	{ ITEM_FORCE_FIELD, "Aegis Field",3,  500, ITEM_ABILITY_ABSORB_HIT,       1, ITEM_PARA_USE,    1,       0,      0,     0,     0,   0,   50,50, "WRIST_1",  0, 0, 0, 0, "Absorb hits" },
	{ ITEM_INVINCIBLE, "Staff of Invincibility",2,  350, ITEM_ABILITY_INVINCIBLE, 1,ITEM_PARA_USE_TIMED, 1, 0,      0,     0,     0,   0,   20,10, "STAFF_1",  0, 0, 0, 0, "Invincibility" },
	{ ITEM_DEC_LOYALTY, "Heartchanger", 1, 300, ITEM_ABILITY_DEC_LOYALTY,    40, ITEM_PARA_NONE,    0,      0,      0,     0,     0,   0,    0, 0, "HAT_1",    0, 0, 0, 0, "Decrease enemy's Loyalty" },
	{ ITEM_FREEZER,     "Cyrostaff",  2, 1000, ITEM_ABILITY_FREEZE,          20, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "STAFF_2",  0, 0, 0, 0, "Freeze enemy" },
	{ ITEM_SUMMON_SCROLL, "Heimdall's Horn", 1, 700, ITEM_ABILITY_SUMMON,     1, ITEM_PARA_USE_MANUAL, 1,   0,      0,     1,     0,   0,    1, 0, "HORN_1",   0, 0, 0, 0, "Summon fryhtan" },
	{ ITEM_INVISIBILITY, "Unseen Torque", 2, 500, ITEM_ABILITY_INVISIBLE,     1, ITEM_PARA_CHARGE_TIMED, 1, 2,      1,    50,     0,   0,   50, 0, "AMULET_1", 0, 0, 0, 0, "Invisibility" },
	{ ITEM_SCOUT_SCOPE, "Vistastaff", 0,  100, ITEM_ABILITY_SCOUT_RANGE,      2, ITEM_PARA_NONE,   0,       0,      0,     0,     0,   0,    0, 0, "STAFF_3",  0, 0, 0, 0, "Scouting range" },
	{ ITEM_BERSERK_POTION, "Cordial of Combat", 1, 250, ITEM_ABILITY_DAMAGE,   8, ITEM_PARA_CHARGE, 4,      8,      1,    20,     0,   0,   20, 0, "POTION_1", 0, 0, 0, 0, "Berserker damage" },
	{ ITEM_TRAIN_BOOK, "The Art of Training",3, 800, ITEM_ABILITY_TRAIN,    100, ITEM_PARA_ENHANCE,0,       0,      0,     0,     0,   0,    0, 0, "BOOK_3",   0, 0, 0, 0, "Increase training speed" },
	{ ITEM_MONSTER1_BOOK, "Kharshuf's Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER,20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    1, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Kharshuf" },
	{ ITEM_MONSTER2_BOOK, "Bregma's Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER,  20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    2, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Bregma" },
	{ ITEM_MONSTER3_BOOK, "Kerassos' Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER, 20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    3, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Kerassos" },
	{ ITEM_MONSTER4_BOOK, "Minotauros' Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER,20,ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    4, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Minotauros" },
	{ ITEM_MONSTER5_BOOK, "Grokken's Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER, 20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    5, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Grokken" },
	{ ITEM_MONSTER6_BOOK, "Ezpinez's Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER, 20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    6, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Ezpinez" },
	{ ITEM_MONSTER7_BOOK, "Exovum's Bane",3,1000, ITEM_ABILITY_ANTI_MONSTER,  20, ITEM_PARA_NONE,   0,      0,      0,     0,     0,   0,    7, 0, "STAFF_4",  0, 0, 0, 0, "Extra Damage against Exovum" },
	{ ITEM_UNIT_CHANGE_NATION, "Judas Ring",4,2000, ITEM_ABILITY_UNIT_BETRAY,  8, ITEM_PARA_USE_MANUAL, 1,  0,      0,     1,     0,   1,    1, 0, "RING_3",   0, 0, 0, 0, "Convert an opponent to your side" },
	{ ITEM_TOWN_CHANGE_NATION, "Judas Rod",4,3000, ITEM_ABILITY_TOWN_BETRAY,   8, ITEM_PARA_USE_MANUAL, 1,  0,      0,     1,     0,   2,    1, 0, "ROD_1",    0, 0, 0, 0, "Convert a town to your side" },
	{ ITEM_TOWN_FREE,     "Neutralizer",4,   2000, ITEM_ABILITY_TOWN_FREE,    8, ITEM_PARA_USE_MANUAL, 1,   0,      0,     1,     0,   2,    1, 0, "ROD_2",    0, 0, 0, 0, "Turn a town independent" },
	{ ITEM_TELEPORT,      "Telecordial",4,  500,ITEM_ABILITY_TELEPORT,         1, ITEM_PARA_USE_MANUAL, 1,  0,      0,     1,     0,   3,    1, 0, "POTION_2", 0, 0, 0, 0, "Teleport across the world" },
	{ ITEM_INVISIBILITY_SX,"Unseen Torque", 4, 200,ITEM_ABILITY_INVISIBLE,     1, ITEM_PARA_USE_TIMED, 1,   2,      0,   100,     0,   0,  100, 0, "AMULET_1", 0, 0, 0, 0, "Invisibility" },

//                name    rare      $    at ra df ck sk mg    init 
//	{ ITEM_SWORD, "Sword",    0,     5,    3, 0, 0, 0, 0, 0,   0, 0, "Close attack +3" },
//	{ ITEM_BOW,   "Bow",      0,     5,    0, 3, 0, 0, 0, 0,   0, 0, "Range attack +3" },
//	{ ITEM_ARMOR, "Armor",    0,     5,    0, 0, 3, 0, 0, 0,   0, 0, "Defense +3" },
//	{ ITEM_CROWN, "Crown",    2,   100,    0, 0, 0, 0,10, 0,   0, 0, "Leadership +10" },
};


//-------- begin of function ItemRes::ItemRes ----------//
//
ItemRes::ItemRes()
{
	init_flag = 0;
	// color_remap_table = NULL;
}
//-------- end of function ItemRes::ItemRes ----------//


//-------- begin of function ItemRes::~ItemRes ----------//
//
ItemRes::~ItemRes()
{
	deinit();
}
//-------- end of function ItemRes::ItemRes ----------//


//-------- begin of function ItemRes::init ----------//
//
void ItemRes::init()
{
	deinit();

	// load res_bitmap

   res_bitmap.init(DIR_RES"I_ITEMB.RES",1,0);       // 1-read into buffer
   res_icon.init(DIR_RES"I_ITEMC.RES",1,0);       // 1-read into buffer
	res_interface.init(DIR_RES"I_ITEMI.RES",1,0);       // 1-read into buffer
	res_unit_interface.init(DIR_RES"I_ITEMU.RES",1,0);       // 1-read into buffer
	
	// set bitmap index

	for( int i = 1; i <= ITEM_TYPE_COUNT; ++i )
	{
		item_info_array[i-1].bitmap_idx = res_bitmap.get_index(item_info_array[i-1].bitmap_name);
		item_info_array[i-1].icon_idx = res_icon.get_index(item_info_array[i-1].bitmap_name);
		item_info_array[i-1].interface_idx = res_interface.get_index(item_info_array[i-1].bitmap_name);
		item_info_array[i-1].unit_interface_idx = res_unit_interface.get_index(item_info_array[i-1].bitmap_name);

		err_when( item_info_array[i-1].rare < MIN_RARITY ||
					 item_info_array[i-1].rare > MAX_RARITY+3 );	// higher than MAX_RARITY for scenario only
	}

	// set color remap table, read from PAL_ITEM.RES

//	color_remap_table = (short *)mem_add(sizeof(short) * 0x100 );
//	RGBColor palBuf[0x100];
//	File palFile;
//	palFile.file_open(DIR_RES"PAL_ITEM.RES");
//	palFile.file_seek(8);               // bypass the header info
//	palFile.file_read(palBuf, sizeof(palBuf));
//	palFile.file_close();
//	for( i = 0; i < 0x100; ++i )
//	{
//		color_remap_table[i] = vga.make_pixel( palBuf );
//	}

	init_flag = 1;
}
//-------- end of function ItemRes::ItemRes ----------//


//-------- begin of function ItemRes::deinit ----------//
//
void ItemRes::deinit()
{
	if( init_flag )
	{
//		mem_del( color_remap_table );

		res_bitmap.deinit();
		res_icon.deinit();
		res_interface.deinit();
		res_unit_interface.deinit();

		init_flag = 0;
	}
}
//-------- end of function ItemRes::deinit ----------//


//-------- begin of function ItemRes::item_name ----------//
//
char *ItemRes::item_name( short itemId, int itemPara )
{
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	char* formatStr = text_item.str_item_name( itemId );
//	switch( itemId )
//	{
//	case 0:
//		return "";			// empty string
//	}
	return formatStr;
	// return item_info_array[itemId-1].item_name;
}
//-------- end of function ItemRes::item_name ----------//


//-------- begin of function ItemRes::item_desc ----------//
//
char *ItemRes::item_desc( short itemId, int itemPara )
{
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	char* formatStr = text_item.str_item_desc( itemId );
//	switch( itemId )
//	{
//	case 0:
//		return "";			// empty string
//	}
	return formatStr;
	// return item_info_array[itemId-1].item_desc;
}
//-------- end of function ItemRes::item_desc ----------//


//-------- begin of function ItemRes::ability ----------//
//
int ItemRes::ability( Item &item, int itemAbilityId )
{
	if( !item.id )
		return 0;

	// if item.para is zero, then it is for testing item ability
	// therefore in this case, return the ability (>0) even though
	// the energy is not enough

	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	// ---- check if it is using -------//

	int enhancement = 0;
	int disableFlag = 0;
	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
		break;

	case ITEM_PARA_ENHANCE:
		enhancement += item.para;
		break;

	case ITEM_PARA_USE:
	case ITEM_PARA_CHARGE:
		if( item.para < itemInfo->para_use_limit )
			disableFlag = 1;
		break;

	case ITEM_PARA_USE_MANUAL:
		break;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		if( item.para > 0 )		// positive mean not using
			disableFlag = 1;
		break;
	}

	if( disableFlag )
		return 0;

	//----- return default value value ------//

	if( itemInfo->ability_type == itemAbilityId )
	{
		return itemInfo->ability_value + enhancement;
	}
	else if( itemInfo->ability_type == ITEM_ABILITY_DAMAGE && 
		(itemAbilityId == ITEM_ABILITY_MELEE_DAMAGE || itemAbilityId == ITEM_ABILITY_ARROW_DAMAGE) )
	{
		return itemInfo->ability_value + enhancement;
	}

	return 0;
}
//-------- end of function ItemRes::ability ----------//


//-------- begin of function ItemRes::item_cost ----------//
//
int ItemRes::item_cost( short itemId, int itemPara )
{
	if( !itemId )
		return 0;

	err_when( itemId > ITEM_TYPE_COUNT );

	return item_info_array[itemId-1].cost;
}
//-------- end of function ItemRes::item_cost ----------//


// -------- begin of function ItemRes::can_pick ----------//
//
// unitRecno may not available, eg soldier in camp, use unitId
//
// ####### begin Gilbert 25/5 #########//
int ItemRes::can_pick( short itemId, int unitRecno, int unitId )
{
	if( !itemId )
		return 0;

	err_when( itemId > ITEM_TYPE_COUNT );

	if( !unitId && unitRecno )
		unitId = unit_array[unitRecno]->unit_id;

	err_when( !unitId );		// cannot be both null

	UnitInfo *unitInfo = unit_res[unitId];

	if( !unitInfo->race_id || unitId == UNIT_WAGON )	// only human or monster can equip
		return 0;

	if( unitInfo->is_civilian )				// civilian cannot pick item
		return 0;

	return 1;
}
// ####### end Gilbert 25/5 #########//
// -------- end of function ItemRes::can_pick ----------//


// -------- begin of function ItemRes::can_equip ----------//
//
int ItemRes::can_equip( short itemId, int unitRecno )
{
	Unit *unitPtr = unit_array[unitRecno];
	return can_equip( itemId, unitPtr->unit_id, unitPtr->skill );
}
// -------- end of function ItemRes::can_equip ----------//


// -------- begin of function ItemRes::can_equip ----------//
//
int ItemRes::can_equip( short itemId, int unitId, Skill &skill )
{
	if( !itemId )
		return 0;

	if( !unit_res[unitId]->race_id || unitId == UNIT_WAGON )		// only human or monster can equip
		return 0;

	err_when( itemId > ITEM_TYPE_COUNT );

	// if an item has ITEM_ABILITY_ARROW_DAMAGE but no ITEM_ABILITY_DAMAGE
	// it must be wear by an archer unit

	Item tempItem( itemId, 0 );
	if( ability( tempItem, ITEM_ABILITY_ARROW_DAMAGE) > 0
		&& ability( tempItem, ITEM_ABILITY_DAMAGE) == 0 )
	{
		// find unit attack record to see if any range attack

		int canRangeAttack = 0;
		UnitInfo *unitInfo = unit_res[unitId];
		for( int curAttack = unitInfo->attack_count-1; curAttack >= 0; --curAttack )
		{
			AttackInfo *attackInfo = 
				unit_res.get_attack_info( unitInfo->first_attack + curAttack );
			if( attackInfo->attack_range > 1 
				&& skill.actual_skill_level(NULL) >= attackInfo->combat_level )
			{
				canRangeAttack = 1;
			}
		}

		if( !canRangeAttack )
			return 0;
	}

	return 1;
}
// -------- end of function ItemRes::can_equip ----------//


// -------- begin of function ItemRes::rareness ----------//
char ItemRes::rareness( short itemId )
{
	if( !itemId )
		return 0;

	err_when( itemId > ITEM_TYPE_COUNT );

	return item_info_array[itemId-1].rare;
}
// -------- End of function ItemRes::rareness ----------//


// -------- begin of function ItemRes::random_para  ----------//
int ItemRes::random_para( short itemId, int rnd )
{
	if( !itemId )
		return 0;

	err_when( itemId > ITEM_TYPE_COUNT );

	if( item_info_array[itemId-1].init_para_random > 0 )
	{
		return item_info_array[itemId-1].init_para_const
			+ rnd % item_info_array[itemId-1].init_para_random;
	}
	else
	{
		return item_info_array[itemId-1].init_para_const;
	}
}
// -------- begin of function ItemRes::random_para  ----------//


// -------- begin of function ItemRes::use_now  ----------//
//
void ItemRes::use_now( Item &item )
{
	if( !item.id )
		return;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
	case ITEM_PARA_ENHANCE:
		// no action
		break;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		// no action, because next_day reduce the charge
		break;

	case ITEM_PARA_USE:
	case ITEM_PARA_USE_MANUAL:
		item.para -= itemInfo->para_use_rate;

		// break if drop below zero
		if( item.para <= 0 )
		{
			item.para = 0;
			item.clear();
		}
		break;

	case ITEM_PARA_CHARGE:
		err_when( item.para < itemInfo->para_use_limit );
		item.para -= itemInfo->para_use_limit;
		break;

	default:
		err_here();
	}
}
// -------- end of function ItemRes::use_now  ----------//


// -------- begin of function ItemRes::next_day  ----------//
//
void ItemRes::next_day( Item &item)
{
	if( !item.id )
		return;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_USE_TIMED:
		if( item.para < 0 )		// turned_on increase para
		{
			// keep in mind para is negative and increasing to zero
			item.para += itemInfo->para_use_rate;

			// break if use up
			if( item.para >= 0 )
			{
				item.para = 0;
				item.clear();
			}
		}
		break;

	case ITEM_PARA_CHARGE_TIMED:
		// err_when( item.para == 0 );	// not allow to be zero, avoid at all cost
		if( item.para < 0 )
		{
			// keep in mind para is negative and increasing to zero
			item.para += itemInfo->para_use_rate;

			// turn off if use up
			if( item.para >= 0 )
			{
				item.para = 0;
				item.turn_off();
			}
		}
		else if( item.para >= 0 )
		{
			// recharging
			item.para += itemInfo->para_recover_rate;
			if( item.para > itemInfo->para_max )
				item.para = itemInfo->para_max;
		}
		break;

	case ITEM_PARA_CHARGE:
		// recharge
		if( item.para < itemInfo->para_max )
		{
			item.para += itemInfo->para_recover_rate;
			if( item.para > itemInfo->para_max )
				item.para = itemInfo->para_max;
		}
		break;
	}
}
// -------- end of function ItemRes::next_day  ----------//


// -------- begin of function ItemRes::can_turn  ----------//
//
int ItemRes::can_turn( Item &item )
{
	if( !item.id )
		return 0;
	err_when( item.id > ITEM_TYPE_COUNT );

	switch( item_info_array[item.id-1].para_type )
	{
	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		return 1;
	}
	return 0;
}
// -------- end of function ItemRes::can_turn  ----------//


// -------- begin of function ItemRes::can_turn_on  ----------//
//
int ItemRes::can_turn_on( Item &item )
{
	if( !item.id )
		return 0;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
	case ITEM_PARA_ENHANCE:
	case ITEM_PARA_USE:
	case ITEM_PARA_USE_MANUAL:
	case ITEM_PARA_CHARGE:
		return 0;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		return item.para >= itemInfo->para_use_limit;	// +ve mean charging

	default:
		err_here();
		return 0;
	}
}
// -------- end of function ItemRes::can_turn_on  ----------//


// -------- begin of function ItemRes::is_turn_on  ----------//
//
int ItemRes::is_turn_on( Item &item )
{
	if( !item.id )
		return 0;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
	case ITEM_PARA_ENHANCE:
	case ITEM_PARA_USE:
	case ITEM_PARA_USE_MANUAL:
	case ITEM_PARA_CHARGE:
		return 0;
		break;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		return item.para < 0;

	default:
		err_here();
		return 0;
	}
}
// -------- end of function ItemRes::is_turn_on  ----------//


// -------- begin of function ItemRes::turn_on  ----------//
//
void ItemRes::turn_on( Item &item )
{
	if( !item.id )
		return;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
	case ITEM_PARA_ENHANCE:
	case ITEM_PARA_USE:
	case ITEM_PARA_USE_MANUAL:
	case ITEM_PARA_CHARGE:
		break;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		if( item.para > 0 )
			item.para = -item.para;
		break;

	default:
		err_here();
	}
}
// -------- end of function ItemRes::turn_on  ----------//


// -------- begin of function ItemRes::turn_off ----------//
//
void ItemRes::turn_off( Item &item )
{
	if( !item.id )
		return;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	switch( itemInfo->para_type )
	{
	case ITEM_PARA_NONE:
	case ITEM_PARA_ENHANCE:
	case ITEM_PARA_USE:
	case ITEM_PARA_USE_MANUAL:
	case ITEM_PARA_CHARGE:
		break;

	case ITEM_PARA_USE_TIMED:
	case ITEM_PARA_CHARGE_TIMED:
		if( item.para < 0 )
			item.para = -item.para;
		break;

	default:
		err_here();
	}
}
// -------- end of function ItemRes::turn_off ----------//


// -------- begin of function ItemRes::can_use_manually ----------//
//
// return 0, cannot use manually
// return -1, can use manually no need to pass parameter
// return COMMAND_USE_ITEM_ON_UNIT, if cast on unit
//	return COMMAND_USE_ITEM_ON_TOWN, if cast on town
//
int ItemRes::can_use_manually( Item &item)
{
	if( !item.id )
		return 0;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );
	err_when( COMMAND_USE_ITEM_ON_UNIT == 1 );
	err_when( COMMAND_USE_ITEM_ON_TOWN == 1 );

	if( itemInfo->para_type == ITEM_PARA_USE_MANUAL )
	{
		switch( itemInfo->magic_para )
		{
		case 0:			// parameter
			return -1;
		case 1:
			return COMMAND_USE_ITEM_ON_UNIT;
		case 2:
			return COMMAND_USE_ITEM_ON_TOWN;
		case 3:
			return COMMAND_USE_ITEM_ON_SPACE;
		default:
			err_here();
		}
	}

	return 0;
}
// -------- end of function ItemRes::can_use_manually ----------//


// -------- begin of function ItemRes::item_bitmap ----------//
// bitmap on the map
char *ItemRes::item_bitmap( short itemId )
{
	if( itemId == 0 )
		return NULL;
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	return res_bitmap.get_data( item_info_array[itemId-1].bitmap_idx );
}
// -------- end of function ItemRes::item_bitmap ----------//


// -------- begin of function ItemRes::item_icon ----------//
// bitmap on interface area
char *ItemRes::item_icon( short itemId )
{
	if( itemId == 0 )
		return NULL;
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	return res_icon.get_data( item_info_array[itemId-1].icon_idx );
}
// -------- end of function ItemRes::item_icon ----------//

// -------- begin of function ItemRes::item_interface ----------//
// bitmap on interface area
char *ItemRes::item_interface( short itemId )
{
	if( itemId == 0 )
		return NULL;
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	return res_interface.get_data( item_info_array[itemId-1].interface_idx );
}
// -------- end of function ItemRes::item_interface ----------//

// -------- begin of function ItemRes::item_unit_interface ----------//
// bitmap on unit_interface area
char *ItemRes::item_unit_interface( short itemId )
{
	if( itemId == 0 )
		return NULL;
	err_when( itemId < 0 || itemId > ITEM_TYPE_COUNT );

	return res_unit_interface.get_data( item_info_array[itemId-1].unit_interface_idx );
}
// -------- end of function ItemRes::item_unit_interface ----------//

//---- Begin of function ItemRes::random_item_id ----//
//
// Randomly return a item id. that match the required rarity.
//
int ItemRes::random_item_id(int lowRare, int highRare)
{
	short selectedItem=0;
	int 	match=0;

	for( int i = 1; i <= ITEM_TYPE_COUNT; ++i )
	{
		// ITEM_SCOUT_SCOPE only useful in fog of war / blacken map game
		if( i == ITEM_SCOUT_SCOPE && !config.fog_of_war && config.explore_whole_map )
			continue;

		char rare = rareness(i);
		if( rare >= lowRare && rare <= highRare )
		{
			++match;

			if( m.random(match) == 0 )
				selectedItem = i;
		}
	}

	return selectedItem;
}

//---- End of function ItemRes::random_item_id ----//

// -------- begin of function ItemRes::use_manually ----------//
//
void ItemRes::use_manually( Item &item, int unitRecno, int targetBaseObjRecno, int para2)
{
	if( !item.id )
		return;
	err_when( item.id > ITEM_TYPE_COUNT );

	ItemInfo *itemInfo = item_info_array + item.id - 1;
	err_when( itemInfo->item_id != item.id );

	if( itemInfo->para_type == ITEM_PARA_USE_MANUAL )
	{
		if( unit_array.is_deleted(unitRecno) )
			return;

		Unit *unitPtr = unit_array[unitRecno];

		if( !unitPtr->is_visible() )
			return;

		switch( item.id )
		{
		case ITEM_SUMMON_SCROLL:
			{
				// create a random monster

				int summonUnitId = monster_res[m.random(MAX_MONSTER_TYPE) + 1]->unit_id;
				SpriteInfo *summonSpriteInfo = sprite_res[unit_res[summonUnitId]->sprite_id];

				for( int r = 0; r <= 2; ++r )
				{
					int xLoc1 = unitPtr->obj_loc_x1() - summonSpriteInfo->loc_width - r;
					int yLoc1 = unitPtr->obj_loc_y1() - summonSpriteInfo->loc_height - r;
					int xLoc2 = unitPtr->obj_loc_x2() + summonSpriteInfo->loc_width + r;
					int yLoc2 = unitPtr->obj_loc_y2() + summonSpriteInfo->loc_height + r;

					if( world.locate_space( xLoc1, yLoc1, xLoc2, yLoc2,
						summonSpriteInfo->loc_width, summonSpriteInfo->loc_height) )
					{
						int summonRecno = unit_array.add_unit( summonUnitId, unitPtr->nation_recno, RANK_SOLDIER, 100, xLoc1, yLoc1 );
						if( summonRecno )
						{
							// ####### begin Gilbert 23/2 #########//
							Unit *summonUnit = unit_array[summonRecno];
							// ------- use monster level to determinte combat skill ------//

							int monsterLevel = monster_res[summonUnit->monster_id()]->level;
							err_when( monsterLevel < 3 || monsterLevel > 9 );	// tuned for level between 3 and 9
							int combatSkill = (7 * MAX_COMBAT_TRAIN + m.random(MAX_COMBAT_TRAIN)) / (5 + monster_res[summonUnit->monster_id()]->level);
							combatSkill = min( combatSkill, MAX_COMBAT_TRAIN);
							summonUnit->set_combat_level( combatSkill );
							summonUnit->skill.set_skill_level(CITIZEN_SKILL_LEVEL);
							// ####### end Gilbert 19/2 #########//
							use_now(item);
							break;
						}
					}
				}

				// ####### begin Gilbert 5/3 #########//
				if( r > 2 && unitPtr->is_own() )		// failed
				{
					// failed sound effect
					se_ctrl.immediate_sound("TURN_OFF");
				}
				// ####### end Gilbert 5/3 #########//
			}
			break;

		case ITEM_UNIT_CHANGE_NATION:
			{
				// change unit to my nation

				Unit *targetUnit;
				if( !base_obj_array.is_deleted(targetBaseObjRecno)
					&& (targetUnit = base_obj_array[targetBaseObjRecno]->cast_to_Unit() )
					&& targetUnit->is_visible()
					&& targetUnit->rank_id != RANK_KING
					&& targetUnit->nation_recno != 0 && unitPtr->nation_recno != 0
					&& !targetUnit->is_nation(unitPtr->nation_recno)
					&& unitPtr->area_distance(targetUnit) <= 8
					&& targetUnit->betray(unitPtr->nation_recno) )
				{
					use_now(item);
				}
				else
				{
					if( unitPtr->is_own() )		// failed
					{
						// failed sound effect
						se_ctrl.immediate_sound("TURN_OFF");
					}
				}
			}
			break;

		case ITEM_TOWN_CHANGE_NATION:
			{
				// change town to our nation

				Town *targetTown;
				if( !base_obj_array.is_deleted(targetBaseObjRecno)
					&& (targetTown = base_obj_array[targetBaseObjRecno]->cast_to_Town() )
					&& !targetTown->under_construction
					&& unitPtr->nation_recno != 0
					&& targetTown->nation_recno != unitPtr->nation_recno
					&& unitPtr->area_distance(targetTown) <= 8
					&& (targetTown->surrender(unitPtr->nation_recno), targetTown->nation_recno == unitPtr->nation_recno) )
				{
					use_now(item);
				}
				else
				{
					if( unitPtr->is_own() )
					{
						// failed sound effect
						se_ctrl.immediate_sound("TURN_OFF");
					}
				}
			}
			break;

		case ITEM_TOWN_FREE:
			{
				// change town to independent nation

				Town *targetTown;
				if( !base_obj_array.is_deleted(targetBaseObjRecno)
					&& (targetTown = base_obj_array[targetBaseObjRecno]->cast_to_Town() )
					&& !targetTown->under_construction
					&& targetTown->nation_recno != 0
					&& unitPtr->area_distance(targetTown) <= 8
					&& (targetTown->set_nation(0), targetTown->nation_recno == 0) )
				{
					use_now(item);
				}
				else
				{
					if( unitPtr->is_own() )
					{
						// failed sound effect
						se_ctrl.immediate_sound("TURN_OFF");
					}
				}
			}
			break;

		case ITEM_TELEPORT:
			{
				// targetBaseObjRecno is xLoc,
				// para2 is yLoc
				if( unitPtr->teleport( targetBaseObjRecno, para2 ) )
				{
					use_now(item);
				}
				else
				{
					if( unitPtr->is_own() )
					{
						// failed sound effect
						se_ctrl.immediate_sound("TURN_OFF");
					}
				}
			}
			break;

		default:
			err_here();
		}
	}
}
// -------- end of function ItemRes::use_manually ----------//
