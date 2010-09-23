// Filename    : OBITMAP.H
// Description : header file of bitmap class

#ifndef __OBITMAP_H
#define __OBITMAP_H

class Bitmap
{
public:
	short width_field;
	short height_field;
	unsigned char bitmap[1];		// indefinite size

private:
	Bitmap();				// cannot make any instant

public:
	void	init(short w, short h);

	short get_width() { return width_field; }
	short get_height() { return height_field; }
	short get_pitch();
	short get_true_pitch();
	unsigned char *get_ptr() { return bitmap; }
	unsigned char *get_ptr(short x, short y);

	long	size();

	static Bitmap *mem_add_bitmap(short w, short h);		// same parameter as init
	static long size(short w, short h);		// same parameter as init
};

#endif