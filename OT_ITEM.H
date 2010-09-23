// Filename    : OT_ITEM.H
// Description : header file of Text Resource Item


#ifndef __OT_ITEM_H
#define __OT_ITEM_H

#include <OTXTRES.H>

class TextResItem : public TextResource
{
public:
	void	init();

	char*	str_item_name(int itemId);
	char*	str_item_desc(int itemId);
};

extern TextResItem text_item;

#endif
