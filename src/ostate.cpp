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

// Filename     : OSTATE.CPP
// Description  : state for campaign

#include <ogame.h>
#include <ostate.h>
#include <onation2.h>
#include <ocampgn.h>
#include <stdlib.h>
#include <math.h>

// ------- define static variable -----//

unsigned char StateArray::fill_state_recno;

//-------- begin of function StateArray::StateArray ----------//
//
StateArray::StateArray()
{
	memset( this, 0, sizeof(StateArray) );
}
//-------- end of function StateArray::StateArray ----------//


//-------- begin of function StateArray::~StateArray ----------//
//
StateArray::~StateArray()
{
	deinit();
}
//-------- end of function StateArray::StateArray ----------//


//-------- begin of function StateArray::init ----------//
//
void StateArray::init( int w, int h )
{
	err_when( w > MAX_STATE_MAP_WIDTH );
	err_when( h > MAX_STATE_MAP_HEIGHT );

	deinit();

	max_x_loc = w;
	max_y_loc = h;
	loc_matrix = (StateLocation *)mem_add( sizeof(StateLocation) * w * h );
	memset( loc_matrix, 0, sizeof(StateLocation) * w * h );

	state_count = 0;
	state_info_array = 0;

	init_flag = 1;
}
//-------- end of function StateArray::init ----------//


//-------- begin of function StateArray::deinit ----------//
//
void StateArray::deinit()
{
	if(init_flag)
	{
		mem_del(loc_matrix);
		loc_matrix = NULL;

		if( state_info_array )
		{
			mem_del( state_info_array );
			state_info_array = NULL;
		}

		init_flag = 0;
	}
}
//-------- end of function StateArray::deinit ----------//


//-------- begin of function StateArray::generate ----------//
//
void StateArray::generate( int stateCount )
{
	err_when( stateCount > MAX_STATE );

	//-------- allocate memory ------- //

	state_count = stateCount;
	state_info_array = (StateInfo *)mem_add( sizeof(StateInfo) * stateCount );

	//-------- generate area ----------//

	int needRetry = 0;
	int avgSeparation = m.sqrt( max_x_loc * max_y_loc / state_count );

	int loopCount = 0;
	do
	{
		if( ++loopCount > 1000 )
		{
			err.run( "Fail generating states" );
		}

		needRetry = 0;

		//------ reset array --------//

		memset( state_info_array, 0, sizeof(StateInfo) * state_count );

		//------- randomize center location --------//

		int s;
		for( s = 1; s <= state_count; ++s )
		{
			StateInfo *state = operator[](s);
			state->state_recno = s;

			int trial = s * 16 + 1;
			int xLoc, yLoc;
			while( --trial >= 0 )
			{
				xLoc = random(max_x_loc);
				yLoc = random(max_y_loc);

				int closeFlag = 0;

				// check separation
				for( int t = 1; t < s; ++t )
				{
					if( m.diagonal_distance(xLoc, yLoc, operator[](t)->center_x, operator[](t)->center_y)
						* 2 < avgSeparation )
						closeFlag = 1;
				}
				if( !closeFlag )
					break;
			}

			if( trial < 0 )
			{
				needRetry = 1;
				continue;
			}

			state->o_center_x = state->center_x = xLoc;
			state->o_center_y = state->center_y = yLoc;
			state->strength_x1 = (max_x_loc+random(max_x_loc)) * (max_y_loc+random(max_y_loc)) * (max_y_loc+random(max_y_loc)) / 8;
			state->strength_y1 = (max_x_loc+random(max_x_loc)) * (max_x_loc+random(max_x_loc)) * (max_y_loc+random(max_y_loc)) / 8;

//			state->strength_x2 = (max_y_loc+random(max_y_loc)) * (max_y_loc+random(max_y_loc)) / 4;
//			state->strength_y2 = (max_x_loc+random(max_x_loc)) * (max_x_loc+random(max_x_loc)) / 4;
//			state->strength_xy = (max_x_loc+random(max_x_loc)) * (max_y_loc+random(max_y_loc)) / 4;
			state->strength_x2 = (max_y_loc+random(max_y_loc/2)) * (max_y_loc+random(max_y_loc/2)) / 4;
			state->strength_y2 = (max_x_loc+random(max_x_loc/2)) * (max_x_loc+random(max_x_loc/2)) / 4;
			state->strength_xy = (max_x_loc+random(max_x_loc/2)) * (max_y_loc+random(max_y_loc/2)) / 8;	// this coeff is smaller
		}

	} while(needRetry);

	//------ first pass, set area closest to a capital to that state ----- //

	{
		int x, y;
		for( y = 0; y < max_y_loc; ++y )
		{
			StateLocation *locPtr = get_loc(0, y);
			for( x = 0; x < max_x_loc; ++x, ++locPtr )
			{
				int s;
				unsigned char minState = 0;
				double minDist = 0.0;
				for( s = 1; s <= state_count; ++s )
				{
					double d = operator[](s)->distance_func( (float)x, (float)y );
					if( !minState || d < minDist )
					{
						minDist = d;
						minState = s;
					}
				}

				locPtr->state_recno = minState;
			}
		}
	}

	// ----- eliminate seperated area -------//

	{
		int s;
		for( s = 1; s <= state_count; ++s )
		{
			// replace all loc of state state to zero

			int x, y;
			for( y = 0; y < max_y_loc; ++y )
			{
				StateLocation *locPtr = get_loc(0, y);
				for( x = 0; x < max_x_loc; ++x, ++locPtr )
				{
					if( locPtr->state_recno == s )
					{
						locPtr->state_recno = 0;
					}
				}
			}

			// fill again at center

			fill_state_recno = (unsigned char) s;
			StateInfo *state = operator[](s);
			fill_state( state->center_x, state->center_y );
		}

		// do in reverse direction to remove odded out

		for( s = state_count; s >= 1; --s )
		{
			// replace all loc of state state to zero

			int x, y;
			for( y = 0; y < max_y_loc; ++y )
			{
				StateLocation *locPtr = get_loc(0, y);
				for( x = 0; x < max_x_loc; ++x, ++locPtr )
				{
					if( locPtr->state_recno == s )
					{
						locPtr->state_recno = 0;
					}
				}
			}

			// fill again at center

			fill_state_recno = (unsigned char) s;
			StateInfo *state = operator[](s);
			fill_state( state->center_x, state->center_y );
		}
	}

	//------ set connection -------//

	{
		int x, y;
		for( y = 0; y < max_y_loc; ++y )
		{
			for( x = 0; x < max_x_loc; ++x )
			{
				unsigned char s0 = get_loc(x,y)->state_recno;
				if( x+1 < max_x_loc )		// check right location
				{
					unsigned char s1 = get_loc(x+1,y)->state_recno;
					if( s1 != s0 )
						set_adjacent(s0, s1);
				}
				if( y+1 < max_y_loc )		// check down location
				{
					unsigned char s2 = get_loc(x,y+1)->state_recno;
					if( s2 != s0 )
						set_adjacent(s0, s2);
				}
				if( x+1 < max_x_loc && y+1 < max_y_loc )	// check down right location
				{
					unsigned char s3 = get_loc(x+1,y+1)->state_recno;
					if( s3 != s0 )
						set_adjacent(s0, s3);
				}
			}
		}
	}

	//------- update center as centroid of each area --------//

	{
		long sumX[MAX_STATE];
		long sumY[MAX_STATE];
		long stateArea[MAX_STATE];
		memset( sumX, 0, sizeof(sumX) );
		memset( sumY, 0, sizeof(sumY) );
		memset( stateArea, 0, sizeof(stateArea) );

		//------ find centroid of each state -------//

		int x, y;
		for( y = 0; y < max_y_loc; ++y )
		{
			StateLocation *locPtr = get_loc(0, y);
			for( x = 0; x < max_x_loc; ++x, ++locPtr )
			{
				if( locPtr->state_recno )
				{
					++stateArea[locPtr->state_recno -1];
					sumX[locPtr->state_recno -1] += x;
					sumY[locPtr->state_recno -1] += y;
				}
			}
		}

		//------ update new center of each state ------//

		int s;
		int centerRadius = avgSeparation / 8;
		for( s = 1; s <= state_count; ++s )
		{
			int centerX = sumX[s-1] / stateArea[s-1];
			int centerY = sumY[s-1] / stateArea[s-1];

			//----- if centroid is not of that state, do not update center -----//

			if( centerX >= 0 && centerX < max_x_loc &&
				centerY >= 0 && centerY < max_y_loc &&
				get_loc( centerX, centerY)->state_recno == s )
			{
				operator[](s)->center_x = centerX;
				operator[](s)->center_y = centerY;
			}
		}
	}

	//-------- set bound flags of state --------//

	{
		int x, y, s;

		// set west/east bound flag
		for( y = 0; y < max_y_loc; ++y )
		{
			s = get_loc(0, y)->state_recno;
			if(s)
			{
				operator[](s)->flags |= STATE_WEST_BOUND;
				operator[](s)->west_bound_count++;
			}
			s = get_loc(max_x_loc-1, y)->state_recno;
			if(s)
			{
				operator[](s)->flags |= STATE_EAST_BOUND;
				operator[](s)->east_bound_count++;
			}
		}

		// set north/south bound flag
		for( x = 0; x < max_x_loc; ++x )
		{
			s = get_loc(x, 0)->state_recno;
			if(s)
			{
				operator[](s)->flags |= STATE_NORTH_BOUND;
				operator[](s)->north_bound_count++;
			}
			s = get_loc(x, max_y_loc-1)->state_recno;
			if(s)
			{
				operator[](s)->flags |= STATE_SOUTH_BOUND;
				operator[](s)->south_bound_count++;
			}
		}
	}
}
//-------- end of function StateArray::generate ----------//


//--------- Begin of function StateArray::set_adjacent -------//
//
void StateArray::set_adjacent(int state1, int state2)
{
	err_when( state1 < 0 || state1 > state_count);
	err_when( state2 < 0 || state2 > state_count);
	if( state1 == 0 || state2 == 0)
		return;

	if( state1 == state2 )
		return;

	operator[](state1)->set_adjacent(state2);
	operator[](state2)->set_adjacent(state1);
}
//--------- End of function StateArray::set_adjacent -------//


//--------- Begin of function StateArray::is_adjacent -------//
//
int StateArray::is_adjacent(int state1, int state2)
{
	err_when( state1 <= 0 || state1 > state_count);
	err_when( state2 <= 0 || state2 > state_count);

	return operator[](state1)->is_adjacent(state2);
}
//--------- End of function StateArray::is_adjacent -------//


//-------- begin of function StateArray::get_loc ----------//
//
StateLocation* StateArray::get_loc(int x, int y)
{
	return loc_matrix + max_x_loc * y + x;
}
//-------- end of function StateArray::get_loc ----------//


//-------- begin of function StateArray::operator[] ----------//
//
StateInfo* StateArray::operator[] (int s)
{
	err_when(s < 1 || s > state_count );
	return state_info_array + s - 1;
}
//-------- end of function StateArray::operator[] ----------//


//-------- begin of function StateInfo::set_adjacent ----------//
//
void StateInfo::set_adjacent(int state2)
{
	err_when( state2 <= 0 || state2 > MAX_STATE );
	adj_offset_bit |= 1 << (state2-1);
}
//-------- end of function StateInfo::set_adjacent ----------//


//-------- begin of function StateInfo::is_adjacent ----------//
//
int StateInfo::is_adjacent(int state2)
{
	err_when( state2 <= 0 || state2 > MAX_STATE );
	return adj_offset_bit & (1 << (state2-1));
}
//-------- end of function StateInfo::is_adjacent ----------//


//-------- begin of function StateArray::is_adjacent_to_nation ----------//
//
// Return whether the state is adjacent to any state of the
// given nation.
//
int StateArray::is_adjacent_to_nation(int state1, int campaignNationRecno)
{
	StateInfo* stateInfo = operator[](state1);

	for( int i=size() ; i>0 ; i-- )
	{
		if( state1==i )
			continue;

		if( stateInfo->is_adjacent(i) &&
			 operator[](i)->campaign_nation_recno == campaignNationRecno )
		{
			return i;
		}
	}

	return 0;
}
//-------- end of function StateArray::is_adjacent_to_nation ----------//


//---- Begin of function StateInfo::add_game_nation ----//

int StateInfo::add_game_nation()
{
	err_when( !game.is_campaign_mode() );

	CampaignNation* cNation = game.campaign()->get_nation(campaign_nation_recno);

	int nationRecno = nation_array.new_nation(
							cNation->is_human() ? NATION_HUMAN : NATION_MONSTER,
							cNation->nation_type==CAMPAIGN_NATION_OWN ? NATION_OWN : NATION_AI,
							cNation->race_id, cNation->color_scheme_id );

	if( !nationRecno )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->campaign_nation_recno = campaign_nation_recno;

	if( cNation->custom_name[0] )
		nation_array.set_custom_nation_name( nationRecno, cNation->custom_name );

	return nationRecno;
}
//---- End of function StateInfo::add_game_nation -----//


//---- Begin of function StateInfo::distance_func ----//

double StateInfo::distance_func( float x, float y )
{
	double dx = fabs(x - o_center_x);
	double dy = fabs(y - o_center_y);

	return (strength_x2 * dx + strength_x1) * dx
		+ (strength_y2 * dy + strength_y1 ) * dy
		+ strength_xy * dx * dy;
}
//---- End of function StateInfo::distance_func -----//


//---- Begin of function StateInfo::get_adjacent ----//

int StateInfo::get_adjacent(unsigned char *stateRecnoArray)
{
	if( adj_offset_bit == 0 )
		return 0;

	int stateCount = 0;
	unsigned long bits = adj_offset_bit;
	for( int s = 1; s <= MAX_STATE; ++s, bits >>= 1 )
	{
		if( bits & 1 )
		{
			if( stateRecnoArray )
				stateRecnoArray[stateCount] = s;
			stateCount++;
		}
	}

	return stateCount;
}
//---- End of function StateInfo::get_adjacent -----//


//---- Begin of function StateInfo::is_near_border ----//

// so caller can judge the return value to reject a state is not
// in the border if return value is too low
int StateInfo::is_near_border()
{
	return west_bound_count + north_bound_count + east_bound_count + south_bound_count;
}
//---- End of function StateInfo::is_near_border -----//


//-------- Begin of function StateArray::nation_state_count ------//
//
// Return the number of states owned by the specific nation.
//
int StateArray::nation_state_count(int cNationRecno)
{
	int stateCount=0;

	for( int i=size() ; i>0 ; i-- )
	{
		if( operator[](i)->campaign_nation_recno == cNationRecno )
			stateCount++;
	}

	return stateCount;
}
//-------- End of function StateArray::nation_state_count ------//


//-------- Begin of function StateArray::are_nation_adjacent ------//
//
// Return whether the given two nations are geographically next to
// each other.
//
int StateArray::are_nation_adjacent(int cNation1, int cNation2)
{
	for( int i=size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = operator[](i);

		if( stateInfo->campaign_nation_recno == cNation1 &&
			 is_adjacent_to_nation(i, cNation2) )
		{
			return 1;
		}
	}

	return 0;
}
//-------- End of function StateArray::are_nation_adjacent ------//


//---------- Begin of function StateArray::fill_region -----//
//
// fill state_recno from 0 to fill_state_recno
//
void StateArray::fill_state(int x, int y)
{
	err_when( x < 0 || x >= max_x_loc || y < 0 || y >= max_y_loc);

	int left, right;
	// Location *locPtr;

	// extent x to left and right
	for( left = x; left >= 0 && !get_loc(left,y)->state_recno; --left)
	{
		get_loc(left,y)->state_recno = fill_state_recno;
	}
	++left;

	for( right=x+1; right < max_x_loc && !get_loc(right,y)->state_recno; ++right)
	{
		get_loc(right,y)->state_recno = fill_state_recno;
	}
	--right;

	// ------- scan line below ---------//
	y++;
	if( y < max_y_loc )
	{
		for( x = left>0?left-1:0 ; x <= right+1 && x < max_x_loc; ++x )
		{
			if( !get_loc(x,y)->state_recno )
			{
				fill_state(x,y);
			}
		}
	}

	// ------- scan line above -------- //
	y -= 2;
	if( y >= 0)
	{
		for( x = left>0?left-1:0 ; x <= right+1 && x < max_x_loc; ++x )
		{
			if( !get_loc(x,y)->state_recno )
			{
				fill_state(x,y);
			}
		}
	}
}
//---------- End of function StateArray::fill_region -----//


// --------- begin of function StateArray::random ---------//

unsigned long StateArray::random( unsigned long  u)
{
	if( game.is_campaign_mode() )
		return game.campaign()->random(u);
	else
		return m.random(u);
}
// --------- end of function StateArray::random ---------//
