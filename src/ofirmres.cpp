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

//Filename    : OFIRMRES.CPP
//Description : Firm material resource object

#include <osys.h>
#include <ogameset.h>
#include <oworld.h>
#include <oworldmt.h>
#include <ounit.h>
#include <onation.h>
#include <ofirmres.h>
#include <otransl.h>
#include <omonsres.h>
#include <ovga.h>
#include <ocoltbl.h>
#include <ogame.h> 
#include <oraceres.h>
#include <otech.h>
#include <otechres.h>
#include <oconfig.h>

//---------- #define constant ------------//

#define FIRM_DB   		"FIRM"
#define FIRM_BUILD_DB 	"FBUILD"
#define FIRM_FRAME_DB 	"FFRAME"
#define FIRM_BITMAP_DB 	"FBITMAP"
#define FIRM_GROUP_DB 	"FGROUP"

#define FIRM_SMALL_DB   		"FIRM2"
#define FIRM_BUILD_SMALL_DB 	"FBUILD2"
#define FIRM_FRAME_SMALL_DB 	"FFRAME2"
#define FIRM_BITMAP_SMALL_DB 	"FBITMAP2"
#define FIRM_GROUP_SMALL_DB 	"FGROUP2"

// -------- define static vars --------//

short *FirmRes::cache_color_remap_table;

//------- Begin of function FirmRes::FirmRes -----------//

FirmRes::FirmRes()
{
	init_flag=0;
}
//--------- End of function FirmRes::FirmRes -----------//


//---------- Begin of function FirmRes::init -----------//
//
// This function must be called after a map is generated.
//
void FirmRes::init()
{
	deinit();

	//----- open firm material bitmap resource file -------//

	String str;

//	str  = DIR_RES;
//	str += "I_FIRM.RES";

//	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	str  = DIR_RES;
	if (config.building_size == 1)	
		str += "PAL_FIRM.RES";
	else
		str += "PALFIRM2.RES";

	res_pal.init_imported(str,1);		// 1-read all into buffer

	//------- load database information --------//

	load_firm_bitmap();		// call load_firm_bitmap() first as load_firm_info() will need info loaded by load_firm_bitmap()
	load_firm_build();
	load_firm_group();
	load_firm_info();

	// -------- set same race worker --------//

	firm_res[FIRM_BASE]->same_race_worker = 1;
	firm_res[FIRM_SPECIAL]->same_race_worker = 1;

	//----------------------------------------//

	init_flag=1;
}
//---------- End of function FirmRes::init -----------//


//---------- Begin of function FirmRes::deinit -----------//

void FirmRes::deinit()
{
	if( init_flag )
	{
		// free memory in firmBuild->palw_ptr
//		for( int i = 0; i < firm_build_count; ++i )
//			if( firm_build_array[i].palw_ptr )
//				mem_del( firm_build_array[i].palw_ptr );

		mem_del(firm_info_array);
		delete[] firm_build_array;
//		mem_del(firm_build_array);
		mem_del(firm_bitmap_array);
		mem_del(firm_group_array);

//		res_bitmap.deinit();
		res_pal.deinit();

		init_flag=0;
	}
}
//---------- End of function FirmRes::deinit -----------//


//------- Begin of function FirmRes::load_firm_bitmap -------//
//
// Read in information of FBITMAP.DBF into memory array
//
void FirmRes::load_firm_bitmap()
{
	FirmBitmapRec  *firmBitmapRec;
	FirmBitmap     *firmBitmap;
	int      		i;
//	long				bitmapOffset;
	Database 		*dbFirmBitmap;
	
	if (config.building_size == 1)
		dbFirmBitmap = game_set.open_db(FIRM_BITMAP_DB);
	else
		dbFirmBitmap = game_set.open_db(FIRM_BITMAP_SMALL_DB);

	firm_bitmap_count = (short) dbFirmBitmap->rec_count();
	firm_bitmap_array = (FirmBitmap*) mem_add( sizeof(FirmBitmap)*firm_bitmap_count );

	//------ read in firm bitmap info array -------//

	memset( firm_bitmap_array, 0, sizeof(FirmBitmap) * firm_bitmap_count );
	for( i=0 ; i<firm_bitmap_count ; i++ )
	{
		firmBitmapRec = (FirmBitmapRec*) dbFirmBitmap->read(i+1);
		firmBitmap    = firm_bitmap_array+i;

		memcpy( &firmBitmap->bitmap_ptr, firmBitmapRec->bitmap_ptr, sizeof(long) );

//		pointers on inside memory
//		firmBitmap->bitmap_ptr = res_bitmap.read_imported(bitmapOffset);
//		firmBitmap->width  	  = *((short*)firmBitmap->bitmap_ptr);
//		firmBitmap->height 	  = *(((short*)firmBitmap->bitmap_ptr)+1);
	
		firmBitmap->width  	  = m.atoi( firmBitmapRec->width, firmBitmapRec->LOC_LEN );
		firmBitmap->height 	  = m.atoi( firmBitmapRec->height, firmBitmapRec->LOC_LEN );

		firmBitmap->offset_x = m.atoi( firmBitmapRec->offset_x, firmBitmapRec->OFFSET_LEN );
		firmBitmap->offset_y = m.atoi( firmBitmapRec->offset_y, firmBitmapRec->OFFSET_LEN );

		firmBitmap->loc_width  = m.atoi( firmBitmapRec->loc_width , firmBitmapRec->LOC_LEN );
		firmBitmap->loc_height = m.atoi( firmBitmapRec->loc_height, firmBitmapRec->LOC_LEN );
		
		firmBitmap->delay = m.atoi( firmBitmapRec->delay, firmBitmapRec->DELAY_LEN );

		firmBitmap->ani_part = m.atoi( firmBitmapRec->ani_part, firmBitmapRec->FRAME_ID_LEN );
		firmBitmap->random_flag = firmBitmapRec->random_flag[0] == 'R' ? 1 : 0;

		firmBitmap->display_layer = firmBitmapRec->layer - '0';
		firmBitmap->mode = firmBitmapRec->mode;

#ifdef DEBUG
		if( firmBitmap->loc_width >= 4 || firmBitmap->loc_height >= 4 )
		{
			// break point here
			int x = 0;
		}
#endif

		// modify offset_x/y for 7k2
		firmBitmap->offset_x += -firmBitmap->loc_width*LOCATE_WIDTH/2 - (-firmBitmap->loc_width*ZOOM_LOC_X_WIDTH/2 + -firmBitmap->loc_height*ZOOM_LOC_Y_WIDTH/2);
		firmBitmap->offset_y += -firmBitmap->loc_height*LOCATE_HEIGHT/2 - (-firmBitmap->loc_width*ZOOM_LOC_X_HEIGHT/2 + -firmBitmap->loc_height*ZOOM_LOC_Y_HEIGHT/2);
	}
}
//--------- End of function FirmRes::load_firm_bitmap ---------//

//------- Begin of function FirmRes::load_firm_group -------//
//
// Read in information of FGROUP.DBF into memory array
//
void FirmRes::load_firm_group()
{
	FirmGroupRec *firmGroupRec;
	FirmGroup 	 *firmGroup;
	int      	 i;

	//---- read in firm count and initialize firm info array ----//

	Database *dbFirmGroup;

	if (config.building_size == 1)
		dbFirmGroup = game_set.open_db(FIRM_GROUP_DB);	// only one database can be opened at a time, so we read FIRM.DBF first
	else
		dbFirmGroup = game_set.open_db(FIRM_GROUP_SMALL_DB);

	firm_group_count = (short) dbFirmGroup->rec_count();
	firm_group_array = (FirmGroup*) mem_add( sizeof(FirmGroup)*firm_group_count );

	memset( firm_group_array, 0, sizeof(FirmGroup) * firm_group_count );


	//---------- read in FGROUP.DBF ---------//

	for( i=0 ; i<firm_group_count ; i++ )
	{
		firmGroupRec = (FirmGroupRec*) dbFirmGroup->read(i+1);
		firmGroup	 = firm_group_array+i;

		m.rtrim_fld( firmGroup->firm_code, firmGroupRec->firm_code, firmGroup->FIRM_CODE_LEN );
		
		firmGroup->group 	  = m.atoi( firmGroupRec->group, firmGroupRec->GROUP_LEN );
		
		firmGroup->race_id  = m.atoi( firmGroupRec->race_id, firmGroupRec->RACE_ID_LEN );
		
		m.rtrim_fld( firmGroup->race_code, firmGroupRec->race_code, firmGroup->RACE_CODE_LEN );

	}
}
//--------- End of function FirmRes::load_firm_group ---------//

//------- Begin of function FirmRes::load_firm_build -------//
//
// Read in information of FIRM.DBF into memory array
//
void FirmRes::load_firm_build()
{
	FirmBuildRec *firmBuildRec;
	FirmFrameRec *firmFrameRec;
	FirmBuild 	 *firmBuild;
	FirmBitmap   *firmBitmap;
	int      	 i, j, k, frameRecno, bitmapRecno;
	short			 *firstFrameArray;

	//---- read in firm count and initialize firm info array ----//

	Database *dbFirmBuild;
		
	if (config.building_size == 1)
		dbFirmBuild = game_set.open_db(FIRM_BUILD_DB);	// only one database can be opened at a time, so we read FIRM.DBF first
	else
		dbFirmBuild = game_set.open_db(FIRM_BUILD_SMALL_DB);

	firm_build_count = (short) dbFirmBuild->rec_count();
	firm_build_array = new FirmBuild[firm_build_count];

//	memset( firm_build_array, 0, sizeof(FirmBuild) * firm_build_count );

	//------ allocate an array for storing firstFrameRecno -----//

	firstFrameArray = (short*) mem_add( sizeof(short) * firm_build_count );

	//---------- read in FBUILD.DBF ---------//

	for( i=0 ; i<firm_build_count ; i++ )
	{
		firmBuildRec = (FirmBuildRec*) dbFirmBuild->read(i+1);
		firmBuild	 = firm_build_array+i;

		firmBuild->group 	  = m.atoi( firmBuildRec->group, firmBuildRec->GROUP_LEN );

		m.rtrim_fld( firmBuild->name, firmBuildRec->name, firmBuild->NAME_LEN );

		m.rtrim_fld( firmBuild->firm_resource_name, firmBuildRec->firm_resource_name, firmBuildRec->PAL_NAME_LEN );		
		translate.multi_to_win(firmBuild->name, firmBuild->NAME_LEN);

		firmBuild->animate_full_size = firmBuildRec->animate_full_size=='1';

		firmBuild->use_first_frame = !(firmBuildRec->use_first_frame=='0');

		firmBuild->frame_count = m.atoi( firmBuildRec->frame_count, firmBuildRec->FRAME_COUNT_LEN );
		firmBuild->ani_part_count = m.atoi( firmBuildRec->ani_part_count, firmBuildRec->FRAME_COUNT_LEN );
		if (firmBuild->animate_full_size == 1)
			firmBuild->ani_part_count = 1;

		if( firmBuildRec->pal_file_name[0] == ' ' || firmBuildRec->pal_file_name[0] == '\0' )
		{
			firmBuild->pal_ptr = NULL;
		}
		else
		{
			long paletteOffset;
			memcpy(&paletteOffset, firmBuildRec->pal_offset, sizeof(long));
			firmBuild->pal_ptr = 8+(BYTE *)res_pal.read_imported(paletteOffset);	// skip 8 bytes header
		}
		firmBuild->palw_ptr = NULL;

//		if( firmBuildRec->pal_file_name[0] == ' ' || firmBuildRec->pal_file_name == '\0' )
//			firmBuild->palw_ptr = NULL;
//		else
//		{
//			long paletteOffset;
//			memcpy(&paletteOffset, firmBuildRec->pal_offset, sizeof(long));
//			BYTE *palPtr = 8+(BYTE *)res_pal.read_imported(paletteOffset);	// skip 8 bytes header
//			firmBuild->palw_ptr = (short *)mem_add( 0x100 * sizeof(short) );
//
//			for( int j = 0; j < 0x100; ++j )
//			{
//				firmBuild->palw_ptr[j] = vga.make_pixel( j+(RGBColor *)palPtr );
//			}
//		}

		firmBuild->under_construction_bitmap_recno = m.atoi(firmBuildRec->under_construction_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		firmBuild->under_construction_bitmap_count =
			m.atoi(firmBuildRec->under_construction_bitmap_count, firmBuildRec->FRAME_COUNT_LEN);
		firmBuild->idle_bitmap_recno 					 = m.atoi(firmBuildRec->idle_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		firmBuild->ground_bitmap_recno             = m.atoi(firmBuildRec->ground_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		
		firmBuild->defense_attribute.init(m.atoi(firmBuildRec->sturdiness, firmBuildRec->ATTRIBUTE_LEN),
			m.atoi(firmBuildRec->is_wood, firmBuildRec->IS_WOOD_LEN));
		
		firmBuild->bullet_sprite_id = m.atoi(firmBuildRec->bullet_sprite_id, firmBuildRec->BULLET_ID_LEN);
		m.rtrim_fld( firmBuild->bullet_sprite_code, firmBuildRec->bullet_sprite_code, firmBuild->BULLET_SPRITE_LEN );

//		firmBuild->min_offset_y = m.atoi(firmBuildRec->min_offset_y, firmBuildRec->OFFSET_LEN);	
		err_when( firmBuild->frame_count > MAX_FIRM_FRAME );

		firstFrameArray[i] = m.atoi( firmBuildRec->first_frame, firmBuildRec->FIRST_FRAME_LEN );
	}

	//-------- read in FFRAME.DBF --------//

	Database 	*dbFirmFrame; 
		
	if (config.building_size == 1)
		dbFirmFrame = game_set.open_db(FIRM_FRAME_DB);
	else
		dbFirmFrame = game_set.open_db(FIRM_FRAME_SMALL_DB);

	int 			minOffsetX, minOffsetY;
	int			maxX2, maxY2;

	for( i=0 ; i<firm_build_count ; i++ )
	{
		firmBuild  = firm_build_array+i;
		frameRecno = firstFrameArray[i];

		minOffsetX = minOffsetY = 0xFFFF;
		maxX2 = maxY2 = 0;

		for( j=0 ; j<firmBuild->frame_count ; j++, frameRecno++ )
		{
			firmFrameRec = (FirmFrameRec*) dbFirmFrame->read(frameRecno);

			//------ following animation frames, bitmap sections -----//

			firmBuild->first_bitmap_array[j] = m.atoi( firmFrameRec->first_bitmap, firmFrameRec->FIRST_BITMAP_LEN );
			firmBuild->bitmap_count_array[j] = m.atoi( firmFrameRec->bitmap_count, firmFrameRec->BITMAP_COUNT_LEN );

			firmBuild->frame_delay_array[j] = m.atoi( firmFrameRec->delay, firmFrameRec->DELAY_LEN );

			//---- get the min offset_x, offset_y and max width, height ----//
			//
			// So we can get the largest area of all the frames in this building
			// and this will serve as a normal size setting for this building,
			// with variation from frame to frame
			//
			//--------------------------------------------------------------//

			firmBitmap = firm_bitmap_array + firmBuild->first_bitmap_array[j] - 1;

			for( k=firmBuild->bitmap_count_array[j] ; k>0 ; k--, firmBitmap++ )
			{
				if( firmBitmap->offset_x < minOffsetX )
					minOffsetX = firmBitmap->offset_x;

				if( firmBitmap->offset_y < minOffsetY )
					minOffsetY = firmBitmap->offset_y;

				if( firmBitmap->offset_x + firmBitmap->width > maxX2 )
					maxX2 = firmBitmap->offset_x + firmBitmap->width;

				if( firmBitmap->offset_y + firmBitmap->height > maxY2 )
					maxY2 = firmBitmap->offset_y + firmBitmap->height;
			}
		}

		//------- set FirmBuild Info -------//

		bitmapRecno = firmBuild->first_bitmap_array[0];

		//----- get the info of the first frame bitmap ----//

		firmBitmap = firm_bitmap_array + bitmapRecno - 1;

		firmBuild->loc_width  = firmBitmap->loc_width;
		firmBuild->loc_height = firmBitmap->loc_height;

		firmBuild->min_offset_x = minOffsetX;
		firmBuild->min_offset_y = minOffsetY;

		firmBuild->max_bitmap_width  = maxX2 - minOffsetX;
		firmBuild->max_bitmap_height = maxY2 - minOffsetY;

		//------ set firmBuild's under construction and idle bitmap recno -----//
		// therefore there is no need to set these variables in the fox files //
		// just set them all = 0 in the fox files (FBUILD.DBF) at this moment //

		if( firmBuild->under_construction_bitmap_recno==0 )
		{
			firmBuild->under_construction_bitmap_recno = bitmapRecno;
			firmBuild->under_construction_bitmap_count = 1;
		}

		err_when(firmBuild->under_construction_bitmap_count == 0);

		if( firmBuild->idle_bitmap_recno==0 )
			firmBuild->idle_bitmap_recno = bitmapRecno;
	}

	//------ free up the temporary array -------//

	mem_del( firstFrameArray );
}
//--------- End of function FirmRes::load_firm_build ---------//

//---------- Begin of function FirmBuild::FirmBuild() -----------//

FirmBuild::FirmBuild()
{
	// backup res_bitmap
	char backupRes[sizeof(res_bitmap)];
	memcpy( backupRes, &res_bitmap, sizeof(res_bitmap) );

	memset(this, 0, sizeof(FirmBuild) );

	// restore res_bitmap
	memcpy( &res_bitmap, backupRes, sizeof(res_bitmap) );
}
//------------ End of function FirmBuild::FirmBuild() -----------//

//---------- Begin of function FirmBuild::~FirmBuild() -----------//

FirmBuild::~FirmBuild()
{
	res_bitmap.deinit();
	if (palw_ptr)
		mem_del(palw_ptr);
	palw_ptr = NULL;
}
//------------ End of function FirmBuild::~FirmBuild() -----------//


//------- Begin of function FirmBuild::load_bitmap_res -------//

void FirmBuild::load_bitmap_res()
{
	if( ++loaded_count > 1 )		// if bitmaps of this sprite has been loaded
		return;

	//----- open sprite bitmap resource file -------//

	String str;
//	str  = DIR_RES;
//	str += "I_FIRM.RES";
//	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	if (config.building_size == 1)	
		str = DIR_FIRM;
	else
		str = DIR_FIRM_SMALL;
	str += firm_resource_name;
	str += ".FRM";
	res_bitmap.init_imported(str, 1);  // 1-read all into buffer

	//------ create color palette for eight nation ------//
	err_when( palw_ptr );		// should be cleared
	if( pal_ptr )
	{
		palw_ptr = (short *)mem_add( 0x100 * sizeof(short) );
		for( int j = 0; j < 0x100; ++j )
			palw_ptr[j] = vga.make_pixel( j+(RGBColor *)pal_ptr );
	}
}
//-------- End of function FirmBuild::load_bitmap_res -------//


//------- Begin of function FirmBuild::free_bitmap_res -------//

void FirmBuild::free_bitmap_res()
{
	loaded_count--;

	err_when( loaded_count < 0 );

	if( loaded_count==0 )		// if this bitmap is still needed by other sprites
	{
		res_bitmap.deinit();

		if( palw_ptr )
			mem_del( palw_ptr );
		palw_ptr = NULL;
	}
}
//-------- End of function FirmBuild::free_bitmap_res -------//

//------- Begin of function FirmRes::load_firm_info -------//
//
// Read in information of FIRM.DBF into memory array
//
void FirmRes::load_firm_info()
{
	FirmRec  	 *firmRec;
	FirmInfo 	 *firmInfo;
	FirmBuild	 *firmBuild;
	int      	 i;

	//---- read in firm count and initialize firm info array ----//

	Database *dbFirm;

	if (config.building_size == 1)
		dbFirm = game_set.open_db(FIRM_DB);	// only one database can be opened at a time, so we read FIRM.DBF first
	else
		dbFirm = game_set.open_db(FIRM_SMALL_DB);

	firm_count      = (short) dbFirm->rec_count();
	firm_info_array = (FirmInfo*) mem_add( sizeof(FirmInfo)*firm_count );

	memset( firm_info_array, 0, sizeof(FirmInfo) * firm_count );

	//---------- read in FIRM.DBF ---------//

	for( i=0 ; i<firm_count ; i++ )
	{
		firmRec  = (FirmRec*) dbFirm->read(i+1);
		firmInfo = firm_info_array+i;

		m.rtrim_fld( firmInfo->name, firmRec->name, firmRec->NAME_LEN );
		m.rtrim_fld( firmInfo->short_name, firmRec->short_name, firmRec->SHORT_NAME_LEN );

		m.rtrim_fld( firmInfo->overseer_title, firmRec->overseer_title, firmRec->TITLE_LEN );
		m.rtrim_fld( firmInfo->worker_title  , firmRec->worker_title  , firmRec->TITLE_LEN );
		translate.multi_to_win(firmInfo->name, firmInfo->NAME_LEN);
		translate.multi_to_win(firmInfo->short_name, firmInfo->SHORT_NAME_LEN);
		translate.multi_to_win(firmInfo->overseer_title, firmInfo->TITLE_LEN);
		translate.multi_to_win(firmInfo->worker_title, firmInfo->TITLE_LEN);

		firmInfo->firm_id     	 	  = i+1;
		firmInfo->tera_type   	 	  = firmRec->tera_type-'0';

		firmInfo->max_hit_points 	  = m.atoi( firmRec->hit_points, firmRec->HIT_POINTS_LEN );

		firmInfo->first_build_id = m.atoi( firmRec->first_build, firmRec->FIRST_BUILD_LEN );
		firmInfo->build_count	 = m.atoi( firmRec->build_count, firmRec->BUILD_COUNT_LEN );

		firmInfo->first_group_id = m.atoi( firmRec->first_group, firmRec->FIRST_GROUP_LEN );
		firmInfo->group_count	 = m.atoi( firmRec->group_count, firmRec->GROUP_COUNT_LEN );

		firmInfo->need_overseer  = firmInfo->overseer_title[0] && firmInfo->overseer_title[0] != ' ';
		firmInfo->need_worker  	 = firmInfo->worker_title[0]   && firmInfo->worker_title[0]   != ' ';

//		if( firmInfo->firm_id == FIRM_LAIR )			// BUGHERE
//		{
//			firmInfo->need_overseer = 1;
//			firmInfo->need_worker  	= 1;
//		}

		firmInfo->setup_cost		 = m.atoi( firmRec->setup_cost, firmRec->COST_LEN );
		firmInfo->year_cost		 = m.atoi( firmRec->year_cost, firmRec->COST_LEN );
		firmInfo->setup_live_points_cost = m.atoi( firmRec->setup_live_points_cost, firmRec->COST_LEN );
		firmInfo->year_live		 = m.atoi( firmRec->year_live, firmRec->COST_LEN );

		firmInfo->is_linkable_to_town = firmRec->is_linkable_to_town=='1';
		
		firmInfo->live_in_town = firmRec->live_in_town=='1';

		firmInfo->is_military	 = firmRec->is_military == 'Y';

		firmInfo->buildable		 = firmInfo->setup_cost > 0;

		if( firmRec->all_know=='1' )
			memset( firmInfo->nation_tech_level_array, 1, sizeof(firmInfo->nation_tech_level_array) );
//		memset( firmInfo->nation_tech_level_array, 1, sizeof(firmInfo->nation_tech_level_array) );

		//------- set loc_width & loc_height in FirmInfo --------//

		firmBuild = firm_build_array+firmInfo->first_build_id-1;

		firmInfo->loc_width  = firmBuild->loc_width;
		firmInfo->loc_height = firmBuild->loc_height;

		// ------if FirmBuild::name is empty, take the name from firmInfo -----//

		if( firmInfo->first_build_id && firmInfo->build_count > 0 )
		{
			for( int j = firmInfo->first_build_id; j < firmInfo->first_build_id + firmInfo->build_count; ++j )
			{
				FirmBuild *firmBuild = get_build(j);

				firmBuild->firm_id = firmInfo->firm_id;

				if( firmBuild && firmBuild->name[0] == '\0' )		// empty string
					strcpy( firmBuild->name, firmInfo->name );
			}
		}

		if( firm_group_count )		// fill in firm_id of FirmGroup
		{
			for( int j = firmInfo->first_group_id; j < firmInfo->first_group_id + firmInfo->group_count; ++j )
			{
				get_group(j)->firm_id = firmInfo->firm_id;
			}
		}
		//----- set only_build_race_id -------//

		for( int j=1 ; j<=MAX_MONSTER_TYPE ; j++ )
		{
			if( firmInfo->get_build_id( monster_res[j]->firm_build_code ) )
			{
				if( !firmInfo->only_build_race_id )
					firmInfo->only_build_race_id = -j;
				else                                   // if more than one monster type can build this firm, reset only_build_race_id to zero
				{
					firmInfo->only_build_race_id = 0;
					break;
				}
			}
		}
	}

	//-- the size of camp and fort must be the same as camp can be upgraded to fort --//

	err_when( firm_res[FIRM_CAMP]->loc_width != firm_res[FIRM_FORT]->loc_width ||
				 firm_res[FIRM_CAMP]->loc_height != firm_res[FIRM_FORT]->loc_height );
}
//--------- End of function FirmRes::load_firm_info ---------//


//---------- Begin of function FirmRes::operator[] -----------//

FirmInfo* FirmRes::operator[](int firmId)
{
	err_if( firmId<1 || firmId>firm_count )
		err_now( "FirmRes::operator[]" );

	return firm_info_array+firmId-1;
}
//------------ End of function FirmRes::operator[] -----------//

//-------- Start of function FirmInfo::can_build -------------//
//
// Whether unit of this race can build this firm or not.
//
// <int> unitRecno - check whether this unit knows how to build
//						   this firm.
// [int] checkCash - check if the building nation has enough cash and
//							live points (default: 1)
//
int FirmInfo::can_build(int unitRecno, int checkCash)
{
	Unit* unitPtr = unit_array[unitRecno];

	if( !nation_can_build(unitPtr->nation_recno, unitPtr->race_id, checkCash) )
		return 0;

	//----- see if there is a firm build available for the given race/monster type -----//

	int firmBuildId;

	if( unitPtr->is_monster() )
		firmBuildId = get_build_id( monster_res[unitPtr->monster_id()]->firm_build_code );
	else
		firmBuildId = get_build_id( race_res[unitPtr->race_id]->code );

	if( !firmBuildId )
		return 0;

	//--- see if the unit can build the firm, this is determined by whether the unit is a civilian or a military unit ---//

	if( unitPtr->is_human() )
	{
		// ####### begin Gilbert 6/5 #######//
		if( is_military )    // if this is a military building, which can be built by all units (both civilian and military)
		{
			// except offensive structure which only be built by military unit (reason : space for buttons...)
			if( firm_id >= FIRM_OFFENSIVE_BUILDING_1 && firm_id <= FIRM_OFFENSIVE_BUILDING_4 )
				return !unitPtr->is_civilian();
			else
				return 1;
		}
		// ####### end Gilbert 6/5 #######//
		else						// if this is a civilian building, it can only be built by civilian units.
			return unitPtr->is_civilian();
	}
	else
	{
		return 1;
	}
}
//--------- End of function FirmInfo::can_build ---------------//


//-------- Start of function FirmInfo::nation_can_build -------------//
//
// Whether the given nation can build this firm or not.
//
// <int> nationRecno - recno of the nation.
// <int> raceId		- race of the firm to be built.
// [int] checkCash   - check if the building nation has enough cash and
//							  live points (default: 1)
//
int FirmInfo::nation_can_build(int nationRecno, int raceId, int checkCash)
{
	if( !buildable || !nationRecno || !raceId )
		return 0;

	if( get_nation_tech_level(nationRecno) <= 0 )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	//--- human nation cannot build monster firms before monster firms cost live points ---//

	if( nationPtr->is_human() && raceId<0 )	
		return 0;

	//--- check if the building nation has enough cash and live points ---//
	
	if( checkCash )
	{
		if( nationPtr->cash < setup_cost ||
			 nationPtr->live_points < setup_live_points_cost )
		{
			return 0;
		}
	}

	//----- special unit training structure -----//

	if( firm_id == FIRM_SPECIAL )
	{
		if (raceId < 0)
			return 0;

		err_when( raceId < 1 || raceId > MAX_RACE );

		if( !tech_res[TECH_SPU_STRUCTURE(raceId)]->get_nation_tech_level(nationRecno) )
			return 0;
	}

	//------ Seat of Power ------//

	if( firm_id == FIRM_BASE )	// only if the nation has acquired the myth to build it
	{
		//--- if this nation has acquired the needed scroll of power ---//
		//----- each nation can only build one seat of power -----//
#ifdef DEMO
		return 0;
#else
		if( game.game_mode == GAME_TUTORIAL &&
			nationRecno != nation_array.player_recno )
		{
			return 0;
		}

		if( !( nationPtr->base_count_array[raceId-1] == 0 &&
				 nationPtr->know_base_array[raceId-1] ) )
		{
			return 0;
		}
#endif
	}

	// ###### begin Gilbert 23/12 ########//
	if( firm_id == FIRM_FORTRESS )
	{
		return 0;		// cannot build monster fortress
	}
	// ###### end Gilbert 23/12 ########//

#ifdef DEMO
	if( firm_id == FIRM_OFFENSIVE_BUILDING_1 ||
	 	 firm_id == FIRM_OFFENSIVE_BUILDING_2 ||
		 firm_id == FIRM_OFFENSIVE_BUILDING_3 ||
		 firm_id == FIRM_OFFENSIVE_BUILDING_4 )
	{
		return 0;		// cannot build monster fortress
	}
#endif

	return 1;
}
//--------- End of function FirmInfo::nation_can_build ---------------//

//-------- Start of function FirmInfo::is_linkable_to_firm -------------//
//
int FirmInfo::is_linkable_to_firm(int linkFirmId)
{
	switch(firm_id)
	{
		case FIRM_FACTORY:
			return linkFirmId==FIRM_MINE || linkFirmId==FIRM_MARKET; // || linkFirmId==FIRM_HARBOR;

		case FIRM_MINE:
			return linkFirmId==FIRM_FACTORY || linkFirmId==FIRM_MARKET; // || linkFirmId==FIRM_HARBOR;

		case FIRM_MARKET:
			return linkFirmId==FIRM_MINE || linkFirmId==FIRM_FACTORY; // || linkFirmId==FIRM_HARBOR;

		case FIRM_INN:                   // for an inn to scan for neighbor inns quickly, the link line is not displayed
			return linkFirmId==FIRM_INN;

		case FIRM_LAIR:
      	return linkFirmId==FIRM_LAIR;

//		case FIRM_HARBOR:
//			return linkFirmId==FIRM_MARKET || linkFirmId==FIRM_MINE || linkFirmId==FIRM_FACTORY;

		default:
			return 0;
	}
}
//--------- End of function FirmInfo::is_linkable_to_firm ---------------//


//------ Start of function FirmInfo::default_firm_link_status --------//
//
// return: <int> 1 - the default status is <enable>
//					  0 - the default status is <disable>
//
int FirmInfo::default_firm_link_status(int linkFirmId)
{
	int rc;

	switch(firm_id)
	{
		case FIRM_MINE:
			rc = (linkFirmId!=FIRM_MARKET);
			break;

		case FIRM_FACTORY:
			rc = (linkFirmId==FIRM_MARKET) || (linkFirmId==FIRM_MINE);
			break;

		case FIRM_MARKET:
			rc = (linkFirmId==FIRM_FACTORY); // || (linkFirmId==FIRM_HARBOR);
			break;

//		case FIRM_HARBOR:
//			rc = (linkFirmId==FIRM_MARKET) || (linkFirmId==FIRM_MINE) ||
//				  (linkFirmId==FIRM_FACTORY);
//			break;

		default:
			rc = 1;
	}

	if( rc )
		return LINK_EE;
	else
		return LINK_DD; 
}
//------- End of function FirmInfo::default_firm_link_status -------//


//-------- Start of function FirmInfo::get_build_id -------------//
//
// Give the build code and return the build id.
//
// <char*> buildCode - the build code
//
// return : <int> >0 the firm build id. 
//						==0 if the firm does not have that build code 
//
int FirmInfo::get_build_id (char* buildCode)
{
	//buildCode is the race name

	err_when( build_count<1 );		

	// ###### begin Gilbert 4/11 ######//
//	if( build_count==1 )		// if this firm has only one building type
//		return first_build_id;
	// ###### end Gilbert 4/11 ######//

	int firmBuildId = first_build_id;
	int firmGroupId = first_group_id;
	for( int i=0 ; i<group_count ; i++, firmGroupId++ )              // if this firm has one graphics for each race
	{
		if( strcmp(buildCode, firm_res.get_group(firmGroupId)->race_code)==0 )
			return (firmBuildId + firm_res.get_group(firmGroupId)->group - 1);
	}
	
//	for( i=0 ; i<build_count ; i++, firmBuildId++ )              // if this firm has one graphics for each race
//	{
//		if( strcmp(buildCode, firm_res.get_group(firmBuildId)->build_code)==0 )
//			return firmBuildId;
//	}

	return 0;
}
//--------- End of function FirmInfo::get_build_id ---------------//

//-------- Start of function FirmInfo::get_build_id -------------//
//
// Give the build code and return the build id.
//
// <int> raceId - the build code
//
// return : <int> >0 the firm build id. 
//						==0 if the firm does not have that build code 
//
int FirmInfo::get_build_id (int raceId)
{
	//buildCode is the race name

	err_when( build_count<1 );		

	// ###### begin Gilbert 10/3 ######//
	//if( build_count==1 )		// if this firm has only one building type
	//	return first_build_id;
	// ###### end Gilbert 10/3 ######//

	int firmBuildId = first_build_id;
	int firmGroupId = first_group_id;
	for( int i=0 ; i<group_count ; i++, firmGroupId++ )              // if this firm has one graphics for each race
	{
		if( firm_res.get_group(firmGroupId)->race_id == raceId )
			return (firmBuildId + firm_res.get_group(firmGroupId)->group - 1);
	}
	
//	for( i=0 ; i<build_count ; i++, firmBuildId++ )              // if this firm has one graphics for each race
//	{
//		if( strcmp(buildCode, firm_res.get_group(firmBuildId)->build_code)==0 )
//			return firmBuildId;
//	}

	return 0;
}
//--------- End of function FirmInfo::get_build_id ---------------//

//-------- Start of function FirmInfo::get_race_id -------------//
//
// Give the build code and return the build id.
//
// <char*> buildCode - the build code
//
// return : <int> >0 the firm build id. 
//						==0 if the firm does not have that build code 
//
char FirmInfo::get_race_id (char* buildCode)
{
	//buildCode is the race name

	err_when( build_count<1 );		

	int firmGroupId = first_group_id;
	for( int i=0 ; i<group_count ; i++, firmGroupId++ )              // if this firm has one graphics for each race
	{
		if( strcmp(buildCode, firm_res.get_group(firmGroupId)->race_code)==0 )
			return (firm_res.get_group(firmGroupId)->race_id);
	}
	return 0;
}
//--------- End of function FirmInfo::get_build_id ---------------//


//---- Begin of function FirmInfo::inc_nation_firm_count ----//

void FirmInfo::inc_nation_firm_count(int nationRecno)
{
	err_when( nationRecno<1 || nationRecno>nation_array.size() );

	nation_firm_count_array[nationRecno-1]++;

   nation_array[nationRecno]->total_firm_count++;
}
//----- End of function FirmInfo::inc_nation_firm_count -----//


//---- Begin of function FirmInfo::dec_nation_firm_count ----//

void FirmInfo::dec_nation_firm_count(int nationRecno)
{
	nation_firm_count_array[nationRecno-1]--;

	nation_array[nationRecno]->total_firm_count--;

	err_when( nation_firm_count_array[nationRecno-1] < 0 );

	if( nation_firm_count_array[nationRecno-1] < 0 )		// run-time bug fixing
		nation_firm_count_array[nationRecno-1] = 0;
}
//----- End of function FirmInfo::dec_nation_firm_count -----//


//--------- begin of function FirmBuild::get_color_remap_table ---------//
short* FirmBuild::get_color_remap_table(int nationRecno, int selectedFlag)
{
	if( palw_ptr )
	{
		#define FIRST_REMAP_KEY       0xE0        // the source color code of the colors to be remapped
		#define REMAP_KEY_COUNT       8
		memcpy( palw_ptr+FIRST_REMAP_KEY,
			game.get_color_remap_table(nationRecno, selectedFlag) + FIRST_REMAP_KEY, (sizeof(short) * REMAP_KEY_COUNT) );

		return palw_ptr;
	}

	return game.get_color_remap_table(nationRecno, selectedFlag);
}
//--------- end of function FirmBuild::get_color_remap_table ---------//


//--------- begin of function FirmBitmap::draw_at ---------//

void FirmBitmap::draw_at(int absX, int absY, int absZ, short *colorRemapTable, int displayLayer, FirmBuild* firmBuild)
{

	// -------- skip if display layer is not correct --------//

	if( !(displayLayer & display_layer))
		return;

	char *bitmapPtr = firmBuild->res_bitmap.read_imported(bitmap_ptr);
	world.zoom_matrix->put_bitmap_offset(absX, absY, absZ,
		bitmapPtr, offset_x, offset_y,
		colorRemapTable, 0, 1);
}
// --------- end of function FirmBitmap::draw_at ---------//


// ------- begin of function FirmRes::calc_color_remap_table -------//
//
// make remap table for underconstruction firm 
// return value is also stored in cache_color_remap_table for later access
//
short* FirmRes::calc_color_remap_table(short *srcRemapTable, float completion)
{
	if( completion >= 1.0f )
		return cache_color_remap_table = srcRemapTable;

	static short destRemapTable[256];

	RGBColor rgb;
	unsigned uBrightess = unsigned(completion * 128.0f);
	// ##### begin Gilbert 9/10 #######//
	uBrightess += 102;				// brightness change from 40% to 90%
	// ##### end Gilbert 9/10 #######//

	err_when( uBrightess > 256);

	for( int i = 0xff; i >= 0; --i )
	{
		vga.decode_pixel( srcRemapTable[i], &rgb );
		rgb.red   = BYTE((unsigned)rgb.red   *uBrightess >> 8 );
		rgb.green = BYTE((unsigned)rgb.green *uBrightess >> 8 );
		rgb.blue  = BYTE((unsigned)rgb.blue  *uBrightess >> 8 );
		destRemapTable[i] = vga.make_pixel( &rgb );
	}

	return cache_color_remap_table = destRemapTable;
}
// ##### end Gilbert 21/9 ######//
// ------- end of function FirmRes::calc_color_remap_table -------//

// ###### begin Gilbert 23/10 ######//
// ------- begin of function FirmRes::is_linkable_to_town -------//
//
int FirmRes::is_linkable_to_town( int firmId, int firmRaceId, int townRaceId )
{
	FirmInfo *firmInfo = operator[](firmId);

	return !firmInfo->same_race_worker || firmRaceId == townRaceId;
}
// ------- end of function FirmRes::is_linkable_to_town -------//



// ------- begin of function FirmRes::is_linkable_to_firm -------//
//
int FirmRes::is_linkable_to_firm( int firmId1, int firmRaceId1, int firmNationRecno1, int firmId2, int firmRaceId2, int firmNationRecno2 )
{
	//---- special case: nation lair to independent lair ----//

	if( firmId1==FIRM_LAIR && firmId2==FIRM_LAIR )
	{
		return (firmNationRecno1==0 && firmNationRecno2>0) ||
				 (firmNationRecno1> 0 && firmNationRecno2==0);
	}

	//---------------------------------------------//

	FirmInfo *firmInfo1 = operator[](firmId1);
	FirmInfo *firmInfo2 = operator[](firmId2);

	if( !firmInfo1->is_linkable_to_firm(firmId2) )
		return 0;

	return !(firmInfo1->same_race_worker || firmInfo2->same_race_worker)
		|| firmRaceId1 == firmRaceId2;
}
// ------- end of function FirmRes::is_linkable_to_firm -------//
