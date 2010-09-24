// Filename    : OF_SPEC.CPP
// Description : Firm Stable

#include <OF_SPEC.H>
#include <ALL.H>
#include <OREMOTE.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OUNITRES.H>
#include <ORACERES.H>
#include <OTOWN.H>

//--------- Begin of function FirmSpecial::FirmSpecial() ------//

FirmSpecial::FirmSpecial()
{
	memset( sizeof(FirmTrain)+(char *)this, 0, sizeof(FirmSpecial)-sizeof(FirmTrain) );
}
//--------- End of function FirmSpecial::FirmSpecial() ------//



// ---------- Begin of function FirmSpecial::can_set_rally_point ------//
//
// whether the firm can set rally point
//
// [int] destBaseObjRecno		- destination base obj recno, 0 for setting position
//
int FirmSpecial::can_set_rally_point(int destBaseObjRecno)
{
	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		Firm *firmPtr = base_obj_array[destBaseObjRecno]->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return firmPtr->is_human();
			}
		}
	}

	return 0;
}
// ---------- End of function FirmSpecial::can_set_rally_point ------//
