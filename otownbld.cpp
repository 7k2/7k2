//Filename    : OTOWNSE2.CPP
//Description : Object TownZone

#include <OVGA.H>
#include <OGAME.H>
#include <OINFO.H>
#include <OTOWN.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OTOWNRES.H>
#include <OCOLTBL.H>

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

