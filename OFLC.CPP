// Filename    : OFLC.CPP
// Description : class Flc (animation format)

#include <OFLC.H>
#include <ALL.H>

extern "C"
{
	#include <TOPFLC.H>
	#include <TFLIB.H>
	#include <TFANIMAT.H>
}
//#include <mem.h>


// ------------ begin of function Flc::Flc -------- //
Flc::Flc()
{
	init_flag = 0;
}
// ------------ end of function Flc::Flc -------- //


// ------------ begin of function Flc::~Flc -------- //
Flc::~Flc()
{
	deinit();
}
// ------------ end of function Flc::~Flc -------- //


// ------------ begin of function Flc::open_file -------- //
//
// open a flc file for reading
//
// return TRUE on success
//
int Flc::open_file(char *filename)
{
	deinit();

	// --------- open flc file -------//
	if((flc_handle = TFAnimation_NewFile(filename)) == NULL)
		return 0;

	// --------- allocate frame buffer and palette buffer -------//
	if( TFAnimation_GetInfo(flc_handle, &flc_info) != TF_SUCCESS )
	{
		TFAnimation_Delete(flc_handle);
		return 0;
	}

	frame_buffer = (unsigned char *) mem_add(width() * height());
	palette_buffer = (unsigned char *) mem_add( 256*3 );
	TFBuffers_Set( flc_handle, frame_buffer, palette_buffer);

	init_flag = 1;
   return 1;
}
// ------------ end of function Flc::open_file -------- //


// ------------ begin of function Flc::open_mem -------- //
//
// open, but flc data is already loaded into flcBuffer
//
// return TRUE on success
//
int Flc::open_mem(void *flcBuffer)
{
	deinit();

	// --------- open flc file -------//
	if((flc_handle = TFAnimation_NewMem(flcBuffer)) == NULL)
		return 0;

	// --------- allocate frame buffer and palette buffer -------//
	if( TFAnimation_GetInfo(flc_handle, &flc_info) != TF_SUCCESS )
	{
		TFAnimation_Delete(flc_handle);
		return 0;
	}

	frame_buffer = (unsigned char *) mem_add(width() * height());
	palette_buffer = (unsigned char *) mem_add( 256*3 );
	TFBuffers_Set( flc_handle, frame_buffer, palette_buffer);

	init_flag = 1;
   return 1;
}
// ------------ end of function Flc::open_mem -------- //


// ------------ begin of function Flc::deinit -------- //
void Flc::deinit()
{
	if(init_flag)
	{
		TFAnimation_Delete(flc_handle);
		mem_del(frame_buffer);
		mem_del(palette_buffer);
		init_flag = 0;
	}
}
// ------------ end of function Flc::deinit -------- //

// ------------ begin of function Flc::set_error_handler -------- //
void Flc::set_error_handler(void (*handler)(char *msg) )
{
	TFErrorHandler_Set(handler);
}

// ------------ begin of function Flc::advance -------- //
int Flc::advance()
{
	return ( TFFrame_Decode(flc_handle) == TF_SUCCESS);
}
// ------------ end of function Flc::advance -------- //


// ------------ begin of function Flc::rewind -------- //
int Flc::rewind()
{
	return ( TFFrame_Reset(flc_handle) == TF_SUCCESS);
}
// ------------ end of function Flc::rewind -------- //


// ------------ begin of function Flc::get_area -------- //
//
// copy area (x1,y1),(x2,y2) to destination
//
void Flc::get_area(void *dest, short x1, short y1, short x2, short y2)
{
	// assume the x1,y1,x2,y2 are within flc width/height
	if( x1 > x2)
	{
		short tmp = x1;
		x1 = x2; x2 = tmp;
	}

	if( y1 > y2)
	{
		short tmp = y1;
		y1 = y2; y2 = tmp;
	}

	int destWidth = x2-x1+1;
	for(int y = y1; y <= y2; ++y)
	{
		memcpy( dest, frame_buffer+y*width()+x1, destWidth);
		dest = (unsigned char *)dest + destWidth;
	}
}
// ------------ end of function Flc::get_area -------- //



