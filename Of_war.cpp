//Filename    : OF_WAR.CPP
//Description : Firm War Factory

#include <OINFO.H>
#include <OMODEID.H>
#include <OSTR.H>
#include <OCONFIG.H>
#include <OUNIT.H>
#include <OSYS.H>
#include <ONATION.H>
#include <OREMOTE.H>
#include <OSE.H>
#include <OSERES.H>
#include <OF_WAR.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <OGODRES.H>

//--------- Begin of function FirmWar::FirmWar ---------//
//
FirmWar::FirmWar()
{
	build_unit_id = 0;
	build_queue_count = 0;
}
//----------- End of function FirmWar::FirmWar -----------//


//--------- Begin of function FirmWar::~FirmWar ---------//
//
FirmWar::~FirmWar()
{
}
//----------- End of function FirmWar::~FirmWar -----------//



//--------- Begin of function FirmWar::add_queue ---------//
//
void FirmWar::add_queue(int unitId)
{
	if( build_queue_count+(build_unit_id>0)==MAX_BUILD_QUEUE )
		return;

	build_queue_array[build_queue_count++] = unitId;
}
//----------- End of function FirmWar::add_queue -----------//


//--------- Begin of function FirmWar::remove_queue ---------//
//
void FirmWar::remove_queue(int unitId)
{
	for( int i=build_queue_count-1 ; i>=0 ; i-- )
	{
		if( build_queue_array[i] == unitId )
		{
			err_when( build_queue_count > MAX_BUILD_QUEUE );

			m.del_array_rec( build_queue_array, build_queue_count, sizeof(build_queue_array[0]), i+1 );

			build_queue_count--;
			return;
		}
	}

	if(build_unit_id==unitId)
		cancel_build_unit();
}
//----------- End of function FirmWar::remove_queue -----------//


//--------- Begin of function FirmWar::next_day ---------//
//
void FirmWar::next_day()
{
	//----- call next_day() of the base class -----//

	FirmWork::next_day();

	//--------- process building weapon -------//

	if( build_unit_id )
		process_build();
	else
		process_queue();
}
//----------- End of function FirmWar::next_day -----------//


//--------- Begin of function FirmWar::process_queue ---------//
//
void FirmWar::process_queue()
{
	if( build_queue_count==0 )
		return;

	// ######## begin Gilbert 30/12 #######//
	// --- cancel if technology lost -----//

	// do not cancel build in progress because tech may be recovered
	// cancel when it is going to build

	if( nation_recno )
	{
		// ---- delete queue until the first unit in the queue can build ----//

		while( build_queue_count > 0 
			&& unit_res[build_queue_array[0]]->get_nation_tech_level(nation_recno) == 0 )
		{
			m.del_array_rec( build_queue_array, build_queue_count, sizeof(build_queue_array[0]), 1 );
			build_queue_count--;
		}
	}

	if( build_queue_count==0)
		return;
	// ######## end Gilbert 30/12 #######//



	//--- first check if the nation has enough money to build the weapon ---//

	Nation* nationPtr = nation_array[nation_recno];
	build_unit_id = build_queue_array[0];

	if( nationPtr->cash < unit_res[build_unit_id]->build_cost )
	{
		build_unit_id = 0;
		return;
	}

	nationPtr->add_expense( EXPENSE_WEAPON, unit_res[build_unit_id]->build_cost, 1);

	err_when( build_queue_count > MAX_BUILD_QUEUE );

	m.del_array_rec( build_queue_array, build_queue_count, sizeof(build_queue_array[0]), 1 );

	build_queue_count--;

	//------- set building parameters -------//

	last_process_build_frame_no = sys.frame_count;
	build_progress_days = (float) 0;

	if( firm_array.selected_recno == firm_recno )
	{
		// disable_refresh = 1;
		// info.disp();
		// disable_refresh = 0;
	}
}
//----------- End of function FirmWar::process_queue -----------//


//--------- Begin of function FirmWar::process_build ---------//
//
void FirmWar::process_build()
{
	err_when( !build_unit_id );

	UnitInfo* unitInfo = unit_res[build_unit_id];

	int   totalBuildDays = unitInfo->build_days;
	int	bonusRate = 100;

	if( nation_recno )
	{
		bonusRate += tech_res[TECH_WAR_MACHINE]->get_nation_tech_level(nation_recno) * 25;
		// ##### begin Gilbert 16/2 ######//
		bonusRate += god_res[GOD_INDIAN]->nation_prayer_count(nation_recno) * 30 / MAX_WORKER;	// 30% increase
		// ##### end Gilbert 16/2 ######//
	}

	// productiviey / 100 * (1 + techlevel /4.0 )

	build_progress_days += (float) productivity * bonusRate / 15000;

	//--- if the nation has researched advance war machine manufacturing ---//

	//---------------------------------------------------//

	last_process_build_frame_no = sys.frame_count;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		build_progress_days += (float) 50;

	if( build_progress_days > totalBuildDays )
	{
		SpriteInfo*	spriteInfo = sprite_res[ unitInfo->sprite_id ];
		int 			xLoc=loc_x1; // xLoc & yLoc are used for returning results
		int 			yLoc=loc_y1;

		if( !world.locate_space(xLoc, yLoc, loc_x2, loc_y2,
			 spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type) )
		{
			build_progress_days = (float)(totalBuildDays + 1);
			return;
		}

		int unitRecno = unit_array.add_unit( build_unit_id, nation_recno, 0, 0, xLoc, yLoc );

		// -------- assign to rally point -------//

		if( unitRecno )
			send_to_rally_point( unitRecno );

		// ------------------------------------//

		if( firm_array.selected_recno == firm_recno )
		{
			// BUGHERE : disable_refresh was a static variable
			//disable_refresh = 1;
			// info.disp();
			//disable_refresh = 0;
		}

		if( is_own() )
			se_res.far_sound(center_x, center_y, 1, 'F', firm_id, "FINS", 'S', unit_res[build_unit_id]->sprite_id);

		build_unit_id = 0;
	}
}
//----------- End of function FirmWar::process_build -----------//


//--------- Begin of function FirmWar::cancel_build_unit ---------//
void FirmWar::cancel_build_unit()
{
	build_unit_id = 0;

	if( firm_array.selected_recno == firm_recno )
	{
//		disable_refresh = 1;
//		info.disp();
//		disable_refresh = 0;
	}
}
//----------- End of function FirmWar::cancel_build_unit -----------//


// ---------- Begin of function FirmWar::can_set_rally_point ------//
//
// whether the firm can set rally point
//
// [int] destBaseObjRecno		- destination base obj recno, 0 for setting position
//
int FirmWar::can_set_rally_point(int destBaseObjRecno)
{
	if( destBaseObjRecno == 0 )
	{
		return 1;
	}
	else
	{
		if( base_obj_array.is_deleted(destBaseObjRecno) )
			return 0;

		Firm *firmPtr = base_obj_array[destBaseObjRecno]->cast_to_Firm();
		if( firmPtr )
		{
			if( firmPtr->cast_to_FirmCamp() )
			{
				return 1;
			}
		}
	}

	return 0;
}
// ---------- End of function FirmWar::can_set_rally_point ------//
