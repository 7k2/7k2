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

// Filename    : OMAGIC.CPP
// Description : Magic class, this file contains ALL magic effects
// Ownership   : Ban

#include <math.h>
#include <time.h>
#include <ovgabuf.h>
#include <all.h>
#include <ovga.h>
#include <omisc.h>
#include <omagic.h>
#include <oworldmt.h>
#include <color.h>
#include <imgfun.h>
#include <omodeid.h>
#include <oimgres.h>
#include <oconfig.h>
#include <ogodres.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <ogame.h>
#include <osys.h>
//------------ Define constant ----------//

static int bound_x1;
static int bound_x2;
static int bound_y1;
static int bound_y2;
static int color_r; 
static int color_g;
static int color_b;

//-------- Begin of function Magic::~Magic ----------//
Magic::Magic()
{
	h_particle = NULL;
	check_table = (unsigned short*) mem_add( sizeof(unsigned short) *24);
	color_table = (unsigned short*) mem_add( sizeof(unsigned short) *54);
	dist_table  = (unsigned short*) mem_add( sizeof(unsigned short) *18);

}

Magic::~Magic()
{
	mem_del(check_table);
	check_table = NULL;
	mem_del(color_table);
	color_table = NULL;
	mem_del(dist_table);
	dist_table = NULL;
}
short Magic::add_blend(short *point, short R, short G, short B, int mode)
{
	short prev_r;
	short prev_g;
	short prev_b;
	if (mode == 0)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_b = ( *point & 0x7C00 ) >>10;///1024;
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B; 
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_b > 0x001F)
			prev_b = 0x7C00;
		else
			prev_b = prev_b <<10;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_b = ( *point & 0xF800 ) >>11;///2048;
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B;
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_b > 0x001F)
			prev_b = (prev_b | 0x001F )<< 11;
		else
			prev_b = prev_b <<11;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		prev_b = ( *point & 0x001F ); 
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_r = ( *point & 0x7C00 ) >>10;///1024;
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_r > 0x001F)
			prev_r = 0x7C00;
		else
			prev_r = prev_r <<10;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		prev_b = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_r = ( *point & 0xF800 ) >>11;///2048;
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
	 	if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_r > 0x001F)
			prev_r = (prev_r | 0x001F )<< 11;
		else
			prev_r = prev_r <<11;
		return (prev_b | prev_g | prev_r);
	}
}

short Magic::add_blend2(short *point, short *point2, int mode)
{
	short prev_r;
	short prev_g;
	short prev_b;
	short R, G, B;
	if (mode == 0)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_b = ( *point & 0x7C00 ) >>10;///1024;
		R = ( *point2 & 0x001F );
		G = ( *point2 & 0x03E0 ) >>5;///32;
		B = ( *point2 & 0x7C00 ) >>10;///1024;
		
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B; 
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_b > 0x001F)
			prev_b = 0x7C00;
		else
			prev_b = prev_b <<10;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_b = ( *point & 0xF800 ) >>11;///2048;
		R = ( *point2 & 0x001F );
		G = ( *point2 & 0x07E0 ) >>5;///32;
		B = ( *point2 & 0xF800 ) >>11;///1024;
		
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B;
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_b > 0x001F)
			prev_b = (prev_b | 0x001F )<< 11;
		else
			prev_b = prev_b <<11;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		prev_b = ( *point & 0x001F ); 
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_r = ( *point & 0x7C00 ) >>10;///1024;
		B = ( *point2 & 0x001F );
		G = ( *point2 & 0x03E0 ) >>5;///32;
		R = ( *point2 & 0x7C00 ) >>10;///1024;
		
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_r > 0x001F)
			prev_r = 0x7C00;
		else
			prev_r = prev_r <<10;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		prev_b = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_r = ( *point & 0xF800 ) >>11;///2048;
		B = ( *point2 & 0x001F );
		G = ( *point2 & 0x07E0 ) >>5;///32;
		R = ( *point2 & 0xF800 ) >>11;///1024;
		
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
	 	if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_r > 0x001F)
			prev_r = (prev_r | 0x001F )<< 11;
		else
			prev_r = prev_r <<11;
		return (prev_b | prev_g | prev_r);
	}
}

short Magic::add_blend3(short *point, short R, short G, short B, int mode, int par)
{
	short prev_r;
	short prev_g;
	short prev_b;
	R = R  * (100 - par) /100;
	G = G  * (100 - par) /100;
	B = B  * (100 - par) /100;
	
	if (mode == 0)
	{
		prev_r =  ( *point & 0x001F ) * par /100;
		prev_g = (( *point & 0x03E0 ) >>5) * par /100;///32;
		prev_b = (( *point & 0x7C00 ) >>10) * par /100;///1024;
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B; 
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_b > 0x001F)
			prev_b = 0x7C00;
		else
			prev_b = prev_b <<10;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		prev_r = ( *point & 0x001F ) * par /100;
		prev_g = (( *point & 0x07E0 ) >>5) * par /100;///32;
		prev_b = (( *point & 0xF800 ) >>11) * par /100;///2048;
		prev_r = prev_r + R; 
		prev_g = prev_g + G; 
		prev_b = prev_b + B;
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_b > 0x001F)
			prev_b = (prev_b | 0x001F )<< 11;
		else
			prev_b = prev_b <<11;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		prev_b = ( *point & 0x001F ) * par /100; 
		prev_g = (( *point & 0x03E0 ) >>5) * par /100;///32;
		prev_r = (( *point & 0x7C00 ) >>10) * par /100;///1024;
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_r > 0x001F)
			prev_r = 0x7C00;
		else
			prev_r = prev_r <<10;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		prev_b = ( *point & 0x001F ) * par /100;
		prev_g = (( *point & 0x07E0 ) >>5) * par /100;///32;
		prev_r = (( *point & 0xF800 ) >>11) * par /100;///2048;
		prev_b = prev_b + B; 
		prev_g = prev_g + G; 
		prev_r = prev_r + R; 
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
	 	if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_r > 0x001F)
			prev_r = (prev_r | 0x001F )<< 11;
		else
			prev_r = prev_r <<11;
		return (prev_b | prev_g | prev_r);
	}
}

short Magic::mins_blend(short *point, short R, short G, short B, int mode)
{
	short prev_r;
	short prev_g;
	short prev_b;
	if (mode == 0)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_b = ( *point & 0x7C00 ) >>10;///1024;
		prev_r = R - prev_r; 
		prev_g = G - prev_g; 
		prev_b = B - prev_b; 
		if (prev_r < 0)
			prev_r = 0; 
		if (prev_g < 0)
			prev_g = 0;
		else
			prev_g = prev_g <<5;
		if (prev_b < 0)
			prev_b = 0;
		else
			prev_b = prev_b <<10;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		prev_r = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_b = ( *point & 0xF800 ) >>11;///2048;
		prev_r = R - prev_r; 
		prev_g = G - prev_g; 
		prev_b = B - prev_b;
		if (prev_r < 0)
			prev_r = 0; 
		if (prev_g < 0)
			prev_g = 0;
		else
			prev_g = prev_g <<5;
		if (prev_b < 0)
			prev_b = 0;
		else
			prev_b = prev_b <<11;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		prev_b = ( *point & 0x001F ); 
		prev_g = ( *point & 0x03E0 ) >>5;///32;
		prev_r = ( *point & 0x7C00 ) >>10;///1024;
		prev_b = B - prev_b; 
		prev_g = G - prev_g; 
		prev_r = R - prev_r; 
		if (prev_b < 0)
			prev_b = 0; 
		if (prev_g < 0)
			prev_g = 0;
		else
			prev_g = prev_g <<5;
		if (prev_r < 0)
			prev_r = 0;
		else
			prev_r = prev_r <<10;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		prev_b = ( *point & 0x001F );
		prev_g = ( *point & 0x07E0 ) >>5;///32;
		prev_r = ( *point & 0xF800 ) >>11;///2048;
		prev_b = B - prev_b; 
		prev_g = G - prev_g; 
		prev_r = R - prev_r; 
		if (prev_b < 0)
			prev_b = 0; 
	 	if (prev_g < 0)
			prev_g = 0;
		else
			prev_g = prev_g <<5;
		if (prev_r < 0)
			prev_r = 0;
		else
			prev_r = prev_r <<11;
		return (prev_b | prev_g | prev_r);
	}
}

short Magic::shading(short *origin, short *point, short ratio, int mode)
{
	short prev_r;
	short prev_g;
	short prev_b;
	
	if (!origin)
		origin = point;

	if (mode == 0)
	{
		prev_r = (( (( *point & 0x001F ) + ( *origin & 0x001F ))* ratio) >> 6) & 0x001F;
		prev_g = (( (( *point & 0x03E0 ) + ( *origin & 0x03E0 ))* ratio) >> 6) & 0x03E0;
		prev_b = (( (( *point & 0x7C00 ) + ( *origin & 0x03E0 ))* ratio) >> 6) & 0x7C00;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		prev_r = (( (( *point & 0x001F ) + ( *origin & 0x001F ))* ratio) >> 6) & 0x001F;
		prev_g = (( (( *point & 0x07E0 ) + ( *origin & 0x07E0 ))* ratio) >> 6) & 0x07E0;
		prev_b = (( (( *point & 0xF800 ) + ( *origin & 0xF800 ))* ratio) >> 6) & 0xF800;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		prev_b = (( (( *point & 0x001F ) + ( *origin & 0x001F ))* ratio) >> 6) & 0x001F;
		prev_g = (( (( *point & 0x03E0 ) + ( *origin & 0x03E0 ))* ratio) >> 6) & 0x03E0;
		prev_r = (( (( *point & 0x7C00 ) + ( *origin & 0x7C00 ))* ratio) >> 6) & 0x7C00;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		prev_b = (( (( *point & 0x001F ) + ( *origin & 0x001F ))* ratio) >> 6) & 0x001F;
		prev_g = (( (( *point & 0x07E0 ) + ( *origin & 0x07E0 ))* ratio) >> 6) & 0x07E0;
		prev_r = (( (( *point & 0xF800 ) + ( *origin & 0xF800 ))* ratio) >> 6) & 0xF800;
		return (prev_b | prev_g | prev_r);
	}
}

short Magic::draw(short R, short G, short B, int mode)
{
	short prev_r;
	short prev_g;
	short prev_b;
#ifdef ASM_FOR_MSVC
	_asm
	{
		mov ax, R  //R = R *31 /255;
		shr ax, 3
		mov R, ax
		mov ax, B  //B = B *31 /255;
		shr ax, 3
		mov B, ax
		cmp mode, 1 // Comparing mode for setting G
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G  //G = G *31 /255;
		shr ax, 3
		mov G, ax
		jmp END
MODE_ODD:
		mov ax, G  //G = G *63 /255;
		shr ax, 2
		mov G, ax
END:
	}
#else
	R >>= 3;
	B >>= 3;
	if (mode == 1 || mode == 3)
	{
		G >>= 2;
	}
	else
	{
		G >>= 3;
	}
#endif
	prev_r = R; 
	prev_g = G; 
	prev_b = B; 
			
	if (mode == 0)
	{
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_b > 0x001F)
			prev_b = 0x7C00;
		else
			prev_b = prev_b <<10;
		return (prev_r | prev_g | prev_b);
	}
	else
	if (mode == 1)
	{
		if (prev_r > 0x001F)
			prev_r = 0x001F; 
		if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_b > 0x001F)
			prev_b = (prev_b | 0x001F )<< 11;
		else
			prev_b = prev_b <<11;
		return (prev_r | prev_g | prev_b);
	}
	else 
	if (mode == 2)
	{
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
		if (prev_g > 0x001F)
			prev_g = 0x03E0;
		else
			prev_g = prev_g <<5;
		
		if (prev_r > 0x001F)
			prev_r = 0x7C00;
		else
			prev_r = prev_r <<10;
		return (prev_b | prev_g | prev_r);
	}
	else
	{
		if (prev_b > 0x001F)
			prev_b = 0x001F; 
	 	if (prev_g > 0x003F)
			prev_g = 0x07E0;
		else
			prev_g = prev_g <<5;
		if (prev_r > 0x001F)
			prev_r = (prev_r | 0x001F )<< 11;
		else
			prev_r = prev_r <<11;
		return (prev_b | prev_g | prev_r);
	}
}

void Magic::test_flag()
{
	effect_testing_flag ++;
	if (effect_testing_flag >8)
		effect_testing_flag = 0;
}
void Magic::init(int x1, int y1, int x2, int y2)
{
	bound_x1 = x1;
	bound_x2 = x2;
	bound_y1 = y1;
	bound_y2 = y2;
	
	*(check_table +0 +0) = (unsigned short)0x001F;	*(check_table +6 +0) = (unsigned short)0x001F;
	*(check_table +0 +1) = (unsigned short)0x03E0;	*(check_table +6 +1) = (unsigned short)0x07E0;
	*(check_table +0 +2) = (unsigned short)0x7C00;	*(check_table +6 +2) = (unsigned short)0xF800;
	*(check_table +0 +3) = (unsigned short)0x001F;	*(check_table +6 +3) = (unsigned short)0x003F;
	*(check_table +0 +4) = (unsigned short)0;			*(check_table +6 +4) = (unsigned short)0;
	*(check_table +0 +5) = (unsigned short)10;		*(check_table +6 +5) = (unsigned short)11;
	
	*(check_table +12+0) = (unsigned short)0x7C00;	*(check_table +18+0) = (unsigned short)0xF800;
	*(check_table +12+1) = (unsigned short)0x03E0;	*(check_table +18+1) = (unsigned short)0x07E0;
	*(check_table +12+2) = (unsigned short)0x001F;	*(check_table +18+2) = (unsigned short)0x001F;
	*(check_table +12+3) = (unsigned short)0x001F;	*(check_table +18+3) = (unsigned short)0x003F;
	*(check_table +12+4) = (unsigned short)10;		*(check_table +18+4) = (unsigned short)11;
	*(check_table +12+5) = (unsigned short)0;			*(check_table +18+5) = (unsigned short)0;

	*(dist_table +0)  = (unsigned short)0;
	*(dist_table +1)  = (unsigned short)1;
	*(dist_table +2)  = (unsigned short)4;
	*(dist_table +3)  = (unsigned short)9;
	*(dist_table +4)  = (unsigned short)16;
	*(dist_table +5)  = (unsigned short)25;
	*(dist_table +6)  = (unsigned short)36;
	*(dist_table +7)  = (unsigned short)49;
	*(dist_table +8)  = (unsigned short)64;
	*(dist_table +9)  = (unsigned short)81;
	*(dist_table +10) = (unsigned short)100;
	*(dist_table +11) = (unsigned short)121;
	*(dist_table +12) = (unsigned short)144;
	*(dist_table +13) = (unsigned short)169;
	*(dist_table +14) = (unsigned short)196;
	*(dist_table +15) = (unsigned short)225;
	*(dist_table +16) = (unsigned short)256;
	*(dist_table +17) = (unsigned short)999;
}

//-------- Begin of static function Magic::dist ----------//
double Magic::dist(double dx, double dy)
{
	return sqrt( dx*dx + dy*dy);
}
//-------- End of static function Magic::dist ----------//

//-------- Begin of function Magic::rand_seed ----------//
unsigned Magic::rand_seed()
{
	#define MULTIPLIER      0x015a4e35L
	#define INCREMENT       1
	seed = MULTIPLIER * seed + INCREMENT;
	return (seed>>16);
}
//-------- End of function Magic::rand_seed ----------//

//-------- Begin of function Magic::straight_light_beam ----------//
void Magic::straight_light_beam(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
								int thickness, int lightness, short R1, short G1, short B1, 
								short R2, short G2, short B2, int mode, char angle) 
{
	// All coordinates are in global value
	// and all coordination will not out bound
	// RGB range should be 0 - 255 /31 = 255-248 /0 = 0-7
	int i, k, diff, ratio, limit;
	int x = x1;
	int y = y1;
	int d_x = abs(x2 - x1);
	int d_y = abs(y2 - y1);
	int	x_count, y_count;
	short *point;
	short temp_r, temp_g, temp_b;
	short prev_r, prev_g, prev_b;
	int	line = (y2 - y1) > 0 ? (2 *vgabuf->buf_pitch()) : -(2 *vgabuf->buf_pitch());
	int	x_inc = (x2 - x1) > 0 ? 2 : -2;
	int	y_inc = (x2 - x1) > 0 ? 2 : -2;
	int mode_offset = mode *12;
	unsigned short *base = check_table;
#ifdef ASM_FOR_MSVC
	_asm
	{
		mov ax, R1  //R1 = R1 *31 /255;
		shr ax, 3
		mov R1, ax
		mov ax, R2	//R2 = R2 *31 /255;
		shr ax, 3
		mov R2, ax
		mov ax, B1  //B1 = B1 *31 /255;
		shr ax, 3
		mov B1, ax
		mov ax, B2	//B2 = B2 *31 /255;
		shr ax, 3
		mov B2, ax
		cmp mode, 1 // Comparing mode for setting G1 and G2
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G1  //G1 = G1 *31 /255;
		shr ax, 3
		mov G1, ax
		mov ax, G2	//G2 = G2 *31 /255;
		shr ax, 3
		mov G2, ax
		jmp END
MODE_ODD:
		mov ax, G1  //G1 = G1 *63 /255;
		shr ax, 2
		mov G1, ax
		mov ax, G2	//G2 = G2 *63 /255;
		shr ax, 2
		mov G2, ax
END:	mov eax, thickness
		sub eax, lightness
		mov diff, eax
	}
#else
	R1 >>= 3;
	R2 >>= 3;
	B1 >>= 3;
	B2 >>= 3;
	if (mode == 1 || mode == 3)
	{
		G1 >>= 2;
		G2 >>= 2;
	}
	else
	{
		G1 >>= 3;
		G2 >>= 3;
	}
	diff = thickness - lightness;
#endif
	if (diff <1)
		diff =1;
	if ((d_y == 0) && (d_x == 0))
		return;
	if (d_x > d_y)
	{
		y_inc = (d_y << 8) / d_x;  //just calculate slope no clipping
		for (k = -thickness; k<= thickness; k ++)
		{
			y = 0;
			y_count = 0;
			point = vgabuf->buf_ptr(x1,k + y1);
			limit = d_x;
			if (angle > 1)
			{
				line = (y2 -y1 -k) > 0 ? (2 *vgabuf->buf_pitch()) : -(2 *vgabuf->buf_pitch());
				y_inc = ((abs(y2 -y1 -k)) << 8) / d_x;
			}
			if (angle == 1)
			{
				limit = d_x - d_x * abs(k)/thickness;
				point = vgabuf->buf_ptr(x1 + (x2-x1) *abs(k) /thickness /2,k + y1);
			}
			// Ready Variables for line drawing
			ratio = 15 - (abs(k)-lightness) * 15 / diff;	
			temp_r = (R1 * ratio) >>4;
			temp_g = (G1 * ratio) >>4;
			temp_b = (B1 * ratio) >>4;
			if ((k > - lightness) && (k < lightness))
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
//RED11:				
					mov cl, [edi +8]
					mov ax, R2
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
//GREEN11:			
					mov ax, G2
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
//BLUE11:				
					mov cl, [edi +10]
					mov ax, B2
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
//END11:				
					or  ax, prev_g			//*point = prev_r | prev_g | prev_b;			
					or  ax, prev_r
					mov [esi], ax
					mov eax, y_count		//y_count = y_count + y_inc;
					add eax, y_inc
					mov y_count, eax
					mov ecx, y				//if ((y << 8) < y_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH11				
					add esi, line			//point = point + line;
					inc ecx					// y++;
					mov y, ecx
FINISH11:			add esi, x_inc			//point = point + x_inc;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = R2 << cl;
					prev_g = G2 << 5;
					cl = *(edi + 5);
					prev_b = B2 << cl;
					*point = prev_r | prev_g | prev_b;
					y_count += y_inc;
					if (y_count > (y << 8))
					{
						point += line / 2;
						++y;
					}
					point += x_inc / 2;
				}
#endif
			}
			else
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
					mov dx, [esi]
//RED12:				
					mov cl, [edi +8]
				    mov bx, [edi]
					mov prev_r, bx			// assign R_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_r
					cmp ax, 001fh
					jg  GREEN12
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
GREEN12:			mov bx, [edi +2]
					mov prev_g, bx			// assign G_OVER value
					mov ax, dx
					and ax, bx
					shr ax, 5
					add ax, temp_g
					cmp ax, [edi +6]
					jg  BLUE12
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
BLUE12:				mov cl, [edi +10]
					mov bx, [edi +4]
					mov prev_b, bx			// assign B_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_b
					cmp ax, 001fh
					jg  END12
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
END12:				mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_g			
					or  ax, prev_b
					mov [esi], ax
					mov eax, y_count		//y_count = y_count + y_inc;
					add eax, y_inc
					mov y_count, eax
					mov ecx, y				//if ((y << 8) < y_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH12				
					add esi, line			//point = point + line;
					inc ecx					// y++;
					mov y, ecx
FINISH12:			add esi, x_inc			//point = point + x_inc;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = *edi;
					short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
					if (!(ax > 0x1F))
					{
						prev_r = ax << cl;
					}
					prev_g = *(edi + 1);
					ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
					if (!(ax > *(edi + 3)))
					{
						prev_g = ax << 5;
					}
					cl = *(edi + 5);
					prev_b = *(edi + 2);
					ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
					if (!(ax > 0x1F))
					{
						prev_b = ax << cl;
					}
					*point = prev_r | prev_g | prev_b;
					y_count += y_inc;
					if (y_count > (y << 8))
					{
						point += line / 2;
						++y;
					}
					point += x_inc / 2;
				}
#endif
			}	
		}
	}
	else 
	{
		x_inc = (d_x << 8 )/ d_y;
		for (k = -thickness; k<= thickness; k ++)
		{
			x = 0;
			x_count = 0;
			point = vgabuf->buf_ptr(k + x1,y1);
			limit = d_y;
			if (angle > 1)
			{
				y_inc = (x2 -x1 -k) > 0 ? 2 : -2;
				x_inc = ((abs(x2 -x1 -k)) << 8) / d_y;
			}
			if (angle == 1)
			{
				limit = d_y - d_y * abs(k)/thickness;
				point = vgabuf->buf_ptr(k + x1, y1 + (y2-y1) *abs(k) /thickness /2);
			}
			ratio = 15 - (abs(k)-lightness) * 15 / diff;
			temp_r = (R1 * ratio) >>4;
			temp_g = (G1 * ratio) >>4;
			temp_b = (B1 * ratio) >>4;
			if ((k > - lightness) && (k < lightness))
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
//RED21:		
					mov cl, [edi +8]
					mov ax, R2
				   	shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
//GREEN21:	
					mov ax, G2
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
//BLUE21:
					mov cl, [edi +10]
					mov ax, B2
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
//END21:
					or  ax, prev_g			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_r			
					mov [esi], ax
					mov eax, x_count		//x_count = x_count + x_inc;
					add eax, x_inc
					mov x_count, eax
					mov ecx, x				//if ((x << 8) < x_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH21				
					add esi, y_inc			//point = point + y_inc;
					inc ecx					//x ++;
					mov x, ecx
FINISH21:			add esi, line			//point = point + line;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = R2 << cl;
					prev_g = G2 << 5;
					cl = *(edi + 5);
					prev_b = B2 << cl;
					*point = prev_r | prev_g | prev_b;
					x_count += x_inc;
					if (x_count > (x << 8))
					{
						point += y_inc / 2;
						++x;
					}
					point += line / 2;
				}
#endif
			}
			else
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
					mov dx, [esi]
//RED22:	
					mov cl, [edi +8]
				    mov bx, [edi]
					mov prev_r, bx			// assign R_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_r
					cmp ax, 001fh
					jg  GREEN22
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
GREEN22:			mov bx, [edi +2]
					mov prev_g, bx			// assign G_OVER value
					mov ax, dx
					and ax, bx
					shr ax, 5
					add ax, temp_g
					cmp ax, [edi +6]
					jg  BLUE22
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
BLUE22:				mov cl, [edi +10]
					mov bx, [edi +4]
					mov prev_b, bx			// assign B_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_b
					cmp ax, 001fh
					jg  END22
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
END22:				mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_g			
					or  ax, prev_b
					mov [esi], ax
					mov eax, x_count		//x_count = x_count + x_inc;
					add eax, x_inc
					mov x_count, eax
					mov ecx, x				//if ((x << 8) < x_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH22				
					add esi, y_inc			//point = point + y_inc;
					inc ecx					//x ++;
					mov x, ecx
FINISH22:			add esi, line			//point = point + line;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = *edi;
					short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
					if (!(ax > 0x1F))
					{
						prev_r = ax << cl;
					}
					prev_g = *(edi + 1);
					ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
					if (!(ax > *(edi + 3)))
					{
						prev_g = ax << 5;
					}
					cl = *(edi + 5);
					prev_b = *(edi + 2);
					ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
					if (!(ax > 0x1F))
					{
						prev_b = ax << cl;
					}
					*point = prev_r | prev_g | prev_b;
					x_count += x_inc;
					if (x_count > (x << 8))
					{
						point += y_inc / 2;
						++x;
					}
					point += line / 2;
				}
#endif
			}
		}
	}
}
//-------- End of function Magic::straight_light_beam ----------//

//-------- Begin of function Magic::straight_light_beam2 ----------//
void Magic::straight_light_beam2(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
								int thickness, int lightness, short R1, short G1, short B1, 
								short R2, short G2, short B2, int mode, char angle) 
{
	// this routine is as same as straight_light_beam, but
	// it performs clipping checking for each pixel. 
	// Therefore, this routine is slower than the first one

	// All coordinates are in global value
	// and all coordination will not out bound
	// RGB range should be 0 - 255 /31 = 255-248 /0 = 0-7
	int i, k, diff, ratio, limit;
	int x = x1;
	int y = y1;
	int d_x = abs(x2 - x1);
	int d_y = abs(y2 - y1);
	int	x_count, y_count;
	short *point;
	short temp_r, temp_g, temp_b;
	short prev_r, prev_g, prev_b;
	int	line = (y2 - y1) > 0 ? (2 *vgabuf->buf_pitch()) : -(2 *vgabuf->buf_pitch());
	int	x_inc = (x2 - x1) > 0 ? 2 : -2;
	int	y_inc = (x2 - x1) > 0 ? 2 : -2;
	int mode_offset = mode *12;
	unsigned short *base = check_table;
#ifdef ASM_FOR_MSVC
	_asm
	{
		mov ax, R1  //R1 = R1 *31 /255;
		shr ax, 3
		mov R1, ax
		mov ax, R2	//R2 = R2 *31 /255;
		shr ax, 3
		mov R2, ax
		mov ax, B1  //B1 = B1 *31 /255;
		shr ax, 3
		mov B1, ax
		mov ax, B2	//B2 = B2 *31 /255;
		shr ax, 3
		mov B2, ax
		cmp mode, 1 // Comparing mode for setting G1 and G2
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G1  //G1 = G1 *31 /255;
		shr ax, 3
		mov G1, ax
		mov ax, G2	//G2 = G2 *31 /255;
		shr ax, 3
		mov G2, ax
		jmp END
MODE_ODD:
		mov ax, G1  //G1 = G1 *63 /255;
		shr ax, 2
		mov G1, ax
		mov ax, G2	//G2 = G2 *63 /255;
		shr ax, 2
		mov G2, ax
END:	mov eax, thickness
		sub eax, lightness
		mov diff, eax
	}
#else
	R1 >>= 3;
	R2 >>= 3;
	B1 >>= 3;
	B2 >>= 3;
	if (mode == 1 || mode == 3)
	{
		G1 >>= 2;
		G2 >>= 2;
	}
	else
	{
		G1 >>= 3;
		G2 >>= 3;
	}
	diff = thickness - lightness;
#endif
	if (diff <1)
		diff =1;
	if ((d_y == 0) && (d_x == 0))
		return;
	if (d_x > d_y)
	{
		y_inc = (d_y << 8) / d_x;
		for (k = -thickness; k<= thickness; k ++)
		{
			y = 0;
			y_count = 0;
			
			// calculating the slope for clipping
			int xDiff = (x2-x1);
			int slope;
				// avoid dividing by zero error
			if (xDiff != 0)
				slope = ((y2-y1)<<6) /xDiff;
			else
				slope = 999;



			// clipping the Y axis and calculate the new starting and ending y coordinates
			// no need to clip the x axis because the draw_light_beam() routine has done this 
			// part
				// avoid dividing by zero error
			if (slope == 0)
				slope = 1;
			int tempX1=x1, tempX2=x2, tempY1=y1+k, tempY2=y2+k;
			if (tempY1 <= bound_y1)
			{
				tempY1 = bound_y1;
				tempX1 = x1 + ((tempY1 - (y1+k))<<6) / slope;
				if (tempX1 < bound_x1 || tempX1 > bound_x2)
					continue;
			}
			else
			if (tempY1 >= bound_y2)
			{
				tempY1 = bound_y2;
				tempX1 = x1 + ((tempY1 - (y1+k))<<6) / slope;
				if (tempX1 < bound_x1 || tempX1 > bound_x2)
					continue;
			}
			if (tempY2 <= bound_y1)
			{
				tempY2 = bound_y1;
				tempX2 = x1 + ((tempY2 - (y1+k))<<6) / slope;
				if (tempX2 < bound_x1 || tempX2 > bound_x2)
					continue;
			}
			else
			if (tempY2 >= bound_y2)
			{
				tempY2 = bound_y2;
				tempX2 = x1 + ((tempY2 - (y1+k))<<6) / slope;
				if (tempX2 < bound_x1 || tempX2 > bound_x2)
					continue;
			}


			point = vgabuf->buf_ptr(tempX1, tempY1);
			limit = abs(tempX2 - tempX1);
			if (limit != 0)
				y_inc = (abs(tempY2 - tempY1) << 8) / limit;

			if (angle > 1)
			{
				// calculating the slope for clipping
				// avoid dividing by zero error
				if (xDiff != 0)
					slope = ((y2-y1-k)<<6)/xDiff;
				else
					slope = 999;
			
				// clipping the Y axis and calculate the new starting and ending y coordinates
				// no need to clip the x axis because the draw_light_beam() routine has done this 
					// avoid dividing by zero error
				if (slope == 0)
					slope = 1;
						
				tempY2 = y2;
				if (tempY2 <= bound_y1)
				{
					tempY2 = bound_y1;
					tempX2 = x2 + ((tempY2 - y2)<<6) / slope;
					if (tempX2 < bound_x1 || tempX2 > bound_x2)
						continue;
				}
				else
				if (tempY2 >= bound_y2)
				{
					tempY2 = bound_y2;
					tempX2 = x2 + ((tempY2 - y2)<<6) / slope;
					if (tempX2 < bound_x1 || tempX2 > bound_x2)
						continue;
				}

				
				//calculating varable line, limit and y_inc
				line = (tempY2 -tempY1) > 0 ? (2 *vgabuf->buf_pitch()) : -(2 *vgabuf->buf_pitch());
				limit = abs(tempX2 - tempX1);
				// avoid dividing by zero error
				if (limit != 0)
					y_inc = ((abs(tempY2 -tempY1)) << 8) / limit;

				if (tempY2 == tempY1)
					continue;
			}
			if (angle == 1)
			{
			//	limit = d_x - d_x * abs(k)/thickness;
			//	point = vgabuf->buf_ptr(x1 + (x2-x1) *abs(k) /thickness /2,k + y1);
				limit = limit - limit * abs(k)/thickness;
				point = vgabuf->buf_ptr(tempX1 + (tempX2-tempX1) *abs(k) /thickness /2, tempY1);
			}
			// Ready Variables for line drawing
			ratio = 15 - (abs(k)-lightness) * 15 / diff;	
			temp_r = (R1 * ratio) >>4;
			temp_g = (G1 * ratio) >>4;
			temp_b = (B1 * ratio) >>4;
			if ((k > - lightness) && (k < lightness))
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
//RED11:				
					mov cl, [edi +8]
					mov ax, R2
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
//GREEN11:			
					mov ax, G2
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
//BLUE11:				
					mov cl, [edi +10]
					mov ax, B2
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
//END11:				
					or  ax, prev_g			//*point = prev_r | prev_g | prev_b;			
					or  ax, prev_r
					mov [esi], ax
					mov eax, y_count		//y_count = y_count + y_inc;
					add eax, y_inc
					mov y_count, eax
					mov ecx, y				//if ((y << 8) < y_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH11				
					add esi, line			//point = point + line;
					inc ecx					// y++;
					mov y, ecx
FINISH11:			add esi, x_inc			//point = point + x_inc;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = R2 << cl;
					prev_g = G2 << 5;
					cl = *(edi + 5);
					prev_b = B2 << cl;
					*point = prev_r | prev_g | prev_b;
					y_count += y_inc;
					if (y_count > (y << 8))
					{
						point += line / 2;
						++y;
					}
					point += x_inc / 2;
				}
#endif
			}
			else
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
					mov dx, [esi]
//RED12:				
					mov cl, [edi +8]
				    mov bx, [edi]
					mov prev_r, bx			// assign R_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_r
					cmp ax, 001fh
					jg  GREEN12
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
GREEN12:			mov bx, [edi +2]
					mov prev_g, bx			// assign G_OVER value
					mov ax, dx
					and ax, bx
					shr ax, 5
					add ax, temp_g
					cmp ax, [edi +6]
					jg  BLUE12
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
BLUE12:				mov cl, [edi +10]
					mov bx, [edi +4]
					mov prev_b, bx			// assign B_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_b
					cmp ax, 001fh
					jg  END12
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
END12:				mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_g			
					or  ax, prev_b
					mov [esi], ax
					mov eax, y_count		//y_count = y_count + y_inc;
					add eax, y_inc
					mov y_count, eax
					mov ecx, y				//if ((y << 8) < y_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH12				
					add esi, line			//point = point + line;
					inc ecx					// y++;
					mov y, ecx
FINISH12:			add esi, x_inc			//point = point + x_inc;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = *edi;
					short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
					if (!(ax > 0x1F))
					{
						prev_r = ax << cl;
					}
					prev_g = *(edi + 1);
					ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
					if (!(ax > *(edi + 3)))
					{
						prev_g = ax << 5;
					}
					cl = *(edi + 5);
					prev_b = *(edi + 2);
					ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
					if (!(ax > 0x1F))
					{
						prev_b = ax << cl;
					}
					*point = prev_r | prev_g | prev_b;
					y_count += y_inc;
					if (y_count > (y << 8))
					{
						point += line / 2;
						++y;
					}
					point += x_inc / 2;
				}
#endif
			}	
		}
	}
	else 
	{
		x_inc = (d_x << 8 )/ d_y;
		for (k = -thickness; k<= thickness; k ++)
		{
			x = 0;
			x_count = 0;
			
			
			// calculating the slope for clipping
			int xDiff = (x2-x1);
			int slope;
				// avoid dividing by zero error
			if (xDiff != 0)
				slope = ((y2-y1)<<6)/xDiff;
			else
				slope = 999;


			// clipping the Y axis and calculate the new starting and ending y coordinates
			// no need to clip the x axis because the draw_light_beam() routine has done this 
			// part
				// avoid dividing by zero error
			int tempX1=x1+k, tempX2=x2+k, tempY1=y1, tempY2=y2;
		
			if (tempX1 <= bound_x1)
			{
				tempX1 = bound_x1;
				tempY1 = y1 + (((tempX1 - (x1+k)) * slope)>>6);
				if (tempY1 < bound_y1 || tempY1 > bound_y2)
					continue;
			}
			else
			if (tempX1 >= bound_x2)
			{
				tempX1 = bound_x2;
				tempY1 = y1 + (((tempX1 - (x1+k)) * slope)>>6);
				if (tempY1 < bound_y1 || tempY1 > bound_y2)
					continue;
			}
			if (tempX2 <= bound_x1)
			{
				tempX2 = bound_x1;
				tempY2 = y1 + (((tempX2 - (x1+k)) * slope)>>6);
				if (tempY2 < bound_y1 || tempY2 > bound_y2)
					continue;
			}
			else
			if (tempX2 >= bound_x2)
			{
				tempX2 = bound_x2;
				tempY2 = y1 + (((tempX2 - (x1+k)) * slope)>>6);
				if (tempY2 < bound_y1 || tempY2 > bound_y2)
					continue;
			}
			point = vgabuf->buf_ptr(tempX1, tempY1);
			limit = abs(tempY2 - tempY1);
			if (limit != 0)
				x_inc = (abs(tempX2 - tempX1) << 8 )/ limit;


			if (angle > 1)
			{
	
				// calculating the slope for clipping
				xDiff -= k;
					// avoid dividing by zero error
				if (xDiff != 0)
					slope = ((y2-y1)<<6)/xDiff;
				else
					slope = 999;
			
				
				// clipping the Y axis and calculate the new starting and ending y coordinates
				// no need to clip the x axis because the draw_light_beam() routine has done this 
				// part
				tempX2=x2;
				if (tempX2 <= bound_x1)
				{
					tempX2 = bound_x1;
					tempY2 = y2 + (((tempX2 - x2) * slope)>>6);
					if (tempY2 < bound_y1 || tempY2 > bound_y2)
						continue;
				}
				else
				if (tempX2 >= bound_x2)
				{
					tempX2 = bound_x2;
					tempY2 = y2 + (((tempX2 - x2) * slope)>>6);
					if (tempY2 < bound_y1 || tempY2 > bound_y2)
						continue;
				}
			//	y_inc = (x2 -x1 -k) > 0 ? 2 : -2;
			//	x_inc = ((abs(x2 -x1 -k)) << 8) / d_y;
				
				
				//calculating varable line, limit and y_inc
				y_inc = (tempX2 -tempX1) > 0 ? 2 : -2;
				limit = abs(tempY2 - tempY1);
				// avoid dividing by zero error
				if (limit != 0)
					x_inc = ((abs(tempX2 -tempX1)) << 8) / limit;

				if (tempX2 == tempX1)
					continue;
			}
			if (angle == 1)
			{
				limit = limit - limit * abs(k)/thickness;
				point = vgabuf->buf_ptr(tempX1, tempY1 + (tempY2-tempY1) *abs(k) /thickness /2);
			}
			ratio = 15 - (abs(k)-lightness) * 15 / diff;
			temp_r = (R1 * ratio) >>4;
			temp_g = (G1 * ratio) >>4;
			temp_b = (B1 * ratio) >>4;
			if ((k > - lightness) && (k < lightness))
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
//RED21:		
					mov cl, [edi +8]
					mov ax, R2
				   	shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
//GREEN21:	
					mov ax, G2
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
//BLUE21:
					mov cl, [edi +10]
					mov ax, B2
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
//END21:
					or  ax, prev_g			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_r			
					mov [esi], ax
					mov eax, x_count		//x_count = x_count + x_inc;
					add eax, x_inc
					mov x_count, eax
					mov ecx, x				//if ((x << 8) < x_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH21				
					add esi, y_inc			//point = point + y_inc;
					inc ecx					//x ++;
					mov x, ecx
FINISH21:			add esi, line			//point = point + line;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = R2 << cl;
					prev_g = G2 << 5;
					cl = *(edi + 5);
					prev_b = B2 << cl;
					*point = prev_r | prev_g | prev_b;
					x_count += x_inc;
					if (x_count > (x << 8))
					{
						point += y_inc / 2;
						++x;
					}
					point += line / 2;
				}
#endif
			}
			else
			{
				for (i = 0; i<= limit; i ++) //draw one line
#ifdef ASM_FOR_MSVC
				_asm
				{
					mov edi, base
					add edi, mode_offset
					mov esi, point	//ok
					mov dx, [esi]
//RED22:	
					mov cl, [edi +8]
				    mov bx, [edi]
					mov prev_r, bx			// assign R_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_r
					cmp ax, 001fh
					jg  GREEN22
					shl ax, cl				// assign R_UNDER value
					mov prev_r, ax
GREEN22:			mov bx, [edi +2]
					mov prev_g, bx			// assign G_OVER value
					mov ax, dx
					and ax, bx
					shr ax, 5
					add ax, temp_g
					cmp ax, [edi +6]
					jg  BLUE22
					shl ax, 5				// assign G_UNDER value
					mov prev_g, ax
BLUE22:				mov cl, [edi +10]
					mov bx, [edi +4]
					mov prev_b, bx			// assign B_OVER value
					mov ax, dx
					and ax, bx
					shr ax, cl
					add ax, temp_b
					cmp ax, 001fh
					jg  END22
					shl ax, cl				// assign B_UNDER value
					mov prev_b, ax
END22:				mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
					or  ax, prev_g			
					or  ax, prev_b
					mov [esi], ax
					mov eax, x_count		//x_count = x_count + x_inc;
					add eax, x_inc
					mov x_count, eax
					mov ecx, x				//if ((x << 8) < x_count)
					mov ebx, ecx
					shl ebx, 8
					cmp eax, ebx
					jle FINISH22				
					add esi, y_inc			//point = point + y_inc;
					inc ecx					//x ++;
					mov x, ecx
FINISH22:			add esi, line			//point = point + line;
					mov point, esi
				}
#else
				{
					unsigned short *edi = base + (mode_offset / 2);
					unsigned char cl = *(edi + 4);
					prev_r = *edi;
					short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
					if (!(ax > 0x1F))
					{
						prev_r = ax << cl;
					}
					prev_g = *(edi + 1);
					ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
					if (!(ax > *(edi + 3)))
					{
						prev_g = ax << 5;
					}
					cl = *(edi + 5);
					prev_b = *(edi + 2);
					ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
					if (!(ax > 0x1F))
					{
						prev_b = ax << cl;
					}
					*point = prev_r | prev_g | prev_b;
					x_count += x_inc;
					if (x_count > (x << 8))
					{
						point += y_inc / 2;
						++x;
					}
					point += line / 2;
				}
#endif
			}
		}
	}
}
//-------- End of function Magic::straight_light_beam2 ----------//

// ----------- Begin of function Magic::draw_light_beam ----------//
// This rountine is directly copied from Oanline.cpp, its function is for
// out of bound checking, the actual draw function is straight_light_beam's 
// responsibility

void Magic::draw_light_beam(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
								int thickness, int lightness, short R1, short G1, short B1, 
								short R2, short G2, short B2, int mode, char angle, char fast) 
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int b_x1 = bound_x1;
	int b_x2 = bound_x2;
	int b_y1 = bound_y1;
	int b_y2 = bound_y2;
	if (dx > dy)
	{
		if (fast)
		{
			b_y1 = b_y1 + thickness;
			b_y2 = b_y2 - thickness;
			if ((angle > 1) && ((abs(y2 -y1) +thickness) > abs(x2 -x1)))
			{
				if (x2 > x1)
					x2 = x2 + thickness;
				else
					x2 = x2 - thickness;
			}
		}
		else
		{
			b_y1 = b_y1 - thickness;
			b_y2 = b_y2 + thickness;
		}		
	}
	else
	{
		if (fast)
		{
			b_x1 = b_x1 + thickness;
			b_x2 = b_x2 - thickness;
			if ((angle > 1) && ((abs(x2 -x1) +thickness) > abs(y2 -y1)))
			{	
				if (y2 > y1)
					y2 = y2 + thickness;
				else
					y2 = y2 - thickness;
			}
		}
		else
		{
			b_x1 = b_x1 - thickness;
			b_x2 = b_x2 + thickness;
		}
	}

	if( (x1 < b_x1 && x2 < b_x1) || ( x1 > b_x2 && x2 > b_x2) ||
		(y1 < b_y1 && y2 < b_y1) || ( y1 > b_y2 && y2 > b_y2) )
		return;
	if( x1 >= b_x1 && x1 <= b_x2 && x2 >= b_x1 && x2 <= b_x2 &&
		y1 >= b_y1 && y1 <= b_y2 && y2 >= b_y1 && y2 <= b_y2)
	
		if (fast)
			straight_light_beam(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
		else
			straight_light_beam2(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
	else
	{
		if( y1 == y2)
		{
			// horizontal line
			if( y1 < b_y1 || y1 > b_y2)
				return;
			if( x1 < b_x1)
				x1 = b_x1;
			else if( x1 > b_x2)
				x1 = b_x2;

			if( x2 < b_x1)
				x2 = b_x1;
			else if (x2 > b_x2)
				x2 = b_x2;
			if (fast)
				straight_light_beam(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle);
			else
				straight_light_beam2(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle);
		}
		else if( x1 == x2)
		{
			// vertical line
			if( x1 < b_x1 || x1 > b_x2)
				return;
			if( y1 < b_y1)
				y1 = b_y1;
			else if( y1 > b_y2)
				y1 = b_y2;

			if( y2 < b_y1)
				y2 = b_y1;
			else if( y2 > b_y2)
				y2 = b_y2;
			if (fast)
				straight_light_beam(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
			else
				straight_light_beam2(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
		}
		else
		{
			// neither horizontal or vertical
			int	x1a, y1a, x1b, y1b;
			int	x2a, y2a, x2b, y2b;

			// find intersection points for the first point
			x1a = x1b = x1;
			y1a = y1b = y1;
			if( y1 < b_y1)
			{
				y1a = b_y1;

				x1a = (b_y1-y1) * (x2-x1) / (y2-y1) + x1;
			}
			if( y1 > b_y2)
			{
				y1a = b_y2;
				x1a = (b_y2-y1) * (x2-x1) / (y2-y1) + x1;
			}
			if( x1 < b_x1)
			{
				x1b = b_x1;
				y1b = (b_x1-x1) * (y2-y1) / (x2-x1) + y1;
			}
			if( x1 > b_x2)
			{
				x1b = b_x2;
				y1b = (b_x2-x1) * (y2-y1) / (x2-x1) + y1;
			}
			
			// find intersection points for the second point
			x2a = x2b = x2;
			y2a = y2b = y2;
			if( y2 < b_y1)
			{
				y2a = b_y1;
				x2a = (b_y1-y1) * (x2-x1) / (y2-y1) + x1;
			}
			if( y2 > b_y2)
			{
				y2a = b_y2;
				x2a = (b_y2-y1) * (x2-x1) / (y2-y1) + x1;
			}
			if( x2 < b_x1)
			{
				x2b = b_x1;
				y2b = (b_x1-x1) * (y2-y1) / (x2-x1) + y1;
			}
			if( x2 > b_x2)
			{
				x2b = b_x2;
				y2b = (b_x2-x1) * (y2-y1) / (x2-x1) + y1;
			}

			// replace x1, y1
			if( x1 < b_x1 || x1 > b_x2 || y1 < b_y1 || y1 > b_y2)
			{
				if( x1a < b_x1 || x1a > b_x2 || y1a < b_y1 || y1a > b_y2)
				{
					if( x1b < b_x1 || x1b > b_x2 || y1b < b_y1 || y1b > b_y2)
					{
						return;
					}
					else
					{
						x1 = x1b;
						y1 = y1b;
					}	
				}
				else
				{
					x1 = x1a;
					y1 = y1a;
				}
			}

			// replace x2, y2
			if( x2 < b_x1 || x2 > b_x2 || y2 < b_y1 || y2 > b_y2)
			{
				if( x2a < b_x1 || x2a > b_x2 || y2a < b_y1 || y2a > b_y2)
				{
					if( x2b < b_x1 || x2b > b_x2 || y2b < b_y1 || y2b > b_y2)
					{
						return;
					}
					else
					{
						x2 = x2b;
						y2 = y2b;
					}	
				}
				else
				{
					x2 = x2a;
					y2 = y2a;
				}
			}
			if (fast)	
				straight_light_beam(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
			else
				straight_light_beam2(vgabuf, x1, y1, x2, y2, thickness, lightness, R1, G1, B1, R2, G2, B2, mode, angle) ;
		}
	}
}
// ----------- End of function Magic::draw_light_beam ----------//

// ----------- Begin of function Magic::generate_particle ----------//
void Magic::generate_particle(int x1, int y1, int x2, int y2, int dirx, int diry, int type, int var, int mode,
							  int number, int birth_time, int lifecount, int lifeLimit, int lifeOffset) 
{
	int i, diff;
	int temp_x, temp_y, temp_r1, temp_r2, temp_r3;
	Particle *temp_ptr; 
	diff = lifeLimit - lifecount;
	if (type == 0)
	{
		for (i = 0; i< number; i ++)
		{
			temp_ptr = h_particle + i;
			temp_r1 = (int)(rand_seed() %256);
			temp_r2 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_r3 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_x = (((x2 - x1) *temp_r1)>>7) +x1 +temp_r2;
			temp_y = (((y2 - y1) *temp_r1)>>7) +y1 +temp_r3;
			temp_ptr->life = rand_seed() %5 -birth_time +lifecount;
			temp_ptr->pos_x = temp_x + (dirx + temp_r2)*temp_ptr->life;
			temp_ptr->pos_y = temp_y + (diry + temp_r3)*temp_ptr->life;
		}
	}
	else
	if (type <4)
	{
		for (i = 0; i< number; i ++)
		{
			temp_ptr = h_particle + i;
			temp_r1 = (int)(rand_seed() %256);
			temp_r2 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_r3 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_x = (((x2 - x1) *temp_r1)>>7) +x1 +temp_r2;
			temp_y = (((y2 - y1) *temp_r1)>>7) +y1 +temp_r3;
			temp_ptr->dir_x = dirx + temp_r2;
			temp_ptr->dir_y = diry + temp_r3;
			temp_ptr->life = rand_seed() %5 -birth_time +lifecount;
			diff = temp_ptr->life - lifeLimit;
			if (diff > 0)
			{
				temp_ptr->pos_x = temp_x + temp_ptr->dir_x *lifeLimit + 
									(temp_ptr->dir_x>>2) *diff;
				diff = abs(temp_ptr->dir_y) *diff *diff;
				if (temp_ptr->dir_y <= 0)
					temp_ptr->pos_y = temp_y + temp_ptr->dir_y *lifeLimit +
									diff /lifeLimit;
				else
					temp_ptr->pos_y = temp_y + temp_ptr->dir_y *lifeLimit +
									(diff>>1) /lifeLimit;
			}
			else
			{
				temp_ptr->pos_x = temp_x + temp_ptr->dir_x *temp_ptr->life;
				temp_ptr->pos_y = temp_y + temp_ptr->dir_y *temp_ptr->life;
			}
		}
	}
	else
	if (type == 4)
	{
		for (i = 0; i< number; i ++)
		{
			temp_ptr = h_particle + i;
			temp_r1 = (int)(rand_seed() %256);
			temp_r2 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_r3 = ((var *((int)(rand_seed() %256)))>>7) -var;
			temp_x = (((x2 - x1) *temp_r1)>>7) +x1 +temp_r2;
			temp_y = (((y2 - y1) *temp_r1)>>7) +y1 +temp_r3;
			temp_ptr->life = rand_seed() %5 -birth_time +lifecount;
			temp_ptr->dir_x = dirx + temp_r2;
			temp_ptr->dir_y = diry + temp_r3;
			temp_ptr->prev_x = temp_x + temp_ptr->dir_x *(temp_ptr->life -1);
			temp_ptr->pos_x = temp_ptr->prev_x + temp_ptr->dir_x;	
			temp_ptr->prev_y = temp_y + temp_ptr->dir_y *(temp_ptr->life -1);
			temp_ptr->pos_y = temp_ptr->prev_y + temp_ptr->dir_y;

			// (temp_ptr->life+6) hard coded for firmdie
			if (mode == 0)
				temp_ptr->color = 0xFFFF >> (5 +(temp_ptr->life +lifeOffset) *10 /20);//	temp_ptr->prev_color = 0xFFFF;	temp_ptr->delta_color = 25 +temp_r1/4;
			else
			if (mode == 1)
				temp_ptr->color = 0xFFFF >> (5 +(temp_ptr->life +lifeOffset) *11 /20);//	temp_ptr->prev_color = 0xFFFF;	temp_ptr->delta_color = 25 +temp_r1/4;
			else
			if (mode == 2)
				temp_ptr->color = 0xFFFF << (5 +(temp_ptr->life +lifeOffset) *10 /20);//	temp_ptr->prev_color = 0xFFFF;	temp_ptr->delta_color = 25 +temp_r1/4;
			else
			if (mode == 3)
				temp_ptr->color = 0xFFFF << (5 +(temp_ptr->life +lifeOffset) *11 /20);//	temp_ptr->prev_color = 0xFFFF;	temp_ptr->delta_color = 25 +temp_r1/4;
		}
	}
}
// ----------- End of function Magic::generate_particle ----------//
// ----------- Begin of function Magic::draw_particle ----------//
void Magic::draw_particle(int x1, int y1, int x2, int y2, int dirx, int diry, int type, int var, int size,
						  VgaBuf *vgabuf, int mode, int number, int birth_time, int lifecount, int life_limit, int lifeOffset)
{
	Particle *particle;
	int i;
	seed = (unsigned)(x1 +x2 +y1 +y2);
	(void) rand_seed();
	h_particle = (Particle *) mem_add( sizeof(Particle) *number);
	generate_particle( x1, y1, x2, y2, dirx, diry, type, var, mode, number, birth_time, lifecount, life_limit, lifeOffset);
	//lifecount range = must 0 - 20 !;
	if (type == 0)// direction particle
	{
		for (i = 0; i < number; i++)
		{
			particle = h_particle +i;
			if ((particle->life <= life_limit) && (particle->life >= 0))
				draw_circle(vgabuf, particle->pos_x, particle->pos_y, 5, 1, 1, 200, 100, 100, mode, 0);
		}
	}
	else
	if (type == 1)// explosion particle
	{
		for (i = 0; i < number; i++)
		{
			particle = h_particle +i;
			if ((particle->life <= (life_limit<<1)) && (particle->life >= 0))
				draw_circle(vgabuf, particle->pos_x, particle->pos_y, 5, 1, 1, 200, 100, 100, mode, 0);
		}
	}
	else
	if (type == 2)// explosion circle particle with size variation
	{
		for (i = 0; i < number; i++)
		{
			particle = h_particle +i;
			if ((particle->life <= (life_limit<<1)) && (particle->life >= 0))
				draw_circle(vgabuf, particle->pos_x, particle->pos_y, size, 1, 1, 200, 100, 100, mode, 0);
		}
	}
	else
	if (type == 3)// explosion hole particle with size variation
	{
		for (i = 0; i < number; i++)
		{
			particle = h_particle +i;
			if ((particle->life <= (life_limit<<1)) && (particle->life >= 0))
				draw_circle(vgabuf, particle->pos_x, particle->pos_y, size, 1, 1, 200, 200, 200, mode, 1);
		}
	}
	else// direction line
	{
		if (life_limit > 20)
			life_limit = 20;
		for (i = 0; i < number; i++)
		{
			particle = h_particle +i;
			if ((particle->life <= life_limit) && (particle->life >= 0) && 
				(particle->pos_x <= bound_x2) && (particle->pos_x >= bound_x1) && 
				(particle->prev_x<= bound_x2) && (particle->prev_x>= bound_x1) && 
				(particle->pos_y <= bound_y2) && (particle->pos_y >= bound_y1) &&
				(particle->prev_y<= bound_y2) && (particle->prev_y>= bound_y1))
				IMGline(vgabuf->buf_ptr(), vgabuf->buf_true_pitch(), vgabuf->buf_width(),
						vgabuf->buf_height(), particle->prev_x, particle->prev_y,
						particle->pos_x, particle->pos_y, particle->color);
		}
	}
	mem_del(h_particle);
	h_particle = NULL;
}
// ----------- End of function Magic::draw_particle -----//
// ----------- Begin of function Magic::draw_circle -----//
void Magic::draw_circle(VgaBuf *vgabuf, int x1, int y1, int radius, char r_x, char r_y,
						short R1, short G1, short B1, int mode, int dir) 
{
	int i, j, line = 2*vgabuf->buf_pitch();
	int mag, temp_mag, up_level;
	short *point, *temp_point;
	short temp_r, temp_g, temp_b;
	short prev_r, prev_g, prev_b;
	int dis_state, temp_dis_state;
	int start_x, end_x;
	int start_y, end_y;
	int mode_offset = mode *12;
	unsigned short *base1 = check_table;
	unsigned short *base2 = color_table;
	unsigned short *base3 = dist_table;
	up_level = 	(radius * radius);
#ifdef ASM_FOR_MSVC
	_asm
	{
		mov ax, R1  //R1 = R1 *31 /255;
		shr ax, 3
		mov R1, ax
		mov ax, B1  //B1 = B1 *31 /255;
		shr ax, 3
		mov B1, ax
		cmp mode, 1 // Comparing mode for setting G1 and G2
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G1  //G1 = G1 *31 /255;
		shr ax, 3
		mov G1, ax
		jmp CLIPPING
MODE_ODD:
		mov ax, G1  //G1 = G1 *63 /255;
		shr ax, 2
		mov G1, ax
CLIPPING:
		mov esi, bound_x1	//	start_x = x1 -radius;
	//	add esi, 1
		mov edi, bound_x2
	//	sub edi, 1
		mov ebx, x1			
		mov ecx, radius
		mov eax, ebx
		sub eax, ecx
		cmp eax, esi		// if (start_x < bound_x1)
		jg	BOUND11			
		mov eax, esi		// start_x = bound_x1;
BOUND11:cmp eax, edi		// if (start_x > bound_x2)
		jl	BOUND12
		mov eax, edi		// start_x = bound_x2;
BOUND12:sub eax, ebx		// start_x = start_x -x1;	
		mov start_x, eax
		mov eax, ebx		//	end_x = x1 +radius;
		add eax, ecx
		cmp eax, esi		// if (end_x < bound_x1)
		jg	BOUND21			
		mov eax, esi		// end_x = bound_x1;
BOUND21:cmp eax, edi		// if (end_x > bound_x2)
		jl	BOUND22
		mov eax, edi		// end_x = bound_x2;
BOUND22:sub eax, ebx		// end_x = end_x -x1;	
		mov end_x, eax
		mov esi, bound_y1	//	start_y = y1 -radius;
	//	add esi, 1
		mov edi, bound_y2
	//	sub edi, 1
		mov ebx, y1			
		mov eax, ebx
		sub eax, ecx
		cmp eax, esi		// if (start_y < bound_y1)
		jg	BOUND31			
		mov eax, esi		// start_y = bound_y1;
BOUND31:cmp eax, edi		// if (start_y > bound_y2)
		jl	BOUND32
		mov eax, edi		// start_y = bound_y2;
BOUND32:sub eax, ebx		// start_y = start_y -y1;
		mov start_y, eax	
		mov eax, ebx		//	end_y = y1 +radius;
		add eax, ecx
		cmp eax, esi		// if (end_y < bound_y1)
		jg	BOUND41			
		mov eax, esi		// end_y = bound_y1;
BOUND41:cmp eax, edi		// if (end_y > bound_y2)
		jl	BOUND42
		mov eax, edi		// end_y = bound_y2;
BOUND42:sub eax, ebx		// end_y = end_y -y1;
		mov end_y, eax
		mov esi, base3		//RANGE
		mov ebx, 0
		mov eax, up_level
		inc eax
		shl eax, 1
		mov [esi+24], eax	//*(dist_table +6) = up_level<<1;
		shr eax, 1
		mov [esi+20], eax	//*(dist_table +5) = up_level;
		cmp dir, 0
		je MODE_0
//MODE_1:	
		mov ecx, up_level
		shr ecx, 1
		mov [esi+ 4], eax	//*(dist_table +1) = up_level>>4;
		sub [esi+ 4], ecx
		shr ecx, 1
		mov [esi+ 8], eax	//*(dist_table +2) = up_level>>3;
		sub [esi+ 8], ecx
		shr ecx, 1
		mov [esi+12], eax	//*(dist_table +3) = up_level>>2;
		sub [esi+12], ecx
		shr ecx, 1
		mov [esi+16], eax	//*(dist_table +4) = up_level>>1;
		sub [esi+16], ecx
		mov [esi], ebx
		mov esi, base2		//RED
		mov ax, R1		
		mov [esi+12], bx
		mov [esi+10], ax
		shr ax, 1
		mov [esi+8], ax
		shr ax, 1
		mov [esi+6], ax
		shr ax, 1
		mov [esi+4], ax
		shr ax, 1
		mov [esi+2], ax
		mov [esi], bx
		mov ax, G1			//GREEN
		mov [esi+26], bx
		mov [esi+24], ax
		shr ax, 1
		mov [esi+22], ax
		shr ax, 1
		mov [esi+20], ax
		shr ax, 1
		mov [esi+18], ax
		shr ax, 1
		mov [esi+16], ax
		mov [esi+14], bx
		mov ax, B1			//BLUE
		mov [esi+40], bx
		mov [esi+38], ax
		shr ax, 1
		mov [esi+36], ax
		shr ax, 1
		mov [esi+34], ax
		shr ax, 1
		mov [esi+32], ax
		shr ax, 1
		mov [esi+30], ax
		mov [esi+28], bx
		jmp FINISH
MODE_0:	shr eax, 1
		mov [esi+16], eax	//*(dist_table +4) = up_level>>1;
		shr eax, 1
		mov [esi+12], eax	//*(dist_table +3) = up_level>>2;
		shr eax, 1
		mov [esi+ 8], eax	//*(dist_table +2) = up_level>>3;
		shr eax, 1
		mov [esi+ 4], eax	//*(dist_table +1) = up_level>>4;
		mov [esi], ebx
		mov esi, base2		//RED
		mov ax, R1		
		mov [esi], bx
		mov [esi+2], ax
		shr ax, 1
		mov [esi+4], ax
		shr ax, 1
		mov [esi+6], ax
		shr ax, 1
		mov [esi+8], ax
		shr ax, 1
		mov [esi+10], ax
		mov [esi+12], bx
		mov ax, G1			//GREEN
		mov [esi+14], bx
		mov [esi+16], ax
		shr ax, 1
		mov [esi+18], ax
		shr ax, 1
		mov [esi+20], ax
		shr ax, 1
		mov [esi+22], ax
		shr ax, 1
		mov [esi+24], ax
		mov [esi+26], bx
		mov ax, B1			//BLUE
		mov [esi+28], bx
		mov [esi+30], ax
		shr ax, 1
		mov [esi+32], ax
		shr ax, 1
		mov [esi+34], ax
		shr ax, 1
		mov [esi+36], ax
		shr ax, 1
		mov [esi+38], ax
		mov [esi+40], bx
FINISH:
	}
#else
	R1 >>= 3;
	B1 >>= 3;
	if (mode == 1 || mode == 3)
	{
		G1 >>= 2;
	}
	else
	{
		G1 >>= 3;
	}
	int eax = x1 - radius;
	if (eax <= bound_x1)
	{
		eax = bound_x1;
	}
	if (eax >= bound_x2)
	{
		eax = bound_x2;
	}
	start_x = eax - x1;
	eax = x1 + radius;
	if (eax <= bound_x1)
	{
		eax = bound_x1;
	}
	if (eax >= bound_x2)
	{
		eax = bound_x2;
	}
	end_x = eax - x1;
	eax = y1 - radius;
	if (eax <= bound_y1)
	{
		eax = bound_y1;
	}
	if (eax >= bound_y2)
	{
		eax = bound_y2;
	}
	start_y = eax - y1;
	eax = y1 + radius;
	if (eax <= bound_y1)
	{
		eax = bound_y1;
	}
	if (eax >= bound_y2)
	{
		eax = bound_y2;
	}
	end_y = eax - y1;
	eax = up_level + 1;
	eax <<= 1;
	*(reinterpret_cast<unsigned int *>(base3 + 12)) = eax;
	eax >>= 1;
	*(reinterpret_cast<unsigned int *>(base3 + 10)) = eax;
	if (dir == 0)
	{
		for (int i = 8; i > 0; i -= 2)
		{
			eax >>= 1;
			*(reinterpret_cast<unsigned int *>(base3 + i)) = eax;
		}
		*(reinterpret_cast<unsigned int *>(base3)) = 0;
		*base2 = 0;
		short ax = R1;
		for (int i = 1; i <= 5; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 6) = 0;
		*(base2 + 7) = 0;
		ax = G1;
		for (int i = 8; i <= 12; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 13) = 0;
		*(base2 + 14) = 0;
		ax = B1;
		for (int i = 15; i <= 19; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 20) = 0;
	}
	else
	{
		int ecx = up_level;
		for (int i = 2; i <= 8; i += 2)
		{
			ecx >>= 1;
			*(reinterpret_cast<unsigned int *>(base3 + i)) = eax - ecx;
		}
		*(reinterpret_cast<unsigned int *>(base3)) = 0;
		short ax = R1;
		*(base2 + 6) = 0;
		for (int i = 5; i >= 1; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*base2 = 0;
		ax = G1;
		*(base2 + 13) = 0;
		for (int i = 12; i >= 8; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 7) = 0;
		ax = B1;
		*(base2 + 20) = 0;
		for (int i = 19; i >= 15; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 14) = 0;
	}
#endif
	if (start_x == end_x || start_y == end_y)
		return;
	
	temp_point = vgabuf->buf_ptr(start_x + x1, start_y + y1);
	temp_mag = start_y * start_y + start_x * start_x;
	temp_dis_state = 0;
	for (i = 1; i < 6; i ++)
		if (temp_mag > (*(dist_table + i)))
			temp_dis_state ++;
	for (i = start_y; i <= end_y; i++)
	{
		mag = temp_mag;
		point = temp_point;
#ifdef ASM_FOR_MSVC
		_asm
		{
		mov esi, temp_dis_state	//dis_state = temp_dis_state;
		mov dis_state, esi
		shl esi, 1
		add esi, base2
		mov ax, [esi+ 2]		//temp_r = *(color_table + 7 +dis_state +1);
		mov temp_r, ax
		mov ax, [esi+16]		//temp_g = *(color_table +14 +dis_state +1);
		mov temp_g, ax
		mov ax, [esi+30]		//temp_b = *(color_table +21 +dis_state +1);
		mov temp_b, ax
		}
#else
		dis_state = temp_dis_state;
		temp_r = *(base2 + ((temp_dis_state << 1) / 2) + 1);
		temp_g = *(base2 + ((temp_dis_state << 1) / 2) + 8);
		temp_b = *(base2 + ((temp_dis_state << 1) / 2) + 15);
#endif
		for (j = start_x; j <= end_x; j++)
#ifdef ASM_FOR_MSVC
		_asm
		{
			mov edi, base1
			add edi, mode_offset
			mov esi, point	//ok
			mov dx, [esi]
//RED:		
			mov cl, [edi +8]
			mov bx, [edi]
			mov prev_r, bx			// assign R_OVER value
			mov ax, dx
			and ax, bx
			shr ax, cl
			add ax, temp_r
			cmp ax, 001fh
			jg  GREEN
			shl ax, cl				// assign R_UNDER value
			mov prev_r, ax
GREEN:		
			mov bx, [edi +2]
			mov prev_g, bx			// assign G_OVER value
			mov ax, dx
			and ax, bx
			shr ax, 5
			add ax, temp_g
			cmp ax, [edi +6]
			jg  BLUE
			shl ax, 5				// assign G_UNDER value
			mov prev_g, ax
BLUE:		mov cl, [edi +10]
			mov bx, [edi +4]
			mov prev_b, bx			// assign B_OVER value
			mov ax, dx
			and ax, bx
			shr ax, cl
			add ax, temp_b
			cmp ax, 001fh
			jg  END
			shl ax, cl				// assign B_UNDER value
			mov prev_b, ax
END:		mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
			or  ax, prev_g			
			or  ax, prev_b
			mov [esi], ax
			inc esi					// point ++;
			inc esi
			mov point, esi
			mov cl, r_x
			mov eax, mag			//mag = mag +j +j +1;
			mov ebx, j
			shl ebx, cl
			add eax, ebx
			inc eax
			mov mag, eax
			mov edi, dis_state		//if (mag < *(color_table +dis_state))
			mov ebx, base3
			shl edi,2
			add ebx, edi
			mov esi, base2			
			shr edi,1
			add esi, edi
			shr edi,1
			cmp eax, [ebx]		
			jl	D_MINS
			cmp eax, [ebx+4]		//if (mag > *(color_table +dis_state +1))
			jle D_PLUS
			add esi, 4				//dis_state ++;
			add edi, 2
D_MINS:	
			sub esi, 2				//dis_state --;
			dec edi
			mov ax, [esi+ 2]		//temp_r = *(color_table + 7 +dis_state +1);
			mov temp_r, ax
			mov ax, [esi+16]		//temp_g = *(color_table +14 +dis_state +1);
			mov temp_g, ax
			mov ax, [esi+30]		//temp_b = *(color_table +21 +dis_state +1);
			mov temp_b, ax
			mov dis_state, edi			
D_PLUS:	
		}
#else
		{
			unsigned short *edi = base1 + (mode_offset / 2);
			unsigned char cl = *(edi + 4);
			prev_r = *edi;
			short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
			if (!(ax > 0x1F))
			{
				prev_r = ax << cl;
			}
			prev_g = *(edi + 1);
			ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
			if (!(ax > *(edi + 3)))
			{
				prev_g = ax << 5;
			}
			cl = *(edi + 5);
			prev_b = *(edi + 2);
			ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
			if (!(ax > 0x1F))
			{
				prev_b = ax << cl;
			}
			*point = prev_r | prev_g | prev_b;
			++point;
			mag += (j << r_x) + 1;
			int tmp_ds = dis_state << 2;
			unsigned short *ebx = base3 + (tmp_ds / 2);
			tmp_ds >>= 1;
			unsigned short *esi = base2 + (tmp_ds / 2);
			tmp_ds >>= 1;
			if (mag >= *(reinterpret_cast<unsigned int *>(ebx)))
			{
				if (mag > *(reinterpret_cast<unsigned int *>(ebx + 2)))
				{
					esi += 2;
					tmp_ds += 2;
				}
				else
				{
					goto D_PLUS;
				}
			}
			--esi;
			--tmp_ds;
			temp_r = *(esi + 1);
			temp_g = *(esi + 8);
			temp_b = *(esi + 15);
			dis_state = tmp_ds;
D_PLUS:
			;  // Empty.
		}
#endif
#ifdef ASM_FOR_MSVC
		_asm
		{
		mov eax, line			//temp_point = temp_point +line;
		add temp_point, eax
		mov cl, r_y
		mov eax, temp_mag		//temp_mag = temp_mag +i +i +1;
		mov ebx, i
		shl ebx, cl
		add eax, ebx
		inc eax
		mov temp_mag, eax
		mov edi, temp_dis_state		//if (temp_mag < *(dist_table +temp_dis_state))
		mov ebx, base3
		shl edi,2
		add ebx, edi
		shr edi,2
		cmp eax, [ebx]		//if (mag > *(color_table +dis_state +1))
		jl	TD_MINS
		cmp eax, [ebx+4]	//if (temp_mag > *(dist_table +temp_dis_state +1))
		jle TD_PLUS
		add edi, 2			//	temp_dis_state ++;
TD_MINS:dec edi				//	temp_dis_state --;	
		mov temp_dis_state, edi
TD_PLUS:
		}
#else
		temp_point += line / 2;
		temp_mag += (i << r_y) + 1;
		int edi = temp_dis_state << 2;
		unsigned short *ebx = base3 + (edi / 2);
		edi >>= 2;
		if (temp_mag >= *(reinterpret_cast<unsigned int *>(ebx)))
		{
			if (temp_mag > *(reinterpret_cast<unsigned int *>(ebx + 2)))
			{
				edi += 2;
			}
			else
			{
				goto TD_PLUS;
			}
		}
		--edi;
		temp_dis_state = edi;
TD_PLUS:
		;  // Empty.
#endif
	}
}	
// ----------- End of function Magic::draw_circle ----------//

// ----------- Begin of function Magic::draw_circle2 -----//
//
// Draw an transparent ellipse of colour R1, G1, B1 with radius rX and rY
// x1, y1 = center of the ellipse
// rX, rY = x and y radius of the ellipse
//	R1, G1, B1 = colour of the circle, value range = 0-255
// mode = pixel format that currently using
//	dir = 1-> ellipse colour intensity increases towards the center
//			0-> ellipse colour intensity decreases towards the center
//
// no return values
//
void Magic::draw_circle2(VgaBuf *vgabuf, int x1, int y1, int radius, char r_x, char r_y,
						short R1, short G1, short B1, int mode, int dir) 
{
	int i, j, line = 2*vgabuf->buf_pitch();
	int mag, temp_mag, up_level;
	short *point, *temp_point;
	short temp_r, temp_g, temp_b;
	short prev_r, prev_g, prev_b;
	int dis_state, temp_dis_state;
	int start_x, end_x;
	int start_y, end_y;
	int mode_offset = mode *12;
	unsigned short *base1 = check_table;
	unsigned short *base2 = color_table;
	unsigned short *base3 = dist_table;
	up_level = 	(radius * radius);
	
	if (r_x < 0)
		r_x = 0;
	if (r_y < 0)
		r_y = 0;
		
#ifdef ASM_FOR_MSVC
	_asm
	{
		mov ax, R1  //R1 = R1 *31 /255;
		shr ax, 3
		mov R1, ax
		mov ax, B1  //B1 = B1 *31 /255;
		shr ax, 3
		mov B1, ax
		cmp mode, 1 // Comparing mode for setting G1 and G2
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G1  //G1 = G1 *31 /255;
		shr ax, 3
		mov G1, ax
		jmp CLIPPING
MODE_ODD:
		mov ax, G1  //G1 = G1 *63 /255;
		shr ax, 2
		mov G1, ax
CLIPPING:
		mov esi, bound_x1	//	start_x = x1 -radius;
	//	add esi, 1
		mov edi, bound_x2
	//	sub edi, 1
		mov ebx, x1			
		mov ecx, radius
		mov eax, ebx
		sub eax, ecx
		cmp eax, esi		// if (start_x < bound_x1)
		jg	BOUND11			
		mov eax, esi		// start_x = bound_x1;
BOUND11:cmp eax, edi		// if (start_x > bound_x2)
		jl	BOUND12
		mov eax, edi		// start_x = bound_x2;
BOUND12:sub eax, ebx		// start_x = start_x -x1;	
		mov start_x, eax
		mov eax, ebx		//	end_x = x1 +radius;
		add eax, ecx
		cmp eax, esi		// if (end_x < bound_x1)
		jg	BOUND21			
		mov eax, esi		// end_x = bound_x1;
BOUND21:cmp eax, edi		// if (end_x > bound_x2)
		jl	BOUND22
		mov eax, edi		// end_x = bound_x2;
BOUND22:sub eax, ebx		// end_x = end_x -x1;	
		mov end_x, eax
		mov esi, bound_y1	//	start_y = y1 -radius;
	//	add esi, 1
		mov edi, bound_y2
	//	sub edi, 1
		mov ebx, y1			
		mov eax, ebx
		sub eax, ecx
		cmp eax, esi		// if (start_y < bound_y1)
		jg	BOUND31			
		mov eax, esi		// start_y = bound_y1;
BOUND31:cmp eax, edi		// if (start_y > bound_y2)
		jl	BOUND32
		mov eax, edi		// start_y = bound_y2;
BOUND32:sub eax, ebx		// start_y = start_y -y1;
		mov start_y, eax	
		mov eax, ebx		//	end_y = y1 +radius;
		add eax, ecx
		cmp eax, esi		// if (end_y < bound_y1)
		jg	BOUND41			
		mov eax, esi		// end_y = bound_y1;
BOUND41:cmp eax, edi		// if (end_y > bound_y2)
		jl	BOUND42
		mov eax, edi		// end_y = bound_y2;
BOUND42:sub eax, ebx		// end_y = end_y -y1;
		mov end_y, eax
		mov esi, base3		//RANGE
		mov ebx, 0
		mov eax, up_level
		inc eax
		shl eax, 1
		mov [esi+24], eax	//*(dist_table +6) = up_level<<1;
		shr eax, 1
		mov [esi+20], eax	//*(dist_table +5) = up_level;
		cmp dir, 0
		je MODE_0
//MODE_1:	
		mov ecx, up_level
		shr ecx, 1
		mov [esi+ 4], eax	//*(dist_table +1) = up_level>>4;
		sub [esi+ 4], ecx
		shr ecx, 1
		mov [esi+ 8], eax	//*(dist_table +2) = up_level>>3;
		sub [esi+ 8], ecx
		shr ecx, 1
		mov [esi+12], eax	//*(dist_table +3) = up_level>>2;
		sub [esi+12], ecx
		shr ecx, 1
		mov [esi+16], eax	//*(dist_table +4) = up_level>>1;
		sub [esi+16], ecx
		mov [esi], ebx
		mov esi, base2		//RED
		mov ax, R1		
		mov [esi+12], bx
		mov [esi+10], ax
		shr ax, 1
		mov [esi+8], ax
		shr ax, 1
		mov [esi+6], ax
		shr ax, 1
		mov [esi+4], ax
		shr ax, 1
		mov [esi+2], ax
		mov [esi], bx
		mov ax, G1			//GREEN
		mov [esi+26], bx
		mov [esi+24], ax
		shr ax, 1
		mov [esi+22], ax
		shr ax, 1
		mov [esi+20], ax
		shr ax, 1
		mov [esi+18], ax
		shr ax, 1
		mov [esi+16], ax
		mov [esi+14], bx
		mov ax, B1			//BLUE
		mov [esi+40], bx
		mov [esi+38], ax
		shr ax, 1
		mov [esi+36], ax
		shr ax, 1
		mov [esi+34], ax
		shr ax, 1
		mov [esi+32], ax
		shr ax, 1
		mov [esi+30], ax
		mov [esi+28], bx
		jmp FINISH
MODE_0:	shr eax, 1
		mov [esi+16], eax	//*(dist_table +4) = up_level>>1;
		shr eax, 1
		mov [esi+12], eax	//*(dist_table +3) = up_level>>2;
		shr eax, 1
		mov [esi+ 8], eax	//*(dist_table +2) = up_level>>3;
		shr eax, 1
		mov [esi+ 4], eax	//*(dist_table +1) = up_level>>4;
		mov [esi], ebx
		mov esi, base2		//RED
		mov ax, R1		
		mov [esi], bx
		mov [esi+2], ax
		shr ax, 1
		mov [esi+4], ax
		shr ax, 1
		mov [esi+6], ax
		shr ax, 1
		mov [esi+8], ax
		shr ax, 1
		mov [esi+10], ax
		mov [esi+12], bx
		mov ax, G1			//GREEN
		mov [esi+14], bx
		mov [esi+16], ax
		shr ax, 1
		mov [esi+18], ax
		shr ax, 1
		mov [esi+20], ax
		shr ax, 1
		mov [esi+22], ax
		shr ax, 1
		mov [esi+24], ax
		mov [esi+26], bx
		mov ax, B1			//BLUE
		mov [esi+28], bx
		mov [esi+30], ax
		shr ax, 1
		mov [esi+32], ax
		shr ax, 1
		mov [esi+34], ax
		shr ax, 1
		mov [esi+36], ax
		shr ax, 1
		mov [esi+38], ax
		mov [esi+40], bx
FINISH:
	}
#else
	R1 >>= 3;
	B1 >>= 3;
	if (mode == 1 || mode == 3)
	{
		G1 >>= 2;
	}
	else
	{
		G1 >>= 3;
	}
	int eax = x1 - radius;
	if (eax <= bound_x1)
	{
		eax = bound_x1;
	}
	if (eax >= bound_x2)
	{
		eax = bound_x2;
	}
	start_x = eax - x1;
	eax = x1 + radius;
	if (eax <= bound_x1)
	{
		eax = bound_x1;
	}
	if (eax >= bound_x2)
	{
		eax = bound_x2;
	}
	end_x = eax - x1;
	eax = y1 - radius;
	if (eax <= bound_y1)
	{
		eax = bound_y1;
	}
	if (eax >= bound_y2)
	{
		eax = bound_y2;
	}
	start_y = eax - y1;
	eax = y1 + radius;
	if (eax <= bound_y1)
	{
		eax = bound_y1;
	}
	if (eax >= bound_y2)
	{
		eax = bound_y2;
	}
	end_y = eax - y1;
	eax = up_level + 1;
	eax <<= 1;
	*(reinterpret_cast<unsigned int *>(base3 + 12)) = eax;
	eax >>= 1;
	*(reinterpret_cast<unsigned int *>(base3 + 10)) = eax;
	if (dir == 0)
	{
		for (int i = 8; i > 0; i -= 2)
		{
			eax >>= 1;
			*(reinterpret_cast<unsigned int *>(base3 + i)) = eax;
		}
		*(reinterpret_cast<unsigned int *>(base3)) = 0;
		*base2 = 0;
		short ax = R1;
		for (int i = 1; i <= 5; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 6) = 0;
		*(base2 + 7) = 0;
		ax = G1;
		for (int i = 8; i <= 12; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 13) = 0;
		*(base2 + 14) = 0;
		ax = B1;
		for (int i = 15; i <= 19; ++i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 20) = 0;
	}
	else
	{
		int ecx = up_level;
		for (int i = 2; i <= 8; i += 2)
		{
			ecx >>= 1;
			*(reinterpret_cast<unsigned int *>(base3 + i)) = eax - ecx;
		}
		*(reinterpret_cast<unsigned int *>(base3)) = 0;
		short ax = R1;
		*(base2 + 6) = 0;
		for (int i = 5; i >= 1; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*base2 = 0;
		ax = G1;
		*(base2 + 13) = 0;
		for (int i = 12; i >= 8; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 7) = 0;
		ax = B1;
		*(base2 + 20) = 0;
		for (int i = 19; i >= 15; --i)
		{
			*(base2 + i) = ax;
			ax >>= 1;
		}
		*(base2 + 14) = 0;
	}
#endif

	if (start_x == end_x || start_y == end_y)
		return;
	
	temp_point = vgabuf->buf_ptr(start_x + x1, start_y + y1);
	temp_mag = start_y * start_y + start_x * start_x;
	
	if (r_y > 0)
		temp_mag = temp_mag + ((((radius - abs(start_y))* (1 +radius + abs(start_y))) >> 1) << 1)
						- ((((radius - abs(start_y))* (1 +radius + abs(start_y))) >> 1) >> (r_y-1));

	if (r_x > 0)
		temp_mag = temp_mag + ((((radius - abs(start_x))* (1 +radius + abs(start_x))) >> 1) << 1)
						- ((((radius - abs(start_x))* (1 +radius + abs(start_x))) >> 1) >> (r_x-1));
		
	temp_dis_state = 0;
	for (i = 1; i < 6; i ++)
		if (temp_mag > (*(dist_table + i)))
			temp_dis_state ++;
	for (i = start_y; i <= end_y; i++)
	{
		mag = temp_mag;
		point = temp_point;
#ifdef ASM_FOR_MSVC
		_asm
		{
		mov esi, temp_dis_state	//dis_state = temp_dis_state;
		mov dis_state, esi
		shl esi, 1
		add esi, base2
		mov ax, [esi+ 2]		//temp_r = *(color_table + 7 +dis_state +1);
		mov temp_r, ax
		mov ax, [esi+16]		//temp_g = *(color_table +14 +dis_state +1);
		mov temp_g, ax
		mov ax, [esi+30]		//temp_b = *(color_table +21 +dis_state +1);
		mov temp_b, ax
		}
#else
		dis_state = temp_dis_state;
		temp_r = *(base2 + ((temp_dis_state << 1) / 2) + 1);
		temp_g = *(base2 + ((temp_dis_state << 1) / 2) + 8);
		temp_b = *(base2 + ((temp_dis_state << 1) / 2) + 15);
#endif
		for (j = start_x; j <= end_x; j++)
#ifdef ASM_FOR_MSVC
		_asm
		{
			mov edi, base1
			add edi, mode_offset
			mov esi, point	//ok
			mov dx, [esi]
//RED:		
			mov cl, [edi +8]
			mov bx, [edi]
			mov prev_r, bx			// assign R_OVER value
			mov ax, dx
			and ax, bx
			shr ax, cl
			add ax, temp_r
			cmp ax, 001fh
			jg  GREEN
			shl ax, cl				// assign R_UNDER value
			mov prev_r, ax
GREEN:		
			mov bx, [edi +2]
			mov prev_g, bx			// assign G_OVER value
			mov ax, dx
			and ax, bx
			shr ax, 5
			add ax, temp_g
			cmp ax, [edi +6]
			jg  BLUE
			shl ax, 5				// assign G_UNDER value
			mov prev_g, ax
BLUE:		mov cl, [edi +10]
			mov bx, [edi +4]
			mov prev_b, bx			// assign B_OVER value
			mov ax, dx
			and ax, bx
			shr ax, cl
			add ax, temp_b
			cmp ax, 001fh
			jg  END
			shl ax, cl				// assign B_UNDER value
			mov prev_b, ax
END:		mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
			or  ax, prev_g			
			or  ax, prev_b
			mov [esi], ax
			inc esi					// point ++;
			inc esi
			mov point, esi
			mov cl, r_x
			mov eax, mag			//mag = mag +j +j +1;
			mov ebx, j
		//	shl ebx, cl
			shl ebx, 1

			sar ebx, cl
	
			add eax, ebx
			inc eax
			mov mag, eax
			mov edi, dis_state		//if (mag < *(color_table +dis_state))
			mov ebx, base3
			shl edi,2
			add ebx, edi
			mov esi, base2			
			shr edi,1
			add esi, edi
			shr edi,1
			cmp eax, [ebx]		
			jl	D_MINS
			cmp eax, [ebx+4]		//if (mag > *(color_table +dis_state +1))
			jle D_PLUS
			add esi, 4				//dis_state ++;
			add edi, 2
D_MINS:	
			sub esi, 2				//dis_state --;
			dec edi
			mov ax, [esi+ 2]		//temp_r = *(color_table + 7 +dis_state +1);
			mov temp_r, ax
			mov ax, [esi+16]		//temp_g = *(color_table +14 +dis_state +1);
			mov temp_g, ax
			mov ax, [esi+30]		//temp_b = *(color_table +21 +dis_state +1);
			mov temp_b, ax
			mov dis_state, edi			
D_PLUS:	
		}
#else
		{
			unsigned short *edi = base1 + (mode_offset / 2);
			unsigned char cl = *(edi + 4);
			prev_r = *edi;
			short ax = (static_cast<unsigned short>(*point & prev_r) >> cl) + temp_r;
			if (!(ax > 0x1F))
			{
				prev_r = ax << cl;
			}
			prev_g = *(edi + 1);
			ax = (static_cast<unsigned short>(*point & prev_g) >> 5) + temp_g;
			if (!(ax > *(edi + 3)))
			{
				prev_g = ax << 5;
			}
			cl = *(edi + 5);
			prev_b = *(edi + 2);
			ax = (static_cast<unsigned short>(*point & prev_b) >> cl) + temp_b;
			if (!(ax > 0x1F))
			{
				prev_b = ax << cl;
			}
			*point = prev_r | prev_g | prev_b;
			++point;
			mag += ((j << 1) >> r_x) + 1;
			int tmp_ds = dis_state << 2;
			unsigned short *ebx = base3 + (tmp_ds / 2);
			tmp_ds >>= 1;
			unsigned short *esi = base2 + (tmp_ds / 2);
			tmp_ds >>= 1;
			if (mag >= *(reinterpret_cast<unsigned int *>(ebx)))
			{
				if (mag > *(reinterpret_cast<unsigned int *>(ebx + 2)))
				{
					esi += 2;
					tmp_ds += 2;
				}
				else
				{
					goto D_PLUS;
				}
			}
			--esi;
			--tmp_ds;
			temp_r = *(esi + 1);
			temp_g = *(esi + 8);
			temp_b = *(esi + 15);
			dis_state = tmp_ds;
D_PLUS:
			;  // Empty.
		}
#endif
#ifdef ASM_FOR_MSVC
		_asm
		{
		mov eax, line			//temp_point = temp_point +line;
		add temp_point, eax
		mov cl, r_y
		mov eax, temp_mag		//temp_mag = temp_mag +i +i +1;
		mov ebx, i
	//	shl ebx, cl
		shl ebx, 1

		sar ebx, cl
		
		add eax, ebx
		inc eax
		mov temp_mag, eax
		mov edi, temp_dis_state		//if (temp_mag < *(dist_table +temp_dis_state))
		mov ebx, base3
		shl edi,2
		add ebx, edi
		shr edi,2
		cmp eax, [ebx]		//if (mag > *(color_table +dis_state +1))
		jl	TD_MINS
		cmp eax, [ebx+4]	//if (temp_mag > *(dist_table +temp_dis_state +1))
		jle TD_PLUS
		add edi, 2			//	temp_dis_state ++;
TD_MINS:dec edi				//	temp_dis_state --;	
		mov temp_dis_state, edi
TD_PLUS:
		}
#else
		temp_point += line / 2;
		temp_mag += ((i << 1) >> r_y) + 1;
		int edi = temp_dis_state << 2;
		unsigned short *ebx = base3 + (edi / 2);
		edi >>= 2;
		if (temp_mag >= *(reinterpret_cast<unsigned int *>(ebx)))
		{
			if (temp_mag > *(reinterpret_cast<unsigned int *>(ebx + 2)))
			{
				edi += 2;
			}
			else
			{
				goto TD_PLUS;
			}
		}
		--edi;
		temp_dis_state = edi;
TD_PLUS:
		;  // Empty.
#endif
	}
/*
	int mag1, mag2;
	int level1, level2;
	int modeOffset = mode *12;
	short *point, *tempPoint;
	short x, y, locX, locY;
	short i, j, stepX, stepY;
	short temp_r, temp_g, temp_b;
	short prev_r, prev_g, prev_b;
	unsigned short *base1 = check_table;
	unsigned short *base2 = color_table;
	unsigned short *base3 = dist_table;
		
	_asm
	{
		// Convert R1, B1 and G1 from 0-255 to 0-31 or 0-63 depending on the pixel format
		//R1 = R1 *31 /255
		mov ax, R1  
		shr ax, 3
		mov R1, ax

		//B1 = B1 *31 /255
		mov ax, B1  
		shr ax, 3
		mov B1, ax

		// Comparing mode for setting G1 and G2
		cmp mode, 1 
		je MODE_ODD
		cmp mode, 3
		je MODE_ODD
		mov ax, G1  

		//G1 = G1 *31 /255
		shr ax, 3
		mov G1, ax
		jmp START

	MODE_ODD:

		//G1 = G1 *63 /255
		mov ax, G1  
		shr ax, 2
		mov G1, ax

	START:
		// Preparing the 17 entries RGB colour array 
		mov ebx, 0
		mov esi, base2		
		
		//RED
		mov ax, R1	
		mov cx, ax
		shr cx, 4
		
		mov [esi+34], bx
		mov [esi+32], ax
		sub ax, cx
		mov [esi+30], ax
		sub ax, cx
		mov [esi+28], ax
		sub ax, cx
		mov [esi+26], ax
		sub ax, cx
		mov [esi+24], ax
		sub ax, cx
		mov [esi+22], ax
		sub ax, cx
		mov [esi+20], ax
		sub ax, cx
		mov [esi+18], ax
		sub ax, cx
		mov [esi+16], ax
		sub ax, cx
		mov [esi+14], ax
		sub ax, cx
		mov [esi+12], ax
		sub ax, cx
		mov [esi+10], ax
		sub ax, cx
		mov [esi+8], ax
		sub ax, cx
		mov [esi+6], ax
		sub ax, cx
		mov [esi+4], ax
		sub ax, cx
		mov [esi+2], ax
		mov [esi], cx

		//GREEN
		mov ax, G1	
		mov cx, ax
		shr cx, 4
		
		mov [esi+70], bx
		mov [esi+68], ax
		sub ax, cx
		mov [esi+66], ax
		sub ax, cx
		mov [esi+64], ax
		sub ax, cx
		mov [esi+62], ax
		sub ax, cx
		mov [esi+60], ax
		sub ax, cx
		mov [esi+58], ax
		sub ax, cx
		mov [esi+56], ax
		sub ax, cx
		mov [esi+54], ax
		sub ax, cx
		mov [esi+52], ax
		sub ax, cx
		mov [esi+50], ax
		sub ax, cx
		mov [esi+48], ax
		sub ax, cx
		mov [esi+46], ax
		sub ax, cx
		mov [esi+44], ax
		sub ax, cx
		mov [esi+42], ax
		sub ax, cx
		mov [esi+40], ax
		sub ax, cx
		mov [esi+38], ax
		mov [esi+36], cx

		//BLUE
		mov ax, B1	
		mov cx, ax
		shr cx, 4
		
		mov [esi+106], bx
		mov [esi+104], ax
		sub ax, cx
		mov [esi+102], ax
		sub ax, cx
		mov [esi+100], ax
		sub ax, cx
		mov [esi+98], ax
		sub ax, cx
		mov [esi+96], ax
		sub ax, cx
		mov [esi+94], ax
		sub ax, cx
		mov [esi+92], ax
		sub ax, cx
		mov [esi+90], ax
		sub ax, cx
		mov [esi+88], ax
		sub ax, cx
		mov [esi+86], ax
		sub ax, cx
		mov [esi+84], ax
		sub ax, cx
		mov [esi+82], ax
		sub ax, cx
		mov [esi+80], ax
		sub ax, cx
		mov [esi+78], ax
		sub ax, cx
		mov [esi+76], ax
		sub ax, cx
		mov [esi+74], ax
		mov [esi+72], cx
	}

	mag1 =512; level1 =16;
	mag2 =512; level2 =16;
	locX = (x1 - rX)<<4;	stepX = rX;
	locY = (y1 - rY)<<4;	stepY = rY;
	for(y =-16; y <17; y++)
	{
		for(x =-16; x <17; x++)
		{
			_asm
			{
				mov esi, level2
				shl esi, 1
				add esi, base2
				mov ax, [esi+ 2]		//temp_r = *(color_table + 7 +dis_state +1);
				mov temp_r, ax
				mov ax, [esi+38]		//temp_g = *(color_table +14 +dis_state +1);
				mov temp_g, ax
				mov ax, [esi+74]		//temp_b = *(color_table +21 +dis_state +1);
				mov temp_b, ax
			}
			//draw one pixel
			tempPoint = vgabuf->buf_ptr(locX >>4, locY >>4);
			point = tempPoint;
			for(j =0; j <(rY>>4); j++)
			{
				for(i =0; i <(rX>>4); i++)
				{
					_asm
					{
						mov edi, base1
						add edi, modeOffset
						mov esi, point
						mov dx, [esi]
					//RED:		
						mov cl, [edi +8]
						mov bx, [edi]
						mov prev_r, bx			// assign R_OVER value
						mov ax, dx
						and ax, bx
						shr ax, cl
						add ax, temp_r
						cmp ax, 001fh
						jg  GREEN
						shl ax, cl				// assign R_UNDER value
						mov prev_r, ax
					GREEN:
						mov bx, [edi +2]
						mov prev_g, bx			// assign G_OVER value
						mov ax, dx
						and ax, bx
						shr ax, 5
						add ax, temp_g
						cmp ax, [edi +6]
						jg  BLUE
						shl ax, 5				// assign G_UNDER value
						mov prev_g, ax
					BLUE:
						mov cl, [edi +10]
						mov bx, [edi +4]
						mov prev_b, bx			// assign B_OVER value
						mov ax, dx
						and ax, bx
						shr ax, cl
						add ax, temp_b
						cmp ax, 001fh
						jg  END
						shl ax, cl				// assign B_UNDER value
						mov prev_b, ax
					END:		
						mov ax, prev_r			//*point = prev_r | prev_g | prev_b;
						or  ax, prev_g			
						or  ax, prev_b
						mov [esi], ax
					}
					point ++;
				}
				tempPoint = tempPoint + vgabuf->buf_pitch();
				point = tempPoint;
			}
			locX = locX + rX;
			mag2 = mag2 +x +x +1;
			if (mag2 > dist_table[level2 +1])
				level2 ++;
			else
			if (mag2 < dist_table[level2])
				level2 --;
		}
		mag1 = mag1 +y +y +1;
		if (mag1 > dist_table[level1 +1])
			level1 ++;
		else
		if (mag1 < dist_table[level1])
			level1 --;
		mag2 = mag1;
		level2 = level1;
		locX = (x1 - rX)<<4;
		locY = locY + rY;
	}*/
}
// ----------- End of function Magic::draw_circle2 ----------//

// ----------- Begin of function Magic::generate_lighting_particle ----------//
void Magic::generate_lighting_particle(int x1, int y1, int x2, int y2, int number, int lifecount,
									   Lighting_Particle *h_l_particle) 
{
	int i, limit, internal_count;
	int temp_r1, temp_rx, temp_ry;
	int step_x[16], step_y[16];
	int temp_step_x = x1;
	int temp_step_y = y1;
	int temp_x = x1;
	int temp_y = y1;
	Lighting_Particle *temp_ptr;
	temp_r1 =0;
	internal_count = 4;
	limit = number <<3;

	step_x[0] = 0;
	step_y[0] = 0;
	step_x[1] = (x2 - x1) /number;
	step_y[1] = (y2 - y1) /number;
	for (i = 2; i< 16; i++)
	{
		step_x[i] = step_x[i-1] +step_x[1];
		step_y[i] = step_y[i-1] +step_y[1];
	}
	for (i = 0; i< (number-1); i ++)
	{
		temp_ptr = h_l_particle + i;
		temp_ptr->pos_x = x2;
		temp_ptr->pos_y = y2;
		temp_ptr->prev_x = temp_x;									//assigning prev_position
		temp_ptr->prev_y = temp_y;
		if (temp_r1 <limit)
		{
			temp_ptr->random = (int)(rand_seed() %16);//random output of -10 to 10
			temp_r1 = temp_r1 +temp_ptr->random;
			temp_rx = step_x[temp_ptr->random];
			temp_ry = step_y[temp_ptr->random];
			temp_ptr->dir_x = (temp_ry -(step_y[1]<<3))>>5; //(temp_r1 - 8) * step_y / 32
			temp_ptr->dir_y = ((step_x[1]<<3) -temp_rx)>>5; //(temp_r1 - 8) *-step_x / 32
			temp_ptr->pos_x = temp_ptr->prev_x +(temp_rx>>3) +temp_ptr->dir_x *lifecount;	//assigning current_position
			temp_ptr->pos_y = temp_ptr->prev_y +(temp_ry>>3) +temp_ptr->dir_y *lifecount;	//temp_r1 *(y2-y1) /(number *8) + temp_ptr->dir_y *lifecount
		}
		temp_x = temp_ptr->pos_x;
		temp_y = temp_ptr->pos_y;
	}
	for (i = (number -1); i< (number+2); i ++)
	{
		temp_ptr = h_l_particle +i;
		temp_ptr->pos_x = x2;
		temp_ptr->pos_y = y2;
		temp_ptr->prev_x = temp_x;
		temp_ptr->prev_y = temp_y;
	}
}
// ----------- End of function Magic::generate_particle ----------//
// ----------- Begin of function Magic::draw_particle ----------//
void Magic::draw_lighting(int x1, int y1, int x2, int y2, int thickness, int lightness, short R1, short G1, short B1, 
						  short R2, short G2, short B2, VgaBuf *vgabuf, int var, int mode, int number, int lifecount,
						  int level)
{
	Lighting_Particle *particle, *particle1, *h_l_particle;
	int i;
	int temp, temp1;
	short R3 =(R1 +(R2>>1))>>1, G3 =(G1 +(G2>>1))>>1,B3 =(B1 +(B2>>1))>>1;
	seed = (unsigned)(x1 +x2 +y1 +y2 +var);
	(void) rand_seed();
	h_l_particle = (Lighting_Particle *) mem_add( sizeof(Lighting_Particle) *(number +2));
	generate_lighting_particle( x1, y1, x2, y2, number, lifecount, h_l_particle);
	for (i = 0; i < number; i++)
	{
		particle = h_l_particle +i;
		draw_light_beam(vgabuf, particle->prev_x, particle->prev_y, particle->pos_x, particle->pos_y,
						thickness, lightness, R1, G1, B1, R2, G2, B2, mode, 00);
		draw_circle(vgabuf, particle->pos_x, particle->pos_y, thickness, 1, 1, R3, G3, B3, mode, 0);
	}
	thickness = thickness>>1;
	lightness = lightness>>1;
	R1 = R1>>1; G1 = G1>>1; B1 = B1>>1;
	temp = 1 +(number>>1);
	temp1 = level -1;
	if (level > 0)
	{
		for (i = 0; i < number; i++)
		{
			particle = h_l_particle +i;
			particle1 = particle +2;
			draw_lighting(particle->prev_x, particle->prev_y, particle1->pos_x, particle1->pos_y,
						thickness, lightness, R1, G1, B1, R2, G2, B2, vgabuf, level, mode, temp,
						lifecount, temp1);
		}
	}
	mem_del(h_l_particle);
	h_l_particle = NULL;
}		
// ----------- End of function Magic::draw_particle -----//
// ----------- Begin of function Magic::draw_magic_one -----//
void Magic::draw_magic_one(VgaBuf *vgabuf, int x1, int y1, int mode, int count) 
{
	if (count == 6)
	{
		draw_light_beam(vgabuf, x1-100, y1-200, x1, y1,
							10, 2, 255, 80, 0, 255, 255, 255, mode, 0);
		draw_light_beam(vgabuf, x1+100, y1-200, x1, y1,
							10, 2, 255, 80, 0, 255, 255, 255, mode, 0);
		draw_light_beam(vgabuf, x1-150, y1+100, x1, y1,
							10, 2, 255, 80, 0, 255, 255, 255, mode, 0);
		draw_light_beam(vgabuf, x1+150, y1+100, x1, y1,
							10, 2, 255, 80, 0, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1-100, y1-200, 17, 1, 1, 255, 200, 0, mode, 1);
		draw_circle(vgabuf, x1+100, y1-200, 17, 1, 1, 255, 200, 0, mode, 1); 
		draw_circle(vgabuf, x1-150, y1+100, 25, 1, 1, 255, 200, 0, mode, 1); 
		draw_circle(vgabuf, x1+150, y1+100, 25, 1, 1, 255, 200, 0, mode, 1); 
		draw_particle(x1-100, y1-200, x1-100, y1-200, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 0, 20);
		draw_particle(x1+100, y1-200, x1+100, y1-200, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 0, 20);
		draw_particle(x1-150, y1+100, x1-150, y1+100, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 0, 20);
		draw_particle(x1+150, y1+100, x1+150, y1+100, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 0, 20);
	}
	if ((count == 6) || (count == 7))
		draw_circle(vgabuf, x1, y1, 40, 1, 1, 255, 255, 255, mode, 0);
	if ((count >5) && (count <16))
		draw_circle(vgabuf, x1, y1, (count -4)<<3, 0, 1, 255 -25 *(count -6), 255 -25 *(count -6), 10, mode, 1);
	if ((count >0) && (count <6))
	{
		draw_circle(vgabuf, x1-100, y1-200, 17, 1, 1, 255, 200, 0, mode, 1);
		draw_circle(vgabuf, x1+100, y1-200, 17, 1, 1, 255, 200, 0, mode, 1); 
		draw_circle(vgabuf, x1-150, y1+100, 25, 1, 1, 255, 200, 0, mode, 1); 
		draw_circle(vgabuf, x1+150, y1+100, 25, 1, 1, 255, 200, 0, mode, 1); 
		draw_particle(x1-100, y1-200, x1-100, y1-200, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 6 -count, 20);
		draw_particle(x1+100, y1-200, x1+100, y1-200, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 6 -count, 20);
		draw_particle(x1-150, y1+100, x1-150, y1+100, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 6 -count, 20);
		draw_particle(x1+150, y1+100, x1+150, y1+100, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 6 -count, 20);
	}
	if ((count > 6) && (count <21))
		draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 1, 10, 0, vgabuf, mode, 100, 2, count -6, 6);
}
// ----------- End of function Magic::draw_magic_one -----//
// ----------- Begin of function Magic::draw_magic_two -----//
void Magic::draw_magic_two(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int diff, int mode, int curStep, int totalStep) 
{
	//shift all coordinates to the ZOOM display
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	x2 = x2 + bound_x1;
	y2 = y2 + bound_y1;

	int count = curStep * 20 / totalStep;
	
//	if ((count == 3) || (count == 6))
//		vgabuf->bar_alpha( bound_x1, bound_y1, bound_x2, bound_y2, 1, V_BLACK );

	if (count == 3)
	{
		draw_lighting(x1, y1, x2+diff, y2+diff, 8, 2, 30, 30, 00, 180, 180, 180,
						vgabuf, 27, mode, 10, 3, 1);
		draw_circle(vgabuf, x2+diff, y2+diff, 30, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count > 3) && (count <6))
	{
		draw_lighting(x1, y1, x2+diff, y2+diff, 8, 2, 127, 127, 00, 255, 255, 255,
						vgabuf, (count-3) * 27, mode, 10, 3, 1);
		draw_circle(vgabuf, x2+diff, y2+diff, 40, 1, 1, 255, 255, 255, mode, 0);
	}
	if (count == 6)
	{
		draw_lighting(x1, y1, x2-diff, y2-diff, 8, 2, 30, 30, 00, 180, 180, 180,
						vgabuf, 117, mode, 10, 3, 1);
		draw_circle(vgabuf, x2-diff, y2-diff, 30, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count > 6) && (count <9))
	{
		draw_lighting(x1, y1, x2-diff, y2-diff, 8, 2, 127, 127, 00, 255, 255, 255,
						vgabuf, (count-6) * 117, mode, 10, 3, 1);
		draw_circle(vgabuf, x2-diff, y2-diff, 40, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >0) && (count <4))
	{
//		draw_light_beam(&vga_back, x1, y1, x1, y1-count*15,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1+count*15, y1,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1, y1+count*15,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1-count*15, y1,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_circle(vgabuf, x1, y1, (10 *count), 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1 +5 -(10 *count), (12 *count), 1, 1, 255, 255, 255, mode, 1);
	}
	if ((count >3) && (count <10))
	{
//		draw_light_beam(&vga_back, x1, y1, x1, y1-45,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1+45, y1,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1, y1+45,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
//		draw_light_beam(&vga_back, x1, y1, x1-45, y1,
//						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_circle(vgabuf, x1, y1, 50, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1 -35, 50, 1, 1, 255, 255, 255, mode, 1);
	}
	if ((count > 3) && (count <18))
		draw_particle(x2+diff+10, y2+diff+10, x2+diff-10, y2+diff-10, 0, 0, 1, 10, 0, 
						vgabuf, mode, 100, 2, count-3, 6);
	if ((count > 6) && (count <21))
		draw_particle(x2-diff+10, y2-diff+10, x2-diff-10, y2-diff-10, 0, 0, 1, 10, 0, 
						vgabuf, mode, 100, 2, count-6, 6);
}
// ----------- End of function Magic::draw_magic_two -----//
// ----------- Begin of function Magic::draw_magic_three -----//
void Magic::draw_magic_three(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
	if ((count >0) && (count <7))
	{
		draw_circle(vgabuf, x1, y1, 55, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 35, 0, 1, 255, 0, 0, mode, 1);
	}
	if (count == 1)
	{
		draw_light_beam(vgabuf, x1, y1, x2, y2,	25, 0, 127, 127, 0, 127, 127, 127, mode, 2);
		draw_circle(vgabuf, x2, y2, 35, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >0) && (count <7))
	{
		draw_circle(vgabuf, (x1-x2)/3+x2, (y1-y2)/3+y2, (count+1)<<3, 0, 1, 255 -50 *(count -1), 255 -50 *(count -1), 10, mode, 1);
		draw_circle(vgabuf, (x1-x2)*2/3+x2, (y1-y2)*2/3+y2, (count+2)<<3, 0, 1, 255 -50 *(count -1), 255 -50 *(count -1), 10, mode, 1);
	}

	x1 =x1 -100;
	if ((count >5) && (count <12))
	{
		draw_circle(vgabuf, x1, y1, 55, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 35, 0, 1, 255, 0, 0, mode, 1);
	}
	if (count == 6)
	{
		draw_light_beam(vgabuf, x1, y1, x2, y2,	25, 0, 127, 127, 0, 127, 127, 127, mode, 2);
		draw_circle(vgabuf, x2, y2, 35, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >5) && (count <12))
	{
		draw_circle(vgabuf, (x1-x2)/3+x2, (y1-y2)/3+y2, (count-4)<<3, 0, 1, 255 -50 *(count -6), 255 -50 *(count -6), 10, mode, 1);
		draw_circle(vgabuf, (x1-x2)*2/3+x2, (y1-y2)*2/3+y2, (count-3)<<3, 0, 1, 255 -50 *(count -6), 255 -50 *(count -6), 10, mode, 1);
	}
	x1 =x1 +200;
	if ((count >10) && (count <17))
	{
		draw_circle(vgabuf, x1, y1, 55, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 35, 0, 1, 255, 0, 0, mode, 1);
	}
	if (count == 11)
	{
		draw_light_beam(vgabuf, x1, y1, x2, y2,	25, 0, 127, 127, 0, 127, 127, 127, mode, 2);
		draw_circle(vgabuf, x2, y2, 35, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >10) && (count <17))
	{
		draw_circle(vgabuf, (x1-x2)/3+x2, (y1-y2)/3+y2, (count-9)<<3, 0, 1, 255 -50 *(count -11), 255 -50 *(count -11), 10, mode, 1);
		draw_circle(vgabuf, (x1-x2)*2/3+x2, (y1-y2)*2/3+y2, (count-8)<<3, 0, 1, 255 -50 *(count -11), 255 -50 *(count -11), 10, mode, 1);
	}
	if ((count >0) && (count <21))
		draw_particle(x2+5, y2+5, x2-5, y2-5, 0, 0, 1, 10, 0, vgabuf, mode, 100, 2, count, 9);
	if ((count >5) && (count <21))
		draw_particle(x2+5, y2+5, x2-5, y2-5, 0, 0, 1, 10, 0, vgabuf, mode, 100, 2, count -5, 7);
	if ((count >10) && (count <21))
		draw_particle(x2+5, y2+5, x2-5, y2-5, 0, 0, 1, 10, 0, vgabuf, mode, 100, 2, count -10, 6);
}
// ----------- End of function Magic::draw_magic_three -----//
// ----------- Begin of function Magic::draw_magic_four -----//
void Magic::draw_magic_four(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
	int step_x = (x2 - x1)>>3;
	int step_y = (y2 - y1)>>3;
	int avg = (step_x + step_y)>>2;
	if ((count >0) && (count <3))
	{
		draw_circle(vgabuf, x1, y1, 53, 1, 1, 255, 0, 255, mode, 1);
		draw_circle(vgabuf, x1, y1, 40, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1-50, y1, x1-10, y1, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1-50, y1, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1+50, y1, x1+10, y1, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1+50, y1, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1, y1-50, x1, y1-10, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1, y1-50, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1, y1+50, x1, y1+10, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);	
		draw_circle(vgabuf, x1, y1+50, 10, 1, 1, 255, 255, 255, mode, 0);
	}
	x1 = x1+(count-2)*step_x;
	y1 = y1+(count-2)*step_y;
	if ((count >2) && (count <10))
	{
		draw_lighting(x1-33, y1, x1-10, y1, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1-33, y1, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1+33, y1, x1+10, y1, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1+33, y1, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1, y1-50, x1, y1-10, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x1, y1-50, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x1, y1+50, x1, y1+10, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);	
		draw_circle(vgabuf, x1, y1+50, 10, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 53, 1, 0, 255, 0, 255, mode, 1);
		draw_circle(vgabuf, x1, y1, 40, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >2) && (count <11))
		draw_particle(x1+5, y1+5, x1-5, y1-5, -step_x, -step_y, 0, -avg, 0, vgabuf, mode, 100, 2, 3, 15);
	if ((count >9) && (count <21))
		draw_circle(vgabuf, x2, y2, (count-8)<<3, 0, 1, 255 -22 *(count -10), 10, 255 -22 *(count -10), mode, 1);
	if ((count >9) && (count <21))
		draw_particle(x2+5, y2+5, x2-5, y2-5, -step_x>>1, -step_y>>1, 1, -avg, 0, vgabuf, mode, 100, 2, count-9, 5);
}
// ----------- End of function Magic::draw_magic_four -----//
// ----------- Begin of function Magic::draw_magic_five -----//
void Magic::draw_magic_five(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
	int step_x = (x1 - x2)>>2;
	int step_y = (y1 - y2)>>2;
	int avg = (step_x + step_y)>>1;
	draw_particle(x2+ 8, y2+ 8, x2-10, y2-10, step_x>>1, step_y>>1, 2, avg, 40 -(count<<1), vgabuf, mode, 15, 2, count-1, 8);
	draw_particle(x2+10, y2+10, x2- 8, y2- 8, step_x>>1, step_y>>1, 3, avg, 20, vgabuf, mode, 5, 2, count-1, 8);
}
// ----------- End of function Magic::draw_magic_five -----//
// ----------- Begin of function Magic::draw_magic_six -----//
void Magic::draw_magic_six(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
	if ((count >10) && (count <15))
	{
		draw_circle(vgabuf, x1, y1, 25 +(count<<3), 1, 1, 55 +5 *(count-10), 255 -(count-10)*50, 255 -(count-10)*50, mode, 0);
		draw_circle(vgabuf, x1, y1, 15 +(count<<3), 0, 1, 35 +5 *(count-10), 0, 0, mode, 1);
	}
	if ((count >0) && (count <11))	
	{
		draw_light_beam(vgabuf, x1, y1, (x2-x1)*count/10 +x1, (y2-y1)*count/10 +y1,	25, 0, 127, 127, 0, 127, 127, 127, mode, 2);
		draw_circle(vgabuf, x1, y1, 55, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 35, 0, 1, 255, 0, 0, mode, 1);
	}
	if (count ==10)
		draw_circle(vgabuf, x2, y2, 35, 1, 1, 255, 255, 255, mode, 0);
	if ((count >3) && (count <8))
		draw_circle(vgabuf, (x1-x2)*5/6+x2, (y1-y2)*5/6+y2, (count+1)<<3, 0, 1, 255 -70 *(count -4), 255 -70 *(count -4), 10, mode, 1);
	if ((count >4) && (count <9))
		draw_circle(vgabuf, (x1-x2)*4/6+x2, (y1-y2)*4/6+y2, (count-1)<<3, 0, 1, 255 -70 *(count -5), 255 -70 *(count -5), 10, mode, 1);
	if ((count >5) && (count <10))
		draw_circle(vgabuf, (x1-x2)*3/6+x2, (y1-y2)*3/6+y2, (count-3)<<3, 0, 1, 255 -70 *(count -6), 255 -70 *(count -6), 10, mode, 1);
	if ((count >7) && (count <12))
		draw_circle(vgabuf, (x1-x2)*2/6+x2, (y1-y2)*2/6+y2, (count-6)<<3, 0, 1, 255 -70 *(count -8), 255 -70 *(count -8), 10, mode, 1);
	if ((count >8) && (count <13))
		draw_circle(vgabuf, (x1-x2)*1/6+x2, (y1-y2)*1/6+y2, (count-8)<<3, 0, 1, 255 -70 *(count -9), 255 -70 *(count -9), 10, mode, 1);
	if ((count >10) && (count <21))
		draw_particle(x2+5, y2+5, x2-5, y2-5, 0, 0, 1, 10, 0, vgabuf, mode, 100, 2, count-10, 5);
}
// ----------- End of function Magic::draw_magic_six -----//

// ----------- Begin of function Magic::draw_magic_seven -----//
void Magic::draw_magic_seven(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
//	IMGline(vgabuf->buf_ptr(), vgabuf->buf_true_pitch(), vgabuf->buf_width(),
//						vgabuf->buf_height(), x1-20, y1, x1+20, y1, 0xFFFF);
//	draw_circle(vgabuf, x1, y1, 30, 1, 1, 155, 155, 255, mode, 0);

	int totalNum = 20;
	int totalFrame = 20;
	int dirx = -10;
	int diry = (bound_y2 - bound_y1) / totalFrame;
	int gap = (bound_x2 - bound_x1) / totalNum;
	
	int color[3][3] = {{255, 55, 55}, { 55, 55,255}, { 55 ,255, 55}}; 
	
	seed = (unsigned)(x1 +x2 +y1 +y2);
	(void) rand_seed();

	for (int i = 0; i < totalNum; i++)
	{
		int col = (int)(rand_seed() % 3);
		int step = (int)(rand_seed() % totalFrame) + count;
		
		int curX = step * dirx + gap * i;
		int curY = step * diry;
		
		if (curX < bound_x1)
			curX = bound_x2 - (bound_x1 - curX);
		if (curY < bound_y1)
			curY = bound_y2 - (bound_y1 - curY);
		if (curX > bound_x2)
			curX = bound_x1 + (curX - bound_x2);
		if (curY > bound_y2)
			curY = bound_y1 + (curY - bound_y2);

		int frame = (int)((rand_seed() + count)%15);
		if ((frame >1) && (frame <4))
		{
			draw_circle(vgabuf, curX, curY, 15, 1, 1, 255, 255, 255, mode, 0);
		}	
		if ((frame >3) && (frame <6))
		{
			draw_circle(vgabuf, curX, curY, 15, 1, 1, 255, 255, 255, mode, 0);
			draw_circle(vgabuf, curX, curY, 10, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		}
		if ((frame >5) && (frame <8))
		{
			draw_circle(vgabuf, curX, curY, 15, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
			draw_circle(vgabuf, curX, curY, 15, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
			draw_light_beam(vgabuf, curX, curY, curX-20, curY, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX+20, curY, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
		}	
	
		if ((frame >7) && (frame <10))
		{
			draw_circle(vgabuf, curX, curY, 40, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
			draw_circle(vgabuf, curX, curY, 10, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
			draw_light_beam(vgabuf, curX, curY, curX-20, curY, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX+20, curY, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
		}	

		if ((frame >9) && (frame <12))
		{
			draw_circle(vgabuf, curX, curY, 30, 1, 1, 255, 255, 255, mode, 0);
			draw_circle(vgabuf, curX+8, curY+8, 12, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
			draw_light_beam(vgabuf, curX, curY, curX-30, curY, 5, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX+30, curY, 5, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX, curY-30, 5, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX, curY+30, 5, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		}

		if ((frame > 11) && (frame <14))
		{
			draw_circle(vgabuf, curX, curY, 20, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
			draw_circle(vgabuf, curX, curY, 30, 1, 1, 255, 255, 255, mode, 0);
		//	draw_circle(vgabuf, curX+8, curY+8, 12, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
			draw_circle(vgabuf, curX+8, curY+8, 12, 1, 1, 255, 255, 255, mode, 0);
			draw_light_beam(vgabuf, curX, curY, curX-40, curY, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX+40, curY, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX, curY-40, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
			draw_light_beam(vgabuf, curX, curY, curX, curY+40, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		}	
	}
}
// ----------- End of function Magic::draw_magic_seven -----//

// ----------- Begin of function Magic::draw_magic_eight -----//
void Magic::draw_magic_eight(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count) 
{
//	draw_circle2(vgabuf, x1-100, y1,		 100, 100, 255, 055, 055, mode, 1);
//	draw_circle2(vgabuf, x1-100, y1+200, 100, 100, 255, 055, 055, mode, 1);

//	draw_circle2(vgabuf, x1-100, y1+400, 100, 100, 255, 055, 055, mode, 1);
//	draw_circle2(vgabuf, x1+100, y1+200, 100, 100, 255, 055, 055, mode, 0);
//	draw_circle2(vgabuf, x1+100, y1,		 100, 100, 255, 055, 055, mode, 0);

//1. why circle can't use 0,1 
//2. why when 0,1 occurs clipping problem

// ----------- End of function Magic::draw_magic_eight -----//

	draw_circle2(vgabuf, x1, y1, 80, config.frame_speed /3, 0, 255,   0,   0, mode, 1);
	draw_circle2(vgabuf, x1, y1, 80, 0, config.frame_speed /3,   0, 255,   0, mode, 1);
	draw_circle2(vgabuf, x1, y1, 80, 0, 0,   0,   0, 255, mode, 1);

	draw_circle2(vgabuf, x1, y1+300, 80, config.frame_speed /3, 0, 255,   0,   0, mode, 1);
	draw_circle2(vgabuf, x1, y1+300, 80, 0, config.frame_speed /3,   0, 255,   0, mode, 1);
	draw_circle2(vgabuf, x1, y1+300, 80, 0, 0,   0,   0, 255, mode, 1);
}

// ----------- Begin of function Magic::draw_magic_nine -----//
void Magic::draw_magic_nine(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int curStep, int totalStep) 
{
	//shift all coordinates to the ZOOM display
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	x2 = x2 + bound_x1;
	y2 = y2 + bound_y1;

	int count = curStep * 5 / totalStep;

	//steps in drawing the lighting effect
	if (count == 3)
	{
		draw_lighting(x1, y1, x2, y2, 4, 1, 30, 30, 00, 180, 180, 180,
						vgabuf, 27, mode, 12, 3, 2);
		draw_circle(vgabuf, x2, y2, 10, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count > 3) && (count <6))
	{
		draw_lighting(x1, y1, x2, y2, 4, 1, 127, 127, 00, 255, 255, 255,
						vgabuf, (count+3) * 27, mode, 12, 3, 2);
		draw_circle(vgabuf, x2, y2, 20, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count >0) && (count <4))
	{
		draw_light_beam(vgabuf, x1, y1, x1, y1-count*10,
						5, 2, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(vgabuf, x1, y1, x1+count*10, y1,
						5, 2, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(vgabuf, x1, y1, x1, y1+count*10,
						5, 2, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(vgabuf, x1, y1, x1-count*10, y1,
						5, 2, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_circle(vgabuf, x1, y1, (12 *count), 0, 1, 255, 255, 255, mode, 0);
	}
	if ((count >2) && (count <6))
	{
		draw_circle(vgabuf, x1, y1, 20, 1, 1, 127, 127, 0, mode, 1);
		draw_circle(vgabuf, x1, y1, 25, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1-7, y1+7, 10, 1, 1, 255, 255, 255, mode, 0);
	}
}
// ----------- End of function Magic::draw_magic_nine -----//

// ----------- Begin of function Magic::draw_magic_nine_die -----//
void Magic::draw_magic_nine_die(VgaBuf *vgabuf, int x1, int y1, int mode, int count) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;

	if (count <15)
		draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 1, 10, 0, vgabuf, mode, 50, 2, count, 6);
}
// ----------- End of function Magic::draw_magic_nine_die -----//

// ----------- Begin of function Magic::draw_magic_ten -----//
void Magic::draw_magic_ten(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int curStep, int totalStep, int delay) 
{
	//shift all coordinates to the ZOOM display
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	x2 = x2 + bound_x1;
	y2 = y2 + bound_y1;

	int count = curStep;

	if ((count >0) && (count <= delay))
		draw_particle(x1-5, y1-120, x1+5, y1-110, 0, 0, 0, 6, 0, vgabuf, mode, 100, 2, 9 -curStep, 20);

	if ((count == 6) || (count == 7))
		draw_circle(vgabuf, x1, y1-405, 40, 1, 1, 255, 255, 255, mode, 0);
	
	if ((count >5) && (count <16))
		draw_circle(vgabuf, x1, y1-405, (count -4)<<3, 1, 1, 255 -25 *(count -6), 255 -25 *(count -6), 10, mode, 1);

	if (count == 6)
		draw_lighting(x1, y1-110, x1+5, y1-400, 8, 2, 30, 30, 00, 180, 180, 180, vgabuf, 27, mode, 10, 3, 1);
	
	if ((count > 6) && (count <9))
	{
		draw_lighting(x1, y1-110, x1+5, y1-400, 8, 2, 127, 127, 00, 255, 255, 255, vgabuf, (count-6) * 27, mode, 10, 3, 1);
		draw_circle(vgabuf, x1, y1-405, 40, 1, 1, 255, 255, 255, mode, 0);
	}
	if (count == 9)
	{
		draw_lighting(x1, y1-110, x1-5, y1-400, 8, 2, 30, 30, 00, 180, 180, 180, vgabuf, 117, mode, 10, 3, 1);
		draw_circle(vgabuf, x1, y1-405, 30, 1, 1, 255, 255, 255, mode, 0);
	}
	if ((count > 9) && (count <12))
	{
		draw_lighting(x1, y1-110, x1-5, y1-400, 8, 2, 127, 127, 00, 255, 255, 255, vgabuf, (count-9) * 117, mode, 10, 3, 1);
		draw_circle(vgabuf, x1, y1-405, 40, 1, 1, 255, 255, 255, mode, 0);
	}

	if ((count >1) && (count <5))
		draw_circle(vgabuf, x1, y1-115, (10 *(count)), 1, 1, 255, 255, 255, mode, 0);
	
	if ((count >4) && (count <16))
		draw_circle(vgabuf, x1, y1-115, 60, 1, 1, 255, 255, 255, mode, 0);
			
	if ((count >6) && (count <13))
	{
		draw_light_beam(&vga_back, x1, y1-405, x1, y1-45-405,
						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(&vga_back, x1, y1-405, x1+45, y1-405,
						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(&vga_back, x1, y1-405, x1, y1+45-405,
						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
		draw_light_beam(&vga_back, x1, y1-405, x1-45, y1-405,
						10, 4, 127, 127, 0, 255, 255, 255, vga.pixel_format_flag, 02);
	}

	if ((count >6) && (count <16))
		draw_circle(vgabuf, x1, y1-405, 50, 1, 1, 255, 255, 255, mode, 0);

	if ((count >11) && (count <16))
	{
		draw_light_beam(vgabuf, x1, y1-400, x2, y2, 25, 0, 127, 127, 0, 127, 127, 127, mode, 2, 0);
		draw_circle(vgabuf, x2, y2, 53- 10 * (count-11), 1, 1, 255, 0, 255, mode, 1);
		draw_circle(vgabuf, x2, y2, 40- 10 * (count-11), 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x2-50 + 10 * (count-11), y2, x2-10 + 2 * (count-11), y2, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x2-50 + 10 * (count-11), y2, 10 - 2 * (count-11), 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x2+50 - 10 * (count-11), y2, x2+10 - 2 * (count-11), y2, 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x2+50 - 10 * (count-11), y2, 10 - 2 * (count-11), 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x2, y2-50 + 10 * (count-11), x2, y2-10 + 2 * (count-11), 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);
		draw_circle(vgabuf, x2, y2-50 + 10 * (count-11), 10 - 2 * (count-11), 1, 1, 255, 255, 255, mode, 0);
		draw_lighting(x2, y2+50 - 10 * (count-11), x2, y2+10 - 2 * (count-11), 4, 1, 127, 0, 127, 255, 255, 255,
						vgabuf, count, mode, 5, 2, 0);	
		draw_circle(vgabuf, x2, y2+50 - 10 * (count-11), 10 - 2 * (count-11), 1, 1, 255, 255, 255, mode, 0);
	}
}
// ----------- End of function Magic::draw_magic_ten -----//

// ----------- Begin of function Magic::draw_magic_eleven -----//
void Magic::draw_magic_eleven(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;

	// max step = 16
	int intensity = (curStep >> 2); 

	draw_circle2(vgabuf, x1, y1, 40 * curStep, 1, 0, (curStep<<4)>>intensity, 255>>intensity,
		(255-(curStep<<4))>>intensity, mode, 1);
}
// ----------- End of function Magic::draw_magic_eleven -----//

// ----------- Begin of function Magic::draw_magic_twelve -----//
void Magic::draw_magic_twelve(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int nationRecno) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	
	// max step = 16
	int intensity = (curStep >> 2); 
	int col = 1;
	seed = (unsigned)(curStep);
	(void) rand_seed();	

	int color[3][3] = {{255, 55, 55}, { 55, 55,255}, { 55 ,255, 55}}; 

	if ((curStep >4) && (curStep <17))
	{
		GodInfo* godInfo = god_res[1+(int)(rand_seed() %12)];
		UnitInfo* unitInfo = unit_res[godInfo->unit_id];
		Bitmap *bitmapPtr = (Bitmap *) unitInfo->soldier_icon_ptr;
		
		int locx1 = x1 - (bitmapPtr->get_width()>>1);
		int locy1 = y1 - (bitmapPtr->get_height()>>1);
		int locx2 = locx1 + bitmapPtr->get_width() -1;
		int locy2 = locy1 + bitmapPtr->get_height() -1;
		
		if (locx1 <= bound_x2 && locx2 >= bound_x1 &&
			locy1 <= bound_y2 && locy2 >= bound_y1 )
		{
			int srcX1 = MAX(locx1, bound_x1)-locx1;
			int srcY1 = MAX(locy1, bound_y1)-locy1;
			int srcX2 = MIN(locx2, bound_x2)-locx1;
			int srcY2 = MIN(locy2, bound_y2)-locy1;
		
			short *colorRemapTable = game.get_color_remap_table(nationRecno, 1);
			vgabuf->put_bitmap_area_trans_remap_decompress(locx1, locy1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);
		}
	}

	if ((curStep >0) && (curStep <2))
	{
		draw_circle(vgabuf, x1, y1, 45, 1, 1, 255, 255, 255, mode, 0);
	}	
	if ((curStep >1) && (curStep <3))
	{
		draw_circle(vgabuf, x1, y1, 45, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1, y1, 30, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
	}
	if ((curStep >2) && (curStep <4))
	{
		draw_circle(vgabuf, x1, y1, 45, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
		draw_circle(vgabuf, x1, y1, 45, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		draw_light_beam(vgabuf, x1, y1, x1-60, y1, 5, 1, 255, 255, 255, 255, 255, 255, mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+60, y1, 5, 1, 255, 255, 255, 255, 255, 255, mode, 2);
	}	

	if ((curStep >3) && (curStep <5)) 
	{
		draw_circle(vgabuf, x1, y1, 120, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
		draw_circle(vgabuf, x1, y1, 30, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
		draw_light_beam(vgabuf, x1, y1, x1-60, y1, 5, 1, 255, 255, 255, 255, 255, 255, mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+60, y1, 5, 1, 255, 255, 255, 255, 255, 255, mode, 2);
	}	

	if ((curStep >4) && (curStep <7))
	{
		draw_circle(vgabuf, x1, y1, 60, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		draw_circle(vgabuf, x1, y1, 120, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
		draw_circle(vgabuf, x1+24, y1+24, 36, 1, 1, color[col][0], color[col][1], color[col][2], mode, 0);
		draw_light_beam(vgabuf, x1, y1, x1-90, y1, 8, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+90, y1, 8, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1-90, 8, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1+90, 8, 1, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
	}

	if ((curStep >6) && (curStep <9))
	{
		draw_circle(vgabuf, x1, y1, 60, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		draw_circle(vgabuf, x1, y1, 90, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1+24, y1+24, 36, 1, 1, 255, 255, 255, mode, 0);
		draw_light_beam(vgabuf, x1, y1, x1-120, y1, 8, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+120, y1, 8, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1-120, 8, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1+120, 8, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
	}

	if ((curStep >8) && (curStep <11))
	{
		draw_circle(vgabuf, x1, y1, 70, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		draw_circle(vgabuf, x1, y1, 60, 1, 1, 255, 255, 255, mode, 0);
		draw_circle(vgabuf, x1+16, y1+16, 24, 1, 1, 255, 255, 255, mode, 0);
		draw_light_beam(vgabuf, x1, y1, x1-80, y1, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+80, y1, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1-80, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1, y1+80, 5, 0, 255, 255, 255, color[col][0], color[col][1], color[col][2], mode, 2);
	}

	if ((curStep >10) && (curStep <17))
	{
		draw_circle(vgabuf, x1, y1, 170 - curStep*10, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
		draw_light_beam(vgabuf, x1, y1, x1-(curStep*20-180), y1, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
		draw_light_beam(vgabuf, x1, y1, x1+(curStep*20-180), y1, 3, 1, 255, 255, 255, 255, 255, 255, mode, 2);
	}
			
//	draw_circle2(vgabuf, x1, y1, 40 * curStep, 0, 0, (curStep<<4)>>intensity, 255>>intensity,
//		(255-(curStep<<4))>>intensity, mode, 1);
}
// ----------- End of function Magic::draw_magic_twelve -----//

// ----------- Begin of function Magic::draw_magic_twelve_die -----//
void Magic::draw_magic_twelve_die(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int nationRecno, int random) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	
	// max step = 16
	int col = 1;
	seed = (unsigned)(random);
	(void) rand_seed();	

	int color[3][3] = {{255, 55, 55}, { 55, 55,255}, { 55 ,255, 55}}; 

	if ((curStep >0) && (curStep <17))
	{
		GodInfo* godInfo = god_res[1+(int)(rand_seed() %12)];
		UnitInfo* unitInfo = unit_res[godInfo->unit_id];
		Bitmap *bitmapPtr = (Bitmap *) unitInfo->soldier_icon_ptr;
		
		int locx1 = x1 - (bitmapPtr->get_width()>>1);
		int locy1 = y1 - (bitmapPtr->get_height()>>1);
		int locx2 = locx1 + bitmapPtr->get_width() -1;
		int locy2 = locy1 + bitmapPtr->get_height() -1;
		
		if (locx1 <= bound_x2 && locx2 >= bound_x1 &&
			locy1 <= bound_y2 && locy2 >= bound_y1 )
		{
			int srcX1 = MAX(locx1, bound_x1)-locx1;
			int srcY1 = MAX(locy1, bound_y1)-locy1;
			int srcX2 = MIN(locx2, bound_x2)-locx1;
			int srcY2 = MIN(locy2, bound_y2)-locy1;
		
			short *colorRemapTable = game.get_color_remap_table(nationRecno, 1);
			vgabuf->put_bitmap_area_trans_remap_decompress(locx1, locy1, (char *) bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorRemapTable);
		}
		draw_circle(vgabuf, x1, y1, 70, 1, 1, color[col][0], color[col][1], color[col][2], mode, 1);
	}

	if ((curStep >0) && (curStep <6))
		draw_circle2(vgabuf, x1, y1, (5-curStep) * 35, 1, 0, 50 *curStep, 50 *curStep, 50 *curStep, mode, 1);

	if ((curStep >2) && (curStep <8))
		draw_circle2(vgabuf, x1, y1, (7-curStep) * 35, 1, 0, 50 *curStep -100,
			50 *curStep -100, 50 *curStep -100, mode, 1);
	
	int intensity = ((curStep-4) >> 2);
	if ((curStep >7) && (curStep <17))
		draw_circle2(vgabuf, x1, y1, 40 * (curStep-7), 1, 0, (255-(curStep<<4))>>intensity, 
			255>>intensity, (curStep<<4)>>intensity, mode, 1);
}
// ----------- End of function Magic::draw_magic_twelve_die -----//

// ----------- Begin of function Magic::draw_magic_thirteen -----//
void Magic::draw_magic_thirteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;

	int locX[15] = { 240, 300,  240,    0, -240, -300, -240,   0, 240, 300,  240,    0, -240, -300, -240}; 
	int locY[15] = { 140,   0, -160, -200, -160,    0,  140, 200, 140,   0, -160, -200, -160,    0,  140};		
	int step = curStep;
	int intensity = (curStep >> 2);
	
	if ((curStep >0) && (curStep <10))
		draw_circle2(vgabuf, x1, y1, 40 * step, 1, 0, (255 - 25 * step)>>intensity, 
			(255 - 25 * step)>>intensity, 255>>intensity, mode, 1);
	
	if ((curStep >3) && (curStep <13))
	{
		step = curStep - 3;
		intensity = (step >> 2);
		draw_circle2(vgabuf, x1, y1, 40 * step, 1, 0, (255 - 25 * step)>>intensity, 
			(255 - 25 * step)>>intensity, 255>>intensity, mode, 1);
	}
	
	if ((curStep >6) && (curStep <16))
	{
		step = curStep - 6;
		intensity = (step >> 2);
		draw_circle2(vgabuf, x1, y1, 40 * step, 1, 0, (255 - 25 * step)>>intensity, 
			(255 - 25 * step)>>intensity, 255>>intensity, mode, 1);
	}

	draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 2, 10,10, vgabuf, mode, 50, 2, curStep, 10, 4);
	draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 4, 10, 0, vgabuf, mode, 50, 2, curStep, 10, 4);

	draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 0,  7,10, vgabuf, mode, 50, 2, curStep, 10, 4);


	if ((curStep >= 0) && (curStep <15))
	{
		if ((curStep % 2) == 0)
		{
			draw_light_beam(vgabuf, x1, y1, x1 +locX[curStep>>1], y1 +locY[curStep>>1],
							10, 2, 0, 80, 255, 255, 255, 255, mode, 0);
			draw_circle2(vgabuf, x1 +locX[curStep>>1], y1 +locY[curStep>>1], 30, 0, 0, 0, 80, 255, mode, 0);
			draw_circle2(vgabuf, x1, y1, 30, 0, 0, 0, 80, 255, mode, 0);
		}
		else
			draw_circle2(vgabuf, x1, y1, 90, 0, 0, 0, 80, 255, mode, 0);
	}

	for (int i = 0; i < 8; i ++)
	{
		int j = i<<1;
		if ((curStep >= j) && (curStep <= j +5))
		{
			draw_particle(x1 +locX[j>>1]-5, y1 +locY[j>>1]-5, x1 +locX[j>>1]+5, y1 +locY[j>>1]+5,
				0, 0, 1, 10, 0, vgabuf, mode, 50, 2, curStep - j, 6);
		}
	}

}
// ----------- End of function Magic::draw_magic_thirteen -----//

// ----------- Begin of function Magic::draw_magic_forteen -----//
void Magic::draw_magic_forteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int curDir) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1 - 20;

	draw_circle2(vgabuf, x1, y1, 60, 0, 1, 255>>(curStep>>1), 255>>(curStep>>1), 255>>(curStep>>1), mode, 0);
	y1 += 32;
	switch( curDir )
	{
	case 0:
		draw_light_beam(vgabuf, x1-10, y1 - (curStep<<3), x1 +10, y1 +5 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 1:
		draw_light_beam(vgabuf, x1+7, y1 -8 - (curStep<<3), x1 +13, y1 -8 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 2:
		draw_light_beam(vgabuf, x1-10, y1 +5 - 16 -(curStep<<3), x1 +10, y1 - 16 -(curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 3:
		draw_light_beam(vgabuf, x1-10, y1 -16 - (curStep<<3), x1 +10, y1 -16 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 4:
		draw_light_beam(vgabuf, x1-10, y1 - 16 -(curStep<<3), x1 +10, y1 +5 - 16 -(curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 5:
		draw_light_beam(vgabuf, x1-7, y1 -8 - (curStep<<3), x1 -13, y1 -8 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);		
		break;
	case 6:
		draw_light_beam(vgabuf, x1-10, y1 +5 - (curStep<<3), x1 +10, y1 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	case 7:
		draw_light_beam(vgabuf, x1-10, y1 - (curStep<<3), x1 +10, y1 - (curStep<<3),
			10, 0, 0, 80, 255, 255, 255, 255, mode, 0);
		break;
	}
}
// ----------- End of function Magic::draw_magic_forteen -----//

// ----------- Begin of function Magic::draw_magic_fifteen -----//
void Magic::draw_magic_fifteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	draw_circle(vgabuf, x1, y1, curStep<<6, 0, 1, 0, 0, 510>>curStep, mode, 1);
}
// ----------- End of function Magic::draw_magic_fifteen -----//

// ----------- Begin of function Magic::draw_magic_firm_die -----//
void Magic::draw_magic_firm_die(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep) 
{
	x1 = x1 + bound_x1;
	y1 = y1 + bound_y1;
	
	draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 0, 10, 0, vgabuf, mode, 50, 2, curStep, 10, 4);
	draw_particle(x1+10, y1+10, x1-10, y1-10, 0, 0, 4, 10, 0, vgabuf, mode, 50, 2, curStep, 10, 4);
}
// ----------- End of function Magic::draw_magic_thirteen -----//
