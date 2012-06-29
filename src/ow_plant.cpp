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

// Filename   : OW_PLANT.CPP
// Description: growth of plant
// Ownership  : Gilbert

#include <cstdlib>
#include <omatrix.h>
#include <oworld.h>
#include <oworldmt.h>
#include <oweather.h>
#include <oplant.h>
#include <oterrain.h>
#include <all.h>
#include <math.h>
#include	<orockres.h>
#include	<orock.h>

//------------ Define constant ---------------//
static short opt_temp[3] = { 32, 25, 28 };		// tropical,temperate and both
#define PLANT_ARRAY_SIZE 3

//------------ Define inline function -------//
inline int rand_inner_x()
{
	return LOCATE_WIDTH / 4 + m.random(LOCATE_WIDTH/2);
}

inline int rand_inner_y()
{
	return (LOCATE_HEIGHT * 3) / 8 + m.random(LOCATE_HEIGHT/4);
}

//----------- Begin of function World::plant_ops -----------//
//
void World::plant_ops()
{
	plant_grow(40);
	plant_reprod(10);
	plant_death();
	plant_spread(50);
}
//----------- End of function World::plant_ops -----------//

//------------ begin of function World::plant_grow ------------//
//
// pGrow = prabability of grow, range from 0 to 100
// scanDensity = scan one square per scanDensity^2
//
void World::plant_grow(int pGrow, int scanDensity)
{
	// scan part of the map for plant
	int yBase = m.random(scanDensity);
	int xBase = m.random(scanDensity);
	for( int y = yBase; y < max_y_loc; y += scanDensity)
		for( int x = xBase; x < max_x_loc; x += scanDensity)
		{
			Location *l = get_loc(x,y);
			short bitmapId, basePlantId;

			// is a plant and is not at maximum grade
			if( l->is_plant() && m.random(100) < pGrow &&
				(basePlantId = plant_res.plant_recno(bitmapId = l->plant_id())) != 0 &&
				bitmapId - plant_res[basePlantId]->first_bitmap < plant_res[basePlantId]->bitmap_count -1)
			{
				// increase the grade of plant
				l->grow_plant();
			}
		}
}
//------------ end of function World::plant_grow ------------//


//------------ begin of function World::plant_reprod --------//
//
// pReprod = prabability of reproduction, range from 0 to 20
// scanDensity = scan one square per scanDensity^2
//
void World::plant_reprod(int pReprod, int scanDensity)
{
	if( plant_count > plant_limit )
		return;
	if( 5 * plant_count < 4 * plant_limit )
		pReprod++;              // higher probability to grow

	// determine the rainful, temperature and sunlight
	short t = weather.temp_c();

	// scan the map for plant
	int yBase = m.random(scanDensity);
	int xBase = m.random(scanDensity);
	for( int y = yBase; y < max_y_loc; y += scanDensity)
	{
		for( int x = xBase; x < max_x_loc; x += scanDensity)
		{
			Location *l = get_loc(x,y);
			short bitmapId, basePlantId, plantGrade;
			// is a plant and grade > 3
			if( l->is_plant() && (basePlantId = plant_res.plant_recno(
				bitmapId = l->plant_id())) != 0 &&
				((plantGrade = bitmapId - plant_res[basePlantId]->first_bitmap) >= 3 ||
				 plantGrade == plant_res[basePlantId]->bitmap_count-1))
			{
				// find the optimal temperature for the plant
				short oTemp = opt_temp[plant_res[basePlantId]->climate_zone -1];
				short tempEffect = 5 - abs( oTemp - t);
				tempEffect = tempEffect > 0 ? tempEffect : 0;

				if( m.random(100) < tempEffect * pReprod)
				{
					// produce the same plant but grade 1,
					char trial = 2;
					Location *newl;
					while( trial --)
					{
						newl = NULL;
						switch(m.random(8))
						{
						case 0:		// north square
							if( y > 0)
								newl = get_loc(x,y-1);
							break;
						case 1:		// east square
							if( x < max_x_loc-1 )
								newl = get_loc(x+1,y);
							break;
						case 2:		// south square
							if( y < max_y_loc-1 )
								newl = get_loc(x,y+1);
							break;
						case 3:		// west square
							if( x > 0)
								newl = get_loc(x-1,y);
							break;
						case 4:		// north west square
							if( y > 0 && x > 0)
								newl = get_loc(x-1,y-1);
							break;
						case 5:		// north east square
							if( y > 0 && x < max_x_loc-1 )
								newl = get_loc(x+1,y-1);
							break;
						case 6:		// south east square
							if( y < max_y_loc-1 && x < max_x_loc-1)
								newl = get_loc(x+1,y+1);
							break;
						case 7:		// south west square
							if( y < max_y_loc-1 && x > 0)
								newl = get_loc(x-1,y+1);
							break;
						}

						char teraType;
						// #### begin Gilbert 6/3 #######//
						if( newl && newl->can_add_plant() &&
							(plant_res[basePlantId]->tera_type[0] == 
							 (teraType = terrain_res[newl->terrain_id]->average_type) ||
							 plant_res[basePlantId]->tera_type[1] == teraType ||
							 plant_res[basePlantId]->tera_type[2] == teraType) )
						// #### end Gilbert 6/3 #######//
						{
							newl->set_plant(plant_res[basePlantId]->first_bitmap
								, rand_inner_x(), rand_inner_y() );

							// ------- set flammability ---------
							newl->set_fire_src(100);
							plant_count++;
							//### begin alex 24/6 ###//
							//newl->set_power_off();
							//newl->power_nation_recno = 0;
							//set_surr_power_off(x, y);
							//#### end alex 24/6 ####//
							break;
						}
					}					
				}
			}
		}
	}
}
//------------ end of function World::plant_reprod --------//


//------------ begin of function World::plant_spread ------------//
//
// pSpread = probability of spreading, range from 0 to 1000
//
void World::plant_spread(int pSpread)
{
	if( plant_count > plant_limit)
		return;
	if( 5 * plant_count < 4 * plant_limit )
		pSpread += pSpread;

	if(m.random(1000) >= pSpread )
		return;

	// ------- determine temperature
	short t = weather.temp_c();

	// ------- randomly select a place to seed plant
	int y = 1+m.random(max_y_loc-2);
	int x = 1+m.random(max_x_loc-2);

	Location *l = get_loc(x,y);
	int build_flag = 1;
	char teraType = terrain_res[l->terrain_id]->average_type;

	// ------- all square around are the same terrain type and empty
	for( int y1 = y-1; y1 <= y+1; ++y1)
		for( int x1 = x-1; x1 <= x+1; ++x1)
		{
			l = get_loc(x1,y1);
			// #### begin Gilbert 6/3 #######//
			if( !l->can_add_plant() || terrain_res[l->terrain_id]->average_type != teraType)
				build_flag = 0;
			// #### end Gilbert 6/3 #######//
		}

	if( build_flag)
	{
		char climateZone = 0;
		short plantBitmap = 0;
		for( int retry=0; !climateZone && retry < 5; ++retry)
		{
			for( char j=0; j < 3; ++j)
			{
				if( m.random(5) > abs(t- opt_temp[j]) )
				{
					climateZone = j+1;
					plantBitmap = plant_res.scan( climateZone, teraType, 0);
					if( plantBitmap)
					{
						l = get_loc(x,y);
						l->set_plant( plantBitmap, rand_inner_x(), rand_inner_y() );
						l->set_fire_src(100);
						plant_count++;
					}
					break;
				}
			}
		}
	}
}
//------------ end of function World::plant_spread ------------//


//------------ begin of function World::plant_death ---------//
//
// a plant may death, if it is surrounded by many trees
//
// scanRadius = radius of the search area for killing a tree for subroutine plant_death()
//					remove a tree if the area has more than 3 trees	
//

void World::plant_death(int scanDensity, int scanRadius)
{
	int yBase = m.random(scanDensity);
	int xBase = m.random(scanDensity);
	for( int y = yBase; y < max_y_loc; y += scanDensity)
	{
		for( int x = xBase; x < max_x_loc; x += scanDensity)
//	for( int y = 0; y < max_y_loc; y ++)
//	{
//		for( int x = 0; x < max_x_loc; x ++)
		{
			Location *locPtr = get_loc(x,y);
			// when a plant is found
			if( locPtr->is_plant() )
			{
				char neighbour =0;
				char totalSpace =0;
				// scan the area arounding
				for( int scanY = y - scanRadius; scanY <= y + scanRadius; scanY ++)
				{
					for( int scanX = x - scanRadius; scanX <= x + scanRadius; scanX ++)
					{
						if ((scanX >= 0) && (scanY >= 0) &&
							(scanX < max_x_loc) && (scanY < max_y_loc))
						{
							totalSpace++;
							Location *scanLocPtr = get_loc(scanX,scanY);
							if( (scanLocPtr)->is_plant() )
								neighbour++;
						}
					}
				}
							
				// must remove plant if more than one forth of the space is occupied
			//	if( (totalSpace>>2) <= neighbour )
				if( neighbour > 2 )
				{
					locPtr = get_loc(x,y);
					get_loc(x,y)->remove_plant();
					if( locPtr->fire_src() > 50)
						locPtr->set_fire_src(50);
					plant_count--;
				}
			}
		}
	}
}
//------------ end of function World::plant_death ---------//


//------------ begin of function World::plant_init ------------//
// plantTrial = number of places that randomly selected to plant a forest
// scanRadius = radius of the search area for killing a tree for subroutine plant_death()
//					remove a tree if the area has more than 3 trees	
//
void World::plant_init(int plantTrial, int scanRadius)
{
	plant_count = 0;
	for( int trial = plantTrial; trial > 0; --trial )
	{
		// ------- randomly select a place to seed plant
		int y = 1+m.random(max_y_loc-2);
		int x = 1+m.random(max_x_loc-2);

		Location *l = get_loc(x,y);
		int build_flag = 1;
		char teraType = terrain_res[l->terrain_id]->average_type;

		// ------- all square around are the same terrain type and empty
		for( int y1 = y-1; y1 <= y+1; ++y1)
			for( int x1 = x-1; x1 <= x+1; ++x1)
			{
				l = get_loc(x1,y1);
				// #### begin Gilbert 6/3 #######//
				if( !l->can_add_plant() || terrain_res[l->terrain_id]->average_type != teraType)
					build_flag = 0;
				// #### end Gilbert 6/3 #######//
			}

		if( build_flag )
		{
			short plantBitmap = plant_res.scan( 0, teraType, 0);
			short plantArray[PLANT_ARRAY_SIZE];
			for( int i = 0; i < PLANT_ARRAY_SIZE; ++i)
			{
				plantArray[i] = plant_res.plant_recno(plant_res.scan(0, teraType, 0));
			}
			if( plantArray[0] )
			{
				plant_spray(plantArray, 1 + m.random(3), 1 + m.random(2), x, y);
			}
		}
	}
	int xLoc, yLoc;
	short plantBitmap = plant_res.scan( 0, 0, 0);
	short plantArray[PLANT_ARRAY_SIZE];
	for( int i = 0; i < PLANT_ARRAY_SIZE; ++i)
		plantArray[i] = plant_res.plant_recno(plant_res.scan(0, 0, 0));
					
	for( yLoc = 0; yLoc < max_y_loc; ++yLoc )
	{
		for( xLoc = 0; xLoc < max_x_loc; ++xLoc )
		{
			Location* locPtr = get_loc( xLoc, yLoc );
			if(locPtr->has_dirt())
			{
				Rock *dirtPtr = dirt_array[locPtr->dirt_recno()];
				RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
				if (dirtInfo->rock_type == 'P') 
				{
					if( plantArray[0] )
						plant_spray(plantArray, 0, m.random(3), xLoc, yLoc); 
				}
			}
		}
	}
	// ------- kill some plant ----------//
	plant_death(1, scanRadius);
}
//------------ end of function World::plant_init ------------//


//------------ begin of function World::plant_spray ------------//
void World::plant_spray(short *plantArray, char area, char distance, short x, short y)
{
	// #### begin Ban 8/10 #######//
	int i, j, tempi, tempj;
	Location *newl;
	char teraType;
	for (i=-area; i<=area; i++)
	{
		for (j=-area; j<=area; j++)
		{
			// ###### begin Gilbert 9/11 #####//
			tempi = m.random(distance<<2) + i - (distance<<1);
			tempj = m.random(distance<<2) + j - (distance<<1);
			// ###### end Gilbert 9/11 #####//
			if ( ((x+tempi)<(max_x_loc-1)) &&
				 ((y+tempj)<(max_y_loc-1)) &&
				 ((x+tempi)> 0) && ((y+tempj)> 0)) 
			{
				newl = get_loc(x+tempi, y+tempj);
				short basePlantId = plantArray[m.random(PLANT_ARRAY_SIZE)]; //get type of tree
				short plantSize = m.random(plant_res[basePlantId]->bitmap_count); //get  size of tree
				teraType = terrain_res[newl->terrain_id]->average_type;
				if( newl && newl->can_add_plant() && 
				   ((plant_res[basePlantId]->tera_type[0] == teraType) ||
					(plant_res[basePlantId]->tera_type[1] == teraType) ||
					(plant_res[basePlantId]->tera_type[2] == teraType)))
				{
					newl->set_plant(plant_res[basePlantId]->first_bitmap +plantSize, rand_inner_x(), rand_inner_y());
					newl->set_fire_src(100);
					plant_count++;
				}
				else if( newl && newl->is_plant() &&
					(newl->plant_id() - plant_res[plant_res.plant_recno(newl->plant_id())]->first_bitmap) >	plantSize )
				{
					newl->remove_plant();
					newl->set_plant(plant_res[basePlantId]->first_bitmap +plantSize, rand_inner_x(), rand_inner_y() );
					newl->set_fire_src(100);
				}
			}
		}
	}
	// #### end Ban 8/10 #######//
}
//------------ end of function World::plant_spray ------------//

