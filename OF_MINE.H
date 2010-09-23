//Filename    : OF_MINE.H
//Description : Header of FirmMine

#ifndef __OF_MINE_H
#define __OF_MINE_H

#ifndef __OF_WORK_H
#include <OF_WORK.H>
#endif

#ifndef __ORAWRES_H
#include <ORAWRES.H>
#endif

//-------- define constant ---------//

#define DEFAULT_MINE_MAX_STOCK_QTY 	500

//------- Define class FirmMine --------//

class FirmMine : public FirmWork
{
public:

	short		raw_id;
	short		site_recno;

	float		reserve_qty;		// non-mined raw materials reserve
	float		stock_qty;			// mined raw materials stock
	float		max_stock_qty;

	short		next_output_link_id;
	short		next_output_firm_recno;

	float 	cur_month_production;
	float		last_month_production;
	float		production_30days()		{ return last_month_production*(30-info.game_day)/30 +
															cur_month_production; }

	int		is_operating()		{ return productivity > 0 && reserve_qty > 0; }

	int	   ai_has_excess_worker();

public:
	FirmMine();
	~FirmMine();

	void 		init_derived();

	void 		change_nation(int newNationRecno);

	void		draw(int displayLayer=1);

	void 		put_info(int refreshFlag);
	void 		detect_info();
	void 		disp_mine_info(int dispY1, int refreshFlag);
	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	void		next_day();
	void		next_month();

	virtual	FirmMine* cast_to_FirmMine() { return this; };

	virtual void process_ai();

	//---------- multiplayer checking codes -----------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

private:
	void 		 produce_raw();
	Location* scan_raw_site();
	void 		 set_next_output_firm();
	int 		 builder_stay_after_construction();

	//------------ AI actions ---------------//

	int			think_build_market();
	virtual int	think_adjust_workforce();

	// -------- protected interface function -------//

	void		disp_firm_info(int dispY1, int refreshFlag);
	void		detect_firm_info();
};

//--------------------------------------//

#endif
