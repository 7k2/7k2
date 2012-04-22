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

// Filename    : OMATRIX2.CPP
// Description : render a location

#include <omatrix.h>
#include <oblob2w.h>
#include <oworldmt.h>
#include <ovga.h>
#include <ocoltbl.h>
#include <oterrain.h>
#include <oconfig.h>
#include <osys.h>
#include <oexpmask.h>


// ------- define constant table ---------//
// 

static short last_draw_state = -1;

// slope_brightness_table must be array, cannot be pointer
static long slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1];

static long full_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{
	 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500,
	 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
	 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300,
	 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200,
	 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000, 0x0000,
	 0x0000, 0x0000, 0x0000,-0x0100,-0x0100,-0x0100,-0x0100,-0x0200,
	-0x0200,-0x0200,-0x0300,-0x0300,-0x0400,-0x0400,-0x0500,-0x0500,
	-0x0600,-0x0700,-0x0700,-0x0800,-0x0900,-0x0a00,-0x0a00,-0x0b00,
	-0x0c00,
	-0x0d00,-0x0e00,-0x0f00,-0x1000,-0x1100,-0x1100,-0x1200,-0x1300,
	-0x1400,-0x1500,-0x1600,-0x1700,-0x1800,-0x1900,-0x1a00,-0x1a00,
	-0x1b00,-0x1c00,-0x1d00,-0x1e00,-0x1e00,-0x1f00,-0x1f00,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
};


static long fog_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{	// minus 600 from slope_brightness_table
	-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,
	-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,
	-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,
	-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,
	-0x0500,-0x0500,-0x0500,-0x0500,-0x0500,-0x0500,-0x0600,-0x0600,
	-0x0600,-0x0600,-0x0600,-0x0700,-0x0700,-0x0700,-0x0700,-0x0800,
	-0x0800,-0x0800,-0x0900,-0x0900,-0x0a00,-0x0a00,-0x0b00,-0x0b00,
	-0x0c00,-0x0d00,-0x0d00,-0x0e00,-0x0f00,-0x1000,-0x1000,-0x1100,
	-0x1200,
	-0x1300,-0x1400,-0x1500,-0x1600,-0x1700,-0x1700,-0x1800,-0x1900,
	-0x1a00,-0x1b00,-0x1c00,-0x1d00,-0x1e00,-0x1f00,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
};

static long dark_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{	// all dark
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
};

static long *slope_tables[3] = { dark_slope_brightness_table, fog_slope_brightness_table, full_slope_brightness_table };

#define SEA_COLOR 0xf0


// ---------- define static variable used in rendering --------- //

static long	c00, c10, c01, c20, c11, c02, c30, c21, c12, c03;

// row a
static long rowaZ, rowaDuZM2, rowaDuuZM2, DuuuZM4D3;
static long rowaDvZM2, rowaDvvZM2, DvvvZM4D3, 		// for increase rowaZ
	rowaDuvZM4, DuvvZM4,			// for increase rowaDuZM2
	DuuvZM4;	// for increase rowaDuuZM2

// row b
static long rowbZ, rowbDuZM2, rowbDuuZM2;
static long rowbDvZM2, rowbDvvZM2,					// for increase rowbZ
	rowbDuvZM4;						// for increase rowbDuZM2

// variable for inner loop
// row a
static long aZ, aDuZM2, aDuuZM2;
// row b
static long bZ, bDuZM2, bDuuZM2;

// loop counters
static long vCount, uCount;

// remap table
static unsigned short *remapTables;

// for sea square
static short seaAltitude;
static long seaColor;


// Source of rotl and rotr functions:
//   http://en.wikipedia.org/wiki/Circular_shift
unsigned int rotl(const unsigned int value, int shift)
{
	if( (shift &= sizeof(value) * 8 - 1) == 0 )
		return value;
	return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

unsigned int rotr(const unsigned int value, int shift)
{
	if( (shift &= sizeof(value) * 8 - 1) == 0 )
		return value;
	return (value >> shift) | (value << (sizeof(value) * 8 - shift));
}

long Location::evaluate_z(short x, short y)
{
	long z = ((c30 * x + c20) * x + c10) * x;
	z += ((c03 * y + c02) * y + c01) * y;
	z += ((c21 * x + c12 * y) + c11) * x * y;
	z = (z + c00) >> C_MULTIPLIER_SHIFT;

	return z;
}


void LocationCorners::render(Blob2DW *blob, char *texturePtr, BYTE drawState)
{
	blob->resize(ZOOM_LOC_Y_WIDTH, loc_ptr->min_y, ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH, loc_ptr->max_y-loc_ptr->min_y+1 );

	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// set slope_brightness_table
	if( last_draw_state != drawState )
	{
		switch( drawState )
		{
		case FULL_DARK_MASK_ID:
			memcpy( slope_brightness_table, dark_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case HALF_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, fog_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case FULL_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, full_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		default:
			err_here();	// call render_fog instead of render
		}
		last_draw_state = drawState;
	}

	// remapTables = vga.vga_color_table->get_table();
	remapTables = terrain_res.vga_color_table->get_table();

	short *bufPtr = blob->ptr->bitmap
		+ (-ZOOM_Z_HEIGHT*top_left->altitude - blob->top_edge) * blob->ptr->get_pitch()
		- blob->left_edge;
		// do not use (unsigned char *)blob->p(0, -ZOOM_Z_HEIGHT*top_left->altitude , 0);
		// because it returns NULL when outside boundary

#define bufTruePitch (2*(ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH))
#define bufTruePitchShift 7
	err_when( bufTruePitch != blob->buf_true_pitch() );
	err_when( (1 << bufTruePitchShift) != bufTruePitch );

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

#ifdef ASM_FOR_MSVC
	if( !(loc_ptr->loc_flag & LOCATE_SEA_COAST) )

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_2a:
		// draw row a dot
		 mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		 mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		add	esi, 2

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, aZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch

		sal	edx, bufTruePitchShift
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		neg	edx
		mov	[edi+edx], eax		; // new for 16-bit
		mov	[edi+edx+bufTruePitch], eax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4			; // new for 16-bit
		dec	uCount
		jne	location_render_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_2b:

		; // draw row b dot
		 mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		 mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		add	esi, 2

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, bZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch
		sal	edx, bufTruePitchShift
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		neg	edx
		mov	[edi+edx], eax		; // new for 16-bit
		mov	[edi+edx+bufTruePitch], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4			; // new for 16-bit

		dec	uCount
		jne	location_render_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_1
	}

	else
	{
		seaAltitude = 0;
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;

		seaColor = vga.translate_color(SEA_COLOR) * 0x10001;	// duplicate low word to high word

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_sea_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sea_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2aA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2aB

location_render_sea_2aA:
		mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2aB:
		sal	edx, bufTruePitchShift
		add	esi, 2
		neg	edx
		mov	[edi+edx], eax		; // new for 16-bit
		mov	[edi+edx+bufTruePitch], eax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4		; // new for 16-bit
		dec	uCount
		jne	location_render_sea_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_sea_2b:
		; // draw row b dot
		mov	edx, bZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2bA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2bB

location_render_sea_2bA:
		mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2bB:
		sal	edx, bufTruePitchShift
		add	esi, 2
		neg	edx
		mov	[edi+edx], eax		; // new for 16-bit
		mov	[edi+edx+bufTruePitch], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4		; // new for 16-bit

		dec	uCount
		jne	location_render_sea_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4				; // new for 16-bit
		dec	vCount
		jne	location_render_sea_1
	}		// end _asm
	}
#endif // ASM_FOR_MSVC

#undef bufTruePitch
#undef bufTruePitchShift
#undef MAX_BRIGHTNESS_SHIFT
}




// render special
// effect 1 = color1 and color2 dithered

void LocationCorners::render_special(Blob2DW *blob, int effectId, 
	char *texturePtr, int color1, int color2)
{
	blob->resize(ZOOM_LOC_Y_WIDTH, loc_ptr->min_y, ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH, loc_ptr->max_y-loc_ptr->min_y+1 );

	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// long rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	remapTables = vga.vga_color_table->get_table();

	short *bufPtr = blob->ptr->bitmap
		+ (-ZOOM_Z_HEIGHT*top_left->altitude - blob->top_edge) * blob->ptr->get_pitch()
		- blob->left_edge;
		// do not use (unsigned char *)blob->p(0, -ZOOM_Z_HEIGHT*top_left->altitude , 0);
		// because it returns NULL when outside boundary

#define bufTruePitch (2*(ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH))
#define bufTruePitchShift 7
	err_when( bufTruePitch != blob->buf_true_pitch() );
	err_when( (1 << bufTruePitchShift) != bufTruePitch );

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

	seaAltitude = 0;
	if( loc_ptr->loc_flag & LOCATE_SEA_COAST )
	{
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;
	}

	if( effectId == 1 )
	{
		unsigned short usColor;
		usColor = vga.translate_color(color1);
		color1 = ((unsigned long)usColor << 16) | usColor;
		usColor = vga.translate_color(color2);
		color2 = ((unsigned long)usColor << 16) | usColor;

#ifdef ASM_FOR_MSVC
	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit

location_render_sp1_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sp1_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		mov	eax, color1				; // new for 16-bit
		cmp	dx, seaAltitude
		jg		location_render_sp1_2aA

		movsx	edx, seaAltitude
		;// jmp	location_render_sp1_2aB

location_render_sp1_2aA:
;//location_render_sp1_2aB:
		sal	edx, bufTruePitchShift
		add	esi, 2
		neg	edx
		mov	[edi+edx], eax		; // new for 16-bit
		mov	[edi+edx+bufTruePitch], ax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4		; // new for 16-bit
		dec	uCount
		jne	location_render_sp1_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_sp1_2b:
		; // draw row b dot
		mov	edx, bZ
		sar	edx, C_MULTIPLIER_SHIFT
		mov	eax, color2
		cmp	dx, seaAltitude
		jg		location_render_sp1_2bA

		movsx	edx, seaAltitude
		;// jmp	location_render_sp1_2bB

location_render_sp1_2bA:
;//location_render_sp1_2bB:
		sal	edx, bufTruePitchShift
		add	esi, 2
		neg	edx
		mov	[edi+edx], eax
		mov	[edi+edx+bufTruePitch], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitch + 4	; // new for 16-bit

		dec	uCount
		jne	location_render_sp1_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_sp1_1
	}		// end _asm
#else
		short *tmpBufPtr = bufPtr - 1;
		for( vCount = 0; vCount < LOCATE_HEIGHT / 2; ++vCount )
		{
			aZ = rowaZ;
			aDuZM2 = rowaDuZM2;
			aDuuZM2 = rowaDuuZM2;
			int eax = rowaZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int edx = aZ >> C_MULTIPLIER_SHIFT;
				if( static_cast<unsigned short>(edx) <= seaAltitude )
				{
					edx = seaAltitude;
				}
				edx <<= bufTruePitchShift;
				*(reinterpret_cast<int *>(tmpBufPtr - edx / 2)) = color1;
				*(tmpBufPtr - (edx / 2) + (bufTruePitch / 2)) = color1;

				aZ += aDuZM2 + aDuuZM2 + DuuuZM4D3;
				eax = aZ;
				aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3;
				aDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += (bufTruePitch + 4) / 2;
			}

			bufPtr += bufTruePitch / 2;
			tmpBufPtr = bufPtr - 1;

			bZ = rowbZ;
			bDuZM2 = rowbDuZM2;
			bDuuZM2 = rowbDuuZM2;
			eax = rowbZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int edx = bZ >> C_MULTIPLIER_SHIFT;
				if( static_cast<unsigned short>(edx) <= seaAltitude )
				{
					edx = seaAltitude;
				}
				edx <<= bufTruePitchShift;
				*(reinterpret_cast<int *>(tmpBufPtr - edx / 2)) = color2;
				*(reinterpret_cast<int *>((tmpBufPtr - (edx / 2) + (bufTruePitch / 2)))) = color2;

				bZ += bDuZM2 + bDuuZM2 + DuuuZM4D3;
				eax = bZ;
				bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3;
				bDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += (bufTruePitch + 4) / 2;
			}

			rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3;
			rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3;
			rowaDvvZM2 += 3 * DvvvZM4D3;
			rowaDuZM2 += rowaDuvZM4 + DuvvZM4;
			rowaDuvZM4 += 2 * DuvvZM4;
			rowaDuuZM2 += DuuvZM4;

			rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3;
			rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3;
			rowbDvvZM2 += 3 * DvvvZM4D3;
			rowbDuZM2 += rowbDuvZM4 + DuvvZM4;
			rowbDuvZM4 += 2 * DuvvZM4;
			rowbDuuZM2 += DuuvZM4;

			bufPtr -= 2;
			tmpBufPtr = bufPtr - 1;
		}
#endif
	}		// end if(effectId == 1)
	else
	{
		err_here();
	}


#undef bufTruePitch
#undef bufTruePitchShift
#undef MAX_BRIGHTNESS_SHIFT
}






void LocationCorners::render(BitmapW *bitmapPtr, char *texturePtr, int offsetX, int offsetY, BYTE drawState )
{
	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// set slope_brightness_table
	// set slope_brightness_table
	if( last_draw_state != drawState )
	{
		switch( drawState )
		{
		case FULL_DARK_MASK_ID:
			memcpy( slope_brightness_table, dark_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case HALF_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, fog_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case FULL_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, full_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		default:
			err_here();	// call render_fog instead of render
		}
		last_draw_state = drawState;
	}

	// remapTables = vga.vga_color_table->get_table();
	remapTables = terrain_res.vga_color_table->get_table();

	short *bufPtr = bitmapPtr->get_ptr( offsetX, offsetY );

	const int bufTruePitch = bitmapPtr->get_true_pitch();
	const int negBufTruePitch = -bufTruePitch;
	const int bufTruePitchPlusTwoPixels = bufTruePitch + 2*sizeof(short);

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

#ifdef ASM_FOR_MSVC
	if( !(loc_ptr->loc_flag & LOCATE_SEA_COAST) )

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_2a:
		// draw row a dot
		 mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		 mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		add	esi, 2
		push	esi               ; // save texture pointer
		mov	esi, negBufTruePitch	

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, aZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch

		imul	edx, esi				; // esi is -buffer pitch
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // edx = - z * pitch + buffer pitch (pixel next line)
		mov	[edi+edx], eax		; // new for 16-bit
		pop	esi					; // retrieve texture pointer

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels			; // new for 16-bit
		dec	uCount
		jne	location_render_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_2b:

		; // draw row b dot
		 mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		 mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		add	esi, 2
		push	esi
		mov	esi, negBufTruePitch

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, bZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch
		imul	edx, esi				; // esi is -buffer pitch
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // edx = - z * pitch + buffer pitch (pixel next line)
		mov	[edi+edx], eax		; // new for 16-bit
		pop	esi
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels			; // new for 16-bit

		dec	uCount
		jne	location_render_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_1
	}

	else
	{
		seaAltitude = 0;
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;

		seaColor = vga.translate_color(SEA_COLOR) * 0x10001;	// duplicate low word to high word

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_sea_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sea_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2aA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2aB

location_render_sea_2aA:
		mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2aB:
		imul	edx, negBufTruePitch
		add	esi, 2
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, negBufTruePitch		; // pixel next line
		mov	[edi+edx], eax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit
		dec	uCount
		jne	location_render_sea_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_sea_2b:
		; // draw row b dot
		mov	edx, bZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2bA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2bB

location_render_sea_2bA:
		mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		mov	ecx, remapTables
		mov	ebx, slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2bB:
		imul	edx, negBufTruePitch
		add	esi, 2
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, negBufTruePitch		; // pixel next line
		mov	[edi+edx], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit

		dec	uCount
		jne	location_render_sea_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4				; // new for 16-bit
		dec	vCount
		jne	location_render_sea_1
	}		// end _asm
	}
#else
	if( !(loc_ptr->loc_flag & LOCATE_SEA_COAST) )
	{
		short *tmpBufPtr = bufPtr - 1;
		texturePtr += 4;

		for( vCount = 0; vCount < LOCATE_HEIGHT / 2; ++vCount )
		{
			aZ = rowaZ;
			aDuZM2 = rowaDuZM2;
			aDuuZM2 = rowaDuuZM2;
			int eax = rowaZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int ebx = aDuZM2 >> (C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1);
				ebx = slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE * 2  + ebx];

				eax = (eax & 0xFFFF0000) | *(reinterpret_cast<unsigned short *>(texturePtr));
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr) + 1);
				eax = rotr(eax, 16);
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				eax = rotl(eax, 16);
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr));

				int edx = aZ >> C_MULTIPLIER_SHIFT;
				edx *= negBufTruePitch / 2;
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;
				edx -= negBufTruePitch / 2;
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;

				texturePtr += 2;

				aZ += aDuZM2 + aDuuZM2 + DuuuZM4D3;
				eax = aZ;
				aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3;
				aDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += bufTruePitchPlusTwoPixels / 2;
			}

			bufPtr += bufTruePitch / 2;
			tmpBufPtr = bufPtr - 1;

			bZ = rowbZ;
			bDuZM2 = rowbDuZM2;
			bDuuZM2 = rowbDuuZM2;
			eax = rowbZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int ebx = bDuZM2 >> (C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1);
				ebx = slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE * 2  + ebx];

				eax = (eax & 0xFFFF0000) | *(reinterpret_cast<unsigned short *>(texturePtr));
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr) + 1);
				eax = rotr(eax, 16);
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				eax = rotl(eax, 16);
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr));

				int edx = bZ >> C_MULTIPLIER_SHIFT;
				edx *= negBufTruePitch / 2;
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;
				edx -= negBufTruePitch / 2;
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;

				texturePtr += 2;

				bZ += bDuZM2 + bDuuZM2 + DuuuZM4D3;
				eax = bZ;
				bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3;
				bDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += bufTruePitchPlusTwoPixels / 2;
			}

			rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3;
			rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3;
			rowaDvvZM2 += 3 * DvvvZM4D3;
			rowaDuZM2 += rowaDuvZM4 + DuvvZM4;
			rowaDuvZM4 += 2 * DuvvZM4;
			rowaDuuZM2 += DuuvZM4;

			rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3;
			rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3;
			rowbDvvZM2 += 3 * DvvvZM4D3;
			rowbDuZM2 += rowbDuvZM4 + DuvvZM4;
			rowbDuvZM4 += 2 * DuvvZM4;
			rowbDuuZM2 += DuuvZM4;

			bufPtr -= 2;
			tmpBufPtr = bufPtr - 1;
		}
	}
#endif

#undef MAX_BRIGHTNESS_SHIFT
}



void LocationCorners::render_special(BitmapW *bitmapPtr, int effectId, 
	char *texturePtr, int color1, int color2, int offsetX, int offsetY)
{
	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// long rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	remapTables = vga.vga_color_table->get_table();

	short *bufPtr = bitmapPtr->get_ptr( offsetX, offsetY );

	const int bufTruePitch = bitmapPtr->get_true_pitch();
	const int negBufTruePitch = -bufTruePitch;
	const int bufTruePitchPlusTwoPixels = bufTruePitch + 2*sizeof(short);

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

	seaAltitude = 0;
	if( loc_ptr->loc_flag & LOCATE_SEA_COAST )
	{
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;
	}

	if( effectId == 1 )		// color1 even pixel color, color2 odd pixel color
	{
		unsigned short usColor;
		usColor = vga.translate_color(color1);
		color1 = ((unsigned long)usColor << 16) | usColor;
		usColor = vga.translate_color(color2);
		color2 = ((unsigned long)usColor << 16) | usColor;

#ifdef ASM_FOR_MSVC
	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, negBufTruePitch;
		sub	edi, 2				// new for 16-bit

location_render_sp1_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sp1_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		mov	eax, color1				; // new for 16-bit
		cmp	dx, seaAltitude
		jg		location_render_sp1_2aA

		movsx	edx, seaAltitude
		;// jmp	location_render_sp1_2aB

location_render_sp1_2aA:
;//location_render_sp1_2aB:
		imul	edx, esi
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // pixel next row
		mov	[edi+edx], eax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit
		dec	uCount
		jne	location_render_sp1_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_sp1_2b:
		; // draw row b dot
		mov	edx, bZ
		sar	edx, C_MULTIPLIER_SHIFT
		mov	eax, color2
		cmp	dx, seaAltitude
		jg		location_render_sp1_2bA

		movsx	edx, seaAltitude
		;// jmp	location_render_sp1_2bB

location_render_sp1_2bA:
;// location_render_sp1_2bB:
		imul	edx, esi
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // pixel next row
		mov	[edi+edx], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels	; // new for 16-bit

		dec	uCount
		jne	location_render_sp1_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_sp1_1
	}		// end _asm
#endif
	}		// end if(effectId == 1)
	else if( effectId == 2 )		// 1/2 halftone of color1
	{
		unsigned short usColor;
		usColor = vga.translate_color(color1);
		color1 = ((unsigned long)usColor << 16) | usColor;

#ifdef ASM_FOR_MSVC
	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, negBufTruePitch
		sub	edi, 2				// new for 16-bit

location_render_sp2_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sp2_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		mov	eax, color1				; // new for 16-bit
		cmp	dx, seaAltitude
		jg		location_render_sp2_2aA

		movsx	edx, seaAltitude
		;// jmp	location_render_sp2_2aB

location_render_sp2_2aA:
;//location_render_sp2_2aB:
		imul	edx, esi
		mov	[edi+edx], eax		; // new for 16-bit
		; // no need to write to next row

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit
		dec	uCount
		jne	location_render_sp2_2a
		pop	edi

		; // no row b
		add	edi, bufTruePitch

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // no row b
		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_sp2_1
	}		// end _asm
#endif
	}
	else
	{
		err_here();
	}
}




void LocationCorners::render_fog(BitmapW *bitmapPtr, char *texturePtr, int offsetX, int offsetY, char *fogMask)
{
	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// remapTables = vga.vga_color_table->get_table();
	remapTables = terrain_res.vga_color_table->get_table();

	short *bufPtr = bitmapPtr->get_ptr( offsetX, offsetY );

	const int bufTruePitch = bitmapPtr->get_true_pitch();
	const int negBufTruePitch = -bufTruePitch;
	const int bufTruePitchPlusTwoPixels = bufTruePitch + 2*sizeof(short);

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

#ifdef ASM_FOR_MSVC
	if( !(loc_ptr->loc_flag & LOCATE_SEA_COAST) )

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_2a:
		// draw row a dot
		 mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 

		 ; // begin fog
		mov	ecx, fogMask
		movzx	ecx, byte ptr [ecx]
		add	fogMask, 2			; // add twice as we do two pixels together
		mov	ecx, slope_tables[4*ecx]		// ecx * sizeof( long *)
		mov	ebx, [ecx + MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		mov	ecx, remapTables
		; // end fog

		add	esi, 2
		push	esi               ; // save texture pointer
		mov	esi, negBufTruePitch	
		;

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, aZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch

		imul	edx, esi				; // esi is -buffer pitch
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // edx = - z * pitch + buffer pitch (pixel next line)
		mov	[edi+edx], eax		; // new for 16-bit
		pop	esi					; // retrieve texture pointer

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels			; // new for 16-bit
		dec	uCount
		jne	location_render_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_2b:

		; // draw row b dot
		 mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		 sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 

		 ; // begin fog
		mov	ecx, fogMask
		movzx	ecx, byte ptr [ecx]
		add	fogMask, 2			; // add twice as we do two pixels together
		mov	ecx, slope_tables[4*ecx]		// ecx * sizeof( long *)
		mov	ebx, [ecx + MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		mov	ecx, remapTables
		; // end fog

		add	esi, 2
		push	esi
		mov	esi, negBufTruePitch

		 mov	bl, ah				; // new for 16-bit, do AH first
		 ;mov	bl, 9ch				; // enable this line for grey scale, previous line for textured
		 ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	edx, bZ
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		sar	edx, C_MULTIPLIER_SHIFT
		 rol	eax, 16				; // restore left pixel // new for 16-bit
		 mov	bl, al				; // new for 16-bit
		 ;mov bl, 9ch
		imul	edx, esi				; // esi is -buffer pitch
		 mov	ax, [ecx+ebx*2]	; // new for 16-bit
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, esi				; // edx = - z * pitch + buffer pitch (pixel next line)
		mov	[edi+edx], eax		; // new for 16-bit
		pop	esi
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels			; // new for 16-bit

		dec	uCount
		jne	location_render_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4			; // new for 16-bit
		dec	vCount
		jne	location_render_1
	}

	else
	{
		seaAltitude = 0;
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;

		seaColor = vga.translate_color(SEA_COLOR) * 0x10001;	// duplicate low word to high word

	_asm
	{
		; init outer loop
		mov	vCount, LOCATE_HEIGHT / 2
		mov	edi, bufPtr
		mov	esi, texturePtr
		sub	edi, 2				// new for 16-bit
		add	esi, 4				// skip width height

location_render_sea_1:

		; // init inner loop a
		push	edi
		mov	uCount, LOCATE_WIDTH / 2

		; // set aZ, aDuZM2, aDuuZM2
		mov	eax, rowaZ
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuuZM2
		mov	aZ, eax
		mov	aDuZM2, ebx
		mov	aDuuZM2, ecx

location_render_sea_2a:
		; // draw row a dot
		mov	edx, aZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2aA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2aB

location_render_sea_2aA:
		mov	ebx, aDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		 ; // begin fog
		mov	ecx, fogMask
		movzx	ecx, byte ptr [ecx]
		add	fogMask, 2			; // add twice as we do two pixels together
		mov	ecx, slope_tables[4*ecx]		// ecx * sizeof( long *)
		mov	ebx, [ecx + MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		mov	ecx, remapTables
		; // end fog

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2aB:
		imul	edx, negBufTruePitch
		add	esi, 2
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, negBufTruePitch		; // pixel next line
		mov	[edi+edx], eax		; // new for 16-bit

		; // inc and compare inner loop a
		; // update aZ, aDuZM2, aDuuZM2
		; // aZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, aZ
		mov	ebx, aDuZM2
		add	eax, ebx
		mov	ecx, aDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	aZ, eax

		; // aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3
		; // aDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = aDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = aDuzM2 + 2*aDuuZM3 + 3*DuuuZM4D3
		 mov	aDuuZM2, ecx
		mov	aDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit
		dec	uCount
		jne	location_render_sea_2a
		pop	edi

		; // init inner loop b
		add	edi, bufTruePitch
		mov	uCount, LOCATE_WIDTH / 2
		push	edi

		; // set bZ, bDuZM2, bDuuZM2
		mov	eax, rowbZ
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuuZM2
		mov	bZ, eax
		mov	bDuZM2, ebx
		mov	bDuuZM2, ecx


location_render_sea_2b:
		; // draw row b dot
		mov	edx, bZ
		sar	edx, C_MULTIPLIER_SHIFT
		cmp	dx, seaAltitude
		jg		location_render_sea_2bA

		movsx	edx, seaAltitude
		mov	eax, seaColor		; // new for 16-bit // highword = lowword = sea color
		jmp	location_render_sea_2bB

location_render_sea_2bA:
		mov	ebx, bDuZM2
		mov	ax, [esi]			// top 24-bit is brightness table, low 8 bit is texture color
		sar	ebx, C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1	// aDu1zM2 is doubled, shift one more, 
		 ; // begin fog
		mov	ecx, fogMask
		movzx	ecx, byte ptr [ecx]
		add	fogMask, 2			; // add twice as we do two pixels together
		mov	ecx, slope_tables[4*ecx]		// ecx * sizeof( long *)
		mov	ebx, [ecx + MAX_BRIGHTNESS_ADJUST_DEGREE*2*4 + ebx*4]
		mov	ecx, remapTables
		; // end fog

		mov	bl, ah				; // new for 16-bit do AH first
		;mov	bl, 9ch				// enable this line for grey scale, previous line for textured
		ror	eax, 16				; // protect left pixel in AL, // new for 16-bit
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
		rol	eax, 16				; // restore left pixel // new for 16-bit
		mov	bl, al				; // new for 16-bit
		;mov bl, 9ch
		mov	ax, [ecx+ebx*2]	; // new for 16-bit
location_render_sea_2bB:
		imul	edx, negBufTruePitch
		add	esi, 2
		mov	[edi+edx], eax		; // new for 16-bit
		sub	edx, negBufTruePitch		; // pixel next line
		mov	[edi+edx], eax		; // new for 16-bit
		
		; // update bZ, bDuZM2, bDuuZM2
		; // bZ += aDuZ + aDuuZM2 + DuuuZM4D3
		mov	eax, bZ
		mov	ebx, bDuZM2
		add	eax, ebx
		mov	ecx, bDuuZM2
		add	eax, ecx
		mov	edx, DuuuZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DuuuZM4D3
		mov	bZ, eax

		; // bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3
		; // bDuuZM2 += 3 * DuuuZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx						; ecx = bDuuZM2 + 3*DuuuZM4D3
		add	ebx, edx						; ebx = bDuzM2 + 2*bDuuZM3 + 3*DuuuZM4D3
		 mov	bDuuZM2, ecx
		mov	bDuZM2, ebx

		; // increase edi
		add	edi, bufTruePitchPlusTwoPixels		; // new for 16-bit

		dec	uCount
		jne	location_render_sea_2b
		pop	edi

		; // inc and compare outer loop

		; // update rowaZ
		; // rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3
		mov	eax, rowaZ
		mov	ebx, rowaDvZM2
		add	eax, ebx
		mov	ecx, rowaDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowaZ, eax

		; // update rowaDvZM2, rowaDvvZM2
		; // rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3
		; // rowaDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowaDvvZM2, ecx
		mov	rowaDvZM2, ebx

		; // update rowaDuZM2
		; // rowaDuZM2 += rowaDuvZM4 + DuvvZM4
		; // rowaDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowaDuZM2
		mov	ecx, rowaDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowaDuvZM4, ecx
		mov	rowaDuZM2, ebx

		; // update rowaDuuZM2
		; // rowaDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowaDuuZM2, edx

		; // update rowbZ
		; // rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3
		mov	eax, rowbZ
		mov	ebx, rowbDvZM2
		add	eax, ebx
		mov	ecx, rowbDvvZM2
		add	eax, ecx
		mov	edx, DvvvZM4D3
		add	eax, edx
		lea	edx, [edx + 2 * edx]		; edx = 3 * DvvvZM4D3
		mov	rowbZ, eax

		; // update rowbDvZM2, rowbDvvZM2
		; // rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3
		; // rowbDvvZM2 += 3 * DvvvZM4D3
		lea	ebx, [ebx + 2 * ecx]
		 add	ecx, edx
		add	ebx, edx
		 mov	rowbDvvZM2, ecx
		mov	rowbDvZM2, ebx

		; // update rowbDuZM2
		; // rowbDuZM2 += rowbDuvZM4 + DuvvZM4
		; // rowbDuvZM4 += 2 * DuvvZM4
		mov	ebx, rowbDuZM2
		mov	ecx, rowbDuvZM4
		mov	edx, DuvvZM4
		add	ebx, ecx
		lea	ecx, [ecx + 2*edx]
		add	ebx, edx
		mov	rowbDuvZM4, ecx
		mov	rowbDuZM2, ebx

		; // update rowbDuuZM2
		; // rowbDuuZM2 += DuuvZM4
		mov	edx, DuuvZM4
		add	rowbDuuZM2, edx

		sub	edi, 4				; // new for 16-bit
		dec	vCount
		jne	location_render_sea_1
	}		// end _asm
	}
#endif

#undef MAX_BRIGHTNESS_SHIFT
}
