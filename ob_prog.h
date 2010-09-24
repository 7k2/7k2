// Filename    : OB_PROG.H
// Description : header file for BulletProgram

#ifndef __OB_PROG_H
#define __OB_PROG_H

#include <OBULLET.H>

class BulletProgram : public Bullet
{
public:
	int delay_step;

public:
	BulletProgram();
	~BulletProgram();
	void 	init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType);
	void 	process_move();
	int	process_die();
	char  display_layer();
	void	draw(int outLine, int drawFlag);
	void  update_abs_pos();
	void  cur_sprite_frame();
	int	reflect( int baseObjRecno ) { return 0;}

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();
};

#endif