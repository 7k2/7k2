// Filename    : OF_OFF.H
// Description : header file of magic Tor


#ifndef __OF_OFF_H
#define __OF_OFF_H

#include <OFIRM.H>
#include <OF_MAGI.H>

class FirmOffensive : public FirmMagic
{

public:
	FirmOffensive();
	~FirmOffensive();

	int	is_operating();

	// ------ interface function ------//

	void	put_info(int refreshFlag);
	void	detect_info();

	virtual FirmOffensive* cast_to_FirmOffensive() { return this; }
	
protected:

	// ------- protected interface function ------//

	void	disp_main_menu(int refreshFlag);
	void	detect_main_menu();

	void	disp_magic_menu(int refreshFlag);
	void	detect_magic_menu();
	
};

#endif