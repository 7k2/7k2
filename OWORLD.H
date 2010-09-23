//Filename    : OWORLD.H
//Description : Header file of Object World

#ifndef __OWORLD_H
#define __OWORLD_H

#include <OMATRIX.H>
#include <OMISC.H>

//----------- Define constant ------------//

#define EXPLORE_RANGE   10
#define SCAN_FIRE_DIST 4

//--------------- Define constant -----------------//

#define MIN_LAND_COST   500000       // Minimum land cost even there is no population at all

//------- define terrain map --------//

#define MIN_GRASS_HEIGHT    100
#define MIN_HILL_HEIGHT  	 230
#define MIN_MOUNTAIN_HEIGHT 242
#define MIN_ICE_HEIGHT 		 252


// ------- parameter selectedFlag for go_loc ----------//

#define GO_LOC_SELECT_NONE      0
#define GO_LOC_SELECT_ANY       1
#define GO_LOC_SELECT_LAND      2
#define GO_LOC_SELECT_SEA       2
#define GO_LOC_SELECT_AIR       3
#define GO_LOC_SELECT_SITE      4
#define GO_LOC_SELECT_ANY_UNIT  5
#define GO_LOC_SELECT_FIRM      6
#define GO_LOC_SELECT_TOWN      7

//---------------- Define class World -------------//

class MapMatrix;
class ZoomMatrix;
class File;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
class Weather;
class Plasma;

#define MAX_WORLD_X_LOC  (World::max_x_loc)
#define MAX_WORLD_Y_LOC  (World::max_y_loc)

class World
{
public:
	MapMatrix    *map_matrix;
	ZoomMatrix   *zoom_matrix;
	Location     *loc_matrix;
	Corner       *corner_matrix;

	char			 scan_fire_x;				// cycle from 0 to SCAN_FIRE_DIST-1
	char			 scan_fire_y;
	char			 lightning_signal;
	char			 dummy1;
	int			 plant_count;
	int			 plant_limit;
	long			 gen_map_random_seed;

	// ------- variable for scrolling ------//

	short			 dummy2;
	char			 last_scroll_method;

	// ------ scroll method 1 --------//

	char			 last_scroll_flag;		// bit 0/1/2/3 represend scroll rightleft/down/up respectively
	short			 last_scroll_top_x_loc;
	short			 last_scroll_top_y_loc;
	DWORD			 begin_scroll_time;
	short			 begin_scroll_top_x_loc;
	short			 begin_scroll_top_y_loc;

	// ------- scroll method 0 -------//

	DWORD	 		 next_scroll_time;		 // next scroll time

	//--------- static member vars --------------//

	static int   max_x_loc , max_y_loc;		   // these must be static vars as MAX_WORLD_X_LOC & MAX_WORLD_Y_LOC are defined to use them.
	static char  check_unit_space_avoid_mountain;

public:
	World();
	~World();

	void 		init();
	void 		deinit();

	void 		generate_map(int loadGame);
	void 		generate_map2();
	void 		assign_map();

	void 		paint();
	void 		refresh();
	void		disp_zoom();		//	{ zoom_matrix->disp(); }

	void		load_map(char*);

	int  		detect();
	int		detect_firm_town();
	void 		go_loc(int xLoc, int yLoc, int selectedFlag=0);
	void		disp_next(int seekDir, int sameNation);
	void 		disp_next_with_filter(int seekDir);

	int  		write_file(File*);
	int  		read_file(File*);

public:
	#ifdef DEBUG3
		Location* get_loc(int xLoc,int yLoc);
		BYTE		 get_region_id(int xLoc,int yLoc);
	#else
		Location* get_loc(int xLoc,int yLoc)
						{ return loc_matrix + MAX_WORLD_X_LOC * yLoc + xLoc; }

		BYTE		 get_region_id(int xLoc,int yLoc)
						{ return loc_matrix[MAX_WORLD_X_LOC*yLoc+xLoc].region_id; }
	#endif

	Corner *get_corner(int xLoc, int yLoc)
	{ return corner_matrix + yLoc *(max_x_loc+1) + xLoc; }
	LocationCorners *get_loc_corner(int xLoc, int yLoc, LocationCorners *lc);
	int		interpolate_z(int curX, int curY);
	int 		get_altitude(int xLoc, int yLoc);

	int 		distance_rating(int xLoc1, int yLoc1, int xLoc2, int yLoc2);
	int		effective_distance(int placeId1, int placeId2);

	void		unveil(int xLoc1, int yLoc1, int xLoc2, int yLoc2);
	void		explore(int xLoc1, int yLoc1, int xLoc2, int yLoc2);
	int  		is_fully_explored(int xLoc1, int yLoc1, int xLoc2, int yLoc2);	// fully explored
	int  		is_explored(int xLoc1, int yLoc1, int xLoc2, int yLoc2);	// any location explored
	int		is_explored_quick( int xLoc1, int yLoc1, int xLoc2, int yLoc2);	// four corner only
	// always call unveil before visit //
	void		visit(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int range, int extend =0);
	void		visit_shell(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int visitLevel);

	int		can_build_firm(int xLoc1, int yLoc1, int firmId, short unitRecno= -1);
	int		can_build_town(int xLoc1, int yLoc1, short unitRecno= -1);
	int		can_build_wall(int xLoc1, int yLoc1, short nationRecno);
	int		can_build_site(int xLoc1, int yLoc1, int siteWidth, int siteHeight, int outerSpace );
	int		can_destruct_wall(int xLoc1, int yLoc1, short nationRecno);
	void		build_wall_tile(int xLoc1, int yLoc1, short nationRecno, char remoteAction);
	void		destruct_wall_tile(int xLoc1, int yLoc1, short nationRecno, char remoteAction);

	int 		locate_space(int& xLoc1, int& yLoc1, int xLoc2, int yLoc2,
										int spaceLocWidth, int spaceLocHeight, int mobileType=UNIT_LAND, int regionId=0, int buildFlag=0);
	int		check_unit_space(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int mobileType=UNIT_LAND, int buildFlag=0);

	int 		locate_space_random(int& xLoc1, int& yLoc1, int xLoc2, int yLoc2,
										  int spaceLocWidth, int spaceLocHeight, int maxTries,
										  int regionId=0, int buildSite=0, char teraMask=1);

	int 		net_attacker_power_in_area(int targetXLoc, int targetYLoc, int attackerWorldRecno,
				int defenderWorldRecno, int returnIfWar, int& hasWar, int scanRange);

	int		is_adjacent_region( int x, int y, int regionId );

	void 		draw_link_line(int srcFirmId, int srcTownRecno, int srcXLoc1, int srcYLoc1, int srcXLoc2, int srcYLoc2, int giveEffectiveDis=0);

	void 		set_all_power();
	void 		set_power(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int nationRecno);
	void 		restore_power(int xLoc1, int yLoc1, int xLoc2, int yLoc2, int townRecno, int firmRecno);
	void		set_surr_power_off(int xLoc, int yLoc);

	void		process();
	void		process_visibility();
	void		next_day();

	//------- functions related to plant's growth, see ow_plant.cpp -----//

	void		plant_ops();
	void		plant_grow(int pGrow =4, int scanDensity = 16);
	void		plant_reprod(int pRepord =1, int scanDensity = 16);
	void		plant_death(int scanDensity = 16, int scanRadius =3);
	void		plant_spread(int pSpread =5);
	void		plant_init(int plantTrial, int scanRadius);
	void		plant_spray(short *plantIdArray, char area, char distance, short x, short y);

	//------- functions related to fire's spreading, see ow_fire.cpp ----//

	void		init_fire();
	void		spread_fire(Weather &);
	void		setup_fire(short x, short y, char fireStrength = 30);

	//------- function related to city wall ----------//
	void		build_wall_section(short x1, short y1, short x2, short y2,
				short townRecno, short initHp = 99);
	void		build_wall(int townRecno, short initHp = 99);

	void		open_west_gate(short x2, short y1, short townRecno);
	void		open_east_gate(short x1, short y1, short townRecno);
	void		open_north_gate(short x1, short y2, short townRecno);
	void		open_south_gate(short x1, short y1, short townRecno);

	int		form_wall(short x, short y, short maxRecur=0);
	void		form_world_wall();
	int		correct_wall(short x, short y, short maxRecur=2);

	//-------- function related to rock ----------//
	
	void		add_rock(short rockId, short x1, short y1);
	void		add_dirt(short dirtId, short x1, short y1);
	int		can_add_rock(short x1, short y1, short x2, short y2);
	int		can_add_dirt(short x1, short y1, short x2, short y2);
	void		remove_rock(short rockRecno);
	void		remove_dirt(short dirtRecno);

	void		gen_rocks(int nGrouped=0, int nLarge=0, int nSmall=0);
	void		gen_mountains(char* mountainLayout=0, int adjustAmount=0);

	//-------- function related to road --------//

	void		gen_road(Plasma &, short x1, short y1, short x2, short y2);
	void		set_road_id( Plasma & );

	// ------ function related to weather ---------//

   void     earth_quake();
   void     lightning_strike(short x, short y, short radius=0);

private:
	int	  	detect_scroll();
	// int		detect_firm_town();

	//--------- ambient sound functions --------//

	void		process_ambient_sound();

	//--- called by generate_map() only ---//

	void    add_base_level();
	void    gen_plasma_map();
	void    set_tera_id(Plasma &);
	void    remove_odd(Plasma &, short x, short y, short recur);
	void    smooth_altitude(Plasma &);
	void    set_tera_height(Plasma &);
	void    set_climate();
	void	  set_loc_flags();
	void	  substitute_pattern();
	void    set_region_id();
	void    fill_region(short x, short y);
	void    gen_dirt(int nGrouped, int nLarge, int nSmall);
	void    set_harbor_bit();

	// private function called by build_wall_section
	int	can_build_area(short x1, short y1, short x2, short y2);
	void	build_west_gate(short x1, short y1, short townRec, short initHp);
	void	build_east_gate(short x1, short y1, short townRec, short initHp);
	void	build_north_gate(short x1, short y1, short townRec, short initHp);
	void	build_south_gate(short x1, short y1, short townRec, short initHp);

	void	build_west_wall(short x1, short y1, short y2, short townRec, short initHp);
	void	build_east_wall(short x1, short y1, short y2, short townRec, short initHp);
	void	build_north_wall(short x1, short x2, short y1, short townRec, short initHp);
	void	build_south_wall(short x1, short x2, short y1, short townRec, short initHp);

	// see OGENHILL.CPP
	void	gen_hills(int terrainType);
	void	put_hill_set(short *px, short *py, short hSetId);
	void	put_hill_pattern(short *px, short *py, unsigned char patternId);
	void	fill_hill(short x, short y);
};

//--------- Begin of function World::distance_rating --------//
//
inline int World::distance_rating(int xLoc1, int yLoc1, int xLoc2, int yLoc2)
{
	int curDistance = m.points_distance(xLoc1, yLoc1, xLoc2, yLoc2);

	return 100 - 100 * curDistance / MAX_WORLD_X_LOC;
}
//----------- End of function World::distance_rating --------//


extern World world;

#endif
