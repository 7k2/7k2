// Filename   : OATTACK.CPP
// Description : attack attribute

#include <OATTACK.H>


float AttackAttribute::effectiveness ( DefenseAttribute &da )
{
	return 
		( ((int)sturdiness + explosiveness)/2 * da.sturdiness
		+ (100 - da.sturdiness) * heat + wood_favour * da.is_wood ) / 10000.0f + 1.0f;
}
