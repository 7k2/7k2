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

// Filename    : OITEM.CPP
// Description : Item resource

#include <oitem.h>
#include <oitemres.h>
#include <itemid.h>
#include <all.h>
#include <oconfig.h>


//-------- begin of function Item::next_day-------//
//
void Item::next_day()
{
	item_res.next_day(*this);
}
//-------- end of function Item::next_day -------//


// ------- begin of function Item::item_name -----//
const char *Item::item_name()
{
	return item_res.item_name( id, para );
}
// ------- end of function Item::item_name -----//


// ------- begin of function Item::item_desc -----//
const char *Item::item_desc()
{
	return item_res.item_desc( id, para );
}
// ------- end of function Item::item_desc -----//

//-------- begin of function Item::ability-------//
//
int Item::ability( int itemAbilityId )
{
	return item_res.ability( *this, itemAbilityId );
}
//-------- end of function Item::ability -------//


//-------- begin of function Item::cost-------//
//
int Item::cost()
{
	return item_res.item_cost( id, para );
}
//-------- end of function Item::cost -------//


//-------- begin of function Item::use_now -------//
//
// call when an item is used, reduce energy or wear out
//
// if wear out, set itemId = 0, itemPara = 0;
//
void Item::use_now()
{
	item_res.use_now( *this );
}
//-------- end of function Item::use_now -------//


//-------- begin of function Item::can_turn -------//
//
// is an item has switch on and off usage
//
int Item::can_turn()
{
	return item_res.can_turn(*this);
}
//-------- end of function Item::can_turn -------//


//-------- begin of function Item::can_turn_on -------//
//
// always can_turn_off if can_turn() and is_turn_on();
//
int Item::can_turn_on()
{
	return item_res.can_turn_on(*this);
}
//-------- end of function Item::can_turn_on -------//


//-------- begin of function Item::is_turn_on -------//
//
int Item::is_turn_on()
{
	return item_res.is_turn_on(*this);
}
//-------- end of function Item::is_turn_on -------//


//-------- begin of function Item::turn_on -------//
//
void Item::turn_on()
{
	item_res.turn_on(*this);
}
//-------- end of function Item::turn_on -------//


//-------- begin of function Item::turn_off -------//
//
void Item::turn_off()
{
	item_res.turn_off(*this);
}
//-------- end of function Item::turn_off -------//


//-------- begin of function Item::can_use_manually -------//
//
int Item::can_use_manually()
{
	return item_res.can_use_manually(*this);
}
//-------- end of function Item::can_use_manually -------//


//-------- begin of function Item::use_manually -------//
//
void Item::use_manually(int unitRecno, int targetBaseObjRecno, int para2)
{
	item_res.use_manually(*this, unitRecno, targetBaseObjRecno, para2 );
}
//-------- end of function Item::use_manually -------//


//-------- begin of function Item::init_random ----------//
//
// Randomly initialize the item.
//
// <int> lowRare         lower limit of rareness
// <int> highRare        higher limit of rareness
// [int] unitId          unitId of the carrier, NULL if not carrier (default : 0)
// [Skill *]skillPtr     skill of the carrier, NULL if no carrier   (default : NULL)
//
void Item::init_random( int lowRare, int highRare, int unitId, Skill *skillPtr)
{
	short selectedItem=0;
	int 	match=0;

	err_when( !unitId && skillPtr || unitId && !skillPtr ); // unitId and skillPtr must be both null
	// or both non-null

	for( int i = 1; i <= ITEM_TYPE_COUNT; ++i )
	{
		// ITEM_SCOUT_SCOPE only useful in fog of war / blacken map game
		if( i == ITEM_SCOUT_SCOPE && !config.fog_of_war && config.explore_whole_map )
			continue;

		char rare = item_res.rareness(i);
		if( rare >= lowRare && rare <= highRare 
			&& (!unitId || item_res.can_equip(i, unitId, *skillPtr)) )
		{
			++match;

			if( misc.random(match) == 0 )
				selectedItem = i;
		}
	}

	err_when( match == 0 && selectedItem != 0 );

	id = selectedItem;

	random_para();
}
//-------- end of function Item::init_random ----------//


//-------- begin of function Item::random_para ----------//
//
// randomize the para
//
void Item::random_para()
{
	para = item_res.random_para( id, misc.rand() );
}
//-------- end of function Item::random_para ----------//

