//Filename    : ODB.DBF
//Description : Object Database handling, it read DBF files

#include <string.h>

#include <ALL.H>
#include <ODB.H>

#define RECORD_SPEC_TERM_CHAR '\r'
#define PRE_RECORD_CHAR ' '
#define END_OF_FILE_CHAR '\x1a'

//-------- Begin of function Database constructor ------//
//
// [char*] fileName  = the name of the DBF file to be opened
// [int]   bufferAll = read the whole database into memory or not
//
Database::Database(char* dbFileName, int bufferAll, int importRecSpec)
{
   dbf_buf  = NULL;
   rec_buf  = NULL;
	dbf_buf_allocated = 0;
	dbf_record_spec = NULL;
	dbf_field_count = 0;

   last_read_recno = -1;

   if( dbFileName )
      open( dbFileName, bufferAll, importRecSpec );
}
//----------- End of function Database constructor ------//


//-------- Begin of function Database destructor ------//

Database::~Database()
{
   Database::close();
}

//----------- End of function Database destructor ------//


//-------- Begin of function Database::open --------//
//
// Database::open( char* fileName )
//
// <char*> fileName  = the name of the DBF file to be opened
// [int]   bufferAll = read the whole database into memory or not
//      	       (default : 0)
//
// return 1 : opened successfully
//        0 : opening failure
//
void Database::open( char* fileName, int bufferAll, int importRecSpec )
{
   close();        // if there is a opened file attached to current database, close it first

   file_open(fileName);
   file_read( &dbf_header, sizeof(DbfHeader) );

	if( importRecSpec )
	{
		int recordSpecSize = dbf_header.data_offset - sizeof(dbf_header);
		dbf_record_spec = (DbfRec *)mem_add( recordSpecSize );
		file_read( dbf_record_spec, recordSpecSize );
		for( dbf_field_count = 0; 
			recordSpecSize > 0 && dbf_record_spec[dbf_field_count].field_name[0] != RECORD_SPEC_TERM_CHAR;
			++dbf_field_count, recordSpecSize -= sizeof(DbfRec) );
	}
	else
	{
		dbf_record_spec = NULL;
		dbf_field_count = 0;
	}

   //..........................................//

   if( bufferAll )      // read the whole database into memory or not
   {
      dbf_buf = mem_add( dbf_header.rec_size * dbf_header.last_rec );

      file_seek( 1 + dbf_header.data_offset );
      file_read( dbf_buf, dbf_header.rec_size*dbf_header.last_rec );
      file_close();

      dbf_buf_allocated = 1;	// we allocated the buffer
   }
   else
      rec_buf = mem_add( dbf_header.rec_size );

   cur_recno = 1;
}
//--------- End of function Database::open ---------//


//-------- Begin of function Database::open_from_buf --------//
//
// Open the database from an buffer with the database pre-read in
//
// <char*> dataPtr = the pointer to the pre-read database in the memory
//
void Database::open_from_buf(char* dataPtr, int importRecSpec)
{
   close();        // if there is a open_from_bufed file attached to current database, close it first

   //------- set data pointers ----------//

   memcpy( &dbf_header, dataPtr, sizeof(DbfHeader) );

	if( importRecSpec )
	{
		int recordSpecSize = dbf_header.data_offset - sizeof(dbf_header);
		dbf_record_spec = (DbfRec *)mem_add( recordSpecSize );
		memcpy(dbf_record_spec, dataPtr+sizeof(DbfHeader), recordSpecSize );
		for( dbf_field_count = 0; 
			recordSpecSize > 0 && dbf_record_spec[dbf_field_count].field_name[0] != RECORD_SPEC_TERM_CHAR;
			++dbf_field_count, recordSpecSize -= sizeof(DbfRec) );
	}
	else
	{
		dbf_record_spec = NULL;
		dbf_field_count = 0;
	}

   dbf_buf = dataPtr + 1 + dbf_header.data_offset;

   dbf_buf_allocated = 0;	// we didn't allocate the buffer, so don't bother to free it in deinit()

   cur_recno = 1;
}
//--------- End of function Database::open_from_buf ---------//


//--------- Begin of function Database::read --------//
//
// Database::read( long recNum )
//
// [recNum] = the record number of the record to be read
//	      (default : current record no.)
//
// return : <char*> success, the pointer to the reading buffer
//          <NULL>  fail
//
char* Database::read( long recNo )
{
   if( recNo <= 0 )
      recNo = cur_recno;

   if( recNo < 1 || recNo > dbf_header.last_rec )
      return NULL;

   if( dbf_buf )        // the whole database has been read into memory
   {
      return dbf_buf + dbf_header.rec_size * (recNo-1);
   }
   else            // only a portion of the database is read into the memory at a time
   {
      if( recNo == last_read_recno )    // the record to be read is the same as one in buffer, simply return it
	 return rec_buf;

      file_seek( 1+dbf_header.data_offset + dbf_header.rec_size * (recNo-1) );
      file_read( rec_buf, dbf_header.rec_size );

      last_read_recno = recNo;

      return rec_buf;
   }
}
//----------- End of function Database::read ---------//



//---------- Begin of function Database::close -------//
//
// Database::close()
//
void Database::close()
{
   if( rec_buf )
   {
      mem_del(rec_buf);
      rec_buf = NULL;
   }

	if( dbf_record_spec )
	{
		mem_del(dbf_record_spec);
	}
	dbf_record_spec = NULL;
	dbf_field_count = 0;

   if( dbf_buf && dbf_buf_allocated )
   {
      mem_del( dbf_buf );
      dbf_buf = NULL;
		dbf_buf_allocated = 0;
   }
}
//----------- End of function Database::close ----------//



//---------- Begin of function Database::get_field_offset -------//
//
// Database::get_field_offset
// return -1 if the field not found
//
int Database::get_field_offset(char *fieldName)
{
	if( !dbf_record_spec || !dbf_field_count )
		return -1;

	for( int i = 0; i < dbf_field_count; ++i )
	{
		if( stricmp(fieldName, dbf_record_spec[i].field_name) == 0)
		{
			// the first char is a space, and Database::read don't include that space
			return dbf_record_spec[i].field_offset - 1;
		}
	}
	return -1;
}
//---------- End of function Database::get_field_offset -------//


//---------- Begin of function Database::get_field_width -------//
//
// Database::get_field_width
// return -1 if the field not found
//
int Database::get_field_width(char *fieldName)
{
	if( !dbf_record_spec || !dbf_field_count )
		return -1;

	for( int i = 0; i < dbf_field_count; ++i )
	{
		if( stricmp(fieldName, dbf_record_spec[i].field_name) == 0)
		{
			if( dbf_record_spec[i].field_type == 'N' )
				return dbf_record_spec[i].len_info.num_size.len;				// numeric field
			else
				return dbf_record_spec[i].len_info.char_len;
		}
	}
	return -1;
}
//---------- End of function Database::get_field_width -------//



//---------- Begin of function Database::get_field_dec_width -------//
//
// Database::get_field_width
// return -1 if the field not found or non-numeric field
//
int Database::get_field_dec_width(char *fieldName)
{
	if( !dbf_record_spec || !dbf_field_count )
		return -1;

	for( int i = 0; i < dbf_field_count; ++i )
	{
		if( stricmp(fieldName, dbf_record_spec[i].field_name) == 0)
		{
			if( dbf_record_spec[i].field_type == 'N' )
				return dbf_record_spec[i].len_info.num_size.dec;				// numeric field
			else
				return -1;
		}
	}
	return -1;
}
//---------- End of function Database::get_field_dec_width -------//


//---------- Begin of function Database::write_to_file -------//
int Database::write_file(char *, char *fileName)
{
	if( !dbf_record_spec || !dbf_field_count || !dbf_buf)
	{
		err.run("Cannot write DataBase");
		return 0;
	}

	File outFile;
	outFile.file_create(fileName);

	// write header
	char termChar = RECORD_SPEC_TERM_CHAR;
	dbf_header.data_offset = sizeof(DbfHeader) + sizeof(DbfRec) * dbf_field_count + sizeof(termChar);
	dbf_header.last_rec = rec_count();

	if( !outFile.file_write( &dbf_header, sizeof(DbfHeader)) )
		return 0;

	// write record spec
	if( !outFile.file_write(dbf_record_spec, dbf_field_count * sizeof(DbfRec)) )
		return 0;

	if( !outFile.file_write(&termChar, sizeof(char)) )
		return 0;

	// write record detail
	char preRecordChar = PRE_RECORD_CHAR;
	for( int i = 1; i <= rec_count(); ++i )
	{
		if( !outFile.file_write(&preRecordChar, sizeof(char)) )
			return 0;
		if( !outFile.file_write( read(i), dbf_header.rec_size-sizeof(termChar)) )
			return 0;		// one char less for the 'space'
	}

	// write end-of-file
	char endFileChar = END_OF_FILE_CHAR;
	if( !outFile.file_write( &endFileChar, sizeof(char)) )
		return 0;

	return 1;
}
//---------- End of function Database::write_to_file -------//
