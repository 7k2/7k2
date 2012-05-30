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

//Filename   : OF_OFF2.CPP
//Description: Class of FirmOffensive2
// it auti attack nearby land units only


#include <of_off2.h>
#include <all.h>
#include <osys.h>
#include <oremote.h>
#include <onation.h>
#include <ounitres.h> 
#include <obullet.h>
#include <ospy.h>
#include <oplant.h>
#include <olocate.h>
#include <oworld.h>
#include <otech.h>
#include <otechres.h>
#include <oterrain.h>
#include <oworldmt.h>
#include <omonsres.h>
#include <oraceres.h>
#include <oweather.h>
#include <math.h>
#include <oconfig.h>
#include <oseres.h>

enum { SCAN_X_RANGE = 8,
		 SCAN_Y_RANGE = 8,
		 ATTACK_X_RANGE = 10,
		 ATTACK_Y_RANGE = 10,
		 ATTACK_ANIMATION_DELAY = 12,
		 ATTACK_DELAY = 10 };

//-------- Begin of function FirmOffensive2::FirmOffensive2() ------//
//
FirmOffensive2::FirmOffensive2()
{
	memset( sizeof(Firm)+ (char *)this, 0, sizeof(FirmOffensive2)-sizeof(Firm) );
	animation_offset = 0;
	camouflage_flag = 0;
	firm_attack_animation_delay = -1; //no. of animation frame of attack 
	firm_attack_delay = 0; //delay of enemy checking after each attack
	return_target_obj_recno = 0;
//	attack_object = 'E';
}
//-------- End of function FirmOffensive2::FirmOffensive2() ------//

//-------- Begin of function FirmOffensive2::init ------//
void FirmOffensive2::init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno)
{
	Firm::init(xLoc, yLoc, nationRecno, firmId, buildCode, builderRecno);
	base_firm_build_id = firm_build_id;	

	Location *l = world.get_loc(center_x, center_y);

/*	if(!nation_recno || ( nation_recno && tech_res[TECH_CAMOUFLAGE_ABILITY]->get_nation_tech_level(nation_recno)) )
	{
		plant_id = plant_res.scan(0, terrain_res[l->terrain_id]->average_type, 0);
		is_ai = 0;
	}*/
	// check firmId to change this para
	propagate_count = 1;	
}
//-------- End of function FirmOffensive2::init ------//

//--------- Begin of function Firm::deinit --------//
//
void FirmOffensive2::deinit()
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
//------ begin of function FirmOffensive2::process_monster_firm -------//
//
void FirmOffensive2::process_monster_firm()
{
	BaseObj *targetObj;

//	if (firm_attack_delay > 0)
//		firm_attack_delay --;

//	if (firm_id == FIRM_WILDE_LISHORR)
//		nation_recno = 0;

	if (firm_attack_animation_delay == ATTACK_ANIMATION_DELAY - 6 && !base_obj_array.is_deleted(target_obj_recno))
	{
		se_res.sound(center_x, center_y, 1, 'F', firm_id, "A1" );

		BaseObj *base_obj = base_obj_array[target_obj_recno];
		if (base_obj->cast_to_Unit())
			bullet_array.add_bullet(this, base_obj->cast_to_Unit());
		else
		if (base_obj->cast_to_Firm())
			bullet_array.add_bullet(this, base_obj->cast_to_Firm());
		else
		if (base_obj->cast_to_Town())
			bullet_array.add_bullet(this, base_obj->cast_to_Town());
		else
			err_here();
	}

	if (firm_attack_animation_delay > 0)
	{
		firm_attack_animation_delay = firm_attack_animation_delay -2;
		return;
	}

	//	restore firm animation and valuables just after attacks //
	if (firm_attack_animation_delay == 0)
	{
	//	attack_object = 'E';
		// when no attack the following valuable must = -1 //
		firm_attack_animation_delay = -1;
		firm_attack_delay = ATTACK_DELAY;
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
						int j;
						for ( j=0; j <bitmapCount; j++ )
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
		
	if (firm_attack_delay > 0)
	{
		firm_attack_delay --;
		return;
	}		

	// start attacking
	// if return fire or target selected, return it first
	if (return_target_obj_recno)
	{
		target_obj_recno = return_target_obj_recno;

		if (!base_obj_array.is_deleted(target_obj_recno))
		{	
			BaseObj *base_obj = base_obj_array[target_obj_recno];
			if (!attack_target(base_obj))
				return_target_obj_recno = 0;
		}
		else
			return_target_obj_recno = 0;
	}
	else
	// searching enemies to attack //
	{
		int xLoc1 = loc_x1 - SCAN_X_RANGE;
		int yLoc1 = loc_y1 - SCAN_Y_RANGE;
		int xLoc2 = loc_x2 + SCAN_X_RANGE;
		int yLoc2 = loc_y2 + SCAN_Y_RANGE;
	
		xLoc1 = MAX( xLoc1, 0 );
		yLoc1 = MAX( yLoc1, 0 );
		xLoc2 = MIN( xLoc2, MAX_WORLD_X_LOC-1 );
		yLoc2 = MIN( yLoc2, MAX_WORLD_Y_LOC-1 );

		//------------------------------------------//
	
		int		enemyCombatLevel=0;		// the higher the rating, the easier we can attack the target town.
		int 		xLoc, yLoc;
		int		enemyXLoc= -1, enemyYLoc;
		Unit* 	unitPtr;
	//	Nation* 	nationPtr = nation_array[nation_recno];
		Firm*		firmPtr;
		Location* locPtr;

		for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
		{
			locPtr = world.get_loc(xLoc1, yLoc);

			for( xLoc=xLoc1 ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
			{
		      //--- if there is an enemy unit here ---// 

				if( locPtr->unit_recno(UNIT_LAND) )
				{
					unitPtr = unit_array[ locPtr->unit_recno(UNIT_LAND) ];

					if (unitPtr->is_camouflage( nation_recno ))
						continue;
	
				//	if (firm_id != FIRM_WILDE_LISHORR)
					{
						if( !unitPtr->nation_recno )
							continue;

						if( unitPtr->spy_recno )		// if the target unit is our spy, don't attack
						{
							Spy* spyPtr = spy_array[unitPtr->spy_recno];
			
							if( spyPtr->true_nation_recno == nation_recno &&
							 spyPtr->notify_cloaked_nation_flag == 0 )
								continue;
						}
				
						if( nation_recno && unitPtr->nation_recno )
						{
							NationRelation* nationRelation = nation_array[nation_recno]->get_relation(unitPtr->nation_recno);
	
							if( nationRelation->status != RELATION_HOSTILE || !nationRelation->should_attack )
								continue;
						}
					}

					//--- if the unit is idle and he is our enemy ---//
				
					if(unitPtr->nation_recno != nation_recno	&& unitPtr->unit_id != UNIT_CARAVAN)
					{
	
						enemyCombatLevel += (int) unitPtr->hit_points;

						if( enemyXLoc == -1 || m.random(5)==0 )
						{
							enemyXLoc = xLoc;
							enemyYLoc = yLoc;
							targetObj = unitPtr;
							target_obj_recno = targetObj->base_obj_recno;		
						//	attack_object = 'U';
						}
					}
				}
				//--- if there is an enemy firm here ---//
	
				else if( locPtr->is_firm() )
				{
					firmPtr = firm_array[locPtr->firm_recno()];

				//	if (firm_id != FIRM_WILDE_LISHORR)
					{
						if( !firmPtr->nation_recno )
							continue;

						if( nation_recno && firmPtr->nation_recno )
						{
							NationRelation* nationRelation = nation_array[nation_recno]->get_relation(firmPtr->nation_recno);
		
							if( nationRelation->status != RELATION_HOSTILE || !nationRelation->should_attack )
								continue;
						}
					}
					//------- if this is a firm of our enemy -------//

					if (firmPtr->nation_recno != nation_recno &&	firmPtr->firm_id != FIRM_OFFENSIVE_BUILDING_4)
					{
						int firmCombatLevel = firmPtr->total_combat_level();

						enemyCombatLevel += MAX(50, firmCombatLevel);		// minimum 50 points for each firm

						if( enemyXLoc == -1 || m.random(5)==0 )
						{
							enemyXLoc = firmPtr->loc_x1;
							enemyYLoc = firmPtr->loc_y1;
							targetObj = firmPtr;
							target_obj_recno = targetObj->base_obj_recno;		
						//	attack_object = 'F';
						}
					}
				}
			}
		}
		//--------- attack the target now -----------//
		camouflage_flag = 0;

		if( enemyCombatLevel > 0 )
		{
			err_when( enemyXLoc < 0 );
			attack_target(targetObj);
		}
	}
	//end if of attack
}
//------ End of function FirmOffensive2::process_monster_firm -------//

//------ Begin of function FirmOffensive2::attack_target -------//
int FirmOffensive2::attack_target(BaseObj* targetObj) 
{
	int xLoc1 = loc_x1 - ATTACK_X_RANGE;
	int yLoc1 = loc_y1 - ATTACK_Y_RANGE;
	int xLoc2 = loc_x2 + ATTACK_X_RANGE;
	int yLoc2 = loc_y2 + ATTACK_Y_RANGE;
	
	if(targetObj->cast_to_Unit())
	{
		if ( ((Unit*)targetObj)->next_x_loc() < xLoc1 ||
			  ((Unit*)targetObj)->next_x_loc() > xLoc2 ||
			  ((Unit*)targetObj)->next_y_loc() < yLoc1 ||
			  ((Unit*)targetObj)->next_y_loc() > yLoc2 ||
			  ((Unit*)targetObj)->unit_id == UNIT_CARAVAN ||
			  ((Unit*)targetObj)->nation_recno == nation_recno
			)
			return 0;	

		animation_offset = 1 + get_dir(center_x, center_y, ((Unit*)targetObj)->next_x_loc(), ((Unit*)targetObj)->next_y_loc());
	}
	else
	if(targetObj->cast_to_Firm())
	{
		if ( ((Firm*)targetObj)->center_x < xLoc1 ||
			  ((Firm*)targetObj)->center_x > xLoc2 ||
			  ((Firm*)targetObj)->center_y < yLoc1 ||
			  ((Firm*)targetObj)->center_y > yLoc2 ||
			  ((Firm*)targetObj)->nation_recno == nation_recno
			)
			return 0;	

		animation_offset = 1 + get_dir(center_x, center_y, ((Firm*)targetObj)->center_x, ((Firm*)targetObj)->center_y);
	}
	else
	if(targetObj->cast_to_Town())
	{
		if ( ((Town*)targetObj)->center_x < xLoc1 ||
			  ((Town*)targetObj)->center_x > xLoc2 ||
			  ((Town*)targetObj)->center_y < yLoc1 ||
			  ((Town*)targetObj)->center_y > yLoc2 ||
			  (nation_recno && ((Town*)targetObj)->resistance(nation_recno) <= 0)
			)
			return 0;	

		animation_offset = 1 + get_dir(center_x, center_y, ((Town*)targetObj)->center_x, ((Town*)targetObj)->center_y);
	}
	else
		err_here();
	
	camouflage_flag = 1;

	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);
	firmBuild->free_bitmap_res();

	firm_build_id = base_firm_build_id + animation_offset;
	firm_attack_animation_delay = ATTACK_ANIMATION_DELAY;

	//--------- set animation firm animation for attack vars ---------//
	firmBuild = firm_res.get_build(firm_build_id);
	// ##### patch begin Gilbert 1/9 ######//
	firmBuild->load_bitmap_res();	// load after changing firm_build_id
	// ##### patch end Gilbert 1/9 ######//
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

				int j;
				for ( j=0; j <bitmapCount; j++ )
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
	return 1;
}
//------ End of function FirmOffensive2::attack_target -------//

//------ begin of function FirmOffensive2::return_fire -------//
//
int FirmOffensive2::return_fire(BaseObj* targetObj)
{
	if (targetObj && !return_target_obj_recno)
	{
		return_target_obj_recno = targetObj->base_obj_recno;		
		return 1;
	}
	else
		return 0;

}
//------ end of function FirmOffensive2::return_fire -------//

//------ begin of function FirmOffensive2::bullet_damage -------//

float FirmOffensive2::bullet_damage()
{
	return OFFENSIVE2_BULLET_DAMAGE;
}
//------ end of function FirmOffensive2::bullet_damage -------//


//------ begin of function FirmOffensive2::bullet_radius -------//

short FirmOffensive2::bullet_radius()
{
	return (LOCATE_WIDTH<<1);// - 2 ;
}
//------ end of function FirmOffensive2::bullet_radius -------//


//------ begin of function FirmOffensive2::bullet_fire -------//

char FirmOffensive2::bullet_fire()
{
	return 2;
}
//------ end of function FirmOffensive2::bullet_fire -------//


//------ begin of function FirmOffensive2::bullet_id -------//

short FirmOffensive2::bullet_id()
{
	FirmBuild* firmBuild = firm_res.get_build(base_firm_build_id);
	
	if (config.building_size == 1)	
		return firmBuild->bullet_sprite_id;
	else
		return firmBuild->bullet_sprite_id + 1;
}
//------ end of function FirmOffensive2::bullet_id -------//


//------ begin of function FirmOffensive2::bullet_speed -------//

short FirmOffensive2::bullet_speed()
{
	return OFFENSIVE2_BULLET_SPEED;
}
//------ end of function FirmOffensive2::bullet_speed -------//

//------ begin of function FirmOffensive2::bullet_init_z -------//

short FirmOffensive2::bullet_init_z()
{
	return altitude;
}
//------ end of function FirmOffensive2::bullet_init_z -------//

//------ Begin of function FirmOffensive2::get_dir -------//
//
// Compare the current and destination locations and
// return which direction the sprite should move towards or attack at.
//
int FirmOffensive2::get_dir(int curX, int curY, int destX, int destY)
{
	unsigned xDiff( abs(destX - curX) );
	unsigned yDiff( abs(destY - curY) );
	unsigned squSize = MAX(xDiff, yDiff); // the size of the square we consider

	if( destX == curX )
	{
		if( destY > curY )
			return 1;//7;//DIR_S;
		else
			return 5;//3;//DIR_N;
	}
	else if( destX < curX )
	{
		// west side
		if( destY > curY )
		{ // south west quadrant
			if(2*xDiff <= squSize)
				return 1;//7;//DIR_S;
			else if(2*yDiff <= squSize)
				return 3;//5;//DIR_W;
			else
				return 2;//6;//DIR_SW;
		}
		else
		{ // north west quadrant
			if(2*xDiff <= squSize)
				return 5;//3;//DIR_N;
			else if(2*yDiff <= squSize)
				return 3;//5;//DIR_W;
			else
				return 4;//DIR_NW;
		}
	}
	else // destX > curX
	{
		// east side
		if( destY > curY )
		{ // south east quadrant
			if(2*xDiff <= squSize)
				return 1;//7;//DIR_S;
			else if(2*yDiff <= squSize)
				return 7;//1;//DIR_E;
			else
				return 0;//DIR_SE;
		}
		else
		{
			// north east quadrant
			if(2*xDiff <= squSize)
				return 5;//3;//DIR_N;
			else if(2*yDiff <= squSize)
				return 7;//1;//DIR_E;
			else
				return 6;//2;//DIR_NE;
		}
	}

	return 0;//DIR_N;
}
//------- End of function FirmOffensive2::get_dir --------//

// -------- begin of function FirmOffensive2::draw ---------//
void FirmOffensive2::draw(int displayLayer)
{
	if( !plant_id || camouflage_flag )
	{
		Firm::draw(displayLayer);
	}
	else
	{
		PlantBitmap *plantBitmap = plant_res.get_bitmap(plant_id);
	
		world.zoom_matrix->put_bitmap_offset(loc_x2*LOCATE_WIDTH, loc_y2*LOCATE_HEIGHT,
		world.interpolate_z(loc_x2*LOCATE_WIDTH, loc_y2*LOCATE_HEIGHT), plantBitmap->bitmap_ptr, 
		plantBitmap->offset_x, plantBitmap->offset_y, NULL, 0, 1);
		//	plantBitmap->draw(center_x, center_y);
		//	rock_res.draw(rock_id, loc_x1, loc_y1, altitude, 1 );
	}
}
// -------- end of function FirmOffensive2::draw ---------//

//----------- Begin of function FirmOffensive2::find_best_place_loc ----------//
//
// Determine the location of a new firm. It's best to have the
// new firm within the refective range of: towns, factories and
// mines.
//
// <short> buildFirmId		 - >0 id. of the firm to be built
//										0 if the place to be built is a town
// <short> refXLoc, refYLoc - either the location of a town or a firm,
//									   the market must be built next to it.
// <short&> resultXLoc, resultYLoc - result location of the firm.
//
// return: <int> 1 - succeed, 0 - fail
//

int FirmOffensive2::find_best_place_loc(short buildFirmId, short refXLoc, short refYLoc, short& resultXLoc, short& resultYLoc, short distance)
{
	#define IDEAL_INTER_PLACE_SPACE 		3

	if (distance == 0)
		distance = IDEAL_INTER_PLACE_SPACE;

	Location *locPtr = world.get_loc(refXLoc, refYLoc);
	short centerX, centerY, refX1, refY1, refX2, refY2;

	//-------- get the refective area ---------//

	int   originFirmRecno=0, originTownRecno=0;
	Firm* firmPtr;
	Town* townPtr;

	BYTE buildRegionId  = locPtr->region_id;
	int  buildIsPlateau = locPtr->is_plateau();
	int  effectiveDis;

	//-----------------------------------------//

	if( locPtr->is_firm() )
	{
		originFirmRecno = locPtr->firm_recno();

		firmPtr = firm_array[originFirmRecno];

		centerX = firmPtr->center_x;
		centerY = firmPtr->center_y;

		effectiveDis = world.effective_distance(buildFirmId, firmPtr->firm_id);

//		if( firmPtr->firm_id == FIRM_HARBOR )
//		{
//			buildRegionId  = ((FirmHarbor*)firmPtr)->land_region_id;
//			buildIsPlateau = 0;
//		}
	}
	else if( locPtr->is_town() )
	{
		originTownRecno = locPtr->town_recno();

		townPtr = town_array[originTownRecno];

		centerX = townPtr->center_x;
		centerY = townPtr->center_y;

		effectiveDis = world.effective_distance(buildFirmId, 0);
	}
	else
		err_here();

	refX1 = centerX - effectiveDis;
	refY1 = centerY - effectiveDis;
	refX2 = centerX + effectiveDis;
	refY2 = centerY + effectiveDis;

	//------------------------------------------------------//

	int placeLocWidth, placeLocHeight;

	if( buildFirmId )
	{
		placeLocWidth  = firm_res[buildFirmId]->loc_width;
		placeLocHeight = firm_res[buildFirmId]->loc_height;
	}
	else
	{
		placeLocWidth  = STD_TOWN_LOC_WIDTH;
		placeLocHeight = STD_TOWN_LOC_HEIGHT;
	}

	refX1 -= placeLocWidth/2;		// since we use loc_x1 as the building reference, we need to shift it so it will match the use of center_x in effective distance
	refY1 -= placeLocHeight/2;
	refX1 = MAX(0, refX1);
	refY1 = MAX(0, refY1);
	refX2 = MIN(MAX_WORLD_X_LOC-1, refX2);
	refY2 = MIN(MAX_WORLD_Y_LOC-1, refY2);

	//-------- build a matrix on the refective area ---------//

	int 	 refWidth=refX2-refX1+1, refHeight=refY2-refY1+1;
	short* refMatrix = (short*) mem_add( sizeof(short) * refWidth * refHeight );
	short* refMatrixPtr;

	//------ initialize the weights of the matrix ------//

	int xLoc, yLoc;   	// inner locations in the matrix receives more weights than outer locations do
	int t1, t2;

	for( yLoc=refY1 ; yLoc<=refY2 ; yLoc++ )
	{
		refMatrixPtr = refMatrix + (yLoc-refY1)*refWidth;
		locPtr		 = world.get_loc( refX1, yLoc );

		for( xLoc=refX1 ; xLoc<=refX2 ; xLoc++, refMatrixPtr++, locPtr++ )
		{
			t1 = abs(xLoc-centerX);
			t2 = abs(yLoc-centerY);

			if( locPtr->region_id != buildRegionId ||
				 locPtr->is_plateau() != buildIsPlateau ||
				 locPtr->is_power_off() )
			{
				*refMatrixPtr = -1000;
			}
			else
			{
				*refMatrixPtr = 10-MAX(t1, t2);		// it's negative value, and the value is lower for the outer ones
			}
		}
	}

	//----- calculate weights of the locations in the matrix ----//

	int   xLocB, yLocB, weightAdd, weightReduce;
	short refBX1, refBY1, refBX2, refBY2;
	short refCX1, refCY1, refCX2, refCY2;

	for( yLoc=refY1 ; yLoc<=refY2 ; yLoc++ )
	{
		locPtr = world.get_loc(refX1, yLoc);

		for( xLoc=refX1 ; xLoc<=refX2 ; xLoc++, locPtr++ )
		{
			if( locPtr->region_id != buildRegionId ||
				 locPtr->is_plateau() != buildIsPlateau ||
				 locPtr->is_power_off() )
			{
				continue;
			}

			//------- if there is a firm on the location ------//

			weightAdd = 0;
			weightReduce = 0;

			if( locPtr->is_firm() )
			{
				firmPtr = firm_array[locPtr->firm_recno()];

				if( buildFirmId==FIRM_MARKET || buildFirmId==FIRM_FACTORY )		// only factories & market places need building close to other firms
				{
					int rc = 1;

					if( firmPtr->nation_recno != nation_recno )
						rc = 0;

					//----- check if the firm is of the right type ----//

					if( buildFirmId==FIRM_MARKET )		// build a market place close to mines and factories
					{
						if( firmPtr->firm_id!=FIRM_MINE && firmPtr->firm_id!=FIRM_FACTORY )	// market places should be built close to factories and mines and they are the only two firms that influence the location of the market place
							rc = 0;
					}
					else if( buildFirmId==FIRM_FACTORY )	// build a factory close to mines and market places
					{
						if( firmPtr->firm_id!=FIRM_MINE && firmPtr->firm_id!=FIRM_MARKET )	// market places should be built close to factories and mines and they are the only two firms that influence the location of the market place
							rc = 0;
					}

					//------------------------------------------/

					if( rc )
					{
						effectiveDis = world.effective_distance(firmPtr->firm_id, buildFirmId);

						refBX1 = firmPtr->center_x - effectiveDis;
						refBY1 = firmPtr->center_y - effectiveDis;
						refBX2 = firmPtr->center_x + effectiveDis;
						refBY2 = firmPtr->center_y + effectiveDis;

						weightAdd = 40;
					}
				}

				refCX1 = firmPtr->loc_x1-distance ;		// add negative weights on space around this firm
				refCY1 = firmPtr->loc_y1-distance ;		// so to prevent firms from building right next to the firm
				refCX2 = firmPtr->loc_x2+distance ;		// and leave some space for walking path.
				refCY2 = firmPtr->loc_y2+distance ;

				weightReduce = 20;
			}

			//------- if there is a town on the location ------//

			else if( locPtr->is_town() )
			{
				townPtr = town_array[locPtr->town_recno()];

				effectiveDis = world.effective_distance(0, buildFirmId);

				refBX1 = townPtr->center_x - effectiveDis;
				refBY1 = townPtr->center_y - effectiveDis;
				refBX2 = townPtr->center_x + effectiveDis;
				refBY2 = townPtr->center_y + effectiveDis;

				weightAdd = townPtr->population*2;

				//----- if the town is not our own -----//

				if(townPtr->nation_recno != nation_recno)
				{
					if( townPtr->nation_recno==0 )  		// it's an independent town
						weightAdd = weightAdd * ( 100-(int)townPtr->resistance(nation_recno) ) / 100;
					else											// more friendly nations get higher weights
					{
					//	int relationStatus = get_relation_status(townPtr->nation_recno);

					//	if( relationStatus >= RELATION_NEUTRAL )
					//		weightAdd = weightAdd * (relationStatus-RELATION_NEUTRAL+1) / 4;
					}
				}

				refCX1 = townPtr->loc_x1-distance ;		// add negative weights on space around this firm
				refCY1 = townPtr->loc_y1-distance ;		// so to prevent firms from building right next to the firm
				refCX2 = townPtr->loc_x2+distance ;		// and leave some space for walking path.
				refCY2 = townPtr->loc_y2+distance ;

				weightReduce = 100;
			}
			else
				continue;

			//------ add weights to the matrix ------//

			if( weightAdd )
			{
				for( yLocB=MAX(refY1,refBY1) ; yLocB<=MIN(refY2,refBY2) ; yLocB++ )
				{
					xLocB = MAX(refX1,refBX1);
					refMatrixPtr = refMatrix + (yLocB-refY1)*refWidth + (xLocB-refX1);

					for( ; xLocB<=MIN(refX2,refBX2) ; xLocB++ )
					{
						*refMatrixPtr++ += weightAdd;
					}
				}
			}

			//------ reduce weights from the matrix ------//

			if( weightReduce )
			{
				for( yLocB=MAX(refY1,refCY1) ; yLocB<=MIN(refY2,refCY2) ; yLocB++ )
				{
					xLocB = MAX(refX1,refCX1);
					refMatrixPtr = refMatrix + (yLocB-refY1)*refWidth + (xLocB-refX1);

					for( ; xLocB<=MIN(refX2,refCX2) ; xLocB++ )
					{
						*refMatrixPtr++ -= weightReduce;
					}
				}
			}
		}
	}

	//------ select the best building site in the matrix -------//

	resultXLoc = -1;
	resultYLoc = -1;

	short thisWeight, bestWeight=0;

	refX2 -= placeLocWidth-1;			// do not scan beyond the border
	refY2 -= placeLocHeight-1;

	for( yLoc=refY1 ; yLoc<=refY2 ; yLoc++ )
	{
		for( xLoc=refX1 ; xLoc<=refX2 ; xLoc++ )
		{
			if( world.get_region_id(xLoc, yLoc) != buildRegionId )
				continue;

			if( buildFirmId )
			{
				if( !world.can_build_firm(xLoc, yLoc, buildFirmId) )
					continue;
			}
			else
			{
				if( !world.can_build_town(xLoc, yLoc) )
					continue;
			}

			//---- calculate the average weight of a firm area ----//

			int totalWeight=0;

			refMatrixPtr = refMatrix + (yLoc-refY1)*refWidth + (xLoc-refX1);

			for( int yCount=0 ; yCount<placeLocHeight ; yCount++ )
			{
				for( int xCount=0 ; xCount<placeLocWidth ; xCount++ )
				{
					totalWeight += *refMatrixPtr++;
				}

				refMatrixPtr += refWidth-placeLocWidth;
			}

			//------- compare the weights --------//

			thisWeight = totalWeight / (placeLocWidth*placeLocHeight);

			if( thisWeight > bestWeight )
			{
				bestWeight = thisWeight;

				resultXLoc = xLoc;
				resultYLoc = yLoc;
			}
		}
	}

	//------ release the refective matrix -----//

	mem_del( refMatrix );

	return resultXLoc >= 0;
}
//-------- End of function FirmOffensive2::find_best_place_loc --------//

//-------- Begin of function FirmOffensive2::create_firm_next_to_place -------//
int FirmOffensive2::create_firm_next_to_place(Place* placePtr, int firmId, int firmRaceId, int nationRecno, short distance)
{
	short buildXLoc, buildYLoc;

	if( !find_best_place_loc(firmId, placePtr->loc_x1, placePtr->loc_y1, buildXLoc, buildYLoc, distance) )
		return 0;

	//----- get the build code -----//

	char *buildCode;

	err_when( firmRaceId==0 );

	if( firmRaceId > 0 )
		buildCode = race_res[firmRaceId]->code;
	else
		buildCode = monster_res[-firmRaceId]->firm_build_code;

	//----- build the firm now ------//

	int firmRecno = firm_array.build_firm(buildXLoc, buildYLoc, nationRecno, firmId, buildCode, 0, false, true);		// true-no cost for building the firm

	if( !firmRecno )		// no space for building the firm
		return 0;

	firm_array[firmRecno]->complete_construction();

	return firmRecno;
}
//-------- End of function FirmOffensive2::create_firm_next_to_place -------//


//-------- Begin of function FirmOffensive2::right_click -------//
//
// This function is called when the current firm is selected
// and the mouse right-click on a locatoin.
//
void FirmOffensive2::right_click(int xLoc, int yLoc)
{
	if (firm_id == FIRM_WILDE_LISHORR)
		return;

	Location* locPtr = world.get_loc(xLoc, yLoc);

	int baseObjRecno = locPtr->base_obj_recno();
	
	if (!baseObjRecno)
		return;

	if (base_obj_array.is_deleted(baseObjRecno))
		return;

	BaseObj *base_obj = base_obj_array[baseObjRecno];

	if (base_obj->nation_recno == nation_recno)
		return;

	if ( base_obj->cast_to_Unit() || base_obj->cast_to_Firm() || base_obj->cast_to_Town())
	{
		if( !remote.is_enable() )
			return_target_obj_recno = baseObjRecno;
		else
		{
			// packet structure : <firm recno> <base obj recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_OFF2_ATTACK, 2*sizeof(short));
			shortPtr[0] = firm_recno;
			shortPtr[1] = baseObjRecno;
		}
	}		
}
//-------- End of function FirmOffensive2::right_click -------//
