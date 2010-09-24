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

// Filename    : OW_ROCK.CPP
// Description : generate random rock on map


#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OROCKRES.H>
#include <OROCK.H>
#include <OCAMPGN.H>
#include <OTERRAIN.H>
#include <OSITE.H>
#include <math.h>

//------------ Define constant ----------//

#define PI 3.141592654

static const int GAP=2;
static const int HUGE_ROCK_SIZE=8;		// HILL and PONDS 6
static const int LARGE_ROCK_SIZE=3;		// LARGE STONES 4
static const int SMALL_ROCK_SIZE=2;		// SMALL STONES 2
static const int SMALLEST_ROCK_SIZE=1;

//--------------- begin of function World::can_add_rock ----------//
int World::can_add_rock(short x1, short y1, short x2, short y2)
{
	int x,y;
	err_when(x1 > x2 || y1 > y2);
	// ######## begin Gilbert 30/1 #######//
//	for(y = y1; y <= y2; ++y)
//		for( x = x1; x <= x2; ++x)
//			if( !get_loc(x,y)->can_add_rock(3) )
//				return 0;

	// ######## begin Gilbert 30/1 #######//
	int outerX1 = max( x1-INTER_PLACE_SPACE*2, 0);
	int outerY1 = max( y1-INTER_PLACE_SPACE*2, 0);
	int outerX2 = min( x2+INTER_PLACE_SPACE*2, max_x_loc-1);
	int outerY2 = min( y2+INTER_PLACE_SPACE*2, max_y_loc-1);

	for( y = outerY1; y <= outerY2; ++y )
	{
		Location *locPtr = get_loc( outerX1, y );
		for( x = outerX1; x <= outerX2; ++x, ++locPtr )
		{
			if( y >= y1 && y <= y2 && x >= x1 && x <= x2 )
			{
				// inner location
				if( !locPtr->can_add_rock(3) )
					return 0;
			}
			else
			{
				// outer location
				if( locPtr->is_firm() || locPtr->is_town() ||
					(locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW) )
					return 0;
			}
		}
	}
	// ######## end Gilbert 30/1 #######//
	return 1;
}
//--------------- end of function World::can_add_rock ----------//


// --------- begin of function World::add_rock ----------//
// 
// note : make sure the location in the area is free
//
void World::add_rock(short rockRecno, short x1, short y1)
{
	// ------- get the delay remain count for the first frame -----//
	Rock newRock(rockRecno, x1, y1);
	int rockArrayRecno = rock_array.add(&newRock);
	RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
	for( short dy = 0; dy < rockInfo->loc_height && y1+dy < max_y_loc; ++dy)
	{
		for( short dx = 0; dx < rockInfo->loc_width && x1+dx < max_x_loc; ++dx)
		{
			Location *locPtr = get_loc(x1+dx, y1+dy);
			err_when( !locPtr->can_add_rock(3) );
			locPtr->set_rock(rockArrayRecno);
			// ##### begin Gilbert 5/11 ######//
//			locPtr->set_power_off();		// no longer set_surr_power_off to avoid building too near
//			set_surr_power_off(x1, y1);
			// ##### end Gilbert 5/11 ######//
		}
	}
}
// --------- end of function World::add_rock ----------//

// --------- begin of function World::remove_rock ----------//
void World::remove_rock(short rockArrayRecno )
{
	Rock *rockPtr = rock_array[rockArrayRecno];
	if( rockPtr->rock_recno == 0 )		// deleted
		return;

	RockInfo *rockInfo = rock_res.get_rock_info(rockPtr->rock_recno);
	int x1 = rockPtr->loc_x;
	int y1 = rockPtr->loc_y;
	int x2 = x1 + rockInfo->loc_width - 1;
	int y2 = y1 + rockInfo->loc_height - 1;

	for( int y = y1; y <= y2; ++y )
	{
		int x = x1;
		Location *locPtr = get_loc(x,y);
		for( ; x <= x2; ++x, ++locPtr )
		{
			locPtr->remove_rock();
		}
	}
}
// --------- end of function World::remove_rock ----------//


// --------- begin of function World::gen_rocks ----------//
//
// [int] nGrouped  - the number of rock groups with a
//						   central big rock and several smaller rocks around
// [int] nLarge    - the number of large rocks
// [int] nSmall    - the number of small rocks.
//
// Default: if the number of rocks are not given, randomize the numbers.
//
void World::gen_rocks(int nGrouped, int nLarge, int nSmall)
{
	if( !nGrouped )
		nGrouped = 10 + m.random(10);

	if( !nLarge )
		nLarge = 10 + m.random(10);

	if( !nSmall )
		nSmall = 10 + m.random(10);

	//----------------------------------------------//

	// one 'large' (size 1 to 4) at the center
	// and a number 'small' (size 1 to 2) at the surroundings
	// #### begin Ban 8/10 #######//

	int trial = (nGrouped + nLarge + nSmall) * 2;

	while( (nGrouped > 0 || nLarge > 0 || nSmall > 0) && --trial > 0)
	{
		// generate grouped rocks i.e. small stones must have same terrsin as huge ones
		if( nGrouped > 0 )
		{
			short x = (GAP+LARGE_ROCK_SIZE)+m.random( max_x_loc - HUGE_ROCK_SIZE +1 - 2*(GAP+LARGE_ROCK_SIZE));
			short y = (GAP+LARGE_ROCK_SIZE)+m.random( max_y_loc - HUGE_ROCK_SIZE +1 - 2*(GAP+LARGE_ROCK_SIZE));
			short x2 = x + HUGE_ROCK_SIZE -1;
			short y2 = y + HUGE_ROCK_SIZE -1;

			if( can_add_rock(x,y, x2,y2) )
			{
				short rockRecno;
		//		if (m.random(10) >5)
				rockRecno = rock_res.search("R",LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
		//		else
		//		rockRecno = rock_res.search("P",LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,-1,0,
		//			terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{
					add_rock(rockRecno, x, y);
					char tempwidth =rockInfo->loc_width;
					char tempheight =rockInfo->loc_height;
				
					// add other smaller rock
					for(int subTrial = 12; subTrial > 0 ; --subTrial )
					{
						// sx from x-SMALL_ROCK_SIZE to x+4-1+SMALL_ROCK_SIZE
						short r = m.random(360);
						short sx = (short)(x+ tempwidth/2 + (tempwidth/2 +GAP) * cos(r * PI /180));
						short sy = (short)(y+ tempheight/2 + (tempheight/2 +GAP) * sin(r * PI /180));
						short sx2 = sx + LARGE_ROCK_SIZE-1;
						short sy2 = sy + LARGE_ROCK_SIZE-1;

						if( can_add_rock( sx, sy, sx2, sy2))
						{
							short rock2Recno;
						//	if (m.random(10) >5)
							rock2Recno = rock_res.search("R",1,LARGE_ROCK_SIZE,1,LARGE_ROCK_SIZE,-1,0,
								terrain_res[get_loc(sx,sy)->terrain_id]->average_type );
						//	else
						//	rock2Recno = rock_res.search("P",1,LARGE_ROCK_SIZE,1,LARGE_ROCK_SIZE,-1,0,
						//		terrain_res[get_loc(sx,sy)->terrain_id]->average_type );
							if(!rock2Recno)
								continue;

							RockInfo *rock2Info = rock_res.get_rock_info(rock2Recno);
							sx2 = sx + rock2Info->loc_width -1;
							sy2 = sy + rock2Info->loc_height -1;
							if( rock2Info->valid_terrain(terrain_res[get_loc(sx2,sy)->terrain_id]->average_type)
								&& rock2Info->valid_terrain(terrain_res[get_loc(sx,sy2)->terrain_id]->average_type)
								&& rock2Info->valid_terrain(terrain_res[get_loc(sx2,sy2)->terrain_id]->average_type) )
							{
								add_rock(rock2Recno, sx, sy);
							}
						}
					}
					nGrouped--;
				}
			}
		}

		// generate stand-alone large rock
		if( nLarge > 0 )
		{
			short x = m.random( max_x_loc - LARGE_ROCK_SIZE);
			short y = m.random( max_y_loc - LARGE_ROCK_SIZE);
			short x2 = x + LARGE_ROCK_SIZE -1;
			short y2 = y + LARGE_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) )
			{
				short rockRecno;
			//	if (m.random(10) >5)
				rockRecno = rock_res.search("R", SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
			//	else
			//	rockRecno = rock_res.search("P", SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,-1,0,
			//		terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{
					add_rock(rockRecno, x, y);
					nLarge--;
				}
			}
		}

		// generate stand-alone small rock
		if( nSmall > 0)
		{
			short x = m.random( max_x_loc - SMALL_ROCK_SIZE);
			short y = m.random( max_y_loc - SMALL_ROCK_SIZE);
			short x2 = x + SMALL_ROCK_SIZE -1;
			short y2 = y + SMALL_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) )
			{
				short rockRecno;
			//	if (m.random(10) >5)
				rockRecno = rock_res.search("R", 1,SMALL_ROCK_SIZE,1,SMALL_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
			//	else
			//	rockRecno = rock_res.search("P", 1,SMALL_ROCK_SIZE,1,SMALL_ROCK_SIZE,-1,0,
			//		terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{
					add_rock(rockRecno, x, y);
					nSmall--;
				}
			}
		}
	}
	// #### end Ban 8/10 #######//
}
// --------- end of function World::gen_rocks ----------//


//--------------- begin of function World::can_add_dirt ----------//
int World::can_add_dirt(short x1, short y1, short x2, short y2)
{
	int x,y;
	err_when(x1 > x2 || y1 > y2);
	// ######## begin Gilbert 30/1 #######//
//	for(y = y1; y <= y2; ++y)
//		for( x = x1; x <= x2; ++x)
//			if( !get_loc(x,y)->can_add_dirt() )
//				return 0;

	int outerX1 = max( x1-INTER_PLACE_SPACE*2, 0);
	int outerY1 = max( y1-INTER_PLACE_SPACE*2, 0);
	int outerX2 = min( x2+INTER_PLACE_SPACE*2, max_x_loc-1);
	int outerY2 = min( y2+INTER_PLACE_SPACE*2, max_y_loc-1);

	for( y = outerY1; y <= outerY2; ++y )
	{
		Location *locPtr = get_loc( outerX1, y );
		for( x = outerX1; x <= outerX2; ++x, ++locPtr )
		{
			if( y >= y1 && y <= y2 && x >= x1 && x <= x2 )
			{
				// inner location
				if( !locPtr->can_add_dirt() )
					return 0;
			}
			else
			{
				// outer location
				if( locPtr->is_firm() || locPtr->is_town() ||
					(locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW) )
					return 0;
			}
		}
	}
	// ######## end Gilbert 30/1 #######//
	return 1;
}
//--------------- end of function World::can_add_dirt ----------//


// --------- begin of function World::add_dirt ----------//
//
// note : make sure the location in the area is free
//
void World::add_dirt(short dirtRecno, short x1, short y1)
{
	if( dirt_array.size() >= 255 )
		return;

	// ------- get the delay remain count for the first frame -----//
	Rock newDirt(dirtRecno, x1, y1);
	int dirtArrayRecno = dirt_array.add(&newDirt);

	if( dirtArrayRecno >= 255)		// Location::extra_para is only BYTE
		return;

	RockInfo *dirtInfo = rock_res.get_rock_info(dirtRecno);

	for( short dy = 0; dy < dirtInfo->loc_height && y1+dy < max_y_loc; ++dy)
	{
		for( short dx = 0; dx < dirtInfo->loc_width && x1+dx < max_x_loc; ++dx)
		{
			Location *locPtr = get_loc(x1+dx, y1+dy);
			err_when( !locPtr->can_add_dirt() );
			locPtr->set_dirt(dirtArrayRecno);

			if(( dirtInfo->rock_type == DIRT_BLOCKING_TYPE ) ||
				( dirtInfo->rock_type == DIRT_LAKE_TYPE ))
				locPtr->walkable_off();
		}
	}
}
// --------- end of function World::add_dirt ----------//

// --------- begin of function World::remove_dirt ----------//
void World::remove_dirt(short dirtArrayRecno )
{
	Rock *dirtPtr = dirt_array[dirtArrayRecno];
	if( dirtPtr->rock_recno == 0 )		// deleted
		return;

	RockInfo *dirtInfo = rock_res.get_rock_info(dirtPtr->rock_recno);
	int x1 = dirtPtr->loc_x;
	int y1 = dirtPtr->loc_y;
	int x2 = x1 + dirtInfo->loc_width - 1;
	int y2 = y1 + dirtInfo->loc_height - 1;

	for( int y = y1; y <= y2; ++y )
	{
		int x = x1;
		Location *locPtr = get_loc(x,y);
		for( ; x <= x2; ++x, ++locPtr )
		{
			locPtr->remove_dirt();
		}
	}
}
// --------- end of function World::remove_dirt ----------//


// --------- begin of function World::gen_dirt ----------//
void World::gen_dirt(int nGrouped, int nLarge, int nSmall)
{
	// one 'large' (size 1 to 4) at the center
	// and a number 'small' (size 1 to 2) at the surroundings

	const int GAP=2;
	const int HUGE_ROCK_SIZE=8;
	const int LARGE_ROCK_SIZE=3;
	const int SMALL_ROCK_SIZE=1;

	int trial = (nGrouped + nLarge + nSmall) * 2;

	while( (nGrouped > 0 || nLarge > 0 || nSmall > 0) && --trial > 0)
	{
		// generate grouped dirt
		if( nGrouped > 0 )
		{
			short x = (GAP+LARGE_ROCK_SIZE)+m.random( max_x_loc - HUGE_ROCK_SIZE +1 - 2*(GAP+LARGE_ROCK_SIZE));
			short y = (GAP+LARGE_ROCK_SIZE)+m.random( max_y_loc - HUGE_ROCK_SIZE +1 - 2*(GAP+LARGE_ROCK_SIZE));
			short x2 = x + HUGE_ROCK_SIZE -1;
			short y2 = y + HUGE_ROCK_SIZE -1;

			//	if( can_add_dirt(x,y, x2,y2) )
			if( can_add_rock(x,y, x2,y2) && can_add_dirt(x,y, x2,y2) )
			{
				short rockRecno;
				rockRecno = rock_res.search("DEP",LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{

					add_dirt(rockRecno, x, y);
					char tempwidth =rockInfo->loc_width;
					char tempheight =rockInfo->loc_height;

					// add other smaller rock
					for(int subTrial = 12; subTrial > 0 ; --subTrial )
					{
						// sx from x-SMALL_ROCK_SIZE to x+4-1+SMALL_ROCK_SIZE
						short r = m.random(360);
						short sx = (short)(x+ tempwidth/2 + (tempwidth/2 +GAP) * cos(r * PI /180));
						short sy = (short)(y+ tempheight/2 + (tempheight/2 +GAP) * sin(r * PI /180));
						short sx2 = sx + LARGE_ROCK_SIZE-1;
						short sy2 = sy + LARGE_ROCK_SIZE-1;

						if( can_add_rock(sx,sy, sx2,sy2) && can_add_dirt(sx,sy, sx2,sy2) )
						{
							char scanRockType[2];
							scanRockType[0] = rockInfo->rock_type;
							scanRockType[1] = '\0';

							short rock2Recno;
							rock2Recno = rock_res.search(scanRockType,1,LARGE_ROCK_SIZE,1,LARGE_ROCK_SIZE,-1,0,
								terrain_res[get_loc(sx,sy)->terrain_id]->average_type );

							if(!rock2Recno)
								continue;

							RockInfo *rock2Info = rock_res.get_rock_info(rock2Recno);
							sx2 = sx + rock2Info->loc_width -1;
							sy2 = sy + rock2Info->loc_height -1;
							if( rock2Info->valid_terrain(terrain_res[get_loc(sx2,sy)->terrain_id]->average_type)
								&& rock2Info->valid_terrain(terrain_res[get_loc(sx,sy2)->terrain_id]->average_type)
								&& rock2Info->valid_terrain(terrain_res[get_loc(sx2,sy2)->terrain_id]->average_type) )
							{
								add_dirt(rock2Recno, sx, sy);
							}
						}
					}
					nGrouped--;
				}
			}
		}

		// generate stand-alone large rock
		if( nLarge > 0 )
		{
			short x = m.random( max_x_loc - LARGE_ROCK_SIZE);
			short y = m.random( max_y_loc - LARGE_ROCK_SIZE);
			short x2 = x + LARGE_ROCK_SIZE -1;
			short y2 = y + LARGE_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) && can_add_dirt( x, y, x2, y2) )
			{
				short rockRecno;
				rockRecno = rock_res.search("DEP", SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,SMALL_ROCK_SIZE+1,LARGE_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{
					add_dirt(rockRecno, x, y);
					nLarge--;
				}
			}
		}

		// generate stand-alone small dirt
		if( nSmall > 0 )
		{
			short x = m.random( max_x_loc - SMALL_ROCK_SIZE);
			short y = m.random( max_y_loc - SMALL_ROCK_SIZE);
			short x2 = x + SMALL_ROCK_SIZE -1;
			short y2 = y + SMALL_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) && can_add_dirt( x, y, x2, y2) )
			{
				short rockRecno;
				rockRecno = rock_res.search("DEP", 1,SMALL_ROCK_SIZE,1,SMALL_ROCK_SIZE,-1,0,
					terrain_res[get_loc(x,y)->terrain_id]->average_type );
				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;
				if( rockInfo->valid_terrain(terrain_res[get_loc(x2, y)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x, y2)->terrain_id]->average_type)
					&& rockInfo->valid_terrain(terrain_res[get_loc(x2,y2)->terrain_id]->average_type))
				{
					add_dirt(rockRecno, x, y);
					nSmall--;
				}
			}
		}
	}
}
// --------- end of function World::gen_dirt ----------//


//--------- begin of function World::gen_mountains ----------//
//
// [char*] mountainLayout - if this is given, mountains are
//									 generated based on the layout. Otherwise
//									 mountains are generated based on altitude.
//
// [int] adjustAmount - 100% normal. < 100% less, > 100% more.
//
void World::gen_mountains(char* mountainLayout, int adjustAmount)
{
	int minMountainAltitude;

	if( !mountainLayout )
	{
		//---- tally the number of locations in each altitude measure ---//

		#define MAX_ALTITUDE	255

		int	i, x, y;
		short altitudeCountArray[MAX_ALTITUDE+1];

		memset( altitudeCountArray, 0, sizeof(altitudeCountArray) );

		for( y=0 ; y<max_y_loc-HUGE_ROCK_SIZE ; y+=2 )
		{
			for( x=0 ; x<max_x_loc-HUGE_ROCK_SIZE ; x+=2 )
			{
				int t = get_altitude(x,y);

				err_when( t<0 || t>MAX_ALTITUDE );

				altitudeCountArray[t]++;
			}
		}

		//---- determine the altitude level to add mountains ----//

		int mountainAddCount = MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC
									  * ( 30+m.random(20) ) / 100 / (HUGE_ROCK_SIZE*HUGE_ROCK_SIZE/2);		// 10% to 30% of mountains

		if( adjustAmount )
			mountainAddCount = mountainAddCount * adjustAmount / 100;

		for( i=MAX_ALTITUDE ; i>=0 ; i-- )
		{
			mountainAddCount -= altitudeCountArray[i];

			if( mountainAddCount <= 0 )
			{
				minMountainAltitude = i;
				break;
			}
		}
	}

	//---------- Generate Huge Rocks ---------------------//

	int x, y;

	for( y=0 ; y<max_y_loc-HUGE_ROCK_SIZE ; y++ )
	{
		for( x=0 ; x<max_x_loc-HUGE_ROCK_SIZE ; x++ )
		{
			int mX = (x + m.random(5) - 2) * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY = (y + m.random(5) - 2) * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			int mX2 = x * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY2 = y * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			if( mountainLayout )
			{
				if( !mountainLayout[ mY*MOUNTAIN_LAYOUT_X_COUNT+mX ] &&		// additional areas around the marked area
					 !mountainLayout[ mY2*MOUNTAIN_LAYOUT_X_COUNT+mX2 ] )        // the marked area
				{
					continue;
				}
			}
			else
			{
				if( get_altitude(x,y) < minMountainAltitude )
					continue;
			}

			//------------------------------------//

			int x2 = x + HUGE_ROCK_SIZE -1;
			int y2 = y + HUGE_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) )
			{
				short rockRecno;

				rockRecno = rock_res.search("R",LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,LARGE_ROCK_SIZE +1,HUGE_ROCK_SIZE,-1,0,0 );

				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;

				add_rock(rockRecno, x, y);
			}
		}
	}

	//------ Generate Small Rocks to fill gaps between huge rocks -------//

	for( y=0 ; y<max_y_loc-SMALL_ROCK_SIZE ; y++ )
	{
		for( x=0 ; x<max_x_loc-SMALL_ROCK_SIZE ; x++ )
		{
			int mX = (x + m.random(5) - 2) * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY = (y + m.random(5) - 2) * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			int mX2 = x * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY2 = y * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			if( mountainLayout )
			{
				if( !mountainLayout[ mY*MOUNTAIN_LAYOUT_X_COUNT+mX ] &&		// additional areas around the marked area
					 !mountainLayout[ mY2*MOUNTAIN_LAYOUT_X_COUNT+mX2 ] )        // the marked area
				{
					continue;
				}
			}
			else
			{
				if( get_altitude(x,y) < minMountainAltitude )
					continue;
			}

			//------------------------------------//

			if( m.random(3) != 0 )		// add randomly
				continue;

			int x2 = x + SMALL_ROCK_SIZE -1;
			int y2 = y + SMALL_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) )
			{
				short rockRecno;

				rockRecno = rock_res.search("R", 1,SMALL_ROCK_SIZE,1,SMALL_ROCK_SIZE,-1,0,0 );

				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;

				add_rock(rockRecno, x, y);
			}
		}
	}

	//------ Generate the smallest Rocks to fill gaps between huge rocks -------//

	for( y=0 ; y<max_y_loc ; y++ )
	{
		for( x=0 ; x<max_x_loc ; x++ )
		{
			int mX = (x + m.random(3) - 1) * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY = (y + m.random(3) - 1) * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			int mX2 = x * MOUNTAIN_LAYOUT_X_COUNT / MAX_WORLD_X_LOC;
			int mY2 = y * MOUNTAIN_LAYOUT_Y_COUNT / MAX_WORLD_Y_LOC;

			if( mountainLayout )
			{
				if( !mountainLayout[ mY*MOUNTAIN_LAYOUT_X_COUNT+mX ] &&		// additional areas around the marked area
					 !mountainLayout[ mY2*MOUNTAIN_LAYOUT_X_COUNT+mX2 ] )        // the marked area
				{
					continue;
				}
			}
			else
			{
				if( get_altitude(x,y) < minMountainAltitude )
					continue;
			}

			//------------------------------------//

			if( m.random(3) != 0 )		// add randomly
				continue;

			int x2 = x + SMALLEST_ROCK_SIZE -1;
			int y2 = y + SMALLEST_ROCK_SIZE -1;

			if( can_add_rock( x, y, x2, y2) )
			{
				short rockRecno;

				rockRecno = rock_res.search("R", 1,SMALLEST_ROCK_SIZE,1,SMALLEST_ROCK_SIZE,-1,0,0 );

				if( !rockRecno )
					continue;

				RockInfo *rockInfo = rock_res.get_rock_info(rockRecno);
				x2 = x + rockInfo->loc_width - 1;
				y2 = y + rockInfo->loc_height -1;

				add_rock(rockRecno, x, y);
			}
		}
	}

	// -------- break some rocks to road ------------//

	/* {
		const highPointMax = MAX_MAP_WIDTH * MAX_MAP_HEIGHT / 400;
		int pointMax = (max_x_loc * max_y_loc) / 400;
		if( pointMax > highPointMax )
			pointMax = highPointMax;
		short pointX[highPointMax], pointY[highPointMax];

		// generate points

		int i, j, x, y;
		for( i = 0; i < pointMax; ++i )
		{
			pointX[i] = m.random(max_x_loc);
			pointY[i] = m.random(max_y_loc);
		}
		int pointCount = pointMax;

		for( i = pointCount-1; i >= 0; --i )
		{
			int dropFlag = 0;

			// drop points which are close together

			for( j = 0; j < pointCount; ++j )
			{
				if( j ==i )
					continue;
				if( m.points_distance(pointX[i], pointY[i], pointX[j], pointY[j]) < 20 )
				{
					dropFlag++;
					break;
				}
			}
			
			if( dropFlag )
			{
				m.del_array_rec( pointX, pointCount, sizeof(short), i+1 );
				m.del_array_rec( pointY, pointCount, sizeof(short), i+1 );
				--pointCount;
			}
		}

		// find how many points are very disconnected from other points (blocked by mountain)

		for( i = pointCount-1; i >= 0; --i )
		{
			for( j = 0; j < pointCount; ++j )
			{
				int x = pointX[i];
				int y = pointY[i];
			}
		}

		// remove mountain on the line connecting two points

	}
	*/
}
//--------- end of function World::gen_mountains ----------//
