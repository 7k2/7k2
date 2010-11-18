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

// Filename    : OSEDIT_M.CPP
// Description : Scenario editor misc mode


#include <OSEDITOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OVBROWIF.H>
#include <OIMGRES.H>
#include <ORACERES.H>
#include <OROCKRES.H>
#include <OROCK.H>
#include <OINFO.H>
#include <OPOWER.H>
#include <OWORLDMT.H>
#include <OSITE.H>
#include <OITEM.H>
#include <OITEMRES.H>
#include <ITEMID.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OPLANT.H>
#include <OT_SEDIT.H>



// ------- define constant ----------//

#define ROCK_TYPES 4
static char rock_type_char[ROCK_TYPES] = { 'R', 'D', 'E', 'P' };
// static char *rock_type_str[ROCK_TYPES] = { "Rock", "Dirt", "Flat Rock", "Lake" };
// static char *site_type_str[MAX_SITE_TYPE] = { "Raw", "Power",  "Coins", "Item", "Weapon" };

#define GOLD_COIN_AMOUNT_COUNT 20
static long gold_coin_amount_array[GOLD_COIN_AMOUNT_COUNT] = 
{
	500, 600, 700, 800, 900,
	1000, 1500, 2000, 2500, 3000,
	3500, 4000, 4500, 5000, 5500,
	6000, 7000, 8000, 9000, 10000,
};

#define PLANT_SIZE_MAX 4
// static char *plant_size_str[PLANT_SIZE_MAX] = { "1", "2", "3", "4" };

static int last_rock_or_site;
static int last_rock_type;
static int last_site_type;

// ------- define static variable --------------//

static ButtonGroup button_rock_or_site(3);	// 0=rock 1=site, 2=plant
static ButtonGroup button_rock_type(ROCK_TYPES);		// rock, hill, dirt, blocking dirt
static ButtonGroup button_site_type(MAX_SITE_TYPE);		// SITE_RAW, SITE_SCROLL, SITE_GOLD_COIN, SITE_ITEM, 
static ButtonGroup button_plant_size(PLANT_SIZE_MAX);

static VBrowseIF browse_rock;

static VBrowseIF browse_raw;
static VBrowseIF browse_scroll;
static VBrowseIF browse_gold;
static VBrowseIF browse_item;
static VBrowseIF browse_tech;

static VBrowseIF browse_plant;

// ------- define static functions ------------//

static void disp_rock_rec(int recNo,int x,int y,int refreshFlag);

static void disp_raw_rec(int recNo,int x,int y,int refreshFlag);
static void disp_scroll_rec(int recNo,int x,int y,int refreshFlag);
static void disp_gold_rec(int recNo,int x,int y,int refreshFlag);
static void disp_item_rec(int recNo,int x,int y,int refreshFlag);
static void disp_tech_rec(int recNo,int x,int y,int refreshFlag);

static void disp_plant_rec(int recNo,int x,int y,int refreshFlag);


// ---- begin of function ScenarioEditor::init_misc_mode ------//
//
void ScenarioEditor::init_misc_mode()
{
	rock_or_site = 0;		// rock
	rock_type = 0;			// 0=rock(R), 1=dirt(D), 2=blocking dirt(E), 3=lake(P)
	site_type = 1;			// SITE_RAW, SITE_SCROLL, SITE_GOLD_COIN, SITE_ITEM
	plant_size = 0;

	// ------ divide rocks into four groups -------//

	err_when( sizeof(rock_browse_array) / sizeof(rock_browse_array[0]) < ROCK_TYPES );	// check enough storage

	// [0] is the whole array
	// [1], [2], [3] points to the inside of [0]
	rock_browse_array[0] = (short *)mem_add( rock_res.rock_info_count * sizeof(short) );
	int i, j;
	int k = 0;
	for( i = 0; i < ROCK_TYPES; ++i )
	{
		rock_browse_count[i] = 0;
		rock_browse_array[i] = rock_browse_array[0] + k;
		rock_browse_recno[i] = 1;

		char rockChar = rock_type_char[i];
		for( j = 1; j <= rock_res.rock_info_count; ++j )
		{
			if( rock_res.get_rock_info(j)->rock_type == rockChar )
			{
				err_when(k >= rock_res.rock_info_count ); 
				rock_browse_array[0][k] = j;
				k++;
				++rock_browse_count[i];
			}
		}
	}
	
	raw_browse_recno = 1;
	scroll_browse_recno = 1;
	gold_browse_recno = 1;
	item_browse_recno = 1;
	tech_browse_recno = 1;
	plant_browse_recno = 1;
	plant_size = 0;
}
// ---- end of function ScenarioEditor::init_misc_mode ------//


// ---- begin of function ScenarioEditor::deinit_misc_mode ------//
//
void ScenarioEditor::deinit_misc_mode()
{
	if( rock_browse_array[0] )		// [1], [2] and [3] use the same memory block as [0]
		mem_del(rock_browse_array[0]);
}
// ---- end of function ScenarioEditor::deinit_misc_mode ------//


// ---- begin of function ScenarioEditor::disp_misc_main ------//
//
void ScenarioEditor::disp_misc_main(int refreshFlag)
{
	// ------ terrain object or items button group --------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_rock_or_site[0].create_text( INFO_X1+5, INFO_Y1+5, INFO_X1+74, INFO_Y1+25, 
			text_editor.str_misc_mode(0), 0 ); // "Terrain", 0 );
		button_rock_or_site[1].create_text( INFO_X1+76, INFO_Y1+5, INFO_X1+145, INFO_Y1+25,
			text_editor.str_misc_mode(1), 0 ); // "Objects", 0 );
		button_rock_or_site[2].create_text( INFO_X1+147, INFO_Y1+5, INFO_X1+216, INFO_Y1+25,
			text_editor.str_misc_mode(2), 0 ); // "Plant", 0 );
	}

	font_zoom.put( INFO_X1+10, INFO_Y2-28, text_editor.str_double_left_add(), 0, INFO_X2-5);
	font_zoom.put( INFO_X1+10, INFO_Y2-14, text_editor.str_double_right_del(), 0, INFO_X2-5);

	button_rock_or_site.paint(rock_or_site);

	if( refreshFlag == INFO_REPAINT || last_rock_or_site != rock_or_site )
	{
		refreshFlag = INFO_REPAINT;
		last_rock_or_site = rock_or_site;
	}

	if( rock_or_site == 0)			// terrain objects mode 
	{
		// ------ put rock type buttons --------//

		if( refreshFlag == INFO_REPAINT || last_rock_type != rock_type )
		{
			refreshFlag = INFO_REPAINT;
			last_rock_type = rock_type;
		}

		if( refreshFlag == INFO_REPAINT )
		{
			int j = 0;
			for( int i = 0; i < ROCK_TYPES; ++i )
			{
//				button_rock_type[i].create_text( INFO_X1+5+i*50, INFO_Y1+30, INFO_X1+52+i*50, INFO_Y1+50,
//					text_editor.str_misc_mode(rock_or_site, i), 0 ); // rock_type_str[i], 0 );
				if( rock_browse_count[i] )
				{
					button_rock_type[i].create_text( INFO_X1+5, INFO_Y1+30+j*21, INFO_X2-5, INFO_Y1+49+j*21,
						text_editor.str_misc_mode(rock_or_site, i), 0 ); // rock_type_str[i], 0 );
					j++;
				}
				else
				{
					// no rock in this group, hide the button
					// by put the button at the first place and disable it
					button_rock_type[i].create_text( INFO_X1+5, INFO_Y1+30+0*21, INFO_X2-5, INFO_Y1+49+0*21,
						text_editor.str_misc_mode(rock_or_site, i), 0 ); // rock_type_str[i], 0 );
					button_rock_type[i].visible_flag = 0;
				}
			}
		}
		button_rock_type.paint(rock_type);

		// ------- display rock browser ----------//

		if( refreshFlag == INFO_REPAINT )
		{
			browse_rock.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-30,
				-1, 25, rock_browse_count[rock_type], disp_rock_rec );
			browse_rock.open(rock_browse_recno[rock_type]);
		}
		browse_rock.paint();
		// ####### begin Gilbert 22/2 ######//
		// browse_rock.refresh(rock_browse_recno[rock_type]);
		browse_rock.refresh();
		// ####### end Gilbert 22/2 ######//
	}
	else if( rock_or_site == 1)		// site mode
	{
		// ------ put site type buttons --------//

		if( refreshFlag == INFO_REPAINT || last_site_type != site_type )
		{
			refreshFlag = INFO_REPAINT;
			last_site_type = site_type;
		}

		if( refreshFlag == INFO_REPAINT )
		{
			for( int i = 0; i < MAX_SITE_TYPE; ++i )
			{
//				button_site_type[i].create_text( INFO_X1+5+i*42, INFO_Y1+30, INFO_X1+45+i*42, INFO_Y1+50,
//					text_editor.str_misc_mode(rock_or_site, i), 0 );
				button_site_type[i].create_text( INFO_X1+5, INFO_Y1+30+i*21, INFO_X2-5, INFO_Y1+49+i*21,
					text_editor.str_misc_mode(rock_or_site, i), 0 );
			}
		}
		button_site_type.paint(site_type-1);

		// ------ disp browser ----------//

		switch( site_type )
		{
		case SITE_RAW:
			if( refreshFlag == INFO_REPAINT )
			{
				browse_raw.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-50,
					-1, 36, MAX_RAW, disp_raw_rec );
				browse_raw.open( raw_browse_recno );
			}
			browse_raw.paint();
			// ####### begin Gilbert 22/2 ######//
			// browse_raw.refresh(raw_browse_recno);
			browse_raw.refresh();
			// ####### end Gilbert 22/2 ######//
			break;
		case SITE_SCROLL:
			if( refreshFlag == INFO_REPAINT )
			{
				browse_scroll.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-30,
					-1, 45, MAX_RACE, disp_scroll_rec );
				browse_scroll.open(scroll_browse_recno);
			}
			browse_scroll.paint();
			// ####### begin Gilbert 22/2 ######//
			// browse_scroll.refresh(scroll_browse_recno);
			browse_scroll.refresh();
			// ####### end Gilbert 22/2 ######//
			break;
		case SITE_GOLD_COIN:
			if( refreshFlag == INFO_REPAINT )
			{
				browse_gold.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-30,
					-1, 25, GOLD_COIN_AMOUNT_COUNT, disp_gold_rec );
				browse_gold.open(gold_browse_recno);
			}
			browse_gold.paint();
			// ####### begin Gilbert 22/2 ######//
			// browse_gold.refresh(gold_browse_recno);
			browse_gold.refresh();
			// ####### end Gilbert 22/2 ######//
			break;
		case SITE_ITEM:
			if( refreshFlag == INFO_REPAINT )
			{
				browse_item.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-30,
					-1, 25, ITEM_TYPE_COUNT, disp_item_rec );
				browse_item.open(item_browse_recno);
			}
			browse_item.paint();
			// ####### begin Gilbert 22/2 ######//
			// browse_item.refresh(item_browse_recno);
			browse_item.refresh();
			// ####### end Gilbert 22/2 ######//
			break;

		case SITE_WEAPON_BLUEPRINT:
			if( refreshFlag == INFO_REPAINT )
			{
				browse_tech.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-50,
					-1, 35, tech_res.tech_class(TECH_CLASS_MEGA_WEAPON)->tech_count, disp_tech_rec );
				browse_tech.open(tech_browse_recno);
			}
			browse_tech.paint();
			// ####### begin Gilbert 22/2 ######//
			// browse_tech.refresh(tech_browse_recno);
			browse_tech.refresh();
			// ####### end Gilbert 22/2 ######//
			break;
		default:
			err_here();
		}
	}
	else if( rock_or_site == 2)		// plant mode 
	{
		// ----- display plant browser -------//

		if( refreshFlag == INFO_REPAINT )
		{
			browse_plant.init( INFO_X1+5, INFO_Y1+140, INFO_X2-5, INFO_Y2-30,
				-1, 25, plant_res.plant_count, disp_plant_rec );
			browse_plant.open(plant_browse_recno);
		}
		browse_plant.paint();
		// ####### begin Gilbert 22/2 ######//
		// browse_plant.refresh(plant_browse_recno);
		browse_plant.refresh();
		// ####### end Gilbert 22/2 ######//

		for( int i = 0; i < PLANT_SIZE_MAX; ++i )
		{
			if( refreshFlag == INFO_REPAINT )
			{
				button_plant_size[i].create_text( INFO_X1+5, INFO_Y1+30+i*21, INFO_X2-5, INFO_Y1+49+i*21,
					text_editor.str_misc_mode(rock_or_site, i), 0 );
			}

			button_plant_size[i].enable_flag = !browse_plant.none_record 
				&& plant_res[plant_browse_recno]->bitmap_count > i;
		}

		button_plant_size.paint();
	}
}
// ---- end of function ScenarioEditor::disp_misc_main ------//


// ---- begin of function ScenarioEditor::detect_misc_main ------//
//
void ScenarioEditor::detect_misc_main()
{
	int rc;

	// ------ detect terrain object or items button group --------//

	if( (rc = button_rock_or_site.detect()) >= 0 )
	{
		rock_or_site = rc;
		return;
	}

	if( rock_or_site == 0)			// terrain objects mode 
	{
		if( (rc = button_rock_type.detect()) >= 0 )
		{
			rock_type = rc;
			return;
		}

		// ###### begin Gilbert 22/2 ########//
		if( (rc = browse_rock.detect()) )
		{
//			rock_browse_recno[rock_type] = rc;
			rock_browse_recno[rock_type] = browse_rock.recno();
			return;
		}
		rock_browse_recno[rock_type] = browse_rock.recno();
		// ###### end Gilbert 22/2 ########//
	}
	else if( rock_or_site == 1)		// site mode
	{
		// ------ detect site type buttons --------//

		if( (rc = button_site_type.detect()) >= 0 )
		{
			site_type = 1+rc;
			return;
		}

		// ------ detect browser ----------//

		switch( site_type )
		{
		case SITE_RAW:
			if( (rc = browse_raw.detect()) )
			{
				// raw_browse_recno = rc;
				raw_browse_recno = browse_raw.recno();
				return;
			}
			raw_browse_recno = browse_raw.recno();
			break;

		case SITE_SCROLL:
			if( (rc = browse_scroll.detect()) )
			{
				// scroll_browse_recno = rc;
				scroll_browse_recno = browse_scroll.recno();
				return;
			}
			scroll_browse_recno = browse_scroll.recno();
			break;

		case SITE_GOLD_COIN:
			if( (rc = browse_gold.detect()) )
			{
				// gold_browse_recno = rc;
				gold_browse_recno = browse_gold.recno();
				return;
			}
			gold_browse_recno = browse_gold.recno();
			break;

		case SITE_ITEM:
			if( (rc = browse_item.detect()) )
			{
				// item_browse_recno = rc;
				item_browse_recno = browse_item.recno();
				return;
			}
			item_browse_recno = browse_item.recno();
			break;

		case SITE_WEAPON_BLUEPRINT:
			if( (rc = browse_tech.detect()) )
			{
				// tech_browse_recno = rc;
				tech_browse_recno = browse_tech.recno();
				return;
			}
			tech_browse_recno = browse_tech.recno();
			break;

		default:
			err_here();
		}
	}
	else if( rock_or_site == 2)		// plant mode
	{
		if( (rc = browse_plant.detect()) )
		{
			// plant_browse_recno = rc;
			plant_browse_recno = browse_plant.recno();
			if( plant_size >= plant_res[plant_browse_recno]->bitmap_count )
				plant_size = plant_res[plant_browse_recno]->bitmap_count-1;
			return;
		}
		plant_browse_recno = browse_plant.recno();
		if( plant_size >= plant_res[plant_browse_recno]->bitmap_count )
			plant_size = plant_res[plant_browse_recno]->bitmap_count-1;
	
		if( (rc = button_plant_size.detect()) >= 0 )
		{
			plant_size = rc;
			return;
		}
	}
}
// ---- end of function ScenarioEditor::detect_misc_main ------//


// ---- begin of function ScenarioEditor::disp_misc_view ------//
//
void ScenarioEditor::disp_misc_view()
{
	if( power.command_id )
		return;

	// display build marker

	int drawMarker = 0;
	int validAction = 0;
	int markerX1, markerY1, markerX2, markerY2;

	int xLoc, yLoc;

	if( rock_or_site == 0)			// terrain objects mode 
	{
		if( mouse.in_area(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) 
			&& world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
		{
			Location *locPtr = world.get_loc(xLoc, yLoc);

			// destruction marker if there is a rock/dirt

			if( locPtr->is_rock() || locPtr->has_dirt() )
			{
				Rock *rockPtr;
				if( locPtr->is_rock() )
					rockPtr = rock_array[locPtr->rock_array_recno()];
				else if( locPtr->has_dirt() )
					rockPtr = dirt_array[locPtr->dirt_recno()];
				else
				{
					err_here();
				}

				RockInfo *rockInfo = rock_res.get_rock_info(rockPtr->rock_recno);
				drawMarker = 1;
				validAction = 1;	// destruction marker
				markerX1 = rockPtr->loc_x;
				markerY1 = rockPtr->loc_y;
				markerX2 = rockPtr->loc_x + rockInfo->loc_width -1;
				markerY2 = rockPtr->loc_y + rockInfo->loc_height -1;
			}

			// construction marker

			else if( !browse_rock.none_record )
			{
				int rockId = rock_browse_array[rock_type][rock_browse_recno[rock_type]-1];
				RockInfo *rockInfo = rock_res.get_rock_info( rockId );

				markerX1 = xLoc; 
				markerY1 = yLoc;
				markerX2 = xLoc+rockInfo->loc_width-1;
				markerY2 = yLoc+rockInfo->loc_height-1;
				err_when( xLoc < 0 || yLoc < 0 );
				drawMarker = markerX2 < MAX_WORLD_X_LOC && markerY2 < MAX_WORLD_Y_LOC;

				if( rockInfo->rock_type == ROCK_BLOCKING_TYPE )
				{
					if( world.can_add_rock(xLoc, yLoc, xLoc+rockInfo->loc_width-1, yLoc+rockInfo->loc_height-1) )
						validAction = 1;
				}
				else 
				{
					if( world.can_add_dirt(xLoc, yLoc, xLoc+rockInfo->loc_width-1, yLoc+rockInfo->loc_height-1) )
						validAction = 1;
				}
			}
		}
	}

	else if( rock_or_site == 1 )	// site object
	{
		if( mouse.in_area(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) 
			&& world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL ) )
		{
			Location *locPtr = world.get_loc(xLoc, yLoc);

			// destruction marker if there is a site

			if( locPtr->has_site() )
			{
				drawMarker = 1;
				validAction = 1;
				Site *sitePtr = site_array[locPtr->site_recno()];
				markerX1 = sitePtr->map_x_loc;
				markerY1 = sitePtr->map_y_loc;
				markerX2 = sitePtr->map_x2;
				markerY2 = sitePtr->map_y2;
			}

			// construction marker

			else if( site_type == SITE_RAW && !browse_raw.none_record )
			{
				int rawId = browse_raw.recno();
				int siteWidth = raw_res[rawId]->map_loc_width;
				int siteHeight = raw_res[rawId]->map_loc_height;
				markerX1 = xLoc;
				markerY1 = yLoc;
				markerX2 = xLoc + siteWidth - 1;
				markerY2 = yLoc + siteHeight - 1;
				if( (drawMarker = markerX2 < MAX_WORLD_X_LOC && markerY2 < MAX_WORLD_Y_LOC) )
					validAction = world.can_build_site( xLoc, yLoc, siteWidth, siteHeight, INTER_PLACE_SPACE) ? 1 : 0;
				else
					validAction = 0;

			}
			else if( site_type == SITE_SCROLL && !browse_scroll.none_record 
				|| site_type == SITE_GOLD_COIN && !browse_gold.none_record
				|| site_type == SITE_ITEM && !browse_item.none_record 
				|| site_type == SITE_WEAPON_BLUEPRINT && !browse_tech.none_record )
			{
				drawMarker = 1;
				validAction = world.can_build_site(xLoc, yLoc, 1, 1, 0) ? 1 : 0;
				markerX1 = markerX2 = xLoc;
				markerY1 = markerY2 = yLoc;
			}
		}
	}
	else if( rock_or_site == 2)			// plant object
	{
		if( mouse.in_area(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) 
			&& world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &xLoc, &yLoc, NULL) )
		{
			Location *locPtr = world.get_loc(xLoc, yLoc);

			// destruction marker if there is a plant

			if( locPtr->is_plant() )
			{
				drawMarker = 1;
				validAction = 1;	// destruction marker
				markerX1 = markerX2 = xLoc;
				markerY1 = markerY2 = yLoc;
			}

			// construction marker

			else if( !browse_plant.none_record )
			{
				markerX1 = markerX2 = xLoc; 
				markerY1 = markerY2 = yLoc;
				drawMarker = 1;
				validAction = locPtr->can_add_plant();
			}
		}
	}


	// ------ draw marker --------//

	if( drawMarker)
		draw_marker(validAction, markerX1, markerY1, markerX2, markerY2 );
}
// ---- end of function ScenarioEditor::disp_misc_view ------//


// ---- begin of function ScenarioEditor::detect_misc_view ------//
//
int ScenarioEditor::detect_misc_view()
{
	if( power.command_id )
		return 0;

	if( rock_or_site == 0)			// terrain objects mode 
	{
		// ------- add rock --------//

		if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )		// left button, add rock
		{
			int xLoc, yLoc;
			if( !browse_rock.none_record 
				&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
			{
				Location *locPtr = world.get_loc(xLoc, yLoc);

				int rockId = rock_browse_array[rock_type][rock_browse_recno[rock_type]-1];
				RockInfo *rockInfo = rock_res.get_rock_info( rockId );
				if( rockInfo->rock_type == ROCK_BLOCKING_TYPE )
				{
					if( world.can_add_rock(xLoc, yLoc, xLoc+rockInfo->loc_width-1, yLoc+rockInfo->loc_height-1) )
						world.add_rock( rockId, xLoc, yLoc );
				}
				else 
				{
					if( world.can_add_dirt(xLoc, yLoc, xLoc+rockInfo->loc_width-1, yLoc+rockInfo->loc_height-1) )
						world.add_dirt( rockId, xLoc, yLoc );
				}
			}

			return 1;
		}

		// ------- remove rock -------//

		if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1) )		// right button, remove rock
		{
			int xLoc, yLoc;
			if( world.zoom_matrix->get_detect_location(mouse.click_x(1), mouse.click_y(1), &xLoc, &yLoc, NULL ) )
			{
				Location *locPtr = world.get_loc(xLoc, yLoc);
				if( locPtr->is_rock() )
				{
					world.remove_rock(locPtr->rock_array_recno());
				}
				else if( locPtr->has_dirt() )
				{
					world.remove_dirt(locPtr->dirt_recno());
				}
			}

			return 1;
		}
	}
	else if( rock_or_site == 1 )
	{

		// ------ add site ------//

		switch( site_type )
		{
		case SITE_RAW:
			if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )
			{
				int xLoc, yLoc;
				if( !browse_raw.none_record 
					&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
				{
					int siteWidth = raw_res[raw_browse_recno]->map_loc_width;
					int siteHeight = raw_res[raw_browse_recno]->map_loc_height;
					if( world.can_build_site(xLoc, yLoc, siteWidth, siteHeight, INTER_PLACE_SPACE ) )
						site_array.add_site( xLoc, yLoc, SITE_RAW, raw_browse_recno, MAX_RAW_RESERVE_QTY * (50 + m.random(50)) / 100 );
				}

				return 1;
			}
			break;

		case SITE_SCROLL:
			if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )
			{
				int xLoc, yLoc;
				if( !browse_scroll.none_record
					&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
				{
					if( world.get_loc(xLoc, yLoc)->can_build_site() )
						site_array.add_site( xLoc, yLoc, SITE_SCROLL, scroll_browse_recno );
				}

				return 1;
			}
			break;

		case SITE_GOLD_COIN:
			if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )
			{
				int xLoc, yLoc;
				if( !browse_gold.none_record 
					&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
				{
					if( world.get_loc(xLoc, yLoc)->can_build_site() )
						site_array.add_site( xLoc, yLoc, SITE_GOLD_COIN, gold_coin_amount_array[gold_browse_recno-1] );
				}

				return 1;
			}
			break;

		case SITE_ITEM:
			if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )
			{
				int xLoc, yLoc;
				if( !browse_item.none_record 
					&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
				{
					if( world.get_loc(xLoc, yLoc)->can_build_site() )
						site_array.add_site( xLoc, yLoc, SITE_ITEM, item_browse_recno, item_res.random_para(item_browse_recno, m.rand()) );
				}

				return 1;
			}
			break;

		case SITE_WEAPON_BLUEPRINT:
			if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )
			{
				int xLoc, yLoc;
				if( !browse_tech.none_record
					&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
				{
					if( world.get_loc(xLoc, yLoc)->can_build_site() )
						site_array.add_site( xLoc, yLoc, SITE_WEAPON_BLUEPRINT, tech_browse_recno+tech_res.tech_class(TECH_CLASS_MEGA_WEAPON)->first_tech_id-1 );
				}

				return 1;
			}
			break;


		default:
			err_here();
		}

		// ------ remove site --------//

		if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1) )		// double right click to remove site
		{
			int xLoc, yLoc;
			if( world.zoom_matrix->get_detect_location(mouse.click_x(1), mouse.click_y(1), &xLoc, &yLoc, NULL ) )
			{
				if( world.get_loc(xLoc, yLoc)->has_site() )
					site_array.del_site( world.get_loc(xLoc, yLoc)->site_recno() );
			}

			return 1;
		}
	}
	else if( rock_or_site == 2 )		// plant
	{
		// ------- add plant --------//

		if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 0) )		// left button, add rock
		{
			int xLoc, yLoc;
			if( !browse_plant.none_record 
				&& world.zoom_matrix->get_detect_location(mouse.click_x(0), mouse.click_y(0), &xLoc, &yLoc, NULL ) )
			{
				Location *locPtr = world.get_loc(xLoc, yLoc);

				if( locPtr->can_add_plant() )
				{
					locPtr->set_plant( plant_res[plant_browse_recno]->first_bitmap + plant_size,
						LOCATE_WIDTH/2, LOCATE_HEIGHT/2 );

					// ------- set flammability ---------

					locPtr->set_fire_src(100);
					world.plant_count++;
				}
			}

			return 1;
		}

		// ------- remove plant -------//

		if( mouse.double_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, 1) )		// right button, remove rock
		{
			int xLoc, yLoc;
			if( world.zoom_matrix->get_detect_location(mouse.click_x(1), mouse.click_y(1), &xLoc, &yLoc, NULL ) )
			{
				Location *locPtr = world.get_loc(xLoc, yLoc);
				if( locPtr->is_plant() )
				{
					locPtr->remove_plant();
					world.plant_count--;
				}
			}

			return 1;
		}
	}

	return 0;
}
// ---- end of function ScenarioEditor::detect_misc_view ------//


// ---- begin of static function disp_rock_rec ------//
//
static void disp_rock_rec(int recNo,int x,int y,int refreshFlag)
{
	RockInfo *rockInfo = rock_res.get_rock_info( 
		scenario_editor.rock_browse_array[scenario_editor.rock_type][recNo-1] );

	font_san.put( x+6, y+4, rockInfo->rock_name );
	int x2;
	x2 = font_san.put( x+100, y+4, rockInfo->loc_width );
	x2 = font_san.put( x2+5, y+4, "x" );
	x2 = font_san.put( x2+5, y+4, rockInfo->loc_height );
}
// ---- end of static function disp_rock_rec ------//


// ---- begin of static function disp_raw_rec ------//
//
static void disp_raw_rec(int recNo,int x,int y,int refreshFlag)
{
	RawInfo *rawInfo = raw_res[recNo];
	font_san.put( x+6, y+4, rawInfo->name );
	vga.active_buf->put_bitmap(x+110, y+2, raw_res.large_raw_icon(recNo) );
}
// ---- end of static function disp_raw_rec ------//


// ---- begin of static function disp_scroll_rec ------//
//
static void disp_scroll_rec(int recNo,int x,int y,int refreshFlag)
{
	// scroll of power
	font_san.put( x+6, y+8, race_res[recNo]->name );

	char iconName[]="SCROLLxx";
	iconName[6] = race_res[recNo]->code[0];
	iconName[7] = race_res[recNo]->code[1];
	char *bmpPtr = image_spict.get_ptr(iconName);
	if( !bmpPtr )                                 //BUGHERE, the icons are not complete yet
		err_here();

	vga.active_buf->put_bitmap_trans(x+130, y, image_spict.get_ptr(iconName));
}
// ---- end of static function disp_scroll_rec ------//


// ---- begin of static function disp_gold_rec ------//
//
static void disp_gold_rec(int recNo,int x,int y,int refreshFlag)
{
	long amount;
	if( recNo <= GOLD_COIN_AMOUNT_COUNT )
		amount = gold_coin_amount_array[recNo-1];
	else
		amount = 10000 + 1000 * recNo - GOLD_COIN_AMOUNT_COUNT;
	font_san.put( x+6, y+4, m.format(amount , 2) );
}
// ---- end of static function disp_gold_rec ------//


// ---- begin of static function disp_item_rec ------//
//
static void disp_item_rec(int recNo,int x,int y,int refreshFlag)
{
	font_san.put( x+6, y+4, item_res.item_name(recNo, 0) );
}
// ---- end of static function disp_item_rec ------//


// ---- begin of static function disp_tech_rec ------//
//
static void disp_tech_rec(int recNo,int x,int y,int refreshFlag)
{
	int techId = tech_res.tech_class(TECH_CLASS_MEGA_WEAPON)->first_tech_id+recNo-1;
	font_san.put( x+6, y+4, tech_res[techId]->tech_des() );
}
// ---- end of static function disp_tech_rec ------//


// ---- begin of static function disp_plant_rec -------//
//
static void disp_plant_rec(int recNo, int x, int y, int refreshFlag )
{
	font_san.put( x+6, y+4, plant_res[recNo]->code, 0, x+150 );
}
// ---- end of static function disp_plant_rec -------//
