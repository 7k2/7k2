//Filename   : ON_MONS.CPP
//Description: Functions for the class NationMonster

#include <OBASEOBJ.H>
#include <OTOWN.H>
#include <OCONFIG.H>
#include <OF_LAIR.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OGAME.H>

//--------- Begin of function NationMonster::process_ai --------//

void NationMonster::process_ai()
{
	//------- process parent class AI ------//

	int nationRecno = nation_recno;

	Nation::process_ai();

	if( nation_array.is_deleted(nationRecno) )
		return;

}
//---------- End of function NationMonster::process_ai --------//


//--------- Begin of function NationMonster::process_ai_main --------//

int NationMonster::process_ai_main(int mainActionId)
{
	//---- if we are short of life points, kill to gain some ----//

	if( info.game_date%15 == nation_recno%15 )
	{
		if( think_acquire_live_points() )
			return 1;
   }

	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return 0;	

	//------- process common AI --------//

	if( Nation::process_ai_main(mainActionId) )
		return 1;

	//------- process monster AI -------//

	switch( mainActionId )
	{
		case 11:
			think_attack_hostile_nation();		// think about attacking hostile nations
			return 1;

		case 12:
			think_buy_independent_lair();
			return 1;
	}

	return 0;
}
//---------- End of function NationMonster::process_ai_main --------//


//----- Begin of function NationMonster::think_diplomacy -----//
//
void NationMonster::think_diplomacy()
{
	//--- process incoming messages first, so we won't send out the same request to nation which has already proposed the same thing ---//

	int nationRecno = nation_recno;

	process_action(0, ACTION_AI_PROCESS_TALK_MSG);

	if( nation_array.is_deleted(nationRecno) )		// the nation may have been deleted, if the nation accepts a purchase kingdom offer
		return;

	//---- thinking about war first -----//

	if( think_declare_war() )
		return;

	//----- think buy food first -------//

	if( ai_town_count > 0 )
		think_request_buy_food();   // don't return even if this request is sent

	//----- think request cease fire ----//

	if( think_request_cease_war() )
		return;

	//------ thinking about treaty ---------//

	if( ai_town_count > 0 && ai_market_count > 0 )
	{
		if( think_trade_treaty() )
			return;
	}

	if( think_end_treaty() )
		return;

	//-------- think about other matters --------//

	if( think_demand_tribute_aid() )
		return;

	if( think_exchange_tech() )
		return;

	//---- think about offering to purchase throne ----//

	if( think_request_surrender() )
		return;
}
//------ End of function NationMonster::think_diplomacy ------//


//----- Begin of function NationMonster::think_attack_hostile_nation -----//

void NationMonster::think_attack_hostile_nation()
{
	int targetObjRecno=0;

	//---- either attack a town or a firm -----//

	if( m.random(2)==0 )
	{
		int firmRecno = m.random(firm_array.size())+1;

		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( ++firmRecno > firm_array.size() )
				firmRecno = 1;

			if( firm_array.is_deleted(firmRecno) )
				continue;

			Firm* firmPtr = firm_array[firmRecno];

			if( !firmPtr->nation_recno )
				continue;

			if( firmPtr->nation_ptr()->get_relation_status(nation_recno) == RELATION_HOSTILE )
			{
				targetObjRecno = firmPtr->base_obj_recno;
				break;
			}
		}
	}
	else
	{
		int townRecno = m.random(town_array.size())+1;

		for( int i=town_array.size() ; i>0 ; i-- )
		{
			if( ++townRecno > town_array.size() )
				townRecno = 1;

			if( town_array.is_deleted(townRecno) )
				continue;

			Town* townPtr = town_array[townRecno];

			if( !townPtr->nation_recno )
				continue;

			if( townPtr->population == 0 )
				continue;

			if( townPtr->nation_ptr()->get_relation_status(nation_recno) == RELATION_HOSTILE )
			{
				targetObjRecno = townPtr->base_obj_recno;
				break;
			}
		}
	}

	if( !targetObjRecno )
		return;

	//-------- attack now ---------//

	BaseObj* targetBaseObj = base_obj_array[targetObjRecno];

	ai_attack_target(targetBaseObj->obj_loc_x1(), targetBaseObj->obj_loc_y1(), targetBaseObj->obj_power() );
}
//------ End of function NationMonster::think_attack_hostile_nation ------//


//------ Begin of function NationMonster::think_buy_independent_lair --------//
//
void NationMonster::think_buy_independent_lair()
{
	//--- if we don't have any Lairs, we can't buy any independent lairs ---//

	if( firm_res[FIRM_LAIR]->get_nation_firm_count(nation_recno)==0 )
		return;

	//--------------------------------------------//

	for( int callLoop=1 ; callLoop<=2 ; callLoop++ )
	{
		//--------------------------//

		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			FirmLair* targetFirm = firm_array[i]->cast_to_FirmLair();

			if( !targetFirm || targetFirm->nation_recno )		// only independent Lairs
				continue;

			if( cash < targetFirm->cast_to_FirmLair()->grant_to_non_own_lair_cost() )
				continue;

			int unitId = monster_res[ targetFirm->monster_id() ]->unit_id;

			//--- only buy Fryhtans that we don't have, otherwise it is more cost effective to build them on our own ---//

			if( unit_res[unitId]->nation_all_unit_count(nation_recno) > 0 )
				continue;

			if( callLoop==1 )
			{
				//----- 1st loop: see if we have a Lair linked to it now -----//

				for( int j=0 ; j<targetFirm->linked_firm_count ; j++ )
				{
					FirmLair* firmLair = firm_array[ targetFirm->linked_firm_array[j] ]->cast_to_FirmLair();

					//---- if there is a lair of ours linked to the target lair ----//

					if( firmLair && firmLair->nation_recno == nation_recno )
					{
						targetFirm->grant_to_non_own_lair(nation_recno, COMMAND_AI);
						return;
					}
				}
			}
			else
			{
				//--- 2nd loop: order a unit to build a Lair next to it order to buy it ---//

				//--- pick a lair that is closest to the target ---//

				int 		 curRating, bestRating=0;
				FirmLair* bestLair=NULL;

				for( int j=0 ; j<ai_camp_count ; j++ )
				{
					FirmLair* firmLair = firm_array[ ai_camp_array[j] ]->cast_to_FirmLair();

					if( !firmLair )
						continue;

					curRating = world.distance_rating( targetFirm->center_x, targetFirm->center_y,
									firmLair->center_x, firmLair->center_y );

					if( curRating > bestRating )
					{
						bestRating = curRating;
						bestLair   = firmLair;
					}
				}

				if( bestLair )
				{
					//---- first see if we have enough money to buy the loyalty ----//

					if( !ai_should_spend( 20 + pref_peacefulness/2, (float) bestLair->grant_to_non_own_lair_cost() ) )
						break;

					bestLair->ai_build_lair(targetFirm);
					return;
				}
			}
		}
	}
}
//------- End of function NationMonster::think_buy_independent_lair ---------//


//------ Begin of function NationMonster::think_acquire_live_points ------//
//
// If the Fryhtan nation is short of live points, kill people to
// get live points.
//
int NationMonster::think_acquire_live_points()
{
	//-------------------------------------------//

	if( live_points >= 1000 * (30+pref_live_points_reserve) / 100 )
		return 0;

	// restore life point to the lowest limit in tutorial
	if( game.game_mode == GAME_TUTORIAL )
	{
		live_points = (float) 1000 * (30+pref_live_points_reserve) / 100;
		return 0;
	}

	//--- get the center position of the lairs -----//

	int xLocTotal=0, yLocTotal=0;

	if( ai_camp_count==0 )
		return 0;

	for( int i=0 ; i<ai_camp_count ; i++ )
	{
		Firm* firmPtr = firm_array[ai_camp_array[i]];

		xLocTotal += firmPtr->center_x;
		yLocTotal += firmPtr->center_y;
	}

	int nationCenterX = xLocTotal / ai_camp_count;
	int nationCenterY = yLocTotal / ai_camp_count;

	//-----------------------------------------//

	int   curRating, maxRating= -10000;
	int   curDefense, bestTownDefense;
	Town* bestTown=NULL;

	//---- attack human towns to get live points -----//

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->population <= (100-pref_monster_aggressiveness)/20 )		// do not attack towns with too fewer people
			continue;

		//--- if this town belongs to an enemy of ours ---//

		int rc = 0;

		if( townPtr->nation_recno )
		{
			int relationStatus = get_relation_status(townPtr->nation_recno);

			if( relationStatus <= RELATION_TENSE )		// hostile to tense
			{
				rc = 1;
			}
			else		// Netural to Alliance
			{
				//--- do not attack human players too soon ----//

				static short delayDaysArray[] = { 1000, 720, 365, 180, 90 }; 		// very low, low, medium, high, very high

				if( info.game_date - info.game_start_date < delayDaysArray[config.ai_aggressiveness] )
					continue;

				//--------------------------------------------//

				if( relationStatus == RELATION_NEUTRAL )	// attack neutral
					rc = 1;
				else
					rc = pref_honesty < 50;			// friendly or alliance, break treaty if honesty < 50
			}
		}

		//---- if this is an independent town and there are no lairs controlling it ------//

		else if( townPtr->nation_recno==0 &&
					townPtr->resistance(nation_recno) < MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY )
		{
			rc = 1;
		}

		//------------------------------------------//

		if( rc )
		{
			curDefense = townPtr->obj_power();

			curRating = 200 - curDefense / 10
							+ world.distance_rating( nationCenterX, nationCenterY,
							  townPtr->center_x, townPtr->center_y );

			if( townPtr->nation_recno )
				curRating -= get_relation(townPtr->nation_recno)->ai_relation_level * 2;

			//--- lower tendency to attack towns controlled by Fryhtan kingdoms ---//

			if( townPtr->nation_recno && nation_array[townPtr->nation_recno]->is_monster() )
				curRating -= 50;

			//--- lower tendency to attack slave towns ----//

			if( townPtr->nation_recno==0 && townPtr->is_pay_tribute_to_monster() )
				curRating -= 50;

			//--- add a tendency to attack human players ----//

			if( townPtr->nation_recno && !nation_array[townPtr->nation_recno]->is_ai() )
			{
				curRating += (config.ai_aggressiveness-1) * 30;
			}

			//-----------------------------------------------//

			if( curRating > maxRating )
			{
				maxRating = curRating;
				bestTown  = townPtr;
				bestTownDefense = curDefense;
			}
		}
	}

	if( !bestTown )
		return 0;

	//-------- attack now ---------//

	int defenseMode = live_points<200;		// if live points is too small, set defense mode on to attack aggressively

	ai_attack_target(bestTown->loc_x1, bestTown->loc_y1, bestTownDefense, defenseMode );		// 1-set it to defense mode so that it can attack aggressively

	return 1;
}
//------- End of function NationMonster::think_acquire_live_points ------//

