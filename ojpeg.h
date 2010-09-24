// Filename    : OJPEG.H
// Description : ojpeg decompress class

#ifndef __OJPEG_H
#define __OJPEG_H

// problem with jpeglib.h : FILE not defined c/cpp includes jpeglib.h includes stdio.h first

class VgaBuf;
struct jpeg_error_mgr;
struct jpeg_decompress_struct;


class Jpeg
{
private:
	jpeg_decompress_struct* cinfo;
	jpeg_error_mgr*         jerr;
	int	error_flag;

public:
	Jpeg();
	~Jpeg();

	int put_to_buf( VgaBuf *vgaBuf, int x, int y, char *filename );
};

extern Jpeg jpeg;

#endif