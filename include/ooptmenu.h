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

// Filename    : OOPTMENU.H
// Description : in-game option menu (async version)


#ifndef __OOPTMENU_H
#define __OOPTMENU_H


#include <oconfig.h>
#include <obuttcus.h>
#include <obutton.h>
#include <oslidcus.h>
#include <gamedef.h>

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