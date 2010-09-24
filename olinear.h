// Filename    : OLINEAR.H
// Description : Header file of Linear increment / decrement


#ifndef __OLINEAR_H
#define __OLINEAR_H

class LinearCounter
{
public:
	int	dx;
	int	dy;
	int	x_dest;

	int	x;
	int	y;
	int	d;
	int	q;
	int	r;

public:
	LinearCounter();
	LinearCounter(int x0, int y0, int x1, int y1 );

	void	init(int x0, int y0, int x1, int y1 );
	int	is_end();
	void	inc();
};

#endif