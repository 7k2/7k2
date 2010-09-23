//Filename   : OVGA2.CPP
//Description: Functions for display big image

#include <OVGA.H>
#include <OMODEID.H>
#include <ALL.H>
#include <OSTR.H>
#include <OSYS.H>
#include <OFILE.H>
#include <OMOUSE.H>
#include <OVGALOCK.H>
#include <OCOLTBL.H>
#include <OJPEG.H>

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
