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

//Filename    : OWORLDMT.H
//Description : Header file for World Matrix WorldMap & WorldZoom

#ifndef __OWORLDMT_H
#define __OWORLDMT_H

#include <omatrix.h>
#include <odynarr.h>
#include <oworld.h>
#include <ofont.h>

//------------- Map window -------------//

#define MAX_MAP_WIDTH	200
#define MAX_MAP_HEIGHT	200

#define MAP_LOC_HEIGHT   1 		// when MAP_VIEW_ENTIRE
#define MAP_LOC_WIDTH    1

#define MAP2_LOC_HEIGHT  2			// when MAP_VIEW_SECTION
#define MAP2_LOC_WIDTH   2

//----------- Zoom window -------------//

#define ZOOM_LOC_X_WIDTH    32     // inc in xLoc, increase in screenX
#define ZOOM_LOC_X_HEIGHT   16     // inc in xLoc, increase in screenY
#define ZOOM_LOC_Y_WIDTH   -32     // inc in yLoc, increase in screenX
#define ZOOM_LOC_Y_HEIGHT   16     // inc in yLoc, increase in screenY
#define ZOOM_Z_WIDTH         0     // inc in z, increase in screenX
#define ZOOM_Z_HEIGHT       -1     // inc in y, increase in screenY

#define ZOOM_X_SHIFT_COUNT  5    // x>>5 = xLoc
#define ZOOM_Y_SHIFT_COUNT  5    // y>>5 = yLoc

//---------- define map modes -----------//

#define MAP_MODE_COUNT  3

enum { MAP_MODE_TERRAIN=0,
		 MAP_MODE_POWER,
		 MAP_MODE_TRADE,
		 MAP_MODE_SPOT,
		 MAP_MODE_ALTITUDE,		// not included in MAP_MODE_COUNT
	  };

//-------- Define class MapMatrix -------//

class MapMatrix : public Matrix
{
public:
	char  last_map_mode;
	char	map_mode;
	char	power_mode;		// 1-also display power regions on the zoom map, 0-only display power regions on the mini map
	char	zoom_map_building_disappear;

	char	filter_object_flag;
	char	filter_object_type;	// OBJ_UNIT/OBJ_FIRM/OBJ_TOWN
	short	filter_object_para;
   char	filter_object_locked;
	char	filter_nation_flag;
	char	filter_nation_recno;
	char	filter_nation_locked;

public:
	MapMatrix();
   ~MapMatrix();

	void init_para();
	void draw();
   void paint();
   void disp();
   void draw_square();
   int  detect();
	void toggle_map_mode(int modeId);

	int get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);
	// if outside the screen, return 0

	int get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);
	// if too far outside the screen return 0

protected:
	void draw_map();
	int  detect_area();

	void set_filter_para();
	void disp_mode_button(int putFront=0);
};

//-------- Define class ZoomMatrix -------//

class ZoomMatrix : public Matrix
{
public:
	DynArray land_disp_sort_array;     // an array for displaying objects in a sorted order
	DynArray air_disp_sort_array;
	DynArray land_top_disp_sort_array;
	DynArray land_bottom_disp_sort_array;

	int	init_rain;
	int	rain_channel_id;
	int	wind_channel_id;
	int	fire_channel_id;
	int	last_fire_vol;
	int	init_lightning; // reset on new game, save on save game
	int	init_snow;
	int	init_magic;
	int	init_magic2;
	short	last_brightness;
	int	vibration; // reset on new game, save on save game
	short	lightning_x1, lightning_y1, lightning_x2, lightning_y2; // save on save game

	BYTE *last_draw_state;		// one byte per location, 0=blacken, 1=fog, 2=fully visible

public:
	ZoomMatrix();
	~ZoomMatrix();

	void init_para();
	void draw();
	void disp();
	void draw_frame();
	void scroll(int,int);
	void draw_white_site();
	void put_bitmap_clip(int x, int y, char* bitmapPtr,int compressedFlag=0);
	void put_bitmap_remap_clip(int x, int y, char* bitmapPtr, short* colorRemapTable=NULL,int compressedFlag=0);
	int  detect_bitmap_clip(int x, int y, char* bitmapPtr);

	// ------- new for isometric view -------//
	int	calc_zoom_x(int curX, int curY, int curZ);
	int	calc_zoom_y(int curX, int curY, int curZ);
	static int	calc_abs_x(int curX, int curY, int curZ);
	static int	calc_abs_y(int curX, int curY, int curZ);
	int	get_base_x();	// calc_abs_x() == calc_zoom_x() + get_base_x()
	int	get_base_y();	// calc_abs_y() == calc_zoom_y() + get_base_y()
	int	get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);  // if outside the screen, return 0
	int	get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);

	void 	get_zoom_pos(int locX, int locY, int& xPoint, int& yPoint);

	void	put_bitmap_offset(int curX, int curY, int curZ, char *bitmapPtr,
		int offsetX, int offsetY,
		short *colorRemapTable=NULL, int hMirror=0, int compressedFlag=1);

	void	put_bitmap(int curX, int curY, int curZ, char *bitmapPtr,
		short *colorRemapTable=NULL, int hMirror=0, int compressedFlag=1)
	{
		put_bitmap_offset(curX, curY, curZ, bitmapPtr, 0, 0, colorRemapTable, hMirror, compressedFlag);
	}

	void	put_bitmapW_offset(int curX, int curY, int curZ, short *bitmapPtr,
			int offsetX, int offsetY, int hMirror=0, int compressedFlag=0);

	void	put_bitmapW(int curX, int curY, int curZ, short *bitmapPtr,
		int hMirror=0, int compressed=0)
	{
		put_bitmapW_offset(curX, curY, curZ, bitmapPtr, 0, 0, hMirror, compressed);
	}

	// reverse of calc_abs_x/y when z=0
	static	int calc_cur_x(int absX, int absY, int curZ=0);
	static	int calc_cur_y(int absX, int absY, int curZ=0);
	void put_center_text(int curX, int curY, int curZ, const char* str, char black = 1,
		Font* font_ptr = &font_zoom);

protected:
	void init_var();

	void draw_objects();
	// ##### begin Gilbert 17/9 ######//
	void draw_objects_now(DynArray* unitArray, int = 0, char = 0);
	// ##### end Gilbert 17/9 ######//

	void draw_weather_effects();

	void draw_build_marker();
	void draw_god_cast_range();

	void blacken_unexplored();
	void blacken_fog_of_war();

	void disp_rally_point();
	void disp_text();
	void draw_magic_effects();
};

//------------------------------------------------//

#endif
