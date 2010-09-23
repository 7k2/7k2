//Filename    : OROCKRES.H
//Description : Header file of object RockRes
//Owner       : Gilbert


#ifndef __OROCKRES_H
#define __OROCKRES_H


#include <GAMEDEF.H>
#include <ORESDB.H>
#include <ORESPAL.H>
#include <OBITMAP.H>

// ----------- Define constant ----------//
#define ROCK_BLOCKING_TYPE 'R'
#define DIRT_NON_BLOCKING_TYPE 'D'
#define DIRT_BLOCKING_TYPE 'E'
#define DIRT_LAKE_TYPE 'P'

#define MAX_ROCK_WIDTH 6
#define MAX_ROCK_HEIGHT 6

// ----------- Define struct RockRec -----------//

struct RockRec
{
	enum { ROCKID_LEN=5, LOC_LEN=2, RECNO_LEN=4, MAX_FRAME_LEN=2,
		PAL_NAME_LEN=8, PAL_OFFSET_LEN=4};
	char	rock_id[ROCKID_LEN];
	char	rock_type;
	char	loc_width[LOC_LEN];
	char	loc_height[LOC_LEN];
	char	terrain_1;
	char	terrain_2;
	char	pal_file_name[PAL_NAME_LEN];
	char	pal_offset[PAL_OFFSET_LEN];
	char	first_bitmap_recno[RECNO_LEN];
	char	max_frame[MAX_FRAME_LEN];
};

// ------------ Define struct RockBitmapRec ---------//

struct RockBitmapRec
{
	enum { ROCKID_LEN=5, LOC_LEN=2, OFFSET_LEN=4, FRAME_NO_LEN=2, DELAY_LEN=3,
		FILE_NAME_LEN=8, BITMAP_PTR_LEN=4};
	char rock_id[ROCKID_LEN];
	char frame[FRAME_NO_LEN];
	char delay[DELAY_LEN];
	char next_frame[FRAME_NO_LEN];
	char alt_next[FRAME_NO_LEN];
	char loc_width[LOC_LEN];
	char loc_height[LOC_LEN];
	char offset_x[OFFSET_LEN];
	char offset_y[OFFSET_LEN];
	char file_name[FILE_NAME_LEN];
	char bitmap_ptr[BITMAP_PTR_LEN];
};

// ----------- Define struct RockInfo -----------//

struct RockInfo
{
	enum	{ ROCKID_LEN=5 };
	char	rock_name[ROCKID_LEN+1];
	char	rock_type;
	char	loc_width;
	char	loc_height;
	char	terrain_1;			// TerrainTypeCode
	char	terrain_2;			// TerrainTypeCode
	short	first_bitmap_recno;
	char	max_frame;
//	short	first_block_recno;
//	short block_offset[MAX_ROCK_HEIGHT][MAX_ROCK_WIDTH];	// make RockRes::locate_block faster
	short*	palw_ptr;				// pointing to part of the res_pal
	int	valid_terrain(char terrainType) { return (terrainType >= terrain_1 && terrainType <= terrain_2); }
};

// ------------ Define struct RockBitmapInfo ---------//

struct RockBitmapInfo
{
	char	frame;		// checking only
	char	delay;
	char	next_frame;
	char	alt_next;
	short offset_x;
	short offset_y;
	char*	bitmap_ptr;

	short width()     { return ((Bitmap *)bitmap_ptr)->get_width(); }
	short height()    { return ((Bitmap *)bitmap_ptr)->get_height(); }
	void	draw(RockInfo* rockInfo, short xLoc, short yLoc, short curZ);
	char	choose_next(long path) { return path ? next_frame : alt_next; }
};

// ------------ Define class RockRes -----------//

class RockRes
{
public:
	int              init_flag;

	int              rock_info_count;
	RockInfo*        rock_info_array;

	int              rock_bitmap_count;
	RockBitmapInfo*  rock_bitmap_array;

	ResourceDb       res_bitmap;
	ResourcePal			res_pal;

public:
	RockRes();
	~RockRes();
	void             init();
	void             deinit();

	RockInfo*        get_rock_info(short rockRecno);
	RockBitmapInfo*  get_bitmap_info(short rockBitmapRecno);

	char             choose_next(short rockId, char curFrame, long path);
	void             draw(short rockId, short xLoc, short yLoc, short curZ, char curFrame);
//	void             draw_block(short rockRecno, short xLoc, short yLoc,
//		short offsetX, short offsetY, char curFrame);

//	RockBlockInfo*   operator[](short rockBlockRecno)
//		{ return rock_block_array + rockBlockRecno -1; }

	// return rockId
	short            search(char *rockTypes, short minWidth, short maxWidth, 
		short minHeight, short maxHeight, int animatedFlag=-1, int findFirst=0,
		char terrainType=0);
	short            locate(char *rockName);

	// return rockBlockRecno
	// short            locate_block(short rockId, short xLoc, short yLoc);

	// return rockBitmapRecno
	short            get_bitmap_recno(short rockId, char curFrame);

private:
	void             load_info();
	void             load_bitmap_info();
};

extern RockRes rock_res;

#endif