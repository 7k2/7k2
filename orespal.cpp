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

// Filename    : ORESPAL.H
// Description : Palette ResourcePal



#include <ORESPAL.H>
#include <ALL.H>
#include <OSYS.H>
#include <OVGA.H>
#include <ODB.H>
#include <OCOLTBL.H>
#include <string.h>



//---------- Begin of function ResourcePal::init ---------//
//
// <char*>     resName			= name of the resource file (e.g. "GIF.RES")
// <Database*> dbObj				= name of the database      (e.g. Database db("PFILE.DBF"))
// <int>       indexOffset		= offset of the index field
// [int]       useCommonBuf   = whether use the vga common buffer to store the data or not
//										  (default:0)
//
void ResourcePal::init(char* resName, Database* dbObj, int indexOffset, int useCommonBuf)
{
	ResourceDb::init( resName, dbObj, indexOffset, useCommonBuf );

	if( init_flag )
		generate_pal();
}
//----------- End of function ResourcePal::init ------------//


//---------- Begin of function ResourcePal::deinit ---------//
//
void ResourcePal::deinit()
{
   if( init_flag )
   {
		if( pal_data_buf )
		{
			mem_del(pal_data_buf);
			pal_data_buf = NULL;
		}
		ResourceDb::deinit();
   }
}
//----------- End of function ResourcePal::deinit ----------//


//---------- Begin of function ResourcePal::init_imported ----------//
//
// If the whole database has been read into memory, then only no need to
// tell ResourcePal the database name and the index offset
//
// <char*> resName   = name of the resource file (e.g. "GIF.RES")
// <int>   readAll   = whether read all data into the buffer or read one each time
// [int]   useCommonBuf = whether use the vga common buffer to store the data or not
//                     (default:0)
//
void ResourcePal::init_imported(char* resName, int readAll, int useCommonBuf)
{
	ResourceDb::init_imported(resName, readAll, useCommonBuf);

	if( init_flag )
		generate_pal();
}
//----------- End of function ResourcePal::init_imported -------------//


//---------- Begin of function ResourcePal::generate_pal ---------//
void ResourcePal::generate_pal()
{
	if( read_all )
	{
		pal_data_buf = (short *)mem_add( data_buf_size );

		// process all palettes

		long offset = 0;
		while( offset < data_buf_size )
		{
			// process each palette
			long fileSize = *(long *)(data_buf + offset);
			err_when( fileSize < 0 );
			RGBColor *srcPtr = (RGBColor *)(offset + sizeof(long) + 8 + (char *)data_buf);		// skip length field and 8 byte header
			short *destPtr = (short *)(offset + sizeof(long)  + (char *)pal_data_buf);

			// process each entry
			for( int j = 0; j < 0x100; ++j )
				destPtr[j] = vga.make_pixel( &srcPtr[j] );

			// proceed to next palette
			offset += sizeof(long) + fileSize;			// skip length field
		}
	}
	else
	{
		// read_all is false

		pal_data_buf = (short *)mem_add( 0x100 * sizeof(short) );
	}
}
//---------- End of function ResourcePal::generate_pal ---------//


//---------- Begin of function ResourcePal::read_imported_pal ---------//
//
// similar to read_imported, but get the 16-bit palette
// correspond to the 
//
short* ResourcePal::read_imported_pal(long offset)
{
	err_when( !init_flag );

	if( read_all )
	{
		err_when( offset<0 || offset>=data_buf_size );
		return pal_data_buf + offset + sizeof(long);  // by pass the long parameters which is the size of the data
	}
	else
	{
		RGBColor *srcPtr = (RGBColor *)(8 + (char *)read_imported(offset));		// skip 8 byte header
		short *destPtr = pal_data_buf;

		// process palette 

		for( int j = 0; j < 0x100; ++j )
			destPtr[j] = vga.make_pixel( &srcPtr[j] );

	   return destPtr;
	}
}
//---------- End of function ResourcePal::read_imported_pal ---------//

