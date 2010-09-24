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

//Filename    : OVBROWIF2.CPP
//Description : Object Visual Browse for the Interface

#include <ALL.H>
#include <OSYS.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OVBROWIF.H>
#include <OMOUSE.H>
#include <OMODEID.H>
#include <OIMGRES.H>
//#include <OFONT.H>
//#include <OPOWER.H>
//#include <OVBROWSE.H>


//--------- Begin of function VBrowseIF2::init --------//
void VBrowseIF2::init(int inX1, int inY1, int inX2, int inY2,
						 int recWidth, int recHeight,
						 int totalRec, BrowDispFP dispFunc,
						 int dispFrame, int minInterSpace)
{
	VBrowse::init(inX1, inY1, inX2, inY2, 1,
						 recWidth, recHeight,
						 totalRec, dispFunc,
						 dispFrame, minInterSpace);
}
//--------- End of function VBrowseIF2::init --------//

//---------- Begin of function VBrowseIF::VBrowseIF -------//
//
VBrowseIF2::VBrowseIF2()
{
	vga_front_only = 0;  // if 1, then it does all the bitmap processing on the front buffer only
}
//----------- End of function VBrowseIF::VBrowseIF ----------//


//---------- Begin of function VBrowseIF::init_var -------//
//
void VBrowseIF2::init_var(int totalRec, int recNo)
{
	VBrowse::init_var(totalRec, recNo);
	scroll_bar.init( 1, x1+3, y1-19, x1+34, y2+2, disp_max_rec,
		 (disp_frame && x_max_rec==1 ? 1 : disp_max_rec),    // only use record as scroller unit when it's a vertical browser with frame
		 total_rec_num, 1, 1 );		// last 1 - Interface mode if_flag is 1
}
//----------- End of function VBrowseIF::init_var ----------//

//---------- Begin of function VBrowseIF::close -------//
void VBrowseIF2::close()
{
	state = 0;   // state = CLOSED
}
//----------- End of function VBrowseIF::close ----------//

//--------- Begin of function VBrowseIF::paint ---------//
//
void VBrowseIF2::paint()
{	
	disp_all();
}
//--------- End of function VBrowseIF::paint ---------//

//------- Begin of function VBrowse::update ---------//
//
// Yield all records which currently display in the vbrowse
//
void VBrowseIF2::update(int totalRec)
{
	// ###### begin Gilbert 17/6 #########//
	if (totalRec != total_rec_num)
	{
		open( -1, totalRec );
	}
	else
	{
		VBrowse::update();
	}
	// ###### end Gilbert 17/6 #########//
}
//------- End of function VBrowse::update ---------//


//--------- Begin of function VBrowseIF::refresh ---------//
//
// Reset the no. of record and current record pointer and cause displaying
//
// [int] newRecNo      = new current record no.
//                       (default : -1, no change)
// [int] newTotalRec   = new total no. of records
//                       (default : -1, no change)
//
void VBrowseIF2::refresh(int newRecNo, int newTotalRec)
{
	if( !vga_front_only && !vga.use_back_buf )
		vga.blt_buf( x1, y1, x2, y2, 0 );  // the list content box

	VBrowse::refresh(newRecNo, newTotalRec);
}
//--------- End of function VBrowseIF::refresh ---------//

//--------- Begin of function VBrowse::detect ------//
//
// Detect for any mouse action and carry out relative response
//
// Return : <int> >0 if the new current record number
//                =0 if no action
//
int VBrowseIF2::detect()
{
	int rc;
   int recNo=0;

   double_click = 0;

   if( state == 0 || none_record )             // closed
      return 0;

   //-- Detect pulling on record if the browser is with frame --//
	
//	if( detect_pull_flag && disp_frame && x_max_rec==1 )  // only pull when the vbrowser is vertical, horizontal is technically possible, but not used practically
//	{
//		if( detect_pull() )
//			return rec_no;
//	}

	//--------- Detect pressing on record if disp_frame is 1 ---------//

	if( disp_frame )
	{
		if( mouse.any_click(x1+5, y1-6, x2-20, y2-14) )
		{
			recNo = top_rec_no +
					  (mouse.click_y()-(y1-6)) / (rec_height+rec_y_space) * x_max_rec +
					  (mouse.click_x()-(x1+5)) / (rec_width +rec_x_space);

			if( detect_pull_flag )
				press_record = 1;

			press_vbrowse_ptr = this;
		}

		if( mouse.press_area(x1+5, y1-6, x2-20, y2-14) )
		{
			recNo = top_rec_no +
				(mouse.cur_y-(y1-6)) / (rec_height+rec_y_space) * x_max_rec +
				(mouse.cur_x-(x1+5)) / (rec_width +rec_x_space);

			if( detect_pull_flag )
				press_record = 1;

			press_vbrowse_ptr = this;
		}

		if( recNo>0 && recNo<=top_rec_no-1+disp_max_rec )   // if it is not in the empty browser area
		{
			if( mouse.click_count()==2 )   // double clicked
				double_click = 1;

			if( recNo != rec_no )            // if user point to a new record
			{
				disp_one(rec_no, CLEAR_HIGH);
				disp_one(recNo , DISP_HIGH );    // 2 means display record content only

				rec_no = recNo;
				return rec_no;
			}

			if( double_click )     // even if current record no. doesn't change, return recno if double clicked
				return rec_no;
		}
	}
	
	if( !mouse.any_click(x1+5, y1-6, x2-20, y2-14) )
	{
	if( ( rc=scroll_bar.detect() ) > 0 )
	{
		disp_one(rec_no,CLEAR_HIGH);
		rec_no = rc;

		if( disp_frame && x_max_rec==1 ) // only use record as scroller unit when it's a vertical browser with frame, refer to scroll_bar.init() in VBrowse::init_var()
		{
			if( rec_no < top_rec_no )
			{
				top_rec_no = rec_no;
				disp_all();
			}

			else if( rec_no >= top_rec_no + disp_max_rec )
			{
				top_rec_no = rec_no - disp_max_rec + 1;
				if( top_rec_no < 1 )
					top_rec_no = 1;
				disp_all();
			}

			else
				disp_one(rec_no,DISP_HIGH);
		}
		else
		{
			top_rec_no = rc;
			disp_all();
		}

		return rec_no;
	}
	}
	return 0;
}
//----------- End of function VBrowse::detect -------//

//-------- Begin of function VBrowseIF::disp_all ----------//
//
// Display all records on screen, highlight cur_rec_no
//
void VBrowseIF2::disp_all()
{
	int recNo;

	if( !vga_front_only && !vga.use_back_buf )
		vga.blt_buf( ix1, iy1, ix2, iy2, 0 ); // clear background

	Vga::active_buf->put_bitmap_trans_decompress( x1+2, y1-20, image_button.read("ARROWUP") );
	Vga::active_buf->put_bitmap_trans_decompress( x1+2, y2-11, image_button.read("ARROWDN") );
	
//	int scrollRecno = (disp_frame && x_max_rec==1) ? rec_no : top_rec_no;

	int scrollRecno = rec_no;
   scroll_bar.update( scrollRecno );
		
	for( recNo=top_rec_no ; recNo<=total_rec_num && recNo<top_rec_no+disp_max_rec ; recNo++ )
	{
		disp_one( recNo, DISP_REC );

		if( recNo == rec_no )
			disp_one( recNo, DISP_HIGH );
	}
}
//--------- End of function VBrowseIF::disp_all ----------//


//-------- Begin of function VBrowseIF::disp_one ----------//
//
// <int> recNo     = the no. of the record to display
// <int> putType   = DISP_REC  - display record
//                   DISP_HIGH - highlight rect
//                   CLEAR_HIGH- clear highlight rect
//
void VBrowseIF2::disp_one(int recNo, int dispType)
{
	if( none_record )
		return;

	int x,y;

	y = iy1 + (recNo-top_rec_no)/x_max_rec * (rec_height+rec_y_space);
	x = ix1 + (recNo-top_rec_no)%x_max_rec * (rec_width+rec_x_space);

	//---- put a outline rect around the record if it is highlight ---//

	if( disp_frame && dispType == CLEAR_HIGH && !vga.use_back_buf && !vga_front_only )
	{
		vga.blt_buf( x-2, y-2			  , x+rec_width+1, y-2				, 0 );	// top
		vga.blt_buf( x-2, y+rec_height+1, x+rec_width+1, y+rec_height+1, 0 );	// bottom
		vga.blt_buf( x-2, y-2			  , x-2		     , y+rec_height+1, 0 );	// left
		vga.blt_buf( x+rec_width+1, y-2 , x+rec_width+1, y+rec_height+1, 0 );	// right
	}

	if( dispType == DISP_REC )
		disp_rec( recNo, x, y, INFO_REPAINT );  // call user defined function

	if( disp_frame && dispType == DISP_HIGH )
	{
	//	vga_back.rect( x-2, y-2, x+rec_width+1, y+rec_height+1, 2, VGA_LIGHT_BLUE);
		Vga::active_buf->rect( x-2, y-11, x+rec_width+1, y+rec_height-7, 2, VGA_LIGHT_BLUE);
	}
}
//--------- End of function VBrowseIF::disp_one ----------//


//-------- Begin of function VBrowseIF::disp_rec --------//
//
void VBrowseIF2::disp_rec(int recNo, int x, int y, int refreshFlag)
{
	(*disp_rec_func)( recNo, x, y, refreshFlag );  // call user defined function

	sys.yield();
}
//---------- End of function VBrowseIF::disp_rec -----------//
