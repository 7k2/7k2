// Filename    : OEXPMASK.H
// Description : Header file of explored area mask
// Owner       : Gilbert

#ifndef __OEXPMASK_H
#define __OEXPMASK_H

struct Location;
class ColorTable;

typedef unsigned char BYTE;

#define FULL_DARK_MASK_ID 0x00
#define FULL_BRIGHT_MASK_ID 0xaa
#define HALF_BRIGHT_MASK_ID 0x55

class ExploredMask
{
public:
	char *remap_bitmap;

public:
	void init();
	void deinit();

	char *get_remap_mask( BYTE maskId );

	static BYTE make_mask_id( BYTE nwVisitLevel, BYTE nVisitLevel, BYTE neVisitLevel,
		BYTE wVisitLevel, BYTE cVisitLevel, BYTE eVisitLevel,
		BYTE swVisitLevel, BYTE sVisitLevel, BYTE seVisitLevel );

	static BYTE make_mask_id( int xLoc, int yLoc );

private:
	static BYTE mask_id_to_serial_id( BYTE maskId );
};

extern ExploredMask explored_mask;

#endif