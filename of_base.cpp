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

//Filename    : OF_BASE.CPP
//Description : Firm Base

#include <OF_BASE.H>
#include <OMODEID.H>
#include <OMOUSE.H>
#include <OHELP.H>
#include <OSTR.H>
#include <OCONFIG.H>
#include <OSYS.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OGODRES.H>
#include <OREMOTE.H> 
#include <OSERES.H>
#include <OSE.H>
#include <OEFFECT.H>
#include <ONEWS.H>

// -------- define static variable --------//

char FirmBase::invoked_effect_text[200];

//--------- Begin of function FirmBase::FirmBase ---------//
//
FirmBase::FirmBase()
{
	needed_worker_count = MAX_WORKER;

	invoked_effect_id = 0;
	invoked_effect_para = 0;
	invoked_effect_last = 0;
}
//----------- End of function FirmBase::FirmBase -----------//


//--------- Begin of function FirmBase::~FirmBase ---------//
//
FirmBase::~FirmBase()
{
	if( sys.quick_exit_flag() )
		return;

	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]--;

	err_when( nation_array[nation_recno]->base_count_array[race_id-1] < 0 );
}
//----------- End of function FirmBase::~FirmBase -----------//


//--------- Begin of function FirmBase::init_derived ---------//
//
void FirmBase::init_derived()
{
	pray_points = (float) 0;

	//------ increase NationBase::base_count_array[] -----//

	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]++;

	//---------- set the god id. ----------//

	god_id = 0 ;
	god_unit_recno = 0;

	Nation* nationPtr = nation_array[nation_recno];
	for( int i=1 ; i<=god_res.god_count ; i++ )
	{
		if( god_res[i]->race_id == race_id &&
			 (god_res[i]->is_nation_know(nation_recno) || nationPtr->know_base_array[god_res[i]->race_id-1]))
		{
			god_id = i;
			break;
		} 
	}

	err_when( god_id==0 );
}
//----------- End of function FirmBase::init_derived -----------//


//--------- Begin of function FirmBase::next_day ---------//
//
void FirmBase::next_day()
{
	//----- call next_day() of the base class -----//

	FirmWork::next_day();

	//--------------------------------------//

	calc_productivity();

	//------- increase pray points --------//

	if( pray_points < MAX_PRAY_POINTS )
	{
		if( config.fast_build && is_own() )
			pray_points += productivity;
		else
			pray_points += (float) productivity / 100;

		if( pray_points > MAX_PRAY_POINTS )
			pray_points = (float) MAX_PRAY_POINTS;
	}
	else
		if (is_own() && (info.game_date + firm_recno)%60 == 0)
			news_array.firm_idle(cast_to_Firm()->firm_recno);

	//------ validate god_unit_recno ------//

	if( god_unit_recno )
	{
		if( unit_array.is_deleted(god_unit_recno) )
			god_unit_recno = 0;

	#ifdef DEBUG
		if( god_unit_recno )
		{
			err_when( !unit_array[god_unit_recno]->is_visible() );
			err_when( unit_array[god_unit_recno]->nation_recno != nation_recno );
		}
	#endif
	}
	if (invoked_effect_id && invoked_effect_last > 0)
	{
		invoked_effect_last--;
		if (invoked_effect_last == 0)
			invoked_effect_id = 0; 
	}
}
//----------- End of function FirmBase::next_day -----------//


//------- Begin of function FirmBase::change_nation ---------//
//
void FirmBase::change_nation(int newNationRecno)
{
	//------ update base_count_array[] --------//

	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]--;

	err_when( nation_array[nation_recno]->base_count_array[race_id-1] < 0 );

	nation_array[newNationRecno]->base_count_array[race_id-1]++;

	//----- change the nation recno of the god invoked by the base if there is any ----//

	if( god_unit_recno && !unit_array.is_deleted(god_unit_recno) )
		unit_array[god_unit_recno]->change_nation(newNationRecno);

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmBase::change_nation ---------//


//--------- Begin of function FirmBase::invoke_god ---------//
//
// Invoke God.
//
void FirmBase::invoke_god()
{
	god_unit_recno = god_res[god_id]->invoke(firm_recno, center_x, center_y);
	GodInfo* godInfo = god_res[god_id];
	short effectId = godInfo->out_effect_id;
	Unit* unitPtr = unit_array[god_unit_recno];
	if( effectId )
	//	Effect::create(effectId, center_x *LOCATE_WIDTH, center_y *LOCATE_HEIGHT, SPRITE_IDLE, 1, 8, 0);
		Effect::create(effectId, unitPtr->next_x_loc() *LOCATE_WIDTH, unitPtr->next_y_loc() *LOCATE_HEIGHT, SPRITE_IDLE, 1, 8, 0);
}
//----------- End of function FirmBase::invoke_god -----------//


//--------- Begin of function FirmBase::can_invoke ---------//
//
int FirmBase::can_invoke()
{
	//----- if the base's god creature has been destroyed -----//

	if( god_unit_recno && unit_array.is_deleted(god_unit_recno) )
		god_unit_recno = 0;

	//---------------------------------------------------------//

	return !god_unit_recno &&		// one base can only support one god
			 pray_points >= MAX_PRAY_POINTS/10;		// there must be at least 10% of the maximum pray points to cast a creature
}
//----------- End of function FirmBase::can_invoke -----------//
