//Filename    : OVBROWIF.H
//Description : Object List Box

#ifndef __OVBROWIF_H
#define __OVBROWIF_H

#ifndef __OVBROWSE_H
#include <OVBROWSE.H>
#endif

//----------- Define variable type -----------//

typedef void (*BrowDispFP)(int recNo,int x,int y,int refreshFlag); // user defined function to be called

//---------- Define class ListBox ------------//

class VBrowseIF : public VBrowse
{
public:
	char	vga_front_only;
	char	disp_frame_rect;

public:
	VBrowseIF();

	void  init(int,int,int,int,int,int,int,BrowDispFP,int=1,int=MIN_INTER_SPACE,int=1);
	void  set_vga_front_only()		{ vga_front_only=1; }
	void	init_var(int totalRec, int recNo);
	void  refresh(int= -1,int= -1);
	void  paint();

protected:
	void	disp_all();
	void	disp_one(int recNo, int dispType);
	void	disp_rec(int,int,int,int);
};

//--------------------------------------------------//


class VBrowseIF2 : public VBrowse
{
public:
	char	vga_front_only;

public:
	VBrowseIF2();

	void  init(int,int,int,int,int,int,int,BrowDispFP,int=1,int=MIN_INTER_SPACE);
	void  set_vga_front_only()		{ vga_front_only=1; }
	void	init_var(int totalRec, int recNo);
	void  refresh(int= -1,int= -1);
	void  paint();
	void  close();
	int detect();
	void  update(int);

protected:
	void	disp_all();
	void	disp_one(int recNo, int dispType);
	void	disp_rec(int,int,int,int);
};

//--------------------------------------------------//

#endif
