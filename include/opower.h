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

//Filename    : OPOWER.H
//Description : Header file of Object Power

#ifndef __OPOWER_H
#define __OPOWER_H

#include <cstddef>

//-------- Define command id. -----------//

enum { 
		 COMMAND_UNIT_FIRST=1,

		 COMMAND_BUILD_FIRM = COMMAND_UNIT_FIRST,
		 COMMAND_ASSIGN,
		 COMMAND_BURN,
		 COMMAND_SETTLE,
		 COMMAND_SET_CARAVAN_STOP,
		 COMMAND_GOD_CAST_POWER,
		 COMMAND_TRANSFORM_FORTRESS,
		 COMMAND_USE_ITEM_ON_UNIT,
		 COMMAND_USE_ITEM_ON_TOWN,
		 COMMAND_USE_ITEM_ON_SPACE,

		 COMMAND_UNIT_LAST,
		 COMMAND_FIRM_FIRST = COMMAND_UNIT_LAST,

		 COMMAND_FIRM_MAGIC = COMMAND_FIRM_FIRST,
		 COMMAND_MAGIC_TELEPORT_PRE,
		 COMMAND_MAGIC_TELEPORT_POST,

		 COMMAND_FIRM_LAST,
	  };

// --------- define ScreenObjectType --------//

enum ScreenObjectType
{
	SCREEN_OBJECT_NONE,
	SCREEN_OBJECT_FRIEND_UNIT,
	SCREEN_OBJECT_UNIT_GROUP,
	SCREEN_OBJECT_ENEMY_UNIT,
	SCREEN_OBJECT_ALLIED_UNIT,
	SCREEN_OBJECT_SPY_UNIT,		// our spy, stealthed to other nation, selected object only
	SCREEN_OBJECT_SPY_GROUP,		// our spy, stealthed to other nation, selected object only
	SCREEN_OBJECT_FRIEND_TOWN,
	SCREEN_OBJECT_ENEMY_TOWN,
	SCREEN_OBJECT_ALLIED_TOWN,
	SCREEN_OBJECT_FRIEND_FIRM,
	SCREEN_OBJECT_ENEMY_FIRM,
	SCREEN_OBJECT_ALLIED_FIRM,
	SCREEN_OBJECT_WALL,
	SCREEN_OBJECT_SITE,
};

//--------- Define class Power ----------//

struct Location;

class File;

#pragma pack(1)
class Power
{
public:
	int   		command_id;
	int   		command_unit_recno, command_firm_recno;
	int			command_para;

	char			win_opened;
	char			enable_flag;

	short			locked_unit_recno;		// affect test_detect

public:
	Power();
	~Power();

	void			init();

	void 			enable()		{ enable_flag=1; locked_unit_recno = 0; }
	void 			disable()	{ enable_flag=0; }

	void			issue_command(int,int=0,int=0);
	void			reset_command()			{ command_id=0; }

	void			mouse_handler();
	void  		reset_selection();

	char*			get_link_icon(char linkStatus, int sameNation);

	int 			write_file(File* filePtr);
	int			read_file(File* filePtr);

	//------- cursor related functions ------//

	int			choose_cursor(int scrnX, int scrnY,
						ScreenObjectType selectedObjectType, short selectedObjectRecno,
						ScreenObjectType pointingObjectType, short pointingObjectRecno);

	int			choose_cursor_units(short selectedUnitRecno, short pointingUnitRecno);
	int			choose_cursor_unit_group(short pointingUnitRecno);
	int			choose_cursor_spy_group(short pointingUnitRecno);

	ScreenObjectType	find_selected_type( short *);
	ScreenObjectType	find_pointing_type( Location *, short *);

public:
	int  			detect_frame();
	int 			detect_action();
	Location*   test_detect(int curX, int curY, char *mobileType=NULL, int *retXLoc=NULL, int *retYLoc=NULL);
	int 		 	detect_select(int selX1, int selY1, int selX2, int selY2, int recallGroup, int shiftSelect);
	int			execute_command(int mouseX, int mouseY);
};
#pragma pack()

extern Power power;

//---------------------------------------//

#endif
