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

//Filename    : OTOWNINDP.CPP
//Description : Independent town thining

#include <OWORLD.H>
#include <OUNIT.H>
#include <ONEWS.H>
#include <OCONFIG.H>
#include <ORACERES.H>
#include <OGODRES.H>
#include <OTECHRES.H>
#include <OREGIONS.H>
#include <OFIRM.H>
#include <ONATION2.H>
#include <OINFO.H>
#include <OTOWN.H>
#include <OLOG.H>

//------- Begin of function Town::think_independent_town --------//
//
// Independent town thinking.
//
void Town::think_independent_town()
{
	if( population==0 )
	{
		town_array.del_town(town_recno);
		return;
   }

	if( rebel_recno )		// if this is a rebel town, its AI will be executed in Rebel::think_town_action()
		return;

#if defined(ENABLE_LOG)
	String logStr;

	logStr = "begin Town::think_independent_town, town_recno=";
	logStr += town_recno;
	LOG_MSG(logStr);
#endif

	//---- think about toggling town links ----//

	if( info.game_date%15 == town_recno%15 )
	{
		LOG_MSG(" Town::think_independent_set_link");
		think_independent_set_link();
		LOG_MSG(m.get_random_seed() );
	}

	//---- think about independent units join existing nations ----//

	if( info.game_date%60 == town_recno%60 )
	{
		LOG_MSG(" Town::think_independent_unit_join_nation");
		think_independent_unit_join_nation();
		LOG_MSG(m.get_random_seed() );
	}


	//------ think about building walls ------//

	if( info.game_date%90 == town_recno%90 )
	{
		think_independent_build_wall();
	}

	//----- think about form a new nation -----//

	if( info.game_date%365 == town_recno%365 )
	{
		LOG_MSG(" Town::think_independent_form_new_nation");
		think_independent_form_new_nation();
		LOG_MSG(m.get_random_seed() );
	}

	LOG_MSG("end Town::think_independent_town");
	LOG_MSG(m.get_random_seed());
}
//-------- End of function Town::think_independent_town ---------//


//------- Begin of function Town::think_independent_set_link --------//
//
// Independent town thinking.
//
void Town::think_independent_set_link()
{
	//---- think about working for foreign firms ------//

	int	linkStatus;
	Firm* firmPtr;

	for(int i=0; i<linked_firm_count; i++)
	{
		firmPtr = firm_array[linked_firm_array[i]];

		if( firmPtr->cast_to_FirmCamp() )		// a town cannot change its status with a military camp
			continue;

		//---- think about the link status ----//

		linkStatus = 0;

		if( firmPtr->nation_recno == 0 )		// if the firm is also an independent firm
			linkStatus = 1;

		if( resistance(firmPtr->nation_recno) <= INDEPENDENT_LINK_RESISTANCE )
			linkStatus = 1;

		//---- set the link status -------//

		toggle_firm_link( i+1, linkStatus, COMMAND_AI );
	}

	ai_link_checked = 1;
}
//-------- End of function Town::think_independent_set_link ---------//


//------ Begin of function Town::think_independent_form_new_nation ------//
//
// Independent town thinking.
//
int Town::think_independent_form_new_nation()
{
	if( m.random(10) > 0 )		// 1/10 chance to set up a new nation.
		return 0;

	//-------- check if the town is big enough -------//

	if( population < 30 )
		return 0;

	//---- don't form if the world is already densely populated ----//

	if( nation_array.all_nation_population > 60 * MAX_NATION )
		return 0;

	//----------------------------------------------//

	if( !nation_array.can_form_new_ai_nation() )		// this function looks at config parameters like config.new_town_emerge
		return 0;

	//----------------------------------------------//

	if( !can_independent_form_new_nation() )		// this function just returns whether this town can form a nation 
		return 0;

	return form_new_nation();
}
//------ End of function Town::think_independent_form_new_nation ------//


//------ Begin of function Town::can_independent_form_new_nation ------//
//
// Return whether this independent town can form a new nation.
//
int Town::can_independent_form_new_nation()
{
	if( nation_recno )
		return 0;

	//-- rebel has its own function in Rebel, this shouldn't be called --//

	if( rebel_recno )
		return 0;

	//------ if this is a slave town, don't form a new kingdom -----//

	if( is_pay_tribute_to_monster() )
		return 0;

	return 1;
}
//------ End of function Town::can_independent_form_new_nation ------//


//--------- Begin of function Town::form_new_nation ---------//
//
// This independent town forms a new nation.
//
// Return: <int> recno of the new nation.
//
int Town::form_new_nation()
{
	err_when( nation_recno );

	//---- create the king of the new nation ----//

	int unitId = race_res[race_id]->infantry_unit_id;
	int xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results
	SpriteInfo* spriteInfo = sprite_res[unit_res[unitId]->sprite_id];

	if( !world.locate_space( xLoc, yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return 0;

	//--------- create a new nation ---------//

	int nationRecno = nation_array.new_nation( NATION_HUMAN, NATION_AI, race_id, nation_array.random_unused_color() );

	Nation* nationPtr = nation_array[nationRecno];

	err_when( !nationRecno );

	//-------- create the king --------//

	int kingUnitRecno = unit_array.add_unit( unitId, nationRecno, RANK_KING, 100, xLoc, yLoc );

	Unit* kingUnit = unit_array[kingUnitRecno];

	kingUnit->skill.set_skill_level( 50+m.random(51) );		// leadership skill
	kingUnit->set_combat_level(70+m.random(31));

	nationPtr->set_king(kingUnitRecno, 1);		// 1-this is the first king of the nation

	dec_pop();

	//------ set the nation of the rebel town -----//

	err_when( rebel_recno );		// rebel has its own function in Rebel, this shouldn't be called

	set_nation(nationRecno);		// set the town at last because set_nation() will delete the Town object

	//------ increase the loyalty of the town -----//

	loyalty = (float) 70 + m.random(20);			// 70 to 90 initial loyalty 

	//--------- add news ----------//

	news_array.new_nation(nationRecno);

	//---- prevent the new kingdom from surrendering too soon ----//

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) || i==nationRecno )
			continue;

		nationPtr->get_relation(i)->set_never_accept_until_date(i, 365*2);	// at least two years later
	}

	//--- random extra beginning advantages -----//

	int mobileCount;

	switch( m.random(10) )
	{
		//**BUGHERE

//		case 1:		// knowledge of weapon in the beginning.
//			tech_res[ m.random(tech_res.tech_count)+1 ]->set_nation_tech_level(nationRecno, 1);
//			break;		

		case 2:		// random additional cash
			nationPtr->cash += m.random(5000);
			break;

		case 3:		// random additional food
			nationPtr->food += m.random(5000);
			break;

		case 4:		// random leader units
			mobileCount = m.random(5)+1;

			for( int i=0 ; i<mobileCount && recruitable_pop(0)>0 ; i++ )		// 0-don't recruit spies
			{
				int unitRecno = mobilize_town_people(1, 0);		// 1-dec pop, 0-don't mobilize spies

				if( unitRecno )
				{
					Unit* unitPtr = unit_array[unitRecno];

					unitPtr->skill.set_skill_level( 50 + m.random(50) );
					unitPtr->set_combat_level( 50 + m.random(50) );
				}
				else
					break;
			}
			break;
	}

	return nationRecno;
}
//----------- End of function Town::form_new_nation ---------//


//---- Begin of function Town::think_independent_unit_join_nation ----//
//
// Think having independent units joining existing nations.
//
int Town::think_independent_unit_join_nation()
{
	if( jobless_population==0 )
		return 0;

	if( recruitable_pop(0) == 0 ) 	// 0-don't count spies
		return 0;

	independent_unit_join_nation_min_rating -= 2;		// make it easier to join nation everytime it's called
																		// -2 each time, adding of 30 after a unit has been recruited and calling it once every 2 months make it a normal rate of joining once a year per town

	//------ think about which nation to turn towards -----//

	int    i, bestTownRecno=0, curRating;
	int	 bestRating=independent_unit_join_nation_min_rating;
	Nation *nationPtr;

	if( region_array[region_id]->region_stat_id == 0)
		return 0;

	RegionStat* regionStat = region_array.get_region_stat(region_id);

	//------- locate a town to go to -----------//

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->race_id != race_id )		// the town's race must be the same as the current town's race
			continue;

		if( townPtr->region_id != region_id )
			continue;

		if( !townPtr->nation_recno || nation_array.is_deleted(townPtr->nation_recno) )
			continue;

		//-----------------------------------//

		nationPtr = nation_array[townPtr->nation_recno];

		if( nationPtr->cash <= 0 )
			continue;

		if( info.game_date < nationPtr->last_independent_unit_join_date + 90 )		// don't join too frequently, at most 3 months a unit
			continue;

		//----- calculate the rating of the nation -----//

		curRating = (int) nationPtr->reputation + nationPtr->overall_rating;

		if( curRating > bestRating )
		{
			bestRating 	  = curRating;
			bestTownRecno = i;
		}
	}

	//--------------------------------------------//

	if( !bestTownRecno )
		return 0;

	if( !independent_unit_join_nation(bestTownRecno) )
		return 0;

	//--- set a new value to independent_unit_join_nation_min_rating ---//

	independent_unit_join_nation_min_rating = bestRating + 100 + m.random(30);		// reset it to a higher rating

	if( independent_unit_join_nation_min_rating < 100 )
		independent_unit_join_nation_min_rating = 100;

	return 1;
}
//----- End of function Town::think_independent_unit_join_nation -----//


//---- Begin of function Town::independent_unit_join_nation ----//
//
// <int> toTownRecno - recno of the town the unit should go to
//
int Town::independent_unit_join_nation(int toTownRecno)
{
	//----- mobilize a villager ----//

	int unitRecno = mobilize_town_people(1, 0);		// 1-dec population after mobilizing the unit, 0-don't mobilize spies

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	//------ change nation now --------//

	int toNationRecno = town_array[toTownRecno]->nation_recno;

	if( !unitPtr->betray(toNationRecno) )
		return 0;

	//-------- set last_independent_unit_join_date --------//

	nation_array[toNationRecno]->last_independent_unit_join_date = info.game_date;

	return 1;
}
//----- End of function Town::independent_unit_join_nation -----//



//------- Begin of function Town::think_independent_build_wall --------//
//
int Town::think_independent_build_wall()
{
	if( target_wall_level > current_wall_level ||	// if one is already being trained
		 current_wall_level == MAX_WALL_LEVEL )
	{
		return 0;
	}

	int randomChance;

	switch( config.independent_town_resistance )
	{
		case OPTION_LOW:
			randomChance = 5;
			break;

		case OPTION_MODERATE:
			randomChance = 15;
			break;

		case OPTION_HIGH:
			randomChance = 30;
			break;
	}

	randomChance += population / 2;

	if( m.random(200) < randomChance )
		set_target_wall_level(current_wall_level+1, COMMAND_AI);

	return 1;
}
//-------- End of function Town::think_independent_build_wall ---------//

