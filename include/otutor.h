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

//Filename    : OTUTOR.H
//Description : Header file of object RaceRes

#ifndef __OH
#define __OH

#ifndef __ORESX_H
#include <oresx.h>
#endif

#include <ounit.h>

//------------ Define constant -----------//

#define 	MAX_TUTOR_TEXT_BLOCK	200
#define 	MAX_TUTOR_OBJECTIVE	99
#define	NO_OF_QUICK_START_TUTORIAL_PER_SPECY		1//	6
#define	MAX_TEXT_LENGTH	20
#define	MAX_IN_GAME_BOX_PER_OBJECTIVE	5

//------------ Define struct TutorRec ---------------//

struct TutorRec
{
	enum { CODE_LEN=8, DES_LEN=40 };

	char  code[CODE_LEN];
	char	intro[CODE_LEN];
	char	result[CODE_LEN];
	char	in_box[CODE_LEN];
	char	des_text[CODE_LEN];
	char  des[DES_LEN];
};

//------------- Define struct TutorInfo --------------//

struct TutorInfo
{
	enum { CODE_LEN=8, DES_LEN=40 };

	char  code[CODE_LEN+1];
	char	intro[CODE_LEN+1];
	char	result[CODE_LEN+1];
	char	in_box[CODE_LEN+1];
	char	des_text[CODE_LEN+1];
	char  des[DES_LEN+1];
};

//---------- Define struct TutorTextBlock ------------//

struct TutorTextBlock
{
	enum { BUTTON_CODE_LEN=8 };
	enum { TITLE_CODE_LEN=100 };

	char* text_ptr;          // offset of the help text in the text buffer
	short text_len;          // length of the help text

	char* intro_ptr;          // offset of the help text in the text buffer
	short intro_len;          // length of the help text

	char* result_ptr;          // offset of the help text in the text buffer
	short result_len;          // length of the help text

	char	button_code[BUTTON_CODE_LEN+1];
	char	title_code[TITLE_CODE_LEN+1];
	char	objective_no;
};

struct TutorInGameBox
{
	char text[200];
	short screen_loc_centre_x;
	short screen_loc_centre_y;
	short base_object_recno_no;
	short pointer_offset_y;
	short pointer_offset_x;
};

//----------- Define class Tutor ---------------//

class Tutor
{
public:
	char        init_flag;

	short       tutor_count;
	TutorInfo*  tutor_info_array;

	ResourceIdx res_tutor_text, res_tutor_intro;

	//-------------------------------//

	short				new_tutorial;
	short				building_size;
	short				cur_tutor_id;
	short				cur_text_block_id;
	short				last_text_block_id;
	int				cur_speech_wav_id;
	
	char        	*tutor_text_buf;
	int				tutor_text_buf_size;
	char        	*tutor_intro_buf;
	int				tutor_intro_buf_size;
	char        	*tutor_result_buf;
	int				tutor_result_buf_size;
	
	TutorTextBlock *text_block_array;
	int				text_block_count;
	int				intro_text_block_count;
	int				result_text_block_count;
	
	short				achieved_objective_array[MAX_TUTOR_OBJECTIVE+1];
	short				tutorial_type;
	short				text_disable_flag;
	short				current_objective;

	char				detect_button_name[MAX_TEXT_LENGTH];
	short				detect_button_x1;
	short				detect_button_y1;
	short				detect_button_x2;
	short				detect_button_y2;
	short				detect_button_rc;
	char				allow_detect_button_name[MAX_TEXT_LENGTH];
	short				allow_detect_button_x1;
	short				allow_detect_button_y1;
	short				allow_detect_button_x2;
	short				allow_detect_button_y2;
	short				allow_detect_button_rc;

	short				detect_unit_info_rc;
	short				allow_unit_info_detect_rc;
	short				allow_unit_info_detect_lightness;
	short				not_allow_unit_info_detect_x2;
	short				not_allow_unit_info_detect_y2;
	short				not_allow_info_detect_x1;
	short				not_allow_info_detect_y1;
	short				not_allow_info_detect_x2;
	short				not_allow_info_detect_y2;

	short				should_not_display_report_button_flag;
	short				selectively_detect_flag;
	short				detect_obj_recno;
	short				detect_press_or_release_flag;
	short				detect_left_or_right_flag;
	short				allow_detect_obj_recno;
	short				allow_left_or_right_click;

	short				continue_box;
	short				continue_box_pressed;
	short				close_box;
	short				close_box_pressed;
	short				display_when_view_mode_actived;
	
	int				bypass_allow_mouse_click;
	int				bypass_allow_button_click;

	TutorInGameBox	tutor_box[MAX_IN_GAME_BOX_PER_OBJECTIVE];
	int				detect_talk_message_id;
		
public:
	Tutor();
	~Tutor();

	void        init();
	void        deinit();

	int			select_learning_campaign_menu();
	int			select_tutor(int);
	void			select_run_tutor(int inGameCall);
	void 			run(int tutorId, int inGameCall);
	void			load(int tutorId);

	char* 		get_intro(int tutorId);

	void			disp();
	void			disp_guideline();
	void			disp_hints();

	int			detect();

	void 			prev_text_block();
	void 			next_text_block();

	void			play_speech(int tutorId, int objectiveRecno);
	void			stop_speech();

	int         write_file(File*);
	int         read_file(File*);

	//---------- Quick Start Tutorial -----------//
	int			get_tutorial_type(int campaignId);
	void			load_in_game_box(int tutorId, int objectiveRecno);

	void			update_tutorial_finish_count();
	int			update_tutorial_objective_finish_count();
	void			disp_objective_text(int objestiveRecNo);
	void			disp_result_text(int objestiveRecNo);
	void			disp_msg(char dispFlag, char darkness, char* gameMsg, ...);
	int			detect_button(short x1, short y1, short x2, short y2, const char* buttonName, short buttonRc = -1);
	int			allow_mouse_click(short x, short y, short pressOrRelease = -1, short rightOrLeft = -1);
	int			allow_button_click(short x1, short y1, short x2, short y2, const char* buttonName, short buttonRc = -1);
	int			allow_unit_icon_click(short x1, short y1, UnitInfo *unitInfo, int lightness, short buttonRc = -1);
	int			allow_this_hot_key(int hotKey);
	void			reset_detect_variables();

	void			disable_hero_joining();
	void			disable_message();
	void			enable_message();
	int			message_disable_flag;
	void			disable_random_event();
	void			set_king_immortal();
	void			set_king_not_immortal();
	void			set_fast_build();
	void			disable_fast_build();
	void			set_tech_level_max(int techLevel, int classId = -1);
	void			disable_report_button_display();
	void			enable_report_button_display();
	void			set_view_mode(int viewMode);

	int			custom_detect_view_mode(int viewMode);
	int			custom_detect_talk_message_id(int talkMessageId);

	int			custom_detect_is_unit_selected(int unitRecno);
	int			custom_detect_is_town_selected(int townRecno);
	int			custom_detect_is_firm_selected(int firmRecno, int linkedTownNationRecno = -1);
	int			custom_detect_is_any_unit_selected(int unitRecno, char orderMode, char visible, int rankId, int isCivilian, int isSpy = -1,  int onlyOneSelected = 0);
	int			custom_detect_is_unit_type_selected(int unitId);
	int			custom_detect_is_any_unit_returning_to_town(int townRecno);
	int			custom_detect_is_any_unit_returning_to_firm(int firmRecno);
	int			custom_detect_is_any_unit_attacking_town(int townRecno);
	int			custom_detect_is_any_unit_attacking_firm(int firmRecno);
	int			custom_detect_is_any_town_selected();
	int			custom_search_independent_town(int startTownRecno, char raceId, 
						int popUpLimit, int popLoLimit, int resisUpLimit, int resisLoLimit);
	int			custom_detect_is_firm_type_selected(int firmId, int linkedTownNationRecno = -1);
	int			custom_search_built_firm(int firmId, int linkedTownNationRecno = -1, int checkWorker = 0);
	
	TutorInfo*  operator[](int tutorId);      // pass tutorId  as recno

private:
	void        load_tutor_info();
};

extern Tutor tutor;

//----------------------------------------------------//

#endif
