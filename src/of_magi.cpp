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

// Filename    : OF_MAGI.CPP
// Description : magic tower

#include <of_magi.h>
#include <all.h>
#include <osys.h>
#include <oconfig.h>
#include <oremote.h>
#include <oworld.h>
#include <onation.h> 
#include <otech.h>
#include <otechres.h>
#include <ounit.h>
#include <obullet.h>
#include <onews.h>
#include <ose.h>
#include <ovolume.h>
#include <ow_trv.h>
#include <oeffect.h>

int FirmMagic::teleport_unit_recno;

// -------- define global variable magic_info_array --------//

MagicInfo FirmMagic::magic_info_array[MAGIC_COUNT] = 
{
//   magic id                tech_id/                     life point/cost/delay/effect/range/speed/radius/cost per range/
	{ MAGIC_DESTROY_BUILDING, TECH_MAGIC_DAMAGE_BUILDING,         30,    0,   61,    50,   20,   30,     0,    5, 0  },	// 50% of max hp
// ##### patch begin Gilbert 13/7 ######// 
	{ MAGIC_FREEZE_UNIT,      TECH_MAGIC_FREEZE_UNIT    ,         30,		0,	  30,  FRAMES_PER_DAY*12, // 15,
																														20,   30,     3,    4, 0  },	// 20 frames
// ##### patch end Gilbert 13/7 ######// 
	{ MAGIC_DAMAGE_AREA,      TECH_MAGIC_DAMAGE_AREA    ,        100,		0,	  10,   100,    8,   30,	  3,    6, 0  },	// 20 hit points
	{ MAGIC_TELEPORT,         TECH_MAGIC_TELEPORT       ,         15,		0,	   5,     0,   50,    2,     0,    1, 0  },	// no effect
	{ MAGIC_FREEZE_LASER,		0								 ,				0,	 120,	  50,	  200,	 0,   30,    10,    0, 0  },	// 
	{ MAGIC_GOD_POWER,			0								 ,				0,	 140,	  50,   100,	20,   30,     3,    0, 5  },	// 
	{ MAGIC_LASER,					0								 ,				0,	  70,	  50,  FRAMES_PER_DAY*15,
																														 0,   30,     8,    0, 0  },	// 
};


// ----- begin of function FirmMagic::FirmMagic --------//
//
FirmMagic::FirmMagic()
{
	memset( (char *)this + sizeof(Firm), 0, sizeof(FirmMagic) - sizeof(Firm) );
	animation_offset = 0;
	firm_attack_delay = -1;
	cast_delay_max = MAX_CAST_MAGIC_DELAY;
}	
// ----- end of function FirmMagic::FirmMagic --------//


// ----- begin of function FirmMagic::~FirmMagic --------//
//
FirmMagic::~FirmMagic()
{
}
// ----- end of function FirmMagic::~FirmMagic --------//


//-------- Begin of function FirmMagic::init ------//
//
void FirmMagic::init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno)
{
	Firm::init(xLoc, yLoc, nationRecno, firmId, buildCode, builderRecno);
	base_firm_build_id = firm_build_id;	

	//	use of magic_id_upper_limit and magic_id_lower_limit mainly in of_magia.cpp
	// and this cpp. Although of_magii.cpp also use them, different building always
	// has their own interface cpp file

	switch(firmId)
	{
	case FIRM_MAGIC:
		magic_id_upper_limit = MAGIC_TELEPORT;
		magic_id_lower_limit = MAGIC_DESTROY_BUILDING;
		break;

	case FIRM_OFFENSIVE_BUILDING_1:
		magic_id_upper_limit = MAGIC_GOD_POWER;
		magic_id_lower_limit = MAGIC_GOD_POWER;
		break;

	case FIRM_OFFENSIVE_BUILDING_2:
		magic_id_upper_limit = MAGIC_FREEZE_LASER;
		magic_id_lower_limit = MAGIC_FREEZE_LASER;
		break;

	case FIRM_OFFENSIVE_BUILDING_4:
		magic_id_upper_limit = MAGIC_LASER;
		magic_id_lower_limit = MAGIC_LASER;
		break;

	default:
		err_here();
	}
}
//-------- End of function FirmMagic::init ------//


//--------- Begin of function FirmMagic::deinit --------//
//
void FirmMagic::deinit()
{
	// ##### patch begin Gilbert 1/9 ######//
	if (firm_build_id != base_firm_build_id)
	{
		FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
		firmBuild->free_bitmap_res(); // free before changing firm_build_id
		firm_build_id = base_firm_build_id;
		firmBuild = firm_res.get_build(firm_build_id);
		firmBuild->load_bitmap_res();	// load one again for deinit to free_bitmap_res
	}
	// ##### patch end Gilbert 1/9 ######//
	
	Firm::deinit();
}
//--------- End of function FirmMagic::deinit --------//


//------ begin of function FirmMagic::process_monster_firm -------//
//
void FirmMagic::process_monster_firm()
{
	if (firm_attack_delay > 0)
	{
		firm_attack_delay --;
		return;
	}

	//	restore firm animation and valuables just after attacks //
	if (firm_attack_delay == 0)
	{
		// when no attack the following valuable must = -1 //
		animation_offset = 0;
		firm_attack_delay = -1;
		
		FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
		firmBuild->free_bitmap_res();

		firm_build_id = base_firm_build_id;
		FirmBitmap* firmBitmap;
		firmBuild = firm_res.get_build(firm_build_id);
		// ##### patch begin Gilbert 1/9 ######//
		firmBuild->load_bitmap_res();	// load after changing firm_build_id
		// ##### patch end Gilbert 1/9 ######//

		int bitmapCount;
		int firstBitmap;
		int frameCount = firmBuild->frame_count;
		int aniPartCount = firmBuild->ani_part_count;
		err_when( aniPartCount < 0 );
		err_when( aniPartCount > MAX_FRAME_COUNTERS );
		for (int i = 0; i < aniPartCount ; i++)
		{
			firm_cur_frame[i] = 1;
	
			if( firmBuild->animate_full_size )
				firm_remain_frame_delay[i] = (char) firmBuild->frame_delay(1);
			else
			{
				for ( int k=1; k <frameCount; k++ )
				{
					firstBitmap = firmBuild->first_bitmap(k+1);
					bitmapCount = firmBuild->bitmap_count(k+1);
						for ( int j=0; j <bitmapCount; j++ )
					{
						firmBitmap = firm_res.get_bitmap(firstBitmap + j);
						if( firmBitmap )
						{
							if (firmBitmap->ani_part == (i + 1))
							{
								firm_remain_frame_delay[i] = firmBitmap->delay;
								firm_cur_frame[i] = k+1;
								break;
							}
						}
					}
					if (j < bitmapCount)
						break;
				}
			}
		}
	}
}
//------ end of function FirmMagic::process_monster_firm -------//


// ----- begin of function FirmMagic::process -------//
//
void FirmMagic::process()
{
	if( delayed_magic_count > 0 )
	{
		if( --delayed_magic_count == 0 )
		{
			process_delayed_magic( delayed_magic_id, delayed_magic_x_loc, delayed_magic_y_loc, delayed_magic_unit_recno );
		}
	}
}
// ----- end of function FirmMagic::process -------//


// ----- begin of function FirmMagic::next_day -------//
//
void FirmMagic::next_day()
{
	if( cast_delay < MAX_CAST_MAGIC_DELAY && 
		(GAME_VERSION <= 101 || hit_points >= max_hit_points()) )
	{
		if( config.fast_build && is_own() )
		{
			if( (cast_delay += 10) > MAX_CAST_MAGIC_DELAY )
				cast_delay = MAX_CAST_MAGIC_DELAY;
		}
		else
		{
			++cast_delay;
		}
	}

	Firm::next_day();
}
// ----- end of function FirmMagic::next_day -------//


// ----- begin of function FirmMagic::can_cast -------//
//
int FirmMagic::can_cast(int magicId)
{
	err_when( magicId <= 0 || magicId > MAGIC_COUNT );

	if( nation_array.is_deleted(nation_recno) )
		return 0;

	// ---- check animation finish ---- //

	if( animation_offset == 1 )
		return 0;

	// ---- check delay -------//

	if( cast_delay < MAX_CAST_MAGIC_DELAY )
		return 0;

	// ----- check tech level ------//

	// ######## begin Gilbert 4/5 #########//
	if( magic_info_array[magicId-1].tech_id
		&& tech_res[magic_info_array[magicId-1].tech_id]->get_nation_tech_level(nation_recno) == 0 )
		return 0;
	// ######## end Gilbert 4/5 #########//

	// check nation has minimum life point -------//

	if( nation_ptr()->live_points < magic_info_array[magicId-1].min_live_point )
		return 0;

	// check nation has minimum cash -------//
	if( nation_ptr()->cash < magic_info_array[magicId-1].min_cost )
		return 0;

	return 1;
}
// ----- end of function FirmMagic::can_cast -------//


// ----- begin of function FirmMagic::cast_magic -------//
//
int FirmMagic::cast_magic( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno, char remoteAction )
{
	if( cast_delay < MAX_CAST_MAGIC_DELAY )
		return 0;

	if( targetUnitRecno && unit_array.is_deleted(targetUnitRecno) )
		return 0;

	int livePointCost = calc_live_point_cost(magicId, targetXLoc, targetYLoc, targetUnitRecno);
	int cashCost = calc_cash_cost(magicId, targetXLoc, targetYLoc, targetUnitRecno);

	if( nation_ptr()->live_points < livePointCost )
		return 0;

	if( nation_ptr()->cash < cashCost )
		return 0;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_F_MAGIC_CAST, 5*sizeof(short) );
		// packet struct : <firm recno> <magic Id> <xLoc> <yLoc> <unit recno>
		shortPtr[0] = firm_recno;
		shortPtr[1] = magicId;
		shortPtr[2] = targetXLoc;
		shortPtr[3] = targetYLoc;
		shortPtr[4] = targetUnitRecno;	
		return 1;
	}

	int rc = 0;
	MagicInfo *magicInfo = &magic_info_array[magicId-1];

	casting_magic_id = magicId;

	switch( magicId )
	{
	case MAGIC_DESTROY_BUILDING:
		{
			// get target firm

			Location *locPtr = world.get_loc(targetXLoc, targetYLoc);
			if( locPtr->is_firm() )
			{
				Firm *firmPtr = firm_array[locPtr->firm_recno()];
				if( nation_array.should_attack(nation_recno, firmPtr->nation_recno) )
				{
					int bulletId = bullet_array.add_bullet(this, firmPtr);

					// sound effect
					RelVolume relVolume(PosVolume(center_x, center_y));
					if( relVolume.rel_vol < 90)
						relVolume.rel_vol = 90;
					se_ctrl.request("BUILDUP", relVolume);

					// set delayed attack

					delayed_magic_count = 15;
					delayed_magic_id = magicId;
					delayed_magic_x_loc = targetXLoc;
					delayed_magic_y_loc = targetYLoc;
					delayed_magic_unit_recno = targetUnitRecno;

					// send news message 

					if( !is_own() )		// inform all other player
					{
						news_array.magic_tower_attack(base_obj_recno, magicId, firmPtr->base_obj_recno, firmPtr->center_x, firmPtr->center_y );
					}

					rc = 1;
				}
			}
			else if( locPtr->is_town() )
			{
				Town *townPtr = town_array[locPtr->town_recno()];
				if( nation_array.should_attack(nation_recno, townPtr->nation_recno) )
				{
					int bulletId = bullet_array.add_bullet(this, townPtr);

					// sound effect
					RelVolume relVolume(PosVolume(center_x, center_y));
					if( relVolume.rel_vol < 90)
						relVolume.rel_vol = 90;
					se_ctrl.request("BUILDUP", relVolume);

					// set delayed attack
					delayed_magic_count = 15;
					delayed_magic_id = magicId;
					delayed_magic_x_loc = targetXLoc;
					delayed_magic_y_loc = targetYLoc;
					delayed_magic_unit_recno = targetUnitRecno;

					// send news message 

					if( !is_own() )		// inform all other player
					{
						news_array.magic_tower_attack(base_obj_recno, magicId, townPtr->base_obj_recno, townPtr->center_x, townPtr->center_y );
					}

					rc = 1;
				}
			}
		}
		break;

	case MAGIC_FREEZE_UNIT:
		{
			WorldScanTraverser scanner;

			// freeze each unit

			scanner.init( targetXLoc, targetYLoc, 1, magicInfo->cast_radius );
			for( scanner.restart(); !scanner.is_finish(); scanner.next() )
			{
				Unit *unitPtr = unit_array[scanner.get_unit()];
				if( unitPtr->hit_points <= 0.0f )
					continue;
				if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
					unitPtr->freeze(magicInfo->effect);
			}
			rc = 1;
		}
		break;

	case MAGIC_DAMAGE_AREA:
		{
			enum { ATTACK_DELAY = 10 };

			//- add animation for attack -//
			animation_offset = 1;
			firm_attack_delay	= ATTACK_DELAY;
			FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
			firmBuild->free_bitmap_res();	// free before changing firm_build_id
	
			firm_build_id = base_firm_build_id + animation_offset;

			//--------- set animation firm animation for attack vars ---------//
			firmBuild = firm_res.get_build(firm_build_id);
			firmBuild->load_bitmap_res();
			FirmBitmap* firmBitmap;
			int bitmapCount;
			int firstBitmap;
			int frameCount = firmBuild->frame_count;
			int aniPartCount = firmBuild->ani_part_count;
			err_when( aniPartCount < 0 );
			err_when( aniPartCount > MAX_FRAME_COUNTERS );
				
			for (int i = 0; i < aniPartCount ; i++)
			{
				firm_cur_frame[i] = 1;
		
				if( firmBuild->animate_full_size )
					firm_remain_frame_delay[i] = (char) firmBuild->frame_delay(1);
				else
				{
					for ( int k=1; k <frameCount; k++ )
					{
						firstBitmap = firmBuild->first_bitmap(k+1);
						bitmapCount = firmBuild->bitmap_count(k+1);
	
						for ( int j=0; j <bitmapCount; j++ )
						{
							firmBitmap = firm_res.get_bitmap(firstBitmap + j);
							if( firmBitmap )
							{
								if (firmBitmap->ani_part == (i + 1))
								{
									firm_remain_frame_delay[i] = firmBitmap->delay;
									firm_cur_frame[i] = k+1;
									break;
								}
							}
						}
						if (j < bitmapCount)
							break;
					}
				}
			}

			WorldScanTraverser scanner;

			// add one bullet for each unit inside 

			scanner.init( targetXLoc, targetYLoc, 1, magicInfo->cast_radius );
			for( scanner.restart(); !scanner.is_finish(); scanner.next() )
			{
				Unit *unitPtr = unit_array[scanner.get_unit()];
				if( unitPtr->hit_points <= 0.0f )
					continue;
				if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
					bullet_array.add_bullet(this, unitPtr );
				// sound effect
				RelVolume relVolume(PosVolume(center_x, center_y));
				if( relVolume.rel_vol < 90)
					relVolume.rel_vol = 90;
				se_ctrl.request("BUILDUP", relVolume);
			}
				
			rc = 1;
		}
		break;

	case MAGIC_TELEPORT:
		{
			// ####### begin Gilbert 6/3 ########//
			if( unit_array[targetUnitRecno]->teleport( targetXLoc, targetYLoc) )
			{
				rc = 1;
			}
			// ####### end Gilbert 6/3 ########//
		}
		break;

	case MAGIC_FREEZE_LASER:
		{
			delayed_magic_count = 15;
			delayed_magic_id = magicId;
			delayed_magic_x_loc = targetXLoc;
			delayed_magic_y_loc = targetYLoc;
			delayed_magic_unit_recno = targetUnitRecno;
			rc = 1;
			bullet_array.add_bullet(this, this);
			// sound effect
			RelVolume relVolume(PosVolume(center_x, center_y));
			if( relVolume.rel_vol < 90)
				relVolume.rel_vol = 90;
			se_ctrl.request("BUILDUP", relVolume);

		}
		break;

	case MAGIC_GOD_POWER:
		{
			delayed_magic_count = 16;
			delayed_magic_id = magicId;
			delayed_magic_x_loc = targetXLoc;
			delayed_magic_y_loc = targetYLoc;
			delayed_magic_unit_recno = targetUnitRecno;
			rc = 1;
			bullet_array.add_bullet(this, this);
			delay_again_flag = 1;
			// sound effect
			RelVolume relVolume(PosVolume(center_x, center_y));
			if( relVolume.rel_vol < 90)
				relVolume.rel_vol = 90;
			se_ctrl.request("BUILDUP", relVolume);
		}
		break;			

	case MAGIC_LASER:
		{
			FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
			int aniPartCount = firmBuild->ani_part_count;
			for (int i = 0; i < aniPartCount ; i++)
				firm_cur_frame[i] = 1;
			delayed_magic_count = 16;
			delayed_magic_id = magicId;
			delayed_magic_x_loc = targetXLoc;
			delayed_magic_y_loc = targetYLoc;
			delayed_magic_unit_recno = targetUnitRecno;
			rc = 1;
			bullet_array.add_bullet(this, this);
			delay_again_flag = 1;
			// sound effect
			RelVolume relVolume(PosVolume(center_x, center_y));
			if( relVolume.rel_vol < 90)
				relVolume.rel_vol = 90;
			se_ctrl.request("BUILDUP", relVolume);
		}
		break;			

	default:
		err_here();
	}

	casting_magic_id = 0;

	if( rc )		// cast magic success
	{
		// ------ reduce live point ------//

		nation_ptr()->change_live_points(float(-livePointCost));
		nation_ptr()->add_expense( EXPENSE_WEAPON, (float)cashCost, 1 );
		
		// ------ set delay -------//

//		err_when( cast_delay > 0 );
//		cast_delay = cast_delay_max = magicInfo->cast_delay;
		err_when( cast_delay < MAX_CAST_MAGIC_DELAY );
		cast_delay -= magicInfo->cast_delay;
		err_when( cast_delay < 0 );
	}

	return rc;
}
// ----- end of function FirmMagic::cast_magic -------//


// ----- begin of function FirmMagic::process_delayed_magic -------//
//
int FirmMagic::process_delayed_magic(int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno )
{
	int rc = 0;

	MagicInfo *magicInfo = &magic_info_array[magicId-1];

	casting_magic_id = magicId;
	switch( magicId )
	{
	case MAGIC_DESTROY_BUILDING:
		{
			// get target firm

			Location *locPtr = world.get_loc(targetXLoc, targetYLoc);
			if( locPtr->is_firm() )
			{
				Firm *firmPtr = firm_array[locPtr->firm_recno()];
				if( nation_array.should_attack( nation_recno, firmPtr->nation_recno) )
				{
					firmPtr->hit_points  -= float(firmPtr->max_hit_points() * magicInfo->effect / 100);
					if( firmPtr->hit_points < 0.0f )
						firmPtr->hit_points = 0.0f; 
						// 0.01f to pass the alreadyKilled checking in BaseObj::being_attack_hit
					firmPtr->being_attack_hit( this, 0.0f );		// trigger being attack
					rc = 1;
				}
			}
			else if( locPtr->is_town() )
			{
				Town *townPtr = town_array[locPtr->town_recno()];
				if( nation_array.should_attack( nation_recno, townPtr->nation_recno) )
				{
					int effectCent = magicInfo->effect;
					float effectHit = (float) effectCent * townPtr->max_hit_points() / 100;

					if( townPtr->hit_points == 0.0f )
					{
						// no change
					}
					else if( townPtr->hit_points >= effectHit )		// wall is stronger than damage
					{
						err_when( townPtr->max_hit_points == 0 );	// notice townPtr->hit_points may be 0 when no wall

						townPtr->hit_points  -= effectHit;
						effectCent = 0;				// people are safe
					}
					else if( townPtr->hit_points > 0.0f )		// wall is not as strong, but still can reduce the damage
					{
						err_when( effectHit == 0 );		// divide by zero

						effectCent -= int(townPtr->hit_points * effectCent / effectHit);
						townPtr->hit_points = 0.0f;
						// 0.01f to pass the alreadyKilled checking in BaseObj::being_attack_hit
					}

					townPtr->being_attack_hit( this, 0.0f );		// trigger being attack

					// kill people here

					int peopleKill = townPtr->population * effectCent / 100;
					for( ; townPtr->population > 0 && peopleKill > 0; --peopleKill )
					{
						townPtr->kill_town_people(0);
					}

					rc = 1;
				}
			}
		}
		break;

	case MAGIC_LASER:
		{
			int x1 = targetXLoc - magicInfo->cast_radius;
			if( x1 < 0 )
				x1 = 0;
			int x2 = targetXLoc + magicInfo->cast_radius;
			if( x2 >= MAX_WORLD_X_LOC )
				x2 = MAX_WORLD_X_LOC-1;
			int y1 = targetYLoc - magicInfo->cast_radius;
			if( y1 < 0 )
				y1 = 0;
			int y2 = targetYLoc + magicInfo->cast_radius;
			if( y2 >= MAX_WORLD_Y_LOC )
				y2 = MAX_WORLD_Y_LOC-1;

			for( int y = y1; y <= y2; ++y )
			{
				for( int x = x1; x <= x2; ++x )
				{
					Location *locPtr = world.get_loc(x, y);
					if( locPtr->unit_recno(UNIT_LAND) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_LAND)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
						//	unitPtr->hit_points  -= magicInfo->effect;
							unitPtr->being_attack_hit( this, (float)magicInfo->effect );		// trigger being attack
		
							if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
								locPtr->set_fire_str(30);
							if( locPtr->fire_src() > 0 )
								locPtr->set_fire_src(1);		// such that the fire will be put out quickly
		
							// sound effect
								RelVolume relVolume(PosVolume(x, y));
								if( relVolume.rel_vol < 100)
									relVolume.rel_vol = 100;
								se_ctrl.request("OFF-4", relVolume);
							}
					}
					if( locPtr->unit_recno(UNIT_SEA) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_SEA)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
						//	unitPtr->hit_points  -= magicInfo->effect;
							unitPtr->being_attack_hit( this, (float)magicInfo->effect );		// trigger being attack
			
							if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
								locPtr->set_fire_str(30);
							if( locPtr->fire_src() > 0 )
								locPtr->set_fire_src(1);		// such that the fire will be put out quickly
		
							// sound effect
							RelVolume relVolume(PosVolume(x, y));
							if( relVolume.rel_vol < 100)
								relVolume.rel_vol = 100;
							se_ctrl.request("OFF-4", relVolume);
						}
					}
					if( locPtr->unit_recno(UNIT_AIR) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_AIR)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
						//	unitPtr->hit_points  -= magicInfo->effect;
							unitPtr->being_attack_hit( this, (float)magicInfo->effect );		// trigger being attack
					
							if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
								locPtr->set_fire_str(30);
							if( locPtr->fire_src() > 0 )
								locPtr->set_fire_src(1);		// such that the fire will be put out quickly
		
							// sound effect
							RelVolume relVolume(PosVolume(x, y));
							if( relVolume.rel_vol < 100)
								relVolume.rel_vol = 100;
							se_ctrl.request("OFF-4", relVolume);
						}
					}
				}
			}
		}
		break;

	case MAGIC_GOD_POWER:
		{
			int x1 = targetXLoc - magicInfo->cast_radius;
			if( x1 < 0 )
				x1 = 0;
			int x2 = targetXLoc + magicInfo->cast_radius;
			if( x2 >= MAX_WORLD_X_LOC )
				x2 = MAX_WORLD_X_LOC-1;
			int y1 = targetYLoc - magicInfo->cast_radius;
			if( y1 < 0 )
				y1 = 0;
			int y2 = targetYLoc + magicInfo->cast_radius;
			if( y2 >= MAX_WORLD_Y_LOC )
				y2 = MAX_WORLD_Y_LOC-1;
		//	int bulletId = 0;
			
			for( int y = y1; y <= y2; ++y )
			{
				for( int x = x1; x <= x2; ++x )
				{
					Location *locPtr = world.get_loc(x, y);
					if( locPtr->unit_recno(UNIT_LAND) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_LAND)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
							if (delay_again_flag == 1)
							{
								delayed_magic_count = 10;
								delayed_magic_id = magicId;
								delayed_magic_x_loc = targetXLoc;
								delayed_magic_y_loc = targetYLoc;
								delayed_magic_unit_recno = targetUnitRecno;
								bullet_array.add_bullet(this, unitPtr);
								delay_again_flag = 0;
								y = y2; x = x2;
								break;
							}
							else
							{
							//	unitPtr->hit_points  -= magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width;
								unitPtr->being_attack_hit( this, (float)magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width );		// trigger being attack
							
								if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
									locPtr->set_fire_str(30);
								if( locPtr->fire_src() > 0 )
									locPtr->set_fire_src(1);		// such that the fire will be put out quickly
								// sound effect
								RelVolume relVolume(PosVolume(x, y));
								if( relVolume.rel_vol < 100)
									relVolume.rel_vol = 100;
								se_ctrl.request("OFF-1", relVolume);
							}
						}
					}
					if( locPtr->unit_recno(UNIT_SEA) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_SEA)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
							if (delay_again_flag == 1)
							{
								delayed_magic_count = 10;
								delayed_magic_id = magicId;
								delayed_magic_x_loc = targetXLoc;
								delayed_magic_y_loc = targetYLoc;
								delayed_magic_unit_recno = targetUnitRecno;
								bullet_array.add_bullet(this, unitPtr);
								delay_again_flag = 0;
								y = y2; x = x2;
								break;
							}
							else
							{
							//	unitPtr->hit_points  -= magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width;
								unitPtr->being_attack_hit( this, (float)magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width );		// trigger being attack
				
								if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
									locPtr->set_fire_str(30);
								if( locPtr->fire_src() > 0 )
									locPtr->set_fire_src(1);		// such that the fire will be put out quickly
								// sound effect
								RelVolume relVolume(PosVolume(x, y));
								if( relVolume.rel_vol < 100)
									relVolume.rel_vol = 100;
								se_ctrl.request("OFF-1", relVolume);
							}
						}
					}
					if( locPtr->unit_recno(UNIT_AIR) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_AIR)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
						{
							if (delay_again_flag == 1)
							{
								delayed_magic_count = 10;
								delayed_magic_id = magicId;
								delayed_magic_x_loc = targetXLoc;
								delayed_magic_y_loc = targetYLoc;
								delayed_magic_unit_recno = targetUnitRecno;
								bullet_array.add_bullet(this, unitPtr);
								delay_again_flag = 0;
								y = y2; x = x2;
								break;
							}
							else
							{
							//	unitPtr->hit_points  -= magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width;
								unitPtr->being_attack_hit( this, (float)magicInfo->effect/unitPtr->loc_width/unitPtr->loc_width );		// trigger being attack
		
								if( locPtr->can_set_fire() && locPtr->fire_str() < 30 )
									locPtr->set_fire_str(30);
								if( locPtr->fire_src() > 0 )
									locPtr->set_fire_src(1);		// such that the fire will be put out quickly
								// sound effect
								RelVolume relVolume(PosVolume(x, y));
								if( relVolume.rel_vol < 100)
									relVolume.rel_vol = 100;
								se_ctrl.request("OFF-1", relVolume);
							}
						}
					}
					if( locPtr->is_firm() )
					{
						Firm *firmPtr = firm_array[locPtr->firm_recno()];
						if( nation_array.should_attack( nation_recno, firmPtr->nation_recno) )
						{
							if (delay_again_flag == 1)
							{
								delayed_magic_count = 10;
								delayed_magic_id = magicId;
								delayed_magic_x_loc = targetXLoc;
								delayed_magic_y_loc = targetYLoc;
								delayed_magic_unit_recno = targetUnitRecno;
								bullet_array.add_bullet(this, firmPtr);
								delay_again_flag = 0;
								y = y2; x = x2;
								break;
							}
							else
							{
								// sound effect
								RelVolume relVolume(PosVolume(firmPtr->center_x, firmPtr->center_y));
								if( relVolume.rel_vol < 100)
									relVolume.rel_vol = 100;
								se_ctrl.request("OFF-1", relVolume);
								firmPtr->hit_points  -= (magicInfo->effect<<1)/firmPtr->loc_width()/firmPtr->loc_width();
								firmPtr->being_attack_hit( this, 0.0f );		// trigger being attack
							}
						}
					}
				}
			}
		}
		break;

	case MAGIC_FREEZE_LASER:
		{	
			int x1 = targetXLoc - magicInfo->cast_radius;
			if( x1 < 0 )
				x1 = 0;
			int x2 = targetXLoc + magicInfo->cast_radius;
			if( x2 >= MAX_WORLD_X_LOC )
				x2 = MAX_WORLD_X_LOC-1;
			int y1 = targetYLoc - magicInfo->cast_radius;
			if( y1 < 0 )
				y1 = 0;
			int y2 = targetYLoc + magicInfo->cast_radius;
			if( y2 >= MAX_WORLD_Y_LOC )
				y2 = MAX_WORLD_Y_LOC-1;

			for( int y = y1; y <= y2; ++y )
			{
				for( int x = x1; x <= x2; ++x )
				{
					Location *locPtr = world.get_loc(x, y);
					if( locPtr->unit_recno(UNIT_LAND) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_LAND)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
							unitPtr->freeze(magicInfo->effect);
						// sound effect
						RelVolume relVolume(PosVolume(x, y));
						if( relVolume.rel_vol < 100)
							relVolume.rel_vol = 100;
						se_ctrl.request("OFF-2", relVolume);
					}
					if( locPtr->unit_recno(UNIT_SEA) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_SEA)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
							unitPtr->freeze(magicInfo->effect);
						// sound effect
						RelVolume relVolume(PosVolume(x, y));
						if( relVolume.rel_vol < 100)
							relVolume.rel_vol = 100;
						se_ctrl.request("OFF-2", relVolume);
					}
					if( locPtr->unit_recno(UNIT_AIR) )
					{
						Unit *unitPtr = unit_array[locPtr->unit_recno(UNIT_AIR)];
						if( nation_array.should_attack( nation_recno, unitPtr->nation_recno ) )
							unitPtr->freeze(magicInfo->effect);
						// sound effect
						RelVolume relVolume(PosVolume(x, y));
						if( relVolume.rel_vol < 100)
							relVolume.rel_vol = 100;
						se_ctrl.request("OFF-2", relVolume);
					}
				}
			}
		}
		break;
	
	default:
		err_here();
	}
	casting_magic_id = 0;
	return rc;
}
// ----- end of function FirmMagic::process_delayed_magic -------//


// ----- begin of function FirmMagic::calc_live_point_cost -------//
//
int FirmMagic::calc_live_point_cost( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno )
{
	err_when( magicId <= 0 || magicId > MAGIC_COUNT );

	MagicInfo *magicInfo = &magic_info_array[magicId-1];

	int cost = magicInfo->min_live_point;

	int dist1 = m.area_distance( loc_x1, loc_y1, loc_x2, loc_y2, 
		targetXLoc, targetYLoc, targetXLoc, targetYLoc );
	// ####### begin Gilbert 21/3 #######//
	int dist2 = 0;
	if( targetUnitRecno && !unit_array.is_deleted(targetUnitRecno)
		&& unit_array[targetUnitRecno]->is_visible() )
	{
		dist2 = area_distance(unit_array[targetUnitRecno]);
	}
	// ####### end Gilbert 21/3 #######//

	if( dist1 > magicInfo->cast_range )
		cost += magicInfo->live_point_extra_range * (dist1 - magicInfo->cast_range);
	if( dist2 > magicInfo->cast_range )
		cost += magicInfo->live_point_extra_range * (dist2 - magicInfo->cast_range);

	return cost;
}
// ----- end of function FirmMagic::calc_live_point_cost -------//


// ----- begin of function FirmMagic::calc_cash_cost -------//
//
int FirmMagic::calc_cash_cost( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno )
{
	err_when( magicId <= 0 || magicId > MAGIC_COUNT );

	MagicInfo *magicInfo = &magic_info_array[magicId-1];

	int cost = magicInfo->min_cost;

	int dist1 = m.area_distance( loc_x1, loc_y1, loc_x2, loc_y2, 
		targetXLoc, targetYLoc, targetXLoc, targetYLoc );
	// ####### begin Gilbert 21/3 #######//
	int dist2 = 0;
	if( targetUnitRecno && !unit_array.is_deleted(targetUnitRecno)
		&& unit_array[targetUnitRecno]->is_visible() )
	{
		dist2 = area_distance(unit_array[targetUnitRecno]);
	}
	// ####### end Gilbert 21/3 #######//

	if( dist1 > magicInfo->cast_range )
		cost += magicInfo->cost_extra_range * (dist1 - magicInfo->cast_range);
	if( dist2 > magicInfo->cast_range )
		cost += magicInfo->cost_extra_range * (dist2 - magicInfo->cast_range);

	return cost;
}
// ----- end of function FirmMagic::calc_cash_cost -------//


// ----- begin of function FirmMagic::bullet_damage -------//
//
float FirmMagic::bullet_damage()
{
	switch( casting_magic_id )
	{
	case MAGIC_DESTROY_BUILDING:
	case MAGIC_DAMAGE_AREA:
	case MAGIC_FREEZE_LASER:
	case MAGIC_GOD_POWER:		
	case MAGIC_LASER:
		return (float) magic_info_array[casting_magic_id-1].effect;

	case MAGIC_FREEZE_UNIT:
		return 0;

	case MAGIC_TELEPORT:
	default:
		err_here();
		return 0.0f;
	}
}
// ----- end of function FirmMagic::bullet_damage -------//


// ----- begin of function FirmMagic::bullet_radius -------//
//
short FirmMagic::bullet_radius()
{
	switch( casting_magic_id )
	{
	case MAGIC_DESTROY_BUILDING:
	case MAGIC_DAMAGE_AREA:
	case MAGIC_GOD_POWER:	
	case MAGIC_FREEZE_LASER:
	case MAGIC_LASER:
		return LOCATE_WIDTH-2;

	case MAGIC_FREEZE_UNIT:
		return 0;

	case MAGIC_TELEPORT:
	default:
		err_here();
		return 0;
	}
}
// ----- begin of function FirmMagic::bullet_radius -------//


// ----- begin of function FirmMagic::bullet_fire -------//
//
char FirmMagic::bullet_fire()
{
	switch( casting_magic_id )
	{
	case MAGIC_DESTROY_BUILDING:
	case MAGIC_DAMAGE_AREA:
	case MAGIC_FREEZE_UNIT:
	case MAGIC_FREEZE_LASER:
	case MAGIC_GOD_POWER:
	case MAGIC_LASER:
		return 0;

	case MAGIC_TELEPORT:
	default:
		err_here();
		return 0;
	}
}
// ----- end of function FirmMagic::bullet_fire -------//


// ----- begin of function FirmMagic::bullet_id -------//
//
short FirmMagic::bullet_id()
{
	static spriteId;
	switch( casting_magic_id )
	{
	case MAGIC_DESTROY_BUILDING:
	case MAGIC_DAMAGE_AREA:
	case MAGIC_FREEZE_UNIT:
		spriteId = sprite_res.search_sprite( "MAGIC" );
		err_when( !spriteId );
		return spriteId;

	case MAGIC_FREEZE_LASER:
		spriteId = sprite_res.search_sprite( "MAGIC2" );
		err_when( !spriteId );
		return spriteId;

	case MAGIC_GOD_POWER:
		if (delayed_magic_count > 10)
			spriteId = sprite_res.search_sprite( "MAGIC3" );
		else
			spriteId = sprite_res.search_sprite( "MAGIC4" );
		err_when( !spriteId );
		return spriteId;

	case MAGIC_LASER:
		spriteId = sprite_res.search_sprite( "MAGIC5" );
		err_when( !spriteId );
		return spriteId;

	case MAGIC_TELEPORT:
	default:
		err_here();
		return 0;
	}
}
// ----- end of function FirmMagic::bullet_id -------//


// ----- begin of function FirmMagic::bullet_speed -------//
//
short FirmMagic::bullet_speed()
{
	switch( casting_magic_id )
	{
	case MAGIC_DESTROY_BUILDING:
	case MAGIC_DAMAGE_AREA:
	case MAGIC_FREEZE_UNIT:
	case MAGIC_FREEZE_LASER:
	case MAGIC_GOD_POWER:
	case MAGIC_LASER:
		return magic_info_array[casting_magic_id-1].bullet_speed;

	case MAGIC_TELEPORT:
	default:
		err_here();
		return 1;
	}
}
// ----- end of function FirmMagic::bullet_speed -------//


// ----- begin of function FirmMagic::bullet_init_z -------//
//
short FirmMagic::bullet_init_z()
{
	return altitude + 32;
}
// ----- end of function FirmMagic::bullet_init_z -------//


// ----- begin of function FirmMagic::transfer_energy -------//
//
// transfer energy from this firm to targetFirmRecno
//
int FirmMagic::transfer_energy( int targetFirmRecno, char remoteAction )
{
	FirmMagic *targetFirm;

	if( firm_array.is_deleted(targetFirmRecno) 
		|| !(targetFirm=firm_array[targetFirmRecno]->cast_to_FirmMagic())
		|| targetFirm->under_construction
		|| nation_recno != targetFirm->nation_recno		// same nation
		|| firm_recno == targetFirmRecno )					// not the same firm
		return 0;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_F_MAGIC_TRANSFER, 2*sizeof(short) );
		// packet struct : <firm recno> <target firm recno>
		shortPtr[0] = firm_recno;
		shortPtr[1] = targetFirmRecno;
		return 1;
	}

	// calc the required energy

	float efficiency = calc_transfer_efficiency( targetFirmRecno );
	if( efficiency < 1 / MAX_CAST_MAGIC_DELAY )		// use all energy cannot increase 1 pt of energy
		return 1;

	if( targetFirm->cast_delay >= MAX_CAST_MAGIC_DELAY )		// target is already full
		return 1;

	int delayDec = int( (MAX_CAST_MAGIC_DELAY-targetFirm->cast_delay) / efficiency);
	if( cast_delay >= delayDec )
	{
		// sufficient case
		targetFirm->cast_delay = MAX_CAST_MAGIC_DELAY;	// don't use delayDec * efficiency, avoid round off error
		cast_delay -= delayDec;
	}
	else
	{
		// insufficient case
		delayDec = cast_delay;
		targetFirm->cast_delay += int(delayDec * efficiency);	// don't use delayDec * efficiency, avoid round off error
		cast_delay = 0;
	}

	// add effect
	static int transferEffectId = sprite_res.search_sprite("AN_EFF1");
	Effect::create(transferEffectId, targetFirm->center_x*LOCATE_WIDTH, targetFirm->center_y*LOCATE_HEIGHT, SPRITE_IDLE, 1, 8, 0);

	// sound effect
	if( is_own() )
		se_ctrl.request("CE-B1-5", DEF_REL_VOLUME);

	return 1;
}
// ----- end of function FirmMagic::transfer_energy -------//


// ----- begin of function FirmMagic::calc_transfer_efficieny -------//
//
float	FirmMagic::calc_transfer_efficiency( int targetFirmRecno )
{
	err_when( firm_array.is_deleted(targetFirmRecno) );
	// drop from 0.75 to 0.25
	int dist = area_distance( firm_array[targetFirmRecno] );
	float eff = 0.75f - 0.01f * dist;
	if( eff < 0.25f )
		eff = 0.25f;
	return eff;
}
// ----- end of function FirmMagic::calc_transfer_efficieny -------//
