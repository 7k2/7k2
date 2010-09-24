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

// Filename    : OF_CAMPB.CPP
// Description : Firm Camp bribe function

#include <OF_CAMP.H>
#include <ALL.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OREMOTE.H>
#include <OSE.H>
#include <OHELP.H>
#include <OTRANSL.H>
#include <OBUTTON.H>
#include <OBUTT3D.H>
#include <OBUTTCUS.H>
#include <OCONFIG.H>
#include <ORACERES.H>
#include <ONATION.H>
#include <OSPY.H>
#include <OUNIT.H>
#include <OTECHRES.H> 
#include <OTECH.H>
#include <OGODRES.H>
#include <OT_FIRM.H>
#include <OT_UNIT.H>

//---------- Define constant ------------//

#define BRIBE_OPTION_HEIGHT 	19

//-------- Define bribe amounts ---------//

#define BRIBE_AMOUNT_COUNT  6
#define MAX_BRIBE_AMOUNT	 4000

static short bribe_amount_array[] = { 500, 1000, 1500, 2000, 3000, 4000 };

//-------- Define static vars ---------//

static Button3D button_cancel;
static ButtonCustomGroup button_bribe_amount(BRIBE_AMOUNT_COUNT);
static Button3D button_go;
static int button_nation_count;
static ButtonCustomGroup button_nation_group(MAX_NATION-2);
static Button		button_yes, button_no;
//------ declare static functions -------//

static void i_disp_bribe_button( ButtonCustom *, int );
// static void disp_bribe_button(int y, int bribeAmount, int buttonUp);
static void disp_nation_button(ButtonCustom *button, int repaintBody);


//--------- Begin of function FirmCamp::can_spy_bribe ---------//
//
// <int> bribeSoldierId - soldier id. in this firm to bribe
//								 0 - if bribe an overseer.
//
// <int> briberNationRecno - the nation recno of the briber.
//
int FirmCamp::can_spy_bribe(int bribeSoldierId, int briberNationRecno)
{
	int canBribe=0;
	int spyRecno;

	err_when( bribeSoldierId < 0 || bribeSoldierId > MAX_SOLDIER );

	if( bribeSoldierId > soldier_count					// soldier leaves
		|| bribeSoldierId == 0 && !overseer_recno )		// overseer leaves
		return 0;

	if( bribeSoldierId )		// the overseer is selected
		spyRecno = soldier_array[bribeSoldierId-1].spy_recno;
	else
		spyRecno = unit_array[overseer_recno]->spy_recno;

	if( spyRecno )
	{
		canBribe = spy_array[spyRecno]->true_nation_recno != briberNationRecno; 		// only when the unit is not yet a spy of the player. Still display the bribe button when it's a spy of another nation
	}
	else
	{
		if( bribeSoldierId )
		{
			canBribe = soldier_array[bribeSoldierId-1].is_human();	 // cannot bribe if it's a weapon
		}
		else
		{
			canBribe = unit_array[overseer_recno]->is_human() &&
						  unit_array[overseer_recno]->rank_id != RANK_KING;		// cannot bribe a king
		}
	}

	return canBribe;
}
//----------- End of function FirmCamp::can_spy_bribe -----------//


//--------- Begin of function FirmCamp::spy_bribe ---------//
//
// The money the spy offers to bribe the unit.
//
// <int>   bribeAmount	  - the amount offered
// <short> birberSpyRecno - spy recno of the briber
// <short> soldierId		  - if 0, then bribe the overseer,
//									 if >0, then bribe a soldier.
//
// return: <int> >0 - bribing succeeded, return the spy recno of the bribed unit (as it has been turned into a spy)
//					  0 - bribing failed
//
int FirmCamp::spy_bribe(int bribeAmount, short briberSpyRecno, short soldierId)
{
	if( !can_spy_bribe(soldierId, spy_array[briberSpyRecno]->true_nation_recno) )		// this can happen in multiplayer as there is a one frame delay when the message is sent and when it is processed
		return 0;

	//---------------------------------------//

	int succeedChance = spy_bribe_succeed_chance(bribeAmount, briberSpyRecno, soldierId);

	Spy* spyPtr = spy_array[briberSpyRecno];

	nation_array[spyPtr->true_nation_recno]->add_expense( EXPENSE_BRIBE, (float) bribeAmount, 0 );

	//------ if the bribe succeeds ------//

	if( succeedChance > 0 && m.random(100) < succeedChance )
	{
		int spyRecno = spy_array.add_spy();		// add a new Spy record

		Spy* newSpy = spy_array[spyRecno];

		newSpy->spy_skill = 10;
		newSpy->action_mode = SPY_IDLE;
		newSpy->spy_loyalty = min( 100, max(30,succeedChance) );		// within the 30-100 range

		newSpy->true_nation_recno    = spyPtr->true_nation_recno;
		newSpy->cloaked_nation_recno = spyPtr->cloaked_nation_recno;

		if( soldierId )
		{
			Soldier* soldierPtr = soldier_array+soldierId-1;

			soldierPtr->spy_recno = spyRecno;
			newSpy->race_id = soldierPtr->race_id;
			newSpy->name_id = soldierPtr->name_id;

			err_when( newSpy->race_id < 1 || newSpy->race_id > MAX_RACE );

			if( !newSpy->name_id )		// if this soldier does not have a name, give him one now as a spy must reserve a name (see below on use_name_id() for reasons)
				newSpy->name_id = race_res[newSpy->race_id]->get_new_name_id();
			// ###### begin Gilbert 1/3 #######//
			else
				race_res[newSpy->race_id]->use_name_id( newSpy->name_id );
			// ###### end Gilbert 1/3 #######//

			// ######## begin Gilbert 24/2 ########//
			newSpy->unique_id = soldierPtr->unique_id;
			// ######## end Gilbert 24/2 ########//
		}
		else if( overseer_recno )
		{
			Unit* unitPtr = unit_array[overseer_recno];

			unitPtr->spy_recno = spyRecno;
			newSpy->race_id = unitPtr->race_id;
			newSpy->name_id = unitPtr->name_id;
			// ###### begin Gilbert 1/3 #######//
			race_res[newSpy->race_id]->use_name_id( newSpy->name_id );
			// ###### end Gilbert 1/3 #######//
			// ######## begin Gilbert 24/2 ########//
			newSpy->unique_id = unitPtr->unique_id;
			// ######## end Gilbert 24/2 ########//

			err_when( newSpy->race_id < 1 || newSpy->race_id > MAX_RACE );
		}
		else
			err_here();

		newSpy->set_place( SPY_FIRM, firm_recno );

		//-- Spy always registers its name twice as his name will be freed up in deinit(). Keep an additional right because when a spy is assigned to a town, the normal program will free up the name id., so we have to keep an additional copy

		// ###### begin Gilbert 1/3 #######//
		// race_res[newSpy->race_id]->use_name_id(newSpy->name_id);
		// ###### end Gilbert 1/3 #######//

		if( firm_recno == firm_array.selected_recno && spyPtr->true_nation_recno == nation_array.player_recno )
		{
			// ----- for showing bribe result on the briber machine ------//

			bribe_result = BRIBE_SUCCEED;
//			info.disp();
		}

		return newSpy->spy_recno;
	}
	else //------- if the bribe fails --------//
	{
		int spyTrueNationRecno = spyPtr->true_nation_recno;		// keep before spy killed

		spyPtr->get_killed(0);		// the spy gets killed when the action failed.
											// 0 - don't display new message for the spy being killed, so we already display the msg on the interface

		if( firm_recno == firm_array.selected_recno && spyTrueNationRecno == nation_array.player_recno )
		{
			// ----- for showing bribe result on the briber machine ------//

			bribe_result = BRIBE_FAIL;
//			info.disp();
		}

		return 0;
	}
}
//----------- End of function FirmCamp::spy_bribe -----------//


//--------- Begin of function FirmCamp::spy_birbe_succeed_chance ---------//
//
// The money the spy offers to bribe the unit.
//
// <int>   bribeAmount	  - the amount offered
// <short> birberSpyRecno - spy recno of the briber
// <short> soldierId		  - if 0, then bribe the overseer,
//									 if >0, then bribe a soldier.
//
// return: <int> 1 - bribing succeeded
//					  0 - bribing failed
//
int FirmCamp::spy_bribe_succeed_chance(int bribeAmount, short briberSpyRecno, short soldierId)
{
	Spy* spyPtr = spy_array[briberSpyRecno];

	err_when( spyPtr->spy_place != SPY_FIRM );
	err_when( spyPtr->spy_place_para != firm_recno );

	//---- if the bribing target is a soldier ----//

	int unitLoyalty, unitRaceId, targetSpyRecno, unitCommandPower;

	if( soldierId )
	{
		Soldier* soldierPtr = soldier_array+soldierId-1;

		unitLoyalty = soldierPtr->loyalty;
		unitRaceId  = soldierPtr->race_id;
		unitCommandPower = 0;
		targetSpyRecno = soldierPtr->spy_recno;
	}
	else if( overseer_recno )
	{
		Unit* unitPtr = unit_array[overseer_recno];

		unitLoyalty = unitPtr->loyalty;
		unitRaceId  = unitPtr->race_id;
		unitCommandPower = unitPtr->commander_power();
		targetSpyRecno = unitPtr->spy_recno;
	}
	else
		err_here();

	err_when( unitRaceId < 1 || unitRaceId > MAX_RACE );

	//---- determine whether the bribe will be successful ----//

	int succeedChance;

	if( targetSpyRecno )		// if the bribe target is also a spy
	{
		err_when( spy_array[targetSpyRecno]->true_nation_recno == spyPtr->true_nation_recno );		// the player shouldn't be able to bribe units of his own

		succeedChance = 0;
	}
	else
	{
		int techLevel = 0;
		if( spyPtr->true_nation_recno )
			techLevel = tech_res[TECH_BRIBE]->get_nation_tech_level(spyPtr->true_nation_recno);
		// ##### patch begin Gilbert 16/2 #####//
		if( god_res[GOD_PERSIAN]->nation_prayer_count(spyPtr->true_nation_recno) > 0 )
			techLevel++;
		// ##### patch end Gilbert 16/2 #####//

		succeedChance = spyPtr->spy_skill - unitLoyalty - unitCommandPower
							 + (int) nation_array[spyPtr->true_nation_recno]->reputation
							 + 200 * (1 + techLevel) * bribeAmount / MAX_BRIBE_AMOUNT;

		//-- the chance is higher if the spy or the spy's king is racially homongenous to the bribe target,

		int spyKingRaceId = nation_array[ spyPtr->true_nation_recno ]->race_id;

		succeedChance += race_res.is_same_race(spyPtr->race_id, unitRaceId) * 10 +
							  race_res.is_same_race(spyKingRaceId, unitRaceId) * 10;

//		if( unitLoyalty > 60 )			// harder for bribe units with over 60 loyalty
//			succeedChance -= (unitLoyalty-60);
//		if( unitLoyalty > 70 )			// harder for bribe units with over 70 loyalty
//			succeedChance -= (unitLoyalty-70);
//		if( unitLoyalty > 80 )			// harder for bribe units with over 80 loyalty
//			succeedChance -= (unitLoyalty-80);
//		if( unitLoyalty > 90 )			// harder for bribe units with over 90 loyalty
//			succeedChance -= (unitLoyalty-90);
		if( unitLoyalty > 60 )
			succeedChance -= unitLoyalty - 60;

		if( unitLoyalty == 100 )
			succeedChance = 0;
	}

	return succeedChance;
}
//----------- End of function FirmCamp::spy_birbe_succeed_chance -----------//


//--------- Begin of function FirmCamp::disp_bribe_menu ---------//
//
void FirmCamp::disp_bribe_menu(int refreshFlag)
{
	//---- if the briber or bribe target is no longer valid -----//

	if( bribe_result == BRIBE_NONE )
	{
		if( !validate_cur_bribe() )
		{
			// check in detect_bribe_menu instead
//			firm_menu_mode = FIRM_MENU_MAIN;
//			bribe_result   = BRIBE_NONE;
//			info.disp();
			return;
		}
	}

	//------ display the bribe menu ------//

	if( bribe_result == BRIBE_NONE )
	{
		int y=INFO_Y1+10;

		// font_san.d3_put( INFO_X1, y, INFO_X2, y+19, "Bribe" );
		// font_snds.put( INFO_X1, y, "Bribe" );
		// y+=22;

		disp_bribe_unit( y );
		y+=60;

		for( int i=0 ; i<BRIBE_AMOUNT_COUNT ; i++ )
		{
			if( refreshFlag == INFO_REPAINT )
				button_bribe_amount[i].create( INFO_X1+18, y, INFO_X2-14, y+BRIBE_OPTION_HEIGHT-1, 
					i_disp_bribe_button, ButtonCustomPara( NULL, bribe_amount_array[i]), 0 );

			button_bribe_amount[i].enable_flag = (~nation_array)->cash >= bribe_amount_array[i];

			err_when( bribe_amount_array[i] > MAX_BRIBE_AMOUNT );

			y += BRIBE_OPTION_HEIGHT+2;
		}

		if( refreshFlag == INFO_REPAINT )
		{
		   button_bribe_amount.push(0, 0);

			// spy_action_chance recalculate at button_bribe_amount.detect()
			// here calc. for the first time

			spy_action_chance = spy_bribe_succeed_chance(bribe_amount_array[button_bribe_amount()],
				action_spy_recno, action_target_recno);
			if( !config.show_debug_info )
			{
				spy_action_chance = min( spy_action_chance, 0 );
				spy_action_chance = max( spy_action_chance, 100 );
			}
		}
		button_bribe_amount.paint();

		// display chance

		if( config.show_debug_info )
		{
//			String str;
//			str = "(";
//			str += m.format( spy_action_chance, 3 );
//			str += " chance)";
//			font_snds.put( INFO_X1+18, y+5, str );
			font_snds.put( INFO_X1+18, y+5, text_firm.str_bribe_chance(spy_action_chance) );
		}

		if( refreshFlag == INFO_REPAINT )
		{
			button_go.create( INFO_X1+13, INFO_Y1+281, 'A', "BRIBE" );
		}
		button_go.enable_flag = (~nation_array)->cash >= bribe_amount_array[button_bribe_amount()];
		button_go.paint();
	}
	else
	{
		err_here();
	}

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}
	button_cancel.paint();
}
//----------- End of function FirmCamp::disp_bribe_menu -----------//


//--------- Begin of function FirmCamp::detect_bribe_menu ---------//
//
void FirmCamp::detect_bribe_menu()
{
	if( bribe_result == BRIBE_NONE )
	{
		if( !validate_cur_bribe() )
		{
			// check in detect_bribe_menu instead
			firm_menu_mode = FIRM_MENU_MAIN;
			bribe_result   = BRIBE_NONE;
//			info.disp();
			return;
		}
	}

	//----- if it is display the bribe result right now -----//

	if( bribe_result == BRIBE_NONE )
	{
		if( button_go.detect() )
		{
			firm_menu_mode = FIRM_MENU_BRIBE_RESULT;
			bribe_result = BRIBE_NONE;

			//--------- bribe now ---------//

			if( !remote.is_enable() )
			{
				spy_bribe(bribe_amount_array[button_bribe_amount()], action_spy_recno, action_target_recno );
				action_spy_recno = 0;
			}
			else
			{
				// packet structure <firm recno> <spy recno> <bribe target : worker (0=overseer)> <amount>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_BRIBE, 4*sizeof(short));
				*shortPtr = firm_recno;
				shortPtr[1] = action_spy_recno;
				shortPtr[2] = action_target_recno;
				shortPtr[3] = bribe_amount_array[button_bribe_amount()];
			}
		}

		int rc;
		if( (rc = button_bribe_amount.detect()) >= 0 )
		{
			// update chance
			spy_action_chance = spy_bribe_succeed_chance(bribe_amount_array[rc],
				action_spy_recno, action_target_recno);
			if( !config.show_debug_info )
			{
				spy_action_chance = min( spy_action_chance, 0 );
				spy_action_chance = max( spy_action_chance, 100 );
			}
			se_ctrl.immediate_sound("TURN_ON");
		}
	}

	//------ detect the cancel button --------//

	if( mouse.any_click(RIGHT_BUTTON) > 0 || button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		bribe_result   = BRIBE_NONE;
		se_ctrl.immediate_sound("TURN_OFF");
//		info.disp();
	}
}
//----------- End of function FirmCamp::detect_bribe_menu -----------//


//--------- Begin of function FirmCamp::validate_cur_bribe ---------//
//
// Whether the current bribe action is still valid.
//
int FirmCamp::validate_cur_bribe()
{
	if( spy_array.is_deleted(action_spy_recno) ||
		 spy_array[action_spy_recno]->true_nation_recno != nation_array.player_recno )
	{
		return 0;
	}

//	return can_spy_bribe( selected_soldier_id, spy_array[action_spy_recno]->true_nation_recno );
	return can_spy_bribe( action_target_recno, spy_array[action_spy_recno]->true_nation_recno );
}
//----------- End of function FirmCamp::validate_cur_bribe -----------//


//-------- Begin of static function disp_bribe_button --------//
//
static void i_disp_bribe_button(ButtonCustom *button, int)
{
	// ------ display button frame  -------//

	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2 );
	}
	else
	{
		vga.active_buf->d3_panel_down( button->x1, button->y1, button->x2, button->y2 );
	}

	// ----- display button text ----------//

	String str;

	str = text_firm.str_bribe_amount( button->custom_para.value );

	// ######### begin Gilbert 19/2 ##########//
	if( !button->pushed_flag )
		font_bld.center_put( button->x1, button->y1, button->x2, button->y2-2, str );
	else
		font_bld.center_put( button->x1, button->y1+1, button->x2, button->y2-1, str );
	// ######### end Gilbert 19/2 ##########//

	if( !button->enable_flag )
	{
		vga.active_buf->bar_alpha(button->x1, button->y1, button->x2, button->y2, 2, V_BLACK);
	}
}
//--------- End of static function i_disp_bribe_button ---------//



//--------- Begin of function FirmCamp::disp_bribe_unit ---------//
//
void FirmCamp::disp_bribe_unit(int dispY1)
{
	//---------------- paint the panel -----------------//

//	vga.active_buf->d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+46);

	//------- get the info of the bribe target ---------//

	int 	raceId, unitLoyalty, unitId, rankId;
	char* unitName;

	if( action_target_recno )
	{
		Soldier* soldierPtr = soldier_array+action_target_recno-1;

		raceId 		= soldierPtr->race_id;
		unitId      = soldierPtr->unit_id;
		unitLoyalty = soldierPtr->loyalty;
		// unitName    = race_res[raceId]->get_name(soldierPtr->name_id);
		unitName    = unit_res[unitId]->name;	// don't display soldier name, can spot spy
		rankId      = soldierPtr->rank_id;
	}
	else if( overseer_recno )
	{
		Unit* unitPtr = unit_array[overseer_recno];

		raceId		= unitPtr->race_id;
		unitId      = unitPtr->unit_id;
		unitLoyalty = unitPtr->loyalty;
		unitName    = unitPtr->unit_name();
		rankId      = unitPtr->rank_id;
	}
	else
		err_here();

	//--------- display info of the bribe target ---------//

	int x=INFO_X1+20, y=dispY1+4;
	int x2;

//	font_bld.put( x, y, "Bribe " );
//	y += 15;
//	int x2 = font_bld.put( x, y, unitName );
//	x2 = font_bld.put( x2, y, " ?" );
//	y += 15;
	font_bld.put_paragraph( x, y, INFO_X2-16, y+36, 
		text_firm.str_ask_bribe_unit(unitName) );
	y += 36;

	//------- display skill and productivity ---------//

//	x2 = font_bld.put( x, y, "Loyalty: ");
	x2 = font_bld.put( x, y, text_unit.str_loyalty() );
	x2 = font_bld.put( x2, y, ": " );
	x2 = font_bld.put( x2, y, unitLoyalty );
}
//----------- End of function FirmCamp::disp_bribe_unit -----------//


// ---------- begin of function FirmCamp::disp_bribe_result -------//
//
void FirmCamp::disp_bribe_result(int refreshFlag)
{
//	if( refreshFlag != INFO_REPAINT )
//		return;

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}

	//------ display the bribe result -----//

//	int x=INFO_X1+4, y=INFO_Y1+4, y2=y+font_san.height()-1;
	int x=INFO_X1+22, y=INFO_Y1+14, y2=y+font_san.height()-1;

	switch( bribe_result )
	{
	case BRIBE_SUCCEED:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
		font_whbl.put( x, y, text_firm.str_bribe_success() );// "Bribing Succeeded." );
		break;

	case BRIBE_FAIL:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("EXECUTE") );
		font_whbl.put( x, y, text_firm.str_bribe_fail() ); //"Bribing Failed." );
		y += font_whbl.max_font_height;
		// font_whbl.put( x, y, text_firm.str_bribe_execute() ); //"Your Spy Was Caught And Executed." );
		font_whbl.put_paragraph( x, y, INFO_X2-14, INFO_Y2, text_firm.str_bribe_execute() );
		break;

	case BRIBE_NONE:
		// waiting in multiplayer mode
		break;

	default:
		err_here();
	}

	button_cancel.paint();
}
// ---------- end of function FirmCamp::disp_bribe_result -------//


// ---------- begin of function FirmCamp::detect_bribe_result -------//
//
void FirmCamp::detect_bribe_result()
{
	if( mouse.any_click(RIGHT_BUTTON) > 0 || button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		bribe_result = BRIBE_NONE;
	}
}
// ---------- end of function FirmCamp::detect_bribe_result -------//

//--------- Begin of function Firm::disp_assassinate_confirm ---------//

void FirmCamp::disp_assassinate_confirm(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		button_yes.create_text( INFO_X1+50, INFO_Y1+100, text_firm.str_yes() ); // "Yes" );
		button_no.create_text( INFO_X1+155, INFO_Y1+100, text_firm.str_no() ); // "No" );
	}

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	String str;

	if( overseer_recno && !unit_array.is_deleted(overseer_recno) )
	{
		Unit *overseerPtr = unit_array[overseer_recno];
		// str = "Will you really attempt to assassinate the King?";
		str = text_firm.str_ask_assassin( text_unit.str_title(overseerPtr->race_id, overseerPtr->rank_id) );
		font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, str, 2, 0, 0);
	}

//	str = "You have a ";
//	str += m.format( spy_action_chance, 3 );
//	str += " chance of success!";
	str = text_firm.str_assassin_chance( spy_action_chance );

	font_whbl.center_put_paragraph( INFO_X1 +20 , INFO_Y1 + 80, INFO_X2 -10, INFO_Y1 +100, str, 2, 0, 0 );
	
	button_yes.paint();
	button_no.paint();
}
//--------- End of function Firm::disp_assassinate_confirm ---------//


//--------- Begin of function Firm::detect_assassinate_confirm ---------//
//
void FirmCamp::detect_assassinate_confirm()
{
	// action_spy_recno
	Spy *spyPtr = NULL;

	if( spy_array.is_deleted(action_spy_recno) || !(spyPtr = spy_array[action_spy_recno]) 
		|| spyPtr->spy_place != SPY_FIRM || spyPtr->spy_place_para != firm_recno 
		|| !overseer_recno || unit_array[overseer_recno]->is_own() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;		// spy die or mobilize, go back to main menu
		return;
	}

	if( button_no.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}

	if( button_yes.detect() )
	{
		firm_menu_mode = FIRM_MENU_ASSASSINATE_RESULT;
		// ###### begin Gilbert 17/6 #######//
		assassinate_result = -1;		// don't know
		// ###### end Gilbert 17/6 #######//

		spyPtr->assassinate( overseer_recno, COMMAND_PLAYER );
		se_ctrl.immediate_sound("TURN_ON");

		return;
	}
}
//--------- End of function Firm::detect_assassinate_confirm ---------//


//--------- Begin of function Firm::disp_assassinate_result ---------//
//
void FirmCamp::disp_assassinate_result(int refreshFlag)
{
//	if( refreshFlag != INFO_REPAINT )
//		return;

	if( refreshFlag == INFO_REPAINT )
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );

//	int x=INFO_X1+4, y=INFO_Y1+4, y2=y+font_san.height()-1;

	if( assassinate_result == ASSASSINATE_SUCCEED_AT_LARGE )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, "Assassination Succeeded.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, "Your Spy Escaped.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, text_firm.str_assassin_success(), 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, text_firm.str_assassin_escape(), 2, 0, 0);
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_assassin_success() );	// "Assassination Succeeded." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_assassin_escape() );	// "Your Spy Escaped." );
	}
	else if( assassinate_result == ASSASSINATE_SUCCEED_KILLED )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("EXECUTE") );
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, "Assassination Succeeded.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, "Your Spy Was Caught And Executed.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, text_firm.str_assassin_success(), 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, text_firm.str_assassin_executed(), 2, 0, 0);
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_assassin_success() );	// "Assassination Succeeded." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_assassin_executed() );	// "Your Spy Was Caught And Executed." );
	}
	// ###### begin Gilbert 17/6 ########//
	else if( assassinate_result == ASSASSINATE_FAIL )
	// ###### begin Gilbert 17/6 ########//
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("EXECUTE") );
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, "Assassination Failed.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, "Your Spy Was Caught And Executed.", 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, text_firm.str_assassin_fail(), 2, 0, 0);
		// font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1 +100, INFO_X2 -30, INFO_Y1 +200, text_firm.str_assassin_executed(), 2, 0, 0);
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_assassin_fail() );	// "Assassination Failed." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_assassin_executed() );	// "Your Spy Was Caught And Executed." );
	}
	// ###### begin Gilbert 17/6 ########//
	else		// waiting in multplayer
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
	}
	// ###### end Gilbert 17/6 ########//

	button_cancel.paint();
}
//----------- End of function FirmCamp::disp_assassinate_result -----------//


//--------- Begin of function FirmCamp::detect_assassinate_result ---------//
//
void FirmCamp::detect_assassinate_result()
{
	//----- if it is display the bribe result right now -----//

	
	if( mouse.any_click(RIGHT_BUTTON) > 0 || button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		assassinate_result = 0;
//		info.disp();
	}
}
//----------- End of function Firm::detect_assassinate_result -----------//


// ---------- begin of function FirmCamp::disp_steal_tech_confirm --------//
//
void FirmCamp::disp_steal_tech_confirm(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		button_yes.create_text( INFO_X1+50, INFO_Y1+100, text_firm.str_yes() ); //"Yes" );
		button_no.create_text( INFO_X1+155, INFO_Y1+100, text_firm.str_no() ); //"No" );
	}

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	String str;

//	str = "Do you really want to steal technology?";
	str = text_firm.str_ask_steal_tech();
	
	font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +100, str, 2, 0, 0);

//	str = "You have a ";
//	str += m.format( spy_action_chance, 3 );
//	str += " chance of success!";
	str = text_firm.str_steal_tech_chance( spy_action_chance );

	font_whbl.center_put_paragraph( INFO_X1 +20 , INFO_Y1 + 80, INFO_X2 -10, INFO_Y1 +100, str, 2, 0, 0 );
	
	button_yes.paint();
	button_no.paint();
}
// ---------- end of function FirmCamp::disp_steal_tech_confirm --------//


// ---------- begin of function FirmCamp::detect_steal_tech_confirm --------//
//
void FirmCamp::detect_steal_tech_confirm()
{
	// action_spy_recno
	Spy *spyPtr = NULL;

	if( spy_array.is_deleted(action_spy_recno) || !(spyPtr = spy_array[action_spy_recno]) 
		|| spyPtr->spy_place != SPY_FIRM || spyPtr->spy_place_para != firm_recno )
	{
		firm_menu_mode = FIRM_MENU_MAIN;		// spy die or mobilize, go back to main menu
		return;
	}

	if( button_no.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}

//	int rc = button_go.detect();
//	help.set_custom_help(button_go.x1, button_go.y1, button_go.x2, button_go.y2, "Steal Technology");
//	if( rc )
	if( button_yes.detect() )
	{
		firm_menu_mode = FIRM_MENU_STEAL_TECH_RESULT;
		steal_tech_result = 0;		// don't know

		// switch menu mode before call action, steal_tech change steal_tech_result

		spyPtr->steal_tech( nation_recno, COMMAND_PLAYER );
		se_ctrl.immediate_sound("TURN_ON");
		return;
	}
}
// --------- end of function FirmCamp::detect_steal_tech_confirm --------//


// ---------- begin of function FirmCamp::disp_steal_tech_result --------//
//
void FirmCamp::disp_steal_tech_result(int refreshFlag)
{
	// don't validate spy
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}

	String str;

	if( steal_tech_result == 0 )
	{
		// waiting for result in multi player
	}
	else if( steal_tech_result > 0 )	// success
	{
//		str = "You have stolen technology ";
//		str += tech_res[steal_tech_result]->tech_des();
//		int techLevel = tech_res[steal_tech_result]->get_nation_tech_level(nation_array.player_recno);
//		if( techLevel > 1 )
//		{
//			str += " ";
//			str += m.roman_number(techLevel);
//		}

		int techLevel = tech_res[steal_tech_result]->get_nation_tech_level(nation_array.player_recno);
		str = text_firm.str_steal_tech_success( tech_res[steal_tech_result]->tech_des(), 
			(techLevel > 1 ? techLevel : 0) );		// suppress version if version 1

		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+16, INFO_X2-14, INFO_Y1+56, str );
	}
	else if( steal_tech_result	== -1 ) // fail
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("EXECUTE") );
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_steal_tech_fail() );	// "Steal Technology Failed." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_steal_tech_executed() );	// "Your Spy Was Caught And Executed." );
	}
	else if( steal_tech_result	== -2 )	// success but no tech got
	{
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_steal_tech_fail() );	//"Steal Technology Failed." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_steal_tech_no_yield() ); // "Cannot find any useful technology." );
	}
	else
	{
		err_here();
	}

	button_cancel.paint();
}
// ---------- end of function FirmCamp::disp_steal_tech_result --------//


// ---------- begin of function FirmCamp::detect_steal_tech_result --------//
//
void FirmCamp::detect_steal_tech_result()
{
	if( mouse.any_click(RIGHT_BUTTON) > 0 || button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		steal_tech_result = 0;
	}
}
// ---------- end of function FirmCamp::detect_steal_tech_result --------//


// ---------- begin of function FirmCamp::disp_incident_nation --------//
//
void FirmCamp::disp_incident_nation(int refreshFlag)
{
	int b;

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
	String str;

	//str = "Choose other Kingdom to be involved in the Incident";
	str = text_firm.str_incident_choose();

	// US : font_whbl.center_put_paragraph( INFO_X1 +40 , INFO_Y1, INFO_X2 -30, INFO_Y1 +60, str, 2, 0, 0);
	font_whbl.center_put_paragraph( INFO_X1 +16 , INFO_Y1, INFO_X2 -6, INFO_Y1 +60, str, 2, 0, 0);

	if( refreshFlag == INFO_REPAINT )
	{
		int nationRecno;
		button_nation_count = 0;
		int defaultPushed = -1;

		for( nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno)
		{
			if( nation_array.is_deleted(nationRecno) )
				continue;

			if( nationRecno == nation_array.player_recno			// exclude target nation and own nation
				|| nationRecno == nation_recno )
				continue;

//			if( nation_array[nation_recno]->get_relation_status(nationRecno) <= RELATION_HOSTILE )
//				continue;		// exclude nation which are already hostile

//			button_nation_group[button_nation_count].create( INFO_X1+18 + 40*button_nation_count, INFO_Y1+75, 
//				INFO_X1+18 + 40*(button_nation_count+1) - 1, INFO_Y1+96, 
//				disp_nation_button, ButtonCustomPara(NULL, nationRecno), 0 );
			button_nation_group[button_nation_count].create( INFO_X1+18, INFO_Y1+55+button_nation_count*24,
				INFO_X2-10, INFO_Y1+55+(button_nation_count+1)*24-1, 
				disp_nation_button, ButtonCustomPara(NULL, nationRecno), 0 );

			if( action_target_recno == nationRecno )
				defaultPushed = button_nation_count;

			++button_nation_count;
		}

		// reset unused button

		for( b = button_nation_count; b < button_nation_group.button_num; ++b )
		{
			button_nation_group[b].reset();		// no. of button is not constant
		}

		// set pushed button

		if( defaultPushed >= 0 )
		{
			button_nation_group.push(defaultPushed, 0);
		}
		action_target_recno = button_nation_group[button_nation_group()].custom_para.value;

		// create cancel button

		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );

		button_go.create( INFO_X1+13, INFO_Y1+281, 'A', "INCIDENT" );	// BUGHERE : create incident button
	}

	// -------- display nation button group -------//

	if( button_nation_count > 0 )
	{
		for( b = 0; b < button_nation_count ; ++b )
		{
			button_nation_group[b].visible_flag = 
				!nation_array.is_deleted(button_nation_group[b].custom_para.value);
		}
		button_nation_group.paint();
	}

	button_go.enable_flag = button_nation_count > 0;
	button_go.paint();
	button_cancel.paint();
}
// ---------- end of function FirmCamp::disp_incident_nation --------//


// ---------- begin of function FirmCamp::detect_incident_nation --------//
//
void FirmCamp::detect_incident_nation()
{
	// ####### begin Gilbert 12/1 #######//
//	if( button_nation_count <= 0 )
//	{
//		firm_menu_mode = FIRM_MENU_INCIDENT_RESULT;
//		create_incident_result = -2;		// no nation to target
//		return;
//	}
	// ####### end Gilbert 12/1 #######//

	// action_spy_recno
	Spy *spyPtr = NULL;
	
	if( spy_array.is_deleted(action_spy_recno) || !(spyPtr = spy_array[action_spy_recno]) 
		|| spyPtr->spy_place != SPY_FIRM || spyPtr->spy_place_para != firm_recno )
	{
		firm_menu_mode = FIRM_MENU_MAIN;		// spy die or mobilize, go back to main menu
		return;
	}

	if( button_nation_group.detect() )
		action_target_recno = button_nation_group[button_nation_group()].custom_para.value;

	if( button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		create_incident_result = 0;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}

	int rc = button_go.detect();
	// ### patch begin Gilbert 17/3 ###//
#if(defined(CHINESE) && defined(TRADITIONAL))
//	help.set_custom_help(button_go.x1, button_go.y1, button_go.x2, button_go.y2, "挑起突發事件");
#else
//	help.set_custom_help(button_go.x1, button_go.y1, button_go.x2, button_go.y2, "Create Incident");
#endif
	// ### patch end Gilbert 17/3 ###//
	if( rc )
	{
		// switch menu mode first 
//		firm_menu_mode = FIRM_MENU_INCIDENT_RESULT;
//		create_incident_result = 0;		//waiting
//		// switch menu mode before call action, it change create_incident_result
//		spyPtr->create_incident( nation_recno, action_target_recno, COMMAND_PLAYER );

		firm_menu_mode = FIRM_MENU_INCIDENT_CONFIRM;
		spy_action_chance = spyPtr->get_incident_rating(nation_recno, action_target_recno);
		se_ctrl.immediate_sound("TURN_ON");
		return;
	}
}
// ---------- end of function FirmCamp::detect_incident_nation --------//


// ---------- begin of function FirmCamp::disp_incident_confirm --------//
//
void FirmCamp::disp_incident_confirm(int refreshFlag)
{
//	String str;
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	if( refreshFlag == INFO_REPAINT )
	{
		button_go.create( INFO_X1+13, INFO_Y1+281, 'A', "INCIDENT" );		// BUGHERE : steal tech icon
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}
	font_whbl.put_paragraph( INFO_X1+18, INFO_Y1+12, INFO_X2-14, INFO_Y1+72, 
		text_firm.str_ask_incident() );	// "Do you really want to create an incident?" );

//	str = "(";
//	str += m.format( spy_action_chance, 3 );
//	str += " chance)";
//	font_whbl.put( INFO_X1+18, INFO_Y1+73, str );
	font_whbl.put( INFO_X1+18, INFO_Y1+73, text_firm.str_incident_chance(spy_action_chance) );

	button_go.paint();
	button_cancel.paint();
}
// ---------- end of function FirmCamp::disp_incident_confirm --------//


// ---------- begin of function FirmCamp::detect_incident_confirm --------//
//
void FirmCamp::detect_incident_confirm()
{
	// action_spy_recno
	Spy *spyPtr = NULL;

	if( spy_array.is_deleted(action_spy_recno) || !(spyPtr = spy_array[action_spy_recno]) 
		|| spyPtr->spy_place != SPY_FIRM || spyPtr->spy_place_para != firm_recno )
	{
		firm_menu_mode = FIRM_MENU_MAIN;		// spy die or mobilize, go back to main menu
		return;
	}

	if( button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
		return;
	}

	int rc = button_go.detect();
	// ### patch begin Gilbert 17/3 ###//
#if(defined(CHINESE) && defined(TRADITIONAL) )
	//	help.set_custom_help(button_go.x1, button_go.y1, button_go.x2, button_go.y2, "挑起突發事件");
#else
	// help.set_custom_help(button_go.x1, button_go.y1, button_go.x2, button_go.y2, "Create Incident");
#endif
	// ### patch end Gilbert 17/3 ###//
	if( rc )
	{
		firm_menu_mode = FIRM_MENU_INCIDENT_RESULT;
		create_incident_result = 0;		// don't know

		// switch menu mode before call action, steal_tech change steal_tech_result

		spyPtr->create_incident( nation_recno, action_target_recno, COMMAND_PLAYER );
		se_ctrl.immediate_sound("TURN_ON");
		return;
	}
}
// ---------- end of function FirmCamp::detect_incident_confirm --------//


// ---------- begin of function FirmCamp::disp_incident_result --------//
//
void FirmCamp::disp_incident_result(int refreshFlag)
{
	// don't validate spy

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}

	if( create_incident_result == 0 )
	{
		// waiting for result in multi player
	}
	else if( create_incident_result > 0 )	// success
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+16, INFO_X2-14, INFO_Y1+56, 
			text_firm.str_incident_success() ); // "Create Incident Successful" );
	}
	else	// fail
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("EXECUTE") );
		font_whbl.put( INFO_X1+20, INFO_Y1+16, 
			text_firm.str_incident_fail() ); // "Create Incident Failed." );
		font_whbl.put_paragraph( INFO_X1+20, INFO_Y1+36, INFO_X2-14, INFO_Y1+76, 
			text_firm.str_incident_execute() ); // "Your Spy Was Caught And Executed." );
	}

	button_cancel.paint();
}
// ---------- end of function FirmCamp::disp_incident_result --------//


// ---------- begin of function FirmCamp::detect_incident_result --------//
//
void FirmCamp::detect_incident_result()
{
	if( mouse.any_click(RIGHT_BUTTON) > 0 || button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		create_incident_result = 0;
	}
}
// ---------- end of function FirmCamp::detect_incident_result --------//


// ---------- begin of static function disp_nation_button --------//
//
static void disp_nation_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( x1, y1, x2, y2 );
	}
	else
	{
		vga.active_buf->d3_panel_down( x1, y1, x2, y2 );
		++x1;
		++y1;
		++x2;
		++y2;
	}

	int nationRecno = button->custom_para.value;
	if( !nation_array.is_deleted(nationRecno) )
	{
		nation_array.disp_nation_color(x1+6, y1+8, nation_array[nationRecno]->nation_color);
		// vga.active_buf->bar( x1+6, y1+6, x1+16, y1+16, nation_array[nationRecno]->nation_color );
		font_bld.put( x1+22, y1+4, nation_array[nationRecno]->nation_name(), 0, x2-4 );
	}
}
// ---------- begin of static function disp_nation_button --------//
