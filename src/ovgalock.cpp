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

#include <ddraw.h>
#include <osys.h>
#include <ovgalock.h>
#include <ovgabuf.h>
#include <ovga.h>
#include <ofile.h>
#include <omouse.h>
#include <all.h>


VgaFrontLock::VgaFrontLock()
{
	if( vga_front.dd_buf )
		vga_front.temp_unlock();
	if( vga_back.dd_buf )
		vga_back.temp_unlock();
}

VgaFrontLock::~VgaFrontLock()
{
	if( vga_back.dd_buf )
		vga_back.temp_restore_lock();
	if( vga_front.dd_buf )
		vga_front.temp_restore_lock();
}

VgaFrontReLock::VgaFrontReLock()
{
	if( vga_front.dd_buf )
		vga_front.temp_lock();
	if( vga_back.dd_buf )
		vga_back.temp_lock();
}

VgaFrontReLock::~VgaFrontReLock()
{
	if( vga_back.dd_buf )
		vga_back.temp_restore_unlock();
	if( vga_front.dd_buf )
		vga_front.temp_restore_unlock();
}


VgaCustomPalette::VgaCustomPalette(const char *fileName)
{
	backup_pal = NULL;
	if( save_palette() && fileName)
		set_custom_palette(fileName);
}

VgaCustomPalette::~VgaCustomPalette()
{
	restore_palette();
	if( backup_pal)
		mem_del(backup_pal);
}


int VgaCustomPalette::save_palette()
{
	// ------ allocate space --------//
	if( !backup_pal )
		backup_pal = mem_add( sizeof(PALETTEENTRY) * 256);

	// ------- get current palette --------//
	if( vga.dd_pal->GetEntries(0, 0, 256, (PALETTEENTRY *)backup_pal) )
	{
		// get palette fail, free backup_pal to indicate save_palette failed
		mem_del(backup_pal);
		backup_pal = NULL;
		return 0;
	}
	else
		return 1;
}



int VgaCustomPalette::set_custom_palette(const char *fileName)
{
	PALETTEENTRY palEntry[256];
	char palBuf[256][3];
	File palFile;

	palFile.file_open(fileName);
	palFile.file_seek(8);     				// bypass the header info
	palFile.file_read(palBuf, 256*3);
	palFile.file_close();

	for(int i=0; i<256; i++)
	{
		palEntry[i].peRed   = palBuf[i][0];
		palEntry[i].peGreen = palBuf[i][1];
		palEntry[i].peBlue  = palBuf[i][2];
		palEntry[i].peFlags = 0;
	}

	return !vga.dd_pal->SetEntries(0, 0, 256, palEntry);
}


int VgaCustomPalette::restore_palette()
{
	if( backup_pal)
		return !vga.dd_pal->SetEntries(0, 0, 256, (PALETTEENTRY *)backup_pal);
	else
		return 1;
}


MouseDispCount::MouseDispCount()
{
	mouse.disp_count_start();
}

MouseDispCount::~MouseDispCount()
{
	mouse.disp_count_end();
}

