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

//Filename    : OF_BASEI.CPP
//Description : Firm Base - interface functions

#include <of_base.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <oimgres.h>
#include <ohelp.h>
#include <ostr.h>
#include <ofont.h> 
#include <oconfig.h>
#include <ounit.h>
#include <obutt3d.h>
#include <onation.h>
#include <ogodres.h>
#include <oremote.h>
#include <oseres.h>
#include <ose.h>
#include <obitmapw.h>
#include <osys.h>
#include <oraceres.h>
#include <ot_firm.h>

//----------- Define static vars -------------//

static Button3D   button_invoke, button_unknown_effect;
static Button3D   button_clear_text;
static short edit_pray_x1, edit_pray_y1, edit_pray_x2, edit_pray_y2, edit_pray_enable;

//--------- Begin of function FirmBase::disp_god_info ---------//
//
void FirmBase::disp_god_info(int dispY1, int refreshFlag)
{
	//-------- display the icon of the mythical creature -------//

	int  x=INFO_X1, y=INFO_Y1;

	if( refreshFlag == INFO_REPAINT )
	{
		invoked_effect_text[0] = '\0';	// clear text not to show invoked effect text
	}

	String str;

	short*	hitPointBitmap =NULL;
	int ratio = (int)pray_points *40 / (int)MAX_PRAY_POINTS;
	int size = (int)pray_points *76 / (int)MAX_PRAY_POINTS;
		
	//106 x 35 --- 15 to 90 ie. 0 to 40
	hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	
	if (ratio <11)
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 -13, INFO_Y1 +49 +48, image_spict.read("MTR_10"));
	else
	if (ratio <40)
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 -13, INFO_Y1 +49 +48, image_spict.read("MTR_39"));
	else
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 -13, INFO_Y1 +49 +48, image_spict.read("MTR_40"));
		
	vga.active_buf->read_bitmapW( INFO_X1 +80 -13, INFO_Y1 +49 +48, INFO_X1 +94 +size -13, INFO_Y1 +80 +48, hitPointBitmap );
	vga.active_buf->put_bitmap_trans( INFO_X1 +80 -13, INFO_Y1 +49 +48, image_spict.read("MTR_00"));
	vga.active_buf->put_bitmapW( INFO_X1 +80 -13, INFO_Y1 +49 +48, hitPointBitmap );
	mem_del( hitPointBitmap );
/*
	str = (int)pray_points;
	str += " / ";
	str += (int)MAX_PRAY_POINTS;

	font_snds.center_put( INFO_X1 +80 -13, INFO_Y1 +46 +48, INFO_X1 +187 -13, INFO_Y1 +63 +48, str );
*/

	char *baseDesc;

	if( should_show_info() && invoked_effect_text[0] )
	{
		baseDesc = invoked_effect_text;
	}
	else
	{
		baseDesc = text_firm.str_base_prayer_effect( god_id );
	}
		
	font_snds.center_put_paragraph( INFO_X1+33, INFO_Y1, INFO_X2-22, INFO_Y1+115, baseDesc, -2, 0, 0 );
}
//----------- End of function FirmBase::disp_god_info -----------//


//--------- Begin of function FirmBase::put_info ---------//
//
void FirmBase::put_info(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("GBBASE") );

	FirmWork::put_info(refreshFlag);
}
//----------- End of function FirmBase::put_info -----------//


//--------- Begin of function FirmBase::detect_info ---------//
//
void FirmBase::detect_info()
{
	FirmWork::detect_info();
}
//----------- End of function FirmBase::detect_info -----------//


// -------- begin of function FirmBase::disp_firm_info --------//
//
void FirmBase::disp_firm_info(int dispY1, int refreshFlag)
{
   disp_god_info(INFO_Y1+226, refreshFlag);

	//------ display button -------//

	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	if( refreshFlag == INFO_REPAINT )
	{
		button_clear_text.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}
			
	if( !is_own() )
		return;

	x2 += BUTTON_DISTANCE;

	char fileName[] = "GB-AX";
	char i = (char) (sys.frame_count % 9);
	if (i > 4)
		i = 8 - i;
	fileName[4] = '1' + i;
	
	if( (!can_invoke()) && (god_unit_recno == 0) )
	{
		button_invoke.paint( x2, y2, 'A', "INVOKE" );
		button_invoke.set_help_code("GDINVOKE");
		button_invoke.disable();
	}
	else
	{
		if( !god_unit_recno )
		{
			button_invoke.paint( x2, y2-12, 'A', "GB-A1" );
			button_invoke.set_help_code("GDINVOKE");
		}
		else
		{
			button_invoke.paint( x2, y2-12, 'A', fileName );
			button_invoke.set_help_code("GDINVOKE");
		}
		button_invoke.enable();
	}		

	x2 += BUTTON_DISTANCE;
	
	i = (char) (sys.frame_count % 9);
	if (i > 4)
		i = 8 - i;
	fileName[4] = '1' + i;
	
	fileName[3] = 'B';
	fileName[4] = '1' + i;

	if( (!can_make_wish()) && (invoked_effect_id == 0) )
	{
		button_unknown_effect.paint( x2, y2, 'A', "INVOKE" );
		button_unknown_effect.set_help_code("BLESS");
		button_unknown_effect.disable();
	}
	else
	{
		if ( !invoked_effect_id )
		{
			button_unknown_effect.paint( x2, y2-15, 'A', "GB-B1" );
			button_unknown_effect.set_help_code("BLESS");
			button_unknown_effect.enable();
		}
		else
		{ 
			button_unknown_effect.paint( x2, y2-15, 'A', fileName );
			button_unknown_effect.set_help_code("BLESS");
			button_unknown_effect.enable();
		}
	}

	// ----- display clear invoked_effect_text button -----//

	button_clear_text.visible_flag = invoked_effect_text[0];	// displaying invoked effect text
	button_clear_text.paint();
}
// -------- end of function FirmBase::disp_firm_info --------//


// -------- begin of function FirmBase::detect_firm_info --------//
//
void FirmBase::detect_firm_info()
{
	if( !is_own() )
		return;

	//----------- detect invoke -----------//

	if( button_invoke.detect() && can_invoke() )
	{
		if(remote.is_enable())
		{
			// packet structure : <firm recno>
			short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_BASE_INVOKE_GOD, sizeof(short));
			shortPtr[0] = firm_recno;
		}
		else
		{
			invoke_god();
			// ##### patch begin Gilbert 21/10 #######//
			return;		// call selecting new god now, return immediately
			// ##### patch end Gilbert 21/10 #######//
		}
	}

	// ---------- detect unknown effect -----//

	if( button_unknown_effect.detect() && can_make_wish() )
	{
		invoke_random_effect( 0, COMMAND_PLAYER);
		invoked_effect_last = 3;
		se_ctrl.immediate_sound("TURN_ON");
	}

	// ----- detect clear invoked_effect_text button -----//

	if( button_clear_text.detect() )
	{
		invoked_effect_text[0] = '\0';
		se_ctrl.immediate_sound("TURN_ON");
	}
 
//	help.set_help( INFO_X1+13+54, INFO_Y1+261, INFO_X1+13+53+44, INFO_Y1+280+48, "GDINVOKE" );
//	help.set_help( INFO_X1+13+108, INFO_Y1+261, INFO_X1+13+107+44, INFO_Y1+280+48, "BLESS" );
}
// -------- end of function FirmBase::disp_firm_info --------//


// --------- begin of function FirmBase::disp_edit_mode -------//
//
void FirmBase::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	FirmWork::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit pray points

	edit_pray_enable = 1;
	x = font_whbl.put( x1, y, text_firm.str_pray_points(), 0, x2 );
	x += 20;
	x = edit_pray_x2 = font_blue.put( (edit_pray_x1=x), (edit_pray_y1=y), (int)pray_points, 0, x2 );
	edit_pray_y2 = edit_pray_y1+font_blue.max_font_height;
	y += 18;
}
// --------- end of function FirmBase::disp_edit_mode -------//


// --------- begin of function FirmBase::detect_edit_mode -------//
//
int FirmBase::detect_edit_mode()
{
	int rc;
	if( (rc = FirmWork::detect_edit_mode()) )
		return rc;

	// edit pray points

	if( edit_pray_enable )
	{
		if( mouse.any_click(edit_pray_x1, edit_pray_y1, edit_pray_x2, edit_pray_y2, LEFT_BUTTON) )
		{
			pray_points += 50.0f;
			if( pray_points > MAX_PRAY_POINTS )
				pray_points = MAX_PRAY_POINTS;
			return 1;
		}
		if( mouse.any_click(edit_pray_x1, edit_pray_y1, edit_pray_x2, edit_pray_y2, RIGHT_BUTTON) )
		{
			pray_points -= 50.0f;
			if( pray_points < 0.0f )
				pray_points = 0.0f;
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmBase::detect_edit_mode -------//
