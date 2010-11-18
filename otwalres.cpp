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

// Filename    : OTWALLRES.CPP
// Description : Town Wall Resource


#include <OTWALRES.H>
#include <ALL.H>
#include <OMATRIX.H>
#include <OWORLDMT.H>
#include <OGAMESET.H>


//---------- define constant ------------//

#define TOWN_RACE_WALL_DB 		"TWALLRAC"
#define TOWN_WALL_DB          "TWALL"
#define TOWN_WALL_BITMAP_DB   "TWALLBMP"



// ------- begin of function TownWallRes::TownWallRes -----------//
//
TownWallRes::TownWallRes()
{
	init_flag = 0;
}
// ------- end of function TownWallRes::TownWallRes -----------//


// ------- begin of function TownWallRes::~TownWallRes -----------//
//
TownWallRes::~TownWallRes()
{
	deinit();
}
// ------- end of function TownWallRes::~TownWallRes -----------//


// ------- begin of function TownWallRes::init -----------//
//
void TownWallRes::init()
{
	deinit();


	//----- open town material bitmap resource file -------//

	String str;

	str  = DIR_RES;
	str += "I_TWALL.RES";

	res_bitmap.init_imported(str,1);	 // 1-read all into buffer

	load_town_race_wall();
	load_town_wall();
	load_town_wall_bitmap();

	init_flag = 1;
}
// ------- end of function TownWallRes::init -----------//


// ------- begin of function TownWallRes::deinit -----------//
//
void TownWallRes::deinit()
{
	if(init_flag)
	{
		mem_del(town_race_wall_array);
		mem_del(town_wall_array);
		mem_del(town_wall_bitmap_array);

		res_bitmap.deinit();

		init_flag = 0;
	}
}
// ------- end of function TownWallRes::deinit -----------//


// ------- begin of function TownWallRes::load_town_race_wall -----------//
//
void TownWallRes::load_town_race_wall()
{
	TownRaceWallRec   *townRaceWallRec;
	TownRaceWallInfo  *townRaceWallInfo;
	Database          *database = game_set.open_db(TOWN_RACE_WALL_DB);

	int	 i;

	town_race_wall_count = database->rec_count();
	town_race_wall_array = (TownRaceWallInfo *) mem_add( sizeof(TownRaceWallInfo)*town_race_wall_count );

	memset( town_race_wall_array, 0, sizeof(TownRaceWallInfo)*town_race_wall_count );
	
	for( i = 0; i < town_race_wall_count; ++i )
	{
		townRaceWallRec = (TownRaceWallRec *)database->read(i+1);
		townRaceWallInfo = town_race_wall_array + i;

		townRaceWallInfo->race_id          = m.atoi( townRaceWallRec->race_id, townRaceWallRec->RACE_ID_LEN );
		townRaceWallInfo->fence_id			  = m.atoi( townRaceWallRec->fence_id, townRaceWallRec->WALL_ID_LEN );
		townRaceWallInfo->basic_wall_id    = m.atoi( townRaceWallRec->basic_wall_id, townRaceWallRec->WALL_ID_LEN );
		townRaceWallInfo->adv_wall_id      = m.atoi( townRaceWallRec->adv_wall_id, townRaceWallRec->WALL_ID_LEN );
	}
}
// ------- end of function TownWallRes::load_town_race_wall -----------//


// ------- begin of function TownWallRes::load_town_wall -----------//
//
void TownWallRes::load_town_wall()
{
	TownWallRec   *townWallRec;
	TownWallInfo  *townWallInfo;
	Database          *database = game_set.open_db(TOWN_WALL_DB);

	int	 i;

	town_wall_count = database->rec_count();
	town_wall_array = (TownWallInfo *) mem_add( sizeof(TownWallInfo)*town_wall_count );

	memset( town_wall_array, 0, sizeof(TownWallInfo)*town_wall_count );
	
	for( i = 0; i < town_wall_count; ++i )
	{
		townWallRec = (TownWallRec *)database->read(i+1);
		townWallInfo = town_wall_array + i;

		townWallInfo->first_recno      = m.atoi( townWallRec->first_recno, townWallRec->RECNO_LEN );
		townWallInfo->rec_count        = m.atoi( townWallRec->rec_count, townWallRec->RECNO_LEN );
		townWallInfo->max_frame        = m.atoi( townWallRec->max_frame, townWallRec->FRAME_LEN );
	}
}
// ------- end of function TownWallRes::load_town_wall -----------//


// ------- begin of function TownWallRes::load_town_wall_bitmap -----------//
//
void TownWallRes::load_town_wall_bitmap()
{
	TownWallBitmapRec   *townWallBitmapRec;
	TownWallBitmapInfo  *townWallBitmapInfo;
	Database            *database = game_set.open_db(TOWN_WALL_BITMAP_DB);

	int	 i;

	town_wall_bitmap_count = database->rec_count();
	town_wall_bitmap_array = (TownWallBitmapInfo *) mem_add( sizeof(TownWallBitmapInfo)*town_wall_bitmap_count );

	memset( town_wall_bitmap_array, 0, sizeof(TownWallBitmapInfo)*town_wall_bitmap_count );
	
	for( i = 0; i < town_wall_bitmap_count; ++i )
	{
		townWallBitmapRec = (TownWallBitmapRec *)database->read(i+1);
		townWallBitmapInfo = town_wall_bitmap_array + i;

		townWallBitmapInfo->frame            = m.atoi( townWallBitmapRec->frame, townWallBitmapRec->FRAME_LEN );
		townWallBitmapInfo->offset_x         = m.atoi( townWallBitmapRec->offset_x, townWallBitmapRec->OFFSET_LEN );
		townWallBitmapInfo->offset_y         = m.atoi( townWallBitmapRec->offset_y, townWallBitmapRec->OFFSET_LEN );
		townWallBitmapInfo->mode             = townWallBitmapRec->mode;

		long bitmapOffset;
		memcpy( &bitmapOffset, townWallBitmapRec->bitmap_ptr, sizeof(long) );
		townWallBitmapInfo->bitmap_ptr    = res_bitmap.read_imported(bitmapOffset);

		// modify offset_x/y for 7k2
		int locWidth = m.atoi( townWallBitmapRec->loc_width, townWallBitmapRec->LOC_LEN );
		int locHeight = m.atoi( townWallBitmapRec->loc_height, townWallBitmapRec->LOC_LEN );
		townWallBitmapInfo->offset_x += -locWidth*LOCATE_WIDTH/2 - (-locWidth*ZOOM_LOC_X_WIDTH/2 + -locHeight*ZOOM_LOC_Y_WIDTH/2);
		townWallBitmapInfo->offset_y += -locHeight*LOCATE_HEIGHT/2 - (-locWidth*ZOOM_LOC_X_HEIGHT/2 + -locHeight*ZOOM_LOC_Y_HEIGHT/2);
	}
}
// ------- end of function TownWallRes::load_town_wall_bitmap -----------//




// ------- begin of function TownWallRes::search -----------//
//
// search specific bitmap of the wall
//
// <int> raceId              raceId of the town
// <int> wallLevel           wall level of the town, 1= basic, 2= advanced
// <int> frame               frame of the wall
// <int> mode                'B' for back and 'F' for front
//
// return TownWallBitmapInfo struct pointer, NULL if not found
//
TownWallBitmapInfo *TownWallRes::search( int raceId, int wallLevel, int frame, char mode )
{
	// ------ get the wall id from race and wallLevel --------//

	TownRaceWallInfo *townRaceWallInfo = town_race_wall_array+raceId-1;
	err_when( townRaceWallInfo->race_id != raceId );

	int wallId;
	switch( wallLevel )
	{
	case 0:
		wallId = townRaceWallInfo->fence_id;
		break;
	case 1:
		wallId = townRaceWallInfo->basic_wall_id;
		break;
	case 2:
		wallId = townRaceWallInfo->adv_wall_id;
		break;
	default:
		err_here();
		return NULL;
	}

	if( wallId <= 0 || wallId > town_wall_count )
	{
		err_here();
		return NULL;
	}

	// -------- search wall Bitmap for that frame and mode ---------- //

	TownWallInfo *townWallInfo = town_wall_array + wallId - 1;

	int townWallBitmapRecno = townWallInfo->first_recno;

	if( townWallBitmapRecno <= 0 || townWallBitmapRecno > town_wall_bitmap_count )
	{
		err_here();
		return NULL;
	}

	TownWallBitmapInfo *townWallBitmapInfo = town_wall_bitmap_array + townWallBitmapRecno - 1;

	for(int i = townWallInfo->rec_count; i > 0; --i, ++townWallBitmapInfo )
	{
		if( townWallBitmapInfo->frame == frame && townWallBitmapInfo->mode == mode )
			return townWallBitmapInfo;
	}

	return NULL;
}
// ------- end of function TownWallRes::search -----------//



// ------- begin of function TownWallRes::search -----------//
//
// search specific bitmap of the wall
//
// <int> raceId              raceId of the town
// <int> wallLevel           wall level of the town, 1= basic, 2= advanced
// <float> hp                current hit point of the wall
// <float fullHp             full hit point of the wall
// <int> mode                'B' for back and 'F' for front
//
// return TownWallBitmapInfo struct pointer, NULL if not found
//
TownWallBitmapInfo *TownWallRes::search( int raceId, int wallLevel, float hp, float fullHp, char mode )
{
	// ------ get the wall id from race and wallLevel --------//

	TownRaceWallInfo *townRaceWallInfo = town_race_wall_array+raceId-1;
	err_when( townRaceWallInfo->race_id != raceId );

	int wallId;
	switch( wallLevel )
	{
	case 0:
		wallId = townRaceWallInfo->fence_id;
		break;
	case 1:
		wallId = townRaceWallInfo->basic_wall_id;
		break;
	case 2:
		wallId = townRaceWallInfo->adv_wall_id;
		break;
	default:
		err_here();
		return NULL;
	}

	if( wallId <= 0 || wallId > town_wall_count )
	{
		err_here();
		return NULL;
	}

	// --------- calculate frame ----------//

	TownWallInfo *townWallInfo = town_wall_array + wallId - 1;

	int townWallBitmapRecno = townWallInfo->first_recno;
	int townWallBitmapMaxFrame = townWallInfo->max_frame;
	err_when( fullHp < 1.0f );
	int frame = 1 + int(hp * townWallBitmapMaxFrame / fullHp);
	if( frame > townWallBitmapMaxFrame )
		frame = townWallBitmapMaxFrame;

	if( townWallBitmapRecno <= 0 || townWallBitmapRecno > town_wall_bitmap_count )
	{
		err_here();
		return NULL;
	}

	// -------- search wall Bitmap for that frame and mode ---------- //

	TownWallBitmapInfo *townWallBitmapInfo = town_wall_bitmap_array + townWallBitmapRecno - 1;

	for(int i = townWallInfo->rec_count; i > 0; --i, ++townWallBitmapInfo )
	{
		if( townWallBitmapInfo->frame == frame && townWallBitmapInfo->mode == mode )
			return townWallBitmapInfo;
	}

	return NULL;
}
// ------- end of function TownWallRes::search -----------//
