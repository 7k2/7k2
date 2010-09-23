//Filename    : OF_WAR.H
//Description : Header of FirmWar war factory.

#ifndef __OF_WAR_H
#define __OF_WAR_H

#ifndef __OF_WORK_H
#include <OF_WORK.H>
#endif

//-------- Define constant ---------//

#define MAX_BUILD_QUEUE 	20

//------- Define class FirmWar --------//

class FirmWar : public FirmWork
{
public:
	short build_unit_id;
	DWORD last_process_build_frame_no;
	float build_progress_days;

	char  build_queue_array[MAX_BUILD_QUEUE];		// it stores the unit id.
	char  build_queue_count;

	int	is_operating()		{ return productivity > 0 && build_unit_id; }

public:
	FirmWar();
	~FirmWar();

	void 	put_info(int refreshFlag);
	void 	detect_info();

	void 	disp_main_menu(int refreshFlag);
	void 	detect_main_menu();

	void 	disp_build_menu(int refreshFlag);
	void 	detect_build_menu();

	void	disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int	detect_edit_mode();

	void	next_day();
	void	process_ai();

	// ------ rally point function -----//

	int	can_set_rally_point(int destBaseObjRecno=0);

	// -----------------------------------//

	virtual	FirmWar* cast_to_FirmWar() { return this; };
	void	add_queue(int unitId);
	void	remove_queue(int unitId);
	void	cancel_build_unit();

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	//-------- protected interface function -------//

	void	disp_firm_info(int dispY1, int refreshFlag);
	void	detect_firm_info();

	void 	disp_war_info(int dispY1, int refreshFlag);
	void  disp_build_button(int y, int unitId, int buttonUp);
	void  disp_queue_button(int y, int unitId, int buttonUp);

	void 	process_build();
	void	process_queue();

	//-------- AI actions ---------//

	void	think_new_production();
	int 	should_build_new_weapon();
	int	think_del();
	virtual int think_adjust_workforce();
};

//--------------------------------------//

#endif
