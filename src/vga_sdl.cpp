/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
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

//Filename    : vga_sdl.cpp
//Description : VGA manipulation functions (SDL version)

#include <ovga.h>
#include <all.h>
#include <imgfun.h>
#include <colcode.h>
#include <omouse.h>
#include <omousecr.h>
#include <ocoltbl.h>
#include <ofile.h>
#include <resource.h>
#include <osys.h>
#include <olog.h>
#include <ovgalock.h>
#include <omodeid.h>
#include <dbglog.h>
#include <surface.h>

DBGLOG_DEFAULT_CHANNEL(Vga);

//-------- Define constant --------//

#define UP_OPAQUE_COLOR       (VGA_GRAY+10)
#define DOWN_OPAQUE_COLOR     (VGA_GRAY+13)

//------ Define static class member vars ---------//

char    VgaBase::use_back_buf = 0;
char    VgaBase::opaque_flag  = 0;
VgaBuf* VgaBase::active_buf   = &vga_front;      // default: front buffer

short transparent_code_w;

static int desktop_bpp = 0;
static int window_pitch = 0;
static Uint32 window_pixel_format = 0;

// ------ declare static function ----------//

RGBColor log_alpha_func(RGBColor, int, int);

//-------- Begin of function VgaSDL::VgaSDL ----------//

VgaSDL::VgaSDL() : window(NULL), renderer(NULL), screen(NULL), target(NULL),
                   texture(NULL), video_mode_flags(SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE)
{
   memset(game_pal, 0, sizeof(SDL_Color)*256);
   vga_color_table = new ColorTable;
   vga_blend_table = new ColorTable;
}
//-------- End of function VgaSDL::VgaSDL ----------//


//-------- Begin of function VgaSDL::~VgaSDL ----------//

VgaSDL::~VgaSDL()
{
   deinit();      // 1-is final

   delete vga_blend_table;
   delete vga_color_table;
}
//-------- End of function VgaSDL::~VgaSDL ----------//


//-------- Begin of function VgaSDL::create_window --------//
//
int VgaSDL::create_window()
{
   if (SDL_CreateWindowAndRenderer(800,
                                   600,
                                   video_mode_flags,
                                   &window,
                                   &renderer) < 0)
   {
      ERR("Could not create window and renderer: %s\n", SDL_GetError());
      SDL_Quit();
      return 0;
   }

   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
   SDL_RenderSetLogicalSize(renderer, VGA_WIDTH, VGA_HEIGHT);

   SDL_RendererInfo info;
   if (SDL_GetRendererInfo(renderer, &info) == 0)
   {
      MSG("Name of renderer: %s\n", info.name);
      MSG("Using software fallback: %s\n", info.flags & SDL_RENDERER_SOFTWARE ? "yes" : "no");
      MSG("Using hardware acceleration: %s\n", info.flags & SDL_RENDERER_ACCELERATED ? "yes" : "no");
      MSG("V-sync: %s\n", info.flags & SDL_RENDERER_PRESENTVSYNC ? "on" : "off");
      MSG("Rendering to texture support: %s\n", info.flags & SDL_RENDERER_TARGETTEXTURE ? "yes" : "no");
      MSG("Maximum texture width: %d\n", info.max_texture_width);
      MSG("Maximum texture height: %d\n", info.max_texture_height);
   }

   window_pixel_format = SDL_GetWindowPixelFormat(window);
   if (window_pixel_format == SDL_PIXELFORMAT_UNKNOWN)
   {
      ERR("Unknown pixel format: %s\n", SDL_GetError());
      SDL_Quit();
      return 0;
   }
   MSG("Pixel format: %s\n", SDL_GetPixelFormatName(window_pixel_format));

   window_pitch = VGA_WIDTH * SDL_BYTESPERPIXEL(window_pixel_format);

   if (SDL_PIXELTYPE(window_pixel_format) == SDL_PIXELTYPE_PACKED32)
   {
      desktop_bpp = 32;
   }
   else if (SDL_PIXELTYPE(window_pixel_format) == SDL_PIXELTYPE_PACKED16)
   {
      desktop_bpp = 16;
   }
   else if (SDL_PIXELTYPE(window_pixel_format) == SDL_PIXELTYPE_PACKED8)
   {
      desktop_bpp = 8;
   }
   else
   {
      ERR("Unsupported pixel type\n");
      SDL_Quit();
      return 0;
   }

   SDL_SetWindowTitle(window, WIN_TITLE);

   return 1;
}
//-------- End of function VgaSDL::init_window --------//


//-------- Begin of function VgaSDL::destroy_window --------//
//
void VgaSDL::destroy_window()
{
   SDL_DestroyWindow(window);
}
//-------- End of function VgaSDL::destroy_window --------//


//-------- Begin of function VgaSDL::init ----------//

int VgaSDL::init()
{
   if (SDL_Init(SDL_INIT_VIDEO) != 0)
   {
      return 0;
   }

   if( !create_window() )
      return 0;

   if ( !create_texture() )
   {
      SDL_Quit();
      return 0;
   }

   if ( !create_surface(target, desktop_bpp) )
   {
      SDL_Quit();
      return 0;
   }

   if ( !create_surface(screen, VGA_BPP) )
   {
      SDL_Quit();
      return 0;
   }

   if( !set_mode(VGA_WIDTH, VGA_HEIGHT) )
      return 0;

   init_pal(DIR_RES"PAL_STD.RES");

   // update Sys::deinit and Sys::change_display_mode

   if( sys.use_true_front )                // if we are currently in triple buffer mode, don't lock the front buffer otherwise the system will hang up
   {
      init_front(&vga_true_front);
      init_back(&vga_front);		// create in video memory
      vga_front.is_front = 1;       // set it to 1, overriding the setting in init_back()
      init_back(&vga_back);
   }
   else
   {
      init_front(&vga_front);
      init_back(&vga_back);		// create in system memory
   }

   vga_front.lock_buf();

   vga_back.lock_buf();

   return 1;
}
//-------- End of function VgaSDL::init ----------//


//-------- Begin of function VgaSDL::init_front ----------//
//
// Inform the front buffer of the actual surface.  This function retains
// compatibility with old direct draw code.
//
int VgaSDL::init_front(VgaBuf *b)
{
   b->lock_bit_stack = 0;
   b->lock_stack_count = 0;

   b->default_remap_table = vga.default_remap_table;       // new for 16-bit
   b->default_blend_table = vga.default_blend_table;       // new for 16-bit

   b->init(new SurfaceSDL(screen), 1);
   return 1;
}
//-------- End of function VgaSDL::init_front ----------//


//-------- Begin of function VgaSDL::init_back ----------//
//
// Create an SDL back buffer.
//
// int width      : width of the surface [default 0 : VGA_WIDTH]
// int height     : height of the surface [default 0 : VGA_HEIGHT]
//
int VgaSDL::init_back(VgaBuf *b, int width, int height)
{
   SDL_Surface *surface = SDL_CreateRGBSurface(0,
					       VGA_WIDTH,
					       VGA_HEIGHT,
					       VGA_BPP,
					       0, 0, 0, 0);

   if (surface == NULL)
   {
      ERR("Surface not created!\n");
      return 0;
   }

   b->lock_bit_stack = 0;
   b->lock_stack_count = 0;

   b->default_remap_table = vga.default_remap_table;	// new for 16-bit

   SurfaceSDL *wrapper = new SurfaceSDL(surface);
   b->init(wrapper, 0);
   return 1;
}
//-------- End of function VgaSDL::init_back ----------//


//-------- Begin of function VgaSDL::set_mode ----------//

int VgaSDL::set_mode(int width, int height)
{
   pixel_format_flag = PIXFORM_BGR_565;

   // assembly functions to initalize effect processing

   INITbright(pixel_format_flag);

   return 1;
}
//-------- End of function VgaSDL::set_mode ----------//


//-------- Begin of function VgaSDL::deinit ----------//

void VgaSDL::deinit()
{
   vga_back.deinit();

   if( sys.use_true_front )
   {
      vga_true_front.deinit();
   }

   vga_front.deinit();

   destroy_surface(screen);
   destroy_surface(target);
   destroy_texture();
   destroy_window();

   SDL_Quit();
   video_mode_flags = 0;
}
//-------- End of function VgaSDL::deinit ----------//


// ------- begin of function VgaSDL::is_inited --------//
//
bool VgaSDL::is_inited()
{
   return screen != NULL;
}
// ------- end of function VgaSDL::is_inited --------//


//--------- Start of function VgaSDL::init_pal ----------//
//
// Load the palette from a file and set it to the front buf.
//
int VgaSDL::init_pal(const char* fileName)
{
   char palBuf[256][3];
   File palFile;

   palFile.file_open(fileName);
   palFile.file_seek(8);               // bypass the header info
   palFile.file_read(palBuf, 256*3);
   palFile.file_close();

   for(int i=0; i<256; i++)
   {
      game_pal[i].r = palBuf[i][0];
      game_pal[i].g = palBuf[i][1];
      game_pal[i].b = palBuf[i][2];
   }

   init_color_table();

   // set global variable
   transparent_code_w = translate_color(TRANSPARENT_CODE);

   return 1;
}
//----------- End of function VgaSDL::init_pal ----------//


//--------- Start of function VgaSDL::init_color_table ----------//

void VgaSDL::init_color_table()
{
   //----- initialize interface color table -----//

   PalDesc palDesc( (unsigned char*) game_pal, sizeof(SDL_Color), 256, 8);
   vga_color_table->generate_table_fast( MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func );

   default_remap_table = (short *)vga_color_table->get_table(0);

   //----- initialize interface color table for blending -----//

   vga_blend_table->generate_table_fast( 8, palDesc, log_alpha_func );
   default_blend_table = (short *)vga_blend_table->get_table(0);
}
//----------- End of function VgaSDL::init_color_table ----------//


//-------- Begin of function VgaSDL::is_full_screen --------//
int VgaSDL::is_full_screen()
{
   return video_mode_flags & SDL_WINDOW_FULLSCREEN_DESKTOP;
}
//-------- End of function VgaSDL::is_full_screen ----------//


//-------- Begin of function VgaSDL::toggle_full_screen --------//
void VgaSDL::toggle_full_screen()
{
   int result = 0;

   if (!is_full_screen()) {
      result = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
      video_mode_flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
   } else {
      result = SDL_SetWindowFullscreen(window, 0);
      video_mode_flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
   }
   if (result < 0) {
      ERR("Could not toggle fullscreen: %s\n", SDL_GetError());
      return;
   }

   sys.need_redraw_flag = 1;
}
//-------- End of function VgaSDL::toggle_full_screen ----------//


//----------- Begin of function VgaSDL::flip ----------//
void VgaSDL::flip()
{
#if(defined(USE_FLIP))

	mouse_cursor.before_flip();

	vga_front.temp_unlock();
	vga_back.temp_unlock();

	vga_front.flip(&vga_back);

	vga_back.temp_restore_lock();
	vga_front.temp_restore_lock();

	mouse_cursor.after_flip();
#endif
}
//----------- End of function VgaSDL::flip ----------//


//----------- Begin of function VgaSDL::update_screen ----------//
void VgaSDL::update_screen()
{
   static Uint32 ticks = 0;
   Uint32 cur_ticks = SDL_GetTicks();
   if (cur_ticks > ticks + 34 || cur_ticks < ticks) {
      ticks = cur_ticks;
      SDL_BlitSurface(screen, NULL, target, NULL);
      SDL_UpdateTexture(texture, NULL, target->pixels, window_pitch);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
   }
}
//----------- End of function VgaSDL::update_screen ----------//


//-------- Begin of function VgaSDL::handle_messages --------//
void VgaSDL::handle_messages()
{
   SDL_Event event;

   SDL_PumpEvents();

   while (SDL_PeepEvents(&event,
	                 1,
			 SDL_GETEVENT,
			 SDL_QUIT,
			 SDL_WINDOWEVENT) > 0) {

      switch (event.type) {
      case SDL_WINDOWEVENT:
	if (event.window.event == SDL_WINDOWEVENT_ENTER ||
	    event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
	{
	   sys.active_flag = 1;
	   sys.need_redraw_flag = 1;
	   sys.unpause();
	   SDL_SetModState(KMOD_NONE);
	   mouse.update_skey_state();
	   SDL_ShowCursor(SDL_DISABLE);
	}
	else if (event.window.event == SDL_WINDOWEVENT_LEAVE ||
	         event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
	{
//	   sys.active_flag = 0;
//	   sys.pause();
	   SDL_ShowCursor(SDL_ENABLE);
	}
	else if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
	{
	   sys.need_redraw_flag = 1;
	}
	else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
	{
	   sys.signal_exit_flag = 1;
	}
	break;
      case SDL_QUIT:
	sys.signal_exit_flag = 1;
	break;
      default:
	ERR("unhandled event %d\n", event.type);
	break;
      }
   }
}
//-------- End of function VgaSDL::handle_messages --------//


//-------- Begin of function VgaSDL::change_resolution --------//
int VgaSDL::change_resolution(int width, int height)
{
   destroy_surface(screen);
   destroy_surface(target);
   destroy_texture();

   SDL_SetWindowSize(window, width, height);
   SDL_RenderSetLogicalSize(renderer, VGA_WIDTH, VGA_HEIGHT);
   window_pitch = VGA_WIDTH * SDL_BYTESPERPIXEL(window_pixel_format);

   create_texture();
   create_surface(target, desktop_bpp);
   create_surface(screen, VGA_BPP);

   if( sys.use_true_front )                // if we are currently in triple buffer mode, don't lock the front buffer otherwise the system will hang up
   {
      init_front(&vga_true_front);
      init_back(&vga_front);
      vga_front.is_front = 1;       // set it to 1, overriding the setting in init_back()
      init_back(&vga_back);
   }
   else
   {
      init_front(&vga_front);
      init_back(&vga_back);
   }

   vga_front.lock_buf();

   vga_back.lock_buf();

   return 1;
}
//-------- End of function VgaSDL::change_resolution --------//


//-------- Begin of function VgaSDL::create_surface --------//
//
bool VgaSDL::create_surface(SDL_Surface *&surface, int bpp)
{
   surface = SDL_CreateRGBSurface(0, VGA_WIDTH, VGA_HEIGHT, bpp, 0, 0, 0, 0);
   if (!surface)
   {
      return false;
   }
   else
   {
      return true;
   }
}
//-------- End of function VgaSDL::create_surface --------//


//-------- Begin of function VgaSDL::create_texture --------//
//
bool VgaSDL::create_texture()
{
   texture = SDL_CreateTexture(renderer,
                               window_pixel_format,
                               SDL_TEXTUREACCESS_STREAMING,
                               VGA_WIDTH,
                               VGA_HEIGHT);
   if (!texture)
   {
      return false;
   }
   else
   {
      return true;
   }
}
//-------- End of function VgaSDL::create_texture --------//


//-------- Begin of function VgaSDL::destroy_surface --------//
//
void VgaSDL::destroy_surface(SDL_Surface *&surface)
{
   if (surface != NULL)
   {
      SDL_FreeSurface(surface);
      surface = NULL;
   }
}
//-------- End of function VgaSDL::destroy_surface --------//


//-------- Begin of function VgaSDL::destroy_texture --------//
void VgaSDL::destroy_texture()
{
   if (texture != NULL)
   {
      SDL_DestroyTexture(texture);
      texture = NULL;
   }
}
//-------- End of function VgaSDL::destroy_texture --------//


namespace
{

// Source of rotl function:
//   http://en.wikipedia.org/wiki/Circular_shift
unsigned int rotl(const unsigned int value, int shift)
{
	if( (shift &= sizeof(value) * 8 - 1) == 0 )
		return value;
	return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

}  // namespace

int VgaSDL::make_pixel(Uint8 red, Uint8 green, Uint8 blue)
{
	Uint32 eax = SDL_Swap32((blue << 16) + (green << 8) + red);
	eax = rotl(eax, 8);
	Uint8 al = ((Uint8)eax) >> 3;
	eax = (eax & 0xFFFFFF00) | al;
	eax = rotl(eax, 8);
	Uint16 ax = ((Uint16)eax) >> 2;
	eax = (eax & 0xFFFF0000) | ax;
	eax = rotl(eax, 5);
	return eax;
}

int VgaSDL::make_pixel(RGBColor *rgb)
{
	return make_pixel(rgb->red, rgb->green, rgb->blue);
}

void VgaSDL::decode_pixel(int p, RGBColor *rgb)
{
	Uint32 edx = p;
	p <<= 19;
	edx <<= 5;
	Uint8 ah = (Uint16)edx >> 8;
	p = (p & 0xFFFF00FF) | (ah << 8);
	edx >>= 13;
	p = (p & 0xFFFFFF00) | (Uint8)edx;
	int u = p;
	memcpy(rgb, &u, sizeof(RGBColor));
}



// --------- begin of static function log_alpha_func -------//
//
// function for calculating table for IMGbltBlend
//
RGBColor log_alpha_func(RGBColor i, int scale, int absScale)
{
	RGBColor r;
	if( scale >= 0 )
	{
		r.red   = i.red   - (i.red   >> scale);
		r.green = i.green - (i.green >> scale);
		r.blue  = i.blue  - (i.blue  >> scale);
	}
	else
	{
		r.red   = i.red   >> -scale;
		r.green = i.green >> -scale;
		r.blue  = i.blue  >> -scale;
	}

	return r;
}
