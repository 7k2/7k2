// Filename     : OF_MAGII.CPP
// Description : magic tower interface function


#include <OF_MAGI.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OFONT.H>
#include <OIMGRES.H>
#include <OBUTT3D.H>
#include <OBUTTCUS.H>
#include <OHELP.H>
#include <OINFO.H>
#include <OPOWER.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OWORLDMT.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OBITMAPW.H>
#include <OBITMAP.H>
#include <OSE.H>
#include <OSERES.H>
#include <OT_FIRM.H>

// ------- define static variable ------//

static char menu_mode;
static char last_menu_mode;
static char menu_mode_para;

// Button3D button_magic[MAGIC_COUNT-2];
static ButtonCustom button_magic[MAGIC_COUNT]; //bug
static Button3D button_cancel;

static short edit_energy_x1, edit_energy_y1, edit_energy_x2, edit_energy_y2, edit_energy_enable;

// ------- declare static function -------//

static void disp_magic_button(ButtonCustom *, int);


void FirmMagic::put_info(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
		last_menu_mode = menu_mode = FIRM_MAGIC_MENU_MAIN;
	else if( last_menu_mode != menu_mode )
	{
		refreshFlag = INFO_REPAINT;
		last_menu_mode = menu_mode;
	}

	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_MAIN:
		disp_main_menu(refreshFlag);
		break;

	case FIRM_MAGIC_MENU_CAST:
	case FIRM_MAGIC_MENU_UNIT:
		disp_magic_menu(refreshFlag);
		break;

	default:
		err_here();
	}
}

void FirmMagic::detect_info()
{
	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_MAIN:
		detect_main_menu();
		break;

	case FIRM_MAGIC_MENU_CAST:
	case FIRM_MAGIC_MENU_UNIT:
		detect_magic_menu();
		break;

	default:
		err_here();
	}
}


void FirmMagic::disp_main_menu(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	int i;

	if( refreshFlag == INFO_REPAINT )
	{
		for(i = magic_id_lower_limit-1; i < magic_id_upper_limit; ++i ) //bug
		{
			//button_magic[i].create( INFO_X1+13 + i * BUTTON_DISTANCE, INFO_Y1+235, 'A', "INVOKE" );
			button_magic[i].create( INFO_X1+23, INFO_Y1+80+20*i, INFO_X1+210, INFO_Y1+80+20*i+17,
				disp_magic_button, ButtonCustomPara( this, i+1 ) );
			button_magic[i].visible_flag = 0;
			button_magic[i].enable_flag = 0;
		}
	}

	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	// display delay

	if( cast_delay > 0 )
	{
		short*	hitPointBitmap =NULL;
		int ratio = cast_delay *40 / cast_delay_max;
		int size = cast_delay *76 / cast_delay_max;
		
		//106 x 35 --- 15 to 90 ie. 0 to 40
		int offsetX = -35;
		int offsetY = -40;
		hitPointBitmap = (short *)mem_add( BitmapW::size(15 +size, 35) );	
		if (ratio <11)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_10"));
		else
		if (ratio <40)
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_39"));
		else
			vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_40"));
		
		vga.active_buf->read_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, INFO_X1 +94 +20 +size +offsetX, INFO_Y1 +80 +offsetY, hitPointBitmap );
		vga.active_buf->put_bitmap_trans( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, image_spict.read("MTR_00"));
		vga.active_buf->put_bitmapW( INFO_X1 +80 +20 +offsetX, INFO_Y1 +49 +offsetY, hitPointBitmap );
		mem_del( hitPointBitmap );				
	}

	if( !is_own() )
		return;

	for(i = magic_id_lower_limit-1; i < magic_id_upper_limit; ++i ) //bug
	{
		button_magic[i].visible_flag = (nation_recno && 
			(tech_res[magic_info_array[i].tech_id]->get_nation_tech_level(nation_recno) ||
			 magic_info_array[i].tech_id == 0)) ;
		button_magic[i].enable_flag = can_cast(i+1);

		button_magic[i].paint();
	}
}

void FirmMagic::detect_main_menu()
{
	int i;

	Firm::detect_info();

	if( !is_own() )
		return;

	String str;

	for(i = magic_id_lower_limit-1; i < magic_id_upper_limit; ++i ) //bug
	{
		if( button_magic[i].detect() )
		{
			se_ctrl.immediate_sound("TURN_ON");
			menu_mode_para = i+1;		// magicId
			teleport_unit_recno = 0;

			switch(i+1)
			{
			case MAGIC_DESTROY_BUILDING:
			case MAGIC_FREEZE_UNIT:
			case MAGIC_DAMAGE_AREA:
			case MAGIC_GOD_POWER:
				menu_mode = FIRM_MAGIC_MENU_CAST;
				power.issue_command(COMMAND_FIRM_MAGIC, firm_recno, menu_mode_para);
				break;

			case MAGIC_TELEPORT:
				menu_mode = FIRM_MAGIC_MENU_UNIT;
				power.issue_command(COMMAND_MAGIC_TELEPORT_PRE, firm_recno, menu_mode_para);
				break;

			case MAGIC_FREEZE_LASER:
				cast_magic( MAGIC_FREEZE_LASER, center_x, center_y, 0, COMMAND_PLAYER );

			default:
				err_here();
			}
		}

		if( button_magic[i].visible_flag )
		{
		//	str = "minimun life points cost : ";
		//	str += magic_info_array[i].min_live_point;

		//	help.set_custom_help( button_magic[i].x1, button_magic[i].y1, button_magic[i].x2, button_magic[i].y2,
		//		tech_res[magic_info_array[button_magic[i].custom_para.value -1].tech_id]->tech_des(), str );			
			char helpName[10] = "MAGIC0";
			helpName[5] = '1' + i;
			help.set_help( button_magic[i].x1, button_magic[i].y1, button_magic[i].x2, button_magic[i].y2,
				helpName);			
		}
	}
}


void FirmMagic::disp_magic_menu(int refreshFlag)
{
	// ----- create cancel button -------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1 +13, INFO_Y1 +235, 'A', "CANCEL" );
	}

	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap(INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );

	if( !is_own() )
		return;

	// display instruction
	String str;

	switch( menu_mode )
	{
	case FIRM_MAGIC_MENU_CAST:
		font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +20, INFO_X2 -10, INFO_Y1 +60, 
			text_firm.str_cast_magic_instruct(menu_mode_para), 2, 0, 0);
		break;

	case FIRM_MAGIC_MENU_UNIT:
		// str = "Select a Unit to Teleport";
		font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +20, INFO_X2 -10, INFO_Y1 +60, 
			text_firm.str_cast_teleport_source(), 2, 0, 0);
		break;

	default:
		err_here();
	}

	// display magic name

//	font_snds.put( INFO_X1+20, INFO_Y1+50, tech_res[magic_info_array[menu_mode_para-1].tech_id]->tech_des() );
	font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +60, INFO_X2 -10, INFO_Y1 +80,
		tech_res[magic_info_array[menu_mode_para-1].tech_id]->tech_des(), 2, 0, 0);

	// display cost

	if( menu_mode == FIRM_MAGIC_MENU_CAST )
	{
		char mobileType;
		int pointedXLoc;
		int pointedYLoc;

		if( world.zoom_matrix->get_detect_location(mouse.cur_x, mouse.cur_y, &pointedXLoc, &pointedYLoc, &mobileType) )
		{
		//	int x2 = font_snds.put( INFO_X1+20, INFO_Y1+70, "Life Point Cost : ");
		//	font_snds.put( x2, INFO_Y1+70, calc_live_point_cost( menu_mode_para, pointedXLoc, pointedYLoc, teleport_unit_recno) );

			switch( menu_mode_para )
			{
			case MAGIC_DESTROY_BUILDING:
			case MAGIC_FREEZE_UNIT:
			case MAGIC_DAMAGE_AREA:
				font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +80, INFO_X2 -10, INFO_Y1 +120, 
					text_firm.str_cast_cost_live_points(calc_live_point_cost( menu_mode_para, pointedXLoc, pointedYLoc, 0)),
					2, 0, 0 );
				break;

			case MAGIC_TELEPORT:
				font_snds.center_put_paragraph( INFO_X1 +20 , INFO_Y1 +80, INFO_X2 -10, INFO_Y1 +120,
					text_firm.str_teleport_cost_live_points( calc_live_point_cost(menu_mode_para, pointedXLoc, pointedYLoc, teleport_unit_recno)),
					2, 0, 0);
				break;
			}
		}
	}

	// display cancel button

	button_cancel.paint();
}


void FirmMagic::detect_magic_menu()
{
	// detect cancel button

	if( button_cancel.detect() )
	{
		menu_mode = FIRM_MAGIC_MENU_MAIN;
		menu_mode_para = 0;
		// ##### begin Gilbert 26/1 #####//
		power.reset_command();
		se_ctrl.immediate_sound("TURN_OFF");
		// ##### end Gilbert 26/1 #####//
	}
}


// ---- called by local player only -----//

int FirmMagic::set_teleport_unit(int unitRecno)
{
	if( unit_array.is_deleted(unitRecno) )
		return 0;

	Unit *unitPtr = unit_array[unitRecno];
	if( !unitPtr->is_nation(nation_recno) )
		return 0;

	// success

	err_when( menu_mode_para != MAGIC_TELEPORT || menu_mode != FIRM_MAGIC_MENU_UNIT );
	teleport_unit_recno = unitRecno;
	menu_mode = FIRM_MAGIC_MENU_CAST;

	return 1;
}



// ---------- begin of static function disp_firm_button --------//
static void disp_magic_button(ButtonCustom *button, int)
{
	char *magicName = tech_res[FirmMagic::magic_info_array[button->custom_para.value-1].tech_id]->tech_des();

	if ( !button->enable_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
		vga.active_buf->bar_alpha(button->x1+1, button->y1+1, button->x2-1, button->y2-1, 1, 0);
	}
	else if ( button->pushed_flag )
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDDWN") );
//		vga.blt_buf(button->x1, button->y1, button->x2, button->y2, 0);
		font_bld.center_put(button->x1+1, button->y1-1, button->x2+1, button->y2-1, magicName );
	}
	else if (mouse.in_area(button->x1, button->y1, button->x2, button->y2))
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDSEL") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
	}
	else
	{
		vga.active_buf->put_bitmap(  button->x1, button->y1, image_icon.read("BUILDUP") );
		font_bld.center_put(button->x1, button->y1-2, button->x2, button->y2-2, magicName );
	}
}
// ---------- end of static function disp_firm_button --------//


// --------- begin of function FirmMagic::disp_edit_mode -------//
//
void FirmMagic::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit cast energy

	edit_energy_enable = 1;
	x = font_whbl.put( x1, y, text_firm.str_cast_energy(), 0, x2 ); // Pray Points
	x += 20;
	x = edit_energy_x2 = font_blue.put( (edit_energy_x1=x), (edit_energy_y1=y), cast_delay, 0, x2 );
	edit_energy_y2 = edit_energy_y1+font_blue.max_font_height;
	y += 18;
}
// --------- end of function FirmMagic::disp_edit_mode -------//


// --------- begin of function FirmMagic::detect_edit_mode -------//
//
int FirmMagic::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	// edit energy

	if( edit_energy_enable )
	{
		if( mouse.any_click(edit_energy_x1, edit_energy_y1, edit_energy_x2, edit_energy_y2, LEFT_BUTTON) )
		{
			cast_delay += 10;
			if( cast_delay > cast_delay_max )
				cast_delay = cast_delay_max;
			return 1;
		}
		if( mouse.any_click(edit_energy_x1, edit_energy_y1, edit_energy_x2, edit_energy_y2, RIGHT_BUTTON) )
		{
			cast_delay -= 10;
			if( cast_delay < 0 )
				cast_delay = 0;
			return 1;
		}
	}

	return 0;
}
// --------- end of function FirmMagic::detect_edit_mode -------//


// --------- begin of function FirmMagic::right_click -----//
void FirmMagic::right_click(int xLoc, int yLoc)
{
	int firmRecno = 0;

	Location *locPtr = world.get_loc( xLoc, yLoc );

	// try to transfer energy to the target firm
	if( locPtr->is_firm() && transfer_energy(locPtr->firm_recno(), COMMAND_PLAYER) )
		return;

	Firm::right_click(xLoc, yLoc);
}
// --------- end of function FirmMagic::right_click -----//
