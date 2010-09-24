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

// Filename    : OIMMPLAY.CPP
// Description : I'Magic multiplayer SDK

#if(defined(IMAGICMP))

#include <OIMMPLAY.H>
#include <OVGALOCK.H>
#include <OT_GMENU.H>


// Define constant

// GUID is different from GUID using direct play
// 7k1 guid
//GUID GAME_GUID = 
//{ 0x12f70d44, 0x68be, 0x11d0, { 0xaa, 0xb6, 0x0, 0x0, 0xe9, 0xf9, 0xe, 0x5d } };
//char *GAME_GUID_STR = "12f70d44-68be-11d0-b6aa-0000e9f90e5d";

// 7k2 guid
GUID GAME_GUID = 
{ 0x12f71e45, 0x68be, 0x12d1, { 0xaa, 0xb6, 0x0, 0x0, 0xe9, 0xf9, 0x1f, 0x47 } };
char *GAME_GUID_STR = "12f71e45-68be-12d1-b6aa-0000e9f91f47";

#define DEFAULT_SEND_PRIORITY 3


IMServiceProvider::IMServiceProvider(const IMMSERVICEINFO &srvInfo) :
	IMMSERVICEINFO(srvInfo)
{
}

char *IMServiceProvider::name_str()
{
	switch( cServiceMode )
	{
	case IMMSRVS_IPX:
		return text_game_menu.str_service_provider_name(1);
	case IMMSRVS_TCP_LAN:
	case IMMSRVS_TCP_WAN:
		return text_game_menu.str_service_provider_name(2);
	case IMMSRVS_MODEM:
		return text_game_menu.str_service_provider_name(3);
	case IMMSRVS_SERIAL:
		return text_game_menu.str_service_provider_name(4);
	case IMMSRVS_ONLINE:
		return text_game_menu.str_service_provider_name(5);
	}
	return szServiceName;
}


char *IMServiceProvider::name_str_long()
{
	switch( cServiceMode )
	{
	case IMMSRVS_IPX:
		return text_game_menu.str_service_provider_long(1);
	case IMMSRVS_TCP_LAN:
	case IMMSRVS_TCP_WAN:
		return text_game_menu.str_service_provider_long(2);
	case IMMSRVS_MODEM:
		return text_game_menu.str_service_provider_long(3);
	case IMMSRVS_SERIAL:
		return text_game_menu.str_service_provider_long(4);
	case IMMSRVS_ONLINE:
		return text_game_menu.str_service_provider_long(5);
	}
	return NULL;
}


IMSessionDesc::IMSessionDesc(const IMMSESSIONINFO &sessInfo) :
	IMMSESSIONINFO(sessInfo)
{
}

IMPlayer::IMPlayer(const IMMPLAYERINFO &playerInfo) :
	IMMPLAYERINFO(playerInfo)
{
	connecting = 1;
}


MultiPlayerIM::MultiPlayerIM() : service_providers(sizeof(IMServiceProvider),10),
	current_sessions(sizeof(IMSessionDesc), 10), player_pool(sizeof(IMPlayer),8),
	recv_buffer(new char[MP_RECV_BUFFER_SIZE])
{
	init_flag = 0;
	imm_ptr = 0;
	recv_buffer_size = MP_RECV_BUFFER_SIZE;
	host_flag = 0;
	lobbied_flag = 0;
}

MultiPlayerIM::~MultiPlayerIM()
{
	deinit();
	delete[] recv_buffer;
}

void MultiPlayerIM::pre_init()
{
	VgaFrontLock vgaLock;
	imm_ptr = new CImMulti();
	lobbied_flag = 0;
}

void MultiPlayerIM::init(DWORD serviceProviderCode)
{
	VgaFrontLock vgaLock;
	IMMRET rc = imm_ptr->InitService(serviceProviderCode, GAME_GUID_STR);

	if( rc == IM_OK )
	{
		init_flag = 1;
	}
	else
	{
	}
	skipped_session = 0;
	host_flag = 0;
	use_receive_thread = 0;
	// ####### patch begin Gilbert 24/11 #######//
	my_player_id = IMMBROADCAST;
	// ####### end begin Gilbert 24/11 #######//
}

void MultiPlayerIM::deinit()
{
	host_flag = 0;
	lobbied_flag = 0;
	if(init_flag)
	{
		// ####### patch begin Gilbert 24/11 #######//
		if( my_player_id != IMMBROADCAST )
		{
			destroy_player( my_player_id );
			Sleep(2000);		// 2 seconds
		}

		my_player_id = IMMBROADCAST;		// mark player deleted
		// ######## patch end Gilbert 24/11 ######//

		init_flag = 0;
	}
	
	if(imm_ptr)
	{
		VgaFrontLock vgaLock;
		delete imm_ptr;
		imm_ptr = NULL;
	}
}

// ------- functions on lobby ----------//
void MultiPlayerIM::init_lobbied(int maxPlayers, char *cmdLine)
{
	VgaFrontLock vgaLock;
	IMMRET rc = imm_ptr->InitAutoLaunch(cmdLine, GAME_GUID_STR, IMMSRVS_AUTOLAUNCH);

	switch(rc)
	{
	case IM_SESSIONAUTOCREATE:
		init_flag = 1;
		lobbied_flag = 1;
		break;
	case IM_SESSIONAUTOOPEN:
		init_flag = 1;
		lobbied_flag = 2;
		break;
	case IM_SESSIONSELECTABLE:
		init_flag = 1;
		lobbied_flag = 4;
		break;
	default:
		init_flag = 0;
		lobbied_flag = 0;
	}
	skipped_session = 0;
	host_flag = 0;
}

// return 0=not lobbied, 1=auto create, 2=auto join, 4=selectable
int MultiPlayerIM::is_lobbied()
{
	return lobbied_flag;
}


// ------- functions on service provider ------ //
static BOOL __stdcall imServiceCallBack(PIMMSERVICEINFO spInfo, LPVOID mpPtr, DWORD)
{
	MultiPlayerIM *mpimPtr = (MultiPlayerIM *)mpPtr;
	IMServiceProvider sp(*spInfo);
	mpimPtr->service_providers.linkin(&sp);
	return TRUE;
}

void MultiPlayerIM::poll_service_providers()
{
	service_providers.zap();

	// ------ allocate spaces for service_provider_array -------//
	VgaFrontLock vgaLock;
	imm_ptr->EnumServices(imServiceCallBack, this, 0);
}

IMServiceProvider *MultiPlayerIM::get_service_provider(int i)
{
	if( i <= 0 || i > service_providers.size() )
		return NULL;
	return (IMServiceProvider *) service_providers.get(i);
}

static BOOL __stdcall imSessionCallBack(PIMMSESSIONINFO ssInfo, LPVOID mpPtr, DWORD)
{
	MultiPlayerIM *mpimPtr = (MultiPlayerIM *)mpPtr;
	IMSessionDesc sessionDesc(*ssInfo);
	mpimPtr->current_sessions.linkin(&sessionDesc);
	return TRUE;
}


// ------- functions on session --------//
int MultiPlayerIM::poll_sessions()
{
	err_when(!init_flag);
	current_sessions.zap();

	MouseDispCount showMouse;
	VgaCustomPalette vgaCPal(DIR_RES"PAL_WIN.RES");
	IMMRET rc = imm_ptr->EnumSessions(imSessionCallBack, this, 0);
	return rc == IM_OK || rc == IMERR_NOSESSIONS;
}

IMSessionDesc *MultiPlayerIM::get_session(int i)
{
	if( i <= 0 || i > current_sessions.size() )
		return NULL;
	return ((IMSessionDesc *) current_sessions.get(i))->before_use();
}

int MultiPlayerIM::create_session(char *sessionName, int maxPlayers)
{
	if(!init_flag || maxPlayers < 1)
		return FALSE;

	memset(&joined_session, 0, sizeof(joined_session) );
	strcpy(joined_session.szSessionName, sessionName);
	strcpy(joined_session.szSessionDesc, sessionName);	// Description of game settings
	// joined_session.dwSessionID = 0;						// session identifier
	joined_session.dwMaxPlayers = maxPlayers;				// Maximum number of players allowed in session.
	// joined_session.dwCurrentPlayers = 0;				// Current # players in Session.
	joined_session.szGSPassword[0] = '\0';					// Game Specific Password.
	// joined_session.dwGSFlags = 0;         // Game server Specific flags
	// joined_session.dwGSField1 = 0;        // Game Specific Double Word Field 1 of 3
	// joined_session.dwGSField2 = 0;        // Game Specific Double Word Field 2 of 3
	// joined_session.dwGSField3 = 0;        // Game Specific Double Word Field 3 of 3

	// not create until create player
	host_flag = 1;
	skipped_session = 1;
	return TRUE;
}

int MultiPlayerIM::join_session(IMSessionDesc* sessionDesc)
{
	if( !init_flag )
		return FALSE;

	if( is_lobbied() == 2)
	{
		// nothing
	}
	else
	{
		joined_session = *sessionDesc;
	}
	host_flag = 0;
	skipped_session = 2;
	return TRUE;
}

int MultiPlayerIM::join_session(int currentSessionIndex)
{
	if( !init_flag )
		return FALSE;
	// ##### begin Gilbert 13/2 ########//
	if( is_lobbied() == 2 )
	{
		// nothing, current_sessions is not filled, as poll_session is not called
	}
	else
	{
		if( get_session(currentSessionIndex) )
			joined_session = *get_session(currentSessionIndex);
	}
	// ##### end Gilbert 13/2 ########//
	host_flag = 0;
	skipped_session= 2;
	return TRUE;
}

void MultiPlayerIM::close_session()
{
	VgaFrontLock vgaLock;
	if( init_flag )
		imm_ptr->CloseSession();
	host_flag = 0;
}

void MultiPlayerIM::disable_join_session()
{
	if( init_flag )
	{
		imm_ptr->StopJoin();
	}
}


// -------- functions on player management -------//
int MultiPlayerIM::create_player(char *friendlyName, char *formalName,
	LPVOID, DWORD, DWORD)
{
	IMMPLAYERINFO playerInfo;
	memset(&playerInfo, 0, sizeof(playerInfo));
	// playerInfo.imID = 0;                      // Player ID
	strncpy(playerInfo.szFriendly, friendlyName, IMMSHORTNAMEMAX);
	playerInfo.szFriendly[IMMSHORTNAMEMAX-1] = '\0';
	strncpy(playerInfo.szFormal, formalName, IMMLONGNAMEMAX);
	playerInfo.szFormal[IMMLONGNAMEMAX-1] = '\0';

	IMMRET rc = IM_ERR;

	if( skipped_session )
	{
		if( skipped_session == 1 )		// create session
		{
			MouseDispCount showMouse;
			//	VgaFrontLock vgaLock;		// MouseDispCount unlock vga_front
			VgaCustomPalette vgaCPal(DIR_RES"PAL_WIN.RES");
			rc = imm_ptr->CreateAndJoin(&joined_session, &playerInfo);
		}
		else	// join session
		{
			MouseDispCount showMouse;
			//	VgaFrontLock vgaLock;		// MouseDispCount unlock vga_front
			VgaCustomPalette vgaCPal(DIR_RES"PAL_WIN.RES");
			// #### begin Gilbert 13/2 ######//
			// if session is selected from the lobby, pass 0 as the session Id
			if( is_lobbied() == 2 )
			{
				rc = imm_ptr->OpenAndJoin(0 , "", &playerInfo);
			}
			else
			{
				rc = imm_ptr->OpenAndJoin(joined_session.dwSessionID , "", &playerInfo);
			}
			// #### end Gilbert 13/2 ######//
		}
		skipped_session = 0;

		if( rc == IM_OK || rc == IM_LOCALSERVER || rc == IM_REMOTESERVER )
		{
			// fill in joined_session
			VgaFrontLock vgaLock;
			IMMSESSIONINFO ssInfo;
			imm_ptr->CurrentSession(&ssInfo);
			IMSessionDesc ssDesc(ssInfo);
			joined_session = ssDesc;
		}

		{
			VgaFrontLock vgaLock;
			use_receive_thread = 0;
			// use_receive_thread = imm_ptr->ReceiveThread() == IM_OK;
		}
	}
	else
	{
		VgaFrontLock vgaLock;
		rc = imm_ptr->Join(&playerInfo);
	}

	if( (rc == IM_OK || rc == IM_LOCALSERVER || rc == IM_REMOTESERVER) &&
		playerInfo.imID != 0)	// can easily mixed with directplay, DPID_ALLPLAYERS
	{
		my_player_id = playerInfo.imID;
		return TRUE;
	}
	else
		return FALSE;
}

void MultiPlayerIM::destroy_player( IMMPID playerId )
{
	VgaFrontLock vgaLock;
	imm_ptr->DeletePlayer(playerId);
}

static BOOL __stdcall imPlayerCallBack(PIMMPLAYERINFO playerInfo, LPVOID mpPtr, DWORD)
{
	MultiPlayerIM *mpimPtr = (MultiPlayerIM *)mpPtr;
	IMPlayer imPlayer(*playerInfo);
	imPlayer.connecting = 1;
	mpimPtr->player_pool.linkin(&imPlayer);
	return TRUE;
}


void MultiPlayerIM::poll_players()
{
	player_pool.zap();
	VgaFrontLock vgaLock;
	if(init_flag)
		imm_ptr->EnumPlayers(0, imPlayerCallBack, this, 0 );
}

IMPlayer *MultiPlayerIM::get_player(int i)
{
	if( i <= 0 || i > player_pool.size() )
		return NULL;
	return (IMPlayer *)player_pool.get(i);
}

IMPlayer *MultiPlayerIM::search_player(IMMPID playerId)
{
	IMPlayer *player;
	int i = 0;
	while( (player = get_player(++i)) != NULL )
		if( player->imID == playerId )
			return player;
	return NULL;
}

IMPlayer *MultiPlayerIM::search_player(char *name)
{
	IMPlayer *player;
	int i = 0;
	while( (player = get_player(++i)) != NULL )
		if( strnicmp(player->szFormal, name, IMMLONGNAMEMAX)== 0)
			return player;
	return NULL;
}

int MultiPlayerIM::is_host(IMMPID playerId)
{
	err_here();		// not supported
	return 0;
}

int MultiPlayerIM::am_I_host()
{
	return host_flag;
}

int MultiPlayerIM::is_player_connecting(IMMPID playerId)
{
	for( int p = 1; p <= player_pool.size(); ++p)
	{
		IMPlayer *imPlayer = (IMPlayer *) player_pool.get(p);
		if( imPlayer->imID == playerId )
		{
			return imPlayer->connecting;
		}
	}
	return 0;
}

// ------- functions on message passing ------//
// pass IMMBROADCAST as toID to all players 
int MultiPlayerIM::send(IMMPID toId, LPVOID lpData, DWORD dataSize)
{
	err_when(!init_flag);
	IMMRET rc;
	{
		VgaFrontLock vgaLock;
		rc = imm_ptr->Send(my_player_id, toId, lpData, dataSize, 
			DEFAULT_SEND_PRIORITY, FALSE);
	}

	// see if any player lost
	if( rc == IMERR_INVALIDPLAYER )
	{
		for( int p = 1; p <= player_pool.size(); ++p)
		{
			IMPlayer *imPlayer = (IMPlayer *)player_pool.get(p);
			if( imPlayer->imID == toId )
			{
				imPlayer->connecting = 0;
			}
		}
	}
	return rc == IM_OK;
}

void MultiPlayerIM::begin_stream(IMMPID toID)
{
	// not supported
}

int MultiPlayerIM::send_stream(IMMPID toId, LPVOID lpData, DWORD dataSize)
{
	err_when(!init_flag);
	IMMRET rc;
	VgaFrontLock vgaLock;

	// BUGHERE : cannot handle to group message
	if( toId == IMMBROADCAST )
	{
		// note that player_pool may not update
		// but don't call poll_player here
		rc = IM_OK;
		for( int p = 1; p <= player_pool.size(); ++p)
		{
			IMPlayer *imPlayer = (IMPlayer *)player_pool.get(p);
			if( imPlayer->imID != my_player_id && imPlayer->connecting )
			{
				IMMRET rc2 = imm_ptr->Send(my_player_id, imPlayer->imID,
					lpData, dataSize, DEFAULT_SEND_PRIORITY, TRUE);
				if( rc2 == IMERR_INVALIDPLAYER )
					imPlayer->connecting = 0;		// mark this player disconnect
				else if( rc2 != IM_OK )
					rc = rc2;		// collect the latest error code
			}
		}
	}
	else
	{
		rc = imm_ptr->Send(my_player_id, toId, lpData, dataSize, 
			DEFAULT_SEND_PRIORITY, TRUE);

		// see if any player lost
		if( rc == IMERR_INVALIDPLAYER )
		{
			IMPlayer *imPlayer = search_player(toId);
			if( imPlayer )
			{
				imPlayer->connecting = 0;		// mark this player disconnect
			}
		}
	}

	return rc == IM_OK;
}

void MultiPlayerIM::end_stream(IMMPID toID)
{
	// not supported
}

int MultiPlayerIM::get_msg_count()
{
	return imm_ptr->CheckMessages();
}

char *MultiPlayerIM::receive(IMMPID *from, IMMPID *to, LPDWORD dSize, int *sysMsgCount)
{
	err_when(!init_flag);
	IMMPID fromId, toId;
	DWORD timeStamp;
	DWORD dataSize;
	int retryFlag;
	IMMRET rc;

	VgaFrontLock vgaLock;
	if( sysMsgCount )
		*sysMsgCount = 0;
	do
	{
		retryFlag = 0;
		dataSize = recv_buffer_size;
		//rc=imm_ptr->GetToPlayer(my_player_id, recv_buffer, &dataSize,
		//	&fromId, &toId, &timeStamp);
		rc = imm_ptr->GetTop(recv_buffer, &dataSize, &fromId, &toId, &timeStamp);
		switch(rc)
		{
		case IM_OK:
			if( toId == IMMSERVERID)
			{
				// discard message sent to server
				retryFlag = 1;
			}
			else
			{
				*from = fromId;
				*to = toId;
				*dSize = dataSize;
			}
			break;
		case IM_SYSMSG:
		case IM_CHATMSG:
			handle_system_msg(recv_buffer, dataSize);
			if( sysMsgCount )
				(*sysMsgCount)++;
			retryFlag = 1;
			break;
		case IMERR_BUFFERTOOSMALL:		// assume now dataSize > recv_buffer_size
			delete[] recv_buffer;
			recv_buffer_size = dataSize + 0x400;
			recv_buffer = new char[recv_buffer_size];
			retryFlag = 1;
			break;
		default:
			return NULL;
		}
		
	} while (retryFlag);
	return recv_buffer;
}

void MultiPlayerIM::handle_system_msg(LPVOID lpData, DWORD dSize )
{
	IMMSRVMSG *srvMsg = (IMMSRVMSG *)lpData;
	switch(srvMsg->dwMsgType)
	{
	case IMMSRV_COMMLOST:
	case IMMSRV_PLAYERDESTROYED:
		{
			for( int p = 1; p <= player_pool.size(); ++p)
			{
				IMPlayer *imPlayer = (IMPlayer *)player_pool.get(p);
				if( imPlayer->imID == srvMsg->idPlayer )
				{
					imPlayer->connecting = 0;
				}
			}
		}
		break;
	case IMMSRV_ADDEDPLAYER:
		break;
	case IMMSRV_PLAYERTOGROUP:
		break;
	case IMMSRV_GROUPDESTROYED:
		break;
	case IMMSRV_PLAYERFROMGROUP:
		break;
	case IMMSRV_CONNECTED:
		break;
	case IMMSRV_ADDEDGROUP:
      break;
	case IMMSRV_Reserved_3:
      break;
	case IMMSRV_Reserved_2:
      break;
	case IMMSRV_UNSUPPORTED:
		break;
	case IMMSRV_Reserved_1:
      break;
	case IMMSRV_EXTERNCHAT:			//An external Chat Message Packet
		break;
	case IMMSRV_GAMECHAT:			//An Internal Chat Message Packet
		break;
	}
}


// ------- Begin of function MultiPlayerIM::before_receive ------//
void MultiPlayerIM::before_receive()
{
	if(!use_receive_thread)
	{
		VgaFrontLock vgaLock;
		// imm_ptr->ReceiveAll(1);
		imm_ptr->ReceiveAll(0);
	}
}
// ------- End of function MultiPlayerIM::before_receive ------//


// ------- Begin of function MultiPlayerIM::after_send ------//
void MultiPlayerIM::after_send()
{
	if(!use_receive_thread)
	{
		VgaFrontLock vgaLock;
		imm_ptr->Flush();
	}
}
// ------- End of function MultiPlayerIM::after_send ------//


// ------ Begin of function MultiPlayerIM::sort_sessions -------//

static int sort_session_id(const void *a, const void *b)
{
	return ((IMSessionDesc *)a)->session_id() - ((IMSessionDesc *)b)->session_id();
}


static int sort_session_name(const void *a, const void *b)
{
	return strcmp( ((IMSessionDesc *)a)->name_str(), ((IMSessionDesc *)b)->name_str() );
}

// sort current_sessions
// <int> sortType, 1=sort by GUID, 2=sort by session name
void MultiPlayerIM::sort_sessions(int sortType )
{
	// BUGHERE : quick_sort is a DynArray function but current_sessions is DynArrayB
	switch(sortType)
	{
	case 1:
		current_sessions.quick_sort(sort_session_id);
		break;
	case 2:
		current_sessions.quick_sort(sort_session_name);
		break;
	default:
		err_here();
	}

	for( int s = 1; get_session(s); ++s)
	{
		get_session(s)->before_use();
	}
}
// ------ End of function MultiPlayerIM::sort_sessions -------//

#endif