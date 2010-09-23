// Filename    : OB_HOMIN.H
// Description : BulletHoming, Homing bullet

#include <windows.h>
#include <OB_HOMIN.H>
#include <OWORLDMT.H>
#include <OSPRTRES.H>
#include <OSFRMRES.H>
#include <OF_FORT.H>


// --------- begin of function BulletHoming::BulletHoming --------//
BulletHoming::BulletHoming() : Bullet()
{
	target_base_obj_recno = 0;
}
// --------- end of function BulletHoming::BulletHoming --------//


// --------- begin of function BulletHoming::~BulletHoming --------//
BulletHoming::~BulletHoming()
{
	deinit();
}
// --------- end of function BulletHoming::BulletHoming --------//


// --------- begin of function BulletHoming::init --------//
void BulletHoming::init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType)
{
	Bullet::init(parentType, parentRecno, targetXLoc, targetYLoc, targetMobileType);

	// ------- find the maximum range --------//

	BaseObj *baseObjPtr = base_obj_array[parent_base_obj_recno];
	if( baseObjPtr->cast_to_Unit() )
	{
		Unit *parentUnit = base_obj_array[parent_base_obj_recno]->cast_to_Unit();
		AttackInfo* attackInfo = parentUnit->attack_info_array+parentUnit->cur_attack;
		speed = attackInfo->bullet_speed;
		max_step = char((attackInfo->attack_range * LOCATE_WIDTH + speed-1)/ speed);
	}
	else if( baseObjPtr->cast_to_Firm() && baseObjPtr->cast_to_Firm()->cast_to_FirmFort() )
	{
		FirmFort *firmFort = baseObjPtr->cast_to_Firm()->cast_to_FirmFort();
		speed = firmFort->bullet_speed();
		max_step = 100;		// unlimited range
	}
	else
	{
		err_here();
	}

	//--------- keep backup of centre of the bullet ---------//
	SpriteFrame *spriteFrame = cur_sprite_frame();

	// origin_x/y and origin2_x/y are pointing at the centre of the bullet bitmap //
	origin_x += spriteFrame->offset_x + spriteFrame->width/2;
	origin_y += spriteFrame->offset_y + spriteFrame->height/2;
	origin2_x = origin_x; 
	origin2_y = origin_y;
	go_x += spriteFrame->offset_x + spriteFrame->width/2;
	go_y += spriteFrame->offset_y + spriteFrame->height/2;

	// ------- find the target_type and target_recno ------//

	Location *locPtr = world.get_loc(targetXLoc, targetYLoc);
	target_base_obj_recno = locPtr->base_obj_recno(targetMobileType);
}
// --------- end of function BulletHoming::init --------//


// --------- begin of function BulletHoming::deinit --------//
void BulletHoming::deinit()
{
}
// --------- end of function BulletHoming::deinit --------//


// --------- begin of function BulletHoming::process_move --------//
void BulletHoming::process_move()
{
	int actualStep = total_step;


	if( base_obj_array.is_deleted(target_base_obj_recno) )
	{
		target_base_obj_recno = 0;
	}
	else
	{
		BaseObj *baseObj = base_obj_array[target_base_obj_recno];
		Unit *unitPtr = baseObj->cast_to_Unit();

		if( unitPtr && unitPtr->is_visible() )
		{
			// ---- calculate new target_x_loc, target_y_loc -----//	

			target_x_loc = unitPtr->next_x_loc();
			target_y_loc = unitPtr->next_y_loc();

			// ---- re-calculate go_x, go_y  ------//
			// go_x/y and origin2_x/y are pointing at the centre of the bullet bitmap
			// it is different from Bullet
			go_x = unitPtr->cur_x + LOCATE_WIDTH / 2;
			go_y = unitPtr->cur_y + LOCATE_HEIGHT /2;

			//---------- set bullet movement steps -----------//
			SpriteFrame *spriteFrame = cur_sprite_frame();
			int adjX = spriteFrame->offset_x+spriteFrame->width/2;
			int adjY = spriteFrame->offset_y+spriteFrame->height/2;

			int xStep 	= abs(go_x - (cur_x+adjX))/speed;
			int yStep 	= abs(go_y - (cur_y+adjY))/speed;
			total_step  = cur_step +  max(xStep, yStep);

			// a homing bullet has a limited range, if the target go outside the
			// the limit, the bullet can't attack the target
			// in this case, actualStep is the number step from the source
			// to the target; total_step is the max_step
			// otherwise, actualStep is as same as total_step
			
			actualStep = total_step;
			if( total_step > max_step )
			{
				total_step = max_step;
				// target_x_loc and target_y_loc is limited also
				target_x_loc = (cur_x + adjX) + (int)(go_x-(cur_x+adjX)) / (actualStep - total_step) / LOCATE_WIDTH;
				target_x_loc = (cur_y + adjY) + (int)(go_y-(cur_y+adjY)) / (actualStep - total_step) / LOCATE_HEIGHT;
			}
		}
	}

//	origin2_x = origin_x;
//	origin2_y = origin_y;

	// origin_x/y and origin2_x/y are pointing at the centre of the bullet bitmap //
	SpriteFrame *spriteFrame = cur_sprite_frame();
	short adjX = spriteFrame->offset_x + spriteFrame->width/2;
	short adjY = spriteFrame->offset_y + spriteFrame->height/2;
	origin_x = cur_x + adjX;
	origin_y = cur_y + adjY;

	cur_x = origin_x + (int)(go_x-origin_x) / (actualStep + 1 - cur_step);
	cur_y = origin_y + (int)(go_y-origin_y) / (actualStep + 1 - cur_step);
	// cur_x, cur_y is temporary pointing at the centre of bullet bitmap

	// detect changing direction
	if( cur_step > 3 )	// not allow changing direction so fast
		set_dir(origin2_x, origin2_y, cur_x, cur_y);

	// change cur_x, cur_y to bitmap reference point
	spriteFrame= cur_sprite_frame();
	adjX = spriteFrame->offset_x + spriteFrame->width/2;
	adjY = spriteFrame->offset_y + spriteFrame->height/2;
	cur_x -= adjX;
	cur_y -= adjY;

	cur_step++;

	//------- update frame id. --------//

	if( ++cur_frame > cur_sprite_move()->frame_count )
		cur_frame = 1;

	//----- if the sprite has reach the destintion ----//

	if( cur_step > total_step )
	{
		check_hit();

		cur_action = SPRITE_DIE;		// Explosion
		// ###### begin Gilbert 17/5 ########//
		// if it has die frame, adjust cur_x, cur_y to be align with the target_x_loc, target_y_loc
		if( sprite_info->die.first_frame_recno )
		{
			next_x = cur_x = target_x_loc * LOCATE_WIDTH;
			next_y = cur_y = target_y_loc * LOCATE_HEIGHT;
		}
		// ###### end Gilbert 17/5 ########//
		cur_frame = 1;
	}
	// change of total_step may not call warn_target, so call more warn_target
	else if( total_step - cur_step <= 1 )
	{
		warn_target();
	}
}
// --------- end of function BulletHoming::process_move --------//


// -------- begin of function BulletHoming::reflect ---------//

int BulletHoming::reflect(int baseObjRecno )
{
	// BUGHERE : not tested 

	int oldParent = parent_base_obj_recno;

	// origin_x/y is varying, the true original is stored in origin2_x/y
	origin_x = origin2_x;
	origin_y = origin2_y;

	int rc = Bullet::reflect( baseObjRecno );
	err_when( !rc );		// basic bullet must reflect
	target_base_obj_recno = oldParent;

	origin2_x = origin_x;
	origin2_y = origin_y;

	return 1;
}
// -------- end of function BulletHoming::reflect ---------//


