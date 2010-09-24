// Filename    : OF_SPEC.H
// Description : Header file of FirmBar

#ifndef __OF_SPEC_H
#define __OF_SPEC_H

#include <OF_TRAIN.H>

//------- Define class FirmSpecial -------//

class FirmSpecial : public FirmTrain
{
public:
	FirmSpecial();

	void		process_ai();

	//------- type casting function --------//

	virtual FirmSpecial*	cast_to_FirmSpecial()	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();
	int		represent_attribute(int traineeId);

	// -------- rally point ----------//

	int		can_set_rally_point(int destBaseObjRecno=0);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void		disp_firm_info(int dispY1, int refreshFlag );
	void		detect_firm_info();

	void		disp_train_menu(int refreshFlag);
	void		detect_train_menu();
	void		disp_spy_menu(int refreshFlag);
	void		detect_spy_menu();

	//-------- AI actions ---------//

	void		think_train_new();
	int		think_del();
	int 		think_transfer_unit();
};

//---------------------------------------//

#endif