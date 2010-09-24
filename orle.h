// Filename    : ORLE.H
// Description : run-lenght compress

#ifndef __ORLE_H
#define __ORLE_H


class File;

class Rle
{
public:
	Rle();
	~Rle();

	// nul output, to find output size
	long compress( unsigned char *inPtr, long inByteLen, unsigned char rleKey = 0xff );

	// compressed data in memory
	long compress( unsigned char *inPtr, long inByteLen, unsigned char *outPtr, unsigned char rleKey = 0xff );
	long expand( unsigned char *inPtr, unsigned char *outPtr, unsigned char rleKey = 0xff );

	// compressed data in file
	long compress( unsigned char *inPtr, long inByteLen, File *outFile, unsigned char rleKey = 0xff );
	long expand( File *inFile, unsigned char *outPtr, unsigned char rleKey = 0xff );
};


#endif
