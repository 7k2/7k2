//Filename    : OF_CAMPU.CPP
//Description : Firm Military Camp2 - unit assign functions

#include <OF_CAMP.H>
#include <OSYS.H>
#include <OPOWER.H>
#include <OUN_GRP.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <ONATION.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OUNITRES.H> 
#include <OREMOTE.H>
#include <OGAME.H>
#include <OITEMRES.H>


//--------- Begin of function FirmCamp::assign_unit ---------//
//
void FirmCamp::assign_unit(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-------- assign the unit ----------//

	int rankId = unit_array[unitRecno]->rank_id;

	if( rankId == RANK_GENERAL || rankId==RANK_KING )
	{
		assign_overseer(unitRecno);
	}
	else
	{
		assign_soldier(unitRecno);
	}
}
//----------- End of function FirmCamp::assign_unit -----------//


//--------- Begin of function FirmCamp::assign_overseer --------//
//
// Assign an unit as the overseer of this firm
//
// <int> newOverseerRecno - recno of the new overseer unit.
//									 0 means resign the current overseer
//
// Note: If a new overseer is assigned to the firm, there should be
//			space for the old overseer to initialize and appear in the
//			map (the space the new overseer occupied).
//
//			** If the newOverseerRecno==0, there may be no space for
//			creating the old overseer.  Then, the old overseer will be
//			deleted. **
//			** else there must be space for creating the old overseer,
//			at least the space occupied by the new overseer **
//
void FirmCamp::assign_overseer(int newOverseerRecno)
{
	if( !newOverseerRecno && !overseer_recno )
		return;

	//--- if the new overseer's nation is not the same as the firm's nation, don't assign ---//

	if( newOverseerRecno && unit_array[newOverseerRecno]->nation_recno != nation_recno )
		return;

	//----- reset the new overseer's team info and home camp ------//

	if( newOverseerRecno )
	{
		Unit* unitPtr = unit_array[newOverseerRecno];

		err_when( unitPtr->skill_level()==0 );

		err_when( !unitPtr->race_id );
		err_when( unitPtr->rank_id != RANK_GENERAL && unitPtr->rank_id != RANK_KING );
		err_when( !unitPtr->team_info );

		unitPtr->home_camp_firm_recno 	= 0;
	}

	//------------------------------------------//

	int oldOverseerRecno = overseer_recno;

	if(!newOverseerRecno)
	{
		//------------------------------------------------------------------------------------------------//
		// the old overseer may be kept in firm or killed if remove_firm is true
		//------------------------------------------------------------------------------------------------//
		err_when(!overseer_recno);
		Unit *oldUnitPtr = unit_array[overseer_recno];
		SpriteInfo *spriteInfo = sprite_res[unit_res[oldUnitPtr->unit_id]->sprite_id];
		int xLoc = loc_x1;
		int yLoc = loc_y1;

		if(!locate_space(is_being_deleted, xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height))
		{
			if(is_being_deleted)
				kill_overseer();
		}
		else
		{
			//------ there should be space for creating the overseer -----//

			mobilize_overseer();
		}
	}
	else
	{
		//----------- there should be space for creating the overseer ---------//
		err_when(!newOverseerRecno);

		Unit *unitPtr = unit_array[newOverseerRecno];

		int originalXLoc = unitPtr->next_x_loc();
		int originalYLoc = unitPtr->next_y_loc();

		err_when( unitPtr->hit_points <= 0 );

		unitPtr->deinit_sprite();

      //----------------------------------------------------------------------------------------//
		// There should be at least one location (occupied by the new overseer) for creating the old
		//	overseer.
		//
		// 1) If a town is already created, the new overseer settle down there, free its space for
		//		creating the new overseer.
		// 2) If the overseer and the soldiers live in the firm, no town will be created.  Thus, the
		//		space occupied by the old overseer is free for creating the new overseer.
		// 3) If the overseer and the soldiers need live in town, and a town is created.  i.e. there
		//		is no overseer or soldier in the firm, so just assign the new overseer in the firm
		//----------------------------------------------------------------------------------------//
		
		if(!overseer_recno && !soldier_count)
		{
			//------- set the unit to overseer mode and deinit the sprite ------//

			overseer_recno = newOverseerRecno;
			Unit *unitPtr = unit_array[overseer_recno];
			unitPtr->set_mode(UNIT_MODE_OVERSEE, firm_recno);
			unitPtr->deinit_sprite();     // hide the unit from the world map

			//--------- if the unit is a spy -----------//

			if( unitPtr->spy_recno )
				spy_array[unitPtr->spy_recno]->set_place( SPY_FIRM, firm_recno );
		}
		else
		{
			//------------------------------------------------------------------------------------------------//
			// a town should exist if the overseer need live in town
			//------------------------------------------------------------------------------------------------//

			Unit *unitPtr = unit_array[newOverseerRecno];
			unitPtr->deinit_sprite();

			if(overseer_recno)
				mobilize_overseer();

			overseer_recno = newOverseerRecno;
			unitPtr->set_mode(UNIT_MODE_OVERSEE, firm_recno);

			//--------- if the unit is a spy -----------//

			if( unitPtr->spy_recno )
				spy_array[unitPtr->spy_recno]->set_place( SPY_FIRM, firm_recno );
		}
	}

	//------- update loyalty -------//

	if( newOverseerRecno && !unit_array.is_deleted(newOverseerRecno) )
		unit_array[newOverseerRecno]->update_loyalty();

	//------------- update influence -----------//

	update_influence();

	//----------- refresh display if this firm is selected ----------//

	// ##### begin Gilbert 14/12 #####//
//	if(firm_array.selected_recno == firm_recno)
//		info.disp();
	// ##### end Gilbert 14/12 #####//
}
//----------- End of function FirmCamp::assign_overseer --------//


//--------- Begin of function FirmCamp::assign_soldier --------//
//
// Assign an unit as one of the soldiers of this firm
//
void FirmCamp::assign_soldier(int soldierUnitRecno)
{
	err_when( !soldier_array );    // this function shouldn't be called if this firm does not need soldier

	//-- if the unit is a spy, only allow assign when there is room in the firm --//

	Unit* unitPtr = unit_array[soldierUnitRecno];

	if( unitPtr->true_nation_recno() != nation_recno &&
		 soldier_count == MAX_SOLDIER )
	{
		return;
	}

	//---- if all soldier space are full, resign the worst soldier to release one soldier space for the overseer ----//

	err_when( unitPtr->rank_id == RANK_KING );
	err_when( unitPtr->hit_points <= 0 );

	int unitXLoc= -1, unitYLoc;

	if( soldier_count == MAX_SOLDIER )
	{
		int worstSoldierId=0, minSoldierSkill=0x7FFF, soldierSkill;

		for(int i=0; i<MAX_SOLDIER; i++)
		{
			if( soldier_array[i].is_under_training() )		// don't consider soldiers under training
				continue;

			soldierSkill = soldier_array[i].skill_level();

			if( soldierSkill < minSoldierSkill)
			{
				minSoldierSkill = soldierSkill;
				worstSoldierId = i+1;
			}
		}

		if( !worstSoldierId )		// if every soldier is under training, we cannot resign any
			return;

		err_when(worstSoldierId<1 || worstSoldierId>MAX_SOLDIER);

		unitXLoc = unitPtr->next_x_loc();		// save the location for later init_sprite() if the assign settle action failed
		unitYLoc = unitPtr->next_y_loc();

		unitPtr->deinit_sprite(); // free the location for creating the worst unit

		#ifdef DEBUG
			int oldSoldierCount = soldier_count;
			int resignResult = resign_soldier(worstSoldierId);
			err_when(!resignResult && oldSoldierCount==soldier_count);
		#else
			resign_soldier(worstSoldierId);
		#endif
	}

	// err_when( soldier_count >= MAX_SOLDIER );

	//---------- there is room for the new soldier ------------//

	Soldier* soldierPtr = soldier_array + soldier_count;

	memset( soldierPtr, 0, sizeof(Soldier) );

	soldierPtr->loyalty = unitPtr->loyalty;

	//------- add the soldier to the firm -------//

	soldier_count++;

	err_when( soldier_count > MAX_SOLDIER );

	// ######## begin Gilbert 1/3 ######//
	soldierPtr->name_id	= unitPtr->name_id;
	unitPtr->name_id = 0;			// so Unit::deinit will not call free_name_id
	// ######## end Gilbert 1/3 ######//
	soldierPtr->race_id  = unitPtr->race_id;
	soldierPtr->unit_id	= unitPtr->unit_id;
	soldierPtr->rank_id  = unitPtr->rank_id;
	soldierPtr->is_royal = unitPtr->is_royal;
	soldierPtr->hero_id  = unitPtr->hero_id;
	soldierPtr->unique_id= unitPtr->unique_id;
	// #### begin Gilbert 26/1 #######//
	soldierPtr->source_town_recno = 0;
	// #### end Gilbert 26/1 #######//

	soldierPtr->skill    = unitPtr->skill;

	if( soldierPtr->skill_level() == 0 )
		soldierPtr->skill.set_skill_level(CITIZEN_SKILL_LEVEL);

	err_when( soldierPtr->skill_level()<0 );

	soldierPtr->hit_points   = (int) unitPtr->hit_points;

	err_when( soldierPtr->combat_level() <= 0 );
	err_when( soldierPtr->combat_level() > soldierPtr->skill.max_combat_level );

	err_when( soldierPtr->hit_points < 0 );

	if( soldierPtr->hit_points == 0 )		// 0.? will become 0 in (float) to (int) conversion
		soldierPtr->hit_points = 1;

	// ###### begin Gilbert 24/3 #######//
	// if( unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_WEAPON )
	if( unit_res[soldierPtr->unit_id]->class_info.has_weapon_version )
	// ###### end Gilbert 24/3 #######//
	{
		soldierPtr->extra_para = unitPtr->get_weapon_version();
	}
	else if( unitPtr->race_id )
	{
		soldierPtr->extra_para = unitPtr->cur_power;
	}
	else
	{
		soldierPtr->extra_para = 0;
	}

	soldierPtr->item = unitPtr->item;
	unitPtr->item.clear();

	// soldierPtr->init_potential();		// not for assigned unit

	//------ if the recruited soldier is a spy -----//

	if( unitPtr->spy_recno )
	{
		spy_array[unitPtr->spy_recno]->set_place( SPY_FIRM, firm_recno );

		soldierPtr->spy_recno = unitPtr->spy_recno;
		unitPtr->spy_recno = 0;								// reset it now so Unit::deinit() won't delete the Spy in spy_array
	}

	//--------- the unit disappear in firm -----//

	if( nation_recno )
		unit_res[unitPtr->unit_id]->inc_nation_unit_count(nation_recno);		// disapper_in_firm() will decrease the unit count, which is not necessary, so we must increase it first to offset the later decrease

	unit_array.disappear_in_firm(soldierUnitRecno);

	//--- remove the unit from patrol_unit_array when it returns to the base ---//

	validate_patrol_unit();

	//-------- sort soldiers ---------//

	sort_soldier();
}
//----------- End of function FirmCamp::assign_soldier --------//


//--------- Begin of function FirmCamp::mobilize_overseer --------//
//
int FirmCamp::mobilize_overseer()
{
	if( !overseer_recno )
		return 0;

	//--------- restore overseer's harmony ---------//

	int overseerRecno = overseer_recno;

	Unit* unitPtr = unit_array[overseer_recno];

	//-------- if the overseer is a spy -------//

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place(SPY_MOBILE, unitPtr->sprite_recno);

	//----- get this overseer out of the firm -----//

	int xLoc, yLoc;
	int spaceFound = create_unit_locate_space(unitPtr->unit_id, xLoc, yLoc);

	if(spaceFound)
	{
		unitPtr->init_sprite(xLoc, yLoc);
		unitPtr->set_mode(0);        // reset overseen firm recno
	}
	else
	{
		unit_array.del(overseer_recno);		// delete it when there is no space for the unit
		return 0;
	}

	//--------- reset overseer_recno -------------//

	overseer_recno      = 0;
//	overseer_town_recno = 0;

	//------- update loyalty -------//

	if( overseerRecno && !unit_array.is_deleted(overseerRecno) )
	{
		Unit* overseerUnit = unit_array[overseerRecno];

		overseerUnit->update_loyalty();
		overseerUnit->home_camp_firm_recno = firm_recno;

		err_when( overseerUnit->skill_level()==0 );
	}

	return overseerRecno;
}
//----------- End of function FirmCamp::mobilize_overseer --------//


//--------- Begin of function FirmCamp::mobilize_soldier ---------//
//
// Promote a firm soldier as a unit.
//
// return: <int> the recno of the unit created.
//
int FirmCamp::mobilize_soldier(int soldierId, char remoteAction)
{
	if(!remoteAction && remote.is_enable() )
	{
		// packet strcture : <firm_recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_WORKER, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = soldierId;
		return 0;
	}

	//-------------------------------------------//

	err_when( !soldier_array );    // this function shouldn't be called if this firm does not need soldier

	// ##### begin Gilbert 26/1 #######//
	if( soldierId > soldier_count )			// may happen in multiplayer
		return 0;
	// ##### end Gilbert 26/1 #######//

	err_when( soldierId<1 || soldierId>soldier_count );

	//------- decrease soldier no. and create an unit -----//

	Soldier* soldierPtr = soldier_array+soldierId-1;
	int	  unitRecno = 0;

	// ##### begin Gilbert 26/1 ########//
	// err_when( soldierPtr->is_under_training() );
	// err_when( soldierPtr->skill_level()==0 );
	if( soldierPtr->is_under_training() )
	{
		if( soldierPtr->is_human() )
		{
			// cancel training human
			// change unit id to civilian
			err_when( soldierPtr->hero_id );

			if( nation_recno )
				unit_res[soldierPtr->unit_id]->dec_nation_unit_count(nation_recno);
			soldierPtr->unit_id = race_res[soldierPtr->race_id]->civilian_unit_id;
			if( nation_recno )
				unit_res[soldierPtr->unit_id]->inc_nation_unit_count(nation_recno);

			Skill civilSkill;
			civilSkill.init(soldierPtr->unit_id, CITIZEN_COMBAT_LEVEL, 0);
			soldierPtr->skill = civilSkill;
			soldierPtr->hit_points = soldierPtr->max_hit_points();
		}
		else
		{
			err_here();
		}
	}
	// ##### end Gilbert 26/1 ########//

	Soldier thisSoldier = soldier_array[soldierId-1];

	unitRecno = create_soldier_unit(thisSoldier);	 	// if he is a spy, create_soldier_unit wil call set_place(SPY_MOBILE)

	if(!unitRecno)
		return 0; // return 0 eg there is no space to create the unit

	//------ set the home camp firm recno for later auto return ----//

	// ###### begin Gilbert 26/1 #######//
	if( !soldierPtr->is_under_training() )
		unit_array[unitRecno]->home_camp_firm_recno = firm_recno;
	// ###### end Gilbert 26/1 #######//

	//------- delete the record from the soldier_array ------//

	err_when( soldier_count > MAX_SOLDIER );
	err_when( selected_soldier_id > soldier_count );

	m.del_array_rec(soldier_array, soldier_count, sizeof(Soldier), soldierId);

	if( selected_soldier_id > soldierId || selected_soldier_id == soldier_count )
		selected_soldier_id--;

	soldier_count--;

	err_when( soldier_count < 0 );
	err_when( selected_soldier_id > soldier_count );

	return unitRecno;
}
//----------- End of function FirmCamp::mobilize_soldier -----------//


//--------- Begin of function FirmCamp::resign_all_soldier ---------//
//
// Resign all soldiers in the firm.
//
// [int] disappearFlag - whether the soldier should disappear after
//                       resigning, and does not go back to the town.
//
void FirmCamp::resign_all_soldier(int disappearFlag)
{
	for( int i=soldier_count ; i>0 ; i-- )
	{
		// ###### begin Gilbert 26/1 #####//
		if( !soldier_array[i-1].is_under_training() )
		{
			if( !resign_soldier(i) )		// if the unit cannot be mobilized (can be because it's under training), kill it 
				kill_soldier(i);
		}
		else
		{
			cancel_train_soldier( i, COMMAND_AUTO );
		}
		// ###### end Gilbert 26/1 #####//
	}
}
//----------- End of function FirmCamp::resign_all_soldier -----------//


//--------- Begin of function FirmCamp::resign_soldier ---------//
//
// Resign the soldier from the firm.
//
// return: <int> recno of the mobile unit created if there is one created. 
//
int FirmCamp::resign_soldier(int soldierId)
{
	err_when( !soldier_array );    // this function shouldn't be called if this firm does not need soldier
	err_when( soldierId<1 || soldierId>soldier_count );

	//------- if the soldier is still queue for training, we cannot mobilize it ------//

	Soldier* soldierPtr = soldier_array + soldierId - 1;

	if( soldierPtr->combat_level() == 0 )
		return 0;

	//------- decrease soldier no. and create an unit -----//

	int unitRecno = 0;

	Soldier thisSoldier = soldier_array[soldierId-1];

	unitRecno = create_soldier_unit(thisSoldier);	 	// if he is a spy, create_soldier_unit wil call set_place(SPY_MOBILE)

	if(!unitRecno)
		return 0; // return 0 eg there is no space to create the unit

	//------- delete the record from the soldier_array ------//

	err_when( soldier_count > MAX_SOLDIER );
	err_when( selected_soldier_id > soldier_count );

	m.del_array_rec(soldier_array, soldier_count, sizeof(Soldier), soldierId);

	if( selected_soldier_id > soldierId || selected_soldier_id == soldier_count )
		selected_soldier_id--;

	soldier_count--;

	err_when( soldier_count < 0 );
	err_when( selected_soldier_id > soldier_count );

	return unitRecno;
}
//----------- End of function FirmCamp::resign_soldier -----------//



//--------- Begin of function FirmCamp::create_soldier_unit ---------//
//
int FirmCamp::create_soldier_unit(Soldier& thisSoldier)
{
	//--------- copy the soldier's info --------//

	int unitLoyalty = thisSoldier.loyalty;

	//------------ create an unit --------------//

	int unitId = thisSoldier.unit_id;
	int unitRecno = create_unit( unitId );      // this soldier no longer has a job as it has been resigned

	if( !unitRecno )
		return 0;

	Unit* unitPtr   = unit_array[unitRecno];
	UnitInfo *unitInfo = unit_res[unitId];

	//------- set the unit's parameters --------//

	unitPtr->skill = thisSoldier.skill;

	err_when( unitPtr->combat_level()<=0 );

	unitPtr->loyalty    = unitLoyalty;
	unitPtr->hit_points = thisSoldier.hit_points;
	unitPtr->rank_id    = thisSoldier.rank_id;
	unitPtr->is_royal   = thisSoldier.is_royal;
	unitPtr->hero_id    = thisSoldier.hero_id;
	unitPtr->unique_id  = thisSoldier.unique_id;

	// ###### begin Gilbert 24/3 #######//
	// if( unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_WEAPON )
	if( unit_res[unitPtr->unit_id]->class_info.has_weapon_version )
	// ###### end Gilbert 24/3 #######//
	{
		unitPtr->set_weapon_version( thisSoldier.extra_para );	// restore nation contribution
	}
	else if( unitPtr->race_id )
	{
		unitPtr->cur_power = thisSoldier.extra_para;

		if( unitPtr->cur_power < 0 )
			unitPtr->cur_power = 0;

		if( unitPtr->cur_power > 150 )
			unitPtr->cur_power = 150;
	}

	err_when( unitPtr->hit_points <= 0 );

	unitPtr->init_attack_info();

	if( thisSoldier.name_id && thisSoldier.race_id )		// if this soldier is formerly an unit who has a name
	{
		// ###### begin Gilbert 1/3 #####//
		unitPtr->set_name(thisSoldier.name_id);	// then give the name to the unit
		thisSoldier.free_name();						// free up the name from monster_res or race_res first
		// ###### end Gilbert 1/3 #####//
	}

	err_when( !unitPtr->is_visible() ); 

	// ------ carry item ---------//

	unitPtr->item = thisSoldier.item;
	thisSoldier.item.clear();

	//------ if the unit is a spy -------//

	if( thisSoldier.spy_recno )
	{
		Spy* spyPtr = spy_array[thisSoldier.spy_recno];

		unitPtr->spy_recno = thisSoldier.spy_recno;
		unitPtr->is_ai		 = spyPtr->cloaked_nation_recno &&
									nation_array[spyPtr->cloaked_nation_recno]->is_ai();

		spyPtr->set_place(SPY_MOBILE, unitRecno);
		// ###### begin Gilbert 24/2 #######//
		err_when( thisSoldier.unique_id != spyPtr->unique_id );
		// ###### end Gilbert 24/2 #######//
	}

	unitPtr->set_combat_level(-1);

	//--- decrease the unit count as it is doubled counted when a new unit is created ---//

	unit_res[unitPtr->unit_id]->dec_nation_unit_count(nation_recno);

	// ##### begin Gilbert 24/2 #######//
	// ------- update win/lose condition on changing of unit recno -----//

	game.update_condition_on_mobilize(unitRecno, unitPtr->unique_id );
	// ##### end Gilbert 24/2 #######//

	return unitRecno;
}
//----------- End of function FirmCamp::create_soldier_unit -----------//


//--------- Begin of function FirmCamp::kill_overseer ---------//
//
// Kill the overeseer of the firm
//
void FirmCamp::kill_overseer()
{
	if( !overseer_recno )
		return;

	//-------- if the overseer is a spy -------//

	Unit* unitPtr = unit_array[overseer_recno];

	if( unitPtr->spy_recno )
		spy_array[unitPtr->spy_recno]->set_place(SPY_UNDEFINED, 0);

	// ####### begin Gilbert 23/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(unitPtr->unique_id);		// check unit killed
	// ####### end Gilbert 23/2 ########//

	//-- no need to del the spy here, unit_array.del() will del the spy --//

	unit_array.del(overseer_recno);

	overseer_recno = 0;
}
//----------- End of function FirmCamp::kill_overseer -----------//


//--------- Begin of function FirmCamp::kill_all_soldier ---------//
//
// All the soldiers in the firm are deleted
//
void FirmCamp::kill_all_soldier()
{
	for(int i=soldier_count; i>0; i--)
		kill_soldier(i);
}
//----------- End of function FirmCamp::kill_all_soldier -----------//


//--------- Begin of function FirmCamp::kill_soldier ---------//
//
// Kill a specific soldier.
//
void FirmCamp::kill_soldier(int soldierId)
{
	err_when( !soldier_array );    // this function shouldn't be called if this firm does not need soldier

	err_when( soldierId<1 || soldierId>soldier_count );

	//------- decrease soldier no. and create an unit -----//

	Soldier* soldierPtr = soldier_array+soldierId-1;
	int	  unitRecno = 0;

	// ####### begin Gilbert 24/2 ########//
	// ------ update win/lose condition -------------//

	if( !sys.signal_exit_flag )
		game.update_condition_on_killed(soldierPtr->unique_id);		// check unit killed
	// ####### end Gilbert 24/2 ########//

	soldierPtr->free_name();

	//-------- if this soldier is a spy ---------//

	if( soldierPtr->spy_recno )
	{
		spy_array[soldierPtr->spy_recno]->set_place(SPY_UNDEFINED, 0);
		spy_array.del_spy( soldierPtr->spy_recno );
	}

	//---------- decrease unit count -----------//

	unit_res[soldierPtr->unit_id]->dec_nation_unit_count(nation_recno);

	//------- delete the record from the soldier_array ------//

	err_when( soldier_count > MAX_SOLDIER );
	err_when( selected_soldier_id > soldier_count );

	m.del_array_rec(soldier_array, soldier_count, sizeof(Soldier), soldierId);

	if( selected_soldier_id > soldierId || selected_soldier_id == soldier_count )
		selected_soldier_id--;

	soldier_count--;

	if( soldier_count==0 )
		selected_soldier_id = 0;

	err_when( selected_soldier_id > soldier_count );
}
//----------- End of function FirmCamp::kill_soldier -----------//


// --------- begin of function FirmCamp::free_all_people -------//
//
void FirmCamp::free_all_people()
{
	if( !sys.signal_exit_flag )
	{
		assign_overseer(0);     // this function must be called before restore_world_matrix(), otherwise the power area can't be completely reset
		resign_all_soldier(); // the workers in the firm will be killed if there is no space for creating the workers
	}

	kill_overseer();				// if the overseer or soldiers cannot be mobilized due to lack of space or that some soldiers are still under training, kill them 
	kill_all_soldier();

	Firm::free_all_people();
}
// --------- end of function FirmCamp::free_all_people -------//


//-------- Begin of function FirmCamp::can_accept_assign ------//
//
// Return whether this firm accepts a new unit.
//
// <int> unitRecno - recno of the unit
// [int] actionNationRecno - the nation that executes the action.
//									  (default: nation_recno of the unit)
//
bool FirmCamp::can_accept_assign(int unitRecno, int actionNationRecno)
{
	if( under_construction )
		return false;

	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->nation_recno != nation_recno )
		return false;

	if( unitPtr->is_civilian() || unitPtr->mobile_type!=UNIT_LAND )		// only take land units
		return false;

	UnitInfo* unitInfo = unit_res[unitPtr->unit_id];

	// ###### begin Gilbert 24/3 ########//
//	if( unitInfo->unit_class != UNIT_CLASS_HUMAN &&		// it must be a human or a weapon
//		 unitInfo->unit_class != UNIT_CLASS_WEAPON &&
//		 unitInfo->unit_class != UNIT_CLASS_MONSTER )
	if( !unitInfo->class_info.has_combat_level
		&& unitInfo->unit_class != UNIT_CLASS_WEAPON
		&& unitInfo->unit_class != UNIT_CLASS_MONS_WEAPON )
	{
		return false;
	}
	// ###### end Gilbert 24/3 ########//

	//----------------------------------------//
	// If this is a spy and the action is ordered
	// by the true owner of the spy, then he can
	// only be assigned to an enemy firm when there
	// is space for the unit.
	//----------------------------------------//

	if( actionNationRecno == -1 )
		actionNationRecno = nation_recno;

	if( unitPtr->spy_recno && unitPtr->nation_recno != actionNationRecno )
	{
		if( unitPtr->rank_id == RANK_SOLDIER )
		{
			if( soldier_count >= MAX_SOLDIER )
				return false;
		}
		else
		{
			if( overseer_recno )
				return false;
		}
	}

	return true;
}
//---------- End of function FirmCamp::can_accept_assign --------//


//------- Begin of function FirmCamp::invoke_defense -----------//
//
//	<BaseObj*> attackerObj - BaseObj of the attacker
//
// <int> netAttackerPower - the net power of the attacker force
//									 that we need to deal with.
//
// return: <int> power of the mobilized defense force
//
int FirmCamp::invoke_defense(BaseObj* attackerObj, int netAttackerPower)
{
	if( !attackerObj )
		return 0;

	if( !defense_flag )		// only when the defense mode is on
		return 0;

	if( soldier_count==0 )
		return 0;

	last_attack_nation_recno = attackerObj->nation_recno;		// last_attack_nation_recno is a BaseObj var, it is for defender unit to decide which units to attack

	int defensePower = 0;

	//---- if this is an independent camp, mobilize all ----//

	if( nation_recno==0 && overseer_recno > 0 )
	{
		TeamInfo* teamInfo = unit_array[overseer_recno]->team_info;

		patrol();

		for( int i=0 ; i<teamInfo->member_count ; i++ )
		{
			Unit* unitPtr = unit_array[teamInfo->member_unit_array[i]];

			unitPtr->set_mode(UNIT_MODE_CAMP_DEFENDER, firm_recno);

			defensePower += unitPtr->obj_power();
		}
	}
	else 	//---- if this is a nation camp, only mobilize needed soldiers ----//
	{
		unit_group.zap();

		for( int i=1 ; i<=soldier_count ; i++ )
		{
			Soldier* soldierPtr = soldier_array+i-1;

			if( soldierPtr->is_under_training() )
				continue;

			int unitRecno = mobilize_soldier(i, COMMAND_AUTO);

			if( !unitRecno )		// failure to mobilize a soldier
				break;

			i--;				// decrease i, as a solider has been removed from unit_array

			//----- add the unit to unit_group for group movement -----//

			unit_group.add_unit(unitRecno);

			//------ set the unit to defense mode -----//

			unit_array[unitRecno]->set_mode(UNIT_MODE_CAMP_DEFENDER, firm_recno);

			//------ if defending force is enough by now -----//

			defensePower += soldierPtr->soldier_power();

			if( defensePower > int( (float)netAttackerPower * 2.5f ) )
				break;
		}

		if( unit_group.size() > 0 )
			unit_group.attack(attackerObj->base_obj_recno, COMMAND_AUTO);
	}

	return defensePower;
}
//--------- End of function FirmCamp::invoke_defense -----------//


// -------- begin of function FirmCamp::cancel_train_soldier --------//
//
// sub-class should not call this function
// bug when multiplayer
//
void FirmCamp::cancel_train_soldier( int soldierId, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <soldierId>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_CANCEL_TRAIN, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = soldierId;
		return;
	}

	err_here();
}
// -------- end of function FirmCamp::cancel_train_soldier --------//


// -------- begin of function FirmCamp::validate_soldier --------//
//
void FirmCamp::validate_soldier()
{
	// validate each soldier's source town recno

	for(int i = 1; i <= soldier_count; ++i )
	{
		Soldier *soldierPtr = &soldier_array[i-1];
		if( soldierPtr->source_town_recno )
		{
			if( town_array.is_deleted(soldierPtr->source_town_recno)
				|| town_array[soldierPtr->source_town_recno]->race_id != soldierPtr->race_id
				|| town_array[soldierPtr->source_town_recno]->nation_recno != nation_recno )
			{
				soldierPtr->source_town_recno = 0;
			}
		}
	}
}
// -------- end of function FirmCamp::validate_soldier --------//


// -------- begin of function FirmCamp::promote_soldier ------//
//
void FirmCamp::promote_soldier( int soldierId, char remoteAction )
{
	if( soldierId <= 0 || soldierId > soldier_count )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_F_CAMP_PROMOTE, 2*sizeof(short));
		// packet structure : <firm recno> <soldier Id>
		shortPtr[0] = firm_recno;
		shortPtr[1] = soldierId;
		return;
	}

	if( overseer_recno || soldier_array[soldierId-1].is_under_training()
		|| soldier_array[soldierId-1].skill_level() < CITIZEN_SKILL_LEVEL )
		return;

	int unitRecno = mobilize_soldier( soldierId, COMMAND_AUTO );
	if( unitRecno )
	{
		unit_array[unitRecno]->set_rank( RANK_GENERAL );
		assign_unit(unitRecno);
	}
}
// -------- end of function FirmCamp::promote_soldier ------//


// -------- begin of function FirmCamp::swap_item --------//
//
// update also FirmMonsterFortress::swap_item
//
int FirmCamp::swap_item( int fromSoldierId, int toSoldierId, int verifyItemId, char remoteAction )
{
	if( fromSoldierId == toSoldierId )
		return 0;
	err_when( fromSoldierId < 0 );
	if( fromSoldierId == 0 && !overseer_recno || fromSoldierId > soldier_count )
		return 0;
	err_when( toSoldierId < 0 );
	if( toSoldierId == 0 && !overseer_recno || toSoldierId > soldier_count )
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

	Item &srcItem = fromSoldierId == 0 ? unit_array[overseer_recno]->item : soldier_array[fromSoldierId-1].item;
	Item &destItem = toSoldierId == 0 ? unit_array[overseer_recno]->item : soldier_array[toSoldierId-1].item;

	// verify item id

	if( srcItem.id != verifyItemId )
		return 0;

	// check if item is suitable for the other side

	if( srcItem.id && !item_res.can_pick(srcItem.id, (toSoldierId==0?overseer_recno:0), (toSoldierId!=0?soldier_array[toSoldierId-1].unit_id:0)) )
		return 0;
	if( destItem.id && !item_res.can_pick(destItem.id, (fromSoldierId==0?overseer_recno:0), (fromSoldierId!=0?soldier_array[fromSoldierId-1].unit_id:0)) )
		return 0;

	// swap srcItem and destItem
	Item tempItem = destItem;
	destItem = srcItem;
	srcItem = tempItem;

	return 1;
}
// -------- end of function FirmCamp::swap_item --------//
