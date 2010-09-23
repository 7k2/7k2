// Filename    : OF_INCU.H
// Description : header file of Incubator 


#ifndef __OF_INCU_H
#define __OF_INCU_H

#include <OFIRM.H>

//-------- Define constant ---------//

#define MAX_INCUBATE_TYPE   3
#define MAX_BUILD_QUEUE 	20


class FirmIncubator : public Firm
{
public:
	short build_unit_id;
	DWORD last_process_build_frame_no;
	float build_progress_days;

	short build_queue_array[MAX_BUILD_QUEUE];		// it stores the unit id.
	char  build_queue_count;

	int	is_operating()				{ return build_unit_id; }

static short incubate_unit_id[MAX_INCUBATE_TYPE];

public:
	FirmIncubator();
	~FirmIncubator();

	void 	put_info(int refreshFlag);
	void 	detect_info();
	void	disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int	detect_edit_mode();

	void	process_ai();

	void	next_day();

	// ------ rally point function -----//

	int	can_set_rally_point(int destBaseObjRecno=0);

	// -----------------------------------//

	virtual	FirmIncubator* cast_to_FirmIncubator() { return this; };
	void	add_queue(int unitId);
	void	remove_queue(int unitId);
	void	cancel_build_unit();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	//-------- protected interface function -------//

	void	disp_firm_info(int dispY1, int refreshFlag);
	void	detect_firm_info();

	void 	disp_main_menu(int refreshFlag);
	void 	detect_main_menu();

	void 	disp_build_menu(int refreshFlag);
	void 	detect_build_menu();

	void 	disp_war_info(int dispY1, int refreshFlag);
	void  disp_build_button(int y, int unitId, int buttonUp);
	void  disp_queue_button(int y, int unitId, int buttonUp);

	void 	process_build();
	void	process_queue();

	//-------- AI actions ---------//

	void		think_new_production();
	int 		should_build_new_weapon();
	int		think_del();
};

#endif
