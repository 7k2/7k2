// Filename    : OU_CART.H
// Description : Header file for Explosive Cart

#ifndef __OU_CART_H
#define __OU_CART_H

#include <OUNIT.H>

class UnitExpCart : public Unit
{
public:
	char	triggered;

public:
	UnitExpCart();
	~UnitExpCart();

	virtual int is_camouflage(int viewerNation);				// same result across all players

	int	process_die();
	void	trigger_explode();

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitExpCart*	cast_to_UnitExpCart()	{ return this; }
};

#endif