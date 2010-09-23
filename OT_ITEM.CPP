// Filename    : OT_ITEM.CPP
// Description : text resource item

#include <OT_ITEM.H>
#include <ALL.H>


void TextResItem::init()
{
	TextResource::init( DIR_RES"T_ITEM.RES" );
}


char*	TextResItem::str_item_name(int itemId)
{
	if( !itemId )
		return "";

	static char checkMark[CHECK_MARK_STRLEN+1] = "N000";
	err_when( itemId >= 100);
	checkMark[2] = itemId / 10 + '0';
	checkMark[3] = itemId % 10 + '0';
	return get_format_str( 0+itemId, checkMark );
}

char*	TextResItem::str_item_desc(int itemId)
{
	if( !itemId )
		return "";

	static char checkMark[CHECK_MARK_STRLEN+1] = "D000";
	err_when( itemId >= 100);
	checkMark[2] = itemId / 10 + '0';
	checkMark[3] = itemId % 10 + '0';
	return get_format_str( 40+itemId, checkMark );
}

