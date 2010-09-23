// Filename    : OBITMAP.CPP

#include <OBITMAP.H>
#include <ALL.H>


// ------ begin of function Bitmap::init ------//

void Bitmap::init(short w, short h)
{
	width_field = w;
	height_field = h;
}
// ------ end of function Bitmap::init ------//


// ------ begin of function Bitmap::get_pitch ------//

short Bitmap::get_pitch()
{
	return width_field;
}
// ------ end of function Bitmap::get_pitch ------//


// ------ begin of function Bitmap::get_true_pitch ------//

short Bitmap::get_true_pitch()
{
	return width_field;
}
// ------ end of function Bitmap::get_true_pitch ------//


// ------ begin of function Bitmap::get_ptr -------//
unsigned char *Bitmap::get_ptr(short x, short y)
{
	return bitmap + y * width_field + x;
}
// ------ end of function Bitmap::get_ptr -------//

// ------ begin of function Bitmap::size ------//

long Bitmap::size()
{
	return sizeof(width_field)+sizeof(height_field)+ width_field*height_field*sizeof(*bitmap);
}
// ------ end of function Bitmap::size ------//


// ------ begin of function Bitmap::size ------//

long Bitmap::size(short w, short h)
{
	return sizeof(short)+sizeof(short)+ w*h;
}
// ------ end of function Bitmap::size ------//


// ------ begin of function Bitmap::mem_add_bitmap ------//

Bitmap *Bitmap::mem_add_bitmap(short w, short h)
{
	Bitmap *bitmap = (Bitmap *)mem_add(size(w,h));
	bitmap->init(w,h);
	return bitmap;
}
// ------ end of function Bitmap::mem_add_bitmap ------//
