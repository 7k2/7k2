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

// Filename    : OF_SPEC.H
// Description : Header file of FirmBar

#ifndef __OF_SPEC_H
#define __OF_SPEC_H

#include <of_train.h>

//------- Define class FirmSpecial -------//

class FirmSpecial : public FirmTrain
{
public:
	FirmSpecial();

	void		process_ai();

	//------- type casting function --------//

	virtual FirmSpecial*	cast_to_FirmSpecial()	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();
	int		represent_attribute(int traineeId);

	// -------- rally point ----------//

	int		can_set_rally_point(int destBaseObjRecno=0);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void		disp_firm_info(int dispY1, int refreshFlag );
	void		detect_firm_info();

	void		disp_train_menu(int refreshFlag);
	void		detect_train_menu();
	void		disp_spy_menu(int refreshFlag);
	void		detect_spy_menu();

	//-------- AI actions ---------//

	void		think_train_new();
	int		think_del();
	int 		think_transfer_unit();
};

//---------------------------------------//

#endif