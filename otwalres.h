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

// Filename    : OTWALLRES.H
// Description : TOWN WALL


#ifndef __OTWALLRES_H
#define __OTWALLRES_H

#include <ORESDB.H>


// ------- define struct TownRaceWallRec -------//

struct TownRaceWallRec
{
	enum { RACE_LEN=8, WALL_NAME_LEN=5, RACE_ID_LEN=2, WALL_ID_LEN=2 };
	char race[RACE_LEN];
	char fence[WALL_NAME_LEN];
	char basic_wall[WALL_NAME_LEN];
	char adv_wall[WALL_NAME_LEN];

	char race_id[RACE_ID_LEN];
	char fence_id[WALL_ID_LEN];
	char basic_wall_id[WALL_ID_LEN];
	char adv_wall_id[WALL_ID_LEN];
};

// ------- define struct TownWallRec -------//

struct TownWallRec
{
	enum { WALL_NAME_LEN=5, RECNO_LEN=4, FRAME_LEN=2 };

	char wall_name[WALL_NAME_LEN];
	char first_recno[RECNO_LEN];
	char rec_count[RECNO_LEN];
	char max_frame[FRAME_LEN];
};

// ------- define struct TownWallBitmapRec -------//

struct TownWallBitmapRec
{
	enum { WALL_NAME_LEN=5, FRAME_LEN=2, LOC_LEN=2, OFFSET_LEN=4,
		FILE_NAME_LEN=8, BITMAP_PTR_LEN=4 };

	char wall_name[WALL_NAME_LEN];
	char frame[FRAME_LEN];
	char mode;				// B=back, F=front
	char loc_width[LOC_LEN];
	char loc_height[LOC_LEN];
	char offset_x[OFFSET_LEN];
	char offset_y[OFFSET_LEN];
	char file_name[FILE_NAME_LEN];
	char bitmap_ptr[BITMAP_PTR_LEN];
};


// ------- define struct TownRaceWallInfo -------//

struct TownRaceWallInfo
{
	char race_id;
	char fence_id;
	char basic_wall_id;
	char adv_wall_id;
};

// ------- define struct TownWallInfo -------//

struct TownWallInfo
{
	short first_recno;
	short rec_count;
	short	max_frame;
};

// ------- define struct TownRaceBitmapInfo -------//

struct TownWallBitmapInfo
{
	short frame;
	short offset_x;
	short offset_y;
	char mode;
	char dummy;
	char *bitmap_ptr;
};

// ------- define class TownWallRes ---------//

class TownWallRes
{
public:
	int		init_flag;

	int		town_race_wall_count;
	TownRaceWallInfo *town_race_wall_array;

	int		town_wall_count;
	TownWallInfo *town_wall_array;

	int		town_wall_bitmap_count;
	TownWallBitmapInfo *town_wall_bitmap_array;

	ResourceDb	res_bitmap;

public:
	TownWallRes();
	~TownWallRes();

	void		init();
	void		deinit();

	TownWallBitmapInfo *search( int raceId, int wallLevel, int frame, char mode );
	TownWallBitmapInfo *search( int raceId, int wallLevel, float hp, float fullHp, char mode );

private:
	void		load_town_race_wall();
	void		load_town_wall();
	void		load_town_wall_bitmap();
};


extern TownWallRes town_wall_res;


#endif