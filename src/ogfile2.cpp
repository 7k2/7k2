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

//Filename    : OGFILE2.CPP
//Description : Object Game file, save game and restore game, part 2

#include <oconfig.h>
#include <ounitres.h>
#include <ofirmres.h>

#include <ocampgn.h>
#include <ounit.h>
#include <obullet.h>
#include <osite.h>
#include <otown.h>
#include <onation.h>
#include <ofirm.h>
#include <otechres.h>
#include <oraceres.h>
#include <oherores.h>
#include <otownres.h>
#include <orawres.h>
#include <ogodres.h>
#include <otalkres.h>
#include <otornado.h>
#include <omonsres.h>
#include <orebel.h>
#include <ospy.h>
#include <osnowg.h>
#include <osys.h>
#include <oregion.h>
#include <onews.h>
#include <otutor.h>
#include <oinfo.h>
#include <oweather.h>
#include <oevent.h>
#include <ostate.h>
#include <owaypnt.h>
#include <ogame.h>
#include <opower.h>
#include <oworld.h>
#include <ogfile.h>
#include <orock.h>
#include <ofirmdie.h>
#include <oworldmt.h>
#include <ounit.h>
#include <oun_grp.h>
#include <oblob.h>
#include <orle.h>
#include <oc_east.h>

#if(defined(CHINESE) && defined(DEBUG))
//SXM:Debug
void DumpBin(char* FileName,char* s,int Length);
void Dump(char* FileName,char* s);
void Dump(char* FileName,int i);
void DumpBin(char* s,int Length);
void Dump(char* s);
void Dump(int i);
//SXM:Debug
#else
#define Dump(x)
#endif

//-------- Define constant ----------//

#define BOOK_MARK 0x1000         // book mark for validing saving data


//----------- Game file format --------------//
//
// Data of the following objects will be saved :
//
// RaceRes				race_res;
// UnitRes				unit_res;
// FirmRes				firm_res;
// TownRes				town_res;
// TechRes				tech_res;
// TalkRes				talk_res;
// RawRes				raw_res;
// GodRes				god_res;
// MonsterRes			monster_res;
// HeroRes				hero_res;
//
// UnitArray			unit_array(100); 			// with derived classes
// BulletArray			bullet_array(100);
// SiteArray			site_array;
// TownArray			town_array;
// NationArray			nation_array;
// FirmArray			firm_array;					// with derived classes
// TornadoArray		tornado_array(10);
// RebelArray			rebel_array;
// SpyArray				spy_array;
// SnowGroundArray	snow_ground_array;
// RegionArray			region_array;
// NewsArray			news_array;
// StateArray 			state_array;
//
// Game					game
//	Config				config
// Sys				   sys
// Info					info
// Power					power
// Weather				weather, weather_forecast[MAX_WEATHER_FORECAST];
// MagicWeather		magic_weather
// News					news
// World					world
// Tutor					tutor
//### begin alex 23/9 ###//
// SeekPath				seek_path
//#### end alex 23/9 ####//
//
//-------------------------------------------//

//--------- Define static vars ---------//

static int loaded_random_seed;

//-------- Define static class member vars -------//

File* GameFile::file_ptr;
char  GameFile::last_read_success_flag=0;

//-------- Begin of function GameFile::write_file -------//
//
// Save a game to file
//
// Return : 1 - file written successfully
//          0 - not successful
//
int GameFile::write_file(File* filePtr)
{
	file_ptr = filePtr;			// for write_book_mark()

	//----- check valid version first ------//

	if( game_file_array.demo_format )
		filePtr->file_put_short( -GAME_VERSION );    // negative no. means shareware version
	else
		filePtr->file_put_short( GAME_VERSION );

	//------------------------------------------------//
	//
	// The order of writing data is different between
	// the shareware and registered version.
	//
	//------------------------------------------------//

	if( game_file_array.demo_format )
	{
		if( !write_file_1(filePtr) )
			return 0;

		if( !write_file_2(filePtr) )
			return 0;
	}
	else
	{
		if( !write_file_2(filePtr) )
			return 0;

		if( !write_file_1(filePtr) )
         return 0;
   }

   if( !write_file_3(filePtr) )
		return 0;

   return 1;
}
//---------- End of function GameFile::write_file -------//


//-------- Begin of function GameFile::read_file -------//
//
// Restore a game from file
//
// Return : 1  - file read successfully
//          0  - not successful
//          -1 - incorrect game data version error
//
int GameFile::read_file(File* filePtr)
{
	file_ptr = filePtr;			// for read_book_mark()

	//----- check version no. first ------//

	int originalRandomSeed = m.get_random_seed();

	// compare major version
	if( filePtr->file_get_short()/100 != (game_file_array.demo_format ? -(GAME_VERSION/100) : GAME_VERSION/100) )
		return -1;

	//------------------------------------------------//
	//
	// The order of writing data is different between
	// the shareware and registered version.
	//
	//------------------------------------------------//

	if( game_file_array.demo_format )
	{
		if( !read_file_1(filePtr) )
			return 0;

		if( !read_file_2(filePtr) )
			return 0;
	}
	else
	{
		if( !read_file_2(filePtr) )
			return 0;

		if( !read_file_1(filePtr) )
			return 0;
	}

	if( !read_file_3(filePtr) )
		return 0;

	//-------------------------------------//

	err_when( originalRandomSeed != m.get_random_seed() );

	m.set_random_seed(loaded_random_seed);

	return 1;
}
//---------- End of function GameFile::read_file -------//


//-------- Begin of function GameFile::write_file_1 -------//
//
// Return : 1 - file written successfully
//          0 - not successful
//
int GameFile::write_file_1(File* filePtr)
{
	write_book_mark( BOOK_MARK+1 );

	if( !race_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+2 );

	if( !unit_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+3 );

	if( !firm_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+4 );

	if( !town_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+5 );

	if( !tech_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+6 );

	if( !talk_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+7 );

	if( !raw_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+8 );

	if( !god_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+9 );

	if( !monster_res.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+10 );

	if( !hero_res.write_file(filePtr) )
		return 0;

	return 1;
}
//---------- End of function GameFile::write_file_1 -------//


//-------- Begin of function GameFile::write_file_2 -------//
//
// Save a game to file
//
// Return : 1 - file written successfully
//          0 - not successful
//
int GameFile::write_file_2(File* filePtr)
{
	write_book_mark( BOOK_MARK+101 );

	if( !game.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+102 );

	if( !config.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+103 );

	if( !sys.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+104 );

	if( !info.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+105 );

	if( !power.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+106 );

	if( !weather.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+107 );

	if( !magic_weather.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+108 );

	if( !news_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+109 );

	if( !state_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+110 );

	if( !world.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+111 );

	if( !tutor.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+112 );

	if( !random_event.write_file(filePtr) )
		return 0;

	return 1;
}
//---------- End of function GameFile::write_file_2 -------//


//-------- Begin of function GameFile::write_file_3 -------//
//
// Return : 1 - file written successfully
//          0 - not successful
//
int GameFile::write_file_3(File* filePtr)
{
	write_book_mark( BOOK_MARK+201 );

	if( !unit_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+202 );

	if( !bullet_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+203 );

	if( !site_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+204 );

	if( !town_array.write_file(filePtr) )  // job will affect firm, group, item
		return 0;

	write_book_mark( BOOK_MARK+205 );

	if( !nation_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+206 );

	if( !firm_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+207 );

	if( !tornado_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+208 );

	if( !rebel_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+209 );

	if( !spy_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+210 );

	if( !snow_ground_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+211 );

	if( !region_array.write_file(filePtr) )
		return 0;

	// ####### begin Gilbert 25/9 #########//

	// new_array written in BOOK_MARK+108
//	write_book_mark( BOOK_MARK+212 );
//
//	if( !news_array.write_file(filePtr) )
//		return 0;
//
	write_book_mark( BOOK_MARK+212 );

	if( !rock_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+213 );

	if( !dirt_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+214 );

	if( !firm_die_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+215 );

	for( int i=0 ; i<MAX_DEFINED_UNIT_GROUP ; i++ )
	{
		if( !unit_group_array[i].write_file(filePtr) )
			return 0;
	}

	write_book_mark( BOOK_MARK+216 );

	if( !way_point_array.write_file(filePtr) )
		return 0;

	write_book_mark( BOOK_MARK+217 );

	if( !base_obj_array.write_file(filePtr) )
		return 0;

	return 1;
}
//---------- End of function GameFile::write_file_3 -------//


//-------- Begin of function GameFile::read_file_1 -------//
//
// Return : 1  - file read successfully
//          0  - not successful
//
int GameFile::read_file_1(File* filePtr)
{
	if( !read_book_mark( BOOK_MARK+1 ) )
		return 0;

	if( !race_res.read_file(filePtr) )
		return 0;

	Dump("race_res\n");
	Dump(sizeof(race_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+2 ) )
		return 0;

	if( !unit_res.read_file(filePtr) )
		return 0;

	Dump("unit_res\n");
	Dump(sizeof(unit_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+3 ) )
		return 0;

	if( !firm_res.read_file(filePtr) )
		return 0;

	Dump("firm_res\n");
	Dump(sizeof(firm_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+4 ) )
		return 0;

	if( !town_res.read_file(filePtr) )
		return 0;

	Dump("town_res\n");
	Dump(sizeof(town_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+5 ) )
		return 0;

	if( !tech_res.read_file(filePtr) )
		return 0;

	Dump("tech_res\n");
	Dump(sizeof(tech_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+6 ) )
		return 0;

	if( !talk_res.read_file(filePtr) )
		return 0;

	Dump("talk_res\n");
	Dump(sizeof(talk_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+7 ) )
		return 0;

	if( !raw_res.read_file(filePtr) )
		return 0;

	Dump("raw_res\n");
	Dump(sizeof(raw_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+8 ) )
		return 0;

	if( !god_res.read_file(filePtr) )
		return 0;

	Dump("god_res\n");
	Dump(sizeof(god_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+9 ) )
		return 0;

	if( !monster_res.read_file(filePtr) )
		return 0;

	Dump("monster_res\n");
	Dump(sizeof(monster_res));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+10 ) )
		return 0;

	if( !hero_res.read_file(filePtr) )
		return 0;

	Dump("hero_res\n");
	Dump(sizeof(hero_res));
	Dump("\n");

	return 1;
}
//---------- End of function GameFile::read_file_1 -------//


//-------- Begin of function GameFile::read_file_2 -------//
//
// Return : 1  - file read successfully
//          0  - not successful
//
int GameFile::read_file_2(File* filePtr)
{
	if( !read_book_mark( BOOK_MARK+101 ) )
		return 0;

	if( !game.read_file(filePtr) )
		return 0;

	Dump("game\n");
	Dump(sizeof(game));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+102 ) )
		return 0;

	if( !config.read_file(filePtr, 1) )		// 1-keep system settings
		return 0;

	Dump("config\n");
	Dump(sizeof(config));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+103 ) )
		return 0;

	if( !sys.read_file(filePtr) )
		return 0;

	Dump("sys\n");
	Dump(sizeof(sys));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+104 ) )
		return 0;

	if( !info.read_file(filePtr) )
		return 0;

	Dump("info\n");
	Dump(sizeof(info));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+105 ) )
		return 0;

	if( !power.read_file(filePtr) )
		return 0;

	Dump("power\n");
	Dump(sizeof(power));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+106 ) )
		return 0;

	if( !weather.read_file(filePtr) )
		return 0;

	Dump("weather\n");
	Dump(sizeof(weather));
	Dump("\n");

	weather_forecast[0] = weather;
	weather_forecast[0].next_day();

	for(int foreDay=1; foreDay < MAX_WEATHER_FORECAST; ++foreDay)
	{
		weather_forecast[foreDay] = weather_forecast[foreDay-1];
		weather_forecast[foreDay].next_day();
	}

	if( !read_book_mark( BOOK_MARK+107 ) )
		return 0;

	if( !magic_weather.read_file(filePtr) )
		return 0;

	Dump("magic_weather\n");
	Dump(sizeof(magic_weather));
	Dump("\n");

	sprite_res.update_speed();

	if( !read_book_mark( BOOK_MARK+108 ) )
		return 0;

	if( !news_array.read_file(filePtr) )
		return 0;

	Dump("news_array\n");
	Dump(sizeof(news_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+109 ) )
		return 0;

	if( !state_array.read_file(filePtr) )
		return 0;

	Dump("state_array\n");
	Dump(sizeof(state_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+110 ) )
		return 0;

	if( !world.read_file(filePtr) )
		return 0;

	Dump("world\n");
	Dump(sizeof(world));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+111 ) )
		return 0;

	if( !tutor.read_file(filePtr) )
		return 0;

	Dump("tutor\n");
	Dump(sizeof(tutor));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+112 ) )
		return 0;

	if( !random_event.read_file(filePtr) )
		return 0;

	Dump("random_event\n");
	Dump(sizeof(random_event));
	Dump("\n");

	return 1;
}
//---------- End of function GameFile::read_file_2 -------//


//-------- Begin of function GameFile::read_file_3 -------//
//
// Return : 1  - file read successfully
//          0  - not successful
//
int GameFile::read_file_3(File* filePtr)
{
	if( !read_book_mark( BOOK_MARK+201 ) )
		return 0;

	if( !unit_array.read_file(filePtr) )
		return 0;

	Dump("unit_array\n");
	Dump(sizeof(unit_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+202 ) )
		return 0;

	if( !bullet_array.read_file(filePtr) )
		return 0;

	Dump("bullet_array\n");
	Dump(sizeof(bullet_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+203 ) )
		return 0;

	if( !site_array.read_file(filePtr) )
		return 0;

	Dump("site_array\n");
	Dump(sizeof(site_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+204 ) )
		return 0;

	if( !town_array.read_file(filePtr) )  // job will affect firm, group, item
		return 0;

	Dump("town_array\n");
	Dump(sizeof(town_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+205 ) )
		return 0;

	if( !nation_array.read_file(filePtr) )
		return 0;

	Dump("nation_array\n");
	Dump(sizeof(nation_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+206 ) )
		return 0;

	if( !firm_array.read_file(filePtr) )
		return 0;

	Dump("firm_array\n");
	Dump(sizeof(firm_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+207 ) )
		return 0;

	if( !tornado_array.read_file(filePtr) )
		return 0;

	Dump("tornado_array\n");
	Dump(sizeof(tornado_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+208 ) )
		return 0;

	if( !rebel_array.read_file(filePtr) )
		return 0;

	Dump("rebel_array\n");
	Dump(sizeof(rebel_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+209 ) )
		return 0;

	if( !spy_array.read_file(filePtr) )
		return 0;

	Dump("spy_array\n");
	Dump(sizeof(spy_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+210 ) )
		return 0;

	if( !snow_ground_array.read_file(filePtr) )
		return 0;

	Dump("snow_ground_array\n");
	Dump(sizeof(snow_ground_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+211 ) )
		return 0;

	if( !region_array.read_file(filePtr) )
		return 0;

	Dump("region_array\n");
	Dump(sizeof(region_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+212 ) )
		return 0;

	if( !rock_array.read_file(filePtr) )
		return 0;

	Dump("rock_array\n");
	Dump(sizeof(rock_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+213 ) )
		return 0;

	if( !dirt_array.read_file(filePtr) )
		return 0;

	Dump("dirt_array\n");
	Dump(sizeof(dirt_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+214 ) )
		return 0;

	if( !firm_die_array.read_file(filePtr) )
		return 0;
	else
	{
		int count = 0;
		for (int i = 1; i <= firm_die_array.size(); i++)
		{
			if ( !firm_die_array.is_deleted(i) )
			{
				FirmDie *firmDiePtr = firm_die_array[i];
				FirmBuild *firmBuild = firm_die_res.get_build(firmDiePtr->firm_build_id);
				firmBuild->load_bitmap_res();
				count ++;
				firmBuild->loaded_count = count;
			}
		}
	}

	Dump("firm_die_array\n");
	Dump(sizeof(firm_die_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+215 ) )
		return 0;

	for( int i=0 ; i<MAX_DEFINED_UNIT_GROUP ; i++ )
	{
		if( !unit_group_array[i].read_file(filePtr) )
			return 0;
		Dump("unit_group_array\n");
		Dump(sizeof(unit_group_array));
		Dump("\n");
	}

	if( !read_book_mark( BOOK_MARK+216 ) )
		return 0;

	if( !way_point_array.read_file(filePtr) )
		return 0;

	Dump("way_point_array\n");
	Dump(sizeof(way_point_array));
	Dump("\n");

	if( !read_book_mark( BOOK_MARK+217) )
		return 0;

	if( !base_obj_array.read_file(filePtr) )
		return 0;

	Dump("base_obj_array\n");
	Dump(sizeof(base_obj_array));
	Dump("\n");

	return 1;
}
//---------- End of function GameFile::read_file_3 -------//


//-------- Begin of function GameFile::write_book_mark -------//
//
void GameFile::write_book_mark(short bookMark)
{
	sys.yield();

	file_ptr->file_put_short(bookMark);
}
//---------- End of function GameFile::write_book_mark -------//


//-------- Begin of function GameFile::read_book_mark -------//
//
// Return : 1  - the book mark is matched
//          0  - the book mark is not matched
//
int GameFile::read_book_mark(short bookMark)
{
	sys.yield();

	return file_ptr->file_get_short() == bookMark;
}
//---------- End of function GameFile::read_book_mark -------//

//***//

//-------- Start of function RaceRes::write_file -------------//
//
int RaceRes::write_file(File* filePtr)
{
	//------- write RaceInfo -------//

//	RaceInfo* raceInfo = race_info_array;

//	for( int i=1 ; i<=race_res.race_count ; i++, raceInfo++ )
//	{
//		filePtr->file_put_short( raceInfo->town_name_used_count );
//	}

	return filePtr->file_write( name_used_array, sizeof(name_used_array[0]) * name_count );
}
//--------- End of function RaceRes::write_file ---------------//


//-------- Start of function RaceRes::read_file -------------//
//
int RaceRes::read_file(File* filePtr)
{
	//------- read RaceInfo -------//

//	RaceInfo* raceInfo = race_info_array;

//	for( int i=1 ; i<=race_res.race_count ; i++, raceInfo++ )
//	{
//		raceInfo->town_name_used_count = filePtr->file_get_short();
//	}

	return filePtr->file_read( name_used_array, sizeof(name_used_array[0]) * name_count );
}
//--------- End of function RaceRes::read_file ---------------//

//***//

//-------- Start of function HeroRes::write_file -------------//
//
int HeroRes::write_file(File* filePtr)
{
	// ######## begin Gilbert 1/3 #######//

	// because some hero record are generated, whole hero_info_array is saved

	//------- write HeroInfo -------//
//	HeroInfo* heroInfo = hero_info_array;
//	for( int i=1 ; i<=hero_res.hero_count ; i++, heroInfo++ )
//	{
//		filePtr->file_put_short( heroInfo->for_hire );
//		filePtr->file_put_short( heroInfo->appeared_flag );
//	}

	if( !filePtr->file_put_short(hero_count) )
		return 0;

	// write record one by one so change of sizeof(HeroInfo) is still loadable

	for( int i=1; i <= hero_count; i++ )
	{
		HeroInfo *heroInfo = hero_info_array + i - 1;
		if( !filePtr->file_write(heroInfo, sizeof(HeroInfo)) )
			return 0;
	}
	// ######## end Gilbert 1/3 #######//

	return 1;
}
//--------- End of function HeroRes::write_file ---------------//


//-------- Start of function HeroRes::read_file -------------//
//
int HeroRes::read_file(File* filePtr)
{
	//------- read HeroInfo -------//
//	HeroInfo* heroInfo = hero_info_array;
//	for( int i=1 ; i<=hero_res.hero_count ; i++, heroInfo++ )
//	{
//		heroInfo->for_hire      = (char) filePtr->file_get_short();
//		heroInfo->appeared_flag = (char) filePtr->file_get_short();
//	}

	// because some hero record are generated, whole hero_info_array is saved

	// ###### patch begin Gilbert 4/10 #######//
	int oldHeroCount = hero_count;
	hero_count = filePtr->file_get_short();
	hero_info_array = (HeroInfo *)mem_resize( hero_info_array, hero_count * sizeof(HeroInfo) );

	// write record one by one so change of sizeof(HeroInfo) is still loadable

	for( int i=1; i <= hero_count; i++ )
	{
		HeroInfo oldHeroInfo;
		if( i <= oldHeroCount )			// save original record
			oldHeroInfo = hero_info_array[i-1];

		HeroInfo *heroInfo = hero_info_array + i - 1;
		if( !filePtr->file_read(heroInfo, sizeof(HeroInfo)) )
			return 0;

		if( i <= oldHeroCount )			// set hero name
			strcpy( heroInfo->name, oldHeroInfo.name );
	}

	// ###### patch end Gilbert 4/10 #######//

	return 1;
}
//--------- End of function HeroRes::read_file ---------------//

//***//

//-------- Start of function UnitRes::write_file -------------//
//
int UnitRes::write_file(File* filePtr)
{
	filePtr->file_put_short(mobile_monster_count);

	UnitInfo* unitInfo = unit_info_array;

	for( int i=1 ; i<=unit_res.unit_info_count ; i++, unitInfo++ )
	{
		if( !filePtr->file_write( unitInfo->nation_tech_level_array, sizeof(unitInfo->nation_tech_level_array) ) )
			return 0;

		if( !filePtr->file_write( unitInfo->nation_unit_count_array, sizeof(unitInfo->nation_unit_count_array) ) )
			return 0;

		if( !filePtr->file_write( unitInfo->nation_general_count_array, sizeof(unitInfo->nation_general_count_array) ) )
			return 0;
	}

	return 1;
}
//--------- End of function UnitRes::write_file ---------------//


//-------- Start of function UnitRes::read_file -------------//
//
int UnitRes::read_file(File* filePtr)
{
	mobile_monster_count = filePtr->file_get_short();

	UnitInfo* unitInfo = unit_info_array;

	for( int i=1 ; i<=unit_res.unit_info_count ; i++, unitInfo++ )
	{
		if( !filePtr->file_read( unitInfo->nation_tech_level_array, sizeof(unitInfo->nation_tech_level_array) ) )
			return 0;

		if( !filePtr->file_read( unitInfo->nation_unit_count_array, sizeof(unitInfo->nation_unit_count_array) ) )
			return 0;

		if( !filePtr->file_read( unitInfo->nation_general_count_array, sizeof(unitInfo->nation_general_count_array) ) )
			return 0;
	}

	return 1;
}
//--------- End of function UnitRes::read_file ---------------//

//***//

//-------- Start of function FirmRes::write_file -------------//
//
int FirmRes::write_file(File* filePtr)
{
	return filePtr->file_write( firm_info_array, firm_count * sizeof(FirmInfo) );
}
//--------- End of function FirmRes::write_file ---------------//


//-------- Start of function FirmRes::read_file -------------//
//
int FirmRes::read_file(File* filePtr)
{
	int arraySize = firm_count * sizeof(FirmInfo);

	//----- save the firm names, so that it won't be overwritten by the saved game file ----//

	FirmInfo* oldFirmInfoArray = (FirmInfo*) mem_add(arraySize);

	memcpy( oldFirmInfoArray, firm_info_array, arraySize );

	int rc = filePtr->file_read( firm_info_array, arraySize );

	for( int i=0 ; i<firm_count ; i++ )
	{
		memcpy( firm_info_array[i].name			  , oldFirmInfoArray[i].name			  , FirmInfo::NAME_LEN+1 );
		memcpy( firm_info_array[i].short_name	  , oldFirmInfoArray[i].short_name	  , FirmInfo::SHORT_NAME_LEN+1 );
		memcpy( firm_info_array[i].overseer_title, oldFirmInfoArray[i].overseer_title, FirmInfo::TITLE_LEN+1 );
		memcpy( firm_info_array[i].worker_title  , oldFirmInfoArray[i].worker_title  , FirmInfo::TITLE_LEN+1 );

		// ###### patch begin Gilbert 11/3 ########//
		firm_info_array[i].first_build_id = oldFirmInfoArray[i].first_build_id;
		firm_info_array[i].build_count = oldFirmInfoArray[i].build_count;
		// ###### patch end Gilbert 11/3 ########//
	}

	mem_del( oldFirmInfoArray );

	return rc;
}
//--------- End of function FirmRes::read_file ---------------//

//***//

//-------- Start of function TownRes::write_file -------------//
//
int TownRes::write_file(File* filePtr)
{
	int rc = filePtr->file_write( town_name_used_array, sizeof(town_name_used_array[0]) * town_name_count );
	return  (rc & filePtr->file_write( town_name_array, sizeof(TownName)*town_name_count ));
}
//--------- End of function TownRes::write_file ---------------//


//-------- Start of function TownRes::read_file -------------//
//
int TownRes::read_file(File* filePtr)
{
	int rc = filePtr->file_read( town_name_used_array, sizeof(town_name_used_array[0]) * town_name_count );
	return   (rc & filePtr->file_read( town_name_array, sizeof(TownName)*town_name_count ));
}
//--------- End of function TownRes::read_file ---------------//

//***//

//-------- Start of function TechRes::write_file -------------//
//
int TechRes::write_file(File* filePtr)
{
	if( !filePtr->file_write( tech_class_array, tech_class_count * sizeof(TechClass) ) )
		return 0;

	if( !filePtr->file_write( tech_info_array, tech_count * sizeof(TechInfo) ) )
		return 0;

	// ####### begin Gilbert 23/12 #########//
	if( !filePtr->file_write(total_tech_level_array, sizeof(total_tech_level_array)) )
		return 0;
	// ####### end Gilbert 23/12 #########//

	return 1;
}
//--------- End of function TechRes::write_file ---------------//


//-------- Start of function TechRes::read_file -------------//
//
int TechRes::read_file(File* filePtr)
{
	int i;

	TechClass *backupTechClass = (TechClass *)mem_add( tech_class_count * sizeof(TechClass) );
	memcpy( backupTechClass, tech_class_array, tech_class_count * sizeof(TechClass) );
	if( !filePtr->file_read( tech_class_array, tech_class_count * sizeof(TechClass) ) )
	{
		mem_del( backupTechClass );
		return 0;
	}

	// restore name from dbf
	for( i = 0; i < tech_class_count; ++i )
		strcpy( tech_class_array[i].desc, backupTechClass[i].desc );
	mem_del( backupTechClass );

	TechInfo *backupTechInfo = (TechInfo *) mem_add( tech_count * sizeof(TechInfo) );
	memcpy( backupTechInfo, tech_info_array, tech_count * sizeof(TechInfo) );
	if( !filePtr->file_read( tech_info_array, tech_count * sizeof(TechInfo) ) )
	{
		mem_del( backupTechInfo );
		return 0;
	}

	// restore name from dbf
	for( i = 0; i < tech_count; ++i )
		strcpy( tech_info_array[i].desc, backupTechInfo[i].desc );
	mem_del( backupTechInfo );

	// ####### begin Gilbert 23/12 #########//
	if( !filePtr->file_read(total_tech_level_array, sizeof(total_tech_level_array)) )
		return 0;
	// ####### end Gilbert 23/12 #########//

	return 1;
}
//--------- End of function TechRes::read_file ---------------//

//***//

//-------- Start of function TalkRes::write_file -------------//
//
int TalkRes::write_file(File* filePtr)
{
	if( !filePtr->file_write( this, sizeof(TalkRes) ) )
		return 0;

	if( !talk_msg_array.write_file(filePtr) )
		return 0;

	return 1;
}
//--------- End of function TalkRes::write_file ---------------//


//-------- Start of function TalkRes::read_file -------------//
//
int TalkRes::read_file(File* filePtr)
{
	//------ save talk_msg_array --------//

	char* tempArray = mem_add(sizeof(DynArrayB));

	memcpy( tempArray, &talk_msg_array, sizeof(DynArrayB) );

	//------ read in TalkRes --------//

	if( !filePtr->file_read( this, sizeof(TalkRes) ) )
		return 0;

	//------ restore talk_msg_array --------//

	memcpy( &talk_msg_array, tempArray, sizeof(DynArrayB) );

	mem_del(tempArray);

	//---- read in talk_msg_array ----//

	if( !talk_msg_array.read_file(filePtr) )
		return 0;

	return 1;
}
//--------- End of function TalkRes::read_file ---------------//

//***//

//-------- Start of function RawRes::write_file -------------//
//
int RawRes::write_file(File* filePtr)
{
	for( int i=0 ; i<MAX_RAW ; i++ )
	{
		if( !raw_info_array[i].raw_supply_firm_array.write_file(filePtr) )
			return 0;

		if( !raw_info_array[i].product_supply_firm_array.write_file(filePtr) )
			return 0;
	}

	return 1;
}
//--------- End of function RawRes::write_file ---------------//


//-------- Start of function RawRes::read_file -------------//
//
int RawRes::read_file(File* filePtr)
{
	for( int i=0 ; i<MAX_RAW ; i++ )
	{
		if( !raw_info_array[i].raw_supply_firm_array.read_file(filePtr) )
			return 0;

		if( !raw_info_array[i].product_supply_firm_array.read_file(filePtr) )
			return 0;
	}

	return 1;
}
//--------- End of function RawRes::read_file ---------------//

//***//

//-------- Start of function GodRes::write_file -------------//
//
int GodRes::write_file(File* filePtr)
{
	return filePtr->file_write( god_info_array, sizeof(GodInfo) * god_count );
}
//--------- End of function GodRes::write_file ---------------//


//-------- Start of function GodRes::read_file -------------//
//
int GodRes::read_file(File* filePtr)
{
	//###### patch begin Gilbert  19/1 #####//
	int rc = filePtr->file_read( god_info_array, sizeof(GodInfo) * god_count );
	if( rc )
		fix_info_data();
	return rc;
	//###### patch end Gilbert  19/1 #####//
}
//--------- End of function GodRes::read_file ---------------//

//***//

//-------- Start of function MonsterRes::write_file -------------//
//
int MonsterRes::write_file(File* filePtr)
{
	if( !filePtr->file_write( name_array, sizeof(name_array) ) )
		return 0;

	return filePtr->file_write( name_used_array, sizeof(name_used_array) );
}
//--------- End of function MonsterRes::write_file ---------------//


//-------- Start of function MonsterRes::read_file -------------//
//
int MonsterRes::read_file(File* filePtr)
{
	if( !filePtr->file_read( name_array, sizeof(name_array) ) )
		return 0;

	return filePtr->file_read( name_used_array, sizeof(name_used_array) );
}
//--------- End of function MonsterRes::read_file ---------------//

//***//

//-------- Start of function Game::write_file -------------//
//
int Game::write_file(File* filePtr)
{
	if( !filePtr->file_write( this, sizeof(Game) ) )
		return 0;

	if( is_campaign_mode() )
	{
		filePtr->file_put_short( cur_campaign->campaign_id );

		int writeSize = campaign_class_size(cur_campaign->campaign_id)
							 - sizeof(void*);

		if( !filePtr->file_write( (char*)cur_campaign+sizeof(void*), writeSize ) )		// do not write the virtual function tabel pointer
			return 0;

		if( !cur_campaign->random_terrain_map.write_file(filePtr) )
			return 0;

		// ######## begin Gilbert 14/3 #########//
		if( !cur_campaign->write_derived_file(filePtr) )
			return 0;
		// ######## begin Gilbert 14/3 #########//
	}

	return 1;
}
//--------- End of function Game::write_file ---------------//


//-------- Start of function Game::read_file -------------//
//
int Game::read_file(File* filePtr)
{
	if( !filePtr->file_read(this, sizeof(Game)) )
		return 0;

	// change pixel format for other machines
	init_remap_table();

	if( is_campaign_mode() )
	{
		int campaignId = filePtr->file_get_short();
		cur_campaign = NULL;

		init_campaign(campaignId);
		short* shadowArrayPtr = cur_campaign->shadow_array;

		int readSize = campaign_class_size(cur_campaign->campaign_id)
							 - sizeof(void*);

		if( !filePtr->file_read( (char*)cur_campaign + sizeof(void*), readSize ) )		// do not write the virtual function tabel pointer
			return 0;

		if( !cur_campaign->random_terrain_map.read_file(filePtr) )
			return 0;

		// ######## begin Gilbert 14/3 #########//
		if( !cur_campaign->read_derived_file(filePtr) )
			return 0;
		// ######## begin Gilbert 14/3 #########//

		cur_campaign->text_buf = NULL;
      cur_campaign->text_buf_size = 0;

		cur_campaign->text_buf2 = NULL;
		cur_campaign->text_buf2_size = 0;

		// regenerate the shadow map //
		cur_campaign->shadow_array = shadowArrayPtr;
		int prevHeight = cur_campaign->random_terrain_map.get_pix(0, 0);
		for(int i = 0; i <MAIN_TERRAIN_WIDTH; i++)
		{
			for(int j = 0; j <MAIN_TERRAIN_HEIGHT; j++)
			{
				int height = cur_campaign->random_terrain_map.get_pix(i, j);
				if (height > prevHeight)
					cur_campaign->shadow_array[j * MAIN_TERRAIN_WIDTH + i] = 32;//16 * abs(height-prevHeight) / 8; 
			   else
					cur_campaign->shadow_array[j * MAIN_TERRAIN_WIDTH + i] = 32 - 16 * abs(height-prevHeight) / 8; 
				prevHeight = height;
		   }
		}
	}

	return 1;
}
//--------- End of function Game::read_file ---------------//


// ---- Begin of function CampaignEastWest::write_derived_file -----//
//
int CampaignEastWest::write_derived_file(File *filePtr)
{
	// plot_create_game_FP and plot_next_day_FP are pointers
	// cannot be saved, so find the index in
	// plot_create_game_FP_array and plot_next_day_FP_array

	int i;
	short createGameFPid = 0;

	if( !plot_create_game_FP )
	{
		createGameFPid = 0;
	}
	else
	{
		for( i = 0; plot_create_game_FP_array[i] != NULL
			&& plot_create_game_FP_array[i] != plot_create_game_FP; ++i );

		// add every plot_??_create_game to plot_create_game_FP_array
		err_when( plot_create_game_FP_array[i] != plot_create_game_FP );
		// ( plot_create_game_FP may be NULL )

		createGameFPid = i+1;
	}

	short nextDayFPid = 0;

	if( !plot_next_day_FP )
	{
		nextDayFPid = 0;
	}
	else
	{
		for( i = 0; plot_next_day_FP_array[i] != NULL
			&& plot_next_day_FP_array[i] != plot_next_day_FP; ++i );

		// add every plot_??_next_day to plot_next_day_FP_array
		err_when( plot_next_day_FP_array[i] != plot_next_day_FP );
		// ( plot_next_day_FP may be NULL )

		nextDayFPid = i+1;
	}

	// --------- write two FP id ----------//

	if( !filePtr->file_put_short(createGameFPid) )
		return 0;

	if( !filePtr->file_put_short(nextDayFPid) )
		return 0;

	//----- write stage goal vars -----//

	stage_write_file(filePtr);

	return 1;
}
// ---- End of function CampaignEastWest::write_derived_file -----//

// ---- Begin of function CampaignEastWest::read_derived_file -----//
//
int CampaignEastWest::read_derived_file(File *filePtr)
{
	short createGameFPid = filePtr->file_get_short();
	short nextDayFPid = filePtr->file_get_short();

	// ---------- restore plot_create_game_FP -------//

	int i;

	plot_create_game_FP = NULL;
	if( createGameFPid > 0 )
	{
		// count the strlen of plot_create_game_FP_array
		for( i = 0; plot_create_game_FP_array[i] != NULL; ++i );

		if( createGameFPid > i )
		{
			err_here();		// array out of bound
			return 0;
		}
		plot_create_game_FP = plot_create_game_FP_array[createGameFPid-1];
	}

	// -------- restore plot_next_day_FP ----------//

	plot_next_day_FP = NULL;
	if( nextDayFPid > 0 )
	{
		// count the strlen of plot_next_day_FP_array
		for( i = 0; plot_next_day_FP_array[i] != NULL; ++i );

		if( nextDayFPid > i )
		{
			err_here();		// array out of bound
			return 0;
		}
		plot_next_day_FP = plot_next_day_FP_array[nextDayFPid-1];
	}

	//----- read stage goal vars -----//

	stage_read_file(filePtr);

	return 1;
}
// ---- End of function CampaignEastWest::read_derived_file -----//

//***//

//-------- Start of function Config::write_file -------------//
//
int Config::write_file(File* filePtr)
{
	return filePtr->file_write( this, sizeof(Config) );
}
//--------- End of function Config::write_file ---------------//


//-------- Start of function Config::read_file -------------//
//
int Config::read_file(File* filePtr, int keepSysSettings)
{
	//--- these settings are not game dependent -----//

	char  musicFlag 		 = music_flag;
	short cdMusicVol  	 = cd_music_volume;
	short	wavMusicVol 	 = wav_music_volume;
	char	soundEffectFlag = sound_effect_flag;
	short	soundEffectVol  = sound_effect_volume;
	char	helpMode			 = help_mode;
	int	displayModeId   = display_mode_id;

	int rc = filePtr->file_read( this, sizeof(Config) );

	if( keepSysSettings )
	{
		music_flag		   = musicFlag;
		cd_music_volume   = cdMusicVol;
		wav_music_volume  = wavMusicVol;
		sound_effect_flag = soundEffectFlag;
		sound_effect_volume = soundEffectVol;
		help_mode			= helpMode;
		display_mode_id   = displayModeId;
	}

	return rc;
}
//--------- End of function Config::read_file ---------------//

//***//

//-------- Start of function Info::write_file -------------//
//
int Info::write_file(File* filePtr)
{
	int writeSize = (char*)(&last_write_offset) - (char*)(this);

	//---------- write the info data -----------//

	return filePtr->file_write( this, writeSize );
}
//--------- End of function Info::write_file ---------------//


//-------- Start of function Info::read_file -------------//
//
int Info::read_file(File* filePtr)
{
	int readSize = (char*)(&last_write_offset) - (char*)(this);

	//------- read the info data ----------//

	return filePtr->file_read( this, readSize );
}
//--------- End of function Info::read_file ---------------//

//***//

//-------- Start of function Power::write_file -------------//
//
int Power::write_file(File* filePtr)
{
	return filePtr->file_write( this, sizeof(Power) );
}
//--------- End of function Power::write_file ---------------//


//-------- Start of function Power::read_file -------------//
//
int Power::read_file(File* filePtr)
{
	return filePtr->file_read( this, sizeof(Power) );
}
//--------- End of function Power::read_file ---------------//

//***//

//-------- Start of function Sys::write_file -------------//
//
int Sys::write_file(File* filePtr)
{
	//---- write the current random seed first ----//

	if( !filePtr->file_put_long(m.get_random_seed()) )
		return 0;

	//---------- write some Sys data -----------//

	filePtr->file_put_long(day_frame_count);
	filePtr->file_put_long(frame_count);
	filePtr->file_put_short(view_mode);
	filePtr->file_put_short(last_speed_mode);

	return 1;
}
//--------- End of function Sys::write_file ---------------//


//-------- Start of function Sys::read_file -------------//
//
int Sys::read_file(File* filePtr)
{
	//------- read the random seed --------//

	loaded_random_seed = filePtr->file_get_long();

	//--------- read some Sys data -----------//

	day_frame_count = filePtr->file_get_long();
	frame_count 	 = filePtr->file_get_long();
	view_mode       = (char) filePtr->file_get_short();
	last_speed_mode =	filePtr->file_get_short();

	return 1;
}
//--------- End of function Sys::read_file ---------------//

//***//

//-------- Start of function Weather::write_file -------------//
//
int Weather::write_file(File* filePtr)
{
	return filePtr->file_write( this, sizeof(Weather) );
}
//--------- End of function Weather::write_file ---------------//


//-------- Start of function Weather::read_file -------------//
//
int Weather::read_file(File* filePtr)
{
	return filePtr->file_read( this, sizeof(Weather) );
}
//--------- End of function Weather::read_file ---------------//

//***//

//-------- Start of function MagicWeather::write_file -------------//
//
int MagicWeather::write_file(File* filePtr)
{
	return filePtr->file_write( this, sizeof(MagicWeather) );
}
//--------- End of function MagicWeahter::write_file ---------------//


//-------- Start of function MagicWeahter::read_file -------------//
//
int MagicWeather::read_file(File* filePtr)
{
	return filePtr->file_read( this, sizeof(MagicWeather) );
}
//--------- End of function MagicWeahter::read_file ---------------//

//***//

//-------- Start of function World::write_file -------------//
//
int World::write_file(File* filePtr)
{
	int writeDecompressedMatrix = 0;
	int writeCompressedMatrix = 1;

	// -------- save size of map ---------//

	err_when( max_x_loc != map_matrix->max_x_loc );
	err_when( max_y_loc != map_matrix->max_y_loc );
	err_when( max_x_loc != zoom_matrix->max_x_loc );
	err_when( max_y_loc != zoom_matrix->max_y_loc );

	if( !filePtr->file_put_short(max_x_loc) )
		return 0;

	if( !filePtr->file_put_short(max_y_loc) )
		return 0;

	// ######### begin Gilbert 11/11 ########//
	if( !filePtr->file_put_long(gen_map_random_seed) )
		return 0;
	// ######### end Gilbert 11/11 ########//

	//--------- save map -------------//

//	if( !filePtr->file_write(loc_matrix, max_x_loc*max_y_loc*sizeof(Location) ) )
//		return 0;
//	if( !filePtr->file_write(corner_matrix, (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner) ) )
//		return 0;

	if( !filePtr->file_put_long(writeCompressedMatrix) )
		return 0;
	if( !filePtr->file_put_long(writeDecompressedMatrix) )
		return 0;

	if( writeCompressedMatrix )
	{
		Rle rle;
		Blob buffer(max_x_loc*max_y_loc);
		Blob cBuffer(max_x_loc*max_y_loc + max_x_loc + max_y_loc );
		long writeLen;
		long i;

		register unsigned char *srcBytePtr, *destBytePtr;

		// save low byte of loc_flag
		srcBytePtr = (unsigned char *) &loc_matrix->loc_flag;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save high byte of loc_flag
		err_when( sizeof(loc_matrix->loc_flag) != 2 );
		srcBytePtr = 1+ (unsigned char *) &loc_matrix->loc_flag;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// terrain_id can be generated

		// save low byte of road_terrain_id
		srcBytePtr = (unsigned char *) &loc_matrix->road_terrain_id;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save high byte of road_terrain_id
		err_when( sizeof(loc_matrix->road_terrain_id) != 2 );
		srcBytePtr = 1+ (unsigned char *) &loc_matrix->road_terrain_id;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save low byte of cargo_recno
		srcBytePtr = (unsigned char *) &loc_matrix->cargo_recno;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save high byte of cargo_recno
		err_when( sizeof(loc_matrix->cargo_recno) != 2 );
		srcBytePtr = 1+ (unsigned char *) &loc_matrix->cargo_recno;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save low byte of air_cargo_recno
		srcBytePtr = (unsigned char *) &loc_matrix->air_cargo_recno;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save high byte of air_cargo_recno
		err_when( sizeof(loc_matrix->air_cargo_recno) != 2 );
		srcBytePtr = 1+ (unsigned char *) &loc_matrix->air_cargo_recno;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save extra_para
		srcBytePtr = (unsigned char *) &loc_matrix->extra_para;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save fire_level (rleKey is 0x80)
		srcBytePtr = (unsigned char *) &loc_matrix->fire_level;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0x80);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0x80);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save flammability (rleKey is 0x80)
		srcBytePtr = (unsigned char *) &loc_matrix->flammability;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0x80);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0x80);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save power_nation_recno (rleKey is 0x80)
		srcBytePtr = (unsigned char *) &loc_matrix->power_nation_recno;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0x80);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0x80);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save region_id
		srcBytePtr = (unsigned char *) &loc_matrix->region_id;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// save visit_level
		srcBytePtr = (unsigned char *) &loc_matrix->visit_level;
		destBytePtr = (unsigned char *) buffer.p();
		for( i = max_x_loc*max_y_loc; i > 0; --i, srcBytePtr += sizeof(Location), ++destBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, 0xff);
		if( writeLen > cBuffer.size)
			cBuffer.resize(writeLen+16);
		writeLen = rle.compress( (unsigned char *)buffer.p(), max_x_loc*max_y_loc, (unsigned char *)cBuffer.p(), 0xff);
		filePtr->file_put_long(writeLen);
		if( !filePtr->file_write( cBuffer.p(), writeLen ) )
			return 0;

		// c00 - c03 can be generated,
		// min_y, max_y can be generated
	}

	if( writeDecompressedMatrix )
	{
		// save for comparision
		if( !filePtr->file_write(loc_matrix, max_x_loc*max_y_loc*sizeof(Location)) )
			return 0;
		if( !filePtr->file_write(corner_matrix, (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner)) )
			return 0;
	}

	//--------- save vars -----------//

	filePtr->file_put_short(scan_fire_x);
	filePtr->file_put_short(scan_fire_y);
	filePtr->file_put_short(lightning_signal);
	filePtr->file_put_long(plant_count);
	filePtr->file_put_long(plant_limit);

	map_matrix->last_map_mode = -1;

	filePtr->file_put_short(map_matrix->map_mode);
	filePtr->file_put_short(map_matrix->power_mode);

	filePtr->file_put_long(map_matrix->cur_x_loc);
	filePtr->file_put_long(map_matrix->cur_y_loc);

	filePtr->file_put_long(zoom_matrix->init_lightning);
	filePtr->file_put_long(zoom_matrix->vibration);
	filePtr->file_put_short(zoom_matrix->lightning_x1);
	filePtr->file_put_short(zoom_matrix->lightning_y1);
	filePtr->file_put_short(zoom_matrix->lightning_x2);
	filePtr->file_put_short(zoom_matrix->lightning_y2);

	return 1;
}
//--------- End of function World::write_file ---------------//


//-------- Start of function World::read_file -------------//
//
int World::read_file(File* filePtr)
{
	// ------- read map size --------//

	max_x_loc = filePtr->file_get_short();
	max_y_loc = filePtr->file_get_short();
	// ######### begin Gilbert 11/11 ########//
	gen_map_random_seed = filePtr->file_get_long();
	// ######### end Gilbert 11/11 ########//

	//-------- read in the map --------//

//	loc_matrix = (Location*) mem_resize( loc_matrix, max_x_loc * max_y_loc
//					  * sizeof(Location) );
//	if( !filePtr->file_read(loc_matrix, max_x_loc*max_y_loc*sizeof(Location) ) )
//		return 0;
//	corner_matrix = (Corner*) mem_resize( corner_matrix, 
//		(max_x_loc+1)*(max_y_loc+1)*sizeof(Corner) );
//	if( !filePtr->file_read(corner_matrix, (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner) ) )
//		return 0;

	int writeCompressedMatrix = filePtr->file_get_long();
	int writeDecompressedMatrix = filePtr->file_get_long();

	err_when( !writeDecompressedMatrix && !writeCompressedMatrix );
	// must not be both false

	if( writeCompressedMatrix )
	{
		generate_map(1);

		Rle rle;
		Blob buffer(max_x_loc*max_y_loc);
		Blob cBuffer(max_x_loc*max_y_loc + max_x_loc + max_y_loc );
		long readLen;
		long i;

		register unsigned char *srcBytePtr, *destBytePtr;

		// load low byte of loc_flag
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->loc_flag;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load high byte of loc_flag
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		err_when( sizeof(loc_matrix->loc_flag) != 2 );
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = 1 + (unsigned char *) &loc_matrix->loc_flag;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// terrain_id can be generated

		// load low byte of road_terrain_id
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->road_terrain_id;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load high byte of road_terrain_id
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		err_when( sizeof(loc_matrix->road_terrain_id) != 2 );
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = 1 + (unsigned char *) &loc_matrix->road_terrain_id;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load low byte of cargo_recno
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->cargo_recno;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load high byte of cargo_recno
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		err_when( sizeof(loc_matrix->cargo_recno) != 2 );
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = 1 + (unsigned char *) &loc_matrix->cargo_recno;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load low byte of air_cargo_recno
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->air_cargo_recno;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load high byte of air_cargo_recno
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		err_when( sizeof(loc_matrix->air_cargo_recno) != 2 );
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = 1 + (unsigned char *) &loc_matrix->air_cargo_recno;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load extra_para
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->extra_para;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load fire_level (rleKey is 0x80)
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0x80 ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->fire_level;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load flammability (rleKey is 0x80)
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0x80 ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->flammability;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load power_nation_recno (rleKey is 0x80)
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0x80 ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->power_nation_recno;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load region_id
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->region_id;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// load visit_level
		readLen = filePtr->file_get_long();
		if(readLen > cBuffer.size)
			cBuffer.resize(readLen+16);
		if( !filePtr->file_read(cBuffer.p(), readLen) )
			return 0;
		if( rle.expand( (unsigned char *)cBuffer.p(), (unsigned char *)buffer.p(), 0xff ) != max_x_loc * max_y_loc )
			return 0;
		srcBytePtr = (unsigned char *) buffer.p();
		destBytePtr = (unsigned char *) &loc_matrix->visit_level;
		for( i = max_x_loc*max_y_loc; i > 0; --i, destBytePtr += sizeof(Location), ++srcBytePtr )
		{
			*destBytePtr = *srcBytePtr;
		}

		// c00 - c03 can be generated,
		// min_y, max_y can be generated
	}

	if( writeDecompressedMatrix )
	{
		if( writeCompressedMatrix )
		{
			// verify loc_matrix and corner_matrix

			Blob tempLoc( MAX(max_x_loc*max_y_loc*sizeof(Location), (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner)) );
			// save for comparision
			if( filePtr->file_read(tempLoc.p(), max_x_loc*max_y_loc*sizeof(Location)) )
			{
				if( memcmp( tempLoc.p(), loc_matrix, max_x_loc*max_y_loc*sizeof(Location)) )
					return 0;
			}
			if( filePtr->file_read(tempLoc.p(), (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner)) )
			{
				if( memcmp(tempLoc.p(), corner_matrix, (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner)) )
					return 0;
			}
		}
		else
		{
			// load decompressed matrix 
			if( !filePtr->file_read(loc_matrix, max_x_loc*max_y_loc*sizeof(Location)) )
				return 0;
			if( !filePtr->file_read(corner_matrix, (max_x_loc+1)*(max_y_loc+1)*sizeof(Corner)) )
				return 0;
		}
	}

	assign_map();

	//--------- read in vars ----------//

	scan_fire_x 	  = (char) filePtr->file_get_short();
	scan_fire_y 	  = (char) filePtr->file_get_short();
	lightning_signal = (char) filePtr->file_get_short();
	plant_count      = filePtr->file_get_long();
	plant_limit      = filePtr->file_get_long();

	map_matrix->last_map_mode = -1;

	map_matrix->map_mode   = (char) filePtr->file_get_short();
	map_matrix->power_mode = (char) filePtr->file_get_short();

	map_matrix->cur_x_loc = filePtr->file_get_long();
	map_matrix->cur_y_loc = filePtr->file_get_long();

	zoom_matrix->top_x_loc = map_matrix->cur_x_loc;
	zoom_matrix->top_y_loc = map_matrix->cur_y_loc;

	sys.zoom_need_redraw = 1;

	zoom_matrix->init_lightning = filePtr->file_get_long();
	zoom_matrix->vibration = filePtr->file_get_long();
	zoom_matrix->lightning_x1 = filePtr->file_get_short();
	zoom_matrix->lightning_y1 = filePtr->file_get_short();
	zoom_matrix->lightning_x2 = filePtr->file_get_short();
	zoom_matrix->lightning_y2 = filePtr->file_get_short();

	return 1;
}
//--------- End of function World::read_file ---------------//

//***//

//-------- Start of function Tutor::write_file -------------//
//
int Tutor::write_file(File* filePtr)
{
	filePtr->file_put_short(cur_tutor_id);
	filePtr->file_put_short(cur_text_block_id);
	
	// reset current_objective and
	// reset all achieved_objective_array
//	if (cur_tutor_id <= (NO_OF_QUICK_START_TUTORIAL_PER_SPECY*2) && cur_tutor_id > 0)
	if (get_tutorial_type((cur_tutor_id-1)>>1) && cur_tutor_id > 0)
	{
		filePtr->file_put_short(tutorial_type);
		filePtr->file_put_short(text_disable_flag);
		filePtr->file_put_short(current_objective);
	
		for (int i = 0; i < MAX_TUTOR_OBJECTIVE+1; i++)
			filePtr->file_put_short(achieved_objective_array[i]);
	}

	return 1;
}
//--------- End of function Tutor::write_file ---------------//


//-------- Start of function Tutor::read_file -------------//
//
int Tutor::read_file(File* filePtr)
{
	int curTutorId =	filePtr->file_get_short();

	if( curTutorId > 0 )
	{
		tutor.load(curTutorId);		// load() will reset cur_text_block_id

		cur_text_block_id	= filePtr->file_get_short();
		last_text_block_id = 0;
	//	if (cur_tutor_id <= (NO_OF_QUICK_START_TUTORIAL_PER_SPECY*2))
		if (get_tutorial_type((cur_tutor_id-1)>>1))
		{
			tutorial_type = filePtr->file_get_short();
			text_disable_flag = filePtr->file_get_short();
			current_objective = filePtr->file_get_short();
		
		//	for (int i = 0; i < MAX_TUTOR_OBJECTIVE-79+1; i++)
			for (int i = 0; i < MAX_TUTOR_OBJECTIVE+1; i++)
				achieved_objective_array[i] = filePtr->file_get_short();
		}

		// reset state;
		reset_detect_variables();
		achieved_objective_array[0] = 0;
	}
	else
		cur_text_block_id	= filePtr->file_get_short();

	return 1;
}
//--------- End of function Tutor::read_file ---------------//



// -------- begin of function RandomEvent::write_file ---------//
//
int RandomEvent::write_file( File* filePtr )
{
	return filePtr->file_write( this, sizeof(RandomEvent) );
}
// -------- end of function RandomEvent::write_file ---------//


// -------- begin of function RandomEvent::read_file ---------//
//
int RandomEvent::read_file( File* filePtr )
{
	return filePtr->file_read( this, sizeof(RandomEvent) );
}
// -------- end of function RandomEvent::write_file ---------//

// -------- begin of function Plasma::write_file ---------//
//
int Plasma::write_file( File* filePtr )
{
	return (
	filePtr->file_write( this, sizeof(Plasma) ) &&
	filePtr->file_write( matrix, (max_x+1) * (max_y+1) * sizeof(short) ) 
	);

}
// -------- end of function Plasma::write_file ---------//


// -------- begin of function Plasma::read_file ---------//
//
int Plasma::read_file( File* filePtr )
{
	if (!filePtr->file_read( this, sizeof(Plasma) ))
		return 0;
	matrix = (short *) mem_add( (max_x+1)*(max_y+1)*sizeof(short) );

	return (filePtr->file_read( matrix, (max_x+1) * (max_y+1) * sizeof(short) ));
}
// -------- end of function Plasma::write_file ---------//