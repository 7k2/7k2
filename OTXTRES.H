// Filename    : OTXTRES.H
// Description : header file of text resource for translation


#ifndef __OTXTRES_H
#define __OTXTRES_H


#include <OSTR.H>
#include <stdarg.h>

#define CHECK_MARK_STRLEN 4
#define TEXT_RES_STRLEN 255
#define FREE_STR_COUNT 8
#define TEXT_RES_SHORT_STRLEN 31
#define FREE_SHORT_STR_COUNT 8

class TextResource
{
public:
	char	*main_text;
	int	text_count;
	int	*text_offset_array;

	char	res_name[128];
	static char	str_buf[FREE_STR_COUNT][TEXT_RES_STRLEN+1];
	static int free_str_buf;
	static char	short_str_buf[FREE_SHORT_STR_COUNT][TEXT_RES_SHORT_STRLEN+1];
	static int free_short_str_buf;

public:
	TextResource();
	~TextResource();

	void	init( char *fileName);
	void	deinit();
	int	is_inited() { return res_name[0]; }

	char	*get_format_str(int textId, char *checkMark );

	// see OTXTRES.H for detail descriptions
	static char	*process( char *formatStr, ... );
	static char *vprocess( char *formatStr, va_list );

	// --------- acquiring temp storage -----//

	static char *get_free_str(char* =NULL);
	static char *get_free_short_str(char* =NULL);

	// --------- formatting function -------//

   static char* format(double,int=1);
   static char* format(int,int=1);
	static char* roman_number(int);

private:
	void	load_res(char *fileName);
};


#endif