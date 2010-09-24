// Filename    : OU_CART.CPP
// Description : Explosive Cart

// #include <windows.h>
#include <OU_CART.H>
#include <math.h>

// --------- define constant --------//

#define EXPLODE_RANGE 1
#define EXPLODE_DAMAGE (50 * ATTACK_SLOW_DOWN)
#define CHAIN_TRIGGER_RANGE 2

static AttackAttribute cart_attack_attribute = { 50, 100, 100 };

UnitExpCart::UnitExpCart() : Unit()
{
	triggered = 0;
}

UnitExpCart::~UnitExpCart()
{
}


int UnitExpCart::process_die()
{
	if(triggered && cur_frame == 3)
	{
		short x, y;
		short x1 = next_x_loc();
		short x2 = x1;
		short y1 = next_y_loc();
		short y2 = y1;
		x1 -= CHAIN_TRIGGER_RANGE;
		x2 += CHAIN_TRIGGER_RANGE;
		y1 -= CHAIN_TRIGGER_RANGE;
		y2 += CHAIN_TRIGGER_RANGE;
		if(x1 < 0)
			x1 = 0;
		if(x2 >= world.max_x_loc)
			x2 = world.max_x_loc-1;
		if(y1 < 0)
			y1 = 0;
		if(y2 >= world.max_y_loc)
			y2 = world.max_y_loc-1;

		for( y = y1; y <= y2; ++y)
		{
			for( x = x1; x <= x2; ++x)
			{
				Location *locPtr = world.get_loc(x,y);
				if( locPtr->unit_recno(UNIT_LAND) )
				{
					Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_LAND)];
					if( unitPtr->unit_id == UNIT_EXPLOSIVE_CART )
						((UnitExpCart *)unitPtr)->trigger_explode();
				}
			}
		}
	}

	if(triggered && (cur_frame == 3 || cur_frame == 7) )
	{
		short x, y;
		short x1 = next_x_loc();
		short x2 = x1;
		short y1 = next_y_loc();
		short y2 = y1;
		x1 -= EXPLODE_RANGE;
		x2 += EXPLODE_RANGE;
		y1 -= EXPLODE_RANGE;
		y2 += EXPLODE_RANGE;

		if(x1 < 0)
			x1 = 0;
		if(x2 >= world.max_x_loc)
			x2 = world.max_x_loc-1;
		if(y1 < 0)
			y1 = 0;
		if(y2 >= world.max_y_loc)
			y2 = world.max_y_loc-1;

		if( cur_frame == 3)
		{
			for( y = y1; y <= y2; ++y)
			{
				for( x = x1; x <= x2; ++x)
				{
					Location *locPtr = world.get_loc(x,y);


					if( locPtr->is_plant() )
					{
						locPtr->remove_plant();
						world.plant_count--;
						continue;
					}

					// ###### begin Gilbert 3/6 #####//
					if( locPtr->is_rock() )
					{
						world.remove_rock(locPtr->rock_array_recno());
					}

//					if( locPtr->has_dirt() )	// avoid bomb dirt
//					{
//						world.remove_dirt(locPtr->dirt_recno());
//					}
					// ###### end Gilbert 3/6 #####//

					int baseObjRecno = locPtr->base_obj_recno();

					if( baseObjRecno )
					{
						BaseObj* baseObj = base_obj_array[baseObjRecno];

						//--- air units are not affected by the explosion ---//

						if( baseObj->obj_mobile_type() == UNIT_AIR )
							continue;

						// ##### begin Gilbert 6/11 ######//
						float explodeDamage = (float) EXPLODE_DAMAGE;

						if( unit_res[unit_id]->class_info.has_weapon_version
							&& get_weapon_version() > 2 )
						{
							// at level 3, 1.5 times damage
							explodeDamage *= (get_weapon_version()) * 0.5f;
						}

						baseObj->being_attack_hit(this, (float) EXPLODE_DAMAGE * 
							cart_attack_attribute.effectiveness(baseObj->defense_attribute) );
						// ##### end Gilbert 6/11 ######//
					}
				}
			}
		}
		else if(cur_frame == 7)
		{
			for( y = y1; y <= y2; ++y)
			{
				for( x = x1; x <= x2; ++x)
				{
					Location *locPtr = world.get_loc(x,y);

					int fl = (abs(x - next_x_loc()) + abs(y - next_y_loc())) * -30 + 80;
					if( locPtr->can_set_fire() && locPtr->fire_str() < fl )
						locPtr->set_fire_str(fl);
					if( locPtr->fire_src() > 0 )
						locPtr->set_fire_src(1);		// such that the fire will be put out quickly
				}
			}
		}
	}

	return Unit::process_die();
}


void UnitExpCart::trigger_explode()
{
	if( hit_points > 0)	// so dying cart cannot be triggered
	{
		triggered = 1;
		hit_points = (float) 0;
	}
}


//----------- Begin of function UnitExpCart::is_camouflage ----------//
//
// pass viewerNation -1 to test if camouflage is on/off
//
int UnitExpCart::is_camouflage(int viewerNation)
{
	// has no invisibility from level 2
	if( cur_action != SPRITE_DIE && !is_nation(viewerNation) && get_weapon_version() > 1 )
	{
		return 1;
	}

	return 0;
}
//----------- End of function UnitExpCart::is_camouflage ----------//
