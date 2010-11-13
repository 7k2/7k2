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

//Filename   : OAI_ACT2.CPP
//Description: AI - action progressing functions

#include <ALL.H> 
#include <OBOX.H>
#include <OSYS.H>
#include <OSPY.H>
#include <OINFO.H>
#include <OUNIT.H>
#include <OF_CAMP.H>
#include <OU_WAGON.H>
#include <ONATION.H>
#include <OGAME.H>

//-------- Begin of function Nation::ai_build_firm -------//
//
// action_x_loc, action_y_loc - location of the building site.
// ref_x_loc,    ref_y_loc    - location of the town which the base
//										  should be built next to.
// action_para					   - firm id.
// action_para2				   - firm race id. (for FirmBase and FirmSPU only)
//										  (for other firm types, it doesn't matter as the king's nationality determines the style of buildings)
//
int Nation::ai_build_firm(ActionNode* actionNode)
{
	err_when( actionNode->action_x_loc < 0 || actionNode->action_y_loc < 0 );			// they must be given when this function is called

	//-------- determine the skill id. needed -------//

	int firmId  = actionNode->action_para;
	int raceId  = actionNode->action_para2;

	if( firm_res[firmId]->only_build_race_id )
	{
		err_when( raceId && raceId != firm_res[firmId]->only_build_race_id );

		raceId = firm_res[firmId]->only_build_race_id;
	}

	//------------- get a skilled unit --------------//

	Unit* builderUnit;

	if( actionNode->unit_recno )
		builderUnit = unit_array[actionNode->unit_recno];
	else
	{
		builderUnit = ai_get_unit(true, raceId, actionNode);		// true - get a civilian unit

		if( !builderUnit && firm_res[firmId]->is_military )		// if the building is a military building, try location a military unit
			builderUnit = ai_get_unit(false, raceId, actionNode);
	}

	if( !builderUnit )
		return 0;

	//------- build the firm now ---------//

	builderUnit->build_firm(actionNode->action_x_loc, actionNode->action_y_loc, firmId, COMMAND_AI);

	if( builderUnit->cur_order.mode == UNIT_BUILD_FIRM )
	{
		builderUnit->set_ai_action_id(actionNode->action_id);
		actionNode->unit_recno = builderUnit->sprite_recno;
		return 1;
	}
	else
	{
		return 0;
	}
}
//-------- End of function Nation::ai_build_firm -------//


//-------- Begin of function Nation::ai_build_town -------//
//
// action_x_loc, action_y_loc - location of the building site.
// action_para					   - race id.
//
int Nation::ai_build_town(ActionNode* actionNode)
{
	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return 0;	

	err_when( actionNode->action_x_loc < 0 || actionNode->action_y_loc < 0 );			// they must be given when this function is called

	int raceId = actionNode->action_para;

	Unit* builderUnit;

	if( actionNode->unit_recno )
		builderUnit = unit_array[actionNode->unit_recno];
	else 
		builderUnit = ai_get_unit(1, raceId, actionNode);		// 1-the unit needs to be a civilian

	if( !builderUnit )
		return 0;

	//------- build the firm now ---------//

	builderUnit->settle_town( actionNode->action_x_loc, actionNode->action_y_loc, COMMAND_AI );

	if( builderUnit->cur_order.mode == UNIT_SETTLE_TOWN )
	{
		err_when( builderUnit->nation_recno==0 );

		builderUnit->set_ai_action_id(actionNode->action_id);
		actionNode->unit_recno = builderUnit->sprite_recno;

		return 1;
	}
	else
	{
		return 0;
	}
}
//-------- End of function Nation::ai_build_town -------//


//----- Begin of function Nation::ai_assign_overseer -----//
//
// action_x_loc, action_y_loc - location of the firm to which the overseer should be assigned.
// ref_x_loc,    ref_y_loc    - not used
// action_para					   - firm id.
// [action_para2]				   - race id of the overseer
//										  (if not given, the majority race will be used.)
//
int Nation::ai_assign_overseer(ActionNode* actionNode)
{
	//---------------------------------------------------------------------------//
	// cancel action if the firm is deleted, has incorrect firm_id or nation is
	// changed
	//---------------------------------------------------------------------------//

	int firmId = actionNode->action_para;

   err_when( !firmId );

	if(!check_firm_ready(actionNode->action_x_loc, actionNode->action_y_loc, firmId))	// return 0 to cancel action
		return -1;		// -1 means remove the current action immediately

	//-------- get the poisnter to the firm -------//

	Location* locPtr = world.get_loc(actionNode->action_x_loc, actionNode->action_y_loc);

	err_when(!locPtr->is_firm());

	Firm* firmPtr = firm_array[ locPtr->firm_recno() ];

	//-------- get a skilled unit --------//

	int raceId = firmPtr->majority_race();

	Unit* overseerUnit = ai_get_unit(0, raceId, actionNode);	// 0-the unit is not a civilian unit

	if( !overseerUnit )
		return 0;

	//---------------------------------------------------------------------------//

	FirmInfo* firmInfo = firm_res[firmId];

	err_when( !firmInfo->need_overseer );

	if(overseerUnit->rank_id==RANK_SOLDIER)
		overseerUnit->set_rank(RANK_GENERAL);

	err_when( overseerUnit->is_civilian() );

	overseerUnit->assign(actionNode->action_x_loc, actionNode->action_y_loc);

	if( overseerUnit->cur_order.mode == UNIT_ASSIGN )
	{
		err_when( overseerUnit->nation_recno != nation_recno );

		overseerUnit->set_ai_action_id(actionNode->action_id);
		actionNode->unit_recno = overseerUnit->sprite_recno;
		return 1;
	}
	else
		return 0;
}
//----- End of function Nation::ai_assign_overseer -----//


//----- Begin of function Nation::ai_assign_soldier -----//
//
// action_x_loc, action_y_loc - location of the firm to which the overseer should be assigned.
// ref_x_loc,    ref_y_loc    - not used
// action_para					   - firm id.
// [action_para2]				   - race id of the overseer
//										  (if not given, the majority race will be used.)
//
int Nation::ai_assign_soldier(ActionNode* actionNode)
{
	//---------------------------------------------------------------------------//
	// cancel action if the firm is deleted, has incorrect firm_id or nation is
	// changed
	//---------------------------------------------------------------------------//

	int firmId = actionNode->action_para;

	err_when( !firmId );

	if(!check_firm_ready(actionNode->action_x_loc, actionNode->action_y_loc, firmId))	// return 0 to cancel action
		return -1;		// -1 means remove the current action immediately

	//---------------------------------------------------------------------------//
	// cancel this action if the firm already has enough soldiers
	//---------------------------------------------------------------------------//

	Location* locPtr = world.get_loc(actionNode->action_x_loc, actionNode->action_y_loc);

	err_when(!locPtr->is_firm());

	FirmCamp* firmCamp = firm_array[locPtr->firm_recno()]->cast_to_FirmCamp();

	err_when( !firmCamp );
	err_when( firmCamp->firm_id != firmId );

	if( firmCamp->soldier_count>=MAX_SOLDIER )
		return -1;

	if( MAX_SOLDIER - firmCamp->soldier_count < actionNode->instance_count )		// if the firm now has more soldiers, reduce the number needed to be assigned to the firm
	{
		err_when( actionNode->processing_instance_count >= actionNode->instance_count );
		actionNode->instance_count = max( actionNode->processing_instance_count+1, MAX_SOLDIER - firmCamp->soldier_count );
	}

	//---------------------------------------------------------------------------//
	// firm exists and belongs to our nation. Assign soldier to firm
	//---------------------------------------------------------------------------//

	int   unitRecno=0;
	Unit* unitPtr = NULL;

	//----------- use a trained unit --------//

	if( actionNode->unit_recno )
		unitPtr = unit_array[actionNode->unit_recno];

	//------ recruit on job soldier ----------//

	if( !unitPtr )
	{
		unitRecno = recruit_in_firm_soldier(firmCamp, actionNode->action_para2);

		if( unitRecno )
			unitPtr = unit_array[unitRecno];
	}

	if( !unitPtr )
		return 0;

	//---------------------------------------------------------------------------//

	if( !world.get_loc(actionNode->action_x_loc, actionNode->action_y_loc)->is_firm() ) // firm exists, so assign
		return -1;

	err_when( unitPtr->rank_id != RANK_SOLDIER );

	unitPtr->assign(actionNode->action_x_loc, actionNode->action_y_loc);

	if( unitPtr->cur_order.mode == UNIT_ASSIGN )
	{
		unitPtr->set_ai_action_id(actionNode->action_id);
		err_when( unitPtr->nation_recno != nation_recno );

		return 1;
	}
	else
		return 0;
}
//----- End of function Nation::ai_assign_soldier -------//


//----- Begin of function Nation::ai_settle_town -----//
//
// action_x_loc, action_y_loc - location of the destination town.
// ref_x_loc, ref_y_loc 		- location of the origin town.
//
int Nation::ai_settle_town(ActionNode* actionNode)
{
	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return 0;	

	//------- check if both towns are ready first --------//
	if(!check_town_ready(actionNode->action_x_loc, actionNode->action_y_loc) ||
		!check_town_ready(actionNode->ref_x_loc, actionNode->ref_y_loc))
	{
		return -1;
	}

	//----------------------------------------------------//
	// stop if no jobless population
	//----------------------------------------------------//

	Location* locPtr = world.get_loc(actionNode->ref_x_loc, actionNode->ref_y_loc);

	err_when(!locPtr->is_town() || town_array.is_deleted(locPtr->town_recno()));

	Town* townPtr = town_array[locPtr->town_recno()]; // point to the old town

	//---- if cannot recruit because the loyalty is too low ---//

	if( !townPtr->can_recruit_peasant() )
	{
		//--- if there is not camp, that means we cannot grant villagers to increase loyalty, then do nothing ---//

		if( !townPtr->has_linked_own_camp )
			return 0;

		//--- if cannot recruit because of low loyalty, reward the town people now ---//

		int minRecruitLoyalty = MIN_RECRUIT_LOYALTY +
										townPtr->recruit_dec_loyalty(1, 0);		// 1-recruit one unit only

		if( townPtr->loyalty < minRecruitLoyalty )
		{
			if( cash > 0 && townPtr->accumulated_reward_penalty==0 )
			{
				townPtr->reward(COMMAND_AI);
			}

			if( !townPtr->can_recruit_peasant() )	// if still cannot be recruited, return 0 now
				return 0;
		}

		return 0;
	}

	//------------------------------------------------------//
	// recruit
	//------------------------------------------------------//

	//--- if we need to recruit more than one unit, we can recruit a wagon --//

	bool recruitWagon=false;
	int  recruitCount=1;

	if( actionNode->instance_count - actionNode->processing_instance_count > 1 )
	{
		recruitCount = actionNode->instance_count - actionNode->processing_instance_count;
		recruitCount = min( recruitCount, MAX_WAGON_POPULATION );

		recruitWagon = true;
	}

	int unitRecno = townPtr->recruit(recruitWagon, COMMAND_AI);		// 0-not recruit wagon

	if( !unitRecno )
		return 0;

	//---------------------------------------------------------------------------//
	// since it is not an important action, no need to add processing action
	//---------------------------------------------------------------------------//

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->assign(actionNode->action_x_loc, actionNode->action_y_loc);			// assign to the town

	if( unitPtr->cur_order.mode == UNIT_ASSIGN )
	{
		unitPtr->set_ai_action_id(actionNode->action_id);
		err_when( unitPtr->nation_recno==0 );

		if( recruitCount > 1 )
			actionNode->instance_count = max( actionNode->processing_instance_count+1, actionNode->instance_count-recruitCount+1 );

		return 1;
	}
	else
		return 0;
}
//----- End of function Nation::ai_settle_town -----//


//--------- Begin of function Nation::ai_scout ----------//
//
// action_x_loc, action_y_loc - location of the destination town.
// ref_x_loc, ref_y_loc 		- location of the origin town.
//
int Nation::ai_scout(ActionNode* actionNode)
{
	//------- check if both towns are ready first --------//

	if(!check_town_ready(actionNode->action_x_loc, actionNode->action_y_loc) ||
		!check_town_ready(actionNode->ref_x_loc, actionNode->ref_y_loc))
	{
		return -1;
	}

	//----------------------------------------------------//
	// stop if no jobless population
	//----------------------------------------------------//

	Location* locPtr = world.get_loc(actionNode->ref_x_loc, actionNode->ref_y_loc);

	err_when(!locPtr->is_town() || town_array.is_deleted(locPtr->town_recno()));

	Town* townPtr = town_array[locPtr->town_recno()]; // point to the old town

	//---- if cannot recruit because the loyalty is too low ---//

	if( !townPtr->can_recruit_peasant() && townPtr->has_linked_own_camp )
		return 0;

	//------------------------------------------------------//
	// recruit
	//------------------------------------------------------//

	int unitRecno = townPtr->recruit(0, COMMAND_AI);	// 0-not recruit wagon

	if( !unitRecno )
		return 0;

	//---------------------------------------------------------------------------//
	// since it is not an important action, no need to add processing action
	//---------------------------------------------------------------------------//

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->move( actionNode->action_x_loc, actionNode->action_y_loc );
	unitPtr->set_ai_action_id(actionNode->action_id);

	err_when( unitPtr->nation_recno==0 );

	return 1;
}
//-------- End of function Nation::ai_scout ----------//

