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

//Filename    : vga_ddraw.cpp
//Description : VGA manipulation functions (Direct Draw version)

#define DEBUG_LOG_LOCAL 1

#include <windowsx.h>
// include ddraw before ovga.h such that dd_buf is translated
#include <all.h>
#include <imgfun.h>
#include <colcode.h>
#include <omouse.h>
#include <omousecr.h>
#include <ocoltbl.h>
#include <ofile.h>
#include <resource.h>
#include <osys.h>
#include <ovga.h>
#include <olog.h>
#include <ovgalock.h>
#include <omodeid.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(Vga);

//-------- Define constant --------//

#define UP_OPAQUE_COLOR       (VGA_GRAY+10)
#define DOWN_OPAQUE_COLOR     (VGA_GRAY+13)

//------ Define static class member vars ---------//

char    VgaBase::use_back_buf = 0;
char    VgaBase::opaque_flag  = 0;
VgaBuf* VgaBase::active_buf   = &vga_front;      // default: front buffer

char    low_video_memory_flag = 0;
extern "C"
{
	short transparent_code_w;
}

// ------ declare static function ----------//

RGBColor log_alpha_func(RGBColor, int, int);

extern const char *dd_err_str( const char *str, HRESULT rc);

long FAR PASCAL main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//-------- Begin of function VgaDDraw::VgaDDraw ----------//

VgaDDraw::VgaDDraw()
{
	memset( this, 0, sizeof(Vga) );

   vga_color_table = new ColorTable;
	vga_blend_table = new ColorTable;

   // window related
   main_hwnd = NULL;
   app_hinstance = NULL;
}
//-------- End of function VgaDDraw::VgaDDraw ----------//


//-------- Begin of function VgaDDraw::~VgaDDraw ----------//

VgaDDraw::~VgaDDraw()
{
   deinit();      // 1-is final

	delete vga_blend_table;
   delete vga_color_table;
}
//-------- End of function VgaDDraw::~VgaDDraw ----------//


//-------- Begin of function VgaDDraw::create_window --------//
//
int VgaDDraw::create_window()
{
   app_hinstance = (HINSTANCE)GetModuleHandle(NULL);

   //--------- register window class --------//

   WNDCLASS    wc;
   BOOL        rc;

   wc.style          = CS_DBLCLKS;
   wc.lpfnWndProc    = main_win_proc;
   wc.cbClsExtra     = 0;
   wc.cbWndExtra     = 0;
   wc.hInstance      = (HINSTANCE__ *) app_hinstance;
   wc.hIcon          = LoadIcon( (HINSTANCE__ *) app_hinstance, MAKEINTATOM(IDI_ICON1));
   wc.hbrBackground  = (HBRUSH__ *) GetStockObject(BLACK_BRUSH);
   wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
   wc.lpszMenuName   = NULL;
   wc.lpszClassName  = WIN_CLASS_NAME;

   rc = RegisterClass( &wc );

   if( !rc )
      return 0;

   //--------- create window -----------//

   main_hwnd = CreateWindowEx(
       WS_EX_APPWINDOW | WS_EX_TOPMOST,
       WIN_CLASS_NAME,
       WIN_TITLE,
       WS_VISIBLE |    // so we dont have to call ShowWindow
       WS_POPUP,
       0,
       0,
       GetSystemMetrics(SM_CXSCREEN),
       GetSystemMetrics(SM_CYSCREEN),
       NULL,
       NULL,
       (HINSTANCE__ *) app_hinstance,
       NULL );

   if( !main_hwnd )
      return 0;

   UpdateWindow( main_hwnd );
   SetFocus( main_hwnd );

   return 1;
}
//-------- End of function VgaDDraw::init_window --------//


//-------- Begin of function VgaDDraw::destroy_window --------//
//
void VgaDDraw::destroy_window()
{
   // ####### begin Gilbert 19/2 ######//
   if( main_hwnd )
   {
      PostMessage(main_hwnd, WM_CLOSE, 0, 0);
   }

   sys.init_flag = 0;

   MSG msg;

   if( main_hwnd )		// different from while( main_hwnd && GetMessage(...)) as wnd_proc may clear main_hwnd
   {
      while( GetMessage(&msg, NULL, 0, 0) )
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }
   // ####### end Gilbert 19/2 ######//
}
//-------- End of function VgaDDraw::destroy_window --------//


//-------- Begin of function VgaDDraw::init ----------//

int VgaDDraw::init()
{
	// size check

	err_when( sizeof(dd_obj) > sizeof(vptr_dd_obj) );
	err_when( sizeof(dd_pal) > sizeof(vptr_dd_pal) );
	err_when( sizeof(pal_entry_buf) > sizeof(dw_pal_entry_buf) );

	const char* warnStr = "Warning: Due to the low memory of your display card, "
						 "you may experience problems when you quit the game or "
						 "switch tasks during the game. "
						 "To avoid this problem, set your Windows display "
						 "to 800x600 16-bit color mode before running the game.";

   if( !create_window() )
      return 0;

   //--------- Initialize DirectDraw object --------//

   if( !init_dd() )
      return 0;


//	suspected cause of hang in dx6

   // get current display mode
	DDSURFACEDESC2 ddsd;
	DDSCAPS2  ddsCaps;
	DWORD    dwTotal;
	DWORD    dwFree;

	memset(&ddsd, 0, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);
	ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

	if( dd_obj->GetDisplayMode(&ddsd) == DD_OK &&
		dd_obj->GetAvailableVidMem(&ddsCaps, &dwTotal, &dwFree) == DD_OK )
	{
		if( dwFree < (DWORD) VGA_WIDTH*VGA_HEIGHT*VGA_BPP/8 &&
			!(ddsd.dwWidth==(DWORD)VGA_WIDTH && ddsd.dwHeight==(DWORD)VGA_HEIGHT && (ddsd.ddpfPixelFormat.dwRGBBitCount == (DWORD)VGA_BPP)) )
		{
			// not enough memory except same video mode

			ShowCursor(TRUE);
			// approximation of video memory required, actual video memory used should be calculated from vga_(true)_front->buf_pitch()

			extern char new_config_dat_flag;

			if( new_config_dat_flag )
			{
				MessageBox(main_hwnd, warnStr,
					WIN_TITLE, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND );
			}

			low_video_memory_flag = 1;

			ShowCursor(FALSE);
		}
	}

   if( !set_mode(VGA_WIDTH, VGA_HEIGHT) )
      return 0;

   DEBUG_LOG("Attempt init_pal()");
   init_pal(DIR_RES"PAL_STD.RES");
   DEBUG_LOG("init_pal() finish");

	// update Sys::deinit and Sys::change_display_mode

   if( sys.use_true_front )                // if we are currently in triple buffer mode, don't lock the front buffer otherwise the system will hang up
   {
      DEBUG_LOG("Attempt vga_true_front.init_front()");
      init_front(&vga_true_front);
      DEBUG_LOG("Attempt vga_front.init_back()");
      init_back(&vga_front);		// create in video memory
      vga_front.is_front = 1;       // set it to 1, overriding the setting in init_back()
		DEBUG_LOG("Attempt vga_back.init_back()");
		init_back(&vga_back);
		DEBUG_LOG("vga_back.init_back() finish");
   }
   else
   {
      init_front(&vga_front);
#if(!defined(USE_FLIP))
		init_back(&vga_back);		// create in system memory
#else
		init_back(&vga_back, 0, 0, 1 );		// create in video memory
#endif
   }

#if(defined(USE_FLIP))
	vga_front.attach_surface( &vga_back );
#endif

   DEBUG_LOG("Attempt vga_front.lock_buf()");
   vga_front.lock_buf();
   DEBUG_LOG("vga_front.lock_buf() finish");

   DEBUG_LOG("Attempt vga_back.lock_buf()");
   vga_back.lock_buf();
   DEBUG_LOG("vga_back.lock_buf() finish");

   return 1;
}
//-------- End of function VgaDDraw::init ----------//


//-------- Begin of function VgaDDraw::init_dd ----------//

int VgaDDraw::init_dd()
{
   if(dd_obj)        // the Direct Draw object has been initialized already
      return 1;

   //--------- Create direct draw object --------//

   DEBUG_LOG("Attempt DirectDrawCreate");
   LPDIRECTDRAW dd1Obj;
   int rc = DirectDrawCreate( NULL, &dd1Obj, NULL );
   DEBUG_LOG("DirectDrawCreate finish");

   if( rc != DD_OK )
   {
      ERR("DirectDrawCreate failed err=%d", rc);
		err.run( dd_err_str("DirectDrawCreate failed!", rc) );
      return 0;
   }

   //-------- Query DirectDraw4 interface --------//

   DEBUG_LOG("Attempt Query DirectDraw4");
   rc = dd1Obj->QueryInterface(IID_IDirectDraw4, (void **)&dd_obj);
   DEBUG_LOG("Query DirectDraw2 finish");
   if( rc != DD_OK )
   {
      ERR("Query DirectDraw4 failed err=%d", rc);
		err.run( dd_err_str("Query DirectDraw4(DirectX6) failed", rc) );
      dd1Obj->Release();
      return 0;
   }

   dd1Obj->Release();

   //-----------------------------------------------------------//
   // Convert it to a plain window
   //-----------------------------------------------------------//

   DWORD   dwStyle;
   dwStyle = GetWindowStyle(main_hwnd);
   dwStyle |= WS_POPUP;
   dwStyle &= ~(WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
   SetWindowLong(main_hwnd, GWL_STYLE, dwStyle);

   //-----------------------------------------------------------//
   // grab exclusive mode if we are going to run as fullscreen
   // otherwise grab normal mode.
   //-----------------------------------------------------------//

   DEBUG_LOG("Attempt DirectDraw SetCooperativeLevel");
   rc = dd_obj->SetCooperativeLevel( main_hwnd,
                        DDSCL_EXCLUSIVE |
                        DDSCL_FULLSCREEN );
   DEBUG_LOG("DirectDraw SetCooperativeLevel finish");

   if( rc != DD_OK )
   {
      ERR("SetCooperativeLevel failed err=%d", rc);
		err.run( dd_err_str("SetCooperativeLevel failed", rc) );
      return 0;
   }

   return 1;
}
//-------- End of function VgaDDraw::init_dd ----------//


//-------- Begin of function VgaDDraw::init_front ----------//
//
// Create a direct draw front buffer.
//
int VgaDDraw::init_front(VgaBuf *b)
{
	DDSURFACEDESC2       ddsd;
	Surface              *surface;

	// ##### begin Gilbert 4/11 ######//
	//------ Get Direct Draw capacity info --------//
//	DDCAPS              ddcaps;
//	ZeroMemory( &ddcaps, 0, sizeof(ddcaps) );
//	ddcaps.dwSize = sizeof( ddcaps );
//	if( ddPtr->GetCaps( &ddcaps, NULL ) != DD_OK )
//		err.run( "Error creating Direct Draw front surface!" );
	// ##### end Gilbert 4/11 ######//

	//---------------------------------------------//
	// Create the Front Buffer
	//---------------------------------------------//

	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize = sizeof( ddsd );

	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
//	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

//	LPDIRECTDRAWSURFACE dd1Buf;
//	rc = ddPtr->CreateSurface( &ddsd, &dd1Buf, NULL );
	// no createSurface can direct create direct draw surface 4 interface
	surface = create_surface( &ddsd );
	if( !surface )
	{
		err.run( "Error creating Direct Draw front surface!" );
		return 0;
	}

//	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
//	if( rc != DD_OK )
//	{
//		dd1Buf->Release();
//		err.run ( dd_err_str("Error creating Direct Draw front surface!!", rc) );
//	}
//	dd1Buf->Release();

	b->lock_bit_stack = 0;
	b->lock_stack_count = 0;

	b->default_remap_table = vga.default_remap_table;	// new for 16-bit
	b->default_blend_table = vga.default_blend_table;	// new for 16-bit

	b->init(surface, 1);
	return 1;
}
//-------- End of function VgaDDraw::init_front ----------//


//-------- Begin of function VgaDDraw::init_back ----------//
//
// Create a direct draw back buffer.
//
// [DWORD] w      : width of the surface [default 0 : VGA_WIDTH]
// [DWORD] h      : height of the surface [default 0 : VGA_HEIGHT]
// [int] videoMemoryFlag : 1 for create surface in video memory [default 0 : in system memory]
//
int VgaDDraw::init_back(VgaBuf *b, DWORD w, DWORD h, int videoMemoryFlag)
{
	DDSURFACEDESC2       ddsd;
	Surface              *surface;

	//--------- fill in surface desc -----------//

	memset( &ddsd, 0, sizeof( ddsd ) );
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

	// create back buffer
	if( videoMemoryFlag & 1)
		ddsd.ddsCaps.dwCaps = 0;
	//	ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	ddsd.dwWidth  = w ? w : VGA_WIDTH;
	ddsd.dwHeight = h ? h : VGA_HEIGHT;

//	LPDIRECTDRAWSURFACE dd1Buf;
//	rc = ddPtr->CreateSurface( &ddsd, &dd1Buf, NULL );
	// CreateSurface can now create DIRECTDRAWSURFACE4
	surface = create_surface( &ddsd );
	if( !surface )
	{
		err.run( "Error creating direct draw back surface!" );
		return 0;
	}

//	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
//	if( rc != DD_OK )
//	{
//		dd1Buf->Release();
//		err.run( dd_err_str("Error creating direct draw back surface!!", rc) );
//	}
//	dd1Buf->Release();

	b->lock_bit_stack = 0;
	b->lock_stack_count = 0;

	b->default_remap_table = vga.default_remap_table;	// new for 16-bit

	b->init(surface, 0);
	return 1;
}
//-------- End of function VgaDDraw::init_back ----------//


//-------- Begin of function VgaDDraw::set_mode ----------//

int VgaDDraw::set_mode(int w, int h)
{
   HRESULT rc;

   //-------------- set Direct Draw mode ---------------//

   DEBUG_LOG("Attempt DirectDraw SetDisplayMode");
   // IDirectDraw2::SetDisplayMode requires 5 parameters
   rc = dd_obj->SetDisplayMode( w, h, VGA_BPP, 0, 0);
   DEBUG_LOG("DirectDraw SetDisplayMode finish");

   if( rc != DD_OK )
   {
      ERR("SetMode failed err=%d", rc);
		// ######## begin Gilbert 2/6 ########//
		// err.run( dd_err_str("SetMode failed ", rc) );
		err.msg( dd_err_str("SetMode failed ", rc) );
		// ######## end Gilbert 2/6 ########//
      return 0;
   }

	//----------- get the pixel format flag -----------//

   DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);

	pixel_format_flag = -1;

	if( dd_obj->GetDisplayMode(&ddsd) == DD_OK && ddsd.dwFlags & DDSD_PIXELFORMAT )
	{
		if( ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB
			&& ddsd.ddpfPixelFormat.dwRGBBitCount == (DWORD)VGA_BPP )
		{
			if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
				&& ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
				&& ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 10 )
			{
				pixel_format_flag = PIXFORM_RGB_555;
			}
			else if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
				&& ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
				&& ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 11 )
			{
				pixel_format_flag = PIXFORM_RGB_565;
			}
			else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
				&& ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
				&& ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 10 )
			{
				pixel_format_flag = PIXFORM_BGR_555;
			}
			else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
				&& ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
				&& ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 11 )
			{
				pixel_format_flag = PIXFORM_BGR_565;
			}
		}
	}

	// allow forcing display mode

	if( m.is_file_exist( "PIXMODE.SYS" ) )
	{
		File pixModeFile;
		pixModeFile.file_open( "PIXMODE.SYS" );

		char readBuffer[8];
		memset( readBuffer, 0, sizeof(readBuffer) );
		long readLen = 6;

		pixModeFile.file_read( readBuffer, readLen );

		if( strncmp( readBuffer, "RGB555", readLen ) == 0 )
		{
			pixel_format_flag = PIXFORM_RGB_555;
		}
		else if( strncmp( readBuffer, "RGB565", readLen ) == 0 )
		{
			pixel_format_flag = PIXFORM_RGB_565;
		}
		else if( strncmp( readBuffer, "BGR555", readLen ) == 0 )
		{
			pixel_format_flag = PIXFORM_BGR_555;
		}
		else if( strncmp( readBuffer, "BGR565", readLen ) == 0 )
		{
			pixel_format_flag = PIXFORM_BGR_565;
		}

		pixModeFile.file_close();
	}

	if( pixel_format_flag == -1 )
	{
		MessageBox(main_hwnd, "Cannot determine the pixel format of this display mode.",
			WIN_TITLE, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND );

		pixel_format_flag = PIXFORM_BGR_565;
	}

	// assembly functions to initalize effect processing

	INITeffect(pixel_format_flag);
	INITbright(pixel_format_flag);

   //----------- display the system cursor -------------//

   SetCursor(NULL);

   return 1;
}
//-------- End of function VgaDDraw::set_mode ----------//


//-------- Begin of function VgaDDraw::deinit ----------//

void VgaDDraw::deinit()
{
   DEBUG_LOG("Attempt vga_back.deinit()");
   vga_back.deinit();
   DEBUG_LOG("vga_back.deinit() finish");

   if( sys.use_true_front )
   {
      DEBUG_LOG("Attempt vga_true_front.deinit()");
      vga_true_front.deinit();
      DEBUG_LOG("vga_true_front.deinit() finish");
   }

   DEBUG_LOG("Attempt vga_front.deinit()");
   vga_front.deinit();
   DEBUG_LOG("Attempt vga_front.deinit() finish");

   release_pal();

   if( dd_obj )
   {
      //DEBUG_LOG("Attempt vga.dd_obj->RestoreDisplayMode()");
      // dd_obj->RestoreDisplayMode();
      //DEBUG_LOG("vga.dd_obj->RestoreDisplayMode() finish");

      DEBUG_LOG("Attempt vga.dd_obj->Release()");
      dd_obj->Release();
      DEBUG_LOG("vga.dd_obj->Release() finish");
      dd_obj = NULL;
   }

   destroy_window();
}
//-------- End of function VgaDDraw::deinit ----------//


// ------- begin of function VgaDDraw::is_inited --------//
//
bool VgaDDraw::is_inited()
{
	return vptr_dd_obj != NULL;
}
// ------- end of function VgaDDraw::is_inited --------//


//--------- Start of function VgaDDraw::init_pal ----------//
//
// Load the palette from a file and set it to the front buf.
//
int VgaDDraw::init_pal(const char* fileName)
{
   char palBuf[256][3];
   File palFile;

   palFile.file_open(fileName);
   palFile.file_seek(8);               // bypass the header info
   palFile.file_read(palBuf, 256*3);
   palFile.file_close();

    //--- Create a Direct Draw Palette and associate it with the front buffer ---//

   if( dd_pal == NULL )
   {
      for(int i=0; i<256; i++)
      {
         pal_entry_buf[i].peRed   = palBuf[i][0];
         pal_entry_buf[i].peGreen = palBuf[i][1];
         pal_entry_buf[i].peBlue  = palBuf[i][2];
      }

      HRESULT rc = dd_obj->CreatePalette( DDPCAPS_8BIT, pal_entry_buf, &dd_pal, NULL );

      if( rc != DD_OK )
         return 0;
   }

   init_color_table();
   init_gray_remap_table();

	// set global variable
	transparent_code_w = translate_color(TRANSPARENT_CODE);

   return 1;
}
//----------- End of function VgaDDraw::init_pal ----------//


//--------- Start of function VgaDDraw::init_color_table ----------//

void VgaDDraw::init_color_table()
{
   //----- initialize interface color table -----//

	PalDesc palDesc( (unsigned char*) pal_entry_buf, sizeof(PALETTEENTRY), 256, 8);
	vga_color_table->generate_table_fast( MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func );

	default_remap_table = (short *)vga_color_table->get_table(0);

   //----- initialize interface color table for blending -----//

	vga_blend_table->generate_table_fast( 8, palDesc, log_alpha_func );
	default_blend_table = (short *)vga_blend_table->get_table(0);
}
//----------- End of function VgaDDraw::init_color_table ----------//


//--------- Start of function VgaDDraw::release_pal ----------//

void VgaDDraw::release_pal()
{
   // ##### begin Gilbert 16/9 #######//
   if( dd_pal )
   {
      while( dd_pal->Release() );
      dd_pal = NULL;
   }
   // ##### end Gilbert 16/9 #######//
}
//----------- End of function VgaDDraw::release_pal ----------//


//-------- Begin of function VgaDDraw::adjust_brightness ----------//
//
// <int> changeValue - the value to add to the RGB values of
//                     all the colors in the palette.
//                     the value can be from -255 to 255.
//
// <int> preserveContrast - whether preserve the constrast or not
//
void VgaDDraw::adjust_brightness(int changeValue)
{
   //---- find out the maximum rgb value can change without affecting the contrast ---//

   int          i;
   int          newRed, newGreen, newBlue;
   PALETTEENTRY palBuf[256];

   //------------ change palette now -------------//

   for( i=0 ; i<256 ; i++ )
   {
      newRed   = (int)pal_entry_buf[i].peRed   + changeValue;
      newGreen = (int)pal_entry_buf[i].peGreen + changeValue;
      newBlue  = (int)pal_entry_buf[i].peBlue  + changeValue;

      palBuf[i].peRed   = MIN(255, MAX(newRed,0) );
      palBuf[i].peGreen = MIN(255, MAX(newGreen,0) );
      palBuf[i].peBlue  = MIN(255, MAX(newBlue,0) );
   }

   //------------ set palette ------------//

   vga_front.temp_unlock();

   dd_pal->SetEntries( 0, 0, 256, palBuf );

   vga_front.temp_restore_lock();
}
//--------- End of function VgaDDraw::adjust_brightness ----------//


//----------- Begin of function VgaDDraw::flip ----------//
void VgaDDraw::flip()
{
#if(defined(USE_FLIP))

	mouse_cursor.before_flip();

	vga_front.temp_unlock();
	vga_back.temp_unlock();

	vga_front.dd_buf->Flip(NULL, DDFLIP_WAIT );

	vga_back.temp_restore_lock();
	vga_front.temp_restore_lock();

	mouse_cursor.after_flip();
#endif
}
//----------- End of function VgaDDraw::flip ----------//


//----------- Begin of function VgaDDraw::init_gray_remap_table ----------//
//
// Initialize a gray remap table for VgaBuf::convert_gray to use.
//
void VgaDDraw::init_gray_remap_table()
{
   //------ create a color to gray-scale remap table ------//

   #define FIRST_GRAY_COLOR   0x90
   #define GRAY_SCALE_COUNT   16    // no. of gray colors

// #define FIRST_GRAY_COLOR   0x96
// #define GRAY_SCALE_COUNT   10    // no. of gray colors

   PALETTEENTRY* palEntry = vga.pal_entry_buf;
   int i, grayIndex;

   for( i=0 ; i<256 ; i++, palEntry++ )
   {
      //--------------------------------------------------------//
      //
      // How to calculate the gray index (0-31)
      //
      // formula is : grey = red * 0.3 + green * 0.59 + blue * 0.11
      //              the range of the result value is 0-255
      //              this value is then divided by 8 to 0-31
      //
      //--------------------------------------------------------//

      grayIndex = ((int)palEntry->peRed * 30 + (int)palEntry->peGreen * 59 +
                   (int)palEntry->peBlue * 11) / 100 / (256/GRAY_SCALE_COUNT);

      gray_remap_table[i] = FIRST_GRAY_COLOR + grayIndex;
   }
}
//--------- End of function VgaDDraw::init_gray_remap_table -----------//


//----------- Begin of function VgaDDraw::create_surface ----------//
//
// Create a DirectDraw Surface
//
// On success, the pointer to the surface is returned. The caller is
// responsible for releasing the surface when done.
//
// On failure, the return is NULL.
//
Surface* VgaDDraw::create_surface(LPDDSURFACEDESC2 ddsd)
{
   LPDIRECTDRAWSURFACE4 dd_buf;
   HRESULT rc;

   rc = dd_obj->CreateSurface( ddsd, &dd_buf, NULL );
   if( rc != DD_OK )
      return NULL;

   return new Surface(dd_buf);
}
//--------- End of function VgaDDraw::create_surface -----------//


int VgaDDraw::make_pixel(BYTE red, BYTE green, BYTE blue)
{
	// ##### begin Gilbert 19/10 #######//
	return IMGmakePixel( (blue << 16) + (green << 8) + red);
	// ##### end Gilbert 19/10 #######//
}

int VgaDDraw::make_pixel(RGBColor *rgb)
{
	int u;
	memcpy(&u, rgb, sizeof(RGBColor));
	return IMGmakePixel(u);
}

void VgaDDraw::decode_pixel(int p, RGBColor *rgb)
{
	int u = IMGdecodePixel(p);
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


//-------- Begin of static function main_win_proc --------//
//
// Callback for all Windows messages
//
long FAR PASCAL main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch( message )
   {
      case WM_CREATE:
         vga.main_hwnd = hWnd;
         break;

      case WM_ACTIVATEAPP:
         sys.active_flag = (BOOL)wParam && !IsIconic(hWnd);

         //--------------------------------------------------------------//
         // while we were not-active something bad happened that caused us
         // to pause, like a surface restore failing or we got a palette
         // changed, now that we are active try to fix things
         //--------------------------------------------------------------//

         if( sys.active_flag )
         {
            sys.unpause();
            sys.need_redraw_flag = 1;      // for Sys::disp_frame to redraw the screen
         }
         else
            sys.pause();
         break;

      // ##### begin Gilbert 31/10 #####//
      case WM_PAINT:
         sys.need_redraw_flag = 1;
         break;
      // ##### end Gilbert 31/10 #####//

      case WM_DESTROY:
         vga.main_hwnd = NULL;
         sys.deinit_directx();
         PostQuitMessage( 0 );
         break;

      default:
         break;
   }

   return DefWindowProc(hWnd, message, wParam, lParam);
}
//--------- End of static function main_win_proc ---------//
