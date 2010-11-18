/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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