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

// Filename    : OB_PROG.CPP
// Description : BulletProgram, Programming bullet

#include <ob_prog.h>
#include <cstdlib>
#include <ovga.h>
#include <omagic.h>
#include <of_fort.h>
#include <osfrmres.h>
#include <oworldmt.h>
#include <oseres.h>
#include <oconfig.h>
#include <math.h>

// --------- begin of function BulletProgram::BulletProgram --------//
BulletProgram::BulletProgram()
{
}
// --------- end of function BulletProgram::BulletProgram --------//

// --------- begin of function BulletProgram::~BulletProgram --------//
BulletProgram::~BulletProgram()
{
	// load dummy bitmap resource for the 
	// destructor of SPRITE to free
//	sprite_info = sprite_res[1];
//	sprite_info->load_bitmap_res();
}
// --------- begin of function BulletProgram::~BulletProgram --------//

//--------- Begin of function BulletProgram::init ---------//
//
// <char> parentType		- the type of object emits the bullet
// <short> parentRecno	- the recno of the object
// <short> targetXLoc	- the x loc of the target
// <short> targetYLoc	- the y loc of the target
//	<char> targetMobileType - target mobile type
//
void BulletProgram::init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType)
{
	Bullet::init(parentType, parentRecno, targetXLoc, targetYLoc, targetMobileType);
	cur_step = 0;
	
	BaseObj *baseObjPtr = base_obj_array[parent_base_obj_recno];
	Unit *parentUnit;

	// for fixed total_step of bullet, set sprite.dbf's speed //
	// else total_step = distance to target / unitattk.dbf's bullet_speed //
	
	// start animation	bullet_running		die_animation
	// delay_step			total_step
	// 
	//	bullet_delay in	if sprite.dbf's	no. of frame filled
	// sprite.dbf &		speed > 0, take	in sframe.dbf
	// unitattk.dbf		it else use
	//							distance to 
	//							calculate

	if (sprite_info->speed > 0) 
		total_step = sprite_info->speed;

	if( (parentUnit = baseObjPtr->cast_to_Unit()) )
	{
		// delay obtained from unitattk.dbf //
		AttackInfo* attackInfo = parentUnit->attack_info_array+parentUnit->cur_attack;
		delay_step = attackInfo->bullet_delay;
	}
	else
	{
		// delay obtained from sprite.dbf if the attacker is not a unit //
		delay_step = sprite_info->bullet_delay;
	}
}

//----------- End of function BulletProgram::init -----------//

//--------- Begin of function BulletProgram::process_move --------//

void BulletProgram::process_move()
{
	cur_frame ++;
	//----- if the sprite has reach the destintion ----//

	if( ++cur_step > total_step + delay_step )
	{
		check_hit();
		cur_action = SPRITE_DIE;		// Explosion
		cur_frame = 0;
	}
	else if( total_step - cur_step == 1 )
	{
		warn_target();
	}
}
//---------- End of function BulletProgram::process_move ----------//

//--------- Begin of function BulletProgram::process_die --------//
//
// return : <int> 1 - dying animation completes.
//					   0 - still dying 
//
int BulletProgram::process_die()
{
	// ------- sound effect --------//
	se_res.sound(cur_x_loc(), cur_y_loc(), cur_frame, 'S',sprite_id,"DIE");

	if( ++cur_frame > sprite_info->die.frame_count )
	{

		if( fire_radius > 0)
		{
			Location *locPtr;
			if( fire_radius == 1)
			{
				locPtr = world.get_loc(target_x_loc, target_y_loc);
				if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
					locPtr->set_fire_str(30);
				if( locPtr->fire_src() > 0 )
					locPtr->set_fire_src(1);		// such that the fire will be put out quickly
			}
			else
			{
				short x, y, x1, y1, x2, y2;
				// ##### begin Gilbert 2/10 ######//
				x1 = target_x_loc - fire_radius + 1;
				if( x1 < 0 )
					x1 = 0;
				y1 = target_y_loc - fire_radius + 1;
				if( y1 < 0 )
					y1 = 0;
				x2 = target_x_loc + fire_radius - 1;
				if( x2 >= world.max_x_loc )
					x2 = world.max_x_loc-1;
				y2 = target_y_loc + fire_radius - 1;
				if( y2 >= world.max_y_loc )
					y2 = world.max_y_loc-1;
				// ##### end Gilbert 2/10 ######//
				for( y = y1; y <= y2; ++y)
				{
					locPtr = world.get_loc(x1, y);
					for( x = x1; x <= x2; ++x, ++locPtr)
					{
						// ##### begin Gilbert 30/10 ######//
						int dist = abs(x-target_x_loc) + abs(y-target_y_loc);
						if( dist > fire_radius)
							continue;
						int fl = 30 - dist * 7;
						if( fl < 10 )
							fl = 10;
						if( locPtr->can_set_fire() && locPtr->fire_str() < fl )
							locPtr->set_fire_str(fl);
						if( locPtr->fire_src() > 0 )
							locPtr->set_fire_src(1);		// such that the fire will be put out quickly
						//		##### begin Gilbert 30/10 ######//
					}
				}
			}
		}
		return 1;
	}
	return 0;
}
//--------- End of function BulletProgram::process_die --------//

//--------- Begin of function BulletProgram::display_layer -------//
char BulletProgram::display_layer()
{
	return 8;
}
//--------- End of function BulletFlame::display_layer -------//

//--------- Begin of function BulletProgram::draw --------//
void BulletProgram::draw(int, int drawFlag)
{
	String str;
	str = sprite_info->sprite_code; 
	if ( str == "LIGHTING" )
	{
		// move use cur_step not cur_frame, since the action will repeat
		// cur_frame is the total no of frame of that sprite
		if ( cur_action == SPRITE_MOVE )
		{
				magic.draw_magic_two(&vga_back, world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest)-400,
								world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest), 5,
								vga.pixel_format_flag, cur_step, total_step);								
		}
		else
		// die use cur_frame not cur_step, since the action don't repeat
		if ( cur_action == SPRITE_DIE )
				magic.draw_magic_nine_die(&vga_back, 
								world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest), 
								vga.pixel_format_flag, cur_frame);
	//	else
	//		err_here();
	}
	else
	if ( str == "MAGIC" )
	{
		// move use cur_step not cur_frame, since the action will repeat
		// cur_frame is the total no of frame of that sprite
		if ( cur_action == SPRITE_MOVE )
		{
				if (config.building_size == 1)	
					magic.draw_magic_ten(&vga_back, world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
								world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init),
								world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest), 
								vga.pixel_format_flag, cur_step, total_step, delay_step);
				else
					magic.draw_magic_ten(&vga_back, world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
								world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init)+50,
								world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest), 
								vga.pixel_format_flag, cur_step, total_step, delay_step);
		}
		else
		// die use cur_frame not cur_step, since the action don't repeat
		if ( cur_action == SPRITE_DIE )
				magic.draw_magic_nine_die(&vga_back, 
								world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
								world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest), 
								vga.pixel_format_flag, cur_frame);
	//	else
	//		err_here();
	}
	else
	if ( str == "MAGIC2" )
	{
		if ( cur_action == SPRITE_DIE )
		{
			if (config.building_size == 1)	
				magic.draw_magic_eleven(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init) -42,
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init) -100, 
					vga.pixel_format_flag, cur_frame);
			else
				magic.draw_magic_eleven(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init) -21,
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init) -70, 
					vga.pixel_format_flag, cur_frame);
		}
	//	else
	//		err_here();
	}
	else
	if ( str == "MAGIC3" )
	{
		if ( cur_action == SPRITE_DIE )
		{
			if (config.building_size == 1)	
				magic.draw_magic_twelve(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init), 
					vga.pixel_format_flag, cur_frame, nation_recno);
			else
				magic.draw_magic_twelve(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init), 
					vga.pixel_format_flag, cur_frame, nation_recno);
		}
	//	else
	//		err_here();
	}
	else
	if ( str == "MAGIC4" )
	{
		if ( cur_action == SPRITE_DIE )
		{
			if (config.building_size == 1)	
				magic.draw_magic_twelve_die(&vga_back, 
					world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
					world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
					vga.pixel_format_flag, cur_frame, nation_recno, target_x_loc + target_y_loc);
			else
				magic.draw_magic_twelve_die(&vga_back, 
					world.zoom_matrix->calc_zoom_x(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
					world.zoom_matrix->calc_zoom_y(target_x_loc* LOCATE_WIDTH, target_y_loc* LOCATE_HEIGHT, z_dest),
					vga.pixel_format_flag, cur_frame, nation_recno, target_x_loc + target_y_loc);
		}
	//	else
	//		err_here();
	}
	else
	if ( str == "MAGIC5" )
	{
		if ( cur_action == SPRITE_DIE )
		{
			if (config.building_size == 1)	
				magic.draw_magic_thirteen(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init) -60, 
					vga.pixel_format_flag, cur_frame);
			else
				magic.draw_magic_thirteen(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init) -4,
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init) -33, 
					vga.pixel_format_flag, cur_frame);
		}
	//	else
	//		err_here();
	}
/*	else
	if ( str == "FIRMDIE" )
	{
		// the following assume the target and shooter is a firm of same recno
		// so lots of checking are missing 
		if ( cur_action == SPRITE_DIE && !firm_array.is_deleted(parent_recno) )
		{
			Firm* firmPtr = firm_array[parent_recno];	
			if (firmPtr)			
				magic.draw_magic_firm_die(&vga_back, 
					world.zoom_matrix->calc_zoom_x(origin_x, origin_y, z_init),
					world.zoom_matrix->calc_zoom_y(origin_x, origin_y, z_init), 
					vga.pixel_format_flag, cur_frame);
		}
	//	else
	//		err_here();
	}*/
}

//--------- End of function BulletProgram::draw --------//

//--------- Begin of function BulletProgram::update_abs_pos ---------//
void BulletProgram::update_abs_pos()
{}
//----------- End of function BulletProgram::update_abs_pos -----------//
