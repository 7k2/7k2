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

// Filename    : OUN_ITEM.CPP
// Description : unit item functions

#include <ounit.h>
#include <oremote.h>
#include <oitemres.h>
#include <osite.h>
#include <ogame.h>

//------ Begin of function Unit::pick_item --------//

int Unit::pick_item( short itemId, int itemPara )
{
	if( item.id )
		return 0;

	// ##### begin Gilbert 6/1 #######//
	if( !item_res.can_pick(itemId, sprite_recno) )
	// ##### begin Gilbert 6/1 #######//
		return 0;

	item.set( itemId, itemPara );

	// ### begin Gilbert 4/11 #####//
	set_combat_level(-1);			// update any change because of combat level
	// ### end Gilbert 4/11 #####//

	// ##### begin Gilbert 23/1 ########//
	// ------- update win/lose condition on item taken -----//

	game.update_condition_item_acquired(itemId, nation_recno);
	// ##### end Gilbert 23/1 ########//
	return 1;
}
//------ End of function Unit::pick_item --------//


//------ Begin of function Unit::drop_item --------//

void Unit::drop_item(char remoteAction)
{
	if( !item.id )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_DROP_ITEM, sizeof(short) );
		// packet structure : <sprite recno>
		*shortPtr = sprite_recno;
		return;
	}

	//--------------------------------------//

	short xLoc, yLoc;

	// BUGHERE : how to avoid pick up again right after drop?

	if( get_cur_loc(xLoc, yLoc) )
	{
		for( int r = 1; r <= 6; ++r )
		{
			int xMin = MAX(xLoc-r,0);
			int xMax = xLoc+loc_width-1+r;
			if( xMax >= MAX_WORLD_X_LOC )
				xMax = MAX_WORLD_X_LOC-1;
			int yMin = MAX(yLoc-r,0);
			int yMax = yLoc+loc_height-1+r;
			if( yMax >= MAX_WORLD_Y_LOC )
				yMax = MAX_WORLD_Y_LOC-1;

			for( int y = yMin; y <= yMax; ++y )
			{
				for( int x = xMin; x <= xMax; ++x )
				{
					Location *locPtr = world.get_loc(x, y);
					if( locPtr->can_build_site() && site_array.add_site( x, y, SITE_ITEM, item.id, item.para ) )
					{
						item.clear();
						set_combat_level(-1);			// update any change because of combat level
						return;
					}
				}
			}
		}
	}
}
//------ End of function Unit::drop_item --------//
