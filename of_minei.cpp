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

//Filename    : OF_MINEI.CPP
//Description : Firm Mine - interface functions

#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OSTR.H>
#include <OFONT.H>
#include <ONEWS.H>
#include <OUNIT.H>
#include <ORACERES.H>
#include <ONATION.H>
#include <OSITE.H>
#include <OF_MINE.H>
#include <OF_FACT.H>
#include <OIMGRES.H>
#include <OCONFIG.H>
#include <OT_FIRM.H>

static int animate_seqno =1;
static short edit_reserve_x1, edit_reserve_y1, edit_reserve_x2, edit_reserve_y2, edit_reserve_enable;
static short edit_stock_x1, edit_stock_y1, edit_stock_x2, edit_stock_y2, edit_stock_enable;

//--------- Begin of function FirmMine::put_info ---------//
//

void FirmMine::put_info(int refreshFlag)
{
	// #### begin Gilbert 5/10 ######//
	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MIFABASE") );

	FirmWork::put_info(refreshFlag);
	/*
	disp_basic_info(INFO_Y1, refreshFlag);

	if( !should_show_info() )
		return;

	disp_mine_info(INFO_Y1+52, refreshFlag);
	disp_worker_list(INFO_Y1+127, refreshFlag);
	disp_worker_info(INFO_Y1+191, refreshFlag);

	//---------- display spy button ----------//

	disp_spy_button(INFO_X1, INFO_Y1+249, refreshFlag);
	*/

}
//----------- End of function FirmMine::put_info -----------//


//--------- Begin of function FirmMine::detect_info ---------//
//
void FirmMine::detect_info()
{
	// ##### begin Gilbert 5/10 ######//

	FirmWork::detect_info();

	/*
	//-------- detect basic info -----------//

	if( detect_basic_info() )
		return;

	//----------- detect worker -----------//

	if( detect_worker_list() )
	{
		disp_mine_info(INFO_Y1+52, INFO_UPDATE);
		disp_worker_info(INFO_Y1+191, INFO_UPDATE);
	}

	//-------- detect spy button ----------//

	detect_spy_button();

	if( !is_own() )
		return;

*/
	// ##### end Gilbert 5/10 ######//

}
//----------- End of function FirmMine::detect_info -----------//

/*
//--------- Begin of function FirmMine::disp_mine_info ---------//
//
void FirmMine::disp_mine_info(int dispY1, int refreshFlag)
{
	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+70);

	//------ if there is no natural resource on this location ------//

	if( !raw_id )
	{
		font_san.center_put( INFO_X1, dispY1, INFO_X2, dispY1+70, "No Natural Resources" );
		return;
	}

	//-------------- display mining info -----------//

	int x=INFO_X1+4, y=dispY1+4;

	raw_res.put_small_raw_icon( x+1, y+1, raw_id );

	String str;

	str  = translate.process("Mining ");
	str += raw_res[raw_id]->name;

	font_san.disp( x+20, y, str, INFO_X2-2);
	y+=16;

	font_san.field( x, y, "Monthly Production", x+126, (int) production_30days(), 1, INFO_X2-2, refreshFlag, "MN_PROD");
	y+=16;

	str  = (int) stock_qty;
	str += " / ";
	str += (int) max_stock_qty;

	font_san.field( x, y, "Mined Stock", x+126, str, INFO_X2-2, refreshFlag, "MN_STOCK");
	y+=16;

	font_san.field( x, y, "Untapped Reserve", x+126, (int) reserve_qty, 1, INFO_X2-2, refreshFlag, "MN_UNTAP");
}
//----------- End of function FirmMine::disp_mine_info -----------//
*/


//------- Begin of function FirmMine::draw -----------//
//
// Draw raw materials stocks.
//
void FirmMine::draw(int displayLayer)
{
	Firm::draw(displayLayer);

	if( !should_show_info() )
		return;

	if( under_construction )
		return;

	if( raw_id && displayLayer == 1)
	{
		int cargoCount = MAX_CARGO	* (int)stock_qty / (int)max_stock_qty;

		// draw_cargo( max(1,cargoCount), raw_res.small_raw_icon(raw_id) );
	}
}
//--------- End of function FirmMine::draw -----------//


// -------- begin of function FirmMine::disp_firm_info --------//
//
void FirmMine::disp_firm_info(int dispY1, int refreshFlag)
{
	//---------- display production info -------------//
	// String str; //, str2;

	if (raw_id == 0)
	{
		// str = "No Natural Resources near !! You cannot mine in here.";
		font_snds.put_paragraph( INFO_X1+20, INFO_Y1+38, INFO_X2-5, INFO_Y2-5, 
			text_firm.str_mine_no_raw() );
	}
	else
	{
//		font_snds.center_put( INFO_X1+10, INFO_Y1+5, INFO_X1+96, INFO_Y1+30, 
//			"Unmined Raw", 0, 1 );
		font_snds.put( INFO_X1+14, INFO_Y1+12, text_firm.str_unmined_raw(), 0, -1, 1 );
		
//		font_snds.center_put( INFO_X2-86+12, INFO_Y1+5, INFO_X2, INFO_Y1+30, 
//			text_firm.str_mined_raw(), // "Mined Raw",
//			0, 1 );
		font_snds.right_put( INFO_X2-10, INFO_Y1+12, text_firm.str_mined_raw(), 0, 1 );

		font_snds.center_put( INFO_X1+10, INFO_Y1+20, INFO_X1+96, INFO_Y1+45, 
			raw_res[raw_id]->name, 0, 1 );
		font_snds.center_put( INFO_X2-86+12, INFO_Y1+20, INFO_X2, INFO_Y1+45,
			raw_res[raw_id]->name, 0, 1 );

	//	str  = (int) stock_qty;
	//	str += " / ";
	//	str += (int) max_stock_qty;
		if (stock_qty >= max_stock_qty)
			vga.active_buf->put_bitmap( INFO_X2-64+6, INFO_Y1+40+15, image_icon.read("FULLVILL") );	
		font_snds.center_put( INFO_X2-86+12, INFO_Y1+40, INFO_X2, INFO_Y1+55, 
			m.format((int) stock_qty) ); // str );

//		str  = (int) reserve_qty;
//		if ( reserve_qty <= 0 )
//			vga.active_buf->put_bitmap( INFO_X1+33, INFO_Y1+40+15, image_icon.read("FULLVILL") );
		font_snds.center_put( INFO_X1+10, INFO_Y1+40, INFO_X1+96, INFO_Y1+55, 
			m.format((int) reserve_qty) ); // str );

		if ( (int) production_30days() > 0 )
	//	if ( is_operating() )
		{
			char animate[] = "PICK-0";
			if ( animate_seqno >0 )
				animate[5] = '2';
			if ( animate_seqno >1 )
				animate[5] = '1';

			if (config.frame_speed != 0)
				animate_seqno ++;

			int limit;
			if ( (int) production_30days() >  0 && (int) production_30days() <= 20 )
				limit = 7;
			if ( (int) production_30days() > 20 && (int) production_30days() <= 50 )
				limit = 6;
			if ( (int) production_30days() > 50 && (int) production_30days() <= 90 )
				limit = 5;
			if ( (int) production_30days() > 90 && (int) production_30days() <=140 )
				limit = 4;
			if ( (int) production_30days() >140 && (int) production_30days() <=200 )
				limit = 3;
			if ( (int) production_30days() >200 )
				limit = 2;

			if ( animate_seqno >limit )
				animate_seqno = 1;

			vga.active_buf->put_bitmap( INFO_X1+93, INFO_Y1+36, image_icon.read( animate ) );
		}
		else
			vga.active_buf->put_bitmap( INFO_X1+93, INFO_Y1+36, image_icon.read( "PICK-1" ) );

		if (raw_id == RAW_CLAY)
		{
			vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "CLY-RW_L" ) );
			vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "CLY-RW_R" ) );
		}
		else
		if (raw_id == RAW_COPPER)
		{
			vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "COP-RW_L" ) );
			vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "COP-RW_R" ) );
		}
		else
		if (raw_id == RAW_IRON)
		{
			vga.active_buf->put_bitmap( INFO_X1+20, INFO_Y1+88, image_icon.read( "IRN-RW_L" ) );
			vga.active_buf->put_bitmap_trans( INFO_X1+164, INFO_Y1+83, image_icon.read( "IRN-RW_R" ) );
		}
		else
			err_here();

//		str  = "Monthly";
//		font_snds.center_put( INFO_X1+17, INFO_Y1+90, INFO_X2, INFO_Y1+115, str );
//		str  = "Production";
//		font_snds.center_put( INFO_X1+17, INFO_Y1+106, INFO_X2, INFO_Y1+121, str );
		font_snds.center_put_paragraph( INFO_X1+72, INFO_Y1+85, INFO_X1+169, INFO_Y1+121,
			text_firm.str_monthly_production(), 0, 0, 0 );

//		str = (int) production_30days();
//		font_snds.center_put( INFO_X1+17, INFO_Y1+117, INFO_X2, INFO_Y1+137, str );
		font_snds.center_put( INFO_X1+17, INFO_Y1+117, INFO_X2, INFO_Y1+137,
			m.format((int) production_30days()) );
	}
}
// -------- end of function FirmMine::disp_firm_info --------//


// -------- begin of function FirmMine::detect_firm_info --------//
//
void FirmMine::detect_firm_info()
{
}
// -------- end of function FirmMine::disp_firm_info --------//


// --------- begin of function FirmMine::disp_edit_mode -------//
//
void FirmMine::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	FirmWork::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// display raw type

	font_whbl.put( x1, y, text_firm.str_mine_info(raw_id) );
	y += 18;

	// edit reserved quantity

	edit_reserve_enable = raw_id && !site_array.is_deleted(site_recno);
	if( edit_reserve_enable )
	{
		x = font_whbl.put( x1, y, text_firm.str_unmined_raw(), 0, x2 );
		x += 20;
		x = edit_reserve_x2 = font_blue.put( (edit_reserve_x1=x), (edit_reserve_y1=y), (int)reserve_qty, 0, x2 );
		edit_reserve_y2 = edit_reserve_y1+font_blue.max_font_height;
	}
	y += 18;

	// display stock quantity

	edit_stock_enable = (raw_id != 0);
	if( edit_stock_enable )
	{
		x = font_whbl.put( x1, y, text_firm.str_mined_raw(), 0, x2 );
		x += 20;
		x = edit_stock_x2 = font_blue.put( (edit_stock_x1=x), (edit_stock_y1=y), (int)stock_qty, 0, x2 );
		edit_stock_y2 = edit_stock_y1+font_blue.max_font_height;
	}
	y += 18;
}
// --------- end of function FirmMine::disp_edit_mode -------//


// --------- begin of function FirmMine::detect_edit_mode -------//
//
int FirmMine::detect_edit_mode()
{
	int rc;
	if( (rc = FirmWork::detect_edit_mode()) )
		return rc;

	// detect reserved quantity

	if( edit_reserve_enable && !site_array.is_deleted(site_recno) )
	{
		Site *sitePtr = site_array[site_recno];
		if( mouse.any_click(edit_reserve_x1, edit_reserve_y1, edit_reserve_x2, edit_reserve_y2, LEFT_BUTTON) )
		{
			reserve_qty += 1000.0f;
			if( reserve_qty > (float)MAX_RAW_RESERVE_QTY )
				reserve_qty = (float)MAX_RAW_RESERVE_QTY;
			sitePtr->reserve_qty = (int)reserve_qty;		// update site
			return 1;
		}
		if( mouse.any_click(edit_reserve_x1, edit_reserve_y1, edit_reserve_x2, edit_reserve_y2, RIGHT_BUTTON) )
		{
			if( reserve_qty >= 1.0f )		// avoid decrease less than zero
			{
				reserve_qty -= 1000.0f;
				if( reserve_qty < 1.0f )
					reserve_qty = 1.0f;
				sitePtr->reserve_qty = (int)reserve_qty;		// update site
			}
			return 1;
		}
	}

	// detect stock quantity

	if( edit_stock_enable )
	{
		if( mouse.any_click(edit_stock_x1, edit_stock_y1, edit_stock_x2, edit_stock_y2, LEFT_BUTTON) )
		{
			stock_qty += 100.0f;
			if( stock_qty > max_stock_qty )
				stock_qty = max_stock_qty;
			return 1;
		}
		if( mouse.any_click(edit_stock_x1, edit_stock_y1, edit_stock_x2, edit_stock_y2, RIGHT_BUTTON) )
		{
			stock_qty -= 100.0f;
			if( stock_qty < 0.0f )
				stock_qty = 0.0f;
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmMine::detect_edit_mode -------//
