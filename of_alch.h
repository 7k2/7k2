// Filename    : OF_ALCH.H
// Description : header file of FirmMonsterAlchemy

#ifndef __OF_ALCH_H
#define __OF_ALCH_H


#include <OFIRM.H>


class FirmMonsterAlchemy : public Firm
{
public:
	short		raw_id;
	short		site_recno;

	float		reserve_qty;		// non-mined raw materials reserve
	int		conversion_tech_id;		//	0 or tech_id

	float 	cur_month_production;
	float		last_month_production;
	float		production_30days()		{ return last_month_production*(30-info.game_day)/30 +
															cur_month_production; }
	int		is_operating()		{ return production_30days() > 0; }

public:
	FirmMonsterAlchemy();
	~FirmMonsterAlchemy();

	void 		init_derived();

	void 		change_nation(int newNationRecno);
	void		next_day();
	void		next_month();

	int		can_converse(int newTechId );
	void		change_conversion(int newTechId, char remoteAction );

	//------- AI functions -------//

	void		process_ai();

	//------- type casting function --------//

	virtual  FirmMonsterAlchemy*	cast_to_FirmMonsterAlchemy()	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void		disp_firm_info(int dispY1, int refreshFlag );
	void		detect_firm_info();

private:
	void 		 produce_raw();
	Location* scan_raw_site();

	//-------- AI actions ---------//

	int		think_del();
	void 		think_change_conversion();
};

#endif