//Filename	 : OUN_MODE.CPP
//Description: Unit mode processing functions

#include <OSYS.H>
#include <ONATION.H>
#include <OFIRM.H>
#include <OTOWN.H>
#include <OUNITRES.H>
#include <OUNIT.H>

 
//----------- Begin of function Unit::set_mode ---------//
//
void Unit::set_mode(char modeId, short modePara)
{
	unit_mode 		= modeId;
	unit_mode_para = modePara;

	err_when( modeId && unit_mode_para==0 );

	//--- in defense mode, set the last_attack_date to today for determing when the unit should return home ---//

	if( unit_mode == UNIT_MODE_TOWN_DEFENDER ||
		 unit_mode == UNIT_MODE_CAMP_DEFENDER )
	{
		last_attack_date = info.game_date;
	}
}
//----------- End of function Unit::set_mode ---------//


//--------- Begin of function Unit::process_mode_town_defender ------//
//
void Unit::process_mode_town_defender()
{
	//--- if the home town of this unit has been destroyed ---//

	if( town_array.is_deleted(unit_mode_para) ||
		 !town_array[unit_mode_para]->can_accept_assign(sprite_recno) )	// or it no longer accepts this unit
	{
		set_mode(0);		// reset the mode to normal
		return;
	}

	//-- if it has been 10 days since the unit last attack, return town now --//

	if( is_all_stop() && info.game_date - last_attack_date > 10 )
	{
		Town* townPtr = town_array[unit_mode_para];

		assign( townPtr->loc_x1, townPtr->loc_y1 );
	}
}
//--------- End of function Unit::process_mode_town_defender --------//


//--------- Begin of function Unit::process_mode_camp_defender ------//
//
void Unit::process_mode_camp_defender()
{
	//---- if the unit has an action now, return ----//

	if( !is_all_stop() )
		return;

	//--- if the home camp of this unit has been destroyed ---//

	if( firm_array.is_deleted(unit_mode_para) ||
		 !firm_array[unit_mode_para]->can_accept_assign(sprite_recno) )	// or it no longer accepts this unit
	{
		set_mode(0);		// reset the mode to normal
		return;
	}

	//-- if it has been 10 days since the unit last attack, return camp now --//

	Firm* homeFirm = firm_array[unit_mode_para];

	if( info.game_date - last_attack_date > 8 )
	{
		assign( homeFirm->loc_x1, homeFirm->loc_y1 );
		return;
	}

	//------ look for enemies to attack --------//

	int attackScanRange = EFFECTIVE_DEFEND_DISTANCE;

	int		 xOffset, yOffset;
	int		 xLoc, yLoc;
	Location* locPtr;
	int		 curXLoc = homeFirm->center_x;
	int		 curYLoc = homeFirm->center_y;
	BYTE	 	 regionId = world.get_region_id(curXLoc, curYLoc);

	//---- set the starting and endding point for the detect action ----//

	enum { DETECT_INTERVAL_DIVIDER = 6 };		// it takes 6 days to complete a single detect action

	int totalPoint     = attackScanRange*attackScanRange;
	int detectInterval = totalPoint / DETECT_INTERVAL_DIVIDER;

	int startPoint = detectInterval * ( (info.game_date-last_attack_date) % DETECT_INTERVAL_DIVIDER);		//	use (info.game_date-last_attack_date) so that the unit always look for nearest enemies first
	int endPoint   = startPoint + detectInterval - 1;

	if( sys.frame_count % DETECT_INTERVAL_DIVIDER == DETECT_INTERVAL_DIVIDER-1 )		// if this is last in the series
		endPoint = totalPoint;

	//-------------------------------------------//

	for( int i=startPoint ; i<=endPoint ; i++ )
	{
		m.cal_move_around_a_point(i, attackScanRange, attackScanRange, xOffset, yOffset);

		xLoc = curXLoc + xOffset;
		yLoc = curYLoc + yOffset;

		xLoc = max(0, xLoc);
		xLoc = min(MAX_WORLD_X_LOC-1, xLoc);

		yLoc = max(0, yLoc);
		yLoc = min(MAX_WORLD_Y_LOC-1, yLoc);

		locPtr = world.get_loc(xLoc, yLoc);

		if( locPtr->region_id != regionId )
			continue;

		//----- if there is a unit on the location ------//

		int mobileType;
		int unitRecno = locPtr->get_any_unit(mobileType);

		if( unitRecno )
		{
			if( unit_array.is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = unit_array[unitRecno];

			if( unitPtr->nation_recno != nation_recno &&
				 should_aggressive_attack(unitPtr->base_obj_recno) )
			{
				attack(unitPtr->base_obj_recno, false);
				return;
			}
		}
	}
}
//--------- End of function Unit::process_mode_camp_defender --------//


//--------- Begin of function Unit::process_mode_rebel ------//
//
void Unit::process_mode_rebel()
{
}
//--------- End of function Unit::process_mode_rebel --------//

