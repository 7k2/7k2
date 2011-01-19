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

//Filename    : OFILE.CPP
//Description : Object File

#include <ofile.h>
#include <obox.h>
#include <all.h>
#include <windows.h>
#include <stdio.h>

//--------- Define static variables -----------//

static char *path_array[] = { "" };	// multiple search path

//-------- Begin of function File::file_open ----------//
//
// Open an existing file for reading
// If the file is not found, call err.run()
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
// return : 1-success, 0-fail
//
int File::file_open(const char* fileName, int handleError, int allowVarySize)
{
	if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   if( file_handle != INVALID_HANDLE_VALUE )
      file_close();

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   //--------- search all path to open the file ------//

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, fileName );

      file_handle = CreateFile(filePath,
			   GENERIC_READ,
			   FILE_SHARE_READ,
			   (LPSECURITY_ATTRIBUTES) NULL,
			   OPEN_EXISTING,
			   FILE_ATTRIBUTE_NORMAL,
			   (HANDLE) NULL);

      if( file_handle != INVALID_HANDLE_VALUE)
			return TRUE;
   }

	err.run( "Error opening file %s.", file_name );

   return FALSE;
}
//---------- End of function File::file_open ----------//


//-------- Begin of function File::file_create ----------//
//
// Create a new file for writing (reading is also permitted)
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
//
// return : 1-success, 0-fail
//
int File::file_create(const char* fileName, int handleError, int allowVarySize)
{
   if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   // cannot use creat() because it can't specify Binary file type

   file_handle = CreateFile(fileName,
			   GENERIC_WRITE,
			   0,
			   (LPSECURITY_ATTRIBUTES) NULL,
			   CREATE_ALWAYS,
			   FILE_ATTRIBUTE_NORMAL,
			   (HANDLE) NULL);

   if( file_handle == INVALID_HANDLE_VALUE)
		err.run( "Error creating file %s", file_name );

   return TRUE;
}
//---------- End of function File::file_create ----------//

//### begin alex 24/7 ###//
//-------- Begin of function File::file_append ----------//
//
// Open an existing file for reading
// If the file is not found, call err.run()
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
// return : 1-success, 0-fail
//
int File::file_append(const char* fileName, int handleError, int allowVarySize)
{
	if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   if( file_handle != INVALID_HANDLE_VALUE )
      file_close();

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   //--------- search all path to open the file ------//

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, fileName );

      file_handle = CreateFile(filePath,
			   GENERIC_WRITE,
			   FILE_SHARE_WRITE,
			   (LPSECURITY_ATTRIBUTES) NULL,
			   OPEN_EXISTING,
			   FILE_ATTRIBUTE_NORMAL,
			   (HANDLE) NULL);

      if( file_handle != INVALID_HANDLE_VALUE)
			return TRUE;
   }

	err.run( "Error opening file %s.", file_name );

   return FALSE;
}
//---------- End of function File::file_append ----------//
//#### end alex 24/7 ####//

//-------- Begin of function File::file_close ----------//
//
void File::file_close()
{
   if( file_handle != INVALID_HANDLE_VALUE )
   {
      CloseHandle(file_handle);
      file_handle = INVALID_HANDLE_VALUE;
   }
}
//---------- End of function File::file_close ----------//


//-------- Begin of function File::~File ----------//
//
File::~File()
{
   file_close();
}
//---------- End of function File::~File ----------//


//-------- Begin of function File::file_write ----------//
//
// Write a block of data to the file
//
// <void*>    dataBuf = pointer to data buffer to be written to the file
// <unsigned> dataSize = length of the data (must < 64K)
//
// return : 1-success, 0-fail
//
int File::file_write(void* dataBuf, unsigned dataSize)
{
   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

   if( allow_vary_size )        // allow the writing size and the read size to be different
   {
      if( dataSize > 0xFFFF )
         file_put_unsigned_short(0);    // if exceed the unsigned short limit, don't write it
      else
         file_put_unsigned_short( dataSize );
   }

	DWORD actualWritten;	

   int rc = WriteFile( file_handle, dataBuf, dataSize, &actualWritten, NULL); 
		
   if( !rc && handle_error )
      err.run( "Error writing file %s", file_name );

   return rc;
}
//---------- End of function File::file_write ----------//

#if(defined(CHINESE))
#ifdef DEBUG
//SXM
static Total=0;
void DumpBin(char* FileName,char* s,int Length);
void Dump(char* FileName,char* s);
void Dump(char* FileName,int i);
void DumpBin(char* s,int Length);
void Dump(char* s);
void Dump(int i);
//SXM
#endif
#endif

//-------- Begin of function File::file_read ----------//
//
// Read a block of data from the file
//
// <void*>    dataBuf = pointer to data buffer to be written to the file
// <unsigned> dataSize = length of the data (must < 64K)
//
// return : 1-success, 0-fail
//

int File::file_read(void* dataBuf, unsigned dataSize)
{
#if(defined(CHINESE))
#ifdef DEBUG
	//SXM:Test
	
	char s[10240];
	strcpy(s,file_name);
	strlwr(s);
	if(strstr(s,".scn"))
	{
		Total+=dataSize;
		Dump("Read ");
		Dump(dataSize);
		Dump(" bytes ");
		Dump("  Total ");
		Dump(Total);
		Dump(" bytes");
		Dump("  Offset ");
		Dump(file_pos());
		Dump("\n");
	}
	//SXM:Test
#endif
#endif

	#define MAX_READ_SIZE 0xFFF0

	err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	int curPos=file_pos();

  start_read:

   unsigned readSize=dataSize, writtenSize=dataSize;

   if( allow_vary_size )        // allow the writing size and the read size to be different
   {
      writtenSize = file_get_unsigned_short();

      if( writtenSize )    // writtenSize==0, if the size > 0xFFFF
         readSize = min(dataSize, writtenSize);  // the read size is the minimum of the written size and the supposed read size
   }

	DWORD actualRead;

	// ######## begin Gilbert 29/4 ##########//
   // int rc=ReadFile( file_handle, dataBuf, dataSize, &actualRead, NULL);
   int rc=ReadFile( file_handle, dataBuf, readSize, &actualRead, NULL);
	// ######## end Gilbert 29/4 ##########//
		
   //-------- if the data size has been reduced ----------//

   if( readSize < writtenSize )
      file_seek( writtenSize-readSize, FILE_CURRENT );

   //---- if the data size has been increased, reset the unread area ---//

   if( readSize < dataSize )
      memset( (char*)dataBuf+readSize, 0, dataSize-readSize );

   //----- if reading error, popup box and ask for retry -----//

	if( !rc && handle_error )
	{
		char msgStr[100];

		sprintf( msgStr, "Error reading file %s, Retry ?", file_name );

		if( box.ask(msgStr) )
		{
			file_seek( curPos );
			goto start_read;
		}
	}

	return rc;
}
//---------- End of function File::file_read ----------//


//-------- Begin of function File::file_put_short ----------//
//
// Put a short integer to the file
//
// <short int> = the short integer
//
// return : 1-success, 0-fail
//
int File::file_put_short(short value)
{
   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualWritten;	

	if( WriteFile( file_handle, &value, sizeof(short), &actualWritten, NULL ) )
		return TRUE;
	else
	{
		if( handle_error )
			err.run( "Error writing file %s", file_name );

		return FALSE;
	}
}
//---------- End of function File::file_put_short ----------//


//-------- Begin of function File::file_get_short ----------//
//
// Get a short integer from the file
//
// return : the short integer
//
short File::file_get_short()
{
#if(defined(CHINESE))
#ifdef DEBUG
	//SXM:Test
	char s[10240];
	strcpy(s,file_name);
	strlwr(s);
	if(strstr(s,".scn"))
	{
		Total+=sizeof(short);
		Dump("Read ");
		Dump(sizeof(short));
		Dump(" bytes ");
		Dump("  Total ");
		Dump(Total);
		Dump(" bytes");
		Dump("  Offset ");
		Dump(file_pos());
		Dump("\n");
	}
	//SXM:Test
#endif
#endif

   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualRead;	
	short value;

	if( ReadFile( file_handle, &value, sizeof(short), &actualRead, NULL ) )
		return value;
	else
	{
		if( handle_error )
			err.run( "Error reading file %s", file_name );

		return FALSE;
	}
}
//---------- End of function File::file_get_short ----------//


//-------- Begin of function File::file_put_unsigned_short ----------//
//
// Put a unsigned short integer to the file
//
// <unsigned short> = the short integer
//
// return : 1-success, 0-fail
//
int File::file_put_unsigned_short(unsigned short value)
{
   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualWritten;	

	if( WriteFile( file_handle, &value, sizeof(unsigned short), &actualWritten, NULL ) )
		return TRUE;
	else
	{
		if( handle_error )
			err.run( "Error writing file %s", file_name );

		return FALSE;
	}
}
//---------- End of function File::file_put_unsigned_short ----------//


//-------- Begin of function File::file_get_unsigned_short ----------//
//
// Get a short integer from the file
//
// return : the short integer
//
unsigned short File::file_get_unsigned_short()
{
#if(defined(CHINESE))
#ifdef DEBUG
	//SXM:Test
	char s[10240];
	strcpy(s,file_name);
	strlwr(s);
	if(strstr(s,".scn"))
	{
		Total+=sizeof(unsigned short);
		Dump("Read ");
		Dump(sizeof(unsigned short));
		Dump(" bytes ");
		Dump("  Total ");
		Dump(Total);
		Dump(" bytes");
		Dump("  Offset ");
		Dump(file_pos());
		Dump("\n");
	}
	//SXM:Test
#endif
#endif

   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualRead;	
	unsigned short value;

	if( ReadFile( file_handle, &value, sizeof(unsigned short), &actualRead, NULL ) )
		return value;
	else
	{
		if( handle_error )
			err.run( "Error reading file %s", file_name );
		
		return 0;
	}
}
//---------- End of function File::file_get_unsigned_short ----------//


//-------- Begin of function File::file_put_long ----------//
//
// Put a long integer to the file
//
// <long int> = the long integer
//
// return : 1-success, 0-fail
//
int File::file_put_long(long value)
{
   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualWritten;	

	if( WriteFile( file_handle, &value, sizeof(long), &actualWritten, NULL ) )
		return TRUE;
	else
	{
		if( handle_error )
			err.run( "Error writing file %s", file_name );

		return FALSE;
	}
}
//---------- End of function File::file_put_long ----------//


//-------- Begin of function File::file_get_long ----------//
//
// Get a long integer from the file
//
// return : the long integer
//
long File::file_get_long()
{
#if(defined(CHINESE))
#ifdef DEBUG
	//SXM:Test
	char s[10240];
	strcpy(s,file_name);
	strlwr(s);
	if(strstr(s,".scn"))
	{
		Total+=sizeof(long);
		Dump("Read ");
		Dump(sizeof(long));
		Dump(" bytes ");
		Dump("  Total ");
		Dump(Total);
		Dump(" bytes");
		Dump("  Offset ");
		Dump(file_pos());
		Dump("\n");
	}
	//SXM:Test
#endif
#endif

   err_when( file_handle == INVALID_HANDLE_VALUE );       // not initialized

	DWORD actualRead;	
	long  value;

	if( ReadFile( file_handle, &value, sizeof(long), &actualRead, NULL ) )
		return value;
	else
	{
		if( handle_error )
			err.run( "Error reading file %s", file_name );

		return FALSE;
	}
}
//---------- End of function File::file_get_long ----------//


//---------- Start of function File::file_seek ---------//
//
// <long> offset = seek offset
// [int]  whence = FILE_BEGIN, FILE_CURRENT, FILE_END
//                 (default : FILE_BEGIN)
//
// return : the offset of the pointer's new position, measured in
//          bytes from the file beginning.
//
long File::file_seek(long offset, int whence)
{
	if( whence == -1 )
      whence = FILE_BEGIN;

#if(defined(CHINESE))
#ifdef DEBUG
	//SXM:Test
	
	char s[10240];
	strcpy(s,file_name);
	strlwr(s);
	if(strstr(s,".scn"))
	{
		Dump("Seek ");
		Dump(offset);
		Dump("  Offset ");
		Dump(file_pos());
		Dump("\n");
	}
	//SXM:Test
#endif
#endif
	
	return SetFilePointer( file_handle, offset, NULL, whence );
}
//------------ End of function File::file_seek ----------//


//---------- Start of function File::file_pos ---------//
//
// Get the position of current file pointer
//
// return : the position of current file pointer
//
long File::file_pos()
{
   return SetFilePointer( file_handle, 0, NULL, FILE_CURRENT );	 
}

//------------ End of function File::file_pos ----------//


//---------- Start of function File::file_size ---------//

long File::file_size()
{
	return GetFileSize(file_handle, NULL);
}
//------------ End of function File::file_size ----------//
