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
