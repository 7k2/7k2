// Filename    : OB_HOMIN.H
// Description : header file for BulletHoming

#ifndef __OB_HOMIN_H
#define __OB_HOMIN_H

#include <OBULLET.H>

class BulletHoming : public Bullet
{
public:
	char	max_step;
	short	target_base_obj_recno;
	short	speed;
	short origin2_x, origin2_y;

public:
	BulletHoming();
	~BulletHoming();
	void	init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType); // virtual function from obullet.h
	void	deinit();

	void	process_move();
	int	reflect( int baseObjRecno );

	// int	write_derived_file(File *);
	// int	read_derived_file(File *);

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();
};

#endif