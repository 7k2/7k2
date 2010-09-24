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

//Filename    : ORAWRES.H
//Description : Header file of object RawRes

#ifndef __ORAWRES_H
#define __ORAWRES_H

#ifndef __ORES_H
#include <ORES.H>
#endif

#ifndef __ODYNARR_H
#include <ODYNARR.H>
#endif

#include <ORESDB.H>

//------------- define constant --------------//

#define MAX_RAW			 		    3
#define MAX_PRODUCT					 3
#define MAX_RAW_RESERVE_QTY	20000

//------------ define icon size -------------//

enum { RAW_SMALL_ICON_WIDTH=10,
		 RAW_SMALL_ICON_HEIGHT=10,
		 RAW_LARGE_ICON_WIDTH=32,
		 RAW_LARGE_ICON_HEIGHT=32
	  };

//----------- Define raw material types ---------//

enum { RAW_CLAY=1,
		 RAW_COPPER,
		 RAW_IRON,		 };

//------------ Define struct RawRec ---------------//

struct RawRec
{
	enum { NAME_LEN=12, TERA_TYPE_LEN=1, FILE_NAME_LEN=8, BITMAP_PTR_LEN=4, OFFSET_LEN=4, LOC_LEN=2 };

	char name[NAME_LEN];
	char tera_type[TERA_TYPE_LEN];

	char map_icon_file_name[FILE_NAME_LEN];
	char map_loc_width[LOC_LEN];
	char map_loc_height[LOC_LEN];
	char map_icon_offset_x[OFFSET_LEN];
	char map_icon_offset_y[OFFSET_LEN];
	char map_icon_ptr[BITMAP_PTR_LEN];

	char small_raw_file_name[FILE_NAME_LEN];
	char small_raw_ptr[BITMAP_PTR_LEN];
	char small_product_file_name[FILE_NAME_LEN];
	char small_product_ptr[BITMAP_PTR_LEN];

	char large_raw_file_name[FILE_NAME_LEN];
	char large_raw_ptr[BITMAP_PTR_LEN];
	char large_product_file_name[FILE_NAME_LEN];
	char large_product_ptr[BITMAP_PTR_LEN];

	char interface_raw_file_name[FILE_NAME_LEN];
	char interface_raw_ptr[BITMAP_PTR_LEN];
};

//------------- Define struct RawInfo --------------//

struct RawInfo
{
public:
	enum { NAME_LEN=20 };

	char	raw_id;
	char 	name[NAME_LEN+1];
	char  tera_type;

	char*	map_icon_ptr;
	char	map_loc_width;
	char	map_loc_height;
	short	map_icon_offset_x;
	short	map_icon_offset_y;

	char*	small_raw_ptr;
	char* small_product_ptr;

	char*	large_raw_ptr;
	char* large_product_ptr;

	char* interface_raw_ptr;

	DynArray raw_supply_firm_array;
	DynArray product_supply_firm_array;

public:
	RawInfo();

	void 		add_raw_supply_firm(short firmRecno);
	void 		add_product_supply_firm(short firmRecno);

	short		get_raw_supply_firm(short recNo) 		{ return *( (short*) raw_supply_firm_array.get(recNo) ); }
	short		get_product_supply_firm(short recNo) 	{ return *( (short*) product_supply_firm_array.get(recNo) ); }

	void		put_map_bitmap(int curX, int curY, int curZ );
};

//----------- Define class RawRes ---------------//

class RawRes
{
public:
	short			raw_count;
	RawInfo*		raw_info_array;

	char			init_flag;

	ResourceDb	res_map_icon;
	ResourceDb	res_small_raw;
	ResourceDb	res_small_product;
	ResourceDb	res_large_raw;
	ResourceDb	res_large_product;
	ResourceDb	res_interface_raw;
	
public:
	RawRes();

	void 		init();
	void 		deinit();

	void		next_day();
	void		update_supply_firm();

	void  	put_small_product_icon(int x, int y, int rawId);
	void		put_small_raw_icon(int x, int y, int rawId);

	char* 	large_product_icon(int rawId);
	char* 	small_product_icon(int rawId);
	char* 	large_raw_icon(int rawId);
	char* 	small_raw_icon(int rawId);
	char* 	interface_raw_icon(int rawId);

	int  		write_file(File*);
	int  		read_file(File*);

	RawInfo* operator[](int rawId);      // pass rawId  as recno

private:
	void 		load_all_info();
};

extern RawRes raw_res;

//----------------------------------------------------//

#endif
