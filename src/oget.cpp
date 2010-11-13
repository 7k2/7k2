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

//Filename    : Get.CPP
//Description : Object Get

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include <ALL.H>
#include <KEY.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OGET.H>

//-------- Define constant ----------//

#define DEFAULT_DATE "DD/MM/YY"
#define PICTURE_CHRS "!9#ALNYXCMD"


//------- BEGIN OF FUNCTION Get::Get -----------//

Get::Get()    // constructor
{
   memset( this, 0, sizeof(Get) );

	font_ptr = &font_std;
}
//--------- END OF FUNCTION Get::Get-------------//


//-------- Begin of function Get::field -----------//
//
// High level function of creating a getting field
//
// <int>   - desX - start of the field description
// <int>   - y1
// <char*> - field description
// <int>   - fldX1 - start of the field value
// <char*> - the data to be got (only character type)
// <int>   - data width
// <int>   - fldX2 - end of the field value
//
void Get::field(int desX, int y1, char* fldDes, int fldX1, char* dataPtr,
		int dataWidth, int fldX2)
{
	//-------------------------------------//

	fld_x1 = fldX1;     // for detect()
	fld_y1 = y1;
	fld_x2 = fldX2;
	fld_y2 = y1+font_ptr->height()+5;

	//-------------------------------------//

	init( 1, 0 );        // one field only for this get, 1-no updown exit

	vga.d3_panel_up( desX, fld_y1, fld_x1, fld_y2, 1 );	 // field description

	font_ptr->put( desX+3, fld_y1+3, fldDes );

	vga.d3_panel_down( fld_x1+4, fld_y1, fld_x2, fld_y2 );  // field content

	add  ( fld_x1+8, fld_y1+4, dataPtr, 'C', fld_x2-3 );
	width( dataWidth, dataWidth );

	display();
}
//----------- End of function Get::field -------------//


//-------- Begin of function Get::field -----------//
//
// High level function of creating a getting field
// Another version of field() without field description.
//
// <int>   - desX - start of the field description
// <int>   - y1
// <char*> - the data to be got (only character type)
// <int>   - data width
// <int>   - fldX2 - end of the field value
//
void Get::field(int x1, int y1, char* dataPtr, int dataWidth, int x2)
{
	//-------------------------------------//

	fld_x1 = x1;     // for detect()
	fld_y1 = y1;
	fld_x2 = x2;
	fld_y2 = y1+font_ptr->height()+5;

	//-------------------------------------//

	init( 1, 0 );        // one field only for this get, 1-no updown exit

	vga.d3_panel_down( fld_x1, fld_y1, fld_x2, fld_y2 );  // field content

	add  ( fld_x1+4, fld_y1+4, dataPtr, 'C', fld_x2-3 );
	width( dataWidth, dataWidth );

   display();
}
//----------- End of function Get::field -------------//


//-------- Begin of function Get::detect -----------//
//
int Get::detect()
{
   if( get_num==0 )
      return 0;

   if( mouse.single_click(fld_x1, fld_y1, fld_x2, fld_y2 ) )
      return read();

   return 0;
}
//----------- End of function Get::detect -------------//


//-------- BEGIN OF FUNCTION Get::init --------------//
//
// Description : Initiate all setting, allocate memory,
//               this is called when a new GET object is defined.
//
// <int> = the no. of gets
// <int> = exit when UP or DOWN is pressed at the first or last field
//
void Get::init(int maxGetNum,int updownExitFlag)
{
   err_when( maxGetNum==0 );

   deinit();

   fld_array = (GetFld*) mem_add( sizeof(GetFld) * maxGetNum );
   memset(fld_array, NULL, sizeof(GetFld) * maxGetNum );

   max_get_num  = maxGetNum;
   up_down_exit = updownExitFlag;

   get_num=0;    // get_num is increased first, so that after running Get::add(), the get_num is still pointing to the last get
}
//------------- END OF FUNCTION Get::init --------------//


//-------- BEGIN OF FUNCTION Get::deinit -----------//
//
void Get::deinit()
{
   if (get_num>0 || max_get_num>0 )
   {
      get_num=0;
      max_get_num=0;
   }

   if( fld_array )
   {
      mem_del( fld_array );
      fld_array = NULL;
   }
}
//-------- END OF FUNCTION Get::deinit ------------//


//----------- Begin of function Get::set_font ------------//
//
// Set the font of the get, if this function is not called,
// font_san will be used as default
//
// <Font*> = the pointer to the font used in the Get
//
void Get::set_font(Font* fontPtr)
{
	font_ptr = fontPtr;
}
//------------- End of function Get::set_font ------------//


//-------- BEGIN OF FUNCTION Get::add -----------//
//
// Description : add a GET element
//
// Syntax      : add( <int>,<int>,<void*>,<char>,[char] )
//
// <int>   = x
// <int>   = y
// <void*> = the data to be got
// <char>  = type
// <int>   = x2 of the field
//
// 'C'-Character
// 'D'-Date
// 'L'-Logical
// 'N'-Numerical
// 'l'-long
// 'i'-integer
// 'c'-char integer ( -128 - 127 )
//
// ** Note : Currently it only support "Character" type
//
void Get::add(int x, int y, void* data, char type, int x2)
{
   err_when( get_num<0 || get_num >= max_get_num );

   err_if( type != 'C' )
      err_now( "Get class now only support Character type" );

   fld_ptr = fld_array + get_num;

   fld_ptr->x     = x;
   fld_ptr->y     = y;
   fld_ptr->data  = data;
   fld_ptr->type  = type;
   fld_ptr->x2    = x2;

   get_num++;
}
//----------- END OF FUNCTION Get::add -------------//


//-------- BEGIN OF FUNCTION Get::picture ----------//
//
// Description : add a picture to the last get
//
// Syntax      : picture( <char*>,<int> )
//
// <char*> = picture
// <int>   = convert all characters to upper case (default : 0)
//
void Get::picture( char* in_picture,int upper_convert )
{
   err_when( get_num<1 || get_num>max_get_num );

   fld_ptr->picture       = in_picture;
   fld_ptr->upper_convert = upper_convert;
}
//---------- END OF FUNCTION Get::picture -------------//


//------- BEGIN OF FUNCTION Get::call -------------//
//
// Description : add the call routine pointer to the last get
//
// Syntax      : call( <*()>,<int> )
//
//
void Get::call( GetCall *callPtr, int callData )
{
   err_when( get_num<1 || get_num>max_get_num );

   fld_ptr->call_ptr  = callPtr;
   fld_ptr->call_data = callData;
}
//-------- END OF FUNCTION Get::call --------------//


//-------- BEGIN OF FUNCTION Get::valid  ----------//
//
// Description : add a valid function pointer to the last get
//
// Syntax      : valid( <*()> )
//
// <*()> = the valid function pointer
//
void Get::valid( char (*validPtr)() )
{
   err_when( get_num<1 || get_num>max_get_num );

   fld_ptr->valid_ptr = validPtr;
}
//-------- END OF FUNCTION Get::valid --------------//


//------ BEGIN OF FUNCTION Get::width -------------//
//
// Description : Set the width of the last get field
//
// Syntax : width( <int>,<int> )
//
// <int> = the width of the data   ( width_data )
// <int> = the width of the screen ( width_scr  )
//
void Get::width( int widthData, int widthScr )
{
   err_when( get_num<1 || get_num>max_get_num ||
             widthData > MAX_GET_WIDTH || widthScr > widthData );

   if ( widthData > 0)
      fld_ptr->width_data = widthData;

   if ( widthScr > 0)
      fld_ptr->width_scr  = widthScr;
}
//------- END OF FUNCTION Get::width -------------//


//------ BEGIN OF FUNCTION Get::check_data -----------//
//
// Description : check_data the len, type of the data
//
// Syntax      : check_data(<char*>)
//
// <char*> = the get pointer
//
void Get::check_data( GetFld* getPtr )
{
   int   i, pictLen;
   char* ptr;

   if ( getPtr->picture == NULL )      // get the length of the picture
      pictLen = 0;
   else
      pictLen = strlen( getPtr->picture );

   // if length of picture > 0, then the length of the get is equal to it
   // if not, than the length of the get is equal to the length of the data

   switch( getPtr->type )
   {
      case 'C':
      case 'N':
      case 'D':

         //....... set data length ...........//

         if( getPtr->width_data <= 0 )
            getPtr->width_data = pictLen;    // if the length of the data is not defined picture length

         if( getPtr->width_data <= 0 )    // fill with the length of the data
         {
            if( getPtr->type == 'D' )
               getPtr->width_data = strlen(DEFAULT_DATE);
            else
               getPtr->width_data = strlen( (char*) getPtr->data );
         }

         if ( getPtr->width_data <= 0 )
            getPtr->width_data = 1;

         //....... set data screen length ...........//

         if (getPtr->width_scr <= 0 || getPtr->width_scr > getPtr->width_data )
            getPtr->width_scr = getPtr->width_data;

         //....... Decimal place ..............//

         if ( getPtr->type == 'N' )
         {
           if ( pictLen > 0 )
              ptr = getPtr->picture;
           else
              ptr = (char*) getPtr->data;

           for( i=1; i<= getPtr->width_data && *ptr ; i++,ptr++)
              if (*ptr == '.')
              {
                 getPtr->dec_num = getPtr->width_data - i;
                 break;
              }

           if ( getPtr->dec_num < 0 )
              getPtr->dec_num = 0 ;
         }
         break;

      case 'L':
         getPtr->width_data = getPtr->width_scr = 1 ;
         break;

      case 'l':
      case 'i':
      case 'c':
         if ( getPtr->width_scr <= 0)
         {
            if ( pictLen > 0 )
               getPtr->width_scr = pictLen;
            else
            {
               switch( getPtr->type )
               {
                  case 'l' :
                     getPtr->width_scr = 8;
                  case 'i' :
                     getPtr->width_scr = 4;
                  case 'c' :
                     getPtr->width_scr = 2;   // maximum 99
               }
            }
         }
         getPtr->width_data = getPtr->width_scr ;
         break;

      case 'd':
         if ( pictLen > 0 )
         {
            ptr = strchr( getPtr->picture, '.' );
            if ( ptr == NULL )
               getPtr->dec_num = 0 ;
            else
               getPtr->dec_num = pictLen - (int) getPtr->picture - 1;

            getPtr->width_scr = pictLen;
         }
         else
         {
            if ( getPtr->width_scr <= 0 )
               getPtr->width_scr = 8;

            if ( getPtr->dec_num < 0 )
               getPtr->dec_num = 2;

            if ( getPtr->dec_num >= getPtr->width_scr )
               getPtr->dec_num = 0;
         }

         getPtr->width_data = getPtr->width_scr ;
         break;
   }
}

//--------- END OF FUNCTION Get::check_data ----------//


//------- BEGIN OF FUNCTION Get::load_data ---------//
//
// Description : convert the raw data to pictured data
//
// Syntax : load_data( <GetFld*>, <char*> )
//
// <GetFld*> = the pointer to the get
//
// <char*>   = the destination buffer to store the pictured data
//
//
int Get::load_data( GetFld *getPtr, char *chr_buf )
{
   //------- Currently it only support "Character" type -----//
/*
	char* pict_char;
	int   len;

   if ( getPtr->type == 'd' )
   {
      ptr = c1dtoa( * ((double *) getPtr->data), getPtr->width_scr,
                                  getPtr->dec_num);
      memcpy( chr_buf, ptr, (size_t) getPtr->width_scr ) ;
      return( getPtr->width_scr ) ;
   }

   if ( getPtr->type == 'l' )
   {
      c1ltoa( * ((long *) getPtr->data), chr_buf, getPtr->width_scr ) ;
      return( getPtr->width_scr ) ;
   }

   if ( getPtr->type == 'i' )
   {
      c1ltoa((long) (* ((int *)getPtr->data)), chr_buf, getPtr->width_scr) ;
      return( getPtr->width_scr ) ;
   }

   if ( getPtr->type == 'c' )
   {
      c1ltoa((long) (* ((char *)getPtr->data)), chr_buf, getPtr->width_scr) ;
      return( getPtr->width_scr ) ;
   }


   if ( getPtr->type == 'D' )
   {
      if ( getPtr->picture == (char *) 0 )
      {
         ptr =  c1dt_format( (char *) getPtr->data, DEFAULT_DATE ) ;
         len =  (int) strlen(DEFAULT_DATE ) ;
      }
      else
      {
         ptr =  c1dt_format( (char *) getPtr->data, getPtr->picture ) ;
         len =  (int) strlen( getPtr->picture ) ;
      }

      memcpy( chr_buf, ptr, (size_t) len ) ;
      return( len ) ;
   }

   if ( getPtr->type == 'L' )
   {
      if ( getPtr->picture == (char *) 0 )
         pict_char =  'L' ;
      else
         pict_char =  getPtr->picture[0] ;

      if ( * ((int *) getPtr->data) )
      {
         if ( pict_char == 'L' )
            chr_buf[0] = 'T' ;
         else
            chr_buf[0] = 'Y' ;
      }
      else
      {
         if ( pict_char == 'L' )
            chr_buf[0] =  'F' ;
         else
            chr_buf[0] =  'N' ;
      }
      return 1 ;
   }

   if ( getPtr->type == 'N' && getPtr->dec_num > 0 )
   {
      ( (char *)getPtr->data )
         [ getPtr->width_data - getPtr->dec_num - 1] = '.' ;
   }
*/

   //------- Currently it only support "Character" type -----//

   err_when( (int) strlen((char*)getPtr->data) > (int) getPtr->width_data );

   //---- fill the string with tailing space in the whole data width ---//

   memset( chr_buf, ' ', getPtr->width_data );   // char after the NULL will be unknown in the original data

   if( mask_flag )
      memset( chr_buf, '*', strlen((char*)getPtr->data) );	// masked the field for displaying hidden data (like password)
   else
      memcpy( chr_buf, (char*)getPtr->data, strlen((char*)getPtr->data) );	// don't copy the NULL character

   chr_buf[getPtr->width_data] = NULL;

   //---- if has picture, process the character buf with the picture ----//

   char* ptr = getPtr->picture;

   if( ptr )
   {
      for ( int i= 0; *ptr != '\0'; i++, ptr++ )
      {
         if ( strchr( PICTURE_CHRS, *ptr) == (char *) 0 )
            chr_buf[i] =  getPtr->picture[i] ;
      }
   }

   return( getPtr->width_data ) ;
}
//--------- END OF FUNCTION Get::load_data ----------//


//------- BEGIN OF FUNCTION Get::display_get ---------//
//
// Description : display one get a time
//
// Syntax      : display_get( <GetFld*>,<int> )
//
// <GetFld*> = the pointer to get
// <int>     = 1-Getting field, 0-Displaying field
//
//
void Get::display_get( GetFld *getPtr,int getting)
{
   char  buf[MAX_GET_WIDTH+1] ;
   
   load_data( getPtr, buf ) ;

   if( getting || getPtr->data )
		font_ptr->disp( getPtr->x, getPtr->y, buf, getPtr->x2 );

	sys.blt_virtual_buf();
}
//-------- END OF FUNCTION Get::display_get ------------//


//------ BEGIN OF FUNCTION Get::display ---------------//
//
// Description : display all gets without read
//
// Syntax      : display()
//
void Get::display()
{
	GetFld *getPtr ;
	int    i      ;

	for (i=0; i<get_num ; i++ )
	{
		getPtr = fld_array + i;

		check_data( getPtr );
		display_get( getPtr, 0 );
	}
}
//-------- END OF FUNCTION Get::display ---------------//


//-------- BEGIN OF FUNCTION Get::store_data --------------//
//
// Description : fill the get buffer with data
//
// Syntax      : store_data( <GetFld*>,<char*>,<char*>)
//
// <GetFld*> = get pointer
// <char*>   = buffer pointer
// <char*>   = picture pointer
//

void Get::store_data( GetFld *getPtr, char *buffer, char *pict )
{
   //------- Currently it only support "Character" type -----//

/*
   // Transfer Back to  getPtr->data //
	switch ( getPtr->type )
	{
      case 'd':
         *((double *)getPtr->data) =
                     c1atod( buffer, getPtr->width_scr ) ;
         break ;

      case 'N':
         memcpy( getPtr->data,
                 c1dtoa( c1atod(buffer, getPtr->width_data),
                         getPtr->width_data, getPtr->dec_num),
                 (size_t) getPtr->width_data ) ;
         break ;

      case 'c':
         *((char*)getPtr->data) =
                 (char) c1atoi( (char *) buffer, getPtr->width_scr ) ;
         break ;

      case 'i':
			 *((int *)getPtr->data) =
					  (int) c1atoi( (char *) buffer, getPtr->width_scr ) ;
         break ;

      case 'l':
         buffer[getPtr->width_scr] =  '\0' ;
         *((long *)getPtr->data) =  (long) atol( (char *) buffer ) ;
         break ;

      case 'D':
         memcpy( getPtr->data, c1dt_unformat((char *) buffer, pict), 8) ;
         break;

      case 'L':
         if ( buffer[0] == (int) 'Y'  ||  buffer[0] == (int) 'T' ||
              buffer[0] == (int) 't' || buffer[0] == (int) 'y' )
            *((int *)getPtr->data) =  1 ;
         else
            *((int *)getPtr->data) =  0 ;
         break ;

      default:
         memcpy( getPtr->data, buffer, getPtr->width_data ) ;
         getPtr->data[getPtr->width_data]=NULL;
         break ;
   }
*/
   //------- Currently it only support "Character" type -----//

   if( getPtr->type=='C' )
   {
      memcpy( getPtr->data, buffer, getPtr->width_data ) ;

      ((char*)getPtr->data)[getPtr->width_data] = NULL;

      m.rtrim( (char*) getPtr->data );    // right rtrim the character
   }
}
//---------- END OF FUNCTION Get::store_data -------------//


//-------- BEGIN OF FUNCTION Get::right_pos --------------//
//
// Description : calculate the correct cursor position by passing
//              by the picture digit (e.g. "(999)", than pass '(' and ')' )
//
// Syntax      : right_pos( <int>,<int>,<char*> )
//
// <int>   = buffer position
// <int>   = the length of the buffer
// <char*> = picture pointer
//
int Get::right_pos( int buffer_pos, int len, char *pict )
{
   buffer_pos++ ;

   while ( buffer_pos < len )
   {
      if ( strchr(PICTURE_CHRS,pict[buffer_pos]) == 0)
         buffer_pos++ ;
      else
         break ;
   }

   if ( buffer_pos == len)
   {
      buffer_pos-- ;

      while (buffer_pos > 0 &&
            strchr(PICTURE_CHRS,pict[buffer_pos]) == 0)
      {
         buffer_pos-- ;
      }
   }

   return  buffer_pos ;
}
//------ END OF FUNCTION Get::right_pos -------------//


//-------- BEGIN OF FUNCTION Get::num_pos -------------//
//
// Description : Counts the Number of Positions to the Next Picture Data Character
//
// Syntax      : num_pos( <int>,<int>,<char*> )
//
// <int>   = buffer position
// <int>   = the length of the buffer
// <char*> = picture pointer
//
int Get::num_pos( int bufPos, int len, char *pict )
{
   int  on_pos ;

   on_pos =  bufPos ;

   while ( on_pos < len )
   {
      if ( strchr(PICTURE_CHRS,pict[on_pos]) == 0)
         break ;
      else
         on_pos++ ;
   }

   return ( on_pos - bufPos ) ;
}
//-------- END OF FUNCTION Get::num_pos -------------//


//--------- BEGIN OF FUNCTION Get::read ------------//
//
// Description : read the pre-defined gets
//
// Syntax      : read([int])
//
// [int] = the field to be read first
//         (default : the first field in the get list, 1)
//
// Return : <int> the last key pressed for exiting read
//                e.g. ESC, ENTER.
//
int Get::read(int firstRead)
{
   int    firstFlag, firstKey;
   int    bufPos, curPos;
   int    bufOffset, bufWidth;
   int    fldId;           // id. of the GetFld in the fld_array
   int    num;             // temp value for location character
	int    keyCode;

   GetFld *getPtr ;
   char   dataBuf[MAX_GET_WIDTH+1], pictBuf[MAX_GET_WIDTH+1] ;


   //-------------- init vars -------------------//

   err_when( get_num == 0 );

   firstFlag = 1;          // whether it's the first time on the field or not
   firstKey  = 1;          // If the firstFlag key press is not cursor key, clear the field content, it can be any one of the types c,i,l,d,N
   fldId     = firstRead;
   getPtr    = fld_array + fldId - 1;

	display_get( getPtr, 1 );    // display current getting field


   //--------------------------------------------//

   for (;;)
   {
      //------ it's the first time on this field --------//

      if (firstFlag)
      {
         if ( getPtr->type == 's' )
				cursor_type(0);
         else
         {
            if ( insert_flag )
               cursor_type(2) ;
            else
               cursor_type(1) ;
         }
      }
      else
         firstKey = 0;  // firstKey is for clearing numerical field if the first key stroke is not cursor key

      //--------------------------------------------//

      if ( firstFlag )  // First time for this 'get'
      {
         init_field(dataBuf, pictBuf, getPtr, bufPos, bufOffset, bufWidth);
         firstFlag = 0 ;
      }

      //--------- second time ( firstFlag == 0 ) ---------//

		keyCode = get_next_key( dataBuf, pictBuf, getPtr, bufPos, bufOffset,
                              bufWidth );

		if( !keyCode )
         continue;

      //------------ cusror movement function keys ------------//

		if( process_field_key(keyCode, getPtr, dataBuf, pictBuf, curPos, bufPos) )
      {
                                   // return 1 if field movement key
         continue;                 // return 0 if alphabetical key / inter-field control key
      }

      //----------- Normal Alphanumerical keys --------------//

		if ( keyCode >= 0x20 && keyCode <= 0xFF )
      {
         //------ Check the Picture Template -------//

			if( valid_pict(keyCode, getPtr, dataBuf, pictBuf, bufPos) )
         {
				//----- insert or Replace the character ---------//

            if ( m.str_chr("cildNC", getPtr->type) )    // If the firstFlag key press is not cursor key, clear the field content, it can be any one of the types c,i,l,d,N
            {
               if ( firstKey )
                  memset(dataBuf,' ',getPtr->width_data);
            }

            if ( insert_flag )
            {
               // Insert the Character //
               num = num_pos( bufPos, getPtr->width_data, pictBuf) ;

               if ( num > 1 )
                  memmove( dataBuf+bufPos+1, dataBuf+bufPos, (size_t) num-1) ;
            }

				dataBuf[bufPos] = keyCode;

            if ( bufPos < bufWidth-1 )
               bufPos++;
         }

         continue;
      }
      else //--------- field movement function keys -----------//
      {
			int rc = process_inter_field_key( keyCode, getPtr, firstFlag, firstKey, fldId, bufPos);

         if( rc != -1 )    // -1 means exit read() immediately
         {
				int rc = store_and_valid( keyCode, dataBuf, pictBuf, getPtr, firstFlag );

            if( !rc )       // not valid
               continue;
         }

         //......... Exit Get and Return last pressed key .............//

         if( rc == -1 )    // -1 means exit read() immediately
         {
            store_data( getPtr, dataBuf, pictBuf ) ; // save the previous field dataBuf data

				display_get(getPtr,0);

            cursor_type( 1 ) ;
				return keyCode;
         }

         continue ;
      }
   }
}
//--------- END OF FUNCTION Get::read ----------------//


//-------- BEGIN OF FUNCTION Get::cursor_type -----------//
//
// Description : Set cursor type, represent insert mode or overwrite mode
// Syntax      : cursor_type()
//
// Remark : hasn't be implemented yet
//
void Get::cursor_type(int newType)
{
   newType++;
}
//-------- END OF FUNCTION Get::cursor_type ------------//



//------- Begin of function Get::process_field_key -----------//
//
// Called by read() only
//
// return : <int> 1 - it's a control/function key
//                0 - it's an alphabetic key
//
int Get::process_field_key( int keyCode, GetFld* getPtr, char* dataBuf,
                            char* pictBuf, int& curPos, int& bufPos)
{
   int num;

	switch ( keyCode )
   {
      case KEY_INS:
         insert_flag =  ! insert_flag ;
         if ( insert_flag )
				cursor_type( 2 ) ;
         else
            cursor_type( 1 ) ;
         return 1;

      case KEY_BACK_SPACE:
      case KEY_LEFT:
         curPos =  bufPos - 1 ;

         while (curPos >= 0)
         {
            if ( strchr(PICTURE_CHRS,pictBuf[curPos]) == 0 )
               curPos-- ;
            else
               break;
         }

         if ( curPos < 0 )
            return 1 ;

         bufPos =  curPos ;
			// ####### begin Gilbert 24/7 ########//
         if ( keyCode == KEY_LEFT ) // || keyCode == KEY_CTRL_S )
            return 1 ;
			// ####### end Gilbert 24/7 ########//

         // BACK_SPACE is a LEFT and a DEL //

      case KEY_DEL:
         num =  num_pos( bufPos, getPtr->width_data, pictBuf) ;
         if ( num == 0 )
            return 1 ;
         memcpy( dataBuf+bufPos, dataBuf+bufPos+1, num ) ;
         dataBuf[bufPos+num-1] = ' ' ;

         err_when( bufPos+num > getPtr->width_data );

         return 1;

			// ####### begin Gilbert 24/7 ########//
			/*
      case KEY_CTRL_Y:
         curPos =  bufPos ;
         memset( dataBuf + bufPos, (int) ' ',
            (size_t) num_pos( bufPos, getPtr->width_data, pictBuf));
         return 1;
			*/
			// ####### end Gilbert 24/7 ########//

      case KEY_RIGHT:
			bufPos =
            right_pos( bufPos, getPtr->width_data, pictBuf);
         return 1 ;

      case KEY_HOME:
         bufPos = 0;
         return 1;

      case KEY_END:
         bufPos =  getPtr->width_data - 1;

         while ( bufPos >= 0 )
         {
            if (dataBuf[bufPos] == ' ' || dataBuf[bufPos] == '\0')
               bufPos-- ;
            else
               break;
         }

         bufPos =
            right_pos( bufPos, getPtr->width_data, pictBuf);
         return 1 ;
   }

   return 0;
}
//------- End of function Get::process_field_key -----------//



//------- Begin of function Get::valid_pict -----------//
//
// Called by read() only
//
// return : <int> 1 - ok, the key is validated with the picture
//                0 - not ok, invalid with picture
//
int Get::valid_pict( int keyCode, GetFld* getPtr, char* dataBuf,
                     char* pictBuf, int& bufPos )
{
   if( !pictBuf[bufPos] )
      return 1;

   int ok=0, i, num;

   switch( pictBuf[bufPos] )
   {
      case '9':
      case '#':
         if ( (keyCode >= '0' && keyCode <= '9') || keyCode == '-' )
            ok = 1 ;

         if ( (keyCode == '+' || keyCode == '-' || keyCode == ' ')    &&
                             pictBuf[bufPos] == '#')
         {
            ok =  1 ;
         }

         if ( keyCode == '.'  &&  getPtr->dec_num > 0 )
         {
            int  num_shift ;

            num_shift =  getPtr->width_scr - getPtr->dec_num -
                         bufPos - 1 ;

            if ( --bufPos < 0 )  bufPos = 0 ;

            if ( num_shift > 0  &&  dataBuf[bufPos] != ' ' )
            {
               memmove( dataBuf+ num_shift, dataBuf,
                        (size_t) (getPtr->width_scr - num_shift) ) ;
               memset( dataBuf, (int) ' ', (size_t) num_shift ) ;
            }

            bufPos = getPtr->width_scr - getPtr->dec_num-1;
            memset( dataBuf+ bufPos+1, (int) '0',
                    (size_t) getPtr->dec_num);
            ok = 1 ;
         }
         break ;

      case 'A':
         if ( keyCode >= 'A' && keyCode <= 'Z'  ||
              keyCode >= 'a' && keyCode <= 'z' )
         {
            ok =  1 ;
         }
         break ;

      case 'L':
         keyCode =  (char) m.upper( keyCode ) ;
         if ( keyCode == 'Y' || keyCode == 'N' || keyCode == 'T' || keyCode == 'F' )
            ok = 1;
         break ;

      case 'N':
         if ( keyCode >= 'A' && keyCode <= 'Z'  ||
              keyCode >= 'a' && keyCode <= 'z'  ||
              keyCode >= '0' && keyCode <= '9'  )
         {
            ok =  1 ;
         }
         break ;

      case 'Y':
         if ( getPtr->type == 'D' )  // Date Type: 'Y' for year //
         {
            if ( keyCode >= '0'  &&  keyCode <= '9' || keyCode == ' ' )
               ok =  1 ;
         }
         else
         {
            keyCode =  (char) m.upper( keyCode ) ;

            if ( keyCode == 'Y' ||  keyCode == 'N' )
               ok =  1 ;
         }
         break ;

      case 'X':
      case '!':
         ok =  1 ;
         break ;

      case 'C':
      case 'D':
         if ( getPtr->type == 'D' &&
              (keyCode >= '0'  &&  keyCode <= '9' ||  keyCode == ' ')  )
         {
            ok =  1 ;
         }
         break ;

      case 'M':
         if ( getPtr->type == 'D' )
         {
            if ( keyCode == ' ' )
            {
               ok = 1 ;
               break ;
            }

            for ( num=0, i=0; pictBuf[i] != '\0'; i++ )
               if ( pictBuf[i] == 'M' ) num++ ;

            if ( num > 2 )
            {
               if ( keyCode >= 'A' && keyCode <= 'Z'  ||
                    keyCode >= 'a' && keyCode <= 'z')
               {
                  ok =  1;
               }
            }
            else
            {
               if ( keyCode >= '0' && keyCode <= '9')
                  ok =  1;
            }
         }
         break ;
   }

   return ok;
}
//------- End of function Get::valid_pict -----------//



//------- Begin of function Get::process_inter_field_key -----------//
//
// Called by read() only
//
// return : <int> 1  - it's a inter-field control key
//                0  - it's not
//                -1 - exit read because exiting key is pressed
//
int Get::process_inter_field_key( int keyCode, GetFld* getPtr, int& firstFlag,
                                  int& firstKey, int& fldId, int& bufPos )
{
   int newFldId=0;    // newFldId is set when user move to other field, compare fldId and newFldId, we can know the change

   switch( keyCode )
   {
			// ####### begin Gilbert 24/7 ########//
			/*
      case KEY_CTRL_HOME:
         firstFlag = firstKey = 1;      // firstFlag time for this field
	 newFldId  = 1;   // get number is the firstFlag one
         break;

      case KEY_CTRL_END:
         firstFlag = firstKey = 1 ;
         newFldId  = get_num;   // last one
         break;
			*/
			// ####### begin Gilbert 24/7 ########//

      case KEY_UP:
			// ####### begin Gilbert 24/7 ########//
      // case KEY_CTRL_Z:
			// ####### end Gilbert 24/7 ########//
         firstFlag = firstKey = 1 ;
         newFldId = fldId-1;
         break;

      case KEY_DOWN:
      case KEY_RETURN:
      case KEY_TAB:
			// ####### begin Gilbert 24/7 ########//
      // case KEY_CTRL_B:
			// ####### end Gilbert 24/7 ########//
         firstFlag = firstKey = 1;
         bufPos =  0 ;
         newFldId = fldId+1;
         break;

      case KEY_PGUP:
      case KEY_PGDN:
	 if (up_down_exit)
            return -1;      // exit read()
         break;
   }

   //-------- Also Return if a Function Key was Pressed -------//

   if (
			// ####### begin Gilbert 24/7 ########//
		// keyCode == KEY_CTRL_W  ||   keyCode == KEY_CTRL_Q  ||
			// ####### begin Gilbert 24/7 ########//
       keyCode == KEY_ESC     ||   keyCode == KEY_F10     ||
      ( keyCode >= KEY_F1 && keyCode <= KEY_F9 && up_down_exit ) ||
		 keyCode < 0  )
   {
      return -1;        // exit read()
   }

   //--------- press up in the upper most field --------//

   if ( newFldId > get_num )
   {
      if( keyCode == KEY_RETURN || keyCode == KEY_TAB || up_down_exit )
         return -1;     // exit read()
      else
         newFldId = get_num;                 // down or TAB is pressed and up_down_exit flag is not set
   }

   //------ or down key in the bottom most field -------//

   if ( newFldId < 1 )
   {
      if ( up_down_exit )   // exit when up is pressed at the firstFlag field
         return -1;         // exit read()
      else
         newFldId = 1;      // else than reassign it to fist field
   }

   //------- assign the new field pointer ---------//

   if( newFldId )
   {
      if (fldId != newFldId)
      {
	 display_get( getPtr, 0 );
         fldId = newFldId;
         getPtr = fld_array + fldId - 1;
	 display_get( getPtr, 1 );
      }

      return 1;
   }

   return 0;
}
//------- End of function Get::process_inter_field_key -----------//


//------- Begin of function Get::init_field -----------//
//
// Called by read() only when it's the firstFlag time on the field (get)
//
void Get::init_field(char* dataBuf, char* pictBuf  , GetFld* getPtr,
                     int& bufPos , int&  bufOffset, int &bufWidth )
{
   bufPos    = 0;
   bufOffset = 0;

   //-- Buffer is filled and the width is returned by 'load_data' -//

   bufWidth =  load_data( getPtr, dataBuf ) ;
   dataBuf[bufWidth] =  '\0' ;


   //----- picture buffer is filled --------//

   memset( pictBuf, 0, bufWidth+1 );

   if ( getPtr->picture != (char *) 0 )
   {
      strncpy( pictBuf, getPtr->picture, bufWidth );
   }
   else
   {
      switch( getPtr->type )
      {
         case 'D':
            strcpy( pictBuf, DEFAULT_DATE ) ;
            break ;

         case 'N':
         case 'd':
         case 'i':
         case 'c':
            memset( pictBuf, (int) '#', (size_t) getPtr->width_scr ) ;

            if ( getPtr->dec_num > 0)
            {
               int t = getPtr->width_scr - getPtr->dec_num - 1 ;

               dataBuf[t] = '.';
					pictBuf[t] = '.';
            }
            break ;

         case 'L':
            pictBuf[0] = 'L' ;
            pictBuf[1] = '\0' ;
            break ;
      }
   }
}
//------- End of function Get::init_field -----------//


//------- Begin of function Get::get_next_key ---------//
//
// called by read() only
//
// return : <int> keyCode - key entered by user
//                0       - character from picture template, no needed to input by user
//
int Get::get_next_key( char* dataBuf, char* pictBuf, GetFld* getPtr,
                       int&  bufPos , int&  bufOffset, int &bufWidth )
{
   int keyCode;

	if ( bufPos >= bufWidth )   // the cursor exceeds the field width
	{
      keyCode = KEY_RETURN ;                   // than simulate a RETURN key to next field
		font_ptr->disp( getPtr->x, getPtr->y, dataBuf, getPtr->x2 );
   }
   else
   {
      keyCode =  0 ;

      // if the picture has a mask like e.g. "(999) 9999-999" then the '(',')' and '-' position will not be got.

      if ( strchr( PICTURE_CHRS, pictBuf[bufPos] ) == (char *) 0  &&
           (int) pictBuf[bufPos] >= 0x20  &&
           (int) pictBuf[bufPos] <= 0xFF)
      {
         dataBuf[bufPos] = pictBuf[bufPos] ;
         if ( bufPos < bufWidth-1 )
            bufPos++ ;

         return 0;
      }

		//------ otherwise, check bufOffset --------//

      if ( bufOffset <= bufPos - getPtr->width_scr )
         bufOffset =  1+ bufPos - getPtr->width_scr ;

      if ( bufPos < bufOffset )
         bufOffset =  bufPos ;

      //--------- Say field content ------------//

		font_ptr->disp( getPtr->x, getPtr->y, dataBuf+bufOffset, getPtr->x2 );

		int cursorX = getPtr->x + font_ptr->text_width( dataBuf, bufPos-bufOffset );


      //-------- Call defined function before entering the field ------//

      if ( getPtr->call_ptr )
      {
         dataBuf[bufWidth] = '\0' ;
			keyCode = (*getPtr->call_ptr)( getPtr, dataBuf, getPtr->call_data);

			if ( keyCode == -1 )
				return 0;
      }

      //--- Get key from keyboard and convert to upper case if necessary ----//

		if ( keyCode == 0 )
		{
			sys.yield();

			DWORD nextTime=m.get_time();
			int   cursorDisplay=1;

			// ####### begin Gilbert 25/7 ##########//
			while( mouse.is_key_event() ? ((keyCode = mouse.key_code),mouse.get_event(),0) : 1 )   // while waitingfor input, flash the cursor
			// ####### end Gilbert 25/7 ##########//
			{
				if( m.get_time() >= nextTime )
				{
					if( cursorDisplay )    // currently display -> vanish
						vga.active_buf->bar( cursorX-1, getPtr->y, cursorX, getPtr->y+font_ptr->height()-1, V_WHITE );
					else                    // currently vanish -> display
						vga.active_buf->bar_down( cursorX-1, getPtr->y, cursorX, getPtr->y+font_ptr->height()-1 );

					cursorDisplay = !cursorDisplay;
					nextTime = m.get_time() + 300;          // flash frequency : 0.3 seconds
				}

				sys.yield();
				mouse.get_event();

				sys.blt_virtual_buf();

				//----- if the mouse is press outside the field, exit ----//

				if( mouse.any_click() )
				{
					if( mouse.click_x() < fld_x1 || mouse.click_x() > fld_x2 ||
						 mouse.click_y() < fld_y1 || mouse.click_y() > fld_y2 )
					{
						keyCode = KEY_F10;        // simulate a F10 to exit
						break;
					}
				}
			}

			if( !cursorDisplay )    // last time is display, then erase it
				vga.active_buf->bar_down( cursorX-1, getPtr->y, cursorX, getPtr->y+font_ptr->height()-1 );

			sys.blt_virtual_buf();
		}

		if ( (getPtr->upper_convert ||  pictBuf[bufPos] == '!')  &&  keyCode >= 0x20  &&  keyCode <= 0xFF )
			keyCode = m.upper( keyCode ) ;
	}

	return keyCode;
}
//------- End of function Get::get_next_key ---------//


//------- Begin of function Get::store_and_valid ---------//
//
// called by read() only
//
// return : <int> 1  - valid
//                0  - not valid
//                -1 - exit read
//
int Get::store_and_valid( int keyCode, char* dataBuf, char* pictBuf, GetFld* getPtr,
                          int& firstFlag)
{
   if( firstFlag )
   {
      store_data( getPtr, dataBuf, pictBuf ) ; // save the previous field dataBuf data

      //-------- Call Check Validation function ----------//

		// ####### begin Gilbert 24/7 ########//
		if ( getPtr->valid_ptr  && keyCode != KEY_ESC ) // && keyCode != KEY_CTRL_Q )
		// ####### end Gilbert 24/7 ########//
      {
         if (!( (*getPtr->valid_ptr)() ) )
            return 0;
      }


      //-------- Check validation of date content ---------//


      //------- Currently it only support "Character" type -----//
      /*
		if ( getPtr->type == 'D' && keyCode != KEY_ESC && keyCode != KEY_CTRL_Q )
      {
         double  temp_data ;
			int     date_rc ;

         // Date Check
         date_rc =  c1dt_julian( (char *) getPtr->data, &temp_data ) ;

         if ( date_rc == -1 )
         {
            dataBuf[bufWidth] = '\0' ;
            return 0;
         }
      }
      */
      //------- Currently it only support "Character" type -----//
   }

   return 1;
}
//------- End of function Get::store_and_valid ---------//
