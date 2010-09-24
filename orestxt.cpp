//Filename    : ORESTXT.CPP
//Description : Text Resource library handling object

#include <ALL.H>
#include <OSTR.H>
#include <ORESTXT.H>


//------------ Format of RTI file -------------//
//
// The structure of Resource Text Index file :
//
// The file is consist of a number of index block :
//
// enum { INDEX_NAME_LEN=40, PICT_NAME_LEN=8 };
//
// struct ResIndex
// {
//   char  title[INDEX_NAME_LEN+1];
//   char  gif_name[PICT_NAME_LEN+1];
//   short text_size;   // size of the text body
//   long  file_pos;    // pointer to text body in the RTX file
// }
//
//---------------------------------------------//


//----------- Format of RTX file ---------------//
//
// The structure of Resource Text file :
//
// <char..> = text data
//
//---------------------------------------------//


//---------- Begin of function ResTxt::init ----------//
//
// <char*> resName        = name of the resource file (without extension)
// [int]   alwaysOpenFlag = always open the text body file or not
//			    (default : 1)
//
void ResTxt::init(char* resName, int alwaysOpenFlag)
{
   if( init_flag )
      deinit();

   //------ Load Resource Text Index -------//

   String str  = resName;
   str        += ".RTI";

   File file;
   file.file_open(str);

   rec_count = file.file_get_short();

   txt_index_array = (TxtIndex*) mem_add( sizeof(TxtIndex) * rec_count );

   file.file_read( txt_index_array, sizeof(TxtIndex) * rec_count );

   file.file_close();

   //------ Open the Resource Text file for later reading ------//

   strncpy( res_file_name, resName, MAX_PATH );
   strncat( res_file_name, ".RTX" , MAX_PATH );

   res_file_name[MAX_PATH] = NULL;

   always_open_flag = alwaysOpenFlag;

   if( always_open_flag )
      file_txt.file_open(res_file_name);

   //------- Allocate a buffer for the text ---------//

   txt_buf_size = DEF_TXT_BUF_SIZE;      // this size should be large enough for average text unit, also we don't need realloc very frequently

   txt_buf = mem_add( txt_buf_size );

   init_flag = 1;
}
//----------- End of function ResTxt::init -------------//


//---------- Begin of function ResTxt::deinit ----------//
//
void ResTxt::deinit()
{
   if( !init_flag )
      return;

   mem_del( txt_index_array );
   txt_index_array = NULL;

   mem_del( txt_buf );
   txt_buf = NULL;

   if( always_open_flag )
      file_txt.file_close();

   init_flag = 0;
}
//----------- End of function ResTxt::deinit -------------//


//---------- Begin of function ResTxt::get_title ----------//
//
// Get text title
//
// <int> recNo = the recno of the text unit in the resource file
//               (start from 1)
//
// return : <char*> the text title
//
char* ResTxt::get_title(int recNo)
{
   if( !init_flag )
      return NULL;

   err_when( recNo<1 || recNo>rec_count );

   return txt_index_array[recNo-1].title;
}
//----------- End of function ResTxt::get_title -------------//


//---------- Begin of function ResTxt::get_pict_name ----------//
//
// Get name of the gif file attached to the topic
//
// <int> recNo = the recno of the text unit in the resource file
//               (start from 1)
//
// return : <char*> the gif name
//
char* ResTxt::get_pict_name(int recNo)
{
   if( !init_flag )
      return NULL;

   err_when( recNo<1 || recNo>rec_count );

   return txt_index_array[recNo-1].pict_name;
}
//----------- End of function ResTxt::get_pict_name -------------//


//---------- Begin of function ResTxt::get_body ----------//
//
// Get text body
//
// <int> recNo = the recno of the text unit in the resource file
//               (start from 1)
//
// return : <char*> the text body
//
char* ResTxt::get_body(int recNo)
{
   if( !init_flag )
      return NULL;

   err_when( recNo<1 || recNo>rec_count );


   //----- check if the buffer is large enough -----//

   int textSize = txt_index_array[recNo-1].text_size;

   if( textSize+1 > txt_buf_size )  // +1 is the NULL at the end of the text
   {
      txt_buf_size = textSize+1;
      txt_buf      = mem_resize( txt_buf, txt_buf_size );
   }


   //-------- read in the text into buffer ----------//

   if( !always_open_flag )  	// if not always open, then only open during reading, and close it immediately after that
      file_txt.file_open( res_file_name );

   file_txt.file_seek( txt_index_array[recNo-1].file_pos );

   file_txt.file_read( txt_buf, textSize );

   if( !always_open_flag )
      file_txt.file_close();

   txt_buf[textSize]=NULL;

   return txt_buf;
}
//----------- End of function ResTxt::get_body -------------//


//---------- Begin of function ResTxt::locate_topic ----------//
//
// Given the topic title, find the topic and return the topic id.
//
// <char*> titleStr = the title of the topic
//
// return : <int> >= 1 the topic id.
//                == 0 topic not found
//
int ResTxt::locate_topic(char* titleStr)
{
   if( !init_flag )
      return NULL;

   int       i;
   TxtIndex* txtIndex = txt_index_array;

   for( i=0 ; i<rec_count ; i++, txtIndex++ )
   {
      if( strcmp( txtIndex->title, titleStr )==0 )
         return i+1;
   }

   return 0;
}
//----------- End of function ResTxt::locate_topic -------------//

