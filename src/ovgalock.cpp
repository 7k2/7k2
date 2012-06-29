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

// Filename    : OVGALOCK.CPP
// Description : object to vga_front.temp_unlock and temp_restore_lock
// Onwer       : Gilbert Luis
// Mainly used by ODPLAY.CPP to separate OVGA.H
// some GUID initialization problems exist if OVGA.H is included in ODPLAY.CPP

#include <ovgalock.h>
#include <ovgabuf.h>
#include <omouse.h>


VgaFrontLock::VgaFrontLock()
{
	if( vga_front.is_inited() )
		vga_front.temp_unlock();
	if( vga_back.is_inited() )
		vga_back.temp_unlock();
}

VgaFrontLock::~VgaFrontLock()
{
	if( vga_back.is_inited() )
		vga_back.temp_restore_lock();
	if( vga_front.is_inited() )
		vga_front.temp_restore_lock();
}

VgaFrontReLock::VgaFrontReLock()
{
	if( vga_front.is_inited() )
		vga_front.temp_lock();
	if( vga_back.is_inited() )
		vga_back.temp_lock();
}

VgaFrontReLock::~VgaFrontReLock()
{
	if( vga_back.is_inited() )
		vga_back.temp_restore_unlock();
	if( vga_front.is_inited() )
		vga_front.temp_restore_unlock();
}


MouseDispCount::MouseDispCount()
{
	mouse.disp_count_start();
}

MouseDispCount::~MouseDispCount()
{
	mouse.disp_count_end();
}

