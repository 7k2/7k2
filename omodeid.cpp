// Filename    : OMODEID.H
// Description : DisplayModeInfo


#include <OMODEID.H>
#include <ALL.H>

#define MAX_DISPLAY_MODE_ID 2


static DisplayModeInfo display_info_array[MAX_DISPLAY_MODE_ID] = 
{
	// 800x600x16
	{
		MODE_ID_800x600x16,						// mode id
		800, 600, 16,							// screen_width, height, bbp
		0, 68-12, 575, 598, 576, 531+12,				// zoom_matrix_x1,y1,x2,y2,width,height
		592, 24, 791, 223, 200, 200,			// map_matrix_x1,y1,x2,y2,width,height
		332, 0, 574, 67-12,						// top_menu_x1,y1,x2,y2
		576, 258, 799, 583,						// info_x1,y1,x2,y2
		586, 560, 790, 589,						// msg_info_x1,y1,x2,y2
		716-3, 3, 24, 24,							// map_mode_button_x1, y1, width, height
		588, 6,									// menu_button_x1, y1
		486-30, 24,								// repu_button_x1, y1
		480, 12-2, 105,							// date_x1, y1, length
		378-20, 9+4, 67+100,								// food_x1, y1, length
		376-20, 36+1, 69+100,							// cash_x1, y1, length
		0, 0, 331-19, 66-11,							// scroll_menu_x1, y1, x2, y2 
		577, 584, 799, 599, 								// help_short_x1, y1, x2, y2
		0, 0, 797, 598						// mouse_x1,y1,x2,y2
	},

	// 1024x768x16
	{
		MODE_ID_1024x768x16,					// mode id
		1024,768, 16,							// screen_width, height, bbp
		32, 2, 799, 762, 768, 761,				// zoom_matrix_x1,y1,x2,y2,width,height  {y1 must never 0, 64 must be a factor of y1 - y2}
		816, 193, 1015, 392, 200, 200,			// map_matrix_x1,y1,x2,y2,width,height
	//	810, 120, 1018, 167,					// top_menu_x1,y1,x2,y2
		804-5, 2, 1023, 74,						// top_menu_x1,y1,x2,y2
		800, 426, 1023, 751,					// info_x1,y1,x2,y2
		810, 589-29, 1014, 589,					// msg_info_x1,y1,x2,y2
		937, 171, 24, 24,						// map_mode_button_x1, y1, width, height
		901, 20+60,								// menu_button_x1, y1
		814+155, 165-165,								// repu_button_x1, y1
		875, 150-90, 150,							// date_x1, y1, length
		947-120, 122-95, 72,							// food_x1, y1, length
		848+50, 122-95, 56,							// cash_x1, y1, length
		801-2, 80-5, 1018, 168,						// scroll_menu_x1, y1, x2, y2 
		801, 752, 1023, 767,						// help_short_x1, y1, x2, y2

		32, 2, 1018, 762							// mouse_x1,y1,x2,y2
	},
};

void DisplayModeInfo::init(int modeId)
{
	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
	{
		 *this = display_info_array[modeId];
	}
	else
	{
		err_here();
	}	
}

// return NULL for non-existing display mode
DisplayModeInfo *DisplayModeInfo::get_display_info(int modeId)
{
	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
		return display_info_array + modeId;
	else
		return NULL;
}


void DisplayModeInfo::set_current_display_mode(int modeId)
{
	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
	{
		current_display_mode = display_info_array[modeId];
	}
	else
	{
		err_here();
	}	
}



