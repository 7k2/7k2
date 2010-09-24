// Filename    : OW_TRV.H
// Description : header file of world traverse, find units in range


#ifndef __OW_TRV_H
#define __OW_TRV_H

class BaseObj;

struct ScannedObj
{
	short unit_recno;
	short	sort_weight;
};

class WorldScanTraverser
{
	enum { DEFAULT_ARRAY_SIZE = 64 };

public:
	ScannedObj *unit_recno_array;
	int	traverse_count;		// run from 0 to scan_count-1
	int	scan_count;
	int	alloc_count;			// allocated size

private:
	ScannedObj default_recno_array[DEFAULT_ARRAY_SIZE];

public:
	WorldScanTraverser();
	~WorldScanTraverser();

	void	init( int xLoc, int yLoc, int locSize, int radius );
	int	is_finish();
	int	get_unit();
	void	next();

	// -------- for 2nd pass filtering ----------//

	void	restart();
	void	remove();
	void	pack_removed();

	// -------- 3rd pass setting weight function ---------//

	short	get_weight();
	void	set_weight(short);
	
	// -------- 4rd pass sorting ----------//

	void	sort_by_distance( int xLoc, int yLoc, int locSize );
	void	sort_by_distance( BaseObj *);
	void	sort_by_weight();

private:
	void	append(short unitRecno);
};

#endif
