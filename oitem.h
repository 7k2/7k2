// Filename    : OITEM.H
// Description : item constant


#ifndef __OITEM_H
#define __OITEM_H

//------- Define struct item ----------//

class Skill;

struct Item
{
public:
	short	id;
	int	para;


public:
	Item()                               : id(0), para(0)     {}
	Item( short itemId, short itemPara ) : id(itemId), para(itemPara) {}

	void	set(short itemId, short itemPara) 	{ id = itemId; para = itemPara; }
	void	clear()										{ id = 0; }

	void	next_day();

	char *item_name();
	char *item_desc();
	int	cost();

	int	ability(int itemAbilityId);
	void	use_now();							// use the item now
	int	can_turn();
	int	can_turn_on();
	int	is_turn_on();
	void	turn_on();
	void	turn_off();
	int	can_use_manually();
	void	use_manually(int unitRecno, int targetBaseObjRecno=0, int para2=0);

	void 	init_random( int lowRare, int highRare, int unitId=0, Skill* =0);
	void	random_para();
};

//-------------------------------------//

#endif