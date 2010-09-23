// Filename    : OF_OFF.CPP
// Description : magic tower

#include <OF_OFF.H>
#include <OF_MAGI.H>
#include <ALL.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OREMOTE.H>
#include <OWORLD.H>
#include <ONATION.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OUNIT.H>
#include <OBULLET.H>
#include <ONEWS.H>


// ----- begin of function FirmOffensive::FirmOffensive --------//
//
FirmOffensive::FirmOffensive() : FirmMagic()
{
}	
// ----- end of function FirmOffensive::FirmOffensive --------//


// ----- begin of function FirmOffensive::~FirmOffensive --------//
//
FirmOffensive::~FirmOffensive()
{
}
// ----- end of function FirmOffensive::~FirmOffensive --------//

//--------- Begin of function FirmOffensive::is_operating ---------//
//
int FirmOffensive::is_operating()
{
	return (delayed_magic_count > 0);
}
//----------- End of function FirmOffensive::is_operating -----------//
