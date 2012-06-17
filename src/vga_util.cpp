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

//Filename    : vgautil.cpp
//Description : VGA drawing and display big image functions

#include <vga_util.h>
#include <all.h>
#include <ocoltbl.h>
#include <ofile.h>
#include <ojpeg.h>
#include <omodeid.h>
#include <omouse.h>
#include <omousecr.h>
#include <ostr.h>
#include <osys.h>
#include <ovga.h>
#include <ovgabuf.h>

// -------- define static function --------//

static RGBColor identity_color_function(RGBColor col, int, int)
{
   return col;
}

//----------- Begin of function VgaUtil::VgaUtil -----------//

VgaUtil::VgaUtil()
{
}
//----------- End of function VgaUtil::VgaUtil -----------//


//----------- Begin of function VgaUtil::~VgaUtil -----------//

VgaUtil::~VgaUtil()
{
}
//----------- End of function VgaUtil::~VgaUtil -----------//


//--------- Begin of function VgaUtil::blt_buf ----------//
//
// Blt the back buffer to the front buffer.
//
// <int> x1, y1, x2, y2 - the coordinations of the area to be blit
// [int] putBackCursor  - whether put a mouse cursor onto the back buffer
//                        before blitting.
//                        (default: 1)
//
void VgaUtil::blt_buf(int x1, int y1, int x2, int y2, int putBackCursor)
{
#ifdef DEBUG
	unsigned long startTime = m.get_time();
#endif

   if( putBackCursor )
   {
      mouse_cursor.hide_area_flag = 0;    // do not call mouse.hide_area() which will double paint the cursor

      mouse_cursor.hide_x1 = x1;
      mouse_cursor.hide_y1 = y1;
      mouse_cursor.hide_x2 = x2;
      mouse_cursor.hide_y2 = y2;

      //-------- put mouse cursor ---------//

      mouse_cursor.disp_back_buf(x1, y1, x2, y2);
   }
   else
   {
      mouse.hide_area(x1, y1, x2, y2);
   }

   //--------------------------------------//

	// ###### end Gilbert 12/9 #######//
//	vga_front.blt_buf_fast( &vga_back, x1, y1, x2, y2 );
	// use directx blt function
	vga_front.temp_unlock();
	vga_back.temp_unlock();
	vga_front.blt_buf_area( &vga_back, x1, y1, x2+1, y2+1 );
	vga_back.temp_restore_lock();
	vga_front.temp_restore_lock();
	// ###### end Gilbert 12/10 #######//

   //--------------------------------------//

   if( putBackCursor )
      mouse_cursor.hide_area_flag = 0;    // do not call mouse.show_area() which will double paint the cursor
   else
      mouse.show_area();

#ifdef DEBUG
	startTime = m.get_time() - startTime;
#endif
}
//---------- End of function VgaUtil::blt_buf ----------//


//----------- Begin of function VgaUtil::d3_panel_up ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void VgaUtil::d3_panel_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf* vgaBuf;

   if( vgaFrontOnly )
      vgaBuf = &vga_front;
   else
      vgaBuf = &vga_back;

   if( !drawBorderOnly )
   {
		// ##### begin Gilbert 19/10 #####//
      if( Vga::opaque_flag )
         // vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, UP_OPAQUE_COLOR);
         vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, VgaBuf::color_light);
      else
         // vgaBuf->bar_alpha(x1+1, y1+1, x2-1, y2-1, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);
			vgaBuf->bar_alpha( x1+1, y1+1, x2-1, y2-1, IF_UP_BRIGHTNESS_ADJUST/2-1, VgaBuf::color_light );
		// ##### end Gilbert 19/10 #####//
   }

	// ##### begin Gilbert 19/10 ######//
   // mouse.hide_area( x1,y1,x2,y2 );

   //--------- white border on top and left sides -----------//

	// vgaBuf->bar_fast( x1+1,y1,x2,y1, IF_LIGHT_BORDER_COLOR );    // top side
	// vgaBuf->bar_fast( x1,y1,x1,y2  , IF_LIGHT_BORDER_COLOR );    // left side

   //--------- black border on bottom and right sides -----------//

	// vgaBuf->bar_fast( x1+1,y2,x2,y2, IF_DARK_BORDER_COLOR );     // bottom side
	// vgaBuf->bar_fast( x2,y1+1,x2,y2, IF_DARK_BORDER_COLOR );     // right side

	vgaBuf->draw_d3_up_border( x1, y1, x2, y2 );

   //-------------------------------------------//

   // mouse.show_area();
	// ##### end Gilbert 19/10 ######//

   //----- blt the area from the back buffer to the front buffer ------//

   if( !vgaFrontOnly && !vga.use_back_buf )      // only blt the back to the front is the active buffer is the front
      blt_buf(x1, y1, x2, y2, 0);
}
//------------- End of function VgaUtil::d3_panel_up ------------//


//----------- Begin of function VgaUtil::d3_panel_down ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void VgaUtil::d3_panel_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf* vgaBuf;

   if( vgaFrontOnly )
      vgaBuf = &vga_front;
   else
      vgaBuf = &vga_back;

   if( !drawBorderOnly )
   {
		// ##### begin Gilbert 19/10 ######//
      if( Vga::opaque_flag )
         // vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, DOWN_OPAQUE_COLOR);
         vgaBuf->bar(x1+1, y1+1, x2-1, y2-1, VgaBuf::color_dark );
      else
         // vgaBuf->bar_alpha(x1+1, y1+1, x2-1, y2-1, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);
         vgaBuf->bar_alpha(x1+1, y1+1, x2-1, y2-1, IF_DOWN_BRIGHTNESS_ADJUST/2-1, VgaBuf::color_dark );
		// ##### end Gilbert 19/10 ######//
   }

	// ##### begin Gilbert 19/10 ######//
	// mouse.hide_area( x1,y1,x2,y2 );

   //--------- white border on top and left sides -----------//

	// vgaBuf->bar_fast( x1+1,y1,x2,y1, IF_DARK_BORDER_COLOR );    // top side
	// vgaBuf->bar_fast( x1,y1,x1,y2  , IF_DARK_BORDER_COLOR );    // left side

   //--------- black border on bottom and right sides -----------//

	// vgaBuf->bar_fast( x1+1,y2,x2,y2, IF_LIGHT_BORDER_COLOR );   // bottom side
	// vgaBuf->bar_fast( x2,y1+1,x2,y2, IF_LIGHT_BORDER_COLOR );   // right side

	vgaBuf->draw_d3_down_border( x1, y1, x2, y2 );

   //-------------------------------------------//

//   mouse.show_area();
	// ##### end Gilbert 19/10 ######//

   //----- blt the area from the back buffer to the front buffer ------//

   if( !vgaFrontOnly && !vga.use_back_buf )      // only blt the back to the front is the active buffer is the front
      blt_buf(x1, y1, x2, y2, 0);
}
//------------- End of function VgaUtil::d3_panel_down ------------//


//----------- Begin of function VgaUtil::d3_panel2_up ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void VgaUtil::d3_panel2_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf* vgaBuf;

   if( vgaFrontOnly )
      vgaBuf = &vga_front;
   else
      vgaBuf = &vga_back;

   if( !drawBorderOnly )
      // vgaBuf->adjust_brightness(x1+2, y1+2, x2-3, y2-3, IF_UP_BRIGHTNESS_ADJUST);
      vgaBuf->bar_alpha(x1+2, y1+2, x2-3, y2-3, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);

   mouse.hide_area( x1,y1,x2,y2 );

   //--------- white border on top and left sides -----------//

   vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x9a );
   vgaBuf->draw_pixel(x2-2, y1, 0x9a);
   vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x9a );    // left side
   vgaBuf->draw_pixel(x1, y2-2, 0x9a);

   //--------- black border on bottom and right sides -----------//

   vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x90 );     // bottom side
   vgaBuf->draw_pixel(x2-1, y1+1, 0x90);
   vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x90 );      // right side
   vgaBuf->draw_pixel(x1+1, y2-1, 0x90);

   //--------- junction between white and black border --------//
   vgaBuf->draw_pixel(x2-1, y1, 0x97);
   vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
   vgaBuf->draw_pixel(x1, y2-1, 0x97);
   vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

   //--------- gray shadow on bottom and right sides -----------//
   vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x97);
   vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x97);

   //-------------------------------------------//

   mouse.show_area();

   //----- blt the area from the back buffer to the front buffer ------//

   if( !vgaFrontOnly && !vga.use_back_buf )      // only blt the back to the front is the active buffer is the front
      blt_buf(x1, y1, x2, y2, 0);
}
//------------- End of function VgaUtil::d3_panel_up ------------//


//----------- Begin of function VgaUtil::d3_panel2_down ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void VgaUtil::d3_panel2_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf* vgaBuf;

   if( vgaFrontOnly )
      vgaBuf = &vga_front;
   else
      vgaBuf = &vga_back;

   if( !drawBorderOnly )
      // vgaBuf->adjust_brightness(x1+2, y1+2, x2-3, y2-3, IF_DOWN_BRIGHTNESS_ADJUST);
      vgaBuf->bar_alpha(x1+2, y1+2, x2-3, y2-3, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);

   mouse.hide_area( x1,y1,x2,y2 );

   //--------- black border on top and left sides -----------//

   vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x90 );
   vgaBuf->draw_pixel(x2-2, y1, 0x90);
   vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x90 );    // left side
   vgaBuf->draw_pixel(x1, y2-2, 0x90);

   //--------- while border on bottom and right sides -----------//

   vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x9a );     // bottom side
   vgaBuf->draw_pixel(x2-1, y1+1, 0x9a);
   vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x9a );      // right side
   vgaBuf->draw_pixel(x1+1, y2-1, 0x9a);

   //--------- junction between white and black border --------//
   vgaBuf->draw_pixel(x2-1, y1, 0x97);
   vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
   vgaBuf->draw_pixel(x1, y2-1, 0x97);
   vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

   //--------- remove shadow, copy from back  -----------//
   vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x9c);
   vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x9c);

   mouse.show_area();

   //----- blt the area from the back buffer to the front buffer ------//

   if( !vgaFrontOnly && !vga.use_back_buf )      // only blt the back to the front is the active buffer is the front
      blt_buf(x1, y1, x2, y2, 0);
}
//------------- End of function VgaUtil::d3_panel2_down ------------//


//------------- Start of function VgaUtil::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void VgaUtil::separator(int x1, int y1, int x2, int y2)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   if( y1+1==y2 )       // horizontal line
   {
      // vga_front.adjust_brightness(x1, y1, x2, y1, IF_UP_BRIGHTNESS_ADJUST);
      vga_front.bar_alpha(x1, y1, x2, y1, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);
      // vga_front.adjust_brightness(x1, y2, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST);
      vga_front.bar_alpha(x1, y2, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);
   }
   else
   {
      // vga_front.adjust_brightness(x1, y1, x1, y2, IF_UP_BRIGHTNESS_ADJUST);
      vga_front.bar_alpha(x1, y1, x1, y2, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);
      // vga_front.adjust_brightness(x2, y1, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST);
      vga_front.bar_alpha(x2, y1, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);
   }
}
//--------------- End of function VgaUtil::separator --------------//


//---------- Begin of function VgaUtil::disp_image_file --------//
//
// <char*> fileName - file name of the image
// [int]   x1, y1   - the top left display position of the image
//							 (default: 0)
//
void VgaUtil::disp_image_file(const char* fileName, int x1, int y1)
{
   String str;

   // use jpeg first

   str  = DIR_IMAGE;
   str += fileName;
   str += ".JPG";

   if( m.is_file_exist(str) )
   {
      Jpeg jpeg;
      jpeg.put_to_buf( &vga_back, 0, 0, str );
   }
   else
   {
      //-------- load palette first ---------- //

      File palFile;
      ColorTable colorTable;
      short *colorRemapTable = NULL;

      str  = DIR_IMAGE;
      str += fileName;
      str += ".COL";

      if( palFile.file_open(str,0) )
      {
         BYTE palBuf[256][3];
         palFile.file_seek(8);     				// bypass the header info
         palFile.file_read(palBuf, sizeof(palBuf));
         palFile.file_close();

         // ------- palette description -------------//

         PalDesc palBufDesc( palBuf, 3, 256, 8 );

         //-------- create color remap table ---------//

         colorTable.generate_table_fast( 1, palBufDesc, identity_color_function );
         colorRemapTable = (short *)colorTable.get_table(0);
      }

      //-------- backup and switch palette ----------//
      // if( !back_up_pal )		// only save the palette when there isn't one saved already
      //    back_up_pal = new VgaCustomPalette(NULL);

      //---- load the interface into the back buffer ----//

      File pictFile;

      str  = DIR_IMAGE;
      str += fileName;
      str += ".ICN";

      if( pictFile.file_open(str,0) )
      {
         vga_back.put_large_bitmap(x1, y1, &pictFile, colorRemapTable );
         pictFile.file_close();
      }
   }

   //-------- hide and change mouse cursor --------//
   // mouse.hide();

   //------ turn screen dark and blt the buffer ---------//

   if( !vga.use_back_buf )
   {
      vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
      sys.blt_virtual_buf();

      //------- bilt the back buffer to the front ---------//

      blt_buf( 0,0, vga_back.buf_width()-1, vga_back.buf_height()-1, 0 );
   }
}
//----------- End of function VgaUtil::disp_image_file ---------//


//---------- Begin of function VgaUtil::finish_disp_image_file --------//
//
void VgaUtil::finish_disp_image_file()
{
   //------- exiting: turn dark --------//

   vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
   sys.blt_virtual_buf();

   // mouse.show();
}
//----------- End of function VgaUtil::finish_disp_image_file ---------//
