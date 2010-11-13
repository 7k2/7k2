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

// Filename    : OF_CAMPS.CPP
// Description : FirmCamp Spy interface

#include <OF_CAMP.H>
#include <ALL.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OVBROWIF.H>
#include <OBUTT3D.H>
#include <OFONT.H>
#include <OHELP.H>
#include <OREMOTE.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <ONATION.H>
#include <OSPY.H>
#include <OUNIT.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <OCONFIG.H>
#include <OSE.H>
#include <OSERES.H>
#include <OT_FIRM.H>
#include <OT_UNIT.H>

//------------- Define coordinations -----------//

//enum { SPY_BROWSE_X1 = INFO_X1,
//		 SPY_BROWSE_Y1 = INFO_Y1+75,
//		 SPY_BROWSE_X2 = INFO_X2,
//		 SPY_BROWSE_Y2 = SPY_BROWSE_Y1+130,
//	  };
#define SPY_BROWSE_X1 INFO_X1
#define SPY_BROWSE_Y1 (INFO_Y1+75)
#define SPY_BROWSE_X2 INFO_X2
#define SPY_BROWSE_Y2 (SPY_BROWSE_Y1+130)

#define BUTTON_X1 INFO_X1
#define BUTTON_Y1 (SPY_BROWSE_Y2+28)
#define BUTTON_X2 INFO_X2
#define BUTTON_Y2 (BUTTON_Y1+50)

//----------- Define static variables ----------//

static VBrowseIF  browse_spy;
static Button3D   button_spy_menu;
static Button3D	button_spy_mobilize, button_spy_action, button_spy_reward,
						button_assassinate, button_steal_tech, button_view_secret, button_create_incident;
static Button3D	button_bribe, button_capture;
static Button3D	button_cancel;
// static FirmCamp*      firm_ptr;

//----------- Define static functions ----------//

//static int  spy_filter(int recNo=0);
//static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
static int  get_player_spy_recno(int firmRecno);

//--------- Begin of function FirmCamp::disp_spy_menu ---------//
//
void FirmCamp::disp_spy_menu(int refreshFlag)
{
	disp_soldier_list(INFO_Y1, refreshFlag, 1);			// disp own spy only
	disp_spy_info(INFO_Y1+178, refreshFlag);

	// display spy button

	Spy *spyPtr = NULL;
	if( selected_soldier_id == 0 && overseer_recno && unit_array[overseer_recno]->is_own_spy() )
	{
		spyPtr = spy_array[unit_array[overseer_recno]->spy_recno];
	}
	else if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count && soldier_array[selected_soldier_id-1].is_own_spy() )
	{
		spyPtr = spy_array[soldier_array[selected_soldier_id-1].spy_recno];
	}

	//--------- spy menu mode -----------//

	if( firm_menu_mode == FIRM_MENU_SPY )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			// ###### begin Gilbert 31/12 ######//
			button_spy_mobilize.create( INFO_X1+13, INFO_Y1+281, 'A', "MOBILSPY" );
			button_spy_mobilize.enable_flag = 0;
			if (!is_monster())
				button_spy_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "REWARD" );
			else
				button_spy_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "F_REWARD" );
			button_spy_reward.enable_flag = 0;

			button_spy_action.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "CHG-MISS" );
			button_spy_action.enable_flag = 0;
			// ###### end Gilbert 31/12 ######//

			// BUGHERE : need icon for create incident
			// #### begin Gilbert 31/12 ######//
			button_create_incident.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+235, 'A', "INCIDENT" );
			button_create_incident.enable_flag = 0;
			button_steal_tech.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+235, 'A', "STEAL_T" );
			// #### end Gilbert 31/12 ######//
			button_steal_tech.enable_flag = 0;
			button_assassinate.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "ASSASSIN" );
			button_assassinate.enable_flag = 0;
			button_view_secret.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', "VSECRET" );
			button_view_secret.enable_flag = 0;
		}

		if( spyPtr )
		{
			// ----- display mobilize button ------//

			button_spy_mobilize.enable_flag = 1;
		//	button_spy_mobilize.paint();

			// ---- display reward button -------//

			button_spy_reward.enable_flag = 1;
		//	button_spy_reward.paint();

			// ------ display change action button ------//

			button_spy_action.visible_flag = !is_own();	// cannot change action in inns
			button_spy_action.enable_flag = 1;
		//	button_spy_action.paint();

			// ------ create incident button ------//

			if( (button_create_incident.visible_flag = !is_own()) )
			{
				button_create_incident.enable_flag =
					spyPtr->spy_skill >= MIN_CREATE_INCIDENT_SPYING_SKILL
					&& tech_res[TECH_INCIDENT]->get_nation_tech_level(nation_array.player_recno);
		//		button_create_incident.paint();
			}

			//---------- steal tech button -----------//

			if( (button_steal_tech.visible_flag = !is_own()) )
			{
				button_steal_tech.enable_flag = 
					tech_res[TECH_STEAL_TECH]->get_nation_tech_level(nation_array.player_recno)
					&& spyPtr->spy_skill >= MIN_TECH_THEFT_SPYING_SKILL;
		//		button_steal_tech.paint();
			}

			//---------- view secret button -----------//

			if( (button_view_secret.visible_flag = !is_own()) )
			{
				button_view_secret.enable_flag = 
					spyPtr->spy_skill >= MIN_VIEW_SECRET_SPYING_SKILL 
					&& tech_res[TECH_INFO_THEFT]->get_nation_tech_level(spyPtr->true_nation_recno);
		//		button_view_secret.paint();
			}

			//---------- assassination button -----------//

			if( (button_assassinate.visible_flag = !is_own() && firm_res[firm_id]->need_overseer) )
			{
				button_assassinate.enable_flag = 
					spyPtr->spy_skill >= MIN_ASSASSIN_SPYING_SKILL
					&& selected_soldier_id > 0 && selected_soldier_id <= soldier_count
					&& overseer_recno	&& !unit_array[overseer_recno]->is_own();	// overseer cannot assassinate
		//		button_assassinate.paint();
			}
		}
		else
		{
			button_spy_mobilize.enable_flag = 0;
			button_spy_reward.enable_flag = 0;
			button_spy_action.enable_flag = 0;
			button_create_incident.enable_flag = 0;
			button_steal_tech.enable_flag = 0;
			button_assassinate.enable_flag = 0;
			button_view_secret.enable_flag = 0;
		}

		button_spy_mobilize.paint();
		button_spy_reward.paint();
		button_spy_action.paint();
		button_create_incident.paint();
		button_steal_tech.paint();
		button_view_secret.paint();
		button_assassinate.paint();
	}

	//--------- select briber mode --------//

	else if( firm_menu_mode == FIRM_MENU_SELECT_BRIBER )
	{
		if( refreshFlag == INFO_REPAINT )
			button_bribe.create( INFO_X1+13, INFO_Y1+235, 'A', "BRIBE" );

		if( spyPtr && spyPtr->spy_skill >= MIN_BRIBE_SPYING_SKILL )
		{
			button_bribe.enable_flag = 1;
		}
		else
		{
			button_bribe.enable_flag = 0;
		}
		button_bribe.paint();
	}
	else
		err_here();

	//----------- cancel button -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		button_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}

	button_cancel.paint();

}
//----------- End of function FirmCamp::disp_spy_menu -----------//


//--------- Begin of function FirmCamp::can_player_spy_capture ---------//
//
// return: <int> 0  - if the player can't capture this firm
//					  >0 - the recno of the player's spy who can capture this
//							 firm.
//
int FirmCamp::can_player_spy_capture()
{
	if( nation_recno == nation_array.player_recno )		// this is the player's own firm.
		return 0;

//	firm_ptr = this;
//	if( spy_filter()==0 )
//		return 0;

	//---- if the overseer is one of the player's spies -----//

	if( firm_res[firm_id]->need_overseer )
	{
		if( overseer_recno )
		{
			int spyRecno = unit_array[overseer_recno]->spy_recno;

			if( spyRecno )
			{
				if( spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
				{
					return spyRecno;
				}
			}
		}
	}

	//-- if there are no other units in the firm beside the player's spy unit --//

	else
	{
		if( soldier_count == player_spy_count )
		{
			for( int i = 0; i < soldier_count; ++i )
			{
				if( soldier_array[i].is_own_spy() )
				{
					return soldier_array[i].spy_recno;
				}
			}
			//return spy_filter( browse_spy.recno() );
		}
	}

	return 0;
}
//----------- End of function FirmCamp::can_player_spy_capture -----------//


//--------- Begin of function FirmCamp::detect_spy_menu ---------//
//
void FirmCamp::detect_spy_menu()
{
	if( detect_soldier_list(1) )		// select own spy only
		return;

	// detect_spy_info();

	Spy *spyPtr = NULL;
	if( selected_soldier_id == 0 && overseer_recno && unit_array[overseer_recno]->is_own_spy() )
	{
		spyPtr = spy_array[unit_array[overseer_recno]->spy_recno];
	}
	else if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count && soldier_array[selected_soldier_id-1].is_own_spy() )
	{
		spyPtr = spy_array[soldier_array[selected_soldier_id-1].spy_recno];
	}

	// detect spy button

	if( firm_menu_mode == FIRM_MENU_SPY )
	{
		if( button_spy_mobilize.detect() )
		{
			if( !remote.is_enable() )
			{
				if( spyPtr->mobilize_firm_spy() )
				{
					spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it
//					info.disp();
//					return;
				}
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_FIRM, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( button_spy_reward.detect() )
		{
			spyPtr->reward(COMMAND_PLAYER);
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( button_spy_action.detect() )
		{
			if( !remote.is_enable() )
			{
				spyPtr->set_next_action_mode();
				// disp_spy_menu( INFO_UPDATE );
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CYCLE_ACTION, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( button_create_incident.detect() )
		{
			// spyPtr->create_incident( nation_recno, -1, COMMAND_PLAYER );
			firm_menu_mode  = FIRM_MENU_INCIDENT_NATION;
			action_spy_recno = spyPtr->spy_recno;
			se_ctrl.immediate_sound("TURN_ON");
		}
//		if( button_create_incident.visible_flag )
//		{
//			help.set_custom_help(button_create_incident.x1, button_create_incident.y1, 
//				button_create_incident.x2, button_create_incident.y2, "Create Incident" );
//		}

		if( button_steal_tech.detect() )
		{
			// spyPtr->steal_tech( nation_recno, COMMAND_PLAYER );
			action_spy_recno = spyPtr->spy_recno;
			firm_menu_mode   = FIRM_MENU_STEAL_TECH_CONFIRM;
			spy_action_chance = spyPtr->get_steal_rating(nation_recno);
			se_ctrl.immediate_sound("TURN_ON");
		}
//		if( button_steal_tech.visible_flag )
//		{
//			help.set_custom_help(button_steal_tech.x1, button_steal_tech.y1, 
//				button_steal_tech.x2, button_steal_tech.y2, "Steal Technology" );
//		}

		if( button_assassinate.detect() )
		{
			int attackRating, defenseRating, otherDefenderCount;
			if( spyPtr->get_assassinate_rating(overseer_recno, attackRating, defenseRating, otherDefenderCount) )
			{
				action_spy_recno = spyPtr->spy_recno;
				firm_menu_mode   = FIRM_MENU_ASSASSINATE_CONFIRM;
				spy_action_chance = attackRating - defenseRating;
				if( !config.show_debug_info )		// to show the actual value, not bounded to [0,100]
				{
					// ###### begin Gilbert 27/1 ######//
					spy_action_chance = max( spy_action_chance, 0 );
					spy_action_chance = min( spy_action_chance, 100 );
					// ###### end Gilbert 27/1 ######//
				}
				se_ctrl.immediate_sound("TURN_ON");
			}
		}

		if( button_view_secret.detect() )
		{
			action_spy_recno = spyPtr->spy_recno;
			firm_menu_mode   = FIRM_MENU_VIEW_SECRET;
			se_ctrl.immediate_sound("TURN_ON");
			// info.disp();
		}
	}
	else if( firm_menu_mode == FIRM_MENU_SELECT_BRIBER )
	{
		if( button_bribe.detect() )
		{
			action_spy_recno = spyPtr->spy_recno;
			firm_menu_mode   = FIRM_MENU_SET_BRIBE_AMOUNT;
			se_ctrl.immediate_sound("TURN_ON");
		}
	}
	else
	{
		err_here();
	}

	if( button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		se_ctrl.immediate_sound("TURN_OFF");
	}
}
//----------- End of function FirmCamp::detect_spy_menu -----------//


//--------- Begin of function FirmCamp::disp_spy_button --------//
//
void FirmCamp::disp_spy_button(int, int , int refreshFlag)
{
//	if( !is_own() )		// if not own firm, there is not other button other than the spy button, so always display it left-aligned
//		x = INFO_X1;		// if own firm, the x passed will be space position on the interface already, taking into account of the other buttons on interface

	//-------------------------------------//

	if( refreshFlag == INFO_REPAINT )
	{
		// ##### begin Gilbert 31/12 ####//
//		button_spy_menu.create( INFO_X1+13+BUTTON_DISTANCE*2, INFO_Y1+281, 'A', "SPYMENU" );
//		button_bribe.create( INFO_X1+13+BUTTON_DISTANCE , INFO_Y1+281, 'A', "SELBRIBE" );

		button_spy_menu.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "SPYMENU" );
		button_bribe.create( INFO_X1+13+2*BUTTON_DISTANCE , INFO_Y1+235, 'A', "SELBRIBE" );
		button_capture.create( INFO_X1+13, INFO_Y1+281, 'A', "CAPTURE" );
		// ##### end Gilbert 31/12 ####//
	}

	button_spy_menu.enable_flag = 0;
	button_bribe.enable_flag = 0;

	if( player_spy_count > 0 )
	{
		//------------ spy menu button-----------//

		button_spy_menu.enable_flag = 1;
		button_spy_menu.paint();

		// x += BUTTON_ACTION_WIDTH;

		//---------- bribe button -----------//

		if( nation_recno != nation_array.player_recno ) 	// only display the bribe button for non-player owned towns
		{
			//-------- display the bribe button -------//

			button_bribe.enable_flag = (selected_soldier_id || overseer_recno) ?
				can_spy_bribe(selected_soldier_id, nation_array.player_recno ) : 0;
			button_bribe.paint();

			// x += BUTTON_ACTION_WIDTH;
		}

		//--------- capture button ----------//

		if( (button_capture.visible_flag = !is_own()) )
		{
			button_capture.enable_flag = can_soldier_capture(nation_array.player_recno);
		}
		button_capture.paint();

	}

}
//--------- End of function FirmCamp::disp_spy_button --------//


//--------- Begin of function FirmCamp::detect_spy_button --------//

void FirmCamp::detect_spy_button()
{
	if( player_spy_count>0 )
	{
		if( button_spy_menu.detect() )
		{
			firm_menu_mode = FIRM_MENU_SPY;
			se_ctrl.immediate_sound("TURN_ON");
			//info.disp();
		}

		if( nation_recno != nation_array.player_recno )		// only display the bribe button for non-player towns
		{
			if( button_bribe.detect() )
			{
				if( player_spy_count > 1 )
				{
					firm_menu_mode = FIRM_MENU_SELECT_BRIBER;
					bribe_result = BRIBE_NONE;
					action_target_recno = selected_soldier_id;	// recno of bribe/assassinate target
				}
				else if( player_spy_count == 1 )
				{
					firm_menu_mode   = FIRM_MENU_SET_BRIBE_AMOUNT;
					bribe_result = BRIBE_NONE;
					action_spy_recno = get_player_spy_recno(firm_recno);		// the player has only one spy in this firm
					action_target_recno = selected_soldier_id;
				}
				se_ctrl.immediate_sound("TURN_ON");
				// info.disp();
			}
		}
	}

	//-----------------------------------------//

	if( button_capture.detect() && can_soldier_capture(nation_array.player_recno) )
	{
		// ###### begin Gilbert 17/6 ########//

		// find if any spy, if any use spy_capture_firm
		int captureSpyRecno = 0;
		if( overseer_recno && unit_array[overseer_recno]->spy_recno
			&& unit_array[overseer_recno]->true_nation_recno() == nation_array.player_recno )
		{
			captureSpyRecno = unit_array[overseer_recno]->spy_recno;
		}

		if( captureSpyRecno )
		{
			if( !remote.is_enable() )
			{
				spy_array[captureSpyRecno]->capture_firm();
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CAPTURE_FIRM, sizeof(short) );
				*shortPtr = captureSpyRecno;
			}
		}
		else
		{
			if( !remote.is_enable() )
			{
				capture_firm(nation_array.player_recno);               // update RemoteMsg::firm_capture
			}
			else
			{
				// packet structure <firm recno> <capturer's nation recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_CAPTURE, 2*sizeof(short) );
				*shortPtr = firm_recno;
				shortPtr[1] = nation_array.player_recno;
			}
		}
		// ###### end Gilbert 17/6 ########//
		se_ctrl.immediate_sound("TURN_ON");
	}
}
//--------- End of function FirmCamp::detect_spy_button --------//


//--------- Begin of function get_player_spy_recno --------//
//
static int get_player_spy_recno(int firmRecno)
{
	int  spyCount=spy_array.size();
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_FIRM &&
			 spyPtr->spy_place_para==firmRecno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			return i;
		}
	}

	return 0;
}
//------- End of function get_player_spy_recno ---------//


//--------- Begin of function FirmCamp::can_soldier_capture --------//
//
// Return whether current soldiers of <captureNationRecno> can
// capture this firm or not.
//
int FirmCamp::can_soldier_capture(int captureNationRecno)
{
	if( nation_recno == captureNationRecno )		// cannot capture its own firm
		return 0;

	// ------- see Spy::capture_firm for the condition -------//

	err_when( !firm_res[firm_id]->need_overseer );

	return overseer_recno && unit_array[overseer_recno]->spy_recno
		&& unit_array[overseer_recno]->true_nation_recno() == captureNationRecno;

	//----- if this firm needs an overseer, can only capture it when the overseer is the spy ---//

//	if( firm_res[firm_id]->need_overseer )
//	{
//		return overseer_recno &&
//				 unit_array[overseer_recno]->true_nation_recno() == captureNationRecno;
//	}

	//--- if this firm doesn't need an overseer, can capture it if all the units in the firm are the player's spies ---//

//	Soldier* soldierPtr = soldier_array;
//	int 	  captureUnitCount=0, otherUnitCount=0;
//
//	for( int i=0 ; i<soldier_count ; i++, soldierPtr++ )
//	{
//		if( soldierPtr->spy_recno &&
//			 spy_array[soldierPtr->spy_recno]->true_nation_recno == captureNationRecno )
//		{
//			captureUnitCount++;
//			continue;
//		}
//		else
//		{
//			otherUnitCount++;		// must be an own unit in camps and bases if the unit is not a spy
//		}
//	}
//	return captureUnitCount>0 && otherUnitCount==0;
}
//--------- End of function FirmCamp::can_soldier_capture --------//


// -------- begin of function FirmCamp::disp_spy_info -------//
//
void FirmCamp::disp_spy_info(int dispY1, int refreshFlag)
{
	char *unitName;
	String str;
	short unitId;
	int unitLoyalty;
	int unitCombat;
	int unitSpying;
	int unitHp;
	int unitMaxHp;
	Spy *spyPtr = NULL;

	int x=INFO_X1+20, y=dispY1;
	int x2;

	if( selected_soldier_id==0 && overseer_recno )		// display overseer info
	{
		Unit *unitPtr = unit_array[overseer_recno];
		if( unitPtr->spy_recno )
			spyPtr = spy_array[unitPtr->spy_recno];

		unitId = unitPtr->unit_id;
		str = unitPtr->unit_name();	// don't display name_id, can spot spy
		str += " (";
		if( unitPtr->is_human() )
			str += race_res[unitPtr->race_id]->name;
		else
			str += monster_res[unitPtr->monster_id()]->name;
		str += ")";
		unitName = str;
		unitLoyalty = unitPtr->loyalty;

		unitCombat = unitPtr->combat_level();
		unitHp 	  = (int)unitPtr->hit_points;
		unitMaxHp  = unitPtr->max_hit_points();
	}
	else if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		Soldier *soldierPtr = soldier_array + selected_soldier_id - 1;
		if( soldierPtr->spy_recno )
			spyPtr = spy_array[soldierPtr->spy_recno];

		unitId = soldierPtr->unit_id;
		unitName = unit_res[unitId]->name;	// don't display name_id, can spot spy
		unitLoyalty = soldierPtr->loyalty;

		unitCombat = soldierPtr->combat_level();
		unitHp 	  = soldierPtr->hit_points;
		unitMaxHp  = soldierPtr->max_hit_points();
	}

	if( !spyPtr )
		return;

	unitLoyalty = spyPtr->spy_loyalty;
	unitSpying = spyPtr->spy_skill;

//	font_snds.put( x, y, unitName );
	font_snds.put( x, y, unitName, 0, INFO_X2-8, 1 );

	// line spacing 24
	if( unit_res[unitId]->race_id )
	{
		err_when( unitId == UNIT_WAGON );

		// ------- display loyalty ---------//

	//	info.disp_loyalty( x, y+14, x+60, unitLoyalty, unitLoyalty, nation_recno, refreshFlag );
		info.disp_loyalty( INFO_X1+20, y+12, INFO_X2-99 - font_snds.text_width(m.format(unitLoyalty, 4)),
							unitLoyalty, unitLoyalty, nation_recno, refreshFlag );
	
		// ------- display combat ----------//

		// x2 = font_snds.put( x+110, y+12, "Combat" ) + 10;
		x2 = font_snds.put( x+110, y+12, text_unit.str_combat_level() ) + 10;
		font_snds.right_put( INFO_X2-10, y+12, m.format(unitCombat, 4) );
	//	font_snds.put( x2, y+14, unitCombat );

	}
//		else
//			font_san.field( x, y, "Loyalty", x+100, "N/A", INFO_X2-2, refreshFlag );		// no loyalty because it does not belong to your empire

	// ------- display spying  -------//

//	x2 = font_snds.put( x+110, y+26, "Spying" ) + 10;
	x2 = font_snds.put( x+110, y+26, text_unit.str_spy_skill() ) + 10;
	font_snds.right_put( INFO_X2-10, y+26, m.format(unitSpying, 4) );
//	font_snds.put( x2, y+28, unitSpying );

	// -------- display spy action ------//

	font_snds.put( x, y+28, spyPtr->action_str() );

	// ----- display hit point ---------//
//	x2 = font_snds.put( x, y+28, "Hit Points" ) + 10;
//	x2 = font_snds.put( x2, y+28, unitHp );
//	x2 = font_snds.put( x2, y+28, "/" );
//	x2 = font_snds.put( x2, y+28, unitMaxHp );
}
// -------- end of function FirmCamp::disp_spy_info -------//
