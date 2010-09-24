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

//Filename    : OGENMAP.CPP
//Description : World Map generation function part 1
//Ownership   : Gilbert

#include <time.h>
#include <stdlib.h>
#include <math.h>

#include <ALL.H>
#include <OGAME.H>
#include <OVGA.H>
#include <OTERRAIN.H>
#include <OWORLD.H>
#include <OPLASMA.H>
#include <OCAMPGN.H>
#include <OREGION.H>
#include <OFIRMID.H>
#include <OCONFIG.H>
#include <OWORLDMT.H>
#include <OVGALOCK.H>
#include <OSYS.H>


//-------- Begin of function World::generate_map ----------//
//
void World::generate_map(int loadGame)
{
	if( !loadGame )
		game.disp_gen_game_status(0);

	//--- loc_matrix, first store terrain height, then world map icon id ---//

	loc_matrix = (Location*) mem_resize( loc_matrix, MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC * sizeof(Location) );
	corner_matrix = (Corner *) mem_resize( corner_matrix, (MAX_WORLD_X_LOC+1) * (MAX_WORLD_Y_LOC+1) * sizeof(Corner));

	max_x_loc = MAX_WORLD_X_LOC;
	max_y_loc = MAX_WORLD_Y_LOC;

	// -------- save random seed ----------//

	long backupRandomSeed;
	if( !loadGame )
	{
		gen_map_random_seed = m.get_random_seed();
	}
	else
	{
		backupRandomSeed = m.get_random_seed();
		m.set_random_seed( gen_map_random_seed );
	}

	// ---------- generate plasma map ----------//

	Plasma heightMap, terrainMap;
	memset( loc_matrix , 0, sizeof(Location) * MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC );
	heightMap.init(max_x_loc, max_y_loc);

//	if( !sys.testing_session )
		heightMap.generate( m.random(2), 5, m.rand() );

	terrainMap = heightMap;

	if( !loadGame )
		game.disp_gen_game_status(10);

	//--------- remove odd terrain --------//

	for(short y = 0; y <= terrainMap.max_y; ++y)
		for(short x = 0; x <= terrainMap.max_x; ++x)
	{
		remove_odd(terrainMap, x, y, 5);
	}

	if( !loadGame )
		game.disp_gen_game_status(5);

	//--------- shuffle sub-terrain level ---------//

	set_tera_id(terrainMap);

	if( !loadGame )
		game.disp_gen_game_status(5);

	//---------------------------------//

	smooth_altitude(heightMap);

	if( !loadGame )
		game.disp_gen_game_status(5);

	//---------------------------------//

	set_tera_height(heightMap);

	if( !loadGame )
		game.disp_gen_game_status(5);

	//---------------------------------//

	terrainMap.deinit();		// free memory early
	heightMap.deinit();		// free memory early

	substitute_pattern();

	if( !loadGame )
		game.disp_gen_game_status(5);

	//---------------------------------//

	if( !loadGame )
	{
		set_loc_flags();
		assign_map();
		set_region_id();

		//--- if campaign's mountain layout is enabled, we leave it to the campaign functions to generate the mountains ---//

		if( !game.is_campaign_mode() )
			gen_mountains();

		else if( !game.campaign()->mountain_layout_enabled )
			world.gen_mountains(0, 50);		// 50% the amount of mountains in a campaign game is 50% less than a normal game.

		game.disp_gen_game_status(5);
	}

	// ---------- restore random seed ---------//

	if( loadGame )
	{
		m.set_random_seed( backupRandomSeed );
	}

}
//---------- End of function World::generate_map ------------//


//-------- Begin of function World::generate_map2 ----------//
//
void World::generate_map2()
{
	gen_dirt(25,10,15);

	game.disp_gen_game_status(5);

	//---------------------------------//

	plant_init(60, 2);

	game.disp_gen_game_status(10);

	//---------------------------------//

	init_fire();

	game.disp_gen_game_status(5);

	//----- debug code: validate terrain_id -----//

	#ifdef DEBUG

	Location* locPtr = loc_matrix;

	for( int i=0 ; i<MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC ; i++, locPtr++ )
	{
		err_when( locPtr->terrain_id < 1 ||
					 locPtr->terrain_id > terrain_res.terrain_count );
	}

	#endif
}
//---------- End of function World::generate_map2 ------------//


//---------- Begin of function World::set_tera_id -----------//
//
// Set terrain icon id
//
void World::set_tera_id(Plasma &plasma)
{
	//------- create a world map based on the terrain map ------//

	memset(loc_matrix, 0, sizeof(Location)*max_x_loc*max_y_loc);

	for( int y = 0; y < max_y_loc; ++y)
	{
		for( int x = 0; x < max_x_loc; ++x)
		{
			int nwType, neType, swType, seType;
			int nwSubType, neSubType, swSubType, seSubType;
			nwType = TerrainRes::terrain_height(plasma.get_pix(x,y), &nwSubType);
			neType = TerrainRes::terrain_height(plasma.get_pix(x+1,y), &neSubType);
			swType = TerrainRes::terrain_height(plasma.get_pix(x,y+1), &swSubType);
			seType = TerrainRes::terrain_height(plasma.get_pix(x+1,y+1), &seSubType);

			if((get_loc(x,y)->terrain_id = terrain_res.scan( nwType, nwSubType,
				neType, neSubType, swType, swSubType, seType, seSubType ,0,1,0)) == 0)
			{
				err.run("Error World::set_tera_id, Cannot find terrain type %d:%d, %d:%d, %d:%d, %d:%d",
					nwType, nwSubType, neType, neSubType, swType, swSubType,
					seType, seSubType);
			}
		}
	}
}
//---------- End of function World::set_tera_id -----------//


//---------- Begin of function World::set_loc_flags -----------//
//
void World::set_loc_flags()
{
	int       i;
	int       totalLoc=MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC;
	Location* locPtr=loc_matrix;

	//----- set power_off of the map edges -----//

	for( int xLoc=0 ; xLoc<MAX_WORLD_X_LOC ; xLoc++ )	// set the top and bottom edges
	{
		get_loc(xLoc, 0)->set_power_off();
		get_loc(xLoc, MAX_WORLD_Y_LOC-1)->set_power_off();
	}

	for( int yLoc=0 ; yLoc<MAX_WORLD_Y_LOC ; yLoc++ )	// set the left and right edges
	{
		get_loc(0, yLoc)->set_power_off();
		get_loc(MAX_WORLD_X_LOC-1, yLoc)->set_power_off();
	}

	//-----------------------------------------//

	if( config.explore_whole_map )
	{
		for( i=0 ; i<totalLoc ; i++, locPtr++ )
		{
			//------- set explored flag ----------//
			locPtr->explored_on();
//			if( terrain_res[locPtr->terrain_id]->is_coast() )
//			{
//				locPtr->loc_flag |= LOCATE_COAST;
//				if(terrain_res[locPtr->terrain_id]->average_type!=TERRAIN_OCEAN)
//					locPtr->set_power_off();
//				else
//					set_surr_power_off(i%MAX_WORLD_X_LOC, i/MAX_WORLD_X_LOC);
//			}
			locPtr->walkable_reset();
		}
	}
	else
	{
		for( i=0 ; i<totalLoc ; i++, locPtr++ )
		{
			//------- clear explored flag ----------//
			locPtr->explored_off();
//			if( terrain_res[locPtr->terrain_id]->is_coast() )
//			{
//				locPtr->loc_flag |= LOCATE_COAST;
//				if(terrain_res[locPtr->terrain_id]->average_type!=TERRAIN_OCEAN)
//					locPtr->set_power_off();
//				else
//					set_surr_power_off(i%MAX_WORLD_X_LOC, i/MAX_WORLD_X_LOC);
//			}
			locPtr->walkable_reset();
		}
	}
}
//---------- End of function World::set_loc_flags -----------//


//---------- Begin of function World::remove_odd --------//
void World::remove_odd(Plasma &plasma, short x, short y, short recur)
{
	if( recur < 0)
		return;

	// -------- compare the TerrainTypeCode of four adjacent square ------//
	int center = TerrainRes::terrain_height(plasma.get_pix(x,y));
	int same = 0;
	int diff = 0;
	short diffTerrain = -1;
	short diffHeight;
	short sameX, sameY;
	
	// ------- compare north square -------//
	if( y > 0)
	{
		if( center == TerrainRes::terrain_height(plasma.get_pix(x,y-1)) )
		{
			same++;
			sameX = x; sameY = y-1;
		}
		else
		{
			diff++;
			if( diffTerrain < 0)
			{
				// new diffHeight
				diffHeight = plasma.get_pix(x,y-1);
				diffTerrain = TerrainRes::terrain_height(diffHeight);
				
			}
			else
			{
				// three terrain types are close, don't change anything
				if( diffTerrain != TerrainRes::terrain_height(plasma.get_pix(x,y-1)))
					return;
			}
		}
	}

	// ------- compare south square -------//
	if( y < plasma.max_y)
	{
		if( center == TerrainRes::terrain_height(plasma.get_pix(x,y+1)) )
		{
			same++;
			sameX = x; sameY = y+1;
		}
		else
		{
			diff++;
			if( diffTerrain < 0)
			{
				// new diffHeight
				diffHeight = plasma.get_pix(x,y+1);
				diffTerrain = TerrainRes::terrain_height(diffHeight);
			}
			else
			{
				// three terrain types are close, don't change anything
				if( diffTerrain != TerrainRes::terrain_height(plasma.get_pix(x,y+1)))
					return;
			}
		}
	}

		// ------- compare west square -------//
	if( x > 0)
	{
		if( center == TerrainRes::terrain_height(plasma.get_pix(x-1,y)) )
		{
			same++;
			sameX = x-1; sameY = y;
		}
		else
		{
			diff++;
			if( diffTerrain < 0)
			{
				// new diffHeight
				diffHeight = plasma.get_pix(x-1,y);
				diffTerrain = TerrainRes::terrain_height(diffHeight);
			}
			else
			{
				// three terrain types are close, don't change anything
				if( diffTerrain != TerrainRes::terrain_height(plasma.get_pix(x-1,y)))
					return;
			}
		}
	}

	// ------- compare east square -------//
	if( x < plasma.max_x)
	{
		if( center == TerrainRes::terrain_height(plasma.get_pix(x+1,y)) )
		{
			same++;
			sameX = x+1; sameY = y;
		}
		else
		{
			diff++;
			if( diffTerrain < 0)
			{
				// new diffHeight
				diffHeight = plasma.get_pix(x+1,y);
				diffTerrain = TerrainRes::terrain_height(diffHeight);
			}
			else
			{
				// three terrain types are close, don't change anything
				if( diffTerrain != TerrainRes::terrain_height(plasma.get_pix(x+1,y)))
					return;
			}
		}
	}

	if( same <= 1 && diff >= 2)
	{
		// flatten
		plasma.plot(x,y, diffHeight);

		// propagate to next square
		if( same == 1)
		{
			remove_odd(plasma, sameX, sameY, recur-1);
		}
	}
}
//---------- End of function World::remove_odd --------//


//---------- Begin of function World::substitute_pattern -----//
void World::substitute_pattern()
{
	short terrainId;
	int SubFound;
	const resultArraySize = 20;
	TerrainSubInfo *candSub[resultArraySize];

	for( short y = 0; y < max_y_loc; ++y)
	{
		for( short x = 0; x < max_x_loc; ++x)
		{
			terrainId = get_loc(x,y)->terrain_id;
			SubFound = terrain_res.search_pattern(
				terrain_res[terrainId]->pattern_id, candSub, resultArraySize);
			for( int i = 0; i < SubFound; ++i)
			{
				short tx = x, ty = y;
				char flag = 1;
				TerrainSubInfo *terrainSubInfo = candSub[i];

				// ----- test if a substitution matches
				for(terrainSubInfo = candSub[i]; terrainSubInfo != NULL; 
					terrainSubInfo = terrainSubInfo->next_step)
				{
					if( tx < 0 || tx >= max_x_loc || ty < 0 || ty >= max_y_loc ||
						terrain_res[get_loc(tx,ty)->terrain_id]->pattern_id
						!= terrainSubInfo->old_pattern_id)
					{
						flag = 0;
						break;
					}

					// ----- update tx, ty according to post_move -----//
					switch(terrainSubInfo->post_move)
					{
					case 1: ty--; break;				// North
					case 2: ty--; tx++; break;		// NE
					case 3: tx++; break;				// East
					case 4: tx++; ty++; break;		// SE
					case 5: ty++; break;				// South
					case 6: ty++; tx--; break;		// SW
					case 7: tx--; break;				// West
					case 8: tx--; ty--; break;		// NW
					}
				}

				// ------ replace pattern -------//
				if(flag)
				{
					tx = x; ty = y;
					for(terrainSubInfo = candSub[i]; terrainSubInfo != NULL; 
						terrainSubInfo = terrainSubInfo->next_step)
					{
						TerrainInfo *oldTerrain = terrain_res[get_loc(tx,ty)->terrain_id];
						if( !(get_loc(tx,ty)->terrain_id = terrain_res.scan(oldTerrain->average_type,
							oldTerrain->secondary_type + terrainSubInfo->sec_adj, 
							terrainSubInfo->new_pattern_id, 0,1,0) ))
						{
							err_here();		// cannot find terrain_id
						}

						// ----- update tx, ty according to post_move -----//
						switch(terrainSubInfo->post_move)
						{
						case 1: ty--; break;				// North
						case 2: ty--; tx++; break;		// NE
						case 3: tx++; break;				// East
						case 4: tx++; ty++; break;		// SE
						case 5: ty++; break;				// South
						case 6: ty++; tx--; break;		// SW
						case 7: tx--; break;				// West
						case 8: tx--; ty--; break;		// NW
						}
					}
					break;
				}
			}			
		}
	}
}
//---------- End of function World::substitute_pattern -----//


//---------- Begin of function World::set_region_id -----//
// must be called before any mountain or buildings on the map
static RegionType walkable;					// to save stack space
static unsigned char regionId;
void World::set_region_id()
{
	int            i,x,y;
	int            totalLoc=max_x_loc * max_y_loc;
	Location*      locPtr=loc_matrix;

	// -------- reset region_id to zero
	for( i=0 ; i<totalLoc ; i++, locPtr++ )
	{
		locPtr->region_id = 0;
	}

	regionId = 0;
	for( y = 0; y < max_y_loc; ++y)
	{
		locPtr = get_loc(0,y);
		for( x = 0; x < max_x_loc; ++x, ++locPtr)
		{
			if( !locPtr->region_id && locPtr->region_type() != REGION_INPASSABLE)
			{
				walkable = locPtr->region_type();
				++regionId;
				fill_region(x,y);
				err_when( regionId == 255);
			}
		}
	}

	region_array.init(regionId);

	// ------ update adjacency information and region area ------//

	regionId = 0;
	for( y = 0; y < max_y_loc; ++y)
	{
		locPtr = get_loc(0,y);
		for( x = 0; x < max_x_loc; ++x, ++locPtr)
		{
			int thisRegionId = locPtr->region_id;
			// #### begin Gilbert 19/2 ######//
			if( thisRegionId > 0)
			{
				region_array.inc_size( thisRegionId );
			}
			// #### end Gilbert 19/2 ######//
			if( thisRegionId > regionId)
			{
				if(thisRegionId == regionId+1)
					regionId++;
				region_array.set_region( thisRegionId, locPtr->region_type());
			}

			int adjRegionId;
			if( y > 0)
			{
				if( x > 0 && (adjRegionId = get_loc(x-1,y-1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
				if( (adjRegionId = get_loc(x,y-1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
				if( x < max_x_loc-1 && (adjRegionId = get_loc(x+1,y-1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
			}

			if( x > 0 && (adjRegionId = get_loc(x-1,y)->region_id) < thisRegionId )
				region_array.set_adjacent( thisRegionId, adjRegionId);
			if( x < max_x_loc-1 && (adjRegionId = get_loc(x+1,y)->region_id) < thisRegionId )
				region_array.set_adjacent( thisRegionId, adjRegionId);

			if( y < max_y_loc-1)
			{
				if( x > 0 && (adjRegionId = get_loc(x-1,y+1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
				if( (adjRegionId = get_loc(x,y+1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
				if( x < max_x_loc-1 && (adjRegionId = get_loc(x+1,y+1)->region_id) < thisRegionId )
					region_array.set_adjacent( thisRegionId, adjRegionId);
			}
		}
	}

   //---- sort the region after setting its size ----//

	region_array.sort_region();

	//-------- initialize region_stat_array ----------//

	region_array.init_region_stat();
}
//---------- End of function World::set_region_id -----//


//---------- Begin of function World::fill_region -----//
void World::fill_region(short x, short y)
{
	err_when( x < 0 || x >= max_x_loc || y < 0 || y >= max_y_loc);

	short left, right;
	// Location *locPtr;

	// extent x to left and right
	for( left = x; left >= 0 && !get_loc(left,y)->region_id && get_loc(left,y)->region_type() == walkable; --left)
	{
		get_loc(left,y)->region_id = regionId;
	}
	++left;

	for( right=x+1; right < max_x_loc && !get_loc(right,y)->region_id && get_loc(right,y)->region_type() == walkable; ++right)
	{
		get_loc(right,y)->region_id = regionId;
	}
	--right;

	// ------- scan line below ---------//
	y++;
	if( y < max_y_loc )
	{
		for( x = left>0?left-1:0 ; x <= right+1 && x < max_x_loc; ++x )
		{
			if( !get_loc(x,y)->region_id && get_loc(x,y)->region_type() == walkable)
			{
				fill_region(x,y);
			}
		}
	}

	// ------- scan line above -------- //
	y -= 2;
	if( y >= 0)
	{
		for( x = left>0?left-1:0 ; x <= right+1 && x < max_x_loc; ++x )
		{
			if( !get_loc(x,y)->region_id && get_loc(x,y)->region_type() == walkable)
			{
				fill_region(x,y);
			}
		}
	}
}
//---------- End of function World::fill_region -----//


//---------- Begin of function World::set_harbor_bit -----//
void World::set_harbor_bit()
{
	// a pass during genmap to set LOCATE_HARBOR_BIT
	// notice this bit is only a necessary condition to build harbor
//	int x,y;
//	Location *locPtr;
//	for( y = 0; y < max_y_loc-2; ++y)
//	{
//		locPtr = get_loc(0,y);
//		for( x = 0; x < max_x_loc-2; ++x, ++locPtr)
//		{
//			if( can_build_firm(x, y, FIRM_HARBOR))
//			{
//				locPtr->set_harbor_bit();
//			}
//		}
//	}
}
//---------- End of function World::set_harbor_bit -----//


void World::set_tera_height(Plasma &plasma)
{
	memset(corner_matrix, 0, sizeof(Corner)*(max_x_loc+1)*(max_y_loc+1));

	// short seaHeight = TerrainRes::max_height(TERRAIN_OCEAN);
	short seaHeight = 0;			// no more sea
	int y;
	int x;
	for(y = 0; y <= max_y_loc; ++y )
	{
		for(x = 0; x <= max_x_loc; ++x)
		{
			Corner *cornerPtr = get_corner(x,y);
			cornerPtr->set_altitude(plasma.get_pix(x,y));
			cornerPtr->set_sea_altitude(seaHeight);

			// set gradient_x
			float gradientX;
			if(x <= 0)
			{
				err_when(x < 0);
				gradientX = float(4*plasma.get_pix(x+1,y) - plasma.get_pix(x+2,y) - 3*plasma.get_pix(x,y)) 
					/ (2 * LOCATE_WIDTH);
			}
			else if( x >= max_x_loc )
			{
				err_when(x > max_x_loc);
				gradientX = float(plasma.get_pix(x-2,y) - 4*plasma.get_pix(x-1,y) + 3*plasma.get_pix(x,y)) 
					/ (2 * LOCATE_WIDTH);
			}
			else
			{
				gradientX = float(plasma.get_pix(x+1,y) - plasma.get_pix(x-1,y)) 
					/ (2 * LOCATE_WIDTH);
			}

//			if( gradientX >= 0.0f && gradientX <= 0.25f )
//				gradientX += 0.15f;
//			else if( gradientX < 0.0f && gradientX >= -0.25f )
//				gradientX -= 0.15f;
			cornerPtr->set_gradient_x( gradientX );

			// set gradient_y
			float gradientY;
			if(y <= 0)
			{
				err_when(y < 0);
				gradientY = float(4*plasma.get_pix(x,y+1) - plasma.get_pix(x,y+2) - 3*plasma.get_pix(x,y))
					/ (2 * LOCATE_HEIGHT);
			}
			else if( y >= max_y_loc )
			{
				err_when(y > max_y_loc);
				gradientY = float(plasma.get_pix(x,y-2) - 4*plasma.get_pix(x,y-1) + 3*plasma.get_pix(x,y))
					/ (2 * LOCATE_HEIGHT);
			}
			else
			{
				gradientY = float(plasma.get_pix(x,y+1) - plasma.get_pix(x,y-1))
					/ (2 * LOCATE_HEIGHT);
			}

//			if( gradientY >= 0.0f && gradientY <= 0.25f )
//				gradientY += 0.15f;
//			else if( gradientY < 0.0f && gradientY >= -0.25f )
//				gradientY -= 0.15f;
			cornerPtr->set_gradient_y( gradientY );

		}
	}

	// calculate polynomial coefficient
	for(y = 0; y < max_y_loc; ++y )
	{
		for(x = 0; x < max_x_loc; ++x)
		{
			LocationCorners lc;
			get_loc_corner(x,y, &lc);
			short &z0 = lc.top_left->altitude;
			short &z1 = lc.top_right->altitude;
			short &z2 = lc.bottom_left->altitude;
			short &z3 = lc.bottom_right->altitude;
			short &duz0w = lc.top_left->gradient_x;		// du/dz * LOCATE_WIDTH
			short &dvz0h = lc.top_left->gradient_y;
			short &duz1w = lc.top_right->gradient_x;
			short &dvz1h = lc.top_right->gradient_y;
			short &duz2w = lc.bottom_left->gradient_x;
			short &dvz2h = lc.bottom_left->gradient_y;
			short &duz3w = lc.bottom_right->gradient_x;
			short &dvz3h = lc.bottom_right->gradient_y;

			lc.loc_ptr->c30 = (duz1w + duz0w - (z1-z0) * 2) << (C_MULTIPLIER_SHIFT - LOCATE_WIDTH_SHIFT - LOCATE_WIDTH_SHIFT - LOCATE_WIDTH_SHIFT);
			lc.loc_ptr->c21 = (duz3w + duz0w - duz1w - duz2w) << (C_MULTIPLIER_SHIFT - 1 - LOCATE_WIDTH_SHIFT - LOCATE_WIDTH_SHIFT - LOCATE_HEIGHT_SHIFT);
			lc.loc_ptr->c12 = (dvz3h + dvz0h - dvz1h - dvz2h) << (C_MULTIPLIER_SHIFT - 1 - LOCATE_WIDTH_SHIFT - LOCATE_HEIGHT_SHIFT - LOCATE_HEIGHT_SHIFT);
			lc.loc_ptr->c03 = (dvz2h + dvz0h - (z2-z0) * 2) << (C_MULTIPLIER_SHIFT - LOCATE_HEIGHT_SHIFT - LOCATE_HEIGHT_SHIFT - LOCATE_HEIGHT_SHIFT);
			lc.loc_ptr->c20 = ( (z1-z0) * 3 - duz1w - duz0w * 2 ) << (C_MULTIPLIER_SHIFT - LOCATE_WIDTH_SHIFT - LOCATE_WIDTH_SHIFT);
			lc.loc_ptr->c11 = (( z3 + z0 - z1 - z2 ) << (C_MULTIPLIER_SHIFT - LOCATE_WIDTH_SHIFT - LOCATE_HEIGHT_SHIFT))
				- (lc.loc_ptr->c21 << LOCATE_WIDTH_SHIFT) - (lc.loc_ptr->c12 << LOCATE_HEIGHT_SHIFT);
			lc.loc_ptr->c02 = ( (z2-z0) * 3 - dvz2h - dvz0h * 2 ) << (C_MULTIPLIER_SHIFT - LOCATE_HEIGHT_SHIFT - LOCATE_HEIGHT_SHIFT);
			lc.loc_ptr->c00 = z0 << C_MULTIPLIER_SHIFT;
			lc.loc_ptr->c10 = duz0w << (C_MULTIPLIER_SHIFT - LOCATE_WIDTH_SHIFT);
			lc.loc_ptr->c01 = dvz0h << (C_MULTIPLIER_SHIFT - LOCATE_HEIGHT_SHIFT);

			// find the height of the rendered bitmap
			{
				lc.loc_ptr->min_y = 0;
				lc.loc_ptr->max_y = 0;

				int seaLevel = 0;
				if( lc.top_left->is_sea() )
					seaLevel = lc.top_left->sea_altitude;
				if( lc.top_right->is_sea() )
					seaLevel = lc.top_right->sea_altitude;
				if( lc.bottom_left->is_sea() )
					seaLevel = lc.bottom_left->sea_altitude;
				if( lc.bottom_right->is_sea() )
					seaLevel = lc.bottom_right->sea_altitude;

				if( seaLevel )
					lc.loc_ptr->loc_flag |= LOCATE_SEA_COAST;
				else
					lc.loc_ptr->loc_flag &= ~LOCATE_SEA_COAST;


				// height relative to top left corner
				// int refHeight = ZoomMatrix::calc_abs_y(0, 0, lc.loc_ptr->evaluate_z(0, 0));
				int refHeight = ZoomMatrix::calc_abs_y(0, 0, lc.loc_ptr->c00 >> C_MULTIPLIER_SHIFT);

				// include LOCATE_WIDTH, LOCATE_HEIGHT for wider min_y, max_y
				for(int v = 0; v <= LOCATE_HEIGHT; v+=LOCATE_HEIGHT/4 )
				{
					for( int u = 0; u <= LOCATE_WIDTH; u+=LOCATE_WIDTH/4 )
					{
						long z = lc.loc_ptr->evaluate_z(u, v);

						// if cover by sea
						if( seaLevel && z < seaLevel )
							z = seaLevel;

						int pointY = ZoomMatrix::calc_abs_y(u, v, z) - refHeight;
							//(z - z0) * ZOOM_Z_HEIGHT
							//+ (u * ZOOM_LOC_X_HEIGHT * LOCATE_HEIGHT
							//+ v * ZOOM_LOC_Y_HEIGHT * LOCATE_WIDTH ) 
							//>> (LOCATE_WIDTH_SHIFT + LOCATE_HEIGHT_SHIFT);

						if( pointY < lc.loc_ptr->min_y)
							lc.loc_ptr->min_y = pointY;
						if( pointY > lc.loc_ptr->max_y)
							lc.loc_ptr->max_y = pointY;
					}
				}

				lc.loc_ptr->min_y -= 8;		// -4 for more space
				lc.loc_ptr->max_y += 8;		// +4 for more space
			}
		}
	}
}

/*
void World::add_fringe( Plasma& heightMap, Plasma &fringeMap )
{
	short x, y;

	err_when( heightMap.max_x != fringeMap.max_x );
	err_when( heightMap.max_y != fringeMap.max_y );

	for( y = 1; y < heightMap.max_y; ++y )
	{
		for( x = 1; x < heightMap.max_x; ++x )
		{
			// find place where around 128
			short f = fringeMap.get_pix(x, y);
			if( f < 128 && 
				( fringeMap.get_pix(x-1,y) >= 128 || fringeMap.get_pix(x+1,y) >= 128
				|| fringeMap.get_pix(x,y-1) >= 128 || fringeMap.get_pix(x,y+1) >= 128) )
			{
				short h;
				if( (h = heightMap.get_pix(x, y)) < 220 )
				{
					heightMap.plot( x, y, h + 8 );
				}
			}
		}
	}
}
*/

void World::smooth_altitude( Plasma &plasma )
{
	// ##### begin Gilbert 29/12 #######//

	// --------- reduce height different between (x-6,y), (x,y-6) --------//

	const int d = 6;
	static short coeff[d+1] = { 8, 2, 0, 0, 0, 1, 5 };	// weight at (x-0), (x-1), (x-2) ...
	{
		for(short y = 0; y <= plasma.max_y; ++y)
		{
			for(short x = 0; x <= plasma.max_x; ++x)
			{
				short h0 = plasma.get_pix( x, y );
				short dh1;
				short dh2;
				short dh3;

				if( x >= d )
					dh1 = abs(plasma.get_pix(x-d, y) - h0);
				if( y >= d )
					dh2 = abs(plasma.get_pix(x, y-d) - h0);
				if( x >= d && y >= d )
					dh3 = abs(plasma.get_pix(x-d, y-d) - h0);

				if( x >= d && y >= d && dh1 <= 16 && dh2 <= 16 && dh3 <= 16 )		// no filtering needed
					continue;

				long sum = coeff[0] * h0;
				short w = coeff[0];

				short minXD = min(x,d);
				for( short dx = minXD; dx > 0; --dx )
				{
					sum += coeff[dx] * plasma.get_pix( x-dx, y );
					w += coeff[dx];
				}

				short minYD = min(y,d);
				for( short dy = minYD; dy > 0; --dy )
				{
					sum += coeff[dy] * plasma.get_pix( x, y-dy );
					w += coeff[dy];
				}

				short minXYD = min( minXD, minYD );
				for( short dxy = minXYD; dxy > 0; --dxy )
				{
					sum += coeff[dxy] * plasma.get_pix( x-dxy, y-dxy );
					w += coeff[dxy];
				}

				plasma.plot( x, y, sum/w );
			}
		}
	}

	// ##### end Gilbert 29/12 #######//
}


void World::gen_road(Plasma &roadMap, short x1, short y1, short x2, short y2)
{
	int x, y;
	short roadMinHeight = terrain_res.min_height(TERRAIN_ROAD);
	short roadMaxHeight = terrain_res.max_height(TERRAIN_ROAD);
	short h = roadMinHeight;

	// draw road randomly

	int steps = (abs(x2-x1) + abs(y2-y1)) / 8;

	// found down steps to the nearest power of two
	int j;
	char p;
	for( p = 0; (1<<p) < steps; ++p );
	if( p > 1 )
		--p;
	steps = 1 << p;

	// generate midpoints
	short *midX = (short *) mem_add( sizeof(short) * (steps+1) );
	short *midY = (short *) mem_add( sizeof(short) * (steps+1) );
	memset( midX, 0, sizeof(short) * (steps+1) );
	memset( midY, 0, sizeof(short) * (steps+1) );
	midX[0] = x1;
	midY[0] = y1;
	midX[steps] = x2;
	midY[steps] = y2;

	// generating sequence :
	// 1/2
	// 1/4, 3/4
	// 1/8, 3/8, 5/8, 7/8
	// 1/16, 3/16, 5/16, 7/16, 9/16, 11/16, 13/16, 15/16 ...
	for( ; p > 0; --p )
	{
		int dist = 1<<p;
		int halfDist = 1<<(p-1);
		for( j = halfDist; j < steps; j += dist )
		{
			err_when ( midX[j] != 0 );
			err_when ( midY[j] != 0 );
			// int wx = 5 + m.random(7);		// varies from 5:11 to 11:5
			// int wy = 5 + m.random(7);		// varies from 5:11 to 11:5
			int wx = 6 + m.random(5);		// varies from 6:10 to 10:6
			int wy = 6 + m.random(5);		// varies from 6:10 to 10:6
			midX[j] = (midX[j-halfDist]*wx + midX[j+halfDist]*(16-wx))/16;
			midY[j] = (midY[j-halfDist]*wy + midY[j+halfDist]*(16-wy))/16;
		}
	}

	// draw line segments

	x = midX[0];
	y = midY[0];

	for( j = 0; j < steps; ++j )
	{
		// draw line from x,y to midX[j+1],midY[j+1]
		short xb = midX[j+1];
		short yb = midY[j+1];

		// determine direction

		if( x == xb && y == yb )
			continue;

		int absX = abs(xb-x);
		int absY = abs(yb-y);

		if( absX >= absY )
		{
			// x as base increment
			short xInc = xb>=x?1:-1;
			short yInc = yb>=y?1:-1;
			short d = 0;			// a rational number multiplied by absX
			for( ; x != xb; x += xInc )
			{
				if( x >= 0 && x < max_x_loc && y >=0 && y < max_y_loc )
					roadMap.plot(x, y, roadMaxHeight );

				h = roadMinHeight + (roadMaxHeight-roadMinHeight) * (absX-d) / absX;
				if( x >= 0 && x < max_x_loc && y-yInc >= 0 && y-yInc < max_y_loc 
					&& roadMap.get_pix(x,y-yInc) < h )
					roadMap.plot(x, y-yInc, h );

				h = roadMinHeight + (roadMaxHeight-roadMinHeight) * d / absX;
				if( x >= 0 && x < max_x_loc && y+yInc >= 0 && y+yInc < max_y_loc 
					&& roadMap.get_pix(x,y+yInc) < h )
					roadMap.plot(x, y+yInc, h );

				d += absY;
				if( d >= absX )
				{
					d -= absX;
					y += yInc;
				}
			}
		}
		else
		{
			// y as base increment
			short xInc = xb>=x?1:-1;
			short yInc = yb>=y?1:-1;
			short d = 0;			// a rational number multiplied by absY
			for( ; y != yb; y += yInc )
			{
				if( x >= 0 && x < max_x_loc && y >=0 && y < max_y_loc )
					roadMap.plot(x, y, roadMaxHeight );
				
				h = roadMinHeight + (roadMaxHeight-roadMinHeight) * (absY-d) / absY;
				if( x-xInc >= 0 && x-xInc < max_x_loc && y >= 0 && y < max_y_loc
					&& roadMap.get_pix(x-xInc, y) < h )
					roadMap.plot(x-xInc, y, h );

				h = roadMinHeight + (roadMaxHeight-roadMinHeight) * d / absY;
				if( x+xInc >= 0 && x+xInc < max_x_loc && y >=0 && y < max_y_loc
					&& roadMap.get_pix(x+xInc, y) < h )
					roadMap.plot(x+xInc, y, h );

				d += absX;
				if( d >= absY )
				{
					d -= absY;
					x += xInc;
				}
			}
		}
	}

	mem_del(midX);
	mem_del(midY);

}


void World::set_road_id( Plasma &roadMap )
{

	int x, y;
	// BUGHERE : need to fill the rest of pixels in roadMap

	// substitude road

	for( y = 0; y < max_y_loc; ++y)
	{
		for( x = 0; x < max_x_loc; ++x)
		{
			int nwType, neType, swType, seType;
			int nwSubType, neSubType, swSubType, seSubType;
			nwType = TerrainRes::terrain_height(roadMap.get_pix(x,y), &nwSubType);
			neType = TerrainRes::terrain_height(roadMap.get_pix(x+1,y), &neSubType);
			swType = TerrainRes::terrain_height(roadMap.get_pix(x,y+1), &swSubType);
			seType = TerrainRes::terrain_height(roadMap.get_pix(x+1,y+1), &seSubType);

			if( nwType != TERRAIN_ROAD && neType != TERRAIN_ROAD && swType != TERRAIN_ROAD
				&& seType != TERRAIN_ROAD )
			{
				get_loc(x,y)->road_terrain_id = 0;
				continue;		// no road in this tile
			}

			if((get_loc(x,y)->road_terrain_id = terrain_res.scan( nwType, nwSubType,
				neType, neSubType, swType, swSubType, seType, seSubType ,0,1,0)) == 0)
			{
				err.run("Error World::gen_road, Cannot find terrain type %d:%d, %d:%d, %d:%d, %d:%d",
					nwType, nwSubType, neType, neSubType, swType, swSubType,
					seType, seSubType);
			}
		}
	}
}