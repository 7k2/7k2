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

//Filename   : OU_GOD2.CPP
//Description: God Unit - AI functions

#include <OGODRES.H>
#include <ORACERES.H>
#include <OF_CAMP.H>
#include <OF_TRAIN.H>
#include <OU_GOD.H>

//------- Begin of function UnitGod::process_ai --------//

void UnitGod::process_ai()
{
	if( !is_all_stop() )
		return;

	if( info.game_date%7 != sprite_recno%7 )
		return;

	switch( god_id )
	{
	case GOD_NORMAN:
		think_norman_god();		// attack
		break;

	case GOD_VIKING:
		think_viking_god();	// attack
		break;

	case GOD_CELTIC:
		think_celt_god();		// attack
		break;

	case GOD_ROMAN:
		think_roman_god();		// as same as zulu god, add leadership to roman leader
		break;

	case GOD_GREEK:
		think_greek_god();		// protection
		break;

	case GOD_CARTH:
		think_carth_god();		// earthquake
		break;

	case GOD_CHINESE:
		think_chinese_god();	// attack
		break;

	case GOD_JAPANESE:
		think_japanese_god();	// clear spy
		break;

	case GOD_MONGOL:
		think_mongol_god();	// betray to your nation
		break;

	case GOD_EGYPTIAN:
		think_egyptian_god();	// same as old egyptian god
		break;

	case GOD_PERSIAN:
		think_persian_god();	// reveal building info
		break;

	case GOD_INDIAN:
		think_indian_god();	// add tornado
		break;
	}
}
//------- End of function UnitGod::process_ai --------//


// ------ Begin of function UnitGod::think_norman_god ------//
//
void UnitGod::think_norman_god()
{
	int targetRecno, targetXLoc, targetYLoc;

	if( (targetRecno = think_god_attack_target(targetXLoc, targetYLoc)) )
	{
		attack(targetRecno, 1 );
	}

	// think resign itself

	else if( (int)hit_points /2 < max_hit_points()
		&& nation_ptr()->peaceful_days() > 100
		&& m.random(max_hit_points()) > 2 * (int) hit_points )
	{
		resign(COMMAND_AI);
		return;
	}
}
// ------ End of function UnitGod::think_norman_god ------//


// ------ Begin of function UnitGod::think_viking_god ------//
//
void UnitGod::think_viking_god()
{
	int targetRecno, targetXLoc, targetYLoc;

	if( (targetRecno = think_god_attack_target(targetXLoc, targetYLoc)) )
	{
		attack(targetRecno, 1 );
	}

	// think resign itself

	else if( (int)hit_points /2 < max_hit_points()
		&& nation_ptr()->peaceful_days() > 100
		&& m.random(max_hit_points()) > 2 * (int) hit_points )
	{
		resign(COMMAND_AI);
		return;
	}
}
// ------ End of function UnitGod::think_viking_god ------//


// ------ Begin of function UnitGod::think_celt_god ------//
//
void UnitGod::think_celt_god()
{
	int targetRecno, targetXLoc, targetYLoc;

	if( (targetRecno = think_god_attack_target(targetXLoc, targetYLoc)) )
	{
		attack(targetRecno, 1 );
	}

	// think resign itself

	else if( (int)hit_points /2 < max_hit_points()
		&& nation_ptr()->peaceful_days() > 100
		&& m.random(max_hit_points()) > 2 * (int) hit_points )
	{
		resign(COMMAND_AI);
		return;
	}
}
// ------ End of function UnitGod::think_celt_god ------//


// ------ Begin of function UnitGod::think_chinese_god ------//
//
void UnitGod::think_chinese_god()
{
	int targetRecno, targetXLoc, targetYLoc;

	if( (targetRecno = think_god_attack_target(targetXLoc, targetYLoc)) )
	{
		attack(targetRecno, 1 );
	}

	// think resign itself

	else if( (int)hit_points /2 < max_hit_points()
		&& nation_ptr()->peaceful_days() > 100
		&& m.random(max_hit_points()) > 2 * (int) hit_points )
	{
		resign(COMMAND_AI);
		return;
	}
}
// ------ End of function UnitGod::think_chinese_god ------//


//------- Begin of function UnitGod::think_roman_god --------//
//
void UnitGod::think_roman_god()
{
	//------- there is no action, now think a new one ------//

	Nation* ownNation = nation_array[nation_recno];
	FirmCamp*	  firmCamp;
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=ownNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		firmCamp = firm_array[ ownNation->ai_camp_array[i] ]->cast_to_FirmCamp();

		curRating = 0;

		Unit* unitPtr;
		if( firmCamp->overseer_recno
			&& (unitPtr = unit_array[firmCamp->overseer_recno])
			// ##### patch begin Gilbert 16/2 ########//
			// &&	unitPtr->race_id == RACE_ROMAN			// only consider ROMAN leader
			&& unitPtr->is_human()
			// ##### patch end Gilbert 16/2 ########//
			&& unitPtr->skill_level() <= 70)
		{
			if( unitPtr->rank_id == RANK_KING )
				curRating += 5000;		// weak king need leadership very much

			if( unitPtr->skill_level() >= 40 )
				curRating += 5000 - (unitPtr->skill_level() - 40) * 60;	// strong leader need not be enhanced
			else
				curRating += 5000 - (40 - unitPtr->skill_level() ) * 80;		// don't add weak leader

			// calculat the benefits to his soldiers
			for( int j=firmCamp->soldier_count-1 ; j>=0 ; j-- )
			{
				Soldier* soldierPtr = firmCamp->soldier_array + j;
				if( soldierPtr->race_id == RACE_ROMAN )
					curRating += (unitPtr->combat_level() - soldierPtr->combat_level() )*2;
				else
					curRating += unitPtr->combat_level() - soldierPtr->combat_level() ;
			}

			if( curRating > bestRating )
			{
				bestRating = curRating;
				targetXLoc = firmCamp->center_x;
				targetYLoc = firmCamp->center_y;
			}
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_roman_god --------//


//------- Begin of function UnitGod::think_greek_god --------//
//
void UnitGod::think_greek_god()
{
	int castRange = god_res[god_id]->cast_power_range;

	// scan current location

	int targetXLoc = next_x_loc();
	int targetYLoc = next_y_loc();
	int bestWeight = 0;

	{
		int weight = 0;

		// count nearby attacking unit, not under protection

		int x1 = next_x_loc() - castRange;
		if( x1 < 0 )
			x1 = 0;
		int y1 = next_y_loc() - castRange;
		if( y1 < 0 )
			y1 = 0;
		int x2 = next_x_loc() + castRange;
		if( x2 >= MAX_WORLD_X_LOC )
			x2 = MAX_WORLD_X_LOC-1;
		int y2 = next_y_loc() + castRange;
		if( y2 >= MAX_WORLD_Y_LOC )
			y2 = MAX_WORLD_Y_LOC-1;

		for( int y = y1; y <= y2; ++y )
		{
			Location *locPtr = world.get_loc(x1, y);
			for( int x = x1; x <= x2; ++x, ++locPtr )
			{
				int unitRecno;
				Unit *unitPtr;
				if( (unitRecno = locPtr->unit_recno(UNIT_LAND))
					&& (unitPtr = unit_array[unitRecno])
					&& unitPtr->nation_recno == nation_recno
					&& unitPtr->cur_order.mode == UNIT_ATTACK
					&& unitPtr->invulnerable_day_count < 5	)
				{
					float ratio = unitPtr->hit_points / unitPtr->max_hit_points();
					if( ratio >= 0.5f )		// heavy on mid hit point
						weight += 5 + int( 5.0f * (1.0f - ratio));
					else
						weight += 5 + int( 5.0f * ratio);

					if( unitPtr->rank_id == RANK_KING )
						weight += 5;
					else if( unitPtr->rank_id == RANK_GENERAL )
						weight += 2;
				}
			}
		}

		if( bestWeight < weight )
		{
			bestWeight = weight;
			targetXLoc = next_x_loc();
			targetYLoc = next_y_loc();
		}
	}

	if( bestWeight >= 20  && bestWeight+ai_para >= 40 )
	{
		ai_para = 0;
		go_cast_power(targetXLoc, targetYLoc, COMMAND_AI);		// 1 - cast power type
		return;
	}


	// scan around general

	bestWeight = 0;
	int isTargetLoc = 0;

	short *generalArray = nation_array[nation_recno]->ai_general_array;
	short generalCount = nation_array[nation_recno]->ai_general_count;

	for( int i = generalCount; i >= 0; --i )
	{
		int weight = 0;
		Unit *generalPtr = NULL;
		if( i == 0 )		// i = 0 for king
		{
			if( !unit_array.is_deleted( nation_array[nation_recno]->king_unit_recno) )
				generalPtr = unit_array[nation_array[nation_recno]->king_unit_recno];
		}
		else
		{
			if( !unit_array.is_deleted( generalArray[i-1]) )
				generalPtr = unit_array[generalArray[i-1]];
		}

		if( !generalPtr || !generalPtr->is_visible() )
			continue;

		// count nearby attacking unit, not under protection

		int x1 = generalPtr->next_x_loc() - castRange;
		if( x1 < 0 )
			x1 = 0;
		int y1 = generalPtr->next_y_loc() - castRange;
		if( y1 < 0 )
			y1 = 0;
		int x2 = generalPtr->next_x_loc() + castRange;
		if( x2 >= MAX_WORLD_X_LOC )
			x2 = MAX_WORLD_X_LOC-1;
		int y2 = generalPtr->next_y_loc() + castRange;
		if( y2 >= MAX_WORLD_Y_LOC )
			y2 = MAX_WORLD_Y_LOC-1;

		for( int y = y1; y <= y2; ++y )
		{
			Location *locPtr = world.get_loc(x1, y);
			for( int x = x1; x <= x2; ++x, ++locPtr )
			{
				int unitRecno;
				Unit *unitPtr;
				if( (unitRecno = locPtr->unit_recno(UNIT_LAND))
					&& (unitPtr = unit_array[unitRecno])
					&& unitPtr->nation_recno == nation_recno
					&& unitPtr->cur_order.mode == UNIT_ATTACK
					&& unitPtr->invulnerable_day_count < 5	)
				{
					float ratio = unitPtr->hit_points / unitPtr->max_hit_points();
					if( ratio >= 0.5f )		// heavy on mid hit point
						weight += 5 + int( 5.0f * (1.0f - ratio));
					else
						weight += 5 + int( 5.0f * ratio);

					if( unitPtr->rank_id == RANK_KING )
						weight += 5;
					else if( unitPtr->rank_id == RANK_GENERAL )
						weight += 2;
				}
			}
		}

		// distance
		int dist = m.points_distance( next_x_loc(), next_y_loc(),
			generalPtr->next_x_loc(), generalPtr->next_y_loc() );
		weight -= dist / 8;

		if( bestWeight < weight )
		{
			bestWeight = weight;
			// go the the attack target, instead of the unit
			if( generalPtr->cur_order.mode == UNIT_ATTACK )
			{
				isTargetLoc = 1;
				targetXLoc = generalPtr->cur_order.loc_x;
				targetYLoc = generalPtr->cur_order.loc_y;
			}
			else
			{
				isTargetLoc = 0;
				targetXLoc = generalPtr->next_x_loc();
				targetYLoc = generalPtr->next_y_loc();
			}
		}
	}

	if( bestWeight >= 20 && bestWeight+ai_para >= 40 )
	{
		// if close to god, cast
		// else go to there
		if( m.points_distance( next_x_loc(), next_y_loc(), targetXLoc, targetYLoc ) <= 20 )
		{
			if( !isTargetLoc )
			{
				// go the unit
				ai_para = 0;
				go_cast_power(targetXLoc, targetYLoc, COMMAND_AI);		// 1 - cast power type
			}
			else
			{
				// go to the unit destination
				ai_para += 5;
				move(targetXLoc, targetYLoc);
			}
			return;
		}
		else if( bestWeight+ai_para > m.random(100) )
		{
			ai_para += 5;
			move(targetXLoc, targetYLoc);
			return;
		}
	}

	ai_para+=2;
	if( ai_para > 10)
	{
		ai_para = 10;

		// think resign itself

		if( bestWeight < 10 && (int)hit_points /2 < max_hit_points()
			&& nation_ptr()->peaceful_days() > 100
			&& m.random(max_hit_points()) > 2 * (int) hit_points )
		{
			resign(COMMAND_AI);
			return;
		}
	}
}
//------- End of function UnitGod::think_greek_god --------//


//------- Begin of function UnitGod::think_carth_god -------//
//
void UnitGod::think_carth_god()
{
	int castRadius = 32;

	int bestWeight = 0;
	int targetXLoc;
	int targetYLoc;

	// analyze 20 places and select one with full of enemy

	for( int i = 20; i > 0; --i )
	{
		int weight = 0;
		int x1 = m.random( MAX_WORLD_X_LOC - castRadius - castRadius );
		int y1 = m.random( MAX_WORLD_Y_LOC - castRadius - castRadius );
		int x2 = x1 + castRadius + castRadius;
		int y2 = y1 + castRadius + castRadius;

		for( int y = y1; y <= y2; ++y )
		{		
			Location *locPtr = world.get_loc(x1, y);
			for( int x = x1; x <= x2; ++x, ++locPtr )
			{
				int targetRecno = locPtr->base_obj_recno(UNIT_LAND);
				if( !targetRecno || base_obj_array.is_deleted(targetRecno) )
					continue;

				BaseObj *targetObj = base_obj_array[targetRecno];

				// especially for building, count the four corners only
				// so that weight count for at most 4 times

				if( x != targetObj->obj_loc_x1() && x != targetObj->obj_loc_x2() )
					continue;
				if( y != targetObj->obj_loc_y1() && y != targetObj->obj_loc_y2() )
					continue;

				if( targetObj->nation_recno == nation_recno )
					weight -= 4;
				else if( nation_recno && targetObj->nation_recno )
				{
					switch( nation_ptr()->get_relation_status(targetObj->nation_recno) )
					{
					case RELATION_HOSTILE:   weight += 3; break;
					case RELATION_TENSE:     weight += 2; break;
					case RELATION_NEUTRAL:   weight += 1; break;
					case RELATION_FRIENDLY:  weight -= 1; break;
					case RELATION_ALLIANCE:  weight -= 1; break;
					}
				}
				else
				{
					weight += 1;
				}
			}
		}

		// distance factor

		int dist = m.points_distance( next_x_loc(), next_y_loc(), (x1+x2)/2, (y1+y2)/2 );
		weight -= dist/8;

		if( bestWeight < weight )
		{
			bestWeight = weight;
			targetXLoc = (x1+x2)/2;
			targetYLoc = (y1+y2)/2;
		}
	}

	if( bestWeight >= 10 && bestWeight+ai_para >= 20 && bestWeight+ai_para >= m.random(50) )
	{
		ai_para = 0;
		go_cast_power(targetXLoc, targetYLoc, COMMAND_AI);		// 1 - cast power type
	}
	else
	{
		ai_para++;
		if( ai_para > 4 )
			ai_para = 4;
	}

}
//------- End of function UnitGod::think_carth_god -------//


// ------ begin of function UnitGod::think_japanese_god ------//
//
void UnitGod::think_japanese_god()
{
	int bestWeight = 0;
	int bestRecno;
	int targetXLoc;
	int targetYLoc;

	// ai_para store three low byte of base_obj_recno which casted
	// (MSB) [count] [recno 2] [recno 1] [recno 0] (LSB)
	// so it won't cast on the same camp again

	struct
	{
		unsigned char array[sizeof(ai_para)-1];
		char count;
	} aiParaStruct;
	err_when( sizeof(aiParaStruct) != sizeof(ai_para) );
	memcpy( &aiParaStruct, &ai_para, sizeof(ai_para) );

	for(int i = base_obj_array.size(); i > 0; --i )
	{
		if( base_obj_array.is_deleted(i) )
			continue;

		BaseObj *baseObj = base_obj_array[i];

		if( baseObj->nation_recno != nation_recno )
			continue;

		err_when( sizeof(ai_para)-1 > 3 );
		if( aiParaStruct.count >= 1 && (i & 0xff) == aiParaStruct.array[0] )
			continue;
		if( aiParaStruct.count >= 2 && (i & 0xff) == aiParaStruct.array[1] )
			continue;
		if( aiParaStruct.count >= 3 && (i & 0xff) == aiParaStruct.array[2] )
			continue;

		int weight = 0;
		if( baseObj->cast_to_Town() )
		{
			Town *townPtr = baseObj->cast_to_Town();
			if( townPtr->under_construction || info.game_date - townPtr->setup_date < 365 )		// don't care building built less than one year
				continue;
			if( townPtr->population <= 0 )
				continue;
			weight += 5 + townPtr->population / 10;		// large population large change
			weight += m.random(5);
		}
		else if( baseObj->cast_to_Firm() )
		{
			Firm *firmPtr = baseObj->cast_to_Firm();
			if( firmPtr->under_construction || info.game_date - firmPtr->setup_date < 365 )		// don't care building built less than one year
				continue;

			switch( firmPtr->firm_id )
			{
			case FIRM_CAMP:
			case FIRM_FORT:
			// no need to clear FIRM_LAIR
				if( firmPtr->cast_to_FirmCamp() )
				{
					// range of weight small because we don't know
					// which firm has cleared spy.

					FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

					if( firmCamp->soldier_count > 0 || firmCamp->overseer_recno )
					{
						weight += 5 + firmCamp->soldier_count;
						if( firmCamp->overseer_recno )
						{
							if( unit_array[firmCamp->overseer_recno]->rank_id == RANK_KING )
								weight += 3;
							else
								weight += 2;
						}

						weight += m.random(5);
					}
				}
				break;

			case FIRM_SPECIAL:
			// no need to clear FIRM_SPY
				if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();

					if( firmTrain->trainee_count > 0 )
					{
						// less important than firm camp/fort
						weight += 1 + firmTrain->trainee_count;
						weight += m.random(5);
					}
				}
				break;
			}
		}

		if( !baseObj->is_visible() )
			continue;

//		weight -= area_distance( baseObj ) / 16;

		if( bestWeight < weight )
		{
			bestWeight = weight;
			bestRecno = i;
			targetXLoc = (baseObj->obj_loc_x1() + baseObj->obj_loc_x2())/2;
			targetYLoc = (baseObj->obj_loc_y1() + baseObj->obj_loc_y2())/2;
		}
	}

	if( bestWeight >= 10 && bestWeight > m.random(20) )
	{
		// update ai_para
		if( aiParaStruct.count < sizeof(ai_para)-1 )
		{
			aiParaStruct.array[aiParaStruct.count] = bestRecno & 0xff;
			aiParaStruct.count++;
		}

		go_cast_power( targetXLoc, targetYLoc, COMMAND_AI );
	}

	// ------- compose ai_para again -------//

	memcpy( &ai_para, &aiParaStruct, sizeof(ai_para) );
}
// ------ end of function UnitGod::think_japanese_god ------//


// ------ begin of function UnitGod::think_mongol_god -----//
//
void UnitGod::think_mongol_god()
{
	// check if any standing target nearby

	const shortRange = 5;
	int x1, y1, x2, y2, x, y;

	x1 = next_x_loc() - shortRange;
	y1 = next_y_loc() - shortRange;
	x2 = next_x_loc() + shortRange;
	y2 = next_y_loc() + shortRange;
	if( x1 < 0 )
		x1 = 0;
	if( y1 < 0 )
		y1 = 0;
	if( x2 >= MAX_WORLD_X_LOC )
		x2 = MAX_WORLD_X_LOC-1;
	if( y2 >= MAX_WORLD_Y_LOC )
		y2 = MAX_WORLD_Y_LOC-1;

	int bestUnitRecno = 0;
	int bestWeight = 0;
	int targetXLoc, targetYLoc;
	int unitRecno;

	for( y = y1; y <= y2; ++y )
	{
		Location *locPtr = world.get_loc(x1,y);
		for( x = x1; x <= x2; ++x, ++locPtr )
		{
			unitRecno = locPtr->unit_recno(UNIT_LAND);
			Unit *unitPtr;
			if( unitRecno && !unit_array.is_deleted(unitRecno) 
				&& (unitPtr = unit_array[unitRecno])
				&& unitPtr->nation_recno != 0
				&& !unitPtr->is_nation(nation_recno)		// instead of unitPtr->nation_recno != nation_recno
				&& (unitPtr->cur_action == SPRITE_IDLE || unitPtr->cur_action == SPRITE_ATTACK)
				&& unitPtr->rank_id != RANK_KING
				&& unitPtr->race_id )
			{
				int weight = int(unitPtr->combat_level() + unitPtr->hit_points
					+ 3 * unitPtr->skill_level());
				if( bestWeight < weight )
				{
					bestWeight = weight;
					bestUnitRecno = unitRecno;
					targetXLoc = unitPtr->next_x_loc();
					targetYLoc = unitPtr->next_y_loc();
				}
			}
		}
	}

	if( bestWeight >= CITIZEN_COMBAT_LEVEL*3 
		&& bestWeight+ai_para >= 100 && bestWeight+ai_para > m.random(300) )
	{
		go_cast_power( targetXLoc, targetYLoc, COMMAND_AI, bestUnitRecno );
		ai_para = 0;
		return;
	}

	// wander to find new target

	bestWeight = 0;
	for(unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
	{
		Unit *unitPtr;
		if( !unit_array.is_deleted(unitRecno) 
			&& (unitPtr = unit_array[unitRecno])
			&& unitPtr->is_visible()
			&& unitPtr->mobile_type == UNIT_LAND
			&& unitPtr->nation_recno != 0
			&& !unitPtr->is_nation(nation_recno)		// instead of unitPtr->nation_recno != nation_recno
			&& unitPtr->cur_action == SPRITE_IDLE
			&& unitPtr->rank_id != RANK_KING
			&& unitPtr->race_id )
		{
			int weight = int( unitPtr->combat_level() + unitPtr->hit_points
				+ 3 * unitPtr->skill_level());
			weight -= area_distance( unitPtr ) / 4;

			if( bestWeight < weight )
			{
				bestWeight = weight;
				targetXLoc = unitPtr->next_x_loc();
				targetYLoc = unitPtr->next_y_loc();
			}
		}
	}

	if( bestWeight >= CITIZEN_COMBAT_LEVEL*3 && bestWeight+ai_para >= 80 && bestWeight+ai_para > m.random(150) )
	{
		move( targetXLoc, targetYLoc );
		ai_para += 10;			// so it is easier to cast after move
	}
	else
	{
		ai_para += 5;			// failure accumulated
		if( ai_para > 50 )
			ai_para = 50;
	}
}
// ------ end of function UnitGod::think_mongol_god -----//


//------- Begin of function UnitGod::think_egyptian_god --------//
void UnitGod::think_egyptian_god()
{
	Nation* ownNation = nation_array[nation_recno];
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		//------ only cast on own nations ------//

		if( townPtr->nation_recno != nation_recno )
			continue;

		//------ calculate the rating of the firm -------//

		// ###### patch begin Gilbert 20/1 ########//
		if( townPtr->population > MAX_TOWN_GROWTH_POPULATION - 10 - 5 )
		// ###### patch end Gilbert 20/1 ########//
			continue;

		// maximize the total loyalty gain.
		curRating = 5 * (int)townPtr->loyalty;

		// calc rating on the number of people
		if( townPtr->population >= MAX_TOWN_GROWTH_POPULATION/2 )
			curRating -= (townPtr->population - MAX_TOWN_GROWTH_POPULATION/2) * 300 / MAX_TOWN_GROWTH_POPULATION;
		else
			curRating -= (MAX_TOWN_GROWTH_POPULATION/2 - townPtr->population) * 300 / MAX_TOWN_GROWTH_POPULATION;

		if( curRating > bestRating )
		{
			bestRating = curRating;
			targetXLoc = townPtr->center_x;
			targetYLoc = townPtr->center_y;
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, COMMAND_AI);
	}
}
//------- End of function UnitGod::think_egyptian_god --------//


//------- Begin of function UnitGod::think_persian_god -------//
//
// reveal info of firm
//
void UnitGod::think_persian_god()
{
	// start from a random firm

	int firmArraySize = firm_array.size();
	int firmRecno = 1 + firmArraySize * (info.game_day-1) / 30;

	struct
	{
		unsigned char array[sizeof(ai_para)-1];
		char count;
	} aiParaStruct;
	err_when( sizeof(aiParaStruct) != sizeof(ai_para) );
	memcpy( &aiParaStruct, &ai_para, sizeof(ai_para) );

	for( int i = firmArraySize; i > 0; --i, ++firmRecno )
	{
		if( firmRecno > firmArraySize )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		err_when( sizeof(ai_para)-1 > 3 );
		if( aiParaStruct.count >= 1 && (firmRecno & 0xff) == aiParaStruct.array[0] )
			continue;
		if( aiParaStruct.count >= 2 && (firmRecno & 0xff) == aiParaStruct.array[1] )
			continue;
		if( aiParaStruct.count >= 3 && (firmRecno & 0xff) == aiParaStruct.array[2] )
			continue;


		// never read firmPtr->reveal_info as it is local player variable
		Firm *firmPtr = firm_array[firmRecno];

		// skip own firm
		if( firmPtr->nation_recno != nation_recno
			&& (firmPtr->nation_recno == 0 
			|| nation_ptr()->get_relation_status(firmPtr->nation_recno) <= RELATION_NEUTRAL ) )
		{
			go_cast_power( firmPtr->center_x, firmPtr->center_y, COMMAND_AI );
			break;
		}
	}

	// ------- compose ai_para again -------//

	memcpy( &ai_para, &aiParaStruct, sizeof(ai_para) );
}
//------- End of function UnitGod::think_persian_god -------//


//------- Begin of function UnitGod::think_indian_god --------//

void UnitGod::think_indian_god()
{
	int targetXLoc, targetYLoc;

	if( think_god_attack_target(targetXLoc, targetYLoc) )
	{
		go_cast_power(targetXLoc+1, targetYLoc+1, COMMAND_AI);		// 2 - cast power type
	}

	// think resign itself

	else if( (int)hit_points /2 < max_hit_points()
		&& nation_ptr()->peaceful_days() > 100
		&& m.random(max_hit_points()) > 2 * (int) hit_points )
	{
		resign(COMMAND_AI);
		return;
	}

}
//------- End of function UnitGod::think_indian_god --------//


/*
//------- Begin of function UnitGod::think_god_attack_target --------//
//
// <int&> targetXLoc, targetYLoc - reference vars for returning the
//											  location of the target.
//
// return: <int> non-zero - target base obj recno selected
//					  0 - no target selected
//
int UnitGod::think_god_attack_target(int& targetXLoc, int& targetYLoc)
{
	Firm* 	firmPtr;
	Nation* 	ownNation = nation_array[nation_recno];
	int		curXLoc=next_x_loc(), curYLoc=next_y_loc();
	int   	totalFirm = firm_array.size();
	int   	firmRecno = m.random(totalFirm)+1;

	for( int i=totalFirm ; i>0 ; i-- )
	{
		if( ++firmRecno > totalFirm )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr = firm_array[firmRecno];

		// ------- do not attack same nation or indepedent nation --------//

		if( firmPtr->nation_recno == nation_recno || !firmPtr->nation_recno )
			continue;

		//-------- only attack enemies ----------//

		if( ownNation->get_relation(firmPtr->nation_recno)->status != RELATION_HOSTILE )
			continue;

		if( !can_attack_target(firmPtr->base_obj_recno) )
			continue;

		//---- only attack enemy base and camp ----//

		if( !firmPtr->cast_to_FirmBase() && !firmPtr->cast_to_FirmCamp() )
		{
			continue;
		}

		//------- attack now --------//

		targetXLoc = firmPtr->loc_x1;
		targetYLoc = firmPtr->loc_y1;

		return firmPtr->base_obj_recno;
	}

	//----- if there is no enemy to attack, attack Fryhtans ----//

	for( i=totalFirm ; i>0 ; i-- )
	{
		if( ++firmRecno > totalFirm )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr = firm_array[firmRecno];

		if( !can_attack_target(firmPtr->base_obj_recno) )
			continue;

		if( !firmPtr->nation_recno && firmPtr->cast_to_FirmLair() )
		{
			targetXLoc = firmPtr->loc_x1;
			targetYLoc = firmPtr->loc_y1;

			return firmPtr->base_obj_recno;
		}
	}

	//---------------------------------------------------//

	return 0;
}
//------- End of function UnitGod::think_god_attack_target --------//
*/

//------- Begin of function UnitGod::think_god_attack_target --------//
//
// <int&> targetXLoc, targetYLoc - reference vars for returning the
//											  location of the target.
//
// return: <int> non-zero - target base obj recno selected
//					  0 - no target selected
//
int UnitGod::think_god_attack_target(int& targetXLoc, int& targetYLoc)
{
	// assist attacking unit of same nation

	int unitArraySize = unit_array.size();
	int unitRecno = m.random(unitArraySize) + 1;
	int bestUnitRecno = 0;
	int bestWeight = 0;
	int bestTargetRecno;

	for( int i = unit_array.size(); i > 0; --i, ++unitRecno )
	{
		if( unitRecno > unitArraySize )
			unitRecno = 1;

		if( unit_array.is_deleted(unitRecno) )
			continue;

		Unit *unitPtr = unit_array[unitRecno];

		if( unitPtr->nation_recno != nation_recno	|| !unitPtr->is_visible() )
			continue;

		int weight = 0;
		int targetObjRecno = 0;

		if( unitPtr->cur_order.mode == UNIT_ATTACK 
			&& (targetObjRecno = unitPtr->cur_order.para)
			&& !base_obj_array.is_deleted(targetObjRecno) )
		{
			BaseObj *targetObj = base_obj_array[targetObjRecno];
			if( targetObj->is_visible() )
			{
				if( targetObj->cast_to_Unit() )
				{
					// ----- see if any more follower -------//

					Unit *targetUnit = targetObj->cast_to_Unit();
					TeamInfo* teamInfo;
					if( (teamInfo = targetUnit->team_info)
						|| !unit_array.is_deleted(targetUnit->leader_unit_recno) 
							&& (teamInfo = unit_array[targetUnit->leader_unit_recno]->team_info) )
					{
						for( int j = 0; j < teamInfo->member_count; ++j )
						{
							int memberRecno = teamInfo->member_unit_array[j];
							if( !unit_array.is_deleted(memberRecno) )
							{
								weight += (int) unit_array[memberRecno]->hit_points;
							}
						}
					}
					else
					{
						weight -= (int) targetObj->hit_points;		// clear weight
					}

				}
				else if( targetObj->cast_to_Firm() )
				{
					weight = (int) targetObj->hit_points;
					if( targetObj->cast_to_Firm()->cast_to_FirmCamp() )
					{
						weight += (int) targetObj->hit_points;
					}
				}
				else if( targetObj->cast_to_Town() )
				{
					weight += (int) targetObj->hit_points		// wall strength
						+ (targetObj->cast_to_Town()->jobless_population + 
						+ targetObj->cast_to_Town()->town_defender_count) * 2;		// defense strength
				}

				// distance effect

				weight -= area_distance( targetObj ) / 8;
			}
		}

		if( unitPtr->rank_id == RANK_GENERAL )
			weight += weight / 10;		// 10% heavier
		else if( unitPtr->rank_id == RANK_KING )
			weight += weight / 5;		// 20% heavier

		if( bestWeight < weight )
		{
			bestWeight = weight;
			bestUnitRecno = unitRecno;
			bestTargetRecno = targetObjRecno;
			if( !base_obj_array.is_deleted(targetObjRecno) )
			{
				targetXLoc = base_obj_array[targetObjRecno]->obj_loc_x1();
				targetYLoc = base_obj_array[targetObjRecno]->obj_loc_y1();
			}
		}
	}

	if( bestWeight >= 50 )
	{
		return bestTargetRecno;
	}

	return 0;
}
//------- End of function UnitGod::think_god_attack_target --------//




/*
//------- Begin of function UnitGod::think_dragon --------//

void UnitGod::think_dragon()
{
	int targetXLoc, targetYLoc;

	if( think_god_attack_target(targetXLoc, targetYLoc) )
		attack_firm( targetXLoc, targetYLoc );
}
//------- End of function UnitGod::think_dragon --------//


//------- Begin of function UnitGod::think_chinese_dragon --------//

void UnitGod::think_chinese_dragon()
{
	int targetXLoc, targetYLoc;

	if( think_god_attack_target(targetXLoc, targetYLoc) )
		attack_firm( targetXLoc, targetYLoc );
}
//------- End of function UnitGod::think_chinese_dragon --------//


//------- Begin of function UnitGod::think_phoenix --------//

void UnitGod::think_phoenix()
{
	int xLoc = m.random(MAX_WORLD_X_LOC);
	int yLoc = m.random(MAX_WORLD_Y_LOC);

	move( xLoc, yLoc );
}
//------- End of function UnitGod::think_phoenix --------//


//------- Begin of function UnitGod::think_viking_god --------//

void UnitGod::think_viking_god()
{
	int targetXLoc, targetYLoc;

	if( think_god_attack_target(targetXLoc, targetYLoc) )
	{
		go_cast_power(targetXLoc+1, targetYLoc+1, 2, COMMAND_AI);		// 2 - cast power type
	}
}
//------- End of function UnitGod::think_viking_god --------//


//------- Begin of function UnitGod::think_persian_god --------//

void UnitGod::think_persian_god()
{
	//------- there is no action, now think a new one ------//

	Nation* ownNation = nation_array[nation_recno];
	Firm*	  firmPtr;
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=ownNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		firmPtr = firm_array[ ownNation->ai_camp_array[i] ];

		//----- calculate the injured rating of the camp ----//

		Worker* workerPtr = firmPtr->worker_array;
		int	  totalHitPoints = 0;
		int	  totalMaxHitPoints = 0;

		for( int j=0 ; j<firmPtr->worker_count ; j++, workerPtr++ )
		{
			totalHitPoints 	+= workerPtr->hit_points;
			totalMaxHitPoints += workerPtr->max_hit_points();
		}

		if( !totalMaxHitPoints )
			continue;

		curRating = 100 * (totalMaxHitPoints-totalHitPoints) / totalMaxHitPoints;

		//---- if the king is the commander of this camp -----//

		if( firmPtr->overseer_recno &&
			 unit_array[firmPtr->overseer_recno]->rank_id == RANK_KING )
		{
			curRating += 20;
		}

		if( curRating > bestRating )
		{
			bestRating = curRating;
			targetXLoc = firmPtr->center_x;
			targetYLoc = firmPtr->center_y;
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, 1, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_persian_god --------//


//------- Begin of function UnitGod::think_japanese_god --------//

void UnitGod::think_japanese_god()
{
	//------- there is no action, now think a new one ------//

	Nation* ownNation = nation_array[nation_recno];
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	//------ think firm target --------//

	if( m.random(2)==0 )
	{
		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			Firm* firmPtr = firm_array[i];

			//------- only cast on camps ---------//

			if( !firmPtr->cast_to_FirmCamp() )
				continue;

			//------ only cast on hostile and tense nations ------//

			if( ownNation->get_relation(firmPtr->nation_recno)->status > RELATION_TENSE )
				continue;

			//------ calculate the rating of the firm -------//

			curRating = ((FirmCamp*)firmPtr)->total_combat_level()/10;

			if( curRating > bestRating )
			{
				bestRating  = curRating;
				targetXLoc  = firmPtr->center_x;
				targetYLoc  = firmPtr->center_y;
			}
		}
	}
	else
	{
		//------ think town target --------//

		for( int i=town_array.size() ; i>0 ; i-- )
		{
			if( town_array.is_deleted(i) )
				continue;

			Town* townPtr = town_array[i];

			//------ only cast on hostile and tense nations ------//

			if( townPtr->nation_recno && ownNation->get_relation(townPtr->nation_recno)->status > RELATION_TENSE )
				continue;

			//------ calculate the rating of the firm -------//

			curRating = townPtr->population + (100-townPtr->average_loyalty());

			if( curRating > bestRating )
			{
				bestRating = curRating;
				targetXLoc = townPtr->center_x;
				targetYLoc = townPtr->center_y;
			}
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, 1, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_japanese_god --------//


//------- Begin of function UnitGod::think_god_attack_target --------//
//
// <int&> targetXLoc, targetYLoc - reference vars for returning the
//											  location of the target.
//
// return: <int> 1 - target selected
//					  0 - no target selected
//
int UnitGod::think_god_attack_target(int& targetXLoc, int& targetYLoc)
{
	Firm* 	firmPtr;
	Nation* 	ownNation = nation_array[nation_recno];
	int		curXLoc=next_x_loc(), curYLoc=next_y_loc();
	int   	totalFirm = firm_array.size();
	int   	firmRecno = m.random(totalFirm)+1;

	for( int i=totalFirm ; i>0 ; i-- )
	{
		if( ++firmRecno > totalFirm )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr = firm_array[firmRecno];

//		if( firmPtr->firm_id == FIRM_MONSTER )
//			continue;			

		//-------- only attack enemies ----------//

		if( ownNation->get_relation(firmPtr->nation_recno)->status != RELATION_HOSTILE )
			continue;

		//---- only attack enemy base and camp ----//

		if( firmPtr->firm_id != FIRM_BASE &&
			 !firmPtr->cast_to_FirmCamp() )
		{
			continue;
		}

		//------- attack now --------//

		targetXLoc = firmPtr->loc_x1;
		targetYLoc = firmPtr->loc_y1;

		return 1;
	}

	//----- if there is no enemy to attack, attack Fryhtans ----//

	for( i=totalFirm ; i>0 ; i-- )
	{
		if( ++firmRecno > totalFirm )
			firmRecno = 1;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr = firm_array[firmRecno];

		if( firmPtr->firm_id == FIRM_MONSTER )
		{
			targetXLoc = firmPtr->loc_x1;
			targetYLoc = firmPtr->loc_y1;

			return 1;
		}
	}

	//---------------------------------------------------//

	return 0;
}
//------- End of function UnitGod::think_god_attack_target --------//


//------- Begin of function UnitGod::think_maya_god --------//

void UnitGod::think_maya_god()
{
	//------- there is no action, now think a new one ------//

	Nation* ownNation = nation_array[nation_recno];
	Firm*	  firmPtr;
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=ownNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		firmPtr = firm_array[ ownNation->ai_camp_array[i] ];

		curRating = 0;

		if( firmPtr->overseer_recno )
		{
			Unit* unitPtr = unit_array[firmPtr->overseer_recno];

			if( unitPtr->race_id == RACE_MAYA && unitPtr->skill.combat_level < 100 )
				curRating += 10;
		}

		Worker* workerPtr = firmPtr->worker_array;

		for( int j=firmPtr->worker_count-1 ; j>=0 ; j--, workerPtr++ )
		{
			if( workerPtr->race_id == RACE_MAYA && workerPtr->combat_level < 100 )
				curRating += 5;
		}

		if( curRating > bestRating )
		{
			bestRating = curRating;
			targetXLoc = firmPtr->center_x;
			targetYLoc = firmPtr->center_y;
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, 1, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_maya_god --------//

//------- Begin of function UnitGod::think_egyptian_god --------//
void UnitGod::think_egyptian_god()
{
	Nation* ownNation = nation_array[nation_recno];
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		//------ only cast on own nations ------//

		if( townPtr->nation_recno != nation_recno )
			continue;

		//------ calculate the rating of the firm -------//

		if( townPtr->population > MAX_TOWN_GROWTH_POPULATION - 5 )
			continue;

		// maximize the total loyalty gain.
		curRating = 5 * townPtr->average_loyalty();

		// calc rating on the number of people
		if( townPtr->population >= MAX_TOWN_GROWTH_POPULATION/2 )
			curRating -= (townPtr->population - MAX_TOWN_GROWTH_POPULATION/2) * 300 / MAX_TOWN_GROWTH_POPULATION;
		else
			curRating -= (MAX_TOWN_GROWTH_POPULATION/2 - townPtr->population) * 300 / MAX_TOWN_GROWTH_POPULATION;

		if( curRating > bestRating )
		{
			bestRating = curRating;
			targetXLoc = townPtr->center_x;
			targetYLoc = townPtr->center_y;
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, 1, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_egyptian_god --------//


//------- Begin of function UnitGod::think_indian_god --------//
void UnitGod::think_indian_god()
{
	Nation* ownNation = nation_array[nation_recno];

	// see if any unit near by

	int castRadius = 2;
	int leftLocX = next_x_loc() - castRadius;
	if(leftLocX < 0)
		leftLocX = 0;

	int rightLocX = next_x_loc() + castRadius;
	if(rightLocX >= MAX_WORLD_X_LOC)
		rightLocX = MAX_WORLD_X_LOC-1;

	int topLocY = next_y_loc() - castRadius;
	if(topLocY < 0)
		topLocY = 0;

	int bottomLocY = next_y_loc() + castRadius;
	if(bottomLocY >= MAX_WORLD_Y_LOC)
		bottomLocY = MAX_WORLD_Y_LOC-1;

	int curRating = 0;
	int xLoc, yLoc;
	for( yLoc = topLocY; yLoc <= bottomLocY; ++yLoc)
	{
		for( xLoc = leftLocX; xLoc <= rightLocX; ++xLoc )
		{
			Location *locPtr = world.get_loc(xLoc, yLoc);
			int unitRecno;
			Unit *unitPtr;
			if( locPtr->unit_recno(UNIT_LAND)
				&&	(unitRecno = locPtr->unit_recno(UNIT_LAND))
				&& !unit_array.is_deleted(unitRecno)
				&& (unitPtr = unit_array[unitRecno])

				&& unitPtr->nation_recno			// don't affect indepedent unit
				&& unitPtr->nation_recno != nation_recno
				&& (unitPtr->loyalty >= 20 && unitPtr->loyalty <= 60 ||
					unitPtr->loyalty <= 80 && unitPtr->target_loyalty < 30) )
			{
				switch( ownNation->get_relation(unitPtr->nation_recno)->status )
				{
				case RELATION_HOSTILE:
					curRating += 3;
					break;

				case RELATION_TENSE:
				case RELATION_NEUTRAL:
					// curRating += 0;		// unchange
					break;

				case RELATION_FRIENDLY:
					curRating -= 1;			// actually friendly humans are not affected
					break;

				case RELATION_ALLIANCE:
					curRating -= 1;			// actually allied humans are not affected
					break;

				default:
					err_here();
				}
			}
		}
	}

	if( curRating > 1 )
	{
		// if enemy unit come near, cast
		go_cast_power(next_x_loc(), next_y_loc(), 1, COMMAND_AI);		// 1 - cast power type
	}
	else
	{
		// find any unit suitable, go to that area first
		int bestUnitCost = 20000;
		for( int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
		{
			Unit *unitPtr;
			if( !unit_array.is_deleted(unitRecno)
				&& (unitPtr = unit_array[unitRecno])
				&& unitPtr->is_visible()
				&&	unitPtr->mobile_type == UNIT_LAND

				&& unitPtr->nation_recno			// don't affect indepedent unit
				&& unitPtr->nation_recno != nation_recno
				&& (unitPtr->loyalty >= 20 && unitPtr->loyalty <= 60 ||
					unitPtr->loyalty <= 80 && unitPtr->target_loyalty < 30)
				&& ownNation->get_relation(unitPtr->nation_recno)->status == RELATION_HOSTILE )
			{
				int cost = m.points_distance(next_x_loc(), next_y_loc(), unitPtr->next_x_loc(), unitPtr->next_y_loc());
				if( cost < bestUnitCost )
				{
					bestUnitCost = cost;
					xLoc = unitPtr->next_x_loc();
					yLoc = unitPtr->next_y_loc();
				}
			}
		}

		if( bestUnitCost < 100 )
		{
			if( m.points_distance(next_x_loc(), next_y_loc(), xLoc, yLoc) <= god_res[god_id]->cast_power_range )
				go_cast_power(xLoc, yLoc, 1, COMMAND_AI);		// 1 - cast power type
			else
				move( xLoc, yLoc );
		}
		else if( m.random(4) == 0 )
		{
			// move to a near random location
			xLoc = next_x_loc() + m.random(100) - 50;
			if( xLoc < 0 )
				xLoc = 0;
			if( xLoc >= MAX_WORLD_X_LOC)
				xLoc = MAX_WORLD_X_LOC-1;
			yLoc = next_y_loc() + m.random(100) - 50;
			if( yLoc < 0 )
				yLoc = 0;
			if( yLoc >= MAX_WORLD_Y_LOC)
				yLoc = MAX_WORLD_Y_LOC-1;
			move( xLoc, yLoc );
		}
	}
}
//------- End of function UnitGod::think_indian_god --------//


//------- Begin of function UnitGod::think_zulu_god --------//
void UnitGod::think_zulu_god()
{
	//------- there is no action, now think a new one ------//

	Nation* ownNation = nation_array[nation_recno];
	Firm*	  firmPtr;
	int  	  curRating, bestRating=0;
	int	  targetXLoc, targetYLoc;

	for( int i=ownNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		firmPtr = firm_array[ ownNation->ai_camp_array[i] ];

		curRating = 0;

		Unit* unitPtr;
		if( firmPtr->overseer_recno
			&& (unitPtr = unit_array[firmPtr->overseer_recno])
			&&	unitPtr->race_id == RACE_ZULU			// only consider ZULU leader
			&& unitPtr->skill.skill_level <= 70)
		{
			if( unitPtr->rank_id == RANK_KING )
				curRating += 5000;		// weak king need leadership very much

			if( unitPtr->skill.skill_level >= 40 )
				curRating += 5000 - (unitPtr->skill.skill_level - 40) * 60;	// strong leader need not be enhanced
			else
				curRating += 5000 - (40 - unitPtr->skill.skill_level) * 80;		// don't add weak leader

			// calculat the benefits to his soldiers
			Worker* workerPtr = firmPtr->worker_array;
			for( int j=firmPtr->worker_count-1 ; j>=0 ; j--, workerPtr++ )
			{
				if( workerPtr->race_id == RACE_ZULU )
					curRating += (unitPtr->skill.combat_level - workerPtr->combat_level)*2;
				else
					curRating += unitPtr->skill.combat_level - workerPtr->combat_level;
			}

			if( curRating > bestRating )
			{
				bestRating = curRating;
				targetXLoc = firmPtr->center_x;
				targetYLoc = firmPtr->center_y;
			}
		}
	}

	//-------------------------------------//

	if( bestRating )
	{
		go_cast_power(targetXLoc, targetYLoc, 1, COMMAND_AI);		// 1 - cast power type
	}
}
//------- End of function UnitGod::think_zulu_god --------//
*/
