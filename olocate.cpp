//Filename   : OLOCATE.CPP
//Description: Functions in Location class

#include <ALL.H>
#include <OHILLRES.H>
#include <OWALLRES.H>
#include <OFIRM.H>
#include <OTOWN.H>
#include <OUNIT.H> 
#include <OMATRIX.H>
#include <OROCK.H>
#include <OROCKRES.H>
#include <OWORLD.H>

// --------- define constant ----------//

#define DEFAULT_WALL_TIMEOUT 10
#define WALL_DEFENCE 5
#define MIN_WALL_DAMAGE 3

// ------- Begin of function Location::walkable_reset -----/
void Location::walkable_reset()
{
	if( loc_flag & LOCATE_BLOCK_MASK )
	{
		walkable_off();
	}
	else
	{
//		if( terrain_res[terrain_id]->average_type == TERRAIN_OCEAN)
//			loc_flag |= LOCATE_WALK_SEA;
//		else
			loc_flag |= LOCATE_WALK_LAND;
	}
}
// ----------- End of function Location::walkable_reset -------//


// ----------- Begin of function Location::is_plateau ---------//
//int Location::is_plateau()
//{
	// return terrain_res[terrain_id]->average_type == TERRAIN_DARK_DIRT;		//**BUGHERE, to be changed to TERRAIN_HILL when the no. of terrain type has been reduced to 4 from 7
//	return 0;
//}
// ----------- End of function Location::is_plateau ---------//


//---------- Begin of function Location::set_site ------------//
//
void Location::set_site(int siteRecno)
{
	err_when( !can_build_site() );

	loc_flag = loc_flag & ~LOCATE_SITE_MASK | LOCATE_HAS_SITE;
	// loc_flag |= LOCATION_HAS_SITE;

	extra_para = siteRecno;
}
//------------ End of function Location::set_site ------------//


//---------- Begin of function Location::remove_site ------------//
//
void Location::remove_site()
{
	err_when( !has_site() );

	loc_flag &= ~LOCATE_SITE_MASK;

	extra_para  = 0;
}
//------------ End of function Location::remove_site ------------//


//------------ Begin of function Location::set_wall_timeout ------------//
void Location::set_wall_timeout(int initTimeout)
{
	err_when((!can_build_site() && !had_wall()) || initTimeout <= 0);
	loc_flag = (loc_flag & ~LOCATE_SITE_MASK) | LOCATE_HAD_WALL;
	extra_para = (unsigned char) initTimeout;
}
//------------ End of function Location::set_wall_timeout ------------//


//------------ Begin of function Location::dec_wall_timeout ------------//
// return : true if the timeout drop to zero, and is removed
int Location::dec_wall_timeout(int t)
{
	err_when( !had_wall() );
	if( (extra_para -= t) <= 0)
	{
		remove_wall_timeout();
		return 1;
	}
	return 0;
}
//------------ End of function Location::dec_wall_timeout ------------//


//------------ Begin of function Location::remove_wall_timeout ------------//
void Location::remove_wall_timeout()
{
	err_when( !had_wall() );
	loc_flag &= ~LOCATE_SITE_MASK;
	extra_para = 0;
}
//------------ End of function Location::remove_wall_timeout ------------//


//---------- Begin of function Location::set_dirt ------------//
//
void Location::set_dirt(int dirtRecno)
{
	err_when( !can_add_dirt() );

	loc_flag = loc_flag & ~LOCATE_SITE_MASK | LOCATE_HAS_DIRT;

	extra_para = dirtRecno;
}
//------------ End of function Location::set_dirt ------------//


//---------- Begin of function Location::remove_dirt ------------//
//
void Location::remove_dirt()
{
	err_when( !has_dirt() );

	loc_flag &= ~LOCATE_SITE_MASK;

	extra_para  = 0;

	walkable_reset();		// some rock are block, so reset walkable
}
//------------ End of function Location::remove_dirt ------------//


//---------- Begin of function Location::set_firm ------------//
//
void Location::set_firm(int firmRecno)
{
	// can't check the terrain type here
	err_when( !can_build_firm() && !firmRecno );

	walkable_off();
	loc_flag = (loc_flag & ~LOCATE_BLOCK_MASK) | LOCATE_IS_FIRM;

	cargo_recno = firmRecno;
}
//------------ End of function Location::set_firm ------------//


//---------- Begin of function Location::remove_firm ------------//
//
void Location::remove_firm()
{
	err_when( !is_firm() );

	loc_flag &= ~LOCATE_BLOCK_MASK;
	cargo_recno = 0;
	walkable_reset();

	err_when(is_firm());
}
//------------ End of function Location::remove_firm ------------//


//---------- Begin of function Location::set_town ------------//
//
void Location::set_town(int townRecno)
{
	err_when( !can_build_town() || !townRecno );

	walkable_off();
	loc_flag = loc_flag & ~LOCATE_BLOCK_MASK | LOCATE_IS_TOWN;

	cargo_recno = townRecno;
}
//------------ End of function Location::set_town ------------//


//---------- Begin of function Location::remove_town ------------//
//
void Location::remove_town()
{
	err_when( !is_town() );

	loc_flag &= ~LOCATE_BLOCK_MASK;
	cargo_recno = 0;
	walkable_reset();

	err_when(is_firm());
}
//------------ End of function Location::remove_town ------------//


//---------- Begin of function Location::set_hill ------------//
// set hillId to hill_id1 (cargo_recno) or hill_id2 (extra_para)
// depend on the priority of the hill block
void Location::set_hill(int hillId)
{
	err_when( !can_add_hill() || !hillId );
	err_when( !hill_res[hillId] );

	// clear LOCATE_WALK_LAND and LOCATE_WALK_SEA bits
	walkable_off();

	if( has_hill() )
	{
		// already has a hill block
		// compare which is on the top, swap if necessary
		if(hill_res[cargo_recno]->priority <= hill_res[hillId]->priority)
		{
			err_when(cargo_recno >= 256);
			extra_para = (unsigned char) cargo_recno;
			cargo_recno = hillId;
		}
		else
		{
			// if two hill blocks there, the lower one get replaced
			err_when( hillId >= 256);
			extra_para = (unsigned char) hillId;
		}
	}
	else
	{
		// no existing hill block
		loc_flag = loc_flag & ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK )
			| (LOCATE_IS_HILL | LOCATE_SITE_RESERVED);
		cargo_recno = hillId;
		extra_para = 0;
	}
}
//------------ End of function Location::set_hill ------------//


//---------- Begin of function Location::remove_hill ------------//
void Location::remove_hill()
{
	err_when( !has_hill() );

	loc_flag &= ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK);

	extra_para  = 0;
	cargo_recno = 0;
	// err_when(is_firm());
	// BUGHERE : need to call walkable_reset();
}
//------------ End of function Location::remove_hill ------------//


//---------- Begin of function Location::set_wall ------------//
//
// <int> wallId	 = the id. of the wall
// <int> townRecno = recno of the town which the wall belongs to
// <int> hitPoints = hit points remained for the wall
//
void Location::set_wall(int wallId, int townRecno, int hitPoints)
{
	err_when( !can_build_wall() || !wallId );

	walkable_off();
	loc_flag = loc_flag & ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK )
		| (LOCATE_IS_WALL | LOCATE_SITE_RESERVED);

	extra_para  = wallId;
	cargo_recno = (hitPoints<<8) + townRecno;
}
//------------ End of function Location::set_wall ------------//


//------------ Begin of function Location::set_wall_creating ------------//
void Location::set_wall_creating()
{
	err_when( !is_wall() );
	int hp = wall_hit_point();
	if( hp < 0)
		hp = -hp;
	cargo_recno = (hp << 8) | wall_town_recno();
}
//------------ End of function Location::set_wall_creating ------------//


//------------ Begin of function Location::set_wall_destructing------------//
void Location::set_wall_destructing()
{
	err_when( !is_wall() );
	int hp = wall_hit_point();
	if( hp > 0)
		hp = -hp;
	cargo_recno = (hp << 8) | wall_town_recno();
}
//------------ End of function Location::set_wall_desctructing ------------//


//------------ Begin of function Location::inc_wall_hit_point ------------//
int Location::inc_wall_hit_point(int grow)
{
	err_when( !is_wall() );
	int hp = wall_hit_point();
	if( hp < 0 && hp > -grow)
	{
		hp = 0;
	}
	else if( hp > 100-grow)
	{
		hp = 100;
	}
	else
		hp += grow;
	cargo_recno = (hp << 8) | wall_town_recno();
	return hp;
}
//------------ End of function Location::inc_wall_hit_point ------------//


//------------ Begin of function Location::attack_wall ------------//
//
// attack wall
// int damage       damage to a wall
// note : if the return value is 0, call world.correct_wall to
//        correct the shape of the adjacent squares
//
int Location::attack_wall(int damage)
{
	err_when( !is_wall() );

	if(damage >= WALL_DEFENCE + MIN_WALL_DAMAGE)    // damage >= 8, damage -= 5
		damage -= WALL_DEFENCE;
	else if( damage >= MIN_WALL_DAMAGE )            // 3 <= damage < 8, damage = 3
		damage = MIN_WALL_DAMAGE;
	else if( damage <= 0)                           // 0 < damage < 3, no change to
		return wall_hit_point();                     // no change to hit point to damage

	int hp = wall_hit_point();
	if( hp > 0)
	{
		hp-= damage;
		if( hp <= 0)
		{
			hp = 0;
			remove_wall();
			return 0;
		}
	}
	else if( hp < 0)
	{
		hp+= damage;
		if( hp >= 0)
		{
			hp = 0;
			remove_wall();
			return 0;
		}
	}
	cargo_recno = (hp << 8) | wall_town_recno();
	return hp;
}
//------------ End of function Location::attack_wall ------------//


//---------- Begin of function Location::remove_wall ------------//
//
// <int> setTimeOut   call set_wall_timeout to refuse building wall at the same place
//                    0 to disable timeout, -1 to take default: 10
//
void Location::remove_wall(int setTimeOut)
{
	err_when( !is_wall() );

	loc_flag &= ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK);
	extra_para  = 0;
	cargo_recno = 0;
	walkable_reset();

	if( setTimeOut < 0)
		set_wall_timeout( DEFAULT_WALL_TIMEOUT );
	else if( setTimeOut > 0)
	{
		err_when( setTimeOut > 255 );
		set_wall_timeout( setTimeOut );
	}


	err_when(is_firm());
}
//------------ End of function Location::remove_wall ------------//


//---------- Begin of function Location::set_plant ------------//
//
void Location::set_plant(int plantId, int offsetX, int offsetY)
{
	err_when( !can_add_plant() || !plantId );

	walkable_off();
	loc_flag = loc_flag & ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK )
		| (LOCATE_IS_PLANT | LOCATE_SITE_RESERVED);

	extra_para  = plantId;
	cargo_recno = (offsetY<<8) + offsetX;
	err_when(cargo_recno==0 || is_firm());
}
//------------ End of function Location::set_plant ------------//


//---------- Begin of function Location::remove_plant ------------//
//
void Location::remove_plant()
{
	err_when( !is_plant() );

	loc_flag &= ~(LOCATE_BLOCK_MASK | LOCATE_SITE_MASK);
	extra_para  = 0;
	cargo_recno = 0;
	walkable_reset();

	err_when(is_firm());
}
//------------ End of function Location::remove_plant ------------//


//---------- Begin of function Location::set_rock ------------//
//
void Location::set_rock(short rockArrayRecno)
{
	err_when( !can_add_rock(3) || !rockArrayRecno );
	walkable_off();
	loc_flag = loc_flag & ~LOCATE_BLOCK_MASK | LOCATE_IS_ROCK;

	cargo_recno = rockArrayRecno;
}
//------------ End of function Location::set_rock ------------//


//---------- Begin of function Location::remove_rock ------------//
//
void Location::remove_rock()
{
	err_when( !is_rock() );

	loc_flag &= ~LOCATE_BLOCK_MASK;
	cargo_recno = 0;
	walkable_reset();
}
//------------ End of function Location::remove_rock ------------//


//-------- Begin of function Location::unit_recno --------//

int Location::unit_recno(int mobileType)
{
	switch(mobileType)
	{
		case UNIT_LAND:
			if( walkable() )
				return cargo_recno;
			break;

		case UNIT_SEA:
			if( sailable() )
				return cargo_recno;
			break;

		case UNIT_AIR:
			return air_cargo_recno;
	}

	return 0;
}
//-------- End of function Location::unit_recno --------//


//-------- Begin of function Location::has_any_unit --------//
//
// <int> mobileType - (default: UNIT_LAND)
//
// return the mobile_type if there is any unit here
// return 0 otherwise
//
int Location::has_any_unit(int mobileType)
{
	if(mobileType==UNIT_LAND)
	{
		if(air_cargo_recno)
			return UNIT_AIR;
		else if(walkable() && cargo_recno)
			return UNIT_LAND;
		else if(sailable() && cargo_recno)
			return UNIT_SEA;	
	}
	else
	{
		if(walkable() && cargo_recno)
			return UNIT_LAND;
		else if(sailable() && cargo_recno)
			return UNIT_SEA;	
		else if(air_cargo_recno)
			return UNIT_AIR;
	}

	return 0;
}
//-------- End of function Location::has_any_unit --------//


//-------- Begin of function Location::get_any_unit --------//
//
// <int&> mobileType - var for returning the mobile type of
//								unit in the location.
//
// return: <int> unit recno of the unit.
//
int Location::get_any_unit(int& mobileType)
{
	if(air_cargo_recno)
	{
		mobileType = UNIT_AIR;
		return air_cargo_recno;
	}
	else if(walkable() && cargo_recno)
	{
		mobileType = UNIT_LAND;
		return cargo_recno;
	}
	else if(sailable() && cargo_recno)
	{
		mobileType = UNIT_SEA;
		return cargo_recno;
	}

	return 0;
}
//-------- End of function Location::get_any_unit --------//


//-------- Begin of function Location::set_unit --------//

void Location::set_unit(int unitRecno, int mobileType)
{
	if( mobileType == UNIT_AIR )
	{
		err_when( air_cargo_recno && (air_cargo_recno != unitRecno));
		air_cargo_recno = unitRecno;
	}
	else
	{
		err_when( cargo_recno && (cargo_recno != unitRecno));
		cargo_recno = unitRecno;
	}
}
//-------- End of function Location::set_unit --------//


//-------- Begin of function Location::remove_unit --------//

void Location::remove_unit(int mobileType)
{
	if( mobileType == UNIT_AIR )
	{
		air_cargo_recno = 0;
	}
	else
	{
		err_when( loc_flag & LOCATE_BLOCK_MASK );		// if there is a still object on the location
		cargo_recno = 0;
	}
}
//-------- End of function Location::remove_unit --------//


//-------- Begin of function Location::set_power_on --------//
void Location::set_power_on()
{
	loc_flag &= ~LOCATE_POWER_OFF;
}
//-------- End of function Location::set_power_on --------//


//-------- Begin of function Location::set_power_off --------//
void Location::set_power_off()
{
	loc_flag |= LOCATE_POWER_OFF;
}
//-------- End of function Location::set_power_off --------//


//-------- Begin of function Location::is_power_off --------//
int Location::is_power_off()
{
	return (loc_flag & LOCATE_POWER_OFF);
}
//-------- End of function Location::is_power_off --------//


//------- Begin of function LocationCorners::interpolate_z ------//
int LocationCorners::interpolate_z(int curX, int curY)
{
	short xPixel, yPixel;	// 0 to LOCATE_WIDTH-1
	xPixel = curX % LOCATE_WIDTH;
	yPixel = curY % LOCATE_HEIGHT;

	return ((int) top_left->get_altitude() * (LOCATE_WIDTH - xPixel) * (LOCATE_HEIGHT - yPixel)
		+ (int) top_right->get_altitude() * xPixel * (LOCATE_HEIGHT - yPixel)
		+ (int) bottom_left->get_altitude() * (LOCATE_WIDTH - xPixel) * yPixel
		+ (int) bottom_right->get_altitude() * xPixel * yPixel )
		/ (LOCATE_WIDTH * LOCATE_HEIGHT);
}
//------- End of function LocationCorners::interpolate_z ------//


//---------- Begin of function Location::base_obj_recno ------------//
//
// Return the base_obj_recno if the object on this location uses the
// base class BaseObj. Return 0 if the object's base class is not baseObj.
//
short Location::base_obj_recno()
{
	int mobileType;
	int unitRecno = get_any_unit(mobileType);

	if( unitRecno )
		return unit_array[unitRecno]->base_obj_recno;

	if( is_firm() )
		return firm_array[firm_recno()]->base_obj_recno;

	if( is_town() )
		return town_array[town_recno()]->base_obj_recno;

	return 0;
}
//------------ End of function Location::base_obj_recno ------------//


//---------- Begin of function Location::base_obj_recno ------------//
//
// Return the base_obj_recno if the object on this location uses the
// base class BaseObj. Return 0 if the object's base class is not baseObj.
//
short Location::base_obj_recno(char mobileType)
{
	int unitRecno = unit_recno(mobileType);

	if( unitRecno )
		return unit_array[unitRecno]->base_obj_recno;

	if( mobileType == UNIT_LAND )
	{
		if( is_firm() )
			return firm_array[firm_recno()]->base_obj_recno;

		if( is_town() )
			return town_array[town_recno()]->base_obj_recno;
	}

	return 0;
}
//---------- End of function Location::base_obj_recno ------------//



// ###### begin Gilbert 5/11 #######//
//---------- Begin of function Location::can_clear_obstacle ------------//
//
// return if the obstacle can be removed to build firm
//
int Location::can_clear_obstacle()
{
	if( is_plant() )
	{
		return 1;
	}
	else if( is_rock() )
	{
		RockInfo *rockInfo = rock_array[rock_array_recno()]->rock_info();
		if( rockInfo->loc_width <= 2 && rockInfo->loc_height <= 2 )
			return 1;
	}

	if( has_dirt() )
	{
		RockInfo *rockInfo = dirt_array[dirt_recno()]->rock_info();
		if( rockInfo->loc_width <= 2 && rockInfo->loc_height <= 2 )
			return 1;
	}

	return 0;
}
//---------- End of function Location::can_clear_obstacle ------------//


//---------- Begin of function Location::clear_obstacle ------------//
//
// clear obstacle for building firm
//
void Location::clear_obstacle()
{
	if( is_plant() )
	{
		remove_plant();
		--world.plant_count;
	}
	else if( is_rock() )
	{
		world.remove_rock(rock_array_recno());
	}

	if( has_dirt() )
	{
		world.remove_dirt(dirt_recno());
	}
}
//---------- Begin of function Location::clear_obstacle ------------//
// ###### end Gilbert 5/11 #######//
