// Filename    : OBITMAPW.H
// Description : header file of bitmap class

#ifndef __OBITMAPW_H
#define __OBITMAPW_H

class BitmapW
{
public:
	short width_field;
	short height_field;
	short bitmap[1];		// indefinite size

private:
	BitmapW();				// cannot make any instant

public:
	void	init(short w, short h);

	short get_width() { return width_field; }
	short get_height() { return height_field; }
	short get_pitch();
	short	get_true_pitch();

	short *get_ptr() { return bitmap; }
	short *get_ptr(short x, short y);

	long	size();

	static BitmapW *mem_add_bitmap(short w, short h);		// same parameter as init
	static long size(short w, short h);		// same parameter as init
};

#endif