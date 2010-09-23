//Filename    : OPLANT.CPP
//Description : Plant resource object
//Owner       : Gilbert

#include <OSYS.H>
#include <OVGA.H>
#include <OGAMESET.H>
#include <OWORLD.H>
#include <OTERRAIN.H>
#include <OINFO.H>
#include <OPLANT.H>
#include <OCONFIG.H>
#include <OWORLDMT.H>
#include <OCOLTBL.H>
#include <OBITMAP.H>

//---------- #define constant ------------//

//#define PLANT_DB   		"PLANT"
//#define PLANT_BITMAP_DB "PLANTBMP"

//------- Begin of function PlantRes::PlantRes -----------//

PlantRes::PlantRes()
{
	init_flag=0;
}
//--------- End of function PlantRes::PlantRes -----------//


//---------- Begin of function PlantRes::init -----------//
//
// This function must be called after a map is generated.
//
void PlantRes::init()
{
	deinit();

	//----- open plant material bitmap resource file -------//

	String str;

	str  = DIR_RES;
	// str += "I_PLANT.RES";
	str += "I_PLANT";
	str += config.terrain_set;
	str += ".RES";

	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	//------- load database information --------//

	load_plant_info();
	load_plant_bitmap();

	// ##### begin Gilbert 15/10 ########//
	plant_map_color = (char) V_DARK_GREEN;
	// ##### end Gilbert 15/10 ########//

	init_flag=1;
}
//---------- End of function PlantRes::init -----------//


//---------- Begin of function PlantRes::deinit -----------//

void PlantRes::deinit()
{
	if( init_flag )
	{
		mem_del(plant_info_array);
		mem_del(plant_bitmap_array);
      mem_del(scan_id_array);
		init_flag=0;
	}
}
//---------- End of function PlantRes::deinit -----------//

#if(defined(CHINESE))
//SXM: Prototype
char* StrReplace(char* String,const char* ReplaceWith,const char* ReplaceTo);
//SXM
#endif

//------- Begin of function PlantRes::load_plant_info -------//
//
void PlantRes::load_plant_info()
{
	PlantRec  	 *plantRec;
	PlantInfo 	 *plantInfo;
	int      	 i;

	//---- read in plant count and initialize plant info array ----//

	String plantDbName;
	plantDbName = DIR_RES;
	plantDbName += "PLANT";
	plantDbName += config.terrain_set;
	plantDbName += ".RES";
	Database plantDbObj(plantDbName, 1);
	// Database *dbPlant = game_set.open_db(PLANT_DB);	
	Database *dbPlant = &plantDbObj;

	plant_count      = (short) dbPlant->rec_count();
	plant_info_array = (PlantInfo*) mem_add( sizeof(PlantInfo)*plant_count );

	memset( plant_info_array, 0, sizeof(PlantInfo) * plant_count );

	//---------- read in PLANT.DBF ---------//

	for( i=0 ; i<plant_count ; i++ )
	{
		plantRec  = (PlantRec*) dbPlant->read(i+1);
		plantInfo = plant_info_array+i;

		m.rtrim_fld( plantInfo->code, plantRec->code, plantRec->CODE_LEN );
#if(defined(CHINESE))
		// so scenario editor can display Chinese word of "tree"
		//SXM:Risk
#if(defined(TRADITIONAL))
		StrReplace(plantInfo->code,"TREE","¾ð");
#else
		StrReplace(plantInfo->code,"TREE","Ê÷");
#endif
		//SXM
#endif
		plantInfo->climate_zone = m.atoi( plantRec->climate_zone, plantRec->ZONE_LEN );

		if( plantRec->tera_type1[0] == 'T' )		// town plant
		{
			plantInfo->tera_type[0] = 'T';
		}
		else
		{
			if(plantRec->tera_type1[0] != ' ')
				plantInfo->tera_type[0] = terrain_res.get_tera_type_id( plantRec->tera_type1 );
			else
				plantInfo->tera_type[0] = 0;
		}

		if( plantRec->tera_type2[0] == 'T' )		// town plant
		{
			plantInfo->tera_type[1] = 'T';
		}
		else
		{
			if(plantRec->tera_type2[0] != ' ')
				plantInfo->tera_type[1] = terrain_res.get_tera_type_id( plantRec->tera_type2 );
			else
				plantInfo->tera_type[1] = 0;
		}

		if( plantRec->tera_type3[0] == 'T' )		// town plant
		{
			plantInfo->tera_type[2] = 'T';
		}
		else
		{
			if(plantRec->tera_type3[0] != ' ')
				plantInfo->tera_type[2] = terrain_res.get_tera_type_id( plantRec->tera_type3 );
			else
				plantInfo->tera_type[2] = 0;
		}

		plantInfo->first_bitmap = m.atoi( plantRec->first_bitmap, plantRec->FIRST_BITMAP_LEN );
		plantInfo->bitmap_count = m.atoi( plantRec->bitmap_count, plantRec->BITMAP_COUNT_LEN );
	}
}
//--------- End of function PlantRes::load_plant_info ---------//


//------- Begin of function PlantRes::load_plant_bitmap -------//
//
void PlantRes::load_plant_bitmap()
{
	PlantBitmapRec  *plantBitmapRec;
	PlantBitmap     *plantBitmap;
	int      		i;
	long				bitmapOffset;

	String plantDbName;
	plantDbName = DIR_RES;
	plantDbName += "PLANTBM";
	plantDbName += config.terrain_set;
	plantDbName += ".RES";
	Database plantDbObj(plantDbName, 1);
	// Database 		*dbPlantBitmap = game_set.open_db(PLANT_BITMAP_DB);
	Database *dbPlantBitmap = &plantDbObj;

	plant_bitmap_count = (short) dbPlantBitmap->rec_count();
	plant_bitmap_array = (PlantBitmap*) mem_add( sizeof(PlantBitmap)*plant_bitmap_count );

	scan_id_array = (short*) mem_add( sizeof(short)*plant_bitmap_count );

	//-------- read in PLANTBMP.DBF -------//

	memset( plant_bitmap_array, 0, sizeof(PlantBitmap) * plant_bitmap_count );

	for( i=0 ; i<plant_bitmap_count ; i++ )
	{
		plantBitmapRec = (PlantBitmapRec*) dbPlantBitmap->read(i+1);
		plantBitmap    = plant_bitmap_array+i;

		plantBitmap->size = m.atoi( plantBitmapRec->size, plantBitmapRec->SIZE_LEN );

		memcpy( &bitmapOffset, plantBitmapRec->bitmap_ptr, sizeof(long) );

		plantBitmap->bitmap_ptr 	= res_bitmap.read_imported(bitmapOffset);
		plantBitmap->bitmap_width  = *((short*)plantBitmap->bitmap_ptr);
		plantBitmap->bitmap_height = *(((short*)plantBitmap->bitmap_ptr)+1);

		plantBitmap->offset_x = m.atoi( plantBitmapRec->offset_x, plantBitmapRec->OFFSET_LEN );
		plantBitmap->offset_y = m.atoi( plantBitmapRec->offset_y, plantBitmapRec->OFFSET_LEN );

		// modify offset_x/y for 7k2
		plantBitmap->offset_x += -LOCATE_WIDTH/2 - (-ZOOM_LOC_X_WIDTH/2 + -ZOOM_LOC_Y_WIDTH/2);
		plantBitmap->offset_y += -LOCATE_HEIGHT/2 - (-ZOOM_LOC_X_HEIGHT/2 + -ZOOM_LOC_Y_HEIGHT/2);

		if( plantBitmapRec->town_age >= '1' && plantBitmapRec->town_age <= '9' )
			plantBitmap->town_age = plantBitmapRec->town_age-'0';
	}
}
//--------- End of function PlantRes::load_plant_bitmap ---------//


//---------- Begin of function PlantRes::scan -----------//
//
// Check if any plant in resource matches the given criteria and
// return the plant bitmap id. of the matched one.
//
// <int> climateZone - the climate zone where the plant is.
//							  (either ZONE_TROPICAL or ZONE_TEMPERATE)
//							  0 - any climate zone
// <int> teraType    - the terrain type where the plant is.
//							  0 - any terrain type
// <int> townAge 	   - the age level of the town.
//							  0 - any town age
//
// return : <int> plantBitmapId - the id. of the plant bitmap.
//
int PlantRes::scan(int climateZone, int teraType, int townAge)
{
	int 		  	 i, j;
	int 			 matchCount=0;
	PlantInfo* 	 plantInfo = plant_info_array;
	PlantBitmap* plantBitmap = plant_bitmap_array + plantInfo->first_bitmap - 1;

	//-------- scan plant id. ----------//

	for( i=0 ; i<plant_count ; i++, plantInfo++ )
	{
		if( !climateZone || (plantInfo->climate_zone & climateZone) )
		{
			if( !teraType ||
				 plantInfo->tera_type[0] == teraType ||
				 plantInfo->tera_type[1] == teraType ||
				 plantInfo->tera_type[2] == teraType )
			{
				//------ scan plant bitmap ----------//
				plantBitmap = plant_bitmap_array + plantInfo->first_bitmap - 1;
				for( j=0 ; j<plantInfo->bitmap_count ; j++, plantBitmap++ )
				{
					if( !townAge || plantBitmap->town_age == townAge || plantBitmap->town_age=='*' )		// * = wildcard type, could apply to any town age level
					{
						scan_id_array[matchCount++] = plantInfo->first_bitmap + j;

						err_when( matchCount > plant_bitmap_count );
					}
				}
			}
		}
	}

	//--- pick one from those plants that match the criteria ---//

	if( matchCount > 0 )
	{
		int plantBitmapId = scan_id_array[m.random(matchCount)];

		err_when( plantBitmapId < 1 || plantBitmapId > plant_bitmap_count );

		return plantBitmapId;
	}
	else
		return 0;
}
//---------- End of function PlantRes::scan -----------//


//---------- Begin of function PlantRes::plant_recno ----------//

short PlantRes::plant_recno(short bitmapId)
{
	int i;
	PlantInfo	*plantInfo = plant_info_array;
	for( i = 0; i<plant_count; ++i, plantInfo++ )
		if( plantInfo->first_bitmap <= bitmapId &&
			bitmapId < plantInfo->first_bitmap+ plantInfo->bitmap_count)
			return i+1;
	return 0;
}
//---------- End of function PlantRes::plant_recno ----------//

#ifdef DEBUG

//---------- Begin of function PlantRes::operator[] -----------//

PlantInfo* PlantRes::operator[](int plantId)
{
	err_if( plantId<1 || plantId>plant_count )
		err_now( "PlantRes::operator[]" );

	return plant_info_array+plantId-1;
}
//------------ End of function PlantRes::operator[] -----------//


//---------- Begin of function PlantRes::get_bitmap -----------//

PlantBitmap* PlantRes::get_bitmap(int bitmapId)
{
	err_if( bitmapId<1 || bitmapId>plant_bitmap_count )
		err_now( "PlantRes::get_bitmap" );

	return plant_bitmap_array+bitmapId-1;
}
//------------ End of function PlantRes::get_bitmap -----------//

#endif 

//------- Begin of function PlantBitmap::draw -----------//
//
// Draw the current plant on the map
//
void PlantBitmap::draw(int xLoc, int yLoc)
{
	//---- get the inner position of the plant inside the location ----//

	Location* locPtr = world.get_loc(xLoc, yLoc);

	int innerX  = locPtr->plant_inner_x();
	int innerY  = locPtr->plant_inner_y();

	world.zoom_matrix->put_bitmap_offset(xLoc*LOCATE_WIDTH+innerX, yLoc*LOCATE_HEIGHT+innerX,
		world.interpolate_z(xLoc*LOCATE_WIDTH+innerX, yLoc*LOCATE_HEIGHT+innerY), bitmap_ptr, 
		offset_x, offset_y,
		NULL, 0, 1);
}
//--------- End of function PlantBitmap::draw -----------//


//------- Begin of function PlantBitmap::draw_at -----------//
//
// Draw the plant on the zoom map, given the exact pixel position to put
// the bitmap.
//
// <int> curX, curY, curZ - the map cooridnate of the plant
//
// Draw the current plant on the map
//
void PlantBitmap::draw_at(int curX, int curY, int curZ, int constructionCompletionPercent )
{
	short *remapTable;

	remapTable = (short *)vga.vga_color_table->get_table(	constructionCompletionPercent * 20 / 100 - 20 );

	short bitmapWidth = ((Bitmap *)bitmap_ptr)->get_width();
	short bitmapHeight = ((Bitmap *)bitmap_ptr)->get_height();
	
	world.zoom_matrix->put_bitmap_offset(curX, curY, curZ, bitmap_ptr, 
		-bitmapWidth/2, -bitmapHeight+15,
		remapTable, 0, 1);
}
//--------- End of function PlantBitmap::draw_at -----------//

