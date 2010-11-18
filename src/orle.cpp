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

// Filename    : ORLE.CPP
// Description : run length compression


#include <orle.h>
#include <ofile.h>
#include <all.h>


// --------- begin of function Rle::Rle -------------//
//
Rle::Rle()
{
}
// --------- end of function Rle::Rle -------------//


// --------- begin of function Rle::~Rle -------------//
//
Rle::~Rle()
{
}
// --------- end of function Rle::~Rle -------------//


// --------- begin of function Rle::compress -------------//
//
// nul output, to find output size
//
long Rle::compress( unsigned char *inPtr, long inByteLen, unsigned char rleKey )
{
	register unsigned char *p = inPtr;
	register long c = inByteLen;

	unsigned char last;
	long count;

	long outCount = 0;

	// first byte

	if( c == 0 )
	{
		// write <rlekey> <0>
		return 2;	// end of file marker
	}

	last = *p;
	count = 1;
	++p;
	--c;

	for( ; c > 0; --c, ++p )
	{
		if( *p != last )
		{
			// flush last/count
			if( last == rleKey || count > 2 )
			{
				for( ;count > 0xff; count -= 0xff )
				{
					// write <rleKey> <xff> <last>
					outCount += 3;
				}

				// write <rleKey> <count> <last>
				outCount += 3;
			}
			else
			{
				for( ; count > 0; --count )
				{
					// write <last>
					++outCount;
				}
			}

			// reset last/count
			last = *p;
			count = 0;
		}

		++count;
	}

	// flush last/count
	if( last == rleKey || count > 2 )
	{
		for( ;count > 0xff; count -= 0xff )
		{
			// write <rleKey> <xff> <last>
			outCount += 3;
		}

		// write <rleKey> <count> <last>
		outCount += 3;
	}
	else
	{
		for( ; count > 0; --count )
		{
			// write <last>
			++outCount;
		}
	}

	// end of file marker
	// write <rlekey> <0>
	outCount += 2;

	return outCount;
}
// --------- end of function Rle::compress -------------//


// --------- begin of function Rle::compress -------------//
//
// compressed data to memory
//
long Rle::compress( unsigned char *inPtr, long inByteLen, unsigned char *outPtr, unsigned char rleKey )
{
	register unsigned char *p = inPtr;
	register long c = inByteLen;

	unsigned char last;
	long count;

	long outCount = 0;

	// first byte

	if( c == 0 )
	{
		outPtr[0] = rleKey;
		outPtr[1] = 0;			// write <rlekey> <0>
		return 2;	// end of file marker
	}

	last = *p;
	count = 1;
	++p;
	--c;

	for( ; c > 0; --c, ++p )
	{
		if( *p != last )
		{
			// flush last/count
			if( last == rleKey || count > 2 )
			{
				for( ;count > 0xff; count -= 0xff )
				{
					outPtr[outCount] = rleKey;
					outPtr[outCount+1] = 0xff;
					outPtr[outCount+2] = last;	// write <rleKey> <xff> <last>
					outCount += 3;
				}

				outPtr[outCount] = rleKey;
				outPtr[outCount+1] = (unsigned char) count;
				outPtr[outCount+2] = last;		// write <rleKey> <count> <last>
				outCount += 3;
			}
			else
			{
				for( ; count > 0; --count )
				{
					outPtr[outCount] = last;	// write <last>
					++outCount;
				}
			}

			// reset last/count
			last = *p;
			count = 0;
		}

		++count;
	}

	// flush last/count
	if( last == rleKey || count > 2 )
	{
		for( ;count > 0xff; count -= 0xff )
		{
			outPtr[outCount] = rleKey;
			outPtr[outCount+1] = 0xff;
			outPtr[outCount+2] = last;	// write <rleKey> <xff> <last>
			outCount += 3;
		}

		outPtr[outCount] = rleKey;
		outPtr[outCount+1] = (unsigned char) count;
		outPtr[outCount+2] = last;		// write <rleKey> <count> <last>
		outCount += 3;
	}
	else
	{
		for( ; count > 0; --count )
		{
			outPtr[outCount] = last;	// write <last>
			++outCount;
		}
	}

	// end of file marker
	outPtr[outCount] = rleKey;
	outPtr[outCount+1] = 0;			// write <rlekey> <0>
	outCount += 2;

	return outCount;
}
// --------- end of function Rle::compress -------------//


// --------- begin of function Rle::expand -------------//
//
// expand from memory
//
long Rle::expand( unsigned char *inPtr, unsigned char *outPtr, unsigned char rleKey )
{
	register unsigned char *p = inPtr;

	long outCount = 0;

	while(1)
	{
		if( *p == rleKey )
		{
			if( p[1] == 0 )
				break;			// end of file

			memset( outPtr+outCount, p[2], p[1] );		// <rleKey> <count> <data>
			outCount += p[1];
			p += 3;
		}
		else
		{
			outPtr[outCount] = *p;
			++outCount;
			++p;
		}
	}

	return outCount;
}
// --------- end of function Rle::expand -------------//


// --------- begin of function Rle::compress -------------//
//
// compressed data to file
// return -1 if write file error
//
long Rle::compress( unsigned char *inPtr, long inByteLen, File *outFile, unsigned char rleKey)
{
	err_when( outFile->allow_vary_size );	// not support
	register unsigned char *p = inPtr;
	register long c = inByteLen;

	unsigned char last;
	long count;

	long outCount = 0;

	// first byte

	unsigned char buffer[3];

	if( c == 0 )
	{
		buffer[0] = rleKey;
		buffer[1] = 0;
		if( !outFile->file_write(buffer, 2) )		// write <rlekey> <0>
			return -1;
		return 2;	// end of file marker
	}

	last = *p;
	count = 1;
	++p;
	--c;

	for( ; c > 0; --c, ++p )
	{
		if( *p != last )
		{
			// flush last/count
			if( last == rleKey || count > 2 )
			{
				for( ;count > 0xff; count -= 0xff )
				{
					buffer[0] = rleKey;
					buffer[1] = 0xff;
					buffer[2] = last;
					if( !outFile->file_write(buffer, 3) )		// write <rleKey> <xff> <last>
						return -1;
					outCount += 3;
				}

				buffer[0] = rleKey;
				buffer[1] = (unsigned char) count;
				buffer[2] = last;
				if( !outFile->file_write(buffer, 3) )			// write <rleKey> <count> <last>
					return -1;
				outCount += 3;
			}
			else
			{
				err_when( count > sizeof(buffer) );
				memset(buffer, last, count );
				if( !outFile->file_write(buffer, count) )		// write <last>
					return -1;
				outCount += count;
			}

			// reset last/count
			last = *p;
			count = 0;
		}

		++count;
	}

	// flush last/count
	if( last == rleKey || count > 2 )
	{
		for( ;count > 0xff; count -= 0xff )
		{
			buffer[0] = rleKey;
			buffer[1] = 0xff;
			buffer[2] = last;
			if( !outFile->file_write(buffer, 3) )		// write <rleKey> <xff> <last>
				return -1;
			outCount += 3;
		}

		buffer[0] = rleKey;
		buffer[1] = (unsigned char) count;
		buffer[2] = last;
		if( !outFile->file_write(buffer, 3) )			// write <rleKey> <count> <last>
			return -1;
		outCount += 3;
	}
	else
	{
		err_when( count > sizeof(buffer) );
		memset(buffer, last, count );
		if( !outFile->file_write(buffer, count) )		// write <last>
			return -1;
		outCount += count;
	}

	// end of file marker
	// write <rlekey> <0>
	buffer[0] = rleKey;
	buffer[1] = 0;
	if( !outFile->file_write(buffer, 2) )
		return -1;
	outCount += 2;

	return outCount;
}
// --------- end of function Rle::compress -------------//


// --------- begin of function Rle::expand -------------//
//
// expand from file
//
long Rle::expand( File *inFile, unsigned char *outPtr, unsigned char rleKey )
{
	err_when( inFile->allow_vary_size );	// not support
	long outCount = 0;

	unsigned char p[3];

	while(1)
	{
		if( !inFile->file_read(p, 1) )
			return -1;

		if( *p == rleKey )
		{
			if( !inFile->file_read(p+1,1) )
				return -1;

			if( p[1] == 0 )
				break;		// end of file

			if( !inFile->file_read(p+2,1) )
				return -1;

			memset( outPtr+outCount, p[2], p[1] );	// read <rleKey> <count> <data>
			outCount += p[1];
		}
		else
		{
			// write 
			outPtr[outCount] = *p;
			++outCount;
		}
	}

	return outCount;
}
// --------- end of function Rle::expand -------------//



// testing driver
#if(0)
void test_rle()
{
	// testing run length compression

	Rle rle;
	unsigned char testStr[] = 
	{
		0x00, 0x01, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03,
		0x03, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff,
		0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
	};

	unsigned char compressStr[sizeof(testStr)+20];
	unsigned char decompressStr[sizeof(testStr)+10];

	long rc1, rc2;

	rc1 = rle.compress(testStr, sizeof(testStr));
	err_when( rc1 > sizeof(compressStr) );

	// test compress memory stream
	rc2 = rle.compress(testStr, sizeof(testStr), compressStr);
	err_when( rc2 != rc1 );

	memset( decompressStr, 0, sizeof(decompressStr) );
	rc2 = rle.expand(compressStr, decompressStr);
	err_when( rc2 != sizeof(testStr) );
	err_when( memcmp(testStr, decompressStr, rc2 ) );

	// test compress to file stream

	File compressFile;
	compressFile.file_create( "TESTRLE.BIN" );

	rc2 = rle.compress(testStr, sizeof(testStr), &compressFile);
	err_when( rc2 != rc1 );
	compressFile.file_close();

	compressFile.file_open( "TESTRLE.BIN" );
	memset( decompressStr, 0, sizeof(decompressStr) );
	rc2 = rle.expand(&compressFile, decompressStr);
	err_when( rc2 != sizeof(testStr) );
	err_when( memcmp(testStr, decompressStr, rc2 ) );
	compressFile.file_close();
}
#endif

