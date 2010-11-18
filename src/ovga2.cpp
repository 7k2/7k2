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

//Filename   : OVGA2.CPP
//Description: Functions for display big image

#include <ovga.h>
#include <omodeid.h>
#include <all.h>
#include <ostr.h>
#include <osys.h>
#include <ofile.h>
#include <omouse.h>
#include <ovgalock.h>
#include <ocoltbl.h>
#include <ojpeg.h>

// -------- define static function --------//

static RGBColor identity_color_function(RGBColor col, int, int)
{
	return col;
}

//---------- Begin of function Vga::disp_image_file --------//
//
// <char*> fileName - file name of the image
// [int]   x1, y1   - the top left display position of the image
//							 (default: 0)
//
void Vga::disp_image_file(char* fileName, int x1, int y1)
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
	//	if( !back_up_pal )		// only save the palette when there isn't one saved already
	//		back_up_pal = new VgaCustomPalette(NULL);

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

	if( !use_back_buf )
	{
		vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
		sys.blt_virtual_buf();

		//------- bilt the back buffer to the front ---------//

		vga.blt_buf( 0,0, vga_back.buf_width()-1, vga_back.buf_height()-1, 0 );
	}
}
//----------- End of function Vga::disp_image_file ---------//


//---------- Begin of function Vga::finish_disp_image_file --------//
//
void Vga::finish_disp_image_file()
{
	//------- exiting: turn dark --------//

	vga_front.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
	sys.blt_virtual_buf();

	// mouse.show();
}
//----------- End of function Vga::finish_disp_image_file ---------//
