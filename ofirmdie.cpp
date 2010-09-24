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

// Filename    : OFIRMDIE.CPP
// Description : destruting firm


#include <OSTR.H>
#include <OFIRMRES.H>
#include <OFIRMDIE.H>
#include <OGAMESET.H>
#include <OGAME.H>
#include <ORESDB.H>
#include <ALL.H>
#include <OWORLDMT.H>
#include <OVGA.H>
#include <OCOLTBL.H>
#include <OMISC.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OSERES.H>

#define FIRM_BUILD_DB 	"FDBUILD"
#define FIRM_FRAME_DB 	"FDFRAME"
#define FIRM_BITMAP_DB 	"FDBITMAP"


struct FirmDieBitmap : public FirmBitmap
{
	int	bitmap_offset;		// fill bitmap_ptr, width and height before draw()
};

FirmDieRes::FirmDieRes()
{
	firm_build_count = 0;
	firm_bitmap_count = 0;
	firm_build_array = NULL;
	firm_bitmap_array = NULL;
	init_flag = 0;
}

FirmDieRes::~FirmDieRes()
{
	deinit();
}

void FirmDieRes::init()
{
	deinit();

	//----- open firm material bitmap resource file -------//

	String str;

//	str  = DIR_RES;
//	str += "I_FIRMDI.RES";

//	res_bitmap.init_imported(str,0,1);		// 0 - do not read all data into buffer
	str  = DIR_RES;
	str += "PALFIRMD.RES";

	res_pal.init_imported(str,1);		// 1-read all into buffer

	//------- load database information --------//

	load_bitmap_info();		// call load_firm_bitmap() first as load_firm_info() will need info loaded by load_firm_bitmap()
	load_build_info();

   //----------------------------------------//

	init_flag=1;
}

void FirmDieRes::deinit()
{
	if(init_flag)
	{
//		mem_del(firm_build_array);
		delete[] firm_build_array;
		mem_del(firm_bitmap_array);

//		res_bitmap.deinit();
		res_pal.deinit();

		init_flag = 0;
	}
}


//------- Begin of function FirmDieRes::load_firm_bitmap -------//
//
// Read in information of FDBITMAP.DBF into memory array
//
void FirmDieRes::load_bitmap_info()
{
	FirmBitmapRec  *firmBitmapRec;
	FirmDieBitmap	*firmBitmap;
	int      		i;
//	long				bitmapOffset;
	Database 		*dbFirmBitmap = game_set.open_db(FIRM_BITMAP_DB);

	firm_bitmap_count = (short) dbFirmBitmap->rec_count();
	firm_bitmap_array = (FirmDieBitmap*) mem_add( sizeof(FirmDieBitmap)*firm_bitmap_count );

	//------ read in firm bitmap info array -------//

	memset( firm_bitmap_array, 0, sizeof(FirmDieBitmap) * firm_bitmap_count );

	for( i=0 ; i<firm_bitmap_count ; i++ )
	{
		firmBitmapRec = (FirmBitmapRec*) dbFirmBitmap->read(i+1);
		firmBitmap    = firm_bitmap_array+i;

		memcpy( &firmBitmap->bitmap_ptr, firmBitmapRec->bitmap_ptr, sizeof(long) );
	//	memcpy( &bitmapOffset, firmBitmapRec->bitmap_ptr, sizeof(long) );
	//	firmBitmap->bitmap_ptr = NULL;

		firmBitmap->width  	  = m.atoi( firmBitmapRec->width, firmBitmapRec->LOC_LEN );
		firmBitmap->height 	  = m.atoi( firmBitmapRec->height, firmBitmapRec->LOC_LEN );

		firmBitmap->offset_x = m.atoi( firmBitmapRec->offset_x, firmBitmapRec->OFFSET_LEN );
		firmBitmap->offset_y = m.atoi( firmBitmapRec->offset_y, firmBitmapRec->OFFSET_LEN );

		firmBitmap->loc_width  = m.atoi( firmBitmapRec->loc_width , firmBitmapRec->LOC_LEN );
		firmBitmap->loc_height = m.atoi( firmBitmapRec->loc_height, firmBitmapRec->LOC_LEN );
		
		firmBitmap->delay = m.atoi( firmBitmapRec->delay, firmBitmapRec->DELAY_LEN );

		firmBitmap->ani_part = m.atoi( firmBitmapRec->ani_part, firmBitmapRec->FRAME_ID_LEN );
	//	firmBitmap->random_flag = firmBitmapRec->random_flag[0] == 'R' ? 1 : 0;
		firmBitmap->random_flag = firmBitmapRec->random_flag[0];

		firmBitmap->display_layer = firmBitmapRec->layer - '0';

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
//--------- End of function FirmDieRes::load_firm_bitmap ---------//


//------- Begin of function FirmDieRes::load_firm_build -------//
//
// Read in information of FIRM.DBF into memory array
//
void FirmDieRes::load_build_info()
{
	FirmBuildRec *firmBuildRec;
	FirmFrameRec *firmFrameRec;
	FirmBuild 	 *firmBuild;
	FirmBitmap   *firmBitmap;
	int      	 i, j, k, frameRecno, bitmapRecno;
	short			 *firstFrameArray;

	//---- read in firm count and initialize firm info array ----//

	Database *dbFirmBuild = game_set.open_db(FIRM_BUILD_DB);	// only one database can be opened at a time, so we read FIRM.DBF first

	firm_build_count = (short) dbFirmBuild->rec_count();
	firm_build_array = new FirmBuild[firm_build_count];
//	firm_build_array = (FirmBuild*) mem_add( sizeof(FirmBuild)*firm_build_count );
// memset( firm_build_array, 0, sizeof(FirmBuild) * firm_build_count );

	//------ allocate an array for storing firstFrameRecno -----//

	firstFrameArray = (short*) mem_add( sizeof(short) * firm_build_count );

	//---------- read in FDBUILD.DBF ---------//

	for( i=0 ; i<firm_build_count ; i++ )
	{
		firmBuildRec = (FirmBuildRec*) dbFirmBuild->read(i+1);
		firmBuild	 = firm_build_array+i;

		firmBuild->group 	  = m.atoi( firmBuildRec->group, firmBuildRec->GROUP_LEN );
		
		m.rtrim_fld( firmBuild->name, firmBuildRec->name, firmBuild->NAME_LEN );

		m.rtrim_fld( firmBuild->firm_resource_name, firmBuildRec->firm_resource_name, firmBuildRec->PAL_NAME_LEN );

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

		firmBuild->under_construction_bitmap_recno = m.atoi(firmBuildRec->under_construction_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		firmBuild->under_construction_bitmap_count =
			m.atoi(firmBuildRec->under_construction_bitmap_count, firmBuildRec->FRAME_COUNT_LEN);
		firmBuild->idle_bitmap_recno 					 = m.atoi(firmBuildRec->idle_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		firmBuild->ground_bitmap_recno             = m.atoi(firmBuildRec->ground_bitmap_recno, firmBuildRec->BITMAP_RECNO_LEN);
		
		firmBuild->defense_attribute.init(m.atoi(firmBuildRec->sturdiness, firmBuildRec->ATTRIBUTE_LEN), 0);

		firmBuild->bullet_sprite_id = m.atoi(firmBuildRec->bullet_sprite_id, firmBuildRec->BULLET_ID_LEN);
		m.rtrim_fld( firmBuild->bullet_sprite_code, firmBuildRec->bullet_sprite_code, firmBuild->BULLET_SPRITE_LEN );

		err_when( firmBuild->frame_count > MAX_FIRM_FRAME );

		// the following variables are hard coded //
		firmBuild->frame_count = 39;
		firstFrameArray[i] = 1;
	//	firstFrameArray[i] = m.atoi( firmBuildRec->first_frame, firmBuildRec->FIRST_FRAME_LEN );
	}

	//-------- read in FDFRAME.DBF --------//

	Database 	*dbFirmFrame = game_set.open_db(FIRM_FRAME_DB);
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
//--------- End of function FirmDieRes::load_firm_build ---------//

//--------- Begin of function FirmDieRes::get_build ---------//
FirmBuild*	FirmDieRes::get_build(int buildId)
{
	err_when( buildId < 1 || buildId > firm_build_count);
	return firm_build_array+buildId-1;
}
//--------- End of function FirmDieRes::get_build ---------//


//--------- Begin of function FirmDieRes::get_bitmap ---------//
FirmDieBitmap* FirmDieRes::get_bitmap(int bitmapId)
{
	err_when( bitmapId < 1 || bitmapId > firm_bitmap_count);
	return firm_bitmap_array+bitmapId-1;
}
//--------- End of function FirmDieRes::get_bitmap ---------//


//--------- Begin of function FirmDie::init --------//
void FirmDie::init(short firmId, short firmBuildId, short nationRecno,
	short	locX1, short locY1, short locX2, short locY2, short alt)
{
	firm_id = firmId;
	firm_build_id = firmBuildId;
	nation_recno = nationRecno;
	loc_x1 = locX1;
	loc_y1 = locY1;
	loc_x2 = locX2;
	loc_y2 = locY2;
	altitude = alt;
	frame = 1;
}

// add before delete the firm
void FirmDie::init(Firm *firmPtr, short dieFlag, int random)
{
	int x, y;
	firm_id = firmPtr->firm_id;
	firm_build_id = firmPtr->firm_build_id;
	nation_recno = firmPtr->nation_recno;
	loc_x1 = firmPtr->loc_x1;
	loc_y1 = firmPtr->loc_y1;
	loc_x2 = firmPtr->loc_x2;
	loc_y2 = firmPtr->loc_y2;

	parent_loc_x1 = firmPtr->loc_x1;
	parent_loc_y1 = firmPtr->loc_y1;
	parent_loc_x2 = firmPtr->loc_x2;
	parent_loc_y2 = firmPtr->loc_y2;

	altitude = firmPtr->altitude;
//	frame = 1;
	frame_delay_count = 0;
	die_flag = dieFlag;
	start_frame = 0;
	end_frame = 0;
	frame_looping = 0;

	if (die_flag == 6)
		se_res.sound(firmPtr->center_x, firmPtr->center_y, 1, 'F', 0, "DIE" );
	else
		se_res.sound(firmPtr->center_x, firmPtr->center_y, 1, 'F', 0, "A1" );
	
	FirmBuild *firmBuild = firm_die_res.get_build(firm_build_id);
	firmBuild->load_bitmap_res();

	int bitmapCount;
	int firstBitmap;
	FirmBitmap* firmBitmap;
	int frameCount = firmBuild->frame_count;
	for ( int k=1; k <= frameCount; k++ )
	{
		firstBitmap = firmBuild->first_bitmap(k);
		bitmapCount = firmBuild->bitmap_count(k);
		for ( int j=0; j <bitmapCount; j++ )
		{
			firmBitmap = firm_die_res.get_bitmap(firstBitmap + j);
			if( firmBitmap )
			{
				if (firmBitmap->ani_part == die_flag)
				{
					if (start_frame == 0)
					{
						start_frame = k;
						end_frame = k;
						frame_delay_count = firmBitmap->delay;
						if (firmBitmap->random_flag == 'R' || (firmBitmap->random_flag >'0' && firmBitmap->random_flag <='9'))
						{
							x = (loc_x2 - loc_x1 + 1)/firmBitmap->loc_width;
							y = (loc_y2 - loc_y1 + 1)/firmBitmap->loc_height;
							random = random % (x * y);
							loc_x1 = loc_x1 + random % x;
							loc_y1 = loc_y1 + random / x;
							loc_x2 = loc_x1 + firmBitmap->loc_width - 1;
							loc_y2 = loc_y1 + firmBitmap->loc_height - 1;
							//		altitude = world.get_corner( loc_x1, loc_y1 )->get_altitude();
							if (firmBitmap->random_flag != 'R')
								frame_looping = firmBitmap->random_flag - '0';
						}
					}
					else
						end_frame = k;
					break;
				}
			}
		}
	}

	err_when(end_frame < start_frame || end_frame == 0);
		
	frame = start_frame;
}


void FirmDie::pre_process()
{
	//nothing
}

int FirmDie::process()
{
	FirmBuild *firmBuild = firm_die_res.get_build(firm_build_id);

	if( --frame_delay_count < 1 )
//	if( ++frame_delay_count > firmBuild->frame_delay_array[frame-1])
	{
	//	frame_delay_count = 0;
	//	if( ++frame > 	firmBuild->frame_count)
		if( ++frame > 	end_frame)
		{
			if (frame_looping >0)
			{
				frame_looping --;
				frame = start_frame;
				return 0;
			}
			firmBuild->free_bitmap_res();
			return 1;
		}
	}
	return 0;
}

void FirmDie::draw(int displayLayer)
{
	// get ground dirt from firm_res
	FirmBuild* firmBuild = firm_res.get_build(firm_build_id);

	if( firmBuild->ground_bitmap_recno )
	{
//		firm_res.get_bitmap(firmBuild->ground_bitmap_recno)
//			->draw_at(loc_x1*ZOOM_LOC_WIDTH, loc_y1*ZOOM_LOC_HEIGHT, NULL, displayLayer);
	}

	//---------- draw animation now ------------//

	firmBuild = firm_die_res.get_build(firm_build_id);
	FirmDieBitmap* firmBitmap;

	int bitmapRecno, i;
	int firstBitmap = firmBuild->first_bitmap(frame);
	int bitmapCount = firmBuild->bitmap_count(frame);
	

	short* colorRemapTable = firmBuild->get_color_remap_table(nation_recno, 0);

	for( i=0, bitmapRecno=firstBitmap ; i<bitmapCount ; i++, bitmapRecno++ )
	{
		firmBitmap = firm_die_res.get_bitmap(bitmapRecno);

		if( firmBitmap )
		{
		//	char *bitmapPtr;
		//	firmBitmap->bitmap_ptr = bitmapPtr = firm_die_res.res_bitmap.read_imported(firmBitmap->bitmap_ptr);
		//	firmBitmap->width = *(short *)bitmapPtr;
		//	firmBitmap->height= *(1+(short *)bitmapPtr);
			firmBitmap->draw_at(loc_x1 * LOCATE_WIDTH, loc_y1 * LOCATE_HEIGHT, altitude, colorRemapTable, displayLayer, firmBuild);
		}
	}
}

// ------ Begin of function Place::is_stealth -------//
//
int FirmDie::is_stealth()
{
	return config.blacken_map && !world.is_explored_quick( loc_x1, loc_y1, loc_x2, loc_y2 );
}
// ------ End of function Place::is_stealth -------//


FirmDieArray::FirmDieArray() : DynArrayB(sizeof(FirmDie),10, DEFAULT_REUSE_INTERVAL_DAYS)
{
	// nothing
}

FirmDieArray::~FirmDieArray()
{
   deinit();
}

void FirmDieArray::init()
{
	zap();
}

void FirmDieArray::deinit()
{
	// nothing
}

void FirmDieArray::process()
{
	int i, j;

	for( i=1, j=size(); j; --j, ++i)
	{
		if( is_deleted(i) )
			continue;

		FirmDie *firmDiePtr = this->operator[](i);

		if( firmDiePtr->process() )
		{
			del(i);
			continue;
		}
	}
}



int FirmDieArray::add(FirmDie *r)
{
	linkin(r);
	return recno();
}

void FirmDieArray::del(int i)
{
	linkout(i);
}


FirmDie *FirmDieArray::operator[](int recNo)
{
   FirmDie* firmDiePtr = (FirmDie*) get(recNo);

   if( !firmDiePtr || is_deleted(recNo) )
      err.run( "FirmDieArray[] is deleted" );

   return firmDiePtr;
}

int FirmDieArray::is_deleted(int recNo)
{
	FirmDie* firmDiePtr = (FirmDie*) get(recNo);
	if( !firmDiePtr || firmDiePtr->firm_id == 0)
		return 1;
	return 0;
}

