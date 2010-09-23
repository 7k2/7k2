//Filename    : OBULLET.CPP
//Description : Object Bullet
//Owner		  : Alex

#include <OVGA.H>
#include <OUNIT.H>
#include <OBULLET.H>
#include <OF_FORT.H>
#include <OWORLD.H>
#include <OSERES.H>
#include <OU_CART.H>
#include <OTOWN.H>
#include <ONATIONA.H>
#include <OWORLDMT.H>
#include <OSPRTRES.H>
#include <OSFRMRES.H>
#include <OUN_SPAB.H>
#include <math.h>
#include <OSYS.H>
#include <OEFFECT.H>

// -------- Define constant ---------//

const SCAN_RADIUS = 2;
const SCAN_RANGE = SCAN_RADIUS * 2 + 1;

// from the closet to the far
static char spiral_x[SCAN_RANGE*SCAN_RANGE] = 
	{ 0, 0,-1, 0, 1,-1,-1, 1, 1, 0,-2, 0, 2, -1,-2,-2,-1, 1, 2, 2, 1,-2,-2, 2, 2};
static char spiral_y[SCAN_RANGE*SCAN_RANGE] = 
	{ 0,-1, 0, 1, 0,-1, 1, 1,-1,-2, 0, 2, 0, -2,-1, 1, 2, 2, 1,-1,-2,-2, 2, 2,-2};

//--------- Begin of function Bullet::Bullet -------//

Bullet::Bullet()
{
	sprite_id = 0;
}
//--------- End of function Bullet::Bullet -------//


//--------- Begin of function Bullet::init ---------//
//
// <char> parentType		- the type of object emits the bullet
// <short> parentRecno	- the recno of the object
// <short> targetXLoc	- the x loc of the target
// <short> targetYLoc	- the y loc of the target
//	<char> targetMobileType - target mobile type
//
void Bullet::init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType)
{
	parent_recno = parentRecno;
	target_mobile_type = targetMobileType;
//	int byPassFlag = 0;

	Unit *parentUnit = NULL;
	Firm *parentFirm = NULL;
	Town *parentTown = NULL;

	if( parentType == BULLET_BY_UNIT )
	{
		parentUnit = unit_array[parentRecno];
		parent_base_obj_recno = parentUnit->base_obj_recno;
	}
	else if( parentType == BULLET_BY_FIRM )
	{
		parentFirm = firm_array[parentRecno];
		parent_base_obj_recno = parentFirm->base_obj_recno;
	}
	else if( parentType == BULLET_BY_TOWN )
	{
		parentTown = town_array[parentRecno];
		parent_base_obj_recno = parentTown->base_obj_recno;
	}
	else
	{
		err_here();
	}

	BaseObj *BaseObjPtr = base_obj_array[parent_base_obj_recno];

	int bulletSpeed = 10;

	if( parentUnit )
	{
		//---------- copy attack info from the parent unit --------//

		AttackInfo* attackInfo = parentUnit->attack_info_array+parentUnit->cur_attack;

		attack_damage  = parentUnit->actual_damage();
		damage_radius  = attackInfo->bullet_radius;
		nation_recno   = parentUnit->nation_recno;
		fire_radius    = attackInfo->fire_radius;
		sprite_id 	   = attackInfo->bullet_sprite_id;
		mobile_type    = parentUnit->mobile_type;	
		bulletSpeed    = attackInfo->bullet_speed;
		attack_attribute = attackInfo->attack_attribute;
	}
	else if( parentFirm )
	{
		nation_recno   = parentFirm->nation_recno;

	/*	if (parentFirm->center_x == targetXLoc && parentFirm->center_y == targetYLoc &&
			 (parentFirm->bullet_id() == 0 || sprite_res[parentFirm->bullet_id()]->sprite_sub_type != 'M'))
		{
			attack_damage  = 0;
			damage_radius  = 1;
			fire_radius    = 0;
			sprite_id		= sprite_res.search_sprite( "FIRMDIE" );
			bulletSpeed		= 1;
			byPassFlag = 1;
		}
		else		*/
		{
			attack_damage  = parentFirm->bullet_damage();
			damage_radius  = parentFirm->bullet_radius();
			fire_radius    = parentFirm->bullet_fire();
			sprite_id		= parentFirm->bullet_id();
			bulletSpeed		= parentFirm->bullet_speed();
		}

		mobile_type    = UNIT_LAND;
		attack_attribute.init(40, 0, 0, 0);
	}
	else if( parentTown )
	{
		attack_damage  = parentTown->bullet_damage();
		damage_radius  = parentTown->bullet_radius();
		nation_recno   = parentTown->nation_recno;
		fire_radius    = parentTown->bullet_fire();
		sprite_id 	   = parentTown->bullet_id();
		mobile_type    = UNIT_LAND;
		bulletSpeed    = parentTown->bullet_speed();
		attack_attribute.init(0, 0, 0, 0);
	}
	else
	{
		err_here();
	}

	//----- clone vars from sprite_res for fast access -----//

	sprite_info = sprite_res[sprite_id];

	sprite_info->load_bitmap_res();		// the sprite bitmap will be freed by ~Sprite(), so we don't have to add ~Bullet() to free it. 

	//--------- set the starting position of the bullet -------//

	cur_action = SPRITE_MOVE;
	cur_frame  = 1;

	if( parentUnit )
	{
		set_dir(parentUnit->attack_dir);
		origin_x = cur_x = parentUnit->cur_x;
		origin_y = cur_y = parentUnit->cur_y;

	}
	else if( parentFirm )
	{
		if (parentFirm->cast_to_FirmLishorr() || parentFirm->cast_to_FirmOffensive2())
		{
			set_dir( parentFirm->center_x, parentFirm->center_y, targetXLoc, targetYLoc );
			origin_x = cur_x = parentFirm->loc_x1 * LOCATE_WIDTH;
			origin_y = cur_y = parentFirm->loc_y1 * LOCATE_HEIGHT;
		}
		else
		{
			set_dir( parentFirm->center_x, parentFirm->center_y, targetXLoc, targetYLoc );
		/*	if (byPassFlag == 1)
			{
				origin_x = cur_x = (parentFirm->loc_x1 +
					sys.frame_count %(parentFirm->loc_x2 -parentFirm->loc_x1 +1)) *LOCATE_WIDTH;
				origin_y = cur_y = (parentFirm->loc_y1 +
					(sys.frame_count + parentFirm->firm_recno)%(parentFirm->loc_y2 -parentFirm->loc_y1 +1)) *LOCATE_HEIGHT;
			}
			else*/
			{
				origin_x = cur_x = parentFirm->center_x * LOCATE_WIDTH;
				origin_y = cur_y = parentFirm->center_y * LOCATE_HEIGHT;
			}
		}
	}
	else if( parentTown )
	{
		set_dir( parentTown->center_x, parentTown->center_y, targetXLoc, targetYLoc );
		origin_x = cur_x = parentTown->center_x * LOCATE_WIDTH;
		origin_y = cur_y = parentTown->center_y * LOCATE_HEIGHT;
	}
	else
		err_here();

	//------ set the target position and bullet mobile_type -------//

	target_x_loc = targetXLoc;
	target_y_loc = targetYLoc;

	err_when( abs( origin_x/LOCATE_WIDTH  - target_x_loc ) >= MAX_WORLD_X_LOC );
	err_when( abs( origin_y/LOCATE_HEIGHT - target_y_loc ) >= MAX_WORLD_Y_LOC );

	// So the bullet will be centered on the target

	SpriteFrame* spriteFrame = cur_sprite_frame();

	int correctAbsX = - spriteFrame->offset_x - spriteFrame->width/2;
	int correctAbsY = - spriteFrame->offset_y - spriteFrame->height/2;
	go_x = target_x_loc * LOCATE_WIDTH  + LOCATE_WIDTH/2 + ZoomMatrix::calc_cur_x(correctAbsX, correctAbsY);
	go_y = target_y_loc * LOCATE_HEIGHT + LOCATE_HEIGHT/2 + ZoomMatrix::calc_cur_y(correctAbsX, correctAbsY);

	// testing reversiblitly of calc_abs_x/y and calc_cur_x/y
#ifdef DEBUG
	int correctCurX = ZoomMatrix::calc_cur_x(correctAbsX, correctAbsY);
	int correctCurY = ZoomMatrix::calc_cur_y(correctAbsX, correctAbsY);
	err_when( abs(correctAbsX - ZoomMatrix::calc_abs_x(correctCurX, correctCurY, 0)) > 1);
	err_when( abs(correctAbsY - ZoomMatrix::calc_abs_y(correctCurX, correctCurY ,0)) > 1);
#endif

	//---------- set bullet movement steps -----------//

	int xStep 	= (go_x - cur_x)/bulletSpeed;
	int yStep 	= (go_y - cur_y)/bulletSpeed;

	total_step  = max(1, max(abs(xStep), abs(yStep)));
	cur_step    = 0;

	err_when( total_step < 0 );		// number overflow

	// ------- find init z and final z ----------//

	if( parentUnit )
	{
		z_init = parentUnit->get_z();
	}
	else if( parentFirm )
	{
	/*	if (byPassFlag == 1)
			z_init = parentFirm->altitude;
		else*/
			z_init = parentFirm->bullet_init_z();
	}
	else if( parentTown )
	{
		z_init = parentTown->bullet_init_z();
	}
	else
	{
		z_init = world.interpolate_z(origin_x, origin_x);
	}

	z_dest = world.interpolate_z(target_x_loc * LOCATE_WIDTH, target_y_loc * LOCATE_HEIGHT);

	if( target_mobile_type == UNIT_AIR )
	{
		z_dest += 32;
	}
}
//----------- End of function Bullet::init -----------//


//--------- Begin of function Bullet::get_z --------//
short Bullet::get_z()
{
	return z_init + (int)(z_dest - z_init) * cur_step / total_step;
}
//--------- End of function Bullet::get_z --------//


//--------- Begin of function Bullet::process_move --------//

void Bullet::process_move()
{
	//-------------- update position -----------------//
	//
	// If it gets very close to the destination, fit it
	// to the destination ingoring the normal vector.
	//
	//------------------------------------------------//

	cur_x = origin_x + (int)(go_x-origin_x) * cur_step / total_step;
	cur_y = origin_y + (int)(go_y-origin_y) * cur_step / total_step;

	//cur_step++;
 
	//------- update frame id. --------//

	if( ++cur_frame > cur_sprite_move()->frame_count )
		cur_frame = 1;

	//----- if the sprite has reach the destintion ----//

	//if( cur_step > total_step )
	if( ++cur_step > total_step )
	{
		check_hit();

		cur_action = SPRITE_DIE;		// Explosion

		// if it has die frame, adjust cur_x, cur_y to be align with the target_x_loc, target_y_loc
		if( sprite_info->die.first_frame_recno )
		{
			next_x = cur_x = target_x_loc * LOCATE_WIDTH;
			next_y =cur_y = target_y_loc * LOCATE_HEIGHT;
		}
		cur_frame = 1;
	}
	else if( total_step - cur_step == 1 )
	{
		warn_target();
	}
}
//---------- End of function Bullet::process_move ----------//


//--------- Begin of function Bullet::process_die --------//
//
// return : <int> 1 - dying animation completes.
//					   0 - still dying 
//
int Bullet::process_die()
{
	// ------- sound effect --------//
	se_res.sound(cur_x_loc(), cur_y_loc(), cur_frame, 'S',sprite_id,"DIE");

	//--------- next frame ---------//
	if( ++cur_frame > sprite_info->die.frame_count )
	{
		// ------- set fire on the target area --------//
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
				for( y = y1; y <= y2; ++y)
				{
					locPtr = world.get_loc(x1, y);
					for( x = x1; x <= x2; ++x, ++locPtr)
					{
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
					}
				}
			}
		}
		return 1;
	}
	return 0;
}
//--------- End of function Bullet::process_die --------//


//--------- Begin of function Bullet::hit_target --------//

void Bullet::hit_target(short x, short y)
{
	//---- check if there is any unit in the target location ----//

	Location* locPtr = world.get_loc(x, y);

	short	targetUnitRecno = locPtr->unit_recno(target_mobile_type);
	if(unit_array.is_deleted(targetUnitRecno))
		return; // the target unit is deleted

	Unit* targetUnit = unit_array[targetUnitRecno];

	BaseObj *parentUnit = NULL;
	nation_recno = 0;

	if( !base_obj_array.is_deleted(parent_base_obj_recno) )
	{
		parentUnit = base_obj_array[parent_base_obj_recno];
		nation_recno = parentUnit->nation_recno;
	}

	// #### begin Gilbert 10/11 ######//
	float attackDamage = attenuated_damage(targetUnit)
		* attack_attribute.effectiveness(targetUnit->defense_attribute);

	// bonus from attack direction, 50% from back, 25% from side
	int dirDiff = abs( (cur_dir & 7) - (targetUnit->cur_dir & 7) );

	// same dir means attack from behind, dirDiff is 0
	// opposite direction, dirDiff is 4, no bonus

	if( dirDiff < 4 )
		attackDamage += attackDamage * (4-dirDiff) / 8;
	else if( dirDiff > 4 )
		attackDamage += attackDamage * (dirDiff-4) / 8;
	// #### end Gilbert 10/11 ######//

	// -------- if the unit is guarding reduce damage ----------//
	err_when(unit_array.is_deleted(locPtr->unit_recno(target_mobile_type)));
	if( attackDamage == 0 )
		return;

	if( targetUnit->is_nation(nation_recno) )
	{
		if( targetUnit->unit_id == UNIT_EXPLOSIVE_CART )
			((UnitExpCart *)targetUnit)->trigger_explode();
		return;
	}
	if( !nation_array.should_attack(nation_recno, targetUnit->nation_recno) )
		return;
	if(targetUnit->is_guarding())
	{
		switch(targetUnit->cur_action)
		{
		case SPRITE_IDLE:
		case SPRITE_READY_TO_MOVE:
		case SPRITE_TURN:
		case SPRITE_MOVE:
		// case SPRITE_ATTACK:
			// check if on the opposite direction
			if( (targetUnit->cur_dir & 7)== ((cur_dir + 4 ) & 7)
				|| (targetUnit->cur_dir & 7)== ((cur_dir + 3 ) & 7)
				|| (targetUnit->cur_dir & 7)== ((cur_dir + 5 ) & 7) )
			{
				// ###### begin Gilbert 22/10 ###### //
				// attackDamage = attackDamage > (float)10/ATTACK_SLOW_DOWN ? attackDamage - (float)10/ATTACK_SLOW_DOWN : 0;
				// do not divide by ATTACK_SLOW_DOWN
				attackDamage -= 10.0f;
				// ###### end Gilbert 22/10 ###### //
				se_res.sound( targetUnit->cur_x_loc(), targetUnit->cur_y_loc(), 1,
				'S', targetUnit->sprite_id, "DEF", 'S', sprite_id );
			}
			break;

		case SPRITE_ATTACK:
			// check if on the opposite direction
			if( (targetUnit->cur_dir & 7)== ((cur_dir + 4 ) & 7)
				|| (targetUnit->cur_dir & 7)== ((cur_dir + 3 ) & 7)
				|| (targetUnit->cur_dir & 7)== ((cur_dir + 5 ) & 7) )
			{
				attackDamage -= 4.0f;
				// no sound if attacking
			}
			break;
		}
	}

	targetUnit->being_attack_hit(parentUnit, attackDamage);
}
//---------- End of function Bullet::hit_target ----------//


//------- Begin of function Bullet::hit_building -----//
//	building means firm or town
//
void Bullet::hit_building(short x, short y)
{
	Location* locPtr = world.get_loc(x, y);
	BaseObj*  targetObj;

	if(locPtr->is_firm())
	{
		Firm *firmPtr = firm_array[locPtr->firm_recno()];

		if( !firmPtr || !nation_array.should_attack(nation_recno, firmPtr->nation_recno) )
			return;

      targetObj = firmPtr;
	}
	else if(locPtr->is_town())
	{
		Town *townPtr = town_array[locPtr->town_recno()];

		if( !townPtr || !nation_array.should_attack(nation_recno, townPtr->nation_recno) )
			return;

		targetObj = townPtr;
	}
	else
		return;

	//----------------------------------------//

	// ##### begin Gilbert 6/11 ######//
	float attackDamage = attenuated_damage(targetObj)
		* attack_attribute.effectiveness(targetObj->defense_attribute);

	if( attackDamage == 0)
		return;

//	if( !unit_array.is_deleted(parent_recno) )
//		targetObj->being_attack_hit(unit_array[parent_recno], attackDamage
//	else
//		targetObj->being_attack_hit(NULL, attackDamage);

	targetObj->being_attack_hit( 
		base_obj_array.is_deleted(parent_base_obj_recno) ? NULL :
		base_obj_array[parent_base_obj_recno], attackDamage  );
	// ##### end Gilbert 6/11 ######//

}
//---------- End of function Bullet::hit_building ----------//


//--------- Begin of function Bullet::check_hit -------//
// check if the bullet hit a target
// return true if hit
int Bullet::check_hit()
{
	err_when(SCAN_RANGE != 5);
	
	short x,y;
	// ##### begin Gilbert 3/11 ######//
	short baseObjHit[SCAN_RANGE*SCAN_RANGE];
	int baseObjHitCount = 0;
	for( int c = 0; c < SCAN_RANGE*SCAN_RANGE; ++c )
	{
		x = target_x_loc + spiral_x[c];
		y = target_y_loc + spiral_y[c];
		if( x >= 0 && x < world.max_x_loc && y >= 0 && y < world.max_y_loc )
		{
			Location *locPtr = world.get_loc(x, y);
			short baseObjRecno = locPtr->base_obj_recno(target_mobile_type);

			if( !baseObjRecno)
				continue;

			for( short *baseObjHitPtr = baseObjHit+baseObjHitCount-1; baseObjHitPtr >= baseObjHit; --baseObjHitPtr )
			{
				if( *baseObjHitPtr == baseObjRecno )
					break;
			}
			if( baseObjHitPtr < baseObjHit )				// not found
			{
				baseObjHit[baseObjHitCount++] = baseObjRecno;
				if( base_obj_array[baseObjRecno]->cast_to_Unit() )
					hit_target(x,y);	// note: no error checking here because mobile_type should be taken into account
				else
					hit_building(x,y);
			}
		}
	}

	return baseObjHitCount;
	// ##### end Gilbert 3/11 ######//
}
//--------- End of function Bullet::check_hit -------//


//--------- Begin of function Bullet::warn_target -------//
//
// warn a unit before hit
// return true if a unit is warned
int Bullet::warn_target()
{
	err_when(SCAN_RANGE != 5);
	
	short x,y;
	int warnCount = 0;

	for( int c = 0; c < SCAN_RANGE*SCAN_RANGE; ++c )
	{
		x = target_x_loc + spiral_x[c];
		y = target_y_loc + spiral_y[c];
		if( x >= 0 && x < world.max_x_loc && y >= 0 && y < world.max_y_loc )
		{
			Location *locPtr = world.get_loc(x, y);
				short unitRecno = locPtr->unit_recno(target_mobile_type);
				if( !unit_array.is_deleted(unitRecno) )
				{
					Unit *unitPtr = unit_array[unitRecno];
					if( attenuated_damage(unitPtr) > 0 )
					{
						if( unitPtr->is_special_ability_effective() == SPAB_REFLECT_ARROW
							&& attack_damage > 1.0f		// reflect will decrease damage, so to avoid infinte reflect
							&& reflect( unitPtr->base_obj_recno ) )
						{
							int effectId = sprite_res.search_sprite( "REFLECT" );
							if( effectId )
								Effect::create(effectId, cur_x, cur_y, SPRITE_DIE, cur_dir & 7, 2, 0); 
							return 0;
						}

						warnCount++;
						switch(unitPtr->cur_action)
						{
						case SPRITE_IDLE:
						case SPRITE_READY_TO_MOVE:
							if( unitPtr->can_stand_guard() && !unitPtr->is_guarding() )
							{
								unitPtr->set_dir( (cur_dir + 4 ) & 7);  // opposite direction of arrow
								unitPtr->set_guard_on();
							}
							break;
						case SPRITE_MOVE:
							if( unitPtr->can_move_guard() && !unitPtr->is_guarding() 
								&& (	(unitPtr->cur_dir & 7)== ((cur_dir + 4 ) & 7)
									||	(unitPtr->cur_dir & 7)== ((cur_dir + 5 ) & 7)
									||	(unitPtr->cur_dir & 7)== ((cur_dir + 3 ) & 7)
									)
								)
							{
								unitPtr->set_guard_on();
							}
							break;
						case SPRITE_ATTACK:
							if( unitPtr->can_attack_guard() && !unitPtr->is_guarding()
								&& unitPtr->remain_attack_delay >= GUARD_COUNT_MAX
								&& (	(unitPtr->cur_dir & 7)== ((cur_dir + 4 ) & 7)
									||	(unitPtr->cur_dir & 7)== ((cur_dir + 5 ) & 7)
									||	(unitPtr->cur_dir & 7)== ((cur_dir + 3 ) & 7)
									)
								)
							{
								unitPtr->set_guard_on();
							}
							break;
						}
					}
				}
			//}
		}
	}

	return warnCount;
}
//--------- End of function Bullet::warn_target -------//


//--------- Begin of function Bullet::display_layer -------//
char Bullet::display_layer()
{
	if( mobile_type == UNIT_AIR || target_mobile_type == UNIT_AIR )
		return 8;
	else
		return 1;
}
//--------- End of function Bullet::display_layer -------//


//------- Begin of function Bullet::attenuated_damage -----//
float	Bullet::attenuated_damage(BaseObj *objPtr)
{
//	short d = m.points_distance(curX, curY, target_x_loc * LOCATE_WIDTH, target_y_loc * LOCATE_HEIGHT);
	short d;

	if( objPtr->cast_to_Unit() )
	{
		register Unit *unitPtr = objPtr->cast_to_Unit();
		d = m.area_distance( 
			unitPtr->cur_x, unitPtr->cur_y, 
			(unitPtr->loc_width-1) * LOCATE_WIDTH + unitPtr->cur_x,		// size of target is 1 loc smaller
			(unitPtr->loc_height-1) * LOCATE_HEIGHT + unitPtr->cur_y,
			target_x_loc * LOCATE_WIDTH, target_y_loc * LOCATE_HEIGHT,		// assume bullet is point size
			target_x_loc * LOCATE_WIDTH, target_y_loc * LOCATE_HEIGHT );
	}
	else
	{
		d = m.area_distance( 
			objPtr->obj_loc_x1() * LOCATE_WIDTH,	// size of target is 1 loc smaller
			objPtr->obj_loc_y1() * LOCATE_HEIGHT,
			objPtr->obj_loc_x2() * LOCATE_WIDTH,
			objPtr->obj_loc_y2() * LOCATE_HEIGHT, 
			target_x_loc * LOCATE_WIDTH,				// assume bullet is point size
			target_y_loc * LOCATE_HEIGHT,
			target_x_loc * LOCATE_WIDTH,
			target_y_loc * LOCATE_HEIGHT );
	}
	
	// damage drops from attack_damage to attack_damage/2, as range drops from 0 to damage_radius
	err_when(damage_radius == 0);
	if( d > damage_radius)
		return (float) 0;
	else if( d > 1 )
		return attack_damage - attack_damage*d/(2*damage_radius);
	else
		return attack_damage;
}
//------- End of function Bullet::attenuated_damage -----//

#ifdef DEBUG

//------- Begin of function BulletArray::operator[] -----//

Bullet* BulletArray::operator[](int recNo)
{
	Bullet* bulletPtr = (Bullet*) get_ptr(recNo);

	if( !bulletPtr )
		err.run( "BulletArray[] is deleted" );

	return bulletPtr;
}

//--------- End of function BulletArray::operator[] ----//

#endif



// ------- Begin of function Bullet::reflect ---------//
//
int Bullet::reflect(int baseObjRecno)
{
	BaseObj *reflecter = base_obj_array[baseObjRecno];

	// set base obj variable

	nation_recno = reflecter->nation_recno;

	// set sprite

	cur_dir ^= 4;		// reverse direction
	final_dir^= 4;
	cur_frame = 1;		// start from beginning
	cur_action = SPRITE_MOVE;

	short oldCurX = cur_x;
	short oldCurY = cur_y;
	short oldGoX = go_x;
	short oldGoY = go_y;
	short	oldOriginX = origin_x;
	short oldOriginY = origin_y;
	short oldTargetXLoc = target_x_loc;
	short oldTargetYLoc = target_y_loc;
	char oldMobileType = mobile_type;
	char oldTargetMobileType = target_mobile_type;
	short oldParentRecno = parent_recno;
	short oldParentBaseObjRecno = parent_base_obj_recno;

	// swap mobile type
	mobile_type = oldTargetMobileType;

	origin_x = cur_x = oldTargetXLoc << ZOOM_X_SHIFT_COUNT;
	origin_y = cur_y = oldTargetYLoc << ZOOM_Y_SHIFT_COUNT;

	SpriteFrame* spriteFrame = cur_sprite_frame();
	int correctAbsX = - spriteFrame->offset_x - spriteFrame->width/2;
	int correctAbsY = - spriteFrame->offset_y - spriteFrame->height/2;
	target_x_loc = oldOriginX >> ZOOM_X_SHIFT_COUNT;
	target_y_loc = oldOriginY >> ZOOM_Y_SHIFT_COUNT;
	go_x = target_x_loc * LOCATE_WIDTH  + LOCATE_WIDTH/2 + ZoomMatrix::calc_cur_x(correctAbsX, correctAbsY);
	go_y = target_y_loc * LOCATE_HEIGHT + LOCATE_HEIGHT/2 + ZoomMatrix::calc_cur_y(correctAbsX, correctAbsY);

	cur_step = 1;
	attack_damage -= 1.0f;
	if( attack_damage < 0.0f )
		attack_damage = 0.0f;

	// nation_recno = reflecter->nation_recno;
	target_mobile_type = oldMobileType;

	// swap z
	short oldZ = z_init;
	z_init = z_dest;
	z_dest = oldZ;

	// set new parent

	parent_base_obj_recno = baseObjRecno;
	parent_recno = reflecter->obj_extern_recno;

	return 1;
}
// ------- End of function Bullet::reflect ---------//
