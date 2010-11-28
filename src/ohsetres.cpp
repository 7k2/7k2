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

//Filename    : OHSETRES.CPP
//Description : Hill Set, Pattern resource object
//Ownership   : Gilbert

#include <ohsetres.h>
#include <all.h>
#include <osys.h>
#include <ovga.h>
#include <ogameset.h>
#include <oworld.h>

//---------- #define constant ------------//

#define EQPATTERN_DB		"EQHILL"
#define HSET_DB			"HSET"
#define HILLSET_DB		"HILLSET"

//------- Begin of function HSetRes::HSetRes -----------//

HSetRes::HSetRes()
{
	init_flag=0;
}
//--------- End of function HSetRes::HSetRes -----------//


//---------- Begin of function HSetRes::init -----------//
//
// This function must be called after a map is generated.
//
void HSetRes::init(unsigned maxPatternId)
{
	deinit();

	//------- load database information --------//

	max_pattern_id = maxPatternId;
	load_eqv_pattern_info(maxPatternId);
	load_hset_info();
	load_hill_set_info();
	init_flag=1;
}
//---------- End of function HSetRes::init -----------//


//---------- Begin of function HSetRes::deinit -----------//

void HSetRes::deinit()
{
	if( init_flag )
	{
		mem_del(hill_set_info_array);
		mem_del(hset_info_array);
		mem_del(eqv_pattern_count);
		mem_del(eqv_pattern_id);
		init_flag=0;
	}
}
//---------- End of function HSetRes::deinit -----------//


//------- Begin of function HSetRes::load_eqv_pattern_info -------//
//
void HSetRes::load_eqv_pattern_info(unsigned maxPatternId)
{
	int			i;
	
	//---- read in record count and initialize arrays ----//

	Database *dbEqPattern = game_set.open_db(EQPATTERN_DB);	// only one database can be opened at a time

	short record_count = (short) dbEqPattern->rec_count();
	eqv_pattern_count = (unsigned char *) mem_add(sizeof(unsigned char) * max_pattern_id);
	eqv_pattern_id = (unsigned char *)mem_add(sizeof(unsigned char) * max_pattern_id * MAX_EQV_PATTERN);

	memset(eqv_pattern_count, 0, sizeof(unsigned char) * maxPatternId);
	memset(eqv_pattern_id, 0, sizeof(unsigned char) * maxPatternId * MAX_EQV_PATTERN);
	
	//---------- read in EQHILL.DBF ---------//

	for( i=0 ; i< record_count ; i++ )
	{
		EqPatternRec *eqPatternRec = (EqPatternRec*) dbEqPattern->read(i+1);
		unsigned char patternId = (unsigned char) m.atoi(eqPatternRec->pattern_id, eqPatternRec->PATTERN_ID_LEN);
		unsigned char eqPatternId = (unsigned char) m.atoi(eqPatternRec->eqv_pattern_id, eqPatternRec->PATTERN_ID_LEN);

		if( eqv_pattern_count[patternId-1] < MAX_EQV_PATTERN )
		{
			eqv_pattern_id[MAX_EQV_PATTERN * (patternId-1) + eqv_pattern_count[patternId-1]]
				= eqPatternId;
			eqv_pattern_count[patternId-1]++;
		}
		// 5th similar pattern is discarded
	}
}
//--------- End of function HSetRes::load_eqv_pattern_info ---------//

//------- Begin of function HSetRes::load_hset_info -------//
//
void HSetRes::load_hset_info()
{
	HSetRec	*hSetRec;
	HSetInfo	*hSetInfo;
	int			i;
	
	//---- read in record count and initialize arrays ----//

	Database *dbHSet = game_set.open_db(HSET_DB);	// only one database can be opened at a time

	hset_info_count = (short) dbHSet->rec_count();

	hset_info_array = (HSetInfo *) mem_add(sizeof(HSetInfo) * hset_info_count);
	memset( hset_info_array, 0, sizeof(HSetInfo) *hset_info_count);
	
	//---------- read in HSET.DBF ---------//

	for( i=0 ; i< hset_info_count ; i++ )
	{
		hSetRec = (HSetRec *) dbHSet->read(i+1);

		short j = (short) m.atoi(hSetRec->set_id, hSetRec->SET_ID_LEN);
		if( j > hset_info_count)
		{
			err_when( j > hset_info_count);
			break;
		}

		hSetInfo = hset_info_array + (j-1);
		hSetInfo->set_id = j;
		switch( hSetRec->side[0] )
		{
		case 'N':
			switch(hSetRec->side[1])
			{
			case 'E':
				hSetInfo->side = 2;
				break;
			case 'W':
				hSetInfo->side = 8;
				break;
			default:
				hSetInfo->side = 1;
			}
			break;
		case 'E':
			hSetInfo->side = 3;
			break;
		case 'S':
			switch(hSetRec->side[1])
			{
			case 'E':
				hSetInfo->side = 4;
				break;
			case 'W':
				hSetInfo->side = 6;
				break;
			default:
				hSetInfo->side = 5;
			}
			break;
		case 'W':
			hSetInfo->side = 7;
			break;
		case '4':
			hSetInfo->side = 9;		// a standalone hill2
			break;
		default:
			hSetInfo->side = 0;
		}

		hSetInfo->replaced_length = (char) m.atoi(hSetRec->replaced_length, hSetRec->LENGTH_LEN);
		hSetInfo->replaced_length2= (char) m.atoi(hSetRec->replaced_length2, hSetRec->LENGTH_LEN);
		hSetInfo->replaced_width = (char) m.atoi(hSetRec->replaced_width, hSetRec->LENGTH_LEN);
		hSetInfo->replaced_width2 = (char) m.atoi(hSetRec->replaced_width2, hSetRec->LENGTH_LEN);
		hSetInfo->step_num = (char) m.atoi(hSetRec->step_num, hSetRec->STEP_LEN);
		hSetInfo->first_hill_set = NULL;
	}
}
//--------- End of function HSetRes::load_hset_info ---------//

//------- Begin of function HSetRes::load_hill_set_info -------//
//
void HSetRes::load_hill_set_info()
{
	HillSetRec	*hillSetRec;
	HillSetInfo	*hillSetInfo;
	int			i;
	
	//---- read in record count and initialize arrays ----//

	Database *dbHillSet = game_set.open_db(HILLSET_DB);	// only one database can be opened at a time

	hill_set_info_count = (short) dbHillSet->rec_count();

	hill_set_info_array = (HillSetInfo *) mem_add(sizeof(HillSetInfo) * hill_set_info_count);
	memset( hill_set_info_array, 0, sizeof(HillSetInfo) * hill_set_info_count);
	
	//---------- read in HILLSET.DBF ---------//

	for( i=0, hillSetInfo=hill_set_info_array ; i< hill_set_info_count ; i++, hillSetInfo++ )
	{
		hillSetRec = (HillSetRec *) dbHillSet->read(i+1);

		short j = (short) m.atoi(hillSetRec->set_id, hillSetRec->SET_ID_LEN);
		if( j > hset_info_count)
		{
			err_when( j > hset_info_count);
			break;
		}

		hillSetInfo->set_id = j;

		switch( hillSetRec->post_move[0] )
		{
		case 'N':
			switch(hillSetRec->post_move[1])
			{
			case 'E':
				hillSetInfo->post_move = 2;
				break;
			case 'W':
				hillSetInfo->post_move = 8;
				break;
			default:
				hillSetInfo->post_move = 1;
			}
			break;
		case 'E':
			hillSetInfo->post_move = 3;
			break;
		case 'S':
			switch(hillSetRec->post_move[1])
			{
			case 'E':
				hillSetInfo->post_move = 4;
				break;
			case 'W':
				hillSetInfo->post_move = 6;
				break;
			default:
				hillSetInfo->post_move = 5;
			}
			break;
		case 'W':
			hillSetInfo->post_move = 7;
			break;
		default:
			hillSetInfo->post_move = 0;
		}

		hillSetInfo->pattern_id = (unsigned char) m.atoi(hillSetRec->pattern_id,
			hillSetRec->PATTERN_ID_LEN);
		hillSetInfo->step = (char) m.atoi(hillSetRec->step, hillSetRec->STEP_LEN);
		hillSetInfo->next_hill_set = NULL;

		// ----------- link from previous one to this -----------//
		if( hillSetInfo->step == 1)
		{
			// ---------- link from HSetInfo to this
			err_when( hset_info_array[j-1].set_id != j ||
				hset_info_array[j-1].set_id != hillSetInfo->set_id);
			hset_info_array[j-1].first_hill_set = hillSetInfo;
		}
		else
		{
			// ---------- search backward for previous one
			int k;
			for(k = i-1; k >= 0; --k)
			{
				if( hill_set_info_array[k].set_id == hillSetInfo->set_id &&
					hill_set_info_array[k].step == hillSetInfo->step -1)
				{
					err_when( hill_set_info_array[k].next_hill_set);	// linked already
					hill_set_info_array[k].next_hill_set = hillSetInfo;
					break;
				}
			}
			err_when(k < 0);		// not found
		}
	}
}
//--------- End of function HSetRes::load_hill_set_info ---------//

//------------ Begin of function HSetRes::get_eqv_pattern_count ------//
unsigned char HSetRes::get_eqv_pattern_count(unsigned char patternId)
{
	err_when( !init_flag || patternId <= 0 || patternId > max_pattern_id);
	return eqv_pattern_count[patternId-1];
}
//------------ End of function HSetRes::get_eqv_pattern_count ------//


//------------ Begin of function HSetRes::get_eqv_pattern_id ------//
unsigned char HSetRes::get_eqv_pattern_id(unsigned char patternId, unsigned char seqno)
{
	err_when( seqno < 0 || seqno >= MAX_EQV_PATTERN ||
		seqno >= get_eqv_pattern_count(patternId) );
	return eqv_pattern_id[MAX_EQV_PATTERN * (patternId-1) + seqno];
}
//------------ End of function HSetRes::get_eqv_pattern_id ------//

//------------ Begin of function HSetRes::get_random_pattern ------//
//
// randomly select a similar pattern or the same pattern
//
unsigned char HSetRes::get_random_pattern(unsigned char patternId)
{
	err_when( !init_flag || patternId <= 0 || patternId > max_pattern_id);
	unsigned char c = eqv_pattern_count[patternId-1];
	unsigned char d;
	if( (d = (unsigned char)m.random(c+1)) != 0)
		return get_eqv_pattern_id(patternId, d-1);
	else
		return patternId;
}
//------------ End of function HSetRes::get_random_pattern ------//


//------------ Begin of function HSetRes::random_hset ------//
HSetInfo *HSetRes::random_hset(char isSide, char maxLength, char maxWidth, char maxLength2, char maxWidth2)
{
	HSetInfo *lastResult = NULL;
	short		hSetCount = 0;

	err_when(!init_flag || hset_info_count <= 0);

	for(int i = 0; i < hset_info_count; ++i)
	{
		HSetInfo *hSetInfo = hset_info_array + i;
		if( hSetInfo->side == isSide && 
			hSetInfo->replaced_length <= maxLength &&
			hSetInfo->replaced_width <= maxWidth &&
			hSetInfo->replaced_length2 <= maxLength2 &&
			hSetInfo->replaced_width2 <= maxWidth2)
		{
			++hSetCount;

			// ------ replace the lastResult ---------//
			// chance of replace is 1/(hSetCount)
			// all found records have equal chance to be selected
			if( m.random(hSetCount) == 0)
				lastResult = hSetInfo;
		}
	}

	return lastResult;
}
//------------ End of function HSetRes::random_hset ------//

