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

//Filename    : OF_LARA2.CPP
//Description : Firm Monster Lair  - AI functions part 2

#include <OTOWN.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OMONSRES.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OFIRMALL.H>

//--------- Begin of function FirmLair::think_build_special --------//

void FirmLair::think_build_special()
{
	//--- if an action is already queued for building a special structure, don't build new ones ---//

	int 			i;
	ActionNode* actionNode;
	Nation*		ownNation = nation_array[nation_recno];

	for( i=ownNation->action_count() ; i>0 ; i-- )
	{
		if( ownNation->is_action_deleted(i) )
			continue;

		actionNode = ownNation->get_action(i);

		if( actionNode->action_mode == ACTION_AI_BUILD_FIRM &&
			 actionNode->action_para >= FIRST_MONSTER_SPECIAL_FIRM &&
			 actionNode->action_para <= LAST_MONSTER_SPECIAL_FIRM )
		{
			return;
		}
	}

	//------------------------------------------------//

	switch( monster_id() )
	{
		case MONSTER_KHARSHUF:
			think_build_kharsuf_special();
			break;

		case MONSTER_BREGMA:
			think_build_bregma_special();
			break;

		case MONSTER_KERASSOS:
			think_build_kerassos_special();
			break;

		case MONSTER_MINOTAUROS:
			think_build_minotauros_special();
			break;

		case MONSTER_GROKKEN:
			think_build_grokken_special();
			break;

		case MONSTER_EZPINEZ:
			think_build_ezpinez_special();
			break;

		case MONSTER_EXOVUM:
			think_build_exovum_special();
			break;
	}
}
//----------- End of function FirmLair::think_build_special --------//


//--------- Begin of function FirmLair::think_buy_tech --------//

int FirmLair::think_buy_tech()
{
	int techId=0;

	switch( monster_id() )
	{
		case MONSTER_KHARSHUF:
			techId = think_kharsuf_buy_tech();
			break;

		case MONSTER_BREGMA:
			techId = think_bregma_buy_tech();
			break;

		case MONSTER_KERASSOS:
			techId = think_kerassos_buy_tech();
			break;

		case MONSTER_MINOTAUROS:
			techId = think_minotauros_buy_tech();
			break;

		case MONSTER_GROKKEN:
			techId = think_grokken_buy_tech();
			break;

		case MONSTER_EZPINEZ:
			techId = think_ezpinez_buy_tech();
			break;

		case MONSTER_EXOVUM:
			techId = think_exovum_buy_tech();
			break;
	}

	if( techId )
		buy_tech( techId, COMMAND_AI );

	return techId;
}
//----------- End of function FirmLair::think_buy_tech --------//


//--------- Begin of function FirmLair::should_build_special --------//
//
int FirmLair::should_build_special(int firmId)
{
	FirmInfo* firmInfo = firm_res[firmId];

	if( !firmInfo->nation_can_build(nation_recno, race_id) )
		return 0;

	//--- if we can build one now and we don't have one currently, then build immediately ---//

	int ownFirmCount = firmInfo->get_nation_firm_count(nation_recno);

	if( ownFirmCount==0 )
		return 1;

	int importanceRating = 40 + nation_array[nation_recno]->pref_monster_special/5
								  - ownFirmCount * 10;

	return nation_array[nation_recno]->ai_should_spend( importanceRating, firmInfo->setup_cost, firmInfo->setup_live_points_cost );
}
//----------- End of function FirmLair::should_build_special --------//


//--------- Begin of function FirmLair::think_build_kharsuf_special --------//

int FirmLair::think_build_kharsuf_special()
{
	if( !should_build_special(FIRM_LISHORR) )
		return 0;

	//--- locate a place where our military is weak to build Lishorr ---//

	Nation* nationPtr = nation_array[nation_recno];

	// ####### begin Gilbert 10/3 #########//
	int arrayCount = 0;
	short *array = nationPtr->update_ai_firm_array( FIRM_LISHORR, 0, 0, arrayCount );
	for( int i=0 ; i<arrayCount; i++, array++ )
	{
		Firm* firmPtr = firm_array[ *array ];
	// ####### end Gilbert 10/3 #########//

		if( firmPtr->firm_id == FIRM_LISHORR )
			continue;

		int hasWar;
		int scanRange=8;

		int netPower = world.net_attacker_power_in_area( firmPtr->center_x, firmPtr->center_y,
					 	   nation_recno, 0, 0, hasWar, scanRange );

		if( netPower < nationPtr->ai_last_build_lishorr_net_power )
		{
			if( firmPtr->ai_build_neighbor_firm(FIRM_LISHORR) )
			{
				nationPtr->ai_last_build_lishorr_net_power = netPower;
				return 1;
         }
		}
	}

	//--- if no new Lishorr is built because none of the firms' power is below ai_last_build_lishorr_net_power

	nationPtr->ai_last_build_lishorr_net_power += 100;		// increase it 100 at a time, so the next time more firms will meet the condition

	return 0;
}
//----------- End of function FirmLair::think_build_kharsuf_special --------//


//--------- Begin of function FirmLair::think_build_bregma_special --------//

int FirmLair::think_build_bregma_special()
{
	int firmId;

	if( m.random(2)==0 )
		firmId = FIRM_BEE;
	else
		firmId = FIRM_TERMITE;

	if( !should_build_special(firmId) )
		return 0;

	//--- if any of the existing Alchemy firm is lowerly utilized, don't build new ones ---//

	Nation* nationPtr = nation_array[nation_recno];

	for( int i=0 ; i<nationPtr->ai_mons_train_count ; i++ )
	{
		FirmMonsterTrain* firmPtr = firm_array[ nationPtr->ai_mons_train_array[i] ]->cast_to_FirmMonsterTrain();

		// ####### begin Gilbert 10/3 #######//
		if( !firmPtr )
			continue;
		err_when( !firmPtr );
		// ####### end Gilbert 10/3 #######//

		if( firmPtr->firm_id == firmId &&
			 firmPtr->trainee_count < MAX_MONSTER_TRAINEE )
		{
			return 0;
		}
	}

	return ai_build_neighbor_firm(firmId);
}
//----------- End of function FirmLair::think_build_bregma_special --------//


//--------- Begin of function FirmLair::think_build_kerassos_special --------//

int FirmLair::think_build_kerassos_special()
{
	int firmId = FIRM_ANIMAL;

	if( !should_build_special(firmId) )
		return 0;

	//--- if any of the existing Alchemy firm is lowerly utilized, don't build new ones ---//

	Nation* nationPtr = nation_array[nation_recno];

	for( int i=0 ; i<nationPtr->ai_mons_animal_count ; i++ )
	{
		FirmAnimal* firmPtr = firm_array[ nationPtr->ai_mons_animal_array[i] ]->cast_to_FirmAnimal();

		err_when( !firmPtr );

		if( firmPtr->animal_unit_count >= 2 )		// don't build new ones if existing ones have sufficient animal stocks
			return 0;
	}

	return ai_build_neighbor_firm(firmId);
}
//----------- End of function FirmLair::think_build_kerassos_special --------//


//--------- Begin of function FirmLair::think_build_minotauros_special --------//

int FirmLair::think_build_minotauros_special()
{
	int firmId=FIRM_ALCHEMY;

	Nation* nationPtr = nation_array[nation_recno];

	//----- try to build on raw material sites --------//

	if( tech_res[TECH_RAW_TO_LIVE] || tech_res[TECH_RAW_TO_GOLD] )
	{
		short xLoc, yLoc, refXLoc, refYLoc;

		int rawId = nationPtr->seek_mine(xLoc, yLoc, refXLoc, refYLoc);	//reference location is the raw material location

		if( rawId )
		{
			if( !nationPtr->is_action_exist(ACTION_AI_BUILD_FIRM, firmId) )
			{
				nationPtr->add_action(xLoc, yLoc, refXLoc, refYLoc, ACTION_AI_BUILD_FIRM, firmId);
				return 1;
			}
		}
	}

	//-----------------------------------------------//

	if( !should_build_special(firmId) )
		return 0;

	//--- if any of the existing Alchemy firm is lowerly utilized, don't build new ones ---//

	for( int i=0 ; i<nationPtr->ai_mons_alchemy_count ; i++ )
	{
		FirmMonsterAlchemy* firmPtr = firm_array[ nationPtr->ai_mons_alchemy_array[i] ]->cast_to_FirmMonsterAlchemy();

		err_when( !firmPtr );

		if( firmPtr->production_30days() < 10 )
			return 0;
	}

	//-------------------------------------------//

	return ai_build_neighbor_firm(firmId);
}
//----------- End of function FirmLair::think_build_minotauros_special --------//


//--------- Begin of function FirmLair::think_build_grokken_special --------//

int FirmLair::think_build_grokken_special()
{
	return 0;
}
//----------- End of function FirmLair::think_build_grokken_special --------//


//--------- Begin of function FirmLair::think_build_ezpinez_special --------//

int FirmLair::think_build_ezpinez_special()
{
	int firmId = FIRM_INCUBATOR;

	if( !should_build_special(firmId) )
		return 0;

	//--- if any of the existing Incubator firm is lowerly utilized, don't build new ones ---//

	Nation* nationPtr = nation_array[nation_recno];

	for( int i=0 ; i<nationPtr->ai_incubator_count ; i++ )
	{
		FirmIncubator* firmPtr = firm_array[nationPtr->ai_incubator_array[i]]->cast_to_FirmIncubator();

		err_when( !firmPtr );

		if( !firmPtr->build_unit_id )
			return 0;
	}

	//-------------------------------------------//

	return ai_build_neighbor_firm(firmId);
}
//----------- End of function FirmLair::think_build_ezpinez_special --------//


//--------- Begin of function FirmLair::think_build_exovum_special --------//

int FirmLair::think_build_exovum_special()
{
	int firmId = FIRM_MAGIC;

	if( !should_build_special(firmId) )
		return 0;

	//--- if any of the existing magic tors are idle, don't build new ones ---//

	Nation* nationPtr = nation_array[nation_recno];

	for( int i=0 ; i<nationPtr->ai_mons_magic_count ; i++ )
	{
		FirmMagic* firmPtr = firm_array[nationPtr->ai_mons_magic_array[i]]->cast_to_FirmMagic();

		err_when( !firmPtr );

		if( firmPtr->cast_delay == firmPtr->cast_delay_max )
			return 0;
	}

	//-------------------------------------------//

	return ai_build_neighbor_firm(firmId);
}
//----------- End of function FirmLair::think_build_exovum_special --------//


//--------- Begin of function FirmLair::think_kharsuf_buy_tech --------//

int FirmLair::think_kharsuf_buy_tech()
{
	static short techIdArray[]   = { TECH_LISHORR, TECH_WILDE_LISHORR, TECH_CAMOUFLAGE_ABILITY };
	static short techPrefArray[] = { 80          , 50                , 50                      };

	return think_tech_to_buy( techIdArray, techPrefArray, 3 );
}
//----------- End of function FirmLair::think_kharsuf_buy_tech --------//


//--------- Begin of function FirmLair::think_tech_to_buy --------//
//
// Think about if there is any tech we should buy now.
//
int FirmLair::think_tech_to_buy(short* techIdArray, short* techPrefArray, int techCount)
{
	Nation* ownNation = nation_array[nation_recno];

	//--- calculate the total tech level of this class bought by the kingdom ---//

	int totalTechLevel=0;

	for( int i=0 ; i<techCount ; i++ )
		totalTechLevel += tech_res[ techIdArray[i] ]->get_nation_tech_level(nation_recno);

	//---- go over the tech list and see if there is any tech we should buy now ---//

	int techSeq = m.random(techCount);

	for( i=0 ; i<techCount ; i++ )
	{
		if( ++techSeq >= techCount )
			techSeq = 0;

		int techId  = techIdArray[techSeq];
		int buyPref = techPrefArray[techSeq]; 	// buying preference

		if( ownNation->military_rating + ownNation->pref_monster_special/10 + buyPref/5
			 > 40 + totalTechLevel*10 ||
			 have_excess_live_point( techId, 1000 + totalTechLevel*100 - buyPref*4) )
		{
			if( tech_res[techId]->can_research(nation_recno) )
				return techId;
		}
	}

	return 0;
}
//----------- End of function FirmLair::think_tech_to_buy --------//


//--------- Begin of function FirmLair::have_excess_live_point --------//
//
// Return whether the Fryhtan kingdom will have the given amount
// of excessive live points after buying the specific tech.
//
// <int> techId - id. of the tech.
// <int> excessLivePoints - the amount of excessive live points required
//
int FirmLair::have_excess_live_point(int techId, int excessLivePoints)
{
	Nation* ownNation = nation_array[nation_recno];

	return ownNation->live_points >
			 tech_res[techId]->live_points_cost() + excessLivePoints
			 * (75 + ownNation->pref_live_points_reserve/2) / 100;
}
//----------- End of function FirmLair::have_excess_live_point --------//


//--------- Begin of function FirmLair::think_bregma_buy_tech --------//

int FirmLair::think_bregma_buy_tech()
{
	static short techIdArray[]   = { TECH_BEE, TECH_TERMITE };
	static short techPrefArray[] = { 80      , 80           };

	return think_tech_to_buy( techIdArray, techPrefArray, 2 );
}
//----------- End of function FirmLair::think_bregma_buy_tech --------//


//--------- Begin of function FirmLair::think_kerassos_buy_tech --------//

int FirmLair::think_kerassos_buy_tech()
{
	static short techIdArray[]   = { TECH_JAGUAR, TECH_GORILLA, TECH_RHINO };
	static short techPrefArray[] = { 70         , 70          , 50		     };

	return think_tech_to_buy( techIdArray, techPrefArray, 3 );
}
//----------- End of function FirmLair::think_kerassos_buy_tech --------//


//--------- Begin of function FirmLair::think_ezpinez_buy_tech --------//

int FirmLair::think_ezpinez_buy_tech()
{
	static short techIdArray[]   = { TECH_WEAPON_1, TECH_WEAPON_2, TECH_WEAPON_3 };
	static short techPrefArray[] = { 60           , 60           , 60		    	  };

	return think_tech_to_buy( techIdArray, techPrefArray, 3 );
}
//----------- End of function FirmLair::think_ezpinez_buy_tech --------//


//--------- Begin of function FirmLair::think_exovum_buy_tech --------//

int FirmLair::think_exovum_buy_tech()
{
	static short techIdArray[]   = { TECH_MAGIC_DAMAGE_BUILDING, TECH_MAGIC_FREEZE_UNIT, TECH_MAGIC_DAMAGE_AREA, TECH_MAGIC_TELEPORT };
	static short techPrefArray[] = { 90, 30, 50, 30 };

	return think_tech_to_buy( techIdArray, techPrefArray, 4 );
}
//----------- End of function FirmLair::think_exovum_buy_tech --------//


//--------- Begin of function FirmLair::think_grokken_buy_tech --------//

int FirmLair::think_grokken_buy_tech()
{
	static short techIdArray[]   = { TECH_FORTRESS, TECH_FORTRESS_CAMOUFLAGE };
	static short techPrefArray[] = { 50, 50 };

	return think_tech_to_buy( techIdArray, techPrefArray, 2 );
}
//----------- End of function FirmLair::think_grokken_buy_tech --------//


//--------- Begin of function FirmLair::think_minotauros_buy_tech --------//

int FirmLair::think_minotauros_buy_tech()
{
	static short techIdArray[]   = { TECH_LIVE_TO_GOLD, TECH_GOLD_TO_LIVE, TECH_RAW_TO_LIVE, TECH_RAW_TO_GOLD, TECH_ALCHEMY_YIELD };
	static short techPrefArray[] = { 20, 70, 90, 60, 50 };

	return think_tech_to_buy( techIdArray, techPrefArray, 5 );
}
//----------- End of function FirmLair::think_minotauros_buy_tech --------//

