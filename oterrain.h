//Filename    : OTERRAIN.H
//Description : Header file of object TerrainRes
//Ownership   : Gilbert

#ifndef __OTERRAIN_H
#define __OTERRAIN_H

#include <ORESDB.H>
#define NULL 0


// ------- define constant --------//

#define MAX_BRIGHTNESS_ADJUST_DEGREE	32

//---------- Define terrain type id. ----------//

enum { TOTAL_TERRAIN_TYPE=4 };
enum { MAX_GEN_TERRAIN_TYPE=3 };		// map generator will not generate light dirt or above

enum TerrainTypeCode
{
//	TERRAIN_OCEAN = 1,					// abbrev = 'S'
	TERRAIN_DARK_GRASS= 1,					// 'G'
	TERRAIN_LIGHT_GRASS,					// 'F'
	TERRAIN_DARK_DIRT,					// 'D' - hill
	TERRAIN_ROAD,							// 'R'
};

enum SubTerrainMask
{
	BOTTOM_MASK = 1,											// abbrev = '-'
	MIDDLE_MASK = 2,											// '0'
	TOP_MASK = 4,												// '+'
	NOT_TOP_MASK = BOTTOM_MASK | MIDDLE_MASK,			// 'B'
	NOT_BOTTOM_MASK = MIDDLE_MASK | TOP_MASK,			// 'A'
	ALL_MASK = BOTTOM_MASK | MIDDLE_MASK | TOP_MASK	// '*'
};

enum TerrainFlag
{
	TERRAIN_FLAG_SNOW = 1,
};


enum { TERRAIN_TILE_WIDTH  = 64,
		 TERRAIN_TILE_HEIGHT = 64,
		 TERRAIN_TILE_X_MASK = 63,
		 TERRAIN_TILE_Y_MASK = 63,
	  };

//------------ Define struct TerrainRec ---------------//

struct TerrainRec
{
	enum { TYPE_CODE_LEN=2, FILE_NAME_LEN=8, BITMAP_PTR_LEN=4, PATTERN_ID_LEN=2 };

	char nw_type_code[TYPE_CODE_LEN];
	char ne_type_code[TYPE_CODE_LEN];
	char sw_type_code[TYPE_CODE_LEN];
	char se_type_code[TYPE_CODE_LEN];

	char extra_flag;
	char special_flag;
	char represent_type;
	char secondary_type;
	char pattern_id[PATTERN_ID_LEN];

	char file_name[FILE_NAME_LEN];
	char bitmap_ptr[BITMAP_PTR_LEN];
};

//------------- Define struct TerrainInfo --------------//

struct TerrainInfo
{
	char 	nw_type, nw_subtype;
	char 	ne_type, ne_subtype;
	char 	sw_type, sw_subtype;
	char 	se_type, se_subtype;

	char	average_type;
	char 	extra_flag;
	char	special_flag;
	char	secondary_type;
	char	lower_type;
	char	higher_type;
	char	pattern_id;

	// int 	is_coast() { return 0; }

	char  alternative_count_with_extra;  	  // no. of alternative bitmaps of this terrain
	char  alternative_count_without_extra;   // no. of alternative bitmaps of this terrain
	unsigned char	flags;

	char* bitmap_ptr;
	int	anim_frames;
	char** anim_bitmap_ptr;

	char *get_bitmap(unsigned frameNo);
	int	can_snow()						{ return flags & TERRAIN_FLAG_SNOW; }
};

//------------ Define struct TerrainType ---------------//

struct TerrainType
{
	short	first_terrain_id;
	short	last_terrain_id;
	short min_height;
	short first_texture_id;
};

// field related to terrain pattern substitution
//----------- Define struct TerrainSubRec --------//

struct TerrainSubRec
{
	enum { SUB_NO_LEN=4, PATTERN_ID_LEN=2, DIRECTION_LEN=2, STEP_ID_LEN=2, SEC_ADJ_LEN=2};
	char sub_no[SUB_NO_LEN];
	char step_id[STEP_ID_LEN];
	char old_pattern_id[PATTERN_ID_LEN];
	char new_pattern_id[PATTERN_ID_LEN];
	char sec_adj[SEC_ADJ_LEN];
	char post_move[DIRECTION_LEN];
};

//---------- Define struct TerrainSubInfo --------//

struct TerrainSubInfo
{
	short sub_no;
	short	step_id;
	char	old_pattern_id;
	char	new_pattern_id;
	char	sec_adj;
	char	post_move;
	TerrainSubInfo *next_step;
};

//----------- Define struct TerrainAnimRec -------//

struct TerrainAnimRec
{
	enum { FILE_NAME_LEN=8, FRAME_NO_LEN=2, BITMAP_PTR_LEN=4 };
	char	base_file[FILE_NAME_LEN];
	char	frame_no[FRAME_NO_LEN];
	char	next_frame[FRAME_NO_LEN];
	char	filename[FILE_NAME_LEN];
	char	bitmap_ptr[BITMAP_PTR_LEN];
};

// ------- Define struct TerrainTextureRec ----------//

struct TerrainTextureRec
{
	enum { CELL_ID_LEN = 2, FILE_NAME_LEN = 8, BITMAP_PTR_LEN = 4 };

	char type_code;
	char cell_id[CELL_ID_LEN];
	char file_name[FILE_NAME_LEN];
	char bitmap_ptr[BITMAP_PTR_LEN];
};

// ------- Define struct TerrainTextureInfo ----------//

struct TerrainTextureInfo
{
	char type_code;
	unsigned char cell_id;
	char *bitmap_ptr;
};

//----------- Define class TerrainRes ---------------//

class ColorTable;

class TerrainRes
{
public:
	short			 terrain_count;
	TerrainInfo* terrain_info_array;
	char* 		 file_name_array;

	TerrainType	 terrain_type_array[TOTAL_TERRAIN_TYPE];
	short			 nw_type_min[TOTAL_TERRAIN_TYPE];
	short			 nw_type_max[TOTAL_TERRAIN_TYPE];

	//----- field related to terrain pattern substitution -----//
	short			 ter_sub_rec_count;
	short			 ter_sub_index_count;
	TerrainSubInfo* ter_sub_array;
	TerrainSubInfo** ter_sub_index;

	//------ field related to terrain texture ----------//
	short			 texture_count;
	TerrainTextureInfo* texture_array;

	//------ field related to palette ---------//
	ColorTable*	 vga_color_table;


	char	   	 init_flag;
	ResourceDb	 res_bitmap;
	ResourceDb   anm_bitmap;
	ResourceDb	 tex_bitmap;

public:
	TerrainRes();

	void 		 	 init();
	void 		 	 deinit();

	int 			 get_tera_type_id(char* teraTypeCode);
	char*			 get_map_tile(int terrainId);

	int 			 scan(int nwType, int nwSubType, int neType, int neSubType,
							int swType, int swSubType, int seType, int seSubType,
							int firstInstance=0, int includeExtra=0, int special=0);

	int			 scan(int primaryType, int secondaryType, int patternId,
							int firstInstance=0, int includeExtra=0, int special=0);

	//---- function related to terrain pattern substitution ----//

	int			 search_pattern(int nwPatternId, TerrainSubInfo **resultArray,
										 int maxResult);

	//---------- define code conversion function -------//

	static TerrainTypeCode terrain_code(char);
	static SubTerrainMask terrain_mask(char);
	static int terrain_height(int height, int* =NULL);
	static short min_height(TerrainTypeCode, SubTerrainMask = BOTTOM_MASK);
	static short max_height(TerrainTypeCode, SubTerrainMask = TOP_MASK);
	static short min_height(int , int = BOTTOM_MASK);
	static short max_height(int , int = TOP_MASK);

	//-------------------------------------------------//

	#ifdef DEBUG
		TerrainInfo* operator[](int terrainId);
	#else
		TerrainInfo* operator[](int terrainId) 	{ return terrain_info_array+terrainId-1; }
	#endif

	TerrainTextureInfo *get_texture(char terrainType, unsigned char cellId);
	static char *mask_texture(char *maskPtr, char *tex1, char *tex2, char *outTex=NULL);

private:
	void 		    load_info();
	void			 load_sub_info();
	void			 load_anim_info();
	void			 load_tex_info();
	void			 load_pal();
};

extern TerrainRes terrain_res;

//----------------------------------------------------//

#endif
