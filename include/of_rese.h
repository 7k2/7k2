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

//Filename    : OF_RESE.H
//Description : Header of FirmResearch

#ifndef __OF_RESE_H
#define __OF_RESE_H

#ifndef __OF_WORK_H
#include <of_work.h>
#endif


//------- Define class FirmResearch --------//

#pragma pack(1)
class FirmResearch : public FirmWork
{
public:
	int		is_operating();

public:
	FirmResearch();
	~FirmResearch();

	void 		put_info(int refreshFlag);
	void 		detect_info();

	void 		disp_main_menu(int refreshFlag);
	void 		detect_main_menu();

	void 		disp_research_class_menu(int refreshFlag);
	void 		detect_research_class_menu();

	void 		disp_research_menu(int refreshFlag);
	void 		detect_research_menu();

	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	void		next_day();
	void		process_ai();

	virtual FirmResearch* cast_to_FirmResearch() { return this; };

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	
	// -------- protected interface function -------//

	void		disp_firm_info(int dispY1, int refreshFlag);
	void		detect_firm_info();

private:
	void 		progress_research();

	void		disp_research_info(int dispY1, int refreshFlag);
	void 		disp_research_button(int y, int techId, int buttonUp);

	//-------- AI actions ---------//

	int		think_del();
	int 		think_adjust_workforce();
};
#pragma pack()

//--------------------------------------//

#endif
