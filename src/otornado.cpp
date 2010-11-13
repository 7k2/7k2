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

//Filename    : OTORNADO.CPP
//Description : Object Tornado
//Ownership   : Gilbert

#include <OTORNADO.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OUNIT.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OWEATHER.H>
#include <OFIRM.H>
#include <OFIRMA.H>
#include <OSERES.H>
#include <OTOWN.H>
#include <OCONFIG.H>
#include <ONEWS.H>
#include <math.h>


#define M_PI 3.141592654
#define DAMAGE_POINT_RADIUS 32

// #define TORNADO_SPRITE_ID  12          // Tornado sprite in SPRITE.DBF
#define TORNADO_SPRITE_NAME "TORNADO"

//--------- Begin of function Tornado::init ---------//
//
// <short> startX, startY       starting location of the tornado
// <short> lifeTime             time (no. of frames) the tornado survive
//
void Tornado::init(short startX, short startY, short lifeTime)
{
	static int spriteId = sprite_res.search_sprite( TORNADO_SPRITE_NAME );
	err_when( !spriteId );

	Sprite::init(spriteId, startX, startY);
	// err_when( strcmp("TORNADO", sprite_info->sprite_code ) );

	attack_damage  = (float) 2.0 / ATTACK_SLOW_DOWN;
	life_time = lifeTime;

	set_dir(DIR_N);
	cur_action = SPRITE_MOVE;		// always moving
	dmg_offset_x = 0;
	dmg_offset_y = 0;

	damage_player_flag = 0;
}
//----------- End of function Tornado::init -----------//

//----------- Begin of function Tornado::pre_process ----------//
void Tornado::pre_process()
{
	// ##### patch begin Gilbert 16/2 ######//
//	double angle = m.random(32) / 16.0 * M_PI;
//	dmg_offset_x = short(DAMAGE_POINT_RADIUS * sin(angle));
//	dmg_offset_y = short(DAMAGE_POINT_RADIUS * cos(angle));
	dmg_offset_x = m.random(LOCATE_WIDTH*3) - LOCATE_WIDTH;
	dmg_offset_y = m.random(LOCATE_HEIGHT*3) - LOCATE_HEIGHT;
	// ##### patch end Gilbert 16/2 ######//
	if( --life_time <= 0)
		cur_action = SPRITE_DIE;
}
//----------- End of function Tornado::pre_process ----------//

//--------- Begin of function Tornado::process_move --------//

void Tornado::process_move()
{
	int speed = weather.wind_speed() / 6;
	int minSpeed = magic_weather.wind_day > 0 ? 1 : 5;
	if( speed < minSpeed)
		speed = minSpeed;
	if( speed > 10)
		speed = 10;

	double windDir = weather.wind_direct_rad() + (m.random(31)-15)*M_PI/180.0;
	cur_x += short(speed * sin(windDir));
	cur_y -= short(speed * cos(windDir));
	if( ++cur_frame > cur_sprite_move()->frame_count )
		cur_frame = 1;
	// static UCHAR nextFrame[] = { 1,6,1,1,1,1,4 };		// 1->6->4->1 ...
	// cur_frame = nextFrame[cur_frame];

	hit_target();
}
//---------- End of function Tornado::process_move ----------//


//--------- Begin of function Tornado::hit_target --------//

void Tornado::hit_target()
{
	//---- check if there is any unit in the target location ----//

	short damageXLoc = damage_x_loc();
	short damageYLoc = damage_y_loc();
	if( damageXLoc < 0 || damageXLoc >= world.max_x_loc ||
		damageYLoc < 0 || damageYLoc >= world.max_y_loc)
		return;

	Location *locPtr = world.get_loc(damageXLoc, damageYLoc);

	Unit *targetUnit;
	if( locPtr->unit_recno(UNIT_AIR))
	{
		targetUnit = unit_array[locPtr->unit_recno(UNIT_AIR)];
		// ###### patch begin Gilbert 17/2 #######//
		// targetUnit->hit_points -= 2*attack_damage;
		targetUnit->hit_points -= 16*attack_damage;
		// ###### patch end Gilbert 17/2 #######//
		if( targetUnit->hit_points <= 0)
			targetUnit->hit_points = (float) 0;

		if( !damage_player_flag && targetUnit->is_own() && !targetUnit->cast_to_UnitGod() )
		{
			// do not add news when hitting air unit (god)
			damage_player_flag = 1;
			// add news message
			news_array.tornado_hit(sprite_recno);
		}
	}

	if( locPtr->unit_recno(UNIT_LAND))
	{
		targetUnit = unit_array[locPtr->unit_recno(UNIT_LAND)];
		// ###### patch begin Gilbert 17/2 #######//
		// targetUnit->hit_points -= attack_damage;
		targetUnit->hit_points -= 8*attack_damage;
		// ###### patch end Gilbert 17/2 #######//
		if( targetUnit->hit_points <= 0)
			targetUnit->hit_points = (float) 0;

		if( !damage_player_flag && targetUnit->is_own() )
		{
			damage_player_flag = 1;
			// add news message
			news_array.tornado_hit(sprite_recno);
		}
	}
	else if( locPtr->unit_recno(UNIT_SEA))
	{
		targetUnit = unit_array[locPtr->unit_recno(UNIT_SEA)];
		// ###### patch begin Gilbert 17/2 #######//
		// targetUnit->hit_points -= attack_damage;
		targetUnit->hit_points -= 8*attack_damage;
		// ###### patch end Gilbert 17/2 #######//
		if( targetUnit->hit_points <= 0)
			targetUnit->hit_points = (float) 0;

		if( !damage_player_flag && targetUnit->is_own() )
		{
			damage_player_flag = 1;
			// add news message
			news_array.tornado_hit(sprite_recno);
		}
	}
	else
	{
		hit_building();	// pass to hit_building to check whether a building is in the location
	}

	hit_plant();
	hit_fire();
}
//---------- End of function Tornado::hit_target ----------//


//------- Begin of function Tornado::hit_building -----//
//	building means firm or town
//
void Tornado::hit_building()
{
	short damageXLoc = damage_x_loc();
	short damageYLoc = damage_y_loc();
	if( damageXLoc < 0 || damageXLoc >= world.max_x_loc ||
		damageYLoc < 0 || damageYLoc >= world.max_y_loc)
		return;

	Location* locPtr = world.get_loc(damageXLoc, damageYLoc);

	if(locPtr->is_firm() )
//		&& firm_res[(firmPtr=firm_array[locPtr->firm_recno()])->firm_id]->buildable )
	{
		Firm *firmPtr = firm_array[locPtr->firm_recno()];
		firmPtr->hit_points -= attack_damage*2;

		if( !damage_player_flag && firmPtr->is_own() )
		{
			damage_player_flag = 1;
			// add news message
			news_array.tornado_hit(sprite_recno);
		}

		if( firmPtr->hit_points <= 0)
		{
			firmPtr->hit_points = (float) 0;

			se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
				'F', firmPtr->firm_id, "DIE" );

			firm_array.del_firm(locPtr->firm_recno());
		}
	}

	if( locPtr->is_town() )
	{
		Town *townPtr = town_array[locPtr->town_recno()];

		if( !damage_player_flag && townPtr->is_own() )
		{
			damage_player_flag = 1;
			// add news message
			news_array.tornado_hit(sprite_recno);
		}

		if( townPtr->hit_points > 0.0f )
		{
			townPtr->hit_points -= attack_damage*2;
			if( townPtr->hit_points <= 0 )
			{
				townPtr->hit_points = (float) 0;
			}
		}
		else
		{
			// ###### begin Gilbert 10/2 #######//
			if( (life_time % 10) == 0 ) 
				townPtr->kill_town_people(0);
			// ###### end Gilbert 10/2 #######//
		}
	}
}
//---------- End of function Tornado::hit_building ----------//


//---------- Begin of function Tornado::hit_plant --------//
void Tornado::hit_plant()
{
	short damageXLoc = damage_x_loc();
	short damageYLoc = damage_y_loc();
	if( damageXLoc < 0 || damageXLoc >= world.max_x_loc ||
		damageYLoc < 0 || damageYLoc >= world.max_y_loc)
		return;

	Location *locPtr = world.get_loc(damageXLoc, damageYLoc);
	if(locPtr->is_plant())
	{
		locPtr->remove_plant();
		// ###### begin Gilbert 24/6 ########//
		world.plant_count--;
		// ###### end Gilbert 24/6 ########//
	}
}
//---------- End of function Tornado::hit_plant --------//


//---------- Begin of function Tornado::hit_fire --------//
void Tornado::hit_fire()
{
	short damageXLoc = damage_x_loc();
	short damageYLoc = damage_y_loc();
	if( damageXLoc < 0 || damageXLoc >= world.max_x_loc ||
		damageYLoc < 0 || damageYLoc >= world.max_y_loc)
		return;

	Location *locPtr = world.get_loc(damageXLoc, damageYLoc);
	if(locPtr->fire_str() > 0)
	{
		locPtr->set_fire_str(1);
	}
}
//---------- End of function Tornado::hit_plant --------//


// ---------- Begin of function Sprite::is_stealth --------//
int Tornado::is_stealth()
{
	err_when( cur_x_loc() < 0 || cur_x_loc() >= MAX_WORLD_X_LOC );
	err_when( cur_y_loc() < 0 || cur_y_loc() >= MAX_WORLD_Y_LOC );
	// if the visibility of location is just explored, consider stealth
	// ######## begin Gilbert 19/2 ########//
	return config.blacken_map && !config.explore_whole_map 
		&& !world.get_loc(cur_x_loc(), cur_y_loc())->explored();
	// ######## end Gilbert 19/2 ########//
}
// ---------- End of function Sprite::is_stealth --------//



#ifdef DEBUG

//------- Begin of function TornadoArray::operator[] -----//

Tornado* TornadoArray::operator[](int recNo)
{
	Tornado* tornadoPtr = (Tornado*) get_ptr(recNo);

	if( !tornadoPtr )
		err.run( "TornadoArray[] is deleted" );

	return tornadoPtr;
}

//--------- End of function TornadoArray::operator[] ----//

#endif

//--------- Begin of function TornadoArray::TornadoArray ------//
TornadoArray::TornadoArray(int initArraySize): SpriteArray(initArraySize)
{
}
//--------- End of function TornadoArray::TornadoArray ------//


//--------- Begin of function TornadoArray::add_tornado ------//
//
// <short> startX, startY       starting location of the tornado
// <short> lifeTime             time (no. of frames) the tornado survive
//
short TornadoArray::add_tornado(int xLoc, int yLoc, short lifeTime)
{
	Tornado *tornadoPtr = new Tornado;
	tornadoPtr->init(xLoc, yLoc, lifeTime);
	add(tornadoPtr);
	return 1;
}
//--------- End of function TornadoArray::add_tornado ------//


//--------- Begin of function TornadoArray::create_tornado ------//
//
// return: <int> recno of the tornado created. 
//
short TornadoArray::create_tornado()
{
	Tornado *tornadoPtr = new Tornado;
	add(tornadoPtr);

	return recno();
}
//--------- End of function TornadoArray::create_tornado ------//


//--------- Begin of function TornadoArray::process ---------//

void TornadoArray::process()
{
	int 	  i;
	Tornado* tornadoPtr;

	for (i=1; i <=size() ; i++)
	{
		tornadoPtr = (Tornado*)get_ptr(i);

		if( !tornadoPtr)
			continue;

		//-------- system yield ---------//

		if( i%20==1 )
			sys.yield();

		//------- process tornado --------//

		tornadoPtr->pre_process();

		switch(tornadoPtr->cur_action)
		{
			case SPRITE_IDLE:
			case SPRITE_READY_TO_MOVE:
				tornadoPtr->process_idle();
				break;

			case SPRITE_MOVE:
				tornadoPtr->process_move();
				break;

			//### begin alex 7/3 ###//
			case SPRITE_TURN:
				err_here();
				break;
			//#### end alex 7/3 ####//

			case SPRITE_WAIT:
				tornadoPtr->process_wait();
				break;

			case SPRITE_ATTACK:
				tornadoPtr->process_attack();
				break;

			case SPRITE_DIE:
				if( tornadoPtr->process_die() )
					del(i);
				break;
		}
	}
}
//----------- End of function TornadoArray::process -----------//


//--------- Begin of function TornadoArray::draw_dot ---------//
//
// Draw 2x2 tiny squares on map window representing the location of the unit ---//
//
void TornadoArray::draw_dot()
{
	short*	writePtr;
	Tornado*	tornadoPtr;
	int		i, mapX, mapY;
	int		vgaBufPitch = vga_back.buf_pitch();

	short		unexploredColor = vga_back.translate_color(UNEXPLORED_COLOR);
	short		tornadoColor[2];
	tornadoColor[0] = vga_back.translate_color(TORNADO_COLOR1);
	tornadoColor[1] = vga_back.translate_color(TORNADO_COLOR2);

	for(i=1; i <=size() ; i++)
	{
		tornadoPtr = (Tornado*)get_ptr(i);

		if( !tornadoPtr )
			continue;

		//  ###### begin Gilbert 19/1 #############//
		// ------ draw at least 1 loc inside boundary -------//

		if( tornadoPtr->cur_x_loc() <= 0 || tornadoPtr->cur_x_loc()+1 >= MAX_WORLD_X_LOC
			|| tornadoPtr->cur_y_loc() <= 0 || tornadoPtr->cur_y_loc()+1 >= MAX_WORLD_Y_LOC )
			continue;
		//  ###### end Gilbert 19/1 #############//

		if( tornadoPtr->is_stealth() )		// check inside map first
			continue;

		mapX = world.map_matrix->calc_loc_pos_x(tornadoPtr->cur_x_loc(), tornadoPtr->cur_y_loc()) + MAP_X1;
		mapY = world.map_matrix->calc_loc_pos_y(tornadoPtr->cur_x_loc(), tornadoPtr->cur_y_loc()) + MAP_Y1;

		if( mapX < MAP_X1 || mapX > MAP_X2 || mapY < MAP_Y1 || mapY > MAP_Y2 )
			return;

		writePtr = vga_back.buf_ptr(mapX, mapY);

		if( mapY > MAP_Y1)
		{
			if( mapX > MAP_X1 && writePtr[-vgaBufPitch-1] != unexploredColor)
				writePtr[-vgaBufPitch-1] = tornadoColor[1];
			if( writePtr[-vgaBufPitch] != unexploredColor)
				writePtr[-vgaBufPitch] = tornadoColor[0];
			if( mapX < MAP_X2 && writePtr[-vgaBufPitch+1] != unexploredColor)
				writePtr[-vgaBufPitch+1] = tornadoColor[1];
		}

		if( mapX > MAP_X1 && writePtr[-1] != unexploredColor)
			writePtr[-1] = tornadoColor[0];
		if( mapX < MAP_X2 && writePtr[1] != unexploredColor)
			writePtr[1] = tornadoColor[0];

		if( mapY < MAP_Y2)
		{
			if( mapX > MAP_X1 && writePtr[vgaBufPitch-1] != unexploredColor)
				writePtr[vgaBufPitch-1] = tornadoColor[1];
			if( writePtr[vgaBufPitch] != unexploredColor)
				writePtr[vgaBufPitch] = tornadoColor[0];
			if( mapX < MAP_X2 && writePtr[vgaBufPitch+1] != unexploredColor)
				writePtr[vgaBufPitch+1] = tornadoColor[1];
		}

	}
}
//----------- End of function TornadoArray::draw_dot -----------//

