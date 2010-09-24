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

// Filename    : OITEMRES.H
// Description : Item resource

#ifndef __OITEMRES_H
#define __OITEMRES_H

#include <ORESX.H>

// ------- define constant -------//

enum { MIN_RARITY = 0,
		 MAX_RARITY = 3 };

#define ITEM_ICON_WIDTH 40
#define ITEM_ICON_HEIGHT 46
#define ITEM_UNIT_ICON_WIDTH 22
#define ITEM_UNIT_ICON_HEIGHT 20

//--------- define struct ItemInfo ----------//

struct ItemInfo
{
	enum { ITEM_NAME_LEN = 25 };

	short item_id;
	char	item_name[ITEM_NAME_LEN+1];

	char	rare;				// 0=ordinary, 1+ rare
	long	cost;				// cost added to inn hire cost

	int	ability_type;
	int	ability_value;

	int	para_type;
	int	para_use_rate;
	int	para_use_limit;
	int	para_recover_rate;
	int	para_max;
	int	magic_id;		// cast magic
	int	magic_para;
	int	init_para_const;
	int	init_para_random;

	char	*bitmap_name;
	int	bitmap_idx;
	int	icon_idx;
	int	interface_idx;
	int	unit_interface_idx;
	char	*item_desc;
};

struct Item;
class Skill;

//--------- define class ItemRes ----------//

class ItemRes
{
public:
	int	init_flag;

	ResourceIdx res_bitmap;
	ResourceIdx res_icon;
	ResourceIdx res_interface;
	ResourceIdx res_unit_interface;

public:
	ItemRes();
	~ItemRes();

	void	init();
	void	deinit();

	int 	random_item_id(int lowRare, int highRare);

	// -------- functions on each item --------//

	char *item_name( short itemId, int itemPara );
	char *item_desc( short itemId, int itemPara );
	int	item_cost( short itemId, int itemPara );

	int	ability( Item &, int itemAbilityId);
	void	use_now( Item & );
	void	next_day( Item &);
	int	can_turn( Item &);
	int	can_turn_on( Item &);
	int	is_turn_on( Item &);
	void	turn_on( Item &);
	void	turn_off( Item &);
	int	can_use_manually( Item &);
	void	use_manually( Item &, int unitRecno, int targetBaseObjRecno=0, int para2=0);

	int	can_pick( short itemId, int unitRecno, int unitId=0 );
	int	can_equip( short itemId, int unitRecno );
	int	can_equip( short itemId, int unitId, Skill & );
	char	rareness( short itemId );
	int	random_para( short itemId, int random);

	char  *item_bitmap( short itemId );		// bitmap on map
	char  *item_icon( short itemId );		// bitmap on interface area
	char  *item_interface( short itemId );	// bitmap on interface area
	char  *item_unit_interface( short itemId );     // bitmap on unit icon
};

extern ItemRes item_res;

//--------------------------------------------//

#endif


