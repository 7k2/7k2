// Filename    : OINGMENU.H
// Description : in-game menu (async version)


#ifndef __OINGMENU_H
#define __OINGMENU_H

class InGameMenu
{
public:
	enum { GAME_OPTION_COUNT = 11 };

	int	active_flag;
	int	refresh_flag;

	char game_menu_option_flag[GAME_OPTION_COUNT];
	static unsigned menu_hot_key[GAME_OPTION_COUNT];

	short*	color_remap_table;
	char*		background_bitmap;

public:
	InGameMenu();
	~InGameMenu();

	int	is_active()		{ return active_flag; }
	void	set_active();		// set active_flag and load color_remap_table, background_bitmap
	void	clear_active();	// clear active_flag and dispose color_remap_table, background_bitmap

	void	enter(char untilExitFlag);
	void	disp(int needRepaint=0);
	int	detect();
	void	exit(int action);
	void	abort();
};

extern InGameMenu in_game_menu;

#endif
