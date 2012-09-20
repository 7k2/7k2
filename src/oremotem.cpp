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

//Filename    : OREMOTEM.CPP
//Description : Object RemoteMsg

#include <all.h>
#include <ofont.h>
#include <onews.h>
#include <osys.h>
#include <onation2.h>
#include <ounit.h>
#include <oun_grp.h>
#include <ogame.h>
#include <oremote.h>
#include <ofirm.h>
#include <ofirmall.h>
#include <otown.h>
#include <olog.h>
#include <ounitall.h>
#include <ospy.h>
#include <otalkres.h>
#include <olonglog.h> 
#include <ocrc_sto.h>
#include <obox.h>
#include <oworldmt.h>

//---------------- Define variable type ---------------//

typedef void (RemoteMsg::*MsgProcessFP)();

//----------- Define function pointers array -----------//

static MsgProcessFP msg_process_function_array[] =
{
	&RemoteMsg::queue_header,
	&RemoteMsg::queue_trailer,
	&RemoteMsg::new_nation,
	&RemoteMsg::update_game_setting,
	&RemoteMsg::start_game,
	&RemoteMsg::next_frame,
	&RemoteMsg::request_resend,
	&RemoteMsg::tell_send_time,
	&RemoteMsg::set_speed,
	&RemoteMsg::tell_random_seed,
	&RemoteMsg::request_save_game,
	&RemoteMsg::player_quit,

	&RemoteMsg::unit_stop,
	&RemoteMsg::unit_move,
	&RemoteMsg::unit_set_force_move,
	&RemoteMsg::unit_attack,
	&RemoteMsg::unit_assign,
	&RemoteMsg::unit_change_nation,
	&RemoteMsg::unit_build_firm,
	&RemoteMsg::unit_burn,
	&RemoteMsg::units_settle,
	&RemoteMsg::unit_set_guard,
	&RemoteMsg::unit_set_rank,
	&RemoteMsg::unit_dismount,
	&RemoteMsg::unit_reward,
	&RemoteMsg::units_transform,
	&RemoteMsg::unit_resign,
	&RemoteMsg::units_assign_to_ship,
	&RemoteMsg::units_ship_to_beach,
	&RemoteMsg::unit_succeed_king,
	&RemoteMsg::units_return_camp,
	&RemoteMsg::units_formation,
	&RemoteMsg::units_form_turn,
	&RemoteMsg::unit_use_item,
	&RemoteMsg::unit_drop_item,
	&RemoteMsg::unit_hire_hero,
	&RemoteMsg::caravan_change_goods,
	&RemoteMsg::caravan_set_stop,
	&RemoteMsg::caravan_del_stop,
	&RemoteMsg::caravan_selected,
	&RemoteMsg::caravan_set_auto,
	&RemoteMsg::caravan_duplicate,
	&RemoteMsg::ship_unload_unit,
	&RemoteMsg::ship_unload_all_units,
	&RemoteMsg::ship_change_goods,
	&RemoteMsg::ship_set_stop,
	&RemoteMsg::ship_del_stop,
	&RemoteMsg::ship_change_mode,
	&RemoteMsg::ship_selected,
	&RemoteMsg::god_cast,
	&RemoteMsg::change_spy_nation,
	&RemoteMsg::notify_cloaked_nation,
	&RemoteMsg::unit_change_behavior_mode,
	&RemoteMsg::spy_change_notify_flag,
	&RemoteMsg::spy_assassinate,
	&RemoteMsg::units_add_way_point,
	&RemoteMsg::units_clear_has_way_point,
	&RemoteMsg::unit_auto_retreat,
	&RemoteMsg::units_transform_mfort,
	&RemoteMsg::units_go_transform_mfort,

	&RemoteMsg::firm_sell,
	&RemoteMsg::firm_cancel,
	&RemoteMsg::firm_destruct,
	&RemoteMsg::firm_set_repair,
	&RemoteMsg::firm_train_level,
	&RemoteMsg::mobilize_worker,
	&RemoteMsg::mobilize_overseer,
	&RemoteMsg::mobilize_builder,
	&RemoteMsg::firm_toggle_link_firm,
	&RemoteMsg::firm_toggle_link_town,
	&RemoteMsg::firm_pull_town_people,
	&RemoteMsg::firm_set_worker_home,
	&RemoteMsg::firm_set_active_link,
	&RemoteMsg::firm_bribe,
	&RemoteMsg::firm_capture,
	&RemoteMsg::firm_reward,
	&RemoteMsg::firm_recruit,
	&RemoteMsg::firm_worker_count,
	&RemoteMsg::firm_upgrade,
	&RemoteMsg::firm_set_rally_dest,
	&RemoteMsg::firm_clear_rally_dest,

	&RemoteMsg::camp_patrol,
	&RemoteMsg::toggle_camp_patrol,
	&RemoteMsg::camp_promote,
	&RemoteMsg::camp_swap_item,
	&RemoteMsg::inn_hire,
	&RemoteMsg::inn_buy_item,
	&RemoteMsg::market_scrap,
	&RemoteMsg::market_hire_caravan,
	&RemoteMsg::research_start,
	&RemoteMsg::build_weapon,
	&RemoteMsg::cancel_weapon,
	&RemoteMsg::skip_build_weapon,
	&RemoteMsg::build_ship,
	&RemoteMsg::sail_ship,
	&RemoteMsg::skip_build_ship,
	&RemoteMsg::factory_change_product,
	&RemoteMsg::factory_auto_switch,
	&RemoteMsg::base_mobilize_prayer,
	&RemoteMsg::invoke_god,
	&RemoteMsg::invoke_effect,
	&RemoteMsg::set_target_archer,
	&RemoteMsg::cancel_train,
	&RemoteMsg::buy_tech,
	&RemoteMsg::lair_grant,
	&RemoteMsg::change_conversion,
	&RemoteMsg::magic_cast,
	&RemoteMsg::magic_transfer,
	&RemoteMsg::lishorr_attack,
	&RemoteMsg::offensive2_attack,
	
	&RemoteMsg::town_recruit,
	&RemoteMsg::town_skip_recruit,
	&RemoteMsg::town_migrate,
	&RemoteMsg::town_collect_tax,
	&RemoteMsg::town_reward,
	&RemoteMsg::town_toggle_link_firm,
	&RemoteMsg::town_toggle_link_town,
	&RemoteMsg::town_auto_tax,
	&RemoteMsg::town_auto_grant,
	&RemoteMsg::town_grant_independent,
	&RemoteMsg::town_sell,
	&RemoteMsg::town_set_target_wall,
	&RemoteMsg::town_set_repair,

	&RemoteMsg::wall_build,
	&RemoteMsg::wall_destruct,

	&RemoteMsg::spy_cycle_action,
	&RemoteMsg::spy_leave_town,
	&RemoteMsg::spy_leave_firm,
	&RemoteMsg::spy_capture_firm,
	&RemoteMsg::spy_drop_identity,
	&RemoteMsg::spy_reward,
	&RemoteMsg::spy_exposed,
	&RemoteMsg::spy_camouflage,
	&RemoteMsg::spy_steal_tech,
	&RemoteMsg::spy_create_incident,

	&RemoteMsg::send_talk_msg,
	&RemoteMsg::reply_talk_msg,
	&RemoteMsg::nation_contact,
	&RemoteMsg::nation_set_should_attack,
	&RemoteMsg::nation_research,
	&RemoteMsg::nation_repair_all_building,

	&RemoteMsg::chat,

	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
	&RemoteMsg::compare_remote_object,
};


//---------- Declare static functions ----------//

static void validate_selected_unit_array(short* selectedUnitArray, short& selectedCount);
static short validate_firm(short firmRecno, unsigned flags = 0);
static short validate_town(short townRecno, unsigned flags = 0);

//------ Begin of function RemoteMsg::process_msg -----//
//
void RemoteMsg::process_msg()
{

	if( id==0 )
		return;

	err_when( id<FIRST_REMOTE_MSG_ID || id>LAST_REMOTE_MSG_ID );

	MsgProcessFP msgProcessFP = msg_process_function_array[id-FIRST_REMOTE_MSG_ID];

	(this->*msgProcessFP)();   // call the corrsponding function to return the news process_msg

}
//------- End of function RemoteMsg::process_msg -----//


//-------- Begin of function RemoteMsg::queue_header ---------//
//
// Nothing here, it shouldn't be called at all.
//
void RemoteMsg::queue_header()
{
}
//--------- End of function RemoteMsg::queue_header ---------//


//-------- Begin of function RemoteMsg::queue_trailer ---------//
//
// Nothing here
//
void RemoteMsg::queue_trailer()
{
#ifdef DEBUG
	long_log->printf("Queue trailer of nation %d\n", *(short *)data_buf);
#endif
}
//--------- End of function RemoteMsg::queue_trailer ---------//


//-------- Begin of function RemoteMsg::new_nation ---------//
//
// Create a remote player.
//
// Sent from a client to the host when the client start a new nation.
//
// Client --> MSG_NEW_NATION --> Host
//
// Sent from the host to a newly joined client to update it with all
// existing nations.
//
// Host --> UPDATE_GAME_SETTING --> Client
//
// structure of data_buf:
//
// <Nation> - Nation object of the newly joined nation
//
void RemoteMsg::new_nation()
{
	//-------- create a new nation now --------//

	if( !remote.is_host )		// ignore this message if this is not the host
		return;

	Nation* nationPtr = (Nation*)(this->data_buf);

	int nationRecno = nation_array.new_nation( NATION_HUMAN, NATION_REMOTE, nationPtr->race_id, nationPtr->color_scheme_id, nationPtr->player_id );

//	game.mp_broadcast_setting();
}
//--------- End of function RemoteMsg::new_nation ---------//


//-------- Begin of function RemoteMsg::update_game_setting ---------//
//
// Sent from the host to clients, update the client with the latest
// game settings.
//
// Host --> UPDATE_GAME_SETTING --> Client
//
// structure of data_buf:
//
// <long>     - random seed
// <short>    - The number of nations joined
// <Nation..> - An array of nation objects
//
void RemoteMsg::update_game_setting()
{
	//------- get parameters -----------//

	char* dataPtr = data_buf;

	//------- set random seed -----------//

	misc.set_random_seed(*(long*)dataPtr);
	dataPtr 		    += sizeof(long);

	//------- update nation_array -----------//

	int nationCount  = *(short*)dataPtr;
	dataPtr         += sizeof(short);

	char	  nationType;
	int	  i, ownCount=0;

	nation_array.deinit();		// deinit() first and then recreate it as follows

	for( i=0 ; i<nationCount ; i++ )
	{
		short nationRecno = *(short *)dataPtr;
		dataPtr += sizeof(short);
		DWORD dpPlayerId = *(DWORD *)dataPtr;
		dataPtr += sizeof(DWORD);
		short colorSchemeId = *(short *)dataPtr;
		dataPtr += sizeof(short);
		short raceId = *(short *)dataPtr;
		dataPtr += sizeof(short);

		// int NationType;
		if( dpPlayerId == remote.self_player_id() )	// if this nation is the player's nation
		{
			nationType = NATION_OWN;
			ownCount++;
		}
		else
			nationType = NATION_REMOTE;

		if(nationRecno != nation_array.new_nation( NATION_HUMAN, nationType, raceId, colorSchemeId, dpPlayerId ))
		{
			// nation recno should match across all players
			err.run("Error in transmitting nation data");
		}
	}

	err_when( ownCount>1 );
}
//--------- End of function RemoteMsg::update_game_setting ---------//


//-------- Begin of function RemoteMsg::start_game ---------//
//
// The host sends MSG_START_GAME to the clients to notify them
// to start the game.
//
void RemoteMsg::start_game()
{
	game.started_flag = 1;
}
//--------- End of function RemoteMsg::start_game ---------//


//-------- Begin of function RemoteMsg::next_frame ---------//
//
// Notify the others that we are ready to proceed to the next
// frame.
//
// structure of data_buf:
//
// <short> - nation recno of the message queue
//
void RemoteMsg::next_frame()
{
	short nationRecno = *((short*)data_buf);

	if( !nation_array.is_deleted(nationRecno) )
	{
		nation_array[nationRecno]->next_frame_ready=1;
	}
}
//--------- End of function RemoteMsg::next_frame ---------//


//-------- Begin of function RemoteMsg::request_resend ---------//
//
// Request a specific player to resend its packets.
//
// structure of data_buf:
//
// <DWORD> - player id.
// <DWORD> - frame count of the message queue
//
void RemoteMsg::request_resend()
{
	DWORD *dwordPtr = (DWORD*) data_buf;

	err_when( dwordPtr[0] == (~nation_array)->player_id );   // sent to itself

	remote.send_backup_now(dwordPtr[0], dwordPtr[1]);
}
//--------- End of function RemoteMsg::request_resend ---------//


//-------- Begin of function RemoteMsg::tell_send_time ---------//
//
// Display the delivery time of the packet
//
void RemoteMsg::tell_send_time()
{
	String str;

	unsigned long sendTime = *((unsigned long*)data_buf);

	str  = "Packet Delivery Time: ";
	str += misc.get_time() - sendTime;
	str += " ms ";
	str += misc.get_time();

	// font_san.disp( ZOOM_X1, 4, str, ZOOM_X1+249);
}
//--------- End of function RemoteMsg::tell_send_time ---------//


//-------- Begin of function RemoteMsg::set_speed ---------//
//
// Order the selected units to stop.
//
// structure of data_buf:
//
// <short>  - the game speed setting.
//
void RemoteMsg::set_speed()
{
	err_when( id != MSG_SET_SPEED);
	short* shortPtr = (short*) data_buf;

	sys.set_speed(shortPtr[0], COMMAND_REMOTE);		// 1-remote call
}
//--------- End of function RemoteMsg::set_speed ---------//


//-------- Begin of function RemoteMsg::tell_random_seed ---------//
//
// structure of data_buf:
//
// <short>  - nation recno
// <long>   - random seed
//
void RemoteMsg::tell_random_seed()
{
	char *p = data_buf;
	short nationRecno = *(short *)p;
	p += sizeof(short);
	long remoteSeed = *(long *)p;

#if defined(ENABLE_LOG)
	String logLine("remote random seed ");
	logLine += nationRecno;
	logLine += ",";
	logLine += remoteSeed;
	
	LOG_MSG(logLine);
#endif

	// it assume random seed of each nation come in sequence
	// if may fails when connection lost

	static long lastRemoteSeed = -1;
	static short lastNation = 0x7fff;
	if( nationRecno <= lastNation)
	{
		// assume the smallest human nation
		lastRemoteSeed = remoteSeed;
	}
	else
	{
		if( lastRemoteSeed != remoteSeed )
		{
#ifdef DEBUG
//			delete long_log;
//			long_log = NULL;
#endif
			LOG_DUMP;
			if( (remote.sync_test_level & 1) && (remote.sync_test_level >= 0) )
			{
				remote.sync_test_level = ~remote.sync_test_level;	// signal error encountered
				if( sys.debug_session )
					err.run( "Multiplayer Random Seed Sync Error." );
				else
					box.msg( "Multiplayer Random Seed Sync Error." );
			}
		}
	}
	lastNation = nationRecno;
}
//-------- End of function RemoteMsg::tell_random_seed ---------//


//-------- Begin of function RemoteMsg::request_save_game ---------//
void RemoteMsg::request_save_game()
{
	err_when( id != MSG_REQUEST_SAVE );
	// message struct : <DWORD> frame when the game should save
#ifdef DEBUG
	long_log->printf("Request save on %d\n", *(DWORD*)data_buf);
#endif
	sys.mp_request_save( *(DWORD*)data_buf);
}
//-------- End of function RemoteMsg::request_save_game ---------//


//-------- Begin of function RemoteMsg::unit_stop ---------//
//
// Order the selected units to stop.
//
void RemoteMsg::unit_stop()
{
	err_when( id != MSG_UNIT_STOP);
	short* shortPtr = (short*) data_buf;

	// packet structure : <write_mem...>

	unit_group_remote.read_mem( shortPtr+0 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.stop(COMMAND_REMOTE);
	}
}
//--------- End of function RemoteMsg::unit_stop ---------//


//-------- Begin of function RemoteMsg::unit_move ---------//
//
// Order the selected units to move to the specified location.
//
// structure of data_buf:
//
// <short>  - destXLoc
// <short>  - destYLoc
// <short>  - no. of selected unit.
// <char..> - selected unit recno array
//
void RemoteMsg::unit_move()
{
	err_when( id != MSG_UNIT_MOVE);
	short* shortPtr = (short*) data_buf;

	// packet structure : <xLoc> <yLoc> <forceMoveFlag> <write_mem...>

	unit_group_remote.read_mem( shortPtr+3 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.move(shortPtr[0], shortPtr[1], shortPtr[2] != 0, COMMAND_REMOTE);
	}
}
//--------- End of function RemoteMsg::unit_move ---------//


//--------- Begin of function RemoteMsg::unit_set_force_move ---------//
void RemoteMsg::unit_set_force_move()
{
/*
	err_when(id != MSG_UNIT_SET_FORCE_MOVE);

	// packet structure : <unit count> <unit recno>...
	short* shortPtr = (short*) data_buf;
	validate_selected_unit_array(shortPtr+1, shortPtr[0]);

	if( shortPtr[0] > 0)
	{
		int i;
#ifdef DEBUG
		long_log->printf("set force move to , units : ");
		for(i = 0; i < shortPtr[0]; ++i)
		{
			long_log->printf("%d,", shortPtr[1+i]);
			if( i % 20 == 19)
				long_log->printf("\n");
		}
		long_log->printf("\n");
#endif

		for( i = 0; i < shortPtr[0]; ++i )
		{
			unit_array[shortPtr[1+i]]->force_move_flag = 1;
		}
	}
*/
}
//--------- End of function RemoteMsg::unit_set_force_move ---------//


//-------- Begin of function RemoteMsg::unit_attack ---------//
//
// Order the selected units to attack at the specified location.
//
// structure of data_buf:
//
void RemoteMsg::unit_attack()
{
	err_when( id != MSG_UNIT_ATTACK);
	short* shortPtr = (short*) data_buf;

	// packet structure : <target baseobj recno> <write_mem...>

	unit_group_remote.read_mem( shortPtr+1 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.attack( shortPtr[0], COMMAND_REMOTE);
	}
}
//--------- End of function RemoteMsg::unit_attack ---------//


//-------- Begin of function RemoteMsg::unit_assign ---------//
//
// Order the selected units to move to the specified location.
//
// structure of data_buf:
//
// <short>  - destXLoc
// <short>  - destYLoc
// <short>  - no. of selected unit.
// <char..> - selected unit recno array
//
void RemoteMsg::unit_assign()
{
	err_when( id != MSG_UNIT_ASSIGN);
	short* shortPtr = (short*) data_buf;

	// packet structure : <xLoc> <yLoc> <target mobile type> <actionNationRecno> <forceMoveFlag> <write_mem...>

	unit_group_remote.read_mem( shortPtr+5 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.assign(shortPtr[0], shortPtr[1], shortPtr[2], COMMAND_REMOTE, 
			shortPtr[3], shortPtr[4]!=0 );
	}
}
//--------- End of function RemoteMsg::unit_assign ---------//


//-------- Begin of function RemoteMsg::unit_change_nation ---------//
//
// Order the selected units to move to the specified location.
//
// structure of data_buf:
//
// <short>  - newNationRecno
// <short>  - no. of selected unit.
// <char..> - selected unit recno array
//
void RemoteMsg::unit_change_nation()
{
//	short* shortPtr = (short*) data_buf;
//	short* selectedUnitArray = shortPtr+2;
//	int	 selectedCount		 = shortPtr[1];
//	validate_selected_unit_array(selectedUnitArray, selectedCount);
//	unit_array.change_nation( shortPtr[0], selectedUnitArray, selectedCount, COMMAND_REMOTE );   // 1-remote action
}
//--------- End of function RemoteMsg::unit_change_nation ---------//


//------ Begin of static function validate_selected_unit_array ------//
//
// Validate all units in selectedUnitArray, remove deleted units from
// selectedUnitArray.
//
// update also UnitGroup::validate_unit_array
//
static void validate_selected_unit_array(short* selectedUnitArray, short& selectedUnitCount)
{
	for( int i=0 ; i<selectedUnitCount ; i++ )
	{
		Unit* unitPtr;
		if( unit_array.is_deleted(selectedUnitArray[i]) ||
			!(unitPtr = unit_array[selectedUnitArray[i]]) ||
			!unitPtr->is_visible() || !unitPtr->is_nation(remote.nation_processing) )
		{
			memmove( selectedUnitArray+i, selectedUnitArray+i+1, sizeof(short) * (selectedUnitCount-i-1) );
			selectedUnitCount--;
			i--;							// stay with the current recno as the records have been moved. The recno in the current position is actually the next record.
		}
	}
}
//------- End of static function validate_selected_unit_array -------//


//------ Begin of static function validate_firm ------//
//
// return the firmRecno if the firm is controllable by the remote player
//
// bit 0 - skip firm's nation checking
//
static short validate_firm(short firmRecno, unsigned flags)
{
	err_when( !(flags & 1) && remote.nation_processing == 0);
	Firm* firmPtr;
	if( firmRecno && !firm_array.is_deleted(firmRecno) 
		&& (firmPtr = firm_array[firmRecno]) 
		&& ((flags & 1) || firmPtr->nation_recno == remote.nation_processing) )
		return firmRecno;
	else
		return 0;
}
//------ End of static function validate_firm ------//


//------ Begin of static function validate_town ------//
//
// return the townRecno if the town is controllable by the remote player
//
// bit 0 - skip town's nation checking
// 
static short validate_town(short townRecno, unsigned flags)
{
	Town* townPtr;
	err_when( !(flags & 1) && remote.nation_processing == 0);
	if( townRecno && !town_array.is_deleted(townRecno) 
		&& (townPtr = town_array[townRecno]) 
		&& ((flags & 1) || townPtr->nation_recno == remote.nation_processing) )
		return townRecno;
	else
		return 0;
}
//------ End of static function validate_firm ------//


// ------- Begin of function RemoteMsg::unit_build ---------//
void RemoteMsg::unit_build_firm()
{

	err_when( id != MSG_UNIT_BUILD_FIRM);
	// packet structure : <unit recno> <xLoc> <yLoc> <firmId>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 )
	{
#ifdef DEBUG
		long_log->printf("unit %d build firm %d at (%d,%d)\n", shortPtr[0], shortPtr[3], shortPtr[1], shortPtr[2]);
#endif
		unit_array[*shortPtr]->build_firm( shortPtr[1], shortPtr[2],
		shortPtr[3], COMMAND_REMOTE );
	}

}
// ------- End of function RemoteMsg::unit_build_firm ---------//


// ------- Begin of function RemoteMsg::unit_burn ---------//
void RemoteMsg::unit_burn()
{
/*
	err_when( id != MSG_UNIT_BURN);
	// packet structure : <unit recno> <xLoc> <yLoc>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 )
	{
#ifdef DEBUG
		long_log->printf("unit %d burn at (%d,%d)\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
		unit_array[*shortPtr]->burn(shortPtr[1], shortPtr[2], COMMAND_REMOTE);
	}
*/
}
// ------- End of function RemoteMsg::unit_burn ---------//


// ------- Begin of function RemoteMsg::units_settle ---------//
void RemoteMsg::units_settle()
{
	err_when( id != MSG_UNITS_SETTLE);
	// packet structure : <xLoc> <yLoc> <no. of units> <unit recno> ...
	short *shortPtr = (short *)data_buf;

	validate_selected_unit_array(shortPtr+3, shortPtr[2]);

	for( int i = 0; i < shortPtr[2]; ++i )
	{
		unit_array[shortPtr[3+i]]->settle_town(shortPtr[0], shortPtr[1], COMMAND_REMOTE );
	}
}
// ------- End of function RemoteMsg::units_settle ---------//


// ------- Begin of function RemoteMsg::unit_set_guard ---------//
//
// **BUGHERE, this function is no longer needed.
//
void RemoteMsg::unit_set_guard()
{
//	err_when( id != MSG_UNIT_SET_GUARD);
//	// packet structure : <unit recno> <new guard mode 0/1>
//	short *shortPtr = (short *)data_buf;
//	unit_array[*shortPtr]->guard_mode = (char) shortPtr[1];
}
// ------- End of function RemoteMsg::unit_set_guard ---------//


// ------- Begin of function RemoteMsg::unit_set_rank ---------//
void RemoteMsg::unit_set_rank()
{
	err_when( id != MSG_UNIT_SET_RANK);
	// packet structure : <unit recno> <new rank>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	// ignore <new rank> parameter
	//	unit_array[*shortPtr]->set_rank(shortPtr[1]);
	if( unitCount > 0)
	{
#ifdef DEBUG
		long_log->printf("unit %d promote/demote to %d\n", shortPtr[0], shortPtr[1]);
#endif
		switch(unit_array[*shortPtr]->rank_id)
		{
		case RANK_SOLDIER:
			unit_array[*shortPtr]->set_rank(RANK_GENERAL);
			break;
		case RANK_GENERAL:
			unit_array[*shortPtr]->set_rank(RANK_SOLDIER);
			break;
		}
	}
}
// ------- End of function RemoteMsg::unit_set_rank ---------//


// ------- Begin of function RemoteMsg::unit_dismount ---------//
void RemoteMsg::unit_dismount()
{
/*
	err_when( id != MSG_UNIT_DISMOUNT);
	// packet structure : <unit recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 )
	{
#ifdef DEBUG
		long_log->printf("unit %d dismount\n", shortPtr[0]);
#endif
		UnitVehicle *uv = (UnitVehicle *) unit_array[*shortPtr];
		uv->dismount();
	}
*/
}
// ------- End of function RemoteMsg::unit_dismount ---------//


// ------- Begin of function RemoteMsg::unit_reward ---------//
void RemoteMsg::unit_reward()
{
	err_when( id != MSG_UNIT_REWARD);
	// packet structure : <rewarding nation> <unit count> <unit recno> ...

	short *shortPtr = (short *)data_buf;
	validate_selected_unit_array(shortPtr+2, shortPtr[1]);

	for(int i = 0; i < shortPtr[1]; ++i )
	{
		unit_array[shortPtr[i+2]]->reward(shortPtr[0]);
	}
}
// ------- End of function RemoteMsg::unit_reward ---------//


// ------- Begin of function RemoteMsg::units_transform ------//
void RemoteMsg::units_transform()
{
/*
	err_when( id != MSG_UNITS_TRANSFORM );
	// packet structure <this recno> <no. of units> <unit recno> ...
	short *shortPtr = (short *)data_buf;
	validate_selected_unit_array(shortPtr+2, shortPtr[1]);

	if( unit_array.is_deleted(*shortPtr) )
	{
		// if <this recno> is dead, use the first unit in
		// the unit group
		if( shortPtr[1] >= 1)
		{
			*shortPtr = shortPtr[2];	
			unit_array[*shortPtr]->group_transform(COMMAND_REMOTE,
				shortPtr +2, shortPtr[1]);
		}
	}
	else
	{
		unit_array[*shortPtr]->group_transform(COMMAND_REMOTE,
			shortPtr +2, shortPtr[1]);
	}
*/
}
// ------- End of function RemoteMsg::units_transform ------//


// ------- Begin of function RemoteMsg::unit_resign ---------//
void RemoteMsg::unit_resign()
{
	err_when( id != MSG_UNIT_RESIGN);
	// packet structure : <unit recno> <nation recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 && unit_array[*shortPtr]->is_nation(shortPtr[1])
		&& unit_array[*shortPtr]->can_resign() )
	{
#ifdef DEBUG
		long_log->printf("nation %d resigns unit %d\n", shortPtr[1], shortPtr[0]);
#endif
		unit_array[*shortPtr]->resign(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::unit_resign ---------//


// ------- Begin of function RemoteMsg::units_assign_to_ship ---------//
void RemoteMsg::units_assign_to_ship()
{
/*
	err_when( id != MSG_UNITS_ASSIGN_TO_SHIP);
	// packet structure : <xLoc> <yLoc> <ship recno> <no. of units> <divided> <unit recno ...>
	short *shortPtr = (short *)data_buf;
	validate_selected_unit_array(shortPtr+5, shortPtr[3]);

	if( shortPtr[3] > 0)
	{
#ifdef DEBUG
		long_log->printf("assign to ship at (%d,%d), units : ", shortPtr[0], shortPtr[1]);
		for(int i = 0; i < shortPtr[3]; ++i)
		{
			long_log->printf("%d,", shortPtr[5+i]);
			if( i % 20 == 19)
				long_log->printf("\n");
		}
		long_log->printf("\n");
#endif
		unit_array.assign_to_ship(shortPtr[0], shortPtr[1], shortPtr[4],  shortPtr+5, shortPtr[3], COMMAND_REMOTE, shortPtr[2]);
	}
*/
}
// ------- End of function RemoteMsg::units_assign_to_ship ---------//


// ------- Begin of function RemoteMsg::units_ship_to_beach ---------//
void RemoteMsg::units_ship_to_beach()
{
/*
	err_when( id != MSG_UNITS_SHIP_TO_BEACH);
	// packet structure : <xLoc> <yLoc> <no. of units> <divided> <unit recno ...>
	short *shortPtr = (short *)data_buf;
	validate_selected_unit_array(shortPtr+4, shortPtr[2]);

	if( shortPtr[2] > 0)
	{
#ifdef DEBUG
		long_log->printf("move ships to beach at (%d,%d), units : ", shortPtr[0], shortPtr[1]);
		for(int i = 0; i < shortPtr[2]; ++i)
		{
			long_log->printf("%d,", shortPtr[4+i]);
			if( i % 20 == 19)
				long_log->printf("\n");
		}
		long_log->printf("\n");
#endif
		unit_array.ship_to_beach(shortPtr[0], shortPtr[1], shortPtr[3],  shortPtr+4, shortPtr[2], COMMAND_REMOTE);
	}
*/
}
// ------- End of function RemoteMsg::units_assign_to_ship ---------//


// ------- Begin of function RemoteMsg::unit_succeed_king ------//
void RemoteMsg::unit_succeed_king()
{
	err_when( id != MSG_UNIT_SUCCEED_KING);
	// packet structure : <unit recno> <nation recno>
	short *shortPtr = (short *)data_buf;

	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 && !nation_array.is_deleted(shortPtr[1]) &&
		unit_array[*shortPtr]->nation_recno == shortPtr[1]
		&& !nation_array[shortPtr[1]]->king_unit_recno 
		&& unit_array[*shortPtr]->can_succeed_king() )		// prevent pressing succeed twice
	{
#ifdef DEBUG
		long_log->printf("unit %d succeed nation %d\n", shortPtr[0], shortPtr[1]);
#endif
		nation_array[shortPtr[1]]->succeed_king(*shortPtr);
		if(unit_array.selected_recno == *shortPtr)
			info.disp();
	}
}
// ------- End of function RemoteMsg::unit_succeed_king ------//


// ------- Begin of function RemoteMsg::units_return_camp ------//
void RemoteMsg::units_return_camp()
{
	err_when( id != MSG_UNITS_RETURN_CAMP );
	short* shortPtr = (short*) data_buf;

	// packet structure : <write_mem...>

	unit_group_remote.read_mem( shortPtr+0 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.return_camp(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::units_return_camp ------//


// ------- Begin of function RemoteMsg::units_formation ------//
void RemoteMsg::units_formation()
{
	err_when( id != MSG_UNITS_FORMATION );
	short* shortPtr = (short*) data_buf;

	// packet structure : <formation id> <write_mem...>

	unit_group_remote.read_mem( shortPtr+1 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.formation( shortPtr[0], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::units_formation ------//


// ------- Begin of function RemoteMsg::units_form_turn ------//
void RemoteMsg::units_form_turn()
{
	err_when( id != MSG_UNITS_FORM_TURN );
	short* shortPtr = (short*) data_buf;

	// packet structure : <direction id> <write_mem...>

	unit_group_remote.read_mem( shortPtr+1 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.formation_turn( shortPtr[0], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::units_form_turn ------//


// ------- Begin of function RemoteMsg::unit_use_item ------//
void RemoteMsg::unit_use_item()
{
	err_when( id != MSG_UNIT_USE_ITEM);
	// packet structure : <unit recno> <target base obj recno> <para2>
	short *shortPtr = (short *)data_buf;

	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 )
	{
		Unit *unitPtr = unit_array[*shortPtr];
		Item &item = unitPtr->item;

		if ( item.can_turn() )
		{
			if( item.is_turn_on() )
				item.turn_off();
			else
				item.turn_on();
		}
		else if( item.can_use_manually() )
		{
			item.use_manually(*shortPtr, shortPtr[1], shortPtr[2]);
		}
	}
}
// ------- End of function RemoteMsg::unit_use_item ------//


// ------- Begin of function RemoteMsg::unit_drop_item ------//
void RemoteMsg::unit_drop_item()
{
	err_when( id != MSG_UNIT_DROP_ITEM );
	// packet structure : <unit recno>
	short *shortPtr = (short *)data_buf;

	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0 )
	{
		unit_array[*shortPtr]->drop_item( COMMAND_REMOTE );
	}
}
// ------- End of function RemoteMsg::unit_drop_item ------//


// ------- Begin of function RemoteMsg::unit_hire_hero ------//
void RemoteMsg::unit_hire_hero()
{
	err_when( id != MSG_UNIT_HIRE_HERO );
	// packet structure : <unit recno> <nation recno>
	short *shortPtr = (short *)data_buf;

	if( !unit_array.is_deleted(*shortPtr) )
	{
		unit_array[*shortPtr]->hire_hero( shortPtr[1] );
	}
}
// ------- End of function RemoteMsg::unit_hire_hero ------//


// ------- Begin of function RemoteMsg::caravan_change_goods ------//
void RemoteMsg::caravan_change_goods()
{
	err_when( id != MSG_U_CARA_CHANGE_GOODS );
	// packet structure <unit recno> <stop id> <new pick_up_type>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitCaravan *caravanPtr;
		if( unitPtr->unit_id != UNIT_CARAVAN)
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
		long_log->printf("caravan %d change goods row %d, %d\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
			caravanPtr = (UnitCaravan *)unitPtr;
			// caravanPtr->stop_array[shortPtr[1]].pick_up_type = (char) shortPtr[2];
			caravanPtr->set_stop_pick_up(shortPtr[1], shortPtr[2], COMMAND_REMOTE);

			//if( unit_array.selected_recno == *shortPtr )
			//	info.disp();
		}
	}
}
// ------- End of function RemoteMsg::caravan_change_goods ------//


// ------- Begin of function RemoteMsg::caravan_set_stop ------//
void RemoteMsg::caravan_set_stop()
{
	err_when( id != MSG_U_CARA_SET_STOP );
	// packet structure <unit recno> <stop id> <stop x> <stop y>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitCaravan *caravanPtr;
		if( unitPtr->unit_id != UNIT_CARAVAN)
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("caravan %d set stop %d at (%d,%d)\n", shortPtr[0], shortPtr[1], shortPtr[2], shortPtr[3]);
#endif
			caravanPtr = (UnitCaravan *)unitPtr;
			caravanPtr->set_stop(shortPtr[1], shortPtr[2], shortPtr[3], COMMAND_REMOTE);
			// if( unit_array.selected_recno == *shortPtr )
			//	info.disp();
		}
	}
}
// ------- End of function RemoteMsg::caravan_set_stop ------//


// ------- Begin of function RemoteMsg::caravan_del_stop ------//
void RemoteMsg::caravan_del_stop()
{
	err_when( id != MSG_U_CARA_DEL_STOP );
	// packet structure <unit recno> <stop id>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitCaravan *caravanPtr;
		if( unitPtr->unit_id == UNIT_CARAVAN)
		{
#ifdef DEBUG
			long_log->printf("caravan %d delete stop %d, %d\n", shortPtr[0], shortPtr[1]);
#endif
			caravanPtr = (UnitCaravan *)unitPtr;
			caravanPtr->del_stop(shortPtr[1], COMMAND_REMOTE);
			//if( unit_array.selected_recno == *shortPtr )
			//	info.disp();
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::caravan_del_stop ------//


// ------- Begin of function RemoteMsg::caravan_set_auto --------//
void RemoteMsg::caravan_set_auto()
{
	err_when( id != MSG_U_CARA_SET_AUTO );
	// packet structure : <unit recno> <0=manual, 1=auto>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitCaravan *caravanPtr;
		if( unitPtr->unit_id == UNIT_CARAVAN)
		{
#ifdef DEBUG
			long_log->printf("caravan %d set %s \n", shortPtr[0], shortPtr[1] ? "auto" : "manual" );
#endif
			caravanPtr = (UnitCaravan *)unitPtr;
			caravanPtr->default_market_trade_mode = (char) shortPtr[1];
			for( int i = 0; i < MAX_STOP_FOR_CARAVAN; ++i )
				if( caravanPtr->stop_array[i].firm_recno && caravanPtr->stop_array[i].firm_id == FIRM_MARKET )
					caravanPtr->set_stop_pick_up(i+1, shortPtr[1] ? AUTO_PICK_UP : MANUAL_PICK_UP, COMMAND_AUTO);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::caravan_set_auto --------//

// ------- Begin of function RemoteMsg::caravan_duplicate ------//
void RemoteMsg::caravan_duplicate()
{
	err_when( id != MSG_U_CARA_DUPLICATE );
	// packet structure : <sprite recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;

	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitCaravan *caravanPtr;
		if( unitPtr->unit_id == UNIT_CARAVAN)
		{
#ifdef DEBUG
			long_log->printf("caravan %d duplicate\n", shortPtr[0] );
#endif
			caravanPtr = (UnitCaravan *)unitPtr;
			caravanPtr->duplicate_caravan(COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::caravan_duplicate ------//

// ------- Begin of function RemoteMsg::ship_unload_unit ---------//
void RemoteMsg::ship_unload_unit()
{
/*
	err_when( id != MSG_U_SHIP_UNLOAD_UNIT );
	// packet structure <unit recno> <unitSeqId>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		if( unitPtr->sprite_info->sprite_type == 'U' 
			&& unitPtr->sprite_info->sprite_sub_type == 'M')
		{
			UnitMarine *shipPtr = (UnitMarine *)unitPtr;

			if( shortPtr[1] <= shipPtr->unit_count )
			{
				// check if the unit is a ship
#ifdef DEBUG
				long_log->printf("ship %d unload unit %d\n", shortPtr[0], shortPtr[1]);
#endif
				shipPtr->unload_unit(shortPtr[1], COMMAND_REMOTE);
				if( unit_array.selected_recno == *shortPtr )
					info.disp();
			}
		}
		else
		{
			err_here();
		}
	}
*/
}
// ------- End of function RemoteMsg::ship_unload_unit ---------//


// ----- Begin of function RemoteMsg::ship_unload_all_units ----- //
void RemoteMsg::ship_unload_all_units()
{
/*
	err_when( id != MSG_U_SHIP_UNLOAD_ALL_UNITS );
	// packet structure <unit recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		if( unitPtr->sprite_info->sprite_type == 'U' 
			&& unitPtr->sprite_info->sprite_sub_type == 'M')
		{
			// check if the unit is a ship
#ifdef DEBUG
			long_log->printf("ship %d unload all units\n", shortPtr[0]);
#endif
			UnitMarine *shipPtr = (UnitMarine *)unitPtr;
			shipPtr->unload_all_units(COMMAND_REMOTE);
			if( unit_array.selected_recno == *shortPtr )
				info.disp();
		}
		else
		{
			err_here();
		}
	}
*/
}
// ----- End of function RemoteMsg::ship_unload_all_units ----- //


// ------- Begin of function RemoteMsg::ship_change_goods ------//
void RemoteMsg::ship_change_goods()
{
/*
	err_when( id != MSG_U_SHIP_CHANGE_GOODS );
	// packet structure <unit recno> <stop id> <new pick_up_type>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitMarine *shipPtr;
		if( unitPtr->sprite_info->sprite_sub_type != 'M')
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("ship %d change goods, row %d, %d\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
			shipPtr = (UnitMarine *)unitPtr;
			// shipPtr->stop_array[shortPtr[1]].pick_up_type = (char) shortPtr[2];
			shipPtr->set_stop_pick_up(shortPtr[1], shortPtr[2], COMMAND_REMOTE);
			// if( unit_array.selected_recno == *shortPtr )
			//	info.disp();
		}
	}
*/
}
// ------- End of function RemoteMsg::ship_change_goods ------//


// ------- Begin of function RemoteMsg::ship_set_stop ------//
void RemoteMsg::ship_set_stop()
{
/*
	err_when( id != MSG_U_SHIP_SET_STOP );
	// packet structure <unit recno> <stop id> <stop x> <stop y>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitMarine *shipPtr;
		if( unitPtr->sprite_info->sprite_sub_type != 'M')
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("ship %d set stop %d at (%d,%d)\n", shortPtr[0], shortPtr[1], shortPtr[2], shortPtr[3]);
#endif
			shipPtr = (UnitMarine *)unitPtr;
			shipPtr->set_stop(shortPtr[1], shortPtr[2], shortPtr[3], COMMAND_REMOTE);
		}
	}
*/
}
// ------- End of function RemoteMsg::ship_set_stop ------//


// ------- Begin of function RemoteMsg::ship_del_stop ------//
void RemoteMsg::ship_del_stop()
{
/*
	err_when( id != MSG_U_SHIP_DEL_STOP );
	// packet structure <unit recno> <stop id>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitMarine *shipPtr;
		if( unitPtr->sprite_info->sprite_sub_type != 'M')
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("ship %d delete stop %d\n", shortPtr[0], shortPtr[1]);
#endif
			shipPtr = (UnitMarine *)unitPtr;
			shipPtr->del_stop(shortPtr[1], COMMAND_REMOTE);
		}
	}
*/
}
// ------- End of function RemoteMsg::ship_del_stop ------//


// ------- Begin of function RemoteMsg::ship_change_mode ------//
void RemoteMsg::ship_change_mode()
{
/*
	err_when( id != MSG_U_SHIP_CHANGE_MODE );
	// packet structure <unit recno> <new mode>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		UnitMarine *shipPtr;
		if( unitPtr->sprite_info->sprite_sub_type != 'M')
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("ship %d changes mode %d\n", shortPtr[0], shortPtr[1]);
#endif
			shipPtr = (UnitMarine *)unitPtr;
			shipPtr->auto_mode = (char) shortPtr[1];

			if(*shortPtr==unit_array.selected_recno)
				info.disp();
		}
	}
*/
}
// ------- End of function RemoteMsg::ship_change_mode ------//


// ------- Begin of function RemoteMsg::god_cast -------//
void RemoteMsg::god_cast()
{

	err_when( id != MSG_U_GOD_CAST );
	// packet structure : <unit recno> <loc x> <loc y> <power type> <target base obj recno>
	short *shortPtr = (short *)data_buf;

	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if(unitCount > 0)
	{
		Unit *unitPtr = unit_array[*shortPtr];
		err_when(unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_GOD);
#ifdef DEBUG
		long_log->printf("god %d cast power %d at(%d,%d)\n", shortPtr[0], shortPtr[3],
			shortPtr[1], shortPtr[2]);
#endif
		unitPtr->go_cast_power(shortPtr[1], shortPtr[2], COMMAND_REMOTE, shortPtr[4] );
	}

}
// ------- End of function RemoteMsg::god_cast -------//


// ------- Begin of function RemoteMsg::change_spy_nation ------//
void RemoteMsg::change_spy_nation()
{
	err_when( id != MSG_UNIT_SPY_NATION );
	// packet structure <unit recno> <new nation recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
#ifdef DEBUG
		long_log->printf("unit %d changes nation %d\n", shortPtr[0], shortPtr[1]);
#endif
		unit_array[*shortPtr]->spy_change_nation(shortPtr[1], COMMAND_REMOTE);
		if( unit_array.selected_recno == *shortPtr || 
			unit_array[*shortPtr]->selected_flag)
			info.disp();
	}
}
// ------- End of function RemoteMsg::change_spy_nation ------//


// ------- Begin of function RemoteMsg::notify_cloaked_nation ------//
void RemoteMsg::notify_cloaked_nation()
{
	err_when( id != MSG_UNIT_SPY_NOTIFY_CLOAKED_NATION );

	// packet structure <unit recno> <new nation recno>
	short *shortPtr = (short *)data_buf;
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount > 0)
	{
		if( !unit_array[*shortPtr]->spy_recno )
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("unit %d notify cloaked nation %d\n", shortPtr[0], shortPtr[1]);
#endif
			spy_array[unit_array[*shortPtr]->spy_recno]->notify_cloaked_nation_flag
				= (char) shortPtr[1];
		}
		if( unit_array.selected_recno == *shortPtr || 
			unit_array[*shortPtr]->selected_flag)
			info.disp();
	}
}
// ------- End of function RemoteMsg::notify_cloaked_nation ------//


//------ Begin of function RemoteMsg::unit_change_behavior_mode -----//
//
void RemoteMsg::unit_change_behavior_mode()
{
	err_when( id != MSG_UNIT_CHANGE_AGGRESSIVE_MODE);

	// packet structure : <new aggressive mode> <unit count> <unit recno> ...

	short *shortPtr = (short *)data_buf;
	validate_selected_unit_array(shortPtr+2, shortPtr[1]);

	for(int i = 0; i < shortPtr[1]; ++i )
	{
		unit_array[shortPtr[i+2]]->behavior_mode = (char) shortPtr[0];
	}
}
//------- End of function RemoteMsg::unit_change_behavior_mode ------//


//------ Begin of function RemoteMsg::spy_change_notify_flag -----//
//
void RemoteMsg::spy_change_notify_flag()
{
	err_when( id != MSG_SPY_CHANGE_NOTIFY_FLAG) ;

	// packet structure : <spy recno> <new notify flag 0/1>

	short *shortPtr = (short *)data_buf;
	spy_array[*shortPtr]->notify_cloaked_nation_flag = (char) shortPtr[1];
}
//------- End of function RemoteMsg::spy_change_notify_flag ------//


//------ Begin of function RemoteMsg::spy_assassinate -----//
//
void RemoteMsg::spy_assassinate()
{
	err_when( id != MSG_SPY_ASSASSINATE );

	// packet structure : <spy recno> <assassinate target unit recno>

	short *shortPtr = (short *)data_buf;
	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d assassinate unit %d", shortPtr[0], shortPtr[1]);
#endif
		spy_array[*shortPtr]->assassinate( shortPtr[1], COMMAND_REMOTE );
	}
}
//------- End of function RemoteMsg::spy_assassinate ------//


//------- Begin of function RemoteMsg::units_add_way_point -------//
void RemoteMsg::units_add_way_point()
{
/*
	err_when( id != MSG_UNITS_ADD_WAY_POINT);
	short* shortPtr = (short*) data_buf;
	validate_selected_unit_array(shortPtr+3, shortPtr[2]);

	if( shortPtr[2] > 0)
	{
#ifdef DEBUG
		long_log->printf("add way point at (%d,%d)\n", shortPtr[0], shortPtr[1]);
		for(int i = 0; i < shortPtr[2]; ++i)
		{
			long_log->printf("%d,", shortPtr[3+i]);
			if( i % 20 == 19)
				long_log->printf("\n");
		}
		long_log->printf("\n");
#endif
		unit_array.add_way_point(shortPtr[0], shortPtr[1], shortPtr+3, shortPtr[2], COMMAND_REMOTE); // 1-remote action
	}
*/
}
//------- End of function RemoteMsg::units_add_way_point -------//


//------- Begin of function RemoteMsg::units_clear_has_way_point -------//
//
void RemoteMsg::units_clear_has_way_point()
{
	err_when( id != MSG_UNITS_CLEAR_HAS_WAY_POINT );

	// packet structure : <write_mem...>

	short *shortPtr = (short *)data_buf;

	unit_group_remote.read_mem( shortPtr+0 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		for( int i = unit_group_remote.size(); i > 0; --i )
		{
			unit_group_remote.get_unit(i)->has_way_point = false;
		}
	}

}
//------- End of function RemoteMsg::units_clear_has_way_point -------//


//------- Begin of function RemoteMsg::units_transform_mfort -------//
//
void RemoteMsg::units_transform_mfort()
{
	err_when( id != MSG_UNITS_TRANSFORM_MFORT );
	short *shortPtr = (short *)data_buf;
	// packet structure : <unit recno> <firmXLoc> <firmYLoc> < 4 x unit recnos>

	short unitCount = 1;
	short builderCount = MAX_EXTRA_BUILDER;
	if( (validate_selected_unit_array(shortPtr+0, unitCount), unitCount > 0)
		&& (validate_selected_unit_array( shortPtr+3, builderCount), builderCount == MAX_EXTRA_BUILDER) )
	{
		FirmMonsterFortress::transform_from_units( shortPtr[0], shortPtr[1], shortPtr[2], shortPtr+3, COMMAND_REMOTE);
	}
}
//------- End of function RemoteMsg::units_transform_mfort -------//


//------- Begin of function RemoteMsg::units_go_transform_mfort -------//
//
void RemoteMsg::units_go_transform_mfort()
{
	err_when( id != MSG_UNITS_GO_TRANSFORM_MFORT );
	short* shortPtr = (short*) data_buf;

	// packet structure : <xLoc> <yLoc> <write_mem...>

	unit_group_remote.read_mem( shortPtr+2 );
	if( unit_group_remote.validate_unit_array(remote.nation_processing) > 0 )
	{
		unit_group_remote.transform_mfort(shortPtr[0], shortPtr[1], COMMAND_REMOTE);
	}
}
//------- End of function RemoteMsg::units_go_transform_mfort -------//


// ------- Begin of function RemoteMsg::firm_sell ---------//
void RemoteMsg::firm_sell()
{
	err_when( id != MSG_FIRM_SELL);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("sell firm %d\n", shortPtr[0]);
#endif
		firm_array[*shortPtr]->sell_firm(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::firm_sell ---------//


// ------- Begin of function RemoteMsg::firm_cancel ---------//
void RemoteMsg::firm_cancel()
{
	err_when( id != MSG_FIRM_CANCEL);
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("firm %d cancel construction\n", shortPtr[0]);
#endif
		firm_array[*shortPtr]->cancel_construction(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::firm_cancel ---------//


// ------- Begin of function RemoteMsg::firm_destruct ---------//
void RemoteMsg::firm_destruct()
{
	err_when( id != MSG_FIRM_DESTRUCT);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("destruct firm %d\n", shortPtr[0]);
#endif
		firm_array[*shortPtr]->destruct_firm(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::firm_destruct ---------//


// ------- Begin of function RemoteMsg::firm_set_repair ---------//
void RemoteMsg::firm_set_repair()
{
	err_when( id != MSG_FIRM_SET_REPAIR);
	// packet structure : <firm recno> <new setting>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
		firm_array[*shortPtr]->set_repair_flag((char)shortPtr[1], COMMAND_REMOTE);
}
// ------- End of function RemoteMsg::firm_set_repair ---------//


// ------- Begin of function RemoteMsg::firm_train_level ---------//
void RemoteMsg::firm_train_level()
{
//		//**BUGHERE, no more training in game 
//	err_when( id != MSG_FIRM_TRAIN_LEVEL);
//	// packet structure : <firm recno> <new train level>
//	short *shortPtr = (short *)data_buf;
//	if( !firm_array.is_deleted(*shortPtr) )
//		firm_array[*shortPtr]->train_level = (char)shortPtr[1];
}
// ------- End of function RemoteMsg::firm_train_level ---------//


// ------- Begin of function RemoteMsg::mobilize_worker ---------//
void RemoteMsg::mobilize_worker()
{
	err_when( id != MSG_FIRM_MOBL_WORKER);
	// packet structure : <firm recno> <workerId>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("firm %d mobilize worker %d\n", shortPtr[0], shortPtr[1]);
#endif
		Firm *firmPtr = firm_array[*shortPtr];

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *camp = firmPtr->cast_to_FirmCamp();
			if( shortPtr[1] >= 1 && shortPtr[1] <= camp->soldier_count && !camp->soldier_array[shortPtr[1]-1].is_under_training() )
			{
				camp->mobilize_soldier(shortPtr[1], COMMAND_REMOTE);
			}
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			FirmTrain *train = firmPtr->cast_to_FirmTrain();
			if( shortPtr[1] == -1				// mobilize all trainee
				|| shortPtr[1] >= 1 && shortPtr[1] <= train->trainee_count && !train->trainee_array[shortPtr[1]-1].is_under_training )
			{
				train->mobilize_trainee(shortPtr[1], COMMAND_REMOTE);
			}
		}
		else if( firmPtr->cast_to_FirmMonsterTrain() )
		{
			FirmMonsterTrain *train = firmPtr->cast_to_FirmMonsterTrain();
			if( shortPtr[1] == -1				// mobilize all trainee
				|| shortPtr[1] >= 1 && shortPtr[1] <= train->trainee_count && !train->trainee_array[shortPtr[1]-1].is_under_training() )
			{
				train->mobilize_trainee(shortPtr[1], COMMAND_REMOTE);
			}
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::mobilize_worker ---------//


// ------- Begin of function RemoteMsg::mobilize_overseer ---------//
void RemoteMsg::mobilize_overseer()
{
	err_when( id != MSG_FIRM_MOBL_OVERSEER);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("firm %d mobilize overseer\n", shortPtr[0]);
#endif
		Firm *firmPtr = firm_array[*shortPtr];
		if( firmPtr->cast_to_FirmCamp() )
		{
			firmPtr->cast_to_FirmCamp()->assign_overseer(0);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::mobilize_overseer ---------//


// ------- Begin of function RemoteMsg::mobilize_builder ---------//
void RemoteMsg::mobilize_builder()
{
	err_when( id != MSG_FIRM_MOBL_BUILDER);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) && firm_array[*shortPtr]->builder_recno )
	{
#ifdef DEBUG
		long_log->printf("firm %d mobilize builder\n", shortPtr[0]);
#endif
		firm_array[*shortPtr]->set_builder(0);
	}
}
// ------- End of function RemoteMsg::mobilize_builder ---------//


// ------ Begin of function RemoteMsg::firm_toggle_link_firm ----//
void RemoteMsg::firm_toggle_link_firm()
{
	err_when( id != MSG_FIRM_TOGGLE_LINK_FIRM);
	// packet structure : <firm recno> <link Id> <toggle Flag>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
		short linkedFirmRecno = 0;
		if( shortPtr[1] <= firmPtr->linked_firm_count
			&& (linkedFirmRecno = firmPtr->linked_firm_array[shortPtr[1]-1])
			&& validate_firm(linkedFirmRecno, 1) )
		{
#ifdef DEBUG
			long_log->printf("firm %d %s firm link %d\n", shortPtr[0], 
				shortPtr[2] ? "set":"clear", shortPtr[1]);
#endif
			firm_array[*shortPtr]->toggle_firm_link(shortPtr[1], shortPtr[2], COMMAND_REMOTE);
		}
	}
}
// ------ End of function RemoteMsg::firm_toggle_link_firm ----//


// ------ Begin of function RemoteMsg::firm_toggle_link_town ----//
void RemoteMsg::firm_toggle_link_town()
{
	err_when( id != MSG_FIRM_TOGGLE_LINK_TOWN);
	// packet structure : <firm recno> <link Id> <toggle Flag>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
		short linkedTownRecno = 0;
		if( shortPtr[1] <= firmPtr->linked_town_count
			&& (linkedTownRecno = firmPtr->linked_town_array[shortPtr[1]-1])
			&& validate_town(linkedTownRecno, 1) )
		{
#ifdef DEBUG
			long_log->printf("firm %d %s town link %d\n", shortPtr[0], 
				shortPtr[2] ? "set":"clear", shortPtr[1]);
#endif
			firmPtr->toggle_town_link(shortPtr[1], shortPtr[2], COMMAND_REMOTE);

			// update town loyalty if the firm is FIRM_CAMP or FIRM_FORT
			if( firmPtr->cast_to_FirmCamp() )
			{
				Town *townPtr = town_array[linkedTownRecno];
				if( townPtr->nation_recno )
					townPtr->update_target_loyalty();
				else
					townPtr->update_target_resistance();
				townPtr->update_camp_link();
			}

			// ######### begin Gilbert 16/11 #######//
			// -------- set active link of fort -------//
//			if( firmPtr->can_set_active_link(linkedTownRecno) )
//			{
//				if( firmPtr->get_linked_town_status(linkedTownRecno) == LINK_EE )
//				{
//					firmPtr->set_active_link(linkedTownRecno);
//				}
//			}
			// ######### end Gilbert 16/11 #######//
		}
	}
}
// ------ End of function RemoteMsg::firm_toggle_link_town ----//


// ------ Begin of function RemoteMsg::firm_pull_town_people ----//
void RemoteMsg::firm_pull_town_people()
{
/*
	err_when( id != MSG_FIRM_PULL_TOWN_PEOPLE);
	// packet structure : <firm recno> <town recno> <race Id or 0> <force Pull>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) && validate_town(shortPtr[1]) )
	{
#ifdef DEBUG
		long_log->printf("firm %d %s pull race %d from town %d\n", shortPtr[0], 
			shortPtr[3] ? "forcely" : "", shortPtr[2], shortPtr[1]);
#endif
		firm_array[*shortPtr]->pull_town_people(shortPtr[1], COMMAND_REMOTE, shortPtr[2], shortPtr[3]);
	}
*/
}
// ------ End of function RemoteMsg::firm_pull_town_people ----//


// ------ Begin of function RemoteMsg::firm_set_worker_home ----//
void RemoteMsg::firm_set_worker_home()
{
/*
	err_when( id != MSG_FIRM_SET_WORKER_HOME);
	// packet structure : <firm recno> <town recno> <workerId>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) && validate_town(shortPtr[1]) )
	{
#ifdef DEBUG
		long_log->printf("firm %d workder %d migrate to town %d\n", shortPtr[0], shortPtr[2], shortPtr[1]);
#endif
		firm_array[*shortPtr]->set_worker_home_town(shortPtr[1], COMMAND_REMOTE, shortPtr[2]);
	}
*/
}
// ------ End of function RemoteMsg::firm_set_worker_home ----//


// ------ Begin of function RemoteMsg::firm_set_active_link ----//
void RemoteMsg::firm_set_active_link()
{
	err_when( id != MSG_FIRM_SET_ACTIVE_LINK);
	// packet structure : <firm recno> <town recno>
	short *shortPtr = (short *)data_buf;

	// town recno can be zero to clear active link

	if( validate_firm(*shortPtr) && (!shortPtr[1] || validate_town(shortPtr[1])) )
	{
		firm_array[*shortPtr]->set_active_link(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------ End of function RemoteMsg::firm_set_active_link ----//


// ------ Begin of function RemoteMsg::firm_bribe ----//
void RemoteMsg::firm_bribe()
{
	err_when( id != MSG_FIRM_BRIBE);
	// packet structure <firm recno> <spy recno> <bribe target : worker (0=overseer)> <amount>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr, 1) && !spy_array.is_deleted(shortPtr[1]) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
#ifdef DEBUG
		long_log->printf("firm %d, spy %d briber worker %d\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *camp = firmPtr->cast_to_FirmCamp();
			if( shortPtr[2] == 0 && camp->overseer_recno 
				|| shortPtr[2] >= 1 && shortPtr[2] <= camp->soldier_count )
			{
				camp->spy_bribe(shortPtr[3], shortPtr[1], shortPtr[2]);
			}
		}
		else
		{
			err_here();
		}
	}
}
// ------ End of function RemoteMsg::firm_bribe ----//


// ------ Begin of function RemoteMsg::firm_capture ----//
void RemoteMsg::firm_capture()
{
	err_when( id != MSG_FIRM_CAPTURE);
	// packet structure <firm recno> <nation recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr, 1) )
	{
#ifdef DEBUG
		long_log->printf("firm %d, capture by nation %d\n", shortPtr[0], shortPtr[1]);
#endif
		Firm *firmPtr = firm_array[*shortPtr];
		if( firmPtr->cast_to_FirmCamp() )
		{
			firmPtr->cast_to_FirmCamp()->capture_firm(shortPtr[1]);
		}
		else
		{
			err_here();
		}
	}
}
// ------ End of function RemoteMsg::firm_capture ----//


// ------- Begin of function RemoteMsg::firm_reward ---------//
void RemoteMsg::firm_reward()
{
	err_when( id != MSG_FIRM_REWARD);
	// packet structure : <firm recno> <worker id>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
#ifdef DEBUG
		long_log->printf("firm %d reward worker %d\n", shortPtr[0], shortPtr[1]);
#endif
		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *camp = firmPtr->cast_to_FirmCamp();
			if( shortPtr[1] == 0 && camp->overseer_recno ||
				shortPtr[1] >= 1 && shortPtr[1] <= camp->soldier_count )
			{
				camp->reward(shortPtr[1], COMMAND_REMOTE);
			}
		}
	}
}
// ------- End of function RemoteMsg::firm_reward ---------//


// ------- Begin of function RemoteMsg::firm_recruit  -------//
void RemoteMsg::firm_recruit()
{
	err_when( id != MSG_FIRM_RECRUIT);
	// packet structure : <firm recno> [source town recno] [train type]
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
		if( firmPtr->cast_to_FirmFort() )
		{
			firmPtr->cast_to_FirmFort()->recruit_soldier(shortPtr[1], shortPtr[2] != 0, COMMAND_REMOTE);
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			firmPtr->cast_to_FirmTrain()->recruit_trainee(shortPtr[1], shortPtr[2], COMMAND_REMOTE);
		}
		else if( firmPtr->cast_to_FirmLair() )
		{
			firmPtr->cast_to_FirmLair()->recruit_soldier(COMMAND_REMOTE);
		}
		else if( firmPtr->cast_to_FirmMonsterTrain() )
		{
			firmPtr->cast_to_FirmMonsterTrain()->recruit_trainee(shortPtr[2], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::firm_recruit  -------//


// ------- Begin of function RemoteMsg::firm_worker_count -------//
void RemoteMsg::firm_worker_count()
{
	err_when( id != MSG_FIRM_SET_WORKER_COUNT);
	// packet structure : <firm recno> <new needed worker count>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];

		if( firmPtr->cast_to_FirmWork() )
		{
			firmPtr->cast_to_FirmWork()->set_needed_worker_count(shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::firm_worker_count -------//


// ------- Begin of function RemoteMsg::firm_upgrade ---------//
//
void RemoteMsg::firm_upgrade()
{
	err_when( id != MSG_FIRM_UPGRADE);
	// packet structure : <firm recno> <firm id>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];

		firmPtr->start_upgrade( shortPtr[1], COMMAND_REMOTE );
	}
}
// ------- end of function RemoteMsg::firm_upgrade ---------//


// ------- Begin of function RemoteMsg::firm_set_rally_dest ---------//
//
void RemoteMsg::firm_set_rally_dest()
{
	err_when( id != MSG_FIRM_SET_RALLY_DEST);
	// packet structure : <firm recno> <dest baseObjRecno> <x> <y>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		firm_array[*shortPtr]->set_rally_point( shortPtr[1], shortPtr[2], shortPtr[3], COMMAND_REMOTE );
	}
}
// ------- end of function RemoteMsg::firm_set_rally_dest ---------//


// ------- Begin of function RemoteMsg::firm_clear_rally_dest ---------//
//
void RemoteMsg::firm_clear_rally_dest()
{
	err_when( id != MSG_FIRM_CLEAR_RALLY_DEST);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;

	if( validate_firm(*shortPtr) )
	{
		firm_array[*shortPtr]->clear_rally_point( COMMAND_REMOTE );
	}
}
// ------- end of function RemoteMsg::firm_clear_rally_dest ---------//


// ------- Begin of function RemoteMsg::camp_patrol ---------//
void RemoteMsg::camp_patrol()
{
	err_when( id != MSG_F_CAMP_PATROL);
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmCamp *camp = firm_array[*shortPtr]->cast_to_FirmCamp();
		FirmMonsterFortress *fortress = firm_array[*shortPtr]->cast_to_FirmMonsterFortress();
		if(camp)
		{
			if(camp->overseer_recno || camp->soldier_count > 0)
			{
#ifdef DEBUG
				long_log->printf("camp %d patrols\n", *shortPtr);
#endif
				camp->patrol(shortPtr[1]);
			}
		}
		else
		if(fortress)
		{
			if(fortress->archer_count > 0)
			{
#ifdef DEBUG
				long_log->printf("fortress %d patrols\n", *shortPtr);
#endif
				fortress->patrol();
			}
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::camp_patrol ---------//


// ------- Begin of function RemoteMsg::toggle_camp_patrol ---------//
void RemoteMsg::toggle_camp_patrol()
{
	err_when( id != MSG_F_CAMP_TOGGLE_PATROL);
	// packet structure <firm recno> <defense_flag>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmCamp *camp = firm_array[*shortPtr]->cast_to_FirmCamp();
		if(camp)
		{
#ifdef DEBUG
			long_log->printf("camp %d %s patrol flag\n", shortPtr[0],
				shortPtr[1] ? "set":"clear");
#endif
			camp->defense_flag = char(shortPtr[1]);
//			if( firm_array.selected_recno == *shortPtr)
//				info.disp();
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::toggle_camp_patrol ---------//


// ------- Begin of function RemoteMsg::inn_hire ---------//
void RemoteMsg::inn_hire()
{
	err_when( id != MSG_F_INN_HIRE);
	// packet structure : <firm recno>, <hire Id> <nation no> <spy escape>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmInn *inn = firm_array[*shortPtr]->cast_to_FirmInn();
		FirmAnimal *animal = firm_array[*shortPtr]->cast_to_FirmAnimal();

		if(inn)
		{
			if( shortPtr[1] >= 1 && shortPtr[1] <= inn->inn_unit_count )
			{
#ifdef DEBUG
				long_log->printf("inn %d hire %d, by nation %d\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
				inn->hire(shortPtr[1], shortPtr[3]);
				if( shortPtr[2] == nation_array.player_recno)
				{
					inn->put_info(INFO_REPAINT);
				}
			}
		}
		else if(animal)
		{
			if( shortPtr[1] >= 1 && shortPtr[1] <= animal->animal_unit_count )
			{
				animal->hire(shortPtr[1]);
//				if( shortPtr[2] == nation_array.player_recno)
//					inn->put_info(INFO_REPAINT);
			}
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::inn_hire ---------//


// ------- Begin of function RemoteMsg::inn_buy_item ---------//
void RemoteMsg::inn_buy_item()
{
	err_when( id != MSG_F_INN_BUY_ITEM);
	// packet structure : <firm recno>, <inn unid id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmInn *inn = firm_array[*shortPtr]->cast_to_FirmInn();

		if(inn && shortPtr[1] >= 1 && shortPtr[1] <= inn->inn_unit_count )
		{
			inn->buy_item(shortPtr[1], 0);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::inn_hire ---------//


// ------- Begin of function RemoteMsg::market_scrap ---------//
void RemoteMsg::market_scrap()
{
	err_when( id != MSG_F_MARKET_SCRAP );
	// packet structure : <firm recno> <cell no 0-3>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("market %d scrap good row %d\n", shortPtr[0], shortPtr[1]);
#endif
		FirmMarket *firmMarket = firm_array[*shortPtr]->cast_to_FirmMarket();
		if(firmMarket)
		{
			firmMarket->clear_market_goods(shortPtr[1]+1);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::market_scrap ---------//


// ------- Begin of function RemoteMsg::market_hire_caravan ---------//
void RemoteMsg::market_hire_caravan()
{
	err_when( id != MSG_F_MARKET_HIRE_CARA );
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmMarket *market = firm_array[*shortPtr]->cast_to_FirmMarket();
		if(market)
		{
#ifdef DEBUG
			long_log->printf("market %d hire caravan\n", shortPtr[0]);
#endif
			market->hire_caravan(COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::market_hire_caravan ---------//


// ------- Begin of function RemoteMsg::research_start ---------//
void RemoteMsg::research_start()
{
/*
	err_when( id != MSG_F_RESEARCH_START );
	// packet structure : <firm recno> <tech Id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmResearch *research = firm_array[*shortPtr]->cast_to_FirmResearch();
		if(research)
		{
#ifdef DEBUG
			long_log->printf("tower of science %d start research tech %d\n", shortPtr[0], shortPtr[1]);
#endif
			research->start_research(shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
*/
}
// ------- End of function RemoteMsg::research_start ---------//


// ------- Begin of function RemoteMsg::build_weapon ---------//
void RemoteMsg::build_weapon()
{
	err_when( id != MSG_F_WAR_BUILD_WEAPON );
	// packet structure : <firm recno> <unit Id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		if(firm_array[*shortPtr]->cast_to_FirmWar())
		{
			FirmWar *warFactory = firm_array[*shortPtr]->cast_to_FirmWar();
#ifdef DEBUG
			long_log->printf("war factory %d start building unit id %d\n", shortPtr[0], shortPtr[1]);
#endif
			warFactory->add_queue(shortPtr[1]);
		}
		else if( firm_array[*shortPtr]->cast_to_FirmIncubator())
		{
			FirmIncubator *incubator = firm_array[*shortPtr]->cast_to_FirmIncubator();
#ifdef DEBUG
			long_log->printf("incubator %d start building unit id %d\n", shortPtr[0], shortPtr[1]);
#endif
			incubator->add_queue(shortPtr[1]);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::build_weapon ---------//


// ------- Begin of function RemoteMsg::cancel_weapon ---------//
void RemoteMsg::cancel_weapon()
{
	err_when( id != MSG_F_WAR_CANCEL_WEAPON );
	// packet structure : <firm recno> <unit Id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{

		if(firm_array[*shortPtr]->cast_to_FirmWar())
		{
			FirmWar *warFactory = firm_array[*shortPtr]->cast_to_FirmWar();
#ifdef DEBUG
			long_log->printf("war factory %d cancel building unit id %d\n", shortPtr[0], shortPtr[1]);
#endif
			warFactory->remove_queue(shortPtr[1]);
		}
		else if( firm_array[*shortPtr]->cast_to_FirmIncubator())
		{
			FirmIncubator *incubator = firm_array[*shortPtr]->cast_to_FirmIncubator();
#ifdef DEBUG
			long_log->printf("incubator %d cancel building unit id %d\n", shortPtr[0], shortPtr[1]);
#endif
			incubator->remove_queue(shortPtr[1]);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::cancel_weapon ---------//


// ------- Begin of function RemoteMsg::skip_build_weapon ---------//
void RemoteMsg::skip_build_weapon()
{
	err_when( id != MSG_F_WAR_SKIP_WEAPON );
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		if(firm_array[*shortPtr]->cast_to_FirmWar())
		{
			FirmWar *warFactory = firm_array[*shortPtr]->cast_to_FirmWar();
#ifdef DEBUG
			long_log->printf("war factory %d skip weapon building\n", shortPtr[0]);
#endif
			warFactory->cancel_build_unit();
		}
		else if( firm_array[*shortPtr]->cast_to_FirmIncubator() )
		{
			FirmIncubator *incubator = firm_array[*shortPtr]->cast_to_FirmIncubator();
#ifdef DEBUG
			long_log->printf("incubator %d skip weapon building\n", shortPtr[0]);
#endif
			incubator->cancel_build_unit();
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::skip_build_weapon ---------//


// ------- Begin of function RemoteMsg::build_ship ---------//
void RemoteMsg::build_ship()
{
/*
	err_when( id != MSG_F_HARBOR_BUILD_SHIP );
	// packet structure : <firm recno> <unit Id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmHarbor *harbor = firm_array[*shortPtr]->cast_to_FirmHarbor();
		if(!harbor)
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("harbor %d start building unit id %d\n", shortPtr[0], shortPtr[1]);
#endif
			// harbor->build_ship(shortPtr[1], COMMAND_REMOTE);
			if( shortPtr[1] > 0)
				harbor->add_queue(shortPtr[1]);
			else if( shortPtr[1] < 0)
				harbor->remove_queue(-shortPtr[1]);
			else
			{
				err_here();
			}
		}
	}
*/
}
// ------- End of function RemoteMsg::build_ship ---------//


// ------- Begin of function RemoteMsg::sail_ship ---------//
void RemoteMsg::sail_ship()
{
/*
	err_when( id != MSG_F_HARBOR_SAIL_SHIP );
	// packet structure : <firm recno> <browse Recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{

		FirmHarbor *harbor = firm_array[*shortPtr]->cast_to_FirmHarbor();
		if(!harbor)
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("ship %d depart from harbor %d\n", shortPtr[1], shortPtr[0]);
#endif
			harbor->sail_ship(shortPtr[1], COMMAND_REMOTE);
		}
	}
*/
}
// ------- End of function RemoteMsg::sail_ship --------//


// ------- Begin of function RemoteMsg::skip_build_ship ---------//
void RemoteMsg::skip_build_ship()
{
/*
	err_when( id != MSG_F_HARBOR_SKIP_SHIP );
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{

		FirmHarbor *harbor = firm_array[*shortPtr]->cast_to_FirmHarbor();
		if(!harbor)
		{
			err_here();
		}
		else
		{
#ifdef DEBUG
			long_log->printf("harbr %d skip ship building\n", shortPtr[0]);
#endif
			harbor->cancel_build_unit();
		}
	}
*/
}
// ------- End of function RemoteMsg::skip_build_ship ---------//


// ------- Begin of function RemoteMsg::factory_change_product ---------//
void RemoteMsg::factory_change_product()
{
	err_when( id != MSG_F_FACTORY_CHG_PROD );
	// packet structure : <firm recno> <product id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmFactory *factory = firm_array[*shortPtr]->cast_to_FirmFactory();

		if(factory)
		{
#ifdef DEBUG
			long_log->printf("factory %d change product to %d\n", shortPtr[0], shortPtr[1]);
#endif
			//factory->product_raw_id = shortPtr[1];
			//factory->stock_qty = (float) 0;
			//factory->max_stock_qty = (float) DEFAULT_FACTORY_MAX_STOCK_QTY;
			//factory->raw_stock_qty = (float) 0;
			//factory->max_raw_stock_qty = (float) DEFAULT_FACTORY_MAX_RAW_STOCK_QTY;

			factory->change_production(COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::factory_change_product --------//



// ------- Begin of function RemoteMsg::factory_auto_switch ---------//
void RemoteMsg::factory_auto_switch()
{
	err_when( id != MSG_F_FACTORY_AUTO );
	// packet structure : <firm recno> <auto switch flag>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmFactory *factory = firm_array[*shortPtr]->cast_to_FirmFactory();

		if(factory)
		{
#ifdef DEBUG
			long_log->printf("factory %d auto switch to %d\n", shortPtr[0], shortPtr[1]);
#endif
			factory->auto_switch_flag = shortPtr[1];
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::factory_auto_switch --------//


// ------- Begin of function RemoteMsg::base_mobilize_prayer ---------//
void	RemoteMsg::base_mobilize_prayer()
{
	err_when( id != MSG_F_BASE_MOBL_PRAYER);
	err_here();
}
// ------- End of function RemoteMsg::base_mobilize_prayer ---------//


// ------- Begin of function RemoteMsg::invoke_god ---------//
void RemoteMsg::invoke_god()
{
	err_when( id != MSG_F_BASE_INVOKE_GOD);
	// packet structure : <firm recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmBase *base = firm_array[*shortPtr]->cast_to_FirmBase();
		if( base )
		{
			if( base->can_invoke() )
			{
#ifdef DEBUG
				long_log->printf("seat of power %d invoke god\n", shortPtr[0]);
#endif
				base->invoke_god();
			}
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::invoke_god ---------//


// ------- Begin of function RemoteMsg::invoke_effect ---------//
void RemoteMsg::invoke_effect()
{
	err_when( id != MSG_F_BASE_INVOKE_EFFECT );
	// packet structure : <firm recno> <wish id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmBase *base = firm_array[*shortPtr]->cast_to_FirmBase();
		if( base )
		{
			base->invoke_random_effect(shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::invoke_effect ---------//


// ------- Begin of function RemoteMsg::set_target_archer ---------//
void RemoteMsg::set_target_archer()
{
	err_when( id != MSG_F_FORT_TARGET_ARCHER);
	// packet structure : <firm recno> <new target archer>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmFort *fort = firm_array[*shortPtr]->cast_to_FirmFort();
		if( fort )
		{
			fort->set_target_archer(shortPtr[1], COMMAND_REMOTE);
#ifdef DEBUG
			long_log->printf("firm %d set target archer %d\n", shortPtr[0], fort->target_archer_count);
#endif
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::set_target_archer ---------//


// ------- Begin of function RemoteMsg::cancel_train ---------//
void RemoteMsg::cancel_train()
{
	err_when( id != MSG_F_CANCEL_TRAIN);
	// packet structure : <firm recno> <soldier id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		Firm *firmPtr = firm_array[*shortPtr];
		if( firmPtr->cast_to_FirmCamp() )
		{
			firmPtr->cast_to_FirmCamp()->cancel_train_soldier(shortPtr[1], COMMAND_REMOTE);
		}
		else if( firmPtr->cast_to_FirmTrain() )
		{
			firmPtr->cast_to_FirmTrain()->cancel_train_trainee(shortPtr[1], COMMAND_REMOTE);
		}
		else if( firmPtr->cast_to_FirmMonsterTrain() )
		{
			firmPtr->cast_to_FirmMonsterTrain()->cancel_train_trainee(shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::lair_cancel_train ---------//


// ------- Begin of function RemoteMsg::buy_tech ---------//
void RemoteMsg::buy_tech()
{
	err_when( id != MSG_F_LAIR_BUY_TECH);
	// packet structure : <firm recno> <tech id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmLair *lair = firm_array[*shortPtr]->cast_to_FirmLair();
		if( lair )
		{
			lair->buy_tech( shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::buy_tech ---------//


// ------- Begin of function RemoteMsg::lair_grant ---------//
void RemoteMsg::lair_grant()
{
	err_when( id != MSG_F_LAIR_GRANT );
	// packet structure : <firm recno> <grant nation>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr, 1) )		// skip checking firm's nation recno
	{
		FirmLair *lair = firm_array[*shortPtr]->cast_to_FirmLair();
		if( lair )
		{
			lair->grant_to_non_own_lair( shortPtr[1], COMMAND_REMOTE);
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::lair_grant ---------//


// ------- Begin of function RemoteMsg::change_conversion -------//
void RemoteMsg::change_conversion()
{
	err_when( id != MSG_F_ALCH_CONVERSE );
	// packet struct : <firm recno> <new tech id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmMonsterAlchemy *alchemy = firm_array[*shortPtr]->cast_to_FirmMonsterAlchemy();
		if( alchemy )
		{
			alchemy->change_conversion( shortPtr[1], COMMAND_REMOTE );
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::change_conversion -------//


// ------ Begin of function RemoteMsg::magic_cast --------//
void RemoteMsg::magic_cast()
{
	err_when( id != MSG_F_MAGIC_CAST );
	// packet struct : <firm recno> <magic Id> <xLoc> <yLoc> <unit recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmMagic *magic = firm_array[*shortPtr]->cast_to_FirmMagic();
		if( magic )
		{
			magic->cast_magic( shortPtr[1], shortPtr[2], shortPtr[3], shortPtr[4], COMMAND_REMOTE );
		}
		else
		{
			err_here();
		}
	}
}
// ------ End of function RemoteMsg::magic_cast --------//


// ------ Begin of function RemoteMsg::magic_transfer ------//
void RemoteMsg::magic_transfer()
{
	err_when( id != MSG_F_MAGIC_TRANSFER );
	// packet struct : <firm recno> <target firm recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmMagic *magic = firm_array[*shortPtr]->cast_to_FirmMagic();
		if( magic )
		{
			magic->transfer_energy( shortPtr[1], COMMAND_REMOTE );
		}
		else
		{
			err_here();
		}
	}
}
// ------ End of function RemoteMsg::magic_transfer ------//


// ------ Begin of function RemoteMsg::lishorr_attack --------//
void RemoteMsg::lishorr_attack()
{
	err_when( id != MSG_F_LISH_ATTACK );
	// packet struct : <firm recno> <base obj recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmLishorr *firm = firm_array[*shortPtr]->cast_to_FirmLishorr();
		firm->return_target_obj_recno = shortPtr[1];
	}
	else
	{
		err_here();
	}
}
// ------ End of function RemoteMsg::lishorr_attack --------//


// ------ Begin of function RemoteMsg::offensive2_attack --------//
void RemoteMsg::offensive2_attack()
{
	err_when( id != MSG_F_OFF2_ATTACK );
	// packet struct : <firm recno> <base obj recno>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		FirmOffensive2 *firm = firm_array[*shortPtr]->cast_to_FirmOffensive2();
		firm->return_target_obj_recno = shortPtr[1];
	}
	else
	{
		err_here();
	}
}
// ------ End of function RemoteMsg::offensive2_attack --------//


// ------- Begin of function RemoteMsg::town_recruit ---------//
void RemoteMsg::town_recruit()
{
	err_when( id != MSG_TOWN_RECRUIT);
	// packet structure : <town recno> <recruitWagon>
	short *shortPtr = (short *)data_buf;
	if( validate_town(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("town %d train unit\n", shortPtr[0]);
#endif
		town_array[*shortPtr]->recruit( shortPtr[1] != 0, COMMAND_REMOTE );
	}
}
// ------- End of function RemoteMsg::town_recruit ---------//


// ------- Begin of function RemoteMsg::town_skip_recruit ---------//
void RemoteMsg::town_skip_recruit()
{
/*
	err_when( id != MSG_TOWN_SKIP_RECRUIT);
	// packet structure : <town recno>
	short *shortPtr = (short *)data_buf;
	if( validate_town(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("town %d skip unit training\n", shortPtr[0]);
#endif
		town_array[*shortPtr]->cancel_train_unit();

		if( town_array.selected_recno == *shortPtr )
			info.disp();
	}
*/
}
// ------- End of function RemoteMsg::town_skip_recruit ---------//


// ------- Begin of function RemoteMsg::town_migrate ---------//
void RemoteMsg::town_migrate()
{
	err_when( id != MSG_TOWN_MIGRATE);
	// packet structure : <town recno> <dest town recno>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) && validate_town(shortPtr[1]) )
	{
#ifdef DEBUG
		long_log->printf("town %d migrate to town %d\n", shortPtr[0], shortPtr[1]);
#endif
		town_array[*shortPtr]->migrate_to(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::town_migrate ---------//


// ------- Begin of function RemoteMsg::town_collect_tax ---------//
void RemoteMsg::town_collect_tax()
{
	err_when( id != MSG_TOWN_COLLECT_TAX );
	// packet structure : <town recno>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("town %d collect tax\n", shortPtr[0]);
#endif
		town_array[*shortPtr]->collect_tax(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::town_collect_tax ---------//


// ------- Begin of function RemoteMsg::town_reward ---------//
void RemoteMsg::town_reward()
{
	err_when( id != MSG_TOWN_REWARD );
	// packet structure : <town recno>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("town %d reward\n", shortPtr[0]);
#endif
		town_array[*shortPtr]->reward(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::town_reward ---------//


// ------ Begin of function RemoteMsg::town_toggle_link_firm ----//
void RemoteMsg::town_toggle_link_firm()
{
	err_when( id != MSG_TOWN_TOGGLE_LINK_FIRM);
	// packet structure : <town recno> <link Id> <toggle Flag>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("town %d %s firm link %d\n", shortPtr[0], 
			shortPtr[2] ? "set" : "clear", shortPtr[1]);
#endif
		Town *townPtr = town_array[*shortPtr];
		short linkedFirmRecno = 0;
		if( shortPtr[1] <= townPtr->linked_firm_count
			&& (linkedFirmRecno = townPtr->linked_firm_array[shortPtr[1]-1])
			&& validate_firm(linkedFirmRecno, 1) )
		{
			townPtr->toggle_firm_link(shortPtr[1], shortPtr[2], COMMAND_REMOTE);

			// update loyalty if the linked firm is FIRM_BASE
			if( firm_array[linkedFirmRecno]->cast_to_FirmCamp() )
			{
				if(townPtr->nation_recno)
					townPtr->update_target_loyalty();
				else
					townPtr->update_target_resistance();
				townPtr->update_camp_link();
			}
		}
	}
}
// ------ End of function RemoteMsg::town_toggle_link_firm ----//


// ------ Begin of function RemoteMsg::town_toggle_link_town ----//
void RemoteMsg::town_toggle_link_town()
{
	err_when( id != MSG_TOWN_TOGGLE_LINK_TOWN);
	// packet structure : <town recno> <link Id> <toggle Flag>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
		Town *townPtr = town_array[*shortPtr];
		short linkedTownRecno = 0;
		if( shortPtr[1] <= townPtr->linked_town_count
			&& (linkedTownRecno = townPtr->linked_town_array[shortPtr[1]-1])
			&& validate_town(linkedTownRecno, 1) )
		{
#ifdef DEBUG
			long_log->printf("town %d %s town link %d\n", shortPtr[0],
				shortPtr[2] ? "set" : "clear", shortPtr[1]);
#endif
			town_array[*shortPtr]->toggle_town_link(shortPtr[1], shortPtr[2], COMMAND_REMOTE);
		}
	}
}
// ------ End of function RemoteMsg::town_toggle_link_town ----//

//#### begin trevor 5/11 ####//

// ------ Begin of function RemoteMsg::town_auto_tax -------//
void RemoteMsg::town_auto_tax()
{
	err_when( id != MSG_TOWN_AUTO_TAX );
	// packet structure : <town recno> <loyalty level>
	// or <-nation recno> <loyalty level>
	short *shortPtr = (short *)data_buf;

	if( *shortPtr > 0)
	{
		if( validate_town(*shortPtr) )
		{
#ifdef DEBUG
			long_log->printf("town %d auto collect tax at loyal %d\n", shortPtr[0], shortPtr[1]);
#endif
			town_array[*shortPtr]->set_auto_collect_tax_loyalty(shortPtr[1]);
			if( town_array.selected_recno == *shortPtr )
				info.disp();
		}
	}
	else
	{
		short nationRecno = -*shortPtr;
		err_when( !nationRecno );
#ifdef DEBUG
			long_log->printf("nation %d auto collect tax at loyal %d\n", nationRecno, shortPtr[1]);
#endif
		if( !nation_array.is_deleted(nationRecno) )
			nation_array[nationRecno]->set_auto_collect_tax_loyalty(shortPtr[1]);
	}
}
// ------ End of function RemoteMsg::town_auto_tax -------//


// ------ Begin of function RemoteMsg::town_auto_grant -------//
void RemoteMsg::town_auto_grant()
{
	err_when( id != MSG_TOWN_AUTO_GRANT );
	// packet structure : <town recno> <loyalty level>
	// or <-nation recno> <loyalty level>
	short *shortPtr = (short *)data_buf;

	if( *shortPtr > 0 )
	{
		if( validate_town(*shortPtr) )
		{
#ifdef DEBUG
			long_log->printf("town %d auto grant at loyal %d\n", shortPtr[0], shortPtr[1]);
#endif
			town_array[*shortPtr]->set_auto_grant_loyalty(shortPtr[1]);
			if( town_array.selected_recno == *shortPtr )
				info.disp();
		}
	}
	else
	{
		short nationRecno = -*shortPtr;
		err_when( !nationRecno );
#ifdef DEBUG
			long_log->printf("nation %d auto grant at loyal %d\n", nationRecno, shortPtr[1]);
#endif
		if( !nation_array.is_deleted(nationRecno) )
			nation_array[nationRecno]->set_auto_grant_loyalty(shortPtr[1]);
	}
}
// ------ End of function RemoteMsg::unit_auto_retreat -------//

void RemoteMsg::unit_auto_retreat()
{
	err_when( id != MSG_UNIT_AUTO_RETREAT );
	// packet structure : <unit recno> <presentage of hit point to return town>
	// or <-nation recno> <presentage of hit point to return town>
	short *shortPtr = (short *)data_buf;
	if( *shortPtr > 0 )
	{ 
#ifdef DEBUG
			if (unit_array[*shortPtr]->rank_id == RANK_KING)
				long_log->printf("unit King %d auto retreats when hit point drops to %d percentage\n", shortPtr[0], shortPtr[1]);
			else
				long_log->printf("unit General %d auto retreats when hit point drops to %d percentage\n", shortPtr[0], shortPtr[1]);
#endif
			unit_array[*shortPtr]->set_auto_retreat(shortPtr[1]);
		//	if( unit_array.selected_recno == *shortPtr )
		//		info.disp();
	}
	else
	{
		short nationRecno = -*shortPtr;
		err_when( !nationRecno );
#ifdef DEBUG
			long_log->printf("nation %d auto retreats all generals when their hit point drops to %d percentage\n", nationRecno, shortPtr[1]);
#endif
		if( !nation_array.is_deleted(nationRecno) )
			nation_array[nationRecno]->set_auto_retreat(shortPtr[1]);
	}
}
// ------ End of function RemoteMsg::unit_auto_retreat -------//

// ------ Begin of function RemoteMsg::town_grant_independent -------//
void RemoteMsg::town_grant_independent()
{
	err_when( id != MSG_TOWN_GRANT_INDEPENDENT );
	// packet structure : <town recno> <nation recno>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr, 1) && !nation_array.is_deleted(shortPtr[1]) )
	{
		town_array[*shortPtr]->grant_to_non_own_town(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------ Begin of function RemoteMsg::town_grant_independent -------//


// ------- Begin of function RemoteMsg::town_sell --------- //
void RemoteMsg::town_sell()
{
	err_when( id != MSG_TOWN_SELL );
	// packet structure : <town recno>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
		town_array.del_town(*shortPtr);
	}
}
// ------- End of function RemoteMsg::town_sell --------- //


// ------- Begin of function RemoteMsg::town_set_target_wall --------- //
void RemoteMsg::town_set_target_wall()
{
	err_when( id != MSG_TOWN_TARGET_WALL );
	// packet structure : <town recno> <new target level>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
		town_array[*shortPtr]->set_target_wall_level(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::town_set_target_wall --------- //


// ------- Begin of function RemoteMsg::town_set_repair --------- //
void RemoteMsg::town_set_repair()
{
	err_when( id != MSG_TOWN_SET_REPAIR );
	// packet structure : <town recno> <new repair flag>
	short *shortPtr = (short *)data_buf;

	if( validate_town(*shortPtr) )
	{
		town_array[*shortPtr]->set_repair_wall(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::town_set_repair --------- //


// ------- Begin of function RemoteMsg::wall_build---------//
void RemoteMsg::wall_build()
{
/*
	err_when( id != MSG_WALL_BUILD);
	// packet structure : <nation recno> <xLoc> <yLoc>
	short *shortPtr = (short *)data_buf;

	if( !nation_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("nation %d build wall at (%d,%d)\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
		world.build_wall_tile( shortPtr[1], shortPtr[2], shortPtr[0], COMMAND_REMOTE);
	}
*/
}
// ------- End of function RemoteMsg::wall_build---------//


// ------- Begin of function RemoteMsg::wall_destruct ---------//
void RemoteMsg::wall_destruct()
{
/*
	err_when( id != MSG_WALL_DESTRUCT);
	// packet structure : <nation recno> <xLoc> <yLoc>
	short *shortPtr = (short *)data_buf;

	if( !nation_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("nation %d destruct wall at (%d,%d)\n", shortPtr[0], shortPtr[1], shortPtr[2]);
#endif
		world.destruct_wall_tile( shortPtr[1], shortPtr[2], shortPtr[0], COMMAND_REMOTE);
	}
*/
}
// ------- End of function RemoteMsg::wall_build---------//


// ------- Begin of function RemoteMsg::spy_cycle_action -------//
void RemoteMsg::spy_cycle_action()
{
	err_when( id != MSG_SPY_CYCLE_ACTION);
	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d change action\n", shortPtr[0]);
#endif
		spy_array[*shortPtr]->set_next_action_mode();
	}
}
// ------- End of function RemoteMsg::spy_cycle_action -------//

// ------- Begin of function RemoteMsg::spy_leave_town -------//
void RemoteMsg::spy_leave_town()
{
	err_when( id != MSG_SPY_LEAVE_TOWN);
	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d leave town\n", shortPtr[0]);
#endif

		if( spy_array[*shortPtr]->spy_place == SPY_TOWN )
		{
			spy_array[*shortPtr]->mobilize_town_spy();
			spy_array[*shortPtr]->notify_cloaked_nation_flag = 0;
		}
	}
}
// ------- End of function RemoteMsg::spy_leave_town -------//


// ------- Begin of function RemoteMsg::spy_leave_firm -------//
void RemoteMsg::spy_leave_firm()
{
	err_when( id != MSG_SPY_LEAVE_FIRM);
	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d leave firm\n", shortPtr[0]);
#endif

		if( spy_array[*shortPtr]->spy_place == SPY_FIRM )
		{
			spy_array[*shortPtr]->mobilize_firm_spy();
			spy_array[*shortPtr]->notify_cloaked_nation_flag = 0;
		}
	}
}
// ------- End of function RemoteMsg::spy_leave_firm -------//


// ------- Begin of function RemoteMsg::spy_capture_firm -------//
void RemoteMsg::spy_capture_firm()
{
	err_when( id != MSG_SPY_CAPTURE_FIRM);
	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d capture firm\n", shortPtr[0]);
#endif
		spy_array[*shortPtr]->capture_firm();
	}
}
// ------- End of function RemoteMsg::capture_firm -------//


// ------- Begin of function RemoteMsg::spy_drop_identity ------//
void RemoteMsg::spy_drop_identity()
{
	err_when( id != MSG_SPY_DROP_IDENTITY );

	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d drop identity\n", shortPtr[0]);
#endif
		spy_array[*shortPtr]->drop_spy_identity();
	}
}
// ------- End of function RemoteMsg::spy_drop_identity ------//


// ------- Begin of function RemoteMsg::spy_reward ------//
void RemoteMsg::spy_reward()
{
	err_when( id != MSG_SPY_REWARD );

	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d reward\n", shortPtr[0]);
#endif
		spy_array[*shortPtr]->reward(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::spy_reward ------//


// ------- Begin of function RemoteMsg::spy_set_exposed ------//
void RemoteMsg::spy_exposed()
{
	err_when( id != MSG_SPY_EXPOSED );

	// packet structure : <spy recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d set exposed\n", shortPtr[0]);
#endif
		spy_array[*shortPtr]->set_exposed(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::spy_set_exposed ------//


// ------- Begin of function RemoteMsg::spy_camouflage ------//
void RemoteMsg::spy_camouflage()
{
	err_when( id != MSG_SPY_CAMOUFLAGE );

	// packet structure : <spy recno> <0=start, 1=stop>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) )
	{
#ifdef DEBUG
		long_log->printf("spy %d %s camouflage\n", shortPtr[0], shortPtr[1] ? "stop" : "start");
#endif
		if( !shortPtr[1] )
			spy_array[*shortPtr]->start_camouflage(COMMAND_REMOTE);
		else
			spy_array[*shortPtr]->stop_camouflage(COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::spy_set_exposed ------//


// ------- Begin of function RemoteMsg::spy_steal_tech ------//
void RemoteMsg::spy_steal_tech()
{
	err_when( id != MSG_SPY_STEAL_TECH );

	// packet structure : <spy recno> <target nation recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) && !nation_array.is_deleted(shortPtr[1]))
	{
		spy_array[*shortPtr]->steal_tech( shortPtr[1], COMMAND_REMOTE );
	}
}
// ------- End of function RemoteMsg::spy_steal_tech ------//


// ------- Begin of function RemoteMsg::spy_create_incident ------//
void RemoteMsg::spy_create_incident()
{
	err_when( id != MSG_SPY_CREATE_INCIDENT );

	// packet structure : <spy recno> <target nation recno> <involved nation recno>
	short *shortPtr = (short *)data_buf;

	if( !spy_array.is_deleted(*shortPtr) 
		&& !nation_array.is_deleted(shortPtr[1])
		&& !nation_array.is_deleted(shortPtr[2]) )
	{
		spy_array[*shortPtr]->create_incident( shortPtr[1], shortPtr[2], COMMAND_REMOTE );
	}
}
// ------- End of function RemoteMsg::spy_create_incident ------//


// ------- Begin of function RemoteMsg::send_talk_msg -------//
void RemoteMsg::send_talk_msg()
{
	err_when( id != MSG_SEND_TALK_MSG);
	// packet structure : <talkMsg>

	TalkMsg *talkMsg = (TalkMsg *)data_buf;
	if( !nation_array.is_deleted(talkMsg->to_nation_recno) )
	{
#ifdef DEBUG
		long_log->printf("talk message from %d to %d, id %d, para1=%d, para2=%d\n",
			talkMsg->from_nation_recno, talkMsg->to_nation_recno,
			talkMsg->talk_id, talkMsg->talk_para1, talkMsg->talk_para2);
#endif
		talk_res.send_talk_msg( (TalkMsg *)data_buf, COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::send_talk_msg -------//


// ------- Begin of function RemoteMsg::reply_talk_msg -------//
void RemoteMsg::reply_talk_msg()
{
	err_when( id != MSG_REPLY_TALK_MSG);
	// packet structure : <talkRecno:int> <reply type:char> <padding:char>

	//####### begin trevor 28/8 ########//
	
	int talkMsgRecno = *(int*)data_buf;

	if( !talk_res.is_talk_msg_deleted(talkMsgRecno) )
	{
#ifdef DEBUG
		long_log->printf("reply talk message %d, %d\n", talkMsgRecno, data_buf[sizeof(int)]);
#endif
		talk_res.reply_talk_msg( talkMsgRecno, data_buf[sizeof(int)], COMMAND_REMOTE);
	}

	//####### end trevor 28/8 ########//
}
// ------- End of function RemoteMsg::reply_talk_msg -------//


// ------- Begin of function RemoteMsg::nation_contact -------//
void RemoteMsg::nation_contact()
{
	err_when( id != MSG_NATION_CONTACT);
	// packet structure : <player nation> <explored nation>
	short *shortPtr = (short *)data_buf;

	err_when( *shortPtr != remote.nation_processing );
	if( !nation_array.is_deleted(*shortPtr) && !nation_array.is_deleted(shortPtr[1]) )
	{
		//####### begin trevor 30/8 #######//
#ifdef DEBUG
		long_log->printf("nation %d discover nation %d\n", shortPtr[0], shortPtr[1]);
#endif
		nation_array[shortPtr[0]]->establish_contact(shortPtr[1]);
		//####### end trevor 30/8 #######//
	}
}
// ------- End of function RemoteMsg::nation_contact -------//


// ------- Begin of function RemoteMsg::nation_set_should_attack -------//
void RemoteMsg::nation_set_should_attack()
{
	err_when( id != MSG_NATION_SET_SHOULD_ATTACK );
	// packet structure : <player nation> <target nation> <new value>
	short *shortPtr = (short *)data_buf;

	err_when( *shortPtr != remote.nation_processing );
	if( !nation_array.is_deleted(*shortPtr) && !nation_array.is_deleted(shortPtr[1]) )
	{
#ifdef DEBUG
		long_log->printf("nation %d %s instruct attack nation %d\n", shortPtr[0],
			shortPtr[2] ? "set":"clear", shortPtr[1]);
#endif
		nation_array[shortPtr[0]]->set_relation_should_attack(shortPtr[1], (char)shortPtr[2], COMMAND_REMOTE);
	}
}
// ------- End of function RemoteMsg::nation_set_should_attack -------//


// ------- begin of function RemoteMsg::nation_research -------//
//
void RemoteMsg::nation_research()
{
	err_when( id != MSG_NATION_RESEARCH );
	// packet structure : <nation recno> <tech id>
	short *shortPtr = (short *)data_buf;

	err_when( *shortPtr != remote.nation_processing );
	if( !nation_array.is_deleted(*shortPtr) )
	{
		nation_array[*shortPtr]->start_research(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------- end of function RemoteMsg::nation_research -------//


// ------- begin of function RemoteMsg::nation_repair_all_building -------//
//
void RemoteMsg::nation_repair_all_building()
{
	err_when( id != MSG_NATION_REPAIR_ALL_BUILDING );
	// packet structure : <nation recno> <repair flag>
	short *shortPtr = (short *)data_buf;

	err_when( *shortPtr != remote.nation_processing );
	if( !nation_array.is_deleted(*shortPtr) )
	{
		nation_array[*shortPtr]->repair_all_building(shortPtr[1], COMMAND_REMOTE);
	}
}
// ------- end of function RemoteMsg::nation_repair_all_building -------//


// ------- Begin of function RemoteMsg::caravan_selected -------//
void RemoteMsg::caravan_selected()
{
	err_when( id != MSG_U_CARA_SELECTED );
	// packet structure : <sprite_recno>
	short *shortPtr = (short *)data_buf;
	
	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if( unitCount <= 0)
		return;
	
	#ifdef DEBUG
		Unit *unitPtr = unit_array[*shortPtr];
		err_when( unitPtr->unit_id != UNIT_CARAVAN);
	#endif

	//------ help to check syn. for multiplayer ------//
	#ifdef DEBUG
		misc.random(100);
	#endif

	// unit_array.mp_add_selected_caravan(*shortPtr);
}
// ------- End of function RemoteMsg::caravan_selected -------//


// ------- Begin of function RemoteMsg::ship_selected -------//
void RemoteMsg::ship_selected()
{
/*
	err_when( id != MSG_U_SHIP_SELECTED );
	// packet structure : <sprite_recno>
	short *shortPtr = (short *)data_buf;

	short unitCount =1;
	validate_selected_unit_array(shortPtr, unitCount);

	if(unitCount <= 0)
		return;

	#ifdef DEBUG
		Unit *unitPtr = unit_array[*shortPtr];
		err_when(unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_SHIP || unitPtr->unit_id==UNIT_TRANSPORT);
	#endif

	//------ help to check syn. for multiplayer ------//
	#ifdef DEBUG
		misc.random(100);
	#endif

	unit_array.mp_add_selected_ship(*shortPtr);
*/
}
// ------- End of function RemoteMsg::ship_selected -------//


//------- Begin of function RemoteMsg::chat -------//
//
// Packet structure : <to nation recno> <from nation recno> <char[CHAT_STR_LEN+1]>
//
void RemoteMsg::chat()
{
	short *shortPtr = (short *)data_buf;

	int toNationRecno   = shortPtr[0];
   int fromNationRecno = shortPtr[1];

#ifdef DEBUG
	long_log->printf("nation %d send chat message to %d\n", 
		shortPtr[1],	shortPtr[0]);
#endif

	if( toNationRecno == nation_array.player_recno ||
		(toNationRecno == 0 && fromNationRecno != nation_array.player_recno) ||
		(toNationRecno == -1 && !nation_array.is_deleted(fromNationRecno) &&
			nation_array[fromNationRecno]->is_allied_with_player) )
	{
		news_array.chat_msg( fromNationRecno, (char*)(shortPtr+2) );
	}
}
//------- End of function RemoteMsg::chat -------//


//------- Begin of function RemoteMsg::compare_remote_object -------//
void	RemoteMsg::compare_remote_object()
{
	err_when( id < MSG_COMPARE_NATION || id > MSG_COMPARE_TALK );

	if( (remote.sync_test_level & 2) && (remote.sync_test_level >= 0)
		&& crc_store.compare_remote(id, data_buf) )
	{
		remote.sync_test_level = ~remote.sync_test_level;	// signal error encountered
		if( sys.debug_session )
			err.run( "Multiplayer Random Seed Sync Error." );
		else
			box.msg( "Multiplayer Random Seed Sync Error." );
	}
}
//------- End of function RemoteMsg::compare_remote_object -------//


// ------- Begin of function RemoteMsg::player_quit ---------//
void RemoteMsg::player_quit()
{
	// to let other player know a player quit voluntarily, not by error condition
	err_when( id != MSG_PLAYER_QUIT );
	// packet structure : <nation recno> <retire flag>

	short *shortPtr = (short *)data_buf;
	if( shortPtr[1])
		news_array.multi_retire(*shortPtr);
	else
		news_array.multi_quit_game(*shortPtr);
}
// ------- End of function RemoteMsg::player_quit ---------//

// ------- Begin of function RemoteMsg::camp_promote ---------//
void RemoteMsg::camp_promote()
{
	err_when( id != MSG_F_CAMP_PROMOTE );

	// packet structure : <firm recno> <soldier Id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		if( firm_array[*shortPtr]->cast_to_FirmCamp() )
		{
			firm_array[*shortPtr]->cast_to_FirmCamp()->promote_soldier( shortPtr[1], COMMAND_REMOTE );
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::camp_promote ---------//


// ------- Begin of function RemoteMsg::camp_swap_item ---------//
void RemoteMsg::camp_swap_item()
{
	err_when( id != MSG_F_CAMP_SWAP_ITEM );

		// packet structure : <firm recno>, <from soldier Id>, <to Soldier Id>, <verify item id>
	short *shortPtr = (short *)data_buf;
	if( validate_firm(*shortPtr) )
	{
		if( firm_array[*shortPtr]->cast_to_FirmCamp() )
		{
			firm_array[*shortPtr]->cast_to_FirmCamp()->swap_item( 
				shortPtr[1], shortPtr[2], shortPtr[3], COMMAND_REMOTE );
		}
		else if( firm_array[*shortPtr]->cast_to_FirmMonsterFortress() )
		{
			firm_array[*shortPtr]->cast_to_FirmMonsterFortress()->swap_item( 
				shortPtr[1], shortPtr[2], shortPtr[3], COMMAND_REMOTE );
		}
		else
		{
			err_here();
		}
	}
}
// ------- End of function RemoteMsg::camp_promote ---------//

