//Filename    : OF_RESE.H
//Description : Header of FirmResearch

#ifndef __OF_RESE_H
#define __OF_RESE_H

#ifndef __OF_WORK_H
#include <OF_WORK.H>
#endif


//------- Define class FirmResearch --------//

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

//--------------------------------------//

#endif
