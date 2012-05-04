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

//Filename    : OLOCATE.H
//Description : class Location

#ifndef __OLOCATE_H
#define __OLOCATE_H

#include <win32_compat.h>

//-------- Define constants ---------//


//----- Define bit meanings Location::flag ------//
#define 	LOCATE_WALK_LAND			0x01
#define	LOCATE_WALK_SEA			0x02
#define	LOCATE_COAST				0x08

// ----- govern the usage of extra_para ---------//
#define	LOCATE_SITE_MASK			0xf0
#define	LOCATE_HAS_SITE			0x10
#define	LOCATE_HAD_WALL			0x20
#define	LOCATE_HAS_DIRT			0x30
#define	LOCATE_SITE_RESERVED		0xf0
//	occupied by other block such as hill, plant

// ----- govern the usage of cargo_recno -------//
#define	LOCATE_BLOCK_MASK			0xf00
#define	LOCATE_IS_HILL				0x100
#define	LOCATE_IS_PLANT			0x200
#define	LOCATE_IS_TOWN				0x300
#define	LOCATE_IS_FIRM				0x400
#define  LOCATE_IS_WALL				0x500
#define	LOCATE_IS_ROCK				0x600
// ##### begin Gilbert 5/11 ######//
//#define  LOCATE_IS_UNIT			0x700
// ##### end Gilbert 5/11 ######//

#define	LOCATE_POWER_OFF			0x1000 // true if no power_nation_recno can be set in this location
#define	LOCATE_HARBOR_BIT       0x2000 // true if the terrain is suitable to build harbor (x,y) to (x+2,y+2)
#define	LOCATE_SEA_COAST			0x4000 // true if any corner has below sea altitude

//------ define constant for the logical dimension of Location -------//
#define	LOCATE_WIDTH            32
#define  LOCATE_HEIGHT           32

#define	LOCATE_WIDTH_SHIFT      5
#if( 1 << LOCATE_WIDTH_SHIFT != LOCATE_WIDTH )
	#error
#endif
#define	LOCATE_WIDTH_MASK			(LOCATE_WIDTH-1)
#define	LOCATE_HEIGHT_SHIFT		5
#if( 1 << LOCATE_WIDTH_SHIFT != LOCATE_HEIGHT )
	#error
#endif
#define	LOCATE_HEIGHT_MASK		(LOCATE_HEIGHT-1)

#define	C_MULTIPLIER				0x10000
#define	C_MULTIPLIER_SHIFT		16

#define RHOMBUS_LOCATION 1
#define LOC_XY_MUL_SHIFT 5

// loc_x_width, loc_x_height, loc_y_width, loc_y_height are multiplied by 32 times

// ------- constant on visibility ----------//
// const unsigned char FULL_VISIBILITY = MAX_BRIGHTNESS_ADJUST_DEGREE * 4 + 3;
#define FULL_VISIBILITY 131

// if a location has not been explored, visibility = 0
// if a location has been explored, visibility is bewtween EXPLORED_VISIBILITY-FULL_VISIBILITY
// don't set EXPLORED_VISIBILITY to multiple of 8
#define EXPLORED_VISIBILITY 43
#define MAX_VISIT_LEVEL FULL_VISIBILITY


//------- Define structure Location -------//

#pragma pack(1)
struct Location
{
public:
	unsigned short loc_flag;
	short				terrain_id;
	short				road_terrain_id;

	short         cargo_recno;
	short         air_cargo_recno;

	unsigned char extra_para;

	//------------------------------------------------//
	// when (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAS_SITE
	// > extra_para = raw recno
	//
	// when (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAD_WALL
	// > extra_para = time remained that can't build wall
	//
	// when (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAS_DIRT
	// > extra_para = dirt recno
	//
	// when (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_HILL
	// > cargo_recno = top hill block id
	// > extra_para = bottom hill block id
	//
	// when (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_FIRM
	// > cargo_recno = firm recno
	//
	// when (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_TOWN
	// > cargo_recno = town zone recno
	//
	//	when (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_PLANT
	// > extra_para  = id. of the plant bitmap
	// > cargo_recno = low byte - inner x, high byte - inner y
	//
	//	when (loc_flag & LOCATE_BLOCK_MASK) == LOCATION_IS_WALL
	// > extra_para  = id. of the city wall bitmap
	// > high byte of cargo_recno = hit points remained for the wall
	//
	//	when (loc_flag & LOCATE_BLOCK_MASK) == LOCATION_IS_ROCK
	// > cargo_recno = rock recno in rock_array
	//
	// when (loc_flag & LOCATE_BLOCK_MASK) == 0 and cargo_recno != 0
	// > carge_recno = unit id
	//------------------------------------------------//

	char		fire_level;					// -100 to 100, current fire level
	char		flammability;				// -100 to 100, likelihood of fire

	char		power_nation_recno;		// 0-no nation has power over this location
	unsigned char region_id;
	unsigned char visit_level;			// drop from FULL_VISIBILITY to 0

	// terrain height polynomial coefficient
	long		c00, c10, c01, c20, c11, c02;		// scaled up by C_MULTIPLIER or shift left by C_MULTIPLIER_SHIFT
	long		c30, c21, c12, c03;

	short		min_y, max_y;							// used in LocationCorners::render

public:
	//------ functions that check the type of the location ------//

	int   walkable()	      { return loc_flag & LOCATE_WALK_LAND; }
	int   sailable()			{ return loc_flag & LOCATE_WALK_SEA; }
	int   walkable(int teraMask)
									{ return loc_flag & teraMask; }
	void	walkable_reset();
	void	walkable_off()		{ loc_flag &= ~(LOCATE_WALK_LAND | LOCATE_WALK_SEA); }

	void	walkable_on(int teraMask)		{ loc_flag |= teraMask; }
	void	walkable_off(int teraMask)		{ loc_flag &= ~teraMask; }

	int	is_coast()			{ return loc_flag & LOCATE_COAST; }
	int	is_road()			{ return road_terrain_id; }

	// ####### begin Gilbert 8/2 ########//
	int	explored()        { return visit_level > 0; }
	void	explored_on()		{ if( visit_level < EXPLORED_VISIBILITY) visit_level = EXPLORED_VISIBILITY; }
	void	explored_off()		{ visit_level = 0; }

	// ---------- visibility --------//
	unsigned char visibility()				{ return visit_level; }
	void	dec_visibility()					{ if( visit_level > EXPLORED_VISIBILITY) --visit_level; }
	void	set_visited()						{ visit_level = MAX_VISIT_LEVEL; }
	void	set_visited(unsigned char v)	{ if( visit_level < v) visit_level = v; }
	// ####### end Gilbert 8/2 ########//
 
	int	is_plateau()                  { return 0; }


	// ----------- site -------------//
	int	can_build_site(int teraMask=LOCATE_WALK_LAND)
			{ return (loc_flag & teraMask) && !(loc_flag & LOCATE_SITE_MASK) && !has_site(); }
	void	set_site(int siteRecno);
	int	has_site()			{ return (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAS_SITE; }
	int	site_recno()		{ if( has_site() )  return extra_para; else return 0; }
	void	remove_site();

	// ------------ wall timeout ----------//
	int	had_wall()			{ return (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAD_WALL; }
	// after wall destructed, cannot build wall again for some time
	// the decrease time
	// (LOCATE_HAD_WALL)
	void	set_wall_timeout(int initTimeout);
	int	wall_timeout()					{ return extra_para; }
	int	dec_wall_timeout(int=1);
	void	remove_wall_timeout();

	// ----------- dirt -------------//
	int	can_add_dirt()
			{ return !(loc_flag & LOCATE_SITE_MASK); }
	void	set_dirt(int dirtRecno);
	int	has_dirt()			{ return (loc_flag & LOCATE_SITE_MASK) == LOCATE_HAS_DIRT; }
	int	dirt_recno()		{ if( has_dirt() )  return extra_para; else return 0; }
	void	remove_dirt();

	// ---------- firm ----------//
	int   is_firm()         { return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_FIRM; }
	int   can_build_firm(int teraMask = LOCATE_WALK_LAND)
			{ return !cargo_recno && (loc_flag & teraMask) && !(loc_flag & LOCATE_BLOCK_MASK) && !is_power_off(); }
	int	can_build_harbor(int teraMask = LOCATE_WALK_LAND)
			{ return !cargo_recno && (loc_flag & teraMask) && !(loc_flag & LOCATE_BLOCK_MASK); }
	void	set_firm(int firmRecno);
	int 	firm_recno()		{ if( is_firm() )   	  return cargo_recno; else return 0; }
	void	remove_firm();

	// ---------- town ------------//
	int   is_town()    	 	{ return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_TOWN; }
	void	set_town(int townRecno);
	int   can_build_town()
			{ return !cargo_recno && (loc_flag & LOCATE_WALK_LAND) && !(loc_flag & LOCATE_BLOCK_MASK) && !has_site() && !is_power_off(); }
	int	town_recno()		{ if( is_town() ) return cargo_recno; else return 0; }
	void	remove_town();

	// ---------- hill -------------//
	int	has_hill()			{ return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_HILL; }
	int	can_add_hill()		// exception : it has already a hill
			{ return has_hill() || // (loc_flag & LOCATE_WALK_LAND) &&
			!cargo_recno && !(loc_flag & (LOCATE_BLOCK_MASK | LOCATE_SITE_MASK)); }
	void	set_hill(int hillId);
	int   hill_id1()			{ return cargo_recno; }
	int   hill_id2()			{ return extra_para; }
	void	remove_hill();

	// ---------- wall ------------//
	int	is_wall()			{ return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_WALL; }
	int	can_build_wall()
			{ return !cargo_recno && (loc_flag & LOCATE_WALK_LAND) &&
			!(loc_flag & (LOCATE_BLOCK_MASK | LOCATE_SITE_MASK)) && !has_site(); }
	void	set_wall(int wallId, int townRecno, int hitPoints);
	void	set_wall_creating();
	void	set_wall_destructing();
	void	chg_wall_id( int wallId)		{ extra_para = wallId; }
	int	wall_id()			{ if( is_wall() ) return extra_para; else return 0; }
	int	wall_nation_recno() { return power_nation_recno; }
	int	wall_hit_point()	{ return cargo_recno >> 8; }
	int	wall_town_recno()	{ return cargo_recno || 0xFF; }
	//---------------------------------------------------//
	// initial 0, 1 to 100:creating, -1 to -100: destructing
	// except 0 or 100, hit point slowly increase by 1
	//---------------------------------------------------//
	int	wall_abs_hit_point()			{ return wall_hit_point() >= 0? wall_hit_point() : -wall_hit_point(); }
	int	inc_wall_hit_point(int grow=1);
	int	attack_wall(int damage=1);
	int	wall_grade()					{ return wall_hit_point() >= 0 ? (wall_hit_point()+24) / 25 : (wall_hit_point()-24)/25;}
	int	is_wall_creating()			{ return wall_hit_point() > 0; }
	int	is_wall_destructing()		{ return wall_hit_point() < 0; }
	void	remove_wall(int setTimeOut=-1);

	// ---------- plant -----------//

	int 	is_plant()						{ return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_PLANT; }
	int 	can_add_plant(int teraMask = LOCATE_WALK_LAND)
			{ return !cargo_recno && (loc_flag & teraMask) && !(loc_flag & (LOCATE_BLOCK_MASK | LOCATE_SITE_MASK)) && !has_site(); }
	void	set_plant(int plantId, int offsetX, int offsetY);
	int	plant_id()						{ if( is_plant() ) return extra_para; else return 0; }
	int	plant_inner_x() 				{ return cargo_recno & 0xFF; }
	int	plant_inner_y() 				{ return cargo_recno >> 8;   }
	void	grow_plant()					{ extra_para++; }
	void	remove_plant();

	// ---------- rock ------------//

	int	is_rock()						{ return (loc_flag & LOCATE_BLOCK_MASK) == LOCATE_IS_ROCK; }
	int	can_add_rock(int teraMask = LOCATE_WALK_LAND)
			{ return !cargo_recno && (loc_flag & teraMask) && !(loc_flag & LOCATE_BLOCK_MASK); }
	void	set_rock(short rockArrayRecno);
	short	rock_array_recno()				{ if( is_rock() ) return cargo_recno; else return 0; }
	void	remove_rock();

	// ##### begin Gilbert 5/11 #######//
	// --------- obstacle ----------//

	int	can_clear_obstacle();
	void	clear_obstacle();
	// ##### end Gilbert 5/11 #######//

	//--------- region ------------//

	// call region_type only when generating region number
	RegionType region_type()			{ return walkable()?REGION_LAND:(sailable()?REGION_SEA:REGION_INPASSABLE); }

	// ---------- BaseObj -----------//

	short	base_obj_recno();
	short base_obj_recno(char mobileType);

	// --------- functions on fire ---------//

	char	fire_str()						{ return fire_level; }
	char	fire_src()						{ return flammability; }
	void	set_fire_str(char str)		{ fire_level = str; }
	void	set_fire_src(char src)		{ flammability = src; }
	void	add_fire_str(char str)		{ fire_level += str; }
	void	add_fire_src(char src)		{ flammability += src; }
	int	can_set_fire()					{ return flammability >= -50; }

	//----- functions whose results affected by mobile_type -----//

	int   unit_recno(int mobileType);

	int	has_any_unit(int mobileType = UNIT_LAND);
	int 	get_any_unit(int& mobileType);
	int   is_accessible(int mobileType);      // whether the location is accessible to the unit of the specific mobile type

	void	set_unit(int unitRecno, int mobileType);
	void	remove_unit(int mobileType);

	int   can_move(int mobileType)      { return is_accessible(mobileType) && (mobileType==UNIT_AIR ? !air_cargo_recno : !cargo_recno); }

	//------------ power --------------//
	void	set_power_on();
	void	set_power_off();
	int	is_power_off();

	//----------- harbor bit -----------//
	void	set_harbor_bit()         { loc_flag |= LOCATE_HARBOR_BIT; }
	void	clear_harbor_bit()       { loc_flag &= ~LOCATE_HARBOR_BIT; }
	int	can_build_whole_harbor() { return loc_flag & LOCATE_HARBOR_BIT; }

	//----------- evaluate z ----------//
	long	evaluate_z(short subX, short subY);
};
#pragma pack()


//------------ Define class Corner ------------//

// data on the corners of Location

struct Corner
{
public:
	short altitude;
	short sea_altitude;			// 0 for land
	short gradient_x;
	short gradient_y;

public:
	void	set_altitude(short h)      { altitude = h; }
	short get_altitude()             { return sea_altitude ? sea_altitude : altitude; }
	void	set_sea_altitude(short s)  { sea_altitude = (s>=altitude?s:0); }
	short	get_sea_altitude()         { return sea_altitude; }
	int	is_sea()							{ return sea_altitude; }

	void	set_gradient_x(float dzdx) { gradient_x = short(dzdx*LOCATE_WIDTH); }
	float	get_gradient_x()           { return (float)gradient_x / LOCATE_WIDTH; }
	void	set_gradient_y(float dzdy) { gradient_y = short(dzdy*LOCATE_HEIGHT); }
	float	get_gradient_y()           { return (float)gradient_y / LOCATE_HEIGHT; }
};

//------------ Define struct LocationCorners ---------//

class Blob2DW;
class BitmapW;

struct LocationCorners
{
	Location *loc_ptr;
	Corner *top_left, *top_right, *bottom_left, *bottom_right;

	int	interpolate_z(int curX, int curY);
	void	render(Blob2DW *, char *texturePtr, BYTE drawState );
	void	render(BitmapW *, char *texturePtr, int x, int y, BYTE drawState);
	void	render_special(Blob2DW *, int effectId, char *texturePtr, int color1, int color2);
	void	render_special(BitmapW *, int effectId, char *texturePtr, int color1, int color2, int x, int y);
	void	render_fog(BitmapW *, char *texturePtr, int x, int y, char *fogMask);
};

//-------- Begin of function Location::is_accessible --------//

inline int Location::is_accessible(int mobileType)
{
	switch(mobileType)
	{
		case UNIT_LAND:
			return walkable();
			break;
/*
		case UNIT_SEA:
			return sailable();
			break;
*/
		case UNIT_AIR:
			return 1;
			break;
	}

	return 0;
}
//-------- End of function Location::is_accessible --------//

#endif
