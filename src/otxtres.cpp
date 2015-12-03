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

// Filename    : OTXTRES.CPP
// Description : text resource


#include <otxtres.h>
#include <all.h>
#include <ofiletxt.h>
#include <c99_printf.h>
#include <stdlib.h>
#include <math.h>


#if(defined(SPANISH))
	#define THOUSAND_SEPARATOR '.'
	#define DECIMAL_SEPARATOR ','
#else
	#define THOUSAND_SEPARATOR ','
	#define DECIMAL_SEPARATOR '.'
#endif

// some country (like japan insert ',' every four digits)
#define THOUSAND_SEPARATOR_COUNT 3


// format of text resource file
// [<remark>mmmm]text...  where mmmm is a 4-character check mark, optional
//


// format of the format string : 

// %%              a single percent
// %<n>s           string, <n> is from 1 to 9, the number in the argument list (eg. %1s)
// %<n>[,f]d       interger <f> is format type passed to Misc::format (eg. %2d)
// %<n>[,f]z       similar to d, but suppressed on zero
// %<n>[,f]f       float <f> is format type passed to Misc::format (eg. %3,1f)
// %<n>p{<s>|<p>}  use <s> singular form or <p> plural form, determined by integer argment <n>
//                 (e.g. %1p{box|boxes})
// %<n>q{<z>|<s>|<p>}  use <z> zero form, <s> singular form or <p> plural form, determined by integer argment <n>
//                 (e.g. %1z%1q{nothing| box| boxes})
// %<n>?{<t>|<f>}  integer argument <n> is non-zero use <t>; zero then <f>
// %<n>j{<c1>|<c2>|<c3>...<ci>}
//                 depend on integer argument <n>, use <c1> on 1, use <c2> on 2
//                 otherwise use the last argument, <ci>.
// %<n>r           roman number, blank if zero


// useage 
// 1. font_news.put( ZOOM_X1+10, dy, test_text_res.process( test_text_res.get_format_str(1, "CHM1"), "a", "bb", "ccc") );
// find format str in line 1, and check [CHM1] mark at the beginning of the that line
// the line appear in resource file may look like:
//
// [CHM1]Test 1 : s1(%1s), s2(%2s), s3(%3s) %%
//
// 2. more preferred method is to use a subclass
// 
//class GameMenuText : public TextResource
//{
//public:
//	void	init() { TextResource::init( <hard-coded file name> ); }
//
//	// ---------- string -----------//
//
// // non formatted text
//	char *quit_text() { return get_format_str( 1, "QUIT" ); }
//	char *single_player_text() { return get_format_str( 2, "SING" ); }
//
//	// argument string, check mark recognize first 4 chars only
//	char *kingdom_name(char *playerName) { return process( get_format_str( 3, "KNAME"), playerName); }
//};


// -------- define constant ----------//

#define MAX_ARG 9

#define CHECK_MARK_BEGIN '['
#define CHECK_MARK_END ']'

enum
{
	ARG_TYPE_INT = 1,
	ARG_TYPE_STR,
	ARG_TYPE_DOUBLE,
};

// --------- define static member ----------//

char TextResource::str_buf[FREE_STR_COUNT][TEXT_RES_STRLEN+1];
int TextResource::free_str_buf = 0;
char TextResource::short_str_buf[FREE_SHORT_STR_COUNT][TEXT_RES_SHORT_STRLEN+1];
int TextResource::free_short_str_buf = 0;

// ------- begin of function TextResource::TextResource -------//
//
TextResource::TextResource()
{
	main_text = NULL;
	text_count = 0;
	text_offset_array = NULL;

	res_name[0] = '\0';
}
// ------- end of function TextResource::TextResource -------//


// ------- begin of function TextResource::~TextResource -------//
//
TextResource::~TextResource()
{
	deinit();
}
// ------- end of function TextResource::~TextResource -------//


// ------- begin of function TextResource::init -------//
//
void TextResource::init( const char *fileName)
{
	deinit();		// free memory

	strcpy( res_name, fileName );
	load_res(fileName);
}
// ------- end of function TextResource::init -------//


// ------- begin of function TextResource::deinit -------//
//
void TextResource::deinit()
{
	if( main_text )
	{
		mem_del(main_text);
		main_text = NULL;
	}

	if( text_offset_array )
	{
		mem_del(text_offset_array);
		text_offset_array = NULL;
	}

	text_count = 0;
	res_name[0] = '\0';
}
// ------- end of function TextResource::deinit -------//


// ------- begin of function TextResource::load_res -------//
//
void TextResource::load_res(const char *fileName)
{
	// open and the whole file

	File file;
	file.file_open(fileName);
	int fileSize = file.file_size();

	FileTxt f(&file, fileSize);

	main_text = mem_add( fileSize );

	// build index and replace '\r' or '\n' to '\0'
	text_count = 0;

	int guessTextCount = fileSize / 64 + 1;		// initial guess, 64 is assumed char per line
	text_offset_array = (int *)mem_add( sizeof(int) * guessTextCount );
	int offset = 0;

	while( !f.is_eof() )
	{
		f.read_line( main_text+offset, fileSize-offset );

		// if a line has check mark, move offset behind ']'
		if( main_text[offset] == CHECK_MARK_BEGIN
			&& strchr(main_text+offset, CHECK_MARK_END) )
		{
			offset = strchr(main_text+offset, CHECK_MARK_END) - main_text + 1;
			// +1 to skip that CHECK_MARK_END char
		}

		// build index, allocate space first
		if( text_count >= guessTextCount )
		{
			guessTextCount = text_count + 16;
			text_offset_array = (int *)mem_resize( text_offset_array, sizeof(int) * guessTextCount );
		}
		text_offset_array[text_count] = offset;

		offset += strlen(main_text+offset) + 1;	// skip the null char
		++text_count;
	}
}
// ------- end of function TextResource::load_res -------//


// ------- begin of function TextResource::get_format_str --------//

char *TextResource::get_format_str(int textId, const char *checkMark )
{
	err_when( !textId && !checkMark );		// cannot be both null
	err_when( sizeof(uint32_t) != CHECK_MARK_STRLEN );	// else use strncmp instead

	if( textId > 0 && textId <= text_count )
	{
		char *formatText = main_text + text_offset_array[textId-1];
		if( !checkMark				// skip mark checking
			||	(formatText > main_text
			&&	formatText[-1] == CHECK_MARK_END
//			&& strncmp(formatText-1-CHECK_MARK_STRLEN, checkMark, CHECK_MARK_STRLEN) == 0) )
			&& *(uint32_t *)(formatText-1-CHECK_MARK_STRLEN) == *(uint32_t *)checkMark) )
		{
			return formatText;
		}
	}

	err.run("Cannot find string %s in %s", checkMark, res_name );
	return NULL;
}
// ------- end of function TextResource::get_format_str --------//

// ------ begin of function TextResource::get_free_str -------//
//
// use str_buf in cycle
// so small recursed call in process() is possible
//
char *TextResource::get_free_str(char* srcStr)
{
	err_when( free_str_buf < 0 );

	char *rc = str_buf[free_str_buf];
	if( ++free_str_buf >= FREE_STR_COUNT )
		free_str_buf = 0;

	if( srcStr )		// copy src string
	{
		strncpy( rc, srcStr, FREE_STR_COUNT );
		rc[FREE_STR_COUNT] = '\0';
	}
	return rc;
}
// ------ end of function TextResource::get_free_str -------//


// ------ begin of function TextResource::get_free_short_str -------//
//
// use short_str_buf in cycle
// so simultaneous call in format(), roman() is possible
//
char *TextResource::get_free_short_str(char *srcStr)
{
	err_when( free_short_str_buf < 0 );

	char *rc = short_str_buf[free_short_str_buf];
	if( ++free_short_str_buf >= FREE_SHORT_STR_COUNT )
		free_short_str_buf = 0;

	if( srcStr )		// copy src string
	{
		strncpy( rc, srcStr, FREE_SHORT_STR_COUNT );
		rc[FREE_SHORT_STR_COUNT] = '\0';
	}
	return rc;
}
// ------ end of function TextResource::get_free_short_str -------//


//---------- Begin of function TextResource::format --------//
//
// similar to Misc::format, but store in a private storage
// 
char* TextResource::format(double inNum, int formatType)
{
   char *outBuf = get_free_short_str();
   char   *outPtr=outBuf;
   char   floatBuf[35];
   char   *floatStr;
   int    i, intDigit;    // intDigit = no. of integer digits

   intDigit = snprintf( floatBuf, sizeof(floatBuf), "%.0lf", fabs(inNum) * 100.0 );
   intDigit -= 2;
   floatStr = floatBuf;

   #ifdef DEBUG
      if( intDigit > 29 )            // integer digits can't exceed 29
         err.run( "TextResource::format(), inNum : %e, formatType : %d", inNum, formatType );
   #endif

   //--------- negetive bracket ------------//

   if( inNum < 0 )
      *outPtr++ = '(';

   //--------- dollar sign ($) ------------//

   if( formatType == 2 )
      *outPtr++ = '$';

   //------- integer number -----------//

   for( i=intDigit ; i>0 ; i-- )
   {
      if( i%THOUSAND_SEPARATOR_COUNT == 0 && i < intDigit )
         *outPtr++ = THOUSAND_SEPARATOR;

      *outPtr++ = *floatStr++;
   }

   if( intDigit <= 0 )
      *outPtr++ = '0';

   //------- dec. place number -----------//

   if( inNum > -1000 && inNum < 1000 )    // if the number is less than 1000, add dec. places
   {                                      // if the number is greater than 1000, truncate any dec. places
      *outPtr++ = DECIMAL_SEPARATOR;

      if( *floatStr && intDigit >= 0 )    // e.g. 0.03 --> str:"3", intDight:-1
         *outPtr++ = *floatStr++;         // 1st dec. place
      else
         *outPtr++ = '0';                 // add a '0' when intDigit < 0

      if( *floatStr )                     // 2nd dec. place
         *outPtr++ = *floatStr++;
      else
         *outPtr++ = '0';
   }

   //--------- percent sign (%) ------------//

   if( formatType == 3 )
      *outPtr++ = '%';

   //--------- negetive bracket ------------//

   if( inNum < 0 )
      *outPtr++ = ')';

   *outPtr++ = '\0';

   return outBuf;
}
//---------- End of function TextResource::format --------//


//---------- Begin of function TextResource::format --------//
//
// similar Misc::format, but store in a private storage
//
char* TextResource::format(int inNum, int formatType)
{
   char *outBuf = get_free_short_str();
   char longBuf[25];
   char   *outPtr=outBuf;
   char   *longStr;
   int    i, intDigit, sign;

   if( inNum < 0 )
   {
      sign  = -1;
      inNum = -inNum;
   }
   else
      sign  = 0;

   longStr  = ltoa( inNum, longBuf, 10 );
   intDigit = strlen(longStr);  // no. of integer digits

   //--------- negetive bracket ------------//

   if( sign < 0 )
      *outPtr++ = '(';

   //--------- dollar sign ------------//

	if( formatType == 2 )
      *outPtr++ = '$';

	//-------- integer number -----------//

	for( i=intDigit ; i>0 ; i-- )
	{
		if( formatType != 4 )		// no thousand separators for format 4
		{
			if( i%THOUSAND_SEPARATOR_COUNT == 0 && i < intDigit )
				*outPtr++ = THOUSAND_SEPARATOR;
		}

		*outPtr++ = *longStr++;
	}

	//--------- percent sign (%) ------------//

	if( formatType == 3 )
      *outPtr++ = '%';

   //--------- negetive bracket ----------//

   if( sign < 0 )
      *outPtr++ = ')';

   *outPtr++ = '\0';

   return outBuf;
}
//---------- End of function TextResource::format --------//


//---------- Begin of function TextResource::roman_number --------//
//
char* TextResource::roman_number(int inNum)
{
	char *outBuf = get_free_short_str();
	char *outPtr = outBuf;

	if( inNum >= 4000 )
		err.run( "TextRes::roman_number %d", inNum );

	if( inNum <= 0 )
	{
		// return empty string
		outPtr[0] = '\0';			
		return outBuf;
	}

	char deciStr[12];
	itoa( inNum, deciStr, 10 );

	int deciLen = strlen( deciStr );

	for( int i = 0; i < deciLen; ++i )
	{
		static const char *unitDigitArray[10] = { "", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };
		static const char *tenDigitArray[10] = { "", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC" };
		static const char *hundredDigitArray[10] = { "", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM" };
		static const char *thousandDigitArray[] = { "", "M", "MM", "MMM" };	// 5000? 

		const char *srcStr = "";	// empty string
		switch( deciLen-i )
		{
		case 1:		// unit digit
			srcStr = unitDigitArray[deciStr[i]-'0'];
			break;
		case 2:		// ten digit
			srcStr = tenDigitArray[deciStr[i]-'0'];
			break;
		case 3:		// hundred digit
			srcStr = hundredDigitArray[deciStr[i]-'0'];
			break;
		case 4:		// thousand digit
			srcStr = thousandDigitArray[deciStr[i]-'0'];
			break;
		}

		// strcpy
		for( ; *srcStr; ++srcStr )
		{
			*outPtr = *srcStr;
			++outPtr;
		}
	}
	*outPtr = '\0';		// null terminate the string

	return outBuf;
}
//---------- End of function TextResource::roman_number --------//


// %%              a single percent
// %<n>s           string, <n> is from 1 to 9, the number in the argument list (eg. %1s)
// %<n>[,f]d       interger <f> is format type passed to Misc::format (eg. %2d)
// %<n>[,f]f       float <f> is format type passed to Misc::format (eg. %3,1f)
// %<n>p{<s>|<p>}  use <s> singular form or <p> plural form, determined by integer argment <n>
//                 (e.g. %1p{box|boxes})
// %<n>?{<t>|<f>}  integer argument <n> is non-zero use <t>; zero then <f>
// %<n>j{<c1>|<c2>|<c3>...<ci>}
//                 depend on integer argument <n>, use <c1> on 1, use <c2> on 2
//                 otherwise use the last argument, <ci>.
// %<n>r           roman number, blank if zero
//                 

// VERY IMPORTANT NOTE:

// TextResource::process is not re-entrance
// because str_buf is common, store the intermediate result first

// if a parameter is String type to be casted to char *,
// cast to (char *) explicitly, variable argument cannot handle class object well.

char *TextResource::process( char *formatStr, ... )
{
	char *buf;
	va_list vl;

	va_start( vl, formatStr );
	buf = vprocess(formatStr, vl);
	va_end(vl);

	return buf;
}


char *TextResource::vprocess( char *formatStr, va_list vl )
{
	int argCount, nonArgCount;

	register char *readPtr, *writePtr;

	char argType[MAX_ARG];
	memset( argType, 0, sizeof(argType) );
	int intArgs[MAX_ARG];
	char *strArgs[MAX_ARG];
	double floatArgs[MAX_ARG];

	// scan the string to identify the type of each argment
	argCount = nonArgCount = 0;
	for( readPtr = formatStr; *readPtr != '\0'; ++readPtr )
	{
		if( *readPtr == '%' )
		{
			char *oldReadPtr = readPtr;
			++readPtr;

			if( *readPtr == '\0' )			// end of string
				break;
			else if( *readPtr == '%' )		// single percent
			{
				nonArgCount++;
			}
			else if( *readPtr >= '0' && *readPtr <= '9' )
			{
				// skip numeric 
				int argId = 0;
				for( ; *readPtr >= '0' && *readPtr <= '9'; ++readPtr )
				{
					argId = argId * 10 + (*readPtr - '0');
				}
				if( *readPtr == '\0' )
					break;

				// skip another numeric
				int formatId = 0;
				if( *readPtr == ',' )
				{
					++readPtr;
					if( *readPtr == '\0')
						break;
					// skip numeric 
					for( ; *readPtr >= '0' && *readPtr <= '9'; ++readPtr )
					{
						formatId = formatId * 10 + (*readPtr - '0');
					}
					if( *readPtr == '\0' )
						break;
				}

				switch( *readPtr )
				{
				case 's':
					if( argId > 0 && argId <= MAX_ARG )
						argType[argId-1] = ARG_TYPE_STR;
					break;
				case 'f':
					if( argId > 0 && argId <= MAX_ARG )
						argType[argId-1] = ARG_TYPE_DOUBLE;
					break;
				case 'd':
				case 'r':
				case 'z':
					if( argId > 0 && argId <= MAX_ARG )
						argType[argId-1] = ARG_TYPE_INT;
					break;
				case 'p':
				case 'q':
				case '?':
				case 'j':
					if( argId > 0 && argId <= MAX_ARG )
						argType[argId-1] = ARG_TYPE_INT;
					// search '}'
					char *endBrace = strchr( readPtr+1, '}' );
					if( endBrace )
						readPtr = endBrace;		// keep pointer at the end of format clause
					break;
				}

				if( argId > argCount )
					argCount = argId;
			}
		}	// end if *readPtr == '%'
	}

	if( !argCount && !nonArgCount )
	{
		return formatStr;		// no argument
	}

	// save the argument

	for( int argId = 1; argId <= argCount && argId <= MAX_ARG; ++argId )
	{
		switch( argType[argId-1] )
		{
		case ARG_TYPE_INT:
			intArgs[argId-1] = va_arg( vl, int );
			break;
		case ARG_TYPE_STR:
			strArgs[argId-1] = va_arg( vl, char *);
			break;
		case ARG_TYPE_DOUBLE:
			floatArgs[argId-1] = va_arg( vl, double );
			break;

		default:
			(void) va_arg( vl, int );
		}
	}


	// format string now

	char* oldWritePtr = writePtr = get_free_str();

	for( readPtr = formatStr; *readPtr != '\0'; ++readPtr )
	{
		if( *readPtr == '%' )
		{
			char *oldReadPtr = readPtr;
			++readPtr;

			if( *readPtr == '\0' )			// end of string
				break;
			else if( *readPtr == '%' )		// single percent
			{
				*writePtr = '%';
				++writePtr;
			}
			else if( *readPtr >= '0' && *readPtr <= '9' )
			{
				// skip numeric 
				int argId = 0;
				for( ; *readPtr >= '0' && *readPtr <= '9'; ++readPtr )
				{
					argId = argId * 10 + (*readPtr - '0');
				}
				if( *readPtr == '\0' )
					break;

				// skip another numeric
				int formatId = 0;
				if( *readPtr == ',' )
				{
					++readPtr;
					if( *readPtr == '\0')
						break;
					// skip numeric 
					for( ; *readPtr >= '0' && *readPtr <= '9'; ++readPtr )
					{
						formatId = formatId * 10 + (*readPtr - '0');
					}
					if( *readPtr == '\0' )
						break;
				}

				switch( *readPtr )
				{
				case 's':		// string
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_STR );
						char *strPtr = strArgs[argId-1];
						// strcpy, but we need end of string
						for( ; *strPtr != '\0'; ++strPtr )	
						{
							*writePtr = *strPtr;
							++writePtr;
						}
					}
					break;
				case 'f':		// float %<n>[,f]f
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_DOUBLE );
						char *strPtr = format(floatArgs[argId-1], formatId?formatId:1 );	// default format is 1
						// strcpy, but we need end of string
						for( ; *strPtr != '\0'; ++strPtr )	
						{
							*writePtr = *strPtr;
							++writePtr;
						}
					}
					break;
				case 'd':		// integer %<n>[,f]d
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr = format(intArgs[argId-1], formatId?formatId:1 );	// default format is 1
						// strcpy, but we need end of string
						for( ; *strPtr != '\0'; ++strPtr )	
						{
							*writePtr = *strPtr;
							++writePtr;
						}
					}
					break;
				case 'r':		// integer %<n>r
					if( argId > 0 && argId < MAX_ARG && intArgs[argId-1] )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr = roman_number(intArgs[argId-1]);
						// strcpy, but we need end of string
						for( ; *strPtr != '\0'; ++strPtr )	
						{
							*writePtr = *strPtr;
							++writePtr;
						}
					}
					break;
				case 'z':		// integer %<n>[,f]z, suppress when zero
					if( argId > 0 && argId <= MAX_ARG && intArgs[argId-1] )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr = format(intArgs[argId-1], formatId?formatId:1 );	// default format is 1
						// strcpy, but we need end of string
						for( ; *strPtr != '\0'; ++strPtr )	
						{
							*writePtr = *strPtr;
							++writePtr;
						}
					}
					break;
				case 'p':		// plural form %<n>p{<s>|<p>}
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr;
						if( intArgs[argId-1] == 0 || intArgs[argId-1] == 1 )
						{
							// singular form
							if( readPtr[1] == '{' )
							{
								readPtr += 2;	// skip "p{"
								for( ; *readPtr != '\0' && *readPtr != '|'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
								if( (strPtr = strchr(readPtr, '}')) )
									readPtr = strPtr;		// go to '}'
							}
						}
						else
						{
							// plural form
							if( readPtr[1] == '{' 
								&&	(strPtr = strchr( readPtr+2, '|' )) )
							{
								readPtr = strPtr+1;	// skip '|'
								for( ; *readPtr != '\0' && *readPtr != '}'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
							}
						}
					}
					else
					{
						// skip to end of clause
						char *strPtr;
						if( (strPtr = strchr(readPtr, '}')) )
							readPtr = strPtr;		// go to '}'
					}
					break;
				case 'q':		// plural form %<n>q{<z>|<s>|<p>}
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr;
						if( intArgs[argId-1] == 0 )
						{
							// zero form
							if( readPtr[1] == '{' )
							{
								readPtr += 2;	// skip "q{"
								for( ; *readPtr != '\0' && *readPtr != '|'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
								if( (strPtr = strchr(readPtr, '}')) )
									readPtr = strPtr;		// go to '}'
							}
						}
						else if( intArgs[argId-1] == 1 )
						{
							// singular form
							if( readPtr[1] == '{' 
								&&	(strPtr = strchr( readPtr+2, '|' )) )
							{
								readPtr = strPtr+1;	// skip '|'
								for( ; *readPtr != '\0' && *readPtr != '|'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
								if( (strPtr = strchr(readPtr, '}')) )
									readPtr = strPtr;		// go to '}'
							}
						}
						else
						{
							// plural form
							if( readPtr[1] == '{' 
								&&	(strPtr = strchr( readPtr+2, '|' ))
								&& (strPtr = strchr( strPtr+1, '|')) )	// find second |
							{
								readPtr = strPtr+1;	// skip '|'
								for( ; *readPtr != '\0' && *readPtr != '}'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
							}
						}
					}
					else
					{
						// skip to end of clause
						char *strPtr;
						if( (strPtr = strchr(readPtr, '}')) )
							readPtr = strPtr;		// go to '}'
					}
					break;
				case '?':          // logical form %<n>?{<t>|<f>}
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr;
						if( intArgs[argId-1] )
						{
							// true
							if( readPtr[1] == '{' )
							{
								readPtr += 2;	// skip "?{"
								for( ; *readPtr != '\0' && *readPtr != '|'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
								if( (strPtr = strchr(readPtr, '}')) )
									readPtr = strPtr;		// go to '}'
							}
						}
						else
						{
							// false
							if( readPtr[1] == '{' 
								&&	(strPtr = strchr( readPtr+2, '|' )) )
							{
								readPtr = strPtr+1;	// skip '|'
								for( ; *readPtr != '\0' && *readPtr != '}'; ++readPtr )
								{
									*writePtr = *readPtr;
									++writePtr;
								}
								if( *readPtr == '\0' )
									break;
							}
						}
					}
					else
					{
						// skip to end of clause
						char *strPtr;
						if( (strPtr = strchr(readPtr, '}')) )
							readPtr = strPtr;		// go to '}'
					}
					break;
				case 'j':	// %<n>j{<c1>|<c2>|<c3>...<ci>}
					if( argId > 0 && argId <= MAX_ARG )
					{
						err_when( argType[argId-1] != ARG_TYPE_INT);
						char *strPtr = NULL;
						// if( intArgs[argId-1] )

						if( readPtr[1] == '{' )
						{
							int a=1;
							strPtr = readPtr+2;	 // skip "j{";
							readPtr++;            // skip "j";

							while( *readPtr != '}' && a != intArgs[argId-1] )
							{
								++readPtr;				// skip '{' or '|'
								// skip to next argument
								for( ; *readPtr != '\0' && *readPtr != '|' && *readPtr != '}'; ++readPtr );
								if( *readPtr == '\0' || *readPtr == '}' )
									break;		// if } break so strPtr is not updated
								strPtr = readPtr+1;		// skip '\0','|'
								++a;
							}

							// if a never match intArgs[argId-1], use last phase before }
							// copy string

							readPtr = strPtr;
							for( ; *readPtr != '\0' && *readPtr != '|' && *readPtr != '}'; ++readPtr )
							{
								*writePtr = *readPtr;
								++writePtr;
							}
							if( *readPtr == '\0' )
								break;

							// skip to end of clause
							if( (strPtr = strchr(readPtr, '}')) )
							{
								readPtr = strPtr;
							}
						}
					}
					else
					{
						// skip to end of clause
						char *strPtr;
						if( (strPtr = strchr(readPtr, '}')) )
							readPtr = strPtr;		// go to '}'
					}
					break;
				} // end case

				err_when( argId > argCount );		// but argCount may > MAX_ARG
			}
			else
			{
				// unrecognized command, also write a %
				*writePtr = '%';
				++writePtr;
			}
		}	// end if *readPtr == '%'
		else
		{
			*writePtr = *readPtr;
			++writePtr;
		}
	}
	*writePtr = '\0';		// null terminate

	err_when( writePtr - oldWritePtr > TEXT_RES_STRLEN );
	return oldWritePtr;
}
