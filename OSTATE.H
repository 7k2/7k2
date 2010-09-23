// Filename    : OSTATE.H
// Description : Header file of state for campaign


#ifndef __OSTATE_H
#define __OSTATE_H

// ------- define constant ------//

#define MAX_STATE_MAP_WIDTH 100
#define MAX_STATE_MAP_HEIGHT 100
#define MAX_STATE 31

// ----- flags --------//

enum
{
	STATE_WEST_BOUND     = 1,
	STATE_NORTH_BOUND    = 2,
	STATE_EAST_BOUND     = 4,
	STATE_SOUTH_BOUND    = 8,
};

// ------ define struct StateLocation -------//

struct StateLocation
{
	unsigned char state_recno;
};


// ------ define class StateInfo -------//

class File;

class StateInfo
{
public:
	unsigned char state_recno;
	short		campaign_nation_recno;
	unsigned short flags;		// bit0=west bound, bit1=north bound, bit2=east bound, bit3=south bound, 
	short		center_x;			// center of centroid
	short		center_y;

	// ------- adjacent state bit -------//

	unsigned long	adj_offset_bit;

	// ------ for generate only -------//

	short		o_center_x;		// center for calc dist_func
	short		o_center_y;

	long		strength_x1;
	long		strength_x2;
	long		strength_y1;
	long		strength_y2;
	long		strength_xy;

	short		west_bound_count;
	short		north_bound_count;
	short		east_bound_count;
	short		south_bound_count;

public:
	
	// ----- distance function -------//

	double	distance_func( float x, float y );

	//------ adjacent state function -----//

	void		set_adjacent(int state2);
	int		is_adjacent(int state2);
//	int		is_adjacent_to_nation(int campaignNationRecno); // use StateArray::is_adjacent_to_nation
	int		is_near_border();
	int		get_adjacent(unsigned char *stateRecnoArray);

	//------ nation function ------//

	void		set_campaign_nation(char n)			{ campaign_nation_recno = n; }

	//---- add a nation to nation_array ----//

	int		add_game_nation();
};


// ------ define class StateArray -------//

class StateArray
{
public:
	int		init_flag;

	int		max_x_loc;
	int		max_y_loc;
	StateLocation*	loc_matrix;

	int		size()			{ return state_count; }

private:
	int			state_count;
	StateInfo*	state_info_array;

static unsigned char fill_state_recno;		// only for fill_state

public:
	StateArray();
	~StateArray();

	void		init( int w, int h);
	void		deinit();

	// ------ generate function --------//

	void		generate( int stateCount );

	// ------ adjacent function --------//

	void		set_adjacent(int state1, int state2);
	int		is_adjacent(int state1, int state2);
	int		is_adjacent_to_nation(int state1, int campaignNationRecno);
	int		are_nation_adjacent(int campaignNation1, int campaignNation2);


	int		nation_state_count(int campaignNationRecno);		// number of states owned by this nation

	// -------------------------------//

	StateLocation *get_loc(int x, int y);
	StateInfo*	operator[] (int stateRecno);

	//------- file functions -------//

	int		write_file(File* filePtr);
	int		read_file(File* filePtr);

private:

	// -------- private function used in generate map -------//

	void		fill_state( int x, int y );	// set fill_state_recno before call
	unsigned long	random( unsigned long );
};


extern StateArray state_array;

#endif