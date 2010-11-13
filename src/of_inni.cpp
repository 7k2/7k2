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

//Filename    : OF_INNI.CPP
//Description : Firm Inn - interface functions

#include <OF_INN.H>
#include <OINFO.H>
// #include <OBOX.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OSTR.H>
#include <OIMGRES.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OHERORES.H>
#include <OHELP.H>
#include <OVBROWIF.H>
#include <OGAME.H>
#include <OCONFIG.H>
#include <OTOWN.H>
#include <OBUTT3D.H>
#include <OBUTTCUS.H>
#include <ONATION.H>
#include <ORAWRES.H>
#include <ORACERES.H>
#include <OWORLD.H>
#include <OSPY.H>
#include <OREMOTE.H>
#include <OSERES.H>
#include <OBITMAP.H>
#include <OITEMRES.H>
#include <OCOLTBL.H>
#include <OSE.H>
#include <OSERES.H>
#include <OT_FIRM.H>
#include <OT_UNIT.H>

// ----------- Define constant --------//

#define UNIT_X_SPACING     25
#define UNIT_Y_SPACING     76

//----------- Define static variables ----------//

static Button3D	button_hire, button_spy_mobilize;
static ButtonCustom button_buy_item;
static int button_hire_visible, button_spy_mobilize_visible ;

static int disp_unit_list_y1;

short  FirmInn::selected_unit_id;

// ---------- declare static variable --------//

static void disp_item_button( ButtonCustom *, int );


//--------- Begin of function FirmInn::put_info ---------//
//
void FirmInn::put_info(int refreshFlag)
{
	// firm_inn_ptr = this;

	// ------- reset selected_unit_id ------//

	if( refreshFlag == INFO_REPAINT )
	{
		selected_unit_id = 0;
	}
	else
	{
		if( selected_unit_id < 0 )
			selected_unit_id = 0;
		if( selected_unit_id > inn_unit_count )
			selected_unit_id = inn_unit_count;
	}

	Firm::put_info(refreshFlag);

//	disp_basic_info(INFO_Y1, refreshFlag);

	if( refreshFlag == INFO_REPAINT )
	{
		button_hire.create(INFO_X1+13, INFO_Y1+235, 'A', "HIREUNIT" );
		button_hire.enable_flag = 0;
		button_spy_mobilize.create(INFO_X1+13, INFO_Y1+235, 'A', "MOBILSPY" );
		button_spy_mobilize.enable_flag = 0;
		button_buy_item.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 
			INFO_X1+13+BUTTON_DISTANCE+ITEM_ICON_WIDTH, INFO_Y1+235+ITEM_ICON_HEIGHT,
			disp_item_button, ButtonCustomPara(NULL, 0) );
		button_buy_item.visible_flag = 0;
	}

	button_hire_visible = button_spy_mobilize_visible = 0;

	if( !should_show_info() )
		return;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("INN_BASE") );

	disp_unit_list( INFO_Y1, refreshFlag );

	disp_unit_info( INFO_Y1 + 165, refreshFlag );

	// ------ main interface --------//

	// ------ display hire button -------//

	button_hire.enable_flag = 0;
	button_spy_mobilize.enable_flag = 0;

	if( selected_unit_id > 0 && selected_unit_id <= inn_unit_count )
	{
		InnUnit *innUnit = inn_unit_array + selected_unit_id - 1;

		if( innUnit->is_own_spy() )
		{
			button_spy_mobilize.enable_flag = 1;
			button_spy_mobilize.paint();
			button_spy_mobilize_visible = 1;
		}
		else if( is_own() )
		{
			button_hire.enable_flag = (~nation_array)->cash >= innUnit->hire_cost;
			button_hire.paint();
			button_hire_visible = 1;
		}
		else
		{
			button_spy_mobilize.paint();
			button_spy_mobilize_visible = 1;
		}

		if( (button_buy_item.visible_flag = innUnit->item.id != 0) )
		{
			button_buy_item.enable_flag = is_own() && buy_item( selected_unit_id, 1 );		// checking only
			button_buy_item.custom_para.value = innUnit->item.id;
			button_buy_item.paint();
		}
	}
	else
	{
		if( is_own() )
		{
			button_hire.paint();
			button_hire_visible = 1;
		}
		else
		{
			button_spy_mobilize.paint();
			button_spy_mobilize_visible = 0;
		}

		button_buy_item.visible_flag = 0;
	}

	/*
	//----------- display browser -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		browse_hire.init( HIRE_BROWSE_X1, HIRE_BROWSE_Y1, HIRE_BROWSE_X2, HIRE_BROWSE_Y2,
								0, 25, inn_unit_count, put_hire_rec );

		browse_hire.open(1);
	}
	else
	{
		if( last_hire_count != inn_unit_count )
		{
			if( last_hire_count==0 || inn_unit_count==0 )		// repaint the whole area as the detail area needs to disappear
			{
				last_hire_count = inn_unit_count;
				info.disp();
				return;
			}

			last_hire_count = inn_unit_count;

			browse_hire.refresh(-1, inn_unit_count);

			if( last_hire_count==0 || inn_unit_count==0 )
				refreshFlag = INFO_REPAINT;
		}
		else
			browse_hire.update();          // update only
	}

	last_hire_count = inn_unit_count;

	put_det(refreshFlag);

	//---------- display spy button ----------//

//	disp_spy_button(INFO_X1+BUTTON_ACTION_WIDTH, HIRE_DET_Y2+4, refreshFlag);
	*/
}
//----------- End of function FirmInn::put_info -----------//


//--------- Begin of function FirmInn::detect_info ---------//
//
void FirmInn::detect_info()
{
//	firm_inn_ptr = this;

	Firm::detect_info();

//	if( detect_basic_info() )
//		return;

	if( !should_show_info() )
		return;

	detect_unit_list();

	// ------ detect hire button -------//

	// since button_hire and button_spy_mobilize share the same place
	// if one is not visible, do not call detect() to avoid incorrect help text displayed

	if( button_hire_visible && button_hire.detect() 
		&& selected_unit_id > 0 && selected_unit_id <= inn_unit_count )
	{
		InnUnit *innUnit = inn_unit_array + selected_unit_id - 1;

		se_ctrl.immediate_sound("TAXGRANT");

		se_res.far_sound(center_x, center_y, 1, 'S', 
			unit_res[innUnit->unit_id]->sprite_id,
			"RDY" );
		if(remote.is_enable())
		{
			// packet structure : <firm recno>, <hire Id> <nation no> <spy escape>
			short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_INN_HIRE, 4*sizeof(short));
			shortPtr[0] = firm_recno;
			shortPtr[1] = selected_unit_id;
			shortPtr[2] = nation_recno;
			// ###### begin Gilbert 13/2 #######//
			shortPtr[3] = 0;
			// ###### end Gilbert 13/2 #######//
		}
		else
		{
			hire(selected_unit_id, 0);
		}

		// ------ clear selected_unit_id ------//

		selected_unit_id = 0;
	}
	else if( button_spy_mobilize_visible && button_spy_mobilize.detect() 
		&& selected_unit_id > 0 && selected_unit_id <= inn_unit_count )
	{
		se_res.far_sound(center_x, center_y, 1, 'S', 
			unit_res[inn_unit_array[selected_unit_id-1].unit_id]->sprite_id,
			"RDY" );

		err_when( spy_array.is_deleted(inn_unit_array[selected_unit_id-1].spy_recno) );
		Spy *spyPtr = spy_array[inn_unit_array[selected_unit_id-1].spy_recno];
		if( !remote.is_enable() )
		{
			if( spyPtr->mobilize_firm_spy() )
			{
				spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it
			}
		}
		else
		{
			// packet structure <spy recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_FIRM, sizeof(short) );
			*shortPtr = spyPtr->spy_recno;
		}

		// ------ clear selected_unit_id ------//

		selected_unit_id = 0;
	}

	// -------- detect buy item ------//

	if( selected_unit_id > 0 && selected_unit_id <= inn_unit_count )
	{
		InnUnit *innUnit = inn_unit_array + selected_unit_id - 1;

		if( button_buy_item.detect() )
		{
			se_ctrl.immediate_sound("TAXGRANT");

			if( !remote.is_enable() )
			{
				buy_item( selected_unit_id, 0 );
			}
			else
			{
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_INN_BUY_ITEM, 2*sizeof(short));
				// packet structure : <firm recno>, <inn unid id>
				shortPtr[0] = firm_recno;
				shortPtr[1] = selected_unit_id;
			}
			se_ctrl.immediate_sound("TURN_ON");
		}
		else if( innUnit->item.id )
		{
			// set custom help

//			String str;
//			str = "Buy ";
//			str += innUnit->item.item_name();
//			help.set_custom_help( button_buy_item.x1, button_buy_item.y1, 
//				button_buy_item.x2, button_buy_item.y2, str, innUnit->item.item_desc() );

			help.set_custom_help( button_buy_item.x1, button_buy_item.y1, 
				button_buy_item.x2, button_buy_item.y2, 
				text_firm.str_buy_item_help( &innUnit->item ),
				innUnit->item.item_desc() );
		}
	}

	//-------- detect spy button ----------//

//	if( !is_own() )
//	{
//		detect_spy_button();
//		return;
//	}

	/*
	//-------------------------------------//

	if( browse_hire.detect() )
	{
		put_det(INFO_UPDATE);
	}

	if( button_hire.detect() && inn_unit_count > 0 && browse_hire.recno() && !browse_hire.none_record )
	{
		InnUnit *innUnit = inn_unit_array + browse_hire.recno() - 1;

		se_ctrl.immediate_sound("TAXGRANT");

		int spyEscape = innUnit->spy_recno 
			&& spy_array[innUnit->spy_recno]->true_nation_recno == nation_array.player_recno;

		if( is_own() || spyEscape )
		{
			se_res.far_sound(center_x, center_y, 1, 'S', 
				unit_res[inn_unit_array[browse_hire.recno()-1].unit_id]->sprite_id,
				"RDY" );
			if(remote.is_enable())
			{
				// packet structure : <firm recno>, <hire Id> <nation no>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_INN_HIRE, 4*sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = browse_hire.recno();
				shortPtr[2] = nation_recno;
				shortPtr[4] = spyEscape;
			}
			else
			{
				hire(browse_hire.recno(), spyEscape);
			}
		}
	}
	*/
}
//----------- End of function FirmInn::detect_info -----------//


//-------- Begin of function FirmInn::should_show_info --------//
//
int FirmInn::should_show_info()
{
	int rc = Firm::should_show_info();
	if(rc)
		return rc;

	// check any spy in the innUnit

	// --------- if if any own spy here -------//

	InnUnit *innUnit = inn_unit_array;
	int playerSpyCount = 0;
	for( int i = 0; i < inn_unit_count; ++i, ++innUnit )
	{
		if( innUnit->is_own_spy() )
			return 1;
	}

	return 0;
}
//-------- End of function FirmInn::should_show_info --------//


/*
//--------- Begin of function FirmInn::put_det ---------//
//
void FirmInn::put_det(int refreshFlag)
{
	if( browse_hire.none_record || browse_hire.recno() == 0 )
	{
		button_hire.reset();
		return;
	}


	//--------- display details ----------//

	InnUnit *innUnit = inn_unit_array+browse_hire.recno()-1;
	int ownSpy = innUnit->spy_recno && spy_array[innUnit->spy_recno]->true_nation_recno == nation_array.player_recno;

	disp_unit_info(HIRE_DET_Y1, innUnit, refreshFlag );

	//------- paint buttons --------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_hire.paint( HIRE_DET_X1, HIRE_DET_Y2+4, 'A', "HIREUNIT" );
	}
	else
	{
		if( inn_unit_count > 0 &&
			 ( (~nation_array)->cash >= innUnit->hire_cost || ownSpy ) )
		{
			button_hire.enable();
		}
		else
		{
			button_hire.disable();
		}
	}
}
//----------- End of function FirmInn::put_det -----------//
*/

// -------- Begin of function FirmInn::disp_unit_list -------//
//
void FirmInn::disp_unit_list(int dispY1, int refreshFlag)
{
	disp_unit_list_y1 = dispY1;

	// ##### begin Gilbert 7/12 #######//
	// count no. of units on each row

	int rowUnitCountOld[2] = { 0, 0 };		// no of soldier and no of spy counted
	rowUnitCountOld[0] = count_soldier();
	rowUnitCountOld[1] = count_spy();
	// ##### end Gilbert 7/12 #######//

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;

		// ##### begin Gilbert 7/10 ######//
		// initial rowUnitCount according to inc
		int rowUnitCount[2];		// no of soldier and no of spy counted
		if( inc >= 0 )
			memset( rowUnitCount, 0, sizeof(rowUnitCount) );
		else
			memcpy( rowUnitCount, rowUnitCountOld, sizeof(rowUnitCount) );
		// ##### end Gilbert 7/10 ######//

		for( int i = inc>=0?1:inn_unit_count; i > 0 && i <= inn_unit_count; i +=inc )
		{
			InnUnit *innUnit = inn_unit_array + i - 1;

			int row = innUnit->spy_skill <= 0 ? 0 : 1;		// soldier on row 0, spy on row 1
			// ##### begin Gilbert 7/10 ######//
			if( inc < 0 )
				--rowUnitCount[row];				// decrease rowUnitCount before use
			// ##### end Gilbert 7/10 ######//
			int x = INFO_X1 + 67 + rowUnitCount[row] * UNIT_X_SPACING;
			int y = INFO_Y1 + 44 + row * UNIT_Y_SPACING;
			int yHp = INFO_Y1 + 7 + row * UNIT_Y_SPACING;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 218;
			int windowY1 = INFO_Y1 + 5 + row * 76;	// 5,81
			int windowY2 = windowY1 + 72 - 1 ;

			// ###### begin Gilbert 7/10 #######//
			if( inc >= 0 )
				++rowUnitCount[row];				// increase rowUnitCount after use
			// ###### end Gilbert 7/10 #######//

			if( inc < 0 )
			{
				// first pass descending, draw unit icon

				info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
					innUnit->unit_id, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 1 );
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
					innUnit->unit_id, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_unit_id?3:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				int fontHeight = font_whbl.max_font_height;

				if( innUnit->is_own_spy() )		// display spy icon
				{
					vga.active_buf->put_bitmap_trans( x+UNIT_X_SPACING/2-8, yHp-5, image_icon.read("U_SPY") );
				}
				else if( innUnit->hero_id )
				{
					vga.active_buf->put_bitmap_trans( x+UNIT_X_SPACING/2-8, yHp-5, image_icon.read("U_HERO") );
				}
				else
				{
					if( row == 0 )		// display combat skill and leadership
					{
						font_whbl.center_put( x, yHp, x+UNIT_X_SPACING, yHp+fontHeight, m.format(innUnit->skill_level()) );
					}
					else if( row == 1 )		// display spy skill
					{
						font_whbl.center_put( x, yHp, x+UNIT_X_SPACING, yHp+fontHeight, m.format(innUnit->spy_skill) );
					}
				}

				if( innUnit->item.id )
				{
					char *iconPtr = item_res.item_unit_interface(innUnit->item.id);
					if( iconPtr )
						vga.active_buf->put_bitmap_trans( x+UNIT_X_SPACING/2 -((Bitmap *)iconPtr)->get_width()/2,
						yHp +53, iconPtr );
				}

			}
		}
	}
}
// -------- End of function FirmInn::disp_unit_list -------//


// -------- Begin of function FirmInn::detect_unit_list -------//
//
void FirmInn::detect_unit_list()
{
	int dispY1 = disp_unit_list_y1;

	int rowUnitCount[2] = { 0, 0 };		// no of soldier and no of spy counted

	// detect in ascending order 

	for( int i = 1; i <= inn_unit_count; ++i )
	{
		InnUnit *innUnit = inn_unit_array + i - 1;

		int row = innUnit->spy_skill <= 0 ? 0 : 1;		// soldier on row 0, spy on row 1
		int x = INFO_X1 + 67 + rowUnitCount[row] * UNIT_X_SPACING;
		int y = INFO_Y1 + 44 + row * UNIT_Y_SPACING;
		int yHp = INFO_Y1 + 7 + row * UNIT_Y_SPACING;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 218;
		int windowY1 = INFO_Y1 + 5 + row * 76;	// 5,81
		int windowY2 = windowY1 + 72 - 1 ;

		++rowUnitCount[row];

		int rc = info.draw_unit_icon( x+UNIT_X_SPACING/2, y,
				innUnit->unit_id, nation_recno,
				windowX1, windowY1, windowX2, windowY2, 8 );		// detect left button

		if( rc & 8)
		{
			selected_unit_id = i;
			break;
		}
	}
}
// -------- End of function FirmInn::detect_unit_list -------//

//--------- Begin of function FirmInn::disp_unit_info ---------//
//
// Display the skill information of the people in the town.
//
// <int> 			 dispY1		 - the top y coordination of the info area
// <InnUnit*> hireInfoPtr - pointer to a HireInfo structure
// <int>    		 refreshFlag - refresh flag
//
void FirmInn::disp_unit_info(int dispY1, int refreshFlag)
{
	if( !selected_unit_id || selected_unit_id > inn_unit_count)
		return;

	// ##### begin Gilbert 8/1 #####//
//	int x = INFO_X1 + 83;
	int x = INFO_X1 + 20;
	// ##### end Gilbert 8/1 #####//
	int y = dispY1;
	int x2;

	err_when( selected_unit_id <= 0 || selected_unit_id > inn_unit_count );

	InnUnit *innUnit = inn_unit_array + selected_unit_id - 1;

	// display unit name

#if(defined(CHINESE))
	//SXM
	y-=3;
	//SXM
#endif

	if( innUnit->hero_id )
		font_snds.put( x, y, hero_res[innUnit->hero_id]->name, 0, -1, 1 );
	else
		font_snds.put( x, y, unit_res[innUnit->unit_id]->name, 0, -1, 1 );

#if(defined(CHINESE))
	//SXM
	y++;
	//SXM
#endif
	y += 14;

	// display combat level

	// x2 = font_snds.put( x, y, "Combat" ) + 10;
	x2 = font_snds.put( x, y, text_unit.str_combat_level() ) + 10;
	font_snds.right_put( INFO_X2-100, y, m.format(innUnit->combat_level(), 4) );
//	x2 = font_snds.put( x2, y, innUnit->combat_level() ) + 15;

	// display spying skill

	if( innUnit->item.id )
	{
		if( innUnit->spy_skill > 0 )
		{
			// x2 = font_snds.put( x+110, y, "Spying" ) + 10;
			x2 = font_snds.put( x+110, y, text_unit.str_spy_skill() ) + 10;
			font_snds.right_put( INFO_X2-10, y, m.format(innUnit->spy_skill,4) );
		}
		else if( innUnit->is_own_spy() )
		{
			// x2 = font_snds.put( x+110, y, "Spying" ) + 10;
			x2 = font_snds.put( x+110, y, text_unit.str_spy_skill() ) + 10;
			font_snds.right_put( INFO_X2-10, y, m.format(spy_array[innUnit->spy_recno]->spy_skill,4) );
		}
		else if ( innUnit->hero_id )
		{
//			String str;
//			str  = race_res[ unit_res[innUnit->unit_id]->race_id ]->adjective;
//			str += " Hero";
//			font_red.right_put( INFO_X2-5, y, str );
			font_red.right_put( INFO_X2-5, y, text_unit.str_race_hero(race_res[ unit_res[innUnit->unit_id]->race_id ]->adjective) );
		}
	}

#if(defined(CHINESE))
	//SXM
	y++;
	//SXM
#endif
	y += 14;

	// display leadership skill

	if( innUnit->skill_level() > 0 )
	{
		// x2 = font_snds.put( x, y, "Leadership" ) + 10;
		x2 = font_snds.put( x, y, text_unit.str_leadership() ) + 10;
		font_snds.right_put( INFO_X2-100, y, m.format(innUnit->skill_level(), 4) );
	//	font_snds.put( x2, y, innUnit->skill_level() );
	}

	// display hire cost

	font_snds.right_put( INFO_X1+212, y, m.format(innUnit->hire_cost, 2) );

#if(defined(CHINESE))
	//SXM
	y++;
	//SXM
#endif
	y += 14;

	// display item

	if( innUnit->item.id )
	{
		font_snds.put( x, y, innUnit->item.item_name() );
		font_snds.right_put( INFO_X1+212, y, m.format(innUnit->item.cost(), 2) );
	}
	else
	{
		if( innUnit->spy_skill > 0 )
		{
			// x2 = font_snds.put( x, y, "Spying" ) + 10;
			x2 = font_snds.put( x, y, text_unit.str_spy_skill() ) + 10;
			font_snds.right_put( INFO_X2-100, y, m.format(innUnit->spy_skill, 4) );
		//	font_snds.put( x2, y, innUnit->spy_skill );
		}
		else if( innUnit->is_own_spy() )
		{
			// x2 = font_snds.put( x, y, "Spying" ) + 10;
			x2 = font_snds.put( x, y, text_unit.str_spy_skill() ) + 10;
			font_snds.right_put( INFO_X2-100, y, m.format(spy_array[innUnit->spy_recno]->spy_skill, 4) );
		//	font_snds.put( x2, y, spy_array[innUnit->spy_recno]->spy_skill );
		}
		else if ( innUnit->hero_id )
		{
//			String str;
//			str  = race_res[ unit_res[innUnit->unit_id]->race_id ]->adjective;
//			str += " Hero";
//			font_red.right_put( INFO_X2-95, y, str );
			font_red.right_put( INFO_X2-95, y, text_unit.str_race_hero(race_res[ unit_res[innUnit->unit_id]->race_id ]->adjective) );
		}
	}
//	info.draw_unit_icon( INFO_X1+38, INFO_Y1+228-32,
//		innUnit->unit_id, nation_recno,
//		INFO_X1+14, INFO_Y1+157, INFO_X1+63, INFO_Y1+228, 1 );

}
//----------- End of function FirmInn::disp_unit_info -----------//



// ---- begin of function FirmInn::explore_for_player --------//
//
int FirmInn::explore_for_player()
{
	if( Firm::explore_for_player() )
		return 1;

	// count number of spy

	for( int i = 0; i < inn_unit_count; ++i )
	{
		int spyRecno;
		if( (spyRecno = inn_unit_array[i].spy_recno)
			&& spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
			return 1;
	}

	return 0;
}
// ---- end of function FirmInn::explore_for_player --------//


// ------- begin of static function disp_item_button ------//

static void disp_item_button(ButtonCustom *button, int )
{
	err_when( !button->custom_para.value );		// no item
	char *bitmapPtr = item_res.item_icon( button->custom_para.value );		// bitmap on interface area

	if( !button->enable_flag )
	{
		vga.active_buf->put_bitmap_remap(button->x1, button->y1, bitmapPtr, 
			(short *)vga.vga_color_table->get_table(-MAX_BRIGHTNESS_ADJUST_DEGREE*3/4) );
	}
	else
	{
		int bx2 = button->x1 + ((Bitmap *)bitmapPtr)->get_width();
		int by2 = button->y1 + ((Bitmap *)bitmapPtr)->get_height();

		if( !button->pushed_flag )
		{
			vga.active_buf->put_bitmap(button->x1, button->y1, bitmapPtr);

			// clear boarder drawn when button down
			vga.active_buf->bar( button->x1, by2, bx2, by2, V_BLACK );
			vga.active_buf->bar( bx2, button->y1, bx2, by2, V_BLACK );
		}
		else
		{
			vga.active_buf->put_bitmap(button->x1+1, button->y1+1, bitmapPtr);

			// clear boarder drawn when button up
			vga.active_buf->bar( button->x1, button->y1, bx2, button->y1, V_BLACK );
			vga.active_buf->bar( button->x1, button->y1, button->x1, by2, V_BLACK );
		}
	}
}
// ------- end of static function disp_item_button ------//
