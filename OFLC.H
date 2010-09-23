// Filename    : OFLC.H
// Description : Header file of class FLC (animation format)


#ifndef __OFLC_H
#define __OFLC_H

extern "C"
{
	#include <TFANIMAT.H>
}

class Flc
{
public:
	TFAnimation*		flc_handle;
	unsigned char*		frame_buffer;
	unsigned char*		palette_buffer;
	TFAnimationInfo	flc_info;
	int					init_flag;

public:
	Flc();
	~Flc();

	int	open_file(char *filename);
	int	open_mem(void *flcBuffer);
	void	deinit();

	static void set_error_handler( void (*handler)(char *msg) );

	// -------- get info function -------//
	short	width()		{ return flc_info.Width; }
	short	height()		{ return flc_info.Height; }
	short max_frame() { return flc_info.NumFrames; }
	short cur_frame() { TFAnimation_GetInfo(flc_handle, &flc_info); return flc_info.CurFrame; }
	unsigned char *get_palette() {TFAnimation_GetInfo(flc_handle, &flc_info); return (unsigned char *)flc_info.Palette; }

	int	advance();		// advance to next frame
	int	rewind();		// rewind to the beginning
	void	get_area(void *, short x1, short y1, short x2, short y2);
};
#endif