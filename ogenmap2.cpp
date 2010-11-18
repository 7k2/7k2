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

//Filename    : OGENMAP2.CPP
//Description : World Map generation part 2 - generate plasma map

#include <stdlib.h>
#include <string.h>

#include <ALL.H>
#include <OGAME.H>
#include <OWORLD.H>

//--------------- define types --------------//

typedef unsigned char  BYTE;
typedef unsigned short U16;
typedef signed   long  S32;

//-------- define constant -------------//

enum { SHIFT_VALUE=18,     // shift based on no. of colors
		 MAX_COLOR=256       // max. no. of colors
	  };

//--------- define static vars -------------//

static int iparmx;      // iparmx = parm.x * 16
static int recur1=1;
static int recur_level = 0;

//--------- define static functions -------------//

static void plasma(int genMethod, int grainFactor, int randomSeed);
static void sub_divide(int x1,int y1,int x2,int y2);
static int  new_sub_divide(int x1,int y1,int x2,int y2, int recur);
static U16  adjust(int xa,int ya,int x,int y,int xb,int yb);
static U16  rand16();


//---------- Define marco functions --------------//

#define plot(x,y,height)  world.loc_matrix[(y)*MAX_WORLD_X_LOC+(x)].terrain_id=height
#define get_pix(x,y)      world.loc_matrix[(y)*MAX_WORLD_X_LOC+(x)].terrain_id

//------ Begin of function World::gen_plasma_map --------//

void World::gen_plasma_map()
{
	memset( loc_matrix , 0, sizeof(Location) * MAX_WORLD_X_LOC * MAX_WORLD_Y_LOC );

	plasma( m.random(2), 5, m.rand() );
}
//------ End of function World::gen_plasma_map --------//


//------ Begin of static function plasma ---------//
//
// <int> genMethod - 0 - original method
//                   1 - new method
//
// <int> grainFactor - Graininess Factor (.1 to 50)
//
// <unsigned int> randomSeed - random seed for the plasma
//
static void plasma(int genMethod, int grainFactor, int randomSeed)
{
   int i,k, n;
   U16 rnd[4];

   iparmx = grainFactor * 8;

   srand(randomSeed);

   for(n = 0; n < 4; n++)
      rnd[n] = 1+(((m.rand()/MAX_COLOR)*(MAX_COLOR-1))>>(SHIFT_VALUE-11));

   plot(                0,                 0, rnd[0]);
   plot(MAX_WORLD_X_LOC-1,                 0, rnd[1]);
   plot(MAX_WORLD_X_LOC-1, MAX_WORLD_Y_LOC-1, rnd[2]);
   plot(                0, MAX_WORLD_Y_LOC-1, rnd[3]);

   recur_level = 0;

   if ( genMethod == 0)         // use original method
   {
      sub_divide(0,0,MAX_WORLD_X_LOC-1,MAX_WORLD_Y_LOC-1);
   }
   else         // use new method
   {
      recur1 = i = k = 1;

      while(new_sub_divide(0,0,MAX_WORLD_X_LOC-1,MAX_WORLD_Y_LOC-1,i)==0)
      {
         k = k * 2;

         if (k  >(int)max(MAX_WORLD_X_LOC-1,MAX_WORLD_Y_LOC-1))
            break;

         i++;
		}
   }
}
//------ End of static function plasma ---------//


//------- Begin of static function sub_divide ---------//

static void sub_divide(int x1,int y1,int x2,int y2)
{
   int x,y;
   S32 v,i;

   if(x2-x1<2 && y2-y1<2)
      return;

   recur_level++;
   recur1 = 320L >> recur_level;

   x = (x1+x2)>>1;
   y = (y1+y2)>>1;

   if((v=get_pix(x,y1)) == 0)
      v=adjust(x1,y1,x ,y1,x2,y1);
   i=v;

   if((v=get_pix(x2,y)) == 0)
      v=adjust(x2,y1,x2,y ,x2,y2);
   i+=v;

   if((v=get_pix(x,y2)) == 0)
      v=adjust(x1,y2,x ,y2,x2,y2);
   i+=v;

   if((v=get_pix(x1,y)) == 0)
      v=adjust(x1,y1,x1,y ,x1,y2);
   i+=v;

   if(get_pix(x,y) == 0)
      plot(x,y,(U16)((i+2)>>2));

   sub_divide(x1,y1,x ,y);
   sub_divide(x ,y1,x2,y);
   sub_divide(x ,y ,x2,y2);
	sub_divide(x1,y ,x ,y2);
   recur_level--;
}
//------- End of static function sub_divide ---------//


//------- Begin of static function new_sub_divide ---------//

static int new_sub_divide(int x1,int y1,int x2,int y2, int recur)
{
   int x,y;
   int nx1;
   int nx;
   int ny1, ny;
   S32 i, v;

   struct sub
   {
      BYTE t;      // top of stack
      int  v[16];  // subdivided value
      BYTE r[16];  // recursion level
   };

   static struct sub subx, suby;

   /*
   recur1=1;
   for (i=1;i<=recur;i++)
      recur1 = recur1 * 2;
   recur1=320/recur1;
   */

   recur1 = 320L >> recur;
   suby.t = 2;
   ny     = suby.v[0] = y2;
   ny1    = suby.v[2] = y1;
   suby.r[0] = suby.r[2] = 0;
   suby.r[1] = 1;
   y      = suby.v[1] = (ny1 + ny) >> 1;

   while (suby.t >= 1)
   {
      while (suby.r[suby.t-1] < recur)
      {
			/*     1.  Create new entry at top of the stack  */
         /*     2.  Copy old top value to new top value.  */
         /*            This is largest y value.           */
         /*     3.  Smallest y is now old mid point       */
         /*     4.  Set new mid point recursion level     */
         /*     5.  New mid point value is average        */
         /*            of largest and smallest            */

         suby.t++;
         ny1  = suby.v[suby.t] = suby.v[suby.t-1];
         ny   = suby.v[suby.t-2];
         suby.r[suby.t] = suby.r[suby.t-1];
         y    = suby.v[suby.t-1]   = (ny1 + ny) >> 1;
         suby.r[suby.t-1]   = (int)max(suby.r[suby.t], suby.r[suby.t-2])+1;
      }

      subx.t = 2;
      nx  = subx.v[0] = x2;
      nx1 = subx.v[2] = x1;
      subx.r[0] = subx.r[2] = 0;
      subx.r[1] = 1;
      x = subx.v[1] = (nx1 + nx) >> 1;

      while (subx.t >= 1)
      {
         while (subx.r[subx.t-1] < recur)
         {
            subx.t++; /* move the top ofthe stack up 1 */
            nx1  = subx.v[subx.t] = subx.v[subx.t-1];
            nx   = subx.v[subx.t-2];
            subx.r[subx.t] = subx.r[subx.t-1];
            x    = subx.v[subx.t-1]   = (nx1 + nx) >> 1;
            subx.r[subx.t-1]   = (int)max(subx.r[subx.t],
                subx.r[subx.t-2])+1;
         }

         if ((i = get_pix(nx, y)) == 0)
            i = adjust(nx,ny1,nx,y ,nx,ny);

         v = i;

         if ((i = get_pix(x, ny)) == 0)
            i = adjust(nx1,ny,x ,ny,nx,ny);

			v += i;

         if(get_pix(x,y) == 0)
         {
            if ((i = get_pix(x, ny1)) == 0)
               i = adjust(nx1,ny1,x ,ny1,nx,ny1);
            v += i;
            if ((i = get_pix(nx1, y)) == 0)
               i = adjust(nx1,ny1,nx1,y ,nx1,ny);
            v += i;
            plot(x,y,(U16)((v + 2) >> 2));
         }

         if (subx.r[subx.t-1] == recur) subx.t = subx.t - 2;
      }

      if (suby.r[suby.t-1] == recur) suby.t = suby.t - 2;
   }

   return(0);
}
//------- End of static function new_sub_divide ---------//


//------- Begin of static function adjust ---------//

static U16 adjust(int xa,int ya,int x,int y,int xb,int yb)
{
   S32 pseudoRandom;

   pseudoRandom = ((S32)iparmx)*((m.rand()-16383));
//   pseudoRandom = pseudoRandom*(abs(xa-xb)+abs(ya-yb));

   pseudoRandom = pseudoRandom * recur1;
   pseudoRandom = pseudoRandom >> SHIFT_VALUE;
   pseudoRandom = (((S32)get_pix(xa,ya)+(S32)get_pix(xb,yb)+1)>>1)+pseudoRandom;

   if (pseudoRandom >= MAX_COLOR)
      pseudoRandom = MAX_COLOR-1;

   if(pseudoRandom < 1)
      pseudoRandom = 1;

   plot(x,y,(U16)pseudoRandom);
	return((U16)pseudoRandom);
}
//------- End of static function adjust ---------//


//------- Begin of static function rand16 ---------//
//
// returns a random 16 bit value that is never 0
//
static U16 rand16()
{
   U16 value;
   value = m.rand();
   value <<= 1;
   value += m.rand()&1;

   if(value < 1)
      value = 1;

   return(value);
}
//------- Enb of static function rand16 ---------//

