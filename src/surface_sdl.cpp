/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 2010 Jesse Allen
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

//Filename    : surface_sdl.cpp
//Description : SDL surface class

#include <surface_sdl.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(Surface);


//-------- Begin of function SurfaceSDL::SurfaceSDL ----------//

SurfaceSDL::SurfaceSDL(SDL_Surface *s) :
	surface(s), save_buf(NULL)
{
	cur_pitch = s->pitch;
}
//-------- End of function SurfaceSDL::SurfaceSDL ----------//


//-------- Begin of function SurfaceSDL::~SurfaceSDL ----------//

SurfaceSDL::~SurfaceSDL()
{
	// surface will be freed by SDL_Quit if it is the front buffer
	// any other surface should be freed here, but we will let
	// SDL_Quit take care of it since it is likely only the back buffer
}
//-------- End of function SurfaceSDL::~SurfaceSDL ----------//


//-------- Begin of function SurfaceSDL::is_buf_lost ----------//
//
BOOL SurfaceSDL::is_buf_lost()
{
	return 0;
}
//--------- End of function SurfaceSDL::is_buf_lost ----------//


//-------- Begin of function SurfaceSDL::restore_buf ----------//
//
// Restore buffers that have been lost.
//
BOOL SurfaceSDL::restore_buf()
{
	return 1;
}
//--------- End of function SurfaceSDL::restore_buf ----------//


//------------- Begin of function SurfaceSDL::lock_buf --------------//

int SurfaceSDL::lock_buf()
{
	return 1;
}
//--------------- End of function SurfaceSDL::lock_buf --------------//


//------------- Begin of function SurfaceSDL::unlock_buf --------------//

int SurfaceSDL::unlock_buf()
{
	return 1;
}
//--------------- End of function SurfaceSDL::unlock_buf --------------//


//-------- Begin of function SurfaceSDL::blt_buf_area --------//
//
// Blit an area of the source surface to local destination surface.
//
void SurfaceSDL::blt_buf_area(SurfaceSDL *srcBuf, int x1, int y1, int x2, int y2)
{
	SDL_Rect bltRect;

	bltRect.x = x1;
	bltRect.y = y1;
	bltRect.w = x2 - x1 + 1;
	bltRect.h = y2 - y1 + 1;

	SDL_BlitSurface(srcBuf->get_surface(), &bltRect, surface, &bltRect);
}
//--------- End of function SurfaceSDL::blt_buf_area ---------//


//-------- Begin of function SurfaceSDL::blt_virtual_buf --------//
//
// Blit entire source surface to local destination surface.
//
void SurfaceSDL::blt_virtual_buf(SurfaceSDL *srcBuf)
{
	SDL_BlitSurface(srcBuf->get_surface(), NULL, surface, NULL);
}
//--------- End of function SurfaceSDL::blt_virtual_buf ---------//


//-------- Begin of function SurfaceSDL::blt_virtual_buf_area --------//
//
// Blit an area of the source surface to local destination surface.
//
void SurfaceSDL::blt_virtual_buf_area(SurfaceSDL *srcBuf, int x1, int y1, int x2, int y2)
{
	SDL_Rect bltRect;

	bltRect.x = x1;
	bltRect.y = y1;
	bltRect.w = x2 - x1 + 2;
	bltRect.h = y2 - y1 + 2;

	SDL_BlitSurface(srcBuf->get_surface(), &bltRect, surface, &bltRect);
}
//--------- End of function SurfaceSDL::blt_virtual_buf_area ---------//


//------------ Begin of function SurfaceSDL::write_bmp_file --------------//
//
// Save the surface as a BMP file.
//
// <char*> fileName - the name of the BMP file.
//
// return : <int> 1-succeeded, 0-failed.
//
int SurfaceSDL::write_bmp_file(char* fileName)
{
	return !SDL_SaveBMP(surface, fileName);
}
//------------ End of function SurfaceSDL::write_bmp_file --------------//


//------------ Begin of function SurfaceSDL::get_surface --------------//
SDL_Surface *SurfaceSDL::get_surface()
{
	return surface;
}
//------------ End of function SurfaceSDL::get_surface --------------//
