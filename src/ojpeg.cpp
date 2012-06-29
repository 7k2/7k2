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

// Filename     : OJPEG.CPP
// Description  : display jpeg image


#include <all.h>
#include <stdio.h>
extern "C"
{
#include <jpeglib.h>
}

#include <ojpeg.h>
#include <ovga.h>
#include <ocoltbl.h>
#include <ovgabuf.h>



static void jpeg_error_exit (j_common_ptr cinfo)
{
	char msgBuffer[256];
	msgBuffer[0] = '\0';
	(*cinfo->err->format_message)(cinfo, msgBuffer);

	// see error message here

	strcpy( msgBuffer, "decompress image error" );
	err.run( msgBuffer );
}


static void jpeg_output_message (j_common_ptr cinfo)
{
	char msgBuffer[256];
	msgBuffer[0] = '\0';
	(*cinfo->err->format_message)(cinfo, msgBuffer);
	msgBuffer[255] = '\0';
	err.msg( msgBuffer );
}


static void jpeg_emit_message (j_common_ptr cinfo, int msgLevel)
{
	// don't display any warning message
	if( msgLevel < 0 )
	{
		int a = 0;	// for setting break point
	}
}


Jpeg::Jpeg() : cinfo( (jpeg_decompress_struct *) mem_add(sizeof(jpeg_decompress_struct))), 
	jerr((jpeg_error_mgr *) mem_add(sizeof(jpeg_error_mgr))),
	error_flag(0)
{
	memset( cinfo, 0, sizeof(*cinfo) );
	memset( jerr, 0, sizeof(*jerr) );

	// We set up the normal JPEG error routines, then override our own routine

	cinfo->err = jpeg_std_error(jerr);

	jerr->error_exit       = jpeg_error_exit;
	jerr->output_message   = jpeg_output_message;
	jerr->emit_message     = jpeg_emit_message;

	// Step 1: allocate and initialize JPEG decompression object

	// Now we can initialize the JPEG decompression object
	jpeg_create_decompress(cinfo);
}


Jpeg::~Jpeg()
{
	// Step 8: Release JPEG decompression object
	// This is an important step since it will release a good deal of memory

	if( !error_flag )
		jpeg_destroy_decompress(cinfo);
	else
		jpeg_abort_decompress(cinfo);

	mem_del(jerr);
	mem_del(cinfo);
}


int Jpeg::put_to_buf( VgaBuf *vgaBuf, int x, int y, char *filename )
{
	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

	if ((infile = fopen(filename, "rb")) == NULL)
	{
		return 0;
	}

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo->output_width * cinfo->output_components;

  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo->mem->alloc_sarray)
		((j_common_ptr) cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */

	err_when( cinfo->output_components != sizeof(RGBColor) );

  while (cinfo->output_scanline < cinfo->output_height)
  {
	  /* jpeg_read_scanlines can increase cinfo->output_scanline */

	 register short *writePtr = vgaBuf->buf_ptr( x, y+cinfo->output_scanline );

    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(cinfo, buffer, 1);

    /* Assume put_scanline_someplace wants a pointer and sample count. */

	 register JSAMPROW bitmapPtr = buffer[0];
	 for( int j = cinfo->output_width; j > 0; --j, ++writePtr, bitmapPtr += sizeof(RGBColor) )
		 *writePtr = vga.make_pixel( (RGBColor *)bitmapPtr );
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  return 1;
}
