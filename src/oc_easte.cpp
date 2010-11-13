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

//Filename   : OC_EASTE.CPP
//Description: CampaignEastWest

#include <ALL.H>
#include <OSYS.H>
#include <ODATE.H>
#include <ONATION.H>
#include <OSTATE.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OFONT.H>
#include <OC_EAST.H>

//------ Define function pointers array ------//

typedef int (CampaignEastWest::*EventFP)();		// Define variable type

static EventFP event_func_array[] =
{
	CampaignEastWest::event_fryhtan_takeover,
	CampaignEastWest::event_random_point_1,
	CampaignEastWest::event_state_betrayal,
	CampaignEastWest::event_fryhtan_offer_peace,
	CampaignEastWest::event_give_throne,
	CampaignEastWest::event_random_point_2,
	CampaignEastWest::event_eastern_empire_offer_help,
	CampaignEastWest::event_fryhtan_attack_fryhtan,
	CampaignEastWest::event_fryhtan_ask_for_joint_attack,
	CampaignEastWest::event_random_point_3,
	CampaignEastWest::event_fryhtan_think_surrender,
	CampaignEastWest::event_ask_eastern_empire_for_help,
	CampaignEastWest::event_attempt_to_unite,
	CampaignEastWest::event_random_point_4,
	CampaignEastWest::event_ultimate_victory,
	CampaignEastWest::event_fryhtan_invade_east,
	CampaignEastWest::event_east_ask_for_joint_attack,
	CampaignEastWest::event_fryhtan_break_up,
	CampaignEastWest::event_player_take_over_fryhtan,
	CampaignEastWest::event_disband_fryhtan_army,
	CampaignEastWest::event_independent_surrender,
	CampaignEastWest::event_fryhtan_army_revolt,
	CampaignEastWest::event_player_take_over_east,
};

//------ Begin of function CampaignEastWest::process_event ------//
//
int CampaignEastWest::process_event()
{
	EventFP eventFP = event_func_array[event_id-1];

	return (this->*eventFP)();   // call the corrsponding function to return the news msg
}
//------ End of function CampaignEastWest::process_event ------//


//-- Begin of function CampaignEastWest::event_fryhtan_takeover --//
//
// Event 1
//
int CampaignEastWest::event_fryhtan_takeover()
{
	err_when( state_array.nation_state_count(western_nation_recno) <= 1 );

	// ###### begin Ban 30/6 #######//
	if( !cur_monster_nation_recno )
		cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno);
	// ###### end Ban 30/6 #######//

	if( random_pick_attack_state( cur_monster_nation_recno, western_nation_recno ) )
	{
		CampaignNation* cNation = get_nation(cur_monster_nation_recno);

		int textId = random(2)+1;		// there are two different sets of text for this event. The program randomly picks one to display.

		disp_narrative( res_event.read( textId==1 ? "1A" : "1A1" ),
			cNation->race_name(), cNation->nation_name() );

		attack_state( attacker_state_recno, target_state_recno, 1, 1, 3 );

		disp_letter( 0, res_event.read( textId==1 ? "1B" : "1B1" ), cNation->race_name(),
			cNation->nation_name(), minister_name(MINISTER_OF_WAR) );
	}
	else	// if the current monster nation does not have a state next to the player's kingdom, randomly pick another monster nation
	{
		cur_monster_nation_recno = random_pick_monster_campaign_nation();
	}

	//----------- next event -------------//

	set_event( EVENT_RANDOM_POINT_1 );

	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_takeover ---//


//-- Begin of function CampaignEastWest::event_random_point_1 --//
//
// Event 2
//
int CampaignEastWest::event_random_point_1()
{
	//--- rotate the active monster nation ----//

	cur_monster_nation_recno = random_pick_monster_campaign_nation();

	//--- if there are no Fryhtans left, go to Event:Random Point 4 ---//

	if( cur_monster_nation_recno==0 )
	{
		set_event( EVENT_RANDOM_POINT_4 );
		return 1;
	}

	//------- Fryhtan kingdom break up if it is beaten too much ------//

	if( m.random(2)==0 &&
		 !has_event_run( EVENT_FRYHTAN_BREAK_UP ) &&
		 state_array.nation_state_count(cur_monster_nation_recno) >= 2 )		// the Fryhtan kingdom should at least have 2 states
	{
		if( player_beat_fryhtan_count >= player_beat_fryhtan_max ||
			 (player_beat_fryhtan_count >= 1 && eastern_nation_recno==0) ||
			 m.random((player_beat_fryhtan_max-player_beat_fryhtan_count)/2)==0 )
		{
			set_event( EVENT_FRYHTAN_BREAK_UP );
			return 1;
		}
	}

	//--- hero debut if the player has too few states left or the Eastern empire is destroyed ---//

	if( state_array.nation_state_count(western_nation_recno) <= hero_debut_state_count ||
		 eastern_nation_recno==0 )
	{
		if( random_pick_attack_state(cur_monster_nation_recno, western_nation_recno) )
		{
			if( run_stage_once(STAGE_HERO_DEBUT) )
				return 1;
		}
	}

	err_when( state_array.nation_state_count(western_nation_recno)==1 );

	//----------------------------------------//

	m.randomize_chance();

	if( m.chance(16) || m.random(6)==0 )		// 1/6 chance X 2
	{
		if( state_array.nation_state_count(western_nation_recno) > 1 )
		{
			 if( run_event_once(EVENT_STATE_BETRAYAL) )
				return 1;
		}

		return 0;		// also return so that random point 1 will be run again and make the randomization more accurate
	}

	if( m.chance(16) && state_array.nation_state_count(western_nation_recno) > 1 )		// Fryhtan will demand one state
	{
		if( run_event_once(EVENT_FRYHTAN_OFFER_PEACE) )
			return 1;

		return 0;
	}

	if( m.chance(16) )
	{
		//--- the likelihood of running Invasion 2 is low if Defense has been run ---//

		if( !has_stage_run(STAGE_DEFENSE) || m.random(10) )
		{
			//----- get the nation with the least states -----//

			cur_monster_nation_recno = least_state_nation(-1, western_nation_recno);		// -1 the nation must be a monster nation. the nation must be next to western empire

			//----- randomly pick attacking and target states -----//

			if( random_pick_attack_state(cur_monster_nation_recno, western_nation_recno) )
			{
				run_stage_once(STAGE_FRYHTAN_INVASION_2);
				return 1;
			}
		}

		return 0;
	}

	if( m.chance(32) )
	{
		if( monster_take_over_east_count >= 1 &&
			 !has_event_run(EVENT_EAST_ASK_FOR_JOINT_ATTACK) )
		{
			int playerStateRecno, eastStateRecno, monsterStateRecno;

			if( get_triangle_states2(playerStateRecno, eastStateRecno, monsterStateRecno) )
			{
				return_event_id = EVENT_RANDOM_POINT_1;
				set_event( EVENT_EAST_ASK_FOR_JOINT_ATTACK );
				return 1;
			}
		}

      //------------------------------------//

		cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno, 1);		//1-must next to 

		if( cur_monster_nation_recno )
		{
			return_event_id = EVENT_RANDOM_POINT_1;
			set_event( EVENT_FRYHTAN_INVADE_EAST );
			return 1;
		}
	}

	if( m.random( event_run_count_array[EVENT_FRYHTAN_TAKEOVER-1] )==0 &&
		 state_array.nation_state_count(western_nation_recno) > 1 )
	{
		if( !cur_monster_nation_recno )
			cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno);

		if( cur_monster_nation_recno )
		{
			set_event(EVENT_FRYHTAN_TAKEOVER);
			return 1;
		}
	}

	err_when( state_array.nation_state_count(western_nation_recno)==0 );

	return 0;
}
//--- End of function CampaignEastWest::event_random_point_1 ---//


//---- Begin of function CampaignEastWest::event_state_betrayal ----//
//
// Event 3
//
int CampaignEastWest::event_state_betrayal()
{
	//--- a state of yours betrays you and turns towards the eastern empire ---//

	if( !random_pick_attack_state( eastern_nation_recno, western_nation_recno ) )
	{
		set_event( EVENT_RANDOM_POINT_1 );	// go to random point 1
		return 1;
	}

	disp_letter( 0, res_event.read("3A"), minister_name(MINISTER_OF_WAR) );

	state_change_nation( target_state_recno, eastern_nation_recno );

	//--- if the player does not have any other state next to the captured state, then don't offer the fight back option ---//

	if( !random_pick_state( western_nation_recno, target_state_recno ) )
	{
		set_event( EVENT_RANDOM_POINT_1 );	// go to random point 1
		return 1;
	}

	//------ Do you want to ask Eastern Empire to return the land to you ---//

	disp_letter( 0, res_event.read("3B"), minister_name(MINISTER_OF_WAR) );

	if( detect_letter()==2 )		// don't ask Eastern Empire
	{
		set_event( EVENT_RANDOM_POINT_1 );	// go to random point 1
		return 1;
	}

	//------ Eastern empire refuses your request -------//

	disp_letter( 0, res_event.read("3C"),
		get_nation(eastern_nation_recno)->minister_name(MINISTER_OF_FOREIGN_AFFAIRS3_ASSIST) );

	//---- Do you want to declare war with the eastern empire and attack the betrayed state? -----//

	disp_letter( 0, res_event.read("3D"), minister_name(MINISTER_OF_WAR) );

	if( detect_letter()==2 )		// if NO, go back to random point 1
	{
		set_event( EVENT_RANDOM_POINT_1 );
		return 1;
	}

	//----- Otherwise go to the game battling the Eastern Empire ----//

	set_stage( STAGE_BATTLE_EAST );
	return 1;
}
//----- End of function CampaignEastWest::event_state_betrayal -----//


//-- Begin of function CampaignEastWest::event_fryhtan_offer_peace --//
//
// Event 4
//
int CampaignEastWest::event_fryhtan_offer_peace()
{
	//--- get a monster nation with the most states (that means it has taken over a state of the player already) ---//

	cur_monster_nation_recno = most_state_nation(-1, western_nation_recno);		// -1 the nation must be a monster nation and it must be next to the western empire

	CampaignNation* cNation = get_nation(cur_monster_nation_recno);

	//------------------------------------------//

	disp_letter( 0, res_event.read("4A"), cNation->race_name(),
		cNation->nation_name(), cNation->minister_name(MINISTER_ORDO), cNation->king_name() );

	int rc = detect_letter();

	if( rc==1 )				// agree to give away land
	{
		//----- give away a state to the monster -----//

		err_when( !cur_monster_nation_recno );

		int rc = random_pick_attack_state(cur_monster_nation_recno, western_nation_recno );

		err_when( !rc );

		state_change_nation( target_state_recno, cur_monster_nation_recno );

		//-------------------------------//

		set_stage( STAGE_BUILD_UP );
		return 1;
	}
	else  	// refuse
	{
		if( random_pick_attack_state(cur_monster_nation_recno, western_nation_recno) )
		{
			set_stage( STAGE_DEFENSE );
		}
		else
		{
			set_event( EVENT_RANDOM_POINT_1 );
		}
	}

	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_offer_peace ---//


//-- Begin of function CampaignEastWest::event_give_throne --//
//
// Event 5
//
int CampaignEastWest::event_give_throne()
{
	disp_narrative( res_event.read("5A"), old_king_name, king_name(), old_king_name, king_name() );

	if( player_pick_attack_target(CAMPAIGN_NATION_MONSTER) )
		set_stage( STAGE_FIGHT_BACK_1 );
	else
		set_event( EVENT_RANDOM_POINT_2 );

	return 1;
}
//--- End of function CampaignEastWest::event_give_throne ---//


//-- Begin of function CampaignEastWest::event_random_point_2 --//
//
// Event 6
//
int CampaignEastWest::event_random_point_2()
{
	//-- 25% chance of going to Game:Train Leader or Game:Grow Population --//

	if( m.random(4)==0 )
	{
		if( m.random(2)==0 && !has_stage_run(STAGE_TRAIN_LEADERS) )
		{
			return_event_id = EVENT_RANDOM_POINT_2;

			set_stage(STAGE_TRAIN_LEADERS);
			return 1;
		}

		else if( !has_stage_run(STAGE_GROW_POPULATION) )
		{
			//--- this game can only run if the player is NOT egyptian and --//
			//-- the player has not acquired the Egyptian scroll of power ---//

			CampaignNation* cNation = get_nation(CAMPAIGN_PLAYER_NATION_RECNO);

			if( cNation->race_id != RACE_EGYPTIAN &&
				 cNation->know_base_array[RACE_EGYPTIAN-1] == 0 )
			{
				return_event_id = EVENT_RANDOM_POINT_2;

				set_stage(STAGE_GROW_POPULATION);
				return 1;
			}
		}
	}

	//------- acquire mega weapon --------//

	if( m.random(4)==0 &&
		 random_pick_attack_state(western_nation_recno, cur_monster_nation_recno) &&
		 !has_stage_run(STAGE_ACQUIRE_MEGA_WEAPON) )
	{
		set_stage( STAGE_ACQUIRE_MEGA_WEAPON );
		return 1;
	}

	//--- if the player's state count >= 4 -----//

	if( state_array.nation_state_count(western_nation_recno) >= 4 &&
		 !has_stage_run( STAGE_DEEP_INTO_FRYHTAN_LAND ) )
	{
		if( player_pick_attack_target(CAMPAIGN_NATION_MONSTER) )
		{
			set_stage( STAGE_DEEP_INTO_FRYHTAN_LAND );
			return 1;
		}
	}

	//------- if the Eastern empire exists -------//

	if( eastern_nation_recno )
	{
		//----- if relationship with Eastern Empire >= Neutral ----//

		if( (get_relation_status(eastern_nation_recno) >= RELATION_NEUTRAL || m.random(2)==0) &&
					m.random( event_run_count(EVENT_EASTERN_EMPIRE_OFFER_HELP)+2 )==0 )		// start with moderate probability and decrease as it has been run more times
		{
			set_event( EVENT_EASTERN_EMPIRE_OFFER_HELP );
			return 1;
		}

		//----- if relationship with Eastern Empire < Neutral ----//

		else if( get_relation_status(eastern_nation_recno) < RELATION_NEUTRAL &&
			 random_pick_attack_state(eastern_nation_recno, western_nation_recno) &&
			 state_array.nation_state_count(western_nation_recno) > 1 &&
			 !defeated_eastern_empire_invasion &&		// the eastern empire will never attack again if it has been defeated.
			 !has_stage_run( STAGE_EASTERN_EMPIRE_INVADE_OUR_LAND ) &&
			 m.random(2)==0 )		// 50% chance
		{
			if( random_pick_attack_state(eastern_nation_recno, western_nation_recno) )
			{
				set_stage( STAGE_EASTERN_EMPIRE_INVADE_OUR_LAND );
				return 1;
			}
		}
	}
	//------- if all Fryhtans are destroyed, go to Random Point 4 -------//

	cur_monster_nation_recno = random_pick_monster_campaign_nation();

	if( cur_monster_nation_recno==0 )
	{
		disp_monster_defeated();

		set_event( EVENT_RANDOM_POINT_4 );
		return 1;
	}

	//------- Fryhtans attack Fryhtans --------//

	if( m.random(2)==0 || has_event_run(EVENT_FRYHTAN_BREAK_UP) )		// if a Fryhtan kingdom has broken up, run the Fryhtan attacks Fryhtan event
	{
		//-- try getting 3 states next to each other, only run the event if the function returns successfully --//

		int humanStateRecno, monsterStateRecno1, monsterStateRecno2;

		if( get_triangle_states1(humanStateRecno, monsterStateRecno1, monsterStateRecno2) )
		{
			if( has_event_run(EVENT_FRYHTAN_ATTACK_FRYHTAN) &&
				 !has_event_run(EVENT_FRYHTAN_ASK_FOR_JOINT_ATTACK) )
			{
				set_event( EVENT_FRYHTAN_ASK_FOR_JOINT_ATTACK );
			}
			else
			{
				set_event( EVENT_FRYHTAN_ATTACK_FRYHTAN );
			}

			return 1;
		}
	}

	//------ Otherwise Game: Fight Back 2 --------//

	if( player_pick_attack_target(CAMPAIGN_NATION_MONSTER) )
	{
		//--- if the Eastern empire is also next to the target ---//

		if( state_array.is_adjacent_to_nation(target_state_recno, eastern_nation_recno) )
		{
			if( m.random(2)==0 && !has_event_run(EVENT_ASK_EASTERN_EMPIRE_FOR_HELP) )
			{
				set_event( EVENT_ASK_EASTERN_EMPIRE_FOR_HELP );
				return 1;
			}
			else if( !has_event_run(EVENT_EAST_ASK_FOR_JOINT_ATTACK) )
			{
				return_event_id = EVENT_RANDOM_POINT_2;
				set_event( EVENT_EAST_ASK_FOR_JOINT_ATTACK );
				return 1;
			}
		}

		set_stage( STAGE_FIGHT_BACK_2 );
		return 1;
	}
	else
	{
		//--- If the player is not next to any of the Fryhtan states (being separated by the Eastern Empire), then have the Fryhtan attacking the Eastern Empire ---//

		cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno);

		if( cur_monster_nation_recno )
		{
			return_event_id = EVENT_RANDOM_POINT_2;
			set_event( EVENT_FRYHTAN_INVADE_EAST );
			return 1;
		}
	}

	return 0;
}
//--- End of function CampaignEastWest::event_random_point_2 ---//


//-- Begin of function CampaignEastWest::event_eastern_empire_offer_help --//
//
// Event 7
//
int CampaignEastWest::event_eastern_empire_offer_help()
{
	///--- get 3 states next to each other with one player state, one eastern empire state and one fryhtan state /

	int humanStateRecno, eastStateRecno, monsterStateRecno;

	if( !get_triangle_states2(humanStateRecno, eastStateRecno, monsterStateRecno) )
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 1;
	}

	//-------------------------------------------//

	disp_letter( 0, res_event.read("7A"),
		get_nation(eastern_nation_recno)->minister_name(MINISTER_OF_FOREIGN_AFFAIRS2) );

	if( detect_letter() == 1 )		// Agree
	{
		attacker_state_recno		 = humanStateRecno;
		attacker2_state_recno	 = eastStateRecno;
		target_state_recno		 = monsterStateRecno;

		cur_monster_nation_recno = state_array[monsterStateRecno]->campaign_nation_recno;

		set_relation_status(western_nation_recno, eastern_nation_recno, RELATION_ALLIANCE);
		set_stage(STAGE_JOINT_FIGHT_BACK);
		return 1;
	}
	else					// Refuse
	{
		set_relation_status(western_nation_recno, eastern_nation_recno, RELATION_NEUTRAL);
		set_event(EVENT_RANDOM_POINT_2);
		return 1;
	}
}
//--- End of function CampaignEastWest::event_eastern_empire_offer_help ---//


//-- Begin of function CampaignEastWest::get_triangle_states2 --//
//
// Get 3 states next to each other with one player state, one eastern
// empire state and one fryhtan state.
//
// <int&> humanStateRecno   - recno of the human state
// <int&> eastStateRecno 	 - recno of the eastern empire state
// <int&> monsterStateRecno - recno of the monster state
//
// return: <int> 1 - succeeded
// 				  0 - failed.
//
int CampaignEastWest::get_triangle_states2(int& humanStateRecno, int& eastStateRecno, int& monsterStateRecno)
{
	for( humanStateRecno=state_array.size() ; humanStateRecno>0 ; humanStateRecno-- )
	{
		//---- look for a player state first ----//

		StateInfo* humanState = state_array[humanStateRecno];

		if( humanState->campaign_nation_recno != western_nation_recno )
			continue;

		//--- if this state is next to states of two different Fryhtan nations ---//

		for( eastStateRecno=state_array.size() ; eastStateRecno>0 ; eastStateRecno-- )
		{
			if( state_array[eastStateRecno]->campaign_nation_recno !=
				 eastern_nation_recno )
			{
				continue;
			}

			//--- if this monster state is next to the current human state ---//

			if( !humanState->is_adjacent(eastStateRecno) )
				continue;

			//--- then look for one more monster state where the human ---//
			//-- kingdom and the first monster nation are both next to ---//

			StateInfo* eastState = state_array[eastStateRecno];

			for( monsterStateRecno=state_array.size() ; monsterStateRecno>0 ; monsterStateRecno-- )
			{
				if( !get_nation(state_array[monsterStateRecno]->campaign_nation_recno)->is_monster() )
					continue;

				if( humanState->is_adjacent(monsterStateRecno) &&
					 eastState->is_adjacent(monsterStateRecno) )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}
//--- End of function CampaignEastWest::get_triangle_states2 ---//


//-- Begin of function CampaignEastWest::event_fryhtan_attack_fryhtan --//
//
// Event 8
//
int CampaignEastWest::event_fryhtan_attack_fryhtan()
{
	///--- get 3 states next to each other with one player state, and two other monster states ---//

	int humanStateRecno, monsterStateRecno1, monsterStateRecno2;

	if( !get_triangle_states1(humanStateRecno, monsterStateRecno1, monsterStateRecno2) )
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 1;
	}

	//-- display the narrative describing the conflict between the two Fryhtan kingdoms --//

	CampaignNation* monsterNation1 = get_nation( state_array[monsterStateRecno1]->campaign_nation_recno);
	CampaignNation* monsterNation2 = get_nation( state_array[monsterStateRecno2]->campaign_nation_recno);

	disp_narrative( res_event.read("8A"), monsterNation1->race_name(), monsterNation1->nation_name(),
		monsterNation2->race_name(), monsterNation2->nation_name() );

	attack_state( monsterStateRecno1, monsterStateRecno2, -1, 1, 3 );		// -1 - the attacker loses

	//-- ask if the player wants to attack a state of either of the Fryhtan kingdoms --//

	disp_letter( 0, res_event.read("8B"), minister_name(MINISTER_OF_WAR),
		monsterNation1->race_name(), monsterNation2->race_name(),
		monsterNation2->race_name(), monsterNation1->race_name()  );

	int rc = detect_letter();

	if( rc==1 )		// attack Fryhtan kingdom A
	{
		cur_monster_nation_recno = state_array[monsterStateRecno2]->campaign_nation_recno;
		attacker_state_recno = humanStateRecno;
		target_state_recno = monsterStateRecno2;

		set_stage( STAGE_OPPORTUNISTIC_ATTACK );
	}
	else if( rc==2 )		// attack Fryhtan kingdom B
	{
		cur_monster_nation_recno = state_array[monsterStateRecno1]->campaign_nation_recno;
		attacker_state_recno = humanStateRecno;
		target_state_recno = monsterStateRecno1;

		set_stage( STAGE_OPPORTUNISTIC_ATTACK );
	}
	else if( rc==3 )		// do not attack
	{
		set_event( EVENT_FRYHTAN_ASK_FOR_JOINT_ATTACK );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_attack_fryhtan ---//


//-- Begin of function CampaignEastWest::get_triangle_states1 --//
//
// This function locates a human state that is next to two Fryhtan kingdom
// states and the two Fryhtan kingdom states must also be next to each other.
//
// <int&> humanStateRecno    - recno of the human state
// <int&> monsterStateRecno1 - recno of the monster 1 state
// <int&> monsterStateRecno2 - recno of the monster 2 state
//
// return: <int> 1 - succeeded
// 				  0 - failed.
//
int CampaignEastWest::get_triangle_states1(int& humanStateRecno, int& monsterStateRecno1, int& monsterStateRecno2)
{
	for( humanStateRecno=state_array.size() ; humanStateRecno>0 ; humanStateRecno-- )
	{
		//---- look for a player state first ----//

		StateInfo* humanState = state_array[humanStateRecno];

		if( humanState->campaign_nation_recno != western_nation_recno )
			continue;

		//--- if this state is next to states of two different Fryhtan nations ---//

		for( monsterStateRecno1=state_array.size() ; monsterStateRecno1>0 ; monsterStateRecno1-- )
		{
			if( !get_nation(state_array[monsterStateRecno1]->campaign_nation_recno)->is_monster() )
				continue;

			//--- if this monster state is next to the current human state ---//

			if( !humanState->is_adjacent(monsterStateRecno1) )
				continue;

			//--- then look for one more monster state where the human ---//
			//-- kingdom and the first monster nation are both next to ---//

			StateInfo* monsterState1 = state_array[monsterStateRecno1];

			for( monsterStateRecno2=state_array.size() ; monsterStateRecno2>0 ; monsterStateRecno2-- )
			{
				if( !get_nation(state_array[monsterStateRecno2]->campaign_nation_recno)->is_monster() )
					continue;

				//--- it must be of a different monster nation ---//

				if( monsterState1->campaign_nation_recno !=
					 state_array[monsterStateRecno2]->campaign_nation_recno )
				{
					if( humanState->is_adjacent(monsterStateRecno2) &&
						 monsterState1->is_adjacent(monsterStateRecno2) )
					{
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
//--- End of function CampaignEastWest::get_triangle_states1 ---//


//-- Begin of function CampaignEastWest::event_fryhtan_ask_for_joint_attack --//
//
// Event 9
//
// cur_monster_nation_recno is the attacking Fryhtan nation
// target_monster_nation_recno is the target Fryhtan nation
//
int CampaignEastWest::event_fryhtan_ask_for_joint_attack()
{
	///--- get 3 states next to each other with one player state, and two other monster states ---//

	int humanStateRecno, monsterStateRecno1, monsterStateRecno2;

	if( !get_triangle_states1(humanStateRecno, monsterStateRecno1, monsterStateRecno2) )
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 1;
	}

	//-------------------------------------------//

	if( m.random(2)==0 )		// either Fryhtan kingdom A or B will ask for a joint attack
	{
		attacker_state_recno = monsterStateRecno1;
		target_state_recno = monsterStateRecno2;
	}
	else
	{
		attacker_state_recno = monsterStateRecno2;
		target_state_recno = monsterStateRecno1;
	}

	attacker2_state_recno = humanStateRecno;

	cur_monster_nation_recno    = state_array[attacker_state_recno]->campaign_nation_recno;
	target_monster_nation_recno = state_array[target_state_recno]->campaign_nation_recno;

	err_when( !get_nation(cur_monster_nation_recno)->is_monster() );

	//-------------------------------------------//

	CampaignNation* monsterNation = get_nation( state_array[attacker_state_recno]->campaign_nation_recno);

	disp_letter( 0, res_event.read("9A"), monsterNation->race_name(),
		monsterNation->minister_name(MINISTER_ORDO), monsterNation->king_name() );

	int rc = detect_letter();

	if( rc == 1 )		// Agree
	{
		fryhtan_joint_attack_reward = 0;
		set_stage(STAGE_FRYHTAN_JOINT_ATTACK);
		return 1;
	}
	else if( rc == 2 )	// Refuse
	{
		if( m.random(2)==0 )	//--- you get Fryhtan mad and it turns against you ---//
		{
			target_state_recno = humanStateRecno;

			set_stage(STAGE_FRYHTAN_RAGE_ATTACK);
			return 1;
		}
		else //--- the Fryhtan attacks the target alone -----//
		{
			if( m.random(2)==0 )		 // the Fryhtan wins
			{
				attack_state(attacker_state_recno, target_state_recno, 1, 1, 3 );

				disp_narrative( res_event.read("9C1"), monsterNation->race_name(),
					monsterNation->king_name() );
			}
			else		// the Fryhtan loses
			{
				attack_state(attacker_state_recno, target_state_recno, -1, 1, 3 );

				disp_narrative( res_event.read("9C2"), monsterNation->race_name(),
					monsterNation->king_name() );
			}

			set_event(EVENT_RANDOM_POINT_2);
			return 1;
		}
	}
	else 				// Demand land
	{
		if( m.random(2)==0 )		// 50% chance the Fryhtan agrees
		{
			disp_letter( 0, res_event.read("9D1"), monsterNation->race_name(),
				monsterNation->minister_name(MINISTER_ORDO), monsterNation->king_name() );

			fryhtan_joint_attack_reward = 1;
			set_stage(STAGE_FRYHTAN_JOINT_ATTACK);
			return 1;
		}
		else					// 50% chance the Fryhtan refuses
		{
			disp_letter( 0, res_event.read("9D2"), monsterNation->race_name(),
				monsterNation->minister_name(MINISTER_ORDO), monsterNation->king_name() );

			target_state_recno = humanStateRecno;

			set_stage(STAGE_FRYHTAN_RAGE_ATTACK);
			return 1;
		}
	}
}
//--- End of function CampaignEastWest::event_fryhtan_ask_for_joint_attack ---//


//-- Begin of function CampaignEastWest::event_random_point_3 --//
//
// Event 10
//
int CampaignEastWest::event_random_point_3()
{
	//-- go to EVENT_DISBAND_FRYHTAN_ARMY if use_fryhtan_count >= use_fryhtan_max )

	if( !disband_fryhtan_reply )
	{
		if( use_fryhtan_count >=1 &&
			 ( use_fryhtan_count >= use_fryhtan_max ||
				m.random(use_fryhtan_max-use_fryhtan_count)==0 ) )
		{
			return_event_id = EVENT_RANDOM_POINT_3;

			set_event( EVENT_DISBAND_FRYHTAN_ARMY );
			return 1;
		}
	}

	//-- if the player refused to disband the Fryhtans, there is a chance that the Fryhtan will revolt. ---//

	if( disband_fryhtan_reply == -1 )
	{
		if( use_fryhtan_count - use_fryhtan_max >= m.random(3) &&
			 packed_nation_count() < MAX_NATION )		// we can still add new nations.
		{
			set_event( EVENT_FRYHTAN_ARMY_REVOLT );
			return 1;
		}
	}

	//-- 25% chance of going to Game:Train Leader or Game:Grow Population --//

	if( m.random(4)==0 )
	{
		if( m.random(2)==0 && !has_stage_run(STAGE_TRAIN_LEADERS) )
		{
			return_event_id = EVENT_RANDOM_POINT_2;

			set_stage(STAGE_TRAIN_LEADERS);
			return 1;
		}

		else if( !has_stage_run(STAGE_GROW_POPULATION) )
		{
			//--- this game can only run if the player is NOT egyptian and --//
			//-- the player has not acquired the Egyptian scroll of power ---//

			CampaignNation* cNation = get_nation(CAMPAIGN_PLAYER_NATION_RECNO);

			if( cNation->race_id != RACE_EGYPTIAN &&
				 cNation->know_base_array[RACE_EGYPTIAN-1] == 0 )
			{
				return_event_id = EVENT_RANDOM_POINT_2;

				set_stage(STAGE_GROW_POPULATION);
				return 1;
			}
		}
	}

	//--- check if there are still Fryhtan states left ---//

	for( int i=state_array.size() ; i>0 ; i-- )
	{
		int cNationRecno = state_array[i]->campaign_nation_recno;

		if( get_nation(cNationRecno)->is_monster() &&
			 state_array.is_adjacent_to_nation(i, CAMPAIGN_PLAYER_NATION_RECNO) )
		{
			//-- if there are still Fryhtan states left ----//

			if( m.random(2)==0 ) 	// start an interactive game
			{
				if( player_pick_attack_target(CAMPAIGN_NATION_MONSTER) )
				{
					set_stage( STAGE_TERMINATE_FRYHTAN );
					return 1;
				}
			}

			//------ otherwise take it over ------//

			cur_monster_nation_recno = random_pick_monster_campaign_nation();

			if( cur_monster_nation_recno &&
				 random_pick_attack_state( western_nation_recno, cur_monster_nation_recno ) )
			{
				set_event( EVENT_PLAYER_TAKE_OVER_FRYHTAN );
				return 1;
			}
		}
	}

	//--- if Fryhtan nations still exist, but Fryhtan nations are separate from human nations by the Eastern empire's territory ---//

	cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno);

	if( cur_monster_nation_recno )
	{
		return_event_id = EVENT_RANDOM_POINT_3;
		set_event( EVENT_FRYHTAN_INVADE_EAST );
		return 1;
	}

	//--- if there are no Fryhtan states left ----//

	if( eastern_nation_recno )		// if the Eastern empire still exists
	{
		disp_monster_defeated();

		set_event( EVENT_ATTEMPT_TO_UNITE );
	}
	else	//--- otherwise, the player achieves victory ---//
	{
		set_event( EVENT_ULTIMATE_VICTORY );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_random_point_3 ---//


//-- Begin of function CampaignEastWest::event_fryhtan_think_surrender --//
//
// Event 11
//
int CampaignEastWest::event_fryhtan_think_surrender()
{
	//--- if 50% chance and there is still a Fryhtan kingdom ---//

	if( !( cur_monster_nation_recno &&					// this function is called after a fryhtan kingdom is deleted, and cur_monster_nation_recno keeps the nation recno of the fryhtan kingdom
			 fryhtan_surrendered_state_recno==0 && 		// only surrender once
			 m.random(3)>0 ) )								// 66% chance
	{
		set_event( EVENT_RANDOM_POINT_3 );
		return 1;
	}

	//--- the Fryhtan kingdom wants to surrender to you. Do you accept? ---//

	CampaignNation* cNation = get_nation(cur_monster_nation_recno);

	disp_letter( 0, res_event.read("11A"), cNation->king_name(), cNation->race_name(),
		cNation->king_name(), cNation->race_name() );

	if( detect_letter()==1 )
	{
		int stateRecno = random_pick_state(cur_monster_nation_recno);

		fryhtan_surrendered_state_recno = stateRecno;

		surrendered_fryhtan_id = get_nation(cur_monster_nation_recno)->monster_id();

		state_change_nation(stateRecno, western_nation_recno);

		//--- ask the player if he wants to use the Fryhtan in his troop ---//

		disp_letter( 0, res_event.read("11B"), cNation->race_name(), minister_name(MINISTER_OF_WAR) );

		if( detect_letter()==1 )
			can_use_fryhtan = 1;			// set this so that in later scenarios, the player can decide whether to send a Fryhtan troop or a human troop
	}

	set_event( EVENT_RANDOM_POINT_3 );
	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_think_surrender ---//


//-- Begin of function CampaignEastWest::event_ask_eastern_empire_for_help --//
//
// Event 12
//
int CampaignEastWest::event_ask_eastern_empire_for_help()
{
	//---------------------------------------------------------//
	// First of all, there must a target Fryhtan state that is
	// next to a Player state and an Eastern empire state.
	//---------------------------------------------------------//

	int playerStateRecno, eastStateRecno, monsterStateRecno;

	if( !get_triangle_states2(playerStateRecno, eastStateRecno, monsterStateRecno) )
	{
		set_event( EVENT_RANDOM_POINT_3 );
		return 0;
	}

	err_when( state_array[playerStateRecno]->campaign_nation_recno != CAMPAIGN_PLAYER_NATION_RECNO );

	//---------------------------------------------------------//

	disp_letter( 0, res_event.read("12A"), minister_name(MINISTER_OF_WAR) );

	if( detect_letter()==1 )		// want to ask for help
	{
		if( m.random(10) < 3 )		// 30% chance the Eastern Empire agrees
		{
			attacker_state_recno	 = playerStateRecno;
			attacker2_state_recno = eastStateRecno;
			target_state_recno	 = monsterStateRecno;

			cur_monster_nation_recno = state_array[monsterStateRecno]->campaign_nation_recno;

			set_stage( STAGE_JOINT_TERMINATE_FRYHTAN );
		}
		else if( state_array.nation_state_count(western_nation_recno) > 1 )		// 70% the Eastern Empire demand land for the help. Only if the western nation has more than one state currently 
		{
			disp_letter( 0, res_event.read("12B"), get_nation(eastern_nation_recno)->minister_name(MINISTER_OF_FOREIGN_AFFAIRS2_ASSIST) );

			if( detect_letter()==1 )		// You agree
			{
				//--- hand a state over to the eastern empire ---//

				random_pick_attack_state( eastern_nation_recno, western_nation_recno, playerStateRecno );		// first try to pick a state next to the eastern empire, exclude playerStateRecno from target choices as it is where the player will send his troop

				int stateRecno = target_state_recno;

				if( !stateRecno )
					stateRecno = random_pick_state( western_nation_recno );		// if not, randomly pick a state in the western empire

				if( stateRecno )
					state_change_nation( stateRecno, eastern_nation_recno );

				//-----------------------------------------//

				attacker_state_recno	 = playerStateRecno;
				attacker2_state_recno = eastStateRecno;
				target_state_recno	 = monsterStateRecno;

				cur_monster_nation_recno = state_array[monsterStateRecno]->campaign_nation_recno;

				set_stage( STAGE_JOINT_TERMINATE_FRYHTAN );
			}
			else			// You refuse
			{
				set_event( EVENT_RANDOM_POINT_3 );
			}
		}
	}
	else			//	do not want to ask for help
	{
		set_event( EVENT_RANDOM_POINT_3 );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_ask_eastern_empire_for_help ---//


//-- Begin of function CampaignEastWest::event_attempt_to_unite --//
//
// Event 13
//
int CampaignEastWest::event_attempt_to_unite()
{
	disp_letter( 0, res_event.read("13A"), minister_name(MINISTER_OF_WAR) );

	//--- propose a peaceful merger with the Eastern Empire ---//

	if( detect_letter()==1 )
	{
		CampaignNation* cNation = get_nation(eastern_nation_recno);

		disp_letter( 0, res_event.read("13B"), cNation->king_name(),
			cNation->minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );

		set_event( EVENT_RANDOM_POINT_4 );
	}
	else //------- attack the Eastern Empire -------//
	{
		int targetNationRecno = player_pick_attack_target(CAMPAIGN_NATION_AI);

		if( targetNationRecno )
		{
			if( targetNationRecno == eastern_nation_recno )
			{
				set_stage( STAGE_ATTACK_EASTERN_EMPIRE_1 );
				return 1;
			}
			else if( targetNationRecno == new_kingdom_nation_recno )
			{
				set_stage( STAGE_ATTACK_NEW_KINGDOM );
				return 1;
			}
		}

		set_event( EVENT_RANDOM_POINT_4 );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_attempt_to_unite ---//


//-- Begin of function CampaignEastWest::event_random_point_4 --//
//
// Event 14
//
int CampaignEastWest::event_random_point_4()
{
	//--- if all states have been conquered by the player ---//

	if( packed_nation_count()==1 || (eastern_nation_recno==0 && new_kingdom_nation_recno==0) )
	{
		err_when( is_nation_deleted(western_nation_recno) );

		set_event( EVENT_ULTIMATE_VICTORY );
		return 1;
	}

	//--- initialize save_player_state_count when this event is first run ---//

	if( saved_player_state_count==0 )
		saved_player_state_count = state_array.nation_state_count(western_nation_recno);

	//--------- randomize chance ----------//

	m.randomize_chance();

	//------ the state turns independent ------//

	if( m.chance(35) && eastern_nation_recno &&
		 new_kingdom_nation_recno==0 &&
		 state_array.nation_state_count(eastern_nation_recno) >= 2 )		// the eastern empire must have at least 2 states for this to happen
	{
		int stateRecno = random_pick_state( eastern_nation_recno );

		if( stateRecno )
		{
			state_change_nation(stateRecno, 0);

			disp_letter( 0, res_event.read("14A"), minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );
		}

		set_event( EVENT_RANDOM_POINT_4 );
		return 1;
	}

	//------ the state turns towards you ------//

	else if( m.chance(20) && eastern_nation_recno )
	{
		int stateRecno = random_pick_state( eastern_nation_recno );

		if( stateRecno )
		{
			state_change_nation(stateRecno, western_nation_recno);
			disp_letter( 0, res_event.read("14B"), minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );
		}

		set_event( EVENT_RANDOM_POINT_4 );
		return 1;
	}

	//--- if there is an independent state and there is no new kingdom, 100% chance that it will form a new kingdom. ---//

	if( state_array.nation_state_count(0) > 0 &&
				new_kingdom_nation_recno == 0 )
	{
		//----- a new kingdom emerges ------//

		new_kingdom_nation_recno = add_nation(CAMPAIGN_NATION_AI);

		//----- pick an independent state --------//

		int stateRecno = random_pick_state(0);		// 0-independent state

		err_when( !stateRecno );

		state_change_nation(stateRecno, new_kingdom_nation_recno);

		//-------- display letter --------//

		disp_letter( 0, res_event.read("14C"), minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );

		set_event( EVENT_RANDOM_POINT_4 );
		return 1;
	}

	//-----------------------------------------------------------------//
	// If Player's current state count - saved_player_state_count >= 2
	// and 66% chance, go to Game: Uprising.
	//-----------------------------------------------------------------//

	if( !has_stage_run( STAGE_UPRISING ) )
	{
		if( (state_array.nation_state_count(western_nation_recno) -
			 saved_player_state_count >= 2 && m.random(2)==0) || m.random(4)==0 )
		{
			set_stage( STAGE_UPRISING );
			return 1;
		}
	}

	//--- if there is only one rivaling kingdom left and it only has one state remaining, go to Game::the Last Battle ---//

	if( eastern_nation_recno && state_array.nation_state_count(eastern_nation_recno)==1 &&
		 state_array.nation_state_count(new_kingdom_nation_recno)==0 )
	{
		if( random_pick_attack_state(western_nation_recno, eastern_nation_recno) )
		{
			set_stage( STAGE_LAST_BATTLE );
			return 1;
		}
		else
			err_here();
	}

//	err_when( state_array.nation_state_count(eastern_nation_recno)==0 &&
//				 state_array.nation_state_count(new_kingdom_nation_recno)==0 );

//	err_when( state_array.nation_state_count(eastern_nation_recno)==0 &&
//				 state_array.nation_state_count(new_kingdom_nation_recno)>0 );

	//----- if a new kingdom has been formed -----//

	if( new_kingdom_nation_recno && m.random(2)==0 )		// if there is a new kingdom and 50% chance
	{
		if( m.random(2)==0 &&
			 !has_stage_run(STAGE_NEW_KINGDOM_INVASION) &&
			 random_pick_attack_state(new_kingdom_nation_recno, western_nation_recno) )
		{
			set_stage( STAGE_NEW_KINGDOM_INVASION );
			return 1;
		}

		//--- 50% chance or its relationship with the eastern empire is hostile ---//

		//-- new kingdom attacks eastern empire -----//

		else if( eastern_nation_recno &&
					( m.random(2)==0 || get_nation(eastern_nation_recno)->get_relation_status(new_kingdom_nation_recno)==RELATION_HOSTILE ) )
		{
			if( random_pick_attack_state(new_kingdom_nation_recno, eastern_nation_recno) )
			{
				int winFlag = m.random(2);

				disp_strategic_screen();
				attack_state( attacker_state_recno, target_state_recno, winFlag, 1, 3 );		// 50% chance win and 50% chance lose

				char* kingName = get_nation(new_kingdom_nation_recno)->king_name();

				if( winFlag )
				{
					disp_letter( 0, res_event.read("14D1"), kingName,
						minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );
				}
				else
				{
					disp_letter( 0, res_event.read("14D2"), kingName, kingName,
						minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );
				}
			}

			set_event( EVENT_RANDOM_POINT_4 );
			return 1;
		}
	}

	//---- If all of the above conditions are not matched ----//

	//---- if there is a new kingdom, let the player choose between -----//
	//----- attacking the Eastern Empire and attacking the new kingdom ---//

	if( eastern_nation_recno && new_kingdom_nation_recno &&
		 state_array.nation_state_count(eastern_nation_recno) > 1 )    // if Eastern empire only has one state left, run STAGE_LAST_BATTLE instead
	{
		//--- if the new kingdom still exists and the Eastern empire just has one state left, then finish the new kingdom first before continue to attack the Eastern Empire (so that the state of the Eastern empire is always the first state to conquer.)

		if( state_array.nation_state_count(eastern_nation_recno)==1 &&
			 state_array.nation_state_count(new_kingdom_nation_recno) > 1 )
		{
			if( random_pick_attack_state( western_nation_recno, new_kingdom_nation_recno ) )
			{
				set_stage( STAGE_ATTACK_NEW_KINGDOM );
				return 1;
			}
		}
		else
		{
			disp_letter( 0, res_event.read("14E"), minister_name(MINISTER_OF_WAR),
				get_nation(eastern_nation_recno)->nation_name(),
				get_nation(new_kingdom_nation_recno)->nation_name() );

			if( detect_letter()==1 )
			{
				if( random_pick_attack_state( western_nation_recno, eastern_nation_recno ) )
				{
					set_stage( STAGE_ATTACK_EASTERN_EMPIRE_2 );
					return 1;
				}
			}
			else
			{
				if( random_pick_attack_state( western_nation_recno, new_kingdom_nation_recno ) )
				{
					set_stage( STAGE_ATTACK_NEW_KINGDOM );
					return 1;
				}
			}
		}
	}
	else if( eastern_nation_recno &&   	// if Eastern empire only has one state left, run STAGE_LAST_BATTLE instead
			 state_array.nation_state_count(eastern_nation_recno) > 1 )
	{
		int targetNationRecno = player_pick_attack_target(CAMPAIGN_NATION_AI);

		if( targetNationRecno == eastern_nation_recno )
		{
			if( !has_stage_run(STAGE_ATTACK_EASTERN_EMPIRE_2) )
				set_stage( STAGE_ATTACK_EASTERN_EMPIRE_2 );
			else
				set_event( EVENT_PLAYER_TAKE_OVER_EAST );		// take over the state without an interactive game.

			return 1;
		}
		else if( targetNationRecno == new_kingdom_nation_recno )
		{
			set_stage( STAGE_ATTACK_NEW_KINGDOM );
			return 1;
		}
	}
	else if( eastern_nation_recno==0 && new_kingdom_nation_recno )		// the eastern kingdom has been destroyed, but the new kingdom is still alive. 
	{
		if( random_pick_attack_state( western_nation_recno, new_kingdom_nation_recno ) )
		{				
			set_stage( STAGE_ATTACK_NEW_KINGDOM );
			return 1;
		}
	}

	return 0;
}
//--- End of function CampaignEastWest::event_random_point_4 ---//


//-- Begin of function CampaignEastWest::event_ultimate_victory --//
//
// Event 15
//
int CampaignEastWest::event_ultimate_victory()
{
	int curDate = date.julian(game_year, m.random(12)+1, m.random(28)+1);		// randomly set the current date

	disp_letter( 0, res_event.read("15A"), date.date_str(curDate),
		minister_name(MINISTER_OF_WAR) );

	disp_final_victory();

	sys.signal_exit_flag = 2;		// exit to the main menu

	return 1;
}
//--- End of function CampaignEastWest::event_ultimate_victory ---//


//-- Begin of function CampaignEastWest::event_fryhtan_invade_east --//
//
// Event 16
//
int CampaignEastWest::event_fryhtan_invade_east()
{
	//---- Fryhan invades the Eastern Empire ----//

	cur_monster_nation_recno = random_pick_monster_campaign_nation(0, eastern_nation_recno);

	if( !cur_monster_nation_recno || !random_pick_attack_state( cur_monster_nation_recno, eastern_nation_recno ) )
	{
		set_event( return_event_id );
		return 0;
	}

	//------------------------------------//

	CampaignNation* cNation = get_nation(cur_monster_nation_recno);

	disp_narrative( res_event.read( "16A" ) );

	//-- 50% chance that the Fryhtan wins and 50% chance that the Easten Empire wins --//

	int attackResult;

	if( m.random(2)==0 )
		attackResult = 1;
	else
		attackResult = -1;

	attack_state( attacker_state_recno, target_state_recno, attackResult, 1, 3 );

	if( attackResult = 1 )
	{
		disp_narrative( res_event.read("16B") );
	}
	else
	{
		disp_narrative( res_event.read("16C") );
	}

	set_event( return_event_id );
	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_invade_east ---//


//-- Begin of function CampaignEastWest::event_east_ask_for_joint_attack --//
//
// Event 17
//
int CampaignEastWest::event_east_ask_for_joint_attack()
{
	///--- get 3 states next to each other with one player state, one eastern empire state and one fryhtan state /

	int playerStateRecno, eastStateRecno, monsterStateRecno;

	if( !get_triangle_states2(playerStateRecno, eastStateRecno, monsterStateRecno) )
	{
		set_event( return_event_id );
		return 1;
	}

	//-------------------------------------------//

	CampaignNation* cNation = get_nation(eastern_nation_recno);

	disp_letter( 0, res_event.read("17A"),
		cNation->minister_name(MINISTER_OF_FOREIGN_AFFAIRS) );

	if( detect_letter() == 1 )		// Agree
	{
		attacker_state_recno		 = playerStateRecno;
		attacker2_state_recno	 = eastStateRecno;
		target_state_recno		 = monsterStateRecno;

		cur_monster_nation_recno = state_array[monsterStateRecno]->campaign_nation_recno;

		set_relation_status(western_nation_recno, eastern_nation_recno, RELATION_ALLIANCE);
		set_stage(STAGE_EAST_JOINT_ATTACK_FRYHTAN);
		return 1;
	}
	else					// Refuse
	{
		set_relation_status(western_nation_recno, eastern_nation_recno, RELATION_NEUTRAL);
		set_event( return_event_id );
		return 1;
	}
}
//--- End of function CampaignEastWest::event_east_ask_for_joint_attack ---//


//-- Begin of function CampaignEastWest::event_fryhtan_break_up --//
//
// Event 18
//
int CampaignEastWest::event_fryhtan_break_up()
{
	//-- first check if there is space for a new kingdom in nation_array[] --//

	if( packed_nation_count() == MAX_NATION )
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 0;
	}

	//--- see if there is a Fryhtan kingdom with at least 2 states ---//

	for( int i=nation_count() ; i>0 ; i-- )
	{
		if( is_nation_deleted(i) )
			continue;

		if( !get_nation(i)->is_monster() )
			continue;

		if( state_array.nation_state_count(i) >= 2 )
			break;
	}

	if( i==0 )
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 0;
	}

	//------ the nation breaks up into two --------//

	CampaignNation* originalNation = get_nation(i);

	err_when( originalNation->is_human() );

	int newNationRecno = add_nation( CAMPAIGN_NATION_MONSTER, originalNation->race_id );

	int stateRecno = random_pick_state(originalNation->campaign_nation_recno);

	err_when( !stateRecno );

	disp_narrative( res_event.read("18A") );

	state_change_nation( stateRecno, newNationRecno );

	//------- the Fryhtans attack each other -----//

	int attackStateRecno = random_pick_attack_state(originalNation->campaign_nation_recno, newNationRecno);

	if( !attackStateRecno )		// the two states are not next to each other.
	{
		set_event( EVENT_RANDOM_POINT_2 );
		return 0;
	}

	if( m.random(2)==0 )		// 50% swap the two states
	{
		int t = attackStateRecno;
		attackStateRecno = stateRecno;
		stateRecno = t;
	}

	int 	    attackResult = ( m.random(2)==0 ? 1 : -1 );
	CampaignNation* attackerNation = get_nation( state_array[attackStateRecno]->campaign_nation_recno );

	disp_narrative( res_event.read( attackResult==1 ? "18B" : "18C" ),
		attackerNation->king_name() );

	attack_state( attackStateRecno, stateRecno, attackResult, 1, 3 );

	//------- the player take the opportunity to take over one of the weakened state --//

	disp_strategic_screen();

	disp_narrative( res_event.read("18D") );

	attackStateRecno = random_pick_state( western_nation_recno, stateRecno );		// pick a player state that is next to the battled state

	if( attackStateRecno )		// if the player has a state next to the breaking up Fryhtan kingdom
		attack_state( attackStateRecno, stateRecno, 1, 1, 3 );

	//-------- return to Random Point 1 --------//

	set_event( EVENT_RANDOM_POINT_2 );

	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_break_up ---//


//-- Begin of function CampaignEastWest::event_player_take_over_fryhtan --//
//
// Event 19
//
int CampaignEastWest::event_player_take_over_fryhtan()
{
	//--- the player invades a state of the Fryhtan and conquers it ---//

	cur_monster_nation_recno = random_pick_monster_campaign_nation();

	if( !cur_monster_nation_recno ||
	    !random_pick_attack_state( western_nation_recno, cur_monster_nation_recno ) )
	{
		set_event( EVENT_RANDOM_POINT_3 );
		return 0;
	}

	disp_narrative( res_event.read("19A"), get_nation(cur_monster_nation_recno)->king_name() );

	attack_state( attacker_state_recno, target_state_recno, 1, 1, 3 );

	//---- 30% chance running Game:Restore Prosperity ----//

	if( m.random(3)==0 )
	{
		return_event_id = EVENT_RANDOM_POINT_3;		// direct Game:Restore Prosperity to go back to EVENT_POINT_3 when it is finished.

		if( !has_stage_run(STAGE_RESTORE_PROSPERITY) )		// only run once
			set_stage( STAGE_RESTORE_PROSPERITY );
		else
			set_event( EVENT_RANDOM_POINT_3 );
	}
	else
	{
		set_event( EVENT_RANDOM_POINT_3 );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_player_take_over_fryhtan ---//


//-- Begin of function CampaignEastWest::event_disband_fryhtan_army --//
//
// Event 20
//
int CampaignEastWest::event_disband_fryhtan_army()
{
	disp_letter( 0, res_event.read("20A"), minister_name(MINISTER_HOME) );

	if( detect_letter() == 1 )
		disband_fryhtan_reply = 1;		// Agree
	else
		disband_fryhtan_reply = -1;   // Refuse

	err_when( !return_event_id );

	set_event( return_event_id );

	return 1;
}
//--- End of function CampaignEastWest::event_disband_fryhtan_army ---//


//-- Begin of function CampaignEastWest::event_independent_surrender --//
//
// Event 21
//
int CampaignEastWest::event_independent_surrender()
{
	//-- an independent state surrender to the player ---//

	//--- pick an independent state first -----//

	int stateRecno = m.random(state_array.size())+1;

	for( int i=state_array.size() ; i>0 ; i-- )
	{
		if( ++stateRecno > state_array.size() )
			stateRecno = 1;

		if( state_array[stateRecno]->campaign_nation_recno==0 )
			break;
	}

	if( i==0 )     	// no independent state found.
	{
		set_event( EVENT_RANDOM_POINT_4 );
		return 0;
	}

	//--------- display the message ---------//

	StateInfo* stateInfo = state_array[stateRecno];

	disp_narrative( res_event.read("21A") );		// an letter from the independent state about surrendering to you

	state_change_nation( stateRecno, western_nation_recno );

	//------ back to random point 4 --------//

	set_event( EVENT_RANDOM_POINT_4 );

	return 1;
}
//--- End of function CampaignEastWest::event_independent_surrender ---//


//-- Begin of function CampaignEastWest::event_fryhtan_army_revolt --//
//
// Event 22
//
int CampaignEastWest::event_fryhtan_army_revolt()
{
	int revoltStateRecno=0;

	//--------------------------------------------------------------//
	// pick a formerly surrendered state, if it is not valid, then
	// randomly pick a state that is close to the front line.
	//--------------------------------------------------------------//

	if( fryhtan_surrendered_state_recno &&
		 state_array[fryhtan_surrendered_state_recno]->campaign_nation_recno == western_nation_recno )
	{
		revoltStateRecno = fryhtan_surrendered_state_recno;
	}
	else
	{

		//--- randomly pick a player state that is next to an enemy ---//

		for( int i=state_array.size() ; i>0 && revoltStateRecno==0 ; i-- )
		{
			if( state_array[i]->campaign_nation_recno != western_nation_recno )
				continue;

			for( int j=state_array.size() ; j>0 ; j-- )
			{
				if( state_array[j]->campaign_nation_recno != western_nation_recno &&
					 state_array[j]->is_adjacent(i) )
				{
					revoltStateRecno = i;
					break;
				}
			}
		}
	}

	err_when( !revoltStateRecno );

	//----------- display letter -------------//

	disp_narrative( res_event.read("22A") );		// an letter from the independent state about surrendering to you

	//----- the revolting Fryhtans form a new kingdom ------//

	int newNationRecno = add_nation(CAMPAIGN_NATION_AI);

	state_change_nation(revoltStateRecno, newNationRecno);

	//------ return to event random point 3 ------//

	set_event(EVENT_RANDOM_POINT_3);

	return 1;
}
//--- End of function CampaignEastWest::event_fryhtan_army_revolt ---//


//-- Begin of function CampaignEastWest::event_player_take_over_east --//
//
// Event 23
//
int CampaignEastWest::event_player_take_over_east()
{
	//--- the player invades a state of the Eastern Empire and conquers it ---//

	if( !random_pick_attack_state( western_nation_recno, eastern_nation_recno ) )
	{
		set_event( EVENT_RANDOM_POINT_4 );
		return 0;
	}

	disp_narrative( res_event.read("23A") );

	attack_state( attacker_state_recno, target_state_recno, 1, 1, 3 );

	//---- 30% chance running Game:Restore Prosperity ----//

	if( m.random(3)==0 && !has_stage_run(STAGE_RESTORE_PROSPERITY) )
	{
		return_event_id = EVENT_RANDOM_POINT_4;		// direct Game:Restore Prosperity to go back to EVENT_POINT_4 when it is finished.

		set_stage( STAGE_RESTORE_PROSPERITY );
	}
	else
	{
		set_event( EVENT_RANDOM_POINT_4 );
	}

	return 1;
}
//--- End of function CampaignEastWest::event_player_take_over_east ---//

