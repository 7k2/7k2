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

// Filename    : OF_OFF.H
// Description : header file of magic Tor


#ifndef __OF_OFF_H
#define __OF_OFF_H

#include <OFIRM.H>
#include <OF_MAGI.H>

class FirmOffensive : public FirmMagic
{

public:
	FirmOffensive();
	~FirmOffensive();

	int	is_operating();

	// ------ interface function ------//

	void	put_info(int refreshFlag);
	void	detect_info();

	virtual FirmOffensive* cast_to_FirmOffensive() { return this; }
	
protected:

	// ------- protected interface function ------//

	void	disp_main_menu(int refreshFlag);
	void	detect_main_menu();

	void	disp_magic_menu(int refreshFlag);
	void	detect_magic_menu();
	
};

#endif