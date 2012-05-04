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

//Filename    : OF_FACT.H
//Description : Header of FirmFactory

#ifndef __OF_FACT_H
#define __OF_FACT_H

#ifndef __OF_WORK_H
#include <of_work.h>
#endif

#define MAX_RAW 3
#define MAX_PRODUCT 3

//------- Define class FirmFactory --------//

#pragma pack(1)
class FirmFactory : public FirmWork
{
public:
	int   	product_raw_id;	// the raw id. of the product

	float		stock_qty;			// mined raw materials stock
	float		max_stock_qty;

	float		raw_stock_qty;			// raw materials stock
	float		max_raw_stock_qty;

	float 	cur_month_production;
	float		last_month_production;
	float		production_30days()		{ return last_month_production*(30-info.game_day)/30 +
															cur_month_production; }
	short		next_output_link_id;
	short		next_output_firm_recno;

	int		is_operating()		{ return productivity > 0 && production_30days() > 0; }

	float		backup_raw_array[MAX_RAW];
	float		backup_product_array[MAX_PRODUCT];

	int		auto_switch_flag;

public:
	FirmFactory();
	~FirmFactory();

	void 		init_derived();

	void		draw(int displayLayer=1);

	void 		put_info(int refreshFlag);
	void 		detect_info();
	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	void		next_day();
	void		next_month();

	void		set_product(int rawId)	{ product_raw_id = rawId; };
	// ##### begin Gilbert 5/10 #######//
	void		change_production(char remoteAction);
	// ##### end Gilbert 5/10 #######//

	virtual	FirmFactory* cast_to_FirmFactory() { return this; };

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

private:
	void		auto_set_product();
	void 		disp_factory_info(int dispY1, int refreshFlag);
	// ##### begin Gilbert 5/10 #######//
//	void		change_production();
	// ##### end Gilbert 5/10 #######//
	void 		set_production(int newProductId);
	void 		set_next_output_firm();
	void 		production();
	void		input_raw();
	void 		manufacture(float maxMftQty);

	//--------------- AI actions ----------------//

	void		process_ai();
	int 		think_build_market();
	int 		think_change_production();
	virtual int think_adjust_workforce();

protected:
	
	// -------- protected interface function -------//

	void	disp_firm_info(int dispY1, int refreshFlag);
	void	detect_firm_info();
};
#pragma pack()

//--------------------------------------//

#endif
