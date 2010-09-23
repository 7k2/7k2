//Filename    : OMONSRES.H
//Description : Header file of object RaceRes

#ifndef __OMONSRES_H
#define __OMONSRES_H

#ifndef __ORESDB_H
#include <ORESDB.H>
#endif

//----------- Define monster types ------------//

#define MAX_MONSTER_TYPE	7

enum { MONSTER_KHARSHUF=1,
		 MONSTER_BREGMA,
		 MONSTER_KERASSOS,
		 MONSTER_MINOTAUROS,
		 MONSTER_GROKKEN,
		 MONSTER_EZPINEZ,
		 MONSTER_EXOVUM
	  };

//----------- Define constant ------------//

#define MONSTER_NAME_COUNT 	2000		// A maximum of 2000 different monster names in each game

#define REPUTATION_INCREASE_PER_ATTACK_MONSTER ((float)0.001)

//------------ Define struct MonsterRec ---------------//

struct MonsterRec
{
	enum { UNIT_CODE_LEN=8, RANK_LEN=8, NAME_LEN=20, FIRM_BUILD_CODE_LEN=8, UNIT_ID_LEN=3 };

	char 	unit_code[UNIT_CODE_LEN];
	char  name[NAME_LEN];
	char  level;				// the level of the 1-9 monster, the higher the level is, the more powerful the monster is.
	char 	firm_build_code[FIRM_BUILD_CODE_LEN];
	char 	unit_id[UNIT_ID_LEN];
};

//------------- Define struct MonsterInfo --------------//

struct MonsterInfo
{
public:
	//------- constant vars --------//

	enum { NAME_LEN=20, FIRM_BUILD_CODE_LEN=8 };

	short monster_id;

	char	name[NAME_LEN+1];
	short	unit_id;
	char	level;
	char  firm_build_code[FIRM_BUILD_CODE_LEN+1];

	// ##### patch begin Gilbert 26/10 ######//
	char	adjective[NAME_LEN+1];
	// ##### patch end Gilbert 26/10 ######//
};

//------------ Define struct MonsterNameRec ------------//

struct MonsterNameRec
{
	enum { NAME_LEN=3 };
	char	name[NAME_LEN];
};

//------------- Define struct MonsterName --------------//

struct MonsterName
{
	enum { NAME_LEN=20 };
	char	name[NAME_LEN+1];
};

//----------- Define class MonsterRes ---------------//

class MonsterRes
{
public:
	char        	init_flag;

	short       	monster_count;
	MonsterInfo*   monster_info_array;

	MonsterName 	name_array[MONSTER_NAME_COUNT];
	char 				name_used_array[MONSTER_NAME_COUNT];

	ResourceDb  	res_bitmap;

public:
	MonsterRes();

	void        init();
	void        deinit();

	void  		second_init();

	char* 		get_name(WORD nameId);
	WORD			get_new_name_id();
	void			free_name_id(WORD nameId);
	void			use_name_id(WORD nameId);
	long			sum_name_used();		// for sum of name_used_array, checking

	int 			write_file(File* filePtr);
	int			read_file(File* filePtr);

	MonsterInfo* operator[](int monsterId);      // pass monsterId  as recno

private:
	void        load_monster_info();
};

extern MonsterRes monster_res;

//----------------------------------------------------//

#endif
