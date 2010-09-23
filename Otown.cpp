//Filename    : OTOWN.CPP
//Description : Object Town

#include <string.h>
#include <ALL.H>
#include <OVGA.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OSYS.H>
#include <OIMGRES.H>
#include <ONEWS.H>
#include <OCONFIG.H>
#include <OFONT.H>
#include <OINFO.H>
#include <OPLANT.H>
#include <ODATE.H>
#include <OSPY.H>
#include <OCONFIG.H>
#include <OF_WORK.H>
#include <OF_MARK.H>
#include <OF_FORT.H>
#include <OF_INN.H>
#include <OUNIT.H>
#include <OTERRAIN.H>
#include <OREBEL.H>
#include <OSPRTRES.H>
#include <ORACERES.H>
#include <ONATION.H>
#include <OREMOTE.H>
#include <OTOWN.H>
#include <ONEWS.H>
#include <OANLINE.H>
#include <OTECHRES.H>
#include <OTECH.H>
#include <OSE.H>
#include <OSERES.H>
#include <OLOG.H>
#include <OITEMRES.H>
#include <OITEM.H>
#include <OF_LAIR.H>
#include <OGODRES.H>


//--------- Begin of function Town::Town ----------//
//
Town::Town()
{
	memset( sizeof(void *) + (char *)this, 0, sizeof(Town) - sizeof(void *) );	// protect vptr
}
//---------- End of function Town::Town ----------//


//--------- Begin of function Town::init ----------//
//
// <int> nationRecno  - the nation recno
// <int> raceId 	    - the race id. of the town
// <int> xLoc, yLoc   - location of the town
// [int] builderRecno - the recno the builder unit,
//								pass 0 if the town's construction is immediately complete, requiring no building time
//
void Town::init(int nationRecno, int raceId, int xLoc, int yLoc, int builderRecno)
{
	nation_recno = nationRecno;
	race_id		 = raceId;

	err_when( raceId<1 || raceId>MAX_RACE );

	//---- set the town section's absolute positions on the map ----//

	loc_x1 = xLoc;
	loc_y1 = yLoc;
	loc_x2 = loc_x1 + STD_TOWN_LOC_WIDTH - 1;
	loc_y2 = loc_y1 + STD_TOWN_LOC_HEIGHT - 1;

	abs_x1 = xLoc * LOCATE_WIDTH;
	abs_y1 = yLoc * LOCATE_HEIGHT;
	abs_x2 = abs_x1 + STD_TOWN_LOC_WIDTH * LOCATE_WIDTH  - 1;
	abs_y2 = abs_y1 + STD_TOWN_LOC_HEIGHT * LOCATE_HEIGHT - 1;

	center_x = (loc_x1+loc_x2)/2;
	center_y = (loc_y1+loc_y2)/2;

	// take the altitude of the top-left corner
	altitude = world.get_corner(loc_x2+1, loc_y2+1)->get_altitude();

	region_id = world.get_region_id( center_x, center_y );

	ai_town = !nation_recno || nation_array[nation_recno]->nation_type == NATION_AI;    // nation_recno==0 for independent towns
	ai_link_checked = 1;			// check the linked towns and firms connected only if ai_link_checked==0

	independent_unit_join_nation_min_rating = 100 + m.random(150);		// the minimum rating a nation must have in order for an independent unit to join it

	setup_date = info.game_date;

	if( builderRecno )
	{
		under_construction = 1;
		set_builder(builderRecno);
	}
	else
	{
		under_construction = 0;
		builder_recno = 0;
	}

	// ##### begin Gilbert 6/11 ######//
	defense_attribute.init( 80, 100 );
	// ##### end Gilbert 6/11 ######//

	//-------- init resistance ------------//

	if( nation_recno==0 )
		init_resistance();

	//-------------------------------------//

	town_name_id = town_res.get_new_name_id(raceId);

	strcpy( town_name(), town_res.get_name(town_name_id) );

	set_world_matrix();

	setup_link();

	auto_set_layout();

	//-------- if this is an AI town ------//

	if( nation_recno )
	{
		nation_array[nation_recno]->add_town_info(town_recno);		// update this for all towns

		if( ai_town )
			update_base_town_status();
	}

	//------ set national auto policy -----//

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		set_auto_collect_tax_loyalty( nationPtr->auto_collect_tax_loyalty );
		set_auto_grant_loyalty( nationPtr->auto_grant_loyalty );
	}

	// #### begin Gilbert 5/2 #####//
	target_wall_man_power = 1;
	// #### end Gilbert 5/2 #####//
}
//--------- End of function Town::init ----------//


//--------- Begin of function Town::deinit ----------//
//
void Town::deinit()
{
	if( sys.quick_exit_flag() )
		return;

	//-----------------------------------//

	if( !town_recno )
		return;

	// ##### begin Gilbert 6/1 ######//

	// ------- unjob all people first ------//

	int unjobTry = population;
	while( worker_population() > 0 && --unjobTry >= 0 )
	{
		unjob_town_people();
	}
	err_when( worker_population() > 0 );

	// ##### end Gilbert 6/1 ######//

	Place::deinit();

	//------- if it's an AI town --------//

	if( nation_recno )
		nation_array[nation_recno]->del_town_info(town_recno);

	//------ if this town is the nation's larget town, reset it ----//

	if( nation_recno &&
		 nation_array[nation_recno]->largest_town_recno == town_recno )
	{
		nation_array[nation_recno]->largest_town_recno = 0;
	}

	//-----------------------------------//

	restore_world_matrix();

	release_link();

	//------- if the current town is the selected -----//

	if( town_array.selected_recno == town_recno )
	{
		town_array.selected_recno = 0;
		info.disp();
	}

	//------- reset parameters ---------//

	town_recno = 0;
}
//--------- End of function Town::deinit ----------//


//--------- Begin of function Town::init_resistance ----------//
//
void Town::init_resistance()
{
	if( nation_recno )		// only for independent town
		return;

	for( int j=0 ; j<MAX_NATION ; j++ )
	{
		resistance_array[j] = (float) town_array.independent_town_resistance();
	}

	//--- some independent towns have higher than normal combat level for its defender ---//

	switch( config.independent_town_resistance )
	{
		case OPTION_LOW:
			town_combat_level = CITIZEN_COMBAT_LEVEL;
			break;

		case OPTION_MODERATE:
			town_combat_level = 30;
			break;

		case OPTION_HIGH:
			town_combat_level = 50;
			break;

		default:
			err_here();
	}
}
//--------- End of function Town::init_resistance ----------//


//------- Begin of function Town::set_world_matrix --------//
//
// Set the cargo id of current town section on the world matrix
//
void Town::set_world_matrix()
{
	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	int xLoc, yLoc;
	Location* locPtr;

	// ##### begin Gilbert 5/11 #######//
	
	// ------ clear obstacle -------//

	for( yLoc=max(loc_y1-INTER_PLACE_SPACE, 0); yLoc<=loc_y2+INTER_PLACE_SPACE && yLoc<world.max_y_loc; yLoc ++ )
	{
		for( xLoc=max(loc_x1-INTER_PLACE_SPACE, 0); xLoc<=loc_x2+INTER_PLACE_SPACE && xLoc<world.max_x_loc; xLoc ++ )
		{
			world.get_loc(xLoc, yLoc)->clear_obstacle();
		}
	}

	// ------- set matrix ------- //
	// ##### end Gilbert 5/11 #######//
	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
		{
			locPtr = world.get_loc(xLoc, yLoc);
			if(!locPtr->cargo_recno)	// skip the location where the settle unit is standing
				locPtr->set_town(town_recno);
		}
	}

	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	establish_contact_with_player();

	//---- set this town's influence on the map ----//

	if( nation_recno )
		world.set_power(loc_x1, loc_y1, loc_x2, loc_y2, nation_recno);

	//------------ reveal new land ----------//

	// ########## begin Gilbert 1/4 #########//
	if( explore_for_player() )
	{
		world.unveil( loc_x1, loc_y1, loc_x2, loc_y2 );
		world.visit( loc_x1, loc_y1, loc_x2, loc_y2, EXPLORE_RANGE-1 );
	}
	// ########## end Gilbert 1/4 #########//

	//---- if the newly built firm is visual in the zoom window, redraw the zoom buffer ----//

	if( is_in_zoom_win() )
		sys.zoom_need_redraw = 1;  // set the flag on so it will be redrawn in the next frame
}
//-------- End of function Town::set_world_matrix --------//


//------- Begin of function Town::establish_contact_with_player --------//
//
// See if the town's location is an explored area, establish contact
// with the player.
//
void Town::establish_contact_with_player()
{
	if( !nation_recno )
		return;

	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	int xLoc, yLoc;
	Location* locPtr;

	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
		{
			locPtr = world.get_loc(xLoc, yLoc);

			if( locPtr->explored() && nation_array.player_recno )
			{
				NationRelation *relation = (~nation_array)->get_relation(nation_recno);

				if( !remote.is_enable() )
				{
					relation->has_contact = 1;
				}
				else
				{
					if( !relation->contact_msg_flag && !relation->has_contact )
					{
						// packet structure : <player nation> <explored nation>
						short *shortPtr = (short *)remote.new_send_queue_msg(MSG_NATION_CONTACT, 2*sizeof(short));
						*shortPtr = nation_array.player_recno;
						shortPtr[1] = nation_recno;
						relation->contact_msg_flag = 1;
					}
				}
			}
		}
	}
}
//-------- End of function Town::establish_contact_with_player --------//


//--------- Begin of function Town::restore_world_matrix --------//
//
// When the town section is destroyed, restore the original land id
//
void Town::restore_world_matrix()
{
	int xLoc, yLoc;

	for( yLoc=loc_y1 ; yLoc<=loc_y2 ; yLoc++ )
	{
		for( xLoc=loc_x1 ; xLoc<=loc_x2 ; xLoc++ )
      {
			world.get_loc(xLoc,yLoc)->remove_town();
		}
	}

	//---- restore this town's influence on the map ----//

	if( nation_recno )
		world.restore_power(loc_x1, loc_y1, loc_x2, loc_y2, town_recno, 0);

	//---- if the newly built firm is visual in the zoom window, redraw the zoom buffer ----//

	if( is_in_zoom_win() )
		sys.zoom_need_redraw = 1;
}
//----------- End of function Town::restore_world_matrix --------//


//---------- Begin of function Town::process_construction --------//
//
void Town::process_construction()
{
	if( !under_construction )
		return;

	//--- can only do construction when the town is not under attack ---//

	if( info.game_date <= last_attack_date+1 )
		return;

	if( sys.frame_count%2!=0 )		// one build every 2 frames
		return;

	//------ increase the construction progress ------//

	Unit *unitPtr = unit_array[builder_recno];

	construction_completion_percent++;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		construction_completion_percent += 10;

	//------- when the construction is complete ----------//

	if( construction_completion_percent >= 100 )
	{
		under_construction = 0;

		if( nation_recno == nation_array.player_recno )	//**BUGHERE - needs to be updated
		{
			se_res.far_sound(center_x, center_y, 1, 'S', unitPtr->sprite_id,
				"FINS", 'F', 1);
		}

		err_when(builder_recno<=0 || unit_array.is_deleted(builder_recno));
		err_when(unitPtr->nation_recno!=nation_recno);

		//---------------------------------------//

		unitPtr->set_mode(0);			// reset it from UNIT_MODE_CONSTRUCT_???
		assign_unit(builder_recno);
		builder_recno = 0;
	}
}
//---------- End of function Town::process_construction --------//


//------- Begin of function Town::complete_construction -----------//
//
// Complete construction instantly.
//
void Town::complete_construction()
{
	under_construction = 0;
}
//--------- End of function Town::complete_construction -----------//


// ##### begin Gilbert 28/12 #######//
//---------- Begin of function Town::process --------//
//
void Town::process()
{
	process_attack_target();
}
//---------- End of function Town::process --------//
// ##### end Gilbert 28/12 #######//


//---------- Begin of function Town::next_day --------//
//
void Town::next_day()
{
#ifdef DEBUG			//BUGHERE

	//------ validate worker count ------//

	int workerCount=0;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		FirmWork* firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

		if( !firmWork )
			continue;

		for( int j=0; j<firmWork->worker_count ; j++ )
		{
			if( firmWork->worker_array[j].town_recno == town_recno )
				workerCount++;
		}
	}

	err_when( workerCount != worker_population() );

#endif

	//---- if the town is still under construction ----//

	if( under_construction )
	{
		construction_completion_percent += 5;

		if( construction_completion_percent >= 100 )
			under_construction = 0;
   }

	//--------------------------------------//

	err_when( population>MAX_TOWN_POPULATION );
	err_when( town_recno<1 );
	err_when( town_recno>town_array.size() );

	int townRecno = town_recno;

	//------ update quality_of_life --------//

	update_quality_of_life();

	//---------- update population ----------//

#if defined(ENABLE_LOG)
	String logStr;
	logStr = "begin Town::next_day, town_recno = ";
	logStr += townRecno;
	logStr += " nation=";
	logStr += nation_recno;
	LOG_MSG(logStr);
#endif

	if( info.game_date%30 == town_recno%30 )
	{
		LOG_MSG(" population_grow");
		population_grow();
		LOG_MSG(m.get_random_seed());
	}

	err_when(population>MAX_TOWN_POPULATION);

	//------- update link status to camps ------//

	LOG_MSG(" update_camp_link");
	update_camp_link();
	LOG_MSG(m.get_random_seed());

	//------ update target loyalty/resistance -------//

	if( info.game_date%15 == town_recno%15 )
	{
		if( nation_recno )
		{
			LOG_MSG(" update_target_loyalty");
			update_target_loyalty();
		}
		else
		{
			LOG_MSG(" update_target_resistance");
			update_target_resistance();		// update resistance for independent towns
		}
		LOG_MSG(m.get_random_seed());
	}

	//------ update loyalty/resistance -------//

	if( info.game_date%5 == town_recno%5 )
	{
		if( nation_recno )
		{
			LOG_MSG(" update_target_loyalty");
			update_loyalty();
		}
		else
		{
			LOG_MSG(" update_target_resistance");
			update_resistance();
		}
		LOG_MSG(m.get_random_seed());

		if( town_array.is_deleted(townRecno) )
			return;
	}

	//-------- think about rebel -----//

	if( nation_recno && info.game_date%15 == town_recno%15 )
	{
		LOG_MSG(" think_rebel");
		think_rebel();
		LOG_MSG(m.get_random_seed() );

		if( town_array.is_deleted(townRecno) )
			return;
	}

	//-------- think about surrender -----//

	if( nation_recno &&
		 ( info.game_date%15==town_recno%15 || loyalty==0 ) )
	{
		LOG_MSG(" think_surrender");
		think_surrender();		// for nation town only, independent town surrender is handled in update_resistance()
		LOG_MSG(m.get_random_seed() );

		if( town_array.is_deleted(townRecno) )
			return;
	}

	// ------- process wall --------//

	int wallManPower = process_wall();

	// ------- replenish archer energy ------//

	archers_energy = jobless_population;

	// ###### begin Gilbert 28/12 ########//
	process_scan_target();
	// ###### end Gilbert 28/12 ########//

	//-------- process food ---------//

	if( nation_recno )
	{
		LOG_MSG(" process_food");
		process_food(wallManPower);
		LOG_MSG(m.get_random_seed());

		if( town_array.is_deleted(townRecno) )
			return;
	}

	//------ auto collect tax and auto grant -------//

	if( nation_recno )
	{
		LOG_MSG(" process_auto");
		process_auto();
		LOG_MSG(m.get_random_seed() );

		if( town_array.is_deleted(townRecno) )
			return;
	}

	//------ collect yearly tax -------//

	if( nation_recno && info.game_month==1 && info.game_day==1 )
	{
		collect_yearly_tax();
	}

	//------ catching spies -------//

	LOG_MSG(" process_auto");

	if( info.game_date%30 == town_recno%30 )
		spy_array.catch_spy(SPY_TOWN, town_recno);

	LOG_MSG(m.get_random_seed() );

	if( town_array.is_deleted(townRecno) )
		return;

	//-------- update visibility ---------//

	// ####### begin Gilbert 1/4 ########//
	if( explore_for_player() )
	// ####### end Gilbert 1/4 ########//
	{
		world.visit( loc_x1, loc_y1, loc_x2, loc_y2, EXPLORE_RANGE-1 );
	}

	//--- recheck no_neighbor_space after a period, there may be new space available now ---//

	if( no_neighbor_space && info.game_date%180 == town_recno%180 )
	{
		LOG_MSG("begin finding neighor space");
		short buildXLoc, buildYLoc;

		//--- for independent town, since we can't call find_best_place_loc(), we just set no_neighbor_space to 0 every 6 months, if it still has no space, then no_neighbor_space will be set 1 again. ---//

		if( nation_recno==0 || nation_array[nation_recno]->find_best_place_loc(FIRM_INN, loc_x1, loc_y1, buildXLoc, buildYLoc) )		// whether it's FIRM_INN or not really doesn't matter, just any firm type will do
			no_neighbor_space = 0;

		LOG_MSG(m.get_random_seed());
	}

	//------ decrease penalties -----//

	if( accumulated_collect_tax_penalty > 0 )
		accumulated_collect_tax_penalty--;

	if( accumulated_reward_penalty > 0 )
		accumulated_reward_penalty--;

	if( accumulated_recruit_penalty > 0 )
		accumulated_recruit_penalty--;

	if( accumulated_enemy_grant_penalty > 0 )
		accumulated_enemy_grant_penalty--;

	//---- decrease bonus loyalty -----//

	if( bonus_loyalty > 0 && info.game_date%10 == town_recno%10 )
		bonus_loyalty--;

   //-----------------------------------------------------------//
	// Try to achieve even workforce distribution by transferring
	// workers from high employment firms to low employment firms.
	//-----------------------------------------------------------//

	if( jobless_population==0 && info.game_date%30 == town_recno%30 )
		evenly_distribute_worker();
}
//----------- End of function Town::next_day ---------//

//---------- Begin of function Town::next_month --------//
//
void Town::next_month()
{
	pay_tribute_to_monster();
}
//----------- End of function Town::next_month ---------//


//---------- Begin of function Town::pay_tribute_to_monster --------//
//
void Town::pay_tribute_to_monster()
{
	if( nation_recno )		// only independent towns pay tribute
		return;

	//---- count the number of linked lairs ----//

	int   lairCount=0;
	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->firm_id == FIRM_LAIR &&
			 firmPtr->nation_recno &&
          !firmPtr->under_construction && 
			 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			lairCount++;
		}
	}

	if( lairCount==0 )
		return;

	//------ pay tributes now --------//

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		float tributeAmount = tribute_to_lair( linked_firm_array[i], lairCount );

		if( tributeAmount > 0 )
		{
			Firm* firmPtr = firm_array[ linked_firm_array[i] ];

			nation_array[firmPtr->nation_recno]->add_income( INCOME_TRIBUTE, tributeAmount );
		}
	}
}
//----------- End of function Town::pay_tribute_to_monster ---------//


//---------- Begin of function Town::tribute_to_lair --------//
//
// Return the amount of tribute paid to the given firm by this town.
//
// <int> firmRecno - recno of the lair.
// <int> lairCount - the number of lairs connected to the town.
//
float Town::tribute_to_lair(int firmRecno, int lairCount)
{
	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->firm_id == FIRM_LAIR &&
		 firmPtr->nation_recno &&
		 !firmPtr->under_construction &&
		 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY &&
		 firmPtr->cast_to_FirmLair()->overseer_recno )
	{
		int commanderSkill = unit_array[firmPtr->cast_to_FirmLair()->overseer_recno]->skill_level();

		return (float) population * MONSTER_TOWN_TRIBUTE_PER_UNIT / lairCount *
				 (100 + min(commanderSkill,100)) / 200;
	}

	return 0;
}
//----------- End of function Town::tribute_to_lair ---------//


//---------- Begin of function Town::total_tribute_to_monster --------//
//
// <int> nationRecno - the total tribute paid to Fryhtans lairs of this
//							  nation.
//
// return: <int> - the total amount of tribute.
//
int Town::total_tribute_to_monster(int nationRecno)
{
	if( nation_recno )		// only independent towns pay tribute
		return 0;

	//---- count the number of linked lairs ----//

	int   lairCount=0;
	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->firm_id == FIRM_LAIR &&
			 firmPtr->nation_recno &&
			 !firmPtr->under_construction &&
			 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			lairCount++;
		}
	}

	if( lairCount==0 )
		return 0;

	//--- calculate the tribute needed to be paid to the specific nation ---//

	float totalTribute=0;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->nation_recno == nationRecno )
		{
			totalTribute += tribute_to_lair( linked_firm_array[i], lairCount );
		}
	}

	return (int) totalTribute;
}
//----------- End of function Town::total_tribute_to_monster ---------//


//---------- Begin of function Town::is_pay_tribute_to_monster --------//
//
// Return whether this town is paying any tributes to Fryhtans.
//
// [int] nationRecno - if this is given, then check if the town
//							  is paying tribute to this nation. Otherwise
//							  just check if this town is paying tribute
//							  to ANY nations.
//
int Town::is_pay_tribute_to_monster(int nationRecno)
{
	if( nation_recno )		// only independent towns pay tribute
		return 0;

	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->firm_id == FIRM_LAIR &&
			 firmPtr->nation_recno &&
			 !firmPtr->under_construction &&
			 resistance(firmPtr->nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			if( !nationRecno || nationRecno==firmPtr->nation_recno )
				return 1;
		}
	}

	return 0;
}
//----------- End of function Town::is_pay_tribute_to_monster ---------//


//---------- Begin of function Town::process_food --------//
//
void Town::process_food(int busyPeasant)
{
	err_when( busyPeasant > jobless_population );

	float yieldRate = 1.0f;

	if( nation_recno )
	{
		yieldRate += 0.25f * tech_res[TECH_FARMING]->get_nation_tech_level(nation_recno);
		// ###### begin Gilbert 16/2 ######//
		// yieldRate += 0.03125f * god_res[GOD_CARTH]->nation_prayer_count(nation_recno);
		yieldRate += 0.05f * god_res[GOD_CARTH]->nation_prayer_count(nation_recno);
		// ###### end Gilbert 16/2 ######//
	}

	//--------- Peasants produce food ---------//

	int foodYearProduction;

	if( !nation_array[nation_recno]->is_ai() &&
		 config.start_up_cash <= OPTION_MODERATE )		// if startup cash >= OPTION_HIGH, then not reduction in food production
	{
		foodYearProduction = PEASANT_FOOD_YEAR_PRODUCTION * 25 / 30;		// less food production for the player
	}
	else
		foodYearProduction = PEASANT_FOOD_YEAR_PRODUCTION;

	nation_array[nation_recno]->add_food( float(jobless_population - busyPeasant) * yieldRate * foodYearProduction / 365 );

	//---------- People consume food ----------//

	nation_array[nation_recno]->consume_food( (float) population * UNIT_FOOD_YEAR_CONSUMPTION / 365 );
}
//----------- End of function Town::process_food ---------//


//---------- Begin of function Town::process_auto --------//
//
void Town::process_auto()
{
	if( !has_linked_own_camp )		// can only collect or grant when there is a linked camp of its own.
		return;

	Nation* nationPtr = nation_array[nation_recno];

	//----- auto collect tax -----//

	if( auto_collect_tax_loyalty > 0 )
	{
		if( accumulated_collect_tax_penalty == 0 &&
			 loyalty >= auto_collect_tax_loyalty )
		{
			collect_tax(COMMAND_AI);
		}
	}

	//---------- auto grant -----------//

	if( auto_grant_loyalty > 0 )
	{
		if( accumulated_reward_penalty == 0 &&
			 loyalty < auto_grant_loyalty && nationPtr->cash > 0 )
		{
			reward(COMMAND_AI);
		}
	}
}
//----------- End of function Town::process_auto ---------//


//---------- Begin of function Town::set_nation --------//
//
// Set the nation of this town.
//
// Possible changes:
//
// independent town -> nation town
// nation A's town -> nation B's town (surrender)
//
void Town::set_nation(int newNationRecno)
{
	if( nation_recno == newNationRecno )
		return;

	//------------- stop all actions to attack this town ------------//

	unit_array.stop_attack_obj(base_obj_recno);
	rebel_array.stop_attack_town(town_recno);

	//--------- update AI town info ---------//

	if( nation_recno )
		nation_array[nation_recno]->del_town_info(town_recno);

	//--------- reset vars ---------//

	is_base_town = 0;
	town_defender_count = 0; 	// reset defender count

	//----- set power region of the new nation ------//

	world.restore_power(loc_x1, loc_y1, loc_x2, loc_y2, town_recno, 0);		// restore power of the old nation
	world.set_power(loc_x1, loc_y1, loc_x2, loc_y2, newNationRecno);			// set power of the new nation

	//--- update the cloaked_nation_recno of all spies in the firm ---//

	spy_array.change_cloaked_nation(SPY_TOWN, town_recno, nation_recno, newNationRecno);		// check the cloaked nation recno of all spies in the firm

	//--------- set nation_recno --------//

	int oldNationRecno = nation_recno;
	nation_recno = newNationRecno;

	if( nation_recno )      // reset rebel_recno if the town is then ruled by a nation
	{
		if( rebel_recno )
		{
			Rebel* rebelPtr = rebel_array[rebel_recno];

			err_when( rebelPtr->mobile_rebel_count > 0 );	// change nation shouldn't happen if mobile_rebel_count > 0

			rebel_array.del_rebel(rebel_recno);		// delete the rebel group
			rebel_recno = 0;
		}
	}

	//--------- update ai_town ----------//

	if( nation_recno==0 )		// independent town
	{
		ai_town = 1;
	}
	else
	{
		nation_array[nation_recno]->add_town_info(town_recno);

		ai_town = nation_array[nation_recno]->nation_type==NATION_AI;
	}

	//------ if there is a builder in this firm, change its nation also ----//

	if( builder_recno )
	{
		Unit* unitPtr = unit_array[builder_recno];

		if( unitPtr->nation_recno != newNationRecno )
		{
			unitPtr->change_nation(newNationRecno);

			//--- if this is a spy, chance its cloak ----//

			if( unitPtr->spy_recno )
				spy_array[unitPtr->spy_recno]->cloaked_nation_recno = newNationRecno;
		}
	}

	//------ set the loyalty of the town people ------//

	if( nation_recno == 0 )			// independent town set up by rebel
	{
		loyalty = 80;
	}
	else
	{
		if( nation_array[nation_recno]->race_id == race_id )
			loyalty = 60;					 // loyalty is higher if the ruler and the people are the same race
		else
			loyalty = 50;
	}

	bonus_loyalty = 0;

	//------- reset town_combat_level -------//

	town_combat_level = 0;
	independent_unit_join_nation_min_rating = 100 + m.random(150);		// prevent it from changing nation again too soon

	//------- reset build wall vars -------//

	target_wall_level = current_wall_level;
	upgrade_wall_progress = 0;

	//------ reset accumulated penalty ------//

	accumulated_collect_tax_penalty = 0;
	accumulated_reward_penalty	 = 0;
	accumulated_enemy_grant_penalty = 0;
	accumulated_recruit_penalty = 0;

	//-------- update loyalty ---------//

	update_target_loyalty();

	//--------- update links ----------//

	set_nation_unjob_worker();

	release_link();			// we need to update links as when the nation link the links to original firms may be broken.
	setup_link();

	//--- if a nation set up a town in a location that the player has explored, contact between the nation and the player is established ---//

	establish_contact_with_player();

	//------ set national auto policy -----//

	if( nation_recno )
	{
		Nation* nationPtr = nation_array[nation_recno];

		set_auto_collect_tax_loyalty( nationPtr->auto_collect_tax_loyalty );
		set_auto_grant_loyalty( nationPtr->auto_grant_loyalty );
	}
	else
	{
		init_resistance();		// generate resistance for independent town
	}

	//---- reset the action mode of all spies in this town ----//

	spy_array.set_action_mode( SPY_TOWN, town_recno, SPY_IDLE );      // we need to reset it. e.g. when we have captured an enemy town, SPY_SOW_DISSENT action must be reset to SPY_IDLE

	//-------- refresh display ----------//

	if( town_array.selected_recno == town_recno )
		info.disp();

	// ###### begin Gilbert 5/7 ########//
	if( explore_for_player() )
	{
		world.unveil( loc_x1, loc_y1, loc_x2, loc_y2 );
		world.visit( loc_x1, loc_y1, loc_x2, loc_y2, EXPLORE_RANGE-1 );
	}
	// ###### end Gilbert 5/7 ########//
}
//----------- End of function Town::set_nation ---------//


//--------- Begin of function Town::set_nation_unjob_worker -------//
//
// This function is only called by Town::set_nation()
//
// Remove workers from firms that no longer have valid links to this
// town after changing the nation recno.
//
// Note: if a group of firms and towns change nation, firms should
// change nation first, so when towns change nation, the workers can
// stay in the firms.
//
void Town::set_nation_unjob_worker()
{
	if( nation_recno==0 )		// if this is an independent town, citizens there can work for any firms
		return;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		FirmWork* firmWork = firm_array[linked_firm_array[i]]->cast_to_FirmWork();

		if( !firmWork )
			continue;

		//--- if this firm longer has a valid link to the town after changing the nation recno ---//

		if( firmWork->nation_recno != nation_recno )
		{
			for( int j=firmWork->worker_count ; j>0 ; j-- )
			{
				if( firmWork->worker_array[j-1].town_recno == town_recno )
					firmWork->resign_worker(j);
			}
		}
	}
}
//---------- End of function Town::set_nation_unjob_worker --------//


//---------- Begin of function Town::set_race --------//
//
// Set the race of this town. This function is for scenario
// creation or editing only.
//
void Town::set_race(int newRaceId)
{
	race_id = newRaceId;

	update_target_loyalty();
	update_target_resistance();

	auto_set_layout();
}
//----------- End of function Town::set_race ---------//


//------ Begin of function Town::camp_influence ------//
//
// Return a specific military camp's influence on this town.
//
// <FirmCamp*> firmCamp - the pointer to the camp or fort.
//
// return: <int> influence index (0-100)
//
int Town::camp_influence(FirmCamp* firmCamp)
{
	if( !firmCamp->overseer_recno )
		return 0;

	//----- get the influence ------//

	int campInfluence = camp_influence( unit_array[firmCamp->overseer_recno] );

	//--- additional influence for a fort ---//

	if( firmCamp->firm_id == FIRM_FORT )
		campInfluence += 15;

	return campInfluence;
}
//------- End of function Town::camp_influence ------//


//------ Begin of function Town::camp_influence ------//
//
// Return a specific military camp's influence on this town.
//
// <Unit*> unitPtr - unit of the commander in the military camp.
//
// return: <int> influence index (0-100)
//
int Town::camp_influence(Unit* unitPtr)
{
	return camp_influence( unitPtr->nation_recno, unitPtr->race_id, unitPtr->skill_level() );
}
//------- End of function Town::camp_influence ------//


//------ Begin of function Town::camp_influence ------//
//
// Return a specific military camp's influence on this town.
//
// <InnUnit*> innUnit - an Inn unit of the commander in the military camp.
// <int> 	  nationRecno - nation recno of the inn unit.
//
// return: <int> influence index (0-100)
//
int Town::camp_influence(InnUnit* innUnit, int nationRecno)
{
	return camp_influence( nationRecno, unit_res[innUnit->unit_id]->race_id, innUnit->skill_level() );
}
//------- End of function Town::camp_influence ------//


//------ Begin of function Town::camp_influence ------//
//
// Return a specific military camp's influence on this town.
//
// The influence is calculated based on :
// - the leadership of the commander
// - whether the leader is racially homogenous to this town.
// - whether the king is racially homogenous to this town.
// - the reputation of the nation
//
// <int> nationRecno - nation recno of the commander
// <int> raceId	   - race id. of the unit.
// <int> skillLevel  - leadership skill of the unit.
//
// return: <int> influence index (0-100)
//
int Town::camp_influence(int nationRecno, int raceId, int skillLevel)
{
	Nation* nationPtr = nation_array[nationRecno];   // nation of the unit

	if( raceId <= 0 )		// only human commanders have influence
		return 0;

	int thisInfluence = min(100,skillLevel) / 2;			// 50% of the leadership, the maximum effective skill is 100

	if( race_res.is_same_race(race_id, raceId) )
		thisInfluence += thisInfluence/3;		// 33% bonus if commander's race is the same as the town's.
	else
		thisInfluence -= 20;			// 20 points penalty if the nationalities are different

	thisInfluence += (int) nationPtr->reputation/2;

	thisInfluence = min(100, thisInfluence);

	return max(0, thisInfluence);
}
//------- End of function Town::camp_influence ------//


//---------- Begin of function Town::collect_yearly_tax --------//
//
// Collect yearly tax from villagers - this does not decrease their
// loyalty.
//
void Town::collect_yearly_tax()
{
	int taxPerUnit = TAX_PER_UNIT;

	//-------- increased cash for human players -------//

	if( !is_ai )		// human player
	{
		switch( config.start_up_cash )
		{
			case OPTION_VERY_HIGH:
				taxPerUnit += 3;
				break;

			case OPTION_HIGH:
				taxPerUnit += 2;
				break;

			case OPTION_LOW:
				taxPerUnit -= 1;
				break;
		}
	}
	else					// AI
	{
		switch( config.ai_start_up_cash )
		{
			case OPTION_VERY_HIGH:
				taxPerUnit += 2;
				break;

			case OPTION_HIGH:
				taxPerUnit += 1;
				break;
		}
	}

	nation_array[nation_recno]->add_income( INCOME_TAX, (float)population * taxPerUnit );
}
//----------- End of function Town::collect_yearly_tax ---------//


//---------- Begin of function Town::collect_tax --------//
//
// Collect tax from this town.
//
void Town::collect_tax(char remoteAction)
{
	if( !has_linked_own_camp )
		return;

	//------------------------------------------//

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <race id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_COLLECT_TAX, sizeof(short));
		shortPtr[0] = town_recno;
		return;
	}

	//----- calculate the loyalty decrease amount ------//
	//
	// If you reward too frequently, the negative effect
	// on loyalty will get larger.
	//
	//--------------------------------------------------//

	int loyaltyDecrease = COLLECT_TAX_LOYALTY_DECREASE + accumulated_collect_tax_penalty/5;

	loyaltyDecrease = min(loyaltyDecrease, 20);

	// ###### begin Gilbert 19/11 #######//
	// ---- effect of god -------//

	// ##### patch begin Gilbert 16/2 ########//
//	if( race_id == RACE_NORMAN && nation_recno 
//		&& god_res[GOD_NORMAN]->nation_prayer_count(nation_recno) > 0 )
	if( nation_recno && god_res[GOD_NORMAN]->nation_prayer_count(nation_recno) > 0 )
	{
		loyaltyDecrease -= 3;
	}
	// ##### patch end Gilbert 16/2 ########//

	accumulated_collect_tax_penalty += 10;

	// ------ cash increase depend on loyalty drop --------//

	float taxCollected = 0.0f;
	float beforeLoyalty = loyalty;
	change_loyalty( (float) -loyaltyDecrease );
	taxCollected += (beforeLoyalty - loyalty) * population * TAX_PER_UNIT / loyaltyDecrease;

	//----------- increase cash ------------//

	nation_array[nation_recno]->add_income(INCOME_TAX, taxCollected );

	// ###### end Gilbert 19/11 #######//

	//------------ think rebel -------------//

	think_rebel();
}
//----------- End of function Town::collect_tax ---------//


//---------- Begin of function Town::reward --------//
//
// Reward cash to the peasants.
//
void Town::reward(char remoteAction)
{
	if( !has_linked_own_camp )
		return;

	//------------------------------------------//

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <race id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_REWARD, sizeof(short));
		shortPtr[0] = town_recno;
		return;
	}

	// ###### begin Gilbert 13/5 ########//
	if( nation_array[nation_recno]->cash <= 0 )
		return;
	// ###### end Gilbert 13/5 ########//

	//----- calculate the loyalty increase amount ------//
	//
	// If you reward too frequently, the effect of the
	// granting will be diminished.
	//
	//--------------------------------------------------//

	int loyaltyIncrease = TOWN_REWARD_LOYALTY_INCREASE - accumulated_reward_penalty/5;

	loyaltyIncrease = max(3, loyaltyIncrease);

	accumulated_reward_penalty += 10;

	//------ increase the loyalty of the town people ------//

	change_loyalty( (float) loyaltyIncrease );

	//----------- decrease cash ------------//

	nation_array[nation_recno]->add_expense(EXPENSE_GRANT_OWN_TOWN, (float)population * TOWN_REWARD_PER_UNIT );
}
//----------- End of function Town::reward ---------//

//------- Begin of function Town::think_surrender ---------//
//
// Think about surrendering to a nation with camps linked to
// this town.
//
int Town::think_surrender()
{
	//--- if this is a slave town, it can never surrender to a human kingdom unless the enslaving Fryhtan lair is destroyed ---//

	if( is_pay_tribute_to_monster() )
		return 0;

	//----------------------------------//

	if( nation_recno )
	{
		//--- only surrender when there is no own camps, but has enemy camps linked to this town ---//

		if( has_linked_own_camp || !has_linked_enemy_camp )
			return 0;

		if( loyalty >= SURRENDER_LOYALTY )
			return 0;
	}

	//-------- think about surrender to which nation ------//

	int		 curRating, bestNationRecno=0;
	FirmCamp* firmCamp;

	// ###### begin Gilbert 22/4 ########//
	int bestRating = nation_recno ? (int) loyalty : 0;	// if it belongs to a nation, surrender only if resistance below loyalty
	// ###### end Gilbert 22/4 ########//

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		firmCamp = (FirmCamp*) firm_array[linked_firm_array[i]];

		//---- if this is an enemy camp ----//

		if( firmCamp->cast_to_FirmCamp() &&
			 firmCamp->nation_recno != nation_recno &&
			 firmCamp->nation_recno &&
			 firmCamp->overseer_recno &&
			 firmCamp->is_human() )				// only surrender to humans
		{
			//----- for independent towns, only surrender if resistance < SURRENDER_RESISTANCE

			if( nation_recno==0 &&
				 resistance( firmCamp->nation_recno ) >= SURRENDER_RESISTANCE )
			{
				continue;
			}

			//------------------------------//

			curRating = camp_influence(firmCamp);		// see camp_influence() for details on how the rating is calculated

			if( curRating > bestRating )
			{
				bestRating = curRating;
				bestNationRecno = firmCamp->nation_recno;
			}
		}
	}

	//------------------------------------//

	if( bestNationRecno )
	{
		surrender( bestNationRecno );
		return 1;
	}
	else
		return 0;
}
//------- End of function Town::think_surrender ---------//



//------- Begin of function Town::surrender ---------//
//
// This town surrenders to another nation.
//
// <int> toNationRecno - the recno of the nation this town surrenders to.
//
void Town::surrender(int toNationRecno)
{
	//--- if this is a rebel town and the mobile rebel count is > 0, don't surrender (this function can be called by update_resistance() when resistance drops to zero ---//

	if( rebel_recno )
	{
		Rebel* rebelPtr = rebel_array[rebel_recno];

		if( rebelPtr->mobile_rebel_count > 0 )
			return;
	}

	//----------------------------------------//

	if( nation_recno  == nation_array.player_recno ||
		 toNationRecno == nation_array.player_recno )
	{
		news_array.town_surrendered(town_recno, toNationRecno);

		if( toNationRecno == nation_array.player_recno )
			se_ctrl.immediate_sound("GET_TOWN");
	}

	set_nation( toNationRecno );

	//---- open links to firms of new master nation ----//

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		Firm* firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->nation_recno == toNationRecno )
		{
			toggle_firm_link( i+1, 1, 1 );			// 1-set links of both sides

			//--- if this is a camp and it has no linked town, set active linked town to it ---//

			if( firmPtr->firm_id == FIRM_FORT )
			{
				FirmFort* firmFort = firmPtr->cast_to_FirmFort();

				if( firmFort->active_link_town_recno==0 &&
					 firmFort->can_set_active_link(town_recno) )
				{
					firmFort->set_active_link(town_recno, COMMAND_AUTO);
				}
			}
		}
	}
}
//------- End of function Town::surrender ---------//


//------- Begin of function Town::think_rebel ---------//
//
void Town::think_rebel()
{
	if( !nation_recno )
		return;

	if( loyalty > REBEL_LOYALTY )
		return;

	#define REBEL_INTERVAL_MONTH		3		// don't rebel twice in less than 3 months

	if( info.game_date < last_rebel_date + REBEL_INTERVAL_MONTH*30 )
		return;

	if( town_defender_count > 0 || info.game_date < last_attack_date + 30 )		// don't rebel within 30 days after being attacked by a hostile unit
		return;

	if( spy_count >= population )	// spies do not rebel together with the rebellion
		return;

	//----- if there was just one unit in the town and he rebels ----//

	int oneRebelOnly=0;

	if( population==1 )
	{
		news_array.town_rebel(town_recno, 1);
		oneRebelOnly = 1;
	}

	//----- create the rebel leader and the rebel group ------//

	int rebelCount=1;
	int leaderUnitRecno = create_rebel_unit(1);		// 1-the unit is the rebel leader

	if( !leaderUnitRecno )
		return;

	Unit *unitPtr = unit_array[leaderUnitRecno];

	//--- if the town only has one unit and that unit rebels, handle specially with the below code ---//

	if( oneRebelOnly )		// if there was just one unit in the town and he rebels
	{
		rebel_array.create_rebel(leaderUnitRecno, nation_recno);
		return;
	}

	int rebelRecno = rebel_array.create_rebel(leaderUnitRecno, nation_recno, REBEL_ATTACK_TOWN, town_recno);		// create a rebel group

	//------- create other rebel units in the rebel group -----//

	int raceRebelCount = (int) (population-spy_count) * (30+m.random(30)) / 100;		// 30% - 60% of the unit will rebel.

	err_when(raceRebelCount+1 > MAX_TOWN_POPULATION); // plus 1 for the leader, cannot excess MAX_TOWN_POPULATION, consider the case these units settle immediately

	for( int j=0 ; j<raceRebelCount ; j++ )		// no. of rebel units of this race
	{
		int unitRecno = create_rebel_unit(0);

		if( !unitRecno )		// 0-the unit is not the rebel leader
			break;

		unitPtr = unit_array[unitRecno];
		unitPtr->leader_unit_recno = leaderUnitRecno;

		rebel_array[rebelRecno]->join(unitRecno);

		rebelCount++;
	}

	//--- when disloyal units left, the average loyalty increases ---//

	change_loyalty( (float) 50 * j / population );

	//------- order the rebel to attack this town -------//

//	rebel_array[rebelRecno]->all_rebel_attack( base_obj_recno );

	//---------- add news -------------//

	last_rebel_date = info.game_date;

	news_array.town_rebel(town_recno, rebelCount);		// add the news first as after callijng ai_spy_town_rebel, the town may disappear as all peasants are gone

	//--- tell the AI spies in the town that a rebellion is happening ---//

	spy_array.ai_spy_town_rebel(town_recno);
}
//------- End of function Town::think_rebel ---------//


//-------- Begin of function Town::create_rebel_unit --------//
//
// Create a rebel unit from a town.
//
// <int> isLeader - whether the unit is a rebel leader
//
// return: <int> recno of the unit created.
//
int Town::create_rebel_unit(int isLeader)
{
	if( recruitable_pop(0)==0 )	// if the unjob unit is a spy too, then don't rebel
		return 0;

	//----look for an empty locatino for the unit to stand ----//
	//--- scan for the 5 rows right below the building ---//

	int            unitId = race_res[race_id]->civilian_unit_id;
	SpriteInfo*    spriteInfo = sprite_res[unit_res[unitId]->sprite_id];
	int            xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results

	if( !world.locate_space( xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return 0;

	//---------- add the unit now -----------//

	int rankId;

	if( isLeader )
		rankId = RANK_GENERAL;
	else
		rankId = RANK_SOLDIER;

	int unitRecno = unit_array.add_unit( unitId, 0, rankId, 0, xLoc, yLoc );

	dec_pop();

	//------- set the unit's parameters --------//

	Unit* unitPtr = unit_array[unitRecno];

	if( isLeader )
	{
		int combatLevel 	  = 10 + population*2 + m.random(10);		// the higher the population is, the higher the combat level will be
		int leadershipLevel = 10 + population   + m.random(10);		// the higher the population is, the higher the combat level will be

		unitPtr->set_combat_level( min(combatLevel, 100) );
		unitPtr->skill.set_skill_level( min(leadershipLevel, 100) );
	}
	else
	{
		unitPtr->set_combat_level(CITIZEN_COMBAT_LEVEL); 	// combat: 10
	}

	return unitRecno;
}
//--------- End of function Town::create_rebel_unit ---------//


//--------- Begin of function Town::being_attack_hit -------//
//
void Town::being_attack_hit(BaseObj* attackerObj, float damagePoint)
{
	//----------------------------------------------------------------//
	// If the derived class does not call being_attack_hit(), it still
	// has to call being_attack_hit2(), because some vars must be set
	// for an attack action.
	//----------------------------------------------------------------//

	being_attack_hit2(attackerObj);

	//-------------------------------------------------//

	int attackerNationRecno = 0;

	if( attackerObj )
	{
		if( rebel_recno )		// if this town is controlled by a rebel group
			rebel_array[rebel_recno]->town_being_attacked(attackerObj);

		if( attackerObj->nation_recno == nation_recno )		// this can happen when the unit has just changed nation
			return;

		attackerNationRecno = attackerObj->nation_recno;

		//--------- store attacker nation recno -----------//

		err_when(attackerNationRecno<0 || attackerNationRecno>MAX_NATION);

		// --------- return fire ----------//

		return_fire( attackerObj );		// didn't call Place::return_fire, so call myself

		//------ call out defender (only when the wall is gone) ------//

		if( hit_points <= 0 )
		{
			// only call out defender when the attacking unit is within the effective defending distance

			if( m.points_distance( attackerObj->obj_loc_x1(), attackerObj->obj_loc_y1(),
				 center_x, center_y ) <= EFFECTIVE_DEFEND_DISTANCE )
			{
				int loopCount=0;

				while(1)
				{
					if( !mobilize_defender(attackerNationRecno) )
						break;

					// ####### begin Gilbert 15/4 ########//
					// attack first, don't wait for next_day's call think_aggressive_attack
					// unit_array[?]->think_aggressive_attack( 0, true );		
					// ####### end Gilbert 15/4 ########//

					err_when( loopCount++ > 1000 );
				}
			}
		}

		invoke_camp_defense(attackerObj);
	}

	//------- if the town has a wall, the wall absorbs the damage -------//

	if( hit_points > 0.0f )
	{
		hit_points -= damagePoint / ATTACK_SLOW_DOWN / 5;		// divided by 5 is to make it slower in destroying towns

		if( hit_points <= 0.0f )
		{
			hit_points = 0.0f;
			received_hit_count = 0.0f;
		}
	}

	//----- if there are people in the town -----//

	else if( population > 0 )
	{
		//-------- town people get killed ---------//

		if( town_defender_count==0 )
			received_hit_count += damagePoint;
		else
			received_hit_count += damagePoint / 3;		// if there are still defenders out there, the number of people killed will be fewer. This is a gameplay design to prevent getting too many people unnecessarily killed (as the attacker ultimate want to take over the town and the town people will become his later.)

		if( received_hit_count >= RECEIVED_HIT_PER_KILL )
		{
			received_hit_count = (float) 0;

			int townRecno = town_recno;

			kill_town_people(attackerNationRecno);		// kill a town people

			if( town_array.is_deleted(townRecno) )		// the town may have been deleted when all pop are killed
				return;
		}

		//---- decrease resistance if this is an independent town ----//

		if( town_defender_count==0 )
		{
			//--- Resistance/loyalty of the town people decrease if the attacking continues ---//
			//
			// Resistance/Loyalty decreases faster:
			//
			// -when there are few people in the town
			// -when there is no defender
			//
			//---------------------------------------//

			float loyaltyDec;
			int	zeroLoyaltyResistance=0;		// if the loyalty or resistance has reached 0

			if( nation_recno )		// if the town belongs to a nation
			{
				float originalLoyalty = loyalty;

				//---- decrease loyalty of the town people ----//

				if( has_linked_own_camp )		// if it is linked to one of its camp, the loyalty will decrease slower
					loyaltyDec = (float) 5 / population;
				else
					loyaltyDec = (float) 10 / population;

				loyaltyDec = min( loyaltyDec, (float) 1 );

				change_loyalty( -loyaltyDec * damagePoint / (20/ATTACK_SLOW_DOWN) );

				//--- if the resistance < 1, think_surrender() ---//

				if( loyalty < 1 )
				{
					if( think_surrender() )
						return;
				}

				//-------------------------------------------------------//
				// If the loyalty drops to 0 for the first time recently //
				// Order all units to stop attack the town               //
				//-------------------------------------------------------//

				if( loyalty < 1 && originalLoyalty >= 1 )
					unit_array.stop_attack_obj( base_obj_recno );

				zeroLoyaltyResistance = loyalty==0;
			}
			else						// if the town is an independent town
			{
				if( !attackerNationRecno )		// if independent units do not attack independent towns
					return;

				float originalResistance = resistance(attackerNationRecno);

				//---- decrease resistance of all races in the town ----//

				loyaltyDec = (float) 10 / population;		// decrease faster for independent towns than towns belonging to nations
				loyaltyDec = min( loyaltyDec, (float) 1 );

				resistance_array[attackerNationRecno-1] -= loyaltyDec * damagePoint / (20/ATTACK_SLOW_DOWN);

				if( resistance_array[attackerNationRecno-1] < 0 )
					resistance_array[attackerNationRecno-1] = (float) 0;

				//--- if the resistance of all the races are zero, think_change_nation() ---//

				float curResistance = resistance(attackerNationRecno);

				if( curResistance < 1 )
					think_surrender();

				//-------------------------------------------------------//
				// If the loyalty drops to 0 for the first time recently //
				// Order all units to stop attack the town               //
				//-------------------------------------------------------//

				if( curResistance < 1 && originalResistance >= 1 )
				{
					unit_array.stop_attack_obj( base_obj_recno, attackerNationRecno );
				}

				zeroLoyaltyResistance = curResistance==0;
			}

			//----- if resistance==0 && the attacking nation is a monster nation
			//-- some citizens will surrender, although the entire town will never surrender --//

			if( zeroLoyaltyResistance &&
				 attackerNationRecno && nation_array[attackerNationRecno]->is_monster() )
			{
				if( m.random(50)==0 )
				{
					int unitRecno = recruit(false, COMMAND_AUTO);		// false-not a wagon, just a unit

					//--- the unit surrender to the attacker ---//

					if( unitRecno )
						unit_array[unitRecno]->betray(attackerNationRecno);
				}
			}
		}
	}
	else	//--------- no wall, no people ----------//
	{
		// --------- continue attack will destroy the town ------- //

		// ##### begin Gilbert 4/2 ######//
		if( town_defender_count==0 )
			received_hit_count += damagePoint;
		else
			received_hit_count += damagePoint / 8;
		// ##### end Gilbert 4/2 ######//

		if( received_hit_count >= 4 * RECEIVED_HIT_PER_KILL )
		{
			if( nation_recno == nation_array.player_recno )
				news_array.town_destroyed(town_name(), center_x, center_y, attackerObj);

			if( town_recno )
				town_array.del_town(town_recno);
			else
				err_here();
			return;
		}
	}
}
//---------- End of function Town::being_attack_hit -----//


//--------- Begin of function Town::distribute_demand ---------//
//
// Distribute demands among
//
void Town::distribute_demand()
{
	#define MAX_ACTIVE_MARKET_COUNT     20

	//------------ define struct MarketInfo --------------//

	struct MarketGoodsInfo
	{
		FirmMarket* market_ptr[MAX_ACTIVE_MARKET_COUNT];
		float			total_supply;
		float			total_own_supply;
		short			market_count;
	};

	//------ scan for a firm to input raw materials --------//

	int					i, j;
	Firm*					firmPtr;
	MarketGoodsInfo	marketGoodsInfoArray[MAX_PRODUCT];
	MarketGoodsInfo*  marketGoodsInfo;
	float					thisSupply;
	MarketGoods*		marketGoods;

	memset( marketGoodsInfoArray, 0, sizeof(marketGoodsInfoArray) );

	//------- count the no. of market place that are near to this town ----//

	for( int linkedFirmId=0 ; linkedFirmId<linked_firm_count ; linkedFirmId++ )
	{
		firmPtr = firm_array[ linked_firm_array[linkedFirmId] ];

		if( firmPtr->firm_id != FIRM_MARKET )
			continue;

		if( linked_firm_enable_array[linkedFirmId] != LINK_EE )
			continue;

		firmPtr = firm_array[linked_firm_array[linkedFirmId]];

		//---------- process market -------------//

		for( i=0 ; i<MAX_PRODUCT ; i++ )
		{
//			marketGoods = ((FirmMarket*)firmPtr)->market_product_array[i];
			marketGoods = ((FirmMarket*)firmPtr)->market_product_array(i+1);
			marketGoodsInfo = marketGoodsInfoArray+i;

			if( marketGoods && marketGoodsInfo->market_count < MAX_ACTIVE_MARKET_COUNT )
			{
				thisSupply  = marketGoods->stock_qty;

				marketGoodsInfo->market_ptr[marketGoodsInfo->market_count] = (FirmMarket*) firmPtr;

				marketGoodsInfo->total_supply += thisSupply;

				if( firmPtr->nation_recno == nation_recno )				// vars for later use, so that towns will always try to buy goods from their own markets first.
					marketGoodsInfo->total_own_supply += thisSupply;

				marketGoodsInfo->market_count++;
			}
		}
	}

	//-- set the monthly demand of the town on each product --//

	float townDemand = (float) jobless_population * (float) PEASANT_GOODS_MONTH_DEMAND
							 + (float) worker_population() * (float) WORKER_GOODS_MONTH_DEMAND;

	float ownShareDemand;		// the share of demand for own markets

	//---------- sell goods now -----------//

	FirmMarket* firmMarket;

	for( i=0 ; i<MAX_PRODUCT ; i++ )
	{
		marketGoodsInfo = marketGoodsInfoArray+i;

		for( j=0 ; j<marketGoodsInfo->market_count ; j++ )
		{
			//----------------------------------//
			//
			// If the totalSupply < town demand:
			// a market's demand = its_supply + (town_demand-totalSupply) / market_count
			//
			// If the totalSupply > town demand:
			// a market's demand = town_demand * its_supply / totalSupply
			//
			//----------------------------------//

			firmMarket = marketGoodsInfo->market_ptr[j];

//			marketGoods = firmMarket->market_product_array[i];
			marketGoods = firmMarket->market_product_array(i+1);

			if( marketGoods )
			{
				//---- if the demand is larger than the supply -----//

				if( marketGoodsInfo->total_supply < townDemand )
				{
					marketGoods->month_demand += marketGoods->stock_qty +
														 (townDemand - marketGoodsInfo->total_supply)
														 / marketGoodsInfo->market_count;				// evenly distribute the excessive demand on all markets
				}
				else //---- if the supply is larger than the demand -----//
				{
					//--- towns always try to buy goods from their own markets first ---//

					ownShareDemand = min(townDemand, marketGoodsInfo->total_own_supply);

					if( firmMarket->nation_recno == nation_recno )
					{
						marketGoods->month_demand += ownShareDemand * marketGoods->stock_qty / marketGoodsInfo->total_own_supply;
					}
					else
					{
						marketGoods->month_demand += (townDemand-ownShareDemand) * marketGoods->stock_qty / marketGoodsInfo->total_supply;
					}
				}
			}
		}
	}
}
//----------- End of function Town::distribute_demand -----------//


//-------- Begin of function Town::can_recruit_peasant ------//
//
int Town::can_recruit_peasant()
{
	//----------------------------------------------------//
	// Cannot recruit when you have none of your own camps
	// linked to this town, but your enemies have camps
	// linked to it.
	//----------------------------------------------------//

	if( !has_linked_own_camp && has_linked_enemy_camp )
		return 0;

	if( recruitable_pop(1)==0 )
		return 0;

	err_when( recruitable_pop(1) < 0 );

	//---------------------------------//

	int minRecruitLoyalty = MIN_RECRUIT_LOYALTY;

	//--- for the AI, only recruit if the loyalty still stay at 30 after recruiting the unit ---//

	if( ai_town && nation_recno )
		minRecruitLoyalty += 3 + recruit_dec_loyalty(1, 0);		// 1-recruit one unit only, 0-don't actually decrease it, just return the loyalty to be decreased.

	return loyalty >= minRecruitLoyalty;
}
//---------- End of function Town::can_recruit_peasant --------//


//-------- Begin of function Town::can_train ------//
//
// Whether a linked structure like a fort and a espionage college can
// train a unit from this town. 
//
int Town::can_train()
{
	return recruitable_pop(1) > 0;
}
//---------- End of function Town::can_train --------//


//----- Begin of function Town::set_auto_collect_tax_loyalty -----//
//
// Note:
//
// Auto collect tax loyalty must always be higher than the auto grant
// loyalty. If the player has set it incorrectly, the program
// will automatically adjust it. e.g. If the player sets the auto
// grant level to 80 while the auto tax level is currently 60,
// the program will adjust auto tax level to 90. And if the player
// sets the auto grant level to 100, auto tax will be disabled.
//
void Town::set_auto_collect_tax_loyalty(int loyaltyLevel)
{
	auto_collect_tax_loyalty = loyaltyLevel;

	if( auto_collect_tax_loyalty > 0 &&
		 auto_grant_loyalty > 0 &&
		 auto_grant_loyalty >= auto_collect_tax_loyalty )
	{
		auto_grant_loyalty = auto_collect_tax_loyalty-10;
	}
}
//------ End of function Town::set_auto_collect_tax_loyalty -----//


//----- Begin of function Town::set_auto_grant_loyalty -----//
//
void Town::set_auto_grant_loyalty(int loyaltyLevel)
{
	auto_grant_loyalty = loyaltyLevel;

	if( auto_collect_tax_loyalty > 0 &&
		 auto_grant_loyalty > 0 &&
		 auto_grant_loyalty >= auto_collect_tax_loyalty )
	{
		auto_collect_tax_loyalty = auto_grant_loyalty+10;

		if( auto_collect_tax_loyalty > 100 )
			auto_collect_tax_loyalty = 0;					// disable auto collect tax if it's over 100
	}
}
//------ End of function Town::set_auto_grant_loyalty -----//


//----- Begin of function Town::can_grant_to_non_own_town -----//
//
// Return whether the given nation is allowed to grant to
// this independent town. Only when the nation has a camp (with
// an overseer) linked to the town, the nation will be able
// to grant the independent village.
//
// <int> grantNationRecno - the recno of the nation that
//									 is going to grant this town.
//
int Town::can_grant_to_non_own_town(int grantNationRecno)
{
	if( nation_recno == grantNationRecno )		// only for independent town
		return 0;

	if( !nation_array[grantNationRecno]->is_human() )		// only human nations can grant to independent towns
		return 0;

	if( nation_recno == 0 )		// independent town
	{
		return has_linked_camp( grantNationRecno, 1 );		// 1-only count camps with overseers
	}
	else  // for nation town, when the enemy doesn't have camps linked to it and the granting nation has camps linked to it
	{
		return has_linked_camp( nation_recno, 0 )==0 &&		// 0-count camps regardless of the presence of overseers
				 has_linked_camp( grantNationRecno, 1 );		// 1-only count camps with overseers
	}
}
//------ End of function Town::can_grant_to_non_own_town -----//


//----- Begin of function Town::grant_to_non_own_town -----//
//
// <int> grantNationRecno - the recno of the nation that
//									 is going to grant this town.
//
int Town::grant_to_non_own_town(int grantNationRecno, int remoteAction)
{
	if( !can_grant_to_non_own_town(grantNationRecno) )
		return 0;

	Nation* grantNation = nation_array[grantNationRecno];

	if( grantNation->cash < 0 )
		return 0;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_GRANT_INDEPENDENT, 2*sizeof(short) );
		shortPtr[0] = town_recno;
		shortPtr[1] = grantNationRecno;
		return 1;
	}

	//---- calculate the resistance to be decreased -----//

	int resistanceDec = IND_TOWN_GRANT_RESISTANCE_DECREASE - accumulated_enemy_grant_penalty/5;

	resistanceDec = max(3, resistanceDec);

	accumulated_enemy_grant_penalty += 10;

	//------ decrease the resistance of the independent villagers ------//

	//----- if this is an independent town ------//

	if( nation_recno==0 )
	{
		resistance_array[grantNationRecno-1] -= resistanceDec;

		if( resistance_array[grantNationRecno-1] < 0 )
			resistance_array[grantNationRecno-1] = (float) 0;
	}
	else  //----- if this is an nation town ------//
	{
		loyalty -= resistanceDec;

		if( loyalty < 0 )
			loyalty = (float) 0;
	}

	//----------- decrease cash ------------//

	grantNation->add_expense(EXPENSE_GRANT_OTHER_TOWN, (float)population * IND_TOWN_GRANT_PER_UNIT );

	return 1;
}
//------ End of function Town::grant_to_non_own_town -----//


//------- Begin of function Town::closest_own_camp -------//
//
// Return the firm recno of the camp that is closest to this town.
//
int Town::closest_own_camp()
{
	Firm *firmPtr;
	int  curDistance, minDistance=1000, closestFirmRecno=0;

	for( int i=linked_firm_count-1 ; i>=0 ; i-- )
	{
		firmPtr = firm_array[ linked_firm_array[i] ];

		if( !firmPtr->cast_to_FirmCamp() ||
			 firmPtr->nation_recno != nation_recno )
		{
			continue;
		}

		curDistance = m.points_distance( center_x, center_y,
						  firmPtr->center_x, firmPtr->center_y );

		if( curDistance < minDistance )
		{
			minDistance = curDistance;
			closestFirmRecno = firmPtr->firm_recno;
		}
	}

	return closestFirmRecno;
}
//-------- End of function Town::closest_own_camp ---------//


//------ Begin of function Town::possible_defender_combat_level ------//
//
// Return the total combat level of defenders who can possibly come
// out from the town.
//
// <int> attackerNationRecno - the nation recno of the attacker.
//										 if it is 0, then assume resistance is 0
//										 if it's an independent town.
//
int Town::possible_defender_combat_level(int attackerNationRecno)
{
	float thisResistance, resistanceDiff;

	if( nation_recno )
	{
		thisResistance = loyalty;
		resistanceDiff = thisResistance - MIN_NATION_DEFEND_LOYALTY;
	}
	else
	{
		if( attackerNationRecno )
		{
			thisResistance = resistance(attackerNationRecno);
			resistanceDiff = thisResistance - MIN_INDEPENDENT_DEFEND_LOYALTY;
		}
		else
		{
			thisResistance = 0;
			resistanceDiff = 0;
		}
	}

	int totalDefenseLevel = 0;

	if( resistanceDiff >= 0 )
	{
//		float resistanceDecPerDefender = thisResistance/4;		// each mobilized defender is counted as 5 points in Town::mobilize_defender()

//		int defenderCount = int(resistanceDiff / resistanceDecPerDefender)+1;		// no. of defenders will go out if you attack this town

		int defenderCount = int(resistanceDiff/3)+1;

		totalDefenseLevel += town_combat_level * 2 * defenderCount;	// *2 is defenseCombatLevel is actually the sum of hit points, not combat level

		totalDefenseLevel += (int)hit_points * 4;		// the hit points of the wall, 50 is the arrow attack even in the absence of a wall. * 2 for the stones throwing out from the wall
	}

	//---- if this is a nation town, also add combat level of linked camp ---//

	if( nation_recno )
	{
		for( int i=0 ; i<linked_firm_count ; i++ )
		{
			Firm* firmPtr = firm_array[ linked_firm_array[i] ];

			if( firmPtr->nation_recno == nation_recno &&
				 firmPtr->cast_to_FirmCamp() )
			{
				totalDefenseLevel += firmPtr->obj_power();
			}
		}
	}

	return totalDefenseLevel;
}
//------- End of function Town::possible_defender_combat_level -------//

//----------- Begin of function Town::town_name ----------------------//
//
// Return the name of the current town
//
char* Town::town_name()
{
	return town_res.get_name(town_name_id);
}
//----------- End of function Town::town_name ------------------------//

#ifdef DEBUG

//-------- Begin of function Town::resistance -------//
//
float Town::resistance(int nationRecno)
{
	err_when( nationRecno < 1 || nationRecno > MAX_NATION );

	return resistance_array[ nationRecno-1 ];
}
//-------- End of function Town::resistance --------//


//-------- Begin of function Town::target_resistance -------//
//
short Town::target_resistance(int nationRecno)
{
	err_when( nationRecno < 1 || nationRecno > MAX_NATION );

	return target_resistance_array[ nationRecno-1 ];
}
//-------- End of function Town::target_resistance --------//

#endif
