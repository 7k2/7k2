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

//Filename    : KEY.H
//Description : Header file for Object Keyboard input handler

#ifndef __KEY_H
#define __KEY_H

//---------------------------------------------//

enum 
{
	K_CAPITAL_LETTER      = 0x0001,
	K_IS_SHIFT            = 0x0002,
	K_IS_CTRL             = 0x0004,
	K_IS_ALT              = 0x0008,
	K_ON_NUMPAD           = 0x0010,

	K_CASE_INSENSITIVE    = 0x0100,
	K_IGNORE_SHIFT        = 0x0200,
	K_IGNORE_CTRL         = 0x0400,
	K_IGNORE_ALT          = 0x0800,
	K_IGNORE_NUM_LOCK     = 0x1000,
	K_IGNORE_CAP_LOCK     = 0x2000,
	K_IGNORE_NUMPAD       = 0x4000,
	K_TRANSLATE_KEY       = 0x8000,

	// oftenly used composite
	K_CHAR_KEY = K_IGNORE_NUMPAD | K_IGNORE_SHIFT | K_IGNORE_CAP_LOCK | K_TRANSLATE_KEY,
	K_UNIQUE_KEY = K_IGNORE_SHIFT | K_IGNORE_CTRL
		| K_IGNORE_ALT | K_IGNORE_NUM_LOCK | K_IGNORE_CAP_LOCK,
};

//-----------------------------------------------//

#define   KEY_BACK_SPACE   0x08
#define   KEY_TAB          0x09
#define   KEY_ESC          0x1B

#define   KEY_RETURN       0xD
#define   KEY_HOME         0x4700
#define   KEY_END          0x4F00
#define   KEY_PGUP         0x4900
#define   KEY_PGDN         0x5100
#define   KEY_UP           0x4800
#define   KEY_DOWN         0x5000
#define   KEY_RIGHT        0x4D00
#define   KEY_LEFT         0x4B00
#define   KEY_CENTER       0x4C00    // Extended keyboard only
#define   KEY_SHIFT_TAB    0x0F00
#define   KEY_INS          0x5200
#define   KEY_DEL          0x5300

#define   KEY_F1           0x3B00
#define   KEY_F2           0x3C00
#define   KEY_F3           0x3D00
#define   KEY_F4           0x3E00
#define   KEY_F5           0x3F00
#define   KEY_F6           0x4000
#define   KEY_F7           0x4100
#define   KEY_F8           0x4200
#define   KEY_F9           0x4300
#define   KEY_F10          0x4400
#define   KEY_F11          0x5700
#define   KEY_F12          0x5800

// 104-key keyboard
#define   KEY_LWIN         0xDB00
#define   KEY_RWIN         0xDC00
#define   KEY_APPS         0xDD00

//-------------------------------------------------//

#endif

