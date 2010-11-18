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

//Filename    : OTOWNSE2.CPP
//Description : Object TownZone

#include <ovga.h>
#include <ogame.h>
#include <oinfo.h>
#include <otown.h>
#include <oworld.h>
#include <oworldmt.h>
#include <otownres.h>
#include <ocoltbl.h>

//------- Begin of function TownBuild::draw -----------//
//
// Draw the town building on the zoom map
//
// <int> curX, curY - the absolute base (center-bottom) coordination
//										of the building.
//
// <int> selectedFlag - whether the town of this building is currently selected or not.
//
void TownBuild::draw(int townRecno, int curX, int curY, int constructionCompletionPercent)
{
	//------- get the color remap table for this sprite ------//

	Town* townPtr = town_array[townRecno];

	short* colorRemapTable = game.get_color_remap_table(townPtr->nation_recno, town_array.selected_recno==townRecno);
	
	colorRemapTable = town_res.calc_color_remap_table( colorRemapTable, (float) constructionCompletionPercent / 100);
	
	curX = curX - loc_width * LOCATE_WIDTH /2;
	curY = curY - loc_height * LOCATE_HEIGHT /2;
	world.zoom_matrix->put_bitmap_offset( curX, curY, townPtr->altitude,
		bitmap_ptr, offset_x, offset_y,
		colorRemapTable, 0, 1);
}
//--------- End of function TownBuild::draw -----------//

