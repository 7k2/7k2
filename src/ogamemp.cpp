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

//Filename    : OGAMEMP.CPP
//Description : Main Game Object - Multiplayer Game (using Imagic multiplayer SDK)

#include <osys.h>
#include <omouse.h>
#include <omousecr.h>
#include <obox.h>
#include <ovga.h>
#include <ovgalock.h>
#include <ofont.h> 
#include <obutton.h>
#include <obutt3d.h>
#include <obuttcus.h>
#include <onation.h>
#include <oraceres.h>
#include <oremote.h>
#include <obattle.h>
#include <ogame.h>
#include <odplay.h>
#include <oimmplay.h>
#include <oerrctrl.h>
#include <ogfile.h>
#include <oconfig.h>
#include <oserial.h>
#include <oimgres.h>
#include <oget.h>
#include <key.h>
#include <omusic.h>
#include <ocoltbl.h>
#include <ogeta.h>
#include <oslidcus.h>
#include <oblob2w.h>
#include <ogammenu.h>
#include <obitmap.h>
#include <oprofile.h>
#include <ot_gmenu.h>
#include <ot_reps.h>

#ifdef DEMO
#define ONLY_TERRAIN_SET_1
#define ONLY_SMALL_BUILDING
#endif


//---------- Define option modes --------//

enum
{ 
	OPTION_SPECIES,
	OPTION_BASIC,
	OPTION_ADVANCED,
	OPTION_ADVANCE2,
	OPTION_GOAL,
	OPTION_CHAT_LOG,
};

// --------- Define constant --------//

#define PLAYER_RATIO_CDROM 3
#define PLAYER_RATIO_NOCD -1
// #define PLAYER_RATIO_STRING "four"
// #define FORCE_MAX_FRAME_DELAY 5

// undef MAX_GEM_STONES to disable gemstone selection
#define MAX_GEM_STONES 10
#define APPLY_RANKING

static char sub_game_mode;		// 0 = new multiplayer game, 1 = load multiplayer game
#define GAME_VERSION_CRYPTED_COUNT 1
extern int GAME_VERSION_CRYPTED[GAME_VERSION_CRYPTED_COUNT];

// -------- declare static function --------//

static void disp_service_button(ButtonCustom *, int);
static void i_disp_text_button(ButtonCustom *, int repaintBody);
static void i_disp_race_button(ButtonCustom *, int repaintBody);
static void i_disp_color_button(ButtonCustom *, int repaintBody);
static void i_disp_int_button(ButtonCustom *, int repaintBody);
static void i_disp_tick_button(ButtonCustom *, int repaintBody);
static int process_load_game_chat_command( char *, DynArray * );

static int debug_version_flag()
{
	// bit 0 = DEBUG, bit 1 = BETA, bit 2 = DEMO
	return
#ifdef DEBUG
		1
#else
		0
#endif
#ifdef BETA
		| 2
#endif
#ifdef DEMO
		| 4
#endif
	;
}


// put each compatible version here
// static int compatbile_version[] = {};
static int is_compatible_version(int remoteVersion, int debugVersion)
{
	if( debug_version_flag() != debugVersion )
		return 0;

	if( remoteVersion == GAME_VERSION )
		return 1;

	if( remoteVersion > GAME_VERSION )
		return 0;

//	for( int i = 0; i < sizeof(compatbile_version)/sizeof(compatbile_version[0]); ++i )
//	{
//		if( compatible_version[i] == i )
//			return 1;
//	}
	return 0;
}



enum
{
	MPMSG_START_GAME = 0x1f5a0001,
	MPMSG_ABORT_GAME,
	MPMSG_RANDOM_SEED,			// see MpStructSeed
	MPMSG_RANDOM_SEED_STR,
	MPMSG_DECLARE_NATION,		// see MpStructNation
	MPMSG_END_SETTING,
	MPMSG_START_LOAD_GAME,
	MPMSG_ACCEPT_DECLARE_NATION,		// used in loading multiplayer game
	MPMSG_REFUSE_DECLARE_NATION,
	MPMSG_SEND_CONFIG,

	MPMSG_NEW_PLAYER,
	MPMSG_ACCEPT_NEW_PLAYER,
	MPMSG_REFUSE_NEW_PLAYER,
	MPMSG_ACQUIRE_COLOR,
	MPMSG_ACCEPT_COLOR,
	MPMSG_REFUSE_COLOR,
	MPMSG_ACQUIRE_RACE,
	MPMSG_ACCEPT_RACE,
	MPMSG_REFUSE_RACE,
	MPMSG_PLAYER_READY,
	MPMSG_PLAYER_UNREADY,
	MPMSG_SET_GEM_STONE,
	MPMSG_LOAD_GAME_NEW_PLAYER,
	MPMSG_SEND_CHAT_MSG,

	// ### patch begin Gilbert 22/1 ######//
	// introduced since version 111
	MPMSG_SEND_SYNC_TEST_LEVEL,
	// ### patch begin Gilbert 22/1 ######//
	MPMSG_TEST_LATENCY_SEND,
	MPMSG_TEST_LATENCY_ECHO,
	MPMSG_SET_PROCESS_FRAME_DELAY,
};

struct MpStructBase
{
	DWORD msg_id;
	MpStructBase(DWORD msgId) : msg_id(msgId) {}
};

struct MpStructSeed : public MpStructBase
{
	long	seed;
	MpStructSeed(long s) : MpStructBase(MPMSG_RANDOM_SEED), seed(s) {}
};

struct MpStructSeedStr : public MpStructBase
{
	enum { RANDOM_SEED_MAX_LEN = 11 };
	char	seed_str[RANDOM_SEED_MAX_LEN+1];
	MpStructSeedStr(char *s) : MpStructBase(MPMSG_RANDOM_SEED_STR)
	{
		if(s)
			strcpy(seed_str, s);
		else
			seed_str[0] = '\0';
	}

	MpStructSeedStr(long l) : MpStructBase(MPMSG_RANDOM_SEED_STR)
	{
		ltoa(l, seed_str, 10);
	}
};

struct MpStructNation : public MpStructBase
{
	short nation_recno;
	DWORD dp_player_id;
	short color_scheme;
	short race_id;
	char  player_name[NationArray::CUSTOM_NAME_LEN+1];
	short use_gem_stones;
	WORD	ranking;

	MpStructNation() : MpStructBase(MPMSG_DECLARE_NATION) {}
	MpStructNation(short n, DWORD playerId, short scheme, short race,
		char *playerName, short useGemStones, WORD rank):
		MpStructBase(MPMSG_DECLARE_NATION), nation_recno(n),
		dp_player_id(playerId), color_scheme(scheme), race_id(race),
		use_gem_stones(useGemStones), ranking(rank)
		{
			strcpy(player_name, playerName);
		}
	void init(short n, DWORD playerId, short scheme, short race,
		char *playerName, short useGemStones, WORD rank)
	{
		msg_id = MPMSG_DECLARE_NATION;
		nation_recno = n;
		dp_player_id = playerId;
		color_scheme = scheme;
		race_id = race;
		strcpy(player_name, playerName);
		use_gem_stones = useGemStones;
		ranking = rank;
	}
};

struct MpStructConfig : public MpStructBase
{
	Config game_config;

	MpStructConfig(Config &c) : MpStructBase(MPMSG_SEND_CONFIG), game_config(c) {}
};


struct MpStructNewPlayer : public MpStructBase
{
	PID_TYPE player_id;
	int game_version;
	int debug_version;
	short player_balance;	// 2 for CD-ROM version, -1 for non CD-ROM version
	WORD ranking;
	MpStructNewPlayer(PID_TYPE p, short bal) : MpStructBase(MPMSG_NEW_PLAYER), player_id(p),
		player_balance(bal), game_version(GAME_VERSION),
		debug_version( debug_version_flag() ),
		ranking( player_profile.ranking )
	{
		// empty
	}

	int	is_compatible() { return is_compatible_version(game_version, debug_version); }
};

struct MpStructAcceptNewPlayer : public MpStructBase
{
	PID_TYPE player_id;
	MpStructAcceptNewPlayer(PID_TYPE p) : MpStructBase(MPMSG_ACCEPT_NEW_PLAYER),
		player_id(p) {}
};


struct MpStructRefuseNewPlayer : public MpStructBase
{
	// reason_id
	enum
	{
		REFUSE_REASON_OTHER = 0,
		REFUSE_HOST_NEW_GAME,
		REFUSE_HOST_LOAD_GAME,
		REFUSE_LOAD_GAME_UNSYNC,
		REFUSE_VERSION,
		REFUSE_SEAT_FULL,
	};

	PID_TYPE player_id;
	int		reason_id;
	MpStructRefuseNewPlayer(PID_TYPE p, int reasonId) : MpStructBase(MPMSG_REFUSE_NEW_PLAYER), player_id(p), reason_id(reasonId) {}

	char *reason_str()
	{
		return text_game_menu.str_mp_refuse_reason(reason_id);
	}
};

struct MpStructAcquireColor : public MpStructBase
{
	short	color_scheme_id;
	MpStructAcquireColor(short c) : MpStructBase(MPMSG_ACQUIRE_COLOR), color_scheme_id(c) {}
};

struct MpStructAcceptColor : public MpStructBase
{
	PID_TYPE  request_player_id;
	short	color_scheme_id;
	MpStructAcceptColor(PID_TYPE p,short c) : MpStructBase(MPMSG_ACCEPT_COLOR),
		request_player_id(p), color_scheme_id(c) {}
};

struct MpStructRefuseColor : public MpStructBase
{
	PID_TYPE  request_player_id;
	short	color_scheme_id;
	MpStructRefuseColor(PID_TYPE p, short c) : MpStructBase(MPMSG_REFUSE_COLOR), 
		request_player_id(p), color_scheme_id(c) {}
};

struct MpStructAcquireRace : public MpStructBase
{
	short	race_id;
	MpStructAcquireRace(short raceId) : MpStructBase(MPMSG_ACQUIRE_RACE), race_id(raceId) {}
};

struct MpStructAcceptRace : public MpStructBase
{
	PID_TYPE  request_player_id;
	short	race_id;
	MpStructAcceptRace(PID_TYPE p,short raceId) : MpStructBase(MPMSG_ACCEPT_RACE),
		request_player_id(p), race_id(raceId) {}
};

struct MpStructRefuseRace : public MpStructBase
{
	PID_TYPE  request_player_id;
	short	race_id;
	MpStructRefuseRace(PID_TYPE p, short raceId) : MpStructBase(MPMSG_REFUSE_RACE), 
		request_player_id(p), race_id(raceId) {}
};

struct MpStructPlayerReady : public MpStructBase
{
	PID_TYPE player_id;
	MpStructPlayerReady(PID_TYPE p) : MpStructBase(MPMSG_PLAYER_READY), player_id(p) {}
};

struct MpStructPlayerUnready : public MpStructBase
{
	PID_TYPE player_id;
	MpStructPlayerUnready(PID_TYPE p) : MpStructBase(MPMSG_PLAYER_UNREADY), player_id(p) {}
};


struct MpStructSetGemStones : public MpStructBase
{
	PID_TYPE player_id;
	long	total_gem_stones;
	short use_gem_stones;
	MpStructSetGemStones(PID_TYPE p, short useGemStones, long totalGemStones) : MpStructBase(MPMSG_SET_GEM_STONE),
		player_id(p),
		use_gem_stones(useGemStones),
		total_gem_stones(totalGemStones) {}
};


struct MpStructLoadGameNewPlayer : public MpStructBase
{
	PID_TYPE player_id;
	int	game_version;
	int	debug_version;
	short nation_recno;
	short color_scheme_id;
	short race_id;
	DWORD frame_count;			// detail to test save game from the same game
	long	random_seed;
	short player_balance;

	MpStructLoadGameNewPlayer(PID_TYPE p, Nation *n, DWORD frame, long seed, short bal) : 
		MpStructBase(MPMSG_LOAD_GAME_NEW_PLAYER), player_id(p), game_version(GAME_VERSION),
		debug_version( debug_version_flag() ),
		nation_recno(n->nation_recno), color_scheme_id(n->color_scheme_id),
		race_id(n->race_id), frame_count(frame), random_seed(seed), player_balance(bal) 
		{
		}

	int	is_compatible() { return is_compatible_version(game_version, debug_version); }
};

struct MpStructChatMsg : public MpStructBase
{
	enum { MSG_LENGTH = 60 };
	char sender[MP_FRIENDLY_NAME_LEN+1];
	char content[MSG_LENGTH+1];
	MpStructChatMsg(char *fromName, char *message) : MpStructBase(MPMSG_SEND_CHAT_MSG)
	{
		if( fromName )
			strcpy(sender, fromName);
		else
			sender[0] = '\0';
		if( message )
			strcpy(content, message);
		else
			content[0] = '\0';
	}
};

struct MpStructSyncLevel : public MpStructBase
{
	char sync_test_level;
	MpStructSyncLevel(char syncLevel) : MpStructBase(MPMSG_SEND_SYNC_TEST_LEVEL), sync_test_level(syncLevel)
	{
	}
};


struct MpStructLatencySend : public MpStructBase
{
	int test_id;
	DWORD send_time;
	MpStructLatencySend(int testId, DWORD sendTime) : MpStructBase(MPMSG_TEST_LATENCY_SEND),
		test_id(testId), send_time(sendTime)
	{
	}
};

struct MpStructLatencyReturn : public MpStructLatencySend
{
	MpStructLatencyReturn(const MpStructLatencySend &ls) : MpStructLatencySend(ls)
	{
		msg_id = MPMSG_TEST_LATENCY_ECHO;
	}
};


struct MpStructProcessFrameDelay : public MpStructBase
{
	int	common_process_frame_delay;
	MpStructProcessFrameDelay(int newFrameDelay) : MpStructBase(MPMSG_SET_PROCESS_FRAME_DELAY),
		common_process_frame_delay(newFrameDelay)
	{
	}
};




// --------- Begin of static function multi_player_game ----------//
// avoid creating local variable in this function
void Game::multi_player_game(char *cmdLine)
{
	sub_game_mode = 0;
	info.init_random_seed(0);			// initialize the random seed

	int choice, p;

	// if already initialized, don't init again
	if( !mp_obj.init_flag )
	{
		mp_obj.pre_init();

		if( !cmdLine || (mp_obj.init_lobbied(MAX_NATION, cmdLine), !mp_obj.init_flag) )
		{	// not launched from lobby

			mp_obj.poll_service_providers();
			choice = mp_select_service();
			if( !choice )
			{
				mp_obj.deinit();
				return;
			}

			mp_obj.init(mp_obj.get_service_provider(choice)->service_id());
		}
	}

	// do not call remote.init here, or sys.yield will call remote.poll_msg
	if(!mp_obj.init_flag)
	{
		box.msg( text_game_menu.str_mp_error_dplay() ); //"Cannot initialize DirectPlay.");
		mp_obj.deinit();
		return;
	}

	// create game or join game
	switch( mp_select_mode(NULL) )
	{
	case 1:		// create game
		if( (choice = mp_obj.create_session(config.player_name, MAX_NATION)) <= 0 )
		{
			if( choice < 0 )
				box.msg( text_game_menu.str_mp_error_session() );// "Cannot create session" );
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.create_player(config.player_name, config.player_name)) <= 0 )
		{
			if( choice < 0 )
				box.msg( text_game_menu.str_mp_error_session() ); //"Cannot create player" );
			mp_obj.deinit();
			return;
		}
		else
		{
			remote.init(&mp_obj);
			remote.create_game();
		}
		break;
	case 2:		// join game
		if( !(choice = mp_select_session()) )
		{
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.join_session(choice)) <= 0 )
		{
			if(choice < 0)
				box.msg( text_game_menu.str_mp_error_join() ); //"Cannot join session" );
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.create_player(config.player_name, config.player_name)) <= 0 )
		{
			if( choice < 0)
				box.msg( text_game_menu.str_mp_error_player() ); //"Cannot create player" );
			mp_obj.deinit();
			return;
		}
		else
		{
			remote.init(&mp_obj);
			remote.connect_game();
		}
		break;
	default:			// cancel
		mp_obj.deinit();
		return;
	}

	// config game session ...
	NewNationPara *nationPara = (NewNationPara *)mem_add(sizeof(NewNationPara)*MAX_NATION);
	int mpPlayerCount = 0;
	if( (choice = mp_select_option(nationPara, &mpPlayerCount)) <= 0 )
	{
		if( choice < 0 )
		{
			// BUGHERE : error message here
		}
		mem_del(nationPara);
		if( remote.is_host )
			mp_obj.close_session();
		remote.deinit();
		mp_obj.deinit();
		return;
	}

	// assign nation is done is mp_select_option

	// ---------- initialize ec_remote --------- //
	// find itself
	for( p = 0; p < mpPlayerCount; ++p )
	{
		if( nationPara[p].dp_player_id == mp_obj.my_player_id )
		{
			ec_remote.init( &mp_obj, char(p+1) );
			break;
		}
	}

	if( p >= mpPlayerCount )
	{
		err_here();
		mem_del(nationPara);
		remote.deinit();
		mp_obj.deinit();
		return;
	}

	for( p = 0; p < mpPlayerCount; ++p )
	{
		if( nationPara[p].dp_player_id != mp_obj.my_player_id )
		{
			ec_remote.set_dp_id(char(p+1), nationPara[p].dp_player_id );
		}
	}

	//---------- start game now ----------//

//	vga_front.bar(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_LIGHT_BLUE);
//	sys.blt_virtual_buf();

	remote.init_start_mp();

	// suppose inital sys.frame_count is 1
	// find nation_array.player_recno from nationPara
	for( p = 0; p < mpPlayerCount; ++p )
	{
		if( nationPara[p].dp_player_id == mp_obj.my_player_id )
		{
			// remote.init_send_queue(1, nation_array.player_recno);	// but nation_array.player_recno is not set
			remote.init_send_queue(1, nationPara[p].nation_recno);   // initialize the send queue for later sending
			break;
		}
	}
	err_when( p >= mpPlayerCount );

	remote.init_receive_queue(1);

	init();
	remote.handle_vga_lock = 0;	// disable lock handling

	// ####### begin Gilbert 2/7 #########//
	if( mp_obj.is_lobbied() )
	{
		player_profile.reload();
		++player_profile.num_start_games;
#ifdef DEMO
		if( player_profile.demo_try_count < 255 )
			++player_profile.demo_try_count; 
#endif
		// update profile
		player_profile.save();

		// generate a serial number, to guard against bonus of winning again using load game

		generated_serial = serial_repository.get_serial_number();
		serial_repository.add( generated_serial );
	}
	// ####### end Gilbert 2/7 #########//

	battle.run(nationPara, mpPlayerCount);

	mem_del(nationPara);
	remote.deinit();
	mp_obj.deinit();
	deinit();
}
// --------- End of static function multi_player_game ----------//


// --------- Begin of static function load_mp_game ----------//
// avoid creating local variable in this function
void Game::load_mp_game(char *fileName, char *cmdLine)
{
	sub_game_mode = 1;

	int nationRecno;
	int choice, p;

	// if already initialized, don't init again
	if( !mp_obj.init_flag )
	{
		mp_obj.pre_init();

		if( !cmdLine || (mp_obj.init_lobbied(MAX_NATION, cmdLine), !mp_obj.init_flag) )
		{	// not launched from lobby

			mp_obj.poll_service_providers();
			choice = mp_select_service();
			if( !choice )
			{
				mp_obj.deinit();
				return;
			}

			mp_obj.init(mp_obj.get_service_provider(choice)->service_id());
		}
	}

	// do not call remote.init here, or sys.yield will call remote.poll_msg
	if(!mp_obj.init_flag)
	{
		box.msg( text_game_menu.str_mp_error_dplay() ); //"Cannot initialize DirectPlay.");
		mp_obj.deinit();
		return;
	}

	// count required player
	int gamePlayerCount = 0;
	for(nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno)
	{
		if( !nation_array.is_deleted(nationRecno) && 
			(nation_array[nationRecno]->is_own() || nation_array[nationRecno]->is_remote()) )
		{
			++gamePlayerCount;
		}
	}

	// create game or join game
	switch( mp_select_mode(fileName) )
	{
	case 1:		// create game
		if( (choice = mp_obj.create_session(config.player_name, gamePlayerCount)) <= 0 )
		{
			if( choice < 0 )
				box.msg( text_game_menu.str_mp_error_session() ); //"Cannot create session" );
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.create_player(config.player_name, config.player_name)) <= 0 )
		{
			if( choice < 0 )
				box.msg( text_game_menu.str_mp_error_player() ); //"Cannot create player" );
			mp_obj.deinit();
			return;
		}
		else
		{
			remote.init(&mp_obj);
			remote.create_game();
		}
		break;
	case 2:		// join game
		if( !(choice = mp_select_session()) )
		{
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.join_session(choice)) <= 0 )
		{
			if(choice < 0)
				box.msg( text_game_menu.str_mp_error_join() ); //"Cannot join session" );
			mp_obj.deinit();
			return;
		}
		else if( (choice = mp_obj.create_player(config.player_name, config.player_name)) <= 0 )
		{
			if( choice < 0)
				box.msg( text_game_menu.str_mp_error_player() ); //"Cannot create player" );
			mp_obj.deinit();
			return;
		}
		else
		{
			// count required player
			gamePlayerCount = 0;
			for(nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno)
				if( !nation_array.is_deleted(nationRecno) && !nation_array[nationRecno]->is_ai() )
					++gamePlayerCount;

			remote.init(&mp_obj);
			remote.connect_game();
		}
		break;
	default:			// cancel
		mp_obj.deinit();
		return;
	}

	// config game session ...
	if( (choice = mp_select_load_option(fileName)) <= 0 )
	{
		if( choice < 0 )
		{
			// BUGHERE : error message here
		}
		if( remote.is_host )
			mp_obj.close_session();
		remote.deinit();
		mp_obj.deinit();
		return;
	}

	// assign nation is done is mp_select_load_option

	// ---------- initialize ec_remote --------- //
	// find itself
	ec_remote.init( &mp_obj, (char) (~nation_array)->nation_recno );
	for( p = 1; p <= nation_array.size(); ++p )
	{
		if( !nation_array.is_deleted(p) )
		{
			Nation *nationPtr = nation_array[p];
			if( nationPtr->is_remote() )
			{
				ec_remote.set_dp_id((char) p, nationPtr->player_id );
			}
		}
	}

	//---------- start game now ----------//

//	vga_front.bar(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_LIGHT_BLUE);
//	sys.blt_virtual_buf();

	remote.init_start_mp();
	remote.init_send_queue(sys.frame_count, nation_array.player_recno);   // initialize the send queue for later sending
	remote.init_receive_queue(sys.frame_count);

//	init();
	remote.handle_vga_lock = 0;	// disable lock handling

	if( mp_obj.is_lobbied() )
	{
		player_profile.reload();
		// ++player_profile.num_start_games;	// new game only
#ifdef DEMO
		if( player_profile.demo_try_count < 255 )
			++player_profile.demo_try_count; 
#endif
		// update profile
		player_profile.save();
	}

	battle.run_loaded();		// 1-multiplayer game

	remote.deinit();
	mp_obj.deinit();
	deinit();
}
// --------- End of static function load_mp_game ----------//


//-------- Begin of function Game::mp_select_service --------//
//
// Select multiplayer mode. Create a new game or connect
// to an existing game ?
//
// return : <int> service selected, starting from 1
// 
int Game::mp_select_service()
{
#define SVOPTION_ALL_SLOTS   0x00000001
#define SVOPTION_PAGE        0x40000000
#define SVOPTION_ALL         0x7fffffff

	const int MAX_SERVICE_BUTTON = 6;

	int refreshFlag = SVOPTION_ALL;

	// -------- display button ---------//

	int sy1 = SCROLL_SHEET_Y1+35;
	int sy2 = SCROLL_SHEET_Y2-10;
	int ySpacing = (sy2 - sy1 + 1) / MAX_SERVICE_BUTTON;
	int sh = ySpacing - 4;

	// ------ count button -----//

	for( int b = 0; b < MAX_SERVICE_BUTTON && mp_obj.get_service_provider(b+1); ++b );
	int buttonCount = b;
	if( buttonCount <= 0 )		// no service
		return -1;

	ButtonCustomGroup serviceButtonGroup(buttonCount);
	for( b = 0; b < buttonCount; ++b )
	{
		err_when( !mp_obj.get_service_provider(b+1) );
		serviceButtonGroup[b].create( SCROLL_SHEET_X1+20, sy1 + b * ySpacing,
			SCROLL_SHEET_X2-20, sy1 + b * ySpacing + sh - 1,
			disp_service_button, ButtonCustomPara(NULL,b+1), 0 );
	}
	serviceButtonGroup.push(-1, 0);		// unselect all button, don't paint

	int choice = 0;
	{
		VgaFrontLock vgaLock;		// unlock

		//-------- detect buttons ---------//

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return 0;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SVOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;		// lock

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			if( refreshFlag )
			{
				if( refreshFlag & SVOPTION_PAGE )
				{
					//--------- display interface screen -------//

					vga.use_back();
					vga.disp_image_file("CHOOSE");

					// -------- display title -------//

					font_bold_black.center_put( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCROLL_SHEET_Y1+20, text_game_menu.str_mp_select_service()); //"Select Connection Method") ;

					// -------- display service button --------//

					font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_continue() ); //"Continue" );
					font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_cancel() ); // "Cancel" );

					vga.use_front();
					vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				}

				// --------- refresh service button -------//

				if( refreshFlag & SVOPTION_ALL_SLOTS )
				{
					serviceButtonGroup.paint();
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();		// blt the virtual front buffer to the screen

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// ------- detect protocol button ---------//

			if( serviceButtonGroup.detect() >= 0 )
			{
				choice = serviceButtonGroup() + 1;
				// refreshFlag |= SVOPTION_ALL_SLOTS;
			}

			// ------- double click on protocol button -------//

			if( serviceButtonGroup() >= 0 )
			{
				ButtonCustom *button = &serviceButtonGroup[serviceButtonGroup()];
				if( mouse.double_click( button->x1, button->y1, button->x2, button->y2 ) )
				{
					break;		// break while(1) loop like press continue
				}
			}

			// ------- detect continue button ------//

			if( mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2))
			{
				break;
			}

			// ------- detect return button ------//

			if( mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2))
			{
				choice = 0;
				break;
			}
		} // end while(1)
	}	// end scope of vgalock

	return choice;
}
//--------- End of function Game::mp_select_service ---------//


//-------- Begin of function Game::mp_select_mode --------//
// return 0 = cancel, 1 = create, 2 = join
int Game::mp_select_mode(char *defSaveFileName)
{
#define SMOPTION_GETA(n)   (1 << (n))
#define SMOPTION_GETA_ALL  0x000000ff
#define SMOPTION_PAGE      0x40000000
#define SMOPTION_ALL       0x7fffffff

	int refreshFlag = SMOPTION_ALL;

//	Button3D createButton, joinButton, returnButton;

	if( mp_obj.is_lobbied() && mp_obj.get_lobbied_name() )
	{
		// use name from lobby
		strncpy(config.player_name, mp_obj.get_lobbied_name(), Config::PLAYER_NAME_LEN );
		config.player_name[Config::PLAYER_NAME_LEN] = '\0';
	}
	else if( sub_game_mode == 0 && player_profile.is_registered() )
	{
		// use profile name ( new multiplayer game only )
		strcpy( config.player_name, player_profile.player_name );
	}

	//Get get_name;
	//get_name.field( 374, 470, config.player_name, Config::PLAYER_NAME_LEN, 574 );
	char saveFileName[8+1];		// save game name without path or extension
	if( defSaveFileName )
	{
		char extractedFileName[MAX_PATH+1];
		m.extract_file_name(extractedFileName, defSaveFileName);

		int newLen = m.str_str(extractedFileName, "." );
		if( newLen > 8)
			m.str_cut(saveFileName, extractedFileName, 1, 8);	// saveFileName has 8 char only plus string terminator
		else if( newLen > 1)
			m.str_cut(saveFileName, extractedFileName, 1, newLen-1);
		else
			err_here();
		if( m.str_icmpx(saveFileName, "AUTO") || m.str_icmpx(saveFileName, "AUTO2") )
		{
			strcpy(saveFileName, "MULTI");
		}
	}
	else
	{
		strcpy(saveFileName, "MULTI");
	}

	GetAGroup keyInField(2);
	GetA &getName = keyInField[0];
	GetA &getSaveFile = keyInField[1];
	getName.init( SCROLL_SHEET_X1+30, SCROLL_SHEET_Y1+120, SCROLL_SHEET_X2-30, config.player_name, Config::PLAYER_NAME_LEN, &font_san, 0 ,1);
	getSaveFile.init( SCROLL_SHEET_X1+30, SCROLL_SHEET_Y1+220, SCROLL_SHEET_X2-30, saveFileName, 8, &font_san, 0, 1);
	keyInField.set_focus(0, 0);		// 0 in 2nd parameter - don't display

	int rc = 0;

	VgaFrontLock vgaLock;

	while(1)
	{
		MSG msg;
		if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage( &msg, NULL, 0, 0))
			{
				sys.signal_exit_flag = 1;
				// BUGHERE : vga_front is unlocked
				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		else if( sys.paused_flag || !sys.active_flag )
		{
			WaitMessage();
			continue;
		}
		if( sys.need_redraw_flag )
		{
			refreshFlag = SMOPTION_ALL;
			sys.need_redraw_flag = 0;
		}

		VgaFrontReLock vgaReLock;

		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		if( refreshFlag )
		{
			if( refreshFlag & SMOPTION_PAGE )
			{
				//--------- display interface screen -------//

				vga.use_back();
				vga.disp_image_file("CHOOSE");

				// -------- display title -------//

				// font_bold_black.center_put( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCORLL_SHEET_Y1+20, "Enter your name" ;
				font_bold_black.put( SCROLL_SHEET_X1+30, SCROLL_SHEET_Y1+100, text_game_menu.str_mp_your_name()); // "Your Name" );
				font_bold_black.put( SCROLL_SHEET_X1+30, SCROLL_SHEET_Y1+200, text_game_menu.str_mp_save_file_name()); // "Save Game File Name" );

				// ----- display button ------//

				if( mp_obj.is_lobbied() != 2 )		// 2 = join game
					font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_create()); // "Create" );

				if( mp_obj.is_lobbied() != 1 )		// 1 = create game
					font_thin_black.center_put( BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2, text_game_menu.str_join()); //"Join" );

				font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_cancel()); // "Cancel" );

				vga.use_front();
				vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
			}

			if( refreshFlag & SMOPTION_GETA_ALL )
			{
				keyInField.paint();
			}

			refreshFlag = 0;
		}

		sys.blt_virtual_buf();		// blt the virtual front buffer to the screen

		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		// ######## begin Gilbert 3/7 #######//
		if( mp_obj.is_lobbied() == 1 || mp_obj.is_lobbied() == 2 )
		{
			rc = mp_obj.is_lobbied();	// 1=create game 2=join game
		}

		// ------- detect key fields -------//

		else if( keyInField.detect() )
		// ######## end Gilbert 3/7 #######//
		{
			// refreshFlag |= SMOPTION_GETA_ALL;
		}

		// ------- detect create button ------//

		else if( mp_obj.is_lobbied() != 2 && mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
		{
			rc = 1;
		}

		// ------- detect join button -------//

		else if( mp_obj.is_lobbied() != 1 && mouse.single_click(BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2) )
		{
			rc = 2;
		}

		// ------- detect cancel button -------//
		
		else if( mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
		{
			rc = 0;
			break;
		}

		if( rc )
		{
			// check saveFileName is AUTO*
			if( m.str_icmpx(getSaveFile.input_field, "AUTO") ||
				m.str_icmpx(getSaveFile.input_field, "AUTO2") )
			{
				if( !box.ask( text_game_menu.str_change_auto_save()) ) // "It is not recommended to use this save game file name, do you wish to continue?") )
				{
					rc = 0;
				}
			}
		}

		if( rc )
		{
			// correct saveFileName
			int newLen;
			if( (newLen = m.str_str(getSaveFile.input_field, ".")) > 0 )
			{
				m.str_cut(getSaveFile.input_field, getSaveFile.input_field, 1, newLen-1);
			}
			if( strlen(getSaveFile.input_field) == 0 )
			{
				// empty, set back to default save game name
				strcpy( getSaveFile.input_field, "MULTI" );
			}

			strcpy(remote.save_file_name, saveFileName);
			strcat(remote.save_file_name, ".SVM");
			break;
		}
	}

	return rc;
}
//-------- End of function Game::mp_select_mode --------//


//-------- Begin of function Game::mp_select_session --------//
int Game::mp_select_session()
{

#define SSOPTION_PAGE           0x40000000
#define SSOPTION_POLL_SESSION   0x00000001
#define SSOPTION_DISP_SESSION   0x00000002
#define SSOPTION_SCROLL_BAR     0x00000004
#define SSOPTION_ALL            0x7fffffff

	const int SESSION_PER_PAGE = 10;
	const int sx1 = SCROLL_SHEET_X1+16;
	const int sx2 = SCROLL_SHEET_X2-16;
	const int sy1 = SCROLL_SHEET_Y1 + 30;
	const int sy2 = SCROLL_SHEET_Y2 - 10;
	const int ySpacing = (sy2 - sy1 + 1) / SESSION_PER_PAGE;

	// join session, but selected in lobby
	if( mp_obj.is_lobbied() == 2 )
	{
		return 1;
	}

	unsigned long refreshTime;
	int refreshFlag = SSOPTION_ALL;
	int choice = 0;
	SessionIdType sessionGuid;
	memset(&sessionGuid, 0, sizeof(sessionGuid));

	int page = 0;
	int maxPage = 1;
	int flashingJoin = 0;

	unsigned long pollTime = 1000;

	VgaFrontLock vgaLock;

	while(1)
	{
		int s;
		int b;

		MSG msg;
		if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage( &msg, NULL, 0, 0))
			{
				sys.signal_exit_flag = 1;
				// BUGHERE : vga_front is unlocked
				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		else if( sys.paused_flag || !sys.active_flag )
		{
			WaitMessage();
			continue;
		}
		if( sys.need_redraw_flag )
		{
			refreshFlag = SSOPTION_ALL;
			sys.need_redraw_flag = 0;
		}

		VgaFrontReLock vgaReLock;

		sys.yield();
		mouse.get_event();
		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		if( refreshFlag )
		{
			if( refreshFlag & SSOPTION_PAGE )
			{
				vga.use_back();
				vga.disp_image_file("CHOOSE", 0, 0 );

				// --------- display title ----------//

				font_bold_black.center_put( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCROLL_SHEET_Y1+20, text_game_menu.str_mp_select_session()); // "Select A Game" );

				// -------- display button -------//

//				font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_join()); // "Join" );
				font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, text_game_menu.str_cancel()); // "Cancel" );

				vga.use_front();
				vga.blt_buf(0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);
			}

			if( refreshFlag & SSOPTION_POLL_SESSION )
			{
				pollTime = m.get_time();
				if( !mp_obj.poll_sessions() )
				{
					// return fail if poll_sessions fails or cancel the dialogue box
					choice = 0;
					break;
				}

				// limit the pollTime between 1 sec to 10 sec
				pollTime = m.get_time() - pollTime + 1000;
				if( pollTime > 10000 )
					pollTime = 10000;

				refreshTime = m.get_time();

				// ------- sort by name ---------//
				mp_obj.sort_sessions(2);		// sort by session name

				// ------- update choice ---------- //
				choice = 0;
				for( s = 1; mp_obj.get_session(s); ++s )
				{
					if( sessionGuid == mp_obj.get_session(s)->session_id() )
						choice = s;
				}

				// ------- recalculate maxPage and update page -------//

				// s-1 is number of available session
				maxPage = (s-1 + SESSION_PER_PAGE-1) / SESSION_PER_PAGE;
				if( maxPage <= 0 )
					maxPage = 1;
				if( page >= maxPage )		// page between 0 to maxPage-1
					page = maxPage-1;

				//------- update scroll bar --------//
				// BUGHERE : error if empty
				// scrollBar.set(1, s-1, scrollBar.view_recno);
				refreshFlag |= SSOPTION_SCROLL_BAR;
				refreshFlag |= SSOPTION_DISP_SESSION;
			}

			if( refreshFlag & SSOPTION_DISP_SESSION )
			{
				for( b = 0, s = page*SESSION_PER_PAGE+1; b < SESSION_PER_PAGE; ++b, ++s )
				{
					int x1 = sx1;
					int x2 = sx2;
					int y1 = sy1 + ySpacing * b;
					int y2 = y1 + ySpacing - 4;
					vga.blt_buf( x1, y1, x2, y2, 0 );

					if( mp_obj.get_session(s) )
					{
						// display session description
						font_bld.center_put( x1, y1, x2, y2, mp_obj.get_session(s)->name_str() );

						// display cursor 
						if( s == choice )
						{
							vga.active_buf->rect( x1, y1, x2, y2, 2, V_BLACK );
						}
					}
				}
			}

			// display scroll left/right
			{
				// ---- display < / > ------//

				char *bitmapPtr = image_button.read("TRI-R");
				int bitmapWidth = ((Bitmap *)bitmapPtr)->get_width();
				int bitmapHeight = ((Bitmap *)bitmapPtr)->get_height();

				// centering
				int arrowX1 = (LSCROLL_X2 + LSCROLL_X1 - bitmapWidth) / 2;
				int arrowY1 = (LSCROLL_Y2 + LSCROLL_Y1 - bitmapHeight) / 2;
				int arrowX2 = arrowX1 + bitmapWidth-1;
				int arrowY2 = arrowY1 + bitmapHeight-1;
				if( page > 0 )
				{
					mouse.hide_area( arrowX1, arrowY1, arrowX2, arrowY2 );
					vga.active_buf->put_bitmap_trans_decompress_hmirror( arrowX1, arrowY1, bitmapPtr );
					mouse.show_area();
				}
				else
				{
					vga.blt_buf( arrowX1, arrowY1, arrowX2, arrowY2, 0 );
				}

				// centering
				arrowX1 = (RSCROLL_X2 + RSCROLL_X1 - bitmapWidth) / 2;
				arrowX2 = arrowX1 + bitmapWidth-1;
				if( page < maxPage-1 )
				{
					mouse.hide_area( arrowX1, arrowY1, arrowX2, arrowY2 );
					vga.active_buf->put_bitmap_trans_decompress( arrowX1, arrowY1, bitmapPtr );
					mouse.show_area();
				}
				else
				{
					vga.blt_buf( arrowX1, arrowY1, arrowX2, arrowY2, 0 );
				}
			}

			refreshFlag = 0;
		}

		// display join

		if( choice > 0 && flashingJoin != 0 )
			font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, text_game_menu.str_join()); // "Join" );
		else
			vga.blt_buf( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, 0 );
		if( ++flashingJoin > 4 )
			flashingJoin = 0;

		sys.blt_virtual_buf();		// blt the virtual front buffer to the screen

		if( config.music_flag )
		{
			if( !music.is_playing(3) )
				music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		int doubleClickReturn = 0;
		for( b = 0, s = page*SESSION_PER_PAGE+1; b < SESSION_PER_PAGE && mp_obj.get_session(s); ++b, ++s )
		{
			int x1 = sx1;
			int x2 = sx2;
			int y1 = sy1 + ySpacing * b;
			int y2 = y1 + ySpacing - 4;

			if( mouse.double_click( x1, y1, x2, y2 ) )
			{
				choice = s;
				sessionGuid = mp_obj.get_session(s)->session_id();
				doubleClickReturn = 1;
				break;
			}
			else if( mouse.single_click( x1, y1, x2, y2 ) )
			{
				choice = s;
				sessionGuid = mp_obj.get_session(s)->session_id();
				refreshFlag |= SSOPTION_DISP_SESSION;

				// suspend the refreshTime, so session list won't update immediate after release dragging
				refreshTime = m.get_time();

				// if poll session flag is on turn it off
				refreshFlag &= ~SSOPTION_POLL_SESSION;
			}
		}

		if( doubleClickReturn )
		{
			return choice;
		}

		char *bitmapPtr = image_button.read("TRI-R");
		int bitmapWidth = ((Bitmap *)bitmapPtr)->get_width();
		int bitmapHeight = ((Bitmap *)bitmapPtr)->get_height();
		if( page > 0 && mouse.any_click( 
			(LSCROLL_X2 + LSCROLL_X1 - bitmapWidth) / 2,
			(LSCROLL_Y2 + LSCROLL_Y1 - bitmapHeight) / 2,
			(LSCROLL_X2 + LSCROLL_X1 + bitmapWidth) / 2,
			(LSCROLL_Y2 + LSCROLL_Y1 + bitmapHeight) / 2) )
		{
			--page;
			refreshFlag |= SSOPTION_DISP_SESSION | SSOPTION_SCROLL_BAR;
		}
		
		if( page < maxPage-1 && mouse.any_click( 
			(RSCROLL_X2 + RSCROLL_X1 - bitmapWidth) / 2,
			(RSCROLL_Y2 + RSCROLL_Y1 - bitmapHeight) / 2,
			(RSCROLL_X2 + RSCROLL_X1 + bitmapWidth) / 2,
			(RSCROLL_Y2 + RSCROLL_Y1 + bitmapHeight) / 2) )
		{
			++page;
			refreshFlag |= SSOPTION_DISP_SESSION | SSOPTION_SCROLL_BAR;
		}

		// detect join button

		if( choice > 0 && mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
		{
			break;
		}

		// detect cancel button
		
		if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
		{
			choice = 0;
			break;
		}

		if( !(mouse.skey_state & SHIFT_KEY_MASK) && m.get_time() - refreshTime > pollTime )
			refreshFlag |= SSOPTION_POLL_SESSION | SSOPTION_DISP_SESSION;
	}

	return choice;
}
//-------- End of function Game::mp_select_session --------//


#define MGOPTION_PLAYERS        0x00000001
#define MGOPTION_IN_MESSAGE     0x00000002
#define MGOPTION_OUT_MESSAGE    0x00000004
#define MGOPTION_FRAME_DELAY    0x00000008
#define MGOPTION_GEM_STONES     0x00000010
#define MGOPTION_ALL            0x7fffffff

#define CHAT_LOG_X1 670
#define CHAT_LOG_Y1 500
#define CHAT_LOG_X2 686
#define CHAT_LOG_Y2 516

//-------- Begin of function Game::mp_select_option -----------//
// return 0 = cancel, 1 = ok
int Game::mp_select_option(NewNationPara *nationPara, int *mpPlayerCount)
{
	char optionMode = OPTION_SPECIES;
	char backupOptionMode;		// when switch to OPTION_CHAT_LOG, backup the current optionMode

	const int SHEET_X1 = 95;
	const int SHEET_Y1 = 74;
	const int SHEET_X2 = 703;
	const int SHEET_Y2 = 439;

	Config tempConfig = config;
	tempConfig.reset_cheat_setting();

	// some setting may be modified in the last game
	if( tempConfig.difficulty_level != OPTION_CUSTOM )
		tempConfig.change_difficulty(tempConfig.difficulty_level);

	// ######## begin Gilbert 24/5 ##########//
	// clear goals
	tempConfig.clear_goals();
	// ######## end Gilbert 24/5 ##########//

#ifdef ONLY_TERRAIN_SET_1
	tempConfig.terrain_set = 1;
#endif
#ifdef ONLY_SMALL_BUILDING
	tempConfig.building_size = 2;
#endif

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init
	TempMonsterRes tempMonsterRes;			// race_res.init

	PID_TYPE from, to;
	DWORD recvLen;
	int sysMsgCount;
	char *recvPtr;

	char raceAssignedBiased[MAX_MONSTER_TYPE+1+MAX_RACE];		// master copy belongs to host's
	const int raceBias = MAX_MONSTER_TYPE;			// access raceAssignedBiased[raceBias + raceId]
	char *raceAssigned_ = raceAssignedBiased + MAX_MONSTER_TYPE;		// change back to raceAssigned later
	char colorAssigned[MAX_COLOR_SCHEME];		// master copy belongs to host's

	DynArray messageList(sizeof(MpStructChatMsg));
	MpStructChatMsg typingMsg(tempConfig.player_name, NULL);
	
	memset( raceAssignedBiased, 0, sizeof(raceAssignedBiased) );
	memset( colorAssigned, 0, sizeof(colorAssigned) );

	PID_TYPE hostPlayerId = BROADCAST_PID;
	PID_TYPE regPlayerId[MAX_NATION];
	memset( regPlayerId, 0, sizeof(regPlayerId) );
	char playerReadyFlag[MAX_NATION];
	memset( playerReadyFlag, 0, sizeof(playerReadyFlag) );
	short playerRace[MAX_NATION];	// host only
	memset( playerRace, 0, sizeof(playerRace) );
	short playerColor[MAX_NATION];	// host only
	memset( playerColor, 0, sizeof(playerColor) );
	short playerBalance[MAX_NATION];
	memset( playerBalance, 0, sizeof(playerBalance) );
#ifdef MAX_GEM_STONES
	short useGemStones[MAX_NATION];
	memset( useGemStones, 0, sizeof(useGemStones) );
	long totalGemStones[MAX_NATION];
	memset( totalGemStones, 0, sizeof(totalGemStones) );
#endif
#ifdef APPLY_RANKING
	WORD playerRanking[MAX_NATION];
	memset( playerRanking, 0, sizeof(playerRanking) );
#endif

	int regPlayerCount = 0;
	int selfReadyFlag = 0;
	int shareRace = 1;		// host only, 0= exclusive race of each player
	int recvEndSetting = 0;			// number of MPMSG_END_SETTING received, it may come earlier
#ifdef MAX_GEM_STONES
	int selfUseGemStones = 0;
	int useGemStoneFlag = 0;
#endif

	mp_obj.poll_players();
	for( int p = 1; p <= MAX_NATION && mp_obj.get_player(p); ++p )
	{
		// host only identify himself
		if( !remote.is_host || mp_obj.get_player(p)->pid() == mp_obj.my_player_id )
		{
			regPlayerId[regPlayerCount] = mp_obj.get_player(p)->pid();
			playerReadyFlag[regPlayerCount] = 0;
			playerColor[regPlayerCount] = 0;
			playerRace[regPlayerCount] = 0;
			playerBalance[regPlayerCount] = 0;
#ifdef MAX_GEM_STONES
			useGemStones[regPlayerCount] = 0;
			totalGemStones[regPlayerCount] = 0;
#endif
#ifdef APPLY_RANKING
			playerRanking[regPlayerCount] = 0;
#endif
			++regPlayerCount;
		}
	}

#ifdef MAX_GEM_STONES
	if( remote.is_host )
	{
		useGemStoneFlag = remote.is_lobbied() &&  player_profile.mp_use_gem_stone;
		if( useGemStoneFlag )
		{
			// must use one gem stone if has any
			selfUseGemStones = player_profile.gem_stones > 0 ? 1 : 0;
			// update himself
			for( p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
			if( p < regPlayerCount )
			{
				useGemStones[p] = selfUseGemStones;
				totalGemStones[p] = player_profile.gem_stones;
			}
		}
	}

	// non-host will set to useGemStoneFlag to true when receive
	if( player_profile.mp_use_gem_stone )
	{
		player_profile.reload();
		player_profile.mp_use_gem_stone = 0;		// clear 
		player_profile.save();
	}
#endif
#ifdef APPLY_RANKING
	if( remote.is_host )
	{
		// set ranking for the host
		for( p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
		if( p < regPlayerCount )
			playerRanking[p] = player_profile.ranking;
	}
#endif

	static int tickX[MAX_NATION] = { 132, 396, 132, 396, 132, 396, 132 };
	static int tickY[MAX_NATION] = {   6,   6,  22,  22,  38,  38,  54 };
	const int nameTickWidth = 13;
	const int nameTickHeight = 13;

	//--------- initialize variable ---------//

	int i;
	int w, h;
	int cx, cy;
	String str;
	long refreshFlag = SGOPTION_ALL;
	long mRefreshFlag = MGOPTION_ALL;
	int retFlag = 0;

	// randomly select a race
	if( remote.is_host )
	{
		tempConfig.race_id = char(mp_obj.my_player_id % MAX_RACE + 1);

		// modify the race_id using the icon pointer
		if( mp_obj.is_lobbied() )
		{
			if( player_profile.icon_id +1 <= MAX_RACE )		// monster icon
				tempConfig.race_id = player_profile.icon_id + 1;
			else if( player_profile.icon_id+1 <= MAX_RACE + MAX_MONSTER_TYPE )
				tempConfig.race_id = -(player_profile.icon_id + 1 - MAX_RACE);
		}

		raceAssigned_[tempConfig.race_id] = 1;

		tempConfig.player_nation_color = 1;
		colorAssigned[tempConfig.player_nation_color-1] = 1;
		for( p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p );
		if( p < regPlayerCount )
		{
			playerRace[p] = tempConfig.race_id;
			playerColor[p] = tempConfig.player_nation_color;
			playerBalance[p] = sys.cdrom_drive ? PLAYER_RATIO_CDROM : PLAYER_RATIO_NOCD;
		}
	}
	else
	{
		tempConfig.race_id = 0;
		tempConfig.player_nation_color = 0;
		// ask host for a race and color code
		MpStructNewPlayer msgNewPlayer( mp_obj.my_player_id, 
			sys.cdrom_drive ? PLAYER_RATIO_CDROM : PLAYER_RATIO_NOCD );
		mp_obj.send_stream(BROADCAST_PID, &msgNewPlayer, sizeof(msgNewPlayer) );

		// ####### begin Gilbert 15/4 ##########//
		// put a message to the window
		MpStructChatMsg waitReplyMessage(NULL, text_game_menu.str_mp_wait_host()); // "Waiting response from host..." );
		messageList.linkin( &waitReplyMessage);
		// ####### end Gilbert 15/4 ##########//

		// send request race
		if( mp_obj.is_lobbied() )
		{
			if( player_profile.icon_id+1 <= MAX_RACE )		// monster icon
			{
				// hostPlayerId is not yet known 
				MpStructAcquireRace msgAcquire(player_profile.icon_id+1);
				mp_obj.send_stream(hostPlayerId, &msgAcquire, sizeof(msgAcquire) );
			}
			else if( player_profile.icon_id+1 <= MAX_RACE + MAX_MONSTER_TYPE )
			{
				// hostPlayerId is not yet known 
				MpStructAcquireRace msgAcquire(-(player_profile.icon_id+1-MAX_RACE));
				mp_obj.send_stream(hostPlayerId, &msgAcquire, sizeof(msgAcquire) );
			}
		}
	}


	// -------- generate palette ------//

	short colorRemapTable[0x100];
	{
		str = DIR_IMAGE;
		str += "CHOOSE.COL";

		File palFile;
		palFile.file_open(str);
		ColorTable colorTable;

		BYTE palBuf[0x100][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	// ------- init message field --------//

	GetA messageField;
	messageField.init( 126+8, 528+4, 663-4, typingMsg.content,
		MpStructChatMsg::MSG_LENGTH, &font_san, 0, 1);

	// -------- initialize info.random_seed field ------//

	const int mapIdSize = 11;		// enough to hold a dword in decimal
	char mapIdStr[mapIdSize+1];
	info.init_random_seed(0);
	ltoa( info.random_seed , mapIdStr, 10);
	// GetA mapIdField;

	MpStructSeedStr msgSeedStr(info.random_seed);
//	GetA mapIdField;
//	mapIdField.init( MAPID_X1, offsetY+83, 700, msgSeedStr.seed_str, msgSeedStr.RANDOM_SEED_MAX_LEN, &font_san, 0, 1);


	// ------ initialize human / fryhtan button ------//

	ButtonCustomGroup speciesGroup(2);
	
	w = font_thin_black.text_width( text_game_menu.str_species_humans() ); //  "Humans" 
	h = font_thin_black.text_height();
	cx = 256;
	cy = 116;
	//SXM -30
#if(defined(CHINESE))
	speciesGroup[0].create( cx-w/2-30, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );
#else
	speciesGroup[0].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );
#endif
	//SXM

	w = font_thin_black.text_width( text_game_menu.str_species_fryhtans() );
	h = font_thin_black.text_height();
	cx = 532;
	cy = 116;
#if(defined(CHINESE))
	speciesGroup[1].create( cx-w/2-10 -30, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );
#else
	speciesGroup[1].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );
#endif

	// ###### begin Gilbert 11/3 #########//
	// ----- initialize process frame delay group -------//

	char frameDelayGroupSetting = 2;		// default to high
	char frameDelaySettingArray[3] = { 2, 4, 5 };		// corresponding process frame delay value
	//char *frameDelayGroupStr[3] = { "Low", "Medium", "High" };
	ButtonCustomGroup frameDelayGroup(3);

	cx = 360;
	cy = 368;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_low_to_high(i+1) );
		h = font_thin_black.text_height();
		frameDelayGroup[i].create( cx, cy, cx+w+10, cy+20-1,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_low_to_high(i+1), i), 0 );
		cx += w+20;
	}
	// ###### end Gilbert 11/3 #########//

#ifdef MAX_GEM_STONES
	// -------- initialize gem stone group ------//

	ButtonCustomGroup gemStoneGroup(MAX_GEM_STONES+1);		// 0 - MAX_GEM_STONES
	cx = 360;
	cy = 393;
	for( i = 0; i <= MAX_GEM_STONES; ++i )
	{
		gemStoneGroup[i].create(cx, cy, cx+22, cy+20-1,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0);
		if( i > 0 && player_profile.gem_stones < i
			|| i == 0 && player_profile.gem_stones > 0 )
			gemStoneGroup[i].visible_flag = 0;
		cx += 25;
	}
#endif

	// -------- initialize player name field ------//

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// -------- initialize human group ---------//

	ButtonCustomGroup raceGroup(MAX_RACE);

	err_when( !race_res.init_flag );
	cx = 210;
	cy = 160;
	const int raceGroupYSpacing = 22;
	for( i = 0; i < MAX_RACE; ++i, (cy += raceGroupYSpacing) )
	{
		w = font_thin_black.text_width( race_res[i+1]->name );
		h = raceGroupYSpacing;
		raceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, i+1 ), 0 );
	}

	// --------- initialize monster group -------//

	ButtonCustomGroup monsterRaceGroup(MAX_MONSTER_TYPE);		// MAX_MONSTER
	err_when( !monster_res.init_flag );
	cx = 210;
	cy = 160;
	const int monsterGroupYSpacing = 22;
	for( i = 0; i < MAX_MONSTER_TYPE; ++i, (cy += monsterGroupYSpacing) )		// MAX_MONSTER
	{
		w = font_thin_black.text_width( monster_res[i+1]->name );
		h = monsterGroupYSpacing;
		monsterRaceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, -(i+1) ), 0 );
	}

	// ------- create color group --------//

	ButtonCustomGroup colorGroup(MAX_COLOR_SCHEME);
	const int colorButtonFrameX = 382;
	const int colorButtonFrameY = 155;
	cx = 391;
	cy = 162;
	const int colorButtonWidth = 29;
	const int colorButtonHeight = 30;
	for(i = 0; i < MAX_COLOR_SCHEME; ++i, (cx+=colorButtonWidth) )
	{
		colorGroup[i].create( cx, cy, cx+colorButtonWidth-1, cy+colorButtonHeight-1, 
			i_disp_color_button, ButtonCustomPara(NULL, i+1), 0 );
	}

	// -------- create ai human nation group -------//

	ButtonCustomGroup aiHumanNationGroup(MAX_NATION);
	cx = 361;
	cy = 258-20;
	const int nationButtonSpacing = 30;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiHumanNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiHumanNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// -------- create ai monster nation group -------//

	ButtonCustomGroup aiMonsterNationGroup(MAX_NATION);
	cx = 361;
	cy = 280-20;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiMonsterNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiMonsterNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// ---------- initialize difficulty_level button group -------//

	ButtonCustomGroup diffGroup(OPTION_HARDEST-OPTION_EASIEST+2);		// 0-7 + OPTION_CUSTOM
	char diffButtonStr[OPTION_HARDEST-OPTION_EASIEST+1][10];
	cx = 360;
	cy = 331-35;
	for( i = 0; i < OPTION_HARDEST-OPTION_EASIEST+1; ++i )
	{
		strcpy( diffButtonStr[i], m.roman_number(i+1) );
		w = font_thin_black.text_width(diffButtonStr[i]);
		h = font_thin_black.text_height();
		diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(diffButtonStr[i], i+OPTION_EASIEST), 0);
		cx += w + 12;
	}

	w = font_thin_black.text_width(text_game_menu.str_difficulty_custom());
	h = font_thin_black.text_height();
	diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara(text_game_menu.str_difficulty_custom(), OPTION_CUSTOM), 0);
	cx += w + 12;

	// --------- initialize terrain group --------//

	ButtonCustomGroup terrainGroup(2);
	// char *terrainButtonStr[3] = { "Fertile", "Badlands", "Arctic" };
	cx = 370+45;
	cy = 406-65;
	for( i = 0; i < terrainGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_terrain_set(i+1) );
		h = font_thin_black.text_height();
		terrainGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_terrain_set(i+1), i+1), 0 );
		cx += w + 12;
	}

	// --------- initialize building size --------//

	ButtonCustomGroup buildingSizeGroup(2);
	// char *buildingSizeButtonStr[2] = { "Large", "Small" };
	cx = 370+63;
	cy = 386;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_building_set(i+1) );
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_building_set(i+1), i+1), 0 );
		cx += w + 12;
	}
	
	// ------- initialize explored group ------//

	const int option3X = 320;
	ButtonCustomGroup exploreGroup(2);
	cx = option3X;
	cy = 99;
	w = font_thin_black.text_width( text_game_menu.str_world_map(0) );	// "Unexplored"
	h = font_thin_black.text_height();
	exploreGroup[0].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_world_map(1) );	// "Explored"
	exploreGroup[1].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(1), 1), 0 );

	// ------ initialize fog of war ------//

	ButtonCustomGroup fogGroup(2);
	cx = option3X;
	cy = 143;
	w = font_thin_black.text_width( text_game_menu.str_off_on(0) );	// "Off"
	h = font_thin_black.text_height();
	fogGroup[0].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_off_on(1) );
	fogGroup[1].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(1), 1), 0 );

	// --------- initialize start_up_cash/start_up_food button group -------//

//	char *treasureSizeStr[4] = { "Small", "Medium", "Large", "Huge" };

	ButtonCustomGroup treasureGroup(4);
	cx = option3X;
	cy = 183;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		treasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_start_up_cash/food button group -------//

	ButtonCustomGroup aiTreasureGroup(4);
	cx = option3X;
	cy = 235;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		aiTreasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_aggressiveness -------//

	// #### begin Gilbert 10/2 #########//
//	char *aggressiveStr[5] = { "Very Low", "Low", "Medium", "Great", "Very great" };

	ButtonCustomGroup aiAggressiveGroup(5);
	cx = option3X;
	cy = 289;
	for( i = 0; i < 5; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_aggressiveness(i) );
		h = font_thin_black.text_height();
		aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		if( aiAggressiveGroup[i].x2 > SCROLL_SHEET_X2 )
		{	// line feed
			cy = aiAggressiveGroup[i].y2+1 ;
			cx = option3X;
			aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
				i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		}
		cx += w+24;
	}
	// #### end Gilbert 10/2 #########//

	// --------- initialize spy methodology-------//

	// char *spyMethodStr[2] = { "Must  Research", "All  Available" };

	ButtonCustomGroup spyMethodGroup(2);
	cx = option3X;
	// US : left to right
	cy = 348;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_need_research(i) );
		h = font_thin_black.text_height();
		spyMethodGroup[i].create( cx, cy, cx+w+10, cy+h+5,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_need_research(i), i), 0 );
		cx += w+24;
	}

	// --------- initialize random startup button group -------//

//	char *yesNoStr[4] = { "No", "Yes" };

	ButtonCustomGroup randomStartUpGroup(2);
	cx = option3X;
	cy = 389;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		randomStartUpGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	//  -------- initialize start_up_raw_site buttons --------- //

	const int option4X = 410;
	ButtonCustomGroup rawSiteGroup(7);
	cx = option4X;
	cy = 100;
	for( i = 0; i < 7; ++i )
	{
		rawSiteGroup[i].create(cx, cy, cx+15, cy+font_thin_black.height()+10,
			i_disp_int_button, ButtonCustomPara(NULL, i+1), 0);
		cx += 25;
	}

	// --------- initialize start_up_has_mine_nearby button group --------//

	ButtonCustomGroup nearRawGroup(2);
	cx = option4X;
	cy = 152;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nearRawGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	// --------- initialize start_up_independent_town button group --------//

	short startTownArray[3] = { 7, 15, 30 };
//	char *fewManyStr[3] = { "Few", "More", "Many" };

	ButtonCustomGroup townStartGroup(3);
	cx = option4X;
	cy = 206;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_few_to_many(i+1) );
		h = font_thin_black.text_height();
		townStartGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_few_to_many(i+1), startTownArray[i]), 0);
		cx += w+20;
	}

	// --------- initialize independent_town_resistance button group --------//

//	char *lowHighStr[3] = { "Low", "Med", "High" };

	ButtonCustomGroup townResistGroup(3);
	cx = option4X;
	cy = 246;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_low_to_high(i+1) );
		h = font_thin_black.text_height();
		townResistGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_low_to_high(i+1), i+1), 0);
		cx += w+20;
	}

	// --------- initialize new_independent_town_emerge button group --------//

	ButtonCustomGroup townEmergeGroup(2);
	cx = option4X;
	cy = 293;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		townEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize new_nation_emerge button group --------//

	ButtonCustomGroup nationEmergeGroup(2);
	cx = option4X;
	cy = 340;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nationEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize random_event_frequency button group --------//

//	char *randomEventStr[4] = { "Never", "Seldom", "Occasional", "Frequent" };

	ButtonCustomGroup randomEventGroup(4);
	cx = option4X;
	cy = 384;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_never_to_frequent(i) );
		h = font_thin_black.text_height();
		randomEventGroup[i].create(cx, cy, cx+w+10, cy+h+6,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_never_to_frequent(i), i), 0);
		if( (i+1) % 2 == 0 )
		{
			// line feed
			cx = option4X;
			cy += 26;
		}
		else
		{
			cx += 144;
		}
	}

	// ------- initialize goal button ---------- //

	// ###### begin Gilbert 11/3 #########//
	ButtonCustom clearEnemyButton, clearMonsterButton, allyWinButton, enoughPeopleButton, enoughIncomeButton, enoughScoreButton, timeLimitButton;
	Button peopleInc, peopleDec, incomeInc, incomeDec, scoreInc, scoreDec, yearInc, yearDec;

	const int option5X = 126;
	const int tickWidth = 20;
	const int tickHeight = 20;
	const int option5X2 = 620;
	const int option5X3 = 642;
	w = tickWidth;
	h = tickHeight;

	clearEnemyButton.create( option5X, 145, option5X+w-1, 145+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 1);
	clearEnemyButton.enable_flag = 0;
	clearMonsterButton.create( option5X, 178, option5X+w-1, 178+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_destroy_monster);
	allyWinButton.create( option5X, 211, option5X+w-1, 211+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_alliance_win_flag);
	enoughPeopleButton.create( option5X, 244, option5X+w-1, 244+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_population_flag);
	enoughIncomeButton.create( option5X, 277, option5X+w-1, 277+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_economic_score_flag);
	enoughScoreButton.create( option5X, 310, option5X+w-1, 310+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_total_score_flag);
	timeLimitButton.create( option5X, 343, option5X+w-1, 343+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_year_limit_flag);

	peopleInc.create_text( option5X2, 244, option5X2+w-1, 244+h-1, "+" );
	peopleDec.create_text( option5X3, 244, option5X3+w-1, 244+h-1, "-" );

	incomeInc.create_text( option5X2, 277, option5X2+w-1, 277+h-1, "+" );
	incomeDec.create_text( option5X3, 277, option5X3+w-1, 277+h-1, "-" );

	scoreInc.create_text( option5X2, 310, option5X2+w-1, 310+h-1, "+" );
	scoreDec.create_text( option5X3, 310, option5X3+w-1, 310+h-1, "-" );

	yearInc.create_text( option5X2, 343, option5X2+w-1, 343+h-1, "+" );
	yearDec.create_text( option5X3, 343, option5X3+w-1, 343+h-1, "-" );
	// ###### end Gilbert 11/3 #########//

	// ------- loop ---------//

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return 0;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			// ######### begin Gilbert 14/5 ########//
			else if( !sys.use_true_front && (sys.paused_flag || !sys.active_flag) )
			// ######### end Gilbert 14/5 ########//
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SGOPTION_ALL;
				mRefreshFlag = MGOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// -------- display ----------//
			// ######### begin Gilbert 14/5 ########//
			if( !sys.paused_flag && sys.active_flag && (refreshFlag || mRefreshFlag) )
			// ######### end Gilbert 14/5 ########//
			{
				if( optionMode == OPTION_SPECIES )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ###### begin Gilbert 29/6 #########//
						// ----- display picture -------//

						const int pictureWidth = 209; // 298;
						const int pictureHeight = 210; // 300;
						const int pictureXoffset = 35;
						const int pictureYoffset = 10;

						if( !vga.use_back_buf )
						{
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}
						else
						{
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}

						// "Network Latency"
						font_bold_black.put( 126, 368, text_game_menu.str_mp_latency(), 0, 360-10 );

						// ###### end Gilbert 29/6 #########//

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
						speciesGroup.paint( tempConfig.race_id < 0 );

					// ####### begin Gilbert 11/3 ######//
					if( mRefreshFlag & MGOPTION_FRAME_DELAY )
						frameDelayGroup.paint(frameDelayGroupSetting);
					// ####### end Gilbert 11/3 ######//

#ifdef MAX_GEM_STONES
					// display number gem stone to use
					if( mRefreshFlag & MGOPTION_GEM_STONES && useGemStoneFlag )
					{
						// "Gem Stones (xxx)" );
						font_bold_black.put( 126, 393, 
							text_game_menu.str_mp_gem_stones(player_profile.gem_stones), 1, 360-10 );
						// 15% per each gemstones
						font_small.put( 126, 413,
							text_game_menu.str_mp_gem_stones_inst( 15 * selfUseGemStones ), 1, 360-10 );
						gemStoneGroup.paint(selfUseGemStones);
					}
#endif
					// other ...
				}

				// ------- display basic option ---------//
				if( optionMode == OPTION_BASIC )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");
						// BUGHERE : option menu column and finger

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.right_put( playerNameField.x, playerNameField.y,
							text_game_menu.str_kingdom_of() ); // "Kingdom of " );

						if( speciesGroup() == 0 )
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_nationality()); // "Nationality");
						else if( speciesGroup() == 1 )
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_species()); // "Fryhtan Species");
						font_bold_black.center_put( 382, 129, 600, 149, text_game_menu.str_color()); // "Color" );
						font_bold_black.center_put( 297, 225-20, 698, 243-20, text_game_menu.str_ai_nation_count()); // "Computer Controlled Kingdoms" );
						font_bold_black.put( aiHumanNationGroup[MAX_NATION-1].x2+10, aiHumanNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_human() ); // "Human" );
						font_bold_black.put( aiMonsterNationGroup[MAX_NATION-1].x2+10, aiMonsterNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_fryhtan() ); //"Fryhtan" );
						font_bold_black.center_put( 341, 305-30, 654, 324-30, text_game_menu.str_difficulty_level()); // "Difficulty Level" );
#ifndef ONLY_TERRAIN_SET_1
						font_bold_black.center_put( 370, 380-60, 633, 393-60, text_game_menu.str_terrain_set()); // "Terrain Type" );
#endif
#ifndef ONLY_SMALL_BUILDING
						font_bold_black.center_put( 341, 365, 660, 384, text_game_menu.str_building_set()); // "Building Size" );
#endif

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
					{
						if( speciesGroup() == 0 )		// human page
						{
							if( tempConfig.race_id > 0 )
								raceGroup.paint( tempConfig.race_id-1 );
							else
							{
								raceGroup.push( -1, 0 );		// pop all button, and paint
								raceGroup.paint();
							}
						}
						else if( speciesGroup() == 1 )		// monster page
						{
							if( tempConfig.race_id < 0 )
							{
								monsterRaceGroup.paint( -tempConfig.race_id-1 );

								char str[10] = "P-X";
								str[2] = '0' - tempConfig.race_id;
								char* bitmapPtr = image_interface.get_ptr(str);
								// all p-x must be the same size
								// center to 220,370
								const int picWidth = 198;
								const int picHeight = 121;
								const int picX = 220-picWidth/2;
								const int picY = 370-picHeight/2;
								if( !vga.use_back_buf )
								{
									mouse.hide_area(picX, picY, picX+picWidth-1, picY+picHeight-1 );
									vga.active_buf->join_trans( &vga_back, picX, picY, bitmapPtr );
									mouse.show_area();
								}
								else
								{
									vga.active_buf->put_bitmap_trans( picX, picY, bitmapPtr );
								}
							}
							else
							{
								monsterRaceGroup.push( -1, 0 );		// pop all button and paint
								monsterRaceGroup.paint();
							}
						}
					}
					if( refreshFlag & SGOPTION_COLOR )
					{
						vga.use_back();		// to avoid flickering

						// ------ put color box ------ //
						char *bitmapPtr = image_button.read("F-COLOR");
						vga.active_buf->put_bitmap_trans_remap_decompress(
							colorButtonFrameX, colorButtonFrameY, bitmapPtr,
							game.color_remap_array[tempConfig.player_nation_color].color_table );
						colorGroup.paint(tempConfig.player_nation_color-1);

						vga.use_front();

						vga.blt_buf( colorButtonFrameX, colorButtonFrameY,
							colorButtonFrameX + ((Bitmap *)bitmapPtr)->get_width() - 1,
							colorButtonFrameY + ((Bitmap *)bitmapPtr)->get_height() - 1, 0 );
					}
					if( refreshFlag & SGOPTION_AI_NATION )
					{
						aiHumanNationGroup.paint(tempConfig.ai_human_nation_count);
						aiMonsterNationGroup.paint(tempConfig.ai_monster_nation_count);
					}
					if( refreshFlag & SGOPTION_DIFFICULTY )
					{
						if( tempConfig.difficulty_level != OPTION_CUSTOM )
							diffGroup.paint(tempConfig.difficulty_level);
						else
							diffGroup.paint(diffGroup.button_num-1);	// last button
					}
#ifndef ONLY_TERRAIN_SET_1
					if( refreshFlag & SGOPTION_TERRAIN )
						terrainGroup.paint(tempConfig.terrain_set-1);
#endif
#ifndef ONLY_SMALL_BUILDING
					if( refreshFlag & SGOPTION_BUILDING_SIZE )
						buildingSizeGroup.paint(tempConfig.building_size-1);
#endif
					if( refreshFlag & SGOPTION_NAME_FIELD )
						playerNameField.paint(0);		// don't put cursor, it is not inputable
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCED )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put_paragraph( 126, 99, option3X-10, 143-1,
							text_game_menu.str_world_map()); // "World Map" );
						font_bold_black.put_paragraph( 126, 143, option3X-10, 183-1,
							text_game_menu.str_fog_of_war()); // "Fog of War" );
						font_bold_black.put_paragraph( 126, 183, option3X-10, 235-1,
						//	"Human Player's Treasure" );
							text_game_menu.str_your_treasure()); // "Your Treasure" );
						font_bold_black.put_paragraph( 126, 235, option3X-10, 289-1,
							text_game_menu.str_ai_treasure()); // "Computer's Treasure" );
						font_bold_black.put_paragraph( 126, 289, option3X-10, 339-1,
							text_game_menu.str_ai_aggressiveness()); // "Computer's Aggressiveness" );
#if(defined(GERMAN))
						font_bold_black.put_paragraph( 126, 348, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#else
						font_bold_black.put_paragraph( 126, 339, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#endif
						font_bold_black.put_paragraph( 126, 389, option3X-10, 449-1,
							text_game_menu.str_random_kingdom()); // "Random Kingdoms" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_EXPLORED )
						exploreGroup.paint(tempConfig.explore_whole_map);
					if( refreshFlag & SGOPTION_FOG )
						fogGroup.paint(tempConfig.fog_of_war);
					if( refreshFlag & SGOPTION_TREASURE )
						treasureGroup.paint( tempConfig.start_up_cash-1 );
					if( refreshFlag & SGOPTION_AI_TREASURE )
						aiTreasureGroup.paint( tempConfig.ai_start_up_cash-1 );
					// ##### begin Gilbert 10/2 #####//
					if( refreshFlag & SGOPTION_AI_AGGRESSIVE )
						aiAggressiveGroup.paint(tempConfig.ai_aggressiveness);
					// ##### end Gilbert 10/2 #####//
					if( refreshFlag & SGOPTION_SPY_METHOD )
						spyMethodGroup.paint(tempConfig.spy_methodology);
					if( refreshFlag & SGOPTION_RANDOM_STARTUP )
						randomStartUpGroup.paint(tempConfig.random_start_up);
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCE2 )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put_paragraph( 126, 100, option4X-10, 152-1,
							text_game_menu.str_start_up_raw_site()); // "Natural Resources at Start" );
						font_bold_black.put_paragraph( 126, 152, option4X-10, 206-1,
							text_game_menu.str_raw_nearby()); // "Natural Resources Nearby" );
						font_bold_black.put_paragraph( 126, 206, option4X-10, 246-1,
							text_game_menu.str_independent_town()); // "Independent Towns" );
						font_bold_black.put_paragraph( 126, 246, option4X-10, 293-1,
							text_game_menu.str_town_resistance()); // "Independent Town Resistance" );
						font_bold_black.put_paragraph( 126, 293, option4X-10, 340-1,
							text_game_menu.str_new_town_emerge()); // "New Towns Emerge" );
						font_bold_black.put_paragraph( 126, 340, option4X-10, 384-1,
							text_game_menu.str_new_kingdom_emerge()); // "New Kingdoms Emerge" );
						font_bold_black.put_paragraph( 126, 384, option4X-10, 434-1,
							text_game_menu.str_random_events()); // "Random Events" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RAW )
						rawSiteGroup.paint(tempConfig.start_up_raw_site-1);
					if( refreshFlag & SGOPTION_NEAR_RAW )
						nearRawGroup.paint(tempConfig.start_up_has_mine_nearby);
					if( refreshFlag & SGOPTION_START_TOWN )
					{
						for( i = 0; tempConfig.start_up_independent_town > startTownArray[i]; ++i );
						townStartGroup.paint(i);
					}
					if( refreshFlag & SGOPTION_TOWN_STRENGTH )
						townResistGroup.paint(tempConfig.independent_town_resistance-1);
					if( refreshFlag & SGOPTION_TOWN_EMERGE )
						townEmergeGroup.paint(tempConfig.new_independent_town_emerge);
					if( refreshFlag & SGOPTION_KINGDOM_EMERGE )
						nationEmergeGroup.paint(tempConfig.new_nation_emerge);
					if( refreshFlag & SGOPTION_RANDOM_EVENT )
						randomEventGroup.paint(tempConfig.random_event_frequency);
				}

				// ------- display goal option ---------//
				if( optionMode == OPTION_GOAL )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put( option5X, 112, text_game_menu.str_goal() );	// You will be victorious when you have:

						font_thin_black.put_paragraph( option5X+tickWidth+10, 145, option5X3+25, 178-1, 
							text_game_menu.str_defeat_others()); // "Defeated All Others" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 178, option5X3+25, 211-1, 
							text_game_menu.str_defeat_fryhtan_lairs()); // "Destroyed All Independent Fryhtan Lairs" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 211, option5X3+25, 244-1, 
							text_game_menu.str_defeat_except_ally()); // "Defeated All except one Allied Kingdom" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 244, option5X2-65, 277-1, 
							text_game_menu.str_reach_population()); // "Achieved A Population Of" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 277, option5X2-65, 310-1, 
							text_game_menu.str_reach_economy()); // "Reached An Economic Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 310, option5X2-65, 343-1, 
							text_game_menu.str_reach_total_score()); //"Reached A Total Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 343, option5X2-100, 376-1, 
							text_game_menu.str_goal_time_limit()); // "Achieved The Selected Within" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					font_bold_black.use_max_height();

					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						clearEnemyButton.paint();
					if( refreshFlag & SGOPTION_CLEAR_MONSTER )
						clearMonsterButton.paint(tempConfig.goal_destroy_monster);
					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						allyWinButton.paint(tempConfig.goal_alliance_win_flag);
					if( refreshFlag & SGOPTION_ENOUGH_PEOPLE )
					{
						enoughPeopleButton.paint(tempConfig.goal_population_flag);
						font_bold_black.put( peopleInc.x1-65, peopleInc.y1, 
							m.format(tempConfig.goal_population), 1, peopleInc.x1-1);
						peopleInc.paint();
						peopleDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_INCOME )
					{
						enoughIncomeButton.paint(tempConfig.goal_economic_score_flag);
						font_bold_black.put( incomeInc.x1-65, incomeInc.y1, 
							m.format(tempConfig.goal_economic_score), 1, incomeInc.x1-1 );
						incomeInc.paint();
						incomeDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_SCORE )
					{
						enoughScoreButton.paint(tempConfig.goal_total_score_flag);
						font_bold_black.put( scoreInc.x1-65, scoreInc.y1, 
							m.format(tempConfig.goal_total_score), 1, scoreInc.x1-1 );
						scoreInc.paint();
						scoreDec.paint();
					}
					if( refreshFlag & SGOPTION_TIME_LIMIT )
					{
						timeLimitButton.paint(tempConfig.goal_year_limit_flag);
						int x2 = font_bold_black.put( yearInc.x1-100, yearInc.y1, 
							m.format(tempConfig.goal_year_limit), 1, yearInc.x1-60-1 );
						font_thin_black.put( yearInc.x1-60, yearInc.y1, text_game_menu.str_goal_time_units(), 1 );	// "years"
						yearInc.paint();
						yearDec.paint();
					}

					font_bold_black.use_std_height();
				}

				// ##### begin Gilbert 6/7 ##########//
				// ------- display chat log --------//
				if( optionMode == OPTION_CHAT_LOG )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, SCROLL_SHEET_Y1+11, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					font_bold_black.use_max_height();
					font_bold_black.use_std_height();
				}
				// ##### end Gilbert 6/7 ##########//

				// -------- refresh players in the session --------//
				if( mRefreshFlag & MGOPTION_PLAYERS )
				{
					vga.use_back();
					vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
					for( p = 0; p < regPlayerCount; ++p)
					{
						// put a tick if ready
						if( !playerReadyFlag[p] )
						{
							vga.active_buf->bar( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, V_WHITE );
							vga.active_buf->d3_panel_up( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, 4, 0 );
						}
						else
						{
							vga.active_buf->d3_panel_down( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, 4, 0 );
							vga.active_buf->put_bitmap_trans_decompress( tickX[p]+4, tickY[p]+4,
								image_button.read("TICK-S") );
						}

						// put color of player
						vga.active_buf->bar( tickX[p]+nameTickWidth, tickY[p], tickX[p]+nameTickWidth*2-1, tickY[p]+nameTickHeight-1,
							game.color_remap_array[playerColor[p]].main_color );
						vga.active_buf->d3_panel_up( tickX[p]+nameTickWidth, tickY[p], tickX[p]+nameTickWidth*2-1, tickY[p]+nameTickHeight-1, 4, 0 );
						PlayerDesc *dispPlayer = mp_obj.search_player(regPlayerId[p]);
						int x2 = font_bld.put( tickX[p]+nameTickWidth*2+3, tickY[p]+0, dispPlayer?dispPlayer->friendly_name_str():(char*)"?anonymous?",
							0, tickX[p]+260 );
#ifdef MAX_GEM_STONES
						if( totalGemStones[p] )
						{
							x2 = font_bld.put(x2, tickY[p]+0, " (", 0, tickX[p]+260 );
							if( sys.debug_session )
							{
								x2 = font_bld.put(x2, tickY[p]+0, useGemStones[p], 0, tickX[p]+260 );
								x2 = font_bld.put(x2, tickY[p]+0, "/", 0, tickX[p]+260 );
							}
							x2 = font_bld.put(x2, tickY[p]+0, totalGemStones[p], 0, tickX[p]+260 );
							x2 = font_bld.put(x2, tickY[p]+0, ")", 0, tickX[p]+260 );
						}
#endif
					}
					vga.blt_buf( 126, 1, 665, 69, 0 );
					vga.use_front();
				}

				// ------------ display chat message --------//
				if( mRefreshFlag & MGOPTION_OUT_MESSAGE )
				{
					messageField.paint();
				}

				// ------------- display incoming chat message --------//
				if( mRefreshFlag & MGOPTION_IN_MESSAGE )
				{
					vga.use_back();
					// ######### begin Gilbert 6/7 ##########//
					int y1 = optionMode != OPTION_CHAT_LOG ? 444 : SCROLL_SHEET_Y1+11;
					int y2 = 516;
					vga.active_buf->d3_panel_down( 126, y1, 665, y2 );
					int ny = y1 + 4;
					int lineDisp = (y2-4 - ny + 1 ) / font_cara.max_font_height;
					p = messageList.size() - lineDisp + 1;		// display 4 lines
					if( p <= 0 )
						p = 1;
					for( ; p <= messageList.size() ; ++p, (ny += font_cara.max_font_height))
					{
						int nx = 132;
						if( ((MpStructChatMsg *)messageList.get(p))->sender[0] )
						{
							nx = font_cara.put( nx, ny, ((MpStructChatMsg *)messageList.get(p))->sender, 0, 661);
							nx = font_cara.put( nx, ny, " : ", 0, 661 );
						}
						nx = font_cara.put( nx, ny, ((MpStructChatMsg *)messageList.get(p))->content, 0, 661 );
					}
					vga.blt_buf( 126, y1, 665, y2, 0 );
					vga.use_front();

					// display button
					if( optionMode != OPTION_CHAT_LOG )
						vga.active_buf->d3_panel_up( CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2 );
					else
						vga.active_buf->d3_panel_down( CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2 );
					// ######### end Gilbert 6/7 ##########//
				}

				// ----- display difficulty -------//

				if( optionMode != OPTION_SPECIES && refreshFlag & SGOPTION_DIFFICULTY )
				{
					font_bld.center_put( 40, 85, 82, 105, text_game_menu.str_level_in_score(), 1 );	// "Level"
					font_bold_black.center_put( 40, 105, 82, 125, m.format(tempConfig.multi_player_difficulty(regPlayerCount-1, 0)), 1 );
				}

				refreshFlag = 0;
				mRefreshFlag = 0;
			}

			sys.blt_virtual_buf();

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// --------- detect remote message -------//
			mp_obj.before_receive();
			recvPtr = mp_obj.receive(&from, &to, &recvLen, &sysMsgCount);
			mp_obj.after_send();

			if( sysMsgCount )
			{
				// see any player disconnect
				for(int q = 0; q < regPlayerCount; ++q)
				{
					if( !mp_obj.is_player_connecting(regPlayerId[q]) )
					{
						mRefreshFlag |= MGOPTION_PLAYERS;

						memmove( regPlayerId+q, regPlayerId+q+1, (MAX_NATION-1-q)*sizeof(regPlayerId[0]) );
						regPlayerId[MAX_NATION-1] = 0;
						memmove( playerReadyFlag+q, playerReadyFlag+q+1, (MAX_NATION-1-q)*sizeof(playerReadyFlag[0]) );
						playerReadyFlag[MAX_NATION-1] = 0;
						short freeColor = playerColor[q];
						memmove( playerColor+q, playerColor+q+1, (MAX_NATION-1-q)*sizeof(playerColor[0]) );
						playerColor[MAX_NATION-1] = 0;
						if(freeColor > 0 && freeColor <= MAX_COLOR_SCHEME)
							colorAssigned[freeColor-1] = 0;
						short freeRace = playerRace[q];
						memmove( playerRace+q, playerRace+q+1, (MAX_NATION-1-q)*sizeof(playerRace[0]) );
						playerRace[MAX_NATION-1] = 0;
						if(freeRace != 0)
							raceAssigned_[freeRace]--;
						memmove( playerBalance+q, playerBalance+q+1, (MAX_NATION-1-q)*sizeof(playerBalance[0]) );
						playerBalance[MAX_NATION-1] = 0;
#ifdef MAX_GEM_STONES
						memmove( useGemStones+q, useGemStones+q+1, (MAX_NATION-1-q)*sizeof(useGemStones[0]) );
						useGemStones[MAX_NATION-1] = 0;
						memmove( totalGemStones+q, totalGemStones+q+1, (MAX_NATION-1-q)*sizeof(totalGemStones[0]) );
						totalGemStones[MAX_NATION-1] = 0;
#endif
#ifdef APPLY_RANKING
						memmove( playerRanking+q, playerRanking+q+1, (MAX_NATION-1-q)*sizeof(playerRanking[0]) );
						playerRanking[MAX_NATION-1] = 0;
#endif
						--regPlayerCount;
						--q;
					}
				}
			}

			if( recvPtr )
			{
				if( ((MpStructBase *)recvPtr)->msg_id == MPMSG_START_GAME )
				{
					retFlag = 1;
					break;		// break while(1) loop
				}
				else
				{
					switch( ((MpStructBase *)recvPtr)->msg_id )
					{
					case MPMSG_ABORT_GAME:
						box.msg( text_game_menu.str_mp_host_abort() );
						return 0;
					case MPMSG_SEND_CONFIG:
						// ##### patch begin Gilbert 14/1 #######//
						// now validate config sent from host
						if( ((MpStructConfig *)recvPtr)->game_config.validate() )
						{
							tempConfig.change_game_setting( ((MpStructConfig *)recvPtr)->game_config );
							refreshFlag |= SGOPTION_ALL_OPTIONS;
						}
						else
						{
							box.msg( text_game_menu.str_mp_refuse_reason(MpStructRefuseNewPlayer::REFUSE_VERSION) );
							return 0;
						}
						// ##### patch end Gilbert 14/1 #######//
						break;
					case MPMSG_RANDOM_SEED:
						info.init_random_seed( ((MpStructSeed *)recvPtr)->seed );
						break;
					case MPMSG_RANDOM_SEED_STR:
						msgSeedStr = *(MpStructSeedStr *)recvPtr;
//						mapIdField.select_whole();
						refreshFlag |= SGOPTION_MAP_ID;
						break;
					case MPMSG_NEW_PLAYER:
						mp_obj.poll_players();
						if( remote.is_host )
						{
							MpStructNewPlayer *newPlayerMsg = (MpStructNewPlayer *)recvPtr;
							if( regPlayerCount >= MAX_NATION )
							{
								// reply refuse new player
								MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_SEAT_FULL);
								mp_obj.send_stream( BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
							}
							else if( !newPlayerMsg->is_compatible() )
							{
								// reply refuse new player
								MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_VERSION);
								mp_obj.send_stream( BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
							}
							else
							{
								regPlayerId[regPlayerCount] = newPlayerMsg->player_id;
								playerReadyFlag[regPlayerCount] = 0;

								// send accept new player to all player
								MpStructAcceptNewPlayer msgAccept(from);
								mp_obj.send_stream( BROADCAST_PID, &msgAccept, sizeof(msgAccept) );

								// assign initial race
								int c = m.get_time() % MAX_RACE;
								for( int t = 0; t < MAX_RACE; ++t, ++c )
								{
									c %= MAX_RACE;
									if( raceAssigned_[c+1] == 0 )
									{
										raceAssigned_[c+1]++;
										playerRace[regPlayerCount] = c+1;
										MpStructAcceptRace msgAcceptRace(from, c+1);
										mp_obj.send_stream( from, &msgAcceptRace, sizeof(msgAcceptRace) );
										break;
									}
								}
								err_when( t >= MAX_RACE );		// not found

								// assign initial color
								c = m.get_time() % MAX_COLOR_SCHEME;
								for( t = 0; t < MAX_COLOR_SCHEME; ++t, ++c )
								{
									c %= MAX_COLOR_SCHEME;
									if( !colorAssigned[c] )
									{
										colorAssigned[c]=1;
										playerColor[regPlayerCount] = c+1;
										MpStructAcceptColor msgAcceptColor(from, c+1);
										mp_obj.send_stream( BROADCAST_PID, &msgAcceptColor, sizeof(msgAcceptColor) );
										break;
									}
								}
								err_when( t >= MAX_COLOR_SCHEME );		// not found

								// send random seed
								mp_obj.send_stream( from, &msgSeedStr, sizeof(msgSeedStr) );

								// send config 
								MpStructConfig msgConfig( tempConfig );
								mp_obj.send_stream( from, &msgConfig, sizeof(msgConfig) );

								// send ready flag
								for( c = 0; c < regPlayerCount; ++c)
								{
									if( playerReadyFlag[c] )
									{
										MpStructPlayerReady msgReady(regPlayerId[c]);
										mp_obj.send_stream(from, &msgReady, sizeof(msgReady));
									}
								}

								// send color chosen
								for( c = 0; c < regPlayerCount; ++c)
								{
									if( playerColor[c] )
									{
										MpStructAcceptColor msgAcceptColor(regPlayerId[c], playerColor[c]);
										mp_obj.send_stream( from, &msgAcceptColor, sizeof(msgAcceptColor) );
									}
								}

								// ##### begin Gilbert 11/3 ########//
								// send process frame delay
								MpStructProcessFrameDelay msgframeDelay(frameDelaySettingArray[frameDelayGroupSetting]);
								mp_obj.send_stream( from, &msgframeDelay, sizeof(msgframeDelay) );
								// ##### end Gilbert 11/3 ########//

								// send remote.sync_test_level
								MpStructSyncLevel msgSyncTest(remote.sync_test_level);
								mp_obj.send_stream( from, &msgSyncTest, sizeof(msgSyncTest) );

								// update balance
								playerBalance[regPlayerCount] = newPlayerMsg->player_balance;
#ifdef MAX_GEM_STONES
								useGemStones[regPlayerCount] = 0;
								totalGemStones[regPlayerCount] = 0;

								// send use Gemstone
								if( useGemStoneFlag )
								{
									for( c = 0; c < regPlayerCount; ++c)
									{
										MpStructSetGemStones msgUseGemStones(regPlayerId[c], useGemStones[c], totalGemStones[c]);
										mp_obj.send_stream(from, &msgUseGemStones, sizeof(msgUseGemStones));
									}
								}
#endif
#ifdef APPLY_RANKING
								playerRanking[regPlayerCount] = newPlayerMsg->ranking;
#endif

								regPlayerCount++;
								mRefreshFlag |= MGOPTION_PLAYERS;
							}
						}
						break;
					case MPMSG_LOAD_GAME_NEW_PLAYER:
						{
							// incorrect message, reject
							MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_HOST_NEW_GAME);
							mp_obj.send_stream(BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
						}
						break;
					case MPMSG_ACCEPT_NEW_PLAYER:
						hostPlayerId = from;
						if( regPlayerCount < MAX_NATION && ((MpStructAcceptNewPlayer *)recvPtr)->player_id != mp_obj.my_player_id )
						{
							// search if this player has existed
							for( p=0; p < regPlayerCount && regPlayerId[p] != ((MpStructAcceptNewPlayer *)recvPtr)->player_id; ++p );
							regPlayerId[p] = ((MpStructAcceptNewPlayer *)recvPtr)->player_id;
							playerReadyFlag[p] = 0;
							playerColor[p] = 0;
							if( p >= regPlayerCount )
							{
								err_when( p != regPlayerCount );
								regPlayerCount++;		// now regPlayerCount == p
							}
#ifdef MAX_GEM_STONES
							useGemStones[p] = 0;
							totalGemStones[p] = 0;
#endif
							mRefreshFlag |= MGOPTION_PLAYERS;
						}

						// ####### begin Gilbert 15/4 ##########//
						// put a message to the window
						if( ((MpStructAcceptNewPlayer *)recvPtr)->player_id == mp_obj.my_player_id )
						{
							MpStructChatMsg waitReplyMessage(NULL, text_game_menu.str_mp_host_ok() ); //"Ok" );
							messageList.linkin( &waitReplyMessage);
							mRefreshFlag |= MGOPTION_IN_MESSAGE;
						}
						// ####### end Gilbert 15/4 ##########//

						break;
					case MPMSG_ACQUIRE_RACE:
						if( remote.is_host )
						{
							short cl = ((MpStructAcquireRace *)recvPtr)->race_id;
							if( !cl )
							{
								for( cl = 1; cl <= MAX_RACE; ++cl)
								{
									if( raceAssigned_[cl] == 0 )
										break;
								}
							}
							for( int p = 0; p < regPlayerCount && regPlayerId[p] != from; ++p );
							if( cl <= MAX_RACE && p < regPlayerCount &&
								(shareRace || raceAssigned_[cl] == 0 ) )		// more than one player can use the same race
							{
								// unassign current race
								if( playerRace[p] )
									raceAssigned_[playerRace[p]]--;

								// mark race assigned
								raceAssigned_[cl]++;
								playerRace[p] = cl;

								// reply accept race
								MpStructAcceptRace msgAcceptRace(from, cl );
								mp_obj.send_stream( from, &msgAcceptRace, sizeof(msgAcceptRace) );
							}
							else
							{
								// reply refuse race
								MpStructRefuseRace msgRefuseRace(from, ((MpStructAcquireRace *)recvPtr)->race_id );
								mp_obj.send_stream( from, &msgRefuseRace, sizeof(msgRefuseRace) );
							}
						}
						break;
					case MPMSG_ACCEPT_RACE:
						if( ((MpStructAcceptRace *)recvPtr)->request_player_id == mp_obj.my_player_id )
						{
							tempConfig.race_id = char(((MpStructAcceptRace *)recvPtr)->race_id);
							refreshFlag |= SGOPTION_RACE | SGOPTION_DIFFICULTY;	// change species also change difficulty

						}
						break;
					case MPMSG_REFUSE_RACE:
						if( ((MpStructRefuseRace *)recvPtr)->request_player_id == mp_obj.my_player_id )
						{
							refreshFlag |= SGOPTION_RACE;
							// sound effect here
						}
						break;
					case MPMSG_ACQUIRE_COLOR:
						if( remote.is_host )
						{
							short cl = ((MpStructAcquireColor *)recvPtr)->color_scheme_id;
							if( !cl )
							{
								for( cl = 1; cl <= MAX_COLOR_SCHEME && colorAssigned[cl-1]; ++cl);
							}
							for( int p = 0; p < regPlayerCount && regPlayerId[p] != from; ++p );
							if( cl <= MAX_COLOR_SCHEME && !colorAssigned[cl-1] && p < regPlayerCount )
							{
								if( playerColor[p] > 0 )
									colorAssigned[playerColor[p]-1] = 0;

								// mark color assigned
								colorAssigned[cl-1] = 1;
								playerColor[p] = cl;

								// reply accept color
								MpStructAcceptColor msgAcceptColor(from, cl );
								mp_obj.send_stream( BROADCAST_PID, &msgAcceptColor, sizeof(msgAcceptColor) );

								mRefreshFlag |= MGOPTION_PLAYERS;
							}
							else
							{
								// reply refuse color
								MpStructRefuseColor msgRefuseColor(from, ((MpStructAcquireColor *)recvPtr)->color_scheme_id );
								mp_obj.send_stream( from, &msgRefuseColor, sizeof(msgRefuseColor) );
							}
						}
						break;
					case MPMSG_ACCEPT_COLOR:
						if( ((MpStructAcceptColor *)recvPtr)->request_player_id == mp_obj.my_player_id )
						{
							tempConfig.player_nation_color = char(((MpStructAcceptColor *)recvPtr)->color_scheme_id);
							refreshFlag |= SGOPTION_COLOR;
						}
						{	// other player change color
							err_when( remote.is_host );
							for( int p = 0; p < regPlayerCount && regPlayerId[p] != ((MpStructAcceptColor *)recvPtr)->request_player_id; ++p );
							short cl = ((MpStructAcceptColor *)recvPtr)->color_scheme_id;
							if( cl <= MAX_COLOR_SCHEME && p < regPlayerCount )
							{
								if( playerColor[p] > 0 )
									colorAssigned[playerColor[p]-1] = 0;

								// mark color assigned
								colorAssigned[cl-1] = 1;
								playerColor[p] = cl;
							}
							mRefreshFlag |= MGOPTION_PLAYERS;
						}
						break;
					case MPMSG_REFUSE_COLOR:
						if( ((MpStructRefuseColor *)recvPtr)->request_player_id == mp_obj.my_player_id )
						{
							refreshFlag |= SGOPTION_COLOR;
							// sound effect here
						}
						break;
					case MPMSG_PLAYER_READY:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructPlayerReady *)recvPtr)->player_id )
								{
									playerReadyFlag[p] = 1;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
						}
						break;
					case MPMSG_PLAYER_UNREADY:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructPlayerUnready *)recvPtr)->player_id )
								{
									playerReadyFlag[p] = 0;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
						}
						break;
#ifdef MAX_GEM_STONES
					case MPMSG_SET_GEM_STONE:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructSetGemStones *)recvPtr)->player_id )
								{
									useGemStones[p] = ((MpStructSetGemStones *)recvPtr)->use_gem_stones;
									totalGemStones[p] = ((MpStructSetGemStones *)recvPtr)->total_gem_stones;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
							if( !remote.is_host && !useGemStoneFlag)
							{
								useGemStoneFlag = 1;		// enable useGemStoneFlag
								// must use one gem stone if has any
								selfUseGemStones = player_profile.gem_stones > 0 ? 1 : 0;

								// tell other player how many gemstone to use
								MpStructSetGemStones msgSetGemStones( mp_obj.my_player_id, selfUseGemStones, player_profile.gem_stones );
								mp_obj.send_stream(BROADCAST_PID, &msgSetGemStones, sizeof(msgSetGemStones));

								// update himself
								for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
								if( p < regPlayerCount )
								{
									useGemStones[p] = selfUseGemStones;
									totalGemStones[p] = player_profile.gem_stones;
								}

								mRefreshFlag |= MGOPTION_GEM_STONES | MGOPTION_PLAYERS;
							}
						}
						break;
#endif
					case MPMSG_SEND_CHAT_MSG:
						messageList.linkin(recvPtr);
						mRefreshFlag |= MGOPTION_IN_MESSAGE;
						break;
					// ###### begin Gilbert 11/3 ########//
					case MPMSG_SET_PROCESS_FRAME_DELAY:
						{
							for( frameDelayGroupSetting = 2; frameDelayGroupSetting > 0; --frameDelayGroupSetting )
							{
								if( ((MpStructProcessFrameDelay *)recvPtr)->common_process_frame_delay >= frameDelaySettingArray[frameDelayGroupSetting] )
									break;
							}
							mRefreshFlag |= MGOPTION_FRAME_DELAY;
						}
						break;
					// ###### end Gilbert 11/3 ########//
					case MPMSG_SEND_SYNC_TEST_LEVEL:
						remote.sync_test_level = ((MpStructSyncLevel *)recvPtr)->sync_test_level;
						break;
					case MPMSG_END_SETTING:
						++recvEndSetting;
						break;
					case MPMSG_REFUSE_NEW_PLAYER:
						if( ((MpStructRefuseNewPlayer *)recvPtr)->player_id == mp_obj.my_player_id )
						{
							box.msg(((MpStructRefuseNewPlayer *)recvPtr)->reason_str());
							return 0;
						}
						break;
					default:		// if the game is started, any other thing is received
						return 0;
					}
				}
			}

			// ------ detect species option ---------//

			if( optionMode == OPTION_SPECIES )
			{
				if( speciesGroup.detect() >= 0 
					|| speciesGroup[speciesGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
					mRefreshFlag = MGOPTION_ALL;
				}
				else if( mouse.single_click( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, 368-5) )
				{
					speciesGroup.push( 0, 0 );		// switch to human mode
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
					mRefreshFlag = MGOPTION_ALL;
				}
				else if( mouse.single_click( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, 368-5) )
				{
					speciesGroup.push( 1, 0 );		// switch to fryhtan mode
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
					mRefreshFlag = MGOPTION_ALL;
				}
				else if( remote.is_host && frameDelayGroup.detect() >= 0 )
				{
					frameDelayGroupSetting = frameDelayGroup();
					mRefreshFlag = MGOPTION_FRAME_DELAY;

					// tell other player
					MpStructProcessFrameDelay msgframeDelay(frameDelaySettingArray[frameDelayGroupSetting]);
					mp_obj.send_stream( BROADCAST_PID, &msgframeDelay, sizeof(msgframeDelay) );
				}
#ifdef MAX_GEM_STONES
				else if( useGemStoneFlag && !selfReadyFlag
					&& gemStoneGroup.detect() >= 0 )
				{
					selfUseGemStones = gemStoneGroup();

					MpStructSetGemStones msgSetGemStones( mp_obj.my_player_id, selfUseGemStones, player_profile.gem_stones );
					mp_obj.send_stream(BROADCAST_PID, &msgSetGemStones, sizeof(msgSetGemStones));

					// update itself
					for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
					if( p < regPlayerCount )
					{
						useGemStones[p] = selfUseGemStones;
						totalGemStones[p] = player_profile.gem_stones;
					}
					mRefreshFlag |= MGOPTION_GEM_STONES | MGOPTION_PLAYERS;
				}
#endif
			}

			// ------ detect basic option --------//

			else if( optionMode == OPTION_BASIC )
			{
				if( !selfReadyFlag )
				{
					if( speciesGroup() == 0 && raceGroup.detect() >= 0
						|| speciesGroup() == 1 && monsterRaceGroup.detect() >= 0 )
					{
						int r = 0;
						if( speciesGroup() == 0 )
							r = raceGroup[raceGroup()].custom_para.value;
						else if( speciesGroup() == 1 )
							r = monsterRaceGroup[monsterRaceGroup()].custom_para.value;
						// tempConfig.race_id = r;		// BUGHERE : necessary ?

						if( remote.is_host )
						{
							for( int p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id ; ++p );
							if( r <= MAX_RACE && r >= -MAX_MONSTER_TYPE && p < regPlayerCount &&
								(shareRace || raceAssigned_[r] == 0 ) )		// more than one player can use the same race
							{
								// unassign current race
								if( playerRace[p])
									raceAssigned_[playerRace[p]]--;

								// mark race assigned
								raceAssigned_[r]++;
								playerRace[p] = r;

								tempConfig.race_id = r;
							}
							refreshFlag |= SGOPTION_RACE | SGOPTION_DIFFICULTY;	// change species also change difficulty
						}
						else
						{
							MpStructAcquireRace msgAcquire(r);
							mp_obj.send_stream(hostPlayerId, &msgAcquire, sizeof( msgAcquire) );
						}
						refreshFlag |= SGOPTION_RACE;
					}
					else if( colorGroup.detect() >= 0)
					{
						int r = colorGroup[colorGroup()].custom_para.value;
						if( remote.is_host )
						{
							if( !colorAssigned[r-1] )
							{
								for( int p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p );
								if( r <= MAX_COLOR_SCHEME && !colorAssigned[r-1] && p < regPlayerCount )
								{
									// unmark current color
									if( playerColor[p] > 0 )
										colorAssigned[playerColor[p]-1] = 0;
									// mark color assigned
									colorAssigned[r-1] = 1;
									playerColor[p] = r;
									tempConfig.player_nation_color = r;

									// inform other player host has changed color
									MpStructAcceptColor msgAcceptColor(mp_obj.my_player_id, r);
									mp_obj.send_stream( BROADCAST_PID, &msgAcceptColor, sizeof(msgAcceptColor) );
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
							refreshFlag |= SGOPTION_COLOR;
						}
						else
						{
							MpStructAcquireColor msgAcquire(r);
							mp_obj.send_stream(hostPlayerId, &msgAcquire, sizeof( msgAcquire) );
						}
						//refreshFlag |= SGOPTION_COLOR;
					}
				}
			}

			// -------- detect other option, only host can change ---------//

			unsigned keyCode = 0;

			if( remote.is_host && !selfReadyFlag)
			{
				int configChange = 0;

				// ------ detect basic option ----------//

				if( optionMode == OPTION_BASIC )
				{
					if( aiHumanNationGroup.detect() >= 0 )
					{
						int newHumanNationCount = aiHumanNationGroup[aiHumanNationGroup()].custom_para.value;
						tempConfig.ai_human_nation_count = newHumanNationCount;

						if( newHumanNationCount > 0 )
						{
							// 1 player + ai human + ai monster <= MAX_NATION
							if( 1 + tempConfig.ai_human_nation_count + tempConfig.ai_monster_nation_count > MAX_NATION )
								tempConfig.ai_monster_nation_count = MAX_NATION - 1 - tempConfig.ai_human_nation_count;
						}
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
					}
					else if( aiMonsterNationGroup.detect() >= 0 )
					{
						int newMonsterNationCount = aiMonsterNationGroup[aiMonsterNationGroup()].custom_para.value;
						tempConfig.ai_monster_nation_count = newMonsterNationCount;

						if( newMonsterNationCount > 0 )
						{
							// 1 player + ai human + ai monster <= MAX_NATION
							if( 1 + tempConfig.ai_human_nation_count + tempConfig.ai_monster_nation_count > MAX_NATION )
								tempConfig.ai_human_nation_count = MAX_NATION - 1 - tempConfig.ai_monster_nation_count;
						}
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
					}
					else if( diffGroup.detect() >= 0)
					{
						if( diffGroup[diffGroup()].custom_para.value != OPTION_CUSTOM )
						{
							tempConfig.change_difficulty(diffGroup[diffGroup()].custom_para.value);
							configChange = 1;
							// all but SGOPTION_PAGE;
							// refreshFlag |= SGOPTION_ALL_OPTIONS;
							refreshFlag |= SGOPTION_ALL & ~SGOPTION_PAGE;
						}
						else
						{
							tempConfig.difficulty_level = OPTION_CUSTOM;
							// refreshFlag |= SGOPTION_DIFFICULTY;
						}
					}
#ifndef ONLY_TERRAIN_SET_1
					else if( terrainGroup.detect() >= 0)
					{
						tempConfig.terrain_set = terrainGroup[terrainGroup()].custom_para.value;
						static short latitudeArray[3] = { 45, 20, 70 };
						err_when( tempConfig.terrain_set <= 0 || tempConfig.terrain_set > 3 );
						tempConfig.latitude = latitudeArray[tempConfig.terrain_set-1];
						configChange = 1;
						//refreshFlag |= SGOPTION_TERRAIN;
					}
#endif
#ifndef ONLY_SMALL_BUILDING
					else if( buildingSizeGroup.detect() >= 0)
					{
						tempConfig.building_size = buildingSizeGroup[buildingSizeGroup()].custom_para.value;
						configChange = 1;
						//refreshFlag |= SGOPTION_BUILDING_SIZE;
					}
#endif
				}

				// -------- detect advanced option ---------//

				else if( optionMode == OPTION_ADVANCED )
				{
					if( exploreGroup.detect() >= 0 )
					{
						tempConfig.explore_whole_map = exploreGroup[exploreGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_EXPLORED;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( fogGroup.detect() >= 0 )
					{
						tempConfig.fog_of_war = fogGroup[fogGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_FOG
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( treasureGroup.detect() >= 0 )
					{
						tempConfig.start_up_cash = treasureGroup[treasureGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_TREASURE;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( aiTreasureGroup.detect() >= 0 )
					{
						tempConfig.ai_start_up_cash = aiTreasureGroup[aiTreasureGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_AI_TREASURE;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( aiAggressiveGroup.detect() >= 0 )
					{
						tempConfig.ai_aggressiveness = 
							aiAggressiveGroup[aiAggressiveGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_AI_AGGRESSIVE;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( spyMethodGroup.detect() >= 0 )
					{
						tempConfig.spy_methodology = spyMethodGroup[spyMethodGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_SPY_METHOD;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( randomStartUpGroup.detect() >= 0)
					{
						tempConfig.random_start_up = randomStartUpGroup[randomStartUpGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_RANDOM_STARTUP;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
				}

				// -------- detect advanced option ---------//

				else if( optionMode == OPTION_ADVANCE2 )
				{
					if( rawSiteGroup.detect() >= 0 )
					{
						tempConfig.start_up_raw_site = rawSiteGroup[rawSiteGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_RAW;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( nearRawGroup.detect() >= 0)
					{
						tempConfig.start_up_has_mine_nearby = nearRawGroup[nearRawGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_NEAR_RAW;
						refreshFlag |= SGOPTION_DIFFICULTY;

					}
					else if( townStartGroup.detect() >= 0)
					{
						tempConfig.start_up_independent_town = townStartGroup[townStartGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// resfreshFlag |= SGOPTION_START_TOWN;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( townResistGroup.detect() >= 0)
					{
						tempConfig.independent_town_resistance = townResistGroup[townResistGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// resfreshFlag |= SGOPTION_TOWN_RESIST;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( townEmergeGroup.detect() >= 0)
					{
						tempConfig.new_independent_town_emerge = townEmergeGroup[townEmergeGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_TOWN_EMERGE;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( nationEmergeGroup.detect() >= 0)
					{
						tempConfig.new_nation_emerge = nationEmergeGroup[nationEmergeGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_NATION_EMERGE;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
					else if( randomEventGroup.detect() >= 0)
					{
						tempConfig.random_event_frequency = randomEventGroup[randomEventGroup()].custom_para.value;
						tempConfig.difficulty_level = OPTION_CUSTOM;
						configChange = 1;
						// refreshFlag |= SGOPTION_RANDOM_EVENT;
						refreshFlag |= SGOPTION_DIFFICULTY;
					}
				}

				// -------- detect goal option ----------//

				else if( optionMode == OPTION_GOAL )
				{
					if( clearEnemyButton.detect() )
					{
					}
					else if( clearMonsterButton.detect() )
					{
						tempConfig.goal_destroy_monster = clearMonsterButton.pushed_flag;
						configChange = 1;
					}
					// ######## begin Gilbert 11/3 #########//
					else if( allyWinButton.detect() )
					{
						tempConfig.goal_alliance_win_flag = allyWinButton.pushed_flag;
						configChange = 1;
					}
					// ######## end Gilbert 11/3 #########//
					else if( enoughPeopleButton.detect() )
					{
						tempConfig.goal_population_flag = enoughPeopleButton.pushed_flag;
						configChange = 1;
					}
					else if( enoughIncomeButton.detect() )
					{
						tempConfig.goal_economic_score_flag = enoughIncomeButton.pushed_flag;
						configChange = 1;
					}
					else if( enoughScoreButton.detect() )
					{
						tempConfig.goal_total_score_flag = enoughScoreButton.pushed_flag;
						configChange = 1;
					}
					else if( timeLimitButton.detect() )
					{
						tempConfig.goal_year_limit_flag = timeLimitButton.pushed_flag;
						configChange = 1;
					}
					else if( peopleInc.detect() )
					{
						tempConfig.goal_population += 100;
						if( tempConfig.goal_population > 5000 )
							tempConfig.goal_population = 5000;
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
					}
					else if( peopleDec.detect() )
					{
						tempConfig.goal_population -= 100;
						if( tempConfig.goal_population < 100 )
							tempConfig.goal_population = 100;
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
					}
					else if( incomeInc.detect() )
					{
						tempConfig.goal_economic_score += 100;
						if( tempConfig.goal_economic_score > 5000 )
						{
							tempConfig.goal_economic_score = 5000;
						}
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_INCOME;
					}
					else if( incomeDec.detect() )
					{
						tempConfig.goal_economic_score -= 100;
						if( tempConfig.goal_economic_score < 100 )
						{
							tempConfig.goal_economic_score = 100;
						}
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_INCOME;
					}
					else if( scoreInc.detect() )
					{
						if( tempConfig.goal_total_score >= 2000 )
							tempConfig.goal_total_score += 500;
						else
							tempConfig.goal_total_score += 100;
						if( tempConfig.goal_total_score > 10000 )
							tempConfig.goal_total_score = 10000;
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_SCORE;
					}
					else if( scoreDec.detect() )
					{
						if( tempConfig.goal_total_score > 2000 )
							tempConfig.goal_total_score -= 500;
						else
							tempConfig.goal_total_score -= 100;
						if( tempConfig.goal_total_score < 100 )
							tempConfig.goal_total_score = 100;
						configChange = 1;
						refreshFlag |= SGOPTION_ENOUGH_SCORE;
					}
					else if( yearInc.detect() )
					{
						if( tempConfig.goal_year_limit >= 20 )
							tempConfig.goal_year_limit += 5;
						else
							tempConfig.goal_year_limit++;
						if( tempConfig.goal_year_limit > 100 )
						{
							tempConfig.goal_year_limit = 100;
						}
						configChange = 1;
						refreshFlag |= SGOPTION_TIME_LIMIT;
					}
					else if( yearDec.detect() )
					{
						if( tempConfig.goal_year_limit > 20 )
							tempConfig.goal_year_limit -= 5;
						else
							tempConfig.goal_year_limit--;
						if( tempConfig.goal_year_limit < 1 )
						{
							tempConfig.goal_year_limit = 1;
						}
						configChange = 1;
						refreshFlag |= SGOPTION_TIME_LIMIT;
					}
				}

				if( configChange )
				{
					MpStructConfig msgConfig(tempConfig);
					mp_obj.send_stream( BROADCAST_PID, &msgConfig, sizeof(msgConfig) );
				}
			}

			// -------- detect switch option button ---------//

			for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
			{
				if( mouse.single_click( OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
					OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i) )
				{
					optionMode = i;
					refreshFlag = SGOPTION_ALL;
					mRefreshFlag = MGOPTION_ALL;
				}
			}

			// ######## begin Gilbert 6/7 #########//
			if( mouse.any_click(CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2) )
			{
				if( optionMode != OPTION_CHAT_LOG )
				{
					backupOptionMode = optionMode;
					optionMode = OPTION_CHAT_LOG;
				}
				else
				{
					err_when( backupOptionMode < OPTION_SPECIES || backupOptionMode > OPTION_GOAL );
					optionMode = backupOptionMode;
				}
				refreshFlag = SGOPTION_ALL;
				mRefreshFlag = MGOPTION_ALL;
			}
			// ######## end Gilbert 6/7 #########//

			// --------- detect ready button button --------//

			// ########## begin Gilbert 7/4 ##########//
			// --------- or click at the tick near name ---------//

			for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
			if( p < regPlayerCount 
				&& (mouse.any_click(BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2)
				|| mouse.any_click( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1)) )
			{
				mRefreshFlag |= MGOPTION_PLAYERS;
				if( !selfReadyFlag ) 
				{
					playerReadyFlag[p] = selfReadyFlag = 1;
					MpStructPlayerReady msgReady(mp_obj.my_player_id);
					mp_obj.send_stream(BROADCAST_PID, &msgReady, sizeof(msgReady));
				}
				else
				{
					// else un-ready this player
					playerReadyFlag[p] = selfReadyFlag = 0;
					MpStructPlayerUnready msgUnready(mp_obj.my_player_id);
					mp_obj.send_stream(BROADCAST_PID, &msgUnready, sizeof(msgUnready));
				}
			}
			// ########## end Gilbert 7/4 ##########//

			// ------ detect start button --------//

			if( remote.is_host && mouse.single_click( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2 ) && regPlayerCount >= 2 )
			{
				// see if all player is ready
				short sumBalance = 0;
				for( int q = 0; q < regPlayerCount && playerReadyFlag[q]; ++q)
				{
					err_when( playerBalance[q] == 0 );
					sumBalance += playerBalance[q];
				}
				if( q >= regPlayerCount )		// not all playerReadyFlag[p] = 1;
				{
#if( defined(DEBUG) || defined(BETA) || defined(DEMO) )
					sumBalance = 0;
#endif
					if( sumBalance >= 0 )
					{
//						MpStructBase msgStart(MPMSG_START_GAME);
//						mp_obj.send_stream(BROADCAST_PID, &msgStart, sizeof(msgStart));
						retFlag = 1;
						break;							// break while(1)
					}
					else
					{
						// too many non-CD version
						box.msg( text_game_menu.str_mp_cd_count() );
					}
				}
			}

			// -------- detect return button ---------//

			else if( mouse.single_click(BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2) )
			{
				if( box.ask(text_game_menu.str_mp_confirm_quit()) )
				{
					if( remote.is_host )
					{
						MpStructBase msgAbort(MPMSG_ABORT_GAME);
						mp_obj.send_stream(BROADCAST_PID, &msgAbort, sizeof(msgAbort) );
					}
					retFlag = 0;
					break;			// break while(1)
				}
			}
			else if( !keyCode && (keyCode = messageField.detect()) != 0)		// keyCode may be non-zero if after mapIdField.detect()
			{
				mRefreshFlag |= MGOPTION_OUT_MESSAGE;
				if(keyCode == KEY_RETURN && strlen(typingMsg.content) > 0)
				{
					// send message
					mp_obj.send_stream(BROADCAST_PID, &typingMsg, sizeof(typingMsg) );

					// add to own list
					messageList.linkin(&typingMsg);
					mRefreshFlag |= MGOPTION_IN_MESSAGE;

					// clear the string
					messageField.clear();
				}
				else if( keyCode == KEY_ESC )
				{
					messageField.clear();
				}
			}

			mp_obj.after_send();
		}	// end while
	} // end of scope of VgaLock

	// ---------- final setup to start multiplayer game --------//

	if( retFlag > 0)
	{
		retFlag = 0;

		if( remote.is_host )
			mp_obj.disable_join_session();

		// mp_obj.poll_players();
		nation_array.init();
		nation_array.zap();
			
		int trial;
		unsigned long startTime;
		int playerCount = 0;

		if( remote.is_host )
		{
			VLenQueue setupString;

			// ------- put start game string -------//

			{
				MpStructBase msgStart(MPMSG_START_GAME);
				memcpy( setupString.reserve(sizeof(msgStart)), &msgStart, sizeof(msgStart) );
			}

			// -------- put random seed -------- //
			do
			{
				info.init_random_seed( atol(msgSeedStr.seed_str) );
			} while (m.get_random_seed() == 0L);
			{
				MpStructSeed msgSeed(m.get_random_seed());
				memcpy( setupString.reserve(sizeof(msgSeed)), &msgSeed, sizeof(msgSeed) );
			}

			// -------- send config ------------//
			{
				MpStructConfig msgConfig(tempConfig);
				memcpy( setupString.reserve(sizeof(msgConfig)), &msgConfig, sizeof(msgConfig) );
			}

			// BUGHERE : only terrain_set 1 is available
			// tempConfig.terrain_set = 1;

			config = tempConfig;		// nation_array.new_nation reads setting from config

			// -------- setup nation now ------------ //

			info.init(INFO_DEFAULT_YEAR, INFO_DEFAULT_MONTH, INFO_DEFAULT_DAY);		// init game_date to make nation_array.new_nation successful

			playerCount = 0;

			for( p = 0; p < regPlayerCount; ++p )
			{
				// ensure it is a valid player
				PID_TYPE playerId = regPlayerId[p];
				PlayerDesc *player = mp_obj.search_player(playerId);
				if( !playerId || !player || !player->connecting )
					continue;

				nationPara[playerCount].init(playerCount+1, playerId, playerColor[p], playerRace[p], player->friendly_name_str()
#ifdef MAX_GEM_STONES
					,useGemStones[p]
#else
					,0
#endif
#ifdef APPLY_RANKING
					, playerRanking[p]
#else
					, 0
#endif
					);
				((MpStructNation *)setupString.reserve(sizeof(MpStructNation)))->init(
					playerCount+1, playerId, playerColor[p], playerRace[p], player->friendly_name_str(),
					nationPara[playerCount].use_gem_stones, nationPara[playerCount].ranking );
				playerCount++;
			}

			*mpPlayerCount = playerCount;
			config.human_difficulty_rating = config.multi_player_difficulty(playerCount-1, 1);
			config.monster_difficulty_rating = config.multi_player_difficulty(playerCount-1, -1);

			// ---- force set to the lowest frame delay -------//

			// ###### begin Gilbert 11/3 ########//
#ifdef FORCE_MAX_FRAME_DELAY
			remote.set_process_frame_delay(FORCE_MAX_FRAME_DELAY);
#else
			remote.set_process_frame_delay(frameDelaySettingArray[frameDelayGroupSetting]);
#endif
			// ###### end Gilbert 11/3 ########//
			{
				MpStructProcessFrameDelay msgFrameDelay(remote.get_process_frame_delay());
				memcpy( setupString.reserve(sizeof(msgFrameDelay)), &msgFrameDelay, sizeof(msgFrameDelay));
			}

			// -------- send sync test level ----------//

			{
				MpStructSyncLevel msgSyncTest(remote.sync_test_level);
				memcpy( setupString.reserve(sizeof(msgSyncTest)), &msgSyncTest, sizeof(msgSyncTest));
			}

			mp_obj.send_stream(BROADCAST_PID, setupString.queue_buf, setupString.length() );
			mp_obj.after_send();
		}
		else
		{
			// use the message recving MPMSG_START_GAME

			err_when( !recvPtr);

			DWORD offset = 0;
			int recvStartMsg = 0;
			int recvSeed = 0;
			int recvConfig = 0;
			int ownPlayerFound = 0;
			playerCount = 0;
			char *oriRecvPtr = recvPtr;
			int recvSetFrameDelay = 0;
			int recvSyncTestLevel = 0;

			// process the string received
			while( offset < recvLen )
			{
				DWORD oldOffset = offset;
				recvPtr = oriRecvPtr + offset;

				switch( ((MpStructBase *)(recvPtr))->msg_id )
				{
				case MPMSG_START_GAME:
					recvStartMsg = 1;
					offset += sizeof( MpStructBase );
					break;

				case MPMSG_RANDOM_SEED:
					err_when( ((MpStructSeed *) recvPtr)->seed == 0L);
					info.init_random_seed( ((MpStructSeed *) recvPtr)->seed );
					offset += sizeof( MpStructSeed );
					++recvSeed;
					break;

				case MPMSG_SEND_CONFIG:
					// ###### patch begin Gilbert 14/1 ######//
					if( ((MpStructConfig *)recvPtr)->game_config.validate() )
					{
						tempConfig.change_game_setting( ((MpStructConfig *) recvPtr)->game_config );
						++recvConfig;
						config = tempConfig;		// nation_array.new_nation reads setting from config
					}
					offset += sizeof( MpStructConfig );
					// ###### patch end Gilbert 14/1 ######//
					break;

				case MPMSG_DECLARE_NATION:
					{
						if( playerCount == 0 )
							info.init(INFO_DEFAULT_YEAR, INFO_DEFAULT_MONTH, INFO_DEFAULT_DAY);			// info.init for the first time

						MpStructNation *msgNation = (MpStructNation *)recvPtr;
						nationPara[playerCount].init( msgNation->nation_recno,
							msgNation->dp_player_id, msgNation->color_scheme, 
							msgNation->race_id, msgNation->player_name, 
							msgNation->use_gem_stones, msgNation->ranking );

						if( msgNation->dp_player_id == mp_obj.my_player_id )
							ownPlayerFound++;
						*mpPlayerCount = ++playerCount;
						offset += sizeof( MpStructNation );
						config.human_difficulty_rating = config.multi_player_difficulty(playerCount-1,1);
						config.monster_difficulty_rating = config.multi_player_difficulty(playerCount-1,-1);
					}
					break;

				case MPMSG_SET_PROCESS_FRAME_DELAY:
					remote.set_process_frame_delay(((MpStructProcessFrameDelay *)recvPtr)->common_process_frame_delay);
					++recvSetFrameDelay;
					offset += sizeof( MpStructProcessFrameDelay );
					break;

				case MPMSG_SEND_SYNC_TEST_LEVEL:
					remote.sync_test_level = ((MpStructSyncLevel *)recvPtr)->sync_test_level;
					++recvSyncTestLevel;
					offset += sizeof( MpStructSyncLevel );
					break;
				}  // end switch

				if( !recvStartMsg || offset <= oldOffset )
				{
					err_here();
					box.msg( text_game_menu.str_mp_connect_str_corrupt() ); // "Connection string from host is corrupted" );
					return -1;
				}
			} // end while

			if( !recvSeed )
			{
				box.msg( text_game_menu.str_mp_seed_lost() ); // "Cannot get random seeds from the host." );
				return -1;
			}
			err_when( recvSeed > 1 );
			if( !recvConfig )
			{
				box.msg( text_game_menu.str_mp_config_lost() ); // "Cannot get game configuration info from the host." );
				// or the config from host fail in validation
				return -1;
			}
			err_when( recvConfig > 1 );
			if( playerCount == 0 )
			{
				box.msg( text_game_menu.str_mp_nation_info_lost() ); //"Cannot get kingdom information from the host." );
				return -1;
			}
			err_when( playerCount > MAX_NATION );
			if( !ownPlayerFound )
			{
				box.msg( text_game_menu.str_mp_host_not_recognize() ); // "The host cannot recognize your machine." );
				return -1;
			}
			err_when( ownPlayerFound > 1 );
			if( !recvSetFrameDelay || !recvSyncTestLevel )
			{
				box.msg( text_game_menu.str_mp_miss_connect_info() ); //"Cannot receive important information from the host");
				return -1;
			}
			err_when( recvSetFrameDelay > 1 );
			err_when( recvSyncTestLevel > 1 );
		}

		if( remote.sync_test_level == 0)
		{
			remote.set_alternating_send(playerCount > 4 ? 4 : playerCount);		// automatic setting
		}

		{
			// ------- broadcast end setting string ------- //

			MpStructBase mpEndSetting(MPMSG_END_SETTING);
			mp_obj.send_stream( BROADCAST_PID, &mpEndSetting, sizeof(mpEndSetting) );
			mp_obj.after_send();

			// ------ wait for MPMSG_END_SETTING ----------//
			// ---- to filter other all message until MP_MSG_END_SETTING ---//

			trial = 5000;
			startTime = m.get_time();
			// int recvEndSetting = 0;
			while( --trial > 0 || m.get_time() - startTime < 10000 )
			{
				if( recvEndSetting >= playerCount-1)
					break;
				mp_obj.before_receive();
				recvPtr = mp_obj.receive( &from, &to, &recvLen);
				mp_obj.after_send();
				if( recvPtr )
				{
					trial = max(trial, 1000);
					startTime = m.get_time();
					if( ((MpStructBase *)recvPtr)->msg_id == MPMSG_END_SETTING )
					{
						recvEndSetting++;
					}
				}
			}
			if( recvEndSetting < playerCount-1 )
			{
				box.msg( text_game_menu.str_mp_other_player_fail() ); // "Some player(s) encountered errors when initializing the game.");
				// but continue
			}
		}

		retFlag = 1;
	}		// end if(retFlag)

	return retFlag;
}
//-------- End of function Game::mp_select_option -----------//


#undef MAX_GEM_STONES
// no gem stone in load game
#undef APPLY_RANKING
// no handle ranking in load game

//-------- Begin of function Game::mp_select_load_option -----------//
// return 0 = cancel, 1 = ok
int Game::mp_select_load_option(char *fileName)
{
	char optionMode = OPTION_SPECIES;
	char backupOptionMode;		// when switch to OPTION_CHAT_LOG, backup the current optionMode

	const int SHEET_X1 = 95;
	const int SHEET_Y1 = 74;
	const int SHEET_X2 = 703;
	const int SHEET_Y2 = 439;

	Config &tempConfig = config;

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init
	TempMonsterRes tempMonsterRes;			// race_res.init

	PID_TYPE from, to;
	DWORD recvLen;
	int sysMsgCount;
	char *recvPtr;

	char raceAssignedBiased[MAX_MONSTER_TYPE+1+MAX_RACE];		// master copy belongs to host's
	const int raceBias = MAX_MONSTER_TYPE;			// access raceAssignedBiased[raceBias + raceId]
	char *raceAssigned_ = raceAssignedBiased + MAX_MONSTER_TYPE;		// change back to raceAssigned later
	char colorAssigned[MAX_COLOR_SCHEME];		// master copy belongs to host's

	DynArray messageList(sizeof(MpStructChatMsg));
	MpStructChatMsg typingMsg(tempConfig.player_name, NULL);
	
	memset( raceAssignedBiased, 0, sizeof(raceAssignedBiased) );
	memset( colorAssigned, 0, sizeof(colorAssigned) );

	PID_TYPE hostPlayerId = BROADCAST_PID;
	PID_TYPE regPlayerId[MAX_NATION];
	memset( regPlayerId, 0, sizeof(regPlayerId) );
	char playerReadyFlag[MAX_NATION];
	memset( playerReadyFlag, 0, sizeof(playerReadyFlag) );
	short playerRace[MAX_NATION];	// host only
	memset( playerRace, 0, sizeof(playerRace) );
	short playerColor[MAX_NATION];	// host only
	memset( playerColor, 0, sizeof(playerColor) );
	short playerBalance[MAX_NATION];
	memset( playerBalance, 0, sizeof(playerBalance) );
#ifdef MAX_GEM_STONES
	short useGemStones[MAX_NATION];
	memset( useGemStones, 0, sizeof(useGemStones) );
	long totalGemStones[MAX_NATION];
	memset( totalGemStones, 0, sizeof(totalGemStones) );
#endif
#ifdef APPLY_RANKING
	WORD playerRanking[MAX_NATION];
	memset( playerRanking, 0, sizeof(playerRanking) );
#endif

	int regPlayerCount = 0;
	int selfReadyFlag = 0;
	int maxPlayer;
	int shareRace = 1;		// host only, 0= exclusive race of each player
	int recvEndSetting = 0;			// number of MPMSG_END_SETTING received, it may come earlier

#ifdef MAX_GEM_STONES
	int selfUseGemStones = 0;
	int useGemStoneFlag = 0;
#endif

	mp_obj.poll_players();
	for( int p = 1; p <= MAX_NATION && mp_obj.get_player(p); ++p )
	{
		// host only identify himself
		if( !remote.is_host || mp_obj.get_player(p)->pid() == mp_obj.my_player_id )
		{
			regPlayerId[regPlayerCount] = mp_obj.get_player(p)->pid();
			playerReadyFlag[regPlayerCount] = 0;
			playerColor[regPlayerCount] = 0;
			playerRace[regPlayerCount] = 0;
			playerBalance[regPlayerCount] = 0;
#ifdef MAX_GEM_STONES
			useGemStones[regPlayerCount] = 0;
			totalGemStones[regPlayerCount] = 0;
#endif
#ifdef APPLY_RANKING
			playerRanking[regPlayerCount] = 0;
#endif
			++regPlayerCount;
		}
	}

#ifdef MAX_GEM_STONES
	if( remote.is_host )
	{
		useGemStoneFlag = remote.is_lobbied() &&  player_profile.mp_use_gem_stone;
		if( useGemStoneFlag )
		{
			// must use one gem stone if has any
			// selfUseGemStones = player_profile.gem_stones > 0 ? 1 : 0;
			// BUGHERE : how to get how each player put GemsStone at the beginning
		}
	}

	// non-host will set to useGemStoneFlag to true when receive
	if( player_profile.mp_use_gem_stone )
	{
		player_profile.reload();
		player_profile.mp_use_gem_stone = 0;		// clear 
		player_profile.save();
	}
#endif
#ifdef APPLY_RANKING
	if( remote.is_host )
	{
		// set ranking for the host
		for( p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
		if( p < regPlayerCount )
			playerRanking[p] = player_profile.ranking;
	}
#endif

//	err_when( tempConfig.race_id != (~nation_array)->race_id );
	err_when( tempConfig.player_nation_color != (~nation_array)->color_scheme_id );

	static int tickX[MAX_NATION] = { 132, 396, 132, 396, 132, 396, 132 };
	static int tickY[MAX_NATION] = {   6,   6,  22,  22,  38,  38,  54 };
	const int nameTickWidth = 13;
	const int nameTickHeight = 13;

	//--------- initialize variable ---------//

	int i;
	int w, h;
	int cx, cy;
	String str;
	long refreshFlag = SGOPTION_ALL;
	long mRefreshFlag = MGOPTION_ALL;
	int retFlag = 0;

	// randomly select a race
	if( remote.is_host )
	{
		colorAssigned[tempConfig.player_nation_color-1] = 1;
		for( p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p );
		if( p < regPlayerCount )
		{
			playerColor[p] = tempConfig.player_nation_color;
			playerBalance[p] = sys.cdrom_drive ? PLAYER_RATIO_CDROM : PLAYER_RATIO_NOCD;
		}

		// initialize other colorAssigned, only free for those of the remote players
		maxPlayer = 1;			// host
		for( p = 1; p <= nation_array.size(); ++p )
		{
			if( !nation_array.is_deleted(p) && nation_array[p]->is_remote() )
			{
				colorAssigned[nation_array[p]->color_scheme_id-1] = 0;
				maxPlayer++;
			}
		}
		err_when( maxPlayer > MAX_NATION );
	}
	else
	{
		memset( colorAssigned, 0, sizeof(colorAssigned) );		// assume all color are unassigned
		MpStructLoadGameNewPlayer msgNewPlayer( mp_obj.my_player_id, ~nation_array, sys.frame_count,
			m.get_random_seed(), sys.cdrom_drive ? PLAYER_RATIO_CDROM : PLAYER_RATIO_NOCD );
		mp_obj.send_stream(BROADCAST_PID, &msgNewPlayer, sizeof(msgNewPlayer) );

		// ####### begin Gilbert 15/4 ##########//
		// put a message to the window
		MpStructChatMsg waitReplyMessage(NULL, text_game_menu.str_mp_wait_host()); // "Waiting response from host..." );
		messageList.linkin( &waitReplyMessage);
		// ####### end Gilbert 15/4 ##########//
	}


	// -------- generate palette ------//

	short colorRemapTable[0x100];
	{
		str = DIR_IMAGE;
		str += "CHOOSE.COL";

		File palFile;
		palFile.file_open(str);
		ColorTable colorTable;

		BYTE palBuf[0x100][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	// ------- init message field --------//

	GetA messageField;
	messageField.init( 126+8, 528+4, 663-4, typingMsg.content,
		MpStructChatMsg::MSG_LENGTH, &font_san, 0, 1);

	// -------- initialize info.random_seed field ------//

	const int mapIdSize = 11;		// enough to hold a dword in decimal
	char mapIdStr[mapIdSize+1];
	// info.init_random_seed(0);
	ltoa( info.random_seed , mapIdStr, 10);
	// GetA mapIdField;

	MpStructSeedStr msgSeedStr(info.random_seed);
//	GetA mapIdField;
//	mapIdField.init( MAPID_X1, offsetY+83, 700, msgSeedStr.seed_str, msgSeedStr.RANDOM_SEED_MAX_LEN, &font_san, 0, 1);


	// ------ initialize human / fryhtan button ------//

	ButtonCustomGroup speciesGroup(2);
	
	w = font_thin_black.text_width( text_game_menu.str_species_humans() ); //  "Humans" 
	h = font_thin_black.text_height();
	cx = 256;
	cy = 116;
	//SXM -30
#if(defined(CHINESE))
	speciesGroup[0].create( cx-w/2-30, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );
#else
	speciesGroup[0].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );
#endif
	//SXM

	w = font_thin_black.text_width( text_game_menu.str_species_fryhtans() );
	h = font_thin_black.text_height();
	cx = 532;
	cy = 116;
#if(defined(CHINESE))
	speciesGroup[1].create( cx-w/2-10 -30, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );
#else
	speciesGroup[1].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );
#endif

	// load game only : set pushed button
	speciesGroup.push( tempConfig.race_id >= 0 ? 0 : 1, 0 );

	// ###### begin Gilbert 11/3 #########//
	// ----- initialize process frame delay group -------//

	char frameDelayGroupSetting = 2;		// default to high
	char frameDelaySettingArray[3] = { 2, 4, 5 };		// corresponding process frame delay value
	//char *frameDelayGroupStr[3] = { "Low", "Medium", "High" };
	ButtonCustomGroup frameDelayGroup(3);

	cx = 360;
	cy = 368;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_low_to_high(i+1) );
		h = font_thin_black.text_height();
		frameDelayGroup[i].create( cx, cy, cx+w+10, cy+20-1,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_low_to_high(i+1), i), 0 );
		cx += w+20;
	}
	// ###### end Gilbert 11/3 #########//

#ifdef MAX_GEM_STONES
	// -------- initialize gem stone group ------//

	ButtonCustomGroup gemStoneGroup(MAX_GEM_STONES+1);		// 0 - MAX_GEM_STONES
	cx = 360;
	cy = 393;
	for( i = 0; i <= MAX_GEM_STONES; ++i )
	{
		gemStoneGroup[i].create(cx, cy, cx+22, cy+20-1,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0);
		if( i > 0 && player_profile.gem_stones < i
			|| i == 0 && player_profile.gem_stones > 0 )
			gemStoneGroup[i].visible_flag = 0;
		cx += 25;
	}
#endif

	// -------- initialize player name field ------//

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// -------- initialize human group ---------//

	ButtonCustomGroup raceGroup(MAX_RACE);

	err_when( !race_res.init_flag );
	cx = 210;
	cy = 160;
	const int raceGroupYSpacing = 22;
	for( i = 0; i < MAX_RACE; ++i, (cy += raceGroupYSpacing) )
	{
		w = font_thin_black.text_width( race_res[i+1]->name );
		h = raceGroupYSpacing;
		raceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, i+1 ), 0 );
	}

	// --------- initialize monster group -------//

	ButtonCustomGroup monsterRaceGroup(MAX_MONSTER_TYPE);		// MAX_MONSTER
	err_when( !monster_res.init_flag );
	cx = 210;
	cy = 160;
	const int monsterGroupYSpacing = 22;
	for( i = 0; i < MAX_MONSTER_TYPE; ++i, (cy += monsterGroupYSpacing) )		// MAX_MONSTER
	{
		w = font_thin_black.text_width( monster_res[i+1]->name );
		h = monsterGroupYSpacing;
		monsterRaceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, -(i+1) ), 0 );
	}

	// ------- create color group --------//

	ButtonCustomGroup colorGroup(MAX_COLOR_SCHEME);
	const int colorButtonFrameX = 382;
	const int colorButtonFrameY = 155;
	cx = 391;
	cy = 162;
	const int colorButtonWidth = 29;
	const int colorButtonHeight = 30;
	for(i = 0; i < MAX_COLOR_SCHEME; ++i, (cx+=colorButtonWidth) )
	{
		colorGroup[i].create( cx, cy, cx+colorButtonWidth-1, cy+colorButtonHeight-1, 
			i_disp_color_button, ButtonCustomPara(NULL, i+1), 0 );
	}

	// -------- create ai human nation group -------//

	ButtonCustomGroup aiHumanNationGroup(MAX_NATION);
	cx = 361;
	cy = 258-20;
	const int nationButtonSpacing = 30;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiHumanNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiHumanNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// -------- create ai monster nation group -------//

	ButtonCustomGroup aiMonsterNationGroup(MAX_NATION);
	cx = 361;
	cy = 280-20;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiMonsterNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiMonsterNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// ---------- initialize difficulty_level button group -------//

	ButtonCustomGroup diffGroup(OPTION_HARDEST-OPTION_EASIEST+2);		// 0-7 + OPTION_CUSTOM
	char diffButtonStr[OPTION_HARDEST-OPTION_EASIEST+1][10];
	cx = 360;
	cy = 331-35;
	for( i = 0; i < OPTION_HARDEST-OPTION_EASIEST+1; ++i )
	{
		strcpy( diffButtonStr[i], m.roman_number(i+1) );
		w = font_thin_black.text_width(diffButtonStr[i]);
		h = font_thin_black.text_height();
		diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(diffButtonStr[i], i+OPTION_EASIEST), 0);
		cx += w + 12;
	}

	w = font_thin_black.text_width(text_game_menu.str_difficulty_custom());
	h = font_thin_black.text_height();
	diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara(text_game_menu.str_difficulty_custom(), OPTION_CUSTOM), 0);
	cx += w + 12;

	// --------- initialize terrain group --------//

	ButtonCustomGroup terrainGroup(2);
	// char *terrainButtonStr[3] = { "Fertile", "Badlands", "Arctic" };
	cx = 370+45;
	cy = 406-65;
	for( i = 0; i < terrainGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_terrain_set(i+1) );
		h = font_thin_black.text_height();
		terrainGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_terrain_set(i+1), i+1), 0 );
		cx += w + 12;
	}

	// --------- initialize building size --------//

	ButtonCustomGroup buildingSizeGroup(2);
	// char *buildingSizeButtonStr[2] = { "Large", "Small" };
	cx = 370+63;
	cy = 386;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_building_set(i+1) );
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_building_set(i+1), i+1), 0 );
		cx += w + 12;
	}

	// ------- initialize explored group ------//

	const int option3X = 320;
	ButtonCustomGroup exploreGroup(2);
	cx = option3X;
	cy = 99;
	w = font_thin_black.text_width( text_game_menu.str_world_map(0) );	// "Unexplored"
	h = font_thin_black.text_height();
	exploreGroup[0].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_world_map(1) );	// "Explored"
	exploreGroup[1].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(1), 1), 0 );

	// ------ initialize fog of war ------//

	ButtonCustomGroup fogGroup(2);
	cx = option3X;
	cy = 143;
	w = font_thin_black.text_width( text_game_menu.str_off_on(0) );	// "Off"
	h = font_thin_black.text_height();
	fogGroup[0].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_off_on(1) );
	fogGroup[1].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(1), 1), 0 );

	// --------- initialize start_up_cash/start_up_food button group -------//

//	char *treasureSizeStr[4] = { "Small", "Medium", "Large", "Huge" };

	ButtonCustomGroup treasureGroup(4);
	cx = option3X;
	cy = 183;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		treasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_start_up_cash/food button group -------//

	ButtonCustomGroup aiTreasureGroup(4);
	cx = option3X;
	cy = 235;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		aiTreasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_aggressiveness -------//

	// #### begin Gilbert 10/2 #########//
	char *aggressiveStr[5] = { "Very Low", "Low", "Medium", "Great", "Very great" };

	ButtonCustomGroup aiAggressiveGroup(5);
	cx = option3X;
	cy = 289;
	for( i = 0; i < 5; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_aggressiveness(i) );
		h = font_thin_black.text_height();
		aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		if( aiAggressiveGroup[i].x2 > SCROLL_SHEET_X2 )
		{	// line feed
			cy = aiAggressiveGroup[i].y2+1 ;
			cx = option3X;
			aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
				i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		}
		cx += w+24;
	}
	// #### end Gilbert 10/2 #########//

	// --------- initialize spy methodology-------//

	// char *spyMethodStr[2] = { "Must  Research", "All  Available" };

	ButtonCustomGroup spyMethodGroup(2);
	cx = option3X;
	cy = 348;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_need_research(i) );
		h = font_thin_black.text_height();
		spyMethodGroup[i].create( cx, cy, cx+w+10, cy+h+5,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_need_research(i), i), 0 );
		cx += w+24;
	}

	// --------- initialize random startup button group -------//

//	char *yesNoStr[4] = { "No", "Yes" };

	ButtonCustomGroup randomStartUpGroup(2);
	cx = option3X;
	cy = 389;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		randomStartUpGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	//  -------- initialize start_up_raw_site buttons --------- //

	const int option4X = 410;
	ButtonCustomGroup rawSiteGroup(7);
	cx = option4X;
	cy = 100;
	for( i = 0; i < 7; ++i )
	{
		rawSiteGroup[i].create(cx, cy, cx+15, cy+font_thin_black.height()+10,
			i_disp_int_button, ButtonCustomPara(NULL, i+1), 0);
		cx += 25;
	}

	// --------- initialize start_up_has_mine_nearby button group --------//

	ButtonCustomGroup nearRawGroup(2);
	cx = option4X;
	cy = 152;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nearRawGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	// --------- initialize start_up_independent_town button group --------//

	short startTownArray[3] = { 7, 15, 30 };
//	char *fewManyStr[3] = { "Few", "More", "Many" };

	ButtonCustomGroup townStartGroup(3);
	cx = option4X;
	cy = 206;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_few_to_many(i+1) );
		h = font_thin_black.text_height();
		townStartGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_few_to_many(i+1), startTownArray[i]), 0);
		cx += w+20;
	}

	// --------- initialize independent_town_resistance button group --------//

//	char *lowHighStr[3] = { "Low", "Med", "High" };

	ButtonCustomGroup townResistGroup(3);
	cx = option4X;
	cy = 246;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_low_to_high(i+1) );
		h = font_thin_black.text_height();
		townResistGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_low_to_high(i+1), i+1), 0);
		cx += w+20;
	}

	// --------- initialize new_independent_town_emerge button group --------//

	ButtonCustomGroup townEmergeGroup(2);
	cx = option4X;
	cy = 293;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		townEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize new_nation_emerge button group --------//

	ButtonCustomGroup nationEmergeGroup(2);
	cx = option4X;
	cy = 340;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nationEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize random_event_frequency button group --------//

//	char *randomEventStr[4] = { "Never", "Seldom", "Occasional", "Frequent" };

	ButtonCustomGroup randomEventGroup(4);
	cx = option4X;
	cy = 384;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_never_to_frequent(i) );
		h = font_thin_black.text_height();
		randomEventGroup[i].create(cx, cy, cx+w+10, cy+h+6,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_never_to_frequent(i), i), 0);
		if( (i+1) % 2 == 0 )
		{
			// line feed
			cx = option4X;
			cy += 26;
		}
		else
		{
			cx += 144;
		}
	}

	// ------- initialize goal button ---------- //

	// ###### begin Gilbert 11/3 #########//
	ButtonCustom clearEnemyButton, clearMonsterButton, allyWinButton, enoughPeopleButton, enoughIncomeButton, enoughScoreButton, timeLimitButton;
	Button peopleInc, peopleDec, incomeInc, incomeDec, scoreInc, scoreDec, yearInc, yearDec;

	const int option5X = 126;
	const int tickWidth = 20;
	const int tickHeight = 20;
	const int option5X2 = 620;
	const int option5X3 = 642;
	w = tickWidth;
	h = tickHeight;

	clearEnemyButton.create( option5X, 145, option5X+w-1, 145+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 1);
	clearEnemyButton.enable_flag = 0;
	clearMonsterButton.create( option5X, 178, option5X+w-1, 178+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_destroy_monster);
	allyWinButton.create( option5X, 211, option5X+w-1, 211+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_alliance_win_flag);
	enoughPeopleButton.create( option5X, 244, option5X+w-1, 244+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_population_flag);
	enoughIncomeButton.create( option5X, 277, option5X+w-1, 277+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_economic_score_flag);
	enoughScoreButton.create( option5X, 310, option5X+w-1, 310+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_total_score_flag);
	timeLimitButton.create( option5X, 343, option5X+w-1, 343+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_year_limit_flag);

	peopleInc.create_text( option5X2, 244, option5X2+w-1, 244+h-1, "+" );
	peopleDec.create_text( option5X3, 244, option5X3+w-1, 244+h-1, "-" );

	incomeInc.create_text( option5X2, 277, option5X2+w-1, 277+h-1, "+" );
	incomeDec.create_text( option5X3, 277, option5X3+w-1, 277+h-1, "-" );

	scoreInc.create_text( option5X2, 310, option5X2+w-1, 310+h-1, "+" );
	scoreDec.create_text( option5X3, 310, option5X3+w-1, 310+h-1, "-" );

	yearInc.create_text( option5X2, 343, option5X2+w-1, 343+h-1, "+" );
	yearDec.create_text( option5X3, 343, option5X3+w-1, 343+h-1, "-" );
	// ###### end Gilbert 11/3 #########//

	// ------- loop ---------//

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return 0;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			// ######### begin Gilbert 14/5 ########//
			else if( !sys.use_true_front && (sys.paused_flag || !sys.active_flag) )
			// ######### end Gilbert 14/5 ########//
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SGOPTION_ALL;
				mRefreshFlag = MGOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// -------- display ----------//
			// ######### begin Gilbert 14/5 ########//
			if( !sys.paused_flag && sys.active_flag && (refreshFlag || mRefreshFlag) )
			// ######### end Gilbert 14/5 ########//
			{
				if( optionMode == OPTION_SPECIES )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ###### begin Gilbert 29/6 #########//
						// ----- display picture -------//

						const int pictureWidth = 209; // 298;
						const int pictureHeight = 210; // 300;
						const int pictureXoffset = 35;
						const int pictureYoffset = 10;

						if( !vga.use_back_buf )
						{
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}
						else
						{
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}

						// "Network Latency"
						font_bold_black.put( 126, 368, text_game_menu.str_mp_latency(), 0, 360-10 );

						// display totol gem stones
						if( mRefreshFlag & MGOPTION_GEM_STONES && game.total_gem_stones )
						{
							// "Gem Stones (xxx)" );
							font_bold_black.put( 126, 393, 
								text_game_menu.str_mp_gem_stones(player_profile.gem_stones), 0, 360-10 );
							font_bold_red.put( 360, 393, game.total_gem_stones, 1, 460 );
						}				

						// ###### end Gilbert 29/6 #########//

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
						speciesGroup.paint( tempConfig.race_id < 0 );

					// ####### begin Gilbert 11/3 ######//
					if( mRefreshFlag & MGOPTION_FRAME_DELAY )
						frameDelayGroup.paint(frameDelayGroupSetting);
					// ####### end Gilbert 11/3 ######//

//#ifdef MAX_GEM_STONES
//					// display number gem stone to use
//					if( mRefreshFlag & MGOPTION_GEM_STONES && useGemStoneFlag )
//					{
//						font_bold_black.put_paragraph( 126, 408, 360-10, 458-1,
//							text_game_menu.str_mp_gem_stones(player_profile.gem_stones) ); // "Gem Stones (xxx)" );
//						gemStoneGroup.paint(selfUseGemStones);
//					}
//#endif
					// other ...
				}

				// ------- display basic option ---------//
				if( optionMode == OPTION_BASIC )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");
						// BUGHERE : option menu column and finger

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.right_put( playerNameField.x, playerNameField.y,
							text_game_menu.str_kingdom_of() ); // "Kingdom of " );

						if( speciesGroup() == 0 )
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_nationality()); // "Nationality");
						else if( speciesGroup() == 1 )
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_species()); // "Fryhtan Species");
						font_bold_black.center_put( 382, 129, 600, 149, text_game_menu.str_color()); // "Color" );
						font_bold_black.center_put( 297, 225-20, 698, 243-20, text_game_menu.str_ai_nation_count()); // "Computer Controlled Kingdoms" );
						font_bold_black.put( aiHumanNationGroup[MAX_NATION-1].x2+10, aiHumanNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_human() ); // "Human" );
						font_bold_black.put( aiMonsterNationGroup[MAX_NATION-1].x2+10, aiMonsterNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_fryhtan() ); //"Fryhtan" );
						font_bold_black.center_put( 341, 305-30, 654, 324-30, text_game_menu.str_difficulty_level()); // "Difficulty Level" );
#ifndef ONLY_TERRAIN_SET_1
						font_bold_black.center_put( 370, 380-60, 633, 393-60, text_game_menu.str_terrain_set()); // "Terrain Type" );
#endif
#ifndef ONLY_SMALL_BUILDING
						font_bold_black.center_put( 341, 365, 660, 384, text_game_menu.str_building_set()); // "Building Size" );
#endif

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
					{
						if( speciesGroup() == 0 )		// human page
						{
							if( tempConfig.race_id > 0 )
								raceGroup.paint( tempConfig.race_id-1 );
							else
							{
								raceGroup.push( -1, 0 );		// pop all button, and paint
								raceGroup.paint();
							}
						}
						else if( speciesGroup() == 1 )		// monster page
						{
							if( tempConfig.race_id < 0 )
							{
								monsterRaceGroup.paint( -tempConfig.race_id-1 );

								char str[10] = "P-X";
								str[2] = '0' - tempConfig.race_id;
								char* bitmapPtr = image_interface.get_ptr(str);
								// all p-x must be the same size
								// center to 220,370
								const int picWidth = 198;
								const int picHeight = 121;
								const int picX = 220-picWidth/2;
								const int picY = 370-picHeight/2;
								if( !vga.use_back_buf )
								{
									mouse.hide_area(picX, picY, picX+picWidth-1, picY+picHeight-1 );
									vga.active_buf->join_trans( &vga_back, picX, picY, bitmapPtr );
									mouse.show_area();
								}
								else
								{
									vga.active_buf->put_bitmap_trans( picX, picY, bitmapPtr );
								}
							}
							else
							{
								monsterRaceGroup.push( -1, 0 );		// pop all button and paint
								monsterRaceGroup.paint();
							}
						}
					}
					if( refreshFlag & SGOPTION_COLOR )
					{
						vga.use_back();		// to avoid flickering

						// ------ put color box ------ //
						char *bitmapPtr = image_button.read("F-COLOR");
						vga.active_buf->put_bitmap_trans_remap_decompress(
							colorButtonFrameX, colorButtonFrameY, bitmapPtr,
							game.color_remap_array[tempConfig.player_nation_color].color_table );
						colorGroup.paint(tempConfig.player_nation_color-1);

						vga.use_front();

						vga.blt_buf( colorButtonFrameX, colorButtonFrameY,
							colorButtonFrameX + ((Bitmap *)bitmapPtr)->get_width() - 1,
							colorButtonFrameY + ((Bitmap *)bitmapPtr)->get_height() - 1, 0 );
					}
					if( refreshFlag & SGOPTION_AI_NATION )
					{
						aiHumanNationGroup.paint(tempConfig.ai_human_nation_count);
						aiMonsterNationGroup.paint(tempConfig.ai_monster_nation_count);
					}
					if( refreshFlag & SGOPTION_DIFFICULTY )
					{
						if( tempConfig.difficulty_level != OPTION_CUSTOM )
							diffGroup.paint(tempConfig.difficulty_level);
						else
							diffGroup.paint(diffGroup.button_num-1);	// last button
					}
#ifndef ONLY_TERRAIN_SET_1
					if( refreshFlag & SGOPTION_TERRAIN )
						terrainGroup.paint(tempConfig.terrain_set-1);
#endif
#ifndef ONLY_SMALL_BUILDING
					if( refreshFlag & SGOPTION_BUILDING_SIZE )
						buildingSizeGroup.paint(tempConfig.building_size-1);
#endif
					if( refreshFlag & SGOPTION_NAME_FIELD )
						playerNameField.paint(0);		// don't put cursor, it is not inputable
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCED )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put_paragraph( 126, 99, option3X-10, 143-1,
							text_game_menu.str_world_map()); // "World Map" );
						font_bold_black.put_paragraph( 126, 143, option3X-10, 183-1,
							text_game_menu.str_fog_of_war()); // "Fog of War" );
						font_bold_black.put_paragraph( 126, 183, option3X-10, 235-1,
						//	"Human Player's Treasure" );
							text_game_menu.str_your_treasure()); // "Your Treasure" );
						font_bold_black.put_paragraph( 126, 235, option3X-10, 289-1,
							text_game_menu.str_ai_treasure()); // "Computer's Treasure" );
						font_bold_black.put_paragraph( 126, 289, option3X-10, 339-1,
							text_game_menu.str_ai_aggressiveness()); // "Computer's Aggressiveness" );
#if(defined(GERMAN))
						font_bold_black.put_paragraph( 126, 348, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#else
						font_bold_black.put_paragraph( 126, 339, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#endif
						font_bold_black.put_paragraph( 126, 389, option3X-10, 449-1,
							text_game_menu.str_random_kingdom()); // "Random Kingdoms" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_EXPLORED )
						exploreGroup.paint(tempConfig.explore_whole_map);
					if( refreshFlag & SGOPTION_FOG )
						fogGroup.paint(tempConfig.fog_of_war);
					if( refreshFlag & SGOPTION_TREASURE )
						treasureGroup.paint( tempConfig.start_up_cash-1 );
					if( refreshFlag & SGOPTION_AI_TREASURE )
						aiTreasureGroup.paint( tempConfig.ai_start_up_cash-1 );
					// ###### begin Gilbert 10/2 ######//
					if( refreshFlag & SGOPTION_AI_AGGRESSIVE )
						aiAggressiveGroup.paint(tempConfig.ai_aggressiveness);
					// ###### end Gilbert 10/2 ######//
					if( refreshFlag & SGOPTION_SPY_METHOD )
						spyMethodGroup.paint(tempConfig.spy_methodology);
					if( refreshFlag & SGOPTION_RANDOM_STARTUP )
						randomStartUpGroup.paint(tempConfig.random_start_up);
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCE2 )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put_paragraph( 126, 100, option4X-10, 152-1,
							text_game_menu.str_start_up_raw_site()); // "Natural Resources at Start" );
						font_bold_black.put_paragraph( 126, 152, option4X-10, 206-1,
							text_game_menu.str_raw_nearby()); // "Natural Resources Nearby" );
						font_bold_black.put_paragraph( 126, 206, option4X-10, 246-1,
							text_game_menu.str_independent_town()); // "Independent Towns" );
						font_bold_black.put_paragraph( 126, 246, option4X-10, 293-1,
							text_game_menu.str_town_resistance()); // "Independent Town Resistance" );
						font_bold_black.put_paragraph( 126, 293, option4X-10, 340-1,
							text_game_menu.str_new_town_emerge()); // "New Towns Emerge" );
						font_bold_black.put_paragraph( 126, 340, option4X-10, 384-1,
							text_game_menu.str_new_kingdom_emerge()); // "New Kingdoms Emerge" );
						font_bold_black.put_paragraph( 126, 384, option4X-10, 434-1,
							text_game_menu.str_random_events()); // "Random Events" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RAW )
						rawSiteGroup.paint(tempConfig.start_up_raw_site-1);
					if( refreshFlag & SGOPTION_NEAR_RAW )
						nearRawGroup.paint(tempConfig.start_up_has_mine_nearby);
					if( refreshFlag & SGOPTION_START_TOWN )
					{
						for( i = 0; tempConfig.start_up_independent_town > startTownArray[i]; ++i );
						townStartGroup.paint(i);
					}
					if( refreshFlag & SGOPTION_TOWN_STRENGTH )
						townResistGroup.paint(tempConfig.independent_town_resistance-1);
					if( refreshFlag & SGOPTION_TOWN_EMERGE )
						townEmergeGroup.paint(tempConfig.new_independent_town_emerge);
					if( refreshFlag & SGOPTION_KINGDOM_EMERGE )
						nationEmergeGroup.paint(tempConfig.new_nation_emerge);
					if( refreshFlag & SGOPTION_RANDOM_EVENT )
						randomEventGroup.paint(tempConfig.random_event_frequency);
				}

				// ------- display goal option ---------//
				if( optionMode == OPTION_GOAL )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, 444, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						font_bold_black.put( option5X, 112, text_game_menu.str_goal() );	// You will be victorious when you have:

						font_thin_black.put_paragraph( option5X+tickWidth+10, 145, option5X3+25, 178-1, 
							text_game_menu.str_defeat_others()); // "Defeated All Others" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 178, option5X3+25, 211-1, 
							text_game_menu.str_defeat_fryhtan_lairs()); // "Destroyed All Independent Fryhtan Lairs" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 211, option5X3+25, 244-1, 
							text_game_menu.str_defeat_except_ally()); // "Defeated All except one Allied Kingdom" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 244, option5X2-65, 277-1, 
							text_game_menu.str_reach_population()); // "Achieved A Population Of" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 277, option5X2-65, 310-1, 
							text_game_menu.str_reach_economy()); // "Reached An Economic Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 310, option5X2-65, 343-1, 
							text_game_menu.str_reach_total_score()); //"Reached A Total Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 343, option5X2-100, 376-1, 
							text_game_menu.str_goal_time_limit()); // "Achieved The Selected Within" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					font_bold_black.use_max_height();

					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						clearEnemyButton.paint();
					if( refreshFlag & SGOPTION_CLEAR_MONSTER )
						clearMonsterButton.paint(tempConfig.goal_destroy_monster);
					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						allyWinButton.paint(tempConfig.goal_alliance_win_flag);
					if( refreshFlag & SGOPTION_ENOUGH_PEOPLE )
					{
						enoughPeopleButton.paint(tempConfig.goal_population_flag);
						font_bold_black.put( peopleInc.x1-65, peopleInc.y1, 
							m.format(tempConfig.goal_population), 1, peopleInc.x1-1);
						peopleInc.paint();
						peopleDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_INCOME )
					{
						enoughIncomeButton.paint(tempConfig.goal_economic_score_flag);
						font_bold_black.put( incomeInc.x1-65, incomeInc.y1, 
							m.format(tempConfig.goal_economic_score), 1, incomeInc.x1-1 );
						incomeInc.paint();
						incomeDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_SCORE )
					{
						enoughScoreButton.paint(tempConfig.goal_total_score_flag);
						font_bold_black.put( scoreInc.x1-65, scoreInc.y1, 
							m.format(tempConfig.goal_total_score), 1, scoreInc.x1-1 );
						scoreInc.paint();
						scoreDec.paint();
					}
					if( refreshFlag & SGOPTION_TIME_LIMIT )
					{
						timeLimitButton.paint(tempConfig.goal_year_limit_flag);
						int x2 = font_bold_black.put( yearInc.x1-100, yearInc.y1, 
							m.format(tempConfig.goal_year_limit), 1, yearInc.x1-60-1 );
						font_thin_black.put( yearInc.x1-60, yearInc.y1, text_game_menu.str_goal_time_units(), 1 );	// "years"
						yearInc.paint();
						yearDec.paint();
					}

					font_bold_black.use_std_height();
				}
				// ##### begin Gilbert 6/7 ##########//
				// ------- display chat log --------//
				if( optionMode == OPTION_CHAT_LOG )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- draw frame for multiplayers -------//

						vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
						vga.active_buf->d3_panel_down( 126, SCROLL_SHEET_Y1+11, 665, 516 );
						vga.active_buf->d3_panel_down( 126, 528, 665, 552 );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						if( remote.is_host )
						{
							font_thin_black.center_put( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
								text_game_menu.str_start() ); // "Start" );
						}
						font_thin_black.center_put( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
							text_game_menu.str_ready() ); // "Ready" );
						font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
							text_game_menu.str_cancel() ); // "Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					font_bold_black.use_max_height();
					font_bold_black.use_std_height();
				}
				// ##### end Gilbert 6/7 ##########//

				// -------- refresh players in the session --------//
				if( mRefreshFlag & MGOPTION_PLAYERS )
				{
					vga.use_back();
					vga.active_buf->d3_panel_down( 126, 1, 665, 69 );
					for( p = 0; p < regPlayerCount; ++p)
					{
						// put a tick if ready
						if( !playerReadyFlag[p] )
						{
							vga.active_buf->bar( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, V_WHITE );
							vga.active_buf->d3_panel_up( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, 4, 0 );
						}
						else
						{
							vga.active_buf->d3_panel_down( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1, 4, 0 );
							vga.active_buf->put_bitmap_trans_decompress( tickX[p]+4, tickY[p]+4,
								image_button.read("TICK-S") );
						}

						// put color of player
						vga.active_buf->bar( tickX[p]+nameTickWidth, tickY[p], tickX[p]+nameTickWidth*2-1, tickY[p]+nameTickHeight-1,
							game.color_remap_array[playerColor[p]].main_color );
						vga.active_buf->d3_panel_up( tickX[p]+nameTickWidth, tickY[p], tickX[p]+nameTickWidth*2-1, tickY[p]+nameTickHeight-1, 4, 0 );
						PlayerDesc *dispPlayer = mp_obj.search_player(regPlayerId[p]);
						font_bld.put( tickX[p]+nameTickWidth*2+3, tickY[p]+0, dispPlayer?dispPlayer->friendly_name_str():(char*)"?anonymous?",
							0, tickX[p]+260 );
#ifdef MAX_GEM_STONES
						if( totalGemStones[p] )
						{
							x2 = font_bld.put(x2, tickY[p]+0, " (", 0, tickX[p]+260 );
							if( sys.debug_session )
							{
								x2 = font_bld.put(x2, tickY[p]+0, useGemStones[p], 0, tickX[p]+260 );
								x2 = font_bld.put(x2, tickY[p]+0, "/", 0, tickX[p]+260 );
							}
							x2 = font_bld.put(x2, tickY[p]+0, totalGemStones[p], 0, tickX[p]+260 );
							x2 = font_bld.put(x2, tickY[p]+0, ")", 0, tickX[p]+260 );
						}
#endif
					}
					vga.blt_buf( 126, 1, 665, 69, 0 );
					vga.use_front();
				}

				// ------------ display chat message --------//
				if( mRefreshFlag & MGOPTION_OUT_MESSAGE )
				{
					messageField.paint();
				}

				// ------------- display incoming chat message --------//
				if( mRefreshFlag & MGOPTION_IN_MESSAGE )
				{
					vga.use_back();
					// ######### begin Gilbert 6/7 ##########//
					int y1 = optionMode != OPTION_CHAT_LOG ? 444 : SCROLL_SHEET_Y1+11;
					int y2 = 516;
					vga.active_buf->d3_panel_down( 126, y1, 665, y2 );
					int ny = y1 + 4;
					int lineDisp = (y2-4 - ny + 1 ) / font_cara.max_font_height;
					p = messageList.size() - lineDisp + 1;		// display 4 lines
					if( p <= 0 )
						p = 1;
					for( ; p <= messageList.size() ; ++p, (ny += font_cara.max_font_height))
					{
						int nx = 132;
						if( ((MpStructChatMsg *)messageList.get(p))->sender[0] )
						{
							nx = font_cara.put( nx, ny, ((MpStructChatMsg *)messageList.get(p))->sender, 0, 661);
							nx = font_cara.put( nx, ny, " : ", 0, 661 );
						}
						nx = font_cara.put( nx, ny, ((MpStructChatMsg *)messageList.get(p))->content, 0, 661 );
					}
					vga.blt_buf( 126, y1, 665, y2, 0 );
					vga.use_front();

					// display button

					if( optionMode != OPTION_CHAT_LOG )
						vga.active_buf->d3_panel_up( CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2 );
					else
						vga.active_buf->d3_panel_down( CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2 );
					// ######### end Gilbert 6/7 ##########//
				}

				// ----- display difficulty -------//

				if( optionMode != OPTION_SPECIES && refreshFlag & SGOPTION_DIFFICULTY )
				{
					font_bld.center_put( 40, 85, 82, 105, text_game_menu.str_level_in_score(), 1 );	// "Level"
					// load game only, display difficulty_rating
					// font_bold_black.center_put( 40, 105, 82, 125, m.format(tempConfig.multi_player_difficulty(regPlayerCount-1,0)), 1 );
					if( tempConfig.race_id >= 0 )
						font_bold_black.center_put( 40, 105, 82, 125, m.format(tempConfig.human_difficulty_rating), 1 );
					else
						font_bold_black.center_put( 40, 105, 82, 125, m.format(tempConfig.monster_difficulty_rating), 1 );
				}

				refreshFlag = 0;
				mRefreshFlag = 0;
			}

			sys.blt_virtual_buf();

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// --------- detect remote message -------//
			mp_obj.before_receive();
			recvPtr = mp_obj.receive(&from, &to, &recvLen, &sysMsgCount);
			mp_obj.after_send();

			if( sysMsgCount )
			{
				// see any player disconnect
				for(int q = 0; q < regPlayerCount; ++q)
				{
					if( !mp_obj.is_player_connecting(regPlayerId[q]) )
					{
						mRefreshFlag |= MGOPTION_PLAYERS;

						memmove( regPlayerId+q, regPlayerId+q+1, (MAX_NATION-1-q)*sizeof(regPlayerId[0]) );
						regPlayerId[MAX_NATION-1] = 0;
						memmove( playerReadyFlag+q, playerReadyFlag+q+1, (MAX_NATION-1-q)*sizeof(playerReadyFlag[0]) );
						playerReadyFlag[MAX_NATION-1] = 0;
						short freeColor = playerColor[q];
						memmove( playerColor+q, playerColor+q+1, (MAX_NATION-1-q)*sizeof(playerColor[0]) );
						playerColor[MAX_NATION-1] = 0;
						if(freeColor > 0 && freeColor <= MAX_COLOR_SCHEME)
							colorAssigned[freeColor-1] = 0;
						short freeRace = playerRace[q];
						memmove( playerRace+q, playerRace+q+1, (MAX_NATION-1-q)*sizeof(playerRace[0]) );
						playerRace[MAX_NATION-1] = 0;
						if(freeRace != 0)
							raceAssigned_[freeRace]--;
						memmove( playerBalance+q, playerBalance+q+1, (MAX_NATION-1-q)*sizeof(playerBalance[0]) );
						playerBalance[MAX_NATION-1] = 0;
#ifdef MAX_GEM_STONES
						memmove( useGemStones+q, useGemStones+q+1, (MAX_NATION-1-q)*sizeof(useGemStones[0]) );
						useGemStones[MAX_NATION-1] = 0;
						memmove( totalGemStones+q, totalGemStones+q+1, (MAX_NATION-1-q)*sizeof(totalGemStones[0]) );
						totalGemStones[MAX_NATION-1] = 0;
#endif
#ifdef APPLY_RANKING
						memmove( playerRanking+q, playerRanking+q+1, (MAX_NATION-1-q)*sizeof(playerRanking[0]) );
						playerRanking[MAX_NATION-1] = 0;
#endif
						--regPlayerCount;
						--q;
					}
				}
			}

			if( recvPtr )
			{
				if( ((MpStructBase *)recvPtr)->msg_id == MPMSG_START_GAME )
				{
					retFlag = 1;
					break;		// break while(1) loop
				}
				else
				{
					switch( ((MpStructBase *)recvPtr)->msg_id )
					{
					case MPMSG_ABORT_GAME:
						box.msg( text_game_menu.str_mp_host_abort() );
						return 0;
					case MPMSG_SEND_CONFIG:
						// ##### patch begin Gilbert 14/1 #######//
						// now validate config sent from host
						if( ((MpStructConfig *)recvPtr)->game_config.validate() )
						{
							tempConfig.change_game_setting( ((MpStructConfig *)recvPtr)->game_config );
							refreshFlag |= SGOPTION_ALL_OPTIONS;
						}
						else
						{
							box.msg( text_game_menu.str_mp_refuse_reason(MpStructRefuseNewPlayer::REFUSE_VERSION) );
							return 0;
						}
						// ##### patch end Gilbert 14/1 #######//
						break;
					case MPMSG_RANDOM_SEED:
						info.init_random_seed( ((MpStructSeed *)recvPtr)->seed );
						break;
					case MPMSG_RANDOM_SEED_STR:
						msgSeedStr = *(MpStructSeedStr *)recvPtr;
//						mapIdField.select_whole();
						refreshFlag |= SGOPTION_MAP_ID;
						break;
					case MPMSG_NEW_PLAYER:
						{
							// incorrect message, reject
							MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_HOST_LOAD_GAME);
							mp_obj.send_stream(BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
						}
						break;
					case MPMSG_LOAD_GAME_NEW_PLAYER:
						mp_obj.poll_players();
						if( remote.is_host )
						{
							MpStructLoadGameNewPlayer *newPlayerMsg = (MpStructLoadGameNewPlayer *)recvPtr;
							if( regPlayerCount >= MAX_NATION )
							{
								// reply refuse new player
								MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_SEAT_FULL);
								mp_obj.send_stream( BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
							}
							else if( !newPlayerMsg->is_compatible() )
							{
								// reply refuse new player
								MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_VERSION);
								mp_obj.send_stream( BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
							}
							else 
							if( !(!nation_array.is_deleted(newPlayerMsg->nation_recno)
								&& nation_array[newPlayerMsg->nation_recno]->color_scheme_id == newPlayerMsg->color_scheme_id
								&& nation_array[newPlayerMsg->nation_recno]->race_id == newPlayerMsg->race_id
								&& !colorAssigned[newPlayerMsg->color_scheme_id-1] 
								&& newPlayerMsg->frame_count == sys.frame_count
								&& newPlayerMsg->random_seed == m.get_random_seed()) )
							{
								// reply refuse new player
								MpStructRefuseNewPlayer msgRefuse(from, MpStructRefuseNewPlayer::REFUSE_LOAD_GAME_UNSYNC );
								mp_obj.send_stream( BROADCAST_PID, &msgRefuse, sizeof(msgRefuse) );
							}
							else
							{
								regPlayerId[regPlayerCount] = newPlayerMsg->player_id;
								playerReadyFlag[regPlayerCount] = 0;
								raceAssigned_[newPlayerMsg->race_id]++;
								playerRace[regPlayerCount] = newPlayerMsg->race_id;
								colorAssigned[newPlayerMsg->color_scheme_id-1]=1;
								playerColor[regPlayerCount] = newPlayerMsg->color_scheme_id;

								// send accept new player to all player
								MpStructAcceptNewPlayer msgAccept(from);
								mp_obj.send_stream( BROADCAST_PID, &msgAccept, sizeof(msgAccept) );

								// send new color of this player to all people

								MpStructAcceptColor msgAcceptColor(from, newPlayerMsg->color_scheme_id );
								mp_obj.send_stream( BROADCAST_PID, &msgAcceptColor, sizeof(msgAcceptColor) );

								// send ready flag
								for( int c = 0; c < regPlayerCount; ++c)
								{
									if( playerReadyFlag[c] )
									{
										MpStructPlayerReady msgReady(regPlayerId[c]);
										mp_obj.send_stream(from, &msgReady, sizeof(msgReady));
									}
								}

								// send color chosen
								for( c = 0; c < regPlayerCount; ++c)
								{
									if( playerColor[c] )
									{
										MpStructAcceptColor msgAcceptColor(regPlayerId[c], playerColor[c]);
										mp_obj.send_stream( from, &msgAcceptColor, sizeof(msgAcceptColor) );
									}
								}							

								// ##### begin Gilbert 11/3 ########//
								// send process frame delay
								MpStructProcessFrameDelay msgframeDelay(frameDelaySettingArray[frameDelayGroupSetting]);
								mp_obj.send_stream( from, &msgframeDelay, sizeof(msgframeDelay) );
								// ##### end Gilbert 11/3 ########//

								// send remote.sync_test_level
								MpStructSyncLevel msgSyncTest(remote.sync_test_level);
								mp_obj.send_stream( from, &msgSyncTest, sizeof(msgSyncTest) );

								// update balance
								playerBalance[regPlayerCount] = newPlayerMsg->player_balance;
#ifdef MAX_GEM_STONES
								useGemStones[regPlayerCount] = 0;
								totalGemStones[regPlayerCount] = 0;
								// send use Gemstone
								if( useGemStoneFlag )
								{
									for( c = 0; c < regPlayerCount; ++c)
									{
										MpStructSetGemStones msgUseGemStones(regPlayerId[c], useGemStones[c], totalGemStones[c]);
										mp_obj.send_stream(from, &msgUseGemStones, sizeof(msgUseGemStones));
									}
								}
#endif
#ifdef APPLY_RANKING
								playerRanking[regPlayerCount] = newPlayerMsg->ranking;
#endif

								regPlayerCount++;
								mRefreshFlag |= MGOPTION_PLAYERS;
							}
						}
						break;
					case MPMSG_ACCEPT_NEW_PLAYER:
						hostPlayerId = from;
						if( regPlayerCount < MAX_NATION && ((MpStructAcceptNewPlayer *)recvPtr)->player_id != mp_obj.my_player_id )
						{
							// search if this player has existed
							for( p=0; p < regPlayerCount && regPlayerId[p] != ((MpStructAcceptNewPlayer *)recvPtr)->player_id; ++p );
							regPlayerId[p] = ((MpStructAcceptNewPlayer *)recvPtr)->player_id;
							playerReadyFlag[p] = 0;
							playerColor[p] = 0;
							if( p >= regPlayerCount )
							{
								err_when( p != regPlayerCount );
								regPlayerCount++;		// now regPlayerCount == p
							}
#ifdef MAX_GEM_STONES
							useGemStones[p] = 0;
							totalGemStones[p] = 0;
#endif
							mRefreshFlag |= MGOPTION_PLAYERS;
						}

						// ####### begin Gilbert 15/4 ##########//
						// put a message to the window
						if( ((MpStructAcceptNewPlayer *)recvPtr)->player_id == mp_obj.my_player_id )
						{
							MpStructChatMsg waitReplyMessage(NULL, text_game_menu.str_mp_host_ok() ); //"Ok" );
							messageList.linkin( &waitReplyMessage);
							mRefreshFlag |= MGOPTION_IN_MESSAGE;
						}
						// ####### end Gilbert 15/4 ##########//

						break;
					case MPMSG_ACCEPT_COLOR:
						{	// other player change color
							err_when( remote.is_host );
							for( int p = 0; p < regPlayerCount && regPlayerId[p] != ((MpStructAcceptColor *)recvPtr)->request_player_id; ++p );
							short cl = ((MpStructAcceptColor *)recvPtr)->color_scheme_id;
							if( cl <= MAX_COLOR_SCHEME && p < regPlayerCount )
							{
								if( playerColor[p] > 0 )
									colorAssigned[playerColor[p]-1] = 0;

								// mark color assigned
								colorAssigned[cl-1] = 1;
								playerColor[p] = cl;
							}
							mRefreshFlag |= MGOPTION_PLAYERS;
						}
						break;
					case MPMSG_PLAYER_READY:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructPlayerReady *)recvPtr)->player_id )
								{
									playerReadyFlag[p] = 1;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
						}
						break;
					case MPMSG_PLAYER_UNREADY:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructPlayerUnready *)recvPtr)->player_id )
								{
									playerReadyFlag[p] = 0;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
						}
						break;
#ifdef MAX_GEM_STONES
					case MPMSG_SET_GEM_STONE:
						{
							for( int p = 0; p < regPlayerCount; ++p)
							{
								if( regPlayerId[p] == ((MpStructSetGemStones *)recvPtr)->player_id )
								{
									useGemStones[p] = ((MpStructSetGemStones *)recvPtr)->use_gem_stones;
									totalGemStones[p] = ((MpStructSetGemStones *)recvPtr)->total_gem_stones;
									mRefreshFlag |= MGOPTION_PLAYERS;
								}
							}
							if( !remote.is_host && !useGemStoneFlag)
							{
								useGemStoneFlag = 1;		// enable useGemStoneFlag
								// must use one gem stone if has any
								selfUseGemStones = player_profile.gem_stones > 0 ? 1 : 0;

								// tell other player how many gemstone to use
								MpStructSetGemStones msgSetGemStones( mp_obj.my_player_id, selfUseGemStones, player_profile.gem_stones );
								mp_obj.send_stream(BROADCAST_PID, &msgSetGemStones, sizeof(msgSetGemStones));

								// update itself
								for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
								if( p < regPlayerCount )
								{
									useGemStones[p] = selfUseGemStones;
									totalGemStones[p] = player_profile.gem_stones;
								}

								mRefreshFlag |= MGOPTION_GEM_STONES | MGOPTION_PLAYERS;
							}
						}
						break;
#endif
					case MPMSG_SEND_CHAT_MSG:
						messageList.linkin(recvPtr);
						mRefreshFlag |= MGOPTION_IN_MESSAGE;
						break;
					// ###### begin Gilbert 11/3 ########//
					case MPMSG_SET_PROCESS_FRAME_DELAY:
						{
							for( frameDelayGroupSetting = 2; frameDelayGroupSetting > 0; --frameDelayGroupSetting )
							{
								if( ((MpStructProcessFrameDelay *)recvPtr)->common_process_frame_delay >= frameDelaySettingArray[frameDelayGroupSetting] )
									break;
							}
							mRefreshFlag |= MGOPTION_FRAME_DELAY;
						}
						break;
					// ###### end Gilbert 11/3 ########//
					case MPMSG_SEND_SYNC_TEST_LEVEL:
						remote.sync_test_level = ((MpStructSyncLevel *)recvPtr)->sync_test_level;
						break;
					case MPMSG_END_SETTING:
						++recvEndSetting;
						break;
					case MPMSG_REFUSE_NEW_PLAYER:
						if( ((MpStructRefuseNewPlayer *)recvPtr)->player_id == mp_obj.my_player_id )
						{
							box.msg(((MpStructRefuseNewPlayer *)recvPtr)->reason_str());
							return 0;
						}
						break;
					default:		// if the game is started, any other thing is received
						return 0;
					}
				}
			}

			// ------ detect species option ---------//

			// ####### begin Gilbert 11/3 ########//
			if( optionMode == OPTION_SPECIES )
			{
				if( remote.is_host && frameDelayGroup.detect() >= 0 )
				{
					frameDelayGroupSetting = frameDelayGroup();
					mRefreshFlag = MGOPTION_FRAME_DELAY;

					// tell other player
					MpStructProcessFrameDelay msgframeDelay(frameDelaySettingArray[frameDelayGroupSetting]);
					mp_obj.send_stream( BROADCAST_PID, &msgframeDelay, sizeof(msgframeDelay) );
				}
#ifdef MAX_GEM_STONES
				else if( useGemStoneFlag && !selfReadyFlag
					&& gemStoneGroup.detect() >= 0 )
				{
					selfUseGemStones = gemStoneGroup();

					MpStructSetGemStones msgSetGemStones( mp_obj.my_player_id, selfUseGemStones, player_profile.gem_stones );
					mp_obj.send_stream(BROADCAST_PID, &msgSetGemStones, sizeof(msgSetGemStones));

					// update itself
					for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
					if( p < regPlayerCount )
					{
						useGemStones[p] = selfUseGemStones;
						totalGemStones[p] = player_profile.gem_stones;
					}
					mRefreshFlag |= MGOPTION_GEM_STONES | MGOPTION_PLAYERS;
				}
#endif
			}
			// ####### end Gilbert 11/3 ########//

			// -------- detect switch option button ---------//

			for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
			{
				if( mouse.single_click( OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
					OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i) )
				{
					optionMode = i;
					refreshFlag = SGOPTION_ALL;
					mRefreshFlag = MGOPTION_ALL;
				}
			}

			// ######## begin Gilbert 6/7 #########//
			if( mouse.any_click(CHAT_LOG_X1, CHAT_LOG_Y1, CHAT_LOG_X2, CHAT_LOG_Y2) )
			{
				if( optionMode != OPTION_CHAT_LOG )
				{
					backupOptionMode = optionMode;
					optionMode = OPTION_CHAT_LOG;
				}
				else
				{
					err_when( backupOptionMode < OPTION_SPECIES || backupOptionMode > OPTION_GOAL );
					optionMode = backupOptionMode;
				}
				refreshFlag = SGOPTION_ALL;
				mRefreshFlag = MGOPTION_ALL;
			}
			// ######## end Gilbert 6/7 #########//

			// --------- detect ready button button --------//

			unsigned keyCode;

			// ########## begin Gilbert 7/4 ##########//
			// --------- or click at the tick near name ---------//

			for(p = 0; p < regPlayerCount && regPlayerId[p] != mp_obj.my_player_id; ++p);
			if( p < regPlayerCount 
				&& (mouse.any_click(BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2)
				|| mouse.any_click( tickX[p], tickY[p], tickX[p]+nameTickWidth-1, tickY[p]+nameTickHeight-1)) )
			{
				mRefreshFlag |= MGOPTION_PLAYERS;
				if( !selfReadyFlag ) 
				{
					playerReadyFlag[p] = selfReadyFlag = 1;
					MpStructPlayerReady msgReady(mp_obj.my_player_id);
					mp_obj.send_stream(BROADCAST_PID, &msgReady, sizeof(msgReady));
				}
				else
				{
					// else un-ready this player
					playerReadyFlag[p] = selfReadyFlag = 0;
					MpStructPlayerUnready msgUnready(mp_obj.my_player_id);
					mp_obj.send_stream(BROADCAST_PID, &msgUnready, sizeof(msgUnready));
				}
			}
			// ########## end Gilbert 7/4 ##########//

			// ------ detect start button --------//

			if( remote.is_host && mouse.single_click( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2 ) )
			{
				// see if all player is ready
				short sumBalance = 0;
				for( int q = 0; q < regPlayerCount && playerReadyFlag[q]; ++q)
				{
					err_when( playerBalance[q] == 0 );
					sumBalance += playerBalance[q];
				}
				if( q >= regPlayerCount )		// not all playerReadyFlag[p] = 1;
				{
#if( defined(DEBUG) || defined(BETA) || defined(DEMO) )
					sumBalance = 0;
#endif
					if( regPlayerCount != maxPlayer )
					{
						box.msg( text_game_menu.str_mp_lack_players(regPlayerCount, maxPlayer) );
					}
					else if( sumBalance < 0 )
					{
						// too many non-CD version
						box.msg( text_game_menu.str_mp_cd_count() );
					}
					else
					{
//						MpStructBase msgStart(MPMSG_START_GAME);
//						mp_obj.send_stream(BROADCAST_PID, &msgStart, sizeof(msgStart));
						retFlag = 1;
						break;							// break while(1)
					}
				}
			}

			// -------- detect return button ---------//

			else if( mouse.single_click(BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2) )
			{
				if( box.ask(text_game_menu.str_mp_confirm_quit()) )
				{
					if( remote.is_host )
					{
						MpStructBase msgAbort(MPMSG_ABORT_GAME);
						mp_obj.send_stream(BROADCAST_PID, &msgAbort, sizeof(msgAbort) );
					}
					retFlag = 0;
					break;			// break while(1)
				}
			}
			else if( (keyCode = messageField.detect()) != 0)		// keyCode may be non-zero if after mapIdField.detect()
			{
				mRefreshFlag |= MGOPTION_OUT_MESSAGE;
				if(keyCode == KEY_RETURN && strlen(typingMsg.content) > 0)
				{
					if( !process_load_game_chat_command(messageField.input_field, &messageList) )
					{
						// send message
						mp_obj.send_stream(BROADCAST_PID, &typingMsg, sizeof(typingMsg) );

						// add to own list
						messageList.linkin(&typingMsg);
					}

					mRefreshFlag |= MGOPTION_IN_MESSAGE;

					// clear the string
					messageField.clear();
				}
				else if( keyCode == KEY_ESC )
				{
					messageField.clear();
				}
			}

			mp_obj.after_send();
		}	// end while
	} // end of scope of VgaLock

	mp_obj.after_send();

	// ---------- final setup to start multiplayer game --------//

	if( retFlag > 0)
	{
		retFlag = 0;

		if( remote.is_host )
			mp_obj.disable_join_session();

		// mp_obj.poll_players();
			
		int trial;
		unsigned long startTime;
		int playerCount = 0;

		if( &config != &tempConfig )
			config = tempConfig;

		if( remote.is_host )
		{
			// setup nation now
			VLenQueue setupString;

			// ------- put start game string -------//

			{
				MpStructBase msgStart(MPMSG_START_GAME);
				memcpy( setupString.reserve(sizeof(msgStart)), &msgStart, sizeof(msgStart) );
			}

			// ------- put nations -------//

			playerCount = 0;
			for( p = 0; p < regPlayerCount; ++p )
			{
				PID_TYPE playerId = regPlayerId[p];
				PlayerDesc *player = mp_obj.search_player(playerId);
				if( !playerId || !player || !player->connecting )
					continue;

				// match nation color
				for( short nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno)
				{
					Nation *nationPtr;
					if( !nation_array.is_deleted(nationRecno) && (nationPtr = nation_array[nationRecno]) 
						&& (nationPtr->is_own() || nationPtr->is_remote())
						&& playerColor[p] == nation_array[nationRecno]->color_scheme_id )
					{
						nationPtr->player_id = playerId;
						nationPtr->next_frame_ready = 0;
						((MpStructNation *)setupString.reserve(sizeof(MpStructNation)))->init(
							nationRecno, nationPtr->player_id,
							nationPtr->color_scheme_id, nationPtr->race_id, player->friendly_name_str(),
							0, 0 );	// pass useGemStones and ranking as 0, since checking only
						playerCount++;
						break;
					}
				}
			}

//			for( int d = 1; playerCount<MAX_NATION && mp_obj.get_player(d); ++d)
//			{
//				// ensure it is a valid player
//				PlayerDesc *player = mp_obj.get_player(d);
//				PID_TYPE playerId = player ? player->pid() : 0;
//				if( !playerId || !player->connecting )
//					continue;
//				for( p = 0; p < regPlayerCount && regPlayerId[p] != playerId; ++p);
//				if( p >= regPlayerCount )		// not found
//					continue;
//
//				// match nation color
//				for( short nationRecno = 1; nationRecno <= nation_array.size(); ++nationRecno)
//				{
//					Nation *nationPtr;
//					if( !nation_array.is_deleted(nationRecno) && (nationPtr = nation_array[nationRecno]) 
//						&& (nationPtr->is_own() || nationPtr->is_remote())
//						&& playerColor[p] == nation_array[nationRecno]->color_scheme_id )
//					{
//						nationPtr->player_id = playerId;
//						nationPtr->next_frame_ready = 0;
//						((MpStructNation *)setupString.reserve(sizeof(MpStructNation)))->init(
//							nationRecno, nationPtr->player_id,
//							nationPtr->color_scheme_id, nationPtr->race_id, player->friendly_name_str());
//						playerCount++;
//						break;
//					}
//				}
//			}

			//--- if the current number of players < original number of players ---//

			if( playerCount != maxPlayer )
			{
				box.msg( text_game_menu.str_mp_lack_players(playerCount, maxPlayer) );
				return 0;
			}

			// ---- force set to the lowest frame delay -------//

			// ###### begin Gilbert 11/3 ########//
#ifdef FORCE_MAX_FRAME_DELAY
			remote.set_process_frame_delay(FORCE_MAX_FRAME_DELAY);
#else
			remote.set_process_frame_delay(frameDelaySettingArray[frameDelayGroupSetting]);
#endif
			// ###### end Gilbert 11/3 ########//
			{
				MpStructProcessFrameDelay msgFrameDelay(remote.get_process_frame_delay());
				memcpy( setupString.reserve(sizeof(msgFrameDelay)), &msgFrameDelay, sizeof(msgFrameDelay));
			}

			// -------- send sync test level ----------//

			{
				MpStructSyncLevel msgSyncTest(remote.sync_test_level);
				memcpy( setupString.reserve(sizeof(msgSyncTest)), &msgSyncTest, sizeof(msgSyncTest));
			}

			mp_obj.send_stream( BROADCAST_PID, setupString.queue_buf, setupString.length() );
			mp_obj.after_send();
		}
		else
		{
			// use the message recving MPMSG_START_GAME

			err_when( !recvPtr );

			DWORD offset = 0;
			int recvStartMsg = 0;
			int ownPlayerFound = 0;
			playerCount = 0;
			char *oriRecvPtr = recvPtr;
			int recvSetFrameDelay = 0;
			int recvSyncTestLevel = 0;

			// process the string received
			while( offset < recvLen )
			{
				DWORD oldOffset = offset;
				recvPtr = oriRecvPtr + offset;

				switch( ((MpStructBase *)(recvPtr))->msg_id )
				{
				case MPMSG_START_GAME:
					recvStartMsg = 1;
					offset += sizeof( MpStructBase );
					break;

				case MPMSG_DECLARE_NATION:
					{
						MpStructNation *msgNation = (MpStructNation *)recvPtr;
						short nationRecno = msgNation->nation_recno;
						Nation *nationPtr;
						if( nation_array.is_deleted(nationRecno) ||
							!(nationPtr = nation_array[nationRecno]) ||
							!nationPtr->is_own() && !nationPtr->is_remote() ||
							nationPtr->color_scheme_id != msgNation->color_scheme )
						{
							box.msg( text_game_menu.str_mp_nation_info_incorrect() ); // "Incorrect kingdom information received from the host." );
							return 0;
						}
						nationPtr->player_id = msgNation->dp_player_id;
						if( nationPtr->is_own() && msgNation->dp_player_id == mp_obj.my_player_id)
						{
							ownPlayerFound = 1;
						}
						nationPtr->next_frame_ready = 0;

						//--------- set custom names -----------//

						// nation_array.set_custom_both_name(nationRecno, msgNation->player_name);

						//--------------------------------------//

						++playerCount;
						offset += sizeof( MpStructNation );
					}
					break;

				case MPMSG_SET_PROCESS_FRAME_DELAY:
					remote.set_process_frame_delay(((MpStructProcessFrameDelay *)recvPtr)->common_process_frame_delay);
					++recvSetFrameDelay;
					offset += sizeof( MpStructProcessFrameDelay );
					break;

				case MPMSG_SEND_SYNC_TEST_LEVEL:
					remote.sync_test_level = ((MpStructSyncLevel *)recvPtr)->sync_test_level;
					++recvSyncTestLevel;
					offset += sizeof( MpStructSyncLevel );
					break;
				}  // end switch

				if( !recvStartMsg || offset <= oldOffset )
				{
					err_here();
					box.msg( text_game_menu.str_mp_connect_str_corrupt() ); // "Connection string from host is corrupted" );
					return 0;
				}
			} // end while

			if( playerCount == 0 )
			{
				box.msg( text_game_menu.str_mp_nation_info_lost() ); // "Cannot get kingdom information from the host." );
				return 0;
			}
			err_when( playerCount > MAX_NATION );
			if( !ownPlayerFound )
			{
				box.msg( text_game_menu.str_mp_host_not_recognize() ); // "The host cannot recognize your machine." );
				return 0;
			}
			err_when( ownPlayerFound > 1 );
			if( !recvSetFrameDelay || !recvSyncTestLevel )
			{
				box.msg( text_game_menu.str_mp_miss_connect_info() ); // "Cannot receive important information from the host");
				return 0;
			}
			err_when( recvSetFrameDelay > 1 );
			err_when( recvSyncTestLevel > 1 );

			// ------- send end setting string ------- //

			MpStructBase mpEndSetting(MPMSG_END_SETTING);
			mp_obj.send_stream( from, &mpEndSetting, sizeof(mpEndSetting) );
			mp_obj.after_send();
		}	

		if( remote.sync_test_level == 0)
		{
			remote.set_alternating_send(playerCount > 4 ? 4 : playerCount);		// automatic setting
		}

		{
			// ------- broadcast end setting string ------- //

			MpStructBase mpEndSetting(MPMSG_END_SETTING);
			mp_obj.send_stream( BROADCAST_PID, &mpEndSetting, sizeof(mpEndSetting) );
			mp_obj.after_send();

			// ------ wait for MPMSG_END_SETTING ----------//
			// ---- to filter other all message until MP_MSG_END_SETTING ---//

			trial = 5000;
			startTime = m.get_time();
			// int recvEndSetting = 0;
			while( --trial > 0 || m.get_time() - startTime < 10000 )
			{
				if( recvEndSetting >= playerCount-1)
					break;
				mp_obj.before_receive();
				recvPtr = mp_obj.receive( &from, &to, &recvLen);
				mp_obj.after_send();
				if( recvPtr )
				{
					trial = max(trial, 1000);
					startTime = m.get_time();
					if( ((MpStructBase *)recvPtr)->msg_id == MPMSG_END_SETTING )
					{
						recvEndSetting++;
					}
				}
			}
			if( recvEndSetting < playerCount-1 )
			{
				box.msg( text_game_menu.str_mp_other_player_fail() ); // "Some player(s) encountered errors when initializing the game.");
				// but continue
			}
		}

		retFlag = 1;
	}		// end if(retFlag)

	return retFlag;
}
// --------- End of function Game::mp_select_load_option --------- //



static void disp_service_button(ButtonCustom *button, int)
{
	int serviceSeq = button->custom_para.value;
	ServiceProviderDesc *servicePtr = mp_obj.get_service_provider(serviceSeq);

	if( !button->pushed_flag )
	{
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	}

	(button->pushed_flag ? font_bold_red : font_bold_black).put( 
		button->x1+16, button->y1+6, servicePtr ? servicePtr->name_str() : (char*)"",
		1, button->x2-10 ); 

	if( servicePtr && servicePtr->name_str_long() )
	{
		font_bld.put(button->x1+16, button->y1+26,
			servicePtr->name_str_long(), 1, button->x2-10 );
	}

	if( button->pushed_flag )
	{
		vga.active_buf->rect( button->x1, button->y1, button->x2, button->y2, 2, V_BLACK );
	}
}





// ------ begin of static function i_disp_text_button ------//
//
// BustomCustom callback function to display text on top centre
//
static void i_disp_text_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
//	fontPtr->space_width -= 8;	
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		(char *)button->custom_para.ptr );
//	fontPtr->space_width += 8;	
}
// ------ end of static function i_disp_text_button ------//


// ------ begin of static function i_disp_race_button ------//
//
// BustomCustom callback function to display race name on top centre
//
static void i_disp_race_button(ButtonCustom *button, int repaintBody)
{
	int raceId = button->custom_para.value;
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		raceId >= 0 ? race_res[raceId]->name : monster_res[-raceId]->name );
}
// ------ end of static function i_disp_race_button ------//


// ------ begin of static function i_disp_int_button ------//
//
// BustomCustom callback function to display integer on top centre
//
static void i_disp_int_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		m.format(button->custom_para.value) );
}
// ------ end of static function i_disp_int_button ------//


// ------ begin of static function i_disp_color_button ------//
//
// BustomCustom callback function to display color code marker
//
static void i_disp_color_button(ButtonCustom *button, int repaintBody)
{
	// 9,7,35,37, x+29
	int colorSchemeId = button->custom_para.value;
	short *colorRemapTable = game.color_remap_array[colorSchemeId].color_table;

	if( !button->pushed_flag )
	{
		int clipX1 = 9 + (colorSchemeId-1)*29;
		int clipY1 = 7;
		int clipX2 = clipX1 + 27 - 1;
		int clipY2 = 37;

		vga.active_buf->put_bitmap_area_trans_remap_decompress( 
			button->x1 - clipX1,	button->y1 - clipY1, image_button.read("F-COLOR"), 
			clipX1, clipY1, clipX2, clipY2,	colorRemapTable );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_remap_decompress(
			button->x1, button->y1, image_button.read("F-COL-DW"), colorRemapTable );

	}
}
// ------ end of static function i_disp_color_button ------//



// ------ begin of static function i_disp_tick_button ------//
//
// BustomCustom callback function to display a tick
//
static void i_disp_tick_button(ButtonCustom *button, int repaintBody)
{
	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2 );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_decompress( button->x1+4, button->y1+4, image_button.read("TICK") );
		vga.active_buf->d3_panel_down( button->x1, button->y1, button->x2, button->y2, 4, 0 );
	}
}
// ------ end of static function i_disp_tick_button ------//


// ------ begin of function process_load_game_chat_command -------//
//
// return true if a command
// output is a DynArray of MpStructChatMsg
//
static int process_load_game_chat_command( char *cmdStr, DynArray *output )
{
	if( *cmdStr == '#' )
	{
		++cmdStr;

		switch( *cmdStr )
		{
		case 'r':
			{
				info.set_rank_data(0);

				for( int i = 1; i <= nation_array.size(); ++i )
				{
					if( nation_array.is_deleted(i) )
						continue;
					String str;

					str = text_game_menu.str_mp_ranking( 
						nation_array[i]->nation_name(),
						nation_array[i]->color_scheme_id,
						info.get_rank_score( 1, i ),		// population
						info.get_rank_score( 2, i ),		// military
						info.get_rank_score( 3, i ),		// economy
						info.get_rank_score( 4, i ),		// reputation
						info.get_rank_score( 5, i ),		// fryhtan battling
						info.get_total_score( i ) );		// total score

					MpStructChatMsg dispMsg( NULL, str.left(MpStructChatMsg::MSG_LENGTH) );
					output->linkin( &dispMsg );
				}
			}
			return 1;
		case '?':
			{
				MpStructChatMsg dispMsg( NULL, text_game_menu.str_hash_help(0) );
				output->linkin( &dispMsg );
			}
			{
				MpStructChatMsg dispMsg( NULL, text_game_menu.str_hash_help(1) );
				output->linkin( &dispMsg );
			}
			return 1;
		default:
			return 0;
		}
	}
	return 0;
}
// ------ end of function process_load_game_chat_command -------//
