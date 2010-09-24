// Filename    : OOPTMENU.H
// Description : in-game option menu (async version)


#ifndef __OOPTMENU_H
#define __OOPTMENU_H


#include <OCONFIG.H>
#include <OBUTTCUS.H>
#include <OBUTTON.H>
#include <OSLIDCUS.H>
#include <GAMEDEF.H>

class OptionMenu
{
public:
	int	active_flag;
	int	refresh_flag;
	int	update_flag;

	Config	old_config;

	SlideBar se_vol_slide;
//	SlideBar music_vol_slide;
	ButtonCustomGroup music_group;
	SlideBar frame_speed_slide;
	SlideBar scroll_speed_slide;
//	ButtonCustom race_button[MAX_RACE];
	ButtonCustomGroup help_group;
	ButtonCustomGroup news_group;
	ButtonCustomGroup report_group;
	ButtonCustomGroup show_icon_group;
	ButtonCustomGroup show_path_group;
	ButtonCustomGroup inside_info_group;
	ButtonCustomGroup	scroll_method_group;
	Button start_button, cancel_button;

	short*	color_remap_table;
	char*		background_bitmap;

public:
	OptionMenu();
	~OptionMenu();

	int	is_active()		{ return active_flag; }
	void	set_active();		// set active_flag and load color_remap_table, background_bitmap
	void	clear_active();	// clear active_flag and dispose color_remap_table, background_bitmap

	void	enter(char untilExitFlag);
	void	disp(int needRepaint=0);
	int	detect();
	void	exit(int action);
	void	abort();

	static int slide_to_percent_volume(int slideVolume);
	static int percent_to_slide_volume(int percentVolume);
};

extern OptionMenu option_menu;

#endif