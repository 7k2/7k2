// Filename    : OSPIN_S.CPP
// Description : Small spinner

#include <OSPIN_S.H>
#include <ALL.H> 
#include <OBITMAP.H>
#include <OVGA.H>
#include <OVGABUF.H>
#include <OIMGRES.H>
#include <OVBROWIF.H>
#include <OMOUSE.H>

#define SCROLL_BUTTON_WIDTH 8
#define D3_PANEL_THICK 4

static void disp_vbrowse(int recNo,int x,int y,int refreshFlag);
static SpinDispFP disp_vbrowse_func;
static vbrowse_x2;

SpinnerSmall::SpinnerSmall()
{
	disp_rec_func = NULL;
	none_record = 1;
	vbrowse_enable_flag = 0;
}


SpinnerSmall::~SpinnerSmall()
{
	disp_rec_func = NULL;
	none_record = 1;
	vbrowse_enable_flag = 0;
}

void SpinnerSmall::create(int x1, int y1, int x2, int y2, int vbrowseHeight, int minRec, int recCount, SpinDispFP funcPtr, int recNo)
{
	min_rec_num = minRec;
	err_when( minRec < 0 );			// usually minRec is 0 or 1
	
	total_rec_num = recCount;
	none_record = (recCount == 0);
	rec_no = none_record ? 0 : recNo;
	disp_rec_func = funcPtr;

	ox1 = x1;
	oy1 = y1;
	ox2 = x2;
	oy2 = y2;

	ix1 = x1 + D3_PANEL_THICK;
	iy1 = y1 + D3_PANEL_THICK;
	ix2 = x2 - D3_PANEL_THICK - SCROLL_BUTTON_WIDTH;
	iy2 = y2 - D3_PANEL_THICK;

	int buttonHeight = ((Bitmap *)image_button.read("TRI-D-DW"))->get_height();

	button_up.create(ox2-SCROLL_BUTTON_WIDTH+1, oy1+2, "TRI-U-UP", "TRI-U-DW");
	button_down.create(ox2-SCROLL_BUTTON_WIDTH+1, oy2-buttonHeight+1-2, "TRI-D-UP", "TRI-D-DW");
	button_vbrowse.create(ox2+1, oy1+2, "VBROW-UP", "VBROW-DW");

	disp_vbrowse_func = disp_rec_func;
	vbrowse_height = vbrowseHeight;
}

// newTotalRecno     : set new total rec no (-1 = unchange)
void SpinnerSmall::open(int recNo, int newTotalRec)
{
	if( newTotalRec >= 0 )
	{
		total_rec_num = newTotalRec;
		none_record = (newTotalRec == 0);
	}
	rec_no = none_record ? 0 : recNo;
}

// recNo , new selected record
void SpinnerSmall::paint(int recNo)
{
	if( recNo >= min_rec_num )
		rec_no = recNo;

	vga.active_buf->d3_panel_down(ox1, oy1, ox2-SCROLL_BUTTON_WIDTH, oy2, 
		D3_PANEL_THICK, 1 );

	// display record

	if( !none_record && rec_no >= min_rec_num && disp_rec_func)
	{
		(*disp_rec_func)(recNo, ix1, iy1, ix2, iy2, 0 );
	}

	// paint scorll button

	button_up.paint();
	button_down.paint();
	button_vbrowse.paint();
	
	if (vbrowse_enable_flag)
	{
		vbrowse_x2 = vbrowse.ix2;
		vbrowse.paint();
		vbrowse.refresh();
	}
}


// return -1 for no action, or new rec_no
int SpinnerSmall::detect(int noPaint)
{
	if( !none_record && disp_rec_func )
	{
		if( button_up.detect() )
		{
			vbrowse_enable_flag = 0;
			if( rec_no > min_rec_num )
			{
				if( !noPaint )
					paint(rec_no-1);
				else
					--rec_no;
				return rec_no;
			}
		}
		else if( button_down.detect() )
		{
			vbrowse_enable_flag = 0;
			if ( rec_no < min_rec_num+total_rec_num-1 )
			{
				if( !noPaint )
					paint(rec_no+1);
				else
					++rec_no;
				return rec_no;
			}
		}
		
		if (button_vbrowse.detect() || 
			mouse.single_click( ix1, iy1, ix2, iy2 ))
		{
			vbrowse_enable_flag =! vbrowse_enable_flag;
			if (vbrowse_enable_flag)
			{
				vbrowse.init( ox1, oy2, ox2 -SCROLL_BUTTON_WIDTH +SCROLL_BAR_WIDTH,
					oy2 +vbrowse_height, -1, 20, total_rec_num, disp_vbrowse );
				vbrowse.open(rec_no+1);
			}
		}

		if (vbrowse_enable_flag)
		{
			if (vbrowse.detect())
			{
				if (vbrowse.double_click)
					vbrowse_enable_flag = 0;

				return (rec_no = (vbrowse.recno() -1));
			}
		}
	}

	return -1;
}

// ----- begin of static function disp_vbrowse ------//
static void disp_vbrowse(int recNo,int x,int y,int refreshFlag)
{
	(*disp_vbrowse_func)(recNo-1, x, y, vbrowse_x2, y +20, 0 );
}
// ----- end of static function disp_vbrowse ------//