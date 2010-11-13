/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename    : OSPIN_S.H
// Description : header file of small spinner
 
#ifndef __OSPIN_S_H
#define __OSPIN_S_H


#include <OBUTT3D.H>
#include <OVBROWIF.H>

//----------- Define variable type -----------//

// as same as VBrowse's
typedef void (*SpinDispFP)(int recNo,int x,int y, int x2, int y2, int refreshFlag); // user defined function to be called

// -------- define class BrowDispFp ----------//

class SpinnerSmall
{
public:
	int	rec_no;
	int	min_rec_num;
	int	total_rec_num;
	int	none_record;
	SpinDispFP disp_rec_func;

	int	ox1, oy1, ox2, oy2;
	int	ix1, iy1, ix2, iy2;

	Button3D button_up, button_down, button_vbrowse;

	int	vbrowse_enable_flag;
	int	vbrowse_height;
	VBrowseIF vbrowse;

public:
	SpinnerSmall();
	~SpinnerSmall();

	void	create(int x1, int y1, int x2, int y2, int vbrowseHeight, int minRec, int recCount, SpinDispFP funcPtr, int initRecNo);
	void	paint(int x1, int y1, int x2, int y2, int vbrowseHeight, int minRec, int recCount, SpinDispFP funcPtr, int initRecNo)
			{ create(x1, y1, x2, y2, vbrowseHeight, minRec, recCount, funcPtr, initRecNo); paint(); } 

	int   recno()	    	 { return rec_no; }
	int   total_rec()	    { return none_record ? 0 : total_rec_num; }

	void  open(int recNo=1, int newTotalRec= -1);
	void	paint(int =-1);
	int	detect(int noPaint=0);
};

#endif


