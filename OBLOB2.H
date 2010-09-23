// Filename    : OBLOB2.H
// Description : header file of 2D binary block, four side extensible

#ifndef __OBLOB2_H
#define __OBLOB2_H

#include <OBITMAP.H>

// ------- define class Blob2D ----------//

class Blob2D
{
public:
	Bitmap *ptr;
	unsigned int alloc_size;
	short	left_edge;
	short	top_edge;
	short	width;
	short	height;

public:
	Blob2D();
	Blob2D( Blob2D & );
	~Blob2D();

	Blob2D& operator=(Blob2D &);

	void	resize(short left, short top, short w, short h);
	int	extend_to(short x, short y);
	int	extend_to(short x1, short y1, short x2, short y2);

	char *bitmap_ptr()	{ return (char *)ptr; }
	char *buf_ptr()		{ return (char *)ptr->bitmap; }
	int	buf_pitch()		{ return ptr->get_pitch(); }
	unsigned char *p(short x, short y, int autoExtend=1);
	void	fill_area(short x1, short y1, short x2, short y2, unsigned char color,
		int autoExtend=1);
	void	auto_clip(short *pLeft, short *pTop, short *pWidth, short *pHeight, int autoResize=1);

	void	clear();
	int	is_clear()	{ return width == 0 && height == 0; }
};

#endif