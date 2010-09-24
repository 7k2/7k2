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

//Filename    : OTECHRES.CPP
//Description : Tech class

#include <ALL.H>
#include <OGAME.H>
#include <OGAMESET.H>
#include <OMONSRES.H>
#include <OF_RESE.H>
#include <OUNIT.H>
#include <ORESX.H>
#include <ONATION.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <OTRANSL.H>

//---------- #define constant ------------//

#define TECH_DB		  		"TECH"
#define TECH_CLASS_DB 	 	"TECHCLAS"

//------- Begin of function TechRes::TechRes -----------//

TechRes::TechRes()
{
	init_flag=0;
}
//--------- End of function TechRes::TechRes -----------//


//---------- Begin of function TechRes::init -----------//
//
// This function must be called after a map is generated.
//
void TechRes::init()
{
	deinit();

	//------- load database information --------//

	load_tech_class();
	load_tech_info();

	init_flag=1;
}
//---------- End of function TechRes::init -----------//


//---------- Begin of function TechRes::deinit -----------//

void TechRes::deinit()
{
	if( init_flag )
	{
		mem_del(tech_class_array);
		mem_del(tech_info_array);

		init_flag=0;
	}
}
//---------- End of function TechRes::deinit -----------//


//------- Begin of function TechRes::load_tech_class -------//
//
// Read in information of TECHCLAS.DBF into memory array
//
void TechRes::load_tech_class()
{
	TechClassRec  *techClassRec;
	TechClass     *techClass;
	Database *dbTechClass = game_set.open_db(TECH_CLASS_DB);

	tech_class_count = (short) dbTechClass->rec_count();
	tech_class_array = (TechClass*) mem_add( sizeof(TechClass)*tech_class_count );

	err_when( tech_class_count != MAX_TECH_CLASS );

	//------ read in tech information array -------//

	memset( tech_class_array, 0, sizeof(TechClass) * tech_class_count );

	for( int i=0 ; i<tech_class_count ; i++ )
	{
		techClassRec = (TechClassRec*) dbTechClass->read(i+1);
		techClass    = tech_class_array+i;

		techClass->class_id = i+1;
		m.rtrim_fld( techClass->desc, techClassRec->desc, techClassRec->DESC_LEN );
		translate.multi_to_win(techClass->desc, techClass->DESC_LEN);

		techClass->race_id = m.atoi( techClassRec->race_id, techClassRec->RACE_ID_LEN );

#ifdef DEBUG
		// check code name of tech class
		char techClassCode[techClassRec->CODE_LEN+1];
		m.rtrim_fld( techClassCode, techClassRec->class_code, techClassRec->CODE_LEN );
		switch( techClass->class_id )
		{
		case TECH_CLASS_PRODUCTION:
			err_when( strcmp( techClassCode, "PRODUCT") );
			break;
		case TECH_CLASS_SPYING:
			err_when( strcmp( techClassCode, "SPYING") );
			break;
		case TECH_CLASS_INFANTRY:
			err_when( strcmp( techClassCode, "MILITARY") );
			break;
		case TECH_CLASS_SPU_STRUCTURE:
			err_when( strcmp( techClassCode, "SPU_STRU") );
			break;
		case TECH_CLASS_SPU_BERSERKER:
			err_when( strcmp( techClassCode, "SPU_POW") );
			break;
		case TECH_CLASS_WEAPON:
			err_when( strcmp( techClassCode, "WEAPON") );
			break;
		case TECH_CLASS_OFFENSIVE_STRUCTURE:
			err_when( strcmp( techClassCode, "OFF_BLD") );
			break;
		case TECH_CLASS_MEGA_WEAPON:
			err_when( strcmp( techClassCode, "MEGA_WEP") );
			break;
		case TECH_CLASS_PLANT:
			err_when( strcmp( techClassCode, "PLANT") );
			break;
		case TECH_CLASS_INSECT:
			err_when( strcmp( techClassCode, "LYW") );
			break;
		case TECH_CLASS_ANIMAL:
			err_when( strcmp( techClassCode, "HOBGLOB") );
			break;
		case TECH_CLASS_ALCHEMY:
			err_when( strcmp( techClassCode, "GIANTET") );
			break;
		case TECH_CLASS_FORTRESS:
			err_when( strcmp( techClassCode, "GITH") );
			break;
		case TECH_CLASS_INCUBATION:
			err_when( strcmp( techClassCode, "ROCKMAN") );
			break;
		case TECH_CLASS_MAGIC:
			err_when( strcmp( techClassCode, "GREMJERM") );
			break;
		}
#endif

	}
}
//--------- End of function TechRes::load_tech_class ---------//


//------- Begin of function TechRes::load_tech_info -------//
//
// Read in information of TECH.DBF into memory array
//
void TechRes::load_tech_info()
{
	TechRec  *techRec;
	TechInfo *techInfo;
	Database *dbTech = game_set.open_db(TECH_DB);

	tech_count      = (short) dbTech->rec_count();
	tech_info_array = (TechInfo*) mem_add( sizeof(TechInfo)*tech_count );

	err_when( tech_count != MAX_TECH );

	//------ read in tech information array -------//

	memset( tech_info_array, 0, sizeof(TechInfo) * tech_count );

	int 		  techClassId=0;
	TechClass* techClass;

	memset( total_tech_level_array, 0, sizeof(total_tech_level_array) );

	for( int i=0 ; i<tech_count ; i++ )
	{
		techRec  = (TechRec*) dbTech->read(i+1);
		techInfo = tech_info_array+i;

		techInfo->tech_id			 = i+1;
		techInfo->class_id  		 = m.atoi( techRec->class_id		 , techRec->ID_LEN );

		m.rtrim_fld( techInfo->desc, techRec->desc, techRec->DESC_LEN );
		translate.multi_to_win(techInfo->desc, techInfo->DESC_LEN);

		techInfo->common_max_tech_level = m.atoi( techRec->max_tech_level, techRec->MAX_TECH_LEVEL_LEN );
		techInfo->complex_level  = m.atoi( techRec->complex_level , techRec->COMPLEX_LEVEL_LEN );

		techInfo->unit_id   		 = m.atoi( techRec->unit_id		 , techRec->ID_LEN );
		techInfo->firm_id   		 = m.atoi( techRec->firm_id		 , techRec->ID_LEN );

		techInfo->parent_tech_id = m.atoi( techRec->parent_tech_id, techRec->ID_LEN );
		techInfo->parent_tech_level = m.atoi( &techRec->parent_tech_level, sizeof(techRec->parent_tech_level) );
		techInfo->parent_tech_id2= m.atoi( techRec->parent_tech_id2, techRec->ID_LEN );
		techInfo->parent_tech_level2= m.atoi( &techRec->parent_tech_level2, sizeof(techRec->parent_tech_level2) );
		techInfo->advantage_race_id = m.atoi( techRec->advantage_race_id, techRec->RACE_ID_LEN );

		err_when( techInfo->advantage_race_id < 0 );		// total_tech_level_array support only human race so far
		err_when( techInfo->parent_tech_id && (techInfo->parent_tech_level < 1 || techInfo->parent_tech_level > 9 ));
		err_when( techInfo->parent_tech_id2 && (techInfo->parent_tech_level2 < 1 || techInfo->parent_tech_level2 > 9 ));

		if( techClassId != techInfo->class_id )
		{
			techClass = tech_class(techInfo->class_id);
			techClassId = techInfo->class_id;

			techClass->first_tech_id = i+1;
			techClass->tech_count = 1;
		}
		else
			techClass->tech_count++;

		// total_tech_level += techInfo->max_tech_level;
		for( int raceId = 0; raceId <= MAX_RACE; ++raceId )
		{
			total_tech_level_array[raceId] += techInfo->common_max_tech_level;
			if( techInfo->advantage_race_id && raceId == techInfo->advantage_race_id )
			{
				total_tech_level_array[raceId]++;		// but advantage_race_id is not set here
			}
		}


#ifdef DEBUG
		// check code name of tech class
		char techCode[techRec->CODE_LEN+1];
		m.rtrim_fld( techCode, techRec->tech_code, techRec->CODE_LEN );
		switch( techInfo->tech_id )
		{
		case TECH_MINING:
			err_when( strcmp(techCode, "MINING") );
			break;
		case TECH_AGITATION:
			err_when( strcmp(techCode, "AGITATE") );
			break;
		case TECH_INFANTRY_CLOSE_COMBAT:
			err_when( strcmp(techCode, "INFCLOSE") );
			break;
		case TECH_SPU_STRUCTURE_FIRST:
			err_when( strcmp(techCode, "SPUS_NOR") );
			break;
		case TECH_SPU_BERSERK_FIRST:
			err_when( strcmp(techCode, "SPUB_NOR") );
			break;
		case TECH_CATAPULT:
			err_when( strcmp(techCode, "CATAPULT") );
			break;
		case TECH_OFFENSIVE_STRUCTURE_1:
			err_when( strcmp(techCode, "OFF_BLD1") );
			break;
		case TECH_MEGA_CANNON:
			err_when( strcmp(techCode, "M_CANNON") );
			break;
		case TECH_LISHORR:
			err_when( strcmp(techCode, "LISHORR") );
			break;
		case TECH_BEE:
			err_when( strcmp(techCode, "FBEE") );
			break;
		case TECH_JAGUAR:
			err_when( strcmp(techCode, "JAGUAR") );
			break;
		case TECH_LIVE_TO_GOLD:
			err_when( strcmp(techCode, "LIVEGOLD") );
			break;
		case TECH_FORTRESS:
			err_when( strcmp(techCode, "FORTRESS") );
			break;
		case TECH_WEAPON_1:
			err_when( strcmp(techCode, "WEAPON1") );
			break;
		case TECH_MAGIC_DAMAGE_BUILDING:
			err_when( strcmp(techCode, "MAGIC1") );
			break;
		}
#endif
	}

	//---------- set TechInfo::race_id ------------//

	for( i=0 ; i<tech_count ; i++ )
	{
		techInfo = tech_info_array+i;

		if( techInfo->class_id == TECH_CLASS_SPU_STRUCTURE ||
		 	 techInfo->class_id == TECH_CLASS_SPU_BERSERKER )
		{
			techInfo->race_id = i+1 - tech_class(techInfo->class_id)->first_tech_id + 1;

			err_when( techInfo->race_id < 1 || techInfo->race_id > MAX_RACE );
		}
		else if( tech_class(techInfo->class_id)->race_id )	// copy from tech class if it has
		{
			techInfo->race_id = tech_class(techInfo->class_id)->race_id;
		}
	}
}	
//--------- End of function TechRes::load_tech_info ---------//


//---------- Begin of function TechRes::second_init -----------//
//
void TechRes::second_init()
{
	if( game.is_campaign_mode() )		// tech are not available in the beginning of a campaign.
	{
		for( int i=0 ; i<tech_count ; i++ )
			tech_info_array[i].available_level = 0;
	}
	else
	{
		for( int i=0 ; i<tech_count ; i++ )
		{
			if( tech_info_array[i].class_id == TECH_CLASS_OFFENSIVE_STRUCTURE )
				tech_info_array[i].available_level = 0;		// cannot research offensive structure in single game
			else
				tech_info_array[i].available_level = tech_info_array[i].common_max_tech_level;
		}
	}
}
//---------- End of function TechRes::second_init -----------//


/*
//---------- Begin of function TechRes::second_init -----------//
//
void TechRes::second_init()
{
	err_when( !init_flag );

	int i, j;

	char raceSeq[MAX_RACE];
	for( i = 0; i < MAX_RACE; ++i )
	{
		raceSeq[i] = i+1;
	}

	// shuffle randomly
	for( i = 0; i < MAX_RACE; ++i )
	{
		j = m.random(MAX_RACE);

		// exchance raceSeq[i] and raceSeq[j]
		char t = raceSeq[i];
		raceSeq[i] = raceSeq[j];
		raceSeq[j] = t;
	}

	// initialize advantage_race_id

	j = 0;
	for( i = 1; i <= tech_count; ++i )
	{
		TechInfo *techInfo = tech_info_array + i - 1;

		if( techInfo->common_max_tech_level > 1	// meaningful to have higher levels
			&& !techInfo->race_id						// non-race specific
			&& !techInfo->unit_id && !techInfo->firm_id )	// not related to unit/firm technology
		{
			techInfo->advantage_race_id = raceSeq[j];
			if( ++j >= MAX_RACE )
				j = 0;
		}
	}

	// ------ recalculate total_tech_level_array for each race ------//

	memset( total_tech_level_array, 0, sizeof(total_tech_level_array) );

	for( i = 1; i <= tech_count; ++i )
	{
		TechInfo *techInfo = tech_info_array + i - 1;

		for( int raceId = 0; raceId <= MAX_RACE; ++raceId )
		{
			total_tech_level_array[raceId] += techInfo->common_max_tech_level;
			if( techInfo->advantage_race_id && raceId == techInfo->advantage_race_id )
			{
				total_tech_level_array[raceId]++;		// but advantage_race_id is not set here
			}
		}
	}

}
//---------- End of function TechRes::second_init -----------//
*/

//---------- Begin of function TechInfo::tech_des -----------//

char* TechInfo::tech_des()
{
	return desc;
//	if( unit_id )
//		return unit_res[unit_id]->name;
//	else if( firm_id )
//		return firm_res[firm_id]->name;
//	else
//		return "";
}
//---------- End of function TechInfo::tech_des -----------//

#ifdef DEBUG

//---------- Begin of function TechRes::operator[] -----------//

TechInfo* TechRes::operator[](int techId)
{
	err_if( techId<1 || techId>tech_count )
		err_now( "TechRes::operator[]" );

	return tech_info_array+techId-1;
}
//------------ End of function TechRes::operator[] -----------//


//---------- Begin of function TechRes::tech_class -----------//

TechClass* TechRes::tech_class(int techClassId)
{
	err_if( techClassId<1 || techClassId>tech_count )
		err_now( "TechRes::tech_class" );

	return tech_class_array+techClassId-1;
}
//------------ End of function TechRes::tech_class -----------//

#endif

//--------- Begin of function TechRes::init_nation_tech --------//
//
// Close down all firms under this nation.
//
void TechRes::init_nation_tech(int nationRecno)
{
	int 		 i;
	TechInfo* techInfo = tech_res.tech_info_array;

	for( i=0 ; i<tech_res.tech_count ; i++, techInfo++ )
	{
		// ##### begin Gilbert 14/3 #######//
		// reset tech research
		techInfo->nation_research_progress_array[nationRecno-1] = 0;
		// ##### end Gilbert 14/3 #######//

		techInfo->set_nation_tech_level(nationRecno, 0);

		// ##### begin Gilbert 26/11 #####//
		// since set_nation_tech_level cannot decrease UnitInfo/FirmInfo's nation tech level,
		// call here to reset nation tech level
		if( techInfo->unit_id )
			unit_res[techInfo->unit_id]->set_nation_tech_level( nationRecno, 0 );

		if( techInfo->firm_id )
			firm_res[techInfo->firm_id]->set_nation_tech_level( nationRecno, 0 );
		// ##### end Gilbert 26/11 #####//
	}
}
//----------- End of function TechRes::init_nation_tech ---------//


//--------- Begin of function TechInfo::is_parent_tech_invented --------//
//
// Whether this technology can be researched or not.
//
int TechInfo::is_parent_tech_invented(int nationRecno)
{
	//--- special case: TECH_ALCHEMY_YIELD -----//

	if( tech_id == TECH_ALCHEMY_YIELD )		// only available for research when all basic alchemy technologies have been researched.
	{
		return tech_res[TECH_LIVE_TO_GOLD]->get_nation_tech_level(nationRecno) &&
				 tech_res[TECH_GOLD_TO_LIVE]->get_nation_tech_level(nationRecno) &&
				 tech_res[TECH_RAW_TO_LIVE ]->get_nation_tech_level(nationRecno) &&
				 tech_res[TECH_RAW_TO_GOLD ]->get_nation_tech_level(nationRecno);
	}

	//------------- normal case -------------//

	if( parent_tech_id )
	{
		if( tech_res[parent_tech_id]->get_nation_tech_level(nationRecno) < parent_tech_level )
			return 0;
	}

	if( parent_tech_id2 )
	{
		if( tech_res[parent_tech_id2]->get_nation_tech_level(nationRecno) < parent_tech_level2 )
			return 0;
	}

	return 1;
}
//----------- End of function TechInfo::is_parent_tech_invented ---------//


//--------- Begin of function TechInfo::can_research --------//
//
// Whether this technology can be researched or not.
//
// <int> nationRecno    - nation recno
// [int] checkLivePoint - whether check the live points or not.
//								  (default: 1)
//
int TechInfo::can_research(int nationRecno, int checkLivePoint)
{
	if( get_nation_tech_level(nationRecno) >= available_level )
		return 0;

	if( checkLivePoint && nation_array[nationRecno]->live_points < live_points_cost() )
		return 0;

	if( class_id == TECH_CLASS_MEGA_WEAPON )		// cannot research mega weapon
		return 0;

	//--- if this tech is race specific, the nation must have a town of that race in order to be able to research it ---//

	if( race_id > 0 )
	{
		Nation* nationPtr = nation_array[nationRecno];

		for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
		{
			Town* townPtr = town_array[ nationPtr->ai_town_array[i] ];

			if( townPtr->race_id == race_id )
				break;
		}

		if( i==nationPtr->ai_town_count )
			return 0;
	}

	//--------------------------------------------//

	return get_nation_tech_level(nationRecno) < max_tech_level(nationRecno) &&
			 is_parent_tech_invented(nationRecno);
}
//----------- End of function TechInfo::can_research ---------//


//------ Begin of function TechInfo::set_nation_tech_level ------//
//
// Set the nation's tech level on this technology.
//
// <int> nationRecno - nation recno
// <int> techLevel   - the new tech level
//
void TechInfo::set_nation_tech_level(int nationRecno, int techLevel)
{
	err_when( nationRecno<1 || nationRecno>nation_array.size() );
	err_when( techLevel > common_max_tech_level+(advantage_race_id ? 1 : 0) );

	nation_tech_level_array[nationRecno-1] = techLevel;

   //-----------------------------------------------------------------//
	// sometimes, more than one tech refer to one unit_id (or firm_id),
	// set unit_res/firm_res, tech level if new one is larger
	//-----------------------------------------------------------------//

	if( unit_id && unit_res[unit_id]->get_nation_tech_level(nationRecno) < techLevel )
		unit_res[unit_id]->set_nation_tech_level( nationRecno, techLevel );

	if( firm_id && firm_res[firm_id]->get_nation_tech_level(nationRecno) < techLevel )
		firm_res[firm_id]->set_nation_tech_level( nationRecno, techLevel );

	//--- if the nation is currently researching this tech ----//

	Nation* nationPtr = nation_array[nationRecno];

	if( nationPtr->research_tech_id == tech_id )
	{
		//--- if the player has already reached the maximum tech level ---//

		// ###### begin Gilbert 21/4 #########//

		if( nationPtr->is_ai() )		// AI has its own function in thinking about which technology to research
		{
			nationPtr->research_tech_id = 0;		// reset it
		}	

		else
		{
			int newTechId;

			//--- research the next tech level ---//

			if( can_research(nationRecno) )		
			{
				nationPtr->start_research( nationPtr->research_tech_id, COMMAND_AUTO );
			}

			// --- other related tech ------//

			else if( (newTechId = tech_res.suggest_next_tech(nationRecno, tech_id)) )
			{
				nationPtr->start_research( newTechId, COMMAND_AUTO );
			}

			// ---- stop research -----//
			else 
			{
				nationPtr->research_tech_id = 0;
			}
		}
		// ###### end Gilbert 21/4 #########//
	}
}
//------- End of function TechInfo::set_nation_tech_level -------//


//---------- Begin of function TechRes::inc_all_tech_level -----------//
//
// One of the cheating functions - increase the levels of all technology
// by one level for the specific nation.
//
void TechRes::inc_all_tech_level(int nationRecno)
{
	int 		 curTechLevel;
	TechInfo* techInfo = tech_info_array;

	for( int i=1 ; i<=tech_count ; i++, techInfo++ )
	{
		curTechLevel = techInfo->get_nation_tech_level(nationRecno);

		if( curTechLevel < techInfo->max_tech_level(nationRecno) )
			techInfo->set_nation_tech_level( nationRecno, curTechLevel+1 );
	}
}
//------------ End of function TechRes::inc_all_tech_level -----------//


//---------- Begin of function TechRes::inc_tech_class_level -----------//
//
void TechRes::inc_tech_class_level(int nationRecno, int techClass)
{
	int 		 curTechLevel;
	TechInfo* techInfo = tech_info_array + tech_class(techClass)->first_tech_id - 1;
	int techCount = tech_class(techClass)->tech_count;

	for( int i=1 ; i<=techCount ; i++, techInfo++ )
	{
		curTechLevel = techInfo->get_nation_tech_level(nationRecno);

		if( curTechLevel < techInfo->max_tech_level(nationRecno) )
			techInfo->set_nation_tech_level( nationRecno, curTechLevel+1 );
	}
}
//---------- End of function TechRes::inc_tech_class_level -----------//


//------ Begin of function TechInfo::disable_know ------//
//
// Set the nation's tech level on this technology.
//
void TechInfo::disable_know(int nationRecno)
{
	err_when( nationRecno<1 || nationRecno>nation_array.size() );

	nation_tech_level_array[nationRecno-1] = 0;

	if( unit_id )
		unit_res[unit_id]->set_nation_tech_level( nationRecno, 0 );

	if( firm_id )
		firm_res[firm_id]->set_nation_tech_level( nationRecno, 0 );
}
//------- End of function TechInfo::disable_know -------//


//--------- Begin of function TechClass::total_tech_count --------//
//
// The number of tech researched by each nation in this tech class
//
int TechClass::total_tech_count(int nationRecno)
{
	int		 totalTechCount=0;
	TechInfo* techInfo = tech_res.tech_info_array + first_tech_id - 1; 

	for( int i=0 ; i<tech_count ; i++, techInfo++ )
	{
		totalTechCount += techInfo->get_nation_tech_level(nationRecno);
	}

	return totalTechCount;
}
//----------- End of function TechClass::total_tech_count ---------//


//--------- Begin of function TechClass::random_init --------//
//
// Randomly give technologies to a nation.
//
// <int> nationRecno  - the nation to have the tech.
// <int> minTechCount - the minimum number of tech to be randomly generated
// <int> maxTechCount - the maximum number of tech to be randomly generated
//
void TechClass::random_init(int nationRecno, int minTechCount, int maxTechCount)
{
	int addCount = minTechCount + m.random(maxTechCount-minTechCount+1);

	for( int i=0 ; i<addCount ; i++ )
	{
		int techSubSeq = m.random(tech_count);

		for( int j=0 ; j<tech_count ; j++ )
		{
			if( ++techSubSeq >= tech_count )
				techSubSeq = 0;

			TechInfo* techInfo = tech_res.tech_info_array + first_tech_id + techSubSeq - 1;

			if( techInfo->can_research(nationRecno) )
			{
				techInfo->set_nation_tech_level( nationRecno, techInfo->get_nation_tech_level(nationRecno)+1 );
				break;
			}
		}
	}
}
//----------- End of function TechClass::random_init ---------//


//--------- Begin of function TechClass::researchable_tech_count --------//
//
// Return the number of tech currently available for research.
//
// <int> nationRecno - the nation recno
// [int] raceId		- if this is given, only tech can be researched
//							  by this race of the given nation is counted.
//
int TechClass::researchable_tech_count(int nationRecno, int raceId)
{
	int techCount=0;

	for( int i=0 ; i<tech_count ; i++ )
	{
		if( raceId && raceId != tech_res[first_tech_id+i]->race_id )
			continue;

		if( tech_res[first_tech_id+i]->can_research(nationRecno) )
			techCount++;
	}

	return techCount;
}
//----------- End of function TechClass::researchable_tech_count ---------//


//--------- Begin of function TechRes::researchable_tech_count --------//
//
// Return the number of tech currently available for research.
//
// <int> nationRecno - the nation recno
// [int] raceId		- if this is given, only tech can be researched
//							  by this race of the given nation is counted.
// [int] checkLivePoint - whether check the live points or not.
//								  (default: 1)
//
int TechRes::researchable_tech_count(int nationRecno, int raceId, int checkLivePoint)
{
	int techCount=0;

	for( int i=1 ; i<=tech_count ; i++ )
	{
		if( raceId && raceId != tech_res[i]->race_id )
			continue;

		if( tech_res[i]->can_research(nationRecno, checkLivePoint) )
			techCount++;
	}

	return techCount;
}
//----------- End of function TechRes::researchable_tech_count ---------//


// ---------- begin of function TechInfo::max_tech_level --------//
//
short TechInfo::max_tech_level(int nationRecno)
{
	// ######### begin Gilbert 14/3 #########//
//	if( nationRecno && advantage_race_id && nation_array[nationRecno]->race_id == advantage_race_id )
//		return common_max_tech_level + 1;
//	else
//		return common_max_tech_level;

	// ----- take care of the current tech level -------//
	// steal or exchange tech can bring non-advantage nation to exceed common_max_tech_level

	if( nationRecno && advantage_race_id && nation_array[nationRecno]->race_id == advantage_race_id )
		return max( common_max_tech_level+1, get_nation_tech_level(nationRecno) );
	else
		return common_max_tech_level;

	// ######### end Gilbert 14/3 #########//
}
// ---------- end of function TechInfo::max_tech_level --------//


// ---------- begin of function TechRes::total_tech_level --------//
//
// the sum of research levels of all technology, for each nation race
//
short TechRes::total_tech_level(int nationRecno)
{
	char nationRaceId;
	if( nationRecno && (nationRaceId = nation_array[nationRecno]->race_id) > 0 )
	{
		return total_tech_level_array[nationRaceId];
	}
	return total_tech_level_array[0];	// monster nation (or raceless?)
}
// ---------- end of function TechRes::total_tech_level --------//


//--------- Begin of function TechInfo::is_useful_to_nation --------//
//
// Return whether this tech is useful to the given nation.
//
// <int> nationRecno - nation recno
//
// return: <int> 1 - useful
//					  0 - not useful
//
int TechInfo::is_useful_to_nation(int nationRecno)
{
	Nation* nationPtr = nation_array[nationRecno];

	if( is_human_tech() != nationPtr->is_human() )		// human tech and monster tech
		return 0;

	//------ if this is a human nation ------//

	if( nationPtr->is_human() )
	{
		//----- if this tech is race specific -----//

		if( race_id > 0 )
		{
			//--- if there is a town of this race, then this tech is useful to this nation ---//

			for( int i=0 ; i<nationPtr->ai_town_count ; i++ )
			{
				if( town_array[ nationPtr->ai_town_array[i] ]->race_id == race_id )
					return 1;
			}

			return 0;
		}

		//--- otherwise the tech is always useful to the nation ---//

		return 1;
	}
	else //---- if this is a monster nation ----//
	{
		int 		 unitId 	 = monster_res[ nationPtr->monster_id() ]->unit_id;
		UnitInfo* unitInfo = unit_res[unitId];

		if( unitInfo->nation_unit_count_array[nationRecno-1] +
			 unitInfo->nation_general_count_array[nationRecno-1] > 0 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}
//----------- End of function TechInfo::is_useful_to_nation ---------//


// ---------- begin of function TechInfo::enable_parent_tech --------//
//
// Enable the parent tech of this tech.
//
void TechInfo::enable_parent_tech(int nationRecno)
{
	if( parent_tech_id &&
		 tech_res[parent_tech_id]->get_nation_tech_level( nationRecno )==0 )
	{
		tech_res[parent_tech_id]->set_nation_tech_level( nationRecno, 1 );
		tech_res[parent_tech_id]->enable_parent_tech(nationRecno); 	// recursive call
	}

	if( parent_tech_id2 &&
		 tech_res[parent_tech_id2]->get_nation_tech_level( nationRecno )==0 )
	{
		tech_res[parent_tech_id2]->set_nation_tech_level( nationRecno, 1 );
		tech_res[parent_tech_id2]->enable_parent_tech(nationRecno);
	}
}
// ---------- end of function TechInfo::enable_parent_tech --------//


// ---------- begin of function TechRes::suggest_next_tech -----------//
//
// suggest next technology to research
// return tech Id, to research, 0 if none
//
int TechRes::suggest_next_tech(int nationRecno, int finishedTechId)
{
	if( finishedTechId == 0 )
		return 0;

	err_when( finishedTechId < 1 || finishedTechId > tech_count );


	TechInfo *finishedTechInfo = operator[](finishedTechId);
	int finishedTechLevel = finishedTechInfo->get_nation_tech_level(nationRecno);
	int reachMaxLevel = finishedTechLevel >= finishedTechInfo->max_tech_level(nationRecno);

	int techId, techId2;

	// step 1 : find any tech which is dependent on finishedTechId and can start research

	for( techId = 1; techId <= tech_count; ++techId )
	{
		TechInfo *techInfo = operator[](techId);
		if( finishedTechId == techInfo->parent_tech_id
				&& finishedTechLevel >= techInfo->parent_tech_level
			|| finishedTechId == techInfo->parent_tech_id2
				&& finishedTechLevel >= techInfo->parent_tech_level2 )
		{
			if( techInfo->can_research(nationRecno) )
			{
				return techId;
			}
		}
	}

	// step 1.5 : continue until it reach the max tech level

	if( !reachMaxLevel )
	{
		return finishedTechId;
	}

	// step 2 : find any tech which partly dependent on finishedTechId
	// if the other requirement is not fullfilled, reseach the other requirement ( and its requirement)

	for( techId = 1; techId <= tech_count; ++techId )
	{
		TechInfo *techInfo = operator[](techId);
		if( finishedTechId == techInfo->parent_tech_id
			&& techInfo->parent_tech_id2 != 0
			&& finishedTechLevel >= techInfo->parent_tech_level )
		{
			if( techInfo->can_research(nationRecno) )
			{
				return techId;
			}
			else if( (techId2 = find_researchable_requirement(nationRecno, techId)) != 0)
			{
				return techId2;
			}
		}

		if( finishedTechId == techInfo->parent_tech_id2
			&& techInfo->parent_tech_id != 0
			&& finishedTechLevel >= techInfo->parent_tech_level2 )
		{
			if( techInfo->can_research(nationRecno) )
			{
				return techId;
			}
			else if( (techId2 = find_researchable_requirement(nationRecno, techId)) != 0)
			{
				return techId2;
			}
		}
	}

	int techClassId = finishedTechInfo->class_id;
	if( techClassId )
	{
		// step 3 : other tech of the same class, can be researched directly

		int classFirst = tech_class(techClassId)->first_tech_id;
		int classLast = classFirst + tech_class(techClassId)->tech_count - 1;

		for( techId = classFirst; techId <= classLast; ++techId )
		{
			TechInfo *techInfo = operator[](techId);
			if( techInfo->can_research(nationRecno) )
			{
				return techId;
			}
		}

		// step 4 : other tech of the same class, but need other requirement (special unit berserker...)

		for( techId = classFirst; techId <= classLast; ++techId )
		{
			TechInfo *techInfo = operator[](techId);
			if( (techId2 = find_researchable_requirement(nationRecno, techId)) != 0)
			{
				return techId2;
			}
		}
	}

	return 0;
}
// ---------- end of function TechRes::suggest_next_tech -----------//



// ---------- begin of function TechRes::find_researchable_requirement -----------//
//
// find through the research tree which tech can be research now
//
int TechRes::find_researchable_requirement(int nationRecno, int techId, int recurCount )
{
	TechInfo *techInfo = operator[](techId);
	if( techInfo->can_research(nationRecno) )
		return techId;

	if( recurCount > tech_count )
		return 0;		// terminate recursion

	// find parent 1

	if( techInfo->parent_tech_id 
		&& operator[](techInfo->parent_tech_id)->can_research(nationRecno) )
		return techInfo->parent_tech_id;

	// find parent 2

	if( techInfo->parent_tech_id2
		&& operator[](techInfo->parent_tech_id2)->can_research(nationRecno) )
		return techInfo->parent_tech_id2;

	// recur from parent 1

	int techId2;
	if( techInfo->parent_tech_id 
		&& (techId2 = find_researchable_requirement(nationRecno, techInfo->parent_tech_id, recurCount+1)) != 0 )
		return techId2;

	// recur from parent 2

	if( techInfo->parent_tech_id2 
		&& (techId2 = find_researchable_requirement(nationRecno, techInfo->parent_tech_id2, recurCount+1)) != 0 )
		return techId2;

	return 0;
}
// ---------- end of function TechRes::find_researchable_requirement -----------//

