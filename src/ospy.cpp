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

#include <opower.h>
#include <ogame.h>
#include <odate.h>
#include <onews.h>
#include <ofont.h>
#include <ounit.h>
#include <ou_wagon.h>
#include <oworld.h>
#include <obutton.h>
#include <ofirm.h>
#include <of_camp.h> 
#include <of_inn.h>
#include <of_train.h>
#include <otown.h>
#include <onation.h>
#include <oraceres.h>
#include <osys.h>
#include <ospy.h>
#include <otechres.h>
#include <otech.h>
#include <ogodres.h>
#include <oremote.h>
#include <ose.h>
#include <otalkres.h>
#include <ot_reps.h>


#define MIN_CAMOUFLAGE_POWER      20
#define MAX_CAMOUFLAGE_POWER(skill)		(skill/2+15)


//--------- Begin of function Spy::Spy ----------//
//
Spy::Spy()
{
	memset( this, 0, sizeof(Spy) );
}
//---------- End of function Spy::Spy ----------//


//--------- Begin of function Spy::deinit ----------//
//
void Spy::deinit()
{
	// ##### begin Gilbert 22/3 #######//
	if( sys.signal_exit_flag )
	{
		if( game.is_campaign_mode() )
		{
			// free name in campaign mode
			if( name_id )
			{
				race_res[race_id]->free_name_id(name_id);
				name_id = 0;
			}
		}

		return;
	}
	// ##### end Gilbert 22/3 #######//

	set_place(SPY_UNDEFINED, 0);		// reset spy place vars

	if( name_id )
	{
		race_res[race_id]->free_name_id(name_id);
		name_id = 0;
	}

	spy_recno = 0;
}
//---------- End of function Spy::deinit ----------//


//--------- Begin of function Spy::set_action_mode ----------//
//
void Spy::set_action_mode(int actionMode)
{
	action_mode = actionMode;
}
//---------- End of function Spy::set_action_mode ----------//


//--------- Begin of function Spy::set_place ----------//
//
// Meaning of spy_place_para:
//
// SPY_MOBILE - unit recno of the spy
// SPY_TOWN   - town recno
// SPY_FIRM   - firm recno
// SPY_SHIP	  - unit recno of the ship
//
void Spy::set_place(int spyPlace, int spyPlacePara)
{
	//----- reset spy counter of the current place ----//

	if( spy_place == SPY_FIRM )
	{
		if( !firm_array.is_deleted(spy_place_para) )
		{
			Firm *firmPtr = firm_array[spy_place_para];
			if( firmPtr->cast_to_FirmCamp() )
			{
				if( true_nation_recno == nation_array.player_recno )
					firmPtr->cast_to_FirmCamp()->player_spy_count--;
				err_when( firmPtr->cast_to_FirmCamp()->player_spy_count<0 );
			}
			else if( firmPtr->cast_to_FirmTrain() )
			{
				if( true_nation_recno == nation_array.player_recno )
					firmPtr->cast_to_FirmTrain()->player_spy_count--;
				err_when( firmPtr->cast_to_FirmTrain()->player_spy_count<0 );
			}
			else if( firmPtr->cast_to_FirmInn() )
			{
			}
			else
			{
				err_here();
			}
		}
	}

	else if( spy_place == SPY_TOWN )
	{
		if( !town_array.is_deleted(spy_place_para) )
		{
			town_array[spy_place_para]->spy_count--;
			err_when( town_array[spy_place_para]->spy_count<0 );

			if( town_array[spy_place_para]->spy_count<0 )		// self-fixing in case that this bug still presists. 
				town_array[spy_place_para]->spy_count=0;
		}
	}
	else if( spy_place == SPY_WAGON )
	{
		if( !unit_array.is_truly_deleted(spy_place_para) )
		{
			UnitWagon *wagonPtr = (UnitWagon *)unit_array[spy_place_para];
			if( !wagonPtr->del_spy(spy_recno) )
			{
				err_here();
			}
		}
	}

	//------- set the spy place now ---------//

	spy_place = spyPlace;
	spy_place_para = spyPlacePara;

	action_mode = SPY_IDLE;		// reset the spy mode

	//------- set the spy counter of the new place ------//
	// ------ remember to set is_civilian in every case ----------//

	// ####### begin Gilbert 24/2 #######//
	if( spy_place == SPY_MOBILE )
	{
		if( !unit_array.is_deleted(spy_place_para) )
		{
			unit_array[spy_place_para]->unique_id = unique_id;
			is_civilian = unit_array[spy_place_para]->is_civilian();
		}
	}
	else if( spy_place == SPY_FIRM )
	// ####### end Gilbert 20/4 #######//
	{
		if( !firm_array.is_deleted(spy_place_para) )
		{
			Firm *firmPtr = firm_array[spy_place_para];
			if( firmPtr->cast_to_FirmCamp() )
			{
				if( true_nation_recno == nation_array.player_recno )
					firmPtr->cast_to_FirmCamp()->player_spy_count++;

				is_civilian = 0;
			}
			else if( firmPtr->cast_to_FirmTrain() )
			{
				if( true_nation_recno == nation_array.player_recno )
					firmPtr->cast_to_FirmTrain()->player_spy_count++;

				if( firmPtr->cast_to_FirmSpecial() )
					is_civilian = 0;
				else
				{	// spy and other FirmTrain
					err_here();
				}
			}
			else if( firmPtr->cast_to_FirmInn() )
			{
				is_civilian = 0;
			}
			else
			{
				err_here();
			}
		}

		cloaked_nation_recno = (char) firm_array[spy_place_para]->nation_recno;

		if( firm_array[spy_place_para]->nation_recno != true_nation_recno )
			notify_cloaked_nation_flag = 1;	// when a spy has been assigned to a firm, its notification flag should be set to 1, so the nation can control it as it is one of its own units
	// ####### end Gilbert 20/4 #######//
	}
	else if( spy_place == SPY_TOWN )
	{
		town_array[spy_place_para]->spy_count++;
		is_civilian = 1;

		//-----------------------------------------------------------//
		// We need to update it here as this spy may have resigned from
		// a foreign firm and go back to its home village. And the
		// nation recno of the foreign firm and the home village are
		// different.
		//-----------------------------------------------------------//

		cloaked_nation_recno = (char) town_array[spy_place_para]->nation_recno;

		if( town_array[spy_place_para]->nation_recno != true_nation_recno )		// if it's our own town, don't change notify_cloaked_nation_flag
			notify_cloaked_nation_flag = 1;
	}
	else if( spy_place == SPY_WAGON )
	{
		if( !unit_array.is_truly_deleted(spy_place_para) )
		{
			UnitWagon *wagonPtr = (UnitWagon *)unit_array[spy_place_para];
			wagonPtr->add_spy(spy_recno);
		}

		is_civilian = 1;
	}
}
//---------- End of function Spy::set_place ----------//


//--------- Begin of function Spy::get_loc ----------//
//
// Return the location of the spy.
//
// <int&> xLoc, yLoc - vars for returning the locatin.
//
int Spy::get_loc(int& xLoc, int& yLoc)
{
	switch( spy_place )
	{
		case SPY_FIRM:
			if( !firm_array.is_deleted(spy_place_para) )
			{
				xLoc = firm_array[spy_place_para]->center_x;
				yLoc = firm_array[spy_place_para]->center_y;
				return 1;
			}
			break;

		case SPY_TOWN:
			if( !town_array.is_deleted(spy_place_para) )
			{
				xLoc = town_array[spy_place_para]->center_x;
				yLoc = town_array[spy_place_para]->center_y;
				return 1;
			}
			break;

		case SPY_MOBILE:
		case SPY_WAGON:
			if( !unit_array.is_truly_deleted(spy_place_para) )
			{
				if( unit_array[spy_place_para]->is_visible() )
				{
					xLoc = unit_array[spy_place_para]->next_x_loc();
					yLoc = unit_array[spy_place_para]->next_y_loc();
					return 1;
				}
				else
				{
					short shortXLoc;
					short shortYLoc;
					if( unit_array[spy_place_para]->get_cur_loc(shortXLoc, shortYLoc) )
					{
						xLoc = shortXLoc;
						yLoc = shortYLoc;
						return 1;
					}
				}
			}
			break;
	}

	return 0;
}
//---------- End of function Spy::get_loc ----------//


//--------- Begin of function Spy::get_loc ----------//
//
// Return the location of the spy.
//
// <int&> x1, y1, x2, y2 - vars for returning the locatin.
//
int Spy::get_loc(int& x1, int& y1, int& x2, int &y2)
{
	switch( spy_place )
	{
		case SPY_FIRM:
			if( !firm_array.is_deleted(spy_place_para) )
			{
				Firm *firmPtr = firm_array[spy_place_para];
				x1 = firmPtr->obj_loc_x1();
				y1 = firmPtr->obj_loc_y1();
				x2 = firmPtr->obj_loc_x2();
				y2 = firmPtr->obj_loc_y2();
				return 1;
			}
			break;

		case SPY_TOWN:
			if( !town_array.is_deleted(spy_place_para) )
			{
				Town *townPtr = town_array[spy_place_para];
				x1 = townPtr->obj_loc_x1();
				y1 = townPtr->obj_loc_y1();
				x2 = townPtr->obj_loc_x2();
				y2 = townPtr->obj_loc_y2();
				return 1;
			}
			break;

		case SPY_MOBILE:
		case SPY_WAGON:
			if( !unit_array.is_truly_deleted(spy_place_para) )
			{
				Unit *unitPtr = unit_array[spy_place_para];
				x1 = unitPtr->obj_loc_x1();
				y1 = unitPtr->obj_loc_y1();
				x2 = unitPtr->obj_loc_x2();
				y2 = unitPtr->obj_loc_y2();
				return 1;
			}
			break;
	}

	return 0;
}
//---------- End of function Spy::get_loc ----------//


//--------- Begin of function Spy::get_host_base_obj_recno ----------//
//
// return the base obj recno the spy is hiding
// 
int Spy::get_host_base_obj_recno()
{
	switch( spy_place )
	{
		case SPY_FIRM:
			if( !firm_array.is_deleted(spy_place_para) )
				return firm_array[spy_place_para]->base_obj_recno;
			break;

		case SPY_TOWN:
			if( !town_array.is_deleted(spy_place_para) )
				return town_array[spy_place_para]->base_obj_recno;
			break;

		case SPY_MOBILE:
		case SPY_WAGON:
			if( !unit_array.is_truly_deleted(spy_place_para) )
				return unit_array[spy_place_para]->base_obj_recno;
			break;
	}

	return 0;
}
//--------- End of function Spy::get_host_base_obj_recno ----------//


//------ Begin of function Spy::spy_place_nation_recno -------//
//
// Return the nation recno of the place where the spy stays.
//
int Spy::spy_place_nation_recno()
{
	if( spy_place == SPY_TOWN )
		return town_array[spy_place_para]->nation_recno;

	else if( spy_place == SPY_FIRM )
		return firm_array[spy_place_para]->nation_recno;

	else
		return 0;
}
//-------- End of function Spy::spy_place_nation_recno --------//


//--------- Begin of function Spy::next_day ----------//
//
void Spy::next_day()
{
	//------- pay expenses --------//

	pay_expense();

	//------ when the spy has been exposed -------//

	if( exposed_flag )
	{
		//-- he will be killed immediately unless he is back in his original nation color ---//

		if( true_nation_recno != cloaked_nation_recno )
		{
			get_killed();
			return;
		}
		else
			exposed_flag = 0;		// reset exposed_flag.
	}

	//------ process actions ---------//

	if( info.game_date%30 == spy_recno%30 )
	{
		if( spy_place == SPY_TOWN )
			process_town_action();

		else if( spy_place == SPY_FIRM )
			process_firm_action();
	}

	//------ increase skill --------//

	int rc;

	if( action_mode==SPY_IDLE )		// increase slower when in sleep mode
		rc = info.game_date%80 == spy_recno%80;
	else
		rc = info.game_date%40 == spy_recno%40;

	if( rc && spy_skill < 100 )
		spy_skill++;

	//----- update loyalty & think betray -------//

	if( info.game_date%60 == spy_recno%60 )
	{
		update_loyalty();

		if( think_betray() )
			return;
	}

	//----------- visit map (for fog of war) ----------//

	// ####### begin Gilbert 18/5 ##########//
	// see BaseObj::explore_for_player
//	if( true_nation_recno == nation_array.player_recno )
//	{
//		if( spy_place == SPY_TOWN )
//		{
//			Town* townPtr = town_array[spy_place_para];
//			world.visit( townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2, EXPLORE_RANGE-1 );
//		}
//		else if( spy_place == SPY_FIRM )
//		{
//			Firm* firmPtr = firm_array[spy_place_para];
//			world.visit( firmPtr->loc_x1, firmPtr->loc_y1, firmPtr->loc_x2, firmPtr->loc_y2, EXPLORE_RANGE-1 );
//		}
//	}
	// ####### end Gilbert 18/5 ##########//

	// --------- update camouflage variable -------//

	if( spy_place == SPY_MOBILE && camouflage_count > 0 )
	{
		if( true_nation_recno != cloaked_nation_recno )
			--camouflage_count;				// decrease count at outside
		else
			stop_camouflage(COMMAND_AUTO);			// stop when changed back to original nation
	}
	else
	{
		if( camouflage_power < MAX_CAMOUFLAGE_POWER(spy_skill) )
			++camouflage_power;
	}

	//---------- debug code -----------//

#ifdef DEBUG

	err_when( race_id<1 || race_id>MAX_RACE );

	switch( spy_place )
	{
	case SPY_MOBILE:
	{
		Unit* unitPtr = unit_array[spy_place_para];

		err_when( unitPtr->rank_id == RANK_KING );
		err_when( unitPtr->spy_recno != spy_recno );
//			err_when( unitPtr->skill.skill_id == SKILL_SPYING );
		break;
	}

	case SPY_TOWN:
		err_when( town_array[spy_place_para]->nation_recno != cloaked_nation_recno );
		break;

	case SPY_FIRM:
		err_when( firm_array.is_deleted(spy_place_para) );
		if( firm_array[spy_place_para]->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp();
			int i;
			for( i = 0; i < firmCamp->soldier_count; i++ )
			{
				if( firmCamp->soldier_array[i].spy_recno == spy_recno )
					break;
			}
			if( i==firmCamp->soldier_count )		// not found in worker_array
			{
				err_when( !firmCamp->overseer_recno ||
							 unit_array[firmCamp->overseer_recno]->spy_recno != spy_recno );
			}
		}
		// no more worker spy
//			else if( firm_array[spy_place_para]->cast_to_FirmWork() )
//			{
//				FirmWork* firmPtr = firm_array[spy_place_para]->cast_to_FirmWork();
//				for( int i=0 ; i<firmPtr->worker_count ; i++ )
//				{
//					if( firmPtr->worker_array[i].spy_recno==spy_recno )
//						break;
//				}
//				if( i==firmPtr->worker_count )		// not found in worker_array
//				{
//					err_when( !firmPtr->overseer_recno ||
//								 unit_array[firmPtr->overseer_recno]->spy_recno != spy_recno );
//				}
//			}
		else if( firm_array[spy_place_para]->cast_to_FirmTrain() )
		{
			FirmTrain *firmTrain = firm_array[spy_place_para]->cast_to_FirmTrain();

			int i;
			for( i = 0; i < firmTrain->trainee_count; ++i )
			{
				if( firmTrain->trainee_array[i].spy_recno == spy_recno )
					break;
			}
			err_when( i >= firmTrain->trainee_count );
		}
		else if( firm_array[spy_place_para]->cast_to_FirmInn() )
		{
			FirmInn *firmInn = firm_array[spy_place_para]->cast_to_FirmInn();
			int i;
			for( i = 0; i < firmInn->inn_unit_count; ++i )
			{
				if( firmInn->inn_unit_array[i].spy_recno == spy_recno )
					break;
			}
			err_when( i >= firmInn->inn_unit_count );
		}

		break;
	case SPY_WAGON:
		{
			err_when( unit_array.is_truly_deleted(spy_place_para) );
			UnitWagon *unitWagon = (UnitWagon *)unit_array[spy_place_para];

			int i;
			for( i = 0; i < unitWagon->spy_count; ++i )
			{
				if( unitWagon->spy_recno[i] == spy_recno )
					break;
			}
			err_when( i >= unitWagon->spy_count );
		}
		break;
	}
#endif
}
//---------- End of function Spy::next_day ----------//


//--------- Begin of function Spy::process_town_action ----------//
//
void Spy::process_town_action()
{
	Town* townPtr = town_array[spy_place_para];

	if( action_mode == SPY_SOW_DISSENT )
	{
		//if( townPtr->race_pop_array[race_id-1] > townPtr->race_spy_count_array[race_id-1] )	// only when there are non-spy people
		if( townPtr->population > townPtr->spy_count )	// only when there are non-spy people
		{
			err_when( race_id != townPtr->race_id );
			// ###### patch begin Gilbert 13/7 #######//
			// float decValue = (float)spy_skill / 5 / townPtr->population;		// the more people there, the longer it takes to decrease the loyalty
			float decValue = (float)spy_skill / 4 / townPtr->population;		// the more people there, the longer it takes to decrease the loyalty
			// ###### patch end Gilbert 13/7 #######//

			if( true_nation_recno )
				decValue *= 1.0f + (float)tech_res[TECH_AGITATION]->get_nation_tech_level(true_nation_recno);

			//----- if this is an independent town -----//

			if( townPtr->nation_recno==0 )
			{
				townPtr->resistance_array[true_nation_recno-1] -= decValue;

				if( townPtr->resistance_array[true_nation_recno-1] < 0 )
					townPtr->resistance_array[true_nation_recno-1] = (float) 0;
			}

			//--- if this is an enemy town, decrease the town people's loyalty ---//

			else
			{
				townPtr->loyalty -= decValue;

				if( townPtr->loyalty < (float) 0 )
					townPtr->loyalty = (float) 0;
			}
		}
	}
}
//---------- End of function Spy::process_town_action ----------//


//--------- Begin of function Spy::process_firm_action ----------//
//
void Spy::process_firm_action()
{
	// Firm* firmPtr = firm_array[spy_place_para];

	//---------- Sow Dissent ----------//

	if( action_mode == SPY_SOW_DISSENT )
	{
		FirmCamp *firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp();
		if( firmCamp )
		{
			int techLevel = true_nation_recno ? tech_res[TECH_AGITATION]->get_nation_tech_level(true_nation_recno) : 0;

			if( firmCamp && firmCamp->overseer_recno )
			{
				//---- decrease the loyalty of the overseer if there is any -----//

				Unit* unitPtr = unit_array[firmCamp->overseer_recno];
				if( unitPtr->race_id == race_id )
				{
					if( m.random( (100 - spy_skill + unitPtr->skill_level())/10) <= techLevel
						// a commander with a higher leadership skill will be less influenced by the spy's dissents
						 && unitPtr->loyalty>0 )
					{
						unitPtr->change_loyalty( -1 );
					}
				}
			}

			//----- decrease the loyalty of the workers in the firm -----//

			Soldier* soldierPtr = firmCamp->soldier_array;

			for( int i=0 ; i<firmCamp->soldier_count ; i++, soldierPtr++ )
			{
				if( soldierPtr->race_id != race_id )
					continue;

				//---- if the worker does not live in a town ----//
				if( !soldierPtr->spy_recno )		// the loyalty of the spy himself does not change
				{
					if( m.random(11-spy_skill/10) <= techLevel && soldierPtr->loyalty>0 )
						soldierPtr->loyalty--;
				}
			}
		}
	}
}
//---------- End of function Spy::process_firm_action ----------//


//--------- Begin of function Spy::set_next_action_mode ----------//
//
void Spy::set_next_action_mode()
{
	if( spy_place==SPY_TOWN )
	{
		if( action_mode == SPY_IDLE )
			set_action_mode(SPY_SOW_DISSENT);
		else
			set_action_mode(SPY_IDLE);
	}
	else if( spy_place==SPY_FIRM )
	{
		switch( action_mode )
		{
//			case SPY_IDLE:
//				if( can_sabotage() )
//				{
//					set_action_mode(SPY_SABOTAGE);
//					break;
//				}
//			case SPY_SABOTAGE:
//				set_action_mode(SPY_SOW_DISSENT);
//				break;
//			case SPY_SOW_DISSENT:
//				set_action_mode(SPY_IDLE);
//				break;
		case SPY_IDLE:
			set_action_mode(SPY_SOW_DISSENT);
			break;
		case SPY_SOW_DISSENT:
			set_action_mode(SPY_IDLE);
			break;
		default:
			err_here();
		}
	}
	// ####### begin Gilbert 3/7 #######//
	else
	{
		err_when( !remote.is_enable() );
	}
	// ####### end Gilbert 3/7 #######//
}
//---------- End of function Spy::set_next_action_mode ----------//


//--------- Begin of function Spy::can_sabotage ----------//
//
int Spy::can_sabotage()
{
//	return spy_place == SPY_FIRM &&
//			 firm_array[spy_place_para]->firm_id != FIRM_CAMP; 	// no sabotage actino for military camp
	return 0;
}
//---------- End of function Spy::can_sabotage ----------//


//--------- Begin of function Spy::get_killed ----------//
//
// [int] dispNews - whether display a news message or not
//						  (default: 1)
//
void Spy::get_killed(int dispNews)
{
	//-------- add news --------//

	if( true_nation_recno == nation_array.player_recno ||		// the player's spy is killed
		 cloaked_nation_recno == nation_array.player_recno )				// a spy cloaked as the player's people is killed in the player's firm or firm
	{
		// ####### begin Gilbert 17/6 ########// 
		if( dispNews )
			news_array.spy_killed(spy_recno);
		// ####### end Gilbert 17/6 ########// 
		// ####### begin Gilbert 10/10 #######//
		se_ctrl.immediate_sound("SPY_DIE");
		// ####### end Gilbert 10/10 #######//
	}
	// ####### end Gilbert 17/6 ########// 

	//--- If a spy is caught, the spy's nation's reputation wil decrease ---//

	nation_array[true_nation_recno]->change_reputation((float)-SPY_KILLED_REPUTATION_DECREASE);

	//------- if the spy is in a town -------//

	int hostNationRecno=0;
	int mobileUnit = 0;

	if( spy_place==SPY_TOWN )
	{
		Town* townPtr = town_array[spy_place_para];

		hostNationRecno = townPtr->nation_recno;

		townPtr->dec_pop();
	}

	//------- if the spy is in a firm -------//

	else if( spy_place==SPY_FIRM )
	{
		Firm* firmPtr = firm_array[spy_place_para];
		hostNationRecno = firmPtr->nation_recno;

		FirmCamp *firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp();
		if( firmCamp )
		{
			//------- check if the overseer is the spy -------//

			if( firmCamp->overseer_recno )
			{
				Unit* unitPtr = unit_array[firmCamp->overseer_recno];

				if( unitPtr->spy_recno == spy_recno )
				{
					firmCamp->kill_overseer();
					return;
				}
			}

			//---- check if any of the soldier is the spy ----//

			for( int i=0 ; i<firmCamp->soldier_count ; i++ )
			{
				if( firmCamp->soldier_array[i].spy_recno == spy_recno )
				{
					firmCamp->kill_soldier(i+1);
					return;
				}
			}
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();
			for( int i = 0; i < firmTrain->trainee_count; ++i )
			{
				if( firmTrain->trainee_array[i].spy_recno == spy_recno )
				{
					firmTrain->kill_trainee(i+1);
					return;
				}
			}
		}
		else if( firmPtr->cast_to_FirmInn() )
		{
			FirmInn *firmInn = firmPtr->cast_to_FirmInn();
			for( int i = 0; i < firmInn->inn_unit_count; ++i )
			{
				if( firmInn->inn_unit_array[i].spy_recno == spy_recno )
				{
					firmInn->kill_unit(i+1);
					return;
				}
			}
		}
		else
			err_here();		// the spy is not found here
	}
	else if( spy_place == SPY_MOBILE )
	{
		//### begin alex 31/3 ###//
		//err_here();		// only spies in towns and firms will get killed instantly
		unit_array.del(spy_place_para);
		mobileUnit = 1;
		//#### end alex 31/3 ####//
	}
	else if( spy_place ==  SPY_WAGON )
	{
		err_when( unit_array.is_truly_deleted(spy_place_para) );
		err_when( unit_array[spy_place_para]->unit_id != UNIT_WAGON );
		int rc = ((UnitWagon *)unit_array[spy_place_para])->del_spy(spy_recno);
		err_when( !rc );
		if( rc )
			spy_array.del_spy(rc);
		return;
	}
	else
	{
		err_here();
	}

	//--- If the spy is in an AI town or firm, the AI's relationship towards the spy's owner nation will decrease ---//

	if( hostNationRecno && nation_array[hostNationRecno]->is_ai() )
	{
		nation_array[hostNationRecno]->change_ai_relation_level(true_nation_recno, -5);
	}

	//---- delete the spy from spy_array ----//
	//### begin alex 31/3 ###//
	if(!mobileUnit)
		spy_array.del_spy(spy_recno);
	//else spy_array.del_spy() is called in unit_array.del()
	//#### end alex 31/3 ####//
}
//---------- End of function Spy::get_killed ----------//


//--------- Begin of function Spy::action_str ----------//
//
const char* Spy::action_str()
{
	if( action_mode == SPY_IDLE && cloaked_nation_recno == true_nation_recno )
	{
		if( (spy_place==SPY_TOWN &&
			  town_array[spy_place_para]->nation_recno == true_nation_recno) ||
			 (spy_place==SPY_FIRM &&
			  firm_array[spy_place_para]->nation_recno == true_nation_recno) )
		{
			return text_reports.str_counter_spy_action_mode();
		}
	}

	return text_reports.str_spy_action_mode( action_mode );

/*	switch( action_mode )
	{
		case SPY_IDLE:
		{
			//---- if the spy is in a firm or town of its own nation ---//

			if( (spy_place==SPY_TOWN &&
				  town_array[spy_place_para]->nation_recno == true_nation_recno) ||
				 (spy_place==SPY_FIRM &&
				  firm_array[spy_place_para]->nation_recno == true_nation_recno) )
			{
				return "Counter-Spy";
			}
			else
				return "Sleep";
		}

		case SPY_SOW_DISSENT:
			return "Agitate";

//		case SPY_SABOTAGE:
//			return "Sabotage";

		default:
			err_here();
	}

	return "";
*/
}
//---------- End of function Spy::action_str ----------//


//--------- Begin of function Spy::pay_expense ---------//
//
// -Each spy costs $5 dollars to maintain per month.
//
// -If your spies are mobile:
//  >your nation pays them 1 food and $5 dollars per month
//
// -If your spies are in an enemy's town or firm:
//  >the enemy pays them 1 food and the normal salary of their jobs.
//  >your nation pays them $5 dollars per month. (your nation pays them no food)
//
// -If your spies are in your own town or firm:
//  >your nation pays them 1 food and $5 dollars per month
//
void Spy::pay_expense()
{
	Nation* nationPtr = nation_array[true_nation_recno];

	//---------- reduce cash -----------//

	if( nationPtr->cash > 0 )
	{
		nationPtr->add_expense( EXPENSE_SPY, (float) SPY_YEAR_SALARY / 365, 1 );
	}
	else     // decrease loyalty if the nation cannot pay the unit
	{
		change_loyalty(-1);
	}

	//---------- reduce food -----------//

	int inOwnFirm=0;

	if( spy_place == SPY_FIRM )
	{
		Firm* firmPtr = firm_array[spy_place_para];

		if( firmPtr->nation_recno == true_nation_recno )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
			if( firmCamp && firmCamp->overseer_recno &&
				unit_array[firmCamp->overseer_recno]->spy_recno == spy_recno )
			{
				inOwnFirm = 1;
			}
		}
	}

	if( spy_place == SPY_MOBILE || inOwnFirm )
	{
		if( nationPtr->food > 0 )
		{
			nationPtr->consume_food((float) UNIT_FOOD_YEAR_CONSUMPTION / 365);
		}
		else
		{
			if( info.game_date%NO_FOOD_LOYALTY_DECREASE_INTERVAL == 0 )		// decrease 1 loyalty point every 2 days
				change_loyalty(-1);
		}
	}
}
//----------- End of function Spy::pay_expense -----------//


//--------- Begin of function Spy::update_loyalty ----------//
//
void Spy::update_loyalty()
{
	Nation* ownNation = nation_array[true_nation_recno];

	int targetLoyalty = 50 + (int)ownNation->reputation/4 +
							  ownNation->overall_rank_rating()/4;

	if( race_id == ownNation->race_id )
		targetLoyalty += 20;

	targetLoyalty = min( targetLoyalty, 100 );

	if( spy_loyalty > targetLoyalty )		
		spy_loyalty--;

	else if( spy_loyalty < targetLoyalty )
		spy_loyalty++;
}
//---------- End of function Spy::update_loyalty ----------//


//--------- Begin of function Spy::change_loyalty ----------//
//
void Spy::change_loyalty(int changeAmt)
{
	int newLoyalty = spy_loyalty + changeAmt;

	newLoyalty = max(0, newLoyalty);

	spy_loyalty = min(100, newLoyalty);
}
//---------- End of function Spy::change_loyalty ----------//


//--------- Begin of function Spy::think_betray ----------//
//
// Think about turning towards to the current cloaked nation,
// become a normal unit and lose its spy identity.
//
int Spy::think_betray()
{
	if( spy_loyalty >= UNIT_BETRAY_LOYALTY )		// you when unit is
		return 0;

	if( cloaked_nation_recno == true_nation_recno || cloaked_nation_recno==0 )
		return 0;

	//--- think whether the spy should turn towards the nation ---//

	Nation* nationPtr = nation_array[cloaked_nation_recno];

	int nationScore = (int) nationPtr->reputation;	// reputation can be negative

	if( race_res.is_same_race(nationPtr->race_id, race_id) )
		nationScore += 30;

	if( spy_loyalty < nationScore || spy_loyalty==0 )
	{
		drop_spy_identity();
		return 1;
	}

	return 0;
}
//---------- End of function Spy::think_betray ----------//


//--------- Begin of function Spy::drop_spy_identity ----------//
//
// Drop its spy identity. If it is currently cloaked to another
// nation, it will become units of that nation.
//
void Spy::drop_spy_identity()
{
	if( spy_place == SPY_FIRM )
	{
		Firm* firmPtr = firm_array[spy_place_para];
		int   rc = 0;

		if( firmPtr->cast_to_FirmInn() )
			return;			// cannot drop spy identity while at inn

		else if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
			if( firmCamp->overseer_recno )
			{
				Unit* unitPtr = unit_array[firmCamp->overseer_recno];
				if( unitPtr->spy_recno == spy_recno )
				{
					unitPtr->spy_recno = 0;
					rc = 1;
				}
			}
			if( !rc )
			{
				for( int i=0 ; i<firmCamp->soldier_count ; i++ )
				{
					if( firmCamp->soldier_array[i].spy_recno==spy_recno )
					{
						firmCamp->soldier_array[i].spy_recno = 0;
						rc = 1;
						break;
					}
				}

				err_when( !rc );
			}
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();
			for( int i = 0; i < firmTrain->trainee_count; i++ )
			{
				if( firmTrain->trainee_array[i].spy_recno == spy_recno )
				{
					firmTrain->trainee_array[i].spy_recno = 0;
					rc = 1;
					break;
				}
			}
		}
		else
		{
			err_here();
		}
	}
	else if( spy_place == SPY_MOBILE )
	{
		Unit* unitPtr = unit_array[spy_place_para];

		unitPtr->spy_recno = 0;
	}
	else if( spy_place == SPY_TOWN )
	{
		// no action
	}
	else if( spy_place == SPY_WAGON )
	{
		// no action
	}

	//------ delete this Spy record from spy_array ----//

	spy_array.del_spy(spy_recno);		// Spy::deinit() will take care of the rest of the initialization for the spy
}
//---------- End of function Spy::drop_spy_identity ----------//


//--------- Begin of function Spy::change_true_nation ----------//
//
// Change the spy's true nation recno.
//
void Spy::change_true_nation(int newNationRecno)
{
	err_when( nation_array.is_deleted(newNationRecno) );

	true_nation_recno = newNationRecno;

	//--- update Firm::player_spy_count if the spy is in a firm ---//

	if( spy_place == SPY_FIRM )
	{
		spy_array.update_firm_spy_count(spy_place_para);
	}
}
//---------- End of function Spy::change_true_nation ----------//


//--------- Begin of function Spy::change_cloaked_nation ----------//
//
// Change a spy's cloaked nation. This nation is when the caller
// only have spy_recno as the reference. While Unit::spy_change_nation()
// is called when the caller has unit_recno as the reference. 
//
// <int>  newNationRecno - the new nation the spy changes its cloack to 
//
void Spy::change_cloaked_nation(int newNationRecno)
{
	if( newNationRecno == cloaked_nation_recno )
		return;

	//--- only mobile units and overseers can change nation, spies in firms or towns cannot change nation, their nation recno must be the same as the town or the firm's nation recno

	int rc = 0;

	if( spy_place == SPY_MOBILE )
	{
		unit_array[spy_place_para]->spy_change_nation(newNationRecno, COMMAND_AUTO);
		rc = 1;
	}
	else if( spy_place == SPY_FIRM )
	{
		FirmCamp *firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp();

		if( firmCamp && firmCamp->overseer_recno &&
			 unit_array[firmCamp->overseer_recno]->spy_recno == spy_recno )
		{
			unit_array[firmCamp->overseer_recno]->spy_change_nation(newNationRecno, COMMAND_AUTO);
			rc = 1;
		}
	}
	// ###### begin Gilbert  4/7 #########//
	else if( spy_place == SPY_WAGON )
	{
		UnitWagon *unitWagon = unit_array[spy_place_para]->cast_to_UnitWagon();

		// mobilize the spy

		int mobiledUnitRecno = unitWagon->mobilize_civilian( spy_recno );
		if( mobiledUnitRecno )
		{
			err_when( spy_place_para != mobiledUnitRecno );

			unit_array[spy_place_para]->spy_change_nation(newNationRecno, COMMAND_AUTO);
			rc = 1;
		}
	}
	// ###### end Gilbert  4/7 #########//

	if( rc )
	{
		if( newNationRecno == true_nation_recno )
		{
			stop_camouflage(COMMAND_AUTO);
		}
	}
	else
	{
		err_here();		// cannot change a spy's cloaked_nation_recno in a firm and town as the spy's cloaked nation recno must be the same as the firm and town's nation recno
	}
}
//---------- End of function Spy::change_cloaked_nation ----------//


//--------- Begin of function Spy::can_change_cloaked_nation ----------//
//
// Return whether this spy can change its cloaked to the specific
// nation right now.
//
// <int>  newNationRecno - the new nation the spy changes its cloak to
//
int Spy::can_change_cloaked_nation(int newNationRecno)
{
	//---- can always change back to its original nation ----//

	if( newNationRecno == true_nation_recno )
		return 1;

	//--- only mobile units and overseers can change nation, spies in firms or towns cannot change nation, their nation recno must be the same as the town or the firm's nation recno

	if( spy_place == SPY_MOBILE )
	{
		return unit_array[spy_place_para]->can_spy_change_nation();
	}
	else		// can't change in firms or towns.
	{
		return 0;
	}
}
//---------- End of function Spy::can_change_cloaked_nation ----------//


//--------- Begin of function Spy::capture_firm ----------//
//
// Order the spy to capture the firm he currently stays.
//
int Spy::capture_firm()
{
	if( spy_place != SPY_FIRM )
		return 0;

	Firm* firmPtr = firm_array[spy_place_para];

	//------- if the spy is the overseer of the firm --------//

	if( firm_res[firmPtr->firm_id]->need_overseer )
	{
		//-----------------------------------------------------//
		//
		// If the firm needs an overseer, the firm can only be
		// captured if the spy is the overseer of the firm.
		//
		//-----------------------------------------------------//

		FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
		
		if( firmCamp )
		{
			if( !firmCamp->overseer_recno ||
				 unit_array[firmCamp->overseer_recno]->spy_recno != spy_recno )
			{
				return 0;
			}

			//---------------------------------------------------//
			//
			// For those soldiers who disagree with the spy general will
			// leave the command base and attack it. Soldiers who are not
			// racially homogenous to the spy general tend to disagree. Also
			// if the spy has a higher leadership, there will be a higher
			// chance for the soldiers to follow the general.
			//
			//---------------------------------------------------//

			Unit* unitPtr = unit_array[firmCamp->overseer_recno];

			Soldier* soldierPtr = firmCamp->soldier_array;
			int	  unitLeadership = unitPtr->skill_level();
			int	  nationReputation = (int) nation_array[true_nation_recno]->reputation;
			int	  obeyChance, obeyFlag;

			for( int i=0 ; i<firmCamp->soldier_count ; i++, soldierPtr++ )
			{
				if( soldierPtr->is_under_training() )
					continue;

				//-- if this soldier is a spy, it will stay with you --//

				if( soldierPtr->spy_recno )
					continue;

				//---- if this is a normal soldier -----//

				obeyChance = unitLeadership/2 + nationReputation/2;

				if( race_res.is_same_race(soldierPtr->race_id, race_id) )
					obeyChance += 50;

				obeyFlag = m.random(100) < obeyChance; 		// if obeyChance >= 100, all units will object the overseer

				//--- if the soldier obey, update its loyalty ---//

				if( obeyFlag )
					soldierPtr->loyalty = max(UNIT_BETRAY_LOYALTY, obeyChance/2);

				//--- if the soldier does not obey, it is mobilized and attack the base ---//

				else
					firmCamp->mobilize_soldier(i+1, COMMAND_AUTO);
			}

			//--------- add news message --------//

			if( firmPtr->nation_recno == nation_array.player_recno )
				news_array.firm_captured(spy_place_para, true_nation_recno, 1);		// 1 - the capturer is a spy

			//-------- if this is an AI firm --------//

			if( firmPtr->is_ai )
				firmPtr->ai_firm_captured(true_nation_recno);

			//----- the spy change nation and capture the firm -------//

			unitPtr->spy_change_nation(true_nation_recno, COMMAND_AUTO);
		}

	}
	else
	{
		/*
		//------ otherwise the spy is a worker of the firm -------//

		//---- check whether it's true that the only units in the firms are our spies ---//

		Worker* workerPtr = firmPtr->worker_array;

		for( int i=0 ; i<firmPtr->worker_count ; i++, workerPtr++ )
		{
			if( !workerPtr->spy_recno )		// this worker is not a spy
				return 0;

			if( spy_array[workerPtr->spy_recno]->true_nation_recno != true_nation_recno )
				return 0;							// this worker is a spy, but not belong to the same nation
		}
		*/

		//--------- add news message --------//

		if( firmPtr->nation_recno == nation_array.player_recno )
			news_array.firm_captured(spy_place_para, true_nation_recno, 1);		// 1 - the capturer is a spy

		//-------- if this is an AI firm --------//

		if( firmPtr->is_ai )
			firmPtr->ai_firm_captured(true_nation_recno);

		//----- change the firm's nation recno -----//

		firmPtr->change_nation(true_nation_recno);		// the firm change nation and the spies inside the firm will have their cloaked nation recno changed
	}

	return 1;
}
//---------- End of function Spy::capture_firm ----------//


//-------- Begin of function Spy::mobilize_spy ------//
//
// return: recno of the mobilized unit.
//
int Spy::mobilize_spy()
{
	switch( spy_place )
	{
		case SPY_TOWN:
			return mobilize_town_spy();

		case SPY_FIRM:
			return mobilize_firm_spy();

		case SPY_MOBILE:
			err_when( !unit_array[spy_place_para]->is_visible() );
			return spy_place_para;

		default:
			return 0;
	}
}
//---------- End of function Spy::mobilize_spy --------//


//-------- Begin of function Spy::mobilize_town_spy ------//
//
// [int] decPop - whether need to decrease the population of town when the spy is mobilized.
//					   should set to no when it's calling from training a unit. (default: 1)
//
// return: recno of the mobilized unit.
//
int Spy::mobilize_town_spy(int decPop)
{
	err_when( spy_place != SPY_TOWN );

	if( spy_place != SPY_TOWN )
		return 0;

	Town* townPtr = town_array[spy_place_para];

	int unitRecno = townPtr->mobilize_town_people(decPop, 1);		//1-mobilize spies

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];  // set the spy vars of the mobilized unit

	err_when( !unitPtr->is_visible() );

	unitPtr->spy_recno = spy_recno;
	unitPtr->set_name(name_id);		// set the name id. of this unit
	// ###### begin Gilbert 24/2 ######//
	unitPtr->unique_id = unique_id;		// set_place also set unique_id
	// ###### end Gilbert 24/2 ######//

	set_place(SPY_MOBILE, unitRecno);

	return unitRecno;
}
//---------- End of function Spy::mobilize_town_spy --------//


//-------- Begin of function Spy::mobilize_firm_spy ------//
//
// return: <int> unit recno of the spy mobilized.
//
int Spy::mobilize_firm_spy()
{
	err_when( spy_place != SPY_FIRM );

	if( spy_place != SPY_FIRM )
		return 0;

	Firm* firmPtr = firm_array[spy_place_para];
	int 	spyUnitRecno=0;

	if( firmPtr->cast_to_FirmCamp() )
	{
		FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

		//---- check if the spy is the overseer of the firm -----//

		if( firmCamp->overseer_recno )
		{
			Unit* unitPtr = unit_array[firmCamp->overseer_recno];

			if( unitPtr->spy_recno == spy_recno )
				spyUnitRecno = firmCamp->mobilize_overseer();
		}

		//---- check if the spy is one of the workers of the firm ----//

		if( !spyUnitRecno )
		{
			int i;
			for( i = firmCamp->soldier_count-1; i >= 0; --i )
			{
				if( firmCamp->soldier_array[i].spy_recno == spy_recno )
				{
					spyUnitRecno = firmCamp->mobilize_soldier(i+1, COMMAND_AUTO);		// note: mobilize_woker() will decrease Firm::player_spy_count
					break;
				}
			}

			err_when( i < 0 );		// not found
		}
	}
	else if( firmPtr->cast_to_FirmTrain() )
	{
		FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();

		int i;
		for( i = firmTrain->trainee_count-1; i >= 0; --i )
		{
			if( firmTrain->trainee_array[i].spy_recno == spy_recno )
			{
				// ##### begin Gilbert 27/1 ######//
				// err_when( firmTrain->trainee_array[i].is_under_training );
				// ##### end Gilbert 27/1 ######//
				spyUnitRecno = firmTrain->mobilize_trainee(i+1, COMMAND_AUTO);		// note: mobilize_woker() will decrease Firm::player_spy_count
				break;
			}
		}

		err_when( i < 0 );
	}
	else if( firmPtr->cast_to_FirmInn() )
	{
		FirmInn *firmInn = firmPtr->cast_to_FirmInn();

		int i;
		for( i = firmInn->inn_unit_count-1; i >= 0; --i )
		{
			if( firmInn->inn_unit_array[i].spy_recno == spy_recno )
			{
				spyUnitRecno = firmInn->hire(i+1, 1 );
				break;
			}
		}

		err_when( i < 0 );
	}

	return spyUnitRecno;
}
//---------- End of function Spy::mobilize_firm_spy --------//


//-------- Begin of function Spy::think_become_king ------//
//
// If a nation has picked your spy to succeed to the died king,
// you spy will either:
//
// >he captures the entire nation for you.
// >he betrays you and rule his new empire.
//
void Spy::think_become_king()
{
	err_when( spy_place != SPY_MOBILE );		// it must be mobile

	int hisNationPower = nation_array[cloaked_nation_recno]->overall_rating;
	int parentNationPower = nation_array[true_nation_recno]->overall_rating;

	//--- if his nation is more power than the player's nation, the chance of handing his nation over to his parent nation will be low unless the loyalty is very high ---//

	int acceptLoyalty = 90 + hisNationPower - parentNationPower;

	if( spy_loyalty >= acceptLoyalty &&
		 nation_array[cloaked_nation_recno]->is_ai() )	// never will a spy take over the player's nation
	{
		//------ hand his nation over to his parent nation ------//

		nation_array[cloaked_nation_recno]->surrender(true_nation_recno);
	}
	else //--- betray his parent nation and rule the nation himself ---//
	{
		drop_spy_identity();
	}
}
//---------- End of function Spy::think_become_king --------//


//-------- Begin of function Spy::cloaked_rank_id ------//
//
// Return the cloaked rank id. of this spy.
//
int Spy::cloaked_rank_id()
{
	switch( spy_place )
	{
		case SPY_TOWN:
			return RANK_SOLDIER;

		case SPY_FIRM:
		{
			Firm* firmPtr = firm_array[spy_place_para];

			if( firmPtr->cast_to_FirmCamp() )
			{
				FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
				if( firmCamp->overseer_recno &&
					 unit_array[firmCamp->overseer_recno]->spy_recno == spy_recno )
				{
					return RANK_GENERAL;
				}
			}
			return RANK_SOLDIER;
		}

		case SPY_MOBILE:
			return unit_array[spy_place_para]->rank_id;

		default:
			return RANK_SOLDIER;
	}
}
//---------- End of function Spy::cloaked_rank_id --------//


//-------- Begin of function Spy::cloaked_combat_level ------//
//
// Return the cloaked combat level. of this spy.
//
int Spy::cloaked_combat_level()
{
	switch( spy_place )
	{
		case SPY_TOWN:
			return CITIZEN_COMBAT_LEVEL;

		case SPY_FIRM:
			{
				Firm* firmPtr = firm_array[spy_place_para];

				//------- if it is a camp ---------//

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

					if( firmCamp->overseer_recno &&
						 unit_array[firmCamp->overseer_recno]->spy_recno == spy_recno )
					{
						return unit_array[firmCamp->overseer_recno]->combat_level();
					}

					for(int j = 0; j < firmCamp->soldier_count; ++j)
					{
						if( firmCamp->soldier_array[j].spy_recno == spy_recno )
						{
							return firmCamp->soldier_array[j].combat_level();
						}
					}
				}

				//--------- if it is an inn ---------//

				else if( firmPtr->cast_to_FirmInn() )
				{
					FirmInn *firmInn = firmPtr->cast_to_FirmInn();
					for( int j = 0; j < firmInn->inn_unit_count; ++j )
					{
						if( firmInn->inn_unit_array[j].spy_recno == spy_recno )
						{
							return firmInn->inn_unit_array[j].combat_level();
						}
					}
				}

				//-------- if it is a training structure ------//

				else if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();
					for( int j = 0; j < firmTrain->trainee_count; ++j )
					{
						if( firmTrain->trainee_array[j].spy_recno == spy_recno )
						{
							// do not show cloaked skill in FirmTrain
							return firmTrain->trainee_array[j].combat_level();
						}
					}
				}
			}

			return CITIZEN_COMBAT_LEVEL;

		case SPY_MOBILE:
			return unit_array[spy_place_para]->combat_level();

		default:
			return CITIZEN_COMBAT_LEVEL;
	}
}
//---------- End of function Spy::cloaked_combat_level --------//


//-------- Begin of function Spy::cloaked_skill_level ------//
//
// Return the cloaked skill level. of this spy.
//
int Spy::cloaked_skill_level()
{
	switch( spy_place )
	{
		case SPY_TOWN:
			return 0;

		case SPY_FIRM:
			{
				Firm* firmPtr = firm_array[spy_place_para];

				//------- if it is a camp ---------//

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

					if( firmCamp->overseer_recno &&
						 unit_array[firmCamp->overseer_recno]->spy_recno == spy_recno )
					{
						return unit_array[firmCamp->overseer_recno]->skill_level();
					}

					for(int j = 0; j < firmCamp->soldier_count; ++j)
					{
						if( firmCamp->soldier_array[j].spy_recno == spy_recno )
						{
							return firmCamp->soldier_array[j].skill_level();
						}
					}
				}

				//------- if it is an inn ---------//

				else if( firmPtr->cast_to_FirmInn() )
				{
					FirmInn *firmInn = firmPtr->cast_to_FirmInn();

					for( int j = 0; j < firmInn->inn_unit_count; ++j )
					{
						if( firmInn->inn_unit_array[j].spy_recno == spy_recno )
						{
							return firmInn->inn_unit_array[j].skill_level();
						}
					}
				}

				//-------- if it is a training structure ------//

				else if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();

					for( int j = 0; j < firmTrain->trainee_count; ++j )
					{
						if( firmTrain->trainee_array[j].spy_recno == spy_recno )
						{
							return firmTrain->trainee_array[j].skill_level();
						}
					}
				}
			}
			return 0;

		case SPY_MOBILE:
			return unit_array[spy_place_para]->skill_level();

		default:
			return 0;
	}
}
//---------- End of function Spy::cloaked_skill_level --------//


//-------- Begin of function Spy::reward ------//
//
// The owner nation of the spy rewards him.
//
void Spy::reward(int remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure <spy recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_REWARD, sizeof(short));
		shortPtr[0] = spy_recno;
		return;
	}

	change_loyalty(REWARD_LOYALTY_INCREASE);

	nation_array[true_nation_recno]->add_expense(EXPENSE_REWARD_UNIT, (float)REWARD_COST);
}
//---------- End of function Spy::reward --------//


//-------- Begin of function Spy::set_exposed ------//
//
// Enable the exposed_flag of the spy. The spy will get killed
// should when he has been exposed.
//
void Spy::set_exposed(int remoteAction)
{
	// ##### begin Gilbert 26/9 #######//
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure <spy recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_EXPOSED, sizeof(short));
		shortPtr[0] = spy_recno;
		return;
	}
	// ##### end Gilbert 26/9 #######//
	exposed_flag = 1;
}
//---------- End of function Spy::set_exposed --------//


//-------- Begin of function Spy::assassinate ------//
//
// <int> targetUnitRecno - the recno of the unit to assassinate.
// <int> remoteAction 	 - remote action type.
//
// If this is a player's spy, the result of the assassination
// will be set to Firm::assassinate_result.
//
int Spy::assassinate(int targetUnitRecno, int remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure <spy recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_ASSASSINATE, sizeof(short)*2);
		shortPtr[0] = spy_recno;
		shortPtr[1] = targetUnitRecno;
		return 0;
	}

	//---------- validate first -----------//

	if( spy_place != SPY_FIRM )
		return 0;

	Unit* targetUnit = unit_array[targetUnitRecno];

	if( targetUnit->unit_mode != UNIT_MODE_OVERSEE )
		return 0;

	Firm* firmPtr = firm_array[ targetUnit->unit_mode_para ];
	if( firmPtr->firm_recno != spy_place_para )
		return 0;

	FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
	// any other firm that assassination can take place
	if( !firmCamp )		// && other firms...
		return 0;

	//---- get the attack and defense rating ----//

	int attackRating, defenseRating, otherDefenderCount;

	if( !get_assassinate_rating(targetUnitRecno, attackRating, defenseRating, otherDefenderCount) )
		return 0;

	//-------------------------------------------//

	int rc;
	int trueNationRecno = true_nation_recno;		// need to save it first as the spy may be killed later

	// ##### begin Gilbert 12/12 ######//
	// if( attackRating >= defenseRating )
	if( m.random(100) < attackRating - defenseRating )
	// ##### end Gilbert 12/12 ######//
	{
		//--- whether the spy will be get caught and killed in the mission ---//

		// ###### begin Gilbert 12/12 ##### //
		// int spyKillFlag = otherDefenderCount > 0 && attackRating - defenseRating < 80;
		int spyKillFlag = otherDefenderCount > 0 && !(m.random(100) < attackRating - defenseRating - 20);
		// assassin save chance is 20% lower than assassination
		// ###### end Gilbert 12/12 ##### //

		//--- if the unit assassinated is the player's unit ---//

		if( targetUnit->nation_recno == nation_array.player_recno )
			news_array.unit_assassinated(targetUnit->sprite_recno, spyKillFlag);

		if( firmCamp )
			firmCamp->kill_overseer();
		else
			err_here();

		//-----------------------------------------------------//
		// If there are other defenders in the firm and
		// the difference between the attack rating and defense rating
		// is small, then then spy will be caught and executed.
		//-----------------------------------------------------//

		if( spyKillFlag )
		{
			get_killed(0);		// 0 - don't display new message for the spy being killed

			rc = ASSASSINATE_SUCCEED_KILLED;
		}
		else
		{
			rc = ASSASSINATE_SUCCEED_AT_LARGE;
		}
	}
	else //----- if the assassination fails --------//
	{
		//-- if the spy is attempting to assassinate the player's general or king --//

		// ####### begin Gilbert 17/6 #########//
		if( targetUnit->nation_recno == nation_array.player_recno )
			news_array.assassinator_caught(spy_recno, targetUnit->rank_id, targetUnit->race_id );
		// ####### end Gilbert 17/6 #########//

		get_killed(0);		// 0 - don't display new message for the spy being killed

		rc = ASSASSINATE_FAIL;
	}

	//--- if this firm is the selected firm and the spy is the player's spy ---//

	if( trueNationRecno == nation_array.player_recno &&
		 firmPtr->firm_recno == firm_array.selected_recno )
	{
		if( firmCamp )
		{
			firmCamp->assassinate_result = rc;
			firmCamp->firm_menu_mode = FIRM_MENU_ASSASSINATE_RESULT;
		}
		// info.disp();
	}

	return rc;
}
//---------- End of function Spy::assassinate --------//


//-------- Begin of function Spy::get_assassinate_rating ------//
//
// if return true, chance of assassination is (attackRating - defenseRating)/100
//
int Spy::get_assassinate_rating(int targetUnitRecno, int& attackRating, int& defenseRating, int& otherDefenderCount)
{
	//---------- validate first -----------//

	if( spy_place != SPY_FIRM )
		return 0;

	Unit* targetUnit = unit_array[targetUnitRecno];

	if( targetUnit->unit_mode != UNIT_MODE_OVERSEE )
		return 0;

	Firm* firmPtr = firm_array[ targetUnit->unit_mode_para ];

	if( firmPtr->firm_recno != spy_place_para )
		return 0;

	//------ get the hit points of the spy ----//

	int spyHitPoints = 0;

	if( firmPtr->cast_to_FirmCamp() )
	{
		FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

		int i;
		for( i=0 ; i<firmCamp->soldier_count ; i++ )
		{
			if( firmCamp->soldier_array[i].spy_recno == spy_recno )
			{
				spyHitPoints = firmCamp->soldier_array[i].hit_points;
				break;
			}
		}

		err_when( i==firmCamp->soldier_count );
	}

	//------ calculate success chance ------//

	int assassinTechLevel = 0;
	if( true_nation_recno )
		assassinTechLevel = tech_res[TECH_ASSASSIN]->get_nation_tech_level(true_nation_recno);
	// ##### patch begin Gilbert 16/2 #####//
	if( god_res[GOD_PERSIAN]->nation_prayer_count(true_nation_recno) > 0 )
		assassinTechLevel++;
	// ##### patch end Gilbert 16/2 #####//

	int counterSpyLevel = 0;
	if( targetUnit->nation_recno )
		counterSpyLevel = tech_res[TECH_COUNTER_SPY]->get_nation_tech_level(targetUnit->nation_recno);

	attackRating  = spy_skill + spyHitPoints/2 + 50 * assassinTechLevel;

	defenseRating = (int) targetUnit->hit_points/2;
	otherDefenderCount=0;

	if( targetUnit->spy_recno )
		defenseRating += spy_array[targetUnit->spy_recno]->spy_skill;

	if( targetUnit->rank_id == RANK_KING )
		defenseRating += 50;

	if( firmPtr->cast_to_FirmCamp() )
	{
		FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

		for( int i=0 ; i<firmCamp->soldier_count ; i++ )
		{
			int spyRecno = firmCamp->soldier_array[i].spy_recno;

			//------ if this soldier is a spy ------//

			if( spyRecno )
			{
				Spy* spyPtr = spy_array[spyRecno];

				if( spyPtr->true_nation_recno == true_nation_recno )	// our spy
				{
					attackRating += spyPtr->spy_skill/4 * (1+assassinTechLevel);
				}
				else if( spyPtr->true_nation_recno == firmCamp->nation_recno ) // enemy spy
				{
					defenseRating += spyPtr->spy_skill/2 * (1+counterSpyLevel);
					otherDefenderCount++;
				}
			}
			else //----- if this soldier is not a spy ------//
			{
				defenseRating += 4 + firmCamp->soldier_array[i].hit_points/20;
				otherDefenderCount++;
			}
		}
	}

	//-------- if the assassination succeeds -------//

	// ####### begin Gilbert 12/12 #######//
	//defenseRating += 30 + m.random(30);

	// don't add random component here
	attackRating *= 3;		// normalize to 100
	defenseRating = defenseRating * 3 + 100;
	// ####### end Gilbert 12/12 #######//

	return 1;
}
//---------- End of function Spy::get_assassinate_rating --------//


//-------- Begin of function Spy::can_camouflage ------//
//
int Spy::can_camouflage()
{
	return camouflage_power >= MIN_CAMOUFLAGE_POWER
		&& true_nation_recno && tech_res[TECH_CAMOUFLAGE]->get_nation_tech_level(true_nation_recno)
		&& cloaked_nation_recno != true_nation_recno;
}
//-------- End of function Spy::can_camouflage ------//


//-------- Begin of function Spy::start_camouflage ------//
//
void Spy::start_camouflage(char remoteAction)
{
	if( camouflage_power < MIN_CAMOUFLAGE_POWER )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// ######### begin Gilbert 7/10 ########//
		// packet structure : <spy recno> <0=start, 1=stop>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CAMOUFLAGE, 2*sizeof(short));
		shortPtr[0] = spy_recno;
		shortPtr[1] = 0;
		// ######### end Gilbert 7/10 ########//
		return;
	}

	err_when( camouflage_power < MIN_CAMOUFLAGE_POWER );	// call can_camouflage to disable camouflage
	camouflage_count = camouflage_power;
	camouflage_power = 0;
}
//-------- End of function Spy::start_camouflage ------//


//-------- Begin of function Spy::end_camouflage ------//
//
void Spy::stop_camouflage(char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// ######### begin Gilbert 7/10 ########//
		// packet structure : <spy recno> <0=start, 1=stop>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CAMOUFLAGE, 2*sizeof(short));
		shortPtr[0] = spy_recno;
		shortPtr[1] = 1;
		// ######### end Gilbert 7/10 ########//
		return;
	}

	int maxCamouflagePower = MAX_CAMOUFLAGE_POWER(spy_skill);

	camouflage_power += camouflage_count;		// recharge unused camouflage_count
	camouflage_power = min(camouflage_power, maxCamouflagePower);
	camouflage_count = 0;

	// ------ explore area if mobile ---------//

	if( spy_place == SPY_MOBILE )
	{
		err_when( unit_array.is_truly_deleted(spy_place_para) );

		Unit *unitPtr = unit_array[spy_place_para];

		unitPtr->explore_area();
	}
}
//-------- End of function Spy::end_camouflage ------//



// -------- begin of function Spy::steal_tech --------//

int Spy::steal_tech(int targetNationRecno, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_SPY_STEAL_TECH, 2*sizeof(short) );
		// packet structure : <spy recno> <target nation recno>
		shortPtr[0] = spy_recno;
		shortPtr[1] = targetNationRecno;
		return 1;
	}

	// --------- validate first ------------//

	if( spy_place != SPY_FIRM )
		return 0;

//	if( !get_steal_tech_rating(targetUnitRecno, attackRating, defenseRating, otherDefenderCount) )
//		return 0;

	int rc;
	int trueNationRecno = true_nation_recno;		// need to save it first as the spy may be killed later

	Firm* firmPtr = firm_array[ spy_place_para ];
	if( firmPtr->firm_recno != spy_place_para )
		return 0;

	FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
	// any other firm that assassination can take place
	if( !firmCamp )		// && other firms...
		return 0;

	int rating = get_steal_rating(targetNationRecno);

	if( m.random(100) < rating )
	{
		// randomly find a tech

		int matchTech = -2;
		int matchCount = 0;

		for( int i = 1; i <= tech_res.tech_count; ++i )
		{
			TechInfo *techInfo = tech_res[i];

			if( (techInfo->race_id >= 0) != (race_id >= 0) )		// human get human's, monster get monster
				continue;

			// ####### begin Gilbert 2/3 ########//

			// ------ cannot steal mega weapon. If you want to get, kill the king --------//

			if( techInfo->class_id == TECH_CLASS_MEGA_WEAPON )
				continue;
			// ####### end Gilbert 2/3 ########//

			if( techInfo->get_nation_tech_level(true_nation_recno) >= techInfo->get_nation_tech_level(targetNationRecno) )
				continue;					// skip tech we are more advanced

			++matchCount;
			if( m.random(matchCount) == 0 )	// n-th match have 1/n chance to be selected, therefore evenly distributed
				matchTech = i;
		}

		rc = matchTech;		// if no match, -2 meaning no research stolen
		
		// ----- set nation tech level -----//

		if( matchTech > 0 )
		{
			TechInfo *techInfo = tech_res[matchTech];
			techInfo->set_nation_tech_level(true_nation_recno, techInfo->get_nation_tech_level(targetNationRecno));
		}

	}
	else
	{
		get_killed(0);		// 0 - don't display new message for the spy being killed

		rc = -1;		// failed
	}

	//--- if this firm is the selected firm and the spy is the player's spy ---//

	if( trueNationRecno == nation_array.player_recno &&
		 firmPtr->firm_recno == firm_array.selected_recno )
	{
		if( firmCamp )
		{
			firmCamp->steal_tech_result = rc;
			firmCamp->firm_menu_mode = FIRM_MENU_STEAL_TECH_RESULT;
		}
		// info.disp();
	}

	return rc;
}
// -------- end of function Spy::steal_tech --------//


// -------- begin of function Spy::get_steal_rating --------//

int Spy::get_steal_rating(int targetNationRecno)
{
	int chance = spy_skill;
	int sumResist = 0;
	int maxResist = 0;
	int resistCount = 0;
	int sumAssist = 0;
	int counterSpyLevel = tech_res[TECH_COUNTER_SPY]->get_nation_tech_level(targetNationRecno);

	// ##### patch begin Gilbert 16/2 #####//
	if( god_res[GOD_PERSIAN]->nation_prayer_count(true_nation_recno) > 0 )
		chance += 10;
	// ##### patch end Gilbert 16/2 #####//

	// reduced by counter spy of that nation

	if( spy_place != SPY_FIRM || firm_array.is_deleted(spy_place_para) )
		return 0;

	FirmCamp *firmCamp;

	if( (firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp()) )
	{
		for( int i = 0; i <= firmCamp->soldier_count; ++i )
		{
			int otherSpyRecno;
			if( i==0 )
			{
				if( !firmCamp->overseer_recno )
					continue;
				otherSpyRecno = unit_array[firmCamp->overseer_recno]->spy_recno;
			}
			else
			{
				otherSpyRecno = firmCamp->soldier_array[i-1].spy_recno;
			}

			if( !otherSpyRecno )
				continue;

			if( otherSpyRecno == spy_recno)		// skip this spy
				continue;

			Spy *otherSpy = spy_array[otherSpyRecno];

			Soldier *soldierPtr = firmCamp->soldier_array + i;

			if( otherSpy->true_nation_recno == targetNationRecno )
			{
				sumResist += otherSpy->spy_skill;
				maxResist = max( otherSpy->spy_skill, maxResist );
				++resistCount;
			}
			if( otherSpy->true_nation_recno == true_nation_recno )
			{
				sumAssist += otherSpy->spy_skill;
			}
		}
	}
	else
	{
		err_here();
	}

	if( resistCount == 0 )
		chance -= 20 - sumAssist / 10;			// so a 100 spy skill still have chance to fail
	// ##### patch begin Gilbert 13/10 ######//
//	else if( counterSpyLevel )
//		chance -= 16 + sumResist / 8 + maxResist / 8 - sumAssist / 10;
//	else
//		chance -= 16 + sumResist / 5 + maxResist / 5 - sumAssist / 8;
	else if( counterSpyLevel )
		chance -= 16 + sumResist / 5 + maxResist / 5 - sumAssist / 8;
	else
		chance -= 16 + sumResist / 8 + maxResist / 8 - sumAssist / 10;
	// ##### patch end Gilbert 13/10 ######//

	chance = max(chance, 0);
	chance = min(chance, 100);
	return chance;
}
// -------- end of function Spy::get_steal_rating --------//


// -------- begin of function Spy::create_incident --------//
//
int Spy::create_incident(int targetNationRecno, int involvedNationRecno, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_SPY_CREATE_INCIDENT, 3*sizeof(short) );
		// packet structure : <spy recno> <target nation recno> <involved natino recno>
		shortPtr[0] = spy_recno;
		shortPtr[1] = targetNationRecno;
		shortPtr[2] = involvedNationRecno;
		return 1;
	}

	// --------- validate first ------------//

	if( spy_place != SPY_FIRM )
		return 0;

	if( nation_array.is_deleted(targetNationRecno) || nation_array.is_deleted(involvedNationRecno) )
		return 0;

	int rc;
	int trueNationRecno = true_nation_recno;		// need to save it first as the spy may be killed later

	Firm* firmPtr = firm_array[ spy_place_para ];
	if( firmPtr->firm_recno != spy_place_para )
		return 0;

	FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
	// any other firm that assassination can take place
	if( !firmCamp )		// && other firms...
		return 0;

	int rating = get_incident_rating(targetNationRecno, involvedNationRecno);

	if( m.random(100) < rating )
	{
		talk_res.create_incident( targetNationRecno, involvedNationRecno );
		rc = involvedNationRecno;
	}
	else
	{
		get_killed(0);		// 0 - don't display new message for the spy being killed
		rc = -1;		// failed
	}

	//--- if this firm is the selected firm and the spy is the player's spy ---//

	if( trueNationRecno == nation_array.player_recno &&
		 firmPtr->firm_recno == firm_array.selected_recno )
	{
		if( firmCamp )
		{
			firmCamp->create_incident_result = rc;
			firmCamp->firm_menu_mode = FIRM_MENU_INCIDENT_RESULT;
		}
		// info.disp();
	}

	return rc;
}
// -------- end of function Spy::create_incident --------//


// -------- begin of function Spy::get_incident_rating --------//
//
int Spy::get_incident_rating(int targetNationRecno, int involvedNationRecno)
{
	int chance = spy_skill;
	int sumResist = 0;
	int maxResist = 0;
	int resistCount = 0;
	int sumAssist = 0;
	int counterSpyLevel = tech_res[TECH_COUNTER_SPY]->get_nation_tech_level(targetNationRecno);

	// ##### patch begin Gilbert 16/2 #####//
	if( god_res[GOD_PERSIAN]->nation_prayer_count(true_nation_recno) > 0 )
		chance += 10;
	// ##### patch end Gilbert 16/2 #####//

	// reduced by counter spy of that nation

	if( spy_place != SPY_FIRM || firm_array.is_deleted(spy_place_para) )
		return 0;

	FirmCamp *firmCamp;

	if( (firmCamp = firm_array[spy_place_para]->cast_to_FirmCamp()) )
	{
		for( int i = 0; i <= firmCamp->soldier_count; ++i )
		{
			int otherSpyRecno;
			if( i==0 )
			{
				if( !firmCamp->overseer_recno )
					continue;
				otherSpyRecno = unit_array[firmCamp->overseer_recno]->spy_recno;
			}
			else
			{
				otherSpyRecno = firmCamp->soldier_array[i-1].spy_recno;
			}

			if( !otherSpyRecno )
				continue;

			if( otherSpyRecno == spy_recno)		// skip this spy
				continue;

			Spy *otherSpy = spy_array[otherSpyRecno];

			Soldier *soldierPtr = firmCamp->soldier_array + i;

			if( otherSpy->true_nation_recno == targetNationRecno 
				|| otherSpy->true_nation_recno == involvedNationRecno )
			{
				sumResist += otherSpy->spy_skill;
				maxResist = max( otherSpy->spy_skill, maxResist );
				++resistCount;
			}
			if( otherSpy->true_nation_recno == true_nation_recno )
			{
				sumAssist += otherSpy->spy_skill;
			}
		}
	}
	else
	{
		err_here();
	}

	if( resistCount == 0 )
		chance -= 20 - sumAssist / 10;			// so a 100 spy skill still have chance to fail
	// ##### patch begin Gilbert 13/10 ######//
//	else if( counterSpyLevel )
//		chance -= 16 + sumResist / 8 + maxResist / 8 - sumAssist / 10;
//	else
//		chance -= 16 + sumResist / 5 + maxResist / 5 - sumAssist / 8;
	else if( counterSpyLevel )
		chance -= 16 + sumResist / 5 + maxResist / 5 - sumAssist / 8;
	else
		chance -= 16 + sumResist / 8 + maxResist / 8 - sumAssist / 10;
	// ##### patch end Gilbert 13/10 ######//

	chance = max(chance, 0);
	chance = min(chance, 100);
	return chance;
}
// -------- end of function Spy::get_incident_rating --------//
