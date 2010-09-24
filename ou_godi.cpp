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

//Filename   : OU_GODI.CPP
//Description: God Unit

#include <OVGA.H>
#include <OU_GOD.H>
#include <OSYS.H>
#include <OMOUSE.H>
#include <OGAME.H>
#include <OMODEID.H>
#include <OBUTT3D.H>
#include <OGODRES.H>
#include <OF_BASE.H>
#include <ORACERES.H>
#include <OPOWER.H>
#include <OWEATHER.H>
#include <OTORNADO.H>
#include <OIMGRES.H>
#include <OFONT.H>
#include <OBITMAPW.H>
#include <OHELP.H>
#include <OT_UNIT.H>

//--------- Define static vars ----------//

static Button3D button_cast;


//--------- Begin of function UnitGod::disp_info ---------//
//
void UnitGod::disp_info(int refreshFlag)
{
#ifdef USE_FLIP
	vga.use_back();
#endif

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("GBACT") );

	disp_hit_point();
		
	disp_unit_profile( 1, refreshFlag );

	err_when( god_id <= 0 || god_id > MAX_GOD );
//	font_snds.center_put_paragraph( INFO_X1+33, INFO_Y1, INFO_X2-22, INFO_Y1+115, str, -2, 0, 0 );
	font_snds.center_put_paragraph( INFO_X1+33, INFO_Y1, INFO_X2-22, INFO_Y1+180, 
		text_unit.str_god_desc(god_id), // str,
		-2, 0, 0 );

	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	// ####### begin Gilbert 26/2 #######//

	if( refreshFlag == INFO_REPAINT )
	{
		button_cast.create( x2+BUTTON_DISTANCE, y2-22, 'A', "GBA-1" );
	}

	if( is_own() )
	{
//		if( game.game_mode != GAME_TEST )
//		{
			if( god_res[god_id]->can_cast_power )
			{
				x2 += BUTTON_DISTANCE;
				if( hit_points >= god_res[god_id]->power_pray_points )
				{
					char fileName[] = "GBA-X";
					fileName[4] = '1' + (char) (sys.frame_count%3);
					button_cast.enable_flag = 1;
					button_cast.update_bitmap( fileName );
				}
				else
				{
					// can be any one of the GBA-1,GBA-2,GBA-3
					button_cast.enable_flag = 0;
					button_cast.paint();
				}
			}
//		}
	}
	// ####### end Gilbert 26/2 #######//

#ifdef USE_FLIP
	vga.use_front();
#endif
}
//---------- End of function UnitGod::disp_info ----------//


//--------- Begin of function UnitGod::detect_info ---------//
//
void UnitGod::detect_info()
{
	if( detect_unit_profile() )
		return;

	if( !is_own() )
		return;

	if( god_res[god_id]->can_cast_power )
	{
		button_cast.set_help_code( "MAGIC" );	// update_bitmap will change help code
		if( button_cast.detect() )
			power.issue_command(COMMAND_GOD_CAST_POWER, sprite_recno);
	}
}
//---------- End of function UnitGod::detect_info ----------//

//----------- Begin of function Unit::disp_hit_point -----------//
void UnitGod::disp_hit_point()
{
	int hitPoints;
	String str;

	if( hit_points > (float)0 && hit_points < (float)1 )
		hitPoints = 1;		// display 1 for value between 0 and 1
	else
		hitPoints = (int) hit_points;

	if ( max_hit_points() ) 
	{
		short*	hitPointBitmap =NULL;
		int ratio = hitPoints *40 / (int)max_hit_points();
		int size = hitPoints *76 / (int)max_hit_points();
		
		//106 x 35 --- 15 to 90 ie. 0 to 40
		hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	
		if (ratio <11)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 -15, INFO_Y1 +49 +128, image_spict.read("MTR_10"));
		else
		if (ratio <40)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 -15, INFO_Y1 +49 +128, image_spict.read("MTR_39"));
		else
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 -15, INFO_Y1 +49 +128, image_spict.read("MTR_40"));
		
		vga.active_buf->read_bitmapW( INFO_X1 +80 -15, INFO_Y1 +49 +128, INFO_X1 +94 +size -15, INFO_Y1 +80 +128, hitPointBitmap );
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 -15, INFO_Y1 +49 +128, image_spict.read("MTR_00"));
		vga.active_buf->put_bitmapW( INFO_X1 +80 -15, INFO_Y1 +49 +128, hitPointBitmap );
		mem_del( hitPointBitmap );

		str = (int)hitPoints;
		font_snds.center_put( INFO_X1 +43, INFO_Y1 +190, INFO_X1 +65, INFO_Y1 +202, str );
		str = (int)max_hit_points();
		font_snds.center_put( INFO_X1 +172, INFO_Y1 +190, INFO_X1 +194, INFO_Y1 +202, str );
	}
}
//----------- End of function Unit::disp_hit_point -----------//



// ------ begin of function Unit::send_hotkey --------//
//
// send hot key during sys::process_key
// pass keycode to button.detect() is not good becase
//  in report mode (especially chat mode), button.detect() should not detect keyboard
//
int UnitGod::send_hotkey(unsigned scanCode, unsigned skeyState)
{
	err_when( sprite_recno != unit_array.selected_recno );		// must be the main selected

	int keyCode;

	if( (keyCode = mouse.is_key(scanCode, skeyState, (WORD) 0, 0)) )
	{
		switch( keyCode )
		{
		case 'z':
			if( is_own() && button_cast.visible_flag && button_cast.enable_flag )
			{
				// put the action of button_cast.detect() here
				power.issue_command(COMMAND_GOD_CAST_POWER, sprite_recno);
				return 1;
			}
			break;
		}
	}

	return Unit::send_hotkey(scanCode, skeyState);
}
// ------ end of function Unit::send_hotkey --------//
