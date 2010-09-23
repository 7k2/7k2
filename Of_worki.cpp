// Filename    : OF_WORKI.CPP
// Description : FirmWork interface function


#include <OF_WORK.H>
#include <OMODEID.H>
#include <OINFO.H>

#include <OFIRM.H>
#include <OSTR.H>
#include <KEY.H>
#include <OVGA.H>

#include <OHELP.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OBUTT3D.H>
#include <OBUTTON.H>
#include <OSLIDER.H>
#include <ONATION.H>

#include <ORACERES.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OTOWN.H>
#include <OWORLD.H>
#include <OREMOTE.H>
#include <OIMGRES.H>
#include <OGAME.H>
#include <OSYS.H>
#include <OANLINE.H>
#include <OWORLDMT.H>
#include <OSLIDCUS.H>
#include <OT_FIRM.H>

short pop_disp_y1;

static short edit_need_worker_x1, edit_need_worker_y1, edit_need_worker_x2, edit_need_worker_y2, edit_need_worker_enable;

static SlideBar slide;
//static char
static void disp_slide_bar(SlideBar *slideBar, int);
//int temp_nation_recno;



void FirmWork::put_info(int refreshFlag)
{
//	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MIFABASE") );

	if( !should_show_info() )
		return;
	disp_worker_list(INFO_Y1, refreshFlag);
	disp_firm_info(INFO_Y1, refreshFlag);

	if( selected_worker_id > worker_count )
		selected_worker_id = worker_count;

	if( selected_worker_id )
		disp_worker_info(INFO_Y1, refreshFlag);
}


void FirmWork::detect_info()
{
	detect_firm_info();
	if( !should_show_info() )
		return;
	
	if (is_own())
	{
		detect_worker_list();
		detect_worker_info();
	}
}



void FirmWork::disp_firm_info(int dispY1, int refreshFlag)
{
	// Overwritten by Of_facti.cpp, 
}


void FirmWork::detect_firm_info()
{
	// Overwritten by Of_facti.cpp, 
}


void FirmWork::disp_worker_list(int dispY1, int refreshFlag)
{
	//------ display population composition -------//

	int x = INFO_X1 +20, y = INFO_Y1 +145;

	for( int i=0 ; i< MAX_WORKER ; i++ )
	{
		if ( i < needed_worker_count)
		{
			if( i<worker_count )
			{
				if ( firm_id == FIRM_BASE )
					vga.active_buf->put_bitmap( x, y+4, image_icon.read("PRAY-ACT"));
				else
					vga.active_buf->put_bitmap( x, y, image_icon.read("WRKR-ACT"));

				//## begin trevor 18/11 ##//           
//				if( selected_worker_id == i+1 )
//				{
//					vga.active_buf->rect( x-4, y+2, x+15, y+32, 1, VGA_LIGHT_BLUE);
//				}
				//## end trevor 18/11 ##//
			}
			else
			{	
				if ( firm_id == FIRM_BASE )
					vga.active_buf->put_bitmap( x, y+4, image_icon.read("PRAY-OPN"));
				else
					vga.active_buf->put_bitmap( x, y, image_icon.read("WRKR-OPN"));
			}
		}

		if ( i < slide.view_recno && slide.drag_flag == 1 )
		{
			if ( firm_id == FIRM_BASE )
				vga.active_buf->put_bitmap_trans( x-1, y+6, image_icon.read("PRAY-OUT"));
			else
				vga.active_buf->put_bitmap_trans( x-1, y+2, image_icon.read("WRKR-OUT"));
		}
		
		x += 20;
		if ( i == 9 )
		{
			x = INFO_X1 +20;
			y = INFO_Y1 +178;
		}
	}
	if( refreshFlag == INFO_REPAINT )
	{
		//								1------------------20 
		//				X1 = 34/2 + |<----------------->| + 34/2 = X2
		//							  Y2 - Y1 = button height
		slide.init_slide(INFO_X1+7, INFO_Y1+220, INFO_X1+217, INFO_Y1+251, 34 , disp_slide_bar);
		slide.set(0, MAX_WORKER, needed_worker_count);			
	}

	// do not use slide.paint( needed_worker_count ) 
	// because needed_worker_count is not updated immediately
	slide.paint( slide.view_recno );
}

void FirmWork::detect_worker_list()
{
	int x = INFO_X1 +20, y = INFO_Y1 +145;
	for(int i=0 ; i<worker_count ; i++ )
	{
		switch( mouse.single_click(x, y, x+19, y+32, 2) )
		{
			case 1:         // left button to select worker
				selected_worker_id = i+1;
				break;

			case 2:		
				break;
		}
		x += 20;
		if ( i == 9 )
		{
			x = INFO_X1 +20;
			y = INFO_Y1 +178;
		}
	}

	if( slide.detect() && slide.drag_flag == 0 ) 
	{
		// ###### begin Gilbert 5/2 ######//
		// needed_worker_count = slide.view_recno;
		// set_needed_worker_count (needed_worker_count, COMMAND_PLAYER);
		set_needed_worker_count (slide.view_recno, COMMAND_PLAYER);
		// ###### end Gilbert 5/2 ######//
	}
	help.set_help( INFO_X1+7, INFO_Y1+220, INFO_X1+217, INFO_Y1+251, "SLIDER" );
	help.set_help( INFO_X1, INFO_Y1+143, INFO_X1+223, INFO_Y1+219, "WORKER" );
}

void FirmWork::disp_worker_info(int dispY1, int refreshFlag)
{
}

void FirmWork::detect_worker_info()
{
}

// ---------- begin of static function disp_slide_bar  -----//
static void disp_slide_bar(SlideBar *slideBar, int)
{
	char workerNumberName[] = "SLD-00";
	workerNumberName[4] = '0' + slide.view_recno / 10;
	workerNumberName[5] = '0' + slide.view_recno % 10;

	if (vga.active_buf->is_front)
		mouse.hide_area(INFO_X1, INFO_Y1+213, INFO_X2, INFO_Y1+257 );
	char *nationPict = image_spict.get_ptr(workerNumberName);
	short nationRecno = firm_array[firm_array.selected_recno]->nation_recno;
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1, INFO_Y1+213, nationPict, game.get_color_remap_table(nationRecno, 0) );
	if (vga.active_buf->is_front)
		mouse.show_area();
}
// ---------- end of static function disp_slide_bar  -----//

// --------- begin of function FirmWork::disp_edit_mode -------//
//
void FirmWork::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit hit point

	edit_need_worker_enable = 1;
	x = font_whbl.put( x1, y, text_firm.str_town_worker(), 0, x2 );
	x += 20;
	x = font_whbl.put( x, y, worker_count, 0, x2 );
	x = font_whbl.put( x, y, " / ", 0, x2 );
	x = edit_need_worker_x2 = font_blue.put( (edit_need_worker_x1=x), (edit_need_worker_y1=y), needed_worker_count, 0, x2 );
	edit_need_worker_y2 = edit_need_worker_y1+font_blue.max_font_height;
	y += 18;
}
// --------- end of function FirmWork::disp_edit_mode -------//


// --------- begin of function FirmWork::detect_edit_mode -------//
//
int FirmWork::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	if( edit_need_worker_enable )
	{
		if( mouse.any_click(edit_need_worker_x1, edit_need_worker_y1, edit_need_worker_x2, edit_need_worker_y2, LEFT_BUTTON) )
		{
			int newNeededWorker = needed_worker_count+1;
			if( newNeededWorker > MAX_WORKER )
				newNeededWorker = MAX_WORKER;
			set_needed_worker_count( newNeededWorker, COMMAND_PLAYER );
			slide.view_recno = newNeededWorker;	 // update slider
			return 1;
		}
		if( mouse.any_click(edit_need_worker_x1, edit_need_worker_y1, edit_need_worker_x2, edit_need_worker_y2, RIGHT_BUTTON) )
		{
			int newNeededWorker = needed_worker_count-1;
			if( newNeededWorker < 0 )
				newNeededWorker = 0;
			set_needed_worker_count( newNeededWorker, COMMAND_PLAYER );
			slide.view_recno = newNeededWorker;	 // update slider
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmWork::detect_edit_mode -------//
