//Filename    : GAMEDEF.H
//Description : Maximum game constant

#ifndef __GAMEDEF_H
#define __GAMEDEF_H

//----------- define game version ------------//

//#define GAME_VERSION_STR   "1.1"
extern char *GAME_VERSION_STR;
//#define GAME_VERSION       110          // Version 1.00, don't change it unless the format of save game files has been changed
extern const int GAME_VERSION;

//-------- Define constant ------------//

#define APP_NAME           "7K2"                         // application name
#define WIN_CLASS_NAME     "Seven_Kingdoms_2"            // for registering window class
#define WIN_TITLE          "Seven Kingdoms 2"            // window title

//------- Data sub-directories -------//

#define DIR_RES         "RESOURCE\\"
#define DIR_SPRITE      "SPRITE\\"
#define DIR_FIRM			"FIRM\\"
#define DIR_FIRM_SMALL	"FIRMSML\\"
#define DIR_SOUND       "SOUND\\"
#define DIR_SAVE        "SAVE\\"
#define DIR_IMAGE       sys.dir_image
#define DIR_ENCYC       sys.dir_encyc
#define DIR_ENCYC2		sys.dir_encyc2
#define DIR_MUSIC       sys.dir_music
#define DIR_MOVIE       sys.dir_movie
#define DIR_TUTORIAL		sys.dir_tutorial
#define DIR_SCENARIO		sys.dir_scenario
#define DIR_CAMPAIGN		sys.dir_campaign
// see MAX_SCENARIO_PATH in sys
#define DIR_SCENARIO_PATH(p)	sys.dir_scenario_path[p]

//--------- Define direction types -----------//

enum { MAX_DIR=8 };

enum { DIR_N,     // building directions
       DIR_NE,
       DIR_E,
       DIR_SE,
       DIR_S,
       DIR_SW,
       DIR_W,
       DIR_NW
     };

//------------ Mobile Types -------------//

enum { UNIT_AIR ='A',
		 UNIT_LAND='L',
		 UNIT_SEA ='S',
	  };

//---------- Link enable/disable status ---------//

#define LINK_DD   0     // both sides disabled
#define LINK_ED   1     // host side enabled, client side disabled
#define LINK_DE   2     // host side disabled, client side enabled
#define LINK_EE   3     // both sides enabled

//------------- define command types ------------//

enum { COMMAND_PLAYER=0, COMMAND_REMOTE=1, COMMAND_AI, COMMAND_AUTO };

//-------- Define maximum game constant ---------//

#define MAX_RACE                    12

#define MAX_COLOR_SCHEME             7    // Maximum no. of color schemes

#define MAX_NATION                   7

#if(MAX_NATION > MAX_COLOR_SCHEME)
	// MAX_NATION == MAX_COLOR_SCHEME // Maximum no. of nation should be equal to the maximum no. of color scheme-1 (the exclude one is for independent nations)
	#error
#endif

#define MAX_PLAYER                  7

#define MAX_UNIT_ATTACK_TYPE        3

#define MAX_SPRITE_DIR_TYPE         8

#define MAX_LINKED_FIRM_FIRM        23
#define MAX_LINKED_FIRM_TOWN        30
#define MAX_LINKED_TOWN_TOWN        18

#define EFFECTIVE_POWER_DISTANCE    3

#define PROCESS_GOODS_INTERVAL      3     // Process goods in mines, factories and market places once 3 days

#define PEASANT_FOOD_YEAR_PRODUCTION  	30  // A peasant produce 2 units of food per month
#define UNIT_FOOD_YEAR_CONSUMPTION  	10  // consume one unit of food per unit per month

#define NO_FOOD_LOYALTY_DECREASE_INTERVAL	 5  // When your kingdom runs out of food, the loyalty of all your units will decrease 1 point every 5 days.

#define RAW_PRICE       float(1.5)  // Cost per raw material
#define PRODUCT_PRICE   3           // Cost per product
#define CONSUMER_PRICE  float(4.5)  // Cost per product for consumers

#define REWARD_COST                 30
#define REWARD_LOYALTY_INCREASE     10

#define UNIT_BETRAY_LOYALTY         30 // if a unit's loyalty is below this, it will betray

#define PROMOTE_LOYALTY_INCREASE    20
#define DEMOTE_LOYALTY_DECREASE     40

#define ATTACK_SLOW_DOWN            3  // how many times attacking damages should be reduced, this lessens all attacking damages, thus slowing down all battles.

#define SPY_KILLED_REPUTATION_DECREASE		3

#define REPAIR_POINTS_PER_DAY       1.0f		// points can be repaired per day for firms and towns

#define INTER_PLACE_SPACE    			2			// the no. of space locations between places (including firms and towns)

#define MAX_ROYAL_UNIT	   			10			// the max no. of royal units

//-----------------------------------------------//

#endif
