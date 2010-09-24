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

// Filename    : OF_SPY.H
// Description : Header file of FirmSpy


#ifndef __OF_SPY_H
#define __OF_SPY_H

#include <OF_TRAIN.H>

class FirmSpy : public FirmTrain
{
public:

public:
	FirmSpy();

	void		process_ai();

	//------- type casting function --------//

	virtual FirmSpy*	cast_to_FirmSpy()	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();
	int		represent_attribute(int traineeId);

	//------- functions on trainee ---------//

	void		recruit_trainee( int townRecno, int trainType, char remoteAction );
	virtual int can_recruit(int townRecno, int trainType);

	// -------- rally point ----------//

	int		can_set_rally_point(int destBaseObjRecno=0);
	int		send_to_rally_point( int unitRecno );

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	virtual  void	advanced_train();

	//------- protected interface function --------//

	void		disp_firm_info(int dispY1, int refreshFlag );
	void		detect_firm_info();

	void		disp_train_menu(int refreshFlag);
	void		detect_train_menu();
	void		disp_spy_menu(int refreshFlag);
	void		detect_spy_menu();

	//-------- AI actions ---------//

	int		think_del();
	void		think_train_new();
	int 		has_spy(int raceId, int isCivilian);
	void 		think_spy_action();
};

#endif