//Filename   : OU_MONS.CPP
//Description: Unit Monster header file

#ifndef __OU_MONS_H
#define __OU_MONS_H

#ifndef __OUNIT_H
#include <OUNIT.H>
#endif

//----------- Define class Monster -----------//

class UnitMonster : public Unit
{
public:
	char* unit_name(int withTitle=1, int firstNameOnly=0);

	//---------- multiplayer checking codes ---------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitMonster*	cast_to_UnitMonster()	{ return this; }
};

//--------------------------------------------//

#endif

