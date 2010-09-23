// Filename    : OROCKRES.CPP
// Description : rock resource
// Owner       : Gilbert


#include <OROCKRES.H>
#include <OGAMESET.H>
#include <OMISC.H>
#include <OWORLD.H>
#include <OVGABUF.H>
#include <OTERRAIN.H>
#include <OCONFIG.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <OVGABUF.H>
#include <string.h>

// ---------- Define constant ------------//
//#define ROCK_DB        "ROCK"
//#define ROCK_BITMAP_DB "ROCKBMP"

// one rock is composed of one or many rock blocks
// each rock block has one or many frames
// each rock has its own animation sequence
// any animation sequence can change to one of the two sequences


// ------------ begin of function RockRes::RockRes -----------//
RockRes::RockRes()
{
	init_flag = 0;

	rock_info_count = 0;
	rock_info_array = NULL;

	rock_bitmap_count = 0;
	rock_bitmap_array = NULL;
}
// ------------ end of function RockRes::RockRes -----------//


// ------------ begin of function RockRes::~RockRes -----------//
RockRes::~RockRes()
{
	deinit();
}
// ------------ end of function RockRes::~RockRes -----------//


// ------------ begin of function RockRes::init -----------//
void RockRes::init()
{
	deinit();

	//----- open rock bitmap resource file -------//

	String str;

	str  = DIR_RES;
	// str += "I_ROCK.RES";
	str += "I_ROCK";
	str += config.terrain_set;
	str += ".RES";

	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	str  = DIR_RES;
	str += "PAL_ROC";
	str += config.terrain_set;
	str += ".RES";

	res_pal.init_imported(str,1);  // 1-read all into buffer

	//------- load database information --------//

	load_info();
	load_bitmap_info();

   //----------------------------------------//

	init_flag=1;
}
// ------------ end of function RockRes::init -----------//


// ------------ begin of function RockRes::deinit -----------//
void RockRes::deinit()
{
	if(init_flag)
	{
		mem_del(rock_info_array);
		mem_del(rock_bitmap_array);
		rock_info_count = 0;
		rock_bitmap_count = 0;

		res_pal.deinit();
		res_bitmap.deinit();

		init_flag = 0;
	}
}
// ------------ end of function RockRes::deinit -----------//


// ------------ begin of function RockRes::load_info -----------//
void RockRes::load_info()
{
	RockRec  	 *rockRec;
	RockInfo 	 *rockInfo;
	int      	 i;

	//---- read in rock count and initialize rock info array ----//

	String rockDbName;
	rockDbName  = DIR_RES;
	rockDbName += "ROCK";
	rockDbName += config.terrain_set;
	rockDbName += ".RES";
	Database rockDbObj(rockDbName, 1);
	//Database *dbRock = game_set.open_db(ROCK_DB);	// only one database can be opened at a time, so we read ROCK.DBF first
	Database *dbRock = &rockDbObj;

	rock_info_count = (short) dbRock->rec_count();
	rock_info_array = (RockInfo*) mem_add( sizeof(RockInfo)*rock_info_count );

	memset( rock_info_array, 0, sizeof(RockInfo) * rock_info_count );

	//---------- read in ROCK.DBF ---------//

	for( i=0 ; i<rock_info_count ; i++ )
	{
		rockRec  = (RockRec*) dbRock->read(i+1);
		rockInfo = rock_info_array+i;

		m.rtrim_fld( rockInfo->rock_name, rockRec->rock_id, rockRec->ROCKID_LEN );
		rockInfo->rock_type   	 	  = rockRec->rock_type;
		rockInfo->loc_width          = m.atoi(rockRec->loc_width, rockRec->LOC_LEN);
		rockInfo->loc_height         = m.atoi(rockRec->loc_height, rockRec->LOC_LEN);
		if( rockRec->terrain_1 == 0 || rockRec->terrain_1 == ' ')
			rockInfo->terrain_1 = 0;
		else
			rockInfo->terrain_1       = TerrainRes::terrain_code(rockRec->terrain_1);
		if( rockRec->terrain_2 == 0 || rockRec->terrain_2 == ' ')
			rockInfo->terrain_2 = 0;
		else
			rockInfo->terrain_2       = TerrainRes::terrain_code(rockRec->terrain_2);
		rockInfo->first_bitmap_recno = m.atoi(rockRec->first_bitmap_recno, rockRec->RECNO_LEN);
		rockInfo->max_frame          = m.atoi(rockRec->max_frame, rockRec->MAX_FRAME_LEN);

	//	if( rockRec->pal_file_name[0] == ' ' || rockRec->pal_file_name[0] == '\0' )
	//	{
			rockInfo->palw_ptr = NULL;
	//	}
	//	else 
	//	{
	//		long paletteOffset;
	//		memcpy( &paletteOffset, rockRec->pal_offset, sizeof(long) );
	//		rockInfo->palw_ptr = res_pal.read_imported_pal(paletteOffset);
	//	}
	}
}
// ------------ end of function RockRes::load_info -----------//


// ------------ begin of function RockRes::load_bitmap_info -----------//
void RockRes::load_bitmap_info()
{
	RockBitmapRec  	 *rockBitmapRec;
	RockBitmapInfo 	 *rockBitmapInfo;
	int      	 i;

	//---- read in rock count and initialize rock info array ----//

	String rockDbName;
	rockDbName  = DIR_RES;
	rockDbName += "ROCKBMP";
	rockDbName += config.terrain_set;
	rockDbName += ".RES";
	Database rockDbObj(rockDbName, 1);
	// Database *dbRock = game_set.open_db(ROCK_BITMAP_DB);	// only one database can be opened at a time, so we read ROCK.DBF first
	Database *dbRock = &rockDbObj;

	rock_bitmap_count = (short) dbRock->rec_count();
	rock_bitmap_array = (RockBitmapInfo*) mem_add( sizeof(RockBitmapInfo)*rock_bitmap_count );

	memset( rock_bitmap_array, 0, sizeof(RockBitmapInfo) * rock_bitmap_count );

	//---------- read in ROCKBMP.DBF ---------//

	for( i=0 ; i<rock_bitmap_count ; i++ )
	{
		rockBitmapRec  = (RockBitmapRec*) dbRock->read(i+1);
		rockBitmapInfo = rock_bitmap_array+i;

		rockBitmapInfo->frame = m.atoi(rockBitmapRec->frame, rockBitmapRec->FRAME_NO_LEN);
		rockBitmapInfo->delay = m.atoi(rockBitmapRec->delay, rockBitmapRec->DELAY_LEN);
		rockBitmapInfo->next_frame = m.atoi(rockBitmapRec->next_frame, rockBitmapRec->FRAME_NO_LEN);
		rockBitmapInfo->alt_next = m.atoi(rockBitmapRec->alt_next, rockBitmapRec->FRAME_NO_LEN);
		rockBitmapInfo->offset_x = m.atoi(rockBitmapRec->offset_x, rockBitmapRec->OFFSET_LEN);
		rockBitmapInfo->offset_y = m.atoi(rockBitmapRec->offset_y, rockBitmapRec->OFFSET_LEN);

		long bitmapOffset;
		memcpy( &bitmapOffset, rockBitmapRec->bitmap_ptr, sizeof(long) );
		rockBitmapInfo->bitmap_ptr = res_bitmap.read_imported(bitmapOffset);

		// modify offset_x/y for 7k2
		short width = m.atoi(rockBitmapRec->loc_width, rockBitmapRec->LOC_LEN);
		short height = m.atoi(rockBitmapRec->loc_height, rockBitmapRec->LOC_LEN);
		rockBitmapInfo->offset_x = rockBitmapInfo->offset_x + width*ZOOM_LOC_X_WIDTH/2 + 
									height*ZOOM_LOC_Y_WIDTH/2 -width*LOCATE_WIDTH/2;
		rockBitmapInfo->offset_y = rockBitmapInfo->offset_y + width*ZOOM_LOC_X_HEIGHT/2 +
									height*ZOOM_LOC_Y_HEIGHT/2 -height*LOCATE_HEIGHT/2;
	}
}
// ------------ end of function RockRes::load_bitmap_info -----------//


// ------------ begin of function RockRes::get_rock_info -----------//
RockInfo *RockRes::get_rock_info(short rockId)
{
	err_when(rockId <= 0 || rockId > rock_info_count );
	return rock_info_array + rockId - 1;
}
// ------------ end of function RockRes::get_rock_info -----------//


// ------------ begin of function RockRes::get_bitmap_info -----------//
RockBitmapInfo *RockRes::get_bitmap_info(short rockBitmapRecno)
{
	err_when( rockBitmapRecno <= 0 || rockBitmapRecno > rock_bitmap_count);
	return rock_bitmap_array + rockBitmapRecno - 1;
}
// ------------ end of function RockRes::get_bitmap_info -----------//


// ------------ begin of function RockRes::get_bitmap_recno -----------//
// return rockBitmapRecno
short RockRes::get_bitmap_recno(short rockId, char curFrame)
{
	RockInfo *rockInfo = get_rock_info(rockId);
	short rockBitmapRecno = rockInfo->first_bitmap_recno+curFrame-1;

#ifdef DEBUG
	// --------- validate curFrame with frame ----------//
	RockBitmapInfo *rockBitmapInfo = get_bitmap_info(rockBitmapRecno);
	err_when( rockBitmapInfo->frame != curFrame );
#endif

	return rockBitmapRecno;
}
// ------------ end of function RockRes::get_bitmap_recno -----------//


// ------------ begin of function RockRes::choose_next -----------//
// choose the next frame
// <short> rockId        rock Id
// <char> curFrame       the current frame no.
// <long> path           a random number, related to the probability of choosing alt_next
//                       eg. choose_next(..,.., m.random(x)); prob of using alt_next is 1/x
// return next frame no.
char RockRes::choose_next(short rockId, char curFrame, long path)
{
#ifdef DEBUG
	// -------- validate rockRecno ---------//
	RockInfo *rockInfo = get_rock_info(rockId);
	// -------- validate curFrame ----------//
	err_when(curFrame <= 0 || curFrame > rockInfo->max_frame);
#endif

	RockBitmapInfo *rockBitmapInfo = get_bitmap_info(get_bitmap_recno(rockId, curFrame));

	// -------- validate frame, next_frame and alt_next in rockBitmapInfo -------/
//	err_when(get_anim_info(get_anim_recno(rockRecno, rockBitmapInfo->next_frame))->frame != rockBitmapInfo->next_frame);
//	err_when(get_anim_info(get_anim_recno(rockRecno, rockBitmapInfo->alt_next))->frame != rockBitmapInfo->alt_next);

	return rockBitmapInfo->choose_next(path);
}
// ------------ end of function RockRes::choose_next -----------//


// ------------ begin of function RockRes::draw -----------//
// draw the whole rock at location (xLoc, yLoc)
// <short> rockId  			rock Id
// <short> xLoc,yLoc,curZ  where the rock is drawn
// <char> curFrame         frame no.
void RockRes::draw(short rockId, short xLoc, short yLoc, short curZ, char curFrame)
{
	RockInfo *rockInfo = get_rock_info(rockId);
	get_bitmap_info(get_bitmap_recno(rockId, curFrame))->draw(rockInfo, xLoc, yLoc, curZ );
}
// ------------ end of function RockRes::draw -----------//


// ------------ begin of function RockRes::search -----------//
//
// search which rock id has the specified criteria
//
// <char*> rockTypes   : a string of 'R', 'D', 'E', or NULL for any
// <short> minWidth    : minimum loc_width of RockInfo
// <short> maxWidth    : maximum loc_width of RockInfo
// <short> minHeight   : minimum loc_height of RockInfo
// <short> maxHeight   : maximum loc_height of RockInfo
// <int> animatedFlag  : 0 for non-animated (i.e. max_frame==1),
//                       +ve for animated( max_frame > 1),
//                       -ve for animated or non-animated    (default : -1)
// <int> findFirst     : whether to find the first match     (default : 0)
//
// return rock recno or 0 (for not found)
//
short RockRes::search(char *rockTypes, short minWidth, short maxWidth, short minHeight, short maxHeight,
	int animatedFlag, int findFirst, char terrainType )
{
	// -------- search a rock by rock_type, width and height ---------//
	short rockRecno = 0;
	int	findCount = 0;
	RockInfo *rockInfo;
	int	i;

	for( i = 1, rockInfo = rock_info_array; i <= rock_info_count; ++i, ++rockInfo)
	{
		if( (!rockTypes || strchr(rockTypes,rockInfo->rock_type) )
			&& (terrainType == 0 || rockInfo->valid_terrain(terrainType) )
			&& rockInfo->loc_width >= minWidth && rockInfo->loc_width <= maxWidth
			&& rockInfo->loc_height >= minHeight && rockInfo->loc_height <= maxHeight
			&& (animatedFlag < 0 || animatedFlag == 0 && rockInfo->max_frame == 1 ||
				animatedFlag > 0 && rockInfo->max_frame > 1) )
		{
			++findCount;
			if( findFirst )
			{
				rockRecno = i;
				break;
			}
			else if( m.random(findCount) == 0)
			{
				rockRecno = i;
			}
		}
	}

	return rockRecno;
}
// ------------ end of function RockRes::search -----------//


// ------------ begin of function RockRes::locate -----------//
// find a rock by name
//
// <char *> rockName     : name of rock to find
//
// return rock recno or 0 (for not found)
//
short RockRes::locate(char *rockName)
{
	// -------- search a rock by rock_type, width and height ---------//
	short rockRecno = 0;
	RockInfo *rockInfo;
	int	i;

	for( i = 1, rockInfo = rock_info_array; i <= rock_info_count; ++i, ++rockInfo)
	{
		if( strcmp(rockName, rockInfo->rock_name) == 0 )
		{
			rockRecno = i;
			break;
		}
	}

	return rockRecno;
}
// ------------ end of function RockRes::locate -----------//


// ------------ begin of function RockBitmapInfo::draw ---------//
void RockBitmapInfo::draw(RockInfo* rockInfo, short xLoc, short yLoc, short curZ)
{
	world.zoom_matrix->put_bitmap_offset(
		xLoc * LOCATE_WIDTH, yLoc * LOCATE_HEIGHT, curZ,
		bitmap_ptr, offset_x, offset_y,
		rockInfo->palw_ptr ? rockInfo->palw_ptr : vga.default_remap_table, 0, 1);
}
// ------------ end of function RockBitmapInfo::draw ---------//
