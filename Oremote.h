//Filename    : OREMOTE.H
//Description : Header file of object Remote

#ifndef __OREMOTE_H
#define __OREMOTE_H

#include <MPTYPES.H>
#include <OREMOTEQ.H>

// correct MAX_PATH is found in windef.h
#define MAX_PATH 260

//---------- Define message id. ---------//

enum { FIRST_REMOTE_MSG_ID = 0x25D3 };

enum { MSG_QUEUE_HEADER=FIRST_REMOTE_MSG_ID,
		 MSG_QUEUE_TRAILER,
		 MSG_NEW_NATION,
		 MSG_UPDATE_GAME_SETTING,
		 MSG_START_GAME,
		 MSG_NEXT_FRAME,
		 MSG_REQUEST_RESEND,
		 MSG_TELL_SEND_TIME,
		 MSG_SET_SPEED,
		 MSG_TELL_RANDOM_SEED,
		 MSG_REQUEST_SAVE,
		 MSG_PLAYER_QUIT,

		 MSG_UNIT_STOP,
		 MSG_UNIT_MOVE,
		 MSG_UNIT_SET_FORCE_MOVE,
		 MSG_UNIT_ATTACK,
		 MSG_UNIT_ASSIGN,
		 MSG_UNIT_CHANGE_NATION,
		 MSG_UNIT_BUILD_FIRM,
		 MSG_UNIT_BURN,
		 MSG_UNITS_SETTLE,
		 MSG_UNIT_SET_GUARD,
		 MSG_UNIT_SET_RANK,
		 MSG_UNIT_DISMOUNT,
		 MSG_UNIT_REWARD,
		 MSG_UNITS_TRANSFORM,
		 MSG_UNIT_RESIGN,
		 MSG_UNITS_ASSIGN_TO_SHIP,
		 MSG_UNITS_SHIP_TO_BEACH,
		 MSG_UNIT_SUCCEED_KING,
		 MSG_UNITS_RETURN_CAMP,
		 MSG_UNITS_FORMATION,
		 MSG_UNITS_FORM_TURN,
		 MSG_UNIT_USE_ITEM,
		 MSG_UNIT_DROP_ITEM,
		 MSG_UNIT_HIRE_HERO,
		 MSG_U_CARA_CHANGE_GOODS,
		 MSG_U_CARA_SET_STOP,
		 MSG_U_CARA_DEL_STOP,
		 MSG_U_CARA_SELECTED,
		 MSG_U_CARA_SET_AUTO,
		 MSG_U_CARA_DUPLICATE,
		 MSG_U_SHIP_UNLOAD_UNIT,
		 MSG_U_SHIP_UNLOAD_ALL_UNITS,
		 MSG_U_SHIP_CHANGE_GOODS,
		 MSG_U_SHIP_SET_STOP,
		 MSG_U_SHIP_DEL_STOP,
		 MSG_U_SHIP_CHANGE_MODE,
		 MSG_U_SHIP_SELECTED,
		 MSG_U_GOD_CAST,
		 MSG_UNIT_SPY_NATION,
		 MSG_UNIT_SPY_NOTIFY_CLOAKED_NATION,
		 MSG_UNIT_CHANGE_AGGRESSIVE_MODE,
		 MSG_SPY_CHANGE_NOTIFY_FLAG,
		 MSG_SPY_ASSASSINATE,
		 MSG_UNITS_ADD_WAY_POINT,
		 MSG_UNITS_CLEAR_HAS_WAY_POINT,
		 MSG_UNIT_AUTO_RETREAT,
		 MSG_UNITS_TRANSFORM_MFORT,
		 MSG_UNITS_GO_TRANSFORM_MFORT,
		 
		 MSG_FIRM_SELL,
		 MSG_FIRM_CANCEL,
		 MSG_FIRM_DESTRUCT,
		 MSG_FIRM_SET_REPAIR,
		 MSG_FIRM_TRAIN_LEVEL,
		 MSG_FIRM_MOBL_WORKER,
		 MSG_FIRM_MOBL_OVERSEER,
		 MSG_FIRM_MOBL_BUILDER,
		 MSG_FIRM_TOGGLE_LINK_FIRM,
		 MSG_FIRM_TOGGLE_LINK_TOWN,
		 MSG_FIRM_PULL_TOWN_PEOPLE,
		 MSG_FIRM_SET_WORKER_HOME,
		 MSG_FIRM_SET_ACTIVE_LINK,
		 MSG_FIRM_BRIBE,
		 MSG_FIRM_CAPTURE,
		 MSG_FIRM_REWARD,
		 MSG_FIRM_RECRUIT,
		 MSG_FIRM_SET_WORKER_COUNT,
		 MSG_FIRM_UPGRADE,
		 MSG_FIRM_SET_RALLY_DEST,
		 MSG_FIRM_CLEAR_RALLY_DEST,

		 MSG_F_CAMP_PATROL,
		 MSG_F_CAMP_TOGGLE_PATROL,
		 MSG_F_CAMP_PROMOTE,
		 MSG_F_CAMP_SWAP_ITEM,
		 MSG_F_INN_HIRE,
		 MSG_F_INN_BUY_ITEM,
		 MSG_F_MARKET_SCRAP,
		 MSG_F_MARKET_HIRE_CARA,
		 MSG_F_RESEARCH_START,
		 MSG_F_WAR_BUILD_WEAPON,
		 MSG_F_WAR_CANCEL_WEAPON,
		 MSG_F_WAR_SKIP_WEAPON,
		 MSG_F_HARBOR_BUILD_SHIP,
		 MSG_F_HARBOR_SAIL_SHIP,
		 MSG_F_HARBOR_SKIP_SHIP,
		 MSG_F_FACTORY_CHG_PROD,
		 MSG_F_FACTORY_AUTO,
		 MSG_F_BASE_MOBL_PRAYER,
		 MSG_F_BASE_INVOKE_GOD,
		 MSG_F_BASE_INVOKE_EFFECT,
		 MSG_F_FORT_TARGET_ARCHER,
		 MSG_F_CANCEL_TRAIN,
		 MSG_F_LAIR_BUY_TECH,
		 MSG_F_LAIR_GRANT,
		 MSG_F_ALCH_CONVERSE,
		 MSG_F_MAGIC_CAST,
		 MSG_F_MAGIC_TRANSFER,
		 MSG_F_LISH_ATTACK,
		 MSG_F_OFF2_ATTACK,

		 MSG_TOWN_RECRUIT,
		 MSG_TOWN_SKIP_RECRUIT,
		 MSG_TOWN_MIGRATE,
		 MSG_TOWN_COLLECT_TAX,
		 MSG_TOWN_REWARD,
		 MSG_TOWN_TOGGLE_LINK_FIRM,
		 MSG_TOWN_TOGGLE_LINK_TOWN,
		 MSG_TOWN_AUTO_TAX,
		 MSG_TOWN_AUTO_GRANT,
		 MSG_TOWN_GRANT_INDEPENDENT,
		 MSG_TOWN_SELL,
		 MSG_TOWN_TARGET_WALL,
		 MSG_TOWN_SET_REPAIR,

		 MSG_WALL_BUILD,
		 MSG_WALL_DESTRUCT,

		 MSG_SPY_CYCLE_ACTION,
		 MSG_SPY_LEAVE_TOWN,
		 MSG_SPY_LEAVE_FIRM,
		 MSG_SPY_CAPTURE_FIRM,
		 MSG_SPY_DROP_IDENTITY,
		 MSG_SPY_REWARD,
		 MSG_SPY_EXPOSED,
		 MSG_SPY_CAMOUFLAGE,
		 MSG_SPY_STEAL_TECH,
		 MSG_SPY_CREATE_INCIDENT,

		 MSG_SEND_TALK_MSG,			// for diplomacy
		 MSG_REPLY_TALK_MSG,
		 MSG_NATION_CONTACT,
		 MSG_NATION_SET_SHOULD_ATTACK,
		 MSG_NATION_RESEARCH,
		 MSG_NATION_REPAIR_ALL_BUILDING,
		 MSG_CHAT,

		 MSG_COMPARE_NATION,
		 MSG_COMPARE_UNIT,
		 MSG_COMPARE_FIRM,
		 MSG_COMPARE_TOWN,
		 MSG_COMPARE_BULLET,
		 MSG_COMPARE_REBEL,
		 MSG_COMPARE_SPY,
		 MSG_COMPARE_SITE,
		 MSG_COMPARE_TALK,

		 LAST_REMOTE_MSG_ID			// keep this item last
	  };

enum { REMOTE_MSG_TYPE_COUNT= LAST_REMOTE_MSG_ID - FIRST_REMOTE_MSG_ID };



//--------- Define struct RemoteMsg ---------//

struct RemoteMsg
{
public:
	DWORD	id;
	char  data_buf[1];

public:
	void	process_msg();

	//------ remote message processing functions ------//

	void	queue_header();
	void	queue_trailer();
	void 	new_nation();
	void	update_game_setting();
	void	start_game();
	void	next_frame();
	void  request_resend();
	void	tell_send_time();
	void  set_speed();
	void	tell_random_seed();
	void	request_save_game();
	void	player_quit();

	void	unit_stop();
	void	unit_move();
	void	unit_set_force_move();
	void	unit_attack();
	void	unit_assign();
	void  unit_change_nation();

	void	unit_build_firm();
	void	unit_burn();
	void	units_settle();
	void	unit_set_guard();
	void	unit_set_rank();
	void	unit_dismount();
	void	unit_reward();
	void	units_transform();
	void	unit_resign();
	void	units_assign_to_ship();
	void	units_ship_to_beach();
	void	unit_succeed_king();
	void	units_return_camp();
	void	units_formation();
	void	units_form_turn();
	void	unit_use_item();
	void	unit_drop_item();
	void	unit_hire_hero();
	void	caravan_change_goods();
	void	caravan_set_stop();
	void	caravan_del_stop();
	void	caravan_selected();
	void	caravan_set_auto();
	void	caravan_duplicate();
	void	ship_unload_unit();
	void	ship_unload_all_units();
	void	ship_change_goods();
	void	ship_set_stop();
	void	ship_del_stop();
	void	ship_change_mode();
	void	ship_selected();
	void	god_cast();
	void	change_spy_nation();
	void	notify_cloaked_nation();
	void  unit_change_behavior_mode();
	void  spy_change_notify_flag();
	void  spy_assassinate();
	void	units_add_way_point();
	void	units_clear_has_way_point();
	void	unit_auto_retreat();
	void	units_transform_mfort();
	void	units_go_transform_mfort();

	void	firm_sell();
	void	firm_cancel();				// cancel construction
	void	firm_destruct();
	void	firm_set_repair();
	void	firm_train_level();
	void	mobilize_worker();
	void	mobilize_overseer();
	void	mobilize_builder();
	void	firm_toggle_link_firm();
	void	firm_toggle_link_town();
	void	firm_pull_town_people();
	void	firm_set_worker_home();
	void	firm_set_active_link();
	void	firm_bribe();
	void	firm_capture();
	void	firm_reward();
	void	firm_recruit();
	void	firm_worker_count();
	void	firm_upgrade();
	void	firm_set_rally_dest();
	void	firm_clear_rally_dest();

	void	camp_patrol();
	void	toggle_camp_patrol();
	void	camp_promote();
	void	camp_swap_item();
	void	inn_hire();
	void	inn_buy_item();
	void	market_scrap();
	void	market_hire_caravan();
	void	research_start();
	void	build_weapon();
	void	cancel_weapon();
	void	skip_build_weapon();
	void	build_ship();
	void	sail_ship();
	void	skip_build_ship();
	void	factory_change_product();
	void	factory_auto_switch();
	void	base_mobilize_prayer();
	void	invoke_god();
	void	invoke_effect();
	void	set_target_archer();
	void	cancel_train();
	void	buy_tech();
	void	lair_grant();
	void	change_conversion();
	void	magic_cast();
	void	magic_transfer();
	void  lishorr_attack();
	void  offensive2_attack();

	void	town_recruit();
	void	town_skip_recruit();
	void	town_migrate();
	void	town_collect_tax();
	void	town_reward();
	void	town_toggle_link_firm();
	void	town_toggle_link_town();
	void	town_auto_tax();
	void	town_auto_grant();
	void	town_grant_independent();
	void	town_sell();
	void	town_set_target_wall();
	void	town_set_repair();

	void	wall_build();
	void	wall_destruct();

	void	spy_cycle_action();
	void	spy_leave_town();
	void	spy_leave_firm();
	void	spy_capture_firm();
	void	spy_drop_identity();
	void	spy_reward();
	void	spy_exposed();
	void	spy_camouflage();
	void	spy_steal_tech();
	void	spy_create_incident();

	void	send_talk_msg();
	void	reply_talk_msg();
	void	nation_contact();
	void	nation_set_should_attack();
	void	nation_research();
	void	nation_repair_all_building();

	void	chat();

	void	compare_remote_object();
};

//----------- Define class Remote -----------//

class MultiPlayerType;

class Remote
{
	enum { COMMON_MSG_BUF_SIZE	    	 = 1024,			// Remote allocates a common RemoteMsg object with a data buffer of this size.
			 SEND_QUEUE_BUF_SIZE		  	 = 1024, 		// The default queue buffer size
			 SEND_QUEUE_BUF_INC_SIZE 	 = 1024,			// If the queue is full, expand with this size
			 RECEIVE_QUEUE_BUF_SIZE		 = 8192, 		// The default queue buffer size
			 RECEIVE_QUEUE_BUF_INC_SIZE = 2048,			// If the queue is full, expand with this size
			 MAX_PROCESS_FRAME_DELAY = 5,					// process player action 1 frame later
			 SEND_QUEUE_BACKUP = MAX_PROCESS_FRAME_DELAY+4,
			 RECEIVE_QUEUE_BACKUP = (MAX_PROCESS_FRAME_DELAY+1+4)*2,
		  };

public:
	char					is_host;
	char					handle_vga_lock;
	int					connectivity_mode;
	int					poll_msg_flag;
	MultiPlayerType*	mp_ptr;

	//--------- send queue -----------//

	RemoteQueue		send_queue[SEND_QUEUE_BACKUP];		// 0 for the latest, other for backup
	DWORD				send_frame_count[SEND_QUEUE_BACKUP];

	//------- receive queue ---------//

	RemoteQueue		receive_queue[RECEIVE_QUEUE_BACKUP];
	DWORD				receive_frame_count[RECEIVE_QUEUE_BACKUP];

	char				process_queue_flag;

	//-------------------------------//

	int				packet_send_count;
	int				packet_receive_count;

	//-------------------------------//
	short				nation_processing;		// used in process_receive_queue

	char				save_file_name[MAX_PATH+1];

	char				*common_msg_buf;
	char				sync_test_level;			// 0=disable, bit0= random seed, bit1=crc
	int				process_frame_delay;

	// --------- alternating send frame --------//
	int				alternating_send_rate;	// 1=every frame, 2=send one frame per two frames...

public:
	Remote();
	~Remote();

	void			init(MultiPlayerType *);
	void			deinit();

	void			init_start_mp();
	int			is_enable();
	int			is_lobbied();		// 0 - not lobbied, 1 - create game, 2 - join game
	int			number_of_opponent();
	DWORD			self_player_id();

	int			create_game();
	int			connect_game();
	void			start_game();

	void 			send_msg(RemoteMsg* remoteMsgPtr, int receiverId=0);
	void 			send_free_msg(RemoteMsg* remoteMsgPtr, int receiverId=0);

	RemoteMsg* 	new_msg(int msgId, int dataSize);
	void 			free_msg(RemoteMsg* remoteMsgPtr);

	char* 	 	new_send_queue_msg(int msgId, int msgSize);
	int			send_queue_now(int receiverId=0);
	int			send_backup_now(int receiverId, DWORD requestFrameCount);
	void 			append_send_to_receive();
	void			copy_send_to_backup();
	int			poll_msg();
	void			enable_poll_msg();
	void			disable_poll_msg();

	void 			process_receive_queue();
	void 			process_specific_msg(DWORD msgId);

	void			init_send_queue(DWORD,short);
	void			init_receive_queue(DWORD);

	void			enable_process_queue();
	void			disable_process_queue();

	void			reset_process_frame_delay();
	int			get_process_frame_delay();
	void			set_process_frame_delay(int);
	int			calc_process_frame_delay(int milliSecond);

	// ------- alternating send frame -------//
	void			set_alternating_send(int rate);
	int			get_alternating_send();
	int			has_send_frame(int nationRecno, DWORD frameCount);
	DWORD			next_send_frame(int nationRecno, DWORD frameCount);
};

extern Remote remote;

//------------------------------------//

#endif