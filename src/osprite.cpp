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

//Filename    : OSPRITE.CPP
//Description : Object Sprite

#include <osprite.h>
#include <all.h>
#include <osfrmres.h>
#include <ostr.h>
#include <ovga.h>
#include <oinfo.h>
#include <ogame.h>
#include <onation.h>
#include <ocoltbl.h>
#include <oconfig.h>
#include <oworldmt.h>

//----------- Define static class member variables -----------//

short Sprite::abs_x1, Sprite::abs_y1;		// the absolute postion, taking in account of sprite offset
short Sprite::abs_x2, Sprite::abs_y2;


//-------- Begin of function Sprite::Sprite --------//

Sprite::Sprite()
{
	memset( sizeof(BaseObj) + (char *)this, 0, sizeof(Sprite) - sizeof(BaseObj) );
}
//--------- End of function Sprite::Sprite --------//


//-------- Begin of function Sprite::~Sprite --------//

Sprite::~Sprite()
{
 	deinit();
}
//--------- End of function Sprite::~Sprite --------//


//--------- Begin of function Sprite::init ---------//
//
void Sprite::init(short spriteId, short startXLoc, short startYLoc)
{
	sprite_id = spriteId;

	cur_x = startXLoc * LOCATE_WIDTH;
	cur_y = startYLoc * LOCATE_HEIGHT;

	go_x = next_x = cur_x;
	go_y = next_y = cur_y;

	cur_attack = 0;

	cur_action = SPRITE_IDLE;
	cur_dir 	  = m.random(MAX_SPRITE_DIR_TYPE);	// facing any of the eight directions
	cur_frame  = 1;
	final_dir  = cur_dir;

	//----- clone vars from sprite_res for fast access -----//

	sprite_info = sprite_res[sprite_id];

	sprite_info->load_bitmap_res();

	//------------- init other vars --------------//

	remain_attack_delay = 0;
	remain_frames_per_step = sprite_info->frames_per_step;
}
//----------- End of function Sprite::init -----------//


//--------- Begin of function Sprite::deinit ---------//
//
void Sprite::deinit()
{
	if( sprite_id && cur_x >= 0 )
	{
		sprite_info->free_bitmap_res();
		sprite_id = 0;
	}
}
//----------- End of function Sprite::deinit -----------//


//--------- Begin of function Sprite::cur_sprite_frame ---------//
//
// Return the current frame of the sprite
//
SpriteFrame* Sprite::cur_sprite_frame(int *needMirror)
{
	UCHAR curDir = display_dir();
	if( needMirror)
		*needMirror = need_mirror(curDir);

	// do not update cur_dir as curDir
	err_when(curDir<0 || curDir>=3*MAX_SPRITE_DIR_TYPE);

	switch( cur_action )
	{
		case SPRITE_MOVE:
		//### begin alex 14/4 ###//
		case SPRITE_SHIP_EXTRA_MOVE:
		//#### end alex 14/4 ####//
			if( guard_count)
			{
				if( curDir >= MAX_SPRITE_DIR_TYPE)
				{
					err_here();
					curDir %= MAX_SPRITE_DIR_TYPE;
				}
				return sprite_frame_res[sprite_info->guard_move_array[curDir].first_frame_recno+cur_frame-1];
			}
			else
				return sprite_frame_res[sprite_info->move_array[curDir].first_frame_recno+cur_frame-1];

		case SPRITE_ATTACK:
			err_when(curDir<0 || curDir>=MAX_SPRITE_DIR_TYPE);
			if( guard_count )
			{
				SpriteGuardStop *guardStopAction = sprite_info->guard_stop_array + curDir;
				return sprite_frame_res[guardStopAction->first_frame_recno+
					MIN(guard_count,guardStopAction->frame_count)-1];
			}
			else
				return sprite_frame_res[sprite_info->attack_array[cur_attack][curDir].first_frame_recno+cur_frame-1];

		case SPRITE_TURN:
		case SPRITE_IDLE:
		case SPRITE_WAIT:
			// air unit needs it own stop frames to float on air
			{
				if( guard_count )
				{
					if( curDir >= MAX_SPRITE_DIR_TYPE)
					{
						// if the sprite is turning, adjust direction to next
						if( turn_delay > 0)
							curDir ++;
						curDir %= MAX_SPRITE_DIR_TYPE;
					}

					SpriteGuardStop *guardStopAction = sprite_info->guard_stop_array + curDir;
					return sprite_frame_res[guardStopAction->first_frame_recno+
						MIN(guard_count,guardStopAction->frame_count)-1];
				}
				else
				{
					SpriteStop *stopAction= sprite_info->stop_array +curDir;
					if(cur_frame > stopAction->frame_count)
						return sprite_frame_res[stopAction->frame_recno];       // first frame
					else            // only few sprite has stopAction->frame_count > 1
						return sprite_frame_res[stopAction->frame_recno+cur_frame-1];
				}
			}

		case SPRITE_DIE:
			if(sprite_info->die.first_frame_recno)			// only if this sprite has dying frame
			{
				if( needMirror)
					*needMirror = 0;			// no need to mirror at any direction
				return sprite_frame_res[sprite_info->die.first_frame_recno+cur_frame-1];
			}

		default:
			return sprite_frame_res[sprite_info->move_array[curDir].first_frame_recno+cur_frame-1];
	}
}
//----------- End of function Sprite::cur_sprite_frame -----------//


//--------- Begin of function Sprite::update_abs_pos ---------//
//
// Update the cur_width & cur_height vars of the sprite for later faster access.
//
// [SpriteFrame *] spriteFrame        pointer to the current (default : NULL)
void Sprite::update_abs_pos(SpriteFrame *spriteFrame)
{
	if( !spriteFrame )
		spriteFrame = cur_sprite_frame();

	int tempZ = get_z();
	abs_x1 = ZoomMatrix::calc_abs_x(cur_x, cur_y, tempZ) + spriteFrame->offset_x;		// absolute position 
	abs_y1 = ZoomMatrix::calc_abs_y(cur_x, cur_y, tempZ) + spriteFrame->offset_y;

	abs_x2 = abs_x1 + spriteFrame->width  - 1;
	abs_y2 = abs_y1 + spriteFrame->height - 1;
}
//----------- End of function Sprite::update_abs_pos -----------//


//--------- Begin of function Sprite::draw ---------//
//
void Sprite::draw(int, int drawFlag)
{
	//--------- draw sprite on the zoom window ---------//

	int needMirror;
	SpriteFrame* spriteFrame = cur_sprite_frame(&needMirror);
	// update_abs_pos(spriteFrame);

	err_when( !sprite_info->res_bitmap.init_flag );

	char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);

	// IGNORE sprite_info->remap_bitmap_flag

	world.zoom_matrix->put_bitmap_offset(cur_x, cur_y, get_z(),
		bitmapPtr, spriteFrame->offset_x, spriteFrame->offset_y,
		NULL, needMirror, (sprite_info->remap_bitmap_flag ? 2 : 1) | (drawFlag << 2) );

}
//----------- End of function Sprite::draw -----------//


void Sprite::draw_abs(int colorSchemeId, int scrnX, int scrnY, int clipX1, int clipY1, int clipX2, int clipY2 )
{
	int needMirror;
	SpriteFrame *spriteFrame = cur_sprite_frame(&needMirror);

	// update_abs_pos(spriteFrame);

	err_when( !sprite_info->res_bitmap.init_flag );

	char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);

	short* colorRemapTable = game.color_remap_array[colorSchemeId].color_table;
	if( sprite_info->palw_ptr )
	{
		#define FIRST_REMAP_KEY       0xE0        // the source color code of the colors to be remapped
		#define REMAP_KEY_COUNT       8
		memcpy( sprite_info->palw_ptr+FIRST_REMAP_KEY,
			colorRemapTable + FIRST_REMAP_KEY, (sizeof(short) *REMAP_KEY_COUNT) );
		colorRemapTable = sprite_info->palw_ptr;
	}

	int x1 = scrnX + spriteFrame->offset_x;
	int y1 = scrnY + spriteFrame->offset_y;
	int x2 = x1 + spriteFrame->width - 1;
	int y2 = y1 + spriteFrame->height - 1;

	// -------- skip if whole image outside -------//

	if( x1 > clipX2 || x2 < clipX1 || y1 > clipY2 || y2 < clipY1 )
		return;

	// -------- test if partially inside --------//

	if( x1 >= clipX1 && x2 <= clipX2 && y1 >= clipY1 && y2 <= clipY2 )
	{
		switch( sprite_info->remap_bitmap_flag )
		{
		case 0:		// compressed
			if( !needMirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans_decompress(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_trans_remap_decompress(x1, y1, bitmapPtr, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_trans_decompress_hmirror(x1, y1, bitmapPtr);
				else
					vga_back.put_bitmap_trans_remap_decompress_hmirror(x1, y1, bitmapPtr, colorRemapTable);
			}
			break;
		case 1:		// uncompressed, blending bitmap
			// ignore coloRemapTable
			if( !needMirror )
			{
				vga_back.put_bitmap_blend(x1, y1, bitmapPtr);
			}
			else
			{
				vga_back.put_bitmap_blend_hmirror(x1, y1, bitmapPtr);
			}
			break;

		default:
			err_here();
		}
	}
	else
	{
		int srcX1 = x1 >= clipX1 ? 0 : clipX1-x1;
		int srcY1 = y1 >= clipY1 ? 0 : clipY1-y1;
		int srcX2 = x2 <= clipX2 ? x2 - x1 : clipX2 - x1;
		int srcY2 = y2 <= clipY2 ? y2 - y1 : clipY2 - y1;

		switch(sprite_info->remap_bitmap_flag)
		{
		case 0:		// compressed
			if( !needMirror )
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap_decompress(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			else
			{
				if( !colorRemapTable )
					vga_back.put_bitmap_area_trans_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2);
				else
					vga_back.put_bitmap_area_trans_remap_decompress_hmirror(x1, y1, bitmapPtr,
						srcX1, srcY1, srcX2, srcY2, colorRemapTable);
			}
			break;
		case 1:		// uncompressed, blending
			// ignore coloRemapTable
			if( !needMirror )
			{
				vga_back.put_bitmap_blend_area(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			else
			{
				vga_back.put_bitmap_blend_area_hmirror(x1, y1, bitmapPtr, 
					srcX1, srcY1, srcX2, srcY2);
			}
			break;

		default:
			err_here();
		}

	}
}


// return false if the whole bitmap out the clip rect
int Sprite::get_abs_rect( int scrnX, int scrnY, int clipX1, int clipY1, int clipX2, int clipY2,
	int *rectX1, int *rectY1, int *rectX2, int *rectY2)
{
	int needMirror;
	SpriteFrame *spriteFrame = cur_sprite_frame(&needMirror);

	int x1 = scrnX + spriteFrame->offset_x;
	int y1 = scrnY + spriteFrame->offset_y;
	int x2 = x1 + spriteFrame->width - 1;
	int y2 = y1 + spriteFrame->height - 1;

	if( x1 > clipX2 || x2 < clipX1 || y1 > clipY2 || y2 < clipY1 )
		return 0;

	*rectX1 = x1 >= clipX1 ? x1 : clipX1;
	*rectY1 = y1 >= clipY1 ? y1 : clipY1;
	*rectX2 = x2 <= clipX2 ? x2 : clipX2;
	*rectY2 = y2 <= clipY2 ? y2 : clipY2;

	return 1;
}



// ---------- Begin of function Sprite::display_dir ---------//
UCHAR Sprite::display_dir()
{
	// for isometric, display direction is different from actual direction
	// e.g. cur_dir is North, display direction is NE

	// UCHAR curDir = (cur_dir & ~7) | ((cur_dir & 7) + 1) & 7);
	UCHAR curDir = (cur_dir & ~7) | ((cur_dir + 1) & 7);

	switch( sprite_info->turn_resolution)
	{
	case 0:		// fall through
	case 1:
		curDir &= ~7;		// direction less, remain upward or downard, but set to north
		break;
	case 8:
		// cur_dir can be 0 to 3*MAX_SPRITE_DIR_TYPE-1, such as projectile;
		// curDir = cur_dir;
		break;
	case 16:
		err_when(curDir<0 || curDir>=MAX_SPRITE_DIR_TYPE);
		// curDir should be (from due north, clockwisely) { 0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15 }
		if( turn_delay <= -30)
		{
			curDir = ((curDir+7) & 7) + 8;
		}
		else if( turn_delay >= 30)
		{
			curDir += 8;
		}
		break;
	case 24:
		err_when(curDir<0 || curDir>=MAX_SPRITE_DIR_TYPE);
		// curDir should be (from due north, clockwisely) 
		// { 0,8,16,1,9,17,2,10,18,3,11,19,4,12,20,5,13,21,6,14,22,7,15,23 }
		if( turn_delay <= -20)
		{
			if( turn_delay <= -40)
				curDir = ((curDir+7) & 7) + 8;
			else
				curDir = ((curDir+7) & 7) + 16;
		}
		else if( turn_delay >= 20)
		{
			if( turn_delay >= 40 )
				curDir += 16;
			else
				curDir += 8;
		}
		break;
	default:
		err_here();
	}
	return curDir;
}
// ---------- End of function Sprite::display_dir ---------//


// ---------- Begin of function Sprite::need_mirror --------//
int Sprite::need_mirror(UCHAR dispDir)
{
	return (dispDir < 8 || sprite_info->turn_resolution <= 8) ? (dispDir & 7) >= 5 : (dispDir & 7) >= 4;
}
// ---------- End of function Sprite::need_mirror --------//


// ---------- Begin of function Sprite::is_stealth --------//
int Sprite::is_stealth()
{
	if( !config.blacken_map )
		return 0;

	// ignore loc_width > 1

	if( config.fog_of_war )
	{
		// if the visibility of location is just explored, consider stealth
		return world.get_loc(cur_x_loc(), cur_y_loc())->visibility() <= EXPLORED_VISIBILITY;
	}
	else if( !config.explore_whole_map )
	{
		return !world.get_loc(cur_x_loc(), cur_y_loc())->explored();
	}
	else
		return 0;
}
// ---------- End of function Sprite::is_stealth --------//


//---------- Begin of function Sprite::get_z --------//

short Sprite::get_z()
{
	// projectile shadow may get out of map
	int curX = cur_x;
	if( curX < 0 )
		curX = 0;
	else if( curX >= MAX_WORLD_X_LOC * LOCATE_WIDTH )
		curX = MAX_WORLD_X_LOC * LOCATE_WIDTH - 1;
	int curY = cur_y;
	if( curY < 0 )
		curY = 0;
	else if( curY >= MAX_WORLD_Y_LOC * LOCATE_HEIGHT )
		curY = MAX_WORLD_Y_LOC * LOCATE_HEIGHT - 1;
	return (short) world.interpolate_z(curX, curY);
}
//---------- End of function Sprite::get_z --------//
