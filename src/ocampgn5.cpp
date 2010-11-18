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

//Filename   : OCAMPGN5.CPP
//Description: Campaign functions

#include <ocampgn.h>


//--- Begin of function Campaign::player_pick_attack_target ---//
//
// <int> targetType - can be one of the following:
//							 CAMPAIGN_NATION_MONSTER
//							 CAMPAIGN_NATION_AI
//							 CAMPAIGN_NATION_MONSTER + CAMPAIGN_NATION_AI
//
// return: <int> >0 - the campaign nation recno of the target state
//					  0  - no states available for selection
//
int Campaign::player_pick_attack_target(int targetType)
{
	//--- create an array of states that be selected as the target ---//

	char selectableStateArray[MAX_STATE];
	int  selectableStateCount=0;
	int  rc;

	for( int i=state_array.size() ; i>0 ; i-- )
	{
		int stateNationRecno = state_array[i]->campaign_nation_recno;

		if( stateNationRecno==0 )
			continue;

		int raceId = get_nation(stateNationRecno)->race_id;

		if( state_array.is_adjacent_to_nation(i, western_nation_recno) )
		{
			if( targetType == CAMPAIGN_NATION_MONSTER )
			{
				rc = raceId<0;
			}
			else if( targetType == CAMPAIGN_NATION_AI )
			{
				rc = raceId>0;
			}
			else
				rc = 1;

			if( rc )
				selectableStateArray[selectableStateCount++] = i;
		}
	}

	if( selectableStateCount==0 )
		return 0;

	//---- call the interface fucntion to select the target ----//

	if( selectableStateCount==1 )
		target_state_recno = selectableStateArray[0];		// just one choice
	else
		target_state_recno = selectableStateArray[ m.random(selectableStateCount) ];

	if( targetType == CAMPAIGN_NATION_MONSTER )
	{
		cur_monster_nation_recno = state_array[target_state_recno]->campaign_nation_recno;
	}

	//--- set the player's state where the troop is sent from ---//

	attack_state_recno = 0;

	for( i=state_array.size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = state_array[i];

		if( stateInfo->campaign_nation_recno == western_nation_recno &&
			 stateInfo->is_adjacent(target_state_recno) )
		{
			attack_state_recno = i;
			break;
		}
	}

	err_when( attack_state_recno==0 );

	//------------------------------------------//

	return state_array[target_state_recno]->campaign_nation_recno;
}
//---- End of function Campaign::player_pick_attack_target ----//

