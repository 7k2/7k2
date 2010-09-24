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

//Filename    : OF_BASE.H
//Description : Header of FirmBase

#ifndef __OF_BASE_H
#define __OF_BASE_H

#ifndef __OUNIT_H
#include <OUNIT.H>
#endif

#ifndef __OSKILL_H
#include <OSKILL.H>
#endif

#ifndef __OF_WORK_H
#include <OF_WORK.H>
#endif

//------- Define constant -----------//

#define		MAX_PRAY_POINTS		400


//------- Define class FirmBase --------//

class FirmBase : public FirmWork
{
public:
	short		god_id;
	short  	god_unit_recno;		// unit recno of the summoned god

	float		pray_points;

	short		invoked_effect_id;
	short		invoked_effect_para;
	short		invoked_effect_last;

	static char invoked_effect_text[200];

public:
	FirmBase();
	~FirmBase();

	void		init_derived();

	void 		change_nation(int newNationRecno);

	void 		put_info(int refreshFlag);
	void 		detect_info();
	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	void		next_day();
	void		process_ai();

	int		can_invoke();
	void		invoke_god();

	int		can_make_wish();
	void		invoke_random_effect(int effectId, char remoteAction);

	virtual	FirmBase* cast_to_FirmBase() { return this; };

	//-------- multiplayer checking codes --------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	
	// -------- protected interface function -------//

	void		disp_firm_info(int dispY1, int refreshFlag);
	void		detect_firm_info();

private:
	void 		disp_base_info(int dispY1, int refreshFlag);
	void 		disp_god_info(int dispY1, int refreshFlag);

	//------------- AI actions --------------//

	void 		think_invoke_god();
	int 		think_adjust_workforce();
};

//--------------------------------------//

#endif
