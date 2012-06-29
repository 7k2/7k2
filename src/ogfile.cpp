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

//Filename    : OGFILE.CPP
//Description : Object Game file, save game and restore game

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <obox.h>
#include <ostr.h>
#include <odate.h>
#include <omousecr.h>
#include <otalkres.h>
#include <onation.h>
#include <oworld.h>
#include <opower.h>
#include <ogame.h>
#include <oinfo.h>
#include <ogfile.h>
#include <osys.h>
#include <oaudio.h>
#include <omusic.h>
#include <oconfig.h>
#include <ocampgn.h>
#include <ot_gmenu.h>


// -------- define constant ----------//
#define MIN_FREE_SPACE 1000

//-------- Begin of function GameFile::save_game --------//
//
// <char *> pathName    save game path, without '\' at the end
// <char *> fileName    save game filename, null for unchange, use file_name
// <char *> gameDesc		save game description : null for unchange
//
// return : <int> 1 - saved successfully.
//                0 - not saved.
//
// ###### begin Gilbert 26/5 ########//
int GameFile::save_game(const char *path, const char* fileName, const char *gameDesc)
// ###### end Gilbert 26/5 ########//
{
	File   file;
	String errStr;

	power.win_opened=1;				// to disable power.mouse_handler()

	if( fileName )
	{
		if( path && path[0] != '\0' )		// non empty string
		{
			strcpy(file_name, path);
			strcat(file_name, "\\");
			strcat(file_name, fileName);
		}
		else
		{
			strcpy(file_name, fileName);
		}
	}

	// #### begin Gilbert 20/1 ######//
	if( gameDesc )
		strncpy( game_desc, gameDesc, SAVE_GAME_DESC_LEN );
	game_desc[SAVE_GAME_DESC_LEN] = '\0';
	// #### end Gilbert 20/1 ######//

	// ----- test disk space --------//

	int rc = 1;
	char lowDiskSpaceFlag = 0;
#ifndef NO_WINDOWS  // FIXME
	DWORD sectorPerCluster = 0;
	DWORD bytePerSector = 0;
	DWORD freeCluster = 0;
	DWORD totalCluster = 0;
	if( GetDiskFreeSpace( NULL,	// address of root path, NULL means the current root directory
		&sectorPerCluster, &bytePerSector, &freeCluster, &totalCluster))
	{
		DWORD freeSpace = DWORD( (double)freeCluster * sectorPerCluster * bytePerSector / 1024.0);

		if( m.is_file_exist(file_name) )
		{	
			// if overwritting existing file, count the file size of the existing file
			file.file_open(file_name);
			freeSpace += file.file_size() / 1024;		// count the existing space
			file.file_close();
		}
		if( !(rc = freeSpace >= MIN_FREE_SPACE) )
		{
			errStr = text_game_menu.str_out_of_disk_space(); // "Insufficient disk space ! The game is not saved.";
			lowDiskSpaceFlag = 1;
		}
	}
#endif

	// ##### begin Gilbert 2/11 ######//
	if( rc )
	{
		// create directory
		char *lastSlash = NULL;
		char *s = file_name;
		while( (s = strchr( s, '\\')) )
		{
			lastSlash = s;
			++s;		// start again at next character
		}

		if( lastSlash )
		{
			char backupChar = *lastSlash;
			err_when( backupChar != '\\' );
			*lastSlash = '\0';		// truncate to and not including the last '\\'
			m.mkpath( file_name );
			*lastSlash = backupChar;		// res
		}
	}
	// ##### end Gilbert 2/11 ######//

	if( rc )
	{
		rc = file.file_create( file_name, 0, 1 );	// 0=tell File don't handle error itself
																   // 1=allow the writing size and the read size to be different
		if( !rc )
			// errStr = "Error creating saved game file.";
			errStr = text_game_menu.str_save_error_create();
	}

	if( rc )
	{
		save_process();      // process game data before saving the game

		rc = write_game_header(&file);    // write saved game header information

		if( !rc )
			errStr = text_game_menu.str_save_error_header(); // "Error creating saved game header.";

		if( rc )
		{
			rc = write_file(&file);

			if( !rc )
				errStr = text_game_menu.str_save_error_general(); // "Error writing saved game data.";
		}
	}

	file.file_close();

	power.win_opened=0;

	//------- when saving error ---------//

	if( !rc )
	{
		if( !lowDiskSpaceFlag )
			remove( file_name );         // delete the file as it is not complete

		#ifndef DEBUG
			// errStr = "Insufficient disk space ! The game is not saved.";		// use this message for all types of error message in the release version
			errStr = text_game_menu.str_out_of_disk_space();
		#endif

		box.msg( errStr );
	}

	return rc;
}
//--------- End of function GameFile::save_game --------//


//-------- Begin of function GameFile::load_game --------//
//
// <char *> pathName    save game path, without '\' at the end
// <char *> fileName    save game filename, null for unchange, use file_name
//
// return : <int> 1 - loaded successfully.
//                0 - not loaded.
//               -1 - error and partially loaded
//
int GameFile::load_game(const char *path, const char* fileName)
{
	File file;
	int  rc=0;
	char *errMsg = NULL;

	power.win_opened=1;				// to disable power.mouse_handler()

	int oldCursor = mouse_cursor.get_icon();
	mouse_cursor.set_icon( CURSOR_WAITING );

	int powerEnableFlag = power.enable_flag;

	if( fileName )
	{
		if( path && path[0] != '\0' )		// non empty string
		{
			strcpy(file_name, path);
			strcat(file_name, "\\");
			strcat(file_name, fileName);
		}
		else
		{
			strcpy(file_name, fileName);
		}
	}

	rc = 1;
	if( !file.file_open( file_name, 0, 1 ) )   // 0=tell File don't handle error itself
	{
		rc = 0;
		errMsg = text_game_menu.str_load_error_open(); // "Cannot open save game file";
	}

	//-------- read in the GameFile class --------//

   if( rc )
   {
		if( !file.file_read(this, sizeof(GameFile)) )	// read the whole object from the saved game file
		{
			rc = 0;
			errMsg = text_game_menu.str_load_error_header(); // "Cannot read file header";
		}
		if( rc )
		{
			if( !validate_header() )
			{
				rc = 0;
				errMsg = text_game_menu.str_load_error_old(); // "Save game incompatible";
			}
			else
				strcpy( file_name, file.file_name );	// set the file_name to the actual file
		}
	}

	//--------------------------------------------//
																  // 1=allow the writing size and the read size to be different
	if( rc )
	{
		config.terrain_set = terrain_set;
		config.building_size = building_size;
		// config.display_mode_id = display_mode_id;

		game.deinit_all();		// deinit last game first, 1-it is called during loading of a game
		game.init(1);				// init game

		//-------- read in saved game ----------//

		switch( read_file(&file) )
		{
		case 1:
			rc = 1;
			break;

		case -1:
			rc = 0;		// consider cancel load game
			// errMsg = "Incompatible save game";
			errMsg = text_game_menu.str_load_error_old(); // "Save game incompatible";
			break;

		case 0:
		default:
			rc = -1;
			errMsg = text_game_menu.str_load_error_general(); // "Load game error";
		}

		if( rc > 0 )
			load_process();           // process game data after loading the game
	}

	file.file_close();

	power.enable_flag = powerEnableFlag;

	mouse_cursor.restore_icon( oldCursor );

	power.win_opened=0;

	//---------------------------------------//

	switch(rc)   // don't display msg if loaded successfully (rc==1)
	{
		case 0:
		case -1:
			box.msg( errMsg );
			break;
	}

	last_read_success_flag = rc;		// for external functions to read.
	sys.signal_exit_flag = 4;			// 4-means loading the game successfully. set it to exit the parent loop 

	return rc;
}
//--------- End of function GameFile::load_game --------//


//------- Begin of function GameFile::set_file_name -------//
//
// Set the game file name of current save game, called by
// GameFile::save_game().
//
// e.g. ENLI_001.SAV - the first saved game of the group "Enlight Enterprise"
//
// <char *> extension	- file extension e.g. ".SAV", ".SVM", ".SCN"
//
// notice game_file_array should read save game header of the save path first
//
void GameFile::set_file_name(const char *path, const char *extension)
{
	enum { NAME_PREFIX_LEN = 4,    // Maximum 4 characters in name prefix, e.g. "ENLI" for "Enlight Enterprise"
			 NAME_NUMBER_LEN = 3  };

	String str, str2;
	int    i, j;
	char   nameChar;
	const char* baseName;             // the long name which the file name is based on
	char   addStr[] = "0";       // as a small string for adding to the large string

	baseName = (~nation_array)->king_name();

	// #### begin Gilbert 31/10 #####//
	// str = player_profile.save_game_path("");		// save dir "\\"
	if( path && path != '\0' )
	{
		str = path;
		str += "\\";
	}
	else
	{
		str = "";
	}

	//--------- add the group name prfix ----------//

	// for( i=0 ; i<(int) strlen(baseName) && (int) str.len() < NAME_PREFIX_LEN ; i++ )
	for( i=0, j=0; j < NAME_PREFIX_LEN+1; ++i )
	{
		if( (size_t) i < strlen(baseName) && j < NAME_PREFIX_LEN )
		{
			nameChar = m.upper(baseName[i]);

			if( ( nameChar >= 'A' && nameChar <= 'Z' ) ||
				 ( nameChar >= '0' && nameChar <= '9' ) )
			{
				addStr[0] = nameChar;
				str += addStr;
				++j;
			}
		}
		else
		{
			//----- add tailing characters if prefix len < NAME_PREFIX_LEN+1 ---//

			addStr[0] = '_';
			str += addStr;
			++j;
		}
	}

	int strLen = strlen( str );

	// #### end Gilbert 31/10 #####//

	// BUGHERE : game_file_array should open the corresponding directory first

	//---- find the saved game number for this saved game ----//

	int       curNumber, lastNumber=0;
	GameFile* gameFile;

	for( i=1 ; i<=game_file_array.size() ; i++ )
	{
		gameFile = game_file_array[i];

		// if( memcmp(gameFile->file_name, str, NAME_PREFIX_LEN)==0 )
		// if( strnicmp(gameFile->file_name, str, strLen )==0 )
		if( strnicmp(gameFile->file_name, str, strLen )==0 )
		{
			//------------------------------------------------//
			//
			// if there is a free number in the middle of the list
			// (left by being deleted game), use this number.
			//
			//------------------------------------------------//

			// #### begin Gilbert 2/11 #####//
			curNumber = atoi( gameFile->file_name+strLen );      // +1 is to pass the "_" between the name and the number
			// #### end Gilbert 2/11 #####//

			if( curNumber > lastNumber+1 )   // normally, curNumber should be lastNumber+1
				break;

			lastNumber = curNumber;
		}
	}

	//------- add saved game number after the prefix --------//

	str2 = lastNumber+1;    // use the next number after the last number

	for( i=NAME_NUMBER_LEN-str2.len() ; i>0 ; i-- )   // add "0" before the number if the len of the number < NAME_NUMBER_LEN
		str += "0";

	str += str2;
	str += extension;

	//----- copy the string to file_name ------//

	strncpy( file_name, str, MAX_PATH );

	file_name[MAX_PATH] = NULL;
}
//--------- End of function GameFile::set_file_name -------//


//-------- Begin of function GameFile::save_process -------//
//
// Make the game data ready for saving game
//
// Called before saving the game
//
void GameFile::save_process()
{
   //--------- set the total playing time --------//

	info.total_play_time += m.get_time()-info.start_play_time;

   info.start_play_time  = m.get_time();
}
//--------- End of function GameFile::save_process -------//


//-------- Begin of function GameFile::load_process -------//
//
// Make the game data ready after loading game
//
// Called after loading the game
//
void GameFile::load_process()
{
	info.start_play_time = m.get_time();       // the time player start playing the game
	config.disable_ai_flag = 0;

	//-- if the player is in the diplomatic message screen, rebuild the talk choice list --//

	if( sys.view_mode==MODE_NATION && info.nation_report_mode==NATION_REPORT_TALK )
		talk_res.set_talk_choices();

	mouse_cursor.set_frame(0);		// to fix a frame bug with loading game

	// reflect the effect of config.music_flag, config.wav_music_volume
	audio.set_wav_volume(config.sound_effect_volume);
	if( config.music_flag )
	{
		if( music.is_playing() )
		{
			music.change_volume(config.wav_music_volume);
		}
	}
	else
	{
		music.stop();
	}
}
//--------- End of function GameFile::load_process -------//


//------- Begin of function GameFile::write_game_header -------//
//
// Write saved game header info to the saved game file.
//
// Return : <int> 1 - file written successfully
//                0 - not successful
//
int GameFile::write_game_header(File* filePtr)
{
	class_size = sizeof(GameFile);

	Nation* playerNation = ~nation_array;

	strncpy( player_name, playerNation->king_name(), NationArray::CUSTOM_NAME_LEN );
	player_name[NationArray::CUSTOM_NAME_LEN] = NULL;

	race_id 		 = playerNation->race_id;
	nation_color = playerNation->nation_color;
	terrain_set  = config.terrain_set;
	building_size = config.building_size;
	display_mode_id = config.display_mode_id;
	// ##### begin Gilbert 28/3 #########//

	// --- record sys.scenario_cheat_flag -----//

	scenario_cheat_flag = sys.scenario_cheat_flag;

	// --- record in campaign mode so single game mode cannot load campaign game
	// or vice versa
	if( game.is_campaign_mode() )
		campaign_id = game.campaign()->campaign_id;
	else
		campaign_id = 0;
	// ##### end Gilbert 28/3 #########//

	game_date = info.game_date;

#ifndef NO_WINDOWS  // FIXME
	//----- set the file date ------//

	CoFileTimeNow(&file_date);
#endif

	//------- write GameFile to the saved game file -------//

   return filePtr->file_write( this, sizeof(GameFile) );     // write the whole object to the saved game file
}
//--------- End of function GameFile::write_game_header -------//


//--------- Begin of function GameFile::validate_header -------//
int GameFile::validate_header()
{
	return class_size == sizeof(GameFile) && terrain_set > 0 && building_size > 0;
}
//--------- End of function GameFile::validate_header -------//
