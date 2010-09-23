// Filename     : OF_MFORT.CPP
// Description  : Monster Fortress


#include <OF_MFORT.H>
#include <OSYS.H>
#include <OREMOTE.H>
#include <OPOWER.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OROCKRES.H>
#include <OTERRAIN.H>
#include <ONATIONA.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OSITE.H>
#include <OGAME.H>
#include <OITEMRES.H>


// ----- begin of function FirmMonsterFortress::FirmMonsterFortress -----//
//
FirmMonsterFortress::FirmMonsterFortress(int supportUnitId)
{
	memset( sizeof(Firm) + (char *)this, 0, sizeof(FirmMonsterFortress)-sizeof(Firm) );
	support_unit_id = supportUnitId;
	max_hit_point_ratio = 1.0f;
}
// ----- end of function FirmMonsterFortress::FirmMonsterFortress -----//


// ----- begin of function FirmMonsterFortress::init_derived -----//
//
void FirmMonsterFortress::init_derived()
{
	// ---- calc max hit point ratio from tech level
	if( nation_recno )
		max_hit_point_ratio = firm_res[firm_id]->get_nation_tech_level(nation_recno) * 0.25f + 0.75f;
	else
		max_hit_point_ratio = 1.0f;


	if( nation_recno 
		&& tech_res[TECH_FORTRESS_CAMOUFLAGE]->get_nation_tech_level(nation_recno) )
	{
		rock_id = rock_res.search("R", obj_loc_width(), obj_loc_width(), 
			obj_loc_height(), obj_loc_height(), 0, 0, 
			terrain_res[world.get_loc(center_x, center_y)->terrain_id]->average_type );

		if( !rock_id )
		{
			rock_id = rock_res.search("PR", obj_loc_width()-1, obj_loc_width(), 
				obj_loc_height()-1, obj_loc_height(), 0, 0, 
				terrain_res[world.get_loc(center_x, center_y)->terrain_id]->average_type );
		}	
	}
}
// ----- end of function FirmMonsterFortress::init_derived -----//


// ----- begin of function FirmMonsterFortress::deinit -----//
//
void FirmMonsterFortress::deinit()
{
	Firm::deinit();
}
// ----- end of function FirmMonsterFortress::deinit -----//


// ----- begin of function FirmMonsterFortress::next_day -----//
//
void FirmMonsterFortress::next_day()
{
	process_scan_target();

	// BUGHERE : maintain loyalty of extra_builders

	//----- call next_day() of the base class -----//

	Firm::next_day();
}
// ----- end of function FirmMonsterFortress::next_day -----//


// ----- begin of function FirmMonsterFortress::pay_expense -----//
//
void FirmMonsterFortress::pay_expense()
{
	Firm::pay_expense();

	// unit_array::process call pay_expense of each unit
}
// ----- end of function FirmMonsterFortress::pay_expense -----//


// ----- begin of function FirmMonsterFortress::should_show_info -----//
//
int FirmMonsterFortress::should_show_info()
{
	if( Firm::should_show_info() )
	{
		return 1;
	}
	else
	{
		int i;
		for( i = 0 ; i < extra_builder_count; ++i )
		{
			if( unit_array[extra_builder_recno[i]]->is_own_spy() )
				return 1;
		}
		for( i = 0 ; i < archer_count; ++i )
		{
			if( unit_array[archer_unit_recno[i]]->is_own_spy() )
				return 1;
		}
	}

	return 0;
}
// ----- end of function FirmMonsterFortress::should_show_info -----//


// ----- begin of function FirmMonsterFortress::is_extra_builder_full -----//
//
int FirmMonsterFortress::is_extra_builder_full()
{
	return extra_builder_count >= MAX_EXTRA_BUILDER;
}
// ----- end of function FirmMonsterFortress::is_extra_builder_full -----//


// ----- begin of function FirmMonsterFortress::builder_stay_after_construction -----//
//
int FirmMonsterFortress::builder_stay_after_construction()
{
	if( !builder_recno )
		return 0;

	Unit *unitPtr = unit_array[builder_recno];

	unitPtr->set_mode(0);			// reset it from UNIT_MODE_CONSTRUCT_???

	hit_points = place_max_hit_points = 0.0f;
	err_when( under_construction );		// assume finish construction for assign_extra_builder to work correctly

	// recalcuate hit_points and place_max_hit_points

	for( int i = 0; i < extra_builder_count; ++i )
	{
		hit_points += unit_array[extra_builder_recno[i]]->hit_points * max_hit_point_ratio;
		place_max_hit_points += unit_array[extra_builder_recno[i]]->max_hit_points() * max_hit_point_ratio;
	}

	// assign_extra_builder(builder_recno);
	assign_unit(builder_recno);
	return 1;
}
// ----- end of function FirmMonsterFortress::builder_stay_after_construction -----//


// ----- begin of function FirmMonsterFortress::assign_extra_builder -----//
//
void FirmMonsterFortress::assign_extra_builder(int builderRecno)
{
	if( unit_array.is_deleted(builderRecno) )
		return;

	//--- if the new builder's nation is not the same as the firm's nation, don't assign ---//

	if( builderRecno && unit_array[builderRecno]->nation_recno != nation_recno )
		return;

	// ----- if enough space for extra builder -------//

	if( is_extra_builder_full() )
		return;

	//------------------------------------------//

	Unit *unitPtr = unit_array[builderRecno];

	//----- reset the new builder's team info and home camp ------//

	unitPtr->home_camp_firm_recno 	= 0;

	int originalXLoc = unitPtr->next_x_loc();
	int originalYLoc = unitPtr->next_y_loc();

	err_when( unitPtr->hit_points <= 0 );

	unitPtr->deinit_sprite();

	// -------- set unit mode --------//

	unitPtr->set_mode(UNIT_MODE_OVERSEE, firm_recno);

	//--------- if the unit is a spy -----------//

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place( SPY_FIRM, firm_recno );

	//------- update loyalty -------//

	unitPtr->update_loyalty();

	// ------ put unit recno in the array ------//

	extra_builder_recno[extra_builder_count] = builderRecno;
	extra_builder_count++;

	// ------ recalc hit_points and max_hit_points -------//

	if( !under_construction )
	{
		hit_points += unitPtr->hit_points * max_hit_point_ratio;
		place_max_hit_points += unitPtr->max_hit_points() * max_hit_point_ratio;
	}
	else
	{
		// when construction complete, builder_stay_after_construction will
		// invoke to re-calc hit_points and place_max_hit_points
	}
}
// ----- end of function FirmMonsterFortress::assign_extra_builder -----//


// ----- begin of function FirmMonsterFortress::mobilize_extra_builder -----//
//
int FirmMonsterFortress::mobilize_extra_builder(int builderId)
{
	if( builderId <= 0 || builderId > extra_builder_count )
		return 0;

	short builderRecno = extra_builder_recno[builderId-1];
	Unit* unitPtr = unit_array[builderRecno];

	//-------- if the overseer is a spy -------//

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place(SPY_MOBILE, builderRecno);

	//----- get this overseer out of the firm -----//

	SpriteInfo* spriteInfo = sprite_res[unit_res[unitPtr->unit_id]->sprite_id];
	int         xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results

	int spaceFound = locate_space(is_being_deleted, xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height);

	if(spaceFound)
	{
		unitPtr->init_sprite(xLoc, yLoc);
		unitPtr->set_mode(0);        // reset overseen firm recno
	}
	else
	{
		unit_array.del(builderRecno);		// delete it when there is no space for the unit
	}

	//--------- del builder recno from the builder array -------------//

	m.del_array_rec( extra_builder_recno, MAX_EXTRA_BUILDER, sizeof(extra_builder_recno[0]), builderId );
	--extra_builder_count;

	//------- update loyalty -------//

	if( !unit_array.is_deleted(builderRecno) )
	{
		unitPtr->update_loyalty();
		unitPtr->home_camp_firm_recno = 0;	// builder do not set home_camp_firm_recno
	}

	// ------ evenly distrbute hit points ---------//

	if( !under_construction )
	{
		err_when( place_max_hit_points == 0.0f );
		float ratio = hit_points / place_max_hit_points;
		unitPtr->hit_points = unitPtr->max_hit_points() * ratio;
		if( unitPtr->hit_points < 1.0f )
			unitPtr->hit_points = 1.0f;

		// recalc max_hit_points instead of minus unitPtr->hit_points
		place_max_hit_points = 0;
		for(int i = 0; i < extra_builder_count; ++i )
		{
			place_max_hit_points += unit_array[extra_builder_recno[i]]->max_hit_points() * max_hit_point_ratio;
		}
		hit_points = ratio * place_max_hit_points;
	}
	else
	{
		// destroy the building while construction,
		// unit receive the same hit point
	}

	return builderRecno;
}
// ----- end of function FirmMonsterFortress::mobilize_extra_builder -----//


// ----- begin of function FirmMonsterFortress::kill_extra_builder -----//
//
void FirmMonsterFortress::kill_extra_builder(int builderId)
{
	if( builderId <= 0 || builderId > extra_builder_count )
		return;

	short builderRecno = extra_builder_recno[builderId-1];

	// ####### begin Gilbert 24/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(unit_array[builderRecno]->unique_id);		// check unit killed
	// ####### end Gilbert 24/2 ########//

	unit_array.del(builderRecno);		// delete it when there is no space for the unit

	//--------- del builder recno from the builder array -------------//

	m.del_array_rec( extra_builder_recno, MAX_EXTRA_BUILDER, sizeof(extra_builder_recno[0]), builderId );
	--extra_builder_count;

	// ------ evenly distrbute hit points ---------//

	if( !under_construction )
	{
		err_when( place_max_hit_points == 0.0f );
		float ratio = hit_points / place_max_hit_points;

		// recalc max_hit_points instead of minus unitPtr->hit_points
		place_max_hit_points = 0;
		for(int i = 0; i < extra_builder_count; ++i )
		{
			place_max_hit_points += unit_array[extra_builder_recno[i]]->max_hit_points() * max_hit_point_ratio;
		}
		hit_points = ratio * place_max_hit_points;
	}
	else
	{
		// before construction complete
		// firm max hit points base on firmInfo
	}
}
// ----- end of function FirmMonsterFortress::kill_extra_builder -----//



// ----- begin of function FirmMonsterFortress::assign_unit -----//
//
void FirmMonsterFortress::assign_unit(int unitRecno)
{
	int unitId = unit_array[unitRecno]->unit_id;
	if( unitId == support_unit_id && extra_builder_count < MAX_EXTRA_BUILDER )
	{
		assign_extra_builder(unitRecno);
	}
	else
	{
		assign_soldier(unitRecno);
	}
}
// ----- end of function FirmMonsterFortress::assign_unit -----//


// ----- begin of function FirmMonsterFortress::is_soldier_full -----//
//
int FirmMonsterFortress::is_soldier_full()
{
	return archer_count >= extra_builder_count;
}
// ----- end of function FirmMonsterFortress::is_soldier_full -----//


// ----- begin of function FirmMonsterFortress::assign_soldier -----//
//
void FirmMonsterFortress::assign_soldier(int unitRecno)
{
	if( unit_array.is_deleted(unitRecno) )
		return;

	//--- if the new archer's nation is not the same as the firm's nation, don't assign ---//

	if( unitRecno && unit_array[unitRecno]->nation_recno != nation_recno )
		return;

	// ----- if enough space for extra builder -------//

	if( is_soldier_full() )
		return;

	//------------------------------------------//

	Unit *unitPtr = unit_array[unitRecno];

	//----- reset the new builder's team info and home camp ------//

	unitPtr->home_camp_firm_recno 	= 0;

	int originalXLoc = unitPtr->next_x_loc();
	int originalYLoc = unitPtr->next_y_loc();

	err_when( unitPtr->hit_points <= 0 );

	unitPtr->deinit_sprite();

	// -------- set unit mode --------//

	unitPtr->set_mode(UNIT_MODE_OVERSEE, firm_recno);

	//--------- if the unit is a spy -----------//

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place( SPY_FIRM, firm_recno );

	//------- update loyalty -------//

	unitPtr->update_loyalty();

	// ------- reset cur_attack and remain_attack_delay --------//

	unitPtr->cur_attack = 0;
	unitPtr->remain_attack_delay = 0;

	// ------ put unit recno in the array ------//

	archer_unit_recno[archer_count] = unitRecno;
	archer_count++;
}
// ----- end of function FirmMonsterFortress::assign_soldier -----//


// ----- begin of function FirmMonsterFortress::mobilize_soldier -----//
//
int FirmMonsterFortress::mobilize_soldier(int soldierId, char remoteAction)
{
	if( soldierId <= 0 || soldierId > archer_count )
		return 0;

	short unitRecno = archer_unit_recno[soldierId-1];
	Unit* unitPtr = unit_array[unitRecno];

	//-------- if the soldier is a spy -------//

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place(SPY_MOBILE, unitRecno);

	//----- get this archer out of the firm -----//

	int xLoc, yLoc;
	int spaceFound = 0;

	for( int r = 0; r <= 6 && !spaceFound; r += unitPtr->loc_width )
	{
		xLoc = loc_x1 - r;
		yLoc = loc_y1 - r;

		// int spaceFound = locate_space(is_being_deleted, xLoc, yLoc, loc_x2+2, loc_y2+2, spriteInfo->loc_width, spriteInfo->loc_height);
		spaceFound = world.locate_space(xLoc, yLoc, loc_x2+r, loc_y2+r,
			unitPtr->loc_width, unitPtr->loc_height, unitPtr->mobile_type );
	}

	if(spaceFound)
	{
		unitPtr->init_sprite(xLoc, yLoc);
		unitPtr->set_mode(0);        // reset overseen firm recno
	}
	else
	{
		unit_array.del(unitRecno);		// delete it when there is no space for the unit
	}

	//--------- del builder recno from the builder array -------------//

	m.del_array_rec( archer_unit_recno, MAX_FORTRESS_SOLDIER, sizeof(archer_unit_recno[0]), soldierId );
	--archer_count;

	//------- update loyalty -------//

	if( !unit_array.is_deleted(unitRecno) )
	{
		unitPtr->update_loyalty();
		unitPtr->home_camp_firm_recno = firm_recno;
		unitPtr->cur_attack = 0;
		unitPtr->remain_attack_delay = 0;
	}

	return unitRecno;
}
// ----- end of function FirmMonsterFortress::mobilize_soldier -----//


// ----- begin of function FirmMonsterFortress::patrol -----//
//
void FirmMonsterFortress::patrol()
{
	int rc = 0;
	for( int i = archer_count ; i > 0; --i )
	{
		int unitRecno = mobilize_soldier(i, COMMAND_AUTO );
		if( unitRecno )
		{
			if( !rc || unitRecno < rc )
				rc = unitRecno;

			// set mobilized unit selected
			if( nation_recno == nation_array.player_recno )
			{
				if( !unit_array.selected_recno || unitRecno < unit_array.selected_recno )
					unit_array.selected_recno = unitRecno;

				unit_array[unitRecno]->select();
			}
		}
	}

	if( rc && nation_recno == nation_array.player_recno
		&& firm_array.selected_recno == firm_recno )
	{
		firm_array.selected_recno = 0;
		info.disp();
	}
}
// ----- end of function FirmMonsterFortress::patrol -----//


// ----- begin of function FirmMonsterFortress::kill_all_soldier -----//
//
void FirmMonsterFortress::kill_all_soldier()
{
	for(int i = archer_count; i > 0; --i )
	{
		kill_soldier(i);
	}
}
// ----- end of function FirmMonsterFortress::kill_all_soldier -----//


// ----- begin of function FirmMonsterFortress::kill_soldier -----//
//
void FirmMonsterFortress::kill_soldier(int soldierId)
{
	if( soldierId <= 0 || soldierId > archer_count )
		return;

	short unitRecno = archer_unit_recno[soldierId-1];

	// ####### begin Gilbert 24/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(unit_array[unitRecno]->unique_id);		// check unit killed
	// ####### end Gilbert 24/2 ########//

	unit_array.del(unitRecno);		// delete it when there is no space for the unit

	//--------- del builder recno from the builder array -------------//

	m.del_array_rec( archer_unit_recno, MAX_FORTRESS_SOLDIER, sizeof(archer_unit_recno[0]), soldierId );
	--archer_count;
}
// ----- end of function FirmMonsterFortress::kill_soldier -----//


// ----- begin of function FirmMonsterFortress::free_all_people -----//
//
void FirmMonsterFortress::free_all_people()
{
	Firm::free_all_people();

	if( !sys.signal_exit_flag && hit_points > 0.0f )
	{
		for( int i = extra_builder_count; i > 0; --i )
		{
			if( !mobilize_extra_builder(i) )
				kill_extra_builder(i);
		}

		for( i = archer_count; i > 0; --i )
		{
			if( !mobilize_soldier(i, COMMAND_AUTO) )
				kill_soldier(i);
		}
	}
	else
	{
		for( int i = extra_builder_count; i > 0; --i )
		{
			kill_extra_builder(i);
		}

		for( i = archer_count; i > 0; --i )
		{
			kill_soldier(i);
		}
	}
}
// ----- end of function FirmMonsterFortress::free_all_people -----//


// ----- begin of function FirmMonsterFortress::kill_unit -------//
//
// kill unit by unit recno who may be an extra builder or an archer
//
int FirmMonsterFortress::kill_unit(int unitRecno)
{
	for( char i = 0; i < archer_count && unitRecno != archer_unit_recno[i]; ++i );
	if( i < archer_count && unitRecno == archer_unit_recno[i] )
	{
		kill_soldier(i+1);
		return 1;
	}
	
	for( i = 0; i < extra_builder_count && unitRecno != extra_builder_recno[i]; ++i );
	if( i < extra_builder_count && unitRecno == extra_builder_recno[i] )
	{
		kill_extra_builder(i+1);
		return 1;
	}

	return 0;
}
// ----- end of function FirmMonsterFortress::kill_unit -------//


// ----- begin of function FirmMonsterFortress::can_accept_assign -----//
//
bool FirmMonsterFortress::can_accept_assign(int unitRecno, int actionNationRecno )
{
	if( under_construction )
		return false;

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->nation_recno != nation_recno )
		return false;

//	if( unitPtr->is_civilian() )
//		return false;

	UnitInfo* unitInfo = unit_res[unitPtr->unit_id];

	// ######### begin Gilbert 25/3 #######//
	// if( unitInfo->unit_class == UNIT_CLASS_HUMAN )	// it must not be a human
	if( !unitInfo->class_info.monster_side					// monster side or human weapon
		&& unitInfo->unit_class != UNIT_CLASS_WEAPON )
	{
		return false;
	}
	// ######### end Gilbert 25/3 #######//

	return true;
}
// ----- end of function FirmMonsterFortress::can_accept_assign -----//


//------- Begin of function FirmMonsterFortress::change_nation ---------//
//
void FirmMonsterFortress::change_nation(int newNationRecno)
{
	Unit* unitPtr;

	//--- change the nation recno of the builders ---//

	for( int i=0 ; i<extra_builder_count ; i++ )
	{
		unitPtr = unit_array[ extra_builder_recno[i] ];

		err_when( unitPtr->unit_mode != UNIT_MODE_OVERSEE );

		unitPtr->unit_mode = 0;		// reset it first so unitPtr->change_nation() will not attempt to call Firm::change_nation() in a recusive way
		unitPtr->change_nation( newNationRecno );
		unitPtr->unit_mode = UNIT_MODE_OVERSEE;
	}

	//--- change the nation recno of the archers ---//

	for( i=0 ; i<archer_count ; i++ )
	{
		unitPtr = unit_array[ archer_unit_recno[i] ];

		err_when( unitPtr->unit_mode != UNIT_MODE_OVERSEE );

		unitPtr->unit_mode = 0;		// reset it first so unitPtr->change_nation() will not attempt to call Firm::change_nation() in a recusive way
		unitPtr->change_nation( newNationRecno );
		unitPtr->unit_mode = UNIT_MODE_OVERSEE;
	}

	//-----------------------------------------------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmMonsterFortress::change_nation ---------//


//-------- Begin of function FirmMonsterFortress::tranform_from_units ---------//
//
/*
void FirmMonsterFortress::tranform_from_units(int unitRecno, char remoteAction)
{
	if( unit_array.is_deleted(unitRecno) )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// NEWCODEHERE
		err_here();
		return;
	}

	short builderUnit[MAX_EXTRA_BUILDER];
	builderUnit[0] = unitRecno;

	// find other extra builder

	Unit *unitPtr = unit_array[unitRecno];
	int firmXLoc = -1;
	int firmYLoc;

	// top left corner

	if( firmXLoc < 0 && unitPtr->obj_loc_x1() > 0 && unitPtr->obj_loc_y1() > 0 )
	{
		// find unit on top, top left and left
		short& topUnit = builderUnit[1];
		short& leftUnit = builderUnit[2];
		short& topLeftUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x1(), unitPtr->obj_loc_y1()-1 );
		topUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y1() );
		leftUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y1()-1 );
		topLeftUnit = locPtr->unit_recno( UNIT_LAND );

		if( topUnit && leftUnit && topLeftUnit 

			// same unit id
			&& unit_array[topUnit]->unit_id == unitPtr->unit_id
			&& unit_array[leftUnit]->unit_id == unitPtr->unit_id
			&& unit_array[topLeftUnit]->unit_id == unitPtr->unit_id

			// same nation
			&& unit_array[topUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[leftUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[topLeftUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != topUnit && unitRecno != leftUnit && unitRecno != topLeftUnit
			&& topUnit != leftUnit && topUnit != topLeftUnit && leftUnit != topLeftUnit )
		{
			firmXLoc = unit_array[topLeftUnit]->obj_loc_x1();
			firmYLoc = unit_array[topLeftUnit]->obj_loc_y1();
		}
	}

	// top right corner

	if( firmXLoc < 0 && unitPtr->obj_loc_x2() < MAX_WORLD_X_LOC-1 && unitPtr->obj_loc_y1() > 0 )
	{
		// find unit on top, top left and left
		short& topUnit = builderUnit[1];
		short& rightUnit = builderUnit[2];
		short& topRightUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x2(), unitPtr->obj_loc_y1()-1 );
		topUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y1() );
		rightUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y1()-1 );
		topRightUnit = locPtr->unit_recno( UNIT_LAND );

		if( topUnit && rightUnit && topRightUnit 

			// same unit id
			&& unit_array[topUnit]->unit_id == unitPtr->unit_id
			&& unit_array[rightUnit]->unit_id == unitPtr->unit_id
			&& unit_array[topRightUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[topUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[rightUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[topRightUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != topUnit && unitRecno != rightUnit && unitRecno != topRightUnit
			&& topUnit != rightUnit && topUnit != topRightUnit && rightUnit != topRightUnit )
		{
			firmXLoc = unit_array[topUnit]->obj_loc_x1();
			firmYLoc = unit_array[topUnit]->obj_loc_y1();
		}
	}

	// bottom left corner

	if( firmXLoc < 0 && unitPtr->obj_loc_x1() > 0 && unitPtr->obj_loc_y2() < MAX_WORLD_Y_LOC-1 )
	{
		// find unit on bottom, bottom left and left
		short& bottomUnit = builderUnit[1];
		short& leftUnit = builderUnit[2];
		short& bottomLeftUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x1(), unitPtr->obj_loc_y2()+1 );
		bottomUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y2() );
		leftUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y2()+1 );
		bottomLeftUnit = locPtr->unit_recno( UNIT_LAND );

		if( bottomUnit && leftUnit && bottomLeftUnit 

			// same unit id
			&& unit_array[bottomUnit]->unit_id == unitPtr->unit_id
			&& unit_array[leftUnit]->unit_id == unitPtr->unit_id
			&& unit_array[bottomLeftUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[bottomUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[leftUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[bottomLeftUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != bottomUnit && unitRecno != leftUnit && unitRecno != bottomLeftUnit
			&& bottomUnit != leftUnit && bottomUnit != bottomLeftUnit && leftUnit != bottomLeftUnit )
		{
			firmXLoc = unit_array[leftUnit]->obj_loc_x1();
			firmYLoc = unit_array[leftUnit]->obj_loc_y1();
		}
	}


	// bottom right corner

	if( firmXLoc < 0 && unitPtr->obj_loc_x2() < MAX_WORLD_X_LOC-1 && unitPtr->obj_loc_y2() < MAX_WORLD_Y_LOC-1 )
	{
		// find unit on bottom, bottom right and right
		short& bottomUnit = builderUnit[1];
		short& rightUnit = builderUnit[2];
		short& bottomRightUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x2(), unitPtr->obj_loc_y2()+1 );
		bottomUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y2() );
		rightUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y2()+1 );
		bottomRightUnit = locPtr->unit_recno( UNIT_LAND );

		if( bottomUnit && rightUnit && bottomRightUnit 

			// same unit id
			&& unit_array[bottomUnit]->unit_id == unitPtr->unit_id
			&& unit_array[rightUnit]->unit_id == unitPtr->unit_id
			&& unit_array[bottomRightUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[bottomUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[rightUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[bottomRightUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != bottomUnit && unitRecno != rightUnit && unitRecno != bottomRightUnit
			&& bottomUnit != rightUnit && bottomUnit != bottomRightUnit && rightUnit != bottomRightUnit )
		{
			firmXLoc = unit_array[unitRecno]->obj_loc_x1();
			firmYLoc = unit_array[unitRecno]->obj_loc_y1();
		}
	}

	// has find a place to build firm

	if( firmXLoc >= 0 )
	{
		int selectedFlag = 0;
		int i;
		for(i = 0; i < MAX_EXTRA_BUILDER; ++i )
		{
			if( unit_array.selected_recno == builderUnit[i] )		// check selected_recno instead of selected_flag
				selectedFlag = 1;			// because if the display interface is any of the units, display the firm interface
			unit_array[builderUnit[i]]->deinit_sprite(0);	 	// 0-if the unit is currently selected, deactivate it.
		}

		char *buildCode;

		if( unitPtr->race_id )
		{
			if( unitPtr->is_monster() )
				buildCode = monster_res[unitPtr->monster_id()]->firm_build_code;
			else
				buildCode = race_res[unitPtr->race_id]->code;
		}
		else
			err_here();

		int firmRecno = firm_array.build_firm(firmXLoc, firmYLoc, unitPtr->nation_recno,
			FIRM_FORTRESS, buildCode, unitRecno );

		if( firmRecno )
		{
			// first builder should be in the firm now
			err_when( unitPtr->is_visible() );

			Firm *firmPtr = firm_array[firmRecno];

			for( i = 1; i < MAX_EXTRA_BUILDER; ++i )
				firmPtr->assign_unit(builderUnit[i]);
		}
		else
		{
//			err_here();

			// init_sprite
			for( i = 0; i < MAX_EXTRA_BUILDER; ++i )
			{
				unit_array[builderUnit[i]]->init_sprite(
					unit_array[builderUnit[i]]->next_x_loc(), unit_array[builderUnit[i]]->next_y_loc() );
			}
		}

		if( selectedFlag )
		{
			power.reset_selection();
			firm_array.selected_recno = firmRecno;
//			info.disp();
		}
	}
}
//-------- End of function FirmMonsterFortress::tranform_from_units ---------//
*/

//-------- Begin function FirmMonsterFortress::check_transform_from_units------//
bool FirmMonsterFortress::check_transform_from_units(int unitRecno, int& firmXLoc, int& firmYLoc, short* builderUnit)
{
	if( unit_array.is_deleted(unitRecno) )
		return false;

//	short builderUnit[MAX_EXTRA_BUILDER];
	builderUnit[0] = unitRecno;

	// find other extra builder

	Unit *unitPtr = unit_array[unitRecno];
//	int firmXLoc = -1;
//	int firmYLoc;
	bool check = false;
	// top left corner

	if( !check && unitPtr->obj_loc_x1() > 0 && unitPtr->obj_loc_y1() > 0 )
	{
		// find unit on top, top left and left
		short& topUnit = builderUnit[1];
		short& leftUnit = builderUnit[2];
		short& topLeftUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x1(), unitPtr->obj_loc_y1()-1 );
		topUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y1() );
		leftUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y1()-1 );
		topLeftUnit = locPtr->unit_recno( UNIT_LAND );

		if( topUnit && leftUnit && topLeftUnit 

			// same unit id
			&& unit_array[topUnit]->unit_id == unitPtr->unit_id
			&& unit_array[leftUnit]->unit_id == unitPtr->unit_id
			&& unit_array[topLeftUnit]->unit_id == unitPtr->unit_id

			// same nation
			&& unit_array[topUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[leftUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[topLeftUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != topUnit && unitRecno != leftUnit && unitRecno != topLeftUnit
			&& topUnit != leftUnit && topUnit != topLeftUnit && leftUnit != topLeftUnit )
		{
			firmXLoc = unit_array[topLeftUnit]->obj_loc_x1();
			firmYLoc = unit_array[topLeftUnit]->obj_loc_y1();
			check = true;
			return true;
		}
	}

	// top right corner

	if( !check && unitPtr->obj_loc_x2() < MAX_WORLD_X_LOC-1 && unitPtr->obj_loc_y1() > 0 )
	{
		// find unit on top, top left and left
		short& topUnit = builderUnit[1];
		short& rightUnit = builderUnit[2];
		short& topRightUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x2(), unitPtr->obj_loc_y1()-1 );
		topUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y1() );
		rightUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y1()-1 );
		topRightUnit = locPtr->unit_recno( UNIT_LAND );

		if( topUnit && rightUnit && topRightUnit 

			// same unit id
			&& unit_array[topUnit]->unit_id == unitPtr->unit_id
			&& unit_array[rightUnit]->unit_id == unitPtr->unit_id
			&& unit_array[topRightUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[topUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[rightUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[topRightUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != topUnit && unitRecno != rightUnit && unitRecno != topRightUnit
			&& topUnit != rightUnit && topUnit != topRightUnit && rightUnit != topRightUnit )
		{
			firmXLoc = unit_array[topUnit]->obj_loc_x1();
			firmYLoc = unit_array[topUnit]->obj_loc_y1();
			check = true;
			return true;
		}
	}

	// bottom left corner

	if( !check  && unitPtr->obj_loc_x1() > 0 && unitPtr->obj_loc_y2() < MAX_WORLD_Y_LOC-1 )
	{
		// find unit on bottom, bottom left and left
		short& bottomUnit = builderUnit[1];
		short& leftUnit = builderUnit[2];
		short& bottomLeftUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x1(), unitPtr->obj_loc_y2()+1 );
		bottomUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y2() );
		leftUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x1()-1, unitPtr->obj_loc_y2()+1 );
		bottomLeftUnit = locPtr->unit_recno( UNIT_LAND );

		if( bottomUnit && leftUnit && bottomLeftUnit 

			// same unit id
			&& unit_array[bottomUnit]->unit_id == unitPtr->unit_id
			&& unit_array[leftUnit]->unit_id == unitPtr->unit_id
			&& unit_array[bottomLeftUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[bottomUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[leftUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[bottomLeftUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != bottomUnit && unitRecno != leftUnit && unitRecno != bottomLeftUnit
			&& bottomUnit != leftUnit && bottomUnit != bottomLeftUnit && leftUnit != bottomLeftUnit )
		{
			firmXLoc = unit_array[leftUnit]->obj_loc_x1();
			firmYLoc = unit_array[leftUnit]->obj_loc_y1();
			check = true;
			return true;
		}
	}


	// bottom right corner

	if( !check && unitPtr->obj_loc_x2() < MAX_WORLD_X_LOC-1 && unitPtr->obj_loc_y2() < MAX_WORLD_Y_LOC-1 )
	{
		// find unit on bottom, bottom right and right
		short& bottomUnit = builderUnit[1];
		short& rightUnit = builderUnit[2];
		short& bottomRightUnit = builderUnit[3];

		Location *locPtr = world.get_loc( unitPtr->obj_loc_x2(), unitPtr->obj_loc_y2()+1 );
		bottomUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y2() );
		rightUnit = locPtr->unit_recno( UNIT_LAND );

		locPtr = world.get_loc( unitPtr->obj_loc_x2()+1, unitPtr->obj_loc_y2()+1 );
		bottomRightUnit = locPtr->unit_recno( UNIT_LAND );

		if( bottomUnit && rightUnit && bottomRightUnit 

			// same unit id
			&& unit_array[bottomUnit]->unit_id == unitPtr->unit_id
			&& unit_array[rightUnit]->unit_id == unitPtr->unit_id
			&& unit_array[bottomRightUnit]->unit_id == unitPtr->unit_id
			
			// same nation
			&& unit_array[bottomUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[rightUnit]->nation_recno == unitPtr->nation_recno
			&& unit_array[bottomRightUnit]->nation_recno == unitPtr->nation_recno

			// no duplicated units
			&& unitRecno != bottomUnit && unitRecno != rightUnit && unitRecno != bottomRightUnit
			&& bottomUnit != rightUnit && bottomUnit != bottomRightUnit && rightUnit != bottomRightUnit )
		{
			firmXLoc = unit_array[unitRecno]->obj_loc_x1();
			firmYLoc = unit_array[unitRecno]->obj_loc_y1();
			check = true;
			return true;
		}
	}
	return false;
}
//-------- End function FirmMonsterFortress::check_transform_from_units--------//

//-------- Begin function FirmMonsterFortress::transform_from_units------------//
void FirmMonsterFortress::transform_from_units(int unitRecno, int firmXLoc, int firmYLoc, short* builderUnit, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <unit recno> <firmXLoc> <firmYLoc> < 4 x unit recnos>
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNITS_TRANSFORM_MFORT, (3+MAX_EXTRA_BUILDER)*sizeof(short));
		shortPtr[0] = unitRecno;
		shortPtr[1] = firmXLoc;
		shortPtr[2] = firmYLoc;
		memcpy( shortPtr+3, builderUnit, MAX_EXTRA_BUILDER * sizeof(short) );
		return;
	}
	
	Unit *unitPtr = unit_array[unitRecno];

	// ###### begin Gilbert 23/3 #########//
	// ------ check enough cash and live point -----//
	// do not check in check_transform_from_units because
	// units may keep trying to build a fortress
	if( nation_array[unitPtr->nation_recno]->cash < firm_res[FIRM_FORTRESS]->setup_cost 
		|| nation_array[unitPtr->nation_recno]->live_points < firm_res[FIRM_FORTRESS]->setup_live_points_cost )
	{
		return;
	}
	// ###### end Gilbert 23/3 #########//


	int selectedFlag = 0;
	int i;
	for(i = 0; i < MAX_EXTRA_BUILDER; ++i )
	{
		if( unit_array.selected_recno == builderUnit[i] )		// check selected_recno instead of selected_flag
			selectedFlag = 1;			// because if the display interface is any of the units, display the firm interface
		unit_array[builderUnit[i]]->deinit_sprite(0);	 	// 0-if the unit is currently selected, deactivate it.
	}

	char *buildCode;

	if( unitPtr->race_id )
	{
		if( unitPtr->is_monster() )
			buildCode = monster_res[unitPtr->monster_id()]->firm_build_code;
		else
			buildCode = race_res[unitPtr->race_id]->code;
	}
	else
		err_here();

	int firmRecno = firm_array.build_firm(firmXLoc, firmYLoc, unitPtr->nation_recno,
		FIRM_FORTRESS, buildCode, unitRecno );

	if( firmRecno )
	{
		// first builder should be in the firm now
		err_when( unitPtr->is_visible() );

		Firm *firmPtr = firm_array[firmRecno];

		for( i = 1; i < MAX_EXTRA_BUILDER; ++i )
			firmPtr->assign_unit(builderUnit[i]);
	}
	else
	{
//			err_here();

		// init_sprite
		for( i = 0; i < MAX_EXTRA_BUILDER; ++i )
		{
			unit_array[builderUnit[i]]->init_sprite(
				unit_array[builderUnit[i]]->next_x_loc(), unit_array[builderUnit[i]]->next_y_loc() );
		}
	}

	if( selectedFlag )
	{
		power.reset_selection();
		firm_array.selected_recno = firmRecno;
//			info.disp();
	}

}
//-------- End function FirmMonsterFortress::transform_from_units--------------//


// ------- begin of function FirmMonsterFortress::can_build_fortress ----------//
//
// if builderArray is NULL, used by local player only (see selected_flag of unit)
//
bool FirmMonsterFortress::can_build_fortress( int unitRecno, int xLoc1, int yLoc1, short* builderArray )
{
	err_when( !builderArray && !unit_array[unitRecno]->is_own() );	// if builderArray is NULL, only for local player, because this func is going to read selected_flag

	FirmInfo *firmInfo = firm_res[FIRM_FORTRESS];

	// if any same nation unit inside build area, remove the unit_recno in location

	if( xLoc1 < 0 || yLoc1 < 0 || xLoc1 > MAX_WORLD_X_LOC || yLoc1 > MAX_WORLD_Y_LOC )	
		return 0;		

	//--- if the unit's region is different from the destination's region, do nothing ---//

	int destRegionId = world.get_loc(xLoc1, yLoc1)->region_id;
	int i;
	if( unitRecno > 0 )
	{
		Unit* unitPtr = unit_array[unitRecno];

		if( unitPtr->unit_id != UNIT_ROCK
			|| unitPtr->is_visible() && unitPtr->region_id() != destRegionId )
		{
			return 0;
		}
	}

	if( builderArray )
	{
		for( i = 0; i < MAX_EXTRA_BUILDER; ++i )
		{
			Unit* unitPtr = unit_array[builderArray[i]];

			if( unitPtr->unit_id != UNIT_ROCK
				|| unitPtr->is_visible() && unitPtr->region_id() != destRegionId )
			{
				return 0;
			}
		}
	}

	//------------------------------------------//

	int xLoc, yLoc;
	int xLoc2 = xLoc1 + firmInfo->loc_width - 1;
	int yLoc2 = yLoc1 + firmInfo->loc_height - 1;
	if(xLoc2>=MAX_WORLD_X_LOC || yLoc2>=MAX_WORLD_Y_LOC)
		return 0;

	int xLoc3, yLoc3, xLoc4, yLoc4;
	xLoc3 = max( xLoc1-INTER_PLACE_SPACE, 0 );
	yLoc3 = max( yLoc1-INTER_PLACE_SPACE, 0 );
	xLoc4 = min( xLoc2+INTER_PLACE_SPACE, MAX_WORLD_X_LOC-1 );
	yLoc4 = min( yLoc2+INTER_PLACE_SPACE, MAX_WORLD_Y_LOC-1 );

	//-----------------------------------------//

	Location* locPtr;
	char teraMask;

	switch(firmInfo->tera_type)
	{
	case 1:		// default : land firm
	case 2:		// sea firm
	case 3:		// land or sea firm
		teraMask = firmInfo->tera_type;
		for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
		{
			locPtr = world.get_loc((xLoc=xLoc1), yLoc);

			for( ; xLoc<=xLoc2 ; xLoc++, locPtr++ )
			{
				// allow the building unit to stand in the area
				if( !locPtr->can_build_firm(teraMask))
				{
					int locUnitRecno;
					if( !(locUnitRecno = locPtr->unit_recno(UNIT_LAND)) )
						return 0;

					int isBuilderFlag = 0;
					if( locUnitRecno == unitRecno )
						isBuilderFlag = 1;
					else if( builderArray )
					{
						for( i = 0; i < MAX_EXTRA_BUILDER; ++i )
						{
							if( builderArray[i] == locUnitRecno )
							{
								isBuilderFlag = 1;
								break;
							}
						}
					}
					else		// if it selected and unit id is grokken
					{
						isBuilderFlag = unit_array[locUnitRecno]->selected_flag 
							&& unit_array[locUnitRecno]->unit_id == UNIT_ROCK;
					}

					if( !isBuilderFlag )
						return 0;
				}

				// don't allow building on a site except a mine on a raw site
				// if( locPtr->has_site() && !((firmId == FIRM_MINE || firmId == FIRM_ALCHEMY) && site_array[locPtr->site_recno()]->site_type == SITE_RAW) )
				//	return 0;
			}
		}

		// scan two space wider 

		for( yLoc=yLoc3 ; yLoc<=yLoc4 ; yLoc++ )
		{
			locPtr = world.get_loc((xLoc=xLoc3), yLoc);
			for( ; xLoc<=xLoc4 ; xLoc++, locPtr++ )
			{
				if( !locPtr->walkable(teraMask) )//&& !locPtr->can_clear_obstacle() )
					return 0;

				// don't allow building near a raw site

				if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW 
					&& (xLoc < xLoc1 || xLoc > xLoc2 || yLoc < yLoc1 || yLoc > yLoc2) )
					return 0;

			}
		}
		break;

	default:
		err_here();
	}

	return 1;
}
// ------- end of function FirmMonsterFortress::can_build_fortress ----------//

//-------- Begin of function FirmMonsterFortress::is_visible -------//
int FirmMonsterFortress::is_visible(int nationRecno)
{
	if( nation_recno != nationRecno && rock_id )
		return 0;
	
	return 1;			
}
//-------- End of function FirmMonsterFortress::is_visible -------//


// -------- begin of function FirmCamp::swap_item --------//
//
int FirmMonsterFortress::swap_item( int fromSoldierId, int toSoldierId, int verifyItemId, char remoteAction )
{
	if( fromSoldierId == toSoldierId )
		return 0;
	err_when( fromSoldierId <= 0 );
	if( fromSoldierId > archer_count )
		return 0;
	err_when( toSoldierId <= 0 );
	if( toSoldierId > archer_count )
		return 0;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno>, <from soldier Id>, <to Soldier Id>, <verify item id>
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_F_CAMP_SWAP_ITEM, 4*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = fromSoldierId;
		shortPtr[2] = toSoldierId;
		shortPtr[3] = verifyItemId;
		return 0;
	}

	Item &srcItem = unit_array[archer_unit_recno[fromSoldierId-1]]->item;
	Item &destItem = unit_array[archer_unit_recno[toSoldierId-1]]->item;

	// verify item id

	if( srcItem.id != verifyItemId )
		return 0;

	// check if item is suitable for the other side

	if( srcItem.id && !item_res.can_pick(srcItem.id, archer_unit_recno[toSoldierId-1]) )
		return 0;
	if( destItem.id && !item_res.can_pick(destItem.id, archer_unit_recno[fromSoldierId-1]) )
		return 0;

	// swap srcItem and destItem
	Item tempItem = destItem;
	destItem = srcItem;
	srcItem = tempItem;

	return 1;
}
// -------- end of function FirmCamp::swap_item --------//
