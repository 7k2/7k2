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

//Filename    : OSPY.CPP
//Description : Object Spy

#include <ospy.h>
#include <opower.h>
#include <ogame.h>
#include <odate.h>
#include <onews.h>
#include <ofont.h>
#include <oimgres.h>
#include <ounit.h>
#include <oworld.h>
#include <obutton.h>
#include <obutt3d.h>
#include <obuttcus.h>
#include <ofirm.h>
#include <ofirmall.h>
#include <otown.h>
#include <onation.h>
#include <oraceres.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <osys.h>
#include <otechres.h>
#include <otech.h>
#include <ose.h>
#include <ot_firm.h>
#include <ot_reps.h>

//----- Define constants for viewing secret menu ------//

//#define SECRET_REPORT_COUNT 7
//static char* 	secret_report_str_array[] = { "Kingdoms", "Towns", "Economy", "Trade", "Military", "Technology", "Espionage" };
//static char	 	secret_view_mode_array[]  = { MODE_NATION, MODE_TOWN, MODE_ECONOMY, MODE_TRADE, MODE_MILITARY, MODE_TECH, MODE_SPY };
//static char	 	secret_view_skill_array[] = { 40, 20, 30, 30, 50, 40, 90 };
#define SECRET_REPORT_COUNT 6
//static char* 	secret_report_str_array[] = { "Kingdoms", "Towns", "Economy", "Trade", "Military", "Technology" };
static char	 	secret_view_mode_array[]  = { MODE_NATION, MODE_TOWN, MODE_ECONOMY, MODE_TRADE, MODE_MILITARY, MODE_TECH };
static char	 	secret_view_skill_array[] = { 40, 20, 30, 30, 50, 40 };

static ButtonCustomGroup	button_secret_report_array(SECRET_REPORT_COUNT);
static Button3D	button_view_secret;
static Button3D	button_secret_report_cancel;

// ---------- declare static function -------------//

static void disp_text_button(ButtonCustom *, int);

//--------- Begin of function SpyArray::SpyArray ----------//

SpyArray::SpyArray() : DynArrayB(sizeof(Spy), 10)
{
}
//--------- End of function SpyArray::SpyArary ----------//


//------- Begin of function SpyArray::~SpyArray ----------//
//
SpyArray::~SpyArray()
{
	deinit();
}
//--------- End of function SpyArray::~SpyArray ----------//


//--------- Begin of function SpyArray::init ----------//
//
void SpyArray::init()
{
}
//---------- End of function SpyArray::init ----------//


//--------- Begin of function SpyArray::deinit ----------//
//
void SpyArray::deinit()
{
	if( size()==0 )
		return;

	// ####### begin Gilbert 22/3 #######//
	//----- delete units ------//

	for( int i=1 ; i<=size() ; i++ )
	{
		if( is_deleted(i) )
			continue;

		del_spy(i);
	}
	// ####### end Gilbert 22/3 #######//

	//-------- zap the array -----------//

	zap();
}
//---------- End of function SpyArray::deinit ----------//


//--------- Begin of function SpyArray::add_spy ----------//
//
// <int> unitRecno - unit recno of the spy
// <int> spySkill  - spying skill of the unit
// [int] trueNation - true nation recno (default -1:unit's nation)
//
// return: <int> recno of the spy record added
//
int SpyArray::add_spy(int unitRecno, int spySkill, int trueNation)
{
	Spy 	spy;
	Unit* unitPtr = unit_array[unitRecno];

	memset( &spy, 0, sizeof(spy) );

	spy.spy_place 			= SPY_MOBILE;
	spy.spy_place_para	= unitRecno;
	spy.spy_skill 			= spySkill;
	spy.spy_loyalty 		= unitPtr->loyalty;
	spy.race_id			   = unitPtr->race_id;
	spy.is_civilian		= unitPtr->is_civilian();
	spy.name_id				= unitPtr->name_id;
	// ####### begin Gilbert 24/2 #######//
	spy.unique_id        = unitPtr->unique_id;
	// ####### end Gilbert 24/2 #######//

	err_when( unitPtr->race_id < 1 || unitPtr->race_id > MAX_RACE );

	err_when( nation_array.is_deleted(unitPtr->nation_recno) );

	err_when( trueNation == 0 );
	if( trueNation < 0 )
		trueNation = unitPtr->nation_recno;

	spy.true_nation_recno    = trueNation;
	spy.cloaked_nation_recno = unitPtr->nation_recno;

	//--- spies hold a use right of the name id even though the unit itself will register the usage right of the name already ---//

	race_res[spy.race_id]->use_name_id(spy.name_id);		// the spy will free it up in deinit(). Keep an additional right because when a spy is assigned to a town, the normal program will free up the name id., so we have to keep an additional copy

	// ------- set camouflage vars --------//

	spy.camouflage_count = 0;
	spy.camouflage_power = spySkill / 4;

	//------- link in the spy_array -------//

	linkin( &spy );

	((Spy*)get())->spy_recno = recno();

	return recno();
}
//---------- End of function SpyArray::add_spy ----------//


//--------- Begin of function SpyArray::add_spy ----------//
//
// This overloaded version of add_spy() just add a spy without
// setting parameters of the Spy.
//
// return: <int> recno of the spy record added
//
int SpyArray::add_spy()
{
	Spy spy;

	memset( &spy, 0, sizeof(spy) );

	linkin( &spy );

	((Spy*)get())->spy_recno = recno();

	return recno();
}
//---------- End of function SpyArray::add_spy ----------//


//--------- Begin of function SpyArray::del_spy ----------//
//
// <int> spyRecno - recno of the spy to be deleted
//
void SpyArray::del_spy(int spyRecno)
{
	spy_array[spyRecno]->deinit();

	linkout(spyRecno);
}
//---------- End of function SpyArray::del_spy ----------//


//--------- Begin of function SpyArray::next_day ----------//
//
void SpyArray::next_day()
{
	int  spyCount = size();
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		spyPtr->next_day();

		if( spy_array.is_deleted(i) )
			continue;

		if( nation_array[spyPtr->true_nation_recno]->is_ai() )
			spyPtr->process_ai();
	}

	//---------- update Firm::sabotage_level ----------//

	if( info.game_date%15==0 )
		process_sabotage();
}
//---------- End of function SpyArray::next_day ----------//


//--------- Begin of function SpyArray::find_town_spy ----------//
//
// Find a spy meeting the specific criteria
//
// <int> townRecno - town recno of the spy to find
// <int> raceId    - race id. of the spy to find
// <int> spySeq	 - sequence id. of the spy in spy_array
//
// return: <int> recno of the spy found
//
int SpyArray::find_town_spy(int townRecno, int spySeq)
{
	int  spyCount=size(), matchCount=0;
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==townRecno )
		{

			if( ++matchCount == spySeq )
				return i;
		}
	}

	return 0;
}
//---------- End of function SpyArray::find_town_spy ----------//


//--------- Begin of function SpyArray::process_sabotage ----------//
//
void SpyArray::process_sabotage()
{
	/*
	Spy*  spyPtr;
	Firm* firmPtr;

	//-------- reset firms' sabotage_level -------//

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firm_array[i]->sabotage_level = 0;
	}

	//------- increase firms' sabotage_level -----//

	for( i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->action_mode == SPY_SABOTAGE )
		{
			err_when( spyPtr->spy_place != SPY_FIRM );

			firmPtr = firm_array[spyPtr->spy_place_para];

			firmPtr->sabotage_level += spyPtr->spy_skill/5;

			if( firmPtr->sabotage_level > 100 )
				firmPtr->sabotage_level = 100;
		}
	}
	*/
}
//---------- End of function SpyArray::process_sabotage ----------//


//--------- Begin of function SpyArray::mobilize_all_spy ----------//
//
// Mobilize all spies of the specific nation in the specific place.
//
// <int> spyPlace     - place id. of the spy
// <int> spyPlacePara - town or firm recno of the spy's staying
// <int> nationRecno	 - recno of the nation which the spy should
//							   be mobilized.
//
void SpyArray::mobilize_all_spy(int spyPlace, int spyPlacePara, int nationRecno)
{
	Spy* spyPtr;

	for( int i=size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place == spyPlace &&
			 spyPtr->spy_place_para == spyPlacePara &&
			 spyPtr->true_nation_recno == nationRecno )
		{
			if( spyPtr->spy_place == SPY_TOWN )
				spyPtr->mobilize_town_spy();

			else if( spyPtr->spy_place == SPY_FIRM )
				spyPtr->mobilize_firm_spy();
		}
	}
}
//---------- End of function SpyArray::mobilize_all_spy ----------//


//--------- Begin of function SpyArray::disp_view_secret_menu ---------//
//
void SpyArray::disp_view_secret_menu(int spyRecno, int refreshFlag)
{
//	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );
	//------------------------------------//

//	vga_util.d3_panel_up( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+42 );
//	font_san.put_paragraph( INFO_X1+7, INFO_Y1+5, INFO_X2-7, INFO_Y2-5,
//									"Steal which type of secrets?" );
	font_whbl.put_paragraph( INFO_X1+18, INFO_Y1+12, INFO_X2-14, INFO_Y1+72, 
		text_firm.str_spy_steal_info() ); // "Which category of information do you wish to view?" );

	//------------------------------------//

	int  y=INFO_Y1+45;

	err_when( spy_array.is_deleted(spyRecno) );

	Spy* spyPtr = spy_array[spyRecno];

	int i;
	for( i=0 ; i<SECRET_REPORT_COUNT ; i++ )
	{
		if( refreshFlag == INFO_REPAINT )
		{
//			button_secret_report_array[i].create( INFO_X1+18, y, INFO_X2-10, y+21, 
//				disp_text_button, ButtonCustomPara( secret_report_str_array[i], i), 0 );
			button_secret_report_array[i].create( INFO_X1+18, y, INFO_X2-10, y+21, 
				disp_text_button, ButtonCustomPara( text_reports.str_report_mode(i+1), i), 0 );
		}

		button_secret_report_array[i].enable_flag = 
			tech_res[TECH_INFO_THEFT]->get_nation_tech_level(spyPtr->true_nation_recno) 
			&& spyPtr->spy_skill >= secret_view_skill_array[i];

		y+=23;
	}

	if( refreshFlag == INFO_REPAINT )
	{
		// push the first enabled button
		for( i = 0; i < SECRET_REPORT_COUNT; i++ )
		{
			if( button_secret_report_array[i].enable_flag )
			{
				button_secret_report_array.push(i, 0);
				break;
			}
		}
	}
	button_secret_report_array.paint();

	if( refreshFlag == INFO_REPAINT )
	{
		button_view_secret.create( INFO_X1+13, INFO_Y1+281, 'A', "VSECRET" );
		button_secret_report_cancel.create( INFO_X1+13+3*BUTTON_DISTANCE, INFO_Y1+281, 'A', "PREVMENU" );
	}
	button_view_secret.enable_flag = button_secret_report_array[button_secret_report_array()].enable_flag;
	button_view_secret.paint();
	button_secret_report_cancel.paint();
}
//----------- End of function SpyArray::disp_view_secret_menu -----------//


//--------- Begin of function SpyArray::detect_view_secret_menu ---------//
//
// <int> spyRecno    - recno of the spy to view secret info.
// <int> nationRecno - recno of the nation which this spy is going to investigate
//
int SpyArray::detect_view_secret_menu(int spyRecno, int nationRecno)
{
	//---- detect secret report button ----//

	int rc=0;

	button_secret_report_array.detect();

	if( button_view_secret.detect() )
	{
		sys.set_view_mode( secret_view_mode_array[button_secret_report_array()],
			nationRecno, spyRecno );
		rc=1;
		se_ctrl.immediate_sound("TURN_ON");
	}

	//-------- detect cancel button --------//

	if( button_secret_report_cancel.detect() )
	{
		rc = 1;
		se_ctrl.immediate_sound("TURN_OFF");
	}

	return rc;
}
//----------- End of function SpyArray::detect_view_secret_menu -----------//


//--------- Begin of function SpyArray::update_firm_spy_count ---------//
//
// Update the player_spy_count of this firm. This function is called
// when the firm change its nation.
//
// <int> firmRecno - recno of the firm to be updated.
//
void SpyArray::update_firm_spy_count(int firmRecno)
{
	//---- recalculate Firm::player_spy_count -----//

	Spy*  spyPtr;
	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->cast_to_FirmCamp() )
	{
		FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
		firmCamp->player_spy_count = 0;
		for( int i=spy_array.size() ; i>0 ; i-- )
		{
			if( spy_array.is_deleted(i) )
				continue;

			spyPtr = spy_array[i];

			if( spyPtr->spy_place == SPY_FIRM &&
				 spyPtr->spy_place_para == firmRecno &&
				 spyPtr->true_nation_recno == nation_array.player_recno )
			{
				firmCamp->player_spy_count++;
			}
		}
	}
	else if( firmPtr->cast_to_FirmTrain() )
	{
		FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();
		firmTrain->player_spy_count = 0;
		for( int i=spy_array.size() ; i>0 ; i-- )
		{
			if( spy_array.is_deleted(i) )
				continue;

			spyPtr = spy_array[i];

			if( spyPtr->spy_place == SPY_FIRM &&
				 spyPtr->spy_place_para == firmRecno &&
				 spyPtr->true_nation_recno == nation_array.player_recno )
			{
				firmTrain->player_spy_count++;
			}
		}
	}

	return;
}
//----------- End of function SpyArray::update_firm_spy_count -----------//


//--------- Begin of function SpyArray::change_cloaked_nation ---------//
//
// Change the cloak of all the spies in the specific place.
//
// This function is called when a firm or town change nation.
//
// <int> spyPlace        - spy place
// <int> spyPlacePara    - spy place para
// <int> fromNationRecno - change any spies in the place whose cloaked_nation_recno
// <int> toNationRecno     is fromNationRecno to toNationRecno.
//
void SpyArray::change_cloaked_nation(int spyPlace, int spyPlacePara, int fromNationRecno, int toNationRecno)
{
	Spy* spyPtr;

	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->cloaked_nation_recno != fromNationRecno )
			continue;

		if( spyPtr->spy_place != spyPlace )
			continue;

		//--- check if the spy is in the specific firm or town ---//

		if( spyPlace == SPY_FIRM || spyPlace == SPY_TOWN )  // only check spy_place_para when spyPlace is SPY_TOWN or SPY_FIRM
		{
			if( spyPtr->spy_place_para != spyPlacePara )
				continue;
		}

		if(spyPlace==spyPtr->spy_place && spyPlacePara==spyPtr->spy_place_para &&
			spyPtr->true_nation_recno==toNationRecno)
			spyPtr->set_action_mode(SPY_IDLE);

		//----- if the spy is associated with a unit (mobile or firm overseer), we call Unit::spy_chnage_nation() ---//

		if( spyPlace == SPY_FIRM )
		{
			FirmCamp *firmCamp = firm_array[spyPtr->spy_place_para]->cast_to_FirmCamp();
			if( firmCamp )
			{
				int firmOverseerRecno = firmCamp->overseer_recno;

				if( firmOverseerRecno && unit_array[firmOverseerRecno]->spy_recno == i )
				{
					unit_array[firmOverseerRecno]->spy_change_nation(toNationRecno, COMMAND_AUTO);
					continue;
				}
			}
		}
		else if( spyPlace == SPY_MOBILE )
		{
			unit_array[spyPtr->spy_place_para]->spy_change_nation(toNationRecno, COMMAND_AUTO);
			continue;
		}

		//---- otherwise, just change the spy cloak ----//

		spyPtr->cloaked_nation_recno = toNationRecno;
	}
}
//----------- End of function SpyArray::change_cloaked_nation -----------//


//--------- Begin of function SpyArray::total_spy_skill_level ---------//
//
// Calculate the combined skill levels of all the spies of the
// specific nation in the specific place.
//
// <int> spyPlace        - spy place
// <int> spyPlacePara    - spy place para
// <int> spyNationRecno  - nation recno
// <int&> spyCount		 - the total no. of spies meeting the criteria.
//
int SpyArray::total_spy_skill_level(int spyPlace, int spyPlacePara, int spyNationRecno, int& spyCount)
{
	int  totalSpyLevel=0;
	Spy* spyPtr;

	spyCount = 0;

	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->true_nation_recno != spyNationRecno )
			continue;

		if( spyPtr->spy_place != spyPlace )
			continue;

		if( spyPtr->spy_place_para != spyPlacePara )
			continue;

		spyCount++;

		totalSpyLevel += spyPtr->spy_skill;
	}

	return totalSpyLevel;
}
//----------- End of function SpyArray::total_spy_skill_level -----------//


//-------- Begin of function SpyArray::catch_spy ------//
//
// <int> spyPlace - either SPY_TOWN or SPY_FIRM
// <int> spyPlacePara - town_recno or firm_recno
//
int SpyArray::catch_spy(int spyPlace, int spyPlacePara)
{
	int nationRecno, totalPop;

	if( spyPlace == SPY_TOWN )
	{
		Town* townPtr = town_array[spyPlacePara];

		nationRecno = townPtr->nation_recno;
		totalPop    = townPtr->population;
	}
	else if( spyPlace == SPY_FIRM )
	{
		Firm* firmPtr = firm_array[spyPlacePara];

		nationRecno = firmPtr->nation_recno;
		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
			totalPop    = firmCamp->soldier_count + (firmCamp->overseer_recno>0);
		}
		else if( firmPtr->cast_to_FirmWork() )
		{
			FirmWork *firmWork = firmPtr->cast_to_FirmWork();
			totalPop    = firmWork->worker_count;
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			totalPop = firmPtr->cast_to_FirmTrain()->trainee_count;
			return 0;		// don't catch spy
		}
		else if( firmPtr->cast_to_FirmInn() )
		{
			totalPop = firmPtr->cast_to_FirmInn()->inn_unit_count;
			return 0;		// don't catch spy
		}
		else if( firmPtr->cast_to_FirmMarket() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmMonsterTrain() )
		{
			totalPop = firmPtr->cast_to_FirmMonsterTrain()->trainee_count;
			return 0;		// don't catch spy
		}
		else if( firmPtr->cast_to_FirmMonsterAlchemy() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmLishorr() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmMonsterFortress() )
		{
			FirmMonsterFortress *firmMonsterFortress = firmPtr->cast_to_FirmMonsterFortress();
			totalPop = firmMonsterFortress->archer_count + firmMonsterFortress->extra_builder_count;
		}
		else if( firmPtr->cast_to_FirmAnimal() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmIncubator() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmMagic() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmOffensive() )
		{
			totalPop = 0;
			return 0;
		}
		else if( firmPtr->cast_to_FirmOffensive2() )
		{
			totalPop = 0;
			return 0;
		}
		else
		{
			err_here();
			totalPop = 0;
		}
	}
	else
		err_here();

	//--- calculate the total of anti-spy skill in this town ----//

	int enemySpyCount=0, counterSpySkill=0;
	Spy* spyPtr;

	int techLevel = 0;
	if( nationRecno )
		techLevel = tech_res[TECH_COUNTER_SPY]->get_nation_tech_level(nationRecno);

	int i;
	for( i=size() ; i>0 ; i-- )
	{
		if( is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place == spyPlace &&
			 spyPtr->spy_place_para == spyPlacePara )
		{
			if( spyPtr->true_nation_recno == nationRecno )
				counterSpySkill += spyPtr->spy_skill + spyPtr->spy_skill * techLevel / 2;
			else
				enemySpyCount++;
		}
	}

	//----- if all citizens are enemy spies ----//

	if( enemySpyCount == totalPop )
		return 0;

	err_when( enemySpyCount > totalPop );

	//-------- try to catch enemy spies now ------//

	for( i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->action_mode == SPY_IDLE )		// it is very hard to get caught in sleep mode
			continue;

		if( spyPtr->spy_place == spyPlace &&
			 spyPtr->spy_place_para == spyPlacePara &&
			 spyPtr->true_nation_recno != nationRecno )	// doesn't get caught in sleep mode
		{
			int escapeChance = 100 + spyPtr->spy_skill - counterSpySkill;

			escapeChance = MAX( spyPtr->spy_skill/10, escapeChance );

			if( m.random(escapeChance) == 0 )
			{
				spyPtr->get_killed(); 		// only catch one spy per calling
				return 1;
			}
		}
	}

	return 0;
}
//---------- End of function SpyArray::catch_spy --------//


//--------- Begin of function SpyArray::set_action_mode ----------//
//
// Set all spies in the given place to the specific action mode.
//
void SpyArray::set_action_mode(int spyPlace, int spyPlacePara, int actionMode)
{
	int  spyCount=size();
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==spyPlace &&
			 spyPtr->spy_place_para==spyPlacePara )
		{
			spyPtr->set_action_mode(actionMode);
		}
	}
}
//---------- End of function SpyArray::set_action_mode ----------//


//--------- Begin of function SpyArray::ai_spy_town_rebel ----------//
//
// Tell the AI spies in the town that a rebellion is happening.
//
// When a rebellion happens, all the AI spies in the town will mobilize
// and turn its cloak back to a nation that is not at war with the enemy
// (and notification flag should be off.) and move to a safe place
// (near to one of your towns). Then the spy reaches thedestination, it will
// become idle and then the AI processing function on idle spy will be
// called and handle the spy.
//
// <int> townRecno - recno of the town with rebellion happening.
//
void SpyArray::ai_spy_town_rebel(int townRecno)
{
	int  spyCount=size();
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==townRecno &&
			 nation_array[spyPtr->true_nation_recno]->is_ai() )
		{
			//-------- mobilize the spy ----------//

			int unitRecno = spyPtr->mobilize_town_spy();

			//----- think new action for the spy ------//

			if( unitRecno )
				spyPtr->think_mobile_spy_new_action();
		}
	}
}
//---------- End of function SpyArray::ai_spy_town_rebel ----------//


//--------- Begin of function SpyArray::needed_view_secret_skill ----------//
//
int SpyArray::needed_view_secret_skill(int viewMode)
{
	for( int i=0 ; i<SECRET_REPORT_COUNT ; i++ )
	{
		if( secret_view_mode_array[i] == viewMode )
			return secret_view_skill_array[i];
	}

	return 0;
}
//---------- End of function SpyArray::needed_view_secret_skill ----------//


#ifdef DEBUG

//------- Begin of function SpyArray::operator[] -----//

Spy* SpyArray::operator[](int recNo)
{
	Spy* spyPtr = (Spy*) get(recNo);

	if( !spyPtr || spyPtr->spy_recno==0 )
		err.run( "SpyArray[] is deleted" );

	return spyPtr;
}

//--------- End of function SpyArray::operator[] ----//

#endif


static void disp_text_button(ButtonCustom *button, int)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	// modify x1,y1, x2,y2 to the button body
	if( button->pushed_flag )
	{
		vga.active_buf->d3_panel_down(x1, y1, x2, y2);
		x1++;
		y1++;
	}
	else
	{
		vga.active_buf->d3_panel_up(x1, y1, x2, y2);
		x2--;
		y2--;
	}

	// put name
	font_bld.center_put(x1, y1, x2, y2, (char *)button->custom_para.ptr );

	if( !button->enable_flag )
	{
		vga.active_buf->bar_alpha(x1+3, y1+3, x2-3, y2-3, 1, 0);
	}
}
