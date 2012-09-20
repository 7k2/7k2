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

//Filename    : OMONSRES.CPP
//Description : Monster resource class

#include <osys.h>
#include <ogameset.h>
#include <oworld.h>
#include <osprtres.h>
#include <ounit.h>
#include <ounitres.h>
#include <otown.h>
#include <ofirm.h>
#include <omonsres.h>
#include <otransl.h>

//---------- #define constant ------------//

#define MONSTER_DB   			"MONSTER"
#define MONSTER_NAME_A_DB     "MNAMEA"
#define MONSTER_NAME_B_DB     "MNAMEB"
#define MONSTER_NAME_C_DB     "MNAMEC"

//------- Begin of function MonsterRes::MonsterRes -----------//

MonsterRes::MonsterRes()
{
   init_flag=0;
}
//--------- End of function MonsterRes::MonsterRes -----------//


//---------- Begin of function MonsterRes::init -----------//
//
// This function must be called after a map is generated.
//
void MonsterRes::init()
{
   deinit();

   //------- load database information --------//

	load_monster_info();

   init_flag=1;
}
//---------- End of function MonsterRes::init -----------//


//---------- Begin of function MonsterRes::deinit -----------//

void MonsterRes::deinit()
{
   if( init_flag )
   {
      mem_del(monster_info_array);
      init_flag=0;
   }
}
//---------- End of function MonsterRes::deinit -----------//


//------- Begin of function MonsterRes::load_monster_info -------//
//
// Read in information of MONSTER.DBF into memory array
//
void MonsterRes::load_monster_info()
{
	MonsterRec  *monsterRec;
	MonsterInfo *monsterInfo;
	int         i;
	Database    *dbMonster = game_set.open_db(MONSTER_DB);

	monster_count      = (short) dbMonster->rec_count();
	monster_info_array = (MonsterInfo*) mem_add( sizeof(MonsterInfo)*monster_count );

	err_when( monster_count != MAX_MONSTER_TYPE );

	//------ read in monster information array -------//

	memset( monster_info_array, 0, sizeof(MonsterInfo) * monster_count );

	for( i=0 ; i<monster_count ; i++ )
	{
		monsterRec  = (MonsterRec*) dbMonster->read(i+1);
		monsterInfo = monster_info_array+i;

		monsterInfo->monster_id = i+1;

      misc.rtrim_fld( monsterInfo->name, monsterRec->name, monsterRec->NAME_LEN );
		translate.multi_to_win(monsterInfo->name, monsterInfo->NAME_LEN);

		monsterInfo->unit_id      = misc.atoi(monsterRec->unit_id     , monsterRec->UNIT_ID_LEN);
		monsterInfo->level        = monsterRec->level - '0';

		misc.rtrim_fld( monsterInfo->firm_build_code, monsterRec->firm_build_code, monsterRec->FIRM_BUILD_CODE_LEN );

		strcpy( monsterInfo->adjective, monsterInfo->name );

		// ##### patch begin Gilbert 26/10 ######//
		// FRENCH has different adjective
#if(defined(FRENCH))
		switch(monsterInfo->monster_id)
		{
		case MONSTER_KHARSHUF:
		case MONSTER_BREGMA:
		case MONSTER_GROKKEN:
		case MONSTER_EXOVUM:
			strcat( monsterInfo->adjective, "s" );
			break;
		}
#endif
	// ##### patch end Gilbert 26/10 ######//
   }
}
//--------- End of function MonsterRes::load_monster_info ---------//


//---------- Begin of function MonsterRes::operator[] -----------//

MonsterInfo* MonsterRes::operator[](int monsterId)
{
	err_if( monsterId<1 || monsterId>monster_count )
		err_now( "MonsterRes::operator[]" );

	return monster_info_array+monsterId-1;
}
//------------ End of function MonsterRes::operator[] -----------//


//--------- Begin of function MonsterRes::second_init -------//

void MonsterRes::second_init()
{
	// ###### begin Gilbert 14/3 ########//
	long backupSeed = misc.get_random_seed();	// backup random seed in order to use a fixed random seed
	misc.set_random_seed(5);
	// ###### end Gilbert 14/3 ########//

	struct MonsterNamePart
	{
		enum { NAME_LEN=3 };
		char	name[NAME_LEN+1];
	};

	MonsterNameRec  *nameRec;

   //-------- read name part A --------//

	Database *dbNameA   = game_set.open_db(MONSTER_NAME_A_DB);
	int 		nameACount = (short) dbNameA->rec_count();

	MonsterNamePart* nameArrayA = (MonsterNamePart*) mem_add( sizeof(MonsterNamePart) * nameACount );

	int i;
	for( i=0 ; i<nameACount ; i++ )
	{
		nameRec = (MonsterNameRec*) dbNameA->read( misc.random(nameACount)+1 );
		misc.rtrim_fld( nameArrayA[i].name, nameRec->name, nameRec->NAME_LEN );
	}

	//-------- read name part B --------//

	Database *dbNameB   = game_set.open_db(MONSTER_NAME_B_DB);
	int 		nameBCount = (short) dbNameB->rec_count();

	MonsterNamePart* nameArrayB = (MonsterNamePart*) mem_add( sizeof(MonsterNamePart) * nameBCount );

	for( i=0 ; i<nameBCount ; i++ )
	{
		nameRec = (MonsterNameRec*) dbNameB->read( misc.random(nameBCount)+1 );
		misc.rtrim_fld( nameArrayB[i].name, nameRec->name, nameRec->NAME_LEN );
	}

	//-------- read name part C --------//

	Database *dbnameC   = game_set.open_db(MONSTER_NAME_C_DB);
	int 		nameCCount = (short) dbnameC->rec_count();

	MonsterNamePart* nameArrayC = (MonsterNamePart*) mem_add( sizeof(MonsterNamePart) * nameCCount );

	for( i=0 ; i<nameCCount ; i++ )
	{
		nameRec = (MonsterNameRec*) dbnameC->read( misc.random(nameCCount)+1 );
		misc.rtrim_fld( nameArrayC[i].name, nameRec->name, nameRec->NAME_LEN );
	}

	//-------------------------------------------//

	enum { MONSTER_NAME_COUNT_FORMAT = 6 };

	static const char* nameFormatArray[MONSTER_NAME_COUNT_FORMAT] =
	{
		"ABABC",
		"ABC",
		"ABCABC",
		"ABAB",
		"BCBC",
		"BCABC"
	};

	memset( name_array, 0, sizeof(name_array) );
	memset( name_used_array, 0, sizeof(name_used_array) );

	//-----------------------------------//

	for( i=0 ; i<MONSTER_NAME_COUNT ; i++ )
	{
		const char* nameFormat = nameFormatArray[ misc.random(MONSTER_NAME_COUNT_FORMAT) ];
		int   strLen = (int) strlen(nameFormat);
		const char* namePtr;

		int j;
		for( j=0 ; j<strLen ; j++ )
		{
			switch( nameFormat[j] )
			{
				case 'A':
					namePtr = nameArrayA[misc.random(nameACount)].name;
					break;

				case 'B':
					namePtr = nameArrayB[misc.random(nameBCount)].name;
					break;

				case 'C':
					namePtr = nameArrayC[misc.random(nameCCount)].name;
					break;

				default:
					err_here();
			}

			strncat( name_array[i].name, namePtr, MonsterName::NAME_LEN );
			name_array[i].name[MonsterName::NAME_LEN] = '\0';
		}

		//--- convert the name to lower cases except the first letter ---//

		strLen = (int) strlen(name_array[i].name);

		for( j=1 ; j<strLen ; j++ )
		{
			name_array[i].name[j] = misc.lower( name_array[i].name[j] );
		}
	}

	mem_del( nameArrayA );
	mem_del( nameArrayB );
	mem_del( nameArrayC );

	// ###### begin Gilbert 14/3 ########//
	// -------- random seed --------//

	misc.set_random_seed(backupSeed);
	// ###### end Gilbert 14/3 ########//
}
//----------- End of function MonsterRes::second_init ---------//



//----- Start of function MonsterRes::get_new_name_id ------//
//
// Return an unused name id. and set the used_count of the
// first and first name to 1.
//
WORD MonsterRes::get_new_name_id()
{
	int i;
	int nameId = misc.random(MONSTER_NAME_COUNT)+1;

	for( i=1 ; i<=MONSTER_NAME_COUNT ; i++ )
	{
		if( ++nameId > MONSTER_NAME_COUNT )
			nameId = 1;

		if( !name_used_array[nameId-1] )
			break;
	}

	err_when( nameId < 1 || nameId > MONSTER_NAME_COUNT );

	name_used_array[nameId-1]++;

	return nameId;
}
//------ End of function MonsterRes::get_new_name_id -------//


//----- Start of function MonsterRes::free_name_id ------//
//
// Free an used name id.
//
// Unit names are freed when they settle into a town.
// But unit names are not freed when they are killed.
//
void MonsterRes::free_name_id(WORD nameId)
{
	err_when( nameId < 1 || nameId > MONSTER_NAME_COUNT );

	name_used_array[nameId-1]--;

	err_when( name_used_array[nameId-1] < 0 );
}
//------ End of function MonsterRes::free_name_id -------//


//----- Start of function MonsterRes::use_name_id ------//
//
// Claim the use a specific name id.
//
void MonsterRes::use_name_id(WORD nameId)
{
	err_when( nameId < 1 || nameId > MONSTER_NAME_COUNT );

	name_used_array[nameId-1]++;
}
//------ End of function MonsterRes::use_name_id -------//


//-------- Start of function MonsterRes::get_name -------------//
//
// <WORD> nameId - name id. of the monster
//
char* MonsterRes::get_name(WORD nameId)
{
	err_when( nameId < 1 || nameId > MONSTER_NAME_COUNT );

	return name_array[nameId-1].name;
}
//--------- End of function MonsterRes::get_name ---------------//


// ----------- begin of function MonsterRes::sum_name_used ------//
// for sum of name_used_array, checking
long MonsterRes::sum_name_used()
{
	long sum = 0;
	for( int i = 0; i < MONSTER_NAME_COUNT; ++i )
	{
		if( name_used_array[i] < 0 )		// skip negative value
		{
			err_here();
			continue;
		}
		sum += name_used_array[i];
	}
	
	return sum;
}
// ----------- end of function MonsterRes::sum_name_used ------//
