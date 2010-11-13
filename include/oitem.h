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

// Filename    : OITEM.H
// Description : item constant


#ifndef __OITEM_H
#define __OITEM_H

//------- Define struct item ----------//

class Skill;

struct Item
{
public:
	short	id;
	int	para;


public:
	Item()                               : id(0), para(0)     {}
	Item( short itemId, short itemPara ) : id(itemId), para(itemPara) {}

	void	set(short itemId, short itemPara) 	{ id = itemId; para = itemPara; }
	void	clear()										{ id = 0; }

	void	next_day();

	char *item_name();
	char *item_desc();
	int	cost();

	int	ability(int itemAbilityId);
	void	use_now();							// use the item now
	int	can_turn();
	int	can_turn_on();
	int	is_turn_on();
	void	turn_on();
	void	turn_off();
	int	can_use_manually();
	void	use_manually(int unitRecno, int targetBaseObjRecno=0, int para2=0);

	void 	init_random( int lowRare, int highRare, int unitId=0, Skill* =0);
	void	random_para();
};

//-------------------------------------//

#endif