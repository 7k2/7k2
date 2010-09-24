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

// Filename    : OEFFECT.CPP
// Description : effect array

// note : Effect and effect_array are not saved

#include <OWORLDMT.H>
#include <OEFFECT.H>
#include <OMAGIC.H>
#include <OVGA.H>

// ------- Begin of function Effect::Effect -------//
Effect::Effect() : Sprite()
{
	layer = 1;	// default in land display layer
	life = 0;	// disappear when life < 0
}
// ------- End of function Effect::Effect -------//


// ------- Begin of function Effect::~Effect -------//
Effect::~Effect()
{
}
// ------- End of function Effect::~Effect -------//


// ------- Begin of function Effect::init -------//
//
// for directed effect, put initAction as SPRITE_IDLE and set appropriate initDir
// undirected effect, put initAction as SPRITE_DIE and set initDir to 0
// to find the life of sprite automatically, set effectLife to 0 or negative
//
void Effect::init(short spriteId, short startX, short startY, char initAction, char initDir, char dispLayer, int effectLife)
{
	sprite_id = spriteId;

	cur_x = startX;
	cur_y = startY;

	go_x = next_x = cur_x;
	go_y = next_y = cur_y;

	cur_attack = 0;

	err_when( initAction != SPRITE_IDLE && initAction != SPRITE_DIE);
	cur_action = initAction;
	cur_dir 	  = initDir;
	cur_frame  = 1;

	//----- clone vars from sprite_res for fast access -----//

	sprite_info = sprite_res[sprite_id];

	if (sprite_info->sprite_sub_type == 'M')
	{
		program_effect_flag = 1;
		String str;
		str = sprite_info->sprite_code; 
		if ( str == "FIRMDIE" )
			program_effect_id = 1;
		else
		if ( str == "REFLECT" )
			program_effect_id = 2;
		else
		if ( str == "ICE" )
			program_effect_id = 3;
		else
			err_here();
	}
	else
	{
		program_effect_flag = 0;
		program_effect_id = 0;
	}

	sprite_info->load_bitmap_res();

	// -------- adjust cur_dir -----------//
	if( sprite_info->turn_resolution <= 1)
		cur_dir = 0;
	final_dir  = cur_dir;

	//------------- init other vars --------------//

	remain_attack_delay = 0;
	remain_frames_per_step = sprite_info->frames_per_step;

	layer = dispLayer;
	if( effectLife > 0)
		life = effectLife;
	else
	{
		switch( cur_action )
		{
		case SPRITE_IDLE:
			life = sprite_info->stop_array[cur_dir].frame_count - cur_frame;
			break;
		case SPRITE_DIE:
			life = sprite_info->die.frame_count - cur_frame;
			break;
		default:
			err_here();
		}
	}
}
// ------- End of function Effect::init -------//


// ------- Begin of function Effect::pre_process -------//
void Effect::pre_process()
{
	if( --life < 0)
	{
		effect_array.del(sprite_recno);
		return;
	}
}
// ------- End of function Effect::pre_process -------//


// ------- Begin of function Effect::process_idle -------//
void Effect::process_idle()
{
	if( ++cur_frame > cur_sprite_stop()->frame_count )
		cur_frame = 1;
}
// ------- End of function Effect::process_idle -------//


// ------- Begin of function Effect::process_die -------//
int Effect::process_die()
{
	if( ++cur_frame > cur_sprite_die()->frame_count )
		cur_frame = 1;
	return 0;
}
// ------- End of function Effect::process_die -------//


// ------- Begin of function Effect::create -------//
void Effect::create(short spriteId, short startX, short startY, char initAction, char initDir, char dispLayer, int effectLife)
{
	Effect *effectPtr = new Effect;
	effectPtr->init(spriteId, startX, startY, initAction, initDir, dispLayer, effectLife);
	effect_array.add(effectPtr);
}
// ------- End of function Effect::create -------//

// ------- Begin of function Effect::draw -------//
void Effect::draw(int, int drawFlag)
{
	if (program_effect_flag)
		program_effect_draw();
	else
		Sprite::draw(0, drawFlag);
}
// ------- End of function Effect::draw -------//

// ------- Begin of function Effect::program_effect_draw -------//
void Effect::program_effect_draw()
{
	switch (program_effect_id)
	{
	case 1:
		magic.draw_magic_firm_die(&vga_back, 
			world.zoom_matrix->calc_zoom_x(cur_x, cur_y, get_z()),
			world.zoom_matrix->calc_zoom_y(cur_x, cur_y, get_z()), 
			vga.pixel_format_flag, cur_frame);
		break;

	case 2:
		magic.draw_magic_forteen(&vga_back, 
			world.zoom_matrix->calc_zoom_x(cur_x, cur_y, get_z()),
			world.zoom_matrix->calc_zoom_y(cur_x, cur_y, get_z()), 
			vga.pixel_format_flag, cur_frame, cur_dir);
		break;

	case 3:
		magic.draw_magic_fifteen(&vga_back, 
			world.zoom_matrix->calc_zoom_x(cur_x, cur_y, get_z()),
			world.zoom_matrix->calc_zoom_y(cur_x, cur_y, get_z()), 
			vga.pixel_format_flag, cur_frame);
		break;
	
	default:
		err_here();
		break;
	}
}
// ------- End of function Effect::program_effect_draw -------//

// ------- Begin of function Effect::update_abs_pos -------//
void Effect::update_abs_pos(SpriteFrame *spriteFrame)
{
	switch (program_effect_id)
	{
	case 0:
		Sprite::update_abs_pos(spriteFrame);
		break;

	case 1:
		abs_x1 = ZoomMatrix::calc_abs_x(cur_x, cur_y, get_z()) - 100;		// absolute position 
		abs_y1 = ZoomMatrix::calc_abs_y(cur_x, cur_y, get_z()) - 100;
		abs_x2 = abs_x1 + 200;
		abs_y2 = abs_y1 + 200;
		break;

	case 2:
		abs_x1 = ZoomMatrix::calc_abs_x(cur_x, cur_y, get_z()) - 50;		// absolute position 
		abs_y1 = ZoomMatrix::calc_abs_y(cur_x, cur_y, get_z()) - 25;
		abs_x2 = abs_x1 + 100;
		abs_y2 = abs_y1 + 50;
		break;

	case 3:
		abs_x1 = ZoomMatrix::calc_abs_x(cur_x, cur_y, get_z()) - 100;		// absolute position 
		abs_y1 = ZoomMatrix::calc_abs_y(cur_x, cur_y, get_z()) - 100;
		abs_x2 = abs_x1 + 200;
		abs_y2 = abs_y1 + 200;
		break;
	
	default:
		err_here();
		break;
	}
}