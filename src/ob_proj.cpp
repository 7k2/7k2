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

// Filename   : OB_PROJ.CPP
// Description : header file for Projectile
// Owner       : Gilbert


#include <ob_proj.h>


// --------- Begin of function Projectile::Projectile --------//
Projectile::Projectile() : Bullet(), act_bullet(), bullet_shadow()
{
	act_bullet.sprite_recno = 0;
	bullet_shadow.sprite_recno = 0;
}
// --------- End of function Projectile::Projectile --------//


// --------- Begin of function Projectile::~Projectile --------//
Projectile::~Projectile()
{
	deinit();
}
// --------- End of function Projectile::~Projectile --------//


// --------- Begin of function Projectile::init --------//
//### begin alex 3/5 ###//
void Projectile::init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType)
{
	Bullet::init(parentType, parentRecno, targetXLoc, targetYLoc, targetMobileType);
//#### end alex 3/5 ####//
	short spriteId = sprite_info->get_sub_sprite_info(1)->sprite_id;
	act_bullet.init( spriteId, cur_x_loc(), cur_y_loc() );
	short shadowSpriteId = sprite_info->get_sub_sprite_info(2)->sprite_id;
	bullet_shadow.init( shadowSpriteId, cur_x_loc(), cur_y_loc() );

	// calculate z_coff;
	z_coff = (float)1.0;
	/*
	float dz = z_coff * total_step;
	if( dz >= 10.0)
		cur_dir = cur_dir & 7 | 8;					// pointing up
	else if( dz <= -10.0)
		cur_dir = cur_dir & 7 | 16;				// pointing down
	else
		cur_dir &= 7;
	*/

	// --------- recalcuate spriteFrame pointer ----------//
	SpriteFrame* spriteFrame = cur_sprite_frame();
}
// --------- End of function Projectile::init --------//


// --------- Begin of function Projectile::deinit --------//
void Projectile::deinit()
{
}
// --------- End of function Projectile::deinit --------//


// --------- Begin of function Projectile::draw --------//
void Projectile::draw(int outLine, int drawFlag)
{
	short z = get_z();

	// does not draw itself but the shadow and the actual bullet
	if( cur_action == SPRITE_MOVE )
	{
		// -------- update cur_frame ----------//
		// find last z of last step
		cur_step--;
		short dz = z - get_z();
		cur_step++;

		// ####### begin Gilbert 3/3 #########//

		// ------- force set cur_dir temporary for displaying -------//

		UCHAR tempDir = final_dir;
		UCHAR tempCurDir = cur_dir;

		if( dz >= 8)
			tempDir = (tempDir & 7 )| 8;					// pointing up
		else if( dz <= -8)
			tempDir = (tempDir & 7 )| 16;				// pointing down
		else
			tempDir &= 7;

		// ------- update bullet_shadow coordinate --------//
		err_when((bullet_shadow.sprite_info)->need_turning);
		bullet_shadow.set_dir(tempDir);
		bullet_shadow.cur_frame = cur_frame;
		bullet_shadow.cur_action = SPRITE_MOVE;
		bullet_shadow.set_cur(cur_x, cur_y);

		// ------- update act_bullet coordinate --------//
		err_when((act_bullet.sprite_info)->need_turning);
		act_bullet.set_dir(tempDir);
		act_bullet.cur_frame = cur_frame;
		act_bullet.cur_action = SPRITE_MOVE;
		act_bullet.set_cur(cur_x, cur_y);
		
		cur_dir = tempDir;

		bullet_shadow.draw(outLine, drawFlag);
		// act_bullet.draw(outLine, drawFlag);
		Bullet::draw(outLine, drawFlag);
		
		// restore cur_dir
		// set_dir((tempDir&7));
		cur_dir = tempCurDir;
		// ####### end Gilbert 3/3 #########//
	}
	else
	{
		// ###### begin Gilbert 3/3 #########//
		UCHAR tempCurDir;
		if( cur_action = SPRITE_DIE )
		{
			tempCurDir = cur_dir;
			cur_dir = (final_dir & 7) | 16;			// downward direction
		}
		Bullet::draw(outLine, drawFlag);
		if( cur_action = SPRITE_DIE )
		{
			cur_dir = tempCurDir;
		}
		// ###### end Gilbert 3/3 #########//
	}
}
// --------- End of function Projectile::draw --------//


// --------- Begin of function Projectile::display_layer --------//
char Projectile::display_layer()
{
	//### begin alex 3/5 ###//
	if(mobile_type == UNIT_AIR || target_mobile_type==UNIT_AIR)
	//#### end alex 3/5 ####//
		return 8;
	else
		return 2;
}
// --------- End of function Projectile::display_layer --------//


short Projectile::get_z()
{
	// z = a*(cur_step)^2 + b(cur_step) + c
	// if cur_step = 0, z = z_init, so c = z_init
	// if cur_step = total_step+1, z = z_dest
	// acceleration = 2a = gravity, is a constant

	// b = ((z_dest - z_init) - a*(total_step)^2) / total_step
	// z = a*cur_step*(cur_step - total_step - 1)
	//     + (z_dest - z_init)*cur_step/(total_step+1)
	//     + z_init

	const short a = -4;	// GRAVITY = -8;

	return a * cur_step * (cur_step - total_step - 1)
		+ (z_dest - z_init) * cur_step / (total_step + 1)
		+ z_init;
}


int Projectile::reflect(int baseObjRecno)
{
	return Bullet::reflect(baseObjRecno);
}
