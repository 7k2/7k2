//Filename    : OTUTOR.CPP
//Description : Class Tutor

#include <OTUTOR.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OSYS.H>
#include <OAUDIO.H>
#include <OBATTLE.H>
#include <OFONT.H>
#include <OMONSRES.H>
#include <OIMGRES.H>
#include <OBUTTON.H>
#include <OBUTT3D.H>
#include <OGFILE.H>
#include <OGAME.H>
#include <OGAMESET.H>
#include <OWORLD.H>
// #include <OWORLDMT.H>
#include <OFILETXT.H>
#include <OCONFIG.H>
#include <OMOUSE.H> 
#include <OFIRM.H> 
#include <OTOWN.H> 
#include <OWORLDMT.H>
#include <ONEWS.H>
#include <OT_TUTOR.H>
#include <OT_GMENU.H>
#include <OPOWER.H>
#include <OOPTMENU.H>
#include <OINGMENU.H>
#include <OBOX.H>
#include <ODATE.H>
#include <stdio.h>

//---------- define constant ------------//

#define TUTOR_DB   		"TUT_LIST.RES"
#define TUTOR_TEXT_RES  "TUT_TEXT.RES"
#define TUTOR_INTRO_RES "TUT_INTR.RES"

//---------- define coordinations ------------//

#define TUTOR_X1 ZOOM_X1
#define TUTOR_Y1 ZOOM_Y1
#define TUTOR_X2 (TUTOR_X1+150)
#define TUTOR_Y2 ZOOM_Y2
#define TUTOR_BAR_X1 (TUTOR_X2+1)
#define TUTOR_BAR_Y1 ZOOM_Y1
#define TUTOR_BAR_X2 ZOOM_X2
#define TUTOR_BAR_Y2 (TUTOR_BAR_Y1+25)

#define TUTOR_BUTTON_X1 ((TUTOR_X2+TUTOR_X1)/2 - 20)
#define TUTOR_BUTTON_Y1 (TUTOR_Y2-100)

//-------- Define static vars ----------//

//static Button button_new_tutor, button_quit_tutor;
static Button button_restart;//, button_prev, button_next;
//static Button button_continue;
static Button3D button_continue;
static Button button_close;
static Button3D button_sample;
static Button3D button_prev, button_next;

static minize_detect_area_x1;
static minize_detect_area_y1;
static minize_detect_area_x2;
static minize_detect_area_y2;
	
static unsigned seed = 35;				// last random number

static unsigned rand_seed()
{
	#define MULTIPLIER      0x015a4e35L
	#define INCREMENT       1
	seed = MULTIPLIER * seed + INCREMENT;
	return seed;
}

//------- Begin of function Tutor::Tutor -----------//

Tutor::Tutor()
{
	memset( this, 0, sizeof(Tutor) );
}
//--------- End of function Tutor::Tutor -----------//


//------- Begin of function Tutor::~Tutor -----------//

Tutor::~Tutor()
{
	deinit();
}
//--------- End of function Tutor::~Tutor -----------//


//---------- Begin of function Tutor::init -----------//
//
// This function must be called after a map is generated.
//
void Tutor::init()
{
	deinit();

	//------- allocate text_block_array ----------//

	text_block_array = (TutorTextBlock*) mem_add( sizeof(TutorTextBlock) * (MAX_TUTOR_TEXT_BLOCK+
		MAX_TUTOR_OBJECTIVE + MAX_TUTOR_OBJECTIVE));

	//----- open tutorial text resource file -------//

	String str;

	str  = DIR_RES;
	str += TUTOR_TEXT_RES;

	res_tutor_text.init(str, 0);  	// 0-don't read all into buffer

/*	//----- open tutorial introduction resource file -------//

	str  = DIR_RES;
	str += TUTOR_INTRO_RES;

	res_tutor_intro.init(str, 0);  	// 0-don't read all into buffer*/

	//------- load database information --------//

	load_tutor_info();

	init_flag=1;
}
//---------- End of function Tutor::init -----------//


//---------- Begin of function Tutor::deinit -----------//

void Tutor::deinit()
{
	if( init_flag )
	{
		// data from tutorial.dbf
		mem_del(tutor_info_array);
      tutor_info_array = NULL;

		mem_del(text_block_array);
		text_block_array = NULL;

		if( tutor_text_buf )
		{
			mem_del(tutor_text_buf);
			tutor_text_buf = NULL;
		}

		if( tutor_intro_buf )
		{
			mem_del(tutor_intro_buf);
			tutor_intro_buf = NULL;
		}

		if( tutor_result_buf )
		{
			mem_del(tutor_result_buf);
			tutor_result_buf = NULL;
		}

		res_tutor_text.deinit();
	//	res_tutor_intro.deinit();

		init_flag=0;
	}
}
//---------- End of function Tutor::deinit -----------//


//------- Begin of function Tutor::load_tutor_info -------//
//
// Read in information of GOD.DBF into memory array
//
void Tutor::load_tutor_info()
{
	TutorRec  *tutorRec;
	TutorInfo *tutorInfo;

	String str;

	str = DIR_RES;
	str += TUTOR_DB;

	Database  dbTutor(str, 1);

	tutor_count      = (short) dbTutor.rec_count();
	tutor_info_array = (TutorInfo*) mem_add( sizeof(TutorInfo)*tutor_count );

	//------ read in tutor information array -------//

	memset( tutor_info_array, 0, sizeof(TutorInfo) * tutor_count );

	for( int i=0 ; i<tutor_count ; i++ )
	{
		tutorRec  = (TutorRec*) dbTutor.read(i+1);
		tutorInfo = tutor_info_array+i;

		m.rtrim_fld( tutorInfo->intro, tutorRec->intro, tutorRec->CODE_LEN );
		m.rtrim_fld( tutorInfo->result, tutorRec->result, tutorRec->CODE_LEN );
		m.rtrim_fld( tutorInfo->code, tutorRec->code, tutorRec->CODE_LEN );
		m.rtrim_fld( tutorInfo->in_box, tutorRec->in_box, tutorRec->CODE_LEN );
		m.rtrim_fld( tutorInfo->des_text , tutorRec->des_text , tutorRec->CODE_LEN  );
		m.rtrim_fld( tutorInfo->des , tutorRec->des , tutorRec->DES_LEN  );
		translate.multi_to_win(tutorInfo->des, tutorInfo->DES_LEN);
	}

	//--- exclude the Fryhtan and Seat of Power tutorials in the demo ---//

	#ifdef DEMO
		tutor_count -= 2;
	#endif
}
//--------- End of function Tutor::load_tutor_info ---------//


//------------ Begin of function Tutor::load -------------//
//
// <int> tutorId - id. of the tutorial
//
void Tutor::load(int tutorId)
{
	new_tutorial = 0;
	cur_tutor_id = tutorId;

	//------- get the tutor msgs from the resource file -------//

	int   dataSize, dataSize2, dataSize3;
	File* filePtr = res_tutor_text.get_file( tutor[tutorId]->code, dataSize);
	if( !filePtr )       // if error getting the tutor resource
	{
		err_here();
		return;
	}

	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream
	if( dataSize > tutor_text_buf_size )
	{
		tutor_text_buf      = mem_resize( tutor_text_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		tutor_text_buf_size = dataSize;
	}


	//-------- read in tutor/help text one by one --------//

	TutorTextBlock* tutorTextBlock = text_block_array;
	char*     textPtr = tutor_text_buf;
	int       readLen, totalReadLen=0;    // total length of text read into the tutor_text_buf
	int		 loopCount=0;
	char*		 tokenStr;

	text_block_count=0;
	intro_text_block_count=0;
	result_text_block_count=0;

	fileTxt.next_line();    // by pass the first two lines of file description
	fileTxt.next_line();

	while( !fileTxt.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		//------ read in the display button code of the tutorial segment -------//

		if( strcmpi( tokenStr, "Button" ) == 0 )
		{
			fileTxt.get_token(1);		// advance the pointer

			tokenStr = fileTxt.get_token(1);

			strncpy( tutorTextBlock->button_code, tokenStr, tutorTextBlock->BUTTON_CODE_LEN );
			tutorTextBlock->button_code[tutorTextBlock->BUTTON_CODE_LEN] = NULL;
			
			tokenStr = fileTxt.get_token(0); // advance the pointer
		}
		else
		{
			tutorTextBlock->button_code[0] = NULL;
		}

		//------ read in the display title of the tutorial segment -------//
	
		tutorTextBlock->objective_no = 0;
		if( strcmpi( tokenStr, "Title" ) == 0 )
		{
			fileTxt.get_token(1);		// advance the pointer
			tutorTextBlock->title_code[0] = NULL;
			while (1)
			{
				tokenStr = fileTxt.get_token(1);
				if ( strcmpi( tokenStr, "**" ) == 0 )
					break;
				else
				if ( strcmpi( tokenStr, "*" ) == 0 )
				{
					tokenStr = fileTxt.get_token(1);
					tutorTextBlock->objective_no = (tokenStr[0] - '0') * 10 + tokenStr[1] - '0';
					break;
				}

				strcat( tutorTextBlock->title_code, " ");		
				strcat( tutorTextBlock->title_code, tokenStr);		
			}
		//	tutorTextBlock->title_code[tutorTextBlock->TITLE_CODE_LEN] = NULL;
		}
		else
		{
			tutorTextBlock->title_code[0] = NULL;
		}


		//------- read in the tutorial text -------//

		readLen = fileTxt.read_paragraph(textPtr, tutor_text_buf_size-totalReadLen);

		tutorTextBlock->text_ptr = textPtr;
		tutorTextBlock->text_len = readLen;

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen>tutor_text_buf_size );

		//----------- next tutor block -------------//

		fileTxt.next_line();      // pass the page break line

		text_block_count++;
		tutorTextBlock++;

		err_when( text_block_count >= MAX_TUTOR_TEXT_BLOCK );
	}

	cur_text_block_id = 1;
	last_text_block_id = 0;

	//------- get the tutor msgs from the resource file -------//
	File* filePtr2 = res_tutor_text.get_file( tutor[tutorId]->intro, dataSize2);
	if( !filePtr2 )       // if error getting the tutor resource
	{
		err_here();
		return;
	}
	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt2( filePtr2, dataSize2 );  // initialize fileTxt with an existing file stream
	if( dataSize2 > tutor_intro_buf_size )
	{
		tutor_intro_buf      = mem_resize( tutor_intro_buf, dataSize2 );       // allocate a buffer larger than we need for the largest size possible
		tutor_intro_buf_size = dataSize2;
	}
	//-------- read in intro text one by one --------//

	textPtr = tutor_intro_buf;
	readLen=0; totalReadLen=0;    // total length of text read into the tutor_text_buf
	loopCount=0;
	tokenStr=NULL;

	while( !fileTxt2.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt2.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		tutorTextBlock->button_code[0] = NULL;
		tutorTextBlock->title_code[0] = NULL;
		
		//------- read in the tutorial text -------//

		readLen = fileTxt2.read_paragraph(textPtr, tutor_intro_buf_size-totalReadLen);

		tutorTextBlock->intro_ptr = textPtr;
		tutorTextBlock->intro_len = readLen;

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen>tutor_intro_buf_size );

		//----------- next tutor block -------------//

		fileTxt2.next_line();      // pass the page break line

		intro_text_block_count++;
		tutorTextBlock++;

		err_when( intro_text_block_count >= MAX_TUTOR_OBJECTIVE );
	}


	//------- get the tutor msgs from the resource file -------//
	File* filePtr3 = res_tutor_text.get_file( tutor[tutorId]->result, dataSize3);
	if( !filePtr3 )       // if error getting the tutor resource
	{
		err_here();
		return;
	}
	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt3( filePtr3, dataSize3 );  // initialize fileTxt with an existing file stream
	if( dataSize3 > tutor_result_buf_size )
	{
		tutor_result_buf      = mem_resize( tutor_result_buf, dataSize3 );       // allocate a buffer larger than we need for the largest size possible
		tutor_result_buf_size = dataSize3;
	}

	//-------- read in result text one by one --------//

	textPtr = tutor_result_buf;
	readLen=0; totalReadLen=0;    // total length of text read into the tutor_text_buf
	loopCount=0;
	tokenStr=NULL;

	while( !fileTxt3.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt3.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		tutorTextBlock->button_code[0] = NULL;
		tutorTextBlock->title_code[0] = NULL;
				
		//------- read in the tutorial text -------//

		readLen = fileTxt3.read_paragraph(textPtr, tutor_result_buf_size-totalReadLen);

		tutorTextBlock->result_ptr = textPtr;
		tutorTextBlock->result_len = readLen;

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen>tutor_result_buf_size );

		//----------- next tutor block -------------//

		fileTxt3.next_line();      // pass the page break line

		result_text_block_count++;
		tutorTextBlock++;

		err_when( result_text_block_count >= MAX_TUTOR_OBJECTIVE );
	}

}
//------------- End of function Tutor::load -------------//


//------------ Begin of function Tutor::get_intro -------------//
//
// <int> tutorId - id. of the tutorial
//
// return: <char*> return the introduction text of the tutorial.
//
char* Tutor::get_intro(int tutorId)
{
/*	//------- get the tutor msgs from the resource file -------//

	int   dataSize;
	File* filePtr = res_tutor_intro.get_file( tutor[tutorId]->code, dataSize);

	if( !filePtr )       // if error getting the tutor resource
	{
		err_here();
		return NULL;
	}

	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream

	if( dataSize > tutor_intro_buf_size )
	{
		tutor_intro_buf      = mem_resize( tutor_intro_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		tutor_intro_buf_size = dataSize;
	}

	// #### begin Gilbert 23/9 #######//
	fileTxt.read_paragraph(tutor_intro_buf, tutor_intro_buf_size);
	// #### end Gilbert 23/9 #######//

	return tutor_intro_buf;*/
	return NULL;
}
//------------- End of function Tutor::get_intro -------------//


//-------- Begin of function Tutor::select_run_tutor --------//
//
// <int> inGameCall - whether it's called from the main menu or from in-game tutorial.
//
void Tutor::select_run_tutor(int inGameCall)
{
	if( inGameCall )
		err_here();
	//	select_tutor(-2);

	//int tutorId = select_tutor(1);
	int tutorId = select_learning_campaign_menu();

	if( tutorId > 0 )
		run(tutorId, inGameCall);

	if( inGameCall )
		err_here();
	//	select_tutor(-1);
}
//--------- End of function Tutor::select_run_tutor ---------//


//---------- Begin of function Tutor::run -----------//

void Tutor::run(int tutorId, int inGameCall)
{
	//--- don't load pre-saved game when selecting a new tutorial in the game ---//

	////// Varables need to be save //////
	// reset finised objectives flag for all objectives in this tutorial
	for (int i = 0; i < MAX_TUTOR_OBJECTIVE+1; i++)
		achieved_objective_array[i] = 0;

	tutorial_type = 0;
	text_disable_flag = 0;
	current_objective = 0;
	////// Varables need to be save //////
	reset_detect_variables();

	if( !inGameCall )
	{
		String str;

		str  = DIR_TUTORIAL;
		str += tutor[tutorId]->code;
		
		// building_size = 1 or 0 means large size building
		if (building_size == 2) 
			str += "S";
		else
			str += "L";
		str += ".TUT";

		if( m.is_file_exist(str) )
		{
			// game_file.load_game(str);
			// ####### begin Gilbert 5/7 #########//
			if( game_file.load_game(NULL, str) == 1 )			// first parameter is the path
			{
				// set end condition text
				for( i = 0; i < game.win_condition_count; i++ )
					game.win_condition[i].update_strings(0);		// 0=win condition
				for( i = 0; i < game.lose_condition_count; i++ )
					game.lose_condition[i].update_strings(1);		// 1=lose condition
			}
			// ####### end Gilbert 5/7 #########//
		}
		else
		{
			str = "TUTORIAL\\STANDARD.TUT";
		//	str = "SAVE\\BANT_001\\AUTO.SAV";
		//	str = "SAVE\\BANTHEGR\\AUTO.SAV";
			if( m.is_file_exist(str) )
			{
				// ####### begin Gilbert 5/7 #########//
				// game_file.load_game(str);
				if( game_file.load_game(NULL, str) == 1 )		// first parameter is the path
				{
					// set end condition text
					for( i = 0; i < game.win_condition_count; i++ )
						game.win_condition[i].update_strings(0);		// 0=win condition
					for( i = 0; i < game.lose_condition_count; i++ )
						game.lose_condition[i].update_strings(1);		// 1=lose condition
				}
				// ####### end Gilbert 5/7 #########//
			}
		}

		//----- set the gaming speed to normal -----//

		sys.set_speed(9, COMMAND_AUTO);
	}

	//--------- load the tutorial text ------------//

	load(tutorId);			

	game.game_mode = GAME_TUTORIAL;
	info.goal_deadline = date.julian( date.year(info.game_date)+config.goal_year_limit,
								date.month(info.game_date),
								date.day(info.game_date) );

	info.total_play_time  = 0;
	new_tutorial = 1;

	//------------------------------------------//

	if( !inGameCall )
	{
		battle.run_loaded();
		game.deinit();
	}
}
//----------- End of function Tutor::run ------------//


//------------ Begin of function Tutor::disp ------------//

void Tutor::disp()
{
//	if (in_game_menu.is_active() || option_menu.is_active() ||
//		power.win_opened || box.opened_flag)
	if (sys.view_mode != MODE_NORMAL && !display_when_view_mode_actived)
		return;

	if (!tutorial_type)
		disp_guideline();
	else
		disp_hints();
}
//------------ End of function Tutor::disp ------------//


//----------- Begin of function Tutor::play_speech ------------//

void Tutor::play_speech(int tutorId, int objectiveRecno)
{
	if( !sys.dir_tutorial[0] )
		return;

	if( cur_speech_wav_id )
		stop_speech();

	//---------------------------------//

	String str;
	str  = DIR_TUTORIAL;
	str += "\\SOUND\\";

	char str2[10] = "H1-XX.WAV";
	str2[0] = tutor[tutorId]->code[0];
	str2[1] = tutor[tutorId]->code[3];
	str2[3] = '0'+ objectiveRecno/10;
	str2[4] = '0'+ objectiveRecno%10;

	strcat(str, str2);

	if( !m.is_file_exist(str) )
		return;

	// ##### begin Gilbert 25/9 ######//
	cur_speech_wav_id = audio.play_long_wav(str, DEF_REL_VOLUME);		// cur_speech_wav_id is the WAV id that is needed for stopping playing of the WAV file
	// ##### end Gilbert 25/9 ######//
}
//------------ End of function Tutor::play_speech ------------//


//----------- Begin of function Tutor::stop_speech ------------//

void Tutor::stop_speech()
{
	audio.stop_long_wav(cur_speech_wav_id);
	cur_speech_wav_id = 0;
}
//------------ End of function Tutor::stop_speech ------------//


//---------- Begin of function Tutor::detect ----------//

int Tutor::detect()
{
/*	if( button_new_tutor.detect() )
	{
   	stop_speech();
		select_run_tutor(1);		// select and run tutorial, 1-called from in-game, not from the main menu
		return 1;
	}

	if( button_quit_tutor.detect() )
	{
		stop_speech();
		game.game_mode = GAME_SINGLE_PLAYER;
		return 1;
	}*/

/*	if( button_restart.detect() )
	{
		cur_text_block_id = 1;
		return 1;
	}*/

	bypass_allow_mouse_click = 1;
	bypass_allow_button_click = 1;
	if( close_box_pressed == 0 && cur_text_block_id > 1 && button_prev.detect() )
	{
		bypass_allow_mouse_click = 0;
		bypass_allow_button_click = 0;
		prev_text_block();
		return 1;
	}
	bypass_allow_mouse_click = 0;
	bypass_allow_button_click = 0;
	

	bypass_allow_mouse_click = 1;
	bypass_allow_button_click = 1;
	if( close_box_pressed == 0 && cur_text_block_id < text_block_count && button_next.detect() )
	{
		bypass_allow_mouse_click = 0;
		bypass_allow_button_click = 0;
		next_text_block();
		return 1;
	}
	bypass_allow_mouse_click = 0;
	bypass_allow_button_click = 0;


	if (continue_box == 1)
	{
		bypass_allow_mouse_click = 1;
		bypass_allow_button_click = 1;
		if( close_box_pressed == 0 && button_continue.detect() )
		{
			bypass_allow_mouse_click = 0;
			bypass_allow_button_click = 0;
			continue_box_pressed = 1;
			return 1;
		}
		bypass_allow_mouse_click = 0;
		bypass_allow_button_click = 0;
	}	

	if (close_box == 1)
	{
		for (int i = cur_text_block_id; i < text_block_count; i++)
		{
			TutorTextBlock* textBlock = text_block_array+i;
			if (textBlock->objective_no != current_objective)
				continue;
				
			break;
		}

		bypass_allow_mouse_click = 1;
		bypass_allow_button_click = 1;
		if( close_box_pressed == 0 && i == text_block_count && button_close.detect() )
		{
			bypass_allow_mouse_click = 0;
			bypass_allow_button_click = 0;
			close_box_pressed = 1;
			return 1;
		}
		bypass_allow_mouse_click = 0;
		bypass_allow_button_click = 0;
	}	

	if (mouse.any_click(minize_detect_area_x1, minize_detect_area_y1,
								minize_detect_area_x2, minize_detect_area_y2, LEFT_BUTTON))
	{
		text_disable_flag = !text_disable_flag;
		return 1;
	}

	return 0;
}
//----------- End of function Tutor::detect ------------//


//---------- Begin of function Tutor::prev_text_block ----------//

void Tutor::prev_text_block()
{
	if (!tutorial_type)
	{
		if( cur_text_block_id > 1 )
			cur_text_block_id--;
	}
	else
	{
		for (int i = cur_text_block_id-2; i >= 0; i--)
		{
			TutorTextBlock* textBlock = text_block_array+i;
			if (textBlock->objective_no != current_objective)
				continue;
			
			cur_text_block_id = i+1;
			break;
		}
	}
}
//----------- End of function Tutor::prev_text_block ------------//


//---------- Begin of function Tutor::next_text_block ----------//

void Tutor::next_text_block()
{
	if (!tutorial_type)
	{
		if( cur_text_block_id < text_block_count )
			cur_text_block_id++;
	}
	else
	{
		for (int i = cur_text_block_id; i < text_block_count; i++)
		{
			TutorTextBlock* textBlock = text_block_array+i;
			if (textBlock->objective_no != current_objective)
				continue;
			
			cur_text_block_id = i+1;
			break;
		}
	}
}
//----------- End of function Tutor::next_text_block ------------//


//---------- Begin of function Tutor::operator[] -----------//

TutorInfo* Tutor::operator[](int recNo)
{
	err_when( recNo < 1 || recNo > tutor_count );

	return tutor_info_array + recNo - 1;
}
//----------- End of function Tutor::operator[] ------------//

//------ Begin of function Tutor::disp_msg ------//
void Tutor::disp_msg(char dispFlag, char darkness, char* gameMsg, ...)
{
	char useFront = 1;
	enum { END_GAME_BOX_WIDTH  = 460,
			 MAX_END_GAME_BOX_HEIGHT = 560 };
	int i, j;
	unsigned level;

	vga.use_back();

	//------- get text substitution arguments -----//

	va_list argPtr;        // the argument list structure

	va_start( argPtr, gameMsg );

	//------- merge the vars ----------//

	char* strBuf = mem_add( strlen(gameMsg)+200 );

	vsprintf( strBuf, gameMsg, argPtr );

	//-----------------------------------------//

	if (!dispFlag)
	{
		font_mid.text_width( strBuf, -1, END_GAME_BOX_WIDTH-20 );

		int textHeight = font_mid.text_height()+50;

		textHeight = min( textHeight, MAX_END_GAME_BOX_HEIGHT );

		int x1 = ZOOM_X1 + (ZOOM_WIDTH-END_GAME_BOX_WIDTH) / 2 -10;
		int x2 = x1 + END_GAME_BOX_WIDTH - 1 +10;
		int y1 = ZOOM_Y1 + (ZOOM_HEIGHT-textHeight) / 2 -20;
		int y2 = y1 + textHeight - 1 +20;

		vga_back.bar_alpha( x1, y1, x2, y2, darkness, V_BLACK );
		
		int barWidth = 15;

		for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //upper
		{
			level = 0;
			for (j = y1-barWidth; j < y1; j ++, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}
		
		for (i = x1 - (barWidth>>2); i <= x2 + (barWidth>>2); i ++) //lower
		{
			level = 0;
			for (j = y2+barWidth; j > y2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}
		
		for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //right
		{
			level = 0;
			for (j = x1-barWidth; j < x1; j ++, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}
		
		for (i = y1 - (barWidth>>2); i <= y2 + (barWidth>>2); i ++) //left
		{
			level = 0;
			for (j = x2+barWidth; j > x2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}
		font_mid.put_paragraph( x1+10, y1+10, x2-10, y2-10 -40, strBuf );
		font_mid.put_paragraph( x1+10, y2-10-20, x2-10, y2-10, 
			text_tutorial.str_click_to_continue() ); // "Click to continue.");
	}
	else
	{
		int barWidth = 15;
		int i, j;
		unsigned level;
		seed = 35;

		font_mid.text_width( strBuf, -1, TUTOR_X2 -20 -TUTOR_X1);
		int textHeight = font_mid.text_height() +30;
		vga_back.bar_alpha( TUTOR_X1, TUTOR_Y1, TUTOR_X2, TUTOR_Y1+10+textHeight, darkness, V_BLACK );
		
		for (i = TUTOR_Y1; i <= TUTOR_Y1+15+textHeight; i ++) //right
		{
			level = 0;
			for (j = TUTOR_X2+barWidth; j > TUTOR_X2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}

		for (i = TUTOR_X1; i <= TUTOR_X2+5; i ++) //right
		{
			level = 0;
			for (j = TUTOR_Y1+10+textHeight+barWidth; j > TUTOR_Y1+10+textHeight; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}
		
	//	vga.d3_panel_down( TUTOR_X1, TUTOR_Y1, TUTOR_X2, TUTOR_Y1+10+textHeight );
		font_mid.center_put_paragraph( TUTOR_X1+10, TUTOR_Y1+5, TUTOR_X2-10, TUTOR_Y1+5+textHeight,
			strBuf, 4, 0, 0);
	}		

	//-----------------------------------------//

	mem_del(strBuf);

	vga.use_front();

	if (!dispFlag)
	{
		vga.flip();
		mouse.wait_press();
	}
}
//------ End of function Tutor::disp_msg ------//

//------------ Begin of function Tutor::disp_guideline ------------//

void Tutor::disp_guideline()
{
	TutorTextBlock* tutorTextBlock;
	vga.use_back();

	if (!text_disable_flag)
	{	

		button_prev.create(TUTOR_X1+2, TUTOR_Y2-50, "CPREV", "CPPREV",1, 0);
		button_next.create(TUTOR_X2-70, TUTOR_Y2-50, "CNEXT", "CPNEXT",1, 0);

		int barWidth = 15;
		int i, j;
		unsigned level;
		seed = 35;
		vga_back.bar_alpha( TUTOR_X1, TUTOR_Y1, TUTOR_X2, TUTOR_Y2, 1, V_BLACK );
		for (i = TUTOR_BAR_Y2; i <= TUTOR_Y2; i ++) //right
		{
			level = 0;
			for (j = TUTOR_X2+barWidth; j > TUTOR_X2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( j, i, j, i, 1, V_BLACK );
				}
			}
		}
		vga_back.bar_alpha( TUTOR_BAR_X1, TUTOR_BAR_Y1, TUTOR_BAR_X2, TUTOR_BAR_Y2, 1, V_BLACK );
		for (i = TUTOR_BAR_X1; i <= TUTOR_BAR_X2; i ++) //lower
		{
			level = 0;
			for (j = TUTOR_BAR_Y2+barWidth; j > TUTOR_BAR_Y2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( i, j, i, j, 1, V_BLACK );
				}
			}
		}
	/*	Vga::opaque_flag = config.opaque_report;
		vga.d3_panel_down( TUTOR_X1, TUTOR_Y1, TUTOR_X2, TUTOR_Y2 );
		Vga::opaque_flag = 0;*/

		tutorTextBlock = text_block_array+cur_text_block_id-1;

		//------- display button bitmap ------//

		int textX2 = TUTOR_X2-10;

		if( tutorTextBlock->button_code[0] )
		{
			button_sample.paint( TUTOR_BUTTON_X1, TUTOR_BUTTON_Y1, 'A', tutorTextBlock->button_code );
			textX2 = TUTOR_BUTTON_X1-16;
		}

		//-------- display title text --------//
		if( tutorTextBlock->title_code[0] )
		{
			font_tut.center_put_paragraph(TUTOR_BAR_X1-10, TUTOR_BAR_Y1+2, TUTOR_BAR_X2-20, TUTOR_BAR_Y2+2,
				tutorTextBlock->title_code, 4, 0, 0);
		}
	}

	//-------- display minize button --------//
	int y1 = TUTOR_Y1;
	// "Learning Campaign Guidlines"
	font_bld.text_width( text_tutorial.str_guidelines(), -1, TUTOR_X2 -20 -TUTOR_X1);
	int textHeight = font_bld.text_height() +10;
	
	vga.d3_panel_down( TUTOR_X1, y1, TUTOR_X2, y1 + textHeight);
	minize_detect_area_x1 = TUTOR_X1;
	minize_detect_area_y1 = y1;
	minize_detect_area_x2 = TUTOR_X2;
	minize_detect_area_y2 = y1 + textHeight + 20;

	font_bld.center_put_paragraph(TUTOR_X1+10, y1, TUTOR_X2-10, y1 + textHeight,
		text_tutorial.str_guidelines() , 4, 0, 0);
	y1 += textHeight + 5;

	if (text_disable_flag)
	{
		vga.use_front();
		return;
	}

	//-------- display tutorial text --------//

	font_tut.put_paragraph( TUTOR_X1+10, y1, TUTOR_X2-10, TUTOR_Y2-10,
		tutorTextBlock->text_ptr, 4 );

//	font_tut.center_put_paragraph( TUTOR_X1+10, TUTOR_Y1+10, TUTOR_X2-10, TUTOR_Y2-10,
//									tutorTextBlock->text_ptr, 4, 0 ,0 );

	//--------- display controls ---------//

	int x=TUTOR_X1+10;
	int y=TUTOR_Y1+22;

	//---- display current text block position ----//

	x += 360;

//	String str;
//	str  = cur_text_block_id;
//	str += " of ";
//	str += text_block_count;

	font_tut.put( TUTOR_BAR_X2 -50, TUTOR_BAR_Y1 +8, 
		text_tutorial.str_task_of_total(cur_text_block_id, text_block_count) );

	x += 60;

	//------- display other controls --------//

//	button_restart.paint_text( x, y, "|<<" );
 
	if( cur_text_block_id > 1 )
//		button_prev.paint_text( x+45, y, " < " );
		button_prev.paint();

	if( cur_text_block_id < text_block_count )
//		button_next.paint_text( x+88, y, " > " );
		button_next.paint();

	vga.use_front();

/*	//------ play speech of the tutorial -------//

	if( last_text_block_id != cur_text_block_id )
	{
		last_text_block_id = cur_text_block_id;
		play_speech();
   }*/
}
//------------ End of function Tutor::disp_guideline ------------//


//------------ Begin of function Tutor::disp_hints ------------//

void Tutor::disp_hints()
{
	int textHeight;
	int y1 = TUTOR_Y1;
	
	if (!current_objective)
		return;

	if (close_box_pressed)
		return;

	vga.use_back();

	TutorTextBlock* tutorTextBlock2 = text_block_array+text_block_count+current_objective-1;

	for (int i = 0; i < MAX_IN_GAME_BOX_PER_OBJECTIVE; i ++)
	{
		int boxWidth = 110;
		int pointerLength = 100;
		int x, y;
		if (!tutor_box[i].base_object_recno_no &&
			tutor_box[i].screen_loc_centre_x == -1 &&
			tutor_box[i].screen_loc_centre_y == -1)
			continue;

		if (tutor_box[i].base_object_recno_no &&
			base_obj_array.is_deleted(tutor_box[i].base_object_recno_no))
			continue;

		font_bld.text_width( tutor_box[i].text, -1, boxWidth-10);
		textHeight = font_bld.text_height() +20;

		if (tutor_box[i].base_object_recno_no &&
			!base_obj_array.is_deleted(tutor_box[i].base_object_recno_no))
		{
			BaseObj *base_obj = base_obj_array[tutor_box[i].base_object_recno_no];
			if (base_obj->cast_to_Unit())
			{
				tutor_box[i].screen_loc_centre_x = world.zoom_matrix->calc_zoom_x(((Unit*)base_obj)->cur_x*LOCATE_WIDTH,
																				((Unit*)base_obj)->cur_y*LOCATE_HEIGHT,
																				0);
				tutor_box[i].screen_loc_centre_y = world.zoom_matrix->calc_zoom_y(((Unit*)base_obj)->cur_x*LOCATE_WIDTH,
																				((Unit*)base_obj)->cur_y*LOCATE_HEIGHT,
																				0);
			}
			else
			if (base_obj->cast_to_Firm())
			{
				tutor_box[i].screen_loc_centre_x = world.zoom_matrix->calc_zoom_x(((Firm*)base_obj)->center_x*LOCATE_WIDTH,
																				((Firm*)base_obj)->center_y*LOCATE_HEIGHT,
																				((Firm*)base_obj)->altitude);
				tutor_box[i].screen_loc_centre_y = world.zoom_matrix->calc_zoom_y(((Firm*)base_obj)->center_x*LOCATE_WIDTH,
																				((Firm*)base_obj)->center_y*LOCATE_HEIGHT,
																				((Firm*)base_obj)->altitude);
			}
			else		
			if (base_obj->cast_to_Town())
			{
				tutor_box[i].screen_loc_centre_x = world.zoom_matrix->calc_zoom_x(((Town*)base_obj)->center_x*LOCATE_WIDTH,
																				((Town*)base_obj)->center_y*LOCATE_HEIGHT,
																				((Town*)base_obj)->altitude);
				tutor_box[i].screen_loc_centre_y = world.zoom_matrix->calc_zoom_y(((Town*)base_obj)->center_x*LOCATE_WIDTH,
																				((Town*)base_obj)->center_y*LOCATE_HEIGHT,
																				((Town*)base_obj)->altitude);
			}
			else
				err_here();

			x = tutor_box[i].screen_loc_centre_x - (boxWidth>>1) + tutor_box[i].pointer_offset_x;
			y = tutor_box[i].screen_loc_centre_y + tutor_box[i].pointer_offset_y;
								
			if ((x + boxWidth) >= ZOOM_X2)
			x = ZOOM_X2 -boxWidth;

			if (x < ZOOM_X1)
				x = ZOOM_X1;

			if (y >= ZOOM_Y2)
				y = ZOOM_Y2 -1;

			if ((y - textHeight) < (ZOOM_Y1 + ((ZOOM_Y2-ZOOM_Y1)>>3)))
				y = textHeight + (ZOOM_Y1 + ((ZOOM_Y2-ZOOM_Y1)>>3));

			if (tutor_box[i].screen_loc_centre_x < ZOOM_X1)
				tutor_box[i].screen_loc_centre_x = ZOOM_X1;
			if (tutor_box[i].screen_loc_centre_x > ZOOM_X2)
				tutor_box[i].screen_loc_centre_x = ZOOM_X2;
			if (tutor_box[i].screen_loc_centre_y < ZOOM_Y1)
				tutor_box[i].screen_loc_centre_y = ZOOM_Y1;
			if (tutor_box[i].screen_loc_centre_y > ZOOM_Y2)
				tutor_box[i].screen_loc_centre_y = ZOOM_Y2;
			
			vga_back.line(x+(boxWidth>>1), y-(textHeight>>1),
				tutor_box[i].screen_loc_centre_x,
				tutor_box[i].screen_loc_centre_y, V_WHITE);
		}		
		else
		{
			x = tutor_box[i].screen_loc_centre_x - (boxWidth>>1) + tutor_box[i].pointer_offset_x;
			y = tutor_box[i].screen_loc_centre_y + tutor_box[i].pointer_offset_y;
			
			if ((x + boxWidth) >= MOUSE_X2)
			x = MOUSE_X2 -boxWidth;

			if (x < MOUSE_X1)
				x = MOUSE_X1;

			if (y >= MOUSE_Y2)
				y = MOUSE_Y2 -1;

			if ((y - textHeight) < MOUSE_Y1)
				y = textHeight + MOUSE_Y1;

			vga_back.line(x+(boxWidth>>1), y-(textHeight>>1),
			tutor_box[i].screen_loc_centre_x,
			tutor_box[i].screen_loc_centre_y, V_WHITE);
		}
		
		vga_back.bar_alpha( x, y -textHeight, x+boxWidth-1, y, 2, V_WHITE );
		
		font_bld.center_put_paragraph( x, y -textHeight, x+boxWidth-1, y,
			tutor_box[i].text, 4, 0 ,0);
	}

	TutorTextBlock* tutorTextBlock = text_block_array+cur_text_block_id-1;

	// if no hints for that objective is found,
	// objective will be display in the hint position
	if (tutorTextBlock->objective_no != current_objective)
	{
		if (strcmp(tutorTextBlock2->intro_ptr,"") != 0)
			disp_msg(1, 3, tutorTextBlock2->intro_ptr);
		return;
	}
		
	//-------- display title text --------//

	if( tutorTextBlock->title_code[0] )
	{		
		// IF NO OBJECTIVE TEXT USE BAR_ALPHA INSTEAD OF D3_PANEL
		if (strcmp(tutorTextBlock2->intro_ptr,"") != 0)
		{
			font_bld.text_width( tutorTextBlock->title_code, -1, TUTOR_X2 -20 -TUTOR_X1);
			textHeight = font_bld.text_height() +20;
	
			vga.d3_panel_down( TUTOR_X1, y1, TUTOR_X2, y1 + textHeight + 20 );

			minize_detect_area_x1 = TUTOR_X1;
			minize_detect_area_y1 = y1;
			minize_detect_area_x2 = TUTOR_X2;
			minize_detect_area_y2 = y1 + textHeight + 20;

			font_bld.center_put_paragraph(TUTOR_X1+10, y1, TUTOR_X2-10, y1 + textHeight,
				tutorTextBlock->title_code, 4);
		}
		else
		{
			font_mid.text_width( tutorTextBlock->title_code, -1, TUTOR_X2 -20 -TUTOR_X1);
			textHeight = font_mid.text_height() +50;
	
			int barWidth = 15;
			int darkness = 1;
			seed = 35;
			vga_back.bar_alpha( TUTOR_X1, y1, TUTOR_X2, y1 + textHeight + 20, darkness, V_BLACK );
			for (i = y1; i <= y1 + textHeight + 20; i ++) //right
			{
				unsigned level = 0;
				for (int j = TUTOR_X2+barWidth; j > TUTOR_X2; j --, level ++)
				{
					rand_seed();
					if ((seed%barWidth) < level)
					{
						vga_back.bar_alpha( j, i, j, i, darkness, V_BLACK );
					}
				}
			}
			minize_detect_area_x1 = TUTOR_X1;
			minize_detect_area_y1 = y1;
			minize_detect_area_x2 = TUTOR_X2;
			minize_detect_area_y2 = y1 + textHeight + 20;

			font_mid.center_put_paragraph(TUTOR_X1+10, y1, TUTOR_X2-10, y1 + textHeight,
				tutorTextBlock->title_code, 4);
		}
		
		//---- display current text block position ----//
		int totalTextBlock = 0;
		int curTextBlock = 0;
		for (int i = 0; i <text_block_count; i++)
		{
			TutorTextBlock* textBlock = text_block_array+i;
			
			if (textBlock->objective_no != current_objective)
				continue;

			totalTextBlock ++;			
			if (textBlock == tutorTextBlock)
				curTextBlock = totalTextBlock;
		}

//		String str;
//		str  = "Page ";
//		str += curTextBlock;
//		str += " of ";
//		str += totalTextBlock;
		font_bld.center_put( TUTOR_X1+10, y1 + textHeight, TUTOR_X2-10, y1 + textHeight +20,
			text_tutorial.str_page_of_total(curTextBlock, totalTextBlock) );
		y1 += textHeight +22;
	}

	if (text_disable_flag)
		return;
	
	//-------- display tutorial text --------//

	
	if (strcmp(tutorTextBlock2->intro_ptr,"") != 0)
	{
		font_bld.text_width( tutorTextBlock->text_ptr, -1, TUTOR_X2 -20 -TUTOR_X1);
		textHeight = font_bld.text_height() +40;
		if( tutorTextBlock->button_code[0] )
			textHeight += 50;

		vga.d3_panel_down( TUTOR_X1, y1, TUTOR_X2, y1+10+textHeight );
		if( tutorTextBlock->button_code[0] )
			textHeight -= 50;

		font_bld.center_put_paragraph( TUTOR_X1+10, y1+5, TUTOR_X2-10, y1+5+textHeight,
			tutorTextBlock->text_ptr, 4, 0, 0);
	}
	else
	{
		font_mid.text_width( tutorTextBlock->text_ptr, -1, TUTOR_X2 -20 -TUTOR_X1);
		textHeight = font_mid.text_height() +60;
		if( tutorTextBlock->button_code[0] )
			textHeight += 50;

		int barWidth = 15;
		int darkness = 1;
		seed = 35;
		vga_back.bar_alpha( TUTOR_X1, y1, TUTOR_X2, y1+10+textHeight, darkness, V_BLACK );
		for (i = y1; i <= y1 + textHeight + 15; i ++) //right
		{
			unsigned level = 0;
			for (int j = TUTOR_X2+barWidth; j > TUTOR_X2; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( j, i, j, i, darkness, V_BLACK );
				}
			}
		}
		for (i = TUTOR_X1; i <= TUTOR_X2+5; i ++) //right
		{
			unsigned level = 0;
			for (int j = y1+10+textHeight+barWidth; j > y1+10+textHeight; j --, level ++)
			{
				rand_seed();
				if ((seed%barWidth) < level)
				{
					vga_back.bar_alpha( i, j, i, j, darkness, V_BLACK );
				}
			}
		}

		if( tutorTextBlock->button_code[0] )
			textHeight -= 50;
	
		font_mid.center_put_paragraph( TUTOR_X1+10, y1+5, TUTOR_X2-10, y1+5+textHeight,
			tutorTextBlock->text_ptr, 4, 0, 0);
	//	font_mid.put_paragraph( TUTOR_X1+10, y1+5, TUTOR_X2-10, y1+5+textHeight,
	//		tutorTextBlock->text_ptr);
	}

	//------- display button bitmap ------//

	if( tutorTextBlock->button_code[0] )
	{
		button_sample.paint( TUTOR_BUTTON_X1, y1+10+textHeight, 'A', tutorTextBlock->button_code );
		textHeight += 50;
	}

	y1 += 10 +textHeight;
	
	//--------- display controls ---------//
	button_prev.create(TUTOR_X1, y1, "CPREV", "CPPREV",1, 0);
	button_next.create(TUTOR_X2-70, y1, "CNEXT", "CPNEXT",1, 0);
		
	button_continue.create(TUTOR_X2-70, y1, "CNEXT", "CPNEXT",1, 0);
//	button_continue.set_font( &font_bld );
//	button_continue.create_text(TUTOR_X1, y1+45, TUTOR_X2, y1+75, text_game_menu.str_continue() ); // "Continue" );

	button_close.set_font( &font_bld );
	if (continue_box)
		button_close.create_text(TUTOR_X1, y1+80, TUTOR_X2, y1+110, text_tutorial.str_close_hints() ); // "Close Hints" );
	else
		button_close.create_text(TUTOR_X1, y1+45, TUTOR_X2, y1+75, text_tutorial.str_close_hints() ); //"Close Hints" );

	for (i = cur_text_block_id-2; i >= 0; i--)
	{
		TutorTextBlock* textBlock = text_block_array+i;
		if (textBlock->objective_no != current_objective)
			continue;
		
		button_prev.paint();
		break;
	}
	
	for (i = cur_text_block_id; i < text_block_count; i++)
	{
		TutorTextBlock* textBlock = text_block_array+i;
		if (textBlock->objective_no != current_objective)
			continue;
		
		button_next.paint();
		break;
	}
		
	if (continue_box)
		button_continue.paint();
	
	if (close_box && i == text_block_count)
		button_close.paint();

	vga.use_front();

/*	//------ play speech of the tutorial -------//

	if( last_text_block_id != cur_text_block_id )
	{
		last_text_block_id = cur_text_block_id;
		play_speech();
   }*/
}
//------------ End of function Tutor::disp_hints ------------//

