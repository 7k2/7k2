// Filename    : OWARPT.H
// Description : warning point on map

#ifndef __OWARPT_H
#define __OWARPT_H

//--------- define constant -----------//

#define WARPOINT_ZONE_SIZE 8
#define WARPOINT_ZONE_COLUMN ((MAX_MAP_WIDTH + WARPOINT_ZONE_SIZE -1) / WARPOINT_ZONE_SIZE)
#define WARPOINT_ZONE_ROW ((MAX_MAP_HEIGHT + WARPOINT_ZONE_SIZE -1) / WARPOINT_ZONE_SIZE)

//--------- struct Define WarPoint ----------//

typedef unsigned long DWORD;

struct WarPoint
{
	DWORD strength;

	void	inc();
	void	decay();
};

//------- define class WarPointArray --------//

class WarPointArray
{
public:
	WarPoint *war_point;
	int		active_zone_count;
	char		init_flag;
	char		draw_phase;

	WarPointArray();
	~WarPointArray();

	void	init();
	void	deinit();

	void	draw_dot();
	void	process();

	WarPoint *get_ptr(int xLoc, int yLoc);
	void	add_point(int xLoc, int yLoc);
};

extern WarPointArray war_point_array;

#endif