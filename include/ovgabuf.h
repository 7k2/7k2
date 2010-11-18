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

//Filename    : VGABUF.H
//Description : Header file for class VgaBuffer, Direct Draw Buffer.

#ifndef __VGABUF_H
#define __VGABUF_H

#include <imgfun.h>
// #include <ddraw.h>

typedef void *LPVOID;

//-------- Define class VgaBuf ----------------//

class File;
class Bitmap;
class BitmapW;

class VgaBuf
{
public:
	union
	{
#ifdef __DDRAW_INCLUDED__
		LPDIRECTDRAWSURFACE4  dd_buf;
#endif
		LPVOID					vptr_dd_buf;
	};

	union
	{
#ifdef __DDRAW_INCLUDED__
		DDSURFACEDESC2			buf_des;
#endif
		char					c_buf_des[0x100];
	};

	BOOL						buf_locked;			// whether the and back buffers have been locked or not.
   short*					cur_buf_ptr;
	long						cur_pitch;			// buf_des.lPitch
	char						is_front;			// whether it's the front buffer or not

	// ------- temp_unlock --------- //
	// char						save_locked_flag;
	UCHAR						lock_stack_count;
	WORD						lock_bit_stack;

	//--------- back buffer ----------//

	short* buf_ptr()					{ return cur_buf_ptr; }
	short* buf_ptr(int x, int y)	{ return (short *)((char *)cur_buf_ptr + cur_pitch*y) + x; }

	// pitch in pixel
	int 	buf_pitch()					{ return cur_pitch >> 1; }		// in no. of pixel

	// pitch in byte
	int 	buf_true_pitch()			{ return cur_pitch; }

	int	buf_size();
	int   buf_width();
	int   buf_height();

	//---- GUI colors -----//

	static char color_light;      // color of the light panel side
	static char color_dark;       // color of the dark panel side
	static char color_up;         // color of up_panel surface
	static char color_down;       // color of down_panel surface
	static char color_push;       // color of pushed button surface
	static char color_border;     // color of color

	static short *default_remap_table;				// new for 16-bit
	static short *default_blend_table;				// new for 16-bit

public:
	VgaBuf();
	~VgaBuf();

	//---------- system functions ----------//

	void 		init_front(LPVOID dd4Ptr);		// LPDIRECTDRAW4 dd4Ptr
	void 		init_back(LPVOID ddPtr, DWORD =0, DWORD =0, int videoMemoryFlag=0 );	// LPDIRECTDRAW4 dd4Ptr
	void		attach_surface(VgaBuf *);
	void		detach_surface(VgaBuf *);
	void		deinit();

	BOOL		is_buf_lost();
	BOOL		restore_buf();

	void		activate_pal(LPVOID ddPalPtr);	// LPDIRECTDRAWPALETTE ddPalPtr
	DWORD 	color_match(DWORD rgb);		// COLORREF rgb

	void		lock_buf();
	void		unlock_buf();

	void		temp_unlock();
	void		temp_restore_lock();
	void		temp_lock();
	void		temp_restore_unlock();

	void		set_buf_ptr(short* bufPtr, long pitch)	{ cur_buf_ptr = bufPtr; cur_pitch = pitch; }
	void		set_default_buf_ptr();

	int 		write_bmp_file(char* fileName);

	//---------- painting functions ----------//

	static int	translate_color(unsigned char c)		// calc 8-bit color to 16-bit color // new for 16 bit
				{ return default_remap_table[c]; }

	void		bar(int x1,int y1,int x2,int y2,int colorCode);
	void		bar_fast(int x1,int y1,int x2,int y2,int colorCode)
				{ IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, translate_color(colorCode)); } 
	void		barW_fast(int x1,int y1,int x2,int y2,int colorCode)
				{ IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, colorCode); } 
	void		bar_up(int x1,int y1,int x2,int y2)		{ bar( x1, y1, x2, y2, color_up); }
	void		bar_down(int x1,int y1,int x2,int y2)		{ bar( x1, y1, x2, y2, color_down); }
	void		bar_alpha(int x1, int y1, int x2, int y2, int logAlpha, int colorColor );

	void		draw_pixel(int x1,int y1,int colorCode)
				{ *buf_ptr(x1, y1) = translate_color(colorCode); }

	void  	separator(int,int,int,int);
	void		line(int x1,int y1,int x2,int y2,int colorCode);
	void  	thick_line(int x1,int y1,int x2,int y2,int colorCode);

	void  	indicator(int,int,int,int,float,float,int,int= -1);
	void		indicator(int barType, int x,int y,float,float,int colorScheme);
	void  	v_indicator(int,int,int,int,float,float,int,int= -1);

	void  	rect(int,int,int,int,int,int);
	void  	d3_rect(int,int,int,int);

	void  	d3_panel_up(int,int,int,int,int=2,int=1);
	void  	d3_panel_down(int,int,int,int,int=2,int=1);
	void  	d3_panel_up_clear(int,int,int,int,int=2);
	void  	d3_panel_down_clear(int,int,int,int,int=2);

	void  	tile(int,int,int,int,char*);
	void 		adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree);
	void 		draw_d3_up_border(int x1,int y1,int x2,int y2);
	void 		draw_d3_down_border(int x1,int y1,int x2,int y2);

	void 		convert_gray(int x1, int y1, int x2, int y2);

	//-------- buffer saving functions --------//

	short* 	save_area(int,int,int,int,short* =0L);
	void  	rest_area(short*,int=1 );

	void  	save_area_common_buf(int,int,int,int);
	void  	rest_area_common_buf();

	//------- large bitmap displaying functions -------//

	void 		put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable=0);
	void 		put_large_bitmap_trans(int x1, int y1, File* filePtr, short *colorRemapTable=0);
	void 		put_large_bitmapW(int x1, int y1, File* filePtr);


	//---------- assembly bitmap manipulation functions ----------//

	// 8->16 blt

	void		put_bitmap(int x,int y,char* bitmapBuf);

	void		put_bitmap_hmirror( int x, int y, char *bitmapBuf )
				{ IMGbltRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_area(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2)
				{ IMGbltAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_remap(int desX, int desY, char* bitmapBuf, short *colorRemapTable);
	void		put_bitmap_remap_fast( int x, int y, char *bitmapBuf, short *colorRemapTable )
				{ IMGbltRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_remap_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable )
				{ IMGbltRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_area_remap(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable )
				{ IMGbltAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	void		put_bitmap_area_remap_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	// functions with transparent color keying

	void		put_bitmap_trans(int x,int y,char* bitmapBuf);

	void		put_bitmap_trans_fast(int x,int y,char* bitmapBuf)
				{ IMGbltTransRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_trans_hmirror( int x, int y, char *bitmapBuf )
				{ IMGbltTransRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_area_trans( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaTransRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_area_trans_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaTransRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_trans_remap( int x, int y, char *bitmapBuf, short *colorRemapTable )
				{ IMGbltTransRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_trans_remap_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable )
				{ IMGbltTransRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_area_trans_remap( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable )
				{ IMGbltAreaTransRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	void		put_bitmap_area_trans_remap_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaTransRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	// functions with run-length transparent key decompression

	void		put_bitmap_trans_decompress( int x, int y, char *bitmapBuf )
				{ IMGbltTransRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_trans_decompress_hmirror( int x, int y, char *bitmapBuf )
				{ IMGbltTransRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_area_trans_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaTransRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_area_trans_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaTransRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_trans_remap_decompress( int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ IMGbltTransRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_trans_remap_decompress_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ IMGbltTransRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_area_trans_remap_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaTransRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	void		put_bitmap_area_trans_remap_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaTransRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	// functions with run-length transparent key decompression, half alpha

	void		put_bitmap_half_decompress( int x, int y, char *bitmapBuf )
				{ IMGbltHalfRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_half_decompress_hmirror( int x, int y, char *bitmapBuf )
				{ IMGbltHalfRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_remap_table ); }

	void		put_bitmap_area_half_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaHalfRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_area_half_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltAreaHalfRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_remap_table ); }

	void		put_bitmap_half_remap_decompress( int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ IMGbltHalfRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_half_remap_decompress_hmirror( int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ IMGbltHalfRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		put_bitmap_area_half_remap_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaHalfRemapDecompress( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	void		put_bitmap_area_half_remap_decompress_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ IMGbltAreaHalfRemapDecompressHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable ); }

	// functions with blend bitmap

	void		put_bitmap_blend( int x, int y, char *bitmapBuf )
	{ IMGbltBlendRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_blend_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltBlendRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_blend_area( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltBlendAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	void		put_bitmap_blend_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltBlendAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	// functions with weak blend bitmap

	void		put_bitmap_weak_blend( int x, int y, char *bitmapBuf )
	{ IMGbltWeakblendRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_weak_blend_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltWeakblendRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_weak_blend_area( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWeakblendAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	void		put_bitmap_weak_blend_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWeakblendAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	// 16->16 blt

	void		read_bitmapW(int x1,int y1,int x2,int y2, short* bitmapWBuf)
				{ IMGreadW(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, bitmapWBuf); }
	void		read_bitmapW(int x1,int y1,int x2,int y2, BitmapW* bitmapWBuf)
				{ IMGreadW(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, (short *)bitmapWBuf); }

	void		put_bitmapW(int x, int y, short *bitmapWBuf );
	void		put_bitmapW(int x, int y, BitmapW *bitmapWBuf )
				{ put_bitmapW(x, y, (short *)bitmapWBuf ); }
	void		put_bitmapW_fast(int x, int y, short *bitmapWBuf )
				{ IMGbltW( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf ); }
	void		put_bitmapW_fast(int x, int y, BitmapW *bitmapWBuf )
				{ IMGbltW( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf ); }

	void		put_bitmapW_area( int x, int y, short *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWArea( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }
	void		put_bitmapW_area( int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWArea( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }

	// functions with transparent color keying

	void		put_bitmapW_trans( int x, int y, short *bitmapWBuf )
				{ IMGbltWTrans( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf ); }
	void		put_bitmapW_trans( int x, int y, BitmapW *bitmapWBuf )
				{ IMGbltWTrans( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf ); }

	void		put_bitmapW_area_trans( int x, int y, short *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWAreaTrans( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }
	void		put_bitmapW_area_trans( int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWAreaTrans( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }

	// ##### begin Gilbert 20/10 #######//
	// functions with drawing black mask

	void		put_bitmapW_trans_blacken( int x, int y, short *bitmapWBuf )
				{ IMGbltWTransBlacken( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf ); }
	void		put_bitmapW_trans_blacken( int x, int y, BitmapW *bitmapWBuf )
				{ IMGbltWTransBlacken( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf ); }

	void		put_bitmapW_area_trans_blacken( int x, int y, short *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWAreaTransBlacken( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }
	void		put_bitmapW_area_trans_blacken( int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ IMGbltWAreaTransBlacken( cur_buf_ptr, cur_pitch, x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2 ); }

	// ##### end Gilbert 20/10 #######//

	// --------- VgaBuf to VgaBuf copy ------------ //

	void		blt_buf( VgaBuf *srcBuf, int x1, int y1 );
	// put whole srcBuf at (x1, y1) of this Vgabuf

	void		blt_buf_fast( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2)
				{ IMGcopyW( cur_buf_ptr, cur_pitch, srcBuf->cur_buf_ptr, srcBuf->cur_pitch, srcX1, srcY1, srcX2, srcY2); }
	// cut (srcX1,srcY1)-(srcX2,srcY2) of srcBuf and put at (srcX1,srcY1) of this VgaBuf

	void		blt_buf_bright( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2, int brightness);

	void		join_trans_remap( VgaBuf *srcBuf, int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ IMGjoinTransRemap( cur_buf_ptr, cur_pitch, srcBuf->cur_buf_ptr, srcBuf->cur_pitch, x, y, bitmapBuf, colorRemapTable ); }

	void		join_trans( VgaBuf *srcBuf, int x, int y, char *bitmapBuf )
				{ IMGjoinTransRemap( cur_buf_ptr, cur_pitch, srcBuf->cur_buf_ptr, srcBuf->cur_pitch, x, y, bitmapBuf, default_remap_table ); }
};

extern VgaBuf vga_front, vga_back, vga_true_front;

//--------------------------------------------//

#endif
